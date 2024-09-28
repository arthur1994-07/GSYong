#include "pch.h"

#include "../../../SigmaCore/String/StringUtils.h"
#include "../../../SigmaCore/Log/LogMan.h"

#include "../../DxEffect/Mesh/DxStaticMesh.h"
#include "../../DxMeshs/FrameMesh/DxFrameMesh.h"
#include "../../DxTools/Material/NSMaterialManager.h"
#include "../../DxTools/Material/NSMaterialSetting.h"
#include "../../DxTools/Deffered/NSDeffered.h"
#include "../../DxTools/DxRenderStates.h"
#include "../../DxTools/DxSurfaceTex.h"
#include "../../DxTools/RENDERPARAM.h"
#include "../../DxTools/DxViewPort.h"
#include "../../DxTools/EDITMESHS.h"
#include "../../DxTools/DebugSet.h"
#include "../../DxLand/CollisionMap.h"
#include "../../Common/SubPath.h"
#include "../../Archive/Archive.h"

#include "../LightMap/NSLightMap.h"
#include "../Entity.h"
#include "./DxPieceStaticAni.h"
#include "./DxPieceEffMan.h"
#include "./DxPieceEff.h"

#include "./DxPieceEdit.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// 3500 기준으로 나온 값. 빗변, 50%, 110%
static const float g_fCELL_SIZE_PIECE = 2083.f;

// -----------------------------------------------------------------------------------------------------------------------------------------
//											D	x		P	i	e	c	e		E	d	i	t
// -----------------------------------------------------------------------------------------------------------------------------------------
DxPieceEdit::DxPieceEdit()
    : m_pd3dDevice(NULL)
	, m_pFrameMesh(NULL)
	, m_pCollisionMap(NULL)
	, m_pStaticAniFrame(NULL)
	, m_pPieceEffEDIT(NULL)
	, m_pPieceEff(NULL)
	, m_pTextureEff(NULL)
	, m_nRefCount(0)
	, m_fTime(0.f)
	, m_fAniTime(0.f)
	, m_vMax(-FLT_MAX,-FLT_MAX,-FLT_MAX)
	, m_vMin(FLT_MAX,FLT_MAX,FLT_MAX)
	, m_vStaticMax(-FLT_MAX,-FLT_MAX,-FLT_MAX)
	, m_vStaticMin(FLT_MAX,FLT_MAX,FLT_MAX)
	, m_vCenter(0.f,0.f,0.f)
	, m_fLength(1.f)
    , m_dwFlags(0L)
	, m_fVisibleRadius(DEFAULT_VISIBLE_RADIUS)
	, m_fInvisibleRadius(DEFAULT_INVISIBLE_RADIUS)
{
	m_strFileName = "";
	m_strFrameName = "";
	//D3DXMatrixIdentity( &m_matLocal );
}

DxPieceEdit::~DxPieceEdit()
{
	CleanUp();
}

void DxPieceEdit::CleanUp()
{
    m_dwFlags = 0L;

	SAFE_DELETE( m_pFrameMesh );
	SAFE_DELETE( m_pCollisionMap );
	SAFE_DELETE( m_pStaticAniFrame );
	SAFE_DELETE( m_pPieceEffEDIT );
	SAFE_DELETE( m_pPieceEff );
	SAFE_DELETE( m_pTextureEff );

	m_strFileName = "";
	m_strFrameName = "";
	m_fVisibleRadius = DEFAULT_VISIBLE_RADIUS;
	m_fInvisibleRadius = DEFAULT_INVISIBLE_RADIUS;
}

void DxPieceEdit::NewFrameMesh(LPDIRECT3DDEVICEQ pd3dDevice, const char* szName, const TSTRING& strPieceEditPath )
{
	SAFE_DELETE ( m_pFrameMesh );
	m_pFrameMesh = new DxFrameMesh(FALSE);

	// Note : 읽을 x파일과 버텍스 구조 정보 ( FVF ) 를 준다.
	m_pFrameMesh->SetFile( strPieceEditPath.c_str(), szName );
	m_pFrameMesh->SetFVF( VERTEX_WORLD::FVF );

	// Note : 초기화 한다.
	m_pFrameMesh->Create ( pd3dDevice );
	if ( !m_pFrameMesh->GetFrameRoot() )
	{
		SAFE_DELETE ( m_pFrameMesh );
		return;
	}

	m_pFrameMesh->UpdateFrames();

	// Note : MAX, MIN, CENTER, LENGTH 를 구한다.
	m_vStaticMax = m_vMax = m_pFrameMesh->GetTreeMax();
	m_vStaticMin = m_vMin = m_pFrameMesh->GetTreeMin();
	m_vCenter = (m_vMax+m_vMin)*0.5f;
	D3DXVECTOR3 vVector = m_vCenter - m_vMax;
	m_fLength = D3DXVec3Length( &vVector );

	// Note : DxAABBCollision 생성
	SAFE_DELETE( m_pCollisionMap );
	m_pCollisionMap = new CCollisionMap;
	m_pCollisionMap->SetDxFrameMesh( m_pFrameMesh );
	//m_pCollisionMap->Import( TRUE );

	// Note : DxPieceEff 생성
	SAFE_DELETE( m_pPieceEffEDIT );
	m_pPieceEffEDIT = new DxPieceEff;

	// Note : DxPieceEff 생성
	SAFE_DELETE( m_pPieceEff );
	m_pPieceEff = new DxPieceEff;

	// Note : DxPieceEff 생성
	SAFE_DELETE( m_pTextureEff );
	m_pTextureEff = new DxTextureEffMan;
	m_pTextureEff->Clone( pd3dDevice, m_pFrameMesh->GetTextureEffMan_const() );

	// Note : DxStaticAniFrame 생성		< MakeAniMan() 함수는 기본 AniFrame을 생성하기 위해 호출한다. >
	SAFE_DELETE( m_pStaticAniFrame );
	m_pStaticAniFrame = new DxStaticAniFrame;

	// [shhan][2013.07.10] 새로만들어지는 Piece의 경우 Animation 되는 Mesh 는 DxStaticMaterialMeshAniMan 에서 렌더되기 때문에 필요 없어짐.
	//m_pStaticAniFrame->MakeAniMan( pd3dDevice, m_pFrameMesh );

	m_pFrameMesh->CheckLightMapUV( m_pFrameMesh->GetFrameRoot() );
}

BOOL DxPieceEdit::IsCollision_NoneConst( const D3DXVECTOR3 &vP1, D3DXVECTOR3 &vP2, BOOL bEditor, const DxFrame* pFrame, char* pName )
{
	BOOL bUse = FALSE;
	if( m_pCollisionMap )
	{
		bUse = m_pCollisionMap->IsCollision_NoneConst( vP1, vP2, NULL, bEditor, pFrame, pName, EMCC_CULL_CCW );
	}
	return bUse;
}

DxFrame* const DxPieceEdit::GetCollisionDetectedFrame()
{
	DxFrame*	pFrame = NULL;
	if( m_pCollisionMap )
	{
		pFrame = m_pCollisionMap->GetCollisionDetectedFrame();
	}
	return pFrame;
}

const D3DXVECTOR3 DxPieceEdit::GetCollisionPos ()
{
	D3DXVECTOR3	vPos(0.f,0.f,0.f);
	if( m_pCollisionMap )
	{
		vPos = m_pCollisionMap->GetCollisionPos();
	}
	return vPos;
}

DxFrame* DxPieceEdit::GetFrameRoot()
{ 
	if (m_pFrameMesh)	return m_pFrameMesh->GetFrameRoot(); 
	else				return NULL;
}

BOOL DxPieceEdit::SetEff( LPDIRECT3DDEVICEQ pd3dDevice, const char* szFrameName, const D3DXVECTOR3& vPos, const char* szEffName, std::string& strEditName )
{
	if( !m_pFrameMesh )		return FALSE;
	if( !m_pPieceEffEDIT )	return FALSE;

	m_pFrameMesh->SetAnimationTimeZero();
	DxFrame* pFrame = m_pFrameMesh->GetAniFrame( szFrameName );
    if( m_pPieceEffEDIT->SetEff( pd3dDevice, pFrame, vPos, szFrameName, szEffName, strEditName ) )	
	{
		return TRUE;
	}

	return FALSE;
}

void DxPieceEdit::DelEff( const char* szEffName )
{
	if( !m_pPieceEffEDIT )		return;
	m_pPieceEffEDIT->DelEff( szEffName );
}

void DxPieceEdit::ModifyEff( LPDIRECT3DDEVICEQ pd3dDevice, const char* szEditName, const char* szEffName )
{
	if( !m_pPieceEffEDIT )		return;
	m_pPieceEffEDIT->ModifyEff( pd3dDevice, szEditName, szEffName );
}

LPD3DXMATRIX DxPieceEdit::GetEffMatrix( const char* szEditName )
{
	return m_pPieceEffEDIT->GetEffMatrix( szEditName );
}

void DxPieceEdit::SetCheckCollPoint( const char* szEffName, BOOL bUse )
{
	if( !m_pPieceEffEDIT )		return;
	m_pPieceEffEDIT->SetCheckCollPoint( szEffName, bUse );
}

