// JiggleBonePage.cpp : ���� �����Դϴ�.
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


// CWeaponScalePage ��ȭ �����Դϴ�.

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


// CWeaponScalePage �޽��� ó�����Դϴ�.

// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

BOOL CWeaponScalePage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	RECT listRect;
	m_ctrlWeaponScale.GetClientRect(&listRect);
	int nListWidth = ( listRect.right - listRect.left );
	int nVScrollBarWidth = ::GetSystemMetrics(SM_CXVSCROLL); 
	int nColWidth[2] = { int(nListWidth*0.7f), int(nListWidth*0.31f) };

	m_ctrlWeaponScale.SetExtendedStyle ( m_ctrlWeaponScale.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	m_ctrlWeaponScale.InsertColumn ( 0, _T("Weapon"),	LVCFMT_CENTER, nColWidth[0] );
	m_ctrlWeaponScale.InsertColumn ( 1, _T("Scale"),	LVCFMT_CENTER, nColWidth[1] );

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

BOOL CWeaponScalePage::OnSetActive()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	UpdateWeaponScaleCTRL();

	SetWin_Text( this, IDC_STATIC_BONE_NAME, _T("") );
	SetWin_Num_float( this, IDC_EDIT_SCALE, 1.f );

	return CPropertyPage::OnSetActive();
}

void CWeaponScalePage::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_pSheetTabChar->SetActivePage( CHARTAB_CHARFILE );
}

void CWeaponScalePage::OnNMClickListWeaponScale(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CPropertyPage::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	TSTRING strText = 
		"Link ������.cps �� scale �� �� �ְ� ���ش�. \r\n"
		"\r\n"
		"whip_slot, box_slot, umbrella_slot �� ������.cps �� skin �Ǵ� �Ŷ� scale �� ������� �ʴ´�. \r\n"
		"\r\n"
		"Skin �Ǵ� cps �� scale �� �� �� ����, cps �� �ƿ� �����ؾ� �Ѵ�. \r\n"
		"������ �ϱ� ���� \\data\\skinobject\\changecps.lua ���� �����ؾ� �Ѵ�. \r\n"
		;

	AfxMessageBox( strText.c_str() );
}
