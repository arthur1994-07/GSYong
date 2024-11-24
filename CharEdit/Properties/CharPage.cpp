// CharPage.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "../CharEdit.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "SheetWithTabChar.h"
#include "SheetWithTabFrame.h"
#include "SheetWithTabBone.h"
#include "SheetWithTabAnim.h"
#include "SheetWithTabPiece.h"

#include "../SigmaCore/Util/Util.h"

#include "../../EngineLib/DxTools/DebugSet.h"
#include "../../EngineLib/DxTools/DxMethods.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinObject.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinChar.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinCharData.h"
#include "../MfcExLib/ExLibs/EtcFunction.h"

#include "CharPage.h"
#include ".\charpage.h"

#include "../State/CharStateMachine.h"

extern BOOL	g_bClickSpinRadius;
extern BOOL	g_bClickSpinHeight;

// CCharPage ��ȭ �����Դϴ�.

BOOL CCharPage::g_bMakeNameChfCfg = TRUE;

IMPLEMENT_DYNAMIC(CCharPage, CPropertyPage)
CCharPage::CCharPage() :
    CPieceContainerPage(CCharPage::IDD)
{

}

CCharPage::~CCharPage()
{
    
}

void CCharPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCharPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_NEW, OnBnClickedButtonNew)
	ON_BN_CLICKED(IDC_BUTTON_OPEN, OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	//ON_BN_CLICKED(IDC_BUTTON_SAVE_CHF_BILLBOARD, OnBnClickedButtonSaveChfBillboard)
	//ON_BN_CLICKED(IDC_BUTTON_ALLSAVE, OnBnClickedButtonAllSave)	
	ON_BN_CLICKED(IDC_CHECK_NOCHARACTER, OnBnClickedCheckNocharacter)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_RADIUS, OnDeltaposSpinRadius)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_HEIGHT, OnDeltaposSpinHeight)
	ON_EN_CHANGE(IDC_EDIT_RADIUS, OnEnChangeEditRadius)
	ON_EN_CHANGE(IDC_EDIT_HEIGHT, OnEnChangeEditHeight)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_EE, OnBnClickedButtonLoadEe)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_EE, OnBnClickedButtonSaveEe)
	ON_BN_CLICKED(IDC_CHECK_NONATKMODE, OnBnClickedCheckNonatkmode)
	ON_BN_CLICKED(IDC_CHECK_ONE_COLOR, &CCharPage::OnBnClickedCheckOneColor)
	ON_BN_CLICKED(IDC_CHECK_NAME_CHF_CFG, &CCharPage::OnBnClickedCheckNameChfCfg)
END_MESSAGE_MAP()


// CCharPage �޽��� ó�����Դϴ�.

BOOL CCharPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	CRect SheetRect;
	CWnd *pWnd;
	
	//	Note : CsheetWithTabFrame �� �����.
	//
	pWnd = GetDlgItem ( IDC_STATIC_SHEETTAP1 );
	pWnd->GetWindowRect ( SheetRect );
	ScreenToClient ( &SheetRect );

	m_psheetWithTabFrame = new CsheetWithTabFrame ( SheetRect, this );
	m_psheetWithTabFrame->Create ( this, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0 );
	m_psheetWithTabFrame->m_FramePage.SetFrameEditType( EMFRAMEEDIT_CHAR );

	//	Note : CsheetWithTabAnim �� �����.
	//
	pWnd = GetDlgItem ( IDC_STATIC_SHEETTAP2 );
	pWnd->GetWindowRect ( SheetRect );
	ScreenToClient ( &SheetRect );
	
	m_psheetWithTabAnim = new CsheetWithTabAnim ( SheetRect, this );
	m_psheetWithTabAnim->Create ( this, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0 );

    CRect SheetRectAll;
    GetWindowRect ( SheetRectAll );
	ScreenToClient ( &SheetRectAll );

#ifdef DEF_SKIN_SYNC
	//	Note : CsheetWithTabPiece �� �����.
	//
	m_psheetWithTabPiece = new CsheetWithTabPiece ( SheetRect, SheetRectAll, this );
	m_psheetWithTabPiece->Create ( this, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0 );
	m_psheetWithTabPiece->ShowWindow(SW_HIDE);
