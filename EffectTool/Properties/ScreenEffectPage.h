#pragma once

#include "../resource.h"

// CScreenEffectPage ��ȭ �����Դϴ�.
class CsheetWithTab;
struct EFF_PROPERTY;
struct SCREEN_PROPERTY;

class CScreenEffectPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CScreenEffectPage)

protected:
	CsheetWithTab*		m_pSheetTab;
	SCREEN_PROPERTY*	m_pProperty;

public:
	void SetProperty ( EFF_PROPERTY* pProp );
	void SetSheetTab ( CsheetWithTab *pSheetTab )	{ m_pSheetTab = pSheetTab; }

public:
	CScreenEffectPage();
	virtual ~CScreenEffectPage();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_SCREEN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedButtonPlay();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonOk();
};
