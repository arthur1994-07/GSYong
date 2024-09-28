// EffLine2BonePage.cpp : 구현 파일입니다.
//
#include "stdafx.h"
#include "../CharEdit.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "../Dialog/ColorPickerDlg.h"
#include "SheetWithTabChar.h"

#ifdef DEF_SKIN_SYNC
#include "SheetWithTabPiece.h"
#include "EffLine2BoneDPage.h"
#endif

#include "EffLine2BonePage.h"
#include ".\effline2bonepage.h"

extern std::string	g_strPICK_BONE;

#pragma warning(disable:4996)

#include "../../SigmaCore/DebugInclude.h"

// CEffLine2BonePage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CEffLine2BonePage, CPropertyPage)
CEffLine2BonePage::CEffLine2BonePage()
	: CEffCharPage(CEffLine2BonePage::IDD)
{
	
}

CEffLine2BonePage::~CEffLine2BonePage()
{
}

void CEffLine2BonePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LINEBONE, m_ctrlLINE);
}


BEGIN_MESSAGE_MAP(CEffLine2BonePage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_GET1, OnBnClickedButtonGet1)
	ON_BN_CLICKED(IDC_BUTTON_GET2, OnBnClickedButtonGet2)
	ON_BN_CLICKED(IDC_BUTTON_INSERT, OnBnClickedButtonInsert)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnBnClickedButtonDelete)
	ON_NOTIFY(NM_CLICK, IDC_LIST_LINEBONE, OnNMClickListLinebone)
	ON_BN_CLICKED(IDC_BUTTON_SETTING, OnBnClickedButtonSetting)
END_MESSAGE_MAP()


// CEffLine2BonePage 메시지 처리기입니다.

BOOL CEffLine2BonePage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	RECT listRect;
	m_ctrlLINE.GetClientRect(&listRect);
	int nListWidth = ( listRect.right - listRect.left );
	int nVScrollBarWidth = ::GetSystemMetrics(SM_CXVSCROLL); 
	int nColWidth[3] = { int(nListWidth*0.15f), int(nListWidth*0.43f), int(nListWidth*0.43f) };

	m_ctrlLINE.SetExtendedStyle ( m_ctrlLINE.GetExtendedStyle() |
		LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	m_ctrlLINE.InsertColumn ( 0, _T("ID"),		LVCFMT_CENTER, nColWidth[0] );
	m_ctrlLINE.InsertColumn ( 1, _T("Name"),	LVCFMT_CENTER, nColWidth[1] );
	m_ctrlLINE.InsertColumn ( 2, _T("Name"),	LVCFMT_CENTER, nColWidth[2] );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CEffLine2BonePage::SetProperty( EFFCHAR_PROPERTY *pProp )
{
	if( pProp )	m_Property = *((EFFCHAR_PROPERTY_LINE2BONEEFF*)pProp);
	else
	{
		EFFCHAR_PROPERTY_LINE2BONEEFF sProp;
		m_Property = sProp;
	}

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();


	if ( !m_pEffChar )
	{
		m_pEffChar = (DxEffCharLine2BoneEff*)DxEffCharMan::GetInstance().CreateEffect
		(
			DxEffCharLine2BoneEff::TYPEID,
			&m_Property,
			NULL,
			pSkinPiece
		);

		if ( !m_pEffChar )
		{
			MessageBox ( _T("이팩트 생성에 실패하였습니다."), _T("ERROR"), MB_OK );
			return;
		}

		pSkinPiece->AddEffList ( m_pEffChar );
	}
	else
	{
		m_pEffChar->SetProperty ( &m_Property );
		m_pEffChar->Create ( pView->GetD3dDevice() );
	}

	UpdateLineLIST();

	// Note : 본을 렌더한다고 알려준다.
	pView->SetRenderTypeBone();
}

void CEffLine2BonePage::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if( !m_pEffChar )		return;
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	m_pEffChar->SetProperty ( &m_Property );
	m_pEffChar->Create ( pView->GetD3dDevice() );

    EffectApply();
}

void CEffLine2BonePage::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnBnClickedButtonApply();

    ReturnPage();

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	pView->SetRenderTypeDefault();
}

void CEffLine2BonePage::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    ReturnPage();

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	pView->SetRenderTypeDefault();
}

