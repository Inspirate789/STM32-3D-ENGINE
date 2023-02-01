#ifndef __CONVERTER_H__
#define __CONVERTER_H__

#include "reader.h"
#include "data_structures.h"

#define DISPLAY_INFORMATION_ONLY
#include "display.h"

const vector3_t observers_view_vec = {0., 0., -1.};



void adjust_data_to_display(std::map<std::string, model_t> &models,
                            std::vector<point3_t> &light_position,
                            bool &horizontal_orientation);
polygons_t convert_data(const std::map<std::string, model_t> &models,
                        const std::vector<point3_t> &light_position);
void destroy_polygons(polygons_t &polygons);

#endif // __CONVERTER_H__
