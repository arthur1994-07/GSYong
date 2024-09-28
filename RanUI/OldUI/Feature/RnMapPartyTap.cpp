#include "StdAfx.h"

#include "RnMapPartyTap.h"

#include "../EngineLib/GUInterface/BasicTextBox.h"

#include "../EngineLib/GUInterface/GameTextControl.h"


#include "../../../EngineLib/DxTools/DxFontMan.h"

#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"

#include "../../../RanLogicClient/GLGaeaClient.h"


#include "../Util/RnButton.h"
#include "../Util/RnEditBox.h"


#include "../../../RanLogic/Msg/MapMsg.h"


#include "../../InnerInterface.h"

#include "RnMapHelper.h"



#include "../../../RanLogic/Msg/MapMsg.h"


#include "../../../EngineLib/GUInterface/GameTextControl.h"
// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace
{
	const char* ID2GAMEWORD_PARTY_INFO = "파티 정보";
	const char* ID2GAMEWORD_PARTY_REGISTER= "파티모집 등록";
	const char* ID2GAMEWORD_PARTY_UNREGISTER= "파티모집 해제";

	const char* ID2GAMEWORD_REFRESH = "새로고침";
	
	const char* ID2GAMEWORD_CURRENT_PLAYERS = "현재인원";


	const char* ID2GAMEWORD_LEFT_MOUSE_CLICK = "마우스 좌-클릭하시면 %1%님에게 파티 가입 신청을 보냅니다.";
	const char* ID2GAMEWORD_RIGHT_MOUSE_CLICK = "마우스 우-클릭하시면 %1%님에게 귓속말을 보낼 수 있습니다.";



	const DWORD COLOR_ACTIVE = D3DCOLOR_XRGB(255,255,255);
	const DWORD COLOR_DEACTIVE = D3DCOLOR_XRGB(125,125,125);


}

RnMapPartyTap::RnMapPartyTap( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice,RnMapPrivate::IMapInterface* pMapInterface ) :
CUIGroupHelper(pEngineDevice)
{
	m_pGaeaClient = pGaeaClient;
	m_pInterface  = pInterface;
	m_pMapInterface = pMapInterface;

	m_CurrentMapID = (DWORD)-1;
}

