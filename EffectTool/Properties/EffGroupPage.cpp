// EffGroupPage.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../EffectTool.h"
#include "../MainFrm.h"

#include "../EffectToolDoc.h"
#include "../EffectToolView.h"
#include "EffSheetWithTab.h"
#include "../MfcExLib/ExLibs/DxControlFunc.h"

#include "../../EngineLib/DxEffect/Single/DxEffSingleMan.h"
#include "../../EngineLib/DxEffect/Single/DxEffSinglePropGMan.h"
#include "../../EngineLib/DxEffect/Single/DxEffGroupPlayer.h"
#include "../../EngineLib/DxEffect/EffectLua/NSEffectLua.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinAnimation.h"
#include "../../EngineLib/DxMeshs/SkinMesh/SANIMCONINFO.h"
#include "../../RanLogicClient/Char/GLCharacter.h"

#include "../Dialog/SequenceNumberDlg.h"
#include "../../EngineLib/DxTools/DxEditMat.h"

#include "EffGroupPage.h"

extern BOOL					g_bEFFtoTEX;

EFF_PROPGROUP	CEffGroupPage::m_EffPropGroup;
D3DXMATRIX		CEffGroupPage::m_TargetMatrix;

// CEffGroupPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CEffGroupPage, CPropertyPage)
CEffGroupPage::CEffGroupPage()
	: CPropertyPage(CEffGroupPage::IDD),
	m_SaveProp(NULL)
{
	D3DXMatrixIdentity( &m_TargetMatrix );
}

CEffGroupPage::~CEffGroupPage()
{
	CEffGroupPage::m_EffPropGroup.CleanUp();
}

void CEffGroupPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_EFFTYPE, m_ComboEffectTypes);
	DDX_Control(pDX, IDC_TREE_EFFGROUP, m_treeEffectGroup);
	DDX_Control(pDX, IDC_LIST_CHARANIM, m_ListCharAnim);
}


BEGIN_MESSAGE_MAP(CEffGroupPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_INSERT, OnBnClickedButtonInsert)
	ON_BN_CLICKED(IDC_BUTTON_EFFSTATE, OnBnClickedButtonEffstate)
	ON_BN_CLICKED(IDC_BUTTON_EFFDEL, OnBnClickedButtonEffdel)
	ON_BN_CLICKED(IDC_BUTTON_PLAY, OnBnClickedButtonPlay)
	ON_BN_CLICKED(IDC_BUTTON_MOVE_OBJ, OnBnClickedButtonMoveObj)
	ON_BN_CLICKED(IDC_BUTTON_UP, OnBnClickedButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_DOWN, OnBnClickedButtonDown)
	ON_BN_CLICKED(IDC_BUTTON_COPY, OnBnClickedButtonCopy)
	ON_BN_CLICKED(IDC_BUTTON_PASTE, OnBnClickedButtonPaste)
	ON_BN_CLICKED(IDC_BUTTON_TEST_TARGET, OnBnClickedButtonTestTarget)
	ON_BN_CLICKED(IDC_BUTTON_AABB_CLEAR, OnBnClickedButtonAabbClear)
	ON_BN_CLICKED(IDC_BUTTON_EFFTOTEX_START, OnBnClickedButtonEfftotexStart)
	ON_BN_CLICKED(IDC_BUTTON_EFFTOTEX_END, OnBnClickedButtonEfftotexEnd)
	ON_BN_CLICKED(IDC_CHECK_AUTO_AABBBOX, OnBnClickedCheckAutoAabbbox)
	ON_BN_CLICKED(IDC_BUTTON_RH_APPLY, OnBnClickedButtonRhApply)
	ON_BN_CLICKED(IDC_BUTTON_ANIM_SELECT, OnBnClickedButtonAnimSelect)
	ON_BN_CLICKED(IDC_BUTTON_CHF_LOAD, OnBnClickedButtonChfLoad)
	ON_NOTIFY(NM_RCLICK, IDC_TREE_EFFGROUP, OnNMRclickTreeEffgroup)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_EFFGROUP, OnNMDblclkTreeEffgroup)
	ON_BN_CLICKED(IDC_BUTTON_TEST_ABORT, &CEffGroupPage::OnBnClickedButtonTestAbort)
END_MESSAGE_MAP()


// CEffGroupPage 메시지 처리기입니다.

