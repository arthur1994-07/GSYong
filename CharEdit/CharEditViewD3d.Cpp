// CharEditView.cpp : implementation of the CCharEditView class
//

#include "stdafx.h"
#include "CharEdit.h"

#include "../VisualMaterialLib/Manager/DxVMManager.h"

#include "../EngineLib/DxEffect/DxLoadShader.h"
#include "../EngineLib/DxEffect/NSEffectThread.h"
#include "../EngineLib/DxEffect/Frame/DxEffectMan.h"
#include "../EngineLib/DxMeshs/FrameMesh/DxReplaceContainer.h"
#include "../EngineLib/DxMeshs/FrameMesh/DxFrameMesh.h"
#include "../EngineLib/DxMeshs/SkinMesh/_new/NsSMeshSceneGraph.h"
#include "../EngineLib/DxMeshs/SkinMesh/NSSkinAniThread.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxVehicleThreadNeedData.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinPieceContainer.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkeletonMaintain.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinAniControl.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinAnimation.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxClothColl.h"
#include "../EngineLib/DxTools/Light/NSShadowLight.h"
#include "../EngineLib/DxTools/TextureManager.h"
#include "../EngineLib/DxTools/DxRenderStates.h"
#include "../EngineLib/DxTools/DxInputDevice.h"
#include "../EngineLib/DxTools/DxEnvironment.h"
#include "../EngineLib/DxTools/GammaControl.h"
#include "../EngineLib/DxTools/DxSurfaceTex.h"
#include "../EngineLib/DxTools/DxShadowMap.h"
#include "../EngineLib/DxTools/DxDynamicVB.h"
#include "../EngineLib/DxTools/DxViewPort.h"
#include "../EngineLib/DxTools/PostProcessing/NSBloomHDR.h"
#include "../EngineLib/DxTools/Material/NSMaterialManager.h"
#include "../EngineLib/DxTools/Deffered/NSDeffered.h"
#include "../EngineLib/DxTools/CubeMap/NSCubeMapCulling.h"
#include "../EngineLib/DxTools/CubeMap/NSCubeMapUtil.h"
#include "../EngineLib/DxTools/Light/DxLightMan.h"
#include "../EngineLib/DxTools/DxFontMan.h"
#include "../EngineLib/DxTools/EditMeshs.h"
#include "../EngineLib/DxTools/DxCubeMap.h"
#include "../EngineLib/DxTools/DxGlowMan.h"
#include "../EngineLib/DxTools/RENDERPARAM.h"
#include "../EngineLib/DxTools/DebugSet.h"
#include "../EngineLib/DxLand/DxLandMan.h"
#include "../EngineLib/Water/NSWaterMain.h"
#include "../EngineLib/Common/SubPath.h"
#include "../EngineLib/PhysX/NSPhysX.h"
#include "../EngineLib/DxResponseMan.h"
#include "../RanLogic/RANPARAM.h"

#include "./Properties/sheetWithTabChar.h"
#include "./Properties/sheetWithTabAnim.h"
#include "./Properties/MultianimPage.h"

#include "MainFrm.h"
#include "CharEditDoc.h"
#include "CharEditView.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

ANIMATE			g_Animate;
DXRENDSTATE		g_RendState;
bool			g_bMoveTest = FALSE;
MOVETYPE		g_MoveType  = MOVE_STAY;

bool			g_bViewPassenger = FALSE;

extern int				g_nITEMLEVEL;
extern int				g_nITEMLEVEL_MIN;
extern int				g_nITEMLEVEL_MAX;
extern BOOL				g_bOBJECT100;
extern D3DXVECTOR3		g_vCLOSED_POINT;
extern D3DXVECTOR3		g_vCLOSED_NORMAL;
extern std::string		g_strPICK_BONE;
extern BOOL				g_bAABBBOX_VISIBLE;
extern BOOL             g_bAmbientTest_CHAREDIT;
extern DWORD            g_dwAmbient_CHAREDIT;
extern BOOL             g_bCHAR_CHF_RENDER;
//extern BOOL				g_bWEAPON_SLOT_VISIBLE;

BOOL					g_bClickSpinRadius = FALSE;
BOOL					g_bClickSpinHeight = FALSE;
BOOL					g_bClickSpin3 = FALSE;
BOOL					g_bClickSpin4 = FALSE;

int						g_iClickSpinColor = 0; // by luxes.

EDITOBJTYPE		CCharEditView::m_emObjType = EOBJ_SKINCHAR;
//DxSkinObject	CCharEditView::m_SkinObject;
//DxSkinChar		CCharEditView::m_SkinChar;
//DxSkinCharData	CCharEditView::m_SkinCharData;
//DxSkinPiece		CCharEditView::m_SkinPiece;

BOOL			CCharEditView::m_bSkinTraceVertDraw = FALSE;
CString			CCharEditView::m_strSelectedTracVert = "";

SVERTEXINFLU	CCharEditView::m_VertexInflu;
D3DXVECTOR3		CCharEditView::m_vCollision = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);
RENDERTYPE		CCharEditView::m_emRenderType = RT_DEFAULT;
DWORD			CCharEditView::m_dwRenderPSType(ID_RENDER_PS_3_0);


DxEffKeepDataMain	CCharEditView::g_sEffKeepMain;

HRESULT CCharEditView::ConfirmDevice ( D3DCAPSQ* pCaps, DWORD dwBehavior, D3DFORMAT Format )
{
	if( dwBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING )
		return E_FAIL;

	return S_OK;
}

HRESULT CCharEditView::OneTimeSceneInit()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;

	//	Note : 각종 장치들.
	m_pEngineDevice->OneTimeSceneInit(
        pFrame->m_szAppPath,
        this,
        RANPARAM::strFontType,
        false,
        RENDERPARAM::emLangSet,
        RANPARAM::strGDIFont );

	// Note : Sheet를 초기화 해 놓는다.
	pFrame->m_cDlgEdit.SetAcitveSheetPage( 0 );	//CHARTAB_CHARFILE 0

	return S_OK;
}

HRESULT CCharEditView::InitDeviceObjects()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;

	DxFontMan::GetInstance().InitDeviceObjects ( m_pd3dDevice );
	CD3DFontPar* pD3dFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	//	Note	:	디버그셋 경로 설정 및 초기화
	CDebugSet::InitDeviceObjects( pD3dFont9 );

	//	Note : 각종 장치들.
	m_pEngineDevice->InitDeviceObjects ( m_pd3dDevice );

	D3DXVECTOR3 vFromPt		= D3DXVECTOR3( 0.0f, 80.0f, -80.0f );
	D3DXVECTOR3 vLookatPt	= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUpVec		= D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	DxViewPort::GetInstance().SetViewTrans ( vFromPt, vLookatPt, vUpVec );

	m_vCameraLookAt = D3DXVECTOR3( 0.f, 10.f, 0.f );

	TextureManager::GetInstance().EnableTextureAlphaTypeDebug();

	GammaControl::GetInstance().SetGammaControl( m_hWnd, 1.f, 1, 1.f );

//	D3DXCreateTextureFromFile( m_pd3dDevice, "", m_pBackGroundTex );

	m_pBackGroundTex = NULL;


	m_vSkinBonePickPos = m_vSkinPos = m_vTargetPos = m_vSkinMeshPickPos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	
	m_fSkinRotate = 0.0f;

	m_bRunMove = TRUE;

	m_bSkinCharNonAtkMode = FALSE;

	m_fattTIMER = 0.0f;
	m_nattSTEP  = 0;

	m_spLandMan->InitDeviceObjects( m_pd3dDevice );

	return S_OK;
}

HRESULT CCharEditView::RestoreDeviceObjects()
{
    HRESULT hr = S_OK;

	//	Note : 뷰포트
	DxViewPort::GetInstance().SetProjection ( D3DX_PI/4, (float)m_d3dsdBackBuffer.Width, 
		(float)m_d3dsdBackBuffer.Height, 1.f, 2000.0f );
	
	hr = DxViewPort::GetInstance().SetViewPort ();
	if ( FAILED(hr) )	return hr;

	//	Note : 각종 장치들.
	m_pEngineDevice->RestoreDeviceObjects ();

	m_pd3dDevice->SetRenderState( D3DRS_COLORVERTEX, FALSE );

	m_sObjectMRS.OnResetDevice( m_pd3dDevice );

	D3DXMatrixIdentity ( &m_SkinMat );

	D3DXVECTOR3 vPos(m_SkinMat._41,m_SkinMat._42,m_SkinMat._43);
	m_SkinChar.m_spSkinAniControlThread->SetPosition ( vPos );
	m_SkinChar.RestoreDeviceObjects ( m_pd3dDevice );
	m_AttBone.RestoreDeviceObjects ( m_pd3dDevice );
	m_Vehicle.RestoreDeviceObjects ( m_pd3dDevice );
	m_AttBoneLink.RestoreDeviceObjects( m_pd3dDevice );
	m_SkinPiece.RestoreDeviceObjects ( m_pd3dDevice );

	UINT i;
	for(  i = 0; i < m_SkinPieceList.size(); i++ )
	{
		m_SkinPieceList[i]->RestoreDeviceObjects( m_pd3dDevice );
	}

	for( i = 0; i < MAX_PASSENGER_COUNT; i++ )
	{
		m_SkinPassenger[i].RestoreDeviceObjects( m_pd3dDevice );
	}

	//	Note : 글로우 처리를 하기위해 사용 한다.
	DxGlowMan::GetInstance().SetProjectActiveON();

	NSCubeMapUtil::SetTempCubeMapName( _T("incloudscub1emap.dds") );

	// Note : 애니메이션 Thread
	//			캐릭터 Editer 는 AniThread 를 하지 않는다.
	NSSkinAniThread::EndThread();

	m_spLandMan->RestoreDeviceObjects( m_pd3dDevice );
	
	return S_OK;
}

void CCharEditView::LoadBGTexture( const char *pszFileName )
{
	SAFE_RELEASE( m_pBackGroundTex );
	D3DXCreateTextureFromFile( m_pd3dDevice, pszFileName, &m_pBackGroundTex );
}


bool CCharEditView::PickingPos( D3DXVECTOR3& vPos )
{
	D3DXVECTOR3 vPickRayDir;
	D3DXVECTOR3 vPickRayOrig;


	D3DSURFACE_DESC d3dsdBackBuffer;
	LPDIRECT3DSURFACE9 pBackBuffer = NULL;
	m_pd3dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
	pBackBuffer->GetDesc( &d3dsdBackBuffer );

	POINT ptCursor;
	GetCursorPos( &ptCursor );
	ScreenToClient( &ptCursor );

	RECT rect;
	GetWindowRect( &rect );
	if( ptCursor.x > rect.right )
	{
		if ( pBackBuffer )
		{
			pBackBuffer->Release();
		}

		return FALSE;
	}

	D3DXMATRIX matProj, matView, matWorld;
	m_pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProj );
	m_pd3dDevice->GetTransform( D3DTS_VIEW, &matView );
	m_pd3dDevice->GetTransform( D3DTS_WORLD, &matWorld );

	// Compute the vector of the Pick ray in screen space
	D3DXVECTOR3 v;
	v.x =  ( ( ( 2.0f * ptCursor.x ) / d3dsdBackBuffer.Width  ) - 1 ) / matProj._11;
	v.y = -( ( ( 2.0f * ptCursor.y ) / d3dsdBackBuffer.Height ) - 1 ) / matProj._22;
	v.z =  1.0f;

	D3DXMATRIX mWorldView = matWorld * matView;
	D3DXMATRIX m;
	D3DXMatrixInverse( &m, NULL, &mWorldView );

	// Transform the screen space Pick ray into 3D space
	vPickRayDir.x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
	vPickRayDir.y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
	vPickRayDir.z  = v.x*m._13 + v.y*m._23 + v.z*m._33;
	vPickRayOrig.x = m._41;
	vPickRayOrig.y = m._42;
	vPickRayOrig.z = m._43;

	D3DXVECTOR3 v0, v1, v2;
	D3DXVECTOR3 col = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	float fDist = 0.0f;
	float fBary1, fBary2;
	float fDistanceMin = 1000.0f;
	D3DXVECTOR3 returnPos;

	// 왼쪽 면
	v0 = D3DXVECTOR3( -100.0f, 0.0f,  100.0f );
	v1 = D3DXVECTOR3(  100.0f, 0.0f,  100.0f );
	v2 = D3DXVECTOR3( -100.0f, 0.0f, -100.0f );
	if( DxEffectMan::GetInstance().IntersectTriangle( vPickRayOrig, vPickRayDir, v0, v1, v2, &fDist, &fBary1, &fBary2 ) )
	{
		if( fDistanceMin >= fDist )
		{
			fDistanceMin = fDist;
			col = vPickRayOrig + vPickRayDir * fDist;
			vPos = vPickRayOrig + vPickRayDir * fDist;
		}
	}

	// 오른쪽 면
	v0 = D3DXVECTOR3( -100.0f, 0.0f, -100.0f );
	v1 = D3DXVECTOR3(  100.0f, 0.0f, -100.0f );
	v2 = D3DXVECTOR3(  100.0f, 0.0f,  100.0f );
	if( DxEffectMan::GetInstance().IntersectTriangle( vPickRayOrig, vPickRayDir, v0, v1, v2, &fDist, &fBary1, &fBary2 ) )
	{
		if( fDistanceMin >= fDist )
		{
			fDistanceMin = fDist;
			col = vPickRayOrig + vPickRayDir * fDist;
			vPos = vPickRayOrig + vPickRayDir * fDist;
		}
	}

	if ( pBackBuffer )
	{
		pBackBuffer->Release();
	}

	return TRUE;
}

void CCharEditView::ChangeAnimation()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	if( CCharEditView::m_emObjType == EOBJ_SKINCHAR || CCharEditView::m_emObjType == EOBJ_FLYCAMERA )
	{
		if( g_MoveType == MOVE_STAY )
		{
			pView->GetSkinChar()->SELECTANI( AN_GUARD_N );
			//pView->GetSkinCharData()->SELECTANI( AN_GUARD_N );
		}else if( g_MoveType == MOVE_WALK )
		{
			pView->GetSkinChar()->SELECTANI( AN_WALK );
			//pView->GetSkinCharData()->SELECTANI( AN_WALK );
		}else if( g_MoveType == MOVE_RUN )
		{
			pView->GetSkinChar()->SELECTANI( AN_RUN );
			//pView->GetSkinCharData()->SELECTANI( AN_RUN );
		}
	}
	else if( CCharEditView::m_emObjType == EOBJ_ATTBONE )
	{
		pView->GetAttBone()->SELECTANI( AN_ATTBONE_ATTACK_001 );
		//pView->GetAttBoneData()->SELECTANI( AN_ATTBONE_ATTACK_001 );
	}
	else if( CCharEditView::m_emObjType == EOBJ_VEHICLE )
	{
		if( g_MoveType == MOVE_STAY )
		{
			pView->GetVehicle()->SELECTANI( AN_VEHICLE_STAY );
			//pView->GetVehicleData()->SELECTANI( AN_VEHICLE_STAY );
		}else if( g_MoveType == MOVE_WALK )
		{
			pView->GetVehicle()->SELECTANI( AN_VEHICLE_WALK );
			//pView->GetVehicleData()->SELECTANI( AN_VEHICLE_WALK );
		}else if( g_MoveType == MOVE_RUN )
		{
			pView->GetVehicle()->SELECTANI( AN_VEHICLE_RUN );
			//pView->GetVehicleData()->SELECTANI( AN_VEHICLE_RUN );
		}
	}

	const DxSkinAnimation* pSelectedAniCont = m_SkinChar.m_spSkinAniControlThread->GETCURANIM ();
	if( pSelectedAniCont != NULL )
	{
		DWORD dwUnitTime = pSelectedAniCont->m_UNITTIME;
		if ( dwUnitTime > 0 )
		{
			pFrame->SetTimeLine ( pSelectedAniCont->m_dwSTime/dwUnitTime,
				pSelectedAniCont->m_dwETimeOrig/dwUnitTime, dwUnitTime, AM_PLAY );
		}
	}
}

