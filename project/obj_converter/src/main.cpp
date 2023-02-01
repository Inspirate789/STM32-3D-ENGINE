#include <fstream>
#include <iostream>
#include <map>
#include <algorithm>

#include "reader.h"
#include "converter.h"
#include "writer.h"

#define DEFAULT_LIGHT_POSITION {0., 0., 1e7}

using namespace std;



int check_materials(const string &file_mtl, const map<string, color_t> &materials)
{
    if (!materials.size())
    {
        cout << "There are no valid materials in file " << file_mtl << '.' << endl;
        
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int check_models(const string &file_obj, const map<string, model_t> &models)
{
    if (!models.size())
    {
        cout << "There are no valid models in file " << file_obj << '.' << endl;
        
        return EXIT_FAILURE;
    }
    
    for (auto &model_pair : models)
    {
        if (model_pair.second.points.size() < 4)
        {
            cout << "There are not enough points (";
            cout << model_pair.second.points.size();
            cout << ") in model " << model_pair.first << '.' << endl;
            
            return EXIT_FAILURE;
        }

        if (model_pair.second.surfaces.size() < 3)
        {
            cout << "There are not enough surfaces (";
            cout << model_pair.second.surfaces.size();
            cout << ") in model " << model_pair.first << '.' << endl;
            
            return EXIT_FAILURE;
        }

        for (auto &surface : model_pair.second.surfaces)
        {
            if (surface.point_indicies.size() < 3)
            {
                cout << "There are not enough points (";
                cout << surface.point_indicies.size();
                cout << ") in surface of model " << model_pair.first << '.' << endl;
                
                return EXIT_FAILURE;
            }
        }
    }

    return EXIT_SUCCESS;
}

int check_lights(const string &file_lgt, const vector<point3_t> &lights)
{
    if (!lights.size())
    {
        cout << "There are no valid lights in file " << file_lgt << '.' << endl;
        
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void write_data(const string &file_dst, const string &file_inc, 
                const polygons_t &polygons, bool horizontal_orientation)
{
    ofstream file_h(file_dst, std::ios::binary);
    string filename = file_dst;
    size_t point_index;
    
    while ((point_index = filename.find('.')) != string::npos)
        filename.replace(point_index, 1, 1, '_');
    
    for_each(filename.begin(), filename.end(), [](char &c)
    {
        c = ::toupper(c);
    });

    file_h << "#ifndef __" << filename << "__" << endl;
    file_h << "#define __" << filename << "__" << endl << endl;
    file_h << "#include \"" << file_inc << "\"" << endl << endl;

    if (horizontal_orientation)
        file_h << "#define IMAGE_HORIZONTAL_ORIENTATION true" << endl;
    else
        file_h << "#define IMAGE_HORIZONTAL_ORIENTATION false" << endl;

    file_h << endl << endl << endl;
    write_polygons(file_h, polygons);
    file_h << endl << "#endif" << endl;
}

// argv: *.obj, *.mtl, [*.lgt], *.h (include in destination file), *.h (destination file)
int main(int argc, char **argv)
{
    if (argc < 5 || argc > 6)
    {
        cout << "Invalid arguments count." << endl;
        
        return EXIT_FAILURE;
    }

    ifstream file_mtl(argv[2], ios::in);

    if (!file_mtl.is_open())
    {
        cout << "Cannot open file " << argv[2] << '.' << endl;
        
        return EXIT_FAILURE;
    }

    map<string, color_t> materials;
    int error_code;

    if ((error_code = read_materials(materials, file_mtl)) != EXIT_SUCCESS)
        return error_code;

    if ((error_code = check_materials(argv[2], materials)) != EXIT_SUCCESS)
        return error_code;

    // print_map("Materials: \n", materials);

    ifstream file_obj(argv[1], ios::in);

    if (!file_obj.is_open())
    {
        cout << "Cannot open file " << argv[1] << '.' << endl;
        
        return EXIT_FAILURE;
    }

    map<string, model_t> models;

    if ((error_code = read_models(models, file_obj, materials)) != EXIT_SUCCESS)
        return error_code;

    if ((error_code = check_models(argv[1], models)) != EXIT_SUCCESS)
        return error_code;

    //print_map("Models: \n", models);

    vector<point3_t> lights;

    if (argc == 6)
    {
        ifstream file_lgt(argv[3], ios::in);

        if (!file_lgt.is_open())
        {
            cout << "Cannot open file " << argv[3] << '.' << endl;
            
            return EXIT_FAILURE;
        }

        if ((error_code = read_lights(lights, file_lgt)) != EXIT_SUCCESS)
            return error_code;
    }
    else
        lights.push_back(DEFAULT_LIGHT_POSITION);

    if ((error_code = check_lights(argv[3], lights)) != EXIT_SUCCESS)
        return error_code;
    
    bool horizontal_orientation;
    adjust_data_to_display(models, lights, horizontal_orientation);
    polygons_t polygons = convert_data(models, lights);
    write_data(argv[argc - 1], argv[argc - 2], polygons, horizontal_orientation);
    destroy_polygons(polygons);

    return EXIT_SUCCESS;
}