BOOL DxPieceEdit::GetCheckCollPoint( const char* szEffName )
{
	if( !m_pPieceEffEDIT )		return FALSE;
	return m_pPieceEffEDIT->GetCheckCollPoint( szEffName );
}

const char* DxPieceEdit::GetEffName( const char* pEditName )
{
	if( !m_pPieceEffEDIT )		return NULL;
	return m_pPieceEffEDIT->GetEffName( pEditName );
}

void DxPieceEdit::ReloadTexEffVMFX( LPDIRECT3DDEVICEQ pd3dDevice )
{
    if ( m_pFrameMesh )
        m_pFrameMesh->ReloadTexEffVMFX( pd3dDevice );
}

void DxPieceEdit::OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( m_pPieceEffEDIT )
        m_pPieceEffEDIT->OnCreateDevice( pd3dDevice );
}

void DxPieceEdit::OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	if( m_pStaticAniFrame )
        m_pStaticAniFrame->OnResetDevice( pd3dDevice );

    if( m_pPieceEffEDIT )
        m_pPieceEffEDIT->OnResetDevice( pd3dDevice );

    if( m_pTextureEff )
        m_pTextureEff->OnResetDevice( pd3dDevice );
}

void DxPieceEdit::OnLostDevice()
{
	if( m_pStaticAniFrame )
        m_pStaticAniFrame->OnLostDevice();

    if( m_pPieceEffEDIT )
        m_pPieceEffEDIT->OnLostDevice();

    if( m_pTextureEff )
        m_pTextureEff->OnLostDevice();
}

void DxPieceEdit::OnDestroyDevice()
{
	if( m_pPieceEffEDIT )
        m_pPieceEffEDIT->OnDestroyDevice();

    if( m_pTextureEff )
        m_pTextureEff->OnDestroyDevice();
}

void DxPieceEdit::FrameMoveFrame( const float fTime, const float fElapsedTime )
{
	//if (m_pStaticAniFrame)
 //       m_pStaticAniFrame->FrameMove(fElapsedTime);

	if( m_pFrameMesh )	
	{
		m_pFrameMesh->SetAnimationTimeZero();
        if (m_pPieceEffEDIT)
		    m_pPieceEffEDIT->ModifyMatrix();
		m_pFrameMesh->FrameMove( fTime, fElapsedTime );
	}

	if (m_pPieceEffEDIT)
        m_pPieceEffEDIT->FrameMove( fTime, fElapsedTime );
}

void DxPieceEdit::RenderFrame( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, const D3DXMATRIX& matWorld, BOOL bEditMode )
{
	//if ( m_pFrameMesh && NSLightMapFX::g_emRenderType == NSLIGHTMAPDEF::LMRT_CHECKER )
	//{
	//	// 카메라 동작후 작동되어야 한다.
	//	NSLightMapFX::FrameMove( DxViewPort::GetInstance().GetMatView(), DxViewPort::GetInstance().GetMatProj(), 1.f );
	//	NSLightMapFX::SetLightMapUV_Offset( D3DXVECTOR2(0.f,0.f) );

	//	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
	//	m_pFrameMesh->RenderLightMapChecker( pd3dDevice, m_pFrameMesh->GetFrameRoot(), &sCV );
	//}
	//else
	{
		if( m_pFrameMesh )
		{
			pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
			m_pFrameMesh->Render( pd3dDevice, &matWorld, &sCV );
		}
		//if( m_pStaticAniFrame )
		//{
		//	m_pStaticAniFrame->Render_EDIT( pd3dDevice, m_matLocal );
		//}

		if( m_pPieceEffEDIT )
		{
			pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

			m_pPieceEffEDIT->SetMatrix();	// Frame 모드에서의 Eff를 위한 Matrix 수정
			m_pPieceEffEDIT->RenderSingleEff( pd3dDevice, matWorld, NULL, NULL, 1.f );
			m_pPieceEffEDIT->RenderMS( pd3dDevice, matWorld, NULL, FALSE );

			if ( bEditMode )
			{
				m_vMax = m_vStaticMax;
				m_vMin = m_vStaticMin;
				m_pPieceEffEDIT->SetAABBBox( m_vMax, m_vMin );		// Setting Max Min
			}
		}

		// Note : AABB 박스 테스트
		if ( bEditMode )
		{
			EDITMESHS::RENDERAABB( pd3dDevice, m_vMax, m_vMin );
		}
	}
}

void DxPieceEdit::RenderFrameAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matWorld, float fAlpha )
{
	DWORD dwAlphaBlendEnable;
	pd3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE, &dwAlphaBlendEnable );

	// Note : SetRenderState
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,		D3DBLEND_SRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND,	D3DBLEND_INVSRCALPHA );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

	// Note : Change for Alpha 0.5f
	D3DMATERIALQ sMaterial;
	sMaterial.Diffuse.r = sMaterial.Diffuse.g = sMaterial.Diffuse.b = 1.f;
	sMaterial.Ambient.r = sMaterial.Ambient.g = sMaterial.Ambient.b = 1.f;
	sMaterial.Specular.a = sMaterial.Specular.r = sMaterial.Specular.g = sMaterial.Specular.b = 0.f;
	sMaterial.Emissive.a = sMaterial.Emissive.r = sMaterial.Emissive.g = sMaterial.Emissive.b = 0.f;

	sMaterial.Diffuse.a = sMaterial.Ambient.a = fAlpha;
	pd3dDevice->SetMaterial( &sMaterial );

	if( m_pFrameMesh )
	{
		CLIPVOLUME sCV = DxViewPort::GetInstance().GetClipVolume();
		pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
		m_pFrameMesh->Render( pd3dDevice, &matWorld, &sCV );
	}
	if( m_pPieceEff )
	{
		m_pPieceEff->SetMatrix();
		m_pPieceEff->RenderSingleEff( pd3dDevice, matWorld, NULL, NULL, 1.f );
		m_pPieceEff->RenderMS( pd3dDevice, matWorld, NULL, FALSE );
	}

	// Note : ReSet Material
	sMaterial.Diffuse.a = sMaterial.Ambient.a = 1.f;
	pd3dDevice->SetMaterial( &sMaterial );

	// Note : SetRenderState
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, dwAlphaBlendEnable );
}

void DxPieceEdit::FrameMove( const float fTime, const float fElapsedTime )
{
	m_fTime += fElapsedTime;
	m_fAniTime += fElapsedTime * UNITANIKEY_PERSEC;
	if( m_fTime > 1.0e15f )		m_fTime = 0.f;
	if( m_fAniTime > 1.0e15f )	m_fAniTime = 0.f;

	if( m_pStaticAniFrame )	
	{
		m_pStaticAniFrame->FrameMoveTOOL( m_pd3dDevice, fElapsedTime );
	}
	if( m_pPieceEff )		m_pPieceEff->FrameMove( fTime, fElapsedTime );
	if( m_pTextureEff )		m_pTextureEff->FrameMove( fElapsedTime );
}

