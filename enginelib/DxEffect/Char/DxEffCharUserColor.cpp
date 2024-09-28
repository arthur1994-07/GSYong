#include "pch.h"
#include "../../../SigmaCore/File/SerialFile.h"
#include "../../../SigmaCore/Util/Util.h"

#include "../../DxTools/Light/DxLightMan.h"
#include "../../DxTools/DxViewPort.h"
#include "../../DxTools/TextureManager.h"

#include "../../DxMeshs/SkinMesh/DxCharPart.h"
#include "../../DxMeshs/SkinMesh/DxSkinPieceContainer.h"

#include "./DxEffCharUserColor.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

extern BOOL			g_bCHAR_EDIT_RUN;

DWORD		DxEffCharUserColor::TYPEID			= EMEFFCHAR_USERCOLOR;
const DWORD	DxEffCharUserColor::VERSION			= 0x0102;
char		DxEffCharUserColor::NAME[MAX_PATH]	= "1.UserColor";

LPDIRECT3DSTATEBLOCK9		DxEffCharUserColor::m_pSavedBaseSB		= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffCharUserColor::m_pEffectBaseSB		= NULL;

DxEffCharUserColor::DxEffCharUserColor(void) :
	DxEffChar(),
	m_pSkinMesh(NULL),
	m_pmcMesh(NULL),
	m_dwTempMaterials(0L),
	m_pMaterials(NULL),
	m_pTempMaterials(NULL)
{
}

DxEffCharUserColor::~DxEffCharUserColor(void)
{
	m_dwMaterials = 0L;
	m_dwTempMaterials = 0L;

	SAFE_DELETE_ARRAY ( m_pMaterials );
	SAFE_DELETE_ARRAY ( m_pTempMaterials );
}

DxEffChar* DxEffCharUserColor::CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece, DWORD dwMaterialNum )
{
	HRESULT hr;
	DxEffCharUserColor *pEffChar = new DxEffCharUserColor;
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

void DxEffCharUserColor::SetMaterials ( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwMaterials, DXMATERIAL_CHAR_EFF* pMaterials )
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

        m_pMaterials[i].LoadTexture( EMMM_CHAR_MIPMAP, true );
	}
}

HRESULT DxEffCharUserColor::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

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

		//	Note : SetTextureStageState() 선언
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,	D3DTA_TFACTOR );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );

		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLORARG2,	D3DTA_CURRENT );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,	D3DTOP_MODULATE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG1 );

		pd3dDevice->SetTextureStageState ( 1, D3DTSS_ALPHAARG2,	D3DTA_CURRENT );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG2 );


		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedBaseSB );
		else			pd3dDevice->EndStateBlock( &m_pEffectBaseSB );
	}

	return S_OK;
}

HRESULT DxEffCharUserColor::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSavedBaseSB );
	SAFE_RELEASE( m_pEffectBaseSB );

	return S_OK;
}

HRESULT DxEffCharUserColor::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr(S_OK);

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

HRESULT DxEffCharUserColor::DeleteDeviceObjects ()
{
	SAFE_DELETE_ARRAY( m_pMaterials );

	return S_OK;
}

HRESULT DxEffCharUserColor::FrameMove ( float fTime, float fElapsedTime )
{
	return S_OK;
}

