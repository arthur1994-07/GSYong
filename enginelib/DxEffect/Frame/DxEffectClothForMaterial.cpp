// DxEffectClothForMaterial.cpp: implementation of the DxEffectBlur class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "../../DxMeshs/FrameMesh/DxFrameMesh.h"
#include "../../DxEffect/Frame/DxEffectMan.h"
#include "../../DxTools/Material/NSShadowRenderForMaterial.h"
#include "../../DxTools/Material/NSMaterialManager.h"
#include "../../DxTools/Material/NSMaterialSetting.h"
#include "../../DxTools/Material/MaterialMesh.h"
#include "../../DxTools/Material/MaterialFieldDef.h"
#include "../../DxTools/Light/NSShadowLight.h"
#include "../../DxTools/editmeshs.h"
#include "../../DxTools/DxRenderStates.h"
#include "../../DxTools/DxDynamicVB.h"
#include "../../DxTools/RENDERPARAM.h"
#include "../../DxLand/LightMap/LightMapBoxColor.h"
#include "../../DxLand/LightMap/NSLightMapFX.h"
#include "../../DxLand/DxPiece/NSPieceQuickSort.h"
#include "../../DxLand/DxPiece/DxSingleTexMesh.h"
#include "../../DxLand/DxPiece/DxPieceDef.h"
#include "../../../SigmaCore/File/SerialFile.h"
#include "../../../SigmaCore/Util/Util.h"
#include "../../../SigmaCore/Log/LogMan.h"

#include "./DxEffectClothForMaterial.h"

#define DAMPING		2.0f

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


const DWORD DxEffectClothForMaterial::TYPEID = DEF_EFFECT_CLOTH_FOR_MATRERIAL;
const DWORD	DxEffectClothForMaterial::VERSION = 0x00000100;
const char DxEffectClothForMaterial::NAME[] = "[ 옷,천막(Material) ]";
const DWORD DxEffectClothForMaterial::FLAG = _EFF_REPLACE;

void DxEffectClothForMaterial::GetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD &dwVer )
{
	pProp = (PBYTE) &m_Property;
	dwSize = sizeof(CLOTH_FOR_MATERIAL_PROPERTY);
	dwVer = VERSION;
}

void DxEffectClothForMaterial::SetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD dwVer )
{
	if ( ( dwVer==VERSION && dwSize==sizeof(CLOTH_FOR_MATERIAL_PROPERTY) ) )
	{
		memcpy ( &m_Property, pProp, dwSize );
	}

	// Note : 실행.!@
	m_bPlay = true;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DxEffectClothForMaterial::DxEffectClothForMaterial() :
	m_bEachRender(TRUE),
	m_iNodeCount(0),
	m_fCoEfficient(400.0f),
	m_fWindConstant(1.0f),	
	m_vWindDirection(D3DXVECTOR3(1.0f, 0.0f, 1.0f)),
	m_fWindForce(3.0f),	
	m_fGravity(-3.81f),
	m_fTime(0.0f),	
	m_iWindMin(0),
	m_bPlay(true),
	m_dwNumVertices(0),
	m_dwNumIndices(0),
	m_dwNumLockIndex(0),
	m_pLockIndex(NULL),
	m_pNodeTable(NULL),
	m_pParticle(NULL),
	m_pTempLockIndex(NULL),
	m_pLocalMeshSRC(NULL),
	m_pVertArray(NULL),
	m_fSumTime(0.f)
	, m_bVertexColor(FALSE)
	, m_dwVer(-1)
	, m_dwFlags(0L)
	, m_fCubeMapValue(0.f)
	, m_fSpotSpecPower(0.f)
	, m_fSpotSpecMulti(0.f)
	, m_bCheckTex(FALSE)
	, m_rMaterialSmall(NULL)
{
	StringCchCopy( m_szTexture, MAX_PATH, "_Wa_water1.bmp" );
	D3DXMatrixIdentity ( &m_matWorld );
}

DxEffectClothForMaterial::~DxEffectClothForMaterial()
{
	SAFE_DELETE_ARRAY(m_szAdaptFrame);

	SAFE_RELEASE( m_pLocalMeshSRC );
	SAFE_DELETE_ARRAY( m_pVertArray );

	SAFE_DELETE_ARRAY ( m_pLockIndex );
	SAFE_DELETE_ARRAY ( m_pParticle );
	SAFE_DELETE_ARRAY ( m_pNodeTable );

	SAFE_DELETE_ARRAY ( m_pTempLockIndex );
}

HRESULT DxEffectClothForMaterial::AdaptToDxFrameChild ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	DxMeshes *pmsMeshs;

	if ( pframeCur->pmsMeshs != NULL )
	{
		pmsMeshs = pframeCur->pmsMeshs;

		while ( pmsMeshs != NULL )
		{
			if ( pmsMeshs->m_pLocalMesh )
			{
				m_matWorld = pframeCur->matCombined;
				if ( !pmsMeshs->m_vecMaterialName.empty() )
				{
					m_strMaterialName = pmsMeshs->m_vecMaterialName[0];
				}
				m_bVertexColor = pmsMeshs->m_bVertexColor;

				if ( pmsMeshs->m_cMaterials > 1 )
					AfxMessageBox( _T("Material 갯수는 1개가 되어야 합니다.") );

				if ( pmsMeshs->m_cMaterials >= 1 )
				{
					StringCchCopy( m_szTexture, MAX_PATH, pmsMeshs->m_strTextureFiles[0] );
				}

				
				XFileLoad ( pd3dDevice, pmsMeshs->m_pLocalMesh, pmsMeshs->m_bEnableLightMap, pmsMeshs->m_dwLightMapIndex );
				ThreadLoadingTexture( pd3dDevice );
			}
			else
			{
				BaseBuffToDynamicBuff ( pd3dDevice );
			}
			pmsMeshs = pmsMeshs->m_pMeshNext;
		}
	}

	return hr;
}

void DxEffectClothForMaterial::ThreadLoadingTexture( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Note : Texture Loading
	if( !m_textureRes.IsValid() )
	{
		if( !m_bCheckTex )
		{
			TSTRING strTextureName = m_szTexture;;
			if ( !strTextureName.empty() )
			{
				m_textureRes = TextureManager::GetInstance().LoadTexture(
					m_szTexture,
					false,
					TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
					0,
					D3DFMT_UNKNOWN,
					TEXTURE_RESIZE_NONE,
					TEXTURE_LAND,
					true,
					FALSE );
			}

			if ( m_dwFlags & ENSTM_MASK_MAP )
			{
				TSTRING strMaskTexName = sc::util::GetSpecularName( m_szTexture, _T("_mask") );

				m_pMaskTexRes = TextureManager::GetInstance().LoadTexture(
					strMaskTexName,
					false,
					TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
					0,
					D3DFMT_UNKNOWN,
					TEXTURE_RESIZE_NONE,
					TEXTURE_LAND,
					true,
					FALSE );
			}

			m_bCheckTex = TRUE;
		}
	}
}

HRESULT DxEffectClothForMaterial::AdaptToDxFrame ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !pFrame ) return S_OK;

	this->pLocalFrameEffNext = pFrame->pEffect;
	pFrame->pEffect = this;

	if ( pFrame->szName )
	{
		SAFE_DELETE_ARRAY(m_szAdaptFrame);
		size_t nStrLen = strlen(pFrame->szName)+1;
		m_szAdaptFrame = new char[nStrLen];

		StringCchCopy( m_szAdaptFrame, nStrLen, pFrame->szName );
	}

	return AdaptToDxFrameChild ( pFrame, pd3dDevice );
}

HRESULT DxEffectClothForMaterial::AdaptToEffList ( const DxFrame *const pFrame, LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT ( pFrame );

	if ( pFrame->szName )
	{
		SAFE_DELETE_ARRAY(m_szAdaptFrame);
		size_t nStrLen = strlen(pFrame->szName)+1;
		m_szAdaptFrame = new char[nStrLen];

		StringCchCopy( m_szAdaptFrame, nStrLen, pFrame->szName );
	}

	return S_OK;
}

HRESULT DxEffectClothForMaterial::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_bCheckTex = FALSE;
	ThreadLoadingTexture( pd3dDevice );

	return S_OK;
}

HRESULT DxEffectClothForMaterial::DeleteDeviceObjects ()
{
	m_textureRes.Clear();

	return S_OK;
}

HRESULT DxEffectClothForMaterial::FrameMove ( float fTime, float fElapsedTime )
{
	m_fTime = fElapsedTime;

	return S_OK;
}

