#include "StdAfx.h"

#include "InnerInterface.h"

#include "../SigmaCore/String/StringFormat.h"
#include "../EngineLib/DxTools/DxViewPort.h"
#include "../EngineLib/GUInterface/GameTextControl.h"
#include "../EngineLib/GUInterface/UITextControl.h"
#include "../EngineLib/GUInterface/NSGUI.h"
#include "../RanLogicClient/GLGaeaClient.h"
#include "../MfcExLib/RanFilter.h"
#include "../RanLogic/RANPARAM.h"

#include "OldUI/Interface/UILeftTopGroup.h"
#include "OldUI/Interface/AdminMessageDisplay.h"
#include "OldUI/Interface/BasicChat.h"
#include "OldUI/Interface/BasicChatRightBody.h"
#include "OldUI/Interface/BasicInfoView.h"
#include "OldUI/Interface/BasicSkillTray.h"
#include "OldUI/Interface/MiniMap.h"
#include "OldUI/Interface/SkillTimeUnit.h"
#include "OldUI/Interface/ChatType.h"
#include "OldUI/Club/ClubWindow.h"
#include "OldUI/Club/SubMasterSet.h"
#include "OldUI/Util/ModalWindow.h"
#include "OldUI/Friend/FriendWindow.h"
#include "OldUI/Display/HeadChatDisplayMan.h"
#include "OldUI/Competition/CompetitionNotifyButton.h"
#include "OldUI/Item/InventoryPageWear.h"
#include "OldUI/Item/InventoryPageWearEx.h"
#include "OldUI/Item/ItemSlot.h"
#include "OldUI/Confront/ConftModalWindow.h"
#include "OldUI/Party/PartyModalWindow.h"
#include "OldUI/PrivateMarket/PrivateMarketSellWindow.h"
#include "OldUI/PrivateMarket/PrivateMarketShowMan.h"
#include "OldUI/Message/RebirthDialogue.h"
#include "OldUI/Message/WaitServerDialogue.h"
#include "OldUI/Skill/SkillWindowToTray.h"

#include "../RanLogic/Prison/LimitPrison.h"
#include "../RanLogicClient/Land/GLLandManClient.h"
//-----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
//-----------------------------------------------------------------------------


