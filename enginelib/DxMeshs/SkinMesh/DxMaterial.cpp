#include "pch.h"
#include "./DxMaterial.h"
#include "../../DxTools/TextureManager.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void DXMATERIAL_CHAR_EFF::SaveFile( sc::BaseStream &SFile )
{
    SFile << static_cast<DWORD>( m_emType );
    SFile.WriteBuffer( &d3dMaterial, sizeof( d3dMaterial ) );

    SFile << DWORD( 1 );

    SFile.WriteBuffer( szTexture, sizeof( char ) * MAX_PATH );
    SFile.WriteBuffer( szEffTex, sizeof( char ) * MAX_PATH );
}

void DXMATERIAL_CHAR_EFF::LoadFile( sc::BaseStream &SFile )
{
	DWORD dwType;
    SFile >> dwType;
	m_emType = static_cast<EMCHAR_EFF_TYPE>( dwType );
    SFile.ReadBuffer( &d3dMaterial, sizeof( d3dMaterial ) );

    DWORD nTemp;
    SFile >> nTemp;

    SFile.ReadBuffer( szTexture, sizeof( char ) * MAX_PATH );
    SFile.ReadBuffer( szEffTex, sizeof( char ) * MAX_PATH );
}

void DXMATERIAL_CHAR_EFF::ClearEffTex()
{
	memset(szEffTex, 0, sizeof(char) * (MAX_PATH));
	textureRes.Clear();
}

bool DXMATERIAL_CHAR_EFF::LoadTexture( DWORD mipMap, bool bBackgroundLoading )
{
    if( _tcslen( szEffTex ) <= 0 ) 
        return false;

	// ������ �ȵǾ� ������ �ε��۾��� ���� ����.
	if ( GetType() == EMCET_NULL )
		return false;

	textureRes = TextureManager::GetInstance().LoadTexture(
        szEffTex,
		false,
        TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
        mipMap,
        D3DFMT_UNKNOWN,
        TEXTURE_RESIZE_NONE,
        TEXTURE_CHAR,
		bBackgroundLoading,
		FALSE );

    return true;
}

BOOL DXMATERIAL_CHAR_EFF::IsPathFile()
{
	if( _tcslen( szEffTex ) <= 0 ) 
		return FALSE;

	// [shhan][2014.10.01] �ؽ��� ��ü�� ���� ������ ��� �Ҽ��� ����.
	//						��� Ÿ�� ���� �׳� ������ �ϵ��� �Ѵ�.
	if ( !TextureManager::GetInstance().IsPathFile( szEffTex ) )
		return FALSE;

	return TRUE;
}