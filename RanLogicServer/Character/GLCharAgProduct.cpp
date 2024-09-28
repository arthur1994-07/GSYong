#include "pch.h"
#include "../../SigmaCore/Util/DateTime.h"
#include "../../SigmaCore/String/StringUtil.h"
#include "../../SigmaCore/Json/MinJson.h"
#include "../../SigmaCore/DebugInclude.h"

#include "../../RanLogic/GLUseFeatures.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/Msg/ProductMsg.h"

#include "../AgentServer/GLAgentServer.h"

#include "../Product/GLProductAgent.h"

#include "./GLCharAG.h"

bool GLCharAG::MsgReqProduction( NET_MSG_GENERIC* nmg )
{
    GLMSG::SNET_PRODUCT_REQ* pNetMsg = (GLMSG::SNET_PRODUCT_REQ*)nmg;
    if( m_pProduct == FALSE )
        return FALSE;

    pNetMsg->dwChaDbNum = ChaDbNum();

    // 제작 취소 요청이 아닐 때
    if ( pNetMsg->nKey != UINT_MAX )
    {
        // 제작 가능한 상태인가?
        int nState = m_pProduct->IsProduct( pNetMsg->nKey ,m_pServer, this );

        // 제작 가능 상태 검사 실패시
        if( nState != Product::EStateMotion )
        {
            GLMSG::SNET_PRODUCT_FB NetMsgFb;
            {
                NetMsgFb.dwChaDbNum = ChaDbNum();
                NetMsgFb.nKey = pNetMsg->nKey;
                NetMsgFb.nCount = pNetMsg->nCount;
                NetMsgFb.nState = nState;
            }
            m_pServer->SENDTOCLIENT( ClientSlot(), &NetMsgFb );
            return FALSE;
        }
    }
    // 제작이나 제작취소 요청을 위해서
    // 필드서버로 전송한다.
    m_pServer->SENDTOFIELDSVR(this->GetChannel(), this->GetCurrentFieldServer(), nmg);

    return TRUE;
}

bool GLCharAG::MsgReqProductIsLearnBook( NET_MSG_GENERIC* nmg )
{
    GLMSG::SNETPC_REQ_LEARN_PRODUCTBOOK* NetMsg = (GLMSG::SNETPC_REQ_LEARN_PRODUCTBOOK*) nmg;
    NetMsg->m_dwChaDbNum = ChaDbNum();

    if( m_pProduct == FALSE )
        return FALSE;

    if( m_pProduct->IsUsableBook( NetMsg->nItemID ) == FALSE )
        return FALSE;

    // 에이전트는 제작에 배우기에 대한 정보가 없다.
    // 필드서버로 전송한다.
    m_pServer->SENDTOFIELDSVR(this->GetChannel(), this->GetCurrentFieldServer(), nmg);

    return TRUE;
}

bool GLCharAG::MsgReqProductBookList( NET_MSG_GENERIC* nmg )
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*)nmg;
    GLMSG::NET_PRODUCT_BOOK_LIST_REQ RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return FALSE;

    // 배운 조합서 목록 요청
    if( m_pProduct)
        m_pProduct->MegSendBookList( m_pServer, this ); 

    return TRUE;
}

bool GLCharAG::MsgReqProductTypeList( NET_MSG_GENERIC* nmg )
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*)nmg;
    GLMSG::NET_PRODUCT_TYPE_LIST_REQ RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return FALSE;
    
    // 제작분야 리스트 요청
    if( m_pProduct)
        m_pProduct->MsgSendTypeList( m_pServer, this ); 

    return TRUE;
}

bool GLCharAG::MsgReqProductTypeAdd( NET_MSG_GENERIC* nmg )
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*)nmg;
    GLMSG::NET_PRODUCT_TYPE_ADD_REQ RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return FALSE;

    // 제작분야 추가 요청
    if( m_pProduct)
        m_pProduct->AddType( RecvData.m_nProductType, m_pServer, this );    
    
    return TRUE;
}

bool GLCharAG::MsgReqProductTypeDel( NET_MSG_GENERIC* nmg )
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*)nmg;
    GLMSG::NET_PRODUCT_TYPE_DEL_REQ RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return FALSE;

    if( m_pProduct == NULL )
        return FALSE;

    RecvData.m_dwChaDbNum = ChaDbNum();
    msgpack::sbuffer SendBuffer;
    msgpack::pack(SendBuffer, RecvData);
    // 삭제 [가능, 불가능] 정보는 에이전트에 있지만
    // 삭제가 가능하더라도 캐쉬템과 관려되어있기 때문에
    // 처리는 필드에서 한다.
    if( m_pProduct->IsDelType( RecvData.m_vecProductType, m_pServer, this ) )
        m_pServer->SENDTOFIELDSVR(this->GetChannel(), this->GetCurrentFieldServer(), NET_MSG_PRODUCT_TYPE_DEL_REQ, SendBuffer);

    return TRUE;
}

