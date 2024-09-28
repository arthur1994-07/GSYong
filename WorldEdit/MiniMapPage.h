#pragma once


// CMiniMapPage ��ȭ �����Դϴ�.

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

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_MINIMAP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
