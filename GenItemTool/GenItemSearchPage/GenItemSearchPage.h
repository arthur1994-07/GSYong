#pragma once

#include "afxcmn.h"
#include "../resource.h"
#include "../RanLogic/GenItemTable/GenItemTableDefine.h"
#include "afxwin.h"

class CsheetWithTab;

class CGenItemSearchPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CGenItemSearchPage)
	enum SearchPage
	{
		SPEC_ID = 0,

		ITEM_MAINID = 1,
		ITEM_SUBID = 2,
		ITEM_NAME = 3,

		RATE_SPEC = 4,

		CROW_MAINID = 5,
		CROW_SUBID = 6,
		CROW_NAME = 7,

		GENITEM_FILE_NAME = 8,
		
		GEN_NUM = 9,
		RATE_GEN = 10,
		RATE_TOTAL = 11,
		SKILL_NATIVEID = 12,

		ELEMENT_NSIZE = 13,
		IDD = IDD_GENITEMSEARCHPAGE,
	};

public:
	CGenItemSearchPage(LOGFONT logfont);
	virtual ~CGenItemSearchPage(void);

protected:
	CsheetWithTab* m_pSheetTab;
	CFont* m_pFont;
	
	std::vector<GenItem::GenItemData*> m_vecGenItemData;
	GenItem::SearchData m_SearChdataInfo;	

	bool m_CheckRange[5]; //오름차순 정렬과 내림차순 정렬 확인

private:
	CListCtrl m_listGenItem;
	CEdit m_EditItemName;
	CEdit m_EditGenItemFileName;
	CEdit m_EditCrowName;

public:
	void SetSheetTab(CsheetWithTab* pSheetTab) { m_pSheetTab = pSheetTab; }

	bool CheckSameWord(const std::string &_str);
	void UpdateItems ();

	bool GetSearchWord();	

	static INT CALLBACK CompareFunc(LPARAM LParam1, LPARAM lParam2, LPARAM lParamSort );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog(void) override;
	
	afx_msg void OnNMCustomDrawListGenItemData(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonSearch();		
	afx_msg void OnBnClickedButtonFindwordCrowname();
	afx_msg void OnBnClickedButtonFindwordItemname();
	afx_msg void OnBnClickedButtonFindwordGenitemname();		
	afx_msg void OnLvnColumnclickListGenitem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkGenItemList(NMHDR *pNMHDR, LRESULT *pResult);
};