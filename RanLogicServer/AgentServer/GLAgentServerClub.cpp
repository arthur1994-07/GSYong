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
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
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

    // Db�� ����.		
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

	// ��� �� �α� ����
    m_pItemLimit->LogMoneyExchange(gamelog::ID_CLUB, 0, gamelog::ID_CLUB, ClubDbNum, IncomeMoney, MONEY_ROUTE_CLUB_INCOME, ( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap(), LOG_MONEY_CLUB_GUIDANCE_COMMISSION);
	// ��� �� �α� ����
    m_pItemLimit->LogMoneyExchange(gamelog::ID_CLUB, 0, gamelog::ID_CLUB, ClubDbNum, pClub->GetStorageMoney(), MONEY_ROUTE_CLUB_TOTAL_MONEY, ( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap(), LOG_MONEY_CLUB_STORAGE_CURRENT);    

//     sc::writeLogInfo(
//         sc::string::format(
//             "ClubIncomeMoney ServerClubInComeDn ClubID %1% ChaDbNum %2% IncomeMoney %3%",
//             ClubDbNum,
//             pChar->m_CharDbNum,
//             IncomeMoney));
    
    // Ŭ�������� �뺸
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
	// Ŭ����Ŀ ��� ���� ��û
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
        //�������� �ִ���...����
        pClub->SetValidStorage(boost::logic::indeterminate);

        AddGameAdoJob(
            db::DbActionPtr(
                new db::ClubStorageGet(dwClientID, pChar->ChaDbNum(), pChar->GetClubDbNum())));	
    }
    else
    {
        //boost::logic::indeterminate
        //DB ���� �������� �ִ���
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

    // Ŭ�� â�� ���� ����
    GLMSG::SNET_CLUB_STORAGE_RESET NetMsgClubStorageReset;
    SENDTOCLIENT(ClientSlot, &NetMsgClubStorageReset);

    // â���� �⺻ ���� ����
    GLMSG::SNET_CLUB_STORAGE_UPDATE_MONEY NetMsgMoney(pClub->GetStorageMoney(), pClub->GetIncomeMoney());
    SENDTOCLIENT(ClientSlot, &NetMsgMoney);

    // �������� �ϳ��� ����
    GLMSG::SNET_CLUB_GETSTORAGE_ITEM NetMsgItem;

    // Ŭ���̾�Ʈ�� â�� ����
    // ������ ���鼭 �ϳ��� ����
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

    // Ŭ���� ���� ���� ��û
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
	// Ŭ����Ŀ ��� ���� ��û
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

	// Ŭ��â�� ��� ��û, ����� �����Ѵ�. �� �Լ� ���� �Ŀ� �ٸ����� ���� FALSE �Ѵٸ� �� ResetClubStorageUser �Լ� ȣ������� �Ѵ�.
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

    //	��û�� ������ ��ȿ�� �˻�.
    const SINVENITEM* pClubInvenItem = spClub->GetItem( dwChannel, Data.m_HoldPosX, Data.m_HoldPosY );
    if ( !pClubInvenItem )
	{
		spClub->ResetClubStorageUser();
        return FALSE;
	}

    // Ŭ�� ������ ���
    SINVENITEM InvenBackUp = *pClubInvenItem;
    
    // �������� ����
    if ( false == spClub->DeleteItem( dwChannel, InvenBackUp.wPosX, InvenBackUp.wPosY ) )
    {
		spClub->ResetClubStorageUser();
		return FALSE;
    }

	// Ŭ�� �α�
	// [%1%]���� [%2%]X%3% ��(��) �����Ͽ����ϴ�.
	std::string ClubLog( 
		sc::string::format( 
		ID2GAMEINTEXT("CLUB_LOG_20"), 
		pChar->GetChaName(), 
		pClubInvenItem->sItemCustom.GETNAME(), 
		pClubInvenItem->TurnNum() ) );
	SENDCLUBLOG( spClub->DbNum(), club::LOG_CLUB_STORAGE, ClubLog );

    // Ŭ��â�� �־��� �������� �տ� ��. ��û
    GLMSG::NET_CLUB_STORAGE_TO_INVEN_AF SendData( ChaDbNum, spClub->DbNum(), InvenBackUp.sItemCustom, Data.m_PosX, Data.m_PosY );
    msgpack::sbuffer SendBufferAF;
    msgpack::pack( SendBufferAF, SendData );
    SENDTOFIELDSVR( pChar->GetChannel(), pChar->m_dwCurFieldSvr, NET_MSG_CLUB_STORAGE_TO_INVEN_AF, SendBufferAF, true );

    // â�� ������ ���� ��
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
        // [%1%]X%2% ��(��) ���� �Ǿ����ϴ�.
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
    // Note : agent�� field �� ������� �������� ��ȯ�ؾ��ؼ�, ������ ���� ������ ����.
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

    //// �������� ����
    //pClub->DeleteItem(RecvData.m_Channel, InvenBackUp.wPosX, InvenBackUp.wPosY);

    //// â�� ������ ���� ��
    //GLMSG::SNET_CLUB_STORAGE_DELETE NetMsg;
    //NetMsg.dwChannel = RecvData.m_Channel;
    //NetMsg.wPosX     = InvenBackUp.wPosX;
    //NetMsg.wPosY     = InvenBackUp.wPosY;
    //pClub->SendClubClient(&NetMsg);

    //// Ŭ�� �α�
    //// [%1%]���� [%2%]X%3% ��(��) �����Ͽ����ϴ�.
    //std::string ClubLog(
    //    sc::string::format(
    //        ID2GAMEINTEXT("CLUB_LOG_20"),
    //        pChar->GetChaName(),
    //        pItemData->GetNameStr(),
    //        InvenBackUp.sItemCustom.TurnNum()));
    //SENDCLUBLOG(pClub->DbNum(), club::LOG_CLUB_STORAGE, ClubLog);
    //
    //// Ŭ��â�� �־��� �������� �տ� �� �����۰� ��ü ��û
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
	// Ŭ����Ŀ ��� ���� ��û
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

	// ä�α��� Ȯ���ؾ�, �ٸ� ä���ε� ���� ��ǥ��� ���ϴ� ���� ����
    if ( RecvData.m_PosX == RecvData.m_HoldPosX && RecvData.m_PosY == RecvData.m_HoldPosY && RecvData.m_Channel == RecvData.m_HoldChannel)
        return FALSE;

    DWORD ChaDbNum = pChar->ChaDbNum();

    std::tr1::shared_ptr< GLClubAgent > pClub = GetClub( pChar->GetClubDbNum() );
    if ( !pClub )
        return FALSE;

    if ( !pClub->HaveStorageAuthority( pChar->ChaDbNum() ) )
        return FALSE;

	// Ŭ��â�� ��� ��û, ����� �����Ѵ�. �Ʒ� autoreset ��ü�� ����� �� ResetClubStorageUser �Լ� ȣ��ȴ�.
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

	// MsgClubStorageToInvenCA �Լ������� �߰��� �����ϸ� ������ ResetClubStorageUser �� ȣ���ؾ� �ߴµ�, 
	// �� ������ Ŭ��â��->�κ��丮 �� ������ �̵� �޽����� ���� �ʵ弭������ ���ٿ;� �Ǽ��̴�. 
	// MsgClubStorageExClubStorageCA �Լ��� ��� ������Ʈ���� ���� ���� ������ ������ ResetClubStorageUser �� ȣ�������ʰ� AutoResetClubStorageUser ����Ѵ�.
	// MsgClubStorageExClubStorageCA �Լ� �����ϸ� Ŭ��â�� ����� �ڵ� ���µȴ�. �Ҹ��ڿ��� ȣ���
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

	// SITEM Ȯ��
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

	// ������ Ȯ���� �� �߱� ������ �Ʒ� swap �� �� ������ �߻��ϸ� �ȵȴ�. 
	// ����, ������ ������ ��� ����ó���� �߰��غ�������, ������ �ܰ迡�� ó���� ���ܰ� �ʹ� ����. 
	// �׷��� swap �������� �κ��丮 ���, ���� �߻��ϸ� ������ ����� ��������
	// ���翡 ���� ����� �ʹ� ũ��.
	// �׽�Ʈ �Ⱓ���� ���� �α� Ȯ���ϰ� ��Ȳ�� ���� ���� ó�� �߰�����.
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
	// Ŭ����Ŀ ��� ���� ��û
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
        RecvData.m_PosY == RecvData.m_HoldPosY && RecvData.m_Channel == RecvData.m_HoldChannel )	// ä�α��� Ȯ���ؾ�, �ٸ� ä���ε� ���� ��ǥ�� �������� ���� ���ϴ� ���� ����
        return FALSE;

    DWORD ChaDbNum = pChar->ChaDbNum();

    std::tr1::shared_ptr< GLClubAgent > pClub = GetClub( pChar->GetClubDbNum() );
    if ( !pClub )
        return FALSE;

    if ( !pClub->HaveStorageAuthority( pChar->ChaDbNum() ) )
        return FALSE;

	// Ŭ��â�� ��� ��û, ����� �����Ѵ�. �Ʒ� autoreset ��ü�� ����� �� ResetClubStorageUser �Լ� ȣ��ȴ�.
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

	// MsgClubStorageToInvenCA �Լ������� �߰��� �����ϸ� ������ ResetClubStorageUser �� ȣ���ؾ� �ߴµ�, 
	// �� ������ Ŭ��â��->�κ��丮 �� ������ �̵� �޽����� ���� �ʵ弭������ ���ٿ;� �Ǽ��̴�. 
	// MsgClubStorageToClubStorageCA �Լ��� ��� ������Ʈ���� ���� ���� ������ ������ ResetClubStorageUser �� ȣ�������ʰ� AutoResetClubStorageUser ����Ѵ�.
	// MsgClubStorageToClubStorageCA �Լ� �����ϸ� Ŭ��â�� ����� �ڵ� ���µȴ�. �Ҹ��ڿ��� ȣ���
	AutoResetClubStorageUser autoreset( pClub );

    const SINVENITEM* pClubHoldItem = pClub->GetItem( RecvData.m_HoldChannel, RecvData.m_HoldPosX, RecvData.m_HoldPosY );
    if ( !pClubHoldItem )
        return FALSE;

	// �̹� Ŭ����Ŀ�� �� �ִ� �������ε� ������ ���δ�.
	if ( !( pClubHoldItem->sItemCustom.IsExchange() != NULL ) )
        return FALSE;

    SINVENITEM sHoldBackUp = *pClubHoldItem;
    WORD wHoldPosX = sHoldBackUp.wPosX;		//	�տ� �� ������ ��ġ.
    WORD wHoldPosY = sHoldBackUp.wPosY;

    if ( !pClub->DeleteItem( RecvData.m_HoldChannel, RecvData.m_HoldPosX, RecvData.m_HoldPosY ) )
        return E_FAIL;

    // â�� ������ ���� ��
    GLMSG::SNET_CLUB_STORAGE_DELETE MsgAC;
    MsgAC.dwChannel = RecvData.m_HoldChannel;
    MsgAC.wPosX = RecvData.m_HoldPosX;
    MsgAC.wPosY = RecvData.m_HoldPosY;
    pClub->SendClubClient( &MsgAC );

    if ( !pClub->InsertItem( sHoldBackUp.CustomItem(), RecvData.m_Channel, RecvData.m_PosX, RecvData.m_PosY ) )
    {
        // ���� �� ������ ������ �ٽ� �־��ش�.
		if ( !pClub->InsertItem( sHoldBackUp.CustomItem(), RecvData.m_HoldChannel, RecvData.m_HoldPosX, RecvData.m_HoldPosY ) )
		{
			// ������ �־����� ����, �α� ����ϰ� ���� ������ ó���Ѵ�.
			std::string strGuid = sc::string::uuidToString( sHoldBackUp.GetGuid() );
			sc::writeLogError( sc::string::format( "error MsgClubStorageToClubStorageCA, InsertItem fail, clubnum: %1% guid: %2%", pChar->GetClubDbNum(), strGuid.c_str() ) );

			// cause �� enum ���� ������ �Ѵ�. �Լ� �̸� �״�� ����ϵ���
			InsertErrorItem( sHoldBackUp.GetGuid(), 1 );
		}

        return E_FAIL;
    }

	// ��� ������Ʈ
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
        
        // ������ ����/ĳ���;���
        // ĳ���Ͱ� �������� �ʾ� ��Ȯ�� ���� ������ ���� �˼� ����.
        // �ϴ� ���� �õ��� ���ؼ� ��ü ������ �޽����� �����Ѵ�.
		// �� ĳ���� �������� ����������, ��񿡼��� �������� �ʾҴ�. �ʵ忡 ĳ���Ͱ� ��� ������ �� ����.
        GLMSG::NET_HOLD_ITEM_RESTORE_AF MsgAF( ChaDbNum, InsertedItem );
        msgpack::sbuffer SendBuffer;
        msgpack::pack( SendBuffer, MsgAF );
        SENDTOALLCHANNEL( NET_MSG_CLUB_HOLD_TO_STORAGE_AF, SendBuffer, true );
        return FALSE;
    }

    const SITEM* pItemData = GLogicData::GetInstance().GetItem( InsertedItem.GetBoxWrappedItemID() );
    if ( !pItemData )
    {        
        // ������ ����/�������� �ʴ� ������
        GLMSG::NET_HOLD_ITEM_RESTORE_AF MsgAF( ChaDbNum, InsertedItem );
        msgpack::sbuffer SendBuffer;
        msgpack::pack( SendBuffer, MsgAF );
        SENDTOFIELDSVR( pChar->GetChannel(), pChar->GetCurrentFieldServer(), NET_MSG_CLUB_HOLD_TO_STORAGE_AF, SendBuffer, true );
        return FALSE;
    }

    std::tr1::shared_ptr< GLClubAgent > pClub = GetClub( pChar->GetClubDbNum() );
    if ( !pClub )
    {
        // ������ ����/Ŭ������
		GLMSG::NET_HOLD_ITEM_RESTORE_AF MsgAF( ChaDbNum, InsertedItem );
		msgpack::sbuffer SendBuffer;
		msgpack::pack( SendBuffer, MsgAF );
		SENDTOFIELDSVR( pChar->GetChannel(), pChar->GetCurrentFieldServer(), NET_MSG_CLUB_HOLD_TO_STORAGE_AF, SendBuffer, true );
        return FALSE;
    }

    if ( !pClub->HaveStorageAuthority( ChaDbNum ) )
    {        
        // ������ ����/���Ѿ���
		GLMSG::NET_HOLD_ITEM_RESTORE_AF MsgAF( ChaDbNum, InsertedItem );
		msgpack::sbuffer SendBuffer;
		msgpack::pack( SendBuffer, MsgAF );
		SENDTOFIELDSVR( pChar->GetChannel(), pChar->GetCurrentFieldServer(), NET_MSG_CLUB_HOLD_TO_STORAGE_AF, SendBuffer, true );
        return FALSE;
    }

	// Ŭ��â�� ��� ��û, ����� �����Ѵ�. �Ʒ� autoreset ��ü�� ����� �� ResetClubStorageUser �Լ� ȣ��ȴ�.
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

		// ������ ����/������ Ŭ��â�� �����
		GLMSG::NET_HOLD_ITEM_RESTORE_AF MsgAF( ChaDbNum, InsertedItem );
		msgpack::sbuffer SendBuffer;
		msgpack::pack( SendBuffer, MsgAF );
		SENDTOFIELDSVR( pChar->GetChannel(), pChar->GetCurrentFieldServer(), NET_MSG_CLUB_HOLD_TO_STORAGE_AF, SendBuffer, true );
		return FALSE;
	}

	AutoResetClubStorageUser autoreset( pClub );

    // ������ ����
    if ( !pClub->InsertItem( InsertedItem, Data.m_Channel, Data.m_PosX, Data.m_PosY ) )
    {
		// ������ ����/�������� ���� ���� ����
		GLMSG::NET_HOLD_ITEM_RESTORE_AF MsgAF( ChaDbNum, InsertedItem );
		msgpack::sbuffer SendBuffer;
		msgpack::pack( SendBuffer, MsgAF );
		SENDTOFIELDSVR( pChar->GetChannel(), pChar->GetCurrentFieldServer(), NET_MSG_CLUB_HOLD_TO_STORAGE_AF, SendBuffer, true );
		return FALSE;

//         // �����ϸ� ���ϴ� ��ġ�� �ƴ� �ٸ� ���� �ѹ� �� �ֱ⸦ �õ��Ѵ�.
//         if ( !pClub->InsertItem( Data.m_Item ) )
//         {
//             // ������ ����/�������� ���� ���� ����
// 			GLMSG::NET_HOLD_ITEM_RESTORE_AF MsgAF( ChaDbNum, InsertedItem );
// 			msgpack::sbuffer SendBuffer;
// 			msgpack::pack( SendBuffer, MsgAF );
// 			SENDTOFIELDSVR( pChar->GetChannel(), pChar->GetCurrentFieldServer(), NET_MSG_CLUB_HOLD_TO_STORAGE_AF, SendBuffer, true );
//             return FALSE;
//         }
    }

	pClub->ClubStorageUpdateItem( Data.m_Channel, Data.m_PosX, Data.m_PosY, true );

    // ClubLog
    // [%1%]���� Ŭ�� â�� [%2%]X%3% (��)�� �����ϼ̽��ϴ�.  
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
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
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

    // �ݾ� �α�
	// ��� �� �α� ����
    LogMoneyExchange( gamelog::ID_CHAR, ChaDbNum, gamelog::ID_CLUB, ClubDbNum,              -lnMoney, ITEM_ROUTE_CLUB, ( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap(), LOG_MONEY_CLUB_STORAGE_DRAW );    
	// ��� �� �α� ����
    LogMoneyExchange( gamelog::ID_CHAR,        0, gamelog::ID_CLUB, ClubDbNum, spClub->GetStorageMoney(), MONEY_ROUTE_CLUB_TOTAL_MONEY, ( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap(), LOG_MONEY_CLUB_STORAGE_CURRENT );
    
    // Ŭ���̾�Ʈ�� ��� â�� �ִ� �� �׼� ��ȭ�� �˷���.
    GLMSG::SNET_CLUB_STORAGE_UPDATE_MONEY NetMsgStorage( spClub->GetStorageMoney(), spClub->GetIncomeMoney() );
    spClub->SendClubClient( &NetMsgStorage );

    // ĳ���Ϳ��� �ݾ�����
    GLMSG::NET_CLUB_STORAGE_DRAW_MONEY_AF DataAF( ChaDbNum, ClubDbNum, lnMoney );
    msgpack::sbuffer SendBuffer;
    msgpack::pack( SendBuffer, DataAF );
    SENDTOFIELDSVR( 
        pChar->GetChannel(), 
        pChar->GetCurrentFieldServer(), 
        NET_MSG_CLUB_STORAGE_DRAW_MONEY_AF, 
        SendBuffer );

    // Club log/�� ����
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
    
    // Ŭ���̾�Ʈ�� ��� â�� �ִ� �� �׼� ��ȭ�� �˷���.
    GLMSG::SNET_CLUB_STORAGE_UPDATE_MONEY NetMsgStorage( spClub->GetStorageMoney(), spClub->GetIncomeMoney() );
    spClub->SendClubClient( &NetMsgStorage );

    // [%1%]�ݾ��� �����Ǿ����ϴ�. �����ݾ��� [%2%]�Դϴ�.
    std::string ClubLog( 
        sc::string::format( 
            ID2GAMEINTEXT( "CLUB_LOG_18" ), 
            RecvData.m_Money, 
            spClub->GetStorageMoney() ) );
    SENDCLUBLOG( ClubDbNum, club::LOG_CLUB_STORAGE, ClubLog );

	// ��� �� �α� ����
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
		// ĳ���� ���ӸӴϴ� �̹� ���� �����̳�, ĳ���Ͱ� ����. ���߿� �� ĳ���� ������ �ʿ��ϴ�.
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
        
        // ���ӸӴ� ����
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

        // ���ӸӴ� ����
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
        
        // ���ӸӴ� ����
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

		// ���ӸӴ� ����
		GLMSG::NET_CLUB_STORAGE_SAVE_MONEY_AF SendData( ChaDbNum, AddMoney );
		msgpack::sbuffer SendBuffer;
		msgpack::pack( SendBuffer, SendData );
		SENDTOFIELDSVR( pChar->GetChannel(), pChar->GetCurrentFieldServer(), NET_MSG_CLUB_STORAGE_SAVE_MONEY_AF, SendBuffer );
		return FALSE;
	}

	AutoResetClubStorageMoneyUser autoreset( spClub );

    spClub->AddStorageMoney( AddMoney );

    // �ݾ� �α�
	// ��� �� �α� ����
    LogMoneyExchange( gamelog::ID_CHAR, 0, gamelog::ID_CLUB, ClubDbNum, spClub->GetStorageMoney(), MONEY_ROUTE_CLUB_TOTAL_MONEY, ( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap(), LOG_MONEY_CLUB_STORAGE_CURRENT );
    
    // [%1%]���� [%2%]��ŭ â�� �Ա��ϼ̽��ϴ�. �ѱݾ��� [%3%]�Դϴ�.
    std::string ClubLog( 
        sc::string::format( 
            ID2GAMEINTEXT( "CLUB_LOG_15" ), 
            pChar->GetChaName(), 
            AddMoney, 
            spClub->GetStorageMoney() ) );
    SENDCLUBLOG( ClubDbNum, club::LOG_CLUB_STORAGE, ClubLog );
    
    // Ŭ���̾�Ʈ�� �� �׼� ��ȭ�� �˷���.
    GLMSG::SNET_CLUB_STORAGE_UPDATE_MONEY NetMsgStorage( spClub->GetStorageMoney(), spClub->GetIncomeMoney() );
    spClub->SendClubClient( &NetMsgStorage );

    return TRUE;
}

//! ���ӸӴ� �ŷ��α�
// ��� �� �α� ����
void GLAgentServer::LogMoneyExchange(
    gamelog::EMIDTYPE FromType, DWORD FromDbNum, // �����
    gamelog::EMIDTYPE ToType, DWORD ToDbNum, // ������
    LONGLONG lnPrice, EMITEM_ROUTE Flag, const MapID &sMapID, EMITEM_ROUTE_DETAIL emFlagDetail)
{    
    m_pItemLimit->LogMoneyExchange(
        FromType, FromDbNum,
        ToType, ToDbNum,
        lnPrice, Flag, sMapID, emFlagDetail);
}

// ��� �� �α� ����
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

    // ������� �ٸ� �� ����.
    size_t size = RecvData.m_vecGaeaId.size();
    if( RecvData.m_vecClubDbNum.size() != RecvData.m_vecGaeaId.size() )
        return FALSE;

    for( unsigned int i=0 ; i<RecvData.m_vecGaeaId.size() ; i++ )
    {
        std::tr1::shared_ptr<GLClubAgent> pClub = GetClub(RecvData.m_vecClubDbNum[i]);
        if (!pClub)
            return FALSE;

        // Ŭ������ ����
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
        return; // Ŭ������

    DWORD ChaDbNum = pChar->ChaDbNum();
    if (!spClub->HaveAuthChangeAuthority(ChaDbNum))
        return; // ���� ����

    if (!spClub->IsMember(MemberDbNum))
        return; // ������ �ٲٷ��� ĳ���Ͱ� club member �� �ƴ�

    if (spClub->IsMaster(MemberDbNum))
        return; // Ŭ�� �������� ������ �ٲ��� ����

    spClub->ChangeMemberGrade(MemberDbNum, GradeIndex);

    DWORD ClientSlot = pChar->ClientSlot();
    
    // ��ü field server �� �˸�
    NET_CLUB_MEMBER_GRADE_CHANGE_AF SendAF;
    SendAF.m_ClubDbNum   = ClubDbNum;
    SendAF.m_MemberDbNum = MemberDbNum;
    SendAF.m_Index       = GradeIndex;
    
    msgpack::sbuffer SendBufferAF;
    msgpack::pack(SendBufferAF, SendAF);
    
    SENDTOALLCHANNEL(NET_MSG_CLUB_MEMBER_GRADE_CHANGE_AF, SendBufferAF);
    
    // ��ü club member ���� �˸�
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
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
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

	// CD �������� ����
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
	
	// DB�� ����
	AddGameDbJob(
        db::DbActionPtr(
            new db::ClubDeputySet(ClubDbNum, NewCdCertifier)));

	// Ŭ�� ���� CD ���� ������ ���� �α�        
	// ��� �� �α� ����
	LogCharAction(
		MasterDbNum,		//	�����.
		gamelog::ACT_CLUB_DEPUTY_MEMBER,	//	����.
		gamelog::ID_CHAR, NewCdCertifier,	//	����.
		0,
		0,
		0,
		0, 
		( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap());

    // Club member �� ���� ���� �˸�
    GLMSG::NET_CLUB_CD_CERTIFIER_CHANGE_AC MsgAC(NewCdCertifier);
    SENDTOCLUBCLIENT(ClubDbNum, &MsgAC);    

    // Field �� �˸�
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
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
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

    // Ŭ�� ���� �α� ���.    
	// ��� �� �α� ����
    LogCharAction(
        spClub->MasterCharDbNum(),		//	�����.
        gamelog::ACT_CLUB_JOIN_MEMBER,	//	����.
        gamelog::ID_CHAR, TarChaDbNum,	//	����.
        0,							//	exp
        0,							//	bright
        0,							//	life
        0,							//	money
		( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap()
    );

    // Ŭ���� ���Խ�Ŵ.
    spClub->AddMember(TarChaDbNum, TargetChaName.c_str(), 0, TarChaLevel, LastLoginTime, true, ChaClass);
    //pTarChar->m_ClubDbNum = spClub->m_DbNum;

    // DB�� Ŭ�� ��� �߰�.
    AddGameAdoJob(
        db::DbActionPtr(
            new db::ClubMemberAdd(spClub->m_DbNum, TarChaDbNum)));	

    if (!FromChaName.empty())
    {
        // Club log/%1%���� %2%���� Ŭ���� ���Խ��׽��ϴ�.
        std::string ClubLog(
            sc::string::format(
                ID2GAMEINTEXT("CLUB_LOG_2"),
                FromChaName,
                TargetChaName));
        SENDCLUBLOG(ClubDbNum, club::LOG_MEMBER, ClubLog);
    }
    else
    {
        
        // Club log/%1%���� Ŭ���� �����߽��ϴ�.
        std::string ClubLog(
            sc::string::format(
                ID2GAMEINTEXT("CLUB_LOG_7"),
                TargetChaName));
        SENDCLUBLOG(ClubDbNum, club::LOG_MEMBER, ClubLog);
    }

    // ���� ���� FB.
    GLMSG::NET_CLUB_MEMBER_REQ_FB_AC NetMsgFB(bLobby);
    NetMsgFB.SetTarChaName(TargetChaName.c_str());
    NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_OK;
    SENDTOCLIENT(ClientSlot, &NetMsgFB);

    // Field�� ���� ����.
    GLMSG::SNET_CLUB_MEMBER_ADD_2FLD NetMsgAdd(
        spClub->m_DbNum,
        TarChaDbNum,
        TarChaLevel,
        TargetChaName.c_str(),
        ChaClass);
    SENDTOALLCHANNEL(&NetMsgAdd);

    // ���Ե� ĳ���Ϳ��� Ŭ�� ���� ����
    GLCharAG* pTarChar = GetCharByDbNum(TarChaDbNum);
    if (pTarChar)
    {
        pTarChar->SetClubDbNum( spClub->m_DbNum );

        m_pClubMan->SendClubInfo(pTarChar->ClientSlot(), spClub->DbNum());

        // �ű�Ŭ�� ��� ����
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
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError(std::string("Can not find GetCharByDbNum on AutoClubJoin "));
		return false;
	}

	bool bFOUND = spClub->IsMember( pChar->ChaDbNum() );
	if ( bFOUND )
	{
		return true;
	}

	// Ŭ���� ���Խ�Ŵ.
	bool bAddMember = spClub->AddMember( pChar->ChaDbNum(), pChar->GetChaName(), 0, pChar->GetChaLevel(), sc::time::GetCurrentTime(), true, pChar->GetClass() );
	if ( false == bAddMember )
	{
		return false;
	}

	// DB�� Ŭ�� ��� �߰�.
	AddGameAdoJob(
		db::DbActionPtr(
		new db::ClubMemberAdd( spClub->m_DbNum, pChar->ChaDbNum() ) ) );	

	// Club log/%1%���� Ŭ���� �����߽��ϴ�.
	std::string ClubLog(
		sc::string::format(
		ID2GAMEINTEXT( "CLUB_LOG_7" ),
		pChar->GetChaName() ) );
	SENDCLUBLOG( ClubDbNum, club::LOG_MEMBER, ClubLog );

	// Ŭ�� ���� �α� ���.    
	// ��� �� �α� ����
	LogCharAction(
		spClub->MasterCharDbNum(),			// �����.
		gamelog::ACT_CLUB_AUTO_JOIN_MEMBER,	// ����.
		gamelog::ID_CHAR, pChar->ChaDbNum(),// ����.
		0,									// exp
		0,									// bright
		0,									// life
		0,									// money
		( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap()
		);

	// Field�� ���� ����.
	GLMSG::SNET_CLUB_MEMBER_ADD_2FLD NetMsgAdd(
		spClub->m_DbNum,
		pChar->ChaDbNum(),
		pChar->GetChaLevel(),
		pChar->GetChaName(),
		pChar->GetClass() );
	SENDTOALLCHANNEL( &NetMsgAdd );

	pChar->SetClubDbNum( spClub->m_DbNum );

	m_pClubMan->SendClubInfo( ClientSlot, spClub->DbNum() );

	// �ű�Ŭ�� ��� ����
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

	// Ŭ���� ���Խ�Ŵ.
	spClub->AddMember( pChar->ChaDbNum(), pChar->GetChaName(), 0, pChar->GetChaLevel(), sc::time::GetCurrentTime(), true, pChar->GetClass() );

	// db ���� ������ �ʿ����. ���̱׷��̼����� �̹� ĳ���������� Ŭ����ȣ�� �ִ�.

	// Club log/%1%���� Ŭ���� �����߽��ϴ�.
	std::string ClubLog(
		sc::string::format(
		ID2GAMEINTEXT( "CLUB_LOG_7" ),
		pChar->GetChaName() ) );
	SENDCLUBLOG( pChar->m_ClubDbNum, club::LOG_MEMBER, ClubLog );

	// Ŭ�� ���� �α� ���.    
	// ��� �� �α� ����
	LogCharAction(
		spClub->MasterCharDbNum(),			// �����.
		gamelog::ACT_CLUB_AUTO_JOIN_MEMBER,	// ����.
		gamelog::ID_CHAR, pChar->ChaDbNum(),// ����.
		0,									// exp
		0,									// bright
		0,									// life
		0,									// money
		( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap()
		);

	// Field�� ���� ����.
	GLMSG::SNET_CLUB_MEMBER_ADD_2FLD NetMsgAdd(
		spClub->m_DbNum,
		pChar->ChaDbNum(),
		pChar->GetChaLevel(),
		pChar->GetChaName(),
		pChar->GetClass() );
	SENDTOALLCHANNEL( &NetMsgAdd );

	m_pClubMan->SendClubInfo( ClientSlot, spClub->DbNum() );

	// �ű�Ŭ�� ��� ����
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

//! Ŭ�� ���� ����
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

    // Ŭ���� ���� ���� ����.
    CString strTemp(Notice.c_str());
    if (CRanFilter::GetInstance().ChatFilter(strTemp))
        strTemp = CRanFilter::GetInstance().GetProverb();

    // ���� Ŭ�� ������ Ư������ �� ������ ���� ��쿡 SQL�������� ���ɼ��� ���̹Ƿ� �α׸� �����.
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

    // ���� ���� �˸�.
    NetMsgFB.emFB = EMCLUB_NOTICE_FB_OK;
    NetMsgFB.SetNotice(spClub->GetNotice());
    SENDTOCLIENT(ClientSlot, &NetMsgFB);

    // Ŭ���� ������ �ʵ� ������ �ݿ�.
    GLMSG::SNET_CLUB_NOTICE_FLD NetMsgFld(
        ClubDbNum,
        spClub->GetNoticeDate(),
        spClub->GetNotice(),
        spClub->GetNoticeChaName());    
    SENDTOALLCHANNEL(&NetMsgFld);

    // Club log/����
    std::string ClubLog(
        sc::string::format(
            ID2GAMEINTEXT("CLUB_LOG_NOTICE"),
            ChaName));
    SENDCLUBLOG(ClubDbNum, club::LOG_CLUB_NOTICE, ClubLog);
    return true;
}

//! Ŭ�� ��� �̸� ����
void GLAgentServer::ClubGradeNameChange(GLCharAG* pChar, size_t GradeIndex, const std::string& GradeName)
{
    if (!pChar)
        return;
    
    std::tr1::shared_ptr<GLClubAgent> spClub = GetClub(pChar->GetClubDbNum());
    if (!spClub)
        return;

	if( GradeName == "" && GradeName.c_str() == NULL && GradeName.empty() == true )
		return;

    // ���� ���� ����
    club::AUTH_GRADE OldGradeInfo = spClub->GetAuth(GradeIndex);
    if (!spClub->ChangeGradeName(GradeIndex, GradeName, pChar->ChaDbNum()))
    {
        // ��û�ڿ��� ��� ����/����        
        NET_CLUB_GRADE_NAME_CHANGE_RESULT_AC SendData;
        SendData.m_Result = false;
        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, SendData);
        SENDTOCLIENT(pChar->ClientSlot(), NET_MSG_CLUB_GRADE_NAME_CHANGE_RESULT_AC, SendBuffer);
    }
    else
    {        
        // ��û�ڿ��� ��� ����/���� ---------------------------------------
        NET_CLUB_GRADE_NAME_CHANGE_RESULT_AC SendData1;
        SendData1.m_Result = true;
        msgpack::sbuffer SendBuffer1;
        msgpack::pack(SendBuffer1, SendData1);
        SENDTOCLIENT(pChar->ClientSlot(), NET_MSG_CLUB_GRADE_NAME_CHANGE_RESULT_AC, SendBuffer1);

        // ��ü Ŭ�����鿡�� ��� ���� ------------------------------------
        NET_CLUB_GRADE_NAME_CHANGE_AC SendData2;
        SendData2.m_Index     = GradeIndex;
        SendData2.m_GradeName = GradeName;
        msgpack::sbuffer SendBuffer2;
        msgpack::pack(SendBuffer2, SendData2);        
        SENDTOCLUBCLIENT(pChar->GetClubDbNum(), NET_MSG_CLUB_GRADE_NAME_CHANGE_AC, SendBuffer2);

        // ��ü �ʵ弭���� ���� �˸� ---------------------------------------
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

        // Log �����
        // %1% ���� %2% ��� �̸��� %3% ���� �����Ͽ����ϴ�.
        std::string LogMsg(
            sc::string::format(
                ID2GAMEINTEXT("CLUB_LOG_9"),
                strChrName.c_str(),
                OldGradeInfo.m_Desc,
                GradeName));
        SENDCLUBLOG(pChar->GetClubDbNum(), club::LOG_CLUB_AUTH_CHANGE, LogMsg);
    }
}

//! Ŭ�� ���� ���� Client->Agent
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

//! Ŭ�� ���Ի� �Ұ���
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
	
	// Lobby ���� ��û�Ѵٸ� Char �� �������� �ʴ� �����̴�.
    GLCharAG* pChar = GetCharByDbNum(ChaDbNum);	
    if (pChar)
    {
        ClientSlot = pChar->ClientSlot();
        ChaDbNum = pChar->ChaDbNum();
    }

	if (pChar && pChar->m_ClubDbNum != CLUB_NULL)
	{
        // �̹� ĳ������ Ŭ���� ����
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

	// ��û�� Ŭ�� �̸��� �̹� �����
	DWORD dwOLDGLUB = m_pClubMan->FindClub(ClubName);
	if (dwOLDGLUB != CLUB_NULL)
	{
        GLMSG::NET_CLUB_NEW_FB_FC NetMsgFB(bLobby);
        NetMsgFB.SetClubName(ClubName);
		NetMsgFB.emFB = EMCLUB_NEW_FB_NAME;
		SENDTOCLIENT(ClientSlot, &NetMsgFB);
		return;
	}

	// DB�� ������ ����.	
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
		// Ŭ�� �����Ͱ� �������� ���� ���� Ŭ���� �����Ѵ�.		
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
        // ��Ƽ�� ��� Ŭ�������� ����
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

	// Ŭ�� ����.
	std::tr1::shared_ptr<GLClubAgent> spClubNew = std::tr1::shared_ptr<GLClubAgent> (new GLClubAgent(this));

	spClubNew->m_DbNum = NewClubDbNum;
	spClubNew->SetName(ClubName);
	
	CTime cTIME_CUR = CTime::GetCurrentTime();
	spClubNew->m_tOrganize = cTIME_CUR.GetTime();

	spClubNew->SetMasterCharDbNum(MasterDbNum);
	spClubNew->SetMasterName(MasterName);
    spClubNew->SetMasterCharLevel( pMasterChar->GetChaLevel() );
    // ���� ����
    spClubNew->SetCountry(pPacket->m_Country);

	// �ɸ��� ��ü�� Ŭ�� ID ����.
    if (pMasterChar)
    {
	    //pChar->m_dwGuild = cClubNew.m_dwID;
        pMasterChar->m_ClubDbNum = spClubNew->m_DbNum;
    }

	CTime cTIME_DIS(1970,2,1,1,1,1);
	spClubNew->m_tDissolution = cTIME_DIS.GetTime();

    // Master�� ����� ���
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

	// ����� ���
    if (pParty)
	{
		GLPartyAgent::MEMBER_CITER pos = pParty->m_Member.begin();
		GLPartyAgent::MEMBER_CITER end = pParty->m_Member.end();
		for ( ; pos!=end; ++pos)
		{
			GLCharAG* pPartyMember = GetChar((*pos));
			if (!pPartyMember || pPartyMember->m_ClubDbNum != CLUB_NULL)
                continue;

			// �ɸ��� ��ü�� Ŭ�� ID ����.
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

			// DB�� ��� ����.			
			AddGameAdoJob(
                db::DbActionPtr(
                    new db::ClubMemberAdd(
                        spClubNew->DbNum(),
                        NewMember.m_ChaDbNum)));

            // Club log/%1%���� Ŭ���� �����ϼ̽��ϴ�.
            std::string ClubLog(
                sc::string::format(
                    ID2GAMEINTEXT("CLUB_LOG_1"),
                    pPartyMember->GetChaName()));
            SENDCLUBLOG(spClubNew->DbNum(), club::LOG_MEMBER, ClubLog);
		}
	}    

    // ���� �� ������ Ŭ���̱� ������ ���� ������ ����.
    // �⺻������ �����Ѵ�.
    size_t GradeSize = GLogicData::GetInstance().GetClubGradeSize();
    for (size_t i=0; i<GradeSize; ++i)
        spClubNew->AddAuth(i, GLogicData::GetInstance().GetClubBaseGrade(i));

	m_pClubMan->AddClub(spClubNew);

	// Ŭ�� ���� ����.
	GLMSG::SNET_CLUB_NEW_2FLD NetMsgFLD;
	NetMsgFLD.dwChaNum = MasterDbNum;
	NetMsgFLD.emFB = EMCLUB_NEW_FB_OK;
	NetMsgFLD.dwCLUBID = spClubNew->m_DbNum;
	NetMsgFLD.SetName(spClubNew->Name());
	SENDTOFIELD(ClientSlot, &NetMsgFLD);

	// ���� ���� ����.
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
				// ��� �� �α� ����
				// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
				sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgClubNewDb2Agt ", sCLUBMEMBER.m_ChaDbNum));
			}

			// Ŭ�� ���� �α� ���.
			// ��� �� �α� ����
			LogCharAction(
				spClubNew->MasterCharDbNum(),		//	�����.
				gamelog::ACT_CLUB_JOIN_MEMBER,	//	����.
				gamelog::ID_CHAR, sCLUBMEMBER.m_ChaDbNum,	//	����.
				0,							//	exp
				0,							//	bright
				0,							//	life
				0,							//	money
				( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap()
			);
		}
	}
	SENDTOALLCHANNEL(&NetMsgInfo);

    // Ŭ�� ���� ��ü field server �� ����
    GradeSize = GLogicData::GetInstance().GetClubGradeSize();
    for (size_t i=0; i<GradeSize; ++i)
    {
        club::AUTH_GRADE AuthGrade = spClubNew->GetAuth(i);
        GLMSG::NET_CLUB_AUTH_AF MsgAF(spClubNew->DbNum(), static_cast<BYTE> (i), AuthGrade);
        SENDTOALLCHANNEL(&MsgAF);
    }
        
    // Ŭ�����鿡�� Ŭ�� ���� ����
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

	//	Memo :	���Ϳ� ���ԵǾ� �ִٸ� ���.
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

		// DB�� Ŭ�� �ػ� �ð� ����.		
		AddGameDbJob(
            db::DbActionPtr(
                new db::ClubDissolutionTimeSet(
                    pCLUB->m_DbNum,
                    pCLUB->m_tDissolution)));

		// ��ü ��� to FIELD
		//
		GLMSG::SNET_CLUB_DISSOLUTION_2FLD NetMsgFld;
		NetMsgFld.dwClubID = pCLUB->m_DbNum;
		CTime tDisTime(1970,2,1,1,1,1);
		NetMsgFld.tDissolution = tDisTime.GetTime();
		SENDTOALLCHANNEL ( &NetMsgFld );

		// ��ü ��� FB
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

		// ��ü ������ ����.
		CTime tDisTime = CTime::GetCurrentTime();
		CTimeSpan tADD(GLCONST_CHAR::dwCLUB_DISSOLUTION_DAY,0,10,0);
		tDisTime += tADD;

		pCLUB->DoDissolution(tDisTime.GetTime());

		// DB�� Ŭ�� �ػ� �ð� ����.		
		AddGameDbJob(
            db::DbActionPtr(
                new db::ClubDissolutionTimeSet(
                    pCLUB->m_DbNum,
                    pCLUB->m_tDissolution)));

		// ��ü FB.
		//
		NetMsgFB.emFB = EMCLUB_DIS_FB_OK;
		NetMsgFB.tDissolution = pCLUB->m_tDissolution;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );

		// ��ü to FIELD
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

	// dwGaeaID �� GAEAID_NULL �̴� Ȯ�� ���ʿ�
	/*GLCharAG* pChar = GetCharByGaeaID(dwGaeaID);
	if (!pChar)
	{
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
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

	// Ŭ���� ������ �ݿ�.
    LONGLONG AddIncomeMoney = LONGLONG(pPacket->m_ProfitMoney * GLCONST_CHAR::fEARNING_RATE + 0.1f);
	pCLUB->AddIncomeMoney(AddIncomeMoney);

	// ���ͷα�
	// ��� �� �α� ����
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

	// Db�� ����.	
	AddGameDbJob(
        db::DbActionPtr(
            new db::ClubIncomeMoneySet(
                ClubDbNum,
                IncomeMoney)));

	// Ŭ�������� �뺸;
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

	// Ŭ���� ������ �ݿ�.
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

		// Db�� ����.		
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
    
    // Client �� �˸�
    msgpack::pack(SendBuffer, SendData);
    SENDTOCLIENT(dwClientID, NET_MSG_CLUB_JOIN_TIME_RESET_AC, SendBuffer);

    // DB ������Ʈ
    AddGameAdoJob(
        db::DbActionPtr(
            new db::ChaClubSecedeTimeReset(pTarChar->ChaDbNum())));

    // Field �� �˸�
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

    // client �˸�
    SendDataAC.m_Result = NET_CLUB_ALLIANCE_TIME_RESET_AC::SUCCESS;
    msgpack::pack(SendBufferAC, SendDataAC);
    SENDTOCLIENT(dwClientID, NET_MSG_CLUB_ALLIANCE_TIME_RESET_AC, SendBufferAC);

    // db ������Ʈ
    AddGameDbJob(
        db::DbActionPtr(
            new db::ClubAllianceDisolveTimeSet(spClub->DbNum(), NewTime)));

    AddGameDbJob(
        db::DbActionPtr(
            new db::ClubAllianceTimeSet(spClub->DbNum(), NewTime)));

    // field �˸�
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

	// ������ Ŭ�� ��Ŀ�� �������� ���̳ʸ��� �Ǿ����� ���� ��Ŷ�ȿ� �����͸� �־ �ѹ��� �޽����� ó���Ͽ���.
	// ������ ������ �ϳ��� �޽����� �����ش�. �޽����� ������ �޴ٰ� ĳ���Ͱ� ���� ���� �ִ�. 
	// �׸���, dbaction�� ó���� ȣ���� �� ĳ���� ������ �� �����̰�, ������ ���� Ŭ���̾�Ʈ ���� üũ�� �߰��Ͽ���.
	// Ŭ�� ��Ŀ �������� ó���� �޾ƿ��� ����� ������ ����ؾ� �Ѵ�.
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

			// ���� xml�� ������ �߰��ؼ� �����ʰ� �켱 �̷��� ó���Ѵ�. GS 3,4 ������ ����̴�.
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

			// ������ ���ŵ� ������ ����
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

			// ���� ����
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

	// �̸� �˻�� �̹� �� �� �����̴�.
	if ( false == m_pClubMan->ClubRename( ClubNum, std::string( pPacket->szClubName ) ) )
	{
		sc::writeLogError( sc::string::format( 
			"MsgClubNameChangeFA ClubRename fail clubnum %1% chanum %2% newclubname %3%", 
			ClubNum, ChaDbNum, pPacket->szClubName ) );
		return FALSE;
	}

	// �ʵ� ������ �˸�.
	GLMSG::SNET_CLUB_RENAME_AF NetMsgFld;
	NetMsgFld.ClubNum = ClubNum;
	NetMsgFld.SetClubName( pPacket->szClubName );
	SENDTOALLCHANNEL( &NetMsgFld );

	// Ŭ���α� ����    
	std::string ClubLog( ID2GAMEINTEXT( "CLUB_LOG_33" ) );
	SENDCLUBLOG( ClubNum, club::LOG_CLUB_RENAME, ClubLog );

	return TRUE;
}
