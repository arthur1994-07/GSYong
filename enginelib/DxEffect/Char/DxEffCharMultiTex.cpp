//
//	DXMATERIAL_CHAR_EFF 이 변화하면서 0x105 버전으로 변화 하였다.
//
#include "pch.h"
#include "../../DxEffect/Single/DxEffSingle.h"
#include "../Single/DxEffSinglePropGMan.h"
#include "../../../SigmaCore/File/SerialFile.h"

#include "../../DxTools/DxSurfaceTex.h"
#include "../../DxTools/DxShadowMap.h"
#include "../../DxTools/DxViewPort.h"
#include "../../DxEffect/Frame/DxEffectMan.h"

#include "../../DxMeshs/SkinMesh/DxCharPart.h"
#include "../../DxMeshs/SkinMesh/_new/DxSkinMesh9_Material.h"

#include "./DxEffCharMultiTex.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

DWORD		DxEffCharMultiTex::TYPEID			= EMEFFCHAR_MULTITEX;
const DWORD	DxEffCharMultiTex::VERSION			= 0x0106;
char		DxEffCharMultiTex::NAME[MAX_PATH]	= "2.멀티텍스쳐 Multi";

LPDIRECT3DSTATEBLOCK9		DxEffCharMultiTex::m_pSavedStateBlock	= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffCharMultiTex::m_pEffectStateBlock	= NULL;

DxEffCharMultiTex::DxEffCharMultiTex(void) :
	DxEffChar(),
	m_pSkinMesh(NULL),
	m_pmcMesh(NULL),
	m_vSumTex01(0.f,0.f),
	m_vSumTex02(0.f,0.f),
	m_dwTempMaterials(0L),
	m_pMaterials(NULL),
	m_pTempMaterials(NULL),
	m_pMaterials_EffUse(NULL)
{
}

DxEffCharMultiTex::~DxEffCharMultiTex(void)
{
	m_dwMaterials = 0L;
	m_dwTempMaterials = 0L;

	SAFE_DELETE_ARRAY ( m_pMaterials );
	SAFE_DELETE_ARRAY ( m_pTempMaterials );
	SAFE_DELETE_ARRAY ( m_pMaterials_EffUse );
}

DxEffChar* DxEffCharMultiTex::CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece, DWORD dwMaterialNum )
{
	HRESULT hr;
	DxEffCharMultiTex *pEffChar = new DxEffCharMultiTex;
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

void DxEffCharMultiTex::SetMaterials ( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwMaterials, DXMATERIAL_CHAR_EFF* pMaterials )
{
	SAFE_DELETE_ARRAY ( m_pMaterials );
	m_pMaterials = new DXMATERIAL_CHAR_EFF[dwMaterials];
	m_dwMaterials = dwMaterials;

	for ( DWORD i=0; i<dwMaterials; i++ )
	{
		m_pMaterials[i].SetType( pMaterials[i].GetType() );
		StringCchCopy( m_pMaterials[i].szTexture, MAX_PATH, pMaterials[i].szTexture );
	}
}

HRESULT DxEffCharMultiTex::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{	
	D3DCAPSQ d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		//	Note : SetRenderState() 선언
		float fBias = -0.0002f;
		pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,			*((DWORD*)&fBias) );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,			FALSE );

		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCCOLOR );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );

		//	Note : SetTextureStageState() 선언
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1 );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedStateBlock );
		else			pd3dDevice->EndStateBlock( &m_pEffectStateBlock );
	}

	return S_OK;
}

HRESULT DxEffCharMultiTex::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSavedStateBlock );
	SAFE_RELEASE( m_pEffectStateBlock );

	return S_OK;
}

HRESULT DxEffCharMultiTex::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
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

    m_textureRes1 = TextureManager::GetInstance().LoadTexture(
        m_szTex1,
        false,
        TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
        EMMM_CHAR_MIPMAP,
        D3DFMT_UNKNOWN,
        TEXTURE_RESIZE_NONE,
        TEXTURE_CHAR,
		true,
		FALSE );

    m_textureRes2 = TextureManager::GetInstance().LoadTexture(
        m_szTex2,
        false,
        TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
        EMMM_CHAR_MIPMAP,
        D3DFMT_UNKNOWN,
        TEXTURE_RESIZE_NONE,
        TEXTURE_CHAR,
		true,
		FALSE );

	// DXMATERIAL_CHAR_EFF 생성작업.
	IsExist_Copy_pmcMesh();

	return S_OK;
}

HRESULT DxEffCharMultiTex::DeleteDeviceObjects ()
{
    m_textureRes.Clear();
    m_textureRes1.Clear();
    m_textureRes2.Clear();

	SAFE_DELETE_ARRAY ( m_pMaterials );
	SAFE_DELETE_ARRAY ( m_pMaterials_EffUse );	

	return S_OK;
}

HRESULT DxEffCharMultiTex::FrameMove ( float fTime, float fElapsedTime )
{
	HRESULT hr = S_OK;

	m_vSumTex01 += m_vTex01*fElapsedTime;
	m_vSumTex02 += m_vTex02*fElapsedTime;

	return S_OK;
}

