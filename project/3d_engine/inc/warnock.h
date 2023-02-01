#ifndef __WARNOCK_H__
#define __WARNOCK_H__

#include "wstack.h"
#include "errors.h"



errors_t warnock_subdivide(const window_t &w, const uint16_t bgcolor,
                           void (*set_pixel)(uint16_t, uint16_t, uint16_t));

#endif // __WARNOCK_H__
