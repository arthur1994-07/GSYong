// FramePage.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "WorldEdit.h"
#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"

#include "../MfcExLib/ExLibs/FECFileDialog.h"

#include "../EngineLib/Common/SUBPATH.H"
#include "../EngineLib/DxLand/DxPiece/DxPieceEdit.h"
#include "../EngineLib/DxLand/DxPiece/DxPieceManagerPSF.h"
#include "../EngineLib/DxLand/DxPiece/DxPieceContainer.h"
#include "../EngineLib/DxMeshs/FrameMesh/DxFrameMesh.h"
#include "../EngineLib/DxTools/Material/NSMaterialManager.h"
#include "../EngineLib/DxTools/DxViewPort.h"
#include "../EngineLib/DxEffect/Frame/DxEffectFrame.h"
#include "../EngineLib/DxEffect/Frame/DxEffectMan.h"

#include "../EngineLib/DxEffect/Frame/DxEffectTiling.h"
#include "../EngineLib/DxEffect/Frame/DxEffectCloth.h"
#include "../EngineLib/DxEffect/Frame/DxEffectDot3.h"
#include "../EngineLib/DxEffect/Frame/DxEffectRiver.h"
#include "../EngineLib/DxEffect/Frame/DxEffectShadow.h"
#include "../EngineLib/DxEffect/Frame/DxEffectFur.h"
#include "../EngineLib/DxEffect/Frame/DxEffectRainDrop.h"
#include "../EngineLib/DxEffect/Frame/DxEffectGrass.h"
#include "../EngineLib/DxEffect/Frame/DxEffectNature.h"
#include "../EngineLib/DxEffect/Frame/DxEffectWaterLight.h"
#include "../EngineLib/DxEffect/Frame/DxEffectRenderState.h"
#include "../EngineLib/DxEffect/Frame/DxEffectSpore.h"
#include "../EngineLib/DxEffect/Frame/DxEffectSpecular.h"
#include "../EngineLib/DxEffect/Frame/DxEffectSpecular2.h"
#include "../EngineLib/DxEffect/Frame/DxEffectReflect.h"
#include "../EngineLib/DxEffect/Frame/DxEffectMultiTex.h"
#include "../EngineLib/DxEffect/Frame/DxEffectNeon.h"
#include "../EngineLib/DxEffect/Frame/DxEffectSpecReflect.h"
#include "../EngineLib/DxEffect/Frame/DxEffectLightMap.h"
#include "../EngineLib/DxEffect/Frame/DxEffectGlow.h"

#include "../EngineLib/DxEffect/TexEff/DxTextureEffMan.h"

#include "./PiecePage/TexPieceVMViewer.h"
#include "TextureSetDlg.h"
#include "SheetWithTab.h"
#include "State/WorldStateMachine.h"

#include "FramePage.h"
#include ".\framepage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CFramePage 대화 상자입니다.

static std::set<TSTRING> g_setName;

IMPLEMENT_DYNAMIC(CFramePage, CPropertyPage)
CFramePage::CFramePage()
	: CPropertyPage(CFramePage::IDD),
	m_bDlgInit(FALSE)
{
}

CFramePage::~CFramePage()
{
}

void CFramePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_FRAME, m_TreeFrame);
	DDX_Control(pDX, IDC_COMBO_EFFECT, m_ComboEffectTypes);
	DDX_Control(pDX, IDC_LIST_EFFECT, m_ListBoxEffect);
}


BEGIN_MESSAGE_MAP(CFramePage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_XFILEOPEN, OnBnClickedButtonXfileopen)
	ON_NOTIFY(NM_RCLICK, IDC_TREE_FRAME, OnNMRclickTreeFrame)
	ON_BN_CLICKED(IDC_BUTTON_EFFECTCHANGEADD, OnBnClickedButtonEffectchangeadd)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_FRAME, OnTvnSelchangedTreeFrame)
	ON_BN_CLICKED(IDC_BUTTON_EFFECTSTATE, OnBnClickedButtonEffectstate)
	ON_BN_CLICKED(IDC_BUTTON_EFFECTDELETE, OnBnClickedButtonEffectdelete)
	ON_BN_CLICKED(IDC_BUTTON_FRAMESTATE, OnBnClickedButtonFramestate)
	ON_BN_CLICKED(IDC_BUTTON_REPLACE_SET, OnBnClickedButtonReplaceSet)
	ON_BN_CLICKED(IDC_EDIT_MAPMAINID, OnEnChangeEditMapmainid)
	ON_EN_CHANGE(IDC_EDIT_MAPSUBID, OnEnChangeEditMapsubid)
	ON_BN_CLICKED(IDC_CHECK_NAVIVIEW, OnBnClickedCheckNaviview)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE_SETTING, OnBnClickedButtonTextureSetting)
	ON_BN_CLICKED(IDC_BUTTON_TEXEFF, OnBnClickedButtonTexeff)
	ON_BN_CLICKED(IDC_SNOW_CHECK, OnBnClickedSnowCheck)
	ON_BN_CLICKED(IDC_RAIN_CHECK, OnBnClickedRainCheck)
	ON_BN_CLICKED(IDC_LEAVE_CHECK, OnBnClickedLeaveCheck)
	ON_BN_CLICKED(IDC_CHECK_CAMCOLL, OnBnClickedCheckCamcoll)
    ON_BN_CLICKED(IDC_BUTTON_PIECE_TEX_VM_VIEW, &CFramePage::OnBnClickedButtonPieceTexVmView)
	ON_EN_CHANGE(IDC_EDIT_LIGHTMAP_DETAIL, &CFramePage::OnEnChangeEditLightmapDetail)
	ON_NOTIFY(NM_CLICK, IDC_TREE_FRAME, &CFramePage::OnNMClickTreeFrame)
	ON_BN_CLICKED(IDC_BUTTON_MINIMAP, &CFramePage::OnBnClickedButtonMinimap)
	ON_BN_CLICKED(IDC_BUTTON_OCCLUSION_CULLING, &CFramePage::OnBnClickedButtonOcclusionCulling)
	ON_BN_CLICKED(IDC_BUTTON_CUBE_MAP, &CFramePage::OnBnClickedButtonCubeMap)
