#include "pch.h"
#include "../../DxEffect/Single/DxEffSingle.h"
#include "../Single/DxEffSinglePropGMan.h"
#include "../../../SigmaCore/File/SerialFile.h"

#include "../../DxEffect/DxLoadShader.h"
#include "../../DxTools/DxRenderStates.h"

#include "../../DxTools/Light/DxLightMan.h"
#include "../../DxTools/DxViewPort.h"
#include "../../DxEffect/Frame/DxEffectMan.h"

#include "../../DxTools/RENDERPARAM.h"
#include "../../DxMeshs/SkinMesh/DxSkinChar.h"
#include "./DxEffCharHLSL.h"
#include "../../DxMeshs/SkinMesh/_new/DxSkinMesh9_HLSL.h"

#include "./DxEffCharToon.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

DWORD		DxEffCharToon::TYPEID			= EMEFFCHAR_TOON;
const DWORD	DxEffCharToon::VERSION			= 0x0101;
char		DxEffCharToon::NAME[MAX_PATH]	= "3.Toon Shading";

LPDIRECT3DSTATEBLOCK9		DxEffCharToon::m_pSavedSB			= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffCharToon::m_pEffectSB			= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffCharToon::m_pSavedEdgeSB		= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffCharToon::m_pDrawEdgeSB		= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffCharToon::m_pSavedObliqueSB_1	= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffCharToon::m_pDrawObliqueSB_1	= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffCharToon::m_pSavedObliqueSB_2	= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffCharToon::m_pDrawObliqueSB_2	= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffCharToon::m_pSavedObliqueSB_3	= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffCharToon::m_pDrawObliqueSB_3	= NULL;


DxEffCharToon::DxEffCharToon(void) :
	DxEffChar(),
	m_pSkinMesh(NULL),
	m_pmcMesh(NULL),
	m_fObliqueTEX(1.f)
{
}

DxEffCharToon::~DxEffCharToon(void)
{
	CleanUp ();
}

DxEffChar* DxEffCharToon::CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece, DWORD dwMaterialNum )
{
	HRESULT hr;
	DxEffCharToon *pEffChar = new DxEffCharToon;
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

HRESULT DxEffCharToon::StaticCreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

HRESULT DxEffCharToon::StaticResetDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr(S_OK);
	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		//	Note : SetRenderState() 선언
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState( D3DRS_COLORVERTEX,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );		// Normal 이 일정하면 이렇게 안해도 되는데 ... ㅡㅡ;

		//	Note : SetTextureStageState() 선언
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1 );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,	D3DTOP_MODULATE );

		pd3dDevice->SetTextureStageState ( 1, D3DTSS_TEXCOORDINDEX,	D3DTSS_TCI_CAMERASPACENORMAL );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_TEXTURETRANSFORMFLAGS,	D3DTTFF_COUNT2 );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedSB );
		else			pd3dDevice->EndStateBlock( &m_pEffectSB );
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		//	Note : SetRenderState() 선언
		pd3dDevice->SetRenderState( D3DRS_CULLMODE,			D3DCULL_CW );
		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR ,	0 );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,			FALSE );				// 
		pd3dDevice->SetRenderState( D3DRS_COLORVERTEX,		FALSE );				// 
		pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, FALSE );

		//	Note : SetTextureStageState() 선언
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,	D3DTA_TFACTOR );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1 );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedEdgeSB );
		else			pd3dDevice->EndStateBlock( &m_pDrawEdgeSB );
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState( D3DRS_COLORVERTEX,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );	// Normal 이 일정하면 이렇게 안해도 되는데 ... ㅡㅡ;

		//	Note : SetTextureStageState() 선언
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1 );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_TEXCOORDINDEX,	D3DTSS_TCI_CAMERASPACENORMAL );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_TEXTURETRANSFORMFLAGS,	D3DTTFF_COUNT2 );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedObliqueSB_1 );
		else			pd3dDevice->EndStateBlock( &m_pDrawObliqueSB_1 );
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,		TRUE );

		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_ONE );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_ONE );
		pd3dDevice->SetRenderState( D3DRS_BLENDOP,			D3DBLENDOP_MAX );

		pd3dDevice->SetRenderState( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState( D3DRS_COLORVERTEX,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,		FALSE );

		//	Note : SetTextureStageState() 선언
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1 );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_TEXCOORDINDEX,	D3DTSS_TCI_CAMERASPACEPOSITION );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_TEXTURETRANSFORMFLAGS,	D3DTTFF_COUNT3|D3DTTFF_PROJECTED );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedObliqueSB_2 );
		else			pd3dDevice->EndStateBlock( &m_pDrawObliqueSB_2 );
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,		TRUE );

		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_DESTCOLOR );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_ZERO );

		pd3dDevice->SetRenderState( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState( D3DRS_COLORVERTEX,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,		FALSE );

		//	Note : SetTextureStageState() 선언
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1 );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedObliqueSB_3 );
		else			pd3dDevice->EndStateBlock( &m_pDrawObliqueSB_3 );
	}

	/*
	LPDIRECT3DSTATEBLOCK9		DxEffCharToon::	= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffCharToon::		= NULL;
	*/

	return S_OK;
}

