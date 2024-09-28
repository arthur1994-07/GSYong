#include "pch.h"

#include "../../../VisualMaterialLib/Util/NSNearPointLight.h"

#include "../../../SigmaCore/Util/Util.h"
#include "../../../SigmaCore/Math/Random.h"

#include "../../DxCommon9/DXUTmisc.h"
#include "../../DxEffect/TexEff/DxTexEff.h"
#include "../../DxEffect/Mesh/DxStaticMesh.h"
#include "../../G-Logic/GLPeriod.h"
#include "../../DxMeshs/FrameMesh/NSVERTEXNORTEX_FX.h"
#include "../../DxTools/Occlusion/NSOcclusionQueryFastRender.h"
#include "../../DxTools/Occlusion/NSOcclusionOctree.h"
#include "../../DxTools/Occlusion/DxOcclusionQuery.h"
#include "../../DxTools/Material/NSShadowRenderForMaterial.h"
#include "../../DxTools/Material/NSMaterialManager.h"
#include "../../DxTools/Texture/NSTexture16Bit.h"
#include "../../DxTools/Light/NSShadowLight.h"
#include "../../DxTools/Light/DxShadowLight.h"
#include "../../DxTools/Light/DxLightMan.h"
#include "../../DxTools/DxEnvironment.h"
#include "../../DxTools/DxMethods.h"
#include "../../DxTools/Collision.h"
#include "../../DxLand/LightMap/LightMapObjectVEC.h"
#include "../../DxLand/LightMap/LightMapBoxColor.h"
#include "../../DxLand/LightMap/NSLightMapFX.h"
#include "../../DxLand/LightMap/NSLightMap.h"
#include "../../DxLand/DxSetLandMan.h"
#include "../LightMap/LightMapObject.h"

#include "./NSPieceInstancing.h"
#include "./DxPieceContainer.h"
#include "./NSPieceQuickSort.h"
#include "./DxPieceStaticAni.h"
#include "./DxPieceTexEff.h"
#include "./DxPieceEdit.h"
#include "./DxPieceAABB.h"
#include "./DxPieceEff.h"

#include "./DxPiece.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// -----------------------------------------------------------------------------------------------------------------------------------------
//													D	x		P	i	e	c	e
//
//		보면 PDXEFFECTBASE 이	DxStaticAniFrame과 DxPieceEff에 둘다 들어 있다.. 하지만 2개의 동작은 틀리다.
//		DxStaticAniFrame는 같은 것을 써서 동작은 같지만 부하를 덜 먹고,
//		DxPieceEff는 틀린것을 써서 동작은 틀리지만 부하는 많이 먹는다.
// -----------------------------------------------------------------------------------------------------------------------------------------

DxPiece::DxPiece( LPDIRECT3DDEVICEQ pd3dDevice )
	: m_pd3dDevice(pd3dDevice)
	, m_dwID(0L)
    , m_vMax(-FLT_MAX,-FLT_MAX,-FLT_MAX)
	, m_vMin(FLT_MAX,FLT_MAX,FLT_MAX)
	, m_vStaticMax(FLT_MAX,FLT_MAX,FLT_MAX)
	, m_vStaticMin(FLT_MAX,FLT_MAX,FLT_MAX)
	, m_vCenter(0.f,0.f,0.f)
    , m_fRadius(1.f)
	, m_fRadiusSquare(1.f)
	//, m_fAlpha(0.f)
	, m_bAddAlpha(TRUE)
	, m_bBlendAlpha(FALSE)
	, m_fTime(-10.f)
	, m_fAniTime(0.f)
	, m_ElapsedTime(0.f)
	//, m_rPieceEdit(NULL)
	, m_rStaticAniFrame(NULL)
	, m_pPieceEff(NULL)
	, m_pAniTexEff(NULL)
	, m_pNext(NULL)
	, m_pTreeNext(NULL)
	, m_pSortNext(NULL)
    , m_wwStartTexel(0,0)     // 시작 Texel
    , m_wwTextureTexel(1,1)   // Width, Height ( Texture )
	, m_vLightMapUV_Rate(1.f,1.f) // 
    , m_rPiecePSF(NULL)
    , m_dwFlags(0L)
	, m_pLightMapBoxColor(NULL)
	, m_fVisibleRadius(DEFAULT_VISIBLE_RADIUS)
	, m_fInvisibleRadius(DEFAULT_INVISIBLE_RADIUS)
	, m_bExceptLowOption(FALSE)
	, m_pvecsvecLightMapObj(NULL)
	, m_vVoxelColorUsed(1.f,1.f,1.f,1.f)

	, m_pd3dQuery( NULL )
	, m_bQueryTest( FALSE )
{
	m_fAniTime = sc::Random::RANDOM_POS() * 100.f * UNITANIKEY_PERSEC;

	D3DXMatrixIdentity( &m_matWorld );
	D3DXMatrixIdentity( &m_matWorldOrig );

	m_pLightMapBoxColorFullData = new LightMapBoxColorFullData;

	// PieceParameter 셋팅.
	m_sParameter.m_rmatWorld = &m_matWorld;

	ResetInstancingLink();
}

DxPiece::~DxPiece()
{
	CleanUp();

	SAFE_DELETE( m_pLightMapBoxColorFullData );
}

void DxPiece::CleanUp()
{
    m_dwFlags = 0L;
	m_rStaticAniFrame = NULL;
	SAFE_DELETE( m_pPieceEff );
	SAFE_DELETE( m_pAniTexEff );
	SAFE_DELETE( m_pLightMapBoxColor );
	SAFE_DELETE( m_pvecsvecLightMapObj );

	// 같이 다녀야한다.
	{
		m_textureResCombined.Clear();
		m_textureResDirect_Shadow_1.Clear();
		m_textureResDirect_Shadow_2.Clear();
		m_textureResIndirect.Clear();
		m_sParameter.m_pTextureCombined = NULL;
		m_sParameter.m_pTextureDirect_Shadow_1 = NULL;
		m_sParameter.m_pTextureDirect_Shadow_2 = NULL;
		m_sParameter.m_pTextureIndirect = NULL;
	}

	m_sTriangleLinkLight.m_vecLightID.clear();
	m_sTriangleLinkLight.m_wwLightID = WORDWORD(-1,-1);

	m_bExceptLowOption = FALSE;

	m_vecAnimationMatrixBackUp.clear();

	ResetInstancingLink();
}

// ToolMode 시 호출된다.
void DxPiece::FrameMoveTOOL( const float fTime, const float fElapsedTime )
{
	m_fAniTime += fElapsedTime * UNITANIKEY_PERSEC;
	if( m_fAniTime > 1.0e15f )	
		m_fAniTime = 0.f;

	if( m_rStaticAniFrame )
		m_rStaticAniFrame->FrameMoveTOOL( m_pd3dDevice, fElapsedTime );

	if( m_pPieceEff )
		m_pPieceEff->FrameMove( fTime, fElapsedTime );

	if( m_pAniTexEff )
		m_pAniTexEff->FrameMove( fElapsedTime );

	m_sParameter.m_fAniTime = m_fAniTime;
	m_sParameter.m_fAlpha = m_fAlpha;

	//FrameMoveStep();

	//D3DXVECTOR3 vTemp = (m_vVoxelColor[0]*GLPeriod::GetInstance().GetBlendFact()) + 
	//					(m_vVoxelColor[1]*(1.f-GLPeriod::GetInstance().GetBlendFact()));
	//m_vVoxelColorUsed = D3DXVECTOR4 ( vTemp.x, vTemp.y, vTemp.z, 1.f );

	//for ( DWORD i=0; i<LightMapBoxColor::TEST_COUNT; ++i )
	//{
	//	m_avAmbientCube[2][i] = (m_avAmbientCube[0][i]*GLPeriod::GetInstance().GetBlendFact()) + 
	//							(m_avAmbientCube[1][i]*(1.f-GLPeriod::GetInstance().GetBlendFact())) * 0.5f;
	//}
	//// Disable
	//if( DxOcclusionQuery::g_bForceDisable )
	//{
	//	m_bQueryTest = FALSE;
	//	m_dwCullPixels = 1;
	//	return;
	//}

	//// m_bQueryTest는 카메라가 변동되었을 때만 
	//// 그리고 OcculsionCulling이 된다면 TRUE로 준다.
	//// 
	//// 만약 m_bQueryTest가 TRUE라도 카메라에 안 걸린다면 렌더하는 일은 없다.
	//if( m_bQueryTest )
	//{
	//	// Pixel 갯수를 얻는다.
	//	HRESULT hr = GetDataQuery( m_dwCullPixels );
	//	if( hr == S_OK )
	//	{
	//		// 1차검사 되었다고 2차검사가 제대로 완료 되었으므로 초기화 한다.
	//		m_bQueryTest = FALSE;
	//	}
	//}
}

// Thread 상에서 호출한다.
void DxPiece::FrameMoveTHREAD( const float fElapsedTime )
{
	m_fAniTime += fElapsedTime * UNITANIKEY_PERSEC;
	if( m_fAniTime > 1.0e15f )	
		m_fAniTime = 0.f;

	m_sParameter.m_fAniTime = m_fAniTime;
	m_sParameter.m_fAlpha = m_fAlpha;
}

// GameMode 시 호출된다.
void DxPiece::FrameMoveMAT( const float fTime, const float fElapsedTime )
{
	if( m_rStaticAniFrame )
		m_rStaticAniFrame->FrameMoveMAT( m_pd3dDevice, TRUE );

	if( m_pPieceEff )
		m_pPieceEff->FrameMove( fTime, fElapsedTime );

	// 이전 Piece를 위해 이 작업이 필요함.
	if ( m_pAniTexEff )
	{
		// Note : Texture Eff
		DxTextureEffMan::MAPTEXEFF* pmapTexEff = m_pAniTexEff->GetMapTexEff();
		for ( DxTextureEffMan::MAPTEXEFF::iterator iter = pmapTexEff->begin(); iter!=pmapTexEff->end(); ++iter )
		{
			DxTexEffBase* pCur = (*iter).second;
			while ( pCur )
			{
				pCur->FrameMove( fElapsedTime );
				pCur = pCur->m_pNext;
			}
		}
	}
}

void DxPiece::FrameMoveMAT_OnlyStaticAniFrame( float fElapsedTime )
{
	if( m_rStaticAniFrame )
		m_rStaticAniFrame->FrameMoveMAT( m_pd3dDevice, TRUE );

	// 이전 Piece를 위해 이 작업이 필요함.
	if ( m_pAniTexEff )
	{
		// Note : Texture Eff
		DxTextureEffMan::MAPTEXEFF* pmapTexEff = m_pAniTexEff->GetMapTexEff();
		for ( DxTextureEffMan::MAPTEXEFF::iterator iter = pmapTexEff->begin(); iter!=pmapTexEff->end(); ++iter )
		{
			DxTexEffBase* pCur = (*iter).second;
			while ( pCur )
			{
				pCur->FrameMove( fElapsedTime );
				pCur = pCur->m_pNext;
			}
		}
	}
}

void DxPiece::FrameMoveMAT_OnlyPieceEff( const float fTime, const float fElapsedTime )
{
	if( m_pPieceEff )
		m_pPieceEff->FrameMove( fTime, fElapsedTime );
}

// 강제로 DF 로 렌더된다면 그에 맞는 옵션값들을 변화시킨다.
void DxPiece::SetData_Force_TnL_PS_3_0_DF()
{
	m_pLightMapBoxColorFullData->FrameMoveRAD( m_vVoxelColorUsed, &m_avAmbientCube[0] );
}

//void DxPiece::FrameMove_Instancing( float fElapsedTime )
//{
//	if( m_pPieceEff )
//		m_pPieceEff->FrameMove( 0.f, fElapsedTime );
//}

//// Piece 갯수는 많지만 1fps 에 1개씩만 호출되도록 되어있다.
//void DxPiece::FrameMoveStep( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPSF )
//{
//	m_pLightMapBoxColorFullData->FrameMoveStep
//	( 
//		&m_sParameter.m_sLMBCData.m_pAmbientCube,
//		&m_sParameter.m_sLMBCData.m_fAmbientCubeSpot1,
//		&m_sParameter.m_sLMBCData.m_fAmbientCubeSpot2,
//		&m_sParameter.m_sLMBCData.m_pVoxelColor,
//		&m_sParameter.m_sLMBCData.m_pSpot1_Spot2
//	);
//
//	m_sParameter.m_pd3dDevice = pd3dDevice;
//
//	//m_sParameter.m_sLMBCData.m_pLightMapBoxColor = &m_vVoxelColorUsed;
//	//m_sParameter.m_sLMBCData.m_pAmbientCube = &m_avAmbientCube[0];
//
//	m_sParameter.m_vLightMapUV_Offset.x = static_cast<float>(m_wwStartTexel.wA) / m_wwTextureTexel.wA;
//	m_sParameter.m_vLightMapUV_Offset.y = static_cast<float>(m_wwStartTexel.wB) / m_wwTextureTexel.wB;
//
//	m_sParameter.m_nRenderMode = bPSF ? NSMaterialManager::EMRM_TOOL : NSMaterialManager::EMRM_MATERIAL;
//
//	m_sParameter.m_pTextureCombined			= m_textureResCombined.GetTexture();
//	m_sParameter.m_pTextureDirect_Shadow_1	= m_textureResDirect_Shadow_1.GetTexture();
//	m_sParameter.m_pTextureDirect_Shadow_2	= m_textureResDirect_Shadow_2.GetTexture();
//	m_sParameter.m_pTextureIndirect			= m_textureResIndirect.GetTexture();
//
//	m_sParameter.m_pTriangleLinkLight		= &m_sTriangleLinkLight;
//}

