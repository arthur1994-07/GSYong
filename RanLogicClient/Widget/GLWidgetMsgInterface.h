#pragma once

#include "./GLWidgetMsgInterfaceEnum.h"
#include "./GLWidgetScript.h"

// UI에서 서버로 메시지를 쏘는 함수는 GLWidgetMsgInterface 클래스를 통해 호출해준다.
// 스크립트 작업자가 실수를 하거나 유저가 커스터마이징을 하게 될 경우 서버로 메시지를
// 무분별하게 보낼 수 있기 때문에 각 함수 별로 일정 시간내에 몇 번까지 메시지를 보내게
// 할 것인지 설정해준다. cpp 파일에 정의된 ALLOWCALL_CHECK_VAL 또는 ALLOWCALL_CHECK
// 메크로 함수를 각 함수 상단에서 반드시 사용하도록 한다.

class GLWidgetMsgInterface
{
private :
	GLWidgetMsgInterface();
	GLWidgetMsgInterface( const GLWidgetMsgInterface& value );
	~GLWidgetMsgInterface();

public :
	static GLWidgetMsgInterface& GetInstance();

private :
	struct SMsgTimer
	{
		// 마지막 호출 시간
		float	fCallTime;
		// 시간내에 허용할 호출 카운트
		WORD	wAllowCallCount;

		SMsgTimer()
			: fCallTime( 0.0f )
			, wAllowCallCount( 0 )
		{
		}
	};

private :
	// 현재까지 흐른 시간
	float m_fElapsedTime;

	// 함수 호출 타이머
	SMsgTimer m_MsgTimer[ WidgetMsg_Size ];

public :
	void FrameMove( float fElapsedTime );

