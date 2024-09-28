#include "StdAfx.h"

#include "./StaticUIManager.h"
#include "InnerInterface.h"

#include "../SigmaCore/String/StringFormat.h"

#include "../EngineLib/GUInterface/GameTextControl.h"
#include "../EngineLib/GUInterface/UITextControl.h"
#include "../EngineLib/DxTools/DebugSet.h"

#include "../RanLogic/Item/GLItemMan.h"
#include "../RanLogic/Msg/ApexMsg.h"
#include "../RanLogic/GLogicDataMan.h"
#include "../RanLogic/HideSet/GLHideSet.h"
#include "../RanLogic/GLUseFeatures.h"

#include "../RanLogicClient/Friend/GLFriendClient.h"
#include "../RanLogicClient/Stage/DxGlobalStage.h"
#include "../RanLogicClient/SNS/GLSNSClient.h"
#include "../RanLogicClient/GLGaeaClient.h"
#include "../RanLogicClient/Tutorial/GLBaseTutorial.h"
#include "../RanLogicClient/Land/GLLandManClient.h"
#include "../RanLogicClient/Pet/GLPetClient.h"
#include "../RanLogicClient/Club/GLClubClient.h"
#include "../RanLogicClient/Notify/NotifyClientFactory.h"
#include "../RanLogicClient/Notify/NotifyClientMan.h"
#include "../RanLogicClient/Party/GLPartyManagerClient.h"
#include "../RanLogicClient/Tournament/GLTournamentClient.h"
#include "../RanLogicClient/CostumeStat/CotStatClient.h"


#include "../RanLogic/RANPARAM.h"

#include "OldUI/Util/ModalWindow.h"
#include "OldUI/Club/ClubWindow.h"
#include "OldUI/Confront/ConftModalWindow.h"
#include "OldUI/NPC/DialogueWindow.h"
#include "OldUI/Friend/FriendWindow.h"
#include "OldUI/Item/InventoryWindow.h"
#include "OldUI/Item/InventoryInterface.h"
#include "OldUI/Item/MarketWindow.h"
#include "OldUI/Party/PartyWindow.h"
#include "OldUI/Party/PartyModalWindow.h"
#include "OldUI/Groupchat/ChatGroupModalWindow.h"
#include "OldUI/PrivateMarket/PrivateMarketSellWindow.h"
#include "OldUI/PrivateMarket/PrivateMarketWindow.h"
#include "OldUI/Char/PtoPWindow.h"
#include "OldUI/Quest/QuestWindow.h"
#include "OldUI/Quest/WindowRenewal/QuestWindowRenewal.h"
#include "OldUI/Quest/WindowRenewal/Main/QuestMainRenewal.h"
#include "OldUI/Storage/StorageChargeCard.h"
#include "OldUI/Storage/StorageWindow.h"
#include "OldUI/Trade/TradeWindow.h"
#include "OldUI/Message/WaitServerDialogue.h"
#include "OldUI/Message/RebirthDialogue.h"
#include "OldUI/Feature/ItemMixWindow.h"
#include "OldUI/Vehicle/VehicleWindow.h"
#include "OldUI/Feature/StatsResetWindow.h"
#include "OldUI/Post/PostBoxWindow.h"
#include "OldUI/Post/PostBoxReadPage.h"
#include "OldUI/Post/PostBoxSendPage.h"
#include "OldUI/Post/PostBoxReceivePage.h"
#include "OldUI/SNS/SNSWindow.h"
#include "OldUI/NPC/DialogueWindowRenewal.h"

//-----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
//-----------------------------------------------------------------------------

void CInnerInterface::MsgProcess(LPVOID msgBuffer)
{
	if (!msgBuffer)
        return;

	NET_MSG_GENERIC* nmg = (NET_MSG_GENERIC*) msgBuffer;
	switch (nmg->nType)
	{
    /*
	case NET_MSG_EVENT_LOTTERY_FB:
        MsgEventLotteryFB(nmg);
		break;
	*/
	case NET_MSG_APEX_ANSWER:
        MsgApexAnswer(nmg);
		break;
	
	case NET_MSG_CHAT_CTRL_FB:			// 관리자용 채팅메시지
        MsgChatFB(nmg);
		break;

	case NET_MSG_CHAT_LINK_FB:
        DisplayChatLinkMessage(msgBuffer);
		break;

	case NET_MSG_CHAT_PRIVATE_FAIL:
        MsgChatPrivateFail(nmg);
		break;
	
	case NET_MSG_CHAT_BLOCK:
        MsgChatBlock(nmg);
		break;

	case NET_MSG_GCTRL_DROP_OUT_FORCED:
        MsgDropOutForced(nmg);
		break;

	case NET_MSG_GCTRL_REQ_TAKE_FB:
        MsgReqTakeFB(nmg);
		break;

	case NET_MSG_GCTRL_TRADE_TAR:
        MsgTradeTarget(nmg);
        break;

	case NET_MSG_GCTRL_WITH_VEHICLE_TAR:
        MsgWithVehicleTarget(nmg);
        break;

	case NET_MSG_GCTRL_CONFRONT_TAR:
        MsgConfrontTarget(nmg);
		break;

	case NET_MSG_SERVER_GENERALCHAT:
        MsgServerGeneralChat(nmg);
		break;

//		NOTE
//			CLOSING MESSAGE 처리
//	case BLAH_BLAH:
//		{
//			SetWaitServerDialogueClose ();
//		}
//		break;
    default:
        break;
	};
}

