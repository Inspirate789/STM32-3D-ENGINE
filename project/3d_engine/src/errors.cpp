#include "errors.h"

void handle_error(SPI_TFT_ILI9341 &tft, const errors_t &error_code)
{
    tft.background(Red);
    tft.foreground(0xFFFF);
    tft.cls();
    tft.set_font(const_cast<unsigned char *>(Arial28x28));
    tft.locate(30, 100);

    switch (error_code)
    {
    case MEMORY_ALLOCATE_ERROR:
        tft.printf("MEMORY ALLOCATE ERROR!");
        break;
    
    default:
        break;
    }
}
