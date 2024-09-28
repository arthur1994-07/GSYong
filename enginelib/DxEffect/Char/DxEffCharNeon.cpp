//
//	DXMATERIAL_CHAR_EFF 이 변화하면서 0x104 버전으로 변화 하였다.
//
#include "pch.h"
#include "../../DxEffect/Single/DxEffSingle.h"
#include "../Single/DxEffSinglePropGMan.h"
#include "../../../SigmaCore/File/SerialFile.h"

#include "../../DxTools/DxVertexFVF.h"
#include "../../DxTools/DxSurfaceTex.h"
#include "../../DxTools/DxShadowMap.h"
#include "../../DxTools/DxGlowMan.h"

#include "../../DxTools/Light/DxLightMan.h"
#include "../../DxTools/DxViewPort.h"
#include "../../DxEffect/Frame/DxEffectMan.h"
#include "../../DxEffect/DxPostEffect.h"

#include "../../DxMeshs/SkinMesh/DxCharPart.h"
#include "../../DxMeshs/SkinMesh/DxSkinPieceContainer.h"
#include "../../DxMeshs/SkinMesh/_new/DxSkinMesh9_Material.h"

#include "./DxEffCharNeon.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

DWORD		DxEffCharNeon::TYPEID			= EMEFFCHAR_NEON;
const DWORD	DxEffCharNeon::VERSION			= 0x0106;
char		DxEffCharNeon::NAME[MAX_PATH]	= "3.Glow";

LPDIRECT3DSTATEBLOCK9		DxEffCharNeon::m_pSavedStateBlock	= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffCharNeon::m_pEffectStateBlock	= NULL;

DxEffCharNeon::DxEffCharNeon(void) :
	DxEffChar(),
	m_pSkinMesh(NULL),
	m_pmcMesh(NULL),
	m_pTempTex(NULL),
	m_dwTempMaterials(0L),
	m_pMaterials(NULL),
	m_pTempMaterials(NULL),
	m_pMaterials_EffUse(NULL)
{
}

DxEffCharNeon::~DxEffCharNeon(void)
{
	m_dwMaterials = 0L;
	m_dwTempMaterials = 0L;

	SAFE_DELETE_ARRAY ( m_pMaterials );
	SAFE_DELETE_ARRAY ( m_pTempMaterials );
	SAFE_DELETE_ARRAY ( m_pMaterials_EffUse );
}

DxEffChar* DxEffCharNeon::CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece, DWORD dwMaterialNum )
{
	HRESULT hr;
	DxEffCharNeon *pEffChar = new DxEffCharNeon;
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

void DxEffCharNeon::SetMaterials ( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwMaterials, DXMATERIAL_CHAR_EFF* pMaterials )
{
	SAFE_DELETE_ARRAY ( m_pMaterials );
	m_pMaterials = new DXMATERIAL_CHAR_EFF[dwMaterials];
	m_dwMaterials = dwMaterials;

	for ( DWORD i=0; i<dwMaterials; i++ )
	{
		m_pMaterials[i].SetType( pMaterials[i].GetType() );
		m_pMaterials[i].d3dMaterial = pMaterials[i].d3dMaterial;
		StringCchCopy( m_pMaterials[i].szTexture, MAX_PATH, pMaterials[i].szTexture );
	}
}

HRESULT DxEffCharNeon::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{	
	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_ONE );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_ONE );

		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,			FALSE );
		
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,		D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,		D3DTA_TFACTOR );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		D3DTOP_MODULATE );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedStateBlock );
		else			pd3dDevice->EndStateBlock( &m_pEffectStateBlock );
	}

	return S_OK;
}

HRESULT DxEffCharNeon::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSavedStateBlock );
	SAFE_RELEASE( m_pEffectStateBlock );

	return S_OK;
}

HRESULT DxEffCharNeon::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
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

	m_szBlackTex = "Black.dds";

    m_textureResGlow = TextureManager::GetInstance().LoadTexture(
        m_szTexture,
        false,
        TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
        0,
        D3DFMT_UNKNOWN,
        TEXTURE_RESIZE_NONE,
        TEXTURE_CHAR,
		true,
		FALSE );

    m_textureResBlack = TextureManager::GetInstance().LoadTexture(
        m_szBlackTex,
        false,
        TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
        0,
        D3DFMT_UNKNOWN,
        TEXTURE_RESIZE_NONE,
        TEXTURE_CHAR,
		true,
		FALSE );

	// DXMATERIAL_CHAR_EFF 확인 및 생성작업.
	IsExist_Copy_pmcMesh();

	return S_OK;
}

