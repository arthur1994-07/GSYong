//
//	DXMATERIAL_CHAR_EFF 이 변화하면서 0x104 버전으로 변화 하였다.
//
#include "pch.h"

#include "../../../SigmaCore/Util/Util.h"

#include "../../DxEffect/Single/DxEffSingle.h"
#include "../Single/DxEffSinglePropGMan.h"
#include "../../../SigmaCore/File/SerialFile.h"
#include "../../DxTools/RENDERPARAM.h"

#include "../../DxTools/Light/DxLightMan.h"
#include "../../DxTools/DxViewPort.h"
#include "../../DxEffect/Frame/DxEffectMan.h"

#include "../../DxMeshs/SkinMesh/DxCharPart.h"
#include "../../DxMeshs/SkinMesh/DxSkinPieceContainer.h"

#include "../../DxMeshs/SkinMesh/_new/DxSkinMesh9_HLSL.h"
#include "./DxEffCharHLSL.h"

#include "./DxEffCharReflection2.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

extern int			g_nITEMLEVEL;
extern BOOL			g_bCHAR_EDIT_RUN;

DWORD		DxEffCharReflection2::TYPEID			= EMEFFCHAR_REFLECTION2;
const DWORD	DxEffCharReflection2::VERSION			= 0x0105;
char		DxEffCharReflection2::NAME[MAX_PATH]	= "1.반사 (추가 알파맵)";

LPDIRECT3DSTATEBLOCK9		DxEffCharReflection2::m_pSavedStateBlock	= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffCharReflection2::m_pEffectStateBlock	= NULL;

DxEffCharReflection2::DxEffCharReflection2(void) :
	DxEffChar(),
	m_pSkinMesh(NULL),
	m_pmcMesh(NULL),
	m_dwTempMaterials(0L),
	m_pMaterials(NULL),
	m_pTempMaterials(NULL)
{
}

DxEffCharReflection2::~DxEffCharReflection2(void)
{
	m_dwMaterials = 0L;
	m_dwTempMaterials = 0L;

	SAFE_DELETE_ARRAY ( m_pMaterials );
	SAFE_DELETE_ARRAY ( m_pTempMaterials );
}

DxEffChar* DxEffCharReflection2::CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece, DWORD dwMaterialNum )
{
	HRESULT hr;
	DxEffCharReflection2 *pEffChar = new DxEffCharReflection2;
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

void DxEffCharReflection2::SetMaterials ( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwMaterials, DXMATERIAL_CHAR_EFF* pMaterials )
{
	SAFE_DELETE_ARRAY ( m_pMaterials );
	m_pMaterials = new DXMATERIAL_CHAR_EFF[dwMaterials];
	m_dwMaterials = dwMaterials;

	for ( DWORD i=0; i<dwMaterials; i++ )
	{
		m_pMaterials[i].SetType( pMaterials[i].GetType() );
		m_pMaterials[i].d3dMaterial = pMaterials[i].d3dMaterial;
		StringCchCopy( m_pMaterials[i].szTexture, MAX_PATH, pMaterials[i].szTexture );
		StringCchCopy( m_pMaterials[i].szEffTex, MAX_PATH, pMaterials[i].szEffTex );

		m_pMaterials[i].LoadTexture( EMMM_CHAR_MIPMAP, true );
	}
}

HRESULT DxEffCharReflection2::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	D3DCAPSQ d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		//	Note : SetRenderState() 선언
		float fBias = -0.0002f;
		pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,			*((DWORD*)&fBias) );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,			FALSE );

		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );

		if( d3dCaps.AlphaCmpCaps & D3DPCMPCAPS_GREATEREQUAL )
		{
			pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	TRUE );
			pd3dDevice->SetRenderState( D3DRS_ALPHAREF,			0x01 );
			pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC,		D3DCMP_GREATEREQUAL );
		}

		//	Note : SetTextureStageState() 선언
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


		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedStateBlock );
		else			pd3dDevice->EndStateBlock( &m_pEffectStateBlock );
	}

	return S_OK;
}

