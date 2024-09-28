#include "pch.h"
#include "./DxEffKeepCTShade.h"

#include "../../../SigmaCore/File/SerialFile.h"
#include "../../DxTools/TextureManager.h"
#include "../../DxMeshs/SkinMesh/DxSkinChar.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

DWORD		DxEffKeepData_CTShade::TYPEID	= EFFKT_CT_SHADE;
DWORD		DxEffKeepData_CTShade::VERSION	= 0x0100;
char		DxEffKeepData_CTShade::NAME[64]	= "Toon Shade Change";

EFFKEEP_PROPERTY_CTSHADE::EFFKEEP_PROPERTY_CTSHADE()
{
	StringCchCopy( szShadeTex, 32, _T("") );
}

DxEffKeepData_CTShade::DxEffKeepData_CTShade()
{
}

DxEffKeepData_CTShade::~DxEffKeepData_CTShade()
{
}

DxEffKeep* DxEffKeepData_CTShade::NEWOBJ()
{
	DxEffKeepCTShade*	pEff = new DxEffKeepCTShade;

	StringCchCopy( pEff->m_sProp.szShadeTex, 32, m_sProp.szShadeTex );

	pEff->Create ( m_pd3dDevice );

	return	pEff;
}

HRESULT	DxEffKeepData_CTShade::LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;
	DWORD dwVer, dwSize, dwTypeID;

	SFile >> dwTypeID;
	SFile >> dwVer;
	SFile >> dwSize;

	//	Note : 버전이 일치할 경우. 
	//
	if ( dwVer == VERSION )
	{
		SFile.ReadBuffer( m_sProp.szShadeTex, 32 );
	}
	else
	{
		SFile.SetOffSet ( SFile.GetfTell()+dwSize );
		
		return E_FAIL;
	}

	if ( pd3dDevice )
	{
		hr = Create ( pd3dDevice );
		if ( FAILED(hr) )	return hr;
	}

	return S_OK;
}

HRESULT	DxEffKeepData_CTShade::SaveFile ( sc::SerialFile &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	SFile.BeginBlock();
	{
		SFile.WriteBuffer( m_sProp.szShadeTex, 32 );
	}
	SFile.EndBlock();

	return S_OK;
}

//	-----------------------------------------------	--------------------------------------------------

DxEffKeepCTShade::DxEffKeepCTShade(void) :
	DxEffKeep()
{
}

DxEffKeepCTShade::~DxEffKeepCTShade(void)
{
	CleanUp ();
}

HRESULT DxEffKeepCTShade::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

HRESULT DxEffKeepCTShade::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

HRESULT DxEffKeepCTShade::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
    m_textureRes = TextureManager::GetInstance().LoadTexture(
        m_sProp.szShadeTex,
        false,
        TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
        0,
        D3DFMT_UNKNOWN,
        TEXTURE_RESIZE_NONE,
        TEXTURE_EFFECT,
		false,
		FALSE );

    return S_OK;
}

HRESULT DxEffKeepCTShade::DeleteDeviceObjects()
{
	m_textureRes.Clear();

    return S_OK;
}

HRESULT DxEffKeepCTShade::FrameMove ( float fElapsedTime, SKINEFFDATA& sSKINEFFDATA )
{
	sSKINEFFDATA.m_rToonShadeTex = m_textureRes.GetTexture();

	return S_OK;
}

HRESULT	DxEffKeepCTShade::Render ( const LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}