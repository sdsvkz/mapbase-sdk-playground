#include "cbase.h"
#include "suit_power_device.h"

#ifdef VKZ_INVALID_SUIT_POWER_DEVICE
constinit const CSuitPowerDevice CSuitPowerDevice::Invalid;
#endif

#ifndef VKZ_ADVANCED_SUIT_POWER_DEVICE
constinit CSuitPowerDeviceDataOps CSuitPowerDeviceDataOps::_instance = CSuitPowerDeviceDataOps();
#endif
