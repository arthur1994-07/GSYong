#include "pch.h"
#include "../../../SigmaCore/File/SerialFile.h"
#include "../../../SigmaCore/Util/Util.h"

#include "../../../VisualMaterialLib/Manager/DxVMManager.h"
#include "../../../VisualMaterialLib/Define/DxVMDefine.h"
#include "../../../VisualMaterialLib/Util/NSNearPointLight.h"
#include "../../../VisualMaterialLib/Util/DxTangentMesh.h"

#include "../../DxTools/Light/DxLightMan.h"
#include "../../DxTools/DxBackUpRendTarget.h"
#include "../../DxTools/TextureManager.h"
#include "../../DxTools/DxRenderStates.h"
#include "../../DxTools/DxViewPort.h"
#include "../../DxTools/DxFogMan.h"

#include "../../DxMeshs/SkinMesh/DxCharPart.h"
#include "../../DxMeshs/SkinMesh/DxSkinPieceContainer.h"
#include "../../DxMeshs/SkinMesh/_new/DxSkinMesh9_NORMAL.h"

#include "../../G-Logic/GLPeriod.h"

#include "../Frame/DxEffectMan.h"
#include "./DxEffCharAmbient.h"

#include "./DxEffCharWorldBattleFlag.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

#define V(x)           { hr = x; }

extern BOOL		g_bCHAR_EDIT_RUN;

DWORD		DxEffCharWorldBattleFlag::TYPEID			= EMEFFCHAR_WORLD_BATTLE_FLAG;
const DWORD	DxEffCharWorldBattleFlag::VERSION			= 0x0100;
char		DxEffCharWorldBattleFlag::NAME[MAX_PATH]	= "0.WorldBattleFlag";

DxEffCharWorldBattleFlag::DxEffCharWorldBattleFlag(void)
	: DxEffChar()
	, m_pd3dDevice(NULL)
	, m_pSkinMesh(NULL)
	, m_pmcMesh(NULL)
	, m_pMaterials(NULL)
{
}

DxEffCharWorldBattleFlag::~DxEffCharWorldBattleFlag(void)
{
	m_dwMaterials = 0L;
	SAFE_DELETE_ARRAY ( m_pMaterials );
}

DxEffChar* DxEffCharWorldBattleFlag::CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece, DWORD dwMaterialNum )
{
	HRESULT hr;
	DxEffCharWorldBattleFlag *pEffChar = new DxEffCharWorldBattleFlag;
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

void DxEffCharWorldBattleFlag::SetMaterials ( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwMaterials, DXMATERIAL_CHAR_EFF* pMaterials )
{
	SAFE_DELETE_ARRAY ( m_pMaterials );
	m_pMaterials = new DXMATERIAL_CHAR_EFF[dwMaterials];
	m_dwMaterials = dwMaterials;

	for ( DWORD i=0; i<dwMaterials; i++ )
	{
		m_pMaterials[i].SetType( pMaterials[i].m_emType );
		StringCchCopy( m_pMaterials[i].szTexture,	MAX_PATH, pMaterials[i].szTexture );
	}
}

HRESULT DxEffCharWorldBattleFlag::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr(S_OK);

	m_pd3dDevice = pd3dDevice;

	if ( m_pCharPart )
	{
		m_pSkinMesh = m_pCharPart->m_rSkinMesh;
	}
	else if ( m_pSkinPiece )
	{
		m_pSkinMesh = m_pSkinPiece->m_skinMeshRes.get();
	}

	IsExist_Copy_pmcMesh();

	return S_OK;
}

HRESULT DxEffCharWorldBattleFlag::DeleteDeviceObjects ()
{
	SAFE_DELETE_ARRAY( m_pMaterials );

	return S_OK;
}

