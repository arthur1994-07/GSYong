#include "pch.h"
#include "GLWidgetMsgInterface.h"

#include "../../RanLogic/Country/GLCountryDefine.h"

#include "../GLGaeaClient.h"
#include "../Login/GLLoginClient.h"
#include "../LobyCharacter/GLLobyCharClient.h"
#include "../LobyCharacter/GLLobyCharCreateClient.h"
#include "../Char/GLCharacter.h"
#include "../MiniGame/GLMiniGameManager.h"
#include "../Party/GLPartyClient.h"
#include "../Party/GLPartyManagerClient.h"
#include "../InstanceSystem/InstanceSystemClient.h"
#include "../NPCTalk/GLNpcTalkClient.h"
#include "../PointShop/PointShopClient.h"
#include "../Product/GLProductClient.h"
#include "../Pet/GLPetClient.h"
#include "../Trade/GLTradeClient.h"
#include "../Friend/GLFriendClient.h"
#include "../PrivateMarketSearchBuy/PrivateMarketClient.h"
#include "../GroupChat/GroupChatClient.h"
#include "../ReferChar/ReferChar.h"
#include "../Post/GLPostClient.h"
#include "../PVE/GLPVEClient.h"
#include "../Transport/GLTaxiStationClient.h"
#include "../Transport/GLBusStationClient.h"
#include "../CapturedTheField/GLCaptureTheFieldClient.h"
#include "../LottoSystem/GLLottoSystemManClient.h"
#include "../Attendance/GLAttendanceClient.h"
#include "../RebuildCard/RebuildCard.h"
#include "../Competition/Competition.h"
#include "../FlyCameraControl/GLFlyCameraControl.h"
#include "../RanMobile/GLRanMobileClient.h"

#define ALLOWCALL_CHECK_VAL( em, at, ac, ret ) \
	{\
		if( m_fElapsedTime - m_MsgTimer[ em ].fCallTime < at )\
		{\
			if( m_MsgTimer[ em ].wAllowCallCount >= ac )\
				return ret;\
		}\
		else\
		{\
			m_MsgTimer[ em ].fCallTime = m_fElapsedTime;\
			m_MsgTimer[ em ].wAllowCallCount = 0;\
		}\
		++m_MsgTimer[ em ].wAllowCallCount;\
	}

#define ALLOWCALL_CHECK( em, at, ac ) \
	{\
		if( m_fElapsedTime - m_MsgTimer[ em ].fCallTime < at )\
		{\
			if( m_MsgTimer[ em ].wAllowCallCount >= ac )\
				return;\
		}\
		else\
		{\
			m_MsgTimer[ em ].fCallTime = m_fElapsedTime;\
			m_MsgTimer[ em ].wAllowCallCount = 0;\
		}\
		++m_MsgTimer[ em ].wAllowCallCount;\
	}

GLWidgetMsgInterface::GLWidgetMsgInterface()
:	m_fElapsedTime( 0.0f )
{
}

GLWidgetMsgInterface::GLWidgetMsgInterface( const GLWidgetMsgInterface& value )
{
}

GLWidgetMsgInterface::~GLWidgetMsgInterface()
{
}

GLWidgetMsgInterface& GLWidgetMsgInterface::GetInstance()
{
	static GLWidgetMsgInterface Instance;
	return Instance;
}

void GLWidgetMsgInterface::FrameMove( float fElapsedTime )
{
	m_fElapsedTime += fElapsedTime;
}

bool GLWidgetMsgInterface::ConnectLoginServer()
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_ConnectLoginServer, 1.0f, 1, false );
	return GLLoginClient::GetInstance().ConnectLoginServer();
}

bool GLWidgetMsgInterface::ConnectServerChannel( int nServerIndex, int nServerGroup, int nChannelIndex )
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_ConnectServerChannel, 1.0f, 1, false );
	return GLLoginClient::GetInstance().ConnectServerChannel( nServerIndex, nServerGroup, nChannelIndex );
}

void GLWidgetMsgInterface::Login( const char* strID, const char* strPW, const char* strRP, int nServerChannel )
{
	ALLOWCALL_CHECK( WidgetMsg_Login, 1.0f, 1 );
	GLLoginClient::GetInstance().Login( strID, strPW, strRP, nServerChannel );
}

bool GLWidgetMsgInterface::JoinGame( int nIndex )
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_JoinGame, 1.0f, 1, false );
	return GLLobyCharClient::GetInstance().JoinGame( nIndex );
}

bool GLWidgetMsgInterface::JoinGame2ndPassword( const char* szPassword )
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_JoinGame2ndPassword, 1.0f, 1, false );
	return GLLobyCharClient::GetInstance().JoinGame2ndPassword( szPassword );
}

void GLWidgetMsgInterface::DeleteChar( int nIndex, const char* strPassNum )
{
	ALLOWCALL_CHECK( WidgetMsg_DeleteChar, 1.0f, 1 );
	GLLobyCharClient::GetInstance().DeleteChar( nIndex, strPassNum );
}

int GLWidgetMsgInterface::CreateChar( const char* strName )
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_CreateChar, 1.0f, 1, 0 );
	return GLLobyCharCreateClient::GetInstance().CreateChar( strName );
}

void GLWidgetMsgInterface::ApplyStatPoint( WORD wPow, WORD wDex, WORD wSpi, WORD wStr, WORD wSta )
{
	ALLOWCALL_CHECK( WidgetMsg_ApplyStatPoint, 1.0f, 1 );
	
	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->ApplyStatPoint( wPow, wDex, wSpi, wStr, wSta );
}

void GLWidgetMsgInterface::ReqChangeTitle( DWORD dwIndex, bool bSync )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqChangeTitle, 1.0f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->ReqChangeTitle( dwIndex, bSync );
}

void GLWidgetMsgInterface::ReqInvenItemSort()
{
	ALLOWCALL_CHECK( WidgetMsg_ReqInvenItemSort, 1.0f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->ReqInvenItemSort();
}

void GLWidgetMsgInterface::ReqGarbageResult()
{
	ALLOWCALL_CHECK( WidgetMsg_ReqGarbageResult, 1.0f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->ReqGarbageResult();
}

void GLWidgetMsgInterface::ReqMoneyToField( LONGLONG lnMoney )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqMoneyToField, 1.0f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->ReqMoneyToField( lnMoney );
}

void GLWidgetMsgInterface::ReqHoldToFieldForWidget( const char* szSlotID, int nHoldItemPlace, 
						WORD wSlotPosX, WORD wSlotPosY, float fPosX, float fPosY, float fPosZ )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqHoldToFieldForWidget, 1.0f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->ReqHoldToFieldForWidget( szSlotID, nHoldItemPlace, wSlotPosX, wSlotPosY, fPosX, fPosY, fPosZ );
}

void GLWidgetMsgInterface::SendChatMsg( DWORD dwType, const char* szName, const char* szMsg, LuaTable tbData )
{
	ALLOWCALL_CHECK( WidgetMsg_SendChatMsg, 0.1f, 2 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->SendChatMsg( dwType, szName, szMsg, tbData );
}

void GLWidgetMsgInterface::SkillLevelUp( LuaTable tbSkillUpData )
{
	ALLOWCALL_CHECK( WidgetMsg_SendChatMsg, 1.0f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->SkillLevelUp( tbSkillUpData );
}

void GLWidgetMsgInterface::RequestConflict( LuaTable tb )
{
	ALLOWCALL_CHECK( WidgetMsg_RequestConflict, 1.0f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->RequestConflict( tb );
}

void GLWidgetMsgInterface::AnswerConflict( bool bAns )
{
	ALLOWCALL_CHECK( WidgetMsg_AnswerConflict, 1.0f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->AnswerConflict( bAns );
}

void GLWidgetMsgInterface::ReqInvenHairStyleChange( WORD wHairStyle )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqInvenHairStyleChange, 1.0f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->ReqInvenHairStyleChange( wHairStyle );
}

void GLWidgetMsgInterface::ReqInvenFaceStyleChange( WORD wHairStyle )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqInvenFaceStyleChange, 1.0f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->ReqInvenFaceStyleChange( wHairStyle );
}

void GLWidgetMsgInterface::ReqInvenGenderChange( WORD wFace, WORD wHair )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqInvenGenderChange, 1.0f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->ReqInvenGenderChange( wFace, wHair );
}

void GLWidgetMsgInterface::ReqBikePutOnWith()
{
	ALLOWCALL_CHECK( WidgetMsg_ReqBikePutOnWith, 1.0f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->ReqBikePutOnWith();
}

bool GLWidgetMsgInterface::MoveToMap( DWORD dwMapID, int nPosX, int nPosY, bool bVehicle, bool bGMMove )
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_MoveToMap, 1.0f, 1, false );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return false;

	return pChar->MoveToMap( dwMapID, nPosX, nPosY, bVehicle, bGMMove );
}

