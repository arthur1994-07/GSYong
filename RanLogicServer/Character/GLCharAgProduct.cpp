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

    // ���� ��� ��û�� �ƴ� ��
    if ( pNetMsg->nKey != UINT_MAX )
    {
        // ���� ������ �����ΰ�?
        int nState = m_pProduct->IsProduct( pNetMsg->nKey ,m_pServer, this );

        // ���� ���� ���� �˻� ���н�
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
    // �����̳� ������� ��û�� ���ؼ�
    // �ʵ弭���� �����Ѵ�.
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

    // ������Ʈ�� ���ۿ� ���⿡ ���� ������ ����.
    // �ʵ弭���� �����Ѵ�.
    m_pServer->SENDTOFIELDSVR(this->GetChannel(), this->GetCurrentFieldServer(), nmg);

    return TRUE;
}

bool GLCharAG::MsgReqProductBookList( NET_MSG_GENERIC* nmg )
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*)nmg;
    GLMSG::NET_PRODUCT_BOOK_LIST_REQ RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return FALSE;

    // ��� ���ռ� ��� ��û
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
    
    // ���ۺо� ����Ʈ ��û
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

    // ���ۺо� �߰� ��û
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
    // ���� [����, �Ұ���] ������ ������Ʈ�� ������
    // ������ �����ϴ��� ĳ���۰� �����Ǿ��ֱ� ������
    // ó���� �ʵ忡�� �Ѵ�.
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
    
    // ��ǰ ���ۿ� �����ϸ� ����ġ�� ���������� �����Ѵ�.
    if( msg->nState == Product::EStateSuccess)
	{
        if( m_pProduct)
            msg->nState = m_pProduct->ProductSuccess( msg->nKey, m_pServer, msg->dwChaDbNum );

		if( msg->nState == Product::EStateSuccess ||
			msg->nState == Product::EStateGetEXP )
		{
			// ���� ���õ� Ȯ�� ����;
			switch( msg->nState )
			{
			case Product::EStateSuccess:
				// ������ ������ ���������Ƿ� Exchange �α׸� �����;
				m_pProduct->LogProductExchange( msg->nKey, msg->llMoney, m_pServer, msg->dwChaDbNum, false, msg->iResultItemMakeNum );
				break;
			case Product::EStateGetEXP:
				// ������ ������ ���õ� ȹ�濡 �����Ͽ����Ƿ� ���� �α׿� Exchange �α׸� �� �� �����;
				m_pProduct->GenerateItemSuccess( msg->nKey, m_pServer, msg->dwChaDbNum, true );

				// ���õ� ȹ�濡 �����Ͽ��� ������ true�� �ѱ��;
				m_pProduct->LogProductExchange( msg->nKey, msg->llMoney, m_pServer, msg->dwChaDbNum, true, msg->iResultItemMakeNum );
				break;
			}
		}
	}
    
	// ���� ������ ������ �ִ��� ���� ���� ��Ŷ�� �ΰ��� �и��ϴ� ��������;
	// �߻��ϴ� ��ȿ��;
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
    // ���⿡ �����Ѱ�� ��Ŷ�� ���� �ʴ´�.
    GLMSG::SNETPC_REQ_LEARN_PRODUCTBOOK_FB* msg = (GLMSG::SNETPC_REQ_LEARN_PRODUCTBOOK_FB*)nmg;
    

    const SITEM* pItem = GLogicData::GetInstance().GetItem( msg->nItemID );
    if( !pItem )
        GASSERT( pItem && "�Ұ����� �õ�" );

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