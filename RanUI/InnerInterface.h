#pragma	once

#include <boost/function/function_fwd.hpp>

#include "../SigmaCore/Memory/CMemPool.h"

#include "../EngineLib/GUInterface/UIManCommon.h"
#include "../EngineLib/GUInterface/BasicTextBox.h"
#include "../EngineLib/GUInterface/UITextControl.h"

#include "../RanLogic/GLChatLinkDef.h"
#include "../RanLogic/GLCompetition.h"

#include "../RanLogicClient/WishList/WishList.h"
#include "../RanLogicClient/InterfaceBase.h"

#include "./Factory/Factory.h"
#include "./OldUI/Util/PopupDefine.h"
#include "./OldUI/Display/ItemInfoTooltipGroup.h"
#include "./OldUI/partyrenewal/PartyYesNoWindowDefine.h"
#include "InnerInterfaceGuid.h"

class PrivateMarketSearchBuy;

class CChatGroupWindow;
class CMaxChatWindow;
class CMinChatWindow;

class	CAcademyConftDisplay;	
class	CAdminMessageDisplay;	
class	CBasicButton;
class	CBasicGameMenu;	
class   CQBoxButton;
class	CBasicPotionTray;
class	CBasicSkillTray;	
class	CBasicVarTextBox;
class	CBusWindow;	
//class	CCharacterWindow;
class   RnCharacterWindowInterface;
class   CReferCharWindow;
class   ItemRepairWindow;
class   RnSkillWindow;
class	IInventory;
class	CClubMake;	
class	CClubStorageWindow;	
class	CClubWindow;	
class	CConftConfirm;
class	CCountMsg;	
class	CDamageDisplayMan;	
class	CDialogueWindow;
class	CGambleAgainBox;	
class	CGambleAnimationBox;	
class   CPetSkinMixImage;
class	CGambleBox;	
class	CGambleResultEvenBox;	
class	CGambleResultOddBox;	
class	CGambleSelectBox;
class	CItemBankWindow;	
class	CVNGainSysInventory;
class   CVNGainSysGauge;
class	CItemRebuild;	// ITEMREBUILD_MARK

//class	CLargeMapWindow;
class   RnMapWindowInterface;

class	CMapMoveDisplay;
class CMultiMapMoveDisplay;
class	CMiniPartyWindow;
class	CPetRebirthDialogue;// Monster7j	
class	CPetWindow;	
class	CPrivateMarketMake;	
class	CPrivateMarketSellWindow;
class	CPrivateMarketWindow;
class	CQuestAlarm;	
class	CQuestionItemDisplay;
class	CRebirthDialogue;	
class	CReceiveNoteWindow;	
class	CSimpleHP;	
class	CSimpleMessageMan;
class	CSMSSendWindow;	
class	CStorageChargeCard;	
class	CStorageWindow;	
class	CSubMasterSet;	
class	CSystemMessageWindow;	
class	CBikeMessageWindow;
class	CWaitServerDialogue;	
class	CWriteNoteWindow;	
class   CNotifyWindow;
class   CNotifyButton;
struct	GLCHARLOGIC;	
class	GLClub;	
class	GLMapAxisInfo;	
struct	GLSKILL;	
struct	SCHARSKILL;	
struct	SITEMCUSTOM;	
struct	SNpcTalk;	
class	CBonusTimeGauge;
class	CBonusTimeDisplay;
class	CThaiCCafeMark;
class	CItemGarbage;
class	CGarbageInventoryWindow;
class	CItemShopIconMan;
class	CSummonPosionDisplay;
class	CAttendanceBookWindow;
class	CClubBattleModal;
class	CTaxiWindow;
class	CItemMixWindow;
class	CItemMixInvenWindow;
class	CGatherGauge;
class	CCdmRankingDisplay;
class	CCDMSafeTimeDisplay;
class	CSkillTimeDisplay;
class	CInfoDisplayLink;
class	CPartyLinkWindow;
class	CBikeWindow;
class	CBikeColorChangeWindow;
class	CBikeGetOffWindow;
class	CStatsResetWindow;
class	CBikeBoostWindow;
class	CCostumColorChangeWindow;
class	CPetHungry;
class	CSNSWindow;
class	CFaceBookWindow;
class	CTwitterWindow;
class   CItemPreviewWindow;
class   CMouseTip;
class   CTutorialDialogueExplainText;
class   CTutorialInterfaceLeft;
class   CTutorialInterfaceRight;
class   CTutorialInterfaceUp;
class   CTutorialInterfaceDown;
class   CTutorialInterfaceZoomIn;
class   CTutorialInterfaceZoomOut;
class   CMinimapTarget;
class   CTutorialSkipButton;
class   CTutorialDamageAlarm;
class   CUIColorSelectorWindow;
class   CStudentRecordNotifyButton;
class   CPointShopWindow;
class   CRebirthWindow;
class   CItemShopWindowWeb;
class   CItemInfoTooltip;
class   CSkillInfoTooltip;
class	CSkillLinkInfoTooltip;
class   CForceRebirth;
class   CNotifyCTFSpurt;
class   CDialogueWindowRenewal;
class   GLCharClient;
struct  SFRIEND;
struct  SProductRecipe;
class	CProductResetWindow;
class ItemRebuildRenewal;
class ColorProperty;
class CCarWindow;
class CRearcarWindow;
class CTournamentEntranceConfirm;
class TournamentObserveWindow;
class CTournamentMulty;
class TournamentMiniExpeditionWindow;
class CTournamentExpeditionInfoGroup;
class CTournamentIndicator;
class DoublePoint;
class CCostumeStatWindow;
class SelectiveformBoxWindow;

namespace InstanceSystem
{
	struct RNCDM_RANK_SLOT;
	struct CDM_MEMBER_INFO;
    struct RnCompetitionResultContentRewardObj;
}
// Lotto System;
class ILottoMainUI;
class ILottoBuyLottoUI;
class ILottoBuyListInfo;
class ILottoWinManListInfo;
class ILottoDisplayUI;
class ILottoNotifyButton;

class ISummonStateDisplayUI;

// World Battle System;
class IWorldBattleButton;

// UI Interface class.


class IMiniMap
{
public:
    virtual ~IMiniMap() {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;

    virtual void SetMapAxisInfo(const GLMapAxisInfo &, const UI::String& ) = 0;

    virtual void StartClubGuidBattleTime() = 0;
    virtual void SetClubGuidBattleName( const std::vector<std::string> &vecName ) = 0;
    virtual void UpdateClubGuidBattleTime( const std::vector<float> &vecTime ) = 0;

    virtual void StartClubDeathMatchTime() = 0;
    virtual void UpdateClubDeathMatchTime( const float fTime ) = 0;

    virtual void EndClubBattleTime() = 0;   //선도전과 데스메치 함수 공유

    virtual const UIRECT& GetUIWindowGlobalPos() = 0;
    virtual const UIRECT& GetMinimapBackGlobalPos() = 0;
};

class IDPSWindow
{
public:
	virtual ~IDPSWindow(void)	{};

public:
	virtual void CreateUIWindowAndRegisterOwnership(void) = 0;
};

class IKeySettingWindow
{
public:
    virtual ~IKeySettingWindow() {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;

    virtual UI::String GetdwKeyToString( int dwKey ) = 0;
    virtual void GetShotCutKey() = 0;
};

class IChatMacroWindow
{
public:
    virtual ~IChatMacroWindow() {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;

    virtual void GetChatMacro() = 0;
};

class INameDisplayMan
{
public:
    virtual ~INameDisplayMan()  {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;
    
    virtual void VisibleTargetInfoCtrl( EMCROW emCROW, DWORD dwCtrID, BOOL bVISIBLE ) = 0;
    virtual void ClearList() = 0;
    virtual void SET_COUNT( DWORD _dwCOUNT ) = 0;
    virtual void ADD_DISP_NAME( CROWREN &sDISP_NAME, const TCHAR * szOwnerName = NULL ) = 0;
    virtual int GetNameType() = 0;
    virtual void SetNameType( int nType ) = 0;
    virtual const CROWREN* GetDisplayInfo( EMCROW emCROW, DWORD dwCtrID ) = 0;
    virtual void OmitDisplayName( const EMCROW emCrow, const DWORD dwCrowID ) = 0;

	virtual void SetShowCountryMark ( bool bShowCountry ) = 0;
};

class IPrivateMarketShowMan
{
public:
    virtual ~IPrivateMarketShowMan()    {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;
    virtual void ADD_MARKET( const CROWREN &sDISP ) = 0;
    virtual void DEL_MARKET( DWORD dwGaeaID ) = 0;
    virtual void UPDATE_MARKET( const CROWREN &sDISP ) = 0;
    virtual void RemoveAll() = 0;
};

class ISummonNameDisplayMan
{
public:
    virtual ~ISummonNameDisplayMan()    {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;

    virtual void ADD_DISP( DWORD dwGlobID ) = 0;
    virtual void SET_COUNT( DWORD _dwCOUNT ) = 0;
};

class ISkillWindowToTray
{
public:
    virtual ~ISkillWindowToTray()   {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;

    virtual void SetSkill( SNATIVEID sNativeID ) = 0;
    virtual SNATIVEID GetSkill() = 0;
    virtual void ResetSkill() = 0;
    virtual void SetUIWindowGlobalPos( const UIRECT& rcPos ) = 0;
    virtual void SetUseSnap( BOOL bUseSnap ) = 0;
};

class IPartyWindow
{
public:
    virtual ~IPartyWindow() {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;

	virtual const DWORD GetPartyID(void) const = 0;
	virtual const DWORD GetHashKey(void) const = 0;
	virtual const DWORD GetMasterID(void) const = 0;
	virtual const DWORD GetTimeKey(void) const = 0;
	virtual void SetPartyInfo(const DWORD dwPartyID, const DWORD dwHashKey, const DWORD dwMasterID, const DWORD dwTimeKey) = 0;    
};

class IPartyWindowRenewal
{
public:
	virtual void SetPartyOption(const SPartyOption& PartyOption) = 0;
	virtual ~IPartyWindowRenewal() {};

public:
	virtual void CreateUIWindowAndRegisterOwnership() = 0;	
};

class IPartyYesNoWindow
{
public:	
	virtual ~IPartyYesNoWindow() {};

public:
	virtual const DWORD AddComponent(const std::string& strDescription, PARTY_YESNO_FUNCTION PartyYesNoFunction) = 0;
	virtual void SetID(const DWORD nID) = 0;
	virtual void CreateUIWindowAndRegisterOwnership() = 0;	
};

class IExpeditionInviteWindow
{
public:
	virtual ~IExpeditionInviteWindow() {};

public:
	virtual void CreateUIWindowAndRegisterOwnership() = 0;	
};

class IExpeditionCheckReadyWindow
{
public:
	virtual ~IExpeditionCheckReadyWindow() {};

public:
	virtual void CreateUIWindowAndRegisterOwnership() = 0;	
};

class IPartyStateWindow
{
public:
	virtual void SetVisibleSkillEffect(const BOOL bShow) = 0;
	virtual ~IPartyStateWindow() {};

public:
	virtual void CreateUIWindowAndRegisterOwnership() = 0;
};

class IPartyTenderWindow
{
public:
	virtual ~IPartyTenderWindow(){};

public:	
	virtual void CreateUIWindowAndRegisterOwnership() = 0;	
};

class IPartyTenderNotifyButton
{
public:
	virtual ~IPartyTenderNotifyButton() {};

public:
	virtual void CreateUIWindowAndRegisterOwnership() = 0;
	virtual	void Notify( bool bNotify ) = 0;
};

class IPartyDistributionWindow
{
public:
	virtual ~IPartyDistributionWindow(){};

public:	
	virtual void CreateUIWindowAndRegisterOwnership() = 0;	
};

class IPartyDistributionListWindow
{
public:
	virtual ~IPartyDistributionListWindow(){};

public:	
	virtual void CreateUIWindowAndRegisterOwnership() = 0;	
};

class IMiniExpeditionWindow
{
public:
	virtual ~IMiniExpeditionWindow() {};

public:
	virtual void CreateUIWindowAndRegisterOwnership() = 0;
};

class IBasicChatWindow
{
public:
    virtual ~IBasicChatWindow()   {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;

    virtual void AddChatMacro( UI::String strMsg ) = 0;
    virtual void AddStringToSystemMessage( UI::String strTemp, D3DCOLOR dwColor ) = 0;
    virtual void AddStringToChatLink( UI::String strTemp, WORD wType, VEC_LINK_TEXT_DATA& vecLinkTextData, bool bGM = false ) = 0;
    virtual void AddStringToChatEx( UI::String strTemp, WORD wType, bool bGM = false ) = 0;
	virtual void AddStringToNomal( UI::String strTemp, D3DCOLOR dwColor ) = 0;
    virtual void RefreshFriendList() = 0;
    virtual void ChatLog( bool bChatLog, int nChatLogType ) = 0;
    virtual UI::String GetRecordChat() = 0;
    virtual void SetFriendOnlineList( CONST UI::String& strName, bool bOnline ) = 0;
    virtual bool IsChatBegin() = 0;
    virtual void SetChatOption(WORD ChatFlag) = 0;
    virtual void SetChatType(DWORD ChatType) = 0;
    virtual void ChatToLink( const SLINK_DATA_BASIC& sLinkDataBasic, const UI::String strLinkName, DWORD dwColor ) = 0;
    virtual void BeginPrivateChat( const UI::String& strName = "", const UI::String& strMessage = "" ) = 0;
    virtual const UIRECT& GetUIWindowGlobalPos() = 0;
    virtual const UIRECT& GetUIChangStateButtonGlobalPos() = 0;
    virtual void SetPrivateMsgCharName( const UI::String& strName ) = 0;
    virtual void SetNoUpdate( BOOL bNoUpdate ) = 0;
    virtual void SendReplyAwayState( const std::string& strPrivateName, const std::string& strMsg  ) = 0;
};

class IChatOptionWindow
{
public:
    virtual ~IChatOptionWindow() {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;
};

class ITradeWindow
{
public:
    virtual ~ITradeWindow() {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;

    virtual void SetTradeInfo( UI::String strPlayerName, DWORD dwMasterID ) = 0;
    virtual const UIRECT& GetUIWindowGlobalPos() = 0;
    virtual void SetUIWindowGlobalPos( const D3DXVECTOR2& vPos ) = 0;
    virtual DWORD GetMasterID() = 0;
};

class ITradeInventoryWindow
{
public:
    virtual ~ITradeInventoryWindow()    {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;

    virtual const UIRECT& GetUIWindowGlobalPos() = 0;
    virtual void SetUIWindowGlobalPos( const D3DXVECTOR2& vPos ) = 0;
    virtual void SetOneViewSlot( const int& nONE_VIEW_SLOT ) = 0;
    virtual void SetOpener( bool bOPENER ) = 0;
};

class IPtoPWindow
{
public:
    virtual ~IPtoPWindow()    {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;

    virtual const UIRECT& GetUIWindowGlobalPos() = 0;
    virtual void SetUIWindowGlobalPos( const D3DXVECTOR2& vPos ) = 0;
    virtual void SetPtoPMenu(std::tr1::shared_ptr<GLCharClient> pCharClient, DWORD dwGaeaID, GLClubClient* pMyClub) = 0;
    virtual DWORD GetPlayerID() = 0;
	virtual void SetPlayerID(DWORD dwGaeaID) = 0;

    virtual void showGroupChat() = 0;
};

class IInventoryWindow
{
public:
    virtual ~IInventoryWindow() {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;

    virtual void SetOneViewSlot( const int& nONE_VIEW_SLOT ) = 0;
    virtual const int& GetOneViewSlot() const = 0;
    virtual void SetArmSwapTabButton( BOOL bArmSub ) = 0;
    virtual const UIRECT& GetUIWindowGlobalPos() = 0;
    virtual const UIRECT& GetInventoryPageGlobalPos() = 0;
    virtual void LockTrade() = 0;
    virtual void UnLockTrade() = 0;
    virtual const bool IsLocked() = 0;
    virtual void GetSplitPos( WORD* pwPosX, WORD* pwPosY ) = 0;
};

class IItemMove
{
public:
    virtual ~IItemMove()    {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;

    virtual const UIRECT& GetUIWindowGlobalPos() = 0;
    virtual void SetUIWindowGlobalPos( const UIRECT& rcPos ) = 0;
    virtual SNATIVEID GetItem() = 0;
};

class IMarketWindow
{
public:
    virtual ~IMarketWindow()    {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;

    virtual void InitMarket( SNATIVEID sNPCID, DWORD dwNpcGlobID ) = 0;
    virtual void GetItemIndex( int* pnPosX, int* pnPosY ) = 0;
    virtual SNATIVEID GetNPCID() = 0;
    virtual DWORD GetNPCGlobalID() = 0;
    virtual const SINVENITEM* GetItem( int nPosX, int nPosY ) const = 0;
    virtual SNATIVEID GetMarketID() = 0;
    virtual int GetPageIndex() = 0;
};

class IHeadChatDisplayMan
{
public:
    virtual ~IHeadChatDisplayMan()  {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;

    virtual void AddChatMsg( const UI::String& strName, const D3DCOLOR& dwIDColor,
        const UI::String& strChat, const D3DCOLOR& dwChatColor, const D3DXVECTOR2& vPos ) = 0;
    virtual void CheckOmitDisplayName() = 0;
};

class IConftModalWindow
{
public:
    virtual ~IConftModalWindow()    {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;

    virtual void DoModal( UI::String strText, UIGUID CallerID, UI::String strMyName, UI::String strHisName ) = 0;
    virtual UIGUID GetCallerID() = 0;
};

class IPartyModalWindow
{
public:
    virtual ~IPartyModalWindow()    {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;

    virtual void DoModal( UI::String strText, UIGUID CallerID ) = 0;
	virtual void SetPartyMode( bool bExpedition ) = 0;
	virtual void SetPartyOption(const SPartyOption& PartyOption) = 0;
    virtual UIGUID GetCallerID() = 0;
    virtual void SetCharDbNum( DWORD dwCharDbNum ) = 0;
    virtual DWORD GetCharDbNum() = 0;
};

class IChatGroupModalWindow
{
public:
	virtual ~IChatGroupModalWindow()    {};

public:
	virtual void CreateUIWindowAndRegisterOwnership() = 0;

