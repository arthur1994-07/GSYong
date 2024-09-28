#include "Stdafx.h"

#include "./Factory.h"
#include "../../RanLogic/GLUseFeatures.h"

#include "../OldUI/Interface/MiniMap.h"
#include "../OldUI/DPS/DPSWindow.h"
#include "../OldUI/Option/KeySettingWindow.h"
#include "../OldUI/Option/ChatMacroWindow.h"
#include "../OldUI/Display/NameDisplayMan.h"
#include "../OldUI/PrivateMarket/PrivateMarketShowMan.h"
#include "../OldUI/Display/SummonNameDisplayMan.h"
#include "../OldUI/Skill/SkillWindowToTray.h"
#include "../OldUI/Party/PartyWindow.h"
#include "../OldUI/PartyRenewal/MainWindow/PartyWindowRenewal.h"
#include "../OldUI/PartyRenewal/PartyYesNoWindow.h"
#include "../OldUI/PartyRenewal/ExpeditionInviteWindow/ExpeditionInviteWindow.h"
#include "../OldUI/PartyRenewal/CheckReadyWindow/ExpeditionCheckReadyWindow.h"
#include "../OldUI/PartyRenewal/StateWindow/PartyStateWindow.h"
#include "../OldUI/PartyRenewal/TenderWindow/PartyTenderWindow.h"
#include "../OldUI/PartyRenewal/TenderWindow/PartyTenderNotifyButton.h"
#include "../OldUI/PartyRenewal/DistributionWindow/PartyDistributionWindow.h"
#include "../OldUI/PartyRenewal/DistributionWindow/PartyDistributionListWindow.h"
#include "../OldUI/PartyRenewal/MiniWindow/MiniExpeditionWindow.h"
#include "../OldUI/groupchat/ChatGroupModalWindow.h"
#include "../OldUI/Interface/BasicChat.h"
#include "../OldUI/Interface/ChatOptionWindow.h"
#include "../OldUI/Trade/TradeWindow.h"
#include "../OldUI/Trade/TradeInventoryWindow.h"
#include "../OldUI/Char/PtoPWindow.h"
#include "../OldUI/Competition/CompetitionNotifyButton.h"
#include "../OldUI/Item/InventoryWindow.h"
#include "../OldUI/Item/ItemMove.h"
#include "../OldUI/Item/MarketWindow.h"
#include "../OldUI/Display/HeadChatDisplayMan.h"
#include "../OldUI/Confront/ConftModalWindow.h"
#include "../OldUI/Party/PartyModalWindow.h"
#include "../OldUI/Confront/ConftDisplayMan.h"
#include "../OldUI/Quest/QuestWindow.h"
#include "../OldUI/Quest/WindowRenewal/QuestWindowRenewal.h"
#include "../OldUI/Friend/FriendWindow.h"
#include "../OldUI/Friend/RnFriendWindow.h"
#include "../OldUI/GroupChat/ChatGroupInviteWindow.h"
#include "../OldUI/Util/ModalWindowInner.h"
#include "../OldUI/PrivateMarket/ShopItemSearchWindow.h"
#include "../OldUI/PrivateMarket/ItemSearchResultWindow.h"
#include "../OldUI/Message/MapRequireCheck.h"
#include "../OldUI/Post/PostBoxWindow.h"
#include "../OldUI/Post/PostNotifyButton.h"
#include "../OldUI/Quest/QuestHelper.h"
#include "../OldUI/Tutorial/TutorialDialogue.h"
#include "../OldUI/Tutorial/BallonText.h"
#include "../OldUI/Skill/SkillWindow.h"
#include "../OldUI/Skill/RnSkillWindow.h"
#include "../OldUI/Interface/BasicInfoView.h"
#include "../OldUI/Tutorial/ExplainInterface.h"
#include "../OldUI/Interface/UILeftTopGroup.h"
#include "../OldUI/Interface/SkillTrayTab.h"
#include "../OldUI/Interface/BasicQuickSkillSlot.h"
#include "../OldUI/Interface/ChatTypeList.h"
#include "../OldUI/Display/TargetInfoDisplay.h"
#include "../OldUI/Display/CursorTargetInfo.h"
#include "../OldUI/Tutorial/TutorialViewStep.h"
#include "../OldUI/Interface/LunchBoxTimeDisplay.h"
#include "../OldUI/Interface/SystemBuffTimeDisplay.h"
#include "../OldUI/Vehicle/VehicleWindow.h"
#include "../OldUI/Display/ProgressDisplay.h"
#include "../OldUI/StudentRecord/StudentRecordWindow.h"
#include "../OldUI/PointShop/PointShopWindow.h"
#include "../OldUI/Message/RebirthWindow.h"
#include "../OldUI/Util/PopupMenu.h"
#include "../OldUI/Display/InfoCTFDisplay.h"
#include "../OldUI/Display/CTFResultWindow.h"
#include "../OldUI/Competition/CompetitionWindow.h"
#include "../OldUI/Competition/CompetitionTopRanking.h"
#include "../OldUI/Interface/ReservedSkillAnimation.h"
#include "../OldUI/Product/ProductButton.h"
#include "../OldUI/Product/ProductWindow.h"
#include "../OldUI/Product/ProductProgress.h"
#include "../OldUI/Product/ProductResetWindow.h"
#include "../OldUI/MiniGame/OddEven/OddEvenWindow.h"
#include "../OldUI/MiniGame/TexasHoldem/TexasHoldemWindow.h"
#include "../OldUI/MiniGame/TexasHoldem/TexasHoldemWebWindow.h"
#include "../OldUI/Club/ClubWindow.h"
#include "../OldUI/Club/WbClubWindow.h"
#include "../OldUI/PrivateMarketSearchBuy/PrivateMarketSearchBuy.h"
#include "../OldUI/Competition/PVEUserInterface.h"
#include "../OldUI/Feature/ItemRebuildRenewal.h"
#include "../OldUI/Message/ProgessTimer.h"
#include "../OldUI/MiniGame/ColorProperty/ColorProperty.h"


// Lotto System;
#include "../OldUI/LottoSystem/LottoMainUI.h"
#include "../OldUI/LottoSystem/LottoBuyLottoUI.h"
#include "../OldUI/LottoSystem/LottoBuyListInfo.h"
#include "../OldUI/LottoSystem/LottoWinManListInfo.h"
#include "../OldUI/LottoSystem/LottoDisplayUI.h"
#include "../OldUI/LottoSystem/LottoNotifyButton.h"

// New Instance System;
#include "../OldUI/Interface/SimpleSystemMessageDisplay.h"
#include "../OldUI/InstanceSystem/SimpleTimerMessageBox.h"
#include "../OldUI/Display/RnCDMProgress.h"
#include "../OldUI/Display/RnCDMRankWindow.h"
#include "../OldUI/Display/RnCDMClubInfo.h"
#include "../OldUI/Competition/RnCompetitionWindow.h"
#include "../OldUI/Competition/RnCompetitionResultWindow.h"
#include "../OldUI/Competition/RnCDMTopRanking.h"
#include "../OldUI/Display/CaptureTheFlagEffectMan.h"
#include "../OldUI/Display/GuidanceClubBattleUI.h"
#include "../OldUI/Display/CaptureTheIslandsUI.h"

#include "../OldUI/Tournament/TournamentDashboard.h"
#include "../OldUI/Tournament/TournamentToTo.h"
#include "../OldUI/Tournament/TournamentUI.h"
#include "../OldUI/Tournament/TournamentMoreInfo.h"
#include "../OldUI/Tournament/TournamentUserInterface.h"

#include "../OldUI/FlyCameraControl/FlyCameraControlWindow.h"
#include "../OldUI/Setitem/CSetitemPreviewWindow.h"

#include "../OldUI/Display/SummonStateDisplayUI.h"

#include "../OldUI/FactionSystem/FactionInfoWindow.h"

#include "../OldUI/WorldBattleSystem/WorldBattleButton.h"

