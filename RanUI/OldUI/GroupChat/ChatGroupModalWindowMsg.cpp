#include "StdAfx.h"
#include "../GroupChat/ChatGroupModalWindow.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicComboBox.h"
#include "../../../EngineLib/GUInterface/BasicComboBoxRollOver.h"
#include "../../InnerInterface.h"
#include "../../../RanLogic/RANPARAM.h"
#include "../../../RanLogic/GLogicData.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void ChatGroupModalWindow::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindow::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case CHAT_GROUP_ITEM_COMBO_OPEN:
		{
			if ( !m_bFIX )	//	신청시에만 동작
			{
				if ( CHECK_MOUSE_IN ( dwMsg ) )
				{
					if ( dwMsg & UIMSG_LB_DOWN )
					{
						m_pComboBoxItemRollOver->SetVisibleSingle ( TRUE );
						m_pComboBoxItemRollOver->SetScrollPercent ( 0.0f );
						m_pComboBoxMoneyRollOver->SetVisibleSingle ( FALSE );					

						m_RollOverID = CHAT_GROUP_MODAL_ITEM_COMBO_ROLLOVER;
						m_bFirstLBUP = TRUE;
					}
				}
			}
		}
		break;

	case CHAT_GROUP_MODAL_ITEM_COMBO_ROLLOVER:
		{
			if ( !m_bFIX )	//	신청시에만 동작
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
					int nIndex = m_pComboBoxItemRollOver->GetSelectIndex ();
					if ( nIndex < 0 ) return ;
					RANPARAM::dwPARTY_GET_ITEM = nIndex+1;
					LoadItem ();
				}
			}
		}
		break;

	case CHAT_GROUP_MODAL_MONEY_COMBO_OPEN:
		{
			if ( !m_bFIX )	//	신청시에만 동작
			{
				if ( CHECK_MOUSE_IN ( dwMsg ) )
				{
					if ( dwMsg & UIMSG_LB_DOWN )
					{
						m_pComboBoxItemRollOver->SetVisibleSingle ( FALSE );
						m_pComboBoxMoneyRollOver->SetVisibleSingle ( TRUE );
						m_pComboBoxMoneyRollOver->SetScrollPercent ( 0.0f );

						m_RollOverID = CHAT_GROUP_MODAL_MONEY_COMBO_ROLLOVER;
						m_bFirstLBUP = TRUE;
					}
				}
			}
		}
		break;

	case CHAT_GROUP_MODAL_MONEY_COMBO_ROLLOVER:
		{
			if ( !m_bFIX )	//	신청시에만 동작
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
					if ( m_bReqModal )
					{
						int nIndex = m_pComboBoxMoneyRollOver->GetSelectIndex ();
						if ( nIndex < 0 ) 
							return ;
						RANPARAM::dwPARTY_GET_MONEY = nIndex+1;
						LoadMoney ();
					}					
				}
			}
		}
		break;

	case MODAL_CHAT_GROUP_YES:
	case MODAL_CHAT_GROUP_OK:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				//	메시지
				//
				AddMessageEx ( UIMSG_MODAL_OK );
				m_pInterface->UiHideGroup ( GetWndID () );
			}
		}
		break;

	case MODAL_CHAT_GROUP_NO:
	case MODAL_CHAT_GROUP_CANCEL:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				//	메시지
				//
				AddMessageEx ( UIMSG_MODAL_CANCEL );
				m_pInterface->UiHideGroup ( GetWndID () );
			}
		}
		break;
	}
}