END_MESSAGE_MAP()

BOOL CFramePage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	CMainFrame* pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	if( pFrame )
	{
		CWorldEditView* pView = (CWorldEditView*) pFrame->GetActiveView();
		if( pView )		pView->m_ViewType = FRAME_VIEW;
	}

	CButton *pButton = (CButton *) GetDlgItem ( IDC_CHECK_NAVIVIEW );
	pButton->SetCheck ( DxFrameMesh::m_bNaviDraw );

	CWnd *pWnd;

	pWnd = GetDlgItem ( IDC_EDIT_MAPMAINID );
	pWnd->SetWindowText ( "0" );

	pWnd = GetDlgItem ( IDC_EDIT_MAPSUBID );
	pWnd->SetWindowText ( "0" );

	//	Note : 효과 타입들을 등록.
	//
	//
	DxEffectType *pCurEffectType = DxEffectMan::GetInstance().GetEffectList();
	while ( pCurEffectType )
	{
		int nIndex = m_ComboEffectTypes.AddString ( pCurEffectType->NAME );
		m_ComboEffectTypes.SetItemData ( nIndex, pCurEffectType->TYPEID );

		pCurEffectType = pCurEffectType->pNextType;
	}

	m_bDlgInit = TRUE;

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BOOL CFramePage::OnSetActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CMainFrame* pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	if( pFrame )
	{
		CWorldEditView* pView = (CWorldEditView*) pFrame->GetActiveView();
		if( pView )		pView->m_ViewType = FRAME_VIEW;
	}

	char szValue[256];
	CWnd* pWnd = GetDlgItem( IDC_EDIT_LIGHTMAP_DETAIL );
	_gcvt ( NSMaterialManager::GetEditLightMapPower(), 7, szValue );
	pWnd->SetWindowText( szValue );

	SetLightMapXY();

	return CPropertyPage::OnSetActive();
}

// CFramePage 메시지 처리기입니다.
void CFramePage::AddTreeFrameItem ( DxFrame *pframeCur, HTREEITEM hParent )
{
	//	Note : Frame 의 이름이 존재하는 노드만을 등록한다.
	//
	HTREEITEM htreeitemCur;
	if ( pframeCur->szName )
	{
		htreeitemCur = m_TreeFrame.InsertItem ( pframeCur->szName, hParent );
		m_TreeFrame.SetItemData ( htreeitemCur, (DWORD)pframeCur );

        TSTRING strName = pframeCur->szName;
        if ( !strName.empty() )
        {
            std::set<TSTRING>::iterator iter = g_setName.find( strName.c_str() );
            if ( iter!=g_setName.end() )
            {
                strName += _T(" - Frame Name 이 중복되었습니다. [ERROR] \r\n max 에서 중복된 이름을 바꿔주십시오.");
                AfxMessageBox( strName.c_str() );
            }
            else
            {
                g_setName.insert( pframeCur->szName );
            }
        }

		//	Note : 자식들을 재귀적으로 모두 탐색.
		//
		DxFrame *pframeChild = pframeCur->pframeFirstChild;
		while (pframeChild != NULL)
		{
			AddTreeFrameItem ( pframeChild, htreeitemCur );
			pframeChild = pframeChild->pframeSibling;
		}
	}
}

void CFramePage::SetTreeFrame ( DxFrame *pFrameRoot )
{
    g_setName.clear();

	m_TreeFrame.DeleteAllItems();

	HTREEITEM hParent = TVI_ROOT;

	if ( pFrameRoot )
		AddTreeFrameItem ( pFrameRoot, hParent );
}

