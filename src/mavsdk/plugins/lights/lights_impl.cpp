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






Lights::Result LightsImpl::set_matrix(Lights::LightMatrix matrix)
{
    // Iterate over all strips and send them one by one.
    for (int strip_id = 0; strip_id < matrix.strips.size(); strip_id++) {
        auto result = _system_impl->queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
            mavlink_message_t message;
            mavlink_msg_led_strip_config_pack_chan(
                mavlink_address.system_id,
                mavlink_address.component_id,
                channel,
                &message,
                _system_impl->get_system_id(),
                _system_impl->get_autopilot_id(),
                LED_FILL_MODE_INDEX,
                0,
                8,
                strip_id,
                matrix.strips[i].lights.begin()); // TODO: fix out of bounds
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





Lights::Result LightsImpl::set_strip(uint32_t strip_id, Lights::LightStrip strip)
{
    return _system_impl->queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
            mavlink_message_t message;
            mavlink_msg_led_strip_config_pack_chan(
                mavlink_address.system_id,
                mavlink_address.component_id,
                channel,
                &message,
                _system_impl->get_system_id(),
                _system_impl->get_autopilot_id(),
                LED_FILL_MODE_INDEX,
                0,
                8,
                static_cast<uint8_t>(strip_id),
                strip.lights.begin()); // TODO: fix out of bounds
            return message;
        }) ?
                Lights::Result::Success :
                Lights::Result::ConnectionError;
}





Lights::Result LightsImpl::follow_flight_mode(bool enable)
{
    
    if (!enable) {
        return Lights::Result::Success;
    }

    uint32_t c[8] {0, 0, 0, 0, 0, 0, 0, 0};

    return _system_impl->queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
            mavlink_message_t message;
            mavlink_msg_led_strip_config_pack_chan(
                mavlink_address.system_id,
                mavlink_address.component_id,
                channel,
                &message,
                _system_impl->get_system_id(),
                _system_impl->get_autopilot_id(),
                LED_FILL_MODE_FOLLOW_FLIGHT_MODE,
                0,
                8,
                0,
                c);
            return message;
        }) ?
                Lights::Result::Success :
                Lights::Result::ConnectionError;

}



} // namespace mavsdk