void DxPiece::Render( const LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPSF )
{
	if( !m_rStaticAniFrame )
		return;

	// Note : Point Light를 적용시킨다.
	RenderPointLight( pd3dDevice, bPSF );

	//if ( bPSF && m_rPieceEdit && m_rPieceEdit->GetFrameMesh() )
	//{
	//	CLIPVOLUME sCV = DxViewPort::GetInstance().GetClipVolume();
	//	m_rPieceEdit->RenderFrame( pd3dDevice, sCV, m_matWorld, FALSE );
	//}
	//else
	{
		// 텍스쳐 Matrix로 
		D3DXMATRIX matTexScale;
		{
			// Create texture scaling matrix:
			// | 압축x    0        0        0 |
			// | 0        압축y    0        0 |
			// | 0        0        0        0 |
			// | x        y        0        0 |

			// This will scale and offset -1 to 1 range of x, y
			// coords output by projection matrix to 0-1 texture
			// coord range.
			SecureZeroMemory( &matTexScale, sizeof( D3DMATRIX ) );
			matTexScale._31 = static_cast<float>(m_wwStartTexel.wA) / m_wwTextureTexel.wA; 
			matTexScale._32 = static_cast<float>(m_wwStartTexel.wB) / m_wwTextureTexel.wB;
			matTexScale._11 = 1.f;
			matTexScale._22 = 1.f;
			matTexScale._33 = 1.f;
		}

		// Note : Render
		NSLightMapFX::SetWorld( m_matWorld, m_fAlpha );
		NSVERTEXNORTEX_FX::SetWorld( m_matWorld, m_fAlpha );
		m_rStaticAniFrame->Render
		( 
			pd3dDevice, 
			m_matWorld, 
			m_fAniTime, 
			m_pAniTexEff->GetMapTexEff(), 
			matTexScale, 
			m_fAlpha,
			bPSF ? NSMaterialManager::EMRM_TOOL : NSMaterialManager::EMRM_MATERIAL,
			m_textureResCombined.GetTexture(),			// Day - 이전 버젼에서는 이렇게 쓰임.
			m_textureResDirect_Shadow_1.GetTexture(),	// Night - 이전 버젼에서는 이렇게 쓰임.
			FALSE
		);

		// Note : Saved of Location Matirx. Because Animation is Change to All Time. Is Alpha ?
		//			위치 매트릭스를 저장 시켜 놓는다. Animation이 계속 바뀌기 때문이다. 알파는 .??
		if( !m_pPieceEff )		return;
		m_pPieceEff->SetMatrix();
	}
}

void DxPiece::RenderHardAlpha( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPSF )
{
	if( !m_rStaticAniFrame )
		return;

	// Note : Point Light를 적용시킨다.
	RenderPointLight( pd3dDevice, bPSF );

	//if ( bPSF && m_rPieceEdit && m_rPieceEdit->GetFrameMesh() )
	//{
	//	CLIPVOLUME sCV = DxViewPort::GetInstance().GetClipVolume();
	//	m_rPieceEdit->RenderFrame( pd3dDevice, sCV, m_matWorld, FALSE );
	//}
	//else
	{
		// 텍스쳐 Matrix로 
		D3DXMATRIX matTexScale;
		{
			// Create texture scaling matrix:
			// | 압축x    0        0        0 |
			// | 0        압축y    0        0 |
			// | 0        0        0        0 |
			// | x        y        0        0 |

			// This will scale and offset -1 to 1 range of x, y
			// coords output by projection matrix to 0-1 texture
			// coord range.
			SecureZeroMemory( &matTexScale, sizeof( D3DMATRIX ) );
			matTexScale._31 = static_cast<float>(m_wwStartTexel.wA) / m_wwTextureTexel.wA; 
			matTexScale._32 = static_cast<float>(m_wwStartTexel.wB) / m_wwTextureTexel.wB;
			matTexScale._11 = 1.f;
			matTexScale._22 = 1.f;
			matTexScale._33 = 1.f;
		}

		// Note : Render
		NSLightMapFX::SetWorld( m_matWorld, m_fAlpha );
		NSVERTEXNORTEX_FX::SetWorld( m_matWorld, m_fAlpha );
		m_rStaticAniFrame->Render_HardAlpha
		( 
			pd3dDevice, 
			m_matWorld, 
			m_pAniTexEff->GetMapTexEff(), 
			matTexScale, 
			m_fAlpha,
			bPSF ? NSMaterialManager::EMRM_TOOL : NSMaterialManager::EMRM_MATERIAL,
			m_textureResCombined.GetTexture(),			// Day - 이전 버젼에서는 이렇게 쓰임.
			m_textureResDirect_Shadow_1.GetTexture(),		// Night - 이전 버젼에서는 이렇게 쓰임.
			FALSE
		);
	}
}

void DxPiece::RenderSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPSF )
{
	if( !m_rStaticAniFrame )
		return;

	// Note : Point Light를 적용시킨다.
	RenderPointLight( pd3dDevice, bPSF );

	//if ( bPSF && m_rPieceEdit )
	//	return;

    // 텍스쳐 Matrix로 
    D3DXMATRIX matTexScale;
    {
        // Create texture scaling matrix:
        // | 압축x    0        0        0 |
        // | 0        압축y    0        0 |
        // | 0        0        0        0 |
        // | x        y        0        0 |

        // This will scale and offset -1 to 1 range of x, y
        // coords output by projection matrix to 0-1 texture
        // coord range.
        SecureZeroMemory( &matTexScale, sizeof( D3DMATRIX ) );
        matTexScale._31 = static_cast<float>(m_wwStartTexel.wA) / m_wwTextureTexel.wA; 
        matTexScale._32 = static_cast<float>(m_wwStartTexel.wB) / m_wwTextureTexel.wB;
        matTexScale._11 = 1.f;
        matTexScale._22 = 1.f;
        matTexScale._33 = 1.f;
    }

	// Note : Render
    NSLightMapFX::SetWorld( m_matWorld, m_fAlpha );
    NSVERTEXNORTEX_FX::SetWorld( m_matWorld, m_fAlpha );
	m_rStaticAniFrame->Render_SoftAlpha
    ( 
        pd3dDevice, 
        m_matWorld, 
        m_pAniTexEff->GetMapTexEff(), 
        matTexScale,
        m_fAlpha,
		bPSF ? NSMaterialManager::EMRM_TOOL : NSMaterialManager::EMRM_MATERIAL,
		m_textureResCombined.GetTexture(),			// Day - 이전 버젼에서는 이렇게 쓰임.
		m_textureResDirect_Shadow_1.GetTexture(),		// Night - 이전 버젼에서는 이렇게 쓰임.
		FALSE
    );
}

BOOL DxPiece::IsMaterialMesh() const
{
	if( !m_rStaticAniFrame )
		return FALSE;

	return m_rStaticAniFrame->IsMaterialMesh();
}



//////////////////////////////////////////////////////////////////////////
//			MaterialSystem - Opaque - TnL - TnL_PS_1_1 이상
void DxPiece::RenderMS_Opaque( BOOL bMPShading, BOOL bReflect, float fHeight, BOOL bBlend )
{
	if( !m_rStaticAniFrame )
		return;

	//SetType_PS_3_0_USED();

	if ( !bMPShading )
	{
		if ( bReflect )
		{
			m_matWorld_Mirror = m_matWorld;
			m_matWorld_Mirror._42 = m_matWorld_Mirror._42 - (fHeight*2.f);
			D3DXMatrixMultiply_MIRROR( m_matWorld_Mirror );
			m_sParameter.m_rmatWorld = &m_matWorld_Mirror;
		}
		else
		{
			m_sParameter.m_rmatWorld = &m_matWorld;
		}
	}



	////////////////////////////////////////////////////////////////////////////
	////			애니메이션 계산을 적게하기 위한 Data 기입.
	////			2Pass 시 계산을 적게하기위함.
	//if ( bMPShading && !bReflect )
	//{
	//	m_pStaticAniFrame->SetAnimationMatrix( m_vecAnimationMatrixBackUp );
	//}



	//////////////////////////////////////////////////////////////////////////
	// Note : Render
	m_rStaticAniFrame->RenderMS_Opaque( m_sParameter, bMPShading, bBlend );



	////////////////////////////////////////////////////////////////////////////
	////					애니메이션 계산을 적게하기 위한 백업.
	//if ( !bMPShading && !bReflect )
	//{
	//	const std::vector<D3DXMATRIX*>& vecAnimationMatrix = m_pStaticAniFrame->GetAnimationMatrixPointer();

	//	m_vecAnimationMatrixBackUp.clear();
	//	DWORD dwAnimationNum = vecAnimationMatrix.size();
	//	if ( dwAnimationNum > m_vecAnimationMatrixBackUp.capacity() )
	//	{
	//		m_vecAnimationMatrixBackUp.reserve( dwAnimationNum );
	//	}

	//	for ( DWORD i=0; i<dwAnimationNum; ++i )
	//	{
	//		m_vecAnimationMatrixBackUp.push_back( *vecAnimationMatrix[i] );
	//	}
	//}



	//////////////////////////////////////////////////////////////////////////
	// Note : Saved of Location Matirx. Because Animation is Change to All Time. Is Alpha ?
	//			위치 매트릭스를 저장 시켜 놓는다. Animation이 계속 바뀌기 때문이다. 알파는 .??
	if( !m_pPieceEff )		
		return;

	m_pPieceEff->SetMatrix();
}

void DxPiece::RenderMS_Opaque_Animation( BOOL bMPShading, BOOL bReflect, float fHeight, BOOL bBlend )
{
	if( !m_rStaticAniFrame )
		return;

	//SetType_PS_3_0_USED();

	//if ( !bMPShading )
	//{
	//	if ( bReflect )
	//	{
	//		m_matWorld_Mirror = m_matWorld;
	//		m_matWorld_Mirror._42 = m_matWorld_Mirror._42 - (fHeight*2.f);
	//		D3DXMatrixMultiply_MIRROR( m_matWorld_Mirror );
	//		m_sParameter.m_rmatWorld = &m_matWorld_Mirror;
	//	}
	//	else
	//	{
	//		m_sParameter.m_rmatWorld = &m_matWorld;
	//	}
	//}

	bool bAniLoop(true);
	if ( m_dwFlags & EMPO_ANI_PLAY_ONCE )
	{
		bAniLoop = false;
	}



	//////////////////////////////////////////////////////////////////////////
	//			애니메이션 계산을 적게하기 위한 Data 기입.
	//			2Pass 시 계산을 적게하기위함.
	if ( bMPShading && !bReflect )
	{
		m_rStaticAniFrame->SetAnimationMatrix( m_vecAnimationMatrixBackUp );
	}



	//////////////////////////////////////////////////////////////////////////
	// Note : Render
	m_rStaticAniFrame->RenderMS_Opaque_Animation( m_sParameter, bMPShading, bReflect, fHeight, bBlend, bAniLoop );



	//////////////////////////////////////////////////////////////////////////
	//					애니메이션 계산을 적게하기 위한 백업.
	if ( !bMPShading && !bReflect )
	{
		const std::vector<D3DXMATRIX*>& vecAnimationMatrix = m_rStaticAniFrame->GetAnimationMatrixPointer();

		m_vecAnimationMatrixBackUp.clear();
		DWORD dwAnimationNum = vecAnimationMatrix.size();
		if ( dwAnimationNum > m_vecAnimationMatrixBackUp.capacity() )
		{
			m_vecAnimationMatrixBackUp.reserve( dwAnimationNum );
		}

		for ( DWORD i=0; i<dwAnimationNum; ++i )
		{
			m_vecAnimationMatrixBackUp.push_back( *vecAnimationMatrix[i] );
		}
	}



	////////////////////////////////////////////////////////////////////////////
	//// Note : Saved of Location Matirx. Because Animation is Change to All Time. Is Alpha ?
	////			위치 매트릭스를 저장 시켜 놓는다. Animation이 계속 바뀌기 때문이다. 알파는 .??
	//if( !m_pPieceEff )		
	//	return;

	//m_pPieceEff->SetMatrix();
}



//////////////////////////////////////////////////////////////////////////
//				MaterialSystem - Opaque - TnL - Fixed 
void DxPiece::RenderMS_Fixed()
{
	if( !m_rStaticAniFrame )
		return;

	//SetType_PS_3_0_USED();

	// Fixed Pipeline 을 위함.
	m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matWorld );

	m_sParameter.m_rmatWorld = &m_matWorld;

	//////////////////////////////////////////////////////////////////////////
	// Note : Render
	m_rStaticAniFrame->RenderMS_Opaque( m_sParameter, FALSE, FALSE );
	m_rStaticAniFrame->RenderMS_Opaque_Animation( m_sParameter, FALSE, FALSE, 0.f, FALSE );

	//////////////////////////////////////////////////////////////////////////
	// Note : Saved of Location Matirx. Because Animation is Change to All Time. Is Alpha ?
	//			위치 매트릭스를 저장 시켜 놓는다. Animation이 계속 바뀌기 때문이다. 알파는 .??
	if( !m_pPieceEff )
		return;

	m_pPieceEff->SetMatrix();
}



void DxPiece::RenderMS_HardAlpha( BOOL bMPShading, BOOL bBlend )
{
	if( !m_rStaticAniFrame )
		return;

	// Fixed Pipeline 을 위함.
	m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matWorld );

	// Note : Render
	m_rStaticAniFrame->RenderMS_HardAlpha( m_sParameter, bMPShading, bBlend );
}

void DxPiece::RenderMS_HardAlpha_Animation( BOOL bMPShading, BOOL bBlend )
{
	if( !m_rStaticAniFrame )
		return;

	// Note : Render
	m_rStaticAniFrame->RenderMS_HardAlpha_Animation( m_sParameter, bMPShading, bBlend );
}

void DxPiece::RenderEffFrame()
{
	// Note : Render
	if ( m_pPieceEff )
	{
		m_pPieceEff->RenderMS( m_sParameter.m_pd3dDevice, m_matWorld, &m_sParameter, FALSE );
	}
}

void DxPiece::RenderEffFrameForDeffered( BOOL bLBuffer, BOOL bReflect )
{
	// Note : Render
	if ( m_pPieceEff )
	{
		m_pPieceEff->RenderMS_Deffered( m_sParameter.m_pd3dDevice, m_matWorld, &m_sParameter, bLBuffer, bReflect );
	}
}

void DxPiece::RenderDecal()
{
	if( !m_rStaticAniFrame )
		return;

	// Note : Render
	m_rStaticAniFrame->RenderDecal( m_sParameter );
}

void DxPiece::RenderMS_SoftAlpha( BOOL bMPShading )
{		
	// Fixed Pipeline 을 위함.
	m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matWorld );

	// Note : Render
	if( m_rStaticAniFrame )
	{
		m_rStaticAniFrame->RenderMS_SoftAlpha( m_sParameter, bMPShading );
	}
}

void DxPiece::RenderMS_SoftAlphaForDeffered()
{
	// Note : Render
	if( m_rStaticAniFrame )
	{
		m_rStaticAniFrame->RenderMS_SoftAlphaForPSF( m_sParameter );
	}
}

void DxPiece::RenderMS_SoftAlphaForPSF()
{
	// Note : Render
	if( m_rStaticAniFrame )
	{
		m_rStaticAniFrame->RenderMS_SoftAlphaForPSF( m_sParameter );
	}

	// Note : Render
	if ( m_pPieceEff )
	{
		m_pPieceEff->RenderMSForPSF( m_sParameter.m_pd3dDevice, m_matWorld, &m_sParameter );
	}
}

