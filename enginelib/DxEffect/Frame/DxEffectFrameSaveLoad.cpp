#include "pch.h"

#include "./DxEffectFrame.h"
#include "../../../SigmaCore/File/SerialFile.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void	DxEffectBase::SaveFile ( sc::SerialFile& SFile )
{
	if ( GetAdaptFrameName() )
	{
		SFile << BOOL ( TRUE );
		int		StrLength = (int)strlen ( GetAdaptFrameName () ) + 1;
		SFile << StrLength;
		SFile.WriteBuffer ( GetAdaptFrameName(), sizeof ( char ) * StrLength );
	}
	else
	{
		SFile << BOOL ( FALSE );
	}

    SFile << GetTypeID();

	
	BYTE	*pProp;
	DWORD	dwSize;
	DWORD	dwVer;
	GetProperty ( pProp, dwSize, dwVer );

	SFile << dwVer;
	SFile << dwSize;
	if ( dwSize )
	{
		SFile.WriteBuffer ( pProp, sizeof ( BYTE ) * dwSize );
	}

	//	<--	���� Ŭ���� ���
	if ( IsUseAffineMatrix() )
	{
		SFile << BOOL ( TRUE );
		SFile.WriteBuffer ( m_pAffineParts, sizeof ( DXAFFINEPARTS ) );
	}
	else
	{
		SFile << BOOL ( FALSE );
	}    
	//	-->	���� Ŭ���� ���

	//	Note : SaveBuffer(), Ư�� Effect ������ ���Ǵ� �޸� ���۸� �����Ѵ�.
	//
	SaveBuffer ( SFile );
}