#include "StdAfx.h"

#include "RnMapSearchTap.h"



#include "../EngineLib/GUInterface/BasicTextBox.h"


#include "../Util/RnButton.h"
#include "../Util/CheckBox.h"
#include "../Util/RnEditBox.h"

#include "../../../EngineLib/DxTools/DxFontMan.h"


#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"


#include "../../InnerInterface.h"


#include "../../../RanLogic/Msg/MapMsg.h"


#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/ReferChar/ReferChar.h"
#include "../../../RanLogic/GLUseFeatures.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"

#include "RnMapHelper.h"
#include "../../../RanLogic/Character/GLCharDefine.h"
#include "../EngineLib/GUInterface/GameTextControl.h"
// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace
{
	const char* ID2GAMEWORD_SEARCH = "검색";

	const char* ID2GAMEWORD_ONLY_CURRENT_REGION = "현재 표시 지역에서만";

	const char* ID2GAMEWORD_SEARCH_STUDENT = "학원생 검색";
	const char* ID2GAMEWORD_SEARCH_STUDENT_INFO = "검색 대상 정보";


	const char* ID2GAMEWORD_SHOW_MAP_POSITION = "지도에 표시";
	const char* ID2GAMEWORD_WHISPER = "귓속말";
	const char* ID2GAMEWORD_REGISTER_FRIEND = "친구등록";
	const char* ID2GAMEWORD_CHARACTER_VIEW = "정보보기";
	const char* ID2GAMEWORD_PARTY = "파티";
	const char* ID2GAMEWORD_INVITE_CLUB = "클럽초대";


	const char* ID2GAMEWORD_SHORT_SEARCH_EXPLAIN = "검색하기에 문장이 너무 짧습니다.";

	const char* ID2GAMEWORD_HAVE_PARTY = "파티중";
	const char* ID2GAMEWORD_HAVE_NO_PARTY = "파티 없음";


	const char* ID2GAMEWORD_NAME   = "이름";
	const char* ID2GAMEWORD_SCHOOL = "학원";
	const char* ID2GAMEWORD_BADGE  = "뱃지";
	const char* ID2GAMEWORD_CLASS  = "부서";
	const char* ID2GAMEWORD_CLUB   = "클럽";


}






RnMapSearchTap::RnMapSearchTap( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice,
							   RnMapPrivate::IMapInterface* pMapInterface) :
	CUIGroupHelper(pEngineDevice)
{
	m_pGaeaClient = pGaeaClient;
	m_pInterface = pInterface;

	m_pMapInterface = pMapInterface;

	m_dwDetailInfoGaeaID = NATIVEID_NULL().Id();

	ID2GAMEWORD_SEARCH = ID2GAMEWORD("RN_MAP_WINDOW_SEARCH_TYPE", 0);

	ID2GAMEWORD_ONLY_CURRENT_REGION = ID2GAMEWORD("RN_MAP_WINDOW_SEARCH_TYPE", 1);

	ID2GAMEWORD_SEARCH_STUDENT = ID2GAMEWORD("RN_MAP_WINDOW_SEARCH_TYPE", 2);
	ID2GAMEWORD_SEARCH_STUDENT_INFO = ID2GAMEWORD("RN_MAP_WINDOW_SEARCH_TYPE", 3);


	ID2GAMEWORD_SHOW_MAP_POSITION = ID2GAMEWORD("RN_MAP_WINDOW_SEARCH_TYPE", 4);
	ID2GAMEWORD_WHISPER = ID2GAMEWORD("RN_MAP_WINDOW_SEARCH_TYPE", 5);
	ID2GAMEWORD_REGISTER_FRIEND = ID2GAMEWORD("RN_MAP_WINDOW_SEARCH_TYPE", 6);
	ID2GAMEWORD_CHARACTER_VIEW = ID2GAMEWORD("RN_MAP_WINDOW_SEARCH_TYPE", 7);
	ID2GAMEWORD_PARTY = ID2GAMEWORD("RN_MAP_WINDOW_SEARCH_TYPE", 8);
	ID2GAMEWORD_INVITE_CLUB = ID2GAMEWORD("RN_MAP_WINDOW_SEARCH_TYPE", 9);


	ID2GAMEWORD_SHORT_SEARCH_EXPLAIN = ID2GAMEWORD("RN_MAP_WINDOW_SEARCH_TYPE", 10);

	ID2GAMEWORD_HAVE_PARTY = ID2GAMEWORD("RN_MAP_WINDOW_SEARCH_TYPE", 11);
	ID2GAMEWORD_HAVE_NO_PARTY = ID2GAMEWORD("RN_MAP_WINDOW_SEARCH_TYPE", 12);


	ID2GAMEWORD_NAME   = ID2GAMEWORD("RN_MAP_WINDOW_SEARCH_TYPE", 13);
	ID2GAMEWORD_SCHOOL = ID2GAMEWORD("RN_MAP_WINDOW_SEARCH_TYPE", 14);
	ID2GAMEWORD_BADGE  = ID2GAMEWORD("RN_MAP_WINDOW_SEARCH_TYPE", 15);
	ID2GAMEWORD_CLASS  = ID2GAMEWORD("RN_MAP_WINDOW_SEARCH_TYPE", 16);
	ID2GAMEWORD_CLUB   = ID2GAMEWORD("RN_MAP_WINDOW_SEARCH_TYPE", 17);

}

