#include "pch.h"

#include "../../SigmaCore/String/StringFormat.h"

#include "../../RanLogic/Msg/CharacterMsg.h"
#include "../../RanLogic/Msg/GLContrlPrivateMarket.h"
#include "../../RanLogic/Msg/ConsignmentSaleMsg.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/GLUseFeatures.h"

#include "../Database/DbManager.h"

#include "../PrivateMarket/GLPrivateMarketSearchMan.h"

#include "./CacheServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void CacheServer::PrivateMarketDirectLoad()
{
	if ( m_pPrivateMarketSearch && IsIntegrationServer() )
	{
		m_pPrivateMarketSearch->UpdateBasicOption( 
			private_market::GLPrivateMarketDefine::Instance()->GetCheckChannel(), 
			private_market::GLPrivateMarketDefine::Instance()->GetCheckSchool(), 
			private_market::GLPrivateMarketDefine::Instance()->GetAllPage(), 
			private_market::GLPrivateMarketDefine::Instance()->GetSearchItemNumberPerPage(), 
			private_market::GLPrivateMarketDefine::Instance()->GetLogNumberPerPage(), 
			private_market::GLPrivateMarketDefine::Instance()->GetLogNumberMaxPerCharacter(), 
			private_market::GLPrivateMarketDefine::Instance()->GetSearchOption() );

		if ( true == GLUseFeatures::GetInstance().IsUsingPrivateMarketConsignmentSale() )
		{
			private_market::SALE_ITEMS_VECTOR vecSaleItems;
			if ( m_pDbManager->ConsignmentsSaleSelect( vecSaleItems ) == sc::db::DB_ERROR )
				return;

			for ( size_t i = 0; i < vecSaleItems.size(); i++ )
			{
				private_market::ConsignmentSaleItem& SaleItem = vecSaleItems[i];

				// 아이템 등록
				m_pPrivateMarketSearch->InsertConsigmentSaleItem( 
					SaleItem.strChaName,
					SaleItem.dwChaDbNum, 
					SaleItem.GetGUID(), 
					SaleItem.sItemcustom, 
					SaleItem.wTurnNum, 
					SaleItem.llPrice,
					private_market::GLPrivateMarketDefine::Instance()->SalesCompletionFee(), 
					static_cast<private_market::eSellType>( SaleItem.SellType ),
					SaleItem.ExpirationDate, 
					SaleItem.ServerGroup );
			}
		}
	}
}

void CacheServer::FrameMovePrivateMarket()
{
	if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
	{
		return;
	}

	if ( m_pPrivateMarketSearch )
	{
		m_pPrivateMarketSearch->FrameMove();
	}
}

void CacheServer::MsgPrivateMarketSellerRegisterFH( DWORD dwClient, NET_MSG_GENERIC* pMsg )
{
	if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
	{
		sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
		return;
	}

	GLMSG::SNETPC_PMARKET_SELLER_REGISTER_FH* pPacket = ( GLMSG::SNETPC_PMARKET_SELLER_REGISTER_FH* ) pMsg;

	if ( m_pPrivateMarketSearch )
	{
		// 판매자 등록
		m_pPrivateMarketSearch->InsertSeller( 
			pPacket->dwChaDbNum, 
			pPacket->szChaName, 
			pPacket->sMapID, 
			pPacket->wSchool, 
			pPacket->nChannel, 
			dwClient, 
			pPacket->nServerGroup );
	}
}

void CacheServer::MsgPrivateMarketItemRegisterFH( DWORD dwClient, NET_MSG_GENERIC* pMsg )
{
	if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
	{
		sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
		return;
	}

	GLMSG::SNETPC_PMARKET_ITEM_REGISTER_FH* pPacket = ( GLMSG::SNETPC_PMARKET_ITEM_REGISTER_FH* ) pMsg;

	if ( m_pPrivateMarketSearch )
	{
		// 아이템 등록
		m_pPrivateMarketSearch->InsertSaleItem( 
			pPacket->dwChaDbNum, 
			pPacket->sItemGUID, 
			pPacket->sItemcustom, 
			pPacket->dwItemTurnNum, 
			pPacket->llItemPrice,
			pPacket->fCommissionRate, 
			pPacket->nServerGroup );
	}
}

