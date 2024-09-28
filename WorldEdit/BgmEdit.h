#pragma once


// CBgmEdit ��ȭ �����Դϴ�.
class	CsheetWithTab;

class CBgmEdit : public CPropertyPage
{
	DECLARE_DYNAMIC(CBgmEdit)

public:
	CBgmEdit();
	virtual ~CBgmEdit();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_BGMEDIT };

protected:
	CsheetWithTab	*m_pSheetTab;
	BOOL			m_bDlgInit;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }
	void UpdateContrl ();	

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonFile();
	afx_msg void OnBnClickedButtonPlay();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnBnClickedButtonClear();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonVolume();
};