	bool ConnectLoginServer();
	bool ConnectServerChannel( int nServerIndex, int nServerGroup, int nChannelIndex );
	void Login( const char* strID, const char* strPW, const char* strRP, int nServerChannel );
	bool JoinGame( int nIndex );
	bool JoinGame2ndPassword( const char* szPassword );
	void DeleteChar( int nIndex, const char* strPassNum );
	int CreateChar( const char* strName );
	void ApplyStatPoint( WORD wPow, WORD wDex, WORD wSpi, WORD wStr, WORD wSta );
	void ReqChangeTitle( DWORD dwIndex, bool bSync );
	void ReqInvenItemSort();
	void ReqGarbageResult();
	void ReqMoneyToField( LONGLONG lnMoney );
	void ReqHoldToFieldForWidget( const char* szSlotID, int nHoldItemPlace, 
		WORD wSlotPosX, WORD wSlotPosY, float fPosX, float fPosY, float fPosZ );
	void SendChatMsg( DWORD dwType, const char* szName, const char* szMsg, LuaTable tbData );
	void SkillLevelUp( LuaTable tbSkillUpData );
	void RequestConflict( LuaTable tb );
	void AnswerConflict( bool bAns );
	void ReqInvenHairStyleChange( WORD wHairStyle );
	void ReqInvenFaceStyleChange( WORD wHairStyle );
	void ReqInvenGenderChange( WORD wFace, WORD wHair );
	void ReqBikePutOnWith();
	bool MoveToMap( DWORD dwMapID, int nPosX, int nPosY, bool bVehicle, bool bGMMove );
	void ReqToggleRun();
	void CommitVehicleColor( LuaTable tbColorList );
	void CommitCostumeColor( LuaTable tbColor );
	void RequestResetSkillStats( LuaTable tbStats, const WORD wMaxAmount, const WORD wItemType );
	bool TryRequestBankItemInfo();
	void ReqChargedItemTo(WORD wPosX, WORD wPosY);
	void ResurrectCheckPoint();
	void ResurrectExp();
	void ResurrectCurPos();
	void ResurrectSkill();
	void ReqPartyLinkJoin( DWORD dwMasterID, int nLinkIndex );
	void ReqQuestComplete( DWORD dwQUESTID );
	void ReqQuestGiveUp( DWORD dwQUESTID );
	void ReqInvenLunchBox(WORD wPosX, WORD wPosY);
	void ReqInvenRename( const char* szCharName );
	void ReqPMarketClose();
	bool ReqStorageSaveMoneyForWidget( LONGLONG lnMoney );
	bool ReqStorageDrawMoneyForWidget( LONGLONG lnMoney );
	void ReqStorageOpenCard(WORD wPosX, WORD wPosY);
	bool ReqGetStorageForWidget( DWORD dwChannel );
	void ReqStorageCard( WORD wPosX, WORD wPosY, WORD wSTORAGE );
	void ReqInvenDrug(WORD wPosX, WORD wPosY);
	void ReqStorageCloseCard();
	void ReqGateOut(const DWORD dwIndex);
	void ReqGuidCommissionForWidgt( float fRate );
	void ReqRecoveryExpNpc( DWORD dwNpcGlobID );
	void ReqClubNewForWidget( const char* szClubName );
	void ReqSlotChange();
	bool ReqStorageOpenFromInven();
	void ReqSkillHoldResetForWidget( DWORD dwID, WORD wSlot );
	void ReqSetVehicle( bool bActive );
	void ReqInvenSplit( WORD wPosX, WORD wPosY, WORD wSplitNum );
	void ReqPartyLinkLure( bool bLure, const DWORD dwHashKey, const DWORD dwTimeKey, const DWORD dwGaeaID );
	void ReqSkillRunSet( const WORD wSLOT );
	void SendCustomMessage ( const DWORD _dwDestType, const DWORD _dwInstanceDungeonID, const DWORD _dwParam1, const DWORD _dwParam2, const DWORD _dwParam3, const DWORD _dwParam4 );
	void ReqBeginMinigame(void);
	void ReqSelectOddEvenMinigame(const bool _bOdd);		
	void ReqRollMinigame(void);
	void ReqResultMinigame(void);
	void ReqRewardMinigame(void);
	void ReqSelectRewardMinigame(const unsigned int _indexReward);	
	void ReqReceiveRewardMinigame(void);
	void ReqCloseMinigame(void);
	void InstanceManRequestClose();
	void LureParty(DWORD dwGaeaID);
	void LureNameParty( const char* szName );
	void SecedeParty(DWORD dwIndex);
	void DissolveParty(void);
	void AuthorityParty(const DWORD dwIndex);
	void ChangeOptionParty( WORD wItemOp, WORD wMoneyOp, WORD wItemFilter );
	void ReqInviteParty();
	void ReqPartyInviteAns( bool bAccept, DWORD dwPartyID, DWORD dwHashKey, DWORD dwMasterID, DWORD dwTimeKey );
	bool SelectNpcTalk( DWORD dwAnswerID );
	void BuyItemNpcTalk( DWORD dwPageIndex, WORD wPosX, WORD wPosY, WORD wBuyNum );
	void SellItemNpcTalk( WORD wPosX, WORD wPosY );
	bool BuyCartItemNpcTalk(WORD wTabPage);
	bool RebuyNpcTalk( WORD wIndex );
	void BuyCartListPointShop();
	void ReqProductTypeAdd( WORD wProductType );
	void ReqProductTypeDelete( LuaTable tbProductList, WORD wPosX, WORD wPosY );
	void ReqProduct( DWORD nID, WORD nCount, bool bOngoing );
	void ReqProductCancel();
	void ReqProductTypeList();
	void SetActivatedSkillPet( LuaTable tbActivatedSkill );
	void ReqChangeStylePet( WORD wStyle );
	void CommitColorPet( DWORD dwColor );
	void ReqRenamePet( const char* szCharName );
	bool ReqTradeOpen();
	bool ReqTradeMoney( LONGLONG lnMoney );
	bool ReqTradeLock();
	bool ReqTradeAgree();
	bool ReqTradeCancel();
	void ReqTradeAns( bool bOK );
	void ReqFriendAdd(const char* szName);
	void ReqFriendGroupAdd( const char* szName );
	void ReqFriendGroupRename( WORD GroupNum, const char* NewName );
	void ReqFriendGroupDelete( WORD GroupNum );
	void ReqFriendMoveGroup( const char* szName, WORD GroupNum );
	void ReqFriendBlock(const char* szName, bool bBLOCK);
	void ReqFriendDel(const char* szDelName);
	void ReqFriendInviteParty( const char* szName );
	void ReqMoveToFriend( const char* szName );
	void ReqFriendLureAns(const char* szReqName, bool bOK);
	void SfReqFriendLureAns_throughAlarm(const char* szReqName, bool bOK, LuaTable tbMGuid);
	void ReqBuySearchItemPrivateMarket( LuaTable tbPurchase );
	bool SearchItemPrivateMarket( WORD wPage, LuaTable tbSearchInfo );
	bool RefreshLogPrivateMarket();
	void ReqPointTradeListPrivateMarket();
	bool ReqRegistItemPrivateMarket( int nPosX, int nPosY, WORD wCount, LONGLONG llItemPrice, 
		WORD wSellType, WORD wMoneyIndex, WORD wPeriodIndex );
	bool ReqUnregistItemPrivateMarket( int nSelectedIndex );
	void ReqPrivateMarketRegItemOpen( const char* cMarketName );
	void ReqPrivateMarketRegItem( WORD wPosX, WORD wPosY, LONGLONG llMoney, DWORD dwNum );
	void ReqPrivateMarketOpen( bool bOnlyUI );
	void ReqPrivateMarketClose();
	void ReqPMarketBuyItem( WORD wPosX, WORD wPosY, DWORD dwNum );
	bool ReqGroupChat( const char* szName, int nType );
	void ReqGroupChatFB( bool bAccept );
	void ReqQuitGroupChat( DWORD dwKey );
	void SendChatGroupChat( DWORD dwKey, const char* szMsg );
	void ReqKickGroupChat( const char* szName );
	void ReqSearchCharacter( const char* szName, bool bCurMap, DWORD dwCurMap );
	void ReqSearchCharacterDetailInfo( const char* szName );
	void ReqSearchCharacterPosition( const char* szName );
	bool ReqSetClubMemberGrade( DWORD dwDbNum, unsigned int nIndex );
	bool ReqClubNoticeChange( const char* szNotice );
	bool ReqGetClubStorage();
	bool ReqClubStorageDrawMoney( LONGLONG lnMoney );
	bool ReqClubStorageSaveMoney( LONGLONG lnMoney );
	bool ClubIncomeMoneyToStorageMoney();
	bool ReqClubNick( const char* szNickName );
	void ReqChangeGradeName( int nIndex, const char* szGradeName );
	void ReqAuthorityChange( LuaTable tbAuthData );
	bool ReqClubDissolution();
	bool ReqClubDissolutionCancel();
	bool ReqClubMarkChange( const char* szFileName );
	bool ReqClubSecede();
	bool ClubAutoKickReq( bool AutoKickUse, size_t GradeIndex, WORD Day );
	void DelClubMember(const char* _Name);
	void ClubMasterChange(int nDBNum);
	void ClubCDAuth( const char* _Name );
	void RequestClubJoin();
	bool ReqClubJoinAns(DWORD dwMaster, bool bOK);
	void ReqPVERetry(bool bOk);
	void ReqNewInstanceRetry(bool bOk);
	void ReqClubAuthorityAns(bool bOK);
	bool ReqClubAllianceAns(DWORD dwChiefCharID, bool bOK);
	bool ReqClubBattle( const char* szCharName, WORD wHour, WORD wMin );
	bool ReqAllianceBattleArmistice(DWORD dwCLUBID);
	bool ReqClubBattleArmistice(DWORD dwCLUBID);
	void ReqAllianceBattleArmisticeAns(DWORD dwCLUBID, DWORD ReqChaDbNum, bool bOK);
	void ReqClubBattleArmisticeAns(DWORD dwCLUBID, DWORD ReqChaDbNum, bool bOK);
	bool ReqAllianceBattleAns(DWORD dwChiefCharID, bool bOK);
	bool ReqAllianceBattle( const char* szCharName, WORD wHour, WORD wMin );
	bool ReqClubBattleAns(DWORD dwChiefCharID, bool bOK);
	bool ReqClubAlliance_PToPTarget();
	bool ReqClubAllianceDel_Targetname(const char* szClubName);
	bool ReqClubBattleArmistice_TargetName(const char* szClubName);
	bool ReqClubBattleSubmission_TargetName(const char* szClubName);
	bool ReqClubJoin(DWORD dwGaeaID);
	bool ReqClubJoinByCharname(const char* szCharName);
	void ReqReferChar( const char* szTarName );
	bool ReqReceivePageRefreshPost();
	void ReqSelectPostOpen( const unsigned int nSelect );
	void ReqSelectPostDelete( const unsigned int nSelect, bool bAttachDelete = true );
	void ReqSelectPostTake( const unsigned int nSelect );
	void ReqSelectPostReturn( const unsigned int nSelect );
	void ReqSelectPostTakeItem( const unsigned int nSelect );
	bool ReqSendPostTable( LuaTable tbPost );