HRESULT DxEffCharReflection2::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSavedStateBlock );
	SAFE_RELEASE( m_pEffectStateBlock );

	return S_OK;
}

HRESULT DxEffCharReflection2::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
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

	// DXMATERIAL_CHAR_EFF 확인 및 생성작업.
	IsExist_Copy_pmcMesh();

	return S_OK;
}

HRESULT DxEffCharReflection2::DeleteDeviceObjects ()
{
	m_textureRes.Clear();

	SAFE_DELETE_ARRAY ( m_pMaterials );

	return S_OK;
}

HRESULT DxEffCharReflection2::FrameMove ( float fTime, float fElapsedTime )
{
	HRESULT hr = S_OK;

	return S_OK;
}

void	DxEffCharReflection2::Render( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* arrayBoneMatrices, BOOL bPieceRender )
{
	// DXMATERIAL_CHAR_EFF 확인 및 생성작업.
	if ( !IsExist_Copy_pmcMesh() )
		return;

	if ( !m_pSkinMesh )		return;

	if ( !m_textureRes.Update() )
		return;

	int nLevel = 0;

	//	Note : CharEdit 에서만 쓰이는 것이다.
	if( g_bCHAR_EDIT_RUN )	nLevel = g_nITEMLEVEL;

	if ( nLevel )														return;	// 흐르기 시작하면 반사는 끈다.
	if ( DxEffectMan::GetInstance().GetSkinDetail() == SKD_NONE )		return;
	if ( !(DxEffectMan::GetInstance().GetDetailFlag()&REALSPECULAR) )	return; 

	pd3dDevice->SetTexture ( 1, m_textureRes.GetTexture() );

	switch( RENDERPARAM::emCharRenderTYPE )
	{
	case EMCRT_SOFTWARE:
	case EMCRT_NORMAL:
		RenderNORMAL( pd3dDevice, arrayBoneMatrices, bPieceRender );
		break;
	case EMCRT_VERTEX:
	case EMCRT_PIXEL:
		RenderHLSL( pd3dDevice, bPieceRender );
		break;
	};
}

void DxEffCharReflection2::RenderNORMAL( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* arrayBoneMatrices, BOOL bPieceRender )
{
	//	빛위치
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
	pLight.Specular.r   = m_cSpecular.r;
	pLight.Specular.g   = m_cSpecular.g;
	pLight.Specular.b   = m_cSpecular.b;
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

	m_pSavedStateBlock->Capture();
	m_pEffectStateBlock->Apply();

	// Note : Get Texture
	for ( DWORD i=0; i<m_dwMaterials; i++ )
	{
		m_pMaterials[i].UpdateTexture();
	}

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

	CHARSETTING sCharSetting;
	sCharSetting.pMeshContainerBase = m_pmcMesh;
	sCharSetting.pmtrlPiece			= pMaterialPiece;
	sCharSetting.pmtrlSpecular		= m_pMaterials;
	sCharSetting.bWorldIdentity		= bPieceRender;
	sCharSetting.emRDOP				= CTOP_BASE;

	m_pSkinMesh->SetDrawState ( FALSE, FALSE, FALSE, FALSE );
	m_pSkinMesh->DrawMeshContainer ( pd3dDevice, sCharSetting, m_rSkeleton, *pmatStaticLinkBone, arrayBoneMatrices );

	m_pSavedStateBlock->Apply();

	pd3dDevice->SetTexture ( 1, NULL );

	pd3dDevice->LightEnable ( 0, TRUE );

	pd3dDevice->LightEnable ( 4, bLightEnable4 );
	pd3dDevice->SetLight ( 4, &pSrcLight4 );

	pd3dDevice->LightEnable ( 5, bLightEnable5 );
	pd3dDevice->SetLight ( 5, &pSrcLight5 );
}

