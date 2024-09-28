// LandGatePage.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "WorldEdit.h"
#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"
#include "../EngineLib/DxLand/DxLandGateMan.h"
#include "../EngineLib/DxTools/DxViewPort.h"
#include "State/WorldStateMachine.h"

#include "LandGatePage.h"


// CLandGatePage 대화 상자입니다.

static D3DXMATRIX g_matLocal;

IMPLEMENT_DYNAMIC(CLandGatePage, CPropertyPage)
CLandGatePage::CLandGatePage()
	: CPropertyPage(CLandGatePage::IDD),
	m_bDlgInit(FALSE),
	m_pLandGate(NULL)
{
}

CLandGatePage::~CLandGatePage()
{
}

void CLandGatePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_GATE, m_listLandGate);
}


BEGIN_MESSAGE_MAP(CLandGatePage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_NEW, OnBnClickedButtonNew)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnBnClickedButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_EDIT, OnBnClickedButtonEdit)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_CHECK_REND, OnBnClickedCheckRend)
	ON_BN_CLICKED(IDC_BUTTON_CLONE, OnBnClickedButtonClone)
END_MESSAGE_MAP()


// CLandGatePage 메시지 처리기입니다.
BOOL CLandGatePage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_bDlgInit = TRUE;
	UpdateContrl ();

	CButton *pButton = (CButton *) GetDlgItem ( IDC_CHECK_REND );
	pButton->SetCheck ( DxLandGateMan::m_bREND );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CLandGatePage::ListtingLandGate ()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();


	CString StrOldSelect;
	int nIndex = m_listLandGate.GetCurSel ();
	if ( nIndex!=LB_ERR )
	{
		m_listLandGate.GetDlgItemText ( nIndex, StrOldSelect );
	}

	m_listLandGate.ResetContent ();

	DxSetLandMan* pSetLandMan = pView->GetSetLandMan ();
	DxLandGateMan *pLandGateMan = pSetLandMan->GetLandGateMan ();
	PDXLANDGATE pCur = pLandGateMan->GetListLandGate();
	while ( pCur )
	{
		int nIndex = m_listLandGate.AddString ( pCur->GetName() );
		m_listLandGate.SetItemData ( nIndex, (DWORD)pCur );

		pCur = pCur->m_pNext;
	}

	m_listLandGate.SelectString ( 0, StrOldSelect );	
}

void CLandGatePage::UpdateContrl ()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();

	if ( !m_bDlgInit )	return;

	m_pLandGate = NULL;

	ViewContrls ( FALSE );
	ListtingLandGate ();

	pView->DeActiveEditMatrix();

	pView->SetCurType ( _CUR_SELECT );
}