void GLWidgetMsgInterface::ReqToggleRun()
{
	ALLOWCALL_CHECK( WidgetMsg_ReqToggleRun, 0.1f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->ReqToggleRun();
}

void GLWidgetMsgInterface::CommitVehicleColor( LuaTable tbColorList )
{
	ALLOWCALL_CHECK( WidgetMsg_CommitVehicleColor, 1.0f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->CommitVehicleColor( tbColorList );
}

void GLWidgetMsgInterface::CommitCostumeColor( LuaTable tbColor )
{
	ALLOWCALL_CHECK( WidgetMsg_CommitCostumeColor, 1.0f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->CommitCostumeColor( tbColor );
}

void GLWidgetMsgInterface::RequestResetSkillStats( LuaTable tbStats, const WORD wMaxAmount, const WORD wItemType )
{
	ALLOWCALL_CHECK( WidgetMsg_RequestResetSkillStats, 1.0f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->RequestResetSkillStats( tbStats, wMaxAmount, (EMITEM_TYPE) wItemType );
}

bool GLWidgetMsgInterface::TryRequestBankItemInfo()
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_TryRequestBankItemInfo, 1.0f, 1, false );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return false;

	return pChar->TryRequestBankItemInfo();
}

void GLWidgetMsgInterface::ReqChargedItemTo( WORD wPosX, WORD wPosY )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqChargedItemTo, 1.0f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->ReqChargedItemTo( wPosX, wPosY );
}

void GLWidgetMsgInterface::ResurrectCheckPoint()
{
	ALLOWCALL_CHECK( WidgetMsg_ResurrectCheckPoint, 1.0f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->ResurrectCheckPoint();
}

void GLWidgetMsgInterface::ResurrectExp()
{
	ALLOWCALL_CHECK( WidgetMsg_ResurrectExp, 1.0f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->ResurrectExp();
}

void GLWidgetMsgInterface::ResurrectCurPos()
{
	ALLOWCALL_CHECK( WidgetMsg_ResurrectCurPos, 1.0f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->ResurrectCurPos();
}

void GLWidgetMsgInterface::ResurrectSkill()
{
	ALLOWCALL_CHECK( WidgetMsg_ResurrectSkill, 1.0f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->ResurrectSkill();
}

void GLWidgetMsgInterface::ReqPartyLinkJoin( DWORD dwMasterID, int nLinkIndex )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqPartyLinkJoin, 1.0f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->ReqPartyLinkJoin( dwMasterID, nLinkIndex );
}

void GLWidgetMsgInterface::ReqQuestComplete( DWORD dwQUESTID )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqQuestComplete, 1.0f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->ReqQuestComplete( dwQUESTID );
}

void GLWidgetMsgInterface::ReqQuestGiveUp( DWORD dwQUESTID )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqQuestGiveUp, 1.0f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->ReqQuestGiveUp( dwQUESTID );
}

void GLWidgetMsgInterface::ReqInvenLunchBox(WORD wPosX, WORD wPosY)
{
	ALLOWCALL_CHECK( WidgetMsg_ReqInvenLunchBox, 1.0f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->ReqInvenLunchBox( wPosX, wPosY );
}

void GLWidgetMsgInterface::ReqInvenRename( const char* szCharName )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqInvenRename, 1.0f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->ReqInvenRename( szCharName );
}

void GLWidgetMsgInterface::ReqPMarketClose()
{
	ALLOWCALL_CHECK( WidgetMsg_ReqPMarketClose, 1.0f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->ReqPMarketClose();
}

bool GLWidgetMsgInterface::ReqStorageSaveMoneyForWidget( LONGLONG lnMoney )
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_ReqStorageSaveMoneyForWidget, 5.0f, 1, false );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return false;

	pChar->ReqStorageSaveMoneyForWidget( lnMoney );

	return true;
}

bool GLWidgetMsgInterface::ReqStorageDrawMoneyForWidget( LONGLONG lnMoney )
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_ReqStorageDrawMoneyForWidget, 5.0f, 1, false );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return false;

	pChar->ReqStorageDrawMoneyForWidget( lnMoney );

	return true;
}

void GLWidgetMsgInterface::ReqStorageOpenCard(WORD wPosX, WORD wPosY)
{
	ALLOWCALL_CHECK( WidgetMsg_ReqStorageOpenCard, 1.0f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->ReqStorageOpenCard( wPosX, wPosY );
}

bool GLWidgetMsgInterface::ReqGetStorageForWidget( DWORD dwChannel )
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_ReqGetStorageForWidget, 1.0f, 5, false );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return false;

	return pChar->ReqGetStorageForWidget( dwChannel );
}

void GLWidgetMsgInterface::ReqStorageCard( WORD wPosX, WORD wPosY, WORD wSTORAGE )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqStorageCard, 1.0f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->ReqStorageCard( wPosX, wPosY, wSTORAGE );
}

void GLWidgetMsgInterface::ReqInvenDrug(WORD wPosX, WORD wPosY)
{
	ALLOWCALL_CHECK( WidgetMsg_ReqInvenDrug, 0.1f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->ReqInvenDrug( wPosX, wPosY );
}

void GLWidgetMsgInterface::ReqStorageCloseCard()
{
	ALLOWCALL_CHECK( WidgetMsg_ReqStorageCloseCard, 1.0f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->ReqStorageCloseCard();
}

void GLWidgetMsgInterface::ReqGateOut(const DWORD dwIndex)
{
	ALLOWCALL_CHECK( WidgetMsg_ReqGateOut, 1.0f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->ReqGateOut( dwIndex );
}

void GLWidgetMsgInterface::ReqGuidCommissionForWidgt( float fRate )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqGuidCommissionForWidgt, 1.0f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->ReqGuidCommissionForWidgt( fRate );
}

void GLWidgetMsgInterface::ReqRecoveryExpNpc( DWORD dwNpcGlobID )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqRecoveryExpNpc, 1.0f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->ReqRecoveryExpNpc( dwNpcGlobID );
}

void GLWidgetMsgInterface::ReqClubNewForWidget( const char* szClubName )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqClubNewForWidget, 1.0f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->ReqClubNewForWidget( szClubName );
}

void GLWidgetMsgInterface::ReqSlotChange()
{
	ALLOWCALL_CHECK( WidgetMsg_ReqSlotChange, 0.25f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->ReqSlotChange();
}

bool GLWidgetMsgInterface::ReqStorageOpenFromInven()
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_ReqStorageOpenFromInven, 1.0f, 1, false );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return false;

	return pChar->ReqStorageOpenFromInven();
}

void GLWidgetMsgInterface::ReqSkillHoldResetForWidget( DWORD dwID, WORD wSlot )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqSkillHoldResetForWidget, 1.0f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->ReqSkillHoldResetForWidget( dwID, wSlot );
}

void GLWidgetMsgInterface::ReqSetVehicle( bool bActive )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqSetVehicle, 1.0f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->ReqSetVehicle( bActive );
}

void GLWidgetMsgInterface::ReqInvenSplit( WORD wPosX, WORD wPosY, WORD wSplitNum )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqInvenSplit, 1.0f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->ReqInvenSplit( wPosX, wPosY, wSplitNum );
}

void GLWidgetMsgInterface::ReqPartyLinkLure( bool bLure, const DWORD dwHashKey, const DWORD dwTimeKey, const DWORD dwGaeaID )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqPartyLinkLure, 1.0f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->ReqPartyLinkLure( bLure, dwHashKey, dwTimeKey, dwGaeaID );
}

