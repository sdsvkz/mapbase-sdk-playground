//=============================================================================//
//
// Undefine macros that defined by suit_power_device_defs.h.
//
//=============================================================================//
#ifdef CLIENT_DLL
#undef RecvPropsDevice
#else
#undef SendPropsDevice
#endif

#ifdef VKZ_RESTORABLE_SUIT_POWER_DEVICE
#undef DEFINE_DEVICE_SAVE
#undef DEFINE_DEVICE_SAVE_WITH_NAME
#undef DEFINE_DEVICE_RESTORE
#undef DEFINE_DEVICE_RESTORE_WITH_NAME
#endif
