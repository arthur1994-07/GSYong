// AttBonePage.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "../CharEdit.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "SheetWithTabChar.h"
#include "sheetWithTabFrame.h"
#include "SheetWithTabAnim.h"
#include "sheetWithTabPiece.h"

#include "../SigmaCore/Util/Util.h"

#include "../../EngineLib/DxTools/DebugSet.h"
#include "../../EngineLib/DxTools/DxMethods.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinObject.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxAttBone.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxAttBoneData.h"

#include "../MfcExLib/ExLibs/EtcFunction.h"

#include "../State/CharStateMachine.h"

#include "AttBonePage.h"



extern BOOL	g_bClickSpinRadius;
extern BOOL	g_bClickSpinHeight;


// CAttBonePage ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CAttBonePage, CPropertyPage)
CAttBonePage::CAttBonePage() : 
    CPieceContainerPage(CCharPage::IDD)
{
    
}

CAttBonePage::~CAttBonePage()
{
}

void CAttBonePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAttBonePage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_NEW, OnBnClickedButtonNew)
	ON_BN_CLICKED(IDC_BUTTON_OPEN, OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_CHECK_NOCHARACTER, OnBnClickedCheckNocharacter)
	ON_EN_CHANGE(IDC_EDIT_RADIUS, OnEnChangeEditRadius)
	ON_EN_CHANGE(IDC_EDIT_HEIGHT, OnEnChangeEditHeight)
END_MESSAGE_MAP()


// CAttBonePage �޽��� ó�����Դϴ�.

BOOL CAttBonePage::OnInitDialog()
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
	m_psheetWithTabFrame->m_FramePage.SetFrameEditType( EMFRAMEEDIT_ATTBONE );

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

    //	Note : CsheetWithTabPiece �� �����.
    //
//     m_psheetWithTabPiece = new CsheetWithTabPiece ( SheetRect, SheetRectAll, this );
//     m_psheetWithTabPiece->Create ( this, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0 );
//     m_psheetWithTabPiece->ShowWindow(SW_HIDE);

	pWnd = GetDlgItem ( IDC_EDIT_CHARFILE );
	pWnd->SetWindowText ( "untitle00.abf" );

	// Note : ��ư �� ����
	ReSetCheckButton();

	// Note : ��ư �� ����
	ReSetEditBox();

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CAttBonePage::OnBnClickedButtonNew()
{
	if( m_hWnd == NULL ) return;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	DxAttBone *pAttBone = pView->GetAttBone();
	DxAttBoneData* pAttBoneData = pView->GetAttBoneData ();

	CWnd *pWnd;
	pWnd = GetDlgItem ( IDC_EDIT_CHARFILE );
	pWnd->SetWindowText ( "untitle00.abf" );

	pAttBoneData->ClearAll ();
	pAttBone->ClearAll ();

	CharStateMachine::ClearFileNotifier();

	m_psheetWithTabFrame->ReSetPages ();
	m_psheetWithTabAnim->ReSetPages ();
#ifdef DEF_SKIN_SYNC
    m_psheetWithTabPiece->ReSetPages ();
#endif

	// Note : ��ư �� ����
	ReSetCheckButton();

	// Note : ��ư �� ����
	ReSetEditBox();
}

VOID	CAttBonePage::InsertSkinPiece( const char *pszFileName )
{
	m_psheetWithTabFrame->m_FramePage.InsertSkinPiece(pszFileName);
}

VOID	CAttBonePage::InsertSkinEff( const char *pszFileName )
{
	m_psheetWithTabFrame->m_FramePage.InsertSkinEff(pszFileName);
}

VOID	CAttBonePage::LoadAnimation( const char *pszFileName )
{
	m_psheetWithTabAnim->m_AnimTabPage.GetAnimPage()->LoadAnimation(pszFileName);
}

VOID	CAttBonePage::LoadSkinBone( const char *pszFileName )
{
	m_psheetWithTabFrame->m_FramePage.LoadSkinBone(pszFileName);
}

BOOL CAttBonePage::LoadAbfFile( const char *pszFileName )
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice ();
	DxAttBone *pAttBone = pView->GetAttBone();
	DxAttBoneData *pAttBoneData = pView->GetAttBoneData();

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
    BOOL bOk = pAttBoneData->LoadFile(std::string(pszFileName), pd3dDevice, EMSCD_ZERO, FG_BONEFILE_CHECK|FG_MUSTNEWLOAD );
	if ( !bOk )
	{
		MessageBox ( "���� �б⿡ �����Ͽ����ϴ�." );
		return FALSE;
	}

	//	Note : �ɸ��� Data ����.
	//
	pAttBone->SetAttBoneData ( pAttBoneData, pd3dDevice );

