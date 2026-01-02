//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef HL2_PLAYERLOCALDATA_H
#define HL2_PLAYERLOCALDATA_H
#ifdef _WIN32
#pragma once
#endif

#include "networkvar.h"

#include "hl_movedata.h"

//-----------------------------------------------------------------------------
// Purpose: Player specific data for HL2 ( sent only to local player, too )
//-----------------------------------------------------------------------------
class CHL2PlayerLocalData
{
public:
	// Save/restore
	DECLARE_SIMPLE_DATADESC();
	DECLARE_CLASS_NOBASE( CHL2PlayerLocalData );
	DECLARE_EMBEDDED_NETWORKVAR();

	CHL2PlayerLocalData();

	CNetworkVar( float, m_flSuitPower );
	CNetworkVar( bool,	m_bZooming );
	
	// VKZ Knowledge (Active Suit Device Bitmask):

	// This is a bitmask representing which suit devices are active.
	// Operations:
	// `m_bitsActiveDevices & x` returns 0 if device x is off, non-0 if on
	// `m_bitsActiveDevices |= x` means turn on device with id `x`
	// `m_bitsActiveDevices &= ~x` means turn off device with id `x`
	// `m_bitsActiveDevices ^= ~x` means toggle device with id `x`
	// Special values:
	// 0 means all device is off
	// `m_bitsActiveDevices == x` is true if only device with id `x` is on
	CNetworkVar( int,	m_bitsActiveDevices );

	CNetworkVar( int,	m_iSquadMemberCount );
	CNetworkVar( int,	m_iSquadMedicCount );
	CNetworkVar( bool,	m_fSquadInFollowMode );
	CNetworkVar( bool,	m_bWeaponLowered );
	CNetworkVar( EHANDLE, m_hAutoAimTarget );
	CNetworkVar( Vector, m_vecAutoAimPoint );
	CNetworkVar( bool,	m_bDisplayReticle );
	CNetworkVar( bool,	m_bStickyAutoAim );
	CNetworkVar( bool,	m_bAutoAimTarget );
#ifdef HL2_EPISODIC
	CNetworkVar( float, m_flFlashBattery );
	CNetworkVar( Vector, m_vecLocatorOrigin );
#endif

	// Ladder related data
	CNetworkVar( EHANDLE, m_hLadder );
	LadderMove_t			m_LadderMove;
};

EXTERN_SEND_TABLE(DT_HL2Local);


#endif // HL2_PLAYERLOCALDATA_H
