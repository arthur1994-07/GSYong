//
//	DXMATERIAL_CHAR_EFF �� ��ȭ�ϸ鼭 0x105 �������� ��ȭ �Ͽ���.
//
#include "pch.h"

#include "../../../SigmaCore/Util/Util.h"

#include "../../DxTools/DxDynamicVB.h"

#include "../../DxEffect/Single/DxEffSingle.h"
#include "../Single/DxEffSinglePropGMan.h"
#include "../../../SigmaCore/File/SerialFile.h"
#include "../../DxTools/RENDERPARAM.h"

#include "../../DxTools/MaterialSkin/DxMultiMaterialSkinMesh.h"
#include "../../DxTools/MaterialSkin/MaterialSkinDef.h"
#include "../../DxTools/Light/DxLightMan.h"
#include "../../DxTools/DxBackUpRendTarget.h"
#include "../../DxTools/DxInputDevice.h"

#include "../../DxTools/DxCubeMap.h"
#include "../../DxTools/DxGlowMan.h"
#include "../../DxTools/DxSurfaceTex.h"
#include "../../DxTools/DxShadowMap.h"
#include "../../DxTools/DxViewPort.h"
#include "../../DxEffect/Frame/DxEffectMan.h"
#include "../../DxEffect/DxPostEffect.h"

#include "../../DxMeshs/SkinMesh/DxCharPart.h"
#include "../../DxMeshs/SkinMesh/DxSkinPieceContainer.h"

#include "./DxEffCharHLSL.h"
#include "../../DxMeshs/SkinMesh/_new/DxSkinMesh9_HLSL.h"
#include "../../DxMeshs/SkinMesh/_new/DxSkinMesh9_Material.h"

#include "./DxEffCharLevel.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

extern int			g_nITEMLEVEL;
extern int			g_nITEMLEVEL_MAX;
extern BOOL			g_bCHAR_EDIT_RUN;

DWORD		DxEffCharLevel::TYPEID			= EMEFFCHAR_LEVEL;
const DWORD	DxEffCharLevel::VERSION			= 0x0107;
char		DxEffCharLevel::NAME[MAX_PATH]	= "2.Level (����ȿ��)";

LPDIRECT3DSTATEBLOCK9		DxEffCharLevel::m_pSavedSB_A	= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffCharLevel::m_pDrawSB_A		= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffCharLevel::m_pSavedSB_S	= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffCharLevel::m_pDrawSB_S		= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffCharLevel::m_pSavedSB_R	= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffCharLevel::m_pDrawSB_R		= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffCharLevel::m_pSavedSB_F	= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffCharLevel::m_pDrawSB_F		= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffCharLevel::m_pSavedSB_G	= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffCharLevel::m_pDrawSB_G		= NULL;

BOOL		DxEffCharLevel::m_bFlowReflectUSE = FALSE;

DxEffCharLevel::DxEffCharLevel(void) :
	DxEffChar(),
	m_pSkinMesh(NULL),
	m_pmcMesh(NULL),
	m_dwTempMaterials(0L),		// �⺻���� �͵�..
	m_pMaterials(NULL),
	m_pTempMaterials(NULL),
	m_vSumTex01(0.f,0.f),
	m_vTex01(0.1f,0.f),
	m_vTexUV01(1.f,1.f),
	m_vMove(0.f,0.f),
	m_nTestLEVEL(1),
	m_fTime(0.f),
	m_fLevelTime(0.f),
	m_dwOldAmbient(0L)
	, m_bMaterialSystem(TRUE)
{
}

DxEffCharLevel::~DxEffCharLevel(void)
{
	m_dwMaterials = 0L;
	m_dwTempMaterials = 0L;

	SAFE_DELETE_ARRAY ( m_pMaterials );
	SAFE_DELETE_ARRAY ( m_pTempMaterials );
}

DxEffChar* DxEffCharLevel::CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece, DWORD dwMaterialNum )
{
	HRESULT hr;
	DxEffCharLevel *pEffChar = new DxEffCharLevel;
	pEffChar->SetFileName ( GetFileName() );
	pEffChar->SetLinkObj ( pCharPart, pSkinPiece );
	pEffChar->SetProperty ( &m_Property );
	pEffChar->SetMaterials ( pd3dDevice, m_dwMaterials, m_pMaterials );

	hr = pEffChar->Create ( pd3dDevice );
	if ( FAILED(hr) )
	{
		SAFE_DELETE(pEffChar);
		return NULL;
	}

	return pEffChar;
}

void DxEffCharLevel::SetMaterials ( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwMaterials, DXMATERIAL_CHAR_EFF* pMaterials )
{
	SAFE_DELETE_ARRAY ( m_pMaterials );
	m_pMaterials = new DXMATERIAL_CHAR_EFF[dwMaterials];
	m_dwMaterials = dwMaterials;

	for ( DWORD i=0; i<dwMaterials; i++ )
	{
		m_pMaterials[i].SetType( pMaterials[i].GetType() );
		m_pMaterials[i].d3dMaterial = pMaterials[i].d3dMaterial;
		StringCchCopy( m_pMaterials[i].szTexture,	MAX_PATH, pMaterials[i].szTexture );
		StringCchCopy( m_pMaterials[i].szEffTex,	MAX_PATH, pMaterials[i].szEffTex );

		// Material System ������ szEffTex �̸��� ��� �ε��� �ȵȴ�.
        m_pMaterials[i].LoadTexture( EMMM_CHAR_MIPMAP, true );
	}
}

HRESULT DxEffCharLevel::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{	
	D3DCAPSQ d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );

	m_bFlowReflectUSE = FALSE;
	if( d3dCaps.TextureCaps&D3DPTEXTURECAPS_PROJECTED )		m_bFlowReflectUSE = TRUE;

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		//	Note : SetRenderState() ����
		float fBias = -0.0002f;
		pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,			*((DWORD*)&fBias) );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_NORMALIZENORMALS,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,			FALSE );

		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );

		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,		0xffffffff );

		pd3dDevice->SetTextureStageState ( 1, D3DTSS_TEXCOORDINDEX, 0 );

		//	Note : SetTextureStageState() ����
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,	D3DTA_TFACTOR );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG2 );

		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLORARG2,	D3DTA_TFACTOR );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,	D3DTOP_MODULATE2X );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2,	D3DTA_TFACTOR );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE4X );

		pd3dDevice->SetTextureStageState ( 1, D3DTSS_ALPHAARG2,	D3DTA_CURRENT );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG2 );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedSB_A );
		else			pd3dDevice->EndStateBlock( &m_pDrawSB_A );
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		//	Note : SetRenderState() ����
		float fBias = -0.0002f;
		pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,			*((DWORD*)&fBias) );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_NORMALIZENORMALS,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,			FALSE );

		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );

		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,		0xffffffff );

		//	Note : SetTextureStageState() ����
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,	D3DTA_TFACTOR );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG2 );

		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2,	D3DTA_CURRENT );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,	D3DTOP_SELECTARG1 );

		pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX,			D3DTSS_TCI_CAMERASPACENORMAL );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS,	D3DTTFF_COUNT3 );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2,	D3DTA_TFACTOR );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );

		pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG2,	D3DTA_CURRENT );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG2 );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedSB_S );
		else			pd3dDevice->EndStateBlock( &m_pDrawSB_S );
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		//	Note : SetRenderState() ����
		float fBias = -0.0002f;
		pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,			*((DWORD*)&fBias) );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_NORMALIZENORMALS,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,			FALSE );

		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );

		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,		0xffffffff );

		//	Note : SetTextureStageState() ����
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,	D3DTA_SPECULAR );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG2 );

		pd3dDevice->SetSamplerState ( 1, D3DSAMP_ADDRESSU,	D3DTADDRESS_MIRROR );
		pd3dDevice->SetSamplerState ( 1, D3DSAMP_ADDRESSV,	D3DTADDRESS_MIRROR );

		pd3dDevice->SetTextureStageState ( 1, D3DTSS_TEXCOORDINDEX,	D3DTSS_TCI_CAMERASPACENORMAL );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_TEXTURETRANSFORMFLAGS,	D3DTTFF_PROJECTED | D3DTTFF_COUNT3 );

		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLORARG2,	D3DTA_CURRENT );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,	D3DTOP_MODULATE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2,	D3DTA_TFACTOR );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE2X );

		pd3dDevice->SetTextureStageState ( 1, D3DTSS_ALPHAARG2,	D3DTA_CURRENT );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG2 );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedSB_R );
		else			pd3dDevice->EndStateBlock( &m_pDrawSB_R );
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		//	Note : SetRenderState() ����
		float fBias = -0.0002f;
		pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,			*((DWORD*)&fBias) );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_NORMALIZENORMALS,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,			FALSE );

		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );	// �̰��� ���Ѵ�.

		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,		0xffffffff );

		//	Note : SetTextureStageState() ����
		pd3dDevice->SetSamplerState ( 1, D3DSAMP_ADDRESSU,	D3DTADDRESS_MIRROR );
		pd3dDevice->SetSamplerState ( 1, D3DSAMP_ADDRESSV,	D3DTADDRESS_MIRROR );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_TEXTURETRANSFORMFLAGS,	D3DTTFF_PROJECTED | D3DTTFF_COUNT3 );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,		D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		D3DTOP_SELECTARG1 );

		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLORARG1,		D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLORARG2,		D3DTA_TFACTOR );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,		D3DTOP_MODULATE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1,		D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2,		D3DTA_TFACTOR );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,		D3DTOP_MODULATE2X );

		pd3dDevice->SetTextureStageState ( 1, D3DTSS_ALPHAARG2,		D3DTA_CURRENT );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_ALPHAOP,		D3DTOP_SELECTARG2 );
	
		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedSB_F );
		else			pd3dDevice->EndStateBlock( &m_pDrawSB_F );
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,		FALSE );

		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,			FALSE );

		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );
		
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,	D3DTA_TFACTOR );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG2 );

		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLORARG2,	D3DTA_TFACTOR );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,	D3DTOP_MODULATE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2,	D3DTA_TFACTOR );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );

		pd3dDevice->SetTextureStageState ( 1, D3DTSS_ALPHAARG2,	D3DTA_CURRENT );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG2 );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedSB_G );
		else			pd3dDevice->EndStateBlock( &m_pDrawSB_G );
	}

	return S_OK;
}

HRESULT DxEffCharLevel::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSavedSB_A );
	SAFE_RELEASE( m_pDrawSB_A );
	SAFE_RELEASE( m_pSavedSB_S );
	SAFE_RELEASE( m_pDrawSB_S );
	SAFE_RELEASE( m_pSavedSB_R );
	SAFE_RELEASE( m_pDrawSB_R );
	SAFE_RELEASE( m_pSavedSB_F );
	SAFE_RELEASE( m_pDrawSB_F );
	SAFE_RELEASE( m_pSavedSB_G );
	SAFE_RELEASE( m_pDrawSB_G );

	return S_OK;
}

