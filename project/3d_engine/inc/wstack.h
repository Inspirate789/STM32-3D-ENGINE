#ifndef __WSTACK_H__
#define __WSTACK_H__

#include <cstdlib>
#include <cmath>
#include <algorithm>

#include "data_structures.h"

struct window_t
{
    polygons_t polygons;
    point2_t pbeg, pend;
};

struct wstack_t
{
    size_t size;
    size_t capacity;
    window_t *elems;
};

#include "measuring_memory.h"



inline size_t max_stack_size(const window_t &window)
{
    size_t w = window.pend.x - window.pbeg.x;
    size_t h = window.pend.y - window.pbeg.y;                                                                   CHECK_STACK_TOP;
    
    return trunc(log2(std::max(w, h))) + 2 * (trunc(log2(std::min(w, h))) + 1) + 2;
}

inline wstack_t wstack_init()
{                                                                                                               CHECK_STACK_TOP;
    return {0, 0, nullptr};
}

inline bool wstack_allocate(wstack_t &stack, const window_t &window)
{
    size_t capacity = max_stack_size(window);
    window_t *arr = (window_t *) malloc(capacity);                                                              CHECK_STACK_TOP;

    if (arr)
    {
        arr[0] = window;
        stack = {1, capacity, arr};

        return true;
    }
    else
        return false;
}

inline bool wstack_push(wstack_t &stack, const window_t window)
{                                                                                                               CHECK_STACK_TOP;
    if (stack.size != stack.capacity)
    {
        stack.elems[stack.size++] = window;

        return true;
    }
    else
        return false;
}

inline bool wstack_pop(wstack_t &stack, window_t &window)
{                                                                                                               CHECK_STACK_TOP;
    if (stack.size)
    {
        window = stack.elems[--stack.size];

        return true;
    }
    else
        return false;
}

inline void wstack_free(wstack_t &stack)
{                                                                                                               CHECK_STACK_TOP;
    if (stack.elems)
    {
        stack.size = 0;
        stack.capacity = 0;
        free(stack.elems);
        stack.elems = nullptr;
    }
}

#endif // __WSTACK_H__