void CLandGatePage::ViewContrls ( BOOL bView, BOOL bClear )
{
	if ( !m_bDlgInit )	return;

	DWORD dwButtons[] = { IDC_BUTTON_EDIT, IDC_BUTTON_DEL, IDC_BUTTON_NEW, IDC_BUTTON_CLONE };
	DWORD dwNumButtons = sizeof(dwButtons) / sizeof(DWORD);

	DWORD dwContrls[] = { IDC_EDIT_NAME, IDC_STATIC_GATEID2, IDC_STATIC_MAPID,
		IDC_STATIC_GATEFLAGS, IDC_CHECK_OUT, IDC_CHECK_IN,
		IDC_EDIT_GATEID2, IDC_EDIT_TOGATEID, 
		IDC_STATIC_GATEID, IDC_EDIT_MID, IDC_EDIT_SID, IDC_STATIC_CREATEPOS,
		IDC_EDIT_GEN_DX, IDC_EDIT_GEN_DY, IDC_BUTTON_ADD, IDC_BUTTON_APPLY };
	DWORD dwNumContrls = sizeof(dwContrls) / sizeof(DWORD);

	CWnd *pWnd;
	CButton *pButton;

	for ( DWORD i=0; i < dwNumButtons; i++ )
	{
		pWnd = GetDlgItem ( dwButtons[i] );
		if(pWnd) pWnd->EnableWindow ( !bView );
	}

	for ( DWORD i=0; i < dwNumContrls; i++ )
	{
		pWnd = GetDlgItem ( dwContrls[i] );
		if(pWnd) pWnd->ShowWindow ( bView );
	}

	if ( bClear )
	{
		pWnd = GetDlgItem ( IDC_EDIT_NAME );
		pWnd->SetWindowText ( "" );

		pWnd = GetDlgItem ( IDC_EDIT_GATEID2 );
		pWnd->SetWindowText ( "" );

		pWnd = GetDlgItem ( IDC_EDIT_MID );
		pWnd->SetWindowText ( "" );

		pWnd = GetDlgItem ( IDC_EDIT_SID );
		pWnd->SetWindowText ( "" );

		pWnd = GetDlgItem ( IDC_EDIT_TOGATEID );
		pWnd->SetWindowText ( "" );

		pWnd = GetDlgItem ( IDC_EDIT_GEN_DX );
		pWnd->SetWindowText ( "" );

		pWnd = GetDlgItem ( IDC_EDIT_GEN_DY );
		pWnd->SetWindowText ( "" );
	}
	else if ( m_pLandGate )
	{
		char szValue[256];
		CString strTemp;

		pWnd = GetDlgItem ( IDC_EDIT_NAME );
		pWnd->SetWindowText ( m_pLandGate->GetName() );

		pButton = (CButton *) GetDlgItem ( IDC_CHECK_OUT );
		pButton->SetCheck ( m_pLandGate->GetFlags()&DxLandGate::GATE_OUT );

		pButton = (CButton *) GetDlgItem ( IDC_CHECK_IN );
		pButton->SetCheck ( m_pLandGate->GetFlags()&DxLandGate::GATE_IN );

		//	Note : Gate ID
		pWnd = GetDlgItem ( IDC_EDIT_GATEID2 );
		strTemp.Format ( "%d", m_pLandGate->GetGateID() );
		pWnd->SetWindowText ( strTemp );

		//	Note : To Map ID.
		pWnd = GetDlgItem ( IDC_EDIT_MID );
		strTemp.Format ( "%d", m_pLandGate->GetToMapID().wMainID );
		pWnd->SetWindowText ( strTemp );

		pWnd = GetDlgItem ( IDC_EDIT_SID );
		strTemp.Format ( "%d", m_pLandGate->GetToMapID().wSubID );
		pWnd->SetWindowText ( strTemp );

		//	Note : To Gate ID
		pWnd = GetDlgItem ( IDC_EDIT_TOGATEID );
		strTemp.Format ( "%d", m_pLandGate->GetToGateID() );
		pWnd->SetWindowText ( strTemp );

		//	Note : 분할 간격.
		D3DXVECTOR2 vDiv = m_pLandGate->GetDiv ();
		pWnd = GetDlgItem ( IDC_EDIT_GEN_DX );
		_gcvt ( vDiv.x, 7, szValue );
		pWnd->SetWindowText ( szValue );

		pWnd = GetDlgItem ( IDC_EDIT_GEN_DY );
		_gcvt ( vDiv.y, 7, szValue );
		pWnd->SetWindowText ( szValue );
	}
}

