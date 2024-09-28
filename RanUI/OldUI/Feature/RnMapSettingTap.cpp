#include "StdAfx.h"

#include "RnMapSettingTap.h"


#include "../EngineLib/GUInterface/BasicTextBox.h"
#include "../EngineLib/GUInterface/UIRenderQueue.h"
#include "../EngineLib/GUInterface/BasicLineBoxSmart.h"

#include "../Util/CheckBox.h"

#include "RnOldLargeMapWindow.h"

#include "../../../EngineLib/GUInterface/GameTextControl.h"

namespace
{
	const char* ID2GAMEWORD_SIGN_SETTING = "표시 설정";
	const char* ID2GAMEWORD_GATE_SIGN = "게이트 표시";
	const char* ID2GAMEWORD_NORMAL_NPC_SIGN = "일반 NPC 표시";
	const char* ID2GAMEWORD_PLACE_NAME_SIGN = "지명 표시";
	const char* ID2GAMEWORD_BUS_STATION_SIGN = "버스 정류장 표시";
	const char* ID2GAMEWORD_LOCKER_MANAGER_SIGN = "라커 관리인 표시";
	const char* ID2GAMEWORD_START_POSITION_SIGN = "시작 위치 표시";
	const char* ID2GAMEWORD_CLUB_MANAGER_SIGN = "클럽 관리인 표시";
// 	const char* ID2GAMEWORD_ITEM_SEARCH_SIGN = "아이템 검색 표시";
// 	const char* ID2GAMEWORD_ITEM_MIX_SIGN = "아이템 조합 표시";
	const char* ID2GAMEWORD_POSTBOX_SIGN = "우체통 표시";
	const char* ID2GAMEWORD_PARTY_PLAYER_SIGN = "파티원 표시";


	const char* ID2GAMEWORD_ACTION_SETTING = "동작 설정";
	const char* ID2GAMEWORD_AUTO_RIDE_VEHICLE = "지도 클릭시 자동 탈것 타기";

}

RnMapSettingTap::RnMapSettingTap( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice ) :
	CUIGroupHelper(pEngineDevice)
{
}

