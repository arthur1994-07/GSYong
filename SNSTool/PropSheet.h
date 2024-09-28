#pragma once

#include "Page01.h"
#include "Page02.h"



// CPropSht

class CPropSht : public CPropertySheet
{
	DECLARE_DYNAMIC(CPropSht)

public:
	CPropSht(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CPropSht(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CPropSht();

    
	CPage01 m_Page01;
	CPage02 m_Page02;

protected:
	DECLARE_MESSAGE_MAP()
};