HRESULT DxEffCharToon::StaticLostDevice()
{
	HRESULT hr(S_OK);
	SAFE_RELEASE( m_pSavedSB );
	SAFE_RELEASE( m_pEffectSB );

	SAFE_RELEASE( m_pSavedEdgeSB );
	SAFE_RELEASE( m_pDrawEdgeSB );

	SAFE_RELEASE( m_pSavedObliqueSB_1 );
	SAFE_RELEASE( m_pDrawObliqueSB_1 );

	SAFE_RELEASE( m_pSavedObliqueSB_2 );
	SAFE_RELEASE( m_pDrawObliqueSB_2 );

	SAFE_RELEASE( m_pSavedObliqueSB_3 );
	SAFE_RELEASE( m_pDrawObliqueSB_3 );

	return S_OK;
}

void DxEffCharToon::StaticDestroyDevice()
{
}

HRESULT DxEffCharToon::OneTimeSceneInit ()
{
	return S_OK;
}

HRESULT DxEffCharToon::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
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

 //   m_textureRes = TextureManager::GetInstance().LoadTexture(
 //       m_szToonTex,
 //       false,
 //       TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
 //       0,
 //       D3DFMT_UNKNOWN,
 //       TEXTURE_RESIZE_NONE,
 //       TEXTURE_CHAR,
	//	false,
	//	TRUE );

 //   m_textureResOblique = TextureManager::GetInstance().LoadTexture(
 //       m_szObliqueTex,
 //       false,
 //       TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
 //       0,
 //       D3DFMT_UNKNOWN,
 //       TEXTURE_RESIZE_NONE,
 //       TEXTURE_CHAR,
	//	false,
	//	TRUE );

 //   TextureManager::GetInstance().BackUpCache( m_textureRes );
	//TextureManager::GetInstance().BackUpCache( m_textureResOblique );

	return S_OK;
}

HRESULT DxEffCharToon::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

HRESULT DxEffCharToon::InvalidateDeviceObjects ()
{
	return S_OK;
}

HRESULT DxEffCharToon::DeleteDeviceObjects ()
{
	m_textureRes.Clear();
    m_textureResOblique.Clear();

	return S_OK;
}

HRESULT DxEffCharToon::FinalCleanup ()
{

	return S_OK;
}

HRESULT DxEffCharToon::FrameMove ( float fTime, float fElapsedTime )
{
	HRESULT hr = S_OK;

	m_fObliqueTEX += fElapsedTime;
	if( m_fObliqueTEX > 10.f )	m_fObliqueTEX = 1.f;

	return S_OK;
}

