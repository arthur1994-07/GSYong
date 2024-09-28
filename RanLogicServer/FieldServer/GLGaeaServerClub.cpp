#include "../pch.h"
#include "../../SigmaCore/Util/DateTime.h"
#include "../../SigmaCore/MsgPack/MinMsgPack.h"

#include "../../EngineLib/Common/profile.h"
#include "../../EngineLib/DxTools/DebugSet.h"
#include "../../EngineLib/GUInterface/GameTextControl.h" //추방 메시지 전송을 위하여 추가됨
#include "../../EngineLib/DxTools/RENDERPARAM.h"

#include "../../MfcExLib/RanFilter.h"

#include "../../RanLogic/Msg/ClubMsg.h"

#include "../Database/DbManager.h"
#include "../Database/DBAction/DbActionGameItem.h"
#include "../Database/DBAction/DbActionGameClub.h"
#include "../Club/GLClubManField.h"
#include "../Club/GLClubField.h"
#include "../Guidance/GuidanceField.h"
#include "../Util/GLItemLimit.h"
#include "./GLGaeaServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

std::tr1::shared_ptr<GLClubField> GLGaeaServer::GetClub(DWORD ClubNum)
{
	if (m_pClubMan)
		return m_pClubMan->GetClub(ClubNum);
	else
		return std::tr1::shared_ptr<GLClubField> ((GLClubField*) 0);
}

GLClubManField* GLGaeaServer::GetClubMan()
{ 
    return m_pClubMan;
}

