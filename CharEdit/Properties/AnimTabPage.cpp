
#include "stdafx.h"

#include "../CharEdit.h"
#include "./AnimTabControl.h"
#include "./SheetWithTabAnim.h"
#include "./AnimPage.h"

#include "./AnimTabPage.h"

IMPLEMENT_DYNAMIC(CAnimTabPage, CPropertyPage)
CAnimTabPage::CAnimTabPage(void)
: CPropertyPage(CAnimTabPage::IDD)
, m_pSheetTab(NULL)
{
}

CAnimTabPage::~CAnimTabPage(void)
{
}

void CAnimTabPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_ANIM, m_tabAnim);
}

void CAnimTabPage::SetSheetTab ( CsheetWithTabAnim *pSheetTab ) 
{
	m_pSheetTab = pSheetTab; 
	m_tabAnim.SetSheetTab(pSheetTab);
}

BOOL CAnimTabPage::OnInitDialog(void)
{
	CPropertyPage::OnInitDialog();
	m_tabAnim.Init();
	m_tabAnim.InsertItem(0, _T("SingleAnim") );
	m_tabAnim.InsertItem(1, _T("MultiAnim") );
	m_tabAnim.SetCurFocus(0);
	return TRUE;
}

BOOL CAnimTabPage::OnSetActive(void)
{
	return CPropertyPage::OnSetActive();
}

CAnimPage* CAnimTabPage::GetAnimPage(void)
{
	return m_tabAnim.GetAnimPage();
}

CMultianimPage*	CAnimTabPage::GetMultiAnimPage(void)
{
	return m_tabAnim.GetMultiAnimPage();
}