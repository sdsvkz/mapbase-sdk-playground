#ifndef SUIT_POWER_DEVICE_H
#define SUIT_POWER_DEVICE_H
#ifdef _WIN32
    #pragma once
#endif

#include "gamerules.h"
#include "shareddefs.h"

#if defined(VKZ_RESTORABLE_SUIT_POWER_DEVICE) || defined(VKZ_ADVANCED_SUIT_POWER_DEVICE)
#include <concepts>
#endif

#ifdef VKZ_RESTORABLE_SUIT_POWER_DEVICE
#include "isaverestore.h"
#endif

class CSuitPowerDevice;

#ifdef VKZ_RESTORABLE_SUIT_POWER_DEVICE

// `WriteSelf` function signature for device classes
#define DEFINE_DEVICE_SAVE(fieldInfoParamName, pSaveParamName) \
    virtual void WriteSelf( const SaveRestoreFieldInfo_t& fieldInfoParamName, ISave* pSaveParamName ) const

// `Read` function signature for device classes
#define DEFINE_DEVICE_RESTORE(fieldInfoParamName, pRestoreParamName) \
    static inline ThisClass Read( const SaveRestoreFieldInfo_t& fieldInfoParamName, IRestore* pRestoreParamName )

#endif

#ifdef VKZ_ADVANCED_SUIT_POWER_DEVICE

// Call this in `public` scope right after class declaration
// It will:
// - Define type alias `ThisClass` and `BaseClass`
#define DECLARE_SUIT_POWER_DEVICE(thisClassName, baseClassName) \
    using ThisClass = thisClassName; \
    using BaseClass = baseClassName;

    #ifdef VKZ_RESTORABLE_SUIT_POWER_DEVICE
template<typename T>
concept RestorableSuitDevice = requires(const SaveRestoreFieldInfo_t & fieldInfo, IRestore * pRestore)
{
	{ T::Read(fieldInfo, pRestore) } -> std::same_as<T>;
};
    #endif

template<typename T>
concept SuitPowerDevice =
    std::derived_from<T, CSuitPowerDevice>
    #ifdef VKZ_RESTORABLE_SUIT_POWER_DEVICE
    && RestorableSuitDevice<T>
    #endif
    ;
#endif

//=============================================================================
// Purpose: Data of a HEV suit device that use aux power
// Note that objects should not be default initialized
//=============================================================================
class CSuitPowerDevice
{
public:
    using ThisClass = CSuitPowerDevice;
private:
#ifdef VKZ_INVALID_SUIT_POWER_DEVICE
    // This should only be used to create `CSuitPowerDevice::Invalid`
    consteval CSuitPowerDevice() noexcept
        : m_bitsDeviceID(0), m_flDrainRate(0.0f) {}
#endif
public:
    // NOTE: Prevent to use `bits_SUIT_DEVICE_INVALID` as `bitsID`, because it's reserved for invalid device
    constexpr CSuitPowerDevice(int bitsID, float flDrainRate)
#ifndef VKZ_INVALID_SUIT_POWER_DEVICE
        noexcept
#endif
        : m_bitsDeviceID(bitsID), m_flDrainRate(flDrainRate)
    {
#ifdef VKZ_INVALID_SUIT_POWER_DEVICE
        // You cannot create an invalid suit power device
        // Use `CSuitPowerDevice::Invalid` instead
        Assert(bitsID != bits_SUIT_DEVICE_INVALID);
#endif
    }
protected:
    int m_bitsDeviceID;	// tells what the device is. DEVICE_SPRINT, DEVICE_FLASHLIGHT, etc. BITMASK!!!!!
    float m_flDrainRate; // how quickly does this device deplete suit power? ( percent per second )

public:
#ifdef VKZ_INVALID_SUIT_POWER_DEVICE
    // Invalid suit device that shouldn't be used
    // This serve as a placeholder
    static constinit const CSuitPowerDevice Invalid;

