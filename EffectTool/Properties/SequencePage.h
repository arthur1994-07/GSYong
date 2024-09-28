// CSequencePage ��ȭ �����Դϴ�.
#pragma once

#include "../resource.h"
#include "../MfcExLib/ExLibs/StColor.h"
#include "afxwin.h"

// CSequencePage ��ȭ �����Դϴ�.
class CsheetWithTab;
struct SEQUENCE_PROPERTY;

class CSequencePage : public CPropertyPage
{
	DECLARE_DYNAMIC(CSequencePage)

protected:
	CStColor	m_Color;

protected:
	CsheetWithTab*			m_pSheetTab;
	SEQUENCE_PROPERTY*		m_pProperty;

public:
	void SetProperty ( EFF_PROPERTY* pProp );
	void SetSheetTab ( CsheetWithTab *pSheetTab )	{ m_pSheetTab = pSheetTab; }

public:
	CSequencePage();
	virtual ~CSequencePage();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_SEQUENCE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonPlay();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonWind();
	afx_msg void OnBnClickedButtonRotate();
	afx_msg void OnBnClickedButtonAni();
	afx_msg void OnBnClickedButtonIdcButtonTexrotate();
	afx_msg void OnBnClickedButtonGravityMain();
	afx_msg void OnBnClickedButtonGravityMainE();
	afx_msg void OnBnClickedButtonScale();
	afx_msg void OnBnClickedButtonAlpha();
	afx_msg void OnBnClickedButtonColorS();
	afx_msg void OnBnClickedButtonColorV();
	afx_msg void OnBnClickedButtoncolorE();
	afx_msg void OnBnClickedButtonTexture();
	virtual BOOL OnInitDialog();
	CComboBox m_pComboBlend;
	afx_msg void OnBnClickedCheckGcollision();	
	afx_msg void OnBnClickedButtonEditsound();
	afx_msg void OnBnClickedCheckGotocenter();
	afx_msg void OnBnClickedRadioBillboard();
	afx_msg void OnBnClickedRadioBilldirect();
	afx_msg void OnBnClickedRadioBillup();
	afx_msg void OnBnClickedCheckLighting();
	afx_msg void OnBnClickedCheckDynamicScale();
	afx_msg void OnBnClickedDefaultPos();
	afx_msg void OnBnClickedCheckNoWolrdRs();
	afx_msg void OnBnClickedCheckRandomLife();
};


