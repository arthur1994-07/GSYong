#include "pch.h"

#include <algorithm>

#include "../../../SigmaCore/File/SerialFile.h"
#include "../../../SigmaCore/Util/Util.h"

#include "../../DxMeshs/FrameMesh/NSVERTEXNORTEX_FX.h"
#include "../../DxMeshs/FrameMesh/DxFrameMesh.h"
#include "../../DxTools/Occlusion/NSOcclusionCulling.h"
#include "../../DxTools/Material/NSShadowRenderForMaterial.h"
#include "../../DxTools/DxInputDevice.h"
#include "../../DxTools/DxViewPort.h"
#include "../../DxTools/DxEnvironment.h"
#include "../../DxTools/DxRenderStates.h"
#include "../../DxTools/RENDERPARAM.h"
#include "../../DxLand/LightMap/NSLightMapFX.h"
#include "../../DxLand/LightMap/NSLightMap.h"
#include "../../DxLand/DxLandMan.h"
#include "../../Common/StlFunctions.h"
#include "../../Common/profile.h"
#include "../../DxCommon9/DXUTmisc.h"
#include "../../GlobalParam.h"

#include "../LightMap/NSRadiosity.h"
#include "../PieceInstancingThread.h"
#include "../EntityRenderList.h"
#include "../NSLandThread.h"
#include "./DxPieceManagerPSF.h"
#include "./NSPieceInstancing.h"
#include "./DxPiececontainer.h"
#include "./NSPieceQuickSort.h"
#include "./DxPieceStaticAni.h"
#include "./DxPieceEdit.h"
#include "./DxPieceAABB.h"
#include "./DxPieceDef.h"
#include "./DxPiece.h"

#include "DxPieceManagerWLD.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// 3500 �������� ���� ��. ����, 50%, 110%
//static const float g_fInstancingCellSize(2083.f);
static const float g_fInstancingCellSize(10.f);		// Thread �� Piece �� �����ϱ� ������ ������.

// -----------------------------------------------------------------------------------------------------------------------------------------
//										D	x		P	i	e	c	e		M	a	n	a	g	e	r
// -----------------------------------------------------------------------------------------------------------------------------------------
const DWORD DxPieceManagerWLD::VERSION_WLD = 0x104;

DxPieceManagerWLD::DxPieceManagerWLD()
	: m_fTime(0.f)
	, m_fElapsedTime(0.f)
	, m_bInstancingON(TRUE)
	, m_bUseLightMap(FALSE)
	, m_bLodBlending(FALSE)
	, m_spPieceAABB(new DxPieceAABB)
	, m_spInstancingMeshLM(new PieceInstancingThread<PIECE_INSTANCING_FIRST,DxPieceInstancing>)
	, m_spInstancingMeshReflection(new PieceInstancingThread<PIECE_INSTANCING_FIRST,DxPieceInstancing>)
{
	for ( DWORD i=0; i<NSLandThread::TYPE_INSTANCING_PURE_MESH_SIZE; ++i )
	{
		m_spInstancingPureMesh[i] = boost::shared_ptr<PieceInstancingThread<DxStaticAniFrame*,DxPieceInstancingForShadow>>(new PieceInstancingThread<DxStaticAniFrame*,DxPieceInstancingForShadow>);
	}

	//m_spPieceRenderListSwap = boost::shared_ptr<EntityRenderListSwap>(new EntityRenderListSwap(m_spPieceAABB))
}

DxPieceManagerWLD::~DxPieceManagerWLD()
{
	CleanUp();
}

void DxPieceManagerWLD::CleanUp()
{
	m_fTime = 0.f;
	m_fElapsedTime = 0.f;

	m_bLodBlending = FALSE;
	m_bUseLightMap = FALSE;

	m_spPieceAABB->CleanUp();
	//m_spPieceInstancing->CleanUp();
	m_spInstancingMeshLM->CleanUp();
	m_spInstancingMeshReflection->CleanUp();
	for ( DWORD i=0; i<NSLandThread::TYPE_INSTANCING_PURE_MESH_SIZE; ++i )
	{
		m_spInstancingPureMesh[i]->CleanUp();
	}
}

void DxPieceManagerWLD::OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//if ( m_pPieceAABB )
	{
		m_spPieceAABB->OnResetDevice( pd3dDevice );
	}

	m_spInstancingMeshLM->OnResetDevice( pd3dDevice );
	m_spInstancingMeshReflection->OnResetDevice( pd3dDevice );
	for ( DWORD i=0; i<NSLandThread::TYPE_INSTANCING_PURE_MESH_SIZE; ++i )
	{
		m_spInstancingPureMesh[i]->OnResetDevice( pd3dDevice );
	}

	//m_spPieceInstancing->OnResetDevice( pd3dDevice );
}

void DxPieceManagerWLD::OnLostDevice()
{
	//if ( m_pPieceAABB )
	{
		m_spPieceAABB->OnLostDevice();
	}

	// �۾� �Ϸ� �ɶ����� �������.
	while ( !NSLandThread::IsCompleteData() )
	{
	}

	m_spInstancingMeshLM->OnLostDevice();
	m_spInstancingMeshReflection->OnLostDevice();
	for ( DWORD i=0; i<NSLandThread::TYPE_INSTANCING_PURE_MESH_SIZE; ++i )
	{
		m_spInstancingPureMesh[i]->OnLostDevice();
	}

	//m_spPieceInstancing->OnLostDevice();
}

void DxPieceManagerWLD::CreateQuery( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//if ( m_pPieceAABB )
	{
		m_spPieceAABB->CreateQuery( pd3dDevice );
	}
}

void DxPieceManagerWLD::DestroyQuery()
{
	//if ( m_pPieceAABB )
	{
		m_spPieceAABB->DestroyQuery();
	}
}

// Swap �Ŀ� FrameMove �� �Ѵ�.
void DxPieceManagerWLD::FrameMove( const float fTime, const float fElapsedTime, boost::shared_ptr<EntityRenderListSwap>& spEntityRenderListSwap )
{
	m_fTime = fTime;
	m_fElapsedTime = fElapsedTime;

	m_spInstancingMeshLM->FrameMoveStep( fElapsedTime );
	spEntityRenderListSwap->FrameMoveMAT( fTime, fElapsedTime );
}

// ��򰡿��� Swap �� �Ѵ�.




//////////////////////////////////////////////////////////////////////////
// Material �� ���Ѵ�.
void DxPieceManagerWLD::Active_MATERIAL_DATA( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bTool )
{
	m_spPieceAABB->Active_MATERIAL_DATA( pd3dDevice, bTool );
}

