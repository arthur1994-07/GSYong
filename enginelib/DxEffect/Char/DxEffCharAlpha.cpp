#include "pch.h"
#include "../../../SigmaCore/File/SerialFile.h"

#include "../../DxTools/Light/DxLightMan.h"
#include "../../DxTools/DxViewPort.h"

#include "../../DxMeshs/SkinMesh/DxCharPart.h"

#include "./DxEffCharAlpha.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

DWORD		DxEffCharAlpha::TYPEID			= EMEFFCHAR_ALPHA;
const DWORD	DxEffCharAlpha::VERSION			= 0x0103;
char		DxEffCharAlpha::NAME[MAX_PATH]	= "1.���� ���";

DxEffCharAlpha::DxEffCharAlpha(void) :
	DxEffChar(),
	m_pSkinMesh(NULL),
	m_pmcMesh(NULL),

	m_fElapsedTime(0.f),
	m_fTimeSum(0.f)
{
	m_dwZWriteEnable = NULL;
	m_dwFogEnable = NULL;
	m_dwAlphaBlendEnable = NULL;
	m_dwSecBlend = NULL;
	m_dwDestBlend = NULL;
}

DxEffCharAlpha::~DxEffCharAlpha(void)
{
}

DxEffChar* DxEffCharAlpha::CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece, DWORD dwMaterialNum )
{
	HRESULT hr;
	DxEffCharAlpha *pEffChar = new DxEffCharAlpha;
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

HRESULT DxEffCharAlpha::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
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

	// m_pmcMesh ����.
	IsExist_Copy_pmcMesh();

	return S_OK;
}

HRESULT DxEffCharAlpha::DeleteDeviceObjects ()
{
	return S_OK;
}

HRESULT DxEffCharAlpha::FrameMove ( float fTime, float fElapsedTime )
{
	HRESULT hr = S_OK;

	m_fTimeSum		+= fElapsedTime;
	m_fElapsedTime	= fElapsedTime;

	float	fFullTime = m_fWaitTime + m_fPlayTime;

	if ( m_fTimeSum > fFullTime )
	{
		m_fTimeSum = fFullTime;
	}

	return S_OK;
}

HRESULT	DxEffCharAlpha::SettingState ( LPDIRECT3DDEVICEQ pd3dDevice, SKINEFFDATA& sSKINEFFDATA )
{
	pd3dDevice->GetRenderState( D3DRS_ZWRITEENABLE,		&m_dwZWriteEnable );
	pd3dDevice->GetRenderState( D3DRS_FOGENABLE,		&m_dwFogEnable );
	pd3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE,	&m_dwAlphaBlendEnable );
	pd3dDevice->GetRenderState( D3DRS_SRCBLEND,			&m_dwSecBlend );
	pd3dDevice->GetRenderState( D3DRS_DESTBLEND,		&m_dwDestBlend );

	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		TRUE );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,		FALSE );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_INVSRCALPHA );


	if ( !IsExist_Copy_pmcMesh() )
		return S_OK;

	if ( !m_pmcMesh->MeshData.pMesh )	
		return S_OK;

	if ( !m_pSkinMesh )					
		return S_OK;

	if ( m_dwFlags & USETRANSLUCENT )
	{
		for ( DWORD i=0; i < m_pmcMesh->GetNumMaterials(); i++ )
		{
			m_pmcMesh->pMaterials[i].MatD3D.Diffuse.a	= m_fAlpha;
			m_pmcMesh->pMaterials[i].MatD3D.Ambient.a	= m_fAlpha;
			m_pmcMesh->pMaterials[i].MatD3D.Specular.a	= 0.f;
		}
	}
	else
	{
		float	fFullTime	= m_fWaitTime + m_fPlayTime;

		if ( m_fTimeSum >= m_fWaitTime )
		{
			float	fAlpha = ((fFullTime-m_fTimeSum)/m_fPlayTime);

			if ( fAlpha > 1.f )	fAlpha = 1.f;
			if ( fAlpha < 0.f )	fAlpha = 0.f;

			fAlpha = ((1.f-m_fAlpha)*fAlpha) + m_fAlpha;		// �ּ� ���İ��� �����. ^^;

			for ( DWORD i=0; i < m_pmcMesh->GetNumMaterials(); i++ )
			{
				m_pmcMesh->pMaterials[i].MatD3D.Diffuse.a	= fAlpha;
				m_pmcMesh->pMaterials[i].MatD3D.Ambient.a	= fAlpha;
				m_pmcMesh->pMaterials[i].MatD3D.Specular.a	= fAlpha;
			}
		}
	}

	return S_OK;
}