void CFramePage::ListEffects ( DxFrame *pDxFrame )
{
	if ( !pDxFrame )	return;

	m_ListBoxEffect.ResetContent ();

	DxEffectBase *pEffectCur;
	//	Note : 전, 이팩트
	//
	pEffectCur = pDxFrame->pEffectPrev;
	while ( pEffectCur )
	{
		int nIndex = m_ListBoxEffect.AddString ( pEffectCur->GetName() );
		m_ListBoxEffect.SetItemData ( nIndex, (DWORD) pEffectCur );

		pEffectCur = pEffectCur->pLocalFrameEffNext;
	}

	//	Note : 중, 이팩트
	//
	pEffectCur = pDxFrame->pEffect;
	while ( pEffectCur )
	{
		int nIndex = m_ListBoxEffect.AddString ( pEffectCur->GetName() );
		m_ListBoxEffect.SetItemData ( nIndex, (DWORD) pEffectCur );

		pEffectCur = pEffectCur->pLocalFrameEffNext;
	}

	//	Note : 후, 이팩트
	//
	pEffectCur = pDxFrame->pEffectNext;
	while ( pEffectCur )
	{
		int nIndex = m_ListBoxEffect.AddString ( pEffectCur->GetName() );
		m_ListBoxEffect.SetItemData ( nIndex, (DWORD) pEffectCur );

		pEffectCur = pEffectCur->pLocalFrameEffNext;
	}
}

void CFramePage::SelectChangedTreeFrame ()
{
	HTREEITEM htreeitemCur = m_TreeFrame.GetSelectedItem();
	if ( htreeitemCur == NULL )	return;

	CString sSelectedFrame = m_TreeFrame.GetItemText ( htreeitemCur );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	DxFrameMesh *pFrameMesh = pView->GetFrameMesh();

	DxFrame *pDxFrame = pFrameMesh->FindFrame ( sSelectedFrame.GetString() );

	//	Note : 효과들 리스팅.
	//
	ListEffects ( pDxFrame );
}

void CFramePage::OnBnClickedButtonXfileopen()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;

	//	Note : 파일 필터를 정의한다.
	CString szFilter = "Frame Mesh xFile |*.X|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".x",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CDlgEdit*)this);

    CWorldEditApp* pApp = (CWorldEditApp*) AfxGetApp();
	CWorldEditDoc* pDoc = (CWorldEditDoc *) pFrame->GetActiveDocument ();
    const SUBPATH* pPath = pApp->GetSubPath();

	char szPath[MAX_PATH] = {0};
	strcpy(szPath, pFrame->m_szAppPath);
	strcat(szPath, pPath->ObjFileRoot().c_str()); //SUBPATH::OBJ_FILE_ROOT);

	dlg.m_ofn.lpstrInitialDir = szPath;
	if ( dlg.DoModal() == IDOK )
	{
		CString sFileName = dlg.GetFileName();
		pDoc->SetTitleAddVersion( sFileName );
		LoadXFile( sFileName.GetString() );
	}
}

void CFramePage::OnNMRclickTreeFrame(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	int pMenuID = 0;
	CMenu Menu;

	DWORD SelectionMade;
	VERIFY ( Menu.LoadMenu ( IDR_POPUP_FRAME ) );

	CMenu* pPopup = Menu.GetSubMenu ( pMenuID );
	ASSERT ( pPopup != NULL );	

	POINT pp;
	GetCursorPos (&pp);
	SelectionMade = pPopup->TrackPopupMenu (
		TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,
		pp.x, pp.y, this );

	pPopup->DestroyMenu ();

	switch ( SelectionMade )
	{
	case ID_FRAME:
		{
			HTREEITEM htreeitemCur = m_TreeFrame.GetSelectedItem();
			if ( htreeitemCur == NULL )	return;

			CString sSelectedFrame = m_TreeFrame.GetItemText ( htreeitemCur );
			
			CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
			CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
			DxFrameMesh *pFrameMesh = pView->GetFrameMesh();

			DxFrame *pDxFrame = pFrameMesh->FindFrame ( sSelectedFrame.GetString() );
			pFrameMesh->SetCollisionDetectedFrame ( pDxFrame );

			if ( pDxFrame )
			{
				D3DXVECTOR3 vToMove;
				vToMove.x = pDxFrame->matCombined._41;
				vToMove.y = pDxFrame->matCombined._42;
				vToMove.z = pDxFrame->matCombined._43;

				DxViewPort::GetInstance().CameraJump ( vToMove );
			}
		}
		break;

	case ID_FRAME_SELECTNAVIMESH:
		{
			HTREEITEM htreeitemCur = m_TreeFrame.GetSelectedItem();
			if ( htreeitemCur == NULL )	return;

			CString sSelectedFrame = m_TreeFrame.GetItemText ( htreeitemCur );

			CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
			CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
			DxFrameMesh *pFrameMesh = pView->GetFrameMesh();
			DxSetLandMan* pSetLandMan = pView->GetSetLandMan ();

			DxFrame *pDxFrame = pFrameMesh->FindFrame ( sSelectedFrame.GetString() );
			if (!pDxFrame)
                break;

			if (!pDxFrame->pmsMeshs)
			{
				MessageBox ( "Mesh가 들어있지 않은 프레임을 선택하였습니다.", "ERROR", MB_OK );
				break;
			}

			pSetLandMan->SetNaviFrame ( pDxFrame );

			CWnd *pWnd;
			pWnd = GetDlgItem ( IDC_STATIC_NAVI_NAME );
			if (pDxFrame)
                pWnd->SetWindowText ( pDxFrame->szName );
		}
		break;

	default:
		break;
	};
}

