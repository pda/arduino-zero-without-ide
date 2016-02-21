#include <samd21g18a.h>
#include <port.h>

#define LED_0_PIN PIN_PA17

void configure_port_pins(void) {
  struct port_config config_port_pin;
  port_get_config_defaults(&config_port_pin);
  config_port_pin.direction = PORT_PIN_DIR_OUTPUT;
  port_pin_set_config(LED_0_PIN, &config_port_pin);
}

int main() {
  configure_port_pins();
  bool pin_state = true;
  while (true) {
    port_pin_set_output_level(LED_0_PIN, pin_state);
    pin_state = !pin_state;
  }
}
