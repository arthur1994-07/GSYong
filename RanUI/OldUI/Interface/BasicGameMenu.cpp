#include "StdAfx.h"

#include "../../../EngineLib/GUInterface/BasicVarTextBox.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"

#include "../../../RanLogic/RANPARAM.h"
#include "../../../RanLogic/GLogicDataMan.h"
#include "../../../RanLogic/SNS/GLSNSData.h"
#include "../../../RanLogic/GLUseFeatures.h"

#include "../../../RanLogicClient/Tutorial/GLBaseTutorial.h" // by luxes.
#include "../../../RanLogicClient/Input/GLInputManager.h"
#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../InnerInterface.h"
#include "BasicGameMenu.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const FLOAT CBasicGameMenu::fBUTTON_GAP_X = 1.0f;
const int	CBasicGameMenu::nMenuTextIndex[BASIC_MENU_NUM] = { 0,1,2,3,4,5,6,7,11,8,10,12,9 };
const int	CBasicGameMenu::nMenuShotcut[BASIC_MENU_NUM] = {	EMSHORTCUTS_INVENTORY, 
																EMSHORTCUTS_CHARACTER, 
																EMSHORTCUTS_SKILL, 
																EMSHORTCUTS_PARTY, 
																EMSHORTCUTS_QUEST,
																EMSHORTCUTS_CLUB,  
																EMSHORTCUTS_FRIEND, 
																EMSHORTCUTS_MINIMAP, 
																EMSHORTCUTS_CHATMACRO, 
																EMSHORTCUTS_ITEMBANK, 
																EMSHORTCUTS_POINTSHOP, 
																EMSHORTCUTS_SNS,
																EMSHORTCUTS_RUNMODE,
															};

