# VKZ Knowledge

My notes of some general concepts or code snippets.

They are interpretations of valve's code.
I've experimented a lot on most of them and use tentative words for uncertain parts.
Still, They are totally possible to be proven wrong.

## What does `CNetworkVar` macros do:

This macro is used to define a network variable.
It hijacking assignment of the type to notify state changes, so that the server knows when to send changes client.

### `CNetworkVar`

```c++
// Helper
#define NETWORK_VAR_START( type, name ) \
	class NetworkVar_##name; \
	friend class NetworkVar_##name; \
	typedef ThisClass MakeANetworkVar_##name; \
	class NetworkVar_##name \
	{ \
	public: \
		template <typename T> friend int ServerClassInit(T *);

// Helper
#define NETWORK_VAR_END( type, name, base, stateChangedFn ) \
	public: \
		static inline void NetworkStateChanged( void *ptr ) \
		{ \
			CHECK_USENETWORKVARS ((ThisClass*)(((char*)ptr) - MyOffsetOf(ThisClass,name)))->stateChangedFn( ptr ); \
		} \
	}; \
	base< type, NetworkVar_##name > name;

#define CNetworkVar( type, name ) \
    NETWORK_VAR_START( type, name ) \
    NETWORK_VAR_END( type, name, CNetworkVarBase, NetworkStateChanged )
```

`CNetworkVar`:
1. defines a `Changer` for the type
2. declares the member as a `CNetworkVarBase< type, Changer >`

`CNetworkVarBase<Type, Changer>` is basically a wrapper class for `Type`.
It's a type with default constructor.
You can check out `NetworkVarConstruct` to see how it initialize the value of wrapped `Type`.
Here is the initialization for all types except some Valve-defined types.

```c++
template<typename T>
FORCEINLINE void NetworkVarConstruct( T &x ) { x = T(0); }
```

`CNetworkVarBase<Type, Changer>` will use `Changer::NetworkStateChanged( this )` to notify changes.
It overrides the assignment operator to notify for every changes:

```c++
template< class Type, class Changer >
class CNetworkVarBase
{
public:
    // ...
    template< class C >
    const Type& operator=( const C &val ) 
    { 
	    return Set( ( const Type )val ); 
    }
    // ...
    const Type& Set( const Type &val )
    {
	    if ( memcmp( &m_Value, &val, sizeof(Type) ) )
	    {
		    NetworkStateChanged();
		    m_Value = val;
	    }
	    return m_Value;
    }
    // ...
protected:
	inline void NetworkStateChanged()
	{
		Changer::NetworkStateChanged( this );
	}
}
```