BOOL CEffGroupPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	//	Note : 효과 타입들을 등록.
	//
	//
	DxEffSingleType *pCurEffectType = DxEffSingleMan::GetInstance().GetEffectList();
	while ( pCurEffectType )
	{
		int nIndex = m_ComboEffectTypes.AddString ( pCurEffectType->NAME );
		m_ComboEffectTypes.SetItemData ( nIndex, pCurEffectType->TYPEID );

		pCurEffectType = pCurEffectType->pNext;
	}

	m_ComboEffectTypes.SetCurSel ( 0 );


	SetTreeEffGroup ();

//	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BOOL CEffGroupPage::OnSetActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	if( pFrame )
	{
		CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
		if( pView )		pView->DeActiveEditMatrix();
	}

	CButton* pButton = (CButton*)GetDlgItem( IDC_CHECK_AUTO_AABBBOX );
	if( CEffGroupPage::m_EffPropGroup.m_dwFlag & EFF_CFG_AUTOAABBBOX )
	{
		pButton->SetCheck( TRUE );
	}
	else
	{
		pButton->SetCheck( FALSE );
	}

	CWnd* pWnd;
	CString Str;
	float fZero = 3.f;

	Str.Format( "%3.2f", fZero );
	pWnd = GetDlgItem( IDC_EDIT_RADIUS );
	pWnd->SetWindowText( Str.GetString() );

	Str.Format( "%3.2f", fZero );
	pWnd = GetDlgItem( IDC_EDIT_HEIGHT );
	pWnd->SetWindowText( Str.GetString() );

	return CPropertyPage::OnSetActive();
}

void CEffGroupPage::AddTreeEffGroupItem ( EFF_PROPERTY* pEffProp, HTREEITEM hParent )
{
	//	Note : Frame 의 이름이 존재하는 노드만을 등록한다.
	//
	HTREEITEM htreeitemCur = NULL;
	EFF_PROPERTY* pEffSCur = pEffProp;

	while ( pEffSCur )
	{
		CString StrName;
		StrName = pEffSCur->GetName ();
		if ( pEffSCur->m_bMoveObj )		StrName += "  <- 이동 시간 주체";

		htreeitemCur = m_treeEffectGroup.InsertItem ( StrName, hParent );
		m_treeEffectGroup.SetItemData ( htreeitemCur, (DWORD)pEffSCur );

		if ( pEffSCur->m_pChild )
		{
			AddTreeEffGroupItem ( pEffSCur->m_pChild, htreeitemCur );
		}

		pEffSCur = pEffSCur->m_pSibling;
	}
}

void CEffGroupPage::SetCharAnimList()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	if( !pFrame )	return;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	if( !pView )		return;

	m_ListCharAnim.ResetContent();

	const DxSkinAniControl::VECNODE& vecANI = pView->m_pCharacter->GetSkinChar()->m_spSkinAniControlThread->GetAniList ();
	for ( size_t n=0; n<vecANI.size(); ++n )
	{
		m_ListCharAnim.AddString ( vecANI[n]->m_pDxSkinAni->m_strCfgFile.c_str() );
	}
}

void CEffGroupPage::SetTreeEffGroup ()
{
	m_treeEffectGroup.DeleteAllItems();

	HTREEITEM htreeitemCur;
	char* szRoot = "ROOT";
	htreeitemCur = m_treeEffectGroup.InsertItem ( szRoot, TVI_ROOT );
	m_treeEffectGroup.SetItemData ( htreeitemCur, (DWORD)TVI_ROOT );

	if ( m_EffPropGroup.m_pPropRoot )
	{
		AddTreeEffGroupItem ( m_EffPropGroup.m_pPropRoot, htreeitemCur );
	}

	HTREEITEM hCurItem = 0;
	ExpandTreeItem( m_treeEffectGroup, hCurItem, TVE_EXPAND );
}

void CEffGroupPage::SelectEffProp ( EFF_PROPERTY* pEffProp )
{
	assert(pEffProp);

	HTREEITEM hFindTreeItem = FindTreeItem ( m_treeEffectGroup, (DWORD) pEffProp );
	m_treeEffectGroup.SelectItem ( hFindTreeItem );
}

