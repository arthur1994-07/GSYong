#ifndef __SHEETWITHTAB_H_
#define __SHEETWITHTAB_H_

/////////////////////////////////////////////////////////////////////////////
// CsheetWithTab

#include "./CrowTree.h"
#include "./CrowAttack.h"
#include "./CrowData.h"
#include "./CrowAttackTree.h"

enum EDITSHEET
{
	CROWTREEPAGE = 0,
	CROWATTACKPAGE = 1,
	CROWDATAPAGE = 2,
	CROWATTACKTREEPAGE = 3,

	N_SHEETCOUNT = 4
};

extern CString g_strEditSheet[30];

enum { CDF_CENTER, CDF_TOPLEFT, CDF_NONE };

class CsheetWithTab : public CPropertySheet
{
	DECLARE_DYNAMIC(CsheetWithTab)

public:
	CWnd*				m_pWndParent;

public:
	CCrowTree		m_CrowTreePage;
	CCrowData		m_CrowDataPage;
	CCrowAttack		m_CrowAttackPage;
	CCrowAttackTree m_CrowAttackTreePage;

public:
	void ActiveCrowTreePage ();
	void ActiveCrowDataPage ( int CurPage, SCROWDATA* pCrowData );
	void ActiveCrowAttackTreePage ( int CurPage, SCROWDATA* pCrowData );
	void ActiveCrowAttackPage ( int CurPage, SCROWDATA* pCrowData, DWORD dwID );
	void UpdateSuitTreePage();

// Construction
public:
	CsheetWithTab(CWnd* pParentWnd = NULL);
	CsheetWithTab(CRect& rect, LOGFONT logfont, CWnd* pParentWnd = NULL);
	virtual ~CsheetWithTab();

// Attributes
public:
   CRect      m_Rect;

   BOOL       m_bNoTabs;
   int        m_iDistanceFromTop;
   CFont*	  m_pFont;

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