void DxEffCharUserColor::Render( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* arrayBoneMatrices, BOOL bPieceRender )
{
	// DXMATERIAL_CHAR_EFF 확인 및 생성작업.
	if ( !IsExist_Copy_pmcMesh() )
		return;

	if( !m_pSkinMesh )						return;

	for ( DWORD i=0; i<m_dwMaterials; i++ )
	{
		if ( m_pMaterials[i].IsType( EMCET_BASE ) )
		{
			if( g_bCHAR_EDIT_RUN )	
			{
				float fData;
				m_pMaterials[i].d3dMaterial.Diffuse.a = 1.f;

				fData = m_cColor.r;
				fData = (fData>1.f) ? 1.f : fData;
				m_pMaterials[i].d3dMaterial.Diffuse.r = fData;

				fData = m_cColor.g;
				fData = (fData>1.f) ? 1.f : fData;
				m_pMaterials[i].d3dMaterial.Diffuse.g = fData;

				fData = m_cColor.b;
				fData = (fData>1.f) ? 1.f : fData;
				m_pMaterials[i].d3dMaterial.Diffuse.b = fData;
			}
			else
			{
				DWORD dwColor = m_pCharPart->GetHairColor();
                if ( dwColor == 0xff000000 )
                {
                    float fData;
                    m_pMaterials[i].d3dMaterial.Diffuse.a = 1.f;

                    fData = m_cColor.r;
                    fData = (fData>1.f) ? 1.f : fData;
                    m_pMaterials[i].d3dMaterial.Diffuse.r = fData;

                    fData = m_cColor.g;
                    fData = (fData>1.f) ? 1.f : fData;
                    m_pMaterials[i].d3dMaterial.Diffuse.g = fData;

                    fData = m_cColor.b;
                    fData = (fData>1.f) ? 1.f : fData;
                    m_pMaterials[i].d3dMaterial.Diffuse.b = fData;
                }
                else
                {
                    m_pMaterials[i].d3dMaterial.Diffuse.a = 1.f;
                    m_pMaterials[i].d3dMaterial.Diffuse.r = ((float)((dwColor & 0xff0000)>>16)) / 255.f;
                    m_pMaterials[i].d3dMaterial.Diffuse.g = ((float)((dwColor & 0xff00)>>8)) / 255.f;
                    m_pMaterials[i].d3dMaterial.Diffuse.b = ((float)(dwColor & 0xff)) / 255.f;
                }
			}
		}
		else
		{
			m_pMaterials[i].d3dMaterial.Diffuse.a = 1.f;
			m_pMaterials[i].d3dMaterial.Diffuse.r = 1.f;
			m_pMaterials[i].d3dMaterial.Diffuse.g = 1.f;
			m_pMaterials[i].d3dMaterial.Diffuse.b = 1.f;
		}
	}


	m_pSavedBaseSB->Capture();
	m_pEffectBaseSB->Apply();

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

	CHARSETTING sCharSetting;
	sCharSetting.pMeshContainerBase = m_pmcMesh;
	sCharSetting.pmtrlPiece			= pMaterialPiece;
	sCharSetting.pmtrlSpecular		= m_pMaterials;
	sCharSetting.bWorldIdentity		= bPieceRender;
	sCharSetting.emRDOP				= CTOP_BASE;

	m_pSkinMesh->SetDrawState( FALSE, FALSE, FALSE, FALSE, TRUE );
	m_pSkinMesh->DrawMeshContainer( pd3dDevice, sCharSetting, m_rSkeleton, *pmatStaticLinkBone, arrayBoneMatrices );

	pd3dDevice->SetTexture( 1, NULL );

	m_pSavedBaseSB->Apply();
}

HRESULT DxEffCharUserColor::LoadFile_0100 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 이팩트의 Property 를 저장.
	EFFCHAR_PROPERTY_USERCOLOR_100 sProp;
	SFile.ReadBuffer ( &sProp, sizeof(EFFCHAR_PROPERTY_USERCOLOR_100) );

	m_cColor = sProp.m_cColor;
	m_dwFlag = sProp.m_dwFlag;

	return S_OK;
}

HRESULT DxEffCharUserColor::LoadFile_0101 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 이팩트의 Property 를 저장.
	//
	EFFCHAR_PROPERTY_USERCOLOR_101 Property;

	SFile.ReadBuffer ( &Property, sizeof(EFFCHAR_PROPERTY_USERCOLOR_101) );

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

HRESULT DxEffCharUserColor::LoadFile_0102 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
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

