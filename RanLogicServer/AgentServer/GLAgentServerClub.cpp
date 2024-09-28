#include "../pch.h"
#include "../../SigmaCore/Util/DateTime.h"
#include "../../MfcExLib/RanFilter.h"

#include "../../RanLogic/Msg/ClubMsg.h"
#include "../../RanLogic/Msg/FriendMsg.h"
#include "../../RanLogic/Msg/JsonMsgDefine.h"
#include "../../RanLogic/Msg/WorldBattleMsg.h"

#include "../Database/DBAction/DbActionGameClub.h"
#include "../Club/GLClubMan.h"
#include "./GLAgentServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

GLClubManAgent* GLAgentServer::GetClubMan()
{ 
    return m_pClubMan;
}

std::tr1::shared_ptr<GLClubAgent> GLAgentServer::GetClub(DWORD ClubId)
{
    return m_pClubMan->GetClub(ClubId);
}

std::tr1::shared_ptr<GLClubAgent> GLAgentServer::GetClub(const std::string& ClubName)
{
    return m_pClubMan->GetClub(ClubName);
}

void GLAgentServer::SetCurrentField(GLCharAG* pChar, DWORD dwSvr, const MapID& mapID)
{
    if (pChar)
        pChar->SetCurrentField(pChar->m_nChannel, dwSvr, mapID);
}

BOOL GLAgentServer::MsgClubIncomeReNewCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
    // GLMSG::NET_CLUB_INCOME_RENEW_CA NET_MSG_CLUB_INCOME_RENEW_CA
    GLCharAG* pChar = GetChar(dwGaeaID);
    if (!pChar)
	{
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgClubIncomeReNewCA ", dwGaeaID));
        return FALSE;
	}

    DWORD ClubDbNum = pChar->m_ClubDbNum;
    std::tr1::shared_ptr<GLClubAgent> pClub = GetClub(ClubDbNum);
    if (!pClub)
        return FALSE;

    if (!pClub->HaveMoneyAuthority(pChar->ChaDbNum()))
        return FALSE;
        
    //GLMSG::SNET_CLUB_INCOME_UP
    if (pClub->GetIncomeMoney() <= 0)
        return FALSE;

    if (!pClub->IsValidStorage())
        return FALSE;

    LONGLONG IncomeMoney = pClub->GetIncomeMoney();
    if (IncomeMoney < 0)
    {
        sc::writeLogError(
            sc::string::format(
                "MsgClubIncomeReNewCA underflow. %1%", IncomeMoney));
        return FALSE;
    }

    pClub->AddStorageMoney(IncomeMoney);
    pClub->SetIncomeMoney(0);

    std::string ClubLog(
        sc::string::format(
            ID2GAMEINTEXT("CLUB_LOG_14"),
            pChar->GetChaName(),
            IncomeMoney,
            pClub->GetStorageMoney()));
    SENDCLUBLOG(ClubDbNum, club::LOG_CLUB_STORAGE, ClubLog);

    sc::writeLogInfo(
        sc::string::format(
            "MsgClubIncomeMoneyUp ClubID %1% IncomeMoney %2%",
            ClubDbNum,
            IncomeMoney));

    // Db에 저장.		
    AddGameDbJob(
        db::DbActionPtr(
            new db::ClubIncomeMoneySet(
                ClubDbNum,
                pClub->GetIncomeMoney())));

    AddGameDbJob(
        db::DbActionPtr(
            new db::ClubMoneySet(
                ClubDbNum,
                pClub->GetStorageMoney())));    

	// 통계 및 로그 수정
    m_pItemLimit->LogMoneyExchange(gamelog::ID_CLUB, 0, gamelog::ID_CLUB, ClubDbNum, IncomeMoney, MONEY_ROUTE_CLUB_INCOME, ( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap(), LOG_MONEY_CLUB_GUIDANCE_COMMISSION);
	// 통계 및 로그 수정
    m_pItemLimit->LogMoneyExchange(gamelog::ID_CLUB, 0, gamelog::ID_CLUB, ClubDbNum, pClub->GetStorageMoney(), MONEY_ROUTE_CLUB_TOTAL_MONEY, ( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap(), LOG_MONEY_CLUB_STORAGE_CURRENT);    

//     sc::writeLogInfo(
//         sc::string::format(
//             "ClubIncomeMoney ServerClubInComeDn ClubID %1% ChaDbNum %2% IncomeMoney %3%",
//             ClubDbNum,
//             pChar->m_CharDbNum,
//             IncomeMoney));
    
    // 클럽원에게 통보
    GLMSG::SNET_CLUB_STORAGE_UPDATE_MONEY NetMsgClt(pClub->GetStorageMoney(), pClub->GetIncomeMoney());
    pClub->SendClubClient(&NetMsgClt);
    return TRUE;
}

BOOL GLAgentServer::MsgClubGetStorageCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
    // NET_CLUB_GETSTORAGE_CA NET_MSG_CLUB_GETSTORAGE_CA
    GLCharAG* pChar = GetChar(dwGaeaID);
    if (!pChar)
        return FALSE;

	GLAGLandMan* pLandMan = GetLand( pChar->GetCurrentMap().getGaeaMapID() );
	if ( !pLandMan )
	{
		return FALSE;
	}

	// http://crm.mincoms.com/Ticket/TicketView/275
	// 클럽락커 기능 수정 요청
	if ( pLandMan->IsClubBattleMap() )
	{
		GLGuidance* pGuid = m_pGuidance->Find ( pLandMan->GetGuidance() );
		if ( pGuid && pGuid->m_bBattle )
		{
			return FALSE;
		}
	}

    std::tr1::shared_ptr<GLClubAgent> pClub = GetClub(pChar->GetClubDbNum());
    if (!pClub)
        return FALSE;

    if (!pClub->HaveStorageAuthority(pChar->ChaDbNum()))
        return FALSE;

    boost::logic::tribool State = pClub->IsValidStorage();
    if (State == true)
    {
        MsgClubStorage(pClub, pChar->ClientSlot());
    }
    else if (State == false)
    {
        //가져오고 있는중...세팅
        pClub->SetValidStorage(boost::logic::indeterminate);

        AddGameAdoJob(
            db::DbActionPtr(
                new db::ClubStorageGet(dwClientID, pChar->ChaDbNum(), pChar->GetClubDbNum())));	
    }
    else
    {
        //boost::logic::indeterminate
        //DB 에서 가져오고 있는중
    }
    return TRUE;
}

void GLAgentServer::MsgClubStorage(std::tr1::shared_ptr<GLClubAgent> pClub, DWORD ClientSlot)
{
    if (pClub)
        pClub->SendClubStorage(ClientSlot);
/*
    //if (pClub->MasterCharDbNum() != m_CharDbNum)
    //    return S_OK;

    // 클럽 창고 정보 리샛
    GLMSG::SNET_CLUB_STORAGE_RESET NetMsgClubStorageReset;
    SENDTOCLIENT(ClientSlot, &NetMsgClubStorageReset);

    // 창고의 기본 정보 전송
    GLMSG::SNET_CLUB_STORAGE_UPDATE_MONEY NetMsgMoney(pClub->GetStorageMoney(), pClub->GetIncomeMoney());
    SENDTOCLIENT(ClientSlot, &NetMsgMoney);

    // 아이템을 하나씩 전송
    GLMSG::SNET_CLUB_GETSTORAGE_ITEM NetMsgItem;

    // 클라이언트에 창고 전송
    // 루프를 돌면서 하나씩 전송
    DWORD dwRANK = pClub->GetRank();
    if (dwRANK >= MAX_CLUBSTORAGE)
        dwRANK = MAX_CLUBSTORAGE-1;

    for (DWORD i=0; i<=dwRANK; ++i)
    {
        NetMsgItem.dwChannel = i;
        const GLInventory::CELL_MAP* pInvenList = pClub->m_cStorage[i].GetItemList();

        GLInventory::CELL_MAP_CITER iter = pInvenList->begin();
        GLInventory::CELL_MAP_CITER iter_end = pInvenList->end();
        for ( ; iter!=iter_end; ++iter)
        {
            SINVENITEM* pInvenItem = iter->second;
            NetMsgItem.Data = *pInvenItem;
            SENDTOCLIENT(ClientSlot, &NetMsgItem);
        }
    }

    // 클럽의 수익 갱신 요청
    //MsgReqClubInComeReNew(NULL);
    return S_OK;
*/
}

BOOL GLAgentServer::MsgClubStorageGetDb( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	if ( DBACTION_CLIENT_ID != dwClientID || GAEAID_NULL != dwGaeaID )
	{
		return FALSE;
	}

	NET_MSG_PACK* pPacket = ( NET_MSG_PACK* ) nmg;    
	GLMSG::SNET_CLUB_STORAGE_GET_DB Data;
	if ( !msgpack::BufferToObject( pPacket->DataBuffer(), pPacket->DataSize(), Data ) )
		return FALSE;

    std::tr1::shared_ptr< GLClubAgent > pClub = GetClub( Data.m_dwClubID );
    if ( !pClub )
    {
        return FALSE;
    }

	pClub->InsertItem( Data.sItem );

    return TRUE;    
}

BOOL GLAgentServer::MsgClubStorageToInvenCA( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
    NET_MSG_PACK* pPacket = ( NET_MSG_PACK* ) nmg;
    GLMSG::NET_CLUB_STORAGE_TO_INVEN_CA Data;
    if ( !msgpack::BufferToObject( pPacket->DataBuffer(), pPacket->DataSize(), Data ) )
        return FALSE;

    // crc check
    if ( pPacket->Crc() != sc::string::getCrc32( pPacket->DataBuffer(), pPacket->DataSize() ) )
    {
        sc::writeLogError( "MsgClubStorageToHoldCA CRC" );
        return FALSE;
    }

    if ( Data.m_Channel >= MAX_CLUBSTORAGE )
        return FALSE;

    GLCharAG* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;

	GLAGLandMan* pLandMan = GetLand( pChar->GetCurrentMap().getGaeaMapID() );
	if ( !pLandMan )
	{
		return FALSE;
	}

	// http://crm.mincoms.com/Ticket/TicketView/275
	// 클럽락커 기능 수정 요청
	if ( pLandMan->IsClubBattleMap() )
	{
		GLGuidance* pGuid = m_pGuidance->Find( pLandMan->GetGuidance() );
		if ( pGuid && pGuid->m_bBattle )
		{
			return FALSE;
		}
	}

    std::tr1::shared_ptr< GLClubAgent > spClub = GetClub( pChar->GetClubDbNum() );
    if ( !spClub )
        return FALSE;

    DWORD ChaDbNum = pChar->ChaDbNum();
    if ( !spClub->HaveStorageAuthority( ChaDbNum ) )
        return FALSE;

	// 클럽창고 사용 요청, 사용자 설정한다. 이 함수 콜한 후에 다른데서 리턴 FALSE 한다면 꼭 ResetClubStorageUser 함수 호출해줘야 한다.
	if ( false == spClub->ReqClubStorageUse( pChar->ChaDbNum() ) )
	{
		std::string ClubStorageUser = spClub->GetClubStorageUserName();
		if ( false == ClubStorageUser.empty() )
		{
			GLMSG::SNET_CLUB_STORAGE_USER_FB SendData( ClubStorageUser );
			msgpack::sbuffer SendBufferAC;
			msgpack::pack( SendBufferAC, SendData );
			SENDTOCLIENT( pChar->ClientSlot(), NET_MSG_GCTRL_CLUB_STORAGE_USER_FB, SendBufferAC );
		}
		return FALSE;
	}

    DWORD dwChannel = Data.m_Channel;

    //	요청한 아이템 유효성 검사.
    const SINVENITEM* pClubInvenItem = spClub->GetItem( dwChannel, Data.m_HoldPosX, Data.m_HoldPosY );
    if ( !pClubInvenItem )
	{
		spClub->ResetClubStorageUser();
        return FALSE;
	}

    // 클럽 아이템 백업
    SINVENITEM InvenBackUp = *pClubInvenItem;
    
    // 아이템을 삭제
    if ( false == spClub->DeleteItem( dwChannel, InvenBackUp.wPosX, InvenBackUp.wPosY ) )
    {
		spClub->ResetClubStorageUser();
		return FALSE;
    }

	// 클럽 로그
	// [%1%]님이 [%2%]X%3% 을(를) 반출하였습니다.
	std::string ClubLog( 
		sc::string::format( 
		ID2GAMEINTEXT("CLUB_LOG_20"), 
		pChar->GetChaName(), 
		pClubInvenItem->sItemCustom.GETNAME(), 
		pClubInvenItem->TurnNum() ) );
	SENDCLUBLOG( spClub->DbNum(), club::LOG_CLUB_STORAGE, ClubLog );

    // 클럽창고에 있었던 아이템을 손에 듬. 요청
    GLMSG::NET_CLUB_STORAGE_TO_INVEN_AF SendData( ChaDbNum, spClub->DbNum(), InvenBackUp.sItemCustom, Data.m_PosX, Data.m_PosY );
    msgpack::sbuffer SendBufferAF;
    msgpack::pack( SendBufferAF, SendData );
    SENDTOFIELDSVR( pChar->GetChannel(), pChar->m_dwCurFieldSvr, NET_MSG_CLUB_STORAGE_TO_INVEN_AF, SendBufferAF, true );

    // 창고 아이템 삭제 됨
    GLMSG::SNET_CLUB_STORAGE_DELETE MsgAC;
    MsgAC.dwChannel = dwChannel;
    MsgAC.wPosX = InvenBackUp.wPosX;
    MsgAC.wPosY = InvenBackUp.wPosY;
    spClub->SendClubClient( &MsgAC );
    return S_OK;
}

BOOL GLAgentServer::MsgClubStorageRestoreFA( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
    NET_MSG_PACK* pPacket = ( NET_MSG_PACK* ) nmg;
    GLMSG::NET_CLUB_STORAGE_RESTORE_FA RecvData;
    if ( !msgpack::BufferToObject( pPacket->DataBuffer(), pPacket->DataSize(), RecvData ) )
        return FALSE;

    // Crc check
    if ( sc::string::getCrc32( pPacket->DataBuffer(), pPacket->DataSize() ) != pPacket->Crc() )
    {
        std::string ErrorLog( 
            sc::string::format( 
                "MsgClubStorageRestoreFA CRC ERROR. ClubDbNum %1% ChaDbNum %2% Item %3%/%4% Amount %5%", 
                RecvData.m_ClubDbNum, RecvData.m_ChaDbNum, RecvData.m_Item.Mid(), RecvData.m_Item.Sid(), RecvData.m_Item.TurnNum()));
        sc::writeLogError( ErrorLog );
        return FALSE;
    }

    // Magic num check
    if ( RecvData.m_MagicNum != GLMSG::NET_CLUB_STORAGE_RESTORE_FA::MAGIC_NUM )
    {
        std::string ErrorLog( 
            sc::string::format( 
            "MsgClubStorageRestoreFA MAGIC_NUM ERROR. ClubDbNum %1% ChaDbNum %2% Item %3%/%4% Amount %5%",
            RecvData.m_ClubDbNum, RecvData.m_ChaDbNum, RecvData.m_Item.Mid(), RecvData.m_Item.Sid(), RecvData.m_Item.TurnNum() ) );
        sc::writeLogError( ErrorLog );
        return FALSE;
    }
    
    std::tr1::shared_ptr< GLClubAgent > pClub = GetClub( RecvData.m_ClubDbNum );
    if ( !pClub )
    {
        sc::writeLogError( 
            sc::string::format( 
                "MsgClubStorageRestoreFA ClubDbNum %1% ChaDbNum %2% Item %3%/%4%", 
                RecvData.m_ClubDbNum, RecvData.m_ChaDbNum, RecvData.m_Item.GetNativeID().Mid(), RecvData.m_Item.GetNativeID().Sid() ) );
        return FALSE;
    }

	DWORD CurrentClubStorageUser = pClub->GetClubStorageUser();
	if ( GAEAID_NULL != CurrentClubStorageUser && RecvData.m_ChaDbNum != CurrentClubStorageUser )
	{
		sc::writeLogError( 
			sc::string::format( 
			"MsgClubStorageRestoreFA(RecvData.m_ChaDbNum != CurrentClubStorageUser) can't insert item ClubDbNum %1% ChaDbNum %2% Item %3%/%4% TurnNum %5%", 
			RecvData.m_ClubDbNum, RecvData.m_ChaDbNum, RecvData.m_Item.GetNativeID().Mid(), RecvData.m_Item.GetNativeID().Sid(), RecvData.m_Item.TurnNum() ) );
		return FALSE;
	}
    
    if ( !pClub->InsertItem( RecvData.m_Item ) )
    {
		std::string strGuid = sc::string::uuidToString( RecvData.m_Item.GetGuid() );
        sc::writeLogError( 
            sc::string::format( 
                "MsgClubStorageRestoreFA can't insert item ClubDbNum %1% ChaDbNum %2% Item %3%/%4% TurnNum %5% guid %6%", 
                RecvData.m_ClubDbNum, RecvData.m_ChaDbNum, RecvData.m_Item.GetNativeID().Mid(), RecvData.m_Item.GetNativeID().Sid(), RecvData.m_Item.TurnNum(), strGuid.c_str() ) );
        return FALSE;
    }
    else
    {        
        // [%1%]X%2% 이(가) 복구 되었습니다.
        std::string ClubLog( 
            sc::string::format( 
            ID2GAMEINTEXT( "CLUB_LOG_21" ), 
            RecvData.m_Item.GETNAME(), 
            RecvData.m_Item.TurnNum() ) );
        SENDCLUBLOG( pClub->DbNum(), club::LOG_CLUB_STORAGE, ClubLog );

        return TRUE;
    }
}