HRESULT DxEffCharLevel::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;

	if ( m_pCharPart )
	{
		m_pSkinMesh = m_pCharPart->m_rSkinMesh;
	}
	else if ( m_pSkinPiece )
	{
		m_pSkinMesh = m_pSkinPiece->m_skinMeshRes.get();
	}

	DWORD dwColor;
	DWORD dwColorA, dwColorG, dwColorR;
	D3DXCOLOR cColor;

	////	Level : 0
	//dwColor = D3DCOLOR_ARGB ( 255, 187, 204, 122 );
	////	Level : 1
	//dwColor = D3DCOLOR_ARGB ( 255, 97, 101, 87 );
	//dwColor = D3DCOLOR_ARGB ( 255, 187, 204, 122 );
	////	Level : 2
	//dwColor = D3DCOLOR_ARGB ( 255, 85, 97, 92 );
	//dwColor = D3DCOLOR_ARGB ( 255, 69, 255, 239 );
	////	Level : 3
	//dwColor = D3DCOLOR_ARGB ( 255, 73, 81, 97 );
	//dwColor = D3DCOLOR_ARGB ( 255, 114, 135, 204 );
	//dwColor = D3DCOLOR_ARGB ( 255, 120, 130, 204 );
	////	Level : 4
	//dwColor = D3DCOLOR_ARGB ( 255, 96, 69, 57 );
	//dwColor = D3DCOLOR_ARGB ( 255, 255, 182, 0 );
	//dwColor = D3DCOLOR_ARGB ( 255, 204, 98, 94 );
	////	Level : 5
	//dwColor = D3DCOLOR_ARGB ( 255, 102, 102, 102 );
	//dwColor = D3DCOLOR_ARGB ( 255, 255, 237, 182 );
	//dwColor = D3DCOLOR_ARGB ( 255, 255, 208, 0 );
	//dwColor = D3DCOLOR_ARGB ( 255, 255, 168, 29 );

	//	Level : 0
	m_OpLEVEL[0].m_Specular.m_bUse			= TRUE;
	m_OpLEVEL[0].m_Specular.m_fPower		= m_fPower;
	m_OpLEVEL[0].m_Specular.m_dwColorOP		= m_dwColorOP;
	m_OpLEVEL[0].m_Specular.m_cColor		= m_cSpecular;


	//	Level : 1
	dwColor = D3DCOLOR_ARGB ( 255, 97, 101, 87 );
	cColor		= D3DXCOLOR ( dwColor );
	cColor.a	= 1.f;
	m_OpLEVEL[1].m_Ambient.m_bUse			= TRUE;
	m_OpLEVEL[1].m_Ambient.m_fBaseColor		= 0.3f;
	m_OpLEVEL[1].m_Ambient.m_fDynamicColor	= 0.7f;
	m_OpLEVEL[1].m_Ambient.m_cColor			= cColor;

	dwColor = D3DCOLOR_ARGB ( 255, 187, 204, 122 );
	m_OpLEVEL[1].m_Specular.m_bUse			= TRUE;
	m_OpLEVEL[1].m_Specular.m_fPower		= 5.f;
	m_OpLEVEL[1].m_Specular.m_dwColorOP		= D3DTOP_MODULATE;
	m_OpLEVEL[1].m_Specular.m_cColor		= D3DXCOLOR ( dwColor );


	//	Level : 2
	dwColor = D3DCOLOR_ARGB ( 255, 85, 97, 92 );
	cColor		= D3DXCOLOR ( dwColor );
	cColor.a	= 1.f;
	m_OpLEVEL[2].m_Ambient.m_bUse			= TRUE;
	m_OpLEVEL[2].m_Ambient.m_fBaseColor		= 0.5f;
	m_OpLEVEL[2].m_Ambient.m_fDynamicColor	= 0.8f;
	m_OpLEVEL[2].m_Ambient.m_cColor			= cColor;

	dwColor = D3DCOLOR_ARGB ( 255, 69, 255, 239 );
	m_OpLEVEL[2].m_Specular.m_bUse			= TRUE;
	m_OpLEVEL[2].m_Specular.m_fPower		= 5.f;
	m_OpLEVEL[2].m_Specular.m_dwColorOP		= D3DTOP_MODULATE;
	m_OpLEVEL[2].m_Specular.m_cColor		= D3DXCOLOR ( dwColor );


	//	Level : 3
	if( m_bFlowReflectUSE )
	{
		dwColor = D3DCOLOR_ARGB ( 255, 73, 81, 97 );
		cColor		= D3DXCOLOR ( dwColor );
		cColor.a	= 1.f;
		m_OpLEVEL[3].m_Ambient.m_bUse			= TRUE;
		m_OpLEVEL[3].m_Ambient.m_fBaseColor		= 0.8f;
		m_OpLEVEL[3].m_Ambient.m_fDynamicColor	= 1.0f;
		m_OpLEVEL[3].m_Ambient.m_cColor			= cColor;

		dwColor = D3DCOLOR_ARGB ( 255, 120, 130, 204 );
		m_OpLEVEL[3].m_Reflect.m_bUse			= TRUE;
		m_OpLEVEL[3].m_Reflect.m_fPower			= 14.f;
		m_OpLEVEL[3].m_Reflect.m_dwColorOP		= D3DTOP_MODULATE2X;
		m_OpLEVEL[3].m_Reflect.m_cColor			= D3DXCOLOR ( dwColor );
	}
	else
	{
		dwColor = D3DCOLOR_ARGB ( 255, 73, 81, 97 );
		cColor		= D3DXCOLOR ( dwColor );
		cColor.a	= 1.f;
		m_OpLEVEL[3].m_Ambient.m_bUse			= TRUE;
		m_OpLEVEL[3].m_Ambient.m_fBaseColor		= 0.8f;
		m_OpLEVEL[3].m_Ambient.m_fDynamicColor	= 1.0f;
		m_OpLEVEL[3].m_Ambient.m_cColor			= cColor;
	}


	//	Level : 4
	if( m_bFlowReflectUSE )
	{
		dwColor = D3DCOLOR_ARGB ( 255, 96, 69, 57 );
		cColor		= D3DXCOLOR ( dwColor );
		cColor.a	= 1.f;
		m_OpLEVEL[4].m_Ambient.m_bUse			= TRUE;
		m_OpLEVEL[4].m_Ambient.m_fBaseColor		= 0.8f;
		m_OpLEVEL[4].m_Ambient.m_fDynamicColor	= 0.4f;
		m_OpLEVEL[4].m_Ambient.m_cColor			= cColor;

		dwColor = D3DCOLOR_ARGB ( 255, 255, 70, 0 );
		m_OpLEVEL[4].m_Reflect.m_bUse			= TRUE;
		m_OpLEVEL[4].m_Reflect.m_fPower			= 14.f;
		m_OpLEVEL[4].m_Reflect.m_dwColorOP		= D3DTOP_MODULATE2X;
		m_OpLEVEL[4].m_Reflect.m_cColor			= D3DXCOLOR ( dwColor );

		dwColor = D3DCOLOR_ARGB ( 255, 255, 70, 0 );
		m_OpLEVEL[4].m_Flow.m_bUse		= TRUE;
		m_OpLEVEL[4].m_Flow.m_cColor	= D3DXCOLOR ( dwColor );

		//dwColor = D3DCOLOR_ARGB ( 255, 255, 70, 0 );
		//m_OpLEVEL[4].m_Reflect.m_bUse			= TRUE;
		//m_OpLEVEL[4].m_Reflect.m_fPower			= 14.f;
		//m_OpLEVEL[4].m_Reflect.m_dwColorOP		= D3DTOP_MODULATE2X;
		//m_OpLEVEL[4].m_Reflect.m_cColor			= D3DXCOLOR ( dwColor );

		//dwColor = D3DCOLOR_ARGB ( 255, 255, 70, 0 );
		//m_OpLEVEL[4].m_Flow.m_bUse		= TRUE;
		//m_OpLEVEL[4].m_Flow.m_cColor	= D3DXCOLOR ( dwColor );
	}
	else
	{
		dwColor = D3DCOLOR_ARGB ( 255, 96, 69, 57 );
		cColor		= D3DXCOLOR ( dwColor );
		cColor.a	= 1.f;
		m_OpLEVEL[4].m_Ambient.m_bUse			= TRUE;
		m_OpLEVEL[4].m_Ambient.m_fBaseColor		= 0.8f;
		m_OpLEVEL[4].m_Ambient.m_fDynamicColor	= 0.4f;
		m_OpLEVEL[4].m_Ambient.m_cColor			= cColor;
	}


	//	Level : 5
	if( m_bFlowReflectUSE )
	{
		dwColor = D3DCOLOR_ARGB ( 255, 77, 63, 42 );
		cColor		= D3DXCOLOR ( dwColor );
		cColor.a	= 1.f;
		m_OpLEVEL[5].m_Ambient.m_bUse			= TRUE;
		m_OpLEVEL[5].m_Ambient.m_fBaseColor		= 0.5f;
		m_OpLEVEL[5].m_Ambient.m_fDynamicColor	= 1.5f;
		m_OpLEVEL[5].m_Ambient.m_cColor			= cColor;

		dwColor = D3DCOLOR_ARGB ( 255, 255, 180, 0 );
		m_OpLEVEL[5].m_Reflect.m_bUse			= TRUE;
		m_OpLEVEL[5].m_Reflect.m_fPower			= 14.f;
		m_OpLEVEL[5].m_Reflect.m_dwColorOP		= D3DTOP_MODULATE2X;
		m_OpLEVEL[5].m_Reflect.m_cColor			= D3DXCOLOR ( dwColor );

		dwColor = D3DCOLOR_ARGB ( 255, 255, 180, 0 );
		m_OpLEVEL[5].m_Flow.m_bUse		= TRUE;
		m_OpLEVEL[5].m_Flow.m_cColor	= D3DXCOLOR ( dwColor );

		//dwColor = D3DCOLOR_ARGB ( 255, 255, 180, 0 );
		//m_OpLEVEL[5].m_Reflect.m_bUse			= TRUE;
		//m_OpLEVEL[5].m_Reflect.m_fPower			= 14.f;
		//m_OpLEVEL[5].m_Reflect.m_dwColorOP		= D3DTOP_MODULATE2X;
		//m_OpLEVEL[5].m_Reflect.m_cColor			= D3DXCOLOR ( dwColor );

		//dwColor = D3DCOLOR_ARGB ( 255, 127, 90, 0 );
		//m_OpLEVEL[5].m_Flow.m_bUse		= TRUE;
		//m_OpLEVEL[5].m_Flow.m_cColor	= D3DXCOLOR ( dwColor );
	}
	else
	{
		dwColor = D3DCOLOR_ARGB ( 255, 77, 63, 42 );
		cColor		= D3DXCOLOR ( dwColor );
		cColor.a	= 1.f;
		m_OpLEVEL[5].m_Ambient.m_bUse			= TRUE;
		m_OpLEVEL[5].m_Ambient.m_fBaseColor		= 0.5f;
		m_OpLEVEL[5].m_Ambient.m_fDynamicColor	= 1.5f;
		m_OpLEVEL[5].m_Ambient.m_cColor			= cColor;
	}


	//	Level : 6 (+10) �ϴ�
	dwColorG = D3DCOLOR_ARGB( 200,112,143,143 );	// �۷ο� ��
	dwColorA = D3DCOLOR_ARGB( 255, 41, 78, 78 );	// Ambient
	dwColorR = D3DCOLOR_ARGB( 255, 0,128,128 );	// �� �� 

	m_OpLEVEL[6].m_Ambient.m_bUse			= TRUE;
	m_OpLEVEL[6].m_Ambient.m_fBaseColor		= 0.5f;
	m_OpLEVEL[6].m_Ambient.m_fDynamicColor	= 1.5f;
	m_OpLEVEL[6].m_Ambient.m_cColor			= D3DXCOLOR ( dwColorA );

	m_OpLEVEL[6].m_Glow.m_bUse				= TRUE;
	m_OpLEVEL[6].m_Glow.m_cColor			= D3DXCOLOR ( dwColorG );

	m_OpLEVEL[6].m_Reflect.m_bUse			= TRUE;
	m_OpLEVEL[6].m_Reflect.m_fPower			= 14.f;
	m_OpLEVEL[6].m_Reflect.m_dwColorOP		= D3DTOP_MODULATE2X;
	m_OpLEVEL[6].m_Reflect.m_cColor			= D3DXCOLOR ( dwColorR );

	m_OpLEVEL[6].m_Flow.m_bUse				= TRUE;
	m_OpLEVEL[6].m_Flow.m_cColor			= D3DXCOLOR ( dwColorR );


	//	Level : 7 (+11) �Ķ���
	dwColorG = D3DCOLOR_ARGB( 200,112,112,143 );	// �۷ο� ��
	dwColorA = D3DCOLOR_ARGB( 255, 41, 41, 78 );	// Ambient
	dwColorR = D3DCOLOR_ARGB( 255, 0,0,128 );		// �� �� 

	m_OpLEVEL[7].m_Ambient.m_bUse			= TRUE;
	m_OpLEVEL[7].m_Ambient.m_fBaseColor	= 0.5f;
	m_OpLEVEL[7].m_Ambient.m_fDynamicColor	= 1.5f;
	m_OpLEVEL[7].m_Ambient.m_cColor		= D3DXCOLOR ( dwColorA );

	m_OpLEVEL[7].m_Glow.m_bUse				= TRUE;
	m_OpLEVEL[7].m_Glow.m_cColor			= D3DXCOLOR ( dwColorG );

	m_OpLEVEL[7].m_Reflect.m_bUse			= TRUE;
	m_OpLEVEL[7].m_Reflect.m_fPower		= 14.f;
	m_OpLEVEL[7].m_Reflect.m_dwColorOP		= D3DTOP_MODULATE2X;
	m_OpLEVEL[7].m_Reflect.m_cColor		= D3DXCOLOR ( dwColorR );

	m_OpLEVEL[7].m_Flow.m_bUse				= TRUE;
	m_OpLEVEL[7].m_Flow.m_cColor			= D3DXCOLOR ( dwColorR );




	//	Level : 8 (+12) �ʷ�
	dwColorG = D3DCOLOR_ARGB( 200,112,143,112 );	// �۷ο� ��
	dwColorA = D3DCOLOR_ARGB( 255, 41, 78, 41 );	// Ambient
	dwColorR = D3DCOLOR_ARGB( 255, 0,128,0 );		// �� �� 

	m_OpLEVEL[8].m_Ambient.m_bUse			= TRUE;
	m_OpLEVEL[8].m_Ambient.m_fBaseColor		= 0.5f;
	m_OpLEVEL[8].m_Ambient.m_fDynamicColor	= 1.5f;
	m_OpLEVEL[8].m_Ambient.m_cColor			= D3DXCOLOR ( dwColorA );


	m_OpLEVEL[8].m_Glow.m_bUse				= TRUE;
	m_OpLEVEL[8].m_Glow.m_cColor			= D3DXCOLOR ( dwColorG );
	
	m_OpLEVEL[8].m_Reflect.m_bUse			= TRUE;
	m_OpLEVEL[8].m_Reflect.m_fPower			= 14.f;
	m_OpLEVEL[8].m_Reflect.m_dwColorOP		= D3DTOP_MODULATE2X;
	m_OpLEVEL[8].m_Reflect.m_cColor			= D3DXCOLOR ( dwColorR );

	m_OpLEVEL[8].m_Flow.m_bUse				= TRUE;
	m_OpLEVEL[8].m_Flow.m_cColor			= D3DXCOLOR ( dwColorR );



	//	Level : 9 (+13) ������

	dwColorG = D3DCOLOR_ARGB( 200,143,112,112 );	// �۷ο� ��
	dwColorA = D3DCOLOR_ARGB( 255, 78, 41, 41 );	// Ambient
	dwColorR = D3DCOLOR_ARGB( 255, 128,0,0 );	// �� �� 

	m_OpLEVEL[9].m_Ambient.m_bUse			= TRUE;
	m_OpLEVEL[9].m_Ambient.m_fBaseColor		= 0.5f;
	m_OpLEVEL[9].m_Ambient.m_fDynamicColor	= 1.5f;
	m_OpLEVEL[9].m_Ambient.m_cColor			= D3DXCOLOR ( dwColorA );

	m_OpLEVEL[9].m_Glow.m_bUse				= TRUE;
	m_OpLEVEL[9].m_Glow.m_cColor			= D3DXCOLOR ( dwColorG );

	m_OpLEVEL[9].m_Reflect.m_bUse			= TRUE;
	m_OpLEVEL[9].m_Reflect.m_fPower			= 14.f;
	m_OpLEVEL[9].m_Reflect.m_dwColorOP		= D3DTOP_MODULATE2X;
	m_OpLEVEL[9].m_Reflect.m_cColor			= D3DXCOLOR ( dwColorR );
	
	m_OpLEVEL[9].m_Flow.m_bUse				= TRUE;
	m_OpLEVEL[9].m_Flow.m_cColor			= D3DXCOLOR ( dwColorR );


	//	Level : 10 (+14) �����
	dwColorG = D3DCOLOR_ARGB( 200,143,112,143 );	// �۷ο� ��
	dwColorA = D3DCOLOR_ARGB( 255, 78, 41, 78 );	// Ambient
	dwColorR = D3DCOLOR_ARGB( 255, 128,0,128 );		// �� �� 

	m_OpLEVEL[10].m_Ambient.m_bUse			= TRUE;
	m_OpLEVEL[10].m_Ambient.m_fBaseColor	= 0.5f;
	m_OpLEVEL[10].m_Ambient.m_fDynamicColor	= 1.5f;
	m_OpLEVEL[10].m_Ambient.m_cColor		= D3DXCOLOR ( dwColorA );

	m_OpLEVEL[10].m_Glow.m_bUse				= TRUE;
	m_OpLEVEL[10].m_Glow.m_cColor			= D3DXCOLOR ( dwColorG );

	m_OpLEVEL[10].m_Reflect.m_bUse			= TRUE;
	m_OpLEVEL[10].m_Reflect.m_fPower		= 14.f;
	m_OpLEVEL[10].m_Reflect.m_dwColorOP		= D3DTOP_MODULATE2X;
	m_OpLEVEL[10].m_Reflect.m_cColor		= D3DXCOLOR ( dwColorR );

	m_OpLEVEL[10].m_Flow.m_bUse				= TRUE;
	m_OpLEVEL[10].m_Flow.m_cColor			= D3DXCOLOR ( dwColorR );


	//	Level : 11 (+15) ���
	dwColorG = D3DCOLOR_ARGB( 200,143,143,112 );	// �۷ο� ��
	dwColorA = D3DCOLOR_ARGB( 255, 78, 78, 41 );	// Ambient
	dwColorR = D3DCOLOR_ARGB( 255, 128,128,0 );	// �� �� 

	m_OpLEVEL[11].m_Ambient.m_bUse			= TRUE;
	m_OpLEVEL[11].m_Ambient.m_fBaseColor		= 0.5f;
	m_OpLEVEL[11].m_Ambient.m_fDynamicColor	= 1.5f;
	m_OpLEVEL[11].m_Ambient.m_cColor			= D3DXCOLOR ( dwColorA );

	m_OpLEVEL[11].m_Glow.m_bUse				= TRUE;
	m_OpLEVEL[11].m_Glow.m_cColor			= D3DXCOLOR ( dwColorG );

	m_OpLEVEL[11].m_Reflect.m_bUse			= TRUE;
	m_OpLEVEL[11].m_Reflect.m_fPower			= 14.f;
	m_OpLEVEL[11].m_Reflect.m_dwColorOP		= D3DTOP_MODULATE2X;
	m_OpLEVEL[11].m_Reflect.m_cColor			= D3DXCOLOR ( dwColorR );

	m_OpLEVEL[11].m_Flow.m_bUse				= TRUE;
	m_OpLEVEL[11].m_Flow.m_cColor			= D3DXCOLOR ( dwColorR );	

    m_textureRes = TextureManager::GetInstance().LoadTexture(
        m_szTexture,
        false,
        TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
        EMMM_CHAR_MIPMAP,
        D3DFMT_UNKNOWN,
        TEXTURE_RESIZE_NONE,
        TEXTURE_CHAR,
		true,
		FALSE );

	m_szLevelFlowTex = "Level_Flow.dds";
    m_textureResFlow = TextureManager::GetInstance().LoadTexture(
        m_szLevelFlowTex,
        false,
        TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
        0,
        D3DFMT_UNKNOWN,
        TEXTURE_RESIZE_NONE,
        TEXTURE_CHAR,
		true,
		FALSE );

	// DXMATERIAL_CHAR_EFF �����۾�.
	IsExist_Copy_pmcMesh();

	return S_OK;
}