void	DxEffCharMultiTex::Render( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* arrayBoneMatrices, BOOL bPieceRender )
{
	HRESULT hr = S_OK;

	if ( !m_pSkinMesh )	
		return;

	// DXMATERIAL_CHAR_EFF 확인 및 생성작업.
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

	// BaseRenderOff 가 1개라도 존재하는가~?
	BOOL bEnableBaseRenderOff(FALSE);
	for ( DWORD i=0; i<m_dwMaterials; i++ )
	{
		if ( pMaterialPiece[i].m_bBaseRenderOff )
			bEnableBaseRenderOff = TRUE;
	}

	if ( bEnableBaseRenderOff )
	{
		// 여기 들어온다는건 원본 없이 렌더한다는 것이기 때문에 옵션과는 상관없이 동작해야 할 것이다.
		if ( !DxSurfaceTex::GetInstance().m_pTempSuf128 )
			return;
	}
	else
	{
		if ( DxEffectMan::GetInstance().GetSkinDetail() != SKD_BEST )
			return;
	}

	D3DXVECTOR2	vMove;

	//	기본
	vMove.x = m_vSumTex01.x * m_vTexUV01.x;	// m_vTexUV01 이것은 UV 스케일 값이다.
	vMove.y = m_vSumTex01.y * m_vTexUV01.y;
	DxImageMove ( pd3dDevice, m_textureRes1.GetTexture(), DxSurfaceTex::GetInstance().m_pTempSuf128,
				vMove, TRUE, D3DFVF_DIFFUSE, m_cDiffuse1, m_vTexUV01, 128 );			// Alpha 를 써야 한다면 m_cDiffuse를 건들어야 할 듯.

	//	추가 ( 선택 사항 )
	if ( m_nTexNum == 2 )
	{
		vMove.x = m_vSumTex02.x * m_vTexUV02.x;
		vMove.y = m_vSumTex02.y * m_vTexUV02.y;
		DxImageMove ( pd3dDevice, m_textureRes2.GetTexture(), DxSurfaceTex::GetInstance().m_pTempSuf128, 
					vMove, FALSE, D3DFVF_DIFFUSE, m_cDiffuse2, m_vTexUV02, 128 );
	}

	if ( m_dwFlag & USETEXMAP )
	{
		DxImageMapRHW ( pd3dDevice, m_textureRes.GetTexture(), DxSurfaceTex::GetInstance().m_pTempSuf128, 
					FALSE, 128 );
	}

	m_pSavedStateBlock->Capture();
	m_pEffectStateBlock->Apply();

	D3DCAPSQ d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );
	DWORD	dwSrcBlend, dwDestBlend, dwAlphaBlendEnable, dwAlpgaTestEnable, dwAlphaRef, dwAlphaFunc;

	pd3dDevice->GetRenderState ( D3DRS_SRCBLEND,			&dwSrcBlend );
	pd3dDevice->GetRenderState ( D3DRS_DESTBLEND,			&dwDestBlend );
	pd3dDevice->GetRenderState ( D3DRS_ALPHABLENDENABLE,	&dwAlphaBlendEnable );
	pd3dDevice->GetRenderState ( D3DRS_ALPHATESTENABLE,		&dwAlpgaTestEnable );
	pd3dDevice->GetRenderState ( D3DRS_ALPHAREF,			&dwAlphaRef );
	pd3dDevice->GetRenderState ( D3DRS_ALPHAFUNC,			&dwAlphaFunc );

	//	Note : 블렌딩 요소 셋팅
	//
	switch ( m_nBlend )
	{
	case 0 :
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCCOLOR );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );

		break;
	case 1:
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_ZERO );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_SRCCOLOR );

		break;
	case 2:
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	FALSE );

		if( d3dCaps.AlphaCmpCaps & D3DPCMPCAPS_GREATEREQUAL )
		{
			pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
			pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0x80 );
			pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
		}

		break;			
	}

	for ( DWORD i=0; i<m_dwMaterials; i++ )
	{
		m_pMaterials[i].SetTexture( DxSurfaceTex::GetInstance().m_pTempTex128 );
	}

	if ( m_dwFlag & USE_ALL_TEX )
	{
		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
			m_pMaterials_EffUse[i] = m_pMaterials[i].GetType();	// Use 백업
			m_pMaterials[i].SetType( EMCET_BASE );				// 설정
		}
	}

	CHARSETTING sCharSetting;
	sCharSetting.pMeshContainerBase = m_pmcMesh;
	sCharSetting.pmtrlPiece			= pMaterialPiece;
	sCharSetting.pmtrlSpecular		= m_pMaterials;
	sCharSetting.bWorldIdentity		= bPieceRender;
	sCharSetting.emRDOP				= CTOP_BASE;

	// Draw
	m_pSkinMesh->SetDrawState ( FALSE, FALSE, FALSE, FALSE );
	m_pSkinMesh->DrawMeshContainer ( pd3dDevice, sCharSetting, m_rSkeleton, *pmatStaticLinkBone, arrayBoneMatrices );

	if ( m_dwFlag & USE_ALL_TEX )
	{
		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
			m_pMaterials[i].SetType( m_pMaterials_EffUse[i] );	// Use 백업
		}
	}

	for ( DWORD i=0; i<m_dwMaterials; i++ )
	{
		m_pMaterials[i].SetTexture( NULL );
	}

	pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			dwSrcBlend );
	pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			dwDestBlend );
	pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	dwAlphaBlendEnable );
	pd3dDevice->SetRenderState ( D3DRS_ALPHATESTENABLE,		dwAlpgaTestEnable );
	pd3dDevice->SetRenderState ( D3DRS_ALPHAREF,			dwAlphaRef );
	pd3dDevice->SetRenderState ( D3DRS_ALPHAFUNC,			dwAlphaFunc );

	m_pSavedStateBlock->Apply();
}


