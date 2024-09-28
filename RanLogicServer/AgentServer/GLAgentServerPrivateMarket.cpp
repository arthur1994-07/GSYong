#include "pch.h"

#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringFormat.h"

#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/Msg/GLContrlPrivateMarket.h"
#include "../../RanLogic/Msg/CharacterMsg.h"
#include "../../RanLogic/Market/GLSearchDelay.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/GLUseFeatures.h"

#include "./GLAgentServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

BOOL GLAgentServer::MsgPrivateMarketItemSearchBasicCA( NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID )
{
	/*
    if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
    {
        sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
        return FALSE;
    }

    GLCharAG* pChar = GetChar( GaeaID );
    if ( NULL == pChar )
    {
        return FALSE;
    }

    // 해당 캐릭터가 개인상점 검색카드를 사용중일 경우에만 검색 가능
    if ( pChar->m_dwUserLvl < USER_USER_GM && pChar->m_bUsedPMarketSeacrhCard == false )
        return FALSE;

    // 캐릭터 단위로 마지막 검색 요청 시간을 저장해 놓고 대기시간이 지났는지 확인한다.
    DWORD dwChaDbNum = pChar->ChaDbNum();
    if ( m_pPrivateMarketSearchDelayMgr && m_pPrivateMarketSearchDelayMgr->IsSearchLimitTime( dwChaDbNum ) )
    {
        return FALSE;
    }

    NET_MSG_PACK* pPacket = ( NET_MSG_PACK* ) pMsg;
    GLMSG::SNETPC_PMARKET_ITEM_SEARCH_BASIC_CA RecvData;
    if ( false == msgpack::BufferToObject( pPacket->DataBuffer(), pPacket->DataSize(), RecvData ) )
    {
        return FALSE;
    }

    // cache 에 요청
    GLMSG::SNETPC_PMARKET_ITEM_SEARCH_BASIC_AH NetMsgAH;
    NetMsgAH.sBasic = RecvData.sBasic;
    NetMsgAH.nChannel = pChar->GetChannel();
    NetMsgAH.wSchool = pChar->GetSchool();
    NetMsgAH.dwChaDbNum = dwChaDbNum;

    msgpack::sbuffer SendBuffer;
    msgpack::pack( SendBuffer, NetMsgAH );
    SENDTOCACHE( NET_MSG_GCTRL_PMARKET_ITEM_SEARCH_BASIC_AH, SendBuffer );
	*/
    return TRUE;
}

BOOL GLAgentServer::MsgPrivateMarketItemNumTotalHA( NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID )
{
	/*
    if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
    {
        sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
        return FALSE;
    }

    GLMSG::SNETPC_PMARKET_ITEM_NUM_TOTAL_HAC* pPacket = ( GLMSG::SNETPC_PMARKET_ITEM_NUM_TOTAL_HAC* ) pMsg;
    GLCharAG* pChar = GetCharByDbNum( pPacket->dwChaDbNum );
    if ( NULL == pChar )
    {
        return FALSE;
    }

    SENDTOCLIENT( pChar->ClientSlot(), pMsg );
	*/
    return TRUE;
}

BOOL GLAgentServer::MsgPrivateMarketItemNumHA( NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID )
{
	/*
    if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
    {
        sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
        return FALSE;
    }

    GLMSG::SNETPC_PMARKET_ITEM_NUM_HAC* pPacket = ( GLMSG::SNETPC_PMARKET_ITEM_NUM_HAC* ) pMsg;
    GLCharAG* pChar = GetCharByDbNum( pPacket->dwChaDbNum );
    if ( NULL == pChar )
    {
        return FALSE;
    }

    SENDTOCLIENT( pChar->ClientSlot(), pMsg );
	*/
    return TRUE;
}

BOOL GLAgentServer::MsgPrivateMarketItemHA( NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID )
{
	/*
    if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
    {
        sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
        return FALSE;
    }

    GLMSG::SNETPC_PMARKET_ITEM_HAC* pPacket = ( GLMSG::SNETPC_PMARKET_ITEM_HAC* ) pMsg;
    GLCharAG* pChar = GetCharByDbNum( pPacket->dwChaDbNum );
    if ( NULL == pChar )
    {
        return FALSE;
    }

    SENDTOCLIENT( pChar->ClientSlot(), pMsg );
	*/
    return TRUE;
}