void DxEffCharReflection2::RenderHLSL( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPieceRender )
{
	m_pSavedStateBlock->Capture();
	m_pEffectStateBlock->Apply();

	// Note : Get Texture
	for ( DWORD i=0; i<m_dwMaterials; i++ )
	{
		m_pMaterials[i].UpdateTexture();
	}

	m_pSkinMesh->SetDrawState ( FALSE, FALSE, FALSE, FALSE );
	DxSkinMesh9_HLSL::m_pFX = NSCHARHLSL::m_pReflectFX;
	m_pSkinMesh->DrawMeshSpecular( pd3dDevice, m_pmcMesh, NULL, m_pMaterials, bPieceRender );

	m_pSavedStateBlock->Apply();

	pd3dDevice->SetTexture ( 1, NULL );
}

HRESULT DxEffCharReflection2::LoadFile_0100 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	EFFCHAR_PROPERTY_REFLECTION2_100	pProp;

	//	Note : 이팩트의 Property 를 저장.
	//
	SFile.ReadBuffer ( &pProp, sizeof(EFFCHAR_PROPERTY_REFLECTION2_100) );

	m_dwColorOP = pProp.m_dwColorOP;
	m_dwFlag	= pProp.m_dwFlag;

	m_cSpecular	= D3DXCOLOR ( 0.8f, 0.8f, 0.8f, 1.0f );	// Ver.101	추가

	StringCchCopy( m_szTexture, MAX_PATH, pProp.m_szTexture );

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
		if ( !strcmp ( m_pTempMaterials[i].szEffTex, szChar ) )		// 문자가 같은경우 0이 나옴
		{
			StringCchCopy( m_pTempMaterials[i].szEffTex, MAX_PATH, m_pTempMaterials[i].szTexture );
		}
	}

	return S_OK;
}

HRESULT DxEffCharReflection2::LoadFile_0101 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	EFFCHAR_PROPERTY_REFLECTION2_101	pProp;

	//	Note : 이팩트의 Property 를 저장.
	//
	SFile.ReadBuffer ( &pProp, sizeof(EFFCHAR_PROPERTY_REFLECTION2_101) );

	m_dwColorOP = pProp.m_dwColorOP;
	m_dwFlag	= pProp.m_dwFlag;

	m_cSpecular	= pProp.m_cSpecular;

	StringCchCopy( m_szTexture, MAX_PATH, pProp.m_szTexture );

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
		if ( !strcmp ( m_pTempMaterials[i].szEffTex, szChar ) )		// 문자가 같은경우 0이 나옴
		{
			StringCchCopy( m_pTempMaterials[i].szEffTex, MAX_PATH, m_pTempMaterials[i].szTexture );
		}
	}

	return S_OK;
}

HRESULT DxEffCharReflection2::LoadFile_0102 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	
	//	Note : 이팩트의 Property 를 저장.
	//
	EFFCHAR_PROPERTY_REFLECTION2_102_104 Property;

	SFile.ReadBuffer ( &Property, sizeof(EFFCHAR_PROPERTY_REFLECTION2_102_104) );

	m_Property.Assign( Property );

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

	return S_OK;
}

HRESULT DxEffCharReflection2::LoadFile_0103 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 이팩트의 Property 를 저장.
	//
	EFFCHAR_PROPERTY_REFLECTION2_102_104 Property;

	SFile.ReadBuffer ( &Property, sizeof(EFFCHAR_PROPERTY_REFLECTION2_102_104) );

	m_Property.Assign( Property );

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

	return S_OK;
}

HRESULT DxEffCharReflection2::LoadFile_0104 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 이팩트의 Property 를 저장.
	//
	EFFCHAR_PROPERTY_REFLECTION2_102_104 Property;

	SFile.ReadBuffer ( &Property, sizeof(EFFCHAR_PROPERTY_REFLECTION2_102_104) );

	m_Property.Assign( Property );

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

	return S_OK;
}

HRESULT DxEffCharReflection2::LoadFile_0105 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 이팩트의 Property 를 저장.
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

	return S_OK;
}

HRESULT	DxEffCharReflection2::LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
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
	case 0x0104:	LoadFile_0104( SFile, pd3dDevice );	break;
	case VERSION:	LoadFile_0105( SFile, pd3dDevice );	break;
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