void DxEffCharMultiTex::RenderMaterialAdd ( LPDIRECT3DDEVICEQ pd3dDevice, 
										   SMATERIAL_PIECE* pMaterialSystem, 
										   const D3DXMATRIX& matStaticLinkBone,
										   DxSkinMesh9_Material& sSkinMesh,
										   const DxPhysXCloth* pPhysXCloth )
{
	HRESULT hr = S_OK;

	// DXMATERIAL_CHAR_EFF 확인 및 생성작업.
	if ( !IsExist_Copy_pmcMesh() )
		return;

	DxSkinMesh9_Material* pSkinMesh = &sSkinMesh;

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

	// BaseRenderOff 가 1개라도 존재하는가~?
	BOOL bEnableBaseRenderOff(FALSE);
	for ( DWORD i=0; i<m_dwMaterials; i++ )
	{
		if ( pMaterialPiece[i].m_bBaseRenderOff )
			bEnableBaseRenderOff = TRUE;
	}

	// BaseRenderOff 가 1개도 존재 안한다면, 이 밑으로 작업할 필요가 없다.
	// FALSE 여야 밑으로 작업 안한다. ( BaseRenderOff 라면 RenderMaterialAdd 작업이 전부된다. )
	if ( !bEnableBaseRenderOff )
		return;

	// DXMATERIAL_CHAR_EFF 확인 및 생성작업.
	if ( !IsExist_Copy_pmcMesh() )
		return;

	if ( !DxSurfaceTex::GetInstance().m_pTempSuf128 )
		return;

	// 여기 들어온다는건 원본 없이 렌더한다는 것이기 때문에 옵션과는 상관없이 동작해야 할 것이다.
	//if ( DxEffectMan::GetInstance().GetSkinDetail() != SKD_BEST )	return;

	D3DXVECTOR2	vMove;

	//	기본
	vMove.x = m_vSumTex01.x * m_vTexUV01.x;	// m_vTexUV01 이것은 UV 스케일 값이다.
	vMove.y = m_vSumTex01.y * m_vTexUV01.y;
	DxImageMove ( pd3dDevice, m_textureRes1.GetTexture(), DxSurfaceTex::GetInstance().m_pTempSuf128,
		vMove, TRUE, D3DFVF_DIFFUSE, m_cDiffuse1, m_vTexUV01, 128 );			// Alpha 를 써야 한다면 m_cDiffuse를 건들어야 할 듯.

	//	추가 ( 선택 사항 )
	if ( m_nTexNum == 2 )
	{
		vMove.x = m_vSumTex02.x * m_vTexUV02.x;
		vMove.y = m_vSumTex02.y * m_vTexUV02.y;
		DxImageMove ( pd3dDevice, m_textureRes2.GetTexture(), DxSurfaceTex::GetInstance().m_pTempSuf128, 
			vMove, FALSE, D3DFVF_DIFFUSE, m_cDiffuse2, m_vTexUV02, 128 );
	}

	if ( m_dwFlag & USETEXMAP )
	{
		DxImageMapRHW ( pd3dDevice, m_textureRes.GetTexture(), DxSurfaceTex::GetInstance().m_pTempSuf128, 
			FALSE, 128 );
	}

	m_pSavedStateBlock->Capture();
	m_pEffectStateBlock->Apply();

	D3DCAPSQ d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );
	DWORD	dwSrcBlend, dwDestBlend, dwAlphaBlendEnable, dwAlpgaTestEnable, dwAlphaRef, dwAlphaFunc;

	pd3dDevice->GetRenderState ( D3DRS_SRCBLEND,			&dwSrcBlend );
	pd3dDevice->GetRenderState ( D3DRS_DESTBLEND,			&dwDestBlend );
	pd3dDevice->GetRenderState ( D3DRS_ALPHABLENDENABLE,	&dwAlphaBlendEnable );
	pd3dDevice->GetRenderState ( D3DRS_ALPHATESTENABLE,		&dwAlpgaTestEnable );
	pd3dDevice->GetRenderState ( D3DRS_ALPHAREF,			&dwAlphaRef );
	pd3dDevice->GetRenderState ( D3DRS_ALPHAFUNC,			&dwAlphaFunc );

	//	Note : 블렌딩 요소 셋팅
	//
	switch ( m_nBlend )
	{
	case 0 :
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCCOLOR );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );

		break;
	case 1:
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_ZERO );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_SRCCOLOR );

		break;
	case 2:
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	FALSE );

		if( d3dCaps.AlphaCmpCaps & D3DPCMPCAPS_GREATEREQUAL )
		{
			pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
			pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0x80 );
			pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
		}

		break;			
	}

	for ( DWORD i=0; i<m_dwMaterials; i++ )
	{
		m_pMaterials[i].SetTexture( DxSurfaceTex::GetInstance().m_pTempTex128 );
	}

	if ( m_dwFlag & USE_ALL_TEX )
	{
		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
			m_pMaterials_EffUse[i] = m_pMaterials[i].GetType();	// Use 백업

			// 기본렌더링이 안될때만 동작되도록 한다.
			if ( pMaterialPiece[i].m_bBaseRenderOff )
				m_pMaterials[i].SetType( EMCET_BASE );				// 설정
			else
				m_pMaterials[i].SetType( EMCET_NULL );				// 설정
		}
	}
	else
	{
		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
			m_pMaterials_EffUse[i] = m_pMaterials[i].GetType();	// Use 백업

			// 기본렌더링이 동작되면 Effect 는 동작되어선 안된다.
			if ( !pMaterialPiece[i].m_bBaseRenderOff )
				m_pMaterials[i].SetType( EMCET_NULL );				// 설정
		}
	}

	// Draw
	pSkinMesh->RenderMaterialGlow ( pd3dDevice, m_pmcMesh, pBoneMatrices, pMaterialSystem, m_pMaterials, 0, matStaticLinkBone, pPhysXCloth );

	if ( m_dwFlag & USE_ALL_TEX )
	{
		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
			m_pMaterials[i].SetType( m_pMaterials_EffUse[i] );	// Use 복원
		}
	}

	for ( DWORD i=0; i<m_dwMaterials; i++ )
	{
		m_pMaterials[i].SetTexture( NULL );
	}

	pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			dwSrcBlend );
	pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			dwDestBlend );
	pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	dwAlphaBlendEnable );
	pd3dDevice->SetRenderState ( D3DRS_ALPHATESTENABLE,		dwAlpgaTestEnable );
	pd3dDevice->SetRenderState ( D3DRS_ALPHAREF,			dwAlphaRef );
	pd3dDevice->SetRenderState ( D3DRS_ALPHAFUNC,			dwAlphaFunc );

	m_pSavedStateBlock->Apply();
}

