// UserBackSlotPage.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "../CharEdit.h"
#include "SheetWithTabChar.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "../../enginelib/DxTools/DxInputDevice.h"

#include "UserBackSlotPage.h"


extern TSTRING	g_strPICK_BONE;

// CUserBackSlotPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CUserBackSlotPage, CPropertyPage)

CUserBackSlotPage::CUserBackSlotPage()
	: CEffCharPage(CUserBackSlotPage::IDD)
{

}

CUserBackSlotPage::~CUserBackSlotPage()
{
}

void CUserBackSlotPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CUserBackSlotPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_GETBONENAME, &CUserBackSlotPage::OnBnClickedButtonGetbonename)
	ON_BN_CLICKED(IDC_CHECK_POS_MOVE, &CUserBackSlotPage::OnBnClickedCheckPosMove)
	ON_BN_CLICKED(IDC_CHECK_KEEP_SETTING, &CUserBackSlotPage::OnBnClickedCheckKeepSetting)
	ON_BN_CLICKED(IDC_BUTTON_SETTING_SAVE, &CUserBackSlotPage::OnBnClickedButtonSettingSave)
	ON_BN_CLICKED(IDC_BUTTON_SETTING_LOAD, &CUserBackSlotPage::OnBnClickedButtonSettingLoad)
	ON_BN_CLICKED(IDC_BUTTON_OK, &CUserBackSlotPage::OnBnClickedButtonOk)
END_MESSAGE_MAP()


// CUserBackSlotPage 메시지 처리기입니다.

BOOL CUserBackSlotPage::OnSetActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();
	DxSkinPiece* pSkinPiece = pView->GetSkinPiece();
	if ( !pSkinPiece )
		return FALSE;	// 이럴 수는 없다.

	CWnd* pWnd;
	CButton* pButton;
	CString strText;

	pButton = (CButton*)GetDlgItem( IDC_CHECK_POS_MOVE );
	pButton->SetCheck( FALSE );

	pWnd = GetDlgItem( IDC_EDIT_NAME_BONE );
	pWnd->SetWindowText( pSkinPiece->m_strUserBackSlot.c_str() );

	pButton = (CButton*) GetDlgItem( IDC_CHECK_KEEP_SETTING );
	pButton->SetCheck( pSkinPiece->m_bKeepUserBackSlot );

	// Note : 커서를 쓰기 위한 작업
	D3DXMATRIX matWorld = pSkinPiece->GetInfluenceMatrix();
	m_matWorld = matWorld;

	// Note : 커서를 사용.  미 사용 셋팅
	CheckCurserMRS();

	// Note : 본을 렌더한다고 알려준다.
	pView->SetRenderTypeBone();

	return CPropertyPage::OnSetActive();
}

BOOL CUserBackSlotPage::OnKillActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();
	pView->DeActiveEditMatrix();
	pView->SetRenderTypeDefault();

	return CPropertyPage::OnKillActive();
}

// Matrix를 알맞게 변환한다.
void CUserBackSlotPage::MouseStateLeftDOWN()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();
	DxSkinPiece* pSkinPiece = pView->GetSkinPiece();
	if ( !pSkinPiece )
		return;	// 이럴 수는 없다.

	// Note : Matrix를 얻은 후 값을 넣어준다.
	pSkinPiece->SetChangeMatrix( m_matWorld );
}

void CUserBackSlotPage::UpdateKeyBoardMouse()
{
	if ( DxInputDevice::GetInstance().GetMouseState( DXMOUSE_LEFT ) & DXKEY_DOWNED )
	{
		MouseStateLeftDOWN();
		EffectApply();
	}
}

void CUserBackSlotPage::OnBnClickedButtonGetbonename()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();
	DxSkinPiece* pSkinPiece = pView->GetSkinPiece();
	if ( !pSkinPiece )
		return;	// 이럴 수는 없다.

	pSkinPiece->m_strUserBackSlot = g_strPICK_BONE;

	CWnd* pWnd = GetDlgItem( IDC_EDIT_NAME_BONE );
	pWnd->SetWindowText( pSkinPiece->m_strUserBackSlot.c_str() );

	// Note : 커서를 쓰기 위한 작업
	D3DXMATRIX matWorld = pSkinPiece->GetInfluenceMatrix();
	m_matWorld = matWorld;
}

void CUserBackSlotPage::CheckCurserMRS()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();

	CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK_POS_MOVE);
	if( pButton->GetCheck() )	pView->ActiveEditMatrix( &m_matWorld );
	else						pView->DeActiveEditMatrix();
}

void CUserBackSlotPage::OnBnClickedCheckPosMove()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();

	// Note : 커서를 사용.  미 사용 셋팅
	CheckCurserMRS();
}

void CUserBackSlotPage::OnBnClickedCheckKeepSetting()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();
	DxSkinPiece* pSkinPiece = pView->GetSkinPiece();
	if ( !pSkinPiece )
		return;	// 이럴 수는 없다.

	CButton* pButton = (CButton*) GetDlgItem( IDC_CHECK_KEEP_SETTING );
	if ( pButton->GetCheck() )
	{
		pSkinPiece->m_bKeepUserBackSlot = TRUE;
	}
	else
	{
		pSkinPiece->m_bKeepUserBackSlot = FALSE;
	}
}

void CUserBackSlotPage::OnBnClickedButtonSettingSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece();
	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice ();

	CString szFilter = _T("User Back Slot (*.ubs)|*.ubs|");

	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(FALSE,_T(".ubs"),NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		this);

	dlg.m_ofn.lpstrInitialDir = DxSkinObject::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		CString sFileName = dlg.GetFileName();
		const TCHAR *szFileName = sFileName.GetString ();

		//	Note : User Back Slot Data 저장.
		pSkinPiece->SaveUBS( szFileName );

		sFileName.ReleaseBuffer ();
	}
}

void CUserBackSlotPage::OnBnClickedButtonSettingLoad()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice ();

	CString szFilter = _T("User Back Slot (*.ubs)|*.ubs|");

	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,_T(".ubs"),NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		this);

	CString sFileName;
	dlg.m_ofn.lpstrInitialDir = DxSkinObject::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		sFileName = dlg.GetFileName ();
		const TCHAR *szFileName = sFileName.GetString ();

		//	Note : User Back Slot Data 로드.
		pView->GetSkinPiece()->LoadUBS( szFileName );
		sFileName.ReleaseBuffer ();

		OnSetActive();
	}
}

void CUserBackSlotPage::OnBnClickedButtonOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//m_pEffChar = NULL;
	//m_pSheetTab->m_SkinPiecePage.EffectListting();
	//m_pSheetTabChar->SetActivePage( CHARTAB_CHARPIECE );	

	EffectApply();
	ReturnPage();
}