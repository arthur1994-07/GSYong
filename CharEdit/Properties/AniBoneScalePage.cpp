// AniBoneScalePage.cpp : 구현 파일입니다.
//
#include "stdafx.h"
#include "../CharEdit.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "SheetWithTabAnim.h"
#include "SheetWithTabChar.h"

#include "../EngineLib/DxMeshs/SkinMesh/DxSkinAniControl.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinAnimation.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxAniBoneScale.h"
#include "../EngineLib/DxMeshs/SkinMesh/SANIMCONINFO.h"

#include "../MfcExLib/ExLibs/EtcFunction.h"

#include "AniBoneScalePage.h"
#include ".\anibonescalepage.h"

extern TSTRING	g_strPICK_BONE;

#pragma warning(disable:4996)

#include "../../SigmaCore/DebugInclude.h"

// CAniBoneScalePage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAniBoneScalePage, CPropertyPage)
CAniBoneScalePage::CAniBoneScalePage()
	: CPropertyPage(CAniBoneScalePage::IDD)
	, m_pSheetTabChar(NULL)
	, m_pSheetTabAni(NULL)
{
}

CAniBoneScalePage::~CAniBoneScalePage()
{
}

void CAniBoneScalePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_BONENAME, m_listBONENAME);
	DDX_Control(pDX, IDC_LIST_KEYSCALE, m_ctrlKEYSCALE);
}


BEGIN_MESSAGE_MAP(CAniBoneScalePage, CPropertyPage)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_GETBONENAME, OnBnClickedButtonGetbonename)
	ON_BN_CLICKED(IDC_BUTTON_INSERT, OnBnClickedButtonInsert)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_KEYSCALE_MODIFY, OnBnClickedButtonKeyscaleModify)
	ON_BN_CLICKED(IDC_BUTTON_KEYSCALE_DELETE2, OnBnClickedButtonKeyscaleDelete2)
	ON_LBN_SELCHANGE(IDC_LIST_BONENAME, OnLbnSelchangeListBonename)
	ON_BN_CLICKED(IDC_BUTTON_KEYSCALE_INSERT, OnBnClickedButtonKeyscaleInsert)
	ON_NOTIFY(NM_CLICK, IDC_LIST_KEYSCALE, OnNMClickListKeyscale)
END_MESSAGE_MAP()


// CAniBoneScalePage 메시지 처리기입니다.

BOOL CAniBoneScalePage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	RECT listRect;
	m_ctrlKEYSCALE.GetClientRect(&listRect);
	int nListWidth = ( listRect.right - listRect.left );
	int nVScrollBarWidth = ::GetSystemMetrics(SM_CXVSCROLL); 
	int nColWidth[5] = { int(nListWidth*0.4f), int(nListWidth*0.2f), int(nListWidth*0.2f), int(nListWidth*0.2f) };

	m_ctrlKEYSCALE.SetExtendedStyle ( m_ctrlKEYSCALE.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	m_ctrlKEYSCALE.InsertColumn ( 0, _T("Key"),	LVCFMT_CENTER, nColWidth[0] );
	m_ctrlKEYSCALE.InsertColumn ( 1, _T("X"),	LVCFMT_CENTER, nColWidth[1] );
	m_ctrlKEYSCALE.InsertColumn ( 2, _T("Y"),	LVCFMT_CENTER, nColWidth[2] );
	m_ctrlKEYSCALE.InsertColumn ( 3, _T("Z"),	LVCFMT_CENTER, nColWidth[3] );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BOOL CAniBoneScalePage::OnSetActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	SetWin_Num_float( this, IDC_EDIT_SCALE1, 1.0f );
	SetWin_Num_float( this, IDC_EDIT_SCALE2, 1.0f );
	SetWin_Num_float( this, IDC_EDIT_SCALE3, 1.0f );

	// Note : 본을 렌더한다고 알려준다.
	pView->SetRenderTypeBone();

	return CPropertyPage::OnSetActive();
}