	virtual void DoModal( UI::String strText, UIGUID CallerID ) = 0;
	virtual void SetPartyMode( bool bExpedition ) = 0;
	virtual void Setdwkey( DWORD dwkey ) = 0;
	virtual DWORD Getdwkey() = 0;
	virtual UIGUID	GetCallerID() = 0;
};

class IConftDisplayMan
{
public:
    virtual ~IConftDisplayMan() {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;

    virtual void SetFightBegin( const int nCount ) = 0;
    virtual void SetFightEnd( const int nResult ) = 0;
};

class IQuestWindow
{
public:
    virtual ~IQuestWindow() {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;

    virtual UIGUID GetWndID() = 0;
    virtual void RefreshQuestWindow() = 0;
    virtual int	 GetShowPage() = 0;
    virtual DWORD GetQuestID() = 0;
    virtual void SetShowPage( int nPage, bool bMakeMsg = false ) = 0;
    virtual void AutoSelectPage( DWORD dwQuestID ) = 0;
    virtual const UIRECT& GetUIWindowGlobalPos() = 0;
    virtual void SetUIWindowGlobalPos( const D3DXVECTOR2& vPos ) = 0;
    virtual const UIRECT& GetProgListTextBoxGlobalPos() = 0;
    virtual const UIRECT& GetProgListTextBoxLocalPos() = 0;
    virtual const UIRECT& GetProgMainTextBoxGlobalPos() = 0;
    virtual const UIRECT& GetProgMainTextBoxLocalPos() = 0;
    virtual const UIRECT& GetProgMainMoreBtnGlobalPos() = 0;
};

class IQuestWindowRenewal
{
public:
    virtual ~IQuestWindowRenewal()  {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;

    virtual void AutoLoadQuest( DWORD dwQuestID ) = 0;
    virtual UIGUID GetWndID() = 0;
    virtual void RefreshQuestWindow() = 0;
    virtual void SetShowPage() = 0;
    virtual void AutoSelectPage( DWORD dwQuestID ) = 0;
    virtual const UIRECT& GetUIWindowGlobalPos() = 0;
    virtual void SetUIWindowGlobalPos( const D3DXVECTOR2& vPos ) = 0;
    virtual const UIRECT& GetUIWindowLocalPos() = 0;
    virtual void SetUIWindowLocalPos( const D3DXVECTOR2& vPos ) = 0;
    virtual BOOL IsVisible() = 0;
    virtual const UIRECT& GetMainStepBackPos() = 0;
    virtual int GetPage() = 0;
    virtual void ShowHelperCount() = 0;
    virtual bool CheckListAndReturnResult( DWORD dwID, bool bCurStatus ) = 0;
    virtual void SetQuestListID( DWORD dwID ) = 0;
    virtual bool OpenListAndReturnResult( DWORD dwID, bool bCurStatus ) = 0;
    virtual const UIRECT& GetListPageGlobalPos() = 0;
    virtual DWORD GetQuestMainID() = 0;
    virtual void LoadQuestMain( DWORD dwID ) = 0;
    virtual void LoadQuestMainEnd( DWORD dwID ) = 0;
};

class IFriendWindow
{
public:
    virtual ~IFriendWindow()    {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;

    virtual void AddFriendName_To_EditBox( const UI::String& strName ) = 0;
    virtual void AddFriend( const UI::String& strName ) = 0;
    virtual void LoadFriendList() = 0;
    virtual UI::String GetFriendName() const = 0;
    virtual UI::String GetSelectBlockFriendName() const = 0;
    virtual void EndEditFriendList() = 0;
    virtual std::tr1::shared_ptr<SFRIEND> GetFriendSMSInfo() = 0;

    //리뉴얼UI 추가
    virtual void AddFriendBlock( const std::string& strName, bool bBlock ) = 0;
};

class IChatGroupInviteWindow
{
public:
	virtual ~IChatGroupInviteWindow()    {};

public:
	virtual void CreateUIWindowAndRegisterOwnership() = 0;

	virtual void AddFriendName_To_EditBox( const UI::String& strName ) = 0;
	virtual void AddFriend( const UI::String& strName ) = 0;
	virtual void LoadFriendList() = 0;
	virtual UI::String GetFriendName() const = 0;
	virtual UI::String GetSelectBlockFriendName() const = 0;
	virtual void EndEditFriendList() = 0;
	virtual std::tr1::shared_ptr<SFRIEND> GetFriendSMSInfo() = 0;
};

class IModalWindowInner
{
public:
    virtual ~IModalWindowInner() {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;

    virtual void SetHide( BOOL bHide ) = 0;
    virtual void DoModal( const std::string& strText, int nModalTitle, int nModalType,
        UIGUID CallerID ) = 0;
    virtual void DoModalTipBox ( const std::string& strText, const std::string& strTip,
        int nModalTitle, int nModalType, UIGUID CallerID ) = 0;
    virtual void SetModalData ( int nData1, int nData2 ) = 0;
    virtual void SetModalData( const D3DXVECTOR3& vData ) = 0;
    virtual void GetModalData( int* pnData1, int* pnData2 ) = 0;
    virtual void GetModalData( D3DXVECTOR3* vData ) = 0;
    virtual bool IsUseModaless() = 0;
    virtual UI::String	GetEditString() = 0;
    virtual UIGUID GetCallerID() = 0;
};

class IShopItemSearchWindow
{
public:
    virtual ~IShopItemSearchWindow()    {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;

    virtual SSEARCHITEMDATA GetSearchData() = 0;
};

class IItemSearchResultWindow
{
public:
    virtual ~IItemSearchResultWindow()  {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;

    virtual void UpdateSearchResult(SSEARCHITEMESULT* pSearchItemResult, DWORD dwUpdateNum, DWORD dwPageNum) = 0;
};

class IMapRequireCheck
{
public:
    virtual ~IMapRequireCheck() {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;

    virtual void SetMapCheckType( UI::EMMAPCHECK_TYPE emMapCheckType ) = 0;
};

class IPostBoxWindow
{
public:
    virtual ~IPostBoxWindow()   {};

public:
    virtual void CreateUIWindowAndRegisterOwnership( LPDIRECT3DDEVICEQ pd3dDevice ) = 0;

    virtual void Close() = 0;
    virtual void DoModalError( const std::string& strError) = 0;
    virtual void DoModalMsg( const std::string& strMsg) = 0;
    virtual BOOL IsVisible() = 0;
    virtual const BOOL IsWindowOpenAble( const BOOL bPrint=FALSE ) = 0;
    virtual const BOOL IsWindowCloseAble( const BOOL bPrint=FALSE ) = 0;
    virtual void Process_ReceiveComplete() = 0;
    virtual const BOOL IsSendPageOpen() = 0;
    virtual void PostBoxHistoryPageUpdateList() = 0;
    virtual void RequestPushViewPage( const DWORD dwRequest ) = 0;
    virtual void ReqMultiDelete_Check() = 0;
    virtual void ReqMultiDelete_Commit() = 0;
    virtual void PostBoxReceivePageUpdateItem() = 0;
    virtual void ReqSendPost() = 0;
    virtual void ResetSendPage() = 0;
    virtual bool IsSendPageEnalbe() = 0;
    virtual void Set_TO_Editbox( const UI::String& strText_To ) = 0;
    virtual void ReqPostDelete( bool bConfirmAttach = true ) = 0;
    virtual void ReqPostReturn() = 0;
    virtual void ReqPostTakeItem() = 0;
};

class IPostNotifyButton
{
public:
    virtual ~IPostNotifyButton()    {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;
};

class IQuestHelper
{
public:
    virtual ~IQuestHelper() {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;

    virtual bool SetQuestID( DWORD dwQuestID, bool bPrintMsg = TRUE ) = 0;
    virtual void ReSetQuestID( DWORD dwQuestID ) = 0;
    virtual bool IsSameQuest( DWORD dwQuestID ) = 0;
    virtual int GetHelperCount() = 0;
    virtual int GetHelperMaxCount() = 0;
    virtual void UpdateQuestInfo() = 0;
};

class ITutorialDialogue
{
public:
    virtual ~ITutorialDialogue()    {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;

    virtual void SetDialogueText( const UI::String& strText ) = 0;
    virtual void SetEnalbe( bool bEnableWork ) = 0;
    virtual const UIRECT& GetUIWindowGlobalPos() = 0;
    virtual void SetUIWindowGlobalPos( const D3DXVECTOR2& vPos ) = 0;
    virtual void SetUIWindowLocalPos( const D3DXVECTOR2& vPos ) = 0;
    virtual void SetTutorialDialogueIndex( DWORD Index ) = 0;
    virtual DWORD GetTutorialDialogueIndex() = 0;
    virtual void TutorialDialogueTextStepInc() = 0;
    virtual void SetTutorialDialogueSoundIndex( DWORD Index ) = 0;
    virtual DWORD GetTutorialDialogueSoundIndex() = 0;
    virtual void TutorialDialogueSoundIndexInc() = 0;
};

class IBalloonText
{
public:
    virtual ~IBalloonText() {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;

    virtual void SetUIWindowGlobalPos( const UIRECT& rcPos ) = 0;
    virtual void SetUIWindowLocalPos( const UIRECT& rcPos ) = 0;
};

class ISkillWindow
{
public:
    virtual ~ISkillWindow() {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;
    
    virtual const UIRECT& GetUIWindowGlobalPos() = 0;

public:
    // 예전 스킬창
    virtual void ShowPageForTutorial() = 0;

public:
    // 새로운 스킬창
    virtual void Refresh() = 0;
};

class IBasicInfoView
{
public:
    virtual ~IBasicInfoView()   {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;

    virtual const UIRECT& GetUIWindowGlobalPos() = 0;
    virtual void SetUIWindowGlobalPos( const D3DXVECTOR2& vPos ) = 0;
};

class IExplainInterface
{
public:
    virtual ~IExplainInterface()    {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;

    virtual BOOL IsSlotBoxVisible( unsigned int Index ) const = 0;
    virtual BOOL IsSlotBoxVisibleAtInven() const = 0;
    virtual void SetSlotBoxVisible( unsigned int Index, BOOL Visible ) = 0;
    virtual void SetSlotBoxVisibleAtInven( BOOL State ) = 0;
    virtual void SetSlotBoxOriginPos( D3DXVECTOR2 vPos, int nIndex ) = 0;
    virtual void SetSlotBoxOriginPos( UIRECT rcWindow, int nIndex ) = 0;
    virtual void SetSlotBoxVisibleSingle( BOOL bVisible ) = 0;
    virtual const UIRECT& GetSlotBoxGlobalPos() = 0;
};

class IUILeftTopGroup
{
public:
    virtual ~IUILeftTopGroup()  {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;

    virtual void SetShotcutText( DWORD nID, UI::String& strTemp ) = 0;
    virtual const UIRECT& GetUIWindowGlobalPos() = 0;
    virtual void SetUIWindowGlobalPos( const D3DXVECTOR2& vPos ) = 0;
    virtual CBasicPotionTray* GetBasicPotionTray() = 0;
	virtual void SetQuickPotionSlotIndex(int nIndex )= 0;
	virtual void ResetQuickSlotText() = 0;
};

class ISkillTrayTab
{
public:
    virtual ~ISkillTrayTab()    {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;

    virtual const UIRECT& GetUIWindowGlobalPos() = 0;
	virtual const UIRECT& GetCurrentSkillSlotGlobalPos() = 0;
	virtual const WORD	  FindSkillSlot( const SNATIVEID& sSkillID ) = 0;
	virtual const UIRECT& FindSkillSlotGlobalPos( const SNATIVEID& sSkillID ) = 0;
    virtual void SetShotcutText( DWORD nID, UI::String& strTemp ) = 0;
    virtual INT GetTabIndex() = 0;
    virtual void SetTabIndex( INT nIndex ) = 0;
	virtual void SelectSlot( INT nIndex ) = 0;
	virtual void ResetSlotText() = 0;
    virtual void TrayToggle () = 0;
    virtual void TrayOpen () = 0;
	virtual BOOL IsMoveLock() = 0;
};

class IBasicQuickSkillSlot
{
public:
    virtual ~IBasicQuickSkillSlot() {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;

    virtual const UIRECT& GetSkillImageGlobalPos() = 0;
};

class ITargetInfoDisplay
{
public:
    virtual ~ITargetInfoDisplay()   {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;

	virtual const bool CheckPriority( const UI::EMTARGETINFO_TYPE emInfoType, const STARGETID& sTargetID ) = 0;

	virtual const bool IsMouseOverVisible() = 0;
	virtual void SetMouseOverVisibleOff() = 0;
    virtual const UIRECT& GetMouseOverGlobalPos() = 0;
    virtual void SetMouseOverGlobalPos( const D3DXVECTOR2& vPos ) = 0;
    virtual void SetMouseOverTargetInfo( const STARGETID& sTargetID, float fPercent, UI::String strName, D3DCOLOR dwColor ) = 0;
    virtual void SetMouseOverAuthenticatorInfo( const STARGETID& sTargetID, float fPercentSM, float fPercentHA, float fPercentBH, ENUM_SCHOOL emAuthSchool ) = 0;
    virtual const UI::String GetMouseOverTargetName() const = 0;

	virtual void SetSelectVisibleOff() = 0;
	virtual void SetSelectTargetInfo( const STARGETID& sTargetID, float fPercent, UI::String strName, D3DCOLOR dwColor ) = 0;
	virtual void SetSelectAuthenticatorInfo(  const STARGETID& sTargetID, float fPercentSM, float fPercentHA, float fPercentBH, ENUM_SCHOOL emAuthSchool ) = 0;
	virtual const UIRECT& GetSelectGlobalPos() = 0;
	virtual void SetSelectGlobalPos( const D3DXVECTOR2& vPos ) = 0;

};

class ICursorTargetInfo
{
public:
	virtual ~ICursorTargetInfo() {};

public:
	virtual void CreateUIWindowAndRegisterOwnership() = 0;

	virtual void UpdateInfo() = 0;
};

class IBalloonSpaceBar
{
public:
    virtual ~IBalloonSpaceBar() {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;
};

class ITutorialViewStep
{
public:
    virtual ~ITutorialViewStep()    {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;

    virtual const UIRECT& GetUIWindowGlobalPos() = 0;
    virtual void SetUIWindowGlobalPos( const D3DXVECTOR2& vPos ) = 0;
    virtual void SetUIWindowLocalPos( const D3DXVECTOR2& vPos ) = 0;
    virtual void SetOnStep( int nIndex ) = 0;
};

class ILunchBoxTimeDisplay
{
public:
    virtual ~ILunchBoxTimeDisplay() {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;
};

class ISystemBuffTimeDisplay
{
public:
    virtual ~ISystemBuffTimeDisplay() {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;
};

class IVehicleWindow
{
public:
    virtual ~IVehicleWindow()   {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;

    virtual void SetVehicleInfoID( DWORD dwMasterID ) = 0;
    virtual DWORD GetMasterID() = 0;
};

class IProgressDisplay
{
public:
    virtual ~IProgressDisplay()    {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;
    virtual void Show ( const UI::EMPROGRESSDISPLAY_TYPE emType, const std::string& strText ) = 0;
	virtual void ShowPublic ( const std::string& strText ) = 0;
    virtual void Update () = 0;

};

class IStudentRecordWindow
{
public:
    virtual ~IStudentRecordWindow()   {};

public:
    virtual void        CreateUIWindowAndRegisterOwnership( LPDIRECT3DDEVICEQ pd3dDevice ) = 0;
    virtual void        UpdateActivity            () = 0;
    virtual void        UpdateActivityPoint       () = 0;
    virtual void        UpdateAttendance          () = 0;
    virtual void        UpdateAttendanceTask      () = 0;
	virtual void        UnlockAttendanceUI() = 0;
    virtual const bool  IsOpenable                () = 0;
    virtual const bool  IsOpenActivityPage        () = 0;
    virtual const bool  IsOpenAttendancePage      () = 0;
    virtual const bool  IsOpenableActivityPage    () = 0;
    virtual const bool  IsOpenableAttendancePage  () = 0;
    virtual const bool  IsActivityProgressDisplay () = 0;
    virtual const bool  IsOpenModalWindow         () = 0;
    virtual void        OpenAttendanceWindow      () = 0;
    virtual void        CloseModalWindow          () = 0;


public: // RnAttendance
	virtual void	RnAttendanceSetDayInfoText( const CString strEventText, const D3DCOLOR& TextColor = NS_UITEXTUTIL::ENABLE ) = 0;
	virtual void	RnAttendanceClerText() = 0;
	virtual void	REFRESH_RnattendancePage() = 0;
};


class IPointShopWindow
{
public:
    virtual ~IPointShopWindow()   {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;

    virtual void RefreshWindow() = 0;
    virtual void ReLoadItemList() = 0;
    virtual void ReLoadAll() = 0;
    virtual void SetPointText( __int64 nPoint ) = 0;
    virtual void SetAllCategory( bool bCheck ) = 0;
    virtual void UnCheckIncludeTextSearch() = 0;
    virtual __int64 GetPriceInCart() = 0;
    virtual void ResetCartItem( SNATIVEID sItemID ) = 0;
    virtual void SearchItem( std::string& strSearchName ) = 0;
	virtual std::string GetSearchName() = 0;
    virtual void SetForceCloseWindow( bool _bForceClose ) = 0;
    virtual bool IsForceCloseWindow() = 0;
};

class IRebirthWindow
{
    public:
        virtual ~IRebirthWindow()   {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;

    virtual void SetItemRebirth( bool bItemRebirth, bool bAutoRebirth, bool bCantUseRebirth ) = 0;
    virtual void SetReviveSkill( bool bAct ) = 0;
    virtual bool CapableVisible() = 0;
    virtual void SetAutoRebirthTime( UINT iTime ) = 0;
};

class IChatTypeList
{
public:
    virtual ~IChatTypeList() {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;
    virtual void SetUIGlobalPos( const UIRECT& rcPosition ) = 0;
    virtual const UIRECT& GetUIGlobalPos() = 0;
};

class ICompetitionNotifyButton
{
public:
    virtual ~ICompetitionNotifyButton() {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;
    virtual void Update() = 0;
    virtual void SetLock(const bool _bLock) = 0;
    virtual void ClearAlarm() = 0;
    virtual void SetAlarm(const DWORD _dwMatchingNum, const char* szText) = 0;
    virtual void SetTooltip(const DWORD _dwMatchingNum, const char* szText) = 0;
};

class IPopupMenu
{
public:
    virtual ~IPopupMenu() {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;
    virtual void OpenPopup( popupmenu::SP_POPUP_MENU PopupMenu ) = 0;

};

class IInfoCTFDisplay
{
public:
    virtual ~IInfoCTFDisplay() {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;
    virtual void Update() = 0;

};

class ICTFResultWindow
{
public:
    virtual ~ICTFResultWindow() {};

public:
	virtual void CreateUIWindowAndRegisterOwnership() = 0;
	virtual void Show() = 0;
	virtual void Update() = 0;

};

class ICompetitionWindow
{
public:
	virtual ~ICompetitionWindow() {};

public:
	virtual void CreateUIWindowAndRegisterOwnership( LPDIRECT3DDEVICEQ pd3dDevice ) = 0;
	virtual const bool IsOpen() = 0;
	virtual void	   Open () = 0;
	virtual void	   Close () = 0;
	virtual void	   Update(int nPage) = 0;
	virtual const UIRECT& GetGlobalPos() = 0;
};

class ICompetitionTopRanking
{

public:
	virtual ~ICompetitionTopRanking() {};

public:
	virtual void CreateUIWindowAndRegisterOwnership() = 0;
	virtual void Open ( const PVP::ENUM_CTF_TYPE emType ) = 0;
	virtual void Close () = 0;
	virtual void SetTextTime( const CString strTime ) = 0;
};

class IRnCDMTopRanking
{

public:
	virtual ~IRnCDMTopRanking() {};

public:
	virtual void CreateUIWindowAndRegisterOwnership() = 0;
	virtual void OpenCDMTopRanking() = 0;
	virtual void Close () = 0;
	virtual void SetCDMTopList(std::vector<DWORD> vecClubId,
								LONGLONG lnStartTime,
								LONGLONG lnEndTime,
								std::vector<DWORD> vecRank ) = 0;
	virtual void SetClubMasterClubName(	std::vector<std::string> vecMasterName,
										std::vector<std::string> vecClubName ) = 0;
};

class IReservedSkillAnimation
{

public:
	virtual ~IReservedSkillAnimation() {};

public:
	virtual void CreateUIWindowAndRegisterOwnership() = 0;
	virtual void Show ( const SNATIVEID& sSkillID ) = 0;
	virtual void HideAll () = 0;
};

class IProductButton
{
public:
	virtual ~IProductButton() {};

public:
	virtual void CreateUIWindowAndRegisterOwnership() = 0;
};

class IProductProgress
{
public:
    virtual ~IProductProgress() {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;
    virtual void StartProduct( DWORD nID, WORD nCount ) = 0;
	virtual void GoAlongProduct() = 0;
	virtual void EndProduct() = 0;
	virtual void CancelProduct() = 0;
	virtual bool IsProdution() = 0;
};

class IProductWindow
{
public:
    virtual ~IProductWindow() {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;
    virtual void SetSeletedTabID( UIGUID ControlID ) = 0;
    virtual void SetItemMix( SProductRecipe* pProductRecipe ) = 0;
	virtual void SetProducting( bool bProducting ) = 0;
	virtual void DialogMaterialConfirm() = 0;
};

class IProductResetWindow
{
public:
	virtual ~IProductResetWindow() {};

public:
	virtual void CreateUIWindowAndRegisterOwnership() = 0;
	virtual void Open(WORD wResetAbleNum, WORD wPosX, WORD wPosY) = 0;
};

class IOddEvenWindow
{
public:
	virtual ~IOddEvenWindow(void){};

public:
	virtual void CreateUIWindowAndRegisterOwnership(void) = 0;
	virtual void SetVisible(const bool bVisible) = 0;
};

class ITexasHoldemWindow
{
public:
	virtual ~ITexasHoldemWindow(void){};

public:
	virtual void CreateUIWindowAndRegisterOwnership(void) = 0;
	virtual void SetVisible(const bool bVisible) = 0;