HRESULT DxEffCharNeon::DeleteDeviceObjects ()
{
    m_textureResGlow.Clear();
    m_textureResBlack.Clear();

	SAFE_DELETE_ARRAY ( m_pMaterials );
	SAFE_DELETE_ARRAY ( m_pMaterials_EffUse );	

	return S_OK;
}

HRESULT DxEffCharNeon::FrameMove ( float fTime, float fElapsedTime )
{
	HRESULT hr = S_OK;

	return S_OK;
}

void	DxEffCharNeon::Render( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* arrayBoneMatrices, BOOL bPieceRender )
{
	HRESULT hr = S_OK;

	// 성능을 최저사양으로 함.
	if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_PS_1_1 )
		return;

	if ( !m_pSkinMesh )									
		return;

	// DXMATERIAL_CHAR_EFF 확인 및 생성작업.
	if ( !IsExist_Copy_pmcMesh() )
		return;

	if ( !m_pmcMesh->MeshData.pMesh )					
		return;

	if ( !DxGlowMan::GetInstance().IsOptionEnable() )	
		return;	// 옵션에서 꺼져 있으면 동작하지 않는다.

	LPDIRECT3DSURFACEQ	pSrcSurface, pSrcZBuffer;
	pd3dDevice->GetRenderTarget( 0, &pSrcSurface );
	pd3dDevice->GetDepthStencilSurface ( &pSrcZBuffer );

	// 실제 렌더링 화면만 적용되도록 한다.
	// 미니맵, UI, 반사 등에서는 나오지 않도록 함.
	if ( pSrcSurface == DxSurfaceTex::GetInstance().m_pColorBuffer || 
		pSrcSurface == DxSurfaceTex::GetInstance().m_pColorSuf16F_HDR ||
		pSrcSurface == DxPostEffect::GetInstance().GetFinalSurface() )
	{
		// 문제 없음
	}
	else
	{
		SAFE_RELEASE ( pSrcSurface );
		SAFE_RELEASE ( pSrcZBuffer );
		return;
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

	if( (m_dwFlag&USE_GENERAL) && DxSurfaceTex::GetInstance().m_pGlowSuf_SRC )
	{
		pd3dDevice->SetRenderTarget ( 0, DxSurfaceTex::GetInstance().m_pGlowSuf_SRC );
		pd3dDevice->SetDepthStencilSurface ( pSrcZBuffer );

		BOOL bZBiasChange(FALSE);
		m_pmcMesh->SetMaterial( pd3dDevice, 0, bZBiasChange, pMaterialPiece );
		DxGlowMan::GetInstance().SetGlowON ();
	}
	else if( (m_dwFlag&USE_BURN) && DxSurfaceTex::GetInstance().m_pBurnNewSuf )
	{
		pd3dDevice->SetRenderTarget( 0, DxSurfaceTex::GetInstance().m_pBurnNewSuf );
		pd3dDevice->SetDepthStencilSurface ( pSrcZBuffer );

		BOOL bZBiasChange(FALSE);
		m_pmcMesh->SetMaterial( pd3dDevice, 0, bZBiasChange, pMaterialPiece );
		DxGlowMan::GetInstance().SetGlowON_Burn ();
	}

	m_pTempTex = NULL;
	if ( m_dwFlag & USE_GLOW_USER )
        m_pTempTex = m_textureResGlow.GetTexture();

	if ( m_dwFlag & USE_ALL_TEX )
	{
		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
			m_pMaterials[i].SetTexture( m_pTempTex );

			m_pMaterials_EffUse[i] = m_pMaterials[i].GetType();	// Use 백업
			m_pMaterials[i].SetType( EMCET_BASE );						// 설정
		}
	}
	else //USE_SELECT_TEX
	{
		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
			if ( m_pMaterials[i].IsType( EMCET_BASE ) )
                m_pMaterials[i].SetTexture( m_pTempTex );
			else
                m_pMaterials[i].SetTexture( m_textureResBlack.GetTexture() );

			m_pMaterials_EffUse[i] = m_pMaterials[i].GetType();	// Use 백업
			m_pMaterials[i].SetType( EMCET_BASE );						// 설정
		}
	}

	//	Draw 
	m_pSavedStateBlock->Capture();
	m_pEffectStateBlock->Apply();

	CHARSETTING sCharSetting;
	sCharSetting.pMeshContainerBase = m_pmcMesh;
	sCharSetting.pmtrlPiece			= pMaterialPiece;
	sCharSetting.pmtrlSpecular		= m_pMaterials;
	sCharSetting.bWorldIdentity		= bPieceRender;
	sCharSetting.emRDOP				= CTOP_BASE;
	
	m_pSkinMesh->SetDrawState( FALSE, FALSE, FALSE, FALSE );
	m_pSkinMesh->DrawMeshContainer( pd3dDevice, sCharSetting, m_rSkeleton, *pmatStaticLinkBone, arrayBoneMatrices );

	m_pSavedStateBlock->Apply();

	for ( DWORD i=0; i<m_dwMaterials; i++ )
	{
		m_pMaterials[i].SetTexture( NULL );
		m_pMaterials[i].SetType( m_pMaterials_EffUse[i] );	// Use 백업
	}

	pd3dDevice->SetRenderTarget( 0, pSrcSurface );
	pd3dDevice->SetDepthStencilSurface ( pSrcZBuffer );

	SAFE_RELEASE ( pSrcSurface );
	SAFE_RELEASE ( pSrcZBuffer );
}

