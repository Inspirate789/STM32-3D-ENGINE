#include <algorithm>

#include "reader.h"

using namespace std;



ostream &operator<<(ostream &os, const point3_t &point)
{
    return os  << '(' << point.x << "; " << point.y << "; " << point.z << ')';
}

ostream &operator<<(ostream &os, const model_t &model)
{
    os << "\tPoints: " << endl << "\t\t";

    for (auto &point : model.points)
        os << point << " ";
    
    cout << endl;

    os << "\tNormals: " << endl << "\t\t";

    for (auto &normal : model.normals)
        os << normal << " ";
    
    cout << endl;

    os << "\tSurfaces: " << endl;

    for (auto &surface : model.surfaces)
    {
        os << "\t\tPoint indicies: ";

        for (auto i : surface.point_indicies)
            os << i << " ";
        
        os << endl;
        os << "\t\tNormal index: " << surface.normal_index << endl;
        os << "\t\tColor: " << surface.color << endl;
        os << endl;
    }
    
    return os << endl;
}

void print_map(const string &comment, const map<string, color_t> &m)
{
    cout << comment;
    
    for (const auto& [key, value] : m)
        cout << '[' << key << "] = " << value << "; ";
    
    cout << endl;
}

void print_map(const string &comment, const map<string, model_t> &m)
{
    cout << comment;
    
    for (const auto& [key, value] : m)
        cout << key << ": " << endl << value;
    
    cout << endl;
}

vector<string> split(const string &str, char seperator)
{
    vector<string> output;
    string::size_type prev_pos = 0, pos = 0;

    while((pos = str.find(seperator, pos)) != string::npos)
    {
        string substring(str.substr(prev_pos, pos - prev_pos));
        output.push_back(substring);
        prev_pos = ++pos;
    }

    output.push_back(str.substr(prev_pos, pos - prev_pos));

    return output;
}

int read_materials(std::map<std::string, color_t> &materials, std::ifstream &file_mtl)
{
    string cur_material_name;
    color_t cur_color = {0., 0., 0.};
    float cur_ns = 1.;
    
    for (string line; getline(file_mtl, line); ) 
    {
        size_t tab_index;

        while ((tab_index = line.find('\t')) != string::npos)
            line.erase(tab_index, 1);

        vector<string> tokens = split(line, ' ');

        if (tokens.size() == 2 && tokens[0] == "newmtl")
        {
            if (cur_material_name != "")
                materials[cur_material_name] = {cur_color.x * cur_ns,
                                                cur_color.y * cur_ns,
                                                cur_color.z * cur_ns};

            cur_material_name = tokens[1];
        }
        
        if (tokens.size() == 2 && tokens[0] == "Ns")
            cur_ns = stof(tokens[1]);
        
        if (tokens.size() == 4 && tokens[0] == "Kd")
            cur_color = {stof(tokens[1]), stof(tokens[2]), stof(tokens[3])};
    }

    if (cur_material_name != "")
        materials[cur_material_name] = {cur_color.x * cur_ns,
                                        cur_color.y * cur_ns,
                                        cur_color.z * cur_ns};
    

    return EXIT_SUCCESS;
}

int read_models(std::map<std::string, model_t> &models, std::ifstream &file_obj,
                const std::map<std::string, color_t> &materials)
{
    string cur_model_name;
    color_t cur_color;
    model_t cur_model;
    size_t line_index = 0;
    
    for (string line; getline(file_obj, line); ) 
    {
        ++line_index;
        size_t str_index;

        while ((str_index = line.find('\t')) != string::npos)
            line.erase(str_index, 1);
        
        vector<string> tokens = split(line, ' ');

        string del_token = "";
        tokens.erase(remove(tokens.begin(), tokens.end(), del_token), tokens.end());

        if (tokens.size() == 2 && tokens[0] == "o")
        {
            if (cur_model_name != "")
                models[cur_model_name] = cur_model;
            
            cur_model_name = tokens[1];
        }
        
        if (tokens.size() == 4 && tokens[0] == "v")
        {
            point3_t point = {stof(tokens[1]), stof(tokens[2]), stof(tokens[3])};
            cur_model.points.push_back(point);
        }
        
        if (tokens.size() == 4 && tokens[0] == "vn")
        {
            vector3_t normal = {stof(tokens[1]), stof(tokens[2]), stof(tokens[3])};
            cur_model.normals.push_back(normal);
        }

        if (tokens.size() >= 4 && tokens[0] == "f")
        {
            surface_t surface = {vector<size_t>(), 0, cur_color};
            
            for (size_t i = 1; i < tokens.size(); ++i)
            {
                vector<string> indicies = split(tokens[i], '/');

                if (indicies.size() == 1)
                    continue;

                if (indicies.size() != 3)
                {
                    cout << "Incorrect surface description in line ";
                    cout << line_index << '.' << endl;

                    return EXIT_FAILURE;
                }

                surface.point_indicies.push_back(stoull(indicies[0]) - 1);
                surface.normal_index = stoull(indicies[2]) - 1;
            }
            
            cur_model.surfaces.push_back(surface);
        }

        if (tokens.size() == 2 && tokens[0] == "usemtl")
        {
            auto material_iter = materials.find(tokens[1]);
            
            if (material_iter == materials.end())
            {
                cout << "Use of undefined material " << tokens[1] << "." << endl;

                return EXIT_FAILURE;
            }

            cur_color = material_iter->second;
        }
    }

    if (cur_model_name != "")
        models[cur_model_name] = cur_model;

    return EXIT_SUCCESS;
}

int read_lights(std::vector<point3_t> &lights,
                         std::ifstream &file_lgt)
{
    for (string line; getline(file_lgt, line); ) 
    {
        vector<string> tokens = split(line, ' ');

        if (tokens.size() != 3)
        {
            cout << "Incorrect light positions." << endl;

            return EXIT_FAILURE;
        }

        lights.push_back({stof(tokens[0]), stof(tokens[1]), stof(tokens[2])});
    }
    
    return EXIT_SUCCESS;
}
