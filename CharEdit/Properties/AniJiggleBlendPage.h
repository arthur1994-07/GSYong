#pragma once
#include "afxwin.h"
#include "afxcmn.h"

class CsheetWithTabChar;
class CsheetWithTabAnim;

// CAniJiggleBlendPage 대화 상자입니다.

class CAniJiggleBlendPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CAniJiggleBlendPage)

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
	void UpdateKeyBlendCTRL();

public:
	CAniJiggleBlendPage();
	virtual ~CAniJiggleBlendPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_JIGGLE_BLEND };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnSetActive();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonInsert();
	afx_msg void OnBnClickedButtonDelete();
	virtual BOOL OnInitDialog();
	virtual BOOL OnKillActive();
	CListCtrl	m_ctrlAttachBone;
	CListCtrl	m_ctrlKeyBlend;
	afx_msg void OnEnChangeEditBlend();
	afx_msg void OnBnClickedRadioBase();
	afx_msg void OnBnClickedRadioAttachBone();
	afx_msg void OnNMClickListAttachBone(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickListKeyBlend(NMHDR *pNMHDR, LRESULT *pResult);
};