void CCharEditView::MoveUpdateSkinChar()
{
	if( g_bMoveTest && DxInputDevice::GetInstance().GetMouseState(DXMOUSE_LEFT)&DXKEY_UP ) 
	{
		PickingPos(m_vTargetPos);
		if( m_bRunMove ) g_MoveType = MOVE_RUN;
		else g_MoveType = MOVE_WALK;

		ChangeAnimation();
	}

	if( DxInputDevice::GetInstance().GetKeyState(DIK_R)&DXKEY_UP )
	{
		if( g_MoveType == MOVE_WALK ) g_MoveType = MOVE_RUN;
		else if( g_MoveType == MOVE_RUN ) g_MoveType = MOVE_WALK;
		m_bRunMove = !m_bRunMove;

		ChangeAnimation();
	}

	D3DXVECTOR3 vDir = m_vTargetPos - m_vSkinPos;
	float fDist = D3DXVec3Length( &vDir );

	if( fDist > 0.5f )
	{
		CMainFrame* pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		float fMoveSpeed;

		if( g_MoveType == MOVE_WALK ) fMoveSpeed = pFrame->GetWalkSpeed() * m_fElapsedTime;
		else fMoveSpeed = pFrame->GetRunSpeed() * m_fElapsedTime;

		D3DXVec3Normalize( &vDir, &vDir );
		m_vSkinPos += (vDir*fMoveSpeed);

		D3DXVECTOR3 vDirOrig = D3DXVECTOR3(0,0,-1);
		m_fSkinRotate = DXGetThetaYFromDirection( vDir, vDirOrig );

	}
	else
	{
		m_vSkinPos = m_vTargetPos;
		g_MoveType = MOVE_STAY;

		ChangeAnimation();
	}

	D3DXMATRIX matTrans, matRotate;
	D3DXMatrixTranslation( &matTrans, m_vSkinPos.x, m_vSkinPos.y, m_vSkinPos.z );
	D3DXMatrixRotationY( &matRotate, m_fSkinRotate );
	m_SkinMat = matRotate * matTrans;
}

BOOL CCharEditView::AttackProc ( float fElapsedTime )
{
	m_fattTIMER += fElapsedTime;
	if( !m_SkinChar.m_spSkinAniControlThread->GETCURANIMNODE() ) return FALSE;

	PANIMCONTNODE pAnicontNode = m_SkinChar.m_spSkinAniControlThread->GETCURANIMNODE();
	DxSkinAnimation* pAnimCont = pAnicontNode->m_pDxSkinAni;

	if( !pAnimCont ) 
		return FALSE;

	if ( pAnimCont->m_wStrikeCount == 0 )	return FALSE;

	DWORD dwThisKey = DWORD(m_fattTIMER*UNITANIKEY_PERSEC);

	DWORD dwStrikeKey = pAnimCont->m_sStrikeEff[m_nattSTEP].m_dwFrame;
	if ( m_nattSTEP < pAnimCont->m_wStrikeCount )
	{
		m_SkinChar.UpdateStrikeAttachBone( dwThisKey, m_nattSTEP );

		if ( dwThisKey>=dwStrikeKey )
		{		
			m_nattSTEP++;
		}
	}

	if( m_SkinChar.m_spSkinAniControlThread->ISENDANIM() )
	{
		m_nattSTEP  = 0;
		m_fattTIMER = 0.0f;

		m_SkinChar.ResetStrikeAttachBone();
	}

	CDebugSet::ToView( 15, "attTimer : %f %d %d %d", m_fattTIMER, dwStrikeKey, dwThisKey, m_nattSTEP );

	return TRUE;
}

BOOL CCharEditView::AttackVehicleProc ( float fElapsedTime )
{
	m_fattTIMER += fElapsedTime;
	if( !m_Vehicle.m_spSkinAniControlThread->GETCURANIMNODE() ) return FALSE;

	PANIMCONTNODE pAnicontNode = m_Vehicle.m_spSkinAniControlThread->GETCURANIMNODE();
	DxSkinAnimation* pAnimCont = pAnicontNode->m_pDxSkinAni;

	if( !pAnimCont ) 
		return FALSE;

	if ( pAnimCont->m_wStrikeCount == 0 )	return FALSE;

	DWORD dwThisKey = DWORD(m_fattTIMER*UNITANIKEY_PERSEC);

	DWORD dwStrikeKey = pAnimCont->m_sStrikeEff[m_nattSTEP].m_dwFrame;
	if ( m_nattSTEP < pAnimCont->m_wStrikeCount )
	{
		m_Vehicle.UpdateStrikeAttachBone( dwThisKey, m_nattSTEP );

		if ( dwThisKey>=dwStrikeKey )
		{		
			m_nattSTEP++;
		}
	}

	if( m_Vehicle.ISENDANIM() )
	{
		m_nattSTEP  = 0;
		m_fattTIMER = 0.0f;

		m_Vehicle.ResetStrikeAttachBone();
	}

	CDebugSet::ToView( 15, "attTimer : %f %d %d %d", m_fattTIMER, dwStrikeKey, dwThisKey, m_nattSTEP );

	return TRUE;
}

