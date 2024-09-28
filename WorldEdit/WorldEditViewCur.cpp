#include "stdafx.h"

#include "WorldEdit.h"
#include "../EngineLib/DxTools/DebugSet.h"
#include "../EngineLib/DxTools/DxInputDevice.h"
#include "../EngineLib/DxTools/DxViewPort.h"
#include "../EngineLib/DxTools/Collision.h"
#include "../EngineLib/DxTools/DxEditMat.h"
#include "../EngineLib/DxLand/DxPrefabMan.h"
#include "../EngineLib/DxMeshs/SkinMesh/NSSkinAniThread.h"
#include "../EngineLib/DxMeshs/SkinMesh/NSSkinAniThreadDef.h"

#include "../RanLogicClient/Char/GLCharacter.h"

#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"
#include "AssetDlg.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

extern DWORD				g_dwCLOSED_POINT_INDEX;
extern D3DXVECTOR3			g_vPICK_RAY_DIR;
extern D3DXVECTOR3			g_vPICK_RAY_ORIG;
extern TSTRING			    g_strPICK_TEXTURE;
extern std::string			g_strPICK_FRAME;
extern D3DXVECTOR3			g_vCLOSED_POINT;			// 클릭 했을 시만.
extern BOOL					g_bSET_PIECE;			// 조각 파일 심는거 가능한가 체크.
extern D3DXVECTOR3			g_vDYNAMIC_POINT;		// 계속 움직이는 마우스 포인터
extern D3DXVECTOR3			g_vCLOSED_POINT;