#ifdef DEF_SKIN_SYNC
    for (int i = 0; i < ATTBONE_SIZE; ++i)
    {
        TSTRING strFile = pAttBoneData->GetAttBoneName(EMPIECECHAR(i));

        if (m_psheetWithTabPiece && strFile.empty() == false)
        {            
            m_psheetWithTabPiece->GetSkinPiecePage()->LoadSkinPiece(strFile.c_str());
        }
    }
#endif

	CWnd *pWnd;
	pWnd = GetDlgItem ( IDC_EDIT_CHARFILE );
	pWnd->SetWindowText ( pszFileName );

	m_psheetWithTabFrame->ReSetPages ();
	m_psheetWithTabAnim->SetPages ( NULL, pAttBone->m_spSkinAniControlThread.get() );

	// Note : ��ư �� ����
	ReSetCheckButton();

	// Note : ��ư �� ����
	ReSetEditBox();

	pView->ResetSkinPos();

	pFrame->m_cDlgTimeLine.SetStop();

	CharStateMachine::InsertFileNotifier();

	return TRUE;
}

void CAttBonePage::OnBnClickedButtonOpen()
{
	CString szFilter = "Attach Bone File (*.abf)|*.abf|";
	
	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(TRUE,".abf",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CAttBonePage*)this);

	dlg.m_ofn.lpstrInitialDir = DxAttBoneDataContainer::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		CString sFileName = dlg.GetFileName();

		LoadAbfFile( sFileName.GetString() );
	}
}

void CAttBonePage::OnBnClickedButtonSave()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice ();

	DxAttBone *pAttBone = pView->GetAttBone();
	if( !pAttBone )		return;
	DxAttBoneData *pAttBoneData = pView->GetAttBoneData();
	if( !pAttBoneData )	return;

	std::string strName = sc::util::ChangeExtName( pAttBoneData->GetFileName(), "" );

	//	Note : ���� ���� ���̾˷α׸� ����.
	CString szFilter = "Attach Bone File (*.abf)|*.abf|";
	CFileDialog dlg(FALSE,".abf", strName.c_str() ,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, (CAttBonePage*)this);

	dlg.m_ofn.lpstrInitialDir = DxAttBoneDataContainer::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		CString sFileName = dlg.GetFileName();

		//	Note : �ɸ��� ���� ����.
		//
		BOOL bOk = pAttBoneData->SaveFile ( sFileName.GetString(), pd3dDevice );

		if ( !bOk )
		{
			MessageBox ( "���� ���⿡ �����Ͽ����ϴ�." );
		}

		CWnd *pWnd;
		pWnd = GetDlgItem ( IDC_EDIT_CHARFILE );
		pWnd->SetWindowText ( sFileName.GetString() );
	}
}

void CAttBonePage::OnBnClickedCheckNocharacter()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	CButton* pButton = (CButton*)GetDlgItem( IDC_CHECK_NOCHARACTER );

	DxAttBone *pAttBone = pView->GetAttBone();
	if( pAttBone )
	{
		if( pButton->GetCheck() )	pAttBone->SetWorldObj( TRUE );
		else						pAttBone->SetWorldObj( FALSE );
	}

	DxAttBoneData *pAttBoneData = pView->GetAttBoneData();
	if( pAttBoneData )
	{
		if( pButton->GetCheck() )	pAttBoneData->m_bWorldObj = TRUE;
		else						pAttBoneData->m_bWorldObj = FALSE;
	}
}

void CAttBonePage::ReSetCheckButton()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	if( !pFrame )	return;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	if( !pView )	return;

	CButton* pButton = (CButton*)GetDlgItem( IDC_CHECK_NOCHARACTER );
	DxAttBone *pAttBone = pView->GetAttBone();
	if( pAttBone )
	{
		if( pAttBone->GetWorldObj() )	pButton->SetCheck( TRUE );
		else							pButton->SetCheck( FALSE );
	}
	else
	{
		pButton->SetCheck( FALSE );
	}
}

void CAttBonePage::ReSetEditBox()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	if( !pFrame )	return;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	if( !pView )	return;

	CWnd*	pWnd;
	CString Str;
	DxAttBoneData *pAttBoneData = pView->GetAttBoneData();
	if( pAttBoneData )
	{
		pWnd = GetDlgItem( IDC_EDIT_RADIUS );
		Str.Format ( "%3.2f", pAttBoneData->m_fRadius );
		pWnd->SetWindowText( Str.GetString() );

		pWnd = GetDlgItem( IDC_EDIT_HEIGHT );
		Str.Format ( "%3.2f", pAttBoneData->m_fHeight );
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

}