HRESULT DxEffCharLevel::DeleteDeviceObjects ()
{
    m_textureRes.Clear();
    m_textureResFlow.Clear();

	SAFE_DELETE_ARRAY ( m_pMaterials );

	return S_OK;
}

HRESULT DxEffCharLevel::FrameMove ( const float fTime, const float fElapsedTime )
{
	HRESULT hr = S_OK;
	m_fTime += fElapsedTime;
	if ( m_fTime > 2.f )
	{
		m_fTime -= 2.f;
		if ( m_fTime > 2.f )	m_fTime = 0.f;	// ���� �ʹ� Ŭ���� 0���� �Ѵ�.
	}
	if ( m_fTime <= 1.f )	m_fLevelTime = m_fTime;
	else					m_fLevelTime = 2.f - m_fTime;

	m_vTexUV01 = D3DXVECTOR2 ( 0.05f, 0.05f );	// �ݺ�
	m_vTex01 = D3DXVECTOR2 ( 0.6f, 0.9f );		// �̵�
	//m_vTexUV01 = D3DXVECTOR2 ( 0.2f, 0.05f );	// �ݺ�
	//m_vTex01 = D3DXVECTOR2 ( 1.0f, 0.1f );		// �̵�
	m_vSumTex01 += m_vTex01*fElapsedTime;
	m_vMove.x = m_vSumTex01.x * m_vTexUV01.x;	// m_vTexUV01 �̰��� UV ������ ���̴�.
	m_vMove.y = m_vSumTex01.y * m_vTexUV01.y;

	return S_OK;
}

void DxEffCharLevel::GetLevelData( LPDIRECT3DDEVICEQ pd3dDevice, SKINEFF_LEVEL_DATA** ppLevelData )
{
	// ������ �����ɼ�
	if( DxEffectMan::GetInstance().GetSkinDetail() == SKD_NONE )			
        return;

	//	Note : CharEdit ������ ���̴� ���̴�.
	int nLevel(0);
	if( g_bCHAR_EDIT_RUN )	
	{
		nLevel = g_nITEMLEVEL;
	}
	else if( m_pCharPart )
	{
		nLevel = m_pCharPart->GetGrindLevel();
	}

    // ����ġ�� ����� ���۵��� �ʵ��� �Ѵ�.
	if( nLevel == 0 || nLevel > LEVEL_LIMIT )	
		return;

	for ( DWORD i=0; i<m_dwMaterials; i++ )
	{
		if( m_pMaterials[i].IsType( EMCET_BASE ) && !m_pMaterials[i].GetTexture() )
		{
			m_pMaterials[i].UpdateTexture();
		}
	}

	{
		m_sSkinEffLevelData.m_bLevel = FALSE;
		m_sSkinEffLevelData.m_bLevelFlow = FALSE;

		for ( DWORD i=0; i<m_dwMaterials && i<1; i++ )
		{
			if ( m_OpLEVEL[nLevel].m_Ambient.m_bUse )
			{
				m_sSkinEffLevelData.m_bLevel = TRUE;

				float fData;
				float fFact = (m_fLevelTime*m_OpLEVEL[nLevel].m_Ambient.m_fDynamicColor) + m_OpLEVEL[nLevel].m_Ambient.m_fBaseColor;

				fData = m_OpLEVEL[nLevel].m_Ambient.m_cColor.r*fFact;
				fData = (fData>1.f) ? 1.f : fData;
				m_sSkinEffLevelData.m_vLevelColor.x = fData;

				fData = m_OpLEVEL[nLevel].m_Ambient.m_cColor.g*fFact;
				fData = (fData>1.f) ? 1.f : fData;
				m_sSkinEffLevelData.m_vLevelColor.y = fData;

				fData = m_OpLEVEL[nLevel].m_Ambient.m_cColor.b*fFact;
				fData = (fData>1.f) ? 1.f : fData;
				m_sSkinEffLevelData.m_vLevelColor.z = fData;
			}

			if ( m_OpLEVEL[nLevel].m_Flow.m_bUse )
			{
				m_sSkinEffLevelData.m_bLevelFlow = TRUE;

				m_sSkinEffLevelData.m_vLevelFlowColor.x = m_OpLEVEL[nLevel].m_Flow.m_cColor.r;
				m_sSkinEffLevelData.m_vLevelFlowColor.y = m_OpLEVEL[nLevel].m_Flow.m_cColor.g;
				m_sSkinEffLevelData.m_vLevelFlowColor.z = m_OpLEVEL[nLevel].m_Flow.m_cColor.b;
			}
		}

		if ( !DxSurfaceTex::GetInstance().IsFlowOneDraw() )
		{
			DxImageMove ( pd3dDevice, m_textureResFlow.GetTexture(), DxSurfaceTex::GetInstance().m_pFlowSuf, m_vMove, TRUE, 0L, 0xffffffff, m_vTexUV01, 128 );
			DxSurfaceTex::GetInstance().SetFlowUSE ();	// �ѹ� �ߴٴ� �� �ν� ��Ų��.
		}
	}

	(*ppLevelData) = &m_sSkinEffLevelData;
}

