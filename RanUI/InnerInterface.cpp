#include "StdAfx.h"

#include "./StaticUIManager.h"
#include "InnerInterface.h"

#include "../SigmaCore/String/StringFormat.h"
#include "../SigmaCore/Log/LogMan.h"

#include "../EngineLib/GUInterface/BasicVarTextBox.h"
#include "../EngineLib/GUInterface/GameTextControl.h"
#include "../EngineLib/GUInterface/UITextControl.h"
#include "../EngineLib/GUInterface/UIKeyCheck.h"
#include "../EngineLib/GUInterface/NSGUI.h"
#include "../EngineLib/DxTools/RENDERPARAM.h"
#include "../enginelib/DxEffect/Single/DxEffGroupPlayer.h"

#include "../EngineLib/Common/CommonWeb.h"
#include "../EngineLib/Common/DXInputString.h"
#include "../EngineLib/DxResponseMan.h"
#include "../EngineLib/GUInterface/UIMutuallyExclusive.h"


#include "../RanLogic/Item/GLItemMan.h"
#include "../RanLogic/SNS/GLSNSData.h"
#include "../RanLogic/GLogicDataMan.h"

#include "../RanLogicClient/Stage/DxGlobalStage.h"
#include "../RanLogicClient/GLGaeaClient.h"
#include "../RanLogicClient/Party/GLPartyClient.h"
#include "../RanLogicClient/Tutorial/GLBaseTutorial.h" // by luxes.
#include "../RanLogicClient/Land/GLLandManClient.h"
#include "../RanLogicClient/Input/GLInputManager.h"
#include "../RanLogicClient/Club/GLClubClient.h"
#include "../RanLogicClient/GroupChat/GroupChatClient.h"
#include "../RanLogicClient/Party/GLPartyManagerClient.h"
#include "../RanLogicClient/TriggerSystem/ClientRebirthMessage.h"
#include "../RanLogic/GLUseFeatures.h"
#include "../RanLogicClient/Tournament/GLTournamentClient.h"
#include "../Ranlogicclient/GLGaeaClient.h"

#include "../RanLogic/RANPARAM.h"

#include "../RanLogic/Network/s_CHttpPatch.h"
#include "../RanLogic/SNS/GLSNSData.h"

#include "OldUI/Util/ModalWindow.h"
#include "OldUI/Interface/UILeftTopGroup.h"
#include "OldUI/Interface/AdminMessageDisplay.h"
#include "OldUI/Interface/BasicChat.h"
#include "OldUI/Interface/BasicInfoView.h"
#include "OldUI/Interface/BasicQuickSkillSlot.h"
#include "OldUI/Interface/QuestAlarm.h"
#include "OldUI/Interface/SkillTrayTab.h"
#include "OldUI/Interface/SkillTimeDisplay.h"
#include "OldUI/Interface/CDMSafeTimeDisplay.h"
#include "OldUI/Confront/ConftModalWindow.h"
#include "OldUI/Party/PartyModalWindow.h"
#include "OldUI/Item/InventoryWindow.h"
#include "OldUI/Item/RnInventoryWindow.h"
#include "OldUI/Item/ItemShopWindowWeb.h"
#include "OldUI/Display/MapMoveDisplay.h"
#include "OldUI/Display/NameDisplayMan.h"
#include "OldUI/Display/ProgressDisplay.h"
#include "OldUI/Display/GatherGauge.h"
#include "OldUI/Display/CdmRankingDisplay.h"
#include "OldUI/PrivateMarket/PrivateMarketWindow.h"
#include "OldUI/PrivateMarket/ShopItemSearchWindow.h"
#include "OldUI/PrivateMarket/ItemSearchResultWindow.h"
#include "OldUI/Quest/QuestWindow.h"
#include "OldUI/Quest/WindowRenewal/QuestWindowRenewal.h"
#include "OldUI/Message/RebirthDialogue.h"
#include "OldUI/Message/MapRequireCheck.h"
#include "OldUI/Message/SystemMessageWindow.h"
#include "OldUI/Feature/ItemMixWindow.h"
#include "OldUI/Feature/StatsResetWindow.h"
#include "OldUI/Vehicle/BikeGetOffWindow.h"
#include "OldUI/Vehicle/BikeMessage.h"
#include "OldUI/Vehicle/BikeBoostWindow.h"
#include "OldUI/Post/PostBoxWindow.h"
#include "OldUI/SNS/SNSWindow.h"
#include "OldUI/StudentRecord/ActivityWindowPage.h"
#include "OldUI/Char/CharacterWindow.h"
#include "OldUI/Interface/QBoxButton.h" // by luxes.
#include "OldUI/PointShop/PointShopWindow.h"
#include "OldUI/Message/ForceRebirth.h"
#include "OldUI/Display/ItemInfoTooltip.h"
#include "OldUI/Display/NotifyCTFSpurt.h"
#include "OldUI/NPC/DialogueWindowRenewal.h"
#include "OldUI/Item/ItemPreviewWindow.h"
#include "OldUI/ReferChar/ReferCharWindow.h"
#include "OldUI/Char/CharacterView.h"
#include "OldUI/Char/RnCharacterWindowInterface.h"
#include "OldUI/GroupChat/MaxChatWindow.h"
#include "OldUI/Confront/ConftConfirm.h"
#include "OldUI/Feature/RnMapWindow.h"

// Lotto System
#include "OldUI/LottoSystem/GLLottoSystemManUI.h"

#include "OldUI/Display/QuestionItemDisplay.h"
#include "OldUI/Competition/DoublePoint.h"
#include "OldUI/SelectiveformBox/SelectiveformBoxWindow.h"
#include "OldUI/WorldBattleSystem/WorldBattleButton.h"


#include <boost/tr1/tuple.hpp> 
#include <boost/bind.hpp>
#include <boost/mem_fn.hpp>
#include <boost/function.hpp>
#include <boost/foreach.hpp>
#include <boost/lambda/lambda.hpp>

//-----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
//-----------------------------------------------------------------------------

#include "./OldUI/Display/SkillInforTooltip.h"

#define CHAT_MACRO_SIZE	10

/*
CInnerInterface& CInnerInterface::GetInstance()
{
	static CInnerInterface Instance;
	return Instance;
}
*/

const float	CInnerInterface::fMENU_LIFTUP = 90.0f;
const int CInnerInterface::nOUTOFRANGE = -1;

CInnerInterface::CInnerInterface(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice, bool bToolMode)
    : InterfaceBase(pGaeaClient)
    , m_pEngineDevice(pEngineDevice)
    , m_emConflictReqType(EMCONFT_ONE)
    , m_ConflictReqID(GAEAID_NULL)
    , m_bBlockProgramFound(false)
    , m_dwEventQuestID(NATIVEID_NULL().dwID)
    , m_dwEventQuestStep(UINT_MAX)
    , m_nINFO_DISPLAY_TYPE(EM_INFO_DISPLAY_BASIC)
    , m_bCHANNEL(false)
    , m_bFirstVNGainSysCall(true)
    , m_bDuplicateLogin(FALSE)
    //, m_bArmsSwapReserve(false)
    , m_bItemShopLoad(false)
    , m_bInitUpdateOrder(false)
    , m_INFO_DISPLAY_SKILL_sNativeID_Previous(NATIVEID_NULL())
    , m_INFO_DISPLAY_SKILL_bNextLevel_Previous(FALSE)
    , m_INFO_DISPLAY_SKILL_wLevel_Previous(0)
    , m_bToolMode(bToolMode)
    , m_ReqChaDbNum(0)
    , m_pInvenItemTooltipGroup( NULL )
    , m_pWishItemTooltipGroup( NULL )
    , m_pMarketWishItemTooltipGroup( NULL )
	, m_bBlockModalUI(false)
	, m_pCostumeStatWindow(NULL)
	, m_pSelformBoxWindow(NULL)
{
    m_pPath = m_pGaeaClient->GetSubPath();

	ResetControl();
	
	m_mapLinkData.clear();
	m_mapItemLinkData.clear();
	m_mapPartyLinkData.clear();

	m_dwLinkDataIndex = 0;
	m_dwItemLinkDataIndex = 0;
	m_dwPartyLinkDataIndex = 0;
}

CInnerInterface::~CInnerInterface ()
{
}

void CInnerInterface::UiSetResolution( long lResolution )
{
    uiman::GetInnerUIMan().SetResolution( lResolution );
}

long CInnerInterface::UiGetResolution()
{
    return CUIMan::GetResolution();
}

HRESULT CInnerInterface::UiRender(LPDIRECT3DDEVICEQ pd3dDevice)
{
    return uiman::GetInnerUIMan().Render(pd3dDevice);
}

HRESULT CInnerInterface::UiRestoreDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice)
{
    return uiman::GetInnerUIMan().RestoreDeviceObjects(pd3dDevice);
}

HRESULT CInnerInterface::UiInvalidateDeviceObjects()
{
    return uiman::GetInnerUIMan().InvalidateDeviceObjects();
}

BOOL CInnerInterface::UiRegisterControl( CUIControl* pNewControl, bool bPosisionKeep )
{
    return uiman::GetInnerUIMan().RegisterControl(pNewControl, bPosisionKeep);
}

BOOL CInnerInterface::UiIsVisibleGroup(UIGUID ControlID)
{
    return uiman::GetInnerUIMan().IsVisibleGroup(ControlID);
}

void CInnerInterface::UiShowGroupTop(UIGUID ControlID, bool bMakeMsg)                   
{ 
    uiman::GetInnerUIMan().ShowGroupTop(ControlID, bMakeMsg); 
}

void CInnerInterface::UiShowGroupBottom(UIGUID ControlID, bool bMakeMsg)                
{ 
    uiman::GetInnerUIMan().ShowGroupBottom(ControlID, bMakeMsg); 
}

void CInnerInterface::UiShowGroupUpdate(UIGUID ControlID, bool bMakeMsg)                   
{ 
	uiman::GetInnerUIMan().ShowGroupUpdate(ControlID, bMakeMsg); 
}

void CInnerInterface::UiShowGroupFocus(UIGUID ControlID, bool bMakeMsg)                 
{ 
    UIGUID uiID = UIMutuallyExclusive::GetInstance().GetMutually( ControlID );
    if( uiman::GetInnerUIMan().IsVisibleGroup(uiID) )
        uiman::GetInnerUIMan().HideGroup( ControlID );
    else
        uiman::GetInnerUIMan().ShowGroupFocus(ControlID, bMakeMsg);    
}

void CInnerInterface::UiSetGroupLock(UIGUID ControlID, bool bLock)
{
	uiman::GetInnerUIMan().SetGroupLock(ControlID, bLock);
}

void CInnerInterface::UISetModalGroup(UIGUID ControlID, bool bModal)
{
    uiman::GetInnerUIMan().SetModalGroup(ControlID, bModal);
}

void CInnerInterface::UiHideGroup(UIGUID ControlID, bool bMakeMsg)
{
	uiman::GetInnerUIMan().HideGroup(ControlID, bMakeMsg); 
}

void CInnerInterface::UiRefresh( UIGUID ControlID )
{
	uiman::GetInnerUIMan().Refresh( ControlID ); 
}

void CInnerInterface::ResetControl()
{
	// Lotto System
	m_pLottoMainUI = NULL;
	m_pLottoBuyLottoUI = NULL;
	m_pLottoBuyListInfo = NULL;
	m_pLottoWinManListInfo = NULL;
	m_pLottoDisplayUI = NULL;
	m_pLottoNotifyButton = NULL;
	GLLottoSystemManUI::Instance()->Destroy();

	m_pColorProperty = NULL;

	m_pSummonStateDisplayUI = NULL;
	m_pFactionInfoWindow = NULL;
	m_pWorldBattleButton = NULL;

	m_pSimpleHP = NULL;
	m_pBusWindow = NULL;
	m_pChatGroupWindow = NULL;
	m_pQuestAlarm = NULL;
	m_pDamageDisplayMan = NULL;
	m_pDialogueWindow = NULL;
	m_pStorageWindow = NULL;
	m_pItemBankWindow = NULL;
	m_pVNGainSysInventory = NULL;
	m_pAdminMessageDisplay = NULL;
	m_pInfoDisplay = NULL;
	m_pInfoDisplayEx = NULL;
	m_pInfoDisplayLink = NULL;
	m_pMapMoveDisplay = NULL;
	m_pWaitServerDisplay = NULL;
	m_pAcademyConftDisplay = NULL;
	m_pMapWindowInterface = NULL;
	m_pStorageChargeCard = NULL;
	m_pRebirthDialogue = NULL;
	m_pPrivateMarketWindow = NULL;
	m_pPrivateMarketMake = NULL;
	m_pPrivateMarketSellWindow = NULL;
	m_pClubWindow = NULL;
	m_pPrivateMarketSearchBuy = NULL;
	m_pProgessTimer = NULL;
	m_pRnCDMProgress = NULL;
	m_pRnCDMRankWindow = NULL;
	m_pRnCDMClubInfo = NULL;
	m_pSubMasterSet = NULL;
	m_pClubMake = NULL;
	m_pSystemMessageWindow = NULL;
	m_pQuestionItemDisplay = NULL;
	m_pClubStorageWindow = NULL;
	m_pConftConfirm = NULL;
	m_pCountMsgWindow = NULL;
	m_pGameMenu = NULL;
	m_pQBoxButton = NULL;
	m_pItemRebuildWindow = NULL;	// ITEMREBUILD_MARK
	m_pPetRebirthDialogue = NULL; // monster7j	
	m_pPetWindow = NULL;
	m_pCharacterWindow = NULL;
    m_pReferCharWindow = NULL;
	m_pVehicleColorChangeWindow = NULL;
	m_pBikeGetOffWindow = NULL;

	m_pSystemMessageWindowDummy = NULL;
	m_pMapMoveDisplayDummy = NULL;
	m_pLeftTopGroupDummy = NULL;
	m_pBasicInfoViewDummy = NULL;
	m_pBasicPotionTrayDummy = NULL;
	m_pQuestAlarmDummy = NULL;

	m_pBonusTimeGauge = NULL;
	m_pBonusTimeDisplay = NULL;
	m_pBikeWindow = NULL;
	m_pThaiCCafeMark = NULL;

	m_pItemGarbageWindow = NULL;

	m_pItemShopIconMan = NULL;

	m_pAttendanceBookWindow = NULL;
	m_pClubBattleModalWindow = NULL;

	m_pTaxiWindow = NULL;

	m_bFirstVNGainSysCall = true;	
    m_bInitUpdateOrder = false;

	m_bDuplicateLogin = FALSE;

	m_fItemBankDelay = 5.0f;
	/*m_fVehicleDelay = 0.0f;*/

	m_pItemMixWindow = NULL;
	m_pItemMixInvenWindow = NULL;
	m_pGatherGauge = NULL;
	m_pCdmRankingDisplay = NULL;
	m_pCdmSafeTimeDisplay = NULL;
	m_pSkillTimeDisplay = NULL;

	m_mapLinkData.clear();
	m_mapItemLinkData.clear();
	m_mapPartyLinkData.clear();
	
	m_dwLinkDataIndex = 0;
	m_dwItemLinkDataIndex = 0;
	m_dwPartyLinkDataIndex = 0;

	m_pPetHungry = NULL;
	m_pMouseTip = NULL;

    m_pStudentRecordNotifyButton = NULL;

    // interface pointer reset.
    m_pMiniMap = NULL;
    m_pKeySettingWindow = NULL;
    m_pChatMacroWindow = NULL;
    m_pNameDisplayMan = NULL;
    m_pPrivateMarketShowMan = NULL;
    m_pSummonNameDisplayMan = NULL;
    m_pSkillWindowToTray = NULL;
    m_pPartyWindow = NULL;
	m_pPartyWindowRenewal = NULL;
	m_pPartyStateWindow = NULL;
	m_pPartyTenderWindow = NULL;
	m_pPartyTenderNotifyButton = NULL;
	m_pPartyDistributionWindow = NULL;
	m_pPartyStateWindow = NULL;
	m_pPartyDistributionListWindow = NULL;
	m_pMiniExpeditionWindow = NULL;
	m_pExpeditionInviteWindow = NULL;
    m_pBasicChat = NULL;
    m_pChatOptionWindow = NULL;
    m_pChatTypeList = NULL;
    m_pTradeWindow = NULL;
    m_pTradeInventoryWindow = NULL;
    m_pPtoPWindow = NULL;
    m_pInventoryWindow = NULL;
	m_pInventory = NULL;
    m_pItemMove = NULL;
    m_pMarketWindow = NULL;
    m_pHeadChatDisplayMan = NULL;
    m_pConftModalWindow = NULL;
    m_pPartyModalWindow = NULL;
	m_pChatGroupModalWindow = NULL;
    m_pConftDisplayMan = NULL;
    m_pQuestWindowRenewal = NULL;
    m_pFriendWindow = NULL;
    m_pModalWindow = NULL;
    m_pShopItemSearchWindow = NULL;
    m_pItemSearchResultWindow = NULL;
    m_pMapRequireCheckWindow = NULL;
    m_pPostBoxWindow = NULL;
    m_pPostNotifyButton = NULL;
    m_pQuestHelper = NULL;
    m_pTutorialDialogue = NULL;
    m_pBalloonText = NULL;
    m_pSkillWindow = NULL;
    m_pBasicInfoView = NULL;
    m_pExplainInterface = NULL;
    m_pUILeftTopGroup = NULL;
    m_pSkillTrayTab = NULL;
    m_pBasicQuickSkillSlot = NULL;
    m_pTargetInfoDisplay = NULL;
	m_pCursorTargetInfo = NULL;
    m_pBalloonSpaceBar = NULL;
    m_pTutorialViewStep = NULL;
    m_pLunchBoxTimeDisplay = NULL;
    m_pVehicleWindow = NULL;
    m_pProgressDisplay = NULL;
    m_pStudentRecordWindow = NULL;
    m_pPointShopWindow = NULL;
    m_pRebirthWindow = NULL;
    m_pCompetitionNotifyButton = NULL;
    m_pPopupMenu = NULL;
    m_pInfoCTFDisplay = NULL;
    m_pCTFResultWindow = NULL;
	m_pCompetitionWindow = NULL;
	m_pCompetitionTopRanking = NULL;
	m_pReservedSkillAnimation = NULL;
	m_pProductButton = NULL;
    m_pProductProgress = NULL;
	m_pProductWindow = NULL;
    m_pItemShopWindowWeb = NULL;
    m_pItemInforTooltip = NULL;
    m_pItemInforTooltipWear = NULL;
	m_pItemInforTooltipLink = NULL;
    m_pSkillTooltip = NULL;
	m_pSkillTooltipSubSkill = NULL;
    m_pForceRebirth = NULL;
    m_pNotifyCTFSpurt = NULL;
    m_pDialogueWindowRenewal = NULL;
	m_pPVEUserInterface = NULL;
    m_mmManageControl.clear();
	m_pRnCompetitionWindow = NULL;
    m_pRnCompetitionResultWindow = NULL;
	m_pRnCDMTopRanking = NULL;
    m_pSimpleSystemMessageDisplay = NULL;
    m_pCaptureTheFlagEffecMan = NULL;

	//new instance system
	m_pSimpleSystemMessageDisplay = NULL;
	m_pSimpleTimerMessageBox = NULL;
	m_pRnCDMProgress = NULL;
	m_pRnCDMRankWindow = NULL;
	m_pRnCDMClubInfo = NULL;
	m_pRnCompetitionWindow = NULL;
	m_pRnCompetitionResultWindow = NULL;
	m_pTournamentDashBoard = NULL;
	m_pTournamentUI = NULL;
	m_pTournamentToToWindow = NULL;
	m_pTournamentMoreInfoWindow = NULL;
	m_pRnCDMTopRanking = NULL;
	m_pCaptureTheFlagEffecMan = NULL;
	m_pGuidanceAuthProgressUI = NULL;
	m_pGuidanceProgressInfoUI = NULL;
	m_pGuidanceRankInfoUI = NULL;
	m_pCTIAuthProgressUI = NULL;
	m_pCTIProgressInfoUI = NULL;
	m_pCTIRankInfoUI = NULL;

	m_pTournamentDashBoard	= NULL;
	m_pTournamentUI			= NULL;
	m_pTournamentToToWindow = NULL;
	m_pTournamentMoreInfoWindow = NULL;
	m_pTournamentUserInterface = NULL;
	m_pEntranceConfirm = NULL;
	m_pTournamentMulty = NULL;
	m_pBigGroupWindowRight = NULL;
	m_pBigGroupWindowLeft = NULL;
	m_pTournamentIndicator = NULL;
	m_pMiniGroupWindowLeft = NULL;
	m_pTournamentObserveWindow = NULL;
	m_pMiniGroupWindowRight = NULL;

    SAFE_DELETE( m_pInvenItemTooltipGroup );
    SAFE_DELETE( m_pWishItemTooltipGroup );
    SAFE_DELETE( m_pMarketWishItemTooltipGroup );

    m_vecSkillToolTipRegist.clear();
    m_vecSkillToolTipPool.clear();
    m_vecSkillToolTipUsed.clear();
	//for ( int i=0; i<MAX_GCHAT_SLOT; i ++)
	//{
	//	m_pMaxChatWindow[i] = NULL;
	//}

	m_pSkillTooltipLinkSkill = NULL;
	m_pDoublePoint = NULL;
	m_pSelformBoxWindow = NULL;

	m_pItemInforTooltipDurability = NULL;
}

