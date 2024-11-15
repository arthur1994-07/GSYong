#include "pch.h"

#include "./DxReplaceContainer.h"
#include "../../../SigmaCore/File/SerialFile.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void DXPIECEOBJ::SaveFile ( sc::SerialFile &SFile )
{
	SFile << fCurTime;

	if ( szFileName )
	{
		SFile << BOOL ( TRUE );
		int	StrLength = static_cast<int>( strlen ( szFileName ) + 1 );
		
		SFile << StrLength;
		SFile.WriteBuffer ( szFileName, sizeof ( char ) * StrLength );
	}
	else
	{
		SFile << BOOL ( FALSE );
	}

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

	SFile.WriteBuffer ( &matComb, sizeof ( D3DXMATRIX ) );

	if ( pNext )
	{
		SFile << BOOL ( TRUE );
		pNext->SaveFile ( SFile );
	}
	else
	{
		SFile << BOOL ( FALSE );
	}	
}
/*
void	DXPIECEOBJNODE::SaveFile ( sc::SerialFile &SFile )
{
    if ( pPieceObj )
	{
		SFile << BOOL ( TRUE );
		char	*pString = pPieceObj->szFrameName;
		if ( pString )
		{
			SFile << BOOL ( TRUE );
			int	StrLength = strlen ( pString ) + 1;
			SFile.WriteBuffer ( pString, StrLength );
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

	SFile.WriteBuffer ( &vMax, sizeof ( D3DXVECTOR3 ) );
	SFile.WriteBuffer ( &vMin, sizeof ( D3DXVECTOR3 ) );

	if ( pLeftChild )
	{
		SFile << BOOL ( TRUE );
		pLeftChild->SaveFile ( SFile );
	}
	else
	{
		SFile << BOOL ( FALSE );
	}

    if ( pRightChild )
	{
		SFile << BOOL ( TRUE );
		pRightChild->SaveFile ( SFile );
	}
	else
	{
		SFile << BOOL ( FALSE );
	}
}*/