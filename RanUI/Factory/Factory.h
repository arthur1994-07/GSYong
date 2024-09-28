#pragma once

#include "boost/shared_ptr.hpp"
#include "boost/enable_shared_from_this.hpp"

//---------------------------------------------[ define factory declare ]
#define DECLARE_UI_AP( var )                \
class var;                              \
    typedef std::auto_ptr<var> AP_##var;
//------------------------------------------------------------------------

class EngineDeviceMan;
class CInnerInterface;
class GLGaeaClient;

// Lotto System
DECLARE_UI_AP( ILottoMainUI )
DECLARE_UI_AP( ILottoBuyLottoUI )
DECLARE_UI_AP( ILottoBuyListInfo )
DECLARE_UI_AP( ILottoWinManListInfo )
DECLARE_UI_AP( ILottoDisplayUI )
DECLARE_UI_AP( ILottoNotifyButton )

DECLARE_UI_AP( IMiniMap )
DECLARE_UI_AP( IDPSWindow )
DECLARE_UI_AP( IKeySettingWindow )
DECLARE_UI_AP( IChatMacroWindow )
DECLARE_UI_AP( INameDisplayMan )
DECLARE_UI_AP( IPrivateMarketShowMan )
DECLARE_UI_AP( ISummonNameDisplayMan )
DECLARE_UI_AP( ISkillWindowToTray )
DECLARE_UI_AP( IPartyWindow )
DECLARE_UI_AP( IPartyWindowRenewal )
DECLARE_UI_AP( IPartyYesNoWindow )
DECLARE_UI_AP( IExpeditionInviteWindow )
DECLARE_UI_AP( IExpeditionCheckReadyWindow )
DECLARE_UI_AP( IPartyStateWindow )
DECLARE_UI_AP( IPartyTenderWindow )
DECLARE_UI_AP( IPartyTenderNotifyButton )
DECLARE_UI_AP( IPartyDistributionWindow )
DECLARE_UI_AP( IPartyDistributionListWindow )
DECLARE_UI_AP( IMiniExpeditionWindow )
DECLARE_UI_AP( IBasicChatWindow )
DECLARE_UI_AP( IChatOptionWindow )
DECLARE_UI_AP( ITradeWindow )
DECLARE_UI_AP( ITradeInventoryWindow )
DECLARE_UI_AP( IPtoPWindow )
DECLARE_UI_AP( IInventoryWindow )
DECLARE_UI_AP( IItemMove )
DECLARE_UI_AP( IMarketWindow )
DECLARE_UI_AP( IHeadChatDisplayMan )
DECLARE_UI_AP( IConftModalWindow )
DECLARE_UI_AP( IPartyModalWindow )
DECLARE_UI_AP( IChatGroupModalWindow )
DECLARE_UI_AP( IConftDisplayMan )
DECLARE_UI_AP( IQuestWindow )
DECLARE_UI_AP( IQuestWindowRenewal )
DECLARE_UI_AP( IFriendWindow )
DECLARE_UI_AP( IChatGroupInviteWindow )
DECLARE_UI_AP( IModalWindowInner )
DECLARE_UI_AP( IShopItemSearchWindow )
DECLARE_UI_AP( IItemSearchResultWindow )
DECLARE_UI_AP( IMapRequireCheck )
DECLARE_UI_AP( IPostBoxWindow )
DECLARE_UI_AP( IPostNotifyButton )
DECLARE_UI_AP( IQuestHelper )
DECLARE_UI_AP( ITutorialDialogue )
DECLARE_UI_AP( IBalloonText )
DECLARE_UI_AP( ISkillWindow )
DECLARE_UI_AP( IBasicInfoView )
DECLARE_UI_AP( IExplainInterface )
DECLARE_UI_AP( IUILeftTopGroup )
DECLARE_UI_AP( ISkillTrayTab )
DECLARE_UI_AP( IBasicQuickSkillSlot )
DECLARE_UI_AP( ITargetInfoDisplay )
DECLARE_UI_AP( ICursorTargetInfo )
DECLARE_UI_AP( IBalloonSpaceBar )
DECLARE_UI_AP( ITutorialViewStep )
DECLARE_UI_AP( ILunchBoxTimeDisplay )
DECLARE_UI_AP( ISystemBuffTimeDisplay )
DECLARE_UI_AP( IVehicleWindow )
DECLARE_UI_AP( IFaceBookWindow )
DECLARE_UI_AP( IProgressDisplay )
DECLARE_UI_AP( IStudentRecordWindow )
DECLARE_UI_AP( IPointShopWindow )
DECLARE_UI_AP( IRebirthWindow )
DECLARE_UI_AP( IChatTypeList )
DECLARE_UI_AP( ICompetitionNotifyButton )
DECLARE_UI_AP( IPopupMenu )
DECLARE_UI_AP( IInfoCTFDisplay )
DECLARE_UI_AP( ICTFResultWindow )
DECLARE_UI_AP( ICompetitionWindow )
DECLARE_UI_AP( ICompetitionTopRanking )
DECLARE_UI_AP( IReservedSkillAnimation )
DECLARE_UI_AP( IProductButton )
DECLARE_UI_AP( IProductWindow )
DECLARE_UI_AP( IProductProgress )
DECLARE_UI_AP( IProductResetWindow )
DECLARE_UI_AP( IOddEvenWindow )
DECLARE_UI_AP( ITexasHoldemWindow )
DECLARE_UI_AP( ITexasHoldemWebWindow )
DECLARE_UI_AP( IClubWindow )
DECLARE_UI_AP( IPrivateMarketSearchBuy )
DECLARE_UI_AP( IPVEUserInterface )
DECLARE_UI_AP( IPTournamentUserInterface)
DECLARE_UI_AP( IItemRebuildRenewal )
DECLARE_UI_AP( IColorProperty )
DECLARE_UI_AP( IProgessTimer )

