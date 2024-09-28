
#pragma once

class CRandomOptionEditDlg;

class CRandomOptionConvertDlg : public CDialog
{
	DECLARE_DYNAMIC(CRandomOptionConvertDlg)
private:
	CRandomOptionEditDlg* m_pParentDlg;

public:
	CRandomOptionConvertDlg(CWnd* pParent = NULL);

	// ��ȭ ���� �������Դϴ�;
	enum { IDD = IDD_RANDOMOPTION_CONVERT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.

protected:
	virtual BOOL OnInitDialog(void) override;
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
	afx_msg void OnBnClickedBtnConvertNO(void);
	afx_msg void OnBnClickedBtnConvert(void);
	DECLARE_MESSAGE_MAP()
};