// [shhan][2016.12.21] Piece 를 PointSetting 시 마우스를 따라다니며 보여야 되서 matWorld 추가됨.
void DxPieceEdit::Render( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DCOLOR& colorClear, const D3DXMATRIX& matWorld )
{
	D3DXVECTOR4 vLightMapBoxColor[6];
	vLightMapBoxColor[0] = D3DXVECTOR4(1.f,1.f,1.f,1.f);
	vLightMapBoxColor[1] = D3DXVECTOR4(0.f,0.f,0.f,0.f);
	vLightMapBoxColor[2] = D3DXVECTOR4(1.f,1.f,1.f,1.f);
	vLightMapBoxColor[3] = D3DXVECTOR4(0.f,0.f,0.f,0.f);
	vLightMapBoxColor[4] = D3DXVECTOR4(1.f,1.f,1.f,1.f);
	vLightMapBoxColor[5] = D3DXVECTOR4(0.f,0.f,0.f,0.f);

	float fAmbientCubeSpot1(1.f);
	float fAmbientCubeSpot2(1.f);
	D3DXVECTOR4 vVoxelColor(1.f,1.f,1.f,0.f);
	D3DXVECTOR2 vSpot1_Spot2(1.f,1.f);

	D3DXVECTOR2 vLightMapUV_Offset( 1.f, 1.f );

	TriangleLinkLightForPiece* pNewTLLFP = new TriangleLinkLightForPiece;

	PieceParameter sParameter;
	sParameter.m_pd3dDevice = pd3dDevice;
	sParameter.m_rmatWorld = &matWorld;
	sParameter.m_fAniTime = m_fAniTime;
	sParameter.m_fAlpha = 1.f;
	sParameter.m_nRenderMode = NSMaterialManager::EMRM_TOOL;
	sParameter.m_sLMBCData.m_pAmbientCube = &vLightMapBoxColor[0];
	sParameter.m_sLMBCData.m_fAmbientCubeSpot1 = &fAmbientCubeSpot1;
	sParameter.m_sLMBCData.m_fAmbientCubeSpot2 = &fAmbientCubeSpot2;
	sParameter.m_sLMBCData.m_pVoxelColor = &vVoxelColor;
	sParameter.m_sLMBCData.m_pSpot1_Spot2 = &vSpot1_Spot2;
	sParameter.m_pTriangleLinkLight = pNewTLLFP;

	{
		//// BackUp 후 TnL_PS_3_0_DF 으로 동작되도록 하자.
		//BOOL bFORCE_TnL_PS_2_0_BACKUP = RENDERPARAM::g_bFORCE_TnL_PS_3_0_DF;
		//RENDERPARAM::g_bFORCE_TnL_PS_3_0_DF = TRUE;

		NSMaterialManager::FrameMove
		( 
			DxViewPort::GetInstance().GetMatView(), 
			DxViewPort::GetInstance().GetMatProj(), 
			DxSurfaceTex::GetInstance().m_pColorTex_1st, 
			DxSurfaceTex::GetInstance().m_pColorTex_2nd, 
			DxSurfaceTex::GetInstance().m_pColorTex_3rd, 
			DxSurfaceTex::GetInstance().m_pColorTex_4th
		);

		NSDeffered::RenderDefferedForPiece
		( 
			pd3dDevice, 
			colorClear,
			this,
			DxSurfaceTex::GetInstance().GetWidth(),
			DxSurfaceTex::GetInstance().GetHeight(),
			DxSurfaceTex::GetInstance().m_pColorSuf16F_HDR,
			DxSurfaceTex::GetInstance().m_pColorSuf_1st,
			DxSurfaceTex::GetInstance().m_pColorSuf_2nd,
			DxSurfaceTex::GetInstance().m_pColorSuf_3rd,
			DxSurfaceTex::GetInstance().m_pColorSuf_4th,
			DxSurfaceTex::GetInstance().m_pBurnNewSuf,
			DxSurfaceTex::GetInstance().m_pTexFourCC_INTZ,
			DxSurfaceTex::GetInstance().m_pColorTex_1st,
			DxSurfaceTex::GetInstance().m_pColorTex_2nd, 
			DxSurfaceTex::GetInstance().m_pColorTex_3rd,
			DxSurfaceTex::GetInstance().m_pColorTex_4th,
			DxSurfaceTex::GetInstance().m_pBurnNewTex,
			matWorld,
			DxViewPort::GetInstance().GetMatView(),
			DxViewPort::GetInstance().GetMatProj(),
			DxViewPort::GetInstance().GetClipVolume(),
			FALSE
		);

		NSDeffered::RenderDefferedForPiece
		( 
			pd3dDevice, 
			colorClear,
			this,
			DxSurfaceTex::GetInstance().GetWidth(),
			DxSurfaceTex::GetInstance().GetHeight(),
			DxSurfaceTex::GetInstance().m_pColorSuf16F_HDR,
			DxSurfaceTex::GetInstance().m_pColorSuf_1st,
			DxSurfaceTex::GetInstance().m_pColorSuf_2nd,
			DxSurfaceTex::GetInstance().m_pColorSuf_3rd,
			DxSurfaceTex::GetInstance().m_pColorSuf_4th,
			DxSurfaceTex::GetInstance().m_pBurnNewSuf,
			DxSurfaceTex::GetInstance().m_pTexFourCC_INTZ,
			DxSurfaceTex::GetInstance().m_pColorTex_1st,
			DxSurfaceTex::GetInstance().m_pColorTex_2nd, 
			DxSurfaceTex::GetInstance().m_pColorTex_3rd,
			DxSurfaceTex::GetInstance().m_pColorTex_4th,
			DxSurfaceTex::GetInstance().m_pBurnNewTex,
			matWorld,
			DxViewPort::GetInstance().GetMatView(),
			DxViewPort::GetInstance().GetMatProj(),
			DxViewPort::GetInstance().GetClipVolume(),
			TRUE
		);

		//// 백업된거 되돌림.
		//RENDERPARAM::g_bFORCE_TnL_PS_3_0_DF = bFORCE_TnL_PS_2_0_BACKUP;
	}

	if( m_pPieceEff )
	{
		m_pPieceEff->SetMatrix();
		m_pPieceEff->RenderSingleEff( pd3dDevice, matWorld, NULL, NULL, 1.f );
		m_pPieceEff->RenderMS( pd3dDevice, matWorld, &sParameter, FALSE );
	}

	SAFE_DELETE( pNewTLLFP );
}

void DxPieceEdit::RenderDeffered( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matWorld )
{
	D3DXVECTOR4 vLightMapBoxColor[6];
	vLightMapBoxColor[0] = D3DXVECTOR4(1.f,1.f,1.f,1.f);
	vLightMapBoxColor[1] = D3DXVECTOR4(0.f,0.f,0.f,0.f);
	vLightMapBoxColor[2] = D3DXVECTOR4(1.f,1.f,1.f,1.f);
	vLightMapBoxColor[3] = D3DXVECTOR4(0.f,0.f,0.f,0.f);
	vLightMapBoxColor[4] = D3DXVECTOR4(1.f,1.f,1.f,1.f);
	vLightMapBoxColor[5] = D3DXVECTOR4(0.f,0.f,0.f,0.f);

	float fAmbientCubeSpot1(1.f);
	float fAmbientCubeSpot2(1.f);
	D3DXVECTOR4 vVoxelColor(1.f,1.f,1.f,0.f);
	D3DXVECTOR2 vSpot1_Spot2(1.f,1.f);

	D3DXVECTOR2 vLightMapUV_Offset( 1.f, 1.f );

	TriangleLinkLightForPiece* pNewTLLFP = new TriangleLinkLightForPiece;

	PieceParameter sParameter;
	sParameter.m_pd3dDevice = pd3dDevice;
	sParameter.m_rmatWorld = &matWorld;
	sParameter.m_fAniTime = m_fAniTime;
	sParameter.m_fAlpha = 1.f;
	sParameter.m_nRenderMode = NSMaterialManager::EMRM_TOOL;
	sParameter.m_sLMBCData.m_pAmbientCube = &vLightMapBoxColor[0];
	sParameter.m_sLMBCData.m_fAmbientCubeSpot1 = &fAmbientCubeSpot1;
	sParameter.m_sLMBCData.m_fAmbientCubeSpot2 = &fAmbientCubeSpot2;
	sParameter.m_sLMBCData.m_pVoxelColor = &vVoxelColor;
	sParameter.m_sLMBCData.m_pSpot1_Spot2 = &vSpot1_Spot2;
	sParameter.m_pTriangleLinkLight = pNewTLLFP;

	if( m_pStaticAniFrame )
	{
		m_pStaticAniFrame->RenderMS_Opaque( sParameter, FALSE, FALSE );
		m_pStaticAniFrame->RenderMS_Opaque_Animation( sParameter, FALSE, FALSE, 0.f, FALSE );

		DxRenderStates::GetInstance().SetOnAlphaMap ( pd3dDevice );
		m_pStaticAniFrame->RenderMS_HardAlpha( sParameter, FALSE, FALSE );
		m_pStaticAniFrame->RenderMS_HardAlpha_Animation( sParameter, FALSE, FALSE );
		DxRenderStates::GetInstance().ReSetOnAlphaMap ( pd3dDevice );
	}
	SAFE_DELETE( pNewTLLFP );
}

void DxPieceEdit::RenderDefferedDecal( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matWorld )
{
	D3DXVECTOR4 vLightMapBoxColor[6];
	vLightMapBoxColor[0] = D3DXVECTOR4(1.f,1.f,1.f,1.f);
	vLightMapBoxColor[1] = D3DXVECTOR4(0.f,0.f,0.f,0.f);
	vLightMapBoxColor[2] = D3DXVECTOR4(1.f,1.f,1.f,1.f);
	vLightMapBoxColor[3] = D3DXVECTOR4(0.f,0.f,0.f,0.f);
	vLightMapBoxColor[4] = D3DXVECTOR4(1.f,1.f,1.f,1.f);
	vLightMapBoxColor[5] = D3DXVECTOR4(0.f,0.f,0.f,0.f);

	float fAmbientCubeSpot1(1.f);
	float fAmbientCubeSpot2(1.f);
	D3DXVECTOR4 vVoxelColor(1.f,1.f,1.f,0.f);
	D3DXVECTOR2 vSpot1_Spot2(1.f,1.f);

	D3DXVECTOR2 vLightMapUV_Offset( 1.f, 1.f );

	TriangleLinkLightForPiece* pNewTLLFP = new TriangleLinkLightForPiece;


	PieceParameter sParameter;
	sParameter.m_pd3dDevice = pd3dDevice;
	sParameter.m_rmatWorld = &matWorld;
	sParameter.m_fAniTime = m_fAniTime;
	sParameter.m_fAlpha = 1.f;
	sParameter.m_nRenderMode = NSMaterialManager::EMRM_TOOL;
	sParameter.m_sLMBCData.m_pAmbientCube = &vLightMapBoxColor[0];
	sParameter.m_sLMBCData.m_fAmbientCubeSpot1 = &fAmbientCubeSpot1;
	sParameter.m_sLMBCData.m_fAmbientCubeSpot2 = &fAmbientCubeSpot2;
	sParameter.m_sLMBCData.m_pVoxelColor = &vVoxelColor;
	sParameter.m_sLMBCData.m_pSpot1_Spot2 = &vSpot1_Spot2;
	sParameter.m_pTriangleLinkLight = pNewTLLFP;

	if( m_pStaticAniFrame )
	{
		m_pStaticAniFrame->RenderDecal( sParameter );
	}
	SAFE_DELETE( pNewTLLFP );
}