///////////////////////////////////////////////////////////////////////////
// ������ DF �� �����ȴٸ� �׿� �´� �ɼǰ����� ��ȭ��Ų��.
void DxPieceManagerWLD::SetData_Force_TnL_PS_3_0_DF()
{
	m_spPieceAABB->SetData_Force_TnL_PS_3_0_DF();
}

//////////////////////////////////////////////////////////////////////////
// ��ü �� ȣ��ȴ�.
void DxPieceManagerWLD::RenderOpaque( LPDIRECT3DDEVICEQ pd3dDevice, 
									 //const D3DXVECTOR3& vFromPt, 
									 //const D3DXVECTOR3& vLookatPt,
									 const VEC_DWORD* pvecDWORD, 
									 BOOL bMPShading,
									 BOOL bUsedMaterialSystem,
									 BOOL bReflect,
									 float fReflectHeight,
									 boost::shared_ptr<EntityRenderListSwap>& spEntityRenderListSwap )
{
	//if ( g_emTnLMode_Prev != RENDERPARAM::g_emGroundQulity )
	//{
	//	g_emTnLMode_Prev = RENDERPARAM::g_emGroundQulity;
	//	//if ( m_pPieceAABB )
	//	{
	//		m_spPieceAABB->ResetPieceParameter();
	//	}
	//}

	// LightMap �����̶�� LightMap ���� Piece �� OverLighting �ǵ��� Shader �� ������ �ǵ��� �Ѵ�.
	if ( m_bUseLightMap && !NSPieceQuickSort::g_bBakeRAD )
	{
		// ī�޶� ������ �۵��Ǿ�� �Ѵ�.
		NSVERTEXNORTEX_FX::SetEnable();
	}

	//// 1��° Pass ������ ���۵ȴ�. 2��° Pass ������ �۾����� �ʴ´�.
	//if ( !bMPShading )
	//{
	//	if( !GLOBAL_PARAM::IsValue( GP_PIECE ) )
	//		return;

	//	////if ( m_pPieceAABB )
	//	//{
	//	//	//m_spPieceAABB->InsertQuickSort( sCV, vFromPt, vLookatPt, m_bLodBlending, TRUE );
	//	//	m_spPieceAABB->InsertQuickSort( sCV, vFromPt, vLookatPt, m_bLodBlending, bUsedMaterialSystem ? TRUE : FALSE, bReflect );
	//	//	m_spPieceAABB->FrameMoveStep( pd3dDevice, FALSE );
	//	//}

	//	//NSPieceQuickSort::QuickSortProcess();

	//	//if ( NSOcclusionCulling::IsActivePiece() )
	//	//{
	//	//	NSPieceQuickSort::CheckOcclusionCulling( pvecDWORD );
	//	//}

	//	//if ( NSPieceQuickSort::g_bBakeRAD )
	//	//{
	//	//	BOOL bDay = TRUE;
	//	//	NSPieceQuickSort::FrameMoveRAD( bDay );
	//	//}
	//	//else
	//	//{
	//	//	if ( bUsedMaterialSystem )
	//	//	{
	//	//		NSPieceQuickSort::FrameMoveMAT( m_fTime, m_fElapsedTime );
	//	//	}
	//	//	else
	//	//	{
	//	//		NSPieceQuickSort::FrameMove( m_fTime, m_fElapsedTime );
	//	//		NSPieceQuickSort::FrameMoveONLY( m_fTime, m_fElapsedTime );	// Note : FrameMove Static
	//	//	}
	//	//}

	//	//if ( NSPieceQuickSort::g_bBakeRAD )
	//	//{
	//	//	BOOL bDay = TRUE;
	//	//	NSPieceQuickSort::FrameMoveRAD( bDay );
	//	//}
	//	//else
	//	//{
	//	//	if ( bUsedMaterialSystem )
	//	//	{
	//	//		NSPieceQuickSort::FrameMoveMAT( m_fTime, m_fElapsedTime );
	//	//	}
	//	//	else
	//	//	{
	//	//		NSPieceQuickSort::FrameMove( m_fTime, m_fElapsedTime );
	//	//		NSPieceQuickSort::FrameMoveONLY( m_fTime, m_fElapsedTime );	// Note : FrameMove Static
	//	//	}
	//	//}

	//	// ���� �ɼǿ� ���� ���� �ٲ� �Ź� �Ѱ��� ���ϰ� �Ѵ�.
	//	// ���� ��� ������ ������ ����.
	//	//m_spPieceAABB->FrameMoveStep( pd3dDevice, FALSE );

	//	////// LBuffer �� ����� �ʿ�� ����.  bMPShading==TRUE �� LBuffer �� ��ϵȴ�.
	//	////
	//	////if ( m_pPieceAABB && !bReflect )
	//	//if ( bReflect )
	//	//{
	//	//	m_spInstancingMeshReflection->RenderOpaque( pd3dDevice );
	//	//}
	//	//else
	//	//{
	//	//	//m_spPieceAABB->FrameMove_Instancing( pd3dDevice, m_fElapsedTime );
	//	//	//m_spPieceAABB->RenderOpaque_Instancing( pd3dDevice, sCV );
	//	//	m_spInstancingMeshLM->RenderOpaque( pd3dDevice );
	//	//}

	//	////// �ִϸ��̼� ���� Object
	//	////{
	//	////	NSPieceQuickSort::RenderOpaque( pd3dDevice, sCV, TRUE, bMPShading, bUsedMaterialSystem, bReflect, fReflectHeight );
	//	////}
	//	//spEntityRenderListSwap->RenderOpaque( pd3dDevice, bMPShading, bUsedMaterialSystem, bReflect, fReflectHeight, TRUE );
	//}




	//// LBuffer �� ����� �ʿ�� ����.  bMPShading==TRUE �� LBuffer �� ��ϵȴ�.
	//
	//if ( m_pPieceAABB && !bReflect )

	if ( bReflect )
	{
		m_spInstancingMeshReflection->RenderOpaque( pd3dDevice );
		spEntityRenderListSwap->RenderOpaque( pd3dDevice, bMPShading, bUsedMaterialSystem, bReflect, fReflectHeight, TRUE );
	}
	else if ( bMPShading )
	{
		// ���ο��� bMPShading �ɼ��� ���� ��ü �� ������.
		spEntityRenderListSwap->RenderOpaque( pd3dDevice, bMPShading, bUsedMaterialSystem, bReflect, fReflectHeight, TRUE );
	}
	else
	{
		m_spInstancingMeshLM->RenderOpaque( pd3dDevice );
		spEntityRenderListSwap->RenderOpaque( pd3dDevice, bMPShading, bUsedMaterialSystem, bReflect, fReflectHeight, TRUE );
	}

	//// �ִϸ��̼� ���� Object
	//{
	//	NSPieceQuickSort::RenderOpaque( pd3dDevice, sCV, TRUE, bMPShading, bUsedMaterialSystem, bReflect, fReflectHeight );
	//}
	




    // LightMap ���� �� ��ġ��ǥ�� ���߱� ����.
	if ( m_bUseLightMap && !NSPieceQuickSort::g_bBakeRAD )
    {
        D3DXMATRIX matIdentity;
        D3DXMatrixIdentity( &matIdentity );
        NSLightMapFX::SetWorld( matIdentity, 1.f );
        NSVERTEXNORTEX_FX::SetWorld( matIdentity, 1.f );
		NSVERTEXNORTEX_FX::SetDisable();
    }
}

