#include "../pch.h"
#include "../../EngineLib/DxTools/DebugSet.h"
#include "../GLGaeaClient.h"
#include "./GLCharacter.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void GLCharacter::InitPacketFunc()
{
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_ATTACK_BLOCKING].Msgfunc = &GLCharacter::MsgAttackBlocking;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_ATTACK_AVOID].Msgfunc = &GLCharacter::MsgAttackAvoid;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_ATTACK_DAMAGE].Msgfunc = &GLCharacter::MsgAttackDamage;
	m_GLCharacterMsgFunc[NET_MSG_GCTRL_ATTACK_HEAL].Msgfunc = &GLCharacter::MsgAttackHeal;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_DEFENSE_SKILL_ACTIVE].Msgfunc = &GLCharacter::MsgDefenseSkillActive;
	m_GLCharacterMsgFunc[NET_MSG_GCTRL_BEGIN_SKILL_FC].Msgfunc = &GLCharacter::MsgBeginSkill;
	m_GLCharacterMsgFunc[NET_MSG_GCTRL_END_SKILL_FC].Msgfunc = &GLCharacter::MsgEndSkill;
	m_GLCharacterMsgFunc[NET_MSG_GCTRL_END_SKILL_BRD].Msgfunc = &GLCharacter::MsgEndSkillBrd;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_ACTION_BRD].Msgfunc = &GLCharacter::MsgActionBrd;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_UPDATE_STATE].Msgfunc = &GLCharacter::MsgUpdateState;
	m_GLCharacterMsgFunc[NET_MSG_GCTRL_UPDATE_HIRE_SUMMON_STATE].Msgfunc = &GLCharacter::MsgUpdateHireSummonState;
	m_GLCharacterMsgFunc[NET_MSG_UPDATE_CHARSLOT_FC].Msgfunc = &GLCharacter::MsgUpdateCharacterSlotState;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_UPDATE_EXP].Msgfunc = &GLCharacter::MsgUpdateExp;
    m_GLCharacterMsgFunc[NET_MSG_CHAR_UPDATE_MONEY_FC].Msgfunc = &GLCharacter::MsgUpdateMoney;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_UPDATE_SP].Msgfunc = &GLCharacter::MsgUpdateSp;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_UPDATE_CP].Msgfunc = &GLCharacter::MsgUpdateCp;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_UPDATE_LP].Msgfunc = &GLCharacter::MsgUpdateLp;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_UPDATE_SKP].Msgfunc = &GLCharacter::MsgUpdateSkillPoint;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_UPDATE_BRIGHT].Msgfunc = &GLCharacter::MsgUpdateBright;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_UPDATE_STATS].Msgfunc = &GLCharacter::MsgUpdateStats;

    m_GLCharacterMsgFunc[NET_MSG_GCTRL_PICKUP_MONEY].Msgfunc = &GLCharacter::MsgPickUpMoney;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_PICKUP_ITEM].Msgfunc = &GLCharacter::MsgUpdatePickupItem;

    m_GLCharacterMsgFunc[NET_MSG_GCTRL_REQ_HOLD_FB].Msgfunc = &GLCharacter::MsgReqHoldFb;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_REQ_ITEM_SORT].Msgfunc = &GLCharacter::MsgReqItemSort;
	m_GLCharacterMsgFunc[NET_MSG_GCTRL_REQ_ITEM_SORT_START].Msgfunc = &GLCharacter::MsgReqItemSortStart;
	m_GLCharacterMsgFunc[NET_MSG_GCTRL_REQ_ITEM_SORT_DATA].Msgfunc = &GLCharacter::MsgReqItemSortData;
	m_GLCharacterMsgFunc[NET_MSG_GCTRL_REQ_ITEM_SORT_END].Msgfunc = &GLCharacter::MsgReqItemSortEnd;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_ITEM_SORT_FAIL].Msgfunc = &GLCharacter::MsgItemSortFail;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_INVEN_INSERT].Msgfunc = &GLCharacter::MsgInvenInsert;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_REQ_VNINVEN_TO_INVEN_FB].Msgfunc = &GLCharacter::MsgReqVNInvenToInvenFb;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_INVEN_DELETE].Msgfunc = &GLCharacter::MsgInvenDelete;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_ITEM_COOLTIME_UPDATE].Msgfunc = &GLCharacter::MsgItemCoolTimeUpdate;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_ITEM_COOLTIME_ERROR].Msgfunc = &GLCharacter::MsgItemCoolTimeError;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_INVEN_DEL_INSERT].Msgfunc = &GLCharacter::MsgInvenDelInsert;
    
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_PUTON_RELEASE].Msgfunc = &GLCharacter::MsgPutonRelease;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_PUTON_UPDATE].Msgfunc = &GLCharacter::MsgPutonUpdate;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_PUTON_CHANGE].Msgfunc = &GLCharacter::MsgPutonChange;
    
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_REQ_LEVELUP_FB].Msgfunc = &GLCharacter::MsgReqLevelUpFb;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_REQ_STATSUP_FB].Msgfunc = &GLCharacter::MsgReqStatsUpFb;
	m_GLCharacterMsgFunc[NET_MSG_GCTRL_REQ_STATSUP_EX_FB].Msgfunc = &GLCharacter::MsgReqStatsUpExFb;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_REQ_LEARNSKILL_FB].Msgfunc = &GLCharacter::MsgReqLearnSkillFb;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_REQ_SKILLUP_FB].Msgfunc = &GLCharacter::MsgReqSkillUpFb;
	m_GLCharacterMsgFunc[NET_MSG_GCTRL_REQ_RNSKILLUP_COMPLETE].Msgfunc = &GLCharacter::MsgReqRnSkillUpComplete;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_REQ_SKILL_FB].Msgfunc = &GLCharacter::MsgReqSkillFb;
	m_GLCharacterMsgFunc[NET_MSG_GCTRL_REQ_SKILL_DEALY_ACTION].Msgfunc = &GLCharacter::MsgSkillDealyAction;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_REQ_SKILLHOLD_RS_FB].Msgfunc = &GLCharacter::MsgReqSkillHoldRsFb;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_REQ_INVEN_LUNCHBOX_FB].Msgfunc = &GLCharacter::MsgReqInvenLunchBoxFb;
    
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_SELECT_TARGET].Msgfunc = &GLCharacter::MsgSelectTarget;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_SKILLCONSUME_FB].Msgfunc = &GLCharacter::MsgSkillConsumeFb;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_SKILLFACT_BRD].Msgfunc = &GLCharacter::MsgSkillFactBrd;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_SKILLHOLD_BRD].Msgfunc = &GLCharacter::MsgSkillHoldBrd;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_SKILLHOLD_RS_BRD].Msgfunc = &GLCharacter::MsgSkillHoldRsBrd;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_SAVESKILLFACT_BRD].Msgfunc = &GLCharacter::MsgSaveSkillFactBrd;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_SKILL_CANCEL_BRD].Msgfunc = &GLCharacter::MsgSkillCancelBrd;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_STATEBLOW_BRD].Msgfunc = &GLCharacter::MsgStateBlowBrd;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_CURESTATEBLOW_BRD].Msgfunc = &GLCharacter::MsgCureStateBlowBrd;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_INVEN_DRUG_UPDATE].Msgfunc = &GLCharacter::MsgInvenDrugUpdate;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_PUTON_DRUG_UPDATE].Msgfunc = &GLCharacter::MsgPutonDrugUpdate;

	m_GLCharacterMsgFunc[NET_MSG_GCTRL_REQ_GETSTORAGE_SPECIFIC_ITEM_COUNT_FB].Msgfunc = &GLCharacter::MsgStorageSpecificItemCount;
	m_GLCharacterMsgFunc[NET_MSG_GCTRL_REQ_GETSTORAGE_SPECIFIC_ITEM_FB].Msgfunc = &GLCharacter::MsgStorageSpecificItem;

    m_GLCharacterMsgFunc[NET_MSG_GCTRL_REQ_GETSTORAGE_FB].Msgfunc = &GLCharacter::MsgReqGetStorageFb;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_REQ_GETSTORAGE_ITEM].Msgfunc = &GLCharacter::MsgReqGetStorageItem;
    
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_STORAGE_INSERT].Msgfunc = &GLCharacter::MsgStorageInsert;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_STORAGE_DELETE].Msgfunc = &GLCharacter::MsgStorageDelete;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_STORAGE_ITEM_UPDATE].Msgfunc = &GLCharacter::MsgStorageItemUpdate;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_STORAGE_DEL_INSERT].Msgfunc = &GLCharacter::MsgStorageDelInsert;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_STORAGE_DRUG_UPDATE].Msgfunc = &GLCharacter::MsgStorageDrugUpdate;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_STORAGE_UPDATE_MONEY].Msgfunc = &GLCharacter::MsgStorageUpdateMoney;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_REQ_SKILLQ_FB].Msgfunc = &GLCharacter::MsgReqSkillQFb;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_REQ_ACTIONQ_FB].Msgfunc = &GLCharacter::MsgReqActionQFb;
    
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_INVEN_ITEM_UPDATE].Msgfunc = &GLCharacter::MsgInvenItemUpdate;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_INVEN_GRINDING_FB].Msgfunc = &GLCharacter::MsgInvenGrindingFb;
	m_GLCharacterMsgFunc[NET_MSG_GCTRL_INVEN_PERIODEXTEND_FB].Msgfunc = &GLCharacter::MsgInvenPeriodExtensionFb;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_INVEN_BOXOPEN_FB].Msgfunc = &GLCharacter::MsgInvenBoxOpenFb;	
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_INVEN_RANDOMBOXOPEN_FB].Msgfunc = &GLCharacter::MsgInvenRandomBoxOpenFb;
	m_GLCharacterMsgFunc[NET_MSG_GCTRL_INVEN_LOCKBOX_FB].Msgfunc = &GLCharacter::MsgInvenLockBoxFB;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_INVEN_DISJUNCTION_FB].Msgfunc = &GLCharacter::MsgInvenDisjunctionFb;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_INVEN_DISGUISE_FB].Msgfunc = &GLCharacter::MsgInvenDisguiseFb;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_INVEN_CLEANSER_FB].Msgfunc = &GLCharacter::MsgInvenCleanserFb;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_INVEN_DEL_ITEM_TIMELMT].Msgfunc = &GLCharacter::MsgInvenDelItemTimeLimit;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_INVEN_BOX_WRAPPING_FB].Msgfunc = &GLCharacter::MsgInvenBoxWrappingFb;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_INVEN_BOX_UNWRAPPING_FB].Msgfunc = &GLCharacter::MsgInvenBoxUnwrappingFb;
    
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_PUSHPULL_BRD].Msgfunc = &GLCharacter::MsgPushPullBrd;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_REGEN_GATE_FB].Msgfunc = &GLCharacter::MsgRegenGateFb;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_CHARRESET_FB].Msgfunc = &GLCharacter::MsgCharResetFb;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_FIRECRACKER_FB].Msgfunc = &GLCharacter::MsgFireCrackerFb;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_FIRECRACKER_BRD].Msgfunc = &GLCharacter::MsgFireCrackerBrd;
    
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_INVEN_VIETNAM_ITEMGETNUM_UPDATE].Msgfunc = &GLCharacter::MsgInvenVietnamItemGetNumUpdate;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_INVEN_VIETNAM_ITEMGET_FB].Msgfunc = &GLCharacter::MsgInvenVietnamItemGetFb;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_INVEN_VIETNAM_EXPGET_FB].Msgfunc = &GLCharacter::MsgInvenVietnamExpGetFb;
    
    m_GLCharacterMsgFunc[NET_MSG_VIETNAM_ALLINITTIME].Msgfunc = &GLCharacter::MsgVietnamAllInitTime;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_INVEN_RESET_SKST_FB].Msgfunc = &GLCharacter::MsgInvenResetSkstFb;
    m_GLCharacterMsgFunc[NET_MSG_GM_MOVE2GATE_FB].Msgfunc = &GLCharacter::MsgGmMoveToGateFb;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_CURE_FB].Msgfunc = &GLCharacter::MsgCureFb;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_INVEN_CHARCARD_FB].Msgfunc = &GLCharacter::MsgInvenCharCardFb;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_INVEN_STORAGECARD_FB].Msgfunc = &GLCharacter::MsgInvenStorageCardFb;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_STORAGE_STATE].Msgfunc = &GLCharacter::MsgStorageState;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_INVEN_INVENLINE_FB].Msgfunc = &GLCharacter::MsgInvenInvenLineFb;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_INVEN_REMODELOPEN_FB].Msgfunc = &GLCharacter::MsgInvenReModelOpenFb;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_INVEN_GARBAGEOPEN_FB].Msgfunc = &GLCharacter::MsgInvenGarbageOpenFb;
	m_GLCharacterMsgFunc[NET_MSG_GCTRL_DOMINATE_MOVETO_CF_FB].Msgfunc = &GLCharacter::MsgDominateMoveToFb;
	m_GLCharacterMsgFunc[NET_MSG_GCTRL_DOMINATE_MOVETO_FC].Msgfunc = &GLCharacter::MsgDominateMoveToFc;
	m_GLCharacterMsgFunc[NET_MSG_GCTRL_GOTO].Msgfunc = &GLCharacter::MsgGoto;

        // 락커 관리인과의 대화를 제외한 그밖의 방법으로 창고 열람 ( 긴급 창고 연결 카드... etc );
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_INVEN_STORAGEOPEN_FB].Msgfunc = &GLCharacter::MsgInvenStorageOpenFb;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_INVEN_PREMIUMSET_FB].Msgfunc = &GLCharacter::MsgInvenPremiumSetFb;
    m_GLCharacterMsgFunc[NET_MSG_CHAT_LOUDSPEAKER_FB].Msgfunc = &GLCharacter::MsgChatLoudSpeakerFb;
    
    m_GLCharacterMsgFunc[NET_MSG_CHAT_AREA_FB].Msgfunc = &GLCharacter::MsgChatAreaFb;
    m_GLCharacterMsgFunc[NET_MSG_CHAT_PARTY_RECRUIT_FB].Msgfunc = &GLCharacter::MsgChatPartyRecruitFb;
	m_GLCharacterMsgFunc[NET_MSG_CHAT_FACTION_FB].Msgfunc = &GLCharacter::MsgChatFactionFb;
    
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_PREMIUM_STATE].Msgfunc = &GLCharacter::MsgPremiumState;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_NPC_ITEM_TRADE_FB].Msgfunc = &GLCharacter::MsgNpcItemTradeFb;
    m_GLCharacterMsgFunc[NET_MSG_GET_CHARGED_ITEM_FC].Msgfunc = &GLCharacter::MsgGetCharedItemFromDbFb;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_CHARGED_ITEM_GET_FB].Msgfunc = &GLCharacter::MsgChargedItemGetFb;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_CHARGED_ITEM_DEL].Msgfunc = &GLCharacter::MsgChargedItemDel;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_GETEXP_RECOVERY_NPC_FB].Msgfunc = &GLCharacter::MsgGetExpRecoveryNpcFb;

    m_GLCharacterMsgFunc[NET_MSG_GCTRL_RECOVERY_NPC_FB].Msgfunc = &GLCharacter::MsgRecoveryNpcFb;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_PMARKET_TITLE_FB].Msgfunc = &GLCharacter::MsgPMarketTitleFb;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_PMARKET_REGITEM_FB].Msgfunc = &GLCharacter::MsgPMarketRegItemFb;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_PMARKET_DISITEM_FB].Msgfunc = &GLCharacter::MsgPMarketDisItemFb;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_PMARKET_OPEN_FB].Msgfunc = &GLCharacter::MsgPMarketOpenFb;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_PMARKET_BUY_FB].Msgfunc = &GLCharacter::MsgPMarketBuyFb;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_PMARKET_ITEM_UPDATE_BRD].Msgfunc = &GLCharacter::MsgPMarketItemUpdateBrd;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_PMARKET_CLOSE_BRD].Msgfunc = &GLCharacter::MsgPMarketCloseBrd;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_PMARKET_SEARCH_ITEM_RESULT].Msgfunc = &GLCharacter::MsgPMarketSearchItemResult;
    
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_PLAYERKILLING_ADD].Msgfunc = &GLCharacter::MsgPlayerKillingAdd;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_PLAYERKILLING_DEL].Msgfunc = &GLCharacter::MsgPlayerKillingDel;
   
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_INVEN_HAIR_CHANGE_FB].Msgfunc = &GLCharacter::MsgInvenHairChangeFb;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_INVEN_HAIRCOLOR_CHANGE_FB].Msgfunc = &GLCharacter::MsgInvenHairColorChangeFb;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_COSTUM_COLOR_CHANGE_FB].Msgfunc = &GLCharacter::MsgCostumeColorChangeFb;
    
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_VEHICLE_COLOR_CHANGE_FB].Msgfunc = &GLCharacter::MsgVehicleColorChangeFb;		
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_LEARN_BIKE_BOOST_FB].Msgfunc = &GLCharacter::MsgLearnBikeBoostFb;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_USE_BIKE_BOOST_FB].Msgfunc = &GLCharacter::MsgUseBikeBoostFb;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_USE_BIKE_BOOST_CANCEL_FB].Msgfunc = &GLCharacter::MsgUseBikeBoostCancelFb;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_INVEN_FACE_CHANGE_FB].Msgfunc = &GLCharacter::MsgInvenFaceChangeFb;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_INVEN_GENDER_CHANGE_FB].Msgfunc = &GLCharacter::MsgInvenGenderChangeFb;
	m_GLCharacterMsgFunc[NET_MSG_GCTRL_INVEN_SCHOOL_CHANGE_FB].Msgfunc = &GLCharacter::MsgInvenSchoolChangeFb;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_INVEN_RENAME_FB].Msgfunc = &GLCharacter::MsgInvenRenameFb;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_POSTBOX_OPEN_FB].Msgfunc = &GLCharacter::MsgPostBoxOpenFb;	
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_INVEN_POSTBOX_OPEN_CARD_FB].Msgfunc = &GLCharacter::MsgInvenPostBoxOpenCardFb;
    
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_QITEMFACT_BRD].Msgfunc = &GLCharacter::MsgQuestionItemFactBrd;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_QITEMFACT_END_BRD].Msgfunc = &GLCharacter::MsgQuestionItemFactEndBrd;
    
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_EVENTFACT_BRD].Msgfunc = &GLCharacter::MsgEventFactBrd;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_EVENTFACT_END_BRD].Msgfunc = &GLCharacter::MsgEventFactEndBrd;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_EVENTFACT_INFO].Msgfunc = &GLCharacter::MsgEventFactInfo;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_IPEVENT_BRD].Msgfunc = &GLCharacter::MsgIPEventBrd;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_2_FRIEND_FB].Msgfunc = &GLCharacter::MsgFriendToFb;
    m_GLCharacterMsgFunc[NET_MSG_REBUILD_RESULT].Msgfunc = &GLCharacter::MsgRebuildResult;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_GARBAGE_RESULT_FB].Msgfunc = &GLCharacter::MsgGarbageResultFb;
    
    m_GLCharacterMsgFunc[NET_MSG_REBUILD_MOVE_ITEM].Msgfunc = &GLCharacter::MsgRebuildMoveItem;
    m_GLCharacterMsgFunc[NET_MSG_REBUILD_COST_MONEY].Msgfunc = &GLCharacter::MsgReBuildCostMoeny;
    m_GLCharacterMsgFunc[NET_MSG_REBUILD_INPUT_MONEY].Msgfunc = &GLCharacter::MsgReBuildInputMoney;
    
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_UPDATE_LASTCALL].Msgfunc = &GLCharacter::MsgUpdateLastCall;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_UPDATE_STARTCALL].Msgfunc = &GLCharacter::MsgUpdateStartCall;
    m_GLCharacterMsgFunc[NET_MSG_SMS_SEND_FB].Msgfunc = &GLCharacter::MsgSmsSendFb;
    //m_GLCharacterMsgFunc[NET_MSG_SMS_PHONE_NUMBER_FB].Msgfunc = &GLCharacter::MsgSmsPhoneNumberFb;
	m_GLCharacterMsgFunc[NET_MSG_GCTRL_INVEN_ENTER_NUM_CHARGE_FC].Msgfunc = &GLCharacter::MsgReqEnterNumChargeFb;

    m_GLCharacterMsgFunc[NET_MSG_GM_SHOWMETHEMONEY].Msgfunc = &GLCharacter::MsgGmShowMeTheMoney;
    
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_TRADE_FB].Msgfunc = &GLCharacter::MsgTradeProcess;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_TRADE_LOCK_TAR].Msgfunc = &GLCharacter::MsgTradeProcess;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_TRADE_AGREE_TAR].Msgfunc = &GLCharacter::MsgTradeProcess;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_TRADE_ITEM_REGIST_TAR].Msgfunc = &GLCharacter::MsgTradeProcess;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_TRADE_ITEM_REMOVE_TAR].Msgfunc = &GLCharacter::MsgTradeProcess;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_TRADE_MONEY_TAR].Msgfunc = &GLCharacter::MsgTradeProcess;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_TRADE_COMPLETE_TAR].Msgfunc = &GLCharacter::MsgTradeProcess;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_TRADE_CANCEL_TAR].Msgfunc = &GLCharacter::MsgTradeProcess;
    
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_CONFRONT_FB].Msgfunc = &GLCharacter::MsgProcessConfront;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_CONFRONT_START2_CLT].Msgfunc = &GLCharacter::MsgProcessConfront;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_CONFRONT_FIGHT2_CLT].Msgfunc = &GLCharacter::MsgProcessConfront;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_CONFRONT_END2_CLT].Msgfunc = &GLCharacter::MsgProcessConfront;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_CONFRONTPTY_START2_CLT].Msgfunc = &GLCharacter::MsgProcessConfront;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_CONFRONTPTY_END2_CLT].Msgfunc = &GLCharacter::MsgProcessConfront;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_CONFRONT_RECOVE].Msgfunc = &GLCharacter::MsgProcessConfront;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_CONFRONT_END2_CLT_MBR].Msgfunc = &GLCharacter::MsgProcessConfront;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_CONFRONTCLB_START2_CLT].Msgfunc = &GLCharacter::MsgProcessConfront;

    m_GLCharacterMsgFunc[NET_MSG_GCTRL_REQ_QUEST_START_FB].Msgfunc = &GLCharacter::MsgProcessQuest;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_QUEST_PROG_STREAM].Msgfunc = &GLCharacter::MsgProcessQuest;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_QUEST_PROG_STEP_STREAM].Msgfunc = &GLCharacter::MsgProcessQuest;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_QUEST_PROG_INVEN].Msgfunc = &GLCharacter::MsgProcessQuest;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_QUEST_PROG_DEL].Msgfunc = &GLCharacter::MsgProcessQuest;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_QUEST_END_STREAM].Msgfunc = &GLCharacter::MsgProcessQuest;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_QUEST_END_DEL].Msgfunc = &GLCharacter::MsgProcessQuest;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_QUEST_PROG_NPCTALK_FB].Msgfunc = &GLCharacter::MsgProcessQuest;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_QUEST_PROG_MOBKILL].Msgfunc = &GLCharacter::MsgProcessQuest;
	m_GLCharacterMsgFunc[NET_MSG_GCTRL_QUEST_PROG_ITEMUSE].Msgfunc = &GLCharacter::MsgProcessQuest;
	m_GLCharacterMsgFunc[NET_MSG_GCTRL_QUEST_PROG_ITEMGRINDING].Msgfunc = &GLCharacter::MsgProcessQuest;
	m_GLCharacterMsgFunc[NET_MSG_GCTRL_QUEST_PROG_CONFTING].Msgfunc = &GLCharacter::MsgProcessQuest;
	m_GLCharacterMsgFunc[NET_MSG_GCTRL_QUEST_PROG_LEVEL_UP].Msgfunc = &GLCharacter::MsgProcessQuest;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_QUEST_PARTY_PROG_MOBKILL].Msgfunc = &GLCharacter::MsgProcessQuest;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_QUEST_PROG_QITEM].Msgfunc = &GLCharacter::MsgProcessQuest;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_QUEST_PARTY_PROG_QITEM].Msgfunc = &GLCharacter::MsgProcessQuest;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_QUEST_PROG_REACHZONE].Msgfunc = &GLCharacter::MsgProcessQuest;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_QUEST_PROG_TIMEOVER].Msgfunc = &GLCharacter::MsgProcessQuest;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_QUEST_PROG_TIME].Msgfunc = &GLCharacter::MsgProcessQuest;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_QUEST_PROG_NONDIE].Msgfunc = &GLCharacter::MsgProcessQuest;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_QUEST_PROG_LEAVEMAP].Msgfunc = &GLCharacter::MsgProcessQuest;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_QUEST_PROG_INVEN_INSERT].Msgfunc = &GLCharacter::MsgProcessQuest;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_QUEST_PROG_INVEN_DELETE].Msgfunc = &GLCharacter::MsgProcessQuest;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_QUEST_PROG_INVEN_TURN].Msgfunc = &GLCharacter::MsgProcessQuest;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_QUEST_PROG_INVEN_PICKUP].Msgfunc = &GLCharacter::MsgProcessQuest;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_QUEST_COMPLETE_FB].Msgfunc = &GLCharacter::MsgProcessQuest;
	m_GLCharacterMsgFunc[NET_MSG_GCTRL_REQ_INVEN_DECOMPOSE_FB].Msgfunc = &GLCharacter::MsgInvenItemDecomposeFb;

    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_CLUB_INFO_2CLT].Msgfunc = &GLCharacter::MsgProcessClub;
    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_CLUB_DEL_2CLT].Msgfunc = &GLCharacter::MsgProcessClub;
    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_CLUB_INFO_DISSOLUTION].Msgfunc = &GLCharacter::MsgProcessClub;
    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_CLUB_MEMBER_2CLT].Msgfunc = &GLCharacter::MsgProcessClub;
    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_CLUB_NEW_FB].Msgfunc = &GLCharacter::MsgProcessClub;
    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_CLUB_MEMBER_REQ_ASK].Msgfunc = &GLCharacter::MsgProcessClub;
    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_CLUB_MEMBER_REQ_FB].Msgfunc = &GLCharacter::MsgProcessClub;
    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_CLUB_MARK_CHANGE_2CLT].Msgfunc = &GLCharacter::MsgProcessClub;
    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_CLUB_RANK_2CLT].Msgfunc = &GLCharacter::MsgProcessClub;
    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_CLUB_RANK_FB].Msgfunc = &GLCharacter::MsgProcessClub;
    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_CLUB_MEMBER_NICK_FB].Msgfunc = &GLCharacter::MsgProcessClub;
    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_CLUB_MEMBER_STATE].Msgfunc = &GLCharacter::MsgProcessClub;
    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_CLUB_MEMBER_POS].Msgfunc = &GLCharacter::MsgProcessClub;
    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_CLUB_DISSOLUTION_FB].Msgfunc = &GLCharacter::MsgProcessClub;
    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_CLUB_MEMBER_DEL_2CLT].Msgfunc = &GLCharacter::MsgProcessClub;
    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_CLUB_MEMBER_DEL_FB].Msgfunc = &GLCharacter::MsgProcessClub;
    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_CLUB_MEMBER_SECEDE_FB].Msgfunc = &GLCharacter::MsgProcessClub;
    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_CLUB_AUTHORITY_REQ_ASK].Msgfunc = &GLCharacter::MsgProcessClub;
    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_CLUB_AUTHORITY_REQ_FB].Msgfunc = &GLCharacter::MsgProcessClub;
    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_CLUB_AUTHORITY_CLT].Msgfunc = &GLCharacter::MsgProcessClub;

    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_CLUB_CD_CERTIFY_FB].Msgfunc = &GLCharacter::MsgProcessClub;
    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_CLUB_COMMISSION_FB].Msgfunc = &GLCharacter::MsgProcessClub;
    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_CLUB_STORAGE_RESET].Msgfunc = &GLCharacter::MsgProcessClub;
    //m_GLCharacterMsgFunc[NET_MSG_CLUB_STORAGE_SEND_END].Msgfunc = &GLCharacter::MsgProcessClub;
    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_CLUB_STORAGE_UPDATE_MONEY].Msgfunc = &GLCharacter::MsgProcessClub;
    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_CLUB_GETSTORAGE_ITEM].Msgfunc = &GLCharacter::MsgProcessClub;
    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_CLUB_STORAGE_INSERT].Msgfunc = &GLCharacter::MsgProcessClub;
    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_CLUB_STORAGE_DELETE].Msgfunc = &GLCharacter::MsgProcessClub;
    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_CLUB_STORAGE_DEL_INS].Msgfunc = &GLCharacter::MsgProcessClub;
    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_CLUB_STORAGE_UPDATE_ITEM].Msgfunc = &GLCharacter::MsgProcessClub;
    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_CLUB_NOTICE_FB].Msgfunc = &GLCharacter::MsgProcessClub;
    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_CLUB_NOTICE_CLT].Msgfunc = &GLCharacter::MsgProcessClub;
    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_CLUB_MBR_RENAME_CLT].Msgfunc = &GLCharacter::MsgProcessClub;
    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_CLUB_SUBMASTER_FB].Msgfunc = &GLCharacter::MsgProcessClub;
    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_CLUB_SUBMASTER_BRD].Msgfunc = &GLCharacter::MsgProcessClub;

    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_CLUB_ALLIANCE_REQ_FB].Msgfunc = &GLCharacter::MsgProcessAlliance;
    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_CLUB_ALLIANCE_DEL_FB].Msgfunc = &GLCharacter::MsgProcessAlliance;
    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_CLUB_ALLIANCE_SEC_FB].Msgfunc = &GLCharacter::MsgProcessAlliance;
    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_CLUB_ALLIANCE_DIS_FB].Msgfunc = &GLCharacter::MsgProcessAlliance;
    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_CLUB_ALLIANCE_REQ_ASK].Msgfunc = &GLCharacter::MsgProcessAlliance;
    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_CLUB_ALLIANCE_ADD_CLT].Msgfunc = &GLCharacter::MsgProcessAlliance;
    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_CLUB_ALLIANCE_DEL_CLT].Msgfunc = &GLCharacter::MsgProcessAlliance;
    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_CLUB_ALLIANCE_DIS_CLT].Msgfunc = &GLCharacter::MsgProcessAlliance;

    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_CLUB_BATTLE_REQ_FB].Msgfunc = &GLCharacter::MsgProcessClubBattle;
    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_CLUB_BATTLE_REQ_ASK].Msgfunc = &GLCharacter::MsgProcessClubBattle;
    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_CLUB_BATTLE_BEGIN_CLT].Msgfunc = &GLCharacter::MsgProcessClubBattle;
    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_CLUB_BATTLE_BEGIN_CLT2].Msgfunc = &GLCharacter::MsgProcessClubBattle;
    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_CLUB_BATTLE_ARMISTICE_REQ_FB].Msgfunc = &GLCharacter::MsgProcessClubBattle;
    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_CLUB_BATTLE_ARMISTICE_REQ_ASK].Msgfunc = &GLCharacter::MsgProcessClubBattle;
    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_CLUB_BATTLE_OVER_CLT].Msgfunc = &GLCharacter::MsgProcessClubBattle;
    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_CLUB_BATTLE_SUBMISSION_REQ_FB].Msgfunc = &GLCharacter::MsgProcessClubBattle;
    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_CLUB_BATTLE_KILL_UPDATE].Msgfunc = &GLCharacter::MsgProcessClubBattle;
    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_CLUB_BATTLE_POINT_UPDATE].Msgfunc = &GLCharacter::MsgProcessClubBattle;

    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_ALLIANCE_BATTLE_REQ_FB].Msgfunc = &GLCharacter::MsgProcessAllianceBattle;
    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_ALLIANCE_BATTLE_REQ_ASK].Msgfunc = &GLCharacter::MsgProcessAllianceBattle;
    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_ALLIANCE_BATTLE_ARMISTICE_REQ_FB].Msgfunc = &GLCharacter::MsgProcessAllianceBattle;
    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_ALLIANCE_BATTLE_ARMISTICE_REQ_ASK].Msgfunc = &GLCharacter::MsgProcessAllianceBattle;
    //m_GLCharacterMsgFunc[NET_MSG_GCTRL_ALLIANCE_BATTLE_SUBMISSION_REQ_FB].Msgfunc = &GLCharacter::MsgProcessAllianceBattle;
    
    m_GLCharacterMsgFunc[NET_MSG_CHINA_GAINTYPE].Msgfunc = &GLCharacter::MsgChinaGainType;
    m_GLCharacterMsgFunc[NET_MSG_VIETNAM_GAINTYPE].Msgfunc = &GLCharacter::MsgVietnamGainType;
    m_GLCharacterMsgFunc[NET_MSG_VIETNAM_GAIN_MONEY].Msgfunc = &GLCharacter::MsgVietnamGainMoney;
    m_GLCharacterMsgFunc[NET_MSG_VIETNAM_GAIN_EXP].Msgfunc = &GLCharacter::MsgVietnamGainExp;
    m_GLCharacterMsgFunc[NET_MSG_VIETNAM_TIME_REQ_FB].Msgfunc = &GLCharacter::MsgVietnamTimeReqFb;
    
    m_GLCharacterMsgFunc[NET_MSG_GM_LIMIT_EVENT_APPLY_START].Msgfunc = &GLCharacter::MsgGmLimitEventApplyStart;
    m_GLCharacterMsgFunc[NET_MSG_GM_LIMIT_EVENT_APPLY_END].Msgfunc = &GLCharacter::MsgGmLimitEventApplyEnd;
    m_GLCharacterMsgFunc[NET_MSG_GM_LIMIT_EVENT_BEGIN_FB].Msgfunc = &GLCharacter::MsgGmLimitEventBeginFb;
    m_GLCharacterMsgFunc[NET_MSG_GM_LIMIT_EVENT_END_FB].Msgfunc = &GLCharacter::MsgGmLimitEventEndFb;
    m_GLCharacterMsgFunc[NET_MSG_GM_LIMIT_EVENT_TIME_REQ_FB].Msgfunc = &GLCharacter::MsgGmLimitEventTimeReqFb;
    
    m_GLCharacterMsgFunc[NET_MSG_CHAR_ACTIVE_VEHICLE_FC].Msgfunc = &GLCharacter::MsgActiveVehicleFb;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_LANDEFFECT_UPDATE_BRD].Msgfunc = &GLCharacter::MsgLandEffectUpdateBrd;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_MAPEFFECT_UPDATE_BRD].Msgfunc = &GLCharacter::MsgMapEffectUpdateBrd;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_GET_VEHICLE_FB].Msgfunc = &GLCharacter::MsgGetVehicleFb;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_UNGET_VEHICLE_FB].Msgfunc = &GLCharacter::MsgUngetVehicleFb;
    
    m_GLCharacterMsgFunc[NET_MSG_VEHICLE_REQ_SLOT_EX_HOLD_FB].Msgfunc = &GLCharacter::MsgVehicleReqSlotExHoldFb;
    m_GLCharacterMsgFunc[NET_MSG_VEHICLE_REQ_HOLD_TO_SLOT_FB].Msgfunc = &GLCharacter::MsgVehicleReqHoldToSlotFb;
    m_GLCharacterMsgFunc[NET_MSG_VEHICLE_REQ_SLOT_TO_HOLD_FB].Msgfunc = &GLCharacter::MsgVehicleReqSlotToHoldFb;	
    m_GLCharacterMsgFunc[NET_MSG_VEHICLE_REMOVE_SLOTITEM_FB].Msgfunc = &GLCharacter::MsgVehicleRemoveSlotItemFb;
    m_GLCharacterMsgFunc[NET_MSG_VEHICLE_ACCESSORY_DELETE].Msgfunc = &GLCharacter::MsgVehicleAccessoryDelete;
    m_GLCharacterMsgFunc[NET_MSG_VEHICLE_UPDATE_CLIENT_BATTERY].Msgfunc = &GLCharacter::MsgVehicleUpdateClientBattery;
    m_GLCharacterMsgFunc[NET_MSG_VEHICLE_REQ_GIVE_BATTERY_FB].Msgfunc = &GLCharacter::MsgVehicleReqGiveBatteryFb;
    m_GLCharacterMsgFunc[NET_MSG_VEHICLE_REQ_ITEM_INFO_FB].Msgfunc = &GLCharacter::MsgVehicleReqItemInfoFb;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_WITH_VEHICLE_FB].Msgfunc = &GLCharacter::MsgWithVehicleFb;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_ACTIVE_WITH_VEHICLE_FB].Msgfunc = &GLCharacter::MsgActiveWithVehicleFb;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_WITH_VEHICLE_MOVE_MAP].Msgfunc = &GLCharacter::MsgWithVehicleMoveMap;    
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_VEHICLE_BRD].Msgfunc = &GLCharacter::MsgVehicleBrd;	
    
    m_GLCharacterMsgFunc[NET_QBOX_OPTION_MEMBER].Msgfunc = &GLCharacter::MsgQboxOptionMember;
    m_GLCharacterMsgFunc[NET_MSG_UPDATE_TRACING_CHARACTER].Msgfunc = &GLCharacter::MsgUpdateTracingCharacter;
    
    m_GLCharacterMsgFunc[NET_MSG_REQ_ATTENDLIST_FB].Msgfunc = &GLCharacter::MsgReqAttendListFb;
    m_GLCharacterMsgFunc[NET_MSG_REQ_ATTENDANCE_FB].Msgfunc = &GLCharacter::MsgReqAttendanceFb;
    m_GLCharacterMsgFunc[NET_MSG_REQ_ATTEND_REWARD_CLT].Msgfunc = &GLCharacter::MsgReqAttendRewardClt;
    
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_NPC_RECALL_FB].Msgfunc = &GLCharacter::MsgNpcRecallFb;    
    m_GLCharacterMsgFunc[NET_MSG_INVEN_ITEM_MIX_FC].Msgfunc = &GLCharacter::MsgInvenItemMixFb;
    m_GLCharacterMsgFunc[NET_MSG_REQ_GATHERING_FB].Msgfunc = &GLCharacter::MsgReqGatheringFb;
    m_GLCharacterMsgFunc[NET_MSG_REQ_GATHERING_RESULT].Msgfunc = &GLCharacter::MsgReqGatheringResult;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_UPDATE_CDM_SAFETIME_BRD].Msgfunc = &GLCharacter::MsgUpdateCdmSafeTimeBrd;
	m_GLCharacterMsgFunc[NET_MSG_GCTRL_DIALOGUEWINDOW_OPEN_FB].Msgfunc = &GLCharacter::MsgReqDialogueWindowOpenFb;
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_REQ_NPCTALK_BASIC_FB].Msgfunc = &GLCharacter::MsgReqNpcTalkBasicFb;

    m_GLCharacterMsgFunc[NET_MSG_ACTIVITY_NOTIFY_COMPLETED].Msgfunc  = &GLCharacter::MsgActivityNotifyComplate;    
    m_GLCharacterMsgFunc[NET_MSG_ACTIVITY_NOTIFY_CLOSED].Msgfunc     = &GLCharacter::MsgActivityNotifyClosed;    
    m_GLCharacterMsgFunc[NET_MSG_ACTIVITY_UPDATE_VALUE].Msgfunc      = &GLCharacter::MsgActivityUpdateValue;
    m_GLCharacterMsgFunc[NET_MSG_ACTIVITY_UPDATE_COMPLETED].Msgfunc  = &GLCharacter::MsgActivityUpdateComplate;
    m_GLCharacterMsgFunc[NET_MSG_ACTIVITY_SYNC].Msgfunc              = &GLCharacter::MsgActivitySync;
    m_GLCharacterMsgFunc[NET_MSG_ACTIVITY_REWARD_POINT_SYNC].Msgfunc = &GLCharacter::MsgActivityRewardPoint;
    m_GLCharacterMsgFunc[NET_MSG_ACTIVITY_REWARD_BADGE_SYNC].Msgfunc = &GLCharacter::MsgActivityRewardBadge;
    m_GLCharacterMsgFunc[NET_MSG_ACTIVITY_CHANGE_BADGE_FB].Msgfunc   = &GLCharacter::MsgActivityChangeBadgeFB;
	m_GLCharacterMsgFunc[NET_MSG_ACTIVITY_LOAD_COMPLETED_FF].Msgfunc   = &GLCharacter::MsgActivityLoadCompletedFF;
    
	m_GLCharacterMsgFunc[NET_MSG_GET_CHA_EXTRA_INFO_FH_FB].Msgfunc = &GLCharacter::MsgGetChaExtraInfo;

    m_GLCharacterMsgFunc[NET_MSG_ADD_REBUY_ITEM_AC].Msgfunc = &GLCharacter::MsgAddRebuyItem;
    m_GLCharacterMsgFunc[NET_MSG_BUY_REBUY_ITEM_FC].Msgfunc = &GLCharacter::MsgBuyRebuyItem;
    m_GLCharacterMsgFunc[NET_MSG_CHECK_REBUY_SUPPLIES].Msgfunc = &GLCharacter::MsgCheckRebuySupplies;
	
	m_GLCharacterMsgFunc[NET_MSG_GCTRL_PMARKET_SEARCH_RESULT_HAC].Msgfunc = &GLCharacter::MsgPrivateMarketSearch;

    // 개인상점 검색카드
    m_GLCharacterMsgFunc[NET_MSG_GCTRL_PMARKET_SEARCH_OPEN_FB].Msgfunc = &GLCharacter::MsgInvenPMarketSearchOpenFb;

	//Rnattendance rn출석부
	m_GLCharacterMsgFunc[NET_MSG_RNATTENDANCE_QUEST_LIST_FC].Msgfunc = &GLCharacter::MsgProcessRnattendance;

	m_GLCharacterMsgFunc[NET_MSG_GCTRL_SKILLDELAY].Msgfunc = &GLCharacter::MsgSkillDelay;

	m_GLCharacterMsgFunc[NET_MSG_GCTRL_REQ_EXCHANGE_BUY_FROM_NPC].Msgfunc = &GLCharacter::MsgExchangeItemNpc;

	m_GLCharacterMsgFunc[NET_MSG_GCTRL_INVEN_COSTUME_STATS_FB].Msgfunc = &GLCharacter::MsgCostumeStatsFb;
	m_GLCharacterMsgFunc[NET_MSG_DRUG_FB].Msgfunc = &GLCharacter::MsgDrugFb;

	m_GLCharacterMsgFunc[NET_MSG_GCTRL_INVEN_COSTUEM_RELEASE_FB].Msgfunc = &GLCharacter::MsgCostumeStatsReleaseFb;

	m_GLCharacterMsgFunc[NET_MSG_GCTRL_REQ_JUMPINGLEVELUP_FB].Msgfunc = &GLCharacter::MsgJumpingLevelUpFb;
	m_GLCharacterMsgFunc[NET_MSG_GCTRL_INVEN_SELFORMBOX_SELECTOPEN_FB].Msgfunc = &GLCharacter::MsgSelformBoxSelectOpenFb;

	// 경험치 압축 / 캡슐
	m_GLCharacterMsgFunc[NET_MSG_GCTRL_EXP_COMPRESSOR_ANS].Msgfunc = &GLCharacter::MsgExpCompressorAns;
	m_GLCharacterMsgFunc[NET_MSG_GCTRL_EXP_CAPSULE_ANS].Msgfunc = &GLCharacter::MsgExpCapsuleAns;

	m_GLCharacterMsgFunc[NET_MSG_GCTRL_INVEN_TL_GRINDING].Msgfunc = &GLCharacter::MsgInvenTLGrinding;

	m_GLCharacterMsgFunc[NET_MSG_GCTRL_FORCERESET_SKILL_STAT_FC].Msgfunc = &GLCharacter::MsgForceresetSkillStatFC;

	// 아이템 스킬 변경;
	m_GLCharacterMsgFunc[NET_MSG_INVEN_CHANGE_ITEMSKILL_UIOPEN_FB].Msgfunc = &GLCharacter::MsgInvenChangeItemSkillFB;

	// 아이템 기본능력치 변경;
	m_GLCharacterMsgFunc[NET_MSG_INVEN_CHANGE_BASICSTATS_UIOPEN_FB].Msgfunc = &GLCharacter::MsgInvenChangeBasicStatsFB;

	// 아이템 가산능력치 변경;
	m_GLCharacterMsgFunc[NET_MSG_INVEN_CHANGE_ADDSTATS_UIOPEN_FB].Msgfunc = &GLCharacter::MsgInvenChangeAddStatsFB;
}

void GLCharacter::MsgProcess(NET_MSG_GENERIC* pMsg)
{
	EMNET_MSG emType = pMsg->Type();
    if (emType < 0 || emType >= NET_MSG_GCTRL_MAX)
        return;
	
    if (m_GLCharacterMsgFunc[emType].Msgfunc)
        (this->*m_GLCharacterMsgFunc[emType].Msgfunc) (pMsg);
    else
		CDebugSet::ToListView("GLCharacter::MsgProcess unknown message type %d", pMsg->Type());
}

void GLCharacter::MsgTradeProcess(NET_MSG_GENERIC* pMsg)
{
    m_pGaeaClient->GetTradeClient()->MsgProcess(pMsg);
}
