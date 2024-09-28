#include "pch.h"

#include "../../../RanLogic/Msg/CharacterMsg.h"
#include "../../../RanLogic/Msg/ServerMsg.h"
#include "../../../RanLogic/GLogicEx.h"
#include "../../../RanLogic/GLUseFeatures.h"
#include "../../../RanLogic/Inventory/GLInventoryStorage.h"

#include "../../Server/AgentServer.h"
#include "../../Server/s_CFieldServer.h"

#include "../DbManager.h"
#include "DbActionGameItem.h"

#include "../../../SigmaCore/DebugInclude.h"

namespace db
{
	ItemInsert::ItemInsert( DWORD ChaDbNum, INVEN_TYPE InvenType, SINVENITEM_SAVE& Item, int CallType, DWORD Channel /*= 0*/ )
		: m_ChaDbNum( ChaDbNum )
		, m_InvenType( InvenType )
		, m_Item( Item )
		, m_CallType( CallType )
		, m_Channel( Channel )
	{
	}

	int ItemInsert::Execute( NetServer* pServer )
	{
		if ( db::ADO != m_pDbManager->GetConnectionType() )
		{
			sc::writeLogError( "ItemInsert::Execute. Use ADO" );
			return NET_ERROR;
		}

		{
			// 유저락커 아이템 좌표 처리, 나중에 채널이라는 칼럼을 디비에 추가하던지
			// 따로 함수를 만들던지 해야한다. 락커가 인벤토리 배열로 되어있어 디비에 저장할 때 x좌표값 수정해서 저장해야 한다.
			// 로직에서 좌표를 미리 처리해서 보낼 수도 있는데, 복사 비용도 있고, 로직에서는 다른 예외 처리없이 인벤타입처럼 디비액션 그냥 던지도록 하기 위해서
			// 여기에서 처리한다. 클럽창고도 인벤토리 배열로 되어있다면 이렇게 처리해야 한다. 아직 클럽 창고 작업 하지 않음
			if ( INVEN_LOCKER == m_InvenType )
			{
				if ( 1 == m_Channel )
				{
					m_Item.wPosX = m_Item.wPosX + EStorageX;
				}
				else if ( 2 == m_Channel )
				{
					m_Item.wPosX = m_Item.wPosX + ( EStorageX*2 );
				}
				else if ( 3 == m_Channel )
				{
					m_Item.wPosX = m_Item.wPosX + ( EStorageX*3 );
				}
				else if ( 4 == m_Channel )
				{
					m_Item.wPosX = m_Item.wPosX + ( EStorageX*4 );
				}
			}

			if ( INVEN_CLUB_LOCKER == m_InvenType )
			{
				if ( 1 == m_Channel )
				{
					m_Item.wPosX = m_Item.wPosX + EStorageX;
				}
				else if ( 2 == m_Channel )
				{
					m_Item.wPosX = m_Item.wPosX + ( EStorageX*2 );
				}
				else if ( 3 == m_Channel )
				{
					m_Item.wPosX = m_Item.wPosX + ( EStorageX*3 );
				}
				else if ( 4 == m_Channel )
				{
					m_Item.wPosX = m_Item.wPosX + ( EStorageX*4 );
				}
				else if ( 5 == m_Channel )
				{
					m_Item.wPosX = m_Item.wPosX + ( EStorageX*5 );
				}
			}
		}

		int nRetCode = m_pDbManager->ItemInsert( m_ChaDbNum, m_InvenType, &m_Item, m_CallType );
		if ( sc::db::DB_ERROR == nRetCode )
		{
			sc::writeLogError( sc::string::format( "DB:ItemInsert failed. %d CallType %d", m_ChaDbNum, m_CallType ) );
		}

		return sc::db::DB_OK;
	}


	ItemUpdate::ItemUpdate( DWORD ChaDbNum, INVEN_TYPE InvenType, SINVENITEM_SAVE& Item, int CallType, DWORD Channel /*= 0*/ )
		: m_ChaDbNum( ChaDbNum )
		, m_InvenType( InvenType )
		, m_Item( Item )
		, m_CallType( CallType )
		, m_Channel( Channel )
	{
	}

