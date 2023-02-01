#ifdef MEASURING_MEMORY
#ifndef __MEASURING_MEMORY_H__
#define __MEASURING_MEMORY_H__

#include "display.h"
#include "wstack.h"

#define INIT_STACK_TOP init_stack_top()
#define CHECK_STACK_TOP check_stack_top()
#define PRINT_STACK_USAGE print_stack_usage()

static volatile unsigned *start_stack_top = 0;
static volatile unsigned *min_stack_top = 0;



inline void init_stack_top()
{
    volatile unsigned *top;
    __asm__ volatile ("mov %[top], sp" : [top] "=r" (top) : : );
    
    min_stack_top = start_stack_top = top;
}

inline void check_stack_top()
{
    volatile unsigned *top;
    __asm__ volatile ("mov %[top], sp" : [top] "=r" (top) : : );
    
    if (top < min_stack_top)
        min_stack_top = top;
}

inline void print_stack_usage()
{
    char str[75];
    snprintf(str, 75, "max stack usage: %u bytes\nmax heap usage: %u bytes\n", 
        start_stack_top - min_stack_top, 
        ((size_t)(ceil(log2(std::max(DISPLAY_WIDTH, DISPLAY_HEIGHT))) + 
        2 * ceil(log2(std::min(DISPLAY_WIDTH, DISPLAY_HEIGHT))) + 1)) * 
        sizeof(window_t));
    print(str);
}

#endif // __MEASURING_MEMORY_H__
#else
#define INIT_STACK_TOP
#define CHECK_STACK_TOP
#define PRINT_STACK_USAGE
#endif // MEASURING_MEMORY
