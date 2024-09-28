#include "pch.h"
#include "../../../SigmaCore/File/SerialFile.h"

#include "../../DxTools/MaterialSkin/DxMultiMaterialSkinMesh.h"
#include "../../DxTools/MaterialSkin/MaterialSkinDef.h"
#include "../../DxTools/Light/DxLightMan.h"
#include "../../DxTools/RENDERPARAM.h"
#include "../../DxTools/DxViewPort.h"

#include "./DxEffCharAmbient.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

DWORD		DxEffCharAmbient::TYPEID			= EMEFFCHAR_AMBIENT;
const DWORD	DxEffCharAmbient::VERSION			= 0x0104;
char		DxEffCharAmbient::NAME[MAX_PATH]	= "1.Ambient";

DxEffCharAmbient::DxEffCharAmbient(void) :
	DxEffChar(),
	m_dwOldAmbient(0L),
	m_dwOldTexFactor(0L),
	m_dwOldColorARG1(0L),
	m_dwOldColorARG2(0L),
	m_dwOldColorOP(0L),
	m_fElapsedTime(0.f)
{
}

DxEffCharAmbient::~DxEffCharAmbient(void)
{
}

DxEffChar* DxEffCharAmbient::CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece, DWORD dwMaterialNum )
{
	HRESULT hr;
	DxEffCharAmbient *pEffChar = new DxEffCharAmbient;
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



HRESULT DxEffCharAmbient::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

HRESULT DxEffCharAmbient::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

HRESULT DxEffCharAmbient::OneTimeSceneInit ()
{
	if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
	{
	}
	else
	{
		if( m_dwFlage & USETEXCOLOR )
		{
		}
		else
		{
			if ( !(m_dwFlage&USEALL) )
			{
				// [shhan][2016.01.18] MaterialSystem 으로 변경되면서 현재 효과를 다른 효과로 변경 필요함.
				//						지금은 SetEnd 가 안 들어오는 구조가 되어 버려서 계속 누적이 된다.
				//						그래서 DxCharPart::m_vecEFFECT 의 정보가 push_back 하지 않도록 한다.
				return E_FAIL;
			}
		}
	}

	return S_OK;
}

HRESULT DxEffCharAmbient::FrameMove ( float fTime, float fElapsedTime )
{
	m_fElapsedTime += fElapsedTime;

	return S_OK;
}

HRESULT	DxEffCharAmbient::SettingState ( LPDIRECT3DDEVICEQ pd3dDevice, SKINEFFDATA& sSKINEFFDATA )
{
	if( m_dwFlage & USETEXCOLOR )
	{
		DWORD dwTexFactor = 0xff000000 + ((DWORD)(m_cColor.r*255)<<16) + ((DWORD)(m_cColor.g*255)<<8) + (DWORD)(m_cColor.b*255);
		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, m_dwOldTexFactor );

		pd3dDevice->GetRenderState( D3DRS_TEXTUREFACTOR,		&m_dwOldTexFactor );
		pd3dDevice->GetTextureStageState( 1, D3DTSS_COLORARG1,	&m_dwOldColorARG1 );
		pd3dDevice->GetTextureStageState( 1, D3DTSS_COLORARG2,	&m_dwOldColorARG2 );
		pd3dDevice->GetTextureStageState( 1, D3DTSS_COLOROP,	&m_dwOldColorOP );

		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,		dwTexFactor );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1,	D3DTA_TFACTOR );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2,	D3DTA_CURRENT );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,	D3DTOP_MODULATE );
	}
	else
	{
		D3DLIGHTQ	pLight;
		pLight = DxLightMan::GetInstance()->GetDirectLight()->m_Light;

		pLight.Ambient.r = 0.f;
		pLight.Ambient.g = 0.f;
		pLight.Ambient.b = 0.f;

		pd3dDevice->SetLight ( 0, &pLight );

		pd3dDevice->GetRenderState ( D3DRS_AMBIENT, &m_dwOldAmbient );
		pd3dDevice->SetRenderState ( D3DRS_AMBIENT, D3DCOLOR_XRGB((int)(m_cColor.r*255),(int)(m_cColor.g*255),(int)(m_cColor.b*255)) );
	}

	return S_OK;
}

DWORD DxEffCharAmbient::GetEffAmbientType()
{
    return m_dwFlage & USETEXCOLOR;
}

const D3DXCOLOR& DxEffCharAmbient::GetEffAmbientColor()
{
    return m_Property.m_cColor;
}

