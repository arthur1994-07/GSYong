#include "pch.h"
#include "../../DxEffect/Single/DxEffSingle.h"
#include "../Single/DxEffSinglePropGMan.h"
#include "../../../SigmaCore/File/SerialFile.h"

#include "../../DxTools/DxSurfaceTex.h"
#include "../../DxTools/DxShadowMap.h"
#include "../../DxTools/DxViewPort.h"
#include "../../DxEffect/Frame/DxEffectMan.h"

#include "../../DxMeshs/SkinMesh/DxCharPart.h"
#include "../../DxMeshs/SkinMesh/DxSkinPieceContainer.h"

#include "./DxEffCharMark.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

DWORD		DxEffCharMark::TYPEID			= EMEFFCHAR_MARK;
const DWORD	DxEffCharMark::VERSION			= 0x0101;
char		DxEffCharMark::NAME[MAX_PATH]	= "1.선택표시, 강화표시";

LPDIRECT3DSTATEBLOCK9		DxEffCharMark::m_pSavedStateBlock	= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffCharMark::m_pEffectStateBlock	= NULL;

DxEffCharMark::DxEffCharMark(void) :
	DxEffChar(),
	m_pSkinMesh(NULL),
	m_pmcMesh(NULL),
	m_vSumTex01(0.f,0.f),
	m_vSumTex02(0.f,0.f),
	m_pMaterials(NULL),
	m_dwMaterials(0L)
{
}

DxEffCharMark::~DxEffCharMark(void)
{
}

DxEffChar* DxEffCharMark::CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece, DWORD dwMaterialNum )
{
	HRESULT hr;
	DxEffCharMark *pEffChar = new DxEffCharMark;
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

HRESULT DxEffCharMark::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{	
	D3DCAPSQ d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		//	Note : SetRenderState() 선언
		//pd3dDevice->SetRenderState ( D3DRS_CULLMODE,			D3DCULL_CW );
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

HRESULT DxEffCharMark::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSavedStateBlock );
	SAFE_RELEASE( m_pEffectStateBlock );

	return S_OK;
}

HRESULT DxEffCharMark::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;

	//if ( m_pCharPart )
	//{
	//	m_pSkinMesh = m_pCharPart->m_rSkinMesh;
	//	m_pmcMesh = m_pCharPart->m_rMeshContainer;
	//}
	//else if ( m_pSkinPiece )
	//{
	//	m_pSkinMesh = m_pSkinPiece->m_skinMeshRes.get();
	//	m_pmcMesh = m_pSkinPiece->m_pmcMesh;
	//}

 //   m_textureRes1 = TextureManager::GetInstance().LoadTexture(
 //       m_szTex1,
 //       false,
 //       TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
 //       0,
 //       D3DFMT_UNKNOWN,
 //       TEXTURE_RESIZE_NONE,
 //       TEXTURE_CHAR,
	//	true,
	//	FALSE );

 //   m_textureRes2 = TextureManager::GetInstance().LoadTexture(
 //       m_szTex2,
 //       false,
 //       TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
 //       0,
 //       D3DFMT_UNKNOWN,
 //       TEXTURE_RESIZE_NONE,
 //       TEXTURE_CHAR,
	//	true,
	//	FALSE );

 //   SAFE_DELETE_ARRAY ( m_pMaterials );
	//m_dwMaterials = m_pmcMesh->GetNumMaterials();
	//m_pMaterials = new DXMATERIAL_CHAR_EFF[m_dwMaterials];

	//for ( DWORD i=0; i<m_dwMaterials; ++i )
	//	m_pMaterials[i].SetType( EMCET_BASE );

	return S_OK;
}

HRESULT DxEffCharMark::DeleteDeviceObjects ()
{
 //   m_textureRes1.Clear();
 //   m_textureRes2.Clear();

	//SAFE_DELETE_ARRAY ( m_pMaterials );
	//m_dwMaterials = 0;

	return S_OK;
}

