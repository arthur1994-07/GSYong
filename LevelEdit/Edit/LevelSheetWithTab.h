#ifndef __SHEETWITHTAB_H_
#define __SHEETWITHTAB_H_

/////////////////////////////////////////////////////////////////////////////
// CsheetWithTab
//#include "LevelEdit.h"
//#include "LevelMainFrm.h"
#include "../LevelEditDoc.h"
#include "../LevelEditView.h"

#include "./LevelMainPage.h"
#include "./MobSetPage.h"
#include "./MobSchedulePage.h"
#include "./CrowTree.h"
#include "./RequirePage.h"
#include "./ItemTreePage.h"
#include "./SkillTreePage.h"
#include "./GateListPage.h"
#include "./LevelLandGatePage.h"
#include "./LevelSingleEffect.h"
#include "./EtcFuncPage.h"
#include "./LandMarkPage.h"
#include "./LevelCollision.h"

enum EDITSHEET
{
	MAINPAGEINDEX			= 0,
	MOBSETPAGEINDEX			= 1,
	MOBSCHEDULEINDEX		= 2,
	LANDGATEINDEX			= 3,

	CROWTREEINDEX			= 4,
	REQUIREINDEX			= 5,

	ITEMTREEINDEX			= 6,
	SKILLTREEINDEX			= 7,
	GATELISTINDEX			= 8,

	SINGLEFFECTINDEX		= 9,
	SINGLEFFECTLAYERINDEX	= 10,

	LANDMARKINDEX			= 11,

	ETCFUNCINDEX			= 12,

	COLLISIONINDEX			= 13,
	COLLISIONLAYERINDEX		= 14,

	MATERIALSETPAGEINDEX	= 15,

	MAX_PAGEINDEX			= 16,
};

extern CString g_strEditSheet[30];

class CsheetWithTab : public CPropertySheet
{
	DECLARE_DYNAMIC(CsheetWithTab)

public:
	CWnd*				m_pWndParent;

public:
	CLevelMainPage		m_MainPage;
	CMobSetPage			m_MobSetPage;
	CMobSchedulePage	m_MobSchedulePage;
	CLevelLandGatePage	m_LandGatePage;
	
	CCrowTree			m_CrowTreePage;
	CRequirePage		m_RequirePage;
	CItemTreePage		m_ItemTreePage;
	CSkillTreePage		m_SkillTreePage;
	CGateListPage		m_GateListPage;
	CLevelSingleEffect	m_SingleEffect;
	CLevelSingleEffect	m_SingleEffectLayer;
	CLandMarkPage		m_LandMarkPage;
	CEtcFuncPage		m_EtcFuncPage;
	CLevelCollision		m_Collision;
	CLevelCollision		m_CollisionLayer;

public:
	void ActiveMobSetPage ()
	{
		SetActivePage ( MOBSETPAGEINDEX );
	}

	void ActiveMobSchedulePage(sc::CGLLIST<SMOBACTION>* pMobActList)
	{
		SetActivePage ( MOBSCHEDULEINDEX );
		m_MobSchedulePage.SetSchedule ( pMobActList );
	}

	void ActiveCrowTreePage ( int CalledPage, SNATIVEID NativeID )
	{
        SetActivePage ( CROWTREEINDEX );
		m_CrowTreePage.SetPropertyData ( CalledPage, NativeID );
	}

	void ActiveLandGatePage ( int CalledPage, DWORD dwGateID )
	{
        SetActivePage ( LANDGATEINDEX );
		m_GateListPage.SetPropertyData ( CalledPage, dwGateID );
	}

	void ActiveItemTreePage ( int CalledPage, SNATIVEID NativeID, int iBtnNum )
	{
        SetActivePage ( ITEMTREEINDEX );
		m_ItemTreePage.SetPropertyData ( CalledPage, NativeID, iBtnNum );
	}

	void ActiveSkillTreePage ( int CalledPage, SNATIVEID NativeID )
	{
        SetActivePage ( SKILLTREEINDEX );
		m_SkillTreePage.SetPropertyData ( CalledPage, NativeID );
	}

	void ActiveGateListPage ( int CalledPage, DWORD dwGateID )
	{
        SetActivePage ( GATELISTINDEX );
		m_GateListPage.SetPropertyData ( CalledPage, dwGateID );
	}

public:
	void UpdatePage ();
	void UpdateKeyBoardMouse();

// Construction
public:
	CsheetWithTab(CWnd* pParentWnd = NULL);
	CsheetWithTab(CRect& rect, CWnd* pParentWnd = NULL);
	virtual ~CsheetWithTab();

// Attributes
public:
   CRect      m_Rect;

   BOOL       m_bNoTabs;
   int        m_iDistanceFromTop;

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