	virtual void ClosedExchangeWindow() = 0;
};

class ITexasHoldemWebWindow
{
public:
	virtual ~ITexasHoldemWebWindow(void){};

public:
	virtual void CreateUIWindowAndRegisterOwnership(void) = 0;

	virtual void RunTexasHoldemWindow(const bool bVisible) = 0;

	virtual void SetURL( const std::string& strURL, const std::string& strPOST ) = 0;
};

class IFlyCameraControlWindow
{
public:
	virtual ~IFlyCameraControlWindow(void){};
public:
	virtual void CreateUIWindowAndRegisterOwnership(void) = 0;
	virtual const D3DXVECTOR3& GetLastInputPos(void) = 0;
};

class ISetitemPreviewWindow
{
public:
	virtual ~ISetitemPreviewWindow(void){}
public:
	virtual void CreateUIWindowAndRegisterOwnership(void) = 0;
	virtual void SetitemGroupComposition(SSETITEM* pSetitem, STARGETID sTargetID = STARGETID()) = 0;
	virtual void ResetSetitemComposition(void) = 0;
};

class IClubWindow
{
public:
    virtual ~IClubWindow() {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;
    virtual void LoadClubAllianceList() = 0;
    virtual void LoadClubBattleList() = 0;
    virtual void RefreshState() = 0;
    virtual void SetAnnouncement() = 0;
    virtual const DWORD GetClubMemberID() = 0;
    virtual const DWORD GetBattleClubID() = 0;
    virtual const DWORD GetUnionMemberID() = 0;
    virtual const UI::String GetClubMemberName() = 0;
    virtual void RefreshClubList() = 0;
    virtual void RefreshGuidanceMap() = 0;
    virtual void ShowGradePopup() = 0;
    virtual void ShowClubLog() = 0;
    virtual void ClubStorageUpdateMoney() = 0;
    virtual void ClubUpdateNewbieNotice() = 0;
};

class IPrivateMarketSearchBuy
{
public:
	virtual ~IPrivateMarketSearchBuy()    {};

public:
	virtual void CreateUIWindowAndRegisterOwnership() = 0;
	virtual void PrivatemarketRefreshSearchResult() = 0;
	virtual void CloseAllRollOver() =0;
	virtual void SetPageWaiting( bool bIsWaiting ) = 0;
	virtual bool IsPageWaiting() = 0;
	virtual void SetWhishPageEvent(bool bBOOL) = 0;

    virtual void Refresh() = 0;
};

class IPVEUserInterface
{
public:
	virtual ~IPVEUserInterface() = 0 {}
	virtual void CreateUIWindowAndRegisterOwnership() = 0;	
    virtual void SetEntranceConfirm(bool bShow) = 0;
    virtual void SetEntranceState(int nCur, int nAll, int nSec, bool bShow) = 0;
    virtual void SetStage(int nStage, bool bShow) = 0;
    virtual void SetMessage(const TCHAR* pText, bool bShow) = 0;
    virtual void SetMonsterIndicator(int nCur, int nAll, bool bShow) = 0;
    virtual void SetPositioningIndicator(int nX, int nY, int nNow, int nNeed, bool bShow) = 0;
	virtual void SetTimeIndicator(int nSec, bool bShow) = 0;
    virtual void SetStageIndicator(int nStage, int nEnd, int nRange, int nSec, bool bBlank, bool bShow) = 0;
    virtual void SetStageState(int nState, bool bShow) = 0;
    virtual void SetRetry(LONGLONG nMoney, int nTime, bool bShow) = 0;
    virtual void SetStageResult() = 0;

	virtual void SetNewInstanceRetry(LONGLONG nMoney, int nTime, bool bShow, InstanceSystem::InstanceMapID nInstanceID, SNATIVEID sKeyMapID) = 0;
};

class IPTournamentUserInterface
{
public:
	virtual ~IPTournamentUserInterface() = 0 {}
	virtual void CreateUIWindowAndRegisterOwnership() = 0;	
	virtual void SetEntranceConfirm(bool bShow) = 0;
	virtual void SetEntranceConfirm(bool bShow,int waitentrance) = 0;
	virtual void SetMonsterIndicator(int nCur, int nAll, bool bShow) = 0;
	virtual void SetPositioningIndicator(int nX, int nY, int nNow, int nNeed, bool bShow) = 0;
	virtual void SetStageIndicator(int nSec, bool bBlank, bool bShow) = 0;
	virtual void SetStageState(int nState, bool bShow) = 0;
	virtual void SetCloseAll() = 0;
	void SetStageTimeIndicator( int nSec, bool bBlank, bool bShow );
};


class IItemRebuildRenewal
{
public:
	virtual ~IItemRebuildRenewal(void){};

public:
	virtual void CreateUIWindowAndRegisterOwnership(void) = 0;
	virtual const UIRECT& GetGlobalPos(void) = 0;
	virtual void SetGlobalPos(const D3DXVECTOR2& vPosition) = 0;
	virtual void SetNpc(const SNATIVEID& sNPCID, const DWORD dwNPCGlobalID) = 0;
	virtual void SetPreInventoryItem( WORD wPosX, WORD wPosY ) = 0;	
	virtual void GetPreInventoryItemPos( WORD& wOutPosX, WORD& wOutPosY ) = 0;
	virtual void ResetPreInventoryItem(void) = 0;
	virtual void SetPreStampItem( WORD wPosX, WORD wPosY ) = 0;
	virtual void GetPreStampItemPos( WORD& wOutPosX, WORD& wOutPosY ) = 0;
	virtual void ResetPreStampItem(void) = 0;
	virtual void SetItemRebuildResetCheckBox(void) = 0;
	virtual void SetItemRebuildSetCheckBox(const SITEMCUSTOM& sItemCustom, const RandomOpt::FixOption& fixOption) = 0;
};

class IColorProperty
{
public:
    virtual ~IColorProperty()    {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;
    virtual void bindLightProperty(D3DLIGHT9* _pLight, const DWORD _index = 0) = 0;
    virtual void bindMaterialProperty(D3DMATERIAL9* _pMaterail) = 0;
    virtual void resetProperty(void) = 0;
};

class IProgessTimer
{
public:
	virtual ~IProgessTimer()    {};

public:
	virtual void CreateUIWindowAndRegisterOwnership() = 0;
	virtual void SetProgessTimer( UINT iTime,float fPosX, float fPosY, const char* szText, int nTextIndex ) = 0;
};

class ISimpleSystemMessageDisplay
{
public:
    virtual ~ISimpleSystemMessageDisplay() {};

public:
    virtual void CreateUIWindowAndRegisterOwnerShip() = 0;
    virtual void PrintSimpleSystemMessage( const CString& strMessage, const D3DCOLOR dwMessageColor, const DWORD dwFontSize, const float fLifeTime) =0;
    virtual void ClearSimpleSystemMessage()=0;
};

// new Instance system;
class ISimpleTimerMessageBox
{
public:
    virtual ~ISimpleTimerMessageBox() {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;
    virtual void ShowSimpleTimerMessageBox(const NET_MSG_GENERIC* pMsg) = 0;
    virtual void Close() = 0;
};

class IRnCDMProgress
{
public:
	virtual ~IRnCDMProgress()    {};

public:
	virtual void CreateUIWindowAndRegisterOwnership() = 0;
	virtual void RefreashCdmProgress( DWORD	dwNumberOneClubMarkVER,
									  DWORD	dwNumberOneClubDbNum,
									  char*	szNumberOneName,
									  int	nNumberOneScore,
									  int	nMyRank,
									  DWORD	dwMyScore,
									  int	nHour,
									  int	nMin,
									  int	nSecond ) = 0;
};

class IRnCDMRankWindow
{
public:
	virtual ~IRnCDMRankWindow() {};

public:
	virtual void CreateUIWindowAndRegisterOwnership() = 0;
	virtual void Show() = 0;
	virtual void Update() = 0;
    virtual void SetRankSloatData(InstanceSystem::RNCDM_RANK_SLOT vecRnCdmRankSlot) = 0;
	virtual InstanceSystem::RNCDM_RANK_SLOT GetRankSlotData(int nIndex) = 0;
	virtual void RankListUpdate() = 0;
};

class IRnCDMClubInfo
{
public:
	virtual ~IRnCDMClubInfo() {};

public:
	virtual void CreateUIWindowAndRegisterOwnership() = 0;
	virtual void SetMemberImage( int nIndex, EMCHARINDEX emCharIndex ) = 0;
	virtual void SetMemberName( int nIndex, const CString strMemberName ) = 0;
	virtual void SetTitleClubNameMemberNum( const CString strClubName, int nMembernum ) = 0;
	virtual void SetRnCDMClubInfoPos(int nPosX, int nPosY) = 0;

};

class ICaptureTheFlagEffectMan
{
public:
    virtual ~ICaptureTheFlagEffectMan() {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;
    virtual void SetFlagGuideArrow( const int _flagNum, const STARGETID& sTargetID, const bool bBlink ) = 0;
};

class IGuidanceAuthProgressUI
{
public:
    virtual ~IGuidanceAuthProgressUI() {};
public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;
    virtual void SetAuthTime(const bool bVisible, const float _fAuthTime, const float _fCurrentTime = 0.0f) = 0;
};
class IGuidanceProgressInfoUI
{
public:
    virtual ~IGuidanceProgressInfoUI() {};
public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;
    virtual void SetProgressInfo(const NET_MSG_GENERIC* _pMessag) = 0;
};
class IGuidanceRankInfoUI
{
public:
    virtual ~IGuidanceRankInfoUI() {};
public:
    virtual void CreateUIWindowAndRegisterOwnership() = 0;
    virtual void SetRankInfo(const NET_MSG_GENERIC* _pMessag) = 0;
    virtual void SetRankMyInfo(const NET_MSG_GENERIC* _pMessag) = 0;
};


class ICTIAuthProgressUI
{
public:
	virtual ~ICTIAuthProgressUI() {};
public:
	virtual void CreateUIWindowAndRegisterOwnership() = 0;
	virtual void SetAuthTime(const bool bVisible, const float _fAuthTime, const float _fCurrentTime = 0.0f) = 0;
};
class ICTIProgressInfoUI
{
public:
	virtual ~ICTIProgressInfoUI() {};
public:
	virtual void CreateUIWindowAndRegisterOwnership() = 0;
	virtual void SetProgressInfo(const NET_MSG_GENERIC* _pMessag) = 0;
};
class ICTIRankInfoUI
{
public:
	virtual ~ICTIRankInfoUI() {};
public:
	virtual void CreateUIWindowAndRegisterOwnership() = 0;
	virtual void SetRankInfo(const NET_MSG_GENERIC* _pMessag) = 0;
	virtual void SetRankMyInfo(const NET_MSG_GENERIC* _pMessag) = 0;
};
// end : new Instance system;


class IRnAttendance
{
public:
	virtual ~IRnAttendance(){};

};

class IRnCompetitionWindow
{
public:
	virtual ~IRnCompetitionWindow() {};

public:
	virtual void CreateUIWindowAndRegisterOwnership( LPDIRECT3DDEVICEQ pd3dDevice ) = 0;
	virtual const bool IsOpen() = 0;
	virtual void	   Open () = 0;
	virtual void	   Close () = 0;
	virtual void	   Update(int nPage) = 0;
	virtual const UIRECT& GetGlobalPos() = 0;

	//Rn CDM CompetitionWindow
	virtual void SetCDMMapinfo(std::string	strMapName,
		int nTimeLimit, 
		int nPlayerLimite, 
		int nLevelLimit ) = 0;
	virtual void SetCDMMapImageName(std::string strMapImageName) = 0;
	virtual void SetCDMBattelState(int nStartTime, int nEndTime) = 0;
	virtual void UpDateWdilBuinding() = 0;
};

class IRnCompetitionResultWindow
{
public:
    virtual ~IRnCompetitionResultWindow() {};

public:
    virtual void CreateUIWindowAndRegisterOwnership( LPDIRECT3DDEVICEQ pd3dDevice ) = 0;
    virtual const bool msgProc_RnCompetitionResult(const NET_MSG_GENERIC* _pMessage) = 0;
    virtual void ShowRnCompetitionRewardTooltop(const int emTitleType, const InstanceSystem::RnCompetitionResultContentRewardObj* pReward) = 0;
};

class ITournamentWindowDashBoard
{
public:
	virtual ~ITournamentWindowDashBoard(){}

public:
	virtual void OnCreate() = 0;
	virtual void OnClose() = 0;

	virtual void OnVisible(bool visible) = 0;
	virtual bool GetVisible() = 0;

	virtual void SetPage(DWORD dwPageNum)=0;

	virtual void OnFrameMove() = 0;
	virtual const UIRECT& GetGlobalPos()=0;
};

class ITournamentWindowUI
{
public:
	virtual ~ITournamentWindowUI(){}

public:
	virtual void OnCreate() = 0;
	virtual void OnClose() = 0;

	virtual void OnVisible(bool visible) = 0;
	virtual bool GetVisible() = 0;

	virtual void WinLoseScoreVisible(bool visible) = 0;
	virtual void OnFrameMove() = 0;
	virtual const UIRECT& GetGlobalPos()=0;
};

class ITournamentToToWindow
{
public:
	virtual ~ITournamentToToWindow(){}

public:
	virtual void OnCreate() = 0;
	virtual void OnClose() = 0;

	virtual void OnVisible(bool visible) = 0;
	virtual bool GetVisible() = 0;

	virtual void SetPage(DWORD dwKagNum,DWORD dwPageNum)=0;

	virtual void OnFrameMove() = 0;

	virtual const UIRECT& GetGlobalPos()=0;
};

class ITournamentMoreInfoWindow
{
public:
	virtual ~ITournamentMoreInfoWindow(){}

public:
	virtual void OnCreate() = 0;
	virtual void OnClose() = 0;

	virtual void OnVisible(bool visible) = 0;
	virtual bool GetVisible() = 0;

	virtual void SetPage(DWORD dwPageNum)=0;

	virtual void OnFrameMove() = 0;
	virtual const UIRECT& GetGlobalPos()=0;
};


// UI Util class.
class CUIControlNpcActionable;

//
const float fDEFAULT_WAITTIME_LEFT = 5.0f;

class CInnerInterface : public InterfaceBase, public ITranslateUIMsg
{
private:
	//	정보 표시 타입
	enum ET_INFOTYPE
	{		
		ET_ITEM_INFO,
		ET_SKILL_INFO,
		ET_GENERAL_INFO,
	};

	enum EM_INFO_DISPLAY_TYPE
	{
		EM_INFO_DISPLAY_BASIC,
		EM_INFO_DISPLAY_EX,
		EM_INFO_DISPLAY_LINK,
	};
private:
	struct SSkillToolTip
	{
		UIGUID nID;
		CSkillInfoTooltip* pControl;
	};