HRESULT CCharEditView::FrameMove( BOOL bNotRendering )
{
	//// Note : fps의 제한
	//if( DxInputDevice::GetInstance().GetKeyState(DIK_F4)&DXKEY_UP )
	//{
	//	g_bFRAME_LIMIT = (g_bFRAME_LIMIT) ? FALSE : TRUE;
	//}

	DxViewPort::GetInstance().SetToolCamera();

	// Note : 
	m_sObjectMRS.SetWVL( 0 );
	//if( (m_CurType!=_CUR_CHAR_MOVE) && (m_CurType!=_CUR_SELECT_POINT) )
	//{
	//	if( m_sObjectMRS.IsCollMode() )	m_CurType = _CUR_SELECT;
	//}

	// Note : 스핀 컨트롤을 계속 검사한다.
	int nRadius = m_sSpinRadius.FrameMove( g_bClickSpinRadius, m_bDefWin );
	int nHeight = m_sSpinHeight.FrameMove( g_bClickSpinHeight, m_bDefWin );
	int nSpin3 = m_sSpin3.FrameMove( g_bClickSpin3, m_bDefWin );
	int nSpin4 = m_sSpin4.FrameMove( g_bClickSpin4, m_bDefWin );
	int nColorHeight = m_sSpinColor.FrameMoveColor( g_iClickSpinColor, m_bDefWin );

	// Note : Edit에 넣는다.
	CMainFrame* pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	pFrame->m_cDlgEdit.SetSpinControl( nRadius, nHeight, nSpin3, nSpin4, nColorHeight );

	CCharEditApp *pApp = (CCharEditApp*) AfxGetApp();
	if ( !pApp || !pApp->IsActive() )
		return FALSE;

	// Note : 오브젝트의 갯수
	if( DxInputDevice::GetInstance().GetKeyState(DIK_F3)&DXKEY_UP )
	{
		g_bOBJECT100 = (g_bOBJECT100) ? FALSE : TRUE;
	}

	//	Note : 아이템 레벨 용
	if ( DxInputDevice::GetInstance().GetKeyState(DIK_F5)&DXKEY_UP )	--g_nITEMLEVEL;
	if ( DxInputDevice::GetInstance().GetKeyState(DIK_F6)&DXKEY_UP )	++g_nITEMLEVEL;
	if ( DxInputDevice::GetInstance().GetKeyState(DIK_F7)&DXKEY_UP )	g_nITEMLEVEL = g_nITEMLEVEL_MIN;
	if ( g_nITEMLEVEL<g_nITEMLEVEL_MIN )	g_nITEMLEVEL = g_nITEMLEVEL_MIN;
	if ( g_nITEMLEVEL>g_nITEMLEVEL_MAX )	g_nITEMLEVEL = g_nITEMLEVEL_MAX;

	// CharEdit 에서는 지형이 없어서 실시간CubeMap 을 쓸수 없기 때문에, 셋팅된 CubeMap 을 쓰도록 한다.
	RENDERPARAM::g_emGroundQulity = TnL_PS_1_1;

	//CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	switch ( g_nITEMLEVEL )
	{
	case 0:		pFrame->OnItemlevelDefault();	break;
	case 1:		pFrame->OnItemlevelLevel1();	break;
	case 2:		pFrame->OnItemlevelLevel2();	break;
	case 3:		pFrame->OnItemlevelLevel3();	break;
	case 4:		pFrame->OnItemlevelLevel4();	break;
	case 5:		pFrame->OnItemlevelLevel5();	break;
	case 6:		pFrame->OnItemlevelLevel6();	break;
	case 7:		pFrame->OnItemlevelLevel7();	break;
	case 8:		pFrame->OnItemlevelLevel8();	break;
	case 9:		pFrame->OnItemlevelLevel9();	break;
	case 10:	pFrame->OnItemlevelLevel10();	break;
	case 11:	pFrame->OnItemlevelLevel11();	break;
	};

	CDlgTimeLine *pDlgTimeLine = &pFrame->m_cDlgTimeLine;
	m_fElapsedTime *= pDlgTimeLine->GetAniDelay();
	m_fTime		   *= pDlgTimeLine->GetAniDelay();	

	//	Note : Frame Picking
	//
	DWORD mKeyState = DxInputDevice::GetInstance().GetMouseState ( DXMOUSE_LEFT );

	if ( mKeyState&DXKEY_UP )
	{
		D3DXVECTOR3 vTargetPt, vFromPt;
		vFromPt = DxViewPort::GetInstance().GetFromPt ();
		BOOL bTargetted = DxViewPort::GetInstance().GetMouseTargetPosWnd ( vTargetPt );

		if ( bTargetted )
		{
			switch ( m_emObjType )
			{
			case EOBJ_SKINOBJ:
				m_vCameraLookAt = D3DXVECTOR3( 0.f, 10.f, 0.f );
				break;

			case EOBJ_SKINCHAR:
				m_vCameraLookAt = D3DXVECTOR3( 0.f, 10.f, 0.f );
				break;
#ifndef DEF_SKIN_SYNC
			case EOBJ_SKINPIECE:
				if ( m_SkinPiece.m_pmcMesh )
				{
					DWORD dwIndex;
					D3DXVECTOR3 vNormal;
					if( m_SkinPiece.m_pmcMesh->GetClosedPoint( vFromPt, vTargetPt, m_vCollision, dwIndex, g_vCLOSED_POINT, vNormal, m_SkinPiece.m_skeletonRes.get() ) )
					{
						m_vCameraLookAt = g_vCLOSED_POINT;
					}
					m_SkinPiece.m_pmcMesh->GetVertexInfluences ( dwIndex, &m_VertexInflu );
				}
				break;
#endif
			};

#ifdef DEF_SKIN_SYNC
            DxSkinPiece* pSkinPiece = GetSkinPiece();

            if (pSkinPiece &&  pSkinPiece->m_pmcMesh)
            {
                DWORD dwIndex;
                D3DXVECTOR3 vNormal;
                if( pSkinPiece->m_pmcMesh->GetClosedPoint2( vFromPt, vTargetPt, m_vCollision, dwIndex, g_vCLOSED_POINT, vNormal ) )
                {
                    m_vCameraLookAt = g_vCLOSED_POINT;
                }
                pSkinPiece->m_pmcMesh->GetVertexInfluences ( dwIndex, &m_VertexInflu );
            }
#endif
		}
	}

	//	Note : 각종 장치들.
	m_pEngineDevice->FrameMove( m_fTime, m_fElapsedTime, m_bDefWin );

	//	Note : 뷰포트 업데이트.
	BOOL DxViewPort_FrameMove(FALSE);
	if( m_emRenderType==RT_BONE )
	{
		if( m_SkinPiece.m_skeletonRes.IsValid() )
		{
			DxBoneTrans* pBoneTran = m_SkinPiece.m_skeletonRes.get()->FindBone_t( g_strPICK_BONE.c_str() );
			if( pBoneTran )
			{
				D3DXVECTOR3 vPos(0.f,0.f,0.f);
				vPos.x = pBoneTran->matCombined._41;
				vPos.y = pBoneTran->matCombined._42;
				vPos.z = pBoneTran->matCombined._43;
				DxViewPort::GetInstance().FrameMoveMAX( m_fElapsedTime, vPos );

				DxViewPort_FrameMove = TRUE;
			}
		}
		else
		{
			switch ( m_emObjType )
			{
			case EOBJ_SKINCHAR:
				{
					if ( m_SkinChar.m_spSkinAniControlThread->GetSkeletonForEdit() )
					{
						DxBoneTrans* pBoneTran = m_SkinChar.m_spSkinAniControlThread->GetSkeletonForEdit()->FindBone_t( g_strPICK_BONE.c_str() );
						if( pBoneTran )
						{
							D3DXVECTOR3 vPos(0.f,0.f,0.f);
							vPos.x = pBoneTran->matCombined._41;
							vPos.y = pBoneTran->matCombined._42;
							vPos.z = pBoneTran->matCombined._43;
							DxViewPort::GetInstance().FrameMoveMAX( m_fElapsedTime, vPos );

							DxViewPort_FrameMove = TRUE;
						}
					}
				}
				break;
			};
		}
	}
	if( !DxViewPort_FrameMove )	DxViewPort::GetInstance().FrameMoveMAX( m_fElapsedTime, m_vCameraLookAt );

	if( m_emObjType == EOBJ_ATTLINK )
	{
		if( DxInputDevice::GetInstance().GetKeyState( DIK_F ) & DXKEY_DOWNED )
		{	
			if( !g_strPICK_BONE.empty() && m_pAttBoneLinkSkeleton )
			{	
				DxBoneTrans* pBoneTran = m_pAttBoneLinkSkeleton->FindBone_t( g_strPICK_BONE.c_str() );
				if( pBoneTran )
				{
					D3DXVECTOR3 vFromPt, vLookatPt, vUpVec;
					DxViewPort::GetInstance().GetViewTrans ( vFromPt, vLookatPt, vUpVec );
					vLookatPt = D3DXVECTOR3( pBoneTran->matCombined._41, pBoneTran->matCombined._42, pBoneTran->matCombined._43 );
					DxViewPort::GetInstance().SetViewTrans ( vFromPt, vLookatPt, vUpVec );
				}
			}

		}
	}


    // 디폴트값
    g_bCHAR_CHF_RENDER = FALSE;

	// Note :
	switch ( m_emObjType )
	{
	case EOBJ_SKINOBJ:
		{
			const DxSkinAnimation* pAnimCont = m_SkinObject.GETCURANIM ();

			switch ( g_Animate )
			{
			case AM_STOP:
				break;

			case AM_PLAY:
			case AM_MULTIPLAY:
				m_SkinObject.FrameMove ( m_fElapsedTime );
				break;

			case AM_STARTKEY:
				{
					if ( pAnimCont )
					{
						float fTime = (float)fmod ( static_cast<float>(m_SkinObject.GETCURTIME()), static_cast<float>(pAnimCont->m_dwETime) ) / UNITANIKEY_PERSEC;
						m_SkinObject.SETCURTIME ( 0.0f );
						m_SkinObject.FrameMove ( 0.0f );
					}

					g_Animate = AM_STOP;
				}
				break;

			case AM_ENDKEY:
				{
					if ( pAnimCont )
					{
						m_SkinObject.SETCURTIME ( float(pAnimCont->m_dwETime-pAnimCont->m_UNITTIME) );
						m_SkinObject.FrameMove ( 0.0f );
					}

					g_Animate = AM_STOP;
				}
				break;

			case AM_PREVKEY:
				if ( pAnimCont )
				{
					if ( m_SkinObject.GETCURTIME() < pAnimCont->m_UNITTIME )
						m_SkinObject.SETCURTIME ( 0.0f );
					else
						m_SkinObject.FrameMove ( -float(pAnimCont->m_UNITTIME)/UNITANIKEY_PERSEC );
				}
				
				g_Animate = AM_STOP;
				break;

			case AM_NEXTKEY:
				if ( pAnimCont )
				{
					m_SkinObject.FrameMove ( pAnimCont->m_UNITTIME/UNITANIKEY_PERSEC );
				}
				g_Animate = AM_STOP;
				break;
			};

			//	Note : 타임 라인 조정.
			//
			if ( pAnimCont )
			{
				DWORD dwThisTime = 0;
				DWORD dwUnitTime = pAnimCont->m_UNITTIME;
				if ( dwUnitTime > 0 )
				{
					float fTime = (float)fmod ( static_cast<float>(pAnimCont->m_UNITTIME), static_cast<float>(pAnimCont->m_dwETime) );

					dwThisTime = (DWORD) ( m_SkinObject.GETCURTIME() / dwUnitTime );
				}

				CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
				CDlgTimeLine *pDlgTimeLine = &pFrame->m_cDlgTimeLine;
				if ( pDlgTimeLine->IsTick () )
				{
					dwThisTime = pDlgTimeLine->GetTickTime ();
					pDlgTimeLine->ResetTick ();
					g_Animate = AM_STOP;

					// FrameMove 가 두번 호출되면 문제가 된다.
					//m_SkinObject.FrameMove ( pAnimCont->m_UNITTIME/UNITANIKEY_PERSEC );
				}

				pDlgTimeLine->SetTimeState ( dwThisTime );
			}
		}
		break;

		// 플라이 카메라 페이지에도 캐릭터가 나와야한다;
	case EOBJ_SKINCHAR:
	case EOBJ_FLYCAMERA:
		{
            // SkinChar Render
            g_bCHAR_CHF_RENDER = TRUE;

			const DxSkinAnimation* pAnimCont = m_SkinChar.m_spSkinAniControlThread->GETCURANIM ();

			switch ( g_Animate )
			{
			case AM_STOP:
                m_SkinChar.SetPart_Detail();
				m_SkinChar.FrameMove ( m_fTime, 0.0f, TRUE, m_SkinMat, TRUE, FALSE, !m_bSkinCharNonAtkMode );
				break;

			case AM_PLAY:

				// Loop 가 아닌 Animation 은 모션이 끝나면, 움직이지가 않는 문제가 있어서 수정함.
				if ( m_SkinChar.m_spSkinAniControlThread->ISENDANIM() && pAnimCont )
				{
					m_SkinChar.m_spSkinAniControlThread->SELECTANI_EDIT( pAnimCont->m_strCfgFile.c_str() );
				}

				m_SkinChar.FrameMove ( m_fTime, m_fElapsedTime, TRUE, m_SkinMat, TRUE, FALSE, !m_bSkinCharNonAtkMode );

				break;

			case AM_MULTIPLAY:
				{
//					if ( pAnimCont == NULL )
//						break;
					if ( m_SkinChar.ISENDANIM() )
					{
						CsheetWithTabAnim* pSheetAnim = NULL;
						if ( m_emObjType == EOBJ_SKINCHAR )
							pSheetAnim = pFrame->m_cDlgEdit.GetSheetChar()->m_CharPage.GetAnimationTab();
						else if (m_emObjType == EOBJ_FLYCAMERA )
							pSheetAnim = pFrame->m_cDlgEdit.GetSheetChar()->m_FlyCameraPage.GetAnimationTab();
						else
							break;

						CString strAnim;
						if ( pAnimCont )
						{
							strAnim = pSheetAnim->m_AnimTabPage.GetMultiAnimPage()->GetNextAnimName();
							m_SkinChar.m_spSkinAniControlThread->SELECTANI_EDIT( strAnim.GetString(), FALSE, EMANI_ONLYZEROFRAMESTART|EMANI_IGNORELOOP );
						}
						else
						{
							strAnim = pSheetAnim->m_AnimTabPage.GetMultiAnimPage()->GetAnimName(0);
							if ( strAnim.IsEmpty() == false )
							{
								m_SkinChar.m_spSkinAniControlThread->SELECTANI_EDIT( strAnim.GetString(), FALSE, EMANI_ONLYZEROFRAMESTART|EMANI_IGNORELOOP );
							}
						}

						m_SkinChar.SETCURTIME(0.0f);

						// SetCharData 내부에서 새로만들기 때문에 재 셋팅이 필요함;
						pSheetAnim->m_pSkinAniContrl = this->GetSkinChar()->m_spSkinAniControlThread.get();

						// 타임라인 조정;
						{
							this->SetAttTIMER(0.0f);
							this->SetAttSTEP(0);

							ANIMCONTNODE* pContNode = m_SkinChar.m_spSkinAniControlThread->findanicontnode_none_const(strAnim.GetString());
							if ( pContNode )
							{
								const DxSkinAnimation* pSelectedAniCont = pContNode->m_pDxSkinAni;
								// SELECTANI시 FrameMove 단계에서 애니메이션이 변경된다; 
								// 현재 진행중인 애니메이션 참조하지않고; 데이터를 찾는다;
								// const DxSkinAnimation* pSelectedAniCont = pSheetAnim->GetSkinAniContrl()->GETCURANIM ();
								if ( pSelectedAniCont )
								{
									DWORD dwUnitTime = pSelectedAniCont->m_UNITTIME;
									if ( dwUnitTime > 0 )
									{
										pFrame->SetTimeLine ( pSelectedAniCont->m_dwSTime/dwUnitTime,
											pSelectedAniCont->m_dwETimeOrig/dwUnitTime, dwUnitTime );
									}
								}

								pFrame->m_cDlgTimeLine.SetMultiAnimPlay();	
							}
						}
					}
					
					m_SkinChar.FrameMove ( m_fTime, m_fElapsedTime, TRUE, m_SkinMat, TRUE, FALSE, !m_bSkinCharNonAtkMode );
				}
				break;

			case AM_STARTKEY:
				{
					//if ( pAnimCont )
					{
						//float fTime = (float)fmod ( static_cast<float>(m_SkinChar.GETCURTIME()), static_cast<float>(pAnimCont->m_dwETime) ) / UNITANIKEY_PERSEC;
						m_SkinChar.SETCURTIME ( 0.0f );
						m_SkinChar.FrameMove ( m_fTime, 0.0f, TRUE, m_SkinMat, TRUE, FALSE, !m_bSkinCharNonAtkMode );
					}

					g_Animate = AM_STOP;
				}
				break;

			case AM_ENDKEY:
				{
					if ( pAnimCont )
					{
						m_SkinChar.SETCURTIME ( float(pAnimCont->m_dwETime - pAnimCont->m_UNITTIME) );
						m_SkinChar.FrameMove ( m_fTime, 0.0f, TRUE, m_SkinMat, TRUE, FALSE, !m_bSkinCharNonAtkMode );
					}

					g_Animate = AM_STOP;
				}
				break;

			case AM_PREVKEY:
				if ( pAnimCont )
				{
					if ( m_SkinChar.GETCURTIME() < pAnimCont->m_UNITTIME )
						m_SkinChar.SETCURTIME ( 0.0f );
					else
						m_SkinChar.FrameMove ( m_fTime, -float(pAnimCont->m_UNITTIME)/UNITANIKEY_PERSEC, TRUE, m_SkinMat, TRUE, FALSE, !m_bSkinCharNonAtkMode );
				}
				
				g_Animate = AM_STOP;
				break;

			case AM_NEXTKEY:
				if ( pAnimCont )
				{
					m_SkinChar.FrameMove ( m_fTime, pAnimCont->m_UNITTIME/UNITANIKEY_PERSEC, TRUE, m_SkinMat, TRUE, FALSE, !m_bSkinCharNonAtkMode );
				}
				g_Animate = AM_STOP;
				break;

			default:
				m_SkinChar.FrameMove ( m_fTime, 0.0f, TRUE, m_SkinMat, TRUE, FALSE, !m_bSkinCharNonAtkMode );
				break;
			};

			//	Note : 타임 라인 조정.
			//
			if ( pAnimCont )
			{
				DWORD dwThisTime = 0;
				DWORD dwUnitTime = pAnimCont->m_UNITTIME;
				if ( dwUnitTime > 0 )
				{
					float fTime = (float)fmod ( static_cast<float>(pAnimCont->m_UNITTIME), static_cast<float>(pAnimCont->m_dwETime) );

					dwThisTime = (DWORD) ( m_SkinChar.GETCURTIME() / dwUnitTime );
				}

				CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
				CDlgTimeLine *pDlgTimeLine = &pFrame->m_cDlgTimeLine;
				if ( pDlgTimeLine->IsTick () )
				{
					dwThisTime = pDlgTimeLine->GetTickTime ();
					pDlgTimeLine->ResetTick ();
					g_Animate = AM_STOP;
					m_SkinChar.m_spSkinAniControlThread->SETCURTIME_ALL_FOR_EDIT( (float) pAnimCont->m_UNITTIME * dwThisTime );
					m_SkinChar.SET_ATTBONE_CURTIME_ALL_FOR_EDIT( (float) pAnimCont->m_UNITTIME * dwThisTime );

					// FrameMove 가 두번 호출되면 문제가 된다.
					//m_SkinChar.FrameMove ( m_fTime, 0.0f, TRUE, m_SkinMat, TRUE, FALSE, !m_bSkinCharNonAtkMode );
				}

				pDlgTimeLine->SetTimeState ( dwThisTime );
			}

			AttackProc ( m_fElapsedTime );
		}

		if( g_bMoveTest )
		{
			MoveUpdateSkinChar();
		}
		//else
		//{
		//	D3DXMatrixIdentity( &m_SkinMat );
		//	m_vSkinPos = m_vTargetPos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		//	m_fSkinRotate = 0.0f;
		//}
		if ( m_emObjType == EOBJ_FLYCAMERA )
		{
			// 플라이 카메라 업데이트;
			{
				GLFlyCameraKeyFrameControl& keyframeCtrl = pFrame->m_cDlgEdit.GetSheetChar()->m_FlyCameraPage.GetKeyFrameCtrl();
				keyframeCtrl.Update(0.0f, m_fElapsedTime);

				DxViewPort::GetInstance().SetViewTrans(
					DxViewPort::GetInstance().GetFromPt() + keyframeCtrl.GetReserveMove() 
					, DxViewPort::GetInstance().GetLookatPt() + keyframeCtrl.GetReserveLook()
					, D3DXVECTOR3(0.0f, 1.0f, 0.0f));
			}
		}
		break;
#ifndef DEF_SKIN_SYNC
	case EOBJ_SKINPIECE:
		m_SkinPiece.FrameMove ( m_fTime, m_fElapsedTime );

		if( m_emRenderType==RT_WIREFRAME )
		{
			if( DxInputDevice::GetInstance().GetMouseState(DXMOUSE_LEFT)&DXKEY_UP  )
			{
				D3DXVECTOR3 vTargetPt, vFromPt, vNormal, vSelectPos;
				vFromPt = DxViewPort::GetInstance().GetFromPt ();
				BOOL bTargetted = DxViewPort::GetInstance().GetMouseTargetPosWnd ( vTargetPt );
				DWORD dwIndex;
				D3DXMATRIX vMatrixIdentity;
				D3DXMatrixIdentity(&vMatrixIdentity);

				if( m_SkinPiece.m_pmcMesh->GetClosedPoint2( vFromPt, vTargetPt, m_vCollision, dwIndex, vSelectPos, vNormal, m_SkinPiece.m_skeletonRes.get(), vMatrixIdentity ) )
				{
					m_vSkinMeshPickPos		= g_vCLOSED_POINT = vSelectPos;
					g_vCLOSED_NORMAL		= vNormal;
					m_vCameraLookAt			= vSelectPos;

					bTargetted = DxViewPort::GetInstance().GetMouseTargetPosWnd ( vTargetPt );
					if ( bTargetted )
					{
						m_SkinPiece.m_skeletonRes.get()->IsCollision( vFromPt, vTargetPt, g_strPICK_BONE, 1.0f );
						CDebugSet::ToView( 14, "Select Bone1 : %s", g_strPICK_BONE.c_str() );
					}

					if( !g_strPICK_BONE.empty() )
					{
						DxBoneTrans* pBoneTran = m_SkinPiece.m_skeletonRes.get()->FindBone_t( g_strPICK_BONE.c_str() );
						if( pBoneTran )
						{
							m_vSkinBonePickPos.x = pBoneTran->matCombined._41;
							m_vSkinBonePickPos.y = pBoneTran->matCombined._42;
							m_vSkinBonePickPos.z = pBoneTran->matCombined._43;								
						}
					}
					
				}

			}
		}
		break;
#endif
	case EOBJ_ATTBONE:
		{
			const DxSkinAnimation* pAnimCont = m_AttBone.m_spSkinAniControlThread->GETCURANIM ();

			D3DXMATRIX matIdentity;
			D3DXMatrixIdentity( &matIdentity );

			switch ( g_Animate )
			{
			case AM_STOP:
				m_AttBone.FrameMoveForTool( m_fTime, 0.f, matIdentity );
				break;

			case AM_PLAY:
			case AM_MULTIPLAY:
				m_AttBone.FrameMoveForTool( m_fTime, m_fElapsedTime, matIdentity );
				break;

			case AM_STARTKEY:
				{
					if ( pAnimCont )
					{
						float fTime = (float)fmod ( static_cast<float>(m_AttBone.m_spSkinAniControlThread->GETCURTIME()), static_cast<float>(pAnimCont->m_dwETime) ) / UNITANIKEY_PERSEC;
						m_AttBone.m_spSkinAniControlThread->SETCURTIME ( 0.0f );
						m_AttBone.FrameMoveForTool( m_fTime, 0.0f, matIdentity );
					}

					g_Animate = AM_STOP;
				}
				break;

			case AM_ENDKEY:
				{
					if ( pAnimCont )
					{
						m_AttBone.m_spSkinAniControlThread->SETCURTIME ( float(pAnimCont->m_dwETime - pAnimCont->m_UNITTIME) );
						m_AttBone.FrameMoveForTool( m_fTime, 0.0f, matIdentity );
					}

					g_Animate = AM_STOP;
				}
				break;

			case AM_PREVKEY:
				if ( pAnimCont )
				{
					if ( m_AttBone.m_spSkinAniControlThread->GETCURTIME() < pAnimCont->m_UNITTIME )
					{
						m_AttBone.m_spSkinAniControlThread->SETCURTIME ( 0.0f );
						m_AttBone.FrameMoveForTool( m_fTime, 0.f, matIdentity );
					}
					else
					{
						m_AttBone.FrameMoveForTool( m_fTime, -float(pAnimCont->m_UNITTIME)/UNITANIKEY_PERSEC, matIdentity );
					}
				}

				g_Animate = AM_STOP;
				break;

			case AM_NEXTKEY:
				if ( pAnimCont )
				{
					m_AttBone.FrameMoveForTool( m_fTime, pAnimCont->m_UNITTIME/UNITANIKEY_PERSEC, matIdentity );
				}
				g_Animate = AM_STOP;
				break;

			default:
				m_AttBone.FrameMoveForTool( m_fTime, 0.f, matIdentity );
				break;
			};


			//	Note : 타임 라인 조정.
			//
			if ( pAnimCont )
			{
				DWORD dwThisTime = 0;
				DWORD dwUnitTime = pAnimCont->m_UNITTIME;
				if ( dwUnitTime > 0 )
				{
					float fTime = (float)fmod ( static_cast<float>(pAnimCont->m_UNITTIME), static_cast<float>(pAnimCont->m_dwETime) );

					dwThisTime = (DWORD) ( m_AttBone.m_spSkinAniControlThread->GETCURTIME() / dwUnitTime );
				}

				CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
				CDlgTimeLine *pDlgTimeLine = &pFrame->m_cDlgTimeLine;
				if ( pDlgTimeLine->IsTick () )
				{
					dwThisTime = pDlgTimeLine->GetTickTime ();
					pDlgTimeLine->ResetTick ();
					g_Animate = AM_STOP;
					m_AttBone.SETCURTIME_ALL_FOR_EDIT( (float) pAnimCont->m_UNITTIME * dwThisTime );

					// FrameMove 가 두번 호출되면 문제가 된다.
					//m_AttBone.FrameMoveForTool( m_fTime, 0.0f, matIdentity );
				}

				pDlgTimeLine->SetTimeState ( dwThisTime );
			}
		}

		if( g_bMoveTest )
		{
			MoveUpdateSkinChar();
		}
		//else
		//{
		//	D3DXMatrixIdentity( &m_SkinMat );
		//	m_vSkinPos = m_vTargetPos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		//	m_fSkinRotate = 0.0f;
		//}
		break;
	case EOBJ_ATTLINK:
		{
			m_SkinPiece.FrameMove ( m_fTime, m_fElapsedTime );

			if( m_pAttBoneLinkSkeleton )
			{
				DxSkeletonMaintain sSkeletonMaintain;
				sSkeletonMaintain.Import_PureThread( m_pAttBoneLinkSkeleton );
				m_AttBoneLink.FrameMoveForTool( m_fTime, m_fElapsedTime, &sSkeletonMaintain );

				DxBoneTrans* pBoneTran = m_pAttBoneLinkSkeleton->FindBone_t( g_strPICK_BONE.c_str() );
				if( pBoneTran )
				{
					D3DXVECTOR3 vPos(0.f,0.f,0.f);
					vPos.x = pBoneTran->matCombined._41;
					vPos.y = pBoneTran->matCombined._42;
					vPos.z = pBoneTran->matCombined._43;

					m_vSkinBonePickPos = vPos;


		//				DxViewPort::GetInstance().FrameMoveMAX( m_fElapsedTime, vPos );

					DxViewPort_FrameMove = TRUE;
				}
			}
		}
		break;
	case EOBJ_VEHICLE:
		{
			const DxSkinAnimation* pAnimCont = m_Vehicle.m_spSkinAniControlThread->GETCURANIM ();

			//m_Vehicle.UpdatePassengerPos();

			switch ( g_Animate )
			{
			case AM_STOP:
				m_Vehicle.FrameMove ( m_fTime, 0.0f, TRUE, m_SkinMat );

				if( g_bViewPassenger )
				{
					for( int i = 0; i < MAX_PASSENGER_COUNT; i++ )
					{
						//const DxSkinAnimation* pAnimContPassenger = m_SkinPassenger[i].m_spSkinAniControlThread->GETCURANIM ();
						m_SkinPassenger[i].FrameMoveCharOnVehicle ( m_fTime, 0.0f, TRUE, FALSE, FALSE, &m_Vehicle, i, TRUE );
					}
				}
				break;

			case AM_PLAY:
			case AM_MULTIPLAY:
				m_Vehicle.FrameMove ( m_fTime, m_fElapsedTime, TRUE, m_SkinMat );

				if( g_bViewPassenger )
				{
					for( int i = 0; i < MAX_PASSENGER_COUNT; i++ )
					{
						//const DxSkinAnimation* pAnimContPassenger = m_SkinPassenger[i].m_spSkinAniControlThread->GETCURANIM ();
						m_SkinPassenger[i].FrameMoveCharOnVehicle ( m_fTime, m_fElapsedTime, TRUE, FALSE, FALSE, &m_Vehicle, i, TRUE );
					}
				}
				break;

			case AM_STARTKEY:
				{
					if ( pAnimCont )
					{
						float fTime = (float)fmod ( static_cast<float>(m_Vehicle.m_spSkinAniControlThread->GETCURTIME()), static_cast<float>(pAnimCont->m_dwETime) ) / UNITANIKEY_PERSEC;
						m_Vehicle.m_spSkinAniControlThread->SETCURTIME ( 0.0f );
						m_Vehicle.FrameMove ( m_fTime, 0.0f, TRUE, m_SkinMat );
					}

					if( g_bViewPassenger )
					{
						for( int i = 0; i < MAX_PASSENGER_COUNT; i++ )
						{
							const DxSkinAnimation* pAnimContPassenger = m_SkinPassenger[i].m_spSkinAniControlThread->GETCURANIM ();
							if ( pAnimContPassenger )
							{
								float fTime = (float)fmod ( static_cast<float>(m_SkinPassenger[i].m_spSkinAniControlThread->GETCURTIME()), static_cast<float>(pAnimContPassenger->m_dwETime) ) / UNITANIKEY_PERSEC;
								m_SkinPassenger[i].m_spSkinAniControlThread->SETCURTIME ( 0.0f );
								m_SkinPassenger[i].FrameMoveCharOnVehicle ( m_fTime, 0.f, TRUE, FALSE, FALSE, &m_Vehicle, i, TRUE );
							}
						}
					}

					g_Animate = AM_STOP;
				}
				break;

			case AM_ENDKEY:
				{
					if ( pAnimCont )
					{
						m_Vehicle.m_spSkinAniControlThread->SETCURTIME ( float(pAnimCont->m_dwETime-pAnimCont->m_UNITTIME) );
						m_Vehicle.FrameMove ( m_fTime, 0.0f, TRUE, m_SkinMat );
					}

					if( g_bViewPassenger )
					{
						for( int i = 0; i < MAX_PASSENGER_COUNT; i++ )
						{
							const DxSkinAnimation* pAnimContPassenger = m_SkinPassenger[i].m_spSkinAniControlThread->GETCURANIM ();
							if ( pAnimContPassenger )
							{
								m_SkinPassenger[i].m_spSkinAniControlThread->SETCURTIME ( float(pAnimCont->m_dwETime-pAnimCont->m_UNITTIME) );
								m_SkinPassenger[i].FrameMoveCharOnVehicle ( m_fTime, 0.f, TRUE, FALSE, FALSE, &m_Vehicle, i, TRUE );
							}
						}
					}


					g_Animate = AM_STOP;
				}
				break;

			case AM_PREVKEY:
				if ( pAnimCont )
				{
					if ( m_Vehicle.m_spSkinAniControlThread->GETCURTIME() < pAnimCont->m_UNITTIME )
					{
						m_Vehicle.m_spSkinAniControlThread->SETCURTIME ( 0.0f );
						m_Vehicle.FrameMove ( m_fTime, 0.0f, TRUE, m_SkinMat );
					}
					else
					{
						m_Vehicle.FrameMove ( m_fTime, -float(pAnimCont->m_UNITTIME)/UNITANIKEY_PERSEC, TRUE, m_SkinMat );
					}
				}

				if( g_bViewPassenger )
				{
					for( int i = 0; i < MAX_PASSENGER_COUNT; i++ )
					{
						const DxSkinAnimation* pAnimContPassenger = m_SkinPassenger[i].m_spSkinAniControlThread->GETCURANIM ();
						if ( pAnimContPassenger )
						{
							if ( m_SkinPassenger[i].m_spSkinAniControlThread->GETCURTIME() < pAnimCont->m_UNITTIME )
							{
								m_SkinPassenger[i].m_spSkinAniControlThread->SETCURTIME ( 0.0f );
								m_SkinPassenger[i].FrameMoveCharOnVehicle ( m_fTime, 0.f, TRUE, FALSE, FALSE, &m_Vehicle, i, TRUE );
							}
							else
							{
								m_SkinPassenger[i].FrameMoveCharOnVehicle ( m_fTime, -float(pAnimCont->m_UNITTIME)/UNITANIKEY_PERSEC, TRUE, FALSE, FALSE, &m_Vehicle, i, TRUE );
							}
						}
					}
				}

				g_Animate = AM_STOP;
				break;

			case AM_NEXTKEY:
				if ( pAnimCont )
				{
					m_Vehicle.FrameMove ( m_fTime, pAnimCont->m_UNITTIME/UNITANIKEY_PERSEC, TRUE, m_SkinMat );
				}

				if( g_bViewPassenger )
				{
					for( int i = 0; i < MAX_PASSENGER_COUNT; i++ )
					{
						const DxSkinAnimation* pAnimContPassenger = m_SkinPassenger[i].m_spSkinAniControlThread->GETCURANIM ();
						if ( pAnimContPassenger )
						{
							m_SkinPassenger[i].FrameMoveCharOnVehicle ( m_fTime, pAnimCont->m_UNITTIME/UNITANIKEY_PERSEC, TRUE, FALSE, FALSE, &m_Vehicle, i, TRUE );
						}
					}
				}

				g_Animate = AM_STOP;
				break;
			};

			//	Note : 타임 라인 조정.
			//
			if ( pAnimCont )
			{
				DWORD dwThisTime = 0;
				DWORD dwUnitTime = pAnimCont->m_UNITTIME;
				if ( dwUnitTime > 0 )
				{
					float fTime = (float)fmod ( static_cast<float>(pAnimCont->m_UNITTIME), static_cast<float>(pAnimCont->m_dwETime) );

					dwThisTime = (DWORD) ( m_Vehicle.m_spSkinAniControlThread->GETCURTIME() / dwUnitTime );
				}

				CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
				CDlgTimeLine *pDlgTimeLine = &pFrame->m_cDlgTimeLine;
				if ( pDlgTimeLine->IsTick () )
				{
					dwThisTime = pDlgTimeLine->GetTickTime ();
					pDlgTimeLine->ResetTick ();
					g_Animate = AM_STOP;
					m_Vehicle.m_spSkinAniControlThread->SETCURTIME_ALL_FOR_EDIT( (float) pAnimCont->m_UNITTIME * dwThisTime );
					m_Vehicle.SET_ATTBONE_CURTIME_ALL_FOR_EDIT( (float) pAnimCont->m_UNITTIME * dwThisTime );

					// FrameMove 가 두번 호출되면 문제가 된다.
					//m_Vehicle.FrameMove ( m_fTime, 0.0f, TRUE, m_SkinMat );
				}

				pDlgTimeLine->SetTimeState ( dwThisTime );
			}

			AttackVehicleProc ( m_fElapsedTime );
		}

		if( g_bMoveTest )
		{
			MoveUpdateSkinChar();
		}

		//if( g_bViewPassenger )
		//{
		//	for( int i = 0; i < MAX_PASSENGER_COUNT; i++ )
		//	{
		//		m_SkinPassenger[i].FrameMoveCharOnVehicle ( m_fTime, m_fElapsedTime, TRUE, FALSE, FALSE, &m_Vehicle, i, TRUE );

		//		//m_SkinPassenger[i].FrameMove( m_fTime, m_fElapsedTime, TRUE, m_SkinMat );
		//	}
		//}
		
		break;
	};

	if( m_emObjType != EOBJ_SKINCHAR && m_emObjType != EOBJ_FLYCAMERA )
	{
		D3DXMatrixIdentity( &m_SkinMat );
	}

	D3DXMATRIX matTrans, matRotate;
	//D3DXMatrixTranslation( &matTrans, m_vSkinPos.x, m_vSkinPos.y + m_SkinCharData.m_fHeightYPos, m_vSkinPos.z );
	D3DXMatrixTranslation( &matTrans, m_vSkinPos.x, m_vSkinPos.y, m_vSkinPos.z );
	D3DXMatrixRotationY( &matRotate, m_fSkinRotate );
	m_SkinMat = matRotate * matTrans;

	D3DXVECTOR3 vPos( m_SkinMat._41, m_SkinMat._42, m_SkinMat._43 );
	m_SkinChar.m_spSkinAniControlThread->SetPosition ( vPos );

	m_spLandMan->FrameMove( m_fTime, m_fElapsedTime, DxViewPort::GetInstance().GetClipVolume(), bNotRendering );

	// Frame Picking
	//
	mKeyState = DxInputDevice::GetInstance().GetMouseState ( DXMOUSE_LEFT );
	if ( mKeyState&DXKEY_UP )
	{
		D3DXVECTOR3 vTargetPt, vFromPt;
		vFromPt = DxViewPort::GetInstance().GetFromPt ();
		BOOL bTargetted = DxViewPort::GetInstance().GetMouseTargetPosWnd ( vTargetPt );

		// Target Point 의 위치를 카메라의 방향으로 멀리 보내엉雹
		//		충돌 검출을 재큱E?할 펯E있다.
		//
		D3DXVECTOR3 vDirect = vFromPt - vTargetPt;
		D3DXVec3Normalize ( &vDirect, &vDirect );
		vTargetPt = vFromPt - vDirect * 100000.0f;

		if ( bTargetted )
		{
			LPDXFRAME pDxFrame = NULL;
			BOOL bCollision = FALSE;
			D3DXVECTOR3 vCollision;
			m_spLandMan->IsCollisionNEW( vFromPt, vTargetPt, vCollision, bCollision, EMCC_CULL_CCW, TRUE );

			if ( bCollision )
			{
				DxViewPort::GetInstance().CameraJump( vCollision );


				D3DXVECTOR3 vDir = vCollision - m_vSkinPos;

				m_vSkinPos = vCollision;

				// 방향 Rotate
				D3DXVECTOR3 vDirOrig = D3DXVECTOR3(0,0,-1);
				m_fSkinRotate = DXGetThetaYFromDirection( vDir, vDirOrig );

				// 위치 변경.
				D3DXMATRIX matTrans, matRotate;
				//D3DXMatrixTranslation( &matTrans, m_vSkinPos.x, m_vSkinPos.y + m_SkinCharData.m_fHeightYPos, m_vSkinPos.z );
				D3DXMatrixTranslation( &matTrans, m_vSkinPos.x, m_vSkinPos.y, m_vSkinPos.z );
				D3DXMatrixRotationY( &matRotate, m_fSkinRotate );
				m_SkinMat = matRotate * matTrans;

				D3DXVECTOR3 vPos( m_SkinMat._41, m_SkinMat._42, m_SkinMat._43 );
				m_SkinChar.m_spSkinAniControlThread->SetPosition ( vPos );
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// NSSkinAniThread
	NSSkinAniThread::CalcluateSkinAniControlForCharEditer( m_fElapsedTime );

	// NSEffectThread 대기
	NSEffectThread::Wait_FrameMove( m_pd3dDevice, m_fElapsedTime );

	return S_OK;
}

void CCharEditView::RenderBackGround()
{
	VERTEXRHW sVertex[4];
	
	sVertex[0].vPos = D3DXVECTOR4( 0.f, 0.f, 1.f, 1.f );
	sVertex[1].vPos = D3DXVECTOR4( (float)m_d3dsdBackBuffer.Width, 0.f, 1.f, 1.f );
	sVertex[2].vPos = D3DXVECTOR4( 0.f,							   (float)m_d3dsdBackBuffer.Height, 1.f, 1.f );
	sVertex[3].vPos = D3DXVECTOR4( (float)m_d3dsdBackBuffer.Width, (float)m_d3dsdBackBuffer.Height, 1.f, 1.f );

	sVertex[0].vPos.w = sVertex[1].vPos.w = sVertex[2].vPos.w = sVertex[3].vPos.w = 1.0f;
	sVertex[0].vPos.z = sVertex[1].vPos.z = sVertex[2].vPos.z = sVertex[3].vPos.z = 1.0f;

	sVertex[0].vTex = D3DXVECTOR2( 0.f, 0.f );
	sVertex[1].vTex = D3DXVECTOR2( 1.f, 0.f );
	sVertex[2].vTex = D3DXVECTOR2( 0.f, 1.f );
	sVertex[3].vTex = D3DXVECTOR2( 1.f, 1.f );

	m_pd3dDevice->SetFVF( VERTEXRHW::FVF );
	m_pd3dDevice->SetTexture( 0, m_pBackGroundTex );
	m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, sVertex, sizeof(VERTEXRHW) );
	m_pd3dDevice->SetTexture( 0, NULL );
}

void CCharEditView::ResetSkinPos()
{
	D3DXMatrixIdentity( &m_SkinMat );
	m_vSkinPos = m_vTargetPos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_fSkinRotate = 0.0f;
}

HRESULT CCharEditView::Render()
{
	// PS Mode 를 셋팅한다.
	RENDERPARAM::g_bEnable_HDR = FALSE;
	if ( m_dwRenderPSType==ID_RENDER_PS_3_0 ) 
	{
		if ( m_spLandMan->GetNameWLD().empty() )
		{
			RENDERPARAM::g_emCharacterQulity = TnL_CHAR_PS_2_0;	// 지형이 없을 땐  SelfShadow 를 그리지 말자.
		}
		else
		{
			RENDERPARAM::g_emCharacterQulity = TnL_CHAR_PS_3_0;
		}
		
		RENDERPARAM::g_bEnable_HDR = TRUE;						// 여기는 HDR 다른곳은 안됨.
	}
	else if ( m_dwRenderPSType==ID_RENDER_PS_2_0 ) 
	{
		RENDERPARAM::g_emCharacterQulity = TnL_CHAR_PS_2_0;
	}
	else if ( m_dwRenderPSType==ID_RENDER_PS_1_1 ) 
	{
		RENDERPARAM::g_emCharacterQulity = TnL_CHAR_PS_1_1;
	}
	else if ( m_dwRenderPSType==ID_RENDER_FIXED_HIGH ) 
	{
		RENDERPARAM::g_emCharacterQulity = TnL_CHAR_FIXED_HIGH;
	}
	else if ( m_dwRenderPSType==ID_RENDER_FIXED ) 
	{
		RENDERPARAM::g_emCharacterQulity = TnL_CHAR_FIXED;
	}

	// 와이어프레임 때는 HDR 끔.
	if ( g_RendState.bWireFrame )
	{
		RENDERPARAM::g_bEnable_HDR = FALSE;
	}


    // VisualMaterialFX 파일이 변경되었다면 다시 재 로딩함.
    if ( NS_VM_MANAGER::NeedReload( m_fElapsedTime ) )
    {
        // VisualMaterial FX 정리함.
        NS_VM_MANAGER::ClearUpVMFX( FALSE );

        // TextureEff Reload
        {
            DxSkinChar* pSkinChar = GetSkinChar();
            if ( pSkinChar )
            {
                pSkinChar->ReloadVMFX( GetD3dDevice() );
            }

	        //DxSkinCharData* pSkinCharData = GetSkinCharData();
         //   if ( pSkinCharData )
         //   {
         //       pSkinCharData->ReloadVMFX( GetD3dDevice() );
         //   }

            DxSkinPiece* pSkinPiece = GetSkinPiece();
            if ( pSkinPiece )
            {
                pSkinPiece->ReloadVMFX( GetD3dDevice() );
            }
        }

        CMainFrame* pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	    pFrame->m_cDlgEdit.ResetDialog();
    }

	HRESULT hr(S_OK);

	DxFogMan::GetInstance().RenderFogSB ( m_pd3dDevice, TRUE );

	LPDIRECT3DSURFACEQ pColorBuffer(NULL);
	if ( RENDERPARAM::g_bEnable_HDR )
	{
		pColorBuffer = DxSurfaceTex::GetInstance().m_pColorSuf16F_HDR;
		
	}
	else
	{
		pColorBuffer = DxSurfaceTex::GetInstance().m_pColorBuffer;
	}

	m_pd3dDevice->SetRenderTarget( 0, pColorBuffer );
	hr = m_pd3dDevice->SetDepthStencilSurface( DxSurfaceTex::GetInstance().m_pSufFourCC_INTZ );

	D3DCOLOR colorClear = D3DCOLOR_XRGB(89,89,89);
	if ( !m_spLandMan->GetNameWLD().empty() )
	{
		colorClear = DxFogMan::GetInstance().GetDayColor();
	}

	// Clear the viewport
	hr = m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
						colorClear, 1.0f, 0L );

	// Begin the scene
	if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
	{
		// Note : 월드에딧에서는 정보를 받아와야 하기 때문에 이 Code 가 추가됨.
		NSMaterialManager::SetFogParam( DxFogMan::GetInstance().GetFogStart_RealTime(), DxFogMan::GetInstance().GetFogEnd_RealTime() );

		//// 구름 그림자
		////
		//if ( !m_spLandMan->GetNameWLD().empty() )
		//{
		//	DxWeatherMan::GetInstance()->Render_Prev ( m_pd3dDevice );
		//}

		//	Note : Glow 
		DxGlowMan::GetInstance().RenderTex ( m_pd3dDevice );

		//	Note : Light 설정.
		//
		DxLightMan::GetInstance()->Render ( m_pd3dDevice, FALSE, FALSE );

		//	Note : 버텍스 쉐이더 콘스탄트 설정.
		//
		DXLIGHT &Light = *DxLightMan::GetInstance()->GetDirectLight ();
		m_pd3dDevice->SetVertexShaderConstantF ( VSC_LIGHTDIRECT, (float*)&Light.m_Light.Direction, 1 );
		m_pd3dDevice->SetVertexShaderConstantF ( VSC_LIGHTDIFFUSE, (float*)&Light.m_Light.Diffuse, 1 );
		m_pd3dDevice->SetVertexShaderConstantF ( VSC_LIGHTAMBIENT, (float*)&Light.m_Light.Ambient, 1 );

		D3DXVECTOR3 &vFromPt = DxViewPort::GetInstance().GetFromPt ();
		m_pd3dDevice->SetVertexShaderConstantF ( VSC_CAMERAPOSITION, (float*)&vFromPt, 1 );

		//	Note : 포인트 라이트 VertexShaderConstant
		//
		D3DLIGHTQ	pLight;
		D3DXVECTOR4	vPointPos;
		D3DXVECTOR3	vPointDiff;
		for ( int i=0; i<3; ++i )
		{
			if ( DxLightMan::GetInstance()->GetClosedLight(i+1) )
			{
				pLight = DxLightMan::GetInstance()->GetClosedLight(i+1)->m_Light;
				vPointDiff = D3DXVECTOR3 ( pLight.Diffuse.r, pLight.Diffuse.g, pLight.Diffuse.b );
				vPointPos.x = pLight.Position.x;
				vPointPos.y = pLight.Position.y;
				vPointPos.z = pLight.Position.z;
				vPointPos.w = pLight.Range;
			}
			else
			{
				vPointPos = D3DXVECTOR4 ( 0.f, 0.f, 0.f, 0.1f );
				vPointDiff = D3DXVECTOR3 ( 0.f, 0.f, 0.f );
			}
			m_pd3dDevice->SetVertexShaderConstantF (i*2+VSC_PLIGHTPOS01, (float*)&vPointPos, 1);
			m_pd3dDevice->SetVertexShaderConstantF (i*2+VSC_PLIGHTDIFF01, (float*)&vPointDiff, 1);
		}

		D3DXMATRIX matView = DxViewPort::GetInstance().GetMatView();
		D3DXMATRIX matProj = DxViewPort::GetInstance().GetMatProj();

		m_pd3dDevice->SetVertexShaderConstantF ( VSC_SKIN_DEFAULT, (float*)&D3DXVECTOR4 (1.f, 0.5f, 0.f, 765.01f), 1 );

		D3DXVECTOR3	vLightVector = DxLightMan::GetInstance()->GetDirectLight()->m_Light.Direction;
		D3DXVec3TransformNormal ( &vLightVector, &vLightVector, &matView );
		D3DXVec3Normalize ( &vLightVector, &vLightVector);
		vLightVector = -vLightVector;
		m_pd3dDevice->SetVertexShaderConstantF ( VSC_LIGHTDIRECT_VIEW, (float*)&vLightVector, 1 );

		D3DXMatrixTranspose( &matView, &matView );
		D3DXMatrixTranspose( &matProj, &matProj );

		m_pd3dDevice->SetVertexShaderConstantF ( VSC_MATWVP_01, (float*)&matView, 4 );
		m_pd3dDevice->SetVertexShaderConstantF ( VSC_MATWVP_02, (float*)&matProj, 4 );

		D3DXVECTOR3	vDirect;
		vDirect.x	=  -Light.m_Light.Direction.x;
		vDirect.y	=  -Light.m_Light.Direction.y;
		vDirect.z	=  -Light.m_Light.Direction.z;
		D3DXVec3Normalize ( &vDirect, &vDirect );
		m_pd3dDevice->SetPixelShaderConstantF ( 0, (float*)&vDirect, 1 );				// 빛 방향
		m_pd3dDevice->SetPixelShaderConstantF ( 1, (float*)&Light.m_Light.Diffuse, 1 );
		m_pd3dDevice->SetPixelShaderConstantF ( 2, (float*)&Light.m_Light.Ambient, 1 );

		if ( g_RendState.bWireFrame )	m_pd3dDevice->SetRenderState ( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
		else							m_pd3dDevice->SetRenderState ( D3DRS_FILLMODE, D3DFILL_SOLID );
		m_pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL );

		if ( g_RendState.bFlane )
			EDITMESHS::RENDERPLANE ( m_pd3dDevice, D3DXVECTOR3(0,0,0), D3DXVECTOR3(100,0,100), D3DXVECTOR3(10,0,10) );

		int nCount = 1;
		if( g_bOBJECT100 )	nCount = 100;

		if( g_RendState.bBGTex )
		{
			RenderBackGround();
		}

        CDebugSet::ToView( 16, "LOD Distance : %.4f", D3DXVec3Length(&DxViewPort::GetInstance().GetFromPt()) );

		// PhysX 물리엔진 적용
		NSPhysX::Render( m_fElapsedTime );




		// 지형 그림자를 생성하도록 한다. 
		{
			// SpotLight 셋팅.
			//NSShadowLight::FrameMove( m_fElapsedTime );

			DxShadowMap::GetInstance().ClearShadow ( m_pd3dDevice, m_spLandMan->GetUsedMaterialSystem() );

			//m_pCharacter->RenderShadow( m_pd3dDevice, FALSE );
			DxShadowMap::GetInstance().RenderShadowCharMob ( &m_SkinChar, m_SkinMat, m_pd3dDevice, m_spLandMan->GetUsedMaterialSystem() ? FALSE : TRUE );

			m_spLandMan->RenderShadow( m_pd3dDevice );

			// PS_3_0_HIGH 이상일 경우만 동작한다.
			//if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_CSM )
			{
				m_spLandMan->RenderShadow_SpotLight1( m_pd3dDevice );
				//m_pCharacter->RenderShadow_SpotLight( m_pd3dDevice, m_scpLandMan->GetUsedMaterialSystem(), 0 );
				DxShadowMap::GetInstance().RenderShadowCharMob_SpotLight( &m_SkinChar, m_SkinMat, m_pd3dDevice, m_spLandMan->GetUsedMaterialSystem(), 0 );

				m_spLandMan->RenderShadow_SpotLight2( m_pd3dDevice );
				//m_pCharacter->RenderShadow_SpotLight( m_pd3dDevice, m_scpLandMan->GetUsedMaterialSystem(), 1 );
				DxShadowMap::GetInstance().RenderShadowCharMob_SpotLight( &m_SkinChar, m_SkinMat, m_pd3dDevice, m_spLandMan->GetUsedMaterialSystem(), 1 );
			}
		}

		// 환경맵
		{
			DxEnvironment::GetInstance().ResetReflectionCV();
			DxEnvironment::GetInstance().RenderReflectionMaterial( m_pd3dDevice, m_spLandMan.get() );
		}

		// Render
		{
			//////////////////////////////////////////////////////////////////////////
			// Sky Render
			m_pd3dDevice->SetRenderTarget( 0, DxSurfaceTex::GetInstance().m_pColorSuf16F_HDR );


			//////////////////////////////////////////////////////////////////////////
			//					RenderDefferedOpaqueForPSF
			D3DXMATRIX matWorld;
			D3DXMatrixIdentity( &matWorld );

			{
				NSDeffered::RenderDefferedForSM30NM
				(
					m_pd3dDevice,
					colorClear,
					m_spLandMan.get(),
					DxSurfaceTex::GetInstance().GetWidth(),
					DxSurfaceTex::GetInstance().GetHeight(),
					pColorBuffer,
					DxSurfaceTex::GetInstance().m_pColorSuf_1st,
					DxSurfaceTex::GetInstance().m_pColorSuf_4th,
					DxSurfaceTex::GetInstance().m_pColorSuf_2nd,
					DxSurfaceTex::GetInstance().m_pColorSuf_3rd,	// Color는 unsign64 를 쓰겠다.
					DxSurfaceTex::GetInstance().m_pColorSuf_LBuffer,
					DxSurfaceTex::GetInstance().m_pSufFourCC_INTZ,
					DxSurfaceTex::GetInstance().m_pColorTex_1st,
					DxSurfaceTex::GetInstance().m_pColorTex_4th,
					DxSurfaceTex::GetInstance().m_pColorTex_2nd,
					DxSurfaceTex::GetInstance().m_pColorTex_3rd, 	// Color는 unsign64 를 쓰겠다.
					DxSurfaceTex::GetInstance().m_pColorTex_LBuffer,
					DxSurfaceTex::GetInstance().m_pTexFourCC_INTZ, 
					matWorld,
					DxViewPort::GetInstance().GetMatView(),
					DxViewPort::GetInstance().GetMatProj(),
					DxViewPort::GetInstance().GetClipVolume(),
					FALSE
				);
			}

			// 지형 물 렌더링
			NSWaterMain::Render( m_pd3dDevice );
		}



		// 
		//m_scpLandMan->Re( m_fTime, m_fElapsedTime, DxViewPort::GetInstance().GetClipVolume() );

		for( int i=0; i<nCount; ++i )
		{
			switch ( m_emObjType )
			{
			case EOBJ_SKINOBJ:
				{
					DWORD	dwNormalizeNormals;
					m_pd3dDevice->GetRenderState( D3DRS_NORMALIZENORMALS, &dwNormalizeNormals );
					m_pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );
					m_SkinObject.Render ( m_pd3dDevice, m_SkinMat );
					m_pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, dwNormalizeNormals );

					if( m_emRenderType==RT_BONE )
					{
						m_SkinChar.RenderBone( m_pd3dDevice );
					}
				}
				break;

				// 플라이 카메라 페이지에도 캐릭터가 나와야한다;
			case EOBJ_FLYCAMERA:
			case EOBJ_SKINCHAR:
				{

					POINT ptCursor;
					GetCursorPos( &ptCursor );
					ScreenToClient( &ptCursor );
					CDebugSet::ToView( 10, "MousePos : %dx %dy", ptCursor.x, ptCursor.y );
					CDebugSet::ToView( 9, "TargetPos : %.4fx %.4fy %.4fz", m_vTargetPos.x, m_vTargetPos.y, m_vTargetPos.z );
					switch(g_MoveType)
					{
					case MOVE_STAY:
						CDebugSet::ToView( 8, "Stay!!" );
						break;
					case MOVE_WALK:
						CDebugSet::ToView( 8, "Walk!!" );
						break;
					case MOVE_RUN:
						CDebugSet::ToView( 8, "Run!!" );
						break;
					}

					D3DLIGHTQ sLightBackUp;
					if ( m_SkinChar.IsUseWorldColor() )
					{
						m_pd3dDevice->GetLight( 0, &sLightBackUp );

						D3DLIGHTQ sLight;
						sLight = sLightBackUp;
						sLight.Diffuse.r = 0.f;
						sLight.Diffuse.g = 0.f;
						sLight.Diffuse.b = 0.f;
						m_pd3dDevice->SetLight( 0, &sLight );
					}
					
					if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
					{
						m_SkinChar.RenderFixed( m_pd3dDevice, TRUE, 1.f );
					}
					else
					{
						m_SkinChar.SetPhysX_Cloth( TRUE );
						m_SkinChar.RenderMaterialOpaque ( m_pd3dDevice, 1.f );
						m_SkinChar.RenderMaterialHardAlpha ( m_pd3dDevice, 1.f );
						m_SkinChar.RenderMaterialSoftAlpha ( m_pd3dDevice, 1.f );
						m_SkinChar.RenderMaterialEffect ( m_pd3dDevice, 1.f );
					}

					if ( m_SkinChar.IsUseWorldColor() )
					{
						m_pd3dDevice->SetLight( 0, &sLightBackUp );
					}

					if( g_bAABBBOX_VISIBLE )
					{
						DxSkinCharData *pSkinCharData = GetSkinCharData();
						pSkinCharData->RenderAABBBox( m_pd3dDevice );
					}

					if( m_emRenderType==RT_BONE )
					{
						m_SkinChar.RenderBone( m_pd3dDevice );
					}

					if ( m_emObjType == EOBJ_FLYCAMERA )
					{
						// 플라이 카메라 앵커 렌더;
						CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
						{
							GLFlyCameraKeyFrameControl& keyframeCtrl = pFrame->m_cDlgEdit.GetSheetChar()->m_FlyCameraPage.GetKeyFrameCtrl();
							keyframeCtrl.Render(0.0f, m_fElapsedTime);
						}

						D3DXVECTOR3 vPos(DxViewPort::GetInstance().GetFromPt()), vLook(DxViewPort::GetInstance().GetLookatPt());
						CDebugSet::ToView(17, "Camera Position : %.4fx, %.4fy, %.4fz", vPos.x, vPos.y, vPos.z);
						CDebugSet::ToView(18, "Camera Lookat : %.4fx, %.4fy, %.4fz", vLook.x, vLook.y, vLook.z);
					}
				}
				break;
#ifndef DEF_SKIN_SYNC
			case EOBJ_SKINPIECE:
				{
					DWORD	dwNormalizeNormals;
					m_pd3dDevice->GetRenderState( D3DRS_NORMALIZENORMALS, &dwNormalizeNormals );
					m_pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );
//					if( m_emRenderType==RT_WIREFRAME ) m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
//					else							   m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );

					static float fTestRotateX = 0.0f;
					static float fTestRotateY = 0.0f;

					BYTE keyState[256];
					bool bNumLock = false;
					if ( GetKeyboardState( keyState ) )
					{
						if ( keyState[ VK_NUMLOCK ] & 1 )
							bNumLock = true;
					}

					if ( !bNumLock )
					{	// NumLock 이 꺼져있을때만 작동.
						if( DxInputDevice::GetInstance().GetKeyState( DIK_NUMPAD8 ) & DXKEY_DOWNED )
						{						
							fTestRotateX += 1.0f;
						}else if( DxInputDevice::GetInstance().GetKeyState( DIK_NUMPAD2 ) & DXKEY_DOWNED )
						{						
							fTestRotateX -= 1.0f;
						}
						
						if( DxInputDevice::GetInstance().GetKeyState( DIK_NUMPAD4 ) & DXKEY_DOWNED )
						{						
							fTestRotateY += 1.0f;
						}else if( DxInputDevice::GetInstance().GetKeyState( DIK_NUMPAD6 ) & DXKEY_DOWNED )
						{						
							fTestRotateY -= 1.0f;
						}
						if( DxInputDevice::GetInstance().GetKeyState( DIK_NUMPAD5 ) & DXKEY_DOWNED )
						{	
							fTestRotateX = 0.0f;
							fTestRotateY = 0.0f;
						}
					}

					D3DXMATRIX matRotateX;
					D3DXMATRIX matRotateY;
					D3DXMATRIX matRotateOut;
					D3DXMatrixIdentity( &matRotateX ); 
					D3DXMatrixIdentity( &matRotateY ); 
					D3DXMatrixRotationX( &matRotateX, D3DXToRadian(fTestRotateX) );
					D3DXMatrixRotationY( &matRotateY, D3DXToRadian(fTestRotateY) );
					D3DXMatrixMultiply( &matRotateOut, &matRotateX, &matRotateY );

					m_SkinMat = matRotateOut;

					DWORD dwLButton = DxInputDevice::GetInstance().GetMouseState ( DXMOUSE_LEFT );
					if ( g_RendState.bMovePiece && ( dwLButton&(DXKEY_PRESSED|DXKEY_DRAG) ) )
					{
						D3DXVECTOR3 vPickRayDir;
						D3DXVECTOR3 vPickRayOrig;

						D3DSURFACE_DESC d3dsdBackBuffer;
						LPDIRECT3DSURFACE9 pBackBuffer = NULL;
						m_pd3dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
						pBackBuffer->GetDesc( &d3dsdBackBuffer );

						POINT ptCursor;
						GetCursorPos( &ptCursor );
						ScreenToClient( &ptCursor );

						if ( ((int)ptCursor.x<0) || ((int)ptCursor.x>(int)m_d3dsdBackBuffer.Width) || 
							((int)ptCursor.y<0) || ((int)ptCursor.y>(int)m_d3dsdBackBuffer.Height) )
						{
							// 아무 동작 안함.
						}
						else
						{
							RECT rect;
							GetWindowRect( &rect );

							D3DXMATRIX matProj, matView, matWorld;
							m_pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProj );
							m_pd3dDevice->GetTransform( D3DTS_VIEW, &matView );
							m_pd3dDevice->GetTransform( D3DTS_WORLD, &matWorld );

							// Compute the vector of the Pick ray in screen space
							D3DXVECTOR3 v;
							v.x =  ( ( ( 2.0f * ptCursor.x ) / d3dsdBackBuffer.Width  ) - 1 ) / matProj._11;
							v.y = -( ( ( 2.0f * ptCursor.y ) / d3dsdBackBuffer.Height ) - 1 ) / matProj._22;
							v.z =  1.0f;

							D3DXMATRIX mWorldView = matWorld * matView;
							D3DXMATRIX m;
							D3DXMatrixInverse( &m, NULL, &mWorldView );

							// Transform the screen space Pick ray into 3D space
							vPickRayDir.x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
							vPickRayDir.y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
							vPickRayDir.z  = v.x*m._13 + v.y*m._23 + v.z*m._33;
							vPickRayOrig.x = m._41;
							vPickRayOrig.y = m._42;
							vPickRayOrig.z = m._43;

							float fLength = D3DXVec3Length( &vPickRayOrig );

							m_SkinMat._41 = vPickRayOrig.x + vPickRayDir.x * fLength;
							m_SkinMat._42 = vPickRayOrig.y + vPickRayDir.y * fLength;
							m_SkinMat._43 = vPickRayOrig.z + vPickRayDir.z * fLength;
						}

						if ( pBackBuffer )
						{
							pBackBuffer->Release();
						}
					}

                    // Light 셋팅.
                    D3DLIGHTQ	sSrcLight00;
                    {
	                    D3DLIGHTQ	sDestLight00;

	                    m_pd3dDevice->GetLight ( 0, &sSrcLight00 );

	                    sDestLight00 = sSrcLight00;

                        if ( g_bAmbientTest_CHAREDIT )
                        {
                            sDestLight00.Ambient.r = static_cast<float>(g_dwAmbient_CHAREDIT)/255.f;
	                        sDestLight00.Ambient.g = static_cast<float>(g_dwAmbient_CHAREDIT)/255.f;
	                        sDestLight00.Ambient.b = static_cast<float>(g_dwAmbient_CHAREDIT)/255.f;
                        }

                        m_pd3dDevice->SetLight ( 0, &sDestLight00 );
                    }
#ifdef DEF_ERASE
                    m_SkinPiece.UpdateEffect(m_SkinPiece.m_vecEFFECT, m_pd3dDevice);
#endif
					m_SkinPiece.Render( m_pd3dDevice, m_SkinMat, TRUE );

                    // Light 복원
                    {
                        m_pd3dDevice->SetLight ( 0, &sSrcLight00 );
                    }
					
					m_pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, dwNormalizeNormals );
					
					//  [10/14/2013 gbgim];
					// 이펙트 페이지에서 정해진 충돌위치가 이펙트 페이지를 벗어나도 그대로있어서;
					// 스피어가 출력되어 주석처리함;
					if( m_emRenderType==RT_DEFAULT )
					{
						if( m_SkinPiece.m_skeletonRes.IsValid() )
						{
							m_pd3dDevice->SetRenderState( D3DRS_ZFUNC,		D3DCMP_ALWAYS );
							EDITMESHS::RENDERSPHERE( m_pd3dDevice,			m_vCollision,		0.01f*DxViewPort::GetInstance().GetDistance() );
							EDITMESHS::RENDERSPHERE( m_pd3dDevice,			g_vCLOSED_POINT,	0.005f*DxViewPort::GetInstance().GetDistance() );
							m_pd3dDevice->SetRenderState( D3DRS_ZFUNC,		D3DCMP_LESSEQUAL );
						}
					}
					if( m_emRenderType==RT_BONE )
					{
						if( m_SkinPiece.m_skeletonRes.IsValid() )
						{					

							m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,		FALSE );
							m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	FALSE );

							m_SkinPiece.m_skeletonRes.get()->EditLineSphere( m_pd3dDevice );

							m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,		TRUE );
							m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	TRUE );

							// Note : 본을 클릭하기 위함.
                            if( DxInputDevice::GetInstance().GetMouseState(DXMOUSE_LEFT)&DXKEY_UP )
							{
								// Note : 픽킹을 하여 체크 된 Bone Name을 얻는다.
								//			현재 픽킹 범위를 넓혀 놓았다.
								D3DXVECTOR3 vTargetPt, vFromPt;
								vFromPt = DxViewPort::GetInstance().GetFromPt ();
								BOOL bTargetted = DxViewPort::GetInstance().GetMouseTargetPosWnd ( vTargetPt );
								if ( bTargetted )
								{
									m_SkinPiece.m_skeletonRes.get()->IsCollision( vFromPt, vTargetPt, g_strPICK_BONE, 0.006f*DxViewPort::GetInstance().GetDistance() );
									CDebugSet::ToView( 14, "Select Bone : %s", g_strPICK_BONE.c_str() );
								}
							}
						}
					}else if( m_emRenderType==RT_WIREFRAME )
					{

						if( m_SkinPiece.m_skeletonRes.IsValid() )
						{
							m_SkinPiece.MeshEditRender( m_pd3dDevice, m_SkinMat, NULL );

							m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,		FALSE );
							m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	FALSE );

							m_SkinPiece.m_skeletonRes.get()->EditLineSphere( m_pd3dDevice );

							if( m_vSkinBonePickPos != D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) )
							{
								EDITMESHS::RENDERSPHERE( m_pd3dDevice, m_vSkinBonePickPos, 0.004f*DxViewPort::GetInstance().GetDistance(), NULL, 0xff0000ff );
							}
							

							m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,		TRUE );
							m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	TRUE );

							if( m_vSkinMeshPickPos != D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) )
							{
								EDITMESHS::RENDERSPHERE( m_pd3dDevice, m_vSkinMeshPickPos, 0.004f*DxViewPort::GetInstance().GetDistance(), NULL, 0xffff0000 );

								D3DXVECTOR3 vPos, vTemp;
								D3DXVec3Add( &vPos, &m_vSkinMeshPickPos, D3DXVec3Scale( &vTemp, &g_vCLOSED_NORMAL, 3.0f ) );
								EDITMESHS::RENDERLINE( m_pd3dDevice, m_vSkinMeshPickPos, vPos, 0xff00ff00 );

							}

							if( g_vCLOSED_POINT != D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) )
							{
								EDITMESHS::RENDERSPHERE( m_pd3dDevice, g_vCLOSED_POINT, 0.004f*DxViewPort::GetInstance().GetDistance(), NULL, 0xff00ff00 );
							}

							if( m_vCollision != D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) )
							{
								EDITMESHS::RENDERSPHERE( m_pd3dDevice, m_vCollision, 0.004f*DxViewPort::GetInstance().GetDistance(), NULL, 0xff00ff );
							}

							/*if( m_vSkinMeshPickPos1 != D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) )
							{
								EDITMESHS::RENDERSPHERE( m_pd3dDevice, m_vSkinMeshPickPos1, 0.003f*DxViewPort::GetInstance().GetDistance(), NULL, 0xffff0000 );
							}
							if( m_vSkinMeshPickPos2 != D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) )
							{
								EDITMESHS::RENDERSPHERE( m_pd3dDevice, m_vSkinMeshPickPos2, 0.003f*DxViewPort::GetInstance().GetDistance(), NULL, 0xffff0000 );
								EDITMESHS::RENDERLINE( m_pd3dDevice, m_vSkinMeshPickPos1, m_vSkinMeshPickPos2, 0xffff0000 );
							}*/

						}
					}

					m_SkinPiece.RenderTEST( m_pd3dDevice, m_SkinMat, NULL );

					if ( m_bSkinTraceVertDraw )
					{
						m_SkinPiece.DrawTraceVert ( m_pd3dDevice );
						m_SkinPiece.DrawSelTraceVert ( m_pd3dDevice, m_strSelectedTracVert.GetString() );
					}
				}
				break;