void CacheServer::MsgPrivateMarketItemSearchBasicFH( DWORD dwClient, NET_MSG_GENERIC* pMsg )
{
	if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
	{
		sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
		return;
	}

	NET_MSG_PACK* pPacket = ( NET_MSG_PACK* ) pMsg;
	GLMSG::SNETPC_PMARKET_ITEM_SEARCH_BASIC_FH RecvData;
	if ( false == msgpack::BufferToObject( pPacket->DataBuffer(), pPacket->DataSize(), RecvData ) )
	{
		return;
	}

	if ( m_pPrivateMarketSearch )
	{
		WORD wItemTotalNum = 0;
		std::vector< private_market::sSaleItem > vecSaleItems;
		int nResult = m_pPrivateMarketSearch->GetSaleItems( 
			RecvData.sBasic.wPage, 
			RecvData.nChannel, 
			RecvData.wSchool, 
			RecvData.sBasic, 
			vecSaleItems, 
			wItemTotalNum, 
			RecvData.nServerGroup );

		size_t nSize = vecSaleItems.size();

		if ( 1 == RecvData.sBasic.wPage )
		{
			GLMSG::SNETPC_PMARKET_ITEM_NUM_TOTAL_HFC NetMsgItemNumTotalHA;
			NetMsgItemNumTotalHA.dwChaDbNum = RecvData.dwChaDbNum;
			NetMsgItemNumTotalHA.wItemTotalNum = wItemTotalNum;
			SendClient( dwClient, &NetMsgItemNumTotalHA );
		}

		// agent 로 결과 전송
		GLMSG::SNETPC_PMARKET_ITEM_NUM_HFC NetMsgItemNumHF;
		NetMsgItemNumHF.dwChaDbNum = RecvData.dwChaDbNum;
		NetMsgItemNumHF.wItemNum = static_cast< WORD >( nSize );
		SendClient( dwClient, &NetMsgItemNumHF );

		for ( size_t loop = 0; loop < nSize; ++loop )
		{
			GLMSG::SNETPC_PMARKET_ITEM_HFC NetMsgItemHF( RecvData.dwChaDbNum, vecSaleItems[loop] );
			SendClient( dwClient, &NetMsgItemHF );
		}

		GLMSG::SNETPC_PMARKET_ITEM_END_HFC NetMsgItemEndHF;
		NetMsgItemEndHF.dwChaDbNum = RecvData.dwChaDbNum;
		SendClient( dwClient, &NetMsgItemEndHF );
	}
}

