//
//	DXMATERIAL_CHAR_EFF 이 변화하면서 0x105 버전으로 변화 하였다.
//
#include "pch.h"

#include "../../../SigmaCore/File/SerialFile.h"
#include "../../../SigmaCore/Util/Util.h"

#include "../../DxTools/DxCubeMap.h"
#include "../../DxTools/Light/DxLightMan.h"
#include "../../DxTools/DxViewPort.h"
#include "../../DxTools/RENDERPARAM.h"

#include "../../DxEffect/Frame/DxEffectMan.h"
#include "../../DxEffect/Single/DxEffSingle.h"
#include "../Single/DxEffSinglePropGMan.h"

#include "../../DxMeshs/SkinMesh/DxCharPart.h"
#include "../../DxMeshs/SkinMesh/DxSkinPieceContainer.h"

#include "./DxEffCharHLSL.h"
#include "../../DxMeshs/SkinMesh/_new/DxSkinMesh9_HLSL.h"

#include "./DxEffCharSpecular2.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

extern BOOL			g_bCHAR_EDIT_RUN;
extern int			g_nITEMLEVEL;

DWORD		DxEffCharSpecular2::TYPEID			= EMEFFCHAR_SPECULAR2;
const DWORD	DxEffCharSpecular2::VERSION			= 0x0106;
char		DxEffCharSpecular2::NAME[MAX_PATH]	= "1.SPECULAR";

LPDIRECT3DSTATEBLOCK9		DxEffCharSpecular2::m_pSavedStateBlock	= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffCharSpecular2::m_pEffectStateBlock	= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffCharSpecular2::m_pSavedCubeSB		= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffCharSpecular2::m_pDrawCubeSB		= NULL;

DxEffCharSpecular2::DxEffCharSpecular2(void) :
	DxEffChar(),
	m_pSkinMesh(NULL),
	m_pmcMesh(NULL),
	m_dwTempMaterials(0L),
	m_pMaterials(NULL),
	m_pTempMaterials(NULL),
	m_dwMipMapCount(0)
{
}

DxEffCharSpecular2::~DxEffCharSpecular2(void)
{
	m_dwMaterials = 0L;
	m_dwTempMaterials = 0L;

	SAFE_DELETE_ARRAY ( m_pMaterials );
	SAFE_DELETE_ARRAY ( m_pTempMaterials );
}

DxEffChar* DxEffCharSpecular2::CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece, DWORD dwMaterialNum )
{
	HRESULT hr;
	DxEffCharSpecular2 *pEffChar = new DxEffCharSpecular2;
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

void DxEffCharSpecular2::SetMaterials ( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwMaterials, DXMATERIAL_CHAR_EFF* pMaterials )
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

        m_pMaterials[i].LoadTexture( m_dwMipMapCount, true );
	}
}

HRESULT DxEffCharSpecular2::StaticCreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

HRESULT DxEffCharSpecular2::StaticResetDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	D3DCAPSQ d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );

	float fBias = -0.0002f;

	switch( RENDERPARAM::emCharRenderTYPE )
	{
	case EMCRT_SOFTWARE:
	case EMCRT_NORMAL:
		for( UINT which=0; which<2; which++ )
		{
			pd3dDevice->BeginStateBlock();

			//	Note : SetRenderState() 선언
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
			pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,	D3DTA_SPECULAR );
			pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG2 );

			pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE );
			pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2,	D3DTA_TFACTOR );
			pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );

			if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedStateBlock );
			else			pd3dDevice->EndStateBlock( &m_pEffectStateBlock );
		}
		break;
	case EMCRT_VERTEX:
	case EMCRT_PIXEL:
		for( UINT which=0; which<2; which++ )
		{
			pd3dDevice->BeginStateBlock();

			//	Note : SetRenderState() 선언
			pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,			*((DWORD*)&fBias) );
			pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
			pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );

			pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
			pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );

			if( d3dCaps.AlphaCmpCaps & D3DPCMPCAPS_GREATEREQUAL )
			{
				pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	TRUE );
				pd3dDevice->SetRenderState( D3DRS_ALPHAREF,			0x01 );
				pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC,		D3DCMP_GREATEREQUAL );
			}

			//	Note : SetTextureStageState() 선언
			pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );
			pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG2 );

			pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2,	D3DTA_TFACTOR );
			pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );

			if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedStateBlock );
			else			pd3dDevice->EndStateBlock( &m_pEffectStateBlock );
		}
		break;
	};

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		//	Note : SetRenderState() 선언
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,			0x00000000 );

		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_ONE );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_ONE );

		//	Note : SetTextureStageState() 선언
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1 );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,	D3DTOP_MODULATE );

		pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX,			D3DTSS_TCI_CAMERASPACENORMAL );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS,	D3DTTFF_COUNT3 );

		if( which==0 )	pd3dDevice->EndStateBlock ( &m_pSavedCubeSB );
		else			pd3dDevice->EndStateBlock ( &m_pDrawCubeSB );
	}
	return S_OK;
}