	typedef std::vector< SSkillToolTip > SkillToolTipVec;
	typedef SkillToolTipVec::iterator SkillToolTipVecIter;

public:
    CInnerInterface(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice, bool bToolMode);
    virtual	~CInnerInterface();
//  static CInnerInterface& GetInstance();

public:
	static const int nOUTOFRANGE;
	static const float fMENU_LIFTUP;

private:
    EngineDeviceMan* m_pEngineDevice;
    SUBPATH* m_pPath;
    bool m_bToolMode;

    // 등록되는 interface.

	// Lotto System;
	ILottoMainUI*				m_pLottoMainUI;
	ILottoBuyLottoUI*			m_pLottoBuyLottoUI;
	ILottoBuyListInfo*			m_pLottoBuyListInfo;
	ILottoWinManListInfo*		m_pLottoWinManListInfo;
	ILottoDisplayUI*			m_pLottoDisplayUI;
	ILottoNotifyButton*			m_pLottoNotifyButton;
	ISummonStateDisplayUI*		m_pSummonStateDisplayUI;
	IWorldBattleButton*			m_pWorldBattleButton;
	IFactionInfoWindow*			m_pFactionInfoWindow;
	IFlyCameraControlWindow*	m_pFlyCameraControl;
	ISetitemPreviewWindow*		m_pSetitemPreviewWindow;
    IMiniMap*						m_pMiniMap;
	IDPSWindow*					m_pDPSWindow;
    IKeySettingWindow*			m_pKeySettingWindow;
    IChatMacroWindow*			m_pChatMacroWindow;
    INameDisplayMan*			m_pNameDisplayMan;
    IPrivateMarketShowMan*	m_pPrivateMarketShowMan;
    ISummonNameDisplayMan*	m_pSummonNameDisplayMan;
    ISkillWindowToTray*			m_pSkillWindowToTray;
    IPartyWindow*				m_pPartyWindow;
	IPartyWindowRenewal*		m_pPartyWindowRenewal;
	IPartyYesNoWindow*		m_pPartyYesNoWindow;
	IExpeditionInviteWindow*	m_pExpeditionInviteWindow;
	IExpeditionCheckReadyWindow*	m_pExpeditionCheckReadyWindow;
	IPartyStateWindow*			m_pPartyStateWindow;
	IPartyTenderWindow*		m_pPartyTenderWindow;
	IPartyTenderNotifyButton*		m_pPartyTenderNotifyButton;
	IPartyDistributionWindow*	m_pPartyDistributionWindow;
	IPartyDistributionListWindow*	m_pPartyDistributionListWindow;
	IMiniExpeditionWindow*		m_pMiniExpeditionWindow;
    IBasicChatWindow*           m_pBasicChat;
    IChatOptionWindow*          m_pChatOptionWindow;
    IChatTypeList*              m_pChatTypeList;
    ITradeWindow*               m_pTradeWindow;
    ITradeInventoryWindow*      m_pTradeInventoryWindow;
    IPtoPWindow*                m_pPtoPWindow;
    IInventoryWindow*           m_pInventoryWindow;
	IInventory*					m_pInventory;
    IItemMove*                  m_pItemMove;
    IMarketWindow*              m_pMarketWindow;
    IHeadChatDisplayMan*        m_pHeadChatDisplayMan;
    IConftModalWindow*          m_pConftModalWindow;
    IPartyModalWindow*          m_pPartyModalWindow;
	IChatGroupModalWindow*		m_pChatGroupModalWindow;
    IConftDisplayMan*           m_pConftDisplayMan;
    IQuestWindowRenewal*        m_pQuestWindowRenewal;
    IFriendWindow*              m_pFriendWindow;
	IChatGroupInviteWindow*		m_pChatGroupInviteWindow;
    IModalWindowInner*          m_pModalWindow;
    IShopItemSearchWindow*      m_pShopItemSearchWindow;
    IItemSearchResultWindow*    m_pItemSearchResultWindow;
    IMapRequireCheck*           m_pMapRequireCheckWindow;
    IPostBoxWindow*             m_pPostBoxWindow;
    IPostNotifyButton*          m_pPostNotifyButton;
    IQuestHelper*               m_pQuestHelper;
    ITutorialDialogue*          m_pTutorialDialogue;
    IBalloonText*               m_pBalloonText;
    ISkillWindow*               m_pSkillWindow;
    IBasicInfoView*             m_pBasicInfoView;
    IExplainInterface*          m_pExplainInterface;
    IUILeftTopGroup*            m_pUILeftTopGroup;
    ISkillTrayTab*              m_pSkillTrayTab;
    IBasicQuickSkillSlot*       m_pBasicQuickSkillSlot;
    ITargetInfoDisplay*         m_pTargetInfoDisplay;
	ICursorTargetInfo*          m_pCursorTargetInfo;
    IBalloonSpaceBar*           m_pBalloonSpaceBar;
    ITutorialViewStep*          m_pTutorialViewStep;
    ILunchBoxTimeDisplay*       m_pLunchBoxTimeDisplay;
    ISystemBuffTimeDisplay*     m_pSystemBuffTimeDisplay;
    IVehicleWindow*             m_pVehicleWindow;
    IProgressDisplay*           m_pProgressDisplay;
    IStudentRecordWindow*       m_pStudentRecordWindow;
    IPointShopWindow*           m_pPointShopWindow;
    IRebirthWindow*             m_pRebirthWindow;
    ICompetitionNotifyButton*   m_pCompetitionNotifyButton;
    IPopupMenu*                 m_pPopupMenu;
    IInfoCTFDisplay*            m_pInfoCTFDisplay;
    ICTFResultWindow*           m_pCTFResultWindow;
	ICompetitionWindow*			m_pCompetitionWindow;
	ICompetitionTopRanking*		m_pCompetitionTopRanking;
	IReservedSkillAnimation*	m_pReservedSkillAnimation;
	IProductButton*				m_pProductButton;
    IProductProgress*           m_pProductProgress;
    IProductWindow*             m_pProductWindow;
	IProductResetWindow*        m_pProductResetWindow;	
	IOddEvenWindow*		m_pOddEvenWindow;
	ITexasHoldemWindow*			m_pTexasHoldemWindow;
	ITexasHoldemWebWindow*		m_pTexasHoldemWebWindow;
    IClubWindow*				m_pClubWindow;
	IPrivateMarketSearchBuy*	m_pPrivateMarketSearchBuy;
	IPVEUserInterface*			m_pPVEUserInterface;
	IPTournamentUserInterface*  m_pTournamentUserInterface;
	IProgessTimer*				m_pProgessTimer;

    //new instance system
    ISimpleSystemMessageDisplay*    m_pSimpleSystemMessageDisplay;
    ISimpleTimerMessageBox*         m_pSimpleTimerMessageBox;
	IRnCDMProgress*				m_pRnCDMProgress;
	IRnCDMRankWindow*			m_pRnCDMRankWindow;
	IRnCDMClubInfo*				m_pRnCDMClubInfo;
	IRnCompetitionWindow*		m_pRnCompetitionWindow;
    IRnCompetitionResultWindow* m_pRnCompetitionResultWindow;
	ITournamentWindowDashBoard*	m_pTournamentDashBoard;
	ITournamentWindowUI*		m_pTournamentUI;
	ITournamentToToWindow*		m_pTournamentToToWindow;
	ITournamentMoreInfoWindow*  m_pTournamentMoreInfoWindow;
	IRnCDMTopRanking*			m_pRnCDMTopRanking;
    ICaptureTheFlagEffectMan*   m_pCaptureTheFlagEffecMan;
    IGuidanceAuthProgressUI*    m_pGuidanceAuthProgressUI;
    IGuidanceProgressInfoUI*    m_pGuidanceProgressInfoUI;
    IGuidanceRankInfoUI*        m_pGuidanceRankInfoUI;
	ICTIAuthProgressUI*			m_pCTIAuthProgressUI;
	ICTIProgressInfoUI*			m_pCTIProgressInfoUI;
	ICTIRankInfoUI*				m_pCTIRankInfoUI;
	
private: //	등록되는 모든 컨트롤
	CAcademyConftDisplay*		m_pAcademyConftDisplay;				
	CAdminMessageDisplay*		m_pAdminMessageDisplay;			
	CBasicGameMenu*				m_pGameMenu;		
	CQBoxButton*				m_pQBoxButton;			
	CBasicVarTextBox*			m_pInfoDisplay;			
	CBasicVarTextBox*			m_pInfoDisplayEx;
	CInfoDisplayLink*			m_pInfoDisplayLink;
	CBusWindow*					m_pBusWindow;
	std::vector<CMaxChatWindow*> m_pMaxChatWindow;
    CChatGroupWindow*           m_pChatGroupWindow;
//	CCharacterWindow*			m_pCharacterWindow;
	RnCharacterWindowInterface*	m_pCharacterWindow;
    CReferCharWindow*           m_pReferCharWindow;
    ItemRepairWindow*           m_pItemRepairWindow;

	CClubMake*					m_pClubMake;	
	CClubStorageWindow*			m_pClubStorageWindow;
	CConftConfirm*				m_pConftConfirm;			
	CCountMsg*					m_pCountMsgWindow;	
	CDamageDisplayMan*			m_pDamageDisplayMan;				
	CDialogueWindow*			m_pDialogueWindow;				
	CPetSkinMixImage*			m_pPetSkinMixImage;	
	CItemBankWindow*			m_pItemBankWindow;				
	CVNGainSysInventory*		m_pVNGainSysInventory;				
	CVNGainSysGauge*			m_pVNGainSysGauge;
	CItemRebuild*					m_pItemRebuildWindow;	// ITEMREBUILD_MARK	
	IItemRebuildRenewal*		m_pItemRebuildRenewal;

    CNotifyWindow*              m_pNotifyWindow;
    CNotifyButton*              m_pNotifyButton;
	
	RnMapWindowInterface*		m_pMapWindowInterface;		

	CMapMoveDisplay*			m_pMapMoveDisplay;	
	CMultiMapMoveDisplay*		m_pMultiMapMoveDisplay;
	CPetRebirthDialogue*		m_pPetRebirthDialogue;// Monster7j				
	CPetWindow*					m_pPetWindow;	
	CPrivateMarketMake*			m_pPrivateMarketMake;			
	CPrivateMarketSellWindow*	m_pPrivateMarketSellWindow;				
	CPrivateMarketWindow*		m_pPrivateMarketWindow;

	SelectiveformBoxWindow*		m_pSelformBoxWindow;
	CQuestAlarm*				m_pQuestAlarm;				
	CQuestionItemDisplay*		m_pQuestionItemDisplay;
	CRebirthDialogue*			m_pRebirthDialogue;			
	CSimpleHP*					m_pSimpleHP;				
	CSimpleHP*					m_pSummonHP;			
	CStorageChargeCard*			m_pStorageChargeCard;			
	CStorageWindow*				m_pStorageWindow;			
	CSubMasterSet*				m_pSubMasterSet;		// 부마스터 설정 창
	CSystemMessageWindow*		m_pSystemMessageWindow;		
	CWaitServerDialogue*		m_pWaitServerDisplay;
	CBonusTimeGauge*			m_pBonusTimeGauge;
	CBonusTimeDisplay*			m_pBonusTimeDisplay;
	CBikeWindow*				m_pBikeWindow;
	CThaiCCafeMark*				m_pThaiCCafeMark;
	CItemGarbage*				m_pItemGarbageWindow;
	CItemShopIconMan*			m_pItemShopIconMan;
	CAttendanceBookWindow*		m_pAttendanceBookWindow;
	CClubBattleModal*			m_pClubBattleModalWindow;
	CTaxiWindow*				m_pTaxiWindow;
	CItemMixWindow*				m_pItemMixWindow;
	CItemMixInvenWindow*		m_pItemMixInvenWindow;
	CGatherGauge*				m_pGatherGauge;
	CCdmRankingDisplay*			m_pCdmRankingDisplay;
	CCDMSafeTimeDisplay*		m_pCdmSafeTimeDisplay;
	CSkillTimeDisplay*			m_pSkillTimeDisplay;
	CPartyLinkWindow*			m_pPartyLinkWindow;
	CUIColorSelectorWindow*		m_pVehicleColorChangeWindow;
	CBikeGetOffWindow*			m_pBikeGetOffWindow;
	CBikeMessageWindow*			m_pBikeMessageWindow;
	CBikeBoostWindow*			m_pBikeBoostWindow;
	CStatsResetWindow*			m_pStatsResetWindow;
	CUIColorSelectorWindow*	    m_pCostumColorChangeWindow;
	CPetHungry*					m_pPetHungry;

    CSNSWindow*					m_pSNSWindow;
    CFaceBookWindow*			m_pFaceBook;
    CTwitterWindow*				m_pTwitter;
    CItemShopWindowWeb*         m_pItemShopWindowWeb;

    CItemPreviewWindow*         m_pItemPreviewWindow;

    CStudentRecordNotifyButton* m_pStudentRecordNotifyButton;
	IColorProperty* m_pColorProperty;

	CCarWindow*		m_pCarWindow;
	CRearcarWindow*	m_pRearcarWindow;

	CMouseTip*						m_pMouseTip;
	CTutorialDialogueExplainText*	m_pTutorialDialogueExplainText;
	CTutorialInterfaceLeft*			m_pTutorialInterfaceLeft;
	CTutorialInterfaceRight*		m_pTutorialInterfaceRight;
	CTutorialInterfaceUp*			m_pTutorialInterfaceUp;
	CTutorialInterfaceDown*			m_pTutorialInterfaceDown;
	CTutorialInterfaceZoomIn*		m_pTutorialInterfaceZoomIn;
	CTutorialInterfaceZoomOut*		m_pTutorialInterfaceZoomOut;
	CMinimapTarget*					m_pMinimapTarget;
	CTutorialSkipButton*			m_pTutorialSkipButton;
	CTutorialDamageAlarm*			m_pTutorialDamageAlarm;
	CBasicVarTextBox*				m_pExplainVarText;



    CItemInfoTooltip* m_pItemInforTooltip;
	CItemInfoTooltip* m_pItemInforTooltipWear;
	CItemInfoTooltip* m_pItemInforTooltipLink;
	CItemInfoTooltip* m_pItemInforTooltipDurability;

    CSkillInfoTooltip* m_pSkillTooltip;
	CSkillInfoTooltip* m_pSkillTooltipSubSkill;	// 서브 스킬;
	CSkillLinkInfoTooltip* m_pSkillTooltipLinkSkill;	// 링크 스킬;
	CSkillInfoTooltip* m_pSkillTooltipDelayActionSkill;
    CForceRebirth* m_pForceRebirth;
    CNotifyCTFSpurt* m_pNotifyCTFSpurt;
    CDialogueWindowRenewal* m_pDialogueWindowRenewal;

    CItemToolTipGroup* m_pInvenItemTooltipGroup;
    CItemToolTipGroup* m_pWishItemTooltipGroup;
    CItemToolTipGroup* m_pMarketWishItemTooltipGroup;

	SkillToolTipVec m_vecSkillToolTipRegist;
	SkillToolTipVec m_vecSkillToolTipPool;
	SkillToolTipVec m_vecSkillToolTipUsed;

	//토너먼트 UI
	CTournamentEntranceConfirm*	m_pEntranceConfirm;			// 입장 취소
	TournamentObserveWindow*	m_pTournamentObserveWindow;	// 토너먼트 관전자 UI
	CTournamentMulty*			m_pTournamentMulty;			// Score 내기 UI
	TournamentMiniExpeditionWindow* m_pBigGroupWindowRight;	// 오른쪽팀 캐릭터 정보
	TournamentMiniExpeditionWindow* m_pBigGroupWindowLeft;	// 왼쪽팀 캐릭터 정보
	CTournamentExpeditionInfoGroup* m_pMiniGroupWindowRight; // 오른쪽팀 캐릭터 정보
	CTournamentExpeditionInfoGroup* m_pMiniGroupWindowLeft; // 오른쪽팀 캐릭터 정보
	
	CTournamentIndicator* m_pTournamentIndicator;						// 토너먼트 이디케이터.

	DoublePoint*		m_pDoublePoint;

	CCostumeStatWindow*	m_pCostumeStatWindow;

private:
	CUIControl*	m_pSystemMessageWindowDummy;
	CUIControl*	m_pMapMoveDisplayDummy;
	CUIControl*	m_pMultiMapMoveDisplayDummy;
	CUIControl*	m_pLeftTopGroupDummy;
	CUIControl*	m_pBasicInfoViewDummy;
	CUIControl* m_pBasicPotionTrayDummy;
	CUIControl* m_pQuestAlarmDummy;

	void ResetControl();

private:
    bool m_bFirstVNGainSysCall;

public:
    //! Manangement control
    typedef std::multimap<UIGUID, CUIControl*>                          MM_MANAGEMENT_CONTROL;
    typedef MM_MANAGEMENT_CONTROL::iterator                             ITER_MANAGEMENT_CONTROL;
    typedef std::pair<ITER_MANAGEMENT_CONTROL, ITER_MANAGEMENT_CONTROL> RANGE_MANAGEMENT_CONTROL;

    MM_MANAGEMENT_CONTROL m_mmManageControl;

private:
	//! Management Non Close UI;
	std::vector<UIGUID> m_vNonCloseUI;

public:
	inline void AddNonCloseUI( UIGUID uiID ) { m_vNonCloseUI.push_back( uiID ); }

public:
    SUBPATH* GetSubPath() { return m_pPath; }

public:
    static void UiSetResolution( long lResolution );
    static long UiGetResolution();

public:
    virtual void UiOneTimeSceneInit() { OneTimeSceneInit(); }
    virtual HRESULT UiInitDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice) { return InitDeviceObjects(pd3dDevice); }
    virtual HRESULT UiFinalCleanup() { return FinalCleanup(); }
    virtual	HRESULT UiFrameMove(LPDIRECT3DDEVICEQ pd3dDevice, float fElapsedTime) { return FrameMove(pd3dDevice, fElapsedTime); }
    virtual HRESULT UiRender(LPDIRECT3DDEVICEQ pd3dDevice);
    virtual HRESULT UiRestoreDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice);
    virtual HRESULT UiInvalidateDeviceObjects();
    virtual HRESULT UiDeleteDeviceObjects() { return DeleteDeviceObjects(); }
    virtual void	UiMsgProcess ( LPVOID msgBuffer ) { MsgProcess(msgBuffer); }

