#pragma once
#include "afxwin.h"


// CSingleEffPage ��ȭ �����Դϴ�.
class CsheetWithTab;
struct DXLANDEFF;

class CSingleEffPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CSingleEffPage)

protected:
	CsheetWithTab	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }

public:
	std::string		m_strPrevEffName;
	BOOL			m_bDlgInit;

public:
	void ListtingLandEff ();
	void SetDialog( DXLANDEFF* pLandEff );

	void UpdatePageClickUP();
	void UpdatePageClickDOWN();

	void UpdateContrl ();
	void SelectSingleEffectList( DXLANDEFF* pEff );

public:
	CSingleEffPage();
	virtual ~CSingleEffPage();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_SINGLEEFF };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListBox m_EffListBox;
	afx_msg void OnBnClickedButtonNew();
	afx_msg void OnBnClickedButtonSearch();
	afx_msg void OnBnClickedButtonDel();
	virtual BOOL OnKillActive();
	afx_msg void OnLbnSelchangeListEffect();
	afx_msg void OnBnClickedCheckDspName();
    afx_msg void OnBnClickedButtonModify();
    virtual BOOL OnSetActive();
};