void CInnerInterface::ModalMsgProcess ( UIGUID nCallerID, DWORD dwMsg )
{
	switch ( nCallerID )
	{
	case UI::MODAL_CLOSEGAME:
        ModalCloseGame(dwMsg);
		break;
	
	case UI::MODAL_TEST_MODAL:
		break;

	case UI::MODAL_DO_NOTING:
		break;

    case UI::MODAL_CONFIRM_CUSTOMER_CENTER:
        ModalConfirmCustomerCenter(dwMsg);
        break;

	case UI::MODAL_INVENTORY_MONEY_TO_FIELD:
        ModalInventoryMoneyToField(dwMsg);
		break;

	case UI::MODAL_PARTY_TAR:
        ModalPartyTarget(dwMsg);
		break;

	case UI::MODAL_PARTY_REQ:
        ModalPartyRequest(dwMsg);
		break;

    case UI::MODAL_PARTY_REQ_DBNUM:
        ModalPartyRequestByCharDbNum(dwMsg);
        break;

	case UI::MODAL_CHAT_GROUP_TAR:
		ModalChatGroupTarget(dwMsg);
		break;

	case UI::MODAL_CHAT_GROUP_REQ:
		ModalChatGroupRequest(dwMsg);
		break;

	case UI::MODAL_EXPEDITION_REQ:
        ModalPartyRequest(dwMsg);
		break;
	
	case UI::MODAL_TRADE_REQ:
        ModalTradeRequest(dwMsg);
		break;

	case UI::MODAL_VEHICLE_WITH:
        ModalVehicleWith(dwMsg);
		break;
	
	case UI::MODAL_REBIRTH:
        ModalReBirth(dwMsg);
		break;

	case UI::MODAL_RECOVERY_EXP:
        ModalRecoveryExp(dwMsg);
		break;

	case UI::MODAL_RECOVERY_NPC_EXP:
        ModalRecoveryNpcExp(dwMsg);
		break;


	//case MODAL_DROPMONEY:
	//	{
	//		if ( dwMsg & UIMSG_MODAL_OK )
	//		{
	//			GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	//			if ( pCharacter )
	//			{
	//				CString strTemp = GetModalWindow()->GetEditString ();
	//				LONGLONG lnDropMoney = _ttoi64 ( strTemp.GetString () );
	//				LONGLONG lnMoney = pCharacter->m_lnMoney;
	//				if ( lnMoney < lnDropMoney ) lnDropMoney = lnMoney;

	//				pCharacter->ReqMoneyToField ( lnDropMoney );
	//			}
	//		}
	//	}
	//	break;
	case UI::MODAL_BETTINGMONEY:
		ModalBettingMoney(dwMsg);
		break;

	case UI::MODAL_TRADEMONEY:
        ModalTradeMoney(dwMsg);
		break;

	case UI::MODAL_SAVEMONEY:
        ModalSaveMoney(dwMsg);
		break;

	case UI::MODAL_CLUB_SAVEMONEY:
        ModalClubSaveMoney(dwMsg);
		break;

	case UI::MODAL_LOADMONEY:
        ModalLoadMoney(dwMsg);
		break;

	case UI::MODAL_CLUB_LOADMONEY:
        ModalClubLoadMoney(dwMsg);
		break;

	case UI::MODAL_SPLIT_ITEM:
        ModalSplitItem(dwMsg);
		break;

	case UI::MODAL_STORAGE_SPLIT_ITEM:
        ModalStorageSplitItem(dwMsg);
		break;

	case UI::MODAL_CLUB_STORAGE_SPLIT_ITEM:
        ModalClubStorageSplitItem(dwMsg);
		break;

	case UI::MODAL_CONFLICT_ONE_TAR:
	case UI::MODAL_CONFLICT_PARTY_TAR:
	case UI::MODAL_CONFLICT_GUILD_TAR:
        ModalConflict(dwMsg);
		break;

	case UI::MODAL_CONFLICT_ONE_REQ:
	case UI::MODAL_CONFLICT_PARTY_REQ:
	case UI::MODAL_CONFLICT_GUILD_REQ:
        ModalConflictRequest(dwMsg);
		break;

	case UI::MODAL_PILE_ITEM:
        ModalPileItem(dwMsg);
		break;

    case UI::MODAL_DIALOGUE_PILE_ITEM:
        ModalDlgPileItem(dwMsg);
        break;

	case UI::MODAL_SERVER_STAGE:
        ModalServerStage(dwMsg);
		break;

	case UI::MODAL_BUY_ITEM:
	case UI::MODAL_SELL_ITEM:
        ModalBuySellItem(dwMsg, nCallerID);
		break;
	case UI::MODAL_NPC_EXCHANGE_BUY_ITEM :
		ModalExChangeBuySellItem(dwMsg, nCallerID);
		break;
    case UI::MODAL_DIRECT_SELL_ITEM:
        ModalDirectSellItem(dwMsg);
        break;

	case UI::MODAL_QUEST_GIVEUP:
        ModalQuestGiveUp(dwMsg);
		break;

	case UI::MODAL_FRIEND_ADD:
        ModalFriendAdd(dwMsg);
		break;

	case UI::MODAL_FRIEND_DEL:
        ModalFriendDel(dwMsg);
		break;

	case UI::MODAL_FRIEND_MOVE:
        ModalFriendMove(dwMsg);
		break;

	case UI::MODAL_ADD_FRIEND_LURE_ANS:
        ModalAddFriendLureAns(dwMsg);
		break;

	case UI::MODAL_DEFAULT_POS:
        ModalDefaultPos(dwMsg);
		break;
    /*
	case UI::MODAL_LOTTO:
        ModalLotto(dwMsg);
		break;
    */
	case UI::MODAL_STORAGE_CHARGE:
        ModalStorageCharge(dwMsg);
		break;

	case UI::MODAL_PRIVATE_MARKET_SELL:
        ModalPrivateMarketSell(dwMsg);
		break;

	case UI::MODAL_PRIVATE_MARKET_BUY:
        ModalPrivateMarketBuy(dwMsg);
		break;

	case UI::MODAL_PRIVATE_MARKET_PILE:
        ModalPrivateMarketPile(dwMsg);
		break;

	case UI::MODAL_PRIVATE_MARKET_ONLY_UI:
		ModalPrivateMarketOnlyUI(dwMsg);
		break;

	case UI::MODAL_PRIVATE_MARKET_QUIT:
        ModalPrivateMarketQuit(dwMsg);
		break;

	case UI::MODAL_CLUB_KICK:
        ModalClubKick(dwMsg);
		break;

	case UI::MODAL_CLUB_AUTHORITY:
        ModalClubAuthority(dwMsg);
		break;

	case UI::MODAL_CLUB_AUTHORITY_ASK:
        ModalClubAuthorityAsk(dwMsg);
		break;

    case UI::MODAL_CLUB_CD_AUTHORITY_TO_MEMBER:
        ModalCDAuthorityToMember(dwMsg);
        break;

    case UI::MODAL_CLUB_CD_AUTHORITY_TO_MASTER:
        ModalCDAuthorityToMaster(dwMsg);
        break;

	case UI::MODAL_CLUB_BREAKUP:
        ModalClubBreakUp(dwMsg);
		break;

	case UI::MODAL_CLUB_CHANGE_MARK:
        ModalClubChangeMark(dwMsg);
		break;

	case UI::MODAL_CLUB_NICK:
        ModalClubNick(dwMsg);
		break;

	case UI::MODAL_CLUB_JOIN_ASK:
        ModalClubJoinAsk(dwMsg);
		break;

	case UI::MODAL_CLUB_JOIN:
        ModalClubJoin(dwMsg);
		break;

	case UI::MODAL_CLUB_BREAKUP_CANCEL:
        ModalClubBreakUpCancel(dwMsg);
		break;

	case UI::MODAL_CLUB_SELFQUIT:
        ModalClubSelfQuit(dwMsg);
		break;

	case UI::MODAL_SKILL_LEVEL_UP_CONFIRM:
        ModalSkillLevelUpConfirm(dwMsg);
		break;

	case UI::MODAL_COMMISSION:
        ModalCommission(dwMsg);
		break;

	case UI::MODAL_FRIEND_BLOCK_ADD_N:
        ModalFriendBlockAddN(dwMsg);
		break;
        
	case UI::MODAL_FRIEND_BLOCK_ADD_B:
        ModalFriendBlockAddB(dwMsg);
		break;

	case UI::MODAL_FRIEND_BLOCK_DEL:
        ModalFriendBlockDel(dwMsg);
		break;

	case UI::MODAL_UNION_JOIN_ASK:
        ModalUnionJoinAsk(dwMsg);
		break;

	case UI::MODAL_UNION_JOIN:
        ModalUnionJoin(dwMsg);
		break;

	case UI::MODAL_CLUB_BATTLE_ASK:
        ModalClubBattleAsk(dwMsg);
		break;

	case UI::MODAL_ALLIANCE_BATTLE_ASK:
        ModalAllianceBattleAsk(dwMsg);
		break;

	case UI::MODAL_CLUB_BATTLE_ARMISTICE:
        ModalClubBattleArmistice(dwMsg);
		break;

	case UI::MODAL_ALLIANCE_BATTLE_ARMISTICE:
        ModalAllianceBattleArmistice(dwMsg);
		break;

	case UI::MODAL_CLUB_BATTLE_ARMISTICE_ASK:
        ModalClubBattleArmisticeAsk(dwMsg);
		break;

	case UI::MODAL_ALLIANCE_BATTLE_ARMISTICE_ASK:
        ModalAllianceBattleArmisticeAsk(dwMsg);
		break;

	case UI::MODAL_CLUB_BATTLE_SUBMISSION:
        ModalClubBattleSubmission(dwMsg);
		break;

	case UI::MODAL_ALLIANCE_BATTLE_SUBMISSION:
        ModalAllianceBattleSubmission(dwMsg);
		break;

	case UI::MODAL_UNION_KICK:
        ModalUnionKick(dwMsg);
		break;

	case UI::MODAL_UNION_WITHDRAW:
		ModalUnionWithdraw(dwMsg);
		break;

	case UI::MODAL_UNION_LEAVE:
        ModalUnionLeave(dwMsg);
		break;

	case UI::MODAL_UNION_BREAKUP:
        ModalUnionBreakUp(dwMsg);
		break;

	case UI::MODAL_CHAR_RENAME:
        ModalCharRename(dwMsg);
		break;

	case UI::MODAL_PET_RENAME:
        ModalPetRename(dwMsg);
		break;

	case UI::MODAL_RESET_SKILLSTATS:
        ModalResetSkillStats(dwMsg);
		break;

	case UI::MODAL_ITEMREBUILD_MONEY:	// ITEMREBUILD_MARK
        ModalItemRebuildMoney(dwMsg);
		break;

	case UI::MODAL_ITEMREBUILD_OKQUESTION:
        ModalItemRebuildOkQuestion(dwMsg);
		break;	

	case UI::MODAL_ITEMGARBAGE_OKQUESTION:
        ModalItemGarbageOkQuestion(dwMsg);
		break;

	case UI::MODAL_HAIRCOLOR_INFO:
        ModalHairColorInfo(dwMsg);
		break;

	case UI::MODAL_PETCOLOR_INFO:
        ModalPetColorInfo(dwMsg);
		break;

	case UI::MODAL_MOBILE_NUMBER:
        ModalMobileNumber(dwMsg);
		break;

	case UI::MODAL_GENDER_CHANGE:
        ModalGenderChange(dwMsg);
		break;

	case UI::MODAL_GENDER_CHANGE_END:
        ModalGenderChangeEnd(dwMsg);
		break;

	case UI::MODAL_ITEMMIX_OKQUESTION:
        ModalItemMixOkQuestion(dwMsg);
		break;

	case UI::MODAL_SKILLFACT_REMOVE:
        ModalSkillFactRemove(dwMsg);
		break;

	case UI::MODAL_SKILLFACT_LUNCHBOX_OVERWRITE:
        ModalSkillFactLunchBoxOverWrite(dwMsg);
		break;

	case UI::MODAL_VEHICLE_COLOR:
        ModalVehicleColor(dwMsg);
		break;

	case UI::MODAL_POSTBOX_PAGE_READ_BUTTON_DELETE:
        ModalPostBoxPageReadButtonDelete(dwMsg);
		break;

    case UI::MODAL_POSTBOX_PAGE_READ_BUTTON_DELETE_CONFIRM_ATTACH:
        ModalPostBoxPageReadButtonDeleteConfirmAttach(dwMsg);
        break;

	case UI::MODAL_POSTBOX_PAGE_READ_BUTTON_RETURN:
        ModalPostBoxPageReadButtonReturn(dwMsg);
		break;

	case UI::MODAL_POSTBOX_PAGE_RECEIVE_BUTTON_DELETE_CHECK:
        ModalPostBoxPageReceiveButtonDeleteCheck(dwMsg);
		break;

	case UI::MODAL_POSTBOX_PAGE_RECEIVE_BUTTON_DELETE_COMMIT:
        ModalPostBoxPageReceiveButtonDeleteCommit(dwMsg);
		break;

	case UI::MODAL_POSTBOX_PAGE_READ_BUTTON_TAKE:
        ModalPostBoxPageReadButtonTake(dwMsg);
		break;

	case UI::MODAL_POSTBOX_PAGE_SEND_BUTTON_SEND_COMMISION:
        ModalPostBoxPageSendButtonSendCommision(dwMsg);
		break;

	// by luxes.
	case UI::MODAL_REQ_HOLD_TO_FILED:
        ModalReqHoldToField(dwMsg);
		break;
	
    case UI::MODAL_SNS_FACEBOOK_ADDFRIEND:
        ModalSnsFacebookAddFriend(dwMsg);
        break;

    case UI::MODAL_SNS_TWITTER_FOLLOW:
        ModalSnsTwitterFollow(dwMsg);
        break;

	// by luxes.
	case UI::MODAL_COSTUM_COLOR:
        ModalCostumeColor(dwMsg);
		break;

	// by luxes.
	case UI::MODAL_TUTORIAL_SKIP:
        ModalTutorialSkip(dwMsg);
		break;
	
    case UI::MODAL_ITEMBANK_GET_ITEM:
        ModalItemBankGetItem(dwMsg);
        break;

    case UI::MODAL_SELL_ITEM_FROM_INVEN:
        ModalSellItemFromInven(dwMsg);
        break;

    case UI::MODAL_FORCE_REBIRTH_SKILL:
        ModalForceReBirthSkill(dwMsg);
        break;

	case UI::MODAL_STORAGE_CONNECT_ITEMUSE_REQ:
        ModalStorageConnectItemUseRequest(dwMsg);
		break;

	case UI::MODAL_INVENTORY_ITEMDECOMPOSE_OKQUESTION:
		ModalInventoryItemDecomposeOkQuestion(dwMsg);
		break;
	case UI::MODAL_COSTUME_STAT_APPLY :
		ModalCostumeStatApply(dwMsg);
		break;
	case UI::MODAL_COSTUME_STAT_BASIC_HAS :
		ModalCostumeStatBasic(dwMsg);
		break;
	case UI::MODAL_COSTUME_STAT_APPLIED :
		ModalCostuemStatApplied(dwMsg);
		break;

	case UI::MODAL_CONFIRM_CHANGE_COUNTRY:
		ModalConfirmChangeCountry(dwMsg);
		break;

	case UI::MODAL_CONFIRM_JOIN_PVP_SERVER:
		ModalConfirmJoinPVPServer(dwMsg);
		break;
	case UI::MODAL_COSTUME_STAT_RELEASE :
		ModalCostumeStatRelease(dwMsg);
		break;
	case UI::MODAL_SELFORMW_SELECTITEM:
		ModalSelectiveformBoxSelect(dwMsg);
		break;
	case UI::MODAL_TEMP_ENCHANT  :
		ModalTempEnchant(dwMsg);
		break;

    default:
        break;
	}
}

void CInnerInterface::ModalConfirmCustomerCenter(DWORD dwMsg )
{
    if ( dwMsg & UIMSG_MODAL_OK )
    {
        CWnd* pWnd = ::AfxGetMainWnd();
        HWND hWnd = NULL;
        if ( pWnd)
            hWnd = pWnd->m_hWnd;
        if ( hWnd )
            ::ShowWindow( hWnd, SW_MINIMIZE );

        ::ShellExecute(NULL, "open", RANPARAM::CustomerCenterAddress, NULL, NULL, SW_SHOWNORMAL);
    }
}

void CInnerInterface::ModalWindowSetModalData(int nData1, int nData2)
{
    m_pModalWindow->SetModalData(nData1, nData2);
}

void CInnerInterface::ModalWindowSetModalData(const D3DXVECTOR3& vData)
{
    m_pModalWindow->SetModalData(vData);
}

void CInnerInterface::ModalQuestGiveUp(DWORD dwMsg) 
{
    if ( UIMSG_MODAL_OK & dwMsg )
    {
        //	포기				
        DWORD dwQuestID;
        dwQuestID = GetQuestWindowRenewal()->GetQuestMainID();

        m_pGaeaClient->GetCharacter ()->ReqQuestGiveUp ( dwQuestID );
    }
}

void CInnerInterface::ModalDirectSellItem(DWORD dwMsg) 
{
    if ( dwMsg & UIMSG_MODAL_OK )
    {
        int nPageIndex = m_pDialogueWindowRenewal->GetPageIndex();

        int nPosX = nOUTOFRANGE;
        int nPosY = nOUTOFRANGE;

        GetModalWindow()->GetModalData( &nPosX, &nPosY );
        if ( nPosX < 0 || nPosY < 0 )
            return;

        CDebugSet::ToView( 1, 3, "파는 아이템 : %d %d", nPosX, nPosY );

        m_pGaeaClient->GetCharacter()->ReqNpcDirectSaleTo( GetMarketWindowNPCID(), GetMarketWindowNPCGlobID(), nPosX, nPosY );
    }
}

void CInnerInterface::ModalExChangeBuySellItem(DWORD dwMsg, UIGUID nCallerID)
{
	if ( dwMsg & UIMSG_MODAL_OK )
	{
		int nPageIndex = m_pDialogueWindowRenewal->GetPageIndex();

		int nPosX = nOUTOFRANGE;
		int nPosY = nOUTOFRANGE;
		if ( nCallerID == UI::MODAL_NPC_EXCHANGE_BUY_ITEM )
		{
			m_pDialogueWindowRenewal->GetItemIndex( &nPosX, &nPosY );
			if ( nPosX < 0 || nPosY < 0 ) return ;
		}
		CDebugSet::ToView( 1, 3, "사는 아이템 : %d %d", nPosX, nPosY );

		GLMSG::SNETPC_REQ_EXCHANGE_BUY_FROM_NPC sNetData;

		sNetData.sNID		 = GetMarketWindowNPCID();
		sNetData.dwNpcGlobID = GetMarketWindowNPCGlobID();
		//sNetData.dwChannel   = nPageIndex;
		sNetData.itemlist[0].dwChannel = nPageIndex;
		sNetData.itemlist[0].wPosX   = nPosX;
		sNetData.itemlist[0].wPosY   = nPosY;
		sNetData.itemlist[0].wBuyNum = 1;

		m_pGaeaClient->GetCharacter()->ReqExChangeNpcTo( &sNetData );
	}
}

void CInnerInterface::ModalBuySellItem( DWORD dwMsg, UIGUID nCallerID ) 
{
    if ( dwMsg & UIMSG_MODAL_OK )
    {
        int nPageIndex = m_pDialogueWindowRenewal->GetPageIndex();

        int nPosX = nOUTOFRANGE;
        int nPosY = nOUTOFRANGE;
        if ( nCallerID == UI::MODAL_BUY_ITEM )
        {
            m_pDialogueWindowRenewal->GetItemIndex( &nPosX, &nPosY );
            if ( nPosX < 0 || nPosY < 0 ) return ;
        }
        CDebugSet::ToView( 1, 3, "사는 아이템 : %d %d", nPosX, nPosY );

        m_pGaeaClient->GetCharacter()->ReqNpcTo( GetMarketWindowNPCID(), GetMarketWindowNPCGlobID(), nPageIndex, nPosX, nPosY );
    }
}

