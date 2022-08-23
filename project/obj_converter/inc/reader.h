#ifndef __READER_H__
#define __READER_H__

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>

struct point3_t
{
    float x, y, z;
};

using vector3_t = point3_t;
using color_t = point3_t;

struct surface_t
{
    std::vector<size_t> point_indicies;
    size_t normal_index;
    color_t color;
};

struct model_t
{
    std::vector<point3_t> points;
    std::vector<vector3_t> normals;
    std::vector<surface_t> surfaces;
};



void print_map(const std::string &comment, const std::map<std::string, point3_t> &m);
void print_map(const std::string &comment, const std::map<std::string, model_t> &m);
int read_materials(std::map<std::string, color_t> &materials, std::ifstream &file_mtl);
int read_models(std::map<std::string, model_t> &models, std::ifstream &file_obj,
                const std::map<std::string, color_t> &materials);
int read_lights(std::vector<point3_t> &lights,
                std::ifstream &file_lgt);

#endif // __READER_H__