void DxPiece::RenderMS_DecalPass2()
{
	if( !m_rStaticAniFrame )
		return;

	m_rStaticAniFrame->RenderMS_DecalPass2( m_sParameter );
}

void DxPiece::Render_PIECE_Custom_UNLIT_UNFOG()
{
	if( !m_rStaticAniFrame )
		return;

	m_rStaticAniFrame->Render_PIECE_Custom_UNLIT_UNFOG( m_sParameter );
}

void DxPiece::Render_PIECE_Water_CustomSoftAlpha()
{
	if( !m_rStaticAniFrame )
		return;

	m_rStaticAniFrame->Render_PIECE_Water_CustomSoftAlpha( m_sParameter );
}

//-----------------------------------------------------------------------------
void DxPiece::RenderEff( LPDIRECT3DDEVICEQ pd3dDevice, DxStaticMeshLightMap* pStaticMeshLightMap, DxFrameMesh* pFrameMesh, BOOL bPSF )
{
	if( !m_pPieceEff )
		return;

	// Note : Point Light를 적용시킨다.
	RenderPointLight( pd3dDevice, bPSF );

	// Note : Render
	m_pPieceEff->RenderSingleEff( pd3dDevice, m_matWorld, pStaticMeshLightMap, pFrameMesh, m_fAlpha );
}

void DxPiece::RenderEff_Instancing( LPDIRECT3DDEVICEQ pd3dDevice, DxStaticMeshLightMap* pStaticMeshLightMap )
{
	if( !m_pPieceEff )
		return;

	// Note : Render
	m_pPieceEff->RenderSingleEff( pd3dDevice, m_matWorld, pStaticMeshLightMap, NULL, m_fAlpha );
}

//////////////////////////////////////////////////////////////////////////
// 일단 여기서 Opaque 만 렌더링을 한다.
void DxPiece::RenderCascadeShadowOpaque()
{
	if( !m_rStaticAniFrame )
		return;

	//if( pCV && !COLLISION::IsCollisionVolume( *pCV, m_vCenter, m_fRadius ) )
	//	return;



	// 초기화
	NSShadowRenderForMaterial::SetWorld( m_matWorld );
	NSShadowRenderForMaterial::CommitChanges();



	//m_pStaticAniFrame->FrameMoveMAT( pd3dDevice );
	if ( m_rStaticAniFrame->IsTexType( EMTTF_OPAQUE|EMTTF_UNKNOWN ) )
	{
		// DxSingleMaterialMeshRenderOrder 데이터가 필수다.
		m_sParameter.m_rmatWorld = &m_matWorld;

		m_rStaticAniFrame->RenderCascadeShadowOpaque( m_sParameter.m_pd3dDevice );
	}

	//if ( m_pStaticAniFrame->IsTexType( EMTTF_FRAMEANI_ALPHA_HARD ) )
	//{
	//	vecPieceForFrameAniHardAlpha.push_back( this );
	//}



	////////////////////////////////////////////////////////////////////////////
	////					Instancing 등록 작업.
	//spPieceInstancing->Insert( m_pStaticAniFrame, &m_sParameter );



	// FrameAnimation Mesh
	if( m_rStaticAniFrame->IsTexType( EMTTF_FRAMEANI_OPAQUE ) )
	{
		//////////////////////////////////////////////////////////////////////////
		//			애니메이션 계산을 적게하기 위한 Data 기입.
		m_rStaticAniFrame->SetAnimationMatrix( m_vecAnimationMatrixBackUp );

		//////////////////////////////////////////////////////////////////////////
		//							Rendering
		m_rStaticAniFrame->RenderCascadeShadowOpaque_Animation( m_sParameter.m_pd3dDevice );
	}



	//// Replace Mesh
	//if ( m_pPieceEff )
	//{
	//	vecPieceForReplace.push_back( this );
	//}
}

void DxPiece::RenderCascadeShadowHardAlpha()
{
	if( !m_rStaticAniFrame )
		return;

	NSShadowRenderForMaterial::SetWorld( m_matWorld );
	NSShadowRenderForMaterial::CommitChanges();

	if ( m_rStaticAniFrame->IsTexType( EMTTF_ALPHA_HARD ) )
	{
		m_rStaticAniFrame->RenderCascadeShadowHardAlpha( m_sParameter.m_pd3dDevice );
	}

	if ( m_rStaticAniFrame->IsTexType( EMTTF_FRAMEANI_ALPHA_HARD ) )
	{
		//////////////////////////////////////////////////////////////////////////
		//			애니메이션 계산을 적게하기 위한 Data 기입.
		m_rStaticAniFrame->SetAnimationMatrix( m_vecAnimationMatrixBackUp );

		//////////////////////////////////////////////////////////////////////////
		//							Rendering
		m_rStaticAniFrame->RenderCascadeShadowHardAlpha_Animation( m_sParameter.m_pd3dDevice );
	}
}

void DxPiece::RenderCascadeShadowReplace()
{
	if ( !m_pPieceEff )
		return;

	NSShadowRenderForMaterial::SetWorld( m_matWorld );

	m_pPieceEff->RenderCascadeShadow( m_sParameter.m_pd3dDevice );
}

