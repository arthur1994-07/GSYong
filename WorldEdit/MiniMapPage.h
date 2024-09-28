#pragma once


// CMiniMapPage 대화 상자입니다.

class CMiniMapPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CMiniMapPage)

private:
	DWORD	m_dwTexSize;

protected:
	CsheetWithTab	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }

public:
	CMiniMapPage();
	virtual ~CMiniMapPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_MINIMAP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonMakeMinimap();
public:
	afx_msg void OnBnClickedButtonOk();
public:
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnCbnSelchangeComboTexsize();
public:
	CComboBox m_comboTexSize;
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
};
