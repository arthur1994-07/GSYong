
#pragma once

#include "../Util/ListBoxAni.h"

class CsheetWithTabAnim;

class CMultianimPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CMultianimPage)
	CsheetWithTabAnim*	m_pSheetTab;
	CListBoxAni			m_ListAnimation;

	INT m_nAnimIndex;

public:
	CMultianimPage(void);
	virtual ~CMultianimPage(void);

public:
	enum { IDD = IDD_DIALOG_MULTIANIM
		, LIST_NOTIFYMSG = WM_USER + 100};

public:
	void AddAnim(CString strAnimName);

	void SetSheetTab ( CsheetWithTabAnim *pSheetTab ) { m_pSheetTab = pSheetTab; }

	CString GetAnimName(INT nIndex);
	CString GetNextAnimName(void);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnInitDialog(void);
	afx_msg void OnBtnClickListAlignUp(void);
	afx_msg void OnBtnClickListAlignDown(void);
	afx_msg void OnBtnClickPlay(void);
	afx_msg void OnBtnClickDelete(void);
	afx_msg void OnBnClickedButtonCreateAniSequence(void);
	afx_msg LRESULT OnAnimationCont(WPARAM wParam, LPARAM lParam);
};