void DxPieceManagerWLD::RenderHardAlpha( LPDIRECT3DDEVICEQ pd3dDevice, 
										float fLightMapDetail, 
										BOOL bMPShading, 
										BOOL bUsedMaterialSystem, 
										BOOL bReflect,
										boost::shared_ptr<EntityRenderListSwap>& spEntityRenderListSwap )
{
	DWORD dwAlphaTestEnable(FALSE);
	DWORD dwAlphaOP_0(FALSE);
	pd3dDevice->GetRenderState( D3DRS_ALPHATESTENABLE,		&dwAlphaTestEnable );
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,		TRUE );
	pd3dDevice->GetTextureStageState( 0, D3DTSS_ALPHAOP,	&dwAlphaOP_0 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );

	//NSPieceQuickSort::RenderHardAlpha( pd3dDevice, sCV, TRUE, bMPShading, bUsedMaterialSystem );

	//// [shhan][2014.10.27] �ݻ綧�� Instancing ���� �۾��� �ʿ䰡 ����.
	////if ( !bMPShading )	//&& !bReflect )
	//{
	//	////if ( m_pPieceAABB )
	//	//{
	//	//	m_spPieceAABB->RenderHardAlpha_Instancing( pd3dDevice, sCV );
	//	//}

	//	if ( bReflect )
	//	{
	//		m_spInstancingMeshReflection->RenderHardAlpha( pd3dDevice );
	//	}
	//	else if ( !bMPShading )
	//	{
	//		m_spInstancingMeshLM->RenderHardAlpha( pd3dDevice );
	//	}
	//}

	

	if ( bReflect )
	{
		m_spInstancingMeshReflection->RenderHardAlpha( pd3dDevice );
		spEntityRenderListSwap->RenderHardAlpha( pd3dDevice, bMPShading, bUsedMaterialSystem, TRUE );
	}
	else if ( bMPShading )
	{
		// ���ο��� bMPShading �ɼ��� ���� ��ü �� ������.
		spEntityRenderListSwap->RenderHardAlpha( pd3dDevice, bMPShading, bUsedMaterialSystem, TRUE );
	}
	else
	{
		m_spInstancingMeshLM->RenderHardAlpha( pd3dDevice );
		spEntityRenderListSwap->RenderHardAlpha( pd3dDevice, bMPShading, bUsedMaterialSystem, TRUE );
	}



	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	dwAlphaOP_0 );
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,		dwAlphaTestEnable );
}

void DxPieceManagerWLD::RenderEff( LPDIRECT3DDEVICEQ pd3dDevice, 
								  DxStaticMeshLightMap* pStaticMeshLightMap, 
								  DxFrameMesh* pFrameMesh, 
								  float fLightMapDetail, 
								  BOOL bUsedMaterialSystem,
								  boost::shared_ptr<EntityRenderListSwap>& spEntityRenderListSwap )
{
	if( !GLOBAL_PARAM::IsValue( GP_PIECE ) )
		return;

	// LightMap �����̶�� LightMap ���� Piece �� OverLighting �ǵ��� Shader �� ������ �ǵ��� �Ѵ�.
	if ( m_bUseLightMap )
	{
		NSVERTEXNORTEX_FX::SetEnable();
	}

	//CLIPVOLUME sCV = DxViewPort::GetInstance().GetClipVolume();
	//NSPieceQuickSort::RenderEff( pd3dDevice, pStaticMeshLightMap, pFrameMesh, TRUE, bUsedMaterialSystem );
	spEntityRenderListSwap->RenderEff( pd3dDevice, pStaticMeshLightMap, pFrameMesh, TRUE, bUsedMaterialSystem );
	//m_spInstancingMeshLM->RenderEff( pd3dDevice, pStaticMeshLightMap );

	//if ( m_pPieceAABB )
	//{
	//	m_spPieceAABB->RenderEff_Instancing( pd3dDevice, sCV, pStaticMeshLightMap );
	//}

	// LightMap ���� �� ��ġ��ǥ�� ���߱� ����.
	if ( m_bUseLightMap )
	{
		D3DXMATRIX matIdentity;
		D3DXMatrixIdentity( &matIdentity );
		NSLightMapFX::SetWorld( matIdentity, 1.f );
		NSVERTEXNORTEX_FX::SetWorld( matIdentity, 1.f );
		NSVERTEXNORTEX_FX::SetDisable();
	}
}
// ��ü �� ȣ��ȴ�.
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
// TnL_PS_1_1 ���Ͽ����� ȣ�� �ȴ�.
void DxPieceManagerWLD::RenderEffFrame( boost::shared_ptr<EntityRenderListSwap>& spEntityRenderListSwap )
{
	//NSPieceQuickSort::RenderEffFrame();
	spEntityRenderListSwap->RenderEffFrame();
}

void DxPieceManagerWLD::RenderSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, 
										float fLightMapDetail, 
										BOOL bMPShading, 
										BOOL bUsedMaterialSystem,
										boost::shared_ptr<EntityRenderListSwap>& spEntityRenderListSwap )
{
	//NSPieceQuickSort::RenderSoftAlpha( pd3dDevice, sCV, TRUE, bMPShading, bUsedMaterialSystem );
	spEntityRenderListSwap->RenderSoftAlpha( pd3dDevice, TRUE, bMPShading, bUsedMaterialSystem );
}