void DxPiece::Render_Reflect( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPSF )
{
	D3DXVECTOR3 vCenter = DxEnvironment::GetInstance().GetCenter();
	D3DXMATRIX matWorld = m_matWorld;
	matWorld._42 = matWorld._42 - (vCenter.y*2.f);
	D3DXMatrixMultiply_MIRROR( matWorld );

	// Note : Point Light를 적용시킨다.
	RenderPointLight( pd3dDevice, bPSF );

    // 텍스쳐 Matrix로 
    D3DXMATRIX matTexScale;
    {
        // Create texture scaling matrix:
        // | 압축x    0        0        0 |
        // | 0        압축y    0        0 |
        // | 0        0        0        0 |
        // | x        y        0        0 |

        // This will scale and offset -1 to 1 range of x, y
        // coords output by projection matrix to 0-1 texture
        // coord range.
        SecureZeroMemory( &matTexScale, sizeof( D3DMATRIX ) );
        matTexScale._31 = static_cast<float>(m_wwStartTexel.wA) / m_wwTextureTexel.wA; 
        matTexScale._32 = static_cast<float>(m_wwStartTexel.wB) / m_wwTextureTexel.wB;
        matTexScale._11 = 1.f;
        matTexScale._22 = 1.f;
        matTexScale._33 = 1.f;
    }

	// Note : Render
	if( m_rStaticAniFrame )
	{
        NSLightMapFX::SetWorld( matWorld, 1.f );
        NSVERTEXNORTEX_FX::SetWorld( matWorld, 1.f );
		m_rStaticAniFrame->Render
		( 
			pd3dDevice, 
			matWorld, 
			m_fAniTime, 
			m_pAniTexEff->GetMapTexEff(), 
			matTexScale, 
			1.f,
			bPSF ? NSMaterialManager::EMRM_TOOL : NSMaterialManager::EMRM_MATERIAL,
			m_textureResCombined.GetTexture(),			// Day - 이전 버젼에서는 이렇게 쓰임.
			m_textureResDirect_Shadow_1.GetTexture(),		// Night - 이전 버젼에서는 이렇게 쓰임.
			FALSE
		);

		m_rStaticAniFrame->Render_HardAlpha
		( 
			pd3dDevice, 
			matWorld, 
			m_pAniTexEff->GetMapTexEff(), 
			matTexScale, 
			1.f, 
			bPSF ? NSMaterialManager::EMRM_TOOL : NSMaterialManager::EMRM_MATERIAL,
			m_textureResCombined.GetTexture(),			// Day - 이전 버젼에서는 이렇게 쓰임.
			m_textureResDirect_Shadow_1.GetTexture(),		// Night - 이전 버젼에서는 이렇게 쓰임.
			FALSE
		);

		m_rStaticAniFrame->Render_SoftAlpha
		( 
			pd3dDevice, 
			matWorld, 
			m_pAniTexEff->GetMapTexEff(), 
			matTexScale, 
			1.f, 
			bPSF ? NSMaterialManager::EMRM_TOOL : NSMaterialManager::EMRM_MATERIAL,
			m_textureResCombined.GetTexture(),			// Day - 이전 버젼에서는 이렇게 쓰임.
			m_textureResDirect_Shadow_1.GetTexture(),		// Night - 이전 버젼에서는 이렇게 쓰임.
			FALSE
		);
	}

	// Note : Render
	if( m_pPieceEff )
	{
		m_pPieceEff->RenderSingleEff( pd3dDevice, matWorld, NULL, NULL, 1.f );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------
//			D	x		P	i	e	c	e		-		D	x		O	c	c	l	u	s	i	o	n		Q	u	e	r	y		시작
void DxPiece::Render_Query_Prepare( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV )
{
	if ( !m_pd3dQuery )
		return;

	if( !m_rStaticAniFrame )	
		return;

	if( !COLLISION::IsCollisionVolume( sCV, m_vCenter, m_fRadius ) )
		return;

	// DxSingleMaterialMeshRenderOrder 데이터가 필수다.
	m_rStaticAniFrame->FrameMoveMAT( pd3dDevice, TRUE );

	// Note : Render
	//			내가 필요한 것과 맞아서, 이 함수를 사용함.
	//			1. Opaque 만 렌더링
	//			2. 불필요한거 없이 필요한 것만 렌더링.
	NSOcclusionQueryFastRender::SetWorld( m_matWorld );
	m_rStaticAniFrame->Render_CharShadow( pd3dDevice, NULL );
}

void DxPiece::Render_Query( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV )
{
	if ( !m_pd3dQuery )
		return;

	if( !m_rStaticAniFrame )	
		return;

	if( !COLLISION::IsCollisionVolume( sCV, m_vCenter, m_fRadius ) )
		return;

	if ( NSOcclusionExceptIndex::IsHaveExceptIndexPiece( m_dwID ) )
		return;

	if( !m_bQueryTest )
	{
		// 1차검사 안되었으면 검사하고 플래그를 바꾼다.
		HRESULT hr(D3D_OK);
		hr = m_pd3dQuery->Issue( D3DISSUE_BEGIN );
		if ( hr != D3D_OK )
		{
			int a=0;
		}

		Render_Query2( pd3dDevice, sCV );

		hr = m_pd3dQuery->Issue( D3DISSUE_END );
		if ( hr != D3D_OK )
		{
			int a=0;
		}

		m_bQueryTest = TRUE;
	}
}

void DxPiece::Render_Query2( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV )
{
	// Note : Render
	NSOcclusionQueryFastRender::SetWorld( m_matWorld );
	m_rStaticAniFrame->Render_Query( m_sParameter );
}

VOID DxPiece::CreateQuery( LPDIRECT3DDEVICEQ pd3dDevice )
{
	DestroyQuery();

	pd3dDevice->CreateQuery( D3DQUERYTYPE_OCCLUSION, &m_pd3dQuery );
}

HRESULT DxPiece::GetDataQuery( DWORD& dwCullPixels )
{
	return m_pd3dQuery->GetData( (void*)&dwCullPixels, sizeof( DWORD ), 0 );
}

VOID DxPiece::DestroyQuery()
{
	SAFE_RELEASE( m_pd3dQuery );
	m_bQueryTest = FALSE;
}

void DxPiece::ResetOcclusionQuery()
{
	m_bQueryTest = FALSE;

	m_fAlpha = 1.f;
}

BOOL DxPiece::VisibleOcclusionQueryTest( LPDIRECT3DDEVICEQ pd3dDevice, DWORD& dwCullPixels )
{
	if ( !m_bQueryTest )
	{
		//// 반투명 처리되는 애들은 결국 있는 것들이기 때문에, 존재한다는 처리를 해준다.
		//if ( m_bAplhaBlend )
		//{
		//	dwCullPixels = 1;
		//	return TRUE;
		//}

		dwCullPixels = 0;
		return FALSE;
	}

	DWORD dwCullingDelay = timeGetTime();

	while ( GetDataQuery( dwCullPixels ) == S_FALSE )
	{
		//CLIPVOLUME& sCV = DxViewPort::GetInstance().GetClipVolume();
		//Render_Query2( pd3dDevice, sCV );

		D3DXVECTOR3 VertBox[8];
		VertBox[0] = D3DXVECTOR3( 100.f,	100.f,	100.f );
		VertBox[1] = D3DXVECTOR3( 100.f,	100.f,	0.f );
		VertBox[2] = D3DXVECTOR3( 0.f,		100.f,	100.f );
		VertBox[3] = D3DXVECTOR3( 0.f,		100.f,	0.f );
		VertBox[4] = D3DXVECTOR3( 100.f,	0.f,	100.f );
		VertBox[5] = D3DXVECTOR3( 100.f,	0.f,	0.f );
		VertBox[6] = D3DXVECTOR3( 0.f,		0.f,	100.f );
		VertBox[7] = D3DXVECTOR3( 0.f,		0.f,	0.f );
		pd3dDevice->DrawPrimitiveUP ( D3DPT_POINTLIST, 8, VertBox, 12 );
	}

	// 딜레이를 누적한다.
	DxOcclusionQuery::g_dwCullingDelay += timeGetTime() - dwCullingDelay;

	// 1차검사 되었다고 2차검사가 제대로 완료 되었으므로 초기화 한다.
	m_bQueryTest = FALSE;

	return TRUE;

	//// Pixel 갯수를 얻는다.
	//HRESULT hr = GetDataQuery( dwCullPixels );
	//if( hr == S_OK )
	//{
	//	// 1차검사 되었다고 2차검사가 제대로 완료 되었으므로 초기화 한다.
	//	m_bQueryTest = FALSE;
	//	return TRUE;
	//}

	//// S_OK 가 아니면 문제가 데이터 자체에 문제가 있는것이다.
	//// 보니깐 위치값이 무한대값으로 되어있다.
	//// 그냥 m_bQueryTest 값을 TRUE로 주지 말아서 검사를하지 말고 렌더링 검사도 하지 않게 한다.
	//dwCullPixels = 0;
	//return FALSE;
}

//			D	x		P	i	e	c	e		-		D	x		O	c	c	l	u	s	i	o	n		Q	u	e	r	y		끝
//--------------------------------------------------------------------------------------------------------------------------------------

void DxPiece::SetAABBBox()
{
	if ( !m_rStaticAniFrame && !m_pPieceEff )
	{
		m_vMax = D3DXVECTOR3(0.1f,0.1f,0.1f);
		m_vMin = D3DXVECTOR3(-0.1f,-0.1f,-0.1f);
	}
	else
	{
		m_vMax = D3DXVECTOR3(-FLT_MAX,-FLT_MAX,-FLT_MAX);
		m_vMin = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);

		if ( m_rStaticAniFrame )
		{
			D3DXVECTOR3 vMax(-FLT_MAX,-FLT_MAX,-FLT_MAX);
			D3DXVECTOR3 vMin(FLT_MAX,FLT_MAX,FLT_MAX);
			m_rStaticAniFrame->GetAABBBox( vMax, vMin );

			if ( m_vMax.x < vMax.x )	m_vMax.x = vMax.x;
			if ( m_vMax.y < vMax.y )	m_vMax.y = vMax.y;
			if ( m_vMax.z < vMax.z )	m_vMax.z = vMax.z;

			if ( m_vMin.x > vMin.x )	m_vMin.x = vMin.x;
			if ( m_vMin.y > vMin.y )	m_vMin.y = vMin.y;
			if ( m_vMin.z > vMin.z )	m_vMin.z = vMin.z;
		}

		if ( m_pPieceEff )
		{
			D3DXVECTOR3 vMax(-FLT_MAX,-FLT_MAX,-FLT_MAX);
			D3DXVECTOR3 vMin(FLT_MAX,FLT_MAX,FLT_MAX);
			m_pPieceEff->SetAABBBox( vMax, vMin );

			if ( m_vMax.x < vMax.x )	m_vMax.x = vMax.x;
			if ( m_vMax.y < vMax.y )	m_vMax.y = vMax.y;
			if ( m_vMax.z < vMax.z )	m_vMax.z = vMax.z;

			if ( m_vMin.x > vMin.x )	m_vMin.x = vMin.x;
			if ( m_vMin.y > vMin.y )	m_vMin.y = vMin.y;
			if ( m_vMin.z > vMin.z )	m_vMin.z = vMin.z;
		}
	}

	m_vStaticMax = m_vMax;
	m_vStaticMin = m_vMin;

    // Note : 자신에 맞도록 StaticAABB와 Center, Length를 바꾼다.
    D3DXMatrixAABBTransform( m_vStaticMax, m_vStaticMin, m_matWorld );
    m_vCenter = (m_vStaticMax+m_vStaticMin)*0.5f;
    D3DXVECTOR3 vHalf = m_vStaticMax - m_vCenter;
    m_fRadius = D3DXVec3Length( &vHalf );
    m_fRadiusSquare = m_fRadius*m_fRadius;
}

BOOL DxPiece::IsCollisionLine( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3* pNor, D3DXVECTOR3& vCollision, EMCOLLMODE emCullMode, BOOL bOnlyCamColl ) const
{
	if ( !m_rStaticAniFrame )
		return FALSE;

	D3DXMATRIX matInverse;
	D3DXMatrixInverse( &matInverse, NULL, &m_matWorld );

	D3DXVECTOR3 vNewStart = vStart;
	D3DXVECTOR3 vNewEnd = vEnd;
	D3DXVec3TransformCoord( &vNewStart, &vStart, &matInverse );
	D3DXVec3TransformCoord( &vNewEnd, &vEnd, &matInverse );

	BOOL bCollision(FALSE);
	bCollision = m_rStaticAniFrame->IsCollisionLine( vNewStart, vNewEnd, pNor, vCollision, emCullMode, bOnlyCamColl );

	if( bCollision )
	{
		D3DXVec3TransformCoord( &vEnd, &vNewEnd, &m_matWorld );
        vCollision = vEnd;

		if ( pNor )
		{
			D3DXVec3TransformNormal( pNor, pNor, &m_matWorld );
		}
	}

	return bCollision;
}

BOOL DxPiece::IsCollisionLineSimple( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode ) const
{
	if ( !m_rStaticAniFrame )
		return FALSE;

	D3DXMATRIX matInverse;
	D3DXMatrixInverse( &matInverse, NULL, &m_matWorld );

	D3DXVECTOR3 vNewStart = vStart;
	D3DXVECTOR3 vNewEnd = vEnd;
	D3DXVec3TransformCoord( &vNewStart, &vStart, &matInverse );
	D3DXVec3TransformCoord( &vNewEnd, &vEnd, &matInverse );

	BOOL bCollision(FALSE);
	if ( m_rStaticAniFrame->IsCollisionLineSimple( vNewStart, vNewEnd, emCullMode ) )
		return TRUE;

	return FALSE;
}


// Note : 충돌이 되면 바로 리턴한다.
BOOL DxPiece::IsCollisionLinePixel( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode, int nIndexUV )
{
    D3DXMATRIX matInverse;
	D3DXMatrixInverse( &matInverse, NULL, &m_matWorld );

	D3DXVECTOR3 vNewStart = vStart;
	D3DXVECTOR3 vNewEnd = vEnd;
	D3DXVec3TransformCoord( &vNewStart, &vStart, &matInverse );
	D3DXVec3TransformCoord( &vNewEnd, &vEnd, &matInverse );

	if ( m_rStaticAniFrame->IsCollisionLinePixel( pd3dDevice, vNewStart, vNewEnd, emCullMode, nIndexUV ) )
        return TRUE;

    return FALSE;
}

BOOL DxPiece::GetLightMapColor( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, DWORD& dwColor )
{
	D3DXMATRIX matInverse;
	D3DXMatrixInverse( &matInverse, NULL, &m_matWorld );

	D3DXVECTOR3 vNewStart = vStart;
	D3DXVECTOR3 vNewEnd = vEnd;
	D3DXVec3TransformCoord( &vNewStart, &vStart, &matInverse );
	D3DXVec3TransformCoord( &vNewEnd, &vEnd, &matInverse );

	// Note : UV로 좌표를 구한다.
	NSTexture16Bit::DATA* pDataDay(NULL);
	if ( m_textureResCombined.GetTexture() )
	{
		pDataDay = NSTexture16Bit::Insert( pd3dDevice, m_textureResCombined.GetFileName(), m_textureResCombined.GetTexture() );
	}

	if ( m_rStaticAniFrame->GetLightMapColor( pd3dDevice, vNewStart, vNewEnd, dwColor, pDataDay ) )
	{
		D3DXVec3TransformCoord( &vEnd, &vNewEnd, &m_matWorld );
		return TRUE;
	}

	return FALSE;
}

void DxPiece::ReloadTexEffVMFX( LPDIRECT3DDEVICEQ pd3dDevice )
{
    m_pAniTexEff->ReloadTexEffVMFX( pd3dDevice );
}

void DxPiece::CreateLightMapPiece( LPDIRECT3DDEVICEQ pd3dDevice, const TSTRING& strLightMapName )
{
	// LightMap을 굽지 않는다.
	if ( m_dwFlags & EMPO_UNSUPPORT_RAD )
		return;

	if ( !m_pvecsvecLightMapObj )
		m_pvecsvecLightMapObj = new VEC_SVEC_LIGHTMAPOBJ;

	// 제작자에 의해 LightMap UV 가 있는 경우.
	m_rStaticAniFrame->CreateLightMapUVPiece( pd3dDevice, 
											strLightMapName,
											m_strTextureCombined, 
											m_strTextureDirect_Shadow_1,
											m_strTextureDirect_Shadow_2,
											m_strTextureIndirect,
											*m_pvecsvecLightMapObj,
											WORDWORD(0,0) );

    for ( DWORD i=0; i<(*m_pvecsvecLightMapObj).size(); ++i )
    {
        if ( i == 0 )
        {
            m_wwTextureTexel = NSLightMap::GetTextureSize( (*m_pvecsvecLightMapObj)[i]->m_vecLightMapObject );
        }
    }

    // PSF 용도 Widht와 Height를 셋팅한다.
    if ( m_rPiecePSF )
    {
        m_rPiecePSF->SetLightMapWidthHeight( m_wwStartTexel, m_wwTextureTexel );
    }
}

void DxPiece::CreateLightMapPieceObject( LPDIRECT3DDEVICEQ pd3dDevice, const TSTRING& strLightMapName, BOOL bRan_1_0, float fCellSize )
{
	if ( m_dwFlags & EMPO_UNSUPPORT_RAD )
		return;

	if ( !m_pvecsvecLightMapObj )
		m_pvecsvecLightMapObj = new VEC_SVEC_LIGHTMAPOBJ;

	// 제작자에 의해 LightMap UV 가 있는 경우.
	m_rStaticAniFrame->CreateLightMapUVPieceObject( pd3dDevice, 
													strLightMapName,
													m_strTextureCombined, 
													m_strTextureDirect_Shadow_1,
													m_strTextureDirect_Shadow_2,
													m_strTextureIndirect,
													*m_pvecsvecLightMapObj,
													m_wwStartTexel,
													bRan_1_0, 
													fCellSize );

	m_vLightMapUV_Rate = D3DXVECTOR2( 1.f, 1.f );
	VEC_SVEC_LIGHTMAPOBJ& vecsvecLightMapObj = *m_pvecsvecLightMapObj;
	if ( !vecsvecLightMapObj.empty() )
	{
		if ( !vecsvecLightMapObj[0]->m_vecLightMapObject.empty() )
		{
			vecsvecLightMapObj[0]->m_vecLightMapObject[0]->GetLightMapUV_RATE( m_vLightMapUV_Rate );
		}
	}

	m_rStaticAniFrame->SetLightMapUV_Rate( m_vLightMapUV_Rate );
}

void DxPiece::LightMapCalculationBoxColor( LPDIRECT3DDEVICEQ pd3dDevice, DxStaticMeshCollBase* pStaticMesh, DxPieceManager* pPieceManager )
{
	SAFE_DELETE ( m_pLightMapBoxColor );
	m_pLightMapBoxColor = new LightMapBoxColor( m_vMin, m_vMax, m_matWorld );
	m_pLightMapBoxColor->LightMapCalculationBoxColor( pd3dDevice, pStaticMesh, pPieceManager, m_sTriangleLinkLight.m_wwLightID );

	m_pLightMapBoxColorFullData->LightMapCalculationBoxColor( m_pLightMapBoxColor );
}

void DxPiece::LoadLightMapForRAD( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// LightMap을 굽지 않는다.
	if ( m_dwFlags & EMPO_UNSUPPORT_RAD )
		return;

    LoadLightMap( pd3dDevice );
}

void DxPiece::LoadLightMap( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( m_rStaticAniFrame )
	{
		m_rStaticAniFrame->LoadLightMapOLD( pd3dDevice );
	}

	m_textureResCombined = TextureManager::GetInstance().LoadTexture(
		m_strTextureCombined,
		false,
		TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
		1,
		D3DFMT_UNKNOWN,
		TEXTURE_RESIZE_NONE,
		TEXTURE_LAND,
		false,
		FALSE );

	m_textureResDirect_Shadow_1 = TextureManager::GetInstance().LoadTexture(
		m_strTextureDirect_Shadow_1,
		false,
		TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
		1,
		D3DFMT_UNKNOWN,
		TEXTURE_RESIZE_NONE,
		TEXTURE_LAND,
		false,
		FALSE );

	m_textureResDirect_Shadow_2 = TextureManager::GetInstance().LoadTexture(
		m_strTextureDirect_Shadow_2,
		false,
		TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
		1,
		D3DFMT_UNKNOWN,
		TEXTURE_RESIZE_NONE,
		TEXTURE_LAND,
		false,
		FALSE );

	m_textureResIndirect = TextureManager::GetInstance().LoadTexture(
		m_strTextureIndirect,
		false,
		TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
		1,
		D3DFMT_UNKNOWN,
		TEXTURE_RESIZE_NONE,
		TEXTURE_LAND,
		false,
		FALSE );

	m_sParameter.m_pTextureCombined			= m_textureResCombined.GetTexture();
	m_sParameter.m_pTextureDirect_Shadow_1	= m_textureResDirect_Shadow_1.GetTexture();
	m_sParameter.m_pTextureDirect_Shadow_2	= m_textureResDirect_Shadow_2.GetTexture();
	m_sParameter.m_pTextureIndirect			= m_textureResIndirect.GetTexture();
}

void DxPiece::ReleaseLightMap()
{
	// LightMap을 굽지 않는다.
	if ( m_dwFlags & EMPO_UNSUPPORT_RAD )
		return;

	if ( !m_strTextureCombined.empty() )
	{
		TextureManager::GetInstance().DeleteData( m_strTextureCombined );
	}
	if ( !m_strTextureDirect_Shadow_1.empty() )
	{
		TextureManager::GetInstance().DeleteData( m_strTextureDirect_Shadow_1 );
	}
	if ( !m_strTextureDirect_Shadow_2.empty() )
	{
		TextureManager::GetInstance().DeleteData( m_strTextureDirect_Shadow_2 );
	}
	if ( !m_strTextureIndirect.empty() )
	{
		TextureManager::GetInstance().DeleteData( m_strTextureIndirect );
	}
}

void DxPiece::ResetAnimationTime_DisableLoopType()
{
	// Animation 시간은 랜덤하게 시작하지만 이게 셋팅 되어 있으면 0 부터 시작한다.
	if ( m_dwFlags & EMPO_ANI_PLAY_ONCE )
		m_fAniTime = 0.f;
}

void DxPiece::SetLightMapWidthHeight( WORDWORD wwStartTexel, WORDWORD wwTextureTexel )
{
    m_wwStartTexel = wwStartTexel;
    m_wwTextureTexel = wwTextureTexel;
}

void DxPiece::CreateAlphaTexture32Bit( LPDIRECT3DDEVICEQ pd3dDevice )
{
    m_rStaticAniFrame->CreateAlphaTexture32Bit( pd3dDevice );
}

void DxPiece::GetClosedPoint( D3DXVECTOR3& vCollisionInOut )
{
    if( !m_rStaticAniFrame )
        return;

    // Note : m_matInverse 를 미리 구해 놓음.
	D3DXMatrixInverse( &m_matInverse, NULL, &m_matWorld );

    D3DXVec3TransformCoord( &vCollisionInOut, &vCollisionInOut, &m_matInverse );
    m_rStaticAniFrame->GetClosedPoint( vCollisionInOut );
    D3DXVec3TransformCoord( &vCollisionInOut, &vCollisionInOut, &m_matWorld );
}

// Radiosity
void DxPiece::BakeRadiosity( LPDIRECT3DDEVICEQ pd3dDevice, 
							DxLandMan* pLandMan, 
							LPDIRECT3DTEXTUREQ pTextureSystem, 
							float fReflectance, 
							float fFogBlend,
							DWORD dwFogLength,
							DWORD dwRadiosityIndex )
{
	// Radiosity 를 support 할 수 없다.
	if ( m_dwFlags & EMPO_UNSUPPORT_RAD )
		return;

	if ( !m_pvecsvecLightMapObj )
		return;

	if ( m_pvecsvecLightMapObj->empty() )
		return;

	// m_pvecsvecLightMapObj 없으면 작업할 필요가 없다.
	//if ( !m_pvecsvecLightMapObj )
	//	m_pvecsvecLightMapObj = new VEC_SVEC_LIGHTMAPOBJ;

	// 
	m_rStaticAniFrame->BakeRadiosityForPiece
	( 
		pd3dDevice, 
		pLandMan,
		*m_pvecsvecLightMapObj,
		m_matWorld,
		pTextureSystem,
		fReflectance,
		fFogBlend,
		dwFogLength,
		dwRadiosityIndex
	);
}

void DxPiece::AddRadiosityColorForVoxel( LPDIRECT3DDEVICEQ pd3dDevice, DxLandMan* pLandMan, LPDIRECT3DTEXTUREQ pTextureSystem, float fReflectance, float fFogBlend, DWORD dwFogLength )
{
	// Radiosity 를 support 할 수 없다.
	if ( m_dwFlags & EMPO_UNSUPPORT_RAD )
		return;

	m_pLightMapBoxColor->BakeRadiosity( pd3dDevice, pLandMan, pTextureSystem, fReflectance, fFogBlend, dwFogLength );

	m_pLightMapBoxColorFullData->AddRadiosityColorForVoxel( m_pLightMapBoxColor );
}

void DxPiece::SetPrefabMatrix( const D3DXMATRIX& matPrefab )
{
	D3DXMatrixMultiply( &m_matWorld, &m_matWorldOrig, &matPrefab );

	// AABBBox 셋팅.
	SetAABBBox();

	// Light 도 미리 셋팅해놓자.
	CheckPointLight();
}

void DxPiece::Save( sc::SerialFile& SFile )
{
    SFile << static_cast<DWORD>( VERSION );

    SFile.BeginBlock();
    {
		SFile << m_strName;

	    SFile << m_vMax;		// Maybe It is unnecessary < 필요 없지 싶다. >
	    SFile << m_vMin;		// 필요 없지 싶다. DxPieceEdit을 로드시 부르기 때문에..

	    SFile.WriteBuffer( &m_matWorld, sizeof(D3DXMATRIX) );

        // LightMap 관련
        SFile << m_wwStartTexel.dwData;
        SFile << m_wwTextureTexel.dwData;
		SFile.WriteBuffer( &m_vLightMapUV_Rate, sizeof(D3DXVECTOR2) );
        SFile << m_strTextureCombined;
        SFile << m_strTextureDirect_Shadow_1;
		SFile << m_strTextureDirect_Shadow_2;
		SFile << m_strTextureIndirect;

		// LightMapBoxColor
		m_pLightMapBoxColorFullData->Save( SFile );

		// LOD 관련
		SFile << m_fVisibleRadius;
		SFile << m_fInvisibleRadius;

		// TriangleLinkLight 관련
		SFile << static_cast<DWORD>( m_sTriangleLinkLight.m_emPS_3_0_Type );
		SFile << m_sTriangleLinkLight.m_wwLightID.dwData;

		// Instancing 관련.
		SFile << m_bExceptLowOption;
    }
    SFile.EndBlock();
}

void DxPiece::Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, DWORD dwID, BOOL bWLD )
{
	CleanUp();

    DWORD dwVer, dwBufferSize;
    SFile >> dwVer;
    SFile >> dwBufferSize;

	DWORD dwBufferStart = SFile.GetfTell();

    if ( dwVer == 0x110 )
    {
		SFile >> m_strName;

        SFile >> m_vMax;
	    SFile >> m_vMin;

	    SFile.ReadBuffer( &m_matWorld, sizeof(D3DXMATRIX) );

        // LightMap 관련
        SFile >> m_wwStartTexel.dwData;
        SFile >> m_wwTextureTexel.dwData;
		SFile.ReadBuffer( &m_vLightMapUV_Rate, sizeof(D3DXVECTOR2) );
		SFile >> m_strTextureCombined;
		SFile >> m_strTextureDirect_Shadow_1;
		SFile >> m_strTextureDirect_Shadow_2;
		SFile >> m_strTextureIndirect;

		// LightMapBoxColor
		m_pLightMapBoxColorFullData->Load( SFile );

		// LOD 관련
		SFile >> m_fVisibleRadius;
		SFile >> m_fInvisibleRadius;

		// TriangleLinkLight
		DWORD dwTemp;
		SFile >> dwTemp;	m_sTriangleLinkLight.m_emPS_3_0_Type = static_cast<PS_3_0_TYPE>( dwTemp );
		SFile >> m_sTriangleLinkLight.m_wwLightID.dwData;

		// Instancing 관련.
		SFile >> m_bExceptLowOption;
    }
	else if ( dwVer == 0x109 )
	{
		SFile >> m_strName;

		SFile >> m_vMax;
		SFile >> m_vMin;

		SFile.ReadBuffer( &m_matWorld, sizeof(D3DXMATRIX) );

		// LightMap 관련
		SFile >> m_wwStartTexel.dwData;
		SFile >> m_wwTextureTexel.dwData;
		SFile.ReadBuffer( &m_vLightMapUV_Rate, sizeof(D3DXVECTOR2) );
		SFile >> m_strTextureCombined;
		SFile >> m_strTextureDirect_Shadow_1;
		SFile >> m_strTextureDirect_Shadow_2;
		SFile >> m_strTextureIndirect;

		// LightMapBoxColor
		m_pLightMapBoxColorFullData->Load( SFile );

		// LOD 관련
		SFile >> m_fVisibleRadius;
		SFile >> m_fInvisibleRadius;

		// TriangleLinkLight
		DWORD dwTemp;
		SFile >> dwTemp;	m_sTriangleLinkLight.m_emPS_3_0_Type = static_cast<PS_3_0_TYPE>( dwTemp );
		SFile >> m_sTriangleLinkLight.m_wwLightID.dwData;
	}
	else if ( dwVer == 0x108 )
	{
		SFile >> m_strName;

		SFile >> m_vMax;
		SFile >> m_vMin;

		SFile.ReadBuffer( &m_matWorld, sizeof(D3DXMATRIX) );

		// LightMap 관련
		SFile >> m_wwStartTexel.dwData;
		SFile >> m_wwTextureTexel.dwData;
		SFile.ReadBuffer( &m_vLightMapUV_Rate, sizeof(D3DXVECTOR2) );
		SFile >> m_strTextureCombined;
		SFile >> m_strTextureDirect_Shadow_1;
		SFile >> m_strTextureDirect_Shadow_2;
		SFile >> m_strTextureIndirect;

		// LightMapBoxColor
		m_pLightMapBoxColorFullData->Load( SFile );

		// LOD 관련
		SFile >> m_fVisibleRadius;
		SFile >> m_fInvisibleRadius;

		// TriangleLinkLight
		DWORD dwTemp;
		SFile >> dwTemp;	m_sTriangleLinkLight.m_emPS_3_0_Type = static_cast<PS_3_0_TYPE>( dwTemp );

		DWORD dwSize(0L);
		SFile >> dwSize;
		for ( DWORD i=0; i<dwSize; ++i )
		{
			DWORD dwLightID(0L);
			SFile >> dwLightID;
			if ( i==0 )
			{
				m_sTriangleLinkLight.m_wwLightID.wA = static_cast<WORD>( dwLightID );
			}
			else
			{
				m_sTriangleLinkLight.m_wwLightID.wB = static_cast<WORD>( dwLightID );
			}
		}
	}
	else if ( dwVer == 0x107 )
	{
		SFile >> m_strName;

		SFile >> m_vMax;
		SFile >> m_vMin;

		SFile.ReadBuffer( &m_matWorld, sizeof(D3DXMATRIX) );

		// LightMap 관련
		SFile >> m_wwStartTexel.dwData;
		SFile >> m_wwTextureTexel.dwData;
		SFile.ReadBuffer( &m_vLightMapUV_Rate, sizeof(D3DXVECTOR2) );
		SFile >> m_strTextureCombined;
		SFile >> m_strTextureDirect_Shadow_1;
		SFile >> m_strTextureIndirect;

		// LightMapBoxColor
		m_pLightMapBoxColorFullData->Load( SFile );

		// LOD 관련
		SFile >> m_fVisibleRadius;
		SFile >> m_fInvisibleRadius;

		// TriangleLinkLight
		DWORD dwTemp;
		SFile >> dwTemp;	m_sTriangleLinkLight.m_emPS_3_0_Type = static_cast<PS_3_0_TYPE>( dwTemp );

		DWORD dwSize(0L);
		SFile >> dwSize;
		for ( DWORD i=0; i<dwSize; ++i )
		{
			DWORD dwLightID(0L);
			SFile >> dwLightID;
			if ( i==0 )
			{
				m_sTriangleLinkLight.m_wwLightID.wA = static_cast<WORD>( dwLightID );
			}
			else
			{
				m_sTriangleLinkLight.m_wwLightID.wB = static_cast<WORD>( dwLightID );
			}
		}
	}
	else if ( dwVer == 0x106 )
	{
		SFile >> m_strName;

		SFile >> m_vMax;
		SFile >> m_vMin;

		SFile.ReadBuffer( &m_matWorld, sizeof(D3DXMATRIX) );

		// LightMap 관련
		SFile >> m_wwStartTexel.dwData;
		SFile >> m_wwTextureTexel.dwData;
		SFile.ReadBuffer( &m_vLightMapUV_Rate, sizeof(D3DXVECTOR2) );
		SFile >> m_strTextureCombined;
		SFile >> m_strTextureDirect_Shadow_1;
		SFile >> m_strTextureIndirect;

		// LightMapBoxColor
		m_pLightMapBoxColorFullData->Load( SFile );

		// LOD 관련
		SFile >> m_fVisibleRadius;
		SFile >> m_fInvisibleRadius;
	}
	else if ( dwVer == 0x105 )
	{
		SFile >> m_strName;

		SFile >> m_vMax;
		SFile >> m_vMin;

		SFile.ReadBuffer( &m_matWorld, sizeof(D3DXMATRIX) );

		// LightMap 관련
		SFile >> m_wwStartTexel.dwData;
		SFile >> m_wwTextureTexel.dwData;
		SFile.ReadBuffer( &m_vLightMapUV_Rate, sizeof(D3DXVECTOR2) );
		SFile >> m_strTextureCombined;
		SFile >> m_strTextureDirect_Shadow_1;

		// LightMapBoxColor
		m_pLightMapBoxColorFullData->Load( SFile );

		// LOD 관련
		SFile >> m_fVisibleRadius;
		SFile >> m_fInvisibleRadius;
	}
	else if ( dwVer == 0x104 )
	{
		SFile >> m_strName;

		SFile >> m_vMax;
		SFile >> m_vMin;

		SFile.ReadBuffer( &m_matWorld, sizeof(D3DXMATRIX) );

		// LightMap 관련
		SFile >> m_wwStartTexel.dwData;
		SFile >> m_wwTextureTexel.dwData;
		SFile.ReadBuffer( &m_vLightMapUV_Rate, sizeof(D3DXVECTOR2) );
		SFile >> m_strTextureCombined;
		SFile >> m_strTextureDirect_Shadow_1;

		// LightMapBoxColor
		m_pLightMapBoxColorFullData->LoadPrev( SFile );

		// LOD 관련
		SFile >> m_fVisibleRadius;
		SFile >> m_fInvisibleRadius;
	}
	else if ( dwVer == 0x103 )
	{
		SFile >> m_strName;

		SFile >> m_vMax;
		SFile >> m_vMin;

		SFile.ReadBuffer( &m_matWorld, sizeof(D3DXMATRIX) );

		// LightMap 관련
		SFile >> m_wwStartTexel.dwData;
		SFile >> m_wwTextureTexel.dwData;
		SFile >> m_strTextureCombined;
		SFile >> m_strTextureDirect_Shadow_1;
		
		// LightMapBoxColor
		m_pLightMapBoxColorFullData->LoadPrev( SFile );

		// LOD 관련
		SFile >> m_fVisibleRadius;
		SFile >> m_fInvisibleRadius;
	}
	else if ( dwVer == 0x102 )
	{
		SFile >> m_strName;

		SFile >> m_vMax;
		SFile >> m_vMin;

		SFile.ReadBuffer( &m_matWorld, sizeof(D3DXMATRIX) );

		// LightMap 관련
		SFile >> m_wwStartTexel.dwData;
		SFile >> m_wwTextureTexel.dwData;
		SFile >> m_strTextureCombined;
		SFile >> m_strTextureDirect_Shadow_1;

		// LightMapBoxColor
		m_pLightMapBoxColorFullData->LoadPrev2( SFile );

		// LOD 관련
		SFile >> m_fVisibleRadius;
		SFile >> m_fInvisibleRadius;
	}
	else if ( dwVer == 0x101 )
	{
		SFile >> m_strName;

		SFile >> m_vMax;
		SFile >> m_vMin;

		SFile.ReadBuffer( &m_matWorld, sizeof(D3DXMATRIX) );

		// LightMap 관련
		SFile >> m_wwStartTexel.dwData;
		SFile >> m_wwTextureTexel.dwData;
		SFile >> m_strTextureCombined;
		SFile >> m_strTextureDirect_Shadow_1;

		// LightMapBoxColor
		m_pLightMapBoxColorFullData->LoadPrev2( SFile );

		// 버전을 잘 못 기록하였다. 102 버전인데 Save 시 101 로 해서 101 버젼이 2가지가 생겼다.
		if ( SFile.GetfTell() - dwBufferStart != dwBufferSize )
		{
			// LOD 관련
			SFile >> m_fVisibleRadius;
			SFile >> m_fInvisibleRadius;
		}
	}
	else if ( dwVer == 0x100 )
	{
		SFile >> m_strName;

		SFile >> m_vMax;
		SFile >> m_vMin;

		SFile.ReadBuffer( &m_matWorld, sizeof(D3DXMATRIX) );

		TSTRING strTemp;
		// LightMap 관련 파일
		SFile >> m_wwStartTexel.dwData;
		SFile >> m_wwTextureTexel.dwData;
		SFile >> m_strTextureCombined;
		SFile >> m_strTextureDirect_Shadow_1;
		SFile >> strTemp;
		SFile >> strTemp;
	}
    else
    {
        SFile.SetOffSet( SFile.GetfTell() + dwBufferSize );
        return;
    }

	// Note : DxPieceEdit 로딩
	DxPieceEdit* pSrc(NULL);
	if( !m_strName.empty() )
	{
		if ( bWLD )
		{
			pSrc = DxPieceContainer::GetInstance().LoadPiece( pd3dDevice, m_strName.c_str() );
		}
		else
		{
			pSrc = DxPieceContainer::GetInstancePSF().LoadPiece( pd3dDevice, m_strName.c_str() );
		}
	}

	// [2010.08.05] 이전에 Piece를 심어놨다가 그 Piece 파일을 이름은 같고 형상을 바꾸면,
	//				Max와 Min 값이 꼬일 수 있어서 PieceEdit에 있는 값으로 셋팅해 준다. (MarMa\rOnline)
	if( pSrc )
	{
		m_vMax = pSrc->GetAABBMax();
		m_vMin = pSrc->GetAABBMin();
	}

	// Note : 원본이 없을 경우 대비
	if( !pSrc )
	{
		m_vStaticMax = m_vMax;
		m_vStaticMin = m_vMin;

        // Note : 자신에 맞도록 StaticAABB와 Center, Length를 바꾼다.
	    D3DXMatrixAABBTransform( m_vStaticMax, m_vStaticMin, m_matWorld );
	    m_vCenter = (m_vStaticMax+m_vStaticMin)*0.5f;
	    D3DXVECTOR3 vHalf = m_vStaticMax - m_vCenter;
	    m_fRadius = D3DXVec3Length( &vHalf );
	    m_fRadiusSquare = m_fRadius*m_fRadius;

		// Note : 자신과 맞닿아 있는 빛을 체크를 한다.
		CheckPointLight();

		return;
	}

	// Note : DxPieceAABB쪽에서 보는 ID 를 셋팅함.
	m_dwID = dwID;

	// Note : 얻어온다.
	m_vStaticMax = pSrc->GetStaticMax();
	m_vStaticMin = pSrc->GetStaticMin();
	m_dwFlags	= pSrc->GetFlags();

	// Note : 자신에 맞도록 StaticAABB와 Center, Length를 바꾼다.
    D3DXMatrixAABBTransform( m_vStaticMax, m_vStaticMin, m_matWorld );
    m_vCenter = (m_vStaticMax+m_vStaticMin)*0.5f;
    D3DXVECTOR3 vHalf = m_vStaticMax - m_vCenter;
    m_fRadius = D3DXVec3Length( &vHalf );
    m_fRadiusSquare = m_fRadius*m_fRadius;

	// LOD
	float VisibleRadius = pSrc->GetVisibleRadius();
	float InvisibleRadius = pSrc->GetInvisibleRadius();

	if ( m_fVisibleRadius == DEFAULT_VISIBLE_RADIUS )
		m_fVisibleRadius = VisibleRadius;

	if ( m_fInvisibleRadius == DEFAULT_INVISIBLE_RADIUS )
		m_fInvisibleRadius = InvisibleRadius;

	// 초기값을 가지고 있도록 함.
	m_matWorldOrig = m_matWorld;

	// Note : 자신과 맞닿아 있는 빛을 체크를 한다.
	CheckPointLight();

	// Note : 포인터를 얻어옴.
	m_rStaticAniFrame = pSrc->m_pStaticAniFrame;

	LoadLightMap( pd3dDevice );

	m_pPieceEff = new DxPieceEff;
	if ( !m_pPieceEff->Clone( pd3dDevice, pSrc->m_pPieceEff ) )
	{
		SAFE_DELETE ( m_pPieceEff );
	}

	m_pAniTexEff = new DxPieceTexEff;
    pSrc->m_pTextureEff->CloneMapTexEff( pd3dDevice, *pSrc->m_pTextureEff->GetMapTexEff(), *m_pAniTexEff->GetMapTexEff() );

	// Animation 시간은 랜덤하게 시작하지만 이게 셋팅 되어 있으면 0 부터 시작한다.
	if ( m_dwFlags & EMPO_ANI_PLAY_ONCE )
		m_fAniTime = 0.f;
}