//New Instance System
DECLARE_UI_AP( ISimpleSystemMessageDisplay )
DECLARE_UI_AP( ISimpleTimerMessageBox )
DECLARE_UI_AP( IRnCDMProgress )
DECLARE_UI_AP( IRnCDMRankWindow )
DECLARE_UI_AP( IRnCDMClubInfo )
DECLARE_UI_AP( IRnCompetitionWindow )
DECLARE_UI_AP( IRnCompetitionResultWindow )
DECLARE_UI_AP( IRnCDMTopRanking )
DECLARE_UI_AP( ITournamentWindowDashBoard )
DECLARE_UI_AP( ITournamentWindowUI )
DECLARE_UI_AP( ITournamentToToWindow )
DECLARE_UI_AP( ITournamentMoreInfoWindow )
DECLARE_UI_AP( ICaptureTheFlagEffectMan )
DECLARE_UI_AP( IGuidanceAuthProgressUI)
DECLARE_UI_AP( IGuidanceProgressInfoUI)
DECLARE_UI_AP( IGuidanceRankInfoUI)
DECLARE_UI_AP( ICTIAuthProgressUI)
DECLARE_UI_AP( ICTIProgressInfoUI)
DECLARE_UI_AP( ICTIRankInfoUI)

DECLARE_UI_AP( IFlyCameraControlWindow )
DECLARE_UI_AP( ISetitemPreviewWindow )

DECLARE_UI_AP( ISummonStateDisplayUI )
DECLARE_UI_AP( IFactionInfoWindow )
DECLARE_UI_AP( IWorldBattleButton )

// Lotto System
typedef AP_ILottoMainUI (*factoryLottoMainUI)( CInnerInterface*, EngineDeviceMan* );
typedef AP_ILottoBuyLottoUI (*factoryLottoBuyLottoUI)( CInnerInterface*, EngineDeviceMan* );
typedef AP_ILottoBuyListInfo (*factoryLottoBuyListInfo)( CInnerInterface*, EngineDeviceMan* );
typedef AP_ILottoWinManListInfo (*factoryLottoWinManListInfo)( CInnerInterface*, EngineDeviceMan* );
typedef AP_ILottoDisplayUI (*factoryLottoDisplayUI)( CInnerInterface*, EngineDeviceMan* );
typedef AP_ILottoNotifyButton (*factoryLottoNotifyButton)( CInnerInterface*, EngineDeviceMan* );