    constexpr virtual bool isValid() const noexcept { return m_bitsDeviceID != bits_SUIT_DEVICE_INVALID; }
#endif

    constexpr virtual bool doesDrainPower() const
#ifndef VKZ_INVALID_SUIT_POWER_DEVICE
        noexcept
#endif
    {
#ifdef VKZ_INVALID_SUIT_POWER_DEVICE
        Assert(isValid());
#endif
        return m_flDrainRate > 0;
    }
    constexpr virtual bool doesChargePower() const
#ifndef VKZ_INVALID_SUIT_POWER_DEVICE
        noexcept
#endif
    {
#ifdef VKZ_INVALID_SUIT_POWER_DEVICE
        Assert(isValid());
#endif
        return m_flDrainRate < 0;
    }
    constexpr virtual bool doesChangePower() const
#ifndef VKZ_INVALID_SUIT_POWER_DEVICE
        noexcept
#endif
    {
        return doesDrainPower() || doesChargePower();
    }

    // Doesn't use this to validate device
    constexpr virtual int GetDeviceID(void) const
#ifndef VKZ_INVALID_SUIT_POWER_DEVICE
        noexcept
#endif
    {
#ifdef VKZ_INVALID_SUIT_POWER_DEVICE
        Assert(isValid());
#endif
        return m_bitsDeviceID;
    }

    constexpr virtual int getRawDrainRate(void) const
#ifndef VKZ_INVALID_SUIT_POWER_DEVICE
        noexcept
#endif
    {
#ifdef VKZ_INVALID_SUIT_POWER_DEVICE
        Assert(isValid());
#endif
        return m_flDrainRate;
    }

    constexpr virtual float GetDeviceDrainRate(void) const
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
    constexpr virtual void SetDeviceDrainRate(float flDrainRate)
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
public:
    // Used by `CSuitPowerDeviceDataOps` to save this object
    DEFINE_DEVICE_SAVE(fieldInfo, pSave) {
        // StartBlock called
        // Order to write matters
        pSave->WriteInt(&m_bitsDeviceID);
        pSave->WriteFloat(&m_flDrainRate);
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
    };
#endif
};

#ifdef VKZ_INFINITE_SPRINT
constexpr float DEFAULT_SPRINT_SPEED = 320;

class CSprintDevice : public CSuitPowerDevice
{
public:
    #ifdef VKZ_ADVANCED_SUIT_POWER_DEVICE
    DECLARE_SUIT_POWER_DEVICE(CSprintDevice, CSuitPowerDevice);
    #else
        #error "VKZ_INFINITE_SPRINT requires VKZ_ADVANCED_SUIT_POWER_DEVICE"
    #endif
    constexpr CSprintDevice(int bitsID, float drainRate, float sprintSpeed = DEFAULT_SPRINT_SPEED)
        : BaseClass(bitsID, drainRate), m_flSprintSpeed(sprintSpeed) {}

    constexpr CSprintDevice(const BaseClass& device, float sprintSpeed = DEFAULT_SPRINT_SPEED)
        : BaseClass(device), m_flSprintSpeed(sprintSpeed) {}
protected:
    float m_flSprintSpeed;
public:
    constexpr virtual float getSprintSpeed() const {
    #ifdef VKZ_INVALID_SUIT_POWER_DEVICE
        Assert(isValid());
    #else
        #error "VKZ_INFINITE_SPRINT requires VKZ_INVALID_SUIT_POWER_DEVICE"
    #endif
        return m_flSprintSpeed;
    }

    constexpr virtual void setSprintSpeed(float speed) {
    #ifdef VKZ_INVALID_SUIT_POWER_DEVICE
        Assert(isValid());
    #else
        #error "VKZ_INFINITE_SPRINT requires VKZ_INVALID_SUIT_POWER_DEVICE"
    #endif
        m_flSprintSpeed = speed;
    }

    #ifdef VKZ_RESTORABLE_SUIT_POWER_DEVICE
public:

