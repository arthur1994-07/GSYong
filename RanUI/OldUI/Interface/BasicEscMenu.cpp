#include "StdAfx.h"
#include "BasicEscMenu.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxEx.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DeviceMan.h"

#include "../../InnerInterface.h"
//#include "../../ModalCallerID.h"
#include "../../../RanLogicClient/Stage/DxGlobalStage.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CBasicEscMenu::CBasicEscMenu(CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pInterface(pInterface)
{
}

CBasicEscMenu::~CBasicEscMenu ()
{
}

void CBasicEscMenu::CreateSubControl ()
{
	EMSERVICE_PROVIDER  ServiceProvider = m_pEngineDevice->GetServiceProvider();

	CBasicLineBoxEx* pBasicLineBox = new CBasicLineBoxEx(m_pEngineDevice);
	pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_EX_ESC", UI_FLAG_XSIZE | UI_FLAG_YSIZE );

	/*if ( GLUseFeatures::GetInstance().IsUsingJoinWorldBattle() &&
		false == GLUseFeatures::GetInstance().IsUsingWorldBattle() )
		pBasicLineBox->CreateBaseBoxESCMenu ( "ESC_MENU_BACK_PVP_SERVER" );
	else*/
		pBasicLineBox->CreateBaseBoxESCMenu ( "ESC_MENU_BACK" );

	RegisterControl ( pBasicLineBox );

	//	버튼 만들기
//	CreateTextButton ( "ESC_MENU_CHARACTER_CHANGE", ESC_MENU_CHARACTER_CHANGE, (char*)ID2GAMEWORD ( "ESC_MENU", 0 ) );
	CreateTextButton( "ESC_MENU_SERVER_CHANGE", ESC_MENU_SERVER_CHANGE, (char*)ID2GAMEWORD( "ESC_MENU", 1 ) );
	CreateTextButton( "ESC_MENU_HWOPTION", ESC_MENU_HWOPTION, (char*)ID2GAMEWORD( "ESC_MENU", 2 ) );
	CreateTextButton( "ESC_MENU_HELP", ESC_MENU_HELP, (char*)ID2GAMEWORD( "ESC_MENU", 3 ) );				
	CreateTextButton( "ESC_MENU_KEYSETTING", ESC_MENU_KEYSETTING, (char*)ID2GAMEWORD( "ESC_MENU", 5 ) );
	CreateTextButton( "ESC_MENU_CLOSEGAME", ESC_MENU_CLOSEGAME, (char*)ID2GAMEWORD( "ESC_MENU", 4 ) );

//	게임위 등급 표시
//#if defined(_RELEASED) || defined ( KR_PARAM ) || defined ( KRT_PARAM )
    
    if (ServiceProvider == SP_OFFICE_TEST || ServiceProvider == SP_KOREA || ServiceProvider == SP_KOREA_TEST)
    {
	    CreateTextButton( "ESC_MENU_GRADE_DISPLAY", ESC_MENU_GRADE_DISPLAY, (char*)ID2GAMEWORD( "ESC_MENU", 6 ) );
        CreateTextButton( "ESC_MENU_CUSTOMER_CENTER", ESC_MENU_CUSTOMER_CENTER, (char*)ID2GAMEWORD( "ESC_MENU", 7 ) );
    }
	if (ServiceProvider == SP_OFFICE_TEST || ServiceProvider == SP_KOREA || ServiceProvider == SP_KOREA_TEST )
		CreateTextButton( "ESC_MENU_OPENFORUM", ESC_MENU_OPENFORUM, (char*)ID2GAMEWORD( "ESC_MENU", 9 ) );

	/*if ( ServiceProvider == SP_OFFICE_TEST || ServiceProvider == SP_GS )
	{
		if ( GLUseFeatures::GetInstance().IsUsingJoinWorldBattle() &&
			false == GLUseFeatures::GetInstance().IsUsingWorldBattle() )
			CreateTextButton( "ESC_MENU_JOIN_PVP_SERVER", ESC_MENU_JOIN_PVP_SERVER, (char*)ID2GAMEWORD( "ESC_MENU", 8 ) );
	}*/
//#endif 
	//CreateFlipButton ( "ESC_MENU_CLOSE", "ESC_MENU_CLOSE_F", ESC_MENU_CLOSE );
}

CBasicButton* CBasicEscMenu::CreateFlipButton ( char* szButton, char* szButtonFlip, UIGUID ControlID )
{
	CBasicButton* pButton = new CBasicButton(m_pEngineDevice);
	pButton->CreateSub ( this, szButton, UI_FLAG_DEFAULT, ControlID );
	pButton->CreateFlip ( szButtonFlip, CBasicButton::CLICK_FLIP );
	pButton->SetUseDynamic ( TRUE );
	RegisterControl ( pButton );

	return pButton;
}

