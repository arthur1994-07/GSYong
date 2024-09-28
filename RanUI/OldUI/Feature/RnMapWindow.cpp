
#include "StdAfx.h"

#include "RnMapWindow.h"

#include "../EngineLib/DeviceMan.h"
#include "../EngineLib/GUInterface/BasicTextBox.h"
#include "../EngineLib/DxTools/DxFontMan.h"
#include "../EngineLib/GUInterface/GameTextControl.h"
#include "../EngineLib/GUInterface/BasicButton.h"

#include "../Util/RnButton.h"

#include "../../../RanLogic/Land/GLMapAxisInfo.h"
#include "../../../RanLogic/GLLevelFile.h"

#include "../../StaticUIManager.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Land/GLLandManClient.h"
#include "../../../RanLogicClient/Level/GLLevelFileClient.h"

#include "../../InnerInterface.h"

#include "../../../RanLogic/GLUseFeatures.h"
#include "../../../RanLogic/Quest/GLQuest.h"
#include "../../../RanLogic/Quest/GLQuestMan.h"


#include "RnOldLargeMapWindow.h"

#include "RnMapSettingTap.h"
#include "RnMapInfoTap.h"
#include "RnMapPartyTap.h"
#include "RnMapSearchTap.h"
#include "RnMapGuidanceUI.h"


// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace
{
	const char* ID2GAMEINTEXT_SETTING = "설정";
	const char* ID2GAMEINTEXT_INFO = "정보";
	const char* ID2GAMEINTEXT_PARTY = "파티";
	const char* ID2GAMEINTEXT_SEARCH = "검색";


	const char*  ID2GAMEWORD_ENTER_CONDITION = "지역 진입 조건";
	const char*  ID2GAMEWORD_LEVEL_CONDITION = "레벨 조건";
	const char*  ID2GAMEWORD_QUEST_CONDITION = "진입 퀘스트";

//	const char*  ID2GAMEWORD_PARTY_CONDITION = "파티 조건";
//	const char*  ID2GAMEWORD_CLUB_CONDITION = "클럽 조건";


	const char*  ID2GAMEWORD_ALWAYS = "항상";
	const char*  ID2GAMEWORD_SAME_HIGHER = "이상";
	const char*  ID2GAMEWORD_HIGHER = "초과";
	const char*  ID2GAMEWORD_SAME_LOWER= "이하";
	const char*  ID2GAMEWORD_LOWER = "미만";
	const char*  ID2GAMEWORD_FROMTO = "부터";


	// 이건 존재 안함
//	const char*  ID2GAMEWORD_ENTER_WAIT_TIME = "재입장 대기 시간";

}

RnMapWindow::RnMapWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice ) :
	CUIWindowEx(pInterface,pEngineDevice) 
    , m_pGuidanceUI(NULL)
{
	m_pGaeaClient = pGaeaClient;

	m_pCurrentTap = NULL;

	m_pOldLargeMapWindow = NULL;
	m_pSelectMapID = -1;
// 	m_isMapImageDrag = false;
// 
// 
// 	m_vMapTexturePos.x = 0;
// 	m_vMapTexturePos.y = 0;
// 
// 	m_vMapTextureSize.x = 0;
// 	m_vMapTextureSize.y = 0;


    ID2GAMEINTEXT_SETTING = ID2GAMEINTEXT("RN_MAP_WINDOW_SETTING");
    ID2GAMEINTEXT_INFO = ID2GAMEINTEXT("RN_MAP_WINDOW_INFO");
    ID2GAMEINTEXT_PARTY = ID2GAMEINTEXT("RN_MAP_WINDOW_PARTY");
    ID2GAMEINTEXT_SEARCH = ID2GAMEINTEXT("RN_MAP_WINDOW_SEARCH");


    ID2GAMEWORD_ENTER_CONDITION = ID2GAMEWORD("RN_MAP_WINDOW_CONDITON", 0);
    ID2GAMEWORD_LEVEL_CONDITION = ID2GAMEWORD("RN_MAP_WINDOW_CONDITON", 1);
    ID2GAMEWORD_QUEST_CONDITION = ID2GAMEWORD("RN_MAP_WINDOW_CONDITON", 2);


    ID2GAMEWORD_ALWAYS = ID2GAMEWORD("RN_MAP_WINDOW_ALWAYS");
    ID2GAMEWORD_SAME_HIGHER = ID2GAMEWORD("RN_MAP_WINDOW_SAME_HIGHER");
    ID2GAMEWORD_HIGHER =ID2GAMEWORD( "RN_MAP_WINDOW_HIGHER");
    ID2GAMEWORD_SAME_LOWER= ID2GAMEWORD("RN_MAP_WINDOW_SAME_LOW");
    ID2GAMEWORD_LOWER = ID2GAMEWORD("RN_MAP_WINDOW_LOW");                                                                                      
    ID2GAMEWORD_FROMTO = ID2GAMEWORD("RN_MAP_WINDOW_FROMTO");

}