HRESULT DxEffectClothForMaterial::MeshUpdate ( D3DVERTEXELEMENT9* pELMT )
{
	// 파티클 force를 전부 클리어 시킴.
	// force는 파티클의 거리와 질량으로써 결정되어 지기에 누적을 할 필요가 없어진다.
	// 만약 누적을 한다면 어떻게 될까? ^^

	if( m_pParticle == NULL ) 
		return S_OK;	

	D3DXVECTOR3 vTemp;

	int i = 0;

	if ( m_bPlay && m_pVertArray )
	{
		for( i = 0; i < (int)m_dwNumVertices ; i++ )
			m_pParticle[i].vForce = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );		

		//난수에 의한 Wind의 방향을 전체적으로 준다.
		//wind라기 보다는 노이즈에 가깝다.
		//Wind 부분을 삽입한 부분이다.	
		for( i = 0 ; i < (int)m_dwNumVertices ; i++ )
		{
			if( m_pParticle[i].bLocked == FALSE )
			{
				//중력 부분의 적용
				m_pParticle[i].vForce.y = m_fGravity; 

				// 0 ~ 1.0사이 값으로 셋
				// Note: 바람의 세기 부분 
				D3DXVec3Normalize( &vTemp, &m_vWindDirection );
				m_pParticle[i].vForce += vTemp*m_fWindForce*sinf( D3DXToRadian( rand()%90 ) ); 					
					
				//	Note : 바람 모델 수정 
			//	m_pParticle[i].vForce +=  m_Wind.CalculateForce( m_vWindDirection, pSampleVertices[i].vNor, m_fWindForce, m_fWindConstant );						
			}
		}

		// 각 스프링으로 연결된 노드의 파티클 위치, 속도 정보를 이용하여서 Force을 구하는 부분
		D3DXVECTOR3 vPosition1, vPosition2, vVelocity1, vVelocity2, vForce, vDistance;	
		int index1, index2;

		for( i = 0 ; i < m_iNodeCount ; i++ )
		{	
			// 각 스프링의 노드 정보를 이용하여서 힘을 구하는 부분 
			index1 = m_pNodeTable[i].Index1;
			index2 = m_pNodeTable[i].Index2;

			vPosition1 = m_pParticle[ index1 ].vPosition;
			vPosition2 = m_pParticle[ index2 ].vPosition;

			vVelocity1 = m_pParticle[ index1 ].vVelocity;
			vVelocity2 = m_pParticle[ index2 ].vVelocity;

			vForce = m_Spring.CalculateForce( vPosition1, vPosition2, vVelocity1, vVelocity2, m_pNodeTable[i].fLength ); 

			if( !m_pParticle[ index1 ].bLocked )
				m_pParticle[ index1 ].vForce += vForce; //힘의 누적을 시킨다.그래야 서로 연관된 힘을 얻을수 있다.

			if( !m_pParticle[ index2 ].bLocked )
				m_pParticle[ index2 ].vForce += -vForce;	
		}

		//구한 힘을 이용하여서 각 속도 및 거리를 계산하여서 파티클에 최종적인 데이타를 넣어주는 부분 
		//f = ma에서 m을 생략하고 넣어 준다.	
		//float fElpTime = m_fTime; //고정시킴
		//if ( fElpTime > 0.03f )
		//{
		//	fElpTime = 0.03f;
		//}

		//	시간이 너무 작을 경우 노멀 뒤집고 다른 계산 안한다.
		//
		m_fSumTime += m_fTime;
		if ( m_fSumTime <= 0.03f )
		{
			////	Note : 노멀 뒤집기
			//for ( DWORD i=0; i<m_dwNumVertices; i++ )
			//{
			//	m_pVertArray[i].vNor = -m_pVertArray[i].vNor;
			//}

			return S_OK;
		}

		float fElpTime	= 0.03f;
		m_fSumTime		= 0.f;


		for( i = 0 ; i < (int)m_dwNumVertices ; i++ )
		{
			m_pParticle[i].vAccelerate = m_pParticle[i].vForce;
			m_pParticle[i].vVelocity += m_pParticle[i].vAccelerate*fElpTime;
			m_pParticle[i].vPosition += m_pParticle[i].vVelocity*fElpTime;

		//	CDebugSet::ToView ( "%f,   %f,   %f", 10, m_pParticle[i].vVelocity.x, m_pParticle[i].vVelocity.y, m_pParticle[i].vVelocity.z );
		}

		D3DXVECTOR3* pPos(NULL);
		UINT nVertSize = D3DXGetDeclVertexSize( pELMT, 0 );
		DWORD dwNormalOffset(0L);

		//모든 파티클 정점 정보를 Vertex Buffer로 복사한다.
		for( i = 0 ; i < (int)m_dwNumVertices ; i++ )
		{
			pPos = reinterpret_cast<D3DXVECTOR3*>( m_pVertArray+(i*nVertSize) );
			*pPos = m_pParticle[ i ].vPosition;						
		}

		for ( DWORD i=0; i<MAXD3DDECLLENGTH; ++i )
		{
			if ( pELMT[i].Stream == 0x00ff )
				break;

			if ( pELMT[i].Usage == D3DDECLUSAGE_NORMAL )
			{
				dwNormalOffset = pELMT[i].Offset;
				break;
			}
		}

		WORD* pIndices(NULL);
		m_pLocalMeshSRC->LockIndexBuffer( NULL, (VOID**)&pIndices );
		{
			ComputeNormal( (int)m_dwNumVertices, m_pVertArray, nVertSize, dwNormalOffset, (int)m_dwNumIndices, pIndices ); //고라운드 쉐이딩 노말을 계산 하는 부분 			
		}
		m_pLocalMeshSRC->UnlockIndexBuffer();		
	}

	return S_OK;
}

HRESULT DxEffectClothForMaterial::RenderMAT ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, PieceParameter* pParameter, BOOL bMPShading, BOOL bReflect )
{
	if ( pFrameMesh )
	{
		const NSMaterialSetting::SETTING* pSetting = pFrameMesh->GetMaterialSetting_const()->GetMaterialSetting( m_strMaterialName );

		NSMaterialManager::UNLM_RENDER_MODE emUnLMRenderMode(NSMaterialManager::EMUNLM_RM_LIGHTMAP);
		if ( pSetting && pSetting->m_emUnLightMapType == NSMaterialSetting::RENDER_TYPE_VOXELCOLOR )
		{
			emUnLMRenderMode = NSMaterialManager::EMUNLM_RM_VOXELCOLOR;
		}

		// Note : NSMaterialManager 에 있는 mat 이라면 NSMaterialManager::Render 에서 모든 작업을 한다.
		MATERIAL_DATA sMaterial_Data;
		float fCubeMapValue = pSetting ? (pSetting->m_bCubeMap ? pSetting->m_fCubeMapValue : 0.f) : 0.f;
		if ( NSMaterialManager::Active( pd3dDevice, 
										m_strMaterialName, 
										NSMaterialManager::EMRM_TOOL, 
										emUnLMRenderMode,
										sMaterial_Data,
										FALSE,
										*m_pmatLocal, 
										1.f,
										m_bVertexColor,
										MaterialValue( fCubeMapValue, pSetting->m_fSpotSpecPower, pSetting->m_fSpotSpecMulti ),
										pSetting->m_bNormalMap ) )
		{
			RefreshMesh( pd3dDevice, sMaterial_Data.m_rMaterial, &sMaterial_Data.m_rMaterial->m_sMaterialTOOL );
			MeshUpdate( sMaterial_Data.m_rMaterial->m_sMaterialTOOL.m_pELMT );
			RenderTool( pd3dDevice, sMaterial_Data, pFrameMesh->GetMaterialSetting_const(), &sMaterial_Data.m_rMaterial->m_sMaterialTOOL );
		}

		//	Note : 원 나타내기.
		if ( !m_bPlay && m_pVertArray )
		{
			const D3DXVECTOR3* pPos(NULL);
			UINT nVertSize = D3DXGetDeclVertexSize( sMaterial_Data.m_rMaterial->m_sMaterialTOOL.m_pELMT, 0 );

			for ( DWORD i=0; i<m_dwNumLockIndex; i++ )
			{
				WORD k = m_pLockIndex[i];

				pPos = reinterpret_cast<D3DXVECTOR3*>( m_pVertArray+(k*nVertSize) );
				EDITMESHS::RENDERSPHERE ( pd3dDevice, *pPos, 1.0f );
			}
		}
	}
	else if ( pParameter )
	{
		if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_1_1 )
		{
			// 데칼인 경우에는 렌더링 되지 않도록 한다.
			if ( m_dwFlags & ENSTM_DECAL_FULL )
			{
				return S_OK;
			}

			const LightMapBoxColorData* pLMBCData = &pParameter->m_sLMBCData;

			NSMaterialManager::UNLM_RENDER_MODE emUnLMRenderMode(NSMaterialManager::EMUNLM_RM_LIGHTMAP);
			if ( !pLMBCData )
			{
				emUnLMRenderMode = NSMaterialManager::EMUNLM_RM_NULL;
			}
			else if ( m_dwFlags & ENSTM_UNLIGHTMAP_AMBIENT_CUBE )
			{
				emUnLMRenderMode = NSMaterialManager::EMUNLM_RM_AMBIENTCUBE;
			}
			else if ( m_dwFlags & ENSTM_UNLIGHTMAP_VOXEL_COLOR )
			{
				// RAD 경우는 Voxel 에 렌더링을 하지 않는다.
				if ( NSPieceQuickSort::g_bBakeRAD )
					return S_OK;

				emUnLMRenderMode = NSMaterialManager::EMUNLM_RM_VOXELCOLOR;
			}
			else
			{
				emUnLMRenderMode = NSMaterialManager::EMUNLM_RM_LIGHTMAP;
			}

			MATERIAL_DATA sMaterial_Data;
			if ( pParameter->m_nRenderMode == NSMaterialManager::EMRM_TOOL )
			{
				sMaterial_Data.m_bGameMode = FALSE;
			}
			BOOL bDecal = (m_dwFlags & ENSTM_DECAL_FULL) ? TRUE : FALSE;
			BOOL bVertexColor = (m_dwFlags & ENSTM_VERTEX_COLOR) ? TRUE : FALSE;
			BOOL bCubeMap = (m_dwFlags & ENSTM_CUBEMAP) ? TRUE : FALSE;
			BOOL bNormalMap = (m_dwFlags & ENSTM_NORMALMAP) ? TRUE : FALSE;
			if ( NSMaterialManager::Active_MATERIAL_DATA( pd3dDevice, 
				m_strMaterialName, 
				emUnLMRenderMode,
				sMaterial_Data,
				bDecal,
				bVertexColor,
				bCubeMap ? 1 : 0,
				bNormalMap ) )
			{
				MaterialSmall* pMaterialSmall(NULL);
				if ( pParameter->m_nRenderMode == NSMaterialManager::EMRM_TOOL )
				{
					pMaterialSmall = &sMaterial_Data.m_rMaterial->m_sMaterialTOOL;
				}
				else
				{
					pMaterialSmall = &sMaterial_Data.m_rMaterial->m_sMaterialGAME;
				}

				m_rMaterialSmall = pMaterialSmall;

				if ( bMPShading == FALSE || bReflect )
				{
					RefreshMesh( pd3dDevice, sMaterial_Data.m_rMaterial, pMaterialSmall );
					MeshUpdate( pMaterialSmall->m_pELMT );
				}
				RenderGameRoot( pd3dDevice, sMaterial_Data, pMaterialSmall, emUnLMRenderMode, pParameter, FALSE, bMPShading, FALSE );
			}
		}
		else
		{
			int nTexType(0);
			DWORD dwAlphaTestEnable(0L);
			DWORD dwAlphaBlendEnable(0L);
			TEXTURE_ALPHA sTEXTURE_ALPHA(EMTT_NORMAL);
			sTEXTURE_ALPHA = m_textureRes.GetAlphaType();
			if ( sTEXTURE_ALPHA == EMTT_ALPHA_HARD )
			{
				nTexType = 1;

				pd3dDevice->GetRenderState( D3DRS_ALPHATESTENABLE,	&dwAlphaTestEnable );
				pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	TRUE );
			}
			else if ( sTEXTURE_ALPHA == EMTT_ALPHA_SOFT || sTEXTURE_ALPHA == EMTT_ALPHA_SOFT01 || sTEXTURE_ALPHA == EMTT_ALPHA_SOFT02 )
			{
				nTexType = 2;

				pd3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE,	&dwAlphaBlendEnable );
				pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
				pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
				pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			}

			pd3dDevice->SetTexture( 0, m_textureRes.GetTexture() );

			pd3dDevice->SetTransform ( D3DTS_WORLD, m_pmatLocal );

			DWORD dwCullMode;
			pd3dDevice->GetRenderState( D3DRS_CULLMODE, &dwCullMode );

			pd3dDevice->SetRenderState ( D3DRS_CULLMODE, D3DCULL_CCW );
			m_pLocalMeshSRC->DrawSubset( 0 );

			pd3dDevice->SetRenderState ( D3DRS_CULLMODE, D3DCULL_CW );
			m_pLocalMeshSRC->DrawSubset( 0 );

			pd3dDevice->SetRenderState( D3DRS_CULLMODE, dwCullMode );

			if ( nTexType == 1 )
			{
				pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, dwAlphaTestEnable );
			}
			else if ( nTexType == 2 )
			{
				pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	dwAlphaBlendEnable );
			}
		}
	}

	return S_OK;
}

