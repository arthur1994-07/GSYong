#pragma once
#include "afxwin.h"
#include "../resource.h"
#include "../Util/ClrButton.h"

// CLandMarkPage 대화 상자입니다.
class CsheetWithTab;
struct GLLANDMARK;

class CLandMarkPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CLandMarkPage)

protected:
	CsheetWithTab	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }

	BOOL			m_bDlgInit;
	GLLANDMARK*		m_pLandMark;
	CClrButton		m_Color;

public:
	void ListtingLandMark ();
	void UpdateContrl ();
	void ViewContrls ( BOOL bView, BOOL bClear=FALSE );

public:
	CLandMarkPage();
	virtual ~CLandMarkPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_LANDMARK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_listLandMark;
	afx_msg void OnBnClickedButtonEdit();
	afx_msg void OnBnClickedButtonDel();
	afx_msg void OnBnClickedButtonNew();
	//afx_msg void OnEnChangeEditName();
	afx_msg void OnBnClickedButtonAdd();
	virtual BOOL OnKillActive();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonColor();
	afx_msg void OnBnClickedButtonApply();
	virtual BOOL OnSetActive();
	afx_msg void OnBnClickedButtonNameConvert();
};