#endif
			case EOBJ_ATTBONE:
				{
					POINT ptCursor;
					GetCursorPos( &ptCursor );
					ScreenToClient( &ptCursor );
					CDebugSet::ToView( 10, "MousePos : %dx %dy", ptCursor.x, ptCursor.y );
					CDebugSet::ToView( 9, "TargetPos : %.4fx %.4fy %.4fz", m_vTargetPos.x, m_vTargetPos.y, m_vTargetPos.z );
					switch(g_MoveType)
					{
					case MOVE_STAY:
						CDebugSet::ToView( 8, "Stay!!" );
						break;
					case MOVE_WALK:
						CDebugSet::ToView( 8, "Walk!!" );
						break;
					case MOVE_RUN:
						CDebugSet::ToView( 8, "Run!!" );
						break;
					}


					if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
					{
						m_AttBone.RenderFixed( m_pd3dDevice, 1.f, m_SkinMat, FALSE, TRUE, FALSE );
					}
					else
					{
						DxClothColl sClothColl;
						m_AttBone.SetPhysX_Cloth( TRUE );
						m_AttBone.RenderMaterialOpaque ( m_pd3dDevice, 1.f, FALSE, TRUE, &sClothColl, NULL, 1.f );
						m_AttBone.RenderMaterialHardAlpha ( m_pd3dDevice, 1.f );
						m_AttBone.RenderMaterialSoftAlpha ( m_pd3dDevice, 1.f );
						m_AttBone.RenderEff( m_pd3dDevice, 1.f );
						m_AttBone.RenderMaterialAdd( m_pd3dDevice );
						m_AttBone.RenderMaterialGlow( m_pd3dDevice );
					}
					//m_AttBone.Render ( m_pd3dDevice, 1.f, m_SkinMat );

					if( g_bAABBBOX_VISIBLE )
					{
						DxAttBoneData *pAttBoneData = GetAttBoneData();
						pAttBoneData->RenderAABBBox( m_pd3dDevice );
					}

					if( m_emRenderType==RT_BONE )
					{
						m_AttBone.RenderBone( m_pd3dDevice );
					}
				}
				break;
			case EOBJ_ATTLINK:
				{
					POINT ptCursor;
					GetCursorPos( &ptCursor );
					ScreenToClient( &ptCursor );
					CDebugSet::ToView( 10, "MousePos : %dx %dy", ptCursor.x, ptCursor.y );
					CDebugSet::ToView( 9, "TargetPos : %.4fx %.4fy %.4fz", m_vTargetPos.x, m_vTargetPos.y, m_vTargetPos.z );
                    

					if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
					{
						m_AttBoneLink.RenderFixed( m_pd3dDevice, 1.f, FALSE, TRUE, FALSE, 1.f );
					}

					if ( m_pAttBoneLinkSkeleton )
					{
						DxClothColl sClothColl;
						m_AttBoneLink.SetPhysX_Cloth( TRUE );
						m_AttBoneLink.RenderMaterialOpaque ( m_pd3dDevice, 1.f, FALSE, TRUE, &sClothColl, NULL, 1.f );
						m_AttBoneLink.RenderMaterialHardAlpha ( m_pd3dDevice, 1.f );
						m_AttBoneLink.RenderMaterialSoftAlpha ( m_pd3dDevice, 1.f );
						m_AttBoneLink.RenderEff( m_pd3dDevice, 1.f );
						m_AttBoneLink.RenderMaterialAdd( m_pd3dDevice );
						m_AttBoneLink.RenderMaterialGlow( m_pd3dDevice );

						// Cps 활성화시 보여준다.
						m_AttBoneLink.RenderCpsForEdit( m_pd3dDevice, m_pAttBoneLinkSkeleton );
					}

					//m_AttBoneLink.Render ( m_pd3dDevice, 1.f, m_pAttBoneLinkSkeleton );

					if( m_pAttBoneLinkSkeleton )
					{					

						if( m_AttBoneLink.m_emPieceCharType == PIECE_RHAND ||
							m_AttBoneLink.m_emPieceCharType == PIECE_LHAND ||
							m_AttBoneLink.m_emPieceCharType == PIECE_RHAND_HIDDEN ||
							m_AttBoneLink.m_emPieceCharType == PIECE_LHAND_HIDDEN )
						{
							//추후 작업예정
							//if( m_SkinPiece.m_pmcMesh )
							//{
							//	DxBoneTrans* pSlotHeld = m_pAttBoneLinkSkeleton->FindBone_t( m_SkinPiece.m_pmcMesh->m_strBoneName );
							//	if( pSlotHeld ) 
							//		m_SkinPiece.m_pmcMesh->m_matStaticLinkBone = pSlotHeld->matCombined;
							//		//m_SkinPiece.m_pmcMesh->m_ppBoneMatrixPtrs[2] = &pSlotHeld->matCombined;
							//}
						}
						m_SkinPiece.DrawMeshContainer( m_pd3dDevice );

						D3DXMATRIX mat;
						D3DXMatrixIdentity( &mat );
						m_pAttBoneLinkSkeleton->ResetBone_PureThread();
						m_pAttBoneLinkSkeleton->UpdateBones_PureThread( m_pAttBoneLinkSkeleton->m_skeletonPart.pBoneRoot, mat, 1.f, 1.f );

						m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,		FALSE );
						m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	FALSE );

						m_pAttBoneLinkSkeleton->EditLineSphere( m_pd3dDevice );

						m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,		TRUE );
						m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	TRUE );

						// Note : 본을 클릭하기 위함.
						if( DxInputDevice::GetInstance().GetMouseState(DXMOUSE_LEFT)&DXKEY_UP )
						{
							// Note : 픽킹을 하여 체크 된 Bone Name을 얻는다.
							//			현재 픽킹 범위를 넓혀 놓았다.
							D3DXVECTOR3 vTargetPt, vFromPt;
							vFromPt = DxViewPort::GetInstance().GetFromPt ();
							BOOL bTargetted = DxViewPort::GetInstance().GetMouseTargetPosWnd ( vTargetPt );
							if ( bTargetted )
							{
								m_pAttBoneLinkSkeleton->IsCollision( vFromPt, vTargetPt, g_strPICK_BONE, 0.006f*DxViewPort::GetInstance().GetDistance() );
								CDebugSet::ToView( 14, "Select Bone : %s", g_strPICK_BONE.c_str() );
							}
						}

						if( m_vSkinBonePickPos != D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) )
						{
							EDITMESHS::RENDERSPHERE( m_pd3dDevice, m_vSkinBonePickPos, 0.004f*DxViewPort::GetInstance().GetDistance(), NULL, 0xffff0000 );
						}

						m_pAttBoneLinkSkeleton->CheckSphere( m_pd3dDevice, g_strPICK_BONE.c_str() );
					}
					
				}
				break;
			case EOBJ_VEHICLE:
				{
					POINT ptCursor;
					GetCursorPos( &ptCursor );
					ScreenToClient( &ptCursor );
					CDebugSet::ToView( 10, "MousePos : %dx %dy", ptCursor.x, ptCursor.y );
					CDebugSet::ToView( 9, "TargetPos : %.4fx %.4fy %.4fz", m_vTargetPos.x, m_vTargetPos.y, m_vTargetPos.z );
					switch(g_MoveType)
					{
					case MOVE_STAY:
						CDebugSet::ToView( 8, "Stay!!" );
						break;
					case MOVE_WALK:
						CDebugSet::ToView( 8, "Walk!!" );
						break;
					case MOVE_RUN:
						CDebugSet::ToView( 8, "Run!!" );
						break;
					}


					if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
					{
						m_Vehicle.RenderFixed( m_pd3dDevice, TRUE, 1.f );
					}
					else
					{
						m_Vehicle.SetPhysX_Cloth( TRUE );
						m_Vehicle.RenderMaterialOpaque ( m_pd3dDevice, 1.f );
						m_Vehicle.RenderMaterialHardAlpha ( m_pd3dDevice, 1.f );
						m_Vehicle.RenderMaterialSoftAlpha ( m_pd3dDevice, 1.f );
						m_Vehicle.RenderMaterialEffect( m_pd3dDevice, 1.f );
					}

					

					if( g_bAABBBOX_VISIBLE )
					{
						DxVehicleData *pVehicleData = GetVehicleData();
						pVehicleData->RenderAABBBox( m_pd3dDevice );
					}

					if( m_emRenderType==RT_BONE )
					{
						m_Vehicle.RenderBone( m_pd3dDevice );
					}

					if( g_bViewPassenger )
					{
						//if( !g_bMoveTest ) // same if else
						//{
						if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
						{
							for (int i=0; i<MAX_PASSENGER_COUNT; ++i)
							{
								m_SkinPassenger[i].RenderFixed(m_pd3dDevice, TRUE, 1.f );
							}
						}
						else
						{
							for (int i=0; i<MAX_PASSENGER_COUNT; ++i)
							{
								m_SkinPassenger[i].SetPhysX_Cloth( TRUE );
								m_SkinPassenger[i].RenderMaterialOpaque ( m_pd3dDevice, 1.f );
								m_SkinPassenger[i].RenderMaterialHardAlpha ( m_pd3dDevice, 1.f );
								m_SkinPassenger[i].RenderMaterialSoftAlpha ( m_pd3dDevice, 1.f );
								m_SkinPassenger[i].RenderMaterialEffect ( m_pd3dDevice, 1.f );
							}
						}

							
						//}
                        //else
                        //{
						//	for (int i=0; i<MAX_PASSENGER_COUNT; ++i)
						//	{
						//		m_SkinPassenger[i].Render(m_pd3dDevice, m_Vehicle.m_sPassengerData[i].matWorld);
						//	}
						//}
					}
				}
				break;
			};
		};





		//////////////////////////////////////////////////////////////////////////
		// 이전 맵일 경우.
		//m_pCharacter->RenderShadow( m_pd3dDevice, TRUE );
		//DxShadowMap::GetInstance().RenderShadowCharMob ( &m_SkinChar, m_SkinMat, m_pd3dDevice, m_scpLandMan->GetUsedMaterialSystem() ? FALSE : TRUE );

		// ShadowMap ImageBlur
		DxShadowMap::GetInstance().LastImageBlur( m_pd3dDevice, m_spLandMan->GetUsedMaterialSystem() );







		//// Land_EFF
		{
			//////////////////////////////////////////////////////////////////////////
			//					RenderDefferedOpaqueForPSF
			D3DXMATRIX matWorld;
			D3DXMatrixIdentity( &matWorld );
			D3DCOLOR colorClear = D3DCOLOR_XRGB(0,0,0);

			{
				NSDeffered::RenderDefferedForSM30NM
				(
					m_pd3dDevice,
					colorClear,
					m_spLandMan.get(),
					DxSurfaceTex::GetInstance().GetWidth(),
					DxSurfaceTex::GetInstance().GetHeight(),
					pColorBuffer,
					DxSurfaceTex::GetInstance().m_pColorSuf_1st,
					DxSurfaceTex::GetInstance().m_pColorSuf_4th,
					DxSurfaceTex::GetInstance().m_pColorSuf_2nd,
					DxSurfaceTex::GetInstance().m_pColorSuf_3rd,	// Color는 unsign64 를 쓰겠다.
					DxSurfaceTex::GetInstance().m_pColorSuf_LBuffer,
					DxSurfaceTex::GetInstance().m_pSufFourCC_INTZ,
					DxSurfaceTex::GetInstance().m_pColorTex_1st,
					DxSurfaceTex::GetInstance().m_pColorTex_4th,
					DxSurfaceTex::GetInstance().m_pColorTex_2nd,
					DxSurfaceTex::GetInstance().m_pColorTex_3rd, 	// Color는 unsign64 를 쓰겠다.
					DxSurfaceTex::GetInstance().m_pColorTex_LBuffer,
					DxSurfaceTex::GetInstance().m_pTexFourCC_INTZ, 
					matWorld,
					DxViewPort::GetInstance().GetMatView(),
					DxViewPort::GetInstance().GetMatProj(),
					DxViewPort::GetInstance().GetClipVolume(),
					TRUE
				);
			}

			m_spLandMan->RenderEff_Deffered( m_pd3dDevice, DxViewPort::GetInstance().GetClipVolume() );
		}







