#include "pch.h"
#include "./GLCharClient.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void GLCharClient::MsgProcess(NET_MSG_GENERIC* pMsg)
{
    switch (pMsg->Type())
    {
    case NET_MSG_GCTRL_MOVESTATE_BRD:
        MsgMoveState(pMsg);
        break;

    case NET_MSG_GCTRL_GOTO_BRD:
        MsgGoto(pMsg);
        break;

    case NET_MSG_GCTRL_JUMP_POS_BRD:
        MsgJumpPosBrd(pMsg);
        break;

	case NET_MSG_GCTRL_ANIMOVE_START_BRD:
		MsgAnimoveStartBrd(pMsg);
		break;

    case NET_MSG_GCTRL_ATTACK_BRD:
        MsgAttackBrd(pMsg);
        break;

    case NET_MSG_GCTRL_ATTACK_CANCEL_BRD:
        MsgAttackCancelBrd(pMsg);
        break;

    case NET_MSG_GCTRL_ATTACK_BLOCKING_BRD:
        MsgAttackBlockingBrd(pMsg);
        break;

    case NET_MSG_GCTRL_ATTACK_AVOID_BRD:
        MsgAttackAvoidBrd(pMsg);
        break;

    case NET_MSG_GCTRL_ATTACK_DAMAGE_BRD:
        MsgAttackDamageBrd(pMsg);
        break;

    case NET_MSG_GCTRL_ACTION_BRD:
        MsgActionBrd(pMsg);
        break;

    case NET_MSG_GCTRL_PUTON_RELEASE_BRD:
        MsgPutonReleaseBrd(pMsg);
        break;

    case NET_MSG_GCTRL_PUTON_UPDATE_BRD:
        MsgPutonUpdateBrd(pMsg);
        break;

    case NET_MSG_GCTRL_PUTON_CHANGE_BRD:
        MsgPutonChangeBrd(pMsg);
        break;

    case NET_MSG_GCTRL_REQ_LEVELUP_BRD:
        MsgLevelUpBrd(pMsg);
        break;

    case NET_MSG_GCTRL_REQ_SKILL_BRD:
        MsgReqSkillBrd(pMsg);
        break;

	case NET_MSG_GCTRL_REQ_SKILL_DEALY_ACTION_BRD:
		MsgDealyActionSkillBrd(pMsg);
		break;

	case NET_MSG_GCTRL_END_SKILL_BRD:
		MsgEndSkillBrd(pMsg);
		break;

    case NET_MSG_GCTRL_SKILL_CANCEL_BRD:
        MsgSkillCancelBrd(pMsg);
        break;

    case NET_MSG_GCTRL_SKILLFACT_BRD:
        MsgSkillFactBrd(pMsg);
        break;

    case NET_MSG_GCTRL_SKILLHOLD_BRD:
        MsgSkillHoldBrd(pMsg);
        break;

    case NET_MSG_GCTRL_SAVESKILLFACT_BRD:
        MsgSaveSkillFactBrd(pMsg);
        break;

    case NET_MSG_GCTRL_SKILLHOLD_RS_BRD:
        MsgSkillHOldRsBrd(pMsg);
        break;

    case NET_MSG_GCTRL_STATEBLOW_BRD:
        MsgTateBlowBrd(pMsg);
        break;

    case NET_MSG_GCTRL_CURESTATEBLOW_BRD:
        MsgCureStateBlowBrd(pMsg);
        break;

    case NET_MSG_GCTRL_PUSHPULL_BRD:
        MsgPushPullBrd(pMsg);
        break;

    case NET_MSG_GCTRL_LANDEFFECT_UPDATE_BRD:
        MsgLandEffectUpdateBrd(pMsg);
        break;

    case NET_MSG_GCTRL_MAPEFFECT_UPDATE_BRD:
        MsgMapEffectUpdateBrd(pMsg);
        break;

    case NET_MSG_GCTRL_CONFRONT_END2_CLT_BRD:
        MsgConfrontEndToCltBrd(pMsg);
        break;

    case NET_MSG_GCTRL_UPDATE_BRIGHT_BRD:
        MsgUpdateBrightBrd(pMsg);
        break;

    case NET_MSG_GCTRL_UPDATE_STATE_BRD:
        MsgUpdateStateBrd(pMsg);
        break;

    case NET_MSG_GCTRL_UPDATE_FLAGS:
        MsgUpdateFlags(pMsg);
        break;

    case NET_MSG_GCTRL_PARTY_BRD:
        MsgPartyBrd(pMsg);
        break;

    case NET_MSG_GCTRL_UPDATE_PASSIVE_BRD:
        MsgUpdatePassiveBrd(pMsg);
        break;

    case NET_MSG_GCTRL_PMARKET_OPEN_BRD:
        MsgPrivateMarketOpenBrd(pMsg);
        break;

    case NET_MSG_GCTRL_PMARKET_CLOSE_BRD:
        MsgPrivateMarketCloseBrd(pMsg);
        break;

    case NET_MSG_GCTRL_PMARKET_ITEM_UPDATE_BRD:
        MsgPrivateMarketItemUpdateBrd(pMsg);
        break;

    case NET_MSG_GCTRL_PMARKET_ITEM_INFO_BRD:
        MsgPrivateMarketItemInfoBrd(pMsg);
        break;

    case NET_MSG_GCTRL_CLUB_INFO_BRD:
        MsgClubInfoBrd(pMsg);
        break;

    case NET_MSG_GCTRL_CLUB_INFO_MARK_BRD:
        MsgClubInfoMarkBrd(pMsg);
        break;

    case NET_MSG_GCTRL_CLUB_INFO_NICK_BRD:
        MsgClubInfoNickBrd(pMsg);
        break;

    case NET_MSG_GCTRL_CLUB_DEL_BRD:
        MsgClubDelBrd(pMsg);
        break;

    case NET_MSG_GCTRL_CLUB_AUTHORITY_BRD:
        MsgClubAuthorityBrd(pMsg);
        break;

    case NET_MSG_GCTRL_INVEN_HAIR_CHANGE_BRD:
        MsgInvenHairChangeBrd(pMsg);
        break;

    case NET_MSG_GCTRL_INVEN_HAIRCOLOR_CHANGE_BRD:
        MsgInvenHairColorChangeBrd(pMsg);
        break;

    case NET_MSG_GCTRL_COSTUM_COLOR_CHANGE_BRD:
        MsgCostumeColorChangeBrd(pMsg);
        break;

    case NET_MSG_GCTRL_VEHICLE_COLOR_CHANGE_BRD:
        MsgVehicleColorChangeBrd(pMsg);
        break;

    case NET_MSG_GCTRL_LEARN_BIKE_BOOST_BRD:
        MsgLearnBikeBoostBrd(pMsg);
        break;

    case NET_MSG_GCTRL_USE_BIKE_BOOST_BRD:
        MsgUseBikeBoostBrd(pMsg);
        break;

    case NET_MSG_GCTRL_USE_BIKE_BOOST_CANCEL_BRD:
        MsgUseBikeBoostCancelBrd(pMsg);
        break;

    case NET_MSG_GCTRL_INVEN_FACE_CHANGE_BRD:
        MsgInvenFaceChangeBrd(pMsg);
        break;

    case NET_MSG_GCTRL_INVEN_RENAME_BRD:
        MsgInvenRenameBrd(pMsg);
        break;

    case NET_MSG_GCTRL_REQ_GESTURE_BRD:
        MsgReqGestureBrd(pMsg);
        break;

    case NET_MSG_GCTRL_QITEMFACT_BRD:
        MsgQuestionItemFactBrd(pMsg);
        break;

    case NET_MSG_GCTRL_QITEMFACT_END_BRD:
        MsgQuestionItemFactEndBrd(pMsg);
        break;

    case NET_MSG_GCTRL_EVENTFACT_BRD:
        MsgEventFactBrd(pMsg);
        break;

    case NET_MSG_GCTRL_EVENTFACT_END_BRD:
        MsgEventFactEndBrd(pMsg);
        break;

    case NET_MSG_GCTRL_IPEVENT_BRD:
        MsgIPEventBrd(pMsg);
        break;

    case NET_MSG_GCTRL_ACTIVE_VEHICLE_BRD:
        MsgActiveVehicleBrd(pMsg);
        break;

    case NET_MSG_GCTRL_ACTIVE_WITH_VEHICLE_BRD:
        MsgActiveWithVehicleBrd(pMsg);
        break;

    case NET_MSG_GCTRL_GET_VEHICLE_BRD:
        MsgGetVehicleBrd(pMsg);
        break;

    case NET_MSG_GCTRL_UNGET_VEHICLE_BRD:
        MsgUngetVehicleBrd(pMsg);
        break;

    case NET_MSG_VEHICLE_ACCESSORY_DELETE_BRD:
        MsgVehicleAccessoryDeleteBrd(pMsg);
        break;

    case NET_MSG_VEHICLE_REQ_SLOT_EX_HOLD_BRD:
        MsgVehicleReqSlotExHoldBrd(pMsg);
        break;

    case NET_MSG_VEHICLE_REMOVE_SLOTITEM_BRD:
        MsgVehicleRemoveSlotItemBrd(pMsg);
        break;

    case NET_MSG_GCTRL_VEHICLE_BRD:
        MsgVehicleBrd(pMsg);
        break;

    case NET_MSG_GCTRL_ITEMSHOPOPEN_BRD:
        MsgItemShopOpenBrd(pMsg);
        break;

    case NET_MSG_REQ_GATHERING_BRD:
        MsgReqGatheringBrd(pMsg);
        break;

    case NET_MSG_REQ_GATHERING_CANCEL_BRD:
        MsgReqGatheringCancelBrd(pMsg);
        break;

    case NET_MSG_REQ_GATHERING_RESULT_BRD:
        MsgGatheringResultBrd(pMsg);
        break;

    case NET_MSG_GCTRL_UPDATE_CDM_SAFETIME_BRD:
        MsgUpdateCdmSafetimeBrd(pMsg);
        break;

    case NET_MSG_ACTIVITY_POPUP_COMPLETE:
        MsgActivityPopupComplate(pMsg);
        break;

    case NET_MSG_TITLE_NOTIFY_CHANGE:
        MsgTitleNotifyChange(pMsg);
        break;
	case NET_MSG_DRUG_FB :
		MsgDrugFb(pMsg);
		break;

	case NET_MSG_GCTRL_CLUB_RENAME_BRD:
		MsgClubRenameBrd( pMsg );
		break;

    default:
        break;
    };
}