void CBasicEscMenu::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	const float fBUTTON_CLICK = CBasicButton::fBUTTON_CLICK;

	switch ( ControlID )
	{
	//case ESC_MENU_CHARACTER_CHANGE:
	//	{
	//		if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
	//		{
	//			GASSERT ( 0 && "캐릭터 선택" );
	//		}
	//	}
	//	break;

	case ESC_MENU_SERVER_CHANGE:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
                if ( RANPARAM::bCheckAllDelItemsRebuyPage )
                {
                    int nRebuyItemCount = m_pInterface->GetDialogueWindowRebuyItemCount();

                    if ( nRebuyItemCount <= 0 )
                        m_pInterface->DoModal( ID2GAMEINTEXT( "MODAL_ESC_SERVER_STAGE" ), UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_SERVER_STAGE );
                    else
                    {
                        std::string strCheckRebuyItems =
                            sc::string::format( ID2GAMEINTEXT( "MODAL_ESC_SERVER_STAGE_CHECK_REBUY_ITEMS" ),
                            nRebuyItemCount );
                        m_pInterface->DoModal( strCheckRebuyItems.c_str(), UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_SERVER_STAGE );
                    }
                }
                else
				    m_pInterface->DoModal( ID2GAMEINTEXT( "MODAL_ESC_SERVER_STAGE" ), UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_SERVER_STAGE );

				m_pInterface->UiHideGroup( GetWndID() );
			}
		}
		break;

	case ESC_MENU_HELP:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				//	TODO : 모달 박스 출력 필요.
				//
				m_pInterface->UiShowGroupFocus( HELP_WINDOW );
				m_pInterface->UiHideGroup( GetWndID() );
			}
		}
		break;

	case ESC_MENU_HWOPTION:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				m_pInterface->UiShowGroupFocus( OPTION_HW_WINDOW );
				m_pInterface->UiHideGroup( GetWndID() );
			}
		}
		break;

	case ESC_MENU_CLOSEGAME:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
                if ( RANPARAM::bCheckAllDelItemsRebuyPage )
                {
                    int nRebuyItemCount = m_pInterface->GetDialogueWindowRebuyItemCount();

                    if ( nRebuyItemCount <= 0 )
				        m_pInterface->DoModal( ID2GAMEINTEXT( "MODAL_ESC_MENU_CLOSE" ), UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_CLOSEGAME );
                    else
                    {
                        std::string strCheckRebuyItems =
                            sc::string::format( ID2GAMEINTEXT( "MODAL_ESC_MENU_CLOSE_CHECK_REBUY_ITEMS" ),
                            nRebuyItemCount );
                        m_pInterface->DoModal( strCheckRebuyItems.c_str(), UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_CLOSEGAME );
                    }
                }
                else
                    m_pInterface->DoModal( ID2GAMEINTEXT( "MODAL_ESC_MENU_CLOSE" ), UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_CLOSEGAME );

				m_pInterface->UiHideGroup( GetWndID() );
			}
		}
		break;

	case ESC_MENU_KEYSETTING:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				m_pInterface->GetShotCutKey();
				m_pInterface->UiShowGroupFocus( KEY_SETTING_WINDOW );
				m_pInterface->UiHideGroup( GetWndID() );
			}
		}
		break;

	case ESC_MENU_CLOSE:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				m_pInterface->UiHideGroup( GetWndID() );
			}
		}
		break;
	case ESC_MENU_GRADE_DISPLAY:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				m_pInterface->UiShowGroupFocus( GRADE_DISPLAY );
				m_pInterface->UiHideGroup( GetWndID() );
			}
		}
		break;

    case ESC_MENU_CUSTOMER_CENTER:
        {
            if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
            {
                m_pInterface->DoModal( ID2GAMEINTEXT("CONFIRM_CUSTOMER_CENTER"), UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_CONFIRM_CUSTOMER_CENTER );

                m_pInterface->UiHideGroup( GetWndID() );
            }
        }
        break;
	case ESC_MENU_OPENFORUM:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				CWnd* pWnd = ::AfxGetMainWnd();
				HWND hWnd = NULL;
				if ( pWnd)
					hWnd = pWnd->m_hWnd;
				if ( hWnd )
					::ShowWindow( hWnd, SW_MINIMIZE );

				::ShellExecute(NULL, "open", RANPARAM::strForumURL, NULL, NULL, SW_SHOW);
			}
		}
		break;

	/*case ESC_MENU_JOIN_PVP_SERVER:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				WorldBattleSystem::GLWorldBattleManClient* pWorldBattleMan =
					WorldBattleSystem::GLWorldBattleManClient::Instance();
				if ( NULL == pWorldBattleMan )
					return;

				if ( pWorldBattleMan->IsUsingJoinWorldBattle() )
				{
					m_pInterface->DoModal(
						ID2GAMEINTEXT("CONFIRM_JOIN_PVP_SERVER"),
						UI::MODAL_QUESTION,
						UI::YESNO,
						UI::MODAL_CONFIRM_JOIN_PVP_SERVER );

					m_pInterface->UiHideGroup( GetWndID() );
				}
			}
		}
		break;*/
	}
}

CBasicTextButton*	 CBasicEscMenu::CreateTextButton ( char* szButton, UIGUID ControlID, char* szText )
{
	CBasicTextButton* pTextButton = new CBasicTextButton(m_pEngineDevice);
	pTextButton->CreateSub ( this, "BASIC_TEXT_BUTTON22", UI_FLAG_XSIZE, ControlID );
	pTextButton->CreateBaseButton ( szButton, 
									CBasicTextButton::SIZE22, 
									CBasicButton::CLICK_FLIP, 
									szText,
									_DEFAULT_FONT_SHADOW_EX_FLAG );
	pTextButton->SetFlip ( TRUE );
	RegisterControl ( pTextButton );

	return pTextButton;
}