HRESULT DxEffectClothForMaterial::RenderMATForPSF ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, PieceParameter* pParameter )
{
	if ( pFrameMesh )
	{
		const NSMaterialSetting::SETTING* pSetting = pFrameMesh->GetMaterialSetting_const()->GetMaterialSetting( m_strMaterialName );

		NSMaterialManager::UNLM_RENDER_MODE emUnLMRenderMode(NSMaterialManager::EMUNLM_RM_LIGHTMAP);
		if ( pSetting && pSetting->m_emUnLightMapType == NSMaterialSetting::RENDER_TYPE_VOXELCOLOR )
		{
			emUnLMRenderMode = NSMaterialManager::EMUNLM_RM_VOXELCOLOR;
		}

		// Note : NSMaterialManager 에 있는 mat 이라면 NSMaterialManager::Render 에서 모든 작업을 한다.
		MATERIAL_DATA sMaterial_Data;
		float fCubeMapValue = pSetting ? (pSetting->m_bCubeMap ? pSetting->m_fCubeMapValue : 0.f) : 0.f;
		if ( NSMaterialManager::Active( pd3dDevice, 
			m_strMaterialName, 
			NSMaterialManager::EMRM_TOOL, 
			emUnLMRenderMode,
			sMaterial_Data,
			FALSE,
			*m_pmatLocal, 
			1.f,
			m_bVertexColor,
			MaterialValue( fCubeMapValue, pSetting->m_fSpotSpecPower, pSetting->m_fSpotSpecMulti ),
			pSetting->m_bNormalMap ) )
		{
			RefreshMesh( pd3dDevice, sMaterial_Data.m_rMaterial, &sMaterial_Data.m_rMaterial->m_sMaterialTOOL );
			MeshUpdate( sMaterial_Data.m_rMaterial->m_sMaterialTOOL.m_pELMT );
			RenderTool( pd3dDevice, sMaterial_Data, pFrameMesh->GetMaterialSetting_const(), &sMaterial_Data.m_rMaterial->m_sMaterialTOOL );
		}

		//	Note : 원 나타내기.
		if ( !m_bPlay && m_pVertArray )
		{
			const D3DXVECTOR3* pPos(NULL);
			UINT nVertSize = D3DXGetDeclVertexSize( sMaterial_Data.m_rMaterial->m_sMaterialTOOL.m_pELMT, 0 );

			for ( DWORD i=0; i<m_dwNumLockIndex; i++ )
			{
				WORD k = m_pLockIndex[i];

				pPos = reinterpret_cast<D3DXVECTOR3*>( m_pVertArray+(k*nVertSize) );
				EDITMESHS::RENDERSPHERE ( pd3dDevice, *pPos, 1.0f );
			}
		}
	}
	else if ( pParameter )
	{
		// 데칼인 경우에는 렌더링 되지 않도록 한다.
		if ( m_dwFlags & ENSTM_DECAL_FULL )
		{
			return S_OK;
		}

		const LightMapBoxColorData* pLMBCData = &pParameter->m_sLMBCData;

		NSMaterialManager::UNLM_RENDER_MODE emUnLMRenderMode(NSMaterialManager::EMUNLM_RM_LIGHTMAP);
		if ( !pLMBCData )
		{
			emUnLMRenderMode = NSMaterialManager::EMUNLM_RM_NULL;
		}
		else if ( m_dwFlags & ENSTM_UNLIGHTMAP_AMBIENT_CUBE )
		{
			emUnLMRenderMode = NSMaterialManager::EMUNLM_RM_AMBIENTCUBE;
		}
		else if ( m_dwFlags & ENSTM_UNLIGHTMAP_VOXEL_COLOR )
		{
			// RAD 경우는 Voxel 에 렌더링을 하지 않는다.
			if ( NSPieceQuickSort::g_bBakeRAD )
				return S_OK;

			emUnLMRenderMode = NSMaterialManager::EMUNLM_RM_VOXELCOLOR;
		}
		else
		{
			emUnLMRenderMode = NSMaterialManager::EMUNLM_RM_LIGHTMAP;
		}

		MATERIAL_DATA sMaterial_Data;
		if ( pParameter->m_nRenderMode == NSMaterialManager::EMRM_TOOL )
		{
			sMaterial_Data.m_bGameMode = FALSE;
		}
		BOOL bDecal = (m_dwFlags & ENSTM_DECAL_FULL) ? TRUE : FALSE;
		BOOL bVertexColor = (m_dwFlags & ENSTM_VERTEX_COLOR) ? TRUE : FALSE;
		BOOL bCubeMap = (m_dwFlags & ENSTM_CUBEMAP) ? TRUE : FALSE;
		BOOL bNormalMap = (m_dwFlags & ENSTM_NORMALMAP) ? TRUE : FALSE;
		if ( NSMaterialManager::Active_MATERIAL_DATA( pd3dDevice, 
			m_strMaterialName, 
			emUnLMRenderMode,
			sMaterial_Data,
			bDecal,
			bVertexColor,
			bCubeMap ? 1 : 0,
			bNormalMap ) )
		{
			MaterialSmall* pMaterialSmall(NULL);
			if ( pParameter->m_nRenderMode == NSMaterialManager::EMRM_TOOL )
			{
				pMaterialSmall = &sMaterial_Data.m_rMaterial->m_sMaterialTOOL;
			}
			else
			{
				pMaterialSmall = &sMaterial_Data.m_rMaterial->m_sMaterialGAME;
			}

			m_rMaterialSmall = pMaterialSmall;

			RefreshMesh( pd3dDevice, sMaterial_Data.m_rMaterial, pMaterialSmall );
			MeshUpdate( pMaterialSmall->m_pELMT );
			RenderGameRoot( pd3dDevice, sMaterial_Data, pMaterialSmall, emUnLMRenderMode, pParameter, FALSE, FALSE, TRUE );
		}
	}

	return S_OK;
}

void DxEffectClothForMaterial::RefreshMesh( LPDIRECT3DDEVICEQ pd3dDevice, const Material* pMaterial, const MaterialSmall* pMaterialSmall )
{
	if ( (m_dwVer!=pMaterialSmall->m_dwVer) || !m_pVertArray )
	{
		m_dwVer = pMaterialSmall->m_dwVer;

		LPD3DXMESH pTempMesh(NULL);
		if ( !NSMATERIALMESH::CreateMesh( pd3dDevice, pMaterial, pMaterialSmall, m_vecLightMapUV, m_pLocalMeshSRC, pTempMesh ) )
			return;

		//------------- 버택스 버퍼의 내용으로 파티클 정보를 채우는 부분 -----------//
		SAFE_DELETE_ARRAY( m_pVertArray );
		UINT nVertSize = D3DXGetDeclVertexSize( pMaterialSmall->m_pELMT, 0 );
		m_pVertArray = new BYTE[m_dwNumVertices*nVertSize];
		BYTE* pVerticsSRC(NULL);
		pTempMesh->LockVertexBuffer( NULL, (VOID**)&pVerticsSRC );
		{
			memcpy( m_pVertArray, pVerticsSRC, m_dwNumVertices*nVertSize ); //정점 정보 복사 
		}
		pTempMesh->UnlockVertexBuffer();
		SAFE_RELEASE ( pTempMesh );
	}
}

void DxEffectClothForMaterial::RenderTool( LPDIRECT3DDEVICEQ pd3dDevice, MATERIAL_DATA sMaterial_Data, const CMaterialSetting* pMaterialSetting, const MaterialSmall* pMaterialSmall )
{
	ID3DXEffect* pFX = sMaterial_Data.m_rMaterial->m_sFXnType.m_rFX;
	if ( !pFX )
		return;

	LPDIRECT3DINDEXBUFFER9 pIB(NULL);
	m_pLocalMeshSRC->GetIndexBuffer( &pIB );

	UINT nVertSize = D3DXGetDeclVertexSize( pMaterialSmall->m_pELMT, 0 );

	DWORD dwNormalOffset(0L);
	for ( DWORD i=0; i<MAXD3DDECLLENGTH; ++i )
	{
		if ( pMaterialSmall->m_pELMT[i].Stream == 0x00ff )
			break;

		if ( pMaterialSmall->m_pELMT[i].Usage == D3DDECLUSAGE_NORMAL )
		{
			dwNormalOffset = pMaterialSmall->m_pELMT[i].Offset;
			break;
		}
	}

	pd3dDevice->SetFVF( 0L );
	pd3dDevice->SetStreamSource( 0, DxDynamicVB::g_sVB_PNTI.pVB, 0, nVertSize );
	pd3dDevice->SetIndices( pIB );

	{
		const NSMaterialSetting::SETTING* pSetting = pMaterialSetting->GetMaterialSetting( m_strMaterialName );

		TEXTURE_ALPHA sTEXTURE_ALPHA(EMTT_NORMAL);

		int nTexType(0);
		DWORD dwAlphaTestEnable(0L);
		DWORD dwAlphaBlendEnable(0L);
		//if ( sMaterial_Data.m_bDefaultMaterial )
		{
			sTEXTURE_ALPHA = m_textureRes.GetAlphaType();

			

			pFX->SetTexture( "g_BaseTexture", m_textureRes.GetTexture() );

			if ( pSetting ? pSetting->m_bMaskCubeMapSpecularMap || pSetting->m_bNormalMap : FALSE )
			{
				// 없다면 로딩해본다.
				if ( !m_pMaskTexRes.GetTexture() )
				{
					TSTRING strName = m_textureRes.GetFileName();

					strName = sc::util::GetSpecularName( strName, _T("_mask") );

					m_pMaskTexRes = TextureManager::GetInstance().LoadTexture(
						strName,
						false,
						TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
						0,
						D3DFMT_UNKNOWN,
						TEXTURE_RESIZE_1024,
						TEXTURE_LAND,
						true,
						FALSE );
				}

				// 있다면 Mask 맵을 없다면 흰색으로 작동된다.
				if ( m_pMaskTexRes.GetTexture() )
				{
					pFX->SetTexture( "g_MaskTexture", m_pMaskTexRes.GetTexture() );
				}
				else
				{
					pFX->SetTexture( "g_MaskTexture", NSMaterialManager::GetMaskTexture() );
				}
			}
			else
			{
				pFX->SetTexture( "g_MaskTexture", NSMaterialManager::GetMaskTexture() );
			}

			if ( sTEXTURE_ALPHA == EMTT_ALPHA_HARD )
			{
				nTexType = 1;

				pd3dDevice->GetRenderState( D3DRS_ALPHATESTENABLE,	&dwAlphaTestEnable );
				pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	TRUE );
			}
			else if ( sTEXTURE_ALPHA == EMTT_ALPHA_SOFT || sTEXTURE_ALPHA == EMTT_ALPHA_SOFT01 || sTEXTURE_ALPHA == EMTT_ALPHA_SOFT02 )
			{
				nTexType = 2;

				pd3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE,	&dwAlphaBlendEnable );
				pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
				pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
				pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			}
		}

		pd3dDevice->SetVertexDeclaration( sMaterial_Data.m_rMaterial->m_sMaterialTOOL.m_pDCRT ); 

		//--------------------------------------------------------------------------------------------------
		// Front
		{
			DWORD dwVertexSizeFULL = m_dwNumVertices*nVertSize;
			if( !DxDynamicVB::g_sVB_PNTI.IsValidSize( dwVertexSizeFULL ) )
			{
				return;
			}

			UINT uiStartVertex = DxDynamicVB::g_sVB_PNTI.CopyToVB(
				m_pVertArray,
				dwVertexSizeFULL,
				m_dwNumVertices );

			UINT numPasses;
			pFX->Begin( &numPasses, 0 );
			if( 0 < numPasses )
			{
				pFX->BeginPass( 0 );
				{
					pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, uiStartVertex, 0, m_dwNumVertices, 0, m_dwNumIndices/3 );
				}
				pFX->EndPass();
			}
			pFX->End();
		}

		//--------------------------------------------------------------------------------------------------
		// Back
		if ( m_bPlay )
		{
			DWORD dwCullMode;
			pd3dDevice->GetRenderState( D3DRS_CULLMODE, &dwCullMode );

			pd3dDevice->SetRenderState ( D3DRS_CULLMODE, D3DCULL_CW );

			// 노멀 값 변경
			D3DXVECTOR3* pNor(NULL);
			for( int i = 0 ; i < (int)m_dwNumVertices ; i++ )
			{
				pNor = reinterpret_cast<D3DXVECTOR3*>( m_pVertArray+(i*nVertSize)+dwNormalOffset );
				(*pNor) = -(*pNor);
			}

			DWORD dwVertexSizeFULL = m_dwNumVertices*nVertSize;
			if( !DxDynamicVB::g_sVB_PNTI.IsValidSize( dwVertexSizeFULL ) )
			{
				return;
			}

			UINT uiStartVertex = DxDynamicVB::g_sVB_PNTI.CopyToVB(
				m_pVertArray,
				dwVertexSizeFULL,
				m_dwNumVertices );

			// 노멀 값 되돌림.
			for( int i = 0 ; i < (int)m_dwNumVertices ; i++ )
			{
				pNor = reinterpret_cast<D3DXVECTOR3*>( m_pVertArray+(i*nVertSize)+dwNormalOffset );
				(*pNor) = -(*pNor);
			}

			UINT numPasses;
			pFX->Begin( &numPasses, 0 );
			if( 0 < numPasses )
			{
				pFX->BeginPass( 0 );
				{
					pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, uiStartVertex, 0, m_dwNumVertices, 0, m_dwNumIndices/3 );
				}
				pFX->EndPass();
			}
			pFX->End();

			pd3dDevice->SetRenderState ( D3DRS_CULLMODE, dwCullMode );
		}

		{
			if ( nTexType == 1 )
			{
				pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, dwAlphaTestEnable );
			}
			else if ( nTexType == 2 )
			{
				pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	dwAlphaBlendEnable );
			}
		}
	}
}