bool GLCharAG::MsgReqProductIsGetExpList( NET_MSG_GENERIC* nmg )
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*)nmg;
    GLMSG::NET_PRODUCT_ISGETEXP_LIST_ACK RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return FALSE;

    if( m_pProduct)
        m_pProduct->MsgSendIsGetExpList( m_pServer, this, false, RecvData.m_bFirst, RecvData.m_bLast );   

    return TRUE;
}

bool GLCharAG::MsgAckProductFA( NET_MSG_GENERIC* nmg )
{
    GLMSG::SNET_PRODUCT_FA* msg = (GLMSG::SNET_PRODUCT_FA*)nmg;
    
    // 물품 제작에 성공하면 경험치를 습득할지를 결정한다.
    if( msg->nState == Product::EStateSuccess)
	{
        if( m_pProduct)
            msg->nState = m_pProduct->ProductSuccess( msg->nKey, m_pServer, msg->dwChaDbNum );

		if( msg->nState == Product::EStateSuccess ||
			msg->nState == Product::EStateGetEXP )
		{
			// 제작 숙련도 확률 성공;
			switch( msg->nState )
			{
			case Product::EStateSuccess:
				// 아이템 생성에 성공했으므로 Exchange 로그를 남긴다;
				m_pProduct->LogProductExchange( msg->nKey, msg->llMoney, m_pServer, msg->dwChaDbNum, false, msg->iResultItemMakeNum );
				break;
			case Product::EStateGetEXP:
				// 아이템 생성과 숙련도 획득에 성공하였으므로 제작 로그와 Exchange 로그를 둘 다 남긴다;
				m_pProduct->GenerateItemSuccess( msg->nKey, m_pServer, msg->dwChaDbNum, true );

				// 숙련도 획득에 성공하였기 때문에 true를 넘긴다;
				m_pProduct->LogProductExchange( msg->nKey, msg->llMoney, m_pServer, msg->dwChaDbNum, true, msg->iResultItemMakeNum );
				break;
			}
		}
	}
    
	// 기존 구조의 변경을 최대한 막기 위해 패킷을 두개로 분리하는 과정에서;
	// 발생하는 비효율;
	GLMSG::SNET_PRODUCT_FB NetMsgFB;
	NetMsgFB.dwChaDbNum = msg->dwChaDbNum;
	NetMsgFB.nKey = msg->nKey;
	NetMsgFB.nCount = msg->nCount;
	NetMsgFB.dwCharGaeaID = msg->dwCharGaeaID;
	NetMsgFB.nState = msg->nState;
    m_pServer->SENDTOCLIENT( this->ClientSlot(), &NetMsgFB );

    return TRUE;
}

bool GLCharAG::MsgAckLearnBookFA( NET_MSG_GENERIC* nmg )
{
    // 배우기에 실패한경우 패킷이 오지 않는다.
    GLMSG::SNETPC_REQ_LEARN_PRODUCTBOOK_FB* msg = (GLMSG::SNETPC_REQ_LEARN_PRODUCTBOOK_FB*)nmg;
    

    const SITEM* pItem = GLogicData::GetInstance().GetItem( msg->nItemID );
    if( !pItem )
        GASSERT( pItem && "불가능한 시도" );

    DWORD nProductBook = pItem->sSkillBookOp.sSkill_ID.dwID;

    if( m_pProduct)
        m_pProduct->LearnBookSuccess( nProductBook );  

    m_pServer->SENDTOCLIENT( this->ClientSlot(), nmg );

    return TRUE;
}

bool GLCharAG::MsgAckTypeDeleteFA( NET_MSG_GENERIC* nmg )
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*)nmg;
    GLMSG::NET_PRODUCT_TYPE_DEL_ACK RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return FALSE;

    if( m_pProduct)
        if( RecvData.m_bOK == true )
            m_pProduct->TypeDeleteSuccess( RecvData.m_vecProductType, m_pServer, ChaDbNum() );   

    m_pServer->SENDTOCLIENT( this->ClientSlot(), pPacket );

    return TRUE;

}