//void DxEffCharWorldBattleFlag::CreateBakeTexWB( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ& pResultTex )
//{
//	DWORD dwWidth = 128;
//	DWORD dwHeight = 128;
//
//	LPDIRECT3DTEXTUREQ	pBakeMaskTexTemp(NULL);
//	LPDIRECT3DSURFACEQ	pBakeMaskSuf(NULL);
//
//	HRESULT hr = pd3dDevice->CreateTexture( dwWidth, dwHeight, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pBakeMaskTexTemp, NULL );
//	if( FAILED(hr) )
//	{
//		return;
//	}
//	hr = pBakeMaskTexTemp->GetSurfaceLevel( 0, &pBakeMaskSuf );
//	if( FAILED(hr) )
//	{
//		SAFE_RELEASE( pBakeMaskTexTemp );
//		return;
//	}
//
//
//
//	//////////////////////////////////////////////////////////////////////////
//	//						BeginScene
//	//						사용상 주의
//	// pd3dDevice->BeginScene 이 이전에 호출 되어있으면 hr 에 잘못된 값이 나올 수 있다.
//	HRESULT hr_BeginPass = pd3dDevice->BeginScene();
//
//
//
//	{
//		DxBackupTarget sBackupTarget;
//		sBackupTarget.Backup( pd3dDevice );
//
//		pd3dDevice->SetRenderTarget( 0, pBakeMaskSuf );
//		pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(255,255,255,255), 1.0f, 0L );
//
//		float fMicroOffsetW = 0.5f/(float)dwWidth;
//		float fMicroOffsetH = 0.5f/(float)dwHeight;
//
//		VERTEXRHW	sVertBase[4];
//		sVertBase[0].vPos = D3DXVECTOR4 ( 0.f,				0.f,				1.f, 1.f );
//		sVertBase[1].vPos = D3DXVECTOR4 ( (float)dwWidth,	0.f,				1.f, 1.f );
//		sVertBase[2].vPos = D3DXVECTOR4 ( 0.f,				(float)dwHeight,	1.f, 1.f );
//		sVertBase[3].vPos = D3DXVECTOR4 ( (float)dwWidth,	(float)dwHeight,	1.f, 1.f );
//
//		sVertBase[0].vTex = D3DXVECTOR2 ( 0.f+fMicroOffsetW, 0.f+fMicroOffsetH );
//		sVertBase[1].vTex = D3DXVECTOR2 ( 1.f+fMicroOffsetW, 0.f+fMicroOffsetH );
//		sVertBase[2].vTex = D3DXVECTOR2 ( 0.f+fMicroOffsetW, 1.f+fMicroOffsetH );
//		sVertBase[3].vTex = D3DXVECTOR2 ( 1.f+fMicroOffsetW, 1.f+fMicroOffsetH );
//
//		DWORD dwZWriteEnable;
//		DWORD dwZEnable;
//		DWORD dwFogEnable;
//		pd3dDevice->GetRenderState( D3DRS_ZWRITEENABLE,				&dwZWriteEnable );
//		pd3dDevice->GetRenderState( D3DRS_ZENABLE,					&dwZEnable );
//		pd3dDevice->GetRenderState( D3DRS_FOGENABLE,				&dwFogEnable );
//		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,				FALSE );
//		pd3dDevice->SetRenderState( D3DRS_ZENABLE,					FALSE );
//		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,				FALSE );
//
//		////////////////////////////////////////////////////////////////////////
//		// PixelShader만 쓸 경우 이건 맞춰줘야한다.
//		// DrawPrimitiveUP 를 쓸 경우 SetFVF 가 필요한거 같음.
//		DWORD dwFVF;
//		pd3dDevice->GetFVF(&dwFVF);
//		pd3dDevice->SetFVF(VERTEXRHW::FVF);
//
//		//pd3dDevice->SetTexture( 0,  );
//
//		pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, sVertBase, sizeof(VERTEXRHW) );
//
//		pd3dDevice->SetFVF(dwFVF);
//
//		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	    dwZWriteEnable );
//		pd3dDevice->SetRenderState( D3DRS_ZENABLE,		    dwZEnable );
//		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,		dwFogEnable );
//	}
//
//
//
//	//////////////////////////////////////////////////////////////////////////
//	//						EndScene
//	//						사용상 주의
//	if ( SUCCEEDED( hr_BeginPass ) )
//	{
//		pd3dDevice->EndScene();
//	}
//	else
//	{
//		int a=0;
//	}
//
//
//
//
//	SAFE_RELEASE( pBakeMaskSuf );
//
//	//////////////////////////////////////////////////////////////////////////
//	//					속도가 빠르지만 용량이 크다. Release 문제가 있다.
//	//
//	SAFE_RELEASE( pResultTex );
//	pResultTex = reinterpret_cast<LPDIRECT3DTEXTURE9>( pBakeMaskTexTemp );
//}

