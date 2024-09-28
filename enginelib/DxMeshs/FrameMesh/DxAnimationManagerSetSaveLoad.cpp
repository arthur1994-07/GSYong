#include "pch.h"
#include "DxFrameMesh.h"
#include "../../DxMeshs/FrameMesh/DxAnimationManager.h"
#include "../../../SigmaCore/File/SerialFile.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

DWORD	DxAnimationMan::GetSaveSetSize ()
{
	//	<--	노드의 크기를 byte단위로 계산한다.
	size_t	NODESIZE = 0;
	NODESIZE += sizeof ( ANIMATETYPE );		//	m_AniType
	NODESIZE += sizeof ( BOOL );			//	if ( m_pDxFrameRoot )
	if ( m_pDxFrameRoot )
	{
		NODESIZE += sizeof ( BOOL );		//	if ( m_pDxRoot->szName )
		if ( m_pDxFrameRoot->szName )
		{
			NODESIZE += sizeof ( int );		//	StrLength
			NODESIZE += sizeof ( char ) * ( strlen ( m_pDxFrameRoot->szName ) + 1 );
		}
	}
	//	-->	노드의 크기를 byte단위로 계산한다.

	return static_cast<DWORD>( NODESIZE );
}

void	DxAnimationMan::SaveSet ( sc::SerialFile &SFile )
{
	SFile << m_dwVERSION;
	SFile << GetSaveSetSize ();

	SFile.WriteBuffer ( &m_AniType, sizeof ( ANIMATETYPE ) );
	if ( m_pDxFrameRoot )
	{
		SFile << BOOL ( TRUE );
		if ( m_pDxFrameRoot->szName )
		{
			SFile << BOOL ( TRUE );
			int	StrLength = static_cast<int>( strlen ( m_pDxFrameRoot->szName ) + 1 );
			SFile << StrLength;
            SFile.WriteBuffer ( m_pDxFrameRoot->szName, sizeof ( char ) * StrLength );
		}
		else
		{
			SFile << BOOL ( FALSE );
		}
	}
	else
	{
		SFile << BOOL ( FALSE );
	}

	if ( m_pNext )
	{
		SFile << BOOL ( TRUE );
		m_pNext->SaveSet ( SFile );
	}
	else
	{
		SFile << BOOL ( FALSE );
	}
}