void CLandGatePage::OnBnClickedButtonNew()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();
	DxSetLandMan *pSetLandMan = pView->GetSetLandMan();
	DxLandGateMan *pLandGateMan = pSetLandMan->GetLandGateMan ();

	PDXLANDGATE pLandGate;
	pLandGate = new DxLandGate;

	char szNewName[DxLandGate::MAX_SZNAME];
	
	int i=0;
	do
	{
		sprintf ( szNewName, "%s[%d]", "NoName", i++ );
	} while ( pLandGateMan->FindLandGate(szNewName) );

	pLandGate->SetName ( szNewName );

	DxLandGate* pCur = pLandGateMan->GetListLandGate ();
	
	DWORD dwGateID = 0;
	while ( pLandGateMan->FindLandGate(dwGateID) )
	{
		dwGateID++;
	} 

	pLandGate->SetGateID ( dwGateID );

	const D3DXVECTOR3& vCollPos = pView->GetCollisionPos();
	pLandGate->SetPos ( vCollPos );

	m_pLandGate = pLandGate;
	pView->ActiveEditBox ( m_pLandGate->GetMax(), m_pLandGate->GetMin() );
	pLandGateMan->AddLandGate ( m_pLandGate );
	pLandGateMan->BuildTree ();

    WorldStateMachine::EditAsset(m_pLandGate->GetName());

	m_listLandGate.SelectString ( 0, m_pLandGate->GetName() );

	ViewContrls ( TRUE );
	ListtingLandGate ();

	D3DXMatrixIdentity( &g_matLocal );
	pView->ActiveEditMatrix2( &g_matLocal );
}

void CLandGatePage::OnBnClickedButtonDel()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();
	DxSetLandMan *pSetLandMan = pView->GetSetLandMan();
	DxLandGateMan *pLandGateMan = pSetLandMan->GetLandGateMan ();

	int nIndex = m_listLandGate.GetCurSel ();
	if ( nIndex==LB_ERR )	return;

	PDXLANDGATE pLandGate = (PDXLANDGATE) m_listLandGate.GetItemData ( nIndex );

    WorldStateMachine::EditAsset(pLandGate->GetName());

	pLandGateMan->DelLandGate ( pLandGate->GetName() );
	pLandGateMan->BuildTree ();

	//	현제 편집중인 이팩트일경우.
	if ( pLandGate== m_pLandGate )
	{
		m_pLandGate = NULL;
		ViewContrls ( FALSE );
		UpdateContrl ();
	}
	else
	{
		ListtingLandGate ();
	}
}

void CLandGatePage::OnBnClickedButtonEdit()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();

	int nIndex = m_listLandGate.GetCurSel ();
	if ( nIndex==LB_ERR )	return;
	
	m_pLandGate = (PDXLANDGATE) m_listLandGate.GetItemData ( nIndex );
	pView->ActiveEditBox ( m_pLandGate->GetMax(), m_pLandGate->GetMin() );

	D3DXMatrixIdentity( &g_matLocal );
	pView->ActiveEditMatrix2( &g_matLocal );

	ViewContrls ( TRUE );

	DxViewPort::GetInstance().CameraJump ( (m_pLandGate->GetMax()+m_pLandGate->GetMin())/2.0f );
}