HRESULT CInnerInterface::OneTimeSceneInit ()
{
	switch ( RENDERPARAM::emSERVICE_TYPE )
	{
	case RENDERPARAM::EMSERVICE_DEFAULT:
	case RENDERPARAM::EMSERVICE_KOREA:
	case RENDERPARAM::EMSERVICE_FEYA:
	case RENDERPARAM::EMSERVICE_MALAYSIA_CN:
	case RENDERPARAM::EMSERVICE_MALAYSIA_EN:
	case RENDERPARAM::EMSERVICE_JAPAN:
	case RENDERPARAM::EMSERVICE_THAILAND:
	case RENDERPARAM::EMSERVICE_CHINA:
	case RENDERPARAM::EMSERVICE_INDONESIA:
	case RENDERPARAM::EMSERVICE_PHILIPPINES:
	case RENDERPARAM::EMSERVICE_VIETNAM:
	case RENDERPARAM::EMSERVICE_GLOBAL:
	case RENDERPARAM::EMSERVICE_EU:
	case RENDERPARAM::EMSERVICE_US:
		m_bCHANNEL = true;
		break;

	default:
		m_bCHANNEL = false;
		break;
	};

	return S_OK;
}

HRESULT CInnerInterface::FrameMove( LPDIRECT3DDEVICEQ pd3dDevice, float fElapsedTime )
{
	GASSERT( pd3dDevice );

	//	무조건 안 보이게 만듦.
	//	사용시에 Update()에서 켜고 동작.
    UiHideGroup( ITEM_INFOR_TOOLTIP );
    UiHideGroup( ITEM_INFOR_TOOLTIP_WEAR );
	UiHideGroup( ITEM_INFOR_TOOLTIP_DURABILITY );
    UiHideGroup( SKILL_INFOR_TOOLTIP );
	UiHideGroup( SKILL_INFOR_TOOLTIP_MULTI );
	UiHideGroup( SKILL_INFOR_TOOLTIP_LINK );
	UiHideGroup( SKILL_INFOR_TOOLTIP_DELAY_ACTION );
	UiHideGroup( INFO_DISPLAY );
	UiHideGroup( INFO_DISPLAY_EX );
	

	ResetTargetInfoMouseOver ();	//	타겟 정보창 리셋
	NSGUI::ResetCharMoveAllBlock();	//	캐릭터의 전체 움직임 잠금 풀기
	NSGUI::ResetCharMoveBlock ();	//	캐릭터 움직임 잠금 풀기	
	ResetSnapItem ();		        //	아이템 스냅 리셋
	ResetFirstItemSlot ();	        //	아이템슬롯 붙기 리셋

	m_bUSING_INFO_DISPLAY = false;
 
	// 아이템 뱅크창 딜레이 측정
	if( !IsBankOpen() && m_fItemBankDelay < 5.0f )
	{
		m_fItemBankDelay += fElapsedTime;
	}

	bool bKEYBOARD_BLOCK = GLInputManager::GetInstance().IsShortcutsBlock();

	// DXInputString* pInputString = m_pEngineDevice->GetInputString();
	//if (UiIsVisibleGroup ( MODAL_WINDOW ) ||
	//	UiIsVisibleGroup ( REBIRTH_DIALOGUE ) ||
	//	//UiIsVisibleGroup ( HELP_WINDOW ) ||	// 웹 브라우저용, 아직 사용 안함
	//	UiIsVisibleGroup ( ITEMSHOP_WINDOW ) ||
	//  UiIsVisibleGroup ( FACEBOOK_WINDOW ) ||
	//  UiIsVisibleGroup ( TWITTER_WINDOW ) ||
	//	pInputString->IsOn())
	//{
	//	bKEYBOARD_BLOCK = true;
	//}

	// WebBrowser가 반투명이 지원되지 않으므로
	// 필요한 상황에서는 HELP_WINDOW를 닫는다
    if (COMMON_WEB::IsCreate())
	{
		if (UiIsVisibleGroup(MODAL_WINDOW) ||
			UiIsVisibleGroup(REBIRTH_DIALOGUE) ||
			UiIsVisibleGroup(ESC_MENU))
		{
			UiHideGroup(HELP_WINDOW);
			UiHideGroup(ITEMSHOP_WINDOW);
		}
	}

	if ( !bKEYBOARD_BLOCK )
		UpdateShortcutBefore();

	HRESULT hr = S_OK;
	hr = uiman::GetInnerUIMan().FrameMove( fElapsedTime, this );

	if( IsOpenWindowToMoveBlock() )
	{
        NSGUI::SetCharMoveBlock();
	}

	UpdateStatus();

	//	더이상 아이템 정보를 표시하지 않을 경우에 지워준다. 
	if ( !m_bUSING_INFO_DISPLAY )
		RESET_INFO();
	if ( !bKEYBOARD_BLOCK )
		UpdateShortcutAfter();

	if ( uiman::GetInnerUIMan().IsExclusiveControl() )
		NSGUI::SetCharMoveBlock();

// 	int LB = false == GfxInterfaceBase::GetInstance()->IsMouseHitUI() && 
// 			 false == GfxInterfaceBase::GetInstance()->IsDragging() ?
// 				(int)DxInputDevice::GetInstance().GetMouseState(DXMOUSE_LEFT) : 0;
// 	if ( (LB & DXKEY_UP) || (LB & DXKEY_DUP) )
// 	{
// 		uiman::GetInnerUIMan().ResetExclusiveControl();
// 	}

	MoveBasicInfoWindow ();

    UpdateFocusOrder ();

	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	m_vCharDir = pCharacter->GetDirect();
// 	Actor& actor = pCharacter->GetActorMove();
// 	if (actor.PathIsActive())
//         m_vCharDir = actor.Movement();

	{
        if ( GLUseFeatures::GetInstance().IsUsingItemTooltip() )
        {
			{
				CUIGroup* pTooltip = (CUIGroup*)uiman::GetInnerUIMan().FindControl(ITEM_INFOR_TOOLTIP);
				if ( pTooltip->IsVisible() == true )
				{
					UIRECT rcAllTooltip = pTooltip->GetGlobalPos();
					const long lResolution = uiman::GetInnerUIMan().GetResolution();
					INT nReleativePosX(rcAllTooltip.left)
						, X_RES(HIWORD ( lResolution ))
						, Y_RES(LOWORD ( lResolution ));

					INT uiidItemTooltip[3] = {
						// ITEM_INFOR_TOOLTIP,
						SKILL_INFOR_TOOLTIP_MULTI,				// ITEM_INFOR_TOOLTIP_LINK,
						ITEM_INFOR_TOOLTIP_DURABILITY,
						ITEM_INFOR_TOOLTIP_WEAR
					};

					D3DXVECTOR2 vArrTooltipPos[3];
					INT nPosY(rcAllTooltip.top);
					for ( INT i = 0; i < 3; ++i )
					{
						pTooltip = (CUIGroup*)uiman::GetInnerUIMan().FindControl(uiidItemTooltip[i]);
						if ( pTooltip->IsVisible() == false )
						{
							vArrTooltipPos[i] = D3DXVECTOR2(0.0f, 0.0f);
							continue;
						}

						UIRECT rcTooltip = pTooltip->GetGlobalPos();

						INT nGap(0), nAddWidth(rcAllTooltip.sizeX + rcTooltip.sizeX + 2);

						vArrTooltipPos[i] = D3DXVECTOR2(nReleativePosX + rcAllTooltip.sizeX + 2, rcTooltip.top);
						nPosY = min(nPosY, rcTooltip.top);
						if ( nReleativePosX + nAddWidth > X_RES )
						{
							nGap = X_RES - (nReleativePosX + nAddWidth);
						}

						if ( nReleativePosX + nGap < 0 )
						{
							nGap = -nReleativePosX;
						}

						nReleativePosX += nGap;

						for ( INT j = i; j >= 0; --j )
						{
							vArrTooltipPos[j].x += nGap;
						}

						rcAllTooltip.SetPosition(nReleativePosX, rcAllTooltip.top
							, nAddWidth
							, rcAllTooltip.sizeY);
					}

					UIRECT rcNormalInfo = m_pItemInforTooltip->GetGlobalPos();
					m_pItemInforTooltip->SetGlobalPos( D3DXVECTOR2(nReleativePosX, nPosY) );
					for ( INT i = 0; i < 3; ++i )
					{
						pTooltip = (CUIGroup*)uiman::GetInnerUIMan().FindControl(uiidItemTooltip[i]);
						if ( pTooltip->IsVisible() )
						{
							UIRECT rcTooltip = pTooltip->GetGlobalPos();
							pTooltip->SetGlobalPos( D3DXVECTOR2(vArrTooltipPos[i].x, nPosY) );
						}
					}
				}
			}


			/*
            if ( UiIsVisibleGroup( ITEM_INFOR_TOOLTIP_WEAR ) ) 
            {
                UIRECT rcGlobalPos = m_pItemInforTooltip->GetGlobalPos();
                UIRECT rcGlobalPosEx = m_pItemInforTooltipWear->GetGlobalPos();

                float x = rcGlobalPos.right + 2.0f;
                float y = min(rcGlobalPos.top,rcGlobalPosEx.top);

                const long lResolution = uiman::GetInnerUIMan().GetResolution();
                WORD X_RES = HIWORD ( lResolution );
                WORD Y_RES = LOWORD ( lResolution );

                if ( X_RES < x + rcGlobalPosEx.sizeX )
                {
                    x = X_RES - rcGlobalPosEx.sizeX;

                    m_pItemInforTooltip->SetGlobalPos(D3DXVECTOR2(x-2.0f-rcGlobalPos.sizeX,y));
                    m_pItemInforTooltipWear->SetGlobalPos(D3DXVECTOR2(x,y));
                }
                else
                {
                    m_pItemInforTooltip->SetGlobalPos(D3DXVECTOR2(rcGlobalPos.left,y));
                    m_pItemInforTooltipWear->SetGlobalPos(D3DXVECTOR2(x,y));
                }
            }

			if ( UiIsVisibleGroup( SKILL_INFOR_TOOLTIP_MULTI ) ) 
			{
				if ( UiIsVisibleGroup( ITEM_INFOR_TOOLTIP ) ) 
				{
					UIRECT rcGlobalPos = m_pItemInforTooltip->GetGlobalPos();
					UIRECT rcGlobalPosEx = m_pSkillTooltipSubSkill->GetGlobalPos();

					float x = rcGlobalPos.right + 2.0f;
					float y = min(rcGlobalPos.top,rcGlobalPosEx.top);

					const long lResolution = uiman::GetInnerUIMan().GetResolution();
					WORD X_RES = HIWORD ( lResolution );
					WORD Y_RES = LOWORD ( lResolution );

					if ( X_RES < x + rcGlobalPosEx.sizeX )
					{
						x = X_RES - rcGlobalPosEx.sizeX;

						m_pItemInforTooltip->SetGlobalPos(D3DXVECTOR2(x-2.0f-rcGlobalPos.sizeX,y));
						m_pSkillTooltipSubSkill->SetGlobalPos(D3DXVECTOR2(x,y));
					}
					else
					{
						m_pItemInforTooltip->SetGlobalPos(D3DXVECTOR2(rcGlobalPos.left,y));
						m_pSkillTooltipSubSkill->SetGlobalPos(D3DXVECTOR2(x,y));
					}
				}
			}

			if ( UiIsVisibleGroup( ITEM_INFOR_TOOLTIP_DURABILITY ) ) 
			{
				if( UiIsVisibleGroup( SKILL_INFOR_TOOLTIP_MULTI ) )
				{
					UIRECT rcGlobalPos = m_pSkillTooltipSubSkill->GetGlobalPos();
					UIRECT rcGlobalPosEx = m_pItemInforTooltipDurability->GetGlobalPos();

					float x = rcGlobalPos.right + 2.0f;
					float y = min(rcGlobalPos.top,rcGlobalPosEx.top);

					const long lResolution = uiman::GetInnerUIMan().GetResolution();
					WORD X_RES = HIWORD ( lResolution );
					WORD Y_RES = LOWORD ( lResolution );

					if ( X_RES < x + rcGlobalPosEx.sizeX )
					{
						x = X_RES - rcGlobalPosEx.sizeX;

						m_pSkillTooltipSubSkill->SetGlobalPos(D3DXVECTOR2(x-2.0f-rcGlobalPos.sizeX,y));
						m_pItemInforTooltipDurability->SetGlobalPos(D3DXVECTOR2(x,y));
					}
					else
					{
						m_pSkillTooltipSubSkill->SetGlobalPos(D3DXVECTOR2(rcGlobalPos.left,y));
						m_pItemInforTooltipDurability->SetGlobalPos(D3DXVECTOR2(x,y));
					}
				}
				else
				{
					UIRECT rcGlobalPos = m_pItemInforTooltip->GetGlobalPos();
					UIRECT rcGlobalPosEx = m_pItemInforTooltipDurability->GetGlobalPos();

					float x = rcGlobalPos.right + 2.0f;
					float y = min(rcGlobalPos.top,rcGlobalPosEx.top);

					const long lResolution = uiman::GetInnerUIMan().GetResolution();
					WORD X_RES = HIWORD ( lResolution );
					WORD Y_RES = LOWORD ( lResolution );

					if ( X_RES < x + rcGlobalPosEx.sizeX )
					{
						x = X_RES - rcGlobalPosEx.sizeX;

						m_pItemInforTooltip->SetGlobalPos(D3DXVECTOR2(x-2.0f-rcGlobalPos.sizeX,y));
						m_pItemInforTooltipDurability->SetGlobalPos(D3DXVECTOR2(x,y));
					}
					else
					{
						m_pItemInforTooltip->SetGlobalPos(D3DXVECTOR2(rcGlobalPos.left,y));
						m_pItemInforTooltipDurability->SetGlobalPos(D3DXVECTOR2(x,y));
					}	
				}				
			}
			
        }
		*/
		}
        else
        {
		    if ( UiIsVisibleGroup ( INFO_DISPLAY_EX ) ) 
		    {
			    UIRECT rcGlobalPos = m_pInfoDisplay->GetGlobalPos();
			    UIRECT rcGlobalPosEx = m_pInfoDisplayEx->GetGlobalPos();

			    float x = rcGlobalPos.right + 2.0f;
			    float y = min(rcGlobalPos.top,rcGlobalPosEx.top);

			    const long lResolution = uiman::GetInnerUIMan().GetResolution();
			    WORD X_RES = HIWORD ( lResolution );
			    WORD Y_RES = LOWORD ( lResolution );

			    if ( X_RES < x + rcGlobalPosEx.sizeX )
			    {
				    x = X_RES - rcGlobalPosEx.sizeX;

				    m_pInfoDisplay->SetGlobalPos(D3DXVECTOR2(x-2.0f-rcGlobalPos.sizeX,y));
				    m_pInfoDisplayEx->SetGlobalPos(D3DXVECTOR2(x,y));
			    }
			    else
			    {
				    m_pInfoDisplay->SetGlobalPos(D3DXVECTOR2(rcGlobalPos.left,y));
				    m_pInfoDisplayEx->SetGlobalPos(D3DXVECTOR2(x,y));
			    }
		    }
        }
	}

	{
		if (pCharacter->VehicleState() == EMVEHICLE_DRIVER && pCharacter->VehicleBoosterId())
			UiShowGroupTop( BIKE_BOOST_WINDOW);
		else
			UiHideGroup( BIKE_BOOST_WINDOW );
	}

	{
		if (pCharacter->VehicleState() ==  EMVEHICLE_DRIVER )
		{
			INT nNumber = pCharacter->VehicleePassengerNum();
			if( nNumber > 1 )
			{
				//if( UiIsVisibleGroup ( BIKE_GET_OFF_WINDOW ) )
				{
					const long lResolution = uiman::GetInnerUIMan().GetResolution();
					WORD X_RES = HIWORD ( lResolution );
					WORD Y_RES = LOWORD ( lResolution );

                    UIRECT rcGlobalPos = m_pBikeGetOffWindow->GetGlobalPos();
					D3DXVECTOR2 vNewPos;
					vNewPos.x = (X_RES - rcGlobalPos.sizeX) / 2.0f;
					vNewPos.y = ( Y_RES - 100.0f );

					m_pBikeGetOffWindow->SetGlobalPos( vNewPos );
					UiShowGroupTop( BIKE_GET_OFF_WINDOW );
				}
			}
			else
			{
				if( UiIsVisibleGroup ( BIKE_GET_OFF_WINDOW ) )
				{
					UiHideGroup( BIKE_GET_OFF_WINDOW );
					UiHideGroup ( BIKE_MESSAGE_WINDOW );
				}
			}

		}
		else if( m_pGaeaClient->GetCharacter()->VehicleState() ==  EMVEHICLE_PASSENGER )
		{
			//if( UiIsVisibleGroup ( BIKE_GET_OFF_WINDOW ) )
			{
				const long lResolution = uiman::GetInnerUIMan().GetResolution();
				WORD X_RES = HIWORD ( lResolution );
				WORD Y_RES = LOWORD ( lResolution );
				{
					UIRECT rcGlobalPos = m_pBikeGetOffWindow->GetGlobalPos();
					D3DXVECTOR2 vNewPos;
					vNewPos.x = (X_RES - rcGlobalPos.sizeX) / 2.0f;
					vNewPos.y = ( Y_RES - 100.0f );

					m_pBikeGetOffWindow->SetGlobalPos( vNewPos );
					UiShowGroupTop( BIKE_GET_OFF_WINDOW );
				}

				{
					UIRECT rcGlobalPos = m_pBikeMessageWindow->GetGlobalPos();
					D3DXVECTOR2 vNewPos;
					vNewPos.x = (X_RES - rcGlobalPos.sizeX) / 2.0f;
					vNewPos.y = rcGlobalPos.top;

					m_pBikeMessageWindow->SetGlobalPos( vNewPos );
					UiShowGroupFocus( BIKE_MESSAGE_WINDOW );
				}				
			}
		}
		else
		{
			if( UiIsVisibleGroup ( BIKE_GET_OFF_WINDOW ) )
			{
				UiHideGroup( BIKE_GET_OFF_WINDOW );
				UiHideGroup ( BIKE_MESSAGE_WINDOW );
			}
		}
	}

	if( m_pCursorTargetInfo )
		m_pCursorTargetInfo->UpdateInfo();

	// Lotto System
	GLLottoSystemManUI::Instance()->Update( fElapsedTime );

	if ( m_pGaeaClient )
	{
		GLCharacter* pChar = m_pGaeaClient->GetCharacter();
		if ( pChar )
		{
			if ( !pChar->m_sSummonable.bSummonable )
			{
				if ( UiIsVisibleGroup ( SUMMON_STATE_DISPLAY_UI ) )
					UiHideGroup( SUMMON_STATE_DISPLAY_UI );
			}
			else
			{
				if ( FALSE == UiIsVisibleGroup ( SUMMON_STATE_DISPLAY_UI ) )
					UiShowGroupFocus( SUMMON_STATE_DISPLAY_UI );
			}
		}
	}

	return hr;
}

