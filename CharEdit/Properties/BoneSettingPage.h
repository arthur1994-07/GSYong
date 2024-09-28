#pragma once

#include "../Resource.h"
#include "afxwin.h"

class CsheetWithTabChar;

// CBoneSettingPage ��ȭ �����Դϴ�.

class CBoneSettingPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CBoneSettingPage)

protected:
	CsheetWithTabChar	*m_pSheetTabChar;

public:
	void SetSheetTab ( CsheetWithTabChar *pSheetTab )	{ m_pSheetTabChar = pSheetTab; }

public:
	CBoneSettingPage();
	virtual ~CBoneSettingPage();

	// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_BONE_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedDontSaveOK();
	afx_msg void OnBnClickedButtonFootColl();
	afx_msg void OnBnClickedButtonJiggleBone();
	afx_msg void OnBnClickedButtonCollisionObject();
	afx_msg void OnBnClickedButtonClothCollision();
	afx_msg void OnBnClickedButtonDocument();
};
