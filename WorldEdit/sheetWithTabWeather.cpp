// sheetWithTabWeather.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "WorldEdit.h"
#include "sheetWithTabWeather.h"


// CsheetWithTabWeather

IMPLEMENT_DYNAMIC(CsheetWithTabWeather, CPropertySheet)
CsheetWithTabWeather::CsheetWithTabWeather(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CsheetWithTabWeather::CsheetWithTabWeather(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}

CsheetWithTabWeather::~CsheetWithTabWeather()
{
}


BEGIN_MESSAGE_MAP(CsheetWithTabWeather, CPropertySheet)
END_MESSAGE_MAP()


// CsheetWithTabWeather 메시지 처리기입니다.
