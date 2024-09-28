#pragma once
#include "afxcmn.h"


class CsheetWithTabBone;
// CBoneEdit 대화 상자입니다.

class CBoneEdit : public CPropertyPage
{
	DECLARE_DYNAMIC(CBoneEdit)

protected:
	CsheetWithTabBone	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTabBone *pSheetTab ) { m_pSheetTab = pSheetTab; }

public:
	void ResetPage ();

public:
	CBoneEdit();   // 표준 생성자입니다.
	virtual ~CBoneEdit();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_EDITBONE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonGetbone();
	afx_msg void OnBnClickedButtonInsert();
	CListCtrl m_list_EditBone;
	afx_msg void OnBnClickedButtonOk();

	void UpdateBoneScaleList();


	
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnNMDblclkListEditbone(NMHDR *pNMHDR, LRESULT *pResult);
};