BOOL GLGaeaServer::MsgClubNew(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if (dwGaeaID == GAEAID_NULL)
        return FALSE;

	GLChar* pChar = GetChar(dwGaeaID);
    if (pChar)
    {
        pChar->MsgClubNew(nmg);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL GLGaeaServer::MsgClubRank(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if (dwGaeaID == GAEAID_NULL)
        return FALSE;
    
    GLChar* pChar = GetChar(dwGaeaID);
    if (pChar)
    {        
        pChar->MsgClubRank(nmg);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL GLGaeaServer::MsgClubNew2FLD(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_CLUB_NEW_2FLD *msg = (GLMSG::SNET_CLUB_NEW_2FLD *)nmg;

	//GLChar* pChar = GetChar ( dwGaeaID );
	GLChar* pChar = GetCharByDbNum(msg->dwChaNum);
    if (pChar)
    {        
        pChar->MsgClubNew2FLD(nmg);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

//CF
/*
BOOL GLGaeaServer::MsgClubMemberReq(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;

	GLChar* pChar = GetChar ( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgClubMemberReq( nmg );
    return TRUE;    
}
*/
//CF
/*
BOOL GLGaeaServer::MsgClubMemberAns(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar ( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgClubMemberAns( nmg );
    return TRUE;
}
*/
//CAF
BOOL GLGaeaServer::MsgClubMemberNick(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_CLUB_MEMBER_NICK *msg = static_cast<GLMSG::SNET_CLUB_MEMBER_NICK*> (nmg);    
    GLChar* pChar = GetCharByDbNum(msg->dwChaNum);
    if (pChar)
    {        
	    pChar->MsgClubMemberNick(nmg);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL GLGaeaServer::MsgClubCDCertify(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar ( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgClubCDCertify( nmg );
    return TRUE;    
}
// 
// BOOL GLGaeaServer::MsgReqClubGetStorage(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
// {
//     if ( dwGaeaID == GAEAID_NULL )
//         return FALSE;
// 
//     if ( MsgCharPreProc( dwGaeaID ) )
//     {
//         GLChar* pChar = GetChar ( dwGaeaID );
//         if ( !pChar )
//             return FALSE;
//         pChar->MsgReqClubGetStorage( nmg );
//         return TRUE;
//     }
//     else
//     {
//         return FALSE;
//     }
// }

BOOL GLGaeaServer::MsgReqClubStorageToInven( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( m_dwAgentSlot != dwClientID )
        return FALSE;

    NET_MSG_PACK* pPacket = ( NET_MSG_PACK* ) nmg;
    GLMSG::NET_CLUB_STORAGE_TO_INVEN_AF Data;
    if ( !msgpack::BufferToObject( pPacket->DataBuffer(), pPacket->DataSize(), Data ) )
	{
		// 정상적인 상황이 아니라서 SNET_CLUB_STORAGE_USER_RESET 보내줄 필요는 없다. 
		sc::writeLogError( "error MsgReqClubStorageToInvenmsgpack::BufferToObject" );
        return FALSE;
	}

    // Crc check
    if ( pPacket->Crc() != sc::string::getCrc32( pPacket->DataBuffer(), pPacket->DataSize() ) )
    {
		// 정상적인 상황이 아니라서 SNET_CLUB_STORAGE_USER_RESET 보내줄 필요는 없다. 
        sc::writeLogError( 
            sc::string::format( 
                "MsgReqClubStorageToHold CRC. ClubDbNum %1% ChaDbNum %2% Item %3%/%4% TurnNum %5%", 
                Data.m_ClubDbNum, Data.m_ChaDbNum, Data.m_Item.Mid(), Data.m_Item.Sid(), Data.m_Item.TurnNum() ) );
        return FALSE;
    }

    // Magic num check
    if ( Data.m_MagicNum != GLMSG::NET_CLUB_STORAGE_TO_INVEN_AF::MAGIC_NUM )
    {
		// 정상적인 상황이 아니라서 SNET_CLUB_STORAGE_USER_RESET 보내줄 필요는 없다. 
        sc::writeLogError( 
            sc::string::format( 
                "MsgReqClubStorageToHold MAGIC NUM. ClubDbNum %1% ChaDbNum %2% Item %3%/%4% TurnNum %5%", 
                Data.m_ClubDbNum, Data.m_ChaDbNum, Data.m_Item.Mid(), Data.m_Item.Sid(), Data.m_Item.TurnNum() ) );
        return FALSE;
    }
     
    GLChar* pChar = GetCharByDbNum( Data.m_ChaDbNum );
    if ( pChar )
    {
		// 실패할 경우 안에서 아이템 복구 메시지 보낸다.
        pChar->MsgReqClubStorageToInven( Data.m_ClubDbNum, Data.m_Item, Data.m_PosX, Data.m_PosY );
    }
    else
    {
        // 아이템 복구        
        GLMSG::NET_CLUB_STORAGE_RESTORE_FA MsgFA( Data.m_ChaDbNum, Data.m_ClubDbNum, Data.m_Item );
        msgpack::sbuffer SendBuffer;
        msgpack::pack( SendBuffer, MsgFA );
        SENDTOAGENT( NET_MSG_CLUB_STORAGE_RESTORE_FA, SendBuffer, true );
    }

	GLMSG::SNET_CLUB_STORAGE_USER_RESET Msg;
	Msg.ClubNum = Data.m_ClubDbNum;
	SENDTOAGENT( &Msg );

	return TRUE;
}

BOOL GLGaeaServer::MsgReqClubStorageExInven(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    // Note : agent와 field 간 통신으로 아이템을 교환해야해서, 안정성 저하 문제로 막음.
    return FALSE;

    //// NET_MSG_CLUB_STORAGE_EX_HOLD_AF
    //NET_MSG_PACK* pPacket = (NET_MSG_PACK*) nmg;
    //GLMSG::NET_CLUB_STORAGE_EX_INVEN_AF RecvData;
    //if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
    //    return FALSE;
    //
    //// Crc check
    //if (pPacket->Crc() != sc::string::getCrc32(pPacket->DataBuffer(), pPacket->DataSize()))
    //{
    //    std::string Log(
    //        sc::string::format(
    //            "MsgReqClubStorageExInven CRC ERROR. ClubDbNum %1% ChaDbNum %2% Item %3%/%4%",
    //            RecvData.m_ClubDbNum,
    //            RecvData.m_ChaDbNum,
    //            RecvData.m_Item.Mid(),
    //            RecvData.m_Item.Sid()));
    //    sc::writeLogError(Log);
    //    return FALSE;
    //}

    //// Magic num check
    //if (RecvData.m_MagicNum != GLMSG::NET_CLUB_STORAGE_EX_INVEN_AF::MAGIC_NUM)
    //{
    //    std::string Log(
    //        sc::string::format(
    //            "MsgReqClubStorageExInven MAGIC NUM ERROR. ClubDbNum %1% ChaDbNum %2% Item %3%/%4%",
    //            RecvData.m_ClubDbNum,
    //            RecvData.m_ChaDbNum,
    //            RecvData.m_Item.Mid(),
    //            RecvData.m_Item.Sid()));
    //        sc::writeLogError(Log);
    //    return FALSE;
    //}
    //
    //GLChar* pChar = GetCharByDbNum(RecvData.m_ChaDbNum);
    //if (pChar)
    //{
    //    pChar->MsgReqClubStorageExInven(RecvData.m_ClubDbNum, RecvData.m_Item, RecvData.m_Channel, RecvData.m_ClubStoragePosX, RecvData.m_ClubStoragePosY, RecvData.m_InvenPosX, RecvData.m_InvenPosY);
    //    return TRUE;
    //}
    //else
    //{
    //    // 아이템 복구
    //    GLMSG::NET_CLUB_STORAGE_RESTORE_FA MsgFA(RecvData.m_ChaDbNum, RecvData.m_ClubDbNum, RecvData.m_Item);
    //    msgpack::sbuffer SendBuffer;
    //    msgpack::pack(SendBuffer, MsgFA);
    //    SENDTOAGENT(NET_MSG_CLUB_STORAGE_RESTORE_FA, SendBuffer, true);
    //    return FALSE;
    //}
}

BOOL GLGaeaServer::MsgReqClubInvenToStorage(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
// 	if (m_ServiceProvider == SP_INDONESIA)
// 		return false;

    GLChar* pChar = GetChar(dwGaeaID);
    if (pChar)
    {
        NET_MSG_PACK* pPacket = (NET_MSG_PACK*) nmg;
        GLMSG::NET_CLUB_INVEN_TO_STORAGE_CF Data;
        if (msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), Data))
        {            
            pChar->MsgReqClubInvenToStorage(Data.m_Channel, Data.m_PosX, Data.m_PosY, Data.m_HoldPosX, Data.m_HoldPosY );
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

BOOL GLGaeaServer::MsgReqClubHoldToStorageAF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	// 인벤토리 -> 클럽락커 아이템 이동 처리하다가 실패한 경우 처리 로직
	// 우선 필드에서 아이템을 삭제하고(디비에서는 삭제하지 않음) 에이전트로 메시지가 간다.
	// 에이전트서버에서 NET_CLUB_INVEN_TO_STORAGE_FA 실패했을 때만 여기로 들어온다.
	// 캐릭터가 없으면 그냥 처리하지 않아도 되고, 캐릭터에 다시 아이템을 넣어줄 때 디비 업데이트까지 할 필요는 없다.

	if ( dwClientID != m_dwAgentSlot )
	{
		return FALSE;
	}

    NET_MSG_PACK* pPacket = ( NET_MSG_PACK* ) nmg;
    GLMSG::NET_HOLD_ITEM_RESTORE_AF Data;
    if ( !msgpack::BufferToObject( pPacket->DataBuffer(), pPacket->DataSize(), Data ) )
        return FALSE;

    DWORD ChaDbNum = Data.m_ChaDbNum;
    GLChar* pChar = GetCharByDbNum( ChaDbNum );
    if ( !pChar )
        return FALSE;

    // Check magic num
    if ( Data.m_MagicNum != GLMSG::NET_HOLD_ITEM_RESTORE_AF::MAGIC_NUM )
    {
        sc::writeLogError( 
            sc::string::format( 
                "MsgReqClubHoldToStorageAF MAGIC NUM. ChaDbNum %1% Item %2%/%3% TurnNum %4%", 
                ChaDbNum, Data.m_Item.GetNativeID().Mid(), Data.m_Item.GetNativeID().Sid(), Data.m_Item.TurnNum() ) );
        return FALSE;
    }

    // Check crc
    if ( pPacket->Crc() != sc::string::getCrc32( pPacket->DataBuffer(), pPacket->DataSize() ) )
    {
        sc::writeLogError( 
            sc::string::format( 
                "MsgReqClubHoldToStorageAF CRC. ChaDbNum %1% Item %2%/%3% TurnNum %4%", 
                ChaDbNum, Data.m_Item.GetNativeID().Mid(), Data.m_Item.GetNativeID().Sid(), Data.m_Item.TurnNum() ) );
        return FALSE;
    }
    
	// 인벤토리에 넣어준다. 디비 업데이트는 하지 않는다.
	if ( !pChar->InvenInsertItem( Data.m_Item, true, 1, false ) )
	{
		// 아이템을 넣을 공간이 없다. 난감함. 아이템을 바닥에 떨어뜨린다.
		pChar->DropItem( Data.m_Item, pChar->GetPosition() );
	}

    return TRUE;
}
/*
BOOL GLGaeaServer::MsgReqClubStorageSplit(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar ( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqClubStorageSplit( nmg );
    return TRUE;    
}
*/
BOOL GLGaeaServer::MsgReqClubStorageSaveMoney(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	if (m_ServiceProvider == SP_INDONESIA)
		return false;

    GLChar* pChar = GetChar(dwGaeaID);
    if (pChar)
    {
        pChar->MsgReqClubStorageSaveMoney(nmg);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL GLGaeaServer::MsgReqClubStorageSaveMoneyAF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwClientID != m_dwAgentSlot )
        return FALSE;

    NET_MSG_PACK* pPacket = ( NET_MSG_PACK* ) nmg;
    GLMSG::NET_CLUB_STORAGE_SAVE_MONEY_AF RecvData;
    if ( !msgpack::BufferToObject( pPacket->DataBuffer(), pPacket->DataSize(), RecvData ) )
        return FALSE;

    if ( RecvData.m_MagicNum != GLMSG::NET_CLUB_STORAGE_SAVE_MONEY_AF::MAGIC_NUM )
        return FALSE;

    if ( RecvData.m_Money <= 0 )
        return FALSE;

    GLChar* pChar = GetCharByDbNum( RecvData.m_ChaDbNum );
    if ( pChar )
    {
        pChar->MsgReqClubStorageSaveMoneyAF( RecvData.m_Money );
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL GLGaeaServer::MsgReqClubStorageDrawMoney(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if ( dwClientID != m_dwAgentSlot )
    {
        sc::writeLogError( "MsgReqClubStorageDrawMoney not agent slot." );
        return FALSE;
    }

    if ( dwGaeaID != GAEAID_NULL )
    {
        sc::writeLogError( "MsgReqClubStorageDrawMoney GaeaId not null." );
        return FALSE;
    }

    NET_MSG_PACK* pPacket = ( NET_MSG_PACK* ) nmg;
    GLMSG::NET_CLUB_STORAGE_DRAW_MONEY_AF RecvData;
    if ( !msgpack::BufferToObject( pPacket->DataBuffer(), pPacket->DataSize(), RecvData ) )
        return FALSE;

    if ( RecvData.m_MagicNum != GLMSG::NET_CLUB_STORAGE_DRAW_MONEY_AF::MAGIC_NUM )
        return FALSE;

    GLChar* pChar = GetCharByDbNum( RecvData.m_ChaDbNum );
    if ( pChar )
    {
        pChar->MsgReqClubStorageDrawMoney( RecvData.m_ClubDbNum, RecvData.m_Money );
    }
    else
    {
        // 머니 복구
        GLMSG::NET_CLUB_STORAGE_DRAW_MONEY_FA SendDataFA( RecvData.m_ChaDbNum, RecvData.m_ClubDbNum, RecvData.m_Money );
        msgpack::sbuffer SendBuffer;
        msgpack::pack( SendBuffer, SendDataFA );
        SENDTOAGENT( NET_MSG_CLUB_STORAGE_DRAW_MONEY_FA, SendBuffer );
    }

	GLMSG::SNET_CLUB_STORAGE_MONEY_USER_RESET Msg;
	Msg.ClubNum = RecvData.m_ClubDbNum;
	SENDTOAGENT( &Msg );

	return TRUE;
}
/*
BOOL GLGaeaServer::MsgReqClubInComeReNew(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    // GLMSG::SNET_CLUB_INCOME_RENEW
    if (dwGaeaID == GAEAID_NULL)
        return FALSE;
        
    GLChar* pChar = GetChar(dwGaeaID);
    if (pChar)
    {
        pChar->MsgReqClubInComeReNew(nmg);
        return TRUE;
    }
    else
    {
        return FALSE;
    pChar->MsgReqClubInComeReNew( nmg );
    return TRUE;    
}
*/

BOOL GLGaeaServer::RequestConfrontClubChkMbr(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    GLMSG::SNET_CONFRONTCLB_CHECKMBR_FLD *pNetMsg = (GLMSG::SNET_CONFRONTCLB_CHECKMBR_FLD *)nmg;
    const SNATIVEID &sMapID = pNetMsg->sMapID;

    GLLandMan* pLandMan = GetLand ( sMapID );
    if ( !pLandMan )
		return FALSE;

    std::tr1::shared_ptr<GLClubField> pClubA = m_pClubMan->GetClub ( pNetMsg->dwCLUB_A );
    if (!pClubA)
        return FALSE;

    std::tr1::shared_ptr<GLClubField> pClubB = m_pClubMan->GetClub ( pNetMsg->dwCLUB_B );
    if (!pClubB)
        return FALSE;

    GLChar* pCharA = GetCharByDbNum(pClubA->MasterCharDbNum());
    GLChar* pCharB = GetCharByDbNum(pClubB->MasterCharDbNum());

    if ( !pCharA || !pCharB )
    {
        GLMSG::SNETPC_REQ_CONFRONT_FB NetMsgFB(EMCONFT_GUILD, pNetMsg->dwCLUB_A, EMCONFRONT_FAIL, 0.0f);

        if (pCharA)
		{
			NetMsgFB.dwChaNum = pCharA->CharDbNum();
            SENDTOAGENT(&NetMsgFB);	//pCharA->ClientSlot()
		}
        if (pCharB)
		{
			NetMsgFB.dwChaNum = pCharB->CharDbNum();
            SENDTOAGENT(&NetMsgFB);	//pCharB->ClientSlot()
		}

        return FALSE;
    }

    D3DXVECTOR3 vPosition = ( pCharA->GetPosition() + pCharB->GetPosition() ) * 0.5f;

    GLMSG::SNET_CONFRONTCLB_CHECKMBR_AGT NetMsgCheckMbr( pNetMsg->sMapID, vPosition, pNetMsg->dwCLUB_A, pNetMsg->dwCLUB_B, pNetMsg->sOption );

    bool bSCHOOL_CONFT_LEVEL(true);

    CLUBMEMBERS_ITER pos;
    CLUBMEMBERS_ITER end;
    CLUBMEMBERS& ClubMemberA = pClubA->GetMembers();
    pos = ClubMemberA.begin();
    end = ClubMemberA.end();
    for ( ; pos!=end && NetMsgCheckMbr.dwCLUB_A_NUM < EMMAX_CLUB_NUM; ++pos)
    {
        GLChar* pMember = GetCharByDbNum(pos->first);
        if ( pMember && (pNetMsg->sMapID == pMember->GetCurrentMap().getGaeaMapID()) )
        {
            D3DXVECTOR3 vDistance = pMember->GetPosition() - NetMsgCheckMbr.vPosition;
            float fDistance = D3DXVec3Length(&vDistance);
            if (fDistance < GLCONST_CHAR::fCONFRONT_CLB_DIST)
                NetMsgCheckMbr.dwCLUB_A_MEMBER[NetMsgCheckMbr.dwCLUB_A_NUM++] = pMember->m_CharDbNum;
        }
    }

    CLUBMEMBERS& ClubMemberB = pClubB->GetMembers();
    pos = ClubMemberB.begin();
    end = ClubMemberB.end();
    for ( ; pos!=end && NetMsgCheckMbr.dwCLUB_B_NUM < EMMAX_CLUB_NUM; ++pos)
    {
        GLChar* pMember = GetCharByDbNum(pos->first);
        if ( pMember && (pNetMsg->sMapID == pMember->GetCurrentMap().getGaeaMapID()) )
        {
            D3DXVECTOR3 vDistance = pMember->GetPosition() - NetMsgCheckMbr.vPosition;
            float fDistance = D3DXVec3Length(&vDistance);
            if (fDistance < GLCONST_CHAR::fCONFRONT_CLB_DIST)
            {
                NetMsgCheckMbr.dwCLUB_B_MEMBER[NetMsgCheckMbr.dwCLUB_B_NUM++] = pMember->m_CharDbNum;
            }
        }
    }

    SENDTOAGENT(&NetMsgCheckMbr);
    return TRUE;
}

BOOL GLGaeaServer::RequestConfrontClub(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    GLMSG::SNETPC_CONFRONTCLB_START2_FLD *pNetMsg = (GLMSG::SNETPC_CONFRONTCLB_START2_FLD *)nmg;
    const SNATIVEID &sMapID = pNetMsg->sMapID;

    GLLandMan* pLandMan = GetLand ( sMapID );
    if ( !pLandMan )	return FALSE;

    std::tr1::shared_ptr<GLClubField> pClubA = m_pClubMan->GetClub ( pNetMsg->dwCLUB_A );
    if (!pClubA)
        return FALSE;

    std::tr1::shared_ptr<GLClubField> pClubB = m_pClubMan->GetClub ( pNetMsg->dwCLUB_B );
    if (!pClubB)
        return FALSE;

    GLChar* pCharA = GetCharByDbNum(pClubA->MasterCharDbNum());
    GLChar* pCharB = GetCharByDbNum(pClubB->MasterCharDbNum());

    if ( !pCharA || !pCharB )
    {
        GLMSG::SNETPC_REQ_CONFRONT_FB NetMsgFB(EMCONFT_GUILD, pNetMsg->dwCLUB_A, EMCONFRONT_FAIL, 0.0f);

        if (pCharA)
		{
			NetMsgFB.dwChaNum = pCharA->CharDbNum();
            SENDTOAGENT(&NetMsgFB);	//pCharA->ClientSlot()
		}
        if (pCharB)
		{
			NetMsgFB.dwChaNum = pCharB->CharDbNum();
            SENDTOAGENT(&NetMsgFB);	//pCharB->ClientSlot()
		}
        return FALSE;
    }

    D3DXVECTOR3 vPosition = pNetMsg->vPos;

    //	Note : 첫번째 클럽원들에 '클럽대련' 정보 설정.
    //
    pClubA->m_conftOPT = pNetMsg->sOption;
    pClubA->m_dwconftCLUB = pNetMsg->dwCLUB_B;

    GLMSG::SNETPC_CONFRONTCLB_START2_CLT sNetMsgClt(pNetMsg->dwCLUB_B, vPosition, pNetMsg->sOption);

    sNetMsgClt.dwCLUB_MY_NUM = pNetMsg->dwCLUB_A_NUM;
    memcpy ( sNetMsgClt.dwCONFRONT_MY_MEMBER, pNetMsg->dwCLUB_A_MEMBER, sizeof(DWORD)*EMMAX_CLUB_NUM );
    sNetMsgClt.dwCLUB_TAR_NUM = pNetMsg->dwCLUB_B_NUM;
    memcpy ( sNetMsgClt.dwCONFRONT_TAR_MEMBER, pNetMsg->dwCLUB_B_MEMBER, sizeof(DWORD)*EMMAX_CLUB_NUM );

    StringCchCopy ( sNetMsgClt.szName, sizeof(char)*(CHR_ID_LENGTH+1), pClubB->Name() );

    DWORD i(0);
    DWORD dwMember = 0;
    GLChar* pChar = NULL;
    for ( i=0; i<pNetMsg->dwCLUB_A_NUM; ++i )
    {
        dwMember = pNetMsg->dwCLUB_A_MEMBER[i];
        pChar = GetCharByDbNum ( dwMember );
        if ( !pChar )
        {
            continue;
        }
        else
        {
            pClubA->ADDCONFT_MEMBER ( dwMember );

            pChar->m_sCONFTING.RESET();
            pChar->m_sCONFTING.emTYPE = EMCONFT_GUILD;
            pChar->m_sCONFTING.dwTAR_ID = pClubA->m_dwconftCLUB;
            pChar->m_sCONFTING.vPosition = vPosition;
            pChar->m_sCONFTING.sOption = pNetMsg->sOption;

            //	Note : 클라이언트들에 '클럽대련' 정보 전송.
            //
            SENDTOCLIENT ( pChar->ClientSlot(), &sNetMsgClt );

            //	Note : 적대행위자 삭제.
            //
            pChar->RemoveAllPlayHostiles();

            //	Note : 대련 체력 백업.
            //
            pChar->DoConftStateBackup();

			if ( GLHIDESET::CheckHideSet(EMHIDE_BEING_CLUB) )
			{
				pChar->RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
				pChar->RestoreActStateByInvisible();
			}
        }
    }
    pClubA->SETCONFT_MEMBER ();

    //	Note : 두번쩨 클럽원들에 '클럽대련' 정보 설정.
    //
    pClubB->m_conftOPT = pNetMsg->sOption;
    pClubB->m_dwconftCLUB = pNetMsg->dwCLUB_A;

    sNetMsgClt.dwconftCLUB = pNetMsg->dwCLUB_A;

    sNetMsgClt.dwCLUB_MY_NUM = pNetMsg->dwCLUB_B_NUM;
    memcpy ( sNetMsgClt.dwCONFRONT_MY_MEMBER, pNetMsg->dwCLUB_B_MEMBER, sizeof(DWORD)*EMMAX_CLUB_NUM );
    sNetMsgClt.dwCLUB_TAR_NUM = pNetMsg->dwCLUB_A_NUM;
    memcpy ( sNetMsgClt.dwCONFRONT_TAR_MEMBER, pNetMsg->dwCLUB_A_MEMBER, sizeof(DWORD)*EMMAX_CLUB_NUM );
    sNetMsgClt.vPosition = vPosition;
    sNetMsgClt.sOption = pNetMsg->sOption;

    StringCchCopy ( sNetMsgClt.szName, sizeof(char)*(CHR_ID_LENGTH+1), pClubA->Name() );

    for ( i=0; i<pNetMsg->dwCLUB_B_NUM; ++i )
    {
        dwMember = pNetMsg->dwCLUB_B_MEMBER[i];
        pChar = GetCharByDbNum ( dwMember );
        if ( !pChar )					
        {
            continue;
        }
        else
        {
            pClubB->ADDCONFT_MEMBER ( dwMember );

            pChar->m_sCONFTING.RESET();
            pChar->m_sCONFTING.emTYPE = EMCONFT_GUILD;
            pChar->m_sCONFTING.dwTAR_ID = pClubB->m_dwconftCLUB;
            pChar->m_sCONFTING.vPosition = vPosition;
            pChar->m_sCONFTING.sOption = pNetMsg->sOption;

            SENDTOCLIENT ( pChar->ClientSlot(), &sNetMsgClt );			//	클라이언트들에 '클럽대련' 정보 전송.

            pChar->RemoveAllPlayHostiles();									// 적대행위자 삭제.

            pChar->DoConftStateBackup();								//	대련 체력 백업.
        }
    }
    pClubB->SETCONFT_MEMBER ();

    return TRUE;
}

BOOL GLGaeaServer::RequestConfrontClubEnd(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    GLMSG::SNETPC_CONFRONTCLB_END2_FLD *pNetMsg = (GLMSG::SNETPC_CONFRONTCLB_END2_FLD *)nmg;
    if ( pNetMsg->dwCLUB_A!=CLUB_NULL && pNetMsg->dwCLUB_B!=CLUB_NULL )
    {
        std::tr1::shared_ptr<GLClubField> pClubA = m_pClubMan->GetClub(pNetMsg->dwCLUB_A);
        std::tr1::shared_ptr<GLClubField> pClubB = m_pClubMan->GetClub(pNetMsg->dwCLUB_B);

        if ( pClubA && pClubB )
        {
            float fLIFE_P_A(0), fLIFE_P_B(0);

            if ( pNetMsg->emEND_A==EMCONFRONT_END_CWIN )
            {
                fLIFE_P_A = (float) GLCONST_CHAR::nCONFRONT_CLB_WIN_LP;
            }
            else if ( pNetMsg->emEND_A==EMCONFRONT_END_CLOSS )
            {
                fLIFE_P_A = (float) GLCONST_CHAR::nCONFRONT_CLB_LOSS_LP;
            }

            if ( pNetMsg->emEND_B==EMCONFRONT_END_CWIN )
            {
                fLIFE_P_B = (float) GLCONST_CHAR::nCONFRONT_CLB_WIN_LP;
            }
            else if ( pNetMsg->emEND_B==EMCONFRONT_END_CLOSS )
            {
                fLIFE_P_B = (float) GLCONST_CHAR::nCONFRONT_CLB_LOSS_LP;
            }

            {
                DWORD dwMasterID = pClubA->MasterCharDbNum();
                GLChar* pMASTER = GetCharByDbNum ( dwMasterID );
                if( !pMASTER )
                    return FALSE;

                CLUBCONFT_ITER pos = pClubA->m_setConftSTART.begin();
                CLUBCONFT_ITER end = pClubA->m_setConftSTART.end();
                for ( ; pos!=end; ++pos )
                {
                    GLChar* pCHAR = GetCharByDbNum ( *pos );
                    if ( pCHAR )
                    {
                        pCHAR->ReceiveLivingPoint ( (int)fLIFE_P_A );

                        if ( pCHAR->m_sCONFTING.IsCONFRONTING() )
                        {
                            if ( pCHAR->m_sCONFTING.emTYPE==EMCONFT_GUILD )
                            {
                                // 대련 종료 처리
                                pCHAR->EndConfting();                               

                                // 대련종료 후 무적시간 설정
                                if( pMASTER->GetCurrentMap() == pCHAR->GetCurrentMap() )
                                {
                                    pCHAR->m_sCONFTING.SETPOWERFULTIME ( 10.0f ); 
                                }
                            }
                        }
                    }
                }
            }

            {
                DWORD dwMasterID = pClubB->MasterCharDbNum();
                GLChar* pMASTER = GetCharByDbNum ( dwMasterID );
                if( !pMASTER )
                    return FALSE;

                CLUBCONFT_ITER pos = pClubB->m_setConftSTART.begin();
                CLUBCONFT_ITER end = pClubB->m_setConftSTART.end();
                for ( ; pos!=end; ++pos )
                {
                    GLChar* pCHAR = GetCharByDbNum ( *pos );
                    if ( pCHAR )
                    {
                        pCHAR->ReceiveLivingPoint ( (int)fLIFE_P_B );

                        if ( pCHAR->m_sCONFTING.IsCONFRONTING() )
                        {
                            if ( pCHAR->m_sCONFTING.emTYPE==EMCONFT_GUILD )
                            {
                                //  대련 종료 처리
                                pCHAR->EndConfting();								

                                // 대련종료 후 무적시간 설정
                                if( pMASTER->GetCurrentMap() == pCHAR->GetCurrentMap() )
                                {
                                    pCHAR->m_sCONFTING.SETPOWERFULTIME ( 10.0f ); 
                                }
                            }
                        }
                    }
                }
            }
        }

        if ( pClubA )
        {
            //	Note : 클럽 대련 종료를 클라이언트들에 알림.
            //
            GLMSG::SNETPC_CONFRONTPTY_END2_CLT NetMsgClt(pNetMsg->emEND_A);

            if ( pNetMsg->emEND_A==EMCONFRONT_END_CWIN || pNetMsg->emEND_A==EMCONFRONT_END_CLOSS ||
                pNetMsg->emEND_A==EMCONFRONT_END_CTIME )
            {
                GLMSG::SNETPC_CONFRONT_END2_CLT_BRD NetMsgBrd(pNetMsg->emEND_A);

                DWORD dwMasterID = pClubA->MasterCharDbNum();
                GLChar* pMASTER = GetCharByDbNum ( dwMasterID );
                if( !pMASTER )
                    return FALSE;

                CLUBMEMBERS ClubMembers = pClubA->GetMembers();
                CLUBMEMBERS_ITER pos = ClubMembers.begin();
                CLUBMEMBERS_ITER end = ClubMembers.end();
                for ( ; pos!=end; ++pos)
                {
                    GLChar* pCHAR = GetCharByDbNum(pos->first);
                    if (pCHAR && pMASTER->GetCurrentMap() == pCHAR->GetCurrentMap())
                    {
                        SENDTOCLIENT(pCHAR->ClientSlot(), &NetMsgClt);
                        NetMsgBrd.dwGaeaID = pCHAR->GetGaeaID();
                        pCHAR->SendMsgViewAround(&NetMsgBrd);
                    }
                }
            }
            pClubA->CLEARCONFT();
        }

        if ( pClubB )
        {
            //	Note : 클럽 대련 종료를 클라이언트들에 알림.
            //
            GLMSG::SNETPC_CONFRONTPTY_END2_CLT NetMsgClt(pNetMsg->emEND_B);

            if ( pNetMsg->emEND_B==EMCONFRONT_END_CWIN || pNetMsg->emEND_B==EMCONFRONT_END_CLOSS ||
                pNetMsg->emEND_B==EMCONFRONT_END_CTIME )
            {
                GLMSG::SNETPC_CONFRONT_END2_CLT_BRD NetMsgBrd(pNetMsg->emEND_B);

                DWORD dwMasterID = pClubB->MasterCharDbNum();
                GLChar* pMASTER = GetCharByDbNum ( dwMasterID );
                if( !pMASTER )
                    return FALSE;

                CLUBMEMBERS& ClubMembersB = pClubB->GetMembers();
                CLUBMEMBERS_ITER pos = ClubMembersB.begin();
                CLUBMEMBERS_ITER end = ClubMembersB.end();
                for ( ; pos!=end; ++pos)
                {
                    GLChar* pCHAR = GetCharByDbNum(pos->first);
                    if (pCHAR && pMASTER->GetCurrentMap() == pCHAR->GetCurrentMap())
                    {
                        SENDTOCLIENT(pCHAR->ClientSlot(), &NetMsgClt);
                        NetMsgBrd.dwGaeaID = pCHAR->GetGaeaID();
                        pCHAR->SendMsgViewAround(&NetMsgBrd);
                    }
                }
            }
            pClubB->CLEARCONFT();
        }
    }

    return TRUE;
}

BOOL GLGaeaServer::ServerClub_GuidBattleStart(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_CLUB_GUID_BATTLE_START_FLD *pNetMsg = (GLMSG::SNET_CLUB_GUID_BATTLE_START_FLD *)nmg;
	m_pGuidance->BeginBattle ( pNetMsg->dwID );

	return TRUE;
}

BOOL GLGaeaServer::ServerClub_GuidBattleEnd(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_CLUB_GUID_BATTLE_END_FLD *pNetMsg = (GLMSG::SNET_CLUB_GUID_BATTLE_END_FLD *)nmg;
	m_pGuidance->EndBattle(pNetMsg->dwID);

	return TRUE;
}

BOOL GLGaeaServer::ServerClubCertify(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::NET_CLUB_CERTIFIED_AF *pNetMsg = (GLMSG::NET_CLUB_CERTIFIED_AF *)nmg;
	m_pGuidance->ChangeGuidClub ( pNetMsg->m_GuidanceId, pNetMsg->m_ClubDbNum );

	return TRUE;
}

BOOL GLGaeaServer::ServerClubGuidCommission(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::NET_CLUB_GUID_COMMISSION_FLD* pNetMsg = (GLMSG::NET_CLUB_GUID_COMMISSION_FLD*) nmg;
	m_pGuidance->ChangeCommission( pNetMsg->dwID, pNetMsg->fCommission);
	return TRUE;
}

BOOL GLGaeaServer::ServerClubNotice(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if (dwClientID != m_dwAgentSlot)
        return FALSE;

	GLMSG::SNET_CLUB_NOTICE_FLD* pNetMsg = (GLMSG::SNET_CLUB_NOTICE_FLD*) nmg;
	std::tr1::shared_ptr<GLClubField> pCLUB = m_pClubMan->GetClub(pNetMsg->dwCLUB_ID);
	if (pCLUB)
    {
        // 클럽의 공지를 필드 서버에 반영.
	    pCLUB->SetNotice(pNetMsg->szNotice);
        pCLUB->SetNoticeDate(pNetMsg->m_NoticeDate);
        pCLUB->SetNoticeChaName(pNetMsg->m_NoticeChaName);
	    
        // Client update
	    GLMSG::SNET_CLUB_NOTICE_CLT NetMsgClt(
            pCLUB->GetNoticeDate(),
            pCLUB->GetNotice(),
            pCLUB->GetNoticeChaName());
	    SENDTOCLUBCLIENT(pCLUB->m_DbNum, &NetMsgClt);
	    return TRUE;
    }
    else
    {
        return FALSE;
    }
}
/*
BOOL GLGaeaServer::ServerClubStorageGetDB(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GASSERT(IsField());

	GLMSG::SNET_CLUB_STORAGE_GET_DB *pNetMsg = (GLMSG::SNET_CLUB_STORAGE_GET_DB *)nmg;
	if (!pNetMsg->m_pStream)
        return FALSE;

	GLClubField* pCLUB(NULL);
	GLChar* pCHAR(NULL);

	pCLUB = m_pClubMan->GetClub ( pNetMsg->m_dwClubID );
	if ( !pCLUB )
		goto  _END;

	pCHAR = GetCharByDbNum ( pNetMsg->m_dwCharID );
	if ( !pCHAR )
		goto _END;

	if ( pCHAR->m_ClubDbNum != pCLUB->m_DbNum )
		goto _END;

	if ( pCHAR->m_ClubDbNum != pCLUB->m_DbNum )
		goto _END;
	if ( pCHAR->m_CharDbNum != pCLUB->MasterCharDbNum() )	
        goto _END;

	pCLUB->m_bVALID_STORAGE = TRUE;
	pCLUB->SetStorageMoney(pNetMsg->m_lnStorageMoney);
	pCLUB->SETSTORAGE_BYBUF ( *pNetMsg->m_pStream );

	pCHAR->MsgClubStorage ();

_END:
	SAFE_DELETE(pNetMsg->m_pStream);

	return TRUE;
}
*/
/*
BOOL GLGaeaServer::ServerClubInComeDn(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GASSERT(IsField());

	if ( dwClientID != m_dwAgentSlot )
		return FALSE;

	GLMSG::SNET_CLUB_INCOME_DN* pNetMsg = (GLMSG::SNET_CLUB_INCOME_DN*) nmg;

	GLClubField* pCLUB = m_pClubMan->GetClub(pNetMsg->dwClubID);
	if (!pCLUB)
        return FALSE;

	GLChar* pCHAR = GetCharByDbNum(pCLUB->MasterCharDbNum());
	if (!pCHAR)
        return FALSE;
	if (!pCLUB->m_bVALID_STORAGE)
        return FALSE;

	pCLUB->AddStorageMoney(pNetMsg->lnInComeMoney);

	m_pItemLimit->LogMoneyExchange(gamelog::ID_CLUB, 0, gamelog::ID_CLUB, pNetMsg->dwClubID, pNetMsg->lnInComeMoney, MONEY_ROUTE_CLUB_INCOME);
    m_pItemLimit->LogMoneyExchange(gamelog::ID_CLUB, 0, gamelog::ID_CLUB, pNetMsg->dwClubID, pCLUB->GetStorageMoney(), MONEY_ROUTE_CLUB_TOTAL_MONEY);    

    sc::writeLogInfo(
        sc::string::format(
            "ServerClubInComeDn, ClubID %1% MasterID %2% IncomeMoney %3%",
		    pNetMsg->dwClubID,
            pCLUB->MasterCharDbNum(),
            pNetMsg->lnInComeMoney));

	//	Note : 결과 전송.
	//GLMSG::SNET_CLUB_INCOME_FB NetMsgFB;
	//NetMsgFB.dwClubID = pCLUB->m_dwID;
	//NetMsgFB.lnInComeMoney = pNetMsg->lnInComeMoney;
	//SENDTOAGENT ( &NetMsgFB );

	//	Note : 클럽장에게 통보.
	GLMSG::SNET_CLUB_STORAGE_UPDATE_MONEY NetMsgClt;
	NetMsgClt.lnMoney = pCLUB->GetStorageMoney();
	SENDTOCLIENT(pCHAR->ClientSlot(), &NetMsgClt);
	return TRUE;
}
*/
BOOL GLGaeaServer::ServerClubAllianceAddFld(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_CLUB_ALLIANCE_ADD_FLD *pNetMsg = (GLMSG::SNET_CLUB_ALLIANCE_ADD_FLD *)nmg;
	std::tr1::shared_ptr<GLClubField> pCLUB_CHIEF = m_pClubMan->GetClub ( pNetMsg->dwChiefClubID );
	if ( !pCLUB_CHIEF )
        return FALSE;

	std::tr1::shared_ptr<GLClubField> pCLUB_INDIAN = m_pClubMan->GetClub ( pNetMsg->dwIndianClubID );
	if ( !pCLUB_INDIAN )
        return FALSE;

	//	동맹 결성 및 동맹 클럽 추가.
	pCLUB_CHIEF->m_dwAlliance = pCLUB_CHIEF->m_DbNum;
	pCLUB_INDIAN->m_dwAlliance = pCLUB_CHIEF->m_DbNum;
	
	pCLUB_INDIAN->m_dwAllianceBattleWin = pCLUB_CHIEF->m_dwAllianceBattleWin;
	pCLUB_INDIAN->m_dwAllianceBattleLose = pCLUB_CHIEF->m_dwAllianceBattleLose;
	pCLUB_INDIAN->m_dwAllianceBattleDraw = pCLUB_CHIEF->m_dwAllianceBattleDraw;

	pCLUB_CHIEF->AddAlliance(pCLUB_CHIEF->m_DbNum,  pCLUB_CHIEF->Name(),  pCLUB_CHIEF->GetMasterName(),  pCLUB_CHIEF->GetMarkVersion());
	pCLUB_CHIEF->AddAlliance(pCLUB_INDIAN->m_DbNum, pCLUB_INDIAN->Name(), pCLUB_INDIAN->GetMasterName(), pCLUB_INDIAN->GetMarkVersion());
	//pCLUB_INDIAN->AddAlliance ( pCLUB_CHIEF->m_dwID, pCLUB_CHIEF->m_szName );
	//pCLUB_INDIAN->AddAlliance ( pCLUB_INDIAN->m_dwID, pCLUB_INDIAN->m_szName );

/*
	if (pCLUB_CHIEF->AddAlliance ( pCLUB_CHIEF->m_dwID, pCLUB_CHIEF->m_szName ) == true)
	{
		if (pCLUB_CHIEF->AddAlliance ( pCLUB_INDIAN->m_dwID, pCLUB_INDIAN->m_szName ) == true)
		{
			//	추가되는 클럽에 동맹 설정.
			pCLUB_INDIAN->m_dwAlliance = pCLUB_CHIEF->m_dwID;
			return TRUE;
		} // if (pCLUB_CHIEF->AddAlliance ( pCLUB_INDIAN->m_dwID, pCLUB_INDIAN->m_szName ) == true)
		else
		{
			return FALSE;
		}
	} // if (pCLUB_CHIEF->AddAlliance ( pCLUB_CHIEF->m_dwID, pCLUB_CHIEF->m_szName ) == true)
	else
	{
		return FALSE;
	}
*/
	return TRUE;
}

BOOL GLGaeaServer::ServerClubAllianceDel(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_CLUB_ALLIANCE_DEL_FLD *pNetMsg = (GLMSG::SNET_CLUB_ALLIANCE_DEL_FLD *)nmg;
	std::tr1::shared_ptr<GLClubField> pCLUB_CHIEF = m_pClubMan->GetClub ( pNetMsg->dwAlliance );
	if ( !pCLUB_CHIEF )
        return FALSE;

	std::tr1::shared_ptr<GLClubField> pCLUB_INDIAN = m_pClubMan->GetClub ( pNetMsg->dwDelClubID );
	if ( !pCLUB_INDIAN )
        return FALSE;

	CLUB_ALLIANCE_ITER del = pCLUB_CHIEF->m_setAlliance.find(pNetMsg->dwDelClubID);
	if ( pCLUB_CHIEF->m_setAlliance.end()==del )
        return FALSE;

	//	동맹에서 제외.
	pCLUB_CHIEF->m_setAlliance.erase ( del );

	//	제외되는 클럽 정보 갱신.
	pCLUB_INDIAN->ClearAlliance();

	return TRUE;
}

BOOL GLGaeaServer::ServerClubAllianceDis(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_CLUB_ALLIANCE_DIS_FLD *pNetMsg = (GLMSG::SNET_CLUB_ALLIANCE_DIS_FLD *)nmg;
	std::tr1::shared_ptr<GLClubField> pCLUB_CHIEF = m_pClubMan->GetClub ( pNetMsg->dwChiefClubID );
	if ( !pCLUB_CHIEF )
        return FALSE;

	//	동맹들에게 동맹 해체 알림.
	CLUB_ALLIANCE_ITER pos = pCLUB_CHIEF->m_setAlliance.begin();
	CLUB_ALLIANCE_ITER end = pCLUB_CHIEF->m_setAlliance.end();
	for ( ; pos!=end; ++pos )
	{
		const GLCLUBALLIANCE &sALLIANCE = pos->second;
		if ( sALLIANCE.m_dwID==pCLUB_CHIEF->m_DbNum )
            continue;	//	클럽장은 나중에 삭제.

		std::tr1::shared_ptr<GLClubField> pCLUB_IND = m_pClubMan->GetClub ( sALLIANCE.m_dwID );
		if ( !pCLUB_IND )
            continue;

		pCLUB_IND->ClearAlliance();
	}

	//	클럽장의 연합 정보 삭제.
	pCLUB_CHIEF->ClearAlliance();

	return TRUE;
}

BOOL GLGaeaServer::ServerClubBattleBeginFld(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_CLUB_BATTLE_BEGIN_FLD *pNetMsg = (GLMSG::SNET_CLUB_BATTLE_BEGIN_FLD *)nmg;
	std::tr1::shared_ptr<GLClubField> pCLUB_CHIEF = m_pClubMan->GetClub ( pNetMsg->dwChiefClubID );
	if ( !pCLUB_CHIEF )
        return FALSE;

	std::tr1::shared_ptr<GLClubField> pCLUB_INDIAN = m_pClubMan->GetClub ( pNetMsg->dwIndianClubID );
	if ( !pCLUB_INDIAN )
        return FALSE;

	GLCLUBBATTLE sClubBattle;
	
	sClubBattle.m_tStartTime = pNetMsg->tStartTime;
	sClubBattle.m_tEndTime = pNetMsg->tEndTime;
	sClubBattle.m_bAlliance = pNetMsg->bAlliance;

	if( pNetMsg->bAlliance )
	{
		sClubBattle.m_dwCLUBID = pNetMsg->dwIndianClubID;
		sClubBattle.SetName(pCLUB_INDIAN->Name());

		CLUB_ALLIANCE_ITER pos = pCLUB_CHIEF->m_setAlliance.begin();
		CLUB_ALLIANCE_ITER end = pCLUB_CHIEF->m_setAlliance.end();
		for ( ; pos!=end; ++pos )
		{
			const GLCLUBALLIANCE &sALLIANCE = pos->second;

			std::tr1::shared_ptr<GLClubField> pCLUB = m_pClubMan->GetClub ( sALLIANCE.m_dwID );
			if ( !pCLUB )
                continue;

			pCLUB->ADDBATTLECLUB( sClubBattle );
		}

		
		sClubBattle.m_dwCLUBID = pNetMsg->dwChiefClubID;
		sClubBattle.SetName(pCLUB_CHIEF->Name());

		pos = pCLUB_INDIAN->m_setAlliance.begin();
		end = pCLUB_INDIAN->m_setAlliance.end();
		for ( ; pos!=end; ++pos )
		{
			const GLCLUBALLIANCE &sALLIANCE = pos->second;

			std::tr1::shared_ptr<GLClubField> pCLUB = m_pClubMan->GetClub ( sALLIANCE.m_dwID );
			if ( !pCLUB )
                continue;

			pCLUB->ADDBATTLECLUB( sClubBattle );
		}

	}
	else
	{
		sClubBattle.m_dwCLUBID = pNetMsg->dwIndianClubID;
		sClubBattle.SetName(pCLUB_INDIAN->Name());
		pCLUB_CHIEF->ADDBATTLECLUB( sClubBattle );

		sClubBattle.m_dwCLUBID = pNetMsg->dwChiefClubID;
		sClubBattle.SetName(pCLUB_CHIEF->Name());
		pCLUB_INDIAN->ADDBATTLECLUB( sClubBattle );
	}	

	return TRUE;
}

BOOL GLGaeaServer::ServerClubBattleOverFld(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_CLUB_BATTLE_OVER_FLD *pNetMsg = (GLMSG::SNET_CLUB_BATTLE_OVER_FLD *)nmg;
	std::tr1::shared_ptr<GLClubField> pCLUB_CHIEF = m_pClubMan->GetClub ( pNetMsg->dwWinClubID );
	if ( !pCLUB_CHIEF )
        return FALSE;

	std::tr1::shared_ptr<GLClubField> pCLUB_INDIAN = m_pClubMan->GetClub ( pNetMsg->dwLoseClubID );
	if ( !pCLUB_INDIAN )
        return FALSE;

	GLCLUBBATTLE* pClubBattle = pCLUB_CHIEF->GetClubBattle( pNetMsg->dwLoseClubID );
	if ( !pClubBattle ) return FALSE;

	if ( pClubBattle->m_bAlliance ) 
	{

		// 클럽 배틀 관련 적대 관계 정리
		DelPlayHostileAllianceBattle( pNetMsg->dwWinClubID, pNetMsg->dwLoseClubID );

		// 자신 클럽
		CLUB_ALLIANCE_ITER pos = pCLUB_CHIEF->m_setAlliance.begin();
		CLUB_ALLIANCE_ITER end = pCLUB_CHIEF->m_setAlliance.end();
		for ( ; pos!=end; ++pos )
		{
			const GLCLUBALLIANCE &sALLIANCE = pos->second;
			std::tr1::shared_ptr<GLClubField> pCLUB = m_pClubMan->GetClub ( sALLIANCE.m_dwID );
			if ( !pCLUB )
                continue;		
		
			switch( pNetMsg->emFB )
			{
				case EMCLUB_BATTLE_OVER_DRAW:	pCLUB->m_dwAllianceBattleDraw++;			break;
				case EMCLUB_BATTLE_OVER_WIN:	pCLUB->m_dwAllianceBattleWin++;		break;
				case EMCLUB_BATTLE_OVER_LOSE:										break;
				case EMCLUB_BATTLE_OVER_ARMISTICE:	pCLUB->m_dwAllianceBattleDraw++;	break;
				case EMCLUB_BATTLE_OVER_SUBMISSION:	pCLUB->m_dwAllianceBattleWin++;	break;
				case EMCLUB_BATTLE_OVER_TARSUBMISSION:								break;
			}

			pCLUB->DELBATTLECLUB( pNetMsg->dwLoseClubID );
		}


		// 자신 클럽
		pos = pCLUB_INDIAN->m_setAlliance.begin();
		end = pCLUB_INDIAN->m_setAlliance.end();
		for ( ; pos!=end; ++pos )
		{
			const GLCLUBALLIANCE &sALLIANCE = pos->second;
			std::tr1::shared_ptr<GLClubField> pCLUB = m_pClubMan->GetClub ( sALLIANCE.m_dwID );
			if ( !pCLUB )
                continue;		
		
			switch( pNetMsg->emFB )
			{
				case EMCLUB_BATTLE_OVER_DRAW:	pCLUB->m_dwAllianceBattleDraw++;			break;
				case EMCLUB_BATTLE_OVER_WIN:	pCLUB->m_dwAllianceBattleLose++;		break;
				case EMCLUB_BATTLE_OVER_LOSE:										break;
				case EMCLUB_BATTLE_OVER_ARMISTICE:	pCLUB->m_dwAllianceBattleDraw++;	break;
				case EMCLUB_BATTLE_OVER_SUBMISSION:	pCLUB->m_dwAllianceBattleLose++;	break;
				case EMCLUB_BATTLE_OVER_TARSUBMISSION:								break;
			}

			pCLUB->DELBATTLECLUB( pNetMsg->dwWinClubID );
		}


	}
	else
	{
		switch( pNetMsg->emFB )
		{
			case EMCLUB_BATTLE_OVER_DRAW:
				{
					pCLUB_CHIEF->m_dwBattleDraw++;
					pCLUB_INDIAN->m_dwBattleDraw++;
				}
				break;
			case EMCLUB_BATTLE_OVER_WIN:
				{
					pCLUB_CHIEF->m_dwBattleWin++;
					pCLUB_INDIAN->m_dwBattleLose++;
				}
				break;
			case EMCLUB_BATTLE_OVER_LOSE:			
				break;
			case EMCLUB_BATTLE_OVER_ARMISTICE:
				{
					pCLUB_CHIEF->m_dwBattleDraw++;
					pCLUB_INDIAN->m_dwBattleDraw++;
				}
				break;
			case EMCLUB_BATTLE_OVER_SUBMISSION:
				{
					pCLUB_CHIEF->m_dwBattleWin++;
					pCLUB_INDIAN->m_dwBattleLose++;
				}
				break;
			case EMCLUB_BATTLE_OVER_TARSUBMISSION:
				break;
		}


		// 클럽 배틀 관련 적대 관계 정리
		DelPlayHostileClubBattle( pNetMsg->dwWinClubID, pNetMsg->dwLoseClubID );

		pCLUB_CHIEF->DELBATTLECLUB( pNetMsg->dwLoseClubID );
		pCLUB_INDIAN->DELBATTLECLUB( pNetMsg->dwWinClubID );	

	}


	return TRUE;

}

BOOL GLGaeaServer::ServerClubBattleKillUpdate( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_CLUB_BATTLE_KILL_UPDATE_FLD* pNetMsg = (GLMSG::SNET_CLUB_BATTLE_KILL_UPDATE_FLD*)nmg;
	std::tr1::shared_ptr<GLClubField> pMyClub = m_pClubMan->GetClub ( pNetMsg->dwClubID );
	if ( !pMyClub )
        return FALSE;

	GLCLUBBATTLE* pBattle = pMyClub->GetClubBattle( pNetMsg->dwBattleClubID );
	if ( !pBattle ) return FALSE;

	pBattle->m_wKillPoint = pNetMsg->wKillPoint;
	pBattle->m_wDeathPoint = pNetMsg->wDeathPoint;

	return TRUE;
}

BOOL GLGaeaServer::ServerClubBattleLastKillUpdate( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_CLUB_BATTLE_LAST_KILL_UPDATE_FLD *pNetMsg = (GLMSG::SNET_CLUB_BATTLE_LAST_KILL_UPDATE_FLD *)nmg;
	std::tr1::shared_ptr<GLClubField> pMyClub = m_pClubMan->GetClub ( pNetMsg->dwClubID_A );
	if (!pMyClub)
        return FALSE;

	std::tr1::shared_ptr<GLClubField> pTarClub = m_pClubMan->GetClub(pNetMsg->dwClubID_B);
	if (!pTarClub)
        return FALSE;

	GLCLUBBATTLE* pBattle = pMyClub->GetClubBattle(pNetMsg->dwClubID_B);
	if (!pBattle)
        return FALSE;

	GLMSG::SNET_CLUB_BATTLE_LAST_KILL_UPDATE_AGT NetMsg;
	NetMsg.dwClubID_A = pNetMsg->dwClubID_A;
	NetMsg.dwClubID_B = pNetMsg->dwClubID_B;
	NetMsg.wKillPoint = pBattle->m_wKillPointTemp;
	NetMsg.wDeathPoint = pBattle->m_wDeathPointTemp;

	SENDTOAGENT(&NetMsg);

	return TRUE;
}


BOOL GLGaeaServer::RequestClubRank2Fld(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    GASSERT(IsField());
    GLMSG::NET_CLUB_RANK_CHANGE_AF* pNetMsg = (GLMSG::NET_CLUB_RANK_CHANGE_AF*) nmg;
    std::tr1::shared_ptr<GLClubField> pCLUB = m_pClubMan->GetClub(pNetMsg->dwClubID);
    if (!pCLUB)
        return FALSE;
    if (pNetMsg->dwRank > (GLCONST_CHAR::dwMAX_CLUBRANK-1))
        return FALSE;

    pCLUB->m_dwRank = pNetMsg->dwRank;

    GLMSG::NET_CLUB_RANK_CHANGE_FC NetMsg;
    NetMsg.dwRank = pCLUB->m_dwRank;

    // 체크, 클라이언트들에게 알림
    GLChar* pReqChar = GetCharByDbNum(pNetMsg->m_ReqChaDbNum);
    if (pReqChar)
    {
        LONGLONG lnPAY = GLogicData::GetInstance().GetClubRankData(NetMsg.dwRank).m_dwPay;

        pReqChar->CheckMoneyUpdate(pReqChar->GetInvenMoney(), lnPAY, FALSE, "Club Rank Pay.");

        if (pReqChar->GetInvenMoney() >= lnPAY)
            pReqChar->SubtractInvenMoneyServer(lnPAY, SUBTRACT_INVEN_MONEY_CLUB_RANK);
        else
            pReqChar->SetInvenMoney(0);

        // 금액 로그
		// 통계 및 로그 수정
		m_pItemLimit->LogMoneyExchange( gamelog::ID_CHAR, pReqChar->m_CharDbNum, gamelog::ID_CLUB, 0, -lnPAY, ITEM_ROUTE_DELETE, (pReqChar == NULL) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pReqChar->GetCurrentMap(), LOG_MONEY_CLUB_RANK_UPGREAD );
		// 통계 및 로그 수정
        m_pItemLimit->LogMoneyExchange( gamelog::ID_CHAR, pReqChar->m_CharDbNum, gamelog::ID_CHAR, 0, pReqChar->GetInvenMoney(), ITEM_ROUTE_CHAR, (pReqChar == NULL) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pReqChar->GetCurrentMap(), LOG_MONEY_CHAR_INVEN_CURRENT );

        // 금액 변화 클라이언트에 알려줌.
        pReqChar->SendInvenMoneyUpdate();

        // 요청한 캐릭터에게 전송
        SENDTOCLIENT(pReqChar->ClientSlot(), &NetMsg);

        //	Note : 클럽 랭크 변화 로그 기록.
		// 통계 및 로그 수정
        LogCharAction(
            pReqChar->m_CharDbNum,		//	당사자.
            gamelog::ACT_CLUB_LEV_UP,		//	행위.
            gamelog::ID_CLUB, pCLUB->m_DbNum,		//	상대방.
            0,							//	exp.
            0,							//	bright.
            0,							//	life.
            (int)-lnPAY,					//	money.
			(pReqChar == NULL) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pReqChar->GetCurrentMap()
        );
    }

    SENDTOCLUBCLIENT(pCLUB->DbNum(), &NetMsg);
    return TRUE;
}

BOOL GLGaeaServer::RequestClubInfo(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    GASSERT(IsField());
    // 클럽 정보    
    GLMSG::NET_NEW_CLUB_INFO_AF* pPacket = (GLMSG::NET_NEW_CLUB_INFO_AF*) nmg;
    std::tr1::shared_ptr<GLClubField> pClub = std::tr1::shared_ptr<GLClubField> (new GLClubField(this));
    pClub->SetCountry(pPacket->m_Country);
    pClub->SetDbNum(pPacket->dwClubID);
    pClub->SetName(pPacket->szClubName);
    pClub->SetMasterCharDbNum(pPacket->dwMasterID);
    pClub->SetMasterName(pPacket->szMasterName);
    pClub->SetMasterCharLevel(pPacket->wMasterLevel);
    pClub->m_dwCDCertifior = pPacket->dwCDCertifior;
    pClub->m_dwRank = pPacket->dwRank;
    pClub->m_dwMarkVER = pPacket->dwMarkVER;
    pClub->m_tOrganize = pPacket->tOrganize;
    pClub->m_tDissolution = pPacket->tDissolution;

    for (DWORD i=0; i<pPacket->dwMemberNum; ++i)
    {
        if (i < EMMAXCLUBMEMBER_NEW)
        {
            GLCLUBMEMBER cMEMBER;
            cMEMBER.m_ChaDbNum = pPacket->aMembers[i];
            cMEMBER.SetName(pPacket->szMemberName[i]);
            pClub->AddMemberEx(cMEMBER.m_ChaDbNum, cMEMBER);
        }
    }

    m_pClubMan->AddClub(pClub);

    //	Note : 클럽 마스터, 멤버원에게 클럽 상세 정보 전송.
    //
    GLMSG::SNET_CLUB_INFO_2CLT NetMsg2Clt;	
    NetMsg2Clt.dwClubID = pClub->m_DbNum;
    NetMsg2Clt.SetClubName(pClub->Name());
    NetMsg2Clt.dwMasterID = pClub->MasterCharDbNum();
    NetMsg2Clt.SetMasterName(pClub->GetMasterName());
    NetMsg2Clt.dwCDCertifior = pClub->m_dwCDCertifior;
    NetMsg2Clt.dwMarkVER = pClub->m_dwMarkVER;
    NetMsg2Clt.dwRank = pClub->m_dwRank;
    NetMsg2Clt.tOrganize = pClub->m_tOrganize;
    NetMsg2Clt.tDissolution = pClub->m_tDissolution;

    //GLMSG::SNET_CLUB_MEMBER_2CLT SendData;
    //for (int i=0; i<EMMAXCLUBMEMBER_NEW; ++i)
    //{
    //    if (pPacket->aMembers[i] == 0)
    //        continue;
    //    const GLCLUBMEMBER* pMember = pClub->GetMember(pPacket->aMembers[i]);
    //    if (pMember)
    //        SendData.ADDMEMBER(*pMember);
    //    //pPacket->aMembers[i], NULL, pPacket->szMemberName[i], 1, sc::time::GetCurrentTime(), true);
    //}

    //msgpack::sbuffer SendBuffer;
    //msgpack::pack(SendBuffer, SendData);

    // 마스터, 멤버 주변 사람들에게 클럽 간략 정보 전송.
    GLMSG::SNET_CLUB_INFO_BRD NetMsgBrd;
    NetMsgBrd.dwClubID = pClub->m_DbNum;
    NetMsgBrd.dwMarkVer = pClub->m_dwMarkVER;

    // 마스터, 멤버에게 설정
    GLChar* pMASTER = GetCharByDbNum(pClub->MasterCharDbNum());
    if (pMASTER)
    {
        // 마스터에게 전송
        pMASTER->m_ClubDbNum = pClub->m_DbNum;
        pMASTER->SetNickName(pClub->Name());
        StringCchCopy(NetMsg2Clt.szNickName, CHAR_SZNAME, pMASTER->m_szNick);
        SENDTOCLIENT(pMASTER->ClientSlot(), &NetMsg2Clt);

        //SENDTOCLIENT(pMASTER->ClientSlot(), NET_MSG_GCTRL_CLUB_MEMBER_2CLT, SendBuffer);

        // 마스터의 주변 사람에게.
        NetMsgBrd.dwGaeaID = pMASTER->GetGaeaID();
        StringCchCopy(NetMsgBrd.szNickName, CHAR_SZNAME, pMASTER->m_szNick);
        pMASTER->SendMsgViewAround(&NetMsgBrd);
    }

    GLChar* pMEMBER = NULL;
    for (DWORD i=0; i<pPacket->dwMemberNum; ++i)
    {
        pMEMBER = GetCharByDbNum(pPacket->aMembers[i]);
        if (pMEMBER)
        {
            // 멤버에게
            pMEMBER->m_ClubDbNum = pClub->m_DbNum;

            pMEMBER->SetNickName(pClub->Name());
            StringCchCopy(NetMsg2Clt.szNickName, CHAR_SZNAME, pMEMBER->m_szNick);
            SENDTOCLIENT(pMEMBER->ClientSlot(), &NetMsg2Clt);

            //SENDTOCLIENT(pMEMBER->ClientSlot(), NET_MSG_GCTRL_CLUB_MEMBER_2CLT, SendBuffer);

            // 멤버원 주변에게
            NetMsgBrd.dwGaeaID = pMEMBER->GetGaeaID();
            StringCchCopy(NetMsgBrd.szNickName, CHAR_SZNAME, pMEMBER->m_szNick);
            pMEMBER->SendMsgViewAround(&NetMsgBrd);
        }
    }

    return TRUE;
}

BOOL GLGaeaServer::MsgClubAuthAF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    GLMSG::NET_CLUB_AUTH_AF* pPacket = (GLMSG::NET_CLUB_AUTH_AF*) nmg;
    std::tr1::shared_ptr<GLClubField> pClub = GetClub(pPacket->m_ClubDbNum);
    if (!pClub)
        return FALSE;

    // 권한 업데이트
    club::AUTH_GRADE Auth(pPacket->m_AuthName, pPacket->m_AuthFlag);
    pClub->AddAuth(pPacket->m_Index, Auth);

    // 클라이언트에 알림
    GLMSG::NET_CLUB_AUTH_FC MsgAC(pPacket->m_Index, Auth);
    pClub->SendClubClient(&MsgAC);
	return TRUE;
}

// *****************************************************
// Desc: 클럽정보 갱신
// *****************************************************
BOOL GLGaeaServer::RequestClubDissolution2Fld(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GASSERT(IsField());

    GLMSG::SNET_CLUB_DISSOLUTION_2FLD *pNetMsg = (GLMSG::SNET_CLUB_DISSOLUTION_2FLD *)nmg;
    std::tr1::shared_ptr<GLClubField> pCLUB = m_pClubMan->GetClub ( pNetMsg->dwClubID );
    if ( !pCLUB )
        return FALSE;

    pCLUB->DoDissolution ( pNetMsg->tDissolution );

    //	Note : 클럽 마스터, 멤버원에게 클럽 상세 정보 전송.
    //
    GLMSG::SNET_CLUB_INFO_DISSOLUTION NetMsg2Clt;
    NetMsg2Clt.tDissolution = pCLUB->m_tDissolution;

    //	Note : 마스터, 멤버에게 설정
    //
    GLChar* pMASTER = GetCharByDbNum ( pCLUB->MasterCharDbNum() );
    CTime tDisTime( pNetMsg->tDissolution );
    if ( pMASTER )
    {
        // 년도가 1970년이면 클럽 해체 취소.
        if ( tDisTime.GetYear() != 1970 )
        {
            //	Note : 경험치를 감산시킨다.
            __int64 nDECEXP;
            nDECEXP = __int64 ( GLOGICEX::GLDIE_DECEXP(pMASTER->GETLEVEL()) * 0.01f * pMASTER->GET_LEVELUP_EXP() );
            nDECEXP = __int64 ( nDECEXP * pMASTER->GET_PK_DECEXP_RATE()*0.01f );
            pMASTER->SetDeathPenalty_Exp( nDECEXP );

            // 죽었을 경우만 갱신되게 하기 위하여 미리 저장해놓고 다시 저장한다.
            LONGLONG nReExp = pMASTER->m_lnRestorableExp;

            pMASTER->DoDeathPenaltyExp ();

            pMASTER->m_lnRestorableExp = nReExp;
        }

        //	Note : 마스터에게 전송.
        SENDTOCLIENT ( pMASTER->ClientSlot(), &NetMsg2Clt );
    }

    // 멤버에게
    SENDTOCLUBCLIENT(pCLUB->DbNum(), &NetMsg2Clt);
    
    return TRUE;
}

BOOL GLGaeaServer::RequestClubDel2Fld(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GASSERT(IsField());

    GLMSG::SNET_CLUB_DEL_2FLD *pNetMsg = (GLMSG::SNET_CLUB_DEL_2FLD *)nmg;
    std::tr1::shared_ptr<GLClubField> pCLUB = m_pClubMan->GetClub ( pNetMsg->dwClubID );
    if (!pCLUB)
        return FALSE;

    GLChar* pCHAR = GetCharByDbNum ( pCLUB->MasterCharDbNum() );
    if (pCHAR)
    {
        // 클럽 정보 제거
        pCHAR->m_ClubDbNum = CLUB_NULL;
        // 클라이언트에 클럽 정보 리셋
        GLMSG::SNET_CLUB_DEL_2CLT NetMsgClt;
        SENDTOCLIENT(pCHAR->ClientSlot(), &NetMsgClt);

        // 주변 사람에게 알림
        GLMSG::SNET_CLUB_DEL_BRD NetMsgBrd;
        NetMsgBrd.dwGaeaID = pCHAR->GetGaeaID();
        pCHAR->SendMsgViewAround(&NetMsgBrd);
    }

    // 클럽 삭제
    m_pClubMan->DelClub(pNetMsg->dwClubID);

    return TRUE;
}

BOOL GLGaeaServer::RequestClubAdd2Fld(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    GLMSG::SNET_CLUB_MEMBER_ADD_2FLD* pPacket = (GLMSG::SNET_CLUB_MEMBER_ADD_2FLD *)nmg;
    std::tr1::shared_ptr<GLClubField> pCLUB = m_pClubMan->GetClub(pPacket->m_ClubDbNum);
    if (!pCLUB)
        return FALSE;

    pCLUB->AddMember(pPacket->m_ChaDbNum, pPacket->m_ChaName, 0, pPacket->m_ChaLevel, sc::time::GetCurrentTime(), true, pPacket->m_ChaClass);

    /*
    GLMSG::SNET_CLUB_MEMBER_2CLT NetMsgMember;
    NetMsgMember.ADDMEMBER(pPacket->dwMember, NULL, pPacket->szMember, 1, sc::time::GetCurrentTime());

    GLChar* pMASTER = GetCharByDbNum(pCLUB->MasterCharDbNum());
    if ( pMASTER )
    {
        //	Note : 클라이언트에 신규 멤버 정보 전송.
        SENDTOCLIENT ( pMASTER->ClientSlot(), &NetMsgMember );
    }

    CLUBMEMBERS& Members = pCLUB->GetMembers();
    CLUBMEMBERS_ITER pos = Members.begin();
    CLUBMEMBERS_ITER end = Members.end();
    GLChar* pMember = NULL;
    for ( ; pos!=end; ++pos)
    {
        const GLCLUBMEMBER& cMEMBER = pos->second;
        if (cMEMBER.m_ChaDbNum == pPacket->dwMember)
            continue; // 자기 자신의 경우 제외

        pMember = GetCharByDbNum(cMEMBER.m_ChaDbNum);
        if (pMember)        
            SENDTOCLIENT(pMember->ClientSlot(), &NetMsgMember); // 클라이언트에 신규 멤버 정보 전송
    }
    */

    // 신규로 참가하게된 멤버 설정
    GLChar* pNEW_MEMBER = GetCharByDbNum(pPacket->m_ChaDbNum);
    if (pNEW_MEMBER)
    {
        pNEW_MEMBER->m_ClubDbNum = pCLUB->m_DbNum;

        // 클럽 정보 전송.
        //pNEW_MEMBER->MsgClubInfo ();

        // 주변 사람에게 알림.
        GLMSG::SNET_CLUB_INFO_BRD NetMsgBrd;
        NetMsgBrd.dwGaeaID = pNEW_MEMBER->GetGaeaID();
        NetMsgBrd.dwClubID = pNEW_MEMBER->m_ClubDbNum;
        NetMsgBrd.dwMarkVer = pCLUB->m_dwMarkVER;
        StringCchCopy( NetMsgBrd.szNickName, CHAR_SZNAME, pNEW_MEMBER->m_szNick );
        pNEW_MEMBER->SendMsgViewAround(&NetMsgBrd);
    }

    return TRUE;
}

BOOL GLGaeaServer::RequestClubDel(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GASSERT(IsField());

    GLMSG::NET_CLUB_MEMBER_DEL_AF* pPacket = (GLMSG::NET_CLUB_MEMBER_DEL_AF*) nmg;
    std::tr1::shared_ptr<GLClubField> spClub = m_pClubMan->GetClub(pPacket->dwClub);
    if (!spClub)
        return FALSE;

    // CDM 권한 있을경우 제거
    spClub->SetMemberCDMFlag( pPacket->dwMember, FALSE );

    // 클럽 멤버에서 삭제
    if(spClub->DelMember(pPacket->dwMember))
	{
		GLChar* pCHAR = GetCharByDbNum ( pPacket->dwMember );
		if ( pCHAR )
		{
			//	클럽 정보 제거.
			pCHAR->m_ClubDbNum = CLUB_NULL;

			// 클럽탈퇴시간 갱신
			pCHAR->m_tSECEDE = pPacket->tSECEDE;

			//	자신에게 클럽 정보 알림.
			GLMSG::SNET_CLUB_DEL_2CLT NetMsgInfo;
			SENDTOCLIENT ( pCHAR->ClientSlot(), &NetMsgInfo );

			//	Note : 주변 사람에게 알림.
			GLMSG::SNET_CLUB_DEL_BRD NetMsgBrd;
			NetMsgBrd.dwGaeaID = pCHAR->GetGaeaID();
			pCHAR->SendMsgViewAround(&NetMsgBrd);
		}
		
		//추방 메시지 전달을 위하여 추가됨
		if( pPacket->bIsKick )
		{
			std::string strSender = ID2SERVERTEXT("CLUB_BANISHMENT_POST_SENDER_NAME");
			std::string strTitle =  ID2SERVERTEXT("CLUB_BANISHMENT_POST_TITLE");
			std::string strContent = ID2SERVERTEXT("CLUB_BANISHMENT_POST_CONTENT");

			SPOSTINFO PostInfo;

			ZeroMemory(&PostInfo, sizeof(SPOSTINFO));
			{		
				PostInfo.iDesign        = ATTENDANCE_REWARD_POST_DESIGN;    // 편지지디자인

				PostInfo.byPostClass    = EMPOST_SYSTEM;
				PostInfo.byPostState    = EMPOST_UNREAD;                    // 편지상태
				PostInfo.byPostType 	= EMPOST_TEXT;                      // 편지타입

				StringCchCopy(PostInfo.Title, sizeof(PostInfo.Title), ID2SERVERTEXT("CLUB_BANISHMENT_POST_TITLE"));
				StringCchCopy(PostInfo.Content, sizeof(PostInfo.Content), ID2SERVERTEXT("CLUB_BANISHMENT_POST_CONTENT"));

				PostInfo.llAttachMoney  = 0;                                // 첨부골드
				PostInfo.llCharge       = 0;                                // 청구골드

				PostInfo.iSender        = ATTENDANCE_REWARD_POST_SENDER;								// 보낸사람
				PostInfo.iRecver        = pPacket->dwMember;                    // 받는사람
				
				StringCchCopy(PostInfo.SenderName, sizeof(PostInfo.SenderName), ID2SERVERTEXT("CLUB_BANISHMENT_POST_SENDER_NAME"));
				StringCchCopy(PostInfo.RecverName, sizeof(PostInfo.RecverName), pPacket->cDelMemberName);
			}

			if (m_pDBMan)
			{
				AddGameAdoJob(
					db::DbActionPtr(
					new CPostSendSystemMail(PostInfo, SITEMCUSTOM())));
			}
			else
			{
				sc::writeLogInfo("CPostSendSystemMail: Could not get DBMan." );
				return FALSE;
			}
		}
	}

    //	Note : 삭제 메시지.
    GLMSG::NET_CLUB_MEMBER_DEL_FC NetMsgClt;
    NetMsgClt.dwClub = pPacket->dwClub;
    NetMsgClt.dwMember = pPacket->dwMember;

    GLChar* pMASTER = GetCharByDbNum ( spClub->MasterCharDbNum() );
    if ( pMASTER )
    {
        //	Note : 클라이언트에 삭제 멤버 전송.
        SENDTOCLIENT ( pMASTER->ClientSlot(), &NetMsgClt );
    }

    m_pClubMan->SendClubClient(spClub->DbNum(), &NetMsgClt, pPacket->dwMember);

    return TRUE;
}

BOOL GLGaeaServer::RequestClubMarkChange(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GASSERT(IsField());

    GLMSG::SNET_CLUB_MARK_CHANGE_2FLD *pNetMsg = (GLMSG::SNET_CLUB_MARK_CHANGE_2FLD *)nmg;
    std::tr1::shared_ptr<GLClubField> pCLUB = m_pClubMan->GetClub ( pNetMsg->dwClub );
    if (!pCLUB)
        return FALSE;

    pCLUB->m_dwMarkVER = pNetMsg->dwMarkVER;
    memcpy(pCLUB->m_aryMark, pNetMsg->aryMark, sizeof(DWORD)*EMCLUB_MARK_SX*EMCLUB_MARK_SY);

    GLMSG::SNET_CLUB_INFO_MARK_BRD NetMsgBrd;
    NetMsgBrd.dwMarkVer = pCLUB->m_dwMarkVER;

    CLUBMEMBERS& Members = pCLUB->GetMembers();
    CLUBMEMBERS_CITER pos = Members.begin();
    CLUBMEMBERS_CITER end = Members.end();
    for ( ; pos!=end; ++pos)
    {
        const GLCLUBMEMBER& cMEMBER = pos->second;
        GLChar* pMember = GetCharByDbNum(cMEMBER.m_ChaDbNum);
        if (pMember)
        {
            NetMsgBrd.dwGaeaID = pMember->GetGaeaID();
            //	Note : 주변 사람에게 알림.
            pMember->SendMsgViewAround(&NetMsgBrd);
        }
    }

    return TRUE;
}
/*
BOOL GLGaeaServer::RequestClubSubMaster(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    GLMSG::SNET_CLUB_SUBMASTER_FLD* pNetMsg = (GLMSG::SNET_CLUB_SUBMASTER_FLD*) nmg;
    std::tr1::shared_ptr<GLClubField> spClub = m_pClubMan->GetClub(pNetMsg->dwClubID);
    if (!spClub)
        return FALSE;

    spClub->SetMemberFlag(pNetMsg->dwCharID, pNetMsg->dwFlags);
    spClub->m_dwCDCertifior = pNetMsg->dwCDCertifior;

    DWORD dwCHARID = pNetMsg->dwCharID;
    GLChar* pSUB = GetCharByDbNum ( dwCHARID );
    if ( pSUB )
    {
        GLMSG::SNETPC_UPDATE_FLAGS_BRD NetMsgBrd;
        NetMsgBrd.dwGaeaID = pSUB->GetGaeaID();
        if ( spClub->IsMemberFlgCDCertify(dwCHARID) )
	NetMsgBrd.dwFLAGS |= SDROP_CHAR::CLUB_CD;
        else
											NetMsgBrd.dwFLAGS = NULL;

        pSUB->SendMsgViewAround(&NetMsgBrd);
    }

    dwCHARID = spClub->MasterCharDbNum();
    GLChar* pMASTER = GetCharByDbNum ( dwCHARID );
    if ( pMASTER )
    {
        GLMSG::SNETPC_UPDATE_FLAGS_BRD NetMsgBrd;
        NetMsgBrd.dwGaeaID = pMASTER->GetGaeaID();
        if ( spClub->IsMemberFlgCDCertify(dwCHARID) )
	NetMsgBrd.dwFLAGS |= SDROP_CHAR::CLUB_CD;
        else
											NetMsgBrd.dwFLAGS = NULL;

        pMASTER->SendMsgViewAround(&NetMsgBrd);
    }

    return TRUE;
}
*/
BOOL GLGaeaServer::MsgClubCdCertifyChangeAF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    GLMSG::NET_CLUB_CD_CERTIFIER_CHANGE_AF* pPacket = (GLMSG::NET_CLUB_CD_CERTIFIER_CHANGE_AF*) nmg;
    if (pPacket->m_MagicNum != GLMSG::NET_CLUB_CD_CERTIFIER_CHANGE_AF::MAGIC_NUM)
        return FALSE;
    
    std::tr1::shared_ptr<GLClubField> spClub = m_pClubMan->GetClub(pPacket->m_ClubDbNum);
    if (!spClub)
        return FALSE;
    
    spClub->SetCdCertifier(pPacket->m_NewCdCertifier);
    
    GLChar* pNewCdCertifier = GetCharByDbNum(pPacket->m_NewCdCertifier);
    if (pNewCdCertifier)
    {
        // 신규 cd 인증자
        GLMSG::SNETPC_UPDATE_FLAGS_BRD NetMsgBrd;
        NetMsgBrd.dwGaeaID = pNewCdCertifier->GetGaeaID();
        // 동맹 클럽 체크
        if (spClub->IsMemberFlgCDCertify(pPacket->m_NewCdCertifier))
            NetMsgBrd.dwFLAGS |= SDROP_CHAR_BASE::CLUB_CD;
        else
            NetMsgBrd.dwFLAGS = NULL;
        pNewCdCertifier->SendMsgViewAround(&NetMsgBrd);
    }
    
    GLChar* pOldCdCertifier = GetCharByDbNum(pPacket->m_OldCdCertifier);
    if (pOldCdCertifier)
    {
        // 과거 cd 인증자
        GLMSG::SNETPC_UPDATE_FLAGS_BRD NetMsgBrd;
        NetMsgBrd.dwGaeaID = pOldCdCertifier->GetGaeaID();
        NetMsgBrd.dwFLAGS = NULL;
        pOldCdCertifier->SendMsgViewAround(&NetMsgBrd);
    }
    return TRUE;
}

BOOL GLGaeaServer::RequestClubAuthority(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GASSERT(IsField());

    GLMSG::NET_CLUB_MASTER_CHANGE_AF* pPacket = (GLMSG::NET_CLUB_MASTER_CHANGE_AF*) nmg;
    std::tr1::shared_ptr<GLClubField> spClub = m_pClubMan->GetClub(pPacket->m_ClubDbNum);
    if (!spClub)
        return FALSE;

    if (pPacket->m_ChaDbNum == GAEAID_NULL)
        return FALSE;

    spClub->SetMasterCharDbNum(pPacket->m_ChaDbNum);
    spClub->SetMasterCharLevel(pPacket->m_ChaLevel);

    GLCLUBMEMBER* pMember = spClub->GetMember(pPacket->m_ChaDbNum);
    if (!pMember)
        return FALSE;

    // Club 은 Agent 서버로 옮겨졌음. 2012-03-08 jgkim
    /*
    // 클럽락커를 저장한다.
    // 기존 마스터가 있던 필드라면 저장된다.
    pCLUB->SAVESTORAGE2DB ();
    pCLUB->RESET_STORAGE ();
    */    
    spClub->SetMasterName(pMember->m_ChaName);

    GLMSG::SNET_CLUB_AUTHORITY_CLT NetMsgClt;
    GLMSG::SNET_CLUB_AUTHORITY_BRD NetMsgBrd;

    NetMsgClt.dwCharID = spClub->MasterCharDbNum();
    NetMsgBrd.dwCharID = spClub->MasterCharDbNum();

    CLUBMEMBERS& Members = spClub->GetMembers();
    CLUBMEMBERS_CITER pos = Members.begin();
    CLUBMEMBERS_CITER end = Members.end();
    for ( ; pos!=end; ++pos)
    {
        const GLCLUBMEMBER& cMEMBER = pos->second;
        GLChar* pMember = GetCharByDbNum(cMEMBER.m_ChaDbNum);
        if (pMember)
        {
            SENDTOCLIENT(pMember->ClientSlot(), &NetMsgClt);
            NetMsgBrd.dwGaeaID = pMember->GetGaeaID();
            pMember->SendMsgViewAround(&NetMsgBrd);
        }
    }

    return TRUE;
}

BOOL GLGaeaServer::ReqClubMemberRename(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GASSERT(IsField());

    GLMSG::SNET_CLUB_MEMBER_RENAME_FLD* pNetMsg = (GLMSG::SNET_CLUB_MEMBER_RENAME_FLD*)nmg;
    //GLCLUB* pCLUB = m_cClubMan.GetClub ( pNetMsg->dwClubID );
    std::tr1::shared_ptr<GLClubField> pCLUB = m_pClubMan->GetClub ( pNetMsg->dwClubID );

    // 클럽에 가입했으면 클럽상의 이름 변경
    if ( pCLUB )
    {
        GLCLUBMEMBER* pClubMember = pCLUB->GetMember ( pNetMsg->dwCharID );

        if ( pCLUB->IsMaster ( pNetMsg->dwCharID ) )
        {
            pCLUB->SetMasterName(pNetMsg->szName);
        }
        if ( pClubMember )
        {
            pClubMember->m_ChaName = pNetMsg->szName;
        }
    }

    GLChar* pCHAR = GetCharByDbNum ( pNetMsg->dwCharID );
    if ( pCHAR )
    {
        ChangeNameMap ( pCHAR, pCHAR->m_szName, pNetMsg->szName );
    }

    return TRUE;
}

void GLGaeaServer::ClubMemberLevelUp(DWORD ClubDbNum, DWORD ChaDbNum, WORD ChaLevel)
{
    m_pClubMan->ChaLevelUp(ClubDbNum, ChaDbNum, ChaLevel);
}

void GLGaeaServer::ClubMemberGradeChange(DWORD ClubDbNum, DWORD MemberDbNum, size_t Grade)
{
    std::tr1::shared_ptr<GLClubField> spClub = m_pClubMan->GetClub(ClubDbNum);
    if (spClub)
        spClub->ChangeMemberGrade(MemberDbNum, Grade);
}

void GLGaeaServer::ClubGradeNameChange(DWORD ClubDbNum, size_t GradeIndex, const std::string& GradeName)
{
    std::tr1::shared_ptr<GLClubField> spClub = m_pClubMan->GetClub(ClubDbNum);
    if (spClub)
        spClub->ChangeGradeName(GradeIndex, GradeName, 0);
}

BOOL GLGaeaServer::MsgClubGradeFlagChangeAF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if (dwClientID != m_dwAgentSlot || dwGaeaID != GAEAID_NULL)
        return FALSE;

    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) nmg;
    NET_CLUB_GRADE_FLAG_CHANGE_AF RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return FALSE;

    std::tr1::shared_ptr<GLClubField> spClub = GetClub(RecvData.m_ClubDbNum);
    if (!spClub)
        return FALSE;

    spClub->SetGradeFlag(RecvData.m_Data);
    return TRUE;
}

BOOL GLGaeaServer::MsgClubPublicFlagChangeAF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if (dwClientID != m_dwAgentSlot || dwGaeaID != GAEAID_NULL)
        return FALSE;

    NET_CLUB_PULIC_FLAG_CHANGE_AF* pPacket = (NET_CLUB_PULIC_FLAG_CHANGE_AF*) nmg;
    std::tr1::shared_ptr<GLClubField> spClub = GetClub(pPacket->m_ClubDbNum);
    if (!spClub)
        return FALSE;

    spClub->PublicSet(pPacket->m_PublicFlag);
    return TRUE;
}

BOOL GLGaeaServer::MsgClubAutoKickAF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    // NET_MSG_CLUB_AUTO_KICK_AF
    if (dwClientID != m_dwAgentSlot || dwGaeaID != GAEAID_NULL)
        return FALSE;

    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) nmg;
    NET_CLUB_AUTO_KICKC_AF MsgAF;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), MsgAF))
    {
        sc::writeLogError("GLGaeaServer::MsgClubAutoKickAF");
        return FALSE;
    }
    
    std::tr1::shared_ptr<GLClubField> spClub = GetClub(MsgAF.m_ClubDbNum);
    if (!spClub)
        return FALSE;

    spClub->AutoKickOutSet(MsgAF.m_KickOut);
    return TRUE;
}

BOOL GLGaeaServer::MsgClubNewbieNoticeAF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    // NET_MSG_CLUB_NEWBIE_NOTICE_AF
    if (dwClientID != m_dwAgentSlot || dwGaeaID != GAEAID_NULL)
        return FALSE;

    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) nmg;
    NET_CLUB_NEWBIE_NOTICE_AF RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return FALSE;

    std::tr1::shared_ptr<GLClubField> spClub = GetClub(RecvData.m_ClubDbNum);
    if (spClub)
    {        
        spClub->SetNoticeNewbie(RecvData.m_NoticeNewbie);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL GLGaeaServer::MsgClubJoinTimeResetAF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if (dwClientID != m_dwAgentSlot || dwGaeaID != GAEAID_NULL)
        return FALSE;
    
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) nmg;
    NET_CLUB_JOIN_TIME_RESET_AF RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return FALSE;

    GLChar* pChar = GetCharByDbNum(RecvData.m_ChaDbNum);
    if (pChar)
    {
        pChar->SetClubSecedeTime(RecvData.m_Time);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL GLGaeaServer::MsgClubAllianceTimeResetAF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if (dwClientID != m_dwAgentSlot || dwGaeaID != GAEAID_NULL)
        return FALSE;

    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) nmg;
    NET_CLUB_ALLIANCE_TIME_RESET_AF RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return FALSE;

    std::tr1::shared_ptr<GLClubField> spClub = GetClub(RecvData.m_ClubDbNum);
    if (spClub)
    {
        spClub->SetAllianceSec(RecvData.m_Time);    
        spClub->SetAllianceDis(RecvData.m_Time);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL GLGaeaServer::MsgClubNameChangeCF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_CLUB_RENAME_CF* pNetMsg = ( GLMSG::SNET_CLUB_RENAME_CF* ) nmg;

	GLChar* pChar = GetChar( dwGaeaID );
	if ( !pChar )
	{
		return FALSE;
	}

	GLMSG::SNET_CLUB_RENAME_FB NetMsgFB;

	SINVENITEM* pInvenItem = pChar->InvenGetItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem )
	{
		NetMsgFB.nResult = CLUB_RENAME_FB_WRONG_ITEM;
		SENDTOCLIENT( pChar->ClientSlot(), &NetMsgFB );
		return FALSE;
	}

	if ( pChar->CheckCoolTime( pInvenItem->sItemCustom.GetNativeID() ) )
		return FALSE;

	const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem || pItem->sBasicOp.emItemType != ITEM_CLUBRENAME )
	{
		NetMsgFB.nResult = CLUB_RENAME_FB_WRONG_ITEM;
		SENDTOCLIENT( pChar->ClientSlot(), &NetMsgFB );
		return FALSE;
	}

	// Note : 레벨 조건 확인
	//
	if ( !pChar->SIMPLE_CHECK_ITEM( pInvenItem->sItemCustom ) )
	{
		return FALSE;
	}

	if ( !m_pClubMan )
	{
		return FALSE;
	}

	std::tr1::shared_ptr< GLClubField > pClub = m_pClubMan->GetClub( pChar->m_ClubDbNum );
	if ( !pClub )
		return FALSE;

	if ( !pClub->IsMaster( pChar->m_CharDbNum ) )
	{
		// 현재는 마스터만이 가능하다. 추후에 요청이 있을 때 클럽원 권한으로 이름 변경 권한 추가한다.
		NetMsgFB.nResult = CLUB_RENAME_FB_AUTH;
		SENDTOCLIENT( pChar->ClientSlot(), &NetMsgFB );
		return FALSE;
	}

	if ( !pNetMsg->szClubName )
	{
		return FALSE;
	}

	// Trim 하지 않는다. 아래 strClubName.FindOneOf 에서 체크된다.
	CString strClubName( pNetMsg->szClubName );

	size_t NameLength = strClubName.GetLength();
	if ( NameLength == 0 )
	{
		NetMsgFB.nResult = CLUB_RENAME_FB_NONAME;
		SENDTOCLIENT( pChar->ClientSlot(), &NetMsgFB );
		return FALSE;
	}
	else if ( NameLength < 4 )
	{
		// 한글기준 2글자 이상 되어야, 영문기준 4글자 이상 되어야
		NetMsgFB.nResult = CLUB_RENAME_FB_TOO_SHORT;
		SENDTOCLIENT( pChar->ClientSlot(), &NetMsgFB );
		return FALSE;
	}
	else if ( NameLength > ( CHAR_SZNAME - 1 ) )
	{
		NetMsgFB.nResult = CLUB_RENAME_FB_TOO_LONG;
		SENDTOCLIENT( pChar->ClientSlot(), &NetMsgFB );
		return FALSE;
	}
	
	if ( strClubName.FindOneOf(" ") != -1 )
	{
		NetMsgFB.nResult = CLUB_RENAME_FB_BLANK;
		SENDTOCLIENT( pChar->ClientSlot(), &NetMsgFB );
		return FALSE;
	}

	BOOL bFILTER0 = STRUTIL::CheckString( strClubName );
	BOOL bFILTER1 = CRanFilter::GetInstance().NameFilter( strClubName );
	if ( bFILTER0 || bFILTER1 )
	{
		NetMsgFB.nResult = CLUB_RENAME_FB_BAD_NAME;
		SENDTOCLIENT( pChar->ClientSlot(), &NetMsgFB );
		return FALSE;			
	}

	if ( m_ServiceProvider == SP_KOREA || m_ServiceProvider == SP_KOREA_TEST || m_ServiceProvider == SP_OFFICE_TEST )
	{
		if ( !sc::string::goodChaNameKorea( std::string( strClubName.GetString() ) ) )
		{
			NetMsgFB.nResult = CLUB_RENAME_FB_BAD_NAME;
			SENDTOCLIENT( pChar->ClientSlot(), &NetMsgFB );
			return FALSE;	
		}
	}

	if ( RENDERPARAM::emLangSet == language::THAILAND ) 
	{
		if ( !STRUTIL::CheckThaiString( strClubName ) )
		{
			// fb 필요
			NetMsgFB.nResult = CLUB_RENAME_FB_BAD_NAME;
			SENDTOCLIENT( pChar->ClientSlot(), &NetMsgFB );
			return FALSE;
		}
	}

	// 신청한 클럽 이름이 이미 사용중
	DWORD ClubNum = m_pClubMan->FindClub( pNetMsg->szClubName );
	if ( ClubNum != CLUB_NULL )
	{
		NetMsgFB.nResult = CLUB_RENAME_FB_NAME_DUP;
		SENDTOCLIENT( pChar->ClientSlot(), &NetMsgFB );
		return FALSE;
	}

	// 클럽배틀 여부 체크, 선도전 / CDM 은 체크하지 않는다. 클라이언트에 보여주는 클럽 이름은 서버 로직에서 가져와서 메시지 받아서 출력된다.
	// 에이전트에서만 체크가능한 클럽배틀 대기시간은 따로 여기에서 체크하지 않는다. 실제 적대클럽 리스트는 클럽배틀이 시작되어야만 출력된다.
	if ( pClub->GetAllBattleNum() > 0 )
	{
		NetMsgFB.nResult = CLUB_RENAME_FB_CLUB_BATTLE;
		SENDTOCLIENT( pChar->ClientSlot(), &NetMsgFB );
		return FALSE;
	}

	// 아이템 사용
	pChar->ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY );

	AddGameAdoJob( 
		db::DbActionPtr( 
		new db::ClubRename( pChar->m_ClubDbNum, pChar->m_CharDbNum, std::string( pNetMsg->szClubName ) ) ) );

	return TRUE;
}