BOOL GLAgentServer::MsgClubStorageExInvenCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
    // Note : agent와 field 간 통신으로 아이템을 교환해야해서, 안정성 저하 문제로 막음.
    return FALSE;

    //// NET_MSG_CLUB_STORAGE_EX_HOLD_CA
    //// GLMSG::NET_CLUB_STORAGE_EX_HOLD_CA* pPacket = (GLMSG::NET_CLUB_STORAGE_EX_HOLD_CA*) nmg;
    //const GLCharAG* pChar = GetChar(dwGaeaID);
    //if (!pChar)
    //    return FALSE;

    //NET_MSG_PACK* pPacket = (NET_MSG_PACK*) nmg;
    //if (pPacket->Crc() != sc::string::getCrc32(pPacket->DataBuffer(), pPacket->DataSize()))
    //{
    //    sc::writeLogError("MsgClubStorageExHoldCA CRC");
    //    return FALSE;
    //}

    //GLMSG::NET_CLUB_STORAGE_EX_INVEN_CA RecvData;
    //if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
    //{
    //    sc::writeLogError("MsgClubStorageExInvenCA BufferToObject");
    //    return FALSE;
    //}

    //DWORD ChaDbNum = pChar->ChaDbNum();

    //std::tr1::shared_ptr<GLClubAgent> pClub = GetClub(pChar->GetClubDbNum());
    //if (!pClub)
    //    return FALSE;

    //if (!pClub->HaveStorageAuthority(pChar->ChaDbNum()))
    //    return FALSE;

    //const SINVENITEM* pClubItem = pClub->GetItem(RecvData.m_Channel, RecvData.m_ClubStoragePosX, RecvData.m_ClubStoragePosY);
    //if (!pClubItem)
    //    return FALSE;

    //const SITEM* pItemData = GLogicData::GetInstance().GetItem(pClubItem->GetNativeID());
    //if (!pItemData)
    //    return FALSE;

    //if (!pItemData->CanExchange())
    //    return FALSE;

    //SINVENITEM InvenBackUp = *pClubItem;

    //// 아이템을 삭제
    //pClub->DeleteItem(RecvData.m_Channel, InvenBackUp.wPosX, InvenBackUp.wPosY);

    //// 창고 아이템 삭제 됨
    //GLMSG::SNET_CLUB_STORAGE_DELETE NetMsg;
    //NetMsg.dwChannel = RecvData.m_Channel;
    //NetMsg.wPosX     = InvenBackUp.wPosX;
    //NetMsg.wPosY     = InvenBackUp.wPosY;
    //pClub->SendClubClient(&NetMsg);

    //// 클럽 로그
    //// [%1%]님이 [%2%]X%3% 을(를) 반출하였습니다.
    //std::string ClubLog(
    //    sc::string::format(
    //        ID2GAMEINTEXT("CLUB_LOG_20"),
    //        pChar->GetChaName(),
    //        pItemData->GetNameStr(),
    //        InvenBackUp.sItemCustom.TurnNum()));
    //SENDCLUBLOG(pClub->DbNum(), club::LOG_CLUB_STORAGE, ClubLog);
    //
    //// 클럽창고에 있었던 아이템을 손에 든 아이템과 교체 요청
    //GLMSG::NET_CLUB_STORAGE_EX_INVEN_AF MsgAF(ChaDbNum, pClub->DbNum(), InvenBackUp.sItemCustom);
    //msgpack::sbuffer SendBuffer;
    //MsgAF.m_Channel = RecvData.m_Channel;
    //MsgAF.m_ClubStoragePosX = RecvData.m_ClubStoragePosX;
    //MsgAF.m_ClubStoragePosY = RecvData.m_ClubStoragePosY;
    //MsgAF.m_InvenPosX = RecvData.m_InvenPosX;
    //MsgAF.m_InvenPosY = RecvData.m_InvenPosY;
    //msgpack::pack(SendBuffer, MsgAF);
    //SENDTOFIELDSVR(pChar->GetChannel(), pChar->GetCurrentFieldServer(), NET_MSG_CLUB_STORAGE_EX_INVEN_AF, SendBuffer, true);

    //return TRUE;
}

BOOL GLAgentServer::MsgClubStorageExClubStorageCA( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
    const GLCharAG* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;

	GLAGLandMan* pLandMan = GetLand( pChar->GetCurrentMap().getGaeaMapID() );
	if ( !pLandMan )
	{
		return FALSE;
	}

	// http://crm.mincoms.com/Ticket/TicketView/275
	// 클럽락커 기능 수정 요청
	if ( pLandMan->IsClubBattleMap() )
	{
		GLGuidance* pGuid = m_pGuidance->Find ( pLandMan->GetGuidance() );
		if ( pGuid && pGuid->m_bBattle )
		{
			return FALSE;
		}
	}

    NET_MSG_PACK* pPacket = ( NET_MSG_PACK* ) nmg;
    if ( pPacket->Crc() != sc::string::getCrc32( pPacket->DataBuffer(), pPacket->DataSize() ) )
    {
        sc::writeLogError( "MsgClubStorageExClubStorageCA CRC" );
        return FALSE;
    }

    GLMSG::NET_CLUB_STORAGE_EX_CLUB_STORAGE_CA RecvData;
    if ( !msgpack::BufferToObject( pPacket->DataBuffer(), pPacket->DataSize(), RecvData ) )
    {
        sc::writeLogError( "MsgClubStorageExClubStorageCA BufferToObject" );
        return FALSE;
    }

	// 채널까지 확인해야, 다른 채널인데 같은 좌표라고 못하는 문제 수정
    if ( RecvData.m_PosX == RecvData.m_HoldPosX && RecvData.m_PosY == RecvData.m_HoldPosY && RecvData.m_Channel == RecvData.m_HoldChannel)
        return FALSE;

    DWORD ChaDbNum = pChar->ChaDbNum();

    std::tr1::shared_ptr< GLClubAgent > pClub = GetClub( pChar->GetClubDbNum() );
    if ( !pClub )
        return FALSE;

    if ( !pClub->HaveStorageAuthority( pChar->ChaDbNum() ) )
        return FALSE;

	// 클럽창고 사용 요청, 사용자 설정한다. 아래 autoreset 객체가 사라질 때 ResetClubStorageUser 함수 호출된다.
	if ( false == pClub->ReqClubStorageUse( ChaDbNum ) )
	{
		std::string ClubStorageUser = pClub->GetClubStorageUserName();
		if ( false == ClubStorageUser.empty() )
		{
			GLMSG::SNET_CLUB_STORAGE_USER_FB SendData( ClubStorageUser );
			msgpack::sbuffer SendBufferAC;
			msgpack::pack( SendBufferAC, SendData );
			SENDTOCLIENT( pChar->ClientSlot(), NET_MSG_GCTRL_CLUB_STORAGE_USER_FB, SendBufferAC );
		}
		return FALSE;
	}

	// MsgClubStorageToInvenCA 함수에서는 중간에 리턴하면 일일이 ResetClubStorageUser 을 호출해야 했는데, 
	// 그 이유는 클럽창고->인벤토리 로 아이템 이동 메시지의 끝이 필드서버까지 갔다와야 되서이다. 
	// MsgClubStorageExClubStorageCA 함수의 경우 에이전트에서 끝이 나기 때문에 일일이 ResetClubStorageUser 를 호출하지않고 AutoResetClubStorageUser 사용한다.
	// MsgClubStorageExClubStorageCA 함수 리턴하면 클럽창고 사용자 자동 리셋된다. 소멸자에서 호출됨
	AutoResetClubStorageUser autoreset( pClub );

    const SINVENITEM* pClubItem = pClub->GetItem( RecvData.m_Channel, RecvData.m_PosX, RecvData.m_PosY );
    if ( !pClubItem )
        return FALSE;

	if ( !( pClubItem->sItemCustom.IsExchange() != NULL ) )
        return FALSE;

    const SINVENITEM* pClubHoldItem = pClub->GetItem( RecvData.m_HoldChannel, RecvData.m_HoldPosX, RecvData.m_HoldPosY );
    if ( !pClubHoldItem )
        return FALSE;

	if ( !( pClubHoldItem->sItemCustom.IsExchange() != NULL ) )
        return FALSE;

	// SITEM 확인
	const SITEM* pInvenItemData = GLogicData::GetInstance().GetItem( pClubItem->sItemCustom.GetNativeID() );
	const SITEM* pHoldItemData = GLogicData::GetInstance().GetItem( pClubHoldItem->sItemCustom.GetNativeID() );
	if ( !pInvenItemData || !pHoldItemData )
	{
		return FALSE;
	}

    SINVENITEM sInvenBackUp = *pClubItem;
    WORD wPosX = sInvenBackUp.wPosX;
    WORD wPosY = sInvenBackUp.wPosY;

    SINVENITEM sHoldBackUp = *pClubHoldItem;
    WORD wHoldPosX = sHoldBackUp.wPosX;
    WORD wHoldPosY = sHoldBackUp.wPosY;

	SINVENITEM* pINSERT_ITEM = NULL;
	SINVENITEM* pINSERT_HOLD_ITEM = NULL;

	// #item
	// club locker <-> club locker

	// 아이템 확인을 다 했기 때문에 아래 swap 할 때 에러가 발생하면 안된다. 
	// 삭제, 삽입할 때마다 모두 예외처리를 추가해보았으나, 마지막 단계에서 처리할 예외가 너무 많다. 
	// 그래서 swap 시작전에 인벤토리 백업, 문제 발생하면 복원도 고려해 보았으나
	// 복사에 대한 비용이 너무 크다.
	// 테스트 기간동안 에러 로그 확인하고 상황에 따른 예외 처리 추가하자.
	{
		if ( !pClub->DeleteItem( RecvData.m_Channel, wPosX, wPosY ) )
		{
			sc::writeLogError( sc::string::format( 
				"critical error MsgClubStorageExClubStorageCA, deleteitem fail. clubnum: %1% chanum: %2% guid: %3%", 
				pChar->GetClubDbNum(), pChar->ChaDbNum(), sc::string::uuidToString( sInvenBackUp.GetGuid() ).c_str() ) );
			return FALSE;
		}

		if ( !pClub->DeleteItem( RecvData.m_HoldChannel, wHoldPosX, wHoldPosY ) )
		{
			sc::writeLogError( sc::string::format( 
				"critical error MsgClubStorageExClubStorageCA, deleteitem fail. clubnum: %1% chanum: %2% guid: %3%", 
				pChar->GetClubDbNum(), pChar->ChaDbNum(), sc::string::uuidToString( sHoldBackUp.GetGuid() ).c_str() ) );
			return FALSE;
		}

		if ( !pClub->IsInsertable( RecvData.m_Channel, wPosX, wPosY ) || !pClub->IsInsertable( RecvData.m_HoldChannel, wHoldPosX, wHoldPosY ) )
		{
			sc::writeLogError( sc::string::format( 
				"critical error MsgClubStorageExClubStorageCA, IsInsertable fail. clubnum: %1% chanum: %2% guid: %3% guid: %4%", 
				pChar->GetClubDbNum(), pChar->ChaDbNum(), sc::string::uuidToString( sInvenBackUp.GetGuid() ).c_str(), sc::string::uuidToString( sHoldBackUp.GetGuid() ).c_str() ) );
			return FALSE;
		}

		pINSERT_ITEM = pClub->InsertItemNew( sHoldBackUp.CustomItem(), RecvData.m_Channel, wPosX, wPosY, false );
		if ( !pINSERT_ITEM )
		{
			sc::writeLogError( sc::string::format( 
				"critical error MsgClubStorageExClubStorageCA, InsertItemNew fail. clubnum: %1% chanum: %2% guid: %3%", 
				pChar->GetClubDbNum(), pChar->ChaDbNum(), sc::string::uuidToString( sHoldBackUp.GetGuid() ).c_str() ) );
			return FALSE;
		}

		pINSERT_HOLD_ITEM = pClub->InsertItemNew( sInvenBackUp.CustomItem(), RecvData.m_HoldChannel, wHoldPosX, wHoldPosY, false );
		if ( !pINSERT_HOLD_ITEM )
		{
			sc::writeLogError( sc::string::format( 
				"critical error MsgClubStorageExClubStorageCA, InsertItemNew fail. clubnum: %1% chanum: %2% guid: %3%", 
				pChar->GetClubDbNum(), pChar->ChaDbNum(), sc::string::uuidToString( sInvenBackUp.GetGuid() ).c_str() ) );
			return FALSE;
		}
	}

	GLMSG::SNET_CLUB_STORAGE_DEL_AND_INS NetMsg_Inven_Del_Insert;
	NetMsg_Inven_Del_Insert.dwChannel = RecvData.m_Channel;
	NetMsg_Inven_Del_Insert.wDelX = wPosX;
	NetMsg_Inven_Del_Insert.wDelY = wPosY;
	NetMsg_Inven_Del_Insert.sInsert = *pINSERT_ITEM;
	pClub->SendClubClient( &NetMsg_Inven_Del_Insert );

	NetMsg_Inven_Del_Insert.dwChannel = RecvData.m_HoldChannel;
	NetMsg_Inven_Del_Insert.wDelX = wHoldPosX;
	NetMsg_Inven_Del_Insert.wDelY = wHoldPosY;
	NetMsg_Inven_Del_Insert.sInsert = *pINSERT_HOLD_ITEM;
	pClub->SendClubClient(&NetMsg_Inven_Del_Insert);

    return TRUE;
}

