#include <cmath>
#include <limits>

#include "converter.h"

using namespace std;



void move_points(map<string, model_t> &models, vector<point3_t> &lights,
                 const vector3_t &vector)
{
    for (auto &model_pair : models)
    {
        for (auto &point : model_pair.second.points)
        {
            point.x += vector.x;
            point.y += vector.y;
            point.z += vector.z;
        }
    }

    for (auto & pos : lights)
    {
        pos.x += vector.x;
        pos.y += vector.y;
        pos.z += vector.z;
    }
}

void scale_points(map<string, model_t> &models, vector<point3_t> &lights,
                  const float coef)
{
    for (auto &model_pair : models)
    {
        for (auto &point : model_pair.second.points)
        {
            point.x *= coef;
            point.y *= coef;
            point.z *= coef;
        }
    }

    for (auto & pos : lights)
    {
        pos.x *= coef;
        pos.y *= coef;
        pos.z *= coef;
    }
}

void adjust_data_to_display(map<string, model_t> &models,
                            vector<point3_t> &lights,
                            bool &horizontal_orientation)
{
    float xmin = 0., xmax = 0., ymin = 0., ymax = 0.;

    for (auto &model_pair : models)
    {
        for (auto &point : model_pair.second.points)
        {
            if (point.x < xmin)
                xmin = point.x;
            
            if (point.x > xmax)
                xmax = point.x;
            
            if (point.y < ymin)
                ymin = point.y;
            
            if (point.y > ymax)
                ymax = point.y;
        }
    }
    
    move_points(models, lights, {-xmin, -ymin, 0.});

    xmax -= xmin;
    ymax -= ymin;
    
    size_t display_width, display_height;
    float display_aspect_ratio = DISPLAY_WIDTH / static_cast<float>(DISPLAY_HEIGHT);
    float image_aspect_ratio = xmax / ymax;

    if (display_aspect_ratio > 1.)
    {
        if (image_aspect_ratio > 1.)
        {
            horizontal_orientation = true;
            display_width = DISPLAY_WIDTH;
            display_height = DISPLAY_HEIGHT;
        }
        else
        {
            horizontal_orientation = false;
            display_width = DISPLAY_HEIGHT;
            display_height = DISPLAY_WIDTH;
        }
    }
    else
    {
        if (image_aspect_ratio > 1.)
        {
            horizontal_orientation = true;
            display_width = DISPLAY_HEIGHT;
            display_height = DISPLAY_WIDTH;
        }
        else
        {
            horizontal_orientation = false;
            display_width = DISPLAY_WIDTH;
            display_height = DISPLAY_HEIGHT;
        }
    }

    float scale_coef_x = display_width / xmax;
    float scale_coef_y = display_height / ymax;
    scale_points(models, lights, min(scale_coef_x, scale_coef_y));
}

inline float scalar_product(const vector3_t &vec1, const vector3_t vec2)
{
    return (vec1.x * vec2.x) + (vec1.y * vec2.y) + (vec1.z * vec2.z);
}

inline float magnitude(const vector3_t &vec)
{
    return (sqrtf(powf(vec.x, 2) + powf(vec.y, 2) + powf(vec.z, 2)));
}

inline float cos_angle(const vector3_t &vec1, const vector3_t vec2)
{
    return scalar_product(vec1, vec2) / (magnitude(vec1) * magnitude(vec2));
}

uint16_t convert_color(const vector<vector3_t> &light_vectors,
                       const vector3_t &normal, const color_t &color)
{

    float luminosity_coef_max = cos_angle(light_vectors[0], normal);
    float luminosity_coef_min = luminosity_coef_max;

    for (size_t i = 1; i < light_vectors.size(); ++i)
    {
        float cur_coef = cos_angle(light_vectors[i], normal);
        luminosity_coef_max = max(luminosity_coef_max, cur_coef);
        luminosity_coef_min = min(luminosity_coef_min, cur_coef);
    }

    if (luminosity_coef_min >= 0.)
        return RGB(0, 0, 0);
    
    luminosity_coef_min = -luminosity_coef_min;

    color_t new_color = {color.x * luminosity_coef_min,
                         color.y * luminosity_coef_min,
                         color.z * luminosity_coef_min};
    
    float saturation_coef = max(new_color.x / 255,
                                max(new_color.y / 255, new_color.z / 255));

    if (saturation_coef > 1.)
        new_color = {new_color.x / saturation_coef,
                     new_color.y / saturation_coef,
                     new_color.z / saturation_coef};
    
    uint8_t r = static_cast<uint8_t>(new_color.x);
    uint8_t g = static_cast<uint8_t>(new_color.y);
    uint8_t b = static_cast<uint8_t>(new_color.z);

    return RGB(r, g, b);
}