void DxPieceManagerWLD::RenderPickAlpha( LPDIRECT3DDEVICEQ pd3dDevice, 
										DxStaticMeshLightMap* pStaticMeshLightMap, 
										DxFrameMesh* pFrameMesh,
										BOOL bReflect,
										float fReflectHeight,
										boost::shared_ptr<EntityRenderListSwap>& spEntityRenderListSwap )
{
	// LightMap �����̶�� LightMap ���� Piece �� OverLighting �ǵ��� Shader �� ������ �ǵ��� �Ѵ�.
	if ( m_bUseLightMap )
	{
		NSVERTEXNORTEX_FX::SetEnable();
	}

	//CLIPVOLUME sCV = DxViewPort::GetInstance().GetClipVolume();
	//NSPieceQuickSort::RenderPickAlpha
	//( 
	//	pd3dDevice, 
	//	sCV, 
	//	pStaticMeshLightMap, 
	//	pFrameMesh, 
	//	FALSE,
	//	bReflect, 
	//	fReflectHeight, 
	//	TRUE 
	//);
	spEntityRenderListSwap->RenderPickAlpha( pd3dDevice, pStaticMeshLightMap, NULL, bReflect, fReflectHeight );

	// LightMap ���� �� ��ġ��ǥ�� ���߱� ����.
	if ( m_bUseLightMap )
	{
		D3DXMATRIX matIdentity;
		D3DXMatrixIdentity( &matIdentity );
		NSLightMapFX::SetWorld( matIdentity, 1.f );
		NSVERTEXNORTEX_FX::SetWorld( matIdentity, 1.f );
	}
}

//void DxPieceManagerWLD::Render_Reflect( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPSF )
//{
//	if( !GLOBAL_PARAM::IsValue( GP_PIECE ) )
//		return;
//
//	if ( m_bUseLightMap )
//	{
//		NSVERTEXNORTEX_FX::SetEnable();
//	}
//
//	// LightMap ���� �� ��ġ��ǥ�� ���߱� ����.
//	if ( m_bUseLightMap )
//	{
//		D3DXMATRIX matIdentity;
//		D3DXMatrixIdentity( &matIdentity );
//		NSLightMapFX::SetWorld( matIdentity, 1.f );
//		NSVERTEXNORTEX_FX::SetWorld( matIdentity, 1.f );
//	}
//}

// ENSTM_DECAL_AFTER �� ������ �۾��� �ȴ�.
void DxPieceManagerWLD::RenderDecal( boost::shared_ptr<EntityRenderListSwap>& spEntityRenderListSwap )
{
	//NSPieceQuickSort::RenderDecal();
	spEntityRenderListSwap->RenderDecalPass2();

	spEntityRenderListSwap->RenderCustom_UNLIT_UNFOG();

	spEntityRenderListSwap->RenderWaterCustomSoftAlpha();
}
// TnL_PS_1_1 ���Ͽ����� ȣ�� �ȴ�.
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
// Deffered ��,��,�ֻ� �ɼ�	
void DxPieceManagerWLD::RenderEffFrameForDeffered( BOOL bLBuffer,
												  BOOL bReflect,
												  boost::shared_ptr<EntityRenderListSwap>& spEntityRenderListSwap )
{
	//NSPieceQuickSort::RenderEffFrameForDeffered( bLBuffer );
	//m_spInstancingMeshLM->RenderEffFrameForDeffered( bLBuffer );
	spEntityRenderListSwap->RenderEffFrameForDeffered( bLBuffer, bReflect );

	//if ( m_pPieceAABB )
	//{
		//m_spPieceAABB->RenderEffFrame_Instancing( pd3dDevice, sCV, bLBuffer );
	//}
}

void DxPieceManagerWLD::RenderDecalAndLodBlendDecal( LPDIRECT3DDEVICEQ pd3dDevice,
													BOOL bReflection,
													boost::shared_ptr<EntityRenderListSwap>& spEntityRenderListSwap )
{
	//NSPieceQuickSort::RenderDecalAndLodBlendDecal( pd3dDevice );
	//m_spInstancingMeshLM->RenderDecalAndLodBlendDecal( pd3dDevice );
	spEntityRenderListSwap->RenderDecalAndLodBlendDecal( pd3dDevice );

	if ( bReflection )
	{
		m_spInstancingMeshReflection->RenderDecal( pd3dDevice );
	}
	else
	{
		m_spInstancingMeshLM->RenderDecal( pd3dDevice );
	}

	//if ( m_pPieceAABB )
	//{
		//m_spPieceAABB->RenderDecal_Instancing( pd3dDevice, sCV );
	//}
}

void DxPieceManagerWLD::RenderDecalPass2( LPDIRECT3DDEVICEQ pd3dDevice,
										 BOOL bReflection,
										 boost::shared_ptr<EntityRenderListSwap>& spEntityRenderListSwap )
{
	//NSPieceQuickSort::RenderDecalPass2();
	//NSPieceQuickSort::RenderWaterCustomSoftAlpha();

	////if ( m_pPieceAABB )
	//{
	//	m_spPieceAABB->RenderWaterCustomSoftAlpha_Instancing( pd3dDevice, sCV );
	//}

	////if ( m_pPieceAABB )
	//{
	//	m_spPieceAABB->RenderDecalPass2_Instancing( pd3dDevice, sCV );
	//}

	spEntityRenderListSwap->RenderDecalPass2();

	if ( bReflection )
	{
		m_spInstancingMeshReflection->RenderDecalPass2( pd3dDevice );
	}
	else
	{
		m_spInstancingMeshLM->RenderDecalPass2( pd3dDevice );
	}

	spEntityRenderListSwap->RenderCustom_UNLIT_UNFOG();
	spEntityRenderListSwap->RenderWaterCustomSoftAlpha();
}

void DxPieceManagerWLD::RenderSoftAlpha_Deffered( LPDIRECT3DDEVICEQ pd3dDevice,
												 boost::shared_ptr<EntityRenderListSwap>& spEntityRenderListSwap )
{
	//NSPieceQuickSort::RenderSoftAlpha_Deffered( pd3dDevice );
	spEntityRenderListSwap->RenderSoftAlpha_Deffered( pd3dDevice );
}

void DxPieceManagerWLD::RenderEff_PickBlend_Deffered( LPDIRECT3DDEVICEQ pd3dDevice, 
													 DxStaticMeshLightMap* pStaticMeshLightMap,
													 boost::shared_ptr<EntityRenderListSwap>& spEntityRenderListSwap )
{
	//NSPieceQuickSort::RenderEff_PickBlend_Deffered( pd3dDevice, pStaticMeshLightMap );
	spEntityRenderListSwap->RenderEff_PickBlend_Deffered( pd3dDevice, pStaticMeshLightMap );
}