MyFactoryUI::MyFactoryUI() 
: m_factoryMinimap( fnPtrOldFactoryMinimap )
, m_factoryKeySettingWindow( fnPtrOldFactoryKeySettingWindow )
, m_factoryChatMacroWindow( fnPtrOldFactoryChatMacroWindow )
, m_factoryNameDisplayMan( fnPtrOldFactoryNameDisplayMan )
, m_factoryPrivateMarketShowMan( fnPtrOldFactoryPrivateMarketShowMan )
, m_factorySummonNameDisplayMan( fnPtrOldFactorySummonNameDisplayMan )
, m_factorySkillWindowToTray( fnPtrOldFactorySkillWindowToTray )
, m_factoryPartyWindow( fnPtrOldFactoryPartyWindow )
, m_factoryPartyWindowRenewal( fnPtrOldFactoryPartyWindowRenewal )
, m_factoryPartyYesNoWindow( fnPtrOldFactoryPartyYesNoWindow )
, m_factoryExpeditionInviteWindow( fnPtrOldFactoryExpeditionInviteWindow )
, m_factoryExpeditionCheckReadyWindow( fnPtrOldFactoryExpeditionCheckReadyWindow )
, m_factoryPartyStateWindow( fnPtrOldFactoryPartyStateWindow )
, m_factoryPartyTenderWindow( fnPtrOldFactoryPartyTenderWindow )
, m_factoryPartyTenderNotifyButton( fnPtrOldFactoryPartyTenderNotifyButton )
, m_factoryPartyDistributionWindow( fnPtrOldFactoryPartyDistributionWindow )
, m_factoryPartyDistributionListWindow( fnPtrOldFactoryPartyDistributionListWindow )
, m_factoryMiniExpeditionWindow( fnPtrOldFactoryMiniExpeditionWindow )
, m_factoryChat( fnPtrOldFactoryBasicChatWindow )
, m_factoryChatOptionWindow( fnPtrOldFactoryChatOptionWindow )
, m_factoryTradeWindow( fnPtrOldFactoryTradeWindow )
, m_factoryTradeInventoryWindow( fnPtrOldFactoryTradeInventoryWindow )
, m_factoryPtoPWindow( fnPtrOldFactoryPtoPWindow )
, m_factoryInventoryWindow( fnPtrOldFactoryInventoryWindow )
, m_factoryItemMove( fnPtrOldFactoryItemMove )
, m_factoryMarketWindow( fnPtrOldFactoryMarketWindow )
, m_factoryHeadChatDisplayMan( fnPtrOldFactoryHeadChatDisplayMan )
, m_factoryConftModalWindow( fnPtrOldFactoryConftModalWindow )
, m_factoryPartyModalWindow( fnPtrOldFactoryPartyModalWindow )
, m_factoryChatGroupModalWindow( fnPtrOldFactoryChatGroupModalWindow )
, m_factoryConftDisplayMan( fnPtrOldFactoryConftDisplayMan )
, m_factoryQuestWindow( fnPtrOldFactoryQuestWindow )
, m_factoryQuestWindowRenewal( fnPtrOldFactoryQuestWindowRenewal )
, m_factoryFriendWindow( fnPtrOldFactoryFriendWindow )
, m_factoryChatGroupInviteWindow( fnPtrOldFacotryChatGroupInviteWindow )
, m_factoryModalWindow( fnPtrOldFactoryModalWindow )
, m_factoryShopItemSearchWindow( fnPtrOldFactoryShopItemSearchWindow )
, m_factoryItemSearchResultWindow( fnPtrOldFactoryItemSearchResultWindow )
, m_factoryMapRequireCheck( fnPtrOldFactoryMapRequireCheck )
, m_factoryPostBoxWindow( fnPtrOldFactoryPostBoxWindow )
, m_factoryPostNotifyButton( fnPtrOldFactoryPostNotifyButton )
, m_factoryQuestHelper( fnPtrOldFactoryQuestHelper )
, m_factoryTutorialDialogue( fnPtrOldFactoryTutorialDialogue )
, m_factoryBalloonText( fnPtrOldFactoryBalloonText )
, m_factorySkillWindow( fnPtrOldFactorySkillWindow )
, m_factoryBasicInfoView( fnPtrOldFactoryBasicInfoView )
, m_factoryExplainInterface( fnPtrOldFactoryExplainInterface )
, m_factoryUILeftTopGroup( fnPtrOldFactoryUILeftTopGroup )
, m_factorySkillTrayTab( fnPtrOldFactorySkillTrayTab )
, m_factoryBasicQuickSkillSlot( fnPtrOldFactoryBasicQuickSkillSlot )
, m_factoryTargetInfoDisplay( fnPtrOldFactoryTargetInfoDisplay )
, m_factoryCursorTargetInfo( fnPtrOldFactoryCursorTargetInfo )
, m_factoryBalloonSpaceBar( fnPtrOldFactoryBalloonSpaceBar )
, m_factoryTutorialViewStep( fnPtrOldFactoryTutorialViewStep )
, m_factoryLunchBoxTimeDisplay( fnPtrOldFactoryLunchBoxTimeDisplay )
, m_factorySystemBuffTimeDisplay( fnPtrOldFactorySystemBuffTimeDisplay )
, m_factoryVehicleWindow( fnPtrOldFactoryVehicleWindow )
, m_factoryProgressDisplay( fnPtrOldFactoryProgressDisplay )
, m_factoryStudentRecordWindow( fnPtrOldFactoryStudentRecordWindow )
, m_factoryPointShopWindow( fnPtrOldFactoryPointShopWindow )
, m_factoryRebirthWindow( fnPtrOldFactoryRebirthWindow )
, m_factoryChatTypeList( fnPtrOldFactoryChatTypeList )
, m_factoryCompetitionNotifyButton( fnPtrOldFactoryCompetitionNotifyButton )
, m_factoryPopupMenu( fnPtrOldFactoryPopupMenu )
, m_factoryInfoCTFDisplay( fnPtrOldFactoryInfoCTFDisplay )
, m_factoryCTFResultWindow( fnPtrOldFactoryCTFResultWindow )
, m_factoryCompetitionWindow( fnPtrOldFactoryCompetitionWindow )
, m_factoryCompetitionTopRanking( fnPtrOldFactoryCompetitionTopRanking )
, m_factoryReservedSkillAnimation( fnPtrOldFactoryReservedSkillAnimation )
, m_factoryProductButton( fnPtrOldFactoryProductButton )
, m_factoryProductWindow( fnPtrOldFactoryProductWindow )
, m_factoryProductProgress( fnPtrOldFactoryProductProgress )
, m_factoryProductResetWindow( fnPtrOldFactoryProductResetWindow )
, m_factoryOddEvenWindow( fnPtrOldFactoryOddEvenWindow )
, m_factoryTexasHoldemWindow( fnPtrOldFactoryTexasHoldemWindow )
, m_factoryTexasHoldemWebWindow( fnPtrOldFactoryTexasHoldemWebWindow )
, m_factoryClubWindow( fnPtrOldFactoryClubWindow )
, m_factoryPrivateMarketSearchBuy( fnPtrOldFactoryPrivateMarketSearchBuy )
, m_factoryDPSWindow( fnPtrOldFactoryDPSWindow )
, m_factoryPVEUserInterface( fnPtrOldFactoryPVEUserInterface )
, m_factoryTournamentUserInterface(fnPtrOldFactoryTournamentUserInterface)
, m_factoryItemRebuildRenewal( fnPtrOldFactoryItemRebuildRenewal )
, m_factoryProgessTimer ( fnPtrOldFactoryProgessTimer )
, m_factoryColorProperty ( fnptroldFactoryColorProperty )
// Lotto System----------------------------------------------
, m_factoryLottoMainUI ( fnPtrLottoMainUI )
, m_factoryLottoBuyLottoUI ( fnPtrLottoBuyLottoUI )
, m_factoryLottoBuyListInfo ( fnPtrLottoBuyListInfo )
, m_factoryLottoWinManListInfo ( fnPtrLottoWinManListInfo )
, m_factoryLottoDisplayUI( fnPtrLottoDisplayUI )
, m_factoryLottoNotifyButton( fnPtrLottoNotifyButton )
// New Instance System--------------------------------------
, m_factorySimpleSystemMessageDisplay( fnptroldFactorySimpleSystemMessageDisplay)
, m_factorySimpleTimerMessageBox( fnptroldFactorySimpleTimerMessageBox)
, m_factoryRnCDMprogress ( fnptroldFactoryRnCDMProgress )
, m_factoryRnCDMRankWindow ( fnptroldFactoryRnCDMRankWindow )
, m_factoryRnCDMClubInfo ( fnptroldFactoryRnCDMClubInfo )
, m_factoryRnCompetitionWindow( fnPtrOldFactoryRnCompetitionWindow )
, m_factoryRnCompetitionResultWindow( fnPtrOldFactoryRnCompetitionResultWindow )
, m_factoryRnCDMTopRanking( fnPtroldFactoryRnCDMTopRanking )
, m_factoryCaptureTheFlagEffectMan( fnPtrCaptureTheFlagEffectMan )
, m_factoryGuidanceAuthProgressUI(fnPtrGuidanceAuthProgressUI)
, m_factoryGuidanceProgressInfoUI(fnPtrGuidanceProgressInfoUI)
, m_factoryGuidanceRankInfoUI(fnPtrGuidanceRankInfoUI)
, m_factoryCTIAuthProgressUI(fnPtrCTIAuthProgressUI)
, m_factoryCTIProgressInfoUI(fnPtrCTIProgressInfoUI)
, m_factoryCTIRankInfoUI(fnPtrCTIRankInfoUI)
// Tournament
, m_factoryTournamentWindowDashBoard(fnptroldFactoryTournamentWindowDashBoard)
, m_factoryTournamentWindowUI(fnptroldFactoryTournamentWindowUI)
, m_factoryTournamentToToWindow(fnptroldFactoryTournamentToToWindow)
, m_factoryTournamentMoreInfoWindow(fnptroldFactoryTournamentMoreInfoWindow)
, m_factoryFlyCameraControlWindow(fnptroldFactoryFlyCameraControlWindow)
, m_factorySetitemPreviewWindow(fnptroldFactorySetitemPreviewWindow)
, m_factorySummonStateDisplayUI(fnptrSummonStateDisplayUI)
, m_factoryFactionInfoWindow(fnptrFactionInfoWindow)
, m_factoryWorldBattleButton(fnptrWorldBattleButton)
{
}