void CacheServer::MsgPrivateMarketItemBuyFH( DWORD dwClient, NET_MSG_GENERIC* pMsg )
{
	if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
	{
		sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
		return;
	}

	GLMSG::SNETPC_PMARKET_ITEM_BUY_FH* pPacket = ( GLMSG::SNETPC_PMARKET_ITEM_BUY_FH* ) pMsg;

	// 구매요청
	// sql 에 해당 아이템이 존재하는지 확인한다.
	// 없다면 에러 메시지 전송
	// 있다면
	// 구매자가 있는 필드서버에 가서 아이템금액 지불
	// 판매자가 있는 필드서버에 가서 판매자의 아이템을 구매자한테 우편으로 전송한다.
	// 도중에 문제가 생기지 않도록 한다.
	// 구매자의 돈과 판매자의 아이템을 미리 홀딩시켜 놓고, 문제가 없을 때 처리한다.
	if ( m_pPrivateMarketSearch )
	{
		LONGLONG llPriceTotal = 0;
        int nSellType = 0;

		int nResult = m_pPrivateMarketSearch->PrepareItemBuy( 
			pPacket->sItemGUID, 
			pPacket->dwItemTurnNum, 
			dwClient, 
			pPacket->dwChaDbNum, 
			pPacket->szName, 
			llPriceTotal,
            nSellType, 
			pPacket->nServerGroup );
		if ( private_market::eSuccess != nResult )
		{
			// 무언가 에러 발생
			// nResult 전송
			GLMSG::SNETPC_PMARKET_COMMON_FB NetMsgFB;
			NetMsgFB.nFeedback = nResult;
			NetMsgFB.dwChaDbNum = pPacket->dwChaDbNum;
			NetMsgFB.sItemGUID = pPacket->sItemGUID;
			SendClient( dwClient, &NetMsgFB );
			return;
		}
        
        if ( nSellType == private_market::eSellType_PrivateMarket )
        {
            //! 개인상점 검색 구매

            // 구매자의 필드서버로 가서 구매대금을 지불하도록 한다.
            GLMSG::SNETPC_PMARKET_ITEM_PAY_MONEY_HF NetMsgHF;
            NetMsgHF.dwChaDbNum = pPacket->dwChaDbNum;
            NetMsgHF.llItemPriceTotal = llPriceTotal;
            NetMsgHF.sItemGUID = pPacket->sItemGUID;
            SendClient( dwClient, &NetMsgHF );
        }
        else
        {
            //! 위탁판매 검색 구매

            // 판매자가 있는 필드서버로 가서 처리를 한다.
            // 구매 처리
            // 판매자 정보 가져온다.
            // 필드슬롯, 캐릭터번호
            private_market::sHoldItem sItem;
            int nResult = m_pPrivateMarketSearch->GetHoldItem( pPacket->sItemGUID, sItem );
            if ( private_market::eSuccess != nResult )
            {
                // 구매 실패
                return;
            }

            // 판매자의 필드로 가서 구매처리한다.
            // 판매자의 아이템 구매자에게 우편전송
            // 판매자의 아이템 구매완료 처리
            GLMSG::NET_CONSIGNMENT_SALE_PURCHAS_REQ SendData;
            {
                SendData.strBuyerName = pPacket->szName;
                SendData.dwBuyerDbNum = pPacket->dwChaDbNum;
                SendData.strSellerName = sItem.szName;
                SendData.dwSellerDbNum = sItem.dwSellerChaDbNum;
                
                std::string strGuid = sc::string::uuidToString( pPacket->sItemGUID );
                sc::string::upperString( strGuid );
                SendData.strGUID = strGuid;

                SendData.nPurchaseCount = sItem.dwItemTurnNum;
                SendData.llItemPriceTotal = sItem.llItemPrice * sItem.dwItemTurnNum;
                SendData.nSellType = nSellType;
            }
            msgpack::sbuffer PackBuffer;
            msgpack::pack( PackBuffer, SendData );

            // 필드로 가서 포인트(골드)를 감소시키면서 구매를 진행한다.
            SendClient( dwClient, NET_MSG_CONSIGNMENT_SALE_PURCHASE_REQ, PackBuffer);
        }
	}
}

void CacheServer::MsgPrivateMarketItemPayMoneyFH( DWORD dwClient, NET_MSG_GENERIC* pMsg )
{
	if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
	{
		sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
		return;
	}

	GLMSG::SNETPC_PMARKET_ITEM_PAY_MONEY_FH* pPacket = ( GLMSG::SNETPC_PMARKET_ITEM_PAY_MONEY_FH* ) pMsg;

	if ( m_pPrivateMarketSearch )
	{
		if ( private_market::eSuccess != pPacket->nResult )
		{
			// 구매자의 돈을 미리 빼러 갔는데 문제 발생
			// private_market::eNotExistBuyer
			// private_market::eMoneyLack
			// 홀드된 아이템 삭제하고
			// private_market::eMoneyLack 인 경우 구매자에게 알려준다.

			int nResult = m_pPrivateMarketSearch->DeleteHoldItem( pPacket->sItemGUID );
			if ( private_market::eSuccess != nResult )
			{
				sc::writeLogError( "MsgPrivateMarketItemPayMoneyFH DeleteHoldItem failed." );
			}

			if ( private_market::eMoneyLack == pPacket->nResult )
			{
				// 구매자에게 알려준다.
				GLMSG::SNETPC_PMARKET_COMMON_FB NetMsgFB;
				NetMsgFB.nFeedback = private_market::eMoneyLack;
				NetMsgFB.dwChaDbNum = pPacket->dwChaDbNum;
				NetMsgFB.sItemGUID = pPacket->sItemGUID;
				SendClient( dwClient, &NetMsgFB );
			}

			return;
		}

		// 판매자가 있는 필드서버로 가서 처리를 한다.
		// 구매 처리
		// 판매자 정보 가져온다.
		// 필드슬롯, 캐릭터번호
		private_market::sHoldItem sItem;
		int nResult = m_pPrivateMarketSearch->GetHoldItem( pPacket->sItemGUID, sItem );
		if ( private_market::eSuccess != nResult )
		{
			// eNullTablePointer 의 경우는 테스트 초기에 생길 수 있다. sql 구문 오류
			// eNotExistHoldItem 는 거의 생길 수 없는 경우이다.
			// 구매자의 필드로 가서 돈 롤백한다.
			GLMSG::SNETPC_PMARKET_ITEM_PAY_MONEY_ROLLBACK_HF NetMsgHF;
			NetMsgHF.dwChaDbNum = pPacket->dwChaDbNum;
			NetMsgHF.llItemPriceTotal = pPacket->llItemPriceTotal;
			NetMsgHF.sItemGUID = pPacket->sItemGUID;
			SendClient( dwClient, &NetMsgHF );
			return;
		}

		// 판매자의 필드로 가서 구매처리한다.
		// 판매자의 아이템 구매자에게 우편전송
		// 판매자의 아이템 구매완료 처리
		// 캐시로 결과 전송
		GLMSG::SNETPC_PMARKET_ITEM_BUY_HF NetMsgHF;
		NetMsgHF.SetBuyerName( pPacket->szBuyerName );
		NetMsgHF.dwSellerChaDbNum = sItem.dwSellerChaDbNum;
		NetMsgHF.dwItemTurnNum = sItem.dwItemTurnNum;
		NetMsgHF.llItemPrice = sItem.llItemPrice;
		NetMsgHF.sItemGUID = pPacket->sItemGUID;
		NetMsgHF.dwBuyerChaDbNum = pPacket->dwChaDbNum;
		if ( NET_ERROR == SendClient( sItem.dwSellerFieldSlot, &NetMsgHF ) )
		{
			sc::writeLogError( 
				sc::string::format( 
				"Critical error MsgPrivateMarketItemPayMoneyFH must recover chanum:%1%, game money:%2%",
				pPacket->dwChaDbNum,
				pPacket->llItemPriceTotal ) );
		}
	}
}

