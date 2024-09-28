#pragma once

#include "afxwin.h"
#include "afxcmn.h"
#include "../resource.h"
#include "ParticleTabControl.h"

// CParticleSysPage 대화 상자입니다.
class CsheetWithTab;
struct PARTICLESYS_PROPERTY;

class CParticleSysPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CParticleSysPage)

protected:
	CsheetWithTab*			m_pSheetTab;
	PARTICLESYS_PROPERTY*	m_pProperty;

public:
	void SetProperty ( EFF_PROPERTY* pProp );
	void SetSheetTab ( CsheetWithTab *pSheetTab )	{ m_pSheetTab = pSheetTab; }

public:
	CParticleSysPage();
	virtual ~CParticleSysPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_PARTICLE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonPlay();
	afx_msg void OnBnClickedButtonApply();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonEditsound();
	CParticleTabControl m_tabTYPE;
	virtual BOOL OnSetActive();
};
