#include "pch.h"
#include "../../DxEffect/Single/DxEffSingle.h"
#include "../Single/DxEffSinglePropGMan.h"
#include "../../../SigmaCore/File/SerialFile.h"

#include "../../DxTools/DxViewPort.h"
#include "../../DxEffect/Frame/DxEffectMan.h"

#include "../../DxMeshs/SkinMesh/DxCharPart.h"
#include "../../DxMeshs/SkinMesh/DxSkinPieceContainer.h"

#include "./DxEffCharDust.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

DWORD		DxEffCharDust::TYPEID			= EMEFFCHAR_DUST;
const DWORD	DxEffCharDust::VERSION			= 0x0101;
char		DxEffCharDust::NAME[MAX_PATH]	= "1.먼지,때";

LPDIRECT3DSTATEBLOCK9		DxEffCharDust::m_pSavedBaseSB		= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffCharDust::m_pEffectBaseSB		= NULL;

DxEffCharDust::DxEffCharDust(void) :
	DxEffChar(),
	m_pSkinMesh(NULL),
	m_pmcMesh(NULL),
	m_pMaterials(NULL),
	m_dwMaterials(0L)
{
}

DxEffCharDust::~DxEffCharDust(void)
{
}

DxEffChar* DxEffCharDust::CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece, DWORD dwMaterialNum )
{
	HRESULT hr;
	DxEffCharDust *pEffChar = new DxEffCharDust;
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

HRESULT DxEffCharDust::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{	
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
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );

		//	Note : SetTextureStageState() 선언
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG1 );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedBaseSB );
		else			pd3dDevice->EndStateBlock( &m_pEffectBaseSB );
	}

	return S_OK;
}

HRESULT DxEffCharDust::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSavedBaseSB );
	SAFE_RELEASE( m_pEffectBaseSB );

	return S_OK;
}

HRESULT DxEffCharDust::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
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

	// m_pmcMesh 셋팅.
	IsExist_Copy_pmcMesh();

	return S_OK;
}

HRESULT DxEffCharDust::DeleteDeviceObjects ()
{
	SAFE_DELETE_ARRAY ( m_pMaterials );
	m_dwMaterials = 0L;

    m_textureRes.Clear();

	return S_OK;
}

HRESULT DxEffCharDust::FrameMove ( float fTime, float fElapsedTime )
{
	HRESULT hr = S_OK;

	return S_OK;
}

void	DxEffCharDust::Render ( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* arrayBoneMatrices, BOOL bPieceRender )
{
	// m_pmcMesh
	if ( !IsExist_Copy_pmcMesh() )
		return;

	if ( !m_pSkinMesh )		
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

	if ( DxEffectMan::GetInstance().GetDetailFlag() & REALSPECULAR ) 
	{
		if( m_textureRes.Update() )
		{
            LPDIRECT3DTEXTUREQ pTexture = m_textureRes.GetTexture();
            if( pTexture )
            {
			    for ( DWORD i=0; i<m_dwMaterials; ++i )
				    m_pMaterials[i].SetTexture( pTexture );
            }
		}

		m_pSavedBaseSB->Capture();
		m_pEffectBaseSB->Apply();

		CHARSETTING sCharSetting;
		sCharSetting.pMeshContainerBase = m_pmcMesh;
		sCharSetting.pmtrlPiece			= pMaterialPiece;
		sCharSetting.pmtrlSpecular		= m_pMaterials;
		sCharSetting.bWorldIdentity		= bPieceRender;
		sCharSetting.emRDOP				= CTOP_BASE;

		m_pSkinMesh->SetDrawState ( FALSE, FALSE, FALSE, FALSE );
		m_pSkinMesh->DrawMeshContainer ( pd3dDevice, sCharSetting, m_rSkeleton, *pmatStaticLinkBone, arrayBoneMatrices );

		m_pSavedBaseSB->Apply();
	}
}

HRESULT DxEffCharDust::LoadFile_0100 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 이팩트의 Property 를 저장.
	//

	EFFCHAR_PROPERTY_DUST_100 pProperty;

	SFile.ReadBuffer ( &pProperty, sizeof(EFFCHAR_PROPERTY_DUST_100) );

	m_Property.Assign( pProperty );

	return S_OK;
}

HRESULT DxEffCharDust::LoadFile_0101 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 이팩트의 Property 를 저장.
	//
	SFile.ReadBuffer ( &m_Property, sizeof(m_Property) );

	return S_OK;
}

HRESULT	DxEffCharDust::LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	SFile >> dwSize;


	//	데이터 포맷 변경 진행	

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

HRESULT	DxEffCharDust::SaveFile ( sc::BaseStream &SFile )
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

BOOL DxEffCharDust::IsExist_Copy_pmcMesh()
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


	SAFE_DELETE_ARRAY ( m_pMaterials );
	m_dwMaterials = m_pmcMesh->GetNumMaterials();
	m_pMaterials = new DXMATERIAL_CHAR_EFF[m_dwMaterials];

	for ( DWORD i=0; i<m_dwMaterials; ++i )
	{
		m_pMaterials[i].SetType( EMCET_BASE );

		LPDIRECT3DTEXTUREQ pTexture = m_textureRes.GetTexture();
		if( pTexture )
			m_pMaterials[i].SetTexture( pTexture );
	}

	return TRUE;
}