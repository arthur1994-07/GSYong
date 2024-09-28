// Page02.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "RanSNS.h"
#include "Page02.h"
#include "WebDlg.h"
#include "Twitter.h"
#include "ConsoleMessage.h"
#include ".\page02.h"


// CPage02 대화 상자입니다.

IMPLEMENT_DYNAMIC(CPage02, CPropertyPage)
CPage02::CPage02()
	: CPropertyPage(CPage02::IDD)
	, m_bUserID(FALSE)
{
	m_pTwitter = CTwitter::GetInstance();
}

CPage02::~CPage02()
{
	m_pTwitter->CleanUp();
	m_pTwitter = NULL;
}

void CPage02::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MSG_TW, m_lbMsgTW);
	DDX_Control(pDX, IDC_BUTTON_TWEETMSG, m_btTweetMsg);
	DDX_Control(pDX, IDC_EDIT_TWEETMSG, m_ebTweetMsg);
	DDX_Control(pDX, IDC_EDIT_FOLLOW, m_ebFollow);
	DDX_Control(pDX, IDC_BUTTON_FOLLOW, m_btFollow);
	DDX_Control(pDX, IDC_BUTTON_UNFOLLOW, m_btUnFollow);
	DDX_Check(pDX, IDC_CHECK_FOLLOW, m_bUserID);
}


BEGIN_MESSAGE_MAP(CPage02, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_AUTH02, OnBnClickedButtonAuth)
	ON_BN_CLICKED(IDC_BUTTON_TWEETMSG, OnBnClickedButtonTweetmsg)
	ON_BN_CLICKED(IDC_BUTTON_FOLLOW, OnBnClickedButtonFollow)
	ON_BN_CLICKED(IDC_BUTTON_UNFOLLOW, OnBnClickedButtonUnfollow)
END_MESSAGE_MAP()




BOOL CPage02::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_btTweetMsg.EnableWindow(FALSE);
	m_btFollow.EnableWindow(FALSE);
	m_btUnFollow.EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}



// CPage02 메시지 처리기입니다.

void CPage02::OnBnClickedButtonAuth()
{
	//m_pTwitter->LinkConsole(&m_lbMsgTW);

	m_btTweetMsg.EnableWindow(FALSE);
	m_btFollow.EnableWindow(FALSE);
	m_btUnFollow.EnableWindow(FALSE);


	// Twitter routine test here. (using libOAuth)

	char RequestToken[256]={0,};
	char RequestTokenSecret[256]={0,};
	char AccessToken[256]={0,};
	char AccessTokenSecret[256]={0,};

	char *res_t_key    = NULL; //< replied key
	char *res_t_secret = NULL; //< replied secret

	char *req_url = NULL;	
	char *postarg = NULL;


	//
	// 1. Acquiring RequestToken
	//
	if ( !m_pTwitter->GetRequestToken() )
	{
		CConsoleMessage::GetInstance()->PrintTo(&m_lbMsgTW, _T("GetRequestToken failure."));
		
		return;
	}
	CConsoleMessage::GetInstance()->PrintTo(&m_lbMsgTW, _T("GetRequestToken done."));


	//
	// 2. Compose a URL for Authorization
	//
	if ( !m_pTwitter->MakeAuthURL() )
	{
		CConsoleMessage::GetInstance()->PrintTo(&m_lbMsgTW, _T("Authorize failure."));

		return;
	}
	CConsoleMessage::GetInstance()->PrintTo(&m_lbMsgTW, _T("Authorize success."));

	//
	// 3. Authorize (Input PIN code) on web-browser
	//	
	m_dlgWeb.SetURL(m_pTwitter->GetAuthURL(), TRUE);	//pURL is INPUT & OUTPUT variable. It contains last URL when CWebDlg quits.

	INT_PTR nRet = -1;
	
	nRet = m_dlgWeb.DoModal();

	if ( m_dlgWeb.m_csPIN.GetLength() <= 0 )
	{
		CConsoleMessage::GetInstance()->PrintTo(&m_lbMsgTW, _T("PIN failure."));

		return;
	}
  
	//
	// Now we got the PIN code.
	//

	
	//
	// 4.Exchanging a RequestToken for an AccessToken
	//	
	if ( !m_pTwitter->GetAccessToken( m_dlgWeb.m_csPIN.GetBuffer() ) )
	{
		CConsoleMessage::GetInstance()->PrintTo(&m_lbMsgTW, _T("GetAccessToken failure."));

		return;
	}
	CConsoleMessage::GetInstance()->PrintTo(&m_lbMsgTW, _T("GetAccessToken success."));


	m_btTweetMsg.EnableWindow(TRUE);
	m_btFollow.EnableWindow(TRUE);
	m_btUnFollow.EnableWindow(TRUE);
}



void CPage02::OnBnClickedButtonTweetmsg()
{
	// Write some messages to Twitter

	CString csMsg;

	m_ebTweetMsg.GetWindowText(csMsg);

	if (csMsg.GetLength() == 0)
	{
		CConsoleMessage::GetInstance()->PrintTo(&m_lbMsgTW, _T("TweetMsg requires Message."));

		return;
	}

    std::string msg(csMsg.GetString());

	if ( !m_pTwitter->TweetMsg( msg ) )
	{
		CConsoleMessage::GetInstance()->PrintTo(&m_lbMsgTW, _T("TweetMsg failure."));

		return;
	}

	CConsoleMessage::GetInstance()->PrintTo(&m_lbMsgTW, _T("TweetMsg Success."));	
}

void CPage02::OnBnClickedButtonFollow()
{
	// Follow someone

	UpdateData(TRUE);

	CString csName;

	m_ebFollow.GetWindowText(csName);

	if (csName.GetLength() == 0)
	{
		CConsoleMessage::GetInstance()->PrintTo(&m_lbMsgTW, _T("Follow requires UserID or ScrName."));

		return;
	}

    std::string name(csName.GetString());

	if ( !m_pTwitter->Follow( m_bUserID, name ) )
	{
		CConsoleMessage::GetInstance()->PrintTo(&m_lbMsgTW, _T("Follow failure."));

		return;
	}

	CConsoleMessage::GetInstance()->PrintTo(&m_lbMsgTW, _T("Follow Success."));	
}

void CPage02::OnBnClickedButtonUnfollow()
{
	// UnFollow someone

	UpdateData(TRUE);

	CString csName;

	m_ebFollow.GetWindowText(csName);

	if (csName.GetLength() == 0)
	{
		CConsoleMessage::GetInstance()->PrintTo(&m_lbMsgTW, _T("UnFollow requires UserID or ScrName."));

		return;
	}

    std::string name(csName.GetString());

	if ( !m_pTwitter->UnFollow( m_bUserID, name ) )
	{
		CConsoleMessage::GetInstance()->PrintTo(&m_lbMsgTW, _T("UnFollow failure."));

		return;
	}

	CConsoleMessage::GetInstance()->PrintTo(&m_lbMsgTW, _T("UnFollow Success."));	
}