HRESULT DxEffCharSpecular2::StaticLostDevice()
{
	HRESULT hr(S_OK);
	SAFE_RELEASE( m_pSavedStateBlock );
	SAFE_RELEASE( m_pEffectStateBlock );
	SAFE_RELEASE( m_pSavedCubeSB );
	SAFE_RELEASE( m_pDrawCubeSB );

	return S_OK;
}

void DxEffCharSpecular2::StaticDestroyDevice()
{
}

HRESULT DxEffCharSpecular2::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
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

	// DXMATERIAL_CHAR_EFF 확인 및 생성작업.
	IsExist_Copy_pmcMesh();

	return S_OK;
}

HRESULT DxEffCharSpecular2::DeleteDeviceObjects ()
{
	SAFE_DELETE_ARRAY ( m_pMaterials );

	return S_OK;
}

HRESULT DxEffCharSpecular2::FrameMove ( float fTime, float fElapsedTime )
{
	HRESULT hr = S_OK;

	return S_OK;
}

void	DxEffCharSpecular2::Render( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* arrayBoneMatrices, BOOL bPieceRender )
{
	HRESULT hr = S_OK;

	// DXMATERIAL_CHAR_EFF 확인 및 생성작업.
	if ( !IsExist_Copy_pmcMesh() )
		return;

	if ( !m_pSkinMesh )		return;

	switch( RENDERPARAM::emCharRenderTYPE )
	{
	case EMCRT_SOFTWARE:
	case EMCRT_NORMAL:
		if( m_dwFlag&EFFCHARSPEC2_CUBE )	Render_Cube( pd3dDevice, arrayBoneMatrices, bPieceRender );
		else								Render_Vertex( pd3dDevice, arrayBoneMatrices, bPieceRender );
		break;

	case EMCRT_VERTEX:
	case EMCRT_PIXEL:
		if( m_dwFlag&EFFCHARSPEC2_CUBE )	Render_Cube_SPEC( pd3dDevice, bPieceRender );
		else								Render_Vertex_SPEC( pd3dDevice, bPieceRender );
		break;
	};
}

void DxEffCharSpecular2::Render_Cube( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* arrayBoneMatrices, const BOOL bPieceRender )
{
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

	// Note : Get Texture
	for ( DWORD i=0; i<m_dwMaterials; i++ )
	{
        m_pMaterials[i].UpdateTexture();
	}

	pd3dDevice->SetTexture( 1, DxCubeMap::GetInstance().GetCubeTexTEST() );
	
	m_pSavedCubeSB->Capture();
	m_pDrawCubeSB->Apply();

	CHARSETTING sCharSetting;
	sCharSetting.pMeshContainerBase = m_pmcMesh;
	sCharSetting.pmtrlPiece			= pMaterialPiece;
	sCharSetting.pmtrlSpecular		= m_pMaterials;
	sCharSetting.bWorldIdentity		= bPieceRender;
	sCharSetting.emRDOP				= CTOP_BASE;

	m_pSkinMesh->SetDrawState( FALSE, FALSE, FALSE, FALSE );
	m_pSkinMesh->DrawMeshContainer( pd3dDevice, sCharSetting, m_rSkeleton, *pmatStaticLinkBone, arrayBoneMatrices );

	m_pSavedCubeSB->Apply();

	pd3dDevice->SetTexture( 1, NULL );
}

void DxEffCharSpecular2::Render_Vertex( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* arrayBoneMatrices, const BOOL bPieceRender )
{
	int nLevel = 0;

	//	Note : CharEdit 에서만 쓰이는 것이다.
	if( g_bCHAR_EDIT_RUN )	nLevel = g_nITEMLEVEL;

	if ( nLevel )		return;		// 흐르기 시작하면 스펙큘러는 끈다.
	if ( DxEffectMan::GetInstance().GetSkinDetail() == SKD_NONE )		return;
	if ( !(DxEffectMan::GetInstance().GetDetailFlag()&REALSPECULAR) )	return; 

	//	빛위치
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

	pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	m_dwColorOP );		// 변경

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

	pd3dDevice->LightEnable ( 0, TRUE );

	pd3dDevice->LightEnable ( 4, bLightEnable4 );
	pd3dDevice->SetLight ( 4, &pSrcLight4 );

	pd3dDevice->LightEnable ( 5, bLightEnable5 );
	pd3dDevice->SetLight ( 5, &pSrcLight5 );
}