void CInnerInterface::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{	
	BOOL bMOUSEIN_LBUP = FALSE;

	if ( CHECK_MOUSE_IN ( dwMsg ) )
	{
		switch ( ControlID )
		{
		case ITEM_MOVE:
		case ADMIN_MESSAGE_DISPLAY:
		case DAMAGE_MAN:
		case MAPMOVE_DISPLAY:
		case CONFT_DISPLAY_MAN:
		case HEADCHAT_MAN:
		case NAME_DISPLAY_MAN:		
		case LEFTTOP_CONTROL_GROUP:
		case ACADEMY_CONFT_DISPLAY:		
		case SIMPLE_HP:
		case SIMPLE_MESSAGE_MAN:		
		case QUESTION_ITEM_DISPLAY:
		case CONFT_CONFIRM:
		case WARNING_MSG_WINDOW:
		case QUEST_HELPER_DISPLAY:
		case ITEM_SHOP_ICON_MAN:
		case CDM_RANKING_DISPLAY:
		case SKILL_TIME_DISPLAY:
        case LUNCHBOX_TIME_DISPLAY:
        case SYSTEMBUFF_TIME_DISPLAY:
        case PROGRESS_DISPLAY:
        case INSTANCE_SYSTEM_MESSAGE_DISPLAY:
        case GUIDANCE_PROGRESS_INFO_UI:
        case GUIDANCE_RANK_INFO_UI:
        case COMPETITION_TOURNAMENTENT_INDICATOR:
			break;
		// by luxes.
		case TUTORIAL_VIEW_STEP:
			{
				NSGUI::ResetCharMoveBlock();
			}
			break;
		//

		case MINIMAP:
			{
 				if ( UIMSG_MOUSEIN_FULLSCREENBUTTON & dwMsg )
				{
					NSGUI::SetCharMoveBlock ();
				}
			}
			break;

		case WAITSERVER_DISPLAY:
			{
				int nTYPE = m_pWaitServerDisplay->GetActionTYPE();
				if ( nTYPE == WAITSERVER_TOFRIEND )
				{
					NSGUI::SetCharMoveBlock ();
				}

				if ( UIMSG_MOUSEIN_OKBUTTON & dwMsg )
				{
					NSGUI::SetCharMoveBlock ();
				}
			}
			break;

		case BASIC_CHAT_BOX:
			{
				if ( UIMSG_MOUSEIN_BLOCK_CHARACTER_MOVE & dwMsg )
				{
					NSGUI::SetCharMoveBlock ();
				}

				if ( RANPARAM::bMOVABLE_ON_CHAT )
				{
					//	m_bFirstControl�� �ƴ϶��,
					//	Ű�� ��������, TranslateMessage.. �� �� �Լ� ������
					//	������ �ʴ´�.
					//	����, ä�ùڽ������� ��Ŭ���� �� ���,
					//	m_bFirstControl�� ä�ùڽ��̹Ƿ�,
					//	�Ʒ��� VisibleTest�� �ϴ� ��Ʈ�ѵ��� �ƹ� ����� ���Եȴ�.
					//	�̷����, ĳ���Ͱ� �Ʒ��� ��Ʈ�ѵ��� ��������������
					//	�پ�ٴϰ� �ȴ�.
					if( IsOpenWindowToMoveBlock() )
					{
						NSGUI::SetCharMoveBlock ();
					}
				}
				else
				{
					NSGUI::SetCharMoveBlock ();
				}
			}
			break;

		default:
			{
				NSGUI::SetCharMoveBlock ();
				SetFirstItemSlot ();
			}
		}

		if ( CHECK_LB_UP_LIKE ( dwMsg ) )
		{
			bMOUSEIN_LBUP = TRUE;
		}
	}

	if( ControlID >= INVEN_ITEM_TOOLTIP_BEGIN && ControlID <= INVEN_ITEM_TOOLTIP_END )
	{
        m_pInvenItemTooltipGroup->TranslateMessage( ControlID, dwMsg );
	}
    
    if( ControlID >= WISH_ITEM_TOOLTIP_BEGIN && ControlID <= WISH_ITEM_TOOLTIP_END )
    {
        m_pWishItemTooltipGroup->TranslateMessage( ControlID, dwMsg );
    }

    if( ControlID >= MARKET_WISH_ITEM_TOOLTIP_BEGIN && ControlID <= MARKET_WISH_ITEM_TOOLTIP_END )
    {
        m_pMarketWishItemTooltipGroup->TranslateMessage( ControlID, dwMsg );
    }


	if( ControlID >= SKILL_TOOLTIP_BEGIN && ControlID <= SKILL_TOOLTIP_END )
	{
		UINT nIndex = ControlID - SKILL_TOOLTIP_BEGIN;
		m_vecSkillToolTipRegist[ nIndex ].pControl->TranslateMessage( dwMsg );
	}

	if ( MODAL_WINDOW <= ControlID ) //	NOTE :���ó��
	{
		// MEMO : �Ʒ� �Լ��� ���� ��� ������ ���� ���̴�. �ϴ� ����...
		//CModalWindow* pModalWindow = (CModalWindow*)FindControl( ControlID );
		if ( m_pModalWindow )
		{
			UIGUID CallerID = m_pModalWindow->GetCallerID ();
			ModalMsgProcess ( CallerID, dwMsg );

			if ( !m_pModalWindow->IsUseModaless () )
				NSGUI::SetCharMoveBlock ();
		}
		return;
	}

	switch ( ControlID )
	{
	case LEFTTOP_CONTROL_GROUP:
		{
			if ( UIMSG_MOUSE_IN & dwMsg )
			{
				NSGUI::SetCharMoveBlock ();
			}
		}
		break;

	case GAME_MENU_OPEN_BUTTON:
		{
			if ( bMOUSEIN_LBUP )
			{
				UiHideGroup ( ControlID );
				UiShowGroupBottom ( GAME_MENU, true );
			}
		}
		break;

	case MINIPARTY_OPEN:
		{
			if ( bMOUSEIN_LBUP )
            {
                //��Ƽ���������� �˻��Ͽ� �̴������� ���ų� �ݱ�
                const GLPartyID& MyPartyID = m_pGaeaClient->GetMyPartyClient()->GetPartyID();
                const bool  bIsRenewal = GLUseFeatures::GetInstance().IsUsingPartyWindowRenewal();
                if ( !bIsRenewal )  //���� ��Ƽâ
                {
                    UiIsVisibleGroup( MINIPARTY_WINDOW );
                    UiHideGroup( MINIPARTY_OPEN );
                }
                else if ( UiIsVisibleGroup( MINI_EXPEDITION_WINDOW ) )  //������ ��Ƽâ �������� ��
                {
                    UiHideGroup( MINI_EXPEDITION_WINDOW );
                }
                else    //������ ��Ƽâ �������� ��
                {
                    UiShowGroupFocus( MINI_EXPEDITION_WINDOW );
                }
			}
		}
		break;

	case QUEST_ALARM:
		{
			if ( bMOUSEIN_LBUP )
			{
				// [12/26/2012 gbgim]
				//UiHideGroup ( ControlID );

//				#ifdef CH_PARAM // �߱� �������̽� ����
//					UpdatePotionTrayPosition();
//				#endif
			
				// [12/27/2012 gbgim]
// 				if ( NATIVEID_NULL().dwID == m_dwEventQuestID )
// 				{
// 					UiShowGroupFocus ( QUEST_WINDOW );
// 					REFRESH_QUEST_WINDOW ();					
// 				}
// 				else
// 				{
// 					SetQuestWindowOpen ( m_dwEventQuestID );
// 				}
				if( UiIsVisibleGroup(QUEST_WINDOW) )
					UiHideGroup(QUEST_WINDOW);
				else
				{
					if ( NATIVEID_NULL().dwID == m_dwEventQuestID )
					{
						//UiShowGroupFocus( QUEST_WINDOW );
						REFRESH_QUEST_WINDOW ();
					}
					SetQuestWindowOpen( m_dwEventQuestID );
				}
			}
		}
		break;

	case QUICK_SKILL_TRAY_TAB_WINDOW:
		{
			ISkillWindowToTray* pSkillWindowToTray = GetSkillWindowToTray();
			if ( pSkillWindowToTray )
			{
				pSkillWindowToTray->SetUseSnap ( (dwMsg & UIMSG_MOUSEIN_SKILLSLOT) ? TRUE : FALSE );
			}
		}
		break;

	case TRADE_WINDOW:
	case TRADEINVENTORY_WINDOW:
//	case MARKET_WINDOW:	
//	case STORAGE_WINDOW:
//	case CLUB_STORAGE_WINDOW:
//	case ITEMBANK_WINDOW:
		{			
			BOOL bSnap = (dwMsg & UIMSG_MOUSEIN_WEARSLOT) || (dwMsg & UIMSG_MOUSEIN_ITEMSLOT);
			if ( bSnap )
				SetSnapItem ();

			NSGUI::SetCharMoveBlock ();
		}
		break;
        
	case VEHICLE_WINDOW:
	case PET_WINDOW:
	case INVENTORY_WINDOW:
	case INVENTORY_WINDOW_RENEW:
		{
			BOOL bSnap = (dwMsg & UIMSG_MOUSEIN_WEARSLOT) || 
						 (dwMsg & UIMSG_MOUSEIN_WEARSLOTEX) ||
						 (dwMsg & UIMSG_MOUSEIN_ITEMSLOT);
			if ( bSnap ) SetSnapItem ();
		}
		break;

	//case DIALOGUE_WINDOW:
	//	{
	//		NSGUI::SetCharMoveBlock ();
	//	}
	//	break;

	case CONFT_MODAL_WINDOW:
		{			
			if ( m_pConftModalWindow )
			{
				UIGUID CallerID = m_pConftModalWindow->GetCallerID();
				ModalMsgProcess ( CallerID, dwMsg );
			}
//			NSGUI::SetCharMoveBlock ();
		}
		break;

	case PARTY_MODAL_WINDOW:
		{			
			if ( m_pPartyModalWindow )
			{
				UIGUID CallerID = m_pPartyModalWindow->GetCallerID ();
				ModalMsgProcess ( CallerID, dwMsg );
			}
//			NSGUI::SetCharMoveBlock ();
		}
		break;

	case CHAT_GROUP_MODAL_WINDOW:
		{			
			if ( m_pChatGroupModalWindow )
			{
				UIGUID CallerID = m_pChatGroupModalWindow->GetCallerID ();
				ModalMsgProcess ( CallerID, dwMsg );
			}
			//			NSGUI::SetCharMoveBlock ();
		}
		break;
		/*
	case PET_REBIRTH_DIALOGUE: //Monster7j
		{
			if(m_pPetRebirthDialogue)
			{

			}
		}
		break;
		*/
	case REBIRTH_DIALOGUE:
		{
			if ( m_pRebirthDialogue )
			{
				const UIGUID cID = m_pRebirthDialogue->GetCallerID ();
				ModalMsgProcess ( cID, dwMsg );
			}

			NSGUI::SetCharMoveBlock ();
		}
		break;

	case PRIVATE_MARKET_SELL_WINDOW:
		{
			if ( m_pPrivateMarketSellWindow )
			{
				const UIGUID& CallerID = m_pPrivateMarketSellWindow->GetCallerID ();
				ModalMsgProcess ( CallerID, dwMsg );
			}
		}
		break;

	case PRIVATE_MARKET_SHOW_MAN:
		{
			if ( UIMSG_MOUSEIN_MARKET_ADVERTISE_LBDNUP & dwMsg )
			{
				NSGUI::SetCharMoveBlock ();
			}
		}
		break;

	case CLUB_WINDOW:
		{
			// RM# 1871 Ŭ�� â�� Ŭ���� �θ����� ���� �˾� ����
			// ���� ������� �ʴ� UI��� �ؼ� ���̻� �������� �ʵ�����
			//if( UIMSG_MOUSEIN_SUBMASTER_MAKE & dwMsg )
			//{
			//	if( !UiIsVisibleGroup( SUBMASTER_MAKE ) )
			//	{
			//		if( m_pSubMasterSet )
			//		{
			//			// ���� ���õ� �ε����� Ŭ������� �̸��� ���ͼ� ���긶����â�� �ѱ��.
			//			DWORD dwCharID = m_pClubWindow->GetClubMemberID();
   //                     CString strName = UI::ToString( m_pClubWindow->GetClubMemberName() );
			//			m_pSubMasterSet->SetSubMasterName( strName );
			//			m_pSubMasterSet->LoadCurrentOption( dwCharID );
			//			UiShowGroupFocus( SUBMASTER_MAKE, true );
			//		}
			//	}
			//}

			if ( UIMSG_ADDNAME_TO_CHATBOX_FROM_CLUBWINDOW & dwMsg )
			{
				GetBasicChatWindow()->BeginPrivateChat(  m_pClubWindow->GetClubMemberName() );
				UiShowGroupFocus( BASIC_CHAT_BOX );
			}
		}
		break;

	case FRIEND_WINDOW:
		{
			if( UIMSG_ADDNAME_TO_CHATBOX & dwMsg )
			{
				GetBasicChatWindow()->BeginPrivateChat( m_pFriendWindow->GetFriendName() );
				m_pBasicChat->RefreshFriendList(); // ä��â�� ģ������Ʈ�� ���� �����Ѵ�.
				UiShowGroupFocus ( BASIC_CHAT_BOX );
			}
			else if( UIMSG_EDIT_END & dwMsg )
			{
				m_pFriendWindow->EndEditFriendList();
			}
			else if( UIMSG_RECEIVE_NOTE_VISIBLE & dwMsg )
			{
				if( !UiIsVisibleGroup( RECEIVE_NOTE_WINDOW ) )
				{
					//if( m_pReceiveNoteWindow )
					//{
						UiShowGroupFocus( RECEIVE_NOTE_WINDOW, true );
					//}
				}
			}
			else if( UIMSG_WRITE_NOTE_VISIBLE & dwMsg )
			{
//#if defined(KRT_PARAM)
//				if( !UiIsVisibleGroup( SMS_SEND_WINDOW ) )
//				{
//					UiShowGroupFocus( SMS_SEND_WINDOW, true );
//				}
//#else
				if( !UiIsVisibleGroup( WRITE_NOTE_WINDOW ) )
				{
					UiShowGroupFocus( WRITE_NOTE_WINDOW, true );
				}
//#endif
			}
		}
		break;

	case BASIC_INFO_VIEW:
		{
			if( UIMSG_BASICINFOVIEW_LBDUP & dwMsg )
			{
				BasicInfoViewDoubleClick();
			}
		}
		break;
	}
}

