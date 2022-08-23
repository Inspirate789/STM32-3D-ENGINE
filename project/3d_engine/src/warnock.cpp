#include <iostream>
#include <cmath>
#include <limits>

#include "warnock.h"
#include "measuring_memory.h"

using namespace std;

#ifndef M_PI
#define M_PI atanf(1) * 4
#endif

#define OUTSIDE 1
#define ENCOMPARASSING 2
#define OTHER 3

struct line_t
{
    point2_t beg, end;
};



inline float get_azimuth(const point2_t &o, const point2_t &p)
{                                                                                                               CHECK_STACK_TOP;
    return atan2f(p.y - o.y, p.x - o.x);
}

inline float get_angle(const point2_t &o, const point2_t &a, const point2_t &b)
{
    float result = get_azimuth(o, b) - get_azimuth(o, a);

    if (result > M_PI)
        result -= 2 * M_PI;

    if (result < -M_PI)
        result += 2 * M_PI;
                                                                                                                CHECK_STACK_TOP;
    return result;
}

bool is_inside_polygon(const point2_t &point, const polygon_t &polygon)
{
    float sum = get_angle(point, polygon.vertices[polygon.vertices_cnt - 1], polygon.vertices[0]);

    for (uint16_t i = 0; i < polygon.vertices_cnt - 1; ++i)
        sum += get_angle(point, polygon.vertices[i], polygon.vertices[i + 1]);
                                                                                                                CHECK_STACK_TOP;
    return fabsf(sum) > (float)1e-5;
}

inline bool on_segment(const point2_t &p, const point2_t &q, const point2_t &r)
{                                                                                                               CHECK_STACK_TOP;
    if (q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) &&
        q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y))
       return true;

    return false;
}

inline int orientation(const point2_t &p, const point2_t &q, const point2_t &r)
{
    int val = (q.y - p.y) * (r.x - q.x) -
              (q.x - p.x) * (r.y - q.y);                                                                        CHECK_STACK_TOP;

    if (val == 0) return 0;  // collinear
    
    return (val > 0) ? 1 : 2; // clock or counterclock wise
}

bool check_lines_intersect2D(const line_t &line_1, const line_t &line_2)
{
    int o1 = orientation(line_1.beg, line_1.end, line_2.beg);
    int o2 = orientation(line_1.beg, line_1.end, line_2.end);
    int o3 = orientation(line_2.beg, line_2.end, line_1.beg);
    int o4 = orientation(line_2.beg, line_2.end, line_1.end);
                                                                                                                CHECK_STACK_TOP;
    // General case
    if (o1 != o2 && o3 != o4)
        return true;

    // Special Cases: p1, q1 and p2 are collinear and p2 lies on segment p1q1
    if (o1 == 0 && on_segment(line_1.beg, line_2.beg, line_1.end))
        return true;

    // p1, q1 and q2 are collinear and q2 lies on segment p1q1
    if (o2 == 0 && on_segment(line_1.beg, line_2.end, line_1.end))
        return true;

    // p2, q2 and p1 are collinear and p1 lies on segment p2q2
    if (o3 == 0 && on_segment(line_2.beg, line_1.beg, line_2.end))
        return true;

     // p2, q2 and q1 are collinear and q1 lies on segment p2q2
    if (o4 == 0 && on_segment(line_2.beg, line_1.end, line_2.end))
        return true;

    return false;
}

int overlapping(const polygon_t &polygon, const window_t &window)
{
    uint16_t xmin = window.pbeg.x, xmax = window.pend.x - 1;
    uint16_t ymin = window.pbeg.y, ymax = window.pend.y - 1;
    point2_t wpoints[4] = {{xmin, ymin}, {xmin, ymax}, {xmax, ymax}, {xmax, ymin}};
                                                                                                                CHECK_STACK_TOP;
    for (uint16_t i = 0; i < polygon.vertices_cnt; ++i)
        for (int j = 0; j < 4; ++j)
            if (check_lines_intersect2D({polygon.vertices[(i + 1) % polygon.vertices_cnt],
                                         polygon.vertices[i % polygon.vertices_cnt]},
                                        {wpoints[j % 4], wpoints[(j + 1) % 4]}))
                return OTHER;
    
    if (polygon.vertices[0].x >= xmin && polygon.vertices[0].x <= xmax && 
        polygon.vertices[0].y >= ymin && polygon.vertices[0].y <= ymax)
        return OTHER;

    return is_inside_polygon(wpoints[0], polygon) ? ENCOMPARASSING : OUTSIDE;
}

void fill_pixel(const point2_t &point, const polygons_t &polygons, const uint16_t bgcolor,
                void (*set_pixel)(uint16_t, uint16_t, uint16_t))
{                                                                                                               CHECK_STACK_TOP;
    if (!polygons.count)
    {
#ifndef MEASURING_TIME
        set_pixel(point.x, point.y, bgcolor);
#endif

        return;
    }

    uint16_t color = polygons.array[0].color;
    float zmax = get_z(polygons.array[0].eq, point);

    for (uint16_t i = 1; i < polygons.count; ++i)
    {
        float z = get_z(polygons.array[i].eq, point);

        if (z > zmax)
        {
            zmax = z;
            color = polygons.array[i].color;
        }
    }
                                                                                                                CHECK_STACK_TOP;
#ifndef MEASURING_TIME
    set_pixel(point.x, point.y, color);
#endif
}