void	DxEffCharLevel::RenderGlow ( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* arrayBoneMatrices, BOOL bPieceRender, BOOL bActiveVisualMaterial )
{
	HRESULT hr = S_OK;

	if ( !m_pSkinMesh )
		return;

	// DXMATERIAL_CHAR_EFF Ȯ�� �� �����۾�.
	if ( !IsExist_Copy_pmcMesh() )
		return;

	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );
	const D3DXMATRIX* pmatStaticLinkBone(NULL);

	SMATERIAL_PIECE*	pMaterialPiece = NULL;
	if ( m_pCharPart )	
	{
		pmatStaticLinkBone = &m_pCharPart->GetStaticLinkBone();
		pMaterialPiece = m_pCharPart->GetSkinPiece()->m_arrayMaterialPiece;
	}
	else if ( m_pSkinPiece )
	{
		pmatStaticLinkBone = &matIdentity;
		pMaterialPiece = m_pSkinPiece->m_arrayMaterialPiece;
	}

	int nLevel = 0;

	//	Note : CharEdit ������ ���̴� ���̴�.
	if( g_bCHAR_EDIT_RUN )	nLevel = g_nITEMLEVEL;
	else					nLevel = m_pCharPart->GetGrindLevel();

	BOOL bAllLoad = TRUE;
	for ( DWORD i=0; i<m_dwMaterials; i++ )
	{
        if( m_pMaterials[i].IsType( EMCET_BASE ) && !m_pMaterials[i].GetTexture() )
		{
		    bAllLoad = FALSE;
			m_pMaterials[i].UpdateTexture();
		}
	}

	if ( bActiveVisualMaterial )
    {
  //      m_sSkinEffLevelData.m_bLevel = FALSE;
  //      m_sSkinEffLevelData.m_bLevelFlow = FALSE;

  //      for ( DWORD i=0; i<m_dwMaterials && i<1; i++ )
		//{
		//	if ( m_OpLEVEL[nLevel].m_Ambient.m_bUse )
		//	{
  //              m_sSkinEffLevelData.m_bLevel = TRUE;

		//		float fData;
		//		float fFact = (m_fLevelTime*m_OpLEVEL[nLevel].m_Ambient.m_fDynamicColor) + m_OpLEVEL[nLevel].m_Ambient.m_fBaseColor;

		//		fData = m_OpLEVEL[nLevel].m_Ambient.m_cColor.r*fFact;
		//		fData = (fData>1.f) ? 1.f : fData;
		//		m_sSkinEffLevelData.m_vLevelColor.x = fData;

		//		fData = m_OpLEVEL[nLevel].m_Ambient.m_cColor.g*fFact;
		//		fData = (fData>1.f) ? 1.f : fData;
		//		m_sSkinEffLevelData.m_vLevelColor.y = fData;

		//		fData = m_OpLEVEL[nLevel].m_Ambient.m_cColor.b*fFact;
		//		fData = (fData>1.f) ? 1.f : fData;
		//		m_sSkinEffLevelData.m_vLevelColor.z = fData;
		//	}

  //          if ( m_OpLEVEL[nLevel].m_Flow.m_bUse )
		//	{
  //              m_sSkinEffLevelData.m_bLevelFlow = TRUE;

		//		m_sSkinEffLevelData.m_vLevelFlowColor.x = m_OpLEVEL[nLevel].m_Flow.m_cColor.r;
		//		m_sSkinEffLevelData.m_vLevelFlowColor.y = m_OpLEVEL[nLevel].m_Flow.m_cColor.g;
		//		m_sSkinEffLevelData.m_vLevelFlowColor.z = m_OpLEVEL[nLevel].m_Flow.m_cColor.b;
		//	}
		//}

  //      if ( !DxSurfaceTex::GetInstance().IsFlowOneDraw() )
	 //   {
		//    DxImageMove ( pd3dDevice, m_textureResFlow.GetTexture(), DxSurfaceTex::GetInstance().m_pFlowSuf,
		//			        m_vMove, TRUE, 0L, 0xffffffff, m_vTexUV01, 128 );
		//    DxSurfaceTex::GetInstance().SetFlowUSE ();	// �ѹ� �ߴٴ� �� �ν� ��Ų��.
	 //   }
    }
    else
    {
	    if ( bAllLoad )	
            Render_EX( pd3dDevice, pMaterialPiece, nLevel, bPieceRender, *pmatStaticLinkBone, arrayBoneMatrices );
    }
}

HRESULT	DxEffCharLevel::Render_EX ( const LPDIRECT3DDEVICEQ pd3dDevice, SMATERIAL_PIECE* pMaterialPiece, int nLevel, const BOOL bPieceRender, const D3DXMATRIX& matStaticLinkBone, const D3DXMATRIX* arrayBoneMatrices )
{
	// DXMATERIAL_CHAR_EFF Ȯ�� �� �����۾�.
	if ( !IsExist_Copy_pmcMesh() )
		return S_OK;

	if( DxEffectMan::GetInstance().GetSkinDetail() == SKD_NONE )			return S_OK;	// ������ �����ɼ�

	if( nLevel > LEVEL_LIMIT )	nLevel = LEVEL_LIMIT;	// ���� ����

	if( m_OpLEVEL[nLevel].m_Ambient.m_bUse )
	{
		Render_Ambient( pd3dDevice, pMaterialPiece, m_OpLEVEL[nLevel].m_Ambient, bPieceRender, matStaticLinkBone, arrayBoneMatrices );
	}

	if( !(DxEffectMan::GetInstance().GetDetailFlag()&REALSPECULAR) )		return S_OK;	// �׷��� ī�� 

	if( m_OpLEVEL[nLevel].m_Specular.m_bUse )
	{
		Render_Specular( pd3dDevice, pMaterialPiece, m_OpLEVEL[nLevel].m_Specular, bPieceRender, matStaticLinkBone, arrayBoneMatrices );
	}
	if( m_OpLEVEL[nLevel].m_Reflect.m_bUse )
	{
		Render_Reflect( pd3dDevice, pMaterialPiece, m_OpLEVEL[nLevel].m_Reflect, bPieceRender, matStaticLinkBone, arrayBoneMatrices );
	}
	if( m_OpLEVEL[nLevel].m_Flow.m_bUse )
	{
		Render_Flow( pd3dDevice, pMaterialPiece, m_OpLEVEL[nLevel].m_Flow, bPieceRender, matStaticLinkBone, arrayBoneMatrices );
	}
	if( m_OpLEVEL[nLevel].m_Glow.m_bUse )
	{
		Render_Glow( pd3dDevice, pMaterialPiece, m_OpLEVEL[nLevel].m_Glow, bPieceRender, matStaticLinkBone, arrayBoneMatrices );
	}

	return S_OK;
}

HRESULT	DxEffCharLevel::Render_Ambient( const LPDIRECT3DDEVICEQ pd3dDevice,
										SMATERIAL_PIECE* pMaterialPiece,
										const DXAMBIENT& cData, 
										const BOOL bPieceRender, 
										const D3DXMATRIX& matStaticLinkBone, 
										const D3DXMATRIX* arrayBoneMatrices )
{
	for ( DWORD i=0; i<m_dwMaterials; i++ )
	{
		if ( cData.m_bUse )
		{
			float fData;
			float fFact = (m_fLevelTime*cData.m_fDynamicColor) + cData.m_fBaseColor;

			m_pMaterials[i].d3dMaterial.Diffuse.a = 1.f;

			fData = cData.m_cColor.r*fFact;
			fData = (fData>1.f) ? 1.f : fData;
			m_pMaterials[i].d3dMaterial.Diffuse.r = fData;

			fData = cData.m_cColor.g*fFact;
			fData = (fData>1.f) ? 1.f : fData;
			m_pMaterials[i].d3dMaterial.Diffuse.g = fData;

			fData = cData.m_cColor.b*fFact;
			fData = (fData>1.f) ? 1.f : fData;
			m_pMaterials[i].d3dMaterial.Diffuse.b = fData;            
		}
		else
		{
			m_pMaterials[i].d3dMaterial.Diffuse.a = 1.f;
			m_pMaterials[i].d3dMaterial.Diffuse.r = 1.f;
			m_pMaterials[i].d3dMaterial.Diffuse.g = 1.f;
			m_pMaterials[i].d3dMaterial.Diffuse.b = 1.f;
		}
	}

	m_pSavedSB_A->Capture();
	m_pDrawSB_A->Apply();

	switch( RENDERPARAM::emCharRenderTYPE )
	{
	case EMCRT_SOFTWARE:
	case EMCRT_NORMAL:
		{
			CHARSETTING sCharSetting;
			sCharSetting.pMeshContainerBase = m_pmcMesh;
			sCharSetting.pmtrlPiece			= pMaterialPiece;
			sCharSetting.pmtrlSpecular		= m_pMaterials;
			sCharSetting.bWorldIdentity		= bPieceRender;
			sCharSetting.emRDOP				= CTOP_BASE;

			m_pSkinMesh->SetDrawState ( FALSE, FALSE, FALSE, FALSE, TRUE );
			m_pSkinMesh->DrawMeshContainer ( pd3dDevice, sCharSetting, m_rSkeleton, matStaticLinkBone, arrayBoneMatrices );
		}
		break;


	case EMCRT_VERTEX:
	case EMCRT_PIXEL:
		//DxSkinMesh9_HLSL::m_pFX = NSCHARHLSL::m_pLevel1FX;
		//m_pSkinMesh->SetDrawState( FALSE, FALSE, FALSE, FALSE, TRUE );
		//m_pSkinMesh->DrawMeshSpecular( pd3dDevice, m_pmcMesh, pMaterialPiece, m_pMaterials, bPieceRender );
		break;
	};

	m_pSavedSB_A->Apply();

	for ( DWORD i=0; i<m_dwMaterials; i++ )
	{
		m_pMaterials[i].d3dMaterial.Diffuse.a = 1.f;
		m_pMaterials[i].d3dMaterial.Diffuse.r = 1.f;
		m_pMaterials[i].d3dMaterial.Diffuse.g = 1.f;
		m_pMaterials[i].d3dMaterial.Diffuse.b = 1.f;
	}

	return S_OK;
}