void DxPieceManagerWLD::RenderForLodBlend( LPDIRECT3DDEVICEQ pd3dDevice,
										  boost::shared_ptr<EntityRenderListSwap>& spEntityRenderListSwap,
										  BOOL bReflect,
										  float fReflectHeight )
{
	if( !GLOBAL_PARAM::IsValue( GP_PIECE ) )
		return;

	//NSPieceQuickSort::RenderForLodBlendForDeffered( pd3dDevice );
	spEntityRenderListSwap->RenderForLodBlend( pd3dDevice, bReflect, fReflectHeight );
}

void DxPieceManagerWLD::RenderSoftAlphaForLodBlend( LPDIRECT3DDEVICEQ pd3dDevice,
												   boost::shared_ptr<EntityRenderListSwap>& spEntityRenderListSwap )
{
	if( !GLOBAL_PARAM::IsValue( GP_PIECE ) )
		return;

	//NSPieceQuickSort::RenderSoftAlphaForLodBlendForDeffered( pd3dDevice );
	spEntityRenderListSwap->RenderSoftAlphaForLodBlend( pd3dDevice );
}
// Deffered ��,��,�ֻ� �ɼ�	
//////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////
// TnL_PS_3_0_CSM �̻�
void DxPieceManagerWLD::RenderCascadeShadow( LPDIRECT3DDEVICEQ pd3dDevice,
											NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType,
											boost::shared_ptr<EntityRenderListSwap>& spEntityRenderListSwap )
{
	// Opaque
	{
		pd3dDevice->SetVertexDeclaration( NSShadowRenderForMaterial::g_pDCRT_ONLYPOS );
		if ( NSShadowRenderForMaterial::FX_BeginPass( 0 ) )
		{
			spEntityRenderListSwap->RenderCacadeOpaque( pd3dDevice );
			NSShadowRenderForMaterial::FX_EndPass();
		}

		if ( emCullingRenderType < NSLandThread::TYPE_INSTANCING_ALL_SIZE )
		{
			pd3dDevice->SetVertexDeclaration( NSShadowRenderForMaterial::g_pDCRT_ONLYPOS_INSTANCING );
			if ( NSShadowRenderForMaterial::FX_BeginPass( 2 ) )
			{
				m_spInstancingPureMesh[emCullingRenderType]->RenderOpaque( pd3dDevice );
				NSShadowRenderForMaterial::FX_EndPass();
			}
		}
	}


	// HardAlpha
	{
		DxRenderStates::GetInstance().SetOnAlphaMap ( pd3dDevice );

		pd3dDevice->SetVertexDeclaration( NSShadowRenderForMaterial::g_pDCRT_POSTEX );
		if ( NSShadowRenderForMaterial::FX_BeginPass( 1 ) )
		{
			spEntityRenderListSwap->RenderCascadeHardAlpha( pd3dDevice );
			NSShadowRenderForMaterial::FX_EndPass();
		}

		if ( emCullingRenderType < NSLandThread::TYPE_INSTANCING_ALL_SIZE )
		{
			pd3dDevice->SetVertexDeclaration( NSShadowRenderForMaterial::g_pDCRT_POSTEX_INSTANCING );
			if ( NSShadowRenderForMaterial::FX_BeginPass( 3 ) )
			{
				m_spInstancingPureMesh[emCullingRenderType]->RenderHardAlpha( pd3dDevice );
				NSShadowRenderForMaterial::FX_EndPass();
			}
		}

		DxRenderStates::GetInstance().ReSetOnAlphaMap ( pd3dDevice );	
	}

	// Replace
	pd3dDevice->SetVertexDeclaration( NSShadowRenderForMaterial::g_pDCRT_POSTEX );
	if ( NSShadowRenderForMaterial::FX_BeginPass( 1 ) )
	{
		spEntityRenderListSwap->RenderCascadeShadowReplace();
		NSShadowRenderForMaterial::FX_EndPass();
	}
}
// TnL_PS_3_0_CSM �̻�
//////////////////////////////////////////////////////////////////////////




void DxPieceManagerWLD::PieceSort( const CLIPVOLUME &sCV, const D3DXVECTOR3& vFromPt, const D3DXVECTOR3& vLookatPt )
{
	NSPieceQuickSort::InitPickAlpha();		// Note : Init Static
	NSPieceQuickSort::Reset();

	//if ( m_pPieceAABB )
	{
		m_spPieceAABB->InsertQuickSort( sCV, vFromPt, vLookatPt, m_bLodBlending, TRUE, FALSE );
	}

	NSPieceQuickSort::QuickSortProcess();
}

void DxPieceManagerWLD::ResetOcclusionQuery()
{
	//if ( m_pPieceAABB )
	{
		m_spPieceAABB->ResetOcclusionQuery();
	}
}

void DxPieceManagerWLD::FindOcclusionRenderPieceIndex( LPDIRECT3DDEVICEQ pd3dDevice, VEC_DWORD& vecPieceIndex )
{
	//if ( m_pPieceAABB )
	{
		m_spPieceAABB->FindOcclusionRenderPieceIndex( pd3dDevice, vecPieceIndex );
	}
}

BOOL DxPieceManagerWLD::IsCollisionLine( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3& vCollision, EMCOLLMODE emCullMode, BOOL bOnlyCamColl ) const
{
	//if ( m_pPieceAABB )
	{
		return m_spPieceAABB->IsCollisionLine( vStart, vEnd, NULL, vCollision, emCullMode, bOnlyCamColl );
	}

	return FALSE;
}

BOOL DxPieceManagerWLD::IsCollisionLine2( const D3DXVECTOR3& vStart, 
										D3DXVECTOR3& vEnd, 
										D3DXVECTOR3& vCollision, 
										DxPiece** rrPiece,
                                        EMCOLLMODE emCullMode ) const
{
	//if ( m_pPieceAABB )
	{
		return m_spPieceAABB->IsCollisionLine2( vStart, vEnd, NULL, vCollision, rrPiece, emCullMode, FALSE );
	}

	return FALSE;
}

BOOL DxPieceManagerWLD::IsCollisionLineSimple( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode ) const
{
	//if ( m_pPieceAABB )
	{
		if ( m_spPieceAABB->IsCollisionLineSimple( vStart, vEnd, emCullMode ) )
			return TRUE;
	}

	return FALSE;
}