void DxPieceEdit::RenderDefferedSoftAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matWorld )
{
	D3DXVECTOR4 vLightMapBoxColor[6];
	vLightMapBoxColor[0] = D3DXVECTOR4(1.f,1.f,1.f,1.f);
	vLightMapBoxColor[1] = D3DXVECTOR4(0.f,0.f,0.f,0.f);
	vLightMapBoxColor[2] = D3DXVECTOR4(1.f,1.f,1.f,1.f);
	vLightMapBoxColor[3] = D3DXVECTOR4(0.f,0.f,0.f,0.f);
	vLightMapBoxColor[4] = D3DXVECTOR4(1.f,1.f,1.f,1.f);
	vLightMapBoxColor[5] = D3DXVECTOR4(0.f,0.f,0.f,0.f);

	float fAmbientCubeSpot1(1.f);
	float fAmbientCubeSpot2(1.f);
	D3DXVECTOR4 vVoxelColor(1.f,1.f,1.f,0.f);
	D3DXVECTOR2 vSpot1_Spot2(1.f,1.f);

	D3DXVECTOR2 vLightMapUV_Offset( 1.f, 1.f );

	TriangleLinkLightForPiece* pNewTLLFP = new TriangleLinkLightForPiece;

	PieceParameter sParameter;
	sParameter.m_pd3dDevice = pd3dDevice;
	sParameter.m_rmatWorld = &matWorld;
	sParameter.m_fAniTime = m_fAniTime;
	sParameter.m_fAlpha = 1.f;
	sParameter.m_nRenderMode = NSMaterialManager::EMRM_TOOL;
	sParameter.m_sLMBCData.m_pAmbientCube = &vLightMapBoxColor[0];
	sParameter.m_sLMBCData.m_fAmbientCubeSpot1 = &fAmbientCubeSpot1;
	sParameter.m_sLMBCData.m_fAmbientCubeSpot2 = &fAmbientCubeSpot2;
	sParameter.m_sLMBCData.m_pVoxelColor = &vVoxelColor;
	sParameter.m_sLMBCData.m_pSpot1_Spot2 = &vSpot1_Spot2;
	sParameter.m_pTriangleLinkLight = pNewTLLFP;

	if( m_pStaticAniFrame )
	{
		m_pStaticAniFrame->RenderMS_SoftAlphaForPSF( sParameter );
	}

	SAFE_DELETE( pNewTLLFP );
}

void DxPieceEdit::RenderDecalPass2( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matWorld )
{
	D3DXVECTOR4 vLightMapBoxColor[6];
	vLightMapBoxColor[0] = D3DXVECTOR4(1.f,1.f,1.f,1.f);
	vLightMapBoxColor[1] = D3DXVECTOR4(0.f,0.f,0.f,0.f);
	vLightMapBoxColor[2] = D3DXVECTOR4(1.f,1.f,1.f,1.f);
	vLightMapBoxColor[3] = D3DXVECTOR4(0.f,0.f,0.f,0.f);
	vLightMapBoxColor[4] = D3DXVECTOR4(1.f,1.f,1.f,1.f);
	vLightMapBoxColor[5] = D3DXVECTOR4(0.f,0.f,0.f,0.f);

	float fAmbientCubeSpot1(1.f);
	float fAmbientCubeSpot2(1.f);
	D3DXVECTOR4 vVoxelColor(1.f,1.f,1.f,0.f);
	D3DXVECTOR2 vSpot1_Spot2(1.f,1.f);

	D3DXVECTOR2 vLightMapUV_Offset( 1.f, 1.f );

	TriangleLinkLightForPiece* pNewTLLFP = new TriangleLinkLightForPiece;

	PieceParameter sParameter;
	sParameter.m_pd3dDevice = pd3dDevice;
	sParameter.m_rmatWorld = &matWorld;
	sParameter.m_fAniTime = m_fAniTime;
	sParameter.m_fAlpha = 1.f;
	sParameter.m_nRenderMode = NSMaterialManager::EMRM_TOOL;
	sParameter.m_sLMBCData.m_pAmbientCube = &vLightMapBoxColor[0];
	sParameter.m_sLMBCData.m_fAmbientCubeSpot1 = &fAmbientCubeSpot1;
	sParameter.m_sLMBCData.m_fAmbientCubeSpot2 = &fAmbientCubeSpot2;
	sParameter.m_sLMBCData.m_pVoxelColor = &vVoxelColor;
	sParameter.m_sLMBCData.m_pSpot1_Spot2 = &vSpot1_Spot2;
	sParameter.m_pTriangleLinkLight = pNewTLLFP;

	if( m_pStaticAniFrame )
	{
		m_pStaticAniFrame->RenderMS_DecalPass2( sParameter );
		m_pStaticAniFrame->Render_PIECE_Custom_UNLIT_UNFOG( sParameter );
		m_pStaticAniFrame->Render_PIECE_Water_CustomSoftAlpha( sParameter );;
	}

	SAFE_DELETE( pNewTLLFP );
}


//void DxPieceEdit::RenderAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matWorld, float fAlpha )
//{
//	DWORD dwAlphaBlendEnable;
//	pd3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE, &dwAlphaBlendEnable );
//
//	// Note : SetRenderState
//	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
//	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,		D3DBLEND_SRCALPHA );
//	pd3dDevice->SetRenderState( D3DRS_DESTBLEND,	D3DBLEND_INVSRCALPHA );
//
//	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
//
//	// Note : Change for Alpha 0.5f
//	D3DMATERIALQ sMaterial;
//	sMaterial.Diffuse.r = sMaterial.Diffuse.g = sMaterial.Diffuse.b = 1.f;
//	sMaterial.Ambient.r = sMaterial.Ambient.g = sMaterial.Ambient.b = 1.f;
//	sMaterial.Specular.a = sMaterial.Specular.r = sMaterial.Specular.g = sMaterial.Specular.b = 0.f;
//	sMaterial.Emissive.a = sMaterial.Emissive.r = sMaterial.Emissive.g = sMaterial.Emissive.b = 0.f;
//
//	sMaterial.Diffuse.a = sMaterial.Ambient.a = fAlpha;
//	pd3dDevice->SetMaterial( &sMaterial );
//
//	if( m_pStaticAniFrame )
//	{
//        D3DXMATRIX matIdentity;
//        D3DXMatrixIdentity( &matIdentity );
//		m_pStaticAniFrame->Render( pd3dDevice, matWorld, m_fAniTime, m_pTextureEff->GetMapTexEff(), matIdentity, NULL, NULL, fAlpha );
//		m_pStaticAniFrame->Render_Alpha( pd3dDevice, matWorld, m_pTextureEff->GetMapTexEff(), matIdentity, NULL, NULL, fAlpha );
//	}
//	if( m_pPieceEff )
//	{
//		m_pPieceEff->SetMatrix();
//		m_pPieceEff->Render( pd3dDevice, matWorld, NULL, NULL );
//	}
//
//	// Note : ReSet Material
//	sMaterial.Diffuse.a = sMaterial.Ambient.a = 1.f;
//	pd3dDevice->SetMaterial( &sMaterial );
//
//	// Note : SetRenderState
//	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, dwAlphaBlendEnable );
//}

void DxPieceEdit::SavePEF( const char* szName, const TSTRING& strXFileName )
{
	sc::SerialFile	SFile;

	BOOL bOpened = SFile.OpenFile( FOT_WRITE, szName );
	if ( !bOpened )
	{
		MessageBox ( NULL, "File Creation", "ERROR", MB_OK );
		return;
	}

	m_strFrameName = strXFileName;

	SFile << VERSION_PEF;

	SFile.BeginBlock();
	{
		SFile << m_vMin;
		SFile << m_vMax;
		SFile << m_strFrameName;
		SFile << m_fVisibleRadius;
		SFile << m_fInvisibleRadius;
        SFile << m_dwFlags;

		D3DXMATRIX matWorld;
		D3DXMatrixIdentity( &matWorld );
		SFile.WriteBuffer( &matWorld, sizeof(D3DXMATRIX) );

		{
			if ( m_pFrameMesh->GetFrameRoot() )
			{
				SFile << BOOL ( TRUE );
				DWORD	nNodeCount = 0;
				m_pFrameMesh->GetFrameRoot()->DxFrameNodeCount( nNodeCount );
				SFile << nNodeCount;

				SFile.BeginBlock();
				{
					m_pFrameMesh->GetFrameRoot()->SaveSet ( SFile );
				}
				SFile.EndBlock();
			}
			else
			{
				SFile << BOOL ( FALSE );
			}
		}

		// Frame 효과를 저장한다.
		m_pFrameMesh->SaveFrameEff( SFile );

		// Note : TextureEffMan을 저장 시킨다.
		m_pFrameMesh->GetTextureEffMan()->SavePSF( SFile );

		// Note : Material 에 저장시킨다.
		m_pFrameMesh->GetMaterialSetting()->SaveSet( SFile );
	}
	SFile.EndBlock();


	BOOL bExist;
	bExist = m_pPieceEffEDIT ? TRUE : FALSE;
	SFile << bExist;
	if( m_pPieceEffEDIT )	m_pPieceEffEDIT->SaveEdt( SFile, m_pFrameMesh );

	SFile.CloseFile();
}