void CFramePage::OnBnClickedButtonEffectchangeadd()
{
	//	Note : 선택된 효과 TYPE.
	//
	DWORD nIndex = m_ComboEffectTypes.GetCurSel ();
	if ( nIndex == -1 ) return;
	DWORD dwEffectType = m_ComboEffectTypes.GetItemData ( nIndex );

	//	Note : 선택된 프레임.
	//
	HTREEITEM htreeitemCur = m_TreeFrame.GetSelectedItem();
	if ( htreeitemCur == NULL )	return;

	CString sSelectedFrame = m_TreeFrame.GetItemText ( htreeitemCur );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	DxFrameMesh *pFrameMesh = pView->GetFrameMesh();

	DxFrame *pDxFrame = pFrameMesh->FindFrame ( sSelectedFrame.GetString() );

	if ( !pDxFrame )	return;

	DxEffectBase* pEffectBase = DxEffectMan::GetInstance().AdaptToDxFrame ( dwEffectType, pFrameMesh, pDxFrame, pView->GetD3DDevice() );

	//	Note : 효과들 리스팅.
	//
	ListEffects ( pDxFrame );

	if ( !pEffectBase )
	{
		MessageBox ( "효과 인스턴트 생성에 실패하였습니다.", "오류", MB_OK|MB_ICONEXCLAMATION );
		return;
	}

	//	Note : 효과 설정 탭으로 이동.
	//
	SetActiveEffectPage ( dwEffectType, pEffectBase );
}

//	Note : 새로이 추가 되는 이팩트 에디트 Page 등록.
//
void CFramePage::SetActiveEffectPage ( DWORD TypeID, DxEffectBase* pEffectBase )
{
	if ( TypeID == DxEffectTiling::TYPEID )
		m_pSheetTab->ActiveEffectTilingPage ( (DxEffectTiling*) pEffectBase );
	else if ( TypeID == DxEffectCloth::TYPEID )
		m_pSheetTab->ActiveEffectClothPage ( (DxEffectCloth*) pEffectBase );
	else if ( TypeID == DxEffectDot3::TYPEID )
		m_pSheetTab->ActiveEffectDot3Page ( (DxEffectDot3*) pEffectBase );
	else if ( TypeID == DxEffectRiver::TYPEID )
		m_pSheetTab->ActiveEffectRiverPage ( (DxEffectRiver*) pEffectBase );
	else if ( TypeID == DxEffectShadow::TYPEID )
		m_pSheetTab->ActiveEffectShadowPage ( (DxEffectShadow*) pEffectBase );
	else if ( TypeID == DxEffectFur::TYPEID )
		m_pSheetTab->ActiveEffectFurPage ( (DxEffectFur*) pEffectBase );
	else if ( TypeID == DxEffectSpecular::TYPEID )
		m_pSheetTab->ActiveEffectSpecularPage ( (DxEffectSpecular*) pEffectBase );
	else if ( TypeID == DxEffectRainPoint::TYPEID )
		m_pSheetTab->ActiveEffectRainPointPage ( (DxEffectRainPoint*) pEffectBase );
	else if ( TypeID == DxEffectGrass::TYPEID )
		m_pSheetTab->ActiveEffectGrassPage ( (DxEffectGrass*) pEffectBase );
	else if ( TypeID == DxEffectSpecular2::TYPEID )
		m_pSheetTab->ActiveEffectSpecular2Page ( (DxEffectSpecular2*) pEffectBase );
	else if ( TypeID == DxEffectNature::TYPEID )
		m_pSheetTab->ActiveEffectNaturePage ( (DxEffectNature*) pEffectBase );
	else if ( TypeID == DxEffectWaterLight::TYPEID )
		m_pSheetTab->ActiveEffectWaterLightPage ( (DxEffectWaterLight*) pEffectBase );
	else if ( TypeID == DxEffectRenderState::TYPEID )
		m_pSheetTab->ActiveEffectRenderStatePage ( (DxEffectRenderState*) pEffectBase );
	else if ( TypeID == DxEffectSpore::TYPEID )   
		m_pSheetTab->ActiveEffectSporePage ( (DxEffectSpore*) pEffectBase );
	else if ( TypeID == DxEffectReflect::TYPEID )   
		m_pSheetTab->ActiveEffectReflectPage ( (DxEffectReflect*) pEffectBase );
	else if ( TypeID == DxEffectMultiTex::TYPEID )   
		m_pSheetTab->ActiveEffectMultiTexPage ( (DxEffectMultiTex*) pEffectBase );
	else if ( TypeID == DxEffectNeon::TYPEID )   
		m_pSheetTab->ActiveEffectNeonPage ( (DxEffectNeon*) pEffectBase );
	else if ( TypeID == DxEffectSpecReflect::TYPEID )   
		m_pSheetTab->ActiveEffectSpecReflectPage ( (DxEffectSpecReflect*) pEffectBase );
	else if ( TypeID == DxEffectLightMap::TYPEID )   
		m_pSheetTab->ActiveEffectLightMapPage ( (DxEffectLightMap*) pEffectBase );
	else if ( TypeID == DxEffectGlow::TYPEID )   
		m_pSheetTab->ActiveEffectGlowPage ( (DxEffectGlow*) pEffectBase );
	else
	{
		MessageBox ( "효과 설정 인터페이스가 준비되어 있지 않습니다.", "정보", MB_OK|MB_ICONINFORMATION );
	}
}

