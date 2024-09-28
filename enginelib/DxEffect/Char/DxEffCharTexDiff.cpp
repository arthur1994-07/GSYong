//
//	DXMATERIAL_CHAR_EFF �� ��ȭ�ϸ鼭 0x105 �������� ��ȭ �Ͽ���.
//
#include "pch.h"
#include "../../../SigmaCore/Math/Random.h"
#include "../../../SigmaCore/File/SerialFile.h"
#include "../../../SigmaCore/Util/Util.h"

#include "../Single/DxEffSinglePropGMan.h"
#include "../../DxEffect/Single/DxEffSingle.h"
#include "../../DxTools/DxInputDevice.h"
#include "../../DxTools/DxCubeMap.h"
#include "../../DxTools/DxGlowMan.h"
#include "../../DxTools/DxSurfaceTex.h"
#include "../../DxTools/DxShadowMap.h"
#include "../../DxTools/DxViewPort.h"
#include "../../DxEffect/Frame/DxEffectMan.h"
#include "../../DxMeshs/SkinMesh/DxCharPart.h"
#include "../../DxMeshs/SkinMesh/DxSkinPieceContainer.h"

#include "./DxEffCharTexDiff.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

DWORD		DxEffCharTexDiff::TYPEID			= EMEFFCHAR_TEXDIFF;
const DWORD	DxEffCharTexDiff::VERSION			= 0x0102;
char		DxEffCharTexDiff::NAME[MAX_PATH]	= "2.Tex Diff Up , Down";

LPDIRECT3DSTATEBLOCK9		DxEffCharTexDiff::m_pSavedSB	= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffCharTexDiff::m_pDrawSB		= NULL;

DxEffCharTexDiff::DxEffCharTexDiff(void) :
	DxEffChar(),
	m_pSkinMesh(NULL),
	m_pmcMesh(NULL),
	m_dwTempMaterials(0L),		// �⺻���� �͵�..
	m_pMaterials(NULL),
	m_pTempMaterials(NULL),
	m_bUp(TRUE),
	m_fTime(-D3DX_PI),
	m_fRandSpeed(1.f),
	m_dwColor(0L),
	m_fColor(0.f)
{
}

DxEffCharTexDiff::~DxEffCharTexDiff(void)
{
	m_dwMaterials = 0L;
	m_dwTempMaterials = 0L;

	SAFE_DELETE_ARRAY ( m_pMaterials );
	SAFE_DELETE_ARRAY ( m_pTempMaterials );
}

DxEffChar* DxEffCharTexDiff::CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece, DWORD dwMaterialNum )
{
	HRESULT hr;
	DxEffCharTexDiff *pEffChar = new DxEffCharTexDiff;
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

void DxEffCharTexDiff::SetMaterials ( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwMaterials, DXMATERIAL_CHAR_EFF* pMaterials )
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

HRESULT DxEffCharTexDiff::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{	
	D3DCAPSQ d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );

	//// Anisotropic Filter Ȯ��
	//DWORD dwMinFilter;
	//if( d3dCaps.TextureFilterCaps & D3DPTFILTERCAPS_MINFANISOTROPIC )	dwMinFilter = D3DTEXF_ANISOTROPIC;
	//else if( d3dCaps.TextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR )	dwMinFilter = D3DTEXF_LINEAR;
	//else																dwMinFilter = D3DTEXF_POINT;

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		//if( dwMinFilter == D3DTEXF_ANISOTROPIC )
		//{
		//	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, dwMinFilter );
		//	pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
		//}

		// Note : SetRenderState()	 ���ص� �������.
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_ONE );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_ONE );
		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xffffffff );

		// Note : SetRenderState()
		float fBias = -0.0001f;
		pd3dDevice->SetRenderState( D3DRS_DEPTHBIAS,		*((DWORD*)&fBias) );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,			0x00000000 );

		// Note : SetTextureStageState()
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,	D3DTA_TFACTOR );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedSB );
		else			pd3dDevice->EndStateBlock( &m_pDrawSB );
	}

	return S_OK;
}

HRESULT DxEffCharTexDiff::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSavedSB );
	SAFE_RELEASE( m_pDrawSB );

	return S_OK;
}

HRESULT DxEffCharTexDiff::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
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

    //m_textureRes = TextureManager::GetInstance().LoadTexture(
    //    m_strTex,
    //    false,
    //    TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
    //    EMMM_CHAR_MIPMAP,
    //    D3DFMT_UNKNOWN,
    //    TEXTURE_RESIZE_NONE,
    //    TEXTURE_CHAR,
    //    true );

	// DXMATERIAL_CHAR_EFF Ȯ�� �� �����۾�.
	IsExist_Copy_pmcMesh();

	

	return S_OK;
}

HRESULT DxEffCharTexDiff::DeleteDeviceObjects ()
{
	//m_textureRes.Clear();

	SAFE_DELETE_ARRAY ( m_pMaterials );

	return S_OK;
}