HRESULT	DxEffCharLevel::Render_Specular( const LPDIRECT3DDEVICEQ pd3dDevice,
										SMATERIAL_PIECE* pMaterialPiece,
										const DXSPECULAR& cData,
										const BOOL bPieceRender, 
										const D3DXMATRIX& matStaticLinkBone, 
										const D3DXMATRIX* arrayBoneMatrices )
{
	HRESULT hr = S_OK;

	//	����ġ
	D3DXVECTOR3		vDir, vSrcDir, vWidth, vUp;
	vSrcDir		= DxLightMan::GetInstance()->GetDirectLight()->m_Light.Direction;
	vDir		= DxViewPort::GetInstance().GetLookDir();

	vDir.y  = 0.4f;
	D3DXVec3Normalize ( &vDir, &vDir );

	D3DLIGHTQ	pSrcLight4, pSrcLight5;
	BOOL		bLightEnable4, bLightEnable5;

	pd3dDevice->GetLightEnable	( 4, &bLightEnable4 );
	pd3dDevice->GetLight		( 4, &pSrcLight4 );

	pd3dDevice->GetLightEnable	( 5, &bLightEnable5 );
	pd3dDevice->GetLight		( 5, &pSrcLight5 );

	D3DLIGHTQ	pLight;

	pLight.Type        = D3DLIGHT_DIRECTIONAL;
	pLight.Diffuse.a   = 1.0f;
	pLight.Diffuse.r   = 1.0f;
	pLight.Diffuse.g   = 1.0f;
	pLight.Diffuse.b   = 1.0f;
	pLight.Ambient.a    = 0.6f;
	pLight.Ambient.r	= 0.6f;
	pLight.Ambient.g	= 0.6f;
	pLight.Ambient.b	= 0.6f;
	pLight.Specular.a   = 1.0f;
	pLight.Specular.r   = cData.m_cColor.r;
	pLight.Specular.g   = cData.m_cColor.g;
	pLight.Specular.b   = cData.m_cColor.b;
	pLight.Direction	= vDir;
	pLight.Range		= 80.0f;
	pLight.Attenuation0 = 1.0f;
	pLight.Attenuation1 = 0.0f;
	pLight.Attenuation2 = 0.0f;

	pd3dDevice->LightEnable ( 4, TRUE );
	pd3dDevice->SetLight ( 4, &pLight );

	vDir.y  = -vDir.y;
	pLight.Direction	= vDir;

	pd3dDevice->LightEnable ( 5, TRUE );
	pd3dDevice->SetLight ( 5, &pLight );

	pd3dDevice->LightEnable ( 0, FALSE );

	m_pSavedSB_S->Capture();
	m_pDrawSB_S->Apply();

	pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP, cData.m_dwColorOP );		// ����

	for ( DWORD i=0; i<m_dwMaterials; ++i )
	{
		m_pMaterials[i].d3dMaterial.Power = cData.m_fPower;
	}

	pd3dDevice->SetTexture( 1, DxCubeMap::GetInstance().GetCubeTexTEST() );

	switch( RENDERPARAM::emCharRenderTYPE )
	{
	case EMCRT_SOFTWARE:
	case EMCRT_NORMAL:
		{
			CHARSETTING sCharSetting;
			sCharSetting.pMeshContainerBase = m_pmcMesh;
			sCharSetting.pmtrlPiece			= pMaterialPiece;
			sCharSetting.pmtrlSpecular		= m_pMaterials;
			sCharSetting.bWorldIdentity		= bPieceRender;
			sCharSetting.emRDOP				= CTOP_BASE;

			m_pSkinMesh->SetDrawState ( FALSE, FALSE, FALSE, FALSE );
			m_pSkinMesh->DrawMeshContainer ( pd3dDevice, sCharSetting, m_rSkeleton, matStaticLinkBone, arrayBoneMatrices );
		}
		break;

	case EMCRT_VERTEX:
	case EMCRT_PIXEL:
		//DxSkinMesh9_HLSL::m_pFX = NSCHARHLSL::m_pSpecularFX;
		//m_pSkinMesh->SetDrawState ( FALSE, FALSE, FALSE, FALSE );
		//m_pSkinMesh->DrawMeshSpecular( pd3dDevice, m_pmcMesh, pMaterialPiece, m_pMaterials, bPieceRender );
		break;
	};

	pd3dDevice->SetTexture( 1, NULL );

	m_pSavedSB_S->Apply();

	pd3dDevice->LightEnable ( 0, TRUE );

	pd3dDevice->LightEnable ( 4, bLightEnable4 );
	pd3dDevice->SetLight ( 4, &pSrcLight4 );

	pd3dDevice->LightEnable ( 5, bLightEnable5 );
	pd3dDevice->SetLight ( 5, &pSrcLight5 );

	return S_OK;
}

HRESULT	DxEffCharLevel::Render_Reflect( const LPDIRECT3DDEVICEQ pd3dDevice,
										SMATERIAL_PIECE* pMaterialPiece, 
										const DXSPECULAR& cData, 
										const BOOL bPieceRender, 
										const D3DXMATRIX& matStaticLinkBone, 
										const D3DXMATRIX* arrayBoneMatrices )
{
	pd3dDevice->SetTexture ( 1, m_textureRes.GetTexture() );

	//	����ġ
	D3DXVECTOR3		vDir, vSrcDir, vWidth, vUp;
	vSrcDir		= DxLightMan::GetInstance()->GetDirectLight()->m_Light.Direction;
	vDir		= DxViewPort::GetInstance().GetLookDir();

	vDir.y  = 0.4f;
	//vDir.y	= 1.0f;
	D3DXVec3Normalize ( &vDir, &vDir );

	D3DLIGHTQ	pSrcLight4, pSrcLight5;
	BOOL		bLightEnable4, bLightEnable5;

	pd3dDevice->GetLightEnable	( 4, &bLightEnable4 );
	pd3dDevice->GetLight		( 4, &pSrcLight4 );

	pd3dDevice->GetLightEnable	( 5, &bLightEnable5 );
	pd3dDevice->GetLight		( 5, &pSrcLight5 );

	D3DLIGHTQ	pLight;

	pLight.Type        = D3DLIGHT_DIRECTIONAL;
	pLight.Diffuse.a   = 1.0f;
	pLight.Diffuse.r   = 1.0f;
	pLight.Diffuse.g   = 1.0f;
	pLight.Diffuse.b   = 1.0f;
	pLight.Ambient.a    = 0.6f;
	pLight.Ambient.r	= 0.6f;
	pLight.Ambient.g	= 0.6f;
	pLight.Ambient.b	= 0.6f;
	pLight.Specular.a   = 1.0f;
	pLight.Specular.r   = cData.m_cColor.r;
	pLight.Specular.g   = cData.m_cColor.g;
	pLight.Specular.b   = cData.m_cColor.b;
	pLight.Direction	= vDir;
	pLight.Range		= 80.0f;
	pLight.Attenuation0 = 1.0f;
	pLight.Attenuation1 = 0.0f;
	pLight.Attenuation2 = 0.0f;

	pd3dDevice->LightEnable ( 4, TRUE );
	pd3dDevice->SetLight ( 4, &pLight );

	vDir.y  = -vDir.y;
	pLight.Direction	= vDir;

	pd3dDevice->LightEnable ( 5, TRUE );
	pd3dDevice->SetLight ( 5, &pLight );

	pd3dDevice->LightEnable ( 0, FALSE );

	m_pSavedSB_R->Capture();
	m_pDrawSB_R->Apply();

	pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP, cData.m_dwColorOP );		// ����

	for ( DWORD i=0; i<m_dwMaterials; ++i )
	{
		m_pMaterials[i].d3dMaterial.Power = cData.m_fPower;
	}

	CHARSETTING sCharSetting;
	switch( RENDERPARAM::emCharRenderTYPE )
	{
	case EMCRT_SOFTWARE:
	case EMCRT_NORMAL:

		sCharSetting.pMeshContainerBase = m_pmcMesh;
		sCharSetting.pmtrlPiece			= pMaterialPiece;
		sCharSetting.pmtrlSpecular		= m_pMaterials;
		sCharSetting.bWorldIdentity		= bPieceRender;
		sCharSetting.emRDOP				= CTOP_BASE;

		m_pSkinMesh->SetDrawState ( FALSE, FALSE, FALSE, FALSE );
		m_pSkinMesh->DrawMeshContainer ( pd3dDevice, sCharSetting, m_rSkeleton, matStaticLinkBone, arrayBoneMatrices );
		break;


	case EMCRT_VERTEX:
	case EMCRT_PIXEL:

		//sCharSetting.pMeshContainerBase = m_pmcMesh;
		//sCharSetting.pmtrlPiece			= pMaterialPiece;
		//sCharSetting.pmtrlSpecular		= m_pMaterials;
		//sCharSetting.bWorldIdentity		= bPieceRender;
		//sCharSetting.emRDOP				= CTOP_BASE;

		//DxSkinMesh9_HLSL::m_pFX = NSCHARHLSL::m_pReflectFX;
		//m_pSkinMesh->SetDrawState ( FALSE, FALSE, FALSE, FALSE );
		//m_pSkinMesh->DrawMeshSpecular( pd3dDevice, m_pmcMesh, pMaterialPiece, m_pMaterials, bPieceRender );

		break;
	};

	m_pSavedSB_R->Apply();

	pd3dDevice->SetTexture ( 1, NULL );

	pd3dDevice->LightEnable ( 0, TRUE );

	pd3dDevice->LightEnable ( 4, bLightEnable4 );
	pd3dDevice->SetLight ( 4, &pSrcLight4 );

	pd3dDevice->LightEnable ( 5, bLightEnable5 );
	pd3dDevice->SetLight ( 5, &pSrcLight5 );

	return S_OK;
}

HRESULT	DxEffCharLevel::Render_Flow( const LPDIRECT3DDEVICEQ pd3dDevice,
									SMATERIAL_PIECE* pMaterialPiece,
									const DXFLOW& cData,
									const BOOL bPieceRender, 
									const D3DXMATRIX& matStaticLinkBone, 
									const D3DXMATRIX* arrayBoneMatrices )
{
	if ( !DxSurfaceTex::GetInstance().IsFlowOneDraw() )
	{
		DxImageMove ( pd3dDevice, m_textureResFlow.GetTexture(), DxSurfaceTex::GetInstance().m_pFlowSuf,
					m_vMove, TRUE, 0L, 0xffffffff, m_vTexUV01, 128 );
		DxSurfaceTex::GetInstance().SetFlowUSE ();	// �ѹ� �ߴٴ� �� �ν� ��Ų��.
	}

	for ( DWORD i=0; i<m_dwMaterials; i++ )
	{
		if ( cData.m_bUse )
		{
			float fData;

			fData = cData.m_cColor.a;
			m_pMaterials[i].d3dMaterial.Diffuse.a = fData;

			fData = cData.m_cColor.r;
			m_pMaterials[i].d3dMaterial.Diffuse.r = fData;

			fData = cData.m_cColor.g;
			m_pMaterials[i].d3dMaterial.Diffuse.g = fData;

			fData = cData.m_cColor.b;
			m_pMaterials[i].d3dMaterial.Diffuse.b = fData;
		}
		else
		{
			m_pMaterials[i].d3dMaterial.Diffuse.a = 1.f;
			m_pMaterials[i].d3dMaterial.Diffuse.r = 1.f;
			m_pMaterials[i].d3dMaterial.Diffuse.g = 1.f;
			m_pMaterials[i].d3dMaterial.Diffuse.b = 1.f;
		}
	}

	pd3dDevice->SetTexture ( 1, DxSurfaceTex::GetInstance().m_pFlowTex );

	m_pSavedSB_F->Capture();
	m_pDrawSB_F->Apply();

	//if ( bDest2X )	pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_DESTCOLOR );
	//else			pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );

	//pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_DESTCOLOR );

	CHARSETTING sCharSetting;
	switch( RENDERPARAM::emCharRenderTYPE )
	{
	case EMCRT_SOFTWARE:
	case EMCRT_NORMAL:

		sCharSetting.pMeshContainerBase = m_pmcMesh;
		sCharSetting.pmtrlPiece			= pMaterialPiece;
		sCharSetting.pmtrlSpecular		= m_pMaterials;
		sCharSetting.bWorldIdentity		= bPieceRender;
		sCharSetting.emRDOP				= CTOP_BASE;

		m_pSkinMesh->SetDrawState ( FALSE, FALSE, FALSE, FALSE );
		m_pSkinMesh->DrawMeshContainer ( pd3dDevice, sCharSetting, m_rSkeleton, matStaticLinkBone, arrayBoneMatrices );
		break;


	case EMCRT_VERTEX:
	case EMCRT_PIXEL:

		//sCharSetting.pMeshContainerBase = m_pmcMesh;
		//sCharSetting.pmtrlPiece			= pMaterialPiece;
		//sCharSetting.pmtrlSpecular		= m_pMaterials;
		//sCharSetting.bWorldIdentity		= bPieceRender;
		//sCharSetting.emRDOP				= CTOP_BASE;

		//DxSkinMesh9_HLSL::m_pFX = NSCHARHLSL::m_pReflectFX;
		//m_pSkinMesh->SetDrawState ( FALSE, FALSE, FALSE, FALSE );
		//m_pSkinMesh->DrawMeshContainer ( pd3dDevice, sCharSetting, m_rSkeleton );
		break;
	};
	

	m_pSavedSB_F->Apply();

	pd3dDevice->SetTexture ( 1, NULL );

	for ( DWORD i=0; i<m_dwMaterials; i++ )
	{
		m_pMaterials[i].d3dMaterial.Diffuse.a = 1.f;
		m_pMaterials[i].d3dMaterial.Diffuse.r = 1.f;
		m_pMaterials[i].d3dMaterial.Diffuse.g = 1.f;
		m_pMaterials[i].d3dMaterial.Diffuse.b = 1.f;
	}

	return S_OK;
}

