//
//	DXMATERIAL_CHAR_EFF �� ��ȭ�ϸ鼭 0x105 �������� ��ȭ �Ͽ���.
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
char		DxEffCharMultiTex::NAME[MAX_PATH]	= "2.��Ƽ�ؽ��� Multi";

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

		//	Note : SetRenderState() ����
		float fBias = -0.0002f;
		pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,			*((DWORD*)&fBias) );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,			FALSE );

		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCCOLOR );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );

		//	Note : SetTextureStageState() ����
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

	// DXMATERIAL_CHAR_EFF �����۾�.
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

	// BaseRenderOff �� 1���� �����ϴ°�~?
	BOOL bEnableBaseRenderOff(FALSE);
	for ( DWORD i=0; i<m_dwMaterials; i++ )
	{
		if ( pMaterialPiece[i].m_bBaseRenderOff )
			bEnableBaseRenderOff = TRUE;
	}

	if ( bEnableBaseRenderOff )
	{
		// ���� ���´ٴ°� ���� ���� �����Ѵٴ� ���̱� ������ �ɼǰ��� ������� �����ؾ� �� ���̴�.
		if ( !DxSurfaceTex::GetInstance().m_pTempSuf128 )
			return;
	}
	else
	{
		if ( DxEffectMan::GetInstance().GetSkinDetail() != SKD_BEST )
			return;
	}

	D3DXVECTOR2	vMove;

	//	�⺻
	vMove.x = m_vSumTex01.x * m_vTexUV01.x;	// m_vTexUV01 �̰��� UV ������ ���̴�.
	vMove.y = m_vSumTex01.y * m_vTexUV01.y;
	DxImageMove ( pd3dDevice, m_textureRes1.GetTexture(), DxSurfaceTex::GetInstance().m_pTempSuf128,
				vMove, TRUE, D3DFVF_DIFFUSE, m_cDiffuse1, m_vTexUV01, 128 );			// Alpha �� ��� �Ѵٸ� m_cDiffuse�� �ǵ��� �� ��.

	//	�߰� ( ���� ���� )
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

	//	Note : ���� ��� ����
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
			m_pMaterials_EffUse[i] = m_pMaterials[i].GetType();	// Use ���
			m_pMaterials[i].SetType( EMCET_BASE );				// ����
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
			m_pMaterials[i].SetType( m_pMaterials_EffUse[i] );	// Use ���
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

	// DXMATERIAL_CHAR_EFF Ȯ�� �� �����۾�.
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

	// BaseRenderOff �� 1���� �����ϴ°�~?
	BOOL bEnableBaseRenderOff(FALSE);
	for ( DWORD i=0; i<m_dwMaterials; i++ )
	{
		if ( pMaterialPiece[i].m_bBaseRenderOff )
			bEnableBaseRenderOff = TRUE;
	}

	// BaseRenderOff �� 1���� ���� ���Ѵٸ�, �� ������ �۾��� �ʿ䰡 ����.
	// FALSE ���� ������ �۾� ���Ѵ�. ( BaseRenderOff ��� RenderMaterialAdd �۾��� ���εȴ�. )
	if ( !bEnableBaseRenderOff )
		return;

	// DXMATERIAL_CHAR_EFF Ȯ�� �� �����۾�.
	if ( !IsExist_Copy_pmcMesh() )
		return;

	if ( !DxSurfaceTex::GetInstance().m_pTempSuf128 )
		return;

	// ���� ���´ٴ°� ���� ���� �����Ѵٴ� ���̱� ������ �ɼǰ��� ������� �����ؾ� �� ���̴�.
	//if ( DxEffectMan::GetInstance().GetSkinDetail() != SKD_BEST )	return;

	D3DXVECTOR2	vMove;

	//	�⺻
	vMove.x = m_vSumTex01.x * m_vTexUV01.x;	// m_vTexUV01 �̰��� UV ������ ���̴�.
	vMove.y = m_vSumTex01.y * m_vTexUV01.y;
	DxImageMove ( pd3dDevice, m_textureRes1.GetTexture(), DxSurfaceTex::GetInstance().m_pTempSuf128,
		vMove, TRUE, D3DFVF_DIFFUSE, m_cDiffuse1, m_vTexUV01, 128 );			// Alpha �� ��� �Ѵٸ� m_cDiffuse�� �ǵ��� �� ��.

	//	�߰� ( ���� ���� )
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

	//	Note : ���� ��� ����
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
			m_pMaterials_EffUse[i] = m_pMaterials[i].GetType();	// Use ���

			// �⺻�������� �ȵɶ��� ���۵ǵ��� �Ѵ�.
			if ( pMaterialPiece[i].m_bBaseRenderOff )
				m_pMaterials[i].SetType( EMCET_BASE );				// ����
			else
				m_pMaterials[i].SetType( EMCET_NULL );				// ����
		}
	}
	else
	{
		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
			m_pMaterials_EffUse[i] = m_pMaterials[i].GetType();	// Use ���

			// �⺻�������� ���۵Ǹ� Effect �� ���۵Ǿ �ȵȴ�.
			if ( !pMaterialPiece[i].m_bBaseRenderOff )
				m_pMaterials[i].SetType( EMCET_NULL );				// ����
		}
	}

	// Draw
	pSkinMesh->RenderMaterialGlow ( pd3dDevice, m_pmcMesh, pBoneMatrices, pMaterialSystem, m_pMaterials, 0, matStaticLinkBone, pPhysXCloth );

	if ( m_dwFlag & USE_ALL_TEX )
	{
		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
			m_pMaterials[i].SetType( m_pMaterials_EffUse[i] );	// Use ����
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
	//	Note : ����Ʈ�� Property �� ����.
	//
	SFile.ReadBuffer ( &sProp, sizeof(EFFCHAR_PROPERTY_MULTITEX_100_1) );

	m_dwFlag	= sProp.m_dwFlag;
	m_dwFlag	|= USE_SELECT_TEX;	// Ver.105 ���� ���Ӱ� �߰��� ����

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
	//	Note : ����Ʈ�� Property �� ����.
	//
	SFile.ReadBuffer ( &sProp, sizeof(EFFCHAR_PROPERTY_MULTITEX_100_1) );

	m_dwFlag	= sProp.m_dwFlag;
	m_dwFlag	|= USE_SELECT_TEX;	// Ver.105 ���� ���Ӱ� �߰��� ����

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
	//	Note : ����Ʈ�� Property �� ����.
	//
	SFile.ReadBuffer ( &sProp, sizeof(EFFCHAR_PROPERTY_MULTITEX_102) );

	m_dwFlag	= sProp.m_dwFlag;
	m_dwFlag	|= USE_SELECT_TEX;	// Ver.105 ���� ���Ӱ� �߰��� ����

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
	//	Note : ����Ʈ�� Property �� �б�
	//
	EFFCHAR_PROPERTY_MULTITEX_103_105 pProperty;

	SFile.ReadBuffer ( &pProperty, sizeof(EFFCHAR_PROPERTY_MULTITEX_103_105) );

	m_Property.Assign( pProperty );

	m_dwFlag	|= USE_SELECT_TEX;	// Ver.105 ���� ���Ӱ� �߰��� ����

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
	//	Note : ����Ʈ�� Property �� �б�
	//
	EFFCHAR_PROPERTY_MULTITEX_103_105 pProperty;

	SFile.ReadBuffer ( &pProperty, sizeof(EFFCHAR_PROPERTY_MULTITEX_103_105) );

	m_Property.Assign( pProperty );

	if ( !( m_dwFlag&(USE_ALL_TEX|USE_SELECT_TEX) ) )
	{
		m_dwFlag	|= USE_SELECT_TEX;	// Ver.105 ���� ���Ӱ� �߰��� ����
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
	//	Note : ����Ʈ�� Property �� �б�
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
	//	Note : ����Ʈ�� Property �� �б�
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

HRESULT	DxEffCharMultiTex::SaveFile ( sc::BaseStream &SFile )
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
void DxEffCharMultiTex::PrecalculateForMaterialSystem( LPDIRECT3DDEVICEQ pd3dDevice, 
													  EffectMaterialData* pEffMaterialData, 
													  const D3DXMATRIX* pMatrix, 
													  const float fScale )
{
	// ������ ��������̸� ���� �Ұ�.
	// �ּ� TnL_CHAR_PS_2_0 �� �Ǿ���.
	if ( RENDERPARAM::g_emCharacterQulity < TnL_CHAR_PS_2_0 )
		return;

	// Base RenderOff ��Ȳ������ �� �۾��� �� �ʿ䰡 ����.
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
	// Tool �󿡼� ó�� ���� �� ���� ������ if ( m_dwFlag & USE_ALL_TEX ) �� �ȴ�.
	// �� �ڷ� ������ �ϰ� ������ if ( pEffMaterialData->m_vecEnableMultiTex.size() != m_dwMaterials ) �� FALSE �̱� ������ ��ȭ�� �Ȼ����.
	// �׷��� cps �� FrameMove �ʿ��� EffectMaterialData::InitForTool() �� ���ֵ��� �Ѵ�.
	//
	// Game �󿡼��� �� �� �������� ���� �� ���� ���� ������ EffectMaterialData::InitForTool() �� �ʿ� ����.
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
	// ������ ��������̸� ���� �Ұ�.
	// �ּ� TnL_CHAR_PS_2_0 �� �Ǿ���.
	if ( RENDERPARAM::g_emCharacterQulity < TnL_CHAR_PS_2_0 )
		return;

	// Base RenderOff ��Ȳ������ �� �۾��� �� �ʿ䰡 ����.
	if ( IsBaseRenderOff() )
		return;

	PrecalculateForMaterialSystemMultiTex( pd3dDevice, pEffMaterialData, EMTT_ALPHA_HARD );
}

// SoftAlpha
void DxEffCharMultiTex::PrecalculateForMaterialSystemSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, 
																EffectMaterialData* pEffMaterialData )
{
	// ������ ��������̸� ���� �Ұ�.
	// �ּ� TnL_CHAR_PS_2_0 �� �Ǿ���.
	if ( RENDERPARAM::g_emCharacterQulity < TnL_CHAR_PS_2_0 )
		return;

	// Base RenderOff ��Ȳ������ �� �۾��� �� �ʿ䰡 ����.
	if ( IsBaseRenderOff() )
		return;

	PrecalculateForMaterialSystemMultiTex( pd3dDevice, pEffMaterialData, EMTT_ALPHA_SOFT );
}

// Add ���� �۾�
void DxEffCharMultiTex::PrecalculateForMaterialSystemAdd( LPDIRECT3DDEVICEQ pd3dDevice, EffectMaterialData* pEffMaterialData )
{
	// �� ��Ȳ�� �ɼ� üũ�� ���� �ʴ´�.
	//// ������ ��������̸� ���� �Ұ�.
	//// �ּ� TnL_CHAR_PS_2_0 �� �Ǿ���.
	//if ( RENDERPARAM::g_emCharacterQulity < TnL_CHAR_PS_2_0 )
	//	return;

	// Base RenderON ��Ȳ������ �� �۾��� �� �ʿ䰡 ����.
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

// RenderMaterial �� �ϱ��� �ʿ��� ������ �����ϱ� ���� �۾�.
// DxEffCharMultiTex �� pEffMaterialData->m_rMultiTex ������ �Ź� ���ϱ� ������ 
// Render ���� ���Ѱ� ������ �ֵ��� �Ѵ�.
BOOL DxEffCharMultiTex::PrecalculateForMaterialSystemMultiTex( LPDIRECT3DDEVICEQ pd3dDevice, EffectMaterialData* pEffMaterialData, TEXTURE_ALPHA emTextureType )
{
	// DXMATERIAL_CHAR_EFF Ȯ�� �� �����۾�.
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
			// ��Ƽ �ؽ��� ���Ǵ� �͸� �帣�� ȿ���� �������� �Ѵ�.
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

		//	�⺻
		vMove.x = m_vSumTex01.x * m_vTexUV01.x;	// m_vTexUV01 �̰��� UV ������ ���̴�.
		vMove.y = m_vSumTex01.y * m_vTexUV01.y;
		DxImageMove ( pd3dDevice, m_textureRes1.GetTexture(), DxSurfaceTex::GetInstance().m_pTempSuf128,
			vMove, TRUE, D3DFVF_DIFFUSE, m_cDiffuse1, m_vTexUV01, 128 );			// Alpha �� ��� �Ѵٸ� m_cDiffuse�� �ǵ��� �� ��.

		//	�߰� ( ���� ���� )
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

	// DXMATERIAL_CHAR_EFF Ȯ�� �� �����۾�.
	if ( !IsExist_Copy_pmcMesh() )
		return FALSE;

	// BaseRenderOff �� 1���� �����ϴ°�~?
	for ( DWORD i=0; i<m_dwMaterials; i++ )
	{
		if ( pMaterialPiece[i].m_bBaseRenderOff )
			return TRUE;
	}

	return FALSE;
}

BOOL DxEffCharMultiTex::IsExist_Copy_pmcMesh()
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

	if ( m_pTempMaterials )
	{
		m_dwMaterials = m_pmcMesh->GetNumMaterials();
		SAFE_DELETE_ARRAY ( m_pMaterials );
		m_pMaterials = new DXMATERIAL_CHAR_EFF[m_dwMaterials];

		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
			if( i >= m_dwTempMaterials )
				continue;	// �� �� ���� �̷��� �Ǿ���. Loading�� m_dwTempMaterials = 3 ���� ���� ���� �� ��찡 �ִ�.

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
		m_dwMaterials = m_pmcMesh->GetNumMaterials();		// ��Ʈ���� ������ �ٲ���� �� �׳� ������ ����Ʈ ����
		SAFE_DELETE_ARRAY ( m_pMaterials );
		m_pMaterials = new DXMATERIAL_CHAR_EFF[m_dwMaterials];

		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
			if( m_pmcMesh->pMaterials[i].pTextureFilename )
				StringCchCopy( m_pMaterials[i].szTexture,	MAX_PATH, m_pmcMesh->pMaterials[i].pTextureFilename );
		}
	}
	else if ( !m_pMaterials )							// ��Ʈ������ ���� �� ����Ʈ ����
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

	// ���� ����
	m_pMaterials_EffUse = new EMCHAR_EFF_TYPE [ m_pmcMesh->GetNumMaterials() ];

	return TRUE;
}