HRESULT DxEffCharMultiTex::LoadFile_0100 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	EFFCHAR_PROPERTY_MULTITEX_100_1	sProp;
	//	Note : 이팩트의 Property 를 저장.
	//
	SFile.ReadBuffer ( &sProp, sizeof(EFFCHAR_PROPERTY_MULTITEX_100_1) );

	m_dwFlag	= sProp.m_dwFlag;
	m_dwFlag	|= USE_SELECT_TEX;	// Ver.105 에서 새롭게 추가된 사항

	m_nBlend	= sProp.m_nBlend;
	m_nTexNum	= sProp.m_nTexNum;

	m_vTex01	= sProp.m_vTex01;
	m_vTex02	= sProp.m_vTex02;

	m_vTexUV01	= D3DXVECTOR2 ( 1.f, 1.f );
	m_vTexUV02	= D3DXVECTOR2 ( 1.f, 1.f );

	StringCchCopy( m_szTex1, MAX_PATH, sProp.m_szTex1 );
	StringCchCopy( m_szTex2, MAX_PATH, sProp.m_szTex2 );

	StringCchCopy( m_szTexture, MAX_PATH, sProp.m_szTexture );

	m_cDiffuse1 = 0xff000000;
	m_cDiffuse1 += (DWORD)(sProp.m_cDiffuse1.r*255)<<16;
	m_cDiffuse1 += (DWORD)(sProp.m_cDiffuse1.g*255)<<8;
	m_cDiffuse1 += (DWORD)(sProp.m_cDiffuse1.b*255);

	m_cDiffuse2 = 0xff000000;
	m_cDiffuse2 += (DWORD)(sProp.m_cDiffuse1.r*255)<<16;
	m_cDiffuse2 += (DWORD)(sProp.m_cDiffuse1.g*255)<<8;
	m_cDiffuse2 += (DWORD)(sProp.m_cDiffuse1.b*255);


	SAFE_DELETE_ARRAY ( m_pTempMaterials );
	m_pTempMaterials = new DXMATERIAL_CHAR_EFF[3];
	m_dwTempMaterials = 3;

	if ( m_dwFlag&USETEXTURE1 )			m_pTempMaterials[0].SetType( EMCET_BASE );
	if ( m_dwFlag&USETEXTURE2 )			m_pTempMaterials[1].SetType( EMCET_BASE );
	if ( m_dwFlag&USETEXTURE3 )			m_pTempMaterials[2].SetType( EMCET_BASE );

	StringCchCopy( m_pTempMaterials[0].szTexture, MAX_PATH, sProp.m_szSrcTex_1 );
	StringCchCopy( m_pTempMaterials[1].szTexture, MAX_PATH, sProp.m_szSrcTex_2 );
	StringCchCopy( m_pTempMaterials[2].szTexture, MAX_PATH, sProp.m_szSrcTex_3 );

	return S_OK;
}

