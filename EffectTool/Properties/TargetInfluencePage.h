
#pragma once

#include "afxwin.h"
#include "../resource.h"

class CsheetWithTab;
struct TARGETINFLUENCE_PROPERTY;

class CTargetInfluencePage : public CPropertyPage
{
	DECLARE_DYNAMIC(CTargetInfluencePage)
protected:
	CsheetWithTab*					m_pSheetTab;
	TARGETINFLUENCE_PROPERTY*		m_pProperty;

public:
	void SetProperty ( EFF_PROPERTY* pProp );
	void SetSheetTab ( CsheetWithTab *pSheetTab )	{ m_pSheetTab = pSheetTab; }
	enum { IDD = IDD_DIALOG_TARGETINFLUENCE };
public:
	CTargetInfluencePage();
	virtual ~CTargetInfluencePage();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedCheckParentRotate();
};