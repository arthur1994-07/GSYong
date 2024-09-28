// ZplayOggExamDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BgmSound.h"
#include "ZplayOggExam.h"
#include "ZplayOggExamDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CZplayOggExamDlg dialog




CZplayOggExamDlg::CZplayOggExamDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CZplayOggExamDlg::IDD, pParent)    
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_pBgmSound = NULL;
}

void CZplayOggExamDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_STATIC_FILENAME, m_StaticSoundFile);
    DDX_Control(pDX, IDC_CHECK_LOOP, m_Loop);
    DDX_Control(pDX, IDC_SLIDER_VOLUME, m_Volume);
    DDX_Control(pDX, IDC_PROGRESS_SOUND, m_SoundProgress);
}

BEGIN_MESSAGE_MAP(CZplayOggExamDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_BUTTON_LOAD, &CZplayOggExamDlg::OnBnClickedButtonLoad)
    ON_WM_CLOSE()
    ON_BN_CLICKED(IDC_BUTTON_PLAY, &CZplayOggExamDlg::OnBnClickedButtonPlay)
    ON_BN_CLICKED(IDC_BUTTON_STOP, &CZplayOggExamDlg::OnBnClickedButtonStop)
    ON_BN_CLICKED(IDC_BUTTON_PAUSE, &CZplayOggExamDlg::OnBnClickedButtonPause)
    ON_BN_CLICKED(IDC_BUTTON_RESUME, &CZplayOggExamDlg::OnBnClickedButtonResume)
    ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CZplayOggExamDlg message handlers

BOOL CZplayOggExamDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
    m_Volume.SetRange(0, 100);
    m_Volume.SetRangeMin(0);
    m_Volume.SetRangeMax(100);
    m_Volume.SetPos(50);

    m_pBgmSound = new BgmSound;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CZplayOggExamDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CZplayOggExamDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CZplayOggExamDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CZplayOggExamDlg::OnBnClickedButtonLoad()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    CString szFilter = "OGG File (*.ogg)|*.ogg|";

    //	Note : 파일 오픈 다이알로그를 만듬.
    CFileDialog dlg(
        TRUE,
        ".ogg",
        NULL,
        OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
        szFilter,
        (CZplayOggExamDlg*) this);    
    if (dlg.DoModal() == IDOK)
    {        
        CString OggFileName = dlg.GetPathName().GetString();
        m_StaticSoundFile.SetWindowText(OggFileName);
        //m_pBgmSound->Stop();
        m_pBgmSound->OpenFile(std::string(OggFileName.GetString()));
        int Volume = m_Volume.GetPos();
        m_pBgmSound->SetVolume(Volume);
        m_pBgmSound->Play();
    }
}

void CZplayOggExamDlg::OnClose()
{
    // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
    if (m_pBgmSound)
    {
        delete m_pBgmSound;
        m_pBgmSound = NULL;
    }
    CDialog::OnClose();
}

void CZplayOggExamDlg::OnBnClickedButtonPlay()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    m_pBgmSound->Play();
}

void CZplayOggExamDlg::OnBnClickedButtonStop()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    m_pBgmSound->Stop();
}

void CZplayOggExamDlg::OnBnClickedButtonPause()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    m_pBgmSound->Pause();
}

void CZplayOggExamDlg::OnBnClickedButtonResume()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    m_pBgmSound->Resume();
}

void CZplayOggExamDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
    if (pScrollBar)
    {
        // 어떤 슬라이더인지 검사
        if (pScrollBar == (CScrollBar*) &m_Volume)
        {
            // 슬라이더의 위치를 검사한다.
            int Volume = m_Volume.GetPos();
            m_pBgmSound->SetVolume(Volume);
        }
    }
    CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