void CEffGroupPage::ActivePrppertyPage ( EFF_PROPERTY* pProp )
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();

	//	Note : 설정이 활성화되는 이팩트 프로퍼티의 AffineMatrix 를 편집하기 위해서
	//		편집 인스턴스에 이를 넘겨준다.
	pView->ActiveEditMatrix( &pProp->m_matLocal );
	
	//	Note : 이팩트 프로퍼티의 부모까지의 메트릭스의 합산을 구해서 
	//		이를 넘겨준다.
	D3DXMATRIX matParent, matComb;
	D3DXMatrixIdentity ( &matParent );
	m_EffPropGroup.GetMatCombParent( m_EffPropGroup.m_pPropRoot, pProp, matParent, matComb );
	//pView->m_pEditMat->SetFrameMatrix ( matComb );
	pView->SetParentMatrix( matComb );

	//	Note : 이팩트 프로퍼티의 아이디에 맞는 편집 Page 를 활성화 시킨다.
	//
	DWORD dwType = pProp->GetTypeID ();
	if ( dwType== EFFSINGLE_PARTICE )			m_pSheetTab->ActiveEffParticleSysPage ( pProp );
	else if ( dwType== EFFSINGLE_GROUND )		m_pSheetTab->ActiveEffGroundPage ( pProp );
	else if ( dwType== EFFSINGLE_MESH )			m_pSheetTab->ActiveEffMeshPage ( pProp );
	else if ( dwType== EFFSINGLE_SEQUENCE )		m_pSheetTab->ActiveEffSequencePage ( pProp );
	else if ( dwType== EFFSINGLE_DECAL )		m_pSheetTab->ActiveEffDecalPage ( pProp );
	else if ( dwType== EFFSINGLE_CAMERA )		m_pSheetTab->ActiveEffCameraPage ( pProp );
	else if ( dwType== EFFSINGLE_BLURSYS )		m_pSheetTab->ActiveEffBlurSysPage ( pProp );
	else if ( dwType== EFFSINGLE_LIGHTING )		m_pSheetTab->ActiveEffLightingPage ( pProp );
	else if ( dwType== EFFSINGLE_MOVEROTATE )	m_pSheetTab->ActiveEffMoveRotatePage ( pProp );
	else if ( dwType== EFFSINGLE_SKINMESH )		m_pSheetTab->ActiveEffSkinMeshPage ( pProp );
	else if ( dwType== EFFSINGLE_WAVE )			m_pSheetTab->ActiveEffWavePage ( pProp );
	else if ( dwType== EFFSINGLE_POINTLIGHT )	m_pSheetTab->ActiveEffPointLightPage ( pProp );
	else if ( dwType== EFFSINGLE_MOVETARGET )	m_pSheetTab->ActiveEffMoveTargetPage ( pProp );
	else if ( dwType== EFFSINGLE_TARGETINFLUENCE )	m_pSheetTab->ActiveEffTargetInfluencePage ( pProp );
	else if ( dwType== EFFSINGLE_SCREENEFFECT )	m_pSheetTab->ActiveEffScreenEffectPage ( pProp );
	
	//	Note : 지금의 Sheet 상태를 준다.
	//
	pView->m_iSheetTap = dwType;
}

void CEffGroupPage::OnBnClickedButtonInsert()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;

	//	Note : 선택된 이팩드를 알아냄.
	//
	int nIndex = m_ComboEffectTypes.GetCurSel();
	if ( nIndex==CB_ERR )	return;
	DWORD dwTypeID = m_ComboEffectTypes.GetItemData ( nIndex );

	//	Note : 트리에서 삽입 위치를 결정.
	//
	HTREEITEM htreeitemCur;
	DWORD dwTreeCurData = (DWORD)TVI_ROOT;
	
	htreeitemCur = m_treeEffectGroup.GetSelectedItem();
	if ( htreeitemCur != NULL )
	{
		dwTreeCurData = m_treeEffectGroup.GetItemData ( htreeitemCur );
	}

	//	Note : Property를 생성.
	//
	EFF_PROPERTY* pProperty = EFF_PROPERTY::NEW_PROP ( dwTypeID );
	if ( pProperty == NULL)
	{
		MessageBox ( "Effect Property 생성에 실패하였습니다.", "ERROR" );
		return;
	}

	if ( dwTreeCurData == ((DWORD)TVI_ROOT) )
	{
		//	Note : 루트 리스트에 등록.
		//
		pProperty->m_pSibling = m_EffPropGroup.m_pPropRoot;
		m_EffPropGroup.m_pPropRoot = pProperty;
	}
	else
	{
		EFF_PROPERTY* pParentProp = (EFF_PROPERTY*) dwTreeCurData;

		pProperty->m_pSibling = pParentProp->m_pChild;
		pParentProp->m_pChild = pProperty;
	}

	//	Note : 트리 노드 다시 등록.
	//
	SetTreeEffGroup ();

	

	//	Note :
	//
	pFrame->ListingTimeLine ( &m_EffPropGroup );

	ActivePrppertyPage ( pProperty );
}