void DxPiece::Load_PREV2( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, DxPieceEdit* pSrc )
{
	DWORD dwVer, dwBufferSize;
	SFile >> dwVer;
	SFile >> dwBufferSize;

	if ( dwVer == 0x100 )
	{
		SFile >> m_vMax;
		SFile >> m_vMin;

		SFile.ReadBuffer( &m_matWorld, sizeof(D3DXMATRIX) );

		TSTRING strTemp;
		// LightMap 관련 파일
		SFile >> m_wwStartTexel.dwData;
		SFile >> m_wwTextureTexel.dwData;
		SFile >> m_strTextureCombined;
		SFile >> m_strTextureDirect_Shadow_1;
		SFile >> strTemp;
		SFile >> strTemp;
	}
	else
	{
		SFile.SetOffSet( SFile.GetfTell() + dwBufferSize );
		return;
	}

	// Note : 원본이 없을 경우 대비
	if( !pSrc )
	{
		m_vStaticMax = m_vMax;
		m_vStaticMin = m_vMin;

		// Note : 자신에 맞도록 StaticAABB와 Center, Length를 바꾼다.
		D3DXMatrixAABBTransform( m_vStaticMax, m_vStaticMin, m_matWorld );
		m_vCenter = (m_vStaticMax+m_vStaticMin)*0.5f;
		D3DXVECTOR3 vHalf = m_vStaticMax - m_vCenter;
		m_fRadius = D3DXVec3Length( &vHalf );
		m_fRadiusSquare = m_fRadius*m_fRadius;

		// Note : 자신과 맞닿아 있는 빛을 체크를 한다.
		CheckPointLight();

		return;
	}

	// Note : 얻어온다.
	m_strName = pSrc->GetFileName();
	m_vStaticMax = pSrc->GetStaticMax();
	m_vStaticMin = pSrc->GetStaticMin();
	m_dwFlags	= pSrc->GetFlags();

	// Note : 자신에 맞도록 StaticAABB와 Center, Length를 바꾼다.
	D3DXMatrixAABBTransform( m_vStaticMax, m_vStaticMin, m_matWorld );
	m_vCenter = (m_vStaticMax+m_vStaticMin)*0.5f;
	D3DXVECTOR3 vHalf = m_vStaticMax - m_vCenter;
	m_fRadius = D3DXVec3Length( &vHalf );
	m_fRadiusSquare = m_fRadius*m_fRadius;

	// 초기값을 가지고 있도록 함.
	m_matWorldOrig = m_matWorld;

	// Note : 자신과 맞닿아 있는 빛을 체크를 한다.
	CheckPointLight();

	// Note : 포인터를 얻어옴.
	m_rStaticAniFrame = pSrc->m_pStaticAniFrame;

	LoadLightMap( pd3dDevice );

	m_pPieceEff = new DxPieceEff;
	if ( !m_pPieceEff->Clone( pd3dDevice, pSrc->m_pPieceEff ) )
	{
		SAFE_DELETE ( m_pPieceEff );
	}

	m_pAniTexEff = new DxPieceTexEff;
	pSrc->m_pTextureEff->CloneMapTexEff( pd3dDevice, *pSrc->m_pTextureEff->GetMapTexEff(), *m_pAniTexEff->GetMapTexEff() );

	// Animation 시간은 랜덤하게 시작하지만 이게 셋팅 되어 있으면 0 부터 시작한다.
	if ( m_dwFlags & EMPO_ANI_PLAY_ONCE )
		m_fAniTime = 0.f;
}