void RnMapWindow::CreateSubControl()
{
	CreateBaseWindowLightGray( "MAP_WINDOW_MAIN",  ID2GAMEWORD("LARGEMAP_WINDOW_STATIC") );
	
	CreateLineBox( "MAP_WINDOW_MAIN_REGION", "MAP_WINDOW_MAIN_REGION_TEXTUREINFO" );





	//////////////////////////////////////////////////////////////////////////

	RnButton::CreateArg arg;
	arg.dwColor = D3DCOLOR_XRGB(255,255,255);
	arg.defaultTextureKey = "RNBUTTON_DEFAULT_TEXTURE";
	arg.mouseOverTextureKey = "RNBUTTON_OVER_TEXTURE";
	arg.mouseClickTextureKey = "RNBUTTON_CLICK_TEXTURE";

	arg.text = "<<";
	arg.pFont = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );


	RnButton* pMyPosMapBtn = new RnButton(m_pEngineDevice);
	pMyPosMapBtn->CreateSub(this,"MAP_WINDOW_CHANGEMAP_MYPOS_BTN",UI_FLAG_DEFAULT,CHANGEMAP_MYPOS_BTN);
	pMyPosMapBtn->CreateSubControl(arg);
	RegisterControl(pMyPosMapBtn);

	arg.text = "<";
	RnButton* pPreMapBtn = new RnButton(m_pEngineDevice);
	pPreMapBtn->CreateSub(this,"MAP_WINDOW_CHANGEMAP_PRE_BTN",UI_FLAG_DEFAULT,CHANGEMAP_PRE_BTN);
	pPreMapBtn->CreateSubControl(arg);
	RegisterControl(pPreMapBtn);

	arg.text = ">";
	RnButton* pNextMapBtn = new RnButton(m_pEngineDevice);
	pNextMapBtn->CreateSub(this,"MAP_WINDOW_CHANGEMAP_NEXT_BTN",UI_FLAG_DEFAULT,CHANGEMAP_NEXT_BTN);
	pNextMapBtn->CreateSubControl(arg);
	RegisterControl(pNextMapBtn);

	arg.pFont = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 10, _DEFAULT_FONT_SHADOW_FLAG );
	arg.text = "MAP NAME";
	RnButton* pMapBtn = new RnButton(m_pEngineDevice);
	pMapBtn->CreateSub(this,"MAP_WINDOW_CHANGEMAP_BTN",UI_FLAG_DEFAULT,CHANGEMAP_BTN);
	pMapBtn->CreateSubControl(arg);
	RegisterControl(pMapBtn);
	m_pMapListBtn = pMapBtn;
