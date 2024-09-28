#include "pch.h"

#include "./GLLottoSystemManField.h"
#include "./GLLottoGlobalData.h"

#include "../FieldServer/GLGaeaServer.h"

#include "./GLLottoGlobalData.h"

#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/GLUseFeatures.h"

#include "../../RanLogic/PointShop/PointShopDefine.h"
#include "../../RanLogic/Item/GLITEMLMT.h"

#include "../../SigmaCore/Util/SystemInfo.h"

#include "../../enginelib/Common/SUBPATH.h"
#include "../../enginelib/GUInterface/UITextControl.h"
#include "../../enginelib/GUInterface/GameTextControl.h"

namespace LottoSystem
{

	GLLottoSystemManField::GLLottoSystemManField()
		: m_pGaeaServer( NULL ) { }
	GLLottoSystemManField::~GLLottoSystemManField() { }

	void GLLottoSystemManField::Initialize()
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return;

		// Script 파일의 주소;
		SUBPATH* pPath = GLogicData::GetInstance().GetPath();

		if( !pPath )
			return;

		std::string strScriptPath( sc::getAppPath() );
		strScriptPath += pPath->GLogicServerScriptsPath();
		strScriptPath += GLogicData::GetInstance().GetLottoSystemFileName();

		// Load Script;
		GLLottoGlobalData::Instance()->LoadScriptFile( strScriptPath );
	}

	void GLLottoSystemManField::Destroy()
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return;
	}

	bool GLLottoSystemManField::Update( float fElapsedTime, const CTime& currentTime )
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return true;

		return true;
	}

	bool GLLottoSystemManField::OnMessage( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID )
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return true;

		/// Field Server Message Procedure;
		switch( pMsg->Type() )
		{
		case NET_MSG_GCTRL_LOTTO_SYSTEM_REQ_LOTTO_TO_BUY_CF:
			SendReqLottoToBuyFA( pMsg, dwClientID, dwGaeaID );
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_CONFIRM_BUY_LIST_CF:
			SendConfirmBuyListFA( pMsg, dwClientID, dwGaeaID );
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_REQ_LOTTO_TO_BUY_AF:
			SendReqLottoToBuyFC( pMsg, dwClientID, dwGaeaID );
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_POST_WIN_MONEY_AF:
			SendPostWinMoneyFC( pMsg, dwClientID, dwGaeaID );
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_WINNING_MAN_LIST_AC:
			SendWinManListFC( pMsg, dwClientID, dwGaeaID );
			break;
		}

		return true;
	}

	void GLLottoSystemManField::SendToClientFromField( DWORD dwClientID, GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg )
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return;

		if( !m_pGaeaServer )
			return;

		m_pGaeaServer->SENDTOCLIENT( dwClientID, pMsg );
	}

	void GLLottoSystemManField::SendToAgentFromField( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg )
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return;

		if( !m_pGaeaServer )
			return;

		m_pGaeaServer->SENDTOAGENT( pMsg );
	}

	void GLLottoSystemManField::BroadcastFromField( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg )
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return;

		if( !m_pGaeaServer )
			return;

		m_pGaeaServer->SENDTOALLCLIENT( pMsg );
	}

	// Msg Procedure;
	void GLLottoSystemManField::SendReqLottoToBuyFA( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID )
	{
		GLMSG::NET_LOTTO_MESSAGE_REQ_LOTTO_TO_BUY_CF* pLottoMsg =
			static_cast<GLMSG::NET_LOTTO_MESSAGE_REQ_LOTTO_TO_BUY_CF*>( pMsg );

		LOTTO_SYSTEM_ID id = pLottoMsg->m_lottoSystemId;
		DWORD dwDbNum = pLottoMsg->m_dwDbNum;

		LOTTO_SYSTEM_VALUE sSystemValue =
			GLLottoGlobalData::Instance()->GetLottoSystemValue( id );

		GLMSG::NET_LOTTO_MESSAGE_REQ_LOTTO_TO_BUY_FC sLottoMsgFC( id );

		// 캐릭터의 구매 제한 확인;
		GLChar* pChar = m_pGaeaServer->GetCharByDbNum( dwDbNum );
		if( pChar )
		{
			/// 로또 구매가격이 0보다 작거나 같으면 실패시킨다;
			if ( sSystemValue.nLottoTicketMoney <= 0 )
			{
				sLottoMsgFC.m_emAC = EMLOTTO_REQ_LOTTO_TO_BUY_AC_FAIL;
				SendToClientFromField( pChar->ClientSlot(), &sLottoMsgFC );

				return;
			}

			/// 레벨을 체크한다;
			unsigned int nLevel = pChar->GetLevel();
			if ( nLevel < sSystemValue.nBuyLevel )
			{
				sLottoMsgFC.m_emAC = EMLOTTO_REQ_LOTTO_TO_BUY_AC_LEVEL_FAIL;
				SendToClientFromField( pChar->ClientSlot(), &sLottoMsgFC );

				return;
			}

			/// 캐릭터 돈을 체크한다;
			/// 돈 체크는 꼭 마지막에 하자;
			/// ( 실제로 돈을 제거하기 때문에 구매가 지속되지 않으면 돈이 증발될 수 있다; )
			switch( sSystemValue.emMoneyType )
			{
			case LOTTO_MONEY_TYPE_GAME_MONEY:
				{
					// 게임 머니를 체크한다;
					LONGLONG llInvenMoney = pChar->GetInvenMoney();
					if ( llInvenMoney < sSystemValue.nLottoTicketMoney ||
						llInvenMoney < sSystemValue.llBuyInvenMoney )
					{
						sLottoMsgFC.m_emAC = EMLOTTO_REQ_LOTTO_TO_BUY_AC_MONEY_FAIL;
						SendToClientFromField( pChar->ClientSlot(), &sLottoMsgFC );

						return;
					}

					// 게임 머니를 제거한다;
					pChar->SubtractInvenMoneyServer(
						static_cast<LONGLONG>(
						sSystemValue.nLottoTicketMoney ), SUBTRACT_INVEN_MONEY_LOTTO_BUY );

					// 게임 머니 제거 로그;
					LONGLONG llLottoTicketMoney = sSystemValue.nLottoTicketMoney;
					pChar->LogMoneyExchange(
						gamelog::ID_CHAR,
						pChar->CharDbNum(),
						gamelog::ID_CHAR, 
						pChar->CharDbNum(),
						-llLottoTicketMoney,
						MONEY_ROUTE_BUY_LOTTOSYSTEM_TICKET,
						( NULL == pChar ) ? MapID() : pChar->GetCurrentMap(),
						LOG_MONEY_LOTTOSYSTEM_TICKET_BUY );

					// 인벤의 전체 게임 머니 로그;
					pChar->LogMoneyExchange(
						gamelog::ID_CHAR,
						pChar->CharDbNum(),
						gamelog::ID_CHAR, 
						pChar->CharDbNum(),
						pChar->GetInvenMoney(),
						MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY,
						( NULL == pChar ) ? MapID() : pChar->GetCurrentMap(),
						LOG_MONEY_CHAR_INVEN_CURRENT );
				}
				break;

			case LOTTO_MONEY_TYPE_POINT:
				{
					// 포인트를 체크한다;
					LONGLONG llPoint = pChar->PointShopPoint();
					if ( llPoint < sSystemValue.nLottoTicketMoney ||
						llPoint < sSystemValue.llBuyInvenMoney )
					{
						sLottoMsgFC.m_emAC = EMLOTTO_REQ_LOTTO_TO_BUY_AC_MONEY_FAIL;
						SendToClientFromField( pChar->ClientSlot(), &sLottoMsgFC );

						return;
					}

					// 포인트를 제거한다;
					pChar->PointShopPointSub(
						sSystemValue.nLottoTicketMoney,
						pointshop::LOTTO_SYSTEM,
						pointshop::POINT_SUB_TYPE_LOTTO );						
				}
				break;
			}
			
			/// Agent로 메시지를 전달한다;
			LONGLONG llInvenMoney = pChar->GetInvenMoney();
			LONGLONG llPoint = pChar->PointShopPoint();
			GLMSG::NET_LOTTO_MESSAGE_REQ_LOTTO_TO_BUY_FA sLottoMsgFA(
				pLottoMsg->m_lottoSystemId,
				dwDbNum,
				llInvenMoney,
				llPoint,
				nLevel );
			sLottoMsgFA.m_lottoBuy = pLottoMsg->m_lottoBuy;
			sLottoMsgFA.m_lottoBuy.dwUserNum = pChar->GetUserID();

			SendToAgentFromField( &sLottoMsgFA );
		}	
	}

	void GLLottoSystemManField::SendConfirmBuyListFA( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID )
	{
		GLMSG::NET_LOTTO_MESSAGE_CONFIRM_BUY_LIST_CF* pLottoMsg =
			static_cast<GLMSG::NET_LOTTO_MESSAGE_CONFIRM_BUY_LIST_CF*>( pMsg );

		LOTTO_SYSTEM_ID id = pLottoMsg->m_lottoSystemId;
		DWORD dwDbNum = pLottoMsg->m_dwDbNum;

		// 캐릭터의 구매 제한 확인;
		GLChar* pChar = m_pGaeaServer->GetCharByDbNum( dwDbNum );
		if( pChar )
		{
			LONGLONG invenMoney = pChar->GetInvenMoney();
			LONGLONG point = pChar->PointShopPoint();

			// Agent로 전달 패킷;
			GLMSG::NET_LOTTO_MESSAGE_CONFIRM_BUY_LIST_FA lottoMsgFA(
				pLottoMsg->m_lottoSystemId,
				dwDbNum,
				invenMoney,
				point );
			SendToAgentFromField( &lottoMsgFA );
		}	
	}

	void GLLottoSystemManField::SendReqLottoToBuyFC( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID )
	{
		GLMSG::NET_LOTTO_MESSAGE_REQ_LOTTO_TO_BUY_AF* pLottoMsg =
			static_cast<GLMSG::NET_LOTTO_MESSAGE_REQ_LOTTO_TO_BUY_AF*>( pMsg );

		LOTTO_SYSTEM_ID id = pLottoMsg->m_lottoSystemId;
		DWORD dwDbNum = pLottoMsg->m_dwDbNum;

		LOTTO_SYSTEM_VALUE sSystemValue =
			GLLottoGlobalData::Instance()->GetLottoSystemValue( id );

		GLChar* pChar = m_pGaeaServer->GetCharByDbNum( dwDbNum );
		if( !pChar )
			return;

		switch( pLottoMsg->m_emAC )
		{
		case LottoSystem::EMLOTTO_REQ_LOTTO_TO_BUY_AC_OK:
			pChar->SendInvenMoneyUpdate();
			break;

		case LottoSystem::EMLOTTO_REQ_LOTTO_TO_BUY_AC_FAIL:
		case LottoSystem::EMLOTTO_REQ_LOTTO_TO_BUY_AC_STATE_ERROR:
		case LottoSystem::EMLOTTO_REQ_LOTTO_TO_BUY_AC_LEVEL_FAIL:
		case LottoSystem::EMLOTTO_REQ_LOTTO_TO_BUY_AC_MONEY_FAIL:
			// 실패할 경우 돈을 복구시킨다;
			switch( sSystemValue.emMoneyType )
			{
			case LOTTO_MONEY_TYPE_GAME_MONEY:
				{
					// 게임 머니를 추가한다;
					pChar->AddInvenMoneyServer(
						static_cast<LONGLONG>(
						sSystemValue.nLottoTicketMoney ), ADD_INVEN_MONEY_LOTTO_BUY_RESTORE );
					pChar->SendInvenMoneyUpdate();

					// 게임 머니 복구 로그;
					pChar->LogMoneyExchange(
						gamelog::ID_CHAR,
						pChar->CharDbNum(),
						gamelog::ID_CHAR, 
						pChar->CharDbNum(),
						sSystemValue.nLottoTicketMoney,
						MONEY_ROUTE_RESTORE_LOTTOSYSTEM_TICKET,
						( NULL == pChar ) ? MapID() : pChar->GetCurrentMap(),
						LOG_MONEY_LOTTOSYSTEM_TICKET_RESTORE );

					// 인벤의 전체 게임 머니 로그;
					pChar->LogMoneyExchange(
						gamelog::ID_CHAR,
						pChar->CharDbNum(),
						gamelog::ID_CHAR, 
						pChar->CharDbNum(),
						pChar->GetInvenMoney(),
						MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY,
						( NULL == pChar ) ? MapID() : pChar->GetCurrentMap(),
						LOG_MONEY_CHAR_INVEN_CURRENT );
				}
				break;

			case LOTTO_MONEY_TYPE_POINT:
				{
					// 포인트를 추가한다;
					pChar->PointShopPointAdd(
						0,
						sSystemValue.nLottoTicketMoney,
						pointshop::LOTTO_SYSTEM );						
				}
				break;
			}

			break;
		}

		// 응답 패킷;
		GLMSG::NET_LOTTO_MESSAGE_REQ_LOTTO_TO_BUY_FC lottoMsgFC( id );
		lottoMsgFC.m_emAC = pLottoMsg->m_emAC;
		lottoMsgFC.m_lottoBuy = pLottoMsg->m_lottoBuy;

		// dwClientID로 보내면 제대로 가지 않는다;
		SendToClientFromField( pChar->ClientSlot(), &lottoMsgFC );
	}

	void GLLottoSystemManField::SendPostWinMoneyFC( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID )
	{
		GLMSG::NET_LOTTO_MESSAGE_REQ_POST_WIN_MONEY_AF* pLottoMsg =
			static_cast<GLMSG::NET_LOTTO_MESSAGE_REQ_POST_WIN_MONEY_AF*>( pMsg );

		DWORD dwDbNum = pLottoMsg->m_dwDbNum;
		LONGLONG winMoney = pLottoMsg->m_winMoney;

		std::string strRecverName;
		GLChar* pChar = m_pGaeaServer->GetCharByDbNum( dwDbNum );
		if ( pChar )
		{
			strRecverName = pChar->GetName();

			std::string strWinMoney;
			switch( pLottoMsg->m_moneyType )
			{
			case LOTTO_MONEY_TYPE_GAME_MONEY:
				if( pLottoMsg->m_winMoney > 0 )
				{
					// 게임머니 추가;
					pChar->AddInvenMoneyServer( winMoney, ADD_INVEN_MONEY_LOTTO_WIN );
					pChar->SendInvenMoneyUpdate();
					strWinMoney = sc::string::format( "%d",
						NS_UITEXTCONTROL::MAKE_MONEY_FORMAT(
						winMoney, 3, "," ) );

					// 당첨 게임 머니 지급 로그;
					pChar->LogMoneyExchange(
						gamelog::ID_CHAR,
						pChar->CharDbNum(),
						gamelog::ID_CHAR, 
						pChar->CharDbNum(),
						winMoney,
						MONEY_ROUTE_WIN_LOTTOSYSTEM,
						( NULL == pChar ) ? MapID() : pChar->GetCurrentMap(),
						LOG_MONEY_LOTTOSYSTEM_WIN );

					// 인벤의 전체 게임 머니 로그;
					pChar->LogMoneyExchange(
						gamelog::ID_CHAR,
						pChar->CharDbNum(),
						gamelog::ID_CHAR, 
						pChar->CharDbNum(),
						pChar->GetInvenMoney(),
						MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY,
						( NULL == pChar ) ? MapID() : pChar->GetCurrentMap(),
						LOG_MONEY_CHAR_INVEN_CURRENT );
				}
				break;
			case LOTTO_MONEY_TYPE_POINT:
				// 란 포인트 추가;
				if( pLottoMsg->m_winMoney > 0 )
				{
					// 지급 로그;


					pChar->PointShopPointAdd( winMoney, 0, pointshop::LOTTO_SYSTEM );

					strWinMoney = sc::string::format( "%d Point",
						NS_UITEXTCONTROL::MAKE_MONEY_FORMAT(
						winMoney, 3, "," ) );
				}
				break;
			}

			// 아이템이 있다면;
			CItemDrop sDropItem;
			if ( pLottoMsg->m_sWinItem.dwID != SNATIVEID( false ).dwID )
			{
				const SITEM* pItem = GLogicData::GetInstance().GetItem( pLottoMsg->m_sWinItem );
				if ( pItem )
				{
					SITEMCUSTOM sITEM_NEW( pLottoMsg->m_sWinItem );
					if ( pItem->ISPILE() )
					{
						sITEM_NEW.tBORNTIME = CTime::GetCurrentTime().GetTime();
						sITEM_NEW.wTurnNum = pLottoMsg->m_dwWinItemCount;
						sITEM_NEW.cGenType = EMGEN_ITEMMIX;
						// sITEM_NEW.cChnID = (BYTE)pChar->GetServerChannel();
						//sITEM_NEW.cFieldID = (BYTE)pChar->GetFieldSvrID();
						sc::SeqUniqueGUID::Instance()->getGUID( sITEM_NEW.guid );
					}
					else
					{
						sITEM_NEW.tBORNTIME = CTime::GetCurrentTime().GetTime();
						sITEM_NEW.cGenType = EMGEN_ITEMMIX;
						// sITEM_NEW.cChnID = (BYTE)pChar->GetServerChannel();
						//sITEM_NEW.cFieldID = (BYTE)pChar->GetFieldSvrID();
						sc::SeqUniqueGUID::Instance()->getGUID( sITEM_NEW.guid );
					}

					sITEM_NEW.GenerateBasicStat( false );
					sITEM_NEW.GenerateLinkSkill();
					sITEM_NEW.GenerateAddOption();

					sDropItem.sItemCustom = sITEM_NEW;
				}
			}
			

			// 확인 Post 날리기;
			BOOL bResult = m_pGaeaServer->PostSendSystemMail(
				dwDbNum, 
				strRecverName, 
				&sDropItem,
				0,
				ATTENDANCE_REWARD_POST_DESIGN, 
				ATTENDANCE_REWARD_POST_SENDER,
				std::string( ID2SERVERTEXT( "LOTTOSYSTEM_GIVE_MONEY_POST_SENDERNAME" ) ),
				sc::string::format( ID2SERVERTEXT( "LOTTOSYSTEM_GIVE_MONEY_POST_TITLE" ), pLottoMsg->m_turnNum+1, pLottoMsg->m_ranking ),
				sc::string::format(
				ID2SERVERTEXT( "LOTTOSYSTEM_GIVE_MONEY_POST_CONTENT" ),
				pLottoMsg->m_turnNum+1,
				pLottoMsg->m_ranking,
				strWinMoney ),
				false );

			// 클라이언트에 전달해서 시스템 메시지 출력하기;
			// 응답 패킷;
			GLMSG::NET_LOTTO_MESSAGE_REQ_POST_WIN_MONEY_FC lottoMsgFC(
				pLottoMsg->m_lottoSystemId,
				pLottoMsg->m_dwDbNum,
				pLottoMsg->m_turnNum,
				pLottoMsg->m_winMoney,
				pLottoMsg->m_ranking );
			lottoMsgFC.m_moneyType = pLottoMsg->m_moneyType;

			// dwClientID로 보내면 제대로 가지 않는다;
			SendToClientFromField( pChar->ClientSlot(), &lottoMsgFC );
		}
	}

	void GLLottoSystemManField::SendWinManListFC( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID )
	{
		GLMSG::NET_LOTTO_MESSAGE_WINNING_MAN_LIST_AC* pLottoMsg =
			static_cast<GLMSG::NET_LOTTO_MESSAGE_WINNING_MAN_LIST_AC*>( pMsg );
		if ( NULL == pLottoMsg )
			return;

		// 요청한 캐릭터의 정보를 가져온다;
		GLChar* pChar = m_pGaeaServer->GetCharByDbNum( pLottoMsg->dwChaNum );
		if ( NULL == pChar )
			return;

		SendToClientFromField( pChar->ClientSlot(), pLottoMsg );
	}

}