void CEffGroupPage::OnBnClickedButtonEffstate ()
{
	HTREEITEM htreeitemCur = m_treeEffectGroup.GetSelectedItem ();
	if ( htreeitemCur == NULL )	return;

	DWORD dwData = m_treeEffectGroup.GetItemData ( htreeitemCur );

	if ( dwData == ((DWORD)TVI_ROOT) )
	{
		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();

		//	Note : 설정이 활성화되는 이팩트 프로퍼티의 AffineMatrix 를 편집하기 위해서
		//		편집 인스턴스에 이를 넘겨준다.
		//
		CEffectToolView::m_curType = _CUR_MOVE;
		
		D3DXMATRIX matParent;
		D3DXMatrixIdentity ( &matParent );
		//matParent._42 = m_EffPropGroup.m_vLocal.y;
		//m_TargetMatrix.m_pAffineParts->vTrans.y = m_EffPropGroup.m_vLocal.y;
		m_TargetMatrix._42 = m_EffPropGroup.m_vLocal.y;
		pView->SetParentMatrix( matParent );
		pView->ActiveEditMatrix( &m_TargetMatrix );
		//pView->m_pEditMat->SetFrameMatrix ( matParent );
		//pView->m_pEditMat->SetAffineMatrix ( &m_TargetMatrix );

		m_pSheetTab->ActiveEffGroupPropPage ( &m_EffPropGroup );
	}
	else
	{
		ActivePrppertyPage ( (EFF_PROPERTY*) (dwData) );
	}
}

void CEffGroupPage::OnBnClickedButtonEffdel ()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;

	DxEffGroupPlayer::GetInstance().SetTime ( 0.0f );
	DxEffGroupPlayer::GetInstance().DeleteEff ( CEffGroupPage::m_EffPropGroup.m_strFileName.c_str() );
	DxEffGroupPlayer::GetInstance().RemoveAllEff ();

	HTREEITEM htreeitemCur = m_treeEffectGroup.GetSelectedItem ();
	if ( htreeitemCur == NULL )	return;

	DWORD dwData = m_treeEffectGroup.GetItemData ( htreeitemCur );

	if ( m_SaveProp == (EFF_PROPERTY*)dwData )		m_SaveProp = NULL;	// 복제를 위해 나둔것 지움 

	if ( dwData == ((DWORD)TVI_ROOT) )
	{
		SAFE_DELETE(m_EffPropGroup.m_pPropRoot);
	}
	else
	{
		HRESULT hr = S_OK;
		hr = m_EffPropGroup.DeleteProp ( m_EffPropGroup.m_pPropRoot, (EFF_PROPERTY*) (dwData) );
		if ( hr == S_FALSE )
		{
			MessageBox ( "삭제할 이팩트를 찾지 못했습니다." );
		}
	}

	//	Note : 트리 노드 다시 등록.
	//
	SetTreeEffGroup ();

	//	Note : 타임라인 콘트롤을 초기화 시킨다.
	//
	pFrame->ListingTimeLine ( &m_EffPropGroup );
}

void CEffGroupPage::OnBnClickedButtonPlay()
{
	// 정보 refresh
	NSEffectLua::LuaLoad();

	D3DXMATRIX matTrans;
	D3DXMatrixIdentity ( &matTrans );
	
	//matTrans._41 = 50.f;
	//matTrans._42 = 0.f;
	//matTrans._43 = 50.f;
	

	STARGETID	TargetID;
	TargetID.GaeaId	= 2;
	TargetID.emCrow	= CROW_MOB;
	TargetID.vPos	= D3DXVECTOR3( 0.f, 0.f, 0.f );
	m_TargetMatrix._41 = TargetID.vPos.x;
	m_TargetMatrix._42 = TargetID.vPos.y;
	m_TargetMatrix._43 = TargetID.vPos.z;

	DxEffGroupPlayer::GetInstance().SetTime ( 0.0f );
	DxEffGroupPlayer::GetInstance().DeleteEff ( CEffGroupPage::m_EffPropGroup.m_strFileName.c_str() );
	DxEffGroupPlayer::GetInstance().RemoveAllEff ();

	DxEffSinglePropGMan::GetInstance().CleanUp();
	DxEffSinglePropGMan::GetInstance().Clear();

	DxEffGroupPlayer::GetInstance().NewEffGroup ( &m_EffPropGroup, matTrans, &TargetID );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	g_bEFFtoTEX = FALSE;

	DWORD nIndex = m_ListCharAnim.GetCurSel();
	if ( nIndex == -1 ) return;

	CString sItemText;
	m_ListCharAnim.GetText ( nIndex, sItemText );

	pView->m_pCharacter->GetSkinChar()->SELECTANI_EDIT( sItemText.GetString() );
	pView->m_pCharacter->GetSkinChar()->SETCURTIME( 0.0f );
	pView->m_pCharacter->GetSkinChar()->FrameMove( 0.0f, 0.0f, FALSE, matTrans );
}