AP_IMiniMap MyFactoryUI::CreateMiniMap( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_IMiniMap apCreateMiniMap = m_factoryMinimap( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apCreateMiniMap.get() );
    return apCreateMiniMap;
}

AP_IKeySettingWindow MyFactoryUI::CreateKeySettingWindow( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_IKeySettingWindow apCreateKeySettingWindow = m_factoryKeySettingWindow( pInterface, pEngineDevice );
    GASSERT( apCreateKeySettingWindow.get() );
    return apCreateKeySettingWindow;
}

AP_IChatMacroWindow MyFactoryUI::CreateChatMacroWindow( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_IChatMacroWindow apCreateChatMacroWindow = m_factoryChatMacroWindow( pInterface, pEngineDevice );
    GASSERT( apCreateChatMacroWindow.get() );
    return apCreateChatMacroWindow;
}

AP_INameDisplayMan MyFactoryUI::CreateNameDisplayMan( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_INameDisplayMan apCreateNameDisplayMan = m_factoryNameDisplayMan( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apCreateNameDisplayMan.get() );
    return apCreateNameDisplayMan;
}

AP_IPrivateMarketShowMan MyFactoryUI::CreatePrivateMarketShowMan( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_IPrivateMarketShowMan apCreatePrivateMarketShowMan = m_factoryPrivateMarketShowMan( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apCreatePrivateMarketShowMan.get() );
    return apCreatePrivateMarketShowMan;
}

AP_ISummonNameDisplayMan MyFactoryUI::CreateSummonNameDisplayMan( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_ISummonNameDisplayMan apCreateSummonNameDisplayMan = m_factorySummonNameDisplayMan( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apCreateSummonNameDisplayMan.get() );
    return apCreateSummonNameDisplayMan;
}

AP_ISkillWindowToTray MyFactoryUI::CreateSkillWindowToTray( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_ISkillWindowToTray apCreateSkillWindowToTray = m_factorySkillWindowToTray( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apCreateSkillWindowToTray.get() );
    return apCreateSkillWindowToTray;
}

AP_IPartyWindow MyFactoryUI::CreatePartyWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_IPartyWindow apCreatePartyWindow = m_factoryPartyWindow( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apCreatePartyWindow.get() );
    return apCreatePartyWindow;
}

AP_IPartyWindowRenewal MyFactoryUI::CreatePartyWindowRenewal( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	AP_IPartyWindowRenewal apCreatePartyWindowRenewal = m_factoryPartyWindowRenewal( pGaeaClient, pInterface, pEngineDevice );
	GASSERT( apCreatePartyWindowRenewal.get() );
	return apCreatePartyWindowRenewal;
}

AP_IPartyYesNoWindow MyFactoryUI::CreatePartyYesNoWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	AP_IPartyYesNoWindow apCreatePartyYesNoWindow = m_factoryPartyYesNoWindow( pGaeaClient, pInterface, pEngineDevice );
	GASSERT( apCreatePartyYesNoWindow.get() );
	return apCreatePartyYesNoWindow;
}


AP_IExpeditionInviteWindow MyFactoryUI::CreateExpeditionInviteWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	AP_IExpeditionInviteWindow apCreateExpeditionInviteWindow = m_factoryExpeditionInviteWindow( pGaeaClient, pInterface, pEngineDevice );
	GASSERT( apCreateExpeditionInviteWindow.get() );
	return apCreateExpeditionInviteWindow;
}

AP_IExpeditionCheckReadyWindow MyFactoryUI::CreateExpeditionCheckReadyWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	AP_IExpeditionCheckReadyWindow apCreateExpeditionCheckReadyWindow = m_factoryExpeditionCheckReadyWindow( pGaeaClient, pInterface, pEngineDevice );
	GASSERT( apCreateExpeditionCheckReadyWindow.get() );
	return apCreateExpeditionCheckReadyWindow;
}


AP_IPartyStateWindow MyFactoryUI::CreatePartyStateWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	AP_IPartyStateWindow apCreatePartyStateWindow = m_factoryPartyStateWindow( pGaeaClient, pInterface, pEngineDevice );
	GASSERT( apCreatePartyStateWindow.get() );
	return apCreatePartyStateWindow;
}

AP_IPartyTenderWindow MyFactoryUI::CreatePartyTenderWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	AP_IPartyTenderWindow apPartyTenderWindow = m_factoryPartyTenderWindow( pGaeaClient, pInterface, pEngineDevice );
	GASSERT( apPartyTenderWindow.get() );
	return apPartyTenderWindow;
}

AP_IPartyTenderNotifyButton MyFactoryUI::CreatePartyTenderNotifyButton( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	AP_IPartyTenderNotifyButton apPartyTenderNotifyButton = m_factoryPartyTenderNotifyButton( pGaeaClient, pInterface, pEngineDevice );
	GASSERT( apPartyTenderNotifyButton.get() );
	return apPartyTenderNotifyButton;
}

AP_IPartyDistributionWindow MyFactoryUI::CreatePartyDistributionWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	AP_IPartyDistributionWindow apPartyDistributionWindow = m_factoryPartyDistributionWindow( pGaeaClient, pInterface, pEngineDevice );
	GASSERT( apPartyDistributionWindow.get() );
	return apPartyDistributionWindow;
}

AP_IPartyDistributionListWindow MyFactoryUI::CreatePartyDistributionListWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	AP_IPartyDistributionListWindow apPartyDistributionListWindow = m_factoryPartyDistributionListWindow( pGaeaClient, pInterface, pEngineDevice );
	GASSERT( apPartyDistributionListWindow.get() );
	return apPartyDistributionListWindow;
}


AP_IMiniExpeditionWindow MyFactoryUI::CreateMiniExpeditionWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	AP_IMiniExpeditionWindow apCreateMiniExpeditionWindow = m_factoryMiniExpeditionWindow( pGaeaClient, pInterface, pEngineDevice );\
	GASSERT( apCreateMiniExpeditionWindow.get() );
	return apCreateMiniExpeditionWindow;
}

AP_IBasicChatWindow MyFactoryUI::CreateBasicChatWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_IBasicChatWindow apCreateChat = m_factoryChat( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apCreateChat.get() );
    return apCreateChat;
}

AP_IChatOptionWindow MyFactoryUI::CreateChatOptionWindow( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_IChatOptionWindow apCreateChatOptionWindow = m_factoryChatOptionWindow( pInterface, pEngineDevice );
    GASSERT( apCreateChatOptionWindow.get() );
    return apCreateChatOptionWindow;
}

AP_ITradeWindow MyFactoryUI::CreateTradeWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_ITradeWindow apCreateTradeWindow = m_factoryTradeWindow( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apCreateTradeWindow.get() );
    return apCreateTradeWindow;
}

AP_ITradeInventoryWindow MyFactoryUI::CreateTradeInventoryWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_ITradeInventoryWindow apCreateTradeInventoryWindow = m_factoryTradeInventoryWindow( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apCreateTradeInventoryWindow.get() );
    return apCreateTradeInventoryWindow;
}

AP_IPtoPWindow MyFactoryUI::CreatePtoPWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_IPtoPWindow apCreatePtoPWindow = m_factoryPtoPWindow( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apCreatePtoPWindow.get() );
    return apCreatePtoPWindow;
}

AP_IInventoryWindow MyFactoryUI::CreateInventoryWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_IInventoryWindow apCreateInventoryWindow = m_factoryInventoryWindow( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apCreateInventoryWindow.get() );
    return apCreateInventoryWindow;
}

AP_IItemMove MyFactoryUI::CreateItemMove( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_IItemMove apCreateItemMove = m_factoryItemMove( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apCreateItemMove.get() );
    return apCreateItemMove;
}

AP_IMarketWindow MyFactoryUI::CreateMarketWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_IMarketWindow apCreateMarketWindow = m_factoryMarketWindow( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apCreateMarketWindow.get() );
    return apCreateMarketWindow;
}

AP_IHeadChatDisplayMan MyFactoryUI::CreateHeadChatDisplayMan( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_IHeadChatDisplayMan apCreateHeadChatDisplayMan = m_factoryHeadChatDisplayMan( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apCreateHeadChatDisplayMan.get() );
    return apCreateHeadChatDisplayMan;
}

AP_IConftModalWindow MyFactoryUI::CreateConftModalWindow( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_IConftModalWindow apCreateConftModalWindow = m_factoryConftModalWindow( pInterface, pEngineDevice );
    GASSERT( apCreateConftModalWindow.get() );
    return apCreateConftModalWindow;
}

AP_IPartyModalWindow MyFactoryUI::CreatePartyModalWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_IPartyModalWindow apCreatePartyModalWindow = m_factoryPartyModalWindow( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apCreatePartyModalWindow.get() );
    return apCreatePartyModalWindow;
}

AP_IChatGroupModalWindow MyFactoryUI::CreateChatGroupModalWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	AP_IChatGroupModalWindow apCreateChatGroupModalWindow = m_factoryChatGroupModalWindow( pGaeaClient, pInterface, pEngineDevice );
	GASSERT( apCreateChatGroupModalWindow.get() );
	return apCreateChatGroupModalWindow;
}

