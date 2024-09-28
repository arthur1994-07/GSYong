#pragma once

#include "WeatherPage.h"

// CsheetWithTabWeather

class CsheetWithTabWeather : public CPropertySheet
{
	DECLARE_DYNAMIC(CsheetWithTabWeather)

public:
	CWnd*			m_pWndParent;

public:
	CWeatherPage	m_WeatherPage;

public:
	CRect			m_Rect;

	BOOL			m_bNoTabs;
	int				m_iDistanceFromTop;

public:
	CsheetWithTabWeather(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CsheetWithTabWeather(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CsheetWithTabWeather();

protected:
	DECLARE_MESSAGE_MAP()
};


