#pragma once
#include "afxwin.h"
#include "../resource.h"
#include "../../MfcExLib/ExLibs/StColor.h"

// CGroundPage ��ȭ �����Դϴ�.
class CsheetWithTab;
struct GROUND_PROPERTY;

class CGroundPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CGroundPage)

protected:
	CStColor	m_Color;

protected:
	CsheetWithTab*			m_pSheetTab;
	GROUND_PROPERTY*		m_pProperty;

public:
	void SetProperty ( EFF_PROPERTY* pProp );
	void SetSheetTab ( CsheetWithTab *pSheetTab )	{ m_pSheetTab = pSheetTab; }

public:
	CGroundPage();
	virtual ~CGroundPage();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_GROUND };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonPlay();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnCbnSelchangeComboBlend();
	CComboBox m_pComboBlend;
	afx_msg void OnBnClickedButtonScale();
	afx_msg void OnBnClickedButtonAlpha();
	afx_msg void OnBnClickedButtonColorS();
	afx_msg void OnBnClickedButtonColorV();
	afx_msg void OnBnClickedButtoncolorE();
	afx_msg void OnBnClickedButtonTexture();
	afx_msg void OnBnClickedButtonRotate();
	afx_msg void OnBnClickedButtonAni();
	afx_msg void OnBnClickedButtonHeight();
	afx_msg void OnBnClickedButtonEditsound();
	afx_msg void OnBnClickedCheckNormal();
	afx_msg void OnBnClickedCheckPicking();
};
