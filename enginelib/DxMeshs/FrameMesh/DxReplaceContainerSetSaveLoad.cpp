#include "pch.h"
#include "DxFrameMesh.h"
#include "DxReplaceContainer.h"
#include "../../../SigmaCore/File/SerialFile.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

DWORD	DXPIECEOBJ::GetSaveSetSize ()
{	
	size_t	NODESIZE = 0;
	NODESIZE += sizeof ( float );	//	fCurTime;
	NODESIZE += sizeof ( BOOL );	//	if ( szFileName ) BOOL
	if ( szFrameName )
	{
        NODESIZE += sizeof ( int );	//	StrLength;
        NODESIZE += sizeof ( char ) * ( strlen ( szFrameName ) + 1 );
	}

	NODESIZE += sizeof ( BOOL );	//	if ( pReplacePiece )
	if ( pReplacePiece )
	{
		NODESIZE += sizeof ( BOOL );	//	if ( pReplacePiece->szFileName )
		if ( pReplacePiece->szFileName )
		{
			NODESIZE += sizeof ( int );
			NODESIZE += sizeof ( char ) * ( strlen ( pReplacePiece->szFileName ) + 1 );
		}
	}

	return static_cast<DWORD>( NODESIZE );
}

void	DXPIECEOBJ::SaveSet ( sc::SerialFile &SFile )
{
	SFile << m_dwVERSION;
	SFile << GetSaveSetSize ();

	SFile << fCurTime;

	if ( szFrameName )
	{
		SFile << BOOL ( TRUE );
		int StrLength = static_cast<int>( strlen ( szFrameName ) + 1 );
		SFile << StrLength;
		SFile.WriteBuffer ( szFrameName, sizeof ( char ) * StrLength );
	}
	else
	{
		SFile << BOOL ( FALSE );
	}

	if ( pReplacePiece )
	{
		SFile << BOOL ( TRUE );
		
		if ( pReplacePiece->szFileName )
		{
			SFile << BOOL ( TRUE );
			int	StrLength = static_cast<int>( strlen ( pReplacePiece->szFileName ) + 1 );
			SFile << StrLength;
			SFile.WriteBuffer ( pReplacePiece->szFileName, sizeof ( char ) * StrLength );
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

	if ( pNext )
	{
		SFile << BOOL ( TRUE );
		pNext->SaveSet ( SFile );
	}
	else
	{
		SFile << BOOL ( FALSE );
	}
}