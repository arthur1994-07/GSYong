// JiggleBonePage.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "../CharEdit.h"
#include "FramePage.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "../../MfcExLib/ExLibs/EtcFunction.h"

#include "SheetWithTabChar.h"

#include "WeaponScalePage.h"


// CWeaponScalePage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CWeaponScalePage, CPropertyPage)

CWeaponScalePage::CWeaponScalePage()
	: CPropertyPage(CWeaponScalePage::IDD)
{

}

CWeaponScalePage::~CWeaponScalePage()
{
}

void CWeaponScalePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_WEAPON_SCALE, m_ctrlWeaponScale);
}

BEGIN_MESSAGE_MAP(CWeaponScalePage, CPropertyPage)
	ON_BN_CLICKED(IDOK, &CWeaponScalePage::OnBnClickedOk)
	ON_NOTIFY(NM_CLICK, IDC_LIST_WEAPON_SCALE, OnNMClickListWeaponScale)
	ON_EN_CHANGE(IDC_EDIT_SCALE, &CWeaponScalePage::OnEnChangeEditScale)
	ON_BN_CLICKED(IDC_BUTTON_DOCUMENT, &CWeaponScalePage::OnBnClickedButtonDocument)
END_MESSAGE_MAP()


// CWeaponScalePage 메시지 처리기입니다.

// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

BOOL CWeaponScalePage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	RECT listRect;
	m_ctrlWeaponScale.GetClientRect(&listRect);
	int nListWidth = ( listRect.right - listRect.left );
	int nVScrollBarWidth = ::GetSystemMetrics(SM_CXVSCROLL); 
	int nColWidth[2] = { int(nListWidth*0.7f), int(nListWidth*0.31f) };

	m_ctrlWeaponScale.SetExtendedStyle ( m_ctrlWeaponScale.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	m_ctrlWeaponScale.InsertColumn ( 0, _T("Weapon"),	LVCFMT_CENTER, nColWidth[0] );
	m_ctrlWeaponScale.InsertColumn ( 1, _T("Scale"),	LVCFMT_CENTER, nColWidth[1] );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BOOL CWeaponScalePage::OnSetActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	UpdateWeaponScaleCTRL();

	SetWin_Text( this, IDC_STATIC_BONE_NAME, _T("") );
	SetWin_Num_float( this, IDC_EDIT_SCALE, 1.f );

	return CPropertyPage::OnSetActive();
}

void CWeaponScalePage::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTabChar->SetActivePage( CHARTAB_CHARFILE );
}

void CWeaponScalePage::OnNMClickListWeaponScale(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	POSITION pos = m_ctrlWeaponScale.GetFirstSelectedItemPosition();
	if( pos == NULL )
		return;

	int	nIndex = m_ctrlWeaponScale.GetNextSelectedItem( pos );
	if( nIndex == LB_ERR )
		return;

	if ( nIndex >= EMPEACE_WEAPON_SIZE )
		return;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	SetWin_Text( this, IDC_STATIC_BONE_NAME, SKINTYPE_STING::m_szWHEREBACKTYPE[nIndex] );
	SetWin_Num_float( this, IDC_EDIT_SCALE, pView->GetSkinCharData()->m_arrayfWeaponScale[nIndex] );
}

void CWeaponScalePage::UpdateWeaponScaleCTRL()
{
	m_ctrlWeaponScale.DeleteAllItems();

	if( !m_pSheetTabChar )
		return;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	char szValue[256];
	for ( DWORD i=0; i<EMPEACE_WEAPON_SIZE; ++i )
	{
		m_ctrlWeaponScale.InsertItem( i, SKINTYPE_STING::m_szWHEREBACKTYPE[i] );

		_gcvt( pView->GetSkinCharData()->m_arrayfWeaponScale[i], 7, szValue );
		m_ctrlWeaponScale.SetItemText( i, 1, szValue );
	}
}

void CWeaponScalePage::OnEnChangeEditScale()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CPropertyPage::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

	POSITION pos = m_ctrlWeaponScale.GetFirstSelectedItemPosition();
	if( pos == NULL )
		return;

	int	nIndex = m_ctrlWeaponScale.GetNextSelectedItem( pos );
	if( nIndex == LB_ERR )
		return;

	if ( nIndex >= EMPEACE_WEAPON_SIZE )
		return;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	pView->GetSkinCharData()->m_arrayfWeaponScale[nIndex] = GetWin_Num_float( this, IDC_EDIT_SCALE );

	char szValue[256];
	_gcvt( pView->GetSkinCharData()->m_arrayfWeaponScale[nIndex], 7, szValue );
	m_ctrlWeaponScale.SetItemText( nIndex, 1, szValue );

	pView->GetSkinChar()->SetCharData( pView->GetSkinCharData(), pView->GetD3dDevice(), false, EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE );
}

void CWeaponScalePage::OnBnClickedButtonDocument()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	TSTRING strText = 
		"Link 아이템.cps 를 scale 할 수 있게 해준다. \r\n"
		"\r\n"
		"whip_slot, box_slot, umbrella_slot 의 아이템.cps 는 skin 되는 거라 scale 이 적용되지 않는다. \r\n"
		"\r\n"
		"Skin 되는 cps 는 scale 이 될 수 없고, cps 를 아예 변경해야 한다. \r\n"
		"변경을 하기 위해 \\data\\skinobject\\changecps.lua 값을 셋팅해야 한다. \r\n"
		;

	AfxMessageBox( strText.c_str() );
}