void CacheServer::MsgPrivateMarketItemBuyRollbackFH( DWORD dwClient, NET_MSG_GENERIC* pMsg )
{
	if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
	{
		sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
		return;
	}

	GLMSG::SNETPC_PMARKET_ITEM_BUY_ROLLBACK_FH* pPacket = ( GLMSG::SNETPC_PMARKET_ITEM_BUY_ROLLBACK_FH* ) pMsg;

	// 판매자의 필드서버로 갔으나 판매자가 없다.
	if ( m_pPrivateMarketSearch )
	{
		private_market::sHoldItem sItem;
		int nResult = m_pPrivateMarketSearch->GetHoldItem( pPacket->sItemGUID, sItem );
		if ( private_market::eSuccess == nResult )
		{
			// 구매자 돈 롤백
			GLMSG::SNETPC_PMARKET_ITEM_PAY_MONEY_ROLLBACK_HF NetMsgHF;
			NetMsgHF.dwChaDbNum = pPacket->dwBuyerChaDbNum;
			NetMsgHF.llItemPriceTotal = pPacket->llItemPrice * pPacket->dwItemTurnNum;
			NetMsgHF.sItemGUID = pPacket->sItemGUID;
			SendClient( sItem.dwBuyerFieldSlot, &NetMsgHF );
		}
		else
		{
			// critical error
			// pPacket->dwBuyerChaDbNum 에게 pPacket->llItemPrice * pPacket->dwItemTurnNum 을 돌려줘야 한다.
			sc::writeLogError( sc::string::format( 
				"MsgPrivateMarketItemBuyRollbackFH GetHoldItem error. chanum %1% price %2% turnnum %3%", 
				pPacket->dwBuyerChaDbNum, pPacket->llItemPrice, pPacket->dwItemTurnNum ) );
		}

		// 그 판매자를 삭제하고
		// 그 판매자 아이템 모두 삭제
		// 홀드 아이템도 마찬가지
		// 모두 삭제해야 하는게 정상이지만
		// 우선, 그 홀드아이템과 판매아이템에서 그 아이템만 삭제한다.
		m_pPrivateMarketSearch->DeleteItem( pPacket->sItemGUID );
		m_pPrivateMarketSearch->DeleteHoldItem( pPacket->sItemGUID );
	}
}