HRESULT	DxEffCharAmbient::RestoreState ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( m_dwFlage & USETEXCOLOR )
	{
		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,		m_dwOldTexFactor );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1,	m_dwOldColorARG1 );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2,	m_dwOldColorARG2 );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,	m_dwOldColorOP );
	}
	else
	{
		pd3dDevice->SetRenderState ( D3DRS_AMBIENT, m_dwOldAmbient );

		pd3dDevice->SetLight ( 0, &DxLightMan::GetInstance()->GetDirectLight()->m_Light );

		//	Note : 효과가 끝났을 경우
		//
		if ( !(m_dwFlage&USEALL) && m_fElapsedTime > m_fFullTime )
		{
			m_fElapsedTime = m_fFullTime;
			SetEnd();
		}
	}

	return S_OK;
}

HRESULT DxEffCharAmbient::LoadFile_0100 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	EFFCHAR_PROPERTY_AMBIENT_100	pProp;

	//	Note : 이팩트의 Property 를 저장.
	//
	SFile.ReadBuffer ( &pProp, sizeof(EFFCHAR_PROPERTY_AMBIENT_100) );

	m_dwColorOP	= pProp.m_dwColorOP;
	m_fFullTime = 0.06f;
	m_fSrcColor	= 0.4f;
	m_cColor	= D3DCOLOR_ARGB(0,255,0,0);

	m_dwFlage	= 0;

	return S_OK;
}

HRESULT DxEffCharAmbient::LoadFile_0101 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{

	EFFCHAR_PROPERTY_AMBIENT_101	pProp;

	//	Note : 이팩트의 Property 를 저장.
	//
	SFile.ReadBuffer ( &pProp, sizeof(EFFCHAR_PROPERTY_AMBIENT_101) );

	m_dwColorOP	= pProp.m_dwColorOP;
	m_fFullTime = 0.06f;
	m_fSrcColor	= 0.4f;
	m_cColor	= D3DCOLOR_ARGB(0,255,0,0);

	m_dwFlage	= 0;	

	return S_OK;
}

HRESULT DxEffCharAmbient::LoadFile_0102 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	EFFCHAR_PROPERTY_AMBIENT_102	pProp;

	//	Note : 이팩트의 Property 를 저장.
	//
	SFile.ReadBuffer ( &pProp, sizeof(EFFCHAR_PROPERTY_AMBIENT_102) );

	m_dwColorOP	= pProp.m_dwColorOP;
	m_fFullTime = 0.06f;
	m_fSrcColor	= 0.4f;
	m_cColor	= D3DCOLOR_ARGB(0,255,0,0);

	m_dwFlage	= pProp.m_dwFlage;

	return S_OK;
}

HRESULT DxEffCharAmbient::LoadFile_0103 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{

	EFFCHAR_PROPERTY_AMBIENT_103	pProp;

	//	Note : 이팩트의 Property 를 저장.
	//
	SFile.ReadBuffer ( &pProp, sizeof(EFFCHAR_PROPERTY_AMBIENT_103) );

	m_dwFlage = pProp.m_dwFlage;
	m_fFullTime = pProp.m_fFullTime;
	m_cColor = pProp.m_cColor;
	m_fSrcColor = pProp.m_fSrcColor;
	m_dwColorOP = pProp.m_dwColorOP;

	return S_OK;
}

HRESULT DxEffCharAmbient::LoadFile_0104 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{

	//	Note : 이팩트의 Property 를 저장.
	//
	SFile.ReadBuffer ( &m_Property, sizeof(m_Property) );

	return S_OK;
}

HRESULT	DxEffCharAmbient::LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	SFile >> dwSize;

	switch ( dwVer )
	{
	case 0x0100:	LoadFile_0100( SFile, pd3dDevice );	break;
	case 0x0101:	LoadFile_0101( SFile, pd3dDevice );	break;
	case 0x0102:	LoadFile_0102( SFile, pd3dDevice );	break;
	case 0x0103:	LoadFile_0103( SFile, pd3dDevice );	break;
	case VERSION:	LoadFile_0104( SFile, pd3dDevice );	break;
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

HRESULT	DxEffCharAmbient::SaveFile ( sc::BaseStream &SFile )
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

//////////////////////////////////////////////////////////////////////////
// Material System 에 현재 효과들을 붙이기 위한 작업.
void DxEffCharAmbient::PrecalculateForMaterialSystem( LPDIRECT3DDEVICEQ pd3dDevice, 
													 EffectMaterialData* pEffMaterialData, 
													 const D3DXMATRIX* pMatrix, 
													 const float fScale )
{
	pEffMaterialData->m_bAmbient = TRUE;
	pEffMaterialData->m_vEffAmbient_DiffuseMulti = D3DXVECTOR3( m_cColor.r, m_cColor.g, m_cColor.b );
	pEffMaterialData->m_vEffAmbient_AmbientMulti = D3DXVECTOR3( m_cColor.r, m_cColor.g, m_cColor.b );
}