AP_IConftDisplayMan MyFactoryUI::CreateConftDisplayMan( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_IConftDisplayMan apCreateConftDisplayMan = m_factoryConftDisplayMan( pInterface, pEngineDevice );
    GASSERT( apCreateConftDisplayMan.get() );
    return apCreateConftDisplayMan;
}

AP_IQuestWindow MyFactoryUI::CreateQuestWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_IQuestWindow apCreateQuestWindow = m_factoryQuestWindow( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apCreateQuestWindow.get() );
    return apCreateQuestWindow;
}

AP_IQuestWindowRenewal MyFactoryUI::CreateQuestWindowRenewal( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_IQuestWindowRenewal apCreateQuestWindowRenewal = m_factoryQuestWindowRenewal( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apCreateQuestWindowRenewal.get() );
    return apCreateQuestWindowRenewal;
}

AP_IFriendWindow MyFactoryUI::CreateFriendWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_IFriendWindow apCreateFriendWindow = m_factoryFriendWindow( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apCreateFriendWindow.get() );
    return apCreateFriendWindow;
}

AP_IChatGroupInviteWindow MyFactoryUI::CreateChatGroupInviteWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	AP_IChatGroupInviteWindow apCreateChatGroupInviteWindow = m_factoryChatGroupInviteWindow( pGaeaClient, pInterface, pEngineDevice );
	GASSERT( apCreateChatGroupInviteWindow.get() );
	return apCreateChatGroupInviteWindow;
}

AP_IModalWindowInner MyFactoryUI::CreateModalWindow( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_IModalWindowInner apCreateModalWindow = m_factoryModalWindow( pInterface, pEngineDevice );
    GASSERT( apCreateModalWindow.get() );
    return apCreateModalWindow;
}

AP_IShopItemSearchWindow MyFactoryUI::CreateShopItemSearchWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_IShopItemSearchWindow apCreateShopItemSearchWindow = m_factoryShopItemSearchWindow( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apCreateShopItemSearchWindow.get() );
    return apCreateShopItemSearchWindow;
}

AP_IItemSearchResultWindow MyFactoryUI::CreateItemSearchResultWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_IItemSearchResultWindow apCreateItemSearchResultWindow = m_factoryItemSearchResultWindow( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apCreateItemSearchResultWindow.get() );
    return apCreateItemSearchResultWindow;
}

AP_IMapRequireCheck MyFactoryUI::CreateMapRequireCheck( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_IMapRequireCheck apCreateMapRequireCheck = m_factoryMapRequireCheck( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apCreateMapRequireCheck.get() );
    return apCreateMapRequireCheck;
}

AP_IPostBoxWindow MyFactoryUI::CreatePostBoxWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_IPostBoxWindow apCreatePostBoxWindow = m_factoryPostBoxWindow( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apCreatePostBoxWindow.get() );
    return apCreatePostBoxWindow;
}

AP_IPostNotifyButton MyFactoryUI::CreatePostNotifyButton( GLGaeaClient* pGaeaClient, CInnerInterface* pInerface, EngineDeviceMan* pEngineDevice )
{
    AP_IPostNotifyButton apCreatePostNotifyButton = m_factoryPostNotifyButton( pGaeaClient, pInerface, pEngineDevice );
    GASSERT( apCreatePostNotifyButton.get() );
    return apCreatePostNotifyButton;
}

AP_IQuestHelper MyFactoryUI::CreateQuestHelper( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_IQuestHelper apCreateQuestHelper = m_factoryQuestHelper( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apCreateQuestHelper.get() );
    return apCreateQuestHelper;
}

AP_ITutorialDialogue MyFactoryUI::CreateTutorialDialogue( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_ITutorialDialogue apCreateTutorialDialogue = m_factoryTutorialDialogue( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apCreateTutorialDialogue.get() );
    return apCreateTutorialDialogue;
}

AP_IBalloonText MyFactoryUI::CreateBalloonText( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_IBalloonText apCreateBalloonText = m_factoryBalloonText( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apCreateBalloonText.get() );
    return apCreateBalloonText;
}

AP_ISkillWindow MyFactoryUI::CreateSkillWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_ISkillWindow apCreateSkillWindow = m_factorySkillWindow( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apCreateSkillWindow.get() );
    return apCreateSkillWindow;
}

AP_IBasicInfoView MyFactoryUI::CreateBasicInfoView( GLGaeaClient* pGaeaClient, CInnerInterface* pInerface, EngineDeviceMan* pEngineDevice )
{
    AP_IBasicInfoView apCreateBasicInfoView = m_factoryBasicInfoView( pGaeaClient, pInerface, pEngineDevice );
    GASSERT( apCreateBasicInfoView.get() );
    return apCreateBasicInfoView;
}

AP_IExplainInterface MyFactoryUI::CreateExplainInterface( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_IExplainInterface apCreateExplainInterface = m_factoryExplainInterface( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apCreateExplainInterface.get() );
    return apCreateExplainInterface;
}

AP_IUILeftTopGroup MyFactoryUI::CreateUILeftTopGroup( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_IUILeftTopGroup apCreateUILeftTopGroup = m_factoryUILeftTopGroup( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apCreateUILeftTopGroup.get() );
    return apCreateUILeftTopGroup;
}

AP_ISkillTrayTab MyFactoryUI::CreateSkillTrayTab( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_ISkillTrayTab apCreateSkillTrayTab = m_factorySkillTrayTab( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apCreateSkillTrayTab.get() );
    return apCreateSkillTrayTab;
}

AP_IBasicQuickSkillSlot MyFactoryUI::CreateBasicQuickSkillSlot( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_IBasicQuickSkillSlot apCreateBasicQuickSkillSlot = m_factoryBasicQuickSkillSlot( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apCreateBasicQuickSkillSlot.get() );
    return apCreateBasicQuickSkillSlot;
}

AP_ITargetInfoDisplay MyFactoryUI::CreateTargetInfoDisplay( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_ITargetInfoDisplay apCreateTargetInfoDisplay = m_factoryTargetInfoDisplay( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apCreateTargetInfoDisplay.get() );
    return apCreateTargetInfoDisplay;
}

AP_ICursorTargetInfo MyFactoryUI::CreateCursorTargetInfo( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_ICursorTargetInfo apCreateCursorTargetInfo = m_factoryCursorTargetInfo( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apCreateCursorTargetInfo.get() );
    return apCreateCursorTargetInfo;
}

AP_IBalloonSpaceBar MyFactoryUI::CreateBalloonSpaceBar( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_IBalloonSpaceBar apCreateBalloonSpaceBar = m_factoryBalloonSpaceBar( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apCreateBalloonSpaceBar.get() );
    return apCreateBalloonSpaceBar;
}

AP_ITutorialViewStep MyFactoryUI::CreateTutorialViewStep( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_ITutorialViewStep apCreateTutorialViewStep = m_factoryTutorialViewStep( pInterface, pEngineDevice );
    GASSERT( apCreateTutorialViewStep.get() );
    return apCreateTutorialViewStep;
}

AP_ILunchBoxTimeDisplay MyFactoryUI::CreateLunchBoxTimeDisplay( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_ILunchBoxTimeDisplay apCreateLunchBoxTimeDisplay = m_factoryLunchBoxTimeDisplay( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apCreateLunchBoxTimeDisplay.get() );
    return apCreateLunchBoxTimeDisplay;
}

AP_ISystemBuffTimeDisplay MyFactoryUI::CreateSystemBuffTimeDisplay( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_ISystemBuffTimeDisplay apSystemBuffTimeDisplay = m_factorySystemBuffTimeDisplay( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apSystemBuffTimeDisplay.get() );
    return apSystemBuffTimeDisplay;
}

AP_IVehicleWindow MyFactoryUI::CreateVehicleWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_IVehicleWindow apCreateVehicleWindow = m_factoryVehicleWindow( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apCreateVehicleWindow.get() );
    return apCreateVehicleWindow;
}

AP_IProgressDisplay MyFactoryUI::CreateProgressDisplay( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_IProgressDisplay apCreateProgressDisplay = m_factoryProgressDisplay( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apCreateProgressDisplay.get() );
    return apCreateProgressDisplay;
}

AP_IStudentRecordWindow MyFactoryUI::CreateStudentRecordWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_IStudentRecordWindow apCreateStudentRecordWindow = m_factoryStudentRecordWindow( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apCreateStudentRecordWindow.get() );
    return apCreateStudentRecordWindow;
}

AP_IPointShopWindow MyFactoryUI::CreatePointShopWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_IPointShopWindow apCreatePointShopWindow = m_factoryPointShopWindow( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apCreatePointShopWindow.get() );
    return apCreatePointShopWindow;
}

AP_IRebirthWindow MyFactoryUI::CreateRebirthWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_IRebirthWindow apCreateRebirthWindow = m_factoryRebirthWindow( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apCreateRebirthWindow.get() );
    return apCreateRebirthWindow;
}

AP_IChatTypeList MyFactoryUI::CreateChatTypeList( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_IChatTypeList apCreateChatTypeList = m_factoryChatTypeList( pInterface, pEngineDevice );
    GASSERT( apCreateChatTypeList.get() );
    return apCreateChatTypeList;
}