void CLandGatePage::OnBnClickedButtonApply()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();
	DxSetLandMan *pSetLandMan = pView->GetSetLandMan();
	DxLandGateMan *pLandGateMan = pSetLandMan->GetLandGateMan ();

	CWnd *pWnd;
	CButton *pButton;

	char szValue[256];
	char szName[DxLandGate::MAX_SZNAME];	
	DWORD dwFlags = NULL;
	DWORD GateID = 0;
	SNATIVEID TOMapID;
	DWORD ToGateID = 0;
	D3DXVECTOR2 vDIv(0,0);

	pWnd = GetDlgItem ( IDC_EDIT_NAME );
	pWnd->GetWindowText ( szName, DxLandGate::MAX_SZNAME );

	pButton = (CButton *) GetDlgItem ( IDC_CHECK_OUT );
	if ( pButton->GetCheck() )	dwFlags |= DxLandGate::GATE_OUT;

	pButton = (CButton *) GetDlgItem ( IDC_CHECK_IN );
	if ( pButton->GetCheck() )	dwFlags |= DxLandGate::GATE_IN;

	pWnd = GetDlgItem ( IDC_EDIT_GATEID2 );
	pWnd->GetWindowText ( szValue, 256 );
	GateID = (DWORD) atoi ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_MID );
	pWnd->GetWindowText ( szValue, 256 );
	TOMapID.wMainID = (WORD)atoi(szValue);

	pWnd = GetDlgItem ( IDC_EDIT_SID );
	pWnd->GetWindowText ( szValue, 256 );
	TOMapID.wSubID = (WORD)atoi(szValue);

	//	Note : To Gate ID
	pWnd = GetDlgItem ( IDC_EDIT_TOGATEID );
	pWnd->GetWindowText ( szValue, 256 );
	ToGateID = (DWORD) atoi ( szValue );	

	//	Note : 분할 간격.
	pWnd = GetDlgItem ( IDC_EDIT_GEN_DX );
	pWnd->GetWindowText ( szValue, 256 );
	vDIv.x = (float) atof(szValue);

	pWnd = GetDlgItem ( IDC_EDIT_GEN_DY );
	pWnd->GetWindowText ( szValue, 256 );
	vDIv.y = (float) atof(szValue);


	BOOL bExist = FALSE;
	DxLandGate* pCur = pLandGateMan->GetListLandGate ();
	while ( pCur )
	{
		if ( pCur!=m_pLandGate )
		{
			if ( !strcmp(pCur->GetName(),szName) )
			{
				MessageBox ( "이미 같은 이름으로 등록된 개체가 있어서 문제가 발생합니다.", "주의" );
				bExist = TRUE;
				return;
			}

			if ( pCur->GetGateID() == GateID )
			{
				MessageBox ( "이미 같은 ID로 등록된 개체가 있어서 문제가 발생합니다.", "주의" );
				bExist = TRUE;
				return;
			}
		}

		pCur = pCur->m_pNext;
	}

	DxEditBox* pEditBox = pView->GetEditBox();

	m_pLandGate->SetName ( szName );
	m_pLandGate->SetBox ( pEditBox->m_vMax, pEditBox->m_vMin );
	m_pLandGate->SetGate ( dwFlags, GateID, vDIv );
	m_pLandGate->AddToGateMapID( TOMapID, ToGateID, 0, 0);

	pView->DeActiveEditMatrix();
	
	if ( !pSetLandMan->GetNaviMesh()->IsVailedCollision() )
	{
		BOOL bOk = pSetLandMan->CreateNaviMesh ( pView->GetD3DDevice() );

		if ( !bOk || !pSetLandMan->GetNaviMesh()->IsVailedCollision() )
		{
			MessageBox ( "내비게이션 메시가 지정되어 있지 않아서 시작위치군을 계산할 수 없습니다.", "주의" );
			return;
		}
	}

	m_pLandGate->MakeStartPos ( pSetLandMan->GetNaviMesh() );
	pLandGateMan->BuildTree ();

	m_listLandGate.SelectString ( 0, m_pLandGate->GetName() );
	ListtingLandGate ();
}

void CLandGatePage::OnBnClickedButtonAdd()
{
	OnBnClickedButtonApply();

	m_pLandGate = NULL;
	ViewContrls ( FALSE );
	UpdateContrl ();
}

void CLandGatePage::OnBnClickedCheckRend()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton *pButton = (CButton *) GetDlgItem ( IDC_CHECK_REND );
	DxLandGateMan::m_bREND = pButton->GetCheck();
}

void CLandGatePage::OnBnClickedButtonClone()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();

	int nIndex = m_listLandGate.GetCurSel ();
	if ( nIndex==LB_ERR )	return;

	PDXLANDGATE pLandGate = (PDXLANDGATE) m_listLandGate.GetItemData ( nIndex );

	OnBnClickedButtonNew();

	m_pLandGate->SetBox ( pLandGate->GetMax(), pLandGate->GetMin() );
	pView->ActiveEditBox ( pLandGate->GetMax(), pLandGate->GetMin() );
}