#endif

	pWnd = GetDlgItem ( IDC_STATIC_SHEETTAP3 );
	pWnd->GetWindowRect ( SheetRect );
	ScreenToClient ( &SheetRect );

	m_psheetWithTabBone = new CsheetWithTabBone ( SheetRect, this );
	m_psheetWithTabBone->Create ( this, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0 );

	m_psheetWithTabBone->ShowWindow(FALSE);	

	SetWin_ShowWindow( this, IDC_STATIC_SHEETTAP3, FALSE );

	pWnd = GetDlgItem ( IDC_EDIT_CHARFILE );
	pWnd->SetWindowText ( "untitle00.chf" );

	// Note : ��ư �� ����
	ReSetCheckButton();

	// Note : ��ư �� ����
	ReSetEditBox();

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CCharPage::OnBnClickedButtonNew()
{
	if( m_hWnd == NULL ) return;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	LPDIRECT3DDEVICEQ pd3dDevice	= pView->GetD3dDevice();

	DxSkinChar *pSkinChar = pView->GetSkinChar();
	DxSkinCharData *pSkinCharData = pView->GetSkinCharData();

	CWnd *pWnd;
	pWnd = GetDlgItem ( IDC_EDIT_CHARFILE );
	pWnd->SetWindowText ( "untitle00.chf" );
	
	CharStateMachine::ClearFileNotifier();

	pSkinCharData->ClearAll ();
	pSkinChar->ClearAll ();

	CDlgEdit &DlgEdit = pFrame->m_cDlgEdit;
	DlgEdit.GetSheetChar()->m_CharPage.SetBoneEditTab(FALSE);

	m_psheetWithTabFrame->ReSetPages();
	m_psheetWithTabAnim->ReSetPages();
	m_psheetWithTabBone->ReSetPages();

#ifdef DEF_SKIN_SYNC
    m_psheetWithTabPiece->ReSetPages();
#endif

	// Note : ��ư �� ����
	ReSetCheckButton();

	// Note : ��ư �� ����
	ReSetEditBox();
}

VOID	CCharPage::InsertAttBone( const char *pszFileName )
{
	m_psheetWithTabFrame->m_FramePage.InsertAttBone(pszFileName);
}

VOID	CCharPage::InsertSkinPiece( const char *pszFileName )
{
	m_psheetWithTabFrame->m_FramePage.InsertSkinPiece(pszFileName);
}

VOID	CCharPage::InsertSkinEff( const char *pszFileName )
{
	m_psheetWithTabFrame->m_FramePage.InsertSkinEff(pszFileName);
}

VOID	CCharPage::LoadAnimation( const char *pszFileName )
{
	m_psheetWithTabAnim->m_AnimTabPage.GetAnimPage()->LoadAnimation(pszFileName);
}

VOID	CCharPage::LoadSkinBone( const char *pszFileName )
{
	m_psheetWithTabFrame->m_FramePage.LoadSkinBone(pszFileName);
}

BOOL CCharPage::LoadChfFile( const char *pszFileName )
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice ();
	DxSkinChar *pSkinChar = pView->GetSkinChar();
	DxSkinCharData *pSkinCharData = pView->GetSkinCharData();

	//	Note : Sound Log
#ifdef	_DEBUG
	CDebugSet::ToFile ( "UseSoundList.txt", "" );
	CDebugSet::ToFile ( "UseSoundList.txt", "[-------------------------------------------------------------]" );
	CDebugSet::ToFile ( "UseSoundList.txt", "[!.%s]", pszFileName );

	//////////////////////////////////////////////
	//	<--	���� �ð� Ȯ��
	char	szTime[128];
	SYSTEMTIME	UniversalTime;
	SYSTEMTIME	LocalTime;

	//	<--0	Universal Time ��������
	GetSystemTime( &UniversalTime );
	//	-->0	Universal Time ��������

	//	<--0	TimeZone ��������
	TIME_ZONE_INFORMATION TimeZone;
	GetTimeZoneInformation ( &TimeZone );
	//	-->0	TimeZone ��������

	//	<--0	Universal Time�� TimeZone�� �ռ�
	SystemTimeToTzSpecificLocalTime ( &TimeZone, &UniversalTime, &LocalTime );
	//	-->0	Universal Time�� TimeZone�� �ռ�

	sprintf ( szTime, "%d-%02d-%02dT%02d:%02d", LocalTime.wYear, LocalTime.wMonth,
		LocalTime.wDay, LocalTime.wHour, LocalTime.wMinute );	
	//	-->	���� �ð� Ȯ��
	////////////////////////////////////////////////

	CDebugSet::ToFile ( "UseSoundList.txt", "[$.%s]", szTime );	