BOOL GLGaeaServer::MsgClubNameChangeDF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if ( DBACTION_CLIENT_ID != dwClientID )
		return FALSE;

	GLMSG::SNET_CLUB_RENAME_DF* pNetMsg = ( GLMSG::SNET_CLUB_RENAME_DF* ) nmg;

	if ( pNetMsg->nResult != 0 )
	{
		// 실패 처리 필요, 로그 출력, 캐릭터가 있다면 아이템 복구 처리
		sc::writeLogError( sc::string::format( 
			"MsgClubNameChangeDF db update fail clubnum %1% chanum %2%", 
			pNetMsg->ClubNum, pNetMsg->ChaDbNum ) );
		return TRUE;
	}

	GLMSG::SNET_CLUB_RENAME_FA NetMsg;
	NetMsg.ClubNum = pNetMsg->ClubNum;
	NetMsg.ChaDbNum = pNetMsg->ChaDbNum;
	NetMsg.SetClubName( pNetMsg->szClubName );

	SENDTOAGENT( &NetMsg );

	return TRUE;
}

BOOL GLGaeaServer::MsgClubNameChangeAF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if ( dwClientID != m_dwAgentSlot )
		return FALSE;

	GLMSG::SNET_CLUB_RENAME_AF* pNetMsg = ( GLMSG::SNET_CLUB_RENAME_AF* ) nmg;

	if ( !m_pClubMan )
		return FALSE;

	if ( !m_pClubMan->ClubRename( pNetMsg->ClubNum, std::string( pNetMsg->szClubName ) ) )
	{
		sc::writeLogError( sc::string::format( 
			"MsgClubNameChangeAF SetClubName fail clubnum %1% newclubname %2%", 
			pNetMsg->ClubNum, pNetMsg->szClubName ) );
		return FALSE;
	}

	return TRUE;
}
