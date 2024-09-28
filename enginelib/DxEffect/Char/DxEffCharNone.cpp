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
#include "../../DxTools/DxEnvironment.h"
#include "../../DxTools/DxSurfaceTex.h"

#include "../../DxEffect/Frame/DxEffectMan.h"
#include "../../DxEffect/Single/DxEffSingle.h"
#include "../Single/DxEffSinglePropGMan.h"

#include "../../DxMeshs/SkinMesh/DxCharPart.h"
#include "../../DxMeshs/SkinMesh/DxSkinPieceContainer.h"

#include "./DxEffCharHLSL.h"
#include "../../DxMeshs/SkinMesh/_new/DxSkinMesh9_HLSL.h"

#include "./DxEffCharNone.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

extern BOOL			g_bCHAR_EDIT_RUN;
extern int			g_nITEMLEVEL;

DWORD		DxEffCharNone::TYPEID			= EMEFFCHAR_NULL;
const DWORD	DxEffCharNone::VERSION			= 0x0101;
char		DxEffCharNone::NAME[MAX_PATH]	= "1.Disable Render";

LPDIRECT3DSTATEBLOCK9		DxEffCharNone::m_pSavedStateBlock	= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffCharNone::m_pEffectStateBlock	= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffCharNone::m_pSavedCubeSB		= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffCharNone::m_pDrawCubeSB		= NULL;

DxEffCharNone::DxEffCharNone(void) :
	DxEffChar(),
	m_pSkinMesh(NULL),
	m_pmcMesh(NULL),
	m_dwTempMaterials(0L),
	m_pMaterials(NULL),
	m_pTempMaterials(NULL),
	m_dwMipMapCount(0)
{
}

DxEffCharNone::~DxEffCharNone(void)
{
	m_dwMaterials = 0L;
	m_dwTempMaterials = 0L;

	SAFE_DELETE_ARRAY ( m_pMaterials );
	SAFE_DELETE_ARRAY ( m_pTempMaterials );
}

DxEffChar* DxEffCharNone::CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece, DWORD dwMaterialNum )
{
	HRESULT hr;
	DxEffCharNone *pEffChar = new DxEffCharNone;
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

void DxEffCharNone::SetMaterials ( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwMaterials, DXMATERIAL_CHAR_EFF* pMaterials )
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

HRESULT DxEffCharNone::StaticResetDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	D3DCAPSQ d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		//	Note : SetRenderState() 선언
		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,			FALSE );

		//	Note : SetTextureStageState() 선언
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1 );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_DISABLE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_TEXCOORDINDEX,			D3DTSS_TCI_CAMERASPACEPOSITION );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_PROJECTED | D3DTTFF_COUNT3 );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedStateBlock );
		else			pd3dDevice->EndStateBlock( &m_pEffectStateBlock );
	}

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

HRESULT DxEffCharNone::StaticLostDevice()
{
	HRESULT hr(S_OK);
	SAFE_RELEASE( m_pSavedStateBlock );
	SAFE_RELEASE( m_pEffectStateBlock );
	SAFE_RELEASE( m_pSavedCubeSB );
	SAFE_RELEASE( m_pDrawCubeSB );

	return S_OK;
}

HRESULT DxEffCharNone::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;

	if ( m_pCharPart )
	{
		m_pSkinMesh = m_pCharPart->m_rSkinMesh;
		m_pCharPart->SetBaseRenderOff();
	}
	else if ( m_pSkinPiece )
	{
		m_pSkinMesh = m_pSkinPiece->m_skinMeshRes.get();
		m_pSkinPiece->SetBaseRenderOff();
	}

	// DXMATERIAL_CHAR_EFF 확인 및 생성작업.
	IsExist_Copy_pmcMesh();

	return S_OK;
}

HRESULT DxEffCharNone::DeleteDeviceObjects ()
{
	SAFE_DELETE_ARRAY ( m_pMaterials );

	return S_OK;
}

HRESULT DxEffCharNone::FrameMove ( float fTime, float fElapsedTime )
{
	HRESULT hr = S_OK;

	return S_OK;
}