void GLWidgetMsgInterface::ReqSkillRunSet( const WORD wSLOT )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqSkillRunSet, 1.0f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->ReqSkillRunSet( wSLOT );
}

void GLWidgetMsgInterface::SendCustomMessage (
	const DWORD _dwDestType,
	const DWORD _dwInstanceDungeonID,
	const DWORD _dwParam1,
	const DWORD _dwParam2,
	const DWORD _dwParam3,
	const DWORD _dwParam4 )
{
	ALLOWCALL_CHECK( WidgetMsg_SendCustomMessage, 1.0f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->SendCustomMessage( _dwDestType, _dwInstanceDungeonID, _dwParam1, _dwParam2, _dwParam3, _dwParam4 );
}

void GLWidgetMsgInterface::ReqBeginMinigame(void)
{
	ALLOWCALL_CHECK( WidgetMsg_doMessageRequestBegin, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetMiniGameManager()->doMessageRequestBegin();
}

void GLWidgetMsgInterface::ReqSelectOddEvenMinigame(const bool _bOdd)
{
	ALLOWCALL_CHECK( WidgetMsg_doMessageRequestSelectOddEven, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetMiniGameManager()->doMessageRequestSelectOddEven( _bOdd );
}

void GLWidgetMsgInterface::ReqRollMinigame(void)
{
	ALLOWCALL_CHECK( WidgetMsg_doMessageRequestRoll, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetMiniGameManager()->doMessageRequestRoll();
}

void GLWidgetMsgInterface::ReqResultMinigame(void)
{
	ALLOWCALL_CHECK( WidgetMsg_doMessageRequestResult, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetMiniGameManager()->doMessageRequestResult();
}

void GLWidgetMsgInterface::ReqRewardMinigame(void)
{
	ALLOWCALL_CHECK( WidgetMsg_doMessageRequestReward, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetMiniGameManager()->doMessageRequestReward();
}

void GLWidgetMsgInterface::ReqSelectRewardMinigame(const unsigned int _indexReward)
{
	ALLOWCALL_CHECK( WidgetMsg_doMessageRequestSelectReward, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetMiniGameManager()->doMessageRequestSelectReward( _indexReward );
}

void GLWidgetMsgInterface::ReqReceiveRewardMinigame(void)
{
	ALLOWCALL_CHECK( WidgetMsg_doMessageRequestReceiveReward, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetMiniGameManager()->doMessageRequestReceiveReward();
}

void GLWidgetMsgInterface::ReqCloseMinigame(void)
{
	ALLOWCALL_CHECK( WidgetMsg_RequestClose, 0.05f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetMiniGameManager()->RequestClose();
}

void GLWidgetMsgInterface::InstanceManRequestClose()
{
	ALLOWCALL_CHECK( WidgetMsg_InstanceManrequestClose, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetInstanceManager()->requestClose();
}

void GLWidgetMsgInterface::LureParty(DWORD dwGaeaID)
{
	ALLOWCALL_CHECK( WidgetMsg_LureParty, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetMyPartyClient()->Lure( dwGaeaID );
}

void GLWidgetMsgInterface::LureNameParty( const char* szName )
{
	ALLOWCALL_CHECK( WidgetMsg_LureNameParty, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetMyPartyClient()->LureName( szName );
}

void GLWidgetMsgInterface::SecedeParty(DWORD dwIndex)
{
	ALLOWCALL_CHECK( WidgetMsg_SecedeParty, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetMyPartyClient()->Secede( dwIndex );
}

void GLWidgetMsgInterface::DissolveParty(void)
{
	ALLOWCALL_CHECK( WidgetMsg_DissolveParty, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetMyPartyClient()->Dissolve();
}

void GLWidgetMsgInterface::AuthorityParty(const DWORD dwIndex)
{
	ALLOWCALL_CHECK( WidgetMsg_AuthorityParty, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetMyPartyClient()->Authority( dwIndex );
}

void GLWidgetMsgInterface::ChangeOptionParty( WORD wItemOp, WORD wMoneyOp, WORD wItemFilter )
{
	ALLOWCALL_CHECK( WidgetMsg_ChangeOptionParty, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetMyPartyClient()->ChangeOption( wItemOp, wMoneyOp, wItemFilter );
}

void GLWidgetMsgInterface::ReqInviteParty()
{
	ALLOWCALL_CHECK( WidgetMsg_ReqInviteParty, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetMyPartyClient()->InvitePartyReq();
}

void GLWidgetMsgInterface::ReqPartyInviteAns( bool bAccept, DWORD dwPartyID, DWORD dwHashKey, DWORD dwMasterID, DWORD dwTimeKey )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqPartyInviteAns, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetPartyManager()->ReqPartyInviteAns(
		bAccept, dwPartyID, dwHashKey, dwMasterID, dwTimeKey );
}

bool GLWidgetMsgInterface::SelectNpcTalk( DWORD dwAnswerID )
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_SelectNpcTalk, 0.05f, 1, false );
	return GLNpcTalkClient::GetInstance().SelectTalk( dwAnswerID );
}

void GLWidgetMsgInterface::BuyItemNpcTalk( DWORD dwPageIndex, WORD wPosX, WORD wPosY, WORD wBuyNum )
{
	ALLOWCALL_CHECK( WidgetMsg_BuyItemNpcTalk, 1.0f, 1 );
	GLNpcTalkClient::GetInstance().BuyItem( dwPageIndex, wPosX, wPosY, wBuyNum );
}

void GLWidgetMsgInterface::SellItemNpcTalk( WORD wPosX, WORD wPosY )
{
	ALLOWCALL_CHECK( WidgetMsg_SellItemNpcTalk, 0.1f, 1 );
	GLNpcTalkClient::GetInstance().SellItem( wPosX, wPosY );
}

bool GLWidgetMsgInterface::BuyCartItemNpcTalk(WORD wTabPage)
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_BuyCartItemNpcTalk, 1.0f, 1, false );
	return GLNpcTalkClient::GetInstance().BuyCartItem(wTabPage);
}

bool GLWidgetMsgInterface::RebuyNpcTalk( WORD wIndex )
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_RebuyNpcTalk, 1.0f, 1, false );

	return GLNpcTalkClient::GetInstance().Rebuy( wIndex );
}

void GLWidgetMsgInterface::BuyCartListPointShop()
{
	ALLOWCALL_CHECK( WidgetMsg_BuyCartListPointShop, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetPointShopClient()->BuyCartList();
}

void GLWidgetMsgInterface::ReqProductTypeAdd( WORD wProductType )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqProductTypeAdd, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetProductClient()->ReqProductTypeAdd( wProductType );
}

void GLWidgetMsgInterface::ReqProductTypeDelete( LuaTable tbProductList, WORD wPosX, WORD wPosY )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqProductTypeDelete, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetProductClient()->ReqProductTypeDelete(
		tbProductList, wPosX, wPosY );
}

void GLWidgetMsgInterface::ReqProduct( DWORD nID, WORD nCount, bool bOngoing )
{
//	ALLOWCALL_CHECK( WidgetMsg_ReqProduct, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetProductClient()->ReqProduct( nID, nCount, bOngoing );
}

void GLWidgetMsgInterface::ReqProductCancel()
{
//	ALLOWCALL_CHECK( WidgetMsg_ReqProductCancel, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetProductClient()->ReqProductCancel();
}

void GLWidgetMsgInterface::ReqProductTypeList()
{
//	ALLOWCALL_CHECK( WidgetMsg_ReqProductTypeList, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetProductClient()->ReqProductTypeList();
}

void GLWidgetMsgInterface::SetActivatedSkillPet( LuaTable tbActivatedSkill )
{
	ALLOWCALL_CHECK( WidgetMsg_SetActivatedSkillPet, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetPetClient()->SetActivatedSkill( tbActivatedSkill );
}

void GLWidgetMsgInterface::ReqChangeStylePet( WORD wStyle )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqChangeStylePet, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetPetClient()->ReqChangeStyle( wStyle );
}

void GLWidgetMsgInterface::CommitColorPet( DWORD dwColor )
{
	ALLOWCALL_CHECK( WidgetMsg_CommitColorPet, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetPetClient()->CommitColor( dwColor );
}

void GLWidgetMsgInterface::ReqRenamePet( const char* szCharName )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqRenamePet, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetPetClient()->ReqRename( szCharName );
}

bool GLWidgetMsgInterface::ReqTradeOpen()
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_ReqTradeOpen, 1.0f, 1, false );
	return GLWidgetScript::GetInstance().m_pGaeaClient->GetTradeClient()->ReqTradeOpen();
}

bool GLWidgetMsgInterface::ReqTradeMoney( LONGLONG lnMoney )
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_ReqTradeMoney, 0.25f, 1, false );
	return GLWidgetScript::GetInstance().m_pGaeaClient->GetTradeClient()->ReqTradeMoney( lnMoney );
}

bool GLWidgetMsgInterface::ReqTradeLock()
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_ReqTradeLock, 1.0f, 1, false );
	return GLWidgetScript::GetInstance().m_pGaeaClient->GetTradeClient()->ReqTradeLock();
}

bool GLWidgetMsgInterface::ReqTradeAgree()
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_ReqTradeAgree, 1.0f, 1, false );
	return GLWidgetScript::GetInstance().m_pGaeaClient->GetTradeClient()->ReqTradeAgree();
}

bool GLWidgetMsgInterface::ReqTradeCancel()
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_ReqTradeCancel, 1.0f, 1, false );
	return GLWidgetScript::GetInstance().m_pGaeaClient->GetTradeClient()->ReqTradeCancel();
}

