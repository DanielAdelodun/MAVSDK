#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/lights/lights.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <map>

#include "colormap.hpp"

using namespace mavsdk;
using std::chrono::seconds;

std::shared_ptr<System> get_system(const std::string& connection_url, Mavsdk& mavsdk);

static constexpr uint8_t pixels_per_strip = 20;
static uint8_t num_strips = 4;

void usage(const std::string& bin_name)
{
    std::cerr << "Usage : " << bin_name << " <connection_url> [<num_strip>]\n"
              << "Connection URL format should be :\n"
              << " For TCP : tcp://[server_host][:server_port]\n"
              << " For UDP : udp://[bind_host][:bind_port]\n"
              << " For Serial : serial:///path/to/serial/dev[:baudrate]\n"
              << "For example, to connect to the simulator use URL: udp://:14540\n";
}

int main(int argc, char** argv)
{
    if (argc != 2 && argc != 3) {
        usage(argv[0]);
        return 1;
    }

    if (argc == 3) {
        num_strips = std::stoi(argv[2]);
    }

    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};

    mavsdk.set_configuration(Mavsdk::Configuration{1, 135, 1}); // 135 = Lights user component ID
    ConnectionResult connection_result = mavsdk.add_any_connection(argv[1]);

    if (connection_result != ConnectionResult::Success) {
        std::cerr << "Connection failed: " << connection_result << '\n';
        return 1;
    }

    auto system = mavsdk.first_autopilot(3.0);
    if (!system) {
        std::cerr << "Timed out waiting for system\n";
        return 1;
    }

    auto lights = Lights{system.value()};

    auto colorMap = read_colormap();

    while (true) {
        Lights::LightMatrix matrix;

        for (uint8_t i = 0; i < num_strips; i++) {
            auto colorPair = random_color(colorMap);
            auto colorName = colorPair.first;
            auto color = colorPair.second;

            std::vector<uint32_t> colors(pixels_per_strip, color);
            matrix.strips.push_back(Lights::LightStrip{.lights = std::move(colors)});

            printf("Strip %02d: %s\n", i, colorName.c_str());
        }

        printf("\n");

        lights.set_matrix(matrix);
        std::this_thread::sleep_for(std::chrono::seconds(5));

        lights.follow_flight_mode(true);
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    return 0;
}