#endif	//	_DEBUG

	//	Note : �� �����͸� �ʱ�ȭ.
	OnBnClickedButtonNew();

	//	Note : �ɸ��� ���� �б�.
	//
	BOOL bOk = pSkinCharData->LoadFile ( pszFileName, pd3dDevice, false );
	if ( !bOk )
	{
		MessageBox ( "Failed to read file." );
		return FALSE;
	}

	BILLBOARD_CHF_DATA* pBillboardData = pView->GetBillboardData();
	if ( pBillboardData )
	{
		TSTRING strTexture = sc::util::ChangeExtName( pSkinCharData->GetFileName(), _T("blb") );
		*pBillboardData = BILLBOARD_CHF_DATA();
		pBillboardData->Load( strTexture.c_str() );
	}

	//	Note : �ɸ��� Data ����.
	//
	pSkinChar->SetCharData ( pSkinCharData, pd3dDevice, false, EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE );

#ifndef DEF_SKIN_SYNC
    pSkinChar->UpdateLOD(D3DXVECTOR3(0, 0, 0), pd3dDevice);
#else
    for (int i = 0; i < PIECE_SIZE; ++i)
    {
        TSTRING strFile = pSkinCharData->GetPiece(EMPIECECHAR(i));

        if (m_psheetWithTabPiece && strFile.empty() == false)
        {
            m_psheetWithTabPiece->GetSkinPiecePage()->LoadSkinPiece(strFile.c_str());
        }
    }
#endif

	CWnd *pWnd;
	pWnd = GetDlgItem ( IDC_EDIT_CHARFILE );
	pWnd->SetWindowText ( pszFileName );

	m_psheetWithTabFrame->ReSetPages();
	m_psheetWithTabAnim->SetPages( pView->GetSkinCharData()->GetSkinAniControlData().get(), pSkinChar->m_spSkinAniControlThread.get() );
	m_psheetWithTabBone->SetPages( pSkinChar->m_spSkinAniControlThread.get() );

	// Note : ��ư �� ����
	ReSetCheckButton();

	// Note : ��ư �� ����
	ReSetEditBox();

	pView->ResetSkinPos();

	pFrame->m_cDlgTimeLine.SetStop();

	CharStateMachine::InsertFileNotifier();

	return TRUE;
}

void CCharPage::OnBnClickedButtonOpen()
{

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice ();
	DxSkinChar *pSkinChar = pView->GetSkinChar();
	DxSkinCharData *pSkinCharData = pView->GetSkinCharData();

	CString szFilter = "Charactor File (*.chf)|*.chf|";
	
	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(TRUE,".chf",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CFramePage*)this);

	dlg.m_ofn.lpstrInitialDir = DxSkinCharDataContainer::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		CString sFileName = dlg.GetFileName();
		
		LoadChfFile( sFileName.GetString() );
	}
}

void CCharPage::OnBnClickedButtonSave()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice ();

	DxSkinChar *pSkinChar = pView->GetSkinChar();
	if (!pSkinChar)
        return;
	DxSkinCharData *pSkinCharData = pView->GetSkinCharData();
	if (!pSkinCharData)
        return;

    std::string SkinName(pSkinCharData->GetFileName());
	std::string strName = sc::util::ChangeExtName(SkinName, "");

	//	Note : ���� ���� ���̾˷α׸� ����.
	CString szFilter = "Charactor File (*.chf)|*.chf|";
	CFileDialog dlg(FALSE,".chf", strName.c_str() ,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, (CFramePage*)this);

	dlg.m_ofn.lpstrInitialDir = DxSkinCharDataContainer::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		CString sFileName = dlg.GetFileName();

		//	Note : �ɸ��� ���� ����.
		//
		BOOL bOk = pSkinCharData->SaveFile ( sFileName.GetString(), pd3dDevice );

		if ( !bOk )
		{
			MessageBox ( "Failed to write file." );
		}

		CWnd *pWnd;
		pWnd = GetDlgItem ( IDC_EDIT_CHARFILE );
		pWnd->SetWindowText ( sFileName.GetString() );
	}
}

//void CCharPage::OnBnClickedButtonSaveChfBillboard()
//{
//	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
//	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
//	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice ();
//
//	DxSkinChar *pSkinChar = pView->GetSkinChar();
//	if (!pSkinChar)
//		return;
//	DxSkinCharData *pSkinCharData = pView->GetSkinCharData();
//	if (!pSkinCharData)
//		return;
//
//	std::string SkinName(pSkinCharData->GetFileName());
//	std::string strName = sc::util::ChangeExtName(SkinName, "");
//
//	//	Note : ���� ���� ���̾˷α׸� ����.
//	CString szFilter = "Charactor File (*.chf)|*.chf|";
//	CFileDialog dlg(FALSE,".chf", strName.c_str() ,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, (CFramePage*)this);
//
//	dlg.m_ofn.lpstrInitialDir = DxSkinCharDataContainer::GetInstance().GetPath ();
//	if ( dlg.DoModal() == IDOK )
//	{
//		CString sFileName = dlg.GetFileName();
//
//		//	Note : �ɸ��� ���� ����.
//		//
//		BOOL bOk = pSkinCharData->SaveFile ( sFileName.GetString(), pd3dDevice );
//
//		if ( !bOk )
//		{
//			MessageBox ( "���� ���⿡ �����Ͽ����ϴ�." );
//		}
//
//		CWnd *pWnd;
//		pWnd = GetDlgItem ( IDC_EDIT_CHARFILE );
//		pWnd->SetWindowText ( sFileName.GetString() );
//
//		// ������ ����.
//		pSkinChar->CreateChfBillboard( pView->GetD3dDevice(), FALSE, sFileName.GetString() );
//	}
//}