void RnMapPartyTap::CreateSubControl()
{

	CD3DFontPar* pFont10 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 10, _DEFAULT_FONT_FLAG );

	CreateLineBox("MAP_WINDOW_PARTY_TAP_REGION","MAP_WINDOW_LINEBOX_TEXTUREINFO");

	CUIControl* pBox = NULL;

	pBox = CreateLineBox("MAP_WINDOW_PARTY_TAP_PARTY_INFO_REGION","MAP_WINDOW_LINEBOX2_TEXTUREINFO");
	pBox->SetDiffuse(D3DCOLOR_XRGB( BYTE(111 * 1.7),BYTE(127*1.7),255) );

	CreateText(ID2GAMEWORD("MAP_PARTY_INFO"),"MAP_WINDOW_PARTY_TAP_PARTY_INFO_TEXT",TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP);


	CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
	pScrollBar->CreateSub( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, PARTY_AD_LIST );
	pScrollBar->CreateBaseScrollBar( "MAP_WINDOW_PARTY_TAP_PARTY_LIST_SCROLL" );
	RegisterControl ( pScrollBar );
	m_pPartyInfoScroll = pScrollBar;


	RnButton* pStatBtn = NULL;
	RnButton::CreateArg arg;
	//////////////////////////////////////////////////////////////////////////
	arg.defaultTextureKey = "CHARACTER_WINDOW_STAT_PLUS_MINUS_BUTTON_NORMAL_TEXTUREINFO";
	arg.mouseOverTextureKey = "CHARACTER_WINDOW_STAT_PLUS_MINUS_BUTTON_MOUSEOVER_TEXTUREINFO";
	arg.mouseClickTextureKey = "CHARACTER_WINDOW_STAT_PLUS_MINUS_BUTTON_MOUSECLICK_TEXTUREINFO";
	//	arg.textKey = "CHARACTER_WINDOW_STAT_BUTTON_TEXT_POS";
	arg.pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 10, _DEFAULT_FONT_SHADOW_FLAG );

	//////////////////////////////////////////////////////////////////////////
	arg.text = ID2GAMEWORD("MAP_PARTY_REGISTER");

	RnButton* pRegisterBtn = new RnButton(m_pEngineDevice);
	pRegisterBtn->CreateSub(this,"MAP_WINDOW_PARTY_TAP_REGISTER_BUTTON",UI_FLAG_DEFAULT,PARTY_REGISTER_BUTTON);
	pRegisterBtn->CreateSubControl(arg);
	RegisterControl(pRegisterBtn);
	m_pRegisterBtn = pRegisterBtn;

	arg.text = ID2GAMEWORD("MAP_PARTY_UNREGISTER");
	RnButton* pDeleteBtn = new RnButton(m_pEngineDevice);
	pDeleteBtn->CreateSub(this,"MAP_WINDOW_PARTY_TAP_REGISTER_BUTTON",UI_FLAG_DEFAULT,PARTY_DELETE_BUTTON);
	pDeleteBtn->CreateSubControl(arg);
	RegisterControl(pDeleteBtn);
	pDeleteBtn->SetVisibleSingle(FALSE);
	m_pDeleteBtn = pDeleteBtn;


	arg.text = ID2GAMEWORD("MAP_REFRESH");

	RnButton* pRefreshBtn = new RnButton(m_pEngineDevice);
	pRefreshBtn->CreateSub(this,"MAP_WINDOW_PARTY_TAP_REFRESH_BUTTON",UI_FLAG_DEFAULT,PARTY_REFRESH_BUTTON);
	pRefreshBtn->CreateSubControl(arg);
	RegisterControl(pRefreshBtn);

	for (size_t i=0; i < PARTY_LIST_MAX; ++i)
	{
		m_pPartyInfoRect[i] = CreateLineBox(sc::string::format("MAP_WINDOW_PARTY_TAP_PARTY_RECT%1%",i+1),"CHARACTER_WINDOW_STAT_PLUS_MINUS_BUTTON_MOUSEOVER_TEXTUREINFO", UIGUID(PARTY_INFO_RECT1 + i));
		m_pPartyInfoTitle[i] = CreateText("Title",sc::string::format("MAP_WINDOW_PARTY_TAP_PARTY_TITLE%1%",i+1),TEXT_ALIGN_LEFT|TEXT_ALIGN_CENTER_Y);
		m_pPartyInfoNumber[i] = CreateText("Number : 8/8",sc::string::format("MAP_WINDOW_PARTY_TAP_PARTY_NUMBER%1%",i+1),TEXT_ALIGN_LEFT|TEXT_ALIGN_CENTER_Y);
	}


	CreateLineBox("MAP_WINDOW_PARTY_TAP_TITLE_BACK_LINE","MAP_WINDOW_PARTY_TAP_TITLE_BACK_LINE_TEXTUREINFO");

	RnEditBox* pEditBox = new RnEditBox(m_pEngineDevice);
	pEditBox->Create(this,"MAP_WINDOW_PARTY_TAP_EDIT_MAN","MAP_WINDOW_PARTY_TAP_EDIT","MAP_WINDOW_PARTY_TAP_CARRAT",
		PARTY_INFO_EDITBOX,true,D3DCOLOR_XRGB(255,255,255),pFont10,100);
	RegisterControl(pEditBox);
	m_pEditBox = pEditBox;


	//////////////////////////////////////////////////////////////////////////
	UpdatePartyInfoList();
}