BOOL GLAgentServer::MsgPrivateMarketItemEndHA( NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID )
{
	/*
    if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
    {
        sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
        return FALSE;
    }

    GLMSG::SNETPC_PMARKET_ITEM_END_HAC* pPacket = ( GLMSG::SNETPC_PMARKET_ITEM_END_HAC* ) pMsg;
    GLCharAG* pChar = GetCharByDbNum( pPacket->dwChaDbNum );
    if ( NULL == pChar )
    {
        return FALSE;
    }

    SENDTOCLIENT( pChar->ClientSlot(), pMsg );
	*/
    return TRUE;
}

BOOL GLAgentServer::MsgPrivateMarketLogCA( NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID )
{
	/*
    if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
    {
        sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
        return FALSE;
    }

    GLCharAG* pChar = GetChar( GaeaID );
    if ( NULL == pChar )
    {
        return FALSE;
    }

    // 해당 캐릭터가 개인상점 검색카드를 사용중일 경우에만 기록 확인 가능
    if ( pChar->m_dwUserLvl < USER_USER_GM && pChar->m_bUsedPMarketSeacrhCard == false )
        return FALSE;

    // 캐릭터 단위로 마지막 로그 요청 시간을 저장해 놓고 대기시간이 지났는지 확인한다.
    DWORD dwChaDbNum = pChar->ChaDbNum();
    if ( m_pPrivateMarketLogDelayMgr && m_pPrivateMarketLogDelayMgr->IsSearchLimitTime( dwChaDbNum ) )
    {
        return FALSE;
    }

    //GLMSG::SNETPC_PMARKET_LOG_CA* pPacket = ( GLMSG::SNETPC_PMARKET_LOG_CA* ) pMsg;

    // cache 에 요청
    GLMSG::SNETPC_PMARKET_LOG_AH NetMsgAH;
    NetMsgAH.dwChaDbNum = dwChaDbNum;
    SENDTOCACHE( &NetMsgAH );
	*/
    return TRUE;
}

BOOL GLAgentServer::MsgPrivateMarketLogNumHA( NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID )
{
	/*
    if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
    {
        sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
        return FALSE;
    }

    GLMSG::SNETPC_PMARKET_LOG_NUM_HAC* pPacket = ( GLMSG::SNETPC_PMARKET_LOG_NUM_HAC* ) pMsg;
    GLCharAG* pChar = GetCharByDbNum( pPacket->dwChaDbNum );
    if ( NULL == pChar )
    {
        return FALSE;
    }

    SENDTOCLIENT( pChar->ClientSlot(), pMsg );
	*/
    return TRUE;
}

BOOL GLAgentServer::MsgPrivateMarketLogHA( NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID )
{
	/*
    if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
    {
        sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
        return FALSE;
    }

    NET_MSG_PACK* pPacket = ( NET_MSG_PACK* ) pMsg;
    GLMSG::SNETPC_PMARKET_LOG_HAC RecvData;
    if ( false == msgpack::BufferToObject( pPacket->DataBuffer(), pPacket->DataSize(), RecvData ) )
    {
        return FALSE;
    }

    GLCharAG* pChar = GetCharByDbNum( RecvData.dwChaDbNum );
    if ( NULL == pChar )
    {
        return FALSE;
    }

    SENDTOCLIENT( pChar->ClientSlot(), pMsg );
	*/
    return TRUE;
}

BOOL GLAgentServer::MsgPrivateMarketLogEndHA( NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID )
{
	/*
    if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
    {
        sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
        return FALSE;
    }

    GLMSG::SNETPC_PMARKET_LOG_END_HAC* pPacket = ( GLMSG::SNETPC_PMARKET_LOG_END_HAC* ) pMsg;
    GLCharAG* pChar = GetCharByDbNum( pPacket->dwChaDbNum );
    if ( NULL == pChar )
    {
        return FALSE;
    }

    SENDTOCLIENT( pChar->ClientSlot(), pMsg );
	*/
    return TRUE;
}