void CacheServer::MsgPrivateMarketItemBuyResultFH( DWORD dwClient, NET_MSG_GENERIC* pMsg )
{
	if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
	{
		sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
		return;
	}

	GLMSG::SNETPC_PMARKET_ITEM_BUY_RESULT_FH* pPacket = ( GLMSG::SNETPC_PMARKET_ITEM_BUY_RESULT_FH* ) pMsg;

	if ( m_pPrivateMarketSearch )
	{
		// 로그남긴다. 구매자에게 메시지 전송
		if ( pPacket->bLog )
		{
			// 시간 | 판매자 | 구매자 | 개수 | 금액
			// 실제 데이터베이스에 남길 필요는 없다.
			// 거래를 하면 원래 로그에 남기 때문이다.
			// 서버 로컬에 sqlite 파일로 저장하도록 한다.
			m_pPrivateMarketSearch->InsertSaleLog( pPacket->sItemGUID );
		}

		// 거래 성공
		if ( pPacket->bLog && pPacket->bUpdateSaleItem )
		{
			private_market::sHoldItem sItem;
			int nResult = m_pPrivateMarketSearch->GetHoldItem( pPacket->sItemGUID, sItem );
			if ( private_market::eSuccess == nResult )
			{
				GLMSG::SNETPC_PMARKET_COMMON_FB NetMsgFB;
				NetMsgFB.nFeedback = private_market::eSuccess;
				NetMsgFB.dwChaDbNum = sItem.dwBuyerChaDbNum;
				NetMsgFB.sItemGUID = pPacket->sItemGUID;
				SendClient( sItem.dwBuyerFieldSlot, &NetMsgFB );

				// 구매자 필드로 메시지 보내서 구매한 아이템 우편 전송한다.
				if ( pPacket->AttachItem.GetNativeID() != NATIVEID_NULL() )
				{
					// 필요한 정보
					// sItem.dwSellerChaDbNum, pPacket->szSellerName, sItem.dwBuyerChaDbNum, pPacket->szBuyerName, pPacket->AttachItem
					// 서버그룹이 다르다면 아이템 디비 상태 insert 로 변경
					/*
					BOOL bResult = m_pGaeaServer->PostSendSystemMail(
					pPacket->dwBuyerChaDbNum, 
					strRecverName, 
					AttachItem, 
					0, 
					ATTENDANCE_REWARD_POST_DESIGN, 
					pPacket->dwSellerChaDbNum, 
					strSellerName, 
					std::string( ID2SERVERTEXT( "PRIVATEMARKET_BUY_POST_TITLE" ) ), 
					std::string( ID2SERVERTEXT( "PRIVATEMARKET_BUY_POST_CONTENT" ) ), 
					false
					);
					*/
					GLMSG::SNETPC_PMARKET_POST_SEND_HF NetMsgHF;
					NetMsgHF.dwSellerChaDbNum = sItem.dwSellerChaDbNum;
					NetMsgHF.dwBuyerChaDbNum = sItem.dwBuyerChaDbNum;
					NetMsgHF.SetSellerName( pPacket->szSellerName );
					NetMsgHF.SetBuyerName( pPacket->szBuyerName );
					NetMsgHF.AttachItem = pPacket->AttachItem;
					
					/*if ( sItem.nSellerServerGroup != sItem.nBuyerServerGroup )
					{
						NetMsgHF.AttachItem.SetDbState( db::DB_INSERT, true );
					}*/
					
					// 개인상점검색에서는 실제로 아이템을 삭제시키고, 신규로 삽입하는 방식으로 변경한다.
					// 서버간 거래가 가능하기 때문이다. 겹침 아이템은 어차피 다르게 처리되기 때문에 이렇게 한다.
					NetMsgHF.AttachItem.SetDbState( db::DB_INSERT, true );

					if ( NET_ERROR == SendClient( sItem.dwBuyerFieldSlot, &NetMsgHF ) )
					{
						sc::writeLogError( "MsgPrivateMarketItemBuyResultFH SendClient error." );
					}
				}
			}
			else
			{
				sc::writeLogError( "MsgPrivateMarketItemBuyResultFH GetHoldItem error." );
			}
		}

		// hold 해제
		m_pPrivateMarketSearch->DeleteHoldItem( pPacket->sItemGUID );

		if ( pPacket->bUpdateSaleItem )
		{
			if ( 0 == pPacket->dwItemTurnNum )
			{
				m_pPrivateMarketSearch->DeleteItem( pPacket->sItemGUID );
			}
			else
			{
				m_pPrivateMarketSearch->UpdateItem( pPacket->sItemGUID, pPacket->dwItemTurnNum );
			}
		}
	}
}

