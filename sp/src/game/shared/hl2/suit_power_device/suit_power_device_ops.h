#ifndef SUIT_POWER_DEVICE_OPS_H
#define SUIT_POWER_DEVICE_OPS_H
#ifdef _WIN32
#pragma once
#endif

#if !defined(CLIENT_DLL) && defined(VKZ_RESTORABLE_SUIT_POWER_DEVICE)

#include "cbase.h"
#include "isaverestore.h"
#include "suit_power_device.h"

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
constinit CSuitPowerDeviceDataOps<Device> CSuitPowerDeviceDataOps<Device>::_instance{};

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

#endif // SUIT_POWER_DEVICE_OPS_H