BOOL GLAgentServer::MsgPrivateMarketStateCA( NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID )
{
	/*
    if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
    {
        sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
        return FALSE;
    }

    GLCharAG* pChar = GetCharByGaeaID( GaeaID );
    if ( !pChar )
        return FALSE;

	if (pChar->UserType() < USER_USER_GM)
		return FALSE;

    GLMSG::SNETPC_PMARKET_STATE_AH NetMsgAH;
    NetMsgAH.dwChaDbNum = pChar->m_CharDbNum;
    SENDTOCACHE( &NetMsgAH );
	*/
    return TRUE;
}

BOOL GLAgentServer::MsgPrivateMarketStateHA( NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID )
{
	/*
    if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
    {
        sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
        return FALSE;
    }

    GLMSG::SNETPC_PMARKET_STATE_HAC* pPacket = ( GLMSG::SNETPC_PMARKET_STATE_HAC* ) pMsg;
    GLCharAG* pChar = GetCharByDbNum( pPacket->dwChaDbNum );
    if ( NULL == pChar )
    {
        return FALSE;
    }

    pPacket->fSearchDelay = static_cast< float >( private_market::GLPrivateMarketDefine::Instance()->GetSearchDelayMillisecond() / 1000 );
    pPacket->fLogDelay = static_cast< float >( private_market::GLPrivateMarketDefine::Instance()->GetLogDelayMillisecond() / 1000 );
    pPacket->bSaleCommission = private_market::GLPrivateMarketDefine::Instance()->GetSaleCommission();

    SENDTOCLIENT( pChar->ClientSlot(), pMsg );
	*/
    return TRUE;
}

BOOL GLAgentServer::MsgPrivateMarketItemSearchNameCA( NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID )
{
	/*
    if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
    {
        sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
        return FALSE;
    }

    GLCharAG* pChar = GetChar( GaeaID );
    if ( NULL == pChar )
    {
        return FALSE;
    }

    // 해당 캐릭터가 개인상점 검색카드를 사용중일 경우에만 검색 가능
    if ( pChar->m_dwUserLvl < USER_USER_GM && pChar->m_bUsedPMarketSeacrhCard == false )
        return FALSE;


    // 캐릭터 단위로 마지막 검색 요청 시간을 저장해 놓고 대기시간이 지났는지 확인한다.
    DWORD dwChaDbNum = pChar->ChaDbNum();
    if ( m_pPrivateMarketSearchDelayMgr && m_pPrivateMarketSearchDelayMgr->IsSearchLimitTime( dwChaDbNum ) )
    {
        return FALSE;
    }

    GLMSG::SNETPC_PMARKET_ITEM_SEARCH_NAME_CA* pPacket = ( GLMSG::SNETPC_PMARKET_ITEM_SEARCH_NAME_CA* ) pMsg;

    std::string strItemName( pPacket->szItemName );
    sc::string::trim( strItemName );

    GLMSG::SNETPC_PMARKET_ITEM_SEARCH_NAME_AH NetMsgAH;
    NetMsgAH.wPage = pPacket->wPage;
    NetMsgAH.nSortOption = pPacket->nSortOption;
    NetMsgAH.nSortMethod = pPacket->nSortMethod;
    NetMsgAH.nChannel = pChar->GetChannel();
    NetMsgAH.wSchool = pChar->GetSchool();
    NetMsgAH.dwChaDbNum = dwChaDbNum;
    NetMsgAH.SetName( strItemName.c_str() );

    SENDTOCACHE( &NetMsgAH );
	*/
    return TRUE;
}

BOOL GLAgentServer::MsgPrivateMarketOptionCA( NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID )
{
    if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
    {
        sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
        return FALSE;
    }

    GLCharAG* pChar = GetCharByGaeaID( GaeaID );
    if ( !pChar )
        return FALSE;

	if ( pChar->UserType() < USER_USER_GM )
		return FALSE;

    GLMSG::SNETPC_PMARKET_OPTION_CA* pPacket = ( GLMSG::SNETPC_PMARKET_OPTION_CA* ) pMsg;

	// 전 채널에 알림
	GLMSG::SNETPC_PMARKET_OPTION_AF NetMsgAF;
	NetMsgAF.ChaNum = pChar->ChaDbNum();
	NetMsgAF.eOptionType = pPacket->eOptionType;
	NetMsgAF.fValue = pPacket->fValue;
	NetMsgAF.bValue = pPacket->bValue;
	NetMsgAF.SearchOption = pPacket->SearchOption;
	SENDTOALLCHANNEL( &NetMsgAF );

    return TRUE;
}