// 
	CreateFlipButton("MAP_WINDOW_CHANGEMAP_QUESTION_BUTTON","MAP_WINDOW_CHANGEMAP_QUESTION_BUTTON_OVER",CHANGEMAP_QUESTION_BTN,CBasicButton::MOUSEIN_FLIP);

	//////////////////////////////////////////////////////////////////////////


	RnMapSettingTap* pSettingTap = new RnMapSettingTap(m_pGaeaClient,m_pInterface,m_pEngineDevice);
	pSettingTap->CreateSub(this,"MAP_WINDOW_TAP_REGION",UI_FLAG_DEFAULT,SETTING_TAP);
	pSettingTap->CreateSubControl();
	pSettingTap->SetVisibleSingle(FALSE);
	RegisterControl(pSettingTap);
	m_pSettingTap = pSettingTap;



	RnMapInfoTap* pInfoTap = new RnMapInfoTap(m_pGaeaClient,m_pInterface,m_pEngineDevice,m_pOldLargeMapWindow);
	pInfoTap ->CreateSub(this,"MAP_WINDOW_TAP_REGION",UI_FLAG_DEFAULT,INFO_TAP);
	pInfoTap ->CreateSubControl();
	pInfoTap ->SetVisibleSingle(FALSE);
	RegisterControl( pInfoTap );
	m_pInfoTap = pInfoTap;


	RnMapSearchTap* pSearchTap = new RnMapSearchTap(m_pGaeaClient,m_pInterface,m_pEngineDevice,this);
	pSearchTap->CreateSub(this,"MAP_WINDOW_TAP_REGION",UI_FLAG_DEFAULT,SEARCH_TAP);
	pSearchTap->CreateSubControl();
	pSearchTap->SetVisibleSingle(FALSE);
	RegisterControl(pSearchTap);
	m_pSearchTap = pSearchTap;

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	m_pOldLargeMapWindow = new RnOldLargeMapWindow(m_pGaeaClient,m_pInterface,m_pEngineDevice,pSettingTap,this,pInfoTap,pSearchTap);
	m_pOldLargeMapWindow->CreateSub(this,"MAP_WINDOW_MAP_IMAGE_WINDOW");
	m_pOldLargeMapWindow->CreateSubControl();
	RegisterControl(m_pOldLargeMapWindow);

	//////////////////////////////////////////////////////////////////////////
    {
		// 선도전 클럽 인증 현황 표시;
		// 인증기가 존재하는 모든 컨텐츠에서 사용한다;
        RnMapGuidanceUI* pGuidance = new RnMapGuidanceUI(m_pGaeaClient, m_pInterface, m_pEngineDevice, m_pOldLargeMapWindow);
        pGuidance->CreateSub( this, "GUIDANCE_MAP_WINDOW" );
        pGuidance->SetVisibleSingle ( FALSE );
        RegisterControl ( pGuidance );
        m_pGuidanceUI = pGuidance;
    }
	//////////////////////////////////////////////////////////////////////////

	RnMapPartyTap* pPartyTap = new RnMapPartyTap(m_pGaeaClient,m_pInterface,m_pEngineDevice,this);
	pPartyTap->CreateSub(this,"MAP_WINDOW_TAP_REGION",UI_FLAG_DEFAULT,PARTY_TAP);
	pPartyTap->CreateSubControl();
	pPartyTap->SetVisibleSingle(FALSE);
	RegisterControl(pPartyTap);
	m_pPartyTap = pPartyTap;

	
	//////////////////////////////////////////////////////////////////////////




	m_pMapListRollOver = new CSmartComboBoxRollOver(m_pEngineDevice);
	m_pMapListRollOver->CreateSub( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, CHANGEMAP_ROLLOVER );
	m_pMapListRollOver->CreateSmartComboBoxRollOver( "MAP_WINDOW_CHANGEMAP_ROLLOVER", "MAP_WINDOW_CHANGEMAP_ROLLOVER_TEXINFO" );
	m_pMapListRollOver->SetDiffuse( NS_UITEXTCOLOR::DARKGRAY );
	m_pMapListRollOver->SetVisibleSingle ( FALSE );
	RegisterControl ( m_pMapListRollOver );



	//////////////////////////////////////////////////////////////////////////
	MakeRadioButton("MAP_WINDOW_MAIN_SETTING_RADIO_BUTTON",MAP_SETTING_RADIO,
		ID2GAMEINTEXT_SETTING,TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP,"MAP_WINDOW_MAIN_RADIO_TEXT_POS",
		NS_UITEXTUTIL::BLACK,"MAP_WINDOW_MAIN_RADIO_BUTTON_CHECK_BOX",
		"MAP_WINDOW_RADIO_ON_TEXTUREINFO","MAP_WINDOW_RADIO_OFF_TEXTUREINFO");

	MakeRadioButton("MAP_WINDOW_MAIN_INFO_RADIO_BUTTON",MAP_INFO_RADIO,
		ID2GAMEINTEXT_INFO,TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP,"MAP_WINDOW_MAIN_RADIO_TEXT_POS",
		NS_UITEXTUTIL::BLACK,"MAP_WINDOW_MAIN_RADIO_BUTTON_CHECK_BOX",
		"MAP_WINDOW_RADIO_ON_TEXTUREINFO","MAP_WINDOW_RADIO_OFF_TEXTUREINFO");

	MakeRadioButton("MAP_WINDOW_MAIN_PARTY_RADIO_BUTTON",MAP_PARTY_RADIO,
		ID2GAMEINTEXT_PARTY,TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP,"MAP_WINDOW_MAIN_RADIO_TEXT_POS",
		NS_UITEXTUTIL::BLACK,"MAP_WINDOW_MAIN_RADIO_BUTTON_CHECK_BOX",
		"MAP_WINDOW_RADIO_ON_TEXTUREINFO","MAP_WINDOW_RADIO_OFF_TEXTUREINFO");

	MakeRadioButton("MAP_WINDOW_MAIN_SEARCH_RADIO_BUTTON",MAP_SEARCH_RADIO,
		ID2GAMEINTEXT_SEARCH,TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP,"MAP_WINDOW_MAIN_RADIO_TEXT_POS",
		NS_UITEXTUTIL::BLACK,"MAP_WINDOW_MAIN_RADIO_BUTTON_CHECK_BOX",
		"MAP_WINDOW_RADIO_ON_TEXTUREINFO","MAP_WINDOW_RADIO_OFF_TEXTUREINFO");


	EventSelectedTap(MAP_SETTING_RADIO);
}

