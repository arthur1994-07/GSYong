// Page01.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "RanSNS.h"
#include "Page01.h"
#include "WebDlg.h"
#include "FaceBook.h"
#include "ConsoleMessage.h"



// CPage01 대화 상자입니다.

IMPLEMENT_DYNAMIC(CPage01, CPropertyPage)
CPage01::CPage01()
	: CPropertyPage(CPage01::IDD)
{
	m_pFacebook = CFacebook::GetInstance();


	//
	//following steps MUST be done automatically by configurations.
	//

	FBCUSTOMINFOS custominfo;

	//SNS tool uses only three of urls.
	URLINFO IMAGELOGO;
	URLINFO HREF;
	URLINFO ACTIONLINK;

	/*
	URLINFO LOGINCANCEL;
	URLINFO LOGOUTNEXT;	
	URLINFO IMAGEWELCOME;
	URLINFO IMAGELEVELUP;
	URLINFO IMAGEENCHANTED;
	*/
	

	IMAGELOGO.url	= std::string("http://file.ran-world.com/sns/ko/logo.gif");
	IMAGELOGO.name	= std::string("RAN Online");
	IMAGELOGO.desc	= std::string("란 온라인 클라이언트를 이용하여 작성한 메시지입니다.");

	HREF.url		= std::string("http://www.facebook.com/apps/application.php?id=151612284850725");

	ACTIONLINK.url	= std::string("http://www.ran-world.com/");
	ACTIONLINK.name	= std::string("Start RAN Online");

	custominfo.urlIMAGELOGO		= IMAGELOGO;
	custominfo.urlHREF			= HREF;
	custominfo.urlACTIONLINK	= ACTIONLINK;

	m_pFacebook->SetupCustomInfo(custominfo);	
}

CPage01::~CPage01()
{
	m_pFacebook->CleanUp();
	m_pFacebook = NULL;
	//delete m_pFacebook;
	//m_pFacebook = NULL;
}

void CPage01::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MSG_FB, m_lbMsgFB);
	DDX_Control(pDX, IDC_BUTTON_WALLPOST, m_btWallPost);
	DDX_Control(pDX, IDC_EDIT_WALLMSG, m_ebWallMsg);
	DDX_Control(pDX, IDC_BUTTON_PUBLISH_PHOTOS, m_btPublishPhotos);
	DDX_Control(pDX, IDC_EDIT_CAPTION, m_ebCaption);
	DDX_Control(pDX, IDC_EDIT_PATH, m_ebPath);
}


BEGIN_MESSAGE_MAP(CPage01, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_AUTH, OnBnClickedButtonAuth)
	ON_BN_CLICKED(IDC_BUTTON_WALLPOST, OnBnClickedButtonWallpost)
	ON_BN_CLICKED(IDC_BUTTON_PUBLISH_PHOTOS, OnBnClickedButtonPublishPhotos)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnBnClickedButtonBrowse)
END_MESSAGE_MAP()


// CPage01 메시지 처리기입니다.