void DxPiece::Load_PREV( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, DxPieceEdit* pSrc )
{
    SFile >> m_vMax;
	SFile >> m_vMin;

	SFile.ReadBuffer( &m_matWorld, sizeof(D3DXMATRIX) );

	// Note : 원본이 없을 경우 대비
	if( !pSrc )
	{
		m_vStaticMax = m_vMax;
		m_vStaticMin = m_vMin;

		// Note : 자신에 맞도록 StaticAABB와 Center, Length를 바꾼다.
	    D3DXMatrixAABBTransform( m_vStaticMax, m_vStaticMin, m_matWorld );
	    m_vCenter = (m_vStaticMax+m_vStaticMin)*0.5f;
	    D3DXVECTOR3 vHalf = m_vStaticMax - m_vCenter;
	    m_fRadius = D3DXVec3Length( &vHalf );
	    m_fRadiusSquare = m_fRadius*m_fRadius;

		// Note : 자신과 맞닿아 있는 빛을 체크를 한다.
		CheckPointLight();

		return;
	}

	// Note : 얻어온다.
	m_strName = pSrc->GetFileName();
	m_vStaticMax = pSrc->GetStaticMax();
	m_vStaticMin = pSrc->GetStaticMin();
	m_dwFlags	= pSrc->GetFlags();
	
    // Note : 자신에 맞도록 StaticAABB와 Center, Length를 바꾼다.
    D3DXMatrixAABBTransform( m_vStaticMax, m_vStaticMin, m_matWorld );
    m_vCenter = (m_vStaticMax+m_vStaticMin)*0.5f;
    D3DXVECTOR3 vHalf = m_vStaticMax - m_vCenter;
    m_fRadius = D3DXVec3Length( &vHalf );
    m_fRadiusSquare = m_fRadius*m_fRadius;

	// 초기값을 가지고 있도록 함.
	m_matWorldOrig = m_matWorld;

	// Note : 자신과 맞닿아 있는 빛을 체크를 한다.
	CheckPointLight();

	// Note : 포인터를 얻어옴.
	m_rStaticAniFrame = pSrc->m_pStaticAniFrame;

	m_pPieceEff = new DxPieceEff;
	if ( !m_pPieceEff->Clone( pd3dDevice, pSrc->m_pPieceEff ) )
	{
		SAFE_DELETE ( m_pPieceEff );
	}

	m_pAniTexEff = new DxPieceTexEff;
	pSrc->m_pTextureEff->CloneMapTexEff( pd3dDevice, *pSrc->m_pTextureEff->GetMapTexEff(), *m_pAniTexEff->GetMapTexEff() );

	// Animation 시간은 랜덤하게 시작하지만 이게 셋팅 되어 있으면 0 부터 시작한다.
	if ( m_dwFlags & EMPO_ANI_PLAY_ONCE )
		m_fAniTime = 0.f;
}

void DxPiece::LoadBasicAABB( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// 처음에는 무조건 나오도록.
	m_fAlpha = 1.f;

	if( m_rStaticAniFrame )
	{
		m_rStaticAniFrame->LoadBasicAABB( pd3dDevice );
	}
}

void DxPiece::Import( LPDIRECT3DDEVICEQ pd3dDevice, 
					 DxPieceEdit* pSrc, 
					 const D3DXMATRIX& matLocal, 
					 const D3DXMATRIX& matFrame,
					 float fVisibleRadius,
					 float fInvisibleRadius,
					 BOOL bShader_3_0 )
{
	CleanUp();

	m_strName	= pSrc->GetFileName();
	m_vMax		= pSrc->GetAABBMax();
	m_vMin		= pSrc->GetAABBMin();
	m_vStaticMax = pSrc->GetStaticMax();
	m_vStaticMin = pSrc->GetStaticMin();
	m_vCenter	= pSrc->GetCenter();
	m_fRadius	= pSrc->GetLength();
    m_dwFlags	= pSrc->GetFlags();
	D3DXMatrixMultiply( &m_matWorld, &matLocal, &matFrame );
	//D3DXMatrixMultiply( &m_matWorld, pSrc->GetMatrixLocal(), &m_matWorld );

	// Note : 자신에 맞도록 StaticAABB와 Center, Length를 바꾼다.
    D3DXMatrixAABBTransform( m_vStaticMax, m_vStaticMin, m_matWorld );
    m_vCenter = (m_vStaticMax+m_vStaticMin)*0.5f;
    D3DXVECTOR3 vHalf = m_vStaticMax - m_vCenter;
    m_fRadius = D3DXVec3Length( &vHalf );
    m_fRadiusSquare = m_fRadius*m_fRadius;

	// LOD
	m_fVisibleRadius = fVisibleRadius;
	m_fInvisibleRadius = fInvisibleRadius;

	// Instancing
	m_bExceptLowOption = bShader_3_0;

	// 초기값을 가지고 있도록 함.
	m_matWorldOrig = m_matWorld;

	//m_rPieceEdit = pSrc;
	m_rStaticAniFrame = pSrc->m_pStaticAniFrame;	// 그냥 주소만 주면 됨.

	m_pPieceEff = new DxPieceEff;
	if ( !m_pPieceEff->Clone( pd3dDevice, pSrc->m_pPieceEff ) )
	{
		SAFE_DELETE ( m_pPieceEff );
	}

	m_pAniTexEff = new DxPieceTexEff;
    pSrc->m_pTextureEff->CloneMapTexEff( pd3dDevice, *pSrc->m_pTextureEff->GetMapTexEff(), *m_pAniTexEff->GetMapTexEff() );

	// Animation 시간은 랜덤하게 시작하지만 이게 셋팅 되어 있으면 0 부터 시작한다.
	if ( m_dwFlags & EMPO_ANI_PLAY_ONCE )
		m_fAniTime = 0.f;
}