//void CCharPage::OnBnClickedButtonAllSave()
//{
//	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
//	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
//	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice ();
//	DxSkinChar *pSkinChar = pView->GetSkinChar();
//	DxSkinCharData *pSkinCharData = pView->GetSkinCharData();
//
//	CString strFind = DxSkinCharDataContainer::GetInstance().GetPath ();
//	strFind += "*.chf";
//
//	WIN32_FIND_DATA pInfo;
//	lstrcpy( pInfo.cFileName, strFind.GetString() );
//
//	HANDLE hContext = FindFirstFile( strFind.GetString(), &pInfo );
//	if( hContext == INVALID_HANDLE_VALUE )
//	{
//		FindClose( hContext );
//		return;
//	}
//
//	do
//	{
//		OnBnClickedButtonNew();
//
//		BOOL bLoadOk = pSkinCharData->LoadFile ( pInfo.cFileName, pd3dDevice, false );
//		if ( bLoadOk )
//		{
//			pSkinChar->SetCharData ( pSkinCharData, pd3dDevice, false, EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE );
//			BOOL bSaveOk = pSkinCharData->SaveFile ( pInfo.cFileName, pd3dDevice );
//			
//			if ( !bSaveOk ) MessageBox("chf Save Error",pInfo.cFileName,MB_OK);
//		}
//		else
//		{
//			MessageBox("chf Load Error",pInfo.cFileName,MB_OK);
//		}
//
//	}
//	while( FindNextFile( hContext, &pInfo ) != 0 );
//
//	OnBnClickedButtonNew();
//
//	FindClose( hContext );
//
//	MessageBox( _T("chf All Save Success"), _T("Success"), MB_OK );
//}

void CCharPage::OnBnClickedCheckNocharacter()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	CButton* pButton = (CButton*)GetDlgItem( IDC_CHECK_NOCHARACTER );

	DxSkinChar *pSkinChar = pView->GetSkinChar();
	if( pSkinChar )
	{
		if( pButton->GetCheck() )	pSkinChar->SetWorldObj( TRUE );
		else						pSkinChar->SetWorldObj( FALSE );
	}

	DxSkinCharData *pSkinCharData = pView->GetSkinCharData();
	if( pSkinCharData )
	{
		if( pButton->GetCheck() )	pSkinCharData->m_bWorldObj = TRUE;
		else						pSkinCharData->m_bWorldObj = FALSE;
	}
}

void CCharPage::ReSetCheckButton()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	if( !pFrame )	return;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	if( !pView )	return;

	CButton* pButton = (CButton*)GetDlgItem( IDC_CHECK_NOCHARACTER );
	DxSkinChar *pSkinChar = pView->GetSkinChar();
	if( pSkinChar )
	{
		if( pSkinChar->GetWorldObj() )	pButton->SetCheck( TRUE );
		else							pButton->SetCheck( FALSE );
	}
	else
	{
		pButton->SetCheck( FALSE );
	}

	pButton = (CButton*)GetDlgItem( IDC_CHECK_ONE_COLOR );
	//DxSkinCharData *pSkinCharData = pView->GetSkinCharData();
	if( pSkinChar )
	{
		if( pSkinChar->IsUseWorldColor() )	pButton->SetCheck( TRUE );
		else								pButton->SetCheck( FALSE );
	}
	else
	{
		pButton->SetCheck( FALSE );
	}
}

