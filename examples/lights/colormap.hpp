#pragma once

#include <map>
#include <string>

std::map<std::string, uint32_t> read_colormap(const std::string& cm_file = "colormap.txt");
std::pair<std::string, uint32_t> random_color( std::map<std::string, uint32_t> colormap );
