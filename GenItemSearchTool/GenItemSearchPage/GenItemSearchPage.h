#pragma once

#include "afxcmn.h"
#include "../resource.h"

class CsheetWithTab;

class CGenItemSearchPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CGenItemSearchPage)
	enum
	{
		ELEMENT_NSIZE = 15,
		IDD = IDD_GENITEMSEARCHPAGE,
	};

public:
	CGenItemSearchPage(LOGFONT logfont);
	virtual ~CGenItemSearchPage(void);

protected:
	CsheetWithTab* m_pSheetTab;
	CFont* m_pFont;

public:
	CListCtrl m_listGenItem;

public:
	void SetSheetTab(CsheetWithTab* pSheetTab) { m_pSheetTab = pSheetTab; }

	void Init();
	void UpdateItems ();

	bool NewFile();
	bool LoadFile();
	bool SaveFile();

	bool SaveCsv();
	bool LoadCsv();

	static INT CALLBACK CompareFunc(LPARAM LParam1, LPARAM lParam2, LPARAM lParamSort );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog(void) override;
};