void CCharPage::ReSetEditBox()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	if( !pFrame )	return;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	if( !pView )	return;

	CWnd*	pWnd;
	CString Str;
	DxSkinCharData *pSkinCharData = pView->GetSkinCharData();
	if( pSkinCharData )
	{
		pWnd = GetDlgItem( IDC_EDIT_RADIUS );
		Str.Format ( "%3.2f", pSkinCharData->m_fRadius );
		pWnd->SetWindowText( Str.GetString() );
	
		pWnd = GetDlgItem( IDC_EDIT_HEIGHT );
		Str.Format ( "%3.2f", pSkinCharData->m_fHeight );
		pWnd->SetWindowText( Str.GetString() );
	}
	else
	{
		float fData = 0.f;
		pWnd = GetDlgItem( IDC_EDIT_RADIUS );
		Str.Format ( "%3.2f", fData );
		pWnd->SetWindowText( Str.GetString() );
	
		pWnd = GetDlgItem( IDC_EDIT_HEIGHT );
		Str.Format ( "%3.2f", fData );
		pWnd->SetWindowText( Str.GetString() );
	}

	CButton* pButton = (CButton*)GetDlgItem( IDC_CHECK_NAME_CHF_CFG );
	pButton->SetCheck( TRUE );
	g_bMakeNameChfCfg = TRUE;
}

void CCharPage::OnEnChangeEditRadius()
{
	// TODO:  RICHEDIT ��Ʈ���� ��� �� ��Ʈ����
	// CPropertyPage::����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ�
	// CRichEditCtrl().SetEventMask()�� ȣ���ϵ��� OnInitDialog() �Լ��� ���������� ������
	// �� �˸��� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	if( !pFrame )	return;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	if( !pView )	return;

	DxSkinCharData *pSkinCharData = pView->GetSkinCharData();
	if( !pSkinCharData )	return;

	DxSkinChar *pSkinChar = pView->GetSkinChar();
	if( !pSkinChar )	return;

	CWnd*	pWnd;
	CString Str;
	pWnd = GetDlgItem( IDC_EDIT_RADIUS );
	pWnd->GetWindowText( Str );
	pSkinCharData->m_fRadius = (float)atof( Str.GetString() );

    pSkinCharData->SetRadiusHeight();
	pSkinChar->SetRadiusHeight( pSkinCharData );
}

void CCharPage::OnEnChangeEditHeight()
{
	// TODO:  RICHEDIT ��Ʈ���� ��� �� ��Ʈ����
	// CPropertyPage::����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ�
	// CRichEditCtrl().SetEventMask()�� ȣ���ϵ��� OnInitDialog() �Լ��� ���������� ������
	// �� �˸��� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	if( !pFrame )	return;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	if( !pView )	return;

	DxSkinCharData *pSkinCharData = pView->GetSkinCharData();
	if( !pSkinCharData )	return;

	DxSkinChar *pSkinChar = pView->GetSkinChar();
	if( !pSkinChar )	return;

	CWnd*	pWnd;
	CString Str;
	pWnd = GetDlgItem( IDC_EDIT_HEIGHT );
	pWnd->GetWindowText( Str );
	pSkinCharData->m_fHeight = (float)atof( Str.GetString() );

    pSkinCharData->SetRadiusHeight();
	pSkinChar->SetRadiusHeight( pSkinCharData );
}

void CCharPage::OnDeltaposSpinRadius(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;

	g_bClickSpinRadius = TRUE;
}

void CCharPage::OnDeltaposSpinHeight(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;

	g_bClickSpinHeight = TRUE;
}

void CCharPage::SetSpinControl( int nRadius, int nHeight, int nColorHeight )
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	if( !pFrame )	return;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	if( !pView )	return;

	CWnd*	pWnd;
	CString Str;
	DxSkinCharData *pSkinCharData = pView->GetSkinCharData();
	if (!pSkinCharData)
        return;
	if (pSkinCharData->GetFileName().empty())
        return;

	DxSkinChar *pSkinChar = pView->GetSkinChar();
	if (!pSkinChar)
        return;

	//// Note : ���� ���� ���´�.
	//pWnd = GetDlgItem( IDC_EDIT_RADIUS );
	//pWnd->GetWindowText( Str );
	//pSkinCharData->m_fRadius = (float)atof( Str.GetString() );

	//pWnd = GetDlgItem( IDC_EDIT_HEIGHT );
	//pWnd->GetWindowText( Str );
	//pSkinCharData->m_fHeight = (float)atof( Str.GetString() );

	// Note : ���� ��ȭ��Ŵ
	pSkinCharData->m_fRadius -= (float)nRadius * 0.01f;
	pSkinCharData->m_fHeight -= (float)nHeight * 0.01f;

	// Note : ��ȭ��Ų ���� ǥ����.
	pWnd = GetDlgItem( IDC_EDIT_RADIUS );
	Str.Format ( "%3.2f", pSkinCharData->m_fRadius );
	pWnd->SetWindowText( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_HEIGHT );
	Str.Format ( "%3.2f", pSkinCharData->m_fHeight );
	pWnd->SetWindowText( Str.GetString() );

	pSkinCharData->SetRadiusHeight();
	pSkinChar->SetRadiusHeight( pSkinCharData );

    CPieceContainerPage::SetSpinControl(nRadius, nHeight, nColorHeight);
}