point3_t get_center(const model_t &model, const surface_t &surface)
{
    point3_t center = {0., 0., 0.,};

    for (auto &point_index : surface.point_indicies)
    {
        point3_t cur_point = model.points[point_index];

        center.x += cur_point.x;
        center.y += cur_point.y;
        center.z += cur_point.z;
    }

    center = {center.x / surface.point_indicies.size(),
              center.y / surface.point_indicies.size(),
              center.z / surface.point_indicies.size()};

    return center;
}

int solve_slae(vector<float> &res_column, vector<vector<float>> matrix,
               vector<float> free_coefs_column)
{
    size_t rows_cnt = matrix.size();
    size_t columns_cnt = matrix[0].size();

    for (size_t k = 0; k < columns_cnt; k++)
    {
        float max = abs(matrix[k][k]);
        int row_index = k;

        for (size_t i = k + 1; i < rows_cnt; i++)
            if (abs(matrix[i][k]) > max)
            {
                max = abs(matrix[i][k]);
                row_index = i;
            }

        swap(matrix[k], matrix[row_index]);
        swap(free_coefs_column[k], free_coefs_column[row_index]);
        
        for (size_t i = k + 1; i < rows_cnt; i++)
        {
            float c = matrix[i][k] / matrix[k][k];

            for (size_t j = k; j < columns_cnt; j++)
                matrix[i][j] -= c * matrix[k][j];
                
            free_coefs_column[i] -= c * free_coefs_column[k];
        }
    }
    
    if (!(abs(matrix[rows_cnt - 1][columns_cnt - 1]) > numeric_limits<float>::epsilon()))
        return EXIT_FAILURE;

    for (int i = rows_cnt - 1; i >= 0; i--)
    {
        float sum = 0;
        
        for (size_t j = i + 1; j < columns_cnt; j++)
            sum += matrix[i][j] * res_column[j];
        
        res_column[i] = (free_coefs_column[i] - sum) / matrix[i][i];
    }

    return EXIT_SUCCESS; 
}

surface_equation_t get_surface_eq(const vector<point3_t> &points)
{
    surface_equation_t eq = {0., 0., 0.};

    for (size_t i = 0; i < points.size(); ++i)
        for (size_t j = i + 1; j < points.size(); ++j)
            for (size_t k = j + 1; k < points.size(); ++k)
            {
                vector<float> res(3, 0.);
                vector<vector<float>> matrix = \
                    {
                        {points[i].x, points[i].y, points[i].z},
                        {points[j].x, points[j].y, points[j].z},
                        {points[k].x, points[k].y, points[k].z}
                    };
                vector<float> free_coefs(3, -surface_equation_t::d);

                if (solve_slae(res, matrix, free_coefs) == EXIT_SUCCESS)
                    return {res[0], res[1], res[2]};
            }
    
    return eq;
}

polygons_t convert_data(const std::map<std::string, model_t> &models,
                        const vector<point3_t> &lights)
{
    polygons_t polygons = {0, nullptr};

    for (auto &model : models)
        polygons.count += model.second.surfaces.size();
    
    polygons.array = new polygon_t[polygons.count];
    size_t cur_polygon_index = 0;

    for (auto &model_pair : models)
    {
        model_t model = model_pair.second;

        for (auto &surface : model.surfaces)
        {
            polygon_t cur_polygon;
            cur_polygon.vertices_cnt = surface.point_indicies.size();
            cur_polygon.vertices = new point2_t[cur_polygon.vertices_cnt];

            for (size_t i = 0; i < cur_polygon.vertices_cnt; ++i)
                cur_polygon.vertices[i] = \
                    {static_cast<uint16_t>(model.points[surface.point_indicies[i]].x),
                     static_cast<uint16_t>(model.points[surface.point_indicies[i]].y)};
            
            vector<point3_t> cur_points;

            for (auto &point_index : surface.point_indicies)
                cur_points.push_back(model.points[point_index]);

            cur_polygon.eq = get_surface_eq(cur_points);

            point3_t center = get_center(model, surface);

            vector<vector3_t> light_vectors;

            for (auto & pos : lights)
            {
                light_vectors.push_back({center.x - pos.x,
                                         center.y - pos.y,
                                         center.z - pos.z});
            }
            
            cur_polygon.color = convert_color(light_vectors,
                                              model.normals[surface.normal_index],
                                              surface.color);

            polygons.array[cur_polygon_index++] = cur_polygon;
        }
    }

    return polygons;
}

void destroy_polygons(polygons_t &polygons)
{
    for (size_t i = 0; i <  polygons.count; ++i)
        delete [] polygons.array[i].vertices;

    delete [] polygons.array;
    polygons.array = nullptr;
    polygons.count = 0;
}