void CFramePage::OnTvnSelchangedTreeFrame(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	SelectChangedTreeFrame ();

	SetLightMapXY();
}

void CFramePage::OnBnClickedButtonEffectstate()
{
	//	Note : 효과 설정 탭으로 이동.
	//
	int nIndex = m_ListBoxEffect.GetCurSel ();
	if ( nIndex == -1 ) return;
	DxEffectBase *pEffect = (DxEffectBase*) m_ListBoxEffect.GetItemData ( nIndex );

	SetActiveEffectPage ( pEffect->GetTypeID(), pEffect );
}

void CFramePage::OnBnClickedButtonEffectdelete()
{
	//	Note : 선택된 프레임.
	//
	HTREEITEM htreeitemCur = m_TreeFrame.GetSelectedItem();
	if ( htreeitemCur == NULL )	return;

	CString sSelectedFrame = m_TreeFrame.GetItemText ( htreeitemCur );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	DxFrameMesh *pFrameMesh = pView->GetFrameMesh();

	DxFrame *pDxFrame = pFrameMesh->FindFrame ( sSelectedFrame.GetString() );
	
	if ( !pDxFrame )	return;

	//	Note : 선택된 이팩트.
	//
	int nIndex = m_ListBoxEffect.GetCurSel ();
	if ( nIndex == -1 ) return;
	DxEffectBase *pEffect = (DxEffectBase*) m_ListBoxEffect.GetItemData ( nIndex );

	pFrameMesh->DelEffect ( pDxFrame, pEffect );

	//	Note : 효과들 리스팅.
	//
	ListEffects ( pDxFrame );
}

void CFramePage::OnBnClickedButtonFramestate()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();


	HTREEITEM htreeitemCur = m_TreeFrame.GetSelectedItem();
	if ( htreeitemCur == NULL )	return;

	CString sSelectedFrame = m_TreeFrame.GetItemText ( htreeitemCur );

	DxFrameMesh *pFrameMesh = pView->GetFrameMesh();
	DxFrame *pDxFrame = pFrameMesh->FindFrame ( sSelectedFrame.GetString() );

	if ( !pDxFrame )	return;

	if ( pDxFrame->pmsMeshs )
	{
		m_pSheetTab->ActiveFrameStatePage ( pDxFrame );
	}
	else
	{
		AfxMessageBox( _T("Mesh 정보가 들어있지 않습니다.") );
	}
}

void CFramePage::OnBnClickedButtonReplaceSet()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->ActiveReplaceFramePage ();
}

void CFramePage::ResetContent ()
{
	m_TreeFrame.DeleteAllItems();
	m_ListBoxEffect.ResetContent ();
}

void CFramePage::UpdateContrl ()
{
	if ( !m_bDlgInit )	return;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	DxFrameMesh* pFrameMesh = pView->GetFrameMesh();
	DxSetLandMan* pSetLandMan = pView->GetSetLandMan ();
	SNATIVEID MapID = pSetLandMan->GetMapID ();

	if ( pFrameMesh )
	{
		CWnd *pWnd;
		
		pWnd = GetDlgItem ( IDC_EDIT_XFILE );
		pWnd->SetWindowText ( pFrameMesh->GetFileName() );

		//char szValue[256];
		//pWnd = GetDlgItem( IDC_EDIT_LIGHTMAP_DETAIL );
		//_gcvt ( pSetLandMan->m_fLightMapDetail, 7, szValue );
		//pWnd->SetWindowText( szValue );

		DxFrame *pFrame = NULL;
		pSetLandMan->GetNaviFrame ( &pFrame);

		pWnd = GetDlgItem ( IDC_STATIC_NAVI_NAME );
		if ( pFrame )	pWnd->SetWindowText ( pFrame->szName );
		else			pWnd->SetWindowText ( "" );

		SetTreeFrame ( pFrameMesh->GetFrameRoot() );
		m_ListBoxEffect.ResetContent ();
	}

	CWnd *pWnd;
	CString Str;

	pWnd = GetDlgItem ( IDC_EDIT_MAPMAINID );
	Str.Format ( "%d", MapID.wMainID );
	pWnd->SetWindowText ( Str );

	pWnd = GetDlgItem ( IDC_EDIT_MAPSUBID );
	Str.Format ( "%d", MapID.wSubID );
	pWnd->SetWindowText ( Str );
}