HRESULT	DxEffCharLevel::Render_Glow( const LPDIRECT3DDEVICEQ pd3dDevice,
									SMATERIAL_PIECE* pMaterialPiece,
									const DXGLOW& cData,
									const BOOL bPieceRender, 
									const D3DXMATRIX& matStaticLinkBone, 
									const D3DXMATRIX* arrayBoneMatrices )
{
	HRESULT hr = S_OK;

	if ( !DxGlowMan::GetInstance().IsOptionEnable() )	return S_OK;	// �ɼǿ��� ���� ������ �������� �ʴ´�.

	if ( !DxSurfaceTex::GetInstance().m_pGlowSuf_SRC )
		return S_OK;

	LPDIRECT3DSURFACEQ	pSrcSurface, pSrcZBuffer;
	pd3dDevice->GetRenderTarget ( 0, &pSrcSurface );
	pd3dDevice->GetDepthStencilSurface ( &pSrcZBuffer );

	// ���� ������ ȭ�鸸 ����ǵ��� �Ѵ�.
	// �̴ϸ�, UI, �ݻ� ����� ������ �ʵ��� ��.
	pd3dDevice->GetRenderTarget( 0, &pSrcSurface );

	if ( pSrcSurface == DxSurfaceTex::GetInstance().m_pColorBuffer || 
		pSrcSurface == DxSurfaceTex::GetInstance().m_pColorSuf16F_HDR ||
		pSrcSurface == DxPostEffect::GetInstance().GetFinalSurface() )
	{
		// ���� ����
	}
	else
	{
		SAFE_RELEASE ( pSrcSurface );
		SAFE_RELEASE ( pSrcZBuffer );
		return S_OK;
	}

	pd3dDevice->SetRenderTarget ( 0, DxSurfaceTex::GetInstance().m_pGlowSuf_SRC );
	pd3dDevice->SetDepthStencilSurface ( pSrcZBuffer );
	DxGlowMan::GetInstance().SetGlowON ();


	//m_pTempTex = NULL;
	//if ( m_dwFlag & USE_GLOW_USER )		m_pTempTex = m_pGlowTex;

	//if ( m_dwFlag & USE_ALL_TEX )
	//{
	//	for ( DWORD i=0; i<m_dwMaterials; i++ )
	//	{
	//		m_pMaterials[i].pEffTex = m_pTempTex;

	//		m_pMaterials_EffUse[i] = m_pMaterials[i].bEffUse;	// Use ���
	//		m_pMaterials[i].SetType( EMCET_BASE );						// ����
	//	}
	//}
	//else //USE_SELECT_TEX
	//{
	//	for ( DWORD i=0; i<m_dwMaterials; i++ )
	//	{
	//		if ( m_pMaterials[i].bEffUse )	m_pMaterials[i].pEffTex = m_pTempTex;
	//		else							m_pMaterials[i].pEffTex = m_pBlackTex;

	//		m_pMaterials_EffUse[i] = m_pMaterials[i].bEffUse;	// Use ���
	//		m_pMaterials[i].SetType( EMCET_BASE );						// ����
	//	}
	//}

	for ( DWORD i=0; i<m_dwMaterials; i++ )
	{
		if ( cData.m_bUse )
		{
			float fData;

			fData = cData.m_cColor.a;
			m_pMaterials[i].d3dMaterial.Diffuse.a = fData;

			fData = cData.m_cColor.r;
			m_pMaterials[i].d3dMaterial.Diffuse.r = fData;

			fData = cData.m_cColor.g;
			m_pMaterials[i].d3dMaterial.Diffuse.g = fData;

			fData = cData.m_cColor.b;
			m_pMaterials[i].d3dMaterial.Diffuse.b = fData;
		}
		else
		{
			m_pMaterials[i].d3dMaterial.Diffuse.a = 1.f;
			m_pMaterials[i].d3dMaterial.Diffuse.r = 1.f;
			m_pMaterials[i].d3dMaterial.Diffuse.g = 1.f;
			m_pMaterials[i].d3dMaterial.Diffuse.b = 1.f;
		}
	}

	//	Draw 
	m_pSavedSB_G->Capture();
	m_pDrawSB_G->Apply();

	CHARSETTING sCharSetting;
	sCharSetting.pMeshContainerBase = m_pmcMesh;
	sCharSetting.pmtrlPiece			= pMaterialPiece;
	sCharSetting.pmtrlSpecular		= m_pMaterials;
	sCharSetting.bWorldIdentity		= bPieceRender;
	sCharSetting.emRDOP				= CTOP_BASE;
	
	m_pSkinMesh->SetDrawState ( FALSE, FALSE, FALSE, FALSE );
	m_pSkinMesh->DrawMeshContainer ( pd3dDevice, sCharSetting, m_rSkeleton, matStaticLinkBone, arrayBoneMatrices );

	m_pSavedSB_G->Apply();

	for ( DWORD i=0; i<m_dwMaterials; i++ )
	{
		m_pMaterials[i].d3dMaterial.Diffuse.a = 1.f;
		m_pMaterials[i].d3dMaterial.Diffuse.r = 1.f;
		m_pMaterials[i].d3dMaterial.Diffuse.g = 1.f;
		m_pMaterials[i].d3dMaterial.Diffuse.b = 1.f;
	}

	//if ( m_dwFlag & USE_ALL_TEX )
	//{
	//	for ( DWORD i=0; i<m_dwMaterials; i++ )
	//	{
	//		m_pMaterials[i].pEffTex = NULL;
	//		m_pMaterials[i].bEffUse = m_pMaterials_EffUse[i];	// Use ���
	//	}
	//}
	//else //USE_SELECT_TEX
	//{
	//	for ( DWORD i=0; i<m_dwMaterials; i++ )
	//	{
	//		m_pMaterials[i].pEffTex = NULL;
	//		m_pMaterials[i].bEffUse = m_pMaterials_EffUse[i];	// Use ���
	//		
	//	}
	//}

	pd3dDevice->SetRenderTarget ( 0, pSrcSurface );
	pd3dDevice->SetDepthStencilSurface ( pSrcZBuffer );
	SAFE_RELEASE ( pSrcSurface );
	SAFE_RELEASE ( pSrcZBuffer );

	return S_OK;
}

void DxEffCharLevel::RenderMaterialGlow ( LPDIRECT3DDEVICEQ pd3dDevice, 
										 SMATERIAL_PIECE* pMaterialSystem,
										 const D3DXMATRIX& matStaticLinkBone,
										 DxSkinMesh9_Material& sSkinMesh, 		// sSkinMesh ������ �־�� �ϴ� �Ű�����
										 const DxPhysXCloth* pPhysXCloth )
{
	// DXMATERIAL_CHAR_EFF Ȯ�� �� �����۾�.
	if ( !IsExist_Copy_pmcMesh() )
		return;

	// ���� ������ ȭ�鸸 ����ǵ��� �Ѵ�.
	// �̴ϸ�, UI, �ݻ� ����� ������ �ʵ��� ��.
	{
		LPDIRECT3DSURFACEQ	pSrcSurface;
		pd3dDevice->GetRenderTarget( 0, &pSrcSurface );

		if ( pSrcSurface == DxSurfaceTex::GetInstance().m_pColorBuffer || 
			pSrcSurface == DxSurfaceTex::GetInstance().m_pColorSuf16F_HDR ||
			pSrcSurface == DxPostEffect::GetInstance().GetFinalSurface() )
		{
			// ���� ����
		}
		else
		{
			SAFE_RELEASE ( pSrcSurface );
			return;
		}
		SAFE_RELEASE ( pSrcSurface );
	}

	DxSkinMesh9_Material* pSkinMesh = &sSkinMesh;

	// ������ ����������� ��.
	if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_PS_1_1 )
		return;

	const D3DXMATRIX*		pBoneMatrices(NULL);
	if ( m_pCharPart )
	{
		pBoneMatrices = m_pCharPart->GetBoneMatrices();
		if ( !pBoneMatrices )
			return;
	}
	else if ( m_pSkinPiece )
	{
		pBoneMatrices = DxSkinMesh9::g_pBoneMatrices;
	}

	//	Note : CharEdit ������ ���̴� ���̴�.
	int nLevel(0);
	if( g_bCHAR_EDIT_RUN )	
	{
		nLevel = g_nITEMLEVEL;
	}
	else if( m_pCharPart )
	{
		nLevel = m_pCharPart->GetGrindLevel();
	}

	// ����ġ�� ����� ���۵��� �ʵ��� �Ѵ�.
	if( nLevel == 0 || nLevel > LEVEL_LIMIT )	
		return;

	const DXGLOW& cData = m_OpLEVEL[nLevel].m_Glow;

	if ( !cData.m_bUse )
		return;

	if ( !DxGlowMan::GetInstance().IsOptionEnable() )
		return;	// �ɼǿ��� ���� ������ �������� �ʴ´�.

	if ( !DxSurfaceTex::GetInstance().m_pGlowSuf_SRC )
		return;

	DxBackupTarget sBackupTarget;
	sBackupTarget.Backup( pd3dDevice );
	
	pd3dDevice->SetRenderTarget ( 0, DxSurfaceTex::GetInstance().m_pGlowSuf_SRC );
	DxGlowMan::GetInstance().SetGlowON ();

	for ( DWORD i=0; i<m_dwMaterials; i++ )
	{
		float fData;

		// 0.4f �� ���ϴ� ������ ���� Material System �� ���鼭 mask ���� ��� ������� ������..
		// �װ� �ǵ����� ���ؼ� FX �� PS_GlowInLevel ���� 0.4f �� �ص� �ǰ�, �ƴ� ���⼭ 0.4f �� �ص���.
		// ����ȭ�� ���⼭ 0.4f �� ���Ѵ�.

		//fData = cData.m_cColor.a;
		m_pMaterials[i].d3dMaterial.Diffuse.a = 1.f;

		fData = cData.m_cColor.r;
		m_pMaterials[i].d3dMaterial.Diffuse.r = fData * cData.m_cColor.a * 0.4f;

		fData = cData.m_cColor.g;
		m_pMaterials[i].d3dMaterial.Diffuse.g = fData * cData.m_cColor.a * 0.4f;

		fData = cData.m_cColor.b;
		m_pMaterials[i].d3dMaterial.Diffuse.b = fData * cData.m_cColor.a * 0.4f;
	}

	for ( DWORD i=0; i<m_dwMaterials; i++ )
	{
		if ( pMaterialSystem[i].m_pBakeMask2Tex )
		{
			m_pMaterials[i].SetTexture ( pMaterialSystem[i].m_pBakeMask2Tex );
		}
		else
		{
			m_pMaterials[i].SetTexture ( pMaterialSystem[i].m_textureRes_mask2.GetTexture() );
		}
	}

	//	Draw 
	m_pSavedSB_G->Capture();
	m_pDrawSB_G->Apply();

	pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,		D3DBLEND_ONE );

	pSkinMesh->RenderMaterialGlow( pd3dDevice, m_pmcMesh, pBoneMatrices, pMaterialSystem, m_pMaterials, 1, matStaticLinkBone, pPhysXCloth );

	m_pSavedSB_G->Apply();

	for ( DWORD i=0; i<m_dwMaterials; i++ )
	{
		m_pMaterials[i].SetTexture ( NULL );
	}

	for ( DWORD i=0; i<m_dwMaterials; i++ )
	{
		m_pMaterials[i].d3dMaterial.Diffuse.a = 1.f;
		m_pMaterials[i].d3dMaterial.Diffuse.r = 1.f;
		m_pMaterials[i].d3dMaterial.Diffuse.g = 1.f;
		m_pMaterials[i].d3dMaterial.Diffuse.b = 1.f;
	}
}