void CInnerInterface::ModalServerStage(DWORD dwMsg) 
{
    if ( dwMsg & UIMSG_MODAL_OK )
    {
        WAITSERVER_DIALOGUE_OPEN ( ID2GAMEINTEXT("WAITSERVER_MESSAGE"), WAITSERVER_TOLOBY );
    }
}

void CInnerInterface::ModalDlgPileItem(DWORD dwMsg) 
{
    if ( dwMsg & UIMSG_MODAL_OK )
    {
        CString strTemp = UI::ToString( m_pModalWindow->GetEditString() );
        DWORD dwCount = (DWORD) atoi( strTemp.GetString () );
        if ( dwCount )
        {
            SNATIVEID sMarketID = m_pDialogueWindowRenewal->GetNPCID();
            int nPageIndex = m_pDialogueWindowRenewal->GetPageIndex();

            int nPosX = nOUTOFRANGE;
            int nPosY = nOUTOFRANGE;
            m_pDialogueWindowRenewal->GetItemIndex( &nPosX, &nPosY );
            if ( nPosX < 0 || nPosY < 0 ) return;

            LONGLONG lnMoney = m_pGaeaClient->GetCharacter()->GetInvenMoney();
            const SINVENITEM* pInvenItem = m_pDialogueWindowRenewal->GetItem( nPosX, nPosY );
            if (!pInvenItem)
                return;
            const SITEM* pItem = GLogicData::GetInstance().GetItem(pInvenItem->sItemCustom.GetNativeID());
            if (!pItem)
                return;

			if ( dwCount > MAXWORD )
			{
				dwCount = MAXWORD;
			}
            WORD wBuyNum = static_cast<WORD>( dwCount );
            m_pDialogueWindowRenewal->AddCartList( nPageIndex, nPosX, nPosY, wBuyNum );
        }
    }
}

void CInnerInterface::ModalPileItem(DWORD dwMsg) 
{
    if ( dwMsg & UIMSG_MODAL_OK )
    {
        CString strTemp = UI::ToString( m_pModalWindow->GetEditString() );
        DWORD dwCount = (DWORD) atoi( strTemp.GetString () );
        if ( dwCount )
        {
			if ( dwCount > MAXWORD )
			{
				dwCount = MAXWORD;
			}

			bool bTrade = m_pDialogueWindowRenewal->IsCurPageMarketType(GLInventorySale::CROW_SALE_V_2);

            SNATIVEID sMarketID = m_pDialogueWindowRenewal->GetNPCID();
            int nPageIndex = m_pDialogueWindowRenewal->GetPageIndex();

            int nPosX = nOUTOFRANGE;
            int nPosY = nOUTOFRANGE;
            m_pDialogueWindowRenewal->GetItemIndex( &nPosX, &nPosY );
            if ( nPosX < 0 || nPosY < 0 ) return ;

            LONGLONG lnMoney = m_pGaeaClient->GetCharacter()->GetInvenMoney();
            const SINVENITEM* pInvenItem = m_pDialogueWindowRenewal->GetItem( nPosX, nPosY );
            if (!pInvenItem)
                return;
            const SITEM* pItem = GLogicData::GetInstance().GetItem(pInvenItem->sItemCustom.GetBoxWrappedItemID());
            if (!pItem)
                return;

            float fSHOP_RATE = m_pGaeaClient->GetCharacter()->GetBuyRate();
            float fSHOP_RATE_C = fSHOP_RATE * 0.01f;
            LONGLONG dwPrice = static_cast<LONGLONG>( pItem->sBasicOp.dwBuyPrice * fSHOP_RATE_C );
            DWORD dwMaxCount = (DWORD)(lnMoney / (LONGLONG)dwPrice);

            if ( dwMaxCount < 1 && !bTrade)
            {					
                m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::ORANGE, ID2GAMEINTEXT("BUYITEM_NOMONEY") );
                return ;
            }
            //	버튼 만들어지는대로 넣기
            //	최대 개수로 사기
            if ( dwMaxCount < dwCount && !bTrade )
            {												
                std::string strText = sc::string::format(ID2GAMEINTEXT("BUYITEM_MAXIMUM"), pItem->GetName(), dwMaxCount).c_str();
                m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::ORANGE, strText.c_str());
                dwCount = dwMaxCount;
            }

			if( bTrade )
			{
				GLMSG::SNETPC_REQ_EXCHANGE_BUY_FROM_NPC sNetData;

				sNetData.sNID		 = GetMarketWindowNPCID();
				sNetData.dwNpcGlobID = GetMarketWindowNPCGlobID();
				//sNetData.dwChannel   = nPageIndex;
				sNetData.itemlist[0].dwChannel = nPageIndex;
				sNetData.itemlist[0].wPosX   = nPosX;
				sNetData.itemlist[0].wPosY   = nPosY;
				sNetData.itemlist[0].wBuyNum = (WORD)dwCount;

				m_pGaeaClient->GetCharacter()->ReqExChangeNpcTo( &sNetData );
			}
			else
			{
				m_pGaeaClient->GetCharacter()->ReqNpcTo( GetMarketWindowNPCID(), GetMarketWindowNPCGlobID(), nPageIndex, nPosX, nPosY, (WORD)dwCount );
			}
        }
    }
}

void CInnerInterface::ModalConflictRequest(DWORD dwMsg) 
{
    if ( dwMsg & UIMSG_MODAL_OK )
    {
        DWORD dwGaeaID = m_pPtoPWindow->GetPlayerID ();

        SCONFT_OPTION sOption;
        sOption.bBOUND = RANPARAM::bCONFT_BOUND!=FALSE;
        sOption.wRECOVER = RANPARAM::wCONFT_RECOVER;
        sOption.fHP_RATE = RANPARAM::fCONFT_HP_RATE;
        sOption.fTAR_HP_RATE = RANPARAM::fCONFT_TAR_HP_RATE;

        m_pGaeaClient->GetCharacter()->ReqConflict ( dwGaeaID, sOption );
    }
}

void CInnerInterface::ModalConflict(DWORD dwMsg) 
{
    if ( dwMsg & UIMSG_MODAL_OK )
    {
        //	Note : 대련 수락시에 메시지 전송.
        GLMSG::SNETPC_REQ_CONFRONT_ANS NetMsgAns(m_emConflictReqType, m_ConflictReqID, EMCONFRONT_AGREE, m_sReqConflictOption);
        m_pGaeaClient->NETSENDTOAGENT(&NetMsgAns);
    }
    else if ( dwMsg & UIMSG_MODAL_CANCEL )
    {
        //	Note : 대련 거절시에 메시지 전송.
        GLMSG::SNETPC_REQ_CONFRONT_ANS NetMsgAns(m_emConflictReqType, m_ConflictReqID, EMCONFRONT_REFUSE, m_sReqConflictOption);
        m_pGaeaClient->NETSENDTOAGENT(&NetMsgAns);
    }
}

void CInnerInterface::ModalClubStorageSplitItem(DWORD dwMsg) 
{
    if ( dwMsg & UIMSG_MODAL_OK )
    {
        GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
        if ( pCharacter )
        {
            CString strTemp = UI::ToString( m_pModalWindow->GetEditString() );
            WORD wSplitNum = (WORD) atoi ( strTemp.GetString () );

            WORD wPosX, wPosY;
            int nChannel = m_pClubStorageWindow->GetChannel ();
            m_pClubStorageWindow->GetSplitPos ( &wPosX, &wPosY );
            //m_pGaeaClient->ReqClubStorageSplit ( nChannel, wPosX, wPosY, wSplitNum );
        }
    }
}

void CInnerInterface::ModalStorageSplitItem(DWORD dwMsg) 
{
    if ( dwMsg & UIMSG_MODAL_OK )
    {
        GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
        if ( pCharacter )
        {
            CString strTemp = UI::ToString( m_pModalWindow->GetEditString() );
            WORD wSplitNum = (WORD) atoi ( strTemp.GetString () );

            WORD wPosX, wPosY;
            int nChannel = m_pStorageWindow->GetChannel ();
            m_pStorageWindow->GetSplitPos ( &wPosX, &wPosY );					
            pCharacter->ReqStorageSplit ( GetStorageWindowNPCID(), GetStorageWindowNPCGlobID(), nChannel, wPosX, wPosY, wSplitNum );
        }
    }
}

void CInnerInterface::ModalSplitItem(DWORD dwMsg) 
{
    if ( dwMsg & UIMSG_MODAL_OK )
    {
        GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
        if( pCharacter )
        {
            WORD wPosX, wPosY;

            CString strTemp = UI::ToString( m_pModalWindow->GetEditString() );
            WORD wSplitNum = (WORD)atoi( strTemp.GetString () );

            if( m_pInventory )
                m_pInventory->GetConsumeSplitInvenPos( wPosX, wPosY );
            else
                m_pInventoryWindow->GetSplitPos( &wPosX, &wPosY );

            pCharacter->ReqInvenSplit( wPosX, wPosY, wSplitNum );
        }
    }
}

void CInnerInterface::ModalClubLoadMoney(DWORD dwMsg) 
{
    if (dwMsg & UIMSG_MODAL_OK)
    {
        GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
        if (pCharacter)
        {
            CString strTemp = UI::ToString(m_pModalWindow->GetEditString());
            LONGLONG lnLoadMoney = _ttoi64(strTemp.GetString());
            LONGLONG lnMoney = m_pGaeaClient->GetMyClub()->GetStorageMoney();
            if (lnMoney < lnLoadMoney)
                lnLoadMoney = lnMoney;
            m_pGaeaClient->ReqClubStorageDrawMoney(lnLoadMoney);
        }
    }
}

void CInnerInterface::ModalLoadMoney(DWORD dwMsg) 
{
    if ( dwMsg & UIMSG_MODAL_OK )
    {
        GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
        if ( pCharacter )
        {
            CString strTemp = UI::ToString( m_pModalWindow->GetEditString() );
            LONGLONG lnLoadMoney = _ttoi64 ( strTemp.GetString () );
            LONGLONG lnMoney = pCharacter->GetStorageMoney();
            if ( lnMoney < lnLoadMoney ) lnLoadMoney = lnMoney;

            pCharacter->ReqStorageDrawMoney ( GetStorageWindowNPCID(), GetStorageWindowNPCGlobID(), lnLoadMoney );
        }
    }
}

void CInnerInterface::ModalClubSaveMoney(DWORD dwMsg) 
{
    if (dwMsg & UIMSG_MODAL_OK)
    {
        GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
        if (pCharacter)
        {
            CString strTemp = UI::ToString(m_pModalWindow->GetEditString());
            LONGLONG lnSaveMoney = _ttoi64(strTemp.GetString());
            LONGLONG lnMoney = pCharacter->GetInvenMoney();
            if (lnMoney < lnSaveMoney)
                lnSaveMoney = lnMoney;

            m_pGaeaClient->ReqClubStorageSaveMoney(lnSaveMoney);
        }
    }
}

void CInnerInterface::ModalSaveMoney(DWORD dwMsg) 
{
    if ( dwMsg & UIMSG_MODAL_OK )
    {
        GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
        if ( pCharacter )
        {
            CString strTemp = UI::ToString( m_pModalWindow->GetEditString() );
            LONGLONG lnSaveMoney = _ttoi64 ( strTemp.GetString () );
            LONGLONG lnMoney = pCharacter->GetInvenMoney();
            if ( lnMoney < lnSaveMoney ) lnSaveMoney = lnMoney;

            pCharacter->ReqStorageSaveMoney ( GetStorageWindowNPCID(), GetStorageWindowNPCGlobID(), lnSaveMoney );
        }
    }
}

void CInnerInterface::ModalBettingMoney(DWORD dwMsg) 
{
	if ( dwMsg & UIMSG_MODAL_OK )
	{
		CString strTemp = UI::ToString( m_pModalWindow->GetEditString() );
		LONGLONG lnMoney = _ttoi64 ( strTemp.GetString () );
		m_pGaeaClient->GetTournamentClient()->SetBettingMoney(lnMoney);
	}
}


void CInnerInterface::ModalTradeMoney(DWORD dwMsg) 
{
    if ( dwMsg & UIMSG_MODAL_OK )
    {
        CString strTemp = UI::ToString( m_pModalWindow->GetEditString() );
        LONGLONG lnMoney = _ttoi64 ( strTemp.GetString () );
        m_pGaeaClient->GetTradeClient()->ReqTradeMoney(lnMoney);
    }
}