void CFramePage::OnEnChangeEditMapmainid()
{
	if ( !m_bDlgInit )	return;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	DxSetLandMan* pSetLandMan = pView->GetSetLandMan ();
	SNATIVEID MapID = pSetLandMan->GetMapID ();

	CWnd *pWnd;
	CString Str;

	pWnd = GetDlgItem ( IDC_EDIT_MAPMAINID );
	pWnd->GetWindowText ( Str );

	MapID.wMainID = atoi ( Str.GetString() );

	pSetLandMan->SetMapID ( MapID );
}

void CFramePage::OnEnChangeEditMapsubid()
{
	if ( !m_bDlgInit )	return;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	DxSetLandMan* pSetLandMan = pView->GetSetLandMan ();
	SNATIVEID MapID = pSetLandMan->GetMapID ();

	CWnd *pWnd;
	CString Str;

	pWnd = GetDlgItem ( IDC_EDIT_MAPSUBID );
	pWnd->GetWindowText ( Str );

	MapID.wSubID = atoi ( Str.GetString() );

	pSetLandMan->SetMapID ( MapID );
}

void CFramePage::OnBnClickedCheckNaviview()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton *pButton = (CButton *) GetDlgItem ( IDC_CHECK_NAVIVIEW );
	DxFrameMesh::m_bNaviDraw = pButton->GetCheck ();
}

void CFramePage::OnBnClickedButtonTextureSetting()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	DxSetLandMan* pSetLandMan = pView->GetSetLandMan ();
	if ( !pSetLandMan )
		return;

	DxFrameMesh* pFrameMesh = pSetLandMan->GetFrameMesh();
	if ( !pFrameMesh )
		return;

	CTextureSetDlg sDlg;
	sDlg.SetMaterial( pFrameMesh->GetMaterialSetting() );

	if ( IDOK == sDlg.DoModal () )
	{
		NSMaterialManager::OnlyVersionUpForTool();
	}
}

// Piece 버젼 Change 기능.
void CFramePage::OnBnClickedButtonTexeff()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();

	CString szFilter = _T("Piece File (*.pie)|*.pie");
	CFECFileDialog dlg ( TRUE, _T(".pie"), NULL, OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT | OFN_ENABLESIZING, szFilter, this );

	CWorldEditApp* pApp = (CWorldEditApp*) AfxGetApp();
	const SUBPATH* pPath = pApp->GetSubPath();

	char szPath[MAX_PATH] = {0};
	strcpy(szPath, pFrame->m_szAppPath);
	strcat(szPath, pPath->ObjFilePiece().c_str()); //SUBPATH::OBJ_FILE_PIECE);

	CString StrInitDir = szPath;
	dlg.m_ofn.lpstrInitialDir = StrInitDir;

	if ( dlg.DoModal() == IDOK )
	{
		CString strPathName;
		POSITION pos = dlg.GetStartPosition();
		while (pos != NULL)
		{
			strPathName = dlg.GetNextPathName(pos);
			CString strFileName = strPathName;
			int nPosition = strFileName.ReverseFind('\\');
			strFileName = strFileName.Mid(nPosition+1);

			std::string szFullName;
			szFullName = DxPieceContainer::GetInstancePSF().GetPath();
			szFullName += strFileName.GetString();

			DxPieceEdit sPieceEdit;
			sPieceEdit.LoadPIE( pView->GetD3DDevice(), szFullName.c_str(), DxPieceContainer::GetInstancePSF().GetPieceEditPath(), strFileName.GetString() );
			if ( sPieceEdit.ChangeDxStaticMaterialMesh( pView->GetD3DDevice() ) )
			{
				TSTRING strNewName = strFileName.GetString();
				strNewName += _T("15");
				sPieceEdit.SavePIE( strNewName.c_str() );
			}
		}
	}

	//CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	//CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	//DxFrameMesh* pFrameMesh = pView->GetFrameMesh();
	//if( !pFrameMesh )
	//{
	//	MessageBox( "pFrameMesh Point NULL, X-File Load" );
	//	return;
	//}

	//DxTextureEffMan* pTextureEff = pFrameMesh->GetTextureEffMan();
	//if( !pTextureEff )
	//{
	//	MessageBox( "DxTextureEffMan Pointer NULL" );
	//	return;
	//}

	//if( !pTextureEff->GetTextureListNUM() )
	//{
	//	MessageBox( "Texture 0 Num. X File에 텍스쳐가 하나도 없네요." );
	//	return;
	//}

	//m_pSheetTab->SetActivePageEX( PIECETEXEFFINDEX, FRAMEPAGEINDEX );
}

void CFramePage::OnBnClickedSnowCheck()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( ((CButton*)GetDlgItem(IDC_SNOW_CHECK))->GetCheck() )
	{
		DxWeatherMan::GetInstance()->GetSnow()->SetApplyRate( 1.0f );
		DxWeatherMan::GetInstance()->GetSnow()->SetSnowState( EMWS_STARTING );
	}else{
		DxWeatherMan::GetInstance()->GetSnow()->SetApplyRate( 0.0f );
		DxWeatherMan::GetInstance()->GetSnow()->SetSnowState( EMWS_NULL );
	}

}

