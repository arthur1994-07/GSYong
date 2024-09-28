// DlgStringEncrypt.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include <string>
#include "../SigmaCore/Encrypt/DbEncrypt.h"
#include "../SigmaCore/String/StringUtil.h"
#include "../RanOnlineEd.h"
#include "./DlgStringEncrypt.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// DlgStringEncrypt 대화 상자입니다.

IMPLEMENT_DYNAMIC(DlgStringEncrypt, CDialog)

DlgStringEncrypt::DlgStringEncrypt(CWnd* pParent /*=NULL*/)
	: CDialog(DlgStringEncrypt::IDD, pParent)
{

}

DlgStringEncrypt::~DlgStringEncrypt()
{
}

void DlgStringEncrypt::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DlgStringEncrypt, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_STRING_ENCRYPT, &DlgStringEncrypt::OnBnClickedButtonStringEncrypt)
    ON_BN_CLICKED(IDC_BUTTON_STRING_DECRYPT, &DlgStringEncrypt::OnBnClickedButtonStringDecrypt)
    ON_BN_CLICKED(IDC_BUTTON_STRING_COPY_TO_CLIPBOARD, &DlgStringEncrypt::OnBnClickedButtonStringCopyToClipboard)
END_MESSAGE_MAP()


// DlgStringEncrypt 메시지 처리기입니다.

void DlgStringEncrypt::OnBnClickedButtonStringEncrypt()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    UpdateData(TRUE);

    CString OriText;
    GetDlgItem(IDC_EDIT_STRING_DECRYPT)->GetWindowText(OriText);
    
    std::string strOriText(OriText.GetString());
    std::string strGenerate;
    sc::DbEncrypt Encrypt;
    Encrypt.encrypt(strOriText, strGenerate);

    CWnd* pEdit = GetDlgItem(IDC_EDIT_STRING_ENCRYPT);
    pEdit->SetWindowText(strGenerate.c_str());

    UpdateData(FALSE);
}

void DlgStringEncrypt::OnBnClickedButtonStringDecrypt()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    UpdateData(TRUE);

    CString DecryptText;
    GetDlgItem(IDC_EDIT_STRING_ENCRYPT)->GetWindowText(DecryptText);
    
    std::string strDecrypt;
    sc::DbEncrypt Decrypt;
    Decrypt.decrypt(std::string(DecryptText.GetString()), strDecrypt);

    CWnd* pEdit = GetDlgItem(IDC_EDIT_STRING_DECRYPT);
    pEdit->SetWindowText(strDecrypt.c_str());
    
    UpdateData(FALSE);
}

void DlgStringEncrypt::OnBnClickedButtonStringCopyToClipboard()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    UpdateData(TRUE);
    CString OriText;
    GetDlgItem(IDC_EDIT_STRING_ENCRYPT)->GetWindowText(OriText);
    sc::string::stringToClipboard(std::string(OriText.GetString()));    
}