// Note : �浹�� �Ǹ� �ٷ� �����Ѵ�.
BOOL DxPieceManagerWLD::IsCollisionLinePixel( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode, int nIndexUV )
{
	//if ( m_pPieceAABB )
	{
		return m_spPieceAABB->IsCollisionLinePixel( pd3dDevice, vStart, vEnd, emCullMode, nIndexUV );
	}

    return FALSE;
}

BOOL DxPieceManagerWLD::GetLightMapColor( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, DWORD& dwColor )
{
	//if ( m_pPieceAABB )
	{
		return m_spPieceAABB->GetLightMapColor( pd3dDevice, vStart, vEnd, dwColor );
	}

	return FALSE;
}

void DxPieceManagerWLD::ReloadTexEffVMFX( LPDIRECT3DDEVICEQ pd3dDevice )
{
 //   MAPNAMEPIECE_ITER iter = m_mapNamePiece.begin();
	//for( ; iter!=m_mapNamePiece.end(); ++iter )
	//{
	//	(*iter).second->ReloadTexEffVMFX( pd3dDevice );
	//}
}

void DxPieceManagerWLD::SaveWLD( sc::SerialFile& SFile, float fInstancingCellSize )
{
    SFile << static_cast<DWORD>( VERSION_WLD );
    SFile.BeginBlock();
    {
		SFile << m_bUseLightMap;

		//if ( m_pPieceAABB )
		{
			SFile << (BOOL)TRUE;

			m_spPieceAABB->Save( SFile );

			SFile << fInstancingCellSize;
		}
		//else
		//{
		//	SFile << (BOOL)FALSE;
		//}
    }
    SFile.EndBlock();
}

void DxPieceManagerWLD::LoadWLD( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile )
{
	CleanUp();

    DWORD dwVer, dwBufferSize;
    SFile >> dwVer;
    SFile >> dwBufferSize;

	if ( dwVer == 0x104 )
	{
		SFile >> m_bUseLightMap;

		BOOL bExist;
		SFile >> bExist;
		if( bExist )
		{
			m_spPieceAABB->Load( pd3dDevice, SFile, TRUE );
		}

		// Render List Thread �۾��Ǹ鼭 Instancing �� ��� ����
		float fInstancingCellSize(g_fInstancingCellSize);
		SFile >> fInstancingCellSize;
		fInstancingCellSize = g_fInstancingCellSize;		// 10 ���� �����Ѵ�.
		//m_spPieceAABB->MakeTree();	//, fInstancingCellSize, TRUE, m_spInstancingMeshLM, m_spInstancingReflection );

		// Instancing �� ���� �����͸� �����Ѵ�.
		//if ( m_pPieceAABB )
		//{
		//	m_spPieceAABB->InitInstancing( pd3dDevice, m_spPieceInstancing );
		//}

		//NSLandThread::SetPieceInstancingBase( m_spInstancingMeshLM );
		//NSLandThread::SetPieceInstancingReflection( m_spInstancingReflection );
	}
	else if ( dwVer == 0x103 )
	{
		SFile >> m_bUseLightMap;

		BOOL bExist;
		SFile >> bExist;
		if( bExist )
		{
			m_spPieceAABB->Load( pd3dDevice, SFile, TRUE );
		}

		// Render List Thread �۾��Ǹ鼭 Instancing �� ��� ����
		// [shhan][2014.10.21] 103 �������� ������ Instancing �� �ȵǾ��� �߰��� �ǰԵǾ���.
		//						�̷��� �Ǹ鼭 ������ �߻��Ͽ� Instancing�� �����ϴ°����� ������ �ٲپ���.
		//m_spPieceAABB->MakeTree();	//, g_fInstancingCellSize, TRUE, m_spInstancingMeshLM, m_spInstancingReflection );

		//// Instancing �� ���� �����͸� �����Ѵ�.
		////if ( m_pPieceAABB )
		//{
		//	m_spPieceAABB->InitInstancing( pd3dDevice, m_spPieceInstancing );
		//}

		//NSLandThread::SetPieceInstancingBase( m_spInstancingMeshLM );
		//NSLandThread::SetPieceInstancingReflection( m_spInstancingReflection );
	}
	else if ( dwVer == 0x102 )
	{
		SFile >> m_bUseLightMap;

		BOOL bExist;
		SFile >> bExist;
		if( bExist )
		{
			m_spPieceAABB->Load( pd3dDevice, SFile, TRUE );
		}

		//m_spPieceAABB->MakeTree();

		//// Instancing �� ���� �����͸� �����Ѵ�.
		////if ( m_pPieceAABB )
		//{
		//	m_spPieceAABB->InitInstancing( pd3dDevice, m_spPieceInstancing );
		//}
	}
	else if ( dwVer == 0x101 )
	{
		SFile >> m_bUseLightMap;

		DWORD dwSize = 0;
		SFile >> dwSize;

		std::string szPieceName;
		for( DWORD i=0; i<dwSize; ++i )
		{
			SFile >> szPieceName;

			DxPieceAABB* pNew = new DxPieceAABB;
			pNew->Load_PREV2( pd3dDevice, SFile, szPieceName.c_str(), TRUE );

			m_spPieceAABB->AddPieceAABB_DeleteSrc( pd3dDevice, pNew );
			m_spPieceAABB->MakeTree();
			SAFE_DELETE( pNew );
		}
	}
	else if( dwVer == 0x100 )
	{
		DWORD dwSize = 0;
		SFile >> dwSize;

		std::string szPieceName;
		for( DWORD i=0; i<dwSize; ++i )
		{
			SFile >> szPieceName;

			DxPieceAABB* pNew = new DxPieceAABB;
			pNew->Load_PREV2( pd3dDevice, SFile, szPieceName.c_str(), TRUE );

			m_spPieceAABB->AddPieceAABB_DeleteSrc( pd3dDevice, pNew );
			m_spPieceAABB->MakeTree();
			SAFE_DELETE( pNew );
		}
	}
	else
	{
		SFile.SetOffSet( SFile.GetfTell() + dwBufferSize );
	}


	// NSLandThread 
	//NSLandThread::SetPieceAABB( m_spPieceAABB );

	if ( dwVer >= 0x102 )
	{
		m_bInstancingON = TRUE;
		//NSLandThread::SetPieceInstancingMeshLM( pd3dDevice, m_spPieceAABB.get(), m_spInstancingMeshLM );
		//NSLandThread::SetPieceInstancingMeshReflection( pd3dDevice, m_spPieceAABB.get(), m_spInstancingMeshReflection );

		//for ( DWORD i=0; i<NSLandThread::TYPE_INSTANCING_PURE_MESH_SIZE; ++i )
		//{
		//	NSLandThread::SetPieceInstancingPureMesh( pd3dDevice, m_spPieceAABB.get(), static_cast<NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE>(i), m_spInstancingPureMesh[i] );
		//}
	}
	else
	{
		m_bInstancingON = FALSE;
	}

	// �ʹݿ� ���� �� �־�� �Ǽ� �̰��� �ִ�.
	// ���� ��� ������ ������ ����.
	//m_spPieceAABB->FrameMoveStep( pd3dDevice, FALSE );

	//if ( dwVer >= 0x103 )
	//{
	//	m_bLodBlending = TRUE;
	//}
	//else
	//{
	//	m_bLodBlending = FALSE;
	//}

	// [shhan][2013.05.20] ���ʿ� LOD Blending �� �����ϱ� ����.
	m_bLodBlending = TRUE;
}