void CInnerInterface::DisplayChatLinkMessage ( LPVOID msgBuffer )
{
	if( !msgBuffer) return;

	GLMSG::NET_CHAT_LINK_FB *pNetMsg = (GLMSG::NET_CHAT_LINK_FB*) msgBuffer;

	//���� ���·� GM�� �����Ҽ� ������ GM�� ä�ó��뵵 ���ܰ����ϴ�
	//���� �˻�
	//���� ģ�� ���踦 �޾ƿ´�
	//std::tr1::shared_ptr<SFRIEND> spFriend = m_pGaeaClient->GetFriendClient()->GetData( pNetMsg->szName );
	//if ( spFriend && spFriend->IsBLOCK() && pNetMsg->emType != CHAT_TYPE_PRIVATE )
	//	return;

	/*-- �������� ä�� ���� -------------------------------------------------------------------*/

	if( m_pGaeaClient && pNetMsg->emType == CHAT_TYPE_LOUDSPEAKER ) {
		GLLandManClient* pLand = m_pGaeaClient->GetActiveMap();
		GLCharacter* pChar = m_pGaeaClient->GetCharacter();

		if( pLand && pChar ) {
			const SMAPNODE* pMapNode = m_pGaeaClient->FindMapNode( pLand->GetBaseMapID() );

			if( pMapNode && PRISONLIMIT.IsLimitStatus( pMapNode, pChar->UserLevel(), PML_CHATTING ) ) {
				return;
			}
		}
	}

	if ( m_pBasicChat )
	{
		CString strCombine;
		CString strType;
		CString strTemp(pNetMsg->szChatMsg);
	
		if ( CRanFilter::GetInstance().ChatFilter( strTemp ) )
			strTemp = CRanFilter::GetInstance().GetProverb();


		DWORD dwSize = 0;

		VEC_LINK_TEXT_DATA vecLinkTextData;

		for ( DWORD i = 0; i < CHAT_LINK_MAX; ++i ) 
		{
			if ( !pNetMsg->sLinkDataRecv[i].VALID() ) continue;

			int nIndex = AddLinkDataToList(
				std::string( pNetMsg->szName ),
				pNetMsg->sLinkDataRecv[i].sLinkType,
				pNetMsg->LinkData + dwSize,
				pNetMsg->sLinkDataRecv[i].dwSize );

			dwSize += pNetMsg->sLinkDataRecv[i].dwSize;

			// LinkText�� ��ȯ
			if ( nIndex >= 0 ) 
			{
				SLINK_TEXT_DATA sLinkTextData;
				sLinkTextData.nLinkTypeIndex = nIndex;
				sLinkTextData.dwTextColor = GetLinkColor( nIndex );

				vecLinkTextData.push_back( sLinkTextData );
			}		
		}

		ConvertRecvData ( strTemp, vecLinkTextData );
		
		switch ( pNetMsg->emType )
		{
		case CHAT_TYPE_GLOBAL:		// ������ �۷ι� �޽���
		case CHAT_TYPE_GLOBAL_GM:
			{
				strType.Format( "[%s](%s):", ID2GAMEWORD("CHAT_TYPE",8), ID2GAMEWORD("ADMIN_NAME") );
				int nLength = strType.GetLength();

				for ( DWORD i = 0; i < vecLinkTextData.size(); ++i )
				{
					vecLinkTextData[i].sLinkPos.ADD( nLength );
				}
				
				strCombine = strType + strTemp;            
				
				// �ý��� �޼����� ��ڸ� ������ ����
				m_pBasicChat->AddStringToChatLink( UI::ToString( strCombine ), CHAT_CONST::CHAT_SYSTEM, vecLinkTextData, true );
			}
			break;

		case CHAT_TYPE_NORMAL:		// �Ϲ��� ä�� �޽���
		case CHAT_TYPE_NORMAL_GM:
			{
				if ( CRanFilter::GetInstance().ChatIgnorFilter( pNetMsg->szName ) ) return;

				strType.Format( "[%s](%s):", ID2GAMEWORD("CHAT_TYPE",0), pNetMsg->szName );
				int nLength = strType.GetLength();
				
				for ( DWORD i = 0; i < vecLinkTextData.size(); ++i )
				{
					vecLinkTextData[i].sLinkPos.ADD( nLength );
				}

				strCombine = strType + strTemp;

				m_pBasicChat->AddStringToChatLink( UI::ToString( strCombine ), CHAT_CONST::CHAT_NORMAL, vecLinkTextData, ( pNetMsg->emType >= CHAT_ADD_TYPE_FOPR_GM ) ? true : false  );
			}
			break;

		case CHAT_TYPE_PRIVATE:		// �������� �޽���, �Ӹ�, �ӻ���
		case CHAT_TYPE_PRIVATE_GM:
			{
				m_pBasicChat->SetPrivateMsgCharName( pNetMsg->szName );
                
                std::string strUtil = sc::string::format(ID2GAMEWORD("CHAT_TYPE",9), pNetMsg->szName);
				//std::string strUtil(pNetMsg->szName);
				//strUtil.append(ID2GAMEWORD("CHAT_TYPE",9));
				
				strType.Format("[%s]", strUtil.c_str());

				int nLength = strType.GetLength();
				
				for ( DWORD i = 0; i < vecLinkTextData.size(); ++i )
				{
					vecLinkTextData[i].sLinkPos.ADD( nLength );
				}

				strCombine = strType + strTemp;
				m_pBasicChat->AddStringToChatLink( UI::ToString( strCombine ), CHAT_CONST::CHAT_PRIVATE, vecLinkTextData, ( pNetMsg->emType >= CHAT_ADD_TYPE_FOPR_GM ) ? true : false );

				GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
				if ( !pCharacter )
					break;

                if ( pCharacter->IsAwayGesturing() && pCharacter->UserLevel() < USER_USER_GM )
                {
                    // Note : �ڸ���� ���¸�, �Ӹ� ���� ���濡�� �ڸ���� ���¸� �˸���.
                    std::string strReply = sc::string::format( "%s", ID2GAMEINTEXT( "REPLY_AWAY_STATE" ) );
                    m_pBasicChat->SendReplyAwayState( pNetMsg->szName, strReply );
                }
			}
			break;

		case CHAT_TYPE_PARTY:		// ��Ƽ������ ���޵Ǵ� �޽���
		case CHAT_TYPE_PARTY_GM:
			{
				strType.Format( "[%s](%s):", ID2GAMEWORD("CHAT_TYPE",2), pNetMsg->szName );
				int nLength = strType.GetLength();
				
				for ( DWORD i = 0; i < vecLinkTextData.size(); ++i )
				{
					vecLinkTextData[i].sLinkPos.ADD( nLength );
				}

				strCombine = strType + strTemp;
				m_pBasicChat->AddStringToChatLink( UI::ToString( strCombine ), CHAT_CONST::CHAT_PARTY, vecLinkTextData, ( pNetMsg->emType >= CHAT_ADD_TYPE_FOPR_GM ) ? true : false );
			}
			break;

		case CHAT_TYPE_MASTER:	// ��Ƽ������ ���޵Ǵ� �������� �޽���
		case CHAT_TYPE_MASTER_GM:
			{
				strType.Format( "[%s](%s):", ID2GAMEWORD("CHAT_TYPE",2), pNetMsg->szName );
				int nLength = strType.GetLength();

				for ( DWORD i = 0; i < vecLinkTextData.size(); ++i )
				{
					vecLinkTextData[i].sLinkPos.ADD( nLength );
				}

				strCombine = strType + strTemp;
				m_pBasicChat->AddStringToChatLink( UI::ToString( strCombine ), CHAT_CONST::CHAT_MASTER, vecLinkTextData, ( pNetMsg->emType >= CHAT_ADD_TYPE_FOPR_GM ) );
			}
			break;

        case CHAT_TYPE_GUILD:		// �������� ���޵Ǵ� �޽���
		case CHAT_TYPE_GUILD_GM:
			{
				strType.Format( "[%s](%s):", ID2GAMEWORD("CHAT_TYPE",3), pNetMsg->szName );
				int nLength = strType.GetLength();
				
				for ( DWORD i = 0; i < vecLinkTextData.size(); ++i )
				{
					vecLinkTextData[i].sLinkPos.ADD( nLength );
				}

				strCombine = strType + strTemp;
				m_pBasicChat->AddStringToChatLink( UI::ToString( strCombine ), CHAT_CONST::CHAT_GUILD, vecLinkTextData, ( pNetMsg->emType >= CHAT_ADD_TYPE_FOPR_GM ));
			}
			break;

		case CHAT_TYPE_ALLIANCE:
		case CHAT_TYPE_ALLIANCE_GM:
			{
				strType.Format( "[%s](%s):", ID2GAMEWORD("CHAT_TYPE",4), pNetMsg->szName );
				int nLength = strType.GetLength();
				
				for ( DWORD i = 0; i < vecLinkTextData.size(); ++i )
				{
					vecLinkTextData[i].sLinkPos.ADD( nLength );
				}

				strCombine = strType + strTemp;
				m_pBasicChat->AddStringToChatLink( UI::ToString( strCombine ), CHAT_CONST::CHAT_ALLIANCE, vecLinkTextData, ( pNetMsg->emType >= CHAT_ADD_TYPE_FOPR_GM ));
			}
			break;

		case CHAT_TYPE_AREA:
		case CHAT_TYPE_AREA_GM:
			{
				if ( CRanFilter::GetInstance().ChatIgnorFilter( pNetMsg->szName ) ) return;

				strType.Format( "[%s](%s):", ID2GAMEWORD("CHAT_TYPE",5), pNetMsg->szName );
				int nLength = strType.GetLength();
				
				for ( DWORD i = 0; i < vecLinkTextData.size(); ++i )
				{
					vecLinkTextData[i].sLinkPos.ADD( nLength );
				}

				strCombine = strType + strTemp;
				m_pBasicChat->AddStringToChatLink( UI::ToString( strCombine ), CHAT_CONST::CHAT_AREA, vecLinkTextData, ( pNetMsg->emType >= CHAT_ADD_TYPE_FOPR_GM ) ? true : false );
			}
			break;

		case CHAT_TYPE_PARTY_RECRUIT:
		case CHAT_TYPE_PARTY_RECRUIT_GM:
			{
				if ( CRanFilter::GetInstance().ChatIgnorFilter( pNetMsg->szName ) ) return;

				strType.Format( "[%s](%s):", ID2GAMEWORD("CHAT_TYPE",6), pNetMsg->szName );
				int nLength = strType.GetLength();
				
				for ( DWORD i = 0; i < vecLinkTextData.size(); ++i )
				{
					vecLinkTextData[i].sLinkPos.ADD( nLength );
				}

				strCombine = strType + strTemp;
				m_pBasicChat->AddStringToChatLink( UI::ToString( strCombine ), CHAT_CONST::CHAT_PARTY_RECRUIT, vecLinkTextData, ( pNetMsg->emType >= CHAT_ADD_TYPE_FOPR_GM ) );
			}
			break;

		case CHAT_TYPE_LOUDSPEAKER:
		case CHAT_TYPE_LOUDSPEAKER_GM:
			{
				strType.Format( "[%s](%s):", ID2GAMEWORD("CHAT_TYPE",7), pNetMsg->szName );
				int nLength = strType.GetLength();
				
				for ( DWORD i = 0; i < vecLinkTextData.size(); ++i )
				{
					vecLinkTextData[i].sLinkPos.ADD( nLength );
				}

				strCombine = strType + strTemp;
				m_pBasicChat->AddStringToChatLink( UI::ToString( strCombine ), CHAT_CONST::CHAT_TOALL, vecLinkTextData, (pNetMsg->emType >= CHAT_ADD_TYPE_FOPR_GM));//( pNetMsg->emType >= CHAT_ADD_TYPE_FOPR_GM ) ? true : false );
			}
			break;
		
		case CHAT_TYPE_CTRL_GLOBAL:
		case CHAT_TYPE_CTRL_GLOBAL_GM:
			{
				strType.Format( "[%s](%s):", ID2GAMEWORD("CHAT_TYPE",8), ID2GAMEWORD("NOTIFY_NAME") );
				int nLength = strType.GetLength();
				
				for ( DWORD i = 0; i < vecLinkTextData.size(); ++i )
				{
					vecLinkTextData[i].sLinkPos.ADD( nLength );
				}

				strCombine = strType + strTemp;
				m_pBasicChat->AddStringToChatLink( UI::ToString( strCombine ), CHAT_CONST::CHAT_SYSTEM, vecLinkTextData, (pNetMsg->emType >= CHAT_ADD_TYPE_FOPR_GM) );
			}
			break;

		case CHAT_TYPE_FACTION:		// ���� ä�� �޽���
		case CHAT_TYPE_FACTION_GM:
			{
				if ( CRanFilter::GetInstance().ChatIgnorFilter( pNetMsg->szName ) ) return;

				strType.Format( "[%s](%s):", ID2GAMEWORD("CHAT_TYPE",10), pNetMsg->szName );
				int nLength = strType.GetLength();

				for ( DWORD i = 0; i < vecLinkTextData.size(); ++i )
				{
					vecLinkTextData[i].sLinkPos.ADD( nLength );
				}

				strCombine = strType + strTemp;

				m_pBasicChat->AddStringToChatLink( UI::ToString( strCombine ), CHAT_CONST::CHAT_FACTION, vecLinkTextData, ( pNetMsg->emType >= CHAT_ADD_TYPE_FOPR_GM ) ? true : false  );
			}
			break;

		default:
			GASSERT( FALSE );
			break;
		};

		D3DCOLOR dwIDColor = NS_UITEXTCOLOR::IDCOLOR;
		switch ( pNetMsg->emType )
		{
		case CHAT_TYPE_GLOBAL:
		case CHAT_TYPE_GLOBAL_GM:
			{
				if ( m_pAdminMessageDisplay )
				{
					if ( !UiIsVisibleGroup ( ADMIN_MESSAGE_DISPLAY ) )
						strCombine.Format ( "[%s]:%s", ID2GAMEWORD("ADMIN_NAME"), strTemp );	
					else
						strCombine.Format ( "%s", strTemp );	

					m_pAdminMessageDisplay->AddText ( strCombine, NS_UITEXTCOLOR::ADMIN_COLOR );

					
					UiShowGroupTop ( ADMIN_MESSAGE_DISPLAY );					
				}
			}
			break;

		case CHAT_TYPE_NORMAL:
		case CHAT_TYPE_PARTY:
		case CHAT_TYPE_GUILD:
		case CHAT_TYPE_ALLIANCE:
		case CHAT_TYPE_NORMAL_GM:
		case CHAT_TYPE_PARTY_GM:
		case CHAT_TYPE_FACTION:
		case CHAT_TYPE_FACTION_GM:
//		case CHAT_TYPE_PRIVATE:		
			{
				if ( m_pHeadChatDisplayMan )
				{
					D3DXVECTOR3 *pPos = m_pGaeaClient->FindCharHeadPos( pNetMsg->szName );
					if ( pPos )
					{
						D3DXVECTOR3 vPos = *pPos;
						D3DXVECTOR3 vScreen = DxViewPort::GetInstance().ComputeVec3Project ( &vPos, NULL );

						int nPosX = (int)( vScreen.x);
						int nPosY = (int)( vScreen.y);

						DWORD dwIDColor = NS_UITEXTCOLOR::IDCOLOR;

						////	�̸� �� (PK_COLOR)
						//PGLCHARCLIENT pCHAR = m_pGaeaClient->GetChar ( sTargetID.dwID );
						//if ( !pCHAR ) return ;
						//dwColor = pCHAR->GET_PK_COLOR ();

						if ( IsPartyMember(pNetMsg->szName) == true )
                            dwIDColor = NS_UITEXTCOLOR::GREENYELLOW;
						
						// GM�� ä�� �̸����� �ٸ��� ǥ��
						if(  pNetMsg->emType >= CHAT_ADD_TYPE_FOPR_GM )
							dwIDColor = NS_UITEXTCOLOR::CHAT_GM;

						D3DXVECTOR2 vCharPos = D3DXVECTOR2(float(nPosX), float(nPosY));
						m_pHeadChatDisplayMan->AddChatMsg(
                            pNetMsg->szName,
                            dwIDColor,
                            UI::ToString(strTemp),
                            NS_UITEXTCOLOR::DEFAULT,
                            vCharPos);
					}
				}
			}
			break;

		}
	}
}

