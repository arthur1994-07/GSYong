#include "pch.h"
#include "PostRenewField.h"


#include "../../RanLogic/PointShop/PointShopDefine.h"
#include "../../RanLogic/Post/PostDefine.h"
#include "../../RanLogic/Item/GLItemCustom.h"

#include "../../RanLogic/Msg/PostRnMsg.h"

#include "../Database/DBAction/DbActionGamePost.h"
//#include "../Server/s_CFieldServer.h"
#include "../FieldServer/GLGaeaServer.h"

void PostRenewField::SendPost( DWORD dwChaNumSend, DWORD dwChaNumRecv, std::string strTitle, std::string strConTent, WORD wPostStyle /*=0*/, EMPOST_MONEY_TYPE wMoneyType /*= EMPOST_MONEY_NOT_USE*/, LONGLONG llMoney/*=0*/, SITEMCUSTOM sItemCustom/*=SITEMCUSTOM(NATIVEID_NULL()) */ )
{
	GLMSG::NET_POST_RENEW_SEND_REQ RecvData;

	// 보내는 사람
	GLChar* pChar = m_pGaeaServer->GetCharByDbNum( dwChaNumSend );

	if (pChar)
	{
		PostInfo& post = RecvData.postInfo;

		switch( wMoneyType )
		{
		case EMPOST_MONEY_POINT:
			if ( pChar->PointShopPoint() < llMoney )
				return;
			break;
		case EMPOST_MONEY_GOLD:
			if ( pChar->GetInvenMoney() < llMoney )
				return;
			break;
		}
		post.dwSendUserNum	= pChar->GetUserID();
		post.dwSendChaNum	= pChar->CharDbNum();
		post.strSendChaName = pChar->GetName();
		post.dwRecvChaNum	= dwChaNumRecv;
		post.wStyle			= wPostStyle;
		post.strTitle		= strTitle;
		post.strContent		= strConTent;
		post.wAttachType	|= EMPOST_ATTACH_TEXT;
		post.wMailType	= EMPOST_USER;
		
		if ( pChar->UserLevel() >= USER_USER_GM )
			post.wMailType = EMPOST_MAIL_GM;	//GM Mail
		else
			post.wMailType = EMPOST_MAIL_USER;	//GM Mail

		// 아이템 첨부
		SINVENITEM* pInvenItem = pChar->m_cInventory.FindItem( sItemCustom );
		if( pInvenItem != NULL )
		{
			post.sItemCutom = sItemCustom;
			post.wAttachType |= EMPOST_ATTACH_ITEM;

			// #item
			// 컴파일 에러만 나지 않도록 수정한다. 우편 개선한 거 사용하지 않는다. 
			if( pChar->InvenDeleteItem( pInvenItem->wPosX, pInvenItem->wPosY, true, FALSE, 19843, true ) == false )
				return;


			// 통계 및 로그 수정
			pChar->LogItemExchange(
				sItemCustom, 
				gamelog::ID_CHAR, dwChaNumSend,
				gamelog::ID_CHAR, dwChaNumRecv,
				ITEM_ROUTE_POST, 
				sItemCustom.wTurnNum,
				( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap(), LOG_ITEM_POST_ATTACHEDITEM,
				true );
		}
		else
		{
			post.sItemCutom = SITEMCUSTOM(NATIVEID_NULL());			
		}

		
		// 돈 or 포인트 첨부
		if ( 0 < llMoney && wMoneyType != EMPOST_MONEY_NOT_USE )
		{
			switch( wMoneyType )
			{
			case EMPOST_MONEY_POINT:
				if (llMoney <= pChar->PointShopPoint())
					pChar->PointShopPointSub( llMoney, pointshop::POST_SYSTEM, pointshop::POINT_SUB_TYPE_POST );
				break;
			case EMPOST_MONEY_GOLD:
				if (llMoney <= pChar->GetInvenMoney())
					pChar->SubtractInvenMoneyAndUpdate( llMoney, SUBTRACT_INVEN_MONEY_POST_ATTACHED_MONEY );
				break;
			}
			post.wAttachType |= EMPOST_ATTACH_MONEY;
			post.wMoneyType = wMoneyType;
			post.llMoney = llMoney;
		}

		// 캐쉬로 전송
		msgpack::sbuffer SendBuffer;
		msgpack::pack(SendBuffer, RecvData);
		m_pGaeaServer->SENDTOCACHE( NET_MSG_POST_RENEW_SEND_REQ, SendBuffer );

	}

}

EMPOST_ATTACH_CODE PostRenewField::MsgTakeAttachAck( const DWORD& dwSendChaNum, const DWORD& dwRecvChaNum, const DWORD& dwPostID, const SITEMCUSTOM& sItem, const EMPOST_MONEY_TYPE& emMoneyType, const LONGLONG& llMoney, const EMPOST_ATTACH_CODE& emCode )
{
	GLChar* pChar = m_pGaeaServer->GetCharByDbNum( dwRecvChaNum );

	if (pChar==NULL)
		return EMPOST_ATTACH_ERROR;

	if((EMPOST_ATTACH_CODE)emCode != EMPOST_ATTACH_OK)
	{
		// 돈은 앞에서 뺀다.
		// 롤백 해야할 상황인가?
		// GLGaeaServer::MsgPostRenewAttachAck()
		// DB가 문제있는 경우를 제외하고 여기로 일어온경우 일단 문제를 의심해야한다.
		switch (emMoneyType)
		{
		case EMPOST_MONEY_CHARGE_GOLD:
			pChar->AddInvenMoneyAndUpdate(llMoney, ADD_INVEN_MONEY_POST_CHARGE_ROLLBACK);
			pChar->LogMoneyExchange( gamelog::ID_CHAR, dwSendChaNum, gamelog::ID_CHAR, pChar->m_CharDbNum, llMoney, ITEM_ROUTE_CONSIGMENT_SALE, ( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap(), LOG_MONEY_INFO_POST_BY_CONSIGEMENT );
			pChar->LogMoneyExchange( gamelog::ID_CHAR, pChar->m_CharDbNum, gamelog::ID_CHAR, pChar->m_CharDbNum, pChar->GetInvenMoney(), ITEM_ROUTE_CONSIGMENT_SALE, ( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap(), LOG_MONEY_CURRENT_INVEN_BY_CONSIGEMENT );
			break;
		case EMPOST_MONEY_CHARGE_POINT:
			pChar->PointShopPointAdd( 0, llMoney, pointshop::POST_SYSTEM );

			pChar->LogMoneyExchange( gamelog::ID_CHAR, dwSendChaNum, gamelog::ID_CHAR, pChar->m_CharDbNum, llMoney, ITEM_ROUTE_CONSIGMENT_SALE, ( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap(), LOG_RANPOINT_INFO_POST_BY_CONSIGEMENT );
			break;
		}


		return EMPOST_ATTACH_ERROR;
	}
	

	// 인벤 부족
	if ( pChar->m_cInventory.GetAmountEmptyPile( sItem.GetNativeID(), FALSE ) < sItem.wTurnNum )
	{
		return EMPOST_ATTACH_INVEN_IS_FULL;
	}

	switch (emMoneyType)
	{
	case EMPOST_MONEY_GOLD:
		pChar->AddInvenMoneyAndUpdate(llMoney, ADD_INVEN_MONEY_POST_ATTACHED_MONEY);

		// 통계 및 로그 수정
		pChar->LogMoneyExchange( gamelog::ID_CHAR, dwSendChaNum, gamelog::ID_CHAR, pChar->m_CharDbNum, llMoney, ITEM_ROUTE_CONSIGMENT_SALE, ( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap(), LOG_MONEY_INFO_POST_BY_CONSIGEMENT );
		pChar->LogMoneyExchange( gamelog::ID_CHAR, pChar->m_CharDbNum, gamelog::ID_CHAR, pChar->m_CharDbNum, pChar->GetInvenMoney(), ITEM_ROUTE_CONSIGMENT_SALE, ( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap(), LOG_MONEY_CURRENT_INVEN_BY_CONSIGEMENT );
		break;
	case EMPOST_MONEY_POINT:
		pChar->PointShopPointAdd( 0, llMoney, pointshop::POST_SYSTEM );

		// 통계 및 로그 수정
		pChar->LogMoneyExchange( gamelog::ID_CHAR, dwSendChaNum, gamelog::ID_CHAR, pChar->m_CharDbNum, llMoney, ITEM_ROUTE_CONSIGMENT_SALE, ( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap(), LOG_RANPOINT_INFO_POST_BY_CONSIGEMENT );
		break;
	}

	// #item
	// 컴파일 에러만 나지 않도록 수정한다. 우편 개선한 거 사용하지 않는다. 
	if( pChar->InvenInsertItem( sItem, true, 1, true, false, true ) == FALSE )
	{
		// ERROR 치명적인 에러.
		// 위에서 체크했는데...

		int i=0;
	}


	// 통계 및 로그 수정
	pChar->LogItemExchange(
		sItem, 
		gamelog::ID_CHAR, dwSendChaNum,
		gamelog::ID_CHAR, pChar->CharDbNum(),
		ITEM_ROUTE_POST, 
		sItem.wTurnNum,
		( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap(), LOG_ITEM_POST_ATTACHEDITEM,
		true );


	return EMPOST_ATTACH_OK;
}
