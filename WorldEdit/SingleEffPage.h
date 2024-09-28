#pragma once
#include "afxwin.h"


// CSingleEffPage 대화 상자입니다.
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

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_SINGLEEFF };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

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