void CFramePage::OnBnClickedRainCheck()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( ((CButton*)GetDlgItem(IDC_RAIN_CHECK))->GetCheck() )
	{
		DxWeatherMan::GetInstance()->GetRain()->SetApplyRate( 1.0f );
		DxWeatherMan::GetInstance()->GetRain()->SetRainState( EMWS_STARTING );
	}else{
		DxWeatherMan::GetInstance()->GetRain()->SetApplyRate( 0.0f );
		DxWeatherMan::GetInstance()->GetRain()->SetRainState( EMWS_NULL );
	}
}

void CFramePage::OnBnClickedLeaveCheck()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( ((CButton*)GetDlgItem(IDC_LEAVE_CHECK))->GetCheck() )
	{
		DxWeatherMan::GetInstance()->GetLeaves()->SetApplyRate( 1.0f );
		DxWeatherMan::GetInstance()->GetLeaves()->SetLeavesState( EMWS_STARTING );
	}else{
		DxWeatherMan::GetInstance()->GetLeaves()->SetApplyRate( 0.0f );
		DxWeatherMan::GetInstance()->GetLeaves()->SetLeavesState( EMWS_NULL );
	}
}

void CFramePage::OnBnClickedCheckCamcoll()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	DxLandMan* pLandMan = pView->GetLandMan ();

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( ((CButton*)GetDlgItem(IDC_CHECK_CAMCOLL))->GetCheck() )
	{
		pLandMan->SetCamCollision(TRUE);
	}else{
		pLandMan->SetCamCollision(FALSE);
	}
}

void CFramePage::UpdateCamColl()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	DxLandMan* pLandMan = pView->GetLandMan ();

	if( pLandMan->GetCamCollision() )
	{
		((CButton*)GetDlgItem(IDC_CHECK_CAMCOLL))->SetCheck( TRUE );
	}else{
		((CButton*)GetDlgItem(IDC_CHECK_CAMCOLL))->SetCheck( FALSE );
	}
}

void CFramePage::PostNcDestroy()
{
    // TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.



    CPropertyPage::PostNcDestroy();
}

void CFramePage::OnBnClickedButtonPieceTexVmView()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    CTexPieceVMViewer sDlg;

    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	DxSetLandMan* pSetLandMan = pView->GetSetLandMan ();

    LPCSTR pPieceName = NULL;
	LPCSTR pFrameName = NULL;
    std::set<TSTRING> setPiece;
    // Piece 를 가져와서 텍스쳐 별로 VM 이 있는것만 등록시킨다.
    DxPiece* pPiece(NULL);
    DxPieceManagerPSF* pPieceManager = pSetLandMan->GetPieceManager();
    DWORD dwSize = pPieceManager->GetSize();
    for ( DWORD i=0; i<dwSize; ++i )
    {
        pPieceManager->IsGetName( i, pPieceName, pFrameName );

        // 새로운 Piece 만 등록 하도록 한다.
        std::set<TSTRING>::iterator iter = setPiece.find( pPieceName );
        if ( iter == setPiece.end() )
        {
            setPiece.insert( pPieceName );

            DxPieceEdit* pPiece = DxPieceContainer::GetInstancePSF().LoadPiece( pView->GetD3DDevice(), pPieceName );
            if ( pPiece )
            {
                DWORD dwFlag;
                DWORD dwFlagEff;
	            LPCSTR szTexName = NULL;
	            std::string szNameTYPE;
                TSTRING strVM;

                const DxTextureEffMan* pTexEffMan = pPiece->GetTextureEffMan();
                for ( int i=0; i<pTexEffMan->GetTextureListNUM(); ++i )
	            {
		            if( pTexEffMan->IsGetTextureEFFDXT( i, szTexName, dwFlag, dwFlagEff, strVM ) )
		            {
                        if ( !strVM.empty() )
                        {
                            sDlg.SetData( szTexName, pPieceName, strVM );
                        }
                    }
                }
            }
        }
    }

    if( sDlg.DoModal() == IDOK )
	{
	}
}

void CFramePage::OnEnChangeEditLightmapDetail()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CPropertyPage::OnInitDialog() 함수를 재지정하고  마스크에 OR 연산하여 설정된
	// ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출해야만
	// 해당 알림 메시지를 보냅니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	if ( !pFrame )
		return;

	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	if ( !pView )
		return;

	DxSetLandMan* pSetLandMan = pView->GetSetLandMan();
	if ( !pSetLandMan )
		return;

	CString strText;
	CWnd* pWnd = GetDlgItem( IDC_EDIT_LIGHTMAP_DETAIL );
	pWnd->GetWindowText( strText );
	NSMaterialManager::SetEditLightMapPower( static_cast<float>( atof( strText.GetString() ) ) );
}

void CFramePage::OnNMClickTreeFrame(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	SetLightMapXY();
}