void RnMapSettingTap::CreateSubControl()
{
	CreateLineBox("MAP_WINDOW_SETTING_TAP_REGION","MAP_WINDOW_LINEBOX_TEXTUREINFO");
 	CUIControl* pBox = NULL;
	pBox = CreateLineBox("MAP_WINDOW_SETTING_TAP_SIGN_SETTING_REGION","MAP_WINDOW_LINEBOX2_TEXTUREINFO");
	pBox->SetDiffuse(D3DCOLOR_XRGB( BYTE(111 * 1.7),BYTE(127*1.7),255) );

	
	pBox = CreateLineBox("MAP_WINDOW_SETTING_TAP_ACTION_SETTING_REGION","MAP_WINDOW_LINEBOX2_TEXTUREINFO");
	pBox->SetDiffuse(D3DCOLOR_XRGB( BYTE(111 * 1.7),BYTE(127*1.7),255) );


	m_pCheckBoxViewAllSign = CreateCheckBox("MAP_WINDOW_SETTING_TAP_SIGN_CHECKBOX",CHECKBOX_VIEW_ALL_SIGN,"MAP_WINDOW_CHECKBOX_ON","MAP_WINDOW_CHECKBOX_OFF",true);
	m_pCheckBoxGate = CreateCheckBox("MAP_WINDOW_SETTING_TAP_GATE_CHECKBOX",CHECKBOX_GATE_SIGN,"MAP_WINDOW_CHECKBOX_ON","MAP_WINDOW_CHECKBOX_OFF",true);
	m_pCheckBoxNormalNPC = CreateCheckBox("MAP_WINDOW_SETTING_TAP_NORMAL_NPC_CHECKBOX",CHECKBOX_NORMAL_NPC_SIGN,"MAP_WINDOW_CHECKBOX_ON","MAP_WINDOW_CHECKBOX_OFF",true);
	m_pCheckBoxPlaceName = CreateCheckBox("MAP_WINDOW_SETTING_TAP_PLACE_NAME_CHECKBOX",CHECKBOX_PLACE_NAME_SIGN,"MAP_WINDOW_CHECKBOX_ON","MAP_WINDOW_CHECKBOX_OFF",true);
	m_pCheckBoxBusStation = CreateCheckBox("MAP_WINDOW_SETTING_TAP_BUS_STATION_CHECKBOX",CHECKBOX_BUS_STATION_SIGN,"MAP_WINDOW_CHECKBOX_ON","MAP_WINDOW_CHECKBOX_OFF",true);
	m_pCheckBoxLockerManager = CreateCheckBox("MAP_WINDOW_SETTING_TAP_LOCKER_MANAGER_CHECKBOX",CHECKBOX_LOCKER_MANAGER_SIGN,"MAP_WINDOW_CHECKBOX_ON","MAP_WINDOW_CHECKBOX_OFF",true);
	m_pCheckBoxStartPosition = CreateCheckBox("MAP_WINDOW_SETTING_TAP_START_POSITION_CHECKBOX",CHECKBOX_START_POSITION_SIGN,"MAP_WINDOW_CHECKBOX_ON","MAP_WINDOW_CHECKBOX_OFF",true);
	m_pCheckBoxClubManager = CreateCheckBox("MAP_WINDOW_SETTING_TAP_CLUB_MANAGER_CHECKBOX",CHECKBOX_START_CLUB_MANAGER_SIGN,"MAP_WINDOW_CHECKBOX_ON","MAP_WINDOW_CHECKBOX_OFF",true);	
//	m_pCheckBoxIteamSearch = CreateCheckBox("MAP_WINDOW_SETTING_TAP_ITEM_SEARCH_CHECKBOX",CHECKBOX_ITEM_SEARCH_SIGN,"MAP_WINDOW_CHECKBOX_ON","MAP_WINDOW_CHECKBOX_OFF",true);
//	m_pCheckBoxItemMix = CreateCheckBox("MAP_WINDOW_SETTING_TAP_ITEM_MIX_CHECKBOX",CHECKBOX_ITEM_MIX_SIGN,"MAP_WINDOW_CHECKBOX_ON","MAP_WINDOW_CHECKBOX_OFF",true);
	m_pCheckBoxPostBox = CreateCheckBox("MAP_WINDOW_SETTING_TAP_POSTBOX_CHECKBOX",CHECKBOX_POSTBOX_SIGN,"MAP_WINDOW_CHECKBOX_ON","MAP_WINDOW_CHECKBOX_OFF",true);
	m_pCheckBoxPartyMember = CreateCheckBox("MAP_WINDOW_SETTING_TAP_PARTY_PLAYER_CHECKBOX",CHECKBOX_PARTY_PLAYER_SIGN,"MAP_WINDOW_CHECKBOX_ON","MAP_WINDOW_CHECKBOX_OFF",true);

	m_pCheckBoxActionAllSign = CreateCheckBox("MAP_WINDOW_SETTING_TAP_ACTION_SETTING_CHECKBOX",CHECKBOX_ACTION_SIGN,"MAP_WINDOW_CHECKBOX_ON","MAP_WINDOW_CHECKBOX_OFF",true);
	m_pCheckBoxAutoRideVehicle = CreateCheckBox("MAP_WINDOW_SETTING_TAP_AUTO_RIDE_VEHICLE_CHECKBOX",CHECKBOX_AUTO_RIDE_VEHICLE_SIGN,"MAP_WINDOW_CHECKBOX_ON","MAP_WINDOW_CHECKBOX_OFF",true);
	
	m_pCheckBoxActionAllSign->SetCheckOff();
	m_pCheckBoxAutoRideVehicle->SetCheckOff();

	
	CreateText(ID2GAMEWORD("MAP_SIGN_SETTING"), "MAP_WINDOW_SETTING_TAP_SIGN_TEXT",TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP,NO_ID);
	CreateText(ID2GAMEWORD("MAP_GATE_SIGN"), "MAP_WINDOW_SETTING_TAP_GATE_TEXT",TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP,NO_ID); 
	CreateText(ID2GAMEWORD("MAP_NORMAL_NPC_SIGN"), "MAP_WINDOW_SETTING_TAP_NORMAL_NPC_TEXT",TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP,NO_ID);
	CreateText(ID2GAMEWORD("MAP_PLACE_NAME_SIGN"),"MAP_WINDOW_SETTING_TAP_PLACE_NAME_TEXT",TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP,NO_ID);
	CreateText(ID2GAMEWORD("MAP_BUS_STATION_SIGN"),"MAP_WINDOW_SETTING_TAP_BUS_STATION_TEXT",TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP,NO_ID);
	CreateText(ID2GAMEWORD("MAP_LOCKER_MANAGER_SIGN"),"MAP_WINDOW_SETTING_TAP_LOCKER_MANAGER_TEXT",TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP,NO_ID);
	CreateText(ID2GAMEWORD("MAP_START_POSITION_SIGN"),"MAP_WINDOW_SETTING_TAP_START_POSITION_TEXT",TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP,NO_ID);
	CreateText(ID2GAMEWORD("MAP_CLUB_MANAGER_SIGN"),"MAP_WINDOW_SETTING_TAP_CLUB_MANAGER_TEXT",TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP,NO_ID);
// 	CreateText(ID2GAMEWORD("MAP_ITEM_SEARCH_SIGN,"MAP_WINDOW_SETTING_TAP_ITEM_SEARCH_TEXT",TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP,NO_ID);
// 	CreateText(ID2GAMEWORD("MAP_ITEM_MIX_SIGN,"MAP_WINDOW_SETTING_TAP_ITEM_MIX_TEXT",TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP,NO_ID);
	CreateText(ID2GAMEWORD("MAP_POSTBOX_SIGN"),"MAP_WINDOW_SETTING_TAP_POSTBOX_TEXT",TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP,NO_ID);
	CreateText(ID2GAMEWORD("MAP_PARTY_PLAYER_SIGN"),"MAP_WINDOW_SETTING_TAP_PARTY_PLAYER_TEXT",TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP,NO_ID);

	CreateText(ID2GAMEWORD("MAP_ACTION_SETTING"),"MAP_WINDOW_SETTING_TAP_ACTION_SETTING_TEXT",TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP,NO_ID);
	CreateText(ID2GAMEWORD("MAP_AUTO_RIDE_VEHICLE"),"MAP_WINDOW_SETTING_TAP_AUTO_RIDE_VEHICLE_TEXT",TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP,NO_ID);

}