HRESULT DxPieceEdit::LoadPEF(LPDIRECT3DDEVICEQ pd3dDevice, const char* szName, const TSTRING& strPieceEditPath )
{
	CleanUp();		// 초기화

	if (!strcmp(szName,""))
        return E_FAIL;

	TSTRING strName = strPieceEditPath;
	strName += szName;
	
	sc::SerialFile	SFile;
	BOOL bOpened = SFile.OpenFile( FOT_READ, strName.c_str() );
	if ( !bOpened )
	{
		MessageBox ( NULL, "File Creation", "ERROR", MB_OK );
		return E_FAIL;
	}

	DWORD dwVer;
	DWORD dwBufferSize;

	SFile >> dwVer;
	SFile >> dwBufferSize;

	if( dwVer== 0x0110 )
	{
		TSTRING strXFileLightMap;

		SFile >> m_vMin;
		SFile >> m_vMax;
		SFile >> m_strFrameName;
		SFile >> m_fVisibleRadius;
		SFile >> m_fInvisibleRadius;
		SFile >> m_dwFlags;

		D3DXMATRIX matTemp;
		SFile.ReadBuffer( &matTemp, sizeof(D3DXMATRIX) );

		// EDIT 시에만 FrameMesh를 로드한다.
		NewFrameMesh( pd3dDevice, m_strFrameName.c_str(), strPieceEditPath);

		{
			BOOL bExist;
			SFile >> bExist;
			if ( bExist )
			{
				int nNodeCount(0);
				SFile >> nNodeCount;

				DWORD m_dwBufferSizeMini;
				SFile >> m_dwBufferSizeMini;

				if( m_pFrameMesh && m_pFrameMesh->GetFrameRoot() )
				{
					DWORD dwNodeCountThis(0);
					m_pFrameMesh->GetFrameRoot()->DxFrameNodeCount( dwNodeCountThis );

					if ( dwNodeCountThis == nNodeCount )
					{
						m_pFrameMesh->GetFrameRoot()->LoadSet ( SFile, nNodeCount );
					}
					else
					{
						SFile.SetOffSet( SFile.GetfTell() + m_dwBufferSizeMini );
					}
				}
				else
				{
					SFile.SetOffSet( SFile.GetfTell() + m_dwBufferSizeMini );
				}
			}
		}

		// Frame의 효과를 로드한다.
		LoadFrameEff( pd3dDevice, SFile, m_pFrameMesh );

		// Note : TextureEffMan을 로드 시킨다.
		m_pFrameMesh->GetTextureEffMan()->LoadPSF( pd3dDevice, SFile, FALSE );

		SAFE_DELETE( m_pTextureEff );
		m_pTextureEff = new DxTextureEffMan;
		m_pTextureEff->Clone( pd3dDevice, m_pFrameMesh->GetTextureEffMan_const() );

		// Note : Material 를 로드한다.
		m_pFrameMesh->GetMaterialSetting()->LoadSet( SFile );
	}
	else if( dwVer== 0x0109 )
	{
		TSTRING strXFileLightMap;

		SFile >> m_vMin;
		SFile >> m_vMax;
		SFile >> m_strFrameName;
		SFile >> m_fVisibleRadius;
		SFile >> m_fInvisibleRadius;
		SFile >> m_dwFlags;
		
		D3DXMATRIX matTemp;
		SFile.ReadBuffer( &matTemp, sizeof(D3DXMATRIX) );

		// EDIT 시에만 FrameMesh를 로드한다.
		NewFrameMesh( pd3dDevice, m_strFrameName.c_str(), strPieceEditPath);

		{
			BOOL bExist;
			SFile >> bExist;
			if ( bExist )
			{
				int nNodeCount(0);
				SFile >> nNodeCount;

				DWORD m_dwBufferSizeMini;
				SFile >> m_dwBufferSizeMini;

				if( m_pFrameMesh && m_pFrameMesh->GetFrameRoot() )
				{
					DWORD dwNodeCountThis(0);
					m_pFrameMesh->GetFrameRoot()->DxFrameNodeCount( dwNodeCountThis );

					if ( dwNodeCountThis == nNodeCount )
					{
						m_pFrameMesh->GetFrameRoot()->LoadSet ( SFile, nNodeCount );
					}
					else
					{
						SFile.SetOffSet( SFile.GetfTell() + m_dwBufferSizeMini );
					}
				}
				else
				{
					SFile.SetOffSet( SFile.GetfTell() + m_dwBufferSizeMini );
				}
			}
		}

		// Frame의 효과를 로드한다.
		LoadFrameEff( pd3dDevice, SFile, m_pFrameMesh );

		// Note : TextureEffMan을 로드 시킨다.
		m_pFrameMesh->GetTextureEffMan()->LoadPSF( pd3dDevice, SFile, FALSE );

		SAFE_DELETE( m_pTextureEff );
		m_pTextureEff = new DxTextureEffMan;
		m_pTextureEff->Clone( pd3dDevice, m_pFrameMesh->GetTextureEffMan_const() );
	}
	else if( dwVer== 0x0108 )
	{
		TSTRING strXFileLightMap;

		SFile >> m_vMin;
		SFile >> m_vMax;
		SFile >> m_strFrameName;
		SFile >> m_dwFlags;
		
		D3DXMATRIX matTemp;
		SFile.ReadBuffer( &matTemp, sizeof(D3DXMATRIX) );

		// EDIT 시에만 FrameMesh를 로드한다.
		NewFrameMesh( pd3dDevice, m_strFrameName.c_str(), strPieceEditPath);

		{
			BOOL bExist;
			SFile >> bExist;
			if ( bExist )
			{
				int nNodeCount(0);
				SFile >> nNodeCount;

				DWORD m_dwBufferSizeMini;
				SFile >> m_dwBufferSizeMini;

				if( m_pFrameMesh && m_pFrameMesh->GetFrameRoot() )
				{
					DWORD dwNodeCountThis(0);
					m_pFrameMesh->GetFrameRoot()->DxFrameNodeCount( dwNodeCountThis );

					if ( dwNodeCountThis == nNodeCount )
					{
						m_pFrameMesh->GetFrameRoot()->LoadSet ( SFile, nNodeCount );
					}
					else
					{
						SFile.SetOffSet( SFile.GetfTell() + m_dwBufferSizeMini );
					}
				}
				else
				{
					SFile.SetOffSet( SFile.GetfTell() + m_dwBufferSizeMini );
				}
			}
		}

		// Frame의 효과를 로드한다.
		LoadFrameEff( pd3dDevice, SFile, m_pFrameMesh );

		// Note : TextureEffMan을 로드 시킨다.
		m_pFrameMesh->GetTextureEffMan()->LoadPSF( pd3dDevice, SFile, FALSE );

		SAFE_DELETE( m_pTextureEff );
		m_pTextureEff = new DxTextureEffMan;
		m_pTextureEff->Clone( pd3dDevice, m_pFrameMesh->GetTextureEffMan_const() );
	}
	else if( dwVer== 0x0107 )
	{
		TSTRING strXFileLightMap;

		SFile >> m_vMin;
		SFile >> m_vMax;
		SFile >> m_strFrameName;
		SFile >> strXFileLightMap;
        SFile >> m_dwFlags;
		
		D3DXMATRIX matTemp;
		SFile.ReadBuffer( &matTemp, sizeof(D3DXMATRIX) );

		// EDIT 시에만 FrameMesh를 로드한다.
		NewFrameMesh( pd3dDevice, m_strFrameName.c_str(), strPieceEditPath);

		// EDIT 시에만 FrameMeshUV를 로드한다.
		//NewLightMapUV( pd3dDevice, strXFileLightMap.c_str(), pPath );

		{
			BOOL bExist;
			SFile >> bExist;
			if ( bExist )
			{
				int nNodeCount(0);
				SFile >> nNodeCount;

				DWORD m_dwBufferSizeMini;
				SFile >> m_dwBufferSizeMini;

				if( m_pFrameMesh && m_pFrameMesh->GetFrameRoot() )
				{
					DWORD dwNodeCountThis(0);
					m_pFrameMesh->GetFrameRoot()->DxFrameNodeCount( dwNodeCountThis );

					if ( dwNodeCountThis == nNodeCount )
					{
						m_pFrameMesh->GetFrameRoot()->LoadSet ( SFile, nNodeCount );
					}
					else
					{
						SFile.SetOffSet( SFile.GetfTell() + m_dwBufferSizeMini );
					}
				}
				else
				{
					SFile.SetOffSet( SFile.GetfTell() + m_dwBufferSizeMini );
				}
			}
		}

		// Frame의 효과를 로드한다.
		LoadFrameEff( pd3dDevice, SFile, m_pFrameMesh );

		// Note : TextureEffMan을 로드 시킨다.
		m_pFrameMesh->GetTextureEffMan()->LoadPSF( pd3dDevice, SFile, FALSE );

		SAFE_DELETE( m_pTextureEff );
		m_pTextureEff = new DxTextureEffMan;
        m_pTextureEff->Clone( pd3dDevice, m_pFrameMesh->GetTextureEffMan_const() );

		// LightMap UV 제작한 것을 사용할 것인가 아닌가~?
		if ( strXFileLightMap.empty() )
		{
			m_dwFlags |= EMPO_UNSUPPORT_RAD;
		}
		else
		{
			m_dwFlags &= ~EMPO_UNSUPPORT_RAD;
		}
	}
	else if( dwVer== 0x0106 )
	{
		TSTRING strXFileLightMap;

		SFile >> m_vMin;
		SFile >> m_vMax;
		SFile >> m_strFrameName;
		SFile >> strXFileLightMap;
		SFile >> m_dwFlags;
		
		D3DXMATRIX matTemp;
		SFile.ReadBuffer( &matTemp, sizeof(D3DXMATRIX) );

		// EDIT 시에만 FrameMesh를 로드한다.
		NewFrameMesh( pd3dDevice, m_strFrameName.c_str(), strPieceEditPath);

		// Frame의 효과를 로드한다.
		LoadFrameEff( pd3dDevice, SFile, m_pFrameMesh );

		// Note : TextureEffMan을 로드 시킨다.
		m_pFrameMesh->GetTextureEffMan()->LoadPSF( pd3dDevice, SFile, FALSE );

		SAFE_DELETE( m_pTextureEff );
		m_pTextureEff = new DxTextureEffMan;
		m_pTextureEff->Clone( pd3dDevice, m_pFrameMesh->GetTextureEffMan_const() );

		// LightMap UV 제작한 것을 사용할 것인가 아닌가~?
		if ( strXFileLightMap.empty() )
		{
			m_dwFlags |= EMPO_UNSUPPORT_RAD;
		}
		else
		{
			m_dwFlags &= ~EMPO_UNSUPPORT_RAD;
		}
	}
	else if( dwVer== 0x0105 )
	{
		SFile >> m_vMin;
		SFile >> m_vMax;
		SFile >> m_strFrameName;
		SFile >> m_dwFlags;
		
		D3DXMATRIX matTemp;
		SFile.ReadBuffer( &matTemp, sizeof(D3DXMATRIX) );

		// EDIT 시에만 FrameMesh를 로드한다.
		NewFrameMesh( pd3dDevice, m_strFrameName.c_str(), strPieceEditPath);

		// Frame의 효과를 로드한다.
		LoadFrameEff( pd3dDevice, SFile, m_pFrameMesh );

		// Note : TextureEffMan을 로드 시킨다.
		m_pFrameMesh->GetTextureEffMan()->LoadPSF( pd3dDevice, SFile, FALSE );

		SAFE_DELETE( m_pTextureEff );
		m_pTextureEff = new DxTextureEffMan;
		m_pTextureEff->Clone( pd3dDevice, m_pFrameMesh->GetTextureEffMan_const() );
	}
    else if( dwVer== 0x0104 )
	{
		SFile >> m_vMin;
		SFile >> m_vMax;
		SFile >> m_strFrameName;
        SFile >> m_dwFlags;
		
		D3DXMATRIX matTemp;
		SFile.ReadBuffer( &matTemp, sizeof(D3DXMATRIX) );

		// EDIT 시에만 FrameMesh를 로드한다.
		NewFrameMesh( pd3dDevice, m_strFrameName.c_str(), strPieceEditPath);

		// Frame의 효과를 로드한다.
		LoadFrameEff( pd3dDevice, SFile, m_pFrameMesh );

		// Note : TextureEffMan을 로드 시킨다.
		m_pFrameMesh->GetTextureEffMan()->LoadPSF_VER100( pd3dDevice, SFile );

		SAFE_DELETE( m_pTextureEff );
		m_pTextureEff = new DxTextureEffMan;
		m_pTextureEff->Clone( pd3dDevice, m_pFrameMesh->GetTextureEffMan_const() );
	}
    else if( dwVer== 0x0103 )
	{
		SFile >> m_vMin;
		SFile >> m_vMax;
		SFile >> m_strFrameName;
		
		D3DXMATRIX matTemp;
		SFile.ReadBuffer( &matTemp, sizeof(D3DXMATRIX) );

        m_dwFlags = EMPO_PICK_ALPHA;

		// EDIT 시에만 FrameMesh를 로드한다.
		NewFrameMesh( pd3dDevice, m_strFrameName.c_str(), strPieceEditPath);

		// Frame의 효과를 로드한다.
		LoadFrameEff( pd3dDevice, SFile, m_pFrameMesh );

		// Note : TextureEffMan을 로드 시킨다.
		m_pFrameMesh->GetTextureEffMan()->LoadPSF_VER100( pd3dDevice, SFile );

		SAFE_DELETE( m_pTextureEff );
		m_pTextureEff = new DxTextureEffMan;
		m_pTextureEff->Clone( pd3dDevice, m_pFrameMesh->GetTextureEffMan_const() );
	}
	else if( dwVer==0x0102 || dwVer==0x0101 )
	{
		SFile >> m_vMin;
		SFile >> m_vMax;

		D3DXMATRIX matTemp;
		SFile.ReadBuffer( &matTemp, sizeof(D3DXMATRIX) );

		SFile >> m_strFrameName;

        m_dwFlags = EMPO_PICK_ALPHA;

		// EDIT 시에만 FrameMesh를 로드한다.
		NewFrameMesh( pd3dDevice, m_strFrameName.c_str(), strPieceEditPath );

		// Frame의 효과를 로드한다.
		LoadFrameEff( pd3dDevice, SFile, m_pFrameMesh );

		// Note : TextureEffMan을 로드 시킨다.
		m_pFrameMesh->GetTextureEffMan()->LoadPSF_VER100( pd3dDevice, SFile );

		SAFE_DELETE( m_pTextureEff );
		m_pTextureEff = new DxTextureEffMan;
		m_pTextureEff->Clone( pd3dDevice, m_pFrameMesh->GetTextureEffMan_const() );
	}
	else if( dwVer==0x0100 )
	{
		SFile >> m_vMin;
		SFile >> m_vMax;
		
		D3DXMATRIX matTemp;
		SFile.ReadBuffer( &matTemp, sizeof(D3DXMATRIX) );

		SFile >> m_strFrameName;

        m_dwFlags = EMPO_PICK_ALPHA;

		// EDIT 시에만 FrameMesh를 로드한다.
		NewFrameMesh( pd3dDevice, m_strFrameName.c_str(), strPieceEditPath );

		// Note : TextureEffMan을 로드 시킨다.
		m_pFrameMesh->GetTextureEffMan()->LoadPSF_VER100( pd3dDevice, SFile );

		SAFE_DELETE( m_pTextureEff );
		m_pTextureEff = new DxTextureEffMan;
		m_pTextureEff->Clone( pd3dDevice, m_pFrameMesh->GetTextureEffMan_const() );
	}
	else
	{
		DWORD dwCur = SFile.GetfTell();
		SFile.SetOffSet( dwCur+dwBufferSize );
	}

	// Note : m_pPieceEffEDIT 내부에서 Version 체크가 일어난다.
	BOOL bExist;
	SFile >> bExist;
	if ( bExist )
	{
		SAFE_DELETE( m_pPieceEffEDIT );
		m_pPieceEffEDIT = new DxPieceEff;
		m_pPieceEffEDIT->LoadEdt( pd3dDevice, SFile, m_pFrameMesh );
	}

	SFile.CloseFile();

	return S_OK;
}

