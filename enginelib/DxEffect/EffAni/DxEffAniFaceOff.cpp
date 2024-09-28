#include "pch.h"

#include "../../../SigmaCore/File/SerialFile.h"
#include "../../DxTools/TextureManager.h"
#include "../../DxMeshs/SkinMesh/DxSkinChar.h"
#include "../../DxMeshs/SkinMesh/DxAttBone.h"
#include "../../DxMeshs/SkinMesh/DxVehicle.h"

#include "./DxEffAniFaceOff.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

DWORD		DxEffAniData_FaceOff::TYPEID	= EMEFFANI_FACEOFF;
DWORD		DxEffAniData_FaceOff::VERSION	= 0x0100;
TCHAR		DxEffAniData_FaceOff::NAME[64]	= _T("Face Off");

DxEffAni* DxEffAniData_FaceOff::NEWOBJ( DxSkinAnimation* pAnimContainer )
{
	DxEffAniFaceOff*	pEff = new DxEffAniFaceOff;

	StringCchCopy( pEff->m_szTexture, 32, m_szTexture );

	pEff->Create ( m_pd3dDevice, pAnimContainer );

	return	pEff;
}

HRESULT	DxEffAniData_FaceOff::LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
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
		SFile.ReadBuffer ( &m_Property, sizeof(m_Property) );
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

HRESULT	DxEffAniData_FaceOff::SaveFile ( sc::SerialFile &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	SFile << (DWORD) ( sizeof(m_Property) );

	//	Note : 이팩트의 Property 를 저장.
	//
	SFile.WriteBuffer ( &m_Property, sizeof(m_Property) );

	return S_OK;
}

BOOL DxEffAniData_FaceOff::CheckEff ( DWORD dwCurKeyTime, DWORD dwPrevFrame )
{
	if ( !m_bCreated )			// 다음에는 현재 효과를 또 생성하지 않는다.
	{
		m_bCreated = TRUE;

		return TRUE;
	}

	return FALSE;		
}

//	-----------------------------------------------	--------------------------------------------------

DxEffAniFaceOff::DxEffAniFaceOff(void) :
	DxEffAni(),
	m_pd3dDevice(NULL),
	m_pSavedBaseSB(NULL),
	m_pEffectBaseSB(NULL)
{
}

DxEffAniFaceOff::~DxEffAniFaceOff(void)
{
	m_pd3dDevice = NULL;
	CleanUp ();
}

HRESULT DxEffAniFaceOff::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

HRESULT DxEffAniFaceOff::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

HRESULT DxEffAniFaceOff::OneTimeSceneInit ()
{
	return S_OK;
}

HRESULT DxEffAniFaceOff::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
    DeleteDeviceObjects();

    m_pd3dDevice = pd3dDevice;

    m_textureRes = TextureManager::GetInstance().LoadTexture(
        m_szTexture,
        false,
        TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
        EMMM_CHAR_MIPMAP,
        D3DFMT_UNKNOWN,
        TEXTURE_RESIZE_NONE,
        TEXTURE_EFFECT,
		true,
		FALSE );

	return S_OK;
}

HRESULT DxEffAniFaceOff::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
    InvalidateDeviceObjects();

	if ( !pd3dDevice )
        return S_OK;

	D3DCAPSQ d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );

	m_pd3dDevice = pd3dDevice;

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		//	Note : SetRenderState() 선언
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,			FALSE );

		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );

		pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR,		0xffffffff );

		//	Note : SetTextureStageState() 선언
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );

		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLORARG1,	D3DTA_CURRENT );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLORARG2,	D3DTA_TFACTOR );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,	D3DTOP_MODULATE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2,	D3DTA_TFACTOR );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedBaseSB );
		else			pd3dDevice->EndStateBlock( &m_pEffectBaseSB );
	}

	return S_OK;
}

HRESULT DxEffAniFaceOff::InvalidateDeviceObjects ()
{
	SAFE_RELEASE( m_pSavedBaseSB );
	SAFE_RELEASE( m_pEffectBaseSB );

	return S_OK;
}

HRESULT DxEffAniFaceOff::DeleteDeviceObjects ()
{
	m_textureRes.Clear();

	return S_OK;
}

HRESULT DxEffAniFaceOff::FinalCleanup ()
{
	return S_OK;
}

HRESULT DxEffAniFaceOff::FrameMove ( DxSkinCharEffAniBase* pSkinCharBase, float fTime, float fElapsedTime )
{
	return S_OK;
}

