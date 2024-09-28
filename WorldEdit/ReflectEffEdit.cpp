// ReflectEffEdit.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "WorldEdit.h"
#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"

#include "SheetWithTab.h"

#include "../EngineLib/DxTools/DxViewPort.h"
#include "../EngineLib/DxEffect/Frame/DxEffectReflect.h"

#include "ReflectEffEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ReflectEffEdit ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CReflectEffEdit, CPropertyPage)
CReflectEffEdit::CReflectEffEdit()
	: CPropertyPage(CReflectEffEdit::IDD),
	m_pReflectEff(NULL)
{
}

CReflectEffEdit::~CReflectEffEdit()
{
}

void CReflectEffEdit::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CReflectEffEdit, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
END_MESSAGE_MAP()


// CReflectEffEdit �޽��� ó�����Դϴ�.
void CReflectEffEdit::SetInstance ( DxEffectReflect *pReflectEff )
{
	m_pReflectEff = pReflectEff;
	m_OldReflectProperty = m_ReflectProperty = m_pReflectEff->GetProperty();

	//	Note : �� ����.
	//
	CWnd *pWnd = GetDlgItem ( IDC_EDIT_FRAME_EFF_NAME );
	pWnd->SetWindowText ( m_pReflectEff->GetName() );

	char	szValue[256];
	pWnd = GetDlgItem( IDC_EDIT_POWER );
	_gcvt ( m_ReflectProperty.m_dwPower, 7, szValue );
	pWnd->SetWindowText( szValue );
}

void CReflectEffEdit::OnBnClickedButtonApply()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString strText;
	CWnd *pWnd = GetDlgItem( IDC_EDIT_POWER );
	pWnd->GetWindowText( strText );
	m_ReflectProperty.m_dwPower = atoi( strText.GetString() );

	//	Note : �Ӽ��� ����.
	//
	m_pReflectEff->SetProperty ( m_ReflectProperty );
	m_pReflectEff->CleanUp ();

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	m_pReflectEff->Create ( pView->GetD3DDevice() );
}

void CReflectEffEdit::OnBnClickedButtonOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_pSheetTab->SetActivePage ( 0 );

	//	Note : �Ӽ��� ����.
	//
	OnBnClickedButtonApply();
}

void CReflectEffEdit::OnBnClickedButtonCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_pSheetTab->SetActivePage ( 0 );

	//	Note : ���Ӽ��� �ǵ���.
	//
	m_pReflectEff->SetProperty ( m_OldReflectProperty );
	m_pReflectEff->CleanUp ();

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	m_pReflectEff->Create ( pView->GetD3DDevice() );
}