    virtual BOOL UiRegisterControl( CUIControl* pNewControl, bool bPosisionKeep = false );
    virtual BOOL UiIsVisibleGroup(UIGUID ControlID);
	virtual void UiShowGroupUpdate(UIGUID ControlID, bool bMakeMsg = false);
    virtual void UiShowGroupTop(UIGUID ControlID, bool bMakeMsg = false);
    virtual void UiShowGroupBottom(UIGUID ControlID, bool bMakeMsg = false);
    virtual void UiShowGroupFocus(UIGUID ControlID, bool bMakeMsg = false);
	virtual void UiSetGroupLock(UIGUID ControlID, bool bLock = true); // CUIMan::SetGorupLock() 함수 주석 참조.
    virtual void UISetModalGroup(UIGUID ControlID, bool bModal = true);
    virtual void UiHideGroup(UIGUID ControlID, bool bMakeMsg = false);
	virtual void UiRefresh( UIGUID ControlID );
	virtual void DeleteUIMeshData();

    HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
    HRESULT DeleteDeviceObjects ();    
    HRESULT OneTimeSceneInit ();
    HRESULT FrameMove ( LPDIRECT3DDEVICEQ pd3dDevice, float fElapsedTime );
    virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
    HRESULT FinalCleanup ();

public:
    void UiRegisterNpcActionableControl( CUIControl* pControl );

public:
	virtual void SetMiniMapInfo(const GLLevelFileClient& levFile, CString strMapName );
	void SetFirstVNGainSysCall ( bool bFirstCall ) { m_bFirstVNGainSysCall = bFirstCall; }


    public:

		//std::vector<CMaxChatWindow*>             GetMaxChatWindow(int nIndex)    { return m_pMaxChatWindow[nIndex]; }
		CMaxChatWindow*			GetMaxChatWindow(int nIndex)    { return m_pMaxChatWindow[nIndex]; }
        CChatGroupWindow*			GetGroupChatWindow()            { return m_pChatGroupWindow;}
        IMiniMap*						GetMiniMap()						{ return m_pMiniMap; }
		IDPSWindow*					GetDPSWindow(void)				{ return m_pDPSWindow; }
        IKeySettingWindow*	        GetKeySettingWindow()			{ return m_pKeySettingWindow; }
        IChatMacroWindow*		    GetChatMacroWindow()			{ return m_pChatMacroWindow; }
        INameDisplayMan*  		    GetDispName()						{ return m_pNameDisplayMan; }
        IPrivateMarketShowMan*	GetPMarketShow()				{ return m_pPrivateMarketShowMan; }
        ISkillWindowToTray*		    GetSkillWindowToTray()			{ return m_pSkillWindowToTray; }
        IPartyWindow* 			    GetPartyWindow()				{ return m_pPartyWindow; }
		IPartyWindowRenewal*		GetPartyWindowRenewal()		{ return m_pPartyWindowRenewal; }
		IPartyYesNoWindow*		GetPartyYesNoWindow()		{ return m_pPartyYesNoWindow; }
		IExpeditionInviteWindow*	GetExpeditionInviteWindow()	{ return m_pExpeditionInviteWindow; }
		IExpeditionCheckReadyWindow*	GetExpeditionCheckReadyWindow()	{ return m_pExpeditionCheckReadyWindow; }		
		IPartyStateWindow*			GetPartyStateWindow()			{ return m_pPartyStateWindow; }
		IPartyTenderWindow*		GetPartyTenderWindow()		{ return m_pPartyTenderWindow; }
		IPartyTenderNotifyButton*		GetPartyTenderNotifyButton()		{ return m_pPartyTenderNotifyButton; }
		IPartyDistributionWindow*	GetPartyDistributionWindow()	{ return m_pPartyDistributionWindow; }
		IPartyDistributionListWindow*	GetPartyDistributionListWindow()	{ return m_pPartyDistributionListWindow; }
		IMiniExpeditionWindow*		GetMiniExpeditionWindow()		{ return m_pMiniExpeditionWindow; }
        IBasicChatWindow*   	    GetBasicChatWindow()			{ return m_pBasicChat; }		
        IChatOptionWindow*		    GetChatOptionWindow()		{ return m_pChatOptionWindow; }
        IChatTypeList*	    	    GetChatTypeList()   				{ return m_pChatTypeList; }
        ITradeWindow* 			    GetTradeWindow()				{ return m_pTradeWindow; }
        ITradeInventoryWindow*	GetTradeInventoryWindow()	{ return m_pTradeInventoryWindow; }
        IPtoPWindow*  			    GetPtoPWindow()					{ return m_pPtoPWindow; }
        IInventoryWindow* 		    GetInventoryWindow()			{ return m_pInventoryWindow; }
		IInventory*						GetInventory()						{ return m_pInventory; }
        IItemMove*						GetItemMove()					{ return m_pItemMove; }
        IMarketWindow*			    GetMarketWindow()				{ return m_pMarketWindow; }
        IHeadChatDisplayMan*		GetHeadChatDisplayMan()		{ return m_pHeadChatDisplayMan; }
        IConftModalWindow*		    GetConftModalWindow()		{ return m_pConftModalWindow; }
        IPartyModalWindow*		    GetPartyModalWindow()			{ return m_pPartyModalWindow; }
		IChatGroupModalWindow*	GetChatGroupModalWindow()	{ return m_pChatGroupModalWindow; }
        IConftDisplayMan* 		    GetConftDisplayMan()			{ return m_pConftDisplayMan; }
        IQuestWindowRenewal*		GetQuestWindowRenewal()		{ return m_pQuestWindowRenewal; }
        IFriendWindow*			    GetFriendWindow()				{ return m_pFriendWindow; }
		IChatGroupInviteWindow*	GetChatGroupInviteWindow()	{ return m_pChatGroupInviteWindow; }
        IModalWindowInner*		    GetModalWindow()				{ return m_pModalWindow; }
        IShopItemSearchWindow*	GetShopItemSearchWindow()	{ return m_pShopItemSearchWindow; }
		IItemSearchResultWindow*	GetItemSearchResultWindow()	{ return m_pItemSearchResultWindow; }
        IMapRequireCheck* 		    GetRequireCheck()				{ return m_pMapRequireCheckWindow; }
        IPostBoxWindow*		        GetPostBoxWindow();
        IPostNotifyButton*			GetPostNotifyButton()			{ return m_pPostNotifyButton; }
        IQuestHelper*					GetQuestHelper()					{ return m_pQuestHelper; }
        ITutorialDialogue*				GetTutorialDialogue();
        IBalloonText*					GetBalloonText()					{ return m_pBalloonText; }
        ISkillWindow*					GetSkillWindow();
        IBasicInfoView*			    GetBasicInfoView()				{ return m_pBasicInfoView; }
        IExplainInterface*		    GetExplainInterface();
        IUILeftTopGroup*				GetUILeftTopGroup()				{ return m_pUILeftTopGroup; }
        ISkillTrayTab*					GetSkillTrayTab()					{ return m_pSkillTrayTab; }
        IBasicQuickSkillSlot*			GetBasicQuickSkillSlot()			{ return m_pBasicQuickSkillSlot; }
		ITargetInfoDisplay*		    GetTargetInfoDisplay()			{ return m_pTargetInfoDisplay; }
        IBalloonSpaceBar*			GetBalloonSpaceBar()			{ return m_pBalloonSpaceBar; }
        ITutorialViewStep*		    GetTutorialViewStep();
        ILunchBoxTimeDisplay*		GetLunchBoxTimeDesplay()		{ return m_pLunchBoxTimeDisplay; }
        ISystemBuffTimeDisplay*	GetSystemBuffTimeDisplay()	{ return m_pSystemBuffTimeDisplay; }
        IVehicleWindow*				GetVehicleWindow()				{ return m_pVehicleWindow; }
        IPointShopWindow*			GetPointShopWindow()			{ return m_pPointShopWindow; }
        IProductProgress*			GetProductProgress()			{ return m_pProductProgress; }
		IProductResetWindow*		GetProductResetWindow()		{ return m_pProductResetWindow; }
		IOddEvenWindow*			GetOddEvenWindow(void)		{ return m_pOddEvenWindow; }
		ITexasHoldemWindow*			GetTexasHoldemWindow(void)		{ return m_pTexasHoldemWindow; }
		ITexasHoldemWebWindow*		GetTexasHoldemWebWindow(void)		{ return m_pTexasHoldemWebWindow; }
		IItemRebuildRenewal*		GetItemRebuildRenewalWindow(void)	{ return m_pItemRebuildRenewal; }
		IColorProperty*				GetColorPropertyWindow()		{ return m_pColorProperty; }

		// Lotto System;
		ILottoMainUI*				GetLottoMainUI()					{ return m_pLottoMainUI; }
		ILottoBuyLottoUI*			GetLottoBuyLottoUI()				{ return m_pLottoBuyLottoUI; }
		ILottoBuyListInfo*			GetLottoBuyListInfo()				{ return m_pLottoBuyListInfo; }
		ILottoWinManListInfo*		GetLottoWinManListInfo()			{ return m_pLottoWinManListInfo; }
		ILottoDisplayUI*			GetLottoDisplayUI()					{ return m_pLottoDisplayUI; }
		ILottoNotifyButton*			GetLottoNotifyButton()				{ return m_pLottoNotifyButton; }
		IFlyCameraControlWindow*	GetFlyCameraControlWindow()			{ return m_pFlyCameraControl; }
		ISummonStateDisplayUI*		GetSummonStateDisplayUI()			{ return m_pSummonStateDisplayUI; }
		IFactionInfoWindow*			GetFactionInfoWindow()				{ return m_pFactionInfoWindow; }
		IWorldBattleButton*			GetWorldBattleButton()				{ return m_pWorldBattleButton; }

public:
    CItemPreviewWindow*     GetItemPreviewWindow()          { return m_pItemPreviewWindow; }
	CCostumeStatWindow*		GetCostumeStatWindow()			{ return m_pCostumeStatWindow; }

	//! Reserved Skill Animation
	virtual void ReservedSkillAnimationShow( const SNATIVEID& sSkillID );
	virtual void ReservedSkillAnimationHideAll();

	//! Key Bind
	virtual void ResetKeyBinding();

	//! Open/Close Window
	virtual void ToggleWindowEscMenu();
	virtual void ToggleWindowInventory();
	virtual void ToggleWindowCharacter();
	virtual void ToggleWindowSkill();
	virtual void ToggleWindowMiniGame(const DWORD dwMiniGameID = GAEAID_NULL) override;
	virtual void ToggleWindowParty();
	virtual void ToggleWindowYesNoParty(const DWORD nID = -1);
	virtual void ToggleWindowClub();
	virtual void ToggleWindowQuest();
	virtual void ToggleWindowFriend();
	virtual void ToggleWindowItemBank();
	virtual void ToggleWindowMiniMap();
	virtual void ToggleWindowDPS(void);
	virtual void ToggleWindowPointshop();
	virtual void ToggleWindowWebItemShop();
	virtual void ToggleWindowChatMacro();
	virtual void ToggleWindowSNS();
	virtual void ToggleWindowHelp();
	virtual void ToggleWindowQBox();
	virtual void ToggleWindowPet();
	virtual void ToggleWindowVehicle();
	virtual void ToggleWindowSchoolRecord();
	virtual void ToggleWindowBattleGround();
	virtual void ToggleWindowProduct();
	virtual void ToggleWindowProductReset(WORD wResetAbleNum, WORD wPosX, WORD wPosY);
	virtual void ToggleWindowDice();
	virtual void ToggleWindowTenderItem(const bool bShow);
	virtual void ToggleWindowMasterTransfer(const bool bShow);
	virtual void ToggleWindowExpeditionInvite(void);
	virtual void ToggleWindowExpeditionCheckReady(const bool bShow);
	virtual void ToggleWindowColorProperty(const bool bShow);
	virtual void ToggleWindowFlyCameraControl();

	virtual void SetitemGroupComposition(SSETITEM* pSetitem, STARGETID sTargetID = STARGETID());
	virtual void ResetSetitemUIComposition(void);
	virtual void OpenSetitemPreviewWindow();
	virtual void WaitBattleServerDialogueOpen( bool bOpen );

	virtual const bool CloseFocusWindow();
	virtual void CloseAllWindow();
	void CloseItemToolTipMulti( UIGUID ControlID );
	void CloseSkillToolTipMulti( UIGUID ControlID );
	void CloseSkillToolTipMultiAll();
	void CloseItemToolTipMultiAll();
	void CloseExpeditionInviteWindow();

	//! TexasHoldem
	virtual void OpenTexasHoldemWindow();
	virtual void CloseTexasHoldemWindow();

	virtual void RunTexasHoldemWindow();
	virtual void StopTexasHoldemWindow();
	virtual void SetTexasHoldemURL( const std::string& strURL, const std::string& strPOST );

	//! Quick Potion Slot
	virtual void QuickPotionSlotSetIndex(int nIndex);

    //! Character Window
    virtual void CharacterSetPhoneNumber(const CString& PhoneNumber);

    //! ItemSearch
    virtual void ItemSearchResultUpdate(SSEARCHITEMESULT* pSearchItemResult, DWORD dwUpdateNum, DWORD dwPageNum);

    //! Summon
    virtual void SummonSetCount(DWORD Count);
    virtual void SummonAddDisp(DWORD GlobalId);

    //! Explain
    virtual BOOL ExplainIsSlotBoxVisibleAtInven() const;
    void ExplainSetSlotBoxVisibleAtInven(BOOL State);
    virtual BOOL ExplainIsSlotBoxVisible(unsigned int Index) const;
    void ExplainSetSlotBoxVisible(unsigned int Index, BOOL Visible);

    // for tutorial UI. by luxes.
    virtual void SetTutorialDialogueEnable(bool bSet);
    virtual void SetTutorialDialogueIndex(DWORD Index);
    virtual void TutorialDialogueTextStepInc();
    virtual void TutorialDialogueSoundIndexSet(DWORD Index);
    virtual DWORD GetTutorialDialogueSoundIndex();
    virtual void SetTutorialDialogueExplainText(const CString& strText);
    virtual void OpenMouseTip();
    virtual void OpenTutorialDialogue();
    virtual void OpenTutorialDialogueExplainText();
    virtual void OpenTutorialInterfaceLeft();
    virtual void OpenTutorialInterfaceRight();
    virtual void OpenTutorialInterfaceUp();
    virtual void OpenTutorialInterfaceDown();
    virtual void OpenTutorialInterfaceZoomIn();
    virtual void OpenTutorialInterfaceZoomOut();
    virtual void OpenBalloonText();
    virtual void OpenBalloonText( UIRECT rcWindow );
    virtual void OpenTutorialViewStep();
    virtual void StatsResetWindowRecalculate();
    virtual void StatsResetWindowResetProperty(const WORD wReturnStats, const UI::EMSTATSRESETWINDOW_TYPE emType, const EMITEM_TYPE emItemType);
    virtual void SetTutorialDialogText(const CString& strText);
    virtual void SetTutorialViewStep(int Index);

    virtual void ItemShopAddShopIcon(DWORD GaeaId);
    virtual void ItemShopDelShopIcon(DWORD GaeaId);
    virtual void ItemShopSetRender(DWORD dwGaeaID);

    virtual void NameDisplaySetCount(DWORD Count);
    virtual void NameDisplayAddName(CROWREN &sDISP_NAME, const TCHAR * szOwnerName = NULL);
	virtual void NameDisplayShowCountryMark ( bool bShowCountry );

    //! NotifyWindow
    virtual void NotifyWindowSetNewAlarm();

    //! Mouse Tip
    virtual void MouseTipResizeControl(const UIRECT& rcParentOldPos, const UIRECT& rcParentNewPos);
    virtual const UIRECT& MouseTipGetGlobalPos() const;

	//! SkillTrayTab
	virtual int  SkilSlotTrayGetTab();
	virtual void SkilSlotTraySetTab( int nIndex );
	virtual void SkilSlotTraySelectSlot( int nIndex );
    virtual void SkilSlotTrayToggle ();
    virtual void SkilSlotTrayOpen ();
	
    //! Refer
    virtual void ReferCharWindowOpen( const std::string& strCharName );
	virtual std::string	 GetSelectTextName();
	virtual void SetVisibleReferCharacterAdditionalWindow( bool bVisible );

    //! ItemRepair ( 아이템 수리, 내구도 )
    virtual void ItemRepairWindowOpen( bool bOpen = true  );
    virtual void ItemRepairCardUse( WORD wPosX, WORD wPosY );

public:	
    virtual void GameMenuFlipRunButton(BOOL bRun);
    const UIRECT& GameMenuGetGlobalPos() const;
    virtual bool QBoxButtonGetEnable();
    virtual void QBoxButtonSetEnable(bool bQBoxEnable);
    virtual const UIRECT& QBoxButtonGetGlobalPos() const;

    virtual void ModalWindowSetModalData(const D3DXVECTOR3& vData);
    virtual void ModalWindowSetModalData(int nData1, int nData2);
    virtual void CostumColorChangeWindowSetRestrict(bool bRestrict);
    
    //! Post
    virtual const BOOL PostBoxWindowIsWindowOpenAble(const BOOL bPrint);
    virtual const BOOL PostBoxWindowIsWindowCloseAble(const BOOL bPrint=FALSE);
    virtual BOOL PostBoxWindowIsVisible() const;
    virtual void PostBoxWindowDoModalError(const std::string& strError);
    virtual void PostBoxWindowDoModalMsg(const std::string& strMsg);
    virtual void PostBoxWindowSEndPostFbResult();
    virtual void PostBoxWindowClose();
    virtual void PostBoxProcessReceiveComplete();
    virtual void PostBoxReceivePageUpdateItem();
    virtual void PostBoxHistoryPageUpdateList();

