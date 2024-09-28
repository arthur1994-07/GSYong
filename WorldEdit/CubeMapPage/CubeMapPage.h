#pragma once
#include "afxcmn.h"

class CsheetWithTab;

// CCubeMapPage ��ȭ �����Դϴ�.

class CCubeMapPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CCubeMapPage)

protected:
	CsheetWithTab	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }

public:
	CCubeMapPage();
	virtual ~CCubeMapPage();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_CUBE_MAP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonLookCubemap();
	afx_msg void OnBnClickedButtonCreateLocalCubeMap();
	virtual BOOL OnSetActive();
	afx_msg void OnBnClickedButtonViewSimulationPosition();
	afx_msg void OnBnClickedButtonViewOctreeBox();
	afx_msg void OnBnClickedButtonCreateGlobalCubeMap();
	virtual BOOL OnKillActive();
};