void CEffLine2BonePage::OnBnClickedButtonGet1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();

	CWnd* pWnd = GetDlgItem( IDC_EDIT_BONE1 );
	pWnd->SetWindowText( g_strPICK_BONE.c_str() );
}

void CEffLine2BonePage::OnBnClickedButtonGet2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();

	CWnd* pWnd = GetDlgItem( IDC_EDIT_BONE2 );
	pWnd->SetWindowText( g_strPICK_BONE.c_str() );
}

void CEffLine2BonePage::OnBnClickedButtonInsert()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CWnd* pWnd;

	CString strBone1;
	pWnd = GetDlgItem( IDC_EDIT_BONE1 );
	pWnd->GetWindowText( strBone1 );
	if( strBone1.IsEmpty() )
	{
		// Note : 경고 메시지.
		MessageBox ( _T("Line List 추가에 실패 하였습니다."), _T("ERROR"), MB_OK );
		return;
	}

	CString strBone2;
	pWnd = GetDlgItem( IDC_EDIT_BONE2 );
	pWnd->GetWindowText( strBone2 );
	if( strBone2.IsEmpty() )
	{
		// Note : 경고 메시지.
		MessageBox ( _T("Line List 추가에 실패 하였습니다."), _T("ERROR"), MB_OK );
		return;
	}

    DxEffCharLine2BoneEff* pEffChar = dynamic_cast<DxEffCharLine2BoneEff*>(m_pEffChar);

	pEffChar->InsertLine( strBone1.GetString(), strBone2.GetString() );

	UpdateLineLIST();
}

void CEffLine2BonePage::OnBnClickedButtonDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	POSITION pos = m_ctrlLINE.GetFirstSelectedItemPosition();
	int nIndex = m_ctrlLINE.GetNextSelectedItem(pos);
	if ( nIndex==LB_ERR )	return;

    DxEffCharLine2BoneEff* pEffChar = dynamic_cast<DxEffCharLine2BoneEff*>(m_pEffChar);

	pEffChar->DeleteLine( nIndex );

	UpdateLineLIST();
}

void CEffLine2BonePage::UpdateLineLIST()
{
	m_ctrlLINE.DeleteAllItems();

    DxEffCharLine2BoneEff* pEffChar = dynamic_cast<DxEffCharLine2BoneEff*>(m_pEffChar);

	char szValue[128];
	TSTRING strBoneName1;
	TSTRING strBoneName2;
	DWORD dwSize = pEffChar->GetLineSize();
	for( DWORD i=0; i<dwSize; ++i )
	{
		pEffChar->GetBoneName( i, strBoneName1, strBoneName2 );

		_gcvt( i, 7, szValue );
		m_ctrlLINE.InsertItem( i, szValue );

		m_ctrlLINE.SetItemText( i, 1, strBoneName1.c_str() );
		m_ctrlLINE.SetItemText( i, 2, strBoneName1.c_str() );
	}
}

void CEffLine2BonePage::OnNMClickListLinebone(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	POSITION pos = m_ctrlLINE.GetFirstSelectedItemPosition();
	int nIndex = m_ctrlLINE.GetNextSelectedItem(pos);
	if ( nIndex==LB_ERR )	return;

    DxEffCharLine2BoneEff* pEffChar = dynamic_cast<DxEffCharLine2BoneEff*>(m_pEffChar);

	pEffChar->SelectLine( nIndex );
}

void CEffLine2BonePage::OnBnClickedButtonSetting()
{
#ifndef DEF_SKIN_SYNC
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->SetActivePageTab( CHARTAB_EFFECT_LINE2BONED );
	m_pSheetTab->m_EffLine2BoneD.SetProperty( m_pEffChar, &m_Property );
#else
    CPropertySheet* pParent = static_cast<CPropertySheet*>(GetParent());

    if (pParent)
    {
        if (pParent->SetActivePage(CsheetWithTabPiece::EMEFFPAGE_LINE2BONED))
        {
            CEffLine2BoneDPage* pPage = static_cast<CEffLine2BoneDPage*>(pParent->GetActivePage());

            if (pPage)
            {
                pPage->SetProperty(m_pEffChar, &m_Property);
            }
        }
    }
#endif
}
