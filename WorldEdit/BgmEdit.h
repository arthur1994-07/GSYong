#pragma once


// CBgmEdit 대화 상자입니다.
class	CsheetWithTab;

class CBgmEdit : public CPropertyPage
{
	DECLARE_DYNAMIC(CBgmEdit)

public:
	CBgmEdit();
	virtual ~CBgmEdit();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_BGMEDIT };

protected:
	CsheetWithTab	*m_pSheetTab;
	BOOL			m_bDlgInit;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }
	void UpdateContrl ();	

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonFile();
	afx_msg void OnBnClickedButtonPlay();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnBnClickedButtonClear();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonVolume();
};