HRESULT DxEffCharMultiTex::LoadFile_0101 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	EFFCHAR_PROPERTY_MULTITEX_100_1	sProp;
	//	Note : 이팩트의 Property 를 저장.
	//
	SFile.ReadBuffer ( &sProp, sizeof(EFFCHAR_PROPERTY_MULTITEX_100_1) );

	m_dwFlag	= sProp.m_dwFlag;
	m_dwFlag	|= USE_SELECT_TEX;	// Ver.105 에서 새롭게 추가된 사항

	m_nBlend	= sProp.m_nBlend;
	m_nTexNum	= sProp.m_nTexNum;

	m_vTex01	= sProp.m_vTex01;
	m_vTex02	= sProp.m_vTex02;

	m_vTexUV01	= D3DXVECTOR2 ( 1.f, 1.f );
	m_vTexUV02	= D3DXVECTOR2 ( 1.f, 1.f );

	StringCchCopy( m_szTex1, MAX_PATH, sProp.m_szTex1 );
	StringCchCopy( m_szTex2, MAX_PATH, sProp.m_szTex2 );

	StringCchCopy( m_szTexture, MAX_PATH, sProp.m_szTexture );

	m_cDiffuse1 = 0xff000000;
	m_cDiffuse1 += (DWORD)(sProp.m_cDiffuse1.r*255)<<16;
	m_cDiffuse1 += (DWORD)(sProp.m_cDiffuse1.g*255)<<8;
	m_cDiffuse1 += (DWORD)(sProp.m_cDiffuse1.b*255);

	m_cDiffuse2 = 0xff000000;
	m_cDiffuse2 += (DWORD)(sProp.m_cDiffuse1.r*255)<<16;
	m_cDiffuse2 += (DWORD)(sProp.m_cDiffuse1.g*255)<<8;
	m_cDiffuse2 += (DWORD)(sProp.m_cDiffuse1.b*255);


	SAFE_DELETE_ARRAY ( m_pTempMaterials );
	m_pTempMaterials = new DXMATERIAL_CHAR_EFF[3];
	m_dwTempMaterials = 3;

	if ( m_dwFlag&USETEXTURE1 )			m_pTempMaterials[0].SetType( EMCET_BASE );
	if ( m_dwFlag&USETEXTURE2 )			m_pTempMaterials[1].SetType( EMCET_BASE );
	if ( m_dwFlag&USETEXTURE3 )			m_pTempMaterials[2].SetType( EMCET_BASE );

	StringCchCopy( m_pTempMaterials[0].szTexture, MAX_PATH, sProp.m_szSrcTex_1 );
	StringCchCopy( m_pTempMaterials[1].szTexture, MAX_PATH, sProp.m_szSrcTex_2 );
	StringCchCopy( m_pTempMaterials[2].szTexture, MAX_PATH, sProp.m_szSrcTex_3 );
    
	return S_OK;
}

HRESULT DxEffCharMultiTex::LoadFile_0102 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	EFFCHAR_PROPERTY_MULTITEX_102	sProp;
	//	Note : 이팩트의 Property 를 저장.
	//
	SFile.ReadBuffer ( &sProp, sizeof(EFFCHAR_PROPERTY_MULTITEX_102) );

	m_dwFlag	= sProp.m_dwFlag;
	m_dwFlag	|= USE_SELECT_TEX;	// Ver.105 에서 새롭게 추가된 사항

	m_nBlend	= sProp.m_nBlend;
	m_nTexNum	= sProp.m_nTexNum;

	m_vTex01	= sProp.m_vTex01;
	m_vTex02	= sProp.m_vTex02;

	m_vTexUV01	= sProp.m_vTexUV01;
	m_vTexUV02	= sProp.m_vTexUV02;

	StringCchCopy( m_szTex1, MAX_PATH, sProp.m_szTex1 );
	StringCchCopy( m_szTex2, MAX_PATH, sProp.m_szTex2 );

	StringCchCopy( m_szTexture, MAX_PATH, sProp.m_szTexture );

	m_cDiffuse1 = 0xff000000;
	m_cDiffuse1 += (DWORD)(sProp.m_cDiffuse1.r*255)<<16;
	m_cDiffuse1 += (DWORD)(sProp.m_cDiffuse1.g*255)<<8;
	m_cDiffuse1 += (DWORD)(sProp.m_cDiffuse1.b*255);

	m_cDiffuse2 = 0xff000000;
	m_cDiffuse2 += (DWORD)(sProp.m_cDiffuse1.r*255)<<16;
	m_cDiffuse2 += (DWORD)(sProp.m_cDiffuse1.g*255)<<8;
	m_cDiffuse2 += (DWORD)(sProp.m_cDiffuse1.b*255);


	SAFE_DELETE_ARRAY ( m_pTempMaterials );
	m_pTempMaterials = new DXMATERIAL_CHAR_EFF[3];
	m_dwTempMaterials = 3;

	if ( m_dwFlag&USETEXTURE1 )			m_pTempMaterials[0].SetType( EMCET_BASE );
	if ( m_dwFlag&USETEXTURE2 )			m_pTempMaterials[1].SetType( EMCET_BASE );
	if ( m_dwFlag&USETEXTURE3 )			m_pTempMaterials[2].SetType( EMCET_BASE );

	StringCchCopy( m_pTempMaterials[0].szTexture, MAX_PATH, sProp.m_szSrcTex_1 );
	StringCchCopy( m_pTempMaterials[1].szTexture, MAX_PATH, sProp.m_szSrcTex_2 );
	StringCchCopy( m_pTempMaterials[2].szTexture, MAX_PATH, sProp.m_szSrcTex_3 );

	return S_OK;
}