AP_ICompetitionNotifyButton MyFactoryUI::CreateCompetitionNotifyButton( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_ICompetitionNotifyButton apCreateCompetitionNotifyButton = m_factoryCompetitionNotifyButton( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apCreateCompetitionNotifyButton.get() );
    return apCreateCompetitionNotifyButton;
}

AP_IPopupMenu MyFactoryUI::CreatePopupMenu( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_IPopupMenu apCreatePopupMenu = m_factoryPopupMenu( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apCreatePopupMenu.get() );
    return apCreatePopupMenu;
}

AP_IInfoCTFDisplay MyFactoryUI::CreateInfoCTFDisplay( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_IInfoCTFDisplay apCreateInfoCTFDisplay = m_factoryInfoCTFDisplay( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apCreateInfoCTFDisplay.get() );
    return apCreateInfoCTFDisplay;
}

AP_ICTFResultWindow MyFactoryUI::CreateCTFResultWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_ICTFResultWindow apCreateCTFResultWindow = m_factoryCTFResultWindow( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apCreateCTFResultWindow.get() );
    return apCreateCTFResultWindow;
}

AP_ICompetitionWindow MyFactoryUI::CreateCompetitionWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_ICompetitionWindow apCreateCompetitionWindow= m_factoryCompetitionWindow( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apCreateCompetitionWindow.get() );
    return apCreateCompetitionWindow;
}

AP_ICompetitionTopRanking MyFactoryUI::CreateCompetitionTopRanking( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_ICompetitionTopRanking apCreateCompetitionTopRanking= m_factoryCompetitionTopRanking( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apCreateCompetitionTopRanking.get() );
    return apCreateCompetitionTopRanking;
}

AP_IReservedSkillAnimation MyFactoryUI::CreateReservedSkillAnimation( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_IReservedSkillAnimation apCreateReservedSkillAnimation = m_factoryReservedSkillAnimation( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apCreateReservedSkillAnimation.get() );
    return apCreateReservedSkillAnimation;
}

AP_IProductButton MyFactoryUI::CreateProductButton( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_IProductButton apProductButton = m_factoryProductButton( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apProductButton.get() );
    return apProductButton;
}
AP_IProductWindow MyFactoryUI::CreateProductWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_IProductWindow apProductWIndow = m_factoryProductWindow( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apProductWIndow.get() );
    return apProductWIndow;
}
AP_IProductProgress MyFactoryUI::CreateProductProgress( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_IProductProgress apProductProgress = m_factoryProductProgress( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apProductProgress.get() );
    return apProductProgress;
}
AP_IProductResetWindow MyFactoryUI::CreateProductResetWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	AP_IProductResetWindow apProductResetWindow = m_factoryProductResetWindow( pGaeaClient, pInterface, pEngineDevice );
	GASSERT( apProductResetWindow.get() );
	return apProductResetWindow;
}

AP_IOddEvenWindow MyFactoryUI::CreateOddEvenWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	AP_IOddEvenWindow apOddEvenWindow = m_factoryOddEvenWindow( pGaeaClient, pInterface, pEngineDevice);
	GASSERT( apOddEvenWindow.get() );
	return apOddEvenWindow;
}

AP_ITexasHoldemWindow MyFactoryUI::CreateTexasHoldemWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	AP_ITexasHoldemWindow apTexasHoldemWindow = m_factoryTexasHoldemWindow( pGaeaClient, pInterface, pEngineDevice);
	GASSERT( apTexasHoldemWindow.get() );
	return apTexasHoldemWindow;
}

AP_ITexasHoldemWebWindow MyFactoryUI::CreateTexasHoldemWebWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	AP_ITexasHoldemWebWindow apTexasHoldemWebWindow = m_factoryTexasHoldemWebWindow( pGaeaClient, pInterface, pEngineDevice);
	GASSERT( apTexasHoldemWebWindow.get() );
	return apTexasHoldemWebWindow;
}

AP_IClubWindow MyFactoryUI::CreateClubWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_IClubWindow apClubWindow = m_factoryClubWindow( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apClubWindow.get() );
    return apClubWindow;
}

AP_IPrivateMarketSearchBuy MyFactoryUI::CreatePrivateMarketSearchBuy( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	AP_IPrivateMarketSearchBuy apPrivateMarketSearchBuy = m_factoryPrivateMarketSearchBuy( pGaeaClient, pInterface, pEngineDevice );
	GASSERT( apPrivateMarketSearchBuy.get() );
	return apPrivateMarketSearchBuy;
}

AP_IDPSWindow MyFactoryUI::CreateDPSWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	AP_IDPSWindow apCreateDPSWindow = m_factoryDPSWindow( pGaeaClient, pInterface, pEngineDevice );
	GASSERT( apCreateDPSWindow.get() );
	return apCreateDPSWindow;
}

AP_IPVEUserInterface MyFactoryUI::CreatePVEUserInterface( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	AP_IPVEUserInterface apCreatePVEUserInterface = m_factoryPVEUserInterface( pGaeaClient, pInterface, pEngineDevice );
	GASSERT( apCreatePVEUserInterface.get() );
	return apCreatePVEUserInterface;
}


AP_IPTournamentUserInterface MyFactoryUI::CreateTournamentUserInterface( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	AP_IPTournamentUserInterface apCreateTournamentUserInterface= m_factoryTournamentUserInterface( pGaeaClient, pInterface, pEngineDevice );
	GASSERT( apCreateTournamentUserInterface.get() );
	return apCreateTournamentUserInterface;
}

AP_IItemRebuildRenewal MyFactoryUI::CreateItemRebuildRenewal( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	AP_IItemRebuildRenewal apCreateItemRebuildRenewal = m_factoryItemRebuildRenewal( pGaeaClient, pInterface, pEngineDevice );
	GASSERT( apCreateItemRebuildRenewal.get() );
	return apCreateItemRebuildRenewal;
}

AP_IProgessTimer MyFactoryUI::CreateProgessTimer( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	AP_IProgessTimer apCreateProgessTimer = m_factoryProgessTimer( pGaeaClient, pInterface, pEngineDevice );
	GASSERT( apCreateProgessTimer.get() );
	return apCreateProgessTimer;
}

// Lotto System;
AP_ILottoMainUI MyFactoryUI::CreateLottoMainUI( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	AP_ILottoMainUI apLottoMainUI = m_factoryLottoMainUI( pInterface, pEngineDevice );
	GASSERT( apLottoMainUI.get() );
	return apLottoMainUI;
}

AP_ILottoBuyLottoUI MyFactoryUI::CreateLottoBuyLottoUI( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	AP_ILottoBuyLottoUI apLottoBuyLottoUI = m_factoryLottoBuyLottoUI( pInterface, pEngineDevice );
	GASSERT( apLottoBuyLottoUI.get() );
	return apLottoBuyLottoUI;
}

AP_ILottoBuyListInfo MyFactoryUI::CreateLottoBuyListInfo( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	AP_ILottoBuyListInfo apLottoBuyListInfo = m_factoryLottoBuyListInfo( pInterface, pEngineDevice );
	GASSERT( apLottoBuyListInfo.get() );
	return apLottoBuyListInfo;
}

AP_ILottoWinManListInfo MyFactoryUI::CreateLottoWinManListInfo( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	AP_ILottoWinManListInfo apLottoWinManListInfo = m_factoryLottoWinManListInfo( pInterface, pEngineDevice );
	GASSERT( apLottoWinManListInfo.get() );
	return apLottoWinManListInfo;
}

AP_ILottoDisplayUI MyFactoryUI::CreateLottoDisplayUI( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	AP_ILottoDisplayUI apLottoDisplayUI = m_factoryLottoDisplayUI( pInterface, pEngineDevice );
	GASSERT( apLottoDisplayUI.get() );
	return apLottoDisplayUI;
}

AP_ILottoNotifyButton MyFactoryUI::CreateLottoNotifyButton( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	AP_ILottoNotifyButton apLottoNotifyButton = m_factoryLottoNotifyButton( pInterface, pEngineDevice );
	GASSERT( apLottoNotifyButton.get() );
	return apLottoNotifyButton;
}

AP_ISimpleSystemMessageDisplay MyFactoryUI::CreateSimpleSystemMessageDisplay( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_ISimpleSystemMessageDisplay apCreateSimpleSystemMessageDisplay = m_factorySimpleSystemMessageDisplay( pGaeaClient, pInterface, pEngineDevice);
    GASSERT( apCreateSimpleSystemMessageDisplay.get() );
    return apCreateSimpleSystemMessageDisplay;
}

AP_ISimpleTimerMessageBox MyFactoryUI::CreateSimpleTimerMessageBox( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_ISimpleTimerMessageBox apCreateSimpleTimerMessageBox = m_factorySimpleTimerMessageBox( pGaeaClient, pInterface, pEngineDevice);
    GASSERT( apCreateSimpleTimerMessageBox.get() );
    return apCreateSimpleTimerMessageBox;
}

AP_IRnCDMProgress MyFactoryUI::CreateRnCDMProgress( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	AP_IRnCDMProgress apCreateRnCDMProgress = m_factoryRnCDMprogress( pGaeaClient, pInterface, pEngineDevice );
	GASSERT( apCreateRnCDMProgress.get() );
	return apCreateRnCDMProgress;
}

