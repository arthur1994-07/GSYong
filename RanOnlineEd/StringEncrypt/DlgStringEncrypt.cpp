// DlgStringEncrypt.cpp : ���� �����Դϴ�.
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

// DlgStringEncrypt ��ȭ �����Դϴ�.

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


// DlgStringEncrypt �޽��� ó�����Դϴ�.

void DlgStringEncrypt::OnBnClickedButtonStringEncrypt()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    UpdateData(TRUE);
    CString OriText;
    GetDlgItem(IDC_EDIT_STRING_ENCRYPT)->GetWindowText(OriText);
    sc::string::stringToClipboard(std::string(OriText.GetString()));    
}