void DxPieceEdit::LoadFrameEff( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, DxFrameMesh* pFrameMesh )
{
	// Note : Replace Effect
	BOOL	bResult;
	SFile >> bResult;
	while ( bResult )
	{
		char	*szName = NULL;
		int		StrLength;

		SFile >> bResult;
		if ( bResult )
		{
			SFile >> StrLength;
			szName = new char [ StrLength ];
			SFile.ReadBuffer ( szName, StrLength );
		}

		DWORD	TypeID;
		SFile >> TypeID;
		
		DWORD	dwSize = 0;
		BYTE	*pProp = NULL;

		DWORD	dwVer = 0;

		SFile >> dwVer;
		SFile >> dwSize;
		if ( dwSize )
		{
			pProp = new BYTE [ dwSize ];
			SFile.ReadBuffer ( pProp, sizeof ( BYTE ) * dwSize );
		}

		LPDXAFFINEPARTS	pAffineParts = NULL;
		SFile >> bResult;		
		if ( bResult )
		{
			pAffineParts = new DXAFFINEPARTS;
			SFile.ReadBuffer ( pAffineParts, sizeof ( DXAFFINEPARTS ) );
		}
		
		DxFrame	*pDxFrame=NULL;
		if ( szName ) pDxFrame = pFrameMesh->FindFrame( szName );

		if ( szName && pDxFrame )
		{
			DxEffectBase *pEffect = DxPieceEffMan::GetInstance().AdaptToDxFrame( TypeID, pFrameMesh, pDxFrame, pd3dDevice,
																				pProp, dwSize, dwVer, pAffineParts );

			if ( !pEffect )
			{
				DWORD NODESIZE;
				SFile >> NODESIZE;
				SFile.SetOffSet ( SFile.GetfTell() + NODESIZE );

				CDebugSet::ToFile ( "LoadSet.log", "   [EFF TYPEID %d] 인스턴스 생성에 실패.", TypeID );
				CDebugSet::ToFile ( "LoadSet.log", "   [EFF TYPEID %d] NunNamed Effect Buffer Skip..", TypeID );
			}
			else
			{
				CDebugSet::ToFile ( "LoadSet.log", "   [EFF TYPEID %x] 인스턴스 생성.", TypeID );
			}

			//	Note : LoadBuffer(), 특정 Effect 에서만 사용되는 메모리 버퍼를 읽는다.
			pEffect->LoadBufferSet ( SFile, dwVer, pd3dDevice );

			pEffect->CleanUp ();
			pEffect->Create ( pd3dDevice );
		}
		else
		{
			DWORD NODESIZE;
			SFile >> NODESIZE;
			SFile.SetOffSet ( SFile.GetfTell() + NODESIZE );

			if ( szName )	CDebugSet::ToFile ( "LoadSet.log", "[EFF TYPEID %d] DxFrame for Adapt [%s]...Not Found", TypeID, szName );
			else			CDebugSet::ToFile ( "LoadSet.log", "   [EFF TYPEID %d] NunNamed Effect Buffer Skip..", TypeID );
		}

		SAFE_DELETE_ARRAY ( pProp );
		SAFE_DELETE ( pAffineParts );
		SAFE_DELETE_ARRAY ( szName );

		SFile >> bResult;
	}
}