BOOL CAniBoneScalePage::OnKillActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	pView->SetRenderTypeDefault();

	return CPropertyPage::OnKillActive();
}

void CAniBoneScalePage::SetInstance( const TCHAR* szName, CsheetWithTabAnim* pSheetTabAni )
{
	m_sAnimName = szName;
	m_pSheetTabAni = pSheetTabAni;

	UpdateBoneNameLIST();

	int nSize = m_listBONENAME.GetCount();
	if( nSize>=1 )
	{
		m_listBONENAME.SetCurSel( 0 );
	}

	UpdateKeyScaleCTRL();
}

void CAniBoneScalePage::OnBnClickedOk()
{

	switch ( CCharEditView::m_emObjType )
	{
	case EOBJ_SKINCHAR: m_pSheetTabChar->SetActivePage( CHARTAB_CHARFILE    ); break;
	case EOBJ_ATTBONE : m_pSheetTabChar->SetActivePage( CHARTAB_ATTBONEFILE ); break; 
	case EOBJ_VEHICLE : m_pSheetTabChar->SetActivePage( CHARTAB_VEHICLE     ); break; 
	}
}

void CAniBoneScalePage::OnBnClickedButtonGetbonename()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();

	CWnd* pWnd = GetDlgItem( IDC_EDIT_BONE_NAME );
	pWnd->SetWindowText( g_strPICK_BONE.c_str() );
}

void CAniBoneScalePage::OnBnClickedButtonInsert()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	ANIMCONTNODE *pAnimNodeCur = m_pSheetTabAni->GetSkinAniContrl()->findanicontnode_none_const ( m_sAnimName.GetString() );
	if ( !pAnimNodeCur )	return;
	DxSkinAnimation* pAnimCont = pAnimNodeCur->m_pDxSkinAni;
	if ( !pAnimCont )		return;

	CString strText;
	CWnd* pWnd = GetDlgItem( IDC_EDIT_BONE_NAME );
	pWnd->GetWindowText( strText );
	if( strText.IsEmpty() ) return;

	pAnimCont->m_pAniScale->InsertAniScale( strText.GetString() );

	// Note : List 정리
	UpdateBoneNameLIST();
	UpdateKeyScaleCTRL();

	CString strBoneName = GetWin_Text( this, IDC_EDIT_BONE_NAME );
	if( !strBoneName.IsEmpty() )
	{
		DxAniScale::MAP_ANISCALEMAIN_ITER iter = pAnimCont->m_pAniScale->m_mapAniScaleMain.begin();

        int nIndex = 0;
		for( nIndex = 0; iter!=pAnimCont->m_pAniScale->m_mapAniScaleMain.end(); ++iter, nIndex++ )
		{
			CString strTemp = iter->first.c_str();
			if( strTemp == strBoneName )
			{
				break;
			}
		}
		m_listBONENAME.SetCurSel( nIndex );
	}else{
		int nSize = m_listBONENAME.GetCount();
		m_listBONENAME.SetCurSel( nSize-1 );
	}
}

void CAniBoneScalePage::OnBnClickedButtonDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	ANIMCONTNODE *pAnimNodeCur = m_pSheetTabAni->GetSkinAniContrl()->findanicontnode_none_const ( m_sAnimName.GetString() );
	if ( !pAnimNodeCur )	return;
	DxSkinAnimation* pAnimCont = pAnimNodeCur->m_pDxSkinAni;
	if ( !pAnimCont )		return;

	int nIndex = m_listBONENAME.GetCurSel();
	if( nIndex==LB_ERR )	return;

	pAnimCont->m_pAniScale->DeleteAniScale( nIndex );

	// Note : List 정리
	UpdateBoneNameLIST();
	UpdateKeyScaleCTRL();

	// m_listPoint.SetCurSel( nCur );
}

void CAniBoneScalePage::OnLbnSelchangeListBonename()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateKeyScaleCTRL();
}