BOOL GLAgentServer::MsgClubStorageToClubStorageCA( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
    const GLCharAG* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;

	GLAGLandMan* pLandMan = GetLand( pChar->GetCurrentMap().getGaeaMapID() );
	if ( !pLandMan )
	{
		return FALSE;
	}

	// http://crm.mincoms.com/Ticket/TicketView/275
	// 클럽락커 기능 수정 요청
	if ( pLandMan->IsClubBattleMap() )
	{
		GLGuidance* pGuid = m_pGuidance->Find ( pLandMan->GetGuidance() );
		if ( pGuid && pGuid->m_bBattle )
		{
			return FALSE;
		}
	}

    NET_MSG_PACK* pPacket = ( NET_MSG_PACK* ) nmg;
    if ( pPacket->Crc() != sc::string::getCrc32( pPacket->DataBuffer(), pPacket->DataSize() ) )
    {
        sc::writeLogError( "MsgClubStorageToClubStorageCA CRC" );
        return FALSE;
    }

    GLMSG::NET_CLUB_STORAGE_TO_CLUB_STORAGE_CA RecvData;
    if (!msgpack::BufferToObject( pPacket->DataBuffer(), pPacket->DataSize(), RecvData ) )
    {
        sc::writeLogError( "MsgClubStorageToClubStorageCA BufferToObject" );
        return FALSE;
    }

    if ( RecvData.m_PosX == RecvData.m_HoldPosX &&
        RecvData.m_PosY == RecvData.m_HoldPosY && RecvData.m_Channel == RecvData.m_HoldChannel )	// 채널까지 확인해야, 다른 채널인데 같은 좌표에 아이템을 놓지 못하는 문제 수정
        return FALSE;

    DWORD ChaDbNum = pChar->ChaDbNum();

    std::tr1::shared_ptr< GLClubAgent > pClub = GetClub( pChar->GetClubDbNum() );
    if ( !pClub )
        return FALSE;

    if ( !pClub->HaveStorageAuthority( pChar->ChaDbNum() ) )
        return FALSE;

	// 클럽창고 사용 요청, 사용자 설정한다. 아래 autoreset 객체가 사라질 때 ResetClubStorageUser 함수 호출된다.
	if ( false == pClub->ReqClubStorageUse( ChaDbNum ) )
	{
		std::string ClubStorageUser = pClub->GetClubStorageUserName();
		if ( false == ClubStorageUser.empty() )
		{
			GLMSG::SNET_CLUB_STORAGE_USER_FB SendData( ClubStorageUser );
			msgpack::sbuffer SendBufferAC;
			msgpack::pack( SendBufferAC, SendData );
			SENDTOCLIENT( pChar->ClientSlot(), NET_MSG_GCTRL_CLUB_STORAGE_USER_FB, SendBufferAC );
		}
		return FALSE;
	}

	// MsgClubStorageToInvenCA 함수에서는 중간에 리턴하면 일일이 ResetClubStorageUser 을 호출해야 했는데, 
	// 그 이유는 클럽창고->인벤토리 로 아이템 이동 메시지의 끝이 필드서버까지 갔다와야 되서이다. 
	// MsgClubStorageToClubStorageCA 함수의 경우 에이전트에서 끝이 나기 때문에 일일이 ResetClubStorageUser 를 호출하지않고 AutoResetClubStorageUser 사용한다.
	// MsgClubStorageToClubStorageCA 함수 리턴하면 클럽창고 사용자 자동 리셋된다. 소멸자에서 호출됨
	AutoResetClubStorageUser autoreset( pClub );

    const SINVENITEM* pClubHoldItem = pClub->GetItem( RecvData.m_HoldChannel, RecvData.m_HoldPosX, RecvData.m_HoldPosY );
    if ( !pClubHoldItem )
        return FALSE;

	// 이미 클럽락커에 들어가 있는 아이템인데 무의해 보인다.
	if ( !( pClubHoldItem->sItemCustom.IsExchange() != NULL ) )
        return FALSE;

    SINVENITEM sHoldBackUp = *pClubHoldItem;
    WORD wHoldPosX = sHoldBackUp.wPosX;		//	손에 든 아이템 위치.
    WORD wHoldPosY = sHoldBackUp.wPosY;

    if ( !pClub->DeleteItem( RecvData.m_HoldChannel, RecvData.m_HoldPosX, RecvData.m_HoldPosY ) )
        return E_FAIL;

    // 창고 아이템 삭제 됨
    GLMSG::SNET_CLUB_STORAGE_DELETE MsgAC;
    MsgAC.dwChannel = RecvData.m_HoldChannel;
    MsgAC.wPosX = RecvData.m_HoldPosX;
    MsgAC.wPosY = RecvData.m_HoldPosY;
    pClub->SendClubClient( &MsgAC );

    if ( !pClub->InsertItem( sHoldBackUp.CustomItem(), RecvData.m_Channel, RecvData.m_PosX, RecvData.m_PosY ) )
    {
        // 실패 시 삭제된 아이템 다시 넣어준다.
		if ( !pClub->InsertItem( sHoldBackUp.CustomItem(), RecvData.m_HoldChannel, RecvData.m_HoldPosX, RecvData.m_HoldPosY ) )
		{
			// 강제로 넣었으나 실패, 로그 기록하고 실패 아이템 처리한다.
			std::string strGuid = sc::string::uuidToString( sHoldBackUp.GetGuid() );
			sc::writeLogError( sc::string::format( "error MsgClubStorageToClubStorageCA, InsertItem fail, clubnum: %1% guid: %2%", pChar->GetClubDbNum(), strGuid.c_str() ) );

			// cause 는 enum 으로 만들어야 한다. 함수 이름 그대로 사용하도록
			InsertErrorItem( sHoldBackUp.GetGuid(), 1 );
		}

        return E_FAIL;
    }

	// 디비 업데이트
	pClub->ClubStorageUpdateItem( RecvData.m_Channel, RecvData.m_PosX, RecvData.m_PosY, true );

    return TRUE;
}

BOOL GLAgentServer::MsgClubStorageExHoldFA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{/*
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) nmg;
    GLMSG::NET_CLUB_STORAGE_EX_INVEN_FA Data;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), Data))
        return FALSE;

    // crc check
    if (pPacket->Crc() != sc::string::getCrc32(pPacket->DataBuffer(), pPacket->DataSize()))
    {
        sc::writeLogError(
            sc::string::format(
                "MsgClubStorageExHoldFA CRC ERROR. ClubDbNum %1% ChaDbNum %2% Item %3%/%4% TurnNum %5%",
                Data.m_ClubDbNum,
                Data.m_ChaDbNum,
                Data.m_Item.Mid(),
                Data.m_Item.Sid(),
                Data.m_Item.TurnNum()));
        return FALSE;
    }

    // magic num check
    if (Data.m_MagicNum != GLMSG::NET_CLUB_STORAGE_EX_INVEN_FA::MAGIC_NUM)
    {
        sc::writeLogError(
            sc::string::format(
                "MsgClubStorageExHoldFA MAGIC NUM ERROR. ClubDbNum %1% ChaDbNum %2% Item %3%/%4% TurnNum %5%",
                Data.m_ClubDbNum,
                Data.m_ChaDbNum,
                Data.m_Item.Mid(),
                Data.m_Item.Sid(),
                Data.m_Item.TurnNum()));
        return FALSE;
    }    
    
    GLCharAG* pChar = GetCharByDbNum(Data.m_ChaDbNum);
    if (!pChar)
    {
        sc::writeLogError(
            sc::string::format(
                "MsgClubStorageExHoldFA Character not exist. ClubDbNum %1% ChaDbNum %2% Item %3%/%4% TurnNum %5%",
                Data.m_ClubDbNum,
                Data.m_ChaDbNum,
                Data.m_Item.Mid(),
                Data.m_Item.Sid(),
                Data.m_Item.TurnNum()));        
        return FALSE;
    }

    std::tr1::shared_ptr<GLClubAgent> spClub = GetClub(Data.m_ClubDbNum);
    if (!spClub)
    {
        sc::writeLogError(
            sc::string::format(
                "MsgClubStorageExHoldFA Club not exist. ClubDbNum %1% ChaDbNum %2% Item %3%/%4% TurnNum %5%",
                Data.m_ClubDbNum,
                Data.m_ChaDbNum,
                Data.m_Item.Mid(),
                Data.m_Item.Sid(),
                Data.m_Item.TurnNum()));
        return FALSE;
    }

    spClub->InsertItem(Data.m_Item, Data.m_Channel, Data.m_ClubStoragePosX, Data.m_ClubStoragePosY);*/
    return TRUE;
}