void RnMapSearchTap::CreateSubControl()
{
	CD3DFontPar* pFont10 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 10, _DEFAULT_FONT_FLAG );

	CreateLineBox("MAP_WINDOW_SEARCH_TAP_REGION","MAP_WINDOW_LINEBOX_TEXTUREINFO");

	CUIControl* pBox = NULL;

	pBox = CreateLineBox("MAP_WINDOW_SEARCH_TAP_STUDENT_SEARCH_REGION","MAP_WINDOW_LINEBOX2_TEXTUREINFO");
	pBox->SetDiffuse(D3DCOLOR_XRGB( BYTE(111 * 1.7),BYTE(127*1.7),255) );

	pBox = CreateLineBox("MAP_WINDOW_SEARCH_TAP_STUDENT_INFO_REGION","MAP_WINDOW_LINEBOX2_TEXTUREINFO");
	pBox->SetDiffuse(D3DCOLOR_XRGB( BYTE(111 * 1.7),BYTE(127*1.7),255) );

	CreateText(ID2GAMEWORD("MAP_SEARCH_STUDENT"),"MAP_WINDOW_SEARCH_TAP_STUDENT_SEARCH_TEXT",TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP);


	CreateLineBox("MAP_WINDOW_SEARCH_TAP_SEARCH_BACK_LINE","MAP_WINDOW_SEARCH_TAP_SEARCH_BACK_LINE_TEXTUREINFO");

	CreateText(ID2GAMEWORD("MAP_SEARCH_STUDENT_INFO"),"MAP_WINDOW_SEARCH_TAP_STUDENT_SEARCH_INFO_TEXT",TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP);

	RnEditBox* pEditBox = new RnEditBox(m_pEngineDevice);
	pEditBox->Create(this,"MAP_WINDOW_SEARCH_TAP_EDIT_MAN","MAP_WINDOW_SEARCH_TAP_EDIT","MAP_WINDOW_SEARCH_TAP_CARRAT",
		SEARCH_EDITBOX,true,D3DCOLOR_XRGB(255,255,255),pFont10,CHR_ID_LENGTH);
	RegisterControl(pEditBox);
	m_pEditBox = pEditBox;


	RnButton* pStatBtn = NULL;
	RnButton::CreateArg arg;
	//////////////////////////////////////////////////////////////////////////
	arg.defaultTextureKey = "CHARACTER_WINDOW_STAT_PLUS_MINUS_BUTTON_NORMAL_TEXTUREINFO";
	arg.mouseOverTextureKey = "CHARACTER_WINDOW_STAT_PLUS_MINUS_BUTTON_MOUSEOVER_TEXTUREINFO";
	arg.mouseClickTextureKey = "CHARACTER_WINDOW_STAT_PLUS_MINUS_BUTTON_MOUSECLICK_TEXTUREINFO";
	//	arg.textKey = "CHARACTER_WINDOW_STAT_BUTTON_TEXT_POS";
	arg.pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 10, _DEFAULT_FONT_SHADOW_FLAG );

	//////////////////////////////////////////////////////////////////////////
	arg.text = ID2GAMEWORD("MAP_SEARCH");

	RnButton* pSearchBtn = new RnButton(m_pEngineDevice);
	pSearchBtn->CreateSub(this,"MAP_WINDOW_SEARCH_TAP_SEARCH_BUTTON",UI_FLAG_DEFAULT,SEARCH_BUTTON);
	pSearchBtn->CreateSubControl(arg);
	RegisterControl(pSearchBtn);