HRESULT DxEffCharMultiTex::LoadFile_0103 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 이팩트의 Property 를 읽기
	//
	EFFCHAR_PROPERTY_MULTITEX_103_105 pProperty;

	SFile.ReadBuffer ( &pProperty, sizeof(EFFCHAR_PROPERTY_MULTITEX_103_105) );

	m_Property.Assign( pProperty );

	m_dwFlag	|= USE_SELECT_TEX;	// Ver.105 에서 새롭게 추가된 사항

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
	}

	return S_OK;
}

HRESULT DxEffCharMultiTex::LoadFile_0104 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 이팩트의 Property 를 읽기
	//
	EFFCHAR_PROPERTY_MULTITEX_103_105 pProperty;

	SFile.ReadBuffer ( &pProperty, sizeof(EFFCHAR_PROPERTY_MULTITEX_103_105) );

	m_Property.Assign( pProperty );

	if ( !( m_dwFlag&(USE_ALL_TEX|USE_SELECT_TEX) ) )
	{
		m_dwFlag	|= USE_SELECT_TEX;	// Ver.105 에서 새롭게 추가된 사항
	}

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
	}

	return S_OK;
}

HRESULT DxEffCharMultiTex::LoadFile_0105 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 이팩트의 Property 를 읽기
	//
	EFFCHAR_PROPERTY_MULTITEX_103_105 pProperty;

	SFile.ReadBuffer ( &pProperty, sizeof(EFFCHAR_PROPERTY_MULTITEX_103_105) );

	m_Property.Assign( pProperty );

	SFile >> m_dwMaterials;
	if ( m_dwMaterials )
	{
		SAFE_DELETE_ARRAY ( m_pMaterials );
		m_pMaterials = new DXMATERIAL_CHAR_EFF[m_dwMaterials];
        for( DWORD i=0; i<m_dwMaterials; ++i )
            m_pMaterials[i].LoadFile( SFile );
	}

	return S_OK;
}

HRESULT DxEffCharMultiTex::LoadFile_0106 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 이팩트의 Property 를 읽기
	//
	SFile.ReadBuffer ( &m_Property, sizeof(m_Property) );

	SFile >> m_dwMaterials;
	if ( m_dwMaterials )
	{
		SAFE_DELETE_ARRAY ( m_pMaterials );
		m_pMaterials = new DXMATERIAL_CHAR_EFF[m_dwMaterials];
		for( DWORD i=0; i<m_dwMaterials; ++i )
            m_pMaterials[i].LoadFile( SFile );
	}

	return S_OK;
}

HRESULT	DxEffCharMultiTex::LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
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

HRESULT	DxEffCharMultiTex::SaveFile ( sc::BaseStream &SFile )
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

