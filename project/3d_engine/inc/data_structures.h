#ifndef __DATA_STRUCTURES_H__
#define __DATA_STRUCTURES_H__

#include <stdint.h>

#pragma pack(push, 1)
struct point2_t
{
    uint16_t x, y;
};

struct surface_equation_t
{
    float a, b, c; // a*x + b*y + c*z + d = 0
    static constexpr float d = 1000.;
};

struct polygon_t
{
    uint16_t vertices_cnt;
    point2_t *vertices;
    surface_equation_t eq;
    uint16_t color;
};

struct polygons_t
{
    uint32_t count;
    polygon_t *array;
};
#pragma pack(pop)

inline float get_z(const surface_equation_t &eq, const point2_t &point)
{
    return -(eq.a * point.x + eq.b * point.y + surface_equation_t::d) / eq.c;
}


#endif // __DATA_STRUCTURES_H__