typedef AP_IMiniMap (*factoryMiniMap)( GLGaeaClient* , CInnerInterface*, EngineDeviceMan* );
typedef AP_IDPSWindow (*factoryDPSWindow)( GLGaeaClient* , CInnerInterface*, EngineDeviceMan* );
typedef AP_IKeySettingWindow (*factoryKeySettingWindow)( CInnerInterface*, EngineDeviceMan*  );
typedef AP_IChatMacroWindow (*factoryChatMacroWindow)( CInnerInterface*, EngineDeviceMan*  );
typedef AP_INameDisplayMan (*factoryNameDisplayMan)( GLGaeaClient* , CInnerInterface*, EngineDeviceMan*  );
typedef AP_IPrivateMarketShowMan (*factoryPrivateMarketShowMan)( GLGaeaClient* , CInnerInterface*, EngineDeviceMan*  );
typedef AP_ISummonNameDisplayMan (*factorySummonNameDisplayMan)( GLGaeaClient* , CInnerInterface*, EngineDeviceMan*  );
typedef AP_ISkillWindowToTray (*factorySkillWindowToTray)( GLGaeaClient* , CInnerInterface*, EngineDeviceMan*  );
typedef AP_IPartyWindow (*factoryPartyWindow)( GLGaeaClient* , CInnerInterface*, EngineDeviceMan*  );
typedef AP_IPartyWindowRenewal (*factoryPartyWindowRenewal)( GLGaeaClient* , CInnerInterface*, EngineDeviceMan*  );
typedef AP_IPartyYesNoWindow (*factoryPartyYesNoWindow)( GLGaeaClient* , CInnerInterface*, EngineDeviceMan*  );
typedef AP_IExpeditionInviteWindow (*factoryExpeditionInviteWindow)( GLGaeaClient* , CInnerInterface*, EngineDeviceMan*  );
typedef AP_IExpeditionCheckReadyWindow (*factoryExpeditionCheckReadyWindow)( GLGaeaClient* , CInnerInterface*, EngineDeviceMan*  );
typedef AP_IPartyStateWindow (*factoryPartyStateWindow)( GLGaeaClient* , CInnerInterface*, EngineDeviceMan*  );
typedef AP_IPartyTenderWindow (*factoryPartyTenderWindow)( GLGaeaClient* , CInnerInterface*, EngineDeviceMan*  );
typedef AP_IPartyTenderNotifyButton (*factoryPartyTenderNotifyButton)( GLGaeaClient* , CInnerInterface*, EngineDeviceMan*  );
typedef AP_IPartyDistributionWindow (*factoryPartyDistributionWindow)( GLGaeaClient* , CInnerInterface*, EngineDeviceMan*  );
typedef AP_IPartyDistributionListWindow (*factoryPartyDistributionListWindow)( GLGaeaClient* , CInnerInterface*, EngineDeviceMan*  );
typedef AP_IMiniExpeditionWindow (*factoryMiniExpeditionWindow)( GLGaeaClient* , CInnerInterface*, EngineDeviceMan*  );
typedef AP_IBasicChatWindow (*factoryBasicChat)( GLGaeaClient* , CInnerInterface*, EngineDeviceMan*  );
typedef AP_IChatOptionWindow (*factoryChatOptionWindow)( CInnerInterface*, EngineDeviceMan* );
typedef AP_ITradeWindow (*factoryTradeWindow)( GLGaeaClient* , CInnerInterface*, EngineDeviceMan* );
typedef AP_ITradeInventoryWindow (*factoryTradeInventoryWindow)( GLGaeaClient* , CInnerInterface*, EngineDeviceMan* );
typedef AP_IPtoPWindow (*factoryPtoPWindow)( GLGaeaClient* , CInnerInterface*, EngineDeviceMan* );
typedef AP_IInventoryWindow (*factoryInventoryWindow)( GLGaeaClient* , CInnerInterface*, EngineDeviceMan* );
typedef AP_IItemMove (*factoryItemMove)( GLGaeaClient* , CInnerInterface*, EngineDeviceMan* );
typedef AP_IMarketWindow (*factoryMarketWindow)( GLGaeaClient* , CInnerInterface*, EngineDeviceMan* );
typedef AP_IHeadChatDisplayMan (*factoryHeadChatDisplayMan)( GLGaeaClient* , CInnerInterface*, EngineDeviceMan* );
typedef AP_IConftModalWindow (*factoryConftModalWindow)( CInnerInterface*, EngineDeviceMan* );
typedef AP_IPartyModalWindow (*factoryPartyModalWindow)( GLGaeaClient* , CInnerInterface*, EngineDeviceMan* );
typedef AP_IChatGroupModalWindow (*factoryChatGroupModalWindow)( GLGaeaClient* , CInnerInterface*, EngineDeviceMan* );
typedef AP_IConftDisplayMan (*factoryConftDisplayMan)( CInnerInterface*, EngineDeviceMan* );
typedef AP_IQuestWindow (*factoryQuestWindow)( GLGaeaClient* , CInnerInterface*, EngineDeviceMan* );
typedef AP_IQuestWindowRenewal (*factoryQuestWindowRenewal)( GLGaeaClient* , CInnerInterface*, EngineDeviceMan* );
typedef AP_IFriendWindow (*factoryFriendWindow)( GLGaeaClient* , CInnerInterface*, EngineDeviceMan* );
typedef AP_IChatGroupInviteWindow (*factoryChatGroupInviteWindow)( GLGaeaClient* , CInnerInterface*, EngineDeviceMan* );
typedef AP_IModalWindowInner (*factoryModalWindow)( CInnerInterface*, EngineDeviceMan* );
typedef AP_IShopItemSearchWindow (*factoryShopItemSearchWindow)( GLGaeaClient* , CInnerInterface*, EngineDeviceMan* );
typedef AP_IItemSearchResultWindow (*factoryItemSearchResultWindow)( GLGaeaClient* , CInnerInterface*, EngineDeviceMan* );
typedef AP_IMapRequireCheck (*factoryMapRequireCheck)( GLGaeaClient* , CInnerInterface*, EngineDeviceMan* );
typedef AP_IPostBoxWindow (*factoryPostBoxWindow)( GLGaeaClient* , CInnerInterface*, EngineDeviceMan* );
typedef AP_IPostNotifyButton (*factoryPostNotifyButton)( GLGaeaClient* , CInnerInterface*, EngineDeviceMan* );
typedef AP_IQuestHelper (*factoryQuestHelper)( GLGaeaClient* , CInnerInterface*, EngineDeviceMan* );
typedef AP_ITutorialDialogue (*factoryTutorialDialogue)( GLGaeaClient* , CInnerInterface*, EngineDeviceMan* );
typedef AP_IBalloonText (*factoryBalloonText)( GLGaeaClient* , CInnerInterface*, EngineDeviceMan* );
typedef AP_ISkillWindow (*factorySkillWindow)( GLGaeaClient* , CInnerInterface*, EngineDeviceMan* );
typedef AP_IBasicInfoView (*factoryBasicInfoView)( GLGaeaClient* , CInnerInterface*, EngineDeviceMan* );
typedef AP_IExplainInterface (*factoryExplainInterface)( GLGaeaClient* , CInnerInterface*, EngineDeviceMan* );
typedef AP_IUILeftTopGroup (*factoryUILeftTopGroup)( GLGaeaClient* , CInnerInterface*, EngineDeviceMan* );
typedef AP_ISkillTrayTab (*factorySkillTrayTab)( GLGaeaClient* , CInnerInterface*, EngineDeviceMan* );
typedef AP_IBasicQuickSkillSlot (*factoryBasicQuickSkillSlot)( GLGaeaClient* , CInnerInterface*, EngineDeviceMan* );
typedef AP_ITargetInfoDisplay (*factoryTargetInfoDisplay)( GLGaeaClient*, CInnerInterface*, EngineDeviceMan* );
typedef AP_ICursorTargetInfo(*factoryCursorTargetInfo)( GLGaeaClient*, CInnerInterface*, EngineDeviceMan* );
typedef AP_IBalloonSpaceBar (*factoryBalloonSpaceBar)( GLGaeaClient* , CInnerInterface*, EngineDeviceMan* );
typedef AP_ITutorialViewStep (*factoryTutorialViewStep)( CInnerInterface*, EngineDeviceMan* );
typedef AP_ILunchBoxTimeDisplay (*factoryLunchBoxTimeDisplay)( GLGaeaClient* , CInnerInterface*, EngineDeviceMan* );
typedef AP_ISystemBuffTimeDisplay (*factorySystemBuffTimeDisplay)( GLGaeaClient* , CInnerInterface*, EngineDeviceMan* );
typedef AP_IVehicleWindow (*factoryVehicleWindow)( GLGaeaClient* , CInnerInterface*, EngineDeviceMan* );
typedef AP_IProgressDisplay (*factoryProgressDisplay)( GLGaeaClient* , CInnerInterface*, EngineDeviceMan* );
typedef AP_IStudentRecordWindow (*factoryStudentRecordWindow)( GLGaeaClient* , CInnerInterface*, EngineDeviceMan* );
typedef AP_IPointShopWindow (*factoryPointShopWindow)( GLGaeaClient* , CInnerInterface*, EngineDeviceMan* );
typedef AP_IRebirthWindow (*factoryRebirthWindow)( GLGaeaClient*, CInnerInterface*, EngineDeviceMan* );
typedef AP_IChatTypeList (*factoryChatTypeList)( CInnerInterface*, EngineDeviceMan* );
typedef AP_ICompetitionNotifyButton (*factoryCompetitionNotifyButton)( GLGaeaClient* , CInnerInterface*, EngineDeviceMan* );
typedef AP_IPopupMenu (*factoryPopupMenu)( GLGaeaClient*, CInnerInterface*, EngineDeviceMan* );
typedef AP_IInfoCTFDisplay (*factoryInfoCTFDisplay)( GLGaeaClient*, CInnerInterface*, EngineDeviceMan* );
typedef AP_ICTFResultWindow(*factoryCTFResultWindow)( GLGaeaClient*, CInnerInterface*, EngineDeviceMan* );
typedef AP_ICompetitionWindow(*factoryCompetitionWindow)( GLGaeaClient*, CInnerInterface*, EngineDeviceMan* );
typedef AP_ICompetitionTopRanking(*factoryCompetitionTopRanking)( GLGaeaClient*, CInnerInterface*, EngineDeviceMan* );
typedef AP_IReservedSkillAnimation(*factoryReservedSkillAnimation)( GLGaeaClient*, CInnerInterface*, EngineDeviceMan* );
typedef AP_IProductButton(*factoryProductButton)( GLGaeaClient*, CInnerInterface*, EngineDeviceMan* );
typedef AP_IProductWindow(*factoryProductWindow)( GLGaeaClient*, CInnerInterface*, EngineDeviceMan* );
typedef AP_IProductProgress(*factoryProductProgress)( GLGaeaClient*, CInnerInterface*, EngineDeviceMan* );
typedef AP_IProductResetWindow(*factoryProductResetWindow)( GLGaeaClient*, CInnerInterface*, EngineDeviceMan* );
typedef AP_IOddEvenWindow(*factoryOddEvenWindow)( GLGaeaClient*, CInnerInterface*, EngineDeviceMan* );
typedef AP_ITexasHoldemWindow(*factoryTexasHoldemWindow)( GLGaeaClient*, CInnerInterface*, EngineDeviceMan* );
typedef AP_ITexasHoldemWebWindow(*factoryTexasHoldemWebWindow)( GLGaeaClient*, CInnerInterface*, EngineDeviceMan* );
typedef AP_IClubWindow(*factoryClubWindow)( GLGaeaClient*, CInnerInterface*, EngineDeviceMan* );
typedef AP_IPrivateMarketSearchBuy(*factoryPrivateMarketSearchBuy)( GLGaeaClient*, CInnerInterface*, EngineDeviceMan* );
typedef AP_IPVEUserInterface(*factoryPVEUserInterface)( GLGaeaClient*, CInnerInterface*, EngineDeviceMan* );
typedef AP_IPTournamentUserInterface(*factoryTournamentUserInterface)( GLGaeaClient*, CInnerInterface*, EngineDeviceMan* );
typedef AP_IItemRebuildRenewal(*factoryItemRebuildRenewal)( GLGaeaClient*, CInnerInterface*, EngineDeviceMan* );
typedef AP_IProgessTimer(*factoryProgessTimer)( GLGaeaClient*, CInnerInterface*, EngineDeviceMan* );

