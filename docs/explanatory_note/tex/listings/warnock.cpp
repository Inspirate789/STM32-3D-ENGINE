errors_t warnock_subdivide(const window_t &window, const uint16_t bgcolor, void (*set_pixel)(uint16_t, uint16_t, uint16_t))
{
    wstack_t stack = wstack_init();

    if (!wstack_allocate(stack, window))
    {
        wstack_free(stack);

        return MEMORY_ALLOCATE_ERROR;
    }

    polygon_t nearest_polygon;

    while (stack.size > 0)
    {
        window_t wcur;
        wstack_pop(stack, wcur);
        uint32_t end_outside = 0;
        uint32_t start_encomparassing = wcur.polygons.count;
        uint32_t i = 0;

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
        }
        
        polygons_t new_arr = {wcur.polygons.count - end_outside, wcur.polygons.array + end_outside};
        
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
        }
    }

    wstack_free(stack);

    return SUCCEDED;
}
