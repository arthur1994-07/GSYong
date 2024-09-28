#pragma once
#include "afxwin.h"
#include "../resource.h"
#include "../Util/ClrButton.h"

// CLandMarkPage ��ȭ �����Դϴ�.
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

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_LANDMARK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
