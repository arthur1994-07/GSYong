#ifndef __SHEETWITHTAB_H_
#define __SHEETWITHTAB_H_

/////////////////////////////////////////////////////////////////////////////
// CsheetWithTab

#include "./SkillTree.h"
#include "./SkillData.h"
#include "./SkillLevelData.h"

enum
{
	eNOT_FOUND = 0xFFFFFFFF,		//	잘못된 선택
	eMASK_0x0000FFFF = 0x0000FFFF,	//	마스킹
    eMASK_0xFFFF0000 = 0xFFFF0000,
    eSHIFT_16BIT = 16				//	비트 쉬프트
};

enum EDITSHEET
{
	SKILLTREEPAGE = 0,
	SKILLDATAPAGE = 1,
	SKILLLEVELDATAPAGE = 2,

	N_SHEETCOUNT = 3	
};

enum { CDF_CENTER, CDF_TOPLEFT, CDF_NONE };

extern CString g_strEditSheet[30];

class CsheetWithTab : public CPropertySheet
{
	DECLARE_DYNAMIC(CsheetWithTab)

public:
	CWnd*				m_pWndParent;

public:
	CSkillTree		m_SkillTreePage;
	CSkillData		m_SkillDataPage;
	CSkillLevelData	m_SkillLevelDataPage;

public:
	void ActiveSkillTreePage ();
	void ActiveSkillDataPage ( int CurPage, DWORD GUID );
	void ActiveSkillLevelDataPage ( int CurPage, DWORD GUID );
	void UpdateTreePage();
	void ChangeDialogFont(CWnd* pWnd, CFont* pFont, int nFlag);

	void ExportExcelDetail( boost::function<void (int)> PercentCallbackFunc=NULL );

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
};

/////////////////////////////////////////////////////////////////////////////

#endif	// __SHEETWITHTAB_H_