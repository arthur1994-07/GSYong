#pragma once
#include "afxwin.h"
#include "resource.h"

// CWaterSurfPage ��ȭ �����Դϴ�.

class CWaterSurfPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CWaterSurfPage)

protected:
	CsheetWithTab*	m_pSheetTabParent;
	BOOL			m_bInit;

protected:
	//WATERSURF_PROPERTY	m_stProperty;

private:
	void UpdateList();

public:
	CWaterSurfPage();
	virtual ~CWaterSurfPage();

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTabParent = pSheetTab; }

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_WATERSURF };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonCreate();
	virtual BOOL OnSetActive();
	CListBox m_listSurf;
	afx_msg void OnLbnSelchangeListSurf();
	afx_msg void OnBnClickedButtonModify();
	afx_msg void OnBnClickedButtonInverseUv();
	afx_msg void OnBnClickedButtonMeshModify();
	afx_msg void OnBnClickedButtonAddHeight();
	afx_msg void OnBnClickedButtonDelete();
	virtual BOOL OnKillActive();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonModify2();
};