BOOL CPage01::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_btWallPost.EnableWindow(FALSE);
	m_btPublishPhotos.EnableWindow(FALSE);

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CPage01::OnBnClickedButtonAuth()
{
	//m_pFacebook->LinkConsole(&m_lbMsgFB);

	m_btWallPost.EnableWindow(FALSE);
	m_btPublishPhotos.EnableWindow(FALSE);

	//m_pFacebook->GetAuthToken();
	//char *pURL = "https://graph.facebook.com/oauth/authorize?client_id=151612284850725&redirect_uri=http://www.facebook.com/connect/login_success.html&scope=publish_stream";
	//m_pFacebook->GetAccessToken(&pURL);


	//
	// 1. Get AuthToken
	//
	m_pFacebook->GetAuthToken();


	//
	// 2.Authenticate & Authorize
	//
    std::string authURL;	

	m_pFacebook->GetAuthURL(authURL);
	

	//ShellExecute( NULL, "open", url.c_str( ), NULL, NULL, SW_SHOWNORMAL );
	//CWebDlg br;

	m_dlgWeb.SetURL(authURL.c_str(), FALSE);	//pURL is INPUT & OUTPUT variable. It contains last URL when CWebDlg quits.

	INT_PTR nRet = -1;	
	
	//br.OnSize(
	nRet = m_dlgWeb.DoModal();
	

	//
	// 3.Get SessionKey
	//
	if( !m_pFacebook->GetSession() )
	{
		CConsoleMessage::GetInstance()->PrintTo(&m_lbMsgFB, _T("GetSession failure."));

		//m_pFacebook->CleanUp();
		return;
	}


	//
	// 4.Get AccessToken
	//
	if( !m_pFacebook->GetAccessToken() )
	{
		CConsoleMessage::GetInstance()->PrintTo(&m_lbMsgFB, _T("GetAccessToken failure."));

		//m_pFacebook->CleanUp();
		return;
	}


	CConsoleMessage::GetInstance()->PrintTo(&m_lbMsgFB, _T("Authorize Success!"));

	m_btWallPost.EnableWindow(TRUE);
	m_btPublishPhotos.EnableWindow(TRUE);    	

	
/*
	// Handle the return value from DoModal
	switch ( nRet )
	{
	case -1: 
		AfxMessageBox("Dialog box could not be created!");
		break;
	case IDABORT:
		// Do something
		break;
	case IDOK:
		// Do something
		break;
	case IDCANCEL:
		// Do something
		break;
	default:
		// Do something
		break;
	};
*/
	
}

void CPage01::OnBnClickedButtonWallpost()
{
	CString csMsg;

	m_ebWallMsg.GetWindowText(csMsg);

	if (csMsg.GetLength() == 0)
	{
		CConsoleMessage::GetInstance()->PrintTo(&m_lbMsgFB, _T("WallPost needs Message."));
	
		return;
	}

    std::string msg(csMsg.GetString());

    if ( !m_pFacebook->WallPost(msg) )
	{
		CConsoleMessage::GetInstance()->PrintTo(&m_lbMsgFB, _T("WallPost failure."));

		return;
	}

	CConsoleMessage::GetInstance()->PrintTo(&m_lbMsgFB, _T("WallPost Success."));
}

void CPage01::OnBnClickedButtonPublishPhotos()
{
	if ( m_csPathPhoto.IsEmpty() )
	{
		CConsoleMessage::GetInstance()->PrintTo(&m_lbMsgFB, _T("Select a photo file first."));

		return;
	}

	if ( !m_pFacebook->GetAlbumID() )
	{
		CConsoleMessage::GetInstance()->PrintTo(&m_lbMsgFB, _T("GetAlbumID: Ran Online album does not exist."));

		// Create the album here.
		if ( !m_pFacebook->CreateAlbum() )
		{
			CConsoleMessage::GetInstance()->PrintTo(&m_lbMsgFB, _T("CreateAlbum: Ran Online album could not be created."));
		}
		else
		{
			CConsoleMessage::GetInstance()->PrintTo(&m_lbMsgFB, _T("CreateAlbum: Ran Online album has been created."));			
		}
	}
	else
	{
		CConsoleMessage::GetInstance()->PrintTo(&m_lbMsgFB, _T("GetAlbumID: Ran Online album exits."));
	}

	CString csCaption;
	m_ebCaption.GetWindowText(csCaption);
    std::string caption(csCaption.GetString());

    std::string path(m_csPathPhoto.GetString());

	if ( !m_pFacebook->UploadPhoto(caption, path ) )
	{
		CConsoleMessage::GetInstance()->PrintTo(&m_lbMsgFB, _T("UploadPhoto: Success."));
	}
	else
	{
		CConsoleMessage::GetInstance()->PrintTo(&m_lbMsgFB, _T("UploadPhoto: Failure."));
	}
}

void CPage01::OnBnClickedButtonBrowse()
{
	TCHAR szFilters[]= _T("Image (*.jpg, *.png)|*.jpg;*.png|All Files (*.*)|*.*||");

	CFileDialog FileDialog( TRUE,
							_T("jpg"),
							_T("*.jpg"),
							OFN_HIDEREADONLY,
							szFilters
							);

	if(FileDialog.DoModal() == IDOK)
	{
		m_csPathPhoto = FileDialog.GetPathName();

		m_ebPath.SetWindowText(m_csPathPhoto);
	}
	
}