// 이곳에서 텍스쳐가 만들어지면, 렌더링 되는 BaseTexture 를 사용하지 않고 이곳에서 만들어진 것을 사용하게 된다.
void DxEffCharWorldBattleFlag::SetWorldBattleFlag( LPDIRECT3DTEXTUREQ apOverlayTexOUT[], const TCHAR* pWorldBattleFlag )
{
	// 이전 것은 정리한다.
	for ( DWORD i=0; i<m_dwMaterials; i++ )
	{
		if ( m_pMaterials[i].GetType() == EMCET_TEXTURE_MATRIX )
		{
			SAFE_RELEASE( apOverlayTexOUT[i] );
		}
	}

	if ( !pWorldBattleFlag )
		return;

	// 텍스쳐 이름을 받아온다.
	// 승리국가가 있으면 이름이 있고, 없으면 문자가 없다.
	const TSTRING strTextureName(pWorldBattleFlag);
	if ( !strTextureName.empty() )
	{
		m_textureRes = TextureManager::GetInstance().LoadTexture(
			strTextureName.c_str(),
			false,
			TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
			EMMM_CHAR_MIPMAP,
			D3DFMT_UNKNOWN,
			TEXTURE_RESIZE_NONE,
			TEXTURE_CHAR,
			false,
			FALSE );

		LPDIRECT3DTEXTUREQ pTexture = m_textureRes.GetTexture();

		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
			if ( m_pMaterials[i].GetType() == EMCET_TEXTURE_MATRIX )
			{
				apOverlayTexOUT[i] = pTexture;
				apOverlayTexOUT[i]->AddRef();	// apOverlayTex 에 있는건 Release 가 어느때나 될 수 있으므로 참조값을 올리도록 한다.
			}
		}
	}
}

HRESULT DxEffCharWorldBattleFlag::FrameMove ( float fTime, float fElapsedTime )
{
	return S_OK;
}

HRESULT DxEffCharWorldBattleFlag::LoadFile_0100 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
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
		}
	}

	return S_OK;
}

HRESULT	DxEffCharWorldBattleFlag::LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	SFile >> dwSize;

	switch ( dwVer )
	{
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

HRESULT	DxEffCharWorldBattleFlag::SaveFile ( sc::BaseStream &SFile )
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

BOOL DxEffCharWorldBattleFlag::IsExist_Copy_pmcMesh()
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

	// CharEdit 에서 처음 효과를 생성하면 m_pMaterials 가 NULL 이 나온다.
	if ( m_pMaterials )
	{
		// Material 갯수가 변동되었을 수 있으므로 갯수 확인 후 작업은 해야한다.
		if ( m_dwMaterials != m_pmcMesh->GetNumMaterials() )
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

		return TRUE;
	}

	if ( m_pMaterials && (m_dwMaterials != m_pmcMesh->GetNumMaterials()) )
	{
		m_dwMaterials = m_pmcMesh->GetNumMaterials();		// 매트리얼 정보가 바뀌었을 때 그냥 삭제후 디폴트 생성
		SAFE_DELETE_ARRAY ( m_pMaterials );
		m_pMaterials = new DXMATERIAL_CHAR_EFF[m_dwMaterials];

		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
			if( !m_pmcMesh->pMaterials[i].pTextureFilename )	
				continue;

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
			if( !m_pmcMesh->pMaterials[i].pTextureFilename )	
				continue;

			if( m_pmcMesh->pMaterials[i].pTextureFilename )
				StringCchCopy( m_pMaterials[i].szTexture,	MAX_PATH, m_pmcMesh->pMaterials[i].pTextureFilename );
		}
	}
	else
	{
	}

	for ( DWORD i=0; i<m_dwMaterials; ++i )
	{
		m_pMaterials[i].SetType( EMCET_BASE );
	}

	return TRUE;
}