BOOL GLAgentServer::MsgClubInvenToStorageFA( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
    NET_MSG_PACK* pPacket = ( NET_MSG_PACK* ) nmg;
    GLMSG::NET_CLUB_INVEN_TO_STORAGE_FA Data;
    if ( !msgpack::BufferToObject( pPacket->DataBuffer(), pPacket->DataSize(), Data ) )
        return FALSE;

    // Crc check
    if ( pPacket->Crc() != sc::string::getCrc32( pPacket->DataBuffer(), pPacket->DataSize() ) )
    {
        sc::writeLogError( 
            sc::string::format( 
                "MsgClubHoldToStorageFA CRC. ClubDbNum %1% ChaDbNum %2% Item %3%/%4% TurnNum %5%", 
                Data.m_ClubDbNum, Data.m_ChaDbNum, Data.m_Item.GetNativeID().Mid(), Data.m_Item.GetNativeID().Sid(), Data.m_Item.TurnNum() ) );
        return FALSE;
    }

    // Magic num check
    if ( Data.m_MagicNum != GLMSG::NET_CLUB_INVEN_TO_STORAGE_FA::MAGIC_NUM )
    {
        sc::writeLogError( 
            sc::string::format( 
                "MsgClubHoldToStorageFA MAGIC NUM. ClubDbNum %1% ChaDbNum %2% Item %3%/%4% TurnNum %5%", 
                Data.m_ClubDbNum, Data.m_ChaDbNum, Data.m_Item.GetNativeID().Mid(), Data.m_Item.GetNativeID().Sid(), Data.m_Item.TurnNum() ) );
        return FALSE;
    }

    DWORD ChaDbNum = Data.m_ChaDbNum;
    const SITEMCUSTOM& InsertedItem = Data.m_Item;
    GLCharAG* pChar = GetCharByDbNum( ChaDbNum );
    if ( !pChar )
    {
        sc::writeLogError( 
            sc::string::format( 
                "MsgClubHoldToStorageFA can't find character. ClubDbNum %1% ChaDbNum %2% Item %3%/%4% TurnNum %5%", 
				Data.m_ClubDbNum, ChaDbNum, InsertedItem.Mid(), InsertedItem.Sid(), InsertedItem.TurnNum() ) );
        
        // 아이템 복구/캐릭터없음
        // 캐릭터가 존재하지 않아 정확히 어디로 보내야 할지 알수 없다.
        // 일단 복구 시도를 위해서 전체 서버에 메시지를 전송한다.
		// 그 캐릭터 아이템을 삭제했지만, 디비에서는 삭제하지 않았다. 필드에 캐릭터가 없어도 문제될 건 없다.
        GLMSG::NET_HOLD_ITEM_RESTORE_AF MsgAF( ChaDbNum, InsertedItem );
        msgpack::sbuffer SendBuffer;
        msgpack::pack( SendBuffer, MsgAF );
        SENDTOALLCHANNEL( NET_MSG_CLUB_HOLD_TO_STORAGE_AF, SendBuffer, true );
        return FALSE;
    }

    const SITEM* pItemData = GLogicData::GetInstance().GetItem( InsertedItem.GetBoxWrappedItemID() );
    if ( !pItemData )
    {        
        // 아이템 복구/존재하지 않는 아이템
        GLMSG::NET_HOLD_ITEM_RESTORE_AF MsgAF( ChaDbNum, InsertedItem );
        msgpack::sbuffer SendBuffer;
        msgpack::pack( SendBuffer, MsgAF );
        SENDTOFIELDSVR( pChar->GetChannel(), pChar->GetCurrentFieldServer(), NET_MSG_CLUB_HOLD_TO_STORAGE_AF, SendBuffer, true );
        return FALSE;
    }

    std::tr1::shared_ptr< GLClubAgent > pClub = GetClub( pChar->GetClubDbNum() );
    if ( !pClub )
    {
        // 아이템 복구/클럽없음
		GLMSG::NET_HOLD_ITEM_RESTORE_AF MsgAF( ChaDbNum, InsertedItem );
		msgpack::sbuffer SendBuffer;
		msgpack::pack( SendBuffer, MsgAF );
		SENDTOFIELDSVR( pChar->GetChannel(), pChar->GetCurrentFieldServer(), NET_MSG_CLUB_HOLD_TO_STORAGE_AF, SendBuffer, true );
        return FALSE;
    }

    if ( !pClub->HaveStorageAuthority( ChaDbNum ) )
    {        
        // 아이템 복구/권한없음
		GLMSG::NET_HOLD_ITEM_RESTORE_AF MsgAF( ChaDbNum, InsertedItem );
		msgpack::sbuffer SendBuffer;
		msgpack::pack( SendBuffer, MsgAF );
		SENDTOFIELDSVR( pChar->GetChannel(), pChar->GetCurrentFieldServer(), NET_MSG_CLUB_HOLD_TO_STORAGE_AF, SendBuffer, true );
        return FALSE;
    }

	// 클럽창고 사용 요청, 사용자 설정한다. 아래 autoreset 객체가 사라질 때 ResetClubStorageUser 함수 호출된다.
	if ( false == pClub->ReqClubStorageUse( ChaDbNum ) )
	{
		std::string ClubStorageUser = pClub->GetClubStorageUserName();
		if ( false == ClubStorageUser.empty() )
		{
			GLMSG::SNET_CLUB_STORAGE_USER_FB SendData( ClubStorageUser );
			msgpack::sbuffer SendBufferAC;
			msgpack::pack( SendBufferAC, SendData );
			SENDTOCLIENT( pChar->ClientSlot(), NET_MSG_GCTRL_CLUB_STORAGE_USER_FB, SendBufferAC );
		}

		// 아이템 복구/누군가 클럽창고 사용중
		GLMSG::NET_HOLD_ITEM_RESTORE_AF MsgAF( ChaDbNum, InsertedItem );
		msgpack::sbuffer SendBuffer;
		msgpack::pack( SendBuffer, MsgAF );
		SENDTOFIELDSVR( pChar->GetChannel(), pChar->GetCurrentFieldServer(), NET_MSG_CLUB_HOLD_TO_STORAGE_AF, SendBuffer, true );
		return FALSE;
	}

	AutoResetClubStorageUser autoreset( pClub );

    // 아이템 삽입
    if ( !pClub->InsertItem( InsertedItem, Data.m_Channel, Data.m_PosX, Data.m_PosY ) )
    {
		// 아이템 복구/아이템을 넣을 곳이 없다
		GLMSG::NET_HOLD_ITEM_RESTORE_AF MsgAF( ChaDbNum, InsertedItem );
		msgpack::sbuffer SendBuffer;
		msgpack::pack( SendBuffer, MsgAF );
		SENDTOFIELDSVR( pChar->GetChannel(), pChar->GetCurrentFieldServer(), NET_MSG_CLUB_HOLD_TO_STORAGE_AF, SendBuffer, true );
		return FALSE;

//         // 실패하면 원하는 위치가 아닌 다른 곳에 한번 더 넣기를 시도한다.
//         if ( !pClub->InsertItem( Data.m_Item ) )
//         {
//             // 아이템 복구/아이템을 넣을 곳이 없다
// 			GLMSG::NET_HOLD_ITEM_RESTORE_AF MsgAF( ChaDbNum, InsertedItem );
// 			msgpack::sbuffer SendBuffer;
// 			msgpack::pack( SendBuffer, MsgAF );
// 			SENDTOFIELDSVR( pChar->GetChannel(), pChar->GetCurrentFieldServer(), NET_MSG_CLUB_HOLD_TO_STORAGE_AF, SendBuffer, true );
//             return FALSE;
//         }
    }

	pClub->ClubStorageUpdateItem( Data.m_Channel, Data.m_PosX, Data.m_PosY, true );

    // ClubLog
    // [%1%]님이 클럽 창고에 [%2%]X%3% (을)를 보관하셨습니다.  
    std::string ClubLog( 
        sc::string::format( 
            ID2GAMEINTEXT( "CLUB_LOG_6" ), 
            pChar->GetChaName(), 
            InsertedItem.GETNAME(), 
            InsertedItem.TurnNum() ) );
    SENDCLUBLOG( pChar->GetClubDbNum(), club::LOG_CLUB_STORAGE, ClubLog );
    return TRUE;
}
/*
BOOL GLAgentServer::MsgClubStorageSplit(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
    GLMSG::SNET_CLUB_STORAGE_SPLIT* pPacket = (GLMSG::SNET_CLUB_STORAGE_SPLIT*) nmg;
    GLCharAG* pChar = GetChar(dwGaeaID);
    if (!pChar)
        return FALSE;    

    std::tr1::shared_ptr<GLClubAgent> pClub = GetClub(pChar->GetClubDbNum());
    if (!pClub)
        return FALSE;

    if (pClub->HaveStorageAuthority(pChar->ChaDbNum()))
        return FALSE;

    //if (!IsKEEP_CLUB_STORAGE(pPacket->dwChannel))
    //    return FALSE;

    return pClub->SplitItem(pPacket->dwChannel, pPacket->wPosX, pPacket->wPosY, pPacket->wSplit);
}
*/
BOOL GLAgentServer::MsgClubStorageDrawMoneyCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
    NET_MSG_PACK* pPacket = ( NET_MSG_PACK* ) nmg;
    GLMSG::NET_CLUB_STORAGE_DRAW_MONEY_CA RecvData;
    if ( !msgpack::BufferToObject( pPacket->DataBuffer(), pPacket->DataSize(), RecvData ) )
        return FALSE;
    
    GLCharAG* pChar = GetChar( dwGaeaID );
    if ( !pChar )
	{
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% <-dwGaeaID  on MsgClubStorageDrawMoneyCA ", dwGaeaID ) );
        return FALSE;
	}

    DWORD ChaDbNum = pChar->ChaDbNum();
    DWORD ClubDbNum = pChar->GetClubDbNum();
    std::tr1::shared_ptr< GLClubAgent > spClub = GetClub( ClubDbNum );
    if ( !spClub )
        return FALSE;

    if ( !spClub->HaveMoneyAuthority( ChaDbNum ) )
        return FALSE;
    
    if ( !spClub->IsValidStorage() )
        return FALSE;

	if ( false == spClub->ReqClubStorageMoneyUse( ChaDbNum ) )
	{
		std::string ClubStorageUser = spClub->GetClubStorageMoneyUserName();
		if ( false == ClubStorageUser.empty() )
		{
			GLMSG::SNET_CLUB_STORAGE_USER_FB SendData( ClubStorageUser );
			msgpack::sbuffer SendBufferAC;
			msgpack::pack( SendBufferAC, SendData );
			SENDTOCLIENT( pChar->ClientSlot(), NET_MSG_GCTRL_CLUB_STORAGE_MONEY_USER_FB, SendBufferAC );
		}
		return FALSE;
	}

    LONGLONG lnMoney = RecvData.m_WithdrawMoney;
    if ( lnMoney <= 0 || spClub->GetStorageMoney() < lnMoney )
	{
		spClub->ResetClubStorageMoneyUser();
        return FALSE;
	}

    spClub->SubtractStorageMoney( lnMoney );

    // 금액 로그
	// 통계 및 로그 수정
    LogMoneyExchange( gamelog::ID_CHAR, ChaDbNum, gamelog::ID_CLUB, ClubDbNum,              -lnMoney, ITEM_ROUTE_CLUB, ( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap(), LOG_MONEY_CLUB_STORAGE_DRAW );    
	// 통계 및 로그 수정
    LogMoneyExchange( gamelog::ID_CHAR,        0, gamelog::ID_CLUB, ClubDbNum, spClub->GetStorageMoney(), MONEY_ROUTE_CLUB_TOTAL_MONEY, ( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap(), LOG_MONEY_CLUB_STORAGE_CURRENT );
    
    // 클라이언트에 길드 창고에 있는 돈 액수 변화를 알려줌.
    GLMSG::SNET_CLUB_STORAGE_UPDATE_MONEY NetMsgStorage( spClub->GetStorageMoney(), spClub->GetIncomeMoney() );
    spClub->SendClubClient( &NetMsgStorage );

    // 캐릭터에게 금액전송
    GLMSG::NET_CLUB_STORAGE_DRAW_MONEY_AF DataAF( ChaDbNum, ClubDbNum, lnMoney );
    msgpack::sbuffer SendBuffer;
    msgpack::pack( SendBuffer, DataAF );
    SENDTOFIELDSVR( 
        pChar->GetChannel(), 
        pChar->GetCurrentFieldServer(), 
        NET_MSG_CLUB_STORAGE_DRAW_MONEY_AF, 
        SendBuffer );

    // Club log/돈 인출
    std::string ClubLog( 
        sc::string::format( 
            ID2GAMEINTEXT( "CLUB_LOG_16" ), 
            pChar->GetChaName(), 
            lnMoney, 
            spClub->GetStorageMoney() ) );
    SENDCLUBLOG( ClubDbNum, club::LOG_CLUB_STORAGE, ClubLog );
    return TRUE;
}

BOOL GLAgentServer::MsgClubStorageDrawMoneyFA( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
    if ( dwGaeaID != GAEAID_NULL )
    {
        sc::writeLogError( "MsgClubStorageDrawMoneyFA GaeaID not null." );
        return FALSE;
    }

    NET_MSG_PACK* pPacket = ( NET_MSG_PACK* ) nmg;
    GLMSG::NET_CLUB_STORAGE_DRAW_MONEY_FA RecvData;
    if ( !msgpack::BufferToObject( pPacket->DataBuffer(), pPacket->DataSize(), RecvData ) )
        return FALSE;

    if ( RecvData.m_MagicNum != GLMSG::NET_CLUB_STORAGE_DRAW_MONEY_FA::MAGIC_NUM )
        return FALSE;

    if ( RecvData.m_Money <= 0 )
        return FALSE;

    DWORD ClubDbNum = RecvData.m_ClubDbNum;
    std::tr1::shared_ptr< GLClubAgent > spClub = GetClub( ClubDbNum );
    if (!spClub)
        return FALSE;

	DWORD CurrentClubStorageMoneyUser = spClub->GetClubStorageMoneyUser();
	if ( GAEAID_NULL != CurrentClubStorageMoneyUser && RecvData.m_ChaDbNum != CurrentClubStorageMoneyUser )
	{
		sc::writeLogError( 
			sc::string::format( 
			"MsgClubStorageDrawMoneyFA GAEAID_NULL != CurrentClubStorageMoneyUser && RecvData.m_ChaDbNum != CurrentClubStorageMoneyUser ClubDbNum %1% ChaDbNum %2% Money %3%", 
			RecvData.m_ClubDbNum, RecvData.m_ChaDbNum, RecvData.m_Money ) );
		return FALSE;
	}

    spClub->AddStorageMoney( RecvData.m_Money );
    
    // 클라이언트에 길드 창고에 있는 돈 액수 변화를 알려줌.
    GLMSG::SNET_CLUB_STORAGE_UPDATE_MONEY NetMsgStorage( spClub->GetStorageMoney(), spClub->GetIncomeMoney() );
    spClub->SendClubClient( &NetMsgStorage );

    // [%1%]금액이 복구되었습니다. 보관금액은 [%2%]입니다.
    std::string ClubLog( 
        sc::string::format( 
            ID2GAMEINTEXT( "CLUB_LOG_18" ), 
            RecvData.m_Money, 
            spClub->GetStorageMoney() ) );
    SENDCLUBLOG( ClubDbNum, club::LOG_CLUB_STORAGE, ClubLog );

	// 통계 및 로그 수정
    LogMoneyExchange( gamelog::ID_CHAR, 0, gamelog::ID_CLUB, ClubDbNum, spClub->GetStorageMoney(), MONEY_ROUTE_CLUB_TOTAL_MONEY, MapID( SNATIVEID( false ), SNATIVEID( false ) ), LOG_MONEY_CLUB_STORAGE_CURRENT );

    return TRUE;
}

BOOL GLAgentServer::MsgClubStorageSaveMoneyFA( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
    if ( dwGaeaID != GAEAID_NULL )
        return FALSE;

    NET_MSG_PACK* pPacket = ( NET_MSG_PACK* ) nmg;
    GLMSG::NET_CLUB_STORAGE_SAVE_MONEY_FA RecvData;
    if ( !msgpack::BufferToObject( pPacket->DataBuffer(), pPacket->DataSize(), RecvData ) )
        return FALSE;

    DWORD ChaDbNum    = RecvData.m_ChaDbNum;
    LONGLONG AddMoney = RecvData.m_Money;

    if ( AddMoney <= 0 )
        return FALSE;

    if ( RecvData.m_MagicNum != GLMSG::NET_CLUB_STORAGE_SAVE_MONEY_FA::MAGIC_NUM )
    {
        sc::writeLogError( 
            sc::string::format( 
                "MsgClubStorageSaveMoneyFA Magic num mismatch. ChaDbNum %1% Money %2% MagicNum %3%", 
                ChaDbNum, AddMoney, RecvData.m_MagicNum ) );
        return FALSE;
    }

    GLCharAG* pChar = GetCharByDbNum( ChaDbNum );
    if ( !pChar )
    {
		// 캐릭터 게임머니는 이미 빼진 상태이나, 캐릭터가 없다. 나중에 그 캐릭터 복구가 필요하다.
        sc::writeLogError( 
            sc::string::format( 
                "MsgClubStorageSaveMoneyFA Character not exist. ChaDbNum %1% Money %2%", 
				ChaDbNum, AddMoney ) );
        return FALSE;
    }

    DWORD ClubDbNum = RecvData.m_ClubDbNum;

    if ( ClubDbNum != pChar->GetClubDbNum() )
    {
        sc::writeLogError( 
            sc::string::format( 
                "MsgClubStorageSaveMoneyFA Club num mismatch. ChaDbNum %1% Money %2% ClubDbNum %3% != %4%", 
                ChaDbNum, AddMoney, ClubDbNum, pChar->GetClubDbNum() ) );
        
        // 게임머니 복구
        GLMSG::NET_CLUB_STORAGE_SAVE_MONEY_AF SendData( ChaDbNum, AddMoney );
        msgpack::sbuffer SendBuffer;
        msgpack::pack( SendBuffer, SendData );
        SENDTOFIELDSVR( pChar->GetChannel(), pChar->GetCurrentFieldServer(), NET_MSG_CLUB_STORAGE_SAVE_MONEY_AF, SendBuffer );
        return FALSE;
    }
    
    std::tr1::shared_ptr< GLClubAgent > spClub = GetClub( ClubDbNum );
    if ( !spClub )
    {
        sc::writeLogError( 
            sc::string::format( 
                "MsgClubStorageSaveMoneyFA Club not exist. ChaDbNum %1% Money %2% ClubDbNum %3%", 
                ChaDbNum, AddMoney, ClubDbNum ) );

        // 게임머니 복구
		GLMSG::NET_CLUB_STORAGE_SAVE_MONEY_AF SendData( ChaDbNum, AddMoney );
		msgpack::sbuffer SendBuffer;
		msgpack::pack( SendBuffer, SendData );
		SENDTOFIELDSVR( pChar->GetChannel(), pChar->GetCurrentFieldServer(), NET_MSG_CLUB_STORAGE_SAVE_MONEY_AF, SendBuffer );
		return FALSE;
    }
    
    if ( !spClub->HaveMoneyAuthority( ChaDbNum ) )
    {
        sc::writeLogError( 
            sc::string::format( 
            "MsgClubStorageSaveMoneyFA HaveMoneyAuthority. ChaDbNum %1% Money %2% ClubDbNum %3%", 
            ChaDbNum, AddMoney, ClubDbNum ) );
        
        // 게임머니 복구
		GLMSG::NET_CLUB_STORAGE_SAVE_MONEY_AF SendData( ChaDbNum, AddMoney );
		msgpack::sbuffer SendBuffer;
		msgpack::pack( SendBuffer, SendData );
		SENDTOFIELDSVR( pChar->GetChannel(), pChar->GetCurrentFieldServer(), NET_MSG_CLUB_STORAGE_SAVE_MONEY_AF, SendBuffer );
		return FALSE;
    }

	if ( false == spClub->ReqClubStorageMoneyUse( ChaDbNum ) )
	{
		std::string ClubStorageUser = spClub->GetClubStorageMoneyUserName();
		if ( false == ClubStorageUser.empty() )
		{
			GLMSG::SNET_CLUB_STORAGE_USER_FB SendData( ClubStorageUser );
			msgpack::sbuffer SendBufferAC;
			msgpack::pack( SendBufferAC, SendData );
			SENDTOCLIENT( pChar->ClientSlot(), NET_MSG_GCTRL_CLUB_STORAGE_MONEY_USER_FB, SendBufferAC );
		}

		// 게임머니 복구
		GLMSG::NET_CLUB_STORAGE_SAVE_MONEY_AF SendData( ChaDbNum, AddMoney );
		msgpack::sbuffer SendBuffer;
		msgpack::pack( SendBuffer, SendData );
		SENDTOFIELDSVR( pChar->GetChannel(), pChar->GetCurrentFieldServer(), NET_MSG_CLUB_STORAGE_SAVE_MONEY_AF, SendBuffer );
		return FALSE;
	}

	AutoResetClubStorageMoneyUser autoreset( spClub );

    spClub->AddStorageMoney( AddMoney );

    // 금액 로그
	// 통계 및 로그 수정
    LogMoneyExchange( gamelog::ID_CHAR, 0, gamelog::ID_CLUB, ClubDbNum, spClub->GetStorageMoney(), MONEY_ROUTE_CLUB_TOTAL_MONEY, ( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap(), LOG_MONEY_CLUB_STORAGE_CURRENT );
    
    // [%1%]님이 [%2%]만큼 창고에 입금하셨습니다. 총금액은 [%3%]입니다.
    std::string ClubLog( 
        sc::string::format( 
            ID2GAMEINTEXT( "CLUB_LOG_15" ), 
            pChar->GetChaName(), 
            AddMoney, 
            spClub->GetStorageMoney() ) );
    SENDCLUBLOG( ClubDbNum, club::LOG_CLUB_STORAGE, ClubLog );
    
    // 클라이언트에 돈 액수 변화를 알려줌.
    GLMSG::SNET_CLUB_STORAGE_UPDATE_MONEY NetMsgStorage( spClub->GetStorageMoney(), spClub->GetIncomeMoney() );
    spClub->SendClubClient( &NetMsgStorage );

    return TRUE;
}

//! 게임머니 거래로그
// 통계 및 로그 수정
void GLAgentServer::LogMoneyExchange(
    gamelog::EMIDTYPE FromType, DWORD FromDbNum, // 출발지
    gamelog::EMIDTYPE ToType, DWORD ToDbNum, // 목적지
    LONGLONG lnPrice, EMITEM_ROUTE Flag, const MapID &sMapID, EMITEM_ROUTE_DETAIL emFlagDetail)
{    
    m_pItemLimit->LogMoneyExchange(
        FromType, FromDbNum,
        ToType, ToDbNum,
        lnPrice, Flag, sMapID, emFlagDetail);
}

// 통계 및 로그 수정
void GLAgentServer::LogCharAction(
    DWORD dwCI, gamelog::EMACTION emACT, 
    gamelog::EMIDTYPE emTAR, DWORD dwTAR, 
    __int64 nEXP, int nBRIGHT, int nLIFE, int nMONEY, const MapID &sMapID)
{
    m_pItemLimit->LogCharAction(dwCI, emACT, emTAR, dwTAR, nEXP, nBRIGHT, nLIFE, nMONEY, sMapID);
}

void GLAgentServer::SaveClubStorage2DbAll()
{
    if (m_pClubMan)
        m_pClubMan->SaveStorage2DbAll();
}

BOOL GLAgentServer::MsgReqDropCharClubNameCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*)nmg;
    wb::NET_REQ_DROP_CHAR_CLUB_NAME_CA RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return FALSE;

    // 사이즈는 다를 수 없다.
    size_t size = RecvData.m_vecGaeaId.size();
    if( RecvData.m_vecClubDbNum.size() != RecvData.m_vecGaeaId.size() )
        return FALSE;

    for( unsigned int i=0 ; i<RecvData.m_vecGaeaId.size() ; i++ )
    {
        std::tr1::shared_ptr<GLClubAgent> pClub = GetClub(RecvData.m_vecClubDbNum[i]);
        if (!pClub)
            return FALSE;

        // 클럽정보 전송
        m_pClubMan->SendCountryClubInfo(pClub, dwClientID, jm::MSG_WORLD_BATTLE_CLUB_INFO);    

        wb::NET_UPDATE_DROP_CHAR_CLUB_NAME_AC MsgAC(RecvData.m_vecGaeaId[i], RecvData.m_vecClubDbNum[i]);
        SENDTOCLIENT(dwClientID, &MsgAC);
    }

    return TRUE;
}