void RnMapWindow::VISIBLE_TARGET_POS( BOOL bVisible )
{
	
}

void RnMapWindow::MakeRadioButton(const std::string& radioKey,UIGUID radioID,const std::string& radioText,DWORD align,
								  const std::string& buttonTextPosKey,DWORD color,
								  const std::string& imagePosKey,const std::string& imageOnKey,const std::string& imageOffKey)
{
	DEFAULT_RADIO_BUTTON* pRadio = new DEFAULT_RADIO_BUTTON(m_pEngineDevice);
	pRadio->CreateSub(this,radioKey.c_str(),UI_FLAG_DEFAULT,radioID);
	pRadio->CreateSubControl(radioText.c_str(), align ,buttonTextPosKey.c_str(),color,imagePosKey.c_str(),
		imageOnKey.c_str(),imageOffKey.c_str());

	RegisterTapButton(pRadio);
	RegisterControl(pRadio);
}

void RnMapWindow::EventOverTap(UIGUID ControlID)
{
	const GLMapList::FIELDMAP& mapList = m_pGaeaClient->GetMapList();
	GLMapList::FIELDMAP::const_iterator _iter = mapList.find(m_pSelectMapID);
	if ( _iter == mapList.end() )
		return;

	const SMAPNODE& node = _iter->second;
	if ( node.IsUIMapInfo() == false )
	{
		switch ( ControlID )
		{	
		case MAP_INFO_RADIO:
		case MAP_PARTY_RADIO:
		case MAP_SEARCH_RADIO:
			{
				const DWORD dwMapID = _iter->first;
				const CString strMapName(m_pGaeaClient->GetMapName(dwMapID));
				m_pInterface->BEGIN_COMMON_LINEINFO_MULTI();				
				m_pInterface->ADD_COMMON_LINEINFO_MULTI(sc::string::format(ID2GAMEINTEXT("RN_MAP_WINDOW_MAP_INFO_WARNING"), strMapName).c_str(), NS_UITEXTCOLOR::WHITE);
				m_pInterface->END_COMMON_LINEINFO_MULTI();
			}			
			break;		
		}
	}
} 

