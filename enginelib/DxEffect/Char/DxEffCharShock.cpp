#include "pch.h"
#include "../../DxEffect/Single/DxEffSingle.h"
#include "../Single/DxEffSinglePropGMan.h"
#include "../../../SigmaCore/File/SerialFile.h"

#include "../../DxTools/RENDERPARAM.h"
#include "../../DxTools/DxViewPort.h"

#include "./DxEffCharShock.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

DWORD		DxEffCharShock::TYPEID			= EMEFFCHAR_SHOCK;
const DWORD	DxEffCharShock::VERSION			= 0x0102;
char		DxEffCharShock::NAME[MAX_PATH]	= "4.쇼크";

DxEffCharShock::DxEffCharShock(void) :
	DxEffChar(),
	m_fElapsedTime(0.f),
	m_vDirection(1,0,0),
	m_fTarDirection(0.f),
	m_bLightEnable7(TRUE)
{
}


DxEffCharShock::~DxEffCharShock(void)
{
	CleanUp ();
}

DxEffChar* DxEffCharShock::CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece, DWORD dwMaterialNum )
{
	HRESULT hr;
	DxEffCharShock *pEffChar = new DxEffCharShock;
	pEffChar->SetFileName ( GetFileName() );
	pEffChar->SetLinkObj ( pCharPart, pSkinPiece );
	pEffChar->SetProperty ( &m_Property );

	hr = pEffChar->Create ( pd3dDevice );
	if ( FAILED(hr) )
	{
		SAFE_DELETE(pEffChar);
		return NULL;
	}

	return pEffChar;
}

HRESULT DxEffCharShock::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{	
	return S_OK;
}

HRESULT DxEffCharShock::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

HRESULT DxEffCharShock::OneTimeSceneInit ()
{
	if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
	{
	}
	else
	{
		// [shhan][2016.01.18] MaterialSystem 으로 변경되면서 현재 효과를 다른 효과로 변경 필요함.
		//						지금은 SetEnd 가 안 들어오는 구조가 되어 버려서 계속 누적이 된다.
		//						그래서 DxCharPart::m_vecEFFECT 의 정보가 push_back 하지 않도록 한다.
		return E_FAIL;
	}

	return S_OK;
}

HRESULT DxEffCharShock::FrameMove ( float fTime, float fElapsedTime )
{
	HRESULT hr = S_OK;

	m_fElapsedTime += fElapsedTime;

	return S_OK;
}

HRESULT	DxEffCharShock::SettingState ( LPDIRECT3DDEVICEQ pd3dDevice, SKINEFFDATA& sSKINEFFDATA )
{
	pd3dDevice->GetLightEnable	( 5, &m_bLightEnable7 );
	pd3dDevice->GetLight		( 5, &m_sLight7 );

	D3DLIGHTQ	sLight;

	sLight.Type        = D3DLIGHT_DIRECTIONAL;
	sLight.Diffuse.a   = 1.0f;
	sLight.Diffuse.r   = m_cColor.r;
	sLight.Diffuse.g   = m_cColor.g;
	sLight.Diffuse.b   = m_cColor.b;
	sLight.Ambient.a    = 0.0f;
	sLight.Ambient.r	= 0.0f;
	sLight.Ambient.g	= 0.0f;
	sLight.Ambient.b	= 0.0f;
	sLight.Specular.a   = 1.0f;
	sLight.Specular.r	= m_cColor.r;
	sLight.Specular.g	= m_cColor.g;
	sLight.Specular.b	= m_cColor.b;

	sLight.Direction	= m_vDirection;

	pd3dDevice->LightEnable ( 7, TRUE );
	pd3dDevice->SetLight ( 7, &sLight );

	return S_OK;
}

HRESULT	DxEffCharShock::RestoreState ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	pd3dDevice->LightEnable ( 5, m_bLightEnable7 );
	pd3dDevice->SetLight ( 5, &m_sLight7 );

	//	Note : 효과가 끝났을 경우
	//
	if ( m_fElapsedTime > m_fFullTime )
	{
		m_fElapsedTime = m_fFullTime;
		SetEnd();
	}

	return S_OK;
}

HRESULT DxEffCharShock::LoadFile_0101 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 이팩트의 Property 를 저장.
	//
	EFFCHAR_PROPERTY_SHOCK_101	Property;

	SFile.ReadBuffer ( &Property, sizeof(EFFCHAR_PROPERTY_SHOCK_101) );

	m_Property.Assign( Property );

	return S_OK;
}

HRESULT DxEffCharShock::LoadFile_0102 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 이팩트의 Property 를 저장.
	//
	SFile.ReadBuffer ( &m_Property, sizeof(m_Property) );

	return S_OK;
}

HRESULT	DxEffCharShock::LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	SFile >> dwSize;

	switch ( dwVer )
	{
	case 0x0100:
		{
			SFile.SetOffSet ( SFile.GetfTell()+dwSize );	// 그냥 건너 띤다.
			return S_OK;
		}
		break;
	case 0x0101:	LoadFile_0101( SFile, pd3dDevice );	break;
	case VERSION:	LoadFile_0102( SFile, pd3dDevice );	break;
	default:
		{
			//	Note : 버전이 틀릴 경우에는 파일에 쓰여진 DATA영역을 건더 띄는 작업을 진행.
			//
			SFile.SetOffSet ( SFile.GetfTell()+dwSize );
		
			return E_FAIL;
		}
		break;
	};

	//	Note : Device 자원을 생성한다.
	//
	hr = Create ( pd3dDevice );
	if ( FAILED(hr) )	return hr;

	return S_OK;
}

HRESULT	DxEffCharShock::SaveFile ( sc::BaseStream &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	//	Note : DATA의 사이즈를 기록한다. Load 시에 버전이 틀릴 경우 사용됨.
	//
	SFile << (DWORD) ( sizeof(m_Property) );

	//	Note : 이팩트의 Property 를 저장.
	//
	SFile.WriteBuffer ( &m_Property, sizeof(m_Property) );

	return S_OK;
}