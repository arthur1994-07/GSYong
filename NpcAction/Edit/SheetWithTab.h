#ifndef __SHEETWITHTAB_H_
#define __SHEETWITHTAB_H_

/////////////////////////////////////////////////////////////////////////////
// CsheetWithTab

#include "../ActionPage/NpcActionTable.h"
#include "../ActionPage/NpcActionPage.h"
#include "../ActionPage/NpcActionCondition.h"
#include "../ActionPage/NpcActionAnswer.h"
#include "../ActionPage/TalkPage.h"

enum EDITSHEET
{
	NPCACTIONTABLE		= 0,
	NPCACTIONPAGE		= 1,
	NPCACTIONCONDITION	= 2,
	NPCACTIONANSWER		= 3,

	NPCACTIONTALK		= 4,

	EDITSHEET			= 5
};

class	CNpcDialogue;
extern CString g_strEditSheet[EDITSHEET];

enum { CDF_CENTER, CDF_TOPLEFT, CDF_NONE };

class CsheetWithTab : public CPropertySheet
{
	DECLARE_DYNAMIC(CsheetWithTab)

public:
	CWnd*				m_pWndParent;

public:
	CNpcActionTable		m_NpcActionTable;
	CNpcActionPage		m_NpcActionPage;
	CNpcActionCondition	m_NpcActionCondition;
	CNpcActionAnswer	m_NpcActionAnswer;
	CTalkPage			m_NpcActionTalk;

public:
	void ActiveActionTable();
    void ActiveActionPage(std::tr1::shared_ptr<CNpcDialogue> spNpcDialogue);
	void ActiveActionCondition(int nCallPage, std::tr1::shared_ptr<SNpcTalkCondition> spConditon);
	void ActiveActionAnswer(std::tr1::shared_ptr<CNpcDialogueSet> spNpcDialogueSet, std::tr1::shared_ptr<CNpcDialogueCase> spDialogueCase, const DWORD dwItemIndex );
	void ActiveActionTalk(
        int nCallPage,
        std::tr1::shared_ptr<CNpcDialogueSet> pNpcDialogueSet,
        CNpcTalkControl* pTalkControl, 
        BOOL bModify,
        std::tr1::shared_ptr<SNpcTalk> spTalk);

public:
	spNpcDialogSet GetDialogueSet();

// Construction
public:
	CsheetWithTab(CWnd* pParentWnd = NULL);
	CsheetWithTab(CRect& rect, LOGFONT logfont, CWnd* pParentWnd = NULL );
	void ChangeDialogFont(CWnd* pWnd, CFont* pFont, int nFlag);
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
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

/////////////////////////////////////////////////////////////////////////////

#endif	// __SHEETWITHTAB_H_