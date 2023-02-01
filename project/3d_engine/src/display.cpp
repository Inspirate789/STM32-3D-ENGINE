#include "display.h"
#include "Arial12x12.h"

static SPI_TFT_ILI9341 *_tft = NULL;

#ifdef BUFFERIZE_OUTPUT
static uint16_t *_bitmap = nullptr;
static uint16_t _display_width;
static uint16_t _display_height;
#endif // BUFFERIZE_OUTPUT

errors_t display_init(SPI_TFT_ILI9341 *tft, bool orientation_changed)
{
    _tft = tft;

#ifdef BUFFERIZE_OUTPUT
    _bitmap = static_cast<uint16_t *>(malloc(DISPLAY_WIDTH * DISPLAY_HEIGHT * sizeof(uint16_t)));
    
    if (!_bitmap)
        return MEMORY_ALLOCATE_ERROR;
    
    _display_width = orientation_changed ? DISPLAY_HEIGHT : DISPLAY_WIDTH;
    _display_height = orientation_changed ? DISPLAY_WIDTH : DISPLAY_HEIGHT;
#endif // BUFFERIZE_OUTPUT

    return SUCCEDED;
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

void display_destroy()
{
#ifdef BUFFERIZE_OUTPUT
    free(_bitmap);
    _bitmap = nullptr;
#endif // BUFFERIZE_OUTPUT
}

#ifdef BUFFERIZE_OUTPUT

void save_pixel(uint16_t x, uint16_t y, uint16_t color)
{
    _bitmap[x * _display_height + y] = color;
}

void image_display()
{
    for (uint16_t x = 0; x < _display_width; ++x)
        for (uint16_t y = 0; y < _display_height; ++y)
            _tft->pixel(x, y, _bitmap[x * _display_height + y]);
}

#endif // BUFFERIZE_OUTPUT