//	m_StatApplyButton = pSearchBtn;

	CreateControl("MAP_WINDOW_SEARCH_TAP_LINK");

	m_pCurrentRegionCheckBox = CreateCheckBox("MAP_WINDOW_SEARCH_TAP_LINK_CHECKBOX",CURRENT_REGION_CHECKBOX,"MAP_WINDOW_CHECKBOX_ON","MAP_WINDOW_CHECKBOX_OFF",false);

	CreateText(ID2GAMEWORD("MAP_ONLY_CURRENT_REGION"),"MAP_WINDOW_SEARCH_TAP_LINK_TEXT",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);
	
	//////////////////////////////////////////////////////////////////////////
	arg.text = "▼";
 
	RnButton* pCacheBtn = new RnButton(m_pEngineDevice);
	pCacheBtn->CreateSub(this,"MAP_WINDOW_SEARCH_TAP_CACHE_BUTTON",UI_FLAG_DEFAULT,CACHE_BUTTON);
	pCacheBtn->CreateSubControl(arg);
	RegisterControl(pCacheBtn);

	int testSize = SEARCH_NAME_RECT_MAX - SEARCH_NAME_RECT1;
	for (int i = 0; i < MAX_SEARCH_NAME; ++i)
	{
		std::string key;
		key = sc::string::format("MAP_WINDOW_SEARCH_TAP_SEARCH_NAME%1%",i+1);
		m_pSearchName[i] = CreateText(key,key,TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);

		key = sc::string::format("MAP_WINDOW_SEARCH_TAP_SEARCH_NAME_MOUSEOVER_RECT%1%",i+1);
		
		m_pSearchNameRect[i] = CreateOverRect(key,SEARCH_NAME_RECT1+i);
	}

	CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
	pScrollBar->CreateSub( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, SEARCH_NAME_LIST_SCROLLBAR );
	pScrollBar->CreateBaseScrollBar( "MAP_WINDOW_SEARCH_TAP_SEARCH_NAME_LIST_SCROLL" );
	RegisterControl ( pScrollBar );
	m_pNameListScrollBar = pScrollBar;

	UpdateNameList();
	//////////////////////////////////////////////////////////////////////////

	m_pInfoSchool = CreateText(std::string(ID2GAMEWORD("MAP_SCHOOL")) + " :","MAP_WINDOW_SEARCH_TAP_INFO_SCHOOL",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);
	m_pInfoName  = CreateText(std::string(ID2GAMEWORD("MAP_NAME")) + " :","MAP_WINDOW_SEARCH_TAP_INFO_NAME",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);
	m_pInfoBadge = CreateText(std::string(ID2GAMEWORD("MAP_BADGE")) + " :","MAP_WINDOW_SEARCH_TAP_INFO_BADGE",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);
	m_pInfoClass = CreateText(std::string(ID2GAMEWORD("MAP_CLASS")) + " :","MAP_WINDOW_SEARCH_TAP_INFO_TYPE",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);
	m_pInfoParty = CreateText(std::string(ID2GAMEWORD("MAP_PARTY")) + " :","MAP_WINDOW_SEARCH_TAP_INFO_PARTY",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);
	m_pInfoClub = CreateText(std::string(ID2GAMEWORD("MAP_CLUB")) + " :","MAP_WINDOW_SEARCH_TAP_INFO_CLUB",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);

	

	arg.text = ID2GAMEWORD("MAP_SHOW_MAP_POSITION");
	RnButton* pShowMapPosBtn = new RnButton(m_pEngineDevice);
	pShowMapPosBtn->CreateSub(this,"MAP_WINDOW_SEARCH_TAP_MAP_POSITION_BUTTON",UI_FLAG_DEFAULT,SHOW_MAP_POSITION_BUTTON);
	pShowMapPosBtn->CreateSubControl(arg);
	RegisterControl(pShowMapPosBtn);

	arg.text = ID2GAMEWORD("MAP_WHISPER");
	RnButton* pWhisperBtn = new RnButton(m_pEngineDevice);
	pWhisperBtn->CreateSub(this,"MAP_WINDOW_SEARCH_TAP_WHISPER_BUTTON",UI_FLAG_DEFAULT,WHISPER_BUTTON);
	pWhisperBtn->CreateSubControl(arg);
	RegisterControl(pWhisperBtn);

	arg.text = ID2GAMEWORD("MAP_REGISTER_FRIEND");
	RnButton* pRegisterFriendBtn = new RnButton(m_pEngineDevice);
	pRegisterFriendBtn->CreateSub(this,"MAP_WINDOW_SEARCH_TAP_REGISTER_FRIEND_BUTTON",UI_FLAG_DEFAULT,REGISTER_FRIEND_BUTTON);
	pRegisterFriendBtn->CreateSubControl(arg);
	RegisterControl(pRegisterFriendBtn);

	arg.text = ID2GAMEWORD("MAP_CHARACTER_VIEW");
	RnButton* pCharacterViewBtn = new RnButton(m_pEngineDevice);
	pCharacterViewBtn->CreateSub(this,"MAP_WINDOW_SEARCH_TAP_CHARACTER_VIEW_BUTTON",UI_FLAG_DEFAULT,CHARACTER_VIEW_BUTTON);
	pCharacterViewBtn->CreateSubControl(arg);
	RegisterControl(pCharacterViewBtn);

	arg.text = ID2GAMEWORD("MAP_PARTY");
	RnButton* pPartyBtn = new RnButton(m_pEngineDevice);
	pPartyBtn->CreateSub(this,"MAP_WINDOW_SEARCH_TAP_PARTY_BUTTON",UI_FLAG_DEFAULT,PARTY_BUTTON);
	pPartyBtn->CreateSubControl(arg);
	RegisterControl(pPartyBtn);

	arg.text = ID2GAMEWORD("MAP_INVITE_CLUB");
	RnButton* pClubBtn = new RnButton(m_pEngineDevice);
	pClubBtn->CreateSub(this,"MAP_WINDOW_SEARCH_TAP_INVITE_CLUB_BUTTON",UI_FLAG_DEFAULT,INVITE_CLUB_BUTTON);
	pClubBtn->CreateSubControl(arg);
	RegisterControl(pClubBtn);


}