void DxEffCharNeon::RenderMaterialGlow ( LPDIRECT3DDEVICEQ pd3dDevice, 
										SMATERIAL_PIECE* pMaterialSystem, 
										const D3DXMATRIX& matStaticLinkBone,
										DxSkinMesh9_Material& sSkinMesh,
										const DxPhysXCloth* pPhysXCloth )
{
	HRESULT hr = S_OK;

	// 성능을 최저사양으로 함.
	if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_PS_1_1 )
		return;

	// DXMATERIAL_CHAR_EFF 확인 및 생성작업.
	if ( !IsExist_Copy_pmcMesh() )
		return;

	if ( !m_pmcMesh->MeshData.pMesh )					
		return;

	if ( !DxGlowMan::GetInstance().IsOptionEnable() )	
		return;	// 옵션에서 꺼져 있으면 동작하지 않는다.

	DxSkinMesh9_Material* pSkinMesh = &sSkinMesh;

	LPDIRECT3DSURFACEQ	pSrcSurface, pSrcZBuffer;
	pd3dDevice->GetRenderTarget( 0, &pSrcSurface );
	pd3dDevice->GetDepthStencilSurface ( &pSrcZBuffer );

	// 실제 렌더링 화면만 적용되도록 한다.
	// 미니맵, UI, 반사 등에서는 나오지 않도록 함.
	if ( pSrcSurface == DxSurfaceTex::GetInstance().m_pColorBuffer || 
		pSrcSurface == DxSurfaceTex::GetInstance().m_pColorSuf16F_HDR ||
		pSrcSurface == DxPostEffect::GetInstance().GetFinalSurface() )
	{
		// 문제 없음
	}
	else
	{
		SAFE_RELEASE ( pSrcSurface );
		SAFE_RELEASE ( pSrcZBuffer );
		return;
	}

	const D3DXMATRIX*	pBoneMatrices(NULL);
	SMATERIAL_PIECE*		pMaterialPiece = NULL;
	if ( m_pCharPart )
	{
		pBoneMatrices = m_pCharPart->GetBoneMatrices();
		if ( !pBoneMatrices )
			return;

		pMaterialPiece = m_pCharPart->GetSkinPiece()->m_arrayMaterialPiece;
	}
	else if ( m_pSkinPiece )
	{
		pBoneMatrices = DxSkinMesh9::g_pBoneMatrices;
		pMaterialPiece = m_pSkinPiece->m_arrayMaterialPiece;
	}

	if( (m_dwFlag&USE_GENERAL) && DxSurfaceTex::GetInstance().m_pGlowSuf_SRC )
	{
		pd3dDevice->SetRenderTarget ( 0, DxSurfaceTex::GetInstance().m_pGlowSuf_SRC );
		pd3dDevice->SetDepthStencilSurface ( pSrcZBuffer );

		BOOL bZBiasChange(FALSE);
		m_pmcMesh->SetMaterial( pd3dDevice, 0, bZBiasChange, pMaterialPiece );
		DxGlowMan::GetInstance().SetGlowON ();
	}
	else if( (m_dwFlag&USE_BURN) && DxSurfaceTex::GetInstance().m_pBurnNewSuf )
	{
		pd3dDevice->SetRenderTarget( 0, DxSurfaceTex::GetInstance().m_pBurnNewSuf );
		pd3dDevice->SetDepthStencilSurface ( pSrcZBuffer );

		BOOL bZBiasChange(FALSE);
		m_pmcMesh->SetMaterial( pd3dDevice, 0, bZBiasChange, pMaterialPiece );
		DxGlowMan::GetInstance().SetGlowON_Burn ();
	}

	m_pTempTex = NULL;
	if ( m_dwFlag & USE_GLOW_USER )
		m_pTempTex = m_textureResGlow.GetTexture();

	if ( m_dwFlag & USE_ALL_TEX )
	{
		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
			if ( m_pTempTex )
			{
				m_pMaterials[i].SetTexture( m_pTempTex );
			}
			else
			{
				m_pMaterials[i].SetTexture( pMaterialPiece[i].m_textureRes.GetTexture() );
			}

			m_pMaterials_EffUse[i] = m_pMaterials[i].GetType();	// Use 백업
			m_pMaterials[i].SetType( EMCET_BASE );						// 설정
		}
	}
	else //USE_SELECT_TEX
	{
		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
			if ( m_pMaterials[i].IsType( EMCET_BASE ) )
			{
				if ( m_pTempTex )
				{
					m_pMaterials[i].SetTexture( m_pTempTex );
				}
				else
				{
					m_pMaterials[i].SetTexture( pMaterialPiece[i].m_textureRes.GetTexture() );
				}
			}

			m_pMaterials_EffUse[i] = m_pMaterials[i].GetType();	// Use 백업
		}
	}

	//	Draw 
	m_pSavedStateBlock->Capture();
	m_pEffectStateBlock->Apply();

	pSkinMesh->RenderMaterialGlow( pd3dDevice, m_pmcMesh, pBoneMatrices, pMaterialSystem, m_pMaterials, 0, matStaticLinkBone, pPhysXCloth );

	m_pSavedStateBlock->Apply();

	for ( DWORD i=0; i<m_dwMaterials; i++ )
	{
		m_pMaterials[i].SetTexture( NULL );
		m_pMaterials[i].SetType( m_pMaterials_EffUse[i] );	// Use 백업
	}

	pd3dDevice->SetRenderTarget( 0, pSrcSurface );
	pd3dDevice->SetDepthStencilSurface ( pSrcZBuffer );

	SAFE_RELEASE ( pSrcSurface );
	SAFE_RELEASE ( pSrcZBuffer );
}