//////////////////////////////////////////////////////////////////////////
// Material System 에 현재 효과들을 붙이기 위한 작업.
void DxEffCharMultiTex::PrecalculateForMaterialSystem( LPDIRECT3DDEVICEQ pd3dDevice, 
													  EffectMaterialData* pEffMaterialData, 
													  const D3DXMATRIX* pMatrix, 
													  const float fScale )
{
	// 성능이 최저사양이면 렌더 불가.
	// 최소 TnL_CHAR_PS_2_0 는 되야함.
	if ( RENDERPARAM::g_emCharacterQulity < TnL_CHAR_PS_2_0 )
		return;

	// Base RenderOff 상황에서는 이 작업을 할 필요가 없다.
	if ( IsBaseRenderOff() )
		return;

	switch ( m_nBlend )
	{
	case 0:
	case 1:
	case 2:
		pEffMaterialData->m_nMultiTex_ADD = m_nBlend;
		break;

	default:
		return;
	};

	pEffMaterialData->m_bMultiTex = TRUE;

	//////////////////////////////////////////////////////////////////////////
	// Tool 상에서 처음 셋팅 될 때는 무조건 if ( m_dwFlag & USE_ALL_TEX ) 로 된다.
	// 그 뒤로 변경을 하고 싶지만 if ( pEffMaterialData->m_vecEnableMultiTex.size() != m_dwMaterials ) 가 FALSE 이기 때문에 변화가 안생긴다.
	// 그래서 cps 의 FrameMove 쪽에서 EffectMaterialData::InitForTool() 를 해주도록 한다.
	//
	// Game 상에서는 한 번 정해지고 변경 될 일은 없기 때문에 EffectMaterialData::InitForTool() 는 필요 없다.
	//////////////////////////////////////////////////////////////////////////
	if ( pEffMaterialData->m_vecEnableMultiTex.size() != m_dwMaterials )
	{
		pEffMaterialData->m_vecEnableMultiTex.clear();

		if ( m_dwFlag & USE_ALL_TEX )
		{
			for ( DWORD i=0; i<m_dwMaterials; ++i )
			{
				pEffMaterialData->m_vecEnableMultiTex.push_back( TRUE );
			}
		}
		else
		{
			for ( DWORD i=0; i<m_dwMaterials; ++i )
			{
				pEffMaterialData->m_vecEnableMultiTex.push_back( m_pMaterials[i].IsType( EMCET_BASE ) ? TRUE : FALSE );
			}	
		}
	}

	PrecalculateForMaterialSystemMultiTex( pd3dDevice, pEffMaterialData, EMTT_NORMAL );
}

// HardAlpha
void DxEffCharMultiTex::PrecalculateForMaterialSystemHardAlpha( LPDIRECT3DDEVICEQ pd3dDevice, 
																EffectMaterialData* pEffMaterialData )
{
	// 성능이 최저사양이면 렌더 불가.
	// 최소 TnL_CHAR_PS_2_0 는 되야함.
	if ( RENDERPARAM::g_emCharacterQulity < TnL_CHAR_PS_2_0 )
		return;

	// Base RenderOff 상황에서는 이 작업을 할 필요가 없다.
	if ( IsBaseRenderOff() )
		return;

	PrecalculateForMaterialSystemMultiTex( pd3dDevice, pEffMaterialData, EMTT_ALPHA_HARD );
}

// SoftAlpha
void DxEffCharMultiTex::PrecalculateForMaterialSystemSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, 
																EffectMaterialData* pEffMaterialData )
{
	// 성능이 최저사양이면 렌더 불가.
	// 최소 TnL_CHAR_PS_2_0 는 되야함.
	if ( RENDERPARAM::g_emCharacterQulity < TnL_CHAR_PS_2_0 )
		return;

	// Base RenderOff 상황에서는 이 작업을 할 필요가 없다.
	if ( IsBaseRenderOff() )
		return;

	PrecalculateForMaterialSystemMultiTex( pd3dDevice, pEffMaterialData, EMTT_ALPHA_SOFT );
}

// Add 에서 작업
void DxEffCharMultiTex::PrecalculateForMaterialSystemAdd( LPDIRECT3DDEVICEQ pd3dDevice, EffectMaterialData* pEffMaterialData )
{
	// 이 상황은 옵션 체크를 하지 않는다.
	//// 성능이 최저사양이면 렌더 불가.
	//// 최소 TnL_CHAR_PS_2_0 는 되야함.
	//if ( RENDERPARAM::g_emCharacterQulity < TnL_CHAR_PS_2_0 )
	//	return;

	// Base RenderON 상황에서는 이 작업을 할 필요가 없다.
	if ( !IsBaseRenderOff() )
		return;

	if ( PrecalculateForMaterialSystemMultiTex( pd3dDevice, pEffMaterialData, EMTT_NORMAL ) )
	{
	}
	else if ( PrecalculateForMaterialSystemMultiTex( pd3dDevice, pEffMaterialData, EMTT_ALPHA_HARD ) )
	{
	}
	else if ( PrecalculateForMaterialSystemMultiTex( pd3dDevice, pEffMaterialData, EMTT_ALPHA_SOFT ) )
	{
	}
}