void CEffGroupPage::LoadEffPropGroup ( CString strPathName )
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();

	DxEffGroupPlayer::GetInstance().DeleteEff ( CEffGroupPage::m_EffPropGroup.m_strFileName.c_str() );
	DxEffGroupPlayer::GetInstance().RemoveAllEff ();
	m_EffPropGroup.CleanUp ();

	m_EffPropGroup.LoadFile ( strPathName.GetString(), pView->GetD3dDevice() );
	
	//	Note : 트리 노드 다시 등록.
	//
	SetTreeEffGroup ();

	//	Note : 타임라인 콘트롤을 초기화 시킨다.
	//
	pFrame->ListingTimeLine ( &m_EffPropGroup );

	//	Note : Tap 을 초기화 시킨다.
	//
	CEffectToolView::m_curType = _CUR_NONE;
	m_pSheetTab->SetActivePage ( 0 );

	//	Note : 지금의 Sheet 상태를 준다.
	//
	pView->m_iSheetTap = 0;

	pView->ActiveEditBox ( m_EffPropGroup.m_vMax, m_EffPropGroup.m_vMin );
}

void CEffGroupPage::OnBnClickedButtonMoveObj()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	HTREEITEM htreeitemCur = m_treeEffectGroup.GetSelectedItem ();
	if ( htreeitemCur == NULL )	return;

	DWORD dwData = m_treeEffectGroup.GetItemData ( htreeitemCur );

	if ( dwData == ((DWORD)TVI_ROOT) )
	{
	}
	else
	{
		m_EffPropGroup.RESET_MOVEOBJ_PROP ( m_EffPropGroup.m_pPropRoot, *(EFF_PROPERTY*) (dwData) );
	}

	//	Note : 트리 노드 다시 등록.
	//
	SetTreeEffGroup ();
}

void CEffGroupPage::OnBnClickedButtonUp()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	HTREEITEM htreeitemCur = m_treeEffectGroup.GetSelectedItem ();
	if ( htreeitemCur == NULL )	return;

	DWORD dwData = m_treeEffectGroup.GetItemData ( htreeitemCur );

	EFF_PROPERTY* pTargetProp = (EFF_PROPERTY*) dwData;

	EFF_PROPERTY* pParentProp = NULL;
	EFF_PROPERTY* pCurUpProp = NULL;
	EFF_PROPERTY* pCurUpUpProp = NULL;


	//	Note : 부모가 나를 가르치면 올라 갈 수가 없다.
	pParentProp		= GetTreeEffGroupParent	( pTargetProp, m_EffPropGroup.m_pPropRoot );
	if ( pParentProp )	return;

	//	Note : 없다면 오류다.	올라 갈 수도 없다.
	pCurUpProp		= GetTreeEffGroupSibling	( pTargetProp, m_EffPropGroup.m_pPropRoot );
	if ( !pCurUpProp )	return;

	//	Note : 부모와 친구, 또 친구.
	pParentProp		= GetTreeEffGroupParent		( pCurUpProp, m_EffPropGroup.m_pPropRoot );
	pCurUpUpProp	= GetTreeEffGroupSibling	( pCurUpProp, m_EffPropGroup.m_pPropRoot );

	//	Note : 연결 부분
	if ( m_EffPropGroup.m_pPropRoot == pCurUpProp )
	{
		m_EffPropGroup.m_pPropRoot = pTargetProp;
	}
	if ( pParentProp )
	{
		pParentProp->m_pChild = pTargetProp;
	}
	if ( pCurUpUpProp )
	{
		pCurUpUpProp->m_pSibling = pTargetProp;
	}
	pCurUpProp->m_pSibling	= pTargetProp->m_pSibling;
	pTargetProp->m_pSibling	= pCurUpProp;


	//	Note : 트리 노드 다시 등록.
	//
	SetTreeEffGroup ();
}