void fill_window(const window_t &window, const uint16_t color,
                 void (*set_pixel)(uint16_t, uint16_t, uint16_t))
{
    for (uint16_t x = window.pbeg.x; x < window.pend.x; ++x)
        for (uint16_t y = window.pbeg.y; y < window.pend.y; ++y)
        {
#ifndef MEASURING_TIME
            set_pixel(x, y, color);
#endif
        }                                                                                                       CHECK_STACK_TOP;
}

void split_window(wstack_t &stack, const window_t &window, const polygons_t &new_arr)
{
    uint16_t x_split = window.pbeg.x + ((window.pend.x - window.pbeg.x) >> 1);
    uint16_t y_split = window.pbeg.y + ((window.pend.y - window.pbeg.y) >> 1);

    if ((window.pend.x - window.pbeg.x > 1) and (window.pend.y - window.pbeg.y > 1))
    {
        wstack_push(stack, {new_arr, {window.pbeg.x, window.pbeg.y}, {x_split, y_split}});
        wstack_push(stack, {new_arr, {x_split, window.pbeg.y}, {window.pend.x, y_split}});
        wstack_push(stack, {new_arr, {window.pbeg.x, y_split}, {x_split, window.pend.y}});
        wstack_push(stack, {new_arr, {x_split, y_split}, {window.pend.x, window.pend.y}});
    }
    else if (window.pend.x - window.pbeg.x > 1)
    {
        wstack_push(stack, {new_arr, {window.pbeg.x, window.pbeg.y}, {x_split, window.pend.y}});
        wstack_push(stack, {new_arr, {x_split, window.pbeg.y}, {window.pend.x, window.pend.y}});
    }
    else
    {
        wstack_push(stack, {new_arr, {window.pbeg.x, window.pbeg.y}, {window.pend.x, y_split}});
        wstack_push(stack, {new_arr, {window.pbeg.x, y_split}, {window.pend.x, window.pend.y}});
    }                                                                                                           CHECK_STACK_TOP;
}

bool nearest_polygon_exists(polygon_t &nearest_polygon, const window_t &window, const polygons_t &polygons)
{
    point2_t wpoints[4] = {{window.pbeg.x, window.pbeg.y}, 
                           {window.pbeg.x, window.pend.y - 1}, 
                           {window.pend.x - 1, window.pend.y - 1}, 
                           {window.pend.x - 1, window.pbeg.y}};
    uint32_t nearest_indicies[4] = {0};
    float zmax[4];

    for (uint32_t i = 0; i < 4; ++i)
        zmax[i] = get_z(polygons.array[0].eq, wpoints[i]);
    
    for (uint32_t i = 1; i < polygons.count; ++i)
    {
        float z[4];
        
        for (uint32_t j = 0; j < 4; ++j)
        {
            z[j] = get_z(polygons.array[i].eq, wpoints[j]);
            
            if (z[j] - zmax[j] > numeric_limits<float>::epsilon())
            {
                zmax[j] = z[j];
                nearest_indicies[j] = i;
            }
        }
    }
                                                                                                                CHECK_STACK_TOP;
    if (nearest_indicies[0] == nearest_indicies[1] && 
        nearest_indicies[1] == nearest_indicies[2] && 
        nearest_indicies[2] == nearest_indicies[3])
    {
        nearest_polygon = polygons.array[nearest_indicies[0]];

        return true;
    }
    else
        return false;
}

errors_t warnock_subdivide(const window_t &window, const uint16_t bgcolor,
                           void (*set_pixel)(uint16_t, uint16_t, uint16_t))
{                                                                                                               INIT_STACK_TOP;
    wstack_t stack = wstack_init();                                                                             CHECK_STACK_TOP;

    if (!wstack_allocate(stack, window))
    {
        wstack_free(stack);

        return MEMORY_ALLOCATE_ERROR;
    }

    polygon_t nearest_polygon;                                                                                  CHECK_STACK_TOP;

    while (stack.size > 0)
    {
        window_t wcur;
        wstack_pop(stack, wcur);
        uint32_t end_outside = 0;
        uint32_t start_encomparassing = wcur.polygons.count;
        uint32_t i = 0;                                                                                         CHECK_STACK_TOP;

        while (i < start_encomparassing)
        {
            polygon_t cur_polygon = wcur.polygons.array[i];
            int overlap_flag = overlapping(cur_polygon, wcur);

            if (overlap_flag == OUTSIDE)
                swap(wcur.polygons.array[i++], wcur.polygons.array[end_outside++]);
            else if (overlap_flag == ENCOMPARASSING)
                swap(wcur.polygons.array[i], wcur.polygons.array[--start_encomparassing]);
            else
                ++i;
        }                                                                                                       CHECK_STACK_TOP;
        
        polygons_t new_arr = {wcur.polygons.count - end_outside,
                              wcur.polygons.array + end_outside};                                               CHECK_STACK_TOP;
        
        if ((wcur.pend.x - wcur.pbeg.x == 1) and (wcur.pend.y - wcur.pbeg.y == 1))
            fill_pixel(wcur.pbeg, new_arr, bgcolor, set_pixel);
        else if (start_encomparassing != end_outside)
            split_window(stack, wcur, new_arr);
        else
        {
            if (start_encomparassing == wcur.polygons.count)
                fill_window(wcur, bgcolor, set_pixel);
            else if (nearest_polygon_exists(nearest_polygon, wcur, new_arr))
                fill_window(wcur, nearest_polygon.color, set_pixel);
            else
                split_window(stack, wcur, new_arr);
        }                                                                                                       CHECK_STACK_TOP;
    }

    wstack_free(stack);                                                                                         CHECK_STACK_TOP;
                                                                                                                PRINT_STACK_USAGE;
    return SUCCEDED;
}
