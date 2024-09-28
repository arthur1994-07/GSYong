// JiggleBonePage.cpp : 구현 파일입니다.
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

// CBillboardPage 대화 상자입니다.

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


// CBillboardPage 메시지 처리기입니다.

// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

BOOL CBillboardPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
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
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BOOL CBillboardPage::OnSetActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
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
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	g_RendState.bBillboard = FALSE;

	return CPropertyPage::OnKillActive();
}

void CBillboardPage::OnBnClickedOk_Save()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTabChar->SetActivePage( CHARTAB_CHARFILE );
}

void CBillboardPage::OnEnChangeEditWidth()
{
	// TODO:  RICHEDIT 컨트롤인 경우 이 컨트롤은
	// CPropertyPage::마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여
	// CRichEditCtrl().SetEventMask()를 호출하도록 OnInitDialog() 함수를 재지정하지 않으면
	// 이 알림을 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
	// TODO:  RICHEDIT 컨트롤인 경우 이 컨트롤은
	// CPropertyPage::마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여
	// CRichEditCtrl().SetEventMask()를 호출하도록 OnInitDialog() 함수를 재지정하지 않으면
	// 이 알림을 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CPropertyPage::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CPropertyPage::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	g_bClickSpinRadius = TRUE;
}

void CBillboardPage::OnDeltaposSpinHeight(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	g_bClickSpinHeight = TRUE;
}

void CBillboardPage::OnDeltaposSpinOffsetX(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	g_bClickSpin3 = TRUE;
}

void CBillboardPage::OnDeltaposSpinOffsetY(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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

	// Note : 값을 변화시킴
	pBillboardChfData->m_fWidth			-= (float)nOffset1 * 0.02f;
	pBillboardChfData->m_fHeight		-= (float)nOffset2 * 0.02f;
	pBillboardChfData->m_fRightOffset	-= (float)nOffset3 * 0.02f;
	pBillboardChfData->m_fUpOffset		-= (float)nOffset4 * 0.02f;

	// Note : 변화시킨 값을 표현함.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	TSTRING strText = 
		"여기서 셋팅하는것은 상단에서 설정된 bone의 (.blb)(_blb.dds) 상태로 저장되게 된다. \r\n"
		"예) abc.chf 로 생성하면, abc.blb, abc_blb.dds 가 생성된다.  \r\n"
		"\r\n"
		"\\data\\billboard\\billboard.lua 파일이 필요함. \r\n"
		"lua 파일에서 chf 와 연관된 billboard 정보와 dds 정보를 셋팅한다.  \r\n"
		"\r\n"
		"\r\n"
		"[Save && OK] - blb 에 data를 save 하고, dds 로 texture 를 만든다. \r\n"
		"\r\n"
		"[Don`t Save && OK] - blb 에 save 는 하지 않는다. 하지만 셋팅된 것을 유지된다. Tool 을 끄면 변경 정보는 사용 불가능하다. \r\n"
		;

	AfxMessageBox( strText.c_str() );
}
