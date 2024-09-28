// LandMarkPage.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "../LevelEdit.h"

#include "../LevelMainFrm.h"
#include "../LevelEditDoc.h"
#include "../LevelEditView.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/DxTools/DxViewPort.h"
#include "../../RanLogic/Land/GLLandMark.h"
#include "../Util/ColorPickerDlg.h"

#include "LandMarkPage.h"

// CLandMarkPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CLandMarkPage, CPropertyPage)
CLandMarkPage::CLandMarkPage()
	: CPropertyPage(CLandMarkPage::IDD),
	m_pSheetTab(NULL),

	m_bDlgInit(FALSE),
	m_pLandMark(FALSE)
{
}

CLandMarkPage::~CLandMarkPage()
{
}

void CLandMarkPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LANDMARK, m_listLandMark);
}


BEGIN_MESSAGE_MAP(CLandMarkPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_EDIT, OnBnClickedButtonEdit)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnBnClickedButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_NEW, OnBnClickedButtonNew)
	//ON_EN_CHANGE(IDC_EDIT_NAME, OnEnChangeEditName)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_COLOR, &CLandMarkPage::OnBnClickedButtonColor)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, &CLandMarkPage::OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_NAME_CONVERT, &CLandMarkPage::OnBnClickedButtonNameConvert)
END_MESSAGE_MAP()


// CLandMarkPage 메시지 처리기입니다.
void CLandMarkPage::ListtingLandMark ()
{
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
	CLevelEditView* pView = (CLevelEditView *) pFrame->GetActiveView ();    
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();

	CString StrOldSelect;
	int nIndex = m_listLandMark.GetCurSel ();
	if ( nIndex!=LB_ERR )
	{
		m_listLandMark.GetDlgItemText ( nIndex, StrOldSelect );
	}

	m_listLandMark.ResetContent ();

	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	if ( !pGLLandMan )
        return;

	for ( DWORD i=0; i<pGLLandMan->m_vecLandMark.size(); ++i )
	{
		int nIndex(0);
		if ( pGLLandMan->m_vecLandMark[i]->m_strConvertName.empty() )
		{
			nIndex = m_listLandMark.AddString ( pGLLandMan->m_vecLandMark[i]->m_strMARK.c_str() );
		}
		else
		{
			nIndex = m_listLandMark.AddString ( pGLLandMan->m_vecLandMark[i]->m_strConvertName.c_str() );
		}
		m_listLandMark.SetItemData ( nIndex, DWORD_PTR(pGLLandMan->m_vecLandMark[i]) );
	}

	m_listLandMark.SelectString ( 0, StrOldSelect );	
}

void CLandMarkPage::UpdateContrl ()
{
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
	CLevelEditView* pView = (CLevelEditView *) pFrame->GetActiveView ();

	if ( !m_bDlgInit )	return;

	m_pLandMark = NULL;

	ViewContrls ( FALSE );
	ListtingLandMark ();

	pView->SetCurType ( _CUR_SELECT );
}