void DxPieceEdit::SavePIE( const char* szName )
{
	sc::SerialFile	SFile;

	TSTRING strName(szName);
	if ( m_pStaticAniFrame )
	{
		if ( m_pStaticAniFrame->IsMaterialMesh() )
		{
			// 확장자를 약간 변경해준다.
			CString strDest;
			STRUTIL::ChangeExt( szName, strDest, _T(".pie15") );
			strName = strDest.GetString();
		}
	}

	BOOL bOpened = SFile.OpenFile( FOT_WRITE, strName.c_str() );
	if ( !bOpened )
	{
		MessageBox ( NULL, "File Creation", "ERROR", MB_OK );
		return;
	}

	SFile << VERSION_PIE;

	SFile.BeginBlock();
	{
		SFile << m_vStaticMin;
		SFile << m_vStaticMax;
		SFile << m_vMin;
		SFile << m_vMax;
		SFile << m_vCenter;
		SFile << m_fLength;
		SFile << m_fVisibleRadius;
		SFile << m_fInvisibleRadius;
        SFile << m_dwFlags;
	}
	SFile.EndBlock();

	BOOL bExist;
	bExist = m_pStaticAniFrame ? TRUE : FALSE;
	SFile << bExist;
	if( m_pStaticAniFrame )	m_pStaticAniFrame->Save( SFile );

	bExist = m_pPieceEff ? TRUE : FALSE;
	SFile << bExist;
	if( m_pPieceEff )	m_pPieceEff->Save( SFile );

	bExist = m_pTextureEff ? TRUE : FALSE;
	SFile << bExist;
	if( m_pTextureEff )	
        m_pTextureEff->SavePSF( SFile );
	
	// 더미
	SFile << (BOOL)FALSE;

	SFile.CloseFile();
}

HRESULT DxPieceEdit::LoadPIE_0100( sc::SerialFile& SFile )
{
	SFile >> m_vMin;
	SFile >> m_vMax;
	
	D3DXMATRIX matTemp;
	SFile.ReadBuffer( &matTemp, sizeof(D3DXMATRIX) );

    m_dwFlags = EMPO_PICK_ALPHA;

	m_vStaticMin = m_vMin;
	m_vStaticMax = m_vMax;
	m_vCenter = (m_vMax+m_vMin)*0.5f;
	D3DXVECTOR3 vVector = m_vCenter - m_vMax;
	m_fLength = D3DXVec3Length( &vVector );

	return S_OK;
}

HRESULT DxPieceEdit::LoadPIE_0101( sc::SerialFile& SFile )
{
	SFile >> m_vMin;	
	SFile >> m_vMax;
	
	D3DXMATRIX matTemp;
	SFile.ReadBuffer( &matTemp, sizeof(D3DXMATRIX) );

    m_dwFlags = EMPO_PICK_ALPHA;

	m_vStaticMin = m_vMin;
	m_vStaticMax = m_vMax;
	m_vCenter = (m_vMax+m_vMin)*0.5f;
	D3DXVECTOR3 vVector = m_vCenter - m_vMax;
	m_fLength = D3DXVec3Length( &vVector );
	
	return S_OK;
}

HRESULT DxPieceEdit::LoadPIE_0102( sc::SerialFile& SFile )
{
	SFile >> m_vMin;
	SFile >> m_vMax;
	SFile >> m_vStaticMin;
	SFile >> m_vStaticMax;
	SFile >> m_vCenter;
	SFile >> m_fLength;
	
	D3DXMATRIX matTemp;
	SFile.ReadBuffer( &matTemp, sizeof(D3DXMATRIX) );

    m_dwFlags = EMPO_PICK_ALPHA;

	return S_OK;
}

HRESULT DxPieceEdit::LoadPIE_0103( sc::SerialFile& SFile )
{
	SFile >> m_vStaticMin;
	SFile >> m_vStaticMax;
	SFile >> m_vMin;
	SFile >> m_vMax;
	
	D3DXMATRIX matTemp;
	SFile.ReadBuffer( &matTemp, sizeof(D3DXMATRIX) );

	SFile >> m_vCenter;
	SFile >> m_fLength;

    m_dwFlags = EMPO_PICK_ALPHA;

	return S_OK;
}

HRESULT DxPieceEdit::LoadPIE_0104_0106( sc::SerialFile& SFile )
{
	SFile >> m_vStaticMin;
	SFile >> m_vStaticMax;
	SFile >> m_vMin;
	SFile >> m_vMax;
	
	D3DXMATRIX matTemp;
	SFile.ReadBuffer( &matTemp, sizeof(D3DXMATRIX) );

	SFile >> m_vCenter;
	SFile >> m_fLength;
    SFile >> m_dwFlags;

	return S_OK;
}

HRESULT DxPieceEdit::LoadPIE_0107( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, const TSTRING& strPieceEditPath )
{
	SFile >> m_vStaticMin;
	SFile >> m_vStaticMax;
	SFile >> m_vMin;
	SFile >> m_vMax;
	
	D3DXMATRIX matTemp;
	SFile.ReadBuffer( &matTemp, sizeof(D3DXMATRIX) );

	SFile >> m_vCenter;
	SFile >> m_fLength;
	SFile >> m_dwFlags;

	SFile >> m_strFrameName;

	// EDIT 시에만 FrameMesh를 로드한다.
	NewFrameMesh( pd3dDevice, m_strFrameName.c_str(), strPieceEditPath );

	{
		BOOL bExist;
		SFile >> bExist;
		if ( bExist )
		{
			int nNodeCount(0);
			SFile >> nNodeCount;

			DWORD m_dwBufferSizeMini;
			SFile >> m_dwBufferSizeMini;

			if( m_pFrameMesh && m_pFrameMesh->GetFrameRoot() )
			{
				DWORD dwNodeCountThis(0);
				m_pFrameMesh->GetFrameRoot()->DxFrameNodeCount( dwNodeCountThis );

				if ( dwNodeCountThis == nNodeCount )
				{
					m_pFrameMesh->GetFrameRoot()->LoadSet ( SFile, nNodeCount );
				}
				else
				{
					SFile.SetOffSet( SFile.GetfTell() + m_dwBufferSizeMini );
				}
			}
			else
			{
				SFile.SetOffSet( SFile.GetfTell() + m_dwBufferSizeMini );
			}
		}
	}

	return S_OK;
}

HRESULT DxPieceEdit::LoadPIE_0108( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, const TSTRING& strPieceEditPath )
{
	SFile >> m_vStaticMin;
	SFile >> m_vStaticMax;
	SFile >> m_vMin;
	SFile >> m_vMax;
	
	D3DXMATRIX matTemp;
	SFile.ReadBuffer( &matTemp, sizeof(D3DXMATRIX) );

	SFile >> m_vCenter;
	SFile >> m_fLength;
	SFile >> m_fVisibleRadius;
	SFile >> m_fInvisibleRadius;
	SFile >> m_dwFlags;

	SFile >> m_strFrameName;

	// EDIT 시에만 FrameMesh를 로드한다.
	NewFrameMesh( pd3dDevice, m_strFrameName.c_str(), strPieceEditPath );

	{
		BOOL bExist;
		SFile >> bExist;
		if ( bExist )
		{
			int nNodeCount(0);
			SFile >> nNodeCount;

			DWORD m_dwBufferSizeMini;
			SFile >> m_dwBufferSizeMini;

			if( m_pFrameMesh && m_pFrameMesh->GetFrameRoot() )
			{
				DWORD dwNodeCountThis(0);
				m_pFrameMesh->GetFrameRoot()->DxFrameNodeCount( dwNodeCountThis );

				if ( dwNodeCountThis == nNodeCount )
				{
					m_pFrameMesh->GetFrameRoot()->LoadSet ( SFile, nNodeCount );
				}
				else
				{
					SFile.SetOffSet( SFile.GetfTell() + m_dwBufferSizeMini );
				}
			}
			else
			{
				SFile.SetOffSet( SFile.GetfTell() + m_dwBufferSizeMini );
			}
		}
	}

	return S_OK;
}

