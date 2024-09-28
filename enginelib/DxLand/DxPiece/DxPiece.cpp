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
//		���� PDXEFFECTBASE ��	DxStaticAniFrame�� DxPieceEff�� �Ѵ� ��� �ִ�.. ������ 2���� ������ Ʋ����.
//		DxStaticAniFrame�� ���� ���� �Ἥ ������ ������ ���ϸ� �� �԰�,
//		DxPieceEff�� Ʋ������ �Ἥ ������ Ʋ������ ���ϴ� ���� �Դ´�.
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
    , m_wwStartTexel(0,0)     // ���� Texel
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

	// PieceParameter ����.
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

	// ���� �ٳ���Ѵ�.
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

// ToolMode �� ȣ��ȴ�.
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

	//// m_bQueryTest�� ī�޶� �����Ǿ��� ���� 
	//// �׸��� OcculsionCulling�� �ȴٸ� TRUE�� �ش�.
	//// 
	//// ���� m_bQueryTest�� TRUE�� ī�޶� �� �ɸ��ٸ� �����ϴ� ���� ����.
	//if( m_bQueryTest )
	//{
	//	// Pixel ������ ��´�.
	//	HRESULT hr = GetDataQuery( m_dwCullPixels );
	//	if( hr == S_OK )
	//	{
	//		// 1���˻� �Ǿ��ٰ� 2���˻簡 ����� �Ϸ� �Ǿ����Ƿ� �ʱ�ȭ �Ѵ�.
	//		m_bQueryTest = FALSE;
	//	}
	//}
}

// Thread �󿡼� ȣ���Ѵ�.
void DxPiece::FrameMoveTHREAD( const float fElapsedTime )
{
	m_fAniTime += fElapsedTime * UNITANIKEY_PERSEC;
	if( m_fAniTime > 1.0e15f )	
		m_fAniTime = 0.f;

	m_sParameter.m_fAniTime = m_fAniTime;
	m_sParameter.m_fAlpha = m_fAlpha;
}

// GameMode �� ȣ��ȴ�.
void DxPiece::FrameMoveMAT( const float fTime, const float fElapsedTime )
{
	if( m_rStaticAniFrame )
		m_rStaticAniFrame->FrameMoveMAT( m_pd3dDevice, TRUE );

	if( m_pPieceEff )
		m_pPieceEff->FrameMove( fTime, fElapsedTime );

	// ���� Piece�� ���� �� �۾��� �ʿ���.
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

	// ���� Piece�� ���� �� �۾��� �ʿ���.
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

// ������ DF �� �����ȴٸ� �׿� �´� �ɼǰ����� ��ȭ��Ų��.
void DxPiece::SetData_Force_TnL_PS_3_0_DF()
{
	m_pLightMapBoxColorFullData->FrameMoveRAD( m_vVoxelColorUsed, &m_avAmbientCube[0] );
}

//void DxPiece::FrameMove_Instancing( float fElapsedTime )
//{
//	if( m_pPieceEff )
//		m_pPieceEff->FrameMove( 0.f, fElapsedTime );
//}

//// Piece ������ ������ 1fps �� 1������ ȣ��ǵ��� �Ǿ��ִ�.
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

	// Note : Point Light�� �����Ų��.
	RenderPointLight( pd3dDevice, bPSF );

	//if ( bPSF && m_rPieceEdit && m_rPieceEdit->GetFrameMesh() )
	//{
	//	CLIPVOLUME sCV = DxViewPort::GetInstance().GetClipVolume();
	//	m_rPieceEdit->RenderFrame( pd3dDevice, sCV, m_matWorld, FALSE );
	//}
	//else
	{
		// �ؽ��� Matrix�� 
		D3DXMATRIX matTexScale;
		{
			// Create texture scaling matrix:
			// | ����x    0        0        0 |
			// | 0        ����y    0        0 |
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
			m_textureResCombined.GetTexture(),			// Day - ���� ���������� �̷��� ����.
			m_textureResDirect_Shadow_1.GetTexture(),	// Night - ���� ���������� �̷��� ����.
			FALSE
		);

		// Note : Saved of Location Matirx. Because Animation is Change to All Time. Is Alpha ?
		//			��ġ ��Ʈ������ ���� ���� ���´�. Animation�� ��� �ٲ�� �����̴�. ���Ĵ� .??
		if( !m_pPieceEff )		return;
		m_pPieceEff->SetMatrix();
	}
}

void DxPiece::RenderHardAlpha( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPSF )
{
	if( !m_rStaticAniFrame )
		return;

	// Note : Point Light�� �����Ų��.
	RenderPointLight( pd3dDevice, bPSF );

	//if ( bPSF && m_rPieceEdit && m_rPieceEdit->GetFrameMesh() )
	//{
	//	CLIPVOLUME sCV = DxViewPort::GetInstance().GetClipVolume();
	//	m_rPieceEdit->RenderFrame( pd3dDevice, sCV, m_matWorld, FALSE );
	//}
	//else
	{
		// �ؽ��� Matrix�� 
		D3DXMATRIX matTexScale;
		{
			// Create texture scaling matrix:
			// | ����x    0        0        0 |
			// | 0        ����y    0        0 |
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
			m_textureResCombined.GetTexture(),			// Day - ���� ���������� �̷��� ����.
			m_textureResDirect_Shadow_1.GetTexture(),		// Night - ���� ���������� �̷��� ����.
			FALSE
		);
	}
}

