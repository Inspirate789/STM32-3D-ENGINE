#ifndef __WRITER_H__
#define __WRITER_H__

#include <fstream>

#include "data_structures.h"



void write_polygons(std::ofstream &file_h, const polygons_t &polygons);

#endif // __WRITER_H__
