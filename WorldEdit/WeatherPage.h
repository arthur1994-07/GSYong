#pragma once

#include "afxwin.h"

#include "../MfcExLib/ExLibs/Separator.h"

//#include "CJFlatComboBox.h"
//#include "FlatEdit.h"

#include "../EngineLib/DxTools/DxWeatherMan.h"

// CWeatherPage 대화 상자입니다.
class CsheetWithTab;

class CWeatherPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CWeatherPage)

protected:
	CsheetWithTab	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }

public:
	void UpdateContrl ();

protected:
	BOOL				m_bDlgInit;

	WEATHER_PROPERTY	m_sProperty;
	WEATHER_PROPERTY	m_sOldProperty;

	RAIN_PROPERTY		m_sRainProp;
	RAIN_PROPERTY		m_sOldRainProp;

	SNOW_PROPERTY		m_sSnowProp;
	SNOW_PROPERTY		m_sOldSnowProp;

	CLOUD_PROPERTY		m_sCloudProp;
	CLOUD_PROPERTY		m_sOldCloudProp;

protected:
	void	ReSetCheckBox ();

public:
	CWeatherPage();
	virtual ~CWeatherPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_WEATHER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonWinddir();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonCancel();
	virtual BOOL OnSetActive();
	afx_msg void OnBnClickedButtonDiffF();
	afx_msg void OnBnClickedCheckRain();
	afx_msg void OnBnClickedCheckSnow();
	afx_msg void OnBnClickedCheckLeaves();
	afx_msg void OnBnClickedCheckRainDoing();
	afx_msg void OnBnClickedCheckSnowDoing();
	afx_msg void OnBnClickedCheckLeavesDoing();
	afx_msg void OnBnClickedCheckActive();
};