void CLandMarkPage::ViewContrls ( BOOL bView, BOOL bClear )
{
	if ( !m_bDlgInit )	return;

	DWORD dwButtons[] = { IDC_BUTTON_EDIT, IDC_BUTTON_DEL, IDC_BUTTON_NEW, IDC_BUTTON_CLONE };
	DWORD dwNumButtons = sizeof(dwButtons) / sizeof(DWORD);

	DWORD dwContrls[] = { IDC_EDIT_NAME, IDC_BUTTON_COLOR, IDC_BUTTON_APPLY, IDC_BUTTON_ADD, IDC_EDIT_NAME_INDEX, IDC_EDIT_NAME_GAMEINTEXT, IDC_BUTTON_NAME_CONVERT };
	DWORD dwNumContrls = sizeof(dwContrls) / sizeof(DWORD);

	CWnd *pWnd;

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
	}
	else if ( m_pLandMark )
	{
		pWnd = GetDlgItem ( IDC_EDIT_NAME );
		pWnd->SetWindowText ( m_pLandMark->m_strMARK.c_str() );

		CString Str;
		Str.Format ( "%d", m_pLandMark->m_nKeyWordIndex );
		pWnd = GetDlgItem ( IDC_EDIT_NAME_INDEX );
		pWnd->SetWindowText ( Str );

		pWnd = GetDlgItem ( IDC_EDIT_NAME_GAMEINTEXT );
		pWnd->SetWindowText ( m_pLandMark->m_strConvertName.c_str() );

		int ThisR = ( m_pLandMark->m_dwColor & 0xff0000 ) >> 16;
		int ThisG = ( m_pLandMark->m_dwColor & 0xff00 ) >> 8;
		int ThisB = ( m_pLandMark->m_dwColor & 0xff );

		COLORREF cColorTEXT	= RGB(0, 0, 0);
		COLORREF cColorGRAY = RGB(ThisR, ThisG, ThisB);
		m_Color.SetColour ( cColorTEXT, cColorGRAY );
	}
}

void CLandMarkPage::OnBnClickedButtonEdit()
{
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
	CLevelEditView* pView = (CLevelEditView *) pFrame->GetActiveView ();

	int nIndex = m_listLandMark.GetCurSel ();
	if ( nIndex==LB_ERR )	return;
	
	m_pLandMark = (PLANDMARK) m_listLandMark.GetItemData ( nIndex );
	pView->ActiveEditMatrix ( m_pLandMark );

	ViewContrls ( TRUE );

	DxViewPort::GetInstance().CameraJump ( m_pLandMark->m_pAffineParts->vTrans );
}

void CLandMarkPage::OnBnClickedButtonDel()
{
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
	CLevelEditView* pView = (CLevelEditView *) pFrame->GetActiveView ();    
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	if ( !pGLLandMan )
        return;

	int nIndex = m_listLandMark.GetCurSel ();
	if ( nIndex==LB_ERR )	return;

	PLANDMARK pLandMark = (PLANDMARK) m_listLandMark.GetItemData ( nIndex );

	pGLLandMan->DelLandMark ( pLandMark );
	//pGLLandMan->BuildLandMarkTree ();

	//	현제 편집중인 이팩트일경우.
	if ( pLandMark== m_pLandMark )
	{
		m_pLandMark = NULL;
		ViewContrls ( FALSE );
		UpdateContrl ();
	}
	else
	{
		ListtingLandMark ();
	}
}

void CLandMarkPage::OnBnClickedButtonNew()
{
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
	CLevelEditView* pView = (CLevelEditView *) pFrame->GetActiveView ();    
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	if (!pGLLandMan)
        return;

	GLLANDMARK *pLandMark;
	pLandMark = new GLLANDMARK;

	char szNewName[128];
	
	int i=0;
	do
	{
		sprintf ( szNewName, "%s[%d]", "Zone", i++ );
	} while ( pGLLandMan->FindLandMark(szNewName) );

	pLandMark->m_strMARK = szNewName;

	const D3DXVECTOR3& vCollPos = pView->GetCollisionPos();
	DXAFFINEPARTS Affine;
	Affine.vTrans = vCollPos;
	pLandMark->SetAffineValue ( &Affine );

	m_pLandMark = pLandMark;
	pView->ActiveEditMatrix ( m_pLandMark );
	pGLLandMan->AddLandMark ( m_pLandMark );
	//pGLLandMan->BuildLandMarkTree ();

	ViewContrls ( TRUE );
	ListtingLandMark ();

	DxViewPort::GetInstance().CameraJump ( Affine.vTrans );
}

