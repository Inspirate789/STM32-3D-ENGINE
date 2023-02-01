#include "writer.h"

using namespace std;



void write_points(ofstream &file_h, const polygon_t &polygon, const size_t index)
{
    file_h << "point2_t points_" << index;
    file_h << "[" << polygon.vertices_cnt << "] = {" << endl;
    
    for (int j = 0; j < polygon.vertices_cnt - 1; ++j)
        file_h << "\t (point2_t) {"
                << polygon.vertices[j].x << ','
                << polygon.vertices[j].y << "}," << endl;
    
    file_h << "\t (point2_t) {"
                << polygon.vertices[polygon.vertices_cnt - 1].x << ','
                << polygon.vertices[polygon.vertices_cnt - 1].y << "}";

    file_h << "};" << endl << endl;
}

void write_polygon(ofstream &file_h, const polygon_t &polygon, const size_t index)
{
    file_h << "\t{" << endl;

    file_h << "\t\t" << polygon.vertices_cnt << ',' << endl;

    file_h << "\t\tpoints_" << index << ',' << endl;

    file_h << "\t\t(surface_equation_t) {";
    file_h << polygon.eq.a << ", " << polygon.eq.b << ", " << polygon.eq.c;
    file_h << "}," << endl;

    file_h << "\t\t" << polygon.color << ',' << endl;
    
    file_h << "\t}";
}

void write_polygons(ofstream &file_h, const polygons_t &polygons)
{
    for (size_t i = 0; i < polygons.count; ++i)
        write_points(file_h, polygons.array[i], i);
    
    file_h << endl << endl;

    file_h << "polygon_t data[" << polygons.count << "] = " << endl;
    file_h << "{" << endl;
    
    for (size_t i = 0; i < polygons.count - 1; ++i)
    {
        write_polygon(file_h, polygons.array[i], i);
        file_h << "," << endl;
    }

    write_polygon(file_h, polygons.array[polygons.count - 1], polygons.count - 1);
    file_h << endl;

    file_h << "};" << endl;
}