void RnMapPartyTap::RefreshPartyInfo()
{
	GLMSG::NET_MAP_REFRESH_PARTY_BOARD_REQ_CA msg;
	msg.m_MapID = m_pMapInterface->GetSelectMap();
	m_pGaeaClient->NETSENDTOAGENT(&msg);
}

void RnMapPartyTap::UpdatePartyInfoList()
{
	if( m_CurrentMapID != m_pMapInterface->GetSelectMap() )
	{
		m_CurrentMapID = m_pMapInterface->GetSelectMap();
		RefreshPartyInfo();	
	}

	m_pPartyInfoScroll->GetThumbFrame()->SetState( (int)m_PartInfoDataVec.size(), PARTY_LIST_MAX );
	int scrollPos = m_pPartyInfoScroll->GetThumbFrame()->GetPosition();
	for (int i = 0; i < PARTY_LIST_MAX; ++i)
	{
		m_pPartyInfoRect[i]->SetVisibleSingle(FALSE);
		m_pPartyInfoTitle[i]->SetVisibleSingle(FALSE);
		m_pPartyInfoNumber[i]->SetVisibleSingle(FALSE);
	}

	size_t size = m_PartInfoDataVec.size() < PARTY_LIST_MAX ? m_PartInfoDataVec.size() : PARTY_LIST_MAX;
	for (size_t i=0; i < size; ++i)
	{
		m_pPartyInfoRect[i]->SetVisibleSingle(TRUE);
		m_pPartyInfoTitle[i]->SetVisibleSingle(TRUE);
		m_pPartyInfoNumber[i]->SetVisibleSingle(TRUE);

		const SPartyInfo& data = m_PartInfoDataVec[i+scrollPos];
		
		m_pPartyInfoTitle[i]->SetText( data.m_Title.c_str() );
		std::string numberString = sc::string::format("%1% : %2% / %3%",ID2GAMEWORD("MAP_CURRENT_PLAYERS"),data.m_CurrentPlayer,data.m_MaxPlayer);
		m_pPartyInfoNumber[i]->SetText( numberString.c_str() );
	}


}

void RnMapPartyTap::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroupHelper::Update(x,y,LB,MB,RB,nScroll,fElapsedTime,bFirstControl);

	UpdatePartyInfoList();
}