void CWorldEditView::CurSelect ()
{
	// Frame Picking
	//
	DWORD mKeyState = DxInputDevice::GetInstance().GetMouseState ( DXMOUSE_LEFT );

    TCHAR szCollTex[MAX_PATH] = {0};
    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CDlgEdit &DlgEdit = pFrame->m_cDlgEdit;

	D3DXVECTOR3 vNor(0.f,1.f,0.f);

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
			switch ( m_ViewType )
			{
			case FRAME_VIEW:
            case EFFECT_SET_VIEW:
            case EFFECT_GRASS_VIEW:
			case WATER_SURF_VIEW:
				{
					// 충돌 검사를 하여 충돌 프레임을 선택한다.
					char szCollTex[MAX_PATH] = {0};
					g_rSetLandMan->GetCollisionMap()->IsCollision_NoneConst ( vFromPt, vTargetPt, NULL, TRUE, NULL, szCollTex, EMCC_CULL_CCW );
					g_strPICK_TEXTURE = szCollTex;

                    g_rSetLandMan->GetPieceManager()->IsCollisionLine2( vFromPt, vTargetPt, vNor, g_vCLOSED_POINT, &g_pPIECE, EMCC_CULL_CCW );			// PieceManager Picking
					if( g_pPIECE )
					{
                        m_vCollisionPos = g_vCLOSED_POINT;
						DxViewPort::GetInstance().CameraJump( g_vCLOSED_POINT );

						//g_strPICK_FRAME = g_pPIECE->m_strName;

						//// 선택된 Piece를 찾아 연결시켜준다.
						//CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
						//CDlgEdit &DlgEdit = pFrame->m_cDlgEdit;

						//DlgEdit.SelectPieceList( g_pPIECE );
					}

					// 선택된 프래임을 트리콘트롤에서 찾아 선택.
					//
					LPDXFRAME const pDxFrame = g_rSetLandMan->GetCollisionMap()->GetCollisionDetectedFrame();
					if ( pDxFrame )
					{
						CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
						CDlgEdit &DlgEdit = pFrame->m_cDlgEdit;
                        CAssetDlg &DlgAsset = pFrame->m_cDlgAsset;

						DlgEdit.SelectFrameTree ( pDxFrame->szName );
                        DlgAsset.SelectFrame ( pDxFrame );                        

						m_vCollisionPos = g_rSetLandMan->GetCollisionMap()->GetCollisionPos ();
						g_vCLOSED_POINT = g_rSetLandMan->GetCollisionMap()->GetCollisionPos ();
						g_strPICK_FRAME = pDxFrame->szName;
					}
					else
					{
						g_strPICK_FRAME = "";
					}

					CDebugSet::ToView( 4, "%s", g_strPICK_TEXTURE.c_str() );
					CDebugSet::ToView( 5, "%s", g_strPICK_FRAME.c_str() );

                    // Effect Page일 경우는 Effect까지 검사한다.
					if( m_ViewType == EFFECT_SET_VIEW )
					{
						POBJAABB pOBJ(NULL);
						if( g_rSetLandMan->IsCollisionLineEffect( vFromPt, vTargetPt, pOBJ ) )
						{
							g_vCLOSED_POINT = vTargetPt;
							DlgEdit.SelectSingleEffectList( dynamic_cast<DXLANDEFF*>(pOBJ) );
						}
					}

                    // TextureEffect쪽 창이 활성화 되어 있다면 g_strPICK_TEXTURE 를 선택하도록 한다.
					{
						// 선택된 Piece를 찾아 연결시켜준다.
						CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
						CDlgEdit &DlgEdit = pFrame->m_cDlgEdit;
						DlgEdit.SelectTexEffList( g_strPICK_TEXTURE );
					}
				}
				break;

			case OCTREE_VIEW:
				{
					LPDXFRAME pDxFrame = NULL;
					BOOL bCollision = FALSE;
					D3DXVECTOR3 vCollision;
					//m_spDxLandMan->IsCollision ( vFromPt, vTargetPt, vCollision, bCollision, pDxFrame, TRUE );
					m_spDxLandMan->IsCollisionNEW( vFromPt, vTargetPt, vCollision, bCollision, EMCC_CULL_CCW, TRUE );
					
					if ( bCollision )
					{
						DxViewPort::GetInstance().CameraJump( vCollision );

						m_vCollisionPos = vCollision;
						g_vCLOSED_POINT = vCollision;
					}
				}
				break;
			case FRAMEMESH_VIEW:
				{
					// 충돌 검사를 하여 충돌 프레임을 선택한다.
					char szCollTex[MAX_PATH] = {0};
					m_DxPieceEdit.IsCollision_NoneConst( vFromPt, vTargetPt, TRUE, NULL, szCollTex );
					g_strPICK_TEXTURE = szCollTex;

					// 선택된 프래임을 트리콘트롤에서 찾아 선택.
					LPDXFRAME const pDxFrame = m_DxPieceEdit.GetCollisionDetectedFrame();
					if ( pDxFrame )
					{
						CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
						CDlgEdit &DlgEdit = pFrame->m_cDlgEdit;
						DlgEdit.SelectPieceFrameTree( pDxFrame->szName );

						m_vCollisionPos = m_DxPieceEdit.GetCollisionPos ();
						g_vCLOSED_POINT = m_DxPieceEdit.GetCollisionPos ();
						g_strPICK_FRAME = pDxFrame->szName;
					}
					else
					{
						g_strPICK_FRAME = "";
					}

					CDebugSet::ToView( 4, "%s", g_strPICK_TEXTURE.c_str() );
					CDebugSet::ToView( 5, "%s", g_strPICK_FRAME.c_str() );

					// TextureEffect쪽 창이 활성화 되어 있다면 g_strPICK_TEXTURE 를 선택하도록 한다.
					{
						// 선택된 Piece를 찾아 연결시켜준다.
						CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
						CDlgEdit &DlgEdit = pFrame->m_cDlgEdit;
						DlgEdit.SelectTexEffList( g_strPICK_TEXTURE );
					}
				}
				break;
			case STATICMESH_VIEW:
				break;

			case CAMCOLL_VIEW:
				{
					// 충돌 검사를 하여 충돌 프레임을 선택한다.
					char szCollTex[MAX_PATH] = {0};
					BOOL bCollision = g_rSetLandMan->GetCollisionMap()->IsCollision_NoneConst ( vFromPt, vTargetPt, NULL, TRUE, NULL, szCollTex, EMCC_CULL_CCW );

					if ( !bCollision )
					{
						return;
					}

					LPDXFRAME const pDxFrame = g_rSetLandMan->GetCollisionMap()->GetCollisionDetectedFrame();
					if ( !pDxFrame || !pDxFrame->pmsMeshs )	return;
				
					for( DWORD i=0; i<pDxFrame->pmsMeshs->m_cMaterials; ++i )
					{
						if ( strcmp(szCollTex, pDxFrame->pmsMeshs->m_strTextureFiles[i]) == 0 )
						{
							CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
							CDlgEdit &DlgEdit = pFrame->m_cDlgEdit;

							DlgEdit.SelectCamCollList ( szCollTex );

							break;
						}
					}
				}
				break;

            case PIECE_REPLACE_VIEW:
				{
                    // 충돌 검사를 하여 충돌 프레임을 선택한다.
					char szCollTex[MAX_PATH] = {0};
					g_rSetLandMan->GetCollisionMap()->IsCollision_NoneConst ( vFromPt, vTargetPt, NULL, TRUE, NULL, szCollTex, EMCC_CULL_CCW );
					g_strPICK_TEXTURE = szCollTex;

					// 선택된 프래임을 트리콘트롤에서 찾아 선택.
					//
					LPDXFRAME const pDxFrame = g_rSetLandMan->GetCollisionMap()->GetCollisionDetectedFrame();
					if ( pDxFrame )
					{
						m_vCollisionPos = g_rSetLandMan->GetCollisionMap()->GetCollisionPos ();
						g_vCLOSED_POINT = g_rSetLandMan->GetCollisionMap()->GetCollisionPos ();
					}
					else
					{
						g_strPICK_FRAME = "";
					}

					CDebugSet::ToView( 4, "%s", g_strPICK_TEXTURE.c_str() );
					CDebugSet::ToView( 5, "%s", g_strPICK_FRAME.c_str() );

					// Note : 일단 조각이 걸리면 선택이 되도록 한다.
					g_rSetLandMan->GetPieceManager()->IsCollisionLine2( vFromPt, vTargetPt, vNor, g_vCLOSED_POINT, &g_pPIECE, EMCC_CULL_CCW );	// PieceManager Picking
					if( g_pPIECE )
					{
						// 선택된 Piece를 찾아 연결시켜준다.
						CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
						CDlgEdit &DlgEdit = pFrame->m_cDlgEdit;
						DlgEdit.SelectPieceList( g_pPIECE, static_cast<int>( _CUR_SELECT ) );

                        m_vCollisionPos = g_vCLOSED_POINT;
					}
				}
				break; 

            case PIECE_SETPOS_VIEW:
				{
                    // m_cDlgEditPIECE 활성화일 경우는 밑에 작업 불가능함.
                    if ( pFrame->m_cDlgEditPIECE.m_bActive )
                    {
                        break;
                    }

                    CMainFrame* pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
                    
                    //// Focus 될 때만 사용한다.
                    //if ( pFrame->m_cDlgEditPIECE.GetFocus() )
                    //{
                    //    POINT ptMouse;
	                   // ::GetCursorPos ( &ptMouse );

                    //    RECT listRect;
                    //    pFrame->m_cDlgEditPIECE.GetWindowRect(&listRect);
                    //    if ( ptMouse.x < listRect.right && ptMouse.x > listRect.left &&
                    //        ptMouse.y > listRect.top && ptMouse.y < listRect.bottom )
                    //    {
                    //        // Dialog 선택한다고 작업이 되면 안된다.
                    //        break;
                    //    }
                    //}

					//	Note : 충돌 검사를 하여 충돌 프레임을 선택한다.
                    BOOL bColl(FALSE);
					bColl = g_rSetLandMan->GetCollisionMap()->IsCollision_NoneConst ( vFromPt, vTargetPt, NULL, TRUE, NULL, szCollTex, EMCC_CULL_CCW );	// FrameMesh Picking
					g_strPICK_TEXTURE = szCollTex;

					//NSHfMain::IsCollision( vFromPt, vTargetPt, FALSE, FALSE );										// HeightField Picking
					g_vCLOSED_POINT = vTargetPt;

                    if ( DxPieceManagerPSF::g_emEDITMODE == DxPieceManagerPSF::MODE_INSERT )
                    {
                        // 가까운 위치를 찾는다.
                        DxPiece* pPiece(NULL);
                        g_rSetLandMan->GetPieceManager()->IsCollisionLine2( vFromPt, vTargetPt, vNor, g_vCLOSED_POINT, &pPiece, EMCC_CULL_CCW );	// PieceManager Picking

                        // 아무것도 안 걸린다면 바닥을 충돌처리 하도록 한다.
                        if ( !bColl && !pPiece )
                        {
                            D3DXVECTOR3 vPos[3];
                            vPos[0] = D3DXVECTOR3( -10000.f, 0.f, -10000.f );
                            vPos[2] = D3DXVECTOR3( 10000.f, 0.f, -10000.f );
                            vPos[1] = D3DXVECTOR3( -10000.f, 0.f, 10000.f );
                            COLLISION::IsLineTriangleCollision( &vPos[0], &vPos[1], &vPos[2], &vFromPt, &vTargetPt, &g_vCLOSED_POINT, NULL, EMCC_CULL_NONE, TRUE );

                            vPos[0] = D3DXVECTOR3( -10000.f, 0.f, 10000.f );
                            vPos[2] = D3DXVECTOR3( 10000.f, 0.f, -10000.f );
                            vPos[1] = D3DXVECTOR3( 10000.f, 0.f, 10000.f );
                            COLLISION::IsLineTriangleCollision( &vPos[0], &vPos[1], &vPos[2], &vFromPt, &vTargetPt, &g_vCLOSED_POINT, NULL, EMCC_CULL_NONE, TRUE );
                        }

                        // Note : Piece 삽입.
						DlgEdit.InsertPiece();
                        
						if( pFrame->m_cDlgEditPIECE.m_nRandom == 1)
							pFrame->m_cDlgEditPIECE.m_cDlgPiecePreView.SetRotateRandom();
                        else
                            pFrame->m_cDlgEditPIECE.m_cDlgPiecePreView.SetDefaultRandom();
                    }
					else
					{
						// Note : 일단 조각이 걸리면 선택이 되도록 한다.
						g_rSetLandMan->GetPieceManager()->IsCollisionLine2( vFromPt, vTargetPt, vNor, g_vCLOSED_POINT, &g_pPIECE, EMCC_CULL_CCW );	// PieceManager Picking
						if( g_pPIECE )
						{
							// 선택된 Piece를 찾아 연결시켜준다.
							CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
							CDlgEdit &DlgEdit = pFrame->m_cDlgEdit;

							DlgEdit.SelectPieceList( g_pPIECE, static_cast<int>( _CUR_SELECT ) );

							break;
						}
					}
				}
				break; 

			case PREFAB_VIEW:
				{
					//	Note : Prefab Picking
					g_rSetLandMan->GetPrefabMan()->IsCollision ( vFromPt, vTargetPt, &g_pPrefab );
					if( g_pPrefab )
					{
						// 선택된 Piece를 찾아 연결시켜준다.
						CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
						CDlgEdit &DlgEdit = pFrame->m_cDlgEdit;

						DlgEdit.SelectPrefabList( g_pPrefab );

						break;
					}
				}
				break;
			};
		}
	}

	m_p3dPosMesh->FrameMove( m_fTime, m_fElapsedTime );
}

void CWorldEditView::CurMove ()
{
	m_pDxEditmat->FrameMove ( m_fTime, m_fElapsedTime );
}

void CWorldEditView::CurSelectPoint ()
{
	DWORD mKeyState = DxInputDevice::GetInstance().GetMouseState ( DXMOUSE_LEFT );
	if ( mKeyState&DXKEY_UP )
	{
		D3DXVECTOR3 vTargetPt, vFromPt;
		vFromPt = DxViewPort::GetInstance().GetFromPt ();
		BOOL bTargetted = DxViewPort::GetInstance().GetMouseTargetPosWnd ( vTargetPt );

		if ( bTargetted )
		{
			switch ( m_ViewType )
			{
			case FRAME_VIEW:
            case EFFECT_SET_VIEW:
				{
					if ( !CurSelectPoint_Frame( vFromPt, vTargetPt ) )
                        return;
				}
				break;

			case FRAMEMESH_VIEW:
				{
					LPDXFRAME const pDxFrame = m_DxPieceEdit.GetCollisionDetectedFrame();
					if ( !pDxFrame || !pDxFrame->pmsMeshs )	return;

					BOOL bCollision;
					char szCollTex[MAX_PATH] = {0};
					D3DXVECTOR3 vCollision = vTargetPt;
					//COLLISION::vColTestStart = vFromPt;
					//COLLISION::vColTestEnd = vTargetPt;
					pDxFrame->pmsMeshs->IsCollision_DxMeshes( pDxFrame->matCombined, vFromPt, vTargetPt, NULL, vCollision, bCollision, szCollTex, EMCC_CULL_CCW );
					if ( bCollision )
					{
						m_vCollisionPos = vCollision;

						DWORD dwIndex;
						D3DXVECTOR3 vVert;
						pDxFrame->pmsMeshs->GetClosedPointIndex ( pDxFrame->matCombined, m_vCollisionPos, dwIndex, vVert );

						g_dwCLOSED_POINT_INDEX = dwIndex;
						g_vCLOSED_POINT = vVert;

						g_strPICK_TEXTURE = szCollTex;
					}
				}
				break;

            case PIECE_SETPOS_VIEW:
                {
					D3DXVECTOR3 vNor(0.f,1.f,0.f);
                    D3DXVECTOR3 vCollision = vTargetPt;
                    if ( g_rSetLandMan->GetPieceManager()->IsCollisionLine2( vFromPt, vTargetPt, vNor, vCollision, &g_pPIECE, EMCC_CULL_CCW ) )
                    {
                        g_vCLOSED_POINT = vCollision;

                        if ( g_pPIECE )
                        {
                            // 선택된 Piece를 찾아 연결시켜준다.
							CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
							CDlgEdit &DlgEdit = pFrame->m_cDlgEdit;

							DlgEdit.SelectPieceList( g_pPIECE, static_cast<int>( _CUR_SELECT_POINT ) );

                            g_pPIECE->GetClosedPoint( g_vCLOSED_POINT );
                        }
                    }
                    else
                    {
                        // 충돌 검사를 하여 충돌 프레임을 선택한다.
					    char szCollTex[MAX_PATH] = {0};
					    g_rSetLandMan->GetCollisionMap()->IsCollision_NoneConst ( vFromPt, vTargetPt, NULL, TRUE, NULL, szCollTex, EMCC_CULL_CCW );

                        if ( !CurSelectPoint_Frame( vFromPt, vTargetPt ) )
                            return;
                    }
                }
                break;
			};
		}
	}
}

BOOL CWorldEditView::CurSelectPoint_Frame( const D3DXVECTOR3& vFromPt, D3DXVECTOR3& vTargetPt )
{
    LPDXFRAME const pDxFrame = g_rSetLandMan->GetCollisionMap()->GetCollisionDetectedFrame();
	if ( !pDxFrame || !pDxFrame->pmsMeshs )	
        return FALSE;

	BOOL bCollision;
	char szCollTex[MAX_PATH] = {0};
	D3DXVECTOR3 vCollision = vTargetPt;
	//COLLISION::vColTestStart = vFromPt;
	//COLLISION::vColTestEnd = vTargetPt;
	pDxFrame->pmsMeshs->IsCollision_DxMeshes( pDxFrame->matCombined, vFromPt, vTargetPt, NULL, vCollision, bCollision, szCollTex, EMCC_CULL_CCW );
	if ( bCollision )
	{
		m_vCollisionPos = vCollision;

		DWORD dwIndex;
		D3DXVECTOR3 vVert;
		pDxFrame->pmsMeshs->GetClosedPointIndex ( pDxFrame->matCombined, m_vCollisionPos, dwIndex, vVert );

		g_dwCLOSED_POINT_INDEX = dwIndex;
		g_vCLOSED_POINT = vVert;

		g_strPICK_TEXTURE = szCollTex;
		//CDebugSet::ToView ( 4, "선택된 버텍스 : %d,    %3.4f, %3.4f, %3.4f", dwIndex, vVert.x, vVert.y, vVert.z );
	}

    return TRUE;
}


void	CWorldEditView::CurCharMove ()
{
	// Frame Picking
	//
	m_pCharacter->FrameMove ( m_fTime, m_fElapsedTime );

	D3DXVECTOR3 vCamera = m_pCharacter->GetPosition();
	vCamera.y += 10.0f;
	DxViewPort::GetInstance().CameraJump ( vCamera );


	//////////////////////////////////////////////////////////////////////////
	//			ActorWorldLight 을 Thread 상에서 확인하도록 유도한다.
	// Thread 상으로 넘긴다.
	m_pCharacter->GetWorldLightForTool()->InitData( m_spDxLandMan );
	NSSkinAniThread::AddActorWorldLight( m_pCharacter->GetWorldLightForTool(), m_pCharacter->GetPosition() );
}

//-----------------------------------------------------------------------------
// Name: Pick()
// Desc: Checks if mouse point hits geometry
//       the scene.
//-----------------------------------------------------------------------------
BOOL CWorldEditView::Pick()
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

    // Compute the vector of the pick ray in screen space
    D3DXVECTOR3 v;
	v.x =  ( ( ( 2.0f * ptCursor.x ) / m_d3dsdBackBuffer.Width  ) - 1 ) / matProj._11;
    v.y = -( ( ( 2.0f * ptCursor.y ) / m_d3dsdBackBuffer.Height ) - 1 ) / matProj._22;
    v.z =  1.0f;

    // Get the inverse view matrix
    D3DXMATRIX matView, m;
    m_pd3dDevice->GetTransform( D3DTS_VIEW, &matView );
    D3DXMatrixInverse( &m, NULL, &matView );

    // Transform the screen space pick ray into 3D space
    g_vPICK_RAY_DIR.x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
    g_vPICK_RAY_DIR.y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
    g_vPICK_RAY_DIR.z  = v.x*m._13 + v.y*m._23 + v.z*m._33;
    g_vPICK_RAY_ORIG.x = m._41;
    g_vPICK_RAY_ORIG.y = m._42;
    g_vPICK_RAY_ORIG.z = m._43;

	D3DXVec3Normalize ( &g_vPICK_RAY_DIR, &g_vPICK_RAY_DIR );		// Normalize

	return TRUE;
}

void CWorldEditView::CurPoint_Dynamic()
{
	D3DXVECTOR3 vTargetPt, vFromPt;
	vFromPt = DxViewPort::GetInstance().GetFromPt ();
	BOOL bTargetted = DxViewPort::GetInstance().GetMouseTargetPosWnd ( vTargetPt );

	//	Note : Target Point 의 위치를 카메라의 방향으로 멀리 보내엉雹
	//		충돌 검출을 재큱E?할 펯E있다.
	//
	int nIndex(0);
	D3DXVECTOR3 vDirect = vFromPt - vTargetPt;
	D3DXVec3Normalize ( &vDirect, &vDirect );
	vTargetPt = vFromPt - vDirect * 100000.0f;

	D3DXVECTOR3 vNor(0.f,1.f,0.f);
	if ( bTargetted )
	{
		TCHAR szCollTex[MAX_PATH] = {0};
		LPDXFRAME pDxFrame(NULL);

		switch ( m_ViewType )
		{
		case PIECE_SETPOS_VIEW:
			{
				if( DxPieceManagerPSF::g_emEDITMODE == DxPieceManagerPSF::MODE_EDIT )
				{
					g_bSET_PIECE = FALSE;		// Piece 셋팅 불가능
				}

				// AABB 박스 다시 계산하기 위해 확인되야할 정보.
				//DxPiece* pPiece_Prev = g_pDYNAMIC_PIECE;

				// Note : 일단 조각이 걸리면 선택이 되도록 한다.
                DxPiece* pPiece(NULL);
                if ( DxPieceManagerPSF::g_emEDITMODE == DxPieceManagerPSF::MODE_INSERT )
                {
                    g_rSetLandMan->GetPieceManager()->IsCollisionLine2( vFromPt, vTargetPt, vNor, g_vCLOSED_POINT, &pPiece, EMCC_CULL_CCW );	// PieceManager Picking
                }
                else
                {
                    // 선택 모드
				    g_rSetLandMan->GetPieceManager()->IsCollisionLine2( vFromPt, vTargetPt, vNor, g_vCLOSED_POINT, &g_pDYNAMIC_PIECE, EMCC_CULL_CCW );	// PieceManager Picking
				    if( g_pDYNAMIC_PIECE )
				    {
					    break;
				    }
                }
				
				//// 이전에 선택을 하였었다면 AABB 박스를 다시 계산해야 한다.
				//if( pPiece_Prev )
				//{
				//	GetPieceManager()->MakeTree();	// AABB 박스를 새로만들어야 충돌처리가 가능하다.
				//}

				//	Note : 충돌 검사를 하여 충돌 프레임을 선택한다.
                BOOL bColl(FALSE);
				bColl = g_rSetLandMan->GetCollisionMap()->IsCollision_NoneConst ( vFromPt, vTargetPt, NULL, TRUE, NULL, szCollTex, EMCC_CULL_CCW );	// FrameMesh Picking

                // 선택한 위치를 셋팅한다.
				LPDXFRAME const pDxFrame = g_rSetLandMan->GetCollisionMap()->GetCollisionDetectedFrame();
				if ( pDxFrame )
				{
					vTargetPt = g_rSetLandMan->GetCollisionMap()->GetCollisionPos ();
				}

                // 아무것도 안 걸린다면 바닥을 충돌처리 하도록 한다.
                if ( !bColl && !pPiece )
                {
                    D3DXVECTOR3 vPos[3];
                    vPos[0] = D3DXVECTOR3( -10000.f, 0.f, -10000.f );
                    vPos[1] = D3DXVECTOR3( 10000.f, 0.f, -10000.f );
                    vPos[2] = D3DXVECTOR3( -10000.f, 0.f, 10000.f );
                    if ( COLLISION::IsLineTriangleCollision( &vPos[0], &vPos[1], &vPos[2], &vFromPt, &vTargetPt, &g_vCLOSED_POINT, NULL, EMCC_CULL_NONE, TRUE ) )
                    {
                        vTargetPt = g_vCLOSED_POINT;
                    }

                    vPos[0] = D3DXVECTOR3( -10000.f, 0.f, 10000.f );
                    vPos[1] = D3DXVECTOR3( 10000.f, 0.f, -10000.f );
                    vPos[2] = D3DXVECTOR3( 10000.f, 0.f, 10000.f );
                    if ( COLLISION::IsLineTriangleCollision( &vPos[0], &vPos[1], &vPos[2], &vFromPt, &vTargetPt, &g_vCLOSED_POINT, NULL, EMCC_CULL_NONE, TRUE ) )
                    {
                        vTargetPt = g_vCLOSED_POINT;
                    }
                }

				g_vDYNAMIC_POINT = vTargetPt;

				g_bSET_PIECE = TRUE;		// Piece 셋팅 가능
			}
			break;

        case EFFECT_GRASS_VIEW:
            {
                // 선택 모드
                DxPiece* pPiece(NULL);
			    g_rSetLandMan->GetPieceManager()->IsCollisionLine2( vFromPt, vTargetPt, vNor, g_vCLOSED_POINT, &g_pDYNAMIC_PIECE, EMCC_CULL_CCW );	// PieceManager Picking
			    if( g_pDYNAMIC_PIECE )
			    {
				    break;
			    }

                //	Note : 충돌 검사를 하여 충돌 프레임을 선택한다.
                BOOL bColl(FALSE);
				bColl = g_rSetLandMan->GetCollisionMap()->IsCollision_NoneConst ( vFromPt, vTargetPt, NULL, TRUE, NULL, szCollTex, EMCC_CULL_CCW );	// FrameMesh Picking

                // 선택한 위치를 셋팅한다.
				LPDXFRAME const pDxFrame = g_rSetLandMan->GetCollisionMap()->GetCollisionDetectedFrame();
				if ( pDxFrame )
				{
					vTargetPt = g_rSetLandMan->GetCollisionMap()->GetCollisionPos ();
				}

                // 아무것도 안 걸린다면 바닥을 충돌처리 하도록 한다.
                if ( !bColl && !pPiece )
                {
                    D3DXVECTOR3 vPos[3];
                    vPos[0] = D3DXVECTOR3( -10000.f, 0.f, -10000.f );
                    vPos[1] = D3DXVECTOR3( 10000.f, 0.f, -10000.f );
                    vPos[2] = D3DXVECTOR3( -10000.f, 0.f, 10000.f );
                    if ( COLLISION::IsLineTriangleCollision( &vPos[0], &vPos[1], &vPos[2], &vFromPt, &vTargetPt, &g_vCLOSED_POINT, NULL, EMCC_CULL_NONE, TRUE ) )
                    {
                        vTargetPt = g_vCLOSED_POINT;
                    }

                    vPos[0] = D3DXVECTOR3( -10000.f, 0.f, 10000.f );
                    vPos[1] = D3DXVECTOR3( 10000.f, 0.f, -10000.f );
                    vPos[2] = D3DXVECTOR3( 10000.f, 0.f, 10000.f );
                    if ( COLLISION::IsLineTriangleCollision( &vPos[0], &vPos[1], &vPos[2], &vFromPt, &vTargetPt, &g_vCLOSED_POINT, NULL, EMCC_CULL_NONE, TRUE ) )
                    {
                        vTargetPt = g_vCLOSED_POINT;
                    }
                }

				g_vDYNAMIC_POINT = vTargetPt;
            }
            break;
		};
	};	
}

void CWorldEditView::FrameMoveCurNext( float fElapsedTime )
{
	CMainFrame *pMainFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;

	// Note : 커서 이동중이라면 이거는 작동되지 않는다.
	if( !m_sObjectMRS.IsCollMode() )	return;

	switch ( m_CurType )
	{
	case _CUR_SELECT:
		CurSelect();
		CurPoint_Dynamic();

		pMainFrame->m_cDlgEdit.UpdatePage_Always();

		break;

	//case _CUR_SELECT_POINT:
	//	CurSelectPoint ();
	//	break;

	//case _CUR_CHAR_MOVE:		
	//	CurCharMove ( fElapsedTime );
	//	break;

	//case _CUR_EDIT_BOX:
	//	m_EditBox.FrameMove ( GetD3DDevice(), m_fTime, fElapsedTime );
	//	break;
	};
}