void GLAgentServer::ClubMemberGradeChange(GLCharAG* pChar, DWORD MemberDbNum, int GradeIndex)
{
    if (!pChar)
        return;

    DWORD ClubDbNum = pChar->GetClubDbNum();
    std::tr1::shared_ptr<GLClubAgent> spClub = m_pClubMan->GetClub(ClubDbNum);
    if (!spClub)
        return; // 클럽없음

    DWORD ChaDbNum = pChar->ChaDbNum();
    if (!spClub->HaveAuthChangeAuthority(ChaDbNum))
        return; // 권한 없음

    if (!spClub->IsMember(MemberDbNum))
        return; // 권한을 바꾸려는 캐릭터가 club member 가 아님

    if (spClub->IsMaster(MemberDbNum))
        return; // 클럽 마스터의 권한은 바꾸지 못함

    spClub->ChangeMemberGrade(MemberDbNum, GradeIndex);

    DWORD ClientSlot = pChar->ClientSlot();
    
    // 전체 field server 에 알림
    NET_CLUB_MEMBER_GRADE_CHANGE_AF SendAF;
    SendAF.m_ClubDbNum   = ClubDbNum;
    SendAF.m_MemberDbNum = MemberDbNum;
    SendAF.m_Index       = GradeIndex;
    
    msgpack::sbuffer SendBufferAF;
    msgpack::pack(SendBufferAF, SendAF);
    
    SENDTOALLCHANNEL(NET_MSG_CLUB_MEMBER_GRADE_CHANGE_AF, SendBufferAF);
    
    // 전체 club member 에게 알림
    NET_CLUB_MEMBER_GRADE_CHANGE_AC2 SendAC;
    SendAC.m_MemberDbNum = MemberDbNum;
    SendAC.m_GradeIndex  = GradeIndex;
    
    msgpack::sbuffer SendBufferAC;
    msgpack::pack(SendBufferAC, SendAC);
    
    SENDTOCLUBCLIENT(ClubDbNum, NET_MSG_CLUB_MEMBER_GRADE_CHANGE_AC2, SendBufferAC);
}