HRESULT	DxEffAniFaceOff::Render ( const LPDIRECT3DDEVICEQ pd3dDevice, DxSkinCharEffAniBase* pSkinChar, const D3DXMATRIX& matCurPos, float fAlpha )
{
	if( !m_bFrameMove )	return S_OK;	// 애니메이션이 같지가 않다.

	//if ( !pSkinChar->m_PartArray[PIECE_HEAD].m_rSkinMesh )	return S_OK;
	//if ( !pSkinChar->m_PartArray[PIECE_HEAD].m_rMeshContainer )	return S_OK;

	//pSkinChar->m_sSkinEffData.m_rFaceTex = m_textureRes.GetTexture();
	//pd3dDevice->SetTexture( 0, m_textureRes.GetTexture() );

	//CHARSETTING sCharSetting;
	//sCharSetting.pMeshContainerBase = pSkinChar->m_PartArray[PIECE_HEAD].m_rMeshContainer;
	//sCharSetting.pmtrlPiece			= NULL;
	//sCharSetting.pmtrlSpecular		= NULL;
	//sCharSetting.bWorldIdentity		= FALSE;
	//sCharSetting.emRDOP				= CTOP_NEWTEX;

	//pSkinChar->m_PartArray[PIECE_HEAD].m_rSkinMesh->SetDrawState( TRUE, FALSE, FALSE, FALSE );
	//pSkinChar->m_PartArray[PIECE_HEAD].m_rSkinMesh->DrawMeshContainer( pd3dDevice, sCharSetting, pSkinChar->GetSkeletonMaintain() );

	return S_OK;
}
//
//HRESULT	DxEffAniFaceOff::Render ( const LPDIRECT3DDEVICEQ pd3dDevice, DxAttBone* pAttBone, const D3DXMATRIX& matCurPos, float fAlpha )
//{
//	if( !m_bFrameMove )	return S_OK;	// 애니메이션이 같지가 않다.
//
//	if ( !pAttBone->m_AttBoneArray[ATTBONE_BODY].m_rSkinMesh )	return S_OK;
//	if ( !pAttBone->m_AttBoneArray[ATTBONE_BODY].m_rMeshContainer )	return S_OK;
//
//	pAttBone->m_sSkinEffData.m_rFaceTex = m_textureRes.GetTexture();
//	pd3dDevice->SetTexture( 0, m_textureRes.GetTexture() );
//
//	CHARSETTING sCharSetting;
//	sCharSetting.pMeshContainerBase = pAttBone->m_AttBoneArray[ATTBONE_BODY].m_rMeshContainer;
//	sCharSetting.pmtrlPiece			= NULL;
//	sCharSetting.pmtrlSpecular		= NULL;
//	sCharSetting.bWorldIdentity		= FALSE;
//	sCharSetting.emRDOP				= CTOP_NEWTEX;
//
//	pAttBone->m_AttBoneArray[ATTBONE_BODY].m_rSkinMesh->SetDrawState( TRUE, FALSE, FALSE, FALSE );
//	pAttBone->m_AttBoneArray[ATTBONE_BODY].m_rSkinMesh->DrawMeshContainer( pd3dDevice, sCharSetting, pAttBone->GetSkeletonMaintain() );
//
//	return S_OK;
//}
//
//HRESULT	DxEffAniFaceOff::Render ( const LPDIRECT3DDEVICEQ pd3dDevice, DxVehicle* pVehicle, const D3DXMATRIX& matCurPos, float fAlpha )
//{
//	if( !m_bFrameMove )	return S_OK;	// 애니메이션이 같지가 않다.
//
//	if ( !pVehicle->m_V_PartsArray[PART_V_BODY].m_rSkinMesh )	return S_OK;
//	if ( !pVehicle->m_V_PartsArray[PART_V_BODY].m_rMeshContainer )	return S_OK;
//
//	pVehicle->m_sSkinEffData.m_rFaceTex = m_textureRes.GetTexture();
//	pd3dDevice->SetTexture( 0, m_textureRes.GetTexture() );
//
//	CHARSETTING sCharSetting;
//	sCharSetting.pMeshContainerBase = pVehicle->m_V_PartsArray[PART_V_BODY].m_rMeshContainer;
//	sCharSetting.pmtrlPiece			= NULL;
//	sCharSetting.pmtrlSpecular		= NULL;
//	sCharSetting.bWorldIdentity		= FALSE;
//	sCharSetting.emRDOP				= CTOP_NEWTEX;
//
//	pVehicle->m_V_PartsArray[PART_V_BODY].m_rSkinMesh->SetDrawState( TRUE, FALSE, FALSE, FALSE );
//	pVehicle->m_V_PartsArray[PART_V_BODY].m_rSkinMesh->DrawMeshContainer( pd3dDevice, sCharSetting, pVehicle->GetSkeletonMaintain() );
//
//	return S_OK;
//}


BOOL DxEffAniFaceOff::IsHeadDrawChild()	
{ 
	if( m_bFrameMove )	return TRUE;
	else				return FALSE; 
}