#include "../EngineLib/DxEffect/Char/dxeffchardata.h"
DxEffCharDataArray g_EffCharDataArray;

void CCharPage::OnBnClickedButtonLoadEe()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//	Note : ���� ���͸� �����Ѵ�.
	CString szFilter = "Skin effect |*.effskin_a|";
	
	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg ( TRUE,".effskin_a",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, this );
	dlg.m_ofn.lpstrInitialDir = DxEffcharDataMan::GetInstance().GetPath();

	if ( dlg.DoModal()==IDOK )
	{
		CString sFileName = dlg.GetFileName();
		sFileName.MakeLower ();
		const char *szFileName = sFileName.GetString();

		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
		DxSkinChar *pSkinChar = pView->GetSkinChar();
		DxSkinCharData* pSkinCharData = pView->GetSkinCharData ();

		g_EffCharDataArray.CleanUp();
		BOOL bOK = g_EffCharDataArray.LoadFile ( szFileName );
		if ( !bOK )
		{
			MessageBox ( "DxEffCharDataArray: Failed to read effect file.", szFileName, MB_OK );
			return;
		}

	//	EffCharDataArray.SetSkinCharData ( pSkinCharData, pView->GetD3dDevice() );
	//	pSkinChar->SetCharData ( pSkinCharData, pView->GetD3dDevice() );
	}
}

void CCharPage::OnBnClickedButtonSaveEe()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//	Note : ���� ���͸� �����Ѵ�.
	CString szFilter = "Skin effect |*.effskin_a|";
	
	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg ( FALSE,".effskin_a",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, this );
	dlg.m_ofn.lpstrInitialDir = DxEffcharDataMan::GetInstance().GetPath();

	if ( dlg.DoModal()==IDOK )
	{
		CString sFileName = dlg.GetFileName();
		sFileName.MakeLower ();
		const char *szFileName = sFileName.GetString();

		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	//	DxSkinCharData* pSkinCharData = pView->GetSkinCharData ();

	//	g_EffCharDataArray.Import ( pSkinCharData );
		g_EffCharDataArray.ConvertTPoint( pView->GetD3dDevice() );			// Ÿ�� ����Ʈ ����.
		g_EffCharDataArray.SaveFile ( szFileName );
	}
}

void CCharPage::SetBoneEditTab( bool bEdit )
{

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	if( bEdit )
	{
		m_psheetWithTabFrame->ShowWindow(FALSE);
		m_psheetWithTabAnim->ShowWindow(FALSE);

		SetWin_ShowWindow( this, IDC_STATIC_SHEETTAP1, FALSE );
		SetWin_ShowWindow( this, IDC_STATIC_SHEETTAP2, FALSE );

		m_psheetWithTabBone->SetEditView( TRUE );
		SetWin_ShowWindow( this, IDC_STATIC_SHEETTAP3, TRUE );		

		pView->SetRenderTypeBone();	

	}else{
		m_psheetWithTabFrame->ShowWindow(TRUE);
		m_psheetWithTabAnim->ShowWindow(TRUE);

		SetWin_ShowWindow( this, IDC_STATIC_SHEETTAP1, TRUE );
		SetWin_ShowWindow( this, IDC_STATIC_SHEETTAP2, TRUE );

		m_psheetWithTabBone->SetEditView( FALSE );
		SetWin_ShowWindow( this, IDC_STATIC_SHEETTAP3, FALSE );

		pView->SetRenderTypeDefault();
	}
}

void CCharPage::OnBnClickedCheckNonatkmode()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	pView->SetSkinCharNonAtkMode( GetWin_Check( this, IDC_CHECK_NONATKMODE ) );
}

void CCharPage::OnBnClickedCheckOneColor()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	CButton* pButton = (CButton*)GetDlgItem( IDC_CHECK_ONE_COLOR );

	DxSkinCharData *pSkinCharData = pView->GetSkinCharData();
	if( pSkinCharData )
	{
		if( pButton->GetCheck() )	pSkinCharData->m_dwFlags |= EMSCD2_WORLD_COLOR;
		else						pSkinCharData->m_dwFlags &= ~EMSCD2_WORLD_COLOR;
	}

	DxSkinChar *pSkinChar = pView->GetSkinChar();
	if( pSkinChar )
	{
		if( pButton->GetCheck() )	pSkinChar->m_dwFlags |= EMSCD2_WORLD_COLOR;
		else						pSkinChar->m_dwFlags &= ~EMSCD2_WORLD_COLOR;
	}
}