void CInnerInterface::DisplayChatPartyPickupLinkMessage(const CString& strName, SITEMCUSTOM& sItemCustom)
{
    if ( m_pBasicChat )
    {
        std::string strChatMsg, strTemp;
        CString     strCombine;

        // ��ũ ������ ã�´�.
        SLINK_TYPE sLinkType;
        sLinkType.wMType = ITEM_LINK;
        sLinkType.wSType = ITEM_LINK_INVEN;
        strTemp    = sc::string::format( "%s%d%d%s", strLINK_LEFT_SIMBOL, sLinkType.wMType, sLinkType.wSType, strLINK_RIGHT_SIMBOL );			
        strChatMsg = sc::string::format( ID2GAMEINTEXT("PARTY_MBR_PICKUP_BRD"), strName, strTemp );
        strCombine.Format ( "[%s]%s", ID2GAMEWORD("CHAT_TYPE",8), strChatMsg.c_str() );

        int nIndex = AddLinkDataToList(
			std::string( strName.GetString() ),
			sLinkType,
			(void *)&sItemCustom,
			sizeof(SITEMCUSTOM) );

        VEC_LINK_TEXT_DATA vecLinkTextData;

        // LinkText�� ��ȯ
        if ( nIndex >= 0 ) 
        {
            SLINK_TEXT_DATA sLinkTextData;
            sLinkTextData.nLinkTypeIndex   = nIndex;
            sLinkTextData.dwTextColor      = GetLinkColor( nIndex );
            vecLinkTextData.push_back( sLinkTextData );
        }		

        ConvertRecvData ( strCombine, vecLinkTextData );

        m_pBasicChat->AddStringToChatLink( UI::ToString( strCombine ), CHAT_CONST::CHAT_SYSTEM, vecLinkTextData );
    }
}