void RnMapWindow::EventSelectedTap( UIGUID ControlID )
{
	const GLMapList::FIELDMAP& mapList = m_pGaeaClient->GetMapList();
	GLMapList::FIELDMAP::const_iterator _iter = mapList.find(m_pSelectMapID);
	if ( _iter == mapList.end() )
		return;

	const SMAPNODE& node = _iter->second;
	if ( node.IsUIMapInfo() == true )
	{
		if(m_pCurrentTap)
			m_pCurrentTap->SetVisibleSingle(FALSE);

		m_pCurrentTap = GetTapWindow(ControlID);
		m_pCurrentTap->SetVisibleSingle(TRUE);		
	}
	else
	{
		switch ( ControlID )
		{	
		case MAP_INFO_RADIO:
		case MAP_PARTY_RADIO:
		case MAP_SEARCH_RADIO:		
			TapSelect(MAP_SETTING_RADIO);			
			break;
		default:
			{
				if(m_pCurrentTap)
					m_pCurrentTap->SetVisibleSingle(FALSE);
				m_pCurrentTap = GetTapWindow(MAP_SETTING_RADIO);
				m_pCurrentTap->SetVisibleSingle(TRUE);
			}
		}
	}
}

void RnMapWindow::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage ( ControlID, dwMsg );
	EMTAPSELECTORRESULT emResult = EMTAPSELECTORRESULT_NONE;
	TapSelectorUIMessage( ControlID, dwMsg, emResult );

	switch(ControlID)
	{
	case CHANGEMAP_BTN:
		{
			if( dwMsg & UIMSG_RNBUTTON_CLICK )
			{
				m_pMapListRollOver->ClearText();
				if( m_pMapListRollOver->IsVisible() )
				{
					m_pMapListRollOver->SetVisibleSingle(FALSE);
					m_pOldLargeMapWindow->EnableLButton(true);
					break;
				}

				m_pMapListRollOver->SetVisibleSingle(TRUE);

				const GLMapList::FIELDMAP& mapList = m_pGaeaClient->GetMapList();
				int nIndex = 0;
				for (GLMapList::FIELDMAP::const_iterator it = mapList.begin(); it != mapList.end(); ++it)
				{

					D3DCOLOR dwTextColor = NS_UITEXTUTIL::ENABLE;

					const SMAPNODE& node = it->second;
					DWORD mapID = it->first;	
					if ( node.IsUIMapSelect() == false )
						continue;

					if( SNATIVEID(mapID) == m_pGaeaClient->GetActiveMapID().getGaeaMapID() )
						dwTextColor = D3DCOLOR_XRGB(255,255,50);
					else if(m_pMapListBtn->GetText() == m_pGaeaClient->GetMapName(mapID) )
						dwTextColor = D3DCOLOR_XRGB(50,50,255);

					nIndex = m_pMapListRollOver->AddText( m_pGaeaClient->GetMapName(mapID), FALSE, FALSE, dwTextColor );
					m_pMapListRollOver->SetTextData(nIndex,mapID);
				}

				m_pOldLargeMapWindow->EnableLButton(false);
			}
			break;
		}
	case CHANGEMAP_MYPOS_BTN:
		{
			if( dwMsg & UIMSG_RNBUTTON_CLICK )
			{
				SetMapLevelFile(*m_pGaeaClient->GetActiveMap());
				m_pMapListRollOver->SetVisibleSingle( FALSE );	
				m_pOldLargeMapWindow->EnableLButton(true);
			}
			break;
		}
	case CHANGEMAP_PRE_BTN:
		{
			if( dwMsg & UIMSG_RNBUTTON_CLICK )
			{
				const GLMapList::FIELDMAP& mapList = m_pGaeaClient->GetMapList();
				GLMapList::FIELDMAP::const_iterator it = mapList.find(m_pSelectMapID);
				if ( it == mapList.end() )
					break;

				while ( it != mapList.begin() )
				{
					--it;
					const SMAPNODE& node = it->second;					
					if ( node.IsUIMapSelect() == true )
					{
						GLLevelFileClient selectLevelFile(m_pGaeaClient);
						selectLevelFile.LoadFile(node.m_LevelFile,m_pd3dDevice);
						selectLevelFile.SetBaseMapID(node.GetId());
						selectLevelFile.SetPeaceZone(true);
						SetMapLevelFile(selectLevelFile);
						m_pMapListRollOver->SetVisibleSingle( FALSE );	
						m_pOldLargeMapWindow->EnableLButton(true);
						break;
					}					
				};
			}
		

			break;
		}
	case CHANGEMAP_NEXT_BTN:
		{
			if( dwMsg & UIMSG_RNBUTTON_CLICK )
			{
				const GLMapList::FIELDMAP& mapList = m_pGaeaClient->GetMapList();
				GLMapList::FIELDMAP::const_iterator it = mapList.find(m_pSelectMapID);
				if ( it == mapList.end() )
					break;

				++it;
				if ( it == mapList.end() )
					break;

				while ( it != mapList.end() )
				{					
					const SMAPNODE& node = it->second;					
					if ( node.IsUIMapSelect() == true )
					{
						GLLevelFileClient selectLevelFile(m_pGaeaClient);
						selectLevelFile.LoadFile(node.m_LevelFile,m_pd3dDevice);
						selectLevelFile.SetBaseMapID(node.GetId());
						selectLevelFile.SetPeaceZone(true);
						SetMapLevelFile(selectLevelFile);

						m_pMapListRollOver->SetVisibleSingle( FALSE );	
						m_pOldLargeMapWindow->EnableLButton(true);
						break;
					}

					++it;
				};
			}

			break;
		}
	case CHANGEMAP_ROLLOVER:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{	
				int nIndex = m_pMapListRollOver->GetSelectIndex ();
				if ( nIndex < 0 ) 
					return ;

				DWORD dwData = m_pMapListRollOver->GetTextData( nIndex );

				SetMapLevelFile(dwData);

				m_pMapListRollOver->SetVisibleSingle( FALSE );	
				m_pOldLargeMapWindow->EnableLButton(true);
			}
			break;
		}
	case CHANGEMAP_QUESTION_BTN:
		{
			if( CHECK_MOUSE_IN(dwMsg) )
			{

				int level = GetMapLevelFile()->GetLevelRequire()->m_wLevel;
				int level2 = GetMapLevelFile()->GetLevelRequire()->m_wLevel2;
				EMCDT_SIGN  sign = GetMapLevelFile()->GetLevelRequire()->m_signLevel;

				
				m_pInterface->BEGIN_COMMON_LINEINFO_MULTI(TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);
				m_pInterface->ADD_COMMON_LINEINFO_MULTI( ("[" + m_pMapListBtn->GetText() + "]" + ID2GAMEWORD_ENTER_CONDITION).c_str() ,D3DCOLOR_XRGB(255,255,255));

				if( sign == EMSIGN_ALWAYS)
				{
					m_pInterface->ADD_COMMON_LINEINFO_MULTI( (std::string() + ID2GAMEWORD_LEVEL_CONDITION + " : " + ID2GAMEWORD_ALWAYS).c_str() ,D3DCOLOR_XRGB(255,255,255));
				}
				else if( sign == EMSIGN_HIGHER )
				{
					m_pInterface->ADD_COMMON_LINEINFO_MULTI( sc::string::format("%1% : %2% %3%" , ID2GAMEWORD_LEVEL_CONDITION , level , ID2GAMEWORD_HIGHER ).c_str() ,D3DCOLOR_XRGB(255,255,255));
				}
				else if( sign == EMSIGN_SAMEHIGHER )
				{
					m_pInterface->ADD_COMMON_LINEINFO_MULTI( sc::string::format("%1% : %2% %3%" , ID2GAMEWORD_LEVEL_CONDITION ,level, ID2GAMEWORD_SAME_HIGHER ).c_str() ,D3DCOLOR_XRGB(255,255,255));
				}
				else if( sign == EMSIGN_LOWER)
				{
					m_pInterface->ADD_COMMON_LINEINFO_MULTI( sc::string::format("%1% : %2% %3%" ,ID2GAMEWORD_LEVEL_CONDITION ,level, ID2GAMEWORD_LOWER).c_str() ,D3DCOLOR_XRGB(255,255,255));
				}
				else if( sign == EMSIGN_SAMELOWER)
				{
					m_pInterface->ADD_COMMON_LINEINFO_MULTI( sc::string::format("%1% : %2% %3%" ,ID2GAMEWORD_LEVEL_CONDITION ,level, ID2GAMEWORD_SAME_LOWER).c_str() ,D3DCOLOR_XRGB(255,255,255));
				}
				else if( sign == EMSIGN_SAME)
				{
					m_pInterface->ADD_COMMON_LINEINFO_MULTI( sc::string::format("%1% : %2%" ,ID2GAMEWORD_LEVEL_CONDITION ,level ).c_str(),D3DCOLOR_XRGB(255,255,255));
				}
				else if( sign == EMSIGN_FROMTO )
				{
					m_pInterface->ADD_COMMON_LINEINFO_MULTI( sc::string::format("%1% : %2% - %3%" ,ID2GAMEWORD_LEVEL_CONDITION ,level,level2 ).c_str(),D3DCOLOR_XRGB(255,255,255));
				}

				GLQUEST* pQuest = GLQuestMan::GetInstance().Find(GetMapLevelFile()->GetLevelRequire()->m_sComQuestID.Id());
				if(pQuest)
				{
					if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					{
						CString strKey;
						strKey.Format("QN_%03d_%03d", pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID);
						m_pInterface->ADD_COMMON_LINEINFO_MULTI( sc::string::format("%1% : %2%" ,ID2GAMEWORD_QUEST_CONDITION,CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString() ).c_str(),D3DCOLOR_XRGB(255,255,255));
					}
					else
					{
						m_pInterface->ADD_COMMON_LINEINFO_MULTI( sc::string::format("%1% : %2%" ,ID2GAMEWORD_QUEST_CONDITION,pQuest->GetTITLE() ).c_str(),D3DCOLOR_XRGB(255,255,255));
					}
				}

				m_pInterface->END_COMMON_LINEINFO_MULTI();
			}
			break;
		}
	}
}