void CacheServer::MsgPrivateMarketItemIsHoldFH( DWORD dwClient, NET_MSG_GENERIC* pMsg )
{
	if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
	{
		sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
		return;
	}

	GLMSG::SNETPC_PMARKET_ITEM_IS_HOLD_FH* pPacket = ( GLMSG::SNETPC_PMARKET_ITEM_IS_HOLD_FH* ) pMsg;

	if ( m_pPrivateMarketSearch )
	{
		GLMSG::SNETPC_PMARKET_ITEM_IS_HOLD_HF NetMsgHF;
		NetMsgHF.nResult = m_pPrivateMarketSearch->InsertHoldItem( pPacket->sItemGUID, dwClient, pPacket->dwReqChaDbNum, pPacket->szName, pPacket->nServerGroup );
		NetMsgHF.dwGaeaID = pPacket->dwGaeaID;
		NetMsgHF.sSALEPOS = pPacket->sSALEPOS;
		NetMsgHF.dwNum = pPacket->dwNum;
		NetMsgHF.dwReqChaDbNum = pPacket->dwReqChaDbNum;
		NetMsgHF.sItemGUID = pPacket->sItemGUID;
		SendClient( dwClient, &NetMsgHF );
	}
}

void CacheServer::MsgPrivateMarketLogFH( DWORD dwClient, NET_MSG_GENERIC* pMsg )
{
	if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
	{
		sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
		return;
	}

	GLMSG::SNETPC_PMARKET_LOG_FH* pPacket = ( GLMSG::SNETPC_PMARKET_LOG_FH* ) pMsg;

	if ( m_pPrivateMarketSearch )
	{
		std::vector< private_market::sSaleLog > vecSaleLog;
		int nResult = m_pPrivateMarketSearch->GetSaleLog( pPacket->dwChaDbNum, vecSaleLog, pPacket->nServerGroup );
		
		WORD wSize = static_cast< WORD >( vecSaleLog.size() );

		// agent 로 결과 전송
		GLMSG::SNETPC_PMARKET_LOG_NUM_HFC NetMsgLogNumHF;
		NetMsgLogNumHF.dwChaDbNum = pPacket->dwChaDbNum;
		NetMsgLogNumHF.wLogNum = wSize;
		SendClient( dwClient, &NetMsgLogNumHF );

		for ( WORD loop = 0; loop < wSize; ++loop )
		{
			GLMSG::SNETPC_PMARKET_LOG_HFC NetMsgLogHF( pPacket->dwChaDbNum, vecSaleLog[loop] );
			msgpack::sbuffer SendBuffer;
			msgpack::pack( SendBuffer, NetMsgLogHF );
			SendClient( dwClient, NET_MSG_GCTRL_PMARKET_LOG_HFC, SendBuffer );
		}

		GLMSG::SNETPC_PMARKET_LOG_END_HFC NetMsgLogEndHF;
		NetMsgLogEndHF.dwChaDbNum = pPacket->dwChaDbNum;
		SendClient( dwClient, &NetMsgLogEndHF );
	}
}

void CacheServer::MsgPrivateMarketStateFH( DWORD dwClient, NET_MSG_GENERIC* pMsg )
{
	if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
	{
		sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
		return;
	}

	GLMSG::SNETPC_PMARKET_STATE_FH* pPacket = ( GLMSG::SNETPC_PMARKET_STATE_FH* ) pMsg;

	if ( m_pPrivateMarketSearch )
	{
		GLMSG::SNETPC_PMARKET_STATE_HFC NetMsgHFC;

		NetMsgHFC.dwChaDbNum = pPacket->dwChaDbNum;

		m_pPrivateMarketSearch->GetBasicOption( 
			NetMsgHFC.bCheckChannel, 
			NetMsgHFC.bCheckSchool, 
			NetMsgHFC.bAllPage, 
			NetMsgHFC.wSearchItemNumberPerPage, 
			NetMsgHFC.wLogNumberPerPage, 
			NetMsgHFC.wLogNumberMaxPerCharacter, 
			NetMsgHFC.nSearchOption );

		SendClient( dwClient, &NetMsgHFC );
	}	
}