void CCharPage::OnBnClickedCheckNameChfCfg()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	CButton* pButton = (CButton*)GetDlgItem( IDC_CHECK_NAME_CHF_CFG );

	if( pButton->GetCheck() )	g_bMakeNameChfCfg = TRUE;
	else						g_bMakeNameChfCfg = FALSE;
}

DxSkinPiece* CCharPage::CreateSkinPiece(int nParts)
{
    MIN_ASSERT(nParts >= 0 && nParts <= PIECE_SIZE && _T("Failed to read effect file!"));

    if (m_aSkinPieces[nParts].get() == 0)
    {
        m_aSkinPieces[nParts].reset(new DxSkinPiece);
    }

    return m_aSkinPieces[nParts].get();
}

void CCharPage::SkinPieceClear(int nParts)
{ 
    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
    CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

    if (pView == 0)
    {
        return;
    }

    LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice();
    DxSkinPiece *pSkinPiece = pView->GetSkinPiece(nParts);
    DxSkinChar *pSkinChar = pView->GetSkinChar();
    DxSkinCharData *pSkinCharData = pView->GetSkinCharData();

    if (pSkinChar)
    {
        pSkinChar->ResetPiece(pSkinPiece->m_emType);
    }

    if (pSkinCharData)
    {
        pSkinCharData->SetPiece(pSkinPiece->m_emType, _T(""));
    }

    if (pSkinPiece)
    {
        pSkinPiece->ClearAll();
        pSkinPiece->m_bCompletedLOAD = FALSE;
        DxSkinPieceContainer::GetInstance().DeletePiece(pSkinPiece);
    }

    if (pSkinChar && pSkinCharData && GetSkinPieceSize() == 0)
    {
        pSkinCharData->ClearAll();
        pSkinChar->ClearAll();
    }

    CPieceContainerPage::SkinPieceClear();
}

void CCharPage::SetSkinPiece(DxSkinPiece* pSkinPiece)
{
    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
    CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

    if (pView == 0)
    {
        return;
    }

    LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice();
    DxSkinChar *pSkinChar = pView->GetSkinChar();
    DxSkinCharData *pSkinCharData = pView->GetSkinCharData();

    if (pSkinChar == 0)
    {
        return;
    }

    DxCharPart* pCharPart = pSkinChar->GetPiece(pSkinPiece->m_emType);

    if (pCharPart && pSkinPiece->m_szMeshName)
    {
        pCharPart->SetPart( pSkinChar, pSkinPiece, pd3dDevice, false, NULL );
    }

    if (pSkinChar && pSkinPiece->m_szSkeleton)
    {
        pSkinChar->m_spSkinAniControlThread->LoadSkeletonForEdit(pSkinPiece->m_szSkeleton, pd3dDevice);
        pSkinChar->m_spSkinAniControlThread->ReAttachBoneForEdit();
    }
    else
    {
        pSkinChar->ResetPiece(pSkinPiece->m_emType);
    }

    if (pSkinCharData && pSkinPiece->m_szFileName)
    {
        pSkinCharData->GetSkinAniControlData()->LoadSkeleton(pSkinPiece->m_szSkeleton, pd3dDevice);
        pSkinCharData->SetPiece(pSkinPiece->m_emType, pSkinPiece->m_szFileName);
    }
    else
    {
        pSkinCharData->SetPiece(pSkinPiece->m_emType, _T(""));
    }

    if (m_aSkinPieces[pSkinPiece->m_emType].get() != pSkinPiece)
    {
        m_aSkinPieces[pSkinPiece->m_emType].reset(pSkinPiece);
    }

    CPieceContainerPage::SetSkinPiece(pSkinPiece);
}

void CCharPage::SetSkinPieceType(DxSkinPiece* pSkinPiece)
{
    if (pSkinPiece)
    {
        pSkinPiece->m_emPieceType = PIECETYPE_CHARACTER;
        pSkinPiece->m_emType = static_cast<EMPIECECHAR>(m_nParts);
    }
}

DxCharPart* CCharPage::GetCharPart()
{
    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
    CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

    if (pView == 0)
    {
        return 0;
    }

    DxSkinChar *pSkinChar = pView->GetSkinChar();    

    if (pSkinChar == 0)
    {
        return 0;
    }

    return pSkinChar->GetPiece(m_nParts);
}

int CCharPage::GetSkinPieceSize()
{
    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
    CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

    if (pView == 0)
    {
        return 0;
    }

    DxSkinChar *pSkinChar = pView->GetSkinChar();

    if (pSkinChar == 0)
    {
        return 0;
    }

    int nSize = 0;

    for (int i = 0; i < PIECE_SIZE; ++i)
    {
        DxCharPart* pPart = pSkinChar->GetPiece(i);

        if (pPart && pPart->GetPart())
        {
            ++nSize;
        }
    }

    return nSize;
}