HRESULT DxEffCharNeon::LoadFile_0100 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	EFFCHAR_PROPERTY_NEON_100	sProp;

	//	Note : 이팩트의 Property 를 저장.
	//
	SFile.ReadBuffer ( &sProp, sizeof(EFFCHAR_PROPERTY_NEON_100) );

	m_dwFlag	= sProp.m_dwFlag;
	m_dwFlag	|= USE_SELECT_TEX;	// 새롭게 추가된 사항
	m_dwFlag	|= USE_GLOW_USER;	// 새롭게 추가된 사항
	m_dwFlag	|= USE_BURN;		// 새롭게 추가된 사항
	m_cColor	= 0xffffffff;

	StringCchCopy( m_szTexture, MAX_PATH, sProp.m_szGlowTex_1 );

	if ( m_dwFlag&USETEXTURE1 )			m_pTempMaterials[0].SetType( EMCET_BASE );
	if ( m_dwFlag&USETEXTURE2 )			m_pTempMaterials[1].SetType( EMCET_BASE );
	if ( m_dwFlag&USETEXTURE3 )			m_pTempMaterials[2].SetType( EMCET_BASE );

	StringCchCopy( m_pTempMaterials[0].szTexture, MAX_PATH, sProp.m_szSrcTex_1 );
	StringCchCopy( m_pTempMaterials[1].szTexture, MAX_PATH, sProp.m_szSrcTex_2 );
	StringCchCopy( m_pTempMaterials[2].szTexture, MAX_PATH, sProp.m_szSrcTex_3 );

	// Ver.105
	m_dwFlag &= ~USE_GENERAL;
	m_dwFlag |= USE_BURN;

	return S_OK;
}