void GLWidgetMsgInterface::ReqTradeAns( bool bOK )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqTradeAns, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetTradeClient()->ReqTradeAns( bOK );
}

void GLWidgetMsgInterface::ReqFriendAdd(const char* szName)
{
	ALLOWCALL_CHECK( WidgetMsg_ReqFriendAdd, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetFriendClient()->ReqFriendAdd( szName );
}

void GLWidgetMsgInterface::ReqFriendGroupAdd( const char* szName )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqFriendGroupAdd, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetFriendClient()->ReqGroupAdd( szName );
}

void GLWidgetMsgInterface::ReqFriendGroupRename( WORD GroupNum, const char* NewName )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqFriendGroupRename, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetFriendClient()->ReqGroupRename( GroupNum, NewName );
}

void GLWidgetMsgInterface::ReqFriendGroupDelete( WORD GroupNum )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqFriendGroupDelete, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetFriendClient()->ReqGroupDelete( GroupNum );
}

void GLWidgetMsgInterface::ReqFriendMoveGroup( const char* szName, WORD GroupNum )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqFriendMoveGroup, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetFriendClient()->ReqFriendMoveGroup( szName, GroupNum );
}

void GLWidgetMsgInterface::ReqFriendBlock(const char* szName, bool bBLOCK)
{
	ALLOWCALL_CHECK( WidgetMsg_ReqFriendBlock, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetFriendClient()->ReqFriendBlock( szName, bBLOCK );
}

void GLWidgetMsgInterface::ReqFriendDel(const char* szDelName)
{
	ALLOWCALL_CHECK( WidgetMsg_ReqFriendDel, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetFriendClient()->ReqFriendDel( szDelName );
}

void GLWidgetMsgInterface::ReqFriendInviteParty( const char* szName )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqFriendInviteParty, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetFriendClient()->ReqFriendInviteParty( szName );
}

void GLWidgetMsgInterface::ReqMoveToFriend( const char* szName )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqMoveToFriend, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetFriendClient()->ReqMoveToFriend( szName );
}

void GLWidgetMsgInterface::ReqFriendLureAns( const char* szReqName, bool bOK )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqFriendLureAns, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetFriendClient()->ReqFriendLureAnsForWidget( szReqName, bOK );
}

void GLWidgetMsgInterface::SfReqFriendLureAns_throughAlarm(const char* szReqName, bool bOK, LuaTable tbMGuid)
{
	ALLOWCALL_CHECK( WidgetMsg_ReqFriendLureAns_throughAlarm, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetFriendClient()->SfReqFriendLureAns_throughAlarm( szReqName, bOK, tbMGuid );
}

void GLWidgetMsgInterface::ReqBuySearchItemPrivateMarket( LuaTable tbPurchase )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqBuySearchItem, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetPrivateMarketClient()->ReqBuySearchItem( tbPurchase );
}

bool GLWidgetMsgInterface::SearchItemPrivateMarket( WORD wPage, LuaTable tbSearchInfo )
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_SearchItemPrivateMarket, 1.0f, 1, false );
	return GLWidgetScript::GetInstance().m_pGaeaClient->GetPrivateMarketClient()->SearchItem( wPage, tbSearchInfo );
}

bool GLWidgetMsgInterface::RefreshLogPrivateMarket()
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_RefreshLogPrivateMarket, 1.0f, 1, false );
	return GLWidgetScript::GetInstance().m_pGaeaClient->GetPrivateMarketClient()->RefreshLog();
}

void GLWidgetMsgInterface::ReqPointTradeListPrivateMarket()
{
	ALLOWCALL_CHECK( WidgetMsg_ReqPointTradeListPrivateMarket, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetPrivateMarketClient()->ReqPointTradeList();
}

bool GLWidgetMsgInterface::ReqRegistItemPrivateMarket( int nPosX, int nPosY, WORD wCount, LONGLONG llItemPrice, 
				   WORD wSellType, WORD wMoneyIndex, WORD wPeriodIndex )
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_ReqRegistItemPrivateMarket, 1.0f, 1, false );
	return GLWidgetScript::GetInstance().m_pGaeaClient->GetPrivateMarketClient()->ReqRegistItem( 
		nPosX, nPosY, wCount, llItemPrice, wSellType, wMoneyIndex, wPeriodIndex );
}

bool GLWidgetMsgInterface::ReqUnregistItemPrivateMarket( int nSelectedIndex )
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_ReqUnregistItemPrivateMarket, 1.0f, 1, false );
	return GLWidgetScript::GetInstance().m_pGaeaClient->GetPrivateMarketClient()->ReqUnregistItem( nSelectedIndex );
}

void GLWidgetMsgInterface::ReqPrivateMarketRegItemOpen( const char* cMarketName )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqPrivateMarketRegItemOpen, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetPrivateMarketClient()->ReqPrivateMarketRegItemOpen( cMarketName );
}

void GLWidgetMsgInterface::ReqPrivateMarketRegItem( WORD wPosX, WORD wPosY, LONGLONG llMoney, DWORD dwNum )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqPrivateMarketRegItem, 1.0f, 1 );

	// 0 개를 등록 할 수는 없다.
	// 최소한개 이상이다.
	if ( dwNum == 0 )
		dwNum = 1;

	GLWidgetScript::GetInstance().m_pGaeaClient->GetPrivateMarketClient()->ReqPrivateMarketRegItem( 
		wPosX, wPosY, llMoney, dwNum );
}

void GLWidgetMsgInterface::ReqPrivateMarketOpen( bool bOnlyUI )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqPrivateMarketOpen, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetPrivateMarketClient()->ReqPrivateMarketOpen( bOnlyUI );
}

void GLWidgetMsgInterface::ReqPrivateMarketClose()
{
	ALLOWCALL_CHECK( WidgetMsg_ReqPrivateMarketClose, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetPrivateMarketClient()->ReqPrivateMarketClose();
}

void GLWidgetMsgInterface::ReqPMarketBuyItem( WORD wPosX, WORD wPosY, DWORD dwNum )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqPMarketBuyItem, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetPrivateMarketClient()->ReqPMarketBuyItem( wPosX, wPosY, dwNum );
}

bool GLWidgetMsgInterface::ReqGroupChat( const char* szName, int nType )
{
	// ALLOWCALL_CHECK_VAL( WidgetMsg_ReqGroupChat, 1.0f, 1, false );
	return GLWidgetScript::GetInstance().m_pGaeaClient->GetChatGroup()->ReqGroupChat( szName, nType );
}