	int ItemUpdate::Execute( NetServer* pServer )
	{
		if ( db::ADO != m_pDbManager->GetConnectionType() )
		{
			sc::writeLogError( "ItemUpdate::Execute. Use ADO" );
			return NET_ERROR;
		}

		{
			// 유저락커 아이템 좌표 처리, 나중에 채널이라는 칼럼을 디비에 추가하던지
			// 따로 함수를 만들던지 해야한다. 락커가 인벤토리 배열로 되어있어 디비에 저장할 때 x좌표값 수정해서 저장해야 한다.
			// 로직에서 좌표를 미리 처리해서 보낼 수도 있는데, 복사 비용도 있고, 로직에서는 다른 예외 처리없이 인벤타입처럼 디비액션 그냥 던지도록 하기 위해서
			// 여기에서 처리한다. 클럽창고도 인벤토리 배열로 되어있다면 이렇게 처리해야 한다. 아직 클럽 창고 작업 하지 않음
			if ( INVEN_LOCKER == m_InvenType )
			{
				if ( 1 == m_Channel )
				{
					m_Item.wPosX = m_Item.wPosX + EStorageX;
				}
				else if ( 2 == m_Channel )
				{
					m_Item.wPosX = m_Item.wPosX + ( EStorageX*2 );
				}
				else if ( 3 == m_Channel )
				{
					m_Item.wPosX = m_Item.wPosX + ( EStorageX*3 );
				}
				else if ( 4 == m_Channel )
				{
					m_Item.wPosX = m_Item.wPosX + ( EStorageX*4 );
				}
			}

			if ( INVEN_CLUB_LOCKER == m_InvenType )
			{
				if ( 1 == m_Channel )
				{
					m_Item.wPosX = m_Item.wPosX + EStorageX;
				}
				else if ( 2 == m_Channel )
				{
					m_Item.wPosX = m_Item.wPosX + ( EStorageX*2 );
				}
				else if ( 3 == m_Channel )
				{
					m_Item.wPosX = m_Item.wPosX + ( EStorageX*3 );
				}
				else if ( 4 == m_Channel )
				{
					m_Item.wPosX = m_Item.wPosX + ( EStorageX*4 );
				}
				else if ( 5 == m_Channel )
				{
					m_Item.wPosX = m_Item.wPosX + ( EStorageX*5 );
				}
			}
		}

		int nRetCode = m_pDbManager->ItemUpdate( m_ChaDbNum, m_InvenType, &m_Item, m_CallType );
		if ( sc::db::DB_ERROR == nRetCode )
		{
			sc::writeLogError( sc::string::format( "DB:ItemUpdate failed. %d CallType %d", m_ChaDbNum, m_CallType ) );
		}

		return sc::db::DB_OK;
	}

	EntireItemUpdate::EntireItemUpdate( DWORD ChaDbNum, INVEN_TYPE InvenType, SINVENITEM_SAVE& Item, int CallType, DWORD Channel /*= 0*/ )
		: m_ChaDbNum( ChaDbNum )
		, m_InvenType( InvenType )
		, m_Item( Item )
		, m_CallType( CallType )
		, m_Channel( Channel )
	{
	}