void DxEffectClothForMaterial::RenderGameRoot( LPDIRECT3DDEVICEQ pd3dDevice, 
											  MATERIAL_DATA sMaterial_Data, 
											  const MaterialSmall* pMaterialSmall,
											  NSMaterialManager::UNLM_RENDER_MODE emUnLMRenderMode,
											  PieceParameter* pParameter,
											  BOOL bLightMapTest, 
											  BOOL bMPShading,
											  BOOL bPSF )
{
	UINT unPass(0);
	if ( bMPShading )
	{
		if ( m_textureRes.GetAlphaType() != EMTT_NORMAL )
		{
			unPass = 1;
		}
	}

	Material* pMaterial = sMaterial_Data.m_rMaterial;

	if ( RENDERPARAM::g_bFORCE_TnL_PS_3_0_DF && RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_DF )
	{
		//  옵션 상이상 이지만, 특정한 이유(반사)로 인해 렌더링은 TnL_PS_3_0_DF 으로 동작되도록 한다.
		if ( !pMaterial->m_rFXnType_PS30_DF )
			return;

		ID3DXEffect*  pFX = pMaterial->m_rFXnType_PS30_DF->m_rFX;
		if ( !pFX )
			return;

		D3DXHANDLE hTechniqueHandle(NULL);

		hTechniqueHandle = pMaterial->m_rFXnType_PS30_DF->m_hTechnique_runtime_2;
		if ( !hTechniqueHandle )
			return;

		if ( pParameter->m_pTextureCombined )
		{
			pFX->SetTexture( "g_LightMapDayTex", pParameter->m_pTextureCombined );
		}

		pFX->SetTechnique( hTechniqueHandle );

		D3DXVECTOR4 vLightMapUV_Offset4( pParameter->m_vLightMapUV_Offset.x, pParameter->m_vLightMapUV_Offset.y, 1.f, 1.f );
		pFX->SetVector( "g_vLightMapUV_Offset", &vLightMapUV_Offset4 );

		NSMaterialManager::SetFX_CommonParameter( pFX, pMaterial->m_rFXnType_PS30_DF->m_emFxType );

		NSMaterialManager::SetFX_CommonParameter_Small( pFX, pParameter->m_fAlpha, *pParameter->m_rmatWorld );

		// 렌더링한다.
		RenderGame
		( 
			pd3dDevice, 
			sMaterial_Data, 
			pMaterialSmall,
			pFX,
			emUnLMRenderMode,
			pParameter,
			unPass,
			bPSF
		);
	}
	else if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_NM && !bLightMapTest && (pParameter->m_nRenderMode==NSMaterialManager::EMRM_MATERIAL) )	// && !bDecal )
		//bMPShading && !bLightMapTest && (pParameter->m_nRenderMode==NSMaterialManager::EMRM_MATERIAL) )
	{
		ID3DXEffect* pFX(NULL);
		D3DXHANDLE hTechniqueHandle(NULL);
		NSMaterialManager::FX_TYPE	emFxType(NSMaterialManager::FX_TYPE_DEFAULT);
		if ( bMPShading )
		{
			if ( !pMaterial->m_rFXnType_LBuffer )
				return;

			pFX = pMaterial->m_rFXnType_LBuffer->m_rFX;
			if ( !pFX )
				return;

			emFxType = pMaterial->m_rFXnType_LBuffer->m_emFxType;
			hTechniqueHandle = pMaterial->m_rFXnType_LBuffer->m_hTechnique_runtime_3;
		}
		else
		{
			pFX = pMaterial->m_sFXnType.m_rFX;
			if ( !pFX )
				return;

			emFxType = pMaterial->m_sFXnType.m_emFxType;
			hTechniqueHandle = pMaterial->m_sFXnType.m_hTechnique_runtime_3;
		}

		
		if ( hTechniqueHandle )
		{
			if ( pParameter->m_pTextureIndirect )
			{
				pFX->SetTexture( "g_LightMapDayTex", pParameter->m_pTextureIndirect );
				pFX->SetTexture( "g_LightMapTex_DS_1st", pParameter->m_pTextureDirect_Shadow_1 );
				pFX->SetTexture( "g_LightMapTex_DS_2nd", pParameter->m_pTextureDirect_Shadow_2 );
			}
		}
		else
		{
			return;		
		}

		pFX->SetTechnique( hTechniqueHandle );

		D3DXVECTOR4 vLightMapUV_Offset4( pParameter->m_vLightMapUV_Offset.x, pParameter->m_vLightMapUV_Offset.y, 1.f, 1.f );
		pFX->SetVector( "g_vLightMapUV_Offset", &vLightMapUV_Offset4 );

		NSMaterialManager::SetFX_CommonParameter( pFX, emFxType );

		NSMaterialManager::SetFX_CommonParameter_Small( pFX, pParameter->m_fAlpha, *pParameter->m_rmatWorld );

		BOOL bAlphaTex(FALSE);
		TEXTURE_ALPHA sTEXTURE_ALPHA(EMTT_NORMAL);
		sTEXTURE_ALPHA = m_textureRes.GetAlphaType();
		if ( sTEXTURE_ALPHA == EMTT_ALPHA_SOFT || sTEXTURE_ALPHA == EMTT_ALPHA_SOFT01 || sTEXTURE_ALPHA == EMTT_ALPHA_SOFT02 )
		{
			bAlphaTex = TRUE;
		}

		// 렌더링한다.
		// 3.0 일경우 알맞은 Pass 를 구하도록 한다.
		//size_t unPassTemp = unPass;

		int nNewPass(unPass);
		if ( bMPShading )
		{
			nNewPass = NSShadowLightUtil::GetPassFX_PS_3_0_TYPE_USED_MPShading( pParameter->m_pTriangleLinkLight->m_emPS_3_0_Type, pParameter->m_pTriangleLinkLight->m_wwLightID );
			if ( nNewPass == -1 )
				return;		// 이건 렌더링 할 필요가 없다.			
		}
		else
		{
			nNewPass = NSShadowLightUtil::GetPassFX_PS_3_0_TYPE_USED( pParameter->m_pTriangleLinkLight->m_emPS_3_0_Type, pParameter->m_pTriangleLinkLight->m_wwLightID );
		}

		RenderGame
		( 
			pd3dDevice, 
			sMaterial_Data, 
			pMaterialSmall,
			pFX,
			emUnLMRenderMode,
			pParameter,
			nNewPass,
			bPSF
		);
	}
	else
	{
		ID3DXEffect* pFX = pMaterial->m_sFXnType.m_rFX;
		if ( !pFX )
			return;

		D3DXHANDLE hTechniqueHandle(NULL);

		if ( pParameter->m_nRenderMode ==  NSMaterialManager::EMRM_TOOL )
		{
			hTechniqueHandle = pFX->GetTechniqueByName("tool_deffered");
			if ( hTechniqueHandle )
			{
				NSMaterialManager::SetFX_SpotLightEDITMODE( pFX );
				pFX->SetFloat( "g_fVoxel", 0.f );
			}
		}
		else if ( bLightMapTest )
		{
			hTechniqueHandle = pFX->GetTechniqueByName("lightmaptest");

			pFX->SetTexture( "g_LightMapDayTex", pParameter->m_pTextureCombined );
		}
		else
		{
			if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_NM )
			{
				hTechniqueHandle = pMaterial->m_sFXnType.m_hTechnique_runtime_3;
			}

			if ( !hTechniqueHandle )
			{
				if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_DF )
				{
					hTechniqueHandle = pMaterial->m_sFXnType.m_hTechnique_runtime_2;
				}
				if ( !hTechniqueHandle && RENDERPARAM::g_emGroundQulity >= TnL_PS_1_1 )
				{
					hTechniqueHandle = pMaterial->m_sFXnType.m_hTechnique_runtime_1;
				}
			}

			if ( bMPShading )
			{
				if ( pParameter->m_pTextureCombined )
				{
					pFX->SetTexture( "g_LightMapDayTex", pParameter->m_pTextureCombined );
				}
			}
		}

		if ( !hTechniqueHandle )
			return;

		pFX->SetTechnique( hTechniqueHandle );

		D3DXVECTOR4 vUV_Multiply(1.f,1.f,1.f,1.f);
		if ( NSLightMapFX::g_emRenderType == NSLIGHTMAPDEF::LMRT_CHECKER )
		{
			LPDIRECT3DTEXTUREQ pTextureCombined = pParameter->m_pTextureCombined;

			NSLightMapFX::GetLightMapCheker( pTextureCombined, vUV_Multiply );
			pFX->SetVector( "g_vLightMapUV_Multiply", &vUV_Multiply );

			pFX->SetTexture( "g_LightMapDayTex", pTextureCombined );
		}

		D3DXVECTOR4 vLightMapUV_Offset4( pParameter->m_vLightMapUV_Offset.x, pParameter->m_vLightMapUV_Offset.y, 1.f, 1.f );
		pFX->SetVector( "g_vLightMapUV_Offset", &vLightMapUV_Offset4 );

		NSMaterialManager::SetFX_CommonParameter( pFX, pMaterial->m_sFXnType.m_emFxType );

		NSMaterialManager::SetFX_CommonParameter_Small( pFX, pParameter->m_fAlpha, *pParameter->m_rmatWorld );

		// 렌더링한다.
		RenderGame
		( 
			pd3dDevice, 
			sMaterial_Data, 
			pMaterialSmall,
			pFX,
			emUnLMRenderMode,
			pParameter,
			unPass,
			bPSF
		);
	}
}