void RnMapSettingTap::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroupHelper::Update(x,y,LB,MB,RB,nScroll,fElapsedTime,bFirstControl);
}

void RnMapSettingTap::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	//CUIGroupHelper::TranslateUIMessage ( ControlID, dwMsg );

	switch(ControlID)
	{
	case CHECKBOX_VIEW_ALL_SIGN:
		{
			if( dwMsg & UIMSG_CHECKBOX_PUSH )
			{
				if( m_pCheckBoxViewAllSign->IsChecked() )
				{
					m_pCheckBoxViewAllSign->SetCheckOn();
					m_pCheckBoxGate->SetCheckOn();
					m_pCheckBoxNormalNPC->SetCheckOn();
					m_pCheckBoxPlaceName->SetCheckOn();
					m_pCheckBoxBusStation->SetCheckOn();
					m_pCheckBoxLockerManager->SetCheckOn();
					m_pCheckBoxStartPosition->SetCheckOn();
					m_pCheckBoxClubManager->SetCheckOn();
// 					m_pCheckBoxIteamSearch->SetCheckOn();
// 					m_pCheckBoxItemMix->SetCheckOn();
					m_pCheckBoxPostBox->SetCheckOn();
					m_pCheckBoxPartyMember->SetCheckOn();
				}
				else
				{
					m_pCheckBoxViewAllSign->SetCheckOff();
					m_pCheckBoxGate->SetCheckOff();
					m_pCheckBoxNormalNPC->SetCheckOff();
					m_pCheckBoxPlaceName->SetCheckOff();
					m_pCheckBoxBusStation->SetCheckOff();
					m_pCheckBoxLockerManager->SetCheckOff();
					m_pCheckBoxStartPosition->SetCheckOff();
					m_pCheckBoxClubManager->SetCheckOff();
// 					m_pCheckBoxIteamSearch->SetCheckOff();
// 					m_pCheckBoxItemMix->SetCheckOff();
					m_pCheckBoxPostBox->SetCheckOff();
					m_pCheckBoxPartyMember->SetCheckOff();
				}
			}
		}
	case CHECKBOX_ACTION_SIGN:
		{
			if( dwMsg & UIMSG_CHECKBOX_PUSH )
			{
				if( m_pCheckBoxActionAllSign->IsChecked() )
				{
					m_pCheckBoxAutoRideVehicle->SetCheckOn();
				}
				else
				{
					m_pCheckBoxAutoRideVehicle->SetCheckOff();
				}
			}
			break;
		}
	}
}