There is some useful information about using network variables in the VDC article ["Networking Entities"](https://developer.valvesoftware.com/wiki/Networking_Entities#Networking_entities)

If `Type` is a class or struct, `CNetworkVarBase` won't able to hijack assignment to its fields.
You will need to use `CNetworkVarEmbedded`.

### `CNetworkVarEmbedded`

Used to declare embedded network variables.

#### Usage

If you want to transmit a custom data type `T`:
1. Use `DECLARE_EMBEDDED_NETWORKVAR` in both the server-side and the client-side `T` to declare required components.
2. Declare the field you want to networking as network variable with `CNetworkVar` **on server-side**, declare them normally on client-side
3. Implement send and receive table respectively.

Here is a complete example:

```c++
// suit_power_device.h (Shared)

#ifdef CLIENT_DLL
class C_SprintDevice : public C_SuitPowerDevice
#else
class CSprintDevice : public CSuitPowerDevice
#endif
{
public:
#ifdef CLIENT_DLL
    DECLARE_CLASS(C_SprintDevice, C_SuitPowerDevice);
#else
    DECLARE_CLASS(CSprintDevice, CSuitPowerDevice);
#endif

    DECLARE_EMBEDDED_NETWORKVAR();

    // ...

protected:
#ifdef CLIENT_DLL
    float m_flSprintSpeed;
#else
    CNetworkVar(float, m_flSprintSpeed);
#endif
public:
    // ...
}
```

```c++
// suit_power_device.cpp

// A downside of using embedded network variable is that
// you cannot derive networking tables from base class.
// Honestly, I think it should be valid. Am I did something wrong?
#ifdef CLIENT_DLL
BEGIN_RECV_TABLE_NOBASE(SprintDeviceClass, DT_SprintDevice)
	RecvPropDevice(), // Macro to send data of base device
	RecvPropFloat(RECVINFO(m_flSprintSpeed)),
END_RECV_TABLE()
#else
BEGIN_SEND_TABLE_NOBASE(SprintDeviceClass, DT_SprintDevice)
	SendPropDevice(), // Macro to send data of base device
	SendPropFloat(SENDINFO(m_flSprintSpeed), -1, SPROP_COORD),
END_SEND_TABLE()
#endif
```

```c++
// hl2_player.h (Server)

class CHL2_Player : public CBasePlayer
{
    // ...
private:
    // Member definition
    CNetworkVarEmbedded(CSprintDevice, m_SprintDevice);         // The sprint device player should CHECK_USENETWORKVARS
    // ...
}
```

```c++
// hl2_player.cpp

IMPLEMENT_SERVERCLASS_ST(CHL2_Player, DT_HL2_Player)
    // ...
    // Don't send to other player
    SendPropDataTable(SENDINFO_DT(m_SprintDevice), &REFERENCE_SEND_TABLE(DT_SprintDevice), SendProxy_SendLocalDataTable),
    // ...
END_SEND_TABLE()
```

```c++
// c_basehlplayer.h (Client)

class C_BaseHLPlayer : public C_BasePlayer
{
    // ...
public:
    C_SprintDevice      m_SprintDevice;
    // ...
}
```

```c++
// c_basehlplayer.cpp

IMPLEMENT_CLIENTCLASS_DT(C_BaseHLPlayer, DT_HL2_Player, CHL2_Player)
    // ...
    RecvPropDataTable(RECVINFO_DT(m_SprintDevice),0, &REFERENCE_RECV_TABLE(DT_SprintDevice)),
    // ...
END_RECV_TABLE()
```

#### Explanation

```c++
// Assignment operator is disabled because it doesn't call copy constructors of scalar types
// within the aggregate, so they are not marked changed
#define CNetworkVarEmbedded( type, name ) \
	class NetworkVar_##name; \
	friend class NetworkVar_##name; \
	static inline int GetOffset_##name() { return MyOffsetOf(ThisClass,name); } \
	typedef ThisClass ThisClass_##name; \
	class NetworkVar_##name : public type \
	{ \
		template< class T > NetworkVar_##name& operator=( const T &val ) { *((type*)this) = val; return *this; } \
	public: \
		void CopyFrom( const type &src ) { *((type *)this) = src; NetworkStateChanged(); } \
		virtual void NetworkStateChanged() \
		{ \
			DispatchNetworkStateChanged( (ThisClass_##name*)( ((char*)this) - GetOffset_##name() ) ); \
		} \
		virtual void NetworkStateChanged( void *pVar ) \
		{ \
			DispatchNetworkStateChanged( (ThisClass_##name*)( ((char*)this) - GetOffset_##name() ), pVar ); \
		} \
	}; \
	NetworkVar_##name name; 

```

`CNetworkVarEmbedded` doesn't use `CNetworkVarBase`. It directly defines a new class derived from the type.
This class also serve as the same purpose as `CNetworkVarBase`.
Suppose:
1. Your data type is `T`.
2. There is a field `CNetworkVar(float, m_flSprintSpeed)` in `T`.
3. There is a embedded field `CNetworkVarEmbedded(T, m_data)` in a class.

When you assign to `m_flSprintSpeed`. `T` will notify changes
(I don't know whether it'll send the whole object or just that value).

You can't use assignment operator of `T`, because derived class disabled copy assignment operator.
Use `CopyFrom` method instead. It's essentially the copy assignment operator of `T`, but notify changes when called.