HRESULT DxEffCharLevel::LoadFile_0100 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	EFFCHAR_PROPERTY_LEVEL_100	pProp;

	//	Note : ����Ʈ�� Property �� ����.
	//
	SFile.ReadBuffer ( &pProp, sizeof(EFFCHAR_PROPERTY_LEVEL_100) );

	m_dwColorOP = D3DTOP_MODULATE;
	m_dwFlag	= pProp.m_dwFlag;

	m_cSpecular	= D3DXCOLOR ( 0.8f, 0.8f, 0.8f, 1.0f );	// Ver.101	�߰�

	SAFE_DELETE_ARRAY ( m_pTempMaterials );
	m_pTempMaterials = new DXMATERIAL_CHAR_EFF[4];
	m_dwTempMaterials = 4;

	if ( m_dwFlag&USETEXTURE1 )			m_pTempMaterials[0].SetType( EMCET_BASE );
	if ( m_dwFlag&USETEXTURE2 )			m_pTempMaterials[1].SetType( EMCET_BASE );
	if ( m_dwFlag&USETEXTURE3 )			m_pTempMaterials[2].SetType( EMCET_BASE );
	if ( m_dwFlag&USETEXTURE4 )			m_pTempMaterials[3].SetType( EMCET_BASE );

	StringCchCopy( m_pTempMaterials[0].szTexture, MAX_PATH, pProp.m_szSrcTex_1 );
	StringCchCopy( m_pTempMaterials[1].szTexture, MAX_PATH, pProp.m_szSrcTex_2 );
	StringCchCopy( m_pTempMaterials[2].szTexture, MAX_PATH, pProp.m_szSrcTex_3 );
	StringCchCopy( m_pTempMaterials[3].szTexture, MAX_PATH, pProp.m_szSrcTex_4 );

	StringCchCopy( m_pTempMaterials[0].szEffTex, MAX_PATH, pProp.m_szTexture_1 );
	StringCchCopy( m_pTempMaterials[1].szEffTex, MAX_PATH, pProp.m_szTexture_2 );
	StringCchCopy( m_pTempMaterials[2].szEffTex, MAX_PATH, pProp.m_szTexture_3 );
	StringCchCopy( m_pTempMaterials[3].szEffTex, MAX_PATH, pProp.m_szTexture_4 );

	char szChar[MAX_PATH] = {0};
	StringCchCopy( szChar, MAX_PATH, "1d_Lighting.bmp" );
	for ( DWORD i=0; i<4; i++ )
	{
		if ( !strcmp ( m_pTempMaterials[i].szEffTex, szChar ) )		// ���ڰ� ������� 0�� ����
		{
			StringCchCopy( m_pTempMaterials[i].szEffTex, MAX_PATH, m_pTempMaterials[i].szTexture );
		}
	}

	m_bMaterialSystem = FALSE;

	return S_OK;
}

HRESULT DxEffCharLevel::LoadFile_0101 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	EFFCHAR_PROPERTY_LEVEL_101	pProp;

	//	Note : ����Ʈ�� Property �� ����.
	//
	SFile.ReadBuffer ( &pProp, sizeof(EFFCHAR_PROPERTY_LEVEL_101) );

	m_dwColorOP = D3DTOP_MODULATE;
	m_dwFlag	= pProp.m_dwFlag;

	m_cSpecular	= pProp.m_cSpecular;

	SAFE_DELETE_ARRAY ( m_pTempMaterials );
	m_pTempMaterials = new DXMATERIAL_CHAR_EFF[4];
	m_dwTempMaterials = 4;

	if ( m_dwFlag&USETEXTURE1 )			m_pTempMaterials[0].SetType( EMCET_BASE );
	if ( m_dwFlag&USETEXTURE2 )			m_pTempMaterials[1].SetType( EMCET_BASE );
	if ( m_dwFlag&USETEXTURE3 )			m_pTempMaterials[2].SetType( EMCET_BASE );
	if ( m_dwFlag&USETEXTURE4 )			m_pTempMaterials[3].SetType( EMCET_BASE );

	StringCchCopy( m_pTempMaterials[0].szTexture, MAX_PATH, pProp.m_szSrcTex_1 );
	StringCchCopy( m_pTempMaterials[1].szTexture, MAX_PATH, pProp.m_szSrcTex_2 );
	StringCchCopy( m_pTempMaterials[2].szTexture, MAX_PATH, pProp.m_szSrcTex_3 );
	StringCchCopy( m_pTempMaterials[3].szTexture, MAX_PATH, pProp.m_szSrcTex_4 );

	StringCchCopy( m_pTempMaterials[0].szEffTex, MAX_PATH, pProp.m_szTexture_1 );
	StringCchCopy( m_pTempMaterials[1].szEffTex, MAX_PATH, pProp.m_szTexture_2 );
	StringCchCopy( m_pTempMaterials[2].szEffTex, MAX_PATH, pProp.m_szTexture_3 );
	StringCchCopy( m_pTempMaterials[3].szEffTex, MAX_PATH, pProp.m_szTexture_4 );

	char szChar[MAX_PATH] = {0};
	StringCchCopy( szChar, MAX_PATH, "1d_Lighting.bmp" );
	for ( DWORD i=0; i<4; i++ )
	{
		if ( !strcmp ( m_pTempMaterials[i].szEffTex, szChar ) )		// ���ڰ� ������� 0�� ����
		{
			StringCchCopy( m_pTempMaterials[i].szEffTex, MAX_PATH, m_pTempMaterials[i].szTexture );
		}
	}

	m_bMaterialSystem = FALSE;

	return S_OK;
}

HRESULT DxEffCharLevel::LoadFile_0102 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	EFFCHAR_PROPERTY_LEVEL_102_103	pProp;

	//	Note : ����Ʈ�� Property �� ����.
	//
	SFile.ReadBuffer ( &pProp, sizeof(EFFCHAR_PROPERTY_LEVEL_102_103) );

	m_cSpecular		= pProp.m_cSpecular;
	m_dwColorOP		= D3DTOP_MODULATE;
	m_dwFlag		= pProp.m_dwFlag;
	m_dwMaterials	= pProp.m_dwMaterials;

	SFile >> m_dwMaterials;
	if ( m_dwMaterials )
	{
		DXMATERIAL_SPECULAR*	pMaterials;
		pMaterials = new DXMATERIAL_SPECULAR[m_dwMaterials];
		SFile.ReadBuffer ( pMaterials, sizeof(DXMATERIAL_SPECULAR)*m_dwMaterials );

		SAFE_DELETE_ARRAY ( m_pMaterials );
		m_pMaterials = new DXMATERIAL_CHAR_EFF[m_dwMaterials];
		for ( DWORD i=0; i<m_dwMaterials; ++i )
		{
			m_pMaterials[i].SetType( pMaterials[i] );
			m_pMaterials[i].d3dMaterial.Power	= 14.f;
			StringCchCopy( m_pMaterials[i].szEffTex,	MAX_PATH, pMaterials[i].szSpecTex );
			StringCchCopy( m_pMaterials[i].szTexture,	MAX_PATH, pMaterials[i].szTexture );
		}

		SAFE_DELETE_ARRAY ( pMaterials );

		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
            m_pMaterials[i].LoadTexture( EMMM_CHAR_MIPMAP, true );
		}
	}

	m_bMaterialSystem = FALSE;

	return S_OK;
}

HRESULT DxEffCharLevel::LoadFile_0103 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	
	EFFCHAR_PROPERTY_LEVEL_102_103	pProp;

	//	Note : ����Ʈ�� Property �� ����.
	//
	SFile.ReadBuffer ( &pProp, sizeof(EFFCHAR_PROPERTY_LEVEL_102_103) );

	m_cSpecular		= pProp.m_cSpecular;
	m_dwColorOP		= D3DTOP_MODULATE;
	m_dwFlag		= pProp.m_dwFlag;
	m_dwMaterials	= pProp.m_dwMaterials;

	SFile >> m_dwMaterials;
	if ( m_dwMaterials )
	{
		DXMATERIAL_CHAR_EFF_100*	pMaterials;
		pMaterials = new DXMATERIAL_CHAR_EFF_100[m_dwMaterials];
		SFile.ReadBuffer ( pMaterials, sizeof(DXMATERIAL_CHAR_EFF_100)*m_dwMaterials );

		SAFE_DELETE_ARRAY ( m_pMaterials );
		m_pMaterials = new DXMATERIAL_CHAR_EFF[m_dwMaterials];
		for ( DWORD i=0; i<m_dwMaterials; ++i )
		{
			m_pMaterials[i].SetType( pMaterials[i] );
			m_pMaterials[i].d3dMaterial.Power	= pMaterials[i].fMaterial_Power;
			StringCchCopy( m_pMaterials[i].szEffTex,	MAX_PATH, pMaterials[i].szEffTex );
			StringCchCopy( m_pMaterials[i].szTexture,	MAX_PATH, pMaterials[i].szTexture );
		}

		SAFE_DELETE_ARRAY ( pMaterials );

		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
			m_pMaterials[i].LoadTexture( EMMM_CHAR_MIPMAP, true );
		}
	}

	m_bMaterialSystem = FALSE;

	return S_OK;
}

HRESULT DxEffCharLevel::LoadFile_0104 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : ����Ʈ�� Property �� ����.
	//
	EFFCHAR_PROPERTY_LEVEL_104_105 pProperty;

	SFile.ReadBuffer ( &pProperty, sizeof(EFFCHAR_PROPERTY_LEVEL_104_105) );
	m_Property.Assign( pProperty );

	SFile >> m_dwMaterials;
	if ( m_dwMaterials )
	{
		DXMATERIAL_CHAR_EFF_100*	pMaterials;
		pMaterials = new DXMATERIAL_CHAR_EFF_100[m_dwMaterials];
		SFile.ReadBuffer ( pMaterials, sizeof(DXMATERIAL_CHAR_EFF_100)*m_dwMaterials );

		SAFE_DELETE_ARRAY ( m_pMaterials );
		m_pMaterials = new DXMATERIAL_CHAR_EFF[m_dwMaterials];
		for ( DWORD i=0; i<m_dwMaterials; ++i )
		{
			m_pMaterials[i].SetType( pMaterials[i] );
			m_pMaterials[i].d3dMaterial.Power	= pMaterials[i].fMaterial_Power;
			StringCchCopy( m_pMaterials[i].szEffTex,	MAX_PATH, pMaterials[i].szEffTex );
			StringCchCopy( m_pMaterials[i].szTexture,	MAX_PATH, pMaterials[i].szTexture );
		}

		SAFE_DELETE_ARRAY ( pMaterials );

		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
			m_pMaterials[i].LoadTexture( EMMM_CHAR_MIPMAP, true );
		}
	}

	m_bMaterialSystem = FALSE;

	return S_OK;
}

HRESULT DxEffCharLevel::LoadFile_0105 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : ����Ʈ�� Property �� ����.
	//
	EFFCHAR_PROPERTY_LEVEL_104_105 pProperty;

	SFile.ReadBuffer ( &pProperty, sizeof(EFFCHAR_PROPERTY_LEVEL_104_105) );
	m_Property.Assign( pProperty );

	SFile >> m_dwMaterials;
	if ( m_dwMaterials )
	{
		SAFE_DELETE_ARRAY ( m_pMaterials );
		m_pMaterials = new DXMATERIAL_CHAR_EFF[m_dwMaterials];

		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
            m_pMaterials[i].LoadFile( SFile );
            m_pMaterials[i].LoadTexture( EMMM_CHAR_MIPMAP, true );
		}
	}

	m_bMaterialSystem = FALSE;

	return S_OK;
}

HRESULT DxEffCharLevel::LoadFile_0106 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : ����Ʈ�� Property �� ����.
	//
	SFile.ReadBuffer ( &m_Property, sizeof(m_Property) );

	SFile >> m_dwMaterials;
	if ( m_dwMaterials )
	{
		SAFE_DELETE_ARRAY ( m_pMaterials );
		m_pMaterials = new DXMATERIAL_CHAR_EFF[m_dwMaterials];

		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
            m_pMaterials[i].LoadFile( SFile );
			m_pMaterials[i].LoadTexture( EMMM_CHAR_MIPMAP, true );
		}
	}

	m_bMaterialSystem = FALSE;

	return S_OK;
}

HRESULT DxEffCharLevel::LoadFile_0107 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : ����Ʈ�� Property �� ����.
	//
	SFile.ReadBuffer ( &m_Property, sizeof(m_Property) );

	SFile >> m_dwMaterials;
	if ( m_dwMaterials )
	{
		SAFE_DELETE_ARRAY ( m_pMaterials );
		m_pMaterials = new DXMATERIAL_CHAR_EFF[m_dwMaterials];

		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
			m_pMaterials[i].LoadFile( SFile );
			m_pMaterials[i].ClearEffTex();
		}
	}

	return S_OK;
}


HRESULT	DxEffCharLevel::LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_bMaterialSystem = TRUE;

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
	case 0x0104:	LoadFile_0104( SFile, pd3dDevice );	break;
	case 0x0105:	LoadFile_0105( SFile, pd3dDevice );	break;
	case 0x0106:	LoadFile_0106( SFile, pd3dDevice );	break;
	case VERSION:	LoadFile_0107( SFile, pd3dDevice );	break;
	default:
		{
			//	Note : ������ Ʋ�� ��쿡�� ���Ͽ� ������ DATA������ �Ǵ� ��� �۾��� ����.
			//
			SFile.SetOffSet ( SFile.GetfTell()+dwSize );
		
			return E_FAIL;
		}
		break;
	};

	//	Note : Device �ڿ��� �����Ѵ�.
	//
	hr = Create ( pd3dDevice );
	if ( FAILED(hr) )	return hr;

	return S_OK;
}

