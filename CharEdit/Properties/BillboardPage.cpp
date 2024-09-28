// JiggleBonePage.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"

#include "../CharEdit.h"
#include "FramePage.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "../../SigmaCore/Util/Util.h"

#include "../../enginelib/DxMeshs/SkinMesh/NSBillboardLua.h"
#include "../../enginelib/DxMeshs/SkinMesh/DxBillboard.h"

#include "SheetWithTabChar.h"

#include "BoneSettingPage.h"
#include "BillboardPage.h"


extern BOOL			g_bClickSpinRadius;
extern BOOL			g_bClickSpinHeight;
extern BOOL			g_bClickSpin3;
extern BOOL			g_bClickSpin4;
extern DXRENDSTATE	g_RendState;

// CBillboardPage ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CBillboardPage, CPropertyPage)

CBillboardPage::CBillboardPage()
: CPropertyPage(CBillboardPage::IDD)
{

}

CBillboardPage::~CBillboardPage()
{
}

void CBillboardPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_TEX_WIDTH, m_comboTexWidth);
	DDX_Control(pDX, IDC_COMBO_TEX_HEIGHT, m_comboTexHeight);
}

BEGIN_MESSAGE_MAP(CBillboardPage, CPropertyPage)
	ON_BN_CLICKED(IDOK, &CBillboardPage::OnBnClickedOk_Save)
	ON_BN_CLICKED(IDOK3, &CBillboardPage::OnBnClickedOk)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_WIDTH, OnDeltaposSpinWidth)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_HEIGHT, OnDeltaposSpinHeight)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_OFFSET_X, &CBillboardPage::OnDeltaposSpinOffsetX)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_OFFSET_Y, &CBillboardPage::OnDeltaposSpinOffsetY)
	ON_EN_CHANGE(IDC_EDIT_WIDTH, OnEnChangeEditWidth)
	ON_EN_CHANGE(IDC_EDIT_HEIGHT, OnEnChangeEditHeight)
	ON_EN_CHANGE(IDC_EDIT_OFFSET_X, &CBillboardPage::OnEnChangeEditOffsetX)
	ON_EN_CHANGE(IDC_EDIT_OFFSET_Y, &CBillboardPage::OnEnChangeEditOffsetY)
	ON_CBN_SELCHANGE(IDC_COMBO_TEX_WIDTH, &CBillboardPage::OnCbnSelchangeComboTexWidth)
	ON_CBN_SELCHANGE(IDC_COMBO_TEX_HEIGHT, &CBillboardPage::OnCbnSelchangeComboTexHeight)
	ON_BN_CLICKED(IDC_BUTTON_DOCUMENT, &CBillboardPage::OnBnClickedButtonDocument)
END_MESSAGE_MAP()


// CBillboardPage �޽��� ó�����Դϴ�.

// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

BOOL CBillboardPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	for ( DWORD i=0; i<10; ++i )
	{
		DWORD dwValue = static_cast<DWORD>( powl( 2, i ) );

		std::stringstream ssText;
		ssText << dwValue;

		int nIndex = m_comboTexWidth.AddString( ssText.str().c_str() );
		m_comboTexWidth.SetItemData( nIndex, dwValue );

		nIndex = m_comboTexHeight.AddString( ssText.str().c_str() );
		m_comboTexHeight.SetItemData( nIndex, dwValue );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

BOOL CBillboardPage::OnSetActive()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	if( !pFrame )
		return TRUE;

	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	if( !pView )
		return TRUE;

	DxSkinCharData *pSkinCharData = pView->GetSkinCharData();
	if( !pSkinCharData )
		return TRUE;

	CWnd*	pWnd;
	CString Str;
	const BILLBOARD_CHF_DATA *pBillboardChfData = pView->GetBillboardData();
	if( pBillboardChfData )
	{
		TSTRING strBillboard = sc::util::ChangeExtName( pSkinCharData->GetFileName(), _T("blb") );
		pWnd = GetDlgItem( IDC_EDIT_BILLBOARD_NAME );
		pWnd->SetWindowText( strBillboard.c_str());

		pWnd = GetDlgItem( IDC_EDIT_WIDTH );
		Str.Format ( "%3.2f", pBillboardChfData->m_fWidth );
		pWnd->SetWindowText( Str.GetString() );

		pWnd = GetDlgItem( IDC_EDIT_HEIGHT );
		Str.Format ( "%3.2f", pBillboardChfData->m_fHeight );
		pWnd->SetWindowText( Str.GetString() );

		pWnd = GetDlgItem( IDC_EDIT_OFFSET_X );
		Str.Format ( "%3.2f", pBillboardChfData->m_fRightOffset );
		pWnd->SetWindowText( Str.GetString() );

		pWnd = GetDlgItem( IDC_EDIT_OFFSET_Y );
		Str.Format ( "%3.2f", pBillboardChfData->m_fUpOffset );
		pWnd->SetWindowText( Str.GetString() );

		m_comboTexWidth.SetCurSel( pBillboardChfData->m_nTexSizeIndexWidth );
		m_comboTexHeight.SetCurSel( pBillboardChfData->m_nTexSizeIndexHeight );
	}

	g_RendState.bBillboard = TRUE;

	return CPropertyPage::OnSetActive();
}