    //! SNS
    virtual void SNSWindowDoModalError(const std::string& strError);
    virtual void SNSWebFBWindowSetPageType(const DWORD dwType);
    virtual void SNSWebFBWindowSetAddFriendURL(const std::string& strURL);
    virtual void SNSFBPageClearEditWall();
    virtual void SNSFBPageClearEditPicture();
    virtual void SNSFBPageSetEnableWallButton(const BOOL bEnable);
    virtual void SNSFBPageSetEnablePicture(const BOOL bEnable);
    virtual void SNSTWPageClearEditTweet();
    virtual void SNSTWPageSetEnableTweet(const BOOL bEnable);
    virtual void SNSTWPageSetEnableFollower(const BOOL bEnable);	

public:   
    virtual void PrintMsgText(D3DCOLOR dwColor, const std::string& strText);
    virtual void PrintConsoleText(const std::string& strText);
    virtual void PrintMsgTextDlg(D3DCOLOR dwColor, const std::string& strText);
    virtual void PrintConsoleTextDlg(const std::string& strText);
    virtual void PrintMsgDlg(D3DCOLOR dwColor, const std::string& strText);
	virtual void PrintAddString( D3DCOLOR dwColor, const std::string& strText);

public:
	const char* MakeString ( const char* szFormat, ... );

public:
    //! Pet
    virtual void SetPetHungryItem( SNATIVEID sPetItemID );
    virtual void SetPetHungry();
    virtual void PetWindowSetSkillCombo();    
    virtual void PetSkinMixImageSetItemData(WORD wPosX, WORD wPosY, SNATIVEID sItemData, WORD wHoldPosX, WORD wHoldPosY);
    virtual void PetSatiationUpdate( int Satiation );

	virtual BOOL IsGateOpen() const { return m_bGateOpen; }
	virtual void SetGateOpen(BOOL bGateOpen) { m_bGateOpen = bGateOpen; }
	virtual void SetDamage( D3DXVECTOR3 vPos, int nDamage, DWORD dwDamageFlag, BOOL bAttack );	
	virtual void SetTargetInfoMouseOver( STARGETID sTargetID );
	virtual void SetTargetInfoSelect( STARGETID sTargetID );
	virtual void SetTargetInfo( STARGETID sTargetID, const UI::EMTARGETINFO_TYPE emTargetInfoType );
	virtual void ResetTargetInfoMouseOver();
	virtual void ResetTargetInfoSelect();
	virtual const bool IsVisibleTargetInfoMouseOver();

	virtual void SetFightBegin ( const int nIndex );
	virtual void SetFightEnd ( const int nResult );

	virtual void SetAcademyFightBegin ( const int& nLMARK, const int& nLNUMBER, const int& nRMARK, const int& nRNUMBER );
	virtual void SetAcademyUpdateNumber ( const int& nLNUMBER, const int& nRNUMBER );
	virtual void SetAcademyFightEnd ();

	virtual void SetBlockProgramFound( bool bFOUND )	{ m_bBlockProgramFound = bFOUND; }
	virtual bool IsBlockProgramFound()					{ return m_bBlockProgramFound; }

	// 모달 윈도우 락;
	virtual void SetBlockModalUI( bool bModal )	{ m_bBlockModalUI = bModal; }
	virtual bool IsBlockModalUI()					{ return m_bBlockModalUI; }

public:	
    virtual void UpdateWhenReceiveDamage() override;

	virtual void SetPtoPWindowOpen(std::tr1::shared_ptr<GLCharClient> pCharClient, DWORD dwGaeaID, GLClubClient* pMyClub) override;
	
    virtual void SetQuestWindowOpen ( DWORD dwQuestID );
    virtual const UIRECT& GetQuestWindowGlobalPos() const;
    virtual const UIRECT& GetQuestWindowRenewalGlobalPos() const;
    virtual void SetQuestWindowGlobalPos(const D3DXVECTOR2& vPos);
    virtual void SetQuestWindowRenewalGlobalPos(const D3DXVECTOR2& vPos);
    virtual void SetQuestWindowShowPage(int nPage, bool bMakeMsg = false);
    virtual void SetQuestWindowAutoSelectPage(DWORD dwQuestID);

	virtual void SetTradeWindowOpen ( CString szPlayerName, DWORD dwTargetID );
    virtual void SetTradeWindowClose ();

    // NPC actionable
    virtual void SetDialogueWindowOpen( const SNATIVEID sNPCID, const DWORD dwNPCGlobalID, GLCHARLOGIC* pCHAR );
    virtual void SetDialogueWindowClose();
    virtual void SetBusWindowOpen ( const SNATIVEID sNPCID, const DWORD dwNPCGlobalID, std::tr1::shared_ptr<SNpcTalk> spNpcTalk );
    virtual void SetBusWindowClose();
    virtual void SetStorageWindowNpcOpen ( const SNATIVEID sNPCID, const DWORD dwNPCGlobalID );
    virtual void SetStorageWindowCardOpen ();

	virtual void SetStorageItemToVehicleTapWindow(const std::vector<DWORD>& vehicleIDVec);
	virtual void SetVisibleCharacterAdditionalWindow( bool bVisible );

	virtual void SetStorageWindowClose ();
	virtual void SetClubStorageWindowOpen ( const SNATIVEID sNPCID, const DWORD dwNPCGlobalID );
	virtual void SetClubStorageWindowClose ();
	virtual void SetMarketWindowOpen ( const SNATIVEID sNPCID, const DWORD dwNPCGlobalID );
    virtual void SetMarketWindowClose ();
    virtual void SetItemRebuildWindowNpcOpen( const SNATIVEID sNPCID, const DWORD dwNPCGlobalID );
    virtual void SetItemRebuildWindowCardOpen();
    virtual void SetItemRebuildWindowClose();
	virtual void SetItemRebuildResetCheckBox();
	virtual void SetItemRebuildSetCheckBox(const SITEMCUSTOM& sItemCustom, const RandomOpt::FixOption& fixOption);
    virtual void CheckItemRebuildWindowClose(const bool bUseStamp, const RandomOpt::FixOption& fixOption);
    virtual void SetRebuildWindowPreInvenItem( WORD wPosX, WORD wPosY );
	virtual void SetRebuildWindowPreStampItem( WORD wPosX, WORD wPosY );
    virtual void SetItemMixWindowOpen( const SNATIVEID sNPCID, const DWORD dwNPCGlobalID );
    virtual void SetItemMixWindowClose();    
    virtual void SetPostWindowClose();

    virtual SNATIVEID GetDialogueWindowNPCID();
    virtual DWORD     GetDialogueWindowNPCGlobID();
    virtual SNATIVEID GetBusWindowNPCID();
    virtual DWORD     GetBusWindowNPCGlobID();
    virtual SNATIVEID GetStorageWindowNPCID();
    virtual DWORD     GetStorageWindowNPCGlobID();
    virtual SNATIVEID GetClubStorageWindowNPCID();
    virtual DWORD     GetClubStorageWindowNPCGlobID();
    virtual SNATIVEID GetMarketWindowNPCID();
    virtual DWORD     GetMarketWindowNPCGlobID();
    virtual SNATIVEID GetItemRebuildWindowNPCID();
    virtual DWORD     GetItemRebuildWindowNPCGlobID();	
    virtual SNATIVEID GetItemMixWindowNPCID();
    virtual DWORD     GetItemMixWindowNPCGlobID();    

	virtual void SetStorageChargeOpen ( const WORD& wPosX, const WORD& wPosY );
	virtual void SetItemBankWindowOpen ();
	virtual void SetItemBankInfo ();
	virtual void SetVNGainSysWindowOpen ();
	virtual void SetVNGainSysInfo ();
	virtual void SetDefaultPosInterface(UIGUID ControlID);

    virtual void	SetItemShopSearchWindowOpen();
    virtual void	SetItemShopSearchWindowClose();

    virtual void	SetItemSearchResultWindowOpen();
    virtual void	SetItemSearchResultWindowClose();

	virtual void OpenItemGarbageWindow();	// 휴지통
	virtual void CloseItemGarbageWindow();
    virtual void CheckItemGarbageWindowClose();


	virtual void	SetItemMixResult( CString strMsg, bool bSuccess = false, bool bFail = false );

	virtual void	OPEN_TAXI_WINDOW( WORD wPosX, WORD wPosY );			// 택시 카드

	virtual void	SetPrivateMarketMake ();
	virtual void	SetPrivateMarketOpen ( const bool& bOPENER, const DWORD& dwGaeaID );
	virtual void	GetPrivateMarketInfo ( bool& bOPENER, DWORD& dwGaeaID );
	virtual void	SetPrivateMarketClose ();	

	virtual void	MODAL_PRIVATE_MARKET_SELLITEM ( const bool& bCOUNTABLE, const WORD& wPosX, const WORD& wPosY );

	virtual void	SetClubMake ( const DWORD& dwNpcGlobID );

	virtual void SetShotcutText ( DWORD nID, CString& strTemp );
	virtual void GetShotCutKey();
	virtual void GetChatMacro();
	virtual void AddChatMacro(int nIndex);
	virtual void ChatLog( bool bChatLogt, int nChatLogType );
	virtual CString GET_RECORD_CHAT();
	virtual CString GetdwKeyToString(int dwKey);

	//		채팅창 링크 관련	
	virtual void ChatToLink(const SLINK_DATA_BASIC& sLinkDataBasic, const SITEMCUSTOM *pItemCustom = NULL /*GroupChat에서는 SITEMCUSTOM을 사용한다*/);	

	virtual SLINK_DATA_INDEX* GetLinkBasicData ( int nLinkIndex );

	virtual bool	GetLinkData( const SLINK_DATA_BASIC& sLinkDataBasic, CString& strLinkName, DWORD& dwColor );		
	virtual DWORD	GetLinkColor( int nLinkIndex );
	virtual bool 	GetLinkName( CString& strLinkName, int nLinkIndex );


	//	링크 정보를 등록한다. 
	virtual int		AddLinkDataToList_Client ( const SLINK_DATA_BASIC& sLinkDataBasic );		//	클라이언트 정보로 등록
	virtual int		AddLinkDataToList ( const std::string& strCharName, const SLINK_TYPE& sLinkType, void* pBuffer, DWORD dwSize );
	virtual int		AddLinkDataToTypeList ( const std::string& strCharName, const SLINK_TYPE& sLinkType, void* pBuffer, DWORD dwSize );

	//	전송 데이터 변경
	virtual void	ConvertRecvData ( CString& strText, VEC_LINK_TEXT_DATA& vecLinkTextData );	
	virtual void	ConvertSendData ( CString& strInput, const VEC_LINK_TEXT_DATA_EX& vecLinkData, VEC_LINK_DATA_BASIC& vecLinkDataBasic );

	// 클라이언트 링크 데이터를 서버데이터로 변환 
	virtual void	ConvertLinkData ( const VEC_LINK_TEXT_DATA_EX& vecLinkDataIN, VEC_LINK_TEXT_DATA& vecLinkDataOut );

	virtual BOOL	GetLinkdataIndex(SLINK_DATA_ITEM&, int nIdx);
	virtual bool	LinkDataProcess( int nLinkIndex );	
	virtual void	SetPartyLinkOnce ( int nLinkIndex );
	virtual void	DeleteLinkData ( const SLINK_DATA_INDEX& sLinkIndex );

	virtual void SetPartyOption(const SPartyOption& PartyOption) override;	
	virtual void SetPartyInfo(const DWORD dwPartyID, const DWORD dwHashKey, const DWORD dwMasterID, const DWORD dwTimeKey) override;

	virtual void	SetArmSwap( BOOL bSwap );

	virtual void	AutoArmSwap ();
	virtual void	AutoSkillSolt ( bool bMain );

    virtual void    SetReviveSkill( bool bAct );

public:
	void	SetLottoOpen ( const DWORD& dwGaeaID );

    void    SetChatTypeListOpen();
    void    SetChatTypeListClose();
    void	SetChatOption(WORD wChatFlag);
    void	SetChatType(DWORD dwChatFlag);

public:
	virtual void DOMODAL_ADD_FRIEND_REQ( const CString& strName );
	virtual void DOMODAL_CLUB_JOIN_ASK( const DWORD& dwMasterID, const CString& strClubName, const CString& strClubMaster ) override;
	virtual void DOMODAL_UNION_JOIN_ASK( const DWORD& dwMasterID, const CString& strClubMaster ) override;
	virtual void DOMODAL_CLUB_BATTLE_ASK( const DWORD& dwMasterID, const CString& strClubMaster, DWORD dwBattleTime, bool bAlliance = false) override;
	virtual void DOMODAL_CLUB_BATTLE_ARMISTICE_ASK(DWORD dwClubID, DWORD ReqChaDbNum, const CString& strClubName, bool bAlliance = false) override;
	virtual void DOMODAL_CLUB_AUTHORITY_ASK( const CString& strClubName );

	virtual void OPEN_MODAL ( const CString& strText, int nModalTitle, int nModalType, UIGUID CallerID = NO_ID, BOOL bHide = FALSE );
	virtual void OPEN_MODALESS ( const CString& strText, int nModalTitle, int nModalType, UIGUID CallerID = NO_ID, BOOL bHide = FALSE );
	virtual void CLOSE_MODAL ( const UIGUID cID, bool bMakeMsg );

public:
	DWORD GetEventQuestID()							{ return m_dwEventQuestID; }
	void SetEventQuestID( DWORD dwQuestID )			{ m_dwEventQuestID = dwQuestID; }

public:
	void	WAITSERVER_DIALOGUE_OPEN ( const CString& strMessage, const int nAction, const float fTimer = fDEFAULT_WAITTIME_LEFT );
	void	WAITSERVER_DIALOGUE_CLOSE();

public:
	virtual void ClearNameList();

public:
	virtual void SetTradeWindowCloseReq();
	virtual void SetDuplicateLogin(const BOOL bDuplicateLogin)
	{
		m_bDuplicateLogin = bDuplicateLogin;
	}

private:
	HRESULT	TEXTURE_PRE_LOAD ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT	TEXTURE_PRE_UNLOAD ();

public:
    virtual BOOL IsVisible(UIGUID ControlID) { return UiIsVisibleGroup(ControlID); }
	virtual BOOL IsInventoryOpen();
	virtual BOOL IsBankOpen();
	virtual BOOL IsWishListOpen();
	virtual BOOL IsStorageWindowOpen()		 { return UiIsVisibleGroup( STORAGE_WINDOW ); }
	virtual BOOL IsClubStorageWindowOpen()	 { return UiIsVisibleGroup( CLUB_STORAGE_WINDOW ); }
	virtual BOOL IsMarketWindowOpen()		 { return UiIsVisibleGroup( MARKET_WINDOW ); }
    virtual void ResetCartListInDialogueWindowMarketPage( int nPageIndex, int nPosX, int nPosY );
    virtual void DialogueWindowLoadRebuyList();
    virtual void DialogueWindowCheckRebuySupplies( bool bCheck );
	virtual BOOL IsItemDecompose(void);
    void DialogueWindowSetRebuyCountText( int nCount );
    int GetDialogueWindowRebuyItemCount();
    void GetDialogueWindowMarketItemIndex( int* pnPosX, int* pnPosY );
    int GetDialogueWindowPageIndex();
    bool IsOpenDialogueWindowMarketPage();
    void SetEnableDialogueWindowMarketPage(bool bEnable);

	void UpdateExChangeList();

public:
    virtual bool IsOpenWindowFromNPC   ();
    virtual void CloseAllWindowFromNPC ();

public:
	// 인터페이스 사용시 캐릭터의 움직임을 제어
	BOOL IsOpenWindowToMoveBlock();

	BOOL IsSnapItem()								{ return m_bSnapItem; }
	void SetSnapItem()								{ m_bSnapItem = TRUE; }
	void ResetSnapItem()							{ m_bSnapItem = FALSE; }

	void SetFirstItemSlot()							{ m_bFirstItemSlot = false;	}
	void ResetFirstItemSlot()						{ m_bFirstItemSlot = true; }
	bool IsFirstItemSlot()							{ return m_bFirstItemSlot; }

	void SetFriendName( const CString& strName )	{ m_strMoveFriendName = strName; }
	const CString& GetFriendName() const			{ return m_strMoveFriendName; }

	virtual void SetIPBonusClass(int nClass);

public:
	virtual bool	SET_QUESTION_ITEM_ID ( int nID );

	virtual bool	SET_KEEP_QUESTION_ITEM_ID ( int nID );
	virtual void	RESET_KEEP_QUESTION_ITEM ();

	virtual void	BONUS_TIME_EVENT_START( bool bCharging ); // 이벤트 시작
	virtual void	BONUS_TIME_EVENT_END(); // 이벤트 종료
	virtual void	BONUS_TIME_BUSTER_START(); // 경험치 시작
	virtual void	BONUS_TIME_BUSTER_END(); // 경험치 종료

	// 베트남 탐닉 방지 시스템 게이지
	virtual void SET_VNGAINTYPE_GAUGE( int nPos, int nLimit );

	virtual bool SET_QUEST_HELPER( DWORD dwQuestID, bool bCheckPrintMsg = TRUE );
	virtual void RESET_QUEST_HELPER( DWORD dwQuestID );
    virtual bool IS_SAME_QUEST_HELPER( DWORD dwQuestID );
    virtual void QUEST_HELPER_UPDATE ();

public:
	virtual void SET_CONFT_CONFIRM_VISIBLE(bool bVisible);
	virtual void SET_CONFT_CONFIRM_PERCENT(float fPercent);

	virtual void SetSkillUpID(const DWORD& dwID) { m_dwSkillUpID = dwID; }
	virtual DWORD GetSkillUpID() { return m_dwSkillUpID; }    

private:
	void MoveBasicInfoWindow ();
	void BasicInfoViewDoubleClick();

private:
	const bool IsPartyMember(const char* const szName) const;

public:
    virtual void AUTO_LOAD_QUEST( DWORD dwID );
	virtual void REFRESH_QUEST_WINDOW();
    virtual void QUEST_HELPTER_TO_WINDOW( DWORD dwQuestID );
	virtual void REFRESH_FRIEND_LIST ();
	virtual void REFRESH_FRIEND_STATE();
	virtual void REFRESH_CLUB_LIST ();
	virtual void REFRESH_CLUB_STATE ();
	virtual void REFRESH_CLUB_NOTICE();
	virtual void REFRESH_CLUB_ALLIANCE_LIST();
	virtual void REFRESH_CLUB_BATTLE_LIST();
	virtual void REFRESH_ITEMBANK ();
	virtual void REFRESH_VNGAINSYSTEM ();
	virtual void REFRESH_ATTENDBOOK();
	virtual void SET_GATHER_GAUGE( DWORD dwGaeaID, float fTime );
    

public:
	virtual void SetInventorySlotViewSize(const int& nSLOT_VIEW_SIZE);
    const bool IsInventoryLocked ();

public:
    //! Friend
	void ADD_FRIEND_NAME_TO_EDITBOX( const CString& strName );
	void ADD_FRIEND( const CString& strName );
    void ADD_FRIEND_BLOCK( const CString& strName, bool bBlock );
	virtual void FRIEND_LIST(CONST CString & strName, bool bOnline);