void CacheServer::MsgPrivateMarketItemSearchNameAH( DWORD dwClient, NET_MSG_GENERIC* pMsg )
{
	if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
	{
		sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
		return;
	}

	GLMSG::SNETPC_PMARKET_ITEM_SEARCH_NAME_AH* pPacket = ( GLMSG::SNETPC_PMARKET_ITEM_SEARCH_NAME_AH* ) pMsg;

	if ( m_pPrivateMarketSearch )
	{
		WORD wItemTotalNum = 0;
		std::string strItemName( pPacket->szItemName );
		std::vector< private_market::sSaleItem > vecSaleItems;
		int nResult = m_pPrivateMarketSearch->GetSaleItems( 
			pPacket->wPage, 
			pPacket->nChannel, 
			pPacket->wSchool,
			strItemName, 
			pPacket->nSortOption, 
			pPacket->nSortMethod, 
			vecSaleItems, 
			wItemTotalNum, 
			pPacket->nServerGroup );

		size_t nSize = vecSaleItems.size();

		if ( 1 == pPacket->wPage )
		{
			GLMSG::SNETPC_PMARKET_ITEM_NUM_TOTAL_HFC NetMsgItemNumTotalHA;
			NetMsgItemNumTotalHA.dwChaDbNum = pPacket->dwChaDbNum;
			NetMsgItemNumTotalHA.wItemTotalNum = wItemTotalNum;
			SendClient( dwClient, &NetMsgItemNumTotalHA );
		}

		// agent 로 결과 전송
		GLMSG::SNETPC_PMARKET_ITEM_NUM_HFC NetMsgItemNumHF;
		NetMsgItemNumHF.dwChaDbNum = pPacket->dwChaDbNum;
		NetMsgItemNumHF.wItemNum = static_cast< WORD >( nSize );
		SendClient( dwClient, &NetMsgItemNumHF );

		for ( size_t loop = 0; loop < nSize; ++loop )
		{
			GLMSG::SNETPC_PMARKET_ITEM_HFC NetMsgItemHF( pPacket->dwChaDbNum, vecSaleItems[loop] );
			SendClient( dwClient, &NetMsgItemHF );
		}

		GLMSG::SNETPC_PMARKET_ITEM_END_HFC NetMsgItemEndHF;
		NetMsgItemEndHF.dwChaDbNum = pPacket->dwChaDbNum;
		SendClient( dwClient, &NetMsgItemEndHF );
	}
}

void CacheServer::MsgPrivateMarketOptionFH( DWORD dwClient, NET_MSG_GENERIC* pMsg )
{
	if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
	{
		sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
		return;
	}

	GLMSG::SNETPC_PMARKET_OPTION_FH* pPacket = ( GLMSG::SNETPC_PMARKET_OPTION_FH* ) pMsg;

	if ( NULL == m_pPrivateMarketSearch )
	{
		return;
	}

	switch ( pPacket->eOptionType )
	{
	case private_market::eCommandOptionTypeCheckChannel:
		{
			private_market::GLPrivateMarketDefine::Instance()->SetCheckChannel( pPacket->bValue );
			m_pPrivateMarketSearch->UpdateChannelOption( pPacket->bValue );
		}
		break;
	case private_market::eCommandOptionTypeCheckSchool:
		{
			private_market::GLPrivateMarketDefine::Instance()->SetCheckSchool( pPacket->bValue );
			m_pPrivateMarketSearch->UpdateSchoolOption( pPacket->bValue );
		}
		break;
	case private_market::eCommandOptionTypeAllPage:
		{
			private_market::GLPrivateMarketDefine::Instance()->SetAllPage( pPacket->bValue );
			m_pPrivateMarketSearch->UpdatePageOption( pPacket->bValue );
		}
		break;
	case private_market::eCommandOptionTypeSearchOption:
		{
			private_market::GLPrivateMarketDefine::Instance()->SetSearchOption( pPacket->SearchOption );
			m_pPrivateMarketSearch->UpdateSearchOption( pPacket->SearchOption );
		}
		break;
	}
}

void CacheServer::MsgPrivateMarketCloseFH( DWORD dwClient, NET_MSG_GENERIC* pMsg )
{
	if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
	{
		sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
		return;
	}

	GLMSG::SNETPC_PMARKET_CLOSE_FH* pPacket = ( GLMSG::SNETPC_PMARKET_CLOSE_FH* ) pMsg;

	if ( NULL == m_pPrivateMarketSearch )
	{
		return;
	}

	if ( private_market::eSuccess != m_pPrivateMarketSearch->DeleteItem( pPacket->dwChaDbNum, pPacket->nServerGroup ) )
	{
		// error
	}
}