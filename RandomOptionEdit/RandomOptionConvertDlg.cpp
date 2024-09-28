
#include "stdafx.h"
#include "../RanLogic/RandomOption/RandomOption.h"
#include "./RandomOptionEditDlg.h"
#include "./RandomOptionEditDefine.h"
#include "./RandomOptionEditFunction.h"
#include "./RandomOptionConvertDlg.h"

IMPLEMENT_DYNAMIC(CRandomOptionConvertDlg, CDialog)
CRandomOptionConvertDlg::CRandomOptionConvertDlg(CWnd* pParent)
: CDialog(CRandomOptionConvertDlg::IDD, pParent)
{
}

void CRandomOptionConvertDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CRandomOptionConvertDlg, CDialog)
	ON_BN_CLICKED(ID_BUTTON_CONVERT_NO, OnBnClickedBtnConvertNO)
	ON_BN_CLICKED(ID_BUTTON_CONVERT, OnBnClickedBtnConvert)
END_MESSAGE_MAP()

BOOL CRandomOptionConvertDlg::OnInitDialog()
{
	if ( 0 == (RandomOptionEdit::hWindowConvert = GetSafeHwnd()))//::GetDlgItem(this->GetParent()->GetSafeHwnd(), IDD_RANDOMOPTION_CONVERT)) )
		return FALSE;

	return CDialog::OnInitDialog();
}

BOOL CRandomOptionConvertDlg::PreTranslateMessage(MSG* pMsg)
{
	return CDialog::PreTranslateMessage(pMsg);
}

void CRandomOptionConvertDlg::OnBnClickedBtnConvertNO(void)
{
	if ( RandomOption::LoadFileOld(FALSE) == E_FAIL )
	{
		::MessageBox(0, TEXT("loadFileOld() : 불러오기에 실패 하였습니다;"), TEXT("실패"), MB_OK);
		return;
	}
	CRandomOptionEditDlg* const pParent = reinterpret_cast<CRandomOptionEditDlg*>(this->GetParent());
	pParent->UpdateRandomOptionList();
	this->OnOK();
}

void CRandomOptionConvertDlg::OnBnClickedBtnConvert(void)
{
	if ( RandomOption::LoadFileOld(RandomOption::LOAD_TOOL) == E_FAIL )
	{
		::MessageBox(0, TEXT("loadFileOld() : 불러오기에 실패 하였습니다;"), TEXT("실패"), MB_OK);
		return;
	}
	CRandomOptionEditDlg* const pParent = reinterpret_cast<CRandomOptionEditDlg*>(this->GetParent());
	pParent->UpdateRandomOptionList();
	this->OnOK();
}