	const CString& GetPrivateMarketTitle ();

	void DoBattleModal( const CString strClubName, bool bAlliance = false );
	BOOL ReqClubBattle( DWORD dwTime, bool bAlliance );

private:
	ET_INFOTYPE		m_etInfoType;

public:
    CItemToolTipGroup* GetInvenItemTooltipGroup();
    CItemToolTipGroup* GetWishItemTooltipGroup();
    CItemToolTipGroup* GetMarketWishItemTooltipGroup();

    void	RESET_INFO ();

	void SHOW_ITEM_INFO_SIMPLE(const SITEMCUSTOM& sItemCustom);
	void SHOW_ITEM_INFO(
        const SITEMCUSTOM& sItemCustom,
        BOOL bShopOpen,
        BOOL bInMarket,
        BOOL bInPrivateMarket,
        BOOL bPreviewUse,
		BOOL bCompare,
        WORD wPosX,
        WORD wPosY,
        SNATIVEID sNpcNativeID = SNATIVEID(),
		int  iPageIndex = -1);

	void SHOW_ITEM_INFO_DURABILITY(
		const SITEMCUSTOM& sItemCustom,
		BOOL bShopOpen,
		BOOL bInMarket,
		BOOL bInPrivateMarket,
		BOOL bPreviewUse,
		BOOL bCompare,
		WORD wPosX,
		WORD wPosY,
		SNATIVEID sNpcNativeID = SNATIVEID(),
		int  iPageIndex = -1);

	void    SHOW_ITEM_INFO( WishList::ItemSPtr pWishItem );
	void	SHOW_SKILL_INFO ( SNATIVEID sNativeID, const BOOL bNextLevel, const WORD wAddTempLevel = 0 );

    void	ShowSkillTooltip( const SNATIVEID& sNativeID, const BOOL bNextLevel, const WORD wAddTempLevel = 0 );
	void	ShowSkillToolTipMulti(const SNATIVEID& sNativeID, WORD wLevel );
	void	ShowSkillToolTipMulti( GLCHARLOGIC* pTarget, SNATIVEID sNativeID, int x, int y, BOOL bMultiSkill = FALSE );

	void	SHOW_COMMON_LINEINFO ( const CString& strText, D3DCOLOR dwColor, int nAlignFlag=TEXT_ALIGN_CENTER_X );
	void	SHOW_LINK_ITEM_INFO ( SITEMCUSTOM & sItemCustom );
	void	SHOW_PARTY_LINK_WINDOW ( int nLinkIndex, const GLPARTY_LINK& sPartyLink );

	bool	BEGIN_COMMON_LINEINFO_MULTI ( int nTextAlignFlag=TEXT_ALIGN_CENTER_X );
	bool	ADD_COMMON_LINEINFO_MULTI ( const CString& strText, D3DCOLOR dwColor );
	void	END_COMMON_LINEINFO_MULTI ();
	void	MultiLineCommentUtil( const char* pText, CD3DFontPar* pFont, float fWidth, int nTextAlign, DWORD nColor );

	virtual void WARNING_MSG_ON() { UiShowGroupBottom( WARNING_MSG_WINDOW ); }
	virtual void WARNING_MSG_OFF() { UiHideGroup( WARNING_MSG_WINDOW ); }

	virtual void SET_COUNT_MSG(INT nCount);

public:
	void	CLEAR_TEXT ( int nAlignFlag=TEXT_ALIGN_CENTER_X );	
	void	ADDTEXT_NOSPLIT ( const CString& strText, const D3DCOLOR& dwColor );
	void	ADDTEXT_LONGESTLINE_SPLIT ( const CString& strText, const D3DCOLOR& dwColor );

public:
	void	ModalMsgProcess ( UIGUID nCallerID, DWORD dwMsg );	
    void	MsgProcess ( LPVOID msgBuffer );

public:
	const	D3DXVECTOR3&	GetCharDir () const				{ return m_vCharDir; }

public:
	void	SetAniPic(int nPic);
	void	SetOddEven(BOOL bOdd);

public:	// FlyCamera
	const D3DXVECTOR3& GetFlyCameraUILastInputPos(void);

public:
	virtual void DisplayChatMessage(int nType, const char *szName, const char *szMsg);
    virtual void DisplayChatLinkMessage(LPVOID msgBuffer);
    virtual void DisplayChatPartyPickupLinkMessage(const CString& strName, SITEMCUSTOM& sItemCustom);
	virtual void UpdateClubGuidBattleTime( const std::vector<float> &vecTime );
    virtual void UpdateClubDeathMatchTime( const float fTime );
	virtual void ClearItemBank();
	virtual void ClearVNGainSys();
    virtual void DisableMinimapTarget();
    virtual void SetMapCheckType(UI::EMMAPCHECK_TYPE emMapCheckType);

private:
    void	UpdateStatus ();
    void	UpdateFocusOrder ();
	void	UpdateShortcutBefore ();
	void	UpdateShortcutAfter ();

private:
	void	UpdateStateQuestAlarm ();
	void	UpdateStateSimpleHP ();
	void	UpdateSimpleMessage ();
public:
	void	ReqToggleRun ();
	bool	ItemShopAuth ();		// 일본 커스텀 브라우저 인증 모듈 //	ItemShopAuth
	void	ItemShopVisible();		// 일본 커스텀 브라우저 로딩 대기 //	ItemShopAuth

	virtual void VisibleCDMRanking(bool bVisible);
	virtual void RefreashCDMRanking();
	virtual void VisibleCDMSafeTime(bool bVisible, float fCDM_SAFE_TIME);
	virtual void CalcSkillDisplayPos(bool bVisible);

	//#ifdef CH_PARAM // 중국 인터페이스 변경
	//public:
	//	void UpdatePotionTrayPosition();
	//#endif

public:	//	ETC Func.
	SCONFT_OPTION*	GetConftOption ()			{ return &m_sReqConflictOption; }
	SPartyOption*		GetPartyOption ()			{ return &m_sReqPartyOption; }

public:
	bool IsCHANNEL() { return m_bCHANNEL; }
	virtual bool IsCHAT_BEGIN();

private:
	BOOL			m_bDuplicateLogin;
	BOOL			m_bGateOpen;
	bool			m_bBlockProgramFound;
	int				m_nINFO_DISPLAY_TYPE;
	DWORD			m_dwEventQuestStep;
	DWORD			m_dwEventQuestID;
	EMCONFT_TYPE	m_emConflictReqType;
	DWORD			m_ConflictReqID;
	SCONFT_OPTION	m_sReqConflictOption;
	SPartyOption		m_sReqPartyOption;
	BOOL			m_bSnapItem;
	CString			m_strFriendName;
	D3DXVECTOR3		m_vCharDir;
	DWORD			m_dwSkillUpID;
	bool			m_bUSING_INFO_DISPLAY;
	bool			m_bACADEMY_FIGHT_READY;
	CString			m_strGeneralInfoBack;
	float			m_fMoveWindowDistBack;
	bool			m_bFirstItemSlot;
	DWORD			m_dwClubMasterID;
    DWORD           m_ReqChaDbNum;
	CString			m_strMoveFriendName;
	bool			m_bCHANNEL;
	/*bool			m_bArmsSwapReserve;*/
	/*float			m_fVehicleDelay;*/
	float			m_fItemBankDelay;
	bool			m_bItemShopLoad;	//	ItemShopAuth
    bool            m_bInitUpdateOrder;
	bool            m_bBlockModalUI;	// modalUI;

	MAP_LINK_DATA_INDEX			m_mapLinkData;	
	MAP_ITEM_LINK_DATA_INDEX	m_mapItemLinkData;
	MAP_PARTY_LINK_DATA_INDEX	m_mapPartyLinkData;

	int	m_dwLinkDataIndex;
	int	m_dwItemLinkDataIndex;
	int	m_dwPartyLinkDataIndex;


//---------------------------------------------------------------------------------------------------------------------------
// ( 이전의 NS_ITEMINFO )

public:
	void INFO_DISPLAY_ITEM_RESET ();
	void INFO_DISPLAY_ITEM_LOAD(
        const SITEMCUSTOM& sItemCustom,
        const BOOL bShopOpen,
        const BOOL bInMarket,
        const BOOL bInPrivateMarket,
        const BOOL bUsePreview,
        const BOOL bIsWEAR_ITEM,
        WORD wPosX, 
        WORD wPosY,
        SNATIVEID sNpcNativeID,
		int  iPageIndex = -1 );
	void INFO_DISPLAY_ITEM_LOAD_SIMPLE ( const SITEMCUSTOM &sItemCustom );

    bool DisplayItemInformation(
        const SITEMCUSTOM& sItemCustom,
        const BOOL bShopOpen,
        const BOOL bInMarket,
        const BOOL bInPrivateMarket,
        const BOOL bUsePreview,
        const BOOL bIsWEAR_ITEM,
        WORD wPosX, 
        WORD wPosY,
        SNATIVEID sNpcNativeID,
		int  iPageIndex = -1 );

	void DisplayItemInformationLink(
		const SITEMCUSTOM& sItemCustom,
		const BOOL bShopOpen,
		const BOOL bInMarket,
		const BOOL bInPrivateMarket,
		const BOOL bUsePreview,
		const BOOL bIsWEAR_ITEM,
		WORD wPosX, 
		WORD wPosY,
		SNATIVEID sNpcNativeID );

	void DisplayWishListItemInfo( WishList::ItemSPtr pWishItem );

	bool DisplayItemInformation_Durability(
		const SITEMCUSTOM& sItemCustom,
		const BOOL bShopOpen,
		const BOOL bInMarket,
		const BOOL bInPrivateMarket,
		const BOOL bUsePreview,
		const BOOL bIsWEAR_ITEM,
		const BOOL bIsDurable,
		WORD wPosX, 
		WORD wPosY,
		SNATIVEID sNpcNativeID,
		int  iPageIndex = -1 );

private:

	void		INFO_DISPLAY_ITEM_AddTextNoSplit ( CString strText, D3DCOLOR dwColor );
	void		INFO_DISPLAY_ITEM_AddTextLongestLineSplit ( CString strText, D3DCOLOR dwColor );
	void		INFO_DISPLAY_ITEM_RemoveAllInfo ();
	CString		INFO_DISPLAY_ITEM_GetNumberWithSign ( int nValue );
	void		INFO_DISPLAY_ITEM_AddInfoItemAddon  ( int nBasic, int nAddon, CString strFormat );
	void		INFO_DISPLAY_ITEM_AddInfoItemAddon  ( int nBasic, int nAddon, BYTE uGRADE, CString strFormat );
	void 		INFO_DISPLAY_ITEM_AddInfoItemAddonRange ( int nBasicMin, int nBasicMax, int nAddon, BYTE uGRADE, CString strFormat );
	void 		INFO_DISPLAY_ITEM_APPEND_ITEM_GRADE ( CString& strOrigin, BYTE uGRADE );
	void 		INFO_DISPLAY_ITEM_AddItemTurnInfo ( const SITEMCUSTOM &sItemCustom, const BOOL bInMarket, const BOOL bInPrivateMarket );
	void 		INFO_DISPLAY_ITEM_AddInfoPetSkillItem( const SITEMCUSTOM &sItemCustom );
	void 		INFO_DISPLAY_ITEM_AddInfoSkillItem ( const SITEMCUSTOM &sItemCustom );
	void 		INFO_DISPLAY_ITEM_AddTextSpecialVaule( const SITEMCUSTOM & sItemCustom, const SITEM* pItemData );
	void 		INFO_DISPLAY_ITEM_AddTextAddValue( const SITEMCUSTOM & sItemCustom, const ITEM::SSUIT & sSUIT );
	void 		INFO_DISPLAY_ITEM_AddInfoLunchBoxItem ( const SITEMCUSTOM &sItemCustom );
	void 		INFO_DISPLAY_ITEM_AddInfoPetDualSkillItem ( const SITEMCUSTOM &sItemCustom );
	
	SITEMCUSTOM	m_INFO_DISPLAY_ITEM_sItemCustomBACK;
	BOOL		m_INFO_DISPLAY_ITEM_bShopOpenBACK;
	BOOL		m_INFO_DISPLAY_ITEM_bInMarketBACK;
    BOOL		m_INFO_DISPLAY_ITEM_bInPrivateMarketBACK;
    BOOL		m_INFO_DISPLAY_ITEM_bUsePreviewBACK;
	BOOL		m_INFO_DISPLAY_ITEM_bIsWEAR_ITEMBACK;
	BOOL		m_INFO_DISPLAY_ITEM_bControlBACK;
	WORD		m_INFO_DISPLAY_ITEM_wPosXBACK;
	WORD		m_INFO_DISPLAY_ITEM_wPosYBACK;

//---------------------------------------------------------------------------------------------------------------------------
// ( 이전의 NS_SKILLINFO )

public:
	void		INFO_DISPLAY_SKILL_RESET ();
	void		INFO_DISPLAY_SKILL_LOAD ( const SNATIVEID& sNativeID, const BOOL bNextLevel );
	void		INFO_DISPLAY_SKILL_LOAD ( GLSKILL* const pSkill, SCHARSKILL* const pCharSkill, BOOL bNextLevel );
private:
	void		INFO_DISPLAY_SKILL_AddTextNoSplit ( CString strText, D3DCOLOR dwColor );
	void		INFO_DISPLAY_SKILL_AddTextLongestLineSplit ( CString strText, D3DCOLOR dwColor );
	void		INFO_DISPLAY_SKILL_RemoveAllInfo ();

	SNATIVEID	m_INFO_DISPLAY_SKILL_sNativeID_Previous;
	BOOL		m_INFO_DISPLAY_SKILL_bNextLevel_Previous;
    WORD        m_INFO_DISPLAY_SKILL_wLevel_Previous;
public:
    virtual void DoModal (const std::string& strText, int nModalTitle = UI::MODAL_NOTITLE, int nModalType = UI::OK, UIGUID CallerID = NO_ID, BOOL bHide = FALSE);
    virtual void DoModalTipBox (const std::string& strText, const std::string& strTip, int nModalTitle = UI::MODAL_NOTITLE, int nModalType = UI::OK, UIGUID CallerID = NO_ID, BOOL bHide = FALSE);
	virtual void DoConftModal (const std::string& strText, UIGUID CallerID, CString strMyName, CString strHisName );
	virtual void DoPartyModal (const std::string& strText, UIGUID CallerID, bool bExpedition);
    virtual void DoPartyModal (const std::string& strText, UIGUID CallerID, bool bExpedition, DWORD dwCharDbNum);
	virtual void DoPartyModal (const std::string& strText, UIGUID CallerID, const SPartyOption& PartyOption, const bool bExpedition = false);
	virtual void DoChatGroupModal (const std::string& strText, UIGUID CallerID, bool bExpedition, DWORD dwkey);

    //! Minimap
    virtual void SetVisibleMiniMapClubGuidBattleTime( const BOOL bVisible );
    virtual void SetVisibleMiniMapClubDeathMatchTime( const BOOL bVisible );
    virtual void SetClubGuidBattleName( const std::vector<std::string> &vecName );
    virtual bool MinimapTargetIsEnable() const;
    void MinimapTargetSetEnable(bool State);
    const UIRECT& MinimapTargetGetGlobalPos() const;
    void MinimapTargetSetGlobalPos(const UIRECT& Pos);
    void MinimapTargetSetGlobalPos(const D3DXVECTOR2& vPos);
    void MinimapTargetSetLocalPos(const UIRECT& rcLocalPosNew);
    void MinimapTargetSetLocalPos(const D3DXVECTOR2& vPos);

    //! ProgressDisplay
    virtual void ProgressDisplayShow ( const UI::EMPROGRESSDISPLAY_TYPE emType, const std::string& strText );
	virtual void ProgressDisplayShowPulblic ( const std::string& strText );

    //! StudentRecord
    virtual const bool  IsStudentRecordOpenable ();
    virtual void        UpdateActivityWindow();
    virtual void        UpdateActivityPoint();
    virtual const bool  IsActivityProgressDisplay();
    virtual void        UpdateAttendance();
    virtual void        UpdateAttendanceTask();
	virtual void		UnlockAttendanceUI();
    virtual void        UpdateStudentRecordNotifyButton();
    virtual void        OpenAttendanceWindow();
	//! StudentRecord RnAttendance
	virtual void		RnAttendanceSetDayInfoText( const CString strEventText, const D3DCOLOR& TextColor = NS_UITEXTUTIL::ENABLE );
	virtual void		RnAttendanceClerText();
	virtual void		REFRESH_RnattendancePage();

    //! Character
    virtual void UpdateActivityBadge();

    //! Skill Window
    virtual void SkillWindowSetVisiblePageForTutorial();
    virtual void REFRESH_SKILLWINDOW();

	//! Skill Window To Tray
	virtual SNATIVEID SkillWindowToTrayGetSkill();
	virtual void	  SkillWindowToTrayResetSkill();

    //! TargetInfo
    virtual const CString TargetInfoGetMouseOverName() const;

    //! ItemPreview Window
    virtual void        ItemPreviewWindowOpen           ( const SITEMCUSTOM& sItemCustom );
    virtual const BOOL  ItemPreviewWindowIsPreviewItem  ( const SITEMCUSTOM& sItemCustom );
    virtual const DWORD ItemPreviewWindowItem_2_Slot    ( const SITEMCUSTOM& sItemCustom );

    //! Point Shop Window
    virtual void ReloadPointShopItemList();
    virtual void PointShopSetPoint( __int64 nCurPoint );
    virtual __int64 PointShopGetPriceInCart ();
    virtual void PointShopResetCartItem( SNATIVEID sItemID );
    virtual void SetForceClosePointShopWindow( bool _bForceClose );
    virtual bool IsForceClosePointShopWindow();

    //! Competition
	virtual void CompetitionNotifyButtonUpdate();
	virtual bool CompetitionWindowIsOpen();
	virtual void CompetitionWindowOpen();
	virtual void CompetitionWindowClose();
	virtual void CompetitionCTFUpdate();
    virtual void CompetitionNotifyButtonSetLock(const bool _bLock);
    virtual void CompetitionNotifyButtonClearAlarm();
    virtual void CompetitionNotifyButtonSetAlarm(const DWORD _dwMatchingNum, const char* szText);  
    virtual void CompetitionNotifyButtonSetTooltip(const DWORD _dwMatchingNum, const char* szText);