void DxEffectClothForMaterial::RenderGame( LPDIRECT3DDEVICEQ pd3dDevice, 
										  MATERIAL_DATA sMaterial_Data, 
										  const MaterialSmall* pMaterialSmall,
										  LPD3DXEFFECT pFX,
										  NSMaterialManager::UNLM_RENDER_MODE emUnLMRenderMode,
										  PieceParameter* pParameter,
										  size_t unPass,
										  BOOL bPSF )
{
	const LightMapBoxColorData* pLMBCData = &pParameter->m_sLMBCData;

	{
		NSMaterialManager::SetFX_Parameter( sMaterial_Data, pFX );

		if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_NM )
		{
			if ( pParameter->m_pTriangleLinkLight->m_emPS_3_0_Type != PS_3_0_TYPE_ZERO &&
				pParameter->m_pTriangleLinkLight->m_emPS_3_0_Type != PS_3_0_TYPE_CSM )
			{
				NSMaterialManager::SetFX_SpotLight( pParameter->m_pTriangleLinkLight->m_wwLightID, *pParameter->m_rmatWorld, pFX );
			}

			pFX->SetFloat( "g_fSpecularPower", m_fSpotSpecPower );
			pFX->SetFloat( "g_fSpecularIntensity", m_fSpotSpecMulti );	

			if ( emUnLMRenderMode == NSMaterialManager::EMUNLM_RM_AMBIENTCUBE )
			{
				pFX->SetVectorArray( "g_vAmbientCube", pLMBCData->m_pAmbientCube, LightMapBoxColor::TEST_COUNT );
				pFX->SetFloatArray( "g_arrayfAmbientCubePL1", pLMBCData->m_fAmbientCubeSpot1, LightMapBoxColor::TEST_COUNT );
				pFX->SetFloatArray( "g_arrayfAmbientCubePL2", pLMBCData->m_fAmbientCubeSpot2, LightMapBoxColor::TEST_COUNT );
			}
			else if ( emUnLMRenderMode == NSMaterialManager::EMUNLM_RM_VOXELCOLOR )
			{
				pFX->SetFloat( "g_fVoxel", 1.f );
				pFX->SetVector( "g_vVoxelColor", pLMBCData->m_pVoxelColor );
				pFX->SetFloat( "g_fSpotShadowLight1_VoxelColorPower", pLMBCData->m_pSpot1_Spot2->x );
				pFX->SetFloat( "g_fSpotShadowLight2_VoxelColorPower", pLMBCData->m_pSpot1_Spot2->y );
			}
		}
		else
		{
			if ( emUnLMRenderMode == NSMaterialManager::EMUNLM_RM_AMBIENTCUBE )
			{
				pFX->SetVectorArray( "g_vAmbientCube", pLMBCData->m_pAmbientCube, LightMapBoxColor::TEST_COUNT );
			}
			else if ( emUnLMRenderMode == NSMaterialManager::EMUNLM_RM_VOXELCOLOR )
			{
				pFX->SetFloat( "g_fVoxel", 1.f );
				pFX->SetVector( "g_vVoxelColor", pLMBCData->m_pVoxelColor );
			}
		}

		if ( m_dwFlags & ENSTM_CUBEMAP )
		{
			if ( NSPieceQuickSort::g_bBakeRAD )
			{
				// RAD 작업시에는 CubeMap 이 작동하지 않도록 한다.
				pFX->SetFloat( "g_fCubeMapValue", 0.f );
			}
			else
			{
				pFX->SetFloat( "g_fCubeMapValue", m_fCubeMapValue );
			}
		}
		else
		{
			pFX->SetFloat( "g_fCubeMapValue", 0.f );
		}

		if ( m_dwFlags & ENSTM_DECAL_BEFORE )
		{
			if ( m_dwFlags & ENSTM_DECAL_NORMAL )
			{
				pFX->SetFloat( "g_fDecalNormal", 1.f );
			}
			else
			{
				pFX->SetFloat( "g_fDecalNormal", 0.f );
			}
		}
	}

	LPDIRECT3DINDEXBUFFER9 pIB(NULL);
	m_pLocalMeshSRC->GetIndexBuffer( &pIB );

	UINT nVertSize = D3DXGetDeclVertexSize( pMaterialSmall->m_pELMT, 0 );

	pd3dDevice->SetFVF( 0L );
	pd3dDevice->SetStreamSource( 0, DxDynamicVB::g_sVB_PNTI.pVB, 0, nVertSize );
	pd3dDevice->SetIndices( pIB );

	{
		TEXTURE_ALPHA sTEXTURE_ALPHA(EMTT_NORMAL);

		int nTexType(0);
		DWORD dwAlphaTestEnable(0L);
		DWORD dwAlphaBlendEnable(0L);
		//if ( sMaterial_Data.m_bDefaultMaterial )
		{
			// Note : Texture Loading
			ThreadLoadingTexture( pd3dDevice );

			sTEXTURE_ALPHA = m_textureRes.GetAlphaType();

			pFX->SetTexture( "g_BaseTexture", m_textureRes.GetTexture() );

			if ( m_dwFlags & ENSTM_MASK_MAP )
			{
				// 없다면 로딩해본다.
				if ( !m_pMaskTexRes.GetTexture() )
				{
					TSTRING strName = m_textureRes.GetFileName();

					strName = sc::util::GetSpecularName( strName, _T("_mask") );

					m_pMaskTexRes = TextureManager::GetInstance().LoadTexture(
						strName,
						false,
						TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
						0,
						D3DFMT_UNKNOWN,
						TEXTURE_RESIZE_1024,
						TEXTURE_LAND,
						true,
						FALSE );
				}

				// 있다면 Mask 맵을 없다면 흰색으로 작동된다.
				if ( m_pMaskTexRes.GetTexture() )
				{
					pFX->SetTexture( "g_MaskTexture", m_pMaskTexRes.GetTexture() );
				}
				else
				{
					pFX->SetTexture( "g_MaskTexture", NSMaterialManager::GetMaskTexture() );
				}
			}
			else
			{
				pFX->SetTexture( "g_MaskTexture", NSMaterialManager::GetMaskTexture() );
			}

			if ( sTEXTURE_ALPHA == EMTT_ALPHA_HARD )
			{
				nTexType = 1;

				pd3dDevice->GetRenderState( D3DRS_ALPHATESTENABLE,	&dwAlphaTestEnable );
				pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	TRUE );
			}
			//else if ( !bPSF && (sTEXTURE_ALPHA == EMTT_ALPHA_SOFT || sTEXTURE_ALPHA == EMTT_ALPHA_SOFT01 || sTEXTURE_ALPHA == EMTT_ALPHA_SOFT02) )
			//{
			//	nTexType = 2;

			//	pd3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE,	&dwAlphaBlendEnable );
			//	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
			//	pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			//	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			//}
		}

		pd3dDevice->SetVertexDeclaration( pMaterialSmall->m_pDCRT );

		//--------------------------------------------------------------------------------------------------
		// Front
		UINT uiStartVertex = 0;
		{
			DWORD dwVertexSizeFULL = m_dwNumVertices*nVertSize;
			if( !DxDynamicVB::g_sVB_PNTI.IsValidSize( dwVertexSizeFULL ) )
			{
				return;
			}

			uiStartVertex = DxDynamicVB::g_sVB_PNTI.CopyToVB(
				m_pVertArray,
				dwVertexSizeFULL,
				m_dwNumVertices );

			UINT numPasses;
			pFX->Begin( &numPasses, 0 );
			if( unPass < numPasses )
			{
				pFX->BeginPass( unPass );
				{
					pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, uiStartVertex, 0, m_dwNumVertices, 0, m_dwNumIndices/3 );
				}
				pFX->EndPass();
			}
			pFX->End();
		}

		//--------------------------------------------------------------------------------------------------
		// Back
		{
			//if ( emUnLMRenderMode == NSMaterialManager::EMUNLM_RM_AMBIENTCUBE )
			//{
			//	D3DXVECTOR4 arrayAmbientCube[6];
			//	arrayAmbientCube[0] = pLMBCData->m_pAmbientCube[1];
			//	arrayAmbientCube[1] = pLMBCData->m_pAmbientCube[0];
			//	arrayAmbientCube[2] = pLMBCData->m_pAmbientCube[3];
			//	arrayAmbientCube[3] = pLMBCData->m_pAmbientCube[2];
			//	arrayAmbientCube[4] = pLMBCData->m_pAmbientCube[5];
			//	arrayAmbientCube[5] = pLMBCData->m_pAmbientCube[4];
			//	pFX->SetVectorArray( "g_vAmbientCube", &arrayAmbientCube[0], LightMapBoxColor::TEST_COUNT );
			//}

			DWORD dwNormalOffset(0L);
			for ( DWORD i=0; i<MAXD3DDECLLENGTH; ++i )
			{
				if ( pMaterialSmall->m_pELMT[i].Stream == 0xff )
					break;

				if ( pMaterialSmall->m_pELMT[i].Usage == D3DDECLUSAGE_NORMAL )
				{
					dwNormalOffset = pMaterialSmall->m_pELMT[i].Offset;
					break;
				}
			}

			// 노멀 값 변경
			D3DXVECTOR3* pNor(NULL);
			for( int i = 0 ; i < (int)m_dwNumVertices ; i++ )
			{
				pNor = reinterpret_cast<D3DXVECTOR3*>( m_pVertArray+(i*nVertSize)+dwNormalOffset );
				(*pNor) = -(*pNor);
			}

			DWORD dwVertexSizeFULL = m_dwNumVertices*nVertSize;
			if( !DxDynamicVB::g_sVB_PNTI.IsValidSize( dwVertexSizeFULL ) )
			{
				return;
			}

			UINT uiStartVertex = DxDynamicVB::g_sVB_PNTI.CopyToVB(
				m_pVertArray,
				dwVertexSizeFULL,
				m_dwNumVertices );

			// 노멀 값 되돌림.
			for( int i = 0 ; i < (int)m_dwNumVertices ; i++ )
			{
				pNor = reinterpret_cast<D3DXVECTOR3*>( m_pVertArray+(i*nVertSize)+dwNormalOffset );
				(*pNor) = -(*pNor);
			}

			DWORD dwCullMode;
			pd3dDevice->GetRenderState( D3DRS_CULLMODE, &dwCullMode );

			if ( dwCullMode == D3DCULL_CCW )
			{
				pd3dDevice->SetRenderState ( D3DRS_CULLMODE, D3DCULL_CW );
			}
			else if ( dwCullMode == D3DCULL_CW )
			{
				pd3dDevice->SetRenderState ( D3DRS_CULLMODE, D3DCULL_CCW );
			}

			UINT numPasses;
			pFX->Begin( &numPasses, 0 );
			if( unPass < numPasses )
			{
				pFX->BeginPass( unPass );
				{
					pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, uiStartVertex, 0, m_dwNumVertices, 0, m_dwNumIndices/3 );
				}
				pFX->EndPass();
			}
			pFX->End();

			pd3dDevice->SetRenderState ( D3DRS_CULLMODE, dwCullMode );
		}

		{
			if ( nTexType == 1 )
			{
				pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, dwAlphaTestEnable );
			}
			//else if ( nTexType == 2 )
			//{
			//	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	dwAlphaBlendEnable );
			//}
		}
	}
}