HRESULT DxEffCharNeon::LoadFile_0101 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	EFFCHAR_PROPERTY_NEON_101	sProp;

	//	Note : 이팩트의 Property 를 저장.
	//
	SFile.ReadBuffer ( &sProp, sizeof(EFFCHAR_PROPERTY_NEON_101) );

	m_dwFlag	= sProp.m_dwFlag;
	m_dwFlag	|= USE_SELECT_TEX;	// 새롭게 추가된 사항
	m_dwFlag	|= USE_GLOW_USER;	// 새롭게 추가된 사항
	m_dwFlag	|= USE_BURN;		// 새롭게 추가된 사항
	m_cColor	= sProp.m_cColor_1;

	StringCchCopy( m_szTexture, MAX_PATH, sProp.m_szGlowTex_1 );

	SAFE_DELETE_ARRAY ( m_pTempMaterials );
	m_pTempMaterials = new DXMATERIAL_CHAR_EFF[3];
	m_dwTempMaterials = 3;

	if ( m_dwFlag&USETEXTURE1 )			m_pTempMaterials[0].SetType( EMCET_BASE );
	if ( m_dwFlag&USETEXTURE2 )			m_pTempMaterials[1].SetType( EMCET_BASE );
	if ( m_dwFlag&USETEXTURE3 )			m_pTempMaterials[2].SetType( EMCET_BASE );

	StringCchCopy( m_pTempMaterials[0].szTexture, MAX_PATH, sProp.m_szSrcTex_1 );
	StringCchCopy( m_pTempMaterials[1].szTexture, MAX_PATH, sProp.m_szSrcTex_2 );
	StringCchCopy( m_pTempMaterials[2].szTexture, MAX_PATH, sProp.m_szSrcTex_3 );

	// Ver.105
	m_dwFlag &= ~USE_GENERAL;
	m_dwFlag |= USE_BURN;

	return S_OK;
}

HRESULT DxEffCharNeon::LoadFile_0102 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 이팩트의 Property 를 저장.
	//

	EFFCHAR_PROPERTY_NEON_102_105 Property;

	SFile.ReadBuffer ( &Property, sizeof(EFFCHAR_PROPERTY_NEON_102_105) );

	m_Property.Assign( Property );

	m_dwFlag	|= USE_SELECT_TEX;	// 새롭게 추가된 사항
	m_dwFlag	|= USE_GLOW_USER;	// 새롭게 추가된 사항
	m_dwFlag	|= USE_BURN;		// 새롭게 추가된 사항

	SFile >> m_dwMaterials;
	if ( m_dwMaterials )
	{
		DXMATERIAL_SPECULAR*	pMaterials;
		pMaterials = new DXMATERIAL_SPECULAR[m_dwMaterials];
		SFile.ReadBuffer ( pMaterials, sizeof(DXMATERIAL_SPECULAR)*m_dwMaterials );

		SAFE_DELETE_ARRAY ( m_pMaterials );
		m_pMaterials = new DXMATERIAL_CHAR_EFF[m_dwMaterials];

		DWORD dwR = (m_cColor&0xff0000)>>16;
		DWORD dwG = (m_cColor&0xff00)>>8;
		DWORD dwB = (m_cColor&0xff);

		for ( DWORD i=0; i<m_dwMaterials; ++i )
		{
			m_pMaterials[i].SetType( pMaterials[i] );
			m_pMaterials[i].d3dMaterial.Diffuse.r = (float)dwR*DIV_1_255;
			m_pMaterials[i].d3dMaterial.Diffuse.g = (float)dwG*DIV_1_255;
			m_pMaterials[i].d3dMaterial.Diffuse.b = (float)dwB*DIV_1_255;
			m_pMaterials[i].d3dMaterial.Power	= 14.f;
			StringCchCopy( m_pMaterials[i].szEffTex,	MAX_PATH, pMaterials[i].szSpecTex );
			StringCchCopy( m_pMaterials[i].szTexture,	MAX_PATH, pMaterials[i].szTexture );
		}

		SAFE_DELETE_ARRAY ( pMaterials );
	}

	// Ver.105
	m_dwFlag &= ~USE_GENERAL;
	m_dwFlag |= USE_BURN;

	return S_OK;
}

