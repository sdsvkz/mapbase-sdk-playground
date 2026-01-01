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

#ifdef VKZ_RESTORABLE_SUIT_POWER_DEVICE

// `WriteSelf` function signature for device classes
#define DEFINE_DEVICE_SAVE(fieldInfoParamName, pSaveParamName) \
    virtual void WriteSelf( const SaveRestoreFieldInfo_t& fieldInfoParamName, ISave* pSaveParamName ) const

#define DEFINE_DEVICE_SAVE_WITH_NAME(funcName, fieldInfoParamName, pSaveParamName) \
    virtual void funcName( const SaveRestoreFieldInfo_t& fieldInfoParamName, ISave* pSaveParamName ) const

// `Read` function signature for device classes
#define DEFINE_DEVICE_RESTORE(fieldInfoParamName, pRestoreParamName) \
    static inline ThisClass Read( const SaveRestoreFieldInfo_t& fieldInfoParamName, IRestore* pRestoreParamName )

#define DEFINE_DEVICE_RESTORE_WITH_NAME(funcName, fieldInfoParamName, pRestoreParamName) \
    static inline ThisClass funcName( const SaveRestoreFieldInfo_t& fieldInfoParamName, IRestore* pRestoreParamName )

#endif

#ifdef CLIENT_DLL
class C_SuitPowerDevice;
#ifdef VKZ_ADVANCED_SPRINT
class C_SprintDevice;
#endif
#else
class CSuitPowerDevice;
#ifdef VKZ_ADVANCED_SPRINT
class CSprintDevice;
#endif
#endif

namespace SuitPowerDevice {

#ifdef CLIENT_DLL
    using BaseDevice = C_SuitPowerDevice;
#ifdef VKZ_ADVANCED_SPRINT
    using SprintDevice = C_SprintDevice;
#endif
#else
    using BaseDevice = CSuitPowerDevice;
#ifdef VKZ_ADVANCED_SPRINT
    using SprintDevice = CSprintDevice;
#endif
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