void DxEffectClothForMaterial::RenderCascadeShadow( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !m_rMaterialSmall )
		return;

	pd3dDevice->SetVertexDeclaration( m_rMaterialSmall->m_pDCRT );

	ThreadLoadingTexture( pd3dDevice );

	TEXTURE_ALPHA sTEXTURE_ALPHA(EMTT_NORMAL);
	sTEXTURE_ALPHA = m_textureRes.GetAlphaType();

	if ( sTEXTURE_ALPHA == EMTT_NORMAL )
	{
		NSShadowRenderForMaterial::SetTexture( NSMaterialManager::GetWhiteTexture() );
	}
	else
	{
		LPDIRECT3DTEXTUREQ pTexture = m_textureRes.GetTexture();
		NSShadowRenderForMaterial::SetTexture( pTexture );
	}

	NSShadowRenderForMaterial::CommitChanges();

	{
		LPDIRECT3DINDEXBUFFER9 pIB(NULL);
		m_pLocalMeshSRC->GetIndexBuffer( &pIB );

		UINT nVertSize = D3DXGetDeclVertexSize( m_rMaterialSmall->m_pELMT, 0 );

		pd3dDevice->SetFVF( 0L );
		pd3dDevice->SetStreamSource( 0, DxDynamicVB::g_sVB_PNTI.pVB, 0, nVertSize );
		pd3dDevice->SetIndices( pIB );

		DWORD dwVertexSizeFULL = m_dwNumVertices*nVertSize;
		if( !DxDynamicVB::g_sVB_PNTI.IsValidSize( dwVertexSizeFULL ) )
		{
			return;
		}

		UINT uiStartVertex = 0;
		uiStartVertex = DxDynamicVB::g_sVB_PNTI.CopyToVB(
			m_pVertArray,
			dwVertexSizeFULL,
			m_dwNumVertices );

		pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, uiStartVertex, 0, m_dwNumVertices, 0, m_dwNumIndices/3 );
	}
}

void DxEffectClothForMaterial::DeleteClothData( int a )
{
	if ( a == 0 )
	{
		SAFE_DELETE_ARRAY( m_pVertArray );
		SAFE_RELEASE ( m_pLocalMeshSRC );
	}
	
	if ( a == 1 )
	{
		SAFE_DELETE_ARRAY( m_pVertArray );
	}
}

void DxEffectClothForMaterial::XFileLoad( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pLocalMesh, BOOL bLightMapUV, DWORD dwLightMapIndex )
{	
	HRESULT hr(S_OK);
	DeleteClothData( 0 );

	m_dwNumVertices = pLocalMesh->GetNumVertices();//정점의 수
	m_dwNumIndices  = pLocalMesh->GetNumFaces()*3; //인덱스의 수

	pLocalMesh->CloneMeshFVF( pLocalMesh->GetOptions(), pLocalMesh->GetFVF(), pd3dDevice, &m_pLocalMeshSRC );

	if ( VERTEX_WORLD::FVF == pLocalMesh->GetFVF() )
	{
		VERTEX_WORLD* pVerticesSrc(NULL);
		m_pLocalMeshSRC->LockVertexBuffer( NULL, (VOID**)&pVerticesSrc );
		{
			for( DWORD i = 0 ; i < m_dwNumVertices ; i++ )
			{
				D3DXVec3TransformCoord ( &pVerticesSrc[i].vPos, &pVerticesSrc[i].vPos, &m_matWorld );
			}

			// LightMapUV 를 저장시켜놓는다.
			m_vecLightMapUV.empty();
			if ( bLightMapUV )
			{
				if ( (dwLightMapIndex+1) * 0x100 <= (pLocalMesh->GetFVF()&D3DFVF_TEXCOUNT_MASK) )
				{
					for( DWORD i = 0 ; i < pLocalMesh->GetNumVertices() ; i++ )
					{
						m_vecLightMapUV.push_back( pVerticesSrc[i].vTex[dwLightMapIndex] );
					}
				}
			}
		}
		m_pLocalMeshSRC->UnlockVertexBuffer();
	}

	CreateVertexAndNodeTable( pd3dDevice, m_pLocalMeshSRC );
}

void DxEffectClothForMaterial::BaseBuffToDynamicBuff ( LPDIRECT3DDEVICEQ pd3dDevice )
{	
	DeleteClothData( 1 );
	
	CreateVertexAndNodeTable( pd3dDevice, m_pLocalMeshSRC );

	//	Note : 다시 "락" 걸기.
	//
	for ( DWORD i=0; i<m_dwNumLockIndex; i++ )
	{
		WORD k = m_pLockIndex[i];
		m_pParticle[ k ].bLocked = TRUE;
	}
}

void DxEffectClothForMaterial::CreateVertexAndNodeTable( LPDIRECT3DDEVICEQ pd3dDevice, const LPD3DXMESH pLocalMesh )
{
	D3DXVECTOR3 *pPosition;
	m_dwNumVertices = pLocalMesh->GetNumVertices();//정점의 수
	m_dwNumIndices  = pLocalMesh->GetNumFaces()*3; //인덱스의 수

	pPosition = new D3DXVECTOR3[m_dwNumVertices];

	DWORD dwFVFSize = D3DXGetFVFVertexSize( pLocalMesh->GetFVF() );

	BYTE* pVerticesSrc(NULL);
	pLocalMesh->LockVertexBuffer( NULL, (VOID**)&pVerticesSrc );
	{
		for( DWORD i = 0 ; i < m_dwNumVertices ; i++ )
		{
			pPosition[i] = *reinterpret_cast<D3DXVECTOR3*>( pVerticesSrc + (i*dwFVFSize) );
			//D3DXVec3TransformCoord ( &pPosition[i], &pPosition[i], &m_matWorld );
		}
	}
	pLocalMesh->UnlockVertexBuffer();

	WORD* pwIndexSrc(NULL);
	pLocalMesh->LockIndexBuffer( NULL, (VOID**)&pwIndexSrc );
	{
		CreateVertex( m_dwNumVertices, pPosition );
		CreateNodeTable( pPosition, m_dwNumIndices, pwIndexSrc ); //노드 테이블 만들기
	}
	pLocalMesh->UnlockIndexBuffer();

	m_Spring.Init( m_fCoEfficient, DAMPING ); //스프링 상수 초기화 

	SAFE_DELETE_ARRAY ( pPosition );
}

//파티클의 갯수 만큼 내려가기에 파티클의 갯수로 한다.
//노드 테이블 수를 리턴한다.
void DxEffectClothForMaterial::CreateNodeTable( D3DXVECTOR3 *pVertices, DWORD dwNumIndices, WORD *pIndices )
{	
	NODE *Temp_Node = new NODE[ dwNumIndices ];//임시 노드임
	
	//----------- 인덱스 버퍼에서 3개의 노드로 분리 
	int k = 0;
	int i = 0;

	for( i = 0; i < (int)dwNumIndices ; i+=3 )
	{
		for( int j = 0 ; j < 3 ; j++, k++ )
		{
			Temp_Node[k].Index1 = pIndices[i+j];
			Temp_Node[k].Index2 = pIndices[i + (j+1)%3];
		}
	}

	//------------중복 플래그 초기화------------------------
	BOOL *pDuplicate_Table = new BOOL[ dwNumIndices ];
	for( i = 0 ; i < (int)dwNumIndices ; i++ )
			pDuplicate_Table[i] = FALSE;	

	//------------- 2 단계 중복 플래그 부분 검색 -------------------
	int count = 0; //중복 플래그 갯수 카운터 
    for( int m = 0 ; m < (int)dwNumIndices ; m++ )
	{
		for( int n = m + 1 ; n < (int)dwNumIndices ; n++ )
		{
			if( ((Temp_Node[m].Index1 == Temp_Node[n].Index1) && (Temp_Node[m].Index2 == Temp_Node[n].Index2 ))  //괄호에 주의 하자.!!!
				|| ((Temp_Node[m].Index1 == Temp_Node[n].Index2) && (Temp_Node[m].Index2 == Temp_Node[n].Index1 )) )
			{
				pDuplicate_Table[n] = TRUE;
				count++;
			}			
		}
	}

	 //------------------------- 중복 검사 ---------------------------//	 
	SAFE_DELETE_ARRAY ( m_pNodeTable );
	m_pNodeTable = new NODE[ dwNumIndices - count ];
	m_iNodeCount = dwNumIndices - count; //중복 제거한 노드수 

	int index = 0;
	for( int c = 0 ; c < (int)dwNumIndices ; c++ )
	{
		  if( !pDuplicate_Table[c] )
		  {
			  m_pNodeTable[index ] = Temp_Node[c];
			  index++;
		  }
	 }
	     
	 SAFE_DELETE_ARRAY ( Temp_Node );
	 SAFE_DELETE_ARRAY ( pDuplicate_Table );

	 
	 //-------------------------------- 중복 검사 완결 ------------------------------------------//
	 //파티클이 정점과 같은 위치로 되어 있다고 생각하에...파티클의 위치와 정점의 인덱스가 동일함.
	 int index1, index2;
	 D3DXVECTOR3 vLength;
	 for( int i = 0 ; i < m_iNodeCount ; i++ )
	 {
		 index1 = m_pNodeTable[i].Index1;
		 index2 = m_pNodeTable[i].Index2;
		 vLength = pVertices[index2] - pVertices[index1] ; 
		 m_pNodeTable[i].fLength = D3DXVec3Length( &vLength );	 
	 } 
}

