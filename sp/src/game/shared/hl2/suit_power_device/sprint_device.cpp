#include "cbase.h"
#include "sprint_device.h"
#include "suit_power_device_defs.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#ifdef VKZ_ADVANCED_SPRINT
using SuitPowerDevice::SprintDevice;
#endif

#ifdef CLIENT_DLL

#ifdef VKZ_ADVANCED_SPRINT

#ifndef VKZ_NETWORKABLE_SUIT_POWER_DEVICE
#error "VKZ_ADVANCED_SPRINT requires VKZ_NETWORKABLE_SUIT_POWER_DEVICE"
#else
BEGIN_RECV_TABLE_NOBASE(SprintDevice, DT_SprintDevice)
	RecvPropsDevice(),
	RecvPropFloat(RECVINFO(m_flSprintSpeed)),
END_RECV_TABLE()
#endif

#endif

#else

#ifdef VKZ_ADVANCED_SPRINT

#ifndef VKZ_NETWORKABLE_SUIT_POWER_DEVICE
#error "VKZ_ADVANCED_SPRINT requires VKZ_NETWORKABLE_SUIT_POWER_DEVICE"
#else
BEGIN_SEND_TABLE_NOBASE(SprintDevice, DT_SprintDevice)
	SendPropsDevice(),
	SendPropFloat(SENDINFO(m_flSprintSpeed), -1, SPROP_COORD),
END_SEND_TABLE()
#endif

#if !defined(VKZ_DATADESC_SUIT_POWER_DEVICE) && !defined(VKZ_RESTORABLE_SUIT_POWER_DEVICE)
#error "VKZ_ADVANCED_SPRINT requires VKZ_DATADESC_SUIT_POWER_DEVICE or VKZ_RESTORABLE_SUIT_POWER_DEVICE"
#elif defined(VKZ_DATADESC_SUIT_POWER_DEVICE)
BEGIN_DATADESC(SprintDevice)
	DEFINE_FIELD(m_flSprintSpeed, FIELD_INTEGER),
END_DATADESC()
#endif

#endif

#endif

#ifdef VKZ_ADVANCED_SPRINT
const SprintDevice SprintDevice::Default(DEFAULT_SPRINT_DRAIN_RATE, DEFAULT_SPRINT_SPEED);
#endif
