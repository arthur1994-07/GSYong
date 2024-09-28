#pragma once
#include "afxwin.h"


// CAniDivPage ��ȭ �����Դϴ�.
class CsheetWithTabAnim;

class CAniDivPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CAniDivPage)

protected:
	CsheetWithTabAnim	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTabAnim *pSheetTab ) { m_pSheetTab = pSheetTab; }

protected:
	CString m_sAnimName;

public:
	void SetAnimName ( CString sName );
	void ListtingAniDiv ();
	void FrameMarking ( BOOL bReset=FALSE );

public:
	CAniDivPage();
	virtual ~CAniDivPage();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_ANIDIV };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonDel();
	afx_msg void OnBnClickedOk();
	CListBox m_AniDivList;
};
