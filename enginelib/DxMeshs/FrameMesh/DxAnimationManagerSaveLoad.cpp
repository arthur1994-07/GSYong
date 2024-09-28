#include "pch.h"

#include "../../DxMeshs/FrameMesh/DxAnimationManager.h"
#include "DxFrameMesh.h"
#include "../../../SigmaCore/File/SerialFile.h"
#include "../../DxLand/DxOctree/DxOctree.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

BOOL	DxAnimationMan::SaveFile ( sc::SerialFile &SFile )
{
	SFile << VERSION;

	SFile.WriteBuffer ( &m_AniType, sizeof ( ANIMATETYPE ) );
	SFile << fCurTime;

	if ( m_pDxFrameRoot )
	{
		SFile << BOOL ( TRUE );
		m_pDxFrameRoot->SaveFile ( SFile );
	}
	else
	{
		SFile << BOOL ( FALSE );
	}
	
	if ( m_pDxAnimationHead )
	{
		SFile << BOOL ( TRUE );
		m_pDxAnimationHead->SaveFile ( SFile );
	}
	else
	{
		SFile << BOOL ( FALSE );
	}

	if ( m_pNext )
	{
		SFile << BOOL ( TRUE );
		m_pNext->SaveFile ( SFile );
	}
	else
	{
		SFile << BOOL ( FALSE );
	}
	
	return TRUE;
}

BOOL	DxAnimationMan::LoadFile_NEW( sc::SerialFile &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	BOOL	bExist = FALSE;

	DWORD dwVer;
	SFile >> dwVer;

	SFile.ReadBuffer ( &m_AniType, sizeof ( ANIMATETYPE ) );
	SFile >> fCurTime;

	SFile >> bExist;
	if ( bExist )
	{
		m_pDxFrameRoot = new DxFrame;
		m_pDxFrameRoot->LoadFile_NEW ( SFile, pd3dDevice, m_pEffectHead, m_pDxAlphaFrameHead );
	}

	SFile >> bExist;
	if ( bExist )
	{
		m_pDxAnimationHead = new DxAnimation;
		m_pDxAnimationHead->LoadFile ( SFile, this );
	}

	SFile >> bExist;
	if ( bExist )
	{
		m_pNext = new DxAnimationMan;
		m_pNext->LoadFile_NEW ( SFile, pd3dDevice );
	}

	return TRUE;
}

BOOL	DxAnimationMan::LoadFile_OLD( sc::SerialFile &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	BOOL	bExist = FALSE;

	SFile.ReadBuffer ( &m_AniType, sizeof ( ANIMATETYPE ) );
	SFile >> fCurTime;

	SFile >> bExist;
	if ( bExist )
	{
		m_pDxFrameRoot = new DxFrame;
		m_pDxFrameRoot->LoadFile_OLD ( SFile, pd3dDevice, m_pEffectHead, m_pDxAlphaFrameHead );
	}

	SFile >> bExist;
	if ( bExist )
	{
		m_pDxAnimationHead = new DxAnimation;
		m_pDxAnimationHead->LoadFile ( SFile, this );
	}

	SFile >> bExist;
	if ( bExist )
	{
		m_pNext = new DxAnimationMan;
		m_pNext->LoadFile_OLD ( SFile, pd3dDevice );
	}
	
	return TRUE;
}