	int EntireItemUpdate::Execute( NetServer* pServer )
	{
		if ( db::ADO != m_pDbManager->GetConnectionType() )
		{
			sc::writeLogError( "EntireItemUpdate::Execute. Use ADO" );
			return NET_ERROR;
		}

		{
			// 유저락커 아이템 좌표 처리, 나중에 채널이라는 칼럼을 디비에 추가하던지
			// 따로 함수를 만들던지 해야한다. 락커가 인벤토리 배열로 되어있어 디비에 저장할 때 x좌표값 수정해서 저장해야 한다.
			// 로직에서 좌표를 미리 처리해서 보낼 수도 있는데, 복사 비용도 있고, 로직에서는 다른 예외 처리없이 인벤타입처럼 디비액션 그냥 던지도록 하기 위해서
			// 여기에서 처리한다. 클럽창고도 인벤토리 배열로 되어있다면 이렇게 처리해야 한다. 아직 클럽 창고 작업 하지 않음
			if ( INVEN_LOCKER == m_InvenType )
			{
				if ( 1 == m_Channel )
				{
					m_Item.wPosX = m_Item.wPosX + EStorageX;
				}
				else if ( 2 == m_Channel )
				{
					m_Item.wPosX = m_Item.wPosX + ( EStorageX*2 );
				}
				else if ( 3 == m_Channel )
				{
					m_Item.wPosX = m_Item.wPosX + ( EStorageX*3 );
				}
				else if ( 4 == m_Channel )
				{
					m_Item.wPosX = m_Item.wPosX + ( EStorageX*4 );
				}
			}

			if ( INVEN_CLUB_LOCKER == m_InvenType )
			{
				if ( 1 == m_Channel )
				{
					m_Item.wPosX = m_Item.wPosX + EStorageX;
				}
				else if ( 2 == m_Channel )
				{
					m_Item.wPosX = m_Item.wPosX + ( EStorageX*2 );
				}
				else if ( 3 == m_Channel )
				{
					m_Item.wPosX = m_Item.wPosX + ( EStorageX*3 );
				}
				else if ( 4 == m_Channel )
				{
					m_Item.wPosX = m_Item.wPosX + ( EStorageX*4 );
				}
				else if ( 5 == m_Channel )
				{
					m_Item.wPosX = m_Item.wPosX + ( EStorageX*5 );
				}
			}
		}

		int nRetCode = m_pDbManager->EntireItemUpdate( m_ChaDbNum, m_InvenType, &m_Item, m_CallType );
		if ( sc::db::DB_ERROR == nRetCode )
		{
			sc::writeLogError( sc::string::format( "DB:EntireItemUpdate failed. %d CallType %d", m_ChaDbNum, m_CallType ) );
		}

		return sc::db::DB_OK;
	}

	ItemDelete::ItemDelete( sc::MGUID& Guid, int State, int CallType )
		: m_Guid( Guid )
		, m_State( State )
		, m_CallType( CallType )
	{
	}

	int ItemDelete::Execute( NetServer* pServer )
	{
		return m_pDbManager->ItemDelete( m_Guid, m_State, m_CallType );
	}

	ItemSaleToNpc::ItemSaleToNpc( sc::MGUID& Guid, int State, int CallType )
		: m_Guid( Guid )
		, m_State( State )
		, m_CallType( CallType )
	{
	}

	int ItemSaleToNpc::Execute( NetServer* pServer )
	{
		return m_pDbManager->ItemSaleToNpc( m_Guid, m_State, m_CallType );
	}

	ItemTransfer::ItemTransfer( DWORD UserNum, const std::vector< DWORD >& vecChaNum, const NET_CHA_BBA_INFO_HA* pmsg )
		: m_UserNum( UserNum )
	{
		m_vecChaNum = vecChaNum;
		if ( pmsg )
		{
			m_msg.m_AgentServerSlot = pmsg->m_AgentServerSlot;
			m_msg.m_ChaServerTotalNum = pmsg->m_ChaServerTotalNum;
			for ( size_t i = 0; i < MAX_ONESERVERCHAR_NUM; ++i )
			{			
				m_msg.SetChaNum( i, pmsg->m_ChaDbNum[i] );
			}
		}
	}

	int ItemTransfer::Execute( NetServer* pServer )
	{
		// 아이템 변환 진행중인지, 완료되었는지, 문제가 있는 확인한다.

		// 여기에 상황 체크, 결과를 처리 추가 필요
		if ( sc::db::DB_OK != m_pDbManager->TransferItems( m_UserNum, m_vecChaNum ) )
		{
			// error
		}

		/*CAgentServer* pAgentServer = reinterpret_cast< CAgentServer* >( pServer );
		if ( NULL == pAgentServer )
		{
			return NET_ERROR;
		}

		GLMSG::NET_FRIEND_STATE_REQ_DA MsgDA(m_MainChar, m_SubChar, m_ReqCharDbNum, FriendFlag);
		pAgentServer->InsertMsg(DBACTION_CLIENT_ID, &MsgDA);*/
		return sc::db::DB_OK;
	}