HRESULT	DxEffCharReflection2::SaveFile ( sc::BaseStream &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	//	Note : DATA의 사이즈를 기록한다. Load 시에 버전이 틀릴 경우 사용됨.
	//
	SFile << (DWORD) ( sizeof(m_Property) + sizeof(DWORD) + sizeof(DXMATERIAL_CHAR_EFF_OLD)*m_dwMaterials );

	//	Note : 이팩트의 Property 를 저장.
	//
	SFile.WriteBuffer ( &m_Property, sizeof(m_Property) );

	SFile << m_dwMaterials;
	for( DWORD i=0; i<m_dwMaterials; ++i )
        m_pMaterials[ i ].SaveFile( SFile );

	return S_OK;
}

//HRESULT	DxEffCharReflection2::UpdateReflection ( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwVertex, D3DXSKINEFFVERTEX* pVertices )
//{
//	D3DXVECTOR3			vPos, vNormal, vFromPt;
//	D3DXVECTOR3			vPosFromPt, vUVW;
//	D3DXMATRIX			matWorld, matRotate;
//	float				fAngle;
//
//	vFromPt = DxViewPort::GetInstance().GetFromPt ();
//	pd3dDevice->GetTransform ( D3DTS_WORLD, &matWorld );
//
//	for ( DWORD i=0; i<dwVertex; i++ )
//	{
//		D3DXVec3TransformCoord ( &vPos, &pVertices[i].vPos, &matWorld );
//		D3DXVec3TransformNormal ( &vNormal, &pVertices[i].vNormal, &matWorld );
//		D3DXVec3Normalize ( &vNormal, &vNormal );
//
//		vPosFromPt = vPos - vFromPt;
//		D3DXVec3Normalize ( &vPosFromPt, &vPosFromPt );
//
//		fAngle = D3DXVec3Dot ( &vPosFromPt, &vNormal );
//		fAngle *= 2.f;
//
//		vUVW = vNormal - ( vPosFromPt * fAngle );
//
//		pVertices[i].tu1 = vUVW.x;
//		pVertices[i].tv1 = vUVW.y;
//		pVertices[i].tw1 = vUVW.z;		
//	}
//
//	return S_OK;
//}

BOOL DxEffCharReflection2::IsExist_Copy_pmcMesh()
{
	// 값이 있다면 또 작업하지 않는다.
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

	if ( m_pTempMaterials )
	{
		m_dwMaterials = m_pmcMesh->GetNumMaterials();
		SAFE_DELETE_ARRAY ( m_pMaterials );
		m_pMaterials = new DXMATERIAL_CHAR_EFF[m_dwMaterials];

		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
			if( !m_pmcMesh->pMaterials[i].pTextureFilename )
				continue;

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
		m_dwMaterials = m_pmcMesh->GetNumMaterials();		// 매트리얼 정보가 바뀌었을 때 그냥 삭제후 디폴트 생성
		SAFE_DELETE_ARRAY ( m_pMaterials );
		m_pMaterials = new DXMATERIAL_CHAR_EFF[m_dwMaterials];

		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
			if( !m_pmcMesh->pMaterials[i].pTextureFilename )	continue;

			std::string	strName;
			strName = sc::util::GetSpecularName ( m_pmcMesh->pMaterials[i].pTextureFilename, "_s" );

			if( m_pmcMesh->pMaterials[i].pTextureFilename )
				StringCchCopy( m_pMaterials[i].szTexture,	MAX_PATH, m_pmcMesh->pMaterials[i].pTextureFilename );
			StringCchCopy( m_pMaterials[i].szEffTex,	MAX_PATH, strName.c_str() );

			m_pMaterials[i].LoadTexture( EMMM_CHAR_MIPMAP, true );
		}
	}
	else if ( !m_pMaterials )							// 매트리얼이 없을 때 디폴트 생성
	{
		m_dwMaterials = m_pmcMesh->GetNumMaterials();
		SAFE_DELETE_ARRAY ( m_pMaterials );
		m_pMaterials = new DXMATERIAL_CHAR_EFF[m_dwMaterials];

		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
			if( !m_pmcMesh->pMaterials[i].pTextureFilename )	continue;

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