CUIGroup* RnMapWindow::GetTapWindow( UIGUID controlID )
{
	switch ( controlID )
	{
	case MAP_SETTING_RADIO:
		return m_pSettingTap;
	case MAP_INFO_RADIO:
		return m_pInfoTap;
	case MAP_PARTY_RADIO:
		return m_pPartyTap;
	case MAP_SEARCH_RADIO:
		return m_pSearchTap;
	}

	GASSERT(false);
	return NULL;
}

void RnMapWindow::SetMapLevelFile(const GLLevelFileClient& levFile, const CString& strMapName )
{
	// 외부로부터 이정보를 받을 필요없다	
	// 일단 기존 코드와 호환성을 위해 나두고 완성되면 삭제
}

void RnMapWindow::SetMapLevelFile( const GLLevelFileClient& levFile )
{	
	const GLMapList::FIELDMAP& mapList = m_pGaeaClient->GetMapList();
	GLMapList::FIELDMAP::const_iterator it = mapList.find(levFile.GetBaseMapID().Id());
	if ( it == mapList.end() )
		return;

	const SMAPNODE& node = it->second;
	if ( node.IsUIMapSelect() == false )
		return;	

	std::string mapName = m_pGaeaClient->GetMapName(levFile.GetBaseMapID());	
	m_pSelectMapID = levFile.GetBaseMapID().Id();
	m_pMapListBtn->SetText(mapName.c_str(),D3DCOLOR_XRGB(255,255,255));
	m_pOldLargeMapWindow->SetMapLevelFile(levFile,mapName.c_str());

	if( m_pInfoTap->IsVisible() )
	{
		m_pInfoTap->MakeMonsterInfo();
	}
	if ( node.IsUIMapInfo() == false )
	{
		TapSelect(MAP_SETTING_RADIO);
	}
}

