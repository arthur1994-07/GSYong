#include "pch.h"
#include "../../DxEffect/Single/DxEffSingle.h"
#include "../Single/DxEffSinglePropGMan.h"
#include "../../../SigmaCore/File/SerialFile.h"

#include "../../DxTools/Light/DxLightMan.h"
#include "../../DxTools/DxViewPort.h"
#include "../../DxEffect/Frame/DxEffectMan.h"

#include "../../DxMeshs/SkinMesh/DxCharPart.h"
#include "../../DxMeshs/SkinMesh/DxSkinPieceContainer.h"

#include "./DxEffCharSpecular.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

DWORD		DxEffCharSpecular::TYPEID			= EMEFFCHAR_SPECULAR;
const DWORD	DxEffCharSpecular::VERSION			= 0x0101;
char		DxEffCharSpecular::NAME[MAX_PATH]	= "1.스펙 (권장)";

LPDIRECT3DSTATEBLOCK9		DxEffCharSpecular::m_pSavedStateBlock	= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffCharSpecular::m_pEffectStateBlock	= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffCharSpecular::m_pSavedDisableSB	= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffCharSpecular::m_pEffectDisableSB	= NULL;

DxEffCharSpecular::DxEffCharSpecular(void) :
	DxEffChar(),
	m_pSkinMesh(NULL),
	m_pmcMesh(NULL),
	m_dwDetail_1(CRDT_DEFAULT),
	m_dwDetail_2(CRDT_DEFAULT),
	m_dwDetail_3(CRDT_DEFAULT)
{
}

DxEffCharSpecular::~DxEffCharSpecular(void)
{
	CleanUp ();
}

DxEffChar* DxEffCharSpecular::CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece, DWORD dwMaterialNum )
{
	HRESULT hr;
	DxEffCharSpecular *pEffChar = new DxEffCharSpecular;
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

HRESULT DxEffCharSpecular::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{	
	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,	D3DTA_SPECULAR );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATEALPHA_ADDCOLOR  );

		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLORARG1,	D3DTA_CURRENT );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,	D3DTOP_MODULATE  );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedStateBlock );
		else			pd3dDevice->EndStateBlock( &m_pEffectStateBlock );
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedDisableSB );
		else			pd3dDevice->EndStateBlock( &m_pEffectDisableSB );
	}

	return S_OK;
}

HRESULT DxEffCharSpecular::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSavedStateBlock );
	SAFE_RELEASE( m_pEffectStateBlock );
	SAFE_RELEASE( m_pSavedDisableSB );
	SAFE_RELEASE( m_pEffectDisableSB );

	return S_OK;
}

HRESULT DxEffCharSpecular::OneTimeSceneInit ()
{

	return S_OK;
}

HRESULT DxEffCharSpecular::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
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

	//
	////	Note : 비어있다면 건너 뛴다.
	////
	//if ( !m_pmcMesh )	return S_OK;

	//m_dwDetail_1 = CRDT_DEFAULT;
	//m_dwDetail_2 = CRDT_DEFAULT;
	//m_dwDetail_3 = CRDT_DEFAULT;

	//for ( DWORD i=0; i < m_pmcMesh->GetNumMaterials(); i++ )
	//{
	//	if( !m_pmcMesh->pMaterials[i].pTextureFilename )	continue;

	//	if ( i == 0 )
	//	{
	//		m_dwFlag |= ISTEXTURE1;
	//		if ( m_dwFlag & USETEXTURE1 )	m_dwDetail_1 = CRDT_SPECULAR;

	//		if( m_pmcMesh->pMaterials[i].pTextureFilename )
	//			StringCchCopy( m_szSrcTex_1, MAX_PATH, m_pmcMesh->pMaterials[i].pTextureFilename );
	//	}
	//	else if ( i == 1 )
	//	{
	//		m_dwFlag |= ISTEXTURE2;
	//		if ( m_dwFlag & USETEXTURE2 )	m_dwDetail_2 = CRDT_SPECULAR;

	//		if( m_pmcMesh->pMaterials[i].pTextureFilename )
	//			StringCchCopy( m_szSrcTex_2, MAX_PATH, m_pmcMesh->pMaterials[i].pTextureFilename );
	//	}
	//	else if ( i == 2 )
	//	{
	//		m_dwFlag |= ISTEXTURE3;
	//		if ( m_dwFlag & USETEXTURE3 )	m_dwDetail_3 = CRDT_SPECULAR;

	//		if( m_pmcMesh->pMaterials[i].pTextureFilename )
	//			StringCchCopy( m_szSrcTex_3, MAX_PATH, m_pmcMesh->pMaterials[i].pTextureFilename );
	//	}
	//}

	return S_OK;
}

HRESULT DxEffCharSpecular::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	return S_OK;
}

HRESULT DxEffCharSpecular::InvalidateDeviceObjects ()
{
	m_pd3dDevice = NULL;

	return S_OK;
}

HRESULT DxEffCharSpecular::DeleteDeviceObjects ()
{
    return S_OK;
}

HRESULT DxEffCharSpecular::FinalCleanup ()
{

	return S_OK;
}

HRESULT DxEffCharSpecular::FrameMove ( float fTime, float fElapsedTime )
{
	HRESULT hr = S_OK;

	return S_OK;
}

void	DxEffCharSpecular::Render( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* arrayBoneMatrices, BOOL bPieceRender )
{
	HRESULT hr = S_OK;

 //   if ( !m_pmcMesh ) return;
	//if ( !m_pmcMesh->MeshData.pMesh )	return;
	//if ( !m_pSkinMesh )					return;

	//if ( DxEffectMan::GetInstance().GetDetailFlag() & REALSPECULAR ) 
	//{
	//	m_pSavedStateBlock->Capture();
	//	m_pEffectStateBlock->Apply();

	//	BOOL bZBiasChange(FALSE);
	//	m_pmcMesh->SetMaterial ( pd3dDevice, 0, bZBiasChange );

	////	m_pSkinMesh->DrawMeshContainer ( pd3dDevice, m_pmcMesh, TRUE, TRUE, TRUE, m_dwDetail_1, m_dwDetail_2, m_dwDetail_3 );

	//	m_pSavedStateBlock->Apply();
	//}
	//else
	//{
	//	m_pSavedDisableSB->Capture();
	//	m_pEffectDisableSB->Apply();

	//	BOOL bZBiasChange(FALSE);
	//	m_pmcMesh->SetMaterial ( pd3dDevice, 0, bZBiasChange );

	////	m_pSkinMesh->DrawMeshContainer ( pd3dDevice, m_pmcMesh, TRUE, TRUE, TRUE );

	//	m_pSavedDisableSB->Apply();
	//}
}

HRESULT DxEffCharSpecular::LoadFile_0100 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 이팩트의 Property 를 저장.
	//
	EFFCHAR_PROPERTY_SPECULAR_100 Property;

	SFile.ReadBuffer ( &Property, sizeof(EFFCHAR_PROPERTY_SPECULAR_100) );

	m_Property.Assign( Property );

	return S_OK;
}
HRESULT DxEffCharSpecular::LoadFile_0101 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{

	//	Note : 이팩트의 Property 를 저장.
	//
	SFile.ReadBuffer ( &m_Property, sizeof(m_Property) );

	return S_OK;
}


HRESULT	DxEffCharSpecular::LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
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

HRESULT	DxEffCharSpecular::SaveFile ( sc::BaseStream &SFile )
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