void DxEffCharSpecular2::Render_Cube_SPEC( LPDIRECT3DDEVICEQ pd3dDevice, const BOOL bPieceRender )
{
	// Note : Get Texture
	for ( DWORD i=0; i<m_dwMaterials; i++ )
	{
		m_pMaterials[i].UpdateTexture();
	}

	pd3dDevice->SetTexture( 1, DxCubeMap::GetInstance().GetCubeTexTEST() );
	
	m_pSavedCubeSB->Capture();
	m_pDrawCubeSB->Apply();

	m_pSkinMesh->SetDrawState( FALSE, FALSE, FALSE, FALSE );
	DxSkinMesh9_HLSL::m_pFX = NSCHARHLSL::m_pCubeFX;
	m_pSkinMesh->DrawMeshSpecular( pd3dDevice, m_pmcMesh, NULL, m_pMaterials, bPieceRender );

	m_pSavedCubeSB->Apply();

	pd3dDevice->SetTexture( 1, NULL );
}

void DxEffCharSpecular2::Render_Vertex_SPEC( LPDIRECT3DDEVICEQ pd3dDevice, const BOOL bPieceRender )
{
	int nLevel = 0;

	//	Note : CharEdit 에서만 쓰이는 것이다.
	if( g_bCHAR_EDIT_RUN )	nLevel = g_nITEMLEVEL;

	if ( nLevel )		return;		// 흐르기 시작하면 스펙큘러는 끈다.
	if ( DxEffectMan::GetInstance().GetSkinDetail() == SKD_NONE )		return;
	if ( !(DxEffectMan::GetInstance().GetDetailFlag()&REALSPECULAR) )	return; 

	m_pSavedStateBlock->Capture();
	m_pEffectStateBlock->Apply();

	pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	m_dwColorOP );		// 변경

	// Note : Get Texture
	for ( DWORD i=0; i<m_dwMaterials; i++ )
	{
		m_pMaterials[i].UpdateTexture();
	}

	m_pSkinMesh->SetDrawState ( FALSE, FALSE, FALSE, FALSE );
	DxSkinMesh9_HLSL::m_pFX = NSCHARHLSL::m_pSpecularFX;
	m_pSkinMesh->DrawMeshSpecular ( pd3dDevice, m_pmcMesh, NULL, m_pMaterials, bPieceRender );

	m_pSavedStateBlock->Apply();
}


HRESULT DxEffCharSpecular2::LoadFile_0104 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 이팩트의 Property 를 저장.
	//
	EFFCHAR_PROPERTY_SPECULAR2_104_105 Property;

	SFile.ReadBuffer ( &Property, sizeof(EFFCHAR_PROPERTY_SPECULAR2_104_105) );

	m_Property.Assign ( Property );

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
            m_pMaterials[i].LoadTexture( m_dwMipMapCount, true );
		}
	}	

	return S_OK;
}

HRESULT DxEffCharSpecular2::LoadFile_0105 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 이팩트의 Property 를 저장.
	//
	EFFCHAR_PROPERTY_SPECULAR2_104_105 Property;

	SFile.ReadBuffer ( &Property, sizeof(EFFCHAR_PROPERTY_SPECULAR2_104_105) );

	m_Property.Assign ( Property );

	SFile >> m_dwMaterials;
	if ( m_dwMaterials )
	{
		SAFE_DELETE_ARRAY ( m_pMaterials );
		m_pMaterials = new DXMATERIAL_CHAR_EFF[m_dwMaterials];

		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
            m_pMaterials[i].LoadFile( SFile );
			m_pMaterials[i].LoadTexture( m_dwMipMapCount, true );
		}
	}

	return S_OK;
}

HRESULT DxEffCharSpecular2::LoadFile_0106 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
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
			m_pMaterials[i].LoadTexture( m_dwMipMapCount, true );
		}
	}

	return S_OK;
}

HRESULT	DxEffCharSpecular2::LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	SFile >> dwSize;

	switch ( dwVer )
	{
	case 0x0104:	LoadFile_0104( SFile, pd3dDevice );	break;
	case 0x0105:	LoadFile_0105( SFile, pd3dDevice );	break;
	case VERSION:	LoadFile_0106( SFile, pd3dDevice );	break;
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

HRESULT	DxEffCharSpecular2::SaveFile ( sc::BaseStream &SFile )
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

BOOL DxEffCharSpecular2::IsExist_Copy_pmcMesh()
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

			m_pMaterials[i].LoadTexture( m_dwMipMapCount, true );
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

			m_pMaterials[i].LoadTexture( m_dwMipMapCount, true );
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

			m_pMaterials[i].LoadTexture( m_dwMipMapCount, true );
		}
	}
	else
	{
		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
			m_pMaterials[i].LoadTexture( m_dwMipMapCount, true );
		}
	}

	return TRUE;
}