#ifdef DEF_SKIN_SYNC
        DxSkinPiece* pSkinPiece = GetSkinPiece();

        if (pSkinPiece)
        {
            m_pd3dDevice->SetRenderState( D3DRS_ZFUNC,		D3DCMP_ALWAYS );
            EDITMESHS::RENDERSPHERE( m_pd3dDevice,			m_vCollision,		0.01f*DxViewPort::GetInstance().GetDistance() );
            EDITMESHS::RENDERSPHERE( m_pd3dDevice,			g_vCLOSED_POINT,	0.005f*DxViewPort::GetInstance().GetDistance() );
            m_pd3dDevice->SetRenderState( D3DRS_ZFUNC,		D3DCMP_LESSEQUAL );
        }

        if (pSkinPiece && m_bSkinTraceVertDraw)
        {
            pSkinPiece->DrawTraceVert(m_pd3dDevice, FALSE);
            pSkinPiece->DrawSelTraceVert(m_pd3dDevice, m_strSelectedTracVert.GetString(), FALSE);
        }
#endif

		// Note : Character Scene Graph ~!!
		NSCHARSG::Render( m_pd3dDevice );

		// Note : Optimize
		// Note : 
		OPTMManager::GetInstance().Render_HardAlpha();
		OPTMManager::GetInstance().Render();

		//	Note : 큐브 텍스쳐 렌더링..!!
		//DxCubeMap::GetInstance().Render ( m_pd3dDevice );
		NSCubeMapUtil::NSCubeMapBlend::OnFrameMove( m_fElapsedTime );
		NSCubeMapUtil::NSCubeMapBlend::OnRender( m_pd3dDevice, NSCubeMapCulling::GetCubeMapTexture( DxViewPort::GetInstance().GetLookatPt() ) );

		//	Note : 글로우 처리를 하기위해 사용 한다.
		DxGlowMan::GetInstance().RenderTex ( m_pd3dDevice );
		DxGlowMan::GetInstance().Render( m_pd3dDevice );
		DxGlowMan::GetInstance().RenderBURN( m_pd3dDevice );



		//////////////////////////////////////////////////////////////////////////
		//			HDR - Bloom 적용.
		//m_pd3dDevice->StretchRect( DxSurfaceTex::GetInstance().m_pColorSuf16F_HDR, NULL, DxSurfaceTex::GetInstance().m_pColorBuffer, NULL, D3DTEXF_POINT );
		m_pd3dDevice->SetRenderTarget( 0, DxSurfaceTex::GetInstance().m_pColorBuffer );

		if ( RENDERPARAM::g_bEnable_HDR )
		{
			NSBloomHDR::OnRender( m_pd3dDevice );
		}


		// 지금까지 그린것을 굴절을 위해 복사해놓는다.
		DxEnvironment::GetInstance().UserToRefraction ( m_pd3dDevice );



		//	Note : Wave 되야 하는것을 저장후 마지막으로 뿌린다.
		DxEnvironment::GetInstance().RenderWave ( m_pd3dDevice, DxEffectMan::GetInstance().GetBumpTexture() );

		RenderText();
		CDebugSet::Render ();