void DxPiece::RenderSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPSF )
{
	if( !m_rStaticAniFrame )
		return;

	// Note : Point Light�� �����Ų��.
	RenderPointLight( pd3dDevice, bPSF );

	//if ( bPSF && m_rPieceEdit )
	//	return;

    // �ؽ��� Matrix�� 
    D3DXMATRIX matTexScale;
    {
        // Create texture scaling matrix:
        // | ����x    0        0        0 |
        // | 0        ����y    0        0 |
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
		m_textureResCombined.GetTexture(),			// Day - ���� ���������� �̷��� ����.
		m_textureResDirect_Shadow_1.GetTexture(),		// Night - ���� ���������� �̷��� ����.
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
//			MaterialSystem - Opaque - TnL - TnL_PS_1_1 �̻�
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
	////			�ִϸ��̼� ����� �����ϱ� ���� Data ����.
	////			2Pass �� ����� �����ϱ�����.
	//if ( bMPShading && !bReflect )
	//{
	//	m_pStaticAniFrame->SetAnimationMatrix( m_vecAnimationMatrixBackUp );
	//}



	//////////////////////////////////////////////////////////////////////////
	// Note : Render
	m_rStaticAniFrame->RenderMS_Opaque( m_sParameter, bMPShading, bBlend );



	////////////////////////////////////////////////////////////////////////////
	////					�ִϸ��̼� ����� �����ϱ� ���� ���.
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
	//			��ġ ��Ʈ������ ���� ���� ���´�. Animation�� ��� �ٲ�� �����̴�. ���Ĵ� .??
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
	//			�ִϸ��̼� ����� �����ϱ� ���� Data ����.
	//			2Pass �� ����� �����ϱ�����.
	if ( bMPShading && !bReflect )
	{
		m_rStaticAniFrame->SetAnimationMatrix( m_vecAnimationMatrixBackUp );
	}



	//////////////////////////////////////////////////////////////////////////
	// Note : Render
	m_rStaticAniFrame->RenderMS_Opaque_Animation( m_sParameter, bMPShading, bReflect, fHeight, bBlend, bAniLoop );



	//////////////////////////////////////////////////////////////////////////
	//					�ִϸ��̼� ����� �����ϱ� ���� ���.
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
	////			��ġ ��Ʈ������ ���� ���� ���´�. Animation�� ��� �ٲ�� �����̴�. ���Ĵ� .??
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

	// Fixed Pipeline �� ����.
	m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matWorld );

	m_sParameter.m_rmatWorld = &m_matWorld;

	//////////////////////////////////////////////////////////////////////////
	// Note : Render
	m_rStaticAniFrame->RenderMS_Opaque( m_sParameter, FALSE, FALSE );
	m_rStaticAniFrame->RenderMS_Opaque_Animation( m_sParameter, FALSE, FALSE, 0.f, FALSE );

	//////////////////////////////////////////////////////////////////////////
	// Note : Saved of Location Matirx. Because Animation is Change to All Time. Is Alpha ?
	//			��ġ ��Ʈ������ ���� ���� ���´�. Animation�� ��� �ٲ�� �����̴�. ���Ĵ� .??
	if( !m_pPieceEff )
		return;

	m_pPieceEff->SetMatrix();
}