void CEffGroupPage::OnBnClickedButtonDown()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	HTREEITEM htreeitemCur = m_treeEffectGroup.GetSelectedItem ();
	if ( htreeitemCur == NULL )	return;

	DWORD dwData = m_treeEffectGroup.GetItemData ( htreeitemCur );

	EFF_PROPERTY* pTargetProp = (EFF_PROPERTY*) dwData;

	EFF_PROPERTY* pParentProp	= NULL;
	EFF_PROPERTY* pCurUpProp	= NULL;
	EFF_PROPERTY* pCurDownProp	= NULL;



	//	Note : 부모와 친구
	pParentProp		= GetTreeEffGroupParent		( pTargetProp, m_EffPropGroup.m_pPropRoot );
	pCurUpProp		= GetTreeEffGroupSibling	( pTargetProp, m_EffPropGroup.m_pPropRoot );
	pCurDownProp	= pTargetProp->m_pSibling;

	//	Note : 없다면 실행 안한다.
	if ( !pCurDownProp )	return;


	//	Note : 연결 부분
	if ( m_EffPropGroup.m_pPropRoot == pTargetProp )
	{
		m_EffPropGroup.m_pPropRoot = pCurDownProp;
	}
	if ( pParentProp )
	{
		pParentProp->m_pChild = pCurDownProp;
	}
	if ( pCurUpProp )
	{
		pCurUpProp->m_pSibling = pCurDownProp;
	}
	pTargetProp->m_pSibling		= pCurDownProp->m_pSibling;
	pCurDownProp->m_pSibling	= pTargetProp;


	//	Note : 트리 노드 다시 등록.
	//
	SetTreeEffGroup ();
}

//	부모찾기
//
EFF_PROPERTY* CEffGroupPage::GetTreeEffGroupParent ( EFF_PROPERTY* pTargetProp, EFF_PROPERTY* pParentProp )
{
	EFF_PROPERTY* pResult = NULL;
	EFF_PROPERTY* pCurProp = pParentProp;

	while ( pCurProp )
	{
		if ( pCurProp->m_pChild == pTargetProp )
			return pCurProp;

		if ( pCurProp->m_pChild )
		{
			pResult = GetTreeEffGroupParent ( pTargetProp, pCurProp->m_pChild );
			if ( pResult )	return pResult;
		}

		pCurProp = pCurProp->m_pSibling;
	}

	return NULL;
}

//	바꿀 친구 찾기	-	윗 녀석
//
EFF_PROPERTY* CEffGroupPage::GetTreeEffGroupSibling ( EFF_PROPERTY* pTargetProp, EFF_PROPERTY* pParentProp )
{
	EFF_PROPERTY* pResult = NULL;
	EFF_PROPERTY* pCurProp = pParentProp;

	while ( pCurProp )
	{
		if ( pCurProp->m_pSibling == pTargetProp )
			return pCurProp;

		if ( pCurProp->m_pChild )
		{
			pResult = GetTreeEffGroupSibling ( pTargetProp, pCurProp->m_pChild );
			if ( pResult )	return pResult;
		}

		pCurProp = pCurProp->m_pSibling;
	}

	return NULL;
}

void CEffGroupPage::OnBnClickedButtonCopy()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	HTREEITEM htreeitemCur = m_treeEffectGroup.GetSelectedItem ();
	if ( htreeitemCur == NULL )	return;

	DWORD dwData = m_treeEffectGroup.GetItemData ( htreeitemCur );
	if( dwData == 0xffff0000 ) return;

	m_SaveProp = (EFF_PROPERTY*) dwData;

	DxEffGroupPlayer::GetInstance().SetTime ( 0.0f );
}

void CEffGroupPage::OnBnClickedButtonPaste()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if ( !m_SaveProp )
	{
		MessageBox ( "복사할 효과를 선택하지 않았습니다.", "ERROR" );
		return;
	}


	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();

	DxEffGroupPlayer::GetInstance().SetTime ( 0.0f );
	DxEffGroupPlayer::GetInstance().DeleteEff ( CEffGroupPage::m_EffPropGroup.m_strFileName.c_str() );
	DxEffGroupPlayer::GetInstance().RemoveAllEff ();

	//	Note : 트리에서 삽입 위치를 결정.
	//
	HTREEITEM htreeitemCur;
	DWORD dwTreeCurData = (DWORD)TVI_ROOT;
	
	htreeitemCur = m_treeEffectGroup.GetSelectedItem();
	if ( htreeitemCur != NULL )
	{
		dwTreeCurData = m_treeEffectGroup.GetItemData ( htreeitemCur );
	}

	//	Note : Property를 생성.
	//
	EFF_PROPERTY* pProperty = EFF_PROPERTY::NEW_PROP ( m_SaveProp->GetTypeID() );
	if ( pProperty == NULL)
	{
		MessageBox ( "Effect Property 생성에 실패하였습니다.", "ERROR" );
		return;
	}

	// Note : 복제
	pProperty->CloneEffProp ( pView->GetD3dDevice(), m_SaveProp, FALSE );

	if ( dwTreeCurData == ((DWORD)TVI_ROOT) )
	{
		//	Note : 루트 리스트에 등록.
		//
		pProperty->m_pSibling = m_EffPropGroup.m_pPropRoot;
		m_EffPropGroup.m_pPropRoot = pProperty;
	}
	else
	{
		EFF_PROPERTY* pParentProp = (EFF_PROPERTY*) dwTreeCurData;

		pProperty->m_pSibling = pParentProp->m_pChild;
		pParentProp->m_pChild = pProperty;
	}

	//	Note : 트리 노드 다시 등록.
	//
	SetTreeEffGroup ();
	

	//	Note :
	//
	pFrame->ListingTimeLine ( &m_EffPropGroup );
}

