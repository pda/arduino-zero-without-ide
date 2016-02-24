#define ILI9341_TFTWIDTH  240
#define ILI9341_TFTHEIGHT 320

// 16-bit encodings of common colors (RRRRR GGGGGG BBBBB)
#define ILI9341_BLACK       0x0000  //   0,   0,   0
#define ILI9341_NAVY        0x000F  //   0,   0, 128
#define ILI9341_DARKGREEN   0x03E0  //   0, 128,   0
#define ILI9341_DARKCYAN    0x03EF  //   0, 128, 128
#define ILI9341_MAROON      0x7800  // 128,   0,   0
#define ILI9341_PURPLE      0x780F  // 128,   0, 128
#define ILI9341_OLIVE       0x7BE0  // 128, 128,   0
#define ILI9341_LIGHTGREY   0xC618  // 192, 192, 192
#define ILI9341_DARKGREY    0x7BEF  // 128, 128, 128
#define ILI9341_BLUE        0x001F  //   0,   0, 255
#define ILI9341_GREEN       0x07E0  //   0, 255,   0
#define ILI9341_CYAN        0x07FF  //   0, 255, 255
#define ILI9341_RED         0xF800  // 255,   0,   0
#define ILI9341_MAGENTA     0xF81F  // 255,   0, 255
#define ILI9341_YELLOW      0xFFE0  // 255, 255,   0
#define ILI9341_WHITE       0xFFFF  // 255, 255, 255
#define ILI9341_ORANGE      0xFD20  // 255, 165,   0
#define ILI9341_GREENYELLOW 0xAFE5  // 173, 255,  47
#define ILI9341_PINK        0xF81F

struct ili9341_ctx {
  uint8_t pin_cs;      // e.g. PIN_PA15
  uint8_t pin_dc;      // e.g. PIN_PA14
  uint8_t pin_led;     // e.g. PIN_PA20
  uint8_t pin_reset;   // e.g. PIN_PA21
  struct spi_module * host;
  struct spi_slave_inst dev;
  uint16_t window_height;
  uint16_t window_width;
  uint16_t window_x1;
  uint16_t window_x2;
  uint16_t window_y1;
  uint16_t window_y2;
  uint32_t window_pixels;
  uint32_t window_bytes;
};

void ili9341_init(struct ili9341_ctx *);

void ili9341_blank(struct ili9341_ctx *, uint16_t color);

void ili9341_window(struct ili9341_ctx *, uint16_t x, uint16_t y, uint16_t width, uint16_t height);