CBasicGameMenu::CBasicGameMenu(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
	: CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
	, m_pRunButton(NULL)
	, m_pInfo(NULL)
	, m_nOverMenu( NO_ID )
{
}

CBasicGameMenu::~CBasicGameMenu ()
{
}

void CBasicGameMenu::CreateSubControl ()
{
	InitShotCutString();

    FLOAT fDecSizeX = 0.0f;

/*
#ifdef CH_PARAM // 중국 인터페이스 변경
	CUIControl * pControl = new CUIControl(m_pRenderDevice);
	pControl->CreateSub ( this, "GAME_MENU_TOP" );
	RegisterControl ( pControl );
#endif
*/
	CBasicButton* pButton = new CBasicButton(m_pEngineDevice);
	pButton->CreateSub ( this, "MENU_INVENTORY_BUTTON", UI_FLAG_DEFAULT, MENU_INVENTORY_BUTTON );
	pButton->CreateFlip ( "MENU_OVER_IMAGE", CBasicButton::MOUSEIN_FLIP );
	pButton->SetUseGlobalAction ( TRUE );
	RegisterControl ( pButton );

	pButton = new CBasicButton(m_pEngineDevice);
	pButton->CreateSub ( this, "MENU_CHARACTER_BUTTON", UI_FLAG_DEFAULT, MENU_CHARACTER_BUTTON );
	pButton->CreateFlip ( "MENU_OVER_IMAGE", CBasicButton::MOUSEIN_FLIP );
	pButton->SetUseGlobalAction ( TRUE );
	RegisterControl ( pButton );

	pButton = new CBasicButton(m_pEngineDevice);
	pButton->CreateSub ( this, "MENU_SKILL_BUTTON", UI_FLAG_DEFAULT, MENU_SKILL_BUTTON );
	pButton->CreateFlip ( "MENU_OVER_IMAGE", CBasicButton::MOUSEIN_FLIP );
	pButton->SetUseGlobalAction ( TRUE );
	RegisterControl ( pButton );

	pButton = new CBasicButton(m_pEngineDevice);
	pButton->CreateSub ( this, "MENU_PARTY_BUTTON", UI_FLAG_DEFAULT, MENU_PARTY_BUTTON );
	pButton->CreateFlip ( "MENU_OVER_IMAGE", CBasicButton::MOUSEIN_FLIP );
	pButton->SetUseGlobalAction ( TRUE );
	RegisterControl ( pButton );

	pButton = new CBasicButton(m_pEngineDevice);
	pButton->CreateSub ( this, "MENU_GUILD_BUTTON", UI_FLAG_DEFAULT, MENU_GUILD_BUTTON );
	pButton->CreateFlip ( "MENU_OVER_IMAGE", CBasicButton::MOUSEIN_FLIP );
	pButton->SetUseGlobalAction ( TRUE );
	RegisterControl ( pButton );

    pButton = new CBasicButton(m_pEngineDevice);
    pButton->CreateSub ( this, "MENU_QUEST_BUTTON", UI_FLAG_DEFAULT, MENU_QUEST_BUTTON );
    pButton->CreateFlip ( "MENU_OVER_IMAGE", CBasicButton::MOUSEIN_FLIP );
    pButton->SetUseGlobalAction ( TRUE );
    RegisterControl ( pButton );
	
	pButton = new CBasicButton(m_pEngineDevice);
	pButton->CreateSub ( this, "MENU_FRIEND_BUTTON", UI_FLAG_DEFAULT, MENU_FRIEND_BUTTON );
	pButton->CreateFlip ( "MENU_OVER_IMAGE", CBasicButton::MOUSEIN_FLIP );
	pButton->SetUseGlobalAction ( TRUE );
	RegisterControl ( pButton );

	pButton = new CBasicButton(m_pEngineDevice);
	pButton->CreateSub ( this, "MENU_LARGEMAP_BUTTON", UI_FLAG_DEFAULT, MENU_LARGEMAP_BUTTON );
	pButton->CreateFlip ( "MENU_OVER_IMAGE", CBasicButton::MOUSEIN_FLIP );
	pButton->SetUseGlobalAction ( TRUE );
	RegisterControl ( pButton );

	pButton = new CBasicButton(m_pEngineDevice);
	pButton->CreateSub ( this, "MENU_CHATMACRO_BUTTON", UI_FLAG_DEFAULT, MENU_CHATMACRO_BUTTON );
	pButton->CreateFlip ( "MENU_OVER_IMAGE", CBasicButton::MOUSEIN_FLIP );
	pButton->SetUseGlobalAction ( TRUE );
	RegisterControl ( pButton );

	pButton = new CBasicButton(m_pEngineDevice);
	pButton->CreateSub ( this, "MENU_ITEMBANK_BUTTON", UI_FLAG_DEFAULT, MENU_ITEMBANK_BUTTON );
	pButton->CreateFlip ( "MENU_OVER_IMAGE", CBasicButton::MOUSEIN_FLIP );
	pButton->SetUseGlobalAction ( TRUE );
	RegisterControl ( pButton );

	pButton = new CBasicButton(m_pEngineDevice);
	pButton->CreateSub ( this, "MENU_ITEMSHOP_BUTTON", UI_FLAG_DEFAULT, MENU_POINT_SHOP_BUTTON );
	pButton->CreateFlip ( "MENU_OVER_IMAGE", CBasicButton::MOUSEIN_FLIP );
	pButton->SetUseGlobalAction ( TRUE );
	RegisterControl ( pButton );

    // Note : SNS 아이콘 (bjju.sns)
    //		  이 아이콘은 켜고 끌수 있기 때문에 이후부터는 UI 포지션을 조정해 줘야 합니다.
    {
        pButton = new CBasicButton(m_pEngineDevice);
        pButton->CreateSub ( this, "MENU_SNS_BUTTON", UI_FLAG_DEFAULT, MENU_SNS_BUTTON );
        pButton->CreateFlip ( "MENU_OVER_IMAGE", CBasicButton::MOUSEIN_FLIP );

        if ( GLUseFeatures::GetInstance().IsUsingSNS() )
        {
            pButton->SetUseGlobalAction ( TRUE );
            pButton->SetVisibleSingle ( TRUE );
        }
        else
        {
            pButton->SetVisibleSingle( FALSE );

            const UIRECT& rcLocal = pButton->GetLocalPos();
            fDecSizeX += rcLocal.sizeX + fBUTTON_GAP_X;
        }

        RegisterControl ( pButton );
    }

    // Note : 달리기 아이콘
    //
    {
        m_pRunButton = new CBasicButton(m_pEngineDevice);
        m_pRunButton->CreateSub ( this, "MENU_RUN_BUTTON", UI_FLAG_DEFAULT, MENU_RUN_BUTTON );
        m_pRunButton->CreateFlip ( "MENU_RUN_BUTTON_F", CBasicButton::RADIO_FLIP );
        m_pRunButton->CreateMouseOver( "MENU_OVER_IMAGE" );
        m_pRunButton->SetUseGlobalAction ( FALSE );
        RegisterControl ( m_pRunButton );

        UIRECT rcLocal = m_pRunButton->GetLocalPos();
        rcLocal.left  -= fDecSizeX;
        rcLocal.right -= fDecSizeX;
        m_pRunButton->SetLocalPos( rcLocal );
    }

// #ifndef CH_PARAM // 중국 인터페이스 변경
	pButton = new CBasicButton(m_pEngineDevice);
	pButton->CreateSub ( this, "MENU_CLOSE_BUTTON", UI_FLAG_DEFAULT, MENU_CLOSE_BUTTON );
	pButton->CreateFlip ( "MENU_CLOSE_BUTTON_F", CBasicButton::MOUSEIN_FLIP );
	pButton->SetUseGlobalAction ( TRUE );
	RegisterControl ( pButton );
//#endif

	m_pInfo = new CBasicVarTextBox(m_pEngineDevice);
	m_pInfo->CreateSub ( this, "BASIC_VAR_TEXT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	m_pInfo->CreateSubControl ();
	m_pInfo->SetMousePointGap ( D3DXVECTOR2(-40.0f,-40.0f) );
	m_pInfo->SetVisibleSingle ( FALSE );
	RegisterControl ( m_pInfo );

    // Note : 숨겨진 UI가 있다면 메인 UI의 사이즈를 감소시킨다.
    if ( fDecSizeX != 0.0f )
    {
        UIRECT rcLocal = GetLocalPos();
        rcLocal.left  += fDecSizeX;
        rcLocal.sizeX -= fDecSizeX;
        SetLocalPos( rcLocal );

        UIRECT rcGlobal = GetGlobalPos();
        rcGlobal.left  += fDecSizeX;
        rcGlobal.sizeX -= fDecSizeX;
        SetGlobalPos( rcGlobal );
    }
}

void CBasicGameMenu::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return ;

	m_pInfo->SetVisibleSingle( FALSE );
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CBasicGameMenu::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case MENU_INVENTORY_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( m_nOverMenu != ControlID )
				{
					m_pInfo->SetTextNoSplit ( m_ShotcutText[BASIC_MENU_INVENTORY], NS_UITEXTCOLOR::SILVER );
				}
				m_pInfo->SetVisibleSingle ( TRUE );
				m_nOverMenu = ControlID;

				if ( dwMsg & UIMSG_LB_UP )
				{
					if( GLUseFeatures::GetInstance().IsUsingRenewInvenWindow() )
					{
						if( !m_pInterface->UiIsVisibleGroup( INVENTORY_WINDOW_RENEW ) &&
							!m_pInterface->UiIsVisibleGroup ( TRADEINVENTORY_WINDOW ) )
						{
							m_pInterface->UiShowGroupFocus ( INVENTORY_WINDOW_RENEW );
						}
						else
						{
							m_pInterface->UiHideGroup ( INVENTORY_WINDOW_RENEW );
						}
					}
					else
					{
						if( !m_pInterface->UiIsVisibleGroup( INVENTORY_WINDOW ) &&
							!m_pInterface->UiIsVisibleGroup ( TRADEINVENTORY_WINDOW ) )
						{
							m_pInterface->UiShowGroupFocus ( INVENTORY_WINDOW );
						}
						else
						{
							m_pInterface->UiHideGroup ( INVENTORY_WINDOW );
						}
					}
				}
			}
		}
		break;

	case MENU_CHARACTER_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( m_nOverMenu != ControlID )
				{
					m_pInfo->SetTextNoSplit ( m_ShotcutText[BASIC_MENU_CHARACTER], NS_UITEXTCOLOR::SILVER );
				}
				m_pInfo->SetVisibleSingle ( TRUE );
				m_nOverMenu = ControlID;

				if ( dwMsg & UIMSG_LB_UP )
				{
					if ( m_pInterface->UiIsVisibleGroup( STATSRESET_WINDOW ) )
					{
						 m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::RED, ID2GAMEINTEXT("STATSRESET_ACTION_WARNING") );
					}
					else 
					{
						if ( !m_pInterface->UiIsVisibleGroup ( CHARACTER_WINDOW ) )
						{
							m_pInterface->UiShowGroupFocus ( CHARACTER_WINDOW );
						}
						else
						{
							m_pInterface->UiHideGroup ( CHARACTER_WINDOW );
						}
					}
				}
			}
		}
		break;

	case MENU_SKILL_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( m_nOverMenu != ControlID )
				{
					m_pInfo->SetTextNoSplit ( m_ShotcutText[BASIC_MENU_SKILL], NS_UITEXTCOLOR::SILVER );
				}
				m_pInfo->SetVisibleSingle ( TRUE );
				m_nOverMenu = ControlID;

				if ( dwMsg & UIMSG_LB_UP )
				{
					if ( m_pInterface->UiIsVisibleGroup( STATSRESET_WINDOW ) )
					{
						m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::RED, ID2GAMEINTEXT("STATSRESET_ACTION_WARNING") );
					}
					else 
					{
						if ( !m_pInterface->UiIsVisibleGroup ( SKILL_WINDOW ) )
						{
							m_pInterface->UiShowGroupFocus ( SKILL_WINDOW );
						}
						else
						{
							m_pInterface->UiHideGroup ( SKILL_WINDOW );
						}
					}
				}
			}
		}
		break;

	case MENU_PARTY_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( m_nOverMenu != ControlID )
				{
					m_pInfo->SetTextNoSplit ( m_ShotcutText[BASIC_MENU_PARTY], NS_UITEXTCOLOR::SILVER );
				}
				m_pInfo->SetVisibleSingle ( TRUE );
				m_nOverMenu = ControlID;

				if ( CHECK_LB_UP_LIKE( dwMsg ) )
				{
					m_pInterface->ToggleWindowParty();
				}
			}
		}
		break;

	case MENU_QUEST_BUTTON:
		{
            if ( CHECK_MOUSE_IN ( dwMsg ) )
            {
                if ( m_nOverMenu != ControlID )
                {
                    m_pInfo->SetTextNoSplit ( m_ShotcutText[BASIC_MENU_QUEST], NS_UITEXTCOLOR::SILVER );
                }
                m_pInfo->SetVisibleSingle ( TRUE );
                m_nOverMenu = ControlID;

                if ( dwMsg & UIMSG_LB_UP )
                {
                    if ( !m_pInterface->UiIsVisibleGroup ( QUEST_WINDOW ) )
                    {
                        DWORD dwEventQuestID = m_pInterface->GetEventQuestID ();
                        if ( NATIVEID_NULL().dwID == dwEventQuestID )
                        {
                            m_pInterface->ToggleWindowQuest ();						
                        }
                        else
                        {
                            m_pInterface->SetQuestWindowOpen ( dwEventQuestID );
                        }
                    }
                    else
                    {
                        m_pInterface->UiHideGroup ( QUEST_WINDOW );
                    }
                }
            }
		}
		break;

	case MENU_GUILD_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( m_nOverMenu != ControlID )
				{
					m_pInfo->SetTextNoSplit ( m_ShotcutText[BASIC_MENU_GUILD], NS_UITEXTCOLOR::SILVER );
				}
				m_pInfo->SetVisibleSingle ( TRUE );
				m_nOverMenu = ControlID;

				if ( dwMsg & UIMSG_LB_UP )
				{
					if ( !m_pInterface->UiIsVisibleGroup ( CLUB_WINDOW ) )
					{
						m_pInterface->UiShowGroupFocus ( CLUB_WINDOW );
					}
					else
					{
						m_pInterface->UiHideGroup ( POPUP_MENU );
						m_pInterface->UiHideGroup ( CLUB_WINDOW );
					}
				}
			}
		}
		break;

	case MENU_FRIEND_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( m_nOverMenu != ControlID )
				{
					m_pInfo->SetTextNoSplit ( m_ShotcutText[BASIC_MENU_FRIEND], NS_UITEXTCOLOR::SILVER );
				}
				m_pInfo->SetVisibleSingle ( TRUE );
				m_nOverMenu = ControlID;

				if ( dwMsg & UIMSG_LB_UP )
				{
					if ( !m_pInterface->UiIsVisibleGroup ( FRIEND_WINDOW ) )
					{
						m_pInterface->UiShowGroupFocus ( FRIEND_WINDOW );
					}
					else
					{
						m_pInterface->UiHideGroup ( POPUP_MENU );
						m_pInterface->UiHideGroup ( FRIEND_WINDOW );
					}
				}
			}
		}
		break;

	case MENU_LARGEMAP_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( m_nOverMenu != ControlID )
				{
					m_pInfo->SetTextNoSplit ( m_ShotcutText[BASIC_MENU_LARGEMAP], NS_UITEXTCOLOR::SILVER );
				}
				m_pInfo->SetVisibleSingle ( TRUE );
				m_nOverMenu = ControlID;

				if ( dwMsg & UIMSG_LB_UP )
				{
					if( m_pGaeaClient->IsInstantMap() )
					{
						m_pInterface->UiHideGroup ( MAP_WINDOW );
						m_pGaeaClient->PrintMsgText (NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("MINIMAP_CANNOTSEE") );
					}else{
						if ( !m_pInterface->UiIsVisibleGroup ( MAP_WINDOW ) )
						{
							m_pInterface->UiShowGroupFocus ( MAP_WINDOW );
						}
						else
						{
							m_pInterface->UiHideGroup ( MAP_WINDOW );
						}
					}
				}
			}
		}
		break;
	case MENU_CHATMACRO_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( m_nOverMenu != ControlID )
				{
					m_pInfo->SetTextNoSplit ( m_ShotcutText[BASIC_MENU_CHATMACRO], NS_UITEXTCOLOR::SILVER );
				}
				m_pInfo->SetVisibleSingle ( TRUE );
				m_nOverMenu = ControlID;

				if ( dwMsg & UIMSG_LB_UP )
				{
					if ( !m_pInterface->UiIsVisibleGroup ( CHATMACRO_WINDOW ) )
					{
						m_pInterface->GetChatMacro();
						m_pInterface->UiShowGroupFocus ( CHATMACRO_WINDOW );
					}
					else
					{
						m_pInterface->UiHideGroup ( CHATMACRO_WINDOW );
					}
				}
			}
		}
		break;

	case MENU_ITEMBANK_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( m_nOverMenu != ControlID )
				{
					m_pInfo->SetTextNoSplit ( m_ShotcutText[BASIC_MENU_ITEMBANK], NS_UITEXTCOLOR::SILVER );
				}
				m_pInfo->SetVisibleSingle ( TRUE );
				m_nOverMenu = ControlID;

				if ( dwMsg & UIMSG_LB_UP )
				{
					m_pInterface->SetItemBankWindowOpen();
				}
			}
		}
		break;

	case MENU_POINT_SHOP_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( m_nOverMenu != ControlID )
				{
					m_pInfo->SetTextNoSplit ( m_ShotcutText[BASIC_MENU_POINTSHOP], NS_UITEXTCOLOR::SILVER );
				}
				m_pInfo->SetVisibleSingle ( TRUE );
				m_nOverMenu = ControlID;

				if ( dwMsg & UIMSG_LB_UP )
				{
					if ( m_pGaeaClient->GetTutorial()->IsTutorial() ) // 튜토리얼 중에는 item shop 열지 말자.
					{
						m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("TUTORIAL_CANNOT_USE") );
					}
					else
					{
                        if ( GLogicData::GetInstance().GetServiceProvider() != SP_THAILAND )
                        {
							m_pInterface->ToggleWindowPointshop();
                        }
                        else
                        {
                            if ( !m_pInterface->UiIsVisibleGroup ( ITEMSHOP_WINDOW ) )
                            {
                                // itemShopAuth
                                if ( m_pInterface->ItemShopAuth() )
                                {
                                    m_pInterface->UiShowGroupFocus( ITEMSHOP_WINDOW );
                                }

                                m_pInterface->UiHideGroup( HELP_WINDOW );								
                            }
                            else
                            {
                                m_pInterface->UiHideGroup( ITEMSHOP_WINDOW );
                            }
                        }
					}
				}
			}
		}
		break;

    case MENU_SNS_BUTTON:
        {
            if ( CHECK_MOUSE_IN ( dwMsg ) )
            {
                if ( m_nOverMenu != ControlID )
                {
                    m_pInfo->SetTextNoSplit ( m_ShotcutText[BASIC_MENU_SNS], NS_UITEXTCOLOR::SILVER );
                }
                m_pInfo->SetVisibleSingle ( TRUE );
                m_nOverMenu = ControlID;

                if ( dwMsg & UIMSG_LB_UP )
                {
                    if ( !GLUseFeatures::GetInstance().IsUsingSNS() )
                    {
                        break;
                    }

                    if ( !m_pInterface->UiIsVisibleGroup ( SNS_WINDOW ) )
                    {
                        m_pInterface->UiShowGroupFocus ( SNS_WINDOW );						
                    }
                    else
                    {
                        m_pInterface->UiHideGroup ( SNS_WINDOW );
                    }
                }
            }
        }
        break;

	case MENU_RUN_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( m_nOverMenu != ControlID )
				{
					m_pInfo->SetTextNoSplit ( m_ShotcutText[BASIC_MENU_RUN], NS_UITEXTCOLOR::SILVER );
				}
				m_pInfo->SetVisibleSingle ( TRUE );
				m_nOverMenu = ControlID;

				if ( UIMSG_LB_UP & dwMsg )
				{
					m_pInterface->ReqToggleRun ();
				}
			}
		}
		break;
		
	case MENU_CLOSE_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( CHECK_LB_UP_LIKE ( dwMsg ) )
				{
					m_pInterface->UiHideGroup ( GetWndID () );
					m_pInterface->UiShowGroupBottom ( GAME_MENU_OPEN_BUTTON, true );
				}
			}
		}
		break;
	}
}