void RnMapWindow::SetMapLevelFile( DWORD mapID )
{
	const GLMapList::FIELDMAP& mapList = m_pGaeaClient->GetMapList();
	GLMapList::FIELDMAP::const_iterator it = mapList.find(mapID);
	//GASSERT( mapList.end() != it && "Map Id is not exist in map list" );

    if( it == mapList.end() )
    {
        m_pGaeaClient->PrintMsgText (NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("MINIMAP_CANNOTSEE") );
        return;
    }

	const SMAPNODE& node = it->second;

	if ( node.IsUIMapSelect() == false )
		return;

	GLLevelFileClient selectLevelFile(m_pGaeaClient);
	selectLevelFile.LoadFile(node.m_LevelFile,m_pd3dDevice);
	selectLevelFile.SetBaseMapID(node.GetId());
	selectLevelFile.SetPeaceZone(true);
	SetMapLevelFile(selectLevelFile);
}

std::string RnMapWindow::GetMapName( DWORD mapID )
{
	const GLMapList::FIELDMAP& mapList = m_pGaeaClient->GetMapList();
	GLMapList::FIELDMAP::const_iterator it = mapList.find(mapID);
	if( mapList.end() == it )
	{
		return "NO_EXIST_MAP";
	}
//	GASSERT( mapList.end() != it );

	const SMAPNODE& node = it->second;
	return m_pGaeaClient->GetMapName(node.GetId());
}