void CFramePage::SetLightMapXY()
{
	HTREEITEM htreeitemCur = m_TreeFrame.GetSelectedItem();
	if ( htreeitemCur == NULL )	return;

	CString sSelectedFrame = m_TreeFrame.GetItemText ( htreeitemCur );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	DxFrameMesh *pFrameMesh = pView->GetFrameMesh();

	DxFrame *pDxFrame = pFrameMesh->FindFrame ( sSelectedFrame.GetString() );
	if ( !pDxFrame )
	{
		CWnd* pWnd;
		pWnd = GetDlgItem( IDC_EDIT_LIGHTMAP_X );
		pWnd->SetWindowText( _T("") );
		pWnd = GetDlgItem( IDC_EDIT_LIGHTMAP_Y );
		pWnd->SetWindowText( _T("") );
		return;
	}

	DxMeshes *pmsMeshs = pDxFrame->pmsMeshs;
	if ( !pmsMeshs )
	{
		CWnd* pWnd;
		pWnd = GetDlgItem( IDC_EDIT_LIGHTMAP_X );
		pWnd->SetWindowText( _T("") );
		pWnd = GetDlgItem( IDC_EDIT_LIGHTMAP_Y );
		pWnd->SetWindowText( _T("") );
		return;
	}

	CWnd* pWnd;
	char szValue[256];
	pWnd = GetDlgItem( IDC_EDIT_LIGHTMAP_X );
	_gcvt ( pmsMeshs->m_wwLightMapUserUV_TextureSize.wA, 7, szValue );
	pWnd->SetWindowText( szValue );
	pWnd = GetDlgItem( IDC_EDIT_LIGHTMAP_Y );
	_gcvt ( pmsMeshs->m_wwLightMapUserUV_TextureSize.wB, 7, szValue );
	pWnd->SetWindowText( szValue );
}

bool CFramePage::LoadXFile( const TCHAR* pFileName )
{
	HRESULT hr = S_OK;
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();
	DxSetLandMan* pSetLandMan = pView->GetSetLandMan();
	DxFrameMesh* pFrameMesh = pView->GetFrameMesh ();
	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3DDevice ();

	CWorldEditApp* pApp = (CWorldEditApp*) AfxGetApp();
	const SUBPATH* pPath = pApp->GetSubPath();

	TSTRING strPath;
	strPath = pPath->AppRoot().c_str();
	strPath += pPath->ObjFileRoot().c_str();

	//	Note : 이전 로드된것과 연관된 오브젝트 초기화.
	//
	pView->ResetObjects ();

	//	Note : 종전 데이터 삭제.
	//
	hr = pFrameMesh->CleanUp ();
	if ( FAILED(hr) )	goto e_ERROR;

	//	Note : 읽을 x파일과 버텍스 구조 정보 ( FVF ) 를 준다.
	//
	pFrameMesh->SetFile ( strPath.c_str(), pFileName );
	pFrameMesh->SetFVF ( VERTEX_WORLD::FVF );

	//	Note : 초기화 한다.
	//
	hr = pFrameMesh->Create ( pd3dDevice );
	if ( FAILED(hr) )	goto e_ERROR;
	pFrameMesh->UpdateFrames();

	// 충돌  Mesh 도 작동되도록 한다.
	pSetLandMan->GetCollisionMap()->CleanUp();
	pSetLandMan->GetCollisionMap()->Import( TRUE );

	//	Note : 프레임 트리 등록.
	//
	SetTreeFrame ( pFrameMesh->GetFrameRoot() );

	CWnd *pWnd = GetDlgItem ( IDC_EDIT_XFILE );
	pWnd->SetWindowText ( pFileName );

	WorldStateMachine::ClearFileNotifier();
	WorldStateMachine::InsertFileNotifier();
	return true;

e_ERROR:
	pWnd = GetDlgItem ( IDC_EDIT_XFILE );
	pWnd->SetWindowText ( "" );

	m_TreeFrame.DeleteAllItems();

	MessageBox ( "xFile Load Failed!" );
	return false;
}

void CFramePage::OnBnClickedButtonMinimap()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->SetActivePageEX( MINIMAPPAGEINDEX, FRAMEPAGEINDEX );
}

void CFramePage::OnBnClickedButtonOcclusionCulling()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->SetActivePageEX( OCCLUSIONPAGEINDEX, FRAMEPAGEINDEX );

	//CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	//CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();
	//DxLandMan* pLandMan = pView->GetLandMan();
	//NavigationMesh* pNavigationMesh = pLandMan->GetNaviMesh();
	//if ( !pNavigationMesh )
	//{
	//	MessageBox ( "NavigationMesh 가 존재하지 않습니다." );
	//	return;
	//}

	//TSTRING strNameWLD = pLandMan->GetNameWLD();
	//if ( strNameWLD.empty() )
	//{
	//	MessageBox ( "Octree 파일 이름이 존재하지 않습니다." );
	//	return;
	//}


}

void CFramePage::OnBnClickedButtonCubeMap()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->SetActivePageEX( CUBEMAPPAGEINDEX, FRAMEPAGEINDEX );
}