void CInnerInterface::UpdateShortcutBefore ()
{	
	{
		if ( !m_pNameDisplayMan ) return ;		

		//if ( UIKeyCheck::GetInstance()->CheckSimple ( DIK_LMENU, DXKEY_PRESSED ) )
		//{
		//	for (int i= 0; i < CHAT_MACRO_SIZE; ++i){
		//		if ( UIKeyCheck::GetInstance()->CheckSimple ( DIK_1+i, DXKEY_DOWN ) )
		//		{			
		//			AddChatMacro(i);
		//			break;
		//		}
		//	}
		//}



		if( RANPARAM::bNAME_DISPLAY )
		{
			if( UiIsVisibleGroup( NAME_DISPLAY_MAN ) )
			{
				if( m_pNameDisplayMan->GetNameType() != NAME_DISPLAY_MOBCHAR )
				{
					UiHideGroup( NAME_DISPLAY_MAN );
					m_pNameDisplayMan->SetNameType( NAME_DISPLAY_MOBCHAR );
					UiShowGroupBottom( NAME_DISPLAY_MAN );
				}
			}
			else
			{
				m_pNameDisplayMan->SetNameType( NAME_DISPLAY_MOBCHAR );
				UiShowGroupBottom( NAME_DISPLAY_MAN );
			}

			return ;
		}

		UiHideGroup ( NAME_DISPLAY_MAN );		
		if ( UIKeyCheck::GetInstance()->Check ( DIK_LMENU, DXKEY_PRESSED ) )
		{
			m_pNameDisplayMan->SetNameType ( NAME_DISPLAY_ITEMMONEY );
			UiShowGroupBottom ( NAME_DISPLAY_MAN );
		}
		else if ( UIKeyCheck::GetInstance()->Check ( DIK_LCONTROL, DXKEY_PRESSED ) )
		{
			m_pNameDisplayMan->SetNameType ( NAME_DISPLAY_MOBCHAR );
			UiShowGroupBottom ( NAME_DISPLAY_MAN );
		}
		else if ( UIKeyCheck::GetInstance()->Check ( DIK_LSHIFT, DXKEY_PRESSED ) )
		{
			m_pNameDisplayMan->SetNameType ( NAME_DISPLAY_ALL );
			UiShowGroupBottom ( NAME_DISPLAY_MAN );
		}
		else
		{
			//	BEGIN : CLUB CONFT SETTING
			EMCONFT_TYPE emTYPE = m_pGaeaClient->GetCharacter()->GetConftingType();
			if ( emTYPE == EMCONFT_GUILD )
			{
				m_pNameDisplayMan->SetNameType ( NAME_DISPLAY_CLUB_CONFT );
				UiShowGroupBottom ( NAME_DISPLAY_MAN );                
				return ;
			}

			//	파티
			GLPartyClient* const pMyParty = m_pGaeaClient->GetMyPartyClient(); //	마스터
			if ( pMyParty->isValid() == true )
			{				
				if (m_pGaeaClient->GetCharacter()->IsConfting())	//	파티 대련 중인가?
				{
					m_pNameDisplayMan->SetNameType ( NAME_DISPLAY_PARTY_CONFT );			
				}
				else	//	파티 구성 된 경우
				{
					m_pNameDisplayMan->SetNameType ( NAME_DISPLAY_PARTY );		
				}
				UiShowGroupBottom ( NAME_DISPLAY_MAN );
			}
			else
			{
				//	무조건 뿌림
				m_pNameDisplayMan->SetNameType ( NAME_DISPLAY_DEFAULT );
				UiShowGroupBottom ( NAME_DISPLAY_MAN );
			}
		}
	}	
}

void CInnerInterface::UpdateShortcutAfter ()
{
	// bjju.ControlType
	// 단축키들이 RanLogicClient/GLControlBase.cpp 로 옮겨짐
	// 11-10-31

	if ( UiIsVisibleGroup ( ESC_MENU ) )			return ;
	if ( UiIsVisibleGroup ( HELP_WINDOW ) )			return ;
	if ( UiIsVisibleGroup ( ITEMSHOP_WINDOW ) )		return ;
	if ( UiIsVisibleGroup ( KEY_SETTING_WINDOW ) )	return ;


	GLCharacter* const pCharacter = m_pGaeaClient->GetCharacter ();
	//if ( pCharacter )
	//{
	//	//	퀵 스킬
	//	for( int i = 0; i < QUICK_SKILL_SLOT_MAX; ++i)
	//	{
 //          		 if ( UIKeyCheck::GetInstance()->Check ( RANPARAM::SkillSlot[i], DXKEY_DOWN ) )
	//		{
	//			INT nIndex = m_pSkillTrayTab->GetTabIndex() * QUICK_SKILL_SLOT_MAX + i;
	//			if ( S_OK == pCharacter->ReqSkillRunSet( nIndex ) )
	//			{	
	//				// 스킬을 변경시에 서브 무기에 해당 스킬을 사용할 수 있는 무기가 
	//				// 장착중이라면 변경한다. 단, 이미 스왑 기능 예약시에는 하지 않는다.
	//				AutoArmSwap();
	//			}
	//		}
	//	}
	//}


	////	탭키 예약 ( 이미 예약중이면 동작 X )
	//if ( UIKeyCheck::GetInstance()->Check( DIK_TAB, DXKEY_DOWN ) && !m_bArmsSwapReserve )
	//{
	//	EMCHARCLASS emClass = m_pGaeaClient->GetCharacter()->m_emClass;

	//	if( emClass&GLCC_EXTREME_M || emClass&GLCC_EXTREME_W )
	//	{
	//		m_bArmsSwapReserve = true;

	//		// 미리 스킬 슬롯을 체인지 한다. AutoArmSwap기능이랑 중복 방지 위해
 //           bool bMain = (0!=pCharacter->IsUseArmSub());
	//		AutoSkillSolt( bMain );
	//	}
	//}

	////	탭키 예약 중이면
	//if ( m_bArmsSwapReserve ) 
	//{
	//	if ( S_OK == m_pGaeaClient->GetCharacter()->ReqSlotChange() ) 
	//	{
	//		m_bArmsSwapReserve = false;
	//		// 스킬 슬롯 체인지
	//	}
	//}

	//if ( UIKeyCheck::GetInstance()->CheckSimple ( DIK_V, DXKEY_DOWN ) )
	//{
	//	if (m_pGaeaClient->GetTradeClient()->Valid())
 //           return;

	//	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	//	if (pCharacter->VehicleActiveValue() && m_fVehicleDelay < 1.0f)
	//	{
	//		PrintMsgText(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("VEHICLE_SET_FB_DELAY"));
	//		return;
	//	}

	//	pCharacter->ReqSetVehicle(!pCharacter->VehicleState());
 //       m_fVehicleDelay = 0.0f;
	//}

	if ( UIKeyCheck::GetInstance()->CheckSimple ( DIK_LMENU, DXKEY_PRESSED ) )
	{
		for (int i= 0; i < CHAT_MACRO_SIZE; ++i)
		{
			if ( UIKeyCheck::GetInstance()->CheckSimple ( DIK_1+i, DXKEY_DOWN ) )
			{			
				return ;
			}
		}
	}
	
	//if ( UIKeyCheck::GetInstance()->Check ( RANPARAM::MenuShotcut[SHOTCUT_INVEN], DXKEY_DOWN ) )
	//{		
	//	if ( !UiIsVisibleGroup ( INVENTORY_WINDOW ) && !UiIsVisibleGroup ( TRADEINVENTORY_WINDOW ) )
	//	{
	//		UiShowGroupFocus( INVENTORY_WINDOW );
	//	}
	//	else
	//	{
	//		UiHideGroup ( INVENTORY_WINDOW );
	//	}
	//}


	//if ( UIKeyCheck::GetInstance()->Check ( RANPARAM::MenuShotcut[SHOTCUT_SKILL], DXKEY_DOWN ) )
	//{		
	//	if ( UiIsVisibleGroup( STATSRESET_WINDOW ) )
	//	{
	//		PrintMsgText( NS_UITEXTCOLOR::RED, ID2GAMEINTEXT("STATSRESET_ACTION_WARNING") );
	//	}
	//	else 
	//	{
	//		if ( !UiIsVisibleGroup ( SKILL_WINDOW ) )
	//			UiShowGroupFocus ( SKILL_WINDOW );
	//		else
	//			UiHideGroup ( SKILL_WINDOW );
	//	}
	//}

	//if ( UIKeyCheck::GetInstance()->Check ( RANPARAM::MenuShotcut[SHOTCUT_PARTY], DXKEY_DOWN ) )
	//{		
	//	if ( !UiIsVisibleGroup ( PARTY_WINDOW ) )
	//		UiShowGroupFocus ( PARTY_WINDOW );
	//	else
	//		UiHideGroup ( PARTY_WINDOW );
	//}

	//if ( UIKeyCheck::GetInstance()->Check ( RANPARAM::MenuShotcut[SHOTCUT_CHAR], DXKEY_DOWN ) )
	//{		
	//	if ( UiIsVisibleGroup( STATSRESET_WINDOW ) )
	//	{
	//		PrintMsgText( NS_UITEXTCOLOR::RED, ID2GAMEINTEXT("STATSRESET_ACTION_WARNING") );
	//	}
	//	else 
	//	{
	//		if ( !UiIsVisibleGroup ( CHARACTER_WINDOW ) )
	//			UiShowGroupFocus ( CHARACTER_WINDOW );
	//		else
	//			UiHideGroup ( CHARACTER_WINDOW );
	//	}
	//}

	//if ( UIKeyCheck::GetInstance()->CheckSimple ( RANPARAM::MenuShotcut[SHOTCUT_MAP], DXKEY_DOWN ) )
	//{
	//	if( m_pGaeaClient->IsInstantMap() )
	//	{
	//		UiHideGroup ( LARGEMAP_WINDOW );
	//		PrintMsgText (NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("MINIMAP_CANNOTSEE") );
	//	}else{
	//		if ( !UiIsVisibleGroup ( LARGEMAP_WINDOW ) )
	//			UiShowGroupFocus ( LARGEMAP_WINDOW );
	//		else
	//			UiHideGroup ( LARGEMAP_WINDOW );
	//	}
	//}

	//if ( UIKeyCheck::GetInstance()->CheckSimple ( RANPARAM::MenuShotcut[SHOTCUT_CLUB], DXKEY_DOWN ) )
	//{
	//	if ( !UiIsVisibleGroup ( CLUB_WINDOW ) )
	//		UiShowGroupFocus ( CLUB_WINDOW );
	//	else
	//		UiHideGroup ( CLUB_WINDOW );
	//}

	//if ( UIKeyCheck::GetInstance()->Check ( RANPARAM::MenuShotcut[SHOTCUT_QUEST], DXKEY_DOWN ) )
	//{
	//	if ( !UiIsVisibleGroup ( QUEST_WINDOW ) )
	//	{			
	//		if ( NATIVEID_NULL().dwID == m_dwEventQuestID )
	//		{
	//			UiShowGroupFocus ( QUEST_WINDOW );
	//			REFRESH_QUEST_WINDOW ();				
	//		}
	//		else
	//		{
	//			SetQuestWindowOpen ( m_dwEventQuestID );
	//		}
	//	}
	//	else
	//	{
	//		UiHideGroup ( QUEST_WINDOW );
	//	}
	//}

	//if ( UIKeyCheck::GetInstance()->Check ( RANPARAM::MenuShotcut[SHOTCUT_HELP], DXKEY_DOWN) )
	//{
	//	if ( !UiIsVisibleGroup ( HELP_WINDOW ) )
	//	{
	//		UiHideGroup ( ITEMSHOP_WINDOW );
	//		UiShowGroupFocus ( HELP_WINDOW );
	//	}
	//	else
	//	{
	//		UiHideGroup ( HELP_WINDOW );
	//	}
	//}

    //// bjju.sns
    //if ( UIKeyCheck::GetInstance()->Check ( RANPARAM::MenuShotcut[SHOTCUT_SNS], DXKEY_DOWN) )
    //{
    //    if (GLUseFeatures::GetInstance().IsUsingSNS() )
    //    {
    //        if ( !UiIsVisibleGroup ( SNS_WINDOW ) )
    //        {
    //            UiShowGroupFocus ( SNS_WINDOW );
    //        }
    //        else
    //        {
    //            UiHideGroup ( SNS_WINDOW );
    //        }
    //    }
    //}

    //if ( UIKeyCheck::GetInstance()->Check ( RANPARAM::MenuShotcut[SHOTCUT_POINT_SHOP], DXKEY_DOWN) )
    //{
    //    if ( GLUseFeatures::GetInstance().IsUsingPointShop() )
    //    {
    //        if ( !UiIsVisibleGroup( POINT_SHOP_WINDOW ) )
    //        {
    //            UiShowGroupFocus( POINT_SHOP_WINDOW );

    //            GetPointShopWindow()->RefreshWindow();
    //        }
    //        else
    //        {
    //            UiHideGroup( POINT_SHOP_WINDOW );
    //        }
    //    }
    //}


//#if defined ( CH_PARAM ) || defined ( TH_PARAM ) || defined( PH_PARAM ) || defined ( JP_PARAM ) || defined(_RELEASED) // 필리핀 아이템샵
//	if ( UIKeyCheck::GetInstance()->Check ( RANPARAM::MenuShotcut[SHOTCUT_POINT_SHOP], DXKEY_DOWN) )
//	{
//		if ( !UiIsVisibleGroup ( ITEMSHOP_WINDOW ) )
//		{
//			//	ItemShopAuth
//			if ( ItemShopAuth() )
//			{
//				UiShowGroupFocus ( ITEMSHOP_WINDOW );
//			}
//
//			UiHideGroup ( HELP_WINDOW );
//		}
//		else
//		{
//			UiHideGroup ( ITEMSHOP_WINDOW );
//		}
//	}
//	
//	//	ItemShopAuth	
//	ItemShopVisible();
//
//#endif	

	//if ( UIKeyCheck::GetInstance()->Check ( RANPARAM::MenuShotcut[SHOTCUT_FRIEND], DXKEY_DOWN ) )
	//{
	//	if ( UiIsVisibleGroup ( FRIEND_WINDOW ) )		UiHideGroup ( FRIEND_WINDOW );			
	//	else										UiShowGroupFocus ( FRIEND_WINDOW );
	//}

	//if ( UIKeyCheck::GetInstance()->Check ( RANPARAM::MenuShotcut[SHOTCUT_ITEMBANK], DXKEY_DOWN ) )
	//{
	//	if ( UiIsVisibleGroup ( ITEMBANK_WINDOW ) )	UiHideGroup ( ITEMBANK_WINDOW );
	//	else										SetItemBankWindowOpen ();

	//	/*if ( !UiIsVisibleGroup ( ITEMBANK_WINDOW ) )	
	//	{
	//		SetItemBankWindowOpen();			
	//	}*/
	//}

	/*if ( UIKeyCheck::GetInstance()->Check ( RANPARAM::MenuShotcut[SHOTCUT_VIETNAMGAINSYS], DXKEY_DOWN ) )
	{
		if ( UiIsVisibleGroup ( VNGAINSYS_WINDOW ) )	UiHideGroup ( VNGAINSYS_WINDOW );
		else										SetVNGainSysWindowOpen();
	}*/

	//if ( UIKeyCheck::GetInstance()->Check ( RANPARAM::MenuShotcut[SHOTCUT_CHATMACRO], DXKEY_DOWN ) )
	//{
	//	if ( UiIsVisibleGroup ( CHATMACRO_WINDOW ) )
	//	{
	//		UiHideGroup ( CHATMACRO_WINDOW );
	//	}
	//	else
	//	{
	//		GetChatMacro();
	//		UiShowGroupFocus ( CHATMACRO_WINDOW );
	//	}
	//}


	// 스킬 슬롯 F4키를 할당하면서 이부분은 삭제 한다.
/*
	if ( UIKeyCheck::GetInstance()->CheckSimple ( DIK_LMENU, DXKEY_PRESSED ) )
	{
		if ( UIKeyCheck::GetInstance()->CheckSimple ( DIK_F4, DXKEY_DOWN ) )
		{			
			DoModal ( ID2GAMEINTEXT("CLOSE_GAME"), UI::MODAL_QUESTION, UI::OKCANCEL, UI::MODAL_CLOSEGAME );
		}
	}
*/


////#ifndef CH_PARAM // 중국 인터페이스 변경
//	if( UIKeyCheck::GetInstance()->Check ( DIK_GRAVE, DXKEY_DOWN ) )
////		UIKeyCheck::GetInstance()->Check ( DIK_Y, DXKEY_DOWN ) )
//	{
//		if ( m_pGaeaClient->GetTutorial()->IsTutorial() ) // 튜토리얼 중에는 사용금지.
//			PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("TUTORIAL_CANNOT_USE") );
//		else
//		{
//			SkilSlotTrayToggle();
//		}
//	}
////#endif

	//if( UIKeyCheck::GetInstance()->Check( DIK_F1, DXKEY_DOWN ) )	
	//	m_pSkillTrayTab->SetTabIndex( 0 );
	//if( UIKeyCheck::GetInstance()->Check( DIK_F2, DXKEY_DOWN ) )	
	//	m_pSkillTrayTab->SetTabIndex( 1 );
	//if( UIKeyCheck::GetInstance()->Check( DIK_F3, DXKEY_DOWN ) )	
	//	m_pSkillTrayTab->SetTabIndex( 2 );
	//if( UIKeyCheck::GetInstance()->Check( DIK_F4, DXKEY_DOWN ) )	
	//	m_pSkillTrayTab->SetTabIndex( 3 );

	//if( UIKeyCheck::GetInstance()->Check( RANPARAM::MenuShotcut[SHOTCUT_PET], DXKEY_DOWN ) )
	//{
	//	if( m_pGaeaClient->GetPetClient()->IsVALID() )
	//	{
	//		if ( m_pGaeaClient->GetTradeClient()->Valid() ) return;
	//		
	//		if ( UiIsVisibleGroup( PET_WINDOW ) )		UiHideGroup ( PET_WINDOW );
	//		else									UiShowGroupFocus( PET_WINDOW );			
	//	}
	//}

    //if( UIKeyCheck::GetInstance()->Check( DIK_Y, DXKEY_DOWN ) )
    //{
    //    if ( IsStudentRecordOpenable() )
    //    {
    //        if ( UiIsVisibleGroup ( STUDENTRECORD_WINDOW ) )
    //        {
    //            UiHideGroup ( STUDENTRECORD_WINDOW );
    //        }
    //        else
    //        {
    //            UiShowGroupFocus ( STUDENTRECORD_WINDOW );
    //        }
    //    }
    //}

	//if( UIKeyCheck::GetInstance()->Check( DIK_U, DXKEY_DOWN ) )
	//{
	//	if ( m_pCompetitionWindow )
	//	{
	//		if ( !CompetitionWindowIsOpen() )
	//		{
	//			CompetitionWindowOpen();
	//		}
	//		else
	//		{
	//			CompetitionWindowClose();
	//		}
	//	}
	//}

	//if( UIKeyCheck::GetInstance()->Check( DIK_N, DXKEY_DOWN ) )
	//{
 //       GLCharacter* pChar = m_pGaeaClient->GetCharacter();
	//	if (pChar->VehicleActiveValue())
	//	{
	//		if (m_pGaeaClient->GetTradeClient()->Valid())
 //               return;
	//		
	//		SITEMCUSTOM& sItemCustom = pChar->m_PutOnItems[SLOT_VEHICLE];
	//		if ( sItemCustom.sNativeID == NATIVEID_NULL() )
 //               return;
	//		if ( !pChar->VehicleActiveValue())
 //               return;


	//		SNATIVEID sNativeID = pChar->VehicleId();
	//		SITEM* pItem = GLogicData::GetInstance().GetItem( sNativeID );
	//		if (!pItem)
 //               return;

	//		if( pItem->sVehicle.emVehicleType == VEHICLE_TYPE_BIKE )
	//		{
	//			if ( UiIsVisibleGroup( BIKE_WINDOW ) )	
	//			{
	//				UiHideGroup ( BIKE_WINDOW );
	//			}
	//			else									
	//			{
	//				UiShowGroupFocus( BIKE_WINDOW );
	//			}
	//		}
	//		else if( pItem->sVehicle.emVehicleType == VEHICLE_TYPE_BOARD )
	//		{
	//			if ( UiIsVisibleGroup( VEHICLE_WINDOW ) )	
	//			{
	//				UiHideGroup ( VEHICLE_WINDOW );
	//			}
	//			else									
	//			{
	//				UiShowGroupFocus( VEHICLE_WINDOW );
	//			}
	//		}			
	//	}
	//}

	//if( UIKeyCheck::GetInstance()->Check( RANPARAM::MenuShotcut[SHOTCUT_QBOX], DXKEY_DOWN ) )
	//{
	//	m_pQBoxButton->QBOXButtonDown();
	//}
}

