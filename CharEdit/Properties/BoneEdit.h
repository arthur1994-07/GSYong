#pragma once
#include "afxcmn.h"


class CsheetWithTabBone;
// CBoneEdit ��ȭ �����Դϴ�.

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
	CBoneEdit();   // ǥ�� �������Դϴ�.
	virtual ~CBoneEdit();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_EDITBONE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
