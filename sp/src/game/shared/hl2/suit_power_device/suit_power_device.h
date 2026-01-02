#ifndef SUIT_POWER_DEVICE_H
#define SUIT_POWER_DEVICE_H
#ifdef _WIN32
#pragma once
#endif

#include "gamerules.h"
#include "shareddefs.h"

#include <memory>
#include <concepts>

#ifdef VKZ_RESTORABLE_SUIT_POWER_DEVICE
#include "isaverestore.h"
#include <tuple>
#endif

#include "suit_power_device_defs.h"

#ifdef CLIENT_DLL
class C_SuitPowerDevice;
#else
class CSuitPowerDevice;
#endif

namespace SuitPowerDevice {

#ifdef CLIENT_DLL
	using BaseDevice = C_SuitPowerDevice;
#else
	using BaseDevice = CSuitPowerDevice;
#endif

#ifdef VKZ_INVALID_SUIT_POWER_DEVICE
	template<typename T>
	concept ValidatableSuitPowerDevice = true;
#else
	template<typename T>
	concept ValidatableSuitPowerDevice = requires(const T & device) {
		{ device.isValid() } -> std::same_as<bool>;
	};
#endif

#ifdef VKZ_RESTORABLE_SUIT_POWER_DEVICE
	template<typename T>
	concept RestorableSuitPowerDevice = requires(const SaveRestoreFieldInfo_t & fieldInfo, IRestore * pRestore, const T & device) {
		{ T::Read(fieldInfo, pRestore) } -> std::same_as<T>;
	} && ValidatableSuitPowerDevice<T>;
#endif

	template<typename T>
	concept SuitPowerDevice =
		std::derived_from<T, BaseDevice>
#ifdef VKZ_RESTORABLE_SUIT_POWER_DEVICE
		&& RestorableSuitPowerDevice<T>
#endif
		&& std::convertible_to<decltype(T::TypeName), const char *>;
}

#ifdef CLIENT_DLL
/**
 * @brief HEV suit device that use aux power
 */