HRESULT	DxEffCharLevel::SaveFile ( sc::BaseStream &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	//	Note : DATA�� ����� ����Ѵ�. Load �ÿ� ������ Ʋ�� ��� ����.
	//
	SFile << (DWORD) ( sizeof(m_Property) + sizeof(DWORD) + sizeof(DXMATERIAL_CHAR_EFF_OLD)*m_dwMaterials );

	//	Note : ����Ʈ�� Property �� ����.
	//
	SFile.WriteBuffer ( &m_Property, sizeof(m_Property) );

	SFile << m_dwMaterials;
    for( DWORD i=0; i<m_dwMaterials; ++i )
        m_pMaterials[ i ].SaveFile( SFile );

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// Material System �� ���� ȿ������ ���̱� ���� �۾�.

// ���α׷��� MaskMap �� ����� ���� �۾�.
void DxEffCharLevel::ExportEffTexture( LPDIRECT3DDEVICEQ pd3dDevice, 
									  DWORD dwMaterialID, 
									  LPDIRECT3DTEXTUREQ* ppLevelTex, 
									  LPDIRECT3DTEXTUREQ* ppCubeTex,
									  bool bTexThread,
									  const TCHAR* pTextureName_NoHave )
{
	if ( dwMaterialID >= m_dwMaterials )
		return;

	if ( m_pMaterials[dwMaterialID].IsType( EMCET_BASE ) )
	{
		size_t nSize(0);
		StringCchLength( m_pMaterials[dwMaterialID].szEffTex, MAX_PATH, &nSize );
		if ( nSize == 0 )
			return;

		m_pMaterials[dwMaterialID].LoadTexture( EMMM_CHAR_MIPMAP, bTexThread );
		(*ppLevelTex) = m_pMaterials[dwMaterialID].GetTexture();

		// ���� ���� ���� �־��ش�.
		if ( !(*ppLevelTex) )
		{
			pTextureName_NoHave = m_pMaterials[dwMaterialID].szEffTex;
		}
	}
}

// Effect Texture ���� �� �ε��� �Ǿ��°�~?
BOOL DxEffCharLevel::IsEnableEffTexture( BOOL bTexThread )
{
	for( DWORD i=0; i<m_dwMaterials; ++i )
	{
		if ( m_pMaterials[i].IsType( EMCET_BASE ) )
		{
			// ���ڰ� ���ٸ� �۾�����.
			size_t nSize(0);
			StringCchLength( m_pMaterials[i].szEffTex, MAX_PATH, &nSize );
			if ( nSize == 0 )
				continue;

			if ( !m_pMaterials[i].GetTexture() )
			{
				// [shhan][2014.10.01] �ؽ��� ��ü�� ���� ������ ��� �Ҽ��� ����.
				//						��� Ÿ�� ���� �׳� ������ �ϵ��� �Ѵ�.
				if ( !TextureManager::GetInstance().IsPathFile( m_pMaterials[i].szEffTex ) )
					return TRUE;

				return FALSE;
			}
		}
	}

	return TRUE;
}

// PrecalculateForMaterialSystem�� 2�� �� ���� �̸� ����� ���°Ŷ��, �̰����� ������ �ȴ�.
void DxEffCharLevel::PrecalculateForMaterialSystem( LPDIRECT3DDEVICEQ pd3dDevice, 
												   EffectMaterialData* pEffMaterialData, 
												   const D3DXMATRIX* pMatrix, 
												   const float fScale )
{
	pEffMaterialData->m_rLevelData = NULL;

	// ������ ����������� ��.
	if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_PS_1_1 )
		return;

	//	Note : CharEdit ������ ���̴� ���̴�.
	int nLevel(0);
	if( g_bCHAR_EDIT_RUN )	
	{
		nLevel = g_nITEMLEVEL;
	}
	else if( m_pCharPart )
	{
		nLevel = m_pCharPart->GetGrindLevel();
	}

	// ����ġ�� ����� ���۵��� �ʵ��� �Ѵ�.
	if( nLevel == 0 || nLevel > LEVEL_LIMIT )	
		return;

	{
		if ( m_OpLEVEL[nLevel].m_Ambient.m_bUse )
		{
			m_sEffMaterialData_Level.m_bLevel = TRUE;

			float fData;
			float fFact = (m_fLevelTime*m_OpLEVEL[nLevel].m_Ambient.m_fDynamicColor) + m_OpLEVEL[nLevel].m_Ambient.m_fBaseColor;

			fData = m_OpLEVEL[nLevel].m_Ambient.m_cColor.r*fFact;
			fData = (fData>1.f) ? 1.f : fData;
			m_sEffMaterialData_Level.m_vLevelColor.x = fData;

			fData = m_OpLEVEL[nLevel].m_Ambient.m_cColor.g*fFact;
			fData = (fData>1.f) ? 1.f : fData;
			m_sEffMaterialData_Level.m_vLevelColor.y = fData;

			fData = m_OpLEVEL[nLevel].m_Ambient.m_cColor.b*fFact;
			fData = (fData>1.f) ? 1.f : fData;
			m_sEffMaterialData_Level.m_vLevelColor.z = fData;

			if ( fFact > 1.f )
			{
				m_sEffMaterialData_Level.m_vLevelColor.w = 1.f;
			}
			else if ( fFact < 0.f )
			{
				m_sEffMaterialData_Level.m_vLevelColor.w = 0.f;
			}
			else
			{
				m_sEffMaterialData_Level.m_vLevelColor.w = fFact;
			}
		}
		else
		{
			m_sEffMaterialData_Level.m_bLevel = FALSE;
		}

		if ( m_OpLEVEL[nLevel].m_Flow.m_bUse )
		{
			m_sEffMaterialData_Level.m_bLevelFlow = TRUE;

			m_sEffMaterialData_Level.m_rLevelFlowTex = DxSurfaceTex::GetInstance().m_pFlowTex;

			m_sEffMaterialData_Level.m_vLevelFlowColor.x = m_OpLEVEL[nLevel].m_Flow.m_cColor.r;
			m_sEffMaterialData_Level.m_vLevelFlowColor.y = m_OpLEVEL[nLevel].m_Flow.m_cColor.g;
			m_sEffMaterialData_Level.m_vLevelFlowColor.z = m_OpLEVEL[nLevel].m_Flow.m_cColor.b;
		}
		else
		{
			m_sEffMaterialData_Level.m_bLevelFlow = FALSE;
		}
	}

	pEffMaterialData->m_rLevelData = &m_sEffMaterialData_Level;

	if ( !DxSurfaceTex::GetInstance().IsFlowOneDraw() )
	{
		DxImageMove ( pd3dDevice, m_textureResFlow.GetTexture(), DxSurfaceTex::GetInstance().m_pFlowSuf,
			m_vMove, TRUE, 0L, 0xffffffff, m_vTexUV01, 128 );
		DxSurfaceTex::GetInstance().SetFlowUSE ();	// �ѹ� �ߴٴ� �� �ν� ��Ų��.
	}
}

BOOL DxEffCharLevel::IsExist_Copy_pmcMesh()
{
	// ���� �ִٸ� �� �۾����� �ʴ´�.
	if ( m_pmcMesh )
		return TRUE;

	if ( m_pCharPart )
	{
		m_pmcMesh = m_pCharPart->m_rMeshContainer;
	}
	else if ( m_pSkinPiece )
	{
		m_pmcMesh = m_pSkinPiece->m_pmcMesh;
	}

	if ( !m_pmcMesh )
		return FALSE;

	// Material System �� ���� ���̶�� �ؿ� �۾��� �� �ʿ䰡 ����. ( Ver.0x0107 �̻� )
	// CharEdit ���� ó�� ȿ���� �����ϸ� m_pMaterials �� NULL �� ���´�.
	if ( m_bMaterialSystem && m_pMaterials )
	{
		// Material ������ �����Ǿ��� �� �����Ƿ� ���� Ȯ�� �� �۾��� �ؾ��Ѵ�.
		if ( m_dwMaterials != m_pmcMesh->GetNumMaterials() )
		{
			m_dwMaterials = m_pmcMesh->GetNumMaterials();		// ��Ʈ���� ������ �ٲ���� �� �׳� ������ ����Ʈ ����
			SAFE_DELETE_ARRAY ( m_pMaterials );
			m_pMaterials = new DXMATERIAL_CHAR_EFF[m_dwMaterials];

			for ( DWORD i=0; i<m_dwMaterials; i++ )
			{
				if( m_pmcMesh->pMaterials[i].pTextureFilename )
					StringCchCopy( m_pMaterials[i].szTexture,	MAX_PATH, m_pmcMesh->pMaterials[i].pTextureFilename );
			}
		}

		return TRUE;
	}

	if ( m_pTempMaterials )
	{
		m_dwMaterials = m_pmcMesh->GetNumMaterials();
		SAFE_DELETE_ARRAY ( m_pMaterials );
		m_pMaterials = new DXMATERIAL_CHAR_EFF[m_dwMaterials];

		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
			if( m_pmcMesh->pMaterials[i].pTextureFilename )
				StringCchCopy( m_pMaterials[i].szTexture, MAX_PATH, m_pmcMesh->pMaterials[i].pTextureFilename );

			if ( i<m_dwTempMaterials )
			{
				m_pMaterials[i].SetType( m_pTempMaterials[i].GetType() );
				StringCchCopy( m_pMaterials[i].szEffTex, MAX_PATH, m_pTempMaterials[i].szEffTex );
			}
			else
			{
				m_pMaterials[i].SetType( EMCET_NULL );
				StringCchCopy( m_pMaterials[i].szEffTex, MAX_PATH, m_pMaterials[i].szTexture );
			}

			m_pMaterials[i].LoadTexture( EMMM_CHAR_MIPMAP, true );
		}

		m_dwTempMaterials = 0L;
		SAFE_DELETE_ARRAY ( m_pTempMaterials );
	}
	else if ( m_pMaterials && (m_dwMaterials != m_pmcMesh->GetNumMaterials()) )
	{
		m_dwMaterials = m_pmcMesh->GetNumMaterials();		// ��Ʈ���� ������ �ٲ���� �� �׳� ������ ����Ʈ ����
		SAFE_DELETE_ARRAY ( m_pMaterials );
		m_pMaterials = new DXMATERIAL_CHAR_EFF[m_dwMaterials];

		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
			std::string	strName;
			strName = sc::util::GetSpecularName ( m_pmcMesh->pMaterials[i].pTextureFilename, "_s" );

			if( m_pmcMesh->pMaterials[i].pTextureFilename )
				StringCchCopy( m_pMaterials[i].szTexture,	MAX_PATH, m_pmcMesh->pMaterials[i].pTextureFilename );
			StringCchCopy( m_pMaterials[i].szEffTex,	MAX_PATH, strName.c_str() );

			m_pMaterials[i].LoadTexture( EMMM_CHAR_MIPMAP, true );
		}
	}
	else if ( !m_pMaterials )							// ��Ʈ������ ���� �� ����Ʈ ����
	{
		m_dwMaterials = m_pmcMesh->GetNumMaterials();
		SAFE_DELETE_ARRAY ( m_pMaterials );
		m_pMaterials = new DXMATERIAL_CHAR_EFF[m_dwMaterials];

		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
			std::string	strName;
			strName = sc::util::GetSpecularName ( m_pmcMesh->pMaterials[i].pTextureFilename, "_s" );

			if( m_pmcMesh->pMaterials[i].pTextureFilename )
				StringCchCopy( m_pMaterials[i].szTexture,	MAX_PATH, m_pmcMesh->pMaterials[i].pTextureFilename );
			StringCchCopy( m_pMaterials[i].szEffTex,	MAX_PATH, strName.c_str() );

			m_pMaterials[i].LoadTexture( EMMM_CHAR_MIPMAP, true );
		}
	}
	else
	{
		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
			m_pMaterials[i].LoadTexture( EMMM_CHAR_MIPMAP, true );
		}
	}

	return TRUE;
}