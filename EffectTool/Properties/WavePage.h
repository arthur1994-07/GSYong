#pragma once

#include "../resource.h"

// CWavePage ��ȭ �����Դϴ�.
class CsheetWithTab;
struct WAVE_PROPERTY;

class CWavePage : public CPropertyPage
{
	DECLARE_DYNAMIC(CWavePage)

protected:
	CsheetWithTab*			m_pSheetTab;
	WAVE_PROPERTY*			m_pProperty;

public:
	void SetProperty ( EFF_PROPERTY* pProp );
	void SetSheetTab ( CsheetWithTab *pSheetTab )	{ m_pSheetTab = pSheetTab; }

public:
	CWavePage();
	virtual ~CWavePage();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_WAVE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonPlay();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonScale();
	afx_msg void OnBnClickedButtonAlpha();
	afx_msg void OnBnClickedButtonWave();
	afx_msg void OnBnClickedRadioBillboard();
	afx_msg void OnBnClickedRadioGround();
	afx_msg void OnBnClickedRadioRandom();
	afx_msg void OnBnClickedRadioWave();
	afx_msg void OnBnClickedRadioEmission();
	afx_msg void OnBnClickedRadioCollect();
};