	ItemUpdatePos::ItemUpdatePos( DWORD ChaDbNum, INVEN_TYPE InvenType, sc::MGUID& Guid, WORD PosX, WORD PosY, int CallType, DWORD Channel /*= 0 */ )
		: m_ChaDbNum( ChaDbNum )
		, m_InvenType( InvenType )
		, m_Guid( Guid )
		, m_PosX( PosX )
		, m_PosY( PosY )
		, m_CallType( CallType )
		, m_Channel( Channel )
	{
	}

	int ItemUpdatePos::Execute( NetServer* pServer )
	{
		if ( db::ADO != m_pDbManager->GetConnectionType() )
		{
			sc::writeLogError( "ItemUpdatePos::Execute. Use ADO" );
			return NET_ERROR;
		}

		{
			// 유저락커 아이템 좌표 처리, 나중에 채널이라는 칼럼을 디비에 추가하던지
			// 따로 함수를 만들던지 해야한다. 락커가 인벤토리 배열로 되어있어 디비에 저장할 때 x좌표값 수정해서 저장해야 한다.
			// 로직에서 좌표를 미리 처리해서 보낼 수도 있는데, 복사 비용도 있고, 로직에서는 다른 예외 처리없이 인벤타입처럼 디비액션 그냥 던지도록 하기 위해서
			// 여기에서 처리한다. 클럽창고도 인벤토리 배열로 되어있다면 이렇게 처리해야 한다. 아직 클럽 창고 작업 하지 않음
			if ( INVEN_LOCKER == m_InvenType )
			{
				if ( 1 == m_Channel )
				{
					m_PosX = m_PosX + EStorageX;
				}
				else if ( 2 == m_Channel )
				{
					m_PosX = m_PosX + ( EStorageX*2 );
				}
				else if ( 3 == m_Channel )
				{
					m_PosX = m_PosX + ( EStorageX*3 );
				}
				else if ( 4 == m_Channel )
				{
					m_PosX = m_PosX + ( EStorageX*4 );
				}
			}

			if ( INVEN_CLUB_LOCKER == m_InvenType )
			{
				if ( 1 == m_Channel )
				{
					m_PosX = m_PosX + EStorageX;
				}
				else if ( 2 == m_Channel )
				{
					m_PosX = m_PosX + ( EStorageX*2 );
				}
				else if ( 3 == m_Channel )
				{
					m_PosX = m_PosX + ( EStorageX*3 );
				}
				else if ( 4 == m_Channel )
				{
					m_PosX = m_PosX + ( EStorageX*4 );
				}
				else if ( 5 == m_Channel )
				{
					m_PosX = m_PosX + ( EStorageX*5 );
				}
			}
		}

		int nRetCode = m_pDbManager->ItemUpdatePos( m_ChaDbNum, m_InvenType, m_Guid, m_PosX, m_PosY, m_CallType );
		if ( sc::db::DB_ERROR == nRetCode )
		{
			sc::writeLogError( sc::string::format( "DB:ItemUpdatePos failed. %d CallType %d", m_ChaDbNum, m_CallType ) );
		}

		return sc::db::DB_OK;
	}

	ItemUpdateTurnNum::ItemUpdateTurnNum( sc::MGUID& Guid, WORD TurnNum, int CallType )
		: m_Guid( Guid )
		, m_TurnNum( TurnNum )
		, m_CallType( CallType )
	{
	}

	int ItemUpdateTurnNum::Execute( NetServer* pServer )
	{
		int nRetCode = m_pDbManager->ItemUpdateTurnNum( m_Guid, m_TurnNum, m_CallType );
		if ( sc::db::DB_ERROR == nRetCode )
		{
			sc::writeLogError( sc::string::format( "DB:ItemUpdateTurnNum failed. %d CallType %d", m_TurnNum, m_CallType ) );
		}

		return sc::db::DB_OK;
	}


	ItemInsertError::ItemInsertError( sc::MGUID& Guid, int CallType, __time64_t& Date )
		: m_Guid( Guid )
		, m_CallType( CallType )
		, m_Date( Date )
	{
	}