HRESULT RnMapSettingTap::Render( LPDIRECT3DDEVICEQ pd3dDevice )
{
// 	CUIRenderQueue::Get()->RenderImmediately();
// 	CBasicLineBoxSmart* pSmart = dynamic_cast<CBasicLineBoxSmart*>(m_testControl);
// 	pSmart->SetTest();
// 	pSmart->Render(pd3dDevice);
// 	CUIRenderQueue::Get()->RenderImmediately();
	
	CUIGroupHelper::Render(pd3dDevice);
	return S_OK;
}


bool RnMapSettingTap::IsVisibleGate()
{
	return m_pCheckBoxGate->IsChecked() ? true : false;
}

bool RnMapSettingTap::IsVisibleNormalNpc()
{
	return m_pCheckBoxNormalNPC->IsChecked() ? true : false;
}

bool RnMapSettingTap::IsVisiblePlaceName()
{
	return m_pCheckBoxPlaceName->IsChecked() ? true : false;
}

bool RnMapSettingTap::IsVisibleBusStation()
{
	return m_pCheckBoxBusStation->IsChecked() ? true : false;
}

bool RnMapSettingTap::IsVisibleLockerManager()
{
	return m_pCheckBoxLockerManager->IsChecked() ? true : false;
}

bool RnMapSettingTap::IsVisibleStartPosition()
{
	return m_pCheckBoxStartPosition->IsChecked() ? true : false;
}

bool RnMapSettingTap::IsVisibleClubManager()
{
	return m_pCheckBoxClubManager->IsChecked() ? true : false;
}

// bool RnMapSettingTap::IsVisibleItemSearch()
// {
// 	return m_pCheckBoxIteamSearch->IsChecked() ? true : false;
// }

// bool RnMapSettingTap::IsVisibleItemMix()
// {
// 	return m_pCheckBoxItemMix->IsChecked() ? true : false;
// }

bool RnMapSettingTap::IsVisiblePostBox()
{
	return m_pCheckBoxPostBox->IsChecked() ? true : false;
}

bool RnMapSettingTap::IsVisiblePartyMember()
{
	return m_pCheckBoxPartyMember->IsChecked() ? true : false;
}

bool RnMapSettingTap::IsAutoRideVehicle()
{
	return m_pCheckBoxAutoRideVehicle->IsChecked() ? true : false;
}



