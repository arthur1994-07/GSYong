#pragma once
#include "afxwin.h"
#include "afxcmn.h"

class CsheetWithTabChar;
class CsheetWithTabAnim;

// CAniBoneScalePage 대화 상자입니다.

class CAniBoneScalePage : public CPropertyPage
{
	DECLARE_DYNAMIC(CAniBoneScalePage)

protected:
	CsheetWithTabChar	*m_pSheetTabChar;
	CsheetWithTabAnim	*m_pSheetTabAni;

public:
	void SetSheetTab ( CsheetWithTabChar *pSheetTab ) { m_pSheetTabChar = pSheetTab; }

private:
	CString						m_sAnimName;

public:
	void SetInstance( const TCHAR* szName, CsheetWithTabAnim*pSheetTabAni ); 

private:
	void UpdateBoneNameLIST();
	void UpdateKeyScaleCTRL();

public:
	CAniBoneScalePage();
	virtual ~CAniBoneScalePage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_ANIBONESCALE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnSetActive();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonGetbonename();
	afx_msg void OnBnClickedButtonInsert();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedButtonKeyscaleModify();
	afx_msg void OnBnClickedButtonKeyscaleDelete2();
	CListBox m_listBONENAME;
	CListCtrl m_ctrlKEYSCALE;
	afx_msg void OnLbnSelchangeListBonename();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonKeyscaleInsert();
	virtual BOOL OnKillActive();
	afx_msg void OnNMClickListKeyscale(NMHDR *pNMHDR, LRESULT *pResult);
};