void DxEffectClothForMaterial::SetParticle(int index, D3DXVECTOR3 &vPosition )
{ 
	 //파티클 초기화 ^^
	 	m_pParticle[ index ].vPosition   = vPosition;
		m_pParticle[ index ].vVelocity   = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		m_pParticle[ index ].vAccelerate = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		m_pParticle[ index ].vForce      = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		m_pParticle[ index ].bLocked = FALSE;	 	
}

//이 함수는 vertex의 index, vertex buffer를 생성하는 부분이다.
//이 안에서는 파티클도 셋하여 진다. vertex의 정보와 파티클은 같은 데이타를 공유하게 된다.
// cl 화일 로딩 후에 함수를 로딩한다...
BOOL DxEffectClothForMaterial::CreateVertex( DWORD dwNumVertices, D3DXVECTOR3* pVertices )
{
	// 파티클을 생성하는 부분이다 ^^
	SAFE_DELETE_ARRAY( m_pParticle );	
	m_pParticle = new PARTICLE[ dwNumVertices ];

	//파티클 정점 셋 
	for( int i = 0 ; i < (int)dwNumVertices ; i++ )
	{
		SetParticle( i, pVertices[i] );
	}

	return TRUE;
}

//고라운드 쉐이딩 노말값 계산하여 주는 함수
//한점과 연결된 모든 정점의 인덱스를 이용하여서 노말을 구한다. 
void  DxEffectClothForMaterial::ComputeNormal(int nVertices,  BYTE *pVertices, DWORD dwVertSize, DWORD dwNormalOffset, int nIndices, WORD *pIndices )
{
	D3DXVECTOR3 v1, v2;
	D3DXVECTOR3 vNormal;
	int i;

	D3DXVECTOR3* pPos1(NULL);
	D3DXVECTOR3* pPos2(NULL);
	D3DXVECTOR3* pNormal(NULL);

	// 법선의 누적으로 인한 초기화를 해주는 부분 
	// 삼각형의 두 선분은 두선분의 길이로 구하는 것이다.
	for( i = 0 ; i < nVertices ; i++ )
	{
		pNormal = reinterpret_cast<D3DXVECTOR3*>( pVertices+(dwVertSize*i)+dwNormalOffset );
		(*pNormal) = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	}

	//각 삼각형의 법선을 구하는 부분
    for( i = 0 ; i < nIndices ; i+= 3 )
	{		
		for( int j = 0; j < 3 ; j++ )
		{
			pPos1 = reinterpret_cast<D3DXVECTOR3*>( pVertices+(dwVertSize*pIndices[ (i+1+j)%3 + i ]) );
			pPos2 = reinterpret_cast<D3DXVECTOR3*>( pVertices+(dwVertSize*pIndices[i+j]) );
			v1 = *pPos1 - *pPos2;

			pPos1 = reinterpret_cast<D3DXVECTOR3*>( pVertices+(dwVertSize*pIndices[ (i+2+j)%3 + i ]) );
			v2 = *pPos1 - *pPos2;

			D3DXVec3Cross( &vNormal, &v1, &v2 );

			pNormal = reinterpret_cast<D3DXVECTOR3*>( pVertices+(dwVertSize*pIndices[i+j])+dwNormalOffset );
			*pNormal += vNormal;		
		}
	}

	// Normalize 하는 부분 
	for( i = 0 ; i < nVertices ; i++ )
	{
		pNormal = reinterpret_cast<D3DXVECTOR3*>( pVertices+(dwVertSize*i)+dwNormalOffset );
		D3DXVec3Normalize( pNormal, pNormal );	
	}
}


/*inline float DxEffectClothForMaterial::Random( int nMin, int nMax )
{
	srand( nMax );	
	return (float)(((rand()%nMax)+nMin)%nMax);
}*/

void DxEffectClothForMaterial::SetParticleLock( int index , BOOL bLook )
{
	if ( (m_pParticle == NULL) )
        return;
	if ( WORD(index) >= m_dwNumVertices )
        return;

	if (bLook)
	{
		for (DWORD i=0; i<m_dwNumLockIndex; i++)
        {
			if (m_pLockIndex[i] == index)
                return;
        }

		WORD* pNewLockIndex = new WORD[m_dwNumLockIndex+1];
		
		if ( m_pLockIndex && m_dwNumLockIndex>0 )
			memcpy ( pNewLockIndex, m_pLockIndex, sizeof(WORD)*m_dwNumLockIndex );
		
		pNewLockIndex[m_dwNumLockIndex] = index;

		SAFE_DELETE_ARRAY ( m_pLockIndex );

		m_dwNumLockIndex++;
	
		m_pLockIndex = new WORD[m_dwNumLockIndex];
		memcpy ( m_pLockIndex, pNewLockIndex, sizeof(WORD)*m_dwNumLockIndex );

		SAFE_DELETE_ARRAY ( pNewLockIndex );
	}
	else
	{
		BOOL bLockIndex = FALSE;
		for ( DWORD i=0; i<m_dwNumLockIndex; i++ )
			if ( m_pLockIndex[i] == index ) bLockIndex = TRUE;

		if ( !bLockIndex )	return;

		if ( m_dwNumLockIndex > 1 )
		{
			WORD* pNewLockIndex = new WORD[m_dwNumLockIndex-1];

			DWORD j=0;
			for ( DWORD i=0; i<m_dwNumLockIndex; i++ )
			{
				if ( m_pLockIndex[i] != index )
					pNewLockIndex[j++] = m_pLockIndex[i];
			}

			SAFE_DELETE_ARRAY ( m_pLockIndex );

			m_dwNumLockIndex--;

			m_pLockIndex = new WORD[m_dwNumLockIndex];
			memcpy ( m_pLockIndex, pNewLockIndex, sizeof(WORD)*m_dwNumLockIndex );

			SAFE_DELETE_ARRAY ( pNewLockIndex );
		}
		else
		{
			m_dwNumLockIndex = 0;
			SAFE_DELETE_ARRAY ( m_pLockIndex );
		}
	}

	m_pParticle[ index ].bLocked = bLook;
}

void DxEffectClothForMaterial::SetIndexLock()
{
	SAFE_DELETE_ARRAY ( m_pTempLockIndex );

	if ( m_dwNumLockIndex )
	{
		m_pTempLockIndex = new WORD[m_dwNumLockIndex];
		memcpy ( m_pTempLockIndex, m_pLockIndex, sizeof(WORD)*m_dwNumLockIndex );
	}
}

void DxEffectClothForMaterial::GetIndexLock()
{
	SAFE_DELETE_ARRAY ( m_pLockIndex );

	if ( m_dwNumLockIndex )
	{
		m_pLockIndex = new WORD[m_dwNumLockIndex];
		memcpy ( m_pLockIndex, m_pTempLockIndex, sizeof(WORD)*m_dwNumLockIndex );
	}
	SAFE_DELETE_ARRAY ( m_pTempLockIndex );
}

char* DxEffectClothForMaterial::GetStrList( DWORD i )
{
	static char szStrEdit[256];

	DWORD dwVertSize = D3DXGetFVFVertexSize ( m_pLocalMeshSRC->GetFVF() );

	D3DXVECTOR3* pPosition(NULL);
	BYTE* pVerticesSrc(NULL);
	m_pLocalMeshSRC->LockVertexBuffer( NULL, (VOID**)&pVerticesSrc );
	{
		WORD k = m_pLockIndex[i];
		pPosition = reinterpret_cast<D3DXVECTOR3*>( pVerticesSrc + (k*dwVertSize) );
		StringCchPrintf( szStrEdit, 256, "[%d] %4.2f, %4.2f, %4.2f", k, pPosition->x, pPosition->y, pPosition->z );
	}
	m_pLocalMeshSRC->UnlockVertexBuffer();

	return szStrEdit;
}

DWORD DxEffectClothForMaterial::GetdwVertex( DWORD i )
{
	WORD k = m_pLockIndex[i];

	return k;
}

void DxEffectClothForMaterial::GetAABBSize ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin )
{
	D3DXVECTOR3 vBoxMax;
	D3DXVECTOR3 vBoxMin;

	D3DXMATRIX	matIdentity;
	D3DXMatrixIdentity ( &matIdentity );
	float fSize;

	// Note: x, y 평면상에서의 최대, 최소를 찾아준다.
	BYTE* pVerticesSrc(NULL);
	m_pLocalMeshSRC->LockVertexBuffer( NULL, (VOID**)&pVerticesSrc );
	{
		COLLISION::CalculateBoundingBox( &matIdentity, vBoxMax, vBoxMin, pVerticesSrc, m_dwNumVertices, m_pLocalMeshSRC->GetFVF() );
	}
	m_pLocalMeshSRC->UnlockVertexBuffer();

	//	Note:	z 좌표에 대해서 길이를 구하여서 증감, 감소를 해줌으로 해주면 박스의 최대 최소를 구할수 있다.
	//			위 충돌 부분이 한 평면에 대해서 가져 오는 것이기에 같은 좌표가 존재한다.
	//			그 좌표가 늘려야 하는 좌표가 되는 것이다.	
	vMin	= vBoxMin;
	vMax	= vBoxMax;	

	if( vBoxMax.x == vBoxMin.x ) 
	{	
		fSize = vBoxMax.y - vBoxMin.y;
		
		// Note: 바람과 평면의 방향이 다르면 바람의 세기 만큼 그쪽 방향을 느려주어서 충돌 박스를 만들어 준다.
		if( D3DXVec3Dot( &vBoxMin, &m_vWindDirection ) < 0 )
		{
			vMin.y -= m_fWindForce*0.3f;
			vMax.y += m_fWindForce*0.3f;

			vMin.z -= m_fWindForce*0.3f;
			vMax.z += m_fWindForce*0.3f;
		}            
		
		vMin.x -= fSize*m_fWindForce*0.2f; 
		vMax.x += fSize*m_fWindForce*0.2f; 
		
			//	Note: y를 고정 해서 사용되는 경우에 
	}else if( vBoxMax.y == vBoxMin.y ) 
	{	
		fSize = vBoxMax.z - vBoxMin.z;

		// Note: 바람과 평면의 방향이 다르면 바람의 세기 만큼 그쪽 방향을 늘려주어서 충돌 박스를 만들어 준다.
		if( D3DXVec3Dot( &vBoxMin, &m_vWindDirection ) < 0 )
		{
			vMin.x -= m_fWindForce*0.3f;
			vMax.x += m_fWindForce*0.3f;

			vMin.z -= m_fWindForce*0.3f;
			vMax.z += m_fWindForce*0.3f;
		}            

		vMin.y -= fSize*0.7f;
		vMax.y += fSize*0.7f;

	}else if( vBoxMax.z == vBoxMin.z ) 
	{	
		fSize = vBoxMax.y - vBoxMin.y;

		// Note: 바람과 같은 방향이면 
		if( D3DXVec3Dot( &vBoxMin, &m_vWindDirection ) < 0 )
		{
			vMin.x -= m_fWindForce*0.3f;
			vMax.x += m_fWindForce*0.3f;

			vMin.y -= m_fWindForce*0.3f;
			vMax.y += m_fWindForce*0.3f;
		}	
		
		vMin.z -= fSize*m_fWindForce*0.2f; 
		vMax.z += fSize*m_fWindForce*0.2f;
	};
}