HRESULT	DxEffCharToon::SettingState ( LPDIRECT3DDEVICEQ pd3dDevice, SKINEFFDATA& sSKINEFFDATA )
{
	HRESULT hr = S_OK;

	//if ( !m_pmcMesh )					return S_OK;
	//if ( !m_pmcMesh->MeshData.pMesh )	return S_OK;
	//if ( !m_pSkinMesh )					return S_OK;

	//SMATERIAL_PIECE*	pMaterialPiece = NULL;
	//if ( m_pCharPart )			pMaterialPiece = m_pCharPart->m_pMaterialPiece;
	//else if ( m_pSkinPiece )	pMaterialPiece = m_pSkinPiece->m_pMaterialPiece;


	//D3DXVECTOR3 vDir( 1.f, -1.f, 1.f );
	//D3DXVec3Normalize( &vDir, &vDir );

	//switch( RENDERPARAM::emCharRenderTYPE )
	//{
	//case EMCRT_SOFTWARE:
	//case EMCRT_NORMAL:
	//	// Note : 빗금 렌더링을 사용함.
	//	if( m_dwFlag&USE_OBLIQUE )
	//	{
	//		RenderSHADOW( pd3dDevice, pMaterialPiece );
	//		RenderOBLIQUE( pd3dDevice, pMaterialPiece );
	//		RenderBASE( pd3dDevice, pMaterialPiece );
	//	}
	//	else
	//	{
	//		DxSetTextureMatrix( pd3dDevice, 1, vDir );

	//		m_pSavedSB->Capture();
	//		m_pEffectSB->Apply();
	//		if( sSKINEFFDATA.m_rToonShadeTex )
	//		{
	//			pd3dDevice->SetTexture( 1, sSKINEFFDATA.m_rToonShadeTex );
	//		}
	//		else
	//		{
	//			pd3dDevice->SetTexture( 1, m_textureRes.GetTexture() );
	//		}

	//		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, sSKINEFFDATA.m_dwCTShadeColor );
	//		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,	D3DTA_TFACTOR );
	//		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );

	//		m_pSkinMesh->DrawMeshCartoon( pd3dDevice, m_pmcMesh, pMaterialPiece, CTOP_SHADOW );
	//		pd3dDevice->SetTexture( 1, NULL );
	//		m_pSavedSB->Apply();

	//		DxResetTextureMatrix( pd3dDevice, 1 );
	//	}
	//	break;
	//case EMCRT_VERTEX:
	//	pd3dDevice->SetTexture( 1, m_textureRes.GetTexture() );
	//	DxSkinMesh9_HLSL::m_pFX = NSCHARHLSL::m_pToonFX;
	//	m_pSkinMesh->DrawMeshCartoon( pd3dDevice, m_pmcMesh, pMaterialPiece, CTOP_SHADOW );
	//	pd3dDevice->SetTexture( 1, NULL );
	//	break;
	//case EMCRT_PIXEL:
	//	pd3dDevice->SetTexture( 1, m_textureRes.GetTexture() );
	//	DxSkinMesh9_HLSL::m_pFX = NSCHARHLSL::m_pToonFX;
	//	m_pSkinMesh->DrawMeshCartoon( pd3dDevice, m_pmcMesh, pMaterialPiece, CTOP_SHADOW );
	//	pd3dDevice->SetTexture( 1, NULL );
	//	break;
	//};

	//switch( RENDERPARAM::emCharRenderTYPE )
	//{
	//case EMCRT_SOFTWARE:
	//case EMCRT_NORMAL:
	//	m_pSavedEdgeSB->Capture();
	//	m_pDrawEdgeSB->Apply();

	//	pd3dDevice->SetTexture( 0, NULL );
	//	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, sSKINEFFDATA.m_dwEdgeColor );
	//	DxSkinMesh9_HLSL::m_pFX = NSCHARHLSL::m_pEdgeFX;
	//	m_pSkinMesh->DrawMeshCartoon( pd3dDevice, m_pmcMesh, pMaterialPiece, CTOP_EDGE );

	//	m_pSavedEdgeSB->Apply();
	//	break;
	//case EMCRT_VERTEX:
	//	pd3dDevice->SetTexture( 0, NULL );
	//	DxSkinMesh9_HLSL::m_pFX = NSCHARHLSL::m_pEdgeFX;
	//	m_pSkinMesh->DrawMeshCartoon( pd3dDevice, m_pmcMesh, pMaterialPiece, CTOP_EDGE );
	//	break;
	//case EMCRT_PIXEL:
	//	pd3dDevice->SetTexture( 0, NULL );
	//	DxSkinMesh9_HLSL::m_pFX = NSCHARHLSL::m_pEdgeFX;
	//	m_pSkinMesh->DrawMeshCartoon( pd3dDevice, m_pmcMesh, pMaterialPiece, CTOP_EDGE );
	//	break;
	//};

	return S_OK;
}

HRESULT	DxEffCharToon::RestoreState ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

void DxEffCharToon::RenderSHADOW( LPDIRECT3DDEVICEQ pd3dDevice, SMATERIAL_PIECE *pmtrlPiece )
{
	//D3DXVECTOR3 vDir( 1.f, -1.f, 1.f );
	//D3DXVec3Normalize( &vDir, &vDir );

	//DxSetTextureMatrix( pd3dDevice, 0, vDir );

	//m_pSavedObliqueSB_1->Capture();
	//m_pDrawObliqueSB_1->Apply();
	//pd3dDevice->SetTexture( 0, m_textureRes.GetTexture() );
	//m_pSkinMesh->DrawMeshCartoon( pd3dDevice, m_pmcMesh, pmtrlPiece, CTOP_NEWTEX );
	//m_pSavedObliqueSB_1->Apply();

	//DxResetTextureMatrix( pd3dDevice, 0 );
}