//New Instance System
typedef AP_ISimpleSystemMessageDisplay(*factorySimpleSystemMessageDisplay)( GLGaeaClient*, CInnerInterface*, EngineDeviceMan* );
typedef AP_ISimpleTimerMessageBox(*factorySimpleTimerMessageBox)( GLGaeaClient*, CInnerInterface*, EngineDeviceMan* );
typedef AP_IRnCDMProgress(*factoryRnCDMprogress)( GLGaeaClient*, CInnerInterface*, EngineDeviceMan* );
typedef AP_IRnCDMRankWindow(*factoryRnCDMRankWindow)( GLGaeaClient*, CInnerInterface*, EngineDeviceMan* );
typedef AP_IRnCDMClubInfo(*factoryRnCDMClubInfo)( GLGaeaClient*, CInnerInterface*, EngineDeviceMan* );
typedef AP_IColorProperty(*factoryColorProperty)( GLGaeaClient*, CInnerInterface*, EngineDeviceMan* );
typedef AP_IRnCompetitionWindow(*factoryRnCompetitionWindow)( GLGaeaClient*, CInnerInterface*, EngineDeviceMan* );
typedef AP_IRnCompetitionResultWindow(*factoryRnCompetitionResultWindow)( GLGaeaClient*, CInnerInterface*, EngineDeviceMan* );
typedef AP_IRnCDMTopRanking(*factoryRnCDMTopRanking)( GLGaeaClient*, CInnerInterface*, EngineDeviceMan* );
typedef AP_ICaptureTheFlagEffectMan(*factoryCaptureTheFlagEffectMan)( GLGaeaClient*, CInnerInterface*, EngineDeviceMan* );
typedef AP_IGuidanceAuthProgressUI(*factoryGuidanceAuthProgressUI)( GLGaeaClient*, CInnerInterface*, EngineDeviceMan* );
typedef AP_IGuidanceProgressInfoUI(*factoryGuidanceProgressInfoUI)( GLGaeaClient*, CInnerInterface*, EngineDeviceMan* );
typedef AP_IGuidanceRankInfoUI(*factoryGuidanceRankInfoUI)( GLGaeaClient*, CInnerInterface*, EngineDeviceMan* );
typedef AP_ICTIAuthProgressUI(*factoryCTIAuthProgressUI)( GLGaeaClient*, CInnerInterface*, EngineDeviceMan* );
typedef AP_ICTIProgressInfoUI(*factoryCTIProgressInfoUI)( GLGaeaClient*, CInnerInterface*, EngineDeviceMan* );
typedef AP_ICTIRankInfoUI(*factoryCTIRankInfoUI)( GLGaeaClient*, CInnerInterface*, EngineDeviceMan* );

typedef AP_ITournamentWindowDashBoard(*factoryTournamentWindowDashBoard)( GLGaeaClient*, CInnerInterface*, EngineDeviceMan* );
typedef AP_ITournamentWindowUI(*factoryTournamentWindowUI)( GLGaeaClient*, CInnerInterface*, EngineDeviceMan* );
typedef AP_ITournamentToToWindow(*factoryTournamentToToWindow)( GLGaeaClient*, CInnerInterface*, EngineDeviceMan* );
typedef AP_ITournamentMoreInfoWindow(*factoryTournamentMoreInfoWindow)( GLGaeaClient*, CInnerInterface*, EngineDeviceMan* );

typedef AP_IFlyCameraControlWindow(*factoryFlyCameraControlWindow)( GLGaeaClient*, CInnerInterface*, EngineDeviceMan* );
typedef AP_ISetitemPreviewWindow(*factorySetitemPreviewWindow)( GLGaeaClient*, CInnerInterface*, EngineDeviceMan* );

typedef AP_ISummonStateDisplayUI(*factorySummonStateDisplayUI)( GLGaeaClient*, CInnerInterface*, EngineDeviceMan* );
typedef AP_IFactionInfoWindow(*factoryFactionInfoWindow)( GLGaeaClient*, CInnerInterface*, EngineDeviceMan* );
typedef AP_IWorldBattleButton(*factoryWorldBattleButton)( GLGaeaClient*, CInnerInterface*, EngineDeviceMan* );