void DxPieceManagerWLD::LoadWLD_PREV( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile )
{
	CleanUp();

	DWORD dwSize = 0;
	SFile >> dwSize;

	std::string szPieceName;
	for( DWORD i=0; i<dwSize; ++i )
	{
		SFile >> szPieceName;

		DxPieceAABB* pNew = new DxPieceAABB;
		pNew->Load_PREV( pd3dDevice, SFile, szPieceName.c_str(), TRUE );

		m_spPieceAABB->AddPieceAABB_DeleteSrc( pd3dDevice, pNew );
		SAFE_DELETE( pNew );

		//m_mapNamePiece.insert( std::make_pair( szPieceName.c_str(), pNew ) );
	}

	//m_spPieceAABB->MakeTree( TRUE );

	m_spPieceAABB->MakeTree();

	m_bInstancingON = FALSE;
	NSLandThread::SetPieceAABB( m_spPieceAABB );

	// �ʹݿ� ���� �� �־�� �Ǽ� �̰��� �ִ�.
	// ���� ��� ������ ������ ����.
	//m_spPieceAABB->FrameMoveStep( pd3dDevice, FALSE );
}

void DxPieceManagerWLD::LoadBasicAABB( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin )
{
	//if ( !m_pPieceAABB )
	//	return;

	m_spPieceAABB->LoadBasicAABB( pd3dDevice, vMax, vMin );
}

void DxPieceManagerWLD::ClonePSFtoWLD( LPDIRECT3DDEVICEQ pd3dDevice, 
									  DxStaticMesh* pStaticMesh, 
									  DxPieceManagerPSF* pSrc, 
									  DxFogLocalMan* pFogLocalMan, 
									  BOOL bRan_1_0,
									  BOOL bLightMap, 
									  const TSTRING& strLightMapName,
									  TSTRING& strErrorMsg,
									  float fCellSize )
{
	CleanUp();

	m_bLodBlending = TRUE;
	m_bUseLightMap = bLightMap;

    // Texture Number Count �� �ؾ��Ѵ�.
    // Piece ���� �ƴ����� Ȯ���ؾ� �Ѵ�.
    NSLightMap::Init( TRUE );

	MAPNAMEPIECE mapNamePiece;

	TSTRING strPieceErrorMsg(_T(""));

    int nCount(1);
    DWORD dwStartTextureSize(512);
	MAPNAMEPIECE_ITER iter = pSrc->m_mapNamePiece.begin();
	for( ; iter!=pSrc->m_mapNamePiece.end(); ++iter, ++nCount )
	{
        // ���
        {
            TSTRINGSTREAM strStringStream;
            strStringStream << "Piece LightMap - ";
            strStringStream << nCount;
            strStringStream << "/";
            strStringStream << pSrc->m_mapNamePiece.size();
            ExportProgress::SetStateString( strStringStream.str().c_str() );
        }

		// ����� LightMap�� ���� Pixel �� ����ϰ� �ִ°�~?
		// ���� ���� LightMap �� ������� ������ �ʴ°�~?
		BOOL bEnoughUsedPixelLightMap(TRUE);

        //const DxPieceEdit* pPieceEdit = DxPieceContainer::GetInstance().LoadPiece( pd3dDevice, (*iter).first.c_str() );

		// LightMap �� ����ٸ�, ���⼭ �ϴ� ���� Mesh �и��� Texel ���� �÷������ �۾��ȴ�.
		// LightMap �� �� ����ٸ�, Piece �� �� �и��ϴ� �۾����ϰ� �ȴ�.
		DxPieceAABB* pNew = new DxPieceAABB;
		pNew->CloneData( pd3dDevice, (*iter).second, TRUE, strLightMapName, pStaticMesh, pSrc, dwStartTextureSize, bEnoughUsedPixelLightMap, fCellSize, TRUE );

		// ������� ���ٸ� ����Ѵ�.
		if ( !bEnoughUsedPixelLightMap )
		{
			strPieceErrorMsg += (*iter).first + _T(", ");
		}

		mapNamePiece.insert( std::make_pair( (*iter).first.c_str(), pNew ) );
	}

	if ( !strPieceErrorMsg.empty() )
	{
		strErrorMsg = _T("LightMap UV ������ ����� Ȱ�� ���� Piece �� - ");
		strErrorMsg += strPieceErrorMsg.c_str();
	}

    // LightMap �� ��� 2�ܰ� �۾��� ����.
    if ( bLightMap )
    {
        {
			// Piece �� Fit �ϰ� Resize �ϸ� ������ ����� ��찡 �ִ�.
			// 1,2,3,4,5(�������ؽ���) ���� 4,5 �� �̾��� �ְ� 5�� Fit �ϰ� �ٿ����� UV�� ������ �����.
            //// LightMap Texture�� Resize �Ѵ�
            //NSLightMap::ResizeLightMapTexture();

            // �ؽ��� ����.
            TextureManager::GetInstance().RescanPath();
        }

		// Texture ������(m_vecTextureData)�� ���� Piece���� ���� �÷��� �־��ش�.
		// LightMapRenderMan�� �����ϴ� �۾��� ���ش�.
        MAPNAMEPIECE_ITER iter = mapNamePiece.begin();
        for( ; iter!=mapNamePiece.end(); ++iter )
	    {
            (*iter).second->CreateLightMapPiece( pd3dDevice, strLightMapName, bRan_1_0, fCellSize );
        }

		// ���� �ؽ��ĸ� �����.
        // ������ �����۾�.
        NSLightMap::CreateLightMapFinal_PIECE( pd3dDevice, strLightMapName, _T("_P"), 0, _T("") );

        // NSLightMap�� �ؽ��� ����.
        TextureManager::GetInstance().RescanPath();
    }

	iter = mapNamePiece.begin();
	for( ; iter!=mapNamePiece.end(); ++iter, ++nCount )
	{
		// ���
		{
			TSTRINGSTREAM strStringStream;
			strStringStream << "Piece LightMap - ";
			strStringStream << nCount;
			strStringStream << "/";
			strStringStream << mapNamePiece.size();
			ExportProgress::SetStateString( strStringStream.str().c_str() );
		}

		// Piece ���� ����ϰԵȴ�.
		m_spPieceAABB->AddPieceAABB_DeleteSrc( pd3dDevice, (*iter).second );
	}

	// Tree�� ������.
	m_spPieceAABB->MakeTree();	//, fCellSize, TRUE, m_spInstancingMeshLM, m_spInstancingReflection );

	// LightMap Texture �ε� ��.
	m_spPieceAABB->LoadLightMap( pd3dDevice );

	Active_MATERIAL_DATA( pd3dDevice, FALSE );

	//// Instancing �� ���� �����͸� �����Ѵ�.
	//m_spPieceAABB->InitInstancing( pd3dDevice, m_spPieceInstancing );

	// �ʹݿ� ���� �� �־�� �Ǽ� �̰��� �ִ�.
	// ���� ��� ������ ������ ����.
	//m_spPieceAABB->FrameMoveStep( pd3dDevice, FALSE );

	// ������ ����
	std::for_each( mapNamePiece.begin(), mapNamePiece.end(), std_afunc::DeleteMapObject() );
	mapNamePiece.clear();
}