void DxPiece::CloneData( LPDIRECT3DDEVICEQ pd3dDevice, 
                        DxPiece* pSrc, 
                        BOOL bLightMap, 
                        const TSTRING& strLightMapName, 
                        DxStaticMesh* pStaticMesh, 
                        DxPieceManagerPSF* pPieceManager,
                        DWORD& dwStartTextureSize,
						BOOL& bEnoughUsedPixelLightMap )
{
	CleanUp();

    m_rPiecePSF = pSrc;
	m_strName	= pSrc->m_strName;
	m_vMax		= pSrc->m_vMax;
	m_vMin		= pSrc->m_vMin;
	m_vStaticMax = pSrc->m_vStaticMax;
	m_vStaticMin = pSrc->m_vStaticMin;
	m_vCenter	= pSrc->m_vCenter;
    m_fRadius	= pSrc->m_fRadius;
	m_fRadiusSquare	= pSrc->m_fRadiusSquare;
	m_matWorld	= pSrc->m_matWorld;
    m_dwFlags	= pSrc->m_dwFlags;

	m_wwStartTexel.dwData		= pSrc->m_wwStartTexel.dwData;
	m_wwTextureTexel.dwData		= pSrc->m_wwTextureTexel.dwData;
	m_strTextureCombined		= pSrc->m_strTextureCombined;
	m_strTextureDirect_Shadow_1	= pSrc->m_strTextureDirect_Shadow_1;
	m_strTextureDirect_Shadow_2	= pSrc->m_strTextureDirect_Shadow_2;
	m_strTextureIndirect		= pSrc->m_strTextureIndirect;

	// LOD
	m_fVisibleRadius = pSrc->m_fVisibleRadius;
	m_fInvisibleRadius = pSrc->m_fInvisibleRadius;

	// Instancing
	m_bExceptLowOption = pSrc->m_bExceptLowOption;

	// TriangleLinkLight
	m_sTriangleLinkLight = pSrc->m_sTriangleLinkLight;

	// 초기값을 가지고 있도록 함.
	m_matWorldOrig = m_matWorld;

	// Note : 자신과 맞닿아 있는 빛을 체크를 한다.
	CheckPointLight();

    m_rStaticAniFrame = pSrc->m_rStaticAniFrame;	// 그냥 주소만 주면 됨.

	LoadLightMapForRAD( pd3dDevice );

    // 라이트 맵을 생성한다면 변경이 필요하다.
    if ( bLightMap )
    {
		if ( !(m_dwFlags & EMPO_UNSUPPORT_RAD) )
		{
			if ( !m_pvecsvecLightMapObj )
				m_pvecsvecLightMapObj = new VEC_SVEC_LIGHTMAPOBJ;

			// 제작자에 의해 LightMap UV 가 있는 경우.
			m_rStaticAniFrame->MergeLightMapUVPiece
			( 
				pd3dDevice, 
				strLightMapName, 
				pStaticMesh, 
				pPieceManager, 
				m_matWorld, 
				*m_pvecsvecLightMapObj, 
				dwStartTextureSize,
				m_wwStartTexel,
				bEnoughUsedPixelLightMap,
				m_sTriangleLinkLight.m_wwLightID
			);
		}
    }

	m_pPieceEff = new DxPieceEff;
	if ( !m_pPieceEff->Clone( pd3dDevice, pSrc->m_pPieceEff ) )
	{
		SAFE_DELETE ( m_pPieceEff );
	}

	m_pAniTexEff = new DxPieceTexEff;
	m_pAniTexEff->Clone( pd3dDevice, pSrc->m_pAniTexEff );

	if( pSrc->m_pLightMapBoxColor )
	{
		m_pLightMapBoxColor = new LightMapBoxColor(pSrc->m_pLightMapBoxColor);

		// Code 정리하면서 필요없어보여서 주석처리함.
		// 문제없다면 아예 삭제하자
		//m_vVoxelColor[0] = m_pLightMapBoxColor->GetColorVoxel( 0 );
		//m_vVoxelColor[1] = m_pLightMapBoxColor->GetColorVoxel( 1 );
	}

	// Animation 시간은 랜덤하게 시작하지만 이게 셋팅 되어 있으면 0 부터 시작한다.
	if ( m_dwFlags & EMPO_ANI_PLAY_ONCE )
		m_fAniTime = 0.f;
}

void DxPiece::CloneData( LPDIRECT3DDEVICEQ pd3dDevice, DxPiece* pSrc )
{
	CleanUp();

	m_rPiecePSF = pSrc;
	m_strName	= pSrc->m_strName;
	m_vMax		= pSrc->m_vMax;
	m_vMin		= pSrc->m_vMin;
	m_vStaticMax = pSrc->m_vStaticMax;
	m_vStaticMin = pSrc->m_vStaticMin;
	m_vCenter	= pSrc->m_vCenter;
	m_fRadius	= pSrc->m_fRadius;
	m_fRadiusSquare	= pSrc->m_fRadiusSquare;
	m_matWorld	= pSrc->m_matWorld;
	m_dwFlags	= pSrc->m_dwFlags;

	m_wwStartTexel.dwData		= pSrc->m_wwStartTexel.dwData;
	m_wwTextureTexel.dwData		= pSrc->m_wwTextureTexel.dwData;
	m_strTextureCombined		= pSrc->m_strTextureCombined;
	m_strTextureDirect_Shadow_1	= pSrc->m_strTextureDirect_Shadow_1;
	m_strTextureDirect_Shadow_2	= pSrc->m_strTextureDirect_Shadow_2;
	m_strTextureIndirect		= pSrc->m_strTextureIndirect;

	// LOD
	m_fVisibleRadius = pSrc->m_fVisibleRadius;
	m_fInvisibleRadius = pSrc->m_fInvisibleRadius;

	// Instancing
	m_bExceptLowOption = pSrc->m_bExceptLowOption;

	// TriangleLinkLight
	m_sTriangleLinkLight = pSrc->m_sTriangleLinkLight;

	// 초기값을 가지고 있도록 함.
	m_matWorldOrig = m_matWorld;

	// Note : 자신과 맞닿아 있는 빛을 체크를 한다.
	CheckPointLight();

	m_rStaticAniFrame = pSrc->m_rStaticAniFrame;	// 그냥 주소만 주면 됨.

	LoadLightMapForRAD( pd3dDevice );

	m_pPieceEff = new DxPieceEff;
	if ( !m_pPieceEff->Clone( pd3dDevice, pSrc->m_pPieceEff ) )
	{
		SAFE_DELETE ( m_pPieceEff );
	}

	m_pAniTexEff = new DxPieceTexEff;
	m_pAniTexEff->Clone( pd3dDevice, pSrc->m_pAniTexEff );

	if( pSrc->m_pLightMapBoxColor )
	{
		m_pLightMapBoxColor = new LightMapBoxColor(pSrc->m_pLightMapBoxColor);

		// Code 정리하면서 필요없어보여서 주석처리함.
		// 문제없다면 아예 삭제하자
		//m_vVoxelColor[0] = m_pLightMapBoxColor->GetColorVoxel( 0 );
		//m_vVoxelColor[1] = m_pLightMapBoxColor->GetColorVoxel( 1 );
	}

	// Animation 시간은 랜덤하게 시작하지만 이게 셋팅 되어 있으면 0 부터 시작한다.
	if ( m_dwFlags & EMPO_ANI_PLAY_ONCE )
		m_fAniTime = 0.f;
}

void DxPiece::CheckPointLight()
{
	DxLightMan::GetInstance()->SetPiecePointIndex( m_vCenter, m_fRadiusSquare, m_vecPointIndex );
}

void DxPiece::RenderPointLight( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPSF )
{
    if ( bPSF )
        return;

    NSNearPointLight::CleanUp();

	// Note : 2개만 적용시키자.
	for( DWORD i=0; i<2; ++i )
	{
		if( m_vecPointIndex.size() > i )
		{
			const DXLIGHT* pLight = DxLightMan::GetInstance()->GetDXLIGHT( m_vecPointIndex[i] );
			if( pLight )
			{
		        //	Note : 라이트를 활성화 시킴.
                if ( pLight->IsEnablePL( DxLightMan::GetInstance()->IsWhenDayPointOff() ) )
                {
		            pd3dDevice->SetLight( i+1, &pLight->m_Light );
				    pd3dDevice->LightEnable( i+1, TRUE );

                    NSNearPointLight::InsertPointLight( &pLight->m_Light, pLight->m_fOverLighting );
                }
                else
                {
                    // Note : 안 걸리면 무조건 조명은 끈다.
		            pd3dDevice->LightEnable( i+1, FALSE );
                }
			}
		}
	}
}

void DxPiece::ExportXFile( LPDIRECTXFILESAVEOBJECT pxofsave, LPDIRECTXFILEDATA pDataObjectRoot )
{
	if ( !m_rStaticAniFrame )
		return;

	// 이름 첫 글자가 숫자면 x file 로딩 안되는 문제가 있어서 추가됨.
	TSTRING strNameNEW = _T("_");
	strNameNEW += m_strName;

	m_rStaticAniFrame->ExportXFile( pxofsave, pDataObjectRoot, strNameNEW, m_matWorld );
}

void DxPiece::ExportMAPNAMEPIECE( LPDIRECT3DDEVICEQ pd3dDevice, MAPNAMEPIECE& mapNamePiece )
{
	MAPNAMEPIECE::iterator iter = mapNamePiece.find( m_strName );
	if ( iter != mapNamePiece.end() )
	{
		// 기존에 있는 것
		(*iter).second->AddPiece( pd3dDevice, this );
	}
	else
	{
		// 새로운 것
		DxPieceAABB* pNew = new DxPieceAABB;
		pNew->AddPiece( pd3dDevice, this );
		mapNamePiece.insert ( std::make_pair( m_strName.c_str(), pNew ) );
	}
}

BOOL DxPiece::IsSupportRAD()
{
	return m_rStaticAniFrame->IsMaterialMesh();

	//return (m_dwFlags & EMPO_UNSUPPORT_RAD) ? FALSE : TRUE;
}

void DxPiece::CalculateSlopeMatrix( DxSetLandMan* pSetLandMan, float fRate )
{
	if ( !pSetLandMan )
		return;

	D3DXVECTOR3 vPos(0.f,0.f,0.f);
	vPos.x = m_matWorld._41;
	vPos.y = m_matWorld._42;
	vPos.z = m_matWorld._43;

	D3DXVECTOR3 vNor(0.f,1.f,0.f);
	D3DXVECTOR3 vStart = vPos + D3DXVECTOR3(0.f,10.f,0.f);
	D3DXVECTOR3 vEnd = vPos - D3DXVECTOR3(0.f,10.f,0.f);

	// 현재위치에서 
	pSetLandMan->IsCollisionLineExceptPiece( vStart, vEnd, vNor, EMCC_CULL_CCW, this );

	D3DXVECTOR3 vNorUP(0.f,1.f,0.f);
	vNor = vNor*fRate + vNorUP*(1.f-fRate);
	D3DXVec3Normalize( &vNor, &vNor );

	// 한번에 원하는 방향으로 나오질 않는다 그래서 여러번 회전하도록 한다.
	// 회전횟수는 최대 100 으로 한다.
	for ( DWORD i=0; i<100; ++i )
	{
		D3DXVECTOR3 vNorObject(0.f,1.f,0.f);
		D3DXVec3TransformNormal( &vNorObject, &vNorObject, &m_matWorld );
		D3DXVec3Normalize( &vNorObject, &vNorObject );

		D3DXVECTOR3 vCross;
		D3DXVec3Cross( &vCross, &vNorObject, &vNor );

		float fDot = D3DXVec3Dot( &vNorObject, &vNor );
		if ( fDot > 0.9999f )
		{
			D3DXMATRIX m_matInverse;
			D3DXMatrixInverse( &m_matInverse, NULL, &m_matWorld );

			SetAABBBox();

			return;		// 이제 작업할 필요가 없다.
		}

		float fAngle = acosf( fDot );

		D3DXQUATERNION qRotate;
		D3DXMATRIX matRotate;
		D3DXQuaternionRotationAxis( &qRotate, &vCross, fAngle );
		D3DXMatrixRotationQuaternion ( &matRotate, &qRotate );

		D3DXMATRIX matWorldA;
		D3DXMatrixMultiply( &matWorldA, &matRotate, &m_matWorld );

		D3DXQuaternionRotationAxis( &qRotate, &vCross, -fAngle );
		D3DXMatrixRotationQuaternion ( &matRotate, &qRotate );

		D3DXMATRIX matWorldB;
		D3DXMatrixMultiply( &matWorldB, &matRotate, &m_matWorld );

		vNorObject = D3DXVECTOR3(0.f,1.f,0.f);
		D3DXVec3TransformNormal( &vNorObject, &vNorObject, &matWorldA );
		D3DXVec3Normalize( &vNorObject, &vNorObject );
		float fDotA = D3DXVec3Dot( &vNorObject, &vNor );

		vNorObject = D3DXVECTOR3(0.f,1.f,0.f);
		D3DXVec3TransformNormal( &vNorObject, &vNorObject, &matWorldB );
		D3DXVec3Normalize( &vNorObject, &vNorObject );
		float fDotB = D3DXVec3Dot( &vNorObject, &vNor );

		if ( fDotA >= fDotB )
		{
			m_matWorld = matWorldA;
		}
		else
		{
			m_matWorld = matWorldB;
		}
	}
}

BOOL DxPiece::IsVoxelColor() const
{
	return (m_dwFlags&EMPO_ONLY_VOXELCOLOR) ? TRUE : FALSE;
}

// LightMap UV 를 디폴트 값에서 현재 맵의 LightMap Texture 에 맞춘 Size 로 변경함
void DxPiece::SetLightMapUV_Rate()
{
	if( !m_rStaticAniFrame )
		return;

	m_rStaticAniFrame->SetLightMapUV_Rate( m_vLightMapUV_Rate );
}

BOOL DxPiece::CheckOcclusionCulling( const VEC_DWORD* pvecDWORD ) const
{
	return std::binary_search( pvecDWORD->begin(), pvecDWORD->end(), m_dwID ) ? TRUE : FALSE;
}

// Piece의 TriangleLinkLight 를 생성하도록 한다.
void DxPiece::CreateTriangleLinkLight( LPDIRECT3DDEVICEQ pd3dDevice, 
									  DxStaticMeshCollBase* pStaticMesh, 
									  DxPieceManager* pPieceManager,
									  BOOL bDirectionLight )
{
	if( !m_rStaticAniFrame )
		return;

	// 계산을 하도록 한다.
	{
		// Light
		const DXLIGHT* pLightDAY = DxLightMan::GetInstance()->GetDirectNoonLight();

		if ( bDirectionLight )		// Direction Light 그림자를 만든다고 할 경우만 작동되도록 한다.
		{
			if ( m_rStaticAniFrame->CreateTriangleLinkLightPiece( pd3dDevice, pStaticMesh, pPieceManager, m_sTriangleLinkLight.m_vecLightID, m_matWorld, pLightDAY->m_Light, 1.f ) )
			{
				m_sTriangleLinkLight.m_vecLightID.push_back( 0 );
			}
		}

		const std::vector<std::tr1::shared_ptr<DxShadowSpotLight>>& vecShadowSpotLight = NSShadowLight::GetVecShadowSpotLight();
		std::vector<std::tr1::shared_ptr<DxShadowSpotLight>>::const_iterator citer = vecShadowSpotLight.begin();
		for( ; citer!=vecShadowSpotLight.end(); ++citer )
		{
			// 일단 최단 거리로 닿을 수 있는 곳인지 아닌지 확인한다.
			D3DXVECTOR3 vPos( (*citer)->m_sLight.Position );
			vPos -= m_vCenter;
			float fLength = D3DXVec3Length( &vPos );
			fLength = fabsf( fLength );
			if ( fLength > ((*citer)->m_sLight.Range+m_fRadius) )
			{
				continue;
			}

			if ( m_rStaticAniFrame->CreateTriangleLinkLightPiece( pd3dDevice, pStaticMesh, pPieceManager, m_sTriangleLinkLight.m_vecLightID, m_matWorld, (*citer)->m_sLight, (*citer)->m_fAttEndLineValue ) )
			{
				m_sTriangleLinkLight.m_vecLightID.push_back( (*citer)->m_dwID );
			}
		}
	}
}

