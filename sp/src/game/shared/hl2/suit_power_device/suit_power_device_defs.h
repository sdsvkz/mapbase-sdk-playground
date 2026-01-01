//=============================================================================//
//
// Define macros that related to suit power device.
// Use suit_power_device_undefs.h to undefine macros.
// This header should be included at top level since it also includes necessary headers
//
//=============================================================================//
#ifdef VKZ_NETWORKABLE_SUIT_POWER_DEVICE
#include "dt_common.h"
#ifdef CLIENT_DLL
#include "dt_recv.h"
#else
#include "dt_send.h"
#endif

#ifdef CLIENT_DLL
#define RecvPropsDevice() \
	RecvPropFloat(RECVINFO(m_flDrainRate))
#else
#define SendPropsDevice() \
	SendPropFloat(SENDINFO(m_flDrainRate), -1, SPROP_COORD)
#endif
#endif

#ifdef VKZ_RESTORABLE_SUIT_POWER_DEVICE
#include "isaverestore.h"

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
