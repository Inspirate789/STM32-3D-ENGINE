#ifndef __DRAWING_H__
#define __DRAWING_H__

#ifndef DISPLAY_INFORMATION_ONLY
#include <stdint.h>

#include "SPI_TFT_ILI9341.h"
#endif

#define DISPLAY_WIDTH 320
#define DISPLAY_HEIGHT 240

#define RGB(r,g,b)  (((r&0xF8)<<8)|((g&0xFC)<<3)|((b&0xF8)>>3)) //5 red | 6 green | 5 blue



#ifndef DISPLAY_INFORMATION_ONLY
void display_init(SPI_TFT_ILI9341 *tft);
void print(const char *str);
void set_pixel(uint16_t x, uint16_t y, uint16_t color);
#endif

#endif // __DRAWING_H__