HRESULT DxEffCharMark::FrameMove ( float fTime, float fElapsedTime )
{
	HRESULT hr = S_OK;

	//m_vSumTex01 += m_vTex01*fElapsedTime;
	//m_vSumTex02 += m_vTex02*fElapsedTime;

	return S_OK;
}

HRESULT	DxEffCharMark::SettingState ( LPDIRECT3DDEVICEQ pd3dDevice, SKINEFFDATA& sSKINEFFDATA )
{
	//D3DXMATRIX matIdentity;
	//D3DXMatrixIdentity( &matIdentity );
	//const D3DXMATRIX* pmatStaticLinkBone(NULL);

	//SMATERIAL_PIECE*	pMaterialPiece = NULL;
	//if ( m_pCharPart )	
	//{
	//	pmatStaticLinkBone = &m_pCharPart->GetStaticLinkBone();
	//	pMaterialPiece = m_pCharPart->GetSkinPiece()->m_arrayMaterialPiece;
	//}
	//else if ( m_pSkinPiece )
	//{
	//	pmatStaticLinkBone = &matIdentity;
	//	pMaterialPiece = m_pSkinPiece->m_arrayMaterialPiece;
	//}

	//D3DXVECTOR3		__vTempFromPt;
	//D3DXVECTOR3		vTempFromPt;
	//D3DXVECTOR3		vUp			= D3DXVECTOR3 ( 0.f, 1.f, 0.f );
	//D3DXVECTOR3&	vFromPt		= DxViewPort::GetInstance().GetFromPt();
	//D3DXVECTOR3&	vLookatPt	= DxViewPort::GetInstance().GetLookatPt();

	//__vTempFromPt = vFromPt;

	//D3DXVECTOR3	vDirect = DxViewPort::GetInstance().GetLookDir();

	//vTempFromPt = vFromPt + (vDirect*2.f);

	//DxViewPort::GetInstance().SetViewTrans ( vTempFromPt, vLookatPt, vUp );

	//if ( m_pSkinMesh )
	//{
	//	D3DXVECTOR2	vMove;

	//	//	기본
	//	vMove.x = m_vSumTex01.x * m_vTexUV01.x;	// m_vTexUV01 이것은 UV 스케일 값이다.
	//	vMove.y = m_vSumTex01.y * m_vTexUV01.y;
	//	DxImageMove ( pd3dDevice, m_textureRes1.GetTexture(), DxSurfaceTex::GetInstance().m_pTempSuf128,
	//				vMove, TRUE, D3DFVF_DIFFUSE, m_cDiffuse1, m_vTexUV01, 128 );			// Alpha 를 써야 한다면 m_cDiffuse를 건들어야 할 듯.
	//	//	추가 ( 선택 사항 )
	//	if ( m_nTexNum == 2 )
	//	{
	//		vMove.x = m_vSumTex02.x * m_vTexUV02.x;
	//		vMove.y = m_vSumTex02.y * m_vTexUV02.y;
	//		DxImageMove ( pd3dDevice, m_textureRes2.GetTexture(), DxSurfaceTex::GetInstance().m_pTempSuf128, 
	//					vMove, FALSE, D3DFVF_DIFFUSE, m_cDiffuse2, m_vTexUV02, 128 );
	//	}

	//	m_pSavedStateBlock->Capture();
	//	m_pEffectStateBlock->Apply();

	//	D3DCAPSQ d3dCaps;
	//	pd3dDevice->GetDeviceCaps ( &d3dCaps );
	//	DWORD	dwSrcBlend, dwDestBlend, dwAlphaBlendEnable, dwAlpgaTestEnable, dwAlphaRef, dwAlphaFunc;

	//	pd3dDevice->GetRenderState ( D3DRS_SRCBLEND,			&dwSrcBlend );
	//	pd3dDevice->GetRenderState ( D3DRS_DESTBLEND,			&dwDestBlend );
	//	pd3dDevice->GetRenderState ( D3DRS_ALPHABLENDENABLE,	&dwAlphaBlendEnable );
	//	pd3dDevice->GetRenderState ( D3DRS_ALPHATESTENABLE,		&dwAlpgaTestEnable );
	//	pd3dDevice->GetRenderState ( D3DRS_ALPHAREF,			&dwAlphaRef );
	//	pd3dDevice->GetRenderState ( D3DRS_ALPHAFUNC,			&dwAlphaFunc );

	//	//	Note : 블렌딩 요소 셋팅
	//	//
	//	switch ( m_nBlend )
	//	{
	//	case 0 :
	//		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCCOLOR );
	//		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );

	//		break;
	//	case 1:
	//		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_ZERO );
	//		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_SRCCOLOR );

	//		break;
	//	case 2:
	//		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	FALSE );

	//		if( d3dCaps.AlphaCmpCaps & D3DPCMPCAPS_GREATEREQUAL )
	//		{
	//			pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
	//			pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0x80 );
	//			pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
	//		}

	//		break;			
	//	}

	//	for ( DWORD i=0; i<m_dwMaterials; ++i )
	//	{
	//		m_pMaterials[i].SetTexture( DxSurfaceTex::GetInstance().m_pTempTex128 );
	//	}

	//	CHARSETTING sCharSetting;
	//	sCharSetting.pMeshContainerBase = m_pmcMesh;
	//	sCharSetting.pmtrlPiece			= pMaterialPiece;
	//	sCharSetting.pmtrlSpecular		= m_pMaterials;
	//	sCharSetting.bWorldIdentity		= FALSE;
	//	sCharSetting.emRDOP				= CTOP_BASE;

	//	m_pSkinMesh->SetDrawState ( FALSE, FALSE, FALSE, FALSE );
	//	m_pSkinMesh->DrawMeshContainer ( pd3dDevice, sCharSetting, m_rSkeleton, *pmatStaticLinkBone, arrayBoneMatrices );

	//	for ( DWORD i=0; i<m_dwMaterials; i++ )
 //       {
 //           m_pMaterials[i].SetTexture( NULL );
 //       }

	//	pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			dwSrcBlend );
	//	pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			dwDestBlend );
	//	pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	dwAlphaBlendEnable );
	//	pd3dDevice->SetRenderState ( D3DRS_ALPHATESTENABLE,		dwAlpgaTestEnable );
	//	pd3dDevice->SetRenderState ( D3DRS_ALPHAREF,			dwAlphaRef );
	//	pd3dDevice->SetRenderState ( D3DRS_ALPHAFUNC,			dwAlphaFunc );

	//	m_pSavedStateBlock->Apply();
	//}

	//DxViewPort::GetInstance().SetViewTrans ( __vTempFromPt, vLookatPt, vUp );

	return S_OK;
}

HRESULT	DxEffCharMark::RestoreState ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

HRESULT DxEffCharMark::LoadFile_0100 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 이팩트의 Property 를 저장.
	//
	EFFCHAR_PROPERTY_MARK_0100 pProperty;

	SFile.ReadBuffer ( &pProperty, sizeof(EFFCHAR_PROPERTY_MARK_0100) );

	m_Property.Assign( pProperty );

	return S_OK;
}

HRESULT DxEffCharMark::LoadFile_0101 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 이팩트의 Property 를 저장.
	//
	SFile.ReadBuffer ( &m_Property, sizeof(m_Property) );

	return S_OK;
}

HRESULT	DxEffCharMark::LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	SFile >> dwSize;

	switch ( dwVer )
	{
	case 0x0100:	LoadFile_0100( SFile, pd3dDevice );	break;
	case VERSION:	LoadFile_0101( SFile, pd3dDevice );	break;
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

HRESULT	DxEffCharMark::SaveFile ( sc::BaseStream &SFile )
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