void CInnerInterface::UpdateStatus ()
{
	GLCharacter* const pCharacter = m_pGaeaClient->GetCharacter();

	if ( UiIsVisibleGroup ( SYSTEM_MESSAGE_WINDOW ) )
	{
		UIRECT rcGlobalPos = m_pSystemMessageWindowDummy->GetGlobalPos ();
		//rcGlobalPos.top += fMENU_LIFTUP;
		m_pSystemMessageWindow->SetGlobalPos ( rcGlobalPos );
	}	
	
	
	//	캐릭터 생존 상태 확인
	if ( pCharacter->IsDie () && m_pRebirthWindow->CapableVisible() )
	{
        if ( !UiIsVisibleGroup ( REBIRTH_WINDOW ) )
		{
			SITEM* pReviveItem = pCharacter->FindRevive();

			bool bItemRebirth = ( NULL != pReviveItem ) ? true : false;
			bool bAutoRebirth = false;
			bool bCantUseRebirth = true;

			//	귀혼주 쿨 타임 체크
			if( true == bItemRebirth ) 
			{
				if( NULL != pReviveItem &&
					true == pCharacter->IsCoolTime( pReviveItem->sBasicOp.sNativeID ) )
				{
					bItemRebirth = false;
				}
			}

			if( pCharacter->IsRestartPossible() == FALSE )
			{
				bItemRebirth = false;
				bCantUseRebirth = true;
			}

			if( true == bItemRebirth )
			{
				// 소모성 귀혼주이면 자동 귀혼주 사용
				if( NULL  != pReviveItem &&
					false == pReviveItem->ISINSTANCE() )
					bAutoRebirth = true;
			}

            if ( m_pForceRebirth->IsForceRebirth() )
            {
                const long lResolution = uiman::GetInnerUIMan().GetResolution();
                WORD X_RES = HIWORD( lResolution );
                WORD Y_RES = LOWORD( lResolution );

                const UIRECT& rcLP = m_pForceRebirth->GetLocalPos();

                D3DXVECTOR2 vPos;
                vPos.x = ( X_RES - rcLP.sizeX ) * 0.5f;
                vPos.y = Y_RES * 0.6f;
                m_pForceRebirth->SetGlobalPos( vPos );

                UiShowGroupFocus( FORCE_REBIRTH, true );
            }
            else
            {
                bool bItemRebirth = (pCharacter->FindRevive ())?true:false;
                bool bAutoRebirth = false;
                bool bCantUseRebirth = FALSE;

                //	귀혼주 쿨 타임 체크
                if ( bItemRebirth ) 
                {
                    SITEM* pITEM = pCharacter->GET_SLOT_ITEMDATA(SLOT_DECORATION);
                    if ( pITEM && pCharacter->IsCoolTime( pITEM->sBasicOp.sNativeID ) )
                    {
                        bItemRebirth = FALSE;
                    }
                }

                if( pCharacter->IsRestartPossible() == FALSE )
                {
                    bItemRebirth = FALSE;
                    bCantUseRebirth = TRUE;
                }

                if ( bItemRebirth )
                {
                    // 소모성 귀혼주이면 자동 귀혼주 사용
                    SITEM* pITEM = pCharacter->GET_SLOT_ITEMDATA(SLOT_DECORATION);
                    if ( pITEM && !pITEM->ISINSTANCE() ) bAutoRebirth = true;
                }

                m_pRebirthWindow->SetItemRebirth(pCharacter->m_sResurrectAbleState.isState(EM_RESURRECTION_SPECIAL_ITEM), false, false);

                UiShowGroupFocus( REBIRTH_WINDOW, true );
            }

            UiShowGroupFocus( BASIC_CHAT_BOX, true );

            if ( GLUseFeatures::GetInstance().IsUsingGroupChat() && (!UiIsVisibleGroup ( MIN_GROUP_CHAT_WINDOW )) )
            {
                UiShowGroupFocus ( MIN_GROUP_CHAT_WINDOW, true );
            }

            TriggerSystem::SClientRebirthMessage msg;
            m_pGaeaClient->SendTriggerMessage(&msg);
		}
	}
	else
	{
		UiHideGroup ( REBIRTH_DIALOGUE, true );
		
		if ( IsGateOpen () )
		{			
			MAPNAMEVECTOR mapNameVector;
			pCharacter->DetectGateToMapName(mapNameVector);
			switch ( mapNameVector.size() )
			{
			case 0:
				UiHideGroup( MAPMOVE_DISPLAY );
				UiHideGroup ( MULTIMAPMOVE_DISPLAY );
				break;
			case 1:
				{ //	만약 맵 이름이 없다면, 이동할 수 없다.					
					if ( m_pMapMoveDisplay == NULL )
						break;
					
					GLCharacter* pChar(m_pGaeaClient->GetCharacter());
					
					if ( pChar != NULL )
					{
						switch ( pChar->VehicleState() )
						{
						case EMVEHICLE_DRIVER:
						case EMVEHICLE_OFF:
							{
								const CString& strMapName = mapNameVector[0];
								m_pMapMoveDisplay->SetMapName ( strMapName + ID2GAMEINTEXT ( "MAPMOVE_MESSAGE" ) );

								UIRECT rcGlobalPos = m_pMapMoveDisplayDummy->GetGlobalPos ();
								rcGlobalPos.top -= fMENU_LIFTUP;
								m_pMapMoveDisplay->SetGlobalPos ( rcGlobalPos );
								UiShowGroupFocus( MAPMOVE_DISPLAY );
							}
							break;
						default:
							UiHideGroup( MAPMOVE_DISPLAY );
							UiHideGroup ( MULTIMAPMOVE_DISPLAY );
						}
					}
					else
					{
						UiHideGroup( MAPMOVE_DISPLAY );
						UiHideGroup ( MULTIMAPMOVE_DISPLAY );
					}
				}
				break;
			default:
				{
					if ( m_pMultiMapMoveDisplay == NULL || GLUseFeatures::GetInstance().IsUsingMultiGate() == false )
						break;

					GLCharacter* pChar(m_pGaeaClient->GetCharacter());

					if ( pChar != NULL )
					{
						switch ( pChar->VehicleState() )
						{
						case EMVEHICLE_DRIVER:
						case EMVEHICLE_OFF:
							{
								for ( MAPNAMEVECTORCITER _iteratorMapName(mapNameVector.begin()); _iteratorMapName != mapNameVector.end(); ++_iteratorMapName )
								{
									const CString& strMapName = *_iteratorMapName;
									m_pMultiMapMoveDisplay->AddMapName(strMapName);
								}
								m_pMultiMapMoveDisplay->Done();
								UIRECT rcGlobalPos = m_pMultiMapMoveDisplayDummy->GetGlobalPos ();
								rcGlobalPos.top -= fMENU_LIFTUP;
								m_pMultiMapMoveDisplay->SetGlobalPos ( rcGlobalPos );
								UiShowGroupFocus( MULTIMAPMOVE_DISPLAY );
							}
							break;
						default:
							UiHideGroup( MAPMOVE_DISPLAY );
							UiHideGroup ( MULTIMAPMOVE_DISPLAY );
							break;
						}
					}	
					else
					{
						UiHideGroup( MAPMOVE_DISPLAY );
						UiHideGroup ( MULTIMAPMOVE_DISPLAY );
					}
				}
			}
			
		}		
	}

    // TODO : 불법 프로그램 감지 테스트..
    /*if ( !IsBlockProgramFound() )
        SetBlockProgramFound( true );*/
    //
    
	//	스핵, 오토 프로세스 체크
	if ( IsBlockProgramFound () )
	{
		if ( !UiIsVisibleGroup ( WAITSERVER_DISPLAY ) )
			DoModal ( ID2GAMEINTEXT("BLOCK_PROGRAM_FOUND"), UI::MODAL_INFOMATION, UI::OK, UI::MODAL_CLOSEGAME );
	}

    DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
	if (!pGlobalStage->IsEmulator()  && !m_bDuplicateLogin)	//네트워크 연결이 끊어졌고 중복 로그인이 아닌 경우
	{
		if (!pGlobalStage->IsSTATE(DxGlobalStage::EM_CHANGE))
		{
			if (!pGlobalStage->GetNetClient()->IsOnline())
			{
				if ( !UiIsVisibleGroup ( WAITSERVER_DISPLAY ) )
					DoModal ( ID2GAMEINTEXT("PLAYINTERFACE_5"), UI::MODAL_INFOMATION, UI::OK, UI::MODAL_CLOSEGAME );				
			}
		}
	}

	if ( m_pAdminMessageDisplay )
	{
		if ( UiIsVisibleGroup ( ADMIN_MESSAGE_DISPLAY ) )
		{
			if ( m_pAdminMessageDisplay->GetLifeTime () < 0.0f )
			{
				UiHideGroup ( ADMIN_MESSAGE_DISPLAY );			
			}
		}
	}

	//if ( m_pSimpleMessageMan )
	//{
	//if ( UiIsVisibleGroup ( SIMPLE_MESSAGE_MAN ) )
	//{
	//	if ( m_pSimpleMessageMan->GetLifeTime () < 0.0f )
	//	{
	//		UiHideGroup ( SIMPLE_MESSAGE_MAN );			
	//	}
	//}
	//	}
	
	UpdateStateQuestAlarm ();
	UpdateStateSimpleHP ();	

	if ( UiIsVisibleGroup ( ACADEMY_CONFT_DISPLAY ) || UiIsVisibleGroup ( SYSTEM_MESSAGE_WINDOW )
		|| m_pGaeaClient->GetTutorial()->IsTutorial() ) // 수정 : 튜토리얼 중에 표시 하지 말지. by luxes.
	{
		UiHideGroup ( SIMPLE_MESSAGE_MAN );

		if ( m_pGaeaClient->GetTutorial()->IsTutorial() && m_pGaeaClient->GetTutorial()->m_dwQuest != NATIVEID_NULL().dwID )
		{
			SET_QUEST_HELPER( m_pGaeaClient->GetTutorial()->m_dwQuest );
			m_pGaeaClient->GetTutorial()->m_dwQuest = NATIVEID_NULL().dwID;
		}
	}
	else
	{
		// 토너먼트중 파티를 할수가 없다.
		// TOURNAMENT_UNIQ_DISABLE
		bool bTOURNAMNET = m_pGaeaClient->GetTournamentClient()->GetIsTournamentView();

		if ( RANPARAM::bSHOW_TIP && !bTOURNAMNET)
		{
			UiShowGroupBottom ( SIMPLE_MESSAGE_MAN, true );
		}
		else
		{
			UiHideGroup ( SIMPLE_MESSAGE_MAN, true );
		}
	}

    //! Progress Display 업데이트
    if( m_pProgressDisplay )
    {
        m_pProgressDisplay->Update();		
    }

    //! 점령전 정보 표시 UI 업데이트
    if ( m_pInfoCTFDisplay )
    {
        m_pInfoCTFDisplay->Update();
    }
}

void CInnerInterface::UpdateFocusOrder ()
{
    using namespace boost;
    enum EMORDERSTATE{ EMORDERSTATE_NONE, EMORDERSTATE_CHECK, EMORDERSTATE_CHANGE };
    enum EMORDERTUPLE{ INDEX_SRCID, INDEX_DESTID, INDEX_FUNCTION, INDEX_STATE };

    // 해당ID, 무조건 뒤에 있어야할 ID, 체크함수, 상태
    typedef tuple<UIGUID, UIGUID, function<bool (void)>, EMORDERSTATE>  ORDERTUPLE;
    typedef std::list<ORDERTUPLE>                                       ORDERLIST;
    typedef ORDERLIST::iterator                                         ORDERLIST_ITER;
    lambda::constant_type<bool>::type                                   DEFAULT_FUNCTION(lambda::constant(true));
    
    static ORDERLIST OrderList;

    if ( !m_bInitUpdateOrder )
    {
        if ( !OrderList.empty() )
        {
            OrderList.clear();
        }

        OrderList.push_back( make_tuple(BASIC_INFO_VIEW,             BASIC_CHAT_BOX, !bind( &IBasicChatWindow::IsChatBegin, GetBasicChatWindow() ), EMORDERSTATE_NONE ) );

        OrderList.push_back( make_tuple(QUICK_SKILL_TRAY_TAB_WINDOW, BASIC_CHAT_BOX, !bind( &IBasicChatWindow::IsChatBegin, GetBasicChatWindow() ), EMORDERSTATE_NONE ) );
		
		// Non Close UI는 채팅박스보다 무조건 위에 있어야 한다;
		for( unsigned int i=0; i<m_vNonCloseUI.size(); ++i )
			OrderList.push_back( make_tuple(m_vNonCloseUI.at( i ), BASIC_CHAT_BOX, !bind( &IBasicChatWindow::IsChatBegin, GetBasicChatWindow() ), EMORDERSTATE_NONE ) );

        m_bInitUpdateOrder = true;
    }

    // 초기화
    BOOST_FOREACH ( ORDERTUPLE& sTuple, OrderList )
    {
        EMORDERSTATE& emState = tuples::get<INDEX_STATE>(sTuple);
        emState = EMORDERSTATE_NONE;
    }

    DWORD dwCount = (DWORD)OrderList.size();

    // 순서를 검사한다
    BOOST_REVERSE_FOREACH ( const CUIFocusContainer::SUICONTROL& sControl,
        uiman::GetInnerUIMan().GetFocusList()->GetFocusList() )
    {
        UIGUID cID = sControl.cID;

        BOOST_FOREACH ( ORDERTUPLE& sTuple, OrderList )
        {
            if ( tuples::get<INDEX_STATE>(sTuple) != EMORDERSTATE_NONE )
            {
                continue;
            }

            if ( cID == tuples::get<INDEX_SRCID>(sTuple) )
            {
                EMORDERSTATE& emState = tuples::get<INDEX_STATE>(sTuple);
                emState = EMORDERSTATE_CHECK;
                --dwCount;
            }
            else if ( cID == tuples::get<INDEX_DESTID>(sTuple) )
            {
                EMORDERSTATE& emState = tuples::get<INDEX_STATE>(sTuple);

                if ( tuples::get<INDEX_FUNCTION>(sTuple)() )
                {
                    emState = EMORDERSTATE_CHANGE;
                }
                else
                {
                    emState = EMORDERSTATE_CHECK;
                }

                --dwCount;
            }
        }

        if ( dwCount == 0 )
        {
            break;
        }
    }

    // 바꿔야할 컨트롤들의 위치를 바꾼다
    BOOST_FOREACH ( ORDERTUPLE& sTuple, OrderList )
    {   
        if ( tuples::get<INDEX_STATE>(sTuple) != EMORDERSTATE_CHANGE )
        {
            continue;
        }

        UIGUID cSrcID  = tuples::get<INDEX_SRCID> (sTuple);
        UIGUID cDestID = tuples::get<INDEX_DESTID>(sTuple);

		CUIControl* pControl =
			uiman::GetInnerUIMan().FindControl( cSrcID );
		if ( NULL == pControl )
			continue;

        uiman::GetInnerUIMan().GetFocusList()
			->InsertAfter ( cDestID, cSrcID, pControl );
    }

/*
    CUIFocusContainer::UICONTROL_FOCUSLIST focus_list = uiman::GetInnerUIMan().GetFocusList()->GetFocusList();
    CUIFocusContainer::UICONTROL_FOCUSLIST_RITER riter = focus_list.rbegin ();
    CUIFocusContainer::UICONTROL_FOCUSLIST_RITER riter_end = focus_list.rend ();

    bool bBasicChatBoxShow = false;
    for ( ; riter != riter_end; ++riter )
    {
        UIGUID cID = riter->cID;
        if ( BASIC_INFO_VIEW == cID )
        {
            if ( bBasicChatBoxShow )
            {
                if ( !GetBasicChatWindow()->IsChatBegin() )
                {
                    uiman::GetInnerUIMan().GetFocusList()->InsertAfter ( BASIC_CHAT_BOX, BASIC_INFO_VIEW, riter->pUIControl );
                }
            }
            break;
        }
        else if ( BASIC_CHAT_BOX == cID )
        {
            bBasicChatBoxShow = true;
        }
    }
*/
}