HRESULT DxEffCharTexDiff::FrameMove ( const float fTime, const float fElapsedTime )
{
	HRESULT hr = S_OK;

	m_fTime += m_fSpeed * m_fRandSpeed * fElapsedTime;

	if( m_dwFlag&NO_UPDOWN )
	{
		// Note : Random Speed Setting
		if( m_fTime > 0.f && m_bUp )		
		{
			m_bUp = FALSE;
			m_fRandSpeed = (sc::Random::RANDOM_POS() * 5.f + 1.f);
		}
		else if( m_fTime>D3DX_PI && !m_bUp )
		{
			m_fTime -= D3DX_PI*2.f;	// ���� ���ش�.
			m_bUp = TRUE;
			m_fRandSpeed = (sc::Random::RANDOM_POS() * 5.f + 1.f);
		}

		// Note : ������ ������
		m_fColor = cosf( m_fTime );
		m_fColor += 1.f;
		m_fColor *= 0.5f;
	}
	else
	{
		m_fColor = 1.f;
	}

	//// Note : Color ����
	//m_dwColor = (DWORD)(fSpeed*255.f);
	//m_dwColor = 0xff000000 + (m_dwColor<<16) + (m_dwColor<<8) + m_dwColor;

	return S_OK;
}

void	DxEffCharTexDiff::Render ( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* arrayBoneMatrices, BOOL bPieceRender )
{
	HRESULT hr = S_OK;

	// DXMATERIAL_CHAR_EFF Ȯ�� �� �����۾�.
	if ( !IsExist_Copy_pmcMesh() )
		return;

	if ( !m_pSkinMesh )		return;

	//if ( !m_textureRes.Update() )
	//	return;

	// Note : Get Texture
	for ( DWORD i=0; i<m_dwMaterials; i++ )
	{
		if( m_pMaterials[i].GetTexture() )
		{
			m_pMaterials[i].d3dMaterial.Diffuse.r = m_fColor * m_pMaterials[i].d3dMaterial.Power;
			m_pMaterials[i].d3dMaterial.Diffuse.g = m_fColor * m_pMaterials[i].d3dMaterial.Power;
			m_pMaterials[i].d3dMaterial.Diffuse.b = m_fColor * m_pMaterials[i].d3dMaterial.Power;
        }
	}

	m_pSavedSB->Capture();
	m_pDrawSB->Apply();

	switch( m_dwIntensity )
	{
	case 0:
		//pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );	// ���� �ʿ䰡 ����. �⺻���̱� ������.. 
		break;
	case 1:
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE2X );
		break;
	case 2:
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE4X );
		break;
	};

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

	m_pSavedSB->Apply();
}

HRESULT DxEffCharTexDiff::LoadFile_0100 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : ����Ʈ�� Property �� ����.
	EFFCHAR_PROPERTY_TEXDIFF_100 sProp;
	SFile.ReadBuffer ( &sProp, sizeof(EFFCHAR_PROPERTY_TEXDIFF_100) );

	m_Property.m_dwFlag		= sProp.m_dwFlag;
	m_Property.m_dwMaterials = sProp.m_dwMaterials;
	m_Property.m_fSpeed		= sProp.m_fSpeed;
//	m_Property.m_strTex		= sProp.m_strTex.c_str();
	strncpy(m_Property.m_strTex,sProp.m_strTex,sizeof(m_Property.m_strTex));


	m_Property.m_dwIntensity	= 0;	// Ver.101 ���� �߰�
	m_Property.m_dwFlag			|= NO_UPDOWN;

	SFile >> m_dwMaterials;
	if ( m_dwMaterials )
	{
		SAFE_DELETE_ARRAY ( m_pMaterials );
		m_pMaterials = new DXMATERIAL_CHAR_EFF[m_dwMaterials];

		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
            m_pMaterials[i].LoadFile( SFile );
			m_pMaterials[i].LoadTexture( EMMM_CHAR_MIPMAP, true );

			m_pMaterials[i].d3dMaterial.Power = 1.f;	// Ver.101 ���� �߰�
		}
	}

	return S_OK;
}

HRESULT DxEffCharTexDiff::LoadFile_0101 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : ����Ʈ�� Property �� ����.
	//
	EFFCHAR_PROPERTY_TEXDIFF_101 Property;

	SFile.ReadBuffer ( &Property, sizeof(EFFCHAR_PROPERTY_TEXDIFF_101) );

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

