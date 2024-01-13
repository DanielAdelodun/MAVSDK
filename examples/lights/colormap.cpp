#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

std::map<std::string, uint32_t> read_colormap(const std::string& cm_file = "colormap.txt")
{
    std::ifstream cmFile(cm_file);
    std::map<std::string, uint32_t> colormap;

    uint32_t color;
    std::string name;
    std::string cmLine;

    while (cmFile.good()) {
        std::getline(cmFile, cmLine);
        std::stringstream ss(cmLine);
        ss >> std::hex >> color;

        ss.ignore(1, ' ');

        std::getline(ss, name);

        colormap[name] = color;
    }

    return colormap;
}

std::pair<std::string, uint32_t> random_color(std::map<std::string, uint32_t> colormap)
{
    auto it = colormap.begin();
    std::advance(it, rand() % colormap.size());
    return *it;
}