void CInnerInterface::MoveBasicInfoWindow ()
{
	IUILeftTopGroup* pUILeftTopGroup = GetUILeftTopGroup();

//#ifdef CH_PARAM // 중국 인터페이스 변경
//	pControl = m_pBasicQuickSkillSlot;
//#else
	//pControl = m_pUILeftTopGroup;
//#endif

	if ( !pUILeftTopGroup ) return;

	const UIRECT& rcBasicInfoViewDummy = m_pBasicInfoViewDummy->GetGlobalPos ();
	const UIRECT& rcLeftTopGroupDummy = m_pLeftTopGroupDummy->GetGlobalPos ();
	const UIRECT& rcBasicInfoView = m_pBasicInfoView->GetUIWindowGlobalPos ();
	const UIRECT& rcLeftTopGroup = pUILeftTopGroup->GetUIWindowGlobalPos ();

	const D3DXVECTOR2 vLeftTopGroup ( rcLeftTopGroup.left, rcLeftTopGroup.top );
	const D3DXVECTOR2 vLeftTopGroupDummy ( rcLeftTopGroupDummy.left, rcLeftTopGroupDummy.top );

	const D3DXVECTOR2 vBasicInfoViewDummy (rcBasicInfoViewDummy.left,rcBasicInfoViewDummy.top);
	const D3DXVECTOR2 vBasicInfoView (rcBasicInfoView.left,rcBasicInfoView.top);
	const D3DXVECTOR2 vDist = vBasicInfoViewDummy - vBasicInfoView;
	const float fDist = D3DXVec2LengthSq ( &vDist );

	if ( fDist > 900.0f )	//	컨트롤이 떨어졌나?
	{	
		if ( vBasicInfoViewDummy != vLeftTopGroup ) pUILeftTopGroup->SetUIWindowGlobalPos ( vBasicInfoViewDummy );
	}
	else
	{
		if ( vLeftTopGroupDummy != vLeftTopGroup )		pUILeftTopGroup->SetUIWindowGlobalPos ( vLeftTopGroupDummy );
		if ( vBasicInfoViewDummy != vBasicInfoView )	m_pBasicInfoView->SetUIWindowGlobalPos( vBasicInfoViewDummy );
	}
}

void CInnerInterface::BasicInfoViewDoubleClick()
{
	IUILeftTopGroup* pUILeftTopGroup = GetUILeftTopGroup();

//#ifdef CH_PARAM // 중국 인터페이스 변경
//	pControl = m_pBasicQuickSkillSlot;
//#else
	//pControl = m_pUILeftTopGroup;
//#endif

	const UIRECT& rcBasicInfoViewDummy = m_pBasicInfoViewDummy->GetGlobalPos ();
	const UIRECT& rcLeftTopGroupDummy = m_pLeftTopGroupDummy->GetGlobalPos ();

	const D3DXVECTOR2 vLeftTopGroupDummy ( rcLeftTopGroupDummy.left, rcLeftTopGroupDummy.top );
	const D3DXVECTOR2 vBasicInfoViewDummy (rcBasicInfoViewDummy.left,rcBasicInfoViewDummy.top);

	pUILeftTopGroup->SetUIWindowGlobalPos ( vLeftTopGroupDummy );
	m_pBasicInfoView->SetUIWindowGlobalPos( vBasicInfoViewDummy );
}

