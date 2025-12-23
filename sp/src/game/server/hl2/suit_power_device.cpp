#include "cbase.h"
#include "suit_power_device.h"

#ifdef VKZ_RESTORABLE_SUIT_POWER_DEVICE
constinit CSuitPowerDeviceDataOps CSuitPowerDeviceDataOps::_instance;
#endif

#ifdef VKZ_INVALID_SUIT_POWER_DEVICE
constinit const CSuitPowerDevice CSuitPowerDevice::Invalid;
#endif
