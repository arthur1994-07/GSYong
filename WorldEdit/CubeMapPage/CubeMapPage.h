#pragma once
#include "afxcmn.h"

class CsheetWithTab;

// CCubeMapPage 대화 상자입니다.

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

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_CUBE_MAP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

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