void CAttBonePage::OnEnChangeEditRadius()
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

	DxAttBoneData *pAttBoneData = pView->GetAttBoneData();
	if( !pAttBoneData )	return;

	DxAttBone *pAttBone = pView->GetAttBone();
	if( !pAttBone )	return;

	CWnd*	pWnd;
	CString Str;
	pWnd = GetDlgItem( IDC_EDIT_RADIUS );
	pWnd->GetWindowText( Str );
	pAttBoneData->m_fRadius = (float)atof( Str.GetString() );

	pAttBoneData->SetRadiusHeight();
	pAttBone->SetRadiusHeight( pAttBoneData );
}

void CAttBonePage::OnEnChangeEditHeight()
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

	DxAttBoneData *pAttBoneData = pView->GetAttBoneData();
	if( !pAttBoneData )	return;

	DxAttBone *pAttBone = pView->GetAttBone();
	if( !pAttBone )	return;

	CWnd*	pWnd;
	CString Str;
	pWnd = GetDlgItem( IDC_EDIT_HEIGHT );
	pWnd->GetWindowText( Str );
	pAttBoneData->m_fHeight = (float)atof( Str.GetString() );

	pAttBoneData->SetRadiusHeight();
	pAttBone->SetRadiusHeight( pAttBoneData );
}

DxSkinPiece* CAttBonePage::CreateSkinPiece(int nParts)
{
    MIN_ASSERT(nParts >= 0 && nParts <= ATTBONE_SIZE && _T("���� �ε��� ����!!!"));

    if (m_aSkinPieces[nParts].get() == 0)
    {
        m_aSkinPieces[nParts].reset(new DxSkinPiece);
    }

    return m_aSkinPieces[nParts].get();
}

void CAttBonePage::SkinPieceClear(int nParts)
{
    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
    CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

    if (pView == 0)
    {
        return;
    }

    DxSkinPiece *pSkinPiece = pView->GetSkinPiece(nParts);
    DxAttBone *pAttBone = pView->GetAttBone();
    DxAttBoneData *pAttBoneData = pView->GetAttBoneData();

    if (pAttBone)
    {
        pAttBone->ResetAttBonePiece(pSkinPiece->m_emAttBoneType);
    }

    if (pAttBoneData)
    {
        pAttBoneData->SetAttBonePiece(pSkinPiece->m_emAttBoneType, _T(""));
    }

    if (pSkinPiece)
    {
        pSkinPiece->ClearAll();
        pSkinPiece->m_bCompletedLOAD = FALSE;
        DxSkinPieceContainer::GetInstance().DeletePiece(pSkinPiece);
    }

    if (pAttBone && pAttBoneData && GetSkinPieceSize() == 0)
    {
        pAttBoneData->ClearAll();
        pAttBone->ClearAll();
    }

    CPieceContainerPage::SkinPieceClear();
}

void CAttBonePage::SetSkinPiece(DxSkinPiece* pSkinPiece)
{
    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
    CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

    if (pView == 0)
    {
        return;
    }

    LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice();
    DxAttBone *pAttBone = pView->GetAttBone();
    DxAttBoneData *pAttBoneData = pView->GetAttBoneData();

    if (pAttBone == 0)
    {
        return;
    }    

    DxCharPart* pCharPart = pAttBone->GetAttBonePiece(pSkinPiece->m_emAttBoneType);

    if (pCharPart && pSkinPiece->m_szMeshName)
    {
        pCharPart->SetPart( pAttBone, pSkinPiece, pd3dDevice, false, NULL );
    }

    if (pAttBone && pSkinPiece->m_szSkeleton)
    {
        pAttBone->m_spSkinAniControlThread->LoadSkeletonForEdit(pSkinPiece->m_szSkeleton, pd3dDevice);
        pAttBone->m_spSkinAniControlThread->ReAttachBoneForEdit();
    }
    else
    {
        pAttBone->ResetAttBonePiece(pSkinPiece->m_emAttBoneType);
    }

    if (pAttBoneData && pSkinPiece->m_szFileName)
    {
        pAttBoneData->GetSkinAniControlData()->LoadSkeleton(pSkinPiece->m_szSkeleton, pd3dDevice);
        pAttBoneData->SetAttBonePiece(pSkinPiece->m_emAttBoneType, pSkinPiece->m_szFileName);
    }
    else
    {
        pAttBoneData->SetAttBonePiece(pSkinPiece->m_emAttBoneType, _T(""));
    }

    if (m_aSkinPieces[pSkinPiece->m_emAttBoneType].get() != pSkinPiece)
    {
        m_aSkinPieces[pSkinPiece->m_emAttBoneType].reset(pSkinPiece);
    }

    CPieceContainerPage::SetSkinPiece(pSkinPiece);
}