	virtual const UIRECT& GetCompetitionWindowPos();

	virtual const UIRECT& GetTournamentDashBoardWindowPos(UIGUID ControlID);
	virtual void OpenTournamentJoinWindow(bool _Open, int Time );
	virtual void OpenTournamentJoinWindow(bool _Open);

	void		 CompetitionTopRankingOpen( const PVP::ENUM_CTF_TYPE emType );
	void		 CompetitionTopRankingClose();
	void		 CompetitionTopRankingSetTextTime( const CString strText );

    //! Rebirth
    virtual void SetForceResurrect( const float _fTime);

    //! CTF
    virtual void InfoCTFDisplayUpdate();
	virtual void CTFResultWindowShow();
	virtual void CTFResultWindowUpdate();
    virtual void CheckCTFSpurt();

	// TODO : #rm 695
	void	ctf_display_hide();

	//! Product
	virtual void MsgProductStart( DWORD nID, WORD nCount );
	virtual void MsgProductGoAlong();
	virtual void MsgProductComplete();
	virtual void MsgProductCancel();
	virtual void MsgProductNeedConfirm();
    void ProductWindowSetSeletedTabID( UIGUID ControlID );
    void ProductWindowSetSeletedItem( SProductRecipe* pProductRecipe );

	//! Party Renewal; call by Popup Menu;
	void SetVisibleSkillEffectPartyStateWindow(const BOOL bChecked);
	const DWORD AddPartyYesNoComponent(const std::string& strDescription, PARTY_YESNO_FUNCTION PartyYesNoFunction);

	virtual void OpenPartyTenderNotifyButton( bool bShow );
	virtual void PartyTenderNotify( bool bIsNew );

	//! WishList
	virtual void MsgWishListInsert( BYTE nType );

    virtual void MsgMapSearchNameList(const std::vector<std::pair<int, std::string> >& nameList);
	virtual void MsgMapSearchNameDetailInfo(int level,const std::string& charName,const std::string& title,int school,int emClass,
		bool isParty,const std::string& clubName,DWORD dwGaeaID);
	virtual void	MsgMapSearchNamePosition(int level,const std::string& charName,DWORD mapID,const D3DXVECTOR3& pos);


	virtual void MsgMapPartyInfoListBoard(bool isExistMyBoard,DWORD mapID,
		const std::vector<std::string>& NameVec,const std::vector<DWORD>& MaxPlayerVec,
		const std::vector<DWORD>& CurrentPlayerVec,const std::vector<std::string>& TitleVec);

	virtual void MsgMapPartyInfoDetail(DWORD mapID,const std::vector<GLPARTY_LINK_MEM>& memberVec);

    //! ClubWindow
    void ShowClubwindowSetGradePopup();
    virtual void RefreshClubWindowGuidanceMap() override;
    virtual void ShowClubLogInClubWindow() override;
    virtual void ClubStorageUpdateMoneyInClubWindow() override;
    virtual void ClubUpdateNewbieNotice() override;

	//! 개인상점 검색시스템 ( PriviteMarket )
    virtual void PrivateMarketWindowOpen( bool bOpen = true );
	virtual void PrivatemarketRefreshSearchResult();
	virtual void SetPageWaiting(bool bIsWaiting);
	virtual bool IsPageWaiting();
	virtual void SetWhishPageEvent(bool bBool);

    virtual void PrivattMarketWindowRanPointPageRefresh();

    // PVE
    virtual void UpdatePVEMapTable();
    virtual void SetPVEEntranceConfirm(bool bShow);
    virtual void SetPVEEntranceState(int nCur, int nAll, int nSec, bool bShow);
    virtual void SetPVEStage(int nStage, bool bShow);
    virtual void SetPVEMessage(const TCHAR* pText, bool bShow);
    virtual void SetPVEMonsterIndicator(int nCur, int nAll, bool bShow);
    virtual void SetPVEPositioningIndicator(int nX, int nY, int nNow, int nNeed,  bool bShow);
	virtual void SetPVETimeIndicator( int nSec, bool bShow );
    virtual void SetPVEStageIndicator(int nStage, int nEnd, int nRange, int nSec, bool bBlank, bool bShow);
    virtual void SetPVEStageState(int nState, bool bShow);
    virtual void SetPVERetry(LONGLONG nMoney, int nTime, bool bShow);
    virtual void SetPVEStageResult();

	virtual void SetNewInstanceRetry ( LONGLONG nMoney, int nTime, bool bShow, InstanceSystem::InstanceMapID nInstanceID, SNATIVEID sKeyMapID );

    // --------------------- New Instance System ---------------------------------
    // Common UI
    virtual void PrintSimpleSystemMessage( const CString& strMessage,
        const D3DCOLOR dwMessageColor, const DWORD dwFontSize, const float fLifeTime );

    virtual void ClearSimpleSystemMessage();

    virtual void ShowSimpleTimerMessageBox(const NET_MSG_GENERIC* pMsg);
    virtual void CloseSimpleTimerMessageBox();

    //! Rn CDM
    virtual void RefreashCdmProgress( DWORD	dwNumberOneClubMarkVER,
        DWORD	dwNumberOneClubDbNum,
        char*	szNumberOneName,
        int	nNumberOneScore,
        int	nMyRank,
        DWORD	dwMyScore,
        int	nHour,
        int	nMin,
        int	nSecond );

    virtual void SetRankSloatData(InstanceSystem::RNCDM_RANK_SLOT vecRnCdmRankSlot);
    virtual InstanceSystem::RNCDM_RANK_SLOT GetRankSlotData(int nIndex);
    virtual void RankListUpdate();

    virtual void SetCLubMemberInfo(std::vector<InstanceSystem::CDM_MEMBER_INFO> vecCdmMemberInfo);
    virtual void SetTitleClubNameMemberNum( const CString strClubName, int nMembernum );
    virtual void SetRnCDMClubInfoPos(int nPosX, int nPosY);

    // 깃발전 깃발 가이드 화살표 이펙트
    virtual void SetFlagGuideArrow( const int _flagNum, const STARGETID& vPos, const bool bBlink );

	//Rn CDM CompetitionWindow 
	virtual void SetCDMMapinfo(std::string	strMapName,
		int nTimeLimit, 
		int nPlayerLimite, 
		int nLevelLimit );
	virtual void SetCDMMapImageName(std::string strMapImageName);
	virtual void SetCDMBattelState(int nStartTime, int nEndTime);
	virtual void OpenCDMTopRanking();
	virtual void SetCDMTopList(std::vector<DWORD> vecClubId,
								LONGLONG lnStartTime,
								LONGLONG lnEndTime,
								std::vector<DWORD> vecRank );
	virtual void SetClubMasterClubName(	std::vector<std::string> vecMasterName,
										std::vector<std::string> vecClubName );

    //Rn Competition ResultWindow
    virtual void ShowRnCompetitionRewardTooltop(const int emTitleType, const InstanceSystem::RnCompetitionResultContentRewardObj* pReward);
    virtual const bool msgProc_RnCompetitionResult(const NET_MSG_GENERIC* _pMessage);

    // Guidance Club Battle
    virtual void SetGuidanceClubBattle_UI_Auth(const bool bVisible, const float fAuthTime);
    virtual void SetGuidanceClubBattle_ProgressInfo(const NET_MSG_GENERIC* _pMessage);
    virtual void SetGuidanceClubBattle_RankInfo(const NET_MSG_GENERIC* _pMessage);
    virtual void SetGuidanceClubBattle_RankMyInfo(const NET_MSG_GENERIC* _pMessage);
    virtual void SetGuidanceClubBattle_AuthInfo(const NET_MSG_GENERIC* _pMessage);

	// CTI
	virtual void SetCTI_UI_Auth ( const bool bVisible, const float fAuthTime );
	virtual void SetCTI_ProgressInfo ( const NET_MSG_GENERIC* pMsg );
	virtual void SetCTI_RankInfo ( const NET_MSG_GENERIC* pMsg );
	virtual void SetCTI_RankMyInfo ( const NET_MSG_GENERIC* pMsg );
	virtual void SetCTI_AuthInfo ( const NET_MSG_GENERIC* pMsg );

    // Tournament;
	virtual void OpenTournamentDashboard(DWORD dwPageNum);
	virtual void OpenTournamentBettingWindow(DWORD dwKagNum,DWORD dwPageNum);
	virtual void OpenTournamentMoreInfoWindow(DWORD dwPageNum);
	virtual void OpenTournamentUI_HUD(bool isObserver);
	virtual void OpenTournamentWinLose(bool isWin);
	virtual void SetStageTimeIndicator( const int nSec, const bool bBlank, const bool bShow );
	virtual void SetEntrance(bool bShow,int waitentrance);
	virtual void SetEntrance(bool bShow);
	virtual int  SetInstancePopUp(MatchingSystem::InstancePopUpReceipt* receipt);
	virtual void SetMiniGroupWindowRight(DWORD _factionid);
	virtual void SetMiniGroupWindowLeft(DWORD _factionid);
	virtual void SetBigGroupWindowRight(DWORD _factionid);
	virtual void SetBigGroupWindowLeft(DWORD _factionid);
	virtual void HideAllTournamentWindow();
	

	virtual int GetIndicatorTime();
	virtual int SetIndicatorTime(int nSec, bool bBlank, bool bShow);
	virtual void SetTournamentIndicatorType( DWORD ObserverType , DWORD factionid1, DWORD factionid2);
    virtual void SetTournamentIndicatorInfo( const NET_MSG_GENERIC* const _pMessage );

	virtual void OpenCostumeStatWindow( const SINVENITEM* itemSrc = NULL, const SINVENITEM* itemDest = NULL);
	virtual void OpenSelectiveformBox( std::vector<ITEM::SBOX_ITEM>& vecItemID );
	virtual void ToggleJoinWorldBattleButton(bool bOn);

    //---------------------------------------------------------------------------
	    
    //! Message
    void MsgServerGeneralChat(NET_MSG_GENERIC* nmg);
    void MsgConfrontTarget(NET_MSG_GENERIC* nmg);
    void MsgWithVehicleTarget(NET_MSG_GENERIC* nmg);
    void MsgTradeTarget(NET_MSG_GENERIC* nmg);    
    void MsgReqTakeFB(NET_MSG_GENERIC* nmg);
    void MsgDropOutForced(NET_MSG_GENERIC* nmg);
    void MsgChatPrivateFail(NET_MSG_GENERIC* nmg);
    void MsgChatFB(NET_MSG_GENERIC* nmg);
    void MsgApexAnswer(NET_MSG_GENERIC* nmg);
    void MsgEventLotteryFB(NET_MSG_GENERIC* nmg);
    void MsgChatBlock(NET_MSG_GENERIC* nmg);

    //! Popup
    void OpenPopupMenu( popupmenu::SP_POPUP_MENU PopupMenu );

	//! ProgessTimer
	void ShowProgessTimer( UINT iTime,float fPosX, float fPosY, const char* szText, int nTextIndex );
	void HideProgessTimer();

	void ShowDoublePoint(BOOL bVisible);

    //! Modal
    void ModalCloseGame(DWORD dwMsg);
    void ModalInventoryMoneyToField(DWORD dwMsg);
    void ModalConfirmCustomerCenter(DWORD dwMsg );
    void ModalPartyTarget(DWORD dwMsg); 
    void ModalPartyRequest(DWORD dwMsg);
    void ModalPartyRequestByCharDbNum(DWORD dwMsg);
	void ModalChatGroupTarget(DWORD dwMsg); 
	void ModalChatGroupRequest(DWORD dwMsg);    
    void ModalTradeRequest(DWORD dwMsg); 
    void ModalVehicleWith(DWORD dwMsg); 
    void ModalReBirth(DWORD dwMsg); 
    void ModalRecoveryExp(DWORD dwMsg); 
    void ModalRecoveryNpcExp(DWORD dwMsg); 
    void ModalTradeMoney(DWORD dwMsg); 
	void ModalBettingMoney(DWORD dwMsg); 
    void ModalSaveMoney(DWORD dwMsg); 
    void ModalClubSaveMoney(DWORD dwMsg); 
    void ModalLoadMoney(DWORD dwMsg); 
    void ModalClubLoadMoney(DWORD dwMsg); 
    void ModalSplitItem(DWORD dwMsg); 
    void ModalStorageSplitItem(DWORD dwMsg); 
    void ModalClubStorageSplitItem(DWORD dwMsg); 
    void ModalConflict(DWORD dwMsg); 
    void ModalConflictRequest(DWORD dwMsg); 
    void ModalPileItem(DWORD dwMsg); 
    void ModalDlgPileItem(DWORD dwMsg); 
    void ModalServerStage(DWORD dwMsg); 
    void ModalBuySellItem(DWORD dwMsg, UIGUID nCallerID);
	void ModalExChangeBuySellItem(DWORD dwMsg, UIGUID nCallerID);
    void ModalDirectSellItem(DWORD dwMsg); 
    void ModalQuestGiveUp(DWORD dwMsg); 
    void ModalUnionKick(DWORD dwMsg);
	void ModalUnionWithdraw(DWORD dwMsg);
    void ModalUnionLeave(DWORD dwMsg);
    void ModalUnionBreakUp(DWORD dwMsg);
    void ModalCharRename(DWORD dwMsg);
    void ModalPetRename(DWORD dwMsg);
    void ModalResetSkillStats(DWORD dwMsg);
    void ModalItemRebuildMoney(DWORD dwMsg);
    void ModalItemRebuildOkQuestion(DWORD dwMsg);
    void ModalItemGarbageOkQuestion(DWORD dwMsg);
    void ModalHairColorInfo(DWORD dwMsg);
    void ModalPetColorInfo(DWORD dwMsg);
    void ModalMobileNumber(DWORD dwMsg);
    void ModalGenderChange(DWORD dwMsg);
    void ModalGenderChangeEnd(DWORD dwMsg);
    void ModalItemMixOkQuestion(DWORD dwMsg);
    void ModalSkillFactRemove(DWORD dwMsg);
    void ModalSkillFactLunchBoxOverWrite(DWORD dwMsg);
    void ModalVehicleColor(DWORD dwMsg);
    void ModalPostBoxPageReadButtonDelete(DWORD dwMsg);
    void ModalPostBoxPageReadButtonDeleteConfirmAttach(DWORD dwMsg);
    void ModalPostBoxPageReadButtonReturn(DWORD dwMsg);
    void ModalPostBoxPageReceiveButtonDeleteCheck(DWORD dwMsg);
    void ModalPostBoxPageReceiveButtonDeleteCommit(DWORD dwMsg);
    void ModalPostBoxPageReadButtonTake(DWORD dwMsg);
    void ModalPostBoxPageSendButtonSendCommision(DWORD dwMsg);
    void ModalReqHoldToField(DWORD dwMsg);
    void ModalSnsFacebookAddFriend(DWORD dwMsg);
    void ModalSnsTwitterFollow(DWORD dwMsg);
    void ModalCostumeColor(DWORD dwMsg);
    void ModalTutorialSkip(DWORD dwMsg);
    void ModalItemBankGetItem(DWORD dwMsg);
    void ModalSellItemFromInven(DWORD dwMsg);
    void ModalForceReBirthSkill(DWORD dwMsg);
    void ModalStorageConnectItemUseRequest(DWORD dwMsg);
    void ModalFriendAdd(DWORD dwMsg);
    void ModalFriendDel(DWORD dwMsg);
    void ModalFriendMove(DWORD dwMsg);
    void ModalAddFriendLureAns(DWORD dwMsg);
    void ModalDefaultPos(DWORD dwMsg);
    void ModalLotto(DWORD dwMsg);
    void ModalStorageCharge(DWORD dwMsg);
    void ModalPrivateMarketSell(DWORD dwMsg);
    void ModalPrivateMarketBuy(DWORD dwMsg);
    void ModalPrivateMarketPile(DWORD dwMsg);
	void ModalPrivateMarketOnlyUI(DWORD dwMsg);
    void ModalPrivateMarketQuit(DWORD dwMsg);
    void ModalClubKick(DWORD dwMsg);
    void ModalClubAuthority(DWORD dwMsg);
    void ModalClubAuthorityAsk(DWORD dwMsg);
    void ModalCDAuthorityToMember(DWORD dwMsg);
    void ModalCDAuthorityToMaster(DWORD dwMsg);
    void ModalClubBreakUp(DWORD dwMsg);
    void ModalClubChangeMark(DWORD dwMsg);
    void ModalClubNick(DWORD dwMsg);
    void ModalClubJoinAsk(DWORD dwMsg);
    void ModalClubJoin(DWORD dwMsg);
    void ModalClubBreakUpCancel(DWORD dwMsg);
    void ModalClubSelfQuit(DWORD dwMsg);
    void ModalSkillLevelUpConfirm(DWORD dwMsg);
    void ModalCommission(DWORD dwMsg);
    void ModalFriendBlockAddN(DWORD dwMsg);
    void ModalFriendBlockAddB(DWORD dwMsg);
    void ModalFriendBlockDel(DWORD dwMsg);
    void ModalUnionJoinAsk(DWORD dwMsg);
    void ModalUnionJoin(DWORD dwMsg);
    void ModalClubBattleAsk(DWORD dwMsg);
    void ModalAllianceBattleAsk(DWORD dwMsg);
    void ModalClubBattleArmistice(DWORD dwMsg);
    void ModalAllianceBattleArmistice(DWORD dwMsg);
    void ModalClubBattleArmisticeAsk(DWORD dwMsg);
    void ModalAllianceBattleArmisticeAsk(DWORD dwMsg);
    void ModalClubBattleSubmission(DWORD dwMsg);
    void ModalAllianceBattleSubmission(DWORD dwMsg);
	void ModalInventoryItemDecomposeOkQuestion(DWORD dwMsg);
	void ModalCostumeStatApply(DWORD dwMsg);
	void ModalCostumeStatBasic(DWORD dwMsg);
	void ModalCostuemStatApplied(DWORD dwMsg);
	void ModalConfirmChangeCountry(DWORD dwMsg);
	void ModalConfirmJoinPVPServer(DWORD dwMsg);
	void ModalCostumeStatRelease(DWORD dwMsg);
	void ModalSelectiveformBoxSelect(DWORD dwMsg);
	void ModalTempEnchant(DWORD dwMsg);
};