void CBasicGameMenu::InitShotCutString()
{
	int i=0;
	int nIndex = 0;
	
	for ( i=0; i< BASIC_MENU_NUM; ++i)
	{	
		m_ShotcutText[i] = ID2GAMEWORD ("GAMEMENU", nMenuTextIndex[i]);

		std::string strShortcuts;

		const SSHORTCUTS& sShortcuts = GLInputManager::GetInstance().GetShortcuts(static_cast<EMSHORTCUTS>(nMenuShotcut[i]));

		SSHORTCUTS::VEC_KEY_CITER iter    = sShortcuts.IterBegin();
		SSHORTCUTS::VEC_KEY_CITER iterEnd = sShortcuts.IterEnd();

		for ( ; iter!=iterEnd; ++iter )
		{
			const SSHORTCUTS::KEY& sKey = (*iter);

			if ( sKey.emTYPE == EMKEYTYPE_KEYBOARD )
			{
				std::string strTemp;
				strTemp = m_pInterface->GetdwKeyToString( static_cast<int>(sKey.nVALUE) );

				if ( !strShortcuts.empty() )
				{
					strShortcuts += "+";
				}

				strShortcuts += strTemp;
			}
			else
			{
				continue;
			}
		}
		
		CString strTempShortcuts(strShortcuts.c_str());
		SetShotcutText(i, strTempShortcuts);
	}
 
}

void CBasicGameMenu::SetShotcutText ( DWORD nID, CString& strTemp )
{
	CString ShotcutTemp = m_ShotcutText[nID];

	ShotcutTemp = ShotcutTemp.Left(ShotcutTemp.Find("("));
	ShotcutTemp += "(" + strTemp + ")";
	m_ShotcutText[nID] = ShotcutTemp;	
}