class C_SuitPowerDevice
#else
class CSuitPowerDevice
#endif
{
public:

#ifdef CLIENT_DLL
	static constexpr const char TypeName[] = "C_SuitPowerDevice";
#else
	static constexpr const char TypeName[] = "CSuitPowerDevice";
#endif

#ifdef VKZ_INVALID_SUIT_POWER_DEVICE
private:
	struct InvalidConstructorKey {};
#endif
public:

#ifdef CLIENT_DLL

	DECLARE_CLASS_NOBASE(C_SuitPowerDevice);

#ifdef VKZ_NETWORKABLE_SUIT_POWER_DEVICE
	DECLARE_EMBEDDED_NETWORKVAR();
#endif

#ifdef VKZ_INVALID_SUIT_POWER_DEVICE
private:
	C_SuitPowerDevice(InvalidConstructorKey)
		: m_bitsDeviceID(0), m_flDrainRate(0.0f) {}

public:
	// Create an invalid device
	C_SuitPowerDevice()
		: C_SuitPowerDevice(Invalid) {}
#endif
public:
	// Avoid using `bits_SUIT_DEVICE_INVALID` as `bitsID`, because it's reserved for invalid device
	C_SuitPowerDevice(int bitsID, float flDrainRate)
		: m_bitsDeviceID(bitsID), m_flDrainRate(flDrainRate)
	{
		validate();
	}

#else

	DECLARE_CLASS_NOBASE(CSuitPowerDevice);

#ifdef VKZ_DATADESC_SUIT_POWER_DEVICE
	// VKZ Knowledge (Simple embedded field for DATADESC table)
	DECLARE_DATADESC();
#endif

#ifdef VKZ_NETWORKABLE_SUIT_POWER_DEVICE
	DECLARE_EMBEDDED_NETWORKVAR();
#endif

#ifdef VKZ_INVALID_SUIT_POWER_DEVICE
private:
	CSuitPowerDevice(InvalidConstructorKey)
#ifndef VKZ_NETWORKABLE_SUIT_POWER_DEVICE
		: m_bitsDeviceID(0), m_flDrainRate(0.0f) {}
#else
		: m_bitsDeviceID(0)
	{
		m_flDrainRate = 0.0f;
	}
#endif

public:
	// Create an invalid device
	CSuitPowerDevice()
		: CSuitPowerDevice(Invalid) {}
#endif
public:
	// Avoid using `bits_SUIT_DEVICE_INVALID` as `bitsID`, because it's reserved for invalid device
	CSuitPowerDevice(int bitsID, float flDrainRate)
#ifndef VKZ_NETWORKABLE_SUIT_POWER_DEVICE
		: m_bitsDeviceID(bitsID), m_flDrainRate(flDrainRate)
	{
		validate();
	}
#else
		: m_bitsDeviceID(bitsID)
	{
		validate();
		m_flDrainRate = flDrainRate;
	}
#endif

#endif

protected:

	int m_bitsDeviceID;	// tells what the device is. DEVICE_SPRINT, DEVICE_FLASHLIGHT, etc. BITMASK!!!!!

#if !defined(CLIENT_DLL) && defined(VKZ_NETWORKABLE_SUIT_POWER_DEVICE)
	CNetworkVar(float, m_flDrainRate);	// how quickly does this device deplete suit power? ( percent per second )
#else
	float m_flDrainRate; // how quickly does this device deplete suit power? ( percent per second )
#endif

public:

#ifdef VKZ_INVALID_SUIT_POWER_DEVICE
	static const ThisClass Invalid;

	// For a basic device, valid means `id != bits_SUIT_DEVICE_INVALID`
	virtual bool isValid() const {
		return !(m_bitsDeviceID & bits_SUIT_DEVICE_INVALID);
	}
#endif

	void validate() const {
#ifdef VKZ_INVALID_SUIT_POWER_DEVICE
#ifdef VKZ_DEV
		Assert(isValid());
#else
		if (!isValid()) {
			auto deviceStr = std::make_unique<char[]>(1024);
			toString(deviceStr);
			DevWarning("Invalid device used: %s\n", deviceStr.get());
		}
#endif
#endif
	}

	virtual bool isSameCategory(const ThisClass &device) const {
		return m_bitsDeviceID & device.m_bitsDeviceID;
	}

	bool equals(const ThisClass &device) const
	{
		return isSameCategory(device) && m_flDrainRate == device.m_flDrainRate;
	}

	virtual void toString(std::unique_ptr<char[]> &pDest) const {
		Q_snprintf(pDest.get(), 1024,
			"{\n"
			"\tid: %d,\n"
			"\tdrainRate: %.2f,\n"
			"}",
			m_bitsDeviceID,
			static_cast<float>(m_flDrainRate)
		);
	}

	bool doesDrainPower() const
	{
		validate();
		return m_flDrainRate > 0;
	}

	bool doesChargePower() const
	{
		validate();
		return m_flDrainRate < 0;
	}

	bool doesChangePower() const
	{
		return doesDrainPower() || doesChargePower();
	}

	/**
	* @brief
	*   Doesn't use this to validate device
	*   This is actually counted as using the device
	*   Since Valve use bitmask to represent active devices
	*/
	int GetDeviceID(void) const
	{
		validate();
		return m_bitsDeviceID;
	}

#ifndef CLIENT_DLL
	virtual float GetDeviceDrainRate(void) const
	{
		validate();
		if (g_pGameRules->GetSkillLevel() == SKILL_EASY && hl2_episodic.GetBool() && !(GetDeviceID() & bits_SUIT_DEVICE_SPRINT))
			// Easy mode drains non-sprint devices at half rate in Episodic?
			return m_flDrainRate * 0.5f;
		else
			return m_flDrainRate;
	}
#endif

	int getRawDrainRate(void) const
	{
		validate();
		return m_flDrainRate;
	}

#ifdef MAPBASE

	virtual void SetDeviceDrainRate(float flDrainRate)
	{
		m_flDrainRate = flDrainRate;
	}

#endif

#if !defined(CLIENT_DLL) && defined(VKZ_RESTORABLE_SUIT_POWER_DEVICE)
protected:
	void WriteSelfExceptId(const SaveRestoreFieldInfo_t &fieldInfo, ISave *pSave) const {
		// StartBlock called
		const auto &drainRate = static_cast<const float &>(m_flDrainRate);
		pSave->WriteFloat(&drainRate);
		// EndBlock called
	}

	static inline std::tuple<float> ReadExceptId(const SaveRestoreFieldInfo_t &fieldInfo, IRestore *pRestore) {
		float drainRate;
		pRestore->ReadFloat(&drainRate);
		return std::make_tuple(drainRate);
	}
public:
	// Used by `CSuitPowerDeviceDataOps` to save this object
	DEFINE_DEVICE_SAVE(fieldInfo, pSave) {
		// StartBlock called
		// Order to write matters
		pSave->WriteInt(&m_bitsDeviceID);
		WriteSelfExceptId(fieldInfo, pSave);
		// EndBlock called
	}



	// Used by `CSuitPowerDeviceDataOps` to restore this object
	DEFINE_DEVICE_RESTORE(fieldInfo, pRestore) {
		// StartBlock called
		int id;
		float drainRate;
		// Order to read matters
		pRestore->ReadInt(&id);
		pRestore->ReadFloat(&drainRate);
		return CSuitPowerDevice(id, drainRate);
		// EndBlock called
	}
#endif

public:
	using DrainRateType = decltype(ThisClass::m_flDrainRate);
};

#ifdef VKZ_NETWORKABLE_SUIT_POWER_DEVICE

#ifdef CLIENT_DLL
EXTERN_RECV_TABLE(DT_SuitPowerDevice)
#else
EXTERN_SEND_TABLE(DT_SuitPowerDevice)
#endif

#endif

namespace SuitPowerDevice {
	/**
	 * @brief Downcasting device
	 * @tparam Device Target device class
	 * @param device Object to be cast
	 * @throws std::bad_cast
	 * @return
	 */
	template<SuitPowerDevice::SuitPowerDevice Device>
	const Device &deviceCast(const SuitPowerDevice::BaseDevice &device) {
		try {
			return dynamic_cast<const Device &>(device);
		} catch (const std::bad_cast &e) {
			auto deviceStr = std::make_unique<char[]>(1024);
			device.toString(deviceStr);
			DevWarning("Device is not a %s: %s\ne.what(): %s\n", static_cast<const char *>(Device::TypeName), deviceStr.get(), e.what());
			throw e;
		}
	}

	/**
	 * @brief Downcasting device
	 * @tparam Device Target device class
	 * @param device Object to be cast
	 * @throws std::bad_cast
	 * @return
	 */
	template<SuitPowerDevice::SuitPowerDevice Device>
	Device &deviceCast(SuitPowerDevice::BaseDevice &device) {
		return const_cast<Device &>(deviceCast<Device>(std::as_const(device)));
	}
}

#include "suit_power_device_undefs.h"

#endif // SUIT_POWER_DEVICE_H
