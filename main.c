#include <samd21g18a.h>

#include <port.h>
#include <delay.h>
#include <spi.h>

#define LED_0_PIN PIN_PA17

#define TFT_PIN_CS PIN_PA15
#define TFT_PIN_DC PIN_PA14
#define TFT_PIN_LED PIN_PA20
#define TFT_PIN_RESET PIN_PA21

#define ERRNO_SPI_INIT 2
#define ERRNO_SPI_NOT_READY 3
#define ERRNO_SPI_SELECT_UNSUPPORTED 4
#define ERRNO_SPI_SELECT_BUSY 5
#define ERRNO_SPI_WRITE_ABORTED 6
#define ERRNO_SPI_WRITE_INVALID_ARG 7
#define ERRNO_SPI_WRITE_TIMEOUT 8

static void setup();
static void set_output(const uint8_t pin);
static void configure_spi();
static void reset_tft();
static void error(uint8_t errno);

struct spi_module spi_bus;
struct spi_slave_inst spi_tft;

int main() {
  setup();

  if (!spi_is_ready_to_write(&spi_bus)) {
    error(ERRNO_SPI_NOT_READY);
  }

  while (true) {

    switch(spi_select_slave(&spi_bus, &spi_tft, true)) {
      case STATUS_ERR_UNSUPPORTED_DEV:
        error(ERRNO_SPI_SELECT_UNSUPPORTED);
        break;
      case STATUS_BUSY:
        error(ERRNO_SPI_SELECT_BUSY);
        break;
      default:
        break;
    }

    while (!spi_is_ready_to_write(&spi_bus));

    port_pin_set_output_level(LED_0_PIN, true);
    const char * greeting = "Hello.SPI";
    enum status_code s = spi_write_buffer_wait(
        &spi_bus,
        (const uint8_t *)greeting,
        strlen(greeting)
    );
    port_pin_set_output_level(LED_0_PIN, false);
    switch (s) {
      case STATUS_ABORTED:
        error(ERRNO_SPI_WRITE_ABORTED);
        break;
      case STATUS_ERR_INVALID_ARG:
        error(ERRNO_SPI_WRITE_INVALID_ARG);
        break;
      case STATUS_ERR_TIMEOUT:
        error(ERRNO_SPI_WRITE_TIMEOUT);
        break;
      default:
        break;
    }

    switch(spi_select_slave(&spi_bus, &spi_tft, false)) {
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

  error(1);

  while (true) {
    port_pin_set_output_level(LED_0_PIN, true);
    delay_ms(100);
    port_pin_set_output_level(LED_0_PIN, false);
    delay_ms(900);
  }
}

static void setup() {
  delay_init();
  set_output(LED_0_PIN);

  set_output(TFT_PIN_DC);
  set_output(TFT_PIN_LED);
  set_output(TFT_PIN_RESET);

  port_pin_set_output_level(TFT_PIN_LED, true);

  configure_spi();
  reset_tft();
}

static void set_output(const uint8_t pin) {
  struct port_config config_port_pin;
  port_get_config_defaults(&config_port_pin);
  config_port_pin.direction = PORT_PIN_DIR_OUTPUT;
  port_pin_set_config(pin, &config_port_pin);
}

static void configure_spi() {
  struct spi_slave_inst_config conf_spi_tft;
  spi_slave_inst_get_config_defaults(&conf_spi_tft);
  conf_spi_tft.ss_pin = TFT_PIN_CS;
  spi_attach_slave(&spi_tft, &conf_spi_tft);

  struct spi_config conf_spi_bus;
  spi_get_config_defaults(&conf_spi_bus);
  conf_spi_bus.mux_setting = SPI_SIGNAL_MUX_SETTING_C;
  conf_spi_bus.pinmux_pad0 = PINMUX_PA08C_SERCOM0_PAD0; // MOSI
  conf_spi_bus.pinmux_pad1 = PINMUX_PA09C_SERCOM0_PAD1; // SCK
  conf_spi_bus.pinmux_pad2 = PINMUX_PA10C_SERCOM0_PAD2; // MISO
  conf_spi_bus.pinmux_pad3 = PINMUX_UNUSED;
  conf_spi_bus.mode_specific.master.baudrate = 10000;

  if (spi_init(&spi_bus, SERCOM0, &conf_spi_bus) != STATUS_OK) {
    error(ERRNO_SPI_INIT);
  }
  spi_enable(&spi_bus);
}

static void reset_tft() {
  port_pin_set_output_level(TFT_PIN_CS, true);
  port_pin_set_output_level(TFT_PIN_RESET, true);
  delay_ms(1);
  port_pin_set_output_level(TFT_PIN_RESET, false);
  delay_us(10);
  port_pin_set_output_level(TFT_PIN_RESET, true);
  delay_ms(5);
}

static void error(uint8_t errno) {
  while (true) {
    for (uint8_t i = 0; i < errno; i++) {
      port_pin_set_output_level(LED_0_PIN, true);
      delay_ms(100);
      port_pin_set_output_level(LED_0_PIN, false);
      delay_ms(200);
    }
    delay_ms(1000);
  }
}
