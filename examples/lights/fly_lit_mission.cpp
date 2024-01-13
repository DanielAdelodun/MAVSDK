//
// Demonstrates how to add and fly Waypoint missions using the MAVSDK.
//

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/action/action.h>
#include <mavsdk/plugins/mission/mission.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <mavsdk/plugins/lights/lights.h>

#include <chrono>
#include <functional>
#include <future>
#include <iostream>
#include <thread>

#include "colormap.hpp"

using namespace mavsdk;
using std::chrono::seconds;
using std::this_thread::sleep_for;

uint8_t POLYGON = 7;
uint8_t STEP = 2;

constexpr uint8_t num_strips = 4;
constexpr uint8_t pixels_per_strip = 24;

Mission::MissionItem make_mission_item(
    double latitude_deg,
    double longitude_deg,
    float relative_altitude_m,
    float speed_m_s,
    bool is_fly_through,
    float gimbal_pitch_deg,
    float gimbal_yaw_deg,
    Mission::MissionItem::CameraAction camera_action)
{
    Mission::MissionItem new_item{};
    new_item.latitude_deg = latitude_deg;
    new_item.longitude_deg = longitude_deg;
    new_item.relative_altitude_m = relative_altitude_m;
    new_item.speed_m_s = speed_m_s;
    new_item.is_fly_through = is_fly_through;
    new_item.gimbal_pitch_deg = gimbal_pitch_deg;
    new_item.gimbal_yaw_deg = gimbal_yaw_deg;
    new_item.camera_action = camera_action;
    new_item.yaw_deg = 0.0f;
    return new_item;
}

void usage(const std::string& bin_name)
{
    std::cerr << "Usage : " << bin_name << " <connection_url> [ <polygon> [<step>] ]\n"
              << "Connection URL format should be :\n"
              << " For TCP : tcp://[server_host][:server_port]\n"
              << " For UDP : udp://[bind_host][:bind_port]\n"
              << " For Serial : serial:///path/to/serial/dev[:baudrate]\n"
              << "For example, to connect to the simulator use URL: udp://:14540\n";
}

double add_north(const double origin_lat, const double vector_n)
{
    static constexpr double EARTH_RADIUS = 6371000;
    return origin_lat + ((vector_n / EARTH_RADIUS) * 180 / M_PI);
}

double add_east(const double origin_lat, const double origin_lon, const double vector_e)
{
    static constexpr double EARTH_RADIUS = 6371000;
    return origin_lon +
           ((vector_e / EARTH_RADIUS) / fabs(cos(origin_lat / 180 * M_PI)) * 180 / M_PI);
}

void add_distance_direction(
    const double origin_lat,
    const double origin_lon,
    const double compass_direction,
    const double distance,
    double* end_lat,
    double* end_lon)
{
    const double vector_n = cos(compass_direction / 180 * M_PI) * distance;
    const double vector_e = sin(compass_direction / 180 * M_PI) * distance;

    *end_lat = add_north(origin_lat, vector_n);
    *end_lon = add_east(origin_lat, origin_lon, vector_e);
}

void set_rel_position(
    const double origin_lat,
    const double origin_lon,
    double* end_lat,
    double* end_lon,
    const double x,
    const double y)
{
    *end_lat = add_north(origin_lat, y);
    *end_lon = add_east(origin_lat, origin_lon, x);
}