AP_IRnCDMRankWindow MyFactoryUI::CreateRnCDMRankWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	AP_IRnCDMRankWindow apCreateRnCDMRankWindow = m_factoryRnCDMRankWindow( pGaeaClient, pInterface, pEngineDevice );
	GASSERT( apCreateRnCDMRankWindow.get() );
	return apCreateRnCDMRankWindow;
}

AP_IRnCDMClubInfo MyFactoryUI::CreateRnCDMClubInfo( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	AP_IRnCDMClubInfo apCreateRnCDMClubInfo = m_factoryRnCDMClubInfo( pGaeaClient, pInterface, pEngineDevice );
	GASSERT( apCreateRnCDMClubInfo.get() );
	return apCreateRnCDMClubInfo;
}

AP_IColorProperty MyFactoryUI::CreateColorProperty( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	AP_IColorProperty apCreateColorProperty = m_factoryColorProperty( pGaeaClient, pInterface, pEngineDevice );
	GASSERT( apCreateColorProperty.get() );
	return apCreateColorProperty;
}

AP_IRnCompetitionWindow MyFactoryUI::CreateRnCompetitionWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	AP_IRnCompetitionWindow apCreateRnCompetitionWindow = m_factoryRnCompetitionWindow( pGaeaClient, pInterface, pEngineDevice );
	GASSERT( apCreateRnCompetitionWindow.get() );
	return apCreateRnCompetitionWindow;
}

AP_IRnCompetitionResultWindow MyFactoryUI::CreateRnCompetitionResultWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_IRnCompetitionResultWindow apCreateRnCompetitionResultWindow = m_factoryRnCompetitionResultWindow( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apCreateRnCompetitionResultWindow.get() );
    return apCreateRnCompetitionResultWindow;
}

AP_IRnCDMTopRanking MyFactoryUI::CreateRnCDMTopRanking( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	AP_IRnCDMTopRanking apCreateRnCDMTopRanking = m_factoryRnCDMTopRanking( pGaeaClient, pInterface, pEngineDevice );
	GASSERT( apCreateRnCDMTopRanking.get() );
	return apCreateRnCDMTopRanking;
}

AP_ICaptureTheFlagEffectMan MyFactoryUI::CreateCaptureTheFlagEffectMan( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_ICaptureTheFlagEffectMan apCreateCaptureTheFlagEffectMan = m_factoryCaptureTheFlagEffectMan( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apCreateCaptureTheFlagEffectMan.get() );
    return apCreateCaptureTheFlagEffectMan;
}

AP_IGuidanceAuthProgressUI MyFactoryUI::CreateGuidanceAuthProgressUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_IGuidanceAuthProgressUI apCreateGuidanceAuthProgressUI = m_factoryGuidanceAuthProgressUI( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apCreateGuidanceAuthProgressUI.get() );
    return apCreateGuidanceAuthProgressUI;
}

AP_IGuidanceProgressInfoUI MyFactoryUI::CreateGuidanceProgressInfoUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_IGuidanceProgressInfoUI apCreateGuidanceProgressInfoUI = m_factoryGuidanceProgressInfoUI( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apCreateGuidanceProgressInfoUI.get() );
    return apCreateGuidanceProgressInfoUI;
}

AP_IGuidanceRankInfoUI MyFactoryUI::CreateGuidanceRankInfoUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    AP_IGuidanceRankInfoUI apCreateGuidanceRankInfoUI = m_factoryGuidanceRankInfoUI( pGaeaClient, pInterface, pEngineDevice );
    GASSERT( apCreateGuidanceRankInfoUI.get() );
    return apCreateGuidanceRankInfoUI;
}

AP_ICTIAuthProgressUI MyFactoryUI::CreateCTIAuthProgressUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	AP_ICTIAuthProgressUI apCreateCTIAuthProgressUI = m_factoryCTIAuthProgressUI( pGaeaClient, pInterface, pEngineDevice );
	GASSERT( apCreateCTIAuthProgressUI.get() );
	return apCreateCTIAuthProgressUI;
}

AP_ICTIProgressInfoUI MyFactoryUI::CreateCTIProgressInfoUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	AP_ICTIProgressInfoUI apCreateCTIProgressInfoUI = m_factoryCTIProgressInfoUI( pGaeaClient, pInterface, pEngineDevice );
	GASSERT( apCreateCTIProgressInfoUI.get() );
	return apCreateCTIProgressInfoUI;
}

AP_ICTIRankInfoUI MyFactoryUI::CreateCTIRankInfoUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	AP_ICTIRankInfoUI apCreateCTIRankInfoUI = m_factoryCTIRankInfoUI( pGaeaClient, pInterface, pEngineDevice );
	GASSERT( apCreateCTIRankInfoUI.get() );
	return apCreateCTIRankInfoUI;
}

AP_IFlyCameraControlWindow MyFactoryUI::CreateFlyCameraControlUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	AP_IFlyCameraControlWindow apCreateFlyCameraControlWindow = m_factoryFlyCameraControlWindow( pGaeaClient, pInterface, pEngineDevice );
	GASSERT( apCreateFlyCameraControlWindow.get() );
	return apCreateFlyCameraControlWindow;
}

AP_ISetitemPreviewWindow MyFactoryUI::CreateSetitemPreviewUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	AP_ISetitemPreviewWindow apCreateSetitemPreviewWindow = m_factorySetitemPreviewWindow( pGaeaClient, pInterface, pEngineDevice );
	GASSERT( apCreateSetitemPreviewWindow.get() );
	return apCreateSetitemPreviewWindow;
}

AP_ITournamentWindowDashBoard MyFactoryUI::CreateTournamentDashBoardWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	AP_ITournamentWindowDashBoard apCreateRnCompetitionWindow= m_factoryTournamentWindowDashBoard( pGaeaClient, pInterface, pEngineDevice );
	GASSERT( apCreateRnCompetitionWindow.get() );
	return apCreateRnCompetitionWindow;
}

AP_ITournamentWindowUI MyFactoryUI::CreateTournamentWindowUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	AP_ITournamentWindowUI apCreateRnCompetitionWindow= m_factoryTournamentWindowUI( pGaeaClient, pInterface, pEngineDevice );
	GASSERT( apCreateRnCompetitionWindow.get() );
	return apCreateRnCompetitionWindow;
}

AP_ITournamentToToWindow MyFactoryUI::CreateTournamentToToWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	AP_ITournamentToToWindow apCreateRnCompetitionWindow= m_factoryTournamentToToWindow( pGaeaClient, pInterface, pEngineDevice );
	GASSERT( apCreateRnCompetitionWindow.get() );
	return apCreateRnCompetitionWindow;
}

AP_ITournamentMoreInfoWindow MyFactoryUI::CreateTournamentMoreInfoWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	AP_ITournamentMoreInfoWindow apCreateRnCompetitionWindow= m_factoryTournamentMoreInfoWindow( pGaeaClient, pInterface, pEngineDevice );
	GASSERT( apCreateRnCompetitionWindow.get() );
	return apCreateRnCompetitionWindow;
}

AP_ISummonStateDisplayUI MyFactoryUI::CreateSummonStateDisplayUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	AP_ISummonStateDisplayUI apCreateRnSummonStateDisplayUI = m_factorySummonStateDisplayUI( pGaeaClient, pInterface, pEngineDevice );
	GASSERT( apCreateRnSummonStateDisplayUI.get() );
	return apCreateRnSummonStateDisplayUI;
}

AP_IFactionInfoWindow MyFactoryUI::CreateFactionInfoWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	AP_IFactionInfoWindow apCreateFactionInfoWindow = m_factoryFactionInfoWindow( pGaeaClient, pInterface, pEngineDevice );
	GASSERT( apCreateFactionInfoWindow.get() );
	return apCreateFactionInfoWindow;
}

AP_IWorldBattleButton MyFactoryUI::CreateWorldBattleButton( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	AP_IWorldBattleButton apCreateWorldBattleButton = m_factoryWorldBattleButton( pGaeaClient, pInterface, pEngineDevice );
	GASSERT( apCreateWorldBattleButton.get() );
	return apCreateWorldBattleButton;
}

