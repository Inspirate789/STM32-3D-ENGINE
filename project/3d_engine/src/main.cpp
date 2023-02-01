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
    bool orientation_changed = IMAGE_HORIZONTAL_ORIENTATION ^ DISPLAY_HORIZONTAL_ORIENTATION;
    tft.set_orientation(IMAGE_HORIZONTAL_ORIENTATION ? 1 : 0);
    uint16_t display_width, display_height;

    if (orientation_changed)
    {
        display_width = DISPLAY_HEIGHT;
        display_height = DISPLAY_WIDTH;
        orientation_changed = true;
    }
    else
    {
        display_width = DISPLAY_WIDTH;
        display_height = DISPLAY_HEIGHT;
    }

    errors_t error_code = display_init(&tft, orientation_changed);

    if (error_code != SUCCEDED)
    {
        handle_error(tft, error_code);
        
        return EXIT_FAILURE;
    }

    polygons_t polygons = {sizeof(data) / sizeof(polygon_t), data};

#ifdef MEASURING_TIME
    volatile uint32_t *DWT_CONTROL = static_cast<uint32_t *>(0xE0001000);
    volatile uint32_t *DWT_CYCCNT = static_cast<uint32_t *>(0xE0001004);
    volatile uint32_t *DEMCR = static_cast<uint32_t *>(0xE000EDFC);
    volatile uint32_t *LAR  = static_cast<uint32_t *>(0xE0001FB0);   // <-- added lock access register

    *DEMCR = *DEMCR | 0x01000000;     // enable trace
    *LAR = 0xC5ACCE55;                // <-- added unlock access to DWT (ITM, etc.) registers 
    *DWT_CONTROL = *DWT_CONTROL | 1;  // enable DWT cycle counter

    for (uint32_t i = 10; i <= polygons.count; i += 10)
    {
        *DWT_CYCCNT = 0;                  // clear DWT cycle counter

        warnock_subdivide({{i, polygons.array}, {0, 0}, {display_width, display_height}},
                          DEFAULT_COLOR,
                          set_pixel);

        uint32_t cycles_count = *DWT_CYCCNT;
        char str[50];
        snprintf(str, 50, "%u polygons : %u cycles\n", i, cycles_count);
        print(str);

        dbg_signal();
    }
#else
#ifdef MEASURING_MEMORY
    for (uint32_t i = 10; i <= polygons.count; i += 10)
    {
        warnock_subdivide({{i, polygons.array}, {0, 0}, {display_width, display_height}},
                          DEFAULT_COLOR, set_pixel);

        dbg_signal();
    }

#else
#ifdef BUFFERIZE_OUTPUT
    error_code = warnock_subdivide({polygons, {0, 0}, {display_width, display_height}},
                                   DEFAULT_COLOR, save_pixel);

    if (error_code == SUCCEDED)
    {
        image_display();
        display_destroy();
    }
    else
    {
        display_destroy();
        handle_error(tft, error_code);
        
        return EXIT_FAILURE;
    }


    dbg_signal();
#else
    error_code = warnock_subdivide({polygons, {0, 0}, {display_width, display_height}},
                                   DEFAULT_COLOR, set_pixel);

    if (error_code != SUCCEDED)
    {
        handle_error(tft, error_code);
        
        return EXIT_FAILURE;
    }

    dbg_signal();
#endif // BUFFERIZE_OUTPUT
#endif // MEASURING_MEMORY
#endif // MEASURING_TIME

display_destroy();

return EXIT_SUCCESS;
}