//void CLandMarkPage::OnEnChangeEditName()
//{
//	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
//	CLevelEditView* pView = (CLevelEditView *) pFrame->GetActiveView ();
//
//	int nIndex = m_listLandMark.GetCurSel ();
//	if ( nIndex==LB_ERR )	return;
//	
//	m_pLandMark = (GLLANDMARK*) m_listLandMark.GetItemData ( nIndex );
//	pView->ActiveEditMatrix ( m_pLandMark );
//
//	ViewContrls ( TRUE );
//
//	DxViewPort::GetInstance().CameraJump ( m_pLandMark->m_pAffineParts->vTrans );
//}

void CLandMarkPage::OnBnClickedButtonAdd()
{
	OnBnClickedButtonNameConvert();

	m_pLandMark = NULL;
	ViewContrls ( FALSE );
	UpdateContrl ();
}

BOOL CLandMarkPage::OnKillActive()
{
	m_pLandMark = NULL;
	ViewContrls ( FALSE );
	UpdateContrl ();

	return CPropertyPage::OnKillActive();
}

BOOL CLandMarkPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_bDlgInit = TRUE;
	ViewContrls ( FALSE );
	UpdateContrl ();

	m_Color.SubclassDlgItem ( IDC_BUTTON_COLOR, this );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CLandMarkPage::OnBnClickedButtonColor()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
	CLevelEditView* pView = (CLevelEditView *) pFrame->GetActiveView ();
	//pView->SetCurClick( FALSE );

	int ThisR = ( m_pLandMark->m_dwColor & 0xff0000 ) >> 16;
	int ThisG = ( m_pLandMark->m_dwColor & 0xff00 ) >> 8;
	int ThisB = ( m_pLandMark->m_dwColor & 0xff );

	CColorPickerDlg ColorPicker ( RGB(ThisR,ThisG,ThisB) );
	if ( IDOK == ColorPicker.DoModal() )
	{
		COLORREF SelectColor = ColorPicker.GetColor ();
		ThisR = GetRValue(SelectColor);
		ThisG = GetGValue(SelectColor);
		ThisB = GetBValue(SelectColor);
		m_pLandMark->m_dwColor = 0xff000000;
		m_pLandMark->m_dwColor += ThisR << 16;
		m_pLandMark->m_dwColor += ThisG << 8;
		m_pLandMark->m_dwColor += ThisB;
	}

	COLORREF cColorTEXT	= RGB(0, 0, 0);
	COLORREF cColorGRAY = RGB(ThisR, ThisG, ThisB);
	m_Color.SetColour ( cColorTEXT, cColorGRAY );

	//pView->SetCurClick( TRUE );
}

void CLandMarkPage::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
	CLevelEditView* pView = (CLevelEditView *) pFrame->GetActiveView ();    
	GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	if (!pGLLandMan)
		return;

	CWnd *pWnd;
	CString strMARK;

	pWnd = GetDlgItem ( IDC_EDIT_NAME );
	pWnd->GetWindowText ( strMARK );

	if ( strMARK.IsEmpty() )
	{
		MessageBox ( "It did not set a name.", "Caution" );
		return;
	}

	CString Str;
	pWnd = GetDlgItem ( IDC_EDIT_NAME_INDEX );
	pWnd->GetWindowText ( Str );

	m_pLandMark->m_strMARK = strMARK.GetString();
	m_pLandMark->m_nKeyWordIndex = atoi ( Str.GetString() );
	m_pLandMark->CALC_BOX();

	//pGLLandMan->BuildLandMarkTree ();

	ListtingLandMark ();
}

BOOL CLandMarkPage::OnSetActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CPropertyPage::OnSetActive();
}

void CLandMarkPage::OnBnClickedButtonNameConvert()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnBnClickedButtonApply();

	m_pLandMark->ConvertName();

	CWnd* pWnd = GetDlgItem ( IDC_EDIT_NAME_GAMEINTEXT );
	pWnd->SetWindowText ( m_pLandMark->m_strConvertName.c_str() );

	ListtingLandMark ();
}
