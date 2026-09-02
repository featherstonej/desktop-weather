#pragma once

#include "config.h"

class Storage {
public:
    static bool begin();
    static bool loadConfig(DeviceConfig &config);
    static bool saveConfig(const DeviceConfig &config);
};