    virtual bool isSameCategory(const ThisClass& device) const {
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
    void WriteSelfExceptId(const SaveRestoreFieldInfo_t& fieldInfo, ISave* pSave) const {
        // StartBlock called
        const auto& drainRate = static_cast<const float&>(m_flDrainRate);
        pSave->WriteFloat(&drainRate);
        // EndBlock called
    }

    static inline std::tuple<float> ReadExceptId(const SaveRestoreFieldInfo_t& fieldInfo, IRestore* pRestore) {
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
    const Device& deviceCast(const SuitPowerDevice::BaseDevice& device) {
        try {
            return dynamic_cast<const Device&>(device);
        } catch (const std::bad_cast& e) {
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
    Device& deviceCast(SuitPowerDevice::BaseDevice& device) {
        return const_cast<Device&>(deviceCast<Device>(std::as_const(device)));
    }
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
 * @brief HEV Suit sprint device controls how fast the user run
 * 
 * Note that you still need to set `sv_maxspeed`
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

#ifndef VKZ_NETWORKABLE_SUIT_POWER_DEVICE
    #error "VKZ_ADVANCED_SPRINT requires VKZ_NETWORKABLE_SUIT_POWER_DEVICE"
#endif
    DECLARE_EMBEDDED_NETWORKVAR();

    C_SprintDevice()
        : ThisClass(Default) {}

    C_SprintDevice(float drainRate, float sprintSpeed = DEFAULT_SPRINT_SPEED)
        : BaseClass(bits_SUIT_DEVICE_SPRINT, drainRate), m_flSprintSpeed(sprintSpeed) {}

protected:
    float m_flSprintSpeed;

#else

    DECLARE_CLASS(CSprintDevice, CSuitPowerDevice);

#ifdef VKZ_DATADESC_SUIT_POWER_DEVICE
    DECLARE_DATADESC();
#endif

#ifndef VKZ_NETWORKABLE_SUIT_POWER_DEVICE
    #error "VKZ_ADVANCED_SPRINT requires VKZ_NETWORKABLE_SUIT_POWER_DEVICE"
#endif
    DECLARE_EMBEDDED_NETWORKVAR();

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

    // For a sprint device, returns `true` only if device `id == bits_SUIT_DEVICE_SPRINT`
    virtual bool isValid() const { return m_bitsDeviceID & bits_SUIT_DEVICE_SPRINT; }

    bool equals(const ThisClass& device) const {
        return BaseClass::equals(device) && m_flSprintSpeed == device.m_flSprintSpeed;
    }

    virtual void toString(std::unique_ptr<char[]>& pDest) const {
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

    virtual float getSprintSpeed() const
    {
        validate();
        return m_flSprintSpeed;
    }

    virtual void setSprintSpeed(float speed) {
        m_flSprintSpeed = speed;
    }

    virtual void resetDrainRate() {
        setSprintSpeed(DEFAULT_SPRINT_DRAIN_RATE);
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
		const auto& baseDeviceData = BaseClass::ReadExceptId(fieldInfo, pRestore);
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

#if !defined(CLIENT_DLL) && defined(VKZ_RESTORABLE_SUIT_POWER_DEVICE)
// VKZ Knowledge (Simple custom field for DATADESC table):
// Inherit `ISaveRestoreOps` to define your logic for save & restore the data
// You need to override those virtual functions:
// 
// Let
// - `Data` be the data type you need to save & restore
// - `Entity` be a class contains a `Data` field `m_data`
// - `entity` be an instance of `Entity`
// 
// `fieldInfo.pField` is a pointer points to `entity.m_data`
// 
// `Save` defines how `m_data` is saved
// `entity.m_data`: the value right before saving
// 
// `Restore` defines how your object is restored
// `entity.m_data`: the value after processed by `MakeEmpty`
// 
// `IsEmpty` defines the meaning of empty.
// I think if this return true for the object, it'll not be saved.
// `entity.m_data`: the value right before saving
// DO NOT MODIFY `entity.m_data`
// Because `fieldInfo.pField` for the call is actually `const_cast`ed from `const void *`
// Well, actually, I think it's originally a `void *`.
// So, maybe you can modify `entity.m_data`,
// the only thing is, the value for `Save` probably will be the modified one
//
// `MakeEmpty` creates an empty object.
// I think this will be used to clear `fieldInfo.pField` before restoring object
// `entity.m_data`: the value right after initialized by `Entity` constructor
// 
// `Parse` directly decode the object from string.
// I've never used this one, so I have no idea how to implement this
// I guess if this returns `false` then it'll fallback to `Restore`?
// 
// You can also inherit `CDefSaveRestoreOps` or `CClassPtrSaveRestoreOps`
// - `CDefSaveRestoreOps` defined `Parse` and `IsEmpty` that always returns `false`
// - `CClassPtrSaveRestoreOps` inherits `CDefSaveRestoreOps`, and defines `IsEmpty` and `MakeEmpty` for pointer to the `Data`
//
// If you inherit `CClassPtrSaveRestoreOps`, you gonna need to declare `m_data` as `Data *`
// The empty value is null pointer, so you don't need to define any placeholder value for your data.
// 
// In the other hand, `CDefSaveRestoreOps` do need a placeholder value for `MakeEmpty`,
// or you can leave `entity.m_data` untouched (Then for `Restore`, it'll be the value initialized by `Entity` constructor
// Valve did this in `CVariantSaveDataOps`, but honestly, I'm not sure whether it is safe to do so).
// 
// Also note that, with `CClassPtrSaveRestoreOps`, you can make members of your data `const`.
// What you need to do is just delete the old object if it exists and allocate a new one.
// However, with `CDefSaveRestoreOps`, since you need to directly modify your data object,
// you probably don't want to make any of them `const`. Otherwise, you need to ensure `const` fields
// will be initialized correctly in `Entity` constructor
// 
// I use `CDefSaveRestoreOps` because I don't know how to networking a pointer
//

#ifndef VKZ_NEVER_DEFINE

/**
 * @brief
 *	    Class used to save & restore suit power devices
 * @tparam Device
 *	    std::derived_from<Device, CSuitPowerDevice>
 */
template <SuitPowerDevice::SuitPowerDevice Device = CSuitPowerDevice>
class CSuitPowerDeviceDataOps : public CDefSaveRestoreOps
{
private:

    constexpr CSuitPowerDeviceDataOps() = default;

    // Global object for save & restore `Device`
    static constinit CSuitPowerDeviceDataOps _instance;

    // TIPS:
    // You can declare any object that needed for this process here as member
    // Then inject them with constructor
public:
    static inline CSuitPowerDeviceDataOps* Get() {
        return &_instance;
    }

    // It's not a class defining pure functions to encode or decode your object,
    // but rather directly performing side effects to save and restore a field of the object
    // 
    // For encoding, you have to do side effects with utilities provided by `pSave` or other objects
    // For decoding, simply do side effects like mutating `fieldInfo.pField`, which points to your object

    virtual void Save(const SaveRestoreFieldInfo_t& fieldInfo, ISave* pSave)
        override
    {
        // This is a pointer to the object you need to save
        const auto pDevice = static_cast<const Device*>(fieldInfo.pField);

        // Using `StartBlock` and `EndBlock` to save and restore values
        // is much like operating a queue

        // Call this to start "queuing" values
        pSave->StartBlock();
        {
            // Use those write methods to push values into "queue"
            pDevice->WriteSelf(fieldInfo, pSave);
        }
        // Call this to indicate you are done with the "queue"
        pSave->EndBlock();
    }

    virtual void Restore(const SaveRestoreFieldInfo_t& fieldInfo, IRestore* pRestore)
        override
    {
        // This is a pointer to the object I need to restore
        const auto pDevice = static_cast<Device*>(fieldInfo.pField);

        // Using `StartBlock` and `EndBlock` to save and restore values
        // is much like operating a queue

        // Call this to start "popping" values
        pRestore->StartBlock();
        {
            // Use those read methods to push values into "queue"
            *pDevice = Device::Read(fieldInfo, pRestore);
        }
        // Call this to indicate you are done with the "queue"
        pRestore->EndBlock();
    }

    virtual bool IsEmpty(const SaveRestoreFieldInfo_t& fieldInfo)
        override
    {
        const auto pDevice = static_cast<const Device*>(fieldInfo.pField);
        // Don't save invalid device
        return !pDevice->isValid();
    }

    virtual void MakeEmpty(const SaveRestoreFieldInfo_t& fieldInfo)
        override
    {
        // It's already a good value for restoring
    }
};

template <SuitPowerDevice::SuitPowerDevice Device>
constinit CSuitPowerDeviceDataOps<Device> CSuitPowerDeviceDataOps<Device>::_instance {};

#else

template <SuitPowerDevice Device = CSuitPowerDevice>
class CSuitPowerDeviceDataOps : public CClassPtrSaveRestoreOps
{
private:

    constexpr CSuitPowerDeviceDataOps() = default;

    static constinit CSuitPowerDeviceDataOps _instance;

public:
    static inline CSuitPowerDeviceDataOps* Get() {
        return &_instance;
    }

    virtual void Save(const SaveRestoreFieldInfo_t& fieldInfo, ISave* pSave)
    {
        const auto pDevice = *static_cast<const Device**>(fieldInfo.pField);

        if (pDevice == nullptr) {
            return;
        }

        pSave->StartBlock();
        {
            pDevice->WriteSelf(fieldInfo, pSave);
        }
        pSave->EndBlock();
    }

    virtual void Restore(const SaveRestoreFieldInfo_t& fieldInfo, IRestore* pRestore)
    {
        const auto pDevice = static_cast<Device**>(fieldInfo.pField);

        pRestore->StartBlock();
        {
            *pDevice = new Device(std::move(Device::Read(fieldInfo, pRestore)));
        }
        pRestore->EndBlock();
    }
};

#endif

#endif

#endif // SUIT_POWER_DEVICE_H