void DxEffCharToon::RenderOBLIQUE( LPDIRECT3DDEVICEQ pd3dDevice, SMATERIAL_PIECE *pmtrlPiece )
{
//	D3DXVECTOR3 vDis = DxViewPort::GetInstance().GetFromPt();
//	float fScale = D3DXVec3Length( &vDis );
//	fScale *= 0.1f;
//
//	D3DXMATRIX matScale;	  	   
//	memset( &matScale, 0, sizeof(D3DXMATRIX) );
//	matScale._11 = -fScale;//*m_fObliqueTEX;
//	matScale._22 = fScale;
//	//matScale._33 = m_fObliqueTEX;
//	matScale._33 = 1.f;
//	matScale._44 = 1.f;
//
//	pd3dDevice->SetTransform( D3DTS_TEXTURE0, &matScale );
//
//	//D3DXMATRIX lightView;
//	//D3DXVECTOR3 vFromPt(0.f,0.f,0.f);
//	//D3DXVECTOR3 vLookat( 1.f, -1.f, 1.f );
//	//D3DXVECTOR3 vUp( 0.f, 1.f, 0.f );
//	//D3DXMatrixLookAtLH( &lightView, &vFromPt, &vLookat, &vUp );
//
//	//DxSetTextureMatrix( pd3dDevice, 0, lightView );
//
//	m_pSavedObliqueSB_2->Capture();
//	m_pDrawObliqueSB_2->Apply();
//	pd3dDevice->SetTexture( 0, m_textureResOblique.GetTexture() );
//	m_pSkinMesh->DrawMeshCartoon( pd3dDevice, m_pmcMesh, pmtrlPiece, CTOP_NEWTEX );
//	m_pSavedObliqueSB_2->Apply();
//
//	D3DXMatrixIdentity ( &matScale );
//	pd3dDevice->SetTransform( D3DTS_TEXTURE0, &matScale );
//
////	DxResetTextureMatrix( pd3dDevice, 0 );
}

void DxEffCharToon::RenderBASE( LPDIRECT3DDEVICEQ pd3dDevice, SMATERIAL_PIECE *pmtrlPiece )
{
	//m_pSavedObliqueSB_3->Capture();
	//m_pDrawObliqueSB_3->Apply();
	//m_pSkinMesh->DrawMeshCartoon( pd3dDevice, m_pmcMesh, pmtrlPiece, CTOP_SHADOW );
	//m_pSavedObliqueSB_3->Apply();
}

HRESULT DxEffCharToon::LoadFile_0100 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 이팩트의 Property 를 저장.
	//
	EFFCHAR_PROPERTY_TOON_100 Property;

	SFile.ReadBuffer ( &Property, sizeof(EFFCHAR_PROPERTY_TOON_100) );

	m_Property.Assign( Property );

	return S_OK;
}

HRESULT DxEffCharToon::LoadFile_0101 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{

	//	Note : 이팩트의 Property 를 저장.
	//
	SFile.ReadBuffer ( &m_Property, sizeof(m_Property) );

	return S_OK;
}


HRESULT	DxEffCharToon::LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
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
	hr = Create ( pd3dDevice );
	if ( FAILED(hr) )	return hr;

	return S_OK;
}

HRESULT	DxEffCharToon::SaveFile ( sc::BaseStream &SFile )
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

/*
//	경계선 만들기 - 1픽섹 짜리임.
//		 ( 원본 1번 뿌리고, WireFrame 으로 또 한번 뿌려서 1 Pixel 짜리 얇은 경계선을 만든다. )

HRESULT DxEffCharToon::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{	
	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		//	Note : SetRenderState() 선언
		pd3dDevice->SetRenderState( D3DRS_ANTIALIASEDLINEENABLE, TRUE );
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_DEPTHBIAS,		*((DWORD*)&fBias) );
		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0x00000000 );
		pd3dDevice->SetRenderState( D3DRS_FILLMODE,			D3DFILL_WIREFRAME );

		//	Note : SetTextureStageState() 선언
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,	D3DTA_TFACTOR );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1 );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedSB );
		else			pd3dDevice->EndStateBlock( &m_pEffectSB );
	}
}


HRESULT	DxEffCharToon::SettingState ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !m_pmcMesh )					return S_OK;
	if ( !m_pmcMesh->MeshData.pMesh )	return S_OK;
	if ( !m_pSkinMesh )					return S_OK;

	pd3dDevice->SetRenderState( D3DRS_LASTPIXEL, FALSE );
	m_pSkinMesh->DrawMeshCartoon( pd3dDevice, m_pmcMesh );

	m_pSavedSB->Capture();
	m_pEffectSB->Apply();

	pd3dDevice->SetRenderState( D3DRS_LASTPIXEL, TRUE );
	m_pSkinMesh->DrawMeshCartoon( pd3dDevice, m_pmcMesh );

	m_pSavedSB->Apply();
}
*/