void GLWidgetMsgInterface::ReqGroupChatFB( bool bAccept )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqGroupChatFB, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetChatGroup()->ReqGroupChatFB( bAccept );
}

void GLWidgetMsgInterface::ReqQuitGroupChat( DWORD dwKey )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqQuitGroupChat, 0.1f, 10 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetChatGroup()->ReqQuit( dwKey );
}

void GLWidgetMsgInterface::SendChatGroupChat( DWORD dwKey, const char* szMsg )
{
	ALLOWCALL_CHECK( WidgetMsg_SendChatGroupChat, 0.1f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetChatGroup()->SendChat( dwKey, szMsg );
}

void GLWidgetMsgInterface::ReqKickGroupChat( const char* szName )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqKickGroupChat, 0.5f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetChatGroup()->ReqKick( szName );
}

void GLWidgetMsgInterface::ReqSearchCharacter( const char* szName, bool bCurMap, DWORD dwCurMap )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqSearchCharacter, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->ReqSearchCharacter( szName, bCurMap, dwCurMap );
}

void GLWidgetMsgInterface::ReqSearchCharacterDetailInfo( const char* szName )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqSearchCharacterDetailInfo, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->ReqSearchCharacterDetailInfo( szName );
}

void GLWidgetMsgInterface::ReqSearchCharacterPosition( const char* szName )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqSearchCharacterPosition, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->ReqSearchCharacterPosition( szName );
}

bool GLWidgetMsgInterface::ReqSetClubMemberGrade( DWORD dwDbNum, unsigned int nIndex )
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_ReqSetClubMemberGrade, 1.0f, 1, false );
	return GLWidgetScript::GetInstance().m_pGaeaClient->SetClubMemberGrade( dwDbNum, nIndex );
}

bool GLWidgetMsgInterface::ReqClubNoticeChange( const char* szNotice )
{
	ALLOWCALL_CHECK_VAL( widgetMsg_ReqClubNoticeChange, 1.0f, 1, false );
	return GLWidgetScript::GetInstance().m_pGaeaClient->ReqClubNoticeChange( szNotice );
}

bool GLWidgetMsgInterface::ReqGetClubStorage()
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_ReqGetClubStorage, 1.0f, 1, false );
	return GLWidgetScript::GetInstance().m_pGaeaClient->ReqGetClubStorage();
}

bool GLWidgetMsgInterface::ReqClubStorageDrawMoney( LONGLONG lnMoney )
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_ReqClubStorageDrawMoney, 1.0f, 1, false );
	return GLWidgetScript::GetInstance().m_pGaeaClient->ReqClubStorageDrawMoney( lnMoney );
}

bool GLWidgetMsgInterface::ReqClubStorageSaveMoney( LONGLONG lnMoney )
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_ReqClubStorageSaveMoney, 1.0f, 1, false );
	return GLWidgetScript::GetInstance().m_pGaeaClient->ReqClubStorageSaveMoney( lnMoney );
}

bool GLWidgetMsgInterface::ClubIncomeMoneyToStorageMoney()
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_ClubIncomeMoneyToStorageMoney, 1.0f, 1, false );
	return GLWidgetScript::GetInstance().m_pGaeaClient->ClubIncomeMoneyToStorageMoney();
}

bool GLWidgetMsgInterface::ReqClubNick( const char* szNickName )
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_ReqClubNick, 1.0f, 1, false );
	return GLWidgetScript::GetInstance().m_pGaeaClient->ReqClubNick( szNickName );
}

void GLWidgetMsgInterface::ReqChangeGradeName( int nIndex, const char* szGradeName )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqChangeGradeName, 1.0f, 10 );
	GLWidgetScript::GetInstance().m_pGaeaClient->ReqChangeGradeName( nIndex, szGradeName );
}

void GLWidgetMsgInterface::ReqAuthorityChange( LuaTable tbAuthData )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqAuthorityChange, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->ReqAuthorityChange( tbAuthData );
}

bool GLWidgetMsgInterface::ReqClubDissolution()
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_ReqClubDissolution, 1.0f, 1, false );
	return GLWidgetScript::GetInstance().m_pGaeaClient->ReqClubDissolution();
}

bool GLWidgetMsgInterface::ReqClubDissolutionCancel()
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_ReqClubDissolutionCancel, 1.0f, 1, false );
	return GLWidgetScript::GetInstance().m_pGaeaClient->ReqClubDissolutionCancel();
}

bool GLWidgetMsgInterface::ReqClubMarkChange( const char* szFileName )
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_ReqClubMarkChange, 1.0f, 1, false );
	return GLWidgetScript::GetInstance().m_pGaeaClient->ReqClubMarkChange( szFileName );
}

bool GLWidgetMsgInterface::ReqClubSecede()
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_ReqClubSecede, 1.0f, 1, false );
	return GLWidgetScript::GetInstance().m_pGaeaClient->ReqClubSecede();
}

bool GLWidgetMsgInterface::ClubAutoKickReq( bool AutoKickUse, size_t GradeIndex, WORD Day )
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_ClubAutoKickReq, 1.0f, 1, false );
	return GLWidgetScript::GetInstance().m_pGaeaClient->ClubAutoKickReq( AutoKickUse, GradeIndex, Day );
}

void GLWidgetMsgInterface::DelClubMember(const char* _Name)
{
	ALLOWCALL_CHECK( WidgetMsg_DelClubMember, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->DelClubMember( _Name );
}

void GLWidgetMsgInterface::ClubMasterChange(int nDBNum)
{
	ALLOWCALL_CHECK( WidgetMsg_ClubMasterChange, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->ClubMasterChange( nDBNum );
}

void GLWidgetMsgInterface::ClubCDAuth( const char* _Name )
{
	ALLOWCALL_CHECK( WidgetMsg_ClubCDAuth, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->ClubCDAuth( _Name );
}

void GLWidgetMsgInterface::RequestClubJoin()
{
	ALLOWCALL_CHECK( WidgetMsg_RequestClubJoin, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->RequestClubJoin();
}

bool GLWidgetMsgInterface::ReqClubJoinAns(DWORD dwMaster, bool bOK)
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_ReqClubJoinAns, 1.0f, 1, false );
	return GLWidgetScript::GetInstance().m_pGaeaClient->ReqClubJoinAns( dwMaster, bOK );
}

void GLWidgetMsgInterface::ReqPVERetry(bool bOk)
{
	ALLOWCALL_CHECK( WidgetMsg_ReqPVERetry, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->ReqPVERetry( bOk );
}

void GLWidgetMsgInterface::ReqNewInstanceRetry(bool bOk)
{
	ALLOWCALL_CHECK( WidgetMsg_ReqNewInstanceRetry, 1.0f, 1 );
	InstanceSystem::ManagerClient* pInstance = GLWidgetScript::GetInstance().m_pGaeaClient->GetInstanceManager();
	if ( NULL == pInstance )
		return;

	GLWidgetScript::GetInstance().m_pGaeaClient->ReqNewInstanceRetry(
		bOk, pInstance->GetRetryInstanceID(), pInstance->GetRetryKeyMapID() );
}

void GLWidgetMsgInterface::ReqClubAuthorityAns(bool bOK)
{
	ALLOWCALL_CHECK( WidgetMsg_ReqClubAuthorityAns, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->ReqClubAuthorityAns( bOK );
}

bool GLWidgetMsgInterface::ReqClubAllianceAns(DWORD dwChiefCharID, bool bOK)
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_ReqClubAllianceAns, 1.0f, 1, false );
	return GLWidgetScript::GetInstance().m_pGaeaClient->ReqClubAllianceAns( dwChiefCharID, bOK );
}

bool GLWidgetMsgInterface::ReqClubBattle( const char* szCharName, WORD wHour, WORD wMin )
{
	if( NULL == szCharName )
		return false;

	ALLOWCALL_CHECK_VAL( WidgetMsg_ReqClubBattle, 0.5f, 1, false );
	return GLWidgetScript::GetInstance().m_pGaeaClient->ReqClubBattle( szCharName, wHour, wMin );
}

bool GLWidgetMsgInterface::ReqAllianceBattleArmistice(DWORD dwCLUBID)
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_ReqAllianceBattleArmistice, 1.0f, 1, false );
	return GLWidgetScript::GetInstance().m_pGaeaClient->ReqAllianceBattleArmistice( dwCLUBID );
}