void RnMapPartyTap::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	switch(ControlID)
	{
	case PARTY_INFO_EDITBOX:
	case PARTY_REGISTER_BUTTON:
		{
			if(dwMsg & UIMSG_RNBUTTON_CLICK || dwMsg & UIMSG_RNEDITBOX_RETURN)
			{
				GLMSG::NET_MAP_CREATE_PARTY_BOARD_REQ_CA msg;
				msg.m_baseMapID = m_pMapInterface->GetSelectMap();
				msg.m_Title = m_pEditBox->GetEditString();
                
                msgpack::sbuffer SendBuffer;
                msgpack::pack(SendBuffer, msg);

				m_pGaeaClient->NETSENDTOAGENT(NET_MSG_MAP_CREATE_PARTY_BOARD_REQ_CA, SendBuffer);
			}
			break;
		}
	case PARTY_DELETE_BUTTON:
		{
			if(dwMsg & UIMSG_RNBUTTON_CLICK)
			{
				GLMSG::NET_MAP_DELETE_PARTY_BOARD_REQ_CA msg;
				msg.m_forRefreshMapID = m_pMapInterface->GetSelectMap();
				m_pGaeaClient->NETSENDTOAGENT(&msg);
			}

			break;
		}
	case PARTY_REFRESH_BUTTON:
		{
			if(dwMsg & UIMSG_RNBUTTON_CLICK)
			{
				RefreshPartyInfo();
			}
			break;
		}
	default:
		{
			if( ControlID >= PARTY_INFO_RECT1 && ControlID < PARTY_INFO_RECT_MAX)
			{
				int index = ControlID - PARTY_INFO_RECT1;
				if( CHECK_MOUSE_IN( dwMsg ) )
				{
					m_pPartyInfoRect[index]->SetDiffuse(COLOR_ACTIVE);

					int scrollPos = m_pPartyInfoScroll->GetThumbFrame()->GetPosition();
					const SPartyInfo& data = m_PartInfoDataVec[index+scrollPos];

					m_pInterface->BEGIN_COMMON_LINEINFO_MULTI();
					m_pInterface->ADD_COMMON_LINEINFO_MULTI(sc::string::format(ID2GAMEWORD("MAP_LEFT_MOUSE_CLICK"),data.m_Name).c_str(),D3DCOLOR_XRGB(255,255,255));
					m_pInterface->ADD_COMMON_LINEINFO_MULTI(sc::string::format(ID2GAMEWORD("MAP_RIGHT_MOUSE_CLICK"),data.m_Name).c_str(),D3DCOLOR_XRGB(255,255,255));
					m_pInterface->END_COMMON_LINEINFO_MULTI();
					
					if( UIMSG_LB_DOWN & dwMsg )
					{
						GLMSG::NET_MAP_PARTY_INFO_DETAIL_REQ_CA msg;
						msg.m_MapID = m_pMapInterface->GetSelectMap();
						strncpy_s(msg.m_szChaName,sizeof(msg.m_szChaName),data.m_Name.c_str(),data.m_Name.size() );
						m_pGaeaClient->NETSENDTOAGENT(&msg);
					}
					else if( UIMSG_RB_DOWN & dwMsg )
					{
						IBasicChatWindow* pChatWindow = m_pInterface->GetBasicChatWindow();
						if(!data.m_Name.empty())
						{
							pChatWindow->BeginPrivateChat(data.m_Name);
						}
					}

				}
				else
				{
					m_pPartyInfoRect[index]->SetDiffuse(COLOR_DEACTIVE);
				}
			}

			break;
		}
	}


}


void RnMapPartyTap::SetVisibleSingle( BOOL bVisible )
{
	CUIGroupHelper::SetVisibleSingle(bVisible);

	if( bVisible )
	{
		RefreshPartyInfo();
	}
}

void RnMapPartyTap::MsgMapPartyInfoListBoard( bool isExistMyBoard,DWORD mapID, const std::vector<std::string>& NameVec,const std::vector<DWORD>& MaxPlayerVec, const std::vector<DWORD>& CurrentPlayerVec,const std::vector<std::string>& TitleVec )
{
	if( m_pMapInterface->GetSelectMap() != mapID )
		return;


	if( isExistMyBoard )
	{
		m_pRegisterBtn->SetVisibleSingle(FALSE);
		m_pDeleteBtn->SetVisibleSingle(TRUE);		
	}
	else
	{
		m_pRegisterBtn->SetVisibleSingle(TRUE);
		m_pDeleteBtn->SetVisibleSingle(FALSE);	
	}

	m_PartInfoDataVec.clear();

	for (size_t i = 0; i < NameVec.size(); ++i)
	{
		SPartyInfo testInfo;
		testInfo.m_CurrentPlayer = CurrentPlayerVec[i];
		testInfo.m_MaxPlayer = MaxPlayerVec[i];
		testInfo.m_Name = NameVec[i];
		testInfo.m_Title = TitleVec[i];
		m_PartInfoDataVec.push_back(testInfo);
	}


	UpdatePartyInfoList();
	
}

void RnMapPartyTap::MsgMapPartyInfoDetail( DWORD mapID, const std::vector<GLPARTY_LINK_MEM>& partyMemberInfoVec )
{
	GLPARTY_LINK partyLink;
	partyLink.sMapID = mapID;
	
	for (size_t i=0; i < partyMemberInfoVec.size(); ++i)
	{
		partyLink.sPartyMem[i] = partyMemberInfoVec[i];
	}

	m_pInterface->SHOW_PARTY_LINK_WINDOW(-1,partyLink);
}