//************************************
// Method:    IsOpenWindowToMoveBlock
// FullName:  CInnerInterface::IsOpenWindowToMoveBlock
// Access:    public 
// Returns:   BOOL
// Qualifier: 
//************************************
BOOL CInnerInterface::IsOpenWindowToMoveBlock()
{
	// 여기에 추가되는 컨트롤은 반듯이 InitDeviceObjects에서 생성시 SetVisibleSingle( FALSE )를 해줘야 한다.
	if( UiIsVisibleGroup( TRADE_WINDOW ) || 
		UiIsVisibleGroup( TRADEINVENTORY_WINDOW ) || 
		UiIsVisibleGroup( PRIVATE_MARKET_WINDOW ) ||
 		IsBankOpen() ||
		UiIsVisibleGroup( MAP_REQUIRE_CHECK ) ||
		UiIsVisibleGroup( REBIRTH_DIALOGUE ) ||
		UiIsVisibleGroup( PRIVATE_MARKET_MAKE ) ||
 		UiIsVisibleGroup( ITEM_GARBAGE_WINDOW ) || 
		UiIsVisibleGroup( ITEMSHOP_WINDOW ) ||
 		UiIsVisibleGroup( ITEM_SHOP_SEARCH_WINDOW ) ||
 		UiIsVisibleGroup( ITEM_SEARCH_RESULT_WINDOW ) || // 아이템 검색 창
        UiIsVisibleGroup( PETSKIN_MIX_IMAGE_WINDOW ) ||
		UiIsVisibleGroup( COSTUME_STATS_WINDOW ) )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

bool CInnerInterface::IsOpenWindowFromNPC()
{
    RANGE_MANAGEMENT_CONTROL rangeManagement = m_mmManageControl.equal_range( UI::EMMANAGECONTROL_TYPE_NPCACTIONABLE );

    for ( ITER_MANAGEMENT_CONTROL iter = rangeManagement.first; iter != rangeManagement.second; ++iter )
    {
        CUIControl* pControl = iter->second;

        if ( !pControl )
        {
            continue;
        }

        if ( pControl->IsVisible() )
        {
            return true;
        }
    }

    //if ( UiIsVisibleGroup( MARKET_WINDOW ) ||
    //    UiIsVisibleGroup( STORAGE_WINDOW ) ||
    //    UiIsVisibleGroup( CLUB_STORAGE_WINDOW ) ||
    //    UiIsVisibleGroup( DIALOGUE_WINDOW ) ||
    //    UiIsVisibleGroup( ITEM_REBUILD_WINDOW ) || 
    //    UiIsVisibleGroup( ITEM_GARBAGE_WINDOW ) || 
    //    UiIsVisibleGroup( ITEM_MIX_INVEN_WINDOW ) ||
    //    UiIsVisibleGroup( ITEM_MIX_WINDOW ) )
    //{
    //    return true;
    //}
    //

    return false;
}

void CInnerInterface::CloseAllWindowFromNPC()
{
    RANGE_MANAGEMENT_CONTROL rangeManagement = m_mmManageControl.equal_range( UI::EMMANAGECONTROL_TYPE_NPCACTIONABLE );

    for ( ITER_MANAGEMENT_CONTROL iter = rangeManagement.first; iter != rangeManagement.second; ++iter )
    {
        CUIControl* pControl = iter->second;

        if ( !pControl )
        {
            continue;
        }

        if ( !pControl->IsVisible() )
        {
            continue;
        }

        UIGUID cControlID = pControl->GetWndID();

        if ( cControlID == MARKET_WINDOW )
        {
            SetMarketWindowClose();
        }
        else if ( cControlID == STORAGE_WINDOW )
        {
            SetStorageWindowClose();
        }
        else if ( cControlID == CLUB_STORAGE_WINDOW )
        {
            SetClubStorageWindowClose();
        }
        else if ( cControlID == ITEM_REBUILD_WINDOW )
        {
            SetItemRebuildWindowClose();
        }
        else if ( cControlID == ITEM_MIX_WINDOW || cControlID == ITEM_MIX_INVEN_WINDOW )
        {
            SetItemMixWindowClose();
        }
        else if( cControlID == GAMBLE_DIALOGUE || cControlID == GAMBLE_SELECT_DIALOGUE || 
            cControlID == GAMBLE_AGAIN_DIALOGUE || cControlID == GAMBLE_ANIMATION_DIALOGUE || 
            cControlID == GAMBLE_RESULT_EVEN_DIALOGUE || cControlID == GAMBLE_RESULT_ODD_DIALOGUE  )
        {
            ToggleWindowMiniGame();
        }
        else
        {
            UiHideGroup( cControlID );
        }
    }
}

/*
#ifdef CH_PARAM // 중국 인터페이스 변경
void CInnerInterface::UpdatePotionTrayPosition()
{
	m_pUILeftTopGroup->SetGlobalPos( m_pBasicPotionTrayDummy->GetGlobalPos() );
	m_pQuestAlarm->SetGlobalPos( m_pQuestAlarmDummy->GetGlobalPos() );

	BOOL bVisibleQuest = UiIsVisibleGroup( QUEST_ALARM);
	BOOL bVisibleMini = UiIsVisibleGroup( MINIPARTY_OPEN );

	if( bVisibleQuest && bVisibleMini ) return;

	UIRECT rcDummy = m_pUILeftTopGroup->GetGlobalPos();
	rcDummy.left += 41; // 퀘스트, 미니파티 버튼의 가로 크기가 41
	rcDummy.right = rcDummy.left + rcDummy.sizeX;
	m_pUILeftTopGroup->SetGlobalPos( rcDummy );

	CUIControl * pUIControl(NULL);

	if( !bVisibleQuest && !bVisibleMini )
	{
		pUIControl = m_pUILeftTopGroup;
	}
	else if( !bVisibleMini )
	{
		pUIControl = m_pQuestAlarm;
	}

	if( pUIControl != NULL )
	{
		rcDummy = pUIControl->GetGlobalPos();
		rcDummy.left += 41;
		rcDummy.right = rcDummy.left + rcDummy.sizeX;
		pUIControl->SetGlobalPos( rcDummy );
	}
}
#endif
*/
void CInnerInterface::SetArmSwap( BOOL bSwap )
{
	if(m_pInventoryWindow)
		m_pInventoryWindow->SetArmSwapTabButton( bSwap );

	if( m_pCharacterWindow )
		m_pCharacterWindow->SetArmSwapTabButton();
}

bool CInnerInterface::ItemShopAuth ()
{
//	ItemShopAuth
//#if defined ( JP_PARAM )
	EMSERVICE_PROVIDER sp = m_pGaeaClient->GetServiceProvider();
	
	if (sp == SP_JAPAN)
	{
		if( !m_pItemShopWindowWeb )
			return false;

		if ( m_bItemShopLoad )
			return false;

		GLCharacter* pChar = m_pGaeaClient->GetCharacter();
		if ( !pChar )
			return false;

		CString strData = "";
		strData.Format( "%s?id=%s& uuid=%s", RANPARAM::ItemShopAddress, pChar->m_szUID, RANPARAM::GETJAPANUUID() );

		m_pItemShopWindowWeb->SetAddress( strData.GetString() );
		COMMON_WEB::Navigate( m_pItemShopWindowWeb->GetWebID(), strData.GetString(), TRUE );

		m_bItemShopLoad = true;

		// 로딩 완료 메세지가 올때까지 기다린다.
		return false;
	}
//#endif
	return true;
}

void CInnerInterface::ItemShopVisible()
{
//	ItemShopAuth
//#if defined ( JP_PARAM ) 
	EMSERVICE_PROVIDER sp = m_pGaeaClient->GetServiceProvider();

	if (sp == SP_JAPAN)
	{
		if( !m_pItemShopWindowWeb )
			return;

		if ( !m_bItemShopLoad )
			return;

		if( !COMMON_WEB::IsCompleteLoad( m_pItemShopWindowWeb->GetWebID() ) )
			return;

		UiShowGroupFocus ( ITEMSHOP_WINDOW );
		UiHideGroup ( HELP_WINDOW );

		m_bItemShopLoad = false;
	}
//#endif
}

void CInnerInterface::SetItemMixResult( CString strMsg, bool bSuccess, bool bFail )
{
	m_pItemMixWindow->SetItemMixResult( strMsg, bSuccess, bFail );
}

void CInnerInterface::SET_GATHER_GAUGE( DWORD dwGaeaID, float fTime )
{
	m_pGatherGauge->SetTime( fTime );
	m_pGatherGauge->SetMaterial ( dwGaeaID );
}

void CInnerInterface::VisibleCDMRanking( bool bVisible )
{
	if ( bVisible )
	{
		UiHideGroup( QUEST_HELPER_DISPLAY );
		UiShowGroupBottom ( CDM_RANKING_DISPLAY );
	}
	else
	{
		UiHideGroup( CDM_RANKING_DISPLAY );
		UiShowGroupBottom ( QUEST_HELPER_DISPLAY );
	}
}

void CInnerInterface::RefreashCDMRanking()
{
	if ( m_pCdmRankingDisplay && m_pCdmRankingDisplay->IsVisible() )
		m_pCdmRankingDisplay->RefreashCdmRanking();
}

void CInnerInterface::VisibleCDMSafeTime( bool bVisible, float fCDM_SAFE_TIME )
{
	if ( !m_pCdmSafeTimeDisplay ) return;
    
	// 지속 스킬 정보창 위치 갱신
	if ( bVisible )
	{
		m_pCdmSafeTimeDisplay->SetCDMSafeTime( fCDM_SAFE_TIME );
		UiShowGroupBottom( CDM_SAFETIME_DISPLAY );
	}
	else
	{
		UiHideGroup( CDM_SAFETIME_DISPLAY );
	}
}

void CInnerInterface::CalcSkillDisplayPos( bool bVisible )
{
	if ( !m_pSkillTimeDisplay || !m_pCdmSafeTimeDisplay ) return;
    
	// 지속 스킬 정보창 위치 갱신
	if ( bVisible )
	{
		const UIRECT& rcPos = m_pCdmSafeTimeDisplay->GetGlobalPos ();
		const D3DXVECTOR2 vPos (rcPos.right+1,rcPos.top);
		m_pSkillTimeDisplay->SetGlobalPos( vPos );
	}
	else
	{
		const UIRECT& rcPos = m_pCdmSafeTimeDisplay->GetGlobalPos ();
		const D3DXVECTOR2 vPos (rcPos.left,rcPos.top);
		m_pSkillTimeDisplay->SetGlobalPos( vPos );	
	}
}

void CInnerInterface::SetChatOption(WORD wChatFlag)
{
	if (m_pBasicChat)
        m_pBasicChat->SetChatOption(wChatFlag);
}

void CInnerInterface::SetChatType(DWORD dwChatType)
{
    if (m_pBasicChat)
        m_pBasicChat->SetChatType(dwChatType);
}

void CInnerInterface::ChatToLink( const SLINK_DATA_BASIC& sLinkDataBasic, const SITEMCUSTOM *pItemCustom/*GroupChat에서는 SITEMCUSTOM을 사용한다*/)
{
	/*
	//GroupChat
	if ( 그룹채팅이면 )
	{
		GASSERT(pItemCustom);

		if (pItemCustom == NULL)
			return;

		m_pGroupChat->ChatToLink(pItemCustom);	

		return;
	}
	*/
	CString strLinkName;
	DWORD dwColor = NS_UITEXTUTIL::ENABLE;

	int MAX_GROUP_CHAT_WINDOW_INDEX = 0;
	int MaxGroupChatOpen = static_cast<int>(m_pGaeaClient->GetChatGroup()->GetMaxGroupChatOpen());
	for ( int EMROOMINDEX = 0; EMROOMINDEX < MaxGroupChatOpen; EMROOMINDEX++ )
	{
		MAX_GROUP_CHAT_WINDOW_INDEX = EMROOMINDEX + MAX_GROUP_CHAT_WINDOW_START;

		if ( IsVisible( MAX_GROUP_CHAT_WINDOW_INDEX ) )
		{
			if ( m_pMaxChatWindow[EMROOMINDEX]->IsBeginEdit() )
			{
				if ( !GetLinkData( sLinkDataBasic, strLinkName, dwColor ) )
					return;

				m_pMaxChatWindow[EMROOMINDEX]->ChatToLink( sLinkDataBasic, strLinkName , dwColor, pItemCustom );
				return;
			}
		}
	}

	//CString strLinkName;
	//DWORD dwColor = NS_UITEXTUTIL::ENABLE;

	if ( !GetLinkData( sLinkDataBasic, strLinkName, dwColor ) )
		return;

	if( m_pBasicChat )
		m_pBasicChat->ChatToLink( sLinkDataBasic, UI::ToString( strLinkName ), dwColor );
}

void  CInnerInterface::SetPartyLinkOnce ( int nLinkIndex )
{
	MAP_PARTY_LINK_DATA_INDEX_ITER pos = m_mapPartyLinkData.find ( nLinkIndex );	
	if ( pos == m_mapPartyLinkData.end() ) return;

	// 파티모집 UI표시
	GLPARTY_LINK& sPartyLink = pos->second;
	sPartyLink.bOnce = true;
}

void CInnerInterface::DeleteLinkData ( const SLINK_DATA_INDEX& sLinkIndex )
{
	int nLinkIndex = sLinkIndex.nLinkIndex;
	if ( nLinkIndex < 0 ) return;

	switch ( sLinkIndex.sLinkType.wMType ) 
	{
	case ITEM_LINK:
		{
			switch ( sLinkIndex.sLinkType.wSType ) 
			{
			case ITEM_LINK_INVEN:
			case ITEM_LINK_PUTON:
			case ITEM_LINK_USER_STORAGE:
			case ITEM_LINK_CLUB_STORAGE:
			case ITEM_LINK_CROW_SALE:
				{
					MAP_ITEM_LINK_DATA_INDEX_ITER pos = m_mapItemLinkData.find ( nLinkIndex );	
					if ( pos != m_mapItemLinkData.end() )
						m_mapItemLinkData.erase( pos );
				}
				break;
			}
		}
		break;
	case PARTY_LINK:
		{
			switch ( sLinkIndex.sLinkType.wSType ) 
			{
			case PARTY_LINK_RECRUIT:
			case PARTY_LINK_ACCEPT:
			case PARTY_LINK_REFUSE:
				{
					MAP_PARTY_LINK_DATA_INDEX_ITER pos = m_mapPartyLinkData.find ( nLinkIndex );	
					if ( pos != m_mapPartyLinkData.end() )
						m_mapPartyLinkData.erase( pos );
				}
				break;
			}
		}
		break;
	}
}

void CInnerInterface::SetPartyOption(const SPartyOption& PartyOption)
{
	m_pPartyWindowRenewal->SetPartyOption(PartyOption);
}
void CInnerInterface::SetPartyInfo(const DWORD dwPartyID, const DWORD dwHashKey, const DWORD dwMasterID, const DWORD dwTimeKey)
{
	m_pPartyWindow->SetPartyInfo(dwPartyID, dwHashKey, dwMasterID, dwTimeKey);
}

BOOL CInnerInterface::GetLinkdataIndex(SLINK_DATA_ITEM& itrLinkData, int nIdx)
{
	MAP_ITEM_LINK_DATA_INDEX_ITER pos = m_mapItemLinkData.find( nIdx );
	if ( pos == m_mapItemLinkData.end() )
		return FALSE;

	itrLinkData = pos->second;
	return TRUE;
}

bool CInnerInterface::LinkDataProcess( int nLinkIndex )
{
	// 함수 확장이 필요하다;
	// 아이템에 따라 서로 타입이 다르지만 단축키는 같은 경우가있다;
	// 또 마우스 입력정보는 이함수로는 캐치해내기가 힘들다;
	if ( nLinkIndex < 0 ) return false;
	
	SLINK_DATA_INDEX* pLinkBasic = GetLinkBasicData( nLinkIndex );
	if ( !pLinkBasic ) return false;
	if ( pLinkBasic->nLinkIndex < 0 ) return false;

	int nDataIndex = pLinkBasic->nLinkIndex;

	switch ( pLinkBasic->sLinkType.wMType ) 
	{
	case ITEM_LINK:
		{
			switch ( pLinkBasic->sLinkType.wSType ) 
			{
			case ITEM_LINK_INVEN:
			case ITEM_LINK_PUTON:
			case ITEM_LINK_USER_STORAGE:
			case ITEM_LINK_CLUB_STORAGE:
			case ITEM_LINK_CROW_SALE:
				{
					MAP_ITEM_LINK_DATA_INDEX_ITER pos = m_mapItemLinkData.find( nDataIndex );
					if ( pos == m_mapItemLinkData.end() )
						return false;

					SLINK_DATA_ITEM& sData = pos->second;

					if( DxInputDevice::GetInstance().GetKeyState( DIK_LSHIFT ) & DXKEY_PRESSED )
					{
						WishList::CClient* pWishList = m_pGaeaClient->GetWishList();
						if( pWishList )
							pWishList->NewChatLinkItem( sData.sItem, sData.strCharName );
					}
					else
					{
						SHOW_LINK_ITEM_INFO( sData.sItem );
					}
				}
				return true;
			}
		}
		break;

	case PARTY_LINK:
		{
			switch ( pLinkBasic->sLinkType.wSType ) 
			{
			case PARTY_LINK_RECRUIT:
				{
					MAP_PARTY_LINK_DATA_INDEX_ITER pos = m_mapPartyLinkData.find ( nDataIndex );	
					if ( pos == m_mapPartyLinkData.end() )
						return false;

					// 파티모집 UI표시
					const GLPARTY_LINK& sPartyLink = pos->second;

					//	파티장 확인
					if ( !sPartyLink.sPartyMem[0].VALID() )
						return false;

					SHOW_PARTY_LINK_WINDOW( nDataIndex, sPartyLink );

					return true;
				}
				break;
			case PARTY_LINK_ACCEPT:
				{
					MAP_PARTY_LINK_DATA_INDEX_ITER pos = m_mapPartyLinkData.find ( nDataIndex );	
					if ( pos == m_mapPartyLinkData.end() )
						return false;

					// 파티모집 UI표시
					GLPARTY_LINK& sPartyLink = pos->second;

					//	파티장 확인
					if ( !sPartyLink.sPartyMem[0].VALID() ) return false;
					if ( sPartyLink.bOnce ) return false;

					sPartyLink.bOnce = true;
										
					m_pGaeaClient->GetCharacter()->ReqPartyLinkLure( true, sPartyLink.dwHashKey, sPartyLink.dwTimeKey, sPartyLink.sPartyMem[0].dwGaeaID );

					return true;
				}
				break;
			case PARTY_LINK_REFUSE:
				{
					MAP_PARTY_LINK_DATA_INDEX_ITER pos = m_mapPartyLinkData.find ( nDataIndex );	
					if ( pos == m_mapPartyLinkData.end() ) return false;

					// 파티모집 UI표시
					GLPARTY_LINK& sPartyLink = pos->second;

					//	파티장 확인
					if ( !sPartyLink.sPartyMem[0].VALID() ) return false;

					if ( sPartyLink.bOnce ) return false;

					sPartyLink.bOnce = true;

					m_pGaeaClient->GetCharacter()->ReqPartyLinkLure( false, sPartyLink.dwHashKey, sPartyLink.dwTimeKey, sPartyLink.sPartyMem[0].dwGaeaID );

					return true;
				}
				break;
			}
		}
		break;

	case QUEST_LINK:
	case SKILL_LINK:
	case CROW_LINK:
	case MAP_LINK:
		{
		}
		break;
	}

	return false;
}

bool CInnerInterface::GetLinkData( const SLINK_DATA_BASIC& sLinkDataBasic, CString& strLinkName, DWORD& dwColor )
{
	CString strName;
	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	if ( pCharacter )
	{
		if ( pCharacter->GetLinkData( sLinkDataBasic, strLinkName, dwColor ) )	return true;
	}

	return false;	
}

SLINK_DATA_INDEX* CInnerInterface::GetLinkBasicData ( int nLinkIndex )
{
	MAP_LINK_DATA_INDEX_ITER pos = m_mapLinkData.find ( nLinkIndex );	
	if ( pos == m_mapLinkData.end() ) return NULL;	
	
	return &pos->second;
}

bool CInnerInterface::GetLinkName( CString& strLinkName, int nLinkIndex )
{
	if ( nLinkIndex < 0 ) return false;
	
	SLINK_DATA_INDEX* pLinkBasic = GetLinkBasicData( nLinkIndex );
	if ( !pLinkBasic ) return false;
	if ( pLinkBasic->nLinkIndex < 0 ) return false;

	int nDataIndex = pLinkBasic->nLinkIndex;	

	switch ( pLinkBasic->sLinkType.wMType ) 
	{
	case ITEM_LINK:
		{
			switch ( pLinkBasic->sLinkType.wSType ) 
			{
			case ITEM_LINK_INVEN:
			case ITEM_LINK_PUTON:
			case ITEM_LINK_USER_STORAGE:
			case ITEM_LINK_CLUB_STORAGE:
			case ITEM_LINK_CROW_SALE:			

				//	이미 항목이 있을경우 삭제하고 새로운 데이터로 갱신
				MAP_ITEM_LINK_DATA_INDEX_ITER pos = m_mapItemLinkData.find ( nDataIndex );	
				if ( pos == m_mapItemLinkData.end() )
					return false;

				SLINK_DATA_ITEM& sData = pos->second;

				const SITEM* pItem = GLogicData::GetInstance().GetItem( sData.sItem.GetNativeID() );
				if ( !pItem )
					return false;

				strLinkName.Format( "%s%s%s", strLINK_LEFT_SIMBOL, sData.sItem.GETNAME().c_str(), strLINK_RIGHT_SIMBOL );
				
				return true;			
			}
		}
		break;
	case PARTY_LINK:
		{
			switch ( pLinkBasic->sLinkType.wSType ) 
			{
			case PARTY_LINK_RECRUIT:
				{
					MAP_PARTY_LINK_DATA_INDEX_ITER pos = m_mapPartyLinkData.find ( nDataIndex );	
					if ( pos == m_mapPartyLinkData.end() ) return false;
 
                    CString strLocation("");

                    // 지역 정보 표시
                    if ( m_pGaeaClient )
                    {
                        GLPARTY_LINK& sPartyLink = pos->second;
                        strLocation = m_pGaeaClient->GetMapName(sPartyLink.sMapID);
                    }

					strLinkName.Format( "%s%s, %s%s", strLINK_LEFT_SIMBOL, strLocation, ID2GAMEWORD( "CHAT_LINK_PARTY_RECRUIT", 1 ), strLINK_RIGHT_SIMBOL );					
										
					return true;
				}
				break;
			case PARTY_LINK_ACCEPT:
				{
					MAP_PARTY_LINK_DATA_INDEX_ITER pos = m_mapPartyLinkData.find ( nDataIndex );	
					if ( pos == m_mapPartyLinkData.end() ) return false;

					strLinkName.Format( "%s%s%s", strLINK_LEFT_SIMBOL, ID2GAMEWORD( "CHAT_LINK_PARTY_RECRUIT", 2 ), strLINK_RIGHT_SIMBOL );								
					
					return true;
				}
				break;
			case PARTY_LINK_REFUSE:
				{
					MAP_PARTY_LINK_DATA_INDEX_ITER pos = m_mapPartyLinkData.find ( nDataIndex );	
					if ( pos == m_mapPartyLinkData.end() ) return false;

					strLinkName.Format( "%s%s%s", strLINK_LEFT_SIMBOL, ID2GAMEWORD( "CHAT_LINK_PARTY_RECRUIT", 3 ), strLINK_RIGHT_SIMBOL );					
					
					return true;
				}
				break;
			}
		}
		break;
	case QUEST_LINK:
	case SKILL_LINK:
	case CROW_LINK:
	case MAP_LINK:
		{
		}
		break;
	}

	return false;

}

DWORD CInnerInterface::GetLinkColor( int nLinkIndex )
{
	DWORD dwColor = NS_UITEXTCOLOR::CHAT_NORMAL;
	if ( nLinkIndex < 0 ) return dwColor;
	
	SLINK_DATA_INDEX* pLinkBasic = GetLinkBasicData( nLinkIndex );
	if ( !pLinkBasic ) return dwColor;
	if ( pLinkBasic->nLinkIndex < 0 ) return dwColor;

	int nDataIndex = pLinkBasic->nLinkIndex;	

	switch ( pLinkBasic->sLinkType.wMType ) 
	{
	case ITEM_LINK:
		{
			switch ( pLinkBasic->sLinkType.wSType ) 
			{
			case ITEM_LINK_INVEN:
			case ITEM_LINK_PUTON:
			case ITEM_LINK_USER_STORAGE:
			case ITEM_LINK_CLUB_STORAGE:
			case ITEM_LINK_CROW_SALE:			

				//	이미 항목이 있을경우 삭제하고 새로운 데이터로 갱신
				MAP_ITEM_LINK_DATA_INDEX_ITER pos = m_mapItemLinkData.find ( nDataIndex );	
				if ( pos == m_mapItemLinkData.end() )
					return dwColor;

				SLINK_DATA_ITEM& sData = pos->second;

				const SITEM* pItem = GLogicData::GetInstance().GetItem( sData.sItem.GetNativeID() );
				if ( !pItem )
					return dwColor;

                //아이템 링크별로 색상이 다르도록 수정 
                //dwColor = COMMENT::ITEMCOLOR[pItem->sBasicOp.emLevel];
                dwColor = ITEM_INFOR_COLOR::dwItemRank[ pItem->sBasicOp.emLevel ];


				return dwColor;			
			}
		}
		break;
	case PARTY_LINK:
		{
			switch ( pLinkBasic->sLinkType.wSType ) 
			{
			case PARTY_LINK_RECRUIT:
				{
					MAP_PARTY_LINK_DATA_INDEX_ITER pos = m_mapPartyLinkData.find ( nDataIndex );	
					if ( pos == m_mapPartyLinkData.end() ) return dwColor;

					dwColor = NS_UITEXTCOLOR::CHAT_LINK_PARTY_RECRUIT;

					return dwColor;
				}
				break;
			case PARTY_LINK_ACCEPT:
				{
					MAP_PARTY_LINK_DATA_INDEX_ITER pos = m_mapPartyLinkData.find ( nDataIndex );	
					if ( pos == m_mapPartyLinkData.end() ) return dwColor;

					dwColor = NS_UITEXTCOLOR::CHAT_LINK_PARTY_RECRUIT;
					return dwColor;
				}
				break;
			case PARTY_LINK_REFUSE:
				{
					MAP_PARTY_LINK_DATA_INDEX_ITER pos = m_mapPartyLinkData.find ( nDataIndex );	
					if ( pos == m_mapPartyLinkData.end() ) return dwColor;

					dwColor = NS_UITEXTCOLOR::CHAT_LINK_PARTY_RECRUIT;
					return dwColor;
				}
				break;
			}
		}
		break;
	case QUEST_LINK:
	case SKILL_LINK:
	case CROW_LINK:
	case MAP_LINK:
		{
		}
		break;
	}

	return dwColor;
}

int CInnerInterface::AddLinkDataToList_Client( const SLINK_DATA_BASIC& sLinkDataBasic )
{
	int nLinkIndex = -1;

	if ( !sLinkDataBasic.VALID() ) return nLinkIndex;

	switch ( sLinkDataBasic.sLinkType.wMType ) 
	{
	case ITEM_LINK:
		{
			switch ( sLinkDataBasic.sLinkType.wSType ) 
			{
			case ITEM_LINK_INVEN:
				{
					SINVEN_POS sInvenPos;
					sInvenPos.dwPos = sLinkDataBasic.dwData1;

					GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
					if ( !pCharacter ) break;

					SINVENITEM* pInvenItem = pCharacter->m_cInventory.GetItem( sInvenPos.wPosX, sInvenPos.wPosY );
					if ( !pInvenItem ) break;

					SITEMCUSTOM sItemCustom = pInvenItem->sItemCustom;
                  
					nLinkIndex = AddLinkDataToList (
						std::string( pCharacter->GetName() ),
						sLinkDataBasic.sLinkType,
						&sItemCustom,
						sizeof(SITEMCUSTOM) );					

					return nLinkIndex;
				}
				break;

			case ITEM_LINK_PUTON:
				{
					EMSLOT emSlot = (EMSLOT)sLinkDataBasic.dwData1;
					if ( emSlot >= SLOT_TSIZE ) break;

					GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
					if ( !pCharacter ) break;
					if ( !pCharacter->VALID_SLOT_ITEM( emSlot ) ) break;

					SITEMCUSTOM sItemCustom = pCharacter->m_PutOnItems[emSlot];

					nLinkIndex = AddLinkDataToList(
						std::string( pCharacter->GetName() ),
						sLinkDataBasic.sLinkType,
						&sItemCustom,
						sizeof(SITEMCUSTOM) );

					return nLinkIndex;
				}
				break;

			case ITEM_LINK_USER_STORAGE:
				{
					DWORD dwChannel = sLinkDataBasic.dwData1;
					if ( dwChannel >= MAX_CLUBSTORAGE )
                        break;
					
					SINVEN_POS sInvenPos;
					sInvenPos.dwPos = sLinkDataBasic.dwData2;

					GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
					if ( !pCharacter ) break;
					
					SINVENITEM* pInvenItem = pCharacter->m_cStorage[dwChannel].FindPosItem ( sInvenPos.wPosX, sInvenPos.wPosY );
					if ( !pInvenItem )	break;

					SITEMCUSTOM sItemCustom = pInvenItem->sItemCustom;

					nLinkIndex = AddLinkDataToList(
						std::string( pCharacter->GetName() ),
						sLinkDataBasic.sLinkType,
						&sItemCustom,
						sizeof(SITEMCUSTOM) );

					return nLinkIndex;
				}
				break;

			case ITEM_LINK_CLUB_STORAGE:
				{
					GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
					if ( !pCharacter )
						break;

					if (!m_pGaeaClient->GetMyClub()->IsValid())
                        break;
					if ( m_pGaeaClient->GetMyClub()->MasterCharDbNum() != pCharacter->m_CharDbNum )
                        break;

					DWORD dwChannel = sLinkDataBasic.dwData1;
					if ( dwChannel >= MAX_CLUBSTORAGE )
                        break;

					SINVEN_POS sInvenPos;
					sInvenPos.dwPos = sLinkDataBasic.dwData2;
					
					SINVENITEM* pInvenItem = m_pGaeaClient->GetMyClub()->FindPosItem(dwChannel, sInvenPos.wPosX, sInvenPos.wPosY);
					if (!pInvenItem)
                        break;

					SITEMCUSTOM sItemCustom = pInvenItem->sItemCustom;

					nLinkIndex = AddLinkDataToList(
						std::string( pCharacter->GetName() ),
						sLinkDataBasic.sLinkType,
						&sItemCustom,
						sizeof(SITEMCUSTOM) );

					return nLinkIndex;
				}
				break;

			case ITEM_LINK_CROW_SALE:
				{
					GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
					if ( !pCharacter )
						break;

					DWORD dwNpcGlobID = ( sLinkDataBasic.dwData1 & 0xFFFF0000 ) >> 16;
					DWORD dwChannel = ( sLinkDataBasic.dwData1 & 0x0000FFFF );

					GLLandManClient* pLand = m_pGaeaClient->GetActiveMap();
					if ( !pLand )	break;
					std::tr1::shared_ptr<GLCrowClient> pCrow = pLand->GetCrow ( dwNpcGlobID );
					if ( !pCrow )
                        break;

					if ( pCrow->m_pCrowData->GetSaleNum() <= dwChannel )
                        break;

					GLInventory *pInven = pCrow->m_pCrowData->GetSaleInven(dwChannel);
					if ( !pInven )	 break;

					SINVEN_POS sInvenPos;
					sInvenPos.dwPos = sLinkDataBasic.dwData2;
					
					SINVENITEM* pSaleItem = pInven->GetItem ( sInvenPos.wPosX, sInvenPos.wPosY );
					if ( !pSaleItem ) break;

					SITEMCUSTOM sItemCustom = pSaleItem->sItemCustom;

					nLinkIndex = AddLinkDataToList(
						std::string( pCharacter->GetName() ),
						sLinkDataBasic.sLinkType,
						&sItemCustom,
						sizeof(SITEMCUSTOM) );

					return nLinkIndex;
				}
				break;
			}
		}
		break;

	case PARTY_LINK:
		{
			switch ( sLinkDataBasic.sLinkType.wSType ) 
			{
			case PARTY_LINK_RECRUIT:
				{
					// 나의 정보를 입력하는곳이다.
					// 파티장이 아니면 안된다.
					
					GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
					if ( pCharacter == NULL )
						break;

					GLPartyClient* const pMyParty = m_pGaeaClient->GetMyPartyClient();
					const bool bParty = pMyParty->isValid();
					if ( (bParty == true) && (pMyParty->isMaster(pCharacter->GetGaeaID()) == false) )
						break;

					GLPARTY_LINK sPartyLink;

					int nPartyMem = 1;

					if ( bParty ) 
                    {						
                        sPartyLink.sMapID  = sLinkDataBasic.dwData1;						

						for ( DWORD _i = 0; _i < pMyParty->GetNMember(); ++_i )
						{
							GLPartySlotClient* const pMember = pMyParty->GetMember(_i);			
							if ( pMember == NULL )
								continue;

							sPartyLink.sPartyMem[_i + 1].dwGaeaID = pMember->m_dwGaeaID;
							sPartyLink.sPartyMem[_i + 1].emClass = pMember->m_emClass;
							sPartyLink.sPartyMem[_i + 1].SetName(pMember->m_szName);
						}
					}
					else
                    {
                        sPartyLink.sMapID  = sLinkDataBasic.dwData1;

                        sPartyLink.sPartyMem[0].dwGaeaID = pCharacter->GetGaeaID();
						sPartyLink.sPartyMem[0].emClass  = pCharacter->m_emClass;
						sPartyLink.sPartyMem[0].SetName(pCharacter->m_szName);
						
					}
					
					nLinkIndex = AddLinkDataToList(
						std::string( pCharacter->GetName() ),
						sLinkDataBasic.sLinkType,
						&sPartyLink,
						sizeof(GLPARTY_LINK_MEM) * nPartyMem + sizeof(SNATIVEID) );

					return nLinkIndex;
				}
				break;
			case PARTY_LINK_ACCEPT:				//	해당 사항이 없다.
				break;
			case PARTY_LINK_REFUSE:				//	해당 사항이 없다.
				break;
			}
		}
		break;
	case QUEST_LINK:
	case SKILL_LINK:
	case CROW_LINK:
	case MAP_LINK:
		{
		}
		break;
	}

	return nLinkIndex;


}

int CInnerInterface::AddLinkDataToList ( const std::string& strCharName, const SLINK_TYPE& sLinkType, void* pBuffer, DWORD dwSize )
{
	int nLinkIndex = -1;

	if ( !sLinkType.VALID() )
		return nLinkIndex;

	//	Index를 모두 소모한 경우 0으로 초기화 
	if ( m_dwLinkDataIndex == CHAT_LINK_INDEX_MAX ) 
	{
		m_dwLinkDataIndex = 0;
	}

	int nDataIndex = AddLinkDataToTypeList( strCharName, sLinkType, pBuffer, dwSize );
	if ( nDataIndex >= 0 ) 
	{
		SLINK_DATA_INDEX sLinkDataAdd;
		sLinkDataAdd.sLinkType = sLinkType;
		sLinkDataAdd.nLinkIndex = nDataIndex;
		m_mapLinkData.insert( std::make_pair( m_dwLinkDataIndex, sLinkDataAdd ) );
		nLinkIndex = m_dwLinkDataIndex;

		if ( m_mapLinkData.size() > CHAT_LINK_BASIC_DATA_MAX ) 
		{
			int nDelete = m_dwLinkDataIndex - CHAT_LINK_BASIC_DATA_MAX; 

			// 0보다 작다는것은 Index가 한바퀴 돌았다는 뜻이다.
			if ( nDelete < 0 ) 
			{
				nDelete += CHAT_LINK_INDEX_MAX;
			}

			//	이전 데이터 부터 삭제
			MAP_LINK_DATA_INDEX_ITER pos = m_mapLinkData.find ( nDelete );	
			if ( pos != m_mapLinkData.end() )
			{
				const SLINK_DATA_INDEX& sLinkIndex = pos->second;
				DeleteLinkData( sLinkIndex );
				m_mapLinkData.erase( pos );				
			}
		}	

		m_dwLinkDataIndex++;

		return nLinkIndex;
	}


	return nLinkIndex;
	
}

int	CInnerInterface::AddLinkDataToTypeList ( const std::string& strCharName, const SLINK_TYPE& sLinkType, void* pBuffer, DWORD dwSize )
{
	int nIndex = -1;
	switch ( sLinkType.wMType ) 
	{
	case ITEM_LINK:
		{
			switch ( sLinkType.wSType ) 
			{
			case ITEM_LINK_INVEN:
			case ITEM_LINK_PUTON:
			case ITEM_LINK_USER_STORAGE:
			case ITEM_LINK_CLUB_STORAGE:
			case ITEM_LINK_CROW_SALE:	
				{
					SITEMCUSTOM sItemCustom;

					if ( dwSize != sizeof( SITEMCUSTOM ) )
						return nIndex;

					memcpy ( &sItemCustom, pBuffer, dwSize );

					//	Index를 모두 소모한 경우 0으로 초기화 
					if ( m_dwItemLinkDataIndex == CHAT_LINK_INDEX_MAX )
					{
						m_dwItemLinkDataIndex = 0;
					}

					SLINK_DATA_ITEM sData;
					sData.strCharName = strCharName;
					sData.sItem = sItemCustom;

					m_mapItemLinkData.insert( std::make_pair( m_dwItemLinkDataIndex, sData ) );
					nIndex = m_dwItemLinkDataIndex;

					//	CHAT_LINK_DATA_MAX 사이즈만큼만 가지고 있는다.
					if ( m_mapItemLinkData.size() > CHAT_LINK_DATA_MAX ) 
					{
						int nDelete = m_dwItemLinkDataIndex - CHAT_LINK_DATA_MAX; 

						// 0보다 작다는것은 Index가 한바퀴 돌았다는 뜻이다.
						if ( nDelete < 0 ) 
						{
							nDelete += CHAT_LINK_INDEX_MAX;
						}
						
						MAP_ITEM_LINK_DATA_INDEX_ITER pos = m_mapItemLinkData.find ( nDelete );	
						if ( pos != m_mapItemLinkData.end() )
							m_mapItemLinkData.erase( pos );
					}

					m_dwItemLinkDataIndex++;

					return nIndex;
				}
				break;
			}
		}
		break;

	case PARTY_LINK:
		{
			switch ( sLinkType.wSType ) 
			{
			case PARTY_LINK_RECRUIT:
				{
					GLPARTY_LINK sPartyLink;
                    DWORD        dwOffset  = 0;
                    DWORD        dwCurSize = 0;

					// 맵아이디복사
					memcpy ( &sPartyLink.sMapID, pBuffer, sizeof(SNATIVEID) );
					dwOffset  += sizeof(SNATIVEID);

					if ( dwSize < dwOffset ) return nIndex;
					dwCurSize = dwSize - dwOffset;

					if ( ( dwCurSize % sizeof( GLPARTY_LINK_MEM )) != 0 )
						return nIndex;

					int nPartyNum = dwCurSize / sizeof( GLPARTY_LINK_MEM );
					if ( nPartyNum > MAXPARTY )
						return nIndex;

					memcpy ( sPartyLink.sPartyMem, (BYTE*)pBuffer+dwOffset, dwCurSize );

					//	Index를 모두 소모한 경우 0으로 초기화 
					if ( m_dwPartyLinkDataIndex == CHAT_LINK_INDEX_MAX )
					{
						m_dwPartyLinkDataIndex = 0;
					}

					m_mapPartyLinkData.insert( std::make_pair( m_dwPartyLinkDataIndex, sPartyLink ) );
					nIndex = m_dwPartyLinkDataIndex;

					//	CHAT_LINK_DATA_MAX 사이즈만큼만 가지고 있는다.
					if ( m_mapPartyLinkData.size() > CHAT_LINK_DATA_MAX ) 
					{
						int nDelete = m_dwPartyLinkDataIndex - CHAT_LINK_DATA_MAX; 

						// 0보다 작다는것은 Index가 한바퀴 돌았다는 뜻이다.
						if ( nDelete < 0 ) 
						{
							nDelete += CHAT_LINK_INDEX_MAX;
						}
						
						MAP_PARTY_LINK_DATA_INDEX_ITER pos = m_mapPartyLinkData.find ( nDelete );	
						if ( pos != m_mapPartyLinkData.end() )
							m_mapPartyLinkData.erase( pos );
					}

					m_dwPartyLinkDataIndex++;

					return nIndex;
				}
				break;

			case PARTY_LINK_ACCEPT:
				{
					const DWORD* const pBuffer4Byte((const DWORD* const)pBuffer);
					GLPARTY_LINK sPartyLink;					
					sPartyLink.sPartyMem[0].dwGaeaID = pBuffer4Byte[0];
					sPartyLink.dwHashKey = pBuffer4Byte[1];
					sPartyLink.dwTimeKey = pBuffer4Byte[2];

					//	Index를 모두 소모한 경우 0으로 초기화 
					if ( m_dwPartyLinkDataIndex == CHAT_LINK_INDEX_MAX )
					{
						m_dwPartyLinkDataIndex = 0;
					}

					m_mapPartyLinkData.insert( std::make_pair( m_dwPartyLinkDataIndex, sPartyLink ) );
					nIndex = m_dwPartyLinkDataIndex;

					//	CHAT_LINK_DATA_MAX 사이즈만큼만 가지고 있는다.
					if ( m_mapPartyLinkData.size() > CHAT_LINK_DATA_MAX ) 
					{
						int nDelete = m_dwPartyLinkDataIndex - CHAT_LINK_DATA_MAX; 

						// 0보다 작다는것은 Index가 한바퀴 돌았다는 뜻이다.
						if ( nDelete < 0 ) 
						{
							nDelete += CHAT_LINK_INDEX_MAX;
						}
						
						MAP_PARTY_LINK_DATA_INDEX_ITER pos = m_mapPartyLinkData.find ( nDelete );	
						if ( pos != m_mapPartyLinkData.end() ) m_mapPartyLinkData.erase( pos );
					}

					m_dwPartyLinkDataIndex++;

					return nIndex;
				}
				break;

			case PARTY_LINK_REFUSE:
				{
					GLPARTY_LINK sPartyLink;
					memcpy ( &sPartyLink.sPartyMem[0].dwGaeaID, pBuffer, dwSize );

					//	Index를 모두 소모한 경우 0으로 초기화 
					if ( m_dwPartyLinkDataIndex == CHAT_LINK_INDEX_MAX )
					{
						m_dwPartyLinkDataIndex = 0;
					}

					m_mapPartyLinkData.insert( std::make_pair( m_dwPartyLinkDataIndex, sPartyLink ) );
					nIndex = m_dwPartyLinkDataIndex;

					//	CHAT_LINK_DATA_MAX 사이즈만큼만 가지고 있는다.
					if ( m_mapPartyLinkData.size() > CHAT_LINK_DATA_MAX ) 
					{
						int nDelete = m_dwPartyLinkDataIndex - CHAT_LINK_DATA_MAX; 

						// 0보다 작다는것은 Index가 한바퀴 돌았다는 뜻이다.
						if ( nDelete < 0 ) 
						{
							nDelete += CHAT_LINK_INDEX_MAX;
						}
						
						MAP_PARTY_LINK_DATA_INDEX_ITER pos = m_mapPartyLinkData.find ( nDelete );	
						if ( pos != m_mapPartyLinkData.end() ) m_mapPartyLinkData.erase( pos );
					}

					m_dwPartyLinkDataIndex++;

					return nIndex;
				}
				break;
			}
		}
		break;

	case QUEST_LINK:
	case SKILL_LINK:
	case CROW_LINK:
	case MAP_LINK:
		{
		}
		break;
	}

	return nIndex;
}

void CInnerInterface::ConvertLinkData ( const VEC_LINK_TEXT_DATA_EX& vecLinkDataIN, VEC_LINK_TEXT_DATA& vecLinkDataOut )
{
	// 클라이언트 링크 데이터를 서버데이터로 변환 
	for ( DWORD i =0; i < vecLinkDataIN.size(); ++i )
	{
		int nLinkIndex = AddLinkDataToList_Client( vecLinkDataIN[i].sLinkDataBasic );
		
		if ( nLinkIndex >= 0 ) 
		{
			SLINK_TEXT_DATA sLinkTextData;
			sLinkTextData.sLinkPos = vecLinkDataIN[i].sLinkPos;
			sLinkTextData.dwTextColor = vecLinkDataIN[i].dwTextColor;
			sLinkTextData.nLinkTypeIndex = nLinkIndex;

			vecLinkDataOut.push_back( sLinkTextData );
		}
	}	
}

// 서버에 보낼 정보로 변경한다.
void CInnerInterface::ConvertSendData(CString& strInput, const VEC_LINK_TEXT_DATA_EX& vecLinkData, VEC_LINK_DATA_BASIC& vecLinkDataBasic)
{
    size_t LinkDataSize = vecLinkData.size();
	if (LinkDataSize == 0)
        return;

	for ( int i = static_cast<int>(vecLinkData.size()-1); i >= 0; --i ) 
	{
		strInput.Delete(vecLinkData[i].sLinkPos.nStart, vecLinkData[i].sLinkPos.nEnd-vecLinkData[i].sLinkPos.nStart);

		CString strLink;
        
		strLink.Format(
            "%s%d%d%s",
            strLINK_LEFT_SIMBOL,
            vecLinkData[i].sLinkDataBasic.sLinkType.wMType, 
			vecLinkData[i].sLinkDataBasic.sLinkType.wSType,
            strLINK_RIGHT_SIMBOL);

		strInput.Insert(vecLinkData[i].sLinkPos.nStart, strLink);
	}

	for (size_t i=0; i<LinkDataSize; ++i) 
	{
		vecLinkDataBasic.push_back(vecLinkData[i].sLinkDataBasic);
	}
}


void CInnerInterface::ConvertRecvData( CString& strText, VEC_LINK_TEXT_DATA& vecLinkTextData )
{
	if ( vecLinkTextData.empty() ) return;

	int nStart = 0;
	CString strFind1 = strLINK_LEFT_SIMBOL;
	CString strFind2 = strLINK_RIGHT_SIMBOL;
	CString strCon;
	
	size_t nIndex = 0;

	while (true) 
	{
		int nFind1 = strText.Find ( strFind1, nStart );
		if ( nFind1 < 0 )	break;

		int nFind2 = strText.Find ( strFind2, nFind1 );
		if ( nFind2 < 0 )	break;

		if ( nFind1+2+strFind1.GetLength() != nFind2 )
		{
			nStart = ++nFind1;
			continue;
		}

		int mType = strText.GetAt(nFind1+1) - '0';
		int sType = strText.GetAt(nFind1+2) - '0';

		if ( static_cast<int>(vecLinkTextData.size()) <= nIndex )
		{
			nStart = ++nFind1;
			break;
		}

		SLINK_DATA_INDEX* sLinkDataIndex = GetLinkBasicData( vecLinkTextData[nIndex].nLinkTypeIndex );
		if ( !sLinkDataIndex )
		{
			nStart = ++nFind1;
			continue;		
		}

		if ( sLinkDataIndex->sLinkType.wMType != mType || sLinkDataIndex->sLinkType.wSType != sType ) 
		{
			nStart = ++nFind1;
			continue;	
		}
		
		if ( !GetLinkName( strCon, vecLinkTextData[nIndex].nLinkTypeIndex ) )
		{
			strCon = strLINK_LEFT_SIMBOL+strLINK_RIGHT_SIMBOL;
		}

		strText.Delete ( nFind1, nFind2-nFind1+strFind2.GetLength() );

		nStart = nFind1;

		strText.Insert ( nFind1, strCon );

		nStart = nFind1;
		nStart += strCon.GetLength();

		vecLinkTextData[nIndex].sLinkPos.nStart = nFind1;
		vecLinkTextData[nIndex].sLinkPos.nEnd = nStart;

		nIndex++;
	}

	if ( nIndex < static_cast<int>(vecLinkTextData.size()) )
	{
		vecLinkTextData.erase( vecLinkTextData.begin()+nIndex, vecLinkTextData.end() );
	}
}

//	무기가 바뀐다. ( 바뀔 예정값이 들어온다. ) 
void CInnerInterface::AutoSkillSolt( bool bMain )
{
	//	스킬 슬롯 체인지
	int nIndex = m_pSkillTrayTab->GetTabIndex();
	
	//	메인 무기
	if ( bMain ) 
	{
		//F3,F4 일경우 F1,F2로 간다.
		if ( nIndex == 2 ) nIndex = 0;
		if ( nIndex == 3 ) nIndex = 1;
		
		m_pSkillTrayTab->SetTabIndex( nIndex );
	}
	//	보조무기
	else
	{
		// F1,F2 일경우 F3,F4로 간다.
		if ( nIndex == 0 ) nIndex = 2;
		if ( nIndex == 1 ) nIndex = 3;

		m_pSkillTrayTab->SetTabIndex( nIndex );
	}	
}

void CInnerInterface::AutoArmSwap ()
{
	//if ( m_bArmsSwapReserve ) return;

	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	if ( !pCharacter ) return;

	if( pCharacter->IsReqSwapArms() )
	{
		return;
	}

	EMCHARCLASS emClass = pCharacter->m_emClass;
	if( emClass != GLCC_EXTREME_M && emClass != GLCC_EXTREME_W ) return;

	SNATIVEID sSkillID = pCharacter->GetskillRunSlot();

	if ( sSkillID == NATIVEID_NULL() ) return;

	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sSkillID.wMainID, sSkillID.wSubID );
	
	if ( !pSkill ) return;
	if ( pSkill->m_sBASIC.emROLE == SKILL::EMROLE_PASSIVE )	return;

	//	Note : 아이템 착용 조건 검사.
	//

	EMSLOT emRHand, emRHand_S;

	if ( pCharacter->IsUseArmSub() )
	{
		emRHand = SLOT_RHAND_S;
		emRHand_S = SLOT_RHAND;
	}
	else
	{
		emRHand = SLOT_RHAND;
		emRHand_S = SLOT_RHAND_S;
	}

	const bool bHiddenWeapon = pSkill->m_sLEARN.bHiddenWeapon;
	GLITEM_ATT emITEM_RIGHT = ITEMATT_NOTHING;
	SKILL::GLSKILL_ATT emSKILL_RIGHT = pSkill->m_sBASIC.emUSE_RITEM;

	if ( emSKILL_RIGHT!=SKILL::SKILLATT_NOCARE )
	{
		SITEM* pItem = pCharacter->GET_SLOT_ITEMDATA(emRHand);
		
		//	아이템이 없거나 스킬 구동 조건 아이템이 아닐때 
		if ( !pItem || !CHECHSKILL_ITEM(emSKILL_RIGHT,pCharacter->CONVERT_ITEMATT( pItem->sSuitOp.emAttack ),bHiddenWeapon) )
		{
			// Sub무기를 체크해서 맞을경우 스왑한다.
			SITEM* pItem_S = pCharacter->GET_SLOT_ITEMDATA(emRHand_S);
			if ( pItem_S && CHECHSKILL_ITEM(emSKILL_RIGHT,pCharacter->CONVERT_ITEMATT( pItem_S->sSuitOp.emAttack ),bHiddenWeapon) )
			{
				m_pGaeaClient->GetCharacter()->ReqSlotChange();
			}
		}
	}
}

