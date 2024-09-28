#pragma once

#include "afxwin.h"

#include "../MfcExLib/ExLibs/Separator.h"

#include "../EngineLib/DxTools/DxFogMan.h"
#include "../EngineLib/DxTools/DxSkyMan.h"

// CFogPage ��ȭ �����Դϴ�.
class CsheetWithTab;

class CFogPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CFogPage)

protected:
	CsheetWithTab	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }

public:
	void UpdateContrl ();

protected:
	BOOL			m_bDlgInit;
	FOG_PROPERTY	m_sProperty;
	FOG_PROPERTY	m_sOldProperty;

	SKY_PROPERTY	m_sSkyProperty;
	SKY_PROPERTY	m_sOldSkyProperty;

protected:
	void	ReSetCheckBox ();
	void	ReSetOption ( BOOL bUse );
	void	ReSetOptionRange ( BOOL bUse );

public:
	CFogPage();
	virtual ~CFogPage();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_FOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonDiffuse();
	afx_msg void OnBnClickedButtonDiffuseDay();
	afx_msg void OnBnClickedButtonDiffuseNight();
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	afx_msg void OnBnClickedCheckSky();
	afx_msg void OnBnClickedCheckDayNight();
	afx_msg void OnBnClickedCheckSkyCloud();
	afx_msg void OnBnClickedCheckRangeSetting();
	afx_msg void OnBnClickedRadioX();
	afx_msg void OnBnClickedRadioY();
	afx_msg void OnBnClickedRadioZ();
	afx_msg void OnEnChangeEditAxisValue();
	afx_msg void OnBnClickedCheckSunMoon();
};
