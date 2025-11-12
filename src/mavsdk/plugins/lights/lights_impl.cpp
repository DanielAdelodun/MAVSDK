#include "lights_impl.h"

namespace mavsdk {


LightsImpl::LightsImpl(System& system) : PluginImplBase(system)
{
    _system_impl->register_plugin(this);
}

LightsImpl::LightsImpl(std::shared_ptr<System> system) : PluginImplBase(std::move(system))
{
    _system_impl->register_plugin(this);
}


LightsImpl::~LightsImpl()
{

    _system_impl->unregister_plugin(this);

}

void LightsImpl::init() {}

void LightsImpl::deinit() {}


void LightsImpl::enable() {}

void LightsImpl::disable() {}






Lights::Result LightsImpl::set_matrix(const Lights::LightMatrix matrix)
{
    for (uint8_t strip_id = 0; strip_id < matrix.strips.size(); strip_id++) {
        auto result = set_strip(strip_id, matrix.strips[strip_id]);

        if (result != Lights::Result::Success) {
            return result;
        }
    }

    return Lights::Result::Success;
}





Lights::Result LightsImpl::set_strip(uint32_t strip_id, const Lights::LightStrip strip)
{
    int num_leds = strip.lights.size();
    int num_groups = num_leds / 8;
    int remainder = num_leds % 8;

    for (int i = 0; i < num_groups; i++) {
        std::array<uint32_t, 8> c{0, 0, 0, 0, 0, 0, 0, 0};
        for (int j = 0; j < 8; j++) {
            c[j] = strip.lights[i * 8 + j];
        }

        auto result = _system_impl->queue_message(
                          [c, this, i, strip_id](MavlinkAddress mavlink_address, uint8_t channel) {
                              mavlink_message_t message;
                              mavlink_msg_led_strip_config_pack_chan(
                                  mavlink_address.system_id,
                                  mavlink_address.component_id,
                                  channel,
                                  &message,
                                  _system_impl->get_system_id(),
                                  134, // Lights device component ID
                                  LED_FILL_MODE_INDEX,
                                  i * 8,
                                  8,
                                  strip_id,
                                  c.data());
                              return message;
                          }) ?
                          Lights::Result::Success :
                          Lights::Result::ConnectionError;

        if (result != Lights::Result::Success) {
            return result;
        }
    }

    if (remainder > 0) {
        std::array<uint32_t, 8> c{};
        for (int j = 0; j < remainder; j++) {
            c[j] = strip.lights[num_groups * 8 + j];
        }

        auto result =
            _system_impl->queue_message([c, this, num_groups, remainder, strip_id](
                                            MavlinkAddress mavlink_address, uint8_t channel) {
                mavlink_message_t message;
                mavlink_msg_led_strip_config_pack_chan(
                    mavlink_address.system_id,
                    mavlink_address.component_id,
                    channel,
                    &message,
                    _system_impl->get_system_id(),
                    134,
                    LED_FILL_MODE_INDEX,
                    num_groups * 8,
                    remainder,
                    strip_id,
                    c.data());
                return message;
            }) ?
                Lights::Result::Success :
                Lights::Result::ConnectionError;

        if (result != Lights::Result::Success) {
            return result;
        }
    }

    return Lights::Result::Success;
}





Lights::Result LightsImpl::follow_flight_mode(uint32_t strip_id)
{
    uint32_t c[8]{0, 0, 0, 0, 0, 0, 0, 0};

    return _system_impl->queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
        mavlink_message_t message;
        mavlink_msg_led_strip_config_pack_chan(
            mavlink_address.system_id,
            mavlink_address.component_id,
            channel,
            &message,
            _system_impl->get_system_id(),
            134,
            LED_FILL_MODE_FOLLOW_FLIGHT_MODE,
            0,
            8,
            strip_id,
            c);
        return message;
    }) ?
               Lights::Result::Success :
               Lights::Result::ConnectionError;
}




} // namespace mavsdk