void GLAgentServer::ClubCdCertifyChange(GLCharAG* pChar, DWORD MemberDbNum, bool bCd)
{
    if (!pChar)	
	{
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError(std::string("Can not find GetCharByDbNum on ClubCdCertifyChange "));
        return;
	}

    DWORD ClientSlot = pChar->ClientSlot();
	GLMSG::SNET_CLUB_SUBMASTER_FB NetMsgFB;

    DWORD ClubDbNum = pChar->GetClubDbNum();
    DWORD MasterDbNum = pChar->ChaDbNum();
    std::tr1::shared_ptr<GLClubAgent> spClub = m_pClubMan->GetClub(ClubDbNum);
    if (!spClub)
    {
        NetMsgFB.emFB = EMCLUBSUBMASTER_FB_FAIL;
        SENDTOCLIENT(ClientSlot, &NetMsgFB);
        return;
    }

	if (GuidanceIsBattle())
	{
		NetMsgFB.emFB = EMCLUBSUBMASTER_FB_BATTLE;
		SENDTOCLIENT(ClientSlot, &NetMsgFB);
		return;
	}

	if (ClubDeathMatchIsBattle())
	{
		NetMsgFB.emFB = EMCLUBSUBMASTER_FB_CDM;
		SENDTOCLIENT(ClientSlot, &NetMsgFB);
		return;
	}
	
	if (!spClub->IsMaster(MasterDbNum))
	{
		NetMsgFB.emFB = EMCLUBSUBMASTER_FB_NOTMASTER;
		SENDTOCLIENT(ClientSlot, &NetMsgFB);
		return;
	}
	
    if (!spClub->IsMember(MemberDbNum))
    {
        NetMsgFB.emFB = EMCLUBSUBMASTER_FB_FAIL;
        SENDTOCLIENT(ClientSlot, &NetMsgFB);
        return;
    }

    DWORD OldCdCertifier = spClub->GetCdCertifier();
    if (OldCdCertifier == 0)
        OldCdCertifier = spClub->MasterCharDbNum();

	// CD 인증권한 설정
    DWORD NewCdCertifier = 0;
    if (bCd)
    {
        spClub->SetCdCertifier(MemberDbNum);
        NewCdCertifier = MemberDbNum;
    }
    else
    {
        spClub->SetCdCertifier(MasterDbNum);
        NewCdCertifier = MasterDbNum;
    }
	
	// DB에 저장
	AddGameDbJob(
        db::DbActionPtr(
            new db::ClubDeputySet(ClubDbNum, NewCdCertifier)));

	// 클럽 인증 CD 권한 설정에 대한 로그        
	// 통계 및 로그 수정
	LogCharAction(
		MasterDbNum,		//	당사자.
		gamelog::ACT_CLUB_DEPUTY_MEMBER,	//	행위.
		gamelog::ID_CHAR, NewCdCertifier,	//	상대방.
		0,
		0,
		0,
		0, 
		( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap());

    // Club member 에 변경 성공 알림
    GLMSG::NET_CLUB_CD_CERTIFIER_CHANGE_AC MsgAC(NewCdCertifier);
    SENDTOCLUBCLIENT(ClubDbNum, &MsgAC);    

    // Field 에 알림
    GLMSG::NET_CLUB_CD_CERTIFIER_CHANGE_AF MsgAF(ClubDbNum, NewCdCertifier, OldCdCertifier);
    SENDTOALLCHANNEL(&MsgAF);
}

bool GLAgentServer::MsgClubMemberAddLobby(
    DWORD ClubDbNum,
    DWORD ClientSlot,
    DWORD ChaDbNum,
    const std::string& ChaName,
    EMCHARCLASS ChaClass)
{
    std::tr1::shared_ptr<GLClubAgent> spClub = GetClub(ClubDbNum);
    return ClubMemberAdd(
        std::string(""),
        spClub,
        ClientSlot,
        ChaName,
        ChaDbNum,
        1,
        true,
        sc::time::GetCurrentTime(),
        ChaClass);
}

bool GLAgentServer::ClubMemberAdd(
    const std::string& FromChaName,
    std::tr1::shared_ptr<GLClubAgent> spClub,
    DWORD ClientSlot,
    const std::string& TargetChaName,
    DWORD TarChaDbNum,
    WORD TarChaLevel,
    bool bLobby,
    const __time64_t& LastLoginTime,
    EMCHARCLASS ChaClass)
{
    if (!spClub)
        return false;

	GLCharAG* pChar = GetCharByDbNum(TarChaDbNum);
    if (!pChar)
	{
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on ClubMemberAdd ", TarChaDbNum));
	}

    DWORD ClubDbNum = spClub->DbNum();
    if (spClub->GetAllBattleNum() > 0)
    {
        GLMSG::NET_CLUB_MEMBER_REQ_FB_AC NetMsgFB(bLobby);
        NetMsgFB.SetTarChaName(TargetChaName.c_str());
        NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_CLUBBATTLE;
        SENDTOCLIENT(ClientSlot, &NetMsgFB);
        return false;
    }
    
    bool bFOUND = spClub->IsMember(TarChaDbNum);
    if ( bFOUND )
    {
        GLMSG::NET_CLUB_MEMBER_REQ_FB_AC NetMsgFB(bLobby);
        NetMsgFB.SetTarChaName(TargetChaName.c_str());
        NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_ALREADY;
        SENDTOCLIENT(ClientSlot, &NetMsgFB);
        return false;
    }

	// http://172.16.2.87/redmine/issues/4778
	if ( false == IsAutoJoinClub( ClubDbNum ) )
	{
		const GLCLUBRANK& sRANK_DATA = GLogicData::GetInstance().GetClubRankData(spClub->m_dwRank);
		if (spClub->GetMemberNum() >= sRANK_DATA.m_dwMember)
		{
			GLMSG::NET_CLUB_MEMBER_REQ_FB_AC NetMsgFB(bLobby);
			NetMsgFB.SetTarChaName(TargetChaName.c_str());
			NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_MAXNUM;
			SENDTOCLIENT(ClientSlot, &NetMsgFB);
			return false;
		}
	}

    // 클럽 가입 로그 기록.    
	// 통계 및 로그 수정
    LogCharAction(
        spClub->MasterCharDbNum(),		//	당사자.
        gamelog::ACT_CLUB_JOIN_MEMBER,	//	행위.
        gamelog::ID_CHAR, TarChaDbNum,	//	상대방.
        0,							//	exp
        0,							//	bright
        0,							//	life
        0,							//	money
		( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap()
    );

    // 클럽에 가입시킴.
    spClub->AddMember(TarChaDbNum, TargetChaName.c_str(), 0, TarChaLevel, LastLoginTime, true, ChaClass);
    //pTarChar->m_ClubDbNum = spClub->m_DbNum;

    // DB에 클럽 멤버 추가.
    AddGameAdoJob(
        db::DbActionPtr(
            new db::ClubMemberAdd(spClub->m_DbNum, TarChaDbNum)));	

    if (!FromChaName.empty())
    {
        // Club log/%1%님이 %2%님을 클럽에 가입시켰습니다.
        std::string ClubLog(
            sc::string::format(
                ID2GAMEINTEXT("CLUB_LOG_2"),
                FromChaName,
                TargetChaName));
        SENDCLUBLOG(ClubDbNum, club::LOG_MEMBER, ClubLog);
    }
    else
    {
        
        // Club log/%1%님이 클럽에 가입했습니다.
        std::string ClubLog(
            sc::string::format(
                ID2GAMEINTEXT("CLUB_LOG_7"),
                TargetChaName));
        SENDCLUBLOG(ClubDbNum, club::LOG_MEMBER, ClubLog);
    }

    // 가입 성공 FB.
    GLMSG::NET_CLUB_MEMBER_REQ_FB_AC NetMsgFB(bLobby);
    NetMsgFB.SetTarChaName(TargetChaName.c_str());
    NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_OK;
    SENDTOCLIENT(ClientSlot, &NetMsgFB);

    // Field에 정보 전송.
    GLMSG::SNET_CLUB_MEMBER_ADD_2FLD NetMsgAdd(
        spClub->m_DbNum,
        TarChaDbNum,
        TarChaLevel,
        TargetChaName.c_str(),
        ChaClass);
    SENDTOALLCHANNEL(&NetMsgAdd);

    // 가입된 캐릭터에게 클럽 정보 전송
    GLCharAG* pTarChar = GetCharByDbNum(TarChaDbNum);
    if (pTarChar)
    {
        pTarChar->SetClubDbNum( spClub->m_DbNum );

        m_pClubMan->SendClubInfo(pTarChar->ClientSlot(), spClub->DbNum());

        // 신규클럽 멤버 전송
        GLCLUBMEMBER* pNewMember = spClub->GetMember(TarChaDbNum);
        if (pNewMember)
        {            
            GLMSG::SNET_CLUB_MEMBER_2CLT SendData;
            SendData.ADDMEMBER(*pNewMember);
            msgpack::sbuffer sbuffer;
            msgpack::pack(sbuffer, SendData);
            spClub->SendClubClient(NET_MSG_GCTRL_CLUB_MEMBER_2CLT, sbuffer);
        }

        // Online
        GLMSG::NET_CHAR_STATE_ONLINE_AC SendData;
        SendData.m_ChaDbNum   = TarChaDbNum;
        SendData.m_CurrentMap = pTarChar->GetCurrentMap();
        SendData.m_fPositionX       = pTarChar->GetFieldMapPosition().x;
		SendData.m_fPositionY       = pTarChar->GetFieldMapPosition().y;
		SendData.m_fPositionZ       = pTarChar->GetFieldMapPosition().z;
        SendData.m_Channel    = pTarChar->GetChannel();

        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, SendData);

        spClub->SendClubClient(NET_MSG_CHAR_ONLINE_AC, SendBuffer);
    }

    return true;
}

bool GLAgentServer::AutoClubJoin( DWORD ClubDbNum, GLCharAG* pChar, DWORD ClientSlot )
{
	std::tr1::shared_ptr< GLClubAgent > spClub = GetClub( ClubDbNum );
	if ( !spClub )
		return false;

	if ( !pChar )
	{
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError(std::string("Can not find GetCharByDbNum on AutoClubJoin "));
		return false;
	}

	bool bFOUND = spClub->IsMember( pChar->ChaDbNum() );
	if ( bFOUND )
	{
		return true;
	}

	// 클럽에 가입시킴.
	bool bAddMember = spClub->AddMember( pChar->ChaDbNum(), pChar->GetChaName(), 0, pChar->GetChaLevel(), sc::time::GetCurrentTime(), true, pChar->GetClass() );
	if ( false == bAddMember )
	{
		return false;
	}

	// DB에 클럽 멤버 추가.
	AddGameAdoJob(
		db::DbActionPtr(
		new db::ClubMemberAdd( spClub->m_DbNum, pChar->ChaDbNum() ) ) );	

	// Club log/%1%님이 클럽에 가입했습니다.
	std::string ClubLog(
		sc::string::format(
		ID2GAMEINTEXT( "CLUB_LOG_7" ),
		pChar->GetChaName() ) );
	SENDCLUBLOG( ClubDbNum, club::LOG_MEMBER, ClubLog );

	// 클럽 가입 로그 기록.    
	// 통계 및 로그 수정
	LogCharAction(
		spClub->MasterCharDbNum(),			// 당사자.
		gamelog::ACT_CLUB_AUTO_JOIN_MEMBER,	// 행위.
		gamelog::ID_CHAR, pChar->ChaDbNum(),// 상대방.
		0,									// exp
		0,									// bright
		0,									// life
		0,									// money
		( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap()
		);

	// Field에 정보 전송.
	GLMSG::SNET_CLUB_MEMBER_ADD_2FLD NetMsgAdd(
		spClub->m_DbNum,
		pChar->ChaDbNum(),
		pChar->GetChaLevel(),
		pChar->GetChaName(),
		pChar->GetClass() );
	SENDTOALLCHANNEL( &NetMsgAdd );

	pChar->SetClubDbNum( spClub->m_DbNum );

	m_pClubMan->SendClubInfo( ClientSlot, spClub->DbNum() );

	// 신규클럽 멤버 전송
	GLCLUBMEMBER* pNewMember = spClub->GetMember( pChar->ChaDbNum() );
	if ( pNewMember )
	{            
		GLMSG::SNET_CLUB_MEMBER_2CLT SendData;
		SendData.ADDMEMBER( *pNewMember );
		msgpack::sbuffer sbuffer;
		msgpack::pack( sbuffer, SendData );
		spClub->SendClubClient( NET_MSG_GCTRL_CLUB_MEMBER_2CLT, sbuffer );
	}

	return true;

	// Online
	/*GLMSG::NET_CHAR_STATE_ONLINE_AC SendData;
	SendData.m_ChaDbNum   = pChar->ChaDbNum();
	SendData.m_CurrentMap = pChar->GetCurrentMap();
	SendData.m_fPositionX = pChar->GetFieldMapPosition().x;
	SendData.m_fPositionY = pChar->GetFieldMapPosition().y;
	SendData.m_fPositionZ = pChar->GetFieldMapPosition().z;
	SendData.m_Channel    = pChar->GetChannel();

	msgpack::sbuffer SendBuffer;
	msgpack::pack( SendBuffer, SendData );
	spClub->SendClubClient( NET_MSG_CHAR_ONLINE_AC, SendBuffer );*/
}

void GLAgentServer::AutoClubJoinForMigration( GLCharAG* pChar, DWORD ClientSlot )
{
	if ( !pChar )
	{
		sc::writeLogError( std::string( "Can not find GetCharByDbNum on AutoClubJoinForMigration" ) );
		return;
	}

	std::tr1::shared_ptr< GLClubAgent > spClub = GetClub( pChar->m_ClubDbNum );
	if ( !spClub )
		return;

	bool bFOUND = spClub->IsMember( pChar->ChaDbNum() );
	if ( bFOUND )
	{
		return;
	}

	// 클럽에 가입시킴.
	spClub->AddMember( pChar->ChaDbNum(), pChar->GetChaName(), 0, pChar->GetChaLevel(), sc::time::GetCurrentTime(), true, pChar->GetClass() );

	// db 에는 적용할 필요없다. 마이그레이션으로 이미 캐릭터정보에 클럽번호는 있다.

	// Club log/%1%님이 클럽에 가입했습니다.
	std::string ClubLog(
		sc::string::format(
		ID2GAMEINTEXT( "CLUB_LOG_7" ),
		pChar->GetChaName() ) );
	SENDCLUBLOG( pChar->m_ClubDbNum, club::LOG_MEMBER, ClubLog );

	// 클럽 가입 로그 기록.    
	// 통계 및 로그 수정
	LogCharAction(
		spClub->MasterCharDbNum(),			// 당사자.
		gamelog::ACT_CLUB_AUTO_JOIN_MEMBER,	// 행위.
		gamelog::ID_CHAR, pChar->ChaDbNum(),// 상대방.
		0,									// exp
		0,									// bright
		0,									// life
		0,									// money
		( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap()
		);

	// Field에 정보 전송.
	GLMSG::SNET_CLUB_MEMBER_ADD_2FLD NetMsgAdd(
		spClub->m_DbNum,
		pChar->ChaDbNum(),
		pChar->GetChaLevel(),
		pChar->GetChaName(),
		pChar->GetClass() );
	SENDTOALLCHANNEL( &NetMsgAdd );

	m_pClubMan->SendClubInfo( ClientSlot, spClub->DbNum() );

	// 신규클럽 멤버 전송
	GLCLUBMEMBER* pNewMember = spClub->GetMember( pChar->ChaDbNum() );
	if ( pNewMember )
	{            
		GLMSG::SNET_CLUB_MEMBER_2CLT SendData;
		SendData.ADDMEMBER( *pNewMember );
		msgpack::sbuffer sbuffer;
		msgpack::pack( sbuffer, SendData );
		spClub->SendClubClient( NET_MSG_GCTRL_CLUB_MEMBER_2CLT, sbuffer );
	}
}

//! 클럽 공지 변경
bool GLAgentServer::ClubNoticeChange(
    DWORD ClubDbNum,
    DWORD ChaDbNum,
    DWORD ClientSlot,
    const std::string& ChaName,
    const std::string& Notice)
{
    GLMSG::SNET_CLUB_NOTICE_FB NetMsgFB;
    if (ClubDbNum == CLUB_NULL)
    {
        NetMsgFB.emFB = EMCLUB_NOTICE_FB_FAIL;
        SENDTOCLIENT(ClientSlot, &NetMsgFB);
        return false;
    }

    std::tr1::shared_ptr<GLClubAgent> spClub = GetClub(ClubDbNum);
    if (!spClub)
    {
        NetMsgFB.emFB = EMCLUB_NOTICE_FB_FAIL;
        SENDTOCLIENT(ClientSlot, &NetMsgFB);
        return false;
    }

    if (!spClub->IsMemberFlgNotice(ChaDbNum))
    {
        NetMsgFB.emFB = EMCLUB_NOTICE_FB_NOTMATER;
        SENDTOCLIENT(ClientSlot, &NetMsgFB);
        return false;
    }

    // 클럽의 공지 내용 변경.
    CString strTemp(Notice.c_str());
    if (CRanFilter::GetInstance().ChatFilter(strTemp))
        strTemp = CRanFilter::GetInstance().GetProverb();

    // 만약 클럽 공지에 특수문자 및 공백이 있을 경우에 SQL인젝션의 가능성이 보이므로 로그를 남긴다.
    /*
    BOOL bFILTER = STRUTIL::CheckString(strTemp);
    if (bFILTER)
    {
        std::string HackLog(
            sc::string::format(
                "HACKING! Sql Injection check. ChaDbNum %1% Notice %2%",
                ChaDbNum,
                Notice));
        sc::writeLogError(HackLog);
    }
    */

    std::string NewNotice(strTemp.GetString());    
    spClub->SetNotice(NewNotice);
    spClub->SetNoticeDate(sc::time::GetCurrentTime());
    spClub->SetNoticeChaName(ChaName);

    AddGameAdoJob(
        db::DbActionPtr(
            new db::ClubNoticeSet(ChaName, ClubDbNum, NewNotice.c_str())));

    // 변경 성공 알림.
    NetMsgFB.emFB = EMCLUB_NOTICE_FB_OK;
    NetMsgFB.SetNotice(spClub->GetNotice());
    SENDTOCLIENT(ClientSlot, &NetMsgFB);

    // 클럽의 공지를 필드 서버에 반영.
    GLMSG::SNET_CLUB_NOTICE_FLD NetMsgFld(
        ClubDbNum,
        spClub->GetNoticeDate(),
        spClub->GetNotice(),
        spClub->GetNoticeChaName());    
    SENDTOALLCHANNEL(&NetMsgFld);

    // Club log/공지
    std::string ClubLog(
        sc::string::format(
            ID2GAMEINTEXT("CLUB_LOG_NOTICE"),
            ChaName));
    SENDCLUBLOG(ClubDbNum, club::LOG_CLUB_NOTICE, ClubLog);
    return true;
}

//! 클럽 계급 이름 변경
void GLAgentServer::ClubGradeNameChange(GLCharAG* pChar, size_t GradeIndex, const std::string& GradeName)
{
    if (!pChar)
        return;
    
    std::tr1::shared_ptr<GLClubAgent> spClub = GetClub(pChar->GetClubDbNum());
    if (!spClub)
        return;

	if( GradeName == "" && GradeName.c_str() == NULL && GradeName.empty() == true )
		return;

    // 이전 정보 저장
    club::AUTH_GRADE OldGradeInfo = spClub->GetAuth(GradeIndex);
    if (!spClub->ChangeGradeName(GradeIndex, GradeName, pChar->ChaDbNum()))
    {
        // 요청자에게 결과 전송/실패        
        NET_CLUB_GRADE_NAME_CHANGE_RESULT_AC SendData;
        SendData.m_Result = false;
        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, SendData);
        SENDTOCLIENT(pChar->ClientSlot(), NET_MSG_CLUB_GRADE_NAME_CHANGE_RESULT_AC, SendBuffer);
    }
    else
    {        
        // 요청자에게 결과 전송/성공 ---------------------------------------
        NET_CLUB_GRADE_NAME_CHANGE_RESULT_AC SendData1;
        SendData1.m_Result = true;
        msgpack::sbuffer SendBuffer1;
        msgpack::pack(SendBuffer1, SendData1);
        SENDTOCLIENT(pChar->ClientSlot(), NET_MSG_CLUB_GRADE_NAME_CHANGE_RESULT_AC, SendBuffer1);

        // 전체 클럽원들에게 결과 전송 ------------------------------------
        NET_CLUB_GRADE_NAME_CHANGE_AC SendData2;
        SendData2.m_Index     = GradeIndex;
        SendData2.m_GradeName = GradeName;
        msgpack::sbuffer SendBuffer2;
        msgpack::pack(SendBuffer2, SendData2);        
        SENDTOCLUBCLIENT(pChar->GetClubDbNum(), NET_MSG_CLUB_GRADE_NAME_CHANGE_AC, SendBuffer2);

        // 전체 필드서버에 변경 알림 ---------------------------------------
        NET_CLUB_GRADE_NAME_CHANGE_AF SendData3;
        SendData3.m_ClubDbNum = pChar->GetClubDbNum();
        SendData3.m_Index     = GradeIndex;
        SendData3.m_GradeName = GradeName;
        msgpack::sbuffer SendBuffer3;
        msgpack::pack(SendBuffer3, SendData3);
        SENDTOALLCHANNEL(NET_MSG_CLUB_GRADE_NAME_CHANGE_AF, SendBuffer3);

		std::string strChrName = pChar->GetChaName();
		if( this->GetServiceProvider() == SP_GS )
			strChrName =  sc::string::ansi_to_utf8( strChrName	);

        // Log 남기기
        // %1% 님이 %2% 계급 이름을 %3% 으로 변경하였습니다.
        std::string LogMsg(
            sc::string::format(
                ID2GAMEINTEXT("CLUB_LOG_9"),
                strChrName.c_str(),
                OldGradeInfo.m_Desc,
                GradeName));
        SENDCLUBLOG(pChar->GetClubDbNum(), club::LOG_CLUB_AUTH_CHANGE, LogMsg);
    }
}

//! 클럽 권한 변경 Client->Agent
BOOL GLAgentServer::MsgClubGradeFlagChangeCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
    GLCharAG* pChar = GetChar(dwGaeaID);
    if (!pChar)
        return FALSE;

    std::tr1::shared_ptr<GLClubAgent> spClub = GetClub(pChar->GetClubDbNum());
    if (spClub)
    {
        NET_MSG_PACK* pPacket = (NET_MSG_PACK*) nmg;
        NET_CLUB_GRADE_FLAG_CHANGE_CA RecvData;
        if (msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        {        
            spClub->SetGradeFlag(pChar, RecvData.m_Data);
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        return FALSE;
    }
}

BOOL GLAgentServer::MsgClubPublicFlagChangeCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
    GLCharAG* pChar = GetChar(dwGaeaID);
    if (!pChar)
        return FALSE;

    std::tr1::shared_ptr<GLClubAgent> spClub = GetClub(pChar->GetClubDbNum());
    if (spClub)
    {
        NET_CLUB_PULIC_FLAG_CHANGE_CA* pPacket = (NET_CLUB_PULIC_FLAG_CHANGE_CA*) nmg;
        spClub->PublicSet(pPacket->m_PublicFlag, pChar);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL GLAgentServer::MsgClubAutoKickCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
    GLCharAG* pChar = GetChar(dwGaeaID);
    if (!pChar)
        return FALSE;

    std::tr1::shared_ptr<GLClubAgent> spClub = GetClub(pChar->GetClubDbNum());
    if (spClub)
    {
        NET_CLUB_AUTO_KICKC_CA* pPacket = (NET_CLUB_AUTO_KICKC_CA*) nmg;
        spClub->AutoKickOutSet(pPacket->m_KickOut, pChar);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

//! 클럽 신입생 소개글
BOOL GLAgentServer::MsgClubNewbieNoticeCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) nmg;

    NET_CLUB_NEWBIE_NOTICE_CA RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return FALSE;

    GLCharAG* pChar = GetChar(dwGaeaID);
    if (!pChar)
        return FALSE;

    std::tr1::shared_ptr<GLClubAgent> spClub = GetClub(pChar->GetClubDbNum());
    if (spClub)
    {
        spClub->SetNoticeNewbie(RecvData.m_NoticeNewbie, pChar);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

void GLAgentServer::MsgClubNew2Agt(
    DWORD ClientSlot,
    DWORD ChaDbNum,
    wb::EM_COUNTRY Country,
    const char* ClubName,
    const std::string& ChaName,
    bool bLobby)
{
    if (!ClubName)
        return;
	
	// Lobby 에서 신청한다면 Char 가 존재하지 않는 상태이다.
    GLCharAG* pChar = GetCharByDbNum(ChaDbNum);	
    if (pChar)
    {
        ClientSlot = pChar->ClientSlot();
        ChaDbNum = pChar->ChaDbNum();
    }

	if (pChar && pChar->m_ClubDbNum != CLUB_NULL)
	{
        // 이미 캐릭터의 클럽이 있음
        GLMSG::NET_CLUB_NEW_FB_FC NetMsgFB(bLobby);
        NetMsgFB.emFB = EMCLUB_NEW_FB_FAIL;
        NetMsgFB.SetClubName(ClubName);
		SENDTOCLIENT(ClientSlot, &NetMsgFB);
		return;
	}
	
    /*
    if (pChar)
    {
        GLPARTY* pPARTY = GetParty(pChar->m_dwPartyID);
	    if ( !pPARTY || !pPARTY->ISMASTER(pChar->GaeaId()) )
	    {
            GLMSG::NET_CLUB_NEW_FB_FC NetMsgFB;
            NetMsgFB.SetClubName(ClubName);
		    NetMsgFB.emFB = EMCLUB_NEW_FB_NOTMASTER;
		    SENDTOCLIENT(ClientSlot, &NetMsgFB);
		    return FALSE;
	    }

	    if ( pPARTY->GETNUMBER() < GLCONST_CHAR::dwCLUB_PARTYNUM )
	    {
            GLMSG::NET_CLUB_NEW_FB_FC NetMsgFB;
            NetMsgFB.SetClubName(ClubName);
		    NetMsgFB.emFB = EMCLUB_NEW_FB_NOTMEMBER;
		    SENDTOCLIENT(ClientSlot, &NetMsgFB);
		    return S_FALSE;
	    }
        
        if (pPARTY)
        {
	        GLPARTY::MEMBER_ITER pos = pPARTY->m_cMEMBER.begin();
	        GLPARTY::MEMBER_ITER end = pPARTY->m_cMEMBER.end();
	        for ( ; pos!=end; ++pos )
	        {
		        GLCharAG* pMEMBER = GetChar ( (*pos) );
		        if ( !pMEMBER )
		        {
                    GLMSG::NET_CLUB_NEW_FB_FC NetMsgFB;
                    NetMsgFB.SetClubName(ClubName);
			        NetMsgFB.emFB = EMCLUB_NEW_FB_NOTEXIST_MEM;
			        SENDTOCLIENT ( dwClientID, &NetMsgFB );
			        return FALSE;
		        }

		        if ( pMEMBER->m_ClubDbNum != CLUB_NULL )
		        {
                    GLMSG::NET_CLUB_NEW_FB_FC NetMsgFB;
                    NetMsgFB.SetClubName(ClubName);
			        NetMsgFB.emFB = EMCLUB_NEW_FB_MEM_ALREADY;
			        SENDTOCLIENT ( dwClientID, &NetMsgFB );
			        return FALSE;
		        }
	        }
        }
    }
    */

	// 신청한 클럽 이름이 이미 사용중
	DWORD dwOLDGLUB = m_pClubMan->FindClub(ClubName);
	if (dwOLDGLUB != CLUB_NULL)
	{
        GLMSG::NET_CLUB_NEW_FB_FC NetMsgFB(bLobby);
        NetMsgFB.SetClubName(ClubName);
		NetMsgFB.emFB = EMCLUB_NEW_FB_NAME;
		SENDTOCLIENT(ClientSlot, &NetMsgFB);
		return;
	}

	// DB에 데이터 설정.	
    if (pChar)
    {
	    AddGameAdoJob(
            db::DbActionPtr(
                new db::ClubCreate(
                    ClientSlot,
                    std::string(ClubName),
                    pChar->GetChaName(),
                    ChaDbNum,
                    Country,
                    bLobby)));
    }
    else
    {
        AddGameAdoJob(
            db::DbActionPtr(
                new db::ClubCreate(
                    ClientSlot,
                    std::string(ClubName),
                    ChaName,
                    ChaDbNum,
                    Country,
                    bLobby)));
    }
}

BOOL GLAgentServer::MsgClubNewDb2Agt(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLMSG::SNET_CLUB_NEW_DB2AGT* pPacket = (GLMSG::SNET_CLUB_NEW_DB2AGT*) nmg;
    bool bLobby = pPacket->m_bLobby;
    
	GLCharAG* pMasterChar = GetCharByDbNum(pPacket->m_MasterDbNum);
	if (!bLobby && !pMasterChar)
	{
		// 클럽 마스터가 존재하지 않을 때는 클럽을 삭제한다.		
		AddGameAdoJob(
            db::DbActionPtr(
                new db::ClubDelete(
                    pPacket->m_ClubDbNum,
                    pPacket->m_MasterDbNum)));
		return FALSE;
	}

    DWORD ClientSlot = pPacket->m_ClientSlot;
    if (pMasterChar)
        ClientSlot = pMasterChar->ClientSlot();

	GLMSG::NET_CLUB_NEW_FB_FC NetMsgFB(bLobby);

	if (pPacket->emFB == EMCLUB_NEW_FB_ALREADY)
	{
		NetMsgFB.emFB = EMCLUB_NEW_FB_ALREADY;
		SENDTOCLIENT(ClientSlot, &NetMsgFB);
		return FALSE;
	}

	if (pPacket->emFB == EMCLUB_NEW_FB_NAME)
	{
		NetMsgFB.emFB = EMCLUB_NEW_FB_NAME;
		NetMsgFB.SetClubName(pPacket->m_ClubName);
		SENDTOCLIENT(ClientSlot, &NetMsgFB);
		return FALSE;
	}

	if (pPacket->emFB == EMCLUB_NEW_FB_FAIL)
	{
		NetMsgFB.emFB = EMCLUB_NEW_FB_FAIL;
		SENDTOCLIENT(ClientSlot, &NetMsgFB);
		return FALSE;
	}

    DWORD NewClubDbNum = pPacket->m_ClubDbNum;
    DWORD MasterDbNum = pPacket->m_MasterDbNum;
    std::string MasterName(pPacket->m_ChaName);
    std::string ClubName(pPacket->m_ClubName);
    
    NetMsgFB.SetClubName(ClubName.c_str());
    NetMsgFB.emFB = EMCLUB_NEW_FB_FAIL;

    if (bLobby)
    {
        NetMsgFB.emFB = EMCLUB_NEW_FB_OK;
        SENDTOCLIENT(ClientSlot, &NetMsgFB);
    }

	const GLPartyAgent* const pParty = pMasterChar ? GetParty(pMasterChar->GetPartyID()) : NULL;
    /*
    if (!bLobby && pChar)
    {	    
        // 파티가 없어도 클럽생성은 가능
	    if (!pPARTY || !pPARTY->ISMASTER(pChar->GaeaId()))
	    {        
		    NetMsgFB.emFB = EMCLUB_NEW_FB_NOTMASTER;
		    SENDTOCLIENT(ClientSlot, &NetMsgFB);
    		
		    AddGameDbJob(
                db::DbActionPtr(
                    new db::ClubDelete(
                        NewClubDbNum,
                        MasterDbNum)));
		    return FALSE;
	    }
        if (pPARTY)
        {
	        if (pPARTY->GETNUMBER() < GLCONST_CHAR::dwCLUB_PARTYNUM)
	        {
		        NetMsgFB.emFB = EMCLUB_NEW_FB_NOTMEMBER;
		        SENDTOCLIENT(ClientSlot, &NetMsgFB);
        		
		        AddGameDbJob(
                    db::DbActionPtr(
                        new db::ClubDelete(
                            NewClubDbNum,
                            MasterDbNum)));
		        return S_FALSE;
	        }
        }
    }
    */

	// 클럽 설정.
	std::tr1::shared_ptr<GLClubAgent> spClubNew = std::tr1::shared_ptr<GLClubAgent> (new GLClubAgent(this));

	spClubNew->m_DbNum = NewClubDbNum;
	spClubNew->SetName(ClubName);
	
	CTime cTIME_CUR = CTime::GetCurrentTime();
	spClubNew->m_tOrganize = cTIME_CUR.GetTime();

	spClubNew->SetMasterCharDbNum(MasterDbNum);
	spClubNew->SetMasterName(MasterName);
    spClubNew->SetMasterCharLevel( pMasterChar->GetChaLevel() );
    // 국가 설정
    spClubNew->SetCountry(pPacket->m_Country);

	// 케릭터 개체에 클럽 ID 지정.
    if (pMasterChar)
    {
	    //pChar->m_dwGuild = cClubNew.m_dwID;
        pMasterChar->m_ClubDbNum = spClubNew->m_DbNum;
    }

	CTime cTIME_DIS(1970,2,1,1,1,1);
	spClubNew->m_tDissolution = cTIME_DIS.GetTime();

    // Master를 멤버원 등록
    GLCLUBMEMBER MemberMaster;
    MemberMaster.m_ChaDbNum = MasterDbNum;
    MemberMaster.SetName(MasterName);
    if (pMasterChar)
    {
        MemberMaster.m_ChaLevel = pMasterChar->GetChaLevel();
        MemberMaster.m_Channel  = pMasterChar->GetChannel();
        MemberMaster.m_CurrentMap = pMasterChar->GetCurrentMap();
        MemberMaster.m_Grade      = 0;

        D3DXVECTOR3 MasterPos(0.0f, 0.0f, 0.0f);
        ConvertMapToWorld(pMasterChar->GetCurrentMap(), pMasterChar->GetFieldMapPosition().x, pMasterChar->GetFieldMapPosition().z, MasterPos);

        MemberMaster.m_CurrentPos = MasterPos;
        MemberMaster.m_Online = true;
        MemberMaster.m_LastPlayTime = sc::time::GetCurrentTime();
        MemberMaster.m_ChaClass = pMasterChar->GetClass();
    }

    spClubNew->AddMemberEx(MasterDbNum, MemberMaster);
    AddGameAdoJob(
        db::DbActionPtr(
            new db::ClubMemberAdd(
                spClubNew->DbNum(),
                MasterDbNum)));

	// 멤버원 등록
    if (pParty)
	{
		GLPartyAgent::MEMBER_CITER pos = pParty->m_Member.begin();
		GLPartyAgent::MEMBER_CITER end = pParty->m_Member.end();
		for ( ; pos!=end; ++pos)
		{
			GLCharAG* pPartyMember = GetChar((*pos));
			if (!pPartyMember || pPartyMember->m_ClubDbNum != CLUB_NULL)
                continue;

			// 케릭터 개체에 클럽 ID 지정.
			pPartyMember->m_ClubDbNum = spClubNew->DbNum();

            D3DXVECTOR3 MemberPos(0.0f, 0.0f, 0.0f);
			
            ConvertMapToWorld(pPartyMember->GetCurrentMap(), pPartyMember->GetFieldMapPosition().x, pPartyMember->GetFieldMapPosition().z, MemberPos);

			GLCLUBMEMBER NewMember(
                pPartyMember->ChaDbNum(),
			    pPartyMember->GetChaLevel(),
                pPartyMember->GetChannel(),
                pPartyMember->GetCurrentMap(),
                0,
                MemberPos,
                pPartyMember->GetChaName(),
                true,
                sc::time::GetCurrentTime(),
                pPartyMember->GetClass());

			spClubNew->AddMemberEx(NewMember.m_ChaDbNum, NewMember);

			// DB에 멤버 설정.			
			AddGameAdoJob(
                db::DbActionPtr(
                    new db::ClubMemberAdd(
                        spClubNew->DbNum(),
                        NewMember.m_ChaDbNum)));

            // Club log/%1%님이 클럽에 가입하셨습니다.
            std::string ClubLog(
                sc::string::format(
                    ID2GAMEINTEXT("CLUB_LOG_1"),
                    pPartyMember->GetChaName()));
            SENDCLUBLOG(spClubNew->DbNum(), club::LOG_MEMBER, ClubLog);
		}
	}    

    // 이제 막 생성한 클럽이기 때문에 권한 정보가 없다.
    // 기본값으로 세팅한다.
    size_t GradeSize = GLogicData::GetInstance().GetClubGradeSize();
    for (size_t i=0; i<GradeSize; ++i)
        spClubNew->AddAuth(i, GLogicData::GetInstance().GetClubBaseGrade(i));

	m_pClubMan->AddClub(spClubNew);

	// 클럽 생성 성공.
	GLMSG::SNET_CLUB_NEW_2FLD NetMsgFLD;
	NetMsgFLD.dwChaNum = MasterDbNum;
	NetMsgFLD.emFB = EMCLUB_NEW_FB_OK;
	NetMsgFLD.dwCLUBID = spClubNew->m_DbNum;
	NetMsgFLD.SetName(spClubNew->Name());
	SENDTOFIELD(ClientSlot, &NetMsgFLD);

	// 생성 정보 전송.
	//
	GLMSG::NET_NEW_CLUB_INFO_AF NetMsgInfo(spClubNew->GetCountry());
	NetMsgInfo.dwClubID = spClubNew->m_DbNum;
	NetMsgInfo.SetClubName(spClubNew->Name());

	NetMsgInfo.dwMasterID = spClubNew->MasterCharDbNum();
	NetMsgInfo.SetMasterName(spClubNew->GetMasterName());
    NetMsgInfo.wMasterLevel = spClubNew->GetMasterLevel();
	NetMsgInfo.dwCDCertifior = spClubNew->m_dwCDCertifior;

	NetMsgInfo.dwRank = spClubNew->m_dwRank;
	NetMsgInfo.dwMarkVER = spClubNew->m_dwMarkVER;

	NetMsgInfo.tOrganize = spClubNew->m_tOrganize;
	NetMsgInfo.tDissolution = spClubNew->m_tDissolution;

	NetMsgInfo.dwMemberNum = spClubNew->GetMemberNum();
	{
		DWORD i=0;
        CLUBMEMBERS& Members = spClubNew->GetMembers();
		for (CLUBMEMBERS_ITER pos=Members.begin(); pos!=Members.end(); ++pos)
		{
			const GLCLUBMEMBER& sCLUBMEMBER = pos->second;
			NetMsgInfo.SetMemberName(i, sCLUBMEMBER.m_ChaName.c_str());
			NetMsgInfo.aMembers[i] = sCLUBMEMBER.m_ChaDbNum;
			++i;

			GLCharAG* pTarChar = GetCharByDbNum(sCLUBMEMBER.m_ChaDbNum);
			if ( !pTarChar )
			{		
				// 통계 및 로그 수정
				// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
				sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgClubNewDb2Agt ", sCLUBMEMBER.m_ChaDbNum));
			}

			// 클럽 가입 로그 기록.
			// 통계 및 로그 수정
			LogCharAction(
				spClubNew->MasterCharDbNum(),		//	당사자.
				gamelog::ACT_CLUB_JOIN_MEMBER,	//	행위.
				gamelog::ID_CHAR, sCLUBMEMBER.m_ChaDbNum,	//	상대방.
				0,							//	exp
				0,							//	bright
				0,							//	life
				0,							//	money
				( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap()
			);
		}
	}
	SENDTOALLCHANNEL(&NetMsgInfo);

    // 클럽 권한 전체 field server 에 전송
    GradeSize = GLogicData::GetInstance().GetClubGradeSize();
    for (size_t i=0; i<GradeSize; ++i)
    {
        club::AUTH_GRADE AuthGrade = spClubNew->GetAuth(i);
        GLMSG::NET_CLUB_AUTH_AF MsgAF(spClubNew->DbNum(), static_cast<BYTE> (i), AuthGrade);
        SENDTOALLCHANNEL(&MsgAF);
    }
        
    // 클럽원들에게 클럽 정보 전송
    m_pClubMan->SendClubInfoToAllMember(spClubNew->DbNum());

	return TRUE;
}

BOOL GLAgentServer::MsgClubDissolution ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNET_CLUB_DISSOLUTION *pNetMsg = (GLMSG::SNET_CLUB_DISSOLUTION *) nmg;

	GLCharAG* pChar = GetChar(dwGaeaID);
	if (!pChar)
        return FALSE;

	GLMSG::SNET_CLUB_DISSOLUTION_FB NetMsgFB;

	if ( pChar->m_ClubDbNum == CLUB_NULL )
	{
		NetMsgFB.emFB = EMCLUB_DIS_FB_FAIL;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	std::tr1::shared_ptr<GLClubAgent> pCLUB = m_pClubMan->GetClub ( pChar->m_ClubDbNum );
	if ( !pCLUB )
	{
		NetMsgFB.emFB = EMCLUB_DIS_FB_FAIL;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	if ( !pCLUB->IsMaster( pChar->m_CharDbNum ) )
	{
		NetMsgFB.emFB = EMCLUB_DIS_FB_FAIL;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	//	Memo :	동맹에 가입되어 있다면 취소.
	if ( pCLUB->IsAlliance() )
	{
		NetMsgFB.emFB = EMCLUB_DIS_FB_FAIL;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	if ( pCLUB->GetAllBattleNum() > 0 )
	{
		NetMsgFB.emFB = EMCLUB_DIS_FB_CLUBBATTLE;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	if ( pNetMsg->bCANCEL )
	{
		if ( !pCLUB->IsRegDissolution() )
		{
			NetMsgFB.emFB = EMCLUB_DIS_FB_FAIL;
			SENDTOCLIENT ( dwClientID, &NetMsgFB );
			return FALSE;
		}

		pCLUB->DoDissolutionCancel ();

		// DB에 클럽 해산 시간 설정.		
		AddGameDbJob(
            db::DbActionPtr(
                new db::ClubDissolutionTimeSet(
                    pCLUB->m_DbNum,
                    pCLUB->m_tDissolution)));

		// 해체 취소 to FIELD
		//
		GLMSG::SNET_CLUB_DISSOLUTION_2FLD NetMsgFld;
		NetMsgFld.dwClubID = pCLUB->m_DbNum;
		CTime tDisTime(1970,2,1,1,1,1);
		NetMsgFld.tDissolution = tDisTime.GetTime();
		SENDTOALLCHANNEL ( &NetMsgFld );

		// 해체 취소 FB
		//
		NetMsgFB.emFB = EMCLUB_DIS_FB_CANCEL;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
	}
	else
	{
		if ( pCLUB->IsRegDissolution() )
		{
			NetMsgFB.emFB = EMCLUB_DIS_FB_FAIL;
			SENDTOCLIENT ( dwClientID, &NetMsgFB );
			return FALSE;
		}

		// 해체 예정일 지정.
		CTime tDisTime = CTime::GetCurrentTime();
		CTimeSpan tADD(GLCONST_CHAR::dwCLUB_DISSOLUTION_DAY,0,10,0);
		tDisTime += tADD;

		pCLUB->DoDissolution(tDisTime.GetTime());

		// DB에 클럽 해산 시간 설정.		
		AddGameDbJob(
            db::DbActionPtr(
                new db::ClubDissolutionTimeSet(
                    pCLUB->m_DbNum,
                    pCLUB->m_tDissolution)));

		// 해체 FB.
		//
		NetMsgFB.emFB = EMCLUB_DIS_FB_OK;
		NetMsgFB.tDissolution = pCLUB->m_tDissolution;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );

		// 해체 to FIELD
		//
		GLMSG::SNET_CLUB_DISSOLUTION_2FLD NetMsgFld;
		NetMsgFld.dwClubID = pCLUB->m_DbNum;
		NetMsgFld.tDissolution = pCLUB->m_tDissolution;
		SENDTOALLCHANNEL ( &NetMsgFld );
	}

	return TRUE;
}

BOOL GLAgentServer::MsgClubCertified ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::NET_CLUB_CERTIFIED_FA *pNetMsg = (GLMSG::NET_CLUB_CERTIFIED_FA *) nmg;
	m_pGuidance->ChangeGuidClub(pNetMsg->m_GuidanceId, pNetMsg->m_ClubDbNum);
	return TRUE;
}

BOOL GLAgentServer::MsgClubIncomeMoney ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::NET_CLUB_INCOME_MONEY_FA* pPacket = (GLMSG::NET_CLUB_INCOME_MONEY_FA*) nmg;    
    DWORD ClubDbNum = pPacket->m_ClubDbNum;

	// dwGaeaID 는 GAEAID_NULL 이다 확인 불필요
	/*GLCharAG* pChar = GetCharByGaeaID(dwGaeaID);
	if (!pChar)
	{
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgClubIncomeMoney ", dwGaeaID));
	}*/

    if (dwGaeaID != GAEAID_NULL)
    {
        sc::writeLogError(
            sc::string::format(
            "HACKING! MsgClubIncomeMoney ClubNum %1% GaeaID not null.",
            ClubDbNum));
        return FALSE;
    }

    if (pPacket->m_MagicNum != GLMSG::NET_CLUB_INCOME_MONEY_FA::MAGIC_NUM)
    {
        sc::writeLogError(
            sc::string::format(
                "HACKING! MsgClubIncomeMoney ClubNum %1% MagicNum %2%",
                ClubDbNum,
                pPacket->m_ProfitMoney));
        return FALSE;
    }
    
	std::tr1::shared_ptr<GLClubAgent> pCLUB = m_pClubMan->GetClub(ClubDbNum);
	if (!pCLUB)
        return FALSE;

	if (pPacket->m_ProfitMoney <= 0)
        return FALSE;

	// 클럽의 수익을 반영.
    LONGLONG AddIncomeMoney = LONGLONG(pPacket->m_ProfitMoney * GLCONST_CHAR::fEARNING_RATE + 0.1f);
	pCLUB->AddIncomeMoney(AddIncomeMoney);

	// 수익로그
	// 통계 및 로그 수정
    m_pItemLimit->LogMoneyExchange(
        gamelog::ID_CLUB, ClubDbNum,
        gamelog::ID_CLUB, ClubDbNum,
        AddIncomeMoney,
        MONEY_ROUTE_CLUB_INCOME,
		MapID( SNATIVEID( false ), SNATIVEID( false ) ), LOG_MONEY_CLUB_GUIDANCE_COMMISSION);

    // Club log
    std::string ClubLog(
        sc::string::format(
        ID2GAMEINTEXT("CLUB_LOG_17"),
        AddIncomeMoney));
    SENDCLUBLOG(ClubDbNum, club::LOG_CLUB_STORAGE, ClubLog);

    LONGLONG IncomeMoney = pCLUB->GetIncomeMoney();
//     sc::writeLogInfo(
//         sc::string::format(
//             "ClubIncomeMoney MsgClubIncomeMoney ClubID %1% MoneyFromField %2% IncomeMoney %3%",
//             ClubDbNum,
//             AddIncomeMoney,
//             IncomeMoney));

	// Db에 저장.	
	AddGameDbJob(
        db::DbActionPtr(
            new db::ClubIncomeMoneySet(
                ClubDbNum,
                IncomeMoney)));

	// 클럽원에게 통보;
	GLMSG::SNET_CLUB_STORAGE_UPDATE_MONEY NetMsgClt(
		pCLUB->GetStorageMoney(), pCLUB->GetIncomeMoney() );
	pCLUB->SendClubClient( &NetMsgClt );

	return TRUE;
}

/*
BOOL GLAgentServer::MsgClubIncomeMoneyUp ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNET_CLUB_INCOME_UP *pNetMsg = (GLMSG::SNET_CLUB_INCOME_UP *) nmg;

	std::tr1::shared_ptr<GLClubAgent> pCLUB = m_pClubMan->GetClub ( pNetMsg->dwClubID );
	if ( !pCLUB )
        return FALSE;

	// 클럽의 수익을 반영.
	if ( pCLUB->GetIncomeMoney() > 0 )
	{
		GLMSG::SNET_CLUB_INCOME_DN NetMsgDn;
		NetMsgDn.dwClubID = pCLUB->m_DbNum;
		NetMsgDn.lnInComeMoney = pCLUB->GetIncomeMoney();
		SENDTOALLCHANNEL(&NetMsgDn);

		pCLUB->SetIncomeMoney(0);

        sc::writeLogInfo(
        sc::string::format((
            "MsgClubIncomeMoneyUp ClubID %1% IncomeMoney %2%",
                pCLUB->m_DbNum,
                NetMsgDn.lnInComeMoney));

		// Db에 저장.		
		AddGameDbJob(
            db::DbActionPtr(
                new db::ClubIncomeMoneySet(
                    pCLUB->m_DbNum,
                    pCLUB->GetIncomeMoney())));
	}
	return TRUE;
}
*/
BOOL GLAgentServer::MsgClubMemberDb2Del(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLMSG::SNET_CLUB_MEMBER_DB2DEL* pNetMsg = (GLMSG::SNET_CLUB_MEMBER_DB2DEL*) nmg;

	if (pNetMsg->dwClub == CLUB_NULL)
		return FALSE;
	
	std::tr1::shared_ptr<GLClubAgent> spClub = m_pClubMan->GetClub(pNetMsg->dwClub);
	if (spClub)
    {
        spClub->DelMember(pNetMsg->dwMember, 0, std::string(""));
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL GLAgentServer::MsgClubJoinTimeResetCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
    GLCharAG* pChar = GetCharByGaeaID(dwGaeaID);
    if (!pChar)
        return FALSE;

    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) nmg;
    NET_CLUB_JOIN_TIME_RESET_CA RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return FALSE;

    NET_CLUB_JOIN_TIME_RESET_AC SendData(RecvData.m_ChaName, NET_CLUB_JOIN_TIME_RESET_AC::NOT_EXIST_CHAR);
    msgpack::sbuffer SendBuffer;

    if (pChar->UserType() <= USER_GM4)
    {
        SendData.m_Result = NET_CLUB_JOIN_TIME_RESET_AC::NOT_PERMISSION;
        msgpack::pack(SendBuffer, SendData);
        SENDTOCLIENT(dwClientID, NET_MSG_CLUB_JOIN_TIME_RESET_AC, SendBuffer);
        return FALSE;
    }    

    GLCharAG* pTarChar = GetChar(RecvData.m_ChaName);
    if (!pTarChar)
    {
        SendData.m_Result = NET_CLUB_JOIN_TIME_RESET_AC::NOT_EXIST_CHAR;
        msgpack::pack(SendBuffer, SendData);
        SENDTOCLIENT(dwClientID, NET_MSG_CLUB_JOIN_TIME_RESET_AC, SendBuffer);
        return FALSE;
    }

    // 604800 = 7day (sec)
    __time64_t NewSecedeTime = sc::time::GetCurrentTime() - 604800;
    pTarChar->SetClubSecedeTime(NewSecedeTime);
    SendData.m_Result = NET_CLUB_JOIN_TIME_RESET_AC::SUCCESS;
    
    // Client 에 알림
    msgpack::pack(SendBuffer, SendData);
    SENDTOCLIENT(dwClientID, NET_MSG_CLUB_JOIN_TIME_RESET_AC, SendBuffer);

    // DB 업데이트
    AddGameAdoJob(
        db::DbActionPtr(
            new db::ChaClubSecedeTimeReset(pTarChar->ChaDbNum())));

    // Field 에 알림
    NET_CLUB_JOIN_TIME_RESET_AF SendDataAF(pTarChar->ChaDbNum(), pTarChar->GetClubSecedeTime());
    msgpack::sbuffer SendBufferAF;
    msgpack::pack(SendBufferAF, SendDataAF);
    SENDTOALLCHANNEL(NET_MSG_CLUB_JOIN_TIME_RESET_AF, SendBufferAF);

    return TRUE;
}

BOOL GLAgentServer::MsgClubAllianceTimeResetCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
    GLCharAG* pChar = GetCharByGaeaID(dwGaeaID);
    if (!pChar)
        return FALSE;

    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) nmg;
    NET_CLUB_ALLIANCE_TIME_RESET_CA RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return FALSE;

    NET_CLUB_ALLIANCE_TIME_RESET_AC SendDataAC(RecvData.m_ClubName, NET_CLUB_ALLIANCE_TIME_RESET_AC::NOT_EXIST_CLUB);
    msgpack::sbuffer SendBufferAC;

    if (pChar->UserType() <= USER_GM4)
    {
        SendDataAC.m_Result = NET_CLUB_ALLIANCE_TIME_RESET_AC::HAVE_NOT_PERMISSION;
        msgpack::pack(SendBufferAC, SendDataAC);
        SENDTOCLIENT(dwClientID, NET_MSG_CLUB_ALLIANCE_TIME_RESET_AC, SendBufferAC);
        return FALSE;
    }

    std::tr1::shared_ptr<GLClubAgent> spClub = GetClub(RecvData.m_ClubName);
    if (!spClub)
    {
        SendDataAC.m_Result = NET_CLUB_ALLIANCE_TIME_RESET_AC::NOT_EXIST_CLUB;
        msgpack::pack(SendBufferAC, SendDataAC);
        SENDTOCLIENT(dwClientID, NET_MSG_CLUB_ALLIANCE_TIME_RESET_AC, SendBufferAC);
        return FALSE;
    }

    // 604800 = 7day (sec)
    __time64_t NewTime = sc::time::GetCurrentTime() - 604800;    
    spClub->SetAllianceSec(NewTime);    
    spClub->SetAllianceDis(NewTime);

    // client 알림
    SendDataAC.m_Result = NET_CLUB_ALLIANCE_TIME_RESET_AC::SUCCESS;
    msgpack::pack(SendBufferAC, SendDataAC);
    SENDTOCLIENT(dwClientID, NET_MSG_CLUB_ALLIANCE_TIME_RESET_AC, SendBufferAC);

    // db 업데이트
    AddGameDbJob(
        db::DbActionPtr(
            new db::ClubAllianceDisolveTimeSet(spClub->DbNum(), NewTime)));

    AddGameDbJob(
        db::DbActionPtr(
            new db::ClubAllianceTimeSet(spClub->DbNum(), NewTime)));

    // field 알림
    NET_CLUB_ALLIANCE_TIME_RESET_AF SendDataAF(spClub->DbNum(), NewTime);
    msgpack::sbuffer SendBufferAF;
    msgpack::pack(SendBufferAF, SendDataAF);
    SENDTOALLCHANNEL(NET_MSG_CLUB_ALLIANCE_TIME_RESET_AF, SendBufferAF);
    return TRUE;
}

BOOL GLAgentServer::MsgClubStorageUserReset( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNET_CLUB_STORAGE_USER_RESET* pPacket = ( GLMSG::SNET_CLUB_STORAGE_USER_RESET* ) nmg;

	std::tr1::shared_ptr< GLClubAgent > spClub = GetClub( pPacket->ClubNum );
	if ( !spClub )
		return FALSE;

	spClub->ResetClubStorageUser();

	return TRUE;
}

BOOL GLAgentServer::MsgClubStorageMoneyUserReset( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNET_CLUB_STORAGE_MONEY_USER_RESET* pPacket = ( GLMSG::SNET_CLUB_STORAGE_MONEY_USER_RESET* ) nmg;

	std::tr1::shared_ptr< GLClubAgent > spClub = GetClub( pPacket->ClubNum );
	if ( !spClub )
		return FALSE;
	
	spClub->ResetClubStorageMoneyUser();
	
	return TRUE;
}

BOOL GLAgentServer::MsgClubStorageGetDbEnd( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	if ( DBACTION_CLIENT_ID != dwClientID || GAEAID_NULL != dwGaeaID )
	{
		return FALSE;
	}

	NET_MSG_PACK* pPacket = ( NET_MSG_PACK* ) nmg;    
	GLMSG::SNET_CLUB_STORAGE_GET_DB_END Data;
	if ( !msgpack::BufferToObject( pPacket->DataBuffer(), pPacket->DataSize(), Data ) )
		return FALSE;

	std::tr1::shared_ptr< GLClubAgent > pClub = GetClub( Data.m_dwClubID );
	if (!pClub)
	{
		return FALSE;
	}

	// 기존에 클럽 락커의 아이템이 바이너리로 되어있을 때는 패킷안에 포인터를 넣어서 한번의 메시지로 처리하였다.
	// 이제는 아이템 하나씩 메시지로 보내준다. 메시지를 보내고 받다가 캐릭터가 없을 수도 있다. 
	// 그리고, dbaction을 처음에 호출할 때 캐릭터 검증은 한 상태이고, 그전에 없던 클라이언트 슬롯 체크를 추가하였다.
	// 클럽 락커 아이템을 처음에 받아오는 방식을 변경을 고려해야 한다.
	pClub->SetValidStorage( true );
	pClub->SetStorageMoney( Data.m_lnStorageMoney );

	switch ( GetServiceProvider() )
	{
	case SP_GS:
		{
			if ( FALSE == pClub->ValidateVehicleRandomOption() )
			{
				sc::writeLogError( sc::string::format( 
					"ValidateVehicleRandomOption() clublocker club %1%", pClub->m_DbNum ) );
			}

			// 서버 xml에 설정값 추가해서 하지않고 우선 이렇게 처리한다. GS 3,4 서버만 대상이다.
			int ServerGroup = GetServerGroup();
			if ( 3 == ServerGroup || 4 == ServerGroup )
			{
				std::vector< SITEMCUSTOM > vecDeleteItems;
				if ( FALSE == pClub->ValidateRandomOption( vecDeleteItems ) )
				{
					sc::writeLogInfo( sc::string::format( 
						"ValidateRandomOption() clublocker club %1%", pClub->m_DbNum ) );
				}

				size_t DeleteItemSize = vecDeleteItems.size();
				for ( size_t loop = 0; loop < DeleteItemSize; loop++ )
				{
					sc::MGUID guid = vecDeleteItems[loop].GetGuid();
					int DbState = vecDeleteItems[loop].GetDbState();

					AddGameAdoJob(
						db::DbActionPtr( 
						new db::ItemDelete( guid, DbState, 59908 ) ) );
				}
			}
		}
		break;
	case SP_THAILAND:
		{
			std::vector< SITEMCUSTOM > vecPostItems;
			std::vector< SINVENITEM* > vecUnWrappedItems;
			if ( FALSE == pClub->ValidateWrappedItem( vecPostItems, vecUnWrappedItems ) )
			{
				sc::writeLogError( sc::string::format( 
					"ValidateWrappedItem() clublocker club %1%", pClub->m_DbNum ) );
			}

			// 포장지 제거된 아이템 저장
			size_t UnWrappedItemSize = vecUnWrappedItems.size();
			for ( size_t loop = 0; loop < UnWrappedItemSize; loop++ )
			{
				SINVENITEM* pInvenItem = vecUnWrappedItems[loop];
				if ( !pInvenItem )
				{
					continue;
				}

				AddGameAdoJob(
					db::DbActionPtr( 
					new db::EntireItemUpdate( pClub->DbNum(), INVEN_CLUB_LOCKER, pInvenItem->sSaveData, 59709 ) ) );
			}

			// 우편 전송
			size_t PostItemSize = vecPostItems.size();
			for ( size_t loop = 0; loop < PostItemSize; loop++ )
			{
				std::string strRecverName( pClub->GetMasterName() ), strSenderName( "System" );
				BOOL bResult = PostSendSystemMail(
					pClub->MasterCharDbNum(), 
					strRecverName, 
					vecPostItems[loop], 
					0, 
					ATTENDANCE_REWARD_POST_DESIGN, 
					ATTENDANCE_REWARD_POST_SENDER, 
					strSenderName, 
					std::string( ID2SERVERTEXT( "WRAPPER_ITEM_RESTORE_POST_TITLE" ) ), 
					std::string( ID2SERVERTEXT( "WRAPPER_ITEM_RESTORE_POST_CONTENTS" ) ), 
					false
					);

				if ( FALSE == bResult )
				{
					sc::writeLogInfo( sc::string::format( 
						"GLItemValidation::ValidateWrappedItem() PostSendSystemMail fail. clubmaster chanum %1% mid %2% sid %3%", 
						pClub->MasterCharDbNum(), vecPostItems[loop].GetNativeID().wMainID, vecPostItems[loop].GetNativeID().wSubID ) );
				}
			}
		}
		break;
	default:
		break;
	}

	GLCharAG* pChar = GetCharByDbNum( Data.m_dwCharID );
	if ( !pChar )
	{
		return FALSE;
	}

	if ( pChar->m_ClubDbNum != pClub->m_DbNum )
	{
		return FALSE;
	}

	MsgClubStorage( pClub, pChar->ClientSlot() );

	return TRUE;    
}

BOOL GLAgentServer::MsgClubNameChangeFA( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNET_CLUB_RENAME_FA* pPacket = ( GLMSG::SNET_CLUB_RENAME_FA* ) nmg;
	
	if ( dwGaeaID != GAEAID_NULL || pPacket->MagicNum != GLMSG::SNET_CLUB_RENAME_FA::MAGIC_NUM )
		return FALSE;

	DWORD ClubNum = pPacket->ClubNum;
	DWORD ChaDbNum = pPacket->ChaDbNum;
	std::string strChaName( "unknown" );

	GLCharAG* pChar = GetCharByDbNum( ChaDbNum );
	if ( pChar )
	{
		strChaName = pChar->GetChaName();
	}

	// 이름 검사는 이미 다 한 상태이다.
	if ( false == m_pClubMan->ClubRename( ClubNum, std::string( pPacket->szClubName ) ) )
	{
		sc::writeLogError( sc::string::format( 
			"MsgClubNameChangeFA ClubRename fail clubnum %1% chanum %2% newclubname %3%", 
			ClubNum, ChaDbNum, pPacket->szClubName ) );
		return FALSE;
	}

	// 필드 서버에 알림.
	GLMSG::SNET_CLUB_RENAME_AF NetMsgFld;
	NetMsgFld.ClubNum = ClubNum;
	NetMsgFld.SetClubName( pPacket->szClubName );
	SENDTOALLCHANNEL( &NetMsgFld );

	// 클럽로그 남김    
	std::string ClubLog( ID2GAMEINTEXT( "CLUB_LOG_33" ) );
	SENDCLUBLOG( ClubNum, club::LOG_CLUB_RENAME, ClubLog );

	return TRUE;
}