	// PostItem 초기화
	void ResetCharPostItemData(void);


	bool ReqReceiveHistoryPost();
	bool ReqReceiveSendHistoryPost();
	bool ReqReceiveRecvHistoryPost();
	void ReqUseInvenPostBoxCardPost();
	bool ReqCDMRankInfo( DWORD dwScriptID );
	void ReqCTFJoin(void);
	void ReqCancelCompetition(void);
	void ReqExitCompetition(void);
	void OnEnteranceCompetition(const unsigned int nIndex);
	void OnEnteranceConfirmCompetition( const bool bEnterance );
	void OnClearOfBelongingCompetition(const unsigned int nIndex);
	void OnEnteranceTournament( const DWORD dwSelectID );
	bool ReqGetBettingInfoCompetition( const DWORD dwScriptID );
	bool ReqSetTournamentBettingMoney( const DWORD dwIndex, const LONGLONG llMoney, const DWORD dwScriptID );
	bool ReqCancelTournamentBettingMoney( const int nIndex );
	bool ReqObserveTournament(const DWORD dwScriptID, const char* _nameCharacterTargetObserve);
	bool ReqReferCharCompetition( const int nIndex, const DWORD dwScriptID );
	void ReqAttendanceTakeReward(const UINT nTaskIndex);
	void ReqRebuild_SelectCache(void);
	bool ReqChangeRebuildType(bool bKeepOrigin);
	void ReqRebuildItem_Fix(void);
	void ReqRebuildItem(void);
	void ReqRebuildItem_Cancel(void);
	void ReqRebuildItem_DirectFix(void);
	void ReqRebuildCloseWindow(void);
	void ReqPreWinNumLotto( unsigned int nID, unsigned int turnNum );
	void ReqPreWinManListLotto( unsigned int nID, unsigned int turnNum );
	void ReqPreAccumulateMoneyLotto( unsigned int nID, unsigned int turnNum );
	int ReqBuyLotto( unsigned int nID, LuaTable tbNum, bool bAuto );
	int ReqBuyLottoList( unsigned int nID, unsigned int nCount );
	void ReqPreBuyListLotto( unsigned int nID, unsigned int turnNum );
	void ReqConfirmBuyListLotto( unsigned int nID );
	int GetLottoSystemState ( unsigned int nID );
	bool ReqTaxiStation( int nSelectMap, int nSelectStop );
	void ReqCTFieldQuit(void);
	void ReqKeyFrameInfoFlyCamCtrl();
	bool ReqBusStation( const DWORD dwStationID );
	void ReqPVEExit();
	void ReqChangeCountry( WORD wContinentID, WORD wCountryID );
	void ReqJoiinWorldBattleServer();
	void ReqResetCountry( WORD wPosX, WORD wPosY );
	void ReqUseExpCapsuleItem ( WORD wInvenPositionX, WORD wInvenPositionY );
	void ReqUseExpCompressorItem ( WORD wInvenPositionX, WORD wInvenPositionY );
	void ReqUseUnlockCharacterSlotItem ( WORD wInvenPositionX, WORD wInvenPositionY );
	void ReqCostumeStatCToS();
	void ReqInvenItemDecompose( WORD wPosX, WORD wPosY );
	// 일시강화
	void ReqTLGrinding(WORD wPosX, WORD wPosY);
	void ReqItemRepair( LuaTable tbSlot );
	void ReqInvenHairColorChange( WORD wHairColor );
	void ReqGuidanceMapName();
	void ReqCostumeStatRelease( WORD wPosX, WORD wPosY, WORD wHoldPosX, WORD wHoldPosY );
	void NotifyReferCharPublic();
	void ReqItemBankInfo();
	bool WaitKeyAndTryLogin();
	void ReqPetSkin( DWORD dwItemID, WORD wPosX, WORD wPosY, WORD wHoldPosX, WORD wHoldPosY );
	void ReqRanMobileCreateMoney( const char* szNickName );
	void ReqRanMobileCreateCash( const char* szNickName );
	void ReqRanMobileExtendTime();
	void ReqRanMobileInfo();
	void ReqRanMobileSendJewel( WORD wCount );
	void ReqRanMobileGetDailyItem();
	void ReqRanMobileChangeItem( WORD wIndex );
	void ReqInvenClubRename( const char* szClubName );
	void ReqRanMobileInfoAll();
};