//	���� �Ŀ��� �ý��� �޽����� ó���Ѵ�.
void CInnerInterface::DisplayChatMessage ( int nType, const char *szName, const char *szMsg )
{
	if ( m_pBasicChat )
	{
		CString strCombine;
		CString strTemp = szMsg;

		//�ý��� �޽��� �弳���� ����
		//if ( CRanFilter::GetInstance().ChatFilter( strTemp ) )
		//	strTemp = CRanFilter::GetInstance().GetProverb();

		
		switch ( nType )
		{
		case CHAT_TYPE_GLOBAL:		// ������ �۷ι� �޽���
			{
				strCombine.Format ( "[%s](%s):%s", ID2GAMEWORD("CHAT_TYPE",8), ID2GAMEWORD("ADMIN_NAME"), strTemp );	
				m_pBasicChat->AddStringToChatEx ( UI::ToString( strCombine ), CHAT_CONST::CHAT_SYSTEM, true );
			}
			break;

		case CHAT_TYPE_CTRL_GLOBAL:
			{
				strCombine.Format ( "[%s](%s):%s", ID2GAMEWORD("CHAT_TYPE",8), ID2GAMEWORD("NOTIFY_NAME"), strTemp );	
				m_pBasicChat->AddStringToChatEx ( UI::ToString( strCombine ), CHAT_CONST::CHAT_SYSTEM, true );
			}
			break;

		default:
			GASSERT( FALSE );
			break;
		};

		D3DCOLOR dwIDColor = NS_UITEXTCOLOR::IDCOLOR;
		switch ( nType )
		{
		case CHAT_TYPE_GLOBAL:
			{
				if ( m_pAdminMessageDisplay )
				{
					if ( !UiIsVisibleGroup ( ADMIN_MESSAGE_DISPLAY ) )
						strCombine.Format ( "[%s]:%s", ID2GAMEWORD("ADMIN_NAME"), strTemp );	
					else
						strCombine.Format ( "%s", strTemp );	

					m_pAdminMessageDisplay->AddText ( strCombine, NS_UITEXTCOLOR::ADMIN_COLOR );					
					UiShowGroupTop ( ADMIN_MESSAGE_DISPLAY );					
				}
			}
			break;
		}
	}
}


