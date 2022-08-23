#include <cstdio>

#include "mbed.h"

#include "data.h"
#include "debug.h"
#include "display.h"
#include "errors.h"
#include "warnock.h"



int main(void)
{
    SPI_TFT_ILI9341 tft(D11, D12, D13, D8, D9, D10, "TFT");  // (mosi, miso, sck, cs, reset, dc)

#ifdef VERTICAL_ORIENTATION
    tft.set_orientation(0);
    uint16_t display_width, display_height;

    if (DISPLAY_WIDTH / static_cast<float>(DISPLAY_HEIGHT) > 1.)
    {
        display_width = DISPLAY_HEIGHT;
        display_height = DISPLAY_WIDTH;
    }
    else
    {
        display_width = DISPLAY_WIDTH;
        display_height = DISPLAY_HEIGHT;
    }
#else
    tft.set_orientation(1);
    uint16_t display_width, display_height;

    if (DISPLAY_WIDTH / static_cast<float>(DISPLAY_HEIGHT) > 1.)
    {
        display_width = DISPLAY_WIDTH;
        display_height = DISPLAY_HEIGHT;
    }
    else
    {
        display_width = DISPLAY_HEIGHT;
        display_height = DISPLAY_WIDTH;
    }
#endif

    display_init(&tft);
    polygons_t polygons = {sizeof(data) / sizeof(polygon_t), data};

#ifdef MEASURING_TIME
    volatile uint32_t *DWT_CONTROL = (uint32_t *) 0xE0001000;
    volatile uint32_t *DWT_CYCCNT = (uint32_t *) 0xE0001004;
    volatile uint32_t *DEMCR = (uint32_t *) 0xE000EDFC;
    volatile uint32_t *LAR  = (uint32_t *) 0xE0001FB0;   // <-- added lock access register

    *DEMCR = *DEMCR | 0x01000000;     // enable trace
    *LAR = 0xC5ACCE55;                // <-- added unlock access to DWT (ITM, etc.) registers 
    *DWT_CONTROL = *DWT_CONTROL | 1;  // enable DWT cycle counter

    for (uint32_t i = 10; i <= polygons.count; i += 10)
    {
        *DWT_CYCCNT = 0;                  // clear DWT cycle counter

        warnock_subdivide({{i, polygons.array}, {0, 0}, {display_width, display_height}},
                          0b1111111111111111,
                          set_pixel);

        uint32_t cycles_count = *DWT_CYCCNT;
        char str[50];
        snprintf(str, 50, "%lu polygons : %lu cycles\n", i, cycles_count);
        print(str);

        dbg_signal();
    }
#else
#ifdef MEASURING_MEMORY
    for (uint32_t i = 10; i <= polygons.count; i += 10)
    {
        warnock_subdivide({{i, polygons.array}, {0, 0}, {display_width, display_height}},
                          0b1111111111111111,
                          set_pixel);

        dbg_signal();
    }

#else
    errors_t error_code = warnock_subdivide({polygons, {0, 0}, {display_width, display_height}},
                                            0b1111111111111111,
                                            set_pixel);

    if (error_code != SUCCEDED)
        handle_error(tft, error_code);

    dbg_signal();
#endif // MEASURING_MEMORY
#endif // MEASURING_TIME
}
