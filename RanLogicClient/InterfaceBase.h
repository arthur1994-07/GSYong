#ifndef _INTERFACE_BASE_H_
#define _INTERFACE_BASE_H_

#pragma once

#include <boost/tr1/memory.hpp>
#include "../EngineLib/GUInterface/UIDataType.h"
#include "../EngineLib/G-Logic/GLDefine.h"
#include "../EngineLib/GUInterface/InterfaceBaseDefine.h"
//#include "./InterfaceBaseDefine.h"
#include "../RanLogic/GLChatLinkDef.h"
#include "../RanLogic/GLCompetition.h"
#include "../RanLogic/Party/GLPartyDefine.h"
#include "../RanLogic/MatchSystem/MatchModel.h"
#include "../RanLogic/InstanceSystem/GLInstanceSystemDefine.h"

class GLClubClient;
class GLMapAxisInfo;
class GLClub;

//class GLMapAxisInfo;
class GLLevelFileClient;


class GLCharClient;
class GLGaeaClient;
class SPartyOption;
struct GLCHARLOGIC;
struct SITEMCUSTOM;
struct SNpcTalk;
struct CROWREN;

namespace InstanceSystem
{
	struct RNCDM_RANK_SLOT;
    struct RnCompetitionResultContentRewardObj;
    struct RnCompetitionResultTitleObj;
}
//struct SLINK_DATA_BASIC;
//struct SLINK_DATA_INDEX;
//struct SLINK_TYPE;