void CInnerInterface::ModalRecoveryNpcExp(DWORD dwMsg) 
{
    if ( dwMsg & UIMSG_MODAL_OK )
    {
        int NpcID = 0;
        int DummyData = 0;
        m_pModalWindow->GetModalData( &NpcID, &DummyData );

        m_pGaeaClient->GetCharacter()->ReqRecoveryExpNpc( NpcID );				
    }
}

void CInnerInterface::ModalRecoveryExp(DWORD dwMsg) 
{
    if ( dwMsg & UIMSG_MODAL_OK )
    {
        if ( m_pGaeaClient->GetCharacter()->IsDie () )
        {
            m_pGaeaClient->GetCharacter()->ReqResurrect(EM_RESURRECTION_SPECIAL);
        }
    }
    if ( dwMsg & UIMSG_MODAL_CANCEL )
    {
        if ( m_pGaeaClient->GetCharacter()->IsDie () )
        {
            // 부활하기
            m_pGaeaClient->GetCharacter()->ReqResurrect(EM_RESURRECTION_NOMAL);

            // 열려진 창들 닫기
            CloseAllWindow ();				
        }
    }
}

void CInnerInterface::ModalReBirth(DWORD dwMsg) 
{
    if ( dwMsg & UIMSG_MODAL_OK )
    {
        if ( m_pGaeaClient->GetCharacter()->IsDie () )
        {
            // 부활하기
            m_pGaeaClient->GetCharacter()->ReqResurrect(EM_RESURRECTION_NOMAL);
            SetReviveSkill( false );

            // 열려진 창들 닫기
            CloseAllWindow ();				
        }
    }

    if ( dwMsg & UIMSG_MODAL_CANCEL )
    {
        if ( m_pGaeaClient->GetCharacter()->IsDie () )  
        {
            if ( m_pRebirthDialogue->IsRecoveryAction() )
            {	
                m_pGaeaClient->GetCharacter()->ReqResurrect(EM_RESURRECTION_SPECIAL);
                SetReviveSkill( false );
            }
            else if ( m_pRebirthDialogue->IsReviveAction() ) 
            {
                m_pGaeaClient->GetCharacter()->ReqResurrect(EM_RESURRECTION_SKILL);
                SetReviveSkill( false );
                CloseAllWindow ();				
            }
            else
            {
                // 귀혼주 사용
                m_pGaeaClient->GetCharacter()->ReqResurrect(EM_RESURRECTION_SPECIAL_ITEM);
                SetReviveSkill( false );
                // 열려진 창들 닫기
                CloseAllWindow ();						
            }
        }
    }
}

void CInnerInterface::ModalVehicleWith(DWORD dwMsg) 
{
    if ( dwMsg & UIMSG_MODAL_OK )
    {
        GLMSG::SNET_WITH_VEHICLE_TAR_ANS NetMsg;
        NetMsg.dwMasterID = GetVehicleWindow()->GetMasterID ();
        NetMsg.emAns = EMWITH_VEHICLE_FB_OK;
        m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
    }
    else if ( dwMsg & UIMSG_MODAL_CANCEL )
    {
        GLMSG::SNET_WITH_VEHICLE_TAR_ANS NetMsg;
        NetMsg.dwMasterID = GetVehicleWindow()->GetMasterID ();
        NetMsg.emAns = EMWITH_VEHICLE_CANCEL;
        m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
    }
}

void CInnerInterface::ModalTradeRequest(DWORD dwMsg) 
{
    if ( dwMsg & UIMSG_MODAL_OK )
    {				
        GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();      

        // P2P 거래 수락 처리
        GLMSG::SNET_TRADE_TAR_ANS NetMsg;
        NetMsg.dwChaNum = pCharacter->CharDbNum();
        NetMsg.dwMasterID = GetTradeWindow()->GetMasterID ();
        NetMsg.emAns = EMTRADE_OK;
        NetMsg.m_Crc32 = sc::string::getCrc32(&NetMsg, NetMsg.Size()-sizeof(unsigned int));
        m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
    }
    else if ( dwMsg & UIMSG_MODAL_CANCEL )
    {
        GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
        //	P2P 거래 거절 처리
        GLMSG::SNET_TRADE_TAR_ANS NetMsg;
        NetMsg.dwChaNum = pCharacter->CharDbNum();
        NetMsg.dwMasterID = GetTradeWindow()->GetMasterID ();
        NetMsg.emAns = EMTRADE_CANCEL;
        NetMsg.m_Crc32 = sc::string::getCrc32(&NetMsg, NetMsg.Size()-sizeof(unsigned int));
        m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
    }
}

void CInnerInterface::ModalPartyRequest(DWORD dwMsg) 
{
    if ( dwMsg & UIMSG_MODAL_OK )
    {
        DWORD dwGaeaID = m_pPtoPWindow->GetPlayerID();
		m_pGaeaClient->GetPartyManager()->Lure(dwGaeaID);
    }
}

void CInnerInterface::ModalPartyRequestByCharDbNum(DWORD dwMsg) 
{
    if ( dwMsg & UIMSG_MODAL_OK )
    {
        SPartyOption sOption(RANPARAM::dwPARTY_GET_ITEM, RANPARAM::dwPARTY_GET_MONEY);

        //	Note : 서버에 상대편과의 파티 요청.
        //
        GLMSG::SNET_PARTY_LURE_WITH_CHARNUM NetMsg( sOption );
        NetMsg.dwChaNum = m_pPartyModalWindow->GetCharDbNum();
        m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
    }
}

void CInnerInterface::ModalPartyTarget(DWORD dwMsg) 
{
	IPartyWindow* const pPartyWindow(GetPartyWindow());
    if ( dwMsg & UIMSG_MODAL_OK )
    {
        //	파티 수락 처리
        GLMSG::SNET_PARTY_LURE_TAR_ANS NetMsg( EMPARTY_LURE_OK
			, pPartyWindow->GetPartyID()
			, pPartyWindow->GetHashKey()
            , pPartyWindow->GetMasterID()
			, pPartyWindow->GetTimeKey()
			, m_sReqPartyOption );
        m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
    }
    else if ( dwMsg & UIMSG_MODAL_CANCEL )
    {
        //	파티 거절 처리
        GLMSG::SNET_PARTY_LURE_TAR_ANS NetMsg( EMPARTY_LURE_REFUSE
			, pPartyWindow->GetPartyID()
			, 0
			, pPartyWindow->GetMasterID()
			, 0
			, SPartyOption() );
        m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
    }
}

void CInnerInterface::ModalChatGroupRequest(DWORD dwMsg) 
{
	if ( dwMsg & UIMSG_MODAL_OK )
	{
		GLMSG::SNET_GCHAT_INVITE_CAC NetMsg;
		NetMsg.dwKey = m_pGaeaClient->GETMYGAEAID();
        NetMsg.dwChaNumWhom = m_pChatGroupModalWindow->Getdwkey();
		m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
	}
}

void CInnerInterface::ModalChatGroupTarget(DWORD dwMsg) 
{
	if ( dwMsg & UIMSG_MODAL_OK )
	{
		//	파티 수락 처리
		GLMSG::SNET_GCHAT_INVITE_CAC_FB NetMsg;
		NetMsg.bAccept = TRUE;
		NetMsg.dwKey = GetChatGroupModalWindow()->Getdwkey();
		NetMsg.dwChaWhom = m_pGaeaClient->GetCharacter()->GetCharID();
		m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
	}
	else if ( dwMsg & UIMSG_MODAL_CANCEL )
	{
		//	파티 거절 처리
		GLMSG::SNET_GCHAT_INVITE_CAC_FB NetMsg;
		NetMsg.bAccept = FALSE;
		NetMsg.dwKey = GetChatGroupModalWindow()->Getdwkey();
		NetMsg.dwChaWhom = m_pGaeaClient->GetCharacter()->GetCharID();
		m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
	}
}

void CInnerInterface::ModalInventoryMoneyToField(DWORD dwMsg) 
{
    if ( dwMsg & UIMSG_MODAL_OK )
    {
        GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
        if ( pCharacter )
        {
            CString strTemp = UI::ToString( m_pModalWindow->GetEditString() );
            LONGLONG lnDropMoney = _ttoi64 ( strTemp.GetString () );
            LONGLONG lnMoney = pCharacter->GetInvenMoney();
            if ( lnMoney < lnDropMoney ) lnDropMoney = lnMoney;

            pCharacter->ReqMoneyToField ( lnDropMoney );
        }
    }
}

void CInnerInterface::ModalCloseGame(DWORD dwMsg) 
{
    if ( dwMsg & UIMSG_MODAL_OK )
    {
        WAITSERVER_DIALOGUE_OPEN ( ID2GAMEINTEXT("WAITSERVER_MESSAGE"), WAITSERVER_CLOSEGAME );
    }
}


void CInnerInterface::ModalStorageConnectItemUseRequest(DWORD dwMsg)
{
    if( dwMsg & UIMSG_MODAL_OK )
    {
        int nPosX = 0;
        int nPosY = 0;
        m_pModalWindow->GetModalData( &nPosX, &nPosY );

        m_pGaeaClient->GetCharacter()->ReqStorageOpenCard( WORD( nPosX ), WORD( nPosY ) );
    }
}

void CInnerInterface::ModalForceReBirthSkill(DWORD dwMsg)
{
    if ( dwMsg & UIMSG_MODAL_OK )
    {
        m_pGaeaClient->GetCharacter()->ReqResurrect(EM_RESURRECTION_SKILL);
        CloseAllWindow ();
    }
}

void CInnerInterface::ModalSellItemFromInven(DWORD dwMsg)
{
    if ( dwMsg & UIMSG_MODAL_OK )
    {
        int nPosX = 0;
        int nPosY = 0;

        m_pModalWindow->GetModalData( &nPosX, &nPosY );
        m_pGaeaClient->GetCharacter()->ReqNpcSaleTo( GetMarketWindowNPCID(), GetMarketWindowNPCGlobID(), static_cast<WORD>(nPosX), static_cast<WORD>(nPosY) );
    }
}

void CInnerInterface::ModalItemBankGetItem(DWORD dwMsg)
{
    if ( dwMsg & UIMSG_MODAL_OK )
    {
        int nPosX = 0;
        int nPosY = 0;

        m_pModalWindow->GetModalData( &nPosX, &nPosY );
        m_pGaeaClient->GetCharacter()->ReqChargedItemTo ( static_cast<WORD>(nPosX), static_cast<WORD>(nPosY) );
    }
}

void CInnerInterface::ModalTutorialSkip(DWORD dwMsg)
{
    if ( dwMsg & UIMSG_MODAL_OK )
    {
        if ( UiIsVisibleGroup( TUTORIAL_SKIP_BUTTON ) )
        {
            UiHideGroup( TUTORIAL_SKIP_BUTTON );
        }

        m_pGaeaClient->GetTutorial()->SkipTutorial();
    }
}

void CInnerInterface::ModalCostumeColor(DWORD dwMsg)
{
    if ( dwMsg & UIMSG_MODAL_OK )
    {
        UiShowGroupFocus( COSTUM_COLOR_CHANGE_WINDOW );
    }
}

void CInnerInterface::ModalSnsTwitterFollow(DWORD dwMsg)
{
    if ( dwMsg & UIMSG_MODAL_OK )
    {
        int nValue1 = -1;
        int nValue2 = -1;

        GetModalWindow()->GetModalData( &nValue1, &nValue2 );

        DWORD dwGaeaID = (DWORD)nValue1;

        std::tr1::shared_ptr<GLCharClient> pCharClient = m_pGaeaClient->GetChar( dwGaeaID );
        if (pCharClient)
            m_pGaeaClient->GetSNSClient()->TTFollowReq( pCharClient ); 
    }
}

void CInnerInterface::ModalSnsFacebookAddFriend(DWORD dwMsg)
{
    if ( dwMsg & UIMSG_MODAL_OK )
    {
        int nValue1 = -1;
        int nValue2 = -1;

        GetModalWindow()->GetModalData( &nValue1, &nValue2 );

        DWORD dwGaeaID = (DWORD)nValue1;

        std::tr1::shared_ptr<GLCharClient> pCharClient = m_pGaeaClient->GetChar( dwGaeaID );
        if (pCharClient)
            m_pGaeaClient->GetSNSClient()->FBFriendAddReq( pCharClient ); 
    }
}

void CInnerInterface::ModalReqHoldToField(DWORD dwMsg)
{
    if ( dwMsg & UIMSG_MODAL_OK )
    {
        D3DXVECTOR3 vDropPos;

        m_pModalWindow->GetModalData( &vDropPos );

        m_pGaeaClient->GetCharacter()->ReqHoldToField( vDropPos );
    }
}

void CInnerInterface::ModalPostBoxPageSendButtonSendCommision(DWORD dwMsg)
{
    if ( dwMsg & UIMSG_MODAL_OK )
    {
        if ( m_pPostBoxWindow )
        {
            m_pPostBoxWindow->ReqSendPost();
        }
    }
}

