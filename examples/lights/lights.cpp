#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/lights/lights.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <map>

using namespace mavsdk;
using std::chrono::seconds;

std::map<std::string, uint32_t> read_color_map();
std::shared_ptr<System> get_system(const std::string& connection_url, Mavsdk& mavsdk);
std::pair<std::string, uint32_t> random_color( std::map<std::string, uint32_t> colorMap );

static constexpr uint8_t pixels_per_strip = 8;
static uint8_t num_strips = 4;

void usage(const std::string& bin_name) {
    std::cerr << "Usage : " << bin_name << " <connection_url> [<num_strip>]\n"
              << "Connection URL format should be :\n"
              << " For TCP : tcp://[server_host][:server_port]\n"
              << " For UDP : udp://[bind_host][:bind_port]\n"
              << " For Serial : serial:///path/to/serial/dev[:baudrate]\n"
              << "For example, to connect to the simulator use URL: udp://:14540\n";
}

int main(int argc, char** argv) {
    if (argc != 2 && argc != 3) {
        usage(argv[0]);
        return 1;
    }

    if (argc == 3) {
        num_strips = std::stoi(argv[2]);
    }

    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};
    auto system = get_system(argv[1], mavsdk);

    if (!system) {
        return 1;
    }

    auto lights = Lights{system};

    auto colorMap = read_color_map();

    std::vector<Lights::LightStrip> strips;

    for (int i = 0; i < num_strips; i++) {
        strips.push_back(Lights::LightStrip{.lights = {0, 0, 0, 0, 0, 0, 0, 0}});
    }

    Lights::LightMatrix matrix { .strips = strips };

    while (true) {
        for ( auto& strip : matrix.strips ) {

            auto colorPair = random_color(colorMap);
            auto colorName = colorPair.first;
            auto color = colorPair.second;

            std::vector<uint32_t> colors(pixels_per_strip, color);
            strip.lights = std::move(colors);

            std::cout << "Setting strip to " << colorName << '\n';
        }

        std::cout << std::endl;

        lights.set_matrix(matrix);

        std::this_thread::sleep_for(std::chrono::seconds(1));

        lights.follow_flight_mode(true);

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}

std::map<std::string, uint32_t> read_color_map() {
    std::ifstream cmFile("colormap.txt");
    std::map<std::string, uint32_t> colorMap;

    uint32_t color;
    std::string name;
    std::string cmLine;

    while (cmFile.good()) {
        std::getline(cmFile, cmLine);
        std::stringstream ss(cmLine);
        ss >> std::hex >> color;

        ss.ignore(1, ' ');

        std::getline(ss, name);

        colorMap[name] = color;
    }

    return colorMap;
}

std::shared_ptr<System> get_system(const std::string& connection_url, Mavsdk& mavsdk) {
    mavsdk.set_configuration(Mavsdk::Configuration{1, 135, 1});
    ConnectionResult connection_result = mavsdk.add_any_connection(connection_url);

    if (connection_result != ConnectionResult::Success) {
        std::cerr << "Connection failed: " << connection_result << '\n';
        return nullptr;
    }

    auto system = mavsdk.first_autopilot(3.0);
    if (!system) {
        std::cerr << "Timed out waiting for system\n";
        return nullptr;
    }

    return system.value();
}

std::pair<std::string, uint32_t> random_color( std::map<std::string, uint32_t> colorMap ) {
    auto it = colorMap.begin();
    std::advance(it, rand() % colorMap.size());
    return *it;
}