void RnMapSearchTap::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{

	switch(ControlID)
	{
	case SEARCH_EDITBOX:
		{
			if(dwMsg & UIMSG_RNEDITBOX_RETURN)
			{
				SendSearchMsg();
			}
			break;
		}
	case SEARCH_BUTTON:
		{
			if( dwMsg & UIMSG_RNBUTTON_CLICK)
			{
				SendSearchMsg();
			}
			break;
		}
	case SHOW_MAP_POSITION_BUTTON:
		{
			if( dwMsg & UIMSG_RNBUTTON_CLICK)
			{
				m_SearchCharacerLevel = 0;
				m_SearchCharacerName.clear();
				m_SearchCharacerMapID = SNATIVEID(false).Id();
				m_SearchCharacerPosition = D3DXVECTOR3(0,0,0);
				
				std::string name = GetDetailInfoName();
				if(!name.empty())
				{
					GLMSG::NET_MAP_SEARCH_NAME_POSITION_REQ_CA msg;
					strncpy_s(msg.m_szChaName,sizeof(msg.m_szChaName),name.c_str(),name.size());
					m_pGaeaClient->NETSENDTOAGENT(&msg);
				}
			}
			break;
		}

	case WHISPER_BUTTON:
		{
			if( dwMsg & UIMSG_RNBUTTON_CLICK)
			{
				std::string name = GetDetailInfoName();
				IBasicChatWindow* pChatWindow = m_pInterface->GetBasicChatWindow();
				if(!name.empty())
				{
					pChatWindow->BeginPrivateChat(name);
				}
			}
			break;
		}
	case CHARACTER_VIEW_BUTTON:
		{
			if( dwMsg & UIMSG_RNBUTTON_CLICK)
			{
				std::string name = GetDetailInfoName();
				if(!name.empty())
				{
// 					m_pInterface->UiShowGroupFocus( REFERCHAR_WINDOW );
// 
// 					ReferChar::CClient* pReferChar = m_pGaeaClient->GetReferChar();
// 					if( pReferChar )
// 						pReferChar->Request( m_pGaeaClient->GetConnServerGroup(), name );
                    m_pInterface->ReferCharWindowOpen( name );
				}
			}
			break;
		}
	case REGISTER_FRIEND_BUTTON:
		{
			if( dwMsg & UIMSG_RNBUTTON_CLICK)
			{
				std::string name = GetDetailInfoName();
				if(!name.empty())
				{
					m_pInterface->ADD_FRIEND(name.c_str());
				}
			}
			break;
		}
	case PARTY_BUTTON:
		{
			if( dwMsg & UIMSG_RNBUTTON_CLICK)
			{
				std::string name = GetDetailInfoName();
				if(!name.empty() && m_dwDetailInfoGaeaID != NATIVEID_NULL().Id())
				{
					m_pInterface->GetPtoPWindow()->SetPlayerID(m_dwDetailInfoGaeaID);
					std::string strTemp = sc::string::format( ID2GAMEINTEXT("PARTY_QUESTION"),name );
					m_pInterface->DoPartyModal( strTemp, UI::MODAL_PARTY_REQ, EMPARTY_MODE_PARTY );
				}
			}
			break;
		}
	case INVITE_CLUB_BUTTON:
		{
			if( dwMsg & UIMSG_RNBUTTON_CLICK)
			{
				std::string name = GetDetailInfoName();
				if(!name.empty() && m_dwDetailInfoGaeaID != NATIVEID_NULL().Id())
				{
					m_pInterface->GetPtoPWindow()->SetPlayerID(m_dwDetailInfoGaeaID);
					std::string strCombine = sc::string::format( ID2GAMEINTEXT("CLUB_JOIN"), name );
					m_pInterface->DoModal( strCombine, UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_CLUB_JOIN );	
				}
			}
			break;
		}
	default:
		{
			if( ControlID >= SEARCH_NAME_RECT1 && ControlID < SEARCH_NAME_RECT_MAX)
			{
				int index = ControlID - SEARCH_NAME_RECT1;
				m_pSearchNameRect[index]->SetUseRender(CHECK_MOUSE_IN( dwMsg )); 

				if( m_pSearchNameRect[index]->IsUseRender() )
				{
					if( CHECK_LB_UP_LIKE(dwMsg) )
					{
						int NameListScrollPos = m_pNameListScrollBar->GetThumbFrame()->GetPosition();

						std::string selectName = m_NameList[ index + NameListScrollPos ].second;

						GLMSG::NET_MAP_SEARCH_NAME_DETAIL_INFO_REQ_CA msg;
						strncpy_s(msg.m_szChaName,sizeof(msg.m_szChaName),selectName.c_str(),selectName.size());
						m_pGaeaClient->NETSENDTOAGENT(&msg);
					}
				}
			}

			break;
		}
	}


}

