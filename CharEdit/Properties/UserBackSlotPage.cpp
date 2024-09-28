// UserBackSlotPage.cpp : ���� �����Դϴ�.
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

// CUserBackSlotPage ��ȭ �����Դϴ�.

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


// CUserBackSlotPage �޽��� ó�����Դϴ�.

BOOL CUserBackSlotPage::OnSetActive()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();
	DxSkinPiece* pSkinPiece = pView->GetSkinPiece();
	if ( !pSkinPiece )
		return FALSE;	// �̷� ���� ����.

	CWnd* pWnd;
	CButton* pButton;
	CString strText;

	pButton = (CButton*)GetDlgItem( IDC_CHECK_POS_MOVE );
	pButton->SetCheck( FALSE );

	pWnd = GetDlgItem( IDC_EDIT_NAME_BONE );
	pWnd->SetWindowText( pSkinPiece->m_strUserBackSlot.c_str() );

	pButton = (CButton*) GetDlgItem( IDC_CHECK_KEEP_SETTING );
	pButton->SetCheck( pSkinPiece->m_bKeepUserBackSlot );

	// Note : Ŀ���� ���� ���� �۾�
	D3DXMATRIX matWorld = pSkinPiece->GetInfluenceMatrix();
	m_matWorld = matWorld;

	// Note : Ŀ���� ���.  �� ��� ����
	CheckCurserMRS();

	// Note : ���� �����Ѵٰ� �˷��ش�.
	pView->SetRenderTypeBone();

	return CPropertyPage::OnSetActive();
}

BOOL CUserBackSlotPage::OnKillActive()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();
	pView->DeActiveEditMatrix();
	pView->SetRenderTypeDefault();

	return CPropertyPage::OnKillActive();
}

// Matrix�� �˸°� ��ȯ�Ѵ�.
void CUserBackSlotPage::MouseStateLeftDOWN()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();
	DxSkinPiece* pSkinPiece = pView->GetSkinPiece();
	if ( !pSkinPiece )
		return;	// �̷� ���� ����.

	// Note : Matrix�� ���� �� ���� �־��ش�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();
	DxSkinPiece* pSkinPiece = pView->GetSkinPiece();
	if ( !pSkinPiece )
		return;	// �̷� ���� ����.

	pSkinPiece->m_strUserBackSlot = g_strPICK_BONE;

	CWnd* pWnd = GetDlgItem( IDC_EDIT_NAME_BONE );
	pWnd->SetWindowText( pSkinPiece->m_strUserBackSlot.c_str() );

	// Note : Ŀ���� ���� ���� �۾�
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();

	// Note : Ŀ���� ���.  �� ��� ����
	CheckCurserMRS();
}

void CUserBackSlotPage::OnBnClickedCheckKeepSetting()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();
	DxSkinPiece* pSkinPiece = pView->GetSkinPiece();
	if ( !pSkinPiece )
		return;	// �̷� ���� ����.

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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece();
	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice ();

	CString szFilter = _T("User Back Slot (*.ubs)|*.ubs|");

	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(FALSE,_T(".ubs"),NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		this);

	dlg.m_ofn.lpstrInitialDir = DxSkinObject::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		CString sFileName = dlg.GetFileName();
		const TCHAR *szFileName = sFileName.GetString ();

		//	Note : User Back Slot Data ����.
		pSkinPiece->SaveUBS( szFileName );

		sFileName.ReleaseBuffer ();
	}
}

void CUserBackSlotPage::OnBnClickedButtonSettingLoad()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice ();

	CString szFilter = _T("User Back Slot (*.ubs)|*.ubs|");

	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(TRUE,_T(".ubs"),NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		this);

	CString sFileName;
	dlg.m_ofn.lpstrInitialDir = DxSkinObject::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		sFileName = dlg.GetFileName ();
		const TCHAR *szFileName = sFileName.GetString ();

		//	Note : User Back Slot Data �ε�.
		pView->GetSkinPiece()->LoadUBS( szFileName );
		sFileName.ReleaseBuffer ();

		OnSetActive();
	}
}

void CUserBackSlotPage::OnBnClickedButtonOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//m_pEffChar = NULL;
	//m_pSheetTab->m_SkinPiecePage.EffectListting();
	//m_pSheetTabChar->SetActivePage( CHARTAB_CHARPIECE );	

	EffectApply();
	ReturnPage();
}