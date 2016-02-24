#include <samd21g18a.h>

#include <delay.h>
#include <port.h>
#include <spi.h>

#include "errno.h"
#include "ili9341.h"
#include "ili9341_cmd.h"

static void set_output(const uint8_t pin);
static void ili9341_brightness(struct ili9341_ctx *, uint8_t percent);
static void ili9341_cmd(struct ili9341_ctx *, uint8_t cmd);
static void ili9341_reset(struct ili9341_ctx *);
static void ili9341_select(struct ili9341_ctx *, const bool select);
static void ili9341_write16(struct ili9341_ctx *, uint16_t data);

uint16_t retry_ttl;

static const uint8_t init_commands[] = {
  4, ILI9341_UNDOCUMENTED_EF, 0x03, 0x80, 0x02,
  4, ILI9341_POWERB, 0x00, 0XC1, 0X30,
  5, ILI9341_POWER_SEQ, 0x64, 0x03, 0X12, 0X81,
  4, ILI9341_DTCA, 0x85, 0x00, 0x78,
  6, ILI9341_POWERA, 0x39, 0x2C, 0x00, 0x34, 0x02,
  2, ILI9341_PRC, 0x20,
  3, ILI9341_DTCB, 0x00, 0x00,
  2, ILI9341_POWER1, 0x23,
  2, ILI9341_POWER2, 0x10,
  3, ILI9341_VCOM1, 0x3e, 0x28,
  2, ILI9341_VCOM2, 0x86,
  2, ILI9341_MAC, 0x48,
  2, ILI9341_PIXEL_FORMAT, 0x55,
  3, ILI9341_FRMCTR1, 0x00, 0x18,
  4, ILI9341_DFC, 0x08, 0x82, 0x27,
  2, ILI9341_3GAMMA_EN, 0x00,
  2, ILI9341_GAMMA, 0x01,
  16, ILI9341_PGAMMA, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1,
                      0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00,
  16, ILI9341_NGAMMA, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1,
                      0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F,
  0
};


void ili9341_init(struct ili9341_ctx * ctx) {
  set_output(ctx->pin_dc);
  set_output(ctx->pin_led);
  set_output(ctx->pin_reset);

  struct spi_slave_inst_config dev_cfg;
  spi_slave_inst_get_config_defaults(&dev_cfg);
  dev_cfg.ss_pin = ctx->pin_cs;
  spi_attach_slave(&ctx->dev, &dev_cfg);

  ili9341_reset(ctx);
  ili9341_select(ctx, true);

  // thanks https://github.com/PaulStoffregen/ILI9341_t3
  const uint8_t *addr = init_commands;
  while (1) {
    uint8_t count = *addr++;
    if (count == 0) {
      break;
    }
    ili9341_cmd(ctx, *addr++);
    uint8_t args = count - 1;
    spi_write_buffer_wait(ctx->host, addr, args);
    addr += args;
  }

  ili9341_cmd(ctx, ILI9341_SLEEP_OUT);
  delay_ms(100);
  ili9341_cmd(ctx, ILI9341_DISPLAY_ON);

  ili9341_brightness(ctx, 50);

  ili9341_select(ctx, false);
}

void ili9341_window(struct ili9341_ctx * ctx, uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
  ctx->window_height = height;
  ctx->window_width = width;
  ctx->window_x1 = x;
  ctx->window_x2 = x + width - 1;
  ctx->window_y1 = y;
  ctx->window_y2 = y + height - 1;
  ctx->window_pixels = width * height;
  ctx->window_bytes = width * height * 2;
  ili9341_select(ctx, true);
  ili9341_cmd(ctx, ILI9341_COLUMN_ADDR);
  ili9341_write16(ctx, x);
  ili9341_write16(ctx, x + width - 1);
  ili9341_cmd(ctx, ILI9341_PAGE_ADDR);
  ili9341_write16(ctx, y);
  ili9341_write16(ctx, y + height - 1);
  ili9341_select(ctx, false);
}

void ili9341_blank(struct ili9341_ctx * ctx, uint16_t color) {
  ili9341_select(ctx, true);
  uint8_t hi = color >> 8;
  uint8_t lo = color & 0xFF;
  ili9341_cmd(ctx, ILI9341_RAMWR);
  for (uint32_t i = 0; i < ctx->window_pixels; ++i) {
    spi_write(ctx->host, hi);
    spi_write(ctx->host, lo);
  }
  ili9341_select(ctx, false);
}

static void ili9341_select(struct ili9341_ctx * ctx, const bool select) {
  enum status_code status = spi_select_slave(
    ctx->host,
    &ctx->dev,
    select
  );
  switch(status) {
    case STATUS_ERR_UNSUPPORTED_DEV:
      error(ERRNO_SPI_SELECT_UNSUPPORTED);
      break;
    case STATUS_BUSY:
      error(ERRNO_SPI_SELECT_BUSY);
      break;
    default:
      break;
  }
}

static void ili9341_reset(struct ili9341_ctx * ctx) {
  port_pin_set_output_level(ctx->pin_dc, true);
  port_pin_set_output_level(ctx->pin_reset, true);
  delay_ms(1);
  port_pin_set_output_level(ctx->pin_reset, false);
  delay_us(10);
  port_pin_set_output_level(ctx->pin_reset, true);
  delay_ms(5);
}

static void ili9341_brightness(struct ili9341_ctx * ctx, uint8_t percent) {
  // TODO: PWM
  port_pin_set_output_level(ctx->pin_led, percent > 0);
}

static void set_output(const uint8_t pin) {
  struct port_config config_port_pin;
  port_get_config_defaults(&config_port_pin);
  config_port_pin.direction = PORT_PIN_DIR_OUTPUT;
  port_pin_set_config(pin, &config_port_pin);
}

static void ili9341_cmd(struct ili9341_ctx * ctx, uint8_t cmd) {
  port_pin_set_output_level(ctx->pin_dc, false);
  retry_ttl = 0xFFFF;
  while (!spi_is_ready_to_write(ctx->host) && --retry_ttl);
  if (retry_ttl == 0) {
    error(ERRNO_SPI_NOT_READY);
  }
  spi_write(ctx->host, cmd);
  retry_ttl = 0xFFFF;
  while (!spi_is_write_complete(ctx->host) && --retry_ttl);
  if (retry_ttl == 0) {
    error(ERRNO_SPI_WRITE_TIMEOUT);
  }
  port_pin_set_output_level(ctx->pin_dc, true);
}

static void ili9341_write16(struct ili9341_ctx * ctx, uint16_t data) {
  uint16_t buffer = (data << 8) | (data >> 8);
  spi_write_buffer_wait(ctx->host, (const uint8_t *)&buffer, 2);
}