HRESULT DxEffCharTexDiff::LoadFile_0102 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{

	//	Note : ����Ʈ�� Property �� ����.
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

HRESULT	DxEffCharTexDiff::LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
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

HRESULT	DxEffCharTexDiff::SaveFile ( sc::BaseStream &SFile )
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

// PrecalculateForMaterialSystem�� 2�� �� ���� �̸� ����� ���°Ŷ��, �̰����� ������ �ȴ�.
void DxEffCharTexDiff::PrecalculateForMaterialSystem( LPDIRECT3DDEVICEQ pd3dDevice, 
													  EffectMaterialData* pEffMaterialData, 
													  const D3DXMATRIX* pMatrix, 
													  const float fScale )
{
	// ������ ��������̸� ���� �Ұ�.
	if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_PS_1_1 )
		return;

	float fTexUpDownPower(1.f);
	switch( m_dwIntensity )
	{
	case 0:
		fTexUpDownPower = 1.f;
		break;
	case 1:
		fTexUpDownPower = 2.f;
		break;
	case 2:
		fTexUpDownPower = 4.f;
		break;
	};

	pEffMaterialData->m_bTexDiffColor = TRUE;

	if ( pEffMaterialData->m_vecTexUpDownTexture.size() != m_dwMaterials )
	{
		pEffMaterialData->m_vecTexDiffColor.clear();
		pEffMaterialData->m_vecTexUpDownTexture.clear();

		for ( DWORD i=0; i<m_dwMaterials; ++i )
		{
			pEffMaterialData->m_vecTexDiffColor.push_back( m_fColor*fTexUpDownPower );
			pEffMaterialData->m_vecTexUpDownTexture.push_back( m_pMaterials[i].IsType( EMCET_BASE ) ? m_pMaterials[i].GetTexture() : NULL );
		}
	}
	else
	{
		// [shhan][2015.04.23]
		// ������ �븸���� m_vecTexUpDownTexture �� ����ִ� �ؽ��İ��� ���� �Ȱ� ����־���.
		// ��Ȳ�� �̷���.
		// m_make_bow_01_dark.cps ������ ó�� ���� �� m_dwMaterials �� 2�� ����.
		// ���� �޽��� m_dwMaterials 1���� �����Ͽ���.
		// �׷��鼭 EMCET_NULL ������ ����Ǿ���. (�ϴ� DxEffCharTexDiff ȿ���� ������ EMCET_NULL �� ������ �߻��� �� �־���.)
		// ������ ������ ����ϴ� Part ������ DxEffCharTexDiff �� ����Ͽ���, m_dwMaterials ������ �����Ͽ���. �������� 1��.
		// EMCET_NULL ������ ���� m_vecTexUpDownTexture[i] ���� ������ ���ϰ�, �������� �� �ִ� ���¿��� ����� �ϴ� ������ �߻��Ͽ���.
		//

		// �������� DxEffCharTexDiff �� ���õǾ� ���� ���� �־ �� �۾��� �߰���.
		for ( DWORD i=0; i<m_dwMaterials; ++i )
		{
			if ( m_pMaterials[i].IsType( EMCET_BASE ) )
			{
				pEffMaterialData->m_vecTexDiffColor[i] = m_fColor*fTexUpDownPower;
				pEffMaterialData->m_vecTexUpDownTexture[i] = m_pMaterials[i].GetTexture();
			}
			else
			{
				pEffMaterialData->m_vecTexUpDownTexture[i] = NULL;
			}
		}
	}
}

BOOL DxEffCharTexDiff::IsExist_Copy_pmcMesh()
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

			m_pMaterials[i].LoadTexture( 0, true );
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
			if( !m_pmcMesh->pMaterials[i].pTextureFilename )
				continue;

			std::string	strName;
			strName = sc::util::GetSpecularName ( m_pmcMesh->pMaterials[i].pTextureFilename, "_e" );

			if( m_pmcMesh->pMaterials[i].pTextureFilename )
				StringCchCopy( m_pMaterials[i].szTexture,	MAX_PATH, m_pmcMesh->pMaterials[i].pTextureFilename );
			StringCchCopy( m_pMaterials[i].szEffTex,	MAX_PATH, strName.c_str() );

			m_pMaterials[i].LoadTexture( EMMM_CHAR_MIPMAP, true );
		}
	}
	else if ( !m_pMaterials )							// ��Ʈ������ ���� �� ����Ʈ ����
	{
		m_dwMaterials = m_pmcMesh->GetNumMaterials();
		SAFE_DELETE_ARRAY ( m_pMaterials );
		m_pMaterials = new DXMATERIAL_CHAR_EFF[m_dwMaterials];

		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
			if( !m_pmcMesh->pMaterials[i].pTextureFilename )
				continue;

			std::string	strName;
			strName = sc::util::GetSpecularName ( m_pmcMesh->pMaterials[i].pTextureFilename, "_e" );

			if( m_pmcMesh->pMaterials[i].pTextureFilename )
				StringCchCopy( m_pMaterials[i].szTexture,	MAX_PATH, m_pmcMesh->pMaterials[i].pTextureFilename );
			StringCchCopy( m_pMaterials[i].szEffTex,	MAX_PATH, strName.c_str() );

			m_pMaterials[i].LoadTexture( EMMM_CHAR_MIPMAP, true );

			m_pMaterials[i].d3dMaterial.Power = 1.f;
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