void CInnerInterface::ModalPostBoxPageReadButtonTake(DWORD dwMsg)
{
    if ( dwMsg & UIMSG_MODAL_OK )
    {
        if (m_pPostBoxWindow)
        {
            m_pPostBoxWindow->ReqPostTakeItem();
        }
    }
}

void CInnerInterface::ModalPostBoxPageReceiveButtonDeleteCommit(DWORD dwMsg)
{
    if ( dwMsg & UIMSG_MODAL_OK )
    {
        if (m_pPostBoxWindow)
        {
            m_pPostBoxWindow->ReqMultiDelete_Commit();
        }
    }
    /*
    else if ( dwMsg & UIMSG_MODAL_CANCEL )
    {
    if (m_pPostBoxWindow)
    {
    m_pPostBoxWindow->ReqMultiDelete_Commit();
    }
    }
    */
}

void CInnerInterface::ModalPostBoxPageReceiveButtonDeleteCheck(DWORD dwMsg)
{
    if ( dwMsg & UIMSG_MODAL_OK )
    {				
        if (m_pPostBoxWindow)
        {
            m_pPostBoxWindow->ReqMultiDelete_Check();
        }
    }
}

void CInnerInterface::ModalPostBoxPageReadButtonReturn(DWORD dwMsg)
{
    if ( dwMsg & UIMSG_MODAL_OK )
    {
        if (m_pPostBoxWindow)
        {
            m_pPostBoxWindow->ReqPostReturn();
        }
    }
}

void CInnerInterface::ModalPostBoxPageReadButtonDelete(DWORD dwMsg)
{
    if ( dwMsg & UIMSG_MODAL_OK )
    {
        if (m_pPostBoxWindow)
        {
            m_pPostBoxWindow->ReqPostDelete();
        }
    }
}

void CInnerInterface::ModalPostBoxPageReadButtonDeleteConfirmAttach(DWORD dwMsg)
{
    if ( dwMsg & UIMSG_MODAL_OK )
    {
        if (m_pPostBoxWindow)
        {
            m_pPostBoxWindow->ReqPostDelete( false );
        }
    }
}

void CInnerInterface::ModalVehicleColor(DWORD dwMsg)
{
    if ( dwMsg & UIMSG_MODAL_OK )
    {
        UiShowGroupFocus( VEHICLE_COLOR_CHANGE_WINDOW );
    }
}

void CInnerInterface::ModalSkillFactLunchBoxOverWrite(DWORD dwMsg)
{
    if( UIMSG_MODAL_OK & dwMsg )
    {
        int nPosX = 0;
        int nPosY = 0;

        m_pModalWindow->GetModalData( &nPosX, &nPosY );

        m_pGaeaClient->GetCharacter()->ReqInvenLunchBox( (WORD)nPosX, (WORD)nPosY );
    }
}

void CInnerInterface::ModalSkillFactRemove(DWORD dwMsg)
{
    if( UIMSG_MODAL_OK & dwMsg )
    {
        int nSkillID = UINT_MAX;
        int nSlot    = 0;

        m_pModalWindow->GetModalData( &nSkillID, &nSlot );

        m_pGaeaClient->GetCharacter()->ReqSkillHoldReset( SNATIVEID((DWORD)nSkillID), (WORD)nSlot );
    }
}

void CInnerInterface::ModalItemMixOkQuestion(DWORD dwMsg)
{
    if( UIMSG_MODAL_OK & dwMsg )
    {
        m_pItemMixWindow->SetAnimation();
    }
}

void CInnerInterface::ModalGenderChangeEnd(DWORD dwMsg)
{
    if( UIMSG_MODAL_OK & dwMsg )
    {
        WAITSERVER_DIALOGUE_OPEN ( ID2GAMEINTEXT("WAITSERVER_MESSAGE"), WAITSERVER_TOLOBY );
    }
}

void CInnerInterface::ModalGenderChange(DWORD dwMsg)
{
    if( UIMSG_MODAL_OK & dwMsg )
    {
		//성별 변경전 성별 전용 아이템 착용 개수 체크
		GLInventory* pInventory = &m_pGaeaClient->GetCharacterData().m_cInventory;

		int dwNeedfullInventory = 0;
		for ( int i = 0; i < SLOT_TSIZE; i++ )
		{
			SITEMCUSTOM* pItemcustom = &m_pGaeaClient->GetCharacterData().m_PutOnItems[i];
			if ( pItemcustom )
			{
				SITEM *pITEM = GLogicData::GetInstance().GetItem(pItemcustom->GetNativeID());
				if (pITEM)
				{
					if ( !(GLCC_WOMAN&pITEM->sBasicOp.dwReqCharClass) || !(GLCC_MAN&pITEM->sBasicOp.dwReqCharClass) )
						dwNeedfullInventory++;
					else
					{
						if ( pItemcustom->nidDISGUISE != NATIVEID_NULL() )
						{
							SITEM *pITEM = GLogicData::GetInstance().GetItem(pItemcustom->nidDISGUISE);
							if (pITEM)
							{
								if ( !(GLCC_WOMAN&pITEM->sBasicOp.dwReqCharClass) || !(GLCC_MAN&pITEM->sBasicOp.dwReqCharClass) )
								{
									dwNeedfullInventory++;
								}
							}
						}
					}
				}
			}
		}
		//성별 전용 아이템 개수와 인벤토리 빈공간 체크
		DWORD dwAmountEmpty = pInventory->GetAmountEmpty(FALSE);
		int dwEmptyInventory = 0;

		dwEmptyInventory = static_cast<int>(dwAmountEmpty) - dwNeedfullInventory;

		if ( dwEmptyInventory < 0 )
		{
			std::string strText = sc::string::format( ID2GAMEINTEXT("EMINVEN_GENDER_CHANGE_FAIL_NOT_ENOUGH_INVEN"), -dwEmptyInventory );
			PrintMsgDlg( NS_UITEXTCOLOR::RED, strText.c_str() );
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::RED, strText.c_str() );
			return;
		}

        UiShowGroupFocus( GENDER_CHANGE_WINDOW );
    }
}

void CInnerInterface::ModalMobileNumber(DWORD dwMsg)
{
    if( UIMSG_MODAL_OK & dwMsg )
    {
        CString strInputNumber = UI::ToString(m_pModalWindow->GetEditString());
        //m_pGaeaClient->GetCharacter()->ReqSetPhoneNumber(strInputNumber);
    }
}

void CInnerInterface::ModalPetColorInfo(DWORD dwMsg)
{
    if ( dwMsg & UIMSG_MODAL_OK )
    {
        UiShowGroupFocus( PETCOLORCARD_WINDOW );
    }
}

void CInnerInterface::ModalHairColorInfo(DWORD dwMsg)
{
    if ( dwMsg & UIMSG_MODAL_OK )
    {
        UiShowGroupFocus( HAIRCOLORCARD_WINDOW );
    }
}

void CInnerInterface::ModalItemGarbageOkQuestion(DWORD dwMsg)
{
    if( UIMSG_MODAL_OK & dwMsg )
    {
        m_pGaeaClient->GetCharacter()->ReqGarbageResult();
    }
}

void CInnerInterface::ModalItemRebuildOkQuestion(DWORD dwMsg)
{
    if( UIMSG_MODAL_OK & dwMsg )
    {
		m_pGaeaClient->GetCharacter()->ReqRebuildResult( GetItemRebuildWindowNPCID(), GetItemRebuildWindowNPCGlobID() );
    }
}

void CInnerInterface::ModalItemRebuildMoney(DWORD dwMsg)
{
    if( UIMSG_MODAL_OK & dwMsg )
    {
        CString strInputMoney = UI::ToString( m_pModalWindow->GetEditString() );
        LONGLONG i64InputMoney = _ttoi64( strInputMoney.GetString() );
        m_pGaeaClient->GetCharacter()->ReqRebuildInputMoney( i64InputMoney );
    }
}

void CInnerInterface::ModalResetSkillStats(DWORD dwMsg)
{
    if ( UIMSG_MODAL_OK & dwMsg )
    {
        if (m_pStatsResetWindow)
        {
            m_pStatsResetWindow->ReqResetSkillStats();
            m_pStatsResetWindow->Close();
        }
    }
}

void CInnerInterface::ModalPetRename(DWORD dwMsg)
{
    if ( UIMSG_MODAL_OK & dwMsg )
    {
        CString strName = UI::ToString( m_pModalWindow->GetEditString () );
        m_pGaeaClient->GetPetClient()->ReqRename( strName.GetString() );
    }
}

void CInnerInterface::ModalCharRename(DWORD dwMsg)
{
    if ( UIMSG_MODAL_OK & dwMsg )
    {
        CString strName = UI::ToString( m_pModalWindow->GetEditString() );
        m_pGaeaClient->GetCharacter()->ReqInvenRename( strName.GetString() );
    }
}

void CInnerInterface::ModalUnionBreakUp(DWORD dwMsg)
{
    if ( UIMSG_MODAL_OK & dwMsg )
    {
        m_pGaeaClient->ReqClubAllianceDis();
    }
}

void CInnerInterface::ModalUnionLeave(DWORD dwMsg)
{
    if ( UIMSG_MODAL_OK & dwMsg )
    {
        //	탈퇴 Req
        m_pGaeaClient->ReqClubAllianceSec();
    }
}

void CInnerInterface::ModalUnionKick(DWORD dwMsg)
{
    if ( UIMSG_MODAL_OK & dwMsg )
    {
        const DWORD dwClubID = m_pClubWindow->GetUnionMemberID();
        m_pGaeaClient->ReqClubAllianceDel( dwClubID );
    }
}

void CInnerInterface::ModalUnionWithdraw(DWORD dwMsg)
{
	if ( UIMSG_MODAL_OK & dwMsg )
	{
		const DWORD dwClubID = m_pClubWindow->GetUnionMemberID();
		m_pGaeaClient->ReqClubAllianceWithdraw( dwClubID );
	}
}

void CInnerInterface::ModalAllianceBattleSubmission(DWORD dwMsg)
{
    if ( UIMSG_MODAL_OK & dwMsg )
    {
        DWORD dwClubID = m_pClubWindow->GetBattleClubID ();
        m_pGaeaClient->ReqAllianceBattleSubmission( dwClubID );
    }
}

void CInnerInterface::ModalClubBattleSubmission(DWORD dwMsg)
{
    if ( UIMSG_MODAL_OK & dwMsg )
    {
        DWORD dwClubID = m_pClubWindow->GetBattleClubID ();
        m_pGaeaClient->ReqClubBattleSubmission( dwClubID );
    }
}

void CInnerInterface::ModalAllianceBattleArmisticeAsk(DWORD dwMsg)
{
    if ( UIMSG_MODAL_OK & dwMsg )
    {						
        m_pGaeaClient->ReqAllianceBattleArmisticeAns( m_dwClubMasterID, m_ReqChaDbNum, true );
    }
    else if ( UIMSG_MODAL_CANCEL & dwMsg )
    {
        m_pGaeaClient->ReqAllianceBattleArmisticeAns( m_dwClubMasterID, m_ReqChaDbNum, false );
    }
}

void CInnerInterface::ModalClubBattleArmisticeAsk(DWORD dwMsg)
{
    if ( UIMSG_MODAL_OK & dwMsg )
    {						
        m_pGaeaClient->ReqClubBattleArmisticeAns( m_dwClubMasterID, m_ReqChaDbNum, true );
    }
    else if ( UIMSG_MODAL_CANCEL & dwMsg )
    {
        m_pGaeaClient->ReqClubBattleArmisticeAns( m_dwClubMasterID, m_ReqChaDbNum, false );
    }
}

void CInnerInterface::ModalAllianceBattleArmistice(DWORD dwMsg)
{
    if ( UIMSG_MODAL_OK & dwMsg )
    {
        DWORD dwClubID = m_pClubWindow->GetBattleClubID ();
        m_pGaeaClient->ReqAllianceBattleArmistice( dwClubID );
    }
}

void CInnerInterface::ModalClubBattleArmistice(DWORD dwMsg)
{
    if ( UIMSG_MODAL_OK & dwMsg )
    {
        DWORD dwClubID = m_pClubWindow->GetBattleClubID ();
        m_pGaeaClient->ReqClubBattleArmistice( dwClubID );
    }
}

void CInnerInterface::ModalAllianceBattleAsk(DWORD dwMsg)
{
    if ( UIMSG_MODAL_OK & dwMsg )
    {						
        m_pGaeaClient->ReqAllianceBattleAns( m_dwClubMasterID, true );
    }
    else if ( UIMSG_MODAL_CANCEL & dwMsg )
    {
        m_pGaeaClient->ReqAllianceBattleAns( m_dwClubMasterID, false );
    }
}