void CInnerInterface::DoModal(const std::string& strText, int nModalTitle, int nModalType, UIGUID CallerID, BOOL bHide)
{    
    if (m_pModalWindow && !m_bBlockModalUI )
    {
        // UiHideGroup ( MODAL_WINDOW, true );
        if (UiIsVisibleGroup(MODAL_WINDOW))
        {
            // GASSERT ( 0 && "이미 켜져있습니다." );
            UiHideGroup(MODAL_WINDOW, true);
        }

        m_pModalWindow->SetHide(bHide);
        m_pModalWindow->DoModal(strText, nModalTitle, nModalType, CallerID);
        UiShowGroupFocus(MODAL_WINDOW, true);
    }
}

void CInnerInterface::DoModalTipBox ( const std::string& strText, const std::string& strTip, int nModalTitle, int nModalType, UIGUID CallerID, BOOL bHide )
{
    if (m_pModalWindow)
    {
        // UiHideGroup ( MODAL_WINDOW, true );
        if (UiIsVisibleGroup(MODAL_WINDOW))
        {
            // GASSERT ( 0 && "이미 켜져있습니다." );
            UiHideGroup(MODAL_WINDOW, true);
        }

        m_pModalWindow->SetHide(bHide);
        m_pModalWindow->DoModalTipBox(strText, strTip, nModalTitle, nModalType, CallerID);
        UiShowGroupFocus(MODAL_WINDOW, true);
    }
}

void CInnerInterface::DoConftModal ( const std::string& strText, UIGUID CallerID, CString strMyName, CString strHisName )
{
	IConftModalWindow* pConftModalWindow = GetConftModalWindow();
	if ( pConftModalWindow )
	{
        pConftModalWindow->DoModal( strText, CallerID, UI::ToString( strMyName ), UI::ToString( strHisName ) );
		UiShowGroupFocus ( CONFT_MODAL_WINDOW );
	}
}

void CInnerInterface::DoPartyModal( const std::string& strText, UIGUID CallerID, bool bExpedition )
{
	IPartyModalWindow* pPartyModalWindow = GetPartyModalWindow();
	if ( pPartyModalWindow )
	{
		pPartyModalWindow->SetPartyMode( bExpedition );
		pPartyModalWindow->DoModal( strText.c_str(), CallerID );
		UiShowGroupFocus( PARTY_MODAL_WINDOW );
	}
}

void CInnerInterface::DoPartyModal( const std::string& strText, UIGUID CallerID, bool bExpedition, DWORD dwCharDbNum )
{
    IPartyModalWindow* pPartyModalWindow = GetPartyModalWindow();
    if ( pPartyModalWindow )
    {
        pPartyModalWindow->SetPartyMode( bExpedition );
        pPartyModalWindow->DoModal( strText.c_str(), CallerID );
		pPartyModalWindow->SetCharDbNum( dwCharDbNum );
        UiShowGroupFocus( PARTY_MODAL_WINDOW );
    }
}

void CInnerInterface::DoPartyModal ( const std::string& strText, UIGUID CallerID, const SPartyOption& PartyOption, const bool bExpedition)
{
	IPartyModalWindow* pPartyModalWindow = GetPartyModalWindow();
	IPartyWindowRenewal* pPartyWindowRenewal = GetPartyWindowRenewal();
	if ( pPartyModalWindow == NULL || pPartyWindowRenewal == NULL )
		return;

	// bExpedition : 메세지 다르게 출력해야함;;

	pPartyModalWindow->DoModal( strText.c_str(), CallerID );
	pPartyModalWindow->SetPartyOption(PartyOption);
	UiShowGroupFocus( PARTY_MODAL_WINDOW );
}

void CInnerInterface::DoChatGroupModal( const std::string& strText, UIGUID CallerID, bool bExpedition, DWORD dwkey )
{
	IChatGroupModalWindow* pChatGroupModalWindow = GetChatGroupModalWindow();
	if ( pChatGroupModalWindow )
	{
		pChatGroupModalWindow->SetPartyMode( bExpedition );
		pChatGroupModalWindow->Setdwkey( dwkey );
		pChatGroupModalWindow->DoModal( strText.c_str(), CallerID );
		UiShowGroupFocus( CHAT_GROUP_MODAL_WINDOW );
	}
}