BOOL CBillboardPage::OnKillActive()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	g_RendState.bBillboard = FALSE;

	return CPropertyPage::OnKillActive();
}

void CBillboardPage::OnBnClickedOk_Save()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	if( !pFrame )
		return;

	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	if( !pView )
		return;

	BILLBOARD_CHF_DATA*	pBillboardChfData = pView->GetBillboardData();
	if( !pBillboardChfData )
	{
		return;
	}

	DxSkinCharData *pSkinCharData = pView->GetSkinCharData();
	if( !pSkinCharData )
		return;

	DxSkinChar *pSkinChar = pView->GetSkinChar();
	if( !pSkinChar )
		return;

	TSTRING strTexture = sc::util::ChangeExtName( pSkinCharData->GetFileName(), _T("blb") );
	pBillboardChfData->Save( strTexture.c_str() );

	pSkinChar->CreateChfBillboard( pView->GetD3dDevice(), FALSE, pSkinCharData->GetFileName().c_str() );

	m_pSheetTabChar->SetActivePage( CHARTAB_CHARFILE );
}

void CBillboardPage::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_pSheetTabChar->SetActivePage( CHARTAB_CHARFILE );
}

void CBillboardPage::OnEnChangeEditWidth()
{
	// TODO:  RICHEDIT ��Ʈ���� ��� �� ��Ʈ����
	// CPropertyPage::����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ�
	// CRichEditCtrl().SetEventMask()�� ȣ���ϵ��� OnInitDialog() �Լ��� ���������� ������
	// �� �˸��� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	if( !pFrame )
		return;

	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	if( !pView )
		return;

	BILLBOARD_CHF_DATA*	pBillboardChfData = pView->GetBillboardData();
	if( !pBillboardChfData )
		return;

	CWnd*	pWnd;
	CString Str;
	pWnd = GetDlgItem( IDC_EDIT_WIDTH );
	pWnd->GetWindowText( Str );
	pBillboardChfData->m_fWidth = (float)atof( Str.GetString() );
	NSBillboard::ImportBillboard( pBillboardChfData );
}

void CBillboardPage::OnEnChangeEditHeight()
{
	// TODO:  RICHEDIT ��Ʈ���� ��� �� ��Ʈ����
	// CPropertyPage::����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ�
	// CRichEditCtrl().SetEventMask()�� ȣ���ϵ��� OnInitDialog() �Լ��� ���������� ������
	// �� �˸��� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	if( !pFrame )
		return;

	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	if( !pView )
		return;

	BILLBOARD_CHF_DATA*	pBillboardChfData = pView->GetBillboardData();
	if( !pBillboardChfData )
		return;

	CWnd*	pWnd;
	CString Str;
	pWnd = GetDlgItem( IDC_EDIT_HEIGHT );
	pWnd->GetWindowText( Str );
	pBillboardChfData->m_fHeight = (float)atof( Str.GetString() );
	NSBillboard::ImportBillboard( pBillboardChfData );
}

void CBillboardPage::OnEnChangeEditOffsetX()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CPropertyPage::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	if( !pFrame )
		return;

	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	if( !pView )
		return;

	BILLBOARD_CHF_DATA*	pBillboardChfData = pView->GetBillboardData();
	if( !pBillboardChfData )
		return;

	CWnd*	pWnd;
	CString Str;
	pWnd = GetDlgItem( IDC_EDIT_OFFSET_X );
	pWnd->GetWindowText( Str );
	pBillboardChfData->m_fRightOffset = (float)atof( Str.GetString() );
	NSBillboard::ImportBillboard( pBillboardChfData );
}