// 		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
// 		if ( pFrame && pFrame->m_cDlgEdit.GetActiveIndex() == CHARTAB_FLYCAMERA )
// 		{
// 			GLFlyCameraKeyFrameControl& keyframeCtrl = pFrame->m_cDlgEdit.GetSheetChar()->m_FlyCameraPage.GetKeyFrameCtrl();
// 			keyframeCtrl.Render(0.0f, m_fElapsedTime);
// 		}


		
		// Note : 커서의 Render및 이동,Rotate, 스케일
		m_sObjectMRS.Render( m_pd3dDevice );

        // 키를 누르는 행동.
        {
            CMainFrame *pMainFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
            pMainFrame->m_cDlgEdit.UpdateKeyBoardMouse();
        }

		// End the scene.
		m_pd3dDevice->EndScene();


		// Char Billboard
		if ( g_RendState.bBillboard )
		{
			switch ( m_emObjType )
			{
			case EOBJ_SKINCHAR:
				m_SkinChar.CreateChfBillboard( m_pd3dDevice, g_RendState.bBillboard, NULL );
				break;

			default:
				break;
			};
		}
	}

	return S_OK;
}

/*
#include <set>
#include <sys/stat.h>

void CCharEditView::UpdatePiece()
{
	typedef std::set<LPSTR>				MtlPieceList;
	typedef MtlPieceList::iterator		MtlPieceListIterator;
	typedef std::map<LPSTR, struct tm>	FileTimeList;
	typedef FileTimeList::iterator		FileTimeListIterator;	

	static	FileTimeList	s_FileTime;
	static	FLOAT			s_AccTime = 0;
			MtlPieceList	kPieceList;

	if (m_fTime - s_AccTime < 5.f)
	{
		return;
	}

	if (m_emObjType == EOBJ_SKINCHAR)
	{
		for (int i = 0; i < PIECE_SIZE; ++i)
		{
			if (m_SkinChar.m_PartArray[i].m_dwMaterialNum != 0)
			{
				for (DWORD m = 0; m < m_SkinChar.m_PartArray[i].m_dwMaterialNum; ++m)
				{
					kPieceList.insert(m_SkinChar.m_PartArray[i].m_pmcMesh->pMaterials[m].pTextureFilename);
				}
			}
		}
	}
	else if (m_emObjType == EOBJ_SKINPIECE)
	{
		for (DWORD m = 0; m < m_SkinPiece.m_dwMaterialNum; ++m)
		{
			for (DWORD m = 0; m < m_SkinPiece.m_dwMaterialNum; ++m)
			{
				kPieceList.insert(m_SkinPiece.m_pmcMesh->pMaterials[m].pTextureFilename);
			}
		}
	}
	else if (m_emObjType == EOBJ_ATTBONE)
	{
		//m_AttBone;
	}
	else if (m_emObjType == EOBJ_ATTLINK)
	{
		//m_AttBoneLink;
	}
	else if (m_emObjType == EOBJ_VEHICLE)
	{
		//m_AttBoneLink;
	}

	s_AccTime = m_fTime;

	struct tm t;
	struct _stat buf;

	for (MtlPieceListIterator it = kPieceList.begin(); it != kPieceList.end(); ++it)
	{
		TSTRING strPath = (*it);
		TextureManager::GetInstance().FindAndChangeExtMTF(strPath);

		if (_stat(strPath.c_str(), &buf) == 0)
		{
			SMATERIAL_PIECE &smtrlPiece = m_pPiece->m_pMaterialPiece[i];
			smtrlPiece.m_strTexture		= m_pMaterials[i].strTextureSel;
			smtrlPiece.m_dwZBias		= m_pMaterials[i].dwZBias;
			smtrlPiece.m_dwBlendFlag	= m_pMaterials[i].m_dwBlendFlag;
			smtrlPiece.LoadTexture(GetD3dDevice());

			t = *localtime(&buf.st_mtime);
			s_FileTime.insert(std::make_pair((*it), t));
		}
	}
}
*/