class MyFactoryUI
{
public:
    MyFactoryUI();

	// Lotto System;
	AP_ILottoMainUI CreateLottoMainUI( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	AP_ILottoBuyLottoUI CreateLottoBuyLottoUI( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	AP_ILottoBuyListInfo CreateLottoBuyListInfo( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	AP_ILottoWinManListInfo CreateLottoWinManListInfo( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	AP_ILottoDisplayUI CreateLottoDisplayUI( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	AP_ILottoNotifyButton CreateLottoNotifyButton( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );

    AP_IMiniMap CreateMiniMap( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	AP_IDPSWindow CreateDPSWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );	
    AP_IKeySettingWindow CreateKeySettingWindow( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_IChatMacroWindow CreateChatMacroWindow( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_INameDisplayMan CreateNameDisplayMan( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_IPrivateMarketShowMan CreatePrivateMarketShowMan( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_ISummonNameDisplayMan CreateSummonNameDisplayMan( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_ISkillWindowToTray CreateSkillWindowToTray( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_IPartyWindow CreatePartyWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	AP_IPartyWindowRenewal CreatePartyWindowRenewal( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	AP_IPartyYesNoWindow CreatePartyYesNoWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );	
	AP_IExpeditionInviteWindow CreateExpeditionInviteWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	AP_IExpeditionCheckReadyWindow CreateExpeditionCheckReadyWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );	
	AP_IPartyStateWindow CreatePartyStateWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	AP_IPartyTenderWindow CreatePartyTenderWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );	
	AP_IPartyTenderNotifyButton CreatePartyTenderNotifyButton( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );	
	AP_IPartyDistributionWindow CreatePartyDistributionWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );	
	AP_IPartyDistributionListWindow CreatePartyDistributionListWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );	
	AP_IMiniExpeditionWindow CreateMiniExpeditionWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_IBasicChatWindow CreateBasicChatWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_IChatOptionWindow CreateChatOptionWindow( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_ITradeWindow CreateTradeWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_ITradeInventoryWindow CreateTradeInventoryWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_IPtoPWindow CreatePtoPWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_IInventoryWindow CreateInventoryWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_IItemMove CreateItemMove( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_IMarketWindow CreateMarketWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_IHeadChatDisplayMan CreateHeadChatDisplayMan( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_IConftModalWindow CreateConftModalWindow( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_IPartyModalWindow CreatePartyModalWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	AP_IChatGroupModalWindow CreateChatGroupModalWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_IConftDisplayMan CreateConftDisplayMan( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_IQuestWindow CreateQuestWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_IQuestWindowRenewal CreateQuestWindowRenewal( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_IFriendWindow CreateFriendWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	AP_IChatGroupInviteWindow CreateChatGroupInviteWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_IModalWindowInner CreateModalWindow( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_IShopItemSearchWindow CreateShopItemSearchWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_IItemSearchResultWindow CreateItemSearchResultWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_IMapRequireCheck CreateMapRequireCheck( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_IPostBoxWindow CreatePostBoxWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_IPostNotifyButton CreatePostNotifyButton( GLGaeaClient* pGaeaClient, CInnerInterface* pInerface, EngineDeviceMan* pEngineDevice );
    AP_IQuestHelper CreateQuestHelper( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_ITutorialDialogue CreateTutorialDialogue( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_IBalloonText CreateBalloonText( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_ISkillWindow CreateSkillWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_IBasicInfoView CreateBasicInfoView( GLGaeaClient* pGaeaClient, CInnerInterface* pInerface, EngineDeviceMan* pEngineDevice );
    AP_IExplainInterface CreateExplainInterface( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_IUILeftTopGroup CreateUILeftTopGroup( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_ISkillTrayTab CreateSkillTrayTab( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_IBasicQuickSkillSlot CreateBasicQuickSkillSlot( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_ITargetInfoDisplay CreateTargetInfoDisplay( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_ICursorTargetInfo CreateCursorTargetInfo( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_IBalloonSpaceBar CreateBalloonSpaceBar( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_ITutorialViewStep CreateTutorialViewStep( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_ILunchBoxTimeDisplay CreateLunchBoxTimeDisplay( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_ISystemBuffTimeDisplay CreateSystemBuffTimeDisplay( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_IVehicleWindow CreateVehicleWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_IProgressDisplay CreateProgressDisplay( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_IStudentRecordWindow CreateStudentRecordWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_IPointShopWindow CreatePointShopWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_IRebirthWindow CreateRebirthWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_IChatTypeList CreateChatTypeList( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_ICompetitionNotifyButton CreateCompetitionNotifyButton( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_IPopupMenu CreatePopupMenu( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_IInfoCTFDisplay CreateInfoCTFDisplay( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_ICTFResultWindow CreateCTFResultWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_ICompetitionWindow CreateCompetitionWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_ICompetitionTopRanking CreateCompetitionTopRanking( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_IReservedSkillAnimation CreateReservedSkillAnimation( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_IProductButton CreateProductButton( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_IProductWindow CreateProductWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_IProductProgress CreateProductProgress( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	AP_IProductResetWindow CreateProductResetWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );	
	AP_IOddEvenWindow CreateOddEvenWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );	
	AP_ITexasHoldemWindow CreateTexasHoldemWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	AP_ITexasHoldemWebWindow CreateTexasHoldemWebWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_IClubWindow CreateClubWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	AP_IPrivateMarketSearchBuy CreatePrivateMarketSearchBuy( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );	
	AP_IPVEUserInterface CreatePVEUserInterface( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	AP_IPTournamentUserInterface CreateTournamentUserInterface( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	AP_IItemRebuildRenewal CreateItemRebuildRenewal( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	AP_IColorProperty CreateColorProperty( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_IProgessTimer CreateProgessTimer( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );

    //New Instance System
    AP_ISimpleSystemMessageDisplay CreateSimpleSystemMessageDisplay( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_ISimpleTimerMessageBox CreateSimpleTimerMessageBox( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	AP_IRnCDMProgress CreateRnCDMProgress( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	AP_IRnCDMRankWindow CreateRnCDMRankWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	AP_IRnCDMClubInfo CreateRnCDMClubInfo( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	AP_IRnCompetitionWindow CreateRnCompetitionWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_IRnCompetitionResultWindow CreateRnCompetitionResultWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	AP_IRnCDMTopRanking CreateRnCDMTopRanking( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_ICaptureTheFlagEffectMan CreateCaptureTheFlagEffectMan( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_IGuidanceAuthProgressUI CreateGuidanceAuthProgressUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_IGuidanceProgressInfoUI CreateGuidanceProgressInfoUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    AP_IGuidanceRankInfoUI CreateGuidanceRankInfoUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	AP_ICTIAuthProgressUI CreateCTIAuthProgressUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	AP_ICTIProgressInfoUI CreateCTIProgressInfoUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	AP_ICTIRankInfoUI CreateCTIRankInfoUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );

	AP_ITournamentWindowDashBoard CreateTournamentDashBoardWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	AP_ITournamentWindowUI CreateTournamentWindowUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	AP_ITournamentToToWindow CreateTournamentToToWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	AP_ITournamentMoreInfoWindow CreateTournamentMoreInfoWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    
	AP_IFlyCameraControlWindow CreateFlyCameraControlUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	AP_ISetitemPreviewWindow CreateSetitemPreviewUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );

	AP_ISummonStateDisplayUI CreateSummonStateDisplayUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	AP_IFactionInfoWindow CreateFactionInfoWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	AP_IWorldBattleButton CreateWorldBattleButton( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );

private:
	// Lotto System;
	factoryLottoMainUI m_factoryLottoMainUI;
	factoryLottoBuyLottoUI m_factoryLottoBuyLottoUI;
	factoryLottoBuyListInfo m_factoryLottoBuyListInfo;
	factoryLottoWinManListInfo m_factoryLottoWinManListInfo;
	factoryLottoDisplayUI m_factoryLottoDisplayUI;
	factoryLottoNotifyButton m_factoryLottoNotifyButton;
	
    factoryMiniMap m_factoryMinimap;
	factoryDPSWindow m_factoryDPSWindow;	
    factoryKeySettingWindow m_factoryKeySettingWindow;
    factoryChatMacroWindow m_factoryChatMacroWindow;
    factoryNameDisplayMan m_factoryNameDisplayMan;
    factoryPrivateMarketShowMan m_factoryPrivateMarketShowMan;
    factorySummonNameDisplayMan m_factorySummonNameDisplayMan;
    factorySkillWindowToTray m_factorySkillWindowToTray;
    factoryPartyWindow m_factoryPartyWindow;
	factoryPartyWindowRenewal m_factoryPartyWindowRenewal;
	factoryPartyYesNoWindow m_factoryPartyYesNoWindow;	
	factoryExpeditionInviteWindow m_factoryExpeditionInviteWindow;	
	factoryExpeditionCheckReadyWindow m_factoryExpeditionCheckReadyWindow;	
	factoryPartyStateWindow m_factoryPartyStateWindow;
	factoryPartyTenderWindow m_factoryPartyTenderWindow;
	factoryPartyTenderNotifyButton m_factoryPartyTenderNotifyButton;
	factoryPartyDistributionWindow m_factoryPartyDistributionWindow;
	factoryPartyDistributionListWindow m_factoryPartyDistributionListWindow;
	factoryMiniExpeditionWindow m_factoryMiniExpeditionWindow;
    factoryBasicChat m_factoryChat;
    factoryChatOptionWindow m_factoryChatOptionWindow;
    factoryTradeWindow m_factoryTradeWindow;
    factoryTradeInventoryWindow m_factoryTradeInventoryWindow;
    factoryPtoPWindow m_factoryPtoPWindow;
    factoryInventoryWindow m_factoryInventoryWindow;
    factoryItemMove m_factoryItemMove;
    factoryMarketWindow m_factoryMarketWindow;
    factoryHeadChatDisplayMan m_factoryHeadChatDisplayMan;
    factoryConftModalWindow m_factoryConftModalWindow;
    factoryPartyModalWindow m_factoryPartyModalWindow;
	factoryChatGroupModalWindow m_factoryChatGroupModalWindow;
    factoryConftDisplayMan m_factoryConftDisplayMan;
    factoryQuestWindow m_factoryQuestWindow;
    factoryQuestWindowRenewal m_factoryQuestWindowRenewal;
    factoryFriendWindow m_factoryFriendWindow;
	factoryChatGroupInviteWindow m_factoryChatGroupInviteWindow;
    factoryModalWindow m_factoryModalWindow;
    factoryShopItemSearchWindow m_factoryShopItemSearchWindow;
    factoryItemSearchResultWindow m_factoryItemSearchResultWindow;
    factoryMapRequireCheck m_factoryMapRequireCheck;
    factoryPostBoxWindow m_factoryPostBoxWindow;
    factoryPostNotifyButton m_factoryPostNotifyButton;
    factoryQuestHelper m_factoryQuestHelper;
    factoryTutorialDialogue m_factoryTutorialDialogue;
    factoryBalloonText m_factoryBalloonText;
    factorySkillWindow m_factorySkillWindow;
    factoryBasicInfoView m_factoryBasicInfoView;
    factoryExplainInterface m_factoryExplainInterface;
    factoryUILeftTopGroup m_factoryUILeftTopGroup;
    factorySkillTrayTab m_factorySkillTrayTab;
    factoryBasicQuickSkillSlot m_factoryBasicQuickSkillSlot;
    factoryTargetInfoDisplay m_factoryTargetInfoDisplay;
    factoryCursorTargetInfo m_factoryCursorTargetInfo;
    factoryBalloonSpaceBar m_factoryBalloonSpaceBar;
    factoryTutorialViewStep m_factoryTutorialViewStep;
    factoryLunchBoxTimeDisplay m_factoryLunchBoxTimeDisplay;
    factorySystemBuffTimeDisplay m_factorySystemBuffTimeDisplay;
    factoryVehicleWindow m_factoryVehicleWindow;
    factoryProgressDisplay m_factoryProgressDisplay;
    factoryStudentRecordWindow m_factoryStudentRecordWindow;
    factoryPointShopWindow m_factoryPointShopWindow;
    factoryRebirthWindow m_factoryRebirthWindow;
    factoryChatTypeList m_factoryChatTypeList;
    factoryCompetitionNotifyButton m_factoryCompetitionNotifyButton;
    factoryPopupMenu m_factoryPopupMenu;
    factoryInfoCTFDisplay m_factoryInfoCTFDisplay;
    factoryCTFResultWindow m_factoryCTFResultWindow;
    factoryCompetitionWindow m_factoryCompetitionWindow;
    factoryCompetitionTopRanking m_factoryCompetitionTopRanking;
    factoryReservedSkillAnimation m_factoryReservedSkillAnimation;
    factoryProductButton m_factoryProductButton;
    factoryProductWindow m_factoryProductWindow;
    factoryProductProgress m_factoryProductProgress;
	factoryProductResetWindow m_factoryProductResetWindow;
	factoryOddEvenWindow m_factoryOddEvenWindow;	
	factoryTexasHoldemWindow m_factoryTexasHoldemWindow;
	factoryTexasHoldemWebWindow m_factoryTexasHoldemWebWindow;
    factoryClubWindow m_factoryClubWindow;
	factoryPrivateMarketSearchBuy m_factoryPrivateMarketSearchBuy;
	factoryPVEUserInterface m_factoryPVEUserInterface;
	factoryTournamentUserInterface m_factoryTournamentUserInterface;
	factoryItemRebuildRenewal m_factoryItemRebuildRenewal;
	factoryProgessTimer m_factoryProgessTimer;

    //New Instance System
    factorySimpleSystemMessageDisplay m_factorySimpleSystemMessageDisplay;
    factorySimpleTimerMessageBox m_factorySimpleTimerMessageBox;
	factoryRnCDMprogress m_factoryRnCDMprogress;
	factoryRnCDMRankWindow m_factoryRnCDMRankWindow;
	factoryRnCDMClubInfo m_factoryRnCDMClubInfo;
	factoryColorProperty m_factoryColorProperty;
	factoryRnCompetitionWindow m_factoryRnCompetitionWindow;
    factoryRnCompetitionResultWindow m_factoryRnCompetitionResultWindow;
	factoryRnCDMTopRanking m_factoryRnCDMTopRanking;
    factoryCaptureTheFlagEffectMan m_factoryCaptureTheFlagEffectMan;
    factoryGuidanceAuthProgressUI m_factoryGuidanceAuthProgressUI;
    factoryGuidanceProgressInfoUI m_factoryGuidanceProgressInfoUI;
    factoryGuidanceRankInfoUI m_factoryGuidanceRankInfoUI;
	factoryCTIAuthProgressUI m_factoryCTIAuthProgressUI;
	factoryCTIProgressInfoUI m_factoryCTIProgressInfoUI;
	factoryCTIRankInfoUI m_factoryCTIRankInfoUI;

	factoryTournamentWindowDashBoard m_factoryTournamentWindowDashBoard;
	factoryTournamentWindowUI m_factoryTournamentWindowUI;
	factoryTournamentToToWindow m_factoryTournamentToToWindow;
	factoryTournamentMoreInfoWindow m_factoryTournamentMoreInfoWindow;

	factoryFlyCameraControlWindow m_factoryFlyCameraControlWindow;
	factorySetitemPreviewWindow m_factorySetitemPreviewWindow;

	factorySummonStateDisplayUI m_factorySummonStateDisplayUI;
	factoryFactionInfoWindow m_factoryFactionInfoWindow;
	factoryWorldBattleButton m_factoryWorldBattleButton;
};

// Lotto System;
AP_ILottoMainUI fnPtrLottoMainUI( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_ILottoBuyLottoUI fnPtrLottoBuyLottoUI( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_ILottoBuyListInfo fnPtrLottoBuyListInfo( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_ILottoWinManListInfo fnPtrLottoWinManListInfo( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_ILottoDisplayUI fnPtrLottoDisplayUI( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_ILottoNotifyButton fnPtrLottoNotifyButton( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );

AP_IMiniMap fnPtrOldFactoryMinimap( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IDPSWindow fnPtrOldFactoryDPSWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IKeySettingWindow fnPtrOldFactoryKeySettingWindow( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IChatMacroWindow fnPtrOldFactoryChatMacroWindow( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_INameDisplayMan fnPtrOldFactoryNameDisplayMan( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IPrivateMarketShowMan fnPtrOldFactoryPrivateMarketShowMan( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_ISummonNameDisplayMan fnPtrOldFactorySummonNameDisplayMan( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_ISkillWindowToTray fnPtrOldFactorySkillWindowToTray( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IPartyWindow fnPtrOldFactoryPartyWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IPartyWindowRenewal fnPtrOldFactoryPartyWindowRenewal( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IPartyYesNoWindow fnPtrOldFactoryPartyYesNoWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IExpeditionInviteWindow fnPtrOldFactoryExpeditionInviteWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IExpeditionCheckReadyWindow fnPtrOldFactoryExpeditionCheckReadyWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IPartyStateWindow fnPtrOldFactoryPartyStateWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IPartyTenderWindow fnPtrOldFactoryPartyTenderWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IPartyTenderNotifyButton fnPtrOldFactoryPartyTenderNotifyButton( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IPartyDistributionWindow fnPtrOldFactoryPartyDistributionWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IPartyDistributionListWindow fnPtrOldFactoryPartyDistributionListWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IMiniExpeditionWindow fnPtrOldFactoryMiniExpeditionWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IBasicChatWindow fnPtrOldFactoryBasicChatWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IChatOptionWindow fnPtrOldFactoryChatOptionWindow( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_ITradeWindow fnPtrOldFactoryTradeWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_ITradeInventoryWindow fnPtrOldFactoryTradeInventoryWindow( GLGaeaClient* pGaeClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IPtoPWindow fnPtrOldFactoryPtoPWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IInventoryWindow fnPtrOldFactoryInventoryWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IItemMove fnPtrOldFactoryItemMove( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IMarketWindow fnPtrOldFactoryMarketWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IHeadChatDisplayMan fnPtrOldFactoryHeadChatDisplayMan( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IConftModalWindow fnPtrOldFactoryConftModalWindow( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IPartyModalWindow fnPtrOldFactoryPartyModalWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IChatGroupModalWindow fnPtrOldFactoryChatGroupModalWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IConftDisplayMan fnPtrOldFactoryConftDisplayMan( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IQuestWindow fnPtrOldFactoryQuestWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IQuestWindowRenewal fnPtrOldFactoryQuestWindowRenewal( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IFriendWindow fnPtrOldFactoryFriendWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IChatGroupInviteWindow fnPtrOldFacotryChatGroupInviteWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IModalWindowInner fnPtrOldFactoryModalWindow( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IShopItemSearchWindow fnPtrOldFactoryShopItemSearchWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IItemSearchResultWindow fnPtrOldFactoryItemSearchResultWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IMapRequireCheck fnPtrOldFactoryMapRequireCheck( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IPostBoxWindow fnPtrOldFactoryPostBoxWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInerterface, EngineDeviceMan* pEngineDevice );
AP_IPostNotifyButton fnPtrOldFactoryPostNotifyButton( GLGaeaClient* pGaeaClient, CInnerInterface* pInerface, EngineDeviceMan* pEngineDevice );
AP_IQuestHelper fnPtrOldFactoryQuestHelper( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_ITutorialDialogue fnPtrOldFactoryTutorialDialogue( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IBalloonText fnPtrOldFactoryBalloonText( GLGaeaClient* pGaeaClient, CInnerInterface* pInerface, EngineDeviceMan* pEngineDevice );
AP_ISkillWindow fnPtrOldFactorySkillWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInerface, EngineDeviceMan* pEngineDevice );
AP_IBasicInfoView fnPtrOldFactoryBasicInfoView( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IExplainInterface fnPtrOldFactoryExplainInterface( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IUILeftTopGroup fnPtrOldFactoryUILeftTopGroup( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_ISkillTrayTab fnPtrOldFactorySkillTrayTab( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IBasicQuickSkillSlot fnPtrOldFactoryBasicQuickSkillSlot( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_ITargetInfoDisplay fnPtrOldFactoryTargetInfoDisplay( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_ICursorTargetInfo fnPtrOldFactoryCursorTargetInfo( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IBalloonSpaceBar fnPtrOldFactoryBalloonSpaceBar( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_ITutorialViewStep fnPtrOldFactoryTutorialViewStep( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_ILunchBoxTimeDisplay fnPtrOldFactoryLunchBoxTimeDisplay( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_ISystemBuffTimeDisplay fnPtrOldFactorySystemBuffTimeDisplay( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IVehicleWindow fnPtrOldFactoryVehicleWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IProgressDisplay fnPtrOldFactoryProgressDisplay ( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IStudentRecordWindow fnPtrOldFactoryStudentRecordWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IPointShopWindow fnPtrOldFactoryPointShopWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IRebirthWindow fnPtrOldFactoryRebirthWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IChatTypeList fnPtrOldFactoryChatTypeList( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_ICompetitionNotifyButton fnPtrOldFactoryCompetitionNotifyButton( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IPopupMenu fnPtrOldFactoryPopupMenu( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IInfoCTFDisplay fnPtrOldFactoryInfoCTFDisplay( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_ICTFResultWindow fnPtrOldFactoryCTFResultWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_ICompetitionWindow fnPtrOldFactoryCompetitionWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_ICompetitionTopRanking fnPtrOldFactoryCompetitionTopRanking( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IReservedSkillAnimation fnPtrOldFactoryReservedSkillAnimation( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IProductButton fnPtrOldFactoryProductButton( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IProductWindow fnPtrOldFactoryProductWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IProductProgress fnPtrOldFactoryProductProgress( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IProductResetWindow fnPtrOldFactoryProductResetWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IOddEvenWindow fnPtrOldFactoryOddEvenWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_ITexasHoldemWindow fnPtrOldFactoryTexasHoldemWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_ITexasHoldemWebWindow fnPtrOldFactoryTexasHoldemWebWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IClubWindow fnPtrOldFactoryClubWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IPrivateMarketSearchBuy fnPtrOldFactoryPrivateMarketSearchBuy( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IPVEUserInterface fnPtrOldFactoryPVEUserInterface( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IPTournamentUserInterface fnPtrOldFactoryTournamentUserInterface( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IItemRebuildRenewal fnPtrOldFactoryItemRebuildRenewal( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IProgessTimer fnPtrOldFactoryProgessTimer( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IColorProperty fnptroldFactoryColorProperty( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );

//New Instance System
AP_ISimpleSystemMessageDisplay fnptroldFactorySimpleSystemMessageDisplay( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_ISimpleTimerMessageBox fnptroldFactorySimpleTimerMessageBox( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IRnCDMProgress fnptroldFactoryRnCDMProgress( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IRnCDMRankWindow fnptroldFactoryRnCDMRankWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IRnCDMRankWindow fnptroldFactoryRnCDMRankWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IRnCDMClubInfo fnptroldFactoryRnCDMClubInfo( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IRnCompetitionWindow fnPtrOldFactoryRnCompetitionWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IRnCompetitionResultWindow fnPtrOldFactoryRnCompetitionResultWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IRnCDMTopRanking fnPtroldFactoryRnCDMTopRanking( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_ICaptureTheFlagEffectMan fnPtrCaptureTheFlagEffectMan( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IGuidanceAuthProgressUI fnPtrGuidanceAuthProgressUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IGuidanceProgressInfoUI fnPtrGuidanceProgressInfoUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IGuidanceRankInfoUI fnPtrGuidanceRankInfoUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_ICTIAuthProgressUI fnPtrCTIAuthProgressUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_ICTIProgressInfoUI fnPtrCTIProgressInfoUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_ICTIRankInfoUI fnPtrCTIRankInfoUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );

AP_ITournamentWindowDashBoard fnptroldFactoryTournamentWindowDashBoard( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_ITournamentWindowUI fnptroldFactoryTournamentWindowUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_ITournamentToToWindow fnptroldFactoryTournamentToToWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_ITournamentMoreInfoWindow fnptroldFactoryTournamentMoreInfoWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );

AP_IFlyCameraControlWindow fnptroldFactoryFlyCameraControlWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_ISetitemPreviewWindow fnptroldFactorySetitemPreviewWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );

AP_ISummonStateDisplayUI fnptrSummonStateDisplayUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IFactionInfoWindow fnptrFactionInfoWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
AP_IWorldBattleButton fnptrWorldBattleButton( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );