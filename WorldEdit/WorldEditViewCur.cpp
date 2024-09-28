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
extern D3DXVECTOR3			g_vCLOSED_POINT;			// Ŭ�� ���� �ø�.
extern BOOL					g_bSET_PIECE;			// ���� ���� �ɴ°� �����Ѱ� üũ.
extern D3DXVECTOR3			g_vDYNAMIC_POINT;		// ��� �����̴� ���콺 ������
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

		// Target Point �� ��ġ�� ī�޶��� �������� �ָ� ��������
		//		�浹 ������ �紁E?�� ��E�ִ�.
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
					// �浹 �˻縦 �Ͽ� �浹 �������� �����Ѵ�.
					char szCollTex[MAX_PATH] = {0};
					g_rSetLandMan->GetCollisionMap()->IsCollision_NoneConst ( vFromPt, vTargetPt, NULL, TRUE, NULL, szCollTex, EMCC_CULL_CCW );
					g_strPICK_TEXTURE = szCollTex;

                    g_rSetLandMan->GetPieceManager()->IsCollisionLine2( vFromPt, vTargetPt, vNor, g_vCLOSED_POINT, &g_pPIECE, EMCC_CULL_CCW );			// PieceManager Picking
					if( g_pPIECE )
					{
                        m_vCollisionPos = g_vCLOSED_POINT;
						DxViewPort::GetInstance().CameraJump( g_vCLOSED_POINT );

						//g_strPICK_FRAME = g_pPIECE->m_strName;

						//// ���õ� Piece�� ã�� ��������ش�.
						//CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
						//CDlgEdit &DlgEdit = pFrame->m_cDlgEdit;

						//DlgEdit.SelectPieceList( g_pPIECE );
					}

					// ���õ� �������� Ʈ����Ʈ�ѿ��� ã�� ����.
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

                    // Effect Page�� ���� Effect���� �˻��Ѵ�.
					if( m_ViewType == EFFECT_SET_VIEW )
					{
						POBJAABB pOBJ(NULL);
						if( g_rSetLandMan->IsCollisionLineEffect( vFromPt, vTargetPt, pOBJ ) )
						{
							g_vCLOSED_POINT = vTargetPt;
							DlgEdit.SelectSingleEffectList( dynamic_cast<DXLANDEFF*>(pOBJ) );
						}
					}

                    // TextureEffect�� â�� Ȱ��ȭ �Ǿ� �ִٸ� g_strPICK_TEXTURE �� �����ϵ��� �Ѵ�.
					{
						// ���õ� Piece�� ã�� ��������ش�.
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
					// �浹 �˻縦 �Ͽ� �浹 �������� �����Ѵ�.
					char szCollTex[MAX_PATH] = {0};
					m_DxPieceEdit.IsCollision_NoneConst( vFromPt, vTargetPt, TRUE, NULL, szCollTex );
					g_strPICK_TEXTURE = szCollTex;

					// ���õ� �������� Ʈ����Ʈ�ѿ��� ã�� ����.
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

					// TextureEffect�� â�� Ȱ��ȭ �Ǿ� �ִٸ� g_strPICK_TEXTURE �� �����ϵ��� �Ѵ�.
					{
						// ���õ� Piece�� ã�� ��������ش�.
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
					// �浹 �˻縦 �Ͽ� �浹 �������� �����Ѵ�.
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
                    // �浹 �˻縦 �Ͽ� �浹 �������� �����Ѵ�.
					char szCollTex[MAX_PATH] = {0};
					g_rSetLandMan->GetCollisionMap()->IsCollision_NoneConst ( vFromPt, vTargetPt, NULL, TRUE, NULL, szCollTex, EMCC_CULL_CCW );
					g_strPICK_TEXTURE = szCollTex;

					// ���õ� �������� Ʈ����Ʈ�ѿ��� ã�� ����.
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

					// Note : �ϴ� ������ �ɸ��� ������ �ǵ��� �Ѵ�.
					g_rSetLandMan->GetPieceManager()->IsCollisionLine2( vFromPt, vTargetPt, vNor, g_vCLOSED_POINT, &g_pPIECE, EMCC_CULL_CCW );	// PieceManager Picking
					if( g_pPIECE )
					{
						// ���õ� Piece�� ã�� ��������ش�.
						CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
						CDlgEdit &DlgEdit = pFrame->m_cDlgEdit;
						DlgEdit.SelectPieceList( g_pPIECE, static_cast<int>( _CUR_SELECT ) );

                        m_vCollisionPos = g_vCLOSED_POINT;
					}
				}
				break; 

            case PIECE_SETPOS_VIEW:
				{
                    // m_cDlgEditPIECE Ȱ��ȭ�� ���� �ؿ� �۾� �Ұ�����.
                    if ( pFrame->m_cDlgEditPIECE.m_bActive )
                    {
                        break;
                    }

                    CMainFrame* pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
                    
                    //// Focus �� ���� ����Ѵ�.
                    //if ( pFrame->m_cDlgEditPIECE.GetFocus() )
                    //{
                    //    POINT ptMouse;
	                   // ::GetCursorPos ( &ptMouse );

                    //    RECT listRect;
                    //    pFrame->m_cDlgEditPIECE.GetWindowRect(&listRect);
                    //    if ( ptMouse.x < listRect.right && ptMouse.x > listRect.left &&
                    //        ptMouse.y > listRect.top && ptMouse.y < listRect.bottom )
                    //    {
                    //        // Dialog �����Ѵٰ� �۾��� �Ǹ� �ȵȴ�.
                    //        break;
                    //    }
                    //}

					//	Note : �浹 �˻縦 �Ͽ� �浹 �������� �����Ѵ�.
                    BOOL bColl(FALSE);
					bColl = g_rSetLandMan->GetCollisionMap()->IsCollision_NoneConst ( vFromPt, vTargetPt, NULL, TRUE, NULL, szCollTex, EMCC_CULL_CCW );	// FrameMesh Picking
					g_strPICK_TEXTURE = szCollTex;

					//NSHfMain::IsCollision( vFromPt, vTargetPt, FALSE, FALSE );										// HeightField Picking
					g_vCLOSED_POINT = vTargetPt;

                    if ( DxPieceManagerPSF::g_emEDITMODE == DxPieceManagerPSF::MODE_INSERT )
                    {
                        // ����� ��ġ�� ã�´�.
                        DxPiece* pPiece(NULL);
                        g_rSetLandMan->GetPieceManager()->IsCollisionLine2( vFromPt, vTargetPt, vNor, g_vCLOSED_POINT, &pPiece, EMCC_CULL_CCW );	// PieceManager Picking

                        // �ƹ��͵� �� �ɸ��ٸ� �ٴ��� �浹ó�� �ϵ��� �Ѵ�.
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

                        // Note : Piece ����.
						DlgEdit.InsertPiece();
                        
						if( pFrame->m_cDlgEditPIECE.m_nRandom == 1)
							pFrame->m_cDlgEditPIECE.m_cDlgPiecePreView.SetRotateRandom();
                        else
                            pFrame->m_cDlgEditPIECE.m_cDlgPiecePreView.SetDefaultRandom();
                    }
					else
					{
						// Note : �ϴ� ������ �ɸ��� ������ �ǵ��� �Ѵ�.
						g_rSetLandMan->GetPieceManager()->IsCollisionLine2( vFromPt, vTargetPt, vNor, g_vCLOSED_POINT, &g_pPIECE, EMCC_CULL_CCW );	// PieceManager Picking
						if( g_pPIECE )
						{
							// ���õ� Piece�� ã�� ��������ش�.
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
						// ���õ� Piece�� ã�� ��������ش�.
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
                            // ���õ� Piece�� ã�� ��������ش�.
							CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
							CDlgEdit &DlgEdit = pFrame->m_cDlgEdit;

							DlgEdit.SelectPieceList( g_pPIECE, static_cast<int>( _CUR_SELECT_POINT ) );

                            g_pPIECE->GetClosedPoint( g_vCLOSED_POINT );
                        }
                    }
                    else
                    {
                        // �浹 �˻縦 �Ͽ� �浹 �������� �����Ѵ�.
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
		//CDebugSet::ToView ( 4, "���õ� ���ؽ� : %d,    %3.4f, %3.4f, %3.4f", dwIndex, vVert.x, vVert.y, vVert.z );
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
	//			ActorWorldLight �� Thread �󿡼� Ȯ���ϵ��� �����Ѵ�.
	// Thread ������ �ѱ��.
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

	//	Note : Target Point �� ��ġ�� ī�޶��� �������� �ָ� ��������
	//		�浹 ������ �紁E?�� ��E�ִ�.
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
					g_bSET_PIECE = FALSE;		// Piece ���� �Ұ���
				}

				// AABB �ڽ� �ٽ� ����ϱ� ���� Ȯ�εǾ��� ����.
				//DxPiece* pPiece_Prev = g_pDYNAMIC_PIECE;

				// Note : �ϴ� ������ �ɸ��� ������ �ǵ��� �Ѵ�.
                DxPiece* pPiece(NULL);
                if ( DxPieceManagerPSF::g_emEDITMODE == DxPieceManagerPSF::MODE_INSERT )
                {
                    g_rSetLandMan->GetPieceManager()->IsCollisionLine2( vFromPt, vTargetPt, vNor, g_vCLOSED_POINT, &pPiece, EMCC_CULL_CCW );	// PieceManager Picking
                }
                else
                {
                    // ���� ���
				    g_rSetLandMan->GetPieceManager()->IsCollisionLine2( vFromPt, vTargetPt, vNor, g_vCLOSED_POINT, &g_pDYNAMIC_PIECE, EMCC_CULL_CCW );	// PieceManager Picking
				    if( g_pDYNAMIC_PIECE )
				    {
					    break;
				    }
                }
				
				//// ������ ������ �Ͽ����ٸ� AABB �ڽ��� �ٽ� ����ؾ� �Ѵ�.
				//if( pPiece_Prev )
				//{
				//	GetPieceManager()->MakeTree();	// AABB �ڽ��� ���θ����� �浹ó���� �����ϴ�.
				//}

				//	Note : �浹 �˻縦 �Ͽ� �浹 �������� �����Ѵ�.
                BOOL bColl(FALSE);
				bColl = g_rSetLandMan->GetCollisionMap()->IsCollision_NoneConst ( vFromPt, vTargetPt, NULL, TRUE, NULL, szCollTex, EMCC_CULL_CCW );	// FrameMesh Picking

                // ������ ��ġ�� �����Ѵ�.
				LPDXFRAME const pDxFrame = g_rSetLandMan->GetCollisionMap()->GetCollisionDetectedFrame();
				if ( pDxFrame )
				{
					vTargetPt = g_rSetLandMan->GetCollisionMap()->GetCollisionPos ();
				}

                // �ƹ��͵� �� �ɸ��ٸ� �ٴ��� �浹ó�� �ϵ��� �Ѵ�.
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

				g_bSET_PIECE = TRUE;		// Piece ���� ����
			}
			break;

        case EFFECT_GRASS_VIEW:
            {
                // ���� ���
                DxPiece* pPiece(NULL);
			    g_rSetLandMan->GetPieceManager()->IsCollisionLine2( vFromPt, vTargetPt, vNor, g_vCLOSED_POINT, &g_pDYNAMIC_PIECE, EMCC_CULL_CCW );	// PieceManager Picking
			    if( g_pDYNAMIC_PIECE )
			    {
				    break;
			    }

                //	Note : �浹 �˻縦 �Ͽ� �浹 �������� �����Ѵ�.
                BOOL bColl(FALSE);
				bColl = g_rSetLandMan->GetCollisionMap()->IsCollision_NoneConst ( vFromPt, vTargetPt, NULL, TRUE, NULL, szCollTex, EMCC_CULL_CCW );	// FrameMesh Picking

                // ������ ��ġ�� �����Ѵ�.
				LPDXFRAME const pDxFrame = g_rSetLandMan->GetCollisionMap()->GetCollisionDetectedFrame();
				if ( pDxFrame )
				{
					vTargetPt = g_rSetLandMan->GetCollisionMap()->GetCollisionPos ();
				}

                // �ƹ��͵� �� �ɸ��ٸ� �ٴ��� �浹ó�� �ϵ��� �Ѵ�.
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

	// Note : Ŀ�� �̵����̶�� �̰Ŵ� �۵����� �ʴ´�.
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