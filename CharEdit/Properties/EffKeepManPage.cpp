// EffKeepManPage.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "../CharEdit.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "SheetWithTabChar.h"

#include "../EngineLib/DxEffect/Char/DxEffCharData.h"
#include "../EngineLib/DxEffect/EffKeep/DxEffKeep.h"

#include "EffKeepManPage.h"
#include ".\effkeepmanpage.h"


// CEffKeepManPage ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CEffKeepManPage, CPropertyPage)
CEffKeepManPage::CEffKeepManPage()
	: CPropertyPage(CEffKeepManPage::IDD)
{
}

CEffKeepManPage::~CEffKeepManPage()
{
}

void CEffKeepManPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_TYPE, m_EffType);
	DDX_Control(pDX, IDC_LIST_EFF, m_EffList);
}


BEGIN_MESSAGE_MAP(CEffKeepManPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_LOAD, OnBnClickedButtonLoad)
	ON_BN_CLICKED(IDC_BUTTON_EFF_STATE, OnBnClickedButtonEffState)
	ON_BN_CLICKED(IDC_BUTTON_EFF_DELETE, OnBnClickedButtonEffDelete)
	ON_BN_CLICKED(IDC_BUTTON_EFF_INSERT, OnBnClickedButtonEffInsert)
	ON_LBN_DBLCLK(IDC_LIST_EFF, OnLbnDblclkListEff)
END_MESSAGE_MAP()


// CEffKeepManPage �޽��� ó�����Դϴ�.

BOOL CEffKeepManPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	int nIndex;
	CString	strText;

	m_EffType.ResetContent();

	DxEffKeepType* pType = DxEffKeepMan::GetInstance().GetEffectList();
	while ( pType )
	{
		strText.Format ( pType->NAME );
		nIndex = m_EffType.AddString ( strText );
		m_EffType.SetItemData ( nIndex, pType->TYPEID );

		pType = pType->pNext;
	}

	if ( pType )
	{
		m_EffType.SetCurSel ( 0 );
	}

	EffectListting();

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

BOOL CEffKeepManPage::OnSetActive()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	EffectListting();

	return CPropertyPage::OnSetActive();
}

void CEffKeepManPage::OnBnClickedButtonSave()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = _T("Skin effect |*.effskin_b|");
	
	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg ( FALSE,_T(".effskin_b"),NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, this );
	dlg.m_ofn.lpstrInitialDir = DxEffcharDataMan::GetInstance().GetPath();

	if ( dlg.DoModal()==IDOK )
	{
		CString sFileName = dlg.GetFileName();
		sFileName.MakeLower ();
		const TCHAR *szFileName = sFileName.GetString();

		CCharEditView::g_sEffKeepMain.Save( szFileName );

		CWnd* pWnd = GetDlgItem( IDC_EDIT_NAME );
		pWnd->SetWindowText( szFileName );
	}
}

void CEffKeepManPage::OnBnClickedButtonLoad()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = _T("Skin effect |*.effskin_b|");
	
	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg ( TRUE,_T(".effskin_b"),NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, this );
	dlg.m_ofn.lpstrInitialDir = DxEffcharDataMan::GetInstance().GetPath();

	if ( dlg.DoModal()==IDOK )
	{
		CString sFileName = dlg.GetFileName();
		sFileName.MakeLower ();
		const TCHAR *szFileName = sFileName.GetString();

		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();	

		CCharEditView::g_sEffKeepMain.Load( szFileName, pView->GetD3dDevice() );

		CWnd* pWnd = GetDlgItem( IDC_EDIT_NAME );
		pWnd->SetWindowText( szFileName );
	}

	EffectListting ();
}

void CEffKeepManPage::OnBnClickedButtonEffState()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int nIndex = m_EffList.GetCurSel ();
	if ( nIndex == LB_ERR )		return;

	DxEffKeepData* pEff = (DxEffKeepData*) m_EffList.GetItemData ( nIndex );

	assert(pEff);


	switch( pEff->GetTypeID() )
	{
	case EFFKT_CT_EDGE:	
		m_pSheetTab->SetActivePageTab( CHARTAB_EFFKEEP_CTEDGE );
		m_pSheetTab->m_EffKeepCTEdge.SetInstance( pEff );
		break;
	case EFFKT_CT_SHADE:
		m_pSheetTab->SetActivePageTab( CHARTAB_EFFKEEP_CTSHADE );
		m_pSheetTab->m_EffKeepCTShade.SetInstance( pEff );
		break;
	case EFFKT_CT_COLOR:
		m_pSheetTab->SetActivePageTab( CHARTAB_EFFKEEP_CTCOLOR );
		m_pSheetTab->m_EffKeepCTColor.SetInstance( pEff );
		break;
	};

	EffectListting ();
}