	int ItemInsertError::Execute( NetServer* pServer )
	{
		if ( db::ADO != m_pDbManager->GetConnectionType() )
		{
			sc::writeLogError( "ItemInsertError::Execute. Use ADO" );
			return NET_ERROR;
		}

		int nRetCode = m_pDbManager->ItemInsertError( m_Guid, m_CallType, m_Date );
		if ( sc::db::DB_ERROR == nRetCode )
		{
			sc::writeLogError( sc::string::format( "DB:ItemInsertError failed. CallType %d", m_CallType ) );
		}

		return sc::db::DB_OK;
	}


	CompletelyItemDelete::CompletelyItemDelete( sc::MGUID& Guid, int State, int CallType )
		: m_Guid( Guid )
		, m_State( State )
		, m_CallType( CallType )
	{
	}

	int CompletelyItemDelete::Execute( NetServer* pServer )
	{
		return m_pDbManager->CompletelyItemDelete( m_Guid, m_State, m_CallType );
	}


	DeletedItemInsert::DeletedItemInsert( DWORD ChaDbNum, INVEN_TYPE InvenType, SINVENITEM_SAVE& Item, int CallType, DWORD Channel /*= 0 */ )
		: m_ChaDbNum( ChaDbNum )
		, m_InvenType( InvenType )
		, m_Item( Item )
		, m_CallType( CallType )
		, m_Channel( Channel )
	{
	}

	int DeletedItemInsert::Execute( NetServer* pServer )
	{
		if ( db::ADO != m_pDbManager->GetConnectionType() )
		{
			sc::writeLogError( "DeletedItemInsert::Execute. Use ADO" );
			return NET_ERROR;
		}

		{
			// 유저락커 아이템 좌표 처리, 나중에 채널이라는 칼럼을 디비에 추가하던지
			// 따로 함수를 만들던지 해야한다. 락커가 인벤토리 배열로 되어있어 디비에 저장할 때 x좌표값 수정해서 저장해야 한다.
			// 로직에서 좌표를 미리 처리해서 보낼 수도 있는데, 복사 비용도 있고, 로직에서는 다른 예외 처리없이 인벤타입처럼 디비액션 그냥 던지도록 하기 위해서
			// 여기에서 처리한다. 클럽창고도 인벤토리 배열로 되어있다면 이렇게 처리해야 한다. 아직 클럽 창고 작업 하지 않음
			if ( INVEN_LOCKER == m_InvenType )
			{
				if ( 1 == m_Channel )
				{
					m_Item.wPosX = m_Item.wPosX + EStorageX;
				}
				else if ( 2 == m_Channel )
				{
					m_Item.wPosX = m_Item.wPosX + ( EStorageX*2 );
				}
				else if ( 3 == m_Channel )
				{
					m_Item.wPosX = m_Item.wPosX + ( EStorageX*3 );
				}
				else if ( 4 == m_Channel )
				{
					m_Item.wPosX = m_Item.wPosX + ( EStorageX*4 );
				}
			}

			if ( INVEN_CLUB_LOCKER == m_InvenType )
			{
				if ( 1 == m_Channel )
				{
					m_Item.wPosX = m_Item.wPosX + EStorageX;
				}
				else if ( 2 == m_Channel )
				{
					m_Item.wPosX = m_Item.wPosX + ( EStorageX*2 );
				}
				else if ( 3 == m_Channel )
				{
					m_Item.wPosX = m_Item.wPosX + ( EStorageX*3 );
				}
				else if ( 4 == m_Channel )
				{
					m_Item.wPosX = m_Item.wPosX + ( EStorageX*4 );
				}
				else if ( 5 == m_Channel )
				{
					m_Item.wPosX = m_Item.wPosX + ( EStorageX*5 );
				}
			}
		}

		int nRetCode = m_pDbManager->DeletedItemInsert( m_ChaDbNum, m_InvenType, &m_Item, m_CallType );
		if ( sc::db::DB_ERROR == nRetCode )
		{
			sc::writeLogError( sc::string::format( "DB:DeletedItemInsert failed. %d CallType %d", m_ChaDbNum, m_CallType ) );
		}

		return sc::db::DB_OK;
	}

}