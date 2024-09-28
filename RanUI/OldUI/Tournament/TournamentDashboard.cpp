#include "StdAfx.h"

#include "../../../RanLogic/GLUseFeatures.h"
#include "../../../RanLogic/Tournament/GLTournamentInfoDef.h"
#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/TipBox.h"
#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"
#include "../../../enginelib/GUInterface/BasicComboBoxRollOver.h"
#include "../../../EngineLib/G-Logic/GLogic.h"
#include "../../../RanLogicClient/Tournament/GLTournamentClient.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/PVE/GLPVEClient.h"
#include "./TournamentDashboard.h"
#include "./TournamentNextBattle.h"
#include "./TournamentComboBox.h"

#include <math.h>

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

using namespace MatchingSystem;

TournamentDashBoard::TournamentDashBoard( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CUIWindowEx(pInterface, pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_nCurrentFilter(0)
, selection_indexJo(0)
, selection_indexKang(0)
, _DashboxVisible(FALSE)
, m_isRoundStared(false)
{


}

TournamentDashBoard::~TournamentDashBoard()
{
	//Blank
}

void TournamentDashBoard::CreateSubControl ()
{
	Create( TOURNAMENT_DASHBOARD, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE);
	//
	
	CreateBaseWindowLightGray   ( "TOURNAMENT_DASHBOARDWINDOW", (char*)ID2GAMEWORD("TOURNAMENT_INFO", 0) );
	

	CreateLineBox  ( "TOURNAMENT_OBJECT_PAGE", "COMPETITION_OBJECT_PAGE_LINEBOX_TEXINFO" );
	//CreatePageFrame( "TOURNAMENT__OBJECT_PAGE" );
	//int result = lua_tinker::call<int>(pL, "CreateControl");




	CreateLineBox("TOURNAMENT_CTF_PAGE_INFO_LINEBOX_HEAD",	"COMPETITION_CTF_PAGE_INFO_LINEBOX_TEXINFO_W_GRAY");

	CreateLineBox("TOURNAMENT_CTF_PAGE_INFO_LINEBOX_BODY",	"COMPETITION_CTF_PAGE_INFO_LINEBOX_TEXINFO_W_BLACK");


	m_pBettlePageTable = new CUIControl(m_pEngineDevice);
	m_pBettlePageTable->CreateSub(this, "TOURNAMENT_INFO_TABLE_IMAGE");
	RegisterControl(m_pBettlePageTable);

	CString list[DASHBOARD_END];
	list[0] = "TOURNAMENT_INFO_ICON_1ROUND_1LIST";
	list[1] = "TOURNAMENT_INFO_ICON_1ROUND_2LIST";
	list[2] = "TOURNAMENT_INFO_ICON_1ROUND_3LIST";
	list[3] = "TOURNAMENT_INFO_ICON_1ROUND_4LIST";
	list[4] = "TOURNAMENT_INFO_ICON_2ROUND_2LIST";
	list[5] = "TOURNAMENT_INFO_ICON_2ROUND_1LIST";
	list[6] = "TOURNAMENT_INFO_ICON_3ROUND";


	for(int i=0; i<CONTROL_ID_TOURNAMENT_INFO_ICON_END-CONTROL_ID_TOURNAMENT_INFO_ICON_1;i++)
	{
		m_pDashboard[i] = new CUIControl(m_pEngineDevice);
		m_pDashboard[i]->CreateSub(this,list[i].GetString(),UI_FLAG_XSIZE,CONTROL_ID_TOURNAMENT_INFO_ICON_1+i);
		m_pDashboard[i]->SetVisibleSingle(FALSE);
		RegisterControl(m_pDashboard[i]);
	}


	//CreateLineBox("TOURNAMENT_PVE_DOWN_LINEBOX", "COMPETITION_CTF_PAGE_BATTLE_LINBOX_TEXINFO");

	//CreateLineBox("TOURNAMENT_CTF_PAGE_INFO_LINEBOX_ROUND",	"COMPETITION_CTF_PAGE_INFO_LINEBOX_TEXINFO_W_BLACK");

	//CreateLineBox( "RNFRIEND_FRIEND_TEXTEDIT_POS_BG", "GRAY_BACKGROUND_BLACK_LINE_TEXTURE", ID_TEXTBOX_BACKGROUND );

	CreateLineBox("TOURNAMENT_CTF_PAGE_INFO_LINEBOX_TAIL", "GRAY_BACKGROUND_BLACK_LINE_TEXTURE");

	//
	//CreateLineBox("COMPETITION_PVE_LINE_LISTBOX", "COMPETITION_PVE_PAGE_LISTBOX_TEXINFO_W_BLACK");


	//TOTO 버튼
	m_pBettingButton = new CBasicTextButton(m_pEngineDevice);
	m_pBettingButton->CreateSub(this, "BASIC_TEXT_BUTTON22", UI_FLAG_XSIZE, CONTROL_ID_BUTTON_BETTING);
	m_pBettingButton->CreateBaseButton("TOURNAMENT_OBJECT_BETTING", CBasicTextButton::SIZE22, 
		CBasicButton::CLICK_FLIP, ID2GAMEWORD("TOURNAMENT_UI_TEXT_BETTING_BUTTON", 0));
	RegisterControl(m_pBettingButton);

	//관전 버튼
	m_pWatchingButton = new CBasicTextButton(m_pEngineDevice);
	m_pWatchingButton->CreateSub(this, "BASIC_TEXT_BUTTON22", UI_FLAG_XSIZE, CONTROL_ID_BUTTON_WATCH);
	m_pWatchingButton->CreateBaseButton("TOURNAMENT_OBJECT_WATCHING", CBasicTextButton::SIZE22, 
		CBasicButton::CLICK_FLIP, ID2GAMEWORD("TOURNAMENT_UI_TEXT_WATCHING_BUTTON", 0));
	RegisterControl(m_pWatchingButton);

	m_pExitButton = new CBasicTextButton(m_pEngineDevice);
	m_pExitButton->CreateSub(this, "BASIC_TEXT_BUTTON22", UI_FLAG_XSIZE, CONTROL_ID_BUTTON_EXIT);
	m_pExitButton->CreateBaseButton( "TOURNAMENT_OBJECT_CLOSE", CBasicTextButton::SIZE22, 
		CBasicButton::CLICK_FLIP, ID2GAMEWORD("COMPETITION_WINDOW", 1) );
	RegisterControl( m_pExitButton );

	CD3DFontPar* pFont = m_pEngineDevice->LoadFont(_DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG);
	CD3DFontPar* pFont_Shadow = m_pEngineDevice->LoadFont(_DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG);
	CD3DFontPar* pFont_Small = m_pEngineDevice->LoadFont(_DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG);
	CD3DFontPar* pFont_BIG = m_pEngineDevice->LoadFont(_DEFAULT_FONT, 22, _DEFAULT_FONT_BOLD_FLAG);

	const int nAlignLeftTop = TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP;	
	const int nAlignLeftCenter = TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y;	
	const int nAlignCenter	   = TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y;	

	m_pText_Title = CreateStaticControl("TOURNAMENT_INFO_TITLE", pFont_Shadow,0, nAlignLeftCenter);	
	m_pText_Title->SetOneLineText(ID2GAMEWORD("TOURNAMENT_INFO_TABLE", 0), NS_UITEXTCOLOR::WHITE);

	

	m_pText_Dash_Board_Text = CreateStaticControl("TOURNAMENT_INFO_TABLE_TEXT_MESSAGE", pFont_BIG,0, nAlignCenter);
	//m_pText_Dash_Board_Text->SetOneLineText(ID2GAMEWORD("TOURNAMENT_INFO_TABLE_TEXT_MESSAGE_TXT",0), NS_UITEXTCOLOR::WHITE);
	m_pText_Dash_Board_Text->SetVisibleSingle(FALSE);


	m_pText_Next_battle = CreateStaticControl("TOURNAMENT_INFO_NEXT_POS", pFont_Shadow,0, nAlignLeftCenter);	
	m_pText_Next_battle->SetOneLineText(ID2GAMEWORD("TOURNAMENT_INFO_NEXT", 0), NS_UITEXTCOLOR::WHITESMOKE);

	//! CTF 전투
	{
		m_pBattle = new TournamentNextBattle(m_pGaeaClient, m_pInterface, m_pEngineDevice);
		m_pBattle->CreateSub( this, "TOURNAMENT_PAGE_BATTLE", UI_FLAG_DEFAULT, NO_ID );
		m_pBattle->CreateSubControl();
		RegisterControl( m_pBattle );
	}

	


	{	// 토너먼트 안의 글자들! 캐릭터이름 들어갈곳
		CString DashBoardlist[15];
		_DashBoxCount = 0;
		DashBoardlist[_DashBoxCount++] = "TOUR_INFO_BOX_8K_1";
		DashBoardlist[_DashBoxCount++] = "TOUR_INFO_BOX_8K_2";
		DashBoardlist[_DashBoxCount++] = "TOUR_INFO_BOX_8K_3";
		DashBoardlist[_DashBoxCount++] = "TOUR_INFO_BOX_8K_4";
		DashBoardlist[_DashBoxCount++] = "TOUR_INFO_BOX_8K_5";
		DashBoardlist[_DashBoxCount++] = "TOUR_INFO_BOX_8K_6";
		DashBoardlist[_DashBoxCount++] = "TOUR_INFO_BOX_8K_7";
		DashBoardlist[_DashBoxCount++] = "TOUR_INFO_BOX_8K_8";
		DashBoardlist[_DashBoxCount++] = "TOUR_INFO_BOX_4K_1";
		DashBoardlist[_DashBoxCount++] = "TOUR_INFO_BOX_4K_2";
		DashBoardlist[_DashBoxCount++] = "TOUR_INFO_BOX_4K_3";
		DashBoardlist[_DashBoxCount++] = "TOUR_INFO_BOX_4K_4";
		DashBoardlist[_DashBoxCount++] = "TOUR_INFO_BOX_2K_1";
		DashBoardlist[_DashBoxCount++] = "TOUR_INFO_BOX_2K_2";
		DashBoardlist[_DashBoxCount++] = "TOUR_INFO_BOX_1K";
		
		for(int i =0 ; i < _DashBoxCount ; i ++)
		{
			CString strTemp;
			strTemp.Format("TOUR_TOTO_SELECTBOX%d", i + 1 );

			m_pOrengeBox[i]	=	CreateLineBox(DashBoardlist[i].GetString(),"COLORSELECTOR_WINDOW_BUTTON_DEFAULT");
			m_pOrengeBox[i]->SetVisibleSingle(FALSE);
		}

		for(int i=0; i<_DashBoxCount;i++)
		{

			m_pDashboardText[i] = CreateStaticControl(DashBoardlist[i].GetString(), pFont_Shadow,0, nAlignLeftCenter);
			m_pDashboardText[i]->SetOneLineText("", NS_UITEXTCOLOR::WHITE);
			
		}

		for (int i =0 ; i < _DashBoxCount ; i ++)
		{
			m_pSelectHitBox[i] = CreateLineBox(DashBoardlist[i].GetString(),"TOUR_TOTO_HITBOX",CONTROL_ID_TOURNAMENT_TEXT00+i);
			m_pSelectHitBox[i]->SetVisibleSingle(TRUE);
		}
		
	}

	
	{	
		CString DashBoardlist[15];
		_DashBoardlistCount = 0;
		DashBoardlist[_DashBoardlistCount++] = "TOUR_INFO_TAB_LINE_2R_UP_1L";
		DashBoardlist[_DashBoardlistCount++] = "TOUR_INFO_TAB_LINE_2R_DOWN_1L";
		DashBoardlist[_DashBoardlistCount++] = "TOUR_INFO_TAB_LINE_2R_UP_2L";
		DashBoardlist[_DashBoardlistCount++] = "TOUR_INFO_TAB_LINE_2R_DOWN_2L";
		DashBoardlist[_DashBoardlistCount++] = "TOUR_INFO_TAB_LINE_2R_UP_3L";
		DashBoardlist[_DashBoardlistCount++] = "TOUR_INFO_TAB_LINE_2R_DOWN_3L";
		DashBoardlist[_DashBoardlistCount++] = "TOUR_INFO_TAB_LINE_2R_UP_4L";
		DashBoardlist[_DashBoardlistCount++] = "TOUR_INFO_TAB_LINE_2R_DOWN_4L";

		DashBoardlist[_DashBoardlistCount++] = "TOUR_INFO_TAB_LINE_3R_UP_1L";
		DashBoardlist[_DashBoardlistCount++] = "TOUR_INFO_TAB_LINE_3R_DOWN_1L";
		DashBoardlist[_DashBoardlistCount++] = "TOUR_INFO_TAB_LINE_3R_UP_2L";
		DashBoardlist[_DashBoardlistCount++] = "TOUR_INFO_TAB_LINE_3R_DOWN_2L";

		DashBoardlist[_DashBoardlistCount++] = "TOUR_INFO_TAB_LINE_4R_UP";
		DashBoardlist[_DashBoardlistCount++] = "TOUR_INFO_TAB_LINE_4R_DOWN";
		
		for(int i=0; i<_DashBoardlistCount;i++)
		{
			m_pTournamentLine[i] = new CUIControl(m_pEngineDevice);
			m_pTournamentLine[i]->CreateSub(this,DashBoardlist[i].GetString(),UI_FLAG_XSIZE,CONTROL_ID_TOURNAMENT_LINE_01+i);
			m_pTournamentLine[i]->SetVisibleSingle(FALSE);
			RegisterControl(m_pTournamentLine[i]);
		}
	}


	{	// 토너먼트 라운드 글자

		CString DashBoardlist[5];
		_DashItemRoundCount = 0;

		DashBoardlist[_DashItemRoundCount++] = "TOUR_INFO_NEXBATTLE_1ROUND";
		DashBoardlist[_DashItemRoundCount++] = "TOUR_INFO_NEXBATTLE_2ROUND";
		DashBoardlist[_DashItemRoundCount++] = "TOUR_INFO_NEXBATTLE_3ROUND";
		DashBoardlist[_DashItemRoundCount++] = "TOUR_INFO_NEXBATTLE_4ROUND";
		for(int i=0; i<_DashItemRoundCount;i++)
		{

			m_pRoundBoxImg[i] = CreateLineBox(DashBoardlist[i].GetString(),"COMPETITION_CTF_PAGE_INFO_LINEBOX_TEXINFO_W_BLACK");
			
			//N 라운드 글자 만들기
			CString strTemp;
			strTemp.Format(ID2GAMEWORD("TOURNAMENT_OBJECT_ROUND_VIEW_TEXT",0), i+1 );
			
			m_pRoundText[i] = CreateStaticControl(DashBoardlist[i].GetString(), pFont_Small,0, nAlignCenter);
			m_pRoundText[i]->SetOneLineText(strTemp.GetString(), NS_UITEXTCOLOR::WHITE);
		}
	}

	m_pComboBox1 = new TournamentComboBox(m_pGaeaClient, m_pInterface, m_pEngineDevice);
	m_pComboBox1->CreateSub(this,"TOURNAMENT_COMBO1_VIEWCOMBO_BUTTON",UI_FLAG_XSIZE,CONTROL_ID_COMBO_BUTTON_ONE);
	m_pComboBox1->CreateSubControl();
	RegisterControl( m_pComboBox1 );

	m_pComboBox2 = new TournamentComboBox(m_pGaeaClient, m_pInterface, m_pEngineDevice);
	m_pComboBox2->CreateSub(this,"TOURNAMENT_COMBO2_VIEWCOMBO_BUTTON",UI_FLAG_XSIZE,CONTROL_ID_COMBO_BUTTON_TWO);
	m_pComboBox2->CreateSubControl();
	RegisterControl( m_pComboBox2 );


	DashBoardVisible(FALSE);

	
	SetAlignFlag( UI_FLAG_CENTER_Y | UI_FLAG_CENTER_X );
	m_pInterface->UiRegisterControl(this,true);

}
void TournamentDashBoard::DashBoardVisible(BOOL visible)
{

	_DashboxVisible = visible;
	m_pBettlePageTable->SetVisibleSingle(visible);

	
	
	for(int i=0; i<_DashBoxCount;i++)
	{
		m_pDashboardText[i]->SetVisibleSingle(visible);
	}

	if (visible == FALSE)
	{
		for(int i=0; i<_DashBoardlistCount;i++)
		{
			m_pTournamentLine[i]->SetVisibleSingle(visible);
		}
		for(int i=0; i<CONTROL_ID_TOURNAMENT_INFO_ICON_END-CONTROL_ID_TOURNAMENT_INFO_ICON_1;i++)
		{
			m_pDashboard[i]->SetVisibleSingle(visible);
		}
	}
	

	m_pComboBox1->SetVisibleSingle(visible);
	m_pComboBox2->SetVisibleSingle(visible);

	if (visible == FALSE)
	{
		for(int i =0 ; i < 15 ; i ++)
		{
			m_pOrengeBox[i]->SetVisibleSingle(FALSE);
		}

		m_pText_Dash_Board_Text->SetVisibleSingle(TRUE);
	}
	else
	{
		m_pText_Dash_Board_Text->SetVisibleSingle(FALSE);
	}
	
}

void TournamentDashBoard::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIWindowEx::Update(x,y,LB,MB,RB,nScroll,fElapsedTime,bFirstControl);
	UIRECT testsimple = GetGlobalPos();
	UIRECT RecSimple = GetLocalPos();

	MatchingSystem::GLTouranmentClient* m_pLogic = m_pGaeaClient->GetTournamentClient();

	if(m_pLogic->GetGroupInfoOutofDate())
	{
		m_pLogic->UpdateDashboard(m_pLogic->m_OutofDateID);
		m_pLogic->SetGroupInfoOutofDate(false);
	}

	if(m_pLogic->GetDashboardupdate())
	{
		
		OnSetPage(m_dwPageNum);
		m_pLogic->SetDashboardupdate(false);
	}
	
	int newindexKan = m_pComboBox1->GetSelectIndex();
	int newIndexJo = m_pComboBox2->GetSelectIndex();
	if(selection_indexKang != newindexKan)
	{
		m_pComboBox2->SetSelectText(0);
		selection_indexJo = 0;
		selection_indexKang = newindexKan;
		OnSetPage(m_dwPageNum);	
	}
	else if(newIndexJo != selection_indexJo)
	{
		selection_indexJo = newIndexJo;
		OnSetPage(m_dwPageNum);
	}
	

	
	

}

void TournamentDashBoard::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage(ControlID,dwMsg);
	MatchingSystem::GLTouranmentClient* m_pLogic = m_pGaeaClient->GetTournamentClient();

	switch( ControlID )
	{
		
	case CONTROL_ID_BUTTON_BETTING:
		if( CHECK_MOUSE_IN( dwMsg ) && CHECK_LB_UP_LIKE( dwMsg ) )
		{
			if(_DashboxVisible == FALSE)
				return;

			if(m_pComboBox1->GetSelectIndex() < 0 || m_pComboBox2->GetSelectIndex() <0)
				return;

			//MatchingSystem::GLMatchingInfo* info; 참조되지 않은 지역 변수입니다.

			
			if(m_isRoundStared == true)
			{
				CString strTemp;
				strTemp.Format(ID2GAMEINTEXT("TOURNAMENT_EM_BETTING_OUT_OF_TIME"));
				m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::RED, strTemp.GetString());
				return;
			}			
			//


			

			m_pLogic->m_SelectScriptID = m_ScriptID;
			m_pLogic->GetBettingInfo(m_ScriptID);
			m_pInterface->OpenTournamentBettingWindow(
				m_pComboBox1->GetSelectIndex(),m_pComboBox2->GetSelectIndex());

			return;
		}
		break;
	case CONTROL_ID_BUTTON_WATCH:
		if( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
		{
			if(_DashboxVisible == FALSE)
				return;

			MatchingSystem::GLTouranmentClient* m_pLogic = m_pGaeaClient->GetTournamentClient();
			//m_pGaeaClient->ReqPVEEntrance(pChar->GetCharID(), pInfo->mapid.dwID, pInfo->gateid, nLevel);
			//m_GroupID[UIstartIndex];
			if ( m_selectTournament < 15 && m_pDashboardText[m_selectTournament]->GetText(0) != "" )
				m_pLogic->ObserveTournament(m_ScriptID,m_pDashboardText[m_selectTournament]->GetText(0));
			
		}
		break;

	case CONTROL_ID_BUTTON_EXIT:
		if( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
		{
			

			m_pInterface->UiHideGroup( GetWndID() );
		}
		break;
	case CONTROL_ID_TOURNAMENT_TEXT00:
	case CONTROL_ID_TOURNAMENT_TEXT01:
	case CONTROL_ID_TOURNAMENT_TEXT02:
	case CONTROL_ID_TOURNAMENT_TEXT03:
	case CONTROL_ID_TOURNAMENT_TEXT04:
	case CONTROL_ID_TOURNAMENT_TEXT05:
	case CONTROL_ID_TOURNAMENT_TEXT06:
	case CONTROL_ID_TOURNAMENT_TEXT07:
	case CONTROL_ID_TOURNAMENT_TEXT08:
	case CONTROL_ID_TOURNAMENT_TEXT09:
	case CONTROL_ID_TOURNAMENT_TEXT10:
	case CONTROL_ID_TOURNAMENT_TEXT11:
	case CONTROL_ID_TOURNAMENT_TEXT12:
	case CONTROL_ID_TOURNAMENT_TEXT13:
	case CONTROL_ID_TOURNAMENT_TEXT14:

		if( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
		{
			if(_DashboxVisible == FALSE)
				return;
			
			int _index = ControlID - CONTROL_ID_TOURNAMENT_TEXT00;

			if(_index < 0)
				return;

			MatchingSystem::GLMatChingGroupInfo* GroupInfo = m_pLogic->GetGroupInfobyIndex(m_PosID[_index],m_ScriptID);

			if(GroupInfo == NULL)
				return;

			if(GroupInfo->Position != m_currentRound )
				return;

			//m_currentRound

			for(int i =0 ; i < 15 ; i ++)
			{
				m_pOrengeBox[i]->SetVisibleSingle(FALSE);
			}

			if(m_pDashboardText[_index]->GetText(0) != "")
				m_pOrengeBox[_index]->SetVisibleSingle(TRUE);

			m_selectTournament = (DWORD)_index;
		}
		

		break;
		
	}
}
void TournamentDashBoard::SetView(int _OneRoundCounting,int _MaxCounting,MatchingSystem::GLMatchingInfo* info,int Row)
{

	int KangMaxIndex= m_pComboBox1->GetMaxIndex();
	int KangIndex =selection_indexKang;
	int JoMaxIndex = m_pComboBox2->GetMaxIndex();
	int JoIndex = selection_indexJo;

	//줄수 8개 보여야되면 8 4개짜리면 4  ..2. ..1.

	// 조의 승수 즉 몇제곱(2^x ,2의 x승)인지 알아내는것
	int JpowValue = 0;
	{
		// 줄수를 다시 역순으로 계산하기위한 지역변수
		int Rowvar = Row;

		for(int i = 0 ; Rowvar != 1; i ++)
		{
			JpowValue++;
			Rowvar = Rowvar / 2;
		}
	}

	int uiIndex = 8;
	switch (Row)
	{
	case 8:

		uiIndex = 0;
		break;
	case 4:
		uiIndex = 8;
		break;
	case 2:
		uiIndex = 12;
		break;
	case 1:
		uiIndex = 14;
		break;
	default:
		break;
	}

	int UIIndexPush = 0;
	//UIIndexPush
	if(_OneRoundCounting <= 1)
	{
		UIIndexPush = 14;
	}
	else if(_OneRoundCounting <= 2)
	{
		UIIndexPush = 12;
	}
	else if(_OneRoundCounting <= 4)
	{
		UIIndexPush = 8;
	}
	else
	{
		UIIndexPush = 0;
	}

	// 2^0 승은 0이므로 0부터 카운팅
	//내가 보여야할 조 위치를 계산함
	JpowValue = 3 - JpowValue;
	// 1 2 4 8 
	// 0 1 2 3



	//시작 위치
	int StartIndex = 0;

	// 조선택한만큼 밀려야 한다.
	int SelectReverce = KangMaxIndex- KangIndex + JpowValue;

	if( SelectReverce == 0)
	{
		//선택한 조 숫자가 최고 숫자라면 1부터 계산이므로 더 따져볼 필요없다.
		StartIndex = 1;
	}
	else
	{
		//제곱수
		int Increace = 0;

		// 승수 계산 
		int PowValue = _MaxCounting;
		for(int i = 0 ; i < SelectReverce  ; i ++)
		{
			//제곱수가 감소하면서 결승전까지의 갯수를 구할수있다.
			// 8강 16강 32강 ...
			// 2^3  2^4  2^5
			// ..3...4....5
			// ..0...1.. .2 .. 
			//  48   32   1 <--- 이게 우리가 구해야할 숫자.
			Increace +=  pow(2,(float)PowValue);
			PowValue--;
		}

		// 그리곤 시작위치 1+ 증가값을 더한다.
		StartIndex += Increace +1;
	}

	//그리고 조숫자와 줄숫자를 곱하여 위치값을 계산한다.
	StartIndex += JoIndex * Row;

	int j = 0;
	for(int i = 0 ; i < Row; i ++, j++)
	{
		if(j<0)
			continue;
		int UIPostion = uiIndex + i + UIIndexPush;
		if(UIPostion> 14)
			return;

		//printf("%d::%d\n",StartIndex + j,uiIndex + i);
		BOOST_FOREACH( MatchingSystem::GLMatChingGroupInfo _node , info->m_GroupListInfo )
		{
			if(StartIndex + j == _node.Index)
			{
				CString CharName(_node.GroupName.c_str());
				if(CharName == "")
					CharName = "*****";

				m_pDashboardText[UIPostion]->SetOneLineText(CharName, NS_UITEXTCOLOR::WHITE);
				m_GroupID[UIPostion] = _node.dwGROUPID;
				m_PosID[UIPostion] = _node.Index;
				break;
			}

		}

	}
}

void TournamentDashBoard::SetViewDrawingLine()
{
	MatchingSystem::GLTouranmentClient* m_pLogic = m_pGaeaClient->GetTournamentClient();

	/*------------------------------------------------------------------*/

	for(int i = 0 ; i < _DashBoardlistCount ; i++)
	{
		m_pTournamentLine[i]->SetVisibleSingle(FALSE);
	}

	int j = 8;
	for(int i = 0 ; i < _DashBoardlistCount ; i++,i++,j++)
	{

		int k = i/2;

		if(j < 15)
		{
			if(m_GroupID[i] == m_GroupID[j])
			{
				if(m_GroupID[j] == -1)
					continue;
				m_pTournamentLine[i]->SetVisibleSingle(TRUE);
				m_pTournamentLine[i+1]->SetVisibleSingle(FALSE);
				m_pDashboard[k]->SetVisibleSingle(FALSE);
				m_pDashboardText[i+1]->SetOneLineText(m_pDashboardText[i+1]->GetText(0),NS_UITEXTCOLOR::DARKGRAY );
				m_pDashboardText[i]->SetOneLineText(m_pDashboardText[i]->GetText(0),NS_UITEXTCOLOR::YELLOW );
			}	
			else if(m_GroupID[i+1] == m_GroupID[j])
			{
				if(m_GroupID[j] == -1)
					continue;

				m_pTournamentLine[i]->SetVisibleSingle(FALSE);
				m_pTournamentLine[i+1]->SetVisibleSingle(TRUE);
				m_pDashboard[k]->SetVisibleSingle(FALSE);
				m_pDashboardText[i]->SetOneLineText(m_pDashboardText[i]->GetText(0),NS_UITEXTCOLOR::DARKGRAY );
				m_pDashboardText[i+1]->SetOneLineText(m_pDashboardText[i+1]->GetText(0),NS_UITEXTCOLOR::YELLOW );
			}
		}
		if(m_pTournamentLine[i]->IsVisible() == FALSE && m_pTournamentLine[i+1]->IsVisible() == FALSE)
		{		

			if(m_GroupID[i] == -1)
				continue;
			if(m_GroupID[i+1] == -1)
				continue;

			MatchingSystem::GLMatChingGroupInfo* GroupInfo = m_pLogic->GetGroupInfobyIndex(m_PosID[i],m_ScriptID);

			if(GroupInfo == NULL)
				continue;
			if (GroupInfo->Position == m_currentRound)
			{
				m_pDashboard[k]->SetVisibleSingle(TRUE);
			}
			else
			{
				m_pDashboard[k]->SetVisibleSingle(FALSE);	
				
			}
			
		}
		
		
	}
}

void TournamentDashBoard::OnSetPage(DWORD _dwPageNum)
{
	for(int i = 0 ; i < 15 ; i ++)
		m_GroupID[i] = -1;

	m_dwPageNum = _dwPageNum;
	size_t cur = 0;

	MatchingSystem::GLTouranmentClient* m_pLogic = m_pGaeaClient->GetTournamentClient();

	MatchingSystem::GLTouranmentClient::VEC_MATCHINFO_ITOR _bitor = m_pLogic->m_VecMatchingInfo.begin();
	MatchingSystem::GLTouranmentClient::VEC_MATCHINFO_ITOR _eitor = m_pLogic->m_VecMatchingInfo.end();

	MatchingSystem::GLMatchingInfo* info = NULL;
	


	for(int i=0; i<CONTROL_ID_TOURNAMENT_INFO_ICON_END-CONTROL_ID_TOURNAMENT_INFO_ICON_1;i++)
	{
		m_pDashboard[i]->SetVisibleSingle(FALSE);
	}


	for (; _bitor != _eitor; ++_bitor)
	{
		MatchingSystem::GLMatchingInfo* _Node;

		_Node = *_bitor;
		
		if (_Node->m_ScriptID == m_dwPageNum)
		{
			info = *_bitor;

			bool isProgress = false;

			if(info->m_vecTimeInfo.size() == 0 )
				return;

			CTime cCurTime = CTime::GetCurrentTime();
			int nDayOfWeek = cCurTime.GetDayOfWeek ();
			int nDay = cCurTime.GetDay ();
			int nHour = cCurTime.GetHour();
			int nMinute = cCurTime.GetMinute ();


			BOOST_FOREACH(GLMatching_TimeInfo _timeInfo,info->m_vecTimeInfo)
			{
				if ( ( _timeInfo.wWeekDay != 0 ) && ( _timeInfo.wWeekDay < nDayOfWeek ) )
					continue;

				const int nHourInterval = _timeInfo.wStartHour - nHour;
				const int nMinInterval = _timeInfo.wStartMin - nMinute;

				if ( nHourInterval < 0 )
					continue;

				int nInterval = nHourInterval * 60;
				nInterval += nMinInterval;

				if ( nInterval < 0 )
				{
                    // TO DO : 정기식 컨텐츠의 날짜별 진행이 사라짐, UI변경 되어야함;
					//m_pBattle->OnSetDateTime(0);
					m_pBattle->OnSetStartTime(_timeInfo.wStartHour,_timeInfo.wStartMin);
					m_pBattle->OnSetEndTime(_timeInfo.wStartHour + (int)(_timeInfo.dwDurationMin/60)
						,_timeInfo.wStartMin + (int)(_timeInfo.dwDurationMin%60) );
					m_pBattle->OnSetProgressTime((int)(_timeInfo.dwDurationMin/60)
						,(int)(_timeInfo.dwDurationMin%60));
					isProgress =true;
					break;
				}

				//시간 지났음 참가 불가능합니당.
				break;
			}

			if(isProgress == false)
			{	
				

				GLMatching_TimeInfo _timeInfo = info->m_vecTimeInfo[0];
                // TO DO : 정기식 컨텐츠의 날짜별 진행이 사라짐, UI변경 되어야함;
				//m_pBattle->OnSetDateTime(0);
				m_pBattle->OnSetStartTime(_timeInfo.wStartHour,_timeInfo.wStartMin);
				m_pBattle->OnSetEndTime(_timeInfo.wStartHour + (int)(_timeInfo.dwDurationMin/60)
					,_timeInfo.wStartMin + (int)(_timeInfo.dwDurationMin%60) );
				m_pBattle->OnSetProgressTime((int)(_timeInfo.dwDurationMin/60)
					,(int)(_timeInfo.dwDurationMin%60));
			}

			m_pBattle->OnSetStatus(info->m_ContentsStatus);
			m_pBattle->OnSetBetting(info->m_bCanTOTO);
			m_pBattle->OnSetView(info->m_bCanObserve);			
			break;
		}
		cur++;
	}


	if(info == NULL)
		return;

	
	if(info->m_bCanTOTO == false)
	{
		m_pBettingButton->SetDiffuse( NS_UITEXTCOLOR::DIMGREY );
		m_pBettingButton->SetDiffuseAlpha( 0xDD );
		m_pBettingButton->SetNoMessage( TRUE );

		
	}
	else
	{
		m_pBettingButton->SetDiffuse( NS_UITEXTCOLOR::WHITE );
		m_pBettingButton->SetDiffuseAlpha( 0xFF );
		m_pBettingButton->SetNoMessage( FALSE );
	}

	if(info->m_bCanObserve == false)
	{
		m_pWatchingButton->SetDiffuse( NS_UITEXTCOLOR::DIMGREY );
		m_pWatchingButton->SetDiffuseAlpha( 0xDD );
		m_pWatchingButton->SetNoMessage( TRUE );

		
	}
	else
	{
		m_pWatchingButton->SetDiffuse( NS_UITEXTCOLOR::WHITE );
		m_pWatchingButton->SetDiffuseAlpha( 0xFF );
		m_pWatchingButton->SetNoMessage( FALSE );
	}


	

	
	int StartPos = _DashBoxCount-info->m_dwMatchingSize;

	
	// UI 텍스트 리셋
	for(int i= 0 ; i < 15 ; i++)
	{
		m_pDashboardText[i]->SetOneLineText("", NS_UITEXTCOLOR::WHITE);
	}

	for(int i=0; i<_DashBoardlistCount;i++)
	{
		m_pTournamentLine[i]->SetVisibleSingle(FALSE);
	}

	for(int i=0; i<_DashItemRoundCount;i++)
	{
		m_pRoundText[i]->SetOneLineText("", NS_UITEXTCOLOR::WHITE);
	}


	BOOL dashbordvisible = TRUE;

	switch(info->m_ContentsStatus)
	{
	case MatchingSystem::EMMATCHING_CONTENTS_OPENED:
		m_pText_Dash_Board_Text->SetOneLineText(ID2GAMEINTEXT("TOURNAMENT_INFO_CAN_APPLY",0), NS_UITEXTCOLOR::WHITE);
		dashbordvisible = FALSE;
		DashBoardVisible(FALSE);
		break;
		//case MatchingSystem::EMTORNAMENTSTATUS_PENDING_APPLICATION:
		//	m_pText_Dash_Board_Text->SetOneLineText(ID2GAMEINTEXT("TOURNAMENT_INFO_PENDING_APPLICATION",0), NS_UITEXTCOLOR::WHITE);
		//	DashBoardVisible(FALSE);
		//	break;
	case MatchingSystem::EMMATCHING_CONTENTS_STANDBY_JOIN:
		m_pText_Dash_Board_Text->SetOneLineText(ID2GAMEINTEXT("TOURNAMENT_INFO_STANDBY_POSITION",0), NS_UITEXTCOLOR::WHITE);

		DashBoardVisible(TRUE);
		break;
	case MatchingSystem::EMMATCHING_CONTENTS_CLOSED:
		m_pText_Dash_Board_Text->SetOneLineText(ID2GAMEINTEXT("TOURNAMENT_INFO_NOT_AVAILABLE_TO_ORDER",0), NS_UITEXTCOLOR::RED);
		dashbordvisible = FALSE;
		DashBoardVisible(FALSE);
		break;
	case MatchingSystem::EMMATCHING_CONTENTS_IN_PROGRESS:
		m_pText_Dash_Board_Text->SetOneLineText("", NS_UITEXTCOLOR::WHITE);
		DashBoardVisible(TRUE);
		break;
	}

	if ( dashbordvisible == FALSE)
		return;

	if(info->m_GroupListInfo.size() == 0)
		return;


	int OneRoundPlayerCounting = 0;

	int CurrontMaxRound = 1;

	bool isRoundStared = false;

	if(info == NULL)
		return;
	if(info->m_GroupListInfo.size() == 0)
		return;
	// 라운드 1의 갯수.
	BOOST_FOREACH( MatchingSystem::GLMatChingGroupInfo _node , info->m_GroupListInfo )
	{	
		if(_node.Position == 1)
			OneRoundPlayerCounting++;
		else if(_node.Position >= 1)
			isRoundStared = true;

		if(CurrontMaxRound < static_cast<int>(_node.Position))
			CurrontMaxRound = _node.Position;
	}

	m_isRoundStared = isRoundStared;

	int MaxRoundCount =1;
	int Counting = 1;
	while (true)
	{
		if (info->m_dwMatchingSize > pow(2.0,Counting) )
			Counting ++;
		else
			break;
	}

	MaxRoundCount = Counting ;	



	if (dashbordvisible == TRUE )
	{
		SetView(OneRoundPlayerCounting,MaxRoundCount,info,8);
		SetView(OneRoundPlayerCounting,MaxRoundCount,info,4);
		SetView(OneRoundPlayerCounting,MaxRoundCount,info,2);
		SetView(OneRoundPlayerCounting,MaxRoundCount,info,1);
		SetViewDrawingLine();
	}

	m_ScriptID = info->m_ScriptID;

/*
	BOOST_FOREACH( MatchingSystem::GLMatChingGroupInfo _node , info->m_GroupListInfo )
	{
		if(_node.Position-1 > _DashBoxCount)
			continue;

		CString CharName(_node.Groupname.c_str());
		if(CharName == "")
			CharName = "*****";


		m_pDashboardText[_node.Position -1]->SetOneLineText(CharName, NS_UITEXTCOLOR::WHITE);
	}*/

	m_pComboBox1->ClearText();
	m_pComboBox2->ClearText();

	int TapSize = 1;
	int VisiableSize = 8;

	//SetSelectText

	


	while (true)
	{
		if (info->m_dwMatchingSize > pow(2.0,TapSize+2) )
		{
			TapSize++;
		}
		else
		{
			break;
		}
	}

	for(int i = 1 ; i <= TapSize; i++)
	{
		CString strMatchUp;
		strMatchUp.Format(ID2GAMEWORD("TOUR_TOTO_KANG_TEXTBOX_TEXT"), (int)pow(2.0,i+2) );
		m_pComboBox1->AddText(strMatchUp.GetString());
	}

	int JoSize = m_pComboBox1->GetSelectIndex();
	m_pComboBox1->SetSelectText(JoSize);
	
	for(int i = 1 ; i <= (int)pow(2.0,JoSize) ; i++)
	{
		CString strMatchUp;
		strMatchUp.Format(ID2GAMEWORD("TOUR_TOTO_JO_TEXTBOX_TEXT"), i);
		m_pComboBox2->AddText(strMatchUp.GetString());
	}

	if (selection_indexJo > (int)pow(2.0,JoSize) )
		m_pComboBox2->SetSelectText(0);
	else
		m_pComboBox2->SetSelectText(m_pComboBox2->GetSelectIndex());

	//m_pComboBox1->SetSelectText(0);

	//m_pComboBox2->SetSelectText(selection_indexJo);


	MaxRoundCount += 1;
	int SelectInde = selection_indexKang;
	int RoundCounting =  MaxRoundCount - SelectInde - 4;
	
	if(RoundCounting < 0)
		RoundCounting = 0;

	int UIIndexPush = 0;
	//UIIndexPush
	if(OneRoundPlayerCounting <= 1)
	{
		UIIndexPush = 3;
	}
	else if(OneRoundPlayerCounting <= 2)
	{
		UIIndexPush = 2;
	}
	else if(OneRoundPlayerCounting <= 4)
	{
		UIIndexPush = 1;
	}
	else
	{
		UIIndexPush = 0;
	}

	MaxRoundCount += UIIndexPush;
	CurrontMaxRound += UIIndexPush;

	m_currentRound = info->m_dwCurrentRound;

	for(int i = 0 ; i < 4 ; i++ )
	{
		
		CString strTemp;
		if(RoundCounting + i+1  < 1)
		{
			strTemp= "    ";
			m_pRoundText[i]->SetOneLineText(strTemp.GetString(), NS_UITEXTCOLOR::BLACK);
			continue;
		}

		if(UIIndexPush!= 0)
		{
			if(i<UIIndexPush)
			{
				strTemp= "    ";
				m_pRoundText[i]->SetOneLineText(strTemp.GetString(), NS_UITEXTCOLOR::BLACK);
				continue;
			}
		}

		//TOURNAMENT_OBJECT_ROUND_VIEW_TEXT

		if(MaxRoundCount ==  RoundCounting + i+1 ) // WINNER
			strTemp.Format(ID2GAMEWORD("TOURNAMENT_OBJECT_ROUND_VIEW_TEXT",3), RoundCounting + i+1 );
		else if(MaxRoundCount -1 ==  RoundCounting + i+1 ) // FINAL
			strTemp.Format(ID2GAMEWORD("TOURNAMENT_OBJECT_ROUND_VIEW_TEXT",1), RoundCounting + i+1 );
		else if(MaxRoundCount -2 ==  RoundCounting + i+1 ) //SEMI FINAL
			strTemp.Format(ID2GAMEWORD("TOURNAMENT_OBJECT_ROUND_VIEW_TEXT",2), RoundCounting + i+1 );
		else
			strTemp.Format(ID2GAMEWORD("TOURNAMENT_OBJECT_ROUND_VIEW_TEXT",0), RoundCounting + i+1 );

		
		//N 라운드 글자 만들기

		

		
		if(i +RoundCounting == m_currentRound)
			m_pRoundText[i]->SetOneLineText(strTemp.GetString(), NS_UITEXTCOLOR::YELLOW);
		else if(i+RoundCounting < m_currentRound)
			m_pRoundText[i]->SetOneLineText(strTemp.GetString(), NS_UITEXTCOLOR::BLACK);
		else
			m_pRoundText[i]->SetOneLineText(strTemp.GetString(), NS_UITEXTCOLOR::WHITE);
	}


	//pow(8,1)
	

}
//----------------------------------------------------------------------------------------------------//

MyTournamentDashBoard::MyTournamentDashBoard( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: TournamentDashBoard( pGaeaClient, pInterface, pEngineDevice )
{
}

void MyTournamentDashBoard::OnCreate() 
{ 
	CreateSubControl();
}

void MyTournamentDashBoard::OnVisible(bool visible)
{
	MatchingSystem::GLTouranmentClient* m_pLogic = m_pGaeaClient->GetTournamentClient();
	m_pLogic->GetTournamentInfomation();

	TournamentDashBoard::m_pInterface->UiShowGroupFocus( TOURNAMENT_DASHBOARD );
}

const UIRECT& MyTournamentDashBoard::GetGlobalPos()
{
	return TournamentDashBoard::GetGlobalPos();
}


void MyTournamentDashBoard::SetPage(DWORD dwPageNum)
{	
	TournamentDashBoard::OnSetPage(dwPageNum);
}