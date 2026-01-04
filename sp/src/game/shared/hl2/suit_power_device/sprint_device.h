#ifndef SPRINT_DEVICE_H
#define SPRINT_DEVICE_H
#pragma once

#include "gamerules.h"
#include "shareddefs.h"
#include "suit_power_device.h"

#include <memory>

#ifdef VKZ_RESTORABLE_SUIT_POWER_DEVICE
#include "isaverestore.h"
#include <tuple>
#endif

#include "suit_power_device_defs.h"

#ifdef VKZ_ADVANCED_SPRINT
#ifdef CLIENT_DLL
class C_SprintDevice;
#else
class CSprintDevice;
#endif
#endif

namespace SuitPowerDevice {
#ifdef VKZ_ADVANCED_SPRINT
#ifdef CLIENT_DLL
	using SprintDevice = C_SprintDevice;
#else
	using SprintDevice = CSprintDevice;
#endif
#endif
}

#ifdef HL2MP
// 100 units in 4 seconds
constexpr auto DEFAULT_SPRINT_DRAIN_RATE = 25.0f;
#else
// 100 units in 8 seconds
constexpr auto DEFAULT_SPRINT_DRAIN_RATE = 12.5f;
#endif

constexpr float DEFAULT_SPRINT_SPEED = 320;

#ifdef VKZ_ADVANCED_SPRINT

#ifdef CLIENT_DLL
/**
 * @brief Controls how fast and how much time the user run
 *
 * Note that you still need to set `sv_maxspeed` for speed higher than 320
 */
class C_SprintDevice : public C_SuitPowerDevice
#else
class CSprintDevice : public CSuitPowerDevice
#endif
{
public:

#ifdef CLIENT_DLL
	static constexpr const char TypeName[] = "C_SprintDevice";
#else
	static constexpr const char TypeName[] = "CSprintDevice";
#endif

#ifdef CLIENT_DLL
	DECLARE_CLASS(C_SprintDevice, C_SuitPowerDevice);
#else
	DECLARE_CLASS(CSprintDevice, CSuitPowerDevice);
#endif

#ifndef VKZ_NETWORKABLE_SUIT_POWER_DEVICE
#error "VKZ_ADVANCED_SPRINT requires VKZ_NETWORKABLE_SUIT_POWER_DEVICE"
#endif
	DECLARE_EMBEDDED_NETWORKVAR();

#ifdef CLIENT_DLL

	C_SprintDevice()
		: ThisClass(Default) {}

	C_SprintDevice(float drainRate, float sprintSpeed = DEFAULT_SPRINT_SPEED)
		: BaseClass(bits_SUIT_DEVICE_SPRINT, drainRate), m_flSprintSpeed(sprintSpeed) {}

protected:
	float m_flSprintSpeed;

#else

#ifdef VKZ_DATADESC_SUIT_POWER_DEVICE
	DECLARE_DATADESC();
#endif

	CSprintDevice()
		: CSprintDevice(Default) {}

	CSprintDevice(float drainRate, float sprintSpeed = DEFAULT_SPRINT_SPEED)
		: BaseClass(bits_SUIT_DEVICE_SPRINT, drainRate)
	{
		m_flSprintSpeed = sprintSpeed;
	}

protected:
#ifndef VKZ_NETWORKABLE_SUIT_POWER_DEVICE
#error "VKZ_ADVANCED_SPRINT requires VKZ_NETWORKABLE_SUIT_POWER_DEVICE"
#endif
	CNetworkVar(float, m_flSprintSpeed);

#endif

public:

	static const ThisClass Default;

	// For a sprint device, returns `true` only if device id is `bits_SUIT_DEVICE_SPRINT`
	virtual bool isValid() const { return m_bitsDeviceID & bits_SUIT_DEVICE_SPRINT; }

	bool equals(const ThisClass &device) const {
		return BaseClass::equals(device) && m_flSprintSpeed == device.m_flSprintSpeed;
	}

	virtual void toString(std::unique_ptr<char[]> &pDest) const {
		Q_snprintf(pDest.get(), 1024,
			"{\n"
			"\tid: %d,\n"
			"\tdrainRate: %.2f,\n"
			"\tsprintSpeed: %.2f,\n"
			"}",
			m_bitsDeviceID,
			static_cast<float>(m_flDrainRate),
			static_cast<float>(m_flSprintSpeed)
		);
	}

	virtual void resetDrainRate() {
		SetDeviceDrainRate(DEFAULT_SPRINT_DRAIN_RATE);
	}

	virtual float getSprintSpeed() const
	{
		validate();
		return m_flSprintSpeed;
	}

	virtual void setSprintSpeed(float speed) {
		m_flSprintSpeed = speed;
	}

	virtual void resetSprintSpeed() {
		setSprintSpeed(DEFAULT_SPRINT_SPEED);
	}

#if !defined(CLIENT_DLL) && defined(VKZ_RESTORABLE_SUIT_POWER_DEVICE)
public:
#ifndef VKZ_RESTORABLE_SUIT_POWER_DEVICE
#error "VKZ_ADVANCED_SPRINT requires VKZ_RESTORABLE_SUIT_POWER_DEVICE"
#endif

	DEFINE_DEVICE_SAVE(fieldInfo, pSave) {
		pSave->WriteFloat(&m_flSprintSpeed.Get());
		BaseClass::WriteSelfExceptId(fieldInfo, pSave);
	}

	DEFINE_DEVICE_RESTORE(fieldInfo, pRestore) {
		float sprintSpeed;
		// Careful with ordering
		pRestore->ReadFloat(&sprintSpeed);
		const auto &baseDeviceData = BaseClass::ReadExceptId(fieldInfo, pRestore);
		const auto drainRate = std::get<0>(baseDeviceData);
		return ThisClass(drainRate, sprintSpeed);
	};
#endif

public:
	using SprintSpeedType = decltype(ThisClass::m_flSprintSpeed);
};

#ifdef CLIENT_DLL
EXTERN_RECV_TABLE(DT_SprintDevice)
#else
EXTERN_SEND_TABLE(DT_SprintDevice)
#endif

#endif

#include "suit_power_device_undefs.h"

#endif // SPRINT_DEVICE_H