void CBillboardPage::OnEnChangeEditOffsetY()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CPropertyPage::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	if( !pFrame )
		return;

	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	if( !pView )
		return;

	BILLBOARD_CHF_DATA*	pBillboardChfData = pView->GetBillboardData();
	if( !pBillboardChfData )
		return;

	CWnd*	pWnd;
	CString Str;
	pWnd = GetDlgItem( IDC_EDIT_OFFSET_Y );
	pWnd->GetWindowText( Str );
	pBillboardChfData->m_fUpOffset = (float)atof( Str.GetString() );
	NSBillboard::ImportBillboard( pBillboardChfData );
}

void CBillboardPage::OnDeltaposSpinWidth(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;

	g_bClickSpinRadius = TRUE;
}

void CBillboardPage::OnDeltaposSpinHeight(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;

	g_bClickSpinHeight = TRUE;
}

void CBillboardPage::OnDeltaposSpinOffsetX(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;

	g_bClickSpin3 = TRUE;
}

void CBillboardPage::OnDeltaposSpinOffsetY(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;

	g_bClickSpin4 = TRUE;
}

void CBillboardPage::SetSpinControl( int nOffset1, int nOffset2, int nOffset3, int nOffset4 )
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	if( !pFrame )
		return;

	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	if( !pView )
		return;

	CWnd*	pWnd;
	CString Str;
	BILLBOARD_CHF_DATA*	pBillboardChfData = pView->GetBillboardData();
	if( !pBillboardChfData )
		return;

	// Note : ���� ��ȭ��Ŵ
	pBillboardChfData->m_fWidth			-= (float)nOffset1 * 0.02f;
	pBillboardChfData->m_fHeight		-= (float)nOffset2 * 0.02f;
	pBillboardChfData->m_fRightOffset	-= (float)nOffset3 * 0.02f;
	pBillboardChfData->m_fUpOffset		-= (float)nOffset4 * 0.02f;

	// Note : ��ȭ��Ų ���� ǥ����.
	pWnd = GetDlgItem( IDC_EDIT_WIDTH );
	Str.Format ( "%3.2f", pBillboardChfData->m_fWidth );
	pWnd->SetWindowText( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_HEIGHT );
	Str.Format ( "%3.2f", pBillboardChfData->m_fHeight );
	pWnd->SetWindowText( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_OFFSET_X );
	Str.Format ( "%3.2f", pBillboardChfData->m_fRightOffset );
	pWnd->SetWindowText( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_OFFSET_Y );
	Str.Format ( "%3.2f", pBillboardChfData->m_fUpOffset );
	pWnd->SetWindowText( Str.GetString() );

	NSBillboard::ImportBillboard( pBillboardChfData );
}

void CBillboardPage::OnCbnSelchangeComboTexWidth()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int nIndex = m_comboTexWidth.GetCurSel();
	if( nIndex == -1 )
		return;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	if( !pFrame )
		return;

	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	if( !pView )
		return;

	BILLBOARD_CHF_DATA*	pBillboardChfData = pView->GetBillboardData();
	if( !pBillboardChfData )
		return;

	pBillboardChfData->m_nTexSizeIndexWidth = nIndex;
	NSBillboard::ImportBillboard( pBillboardChfData );
}

void CBillboardPage::OnCbnSelchangeComboTexHeight()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int nIndex = m_comboTexHeight.GetCurSel();
	if( nIndex == -1 )
		return;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	if( !pFrame )
		return;

	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	if( !pView )
		return;

	BILLBOARD_CHF_DATA*	pBillboardChfData = pView->GetBillboardData();
	if( !pBillboardChfData )
		return;

	pBillboardChfData->m_nTexSizeIndexHeight = nIndex;
	NSBillboard::ImportBillboard( pBillboardChfData );
}


void CBillboardPage::OnBnClickedButtonDocument()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	TSTRING strText = 
		"���⼭ �����ϴ°��� ��ܿ��� ������ bone�� (.blb)(_blb.dds) ���·� ����ǰ� �ȴ�. \r\n"
		"��) abc.chf �� �����ϸ�, abc.blb, abc_blb.dds �� �����ȴ�.  \r\n"
		"\r\n"
		"\\data\\billboard\\billboard.lua ������ �ʿ���. \r\n"
		"lua ���Ͽ��� chf �� ������ billboard ������ dds ������ �����Ѵ�.  \r\n"
		"\r\n"
		"\r\n"
		"[Save && OK] - blb �� data�� save �ϰ�, dds �� texture �� �����. \r\n"
		"\r\n"
		"[Don`t Save && OK] - blb �� save �� ���� �ʴ´�. ������ ���õ� ���� �����ȴ�. Tool �� ���� ���� ������ ��� �Ұ����ϴ�. \r\n"
		;

	AfxMessageBox( strText.c_str() );
}