void CAniBoneScalePage::OnNMClickListKeyscale(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	//ANIMCONTNODE *pAnimNodeCur = m_pSheetTabAni->GetSkinAniContrl()->findanicontnode ( m_sAnimName.GetString() );
	//if ( !pAnimNodeCur )	return;
	//DxSkinAnimation* pAnimCont = pAnimNodeCur->m_pDxSkinAni;
	//if ( !pAnimCont )		return;

	//int nIndex = m_listBONENAME.GetCurSel();
	//if( nIndex==LB_ERR )	return;

	//DxAniScaleMAIN* pAniScale = pAnimCont->m_pAniScale->GetAniScaleMAIN( nIndex );
	//if( !pAniScale )	return;

	//CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	//CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	//DxSkinChar* pSkinChar = pView->GetSkinChar ();
	//if( !pSkinChar )	return;

	//char szName[256];
	//CWnd* pWnd = GetDlgItem( IDC_EDIT_SCALE );
	//_gcvt ( , 7, szValue );
	//pWnd->SetWindowText( szName );
}

void CAniBoneScalePage::OnBnClickedButtonKeyscaleInsert()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	ANIMCONTNODE *pAnimNodeCur = m_pSheetTabAni->GetSkinAniContrl()->findanicontnode_none_const ( m_sAnimName.GetString() );
	if ( !pAnimNodeCur )	return;
	DxSkinAnimation* pAnimCont = pAnimNodeCur->m_pDxSkinAni;
	if ( !pAnimCont )		return;

	int nIndex = m_listBONENAME.GetCurSel();
	if( nIndex==LB_ERR )	return;

	DxAniScaleMAIN* pAniScale = pAnimCont->m_pAniScale->GetAniScaleMAIN( nIndex );
	if( !pAniScale )	return;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinAniControl *pAniContrl = NULL;

	switch ( CCharEditView::m_emObjType )
	{
	case EOBJ_SKINCHAR: pAniContrl = pView->GetSkinChar()->m_spSkinAniControlThread.get(); break;
	case EOBJ_ATTBONE : pAniContrl = pView->GetAttBone()->m_spSkinAniControlThread.get(); break; 
	case EOBJ_VEHICLE : pAniContrl = pView->GetVehicle()->m_spSkinAniControlThread.get(); break; 
	}

	if( !pAniContrl )	return;

	DWORD dwKey = pAniContrl->GETCURKEYTIME();

	D3DXVECTOR3 vScale;
	vScale.x = GetWin_Num_float( this, IDC_EDIT_SCALE1 );
	vScale.y = GetWin_Num_float( this, IDC_EDIT_SCALE2 );
	vScale.z = GetWin_Num_float( this, IDC_EDIT_SCALE3 );


	pAniScale->InsertData( dwKey, vScale );

	// Note : Ctrl 정리
	UpdateKeyScaleCTRL();
}

void CAniBoneScalePage::OnBnClickedButtonKeyscaleModify()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	ANIMCONTNODE *pAnimNodeCur = m_pSheetTabAni->GetSkinAniContrl()->findanicontnode_none_const ( m_sAnimName.GetString() );
	if ( !pAnimNodeCur )	return;
	DxSkinAnimation* pAnimCont = pAnimNodeCur->m_pDxSkinAni;
	if ( !pAnimCont )		return;

	int nIndex = m_listBONENAME.GetCurSel();
	if( nIndex==LB_ERR )	return;

	DxAniScaleMAIN* pAniScale = pAnimCont->m_pAniScale->GetAniScaleMAIN( nIndex );
	if( !pAniScale )	return;

	POSITION	pos		= m_ctrlKEYSCALE.GetFirstSelectedItemPosition();
	if( pos == NULL )	return;
	int	nItem = m_ctrlKEYSCALE.GetNextSelectedItem( pos );	

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinChar* pSkinChar = pView->GetSkinChar ();
	if( !pSkinChar )	return;

	D3DXVECTOR3 vScale;
	vScale.x = GetWin_Num_float( this, IDC_EDIT_SCALE1 );
	vScale.y = GetWin_Num_float( this, IDC_EDIT_SCALE2 );
	vScale.z = GetWin_Num_float( this, IDC_EDIT_SCALE3 );


	pAniScale->ModifyData( nItem, vScale );

	// Note : Ctrl 정리
	UpdateKeyScaleCTRL();
}

