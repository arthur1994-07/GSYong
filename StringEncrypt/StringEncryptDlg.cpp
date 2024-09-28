
// StringEncryptDlg.cpp : implementation file
//

#include "stdafx.h"

#include <string>

#include "../SigmaCore/Encrypt/DbEncrypt.h"
#include "../SigmaCore/String/StringUtil.h"

#include "StringEncrypt.h"
#include "StringEncryptDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CStringEncryptDlg dialog




CStringEncryptDlg::CStringEncryptDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CStringEncryptDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CStringEncryptDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CStringEncryptDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_DECRYPT, &CStringEncryptDlg::OnBnClickedButtonDecrypt)
	ON_BN_CLICKED(IDC_BUTTON_ENCRYPT, &CStringEncryptDlg::OnBnClickedButtonEncrypt)
END_MESSAGE_MAP()


// CStringEncryptDlg message handlers

BOOL CStringEncryptDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CStringEncryptDlg::OnPaint()
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
HCURSOR CStringEncryptDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CStringEncryptDlg::OnBnClickedButtonDecrypt()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData( TRUE );

	CString DecryptText;
	GetDlgItem( IDC_EDIT_DECRYPT )->GetWindowText( DecryptText );

	std::string strDecrypt;
	sc::DbEncrypt Decrypt;
	Decrypt.decrypt( std::string( DecryptText.GetString() ), strDecrypt );

	CWnd* pEdit = GetDlgItem( IDC_EDIT_ENCRYPT );
	pEdit->SetWindowText(strDecrypt.c_str());

	UpdateData(FALSE);

}

void CStringEncryptDlg::OnBnClickedButtonEncrypt()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData( TRUE );

	CString OriText;
	GetDlgItem( IDC_EDIT_ENCRYPT )->GetWindowText( OriText );

	std::string strOriText( OriText.GetString() );
	std::string strGenerate;
	sc::DbEncrypt Encrypt;
	Encrypt.encrypt( strOriText, strGenerate );

	CWnd* pEdit = GetDlgItem( IDC_EDIT_DECRYPT );
	pEdit->SetWindowText( strGenerate.c_str() );

	UpdateData(FALSE);
}
