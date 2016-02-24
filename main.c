#include <samd21g18a.h>

#include <port.h>
#include <delay.h>
#include <spi.h>

#include "errno.h"
#include "ili9341.h"

#define LED_0_PIN PIN_PA17

#define TFT_PIN_CS PIN_PA15
#define TFT_PIN_DC PIN_PA14
#define TFT_PIN_LED PIN_PA20
#define TFT_PIN_RESET PIN_PA21


static void set_output(const uint8_t pin);
static void setup();
static void setup_display();
static void setup_spi();

struct spi_module spi_bus;
struct ili9341_ctx display_ctx;

int main() {
  setup();
  ili9341_window(&display_ctx, 0, 0, ILI9341_TFTWIDTH, ILI9341_TFTHEIGHT);
  ili9341_blank(&display_ctx, ILI9341_BLACK);
  while (true) {
    ili9341_blank(&display_ctx, ILI9341_RED);
    ili9341_blank(&display_ctx, ILI9341_GREEN);
    ili9341_blank(&display_ctx, ILI9341_BLUE);
  }
}

static void setup() {
  system_clock_init();
  delay_init();
  set_output(LED_0_PIN);
  setup_spi();
  setup_display();
}

static void set_output(const uint8_t pin) {
  struct port_config config_port_pin;
  port_get_config_defaults(&config_port_pin);
  config_port_pin.direction = PORT_PIN_DIR_OUTPUT;
  port_pin_set_config(pin, &config_port_pin);
}

static void setup_display() {
  display_ctx.pin_cs = TFT_PIN_CS;
  display_ctx.pin_dc = TFT_PIN_DC;
  display_ctx.pin_led = TFT_PIN_LED;
  display_ctx.pin_reset = TFT_PIN_RESET;
  display_ctx.host = &spi_bus;
  ili9341_init(&display_ctx);
}

static void setup_spi() {
  struct spi_config host_cfg;
  spi_get_config_defaults(&host_cfg);
  host_cfg.mux_setting = SPI_SIGNAL_MUX_SETTING_C;
  host_cfg.pinmux_pad0 = PINMUX_PA08C_SERCOM0_PAD0; // MOSI
  host_cfg.pinmux_pad1 = PINMUX_PA09C_SERCOM0_PAD1; // SCK
  host_cfg.pinmux_pad2 = PINMUX_PA10C_SERCOM0_PAD2; // MISO
  host_cfg.pinmux_pad3 = PINMUX_UNUSED;
  host_cfg.receiver_enable = false;
  host_cfg.mode_specific.master.baudrate = 4000000;
  enum status_code status = spi_init(&spi_bus, SERCOM0, &host_cfg);
  if (status != STATUS_OK) {
    error(ERRNO_SPI_INIT);
  }
  spi_enable(&spi_bus);
}