HRESULT DxEffCharNeon::LoadFile_0103 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 이팩트의 Property 를 저장.
	//
	EFFCHAR_PROPERTY_NEON_102_105 Property;

	SFile.ReadBuffer ( &Property, sizeof(EFFCHAR_PROPERTY_NEON_102_105) );

	m_Property.Assign( Property );

	if ( !( m_dwFlag&(USE_ALL_TEX|USE_SELECT_TEX) ) )		m_dwFlag |= USE_SELECT_TEX;	// 새롭게 추가된 사항
	if ( !( m_dwFlag&(USE_GLOW_SRCTEX|USE_GLOW_USER) ) )	m_dwFlag |= USE_GLOW_USER;	// 새롭게 추가된 사항
	if ( !( m_dwFlag&(USE_GENERAL|USE_BURN) ) )				m_dwFlag |= USE_BURN;		// 새롭게 추가된 사항

	SFile >> m_dwMaterials;
	if ( m_dwMaterials )
	{
		DXMATERIAL_CHAR_EFF_100*	pMaterials;
		pMaterials = new DXMATERIAL_CHAR_EFF_100[m_dwMaterials];
		SFile.ReadBuffer ( pMaterials, sizeof(DXMATERIAL_CHAR_EFF_100)*m_dwMaterials );

		SAFE_DELETE_ARRAY ( m_pMaterials );
		m_pMaterials = new DXMATERIAL_CHAR_EFF[m_dwMaterials];

		DWORD dwR = (m_cColor&0xff0000)>>16;
		DWORD dwG = (m_cColor&0xff00)>>8;
		DWORD dwB = (m_cColor&0xff);

		for ( DWORD i=0; i<m_dwMaterials; ++i )
		{
			m_pMaterials[i].SetType( pMaterials[i] );
			m_pMaterials[i].d3dMaterial.Diffuse.r = (float)dwR*DIV_1_255;
			m_pMaterials[i].d3dMaterial.Diffuse.g = (float)dwG*DIV_1_255;
			m_pMaterials[i].d3dMaterial.Diffuse.b = (float)dwB*DIV_1_255;
			m_pMaterials[i].d3dMaterial.Power	= pMaterials[i].fMaterial_Power;
			StringCchCopy( m_pMaterials[i].szEffTex,	MAX_PATH, pMaterials[i].szEffTex );
			StringCchCopy( m_pMaterials[i].szTexture,	MAX_PATH, pMaterials[i].szTexture );
		}

		SAFE_DELETE_ARRAY ( pMaterials );
	}

	// Ver.105
	m_dwFlag &= ~USE_GENERAL;
	m_dwFlag |= USE_BURN;

	return S_OK;
}

HRESULT DxEffCharNeon::LoadFile_0104 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 이팩트의 Property 를 저장.
	//
	EFFCHAR_PROPERTY_NEON_102_105 Property;

	SFile.ReadBuffer ( &Property, sizeof(EFFCHAR_PROPERTY_NEON_102_105) );

	m_Property.Assign( Property );

	SFile >> m_dwMaterials;
	if ( m_dwMaterials )
	{
		SAFE_DELETE_ARRAY ( m_pMaterials );
		m_pMaterials = new DXMATERIAL_CHAR_EFF[m_dwMaterials];
        for( DWORD i=0; i<m_dwMaterials; ++i )
            m_pMaterials[ i ].LoadFile( SFile );
	}

	// Ver.105
	m_dwFlag &= ~USE_GENERAL;
	m_dwFlag |= USE_BURN;

	return S_OK;
}

HRESULT DxEffCharNeon::LoadFile_0105 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 이팩트의 Property 를 저장.
	//
	EFFCHAR_PROPERTY_NEON_102_105 Property;

	SFile.ReadBuffer ( &Property, sizeof(EFFCHAR_PROPERTY_NEON_102_105) );

	m_Property.Assign( Property );

	SFile >> m_dwMaterials;
	if ( m_dwMaterials )
	{
		SAFE_DELETE_ARRAY ( m_pMaterials );
		m_pMaterials = new DXMATERIAL_CHAR_EFF[m_dwMaterials];
		for( DWORD i=0; i<m_dwMaterials; ++i )
            m_pMaterials[ i ].LoadFile( SFile );
	}

	return S_OK;
}