HRESULT	DxEffCharAlpha::RestoreState ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		m_dwZWriteEnable );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,		m_dwFogEnable );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	m_dwAlphaBlendEnable );
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			m_dwSecBlend );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		m_dwDestBlend );

	if ( !IsExist_Copy_pmcMesh() )
		return S_OK;

	if ( !m_pmcMesh->MeshData.pMesh )	
		return S_OK;

	if ( !m_pSkinMesh )					
		return S_OK;

	if ( m_dwFlags & USETRANSLUCENT )
	{
		for ( DWORD i=0; i < m_pmcMesh->GetNumMaterials(); i++ )
		{
			m_pmcMesh->pMaterials[i].MatD3D.Diffuse.a	= 1.0f;
			m_pmcMesh->pMaterials[i].MatD3D.Ambient.a	= 1.0f;
			m_pmcMesh->pMaterials[i].MatD3D.Specular.a	= 1.0f;
		}
	}
	else
	{
		float	fFullTime	= m_fWaitTime + m_fPlayTime;

		if ( m_fTimeSum >= m_fWaitTime )
		{

			for ( DWORD i=0; i < m_pmcMesh->GetNumMaterials(); i++ )
			{
				m_pmcMesh->pMaterials[i].MatD3D.Diffuse.a	= 1.0f;
				m_pmcMesh->pMaterials[i].MatD3D.Ambient.a	= 1.0f;
				m_pmcMesh->pMaterials[i].MatD3D.Specular.a	= 1.0f;
			}
		}
	}

	return S_OK;
}

HRESULT DxEffCharAlpha::LoadFile_0100 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	EFFCHAR_PROPERTY_ALPHA_100	pProp;

	//	Note : ����Ʈ�� Property �� ����.
	//
	SFile.ReadBuffer ( &pProp, sizeof(EFFCHAR_PROPERTY_ALPHA_100) );

	m_fWaitTime = 2.f;
	m_fPlayTime = 1.f;
	m_fHeight	= 50.f;

	return S_OK;
}

HRESULT DxEffCharAlpha::LoadFile_0101 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	EFFCHAR_PROPERTY_ALPHA_101	pProp;

	//	Note : ����Ʈ�� Property �� ����.
	//
	SFile.ReadBuffer ( &pProp, sizeof(EFFCHAR_PROPERTY_ALPHA_101) );

	m_fHeight	= pProp.m_fHeight;
	m_fPlayTime = pProp.m_fPlayTime;
	m_fWaitTime	= pProp.m_fWaitTime;

	return S_OK;
}


HRESULT DxEffCharAlpha::LoadFile_0102 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : ����Ʈ�� Property �� ����.
	//
	EFFCHAR_PROPERTY_ALPHA_102	pProp;

	//	Note : ����Ʈ�� Property �� ����.
	//
	SFile.ReadBuffer ( &pProp, sizeof(EFFCHAR_PROPERTY_ALPHA_102) );

	m_dwFlags = pProp.m_dwFlags;
	m_fHeight = pProp.m_fHeight;
	m_fAlpha = pProp.m_fAlpha;
	m_fWaitTime = pProp.m_fWaitTime;
	m_fPlayTime = pProp.m_fPlayTime;

	return S_OK;
}


HRESULT DxEffCharAlpha::LoadFile_0103 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : ����Ʈ�� Property �� ����.
	//
	SFile.ReadBuffer ( &m_Property, sizeof(m_Property) );

	return S_OK;
}


HRESULT	DxEffCharAlpha::LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
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
	case VERSION:	LoadFile_0103( SFile, pd3dDevice );	break;
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

HRESULT	DxEffCharAlpha::SaveFile ( sc::BaseStream &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	//	Note : DATA�� ����� ����Ѵ�. Load �ÿ� ������ Ʋ�� ��� ����.
	//
	SFile << (DWORD) ( sizeof(m_Property) );

	//	Note : ����Ʈ�� Property �� ����.
	//
	SFile.WriteBuffer ( &m_Property, sizeof(m_Property) );

	return S_OK;
}

BOOL DxEffCharAlpha::IsExist_Copy_pmcMesh()
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

	return TRUE;
}