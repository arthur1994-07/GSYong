#include "pch.h"
#include "./DxEffectFrame.h"
#include "../../../SigmaCore/File/SerialFile.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


void DxEffectBase::SaveBuffer ( sc::SerialFile &SFile )
{
	SFile << DWORD(0);
}

void DxEffectBase::LoadBufferSet ( sc::SerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD Size=0;
	SFile >> Size;
}

void DxEffectBase::LoadBuffer ( sc::SerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD Size=0;
	SFile >> Size;
}

void DxEffectBase::SaveSet ( sc::SerialFile &SFile )
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


	//	Note : ����Ʈ�� ���� ��� ���� ����.
	//
	if ( pEffectNext )
	{
		SFile << BOOL ( TRUE );
		pEffectNext->SaveSet ( SFile );
	}
	else
	{
		SFile << BOOL ( FALSE );
	}
}