HRESULT DxEffCharNeon::LoadFile_0106 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 이팩트의 Property 를 저장.
	//
	SFile.ReadBuffer ( &m_Property, sizeof(m_Property) );

	SFile >> m_dwMaterials;
	if ( m_dwMaterials )
	{
		SAFE_DELETE_ARRAY ( m_pMaterials );
		m_pMaterials = new DXMATERIAL_CHAR_EFF[m_dwMaterials];
		for( DWORD i=0; i<m_dwMaterials; ++i )
            m_pMaterials[ i ].LoadFile( SFile );
	}

	return S_OK;
}

HRESULT	DxEffCharNeon::LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
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

HRESULT	DxEffCharNeon::SaveFile ( sc::BaseStream &SFile )
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

BOOL DxEffCharNeon::IsExist_Copy_pmcMesh()
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

		DWORD dwR = (m_cColor&0xff0000)>>16;
		DWORD dwG = (m_cColor&0xff00)>>8;
		DWORD dwB = (m_cColor&0xff);

		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
			m_pMaterials[i].d3dMaterial.Diffuse.r = (float)dwR*DIV_1_255;
			m_pMaterials[i].d3dMaterial.Diffuse.g = (float)dwG*DIV_1_255;
			m_pMaterials[i].d3dMaterial.Diffuse.b = (float)dwB*DIV_1_255;

			m_pMaterials[i].SetType( m_pTempMaterials[i].GetType() );

			if( m_pmcMesh->pMaterials[i].pTextureFilename )
				StringCchCopy( m_pMaterials[i].szTexture, MAX_PATH, m_pmcMesh->pMaterials[i].pTextureFilename );
		}

		m_dwTempMaterials = 0L;
		SAFE_DELETE_ARRAY ( m_pTempMaterials );
	}
	else if ( m_pMaterials && (m_dwMaterials != m_pmcMesh->GetNumMaterials()) )
	{
		m_dwMaterials = m_pmcMesh->GetNumMaterials();		// 매트리얼 정보가 바뀌었을 때 그냥 삭제후 디폴트 생성
		SAFE_DELETE_ARRAY ( m_pMaterials );
		m_pMaterials = new DXMATERIAL_CHAR_EFF[m_dwMaterials];

		DWORD dwR = (m_cColor&0xff0000)>>16;
		DWORD dwG = (m_cColor&0xff00)>>8;
		DWORD dwB = (m_cColor&0xff);

		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
			m_pMaterials[i].d3dMaterial.Diffuse.r = (float)dwR*DIV_1_255;
			m_pMaterials[i].d3dMaterial.Diffuse.g = (float)dwG*DIV_1_255;
			m_pMaterials[i].d3dMaterial.Diffuse.b = (float)dwB*DIV_1_255;

			if( m_pmcMesh->pMaterials[i].pTextureFilename )
				StringCchCopy( m_pMaterials[i].szTexture, MAX_PATH, m_pmcMesh->pMaterials[i].pTextureFilename );
		}
	}
	else if ( !m_pMaterials )							// 매트리얼이 없을 때 디폴트 생성
	{
		m_dwMaterials = m_pmcMesh->GetNumMaterials();
		SAFE_DELETE_ARRAY ( m_pMaterials );
		m_pMaterials = new DXMATERIAL_CHAR_EFF[m_dwMaterials];

		DWORD dwR = (m_cColor&0xff0000)>>16;
		DWORD dwG = (m_cColor&0xff00)>>8;
		DWORD dwB = (m_cColor&0xff);

		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
			m_pMaterials[i].d3dMaterial.Diffuse.r = (float)dwR*DIV_1_255;
			m_pMaterials[i].d3dMaterial.Diffuse.g = (float)dwG*DIV_1_255;
			m_pMaterials[i].d3dMaterial.Diffuse.b = (float)dwB*DIV_1_255;

			if( m_pmcMesh->pMaterials[i].pTextureFilename )
				StringCchCopy( m_pMaterials[i].szTexture, MAX_PATH, m_pmcMesh->pMaterials[i].pTextureFilename );
		}
	}

	SAFE_DELETE_ARRAY(m_pMaterials_EffUse);	
	m_pMaterials_EffUse = new EMCHAR_EFF_TYPE [ m_pmcMesh->GetNumMaterials() ];

	return TRUE;
}