#include "cbase.h"
#include "suit_power_device.h"

#ifdef CLIENT_DLL
#include "dt_recv.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using SuitPowerDevice::BaseDevice;

#ifdef VKZ_ADVANCED_SPRINT
using SuitPowerDevice::SprintDevice;
#endif

#ifdef CLIENT_DLL

#ifdef VKZ_NETWORKABLE_SUIT_POWER_DEVICE
#define RecvPropDevice() \
	RecvPropFloat(RECVINFO(m_flDrainRate))

BEGIN_RECV_TABLE_NOBASE(BaseDevice, DT_SuitPowerDevice)
	RecvPropDevice(),
END_RECV_TABLE()
#endif

#ifdef VKZ_ADVANCED_SPRINT

#ifndef VKZ_NETWORKABLE_SUIT_POWER_DEVICE
	#error "VKZ_ADVANCED_SPRINT requires VKZ_NETWORKABLE_SUIT_POWER_DEVICE"
#else
BEGIN_RECV_TABLE_NOBASE(SprintDevice, DT_SprintDevice)
	RecvPropDevice(),
	RecvPropFloat(RECVINFO(m_flSprintSpeed)),
END_RECV_TABLE()
#endif

#endif

#else

#ifdef VKZ_NETWORKABLE_SUIT_POWER_DEVICE
#define SendPropDevice() \
	SendPropFloat(SENDINFO(m_flDrainRate), -1, SPROP_COORD)

BEGIN_SEND_TABLE_NOBASE(BaseDevice, DT_SuitPowerDevice)
SendPropDevice(),
END_SEND_TABLE()
#endif

#ifdef VKZ_DATADESC_SUIT_POWER_DEVICE
// VKZ Knowledge (Simple embedded field for DATADESC table)
BEGIN_DATADESC_NO_BASE(BaseDevice)
DEFINE_FIELD(m_bitsDeviceID, FIELD_INTEGER),
DEFINE_FIELD(m_flDrainRate, FIELD_FLOAT),
END_DATADESC()
#endif

#ifdef VKZ_ADVANCED_SPRINT

#ifndef VKZ_NETWORKABLE_SUIT_POWER_DEVICE
#error "VKZ_ADVANCED_SPRINT requires VKZ_NETWORKABLE_SUIT_POWER_DEVICE"
#else
BEGIN_SEND_TABLE_NOBASE(SprintDevice, DT_SprintDevice)
SendPropDevice(),
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

#ifdef VKZ_INVALID_SUIT_POWER_DEVICE
const BaseDevice BaseDevice::Invalid(BaseDevice::InvalidConstructorKey{});
#endif

#ifdef VKZ_ADVANCED_SPRINT
const SprintDevice SprintDevice::Default(DEFAULT_SPRINT_DRAIN_RATE, DEFAULT_SPRINT_SPEED);
#endif