void DxPiece::RenderMS_HardAlpha( BOOL bMPShading, BOOL bBlend )
{
	if( !m_rStaticAniFrame )
		return;

	// Fixed Pipeline �� ����.
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
	// Fixed Pipeline �� ����.
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

	// Note : Point Light�� �����Ų��.
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
// �ϴ� ���⼭ Opaque �� �������� �Ѵ�.
void DxPiece::RenderCascadeShadowOpaque()
{
	if( !m_rStaticAniFrame )
		return;

	//if( pCV && !COLLISION::IsCollisionVolume( *pCV, m_vCenter, m_fRadius ) )
	//	return;



	// �ʱ�ȭ
	NSShadowRenderForMaterial::SetWorld( m_matWorld );
	NSShadowRenderForMaterial::CommitChanges();



	//m_pStaticAniFrame->FrameMoveMAT( pd3dDevice );
	if ( m_rStaticAniFrame->IsTexType( EMTTF_OPAQUE|EMTTF_UNKNOWN ) )
	{
		// DxSingleMaterialMeshRenderOrder �����Ͱ� �ʼ���.
		m_sParameter.m_rmatWorld = &m_matWorld;

		m_rStaticAniFrame->RenderCascadeShadowOpaque( m_sParameter.m_pd3dDevice );
	}

	//if ( m_pStaticAniFrame->IsTexType( EMTTF_FRAMEANI_ALPHA_HARD ) )
	//{
	//	vecPieceForFrameAniHardAlpha.push_back( this );
	//}



	////////////////////////////////////////////////////////////////////////////
	////					Instancing ��� �۾�.
	//spPieceInstancing->Insert( m_pStaticAniFrame, &m_sParameter );



	// FrameAnimation Mesh
	if( m_rStaticAniFrame->IsTexType( EMTTF_FRAMEANI_OPAQUE ) )
	{
		//////////////////////////////////////////////////////////////////////////
		//			�ִϸ��̼� ����� �����ϱ� ���� Data ����.
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
		//			�ִϸ��̼� ����� �����ϱ� ���� Data ����.
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

	// Note : Point Light�� �����Ų��.
	RenderPointLight( pd3dDevice, bPSF );

    // �ؽ��� Matrix�� 
    D3DXMATRIX matTexScale;
    {
        // Create texture scaling matrix:
        // | ����x    0        0        0 |
        // | 0        ����y    0        0 |
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
			m_textureResCombined.GetTexture(),			// Day - ���� ���������� �̷��� ����.
			m_textureResDirect_Shadow_1.GetTexture(),		// Night - ���� ���������� �̷��� ����.
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
			m_textureResCombined.GetTexture(),			// Day - ���� ���������� �̷��� ����.
			m_textureResDirect_Shadow_1.GetTexture(),		// Night - ���� ���������� �̷��� ����.
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
			m_textureResCombined.GetTexture(),			// Day - ���� ���������� �̷��� ����.
			m_textureResDirect_Shadow_1.GetTexture(),		// Night - ���� ���������� �̷��� ����.
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
//			D	x		P	i	e	c	e		-		D	x		O	c	c	l	u	s	i	o	n		Q	u	e	r	y		����
void DxPiece::Render_Query_Prepare( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV )
{
	if ( !m_pd3dQuery )
		return;

	if( !m_rStaticAniFrame )	
		return;

	if( !COLLISION::IsCollisionVolume( sCV, m_vCenter, m_fRadius ) )
		return;

	// DxSingleMaterialMeshRenderOrder �����Ͱ� �ʼ���.
	m_rStaticAniFrame->FrameMoveMAT( pd3dDevice, TRUE );

	// Note : Render
	//			���� �ʿ��� �Ͱ� �¾Ƽ�, �� �Լ��� �����.
	//			1. Opaque �� ������
	//			2. ���ʿ��Ѱ� ���� �ʿ��� �͸� ������.
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
		// 1���˻� �ȵǾ����� �˻��ϰ� �÷��׸� �ٲ۴�.
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
		//// ������ ó���Ǵ� �ֵ��� �ᱹ �ִ� �͵��̱� ������, �����Ѵٴ� ó���� ���ش�.
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

	// �����̸� �����Ѵ�.
	DxOcclusionQuery::g_dwCullingDelay += timeGetTime() - dwCullingDelay;

	// 1���˻� �Ǿ��ٰ� 2���˻簡 ����� �Ϸ� �Ǿ����Ƿ� �ʱ�ȭ �Ѵ�.
	m_bQueryTest = FALSE;

	return TRUE;

	//// Pixel ������ ��´�.
	//HRESULT hr = GetDataQuery( dwCullPixels );
	//if( hr == S_OK )
	//{
	//	// 1���˻� �Ǿ��ٰ� 2���˻簡 ����� �Ϸ� �Ǿ����Ƿ� �ʱ�ȭ �Ѵ�.
	//	m_bQueryTest = FALSE;
	//	return TRUE;
	//}

	//// S_OK �� �ƴϸ� ������ ������ ��ü�� ������ �ִ°��̴�.
	//// ���ϱ� ��ġ���� ���Ѵ밪���� �Ǿ��ִ�.
	//// �׳� m_bQueryTest ���� TRUE�� ���� ���Ƽ� �˻縦���� ���� ������ �˻絵 ���� �ʰ� �Ѵ�.
	//dwCullPixels = 0;
	//return FALSE;
}

//			D	x		P	i	e	c	e		-		D	x		O	c	c	l	u	s	i	o	n		Q	u	e	r	y		��
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

    // Note : �ڽſ� �µ��� StaticAABB�� Center, Length�� �ٲ۴�.
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


// Note : �浹�� �Ǹ� �ٷ� �����Ѵ�.
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

	// Note : UV�� ��ǥ�� ���Ѵ�.
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
	// LightMap�� ���� �ʴ´�.
	if ( m_dwFlags & EMPO_UNSUPPORT_RAD )
		return;

	if ( !m_pvecsvecLightMapObj )
		m_pvecsvecLightMapObj = new VEC_SVEC_LIGHTMAPOBJ;

	// �����ڿ� ���� LightMap UV �� �ִ� ���.
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

    // PSF �뵵 Widht�� Height�� �����Ѵ�.
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

	// �����ڿ� ���� LightMap UV �� �ִ� ���.
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
	// LightMap�� ���� �ʴ´�.
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
	// LightMap�� ���� �ʴ´�.
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
	// Animation �ð��� �����ϰ� ���������� �̰� ���� �Ǿ� ������ 0 ���� �����Ѵ�.
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

    // Note : m_matInverse �� �̸� ���� ����.
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
	// Radiosity �� support �� �� ����.
	if ( m_dwFlags & EMPO_UNSUPPORT_RAD )
		return;

	if ( !m_pvecsvecLightMapObj )
		return;

	if ( m_pvecsvecLightMapObj->empty() )
		return;

	// m_pvecsvecLightMapObj ������ �۾��� �ʿ䰡 ����.
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
	// Radiosity �� support �� �� ����.
	if ( m_dwFlags & EMPO_UNSUPPORT_RAD )
		return;

	m_pLightMapBoxColor->BakeRadiosity( pd3dDevice, pLandMan, pTextureSystem, fReflectance, fFogBlend, dwFogLength );

	m_pLightMapBoxColorFullData->AddRadiosityColorForVoxel( m_pLightMapBoxColor );
}

void DxPiece::SetPrefabMatrix( const D3DXMATRIX& matPrefab )
{
	D3DXMatrixMultiply( &m_matWorld, &m_matWorldOrig, &matPrefab );

	// AABBBox ����.
	SetAABBBox();

	// Light �� �̸� �����س���.
	CheckPointLight();
}

void DxPiece::Save( sc::SerialFile& SFile )
{
    SFile << static_cast<DWORD>( VERSION );

    SFile.BeginBlock();
    {
		SFile << m_strName;

	    SFile << m_vMax;		// Maybe It is unnecessary < �ʿ� ���� �ʹ�. >
	    SFile << m_vMin;		// �ʿ� ���� �ʹ�. DxPieceEdit�� �ε�� �θ��� ������..

	    SFile.WriteBuffer( &m_matWorld, sizeof(D3DXMATRIX) );

        // LightMap ����
        SFile << m_wwStartTexel.dwData;
        SFile << m_wwTextureTexel.dwData;
		SFile.WriteBuffer( &m_vLightMapUV_Rate, sizeof(D3DXVECTOR2) );
        SFile << m_strTextureCombined;
        SFile << m_strTextureDirect_Shadow_1;
		SFile << m_strTextureDirect_Shadow_2;
		SFile << m_strTextureIndirect;

		// LightMapBoxColor
		m_pLightMapBoxColorFullData->Save( SFile );

		// LOD ����
		SFile << m_fVisibleRadius;
		SFile << m_fInvisibleRadius;

		// TriangleLinkLight ����
		SFile << static_cast<DWORD>( m_sTriangleLinkLight.m_emPS_3_0_Type );
		SFile << m_sTriangleLinkLight.m_wwLightID.dwData;

		// Instancing ����.
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

        // LightMap ����
        SFile >> m_wwStartTexel.dwData;
        SFile >> m_wwTextureTexel.dwData;
		SFile.ReadBuffer( &m_vLightMapUV_Rate, sizeof(D3DXVECTOR2) );
		SFile >> m_strTextureCombined;
		SFile >> m_strTextureDirect_Shadow_1;
		SFile >> m_strTextureDirect_Shadow_2;
		SFile >> m_strTextureIndirect;

		// LightMapBoxColor
		m_pLightMapBoxColorFullData->Load( SFile );

		// LOD ����
		SFile >> m_fVisibleRadius;
		SFile >> m_fInvisibleRadius;

		// TriangleLinkLight
		DWORD dwTemp;
		SFile >> dwTemp;	m_sTriangleLinkLight.m_emPS_3_0_Type = static_cast<PS_3_0_TYPE>( dwTemp );
		SFile >> m_sTriangleLinkLight.m_wwLightID.dwData;

		// Instancing ����.
		SFile >> m_bExceptLowOption;
    }
	else if ( dwVer == 0x109 )
	{
		SFile >> m_strName;

		SFile >> m_vMax;
		SFile >> m_vMin;

		SFile.ReadBuffer( &m_matWorld, sizeof(D3DXMATRIX) );

		// LightMap ����
		SFile >> m_wwStartTexel.dwData;
		SFile >> m_wwTextureTexel.dwData;
		SFile.ReadBuffer( &m_vLightMapUV_Rate, sizeof(D3DXVECTOR2) );
		SFile >> m_strTextureCombined;
		SFile >> m_strTextureDirect_Shadow_1;
		SFile >> m_strTextureDirect_Shadow_2;
		SFile >> m_strTextureIndirect;

		// LightMapBoxColor
		m_pLightMapBoxColorFullData->Load( SFile );

		// LOD ����
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

		// LightMap ����
		SFile >> m_wwStartTexel.dwData;
		SFile >> m_wwTextureTexel.dwData;
		SFile.ReadBuffer( &m_vLightMapUV_Rate, sizeof(D3DXVECTOR2) );
		SFile >> m_strTextureCombined;
		SFile >> m_strTextureDirect_Shadow_1;
		SFile >> m_strTextureDirect_Shadow_2;
		SFile >> m_strTextureIndirect;

		// LightMapBoxColor
		m_pLightMapBoxColorFullData->Load( SFile );

		// LOD ����
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

		// LightMap ����
		SFile >> m_wwStartTexel.dwData;
		SFile >> m_wwTextureTexel.dwData;
		SFile.ReadBuffer( &m_vLightMapUV_Rate, sizeof(D3DXVECTOR2) );
		SFile >> m_strTextureCombined;
		SFile >> m_strTextureDirect_Shadow_1;
		SFile >> m_strTextureIndirect;

		// LightMapBoxColor
		m_pLightMapBoxColorFullData->Load( SFile );

		// LOD ����
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

		// LightMap ����
		SFile >> m_wwStartTexel.dwData;
		SFile >> m_wwTextureTexel.dwData;
		SFile.ReadBuffer( &m_vLightMapUV_Rate, sizeof(D3DXVECTOR2) );
		SFile >> m_strTextureCombined;
		SFile >> m_strTextureDirect_Shadow_1;
		SFile >> m_strTextureIndirect;

		// LightMapBoxColor
		m_pLightMapBoxColorFullData->Load( SFile );

		// LOD ����
		SFile >> m_fVisibleRadius;
		SFile >> m_fInvisibleRadius;
	}
	else if ( dwVer == 0x105 )
	{
		SFile >> m_strName;

		SFile >> m_vMax;
		SFile >> m_vMin;

		SFile.ReadBuffer( &m_matWorld, sizeof(D3DXMATRIX) );

		// LightMap ����
		SFile >> m_wwStartTexel.dwData;
		SFile >> m_wwTextureTexel.dwData;
		SFile.ReadBuffer( &m_vLightMapUV_Rate, sizeof(D3DXVECTOR2) );
		SFile >> m_strTextureCombined;
		SFile >> m_strTextureDirect_Shadow_1;

		// LightMapBoxColor
		m_pLightMapBoxColorFullData->Load( SFile );

		// LOD ����
		SFile >> m_fVisibleRadius;
		SFile >> m_fInvisibleRadius;
	}
	else if ( dwVer == 0x104 )
	{
		SFile >> m_strName;

		SFile >> m_vMax;
		SFile >> m_vMin;

		SFile.ReadBuffer( &m_matWorld, sizeof(D3DXMATRIX) );

		// LightMap ����
		SFile >> m_wwStartTexel.dwData;
		SFile >> m_wwTextureTexel.dwData;
		SFile.ReadBuffer( &m_vLightMapUV_Rate, sizeof(D3DXVECTOR2) );
		SFile >> m_strTextureCombined;
		SFile >> m_strTextureDirect_Shadow_1;

		// LightMapBoxColor
		m_pLightMapBoxColorFullData->LoadPrev( SFile );

		// LOD ����
		SFile >> m_fVisibleRadius;
		SFile >> m_fInvisibleRadius;
	}
	else if ( dwVer == 0x103 )
	{
		SFile >> m_strName;

		SFile >> m_vMax;
		SFile >> m_vMin;

		SFile.ReadBuffer( &m_matWorld, sizeof(D3DXMATRIX) );

		// LightMap ����
		SFile >> m_wwStartTexel.dwData;
		SFile >> m_wwTextureTexel.dwData;
		SFile >> m_strTextureCombined;
		SFile >> m_strTextureDirect_Shadow_1;
		
		// LightMapBoxColor
		m_pLightMapBoxColorFullData->LoadPrev( SFile );

		// LOD ����
		SFile >> m_fVisibleRadius;
		SFile >> m_fInvisibleRadius;
	}
	else if ( dwVer == 0x102 )
	{
		SFile >> m_strName;

		SFile >> m_vMax;
		SFile >> m_vMin;

		SFile.ReadBuffer( &m_matWorld, sizeof(D3DXMATRIX) );

		// LightMap ����
		SFile >> m_wwStartTexel.dwData;
		SFile >> m_wwTextureTexel.dwData;
		SFile >> m_strTextureCombined;
		SFile >> m_strTextureDirect_Shadow_1;

		// LightMapBoxColor
		m_pLightMapBoxColorFullData->LoadPrev2( SFile );

		// LOD ����
		SFile >> m_fVisibleRadius;
		SFile >> m_fInvisibleRadius;
	}
	else if ( dwVer == 0x101 )
	{
		SFile >> m_strName;

		SFile >> m_vMax;
		SFile >> m_vMin;

		SFile.ReadBuffer( &m_matWorld, sizeof(D3DXMATRIX) );

		// LightMap ����
		SFile >> m_wwStartTexel.dwData;
		SFile >> m_wwTextureTexel.dwData;
		SFile >> m_strTextureCombined;
		SFile >> m_strTextureDirect_Shadow_1;

		// LightMapBoxColor
		m_pLightMapBoxColorFullData->LoadPrev2( SFile );

		// ������ �� �� ����Ͽ���. 102 �����ε� Save �� 101 �� �ؼ� 101 ������ 2������ �����.
		if ( SFile.GetfTell() - dwBufferStart != dwBufferSize )
		{
			// LOD ����
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
		// LightMap ���� ����
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

	// Note : DxPieceEdit �ε�
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

	// [2010.08.05] ������ Piece�� �ɾ���ٰ� �� Piece ������ �̸��� ���� ������ �ٲٸ�,
	//				Max�� Min ���� ���� �� �־ PieceEdit�� �ִ� ������ ������ �ش�. (MarMa\rOnline)
	if( pSrc )
	{
		m_vMax = pSrc->GetAABBMax();
		m_vMin = pSrc->GetAABBMin();
	}

	// Note : ������ ���� ��� ���
	if( !pSrc )
	{
		m_vStaticMax = m_vMax;
		m_vStaticMin = m_vMin;

        // Note : �ڽſ� �µ��� StaticAABB�� Center, Length�� �ٲ۴�.
	    D3DXMatrixAABBTransform( m_vStaticMax, m_vStaticMin, m_matWorld );
	    m_vCenter = (m_vStaticMax+m_vStaticMin)*0.5f;
	    D3DXVECTOR3 vHalf = m_vStaticMax - m_vCenter;
	    m_fRadius = D3DXVec3Length( &vHalf );
	    m_fRadiusSquare = m_fRadius*m_fRadius;

		// Note : �ڽŰ� �´�� �ִ� ���� üũ�� �Ѵ�.
		CheckPointLight();

		return;
	}

	// Note : DxPieceAABB�ʿ��� ���� ID �� ������.
	m_dwID = dwID;

	// Note : ���´�.
	m_vStaticMax = pSrc->GetStaticMax();
	m_vStaticMin = pSrc->GetStaticMin();
	m_dwFlags	= pSrc->GetFlags();

	// Note : �ڽſ� �µ��� StaticAABB�� Center, Length�� �ٲ۴�.
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

	// �ʱⰪ�� ������ �ֵ��� ��.
	m_matWorldOrig = m_matWorld;

	// Note : �ڽŰ� �´�� �ִ� ���� üũ�� �Ѵ�.
	CheckPointLight();

	// Note : �����͸� ����.
	m_rStaticAniFrame = pSrc->m_pStaticAniFrame;

	LoadLightMap( pd3dDevice );

	m_pPieceEff = new DxPieceEff;
	if ( !m_pPieceEff->Clone( pd3dDevice, pSrc->m_pPieceEff ) )
	{
		SAFE_DELETE ( m_pPieceEff );
	}

	m_pAniTexEff = new DxPieceTexEff;
    pSrc->m_pTextureEff->CloneMapTexEff( pd3dDevice, *pSrc->m_pTextureEff->GetMapTexEff(), *m_pAniTexEff->GetMapTexEff() );

	// Animation �ð��� �����ϰ� ���������� �̰� ���� �Ǿ� ������ 0 ���� �����Ѵ�.
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
		// LightMap ���� ����
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

	// Note : ������ ���� ��� ���
	if( !pSrc )
	{
		m_vStaticMax = m_vMax;
		m_vStaticMin = m_vMin;

		// Note : �ڽſ� �µ��� StaticAABB�� Center, Length�� �ٲ۴�.
		D3DXMatrixAABBTransform( m_vStaticMax, m_vStaticMin, m_matWorld );
		m_vCenter = (m_vStaticMax+m_vStaticMin)*0.5f;
		D3DXVECTOR3 vHalf = m_vStaticMax - m_vCenter;
		m_fRadius = D3DXVec3Length( &vHalf );
		m_fRadiusSquare = m_fRadius*m_fRadius;

		// Note : �ڽŰ� �´�� �ִ� ���� üũ�� �Ѵ�.
		CheckPointLight();

		return;
	}

	// Note : ���´�.
	m_strName = pSrc->GetFileName();
	m_vStaticMax = pSrc->GetStaticMax();
	m_vStaticMin = pSrc->GetStaticMin();
	m_dwFlags	= pSrc->GetFlags();

	// Note : �ڽſ� �µ��� StaticAABB�� Center, Length�� �ٲ۴�.
	D3DXMatrixAABBTransform( m_vStaticMax, m_vStaticMin, m_matWorld );
	m_vCenter = (m_vStaticMax+m_vStaticMin)*0.5f;
	D3DXVECTOR3 vHalf = m_vStaticMax - m_vCenter;
	m_fRadius = D3DXVec3Length( &vHalf );
	m_fRadiusSquare = m_fRadius*m_fRadius;

	// �ʱⰪ�� ������ �ֵ��� ��.
	m_matWorldOrig = m_matWorld;

	// Note : �ڽŰ� �´�� �ִ� ���� üũ�� �Ѵ�.
	CheckPointLight();

	// Note : �����͸� ����.
	m_rStaticAniFrame = pSrc->m_pStaticAniFrame;

	LoadLightMap( pd3dDevice );

	m_pPieceEff = new DxPieceEff;
	if ( !m_pPieceEff->Clone( pd3dDevice, pSrc->m_pPieceEff ) )
	{
		SAFE_DELETE ( m_pPieceEff );
	}

	m_pAniTexEff = new DxPieceTexEff;
	pSrc->m_pTextureEff->CloneMapTexEff( pd3dDevice, *pSrc->m_pTextureEff->GetMapTexEff(), *m_pAniTexEff->GetMapTexEff() );

	// Animation �ð��� �����ϰ� ���������� �̰� ���� �Ǿ� ������ 0 ���� �����Ѵ�.
	if ( m_dwFlags & EMPO_ANI_PLAY_ONCE )
		m_fAniTime = 0.f;
}

void DxPiece::Load_PREV( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, DxPieceEdit* pSrc )
{
    SFile >> m_vMax;
	SFile >> m_vMin;

	SFile.ReadBuffer( &m_matWorld, sizeof(D3DXMATRIX) );

	// Note : ������ ���� ��� ���
	if( !pSrc )
	{
		m_vStaticMax = m_vMax;
		m_vStaticMin = m_vMin;

		// Note : �ڽſ� �µ��� StaticAABB�� Center, Length�� �ٲ۴�.
	    D3DXMatrixAABBTransform( m_vStaticMax, m_vStaticMin, m_matWorld );
	    m_vCenter = (m_vStaticMax+m_vStaticMin)*0.5f;
	    D3DXVECTOR3 vHalf = m_vStaticMax - m_vCenter;
	    m_fRadius = D3DXVec3Length( &vHalf );
	    m_fRadiusSquare = m_fRadius*m_fRadius;

		// Note : �ڽŰ� �´�� �ִ� ���� üũ�� �Ѵ�.
		CheckPointLight();

		return;
	}

	// Note : ���´�.
	m_strName = pSrc->GetFileName();
	m_vStaticMax = pSrc->GetStaticMax();
	m_vStaticMin = pSrc->GetStaticMin();
	m_dwFlags	= pSrc->GetFlags();
	
    // Note : �ڽſ� �µ��� StaticAABB�� Center, Length�� �ٲ۴�.
    D3DXMatrixAABBTransform( m_vStaticMax, m_vStaticMin, m_matWorld );
    m_vCenter = (m_vStaticMax+m_vStaticMin)*0.5f;
    D3DXVECTOR3 vHalf = m_vStaticMax - m_vCenter;
    m_fRadius = D3DXVec3Length( &vHalf );
    m_fRadiusSquare = m_fRadius*m_fRadius;

	// �ʱⰪ�� ������ �ֵ��� ��.
	m_matWorldOrig = m_matWorld;

	// Note : �ڽŰ� �´�� �ִ� ���� üũ�� �Ѵ�.
	CheckPointLight();

	// Note : �����͸� ����.
	m_rStaticAniFrame = pSrc->m_pStaticAniFrame;

	m_pPieceEff = new DxPieceEff;
	if ( !m_pPieceEff->Clone( pd3dDevice, pSrc->m_pPieceEff ) )
	{
		SAFE_DELETE ( m_pPieceEff );
	}

	m_pAniTexEff = new DxPieceTexEff;
	pSrc->m_pTextureEff->CloneMapTexEff( pd3dDevice, *pSrc->m_pTextureEff->GetMapTexEff(), *m_pAniTexEff->GetMapTexEff() );

	// Animation �ð��� �����ϰ� ���������� �̰� ���� �Ǿ� ������ 0 ���� �����Ѵ�.
	if ( m_dwFlags & EMPO_ANI_PLAY_ONCE )
		m_fAniTime = 0.f;
}

void DxPiece::LoadBasicAABB( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// ó������ ������ ��������.
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

	// Note : �ڽſ� �µ��� StaticAABB�� Center, Length�� �ٲ۴�.
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

	// �ʱⰪ�� ������ �ֵ��� ��.
	m_matWorldOrig = m_matWorld;

	//m_rPieceEdit = pSrc;
	m_rStaticAniFrame = pSrc->m_pStaticAniFrame;	// �׳� �ּҸ� �ָ� ��.

	m_pPieceEff = new DxPieceEff;
	if ( !m_pPieceEff->Clone( pd3dDevice, pSrc->m_pPieceEff ) )
	{
		SAFE_DELETE ( m_pPieceEff );
	}

	m_pAniTexEff = new DxPieceTexEff;
    pSrc->m_pTextureEff->CloneMapTexEff( pd3dDevice, *pSrc->m_pTextureEff->GetMapTexEff(), *m_pAniTexEff->GetMapTexEff() );

	// Animation �ð��� �����ϰ� ���������� �̰� ���� �Ǿ� ������ 0 ���� �����Ѵ�.
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

	// �ʱⰪ�� ������ �ֵ��� ��.
	m_matWorldOrig = m_matWorld;

	// Note : �ڽŰ� �´�� �ִ� ���� üũ�� �Ѵ�.
	CheckPointLight();

    m_rStaticAniFrame = pSrc->m_rStaticAniFrame;	// �׳� �ּҸ� �ָ� ��.

	LoadLightMapForRAD( pd3dDevice );

    // ����Ʈ ���� �����Ѵٸ� ������ �ʿ��ϴ�.
    if ( bLightMap )
    {
		if ( !(m_dwFlags & EMPO_UNSUPPORT_RAD) )
		{
			if ( !m_pvecsvecLightMapObj )
				m_pvecsvecLightMapObj = new VEC_SVEC_LIGHTMAPOBJ;

			// �����ڿ� ���� LightMap UV �� �ִ� ���.
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

		// Code �����ϸ鼭 �ʿ������� �ּ�ó����.
		// �������ٸ� �ƿ� ��������
		//m_vVoxelColor[0] = m_pLightMapBoxColor->GetColorVoxel( 0 );
		//m_vVoxelColor[1] = m_pLightMapBoxColor->GetColorVoxel( 1 );
	}

	// Animation �ð��� �����ϰ� ���������� �̰� ���� �Ǿ� ������ 0 ���� �����Ѵ�.
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

	// �ʱⰪ�� ������ �ֵ��� ��.
	m_matWorldOrig = m_matWorld;

	// Note : �ڽŰ� �´�� �ִ� ���� üũ�� �Ѵ�.
	CheckPointLight();

	m_rStaticAniFrame = pSrc->m_rStaticAniFrame;	// �׳� �ּҸ� �ָ� ��.

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

		// Code �����ϸ鼭 �ʿ������� �ּ�ó����.
		// �������ٸ� �ƿ� ��������
		//m_vVoxelColor[0] = m_pLightMapBoxColor->GetColorVoxel( 0 );
		//m_vVoxelColor[1] = m_pLightMapBoxColor->GetColorVoxel( 1 );
	}

	// Animation �ð��� �����ϰ� ���������� �̰� ���� �Ǿ� ������ 0 ���� �����Ѵ�.
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

	// Note : 2���� �����Ű��.
	for( DWORD i=0; i<2; ++i )
	{
		if( m_vecPointIndex.size() > i )
		{
			const DXLIGHT* pLight = DxLightMan::GetInstance()->GetDXLIGHT( m_vecPointIndex[i] );
			if( pLight )
			{
		        //	Note : ����Ʈ�� Ȱ��ȭ ��Ŵ.
                if ( pLight->IsEnablePL( DxLightMan::GetInstance()->IsWhenDayPointOff() ) )
                {
		            pd3dDevice->SetLight( i+1, &pLight->m_Light );
				    pd3dDevice->LightEnable( i+1, TRUE );

                    NSNearPointLight::InsertPointLight( &pLight->m_Light, pLight->m_fOverLighting );
                }
                else
                {
                    // Note : �� �ɸ��� ������ ������ ����.
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

	// �̸� ù ���ڰ� ���ڸ� x file �ε� �ȵǴ� ������ �־ �߰���.
	TSTRING strNameNEW = _T("_");
	strNameNEW += m_strName;

	m_rStaticAniFrame->ExportXFile( pxofsave, pDataObjectRoot, strNameNEW, m_matWorld );
}

void DxPiece::ExportMAPNAMEPIECE( LPDIRECT3DDEVICEQ pd3dDevice, MAPNAMEPIECE& mapNamePiece )
{
	MAPNAMEPIECE::iterator iter = mapNamePiece.find( m_strName );
	if ( iter != mapNamePiece.end() )
	{
		// ������ �ִ� ��
		(*iter).second->AddPiece( pd3dDevice, this );
	}
	else
	{
		// ���ο� ��
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

	// ������ġ���� 
	pSetLandMan->IsCollisionLineExceptPiece( vStart, vEnd, vNor, EMCC_CULL_CCW, this );

	D3DXVECTOR3 vNorUP(0.f,1.f,0.f);
	vNor = vNor*fRate + vNorUP*(1.f-fRate);
	D3DXVec3Normalize( &vNor, &vNor );

	// �ѹ��� ���ϴ� �������� ������ �ʴ´� �׷��� ������ ȸ���ϵ��� �Ѵ�.
	// ȸ��Ƚ���� �ִ� 100 ���� �Ѵ�.
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

			return;		// ���� �۾��� �ʿ䰡 ����.
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

// LightMap UV �� ����Ʈ ������ ���� ���� LightMap Texture �� ���� Size �� ������
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

// Piece�� TriangleLinkLight �� �����ϵ��� �Ѵ�.
void DxPiece::CreateTriangleLinkLight( LPDIRECT3DDEVICEQ pd3dDevice, 
									  DxStaticMeshCollBase* pStaticMesh, 
									  DxPieceManager* pPieceManager,
									  BOOL bDirectionLight )
{
	if( !m_rStaticAniFrame )
		return;

	// ����� �ϵ��� �Ѵ�.
	{
		// Light
		const DXLIGHT* pLightDAY = DxLightMan::GetInstance()->GetDirectNoonLight();

		if ( bDirectionLight )		// Direction Light �׸��ڸ� ����ٰ� �� ��츸 �۵��ǵ��� �Ѵ�.
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
			// �ϴ� �ִ� �Ÿ��� ���� �� �ִ� ������ �ƴ��� Ȯ���Ѵ�.
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

//  ���� ����Ʈ�� ������ �ﰢ������ Ư�� ���Կ� �ڸ��� ������ Ȯ��.
BOOL DxPiece::IsLightID_InsertSlot( DWORD dwLightID, DWORD dwSlot ) const
{
	const VEC_DWORD& vecLightID = m_sTriangleLinkLight.m_vecLightID;
	const WORDWORD& wwLightID = m_sTriangleLinkLight.m_wwLightID;
	VEC_DWORD::const_iterator citer = std::find( vecLightID.begin(), vecLightID.end(), dwLightID );
	if ( citer!=vecLightID.end() )
	{
		if ( dwSlot == 0 )		// 1������
		{
			if ( wwLightID.wA != WORD(-1) )
			{
				return FALSE;	// ���� �ִٸ� FALSE
			}
		}
		else					// 2������
		{
			if ( wwLightID.wB != WORD(-1) )
			{
				return FALSE;	// ���� �ִٸ� FALSE
			}
		}
	}

	return TRUE;	// ���� ������ ���� �� �ִ�.
}

// ���� ����Ʈ�� ������ �ﰢ������ Ư�� ���Կ� ���� LightID �� ����.
void DxPiece::InsertSlotLightID( DWORD dwLightID, DWORD dwSlot )
{
	const VEC_DWORD& vecLightID = m_sTriangleLinkLight.m_vecLightID;
	WORDWORD& wwLightID = m_sTriangleLinkLight.m_wwLightID;
	VEC_DWORD::const_iterator citer = std::find( vecLightID.begin(), vecLightID.end(), dwLightID );
	if ( citer!=vecLightID.end() )
	{
		if ( dwSlot == 0 )		// 1������
		{
			wwLightID.wA = static_cast<WORD>( dwLightID );
		}
		else					// 2������
		{
			wwLightID.wB = static_cast<WORD>( dwLightID );
		}
	}
}

// �� �ﰢ������ Light�� 3�� �̻� ���� �ִ� ���� ã�´�.
BOOL DxPiece::FindTriangleLimitLightNumOver3( DWORD& dwLightID ) const
{
	if ( m_sTriangleLinkLight.m_vecLightID.size() >= 3 )
	{
		dwLightID = m_sTriangleLinkLight.m_vecLightID[2];
		return TRUE;
	}

	return FALSE;
}

// Ư�� Light�� �ٿ��ֵ��� �Ѵ�.
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

// Piece �� PS_3_0�� ����Ÿ���� �����ϵ��� �Ѵ�. < �������� �� >
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

// Piece �� PS_3_0�� ����Ÿ���� �����ϵ��� �Ѵ�. < ���� ������ ���Ǵ� �� >
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
// Spot Light �� ������ �� �޴°�~?
BOOL DxPiece::IS_PS_3_0_TYPE_ZERO_OR_CSM() const
{
	if ( m_sTriangleLinkLight.m_emPS_3_0_Type == PS_3_0_TYPE_ZERO || m_sTriangleLinkLight.m_emPS_3_0_Type == PS_3_0_TYPE_CSM )
		return TRUE;

	return FALSE;
}


// Material �� ���Ѵ�.
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
	// ī�޶� �浹�� ���� �������� Ȱ��ȭ �� ���� Instancing �Ұ�.
	if ( GetPickAlpha() )
		return FALSE;

	// �ɼ��� ���� �� �ִ�. 
	// TnL_PS_3_0_DF �ɼǿ����� SpotLight �� ���� �޾Ƶ� LBuffer �� ������� �ʱ� ������ Instancing �� �� �ִ�.
	// Spot Light �� ���� �޴� Piece �� Instancing �� ���� �� ����.
	if ( RENDERPARAM::g_emGroundQulityTHREAD > TnL_PS_3_0_DF && !IS_PS_3_0_TYPE_ZERO_OR_CSM() )
		return FALSE;

	return TRUE;
}



//////////////////////////////////////////////////////////////////////////
//							LOD Blend
//	���� �����ִ� ���⿡�� ��ü�� ��Ÿ���� 0->1 �� �Ǿ��ϸ�,
//	���� ó�� �ٶ󺸴� ����(ī�޶�ȸ��) ������ 1 �� ���۵Ǿ� �Ѵ�.
void DxPiece::SetAddAlpha( float fTime, BOOL bAddAlpha )
{ 
	m_bAddAlpha = bAddAlpha;

	float fElapsedTime = fTime - m_fTime;
	if ( fElapsedTime > 1.f )	// �Ⱥ����� 1�ʰ� �����ٸ� Alpha ��ȭ�� �൵ �ȴ�.
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