#pragma	once

#include "../../../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../../../EngineLib/G-Logic/GLDefine.h"
#include <map>

class	CBasicScrollBarEx;
class	CBasicTextBoxEx;
class	CBasicLineBox;
class	CQuestListParent;
class	CQuestListChild;
class	CTipBox;
class GLGaeaClient;
class CInnerInterface;

struct SQUEST_LIST
{
	DWORD	m_dwID;
	bool	m_bCheck;

	SQUEST_LIST() :
	m_dwID( NATIVEID_NULL().dwID )
		, m_bCheck(false)
	{
	}
	SQUEST_LIST( DWORD dwID, bool bCheck = false )
	{
		m_dwID = dwID;
		m_bCheck = bCheck;
	}
};

typedef std::map<DWORD,SQUEST_LIST>		MAP_QUEST_LIST; // <맵번호, 자식UI 구조>. by luxes.
typedef MAP_QUEST_LIST::iterator		MAP_QUEST_ITER;

struct SAREA_LIST
{
	DWORD	m_dwID;
	bool	m_bOpen;
	MAP_QUEST_LIST m_mapList;

	SAREA_LIST() :
		m_dwID( NATIVEID_NULL().dwID )
		, m_bOpen( false )
		{
		}

	SAREA_LIST( DWORD dwID, bool bOpen = false )
	{
		m_dwID = dwID;
		m_bOpen = bOpen;
	}

	MAP_QUEST_LIST& GetQuestList()	{ return m_mapList; }
};

typedef std::multimap<DWORD,SAREA_LIST>		MULTIMAP_AREA_LIST; // <지역번호, 부모UI 구조>. by luxes.
typedef MULTIMAP_AREA_LIST::iterator		MULTIMAP_AREA_ITER;
typedef MULTIMAP_AREA_LIST::const_iterator	MULTIMAP_AREA_CITER;

const int g_nLimitQuestCount = 20;

class CQuestListRenewal : public CUIGroup
{
private:
	static	const	int		nLimitQuest;

public:
	//typedef std::multimap<DWORD,bool>			MULTIMAP_QUEST_AREA; // <지역번호, 부모UI 열림 여부>. by luxes.
	//typedef MULTIMAP_QUEST_AREA::iterator		MULTIMAPQQUEST_AREA_ITER;
	//typedef MULTIMAP_QUEST_AREA::const_iterator	MULTIMAPQUEST_AREA_CITER;

private:
	enum
	{
		QUEST_LIST_TEXT_BOX = NO_ID + 1,
		QUEST_LIST_SCROLL_BAR_PROG = QUEST_LIST_TEXT_BOX + 1,
		QUEST_LIST_SCROLL_BAR_END = QUEST_LIST_TEXT_BOX + 2,
		QUEST_LIST_SELECT = QUEST_LIST_TEXT_BOX + 3,
		QUEST_LIST_PARENT = QUEST_LIST_TEXT_BOX + 4,
		QUEST_LIST_CHILD = QUEST_LIST_PARENT + g_nLimitQuestCount,
	};

public:
	CQuestListRenewal(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CQuestListRenewal ();

protected:
    GLGaeaClient* m_pGaeaClient;
    CInnerInterface* m_pInterface;

public:
	void	CreateSubControl ();

public:
	virtual	void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void	TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	bool LoadQuestList();

	MULTIMAP_AREA_LIST& GetAreaList()		{ return m_multimapArea; }
	MULTIMAP_AREA_LIST& GetEndAreaList()	{ return m_multimapEndArea; }

	void	SetQuestID( DWORD dwID );

	DWORD	GetQuestIngID()	{ return m_dwQuestIngID; }
	DWORD	GetQuestEndID()	{ return m_dwQuestEndID; }

	void	AutoLoadQuestList( DWORD dwQuestID );

private:
	void	RecalSize();
	void	CheckDeleteList();
	void	CheckDeleteIngList();
	void	CheckDeleteEndList();

private:
	CBasicLineBox*		m_pLineBox;
	CBasicScrollBarEx*	m_pListScrollBarProg;
	CBasicScrollBarEx*	m_pListScrollBarEnd;
	CQuestListParent*	m_pParentList[ g_nLimitQuestCount ];
	CQuestListChild*	m_pChildList[ g_nLimitQuestCount ];
	CUIControl*			m_pStartTemp;

	CTipBox*			m_pSelect;

private:
	int m_iStartIndexForIng;
	int	m_iStartIndexForEnd;

	int m_nTotalLine;
	int m_nLinePerOneView;

	MULTIMAP_AREA_LIST	m_multimapArea;
	MULTIMAP_AREA_LIST	m_multimapEndArea;

	DWORD	m_dwQuestIngID;
	DWORD	m_dwQuestEndID;
};