//  현재 라이트와 연관된 삼각형들의 특정 슬롯에 자리가 남는지 확인.
BOOL DxPiece::IsLightID_InsertSlot( DWORD dwLightID, DWORD dwSlot ) const
{
	const VEC_DWORD& vecLightID = m_sTriangleLinkLight.m_vecLightID;
	const WORDWORD& wwLightID = m_sTriangleLinkLight.m_wwLightID;
	VEC_DWORD::const_iterator citer = std::find( vecLightID.begin(), vecLightID.end(), dwLightID );
	if ( citer!=vecLightID.end() )
	{
		if ( dwSlot == 0 )		// 1번슬롯
		{
			if ( wwLightID.wA != WORD(-1) )
			{
				return FALSE;	// 값이 있다면 FALSE
			}
		}
		else					// 2번슬롯
		{
			if ( wwLightID.wB != WORD(-1) )
			{
				return FALSE;	// 값을 있다면 FALSE
			}
		}
	}

	return TRUE;	// 현재 공간에 넣을 수 있다.
}

// 현재 라이트와 연관된 삼각형들의 특정 슬롯에 현재 LightID 를 셋팅.
void DxPiece::InsertSlotLightID( DWORD dwLightID, DWORD dwSlot )
{
	const VEC_DWORD& vecLightID = m_sTriangleLinkLight.m_vecLightID;
	WORDWORD& wwLightID = m_sTriangleLinkLight.m_wwLightID;
	VEC_DWORD::const_iterator citer = std::find( vecLightID.begin(), vecLightID.end(), dwLightID );
	if ( citer!=vecLightID.end() )
	{
		if ( dwSlot == 0 )		// 1번슬롯
		{
			wwLightID.wA = static_cast<WORD>( dwLightID );
		}
		else					// 2번슬롯
		{
			wwLightID.wB = static_cast<WORD>( dwLightID );
		}
	}
}

// 한 삼각형에서 Light를 3개 이상 쓰고 있는 것을 찾는다.
BOOL DxPiece::FindTriangleLimitLightNumOver3( DWORD& dwLightID ) const
{
	if ( m_sTriangleLinkLight.m_vecLightID.size() >= 3 )
	{
		dwLightID = m_sTriangleLinkLight.m_vecLightID[2];
		return TRUE;
	}

	return FALSE;
}

// 특정 Light를 줄여주도록 한다.
void DxPiece::EraseLightID( DWORD dwLightID )
{
	VEC_DWORD_ITER iter = m_sTriangleLinkLight.m_vecLightID.begin();
	for ( ; iter!=m_sTriangleLinkLight.m_vecLightID.end(); ++iter )
	{
		if ( dwLightID == (*iter) )
		{
			m_sTriangleLinkLight.m_vecLightID.erase( iter );
			return;
		}
	}
}

// Piece 의 PS_3_0의 렌더타입을 설정하도록 한다. < 구조적인 것 >
void DxPiece::SetType_PS_3_0()
{
	if ( m_sTriangleLinkLight.m_wwLightID.wA == 0 )
	{
		if ( m_sTriangleLinkLight.m_wwLightID.wB == WORD(-1) )
		{
			m_sTriangleLinkLight.m_emPS_3_0_Type = PS_3_0_TYPE_CSM;
		}
		else
		{
			m_sTriangleLinkLight.m_emPS_3_0_Type = PS_3_0_TYPE_CSM_PL;
		}
	}
	else if ( m_sTriangleLinkLight.m_wwLightID.wA != WORD(-1) )
	{
		if ( m_sTriangleLinkLight.m_wwLightID.wB != WORD(-1) )
		{
			m_sTriangleLinkLight.m_emPS_3_0_Type = PS_3_0_TYPE_PL_PL;
		}
		else
		{
			m_sTriangleLinkLight.m_emPS_3_0_Type = PS_3_0_TYPE_PL1;
		}
	}
	else if ( m_sTriangleLinkLight.m_wwLightID.wB != WORD(-1) )
	{
		m_sTriangleLinkLight.m_emPS_3_0_Type = PS_3_0_TYPE_PL2;
	}
	else
	{
		m_sTriangleLinkLight.m_emPS_3_0_Type = PS_3_0_TYPE_ZERO;
	}
}

// Piece 의 PS_3_0의 렌더타입을 설정하도록 한다. < 실제 렌더시 사용되는 것 >
//void DxPiece::SetType_PS_3_0_USED()
//{
//	DWORD dwNumber = NSShadowLight::GetLightNumber();
//
//	switch ( m_sTriangleLinkLight.m_emPS_3_0_Type )
//	{
//	case PS_3_0_TYPE_ZERO:
//		m_sTriangleLinkLight.m_emPS_3_0_Type_USED = PS_3_0_TYPE_USED_ZERO;
//		break;
//
//	case PS_3_0_TYPE_CSM:
//		m_sTriangleLinkLight.m_emPS_3_0_Type_USED = PS_3_0_TYPE_USED_CSM;
//		break;
//
//	case PS_3_0_TYPE_CSM_PL:
//		{
//			BOOL bUsedPL(FALSE);
//			for ( DWORD z=0; z<dwNumber; ++z )
//			{
//				const DxShadowSpotLight* pSpotLight = NSShadowLight::GetShadowSpotLight(z);
//				if ( pSpotLight )
//				{
//					if ( m_sTriangleLinkLight.m_wwLightID.wB == pSpotLight->m_dwID )
//					{
//						bUsedPL = TRUE;
//						break;
//					}
//				}
//			}
//
//			if ( bUsedPL )
//			{
//				m_sTriangleLinkLight.m_emPS_3_0_Type_USED = PS_3_0_TYPE_USED_CSM_PL2_RT;
//			}
//			else
//			{
//				m_sTriangleLinkLight.m_emPS_3_0_Type_USED = PS_3_0_TYPE_USED_CSM_PL2_ON;
//			}
//		}
//		break;
//
//	case PS_3_0_TYPE_PL1:
//		{
//			BOOL bUsedPL(FALSE);
//			for ( DWORD z=0; z<dwNumber; ++z )
//			{
//				const DxShadowSpotLight* pSpotLight = NSShadowLight::GetShadowSpotLight(z);
//				if ( pSpotLight )
//				{
//					if ( m_sTriangleLinkLight.m_wwLightID.wA == pSpotLight->m_dwID )
//					{
//						bUsedPL = TRUE;
//						break;
//					}
//				}
//			}
//
//			if ( bUsedPL )
//			{
//				m_sTriangleLinkLight.m_emPS_3_0_Type_USED = PS_3_0_TYPE_USED_PL1_RT;
//			}
//			else
//			{
//				m_sTriangleLinkLight.m_emPS_3_0_Type_USED = PS_3_0_TYPE_USED_PL1_ON;
//			}
//		}
//		break;
//
//	case PS_3_0_TYPE_PL2:
//		{
//			BOOL bUsedPL(FALSE);
//			for ( DWORD z=0; z<dwNumber; ++z )
//			{
//				const DxShadowSpotLight* pSpotLight = NSShadowLight::GetShadowSpotLight(z);
//				if ( pSpotLight )
//				{
//					if ( m_sTriangleLinkLight.m_wwLightID.wB == pSpotLight->m_dwID )
//					{
//						bUsedPL = TRUE;
//						break;
//					}
//				}
//			}
//
//			if ( bUsedPL )
//			{
//				m_sTriangleLinkLight.m_emPS_3_0_Type_USED = PS_3_0_TYPE_USED_PL2_RT;
//			}
//			else
//			{
//				m_sTriangleLinkLight.m_emPS_3_0_Type_USED = PS_3_0_TYPE_USED_PL2_ON;
//			}
//		}
//		break;
//
//	case PS_3_0_TYPE_PL_PL:
//		{
//			BOOL bUsedPL(FALSE);
//			BOOL bUsedPL2(FALSE);
//			for ( DWORD z=0; z<dwNumber; ++z )
//			{
//				const DxShadowSpotLight* pSpotLight = NSShadowLight::GetShadowSpotLight(z);
//				if ( pSpotLight )
//				{
//					if ( m_sTriangleLinkLight.m_wwLightID.wA == pSpotLight->m_dwID )
//					{
//						bUsedPL = TRUE;
//					}
//					else if ( m_sTriangleLinkLight.m_wwLightID.wB == pSpotLight->m_dwID )
//					{
//						bUsedPL2 = TRUE;
//					}
//				}
//			}
//
//			if ( bUsedPL && bUsedPL2 )
//			{
//				m_sTriangleLinkLight.m_emPS_3_0_Type_USED = PS_3_0_TYPE_USED_PL1_RT_PL2_RT;
//			}
//			else if ( bUsedPL )
//			{
//				m_sTriangleLinkLight.m_emPS_3_0_Type_USED = PS_3_0_TYPE_USED_PL1_RT_PL2_ON;
//			}
//			else if ( bUsedPL2 )
//			{
//				m_sTriangleLinkLight.m_emPS_3_0_Type_USED = PS_3_0_TYPE_USED_PL1_ON_PL2_RT;
//			}
//			else
//			{
//				m_sTriangleLinkLight.m_emPS_3_0_Type_USED = PS_3_0_TYPE_USED_PL1_ON_PL2_ON;
//			}
//		}
//		break;
//	}
//}

// TriangleLinkLight
// Spot Light 에 영향을 안 받는가~?
BOOL DxPiece::IS_PS_3_0_TYPE_ZERO_OR_CSM() const
{
	if ( m_sTriangleLinkLight.m_emPS_3_0_Type == PS_3_0_TYPE_ZERO || m_sTriangleLinkLight.m_emPS_3_0_Type == PS_3_0_TYPE_CSM )
		return TRUE;

	return FALSE;
}


// Material 을 구한다.
void DxPiece::Active_MATERIAL_DATA( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bTool )
{
	if ( !m_rStaticAniFrame )
		return;

	m_rStaticAniFrame->Active_MATERIAL_DATA( pd3dDevice, bTool );

	m_pLightMapBoxColorFullData->FrameMoveStep
	( 
		&m_sParameter.m_sLMBCData.m_pAmbientCube,
		&m_sParameter.m_sLMBCData.m_fAmbientCubeSpot1,
		&m_sParameter.m_sLMBCData.m_fAmbientCubeSpot2,
		&m_sParameter.m_sLMBCData.m_pVoxelColor,
		&m_sParameter.m_sLMBCData.m_pSpot1_Spot2
	);

	m_sParameter.m_pd3dDevice = pd3dDevice;

	//m_sParameter.m_sLMBCData.m_pLightMapBoxColor = &m_vVoxelColorUsed;
	//m_sParameter.m_sLMBCData.m_pAmbientCube = &m_avAmbientCube[0];

	m_sParameter.m_vLightMapUV_Offset.x = static_cast<float>(m_wwStartTexel.wA) / m_wwTextureTexel.wA;
	m_sParameter.m_vLightMapUV_Offset.y = static_cast<float>(m_wwStartTexel.wB) / m_wwTextureTexel.wB;

	m_sParameter.m_nRenderMode = bTool ? NSMaterialManager::EMRM_TOOL : NSMaterialManager::EMRM_MATERIAL;

	m_sParameter.m_pTextureCombined			= m_textureResCombined.GetTexture();
	m_sParameter.m_pTextureDirect_Shadow_1	= m_textureResDirect_Shadow_1.GetTexture();
	m_sParameter.m_pTextureDirect_Shadow_2	= m_textureResDirect_Shadow_2.GetTexture();
	m_sParameter.m_pTextureIndirect			= m_textureResIndirect.GetTexture();

	m_sParameter.m_pTriangleLinkLight		= &m_sTriangleLinkLight;
}


//////////////////////////////////////////////////////////////////////////
//							Instancing
void DxPiece::ResetInstancingLink()
{
	for ( DWORD i=0; i<NSLandThread::TYPE_INSTANCING_ALL_SIZE; ++i )
	{
		m_rPieceInstancingSwap[i] = NULL;
		m_rPieceInstancingSwapForShadow[i] = NULL;
	}
}

void DxPiece::SetInstancingLink( NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, PieceInstancingSwap<DxPieceInstancing>* pInstancing )
{
	m_rPieceInstancingSwap[emCullingRenderType] = pInstancing;
}

void DxPiece::SetInstancingLink( NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, PieceInstancingSwap<DxPieceInstancingForShadow>* pInstancing )
{
	m_rPieceInstancingSwapForShadow[emCullingRenderType] = pInstancing;
}

PieceInstancingSwap<DxPieceInstancing>*	DxPiece::GetInstancingLink( NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType ) const
{ 
	return m_rPieceInstancingSwap[emCullingRenderType]; 
}

PieceInstancingSwap<DxPieceInstancingForShadow>* DxPiece::GetInstancingLinkForShadow( NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType ) const	
{ 
	return m_rPieceInstancingSwapForShadow[emCullingRenderType]; 
}

BOOL DxPiece::IsInstancingDynamicOption() const
{
	// 카메라 충돌로 인한 반투명이 활성화 일 경우는 Instancing 불가.
	if ( GetPickAlpha() )
		return FALSE;

	// 옵션이 변할 수 있다. 
	// TnL_PS_3_0_DF 옵션에서는 SpotLight 에 영향 받아도 LBuffer 를 사용하지 않기 때문에 Instancing 할 수 있다.
	// Spot Light 에 영향 받는 Piece 는 Instancing 에 들어올 수 없다.
	if ( RENDERPARAM::g_emGroundQulityTHREAD > TnL_PS_3_0_DF && !IS_PS_3_0_TYPE_ZERO_OR_CSM() )
		return FALSE;

	return TRUE;
}



//////////////////////////////////////////////////////////////////////////
//							LOD Blend
//	내가 보고있는 방향에서 물체가 나타나면 0->1 로 되야하며,
//	내가 처음 바라보는 방향(카메라회전) 에서는 1 로 시작되야 한다.
void DxPiece::SetAddAlpha( float fTime, BOOL bAddAlpha )
{ 
	m_bAddAlpha = bAddAlpha;

	float fElapsedTime = fTime - m_fTime;
	if ( fElapsedTime > 1.f )	// 안보인지 1초가 지났다면 Alpha 변화를 줘도 된다.
	{
		if ( m_bAddAlpha )
		{
			m_fAlpha = 1.f;
		}
		else
		{
			m_fAlpha = 0.f;
		}
	}

	m_fTime = fTime;
}




//////////////////////////////////////////////////////////////////////////
const D3DXVECTOR4* DxPiece::GetAmbientCube_LightMapBoxColorFullData() const
{
	return m_pLightMapBoxColorFullData->GetAmbientCube();
}