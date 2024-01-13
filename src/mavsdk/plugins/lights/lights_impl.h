#pragma once

#include "plugins/lights/lights.h"

#include "plugin_impl_base.h"

namespace mavsdk {

class LightsImpl : public PluginImplBase {
public:
    explicit LightsImpl(System& system);
    explicit LightsImpl(std::shared_ptr<System> system);

    ~LightsImpl() override;

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    Lights::Result set_strip(uint32_t strip_id, const Lights::LightStrip& strip) const;
    Lights::Result set_matrix(const Lights::LightMatrix& matrix) const;

    Lights::Result follow_flight_mode(bool enable);

private:
};

} // namespace mavsdk