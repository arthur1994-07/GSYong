#include "StdAfx.h"

#include "../../../RanLogic/GLUseFeatures.h"

#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/DeviceMan.h"

#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/TipBox.h"
#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../enginelib/GUInterface/BasicComboBoxRollOver.h"
#include "../../../RanLogicClient/Tournament/GLTournamentClient.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"


#include "./TournamentDashboard.h"
#include "./TournamentToTo.h"

#include "./TournamentComboBox.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

TournamentToToWindow::TournamentToToWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CUIWindowEx(pInterface, pEngineDevice)
, m_pGaeaClient(pGaeaClient), m_dwPageNum(0)
{
}

TournamentToToWindow::~TournamentToToWindow()
{
	//Blank
}


void TournamentToToWindow::OnSetPage(DWORD dwKagNum,DWORD dwPageNum)
{
	static const std::string strClass[GLCI_NUM_ETC] = 
	{
		"CREATE_CHAR_CLASS_BRAWLER_MALE",
		"CREATE_CHAR_CLASS_SWORDMAN_MALE",
		"CREATE_CHAR_CLASS_ARCHER_FEMALE",
		"CREATE_CHAR_CLASS_SHAMAN_FEMALE",
		"CREATE_CHAR_CLASS_EXTREME_MALE",
		"CREATE_CHAR_CLASS_EXTREME_FEMALE",
		"CREATE_CHAR_CLASS_BRAWLER_FEMALE",
		"CREATE_CHAR_CLASS_SWORDMAN_FEMALE",
		"CREATE_CHAR_CLASS_ARCHER_MALE",
		"CREATE_CHAR_CLASS_SHAMAN_MALE",
		"CREATE_CHAR_CLASS_SCIENTIST_MALE",
		"CREATE_CHAR_CLASS_SCIENTIST_FEMALE",
		"CREATE_CHAR_CLASS_ASSASSIN_MALE",
		"CREATE_CHAR_CLASS_ASSASSIN_FEMALE",
		"CREATE_CHAR_CLASS_TRICKER_MALE",
		"CREATE_CHAR_CLASS_TRICKER_FEMALE",
		"CREATE_CHAR_CLASS_ETC_MALE",
		"CREATE_CHAR_CLASS_ETC_FEMALE",

	};


	size_t sel = static_cast<size_t>(dwKagNum);

	MatchingSystem::GLTouranmentClient* m_pLogic = m_pGaeaClient->GetTournamentClient();

	MatchingSystem::GLTouranmentClient::VEC_MATCHINFO_ITOR _bitor = m_pLogic->m_VecMatchingInfo.begin();
	MatchingSystem::GLTouranmentClient::VEC_MATCHINFO_ITOR _eitor = m_pLogic->m_VecMatchingInfo.end();

	MatchingSystem::GLMatchingInfo* info = NULL;

	for (; _bitor != _eitor; ++_bitor)
	{
		MatchingSystem::GLMatchingInfo* _data = *_bitor;
		if (_data->m_ScriptID == m_pLogic->m_SelectScriptID)
		{
			info = *_bitor;
			break;
		}
	}

	if(info == NULL)
		return;


	
	int OneRoundPlayerCounting = 0;

	BOOST_FOREACH( MatchingSystem::GLMatChingGroupInfo _node , info->m_GroupListInfo )
	{	
		if(_node.Position == 1)
			OneRoundPlayerCounting++;
	}


	m_dwPageNum = dwPageNum;
	m_dwKangNum = dwKagNum;

	int _tapsize = 0;

	while (true)
	{
		if (info->m_dwMatchingSize > pow(2.0,_tapsize+2) )
		{
			_tapsize++;
		}
		else
		{
			break;
		}
	}

	
	CString strMatchUp;
	strMatchUp.Format(ID2GAMEWORD("TOUR_TOTO_KANG_TEXTBOX_TEXT"), (int)pow(2.0,_tapsize+2) );
	m_pText_KANG_TEXTBOX->SetOneLineText(strMatchUp, NS_UITEXTCOLOR::WHITE);

	int Combo2index = m_pComboBox2->GetSelectIndex();
	m_pComboBox2->ClearText();
	for(int i = 0 ; i <(int)pow(2.0,(_tapsize-1)) ; i++)
	{
		CString strMatchUp;
		strMatchUp.Format(ID2GAMEWORD("TOUR_TOTO_JO_TEXTBOX_TEXT"), i+1);
		m_pComboBox2->AddText(strMatchUp.GetString());
	}
	if(Combo2index > _tapsize)
	{
		if(Combo2index < 0)
			Combo2index = 0;

		m_pComboBox2->SetSelectText(Combo2index);

	}
	else
	{
		m_pComboBox2->SetSelectText(m_pComboBox2->GetSelectIndex());
	}

	
	//CLASS_NAME_NO_SEX
	{
		CString strRATO;//%.1f
		strRATO.Format(ID2GAMEINTEXT("TOURNAMENT_BETTING_RATE"),0.0f );
		m_pText_RATE_TEXT->SetOneLineText(strRATO, NS_UITEXTCOLOR::WHITE);
	}


	MatchingSystem::GLTouranmentClient * Tourclient = m_pGaeaClient->GetTournamentClient();

	std::vector<MatchingSystem::Bettinginfo*>::iterator bet_bitor = Tourclient->m_MyBettingNormalInfo.begin();

	MatchingSystem::Bettinginfo* pMatchinginfo = NULL;

	for (; bet_bitor != Tourclient->m_MyBettingNormalInfo.end(); ++bet_bitor)
	{
		MatchingSystem::Bettinginfo* _pMatchinginfo = NULL;
		_pMatchinginfo = *bet_bitor;
		if(_pMatchinginfo->ScriptID == m_pLogic->m_SelectScriptID)
		{
			pMatchinginfo = _pMatchinginfo;
			break;
		}
	}


	for (int i = 0; i<8; i++)
	{
		MatchingSystem::GLMatChingGroupInfo* _Bettingclient =
			Tourclient->GetBettingNormalInfo(i + dwPageNum*8+1,m_pLogic->m_SelectScriptID);

		//
		if(_Bettingclient == NULL)
		{
			CString strTempText = "";
			//m_pCHR_CHR_NAME_TEXT[i+1]-ClearText();
			m_pCHR_CHR_NAME_TEXT[i+1]->SetOneLineText(strTempText,NS_UITEXTCOLOR::WHITE);
			m_pCHR_LEVTEXT[i+1]->SetOneLineText(strTempText,NS_UITEXTCOLOR::WHITE);
			m_pImg_MINI_ICON[i]->SetVisibleSingle(FALSE);
			continue;
		}

		if( _Bettingclient->GroupSize == 0 ||	// 그룹 사이즈가 0 유저가없음 
			_Bettingclient->GroupName == "" ||		// 이름이 없으면 안됨
			_Bettingclient->GroupName == "*****" || // 이름이 **** 이면 안됨
			_Bettingclient->Position != 1 )			// 그룹 Round가 1이 아니다 배팅금지상태 ) 
		{
			CString strTempText = "";
			//m_pCHR_CHR_NAME_TEXT[i+1]-ClearText();
			m_pCHR_CHR_NAME_TEXT[i+1]->SetOneLineText(strTempText,NS_UITEXTCOLOR::WHITE);
			m_pCHR_LEVTEXT[i+1]->SetOneLineText(strTempText,NS_UITEXTCOLOR::WHITE);
			m_pImg_MINI_ICON[i]->SetVisibleSingle(FALSE);
			continue;
		}

		CString strTemp;
		strTemp.Format("%s", _Bettingclient->GroupName.c_str() );
		m_pCHR_CHR_NAME_TEXT[i+1]->SetOneLineText(strTemp,NS_UITEXTCOLOR::WHITE);

		CString strTempLevel;

		if(_Bettingclient->GroupSize <= 1)
		{
			strTempLevel.Format("%s/%d%s",
				ID2GAMEWORD("CLASS_NAME_NO_SEX", (DWORD)CharClassToClassIndex((EMCHARCLASS)_Bettingclient->GroupClass)),
				_Bettingclient->CharLevel,
				ID2GAMEWORD("ITEM_ADVANCED_INFO_CONDITION") ); // 공백 삽입
		}
		else if (_Bettingclient->GroupMaxSize <= 8)
		{
			if ( _Bettingclient->ClubName == "")
			{

				strTempLevel.Format("(%d/%d)",
					_Bettingclient->GroupSize,
					_Bettingclient->GroupMaxSize ); // 공백 삽입
			}
			else
			{
				strTempLevel.Format("%s(%d/%d)",_Bettingclient->ClubName,
					_Bettingclient->GroupSize,
					_Bettingclient->GroupMaxSize ); // 공백 삽입
			}
		}
		else if (_Bettingclient->GroupMaxSize <= 40)
		{
			strTempLevel.Format("(%d/%d)",
				_Bettingclient->GroupSize,
				_Bettingclient->GroupMaxSize ); // 공백 삽입
		}

		m_pImg_MINI_ICON[i]->SetVisibleSingle(FALSE);

		m_pCHR_LEVTEXT[i+1]->SetOneLineText(strTempLevel,NS_UITEXTCOLOR::WHITE);

		if(pMatchinginfo == NULL)
		{
			
			continue;
		}
		else
		{

			if(pMatchinginfo->MyBetting.bettingGroup == _Bettingclient->dwGROUPID)
			{

				m_pImg_MINI_ICON[i]->SetVisibleSingle(TRUE);
			}
		}		
	}

	
	if(pMatchinginfo != NULL)
	{
		if(pMatchinginfo->ScriptID == m_pLogic->m_SelectScriptID)
		{
			if(pMatchinginfo->MyBetting.bettingGroup == -1)
			{
			
			}
			else if(pMatchinginfo->MyBetting.BettingCount != 0)
			{
				m_pText_GOLD_BOX_TEXT->SetVisibleSingle(TRUE);
				m_pText_WIN_BOX_TEXT->SetVisibleSingle(TRUE);
				m_pText_WINNER_TEXT_ME->SetVisibleSingle(TRUE);

				MatchingSystem::GLMatChingGroupInfo* _Bettingclient =
					Tourclient->GetGroupInfo(pMatchinginfo->MyBetting.bettingGroup,m_pLogic->m_SelectScriptID);
				

				CString strMaxText = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT(
					pMatchinginfo->MyBetting.BettingCount, 3, "," );

				m_pText_GOLD_BOX_TEXT->SetOneLineText(strMaxText.GetString(), NS_UITEXTCOLOR::WHITE);

				if(_Bettingclient != NULL)					
					m_pText_WIN_BOX_TEXT->SetOneLineText(_Bettingclient->GroupName.c_str(), NS_UITEXTCOLOR::WHITE);

				CString strRATO;//%.1f
				strRATO.Format(ID2GAMEINTEXT("TOURNAMENT_BETTING_RATE"),pMatchinginfo->MyBetting.bettingRatio );
				m_pText_RATE_TEXT->SetOneLineText(strRATO.GetString(), NS_UITEXTCOLOR::WHITE);

				CString strTemp;
				strTemp.Format(ID2GAMEINTEXT("TOURNAMENT_MY_TOTO"));
				m_pText_WINNER_TEXT_ME->SetOneLineText(strTemp, NS_UITEXTCOLOR::WHITE);
				return;
			}
		}
	}
	
	
	m_pText_GOLD_BOX_TEXT->SetVisibleSingle(FALSE);
	m_pText_GOLD_BOX_TEXT->SetOneLineText("", NS_UITEXTCOLOR::WHITE);
	m_pText_WIN_BOX_TEXT->SetVisibleSingle(FALSE);
	m_pText_WIN_BOX_TEXT->SetOneLineText("", NS_UITEXTCOLOR::WHITE);
	m_pText_WINNER_TEXT_ME->SetVisibleSingle(FALSE);
	m_pText_WINNER_TEXT_ME->SetOneLineText("", NS_UITEXTCOLOR::WHITE);



}

