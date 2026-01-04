#ifndef BREATHER_DEVICE_H
#define BREATHER_DEVICE_H
#pragma once

#include "gamerules.h"
#include "shareddefs.h"
#include "suit_power_device.h"

#ifdef VKZ_RESTORABLE_SUIT_POWER_DEVICE
#include "isaverestore.h"
#include <tuple>
#endif

#include "suit_power_device_defs.h"

#ifdef VKZ_ADVANCED_BREATHER
#ifdef CLIENT_DLL
class C_BreatherDevice;
#else
class CBreatherDevice;
#endif
#endif

namespace SuitPowerDevice {
#ifdef VKZ_ADVANCED_BREATHER
#ifdef CLIENT_DLL
	using BreatherDevice = C_BreatherDevice;
#else
	using BreatherDevice = CBreatherDevice;
#endif
#endif
}

constexpr auto DEFAULT_BREATHER_DRAIN_RATE = 6.7f;

#ifdef VKZ_ADVANCED_BREATHER

#ifdef CLIENT_DLL
/**
* @brief Controls how long the user can breath underwater
*/
class C_BreatherDevice : public C_SuitPowerDevice
#else
class CBreatherDevice : public CSuitPowerDevice
#endif
{
public:

#ifdef CLIENT_DLL
	static constexpr const char TypeName[] = "C_BreatherDevice";
#else
	static constexpr const char TypeName[] = "CBreatherDevice";
#endif

#ifdef CLIENT_DLL
	DECLARE_CLASS(C_BreatherDevice, C_SuitPowerDevice);
#else
	DECLARE_CLASS(CBreatherDevice, CSuitPowerDevice);
#ifdef VKZ_DATADESC_SUIT_POWER_DEVICE
	DECLARE_DATADESC();
#endif
#endif

#ifndef VKZ_NETWORKABLE_SUIT_POWER_DEVICE
#error "VKZ_ADVANCED_BREATHER requires VKZ_NETWORKABLE_SUIT_POWER_DEVICE"
#endif
	DECLARE_EMBEDDED_NETWORKVAR();

#ifdef CLIENT_DLL
#define BreatherDeviceConstructor C_BreatherDevice
#else
#define BreatherDeviceConstructor CBreatherDevice
#endif
	BreatherDeviceConstructor()
		: ThisClass(Default) {}

	BreatherDeviceConstructor(float drainRate)
		: BaseClass(bits_SUIT_DEVICE_BREATHER, drainRate) {}
#undef BreatherDeviceConstructor
public:
	static const ThisClass Default;

	// For a breather device, returns `true` only if device id is `bits_SUIT_DEVICE_BREATHER`
	virtual bool isValid() const { return m_bitsDeviceID & bits_SUIT_DEVICE_BREATHER; }

	bool equals(const ThisClass &device) const {
		return BaseClass::equals(device);
	}

#if !defined(CLIENT_DLL) && defined(VKZ_RESTORABLE_SUIT_POWER_DEVICE)
public:
#ifndef VKZ_RESTORABLE_SUIT_POWER_DEVICE
#error "VKZ_ADVANCED_BREATHER requires VKZ_RESTORABLE_SUIT_POWER_DEVICE"
#endif

	DEFINE_DEVICE_SAVE(fieldInfo, pSave) {
		BaseClass::WriteSelfExceptId(fieldInfo, pSave);
	}

	DEFINE_DEVICE_RESTORE(fieldInfo, pRestore) {
		const auto &baseDeviceData = BaseClass::ReadExceptId(fieldInfo, pRestore);
		const auto drainRate = std::get<0>(baseDeviceData);
		return ThisClass(drainRate);
	};
#endif
};

#ifdef CLIENT_DLL
EXTERN_RECV_TABLE(DT_BreatherDevice)
#else
EXTERN_SEND_TABLE(DT_BreatherDevice)
#endif

#endif

#include "suit_power_device_undefs.h"

#endif // BREATHER_DEVICE_H
