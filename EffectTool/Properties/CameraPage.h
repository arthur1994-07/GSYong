#pragma once
#include "afxwin.h"
#include "../resource.h"
// CCameraEffPage ��ȭ �����Դϴ�.
class CsheetWithTab;
struct CAMERA_PROPERTY;

class CCameraEffPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CCameraEffPage)

protected:
	CStColor	m_Color;

protected:
	CsheetWithTab*			m_pSheetTab;
	CAMERA_PROPERTY*		m_pProperty;

public:
	void SetProperty ( EFF_PROPERTY* pProp );
	void SetSheetTab ( CsheetWithTab *pSheetTab )	{ m_pSheetTab = pSheetTab; }

public:
	CCameraEffPage();
	virtual ~CCameraEffPage();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_CAMERA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_pComboCameraSet;
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonPlay();
	afx_msg void OnCbnSelchangeComboCameraSet();
	//afx_msg void OnEnChangeEditPower();
	afx_msg void OnEnChangeEditPlayNum();
};
