// BgmEdit.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "WorldEdit.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"
#include "MainFrm.h"

#include "BgmEdit.h"
#include "sheetWithTab.h"
#include "State/WorldStateMachine.h"

#include "../EngineLib/DxSound/BgmSound.h"

// CBgmEdit 대화 상자입니다.

IMPLEMENT_DYNAMIC(CBgmEdit, CPropertyPage)
CBgmEdit::CBgmEdit()
	: CPropertyPage(CBgmEdit::IDD)
{
	m_bDlgInit = FALSE;
}

CBgmEdit::~CBgmEdit()
{
}

void CBgmEdit::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CBgmEdit, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_FILE, OnBnClickedButtonFile)
	ON_BN_CLICKED(IDC_BUTTON_PLAY, OnBnClickedButtonPlay)
	ON_BN_CLICKED(IDC_BUTTON_STOP, OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, OnBnClickedButtonClear)
	ON_WM_ACTIVATE()
	ON_BN_CLICKED(IDC_BUTTON_VOLUME, OnBnClickedButtonVolume)
END_MESSAGE_MAP()


// CBgmEdit 메시지 처리기입니다.
void CBgmEdit::UpdateContrl ()
{	
	if ( !m_bDlgInit )
	{
		return ;
	}

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	SBGMDATA* pBGMDATA = pView->GetSetLandMan()->GetBGMDATA ();

	if (pBGMDATA)
        DxBgmSound::GetInstance().SetFile(pBGMDATA->strFileName);

	CString file = DxBgmSound::GetInstance().GetFile ();
	GetDlgItem ( IDC_EDIT_FILE )->SetWindowText ( file );

	CString volume;
	volume.Format ( "%d", (LONG)DxBgmSound::GetInstance().GetVolume () );
	GetDlgItem ( IDC_EDIT_VOLUME )->SetWindowText ( volume );
}

void CBgmEdit::OnBnClickedButtonFile()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	char szFilters[]= "Ogg File (*.ogg)|*.ogg||";

	CFileDialog fileDlg (TRUE, "ogg", "*.ogg",
		OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, szFilters, this);
	fileDlg.m_ofn.lpstrInitialDir = DxBgmSound::GetInstance().GetPath ();
	
	if( fileDlg.DoModal ()==IDOK )
	{
		CString fileName = fileDlg.GetFileName();
		GetDlgItem ( IDC_EDIT_FILE )->SetWindowText ( fileName );

		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

		SBGMDATA* pBGMDATA = pView->GetSetLandMan()->GetBGMDATA ();
		pBGMDATA->SetFile ( fileName );

        WorldStateMachine::EditAsset(fileName);

		DxBgmSound::GetInstance().SetFile ( fileName.GetString() );
	}
}

void CBgmEdit::OnBnClickedButtonPlay()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	DxBgmSound::GetInstance().Play ();
}

void CBgmEdit::OnBnClickedButtonStop()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	DxBgmSound::GetInstance().Stop ();
}

void CBgmEdit::OnBnClickedButtonClear()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();    

	SBGMDATA* pBGMDATA = pView->GetSetLandMan()->GetBGMDATA ();
    WorldStateMachine::EditAsset(pBGMDATA->strFileName.GetBuffer());
	pBGMDATA->ClearFile ();

	GetDlgItem ( IDC_EDIT_FILE )->SetWindowText ( NULL );
	DxBgmSound::GetInstance().ForceStop ();
	DxBgmSound::GetInstance().ClearFile ();	
}

BOOL CBgmEdit::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_bDlgInit = TRUE;
	UpdateContrl ();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CBgmEdit::OnBnClickedButtonVolume()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString str;
	GetDlgItem ( IDC_EDIT_VOLUME )->GetWindowText ( str );
	DxBgmSound::GetInstance().SetVolume ( (LONG) atoi ( str.GetString () ) );
}