void	DxEffCharNone::Render( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* arrayBoneMatrices, BOOL bPieceRender )
{
	HRESULT hr = S_OK;

	// DXMATERIAL_CHAR_EFF 확인 및 생성작업.
	if ( !IsExist_Copy_pmcMesh() )
		return;

	if ( !m_pSkinMesh )	
		return;

	//Render_Refract( pd3dDevice, bPieceRender );
	//Render_Cube( pd3dDevice, bPieceRender );
}
//
//void DxEffCharNone::Render_Refract( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* arrayBoneMatrices, BOOL bPieceRender )
//{
//	if ( !DxSurfaceTex::GetInstance().IsEnable() )
//		return;
//
//	D3DXMATRIX matIdentity;
//	D3DXMatrixIdentity( &matIdentity );
//	const D3DXMATRIX* pmatStaticLinkBone(NULL);
//
//	SMATERIAL_PIECE*	pMaterialPiece = NULL;
//	if ( m_pCharPart )	
//	{
//		pmatStaticLinkBone = &m_pCharPart->GetStaticLinkBone();
//		pMaterialPiece = m_pCharPart->GetSkinPiece()->m_arrayMaterialPiece;
//	}
//	else if ( m_pSkinPiece )
//	{
//		pmatStaticLinkBone = &matIdentity;
//		pMaterialPiece = m_pSkinPiece->m_arrayMaterialPiece;
//	}
//	
//	// 활성화
//	DxEnvironment::GetInstance().SetRefractPlayON();
//
//	// Note : Get Texture
//	for ( DWORD i=0; i<m_dwMaterials; i++ )
//	{
//		m_pMaterials[i].SetTexture( DxSurfaceTex::GetInstance().m_pWaveTex );
//	}
//
//	// TextureMatrix 를 만들어야 한다.
//	{
//		//set special texture matrix for shadow mapping
//		unsigned int range;
//		float fOffsetX = 0.5f + (0.5f / (float)DxViewPort::GetInstance().GetWidth());
//		float fOffsetY = 0.5f + (0.5f / (float)DxViewPort::GetInstance().GetHeight());
//
//		range = 0xFFFFFFFF >> (32 - 16);
//
//		float fBias    = -0.001f * (float)range;
//		D3DXMATRIX texScaleBiasMat( 0.5f,     0.0f,     0.0f,         0.0f,
//									0.0f,     -0.5f,     0.0f,         0.0f,
//									0.0f,     0.0f,     (float)range, 0.0f,
//									fOffsetX, fOffsetY, fBias,        1.0f );
//
//		//D3DXMATRIX matTexScale;
//
//		//// Create texture scaling matrix:
//		//// | 0.5      0        0        0 |
//		//// | 0        -0.5     0        0 |
//		//// | 0        0        0        0 |
//		//// | 0.5      0.5      1        1 |
//
//		//// This will scale and offset -1 to 1 range of x, y
//		//// coords output by projection matrix to 0-1 texture
//		//// coord range.
//		//SecureZeroMemory( &matTexScale, sizeof( D3DMATRIX ) );
//		//matTexScale._11 = 0.5f;
//		//matTexScale._22 = -0.5f;
//		//matTexScale._33 = 0.0f; 
//		//matTexScale._41 = 0.5f; 
//		//matTexScale._42 = 0.5f;
//		//matTexScale._43 = 1.0f; 
//		//matTexScale._44 = 1.0f;
//
//		D3DXMATRIX	matTextureMatrix;
//		const D3DXMATRIX& matProj = DxViewPort::GetInstance().GetMatProj();
//		//D3DXMatrixMultiply( &matTextureMatrix, &matProj, &matTexScale );
//		D3DXMatrixMultiply( &matTextureMatrix, &matProj, &texScaleBiasMat );
//
//		pd3dDevice->SetTransform( D3DTS_TEXTURE0, &matTextureMatrix );
//	}
//
//	m_pSavedStateBlock->Capture();
//	m_pEffectStateBlock->Apply();
//
//	CHARSETTING sCharSetting;
//	sCharSetting.pMeshContainerBase = m_pmcMesh;
//	sCharSetting.pmtrlPiece			= pMaterialPiece;
//	sCharSetting.pmtrlSpecular		= m_pMaterials;
//	sCharSetting.bWorldIdentity		= bPieceRender;
//	sCharSetting.emRDOP				= CTOP_BASE;
//
//	m_pSkinMesh->SetDrawState( FALSE, FALSE, FALSE, FALSE );
//	m_pSkinMesh->DrawMeshContainer( pd3dDevice, sCharSetting, m_rSkeleton, *pmatStaticLinkBone, arrayBoneMatrices );
//
//	m_pSavedStateBlock->Apply();
//
//	// 변경한 Texture Matrix 를 되돌린다.
//	{
//		DxResetTextureMatrix ( pd3dDevice, 0 );
//	}
//}
//
//void DxEffCharNone::Render_Cube( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* arrayBoneMatrices, BOOL bPieceRender )
//{
//	D3DXMATRIX matIdentity;
//	D3DXMatrixIdentity( &matIdentity );
//	const D3DXMATRIX* pmatStaticLinkBone(NULL);
//
//	SMATERIAL_PIECE*	pMaterialPiece = NULL;
//	if ( m_pCharPart )	
//	{
//		pmatStaticLinkBone = &m_pCharPart->GetStaticLinkBone();
//		pMaterialPiece = m_pCharPart->GetSkinPiece()->m_arrayMaterialPiece;
//	}
//	else if ( m_pSkinPiece )
//	{
//		pmatStaticLinkBone = &matIdentity;
//		pMaterialPiece = m_pSkinPiece->m_arrayMaterialPiece;
//	}
//
//	// Note : Get Texture
//	for ( DWORD i=0; i<m_dwMaterials; i++ )
//	{
//		m_pMaterials[i].UpdateTexture();
//	}
//
//	pd3dDevice->SetTexture( 1, DxCubeMap::GetInstance().GetCubeTexTEST() );
//
//	m_pSavedCubeSB->Capture();
//	m_pDrawCubeSB->Apply();
//
//	CHARSETTING sCharSetting;
//	sCharSetting.pMeshContainerBase = m_pmcMesh;
//	sCharSetting.pmtrlPiece			= pMaterialPiece;
//	sCharSetting.pmtrlSpecular		= m_pMaterials;
//	sCharSetting.bWorldIdentity		= bPieceRender;
//	sCharSetting.emRDOP				= CTOP_BASE;
//
//	m_pSkinMesh->SetDrawState( FALSE, FALSE, FALSE, FALSE );
//	m_pSkinMesh->DrawMeshContainer( pd3dDevice, sCharSetting, m_rSkeleton, *pmatStaticLinkBone, arrayBoneMatrices );
//
//	m_pSavedCubeSB->Apply();
//
//	pd3dDevice->SetTexture( 1, NULL );
//}

HRESULT DxEffCharNone::LoadFile_0100 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	// 사이즈를 2번 저장했었다.
	DWORD dwSize;
	SFile >> dwSize;

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

HRESULT DxEffCharNone::LoadFile_0101 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
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

HRESULT	DxEffCharNone::LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	SFile >> dwSize;

	switch ( dwVer )
	{
	case VERSION:	LoadFile_0101( SFile, pd3dDevice );	break;
	case 0x0100:	LoadFile_0100( SFile, pd3dDevice );	break;
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

HRESULT	DxEffCharNone::SaveFile ( sc::BaseStream &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	SFile.BeginBlock();
	{
		//	Note : 이팩트의 Property 를 저장.
		//
		SFile.WriteBuffer ( &m_Property, sizeof(m_Property) );

		SFile << m_dwMaterials;
		for( DWORD i=0; i<m_dwMaterials; ++i )
			m_pMaterials[ i ].SaveFile( SFile );
	}
	SFile.EndBlock();

	return S_OK;
}

BOOL DxEffCharNone::IsExist_Copy_pmcMesh()
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

	for ( DWORD i=0; i<m_dwMaterials; ++i )
	{
		m_pMaterials[i].SetType( EMCET_BASE );
	}

	return TRUE;
}