    DEFINE_DEVICE_SAVE(fieldInfo, pSave) {
        pSave->WriteFloat(&m_flSprintSpeed);
        BaseClass::WriteSelf(fieldInfo, pSave);
    }

    DEFINE_DEVICE_RESTORE(fieldInfo, pRestore) {
        float sprintSpeed;
        // Careful with ordering
        pRestore->ReadFloat(&sprintSpeed);
        return ThisClass(BaseClass::Read(fieldInfo, pRestore), sprintSpeed);
    };

    #else
        #error "VKZ_INFINITE_SPRINT requires VKZ_RESTORABLE_SUIT_POWER_DEVICE"
    #endif
};
#endif

#ifdef VKZ_RESTORABLE_SUIT_POWER_DEVICE
// VKZ Knowledge (Simple custom field for DATADESC table):

    #ifdef VKZ_ADVANCED_SUIT_POWER_DEVICE
/**
 * @brief
 *	    Class used to save & restore suit power devices
 * @tparam Device
 *	    std::derived_from<Device, CSuitPowerDevice>
 */
template <SuitPowerDevice Device = CSuitPowerDevice>
class CSuitPowerDeviceDataOps : public CDefSaveRestoreOps
    #else
class CSuitPowerDeviceDataOps : public CDefSaveRestoreOps
    #endif
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

        // Using `StartBlock` and `EndBlock` to save and restore values
        // is much like operating a queue

        // Call this to start "queuing" values
        pSave->StartBlock();
        {
            // Use those write methods to push values into "queue"
            device->WriteSelf(fieldInfo, pSave);
        }
        // Call this to indicate you are done with the "queue"
        pSave->EndBlock();
    }

    virtual void Restore(const SaveRestoreFieldInfo_t& fieldInfo, IRestore* pRestore)
    {
        // This is a pointer to the object I need to restore
        const auto device = static_cast<CSuitPowerDevice*>(fieldInfo.pField);

        // Using `StartBlock` and `EndBlock` to save and restore values
        // is much like operating a queue

        // Call this to start reading values from "queue"
        pRestore->StartBlock();
        {
#ifdef VKZ_ADVANCED_SUIT_POWER_DEVICE
            *device = Device::Read(fieldInfo, pRestore);
#else
            *device = CSuitPowerDevice::Read(fieldInfo, pRestore);
#endif
        }
        // Call this to indicate you are done with the "queue"
        pRestore->EndBlock();
    }

    // Make the object empty. I think this will be called before restoration
    virtual void MakeEmpty(const SaveRestoreFieldInfo_t& fieldInfo)
    {
        // This is a pointer to the object I need to make empty
        const auto device = static_cast<CSuitPowerDevice*>(fieldInfo.pField);
    #ifdef VKZ_INVALID_SUIT_POWER_DEVICE
        *device = CSuitPowerDevice::Invalid;
    #else
        #error "VKZ_RESTORABLE_SUIT_POWER_DEVICE requires VKZ_INVALID_SUIT_POWER_DEVICE"
    #endif
    }

    // The only thing I know about this function is that
    // if this returns true for the object, it won't be saved
    virtual bool IsEmpty(const SaveRestoreFieldInfo_t& fieldInfo)
    {
        // This is a pointer to the object I need to check
        const auto device = static_cast<const CSuitPowerDevice*>(fieldInfo.pField);
    #ifdef VKZ_INVALID_SUIT_POWER_DEVICE
        return !device->isValid();
    #else
        #error "VKZ_RESTORABLE_SUIT_POWER_DEVICE requires VKZ_INVALID_SUIT_POWER_DEVICE"
    #endif
    }
};

    #ifdef VKZ_ADVANCED_SUIT_POWER_DEVICE
template<SuitPowerDevice Device>
constinit CSuitPowerDeviceDataOps<Device> CSuitPowerDeviceDataOps<Device>::_instance = CSuitPowerDeviceDataOps<Device>();
    #endif

#endif

#endif
