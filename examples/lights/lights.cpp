#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/lights/lights.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <chrono>
#include <iostream>
#include <thread>

using namespace mavsdk;
using std::chrono::seconds;

void usage(const std::string& bin_name)
{
    std::cerr << "Usage : " << bin_name << " <connection_url>\n"
              << "Connection URL format should be :\n"
              << " For TCP : tcp://[server_host][:server_port]\n"
              << " For UDP : udp://[bind_host][:bind_port]\n"
              << " For Serial : serial:///path/to/serial/dev[:baudrate]\n"
              << "For example, to connect to the simulator use URL: udp://:14540\n";
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }

    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};
    mavsdk.set_configuration(Mavsdk::Configuration{1, 135, 1});
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

    // Instantiate plugins.
    auto telemetry = Telemetry{system.value()};
    auto lights = Lights{system.value()};

    // print to arm status
    telemetry.subscribe_armed(
        [](bool is_armed) { std::cout << (is_armed ? "armed" : "disarmed") << '\n'; });

    std::vector<uint32_t> colours(8, 0x00FF00FF);
    Lights::LightStrip strip;
    strip.lights = colours;

    while (true) {
        lights.set_strip(0, strip);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        lights.follow_flight_mode(true);
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }

    return 0;
}