//-----------------------------------------------------------------------------
// Name: RenderText()
// Desc: Renders stats and help text to the scene.
//-----------------------------------------------------------------------------
HRESULT CCharEditView::RenderText()
{
	D3DCOLOR fontColor        = D3DCOLOR_ARGB(255,255,255,255);
    D3DCOLOR fontWarningColor = D3DCOLOR_ARGB(255,0,255,255);
    TCHAR szMsg[MAX_PATH] = TEXT("");

	CD3DFontPar* pD3dFont = DxFontMan::GetInstance().FindFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
	if( !pD3dFont )	return S_OK;

    // Output display stats
    FLOAT fNextLine = 40.0f; 

    lstrcpy( szMsg, m_strDeviceStats );
    fNextLine = 0.0f;
    pD3dFont->DrawText( 2, fNextLine, fontColor, szMsg );

    lstrcpy( szMsg, m_strFrameStats );
    fNextLine = 20.0f;
    pD3dFont->DrawText( 2, fNextLine, fontColor, szMsg );

    return S_OK;
}

HRESULT CCharEditView::OnSize ( int cx, int cy )
{
	if ( cx < 10 )	cx = 10;
	if ( cy < 10 )	cy = 10;

	m_d3dpp.BackBufferWidth  = cx;
	m_d3dpp.BackBufferHeight = cy;

	return Resize3DEnvironment();
}

