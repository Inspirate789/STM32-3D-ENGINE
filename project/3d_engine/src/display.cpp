#include "display.h"
#include "Arial12x12.h"

static SPI_TFT_ILI9341 *_tft = NULL;

void display_init(SPI_TFT_ILI9341 *tft)
{
    _tft = tft;
}

void print(const char *str)
{
    _tft->background(0xFFFF);
    _tft->foreground(0);
    _tft->set_font(const_cast<unsigned char *>(Arial12x12));
    _tft->locate(5, 5);
    _tft->printf(str);
}

void set_pixel(uint16_t x, uint16_t y, uint16_t color)
{
    _tft->pixel(x, y, color);
}