bool GLWidgetMsgInterface::ReqClubBattleArmistice(DWORD dwCLUBID)
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_ReqClubBattleArmistice, 1.0f, 1, false );
	return GLWidgetScript::GetInstance().m_pGaeaClient->ReqClubBattleArmistice( dwCLUBID );
}

void GLWidgetMsgInterface::ReqAllianceBattleArmisticeAns(DWORD dwCLUBID, DWORD ReqChaDbNum, bool bOK)
{
	ALLOWCALL_CHECK( WidgetMsg_ReqAllianceBattleArmisticeAns, 1.0, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->ReqAllianceBattleArmisticeAns( dwCLUBID, ReqChaDbNum, bOK );
}

void GLWidgetMsgInterface::ReqClubBattleArmisticeAns(DWORD dwCLUBID, DWORD ReqChaDbNum, bool bOK)
{
	ALLOWCALL_CHECK( WidgetMsg_ReqClubBattleArmisticeAns, 1.0, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->ReqClubBattleArmisticeAns( dwCLUBID, ReqChaDbNum, bOK );
}

bool GLWidgetMsgInterface::ReqAllianceBattleAns(DWORD dwChiefCharID, bool bOK)
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_ReqAllianceBattleAns, 1.0f, 1, false );
	return GLWidgetScript::GetInstance().m_pGaeaClient->ReqAllianceBattleAns( dwChiefCharID, bOK );
}

bool GLWidgetMsgInterface::ReqAllianceBattle( const char* szCharName, WORD wHour, WORD wMin )
{
	if( NULL == szCharName )
		return false;

	ALLOWCALL_CHECK_VAL( WidgetMsg_ReqAllianceBattle, 0.5f, 1, false );
	return GLWidgetScript::GetInstance().m_pGaeaClient->ReqAllianceBattle( szCharName, wHour, wMin );
}

bool GLWidgetMsgInterface::ReqClubBattleAns(DWORD dwChiefCharID, bool bOK)
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_ReqClubBattleAns, 1.0f, 1, false );
	return GLWidgetScript::GetInstance().m_pGaeaClient->ReqClubBattleAns( dwChiefCharID, bOK );
}

bool GLWidgetMsgInterface::ReqClubAlliance_PToPTarget()
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_ReqClubAlliance_PToPTarget, 1.0f, 1, false );
	return GLWidgetScript::GetInstance().m_pGaeaClient->ReqClubAlliance_PToPTarget();
}

bool GLWidgetMsgInterface::ReqClubAllianceDel_Targetname(const char* szClubName)
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_ReqClubAllianceDel_Targetname, 1.0f, 1, false );
	return GLWidgetScript::GetInstance().m_pGaeaClient->ReqClubAllianceDel_Targetname( szClubName );
}

bool GLWidgetMsgInterface::ReqClubBattleArmistice_TargetName(const char* szClubName)
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_ReqClubBattleArmistice_TargetName, 1.0f, 1, false );
	return GLWidgetScript::GetInstance().m_pGaeaClient->ReqClubBattleArmistice_TargetName( szClubName );
}

bool GLWidgetMsgInterface::ReqClubBattleSubmission_TargetName(const char* szClubName)
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_ReqClubBattleSubmission_TargetName, 1.0f, 1, false );
	return GLWidgetScript::GetInstance().m_pGaeaClient->ReqClubBattleSubmission_TargetName( szClubName );
}

bool GLWidgetMsgInterface::ReqClubJoin(DWORD dwGaeaID)
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_ReqClubJoin, 1.0f, 1, false );
	return GLWidgetScript::GetInstance().m_pGaeaClient->ReqClubJoin( dwGaeaID );
}

bool GLWidgetMsgInterface::ReqClubJoinByCharname(const char* szCharName)
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_ReqClubJoinByCharname, 1.0f, 1, false );
	return GLWidgetScript::GetInstance().m_pGaeaClient->ReqClubJoinByCharname( szCharName );
}

void GLWidgetMsgInterface::ReqReferChar( const char* szTarName )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqReferChar, 1.0f, 1 );
	
	std::string strReq(szTarName);

	const GLGaeaClient* const pGaeaClient = GLWidgetScript::GetInstance().m_pGaeaClient;
	//if ( pGaeaClient->GetServiceProvider() == SP_TAIWAN  )
	//	strReq = sc::string::utf8_to_ansi( std::string(szTarName) );
	
	GLWidgetScript::GetInstance().m_pGaeaClient->GetReferChar()->ReqReferChar( strReq.c_str() );
}

bool GLWidgetMsgInterface::ReqReceivePageRefreshPost()
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_ReqReceivePageRefresh, 1.0f, 1, false );
	return GLWidgetScript::GetInstance().m_pGaeaClient->GetPostClient()->ReqReceivePageRefresh();
}

void GLWidgetMsgInterface::ReqSelectPostOpen( const unsigned int nSelect )
{
	// 읽은 정보는 다시 서버에 요청안함으로 지운다;
	// 렌더러 클릭당 우편 정보가 보여야함;
	//ALLOWCALL_CHECK( WidgetMsg_ReqSelectPostOpen, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetPostClient()->ReqSelectPostOpen( nSelect );
}

void GLWidgetMsgInterface::ReqSelectPostDelete( const unsigned int nSelect, bool bAttachDelete )
{
	// 다중 체크시 문제가될 수 있다; 따로 다중 처리는 처리하지않는다;
	// ALLOWCALL_CHECK( WidgetMsg_ReqSelectPostDelete, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetPostClient()->ReqSelectPostDelete( nSelect, bAttachDelete );
}

void GLWidgetMsgInterface::ReqSelectPostTake( const unsigned int nSelect )
{
	// 읽은 정보는 다시 서버에 요청안함으로 지운다;
	// 렌더러 클릭당 우편 정보가 보여야함;
	ALLOWCALL_CHECK( WidgetMsg_ReqSelectPostTake, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetPostClient()->ReqSelectPostTake( nSelect );
}

void GLWidgetMsgInterface::ReqSelectPostReturn( const unsigned int nSelect )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqSelectPostReturn, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetPostClient()->ReqSelectPostReturn( nSelect );
}

void GLWidgetMsgInterface::ReqSelectPostTakeItem( const unsigned int nSelect )
{
	// 여러개 선택시 문제가됨;
	// ALLOWCALL_CHECK( WidgetMsg_ReqSelectPostTakeItem, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetPostClient()->ReqSelectPostTakeItem( nSelect );
}

void GLWidgetMsgInterface::ResetCharPostItemData()
{
	GLWidgetScript::GetInstance().m_pGaeaClient->GetPostClient()->ResetCharPostItemData();
}

bool GLWidgetMsgInterface::ReqSendPostTable( LuaTable tbPost )
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_ReqSendPostTable, 1.0f, 1, false );
	return GLWidgetScript::GetInstance().m_pGaeaClient->GetPostClient()->ReqSendPostTable( tbPost );
}

bool GLWidgetMsgInterface::ReqReceiveHistoryPost()
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_ReqReceiveHistory, 1.0f, 1, false );
	return GLWidgetScript::GetInstance().m_pGaeaClient->GetPostClient()->ReqReceiveHistory();
}

bool GLWidgetMsgInterface::ReqReceiveSendHistoryPost()
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_ReqReceiveSendHistory, 1.0f, 1, false );
	return GLWidgetScript::GetInstance().m_pGaeaClient->GetPostClient()->ReqReceiveSendHistory();
}

bool GLWidgetMsgInterface::ReqReceiveRecvHistoryPost()
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_ReqReceiveRecvHistory, 1.0f, 1, false );
	return GLWidgetScript::GetInstance().m_pGaeaClient->GetPostClient()->ReqReceiveRecvHistory();
}