class InterfaceBase
{
public:
    InterfaceBase(GLGaeaClient* pGaeaClient);
    virtual ~InterfaceBase();

protected:
    GLGaeaClient* m_pGaeaClient;

public :
	GLGaeaClient* GetGaeaClient() { return m_pGaeaClient; }

public:  
    virtual void UiOneTimeSceneInit() = 0;
    virtual HRESULT UiFinalCleanup() = 0;
    virtual	HRESULT UiFrameMove(LPDIRECT3DDEVICEQ pd3dDevice, float fElapsedTime) = 0;
    virtual HRESULT UiRender(LPDIRECT3DDEVICEQ pd3dDevice) = 0;
    virtual HRESULT UiInitDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice) = 0;
    virtual HRESULT UiRestoreDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice) = 0;
    virtual HRESULT UiInvalidateDeviceObjects() = 0;
    virtual HRESULT UiDeleteDeviceObjects() = 0;    
    virtual void    UiMsgProcess( LPVOID msgBuffer ) = 0;

    virtual void PrintMsgText(D3DCOLOR dwColor, const std::string& strText) = 0;
    virtual void PrintConsoleText(const std::string& strText) = 0;
    virtual void PrintMsgTextDlg(D3DCOLOR dwColor, const std::string& strText) = 0;
    virtual void PrintConsoleTextDlg(const std::string& strText)  = 0;
    virtual void PrintMsgDlg( D3DCOLOR dwColor, const std::string& strText) = 0;
	virtual void PrintAddString( D3DCOLOR dwColor, const std::string& strText) = 0;

    virtual void DisplayChatMessage(int nType, const char *szName, const char *szMsg) = 0;
    virtual void DisplayChatPartyPickupLinkMessage(const CString& strName, SITEMCUSTOM& sItemCustom) = 0;
    virtual void DisplayChatLinkMessage(LPVOID msgBuffer) = 0;

    virtual void UiShowGroupBottom(UIGUID ControlID, bool bMakeMsg = false) = 0;
    virtual void UiShowGroupFocus(UIGUID ControlID, bool bMakeMsg = false) = 0;
    virtual BOOL UiIsVisibleGroup(UIGUID ControlID) = 0;
    virtual void UiShowGroupTop(UIGUID ControlID, bool bMakeMsg = false) = 0;
	virtual void UiShowGroupUpdate(UIGUID ControlID, bool bMakeMsg = false) = 0;
    virtual void UiHideGroup(UIGUID ControlID, bool bMakeMsg = false) = 0;
	virtual void UiRefresh( UIGUID ControlID ) = 0;
	virtual void DeleteUIMeshData() = 0;

    virtual void DoModal(const std::string& strText, int nModalTitle = UI::MODAL_NOTITLE, int nModalType = UI::OK, UIGUID CallerID = NO_ID, BOOL bHide = FALSE)                                     = 0;
    virtual void DoModalTipBox (const std::string& strText, const std::string& strTip, int nModalTitle = UI::MODAL_NOTITLE, int nModalType = UI::OK, UIGUID CallerID = NO_ID, BOOL bHide = FALSE)   = 0;
    virtual void DoConftModal ( const std::string& strText, UIGUID CallerID, CString strMyName, CString strHisName )													                            = 0;
	virtual void DoPartyModal ( const std::string& strText, UIGUID CallerID, bool bExpedition ) = 0;
	virtual void DoPartyModal (const std::string& strText, UIGUID CallerID, const SPartyOption& PartyOption, const bool bExpedition = false) = 0;
	virtual void DoChatGroupModal (const std::string& strText, UIGUID CallerID, bool bExpedition, DWORD dwkey ) = 0;

    virtual BOOL IsVisible(UIGUID ControlID) = 0;
    virtual void SetInventorySlotViewSize(const int& nSLOT_VIEW_SIZE) = 0;
    virtual void SetPtoPWindowOpen(std::tr1::shared_ptr<GLCharClient> pCharClient, DWORD dwGaeaID, GLClubClient* pMyClub) = 0;
    virtual bool IsCHAT_BEGIN() = 0;
    virtual const bool IsInventoryLocked () = 0;

    virtual bool IsOpenWindowFromNPC   () = 0;
    virtual void CloseAllWindowFromNPC () = 0;

    virtual void UpdateWhenReceiveDamage() = 0;

	//! Reserved Skill Animation
	virtual void ReservedSkillAnimationShow( const SNATIVEID& sSkillID ) = 0;
	virtual void ReservedSkillAnimationHideAll() = 0;

	//! Key Bind
	virtual void ResetKeyBinding() = 0;

	//! Open/Close Window
	virtual void ToggleWindowEscMenu() = 0;
	virtual void ToggleWindowInventory() = 0;
	virtual void ToggleWindowCharacter() = 0;
	virtual void ToggleWindowSkill() = 0;
	virtual void ToggleWindowMiniGame(const DWORD dwMiniGameID = GAEAID_NULL) = 0;
	virtual void ToggleWindowParty() = 0;	
	virtual void ToggleWindowYesNoParty(const DWORD nID) = 0;
	virtual void ToggleWindowClub() = 0;
	virtual void ToggleWindowQuest() = 0;
	virtual void ToggleWindowFriend() = 0;
	virtual void ToggleWindowItemBank() = 0;
	virtual void ToggleWindowMiniMap() = 0;
	virtual void ToggleWindowPointshop() = 0;
	virtual void ToggleWindowWebItemShop() = 0;
	virtual void ToggleWindowChatMacro() = 0;
	virtual void ToggleWindowSNS() = 0;
	virtual void ToggleWindowHelp() = 0;
	virtual void ToggleWindowQBox() = 0;
	virtual void ToggleWindowPet() = 0;
	virtual void ToggleWindowVehicle() = 0;
	virtual void ToggleWindowSchoolRecord() = 0;
	virtual void ToggleWindowBattleGround() = 0;
	virtual void ToggleWindowProduct() = 0;
	virtual void ToggleWindowProductReset(WORD wResetAbleNum, WORD wPosX, WORD wPosY) = 0;
	virtual void ToggleWindowDice() = 0;
	virtual void ToggleWindowDPS(void) = 0;
	virtual void ToggleWindowFlyCameraControl(void) = 0;
	virtual void ToggleWindowTenderItem(const bool bShow) = 0;
	virtual void ToggleWindowMasterTransfer(const bool bShow) = 0;
	virtual void ToggleWindowExpeditionInvite(void) = 0;
	virtual void ToggleWindowExpeditionCheckReady(const bool bShow) = 0;
	virtual void ToggleWindowColorProperty(const bool bShow) = 0;

	virtual void SetitemGroupComposition(SSETITEM* pSetitem, STARGETID sTargetID = STARGETID()) = 0;
	virtual void ResetSetitemUIComposition(void) = 0;
	virtual void OpenSetitemPreviewWindow() = 0;

	virtual void WaitBattleServerDialogueOpen( bool bOpen ) = 0;

	//! TexasHoldem Open
	virtual void OpenTexasHoldemWindow() = 0;
	virtual void CloseTexasHoldemWindow() = 0;

	virtual void RunTexasHoldemWindow() = 0;
	virtual void StopTexasHoldemWindow() = 0;
	virtual void SetTexasHoldemURL( const std::string& strURL, const std::string& strPOST ) = 0;

	//! Quick Potion Slot
	virtual void QuickPotionSlotSetIndex(int nIndex) = 0;

	//! SkilSlotTray
	virtual int  SkilSlotTrayGetTab() = 0;
	virtual void SkilSlotTraySetTab( int nIndex ) = 0;
	virtual void SkilSlotTraySelectSlot( int nIndex ) = 0;

    //! Tutorial
    virtual void SetTutorialDialogueEnable(bool bSet) = 0;
    virtual void SetTutorialDialogueIndex(DWORD Index) = 0;
    virtual void TutorialDialogueTextStepInc() = 0;
    virtual void TutorialDialogueSoundIndexSet(DWORD Index) = 0;
    virtual DWORD GetTutorialDialogueSoundIndex() = 0;
    virtual void OpenMouseTip() = 0;
    virtual void OpenTutorialDialogue() = 0;
    virtual void OpenTutorialDialogueExplainText() = 0;
    virtual void OpenTutorialInterfaceLeft() = 0;
    virtual void OpenTutorialInterfaceRight() = 0;
    virtual void OpenTutorialInterfaceUp() = 0;
    virtual void OpenTutorialInterfaceDown() = 0;
    virtual void OpenTutorialInterfaceZoomIn() = 0;
    virtual void OpenTutorialInterfaceZoomOut() = 0;
    virtual void OpenBalloonText() = 0;
    virtual void OpenBalloonText(UIRECT rcWindow) = 0;
    virtual void OpenTutorialViewStep() = 0;
    virtual void SetTutorialDialogText(const CString& strText) = 0;
    virtual void SetTutorialViewStep(int Index) = 0;

	virtual void ClearNameList() = 0;
	virtual void CloseAllWindow() = 0;
	virtual const bool CloseFocusWindow() = 0;
    
    //! Minimap
    virtual void SetMiniMapInfo(const GLLevelFileClient& levFile, CString strMapName) = 0;
    virtual void DisableMinimapTarget() = 0;
    virtual bool MinimapTargetIsEnable() const = 0;
    
    virtual void WARNING_MSG_ON() = 0;
    virtual void WARNING_MSG_OFF()  = 0;
    virtual BOOL IsGateOpen() const  = 0;
    virtual void SetGateOpen(BOOL bGateOpen) = 0;

    virtual void UpdateClubGuidBattleTime( const std::vector<float> &vecTime ) = 0;
    virtual void UpdateClubDeathMatchTime( const float fTime ) = 0;
    virtual void SetClubGuidBattleName( const std::vector<std::string> &vecName ) = 0;
    virtual void SetVisibleMiniMapClubGuidBattleTime( const BOOL bVisible ) = 0;
    virtual void SetVisibleMiniMapClubDeathMatchTime( const BOOL bVisible ) = 0;

    virtual void ClearItemBank() = 0;
    virtual void ClearVNGainSys() = 0;
    virtual void SetTradeWindowCloseReq() = 0;
    virtual void SetDuplicateLogin(const BOOL bDuplicateLogin) = 0;
    virtual void SetIPBonusClass(int nClass) = 0;

    virtual void VisibleCDMRanking(bool bVisible) = 0;
    virtual void RefreashCDMRanking() = 0;
    virtual void VisibleCDMSafeTime(bool bVisible, float fCDM_SAFE_TIME) = 0;
    virtual void CalcSkillDisplayPos(bool bVisible) = 0;

    virtual void SET_COUNT_MSG(INT nCount) = 0;
    virtual void SetMapCheckType(UI::EMMAPCHECK_TYPE emMapCheckType) = 0;

    //! Pet
    virtual void SetPetHungryItem( SNATIVEID sPetItemID ) = 0;
    virtual void SetPetHungry() = 0;
    virtual void PetWindowSetSkillCombo() = 0;
    virtual void PetSkinMixImageSetItemData(WORD wPosX, WORD wPosY, SNATIVEID sItemData, WORD wHoldPosX, WORD wHoldPosY ) = 0;
    virtual void PetSatiationUpdate( int Satiation ) = 0;

    virtual void SetDamage(D3DXVECTOR3 vPos, int nDamage, DWORD dwDamageFlag, BOOL bAttack) = 0;
	virtual void SetTargetInfoMouseOver(STARGETID sTargetID) = 0;
	virtual void SetTargetInfoSelect(STARGETID sTargetID) = 0;
	virtual void ResetTargetInfoMouseOver() = 0;
	virtual void ResetTargetInfoSelect() = 0;
	virtual const bool IsVisibleTargetInfoMouseOver() = 0;

    virtual void SetFightBegin(const int nIndex) = 0;
    virtual void SetFightEnd(const int nResult) = 0;

    virtual void SetAcademyFightBegin(const int& nLMARK, const int& nLNUMBER, const int& nRMARK, const int& nRNUMBER) = 0;
    virtual void SetAcademyUpdateNumber(const int& nLNUMBER, const int& nRNUMBER) = 0;
    virtual void SetAcademyFightEnd() = 0;

    virtual void SetBlockProgramFound(bool bFOUND) = 0;
    virtual bool IsBlockProgramFound() = 0;    

    virtual void SET_VNGAINTYPE_GAUGE(int nPos, int nLimit) = 0;

    virtual bool SET_QUEST_HELPER(DWORD dwQuestID, bool bPrintMsg = TRUE ) = 0;
    virtual void RESET_QUEST_HELPER(DWORD dwQuestID) = 0;
    virtual void QUEST_HELPER_UPDATE () = 0;

    virtual void SetShotcutText(DWORD nID, CString& strTemp) = 0;
    virtual void GetShotCutKey() = 0;
    virtual void GetChatMacro() = 0;
    virtual void AddChatMacro(int nIndex) = 0;
    virtual void ChatLog(bool bChatLogt, int nChatLogType) = 0;
    virtual CString GET_RECORD_CHAT() = 0;
    virtual CString GetdwKeyToString(int dwKey) = 0;

    virtual void SET_CONFT_CONFIRM_VISIBLE(bool bVisible) = 0;
    virtual void SET_CONFT_CONFIRM_PERCENT(float fPercent) = 0;

    virtual void SetSkillUpID(const DWORD& dwID) = 0;
    virtual DWORD GetSkillUpID() = 0;

    virtual void DOMODAL_ADD_FRIEND_REQ(const CString& strName) = 0;
    virtual void DOMODAL_CLUB_JOIN_ASK(const DWORD& dwMasterID, const CString& strClubName, const CString& strClubMaster) = 0;
    virtual void DOMODAL_UNION_JOIN_ASK(const DWORD& dwMasterID, const CString& strClubMaster) = 0;
    virtual void DOMODAL_CLUB_BATTLE_ASK(const DWORD& dwMasterID, const CString& strClubMaster, DWORD dwBattleTime, bool bAlliance = false) = 0;
    virtual void DOMODAL_CLUB_BATTLE_ARMISTICE_ASK(DWORD dwClubID, DWORD ReqChaDbNum, const CString& strClubName, bool bAlliance = false) = 0;
    virtual void DOMODAL_CLUB_AUTHORITY_ASK(const CString& strClubName) = 0;

    virtual void OPEN_MODAL(const CString& strText, int nModalTitle, int nModalType, UIGUID CallerID = NO_ID, BOOL bHide = FALSE) = 0;
    virtual void OPEN_MODALESS(const CString& strText, int nModalTitle, int nModalType, UIGUID CallerID = NO_ID, BOOL bHide = FALSE) = 0;
    virtual void CLOSE_MODAL(const UIGUID cID, bool bMakeMsg) = 0;

    virtual void AUTO_LOAD_QUEST( DWORD dwID ) = 0;
    virtual void REFRESH_QUEST_WINDOW() = 0;

	//! FlyCamera
	virtual const D3DXVECTOR3& GetFlyCameraUILastInputPos(void) = 0;

    //! Character
    virtual void CharacterSetPhoneNumber(const CString& PhoneNumber) = 0;

    //! ItemSearch
    virtual void ItemSearchResultUpdate(SSEARCHITEMESULT* pSearchItemResult, DWORD dwUpdateNum, DWORD dwPageNum) = 0;

    //! Summon
    virtual void SummonSetCount(DWORD Count) = 0;
    virtual void SummonAddDisp(DWORD GlobalId) = 0;

    //! Friend
    virtual void REFRESH_FRIEND_LIST() = 0;
    virtual void REFRESH_FRIEND_STATE() = 0;
    virtual void FRIEND_LIST(CONST CString & strName, bool bOnline) = 0;

    virtual void REFRESH_CLUB_LIST() = 0;
    virtual void REFRESH_CLUB_STATE() = 0;
    virtual void REFRESH_CLUB_NOTICE() = 0;
    virtual void REFRESH_CLUB_ALLIANCE_LIST() = 0;
    virtual void REFRESH_CLUB_BATTLE_LIST() = 0;
    virtual void REFRESH_ITEMBANK() = 0;
    virtual void REFRESH_VNGAINSYSTEM() = 0;
    virtual void REFRESH_ATTENDBOOK() = 0;
    virtual void SET_GATHER_GAUGE(DWORD dwGaeaID, float fTime) = 0;

    //		채팅창 링크 관련	
    virtual void ChatToLink(const SLINK_DATA_BASIC& sLinkDataBasic, const SITEMCUSTOM *pItemCustom = NULL) = 0;	

    virtual SLINK_DATA_INDEX* GetLinkBasicData(int nLinkIndex) = 0;

    virtual bool GetLinkData(const SLINK_DATA_BASIC& sLinkDataBasic, CString& strLinkName, DWORD& dwColor) = 0;		
    virtual DWORD GetLinkColor(int nLinkIndex) = 0;
    virtual bool GetLinkName(CString& strLinkName, int nLinkIndex) = 0;


    //	링크 정보를 등록한다. 
    virtual int AddLinkDataToList_Client( const SLINK_DATA_BASIC& sLinkDataBasic ) = 0;		//	클라이언트 정보로 등록
	virtual int	AddLinkDataToList( const std::string& strCharName, const SLINK_TYPE& sLinkType, void* pBuffer, DWORD dwSize ) = 0;
    virtual int	AddLinkDataToTypeList( const std::string& strCharName, const SLINK_TYPE& sLinkType, void* pBuffer, DWORD dwSize ) = 0;

    //	전송 데이터 변경
    virtual void ConvertRecvData(CString& strText, VEC_LINK_TEXT_DATA& vecLinkTextData) = 0;	
    virtual void ConvertSendData(CString& strInput, const VEC_LINK_TEXT_DATA_EX& vecLinkData, VEC_LINK_DATA_BASIC& vecLinkDataBasic) = 0;

    // 클라이언트 링크 데이터를 서버데이터로 변환 
    virtual void ConvertLinkData(const VEC_LINK_TEXT_DATA_EX& vecLinkDataIN, VEC_LINK_TEXT_DATA& vecLinkDataOut) = 0;

	virtual BOOL GetLinkdataIndex(SLINK_DATA_ITEM& itrLinkData, int nIdx) = 0;
    virtual bool LinkDataProcess(int nLinkIndex) = 0;	
    virtual void SetPartyLinkOnce(int nLinkIndex) = 0;
    virtual void DeleteLinkData(const SLINK_DATA_INDEX& sLinkIndex) = 0;

	virtual void SetPartyOption(const SPartyOption& PartyOption) = 0;	
	virtual void SetPartyInfo(const DWORD dwPartyID, const DWORD dwHashKey, const DWORD dwMasterID, const DWORD dwTimeKey) = 0;

    virtual void SetArmSwap(BOOL bSwap) = 0;

    virtual void AutoArmSwap() = 0;
    virtual void AutoSkillSolt(bool bMain) = 0;
	virtual void SkilSlotTrayToggle () = 0;

    virtual bool SET_QUESTION_ITEM_ID(int nID) = 0;

    virtual bool SET_KEEP_QUESTION_ITEM_ID(int nID) = 0;
    virtual void RESET_KEEP_QUESTION_ITEM() = 0;

    virtual void BONUS_TIME_EVENT_START(bool bCharging) = 0; // 이벤트 시작
    virtual void BONUS_TIME_EVENT_END() = 0; // 이벤트 종료
    virtual void BONUS_TIME_BUSTER_START() = 0; // 경험치 시작
    virtual void BONUS_TIME_BUSTER_END() = 0; // 경험치 종료

    virtual void SetQuestWindowOpen(DWORD dwQuestID) = 0;
    virtual const UIRECT& GetQuestWindowGlobalPos() const = 0;
    virtual const UIRECT& GetQuestWindowRenewalGlobalPos() const = 0;
    virtual void SetQuestWindowGlobalPos(const D3DXVECTOR2& vPos) = 0;
    virtual void SetQuestWindowRenewalGlobalPos(const D3DXVECTOR2& vPos) = 0;
    virtual void SetQuestWindowShowPage(int nPage, bool bMakeMsg = false) = 0;
    virtual void SetQuestWindowAutoSelectPage(DWORD dwQuestID) = 0;

    // NPC Actionable
    virtual void SetDialogueWindowOpen( const SNATIVEID sNPCID, const DWORD dwNPCGlobalID, GLCHARLOGIC* pCHAR ) = 0;
    virtual void SetBusWindowOpen ( const SNATIVEID sNPCID, const DWORD dwNPCGlobalID, std::tr1::shared_ptr<SNpcTalk> spNpcTalk ) = 0;
    virtual void SetStorageWindowNpcOpen ( const SNATIVEID sNPCID, const DWORD dwNPCGlobalID ) = 0;
    virtual void SetStorageWindowCardOpen () = 0;
    virtual void SetStorageWindowClose () = 0;

	virtual void SetStorageItemToVehicleTapWindow(const std::vector<DWORD>& vehicleIDVec) = 0;
	virtual void SetVisibleCharacterAdditionalWindow( bool bVisible ) = 0;

	virtual void SetClubStorageWindowOpen ( const SNATIVEID sNPCID, const DWORD dwNPCGlobalID ) = 0;
    virtual void SetClubStorageWindowClose () = 0;
    virtual void SetMarketWindowOpen ( const SNATIVEID sNPCID, const DWORD dwNPCGlobalID ) = 0;
    virtual void SetMarketWindowClose () = 0;
    virtual void SetItemRebuildWindowNpcOpen( const SNATIVEID sNPCID, const DWORD dwNPCGlobalID ) = 0;
    virtual void SetItemRebuildWindowCardOpen() = 0;
    virtual void SetItemRebuildWindowClose() = 0;
	virtual void SetItemRebuildResetCheckBox() = 0;
	virtual void SetItemRebuildSetCheckBox(const SITEMCUSTOM& sItemCustom, const ITEM_RANDOMOP_UTILL::FixOption& fixOption) = 0;
    virtual void CheckItemRebuildWindowClose(const bool bUseStamp, const ITEM_RANDOMOP_UTILL::FixOption& fixOption) = 0;
    virtual void SetRebuildWindowPreInvenItem( WORD wPosX, WORD wPosY ) = 0;
	virtual void SetRebuildWindowPreStampItem( WORD wPosX, WORD wPosY ) = 0;
    virtual void SetItemMixWindowOpen( const SNATIVEID sNPCID, const DWORD dwNPCGlobalID ) = 0;
    virtual void SetItemMixWindowClose() = 0;    

    virtual void SetTradeWindowOpen(CString szPlayerName, DWORD dwTargetID) = 0;
    virtual void SetTradeWindowClose() = 0;
    virtual void SetStorageChargeOpen(const WORD& wPosX, const WORD& wPosY) = 0;
    virtual void SetItemBankWindowOpen() = 0;
    virtual void SetItemBankInfo() = 0;
    virtual void SetVNGainSysWindowOpen() = 0;
    virtual void SetVNGainSysInfo() = 0;
    virtual void SetDefaultPosInterface(UIGUID ControlID) = 0;
    virtual void SetItemShopSearchWindowOpen() = 0;
    virtual void SetItemShopSearchWindowClose() = 0;
    virtual void SetItemSearchResultWindowOpen() = 0;
    virtual void SetItemSearchResultWindowClose() = 0;


    virtual void OpenItemGarbageWindow() = 0;	// 휴지통
    virtual void CloseItemGarbageWindow() = 0;
    virtual void CheckItemGarbageWindowClose() = 0;

    virtual void SetItemMixResult(CString strMsg, bool bSuccess = false, bool bFail = false) = 0;

    virtual void OPEN_TAXI_WINDOW(WORD wPosX, WORD wPosY) = 0;			// 택시 카드
    virtual void SetPrivateMarketMake() = 0;
    virtual void SetPrivateMarketOpen(const bool& bOPENER, const DWORD& dwGaeaID) = 0;
    virtual void GetPrivateMarketInfo(bool& bOPENER, DWORD& dwGaeaID) = 0;
    virtual void SetPrivateMarketClose() = 0;
    virtual void MODAL_PRIVATE_MARKET_SELLITEM(const bool& bCOUNTABLE, const WORD& wPosX, const WORD& wPosY) = 0;
    virtual void SetClubMake (const DWORD& dwNpcGlobID) = 0;

    virtual void SetTutorialDialogueExplainText(const CString& strText) = 0;
	virtual BOOL IsInventoryOpen() = 0;
	virtual BOOL IsBankOpen() = 0;
	virtual BOOL IsWishListOpen() = 0;
    virtual BOOL IsStorageWindowOpen() = 0;
    virtual BOOL IsClubStorageWindowOpen() = 0;
    virtual BOOL IsMarketWindowOpen() = 0;
	virtual BOOL IsItemDecompose() = 0;

    virtual void ResetCartListInDialogueWindowMarketPage( int nPageIndex, int nPosX, int nPosY ) = 0;
    virtual void DialogueWindowLoadRebuyList() = 0;
    virtual void DialogueWindowCheckRebuySupplies( bool bCheck ) = 0;

    virtual void GameMenuFlipRunButton(BOOL bRun) = 0;
    
    //! Q Box
    virtual bool QBoxButtonGetEnable() = 0;
    virtual void QBoxButtonSetEnable(bool bQBoxEnable) = 0;
    virtual const UIRECT& QBoxButtonGetGlobalPos() const = 0;

    virtual void ModalWindowSetModalData(const D3DXVECTOR3& vData) = 0;
    virtual void ModalWindowSetModalData(int nData1, int nData2) = 0;
    virtual void StatsResetWindowRecalculate() = 0;
    virtual void StatsResetWindowResetProperty(const WORD wReturnStats, const UI::EMSTATSRESETWINDOW_TYPE emType, const EMITEM_TYPE emItemType) = 0;
    virtual void CostumColorChangeWindowSetRestrict(bool bRestrict) = 0;
    
    //! Post
    virtual const BOOL PostBoxWindowIsWindowOpenAble(const BOOL bPrint) = 0;
    virtual BOOL PostBoxWindowIsVisible() const = 0;
    virtual void PostBoxWindowDoModalError(const std::string& strError) = 0;
    virtual void PostBoxWindowDoModalMsg(const std::string& strError) = 0;
    virtual void PostBoxWindowSEndPostFbResult() = 0;
    virtual const BOOL PostBoxWindowIsWindowCloseAble(const BOOL bPrint=FALSE) = 0;
    virtual void PostBoxWindowClose() = 0;
    virtual void PostBoxProcessReceiveComplete() = 0;
    virtual void PostBoxReceivePageUpdateItem() = 0;
    virtual void PostBoxHistoryPageUpdateList() = 0;

    virtual void ItemShopAddShopIcon(DWORD GaeaId) = 0;
    virtual void ItemShopDelShopIcon(DWORD GaeaId) = 0;
    virtual void ItemShopSetRender(DWORD dwGaeaID) = 0;
    virtual void NameDisplaySetCount(DWORD Count) = 0;
    virtual void NameDisplayAddName(CROWREN &sDISP_NAME, const TCHAR * szOwnerName = NULL) = 0;
	virtual void NameDisplayShowCountryMark ( bool bShowCountry ) = 0;

    //! SNS
    virtual void SNSWindowDoModalError(const std::string& strError) = 0;
    virtual void SNSWebFBWindowSetPageType(const DWORD dwType) = 0;
    virtual void SNSWebFBWindowSetAddFriendURL(const std::string& strURL) = 0;
    virtual void SNSFBPageClearEditWall() = 0;
    virtual void SNSFBPageClearEditPicture() = 0;
    virtual void SNSFBPageSetEnableWallButton(const BOOL bEnable) = 0;
    virtual void SNSFBPageSetEnablePicture(const BOOL bEnable) = 0;
    virtual void SNSTWPageClearEditTweet() = 0;
    virtual void SNSTWPageSetEnableTweet(const BOOL bEnable) = 0;
    virtual void SNSTWPageSetEnableFollower(const BOOL bEnable) = 0;
    
    //! ReferChar
    virtual void ReferCharWindowOpen( const std::string& strCharNeme ) = 0;
	virtual std::string	 GetSelectTextName() = 0;

    //! ItemRepair
    virtual void ItemRepairWindowOpen( bool bOpen = true ) = 0;
    virtual void ItemRepairCardUse( WORD wPosX, WORD wPosY ) = 0;

    //! Mouse Tip
    virtual void MouseTipResizeControl(const UIRECT& rcParentOldPos, const UIRECT& rcParentNewPos) = 0;
    virtual const UIRECT& MouseTipGetGlobalPos() const = 0;

    //! Explain
    virtual BOOL ExplainIsSlotBoxVisibleAtInven() const = 0;    
    virtual BOOL ExplainIsSlotBoxVisible(unsigned int Index) const = 0;

    //! Skill Window
    virtual void SkillWindowSetVisiblePageForTutorial() = 0;
    virtual void REFRESH_SKILLWINDOW() = 0;

	//! Skill Window To Tray
	virtual SNATIVEID SkillWindowToTrayGetSkill() = 0;
	virtual void	  SkillWindowToTrayResetSkill() = 0;

    //! TargetInfo
    virtual const CString TargetInfoGetMouseOverName() const = 0;

    //! ItemPreview Window
    virtual void        ItemPreviewWindowOpen           ( const SITEMCUSTOM& sItemCustom ) = 0;
    virtual const BOOL  ItemPreviewWindowIsPreviewItem  ( const SITEMCUSTOM& sItemCustom ) = 0;
    virtual const DWORD ItemPreviewWindowItem_2_Slot    ( const SITEMCUSTOM& sItemCustom ) = 0;

	virtual void SetReviveSkill( bool bAct ) = 0;

    //! StudentRecord
    virtual const bool IsActivityProgressDisplay() = 0;
    virtual void       UpdateActivityWindow() = 0;
    virtual void       UpdateActivityPoint() = 0;
    virtual void       UpdateAttendance() = 0;
    virtual void       UpdateAttendanceTask() = 0;
	virtual void		UnlockAttendanceUI() = 0;
    virtual void       UpdateStudentRecordNotifyButton () = 0;
    virtual void       OpenAttendanceWindow() = 0;

	//! StudentRecord RnAttendance
	virtual void		REFRESH_RnattendancePage() = 0;

    // !Character
    virtual void UpdateActivityBadge() = 0;

    //! ProgressDisplay
    virtual void ProgressDisplayShow ( const UI::EMPROGRESSDISPLAY_TYPE emType, const std::string& strText ) = 0;

    //! Point Shop Window
    virtual void ReloadPointShopItemList() = 0;
    virtual void PointShopSetPoint( __int64 nCurPoint ) = 0;
    virtual __int64 PointShopGetPriceInCart() = 0;
    virtual void PointShopResetCartItem( SNATIVEID sItemID ) = 0;
    virtual void SetForceClosePointShopWindow( bool _bForceClose ) = 0;
    virtual bool IsForceClosePointShopWindow() = 0;

    //! Competition
	virtual void CompetitionNotifyButtonUpdate() = 0;
    virtual void CompetitionNotifyButtonSetLock(const bool _bLock) = 0;
    virtual void CompetitionNotifyButtonSetAlarm(const DWORD _dwMatchingNum, const char* szText) = 0;
    virtual void CompetitionNotifyButtonSetTooltip(const DWORD _dwMatchingNum, const char* szText) = 0;
	virtual void CompetitionCTFUpdate() = 0;

    //! Rebirth
    virtual void SetForceResurrect( const float _fTime ) = 0;

    //! CTF
    virtual void InfoCTFDisplayUpdate() = 0;
	virtual void CTFResultWindowShow() = 0;
	virtual void CTFResultWindowUpdate() = 0;
    virtual void CheckCTFSpurt() = 0;

	//! Product
	virtual void MsgProductStart( DWORD nID, WORD nCount ) = 0;
	virtual void MsgProductGoAlong() = 0;
	virtual void MsgProductComplete() = 0;
	virtual void MsgProductNeedConfirm() = 0;
	virtual void MsgProductCancel() = 0;

    //! NotifyWindow
    virtual void NotifyWindowSetNewAlarm() = 0;

	//! WishList
	virtual void MsgWishListInsert( BYTE nType ) = 0;

    virtual void MsgMapSearchNameList(const std::vector<std::pair<int, std::string> >& nameList) = 0;

	virtual void MsgMapSearchNameDetailInfo(int level,const std::string& charName,const std::string& title,int school,int emClass,
		bool isParty,const std::string& clubName,DWORD dwGaeaID) = 0;

	virtual void MsgMapSearchNamePosition(int level,const std::string& charName,DWORD mapID,const D3DXVECTOR3& pos) = 0;

	virtual void MsgMapPartyInfoListBoard(bool isExistMyBoard,DWORD mapID,
		const std::vector<std::string>& NameVec,const std::vector<DWORD>& MaxPlayerVec,
		const std::vector<DWORD>& CurrentPlayerVec,const std::vector<std::string>& TitleVec) = 0;

    //! ClubWindow
    virtual void RefreshClubWindowGuidanceMap() = 0;
    virtual void ShowClubLogInClubWindow() = 0;
    virtual void ClubStorageUpdateMoneyInClubWindow() = 0;
    virtual void ClubUpdateNewbieNotice() = 0;


	virtual void MsgMapPartyInfoDetail(DWORD mapID,const std::vector<GLPARTY_LINK_MEM>& memberVec) = 0;

	//! 개인상점 검색 시스템 ( PriviteMarket )
    virtual void PrivateMarketWindowOpen( bool bOpen = true ) = 0;
	virtual void PrivatemarketRefreshSearchResult() = 0; //개인상점 검색 페이지 리셋
	virtual void SetPageWaiting(bool bIsWaiting) = 0;
	virtual bool IsPageWaiting() = 0;
    virtual void PrivattMarketWindowRanPointPageRefresh() = 0;


    //! PVE
    virtual void UpdatePVEMapTable() = 0;
    virtual void SetPVEEntranceConfirm(bool bShow) = 0;
    virtual void SetPVEEntranceState(int nCur, int nAll, int nSec, bool bShow) = 0;
    virtual void SetPVEStage(int nStage, bool bShow) = 0;
    virtual void SetPVEMessage(const TCHAR* pText, bool bShow) = 0;
    virtual void SetPVEMonsterIndicator(int nCur, int nAll, bool bShow) = 0;
    virtual void SetPVEPositioningIndicator(int nX, int nY, int nNow, int nNeed,  bool bShow) = 0;
	virtual void SetPVETimeIndicator(int nSec, bool bShow ) = 0;
    virtual void SetPVEStageIndicator(int nStage, int nEnd, int nRange, int nSec, bool bBlank, bool bShow) = 0;
    virtual void SetPVEStageState(int nState, bool bShow) = 0;
    virtual void SetPVERetry(LONGLONG nMoney, int nTime, bool bShow) = 0;
    virtual void SetPVEStageResult() = 0;

	virtual void SetNewInstanceRetry ( LONGLONG nMoney, int nTime, bool bShow, InstanceSystem::InstanceMapID nInstanceID, SNATIVEID sKeyMapID ) = 0;

	//! 파티 아이템분배 알림
	virtual void OpenPartyTenderNotifyButton( bool bShow ) = 0;
	virtual void PartyTenderNotify( bool bIsNew ) = 0;

    // New Instance System -----------------------------------------------------------
    // Common UI
    virtual void PrintSimpleSystemMessage( const CString& strMessage,
        const D3DCOLOR dwMessageColor, const DWORD dwFontSize, const float fLifeTime ) = 0;
    virtual void ClearSimpleSystemMessage() = 0;

    virtual void ShowSimpleTimerMessageBox(const NET_MSG_GENERIC* pMsg) = 0;
    virtual void CloseSimpleTimerMessageBox() = 0;

	//! Rn CDM
	virtual void RefreashCdmProgress( DWORD	dwNumberOneClubMarkVER,
									  DWORD	dwNumberOneClubDbNum,
									  char*	szNumberOneName,
									  int	nNumberOneScore,
									  int	nMyRank,
									  DWORD	dwMyScore,
									  int	nHour,
									  int	nMin,
									  int	nSecond ) = 0;

	virtual void SetRankSloatData(InstanceSystem::RNCDM_RANK_SLOT vecRnCdmRankSlot) = 0;
    virtual InstanceSystem::RNCDM_RANK_SLOT GetRankSlotData(int nIndex) = 0;
	virtual void SetTitleClubNameMemberNum( const CString strClubName, int nMembernum ) = 0;
	virtual void RankListUpdate() = 0;
	virtual void SetRnCDMClubInfoPos(int nPosX, int nPosY) = 0;

	//Rn CDM CompetitionWindow
	virtual void SetCDMMapinfo(std::string	strMapName,
		int nTimeLimit, 
		int nPlayerLimite, 
		int nLevelLimit ) = 0;
	virtual void SetCDMMapImageName(std::string strMapImageName) = 0;
	virtual void SetCDMBattelState(int nStartTime, int nEndTime) = 0;
	virtual void OpenCDMTopRanking() = 0;
	virtual void SetCDMTopList(std::vector<DWORD> vecClubId,
								LONGLONG lnStartTime,
								LONGLONG lnEndTime,
								std::vector<DWORD> vecRank ) = 0;
	virtual void SetClubMasterClubName(	std::vector<std::string> vecMasterName,
										std::vector<std::string> vecClubName ) = 0;
	virtual void ShowDoublePoint(BOOL bVisible) = 0;

    // CaptureTheFlagEffect
    virtual void SetFlagGuideArrow( const int _flagNum, const STARGETID& sTargetID, const bool bBlink ) = 0;

    // RnCompetitiom ResultWindow
    virtual void ShowRnCompetitionRewardTooltop(const int emTitleType, const InstanceSystem::RnCompetitionResultContentRewardObj* pReward) = 0;
    virtual const bool msgProc_RnCompetitionResult(const NET_MSG_GENERIC* _pMessage) = 0;

    // Guidance Club Battle
    virtual void SetGuidanceClubBattle_UI_Auth(const bool bVisible, const float fAuthTime) = 0;
    virtual void SetGuidanceClubBattle_ProgressInfo(const NET_MSG_GENERIC* _pMessage) = 0;
    virtual void SetGuidanceClubBattle_RankInfo(const NET_MSG_GENERIC* _pMessage) = 0;
    virtual void SetGuidanceClubBattle_RankMyInfo(const NET_MSG_GENERIC* _pMessage) = 0;
    virtual void SetGuidanceClubBattle_AuthInfo(const NET_MSG_GENERIC* _pMessage) = 0;

	// CTI
	virtual void SetCTI_UI_Auth ( const bool bVisible, const float fAuthTime ) = 0;
	virtual void SetCTI_ProgressInfo ( const NET_MSG_GENERIC* pMsg ) = 0;
	virtual void SetCTI_RankInfo ( const NET_MSG_GENERIC* pMsg ) = 0;
	virtual void SetCTI_RankMyInfo ( const NET_MSG_GENERIC* pMsg ) = 0;
	virtual void SetCTI_AuthInfo ( const NET_MSG_GENERIC* pMsg ) = 0;

    //-------------------------------------------------------------------------------

	virtual void OpenTournamentDashboard(DWORD dwPageNum)=0;
	virtual void OpenTournamentBettingWindow(DWORD dwKagNum,DWORD dwPageNum)=0;

	virtual void OpenTournamentMoreInfoWindow(DWORD dwPageNum) = 0;
	virtual void OpenTournamentUI_HUD(bool isObserver) = 0;
	virtual void OpenTournamentWinLose(bool isWin) = 0;
	virtual void OpenTournamentJoinWindow(bool _Open, int Time )= 0;
	virtual void OpenTournamentJoinWindow(bool _Open)= 0;
	virtual void SetStageTimeIndicator( const int nSec, const bool bBlank, const bool bShow ) = 0;
	virtual int  SetInstancePopUp( MatchingSystem::InstancePopUpReceipt* Receipt ) = 0;
	virtual void SetBigGroupWindowRight(DWORD _factionid) = 0;
	virtual void SetBigGroupWindowLeft(DWORD _factionid) = 0;
	virtual void SetMiniGroupWindowRight(DWORD _factionid) = 0;
	virtual void SetMiniGroupWindowLeft(DWORD _factionid) = 0;
	virtual void HideAllTournamentWindow() = 0;
	virtual int  GetIndicatorTime() = 0;
	virtual int  SetIndicatorTime(int nSec, bool bBlank, bool bShow) = 0;
	virtual void SetTournamentIndicatorType( DWORD ObserverType , DWORD factionid1, DWORD factionid2) = 0;
    virtual void SetTournamentIndicatorInfo( const NET_MSG_GENERIC* const _pMessage ) = 0;

	virtual void OpenCostumeStatWindow( const SINVENITEM* itemSrc = NULL, const SINVENITEM* itemDest = NULL ) = 0;
	virtual void OpenSelectiveformBox( std::vector<ITEM::SBOX_ITEM>& vecItemID ) = 0;
	virtual void ToggleJoinWorldBattleButton(bool bOn) = 0;
};

#endif // _INTERFACE_BASE_H_