void DxPieceManagerWLD::RenderRadiosity( LPDIRECT3DDEVICEQ pd3dDevice, 
										DxLandMan* pLandMan,
										LPDIRECT3DTEXTUREQ pTextureSystem, 
										const TSTRING& strLightMapName,
										int nRadiosityPassCUR,
										float fRadiosityReflectance,
										float fRadiosityFogBlend,
										DWORD dwRadiosityFogLength )
{
	// Object ������
	{
		// �������� ���Ǵ� �����Ϳ� ���� �ִ´�.
		m_spPieceAABB->BakeRadiosity( pd3dDevice, pLandMan, pTextureSystem, fRadiosityReflectance, fRadiosityFogBlend, dwRadiosityFogLength, -1 );

		// ���� �ؽ��ĸ� �����.
		// ������ �����۾�.
		NSLightMap::CreateLightMapFinal_PIECE( pd3dDevice, strLightMapName, _T("_P"), nRadiosityPassCUR+1, _T("") );
	}

	// Voxel �� Color ����.
	{
		// BakeRadiosity �� �ϰ� ���Ǵ� �����Ϳ� ���� �ִ´�.
		m_spPieceAABB->AddRadiosityColorForVoxel( pd3dDevice, pLandMan, pTextureSystem, fRadiosityReflectance, fRadiosityFogBlend, dwRadiosityFogLength );
	}
}

void DxPieceManagerWLD::RefreshLightMap( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//if ( !m_pPieceAABB )
	//	return;

	// LightMap Texture �ε� ��.
	m_spPieceAABB->ReleaseLightMap();
	m_spPieceAABB->LoadLightMap( pd3dDevice );
}

void DxPieceManagerWLD::ResetAnimationTime_DisableLoopType()
{
	m_spPieceAABB->ResetAnimationTime_DisableLoopType();
}

void DxPieceManagerWLD::ExportXFile( LPDIRECTXFILESAVEOBJECT pxofsave, LPDIRECTXFILEDATA pDataObjectRoot )
{
	m_spPieceAABB->ExportXFile( pxofsave, pDataObjectRoot );
	//MAPNAMEPIECE_ITER iter = m_mapNamePiece.begin();
	//for( ; iter!=m_mapNamePiece.end(); ++iter )
	//{
	//	TSTRING strNewName("");
	//	TSTRING strPieceName = sc::util::GetName_ExtErase( (*iter).first );

	//	while( strNewName != strPieceName )
	//	{
	//		strPieceName = sc::util::GetChangeTCHAR( strPieceName, ' ', '_' );	// �۾���.
	//		strNewName = sc::util::GetChangeTCHAR( strPieceName, ' ', '_' );	// �� ������ �ִ��� Ȯ�� �� while �� �񱳸� ���� �۾���.
	//	}

	//	(*iter).second->ExportXFile( pxofsave, pDataObjectRoot, strPieceName );
	//}
}

// �α��� ȭ�� ���� ��� �������� ���� �ε��� �Ǿ� �ְ�, �� ��ȯ�� �Ǳ� ������ NSLandThread �������� ������ �Ǿ��Ѵ�.
void DxPieceManagerWLD::SetNSLandThread( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// NSLandThread 
	NSLandThread::SetPieceAABB( m_spPieceAABB );

	if ( m_bInstancingON )
	{
		NSLandThread::SetPieceInstancingMeshLM( pd3dDevice, m_spPieceAABB.get(), m_spInstancingMeshLM );
		NSLandThread::SetPieceInstancingMeshReflection( pd3dDevice, m_spPieceAABB.get(), m_spInstancingMeshReflection );

		for ( DWORD i=0; i<NSLandThread::TYPE_INSTANCING_PURE_MESH_SIZE; ++i )
		{
			NSLandThread::SetPieceInstancingPureMesh( pd3dDevice, m_spPieceAABB.get(), static_cast<NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE>(i), m_spInstancingPureMesh[i] );
		}
	}
}

void DxPieceManagerWLD::Archive( boost::function<void (const TCHAR*)> fun )
{
	m_spPieceAABB->Archive( fun );
    //BOOST_FOREACH(MAPCHANGEPIS_VALUE& it, m_mapChangePis)
    //{
    //    fun( (TSTRING(it.second->m_pPisName) + DxPieceContainer::GetInstance().GetPath()).c_str() );
    //}

    //BOOST_FOREACH(VECPOINTPIS::value_type& it, m_vecPointPis)
    //{
    //    fun( (TSTRING(it->m_pPisName) + DxPieceContainer::GetInstance().GetPath()).c_str() );
    //}
}