// RenderMaterial 를 하기전 필요한 정보를 셋팅하기 위한 작업.
// DxEffCharMultiTex 의 pEffMaterialData->m_rMultiTex 정보가 매번 변하기 때문에 
// Render 전에 변한걸 가지고 있도록 한다.
BOOL DxEffCharMultiTex::PrecalculateForMaterialSystemMultiTex( LPDIRECT3DDEVICEQ pd3dDevice, EffectMaterialData* pEffMaterialData, TEXTURE_ALPHA emTextureType )
{
	// DXMATERIAL_CHAR_EFF 확인 및 생성작업.
	if ( !IsExist_Copy_pmcMesh() )
		return FALSE;

	SMATERIAL_PIECE*	pMaterialPiece = NULL;
	if ( m_pCharPart )
	{
		pMaterialPiece = m_pCharPart->GetSkinPiece()->m_arrayMaterialPiece;
	}
	else if ( m_pSkinPiece )
	{
		pMaterialPiece = m_pSkinPiece->m_arrayMaterialPiece;
	}

	if ( !pMaterialPiece )
		return FALSE;

	BOOL bRenderOn(FALSE);
	for ( DWORD i=0; i<m_dwMaterials; ++i )
	{
		if ( i >= pEffMaterialData->m_vecEnableMultiTex.size() )
		{
			break;
		}
		else if ( !pEffMaterialData->m_vecEnableMultiTex[i] )
		{
			// 멀티 텍스쳐 사용되는 것만 흐르는 효과가 나오도록 한다.
			continue;
		}

		if ( EMTT_ALPHA_SOFT == emTextureType )
		{
			if ( pMaterialPiece[i].m_textureRes.GetAlphaType() == EMTT_ALPHA_SOFT ||
				pMaterialPiece[i].m_textureRes.GetAlphaType() == EMTT_ALPHA_SOFT01 ||
				pMaterialPiece[i].m_textureRes.GetAlphaType() == EMTT_ALPHA_SOFT02 )
			{
				bRenderOn = TRUE;
				break;
			}
		}
		if ( pMaterialPiece[i].m_textureRes.GetAlphaType() == emTextureType )
		{
			bRenderOn = TRUE;
			break;
		}
	}

	if ( bRenderOn )
	{
		D3DXVECTOR2	vMove;

		//	기본
		vMove.x = m_vSumTex01.x * m_vTexUV01.x;	// m_vTexUV01 이것은 UV 스케일 값이다.
		vMove.y = m_vSumTex01.y * m_vTexUV01.y;
		DxImageMove ( pd3dDevice, m_textureRes1.GetTexture(), DxSurfaceTex::GetInstance().m_pTempSuf128,
			vMove, TRUE, D3DFVF_DIFFUSE, m_cDiffuse1, m_vTexUV01, 128 );			// Alpha 를 써야 한다면 m_cDiffuse를 건들어야 할 듯.

		//	추가 ( 선택 사항 )
		if ( m_nTexNum == 2 )
		{
			vMove.x = m_vSumTex02.x * m_vTexUV02.x;
			vMove.y = m_vSumTex02.y * m_vTexUV02.y;
			DxImageMove ( pd3dDevice, m_textureRes2.GetTexture(), DxSurfaceTex::GetInstance().m_pTempSuf128, 
				vMove, FALSE, D3DFVF_DIFFUSE, m_cDiffuse2, m_vTexUV02, 128 );
		}

		if ( m_dwFlag & USETEXMAP )
		{
			DxImageMapRHW ( pd3dDevice, m_textureRes.GetTexture(), DxSurfaceTex::GetInstance().m_pTempSuf128, 
				FALSE, 128 );
		}

		pEffMaterialData->m_rMultiTex = DxSurfaceTex::GetInstance().m_pTempTex128;

		return TRUE;
	}
	return FALSE;
}

BOOL DxEffCharMultiTex::IsBaseRenderOff()
{
	SMATERIAL_PIECE*	pMaterialPiece = NULL;
	if ( m_pCharPart )
	{
		pMaterialPiece = m_pCharPart->GetSkinPiece()->m_arrayMaterialPiece;
	}
	else if ( m_pSkinPiece )
	{
		pMaterialPiece = m_pSkinPiece->m_arrayMaterialPiece;
	}

	// DXMATERIAL_CHAR_EFF 확인 및 생성작업.
	if ( !IsExist_Copy_pmcMesh() )
		return FALSE;

	// BaseRenderOff 가 1개라도 존재하는가~?
	for ( DWORD i=0; i<m_dwMaterials; i++ )
	{
		if ( pMaterialPiece[i].m_bBaseRenderOff )
			return TRUE;
	}

	return FALSE;
}

BOOL DxEffCharMultiTex::IsExist_Copy_pmcMesh()
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
			if( i >= m_dwTempMaterials )
				continue;	// 잘 못 만들어서 이렇게 되었다. Loading시 m_dwTempMaterials = 3 으로 강제 셋팅 될 경우가 있다.

			if( !m_pmcMesh->pMaterials[i].pTextureFilename )
				continue;

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

		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
			if( m_pmcMesh->pMaterials[i].pTextureFilename )
				StringCchCopy( m_pMaterials[i].szTexture,	MAX_PATH, m_pmcMesh->pMaterials[i].pTextureFilename );
		}
	}
	else if ( !m_pMaterials )							// 매트리얼이 없을 때 디폴트 생성
	{
		m_dwMaterials = m_pmcMesh->GetNumMaterials();
		SAFE_DELETE_ARRAY ( m_pMaterials );
		m_pMaterials = new DXMATERIAL_CHAR_EFF[m_dwMaterials];

		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
			if( m_pmcMesh->pMaterials[i].pTextureFilename )
				StringCchCopy( m_pMaterials[i].szTexture,	MAX_PATH, m_pmcMesh->pMaterials[i].pTextureFilename );
		}
	}

	// 새로 생성
	m_pMaterials_EffUse = new EMCHAR_EFF_TYPE [ m_pmcMesh->GetNumMaterials() ];

	return TRUE;
}