void CAniBoneScalePage::OnBnClickedButtonKeyscaleDelete2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	ANIMCONTNODE *pAnimNodeCur = m_pSheetTabAni->GetSkinAniContrl()->findanicontnode_none_const ( m_sAnimName.GetString() );
	if ( !pAnimNodeCur )	return;
	DxSkinAnimation* pAnimCont = pAnimNodeCur->m_pDxSkinAni;
	if ( !pAnimCont )		return;

	int nIndex = m_listBONENAME.GetCurSel();
	if( nIndex==LB_ERR )	return;

	DxAniScaleMAIN* pAniScale = pAnimCont->m_pAniScale->GetAniScaleMAIN( nIndex );
	if( !pAniScale )	return;

	POSITION	pos		= m_ctrlKEYSCALE.GetFirstSelectedItemPosition();
	if( pos == NULL )	return;
	int	nItem = m_ctrlKEYSCALE.GetNextSelectedItem( pos );	

	pAniScale->DeleteData( nItem );

	// Note : Ctrl 정리
	UpdateKeyScaleCTRL();
}

void CAniBoneScalePage::UpdateBoneNameLIST()
{
	m_listBONENAME.ResetContent();

	if( !m_pSheetTabAni )	return;

	ANIMCONTNODE *pAnimNodeCur = m_pSheetTabAni->GetSkinAniContrl()->findanicontnode_none_const ( m_sAnimName.GetString() );
	if ( !pAnimNodeCur )	return;
	DxSkinAnimation* pAnimCont = pAnimNodeCur->m_pDxSkinAni;
	if ( !pAnimCont )		return;

	DxAniScale::MAP_ANISCALEMAIN_ITER iter = pAnimCont->m_pAniScale->m_mapAniScaleMain.begin();
	for( ; iter!=pAnimCont->m_pAniScale->m_mapAniScaleMain.end(); ++iter )
	{
		int nIndex = m_listBONENAME.AddString(  (*iter).first.c_str() );
		m_listBONENAME.SetItemData( nIndex, nIndex );
	}	
}

void CAniBoneScalePage::UpdateKeyScaleCTRL()
{
	m_ctrlKEYSCALE.DeleteAllItems();

	if( !m_pSheetTabAni )	return;

	const ANIMCONTNODE *pAnimNodeCur = m_pSheetTabAni->GetSkinAniContrl()->findanicontnode ( m_sAnimName.GetString() );
	if ( !pAnimNodeCur )	return;
	const DxSkinAnimation* pAnimCont = pAnimNodeCur->m_pDxSkinAni;
	if ( !pAnimCont )		return;

	int nIndex = m_listBONENAME.GetCurSel();
	if( nIndex==LB_ERR )	return;

	char szValue[256];
	DxAniScaleMAIN* pAniScale = pAnimCont->m_pAniScale->GetAniScaleMAIN( nIndex );
	DxAniScaleMAIN::MAP_ANISCALEDATA_ITER iter = pAniScale->m_mapAniScaleData.begin();
	for( int i=0; iter!=pAniScale->m_mapAniScaleData.end(); ++iter, ++i )
	{
		_gcvt ( (*iter).first, 7, szValue );
		m_ctrlKEYSCALE.InsertItem( i, szValue );

		_gcvt ( (*iter).second.x, 7, szValue );
		m_ctrlKEYSCALE.SetItemText( i, 1, szValue );
		_gcvt ( (*iter).second.y, 7, szValue );
		m_ctrlKEYSCALE.SetItemText( i, 2, szValue );
		_gcvt ( (*iter).second.z, 7, szValue );
		m_ctrlKEYSCALE.SetItemText( i, 3, szValue );
	}
}









