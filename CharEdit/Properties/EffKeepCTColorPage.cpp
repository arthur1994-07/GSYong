// EffKeepCTColorPage.cpp : 구현 파일입니다.
//
#include "stdafx.h"
#include "../CharEdit.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "SheetWithTabChar.h"
#include "../Dialog/ColorPickerDlg.h"

#include "EffKeepCTColorPage.h"
#include ".\effkeepctcolorpage.h"

#pragma warning(disable:4996)

#include "../../SigmaCore/DebugInclude.h"

// CEffKeepCTColorPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CEffKeepCTColorPage, CPropertyPage)
CEffKeepCTColorPage::CEffKeepCTColorPage()
	: CPropertyPage(CEffKeepCTColorPage::IDD)
	, m_pEff(NULL)
	, m_rProperty(NULL)
{
}

CEffKeepCTColorPage::~CEffKeepCTColorPage()
{
}

void CEffKeepCTColorPage::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_LIST_COLOR, m_wndList);
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEffKeepCTColorPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_COLOR_INSERT, OnBnClickedButtonColorInsert)
	ON_BN_CLICKED(IDC_BUTTON_COLOR_STATE, OnBnClickedButtonColorState)
	ON_BN_CLICKED(IDC_BUTTON_COLOR_DELETE, OnBnClickedButtonColorDelete)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_COLOR, OnNMDblclkListColor)
END_MESSAGE_MAP()


// CEffKeepCTColorPage 메시지 처리기입니다.

BOOL CEffKeepCTColorPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	m_wndList.SetColumnHeader(_T("NUM, 50 ; Color, 200"));
	m_wndList.SetGridLines(TRUE);
	m_wndList.SortItems(0,FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CEffKeepCTColorPage::SetInstance( DxEffKeepData* pEff )				
{ 
	if( pEff )
	{
		m_bCreateEff = FALSE;

		m_pEff = pEff; 
		m_rProperty = (EFFKEEP_PROPERTY_CTCOLOR*)m_pEff->GetProperty();
	}
	else
	{
		m_bCreateEff = TRUE;

		m_pEff = DxEffKeepMan::GetInstance().CreateEffect
		(
			DxEffKeepData_CTColor::TYPEID,
			NULL
		);
		m_rProperty = (EFFKEEP_PROPERTY_CTCOLOR*)m_pEff->GetProperty();
	}

	char szValue[256];
	CWnd* pWnd = GetDlgItem( IDC_EDIT_COLOR_TIME );
	_gcvt ( m_rProperty->m_fRoopTime, 7, szValue );
	pWnd->SetWindowText( szValue );

	UpdateColorList();
}

void CEffKeepCTColorPage::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	
	CString strText;
	CWnd* pWnd = GetDlgItem( IDC_EDIT_COLOR_TIME );
	pWnd->GetWindowText( strText );
	m_rProperty->m_fRoopTime = (float)atof( strText.GetString() );

	m_pEff->Create ( pView->GetD3dDevice() );

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

	

	if( m_bCreateEff )
	{
		CCharEditView::g_sEffKeepMain.Insert( m_pEff );		// 효과를 삽입한다.
		m_bCreateEff = FALSE;
	}
}

void CEffKeepCTColorPage::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnBnClickedButtonApply();

	m_pEff = NULL;
	m_rProperty = NULL;
	m_pSheetTab->SetActivePage ( CHARTAB_CHAREFF );
}

void CEffKeepCTColorPage::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if( m_bCreateEff )
	{
		SAFE_DELETE( m_pEff );
		m_rProperty = NULL;
	}
	else
	{
		m_pEff = NULL;
		m_rProperty = NULL;
	}

	m_pSheetTab->SetActivePage( CHARTAB_CHAREFF );
}

void CEffKeepCTColorPage::OnBnClickedButtonColorInsert()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( !m_pEff )	return;

	int ThisR = 255;
	int ThisG = 255;
	int ThisB = 255;

	CColorPickerDlg ColorPicker ( RGB(ThisR,ThisG,ThisB) );
	if ( IDOK == ColorPicker.DoModal() )
	{
		COLORREF SelectColor = ColorPicker.GetColor ();
		DWORD dwColor = 0xff000000 + (GetRValue(SelectColor)<<16) + (GetGValue(SelectColor)<<8) + GetBValue(SelectColor);
		((DxEffKeepData_CTColor*)m_pEff)->InsertColor( dwColor );

		OnBnClickedButtonApply();
	}

	UpdateColorList();
}

void CEffKeepCTColorPage::OnBnClickedButtonColorState()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( !m_pEff )		return;
	if( !m_rProperty )	return;

	POSITION pos = m_wndList.GetFirstSelectedItemPosition();
	int nIndex = m_wndList.GetNextSelectedItem(pos);
	if ( nIndex==LB_ERR )	return;

	int ThisR = (m_rProperty->m_vecColor[nIndex].dwColor & 0xff0000)>>16;
	int ThisG = (m_rProperty->m_vecColor[nIndex].dwColor & 0xff00)>>8;
	int ThisB = (m_rProperty->m_vecColor[nIndex].dwColor & 0xff);

	CColorPickerDlg ColorPicker ( RGB(ThisR,ThisG,ThisB) );
	if ( IDOK == ColorPicker.DoModal() )
	{
		COLORREF SelectColor = ColorPicker.GetColor ();
		DWORD dwColor = 0xff000000 + (GetRValue(SelectColor)<<16) + (GetGValue(SelectColor)<<8) + GetBValue(SelectColor);
		((DxEffKeepData_CTColor*)m_pEff)->ChageColor( nIndex, dwColor );

		UpdateColorList();

		OnBnClickedButtonApply();
	}
}

void CEffKeepCTColorPage::OnBnClickedButtonColorDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( !m_pEff )	return;

	POSITION pos = m_wndList.GetFirstSelectedItemPosition();
	int nIndex = m_wndList.GetNextSelectedItem(pos);
	if ( nIndex==LB_ERR )	return;

	((DxEffKeepData_CTColor*)m_pEff)->DeleteColor( nIndex );

	UpdateColorList();
}

void CEffKeepCTColorPage::UpdateColorList()
{
	if( !m_rProperty )	return;

	m_wndList.DeleteAllItems();

	char szValue[256];
	for( DWORD i=0; i<m_rProperty->m_vecColor.size(); ++i )
	{
		const int IDX = m_wndList.InsertItem(i, _T(""));

		_gcvt ( i, 7, szValue );
		m_wndList.SetItemText( IDX, 0, szValue );

		m_wndList.SetItemText( IDX, 1, _T("") );
		DWORD dwR = (m_rProperty->m_vecColor[i].dwColor & 0xff0000)>>16;
		DWORD dwG = (m_rProperty->m_vecColor[i].dwColor & 0xff00)>>8;
		DWORD dwB = (m_rProperty->m_vecColor[i].dwColor & 0xff);
		m_wndList.SetItemBkColor( i, -1, RGB(dwR, dwG, dwB) );
	}
}

void CEffKeepCTColorPage::OnNMDblclkListColor(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	OnBnClickedButtonColorState();
}