bool CCharPage::IsSameBoneFile(const TCHAR* pName)
{
    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
    CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

    if (pView == 0)
    {
        return 0;
    }

    DxSkinChar *pSkinChar = pView->GetSkinChar();    

    if (pSkinChar == 0)
    {
        return 0;
    }

    const TCHAR* pChar = pSkinChar->m_spSkinAniControlThread->GetSkeletonName();    

    if (pChar != 0 && _tcsclen(pChar) != 0 && _tcsicmp(pChar, pName) != 0)
    {
        TCHAR temp[256], ext[256], temp2[256], ext2[256];
        _tsplitpath(pChar, 0, 0, temp, ext);
        _tsplitpath(pName, 0, 0, temp2, ext2);

        // ��ȣȭ�� �������� üũ �Ѵ�.
        if ( (_tcsicmp(ext, _T(".x")) == 0 && _tcsicmp(ext2, _T(".mxf")) == 0) ||
             (_tcsicmp(ext, _T(".mxf")) == 0 && _tcsicmp(ext2, _T(".x")) == 0) )
        {
            if (_tcsicmp(temp, temp2) == 0)
            {
                return true;
            }
        }

        return false;
    }

    return true;
}

bool CCharPage::HasParts(DxSkinPiece* pTempSkinPiece)
{
    DxSkinPiece* pSkinPiece = GetSkinPiece(pTempSkinPiece->m_emType);

    if (pSkinPiece && pSkinPiece->m_szXFileName && pTempSkinPiece && pTempSkinPiece->m_szXFileName)
    {
        if (pSkinPiece->m_emType == pTempSkinPiece->m_emType)
        {
            return true;
        }
    }

    return false;
}

void CCharPage::ReSetPages()
{
    for (int i = 0; i < PIECE_SIZE; ++i)
    {
        DxSkinPiece* pPiece = GetSkinPiece(i);

        if (pPiece && pPiece->m_szFileName)
        {
            pPiece->ClearAll();
        }
    }

    CPieceContainerPage::ReSetPages();
}

void CCharPage::SwapSkinPiece(int nParts)
{
    if (nParts == m_nParts)
    {
        return;
    }

    DxSkinPiece* pSkinPiece1 = GetSkinPiece();
    DxSkinPiece* pSkinPiece2 = GetSkinPiece(nParts);

    std::swap(pSkinPiece1->m_emType, pSkinPiece2->m_emType);

    m_aSkinPieces[m_nParts].swap(m_aSkinPieces[nParts]);

    SetSkinPiece(pSkinPiece1);
    SetSkinPiece(pSkinPiece2);

    m_nParts = nParts;

    CPieceContainerPage::SwapSkinPiece(nParts);
}

void CCharPage::ReplaceSkinPiece(int nParts)
{
    if (nParts == m_nParts)
    {
        return;
    }

    DxSkinPiece* pSkinPiece1 = GetSkinPiece();
    DxSkinPiece* pSkinPiece2 = GetSkinPiece(nParts);

    std::swap(pSkinPiece1->m_emType, pSkinPiece2->m_emType);

    m_aSkinPieces[m_nParts].swap(m_aSkinPieces[nParts]);
    
    SetSkinPiece(pSkinPiece1);
    SkinPieceClear(pSkinPiece2->m_emType);

    m_nParts = nParts;

    CPieceContainerPage::SwapSkinPiece(nParts);
}

void CCharPage::ShowWindow(bool bShow)
{
    int wndList[] = 
    {
        IDC_BUTTON_NEW,
        IDC_BUTTON_OPEN,
        IDC_BUTTON_SAVE,
        IDC_BUTTON_ALLSAVE,
        IDC_EDIT_CHARFILE,
        IDC_BUTTON_LOAD_EE,
        IDC_BUTTON_SAVE_EE,
        IDC_CHECK_NOCHARACTER,
        IDC_CHECK_ONE_COLOR,
        IDC_CHECK_NAME_CHF_CFG,
        IDC_EDIT_RADIUS,
        IDC_SPIN_RADIUS,
        IDC_EDIT_HEIGHT,
        IDC_SPIN_HEIGHT,
        IDC_CHECK_NONATKMODE,
    };

    int nSize = sizeof(wndList) / sizeof(int);

    for (int i = 0; i < nSize; ++i)
    {
        CWnd* pWnd = GetDlgItem (wndList[i]);

        if (pWnd)
        {
            pWnd->ShowWindow(bShow ? SW_SHOW : SW_HIDE);
        }
    }

    CPieceContainerPage::ShowWindow(bShow);
}