BOOL DxEffectClothForMaterial::IsDetectDivision ( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin )
{
	D3DXVECTOR3 vBoxMax;
	D3DXVECTOR3 vBoxMin;

	D3DXMATRIX	matIdentity;
	D3DXMatrixIdentity ( &matIdentity );

	BYTE* pVerticesSrc(NULL);
	m_pLocalMeshSRC->LockVertexBuffer( NULL, (VOID**)&pVerticesSrc );
	{
		COLLISION::CalculateBoundingBox( &matIdentity, vBoxMax, vBoxMin, pVerticesSrc, m_dwNumVertices, m_pLocalMeshSRC->GetFVF() );
	}
	m_pLocalMeshSRC->UnlockVertexBuffer();

	D3DXVECTOR3 vCenter = ( vBoxMax + vBoxMin ) / 2.0f;

	return COLLISION::IsWithInPoint(vDivMax,vDivMin,vCenter);
}

void DxEffectClothForMaterial::SaveBuffer ( sc::SerialFile &SFile )
{
	//	읽지 않고 스킵용으로 사용됨.
	SFile.BeginBlock();
	{
		SFile << m_dwNumLockIndex;
		if ( m_dwNumLockIndex )
			SFile.WriteBuffer ( m_pLockIndex, sizeof(WORD)*m_dwNumLockIndex );

		SFile.BeginBlock();
		{
			DWORD dwFVF = m_pLocalMeshSRC->GetFVF();
			SFile << m_dwNumVertices;
			SFile << m_dwNumIndices;
			SFile << dwFVF;

			if ( m_dwNumVertices == 0 || m_dwNumIndices == 0 || dwFVF == 0 )
			{
				SFile.EndBlock();
				SFile.EndBlock();
				return;
			}

			DWORD dwFVFSize = D3DXGetFVFVertexSize( dwFVF );
			BYTE* pVerticesSrc(NULL);
			m_pLocalMeshSRC->LockVertexBuffer( NULL, (VOID**)&pVerticesSrc );
			SFile.WriteBuffer ( pVerticesSrc, dwFVFSize * m_dwNumVertices );
			m_pLocalMeshSRC->UnlockVertexBuffer();

			WORD* pIndices(NULL);
			m_pLocalMeshSRC->LockIndexBuffer( NULL, (VOID**)&pIndices );
			SFile.WriteBuffer ( pIndices, sizeof(WORD) * m_dwNumIndices );
			m_pLocalMeshSRC->UnlockIndexBuffer();

			SFile << m_iNodeCount;
			SFile.WriteBuffer ( m_pNodeTable, sizeof(NODE)*m_iNodeCount );

			SFile << static_cast<DWORD>( m_vecLightMapUV.size() );
			for ( DWORD i=0; i<m_vecLightMapUV.size(); ++i )
			{
				SFile.WriteBuffer ( m_vecLightMapUV[i], sizeof(D3DXVECTOR2) );
			}

			SFile << m_dwFlags;
			SFile << m_fCubeMapValue;
			SFile << m_fSpotSpecPower;
			SFile << m_fSpotSpecMulti;
		}
		SFile.EndBlock();
	}
	SFile.EndBlock();
}

void DxEffectClothForMaterial::LoadBufferSet ( sc::SerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwBuffSize;

	SFile >> dwBuffSize;

	SFile >> m_dwNumLockIndex;
	SAFE_DELETE_ARRAY(m_pLockIndex);
	m_pLockIndex = new WORD[m_dwNumLockIndex];
	SFile.ReadBuffer ( m_pLockIndex, sizeof(WORD)*m_dwNumLockIndex );

	SFile >> dwBuffSize;
	SFile.SetOffSet( SFile.GetfTell() + dwBuffSize );

	//	Note : 스프링 설정.
	//
	m_Spring.Init( m_fCoEfficient, DAMPING ); //스프링 상수 초기화 

	//	Note : Base VB, IB를 VB, IB 로 업대이트 하기 위해서 필요하다.
	BaseBuffToDynamicBuff ( pd3dDevice );
}

void DxEffectClothForMaterial::LoadBuffer ( sc::SerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT(pd3dDevice);

	DWORD dwBuffSize;
	PBYTE pbBuff;

	SFile >> dwBuffSize; //	"읽지 않고 스킵" 할 때 사용되므로 실제 로드에서는 무의미.

	SFile >> dwBuffSize;
	m_dwNumLockIndex = dwBuffSize;

	if ( m_dwNumLockIndex>0 )
	{
		pbBuff = new BYTE[sizeof(WORD)*dwBuffSize];
		SFile.ReadBuffer ( pbBuff, sizeof(WORD)*dwBuffSize );

		//	Note : 고정된 버텍스(파티클) 지정.
		//
		SAFE_DELETE_ARRAY(m_pLockIndex);
		if ( m_dwNumLockIndex > 0 )
		{
			m_pLockIndex = new WORD[m_dwNumLockIndex];
			memcpy ( m_pLockIndex, pbBuff, sizeof(WORD)*m_dwNumLockIndex );
		}

		SAFE_DELETE_ARRAY(pbBuff);
	}

	SFile >> dwBuffSize;

	DWORD dwFVF(0L);
	SFile >> m_dwNumVertices;
	SFile >> m_dwNumIndices;
	SFile >> dwFVF;

	if ( m_dwNumVertices == 0 || m_dwNumIndices == 0 || dwFVF == 0 )
	{
		return;
	}

	DWORD dwFVFSize = D3DXGetFVFVertexSize( dwFVF );

	SAFE_RELEASE ( m_pLocalMeshSRC );
	D3DXCreateMeshFVF ( m_dwNumIndices/3, m_dwNumVertices, D3DXMESH_MANAGED, dwFVF, pd3dDevice, &m_pLocalMeshSRC );

	BYTE* pVerticesSrc(NULL);
	m_pLocalMeshSRC->LockVertexBuffer( NULL, (VOID**)&pVerticesSrc );
	SFile.ReadBuffer ( pVerticesSrc, dwFVFSize * m_dwNumVertices );
	m_pLocalMeshSRC->UnlockVertexBuffer();

	WORD* pIndices(NULL);
	m_pLocalMeshSRC->LockIndexBuffer( NULL, (VOID**)&pIndices );
	SFile.ReadBuffer ( pIndices, sizeof(WORD) * m_dwNumIndices );
	m_pLocalMeshSRC->UnlockIndexBuffer();

	{
		SFile >> m_iNodeCount;
		SAFE_DELETE_ARRAY(m_pNodeTable);
		m_pNodeTable = new NODE[m_iNodeCount];
		SFile.ReadBuffer ( m_pNodeTable, sizeof(NODE)*m_iNodeCount );
	}

	m_vecLightMapUV.clear();
	DWORD dwSize;
	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; ++i )
	{
		D3DXVECTOR2 vUV;
		SFile.ReadBuffer( vUV, sizeof(D3DXVECTOR2) );
		m_vecLightMapUV.push_back( vUV );
	}

	SFile >> m_dwFlags;
	SFile >> m_fCubeMapValue;
	SFile >> m_fSpotSpecPower;
	SFile >> m_fSpotSpecMulti;

	//	Note : 스프링 설정.
	//
    m_Spring.Init( m_fCoEfficient, DAMPING ); //스프링 상수 초기화 

	//	Note : Base VB, IB를 VB, IB 로 업대이트 하기 위해서 필요하다.
	BaseBuffToDynamicBuff ( pd3dDevice );
}

HRESULT DxEffectClothForMaterial::CloneDataMAT ( DxEffectBase* pSrcEffect, LPDIRECT3DDEVICEQ pd3dDevice, const CMaterialSetting* pMaterialSetting )
{
	DxEffectClothForMaterial* pSrcCloth = (DxEffectClothForMaterial*) pSrcEffect;

	//	Note : 먼저 초기화.
	DeleteClothData( 0 );

	m_dwNumVertices = pSrcCloth->m_dwNumVertices;
	m_dwNumIndices = pSrcCloth->m_dwNumIndices;

	// Property 설정.
	m_bPlay				= pSrcCloth->m_bPlay;
	m_bEachRender		= pSrcCloth->m_bEachRender; 
	m_iNodeCount		= pSrcCloth->m_iNodeCount;
	m_fCoEfficient		= pSrcCloth->m_fCoEfficient;
	m_vWindDirection	= pSrcCloth->m_vWindDirection;
	m_fWindForce		= pSrcCloth->m_fWindForce;
	m_fGravity			= pSrcCloth->m_fGravity;
	m_textureRes		= pSrcCloth->m_textureRes;
	m_strMaterialName	= pSrcCloth->m_strMaterialName;
	m_vecLightMapUV		= pSrcCloth->m_vecLightMapUV;
	StringCchCopy( m_szTexture, MAX_PATH, pSrcCloth->m_szTexture );

	pSrcCloth->m_pLocalMeshSRC->CloneMeshFVF( pSrcCloth->m_pLocalMeshSRC->GetOptions(), pSrcCloth->m_pLocalMeshSRC->GetFVF(), pd3dDevice, &m_pLocalMeshSRC );

	CreateVertexAndNodeTable( pd3dDevice, m_pLocalMeshSRC );

	//	Note : 고정된 버텍스(파티클) 복사.
	//
	m_dwNumLockIndex = pSrcCloth->m_dwNumLockIndex;
	if ( pSrcCloth->m_pLockIndex )
	{
		SAFE_DELETE_ARRAY(m_pLockIndex);
		m_pLockIndex = new WORD[m_dwNumLockIndex];
		memcpy ( m_pLockIndex, pSrcCloth->m_pLockIndex, sizeof(WORD)*m_dwNumLockIndex );
	}

	//	Note : 파티클 "락" 걸기.
	//
	for ( DWORD i=0; i<m_dwNumLockIndex; i++ )
	{
		WORD k = m_pLockIndex[i];
		m_pParticle[ k ].bLocked = TRUE;
	}


	if ( pMaterialSetting )
	{
		const NSMaterialSetting::SETTING* pSetting = pMaterialSetting->GetMaterialSetting( pSrcCloth->m_strMaterialName );

		m_dwFlags = pSetting->GetFlag( m_vecLightMapUV.empty() );

		// Note : CubeMap Value
		m_fCubeMapValue = pSetting->m_fCubeMapValue;

		// Note : Spot Specular Power
		m_fSpotSpecPower = pSetting->m_fSpotSpecPower;

		// Note : Spot Specular Multi
		m_fSpotSpecMulti = pSetting->m_fSpotSpecMulti;
	}
	else
	{
		m_dwFlags = pSrcCloth->m_dwFlags;

		// Note : CubeMap Value
		m_fCubeMapValue = pSrcCloth->m_fCubeMapValue;

		// Note : Spot Specular Power
		m_fSpotSpecPower = pSrcCloth->m_fSpotSpecPower;

		// Note : Spot Specular Multi
		m_fSpotSpecMulti = pSrcCloth->m_fSpotSpecMulti;
	}

	return S_OK;
}