const GLLevelFileClient* RnMapWindow::GetMapLevelFile()
{
	return m_pOldLargeMapWindow->GetMapLevelFile();
}

void RnMapWindow::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{	
	CUIWindowEx::Update(x,y,LB,MB,RB,nScroll,fElapsedTime,bFirstControl);


}

HRESULT RnMapWindow::InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;
	return CUIWindowEx::InitDeviceObjects(pd3dDevice);
}

void RnMapWindow::SetVisibleSingle( BOOL isVisible )
{
	CUIWindowEx::SetVisibleSingle(isVisible);

	if( isVisible == TRUE)
	{		
		const GLLandManClient* const pLand = m_pGaeaClient->GetActiveMap();		
		if( pLand == NULL )
			return;		

		if ( (m_pSelectMapID == -1) && (pLand->IsUIMapSelect() == false) )
		{
			const GLMapList::FIELDMAP& mapList = m_pGaeaClient->GetMapList();
			for (GLMapList::FIELDMAP::const_iterator it = mapList.begin(); it != mapList.end(); ++it)
			{
				const SMAPNODE& node = it->second;				
				if ( node.IsUIMapSelect() == true )
				{
					const DWORD mapID = it->first;	
					SetMapLevelFile(mapID);
					break;
				}				
			}
		}
		else
			SetMapLevelFile(*pLand);

		TapSelect(MAP_SETTING_RADIO);
		if(m_pCurrentTap)
			m_pCurrentTap->SetVisibleSingle(FALSE);

		m_pCurrentTap = GetTapWindow(MAP_SETTING_RADIO);
		m_pCurrentTap->SetVisibleSingle(TRUE);		
	}
}

void RnMapWindow::MsgMapSearchNameList( const std::vector<std::pair<int, std::string> >& nameList )
{
	m_pSearchTap->MsgMapSearchNameList(nameList);
}

void RnMapWindow::MsgMapSearchNameDetailInfo(int level,const std::string& charName,const std::string& title,int school,int emClass,
										   bool isParty,const std::string& clubName,DWORD dwGaeaID)
{
	m_pSearchTap->MsgMapSearchNameDetailInfo(level,charName,title,school,emClass,isParty,clubName,dwGaeaID);
}

void RnMapWindow::MsgMapSearchNamePosition(int level,const std::string& charName,DWORD mapID,const D3DXVECTOR3& pos)
{
	m_pSearchTap->MsgMapSearchNamePosition(level,charName,mapID,pos);
}


void RnMapWindow::MsgMapPartyInfoListBoard(bool isExistMyBoard,DWORD mapID,
									  const std::vector<std::string>& NameVec,const std::vector<DWORD>& MaxPlayerVec,
									  const std::vector<DWORD>& CurrentPlayerVec,const std::vector<std::string>& TitleVec)
{
	m_pPartyTap->MsgMapPartyInfoListBoard(isExistMyBoard,mapID,NameVec,MaxPlayerVec,CurrentPlayerVec,TitleVec);
}

void RnMapWindow::MsgMapPartyInfoDetail(DWORD mapID,const std::vector<GLPARTY_LINK_MEM>& memberVec)
{
	m_pPartyTap->MsgMapPartyInfoDetail(mapID,memberVec);
}

void RnMapWindow::MsgMapGuidanceAuthInfo(const NET_MSG_GENERIC* _pMessage)
{
	m_pGuidanceUI->SetVisibleSingle( TRUE );
	m_pGuidanceUI->SetGuidanceAuthInfo( _pMessage );
}

void RnMapWindow::MsgMapCTIAuthInfo(const NET_MSG_GENERIC* pMsg )
{
	m_pGuidanceUI->SetVisibleSingle( TRUE );
	m_pGuidanceUI->SetCTIAuthInfo( pMsg );
}

DWORD RnMapWindow::GetSelectMap()
{	
	return GetMapLevelFile()->GetBaseMapID().Id();
}