void CAttBonePage::SetSkinPieceType(DxSkinPiece* pSkinPiece)
{
    if (pSkinPiece)
    {
        pSkinPiece->m_emPieceType = PIECETYPE_ATTACHBONE;
        pSkinPiece->m_emAttBoneType = static_cast<EMPIECEATTBONE>(m_nParts);
    }
}

DxCharPart* CAttBonePage::GetCharPart()
{
    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
    CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

    if (pView == 0)
    {
        return 0;
    }

    DxAttBone *pAttBone = pView->GetAttBone();    

    if (pAttBone == 0)
    {
        return 0;
    }

    return pAttBone->GetAttBonePiece(m_nParts);
}

int CAttBonePage::GetSkinPieceSize()
{
    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
    CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

    if (pView == 0)
    {
        return 0;
    }

    DxAttBone *pAttBone = pView->GetAttBone();

    if (pAttBone == 0)
    {
        return 0;
    }    

    int nSize = 0;

    for (int i = 0; i < ATTBONE_SIZE; ++i)
    {
        DxCharPart* pPart = pAttBone->GetAttBonePiece(i);

        if (pPart && pPart->GetPart())
        {
            ++nSize;
        }
    }

    return nSize;
}

bool CAttBonePage::IsSameBoneFile(const TCHAR* pName)
{
    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
    CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

    if (pView == 0)
    {
        return 0;
    }

    DxAttBone *pAttBone = pView->GetAttBone();

    if (pAttBone == 0)
    {
        return 0;
    }

    const TCHAR* pChar = pAttBone->m_spSkinAniControlThread->GetSkeletonName();

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

bool CAttBonePage::HasParts(DxSkinPiece* pTempSkinPiece)
{
    DxSkinPiece* pSkinPiece = GetSkinPiece(pTempSkinPiece->m_emAttBoneType);

    if (pSkinPiece && pSkinPiece->m_szXFileName && pTempSkinPiece && pTempSkinPiece->m_szXFileName)
    {
        if (pSkinPiece->m_emAttBoneType == pTempSkinPiece->m_emAttBoneType)
        {
            return true;
        }
    }

    return false;
}

void CAttBonePage::ReSetPages()
{
    for (int i = 0; i < ATTBONE_SIZE; ++i)
    {
        DxSkinPiece* pPiece = GetSkinPiece(i);

        if (pPiece && pPiece->m_szFileName)
        {
            pPiece->ClearAll();
        }
    }

    CPieceContainerPage::ReSetPages();
}

void CAttBonePage::SwapSkinPiece(int nParts)
{
    if (nParts == m_nParts)
    {
        return;
    }

    DxSkinPiece* pSkinPiece1 = GetSkinPiece();
    DxSkinPiece* pSkinPiece2 = GetSkinPiece(nParts);

    std::swap(pSkinPiece1->m_emAttBoneType, pSkinPiece2->m_emAttBoneType);

    m_aSkinPieces[m_nParts].swap(m_aSkinPieces[nParts]);

    SetSkinPiece(pSkinPiece1);
    SetSkinPiece(pSkinPiece2);

    m_nParts = nParts;

    CPieceContainerPage::SwapSkinPiece(nParts);
}

void CAttBonePage::ReplaceSkinPiece(int nParts)
{
    if (nParts == m_nParts)
    {
        return;
    }

    DxSkinPiece* pSkinPiece1 = GetSkinPiece();
    DxSkinPiece* pSkinPiece2 = GetSkinPiece(nParts);

    std::swap(pSkinPiece1->m_emAttBoneType, pSkinPiece2->m_emAttBoneType);

    m_aSkinPieces[m_nParts].swap(m_aSkinPieces[nParts]);

    SetSkinPiece(pSkinPiece1);
    SkinPieceClear(pSkinPiece2->m_emAttBoneType);

    m_nParts = nParts;

    CPieceContainerPage::SwapSkinPiece(nParts);
}

void CAttBonePage::ShowWindow(bool bShow)
{
    int wndList[] = 
    {
        IDC_BUTTON_NEW,
        IDC_BUTTON_OPEN,
        IDC_BUTTON_SAVE,
        IDC_BUTTON_ALLSAVE,
        IDC_EDIT_CHARFILE,
        IDC_EDIT_RADIUS,
        IDC_SPIN_RADIUS,
        IDC_EDIT_HEIGHT,
        IDC_SPIN_HEIGHT,
        IDC_CHECK_NOCHARACTER,
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