//void CInnerInterface::TranslateActionMsgQ ( UIGUID ControlID, DWORD dwMsg )
//{
//	CUIMan::TranslateActionMsgQ ( ControlID, dwMsg );
//
//	switch ( ControlID )
//	{
//	case FRIEND_WINDOW:
//		{
//			if ( UIMSG_ADDNAME_TO_CHATBOX & dwMsg )
//			{
//				GetBasicChatWindow()->BEGIN_PRIVATE_CHAT ( m_pFriendWindow->GET_FRIEND_NAME () );
//				m_pBasicChat->REFRESH_FRIEND_LIST(); // ä��â�� ģ������Ʈ�� ���� �����Ѵ�.
//				UiShowGroupFocus ( BASIC_CHAT_BOX );
//			}
//			else if ( UIMSG_EDIT_END & dwMsg )
//			{
//				m_pFriendWindow->EDIT_END ();
//			}
//		}
//		break;		
//
//	case CLUB_WINDOW:
//		{
//			if ( UIMSG_ADDNAME_TO_CHATBOX_FROM_CLUBWINDOW & dwMsg )
//			{
//				GetBasicChatWindow()->BEGIN_PRIVATE_CHAT ( m_pClubWindow->GetClubMemberName() );
//				UiShowGroupFocus ( BASIC_CHAT_BOX );
//			}
//		}
//		break;	
//	}
//
//	if( MODAL_WINDOW <= ControlID ) // MEMO : ���̵� ����̸�...
//	{
//		if( dwMsg & UIMSG_INVISIBLE )
//		{
//			CModalWindow* pModalWindow = (CModalWindow*) FindControl ( ControlID );
//			if ( pModalWindow && pModalWindow->m_bTEST_MULTI )
//			{
//				GetFocusList()->EraseControl ( ControlID );
//				DeleteControl ( ControlID );
//			}
//		}
//	}
//}

void CInnerInterface::MsgProductStart( DWORD nID, WORD nCount )
{
	if( !m_pProductProgress || !m_pProductWindow )
		return;

	m_pProductProgress->StartProduct( nID, nCount );

	m_pProductWindow->SetProducting( true );
}

void CInnerInterface::MsgProductGoAlong()
{
	if( !m_pProductProgress || !m_pProductWindow )
		return;

	m_pProductProgress->GoAlongProduct();
}

void CInnerInterface::MsgProductComplete()
{
	if( !m_pProductProgress || !m_pProductWindow )
		return;

	m_pProductProgress->EndProduct();

	m_pProductWindow->SetProducting( false );
}

void CInnerInterface::MsgProductCancel()
{
	if( !m_pProductProgress || !m_pProductWindow )
		return;

	m_pProductProgress->CancelProduct();

	m_pProductWindow->SetProducting( false );
};

void CInnerInterface::MsgProductNeedConfirm()
{
	if( !m_pProductWindow )
		return;

	m_pProductWindow->DialogMaterialConfirm();
}
