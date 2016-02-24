#include <samd21g18a.h>

#include <delay.h>
#include <port.h>

#include "errno.h"

void error(uint8_t errno) {
  while (true) {
    for (uint8_t i = 0; i < errno; i++) {
      port_pin_set_output_level(ERRNO_PIN, true);
      delay_ms(100);
      port_pin_set_output_level(ERRNO_PIN, false);
      delay_ms(200);
    }
    delay_ms(1000);
  }
}
