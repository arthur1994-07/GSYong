#include "StdAfx.h"
#include "GamePlayOption.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicComboBox.h"
#include "../../../EngineLib/GUInterface/BasicComboBoxRollOver.h"
#include "../../../RanLogicClient/Tool/DxParamSet.h"
#include "../../InnerInterface.h"
//#include "../../ModalCallerID.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void CBasicGamePlayOption::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	
	case HWOPTION_GAMEPLAY_CONFT_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				//DXPARAMSET::GetInstance().m_bDIS_CONFT = !DXPARAMSET::GetInstance().m_bDIS_CONFT;
				//LoadConft ();
				m_bConft = !m_bConft;
				m_pConftButton->SetFlip( m_bConft );
			}
		}
		break;

	case HWOPTION_GAMEPLAY_TRADE_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				//DXPARAMSET::GetInstance().m_bDIS_TRADE = !DXPARAMSET::GetInstance().m_bDIS_TRADE;
				//LoadTrade ();
				m_bTrade = !m_bTrade;
				m_pTradeButton->SetFlip ( m_bTrade );
			}
		}
		break;

	case HWOPTION_GAMEPLAY_PARTY_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				//DXPARAMSET::GetInstance().m_bDIS_PARTY = !DXPARAMSET::GetInstance().m_bDIS_PARTY;
				//LoadParty ();
				m_bParty = !m_bParty;
				m_pPartyButton->SetFlip ( m_bParty );
			}
		}
		break;

	case HWOPTION_GAMEPLAY_SIMPLEHP_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				//DXPARAMSET::GetInstance().m_bSHOW_SIMPLEHP = !DXPARAMSET::GetInstance().m_bSHOW_SIMPLEHP;
				//LoadSimpleHP ();
				m_bSimpleHP = !m_bSimpleHP;
				m_pSimpleHPButton->SetFlip ( m_bSimpleHP );
			}
		}
		break;

	case HWOPTION_GAMEPLAY_FRIEND_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				//RANPARAM::bDIS_FRIEND = !RANPARAM::bDIS_FRIEND;
				//LoadFriend ();
				m_bFriend = !m_bFriend;
				m_pFriendButton->SetFlip ( m_bFriend );
			}
		}
		break;

	case HWOPTION_GAMEPLAY_DEFAULTPOS_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_pInterface->DoModal ( ID2GAMEINTEXT("DEFAULT_POSITION"), UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_DEFAULT_POS );					
			}
		}
		break;

	case HWOPTION_GAMEPLAY_MOVABLE_ON_CHAT_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				//RANPARAM::bMOVABLE_ON_CHAT = !RANPARAM::bMOVABLE_ON_CHAT;
				//LoadMovableOnChat ();
				m_bMovableOnChat = !m_bMovableOnChat;
				m_pMovableOnChatButton->SetFlip ( m_bMovableOnChat );
			}
		}
		break;

	case HWOPTION_GAMEPLAY_SHOW_TIP_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				//RANPARAM::bSHOW_TIP = !RANPARAM::bSHOW_TIP;					
				//LoadShowTip ();
				m_bShowTip = !m_bShowTip;
				m_pShowTipButton->SetFlip ( m_bShowTip );
			}
		}
		break;

	case HWOPTION_GAMEPLAY_FORCED_ATTACK_BUTTON:			// 강제 공격
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				//RANPARAM::bFORCED_ATTACK = !RANPARAM::bFORCED_ATTACK;
				//LoadForcedAttack();
				m_bForcedAttack = !m_bForcedAttack;
				m_pForcedAttackButton->SetFlip ( m_bForcedAttack );
			}
		}
		break;

	case HWOPTION_GAMEPLAY_NAME_DISPLAY_BUTTON:			// 이름 항상 표시
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				//RANPARAM::bNAME_DISPLAY = !RANPARAM::bNAME_DISPLAY;
				//LoadNameDisplay();
				m_bNameDisplay = !m_bNameDisplay;
				m_pNameDisplayButton->SetFlip ( m_bNameDisplay );
			}
		}
		break;

	case HWOPTION_GAMEPLAY_SIMPLE_SUMMON_NAME: // 소환수 이름 단순화
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				//RANPARAM::bNAME_DISPLAY = !RANPARAM::bNAME_DISPLAY;
				//LoadNameDisplay();
				m_bSimpleSummonName = !m_bSimpleSummonName;
				m_pSimpleSummonNameButton->SetFlip ( m_bSimpleSummonName );
			}
		}
		break;

    case HWOPTION_GAMEPLAY_AUTO_CROW_HPBAR: // 몬스터 HP바 자동표시
        {
            if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
            {
                m_bAutoCrowHPBar = !m_bAutoCrowHPBar;
                m_pAutoCrowHPBarButton->SetFlip ( m_bAutoCrowHPBar );
            }
        }
        break;

	case HWOPTION_GAMEPLAY_SIMPLE_COMBO_ROLLOVER:
		{
			if ( dwMsg & UI_MSG_COMBOBOX_ROLLOVER_SCROLL )
			{				
				DWORD dwMsg = GetMessageEx ();
				dwMsg &= ~UI_MSG_COMBOBOX_ROLLOVER_SCROLL;
				ResetMessageEx ();
				AddMessageEx ( dwMsg );

				break;
			}

			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{		
				int nIndex = m_pComboBoxSimpleRollOver->GetSelectIndex ();
				if ( nIndex < 0 ) return ;

				m_dwAroundDetail = nIndex;

				CString strTemp( ID2GAMEWORD ( "HWOPTION_GAMEPLAY_SIMPLE", nIndex ) );
				m_pComboBoxSimpleOpen->SetText ( strTemp );
			}
		}
		break;

	case HWOPTION_GAMEPLAY_SIMPLE_COMBO_OPEN:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( dwMsg & UIMSG_LB_DOWN )
				{			
					m_pComboBoxSimpleRollOver->SetVisibleSingle ( TRUE );
					m_pComboBoxSimpleRollOver->SetScrollPercent ( 0.0f );

					m_RollOverID = HWOPTION_GAMEPLAY_SIMPLE_COMBO_ROLLOVER;
					m_bFirstLBUP = TRUE;
				}
			}
		}
		break;

	case HWOPTION_GAMEPLAY_SKILLTYPE_COMBO_ROLLOVER:
		{
			if ( dwMsg & UI_MSG_COMBOBOX_ROLLOVER_SCROLL )
			{				
				DWORD dwMsg = GetMessageEx ();
				dwMsg &= ~UI_MSG_COMBOBOX_ROLLOVER_SCROLL;
				ResetMessageEx ();
				AddMessageEx ( dwMsg );

				break;
			}

			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{		
				int nIndex = m_pComboSkillTypeRollOver->GetSelectIndex ();
				if ( nIndex < 0 ) return ;

				m_dwSkillTypeUI = nIndex;

				CString strTemp( ID2GAMEWORD ( "HWOPTION_GAMEPLAY_SKILLTYPE", nIndex ) );
				m_pComboSkillTypeOpen->SetText ( strTemp );
			}
		}
		break;

	case HWOPTION_GAMEPLAY_SKILLTYPE_COMBO_OPEN:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( dwMsg & UIMSG_LB_DOWN )
				{			
					m_pComboSkillTypeRollOver->SetVisibleSingle ( TRUE );
					m_pComboSkillTypeRollOver->SetScrollPercent ( 0.0f );

					m_RollOverID = HWOPTION_GAMEPLAY_SKILLTYPE_COMBO_ROLLOVER;
					m_bFirstLBUP = TRUE;
				}
			}
		}
		break;

	case HWOPTION_GAMEPLAY_VEHICLE_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				//RANPARAM::bDIS_FRIEND = !RANPARAM::bDIS_FRIEND;
				//LoadFriend ();
				m_bVehicle = !m_bVehicle;
				m_pVehicleButton->SetFlip ( m_bVehicle );
			}
		}
		break;

	case HWOPTION_GAMEPLAY_CHAR_INFO_PRIVATE:
		{
			if( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				if( GLUseFeatures::GetInstance().IsUsingReferChar() )
				{
					m_bCharInfoPrivate = !m_bCharInfoPrivate;
					m_pCharInfoPrivateButton->SetFlip( m_bCharInfoPrivate );
				}
			}
		}
		break;

	case HWOPTION_GAMEPLAY_CHAR_INFO_ADDITION:
		{
			if( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) && GLCONST_CHAR::bAdditional_ability )
			{
				m_bCharInfoAddition = !m_bCharInfoAddition;
				m_pCharInfoAdditionButton->SetFlip( m_bCharInfoAddition );
			}
		}
		break;

	case HWOPTION_GAMEPLAY_GROUP_CHAT_BUTTON:
		{
			if( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				if( GLUseFeatures::GetInstance().IsUsingGroupChat() )
				{
					m_bGroupChatInvite = !m_bGroupChatInvite;
					m_pGroupChatInvateButton->SetFlip( m_bGroupChatInvite );
				}
			}
		}
		break;

	case HWOPTION_GAMEPLAY_AUTOREBIRTH:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
				m_pAutoRebirthButton->SetFlip((m_bAutoRebirth = !m_bAutoRebirth));
		}
		break;
	}
}