void RnMapSearchTap::SendSearchMsg()
{
	std::string searchText = m_pEditBox->GetEditString();
	if( searchText.empty() )
		return;

	if( searchText.size() < 3 )
	{
		m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::RED, ID2GAMEWORD("MAP_SHORT_SEARCH_EXPLAIN"));
		return;
	}
	
	m_NameList.clear();
	UpdateNameList();

	//m_pEditBox->EndEdit();
	m_pEditBox->SetEditString(std::string(""));
	//m_pEditBox->BeginEdit();	

	DWORD mapID = SNATIVEID(false).Id();
	if( m_pCurrentRegionCheckBox->IsChecked() )
		mapID = m_pMapInterface->GetSelectMap();

	//////////////////////////////////////////////////////////////////////////
	GASSERT(searchText.size() < CHR_ID_LENGTH);
	GLMSG::NET_MAP_SEARCH_NAME_REQ_CF MsgCA;
	MsgCA.SetName(searchText);
	MsgCA.m_mapID = mapID;
 	//m_pGaeaClient->NETSENDTOAGENT(&MsgCA); // 기존 에이젼트로 보내는 부분을 먼저 필드서버에서 검색 허용 여부를 체크한뒤 에이젼트로 전송
	m_pGaeaClient->NETSENDTOFIELD(&MsgCA);
	//////////////////////////////////////////////////////////////////////////

}