void CInnerInterface::ModalClubBattleAsk(DWORD dwMsg)
{
    if ( UIMSG_MODAL_OK & dwMsg )
    {						
        m_pGaeaClient->ReqClubBattleAns( m_dwClubMasterID, true );
    }
    else if ( UIMSG_MODAL_CANCEL & dwMsg )
    {
        m_pGaeaClient->ReqClubBattleAns( m_dwClubMasterID, false );
    }
}

void CInnerInterface::ModalUnionJoin(DWORD dwMsg)
{
    if ( UIMSG_MODAL_OK & dwMsg )
    {
        DWORD dwGaeaID = m_pPtoPWindow->GetPlayerID ();
        m_pGaeaClient->ReqClubAlliance( dwGaeaID );
    }
}

void CInnerInterface::ModalUnionJoinAsk(DWORD dwMsg)
{
    if ( UIMSG_MODAL_OK & dwMsg )
    {						
        m_pGaeaClient->ReqClubAllianceAns( m_dwClubMasterID, true );
    }
    else if ( UIMSG_MODAL_CANCEL & dwMsg )
    {
        m_pGaeaClient->ReqClubAllianceAns( m_dwClubMasterID, false );
    }
}

void CInnerInterface::ModalFriendBlockDel(DWORD dwMsg)
{
    if ( UIMSG_MODAL_OK & dwMsg )
    {
        CString strName = UI::ToString( m_pFriendWindow->GetSelectBlockFriendName() );
        //m_pGaeaClient->GetFriendClient()->ReqFriendBlock ( strName.GetString (), false );
        ADD_FRIEND_BLOCK( strName.GetString(), false );
    }
}

void CInnerInterface::ModalFriendBlockAddB(DWORD dwMsg)
{
    if ( UIMSG_MODAL_OK & dwMsg )
    {
        CString strName = UI::ToString( m_pFriendWindow->GetSelectBlockFriendName() );
        //m_pGaeaClient->GetFriendClient()->ReqFriendBlock ( strName.GetString (), true );
        ADD_FRIEND_BLOCK( strName.GetString(), true );
    }
}

void CInnerInterface::ModalFriendBlockAddN(DWORD dwMsg)
{
    if ( UIMSG_MODAL_OK & dwMsg )
    {
        CString strName = UI::ToString( m_pFriendWindow->GetFriendName() );
        //m_pGaeaClient->GetFriendClient()->ReqFriendBlock ( strName.GetString (), true );
        ADD_FRIEND_BLOCK( strName.GetString(), true );
    }
}

void CInnerInterface::ModalCommission(DWORD dwMsg)
{
    if ( UIMSG_MODAL_OK & dwMsg )
    {
        CString strCommission = UI::ToString( m_pModalWindow->GetEditString() );
        float fCommission = (float) atof ( strCommission.GetString () );
        DWORD dwNPCGlobID = m_pDialogueWindowRenewal->GetNPCGlobalID ();

        m_pGaeaClient->GetCharacter()->ReqGuidCommission ( dwNPCGlobID, fCommission );
    }
}

void CInnerInterface::ModalSkillLevelUpConfirm(DWORD dwMsg)
{
    if ( UIMSG_MODAL_OK & dwMsg )
    {
        SNATIVEID sNativeID ( GetSkillUpID () );
        m_pGaeaClient->GetCharacter()->ReqSkillUp ( sNativeID );
    }
}

void CInnerInterface::ModalClubSelfQuit(DWORD dwMsg)
{
    // 클럽 탈퇴 Req
    if (UIMSG_MODAL_OK & dwMsg)        
        m_pGaeaClient->ReqClubSecede();
}

void CInnerInterface::ModalClubBreakUpCancel(DWORD dwMsg)
{
    if (UIMSG_MODAL_OK & dwMsg)
        m_pGaeaClient->ReqClubDissolutionCancel();
}

void CInnerInterface::ModalClubJoin(DWORD dwMsg)
{
    if ( UIMSG_MODAL_OK & dwMsg )
    {
        DWORD dwGaeaID = m_pPtoPWindow->GetPlayerID ();
        m_pGaeaClient->ReqClubJoin ( dwGaeaID );
    }
}

void CInnerInterface::ModalClubJoinAsk(DWORD dwMsg)
{
    if (UIMSG_MODAL_OK & dwMsg)
        m_pGaeaClient->ReqClubJoinAns(m_dwClubMasterID, true);
    else if (UIMSG_MODAL_CANCEL & dwMsg)
        m_pGaeaClient->ReqClubJoinAns(m_dwClubMasterID, false);
}

void CInnerInterface::ModalClubNick(DWORD dwMsg)
{
    if (UIMSG_MODAL_OK & dwMsg)
    {
        CString strNickName = UI::ToString(m_pModalWindow->GetEditString());
        m_pGaeaClient->ReqClubNick(strNickName.GetString());
    }
}

void CInnerInterface::ModalClubChangeMark(DWORD dwMsg)
{
    if (UIMSG_MODAL_OK & dwMsg)
    {
        CString strFileName = UI::ToString(m_pModalWindow->GetEditString());
        m_pGaeaClient->ReqClubMarkChange(strFileName.GetString());
    }
}

void CInnerInterface::ModalClubBreakUp(DWORD dwMsg)
{
    if (UIMSG_MODAL_OK & dwMsg)
        m_pGaeaClient->ReqClubDissolution();
}

void CInnerInterface::ModalClubAuthorityAsk(DWORD dwMsg)
{
    if ( UIMSG_MODAL_OK & dwMsg )
    {						
        m_pGaeaClient->ReqClubAuthorityAns( true );
    }
    else if ( UIMSG_MODAL_CANCEL & dwMsg )
    {
        m_pGaeaClient->ReqClubAuthorityAns( false );
    }
}

void CInnerInterface::ModalClubAuthority(DWORD dwMsg)
{
    if ( UIMSG_MODAL_OK & dwMsg )
    {
        const DWORD MemberDbNum = m_pClubWindow->GetClubMemberID();        
        //m_pGaeaClient->ReqClubAuthority ( dwGaeaID );
        m_pGaeaClient->ReqClubMasterChange(MemberDbNum);
    }
}

void CInnerInterface::ModalClubKick(DWORD dwMsg)
{
    if ( UIMSG_MODAL_OK & dwMsg )
    {
        const DWORD dwGaeaID = m_pClubWindow->GetClubMemberID();				
        m_pGaeaClient->ReqClubMemberDel ( dwGaeaID );
    }
}

void CInnerInterface::ModalCDAuthorityToMember(DWORD dwMsg)
{
    if ( UIMSG_MODAL_OK & dwMsg )
    {
        const DWORD dwGaeaID = m_pClubWindow->GetClubMemberID();
        m_pGaeaClient->ReqClubMemberCd( dwGaeaID, true );
    }
}

void CInnerInterface::ModalCDAuthorityToMaster(DWORD dwMsg)
{
    if ( UIMSG_MODAL_OK & dwMsg )
    {
        const DWORD dwGaeaID = m_pClubWindow->GetClubMemberID();
        m_pGaeaClient->ReqClubMemberCd( dwGaeaID, false );
    }
}

void CInnerInterface::ModalPrivateMarketOnlyUI(DWORD dwMsg)
{
	//	시작
	if ( dwMsg & UIMSG_MODAL_OK )
	{
		m_pGaeaClient->GetCharacter()->ReqPMarketOpen( TRUE );
	}
	else if ( dwMsg & UIMSG_MODAL_CANCEL )
	{
		m_pGaeaClient->GetCharacter()->ReqPMarketOpen( FALSE );
	}
}

void CInnerInterface::ModalPrivateMarketQuit(DWORD dwMsg)
{
    if ( dwMsg & UIMSG_MODAL_OK )
    {
        SetPrivateMarketClose ();
    }
}

void CInnerInterface::ModalPrivateMarketPile(DWORD dwMsg)
{
    if ( dwMsg & UIMSG_MODAL_OK )
    {
        CString strTemp = UI::ToString( m_pModalWindow->GetEditString() );
        DWORD dwCount = (DWORD) atoi ( strTemp.GetString () );
        if ( dwCount )
        {
            //					CMarketWindow* pMarketWindow = GetMarketWindow ();
            DWORD dwMarketID = m_pPrivateMarketWindow->GetPrivateMarketID ();
            //					int	nPageIndex = pMarketWindow->GetPageIndex ();

            int nPosX = nOUTOFRANGE;
            int nPosY = nOUTOFRANGE;
            m_pModalWindow->GetModalData ( &nPosX, &nPosY );
            if ( nPosX < 0 || nPosY < 0 ) return ;

            LONGLONG lnMoney = m_pGaeaClient->GetCharacter()->GetInvenMoney();

            std::tr1::shared_ptr<GLCharClient> pCLIENT = m_pGaeaClient->GetChar( dwMarketID );
            if ( pCLIENT == NULL )
                return;

            GLPrivateMarket& sPMarket = pCLIENT->m_sPMarket;

            const SSALEITEM* pSALE = sPMarket.GetItem( SNATIVEID( nPosX, nPosY ) ); 
            if( pSALE == NULL )
                return;

            const SITEM* pItem = GLogicData::GetInstance().GetItem(pSALE->sITEMCUSTOM.GetBoxWrappedItemID());
            if (!pItem)
                return;

            DWORD dwPrice = static_cast<DWORD>(pSALE->llPRICE);

            DWORD dwMaxCount = (DWORD)(lnMoney / (LONGLONG)dwPrice);
            if ( dwMaxCount < 1 )
            {					
                m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::ORANGE, ID2GAMEINTEXT("BUYITEM_NOMONEY") );
                return ;
            }
            //	버튼 만들어지는대로 넣기
            //	최대 개수로 사기
            if ( dwMaxCount < dwCount )
            {				
                std::string strText = sc::string::format(ID2GAMEINTEXT("BUYITEM_MAXIMUM"), pItem->GetName(), dwMaxCount).c_str();
                m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::ORANGE, strText.c_str());
                dwCount = dwMaxCount;
            }
            m_pGaeaClient->GetCharacter()->ReqPMarketBuy ( dwMarketID, nPosX, nPosY, (WORD)dwCount );
        }
    }
}

void CInnerInterface::ModalPrivateMarketBuy(DWORD dwMsg)
{
    if ( dwMsg & UIMSG_MODAL_OK )
    {	
        DWORD dwMarketID = m_pPrivateMarketWindow->GetPrivateMarketID ();
        //				DWORD dwNum = static_cast<DWORD>(atoi(GetModalWindow()->GetEditString ()));

        int nPosX = nOUTOFRANGE;
        int nPosY = nOUTOFRANGE;

        m_pModalWindow->GetModalData ( &nPosX, &nPosY );
        if ( nPosX < 0 || nPosY < 0 ) return ;

        //CDebugSet::ToView ( 1, 3, "사는 아이템 : %d %d", nPosX, nPosY );

        m_pGaeaClient->GetCharacter()->ReqPMarketBuy ( dwMarketID, nPosX, nPosY, 1 );
    }
}

void CInnerInterface::ModalPrivateMarketSell(DWORD dwMsg)
{
    if ( UIMSG_MODAL_OK & dwMsg )
    {
        WORD wPosX, wPosY;
        m_pPrivateMarketSellWindow->GetItemIndex ( wPosX, wPosY );

        DWORD dwNum = m_pPrivateMarketSellWindow->GetItemCount ();
        LONGLONG llMoney = m_pPrivateMarketSellWindow->GetItemCost ();

        m_pGaeaClient->GetCharacter()->ReqPMarketRegItem ( wPosX, wPosY, llMoney, dwNum );
    }
}

void CInnerInterface::ModalStorageCharge(DWORD dwMsg)
{
    if ( UIMSG_MODAL_OK & dwMsg )
    {
        WORD wPosX, wPosY, wSTORAGE;
        m_pStorageChargeCard->GetSelectedChargeCard ( wPosX, wPosY, wSTORAGE );

        m_pGaeaClient->GetCharacter()->ReqStorageCard ( wPosX, wPosY, wSTORAGE );
    }
}

void CInnerInterface::ModalLotto(DWORD dwMsg)
{
    /*
    if ( UIMSG_MODAL_OK & dwMsg )
    {
        CString strLotto = UI::ToString( m_pModalWindow->GetEditString() );
        int nLength = strLotto.GetLength ();
        int nGAP = 10 - nLength;
        if ( nGAP )
        {
            strLotto += "0";
        }
        DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
        pGlobalStage->NetSendEventLottery(strLotto);
    }
    */
}

void CInnerInterface::ModalDefaultPos(DWORD dwMsg)
{
    if ( UIMSG_MODAL_OK & dwMsg )
    {
        uiman::GetInnerUIMan().MOVE_DEFAULT_POSITION ();
    }
}

