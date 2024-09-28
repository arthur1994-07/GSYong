#ifndef __SHEETWITHTAB_H_
#define __SHEETWITHTAB_H_

/////////////////////////////////////////////////////////////////////////////
// CsheetWithTab

#include <vector>
#include "./ItemMixPage/ItemMixPage.h"
#include "./TaxiStationPage/TaxiStationPage.h"
#include "./MapListPage/MapsListPage.h"
#include "./AutoLevelPage/AutoLevelPage.h"

enum EDITSHEET
{
	ITEM_MIX_TOOL	= 0,
	MAPS_LIST_TOOL	= 1,
	TAXI_STATION	= 2,
	AUTO_LEVEL_TOOL	= 3,
	PAGE_NUM		= 4,		
};

enum { CDF_CENTER, CDF_TOPLEFT, CDF_NONE };

extern std::string g_strEditSheet[PAGE_NUM];

class CsheetWithTab : public CPropertySheet
{
	DECLARE_DYNAMIC(CsheetWithTab)

public:
	CWnd*				m_pWndParent;

public:
	CItemMixPage		m_ItemMixPage;
	CMapsListPage		m_MapsListPage;
	CTaxiStationPage	m_TaxiStationPage;
	CAutoLevelPage		m_AutoLevelPage;

// Construction
public:
	CsheetWithTab(CWnd* pParentWnd = NULL);
	CsheetWithTab(CRect& rect, LOGFONT logfont, CWnd* pParentWnd = NULL );
	virtual ~CsheetWithTab();

// Attributes
public:
	CRect		m_Rect;

	BOOL		m_bNoTabs;
	int			m_iDistanceFromTop;
	CFont*		m_pFont;

// Get/Set
public:
   void NoTabs() { m_bNoTabs = TRUE; }
   void SetDistanceFromTop( int iDistance ) { m_iDistanceFromTop = iDistance; }
   void ChangeDialogFont(CWnd* pWnd, CFont* pFont, int nFlag);

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CsheetWithTab)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
public:
	bool NewFile( int nPage );
	bool LoadFile( int nPage );
	bool LoadFileOld( int nPage );
	bool SaveFile( int nPage );

	bool CsvLoad( int nPage );
	bool CsvSave( int nPage );

	void SetPage( int nPage );
	bool IsPageEdit( int nPage );

// Generated message map functions
protected:
	//{{AFX_MSG(CsheetWithTab)
   afx_msg HBRUSH CtlColor(CDC* pDc, UINT uCtlColor);	
   //}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

/////////////////////////////////////////////////////////////////////////////

#endif	// __SHEETWITHTAB_H_