void CInnerInterface::SetReviveSkill( bool bAct )
{
    if ( IsVisible( REBIRTH_WINDOW ) || IsVisible( FORCE_REBIRTH ) == FALSE )
    {
        if ( m_pRebirthWindow )
            m_pRebirthWindow->SetReviveSkill( bAct );
    }
    else if ( IsVisible( FORCE_REBIRTH ) )
    {
        std::string strTemp = sc::string::format( "%s", ID2GAMEINTEXT("ForceRebirthSkill") );
        DoModal( strTemp, UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_FORCE_REBIRTH_SKILL );
    }
}

void CInnerInterface::ReloadPointShopItemList()
{
    if ( m_pPointShopWindow )
        m_pPointShopWindow->ReLoadAll();
}

void CInnerInterface::PointShopSetPoint( __int64 nCurPoint )
{
    if ( m_pPointShopWindow )
        m_pPointShopWindow->SetPointText( nCurPoint );
}

__int64 CInnerInterface::PointShopGetPriceInCart ()
{
    if ( m_pPointShopWindow )
        return m_pPointShopWindow->GetPriceInCart();

    return 0;
}

void CInnerInterface::PointShopResetCartItem( SNATIVEID sItemID )
{
    if ( m_pPointShopWindow )
        m_pPointShopWindow->ResetCartItem( sItemID );
}

void CInnerInterface::SetForceClosePointShopWindow( bool _bForceClose )
{
    if ( m_pPointShopWindow )
        m_pPointShopWindow->SetForceCloseWindow( _bForceClose );
}

bool CInnerInterface::IsForceClosePointShopWindow()
{
    if ( m_pPointShopWindow )
        return m_pPointShopWindow->IsForceCloseWindow();

    return false;
}

void CInnerInterface::SetForceResurrect( const float _fTime )
{
    if ( m_pForceRebirth )
    {
        m_pForceRebirth->SetForceRebirth( _fTime );
    }
}

//! CTF Result Window
void CInnerInterface::CTFResultWindowShow()
{
	if ( m_pCTFResultWindow )
	{
        CloseAllWindow();
		m_pCTFResultWindow->Show();
	}
}

void CInnerInterface::CTFResultWindowUpdate()
{
	if ( m_pCTFResultWindow )
	{
		m_pCTFResultWindow->Update();
	}
}

void CInnerInterface::CheckCTFSpurt()
{
    if ( m_pNotifyCTFSpurt )
        m_pNotifyCTFSpurt->CheckCTFSpurt();
}

void CInnerInterface::ctf_display_hide()
{
	// TODO : #rm 695
	if( m_pProgressDisplay )
	{
		MyProgressDisplay *pProgress_ctf = dynamic_cast<MyProgressDisplay *>(m_pProgressDisplay);

		if(pProgress_ctf) {
			pProgress_ctf->ctf_hide_display();
		}
	}

	if ( m_pNotifyCTFSpurt )
		m_pNotifyCTFSpurt->SetVisibleSingle(FALSE);
}

BOOL CInnerInterface::IsInventoryOpen()
{
	if( GLUseFeatures::GetInstance().IsUsingRenewInvenWindow() )
	{
		if( UiIsVisibleGroup( INVENTORY_WINDOW_RENEW ) &&
			m_pInventory->GetTapSelected() == CRnInventoryWindow::ETapInven )
		{
			return TRUE;
		}
	}
	else
	{
		if( UiIsVisibleGroup( INVENTORY_WINDOW ) )
			return TRUE;
	}

	return FALSE;
}

BOOL CInnerInterface::IsItemDecompose(void)
{
	return (m_pInventory && m_pInventory->IsItemDecompose()) ? TRUE : FALSE;
}

BOOL CInnerInterface::IsBankOpen()
{
	if( GLUseFeatures::GetInstance().IsUsingRenewInvenWindow() )
	{
		if( UiIsVisibleGroup( INVENTORY_WINDOW_RENEW ) &&
			m_pInventory->GetTapSelected() == CRnInventoryWindow::ETapBank )
		{
			return TRUE;
		}
	}
	else
	{
		if( UiIsVisibleGroup( ITEMBANK_WINDOW ) )
			return TRUE;
	}

	return FALSE;
}

BOOL CInnerInterface::IsWishListOpen()
{
	if( GLUseFeatures::GetInstance().IsUsingRenewInvenWindow() )
	{
		if( UiIsVisibleGroup( INVENTORY_WINDOW_RENEW ) &&
			m_pInventory->GetTapSelected() == CRnInventoryWindow::ETapWish )
		{
			return TRUE;
		}
	}

	return FALSE;
}

void CInnerInterface::UpdateExChangeList()
{
	if ( m_pDialogueWindowRenewal )
		m_pDialogueWindowRenewal->UpdateExChangeTab();
}

void CInnerInterface::ResetCartListInDialogueWindowMarketPage( int nPageIndex, int nPosX, int nPosY )
{
    if ( m_pDialogueWindowRenewal )
        m_pDialogueWindowRenewal->ResetCartListInDialogueWindowMarketPage( nPageIndex, nPosX, nPosY );
}

void CInnerInterface::DialogueWindowLoadRebuyList()
{
    if ( m_pDialogueWindowRenewal )
        m_pDialogueWindowRenewal->LoadRebuyList();
}

void CInnerInterface::DialogueWindowCheckRebuySupplies( bool bCheck )
{
    if ( m_pDialogueWindowRenewal )
        m_pDialogueWindowRenewal->SetCheckRebuySupplies( bCheck );

}

void CInnerInterface::DialogueWindowSetRebuyCountText( int nCount )
{
    if ( m_pDialogueWindowRenewal )
        m_pDialogueWindowRenewal->SetRebuyTabCountText( nCount );
}

int CInnerInterface::GetDialogueWindowRebuyItemCount()
{
    if ( m_pDialogueWindowRenewal )
        return m_pDialogueWindowRenewal->GetRebuyItemCount();
    
    return 0;
}

void CInnerInterface::GetDialogueWindowMarketItemIndex( int* pnPosX, int* pnPosY )
{
    if ( m_pDialogueWindowRenewal )
        m_pDialogueWindowRenewal->GetItemIndex( pnPosX, pnPosY );
}

int CInnerInterface::GetDialogueWindowPageIndex()
{
    if ( m_pDialogueWindowRenewal )
        return m_pDialogueWindowRenewal->GetPageIndex();

    return nOUTOFRANGE;
}

bool CInnerInterface::IsOpenDialogueWindowMarketPage()
{
    if ( !UiIsVisibleGroup( DIALOGUE_WINDOW_RENEWAL ) )
        return false;

    if ( m_pDialogueWindowRenewal )
        return m_pDialogueWindowRenewal->IsOpenMarketPage();

    return false;
}

void CInnerInterface::SetEnableDialogueWindowMarketPage(bool bEnable)
{
    if (m_pDialogueWindowRenewal)
        m_pDialogueWindowRenewal->SetEnableMarketPage(bEnable);
}

void CInnerInterface::ProductWindowSetSeletedTabID( UIGUID ControlID )
{
	//탭이 변경되었을때
	if( m_pProductWindow )
		m_pProductWindow->SetSeletedTabID( ControlID );
}

void CInnerInterface::ProductWindowSetSeletedItem( SProductRecipe* pProductRecipe )
{
    if( pProductRecipe == NULL )
        return;

	//제조 목록에서 아이템을 선택했을때
	if( m_pProductWindow )
		m_pProductWindow->SetItemMix( pProductRecipe );
}

void CInnerInterface::MsgWishListInsert( BYTE nType )
{
	if( !m_pInventory )
		return;

	// 인벤토리와 뱅크에서 위시리스트로 담을때는 탭 변경하지 말자
	if( nType == WishList::ETypeInventory ||
		nType == WishList::ETypeBank )
	{
		return;
	}

	if( !UiIsVisibleGroup( INVENTORY_WINDOW_RENEW ) )
		UiShowGroupFocus( INVENTORY_WINDOW_RENEW );

	m_pInventory->CallTapSelect( CRnInventoryWindow::ETapWish );
}

void CInnerInterface::DeleteUIMeshData()
{
    if( m_pCharacterWindow )
	    m_pCharacterWindow->DeleteUIMeshData();
    if( m_pItemPreviewWindow )
	    m_pItemPreviewWindow->DeleteUIMeshData();
    if( m_pReferCharWindow )
        m_pReferCharWindow->DeleteUIMeshData();
}

void CInnerInterface::OpenPopupMenu( popupmenu::SP_POPUP_MENU PopupMenu )
{
    if ( m_pPopupMenu )
        m_pPopupMenu->OpenPopup( PopupMenu );
}

void CInnerInterface::UpdateWhenReceiveDamage()
{
    // TODO : 피격 시에 UI에서 해 줄 일들.
}

void CInnerInterface::SetVisibleSkillEffectPartyStateWindow(const BOOL bChecked)
{
	m_pPartyStateWindow->SetVisibleSkillEffect(bChecked);
}

const DWORD CInnerInterface::AddPartyYesNoComponent(const std::string& strDescription, PARTY_YESNO_FUNCTION PartyYesNoFunction)
{
	return m_pPartyYesNoWindow->AddComponent(strDescription, PartyYesNoFunction);
}

void CInnerInterface::OpenPartyTenderNotifyButton( bool bShow )
{
	if( bShow )
	{
		UiShowGroupFocus( PARTY_TENDER_NOTIFY_BUTTON );
	}
	else
	{
		UiHideGroup( PARTY_TENDER_NOTIFY_BUTTON );
	}
}

void CInnerInterface::PartyTenderNotify( bool bIsNew )
{
	m_pPartyTenderNotifyButton->Notify( bIsNew );
}

void CInnerInterface::SetPostWindowClose()
{
    if( m_pPostBoxWindow )
        m_pPostBoxWindow->Close();
}

void CInnerInterface::ShowProgessTimer( UINT iTime,float fPosX, float fPosY, const char* szText, int nTextIndex )
{
	if ( m_pProgessTimer )
	{
		UiShowGroupFocus( PROGESS_TIMER );
		m_pProgessTimer->SetProgessTimer( iTime, fPosX, fPosY, szText, nTextIndex );
	}
}

void CInnerInterface::HideProgessTimer()
{
	UiHideGroup(PROGESS_TIMER);
}

void CInnerInterface::PrintSimpleSystemMessage( const CString& strMessage, const D3DCOLOR dwMessageColor, const DWORD dwFontSize, const float fLifeTime )
{
    m_pSimpleSystemMessageDisplay->PrintSimpleSystemMessage(strMessage, dwMessageColor, dwFontSize, fLifeTime);
}

void CInnerInterface::ClearSimpleSystemMessage()
{
    m_pSimpleSystemMessageDisplay->ClearSimpleSystemMessage();
}

void CInnerInterface::ShowSimpleTimerMessageBox(const NET_MSG_GENERIC* pMsg)
{
    m_pSimpleTimerMessageBox->ShowSimpleTimerMessageBox(pMsg);
}

void CInnerInterface::CloseSimpleTimerMessageBox()
{
    m_pSimpleTimerMessageBox->Close();
}

void CInnerInterface::RefreashCdmProgress( DWORD dwNumberOneClubMarkVER, DWORD dwNumberOneClubDbNum, char* szNumberOneName, int nNumberOneScore, int nMyRank, DWORD dwMyScore, int nHour, int nMin, int nSecond )
{
	m_pRnCDMProgress->RefreashCdmProgress( dwNumberOneClubMarkVER, dwNumberOneClubDbNum, szNumberOneName, nNumberOneScore, nMyRank, dwMyScore, nHour, nMin, nSecond );

}

void CInnerInterface::SetRankSloatData( InstanceSystem::RNCDM_RANK_SLOT vecRnCdmRankSlot )
{
	m_pRnCDMRankWindow->SetRankSloatData( vecRnCdmRankSlot );
}

void CInnerInterface::SetCLubMemberInfo( std::vector<InstanceSystem::CDM_MEMBER_INFO> vecCdmMemberInfo )
{
	int nIndex = 0;
	BOOST_FOREACH(InstanceSystem::CDM_MEMBER_INFO sCdmMemberInfo, vecCdmMemberInfo)
	{
		m_pRnCDMClubInfo->SetMemberImage( nIndex, static_cast<EMCHARINDEX>(sCdmMemberInfo.nEmCharIndex) );
		m_pRnCDMClubInfo->SetMemberName( nIndex, sCdmMemberInfo.strCharName.c_str() );
		nIndex++;
	}
}

InstanceSystem::RNCDM_RANK_SLOT CInnerInterface::GetRankSlotData( int nIndex )
{
	return m_pRnCDMRankWindow->GetRankSlotData( nIndex );
}

void CInnerInterface::RankListUpdate()
{
	m_pRnCDMRankWindow->RankListUpdate();
}

void CInnerInterface::SetTitleClubNameMemberNum( const CString strClubName, int nMembernum )
{
	m_pRnCDMClubInfo->SetTitleClubNameMemberNum( strClubName, nMembernum );
}

void CInnerInterface::SetCDMMapinfo( std::string strMapName, int nTimeLimit, int nPlayerLimite, int nLevelLimit )
{
	m_pRnCompetitionWindow->SetCDMMapinfo( strMapName, nTimeLimit, nPlayerLimite, nLevelLimit );
}

void CInnerInterface::SetCDMMapImageName( std::string strMapImageName )
{
	m_pRnCompetitionWindow->SetCDMMapImageName( strMapImageName );
}

void CInnerInterface::SetCDMBattelState( int nStartTime, int nEndTime )
{
	m_pRnCompetitionWindow->SetCDMBattelState( nStartTime, nEndTime );
}

void CInnerInterface::SetRnCDMClubInfoPos( int nPosX, int nPosY )
{
	m_pRnCDMClubInfo->SetRnCDMClubInfoPos( nPosX, nPosY );
}

void CInnerInterface::OpenCDMTopRanking()
{
	m_pRnCDMTopRanking->OpenCDMTopRanking();
}

void CInnerInterface::SetCDMTopList(std::vector<DWORD> vecClubId,
									LONGLONG lnStartTime,
									LONGLONG lnEndTime,
									std::vector<DWORD> vecRank )
{
	m_pRnCDMTopRanking->SetCDMTopList(vecClubId, lnStartTime, lnEndTime, vecRank );
}

void CInnerInterface::SetClubMasterClubName( std::vector<std::string> vecMasterName, std::vector<std::string> vecClubName )
{
	m_pRnCDMTopRanking->SetClubMasterClubName( vecMasterName, vecClubName );
}

void CInnerInterface::SetFlagGuideArrow( const int _flagNum, const STARGETID& sTargetID, const bool bBlink )
{
    m_pCaptureTheFlagEffecMan->SetFlagGuideArrow(_flagNum, sTargetID, bBlink);
}

void CInnerInterface::ShowRnCompetitionRewardTooltop(const int emTitleType, const InstanceSystem::RnCompetitionResultContentRewardObj* pReward)
{
    m_pRnCompetitionResultWindow->ShowRnCompetitionRewardTooltop(emTitleType, pReward);
}

const bool CInnerInterface::msgProc_RnCompetitionResult(const NET_MSG_GENERIC* _pMessage)
{
    return m_pRnCompetitionResultWindow->msgProc_RnCompetitionResult(_pMessage);
}

void CInnerInterface::SetGuidanceClubBattle_UI_Auth(const bool bVisible, const float fAuthTime)
{
    m_pGuidanceAuthProgressUI->SetAuthTime(bVisible, fAuthTime);
}

void CInnerInterface::SetGuidanceClubBattle_ProgressInfo(const NET_MSG_GENERIC* _pMessage)
{
    m_pGuidanceProgressInfoUI->SetProgressInfo(_pMessage);
}

void CInnerInterface::SetGuidanceClubBattle_RankInfo(const NET_MSG_GENERIC* _pMessage)
{
    m_pGuidanceRankInfoUI->SetRankInfo(_pMessage);
}

void CInnerInterface::SetGuidanceClubBattle_RankMyInfo(const NET_MSG_GENERIC* _pMessage)
{
    m_pGuidanceRankInfoUI->SetRankMyInfo(_pMessage);
}

void CInnerInterface::SetGuidanceClubBattle_AuthInfo(const NET_MSG_GENERIC* _pMessage)
{
    m_pMapWindowInterface->MsgMapGuidanceAuthInfo(_pMessage);
}

void CInnerInterface::SetCTI_UI_Auth(const bool bVisible, const float fAuthTime)
{
	m_pCTIAuthProgressUI->SetAuthTime(bVisible, fAuthTime);
}

void CInnerInterface::SetCTI_ProgressInfo(const NET_MSG_GENERIC* _pMessage)
{
	m_pCTIProgressInfoUI->SetProgressInfo(_pMessage);
}

void CInnerInterface::SetCTI_RankInfo(const NET_MSG_GENERIC* _pMessage)
{
	m_pCTIRankInfoUI->SetRankInfo(_pMessage);
}

void CInnerInterface::SetCTI_RankMyInfo(const NET_MSG_GENERIC* _pMessage)
{
	m_pCTIRankInfoUI->SetRankMyInfo(_pMessage);
}

void CInnerInterface::SetCTI_AuthInfo ( const NET_MSG_GENERIC* pMsg )
{
	m_pMapWindowInterface->MsgMapCTIAuthInfo( pMsg );
}

void CInnerInterface::OpenTournamentDashboard(DWORD dwPageNum)
{
	m_pTournamentDashBoard->SetPage(dwPageNum);
	m_pTournamentDashBoard->OnVisible(true);
	
}

void CInnerInterface::OpenTournamentBettingWindow(DWORD dwKagNum,DWORD dwPageNum)
{
	m_pTournamentToToWindow->SetPage(dwKagNum,dwPageNum);
	m_pTournamentToToWindow->OnVisible(true);
	
}

void CInnerInterface::OpenTournamentMoreInfoWindow(DWORD dwPageNum)
{
	m_pTournamentMoreInfoWindow->SetPage(dwPageNum);
	m_pTournamentMoreInfoWindow->OnVisible(true);
}


void CInnerInterface::OpenTournamentUI_HUD(bool isObserver)
{
	
	if(m_pTournamentUI != NULL)
		if(isObserver == true)
			m_pTournamentUI->OnVisible(isObserver);

	if(m_pTournamentUserInterface != NULL)
		if(isObserver == false)
			m_pTournamentUserInterface->SetCloseAll();

}

void CInnerInterface::OpenTournamentWinLose(bool isWin)
{
	m_pTournamentUI->WinLoseScoreVisible(isWin);
}

void CInnerInterface::ShowDoublePoint(BOOL bVisible)
{
	if ( bVisible )
	{
		UiShowGroupFocus ( DOUBLE_POINT );
		m_pDoublePoint->START ( 1 );
		m_pDoublePoint->KEEP_START ( 1 );
	}
	else
	{
		UiHideGroup ( DOUBLE_POINT );
	}
}

void CInnerInterface::OpenSelectiveformBox( std::vector<ITEM::SBOX_ITEM>& vecItemID )
{
	m_pSelformBoxWindow->Open(vecItemID);
}

void CInnerInterface::ToggleJoinWorldBattleButton(bool bOn)
{
    if ( m_pWorldBattleButton )
    {
        m_pWorldBattleButton->ToggleWorldBattleButton(bOn);
    }
}