HRESULT CCharEditView::InvalidateDeviceObjects()
{
	//	Note : 각종 장치들.
	//
	m_pEngineDevice->InvalidateDeviceObjects ();

	m_SkinChar.InvalidateDeviceObjects ();
	m_AttBone.InvalidateDeviceObjects ();
	m_AttBoneLink.InvalidateDeviceObjects();
	m_Vehicle.InvalidateDeviceObjects ();
	m_SkinPiece.InvalidateDeviceObjects ();

	m_sObjectMRS.OnLostDevice( m_pd3dDevice );

	m_spLandMan->InvalidateDeviceObjects();
	

	return S_OK;
}

HRESULT CCharEditView::DeleteDeviceObjects()
{
	//	Note : 각종 장치들.
	//
	m_pEngineDevice->DeleteDeviceObjects ();

	m_SkinChar.DeleteDeviceObjects();
	m_AttBone.DeleteDeviceObjects();
	m_AttBoneLink.DeleteDeviceObjects();
	m_Vehicle.DeleteDeviceObjects();

	m_SkinCharData.ClearAll ();
	m_AttBoneData.ClearAll();
	m_AttBoneLink.ClearAll();
	m_VehicleData.ClearAll();

	m_spLandMan->DeleteDeviceObjects();

	return S_OK;
}

HRESULT CCharEditView::FinalCleanup()
{
	//	Note : 각종 장치들.
	//
	m_pEngineDevice->FinalCleanup ();

	return S_OK;
}

void CCharEditView::SetRenderTypeDefault()
{ 
	g_strPICK_BONE = "";
	m_emRenderType = RT_DEFAULT; 
}

void CCharEditView::ActiveEditMatrix( D3DXMATRIX* matWorld )
{
	m_sObjectMRS.SetMatrix( matWorld );
}

void CCharEditView::DeActiveEditMatrix()
{
	m_sObjectMRS.SetMatrix( NULL );
}

// m_sObjectMRS 의 커서 기능이 활성화인가~? 비활성화인가~?
BOOL CCharEditView::IsActiveEditMatrix()
{
	return m_sObjectMRS.GetMatrix() ? TRUE : FALSE;
}


void CCharEditView::ClearSkinPieceList()
{
	UINT i;
	for( i = 0; i < m_SkinPieceList.size(); i++ )
	{
		m_SkinPieceList[i]->ClearAll();
		SAFE_DELETE( m_SkinPieceList[i] );
	}
	m_SkinPieceList.clear();
}

//-----------------------------------------------------------------------------
// Name: Pick()
// Desc: Checks if mouse point hits geometry
//       the scene.
//-----------------------------------------------------------------------------
BOOL CCharEditView::Pick()
{
    // Get the pick ray from the mouse position
    //if( GetCapture() )
    //{
    D3DXMATRIX matProj;
    m_pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProj );

    POINT ptCursor;
    GetCursorPos( &ptCursor );
    ScreenToClient( &ptCursor );

	if ( ((int)ptCursor.x<0) || ((int)ptCursor.x>(int)m_d3dsdBackBuffer.Width) || 
		((int)ptCursor.y<0) || ((int)ptCursor.y>(int)m_d3dsdBackBuffer.Height) )
	{
		return FALSE;
	}

 //   // Compute the vector of the pick ray in screen space
 //   D3DXVECTOR3 v;
	//v.x =  ( ( ( 2.0f * ptCursor.x ) / m_d3dsdBackBuffer.Width  ) - 1 ) / matProj._11;
 //   v.y = -( ( ( 2.0f * ptCursor.y ) / m_d3dsdBackBuffer.Height ) - 1 ) / matProj._22;
 //   v.z =  1.0f;

 //   // Get the inverse view matrix
 //   D3DXMATRIX matView, m;
 //   m_pd3dDevice->GetTransform( D3DTS_VIEW, &matView );
 //   D3DXMatrixInverse( &m, NULL, &matView );

 //   // Transform the screen space pick ray into 3D space
 //   g_vPICK_RAY_DIR.x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
 //   g_vPICK_RAY_DIR.y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
 //   g_vPICK_RAY_DIR.z  = v.x*m._13 + v.y*m._23 + v.z*m._33;
 //   g_vPICK_RAY_ORIG.x = m._41;
 //   g_vPICK_RAY_ORIG.y = m._42;
 //   g_vPICK_RAY_ORIG.z = m._43;

	//D3DXVec3Normalize ( &g_vPICK_RAY_DIR, &g_vPICK_RAY_DIR );		// Normalize

	return TRUE;
}

void CCharEditView::ClearSkinPassenger()
{
	for( int i = 0; i < MAX_PASSENGER_COUNT; i++ )
	{
		m_SkinPassenger[i].InvalidateDeviceObjects();
		m_SkinPassenger[i].DeleteDeviceObjects();
		m_SkinPassenger[i].ClearAll();
	}
}

void CCharEditView::SetSkinTranslationInfo(D3DXVECTOR3& vSkinPos, float fRotate)
{
	m_fSkinRotate = fRotate;
	m_vTargetPos = vSkinPos;
	m_vSkinPos = vSkinPos;
	MoveUpdateSkinChar();
}

void CCharEditView::GetSkinTranslationInfo(D3DXVECTOR3& IN OUT vOutput, float& IN OUT fRotate ) const
{
	vOutput = m_vSkinPos;
	fRotate = m_fSkinRotate;
}

//-----------------------------------------------------------------------------------------------------------------------
//												C		S	P	I	N		E	X
//-----------------------------------------------------------------------------------------------------------------------
int CSPINEX::FrameMove( BOOL& bUse, BOOL bDefWin )
{
	if( !bUse )	return 0;

	if( DxInputDevice::GetInstance().GetMouseState( DXMOUSE_LEFT ) & DXKEY_UP )
	{
		bUse = FALSE;
		return 0;
	}
	else if( DxInputDevice::GetInstance().GetMouseState( DXMOUSE_LEFT ) & DXKEY_DOWNED )
	{
		// Note : 마우스가 가장자리에 닿았는지 체크
		DxInputDevice::GetInstance().ScreenOverArray( bDefWin );

		return DxInputDevice::GetInstance().GetMouseMoveY();
	}
	else	return 0;
}

int CSPINEX::FrameMoveColor( int& iUse, BOOL bDefWin )
{
	if( iUse == 0 )	return 0;

	if( DxInputDevice::GetInstance().GetMouseState( DXMOUSE_LEFT ) & DXKEY_UP )
	{
		iUse = 0;
		return 0;
	}
	else if( DxInputDevice::GetInstance().GetMouseState( DXMOUSE_LEFT ) & DXKEY_DOWNED )
	{
		// Note : 마우스가 가장자리에 닿았는지 체크
		DxInputDevice::GetInstance().ScreenOverArray( bDefWin );

		return DxInputDevice::GetInstance().GetMouseMoveY();
	}
	else	return 0;
}