void GLWidgetMsgInterface::ReqUseInvenPostBoxCardPost()
{
	ALLOWCALL_CHECK( WidgetMsg_ReqUseInvenPostBoxCard, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetPostClient()->ReqUseInvenPostBoxCard();
}

bool GLWidgetMsgInterface::ReqCDMRankInfo( DWORD dwScriptID )
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_ReqCDMRankInfo, 1.0f, 1, false );
	return Competition::GetInstance().ReqCDMRankInfo( dwScriptID );
}

void GLWidgetMsgInterface::ReqCTFJoin(void)
{
	ALLOWCALL_CHECK( WidgetMsg_ReqCTFJoin, 1.0f, 1 );
	Competition::GetInstance().ReqCTFJoin();
}

void GLWidgetMsgInterface::ReqCancelCompetition(void)
{
	ALLOWCALL_CHECK( WidgetMsg_ReqCancelCompetition, 1.0f, 1 );
	Competition::GetInstance().ReqCancel();
}

void GLWidgetMsgInterface::ReqExitCompetition(void)
{
	ALLOWCALL_CHECK( WidgetMsg_ReqExitCompetition, 1.0f, 1 );
	Competition::GetInstance().ReqExit();
}

void GLWidgetMsgInterface::OnEnteranceCompetition(const unsigned int nIndex)
{
	ALLOWCALL_CHECK( WidgetMsg_OnEnteranceCompetition, 1.0f, 1 );
	Competition::GetInstance().OnEnterance( nIndex );
}

void GLWidgetMsgInterface::OnEnteranceConfirmCompetition( const bool bEnterance )
{
	ALLOWCALL_CHECK( WidgetMsg_OnEnteranceConfirmCompetition, 1.0f, 1 );
	Competition::GetInstance().OnEnteranceConfirm( bEnterance );
}

void GLWidgetMsgInterface::OnClearOfBelongingCompetition(const unsigned int nIndex)
{
	ALLOWCALL_CHECK( WidgetMsg_OnClearOfBelongingCompetition, 1.0f, 1 );
	Competition::GetInstance().OnClearOfBelonging( nIndex );
}

void GLWidgetMsgInterface::OnEnteranceTournament( const DWORD dwSelectID )
{
	ALLOWCALL_CHECK( WidgetMsg_OnEnteranceTournament, 1.0f, 1 );
	Competition::GetInstance().OnEnteranceTournament( dwSelectID );
}

bool GLWidgetMsgInterface::ReqGetBettingInfoCompetition( const DWORD dwScriptID )
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_ReqGetBettingInfoCompetition, 1.0f, 1, false );
	return Competition::GetInstance().ReqGetBettingInfo( dwScriptID );
}

bool GLWidgetMsgInterface::ReqSetTournamentBettingMoney( const DWORD dwIndex, const LONGLONG llMoney, const DWORD dwScriptID )
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_ReqSetTournamentBettingMoney, 1.0f, 1, false );
	return Competition::GetInstance().ReqSetTournamentBettingMoney( dwIndex, llMoney, dwScriptID );
}

bool GLWidgetMsgInterface::ReqCancelTournamentBettingMoney( const int nIndex )
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_ReqCancelTournamentBettingMoney, 1.0f, 1, false );
	return Competition::GetInstance().ReqCancelTournamentBettingMoney( nIndex );
}

bool GLWidgetMsgInterface::ReqObserveTournament(const DWORD dwScriptID, const char* _nameCharacterTargetObserve)
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_ReqObserveTournament, 1.0f, 1, false );
	return Competition::GetInstance().ReqObserveTournament( dwScriptID, _nameCharacterTargetObserve );
}

bool GLWidgetMsgInterface::ReqReferCharCompetition( const int nIndex, const DWORD dwScriptID )
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_ReqReferCharCompetition, 1.0f, 1, false );
	return Competition::GetInstance().ReqReferChar( nIndex, dwScriptID );
}

void GLWidgetMsgInterface::ReqAttendanceTakeReward(const UINT nTaskIndex)
{
	ALLOWCALL_CHECK( WidgetMsg_ReqAttendanceTakeReward, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetAttendance()->ReqAttendanceTakeReward( nTaskIndex );
}

void GLWidgetMsgInterface::ReqRebuild_SelectCache(void)
{
	RebuildCard::GetInstance().ReqSelectCache();
}

bool GLWidgetMsgInterface::ReqChangeRebuildType(bool bKeepOrigin)
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_ReqRebuild_ChangeRebuildType, 0.3f, 1, false );
	RebuildCard::GetInstance().ReqChangeRebuildType(bKeepOrigin);
}

void GLWidgetMsgInterface::ReqRebuildItem_Fix(void)
{
	ALLOWCALL_CHECK( WidgetMsg_ReqRebuildItem, 0.2f, 1 );
	RebuildCard::GetInstance().ReqRebuildItem_Fix();
}

void GLWidgetMsgInterface::ReqRebuildItem(void)
{
	ALLOWCALL_CHECK( WidgetMsg_ReqRebuildItem, 0.2f, 1 );
	RebuildCard::GetInstance().ReqRebuildItem();
}

void GLWidgetMsgInterface::ReqRebuildItem_Cancel(void)
{
	ALLOWCALL_CHECK( WidgetMsg_ReqRebuildItem, 0.2f, 1 );
	RebuildCard::GetInstance().ReqRebuildItem_Cancel();
}

void GLWidgetMsgInterface::ReqRebuildItem_DirectFix(void)
{
	ALLOWCALL_CHECK( WidgetMsg_ReqRebuildItem, 0.2f, 1 );
	RebuildCard::GetInstance().ReqRebuildItem_DirectFix();
}

void GLWidgetMsgInterface::ReqRebuildCloseWindow(void)
{
	ALLOWCALL_CHECK( WidgetMsg_ReqRebuildCloseWindow, 1.0f, 1 );
	RebuildCard::GetInstance().ReqCloseWindow();
}

void GLWidgetMsgInterface::ReqPreWinNumLotto( unsigned int nID, unsigned int turnNum )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqPreWinNumLotto, 0.1f, 1 );
	LottoSystem::GLLottoSystemManClient::Instance()->ReqPreWinNum( nID, turnNum );
}

void GLWidgetMsgInterface::ReqPreWinManListLotto( unsigned int nID, unsigned int turnNum )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqPreWinManListLotto, 0.1f, 1 );
	LottoSystem::GLLottoSystemManClient::Instance()->ReqPreWinManList( nID, turnNum );
}

void GLWidgetMsgInterface::ReqPreAccumulateMoneyLotto( unsigned int nID, unsigned int turnNum )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqPreAccumulateMoneyLotto, 0.1f, 1 );
	LottoSystem::GLLottoSystemManClient::Instance()->ReqPreAccumulateMoney( nID, turnNum );
}

int GLWidgetMsgInterface::ReqBuyLotto( unsigned int nID, LuaTable tbNum, bool bAuto )
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_ReqBuyLotto, 1.0f, 1, 0 );
	return LottoSystem::GLLottoSystemManClient::Instance()->ReqBuyLotto( nID, tbNum, bAuto );
}

int GLWidgetMsgInterface::ReqBuyLottoList ( unsigned int nID, unsigned int nCount )
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_ReqBuyLottoList, 1.0f, 1, 0 );
	return LottoSystem::GLLottoSystemManClient::Instance()->ReqBuyLottoList( nID, nCount );
}

void GLWidgetMsgInterface::ReqPreBuyListLotto( unsigned int nID, unsigned int turnNum )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqPreBuyListLotto, 0.1f, 1 );
	LottoSystem::GLLottoSystemManClient::Instance()->ReqPreBuyList( nID, turnNum );
}

void GLWidgetMsgInterface::ReqConfirmBuyListLotto( unsigned int nID )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqConfirmBuyListLotto, 1.0f, 1 );
	LottoSystem::GLLottoSystemManClient::Instance()->ReqConfirmBuyList( nID );
}

int GLWidgetMsgInterface::GetLottoSystemState ( unsigned int nID )
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_GetLottoSystemState, 1.0f, 1, 0 );
	return LottoSystem::GLLottoSystemManClient::Instance()->GetLottoSystemState( nID );
}