void RnMapSearchTap::MsgMapSearchNameList( const std::vector<std::pair<int, std::string> >& nameList )
{
	m_NameList = nameList;
	UpdateNameList();
}

void RnMapSearchTap::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroupHelper::Update(x,y,LB,MB,RB,nScroll,fElapsedTime,bFirstControl);

	UpdateNameList();
}

void RnMapSearchTap::MsgMapSearchNameDetailInfo(int level,const std::string& charName,const std::string& title,int school,int emClass,
								bool isParty,const std::string& clubName,DWORD dwGaeaID)
{
	m_dwDetailInfoGaeaID = dwGaeaID;
	m_pInfoSchool->SetText(sc::string::format("%1% : %2%",ID2GAMEWORD("MAP_SCHOOL"),ID2GAMEWORD("ACADEMY_NAME",school)).c_str());
	m_pInfoName->SetText(sc::string::format("%1% : %2%. %3%",ID2GAMEWORD("MAP_NAME"),level,charName).c_str());

	std::string gameTitle;
	if( !title.empty() )
		gameTitle = ID2GAMEINTEXT(title.c_str());
	m_pInfoBadge->SetText(sc::string::format("%1% : %2%",ID2GAMEWORD("MAP_BADGE"),gameTitle).c_str());
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		m_pInfoClass->SetText(sc::string::format("%1% : %2%",ID2GAMEWORD("MAP_CLASS"),CGameTextMan::GetInstance().GetCommentText("CHARCLASS", CharClassToIndex((EMCHARCLASS)emClass)).GetString()).c_str());
	else
		m_pInfoClass->SetText(sc::string::format("%1% : %2%",ID2GAMEWORD("MAP_CLASS"),COMMENT::CHARCLASS[CharClassToIndex((EMCHARCLASS)emClass)]).c_str());
	m_pInfoParty->SetText(sc::string::format("%1% : %2%",ID2GAMEWORD("MAP_PARTY"),isParty ? ID2GAMEWORD("MAP_HAVE_PARTY") : ID2GAMEWORD("MAP_HAVE_NO_PARTY")).c_str());
	m_pInfoClub->SetText(sc::string::format("%1% : %2%",ID2GAMEWORD("MAP_CLUB"),clubName).c_str());
}

void RnMapSearchTap::MsgMapSearchNamePosition(int level,const std::string& charName,DWORD mapID,const D3DXVECTOR3& pos)
{
	m_SearchCharacerLevel = level;
	m_SearchCharacerName = charName;
	m_SearchCharacerMapID = mapID;
	m_SearchCharacerPosition = pos;

	m_pMapInterface->SetMapLevelFile(mapID);
}

void RnMapSearchTap::UpdateNameList()
{
	m_pNameListScrollBar->GetThumbFrame()->SetState( (int)m_NameList.size(), MAX_SEARCH_NAME );
	int NameListScrollPos = m_pNameListScrollBar->GetThumbFrame()->GetPosition();

	for (int i = 0; i < MAX_SEARCH_NAME; ++i)
	{
		m_pSearchName[i]->SetVisibleSingle(FALSE);
		m_pSearchNameRect[i]->SetVisibleSingle(FALSE);
	}

	size_t nSize = m_NameList.size() < MAX_SEARCH_NAME ? m_NameList.size() : MAX_SEARCH_NAME;

	for (size_t i=0; i < nSize; ++i)
	{
		m_pSearchName[i]->SetVisibleSingle(TRUE);
		m_pSearchNameRect[i]->SetVisibleSingle(TRUE);
		m_pSearchName[i]->SetText(	m_NameList[i + NameListScrollPos].second.c_str() );
	}
	

}

bool RnMapSearchTap::GetSearchedCharacter( int* pLevel,std::string* pName,DWORD* pMapID,D3DXVECTOR3* pPos )
{
	if( m_SearchCharacerName.empty() )
		return false;

	*pLevel = m_SearchCharacerLevel;
	*pName  = m_SearchCharacerName;
	*pMapID = m_SearchCharacerMapID;
	*pPos   = m_SearchCharacerPosition;

	return true;
}

std::string RnMapSearchTap::GetDetailInfoName()
{
	std::string levName = m_pInfoName->GetText(0).GetString();
	size_t pos = levName.find(".");
	if( pos == std::string::npos)
		return "";

	return levName.substr(pos + 2);
}







