#include "stdafx.h"

#include "../../SigmaCore/String/StringUtils.h"
#include "../../SigmaCore/File/StringFile.h"
#include "../../SigmaCore/File/SerialFile.h"

#include "../../EngineLib/G-Logic/GLogic.h"
#include "../../MfcExLib/ExLibs/EtcFunction.h"

#include "../GenItemTool.h"
#include "../SheetWithTab.h"
#include "./GenItemSearchPage.h"

// CGenItemSearchPage 대화 상자입니다.
IMPLEMENT_DYNAMIC(CGenItemSearchPage, CPropertyPage)
CGenItemSearchPage::CGenItemSearchPage(LOGFONT logfont)
: CPropertyPage(CGenItemSearchPage::IDD)
, m_pFont(new CFont())
{	
	m_pFont->CreateFontIndirect(&logfont);
}
CGenItemSearchPage::~CGenItemSearchPage(void)
{
	SAFE_DELETE (m_pFont);
}

void CGenItemSearchPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_GENITEM, m_listGenItem);
}

BEGIN_MESSAGE_MAP(CGenItemSearchPage, CPropertyPage)	
END_MESSAGE_MAP()

BOOL CGenItemSearchPage::OnInitDialog(void)
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_pSheetTab->ChangeDialogFont(this, m_pFont, CDF_TOPLEFT);
	m_listGenItem.SetExtendedStyle(m_listGenItem.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	char* szNameColumn[CGenItemSearchPage::ELEMENT_NSIZE] = 
	{
		"SpecID",
		"SkillMID", "SkillSID",
		"ItemMID", "ItemSID", "ItemName", "SpecRate",
		"CrowMID", "CrowSID", "CrowName",
		"GenItem", "Num",
		"GenRate", "SumRate"
	};

	const int widthRateColumn[CGenItemSearchPage::ELEMENT_NSIZE] = 
	{
		6,
		6, 6,
		6, 6, 13, 6,
		6, 6, 13,
		10, 4,
		6, 6
	};

	RECT rectCtrl;
	m_listGenItem.GetClientRect ( &rectCtrl );

	const long lnWidth(rectCtrl.right - rectCtrl.left);
	for ( unsigned int _index(0); _index < CGenItemSearchPage::ELEMENT_NSIZE; ++_index )
	{
		LV_COLUMN lvColumn;
		lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH ;
		lvColumn.fmt = LVCFMT_CENTER;
		lvColumn.pszText = szNameColumn[_index];
		lvColumn.iSubItem = _index;
		lvColumn.cx = ( lnWidth * widthRateColumn[_index] ) / 100;
		m_listGenItem.InsertColumn(_index, &lvColumn);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CGenItemSearchPage::Init(void)
{
}
void CGenItemSearchPage::UpdateItems(void)
{
}

bool CGenItemSearchPage::NewFile(void)
{
	return true;
}
bool CGenItemSearchPage::LoadFile(void)
{
	return true;
}
bool CGenItemSearchPage::SaveFile(void)
{
	return true;
}

bool CGenItemSearchPage::SaveCsv(void)
{
	return true;
}
bool CGenItemSearchPage::LoadCsv(void)
{
	return true;
}