void CInnerInterface::ModalAddFriendLureAns(DWORD dwMsg)
{
    if ( UIMSG_MODAL_OK & dwMsg )
    {
        if ( !m_strFriendName.GetLength () )
        {
            GASSERT ( 0 && "이름이 등록되지 않았습니다." );
            return ;
        }

        m_pGaeaClient->GetFriendClient()->ReqFriendLureAns ( m_strFriendName.GetString(), true );
    }
    else if ( UIMSG_MODAL_CANCEL & dwMsg )
    {
        if ( !m_strFriendName.GetLength () )
        {
            GASSERT ( 0 && "이름이 등록되지 않았습니다." );
            return ;
        }

        m_pGaeaClient->GetFriendClient()->ReqFriendLureAns ( m_strFriendName.GetString(), false );
    }
}

void CInnerInterface::ModalFriendMove(DWORD dwMsg)
{
    if ( UIMSG_MODAL_OK & dwMsg )
    {
        SetFriendName ( UI::ToString( m_pFriendWindow->GetFriendName() ) );
        WAITSERVER_DIALOGUE_OPEN ( ID2GAMEINTEXT("WAITSERVER_MESSAGE"), WAITSERVER_TOFRIEND, 10.0f );
    }
}

void CInnerInterface::ModalFriendDel(DWORD dwMsg)
{
    if ( UIMSG_MODAL_OK & dwMsg )
    {
        CString strName = UI::ToString( m_pFriendWindow->GetFriendName() );
        m_pGaeaClient->GetFriendClient()->ReqFriendDel ( strName.GetString () );
    }
}

void CInnerInterface::ModalFriendAdd(DWORD dwMsg)
{
    if ( UIMSG_MODAL_OK & dwMsg )
    {
        CString strName = UI::ToString( m_pFriendWindow->GetFriendName() );
        //m_pGaeaClient->GetFriendClient()->ReqFriendAdd ( strName.GetString () );
        ADD_FRIEND( m_pFriendWindow->GetFriendName().c_str() );
    }
}


void CInnerInterface::MsgEventLotteryFB( NET_MSG_GENERIC* nmg )
{
    /*
    //	로또
    NET_EVENT_LOTTERY_FB* pNetMsg = (NET_EVENT_LOTTERY_FB*)nmg;
    const int nResult = pNetMsg->nResult;

    enum
    {
        LOTTERY_FB_NO1 = 1,
        LOTTERY_FB_NO2,
        LOTTERY_FB_NO3,
        LOTTERY_FB_NO4,
        LOTTERY_FB_NO5,
        LOTTERY_FB_NO6,
        LOTTERY_FB_ALREADY_INPUTED,
        LOTTERY_FB_NOT_PCROOM,
        LOTTERY_FB_ALREADY_USED,
        LOTTERY_FB_ERROR = 10,
    };

    switch ( nResult )
    {
    case LOTTERY_FB_NO1:
    case LOTTERY_FB_NO2:
    case LOTTERY_FB_NO3:
    case LOTTERY_FB_NO4:
    case LOTTERY_FB_NO5:
    case LOTTERY_FB_NO6:
        {
            std::string strTemp = sc::string::format( ID2GAMEINTEXT("LOTTERY_FB_NO1_6"), nResult ).c_str();
            DoModal(strTemp.c_str());
        }
        break;

    case LOTTERY_FB_ALREADY_INPUTED:
        {
            DoModal ( ID2GAMEINTEXT("LOTTERY_FB_ALREADY_INPUTED") );
        }
        break;
    case LOTTERY_FB_NOT_PCROOM:
        {
            DoModal ( ID2GAMEINTEXT("LOTTERY_FB_NOT_PCROOM") );
        }
        break;
    case LOTTERY_FB_ALREADY_USED:
        {
            DoModal ( ID2GAMEINTEXT("LOTTERY_FB_ALREADY_USED") );
        }
        break;
    case LOTTERY_FB_ERROR:
        {
            DoModal ( ID2GAMEINTEXT("LOTTERY_FB_ERROR") );
        }
        break;
    }
    */
}

void CInnerInterface::MsgApexAnswer( NET_MSG_GENERIC* nmg )
{
    NET_APEX_ANSWER* pNetMsg = (NET_APEX_ANSWER*) nmg;
    switch( pNetMsg->wAction )
    {
        // 대만요청으로 경고는 삭제
        //case 1: // 핵을 사용하는것을 발견했을때 유저한테 경고를 주는 메세지
        //	{
        //		std::string strTemp = ID2GAMEINTEXT( "APEX_ACTION_MESSAGE_WARNING" );
        //		PrintConsoleText( strTemp );
        //		DoModal( strTemp.c_str(), UI::MODAL_INFOMATION, UI::OK, NO_ID );
        //	}
        //	break;
    case 2: // 대화 금지
        {
            std::string strText = sc::string::format(ID2GAMEINTEXT("APEX_ACTION_MESSAGE_CHATBLOCK"), (pNetMsg->wHour*60));
            m_pGaeaClient->PrintConsoleText(strText.c_str());
            DoModal(strText.c_str(), UI::MODAL_INFOMATION, UI::OK, NO_ID);
        }
        break;

    case 1: // 핵을 사용하는것을 발견했을때 경고도 접속종료
    case 3: // 유저 킥
    case 4: // 유저 블록
        {
            std::string strTemp = ID2GAMEINTEXT( "APEX_ACTION_MESSAGE_KICK" );
            m_pGaeaClient->PrintConsoleText( strTemp );
            WAITSERVER_DIALOGUE_OPEN( strTemp.c_str(), WAITSERVER_CLOSEGAME, 30.0f );
        }
        break;
    default:
        GASSERT( 0 && "ERROR : DxGameStage::MsgProcess, NET_MSG_APEX_ANSER" );
        break;
    }
}

void CInnerInterface::MsgChatFB( NET_MSG_GENERIC* nmg )
{
    NET_CHAT_CTRL_FB* pNc = (NET_CHAT_CTRL_FB*) nmg;
    DisplayChatMessage ( pNc->emType, NULL, pNc->szChatMsg );
}

void CInnerInterface::MsgChatPrivateFail( NET_MSG_GENERIC* nmg )
{
    GLMSG::SNETPC_CHAT_PRIVATE_FAIL *pNetMsg = (GLMSG::SNETPC_CHAT_PRIVATE_FAIL *) nmg;

    switch (pNetMsg->emFail)
    {
    default:
    case EMCHAT_PRIVATE_FAIL_NOTFOUND:
    case EMCHAT_PRIVATE_FAIL_BLOCK:
        {
            std::string strText = sc::string::format(ID2GAMEINTEXT("CHAT_PRIVATE_FAIL"), pNetMsg->szName);
            m_pGaeaClient->PrintConsoleText(strText);
        }
        break;

    case EMCHAT_PRIVATE_FAIL_GM_IGNORE:
        {
            m_pGaeaClient->PrintConsoleText(ID2GAMEINTEXT("CHAT_PRIVATE_FAIL_GM_IGNORE"));
        }
        break;
    };
}

void CInnerInterface::MsgChatBlock(NET_MSG_GENERIC* nmg)
{
    EMSERVICE_PROVIDER sp =	m_pGaeaClient->GetServiceProvider();

    if (sp == SP_CHINA)
    {
//#ifndef CH_PARAM
        GLMSG::SNETPC_CHAT_BLOCK *pNetMsg = (GLMSG::SNETPC_CHAT_BLOCK *) nmg;
        CTime cTime( pNetMsg->tChatBlock ); // 서버에서 0이 넘어오지 않는다고 가정한다.

        //	서버 타임으로 변환( TimeZone 계산 )
        m_pGaeaClient->GetConvertServerTime( cTime );

        std::string strText = sc::string::format( "[%s]:%02d/%02d/%02d %02d:%02d%s", ID2GAMEWORD("NOTIFY_NAME"), cTime.GetYear()%2000, 
            cTime.GetMonth(), cTime.GetDay(),	cTime.GetHour(), cTime.GetMinute(), ID2GAMEINTEXT("CHAT_BLOCK_MSG") );

        m_pGaeaClient->PrintConsoleText( strText );
    }
//#endif

	if (sp != SP_CHINA)
	{
		GLMSG::SNETPC_CHAT_BLOCK *pNetMsg = (GLMSG::SNETPC_CHAT_BLOCK *) nmg;
		GLCharacter* pChar =  m_pGaeaClient->GetCharacter();
		if( pChar == NULL )
		{
			return;
		}

		pChar->m_tCHATBLOCK = pNetMsg->tChatBlock;

		CTime cTime( CTime::GetCurrentTime() );
		CTime cBLOCK( pNetMsg->tChatBlock );
		m_pGaeaClient->GetConvertServerTime( cTime );

		CTimeSpan cSPAN = cBLOCK - cTime;

		std::string strChatBlock = sc::string::format( ID2GAMEINTEXT("CHAT_BLOCK_STATE"), cSPAN.GetTotalMinutes() );
		m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, strChatBlock );
	}
}

void CInnerInterface::MsgDropOutForced( NET_MSG_GENERIC* nmg )
{
    GLMSG::SNET_DROP_OUT_FORCED *pNetMsg = (GLMSG::SNET_DROP_OUT_FORCED *) nmg;

    //sc::writeLogError(std::string("overlapping login"));
    DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();

    if ( pNetMsg->emForced==EMDROPOUT_REQLOGIN )
    {
		// 로그인 프로세스 확인을 위한 로그
		if( pGlobalStage->m_bLoginProcessLog) sc::writeLogError(std::string("LOGIN ERROR. Forced Drop Out "));

        DoModal ( ID2GAMEINTEXT ("PLAYINTERFACE_8"), UI::MODAL_INFOMATION, UI::OK, UI::MODAL_CLOSEGAME );
    }
    else
    {
        DoModal ( ID2GAMEINTEXT ("PLAYINTERFACE_9"), UI::MODAL_INFOMATION, UI::OK, UI::MODAL_CLOSEGAME );
    }

	m_pGaeaClient->CloseConnect();
}

void CInnerInterface::MsgReqTakeFB( NET_MSG_GENERIC* nmg )
{
    GLMSG::SNETPC_REQ_TAKE_FB *pNetMsg = (GLMSG::SNETPC_REQ_TAKE_FB *)nmg;

    switch ( pNetMsg->emTakeFB )
    {
    case EMTAKE_FB_OFF:
        //pNetMsg->emCrow 사라짐. (GUI)
        switch ( pNetMsg->emCrow)
        {
        case CROW_ITEM:
            {
                //PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("PLAYINTERFACE_10") );
            }
            break;
        case CROW_MONEY:
            {
                //PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("PLAYINTERFACE_11") );
            }
            break;
        }
        break;
    case EMTAKE_FB_RESERVED:
        {
            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("PLAYINTERFACE_12") );
        }
        break;

    case EMTAKE_FB_INVEN_ERROR:
        {
            m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("PLAYINTERFACE_13") );
        }
        break;
    case EMTAKE_FB_TOO_EVENT_ITEM:
        {
            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("PLAYINTERFACE_14") );
        }
        break;
    case EMTAKE_FB_DISTANCE:
        {
            //					PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMTAKE_FB_DISTANCE") );
        }
        break;
	case EMTAKE_FB_TENDERING:
		{
			m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("PLAYINTERFACE_33") );
		}
		break;
    };
}

void CInnerInterface::MsgTradeTarget( NET_MSG_GENERIC* nmg )
{
    GLMSG::SNET_TRADE_TAR* pPacket = (GLMSG::SNET_TRADE_TAR*) nmg;
    if (pPacket->m_Crc32 != sc::string::getCrc32(pPacket, pPacket->Size()-sizeof(unsigned int)))
        return;

    /*GLLandManClient* pLand = m_pGaeaClient->GetActiveMap();
    if (!pLand)
        return;

    std::tr1::shared_ptr<GLCharClient> pChar = pLand->GetChar( pPacket->dwMasterID );
    if (!pChar)
        return;*/

	DWORD dwDbNum = pPacket->dwChaNum;
    std::string strName(pPacket->szMasterName);

    //	Note : 거래 자동 거부.
    //
    bool bPMARKETOPEN = m_pGaeaClient->GetCharacter()->m_sPMarket.IsOpen();
    bool bPOSTBOXOPEN = (0!=m_pGaeaClient->GetCharacter()->IsPostBoxWindowOpen ());

	// 토너먼트중 거래를 할수가 없다.
	// TOURNAMENT_UNIQ_DISABLE
	bool bTOURNAMENT = m_pGaeaClient->GetTournamentClient()->GetIsTournamentView();

    bool bIsBlock = false;
    std::tr1::shared_ptr<SFRIEND> spFriend = m_pGaeaClient->GetFriendClient()->GetData( strName.c_str() );
    if( spFriend != NULL )
        bIsBlock = spFriend->IsBLOCK();

    if ( RANPARAM::bDIS_TRADE || bPMARKETOPEN || bPOSTBOXOPEN || bIsBlock || bTOURNAMENT )
    {
        //	P2P 거래 거절 처리
        GLMSG::SNET_TRADE_TAR_ANS NetMsg;
        NetMsg.dwChaNum = dwDbNum;
        NetMsg.dwMasterID = pPacket->dwMasterID;
        NetMsg.emAns = EMTRADE_CANCEL;
        NetMsg.m_Crc32 = sc::string::getCrc32(&NetMsg, NetMsg.Size()-sizeof(unsigned int));
        m_pGaeaClient->NETSENDTOAGENT(&NetMsg);

        std::string strText(sc::string::format(ID2GAMEINTEXT("TRADE_FB_CANCEL_AUTO"), strName.c_str()));
        m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::DISABLE, strText.c_str());
        return;
    }

    std::string strTemp(sc::string::format( ID2GAMEINTEXT("TRADE_TAR"), strName.c_str()));
    DoModal(strTemp, UI::MODAL_QUESTION, UI::OKCANCEL, UI::MODAL_TRADE_REQ);

    GetTradeWindow()->SetTradeInfo(strName, pPacket->dwMasterID);
}

