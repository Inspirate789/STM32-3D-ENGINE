#ifndef __ERRORS_H__
#define __ERRORS_H__

#include "SPI_TFT_ILI9341.h"
#include "Arial28x28.h"

enum errors_t
{
    SUCCEDED,
    MEMORY_ALLOCATE_ERROR
};

void handle_error(SPI_TFT_ILI9341 &tft, const errors_t &error_code);

#endif // __ERRORS_H__