void CEffGroupPage::OnBnClickedButtonTestTarget()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	D3DXMATRIX matTrans;
	D3DXMatrixIdentity( &matTrans );
	matTrans._42 = 11.f;

	D3DXVECTOR3 vPos;
	STARGETID	TargetID;
	TargetID.GaeaId	 = 2;
	TargetID.emCrow	 = CROW_MOB;
	vPos.x = (float)(rand()%200-100) / 100.f;
	vPos.y = (float)(rand()%200-100) / 100.f;
	vPos.z = (float)(rand()%200-100) / 100.f;
	m_TargetMatrix._41 = vPos.x;
	m_TargetMatrix._42 = vPos.y;
	m_TargetMatrix._43 = vPos.z;
	TargetID.vPos	 = vPos * 60.f;	// 최대 20.?
	TargetID.vPos.y	= 11.f;

	DxEffGroupPlayer::GetInstance().SetTime( 0.0f );
	DxEffGroupPlayer::GetInstance().DeleteEff( CEffGroupPage::m_EffPropGroup.m_strFileName.c_str() );
	DxEffGroupPlayer::GetInstance().RemoveAllEff();
	DxEffGroupPlayer::GetInstance().NewEffGroup( &m_EffPropGroup, matTrans, &TargetID );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();

	g_bEFFtoTEX = FALSE;
}

void CEffGroupPage::OnBnClickedCheckAutoAabbbox()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton* pButton = (CButton*)GetDlgItem( IDC_CHECK_AUTO_AABBBOX );
	if( pButton->GetCheck() )
	{
		CEffGroupPage::m_EffPropGroup.m_dwFlag |= EFF_CFG_AUTOAABBBOX;
	}
	else
	{
		CEffGroupPage::m_EffPropGroup.m_dwFlag &= ~EFF_CFG_AUTOAABBBOX;
	}
}

void CEffGroupPage::OnBnClickedButtonAabbClear()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CEffGroupPage::m_EffPropGroup.m_vMax = D3DXVECTOR3( 0.f, 0.f, 0.f );
	CEffGroupPage::m_EffPropGroup.m_vMin = D3DXVECTOR3( 0.f, 0.f, 0.f );
}

void CEffGroupPage::OnBnClickedButtonRhApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	float fRadius(0.f);
	float fHeight(0.f);

	CWnd* pWnd;
	CString Str;

	pWnd = GetDlgItem( IDC_EDIT_RADIUS );
	pWnd->GetWindowText( Str );
	fRadius = (float)atof( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_HEIGHT );
	pWnd->GetWindowText( Str );
	fHeight = (float)atof( Str.GetString() );

	D3DXVECTOR3 vMin( 0.f,0.f,0.f );	
	D3DXVECTOR3 vMax( 0.f,0.f,0.f );

	vMax.y = fHeight;

	vMax.x = fRadius;
	vMax.z = fRadius;
	vMin.x = -fRadius;
	vMin.z = -fRadius;

	// Note : AABB Box 생성
	CEffGroupPage::m_EffPropGroup.m_vMax = vMax;
	CEffGroupPage::m_EffPropGroup.m_vMin = vMin;
}

#include "../../EngineLib/DxTools/DxEffToTexMan.h"

void CEffGroupPage::OnBnClickedButtonEfftotexStart()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CSequenceNumberDlg sDlg;
	if ( IDOK == sDlg.DoModal() )
	{
		D3DXMATRIX matTrans;
		D3DXMatrixIdentity ( &matTrans );

		STARGETID	TargetID;
		TargetID.GaeaId	= 2;
		TargetID.emCrow	= CROW_MOB;
		TargetID.vPos	= D3DXVECTOR3( 0.f, 0.f, 0.f );
		m_TargetMatrix._41 = TargetID.vPos.x;
		m_TargetMatrix._42 = TargetID.vPos.y;
		m_TargetMatrix._43 = TargetID.vPos.z;

		DxEffGroupPlayer::GetInstance().SetTime( 0.0f );
		DxEffGroupPlayer::GetInstance().DeleteEff( CEffGroupPage::m_EffPropGroup.m_strFileName.c_str() );
		DxEffGroupPlayer::GetInstance().RemoveAllEff();
		DxEffGroupPlayer::GetInstance().NewEffGroup( &m_EffPropGroup, matTrans, &TargetID );

		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
		g_bEFFtoTEX = TRUE;

		DxEffToTexMan::GetInstance().SetTexSize( sDlg.m_nTexSize );
		DxEffToTexMan::GetInstance().SetTexNumber( sDlg.m_nNumber );
		DxEffToTexMan::GetInstance().SetTexRepeat( sDlg.m_bRepeat );
	}
}