bool GLWidgetMsgInterface::ReqTaxiStation( int nSelectMap, int nSelectStop )
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_ReqTaxiStation, 1.0f, 1, false );
	return GLTaxiStationClient::GetInstance().ReqTaxiStation( nSelectMap, nSelectStop );
}

void GLWidgetMsgInterface::ReqCTFieldQuit(void)
{
	ALLOWCALL_CHECK( WidgetMsg_ReqCTFieldQuit, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetCaptureTheField()->ReqCTFieldQuit();
}

void GLWidgetMsgInterface::ReqKeyFrameInfoFlyCamCtrl()
{
	ALLOWCALL_CHECK( WidgetMsg_ReqKeyFrameInfoFlyCamCtrl, 1.0f, 1 );
	GLFlyCameraControl::GetInstance().ReqKeyFrameInfo();
}

bool GLWidgetMsgInterface::ReqBusStation( const DWORD dwStationID )
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_ReqBusStation, 1.0f, 1, false );
	return GLBusStationClient::GetInstance().ReqBusStation( dwStationID );
}

void GLWidgetMsgInterface::ReqPVEExit()
{
	ALLOWCALL_CHECK( WidgetMsg_ReqPVEExit, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->GetPVEClient()->ReqPVEExit();
}

void GLWidgetMsgInterface::ReqChangeCountry( WORD wContinentID, WORD wCountryID )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqChangeCountry, 1.0f, 1 );

	Country::SCOUNTRY_INFO sCountryInfo;
	sCountryInfo.wContinentID = wContinentID;
	sCountryInfo.wCountryID = wCountryID;

	GLWidgetScript::GetInstance().m_pGaeaClient->ChangeCountry( sCountryInfo );
}

void GLWidgetMsgInterface::ReqJoiinWorldBattleServer()
{
	ALLOWCALL_CHECK( WidgetMsg_ReqJoinWorldBattleServer, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->JoinWorldBattleServer();
}

void GLWidgetMsgInterface::ReqResetCountry( WORD wPosX, WORD wPosY )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqResetCountry, 1.0f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->ReqResetCountry( wPosX, wPosY );
}

void GLWidgetMsgInterface::ReqUseExpCapsuleItem ( WORD wInvenPositionX, WORD wInvenPositionY )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqUseExpCapsuleItem, 1.0f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if ( NULL == pChar )
		return;

	pChar->ReqUseExpCapsuleItem( wInvenPositionX, wInvenPositionY );
}

void GLWidgetMsgInterface::ReqUseExpCompressorItem ( WORD wInvenPositionX, WORD wInvenPositionY )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqUseExpCompressorItem, 1.0f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if ( NULL == pChar )
		return;

	pChar->ReqUseExpCompressorItem( wInvenPositionX, wInvenPositionY );
}

void GLWidgetMsgInterface::ReqUseUnlockCharacterSlotItem ( WORD wInvenPositionX, WORD wInvenPositionY )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqUseExpCompressorItem, 1.0f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if ( NULL == pChar )
		return;

	pChar->ReqUseUnlockCharacterSlotItem( wInvenPositionX, wInvenPositionY );
}

void GLWidgetMsgInterface::ReqCostumeStatCToS()
{
	ALLOWCALL_CHECK( WidgetMsg_ReqCostumeStatCToS, 1.0f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->ReqCostumeStatCToS();
}

void GLWidgetMsgInterface::ReqInvenItemDecompose( WORD wPosX, WORD wPosY )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqInvenItemDecompose, 1.0f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->ReqInvenItemDecompose( wPosX, wPosY );
}

void GLWidgetMsgInterface::ReqTLGrinding(WORD wPosX, WORD wPosY)
{
	ALLOWCALL_CHECK( WidgetMsg_ReqTLGrinding, 1.0f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->ReqTLGrinding();
}

void GLWidgetMsgInterface::ReqItemRepair( LuaTable tbSlot )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqItemRepair, 0.25f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->ReqItemRepair( tbSlot );
}

void GLWidgetMsgInterface::ReqInvenHairColorChange( WORD wHairColor )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqInvenHairColorChange, 1.0f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->ReqInvenHairColorChange( wHairColor );
}

void GLWidgetMsgInterface::ReqGuidanceMapName()
{
	ALLOWCALL_CHECK( WidgetMsg_ReqGuidanceMapName, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->ReqGuidanceMapName();
}

void GLWidgetMsgInterface::ReqCostumeStatRelease( WORD wPosX, WORD wPosY, WORD wHoldPosX, WORD wHoldPosY )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqCostumeStatRelease, 0.5f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->ReqCostumeStatReleaseOK( wPosX, wPosY, wHoldPosX, wHoldPosY );
}

void GLWidgetMsgInterface::NotifyReferCharPublic()
{
	ALLOWCALL_CHECK( WidgetMsg_NotifyReferCharPublic, 0.2f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->NotifyReferCharPublic();
}

void GLWidgetMsgInterface::ReqItemBankInfo()
{
	ALLOWCALL_CHECK( WidgetMsg_ReqItemBankInfo, 0.5f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->ReqItemBankInfo( false );
}

bool GLWidgetMsgInterface::WaitKeyAndTryLogin()
{
	ALLOWCALL_CHECK_VAL( WidgetMsg_WaitKeyAndTryLogin, 0.1f, 1, false );
	return GLLoginClient::GetInstance().WaitKeyAndTryLogin();
}

void GLWidgetMsgInterface::ReqPetSkin( DWORD dwItemID, WORD wPosX, WORD wPosY, WORD wHoldPosX, WORD wHoldPosY )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqPetSkin, 1.0f, 1 );
	GLWidgetScript::GetInstance().m_pGaeaClient->ReqPetSkin( dwItemID, wPosX, wPosY, wHoldPosX, wHoldPosY );
}

void GLWidgetMsgInterface::ReqRanMobileCreateMoney( const char* szNickName )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqRanMobileCreateMoney, 1.0f, 1 );

	if( NULL == szNickName )
		return;

	GLRanMobileClient::GetInstance().ReqCreateMoney( szNickName );
}

void GLWidgetMsgInterface::ReqRanMobileCreateCash( const char* szNickName )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqRanMobileCreateCash, 1.0f, 1 );

	if( NULL == szNickName )
		return;

	GLRanMobileClient::GetInstance().ReqCreateCash( szNickName );
}

void GLWidgetMsgInterface::ReqRanMobileExtendTime()
{
	ALLOWCALL_CHECK( WidgetMsg_ReqRanMobileExtendTime, 1.0f, 1 );
	GLRanMobileClient::GetInstance().ReqExtendTime();
}

void GLWidgetMsgInterface::ReqRanMobileInfo()
{
	ALLOWCALL_CHECK( WidgetMsg_ReqRanMobileInfo, 1.0f, 1 );
	GLRanMobileClient::GetInstance().ReqInfo();
}

void GLWidgetMsgInterface::ReqRanMobileSendJewel( WORD wCount )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqRanMobileSendJewel, 1.0f, 1 );
	GLRanMobileClient::GetInstance().ReqSendJewel( wCount );
}

void GLWidgetMsgInterface::ReqRanMobileGetDailyItem()
{
	ALLOWCALL_CHECK( WidgetMsg_ReqRanMobileGetDailyItem, 1.0f, 1 );
	GLRanMobileClient::GetInstance().ReqGetDailyItem();
}

void GLWidgetMsgInterface::ReqRanMobileChangeItem( WORD wIndex )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqRanMobileChangeItem, 1.0f, 1 );
	GLRanMobileClient::GetInstance().ReqChangeItem( wIndex );
}

void GLWidgetMsgInterface::ReqInvenClubRename( const char* szClubName )
{
	ALLOWCALL_CHECK( WidgetMsg_ReqInvenClubRename, 1.0f, 1 );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->ReqInvenClubRename( szClubName );
}

void GLWidgetMsgInterface::ReqRanMobileInfoAll()
{
	ALLOWCALL_CHECK( WidgetMsg_ReqInvenClubRename, 1.0f, 1 );
	GLRanMobileClient::GetInstance().ReqInfoAll();
}