int main(int argc, char** argv)
{
    if (argc < 2 || argc > 4) {
        usage(argv[0]);
        return 1;
    }

    if (argc > 2) {
        POLYGON = std::atoi(argv[2]);
        if (argc = 4) {
            STEP = std::atoi(argv[3]);
        }
    }

    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};
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

    auto action = Action{system.value()};
    auto mission = Mission{system.value()};
    auto telemetry = Telemetry{system.value()};
    auto lights = Lights{system.value()};

    while (!telemetry.health_all_ok()) {
        std::cout << "Waiting for system to be ready\n";
        sleep_for(seconds(1));
    }

    std::cout << "System ready\n";

    auto colormap = read_colormap();
    std::vector<Lights::LightMatrix> matrices(POLYGON);

    for (int m = 0; m < POLYGON; m++) {
        auto colorPair = random_color(colormap);
        auto colorName = colorPair.first;
        auto color = colorPair.second;

        Lights::LightMatrix matrix;

        for (uint8_t i = 0; i < num_strips; i++) {
            std::vector<uint32_t> colors(pixels_per_strip, color);
            matrix.strips.push_back(Lights::LightStrip{.lights = std::move(colors)});
        }
        printf("Side %02d: %s\n", m, colorName.c_str());
        matrices[m] = std::move(matrix);
    }

    const auto res_and_gps_origin = telemetry.get_gps_global_origin();
    if (res_and_gps_origin.first != Telemetry::Result::Success) {
        std::cerr << "Telemetry failed: " << res_and_gps_origin.first << '\n';
    }
    Telemetry::GpsGlobalOrigin origin = res_and_gps_origin.second;

    std::vector<Mission::MissionItem> mission_items;

    double target_lat;
    double target_lon;
    double polygon_x;
    double polygon_y;
    for (int i = 0; i < POLYGON + 1; i++) {
        polygon_x = cos(2 * M_PI * i * STEP / POLYGON) * 20;
        polygon_y = sin(2 * M_PI * i * STEP / POLYGON) * 20;
        set_rel_position(
            origin.latitude_deg,
            origin.longitude_deg,
            &target_lat,
            &target_lon,
            polygon_x,
            polygon_y);
        mission_items.push_back(make_mission_item(
            target_lat,
            target_lon,
            15.0f + i,
            12.0f,
            false,
            20.0f,
            60.0f,
            Mission::MissionItem::CameraAction::None));
    }

    std::cout << "Uploading mission...\n";
    Mission::MissionPlan mission_plan{};
    mission_plan.mission_items = mission_items;
    const Mission::Result upload_result = mission.upload_mission(mission_plan);

    if (upload_result != Mission::Result::Success) {
        std::cerr << "Mission upload failed: " << upload_result << ", exiting.\n";
        return 1;
    }

    std::cout << "Arming...\n";
    const Action::Result arm_result = action.arm();
    if (arm_result != Action::Result::Success) {
        std::cerr << "Arming failed: " << arm_result << '\n';
        return 1;
    }
    std::cout << "Armed.\n";

    int32_t mission_current = 0;
    int32_t color_current = 0;
    mission.subscribe_mission_progress(
        [&mission_current, &color_current](Mission::MissionProgress mission_progress) {
            std::cout << "Mission status update: " << mission_progress.current << " / "
                      << mission_progress.total << '\n';
            mission_current = mission_progress.current;
        });

    Mission::Result start_mission_result = mission.start_mission();
    if (start_mission_result != Mission::Result::Success) {
        std::cerr << "Starting mission failed: " << start_mission_result << '\n';
        return 1;
    }

    while (!mission.is_mission_finished().second) {
        if (color_current < mission_current) {
            lights.set_matrix(matrices[mission_current - 1]);
            color_current = mission_current;
        }
    }

    lights.follow_flight_mode(true);

    // We are done, and can do RTL to go home.
    std::cout << "Commanding RTL...\n";
    const Action::Result rtl_result = action.return_to_launch();
    if (rtl_result != Action::Result::Success) {
        std::cout << "Failed to command RTL: " << rtl_result << '\n';
        return 1;
    }
    std::cout << "Commanded RTL.\n";

    // We need to wait a bit, otherwise the armed state might not be correct yet.
    sleep_for(seconds(2));

    while (telemetry.armed()) {
        // Wait until we're done.
        sleep_for(seconds(1));
    }
    std::cout << "Disarmed, exiting.\n";
}
