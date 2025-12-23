#include "gamerules.h"
#include "shareddefs.h"

#ifdef VKZ_RESTORABLE_SUIT_POWER_DEVICE
#include "isaverestore.h"
#endif

//=============================================================================
// Purpose: Data of a HEV suit device that use aux power
// Note that objects should not be default initialized
//=============================================================================
class CSuitPowerDevice
{
private:
#ifdef VKZ_INVALID_SUIT_POWER_DEVICE
	// This should only be used to create `CSuitPowerDevice::Invalid`
	consteval CSuitPowerDevice() noexcept
		: m_bitsDeviceID(0), m_flDrainRate(0.0f) {}
#endif
public:
	// NOTE: Prevent to use `bits_SUIT_DEVICE_INVALID` as `bitsID`, because it's reserved for invalid device
	constexpr CSuitPowerDevice(int bitsID, float flDrainRate)
		: m_bitsDeviceID(bitsID), m_flDrainRate(flDrainRate)
#ifndef VKZ_INVALID_SUIT_POWER_DEVICE
		noexcept
#endif
	{
#ifdef VKZ_INVALID_SUIT_POWER_DEVICE
		// You cannot create an invalid suit power device
		// Use `CSuitPowerDevice::Invalid` instead
		Assert(bitsID != bits_SUIT_DEVICE_INVALID);
#endif
	}
private:
	int		m_bitsDeviceID;	// tells what the device is. DEVICE_SPRINT, DEVICE_FLASHLIGHT, etc. BITMASK!!!!!
	float	m_flDrainRate;	// how quickly does this device deplete suit power? ( percent per second )

public:
#ifdef VKZ_INVALID_SUIT_POWER_DEVICE
	// Invalid suit device that shouldn't be used
	// This serve as a placeholder
	static constinit const CSuitPowerDevice Invalid;

	constexpr bool isValid() const noexcept { return m_bitsDeviceID != bits_SUIT_DEVICE_INVALID; }
#endif

	constexpr bool doesDrainPower() const
#ifndef VKZ_INVALID_SUIT_POWER_DEVICE
		noexcept
#endif
	{
#ifdef VKZ_INVALID_SUIT_POWER_DEVICE
		Assert(isValid());
#endif
		return m_flDrainRate > 0;
	}
	constexpr bool doesChargePower() const
#ifndef VKZ_INVALID_SUIT_POWER_DEVICE
		noexcept
#endif
	{
#ifdef VKZ_INVALID_SUIT_POWER_DEVICE
		Assert(isValid());
#endif
		return m_flDrainRate < 0;
	}
	constexpr bool doesChangePower() const
#ifndef VKZ_INVALID_SUIT_POWER_DEVICE
		noexcept
#endif
	{
		return doesDrainPower() || doesChargePower();
	}

	// Doesn't use this to validate device
	constexpr int	GetDeviceID(void) const
#ifndef VKZ_INVALID_SUIT_POWER_DEVICE
		noexcept
#endif
	{
#ifdef VKZ_INVALID_SUIT_POWER_DEVICE
		Assert(isValid());
#endif
		return m_bitsDeviceID;
	}

	float	GetDeviceDrainRate(void) const
#ifndef VKZ_INVALID_SUIT_POWER_DEVICE
		noexcept
#endif
	{
#ifdef VKZ_INVALID_SUIT_POWER_DEVICE
		Assert(isValid());
#endif
		if (g_pGameRules->GetSkillLevel() == SKILL_EASY && hl2_episodic.GetBool() && !(GetDeviceID() & bits_SUIT_DEVICE_SPRINT))
			// Easy mode drains non-sprint devices at half rate in Episodic?
			return m_flDrainRate * 0.5f;
		else
			return m_flDrainRate;
	}

#ifdef MAPBASE
	constexpr void SetDeviceDrainRate(float flDrainRate)
#ifndef VKZ_INVALID_SUIT_POWER_DEVICE
		noexcept
#endif
	{
#ifdef VKZ_INVALID_SUIT_POWER_DEVICE
		Assert(isValid());
#endif
		m_flDrainRate = flDrainRate;
	}
#endif

#ifdef VKZ_RESTORABLE_SUIT_POWER_DEVICE
	friend class CSuitPowerDeviceDataOps;
#endif
};

#ifdef VKZ_RESTORABLE_SUIT_POWER_DEVICE

// VKZ Knowledge (Simple custom field for DATADESC table):

//=============================================================================
// Purpose: Provide method to save & restore `CSuitPowerDevice` objects
//=============================================================================
class CSuitPowerDeviceDataOps : public CDefSaveRestoreOps
{
private:
	consteval CSuitPowerDeviceDataOps() noexcept = default;

	// Global object for save & restore `CSuitPowerDevice`
	static constinit CSuitPowerDeviceDataOps _instance;

	// You can declare any object that needed for this process here as member
	// Then inject them with constructor
public:
	static inline CSuitPowerDeviceDataOps* Get() noexcept {
		return &_instance;
	}

	// It's not a class defining pure functions to encode or decode your object,
	// but rather directly performing side effects to save and restore a field of the object
	// 
	// For encoding, you have to do side effects with utilities provided by `pSave` or other objects
	// For decoding, simply do side effects like mutating `fieldInfo.pField`, which points to your object

	virtual void Save(const SaveRestoreFieldInfo_t& fieldInfo, ISave* pSave)
	{
		// This is a pointer to the object I need to save
		const auto device = static_cast<const CSuitPowerDevice*>(fieldInfo.pField);
		const int id = device->m_bitsDeviceID;
		const float drainRate = device->m_flDrainRate;

		// Using `StartBlock` and `EndBlock` to save and restore values
		// is much like operating a queue

		// Call this to start "queuing" values
		pSave->StartBlock();
		{
			// Use those write methods to push values into "queue"
			pSave->WriteInt(&id);
			pSave->WriteFloat(&drainRate);
		}
		// Call this to indicate you are done with the "queue"
		pSave->EndBlock();
	}

	virtual void Restore(const SaveRestoreFieldInfo_t& fieldInfo, IRestore* pRestore)
	{
		// This is a pointer to the object I need to restore
		const auto device = static_cast<CSuitPowerDevice*>(fieldInfo.pField);
		int id;
		float drainRate;

		// Using `StartBlock` and `EndBlock` to save and restore values
		// is much like operating a queue

		// Call this to start reading values from "queue"
		pRestore->StartBlock();
		{
			pRestore->ReadInt(&id);
			pRestore->ReadFloat(&drainRate);
		}
		// Call this to indicate you are done with the "queue"
		pRestore->EndBlock();

		*device = std::move(CSuitPowerDevice(id, drainRate));
	}

	// Make the object empty. I think this will be called before restoration
	virtual void MakeEmpty(const SaveRestoreFieldInfo_t& fieldInfo)
	{
		// This is a pointer to the object I need to make empty
		const auto device = static_cast<CSuitPowerDevice*>(fieldInfo.pField);
		*device = CSuitPowerDevice::Invalid;
	}

	// The only thing I know about this function is that
	// if this returns true for the object, it won't be saved
	virtual bool IsEmpty(const SaveRestoreFieldInfo_t& fieldInfo)
	{
		// This is a pointer to the object I need to check
		const auto device = static_cast<const CSuitPowerDevice*>(fieldInfo.pField);
		return !device->isValid();
	}
};
#endif
