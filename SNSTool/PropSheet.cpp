// PropSheet.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "RanSNS.h"
#include "PropSheet.h"
#include "curl/curl.h"


// CPropSht

IMPLEMENT_DYNAMIC(CPropSht, CPropertySheet)
CPropSht::CPropSht(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	//curl_global_init(CURL_GLOBAL_ALL);

	AddPage(&m_Page01);
	AddPage(&m_Page02);
}

CPropSht::CPropSht(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}

CPropSht::~CPropSht()
{
	
}


BEGIN_MESSAGE_MAP(CPropSht, CPropertySheet)
END_MESSAGE_MAP()

	//curl_global_cleanup();

// CPropSht 메시지 처리기입니다.