void CInnerInterface::MsgWithVehicleTarget( NET_MSG_GENERIC* nmg )
{
    GLMSG::SNET_WITH_VEHICLE_TAR *pNetMsg = (GLMSG::SNET_WITH_VEHICLE_TAR *) nmg;
    GLLandManClient* pLand = m_pGaeaClient->GetActiveMap();
    if( !pLand )
        return;

    std::tr1::shared_ptr<GLCharClient> pChar = pLand->GetChar( pNetMsg->dwMasterID );
    if( !pChar )
        return;

    std::string strName( pChar->GetName() );

	// 토너먼트중 파티를 할수가 없다.
	// TOURNAMENT_UNIQ_DISABLE
	bool bTOURNAMNET = m_pGaeaClient->GetTournamentClient()->GetIsTournamentView();

    if ( RANPARAM::bDIS_VEHICLE || bTOURNAMNET ) 
    {
        GLMSG::SNET_WITH_VEHICLE_TAR_ANS NetMsg;
        NetMsg.dwMasterID = pNetMsg->dwMasterID;
        NetMsg.emAns = EMWITH_VEHICLE_CANCEL;
        m_pGaeaClient->NETSENDTOAGENT(&NetMsg);

        std::string strText(
            sc::string::format(
            ID2GAMEINTEXT("VEHICLE_FB_CANCEL_AUTO"), strName.c_str()));
        m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::DISABLE, strText.c_str());
        return;
    }

    std::string strTemp(
        sc::string::format(
        ID2GAMEINTEXT("BIKE_TAR"), strName.c_str()));
    DoModal(strTemp.c_str(), UI::MODAL_QUESTION, UI::OKCANCEL, UI::MODAL_VEHICLE_WITH);
    GetVehicleWindow()->SetVehicleInfoID(pNetMsg->dwMasterID);
}

void CInnerInterface::MsgConfrontTarget( NET_MSG_GENERIC* nmg )
{
    GLMSG::SNETPC_REQ_CONFRONT_TAR *pNetMsg = (GLMSG::SNETPC_REQ_CONFRONT_TAR *)nmg;

    /*std::tr1::shared_ptr<GLCharClient> pChar = m_pGaeaClient->GetChar ( pNetMsg->dwID );
    if( !pChar )
        return;*/

    if ( UiIsVisibleGroup ( CONFT_MODAL_WINDOW ) )
    {
        //	Note : 아직 대련신청에 대한 답변을 하지 않은 경우
        //		   최초 이후의 질문은 모두 거절한다.
        GLMSG::SNETPC_REQ_CONFRONT_ANS NetMsgAns(pNetMsg->emTYPE, pNetMsg->dwID, EMCONFRONT_REFUSE, pNetMsg->sOption);
        m_pGaeaClient->NETSENDTOAGENT(&NetMsgAns);
    }

    //	Note : 대련 자동 거부.
    //
    bool bPOSTBOXOPEN = (0!=m_pGaeaClient->GetCharacter()->IsPostBoxWindowOpen ());

	// 토너먼트중 파티를 할수가 없다.
	// TOURNAMENT_UNIQ_DISABLE
	bool bTOURNAMNET = m_pGaeaClient->GetTournamentClient()->GetIsTournamentView();

    if ( RANPARAM::bDIS_CONFT || bPOSTBOXOPEN || bTOURNAMNET)
    {
        //	Note : 대련 거절시에 메시지 전송.
        GLMSG::SNETPC_REQ_CONFRONT_ANS NetMsgAns(pNetMsg->emTYPE, pNetMsg->dwID, EMCONFRONT_REFUSE, pNetMsg->sOption);
        m_pGaeaClient->NETSENDTOAGENT(&NetMsgAns);

        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, sc::string::format( ID2GAMEINTEXT("EMCONFRONT_REFUSE_AUTO"), pNetMsg->szMasterName ).c_str() );
        return;
    }

    if( m_pGaeaClient->GetCharacter()->VehicleState() /*|| pNetMsg->emVehicleState*/ )
    {
        GLMSG::SNETPC_REQ_CONFRONT_ANS NetMsgAns(pNetMsg->emTYPE, pNetMsg->dwID, EMCONFRONT_VEHICLE, pNetMsg->sOption);
        m_pGaeaClient->NETSENDTOAGENT(&NetMsgAns);

        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCONFRONT_BIKE_FAIL") );
        return;
    }				

    const char *szReqName = pNetMsg->szMasterName;
    m_ConflictReqID = pNetMsg->dwID;
    m_emConflictReqType = pNetMsg->emTYPE;
    m_sReqConflictOption = pNetMsg->sOption;
    UIGUID MODAL_CONFT_TYPE = NO_ID;

    std::string strTemp;
    switch (pNetMsg->emTYPE)
    {
    case EMCONFT_NONE:
        //				GASSERT ( 0 && "오류" );
        return ;
        break;
    case EMCONFT_ONE:
        strTemp = sc::string::format(ID2GAMEINTEXT("CONFLICT_ONE_QUESTION_TAR"), szReqName).c_str();
        MODAL_CONFT_TYPE = UI::MODAL_CONFLICT_ONE_TAR;
        break;
    case EMCONFT_PARTY:
        strTemp = sc::string::format(ID2GAMEINTEXT("CONFLICT_PARTY_QUESTION_TAR"), szReqName).c_str();
        MODAL_CONFT_TYPE = UI::MODAL_CONFLICT_PARTY_TAR;
        break;
    case EMCONFT_GUILD:
        strTemp = sc::string::format(ID2GAMEINTEXT("CONFLICT_CLUB_QUESTION_TAR"), szReqName).c_str();
        MODAL_CONFT_TYPE = UI::MODAL_CONFLICT_GUILD_TAR;
        break;
    };

    CString strMyName = m_pGaeaClient->GetCharacter ()->GetName ();
    DoConftModal(strTemp.c_str(), MODAL_CONFT_TYPE, szReqName, strMyName);
}

void CInnerInterface::MsgServerGeneralChat( NET_MSG_GENERIC* nmg )
{
    // NET_MSG_SERVER_GENERALCHAT
    // GLMSG::SNET_SERVER_GENERALCHAT* pNetMsg = (GLMSG::SNET_SERVER_GENERALCHAT*) nmg;
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) nmg;
    GLMSG::SNET_SERVER_GENERALCHAT ChatData;
    if (msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), ChatData))
        DisplayChatMessage(CHAT_TYPE_CTRL_GLOBAL, "", ChatData.m_ChatMsg.c_str());
}

void CInnerInterface::ModalInventoryItemDecomposeOkQuestion(DWORD dwMsg)
{
	if( UIMSG_MODAL_OK & dwMsg )
	{
		WORD wPosX, wPosY;
		m_pInventory->GetItemDecomposePos(wPosX, wPosY);
		m_pGaeaClient->GetCharacter()->ReqInvenItemDecompose(wPosX, wPosY);
	}
}

void CInnerInterface::ModalCostumeStatApply(DWORD dwMsg)
{
	if( UIMSG_MODAL_OK & dwMsg )
	{
		m_pGaeaClient->GetCharacter()->ReqCostumeStatCToS();
	}
}

void CInnerInterface::ModalCostumeStatBasic(DWORD dwMsg)
{
	if( UIMSG_MODAL_OK & dwMsg )
	{
		CCostumeStatClient* pCS = m_pGaeaClient->GetCostumeClient();

		if( pCS ) OpenCostumeStatWindow( pCS->GetSrcItem(), pCS->GetDstItem() );
	}
}

void CInnerInterface::ModalCostuemStatApplied(DWORD dwMsg)
{
	if( UIMSG_MODAL_OK & dwMsg )
	{
		CCostumeStatClient* pCS = m_pGaeaClient->GetCostumeClient();

		if( pCS ) OpenCostumeStatWindow( pCS->GetSrcItem(), pCS->GetDstItem() );
	}
}

void CInnerInterface::ModalConfirmChangeCountry ( DWORD dwMsg )
{
	GLCharacter* pChar = m_pGaeaClient->GetCharacter();
	if ( NULL == pChar )
		return;

	if ( UIMSG_MODAL_OK & dwMsg )
	{
		GLMSG::NET_USE_ITEM_INITIALIZE_COUNTRY_CF sNetMsg;
		sNetMsg.m_wPosX = pChar->m_wInvenPosX1;
		sNetMsg.m_wPosY = pChar->m_wInvenPosY1;

		m_pGaeaClient->NETSENDTOFIELD( &sNetMsg );
	}
	else if ( UIMSG_MODAL_CANCEL & dwMsg )
	{
		pChar->m_wInvenPosX1 = 0;
		pChar->m_wInvenPosY1 = 0;
	}
}

void CInnerInterface::ModalConfirmJoinPVPServer ( DWORD dwMsg )
{
	if ( UIMSG_MODAL_OK & dwMsg )
	{
		m_pGaeaClient->JoinWorldBattleServer();
	}
}

void CInnerInterface::ModalCostumeStatRelease(DWORD dwMsg)
{
	GLCharacter* pChar = m_pGaeaClient->GetCharacter();
	if ( NULL == pChar )
		return;

	if( UIMSG_MODAL_OK & dwMsg )
	{		
		GLMSG::SNET_INVEN_COSTUME_RELEASE NetMsg;

		NetMsg.wPosX	 = pChar->m_wInvenPosX1;
		NetMsg.wPosY	 = pChar->m_wInvenPosY1;
		NetMsg.wHoldPosX = pChar->m_wInvenPosX2;
		NetMsg.wHoldPosY = pChar->m_wInvenPosY2;

		NetMsg.Crc32 = sc::string::getCrc32(&NetMsg, NetMsg.Size()-sizeof(unsigned int));

		m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
	}
	else
	if( UIMSG_MODAL_CANCEL & dwMsg )
	{
		pChar->m_wInvenPosX1 = 0;
		pChar->m_wInvenPosY1 = 0;
		pChar->m_wInvenPosX2 = 0;
		pChar->m_wInvenPosY2 = 0;
	}
}

void CInnerInterface::ModalTempEnchant(DWORD dwMsg)
{
	GLCharacter* pChar = m_pGaeaClient->GetCharacter();
	if ( NULL == pChar )
		return;

	if( UIMSG_MODAL_OK & dwMsg )
	{		
		GLMSG::SNET_INVEN_TL_GRINDING NetMsg;

		NetMsg.wPosX	 = pChar->m_wInvenPosX1;
		NetMsg.wPosY	 = pChar->m_wInvenPosY1;
		NetMsg.wHoldPosX = pChar->m_wInvenPosX2;
		NetMsg.wHoldPosY = pChar->m_wInvenPosY2;

		NetMsg.eResult = EMREQ_TEMP_ENCHANT_EXIST;

		m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
	}
	else
	if( UIMSG_MODAL_CANCEL & dwMsg )
	{
		pChar->m_wInvenPosX1 = 0;
		pChar->m_wInvenPosY1 = 0;
		pChar->m_wInvenPosX2 = 0;
		pChar->m_wInvenPosY2 = 0;
	}
}

void CInnerInterface::ModalSelectiveformBoxSelect(DWORD dwMsg)
{
	//	시작
	if ( dwMsg & UIMSG_MODAL_OK )
	{
		INT nPos(-1);
		m_pSelformBoxWindow->GetSelectPos(nPos);
		m_pGaeaClient->GetCharacter()->ReqSelformBoxSelect(nPos);
	}
	else if ( dwMsg & UIMSG_MODAL_CANCEL )
	{
	}
}