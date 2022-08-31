#ifndef __DRAWING_H__
#define __DRAWING_H__

#ifndef DISPLAY_INFORMATION_ONLY
#include <stdint.h>

#include "SPI_TFT_ILI9341.h"

#include "errors.h"
#endif

#define DISPLAY_WIDTH 320
#define DISPLAY_HEIGHT 240
#define DISPLAY_HORIZONTAL_ORIENTATION (DISPLAY_WIDTH / static_cast<float>(DISPLAY_HEIGHT) > 1.)

#define DEFAULT_COLOR White

#define RGB(r,g,b)  (((r&0xF8)<<8)|((g&0xFC)<<3)|((b&0xF8)>>3)) // 16 bits: 5 red | 6 green | 5 blue



#ifndef DISPLAY_INFORMATION_ONLY
errors_t display_init(SPI_TFT_ILI9341 *tft, bool orientation_changed);
void print(const char *str);
void set_pixel(uint16_t x, uint16_t y, uint16_t color);
void display_destroy();

#ifdef BUFFERIZE_OUTPUT
void save_pixel(uint16_t x, uint16_t y, uint16_t color);
void image_display();
#endif // BUFFERIZE_OUTPUT

#endif // DISPLAY_INFORMATION_ONLY

#endif // __DRAWING_H__