AP_IMiniMap fnPtrOldFactoryMinimap( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_IMiniMap( new MyMiniMap( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IKeySettingWindow fnPtrOldFactoryKeySettingWindow( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_IKeySettingWindow( new MyKeySettingWindow( pInterface, pEngineDevice ) );
}

AP_IChatMacroWindow fnPtrOldFactoryChatMacroWindow( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_IChatMacroWindow( new MyChatMacroWindow( pInterface, pEngineDevice ) );
}

AP_INameDisplayMan fnPtrOldFactoryNameDisplayMan( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_INameDisplayMan( new MyNameDisplayMan( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IPrivateMarketShowMan fnPtrOldFactoryPrivateMarketShowMan( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_IPrivateMarketShowMan( new MyPrivateMarketShowMan(pGaeaClient, pInterface, pEngineDevice ) );
}

AP_ISummonNameDisplayMan fnPtrOldFactorySummonNameDisplayMan( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_ISummonNameDisplayMan( new MySummonNameDisplayMan( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_ISkillWindowToTray fnPtrOldFactorySkillWindowToTray( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_ISkillWindowToTray( new MySkillWindowToTray(pGaeaClient, pInterface, pEngineDevice) );
}

AP_IPartyWindow fnPtrOldFactoryPartyWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_IPartyWindow( new MyPartyWindow( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IPartyWindowRenewal fnPtrOldFactoryPartyWindowRenewal( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{	
	return AP_IPartyWindowRenewal( new MyPartyWindowRenewal( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IPartyYesNoWindow fnPtrOldFactoryPartyYesNoWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{	
	return AP_IPartyYesNoWindow( new MyPartyYesNoWindow( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IExpeditionInviteWindow fnPtrOldFactoryExpeditionInviteWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{	
	return AP_IExpeditionInviteWindow( new MyExpeditionInviteWindow( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IExpeditionCheckReadyWindow fnPtrOldFactoryExpeditionCheckReadyWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{	
	return AP_IExpeditionCheckReadyWindow( new MyExpeditionCheckReadyWindow( pGaeaClient, pInterface, pEngineDevice ) );
}


AP_IPartyStateWindow fnPtrOldFactoryPartyStateWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{	
	return AP_IPartyStateWindow( new MyPartyStateWindow( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IPartyTenderWindow fnPtrOldFactoryPartyTenderWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{	
	return AP_IPartyTenderWindow( new MyPartyTenderWindow( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IPartyTenderNotifyButton fnPtrOldFactoryPartyTenderNotifyButton( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{	
	return AP_IPartyTenderNotifyButton( new MyPartyTenderNotifyButton( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IPartyDistributionWindow fnPtrOldFactoryPartyDistributionWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{	
	return AP_IPartyDistributionWindow( new MyPartyDistributionWindow( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IPartyDistributionListWindow fnPtrOldFactoryPartyDistributionListWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{	
	return AP_IPartyDistributionListWindow( new MyPartyDistributionListWindow( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IMiniExpeditionWindow fnPtrOldFactoryMiniExpeditionWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	return AP_IMiniExpeditionWindow( new MyMiniExpeditionWindow( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IBasicChatWindow fnPtrOldFactoryBasicChatWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_IBasicChatWindow( new MyBasicChat( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IChatOptionWindow fnPtrOldFactoryChatOptionWindow( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_IChatOptionWindow( new MyChatOptionWindow( pInterface, pEngineDevice ) );
}

AP_ITradeWindow fnPtrOldFactoryTradeWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_ITradeWindow( new MyTradeWindow( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_ITradeInventoryWindow fnPtrOldFactoryTradeInventoryWindow( GLGaeaClient* pGaeClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_ITradeInventoryWindow( new MyTradeInventoryWindow( pGaeClient, pInterface, pEngineDevice ) );
}

AP_IPtoPWindow fnPtrOldFactoryPtoPWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_IPtoPWindow( new MyPtoPWindow( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IInventoryWindow fnPtrOldFactoryInventoryWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_IInventoryWindow( new MyInventoryWindow( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IItemMove fnPtrOldFactoryItemMove( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_IItemMove( new MyItemMove( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IMarketWindow fnPtrOldFactoryMarketWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_IMarketWindow( new MyMarketWindow( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IHeadChatDisplayMan fnPtrOldFactoryHeadChatDisplayMan( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_IHeadChatDisplayMan( new MyHeadChatDisplayMan( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IConftModalWindow fnPtrOldFactoryConftModalWindow( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_IConftModalWindow( new MyConftModalWindow( pInterface, pEngineDevice ) );
}

AP_IPartyModalWindow fnPtrOldFactoryPartyModalWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_IPartyModalWindow( new MyPartyModalWindow( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IChatGroupModalWindow fnPtrOldFactoryChatGroupModalWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_IChatGroupModalWindow( new MyChatGroupModalWindow( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IConftDisplayMan fnPtrOldFactoryConftDisplayMan( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_IConftDisplayMan( new MyConftDisplayMan( pInterface, pEngineDevice ) );
}

AP_IQuestWindow fnPtrOldFactoryQuestWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_IQuestWindow( new MyQuestWindow( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IQuestWindowRenewal fnPtrOldFactoryQuestWindowRenewal( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_IQuestWindowRenewal( new MyQuestWindowRenewal( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IFriendWindow fnPtrOldFactoryFriendWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    if( GLUseFeatures::GetInstance().IsUsingRenewFriendWindow() )
        return AP_IFriendWindow( new MyRnFriendWindow( pGaeaClient, pInterface, pEngineDevice ) );
    else
        return AP_IFriendWindow( new MyFriendWindow( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IChatGroupInviteWindow fnPtrOldFacotryChatGroupInviteWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	return AP_IChatGroupInviteWindow( new MyChatGroupInviteWindow( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IModalWindowInner fnPtrOldFactoryModalWindow( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_IModalWindowInner( new MyModalWindow( pInterface, pEngineDevice ) );
}

AP_IShopItemSearchWindow fnPtrOldFactoryShopItemSearchWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_IShopItemSearchWindow( new MyShopItemSearchWindow( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IItemSearchResultWindow fnPtrOldFactoryItemSearchResultWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_IItemSearchResultWindow( new MyItemSearchResultWindow( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IMapRequireCheck fnPtrOldFactoryMapRequireCheck( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_IMapRequireCheck( new MyMapRequireCheck( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IPostBoxWindow fnPtrOldFactoryPostBoxWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInerterface, EngineDeviceMan* pEngineDevice )
{
    return AP_IPostBoxWindow( new MyPostBoxWindow( pGaeaClient, pInerterface, pEngineDevice ) );
}

AP_IPostNotifyButton fnPtrOldFactoryPostNotifyButton( GLGaeaClient* pGaeaClient, CInnerInterface* pInerface, EngineDeviceMan* pEngineDevice )
{
    return AP_IPostNotifyButton( new MyPostNotifyButton( pGaeaClient, pInerface, pEngineDevice ) );
}

AP_IQuestHelper fnPtrOldFactoryQuestHelper( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_IQuestHelper( new MyQuestHelper( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_ITutorialDialogue fnPtrOldFactoryTutorialDialogue( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_ITutorialDialogue( new MyTutorialDialogue( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IBalloonText fnPtrOldFactoryBalloonText( GLGaeaClient* pGaeaClient, CInnerInterface* pInerface, EngineDeviceMan* pEngineDevice )
{
    return AP_IBalloonText( new MyBalloonText( pGaeaClient, pInerface, pEngineDevice ) );
}

AP_ISkillWindow fnPtrOldFactorySkillWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInerface, EngineDeviceMan* pEngineDevice )
{
    if( GLUseFeatures::GetInstance().IsUsingRenewSkillWindow() )
        return AP_ISkillWindow( new MyRnSkillWindow( pGaeaClient, pInerface, pEngineDevice ) );
    else
        return AP_ISkillWindow( new MySkillWindow( pGaeaClient, pInerface, pEngineDevice ) );
}

AP_IBasicInfoView fnPtrOldFactoryBasicInfoView( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_IBasicInfoView( new MyBasicInfoView( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IExplainInterface fnPtrOldFactoryExplainInterface( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_IExplainInterface( new MyExplainInterface( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IUILeftTopGroup fnPtrOldFactoryUILeftTopGroup( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_IUILeftTopGroup( new MyUILeftTopGroup( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_ISkillTrayTab fnPtrOldFactorySkillTrayTab( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_ISkillTrayTab( new MySkillTrayTab( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IBasicQuickSkillSlot fnPtrOldFactoryBasicQuickSkillSlot( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_IBasicQuickSkillSlot( new MyBasicQuickSkillSlot( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_ITargetInfoDisplay fnPtrOldFactoryTargetInfoDisplay( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_ITargetInfoDisplay( new MyTargetInfoDisplay( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_ICursorTargetInfo fnPtrOldFactoryCursorTargetInfo( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_ICursorTargetInfo( new MyCursorTargetInfo( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IBalloonSpaceBar fnPtrOldFactoryBalloonSpaceBar( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_IBalloonSpaceBar( new MyBalloonSpaceBar( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_ITutorialViewStep fnPtrOldFactoryTutorialViewStep( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_ITutorialViewStep( new MyTutorialViewStep( pInterface, pEngineDevice ) );
}

AP_ILunchBoxTimeDisplay fnPtrOldFactoryLunchBoxTimeDisplay( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_ILunchBoxTimeDisplay( new MyLunchBoxTimeDisplay( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_ISystemBuffTimeDisplay fnPtrOldFactorySystemBuffTimeDisplay( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_ISystemBuffTimeDisplay( new MySystemBuffTimeDisplay( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IVehicleWindow fnPtrOldFactoryVehicleWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_IVehicleWindow( new MyVehicleWindow( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IProgressDisplay fnPtrOldFactoryProgressDisplay( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_IProgressDisplay( new MyProgressDisplay( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IStudentRecordWindow fnPtrOldFactoryStudentRecordWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_IStudentRecordWindow( new MyStudentRecordWindow( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IPointShopWindow fnPtrOldFactoryPointShopWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_IPointShopWindow( new MyPointShopWindow( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IRebirthWindow fnPtrOldFactoryRebirthWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_IRebirthWindow( new MyRebirthWindow( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IChatTypeList fnPtrOldFactoryChatTypeList( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_IChatTypeList( new MyChatTypeList( pInterface, pEngineDevice ) );
}

AP_ICompetitionNotifyButton fnPtrOldFactoryCompetitionNotifyButton( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_ICompetitionNotifyButton( new MyCompetitionNotifyButton( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IPopupMenu fnPtrOldFactoryPopupMenu( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_IPopupMenu( new MyPopupMenu( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IInfoCTFDisplay fnPtrOldFactoryInfoCTFDisplay( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_IInfoCTFDisplay( new MyInfoCTFDisplay( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_ICTFResultWindow fnPtrOldFactoryCTFResultWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_ICTFResultWindow( new MyCTFResultWindow( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_ICompetitionWindow fnPtrOldFactoryCompetitionWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_ICompetitionWindow( new MyCompetitionWindow( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_ICompetitionTopRanking fnPtrOldFactoryCompetitionTopRanking( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_ICompetitionTopRanking( new MyCompetitionTopRanking( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IReservedSkillAnimation fnPtrOldFactoryReservedSkillAnimation( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_IReservedSkillAnimation( new MyReservedSkillAnimation( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IProductButton fnPtrOldFactoryProductButton( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_IProductButton( new MyProductButton( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IProductWindow fnPtrOldFactoryProductWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_IProductWindow( new MyProductWindow( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IProductProgress fnPtrOldFactoryProductProgress( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_IProductProgress( new MyProductProgress( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IProductResetWindow fnPtrOldFactoryProductResetWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	return AP_IProductResetWindow( new MyProductResetWindow( pGaeaClient, pInterface, pEngineDevice ) );
}//chjeon

AP_IOddEvenWindow fnPtrOldFactoryOddEvenWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	return AP_IOddEvenWindow( new OddEvenWindow( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_ITexasHoldemWindow fnPtrOldFactoryTexasHoldemWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	return AP_ITexasHoldemWindow( new TexasHoldem::TexasHoldemWindow( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_ITexasHoldemWebWindow fnPtrOldFactoryTexasHoldemWebWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	return AP_ITexasHoldemWebWindow( new TexasHoldem::TexasHoldemWebWindow( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IClubWindow fnPtrOldFactoryClubWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    if( GLUseFeatures::GetInstance().IsUsingWorldBattleClub() )
        return AP_IClubWindow( new MyWbClubWindow( pGaeaClient, pInterface, pEngineDevice ) );
    else
        return AP_IClubWindow( new MyClubWindow( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IPrivateMarketSearchBuy fnPtrOldFactoryPrivateMarketSearchBuy( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	if( GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
		return AP_IPrivateMarketSearchBuy( new MyPrivateMarketSearchBuy( pGaeaClient, pInterface, pEngineDevice ) );
	else
		return AP_IPrivateMarketSearchBuy( new MyPrivateMarketSearchBuy( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IDPSWindow fnPtrOldFactoryDPSWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	return AP_IDPSWindow( new DPSWindow( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IPVEUserInterface fnPtrOldFactoryPVEUserInterface( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	return AP_IPVEUserInterface( new MyPVEUserInterface( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IPTournamentUserInterface fnPtrOldFactoryTournamentUserInterface( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	return AP_IPTournamentUserInterface( new MyTournamentUserInterface( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IItemRebuildRenewal fnPtrOldFactoryItemRebuildRenewal( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	return AP_IItemRebuildRenewal( new ItemRebuildRenewal( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IProgessTimer fnPtrOldFactoryProgessTimer( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	return AP_IProgessTimer( new MyProgessTimer( pGaeaClient, pInterface, pEngineDevice ) );
}

// Lotto System;
AP_ILottoMainUI fnPtrLottoMainUI( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	return AP_ILottoMainUI( new MyLottoMainUI( pInterface, pEngineDevice ) );
}

AP_ILottoBuyLottoUI fnPtrLottoBuyLottoUI( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	return AP_ILottoBuyLottoUI( new MyLottoBuyLottoUI( pInterface, pEngineDevice ) );
}

AP_ILottoBuyListInfo fnPtrLottoBuyListInfo( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	return AP_ILottoBuyListInfo( new MyLottoBuyListInfo( pInterface, pEngineDevice ) );
}

AP_ILottoWinManListInfo fnPtrLottoWinManListInfo( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	return AP_ILottoWinManListInfo( new MyLottoWinManListInfo( pInterface, pEngineDevice ) );
}

AP_ILottoDisplayUI fnPtrLottoDisplayUI( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	return AP_ILottoDisplayUI( new MyLottoDisplayUI( pInterface, pEngineDevice ) );
}

AP_ILottoNotifyButton fnPtrLottoNotifyButton( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	return AP_ILottoNotifyButton( new MyLottoNotifyButton( pInterface, pEngineDevice ) );
}

AP_IColorProperty fnptroldFactoryColorProperty( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_IColorProperty( new MiniGame::ColorProperty( pGaeaClient, pInterface, pEngineDevice ) );
}

// New Instance System
AP_ISimpleSystemMessageDisplay fnptroldFactorySimpleSystemMessageDisplay( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_ISimpleSystemMessageDisplay( new MySimpleSystemMessageDisplay(pGaeaClient, pInterface, pEngineDevice) );
}

AP_ISimpleTimerMessageBox fnptroldFactorySimpleTimerMessageBox( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_ISimpleTimerMessageBox( new MySimpleTimerMessageBox(pGaeaClient, pInterface, pEngineDevice) );
}

AP_IRnCDMProgress fnptroldFactoryRnCDMProgress( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	return AP_IRnCDMProgress( new MyRnCDMProgress( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IRnCDMRankWindow fnptroldFactoryRnCDMRankWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	return AP_IRnCDMRankWindow( new MyRnCDMRankWindow( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IRnCDMClubInfo fnptroldFactoryRnCDMClubInfo( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	return AP_IRnCDMClubInfo( new MyRnCDMClubInfo( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IRnCompetitionWindow fnPtrOldFactoryRnCompetitionWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	return AP_IRnCompetitionWindow( new MyRnCompetitionWindow( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IRnCompetitionResultWindow fnPtrOldFactoryRnCompetitionResultWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_IRnCompetitionResultWindow( new MyRnCompetitionResultWindow( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IRnCDMTopRanking fnPtroldFactoryRnCDMTopRanking( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	return AP_IRnCDMTopRanking( new MyRnCDMTopRanking( pGaeaClient, pInterface, pEngineDevice ) );
}
AP_ITournamentWindowDashBoard fnptroldFactoryTournamentWindowDashBoard( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	return AP_ITournamentWindowDashBoard( new MyTournamentDashBoard( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_ICaptureTheFlagEffectMan fnPtrCaptureTheFlagEffectMan( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_ICaptureTheFlagEffectMan( new MyCaptureTheFlagEffectMan( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IGuidanceAuthProgressUI fnPtrGuidanceAuthProgressUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_IGuidanceAuthProgressUI( new MyGuidanceAuthProgressUI( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IGuidanceProgressInfoUI fnPtrGuidanceProgressInfoUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_IGuidanceProgressInfoUI( new MyGuidanceProgressInfoUI( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IGuidanceRankInfoUI fnPtrGuidanceRankInfoUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_IGuidanceRankInfoUI( new MyGuidanceRankInfoUI( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_ICTIAuthProgressUI fnPtrCTIAuthProgressUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	return AP_ICTIAuthProgressUI( new MyCTIAuthProgressUI( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_ICTIProgressInfoUI fnPtrCTIProgressInfoUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	return AP_ICTIProgressInfoUI( new MyCTIProgressInfoUI( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_ICTIRankInfoUI fnPtrCTIRankInfoUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	return AP_ICTIRankInfoUI( new MyCTIRankInfoUI( pGaeaClient, pInterface, pEngineDevice ) );
}



AP_ITournamentWindowUI fnptroldFactoryTournamentWindowUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_ITournamentWindowUI( new MyTournamentUI( pGaeaClient, pInterface, pEngineDevice ) );
}


AP_ITournamentToToWindow fnptroldFactoryTournamentToToWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_ITournamentToToWindow( new MyTournamentToToWindow( pGaeaClient, pInterface, pEngineDevice ) );
}


AP_ITournamentMoreInfoWindow fnptroldFactoryTournamentMoreInfoWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
    return AP_ITournamentMoreInfoWindow( new MyTournamentMoreInfoWindow( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IFlyCameraControlWindow fnptroldFactoryFlyCameraControlWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	return AP_IFlyCameraControlWindow( new FlyCameraControlWindow(pGaeaClient, pInterface, pEngineDevice) );
}

AP_ISetitemPreviewWindow fnptroldFactorySetitemPreviewWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	return AP_ISetitemPreviewWindow( new CSetitemPreviewWindow(pGaeaClient, pInterface, pEngineDevice) );
}

AP_ISummonStateDisplayUI fnptrSummonStateDisplayUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	return AP_ISummonStateDisplayUI( new MySummonStateDisplayUI( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IFactionInfoWindow fnptrFactionInfoWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	return AP_IFactionInfoWindow( new MyFactionInfoWindow( pGaeaClient, pInterface, pEngineDevice ) );
}

AP_IWorldBattleButton fnptrWorldBattleButton( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
{
	return AP_IWorldBattleButton( new MyWorldBattleButton( pGaeaClient, pInterface, pEngineDevice ) );
}