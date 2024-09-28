#pragma	once

#include "../../../RanLogic/Tournament/GLTournamentInfoDef.h"
#include "../Util/UIWindowObjectController.h"
#include "../../../EngineLib/GUInterface/InterfaceBaseDefine.h"
#include "../../../enginelib/GUInterface/BasicComboBoxRollOver.h"
#include "../../InnerInterface.h"
#include "../Util/UIWindowEx.h"


extern "C"
{
	#include "../../../=LuaPlus/src/LuaPlus/src/lua.h"
	#include "../../../=LuaPlus/src/LuaPlus/src/lualib.h"
	#include "../../../=LuaPlus/src/LuaPlus/src/lauxlib.h"
}

#include "../../../SigmaCore/Lua/MinLua.h"

#include "../../../SigmaCore/Lua/lua_tinker.h"

#include "./TournamentNextBattle.h"
#include "./TournamentComboBox.h"

class TournamentDashBoard : public CUIWindowEx
{
public:

public:
	TournamentDashBoard ( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual	~TournamentDashBoard ();

	enum
	{
		CONTROL_ID_NONE = NO_ID+5,
		CONTROL_ID_BUTTON_BETTING,
		CONTROL_ID_BUTTON_WATCH,
		CONTROL_ID_BUTTON_EXIT,
		CONTROL_ID_COMBO_BUTTON_ONE,
		CONTROL_ID_COMBO_BUTTON_TWO,
		CONTROL_ID_COMBO_ROLLOVER,

		CONTROL_ID_TOURNAMENT_TEXT00,
		CONTROL_ID_TOURNAMENT_TEXT01,
		CONTROL_ID_TOURNAMENT_TEXT02,
		CONTROL_ID_TOURNAMENT_TEXT03,
		CONTROL_ID_TOURNAMENT_TEXT04,
		CONTROL_ID_TOURNAMENT_TEXT05,
		CONTROL_ID_TOURNAMENT_TEXT06,
		CONTROL_ID_TOURNAMENT_TEXT07,
		CONTROL_ID_TOURNAMENT_TEXT08,
		CONTROL_ID_TOURNAMENT_TEXT09,
		CONTROL_ID_TOURNAMENT_TEXT10,
		CONTROL_ID_TOURNAMENT_TEXT11,
		CONTROL_ID_TOURNAMENT_TEXT12,
		CONTROL_ID_TOURNAMENT_TEXT13,
		CONTROL_ID_TOURNAMENT_TEXT14,
		CONTROL_ID_TOURNAMENT_TEXTEND,

		CONTROL_ID_TOURNAMENT_INFO_ICON_1,
		CONTROL_ID_TOURNAMENT_INFO_ICON_2,
		CONTROL_ID_TOURNAMENT_INFO_ICON_3,
		CONTROL_ID_TOURNAMENT_INFO_ICON_4,
		CONTROL_ID_TOURNAMENT_INFO_ICON_5,
		CONTROL_ID_TOURNAMENT_INFO_ICON_6,
		CONTROL_ID_TOURNAMENT_INFO_ICON_7,
		CONTROL_ID_TOURNAMENT_INFO_ICON_END,

		CONTROL_ID_TOURNAMENT_LINE_01,
		CONTROL_ID_TOURNAMENT_LINE_02,
		CONTROL_ID_TOURNAMENT_LINE_03,
		CONTROL_ID_TOURNAMENT_LINE_04,
		CONTROL_ID_TOURNAMENT_LINE_05,
		CONTROL_ID_TOURNAMENT_LINE_06,
		CONTROL_ID_TOURNAMENT_LINE_07,
		CONTROL_ID_TOURNAMENT_LINE_08,
		CONTROL_ID_TOURNAMENT_LINE_09,
		CONTROL_ID_TOURNAMENT_LINE_10,
		CONTROL_ID_TOURNAMENT_LINE_11,
		CONTROL_ID_TOURNAMENT_LINE_12,
		CONTROL_ID_TOURNAMENT_LINE_13,
		CONTROL_ID_TOURNAMENT_LINE_14,
		CONTROL_ID_TOURNAMENT_LINE_END,

		TOUR_INFO_NEXBATTLE_1ROUND,
		TOUR_INFO_NEXBATTLE_2ROUND,
		TOUR_INFO_NEXBATTLE_3ROUND,
		TOUR_INFO_NEXBATTLE_4ROUND,
		TOUR_INFO_NEXBATTLE_ROUND_END,
	};

	enum
	{
		DASHBOARD_A=0,
		DASHBOARD_B,
		DASHBOARD_C,
		DASHBOARD_D,
		DASHBOARD_AB,
		DASHBOARD_CD,
		DASHBOARD_ABCD,
		DASHBOARD_END,
	};

public:
public:
	void CreateSubControl ();
	void OnSetPage(DWORD _dwPageNum);
	void SetView(int _OneRoundCounting,int _MaxCounting,MatchingSystem::GLMatchingInfo* info,int Row);
	void SetViewDrawingLine();
	virtual	void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

private:
	void DashBoardVisible(BOOL visible);
private:
	int _DashItemRoundCount;
	int _DashBoardlistCount;
	int _DashBoxCount;
	BOOL _DashboxVisible;
protected:

	
	lua_State* pL;
public:


protected:
	

	int                 m_nCurrentFilter;

	DWORD				m_dwPageNum;

	DWORD				m_selectTournament;
	DWORD				m_ScriptID;
	DWORD				m_currentRound;

	GLGaeaClient* m_pGaeaClient;
	CBasicTextButton*	        m_pBettingButton;
	CBasicTextButton*	        m_pWatchingButton;
	CBasicTextButton*	        m_pExitButton;
	CBasicTextBox*		        m_pText_Title;
	CBasicTextBox*		        m_pText_Next_battle;

	CBasicTextBox*		        m_pText_Dash_Board_Text;

	CUIControl*					m_pBettlePageTable;

	CUIControl*					m_pDashboard[DASHBOARD_END];

	CBasicTextBox*				m_pDashboardText[15];
	DWORD						m_GroupID[15];
	DWORD						m_PosID[15];

	CUIControl*					m_pSelectHitBox[15];
	CUIControl*					m_pOrengeBox[15];

	CUIControl*					m_pTournamentLine[15];

	CBasicTextBox*				m_pRoundText[5];
	CUIControl*					m_pRoundBoxImg[5];

	TournamentNextBattle*		m_pBattle;

	TournamentComboBox*			m_pComboBox1;
	TournamentComboBox*			m_pComboBox2;
	int selection_indexJo;
	int selection_indexKang;
	bool m_isRoundStared;

};


class MyTournamentDashBoard : public ITournamentWindowDashBoard, private TournamentDashBoard
{
public:
	MyTournamentDashBoard( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual ~MyTournamentDashBoard() {};

public:
	virtual void OnCreate();
	virtual void OnClose() {};
	virtual void OnVisible(bool visible);
	virtual bool GetVisible() {return false;}
	virtual void SetPage(DWORD dwPageNum);
	virtual void OnFrameMove() {};
	virtual const UIRECT& GetGlobalPos();   
};