void TournamentToToWindow::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIWindowEx::Update(x,y,LB,MB,RB,nScroll,fElapsedTime,bFirstControl);
	UIRECT testsimple = GetGlobalPos();
	UIRECT RecSimple = GetLocalPos();


	if(m_pGaeaClient->GetTournamentClient()->GetTOTOupdate())
	{
		OnSetPage(m_dwKangNum,m_dwPageNum);

		m_pGaeaClient->GetTournamentClient()->SetTOTOupdate(false);
	}

	int newindexKan = m_pComboBox2->GetSelectIndex();
	if(m_dwPageNum != newindexKan)
	{
		m_dwPageNum = newindexKan;
		OnSetPage(m_dwKangNum,m_dwPageNum);	
	}
}

void TournamentToToWindow::CreateSubControl ( )
{
	const int nAlignLeftCenter = TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y;	
	const int nAlignRightCenterY = TEXT_ALIGN_RIGHT    | TEXT_ALIGN_CENTER_Y;	

	const int nAlignCenter	   = TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y;	


	CD3DFontPar* pFont_Shadow = m_pEngineDevice->LoadFont(_DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG);
	CD3DFontPar* pFont_Bold = m_pEngineDevice->LoadFont(_DEFAULT_FONT, 9, _DEFAULT_FONT_BOLD_FLAG);

	{
		Create					    ( TOURNAMENT_TOTO, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		CreateBaseWindowLightGray   ( "TOUR_TOTO_WINDOW", (char*)ID2GAMEWORD("TOUR_TOTO_WINDOW_TEXT", 0) );
		SetAlignFlag			    ( UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		SetVisibleSingle		    ( FALSE );
	}

	CreateLineBox("TOUR_TOTO_BACK",	"COMPETITION_OBJECT_PAGE_LINEBOX_TEXINFO");

	
	

	//~강

	CreateLineBox("TOUR_TOTO_KANG_TEXTBOX",	"GRAY_BACKGROUND_BLACK_LINE_TEXTURE");

	m_pText_KANG_TEXTBOX = CreateStaticControl("TOUR_TOTO_KANG_TEXTBOX", pFont_Shadow,0, nAlignCenter);	
	CString strMatchUp;
    strMatchUp.Format(ID2GAMEWORD("TOUR_TOTO_KANG_TEXTBOX_TEXT"), 64 );
	m_pText_KANG_TEXTBOX->SetOneLineText(strMatchUp.GetString(), NS_UITEXTCOLOR::WHITE);
	
	

	//COMPETITION_PVE_PAGE_LISTBOX_TEXINFO_W_BLACK

	


	//표시작


	CreateLineBox("TOUR_TOTO_CHR_BACK",	"COMPETITION_CTF_PAGE_INFO_LINEBOX_TEXINFO_W_GRAY");
	CreateLineBox("TOUR_TOTO_INFO_BACK",	"COMPETITION_CTF_PAGE_INFO_LINEBOX_TEXINFO_W_GRAY");

	CreateLineBox("TOUR_TOTO_UNDER_BACK",	"COLORPICKER_COLORINFO_EDIT_LINEBOX_TEXINFO");
	CreateLineBox("TOUR_TOTO_RATE_BOX",	"COMPETITION_CTF_PAGE_INFO_LINEBOX_TEXINFO_W_GRAY");

	CreateLineBox("TOUR_TOTO_UP_BACK",	"COMPETITION_OBJECT_PAGE_LINEBOX_TEXINFO");

	


	CreateLineBox("TOUR_TOTO_INFO_TITLE",	"COMPETITION_CTF_PAGE_INFO_LINEBOX_TEXINFO_W_BLACK");
	CreateLineBox("TOUR_TOTO_CHRNAME_TITLE",	"COMPETITION_CTF_PAGE_INFO_LINEBOX_TEXINFO_W_BLACK");

	// 상세 정보
	m_pText_INFO_TITLE = CreateStaticControl("TOUR_TOTO_INFO_TITLE", pFont_Shadow,0, nAlignCenter);	
	m_pText_INFO_TITLE->SetOneLineText(ID2GAMEWORD("TOUR_TOTO_INFO_TITLE_TEXT",0), NS_UITEXTCOLOR::WHITE);


	// 캐릭터(팀) 명
	m_pText_CHRNAME_TITLE = CreateStaticControl("TOUR_TOTO_CHRNAME_TITLE", pFont_Shadow,0, nAlignCenter);	
	m_pText_CHRNAME_TITLE->SetOneLineText(ID2GAMEWORD("TOUR_TOTO_CHRNAME_TITLE_TEXT",0), NS_UITEXTCOLOR::WHITE);



	for(int i =0 ; i < 8 ; i ++)
	{
		CString strTemp;
		strTemp.Format("TOUR_TOTO_SELECTBOX%d", i + 1 );

		m_pOrengeBox[i]	=	CreateLineBox(strTemp.GetString(),"TOUR_TOTO_SELECT");
		m_pOrengeBox[i]->SetVisibleSingle(FALSE);
	}
	


	

	for(int i =1 ; i < 9 ; i ++)
	{
		CString strTemp;
        strTemp.Format("TOUR_TOTO_CHR_LEVTEXT%d", i );

		//CreateLineBox(strTemp.GetString(),"GRAY_BACKGROUND_BLACK_LINE_TEXTURE");

		m_pCHR_LEVTEXT[i] = CreateStaticControl(strTemp, pFont_Shadow,0, nAlignRightCenterY);	

		CString _debug;
        _debug.Format("CHR_LEVTEXT-%d", i );
		m_pCHR_LEVTEXT[i]->SetOneLineText(_debug, NS_UITEXTCOLOR::WHITE);

	}


	for(int i =1 ; i < 9 ; i ++)
	{
		CString strTemp;
        strTemp.Format("TOUR_TOTO_CHR_NAMETEXT%d", i ); // 공백 삽입
		//CreateLineBox(strTemp.GetString(),"GRAY_BACKGROUND_BLACK_LINE_TEXTURE");
		m_pCHR_CHR_NAME_TEXT[i] = CreateStaticControl(strTemp, pFont_Shadow,0, nAlignLeftCenter);
/*
		CString _debug;
        _debug.Format("CHR_NAMETEXT-%d", i );
		m_pCHR_CHR_NAME_TEXT[i]->SetOneLineText(_debug, NS_UITEXTCOLOR::WHITE);*/
	}

	

	CreateLineBox("TOUR_TOTO_GOLD_BOX",	"DIALOGUE_WINDOW_MARKET_MONEY_BACK_LINE");

	m_pText_GOLD_BOX_TEXT = CreateStaticControl("TOUR_TOTO_GOLD_BOX", pFont_Shadow,0, nAlignLeftCenter);	
	//m_pText_GOLD_BOX_TEXT->SetOneLineText("TOUR_TOTO_GOLD_BOX_TEXT", NS_UITEXTCOLOR::WHITE);

	//CreateLineBox("TOUR_TOTO_GOLD_ICON",	"COMPETITION_OBJECT_PAGE_LINEBOX_TEXINFO");

	m_pImg_GOLD_ICON = new CUIControl(m_pEngineDevice);
	m_pImg_GOLD_ICON->CreateSub(this, "TOUR_TOTO_GOLD_ICON");
	RegisterControl(m_pImg_GOLD_ICON);

	m_pText_GOLD_TEXT = CreateStaticControl("TOUR_TOTO_GOLD_TEXT", pFont_Shadow,0, nAlignRightCenterY);	
	m_pText_GOLD_TEXT->SetOneLineText(ID2GAMEWORD("TOUR_TOTO_GOLD_BOX_TEXT",0), NS_UITEXTCOLOR::WHITE);

	m_pText_WIN_TEXT = CreateStaticControl("TOUR_TOTO_WINNER_BOX", pFont_Shadow,0, nAlignRightCenterY);	
	m_pText_WIN_TEXT->SetOneLineText(ID2GAMEINTEXT("TOURNAMENT_WINNER"), NS_UITEXTCOLOR::WHITE);

	m_pText_WIN_BOX_TEXT = CreateStaticControl("TOUR_TOTO_WINNER_TEXT", pFont_Shadow,0, nAlignLeftCenter);	
	//m_pText_WIN_BOX_TEXT->SetOneLineText("TOUR_TOTO_WINNER_BOX", NS_UITEXTCOLOR::WHITE);
	
	for(int i =0 ; i < 8 ; i ++)
	{
		CString strTemp;
        strTemp.Format("TOUR_TOTO_MINI_ICON%d", i+1 ); // 공백 삽입

		m_pImg_MINI_ICON[i] = new CUIControl(m_pEngineDevice);
		m_pImg_MINI_ICON[i]->CreateSub(this, strTemp.GetString());
		m_pImg_MINI_ICON[i]->SetVisibleSingle(FALSE);
		RegisterControl(m_pImg_MINI_ICON[i]);

		//CreateLineBox(strTemp.GetString(),"GRAY_BACKGROUND_BLACK_LINE_TEXTURE");
	}
	
	

	

	for(int i =0 ; i < 8 ; i ++)
	{
		CString strTemp;
		strTemp.Format("TOUR_TOTO_SELECTBOX%d", i+1 );

		m_pSelectHitBox[i]	=	CreateLineBox(strTemp.GetString(),"TOUR_TOTO_HITBOX",CONTROL_ID_HIT_BOX0+i);
	}


	for(int i =0 ; i < 8 ; i ++)
	{
		CString strTemp;
		strTemp.Format("TOUR_TOTO_INFO_ICON%d", i+1 ); // 공백 삽입

		RnButton::CreateArg arg;
		//////////////////////////////////////////////////////////////////////////
		arg.defaultTextureKey = "TOUR_TOTO_INFO_ICON_NORMAL";
		arg.mouseOverTextureKey = "TOUR_TOTO_INFO_ICON_OVER";
		arg.mouseClickTextureKey = "TOUR_TOTO_INFO_ICON_DOWN";
		//////////////////////////////////////////////////////////////////////////

		m_pImg_INFO_ICON[i] = new RnButton(m_pEngineDevice);
		m_pImg_INFO_ICON[i]->CreateSub(this, strTemp.GetString(), UI_FLAG_DEFAULT, CONTROL_ID_INFO_BUTTON0+i );
		m_pImg_INFO_ICON[i]->CreateSubControl(arg);
		RegisterControl(m_pImg_INFO_ICON[i]);
	}
	


	m_pCanselButton = new CBasicTextButton(m_pEngineDevice);
	m_pCanselButton->CreateSub(this, "BASIC_TEXT_BUTTON22", UI_FLAG_XSIZE, CONTROL_ID_BUTTON_CANSEL);
	m_pCanselButton->CreateBaseButton("TOUR_TOTO_CANCEL_BUTTON", CBasicTextButton::SIZE22, 
		CBasicButton::CLICK_FLIP, ID2GAMEWORD("WB_CLUB_WINDOW_STORAGE_INPUT_EDIT", 1));
	RegisterControl(m_pCanselButton);

	m_pSelectButton = new CBasicTextButton(m_pEngineDevice);
	m_pSelectButton->CreateSub(this, "BASIC_TEXT_BUTTON22", UI_FLAG_XSIZE, CONTROL_ID_BUTTON_SELECTION);
	m_pSelectButton->CreateBaseButton("TOUR_TOTO_SELECT_BUTTON", CBasicTextButton::SIZE22, 
		CBasicButton::CLICK_FLIP, ID2GAMEWORD("TOURNAMENT_TOTO_SELECT", 0));
	RegisterControl(m_pSelectButton);


	CString strRATO;
	strRATO.Format(ID2GAMEINTEXT("TOURNAMENT_BETTING_RATE"),0 ); // 공백 삽입



	m_pText_RATE_TEXT = CreateStaticControl("TOUR_TOTO_RATE_TEXT", pFont_Shadow,0, nAlignLeftCenter);	
	m_pText_RATE_TEXT->SetOneLineText(strRATO.GetString(), NS_UITEXTCOLOR::WHITE);

	

	m_pText_WINNER_TEXT = CreateStaticControl("TOUR_TOTO_WINNER_TEXT", pFont_Bold,0, nAlignLeftCenter);	
	//m_pText_WINNER_TEXT->SetOneLineText("WINNER_TEXT", NS_UITEXTCOLOR::WHITE);

	m_pText_WINNER_TEXT_ME = CreateStaticControl("TOUR_TOTO_WINNER_TEXT_ME", pFont_Shadow,0, nAlignLeftCenter);	
	//m_pText_RATE_TEXT->SetOneLineText("WINNER_TEXT_ME", NS_UITEXTCOLOR::WHITE);

	/************/

	//조 선택하는 콤보 박스!
	m_pComboBox2 = new TournamentComboBox(m_pGaeaClient, m_pInterface, m_pEngineDevice);
	m_pComboBox2->CreateSub(this,"TOUR_TOTO_JO_COMBOBOX",UI_FLAG_XSIZE,CONTROL_ID_ENTRY_COMBO_BOX);
	m_pComboBox2->CreateSubControl();
	RegisterControl( m_pComboBox2 );
	
	

	m_pCloseButton = new CBasicTextButton(m_pEngineDevice);
	m_pCloseButton->CreateSub(this, "BASIC_TEXT_BUTTON22", UI_FLAG_XSIZE, CONTROL_ID_BUTTON_EXIT);
	m_pCloseButton->CreateBaseButton("TOUR_TOTO_CLOSE_BUTTON", CBasicTextButton::SIZE22, 
		CBasicButton::CLICK_FLIP, ID2GAMEWORD("SELECT_CLUB_WINDOW_CLOSE_BTN", 0));
	RegisterControl(m_pCloseButton);

	SetAlignFlag( UI_FLAG_CENTER_Y | UI_FLAG_CENTER_X );
	m_pInterface->UiRegisterControl(this,true);

}

void TournamentToToWindow::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage(ControlID,dwMsg);
	switch( ControlID )
	{
	case CONTROL_ID_BUTTON_SELECTION:
		if( CHECK_MOUSE_IN( dwMsg ) && CHECK_LB_UP_LIKE( dwMsg ) )
		{
			int Selection = 0;
			for(int i =0 ; i < 8 ; i ++)
			{
				if(m_pOrengeBox[i]->IsVisible())
				{
					Selection = i;
					break;
				}
			}
			MatchingSystem::GLTouranmentClient * Tourclient = m_pGaeaClient->GetTournamentClient();

			MatchingSystem::GLMatChingGroupInfo* __Bettingclient =
				Tourclient->GetBettingNormalInfo(Selection + m_dwPageNum*8 +1,Tourclient->m_SelectScriptID);

			if(__Bettingclient == NULL)
				break;

			if(__Bettingclient->dwGROUPID == -1)
				return;

			Tourclient->m_bettingGroup = __Bettingclient->dwGROUPID;
			Tourclient->m_LeageId = __Bettingclient->ScriptID;
			
			std::vector<MatchingSystem::Bettinginfo*>::iterator _bitor = Tourclient->m_MyBettingNormalInfo.begin();

			MatchingSystem::Bettinginfo* pMatchinginfo = NULL;

			for (; _bitor != Tourclient->m_MyBettingNormalInfo.end(); _bitor++)
			{
				MatchingSystem::Bettinginfo* _pMatchinginfo= NULL;
				_pMatchinginfo = *_bitor;
				if(_pMatchinginfo->ScriptID == __Bettingclient->ScriptID)
				{
					pMatchinginfo = _pMatchinginfo;
					break;
				}
			}

			CString strRATO;
			if(pMatchinginfo != NULL)
			{
				CString strMinText = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT(
					pMatchinginfo->MinimumBettingSize, 3, "," );
				CString strMaxText = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT(
					pMatchinginfo->MaximumBettingSize, 3, "," );

				strRATO.Format(ID2GAMEINTEXT("TOURNAMENT_BETTING_MESSAGEBOX"),__Bettingclient->GroupName.c_str(),
					strMinText,strMaxText); // 공백 삽입
			}
			else
			{
				strRATO.Format(ID2GAMEINTEXT("TOURNAMENT_BETTING_MESSAGEBOX"),__Bettingclient->GroupName.c_str(),
					"0","0"); // 공백 삽입
			}
			


			std::string MessageBoxName(strRATO.GetString());
			m_pInterface->DoModal(
				MessageBoxName,
				UI::MODAL_INPUT,
				UI::EDITBOX_MONEY,
				UI::MODAL_BETTINGMONEY );
			break;
		}
	case CONTROL_ID_BUTTON_CANSEL:
		if( CHECK_MOUSE_IN( dwMsg ) && CHECK_LB_UP_LIKE( dwMsg ) )
		{
			int Selection = 0;
			for(int i =0 ; i < 8 ; i ++)
			{
				if(m_pOrengeBox[i]->IsVisible())
				{
					Selection = i;
					break;
				}
			}
			MatchingSystem::GLTouranmentClient * Tourclient = m_pGaeaClient->GetTournamentClient();

			MatchingSystem::GLMatChingGroupInfo* __Bettingclient =
				Tourclient->GetBettingNormalInfo( Selection + m_dwPageNum*8 +1,Tourclient->m_SelectScriptID);

			if( __Bettingclient == NULL )
				break;
			Tourclient->m_bettingGroup = __Bettingclient->dwGROUPID;
			Tourclient->m_LeageId = __Bettingclient->ScriptID;

			std::vector<MatchingSystem::Bettinginfo*>::iterator _bitor = Tourclient->m_MyBettingNormalInfo.begin();

			MatchingSystem::Bettinginfo* pMatchinginfo = NULL;

			for (; _bitor != Tourclient->m_MyBettingNormalInfo.end(); _bitor++)
			{
				MatchingSystem::Bettinginfo* _pMatchinginfo= NULL;
				_pMatchinginfo = *_bitor;
				if(_pMatchinginfo->ScriptID == __Bettingclient->ScriptID)
				{
					pMatchinginfo = _pMatchinginfo;
					break;
				}
			}

			if(pMatchinginfo != NULL)
			{
				Tourclient->CanselBetting(__Bettingclient->ScriptID);
			}

			break;
		}
		break;
	case CONTROL_ID_HIT_BOX0:
	case CONTROL_ID_HIT_BOX1:
	case CONTROL_ID_HIT_BOX2:
	case CONTROL_ID_HIT_BOX3:
	case CONTROL_ID_HIT_BOX4:
	case CONTROL_ID_HIT_BOX5:
	case CONTROL_ID_HIT_BOX6:
	case CONTROL_ID_HIT_BOX7:
	case CONTROL_ID_HIT_BOX8:
		if( CHECK_MOUSE_IN( dwMsg ) && CHECK_LB_UP_LIKE( dwMsg ) )
		{
			int _index = ControlID - CONTROL_ID_HIT_BOX0;
			if(_index > 8 || _index < 0)
				return;
			for(int i =0 ; i < 8 ; i ++)
			{
				m_pOrengeBox[i]->SetVisibleSingle(FALSE);
			}
			m_pOrengeBox[_index]->SetVisibleSingle(TRUE);

			MatchingSystem::GLTouranmentClient * Tourclient = m_pGaeaClient->GetTournamentClient();

			MatchingSystem::GLMatChingGroupInfo* _Bettingclient =
				Tourclient->GetBettingNormalInfo( _index + m_dwPageNum*8 +1,Tourclient->m_SelectScriptID);

			if(_Bettingclient == NULL)
				return;			

			return;
		}
		break;
	case CONTROL_ID_INFO_BUTTON0:
	case CONTROL_ID_INFO_BUTTON1:
	case CONTROL_ID_INFO_BUTTON2:
	case CONTROL_ID_INFO_BUTTON3:
	case CONTROL_ID_INFO_BUTTON4:
	case CONTROL_ID_INFO_BUTTON5:
	case CONTROL_ID_INFO_BUTTON6:
	case CONTROL_ID_INFO_BUTTON7:
	case CONTROL_ID_INFO_BUTTON8:
		if( CHECK_MOUSE_IN( dwMsg ) && CHECK_LB_UP_LIKE( dwMsg ) )
		{
			int _index = ControlID - CONTROL_ID_INFO_BUTTON0;

			if(_index > 8 || _index < 0)
				return;

			MatchingSystem::GLTouranmentClient * Tourclient = m_pGaeaClient->GetTournamentClient();

			MatchingSystem::GLMatChingGroupInfo* _Bettingclient =
				Tourclient->GetBettingNormalInfo( _index + m_dwPageNum*8 + 1,Tourclient->m_SelectScriptID);
			if(_Bettingclient == NULL)
				return;

			if(_Bettingclient->dwGROUPID == -1)
				return;


			Tourclient->GetBettingInfoEployee(_Bettingclient->dwGROUPID);
			if( _Bettingclient->GroupMaxSize == 1)
				m_pInterface->ReferCharWindowOpen(_Bettingclient->GroupName);
			else
				m_pInterface->OpenTournamentMoreInfoWindow(_Bettingclient->dwGROUPID);
			
			return;
		}
		break;
	case CONTROL_ID_BUTTON_EXIT:	
		{
			if( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				m_pInterface->UiHideGroup( GetWndID() );
			}
		}
		break;
	}
}
//----------------------------------------------------------------------------------------------------//

MyTournamentToToWindow::MyTournamentToToWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: TournamentToToWindow( pGaeaClient, pInterface, pEngineDevice )
{
}

void MyTournamentToToWindow::OnCreate()
{ 
	CreateSubControl();
}

void MyTournamentToToWindow::OnVisible(bool visible)
{
	MatchingSystem::GLTouranmentClient* m_pLogic = m_pGaeaClient->GetTournamentClient();
	m_pLogic->GetBettingGroupInfo();

	UIRECT rcPos   = TournamentToToWindow::m_pInterface->GetTournamentDashBoardWindowPos(TOURNAMENT_DASHBOARD);
	UIRECT rcMyPos = TournamentToToWindow::GetGlobalPos();

	rcPos = UIRECT(rcPos.right, rcPos.top, rcMyPos.sizeX, rcMyPos.sizeY);

	TournamentToToWindow::SetGlobalPos( rcPos );

	TournamentToToWindow::m_pInterface->UiShowGroupFocus( TOURNAMENT_TOTO );
}

const UIRECT& MyTournamentToToWindow::GetGlobalPos()
{
	return TournamentToToWindow::GetGlobalPos();
}

void MyTournamentToToWindow::SetPage(DWORD dwKagNum,DWORD dwPageNum)
{
	TournamentToToWindow::OnSetPage(dwKagNum,dwPageNum);
}