HRESULT DxPieceEdit::LoadPIE_0109( sc::SerialFile& SFile )
{
	SFile >> m_vStaticMin;
	SFile >> m_vStaticMax;
	SFile >> m_vMin;
	SFile >> m_vMax;
	SFile >> m_vCenter;
	SFile >> m_fLength;
	SFile >> m_fVisibleRadius;
	SFile >> m_fInvisibleRadius;
	SFile >> m_dwFlags;

	return S_OK;
}


HRESULT DxPieceEdit::LoadPIE( LPDIRECT3DDEVICEQ pd3dDevice, const char* szFullPath, const TSTRING& strPieceEditPath, const TSTRING& strFileName, boost::function<void (const TCHAR*)> pArchive )
{
	CleanUp();		// 초기화

	if (!strcmp(szFullPath,""))
        return E_FAIL;
	
    SPSerialFile spFile;

    if (Archive::CreateSerialFile(pArchive, spFile) == false)
    {
        return E_FAIL;
    }

    sc::SerialFile& SFile = *spFile;

    BOOL bOpened = SFile.OpenFile( FOT_READ, szFullPath );

    if ( !bOpened )
    {
        std::string ErrorMsg(
            sc::string::format(
            "piece file load fail : %1%", szFullPath));
        CDebugSet::ToLogFile(ErrorMsg);
        return E_FAIL;
    }    

	DWORD dwVer;
	DWORD dwBufferSize;

	SFile >> dwVer;
	SFile >> dwBufferSize;

	switch ( dwVer )
	{
	case 0x0100:	
		LoadPIE_0100( SFile );	    
		break;

	case 0x0101:	
		LoadPIE_0101( SFile );	    
		break;

	case 0x0102:	
		LoadPIE_0102( SFile );	   
		break;

    case 0x0103:	
		LoadPIE_0103( SFile );	   
		break;

	case 0x0104:	
	case 0x0105:
    case 0x0106:
		LoadPIE_0104_0106( SFile );	
		break;

	case 0x0107:
		LoadPIE_0107( pd3dDevice, SFile, strPieceEditPath );
		break;

	case 0x0108:
		LoadPIE_0108( pd3dDevice, SFile, strPieceEditPath );
		break;

	case 0x0109:
		LoadPIE_0109( SFile );
		break;

	default:
		{
			DWORD dwCur = SFile.GetfTell();
			SFile.SetOffSet( dwCur+dwBufferSize );
		}
		break;
	};

	// 버젼 0x0116 이하면, LightMap이 지원되는 Piece 가 아니다. ( DxStaticMaterialMesh 을 쓸 수 없다. )
	if( dwVer<=0x0106 )
	{
		m_dwFlags |= EMPO_UNSUPPORT_RAD;
	}

	// 조각파일 이름을 셋팅함.
	m_strFileName = strFileName;

	// Note : 내부에서 Version 체크가 일어난다.
	BOOL bExist;
	SFile >> bExist;
	if ( bExist )
	{
		SAFE_DELETE( m_pStaticAniFrame );
		m_pStaticAniFrame = new DxStaticAniFrame;
		m_pStaticAniFrame->Load( pd3dDevice, SFile, pArchive );

		if( 0x0109>=dwVer )
		{
			if ( m_pStaticAniFrame->CheckOnlyVoxel() )
			{
				m_dwFlags |= EMPO_ONLY_VOXELCOLOR;
			}
		}
	}

	SFile >> bExist;
	if ( bExist )
	{
		SAFE_DELETE( m_pPieceEff );
		m_pPieceEff = new DxPieceEff;
		m_pPieceEff->Load( pd3dDevice, SFile, m_pStaticAniFrame->GetAniManHead() );
	}

    if ( dwVer >= 0x0105 )
    {
	    SFile >> bExist;
	    if ( bExist )
	    {
		    SAFE_DELETE( m_pTextureEff );
		    m_pTextureEff = new DxTextureEffMan;
		    m_pTextureEff->LoadPSF( pd3dDevice, SFile, TRUE );
	    }
    }
    else
    {
        SFile >> bExist;
	    if ( bExist )
	    {
		    SAFE_DELETE( m_pTextureEff );
		    m_pTextureEff = new DxTextureEffMan;
		    m_pTextureEff->LoadPSF_PREV_PIECE( pd3dDevice, SFile );
	    }
    }

	// 더미
	SFile >> bExist;

	SFile.CloseFile();

	return S_OK;
}

BOOL DxPieceEdit::IsLightMapPiece()
{
    if ( !m_pStaticAniFrame )
        return FALSE;

    return m_pStaticAniFrame->IsLightMapPiece();
}

void DxPieceEdit::SaveLightMapPiece( sc::SerialFile& SFile )
{
    SFile << static_cast<DWORD>( VERSION_LM );

    SFile.BeginBlock();
    {
        if ( m_pStaticAniFrame )
        {
            SFile << static_cast<BOOL>( TRUE );
            m_pStaticAniFrame->SaveLightMapPiece( SFile );
        }
        else
        {
            SFile << static_cast<BOOL>( FALSE );
        }
    }
    SFile.EndBlock();
}

void DxPieceEdit::LoadLightMapPiece( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile )
{
    DWORD dwVer, dwBufferSize;
    SFile >> dwVer;
    SFile >> dwBufferSize;

    BOOL bExist;
    SFile >> bExist;
    if ( bExist )
    {
        DxStaticAniFrame* pStaticAniFrame = new DxStaticAniFrame;
        pStaticAniFrame->LoadLightMapPiece( pd3dDevice, SFile );
        m_pStaticAniFrame->FlipLightMapRenderMan( pStaticAniFrame );
        SAFE_DELETE( pStaticAniFrame );
    }
}

void DxPieceEdit::Export( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bRan_1_0 )
{
	if( !m_pFrameMesh )	
		return;

	//if ( bRan_1_0 )
	//{
	//	if( m_pStaticAniFrame )
	//	{
	//		if ( m_dwFlags & EMPO_UNSUPPORT_RAD )
	//		{
	//			m_pStaticAniFrame->Export( pd3dDevice, m_pFrameMesh, NULL, m_pFrameMesh->GetTextureEffMan_const(), TRUE );
	//		}
	//		else
	//		{
	//			// Note : 초기화 한다.
	//			SP_LIGHTMAPUV_MAIN spLightMapUV_Main = SP_LIGHTMAPUV_MAIN(new LIGHTMAPUV_MAIN);
	//			if ( m_pFrameMesh->GetFrameRoot() )
	//			{
	//				// LightMap에 필요한 정보를 가지고 온다. FrameName / UV_Data
	//				MAP_LIGHTMAPUV mapLightMapUV;
	//				m_pFrameMesh->ExportLightMapUV( mapLightMapUV, m_pFrameMesh->GetFrameRoot() );

	//				// UV 값을 정리한다. UV를 0 ~ 1 사이의 값으로 만든다.
	//				NSLightMap::RemakeLightMapUserUV_PIECE( mapLightMapUV, spLightMapUV_Main );
	//			}
	//			m_pStaticAniFrame->Export( pd3dDevice, m_pFrameMesh, spLightMapUV_Main.get(), m_pFrameMesh->GetTextureEffMan_const(), TRUE );
	//		}
	//	}
	//	if( m_pPieceEffEDIT )
	//	{
	//		m_pPieceEff->Export( pd3dDevice, m_pPieceEffEDIT, m_pFrameMesh, m_pStaticAniFrame->GetAniManHead() );
	//	}
	//	if( m_pTextureEff )
	//	{
	//		m_pTextureEff->Clone( pd3dDevice, m_pFrameMesh->GetTextureEffMan_const() );
	//	}
	//}
	//else
	{
		if( m_pStaticAniFrame )
		{
			// Note : 초기화 한다.
			SP_LIGHTMAPUV_MAIN spLightMapUV_Main = SP_LIGHTMAPUV_MAIN(new LIGHTMAPUV_MAIN);
			if ( m_pFrameMesh->GetFrameRoot() )
			{
				// LightMap에 필요한 정보를 가지고 온다. FrameName / UV_Data
				MAP_LIGHTMAPUV mapLightMapUV;
				m_pFrameMesh->ExportLightMapUV( mapLightMapUV, m_pFrameMesh->GetFrameRoot() );

				// UV 값을 정리한다. UV를 0 ~ 1 사이의 값으로 만든다.
				NSLightMap::RemakeLightMapUserUV_PIECE( mapLightMapUV, spLightMapUV_Main );
			}
			m_pStaticAniFrame->Export( pd3dDevice, m_pFrameMesh, spLightMapUV_Main.get(), NULL, m_pFrameMesh->GetMaterialSetting_const(), FALSE, g_fCELL_SIZE_PIECE );
		}
		if( m_pPieceEffEDIT )
		{
			m_pPieceEff->Export( pd3dDevice, m_pPieceEffEDIT, m_pFrameMesh, m_pStaticAniFrame->GetAniManHead() );
		}
	}
}

BOOL DxPieceEdit::ChangeDxStaticMaterialMesh( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !m_pStaticAniFrame )
		return FALSE;

	if ( m_pStaticAniFrame->ChangeDxStaticMaterialMesh( pd3dDevice, g_fCELL_SIZE_PIECE ) )
	{
		m_dwFlags &= ~EMPO_UNSUPPORT_RAD;
		return TRUE;
	}

	return FALSE;
}