void CEffGroupPage::OnBnClickedButtonEfftotexEnd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame*) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
    CEffectToolApp* pApp = (CEffectToolApp*) AfxGetApp();
	DxEffToTexMan::GetInstance().EndEffToTex(pView->GetD3dDevice(), pApp->GetSubPath());
}

void CEffGroupPage::OnBnClickedButtonAnimSelect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	if (!pFrame)
        return;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	if (!pView)
        return;

	DWORD nIndex = m_ListCharAnim.GetCurSel();
	if ( nIndex == -1 )
        return;

	CString sItemText;
	m_ListCharAnim.GetText ( nIndex, sItemText );

	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );

	pView->m_pCharacter->GetSkinChar()->SELECTANI_EDIT( sItemText.GetString() );
	pView->m_pCharacter->GetSkinChar()->SETCURTIME( 0.0f );
	pView->m_pCharacter->GetSkinChar()->FrameMove( 0.0f, 0.0f, FALSE, matIdentity );

}

void CEffGroupPage::OnBnClickedButtonChfLoad()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	if( !pFrame )	return;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	if( !pView )		return;

	CString szFilter = "Charactor File (*.chf)|*.chf|";

	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".chf",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		this);

	dlg.m_ofn.lpstrInitialDir = DxSkinCharDataContainer::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		CString sFileName = dlg.GetFileName();

		pView->m_pCharacter->SkinLoadForTool( pView->GetD3dDevice(), sFileName.GetString() ); 

		SetCharAnimList();
	}
}

void CEffGroupPage::OnNMRclickTreeEffgroup(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	CMenu Menu;

	int pMenuID = 0;

	DWORD SelectionMade;
	VERIFY ( Menu.LoadMenu ( IDR_MENU_LINEBAR ) );

	CMenu* pPopup = Menu.GetSubMenu ( pMenuID );
	ASSERT ( pPopup != NULL );

	POINT pp;
	GetCursorPos (&pp);
	SelectionMade = pPopup->TrackPopupMenu (
		TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,
		pp.x, pp.y, this );

	pPopup->DestroyMenu ();

	HTREEITEM htreeitemCur = m_treeEffectGroup.GetSelectedItem();

	if( htreeitemCur == NULL ) return;
	DWORD dwTreeCurData = m_treeEffectGroup.GetItemData ( htreeitemCur );

	EFF_PROPERTY* pParentProp = (EFF_PROPERTY*) dwTreeCurData;

	switch ( SelectionMade )
	{
	case IDMENU_SELECT:
		{
			EFF_PROPERTY* pParentProp = (EFF_PROPERTY*) dwTreeCurData;
			CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
			pFrame->GetTimeLine()->ActiveLine((DWORD)pParentProp);
		}
		break;
	case IDMENU_COPY:
		{
			OnBnClickedButtonCopy();
		}
		break;
	case IDMENU_PASTE:
		{
			OnBnClickedButtonPaste();
		}
		break;
	case IDMENU_DELETE:
		{
			OnBnClickedButtonEffdel();
		}
		break;
	};
}

void CEffGroupPage::OnNMDblclkTreeEffgroup(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	HTREEITEM htreeitemCur = m_treeEffectGroup.GetSelectedItem();

	if( htreeitemCur == NULL ) return;
	DWORD dwTreeCurData = m_treeEffectGroup.GetItemData ( htreeitemCur );

	EFF_PROPERTY* pParentProp = (EFF_PROPERTY*) dwTreeCurData;
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	pFrame->GetTimeLine()->ActiveLine((DWORD)pParentProp);
	*pResult = 0;
}

void CEffGroupPage::OnBnClickedButtonTestAbort()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	DxEffGroupPlayer::GetInstance().SetTime ( 0.0f );
	DxEffGroupPlayer::GetInstance().DeleteEff ( CEffGroupPage::m_EffPropGroup.m_strFileName.c_str() );
	DxEffGroupPlayer::GetInstance().RemoveAllEff ();
}
