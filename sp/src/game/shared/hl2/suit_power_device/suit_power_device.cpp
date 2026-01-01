#include "cbase.h"
#include "suit_power_device.h"
#include "suit_power_device_defs.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using SuitPowerDevice::BaseDevice;

#ifdef CLIENT_DLL

#ifdef VKZ_NETWORKABLE_SUIT_POWER_DEVICE
BEGIN_RECV_TABLE_NOBASE(BaseDevice, DT_SuitPowerDevice)
	RecvPropsDevice(),
END_RECV_TABLE()
#endif

#else

#ifdef VKZ_NETWORKABLE_SUIT_POWER_DEVICE
BEGIN_SEND_TABLE_NOBASE(BaseDevice, DT_SuitPowerDevice)
	SendPropsDevice(),
END_SEND_TABLE()
#endif

#ifdef VKZ_DATADESC_SUIT_POWER_DEVICE
// VKZ Knowledge (Simple embedded field for DATADESC table)
BEGIN_DATADESC_NO_BASE(BaseDevice)
	DEFINE_FIELD(m_bitsDeviceID, FIELD_INTEGER),
	DEFINE_FIELD(m_flDrainRate, FIELD_FLOAT),
END_DATADESC()
#endif

#endif

#ifdef VKZ_INVALID_SUIT_POWER_DEVICE
const BaseDevice BaseDevice::Invalid(BaseDevice::InvalidConstructorKey{});
#endif
