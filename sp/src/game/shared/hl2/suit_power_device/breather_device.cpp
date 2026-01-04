#include "cbase.h"
#include "breather_device.h"
#include "suit_power_device_defs.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#ifdef VKZ_ADVANCED_BREATHER
using SuitPowerDevice::BreatherDevice;

#ifdef CLIENT_DLL

#ifndef VKZ_NETWORKABLE_SUIT_POWER_DEVICE
#error "VKZ_ADVANCED_BREATHER requires VKZ_NETWORKABLE_SUIT_POWER_DEVICE"
#else
BEGIN_RECV_TABLE_NOBASE(BreatherDevice, DT_BreatherDevice)
	RecvPropsDevice(),
END_RECV_TABLE()
#endif

#else

#ifndef VKZ_NETWORKABLE_SUIT_POWER_DEVICE
#error "VKZ_ADVANCED_BREATHER requires VKZ_NETWORKABLE_SUIT_POWER_DEVICE"
#else
BEGIN_SEND_TABLE_NOBASE(BreatherDevice, DT_BreatherDevice)
	SendPropsDevice(),
END_SEND_TABLE()
#endif

#if !defined(VKZ_DATADESC_SUIT_POWER_DEVICE) && !defined(VKZ_RESTORABLE_SUIT_POWER_DEVICE)
#error "VKZ_ADVANCED_BREATHER requires VKZ_DATADESC_SUIT_POWER_DEVICE or VKZ_RESTORABLE_SUIT_POWER_DEVICE"
#elif defined(VKZ_DATADESC_SUIT_POWER_DEVICE)
BEGIN_DATADESC(BreatherDevice)
END_DATADESC()
#endif

#endif

const BreatherDevice BreatherDevice::Default(DEFAULT_BREATHER_DRAIN_RATE);
#endif