HRESULT	DxEffCharUserColor::LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	SFile >> dwSize;

	switch ( dwVer )
	{
	case 0x0100:	LoadFile_0100( SFile, pd3dDevice );	break;
	case 0x0101:	LoadFile_0101( SFile, pd3dDevice );	break;
	case VERSION:	LoadFile_0102( SFile, pd3dDevice );	break;
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

HRESULT	DxEffCharUserColor::SaveFile ( sc::BaseStream &SFile )
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

// Color 를 MaterialSystem 에서 사용하기 위한 작업.
void DxEffCharUserColor::LoadUserColorMaskTexture( int nMaterial )
{
	m_pMaterials[ nMaterial ].LoadTexture( EMMM_CHAR_MIPMAP, true );
}

LPDIRECT3DTEXTUREQ DxEffCharUserColor::GetUserColorMaskTexture( int nMaterial )
{
	if ( m_pMaterials[ nMaterial ].IsType( EMCET_BASE ) )
	{
		return m_pMaterials[ nMaterial ].GetTexture();
	}
	
	return NULL;
}

void DxEffCharUserColor::GetUserColor( int nMaterial, D3DXVECTOR4& sColor )
{
	if ( m_pMaterials[ nMaterial ].IsType( EMCET_BASE ) )
	{
		if( g_bCHAR_EDIT_RUN )	
		{
			float fData;

			fData = m_cColor.r;
			fData = (fData>1.f) ? 1.f : fData;
			sColor.x = fData;

			fData = m_cColor.g;
			fData = (fData>1.f) ? 1.f : fData;
			sColor.y = fData;

			fData = m_cColor.b;
			fData = (fData>1.f) ? 1.f : fData;
			sColor.z = fData;

			sColor.w = 1.f;
		}
		else
		{
			DWORD dwColor = m_pCharPart->GetHairColor();
			if ( dwColor == 0xff000000 )
			{
				float fData;
				m_pMaterials[ nMaterial ].d3dMaterial.Diffuse.a = 1.f;

				fData = m_cColor.r;
				fData = (fData>1.f) ? 1.f : fData;
				sColor.x = fData;

				fData = m_cColor.g;
				fData = (fData>1.f) ? 1.f : fData;
				sColor.y = fData;

				fData = m_cColor.b;
				fData = (fData>1.f) ? 1.f : fData;
				sColor.z = fData;

				sColor.w = 1.f;
			}
			else
			{
				sColor.x = ((float)((dwColor & 0xff0000)>>16)) / 255.f;
				sColor.y = ((float)((dwColor & 0xff00)>>8)) / 255.f;
				sColor.z = ((float)(dwColor & 0xff)) / 255.f;
				sColor.w = 1.f;
			}
		}
	}
}

// Effect Texture 들이 다 로딩이 되었는가~?
BOOL DxEffCharUserColor::IsEnableEffTexture( BOOL bTexThread )
{
	BOOL bTextureLoadingOK(TRUE);
	for ( DWORD i=0; i<m_dwMaterials; ++i )
	{
		if ( m_pMaterials[ i ].IsType( EMCET_BASE ) )
		{
			if ( !m_pMaterials[ i ].GetTexture() )
			{
				// 파일이 있다면 Loading 아직 못했다고 해준다.
				if ( m_pMaterials[ i ].IsPathFile() )
					bTextureLoadingOK = FALSE;
			}
		}
	}

	return bTextureLoadingOK;
}

BOOL DxEffCharUserColor::IsExist_Copy_pmcMesh()
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
			if( !m_pmcMesh->pMaterials[i].pTextureFilename )
				continue;

			std::string	strName;
			strName = sc::util::GetSpecularName ( m_pmcMesh->pMaterials[i].pTextureFilename, "_m" );

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
			if( !m_pmcMesh->pMaterials[i].pTextureFilename )
				continue;

			std::string	strName;
			strName = sc::util::GetSpecularName ( m_pmcMesh->pMaterials[i].pTextureFilename, "_m" );

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