void CEffKeepManPage::OnBnClickedButtonEffDelete()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	DxEffKeepDataMain::LIST_EFFKEEPDATA_ITER iter = CCharEditView::g_sEffKeepMain.m_listEffKeepData.begin();
	DxEffKeepDataMain::LIST_EFFKEEPDATA_ITER iter_end = CCharEditView::g_sEffKeepMain.m_listEffKeepData.end();

	int	nIndex = m_EffList.GetCurSel();
	if ( nIndex == LB_ERR )		return;

	int nCount = 0;
	for ( ; iter!=iter_end; ++nCount )
	{
		DxEffKeepDataMain::LIST_EFFKEEPDATA_ITER iter_del = iter++;

		if( nCount == nIndex )
		{
			SAFE_DELETE ( (*iter_del) );
			CCharEditView::g_sEffKeepMain.m_listEffKeepData.erase(iter_del);

			continue;
		}
	}

	EffectListting ();
}

void CEffKeepManPage::OnBnClickedButtonEffInsert()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	assert(m_pSheetTab&&_T("SheetTab�� �����Ͱ� �������� ����."));
	
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();	
	DxSkinChar* pSkinChar = pView->GetSkinChar ();

	int nIndex = m_EffType.GetCurSel();
	if ( nIndex == LB_ERR )		return;

	DWORD dwData = m_EffType.GetItemData ( nIndex );
	if ( dwData == LB_ERR )		return;
	switch ( dwData )
	{
	case EFFKT_CT_EDGE:
		m_pSheetTab->SetActivePageTab( CHARTAB_EFFKEEP_CTEDGE );
		m_pSheetTab->m_EffKeepCTEdge.SetInstance( NULL );
		break;
	case EFFKT_CT_SHADE:
		m_pSheetTab->SetActivePageTab( CHARTAB_EFFKEEP_CTSHADE );
		m_pSheetTab->m_EffKeepCTShade.SetInstance( NULL );
		break;
	case EFFKT_CT_COLOR:
		m_pSheetTab->SetActivePageTab( CHARTAB_EFFKEEP_CTCOLOR );
		m_pSheetTab->m_EffKeepCTColor.SetInstance( NULL );
		break;
	};

	EffectListting ();
}

void CEffKeepManPage::EffectListting()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	DxEffKeepDataMain::LIST_EFFKEEPDATA_ITER iter = CCharEditView::g_sEffKeepMain.m_listEffKeepData.begin();
	DxEffKeepDataMain::LIST_EFFKEEPDATA_ITER iter_end = CCharEditView::g_sEffKeepMain.m_listEffKeepData.end();

	int nCount = 0;
	DWORD	dwFrame_1;
	DWORD	dwFrame_2;
	CString Str;
	m_EffList.ResetContent();
	for ( ; iter!=iter_end; ++iter, ++nCount )
	{
		dwFrame_1 = 0;
		dwFrame_2 = 0;

		Str.Format ( _T("[%s] "), (*iter)->GetName() );

		m_EffList.AddString ( Str );
		m_EffList.SetItemData ( nCount, (DWORD) (*iter) );
	}

	if( CCharEditView::m_emObjType == EOBJ_SKINCHAR )
	{
		if( pView->GetSkinChar() )
		{
			pView->GetSkinChar()->Reset();
			pView->GetSkinChar()->CreateEff( &CCharEditView::g_sEffKeepMain );
		}
	}else if( CCharEditView::m_emObjType == EOBJ_ATTBONE )
	{
		if( pView->GetAttBone() )
		{
			pView->GetAttBone()->Reset();
			pView->GetAttBone()->CreateEff( &CCharEditView::g_sEffKeepMain );
		}
	}else if( CCharEditView::m_emObjType == EOBJ_VEHICLE )
	{
		if( pView->GetVehicle() )
		{
			pView->GetVehicle()->Reset();
			pView->GetVehicle()->CreateEff( &CCharEditView::g_sEffKeepMain );
		}
	}
}

void CEffKeepManPage::OnLbnDblclkListEff()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	OnBnClickedButtonEffState();
}
