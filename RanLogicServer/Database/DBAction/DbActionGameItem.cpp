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
			// ������Ŀ ������ ��ǥ ó��, ���߿� ä���̶�� Į���� ��� �߰��ϴ���
			// ���� �Լ��� ������� �ؾ��Ѵ�. ��Ŀ�� �κ��丮 �迭�� �Ǿ��־� ��� ������ �� x��ǥ�� �����ؼ� �����ؾ� �Ѵ�.
			// �������� ��ǥ�� �̸� ó���ؼ� ���� ���� �ִµ�, ���� ��뵵 �ְ�, ���������� �ٸ� ���� ó������ �κ�Ÿ��ó�� ���׼� �׳� �������� �ϱ� ���ؼ�
			// ���⿡�� ó���Ѵ�. Ŭ��â�� �κ��丮 �迭�� �Ǿ��ִٸ� �̷��� ó���ؾ� �Ѵ�. ���� Ŭ�� â�� �۾� ���� ����
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
			// ������Ŀ ������ ��ǥ ó��, ���߿� ä���̶�� Į���� ��� �߰��ϴ���
			// ���� �Լ��� ������� �ؾ��Ѵ�. ��Ŀ�� �κ��丮 �迭�� �Ǿ��־� ��� ������ �� x��ǥ�� �����ؼ� �����ؾ� �Ѵ�.
			// �������� ��ǥ�� �̸� ó���ؼ� ���� ���� �ִµ�, ���� ��뵵 �ְ�, ���������� �ٸ� ���� ó������ �κ�Ÿ��ó�� ���׼� �׳� �������� �ϱ� ���ؼ�
			// ���⿡�� ó���Ѵ�. Ŭ��â�� �κ��丮 �迭�� �Ǿ��ִٸ� �̷��� ó���ؾ� �Ѵ�. ���� Ŭ�� â�� �۾� ���� ����
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
			// ������Ŀ ������ ��ǥ ó��, ���߿� ä���̶�� Į���� ��� �߰��ϴ���
			// ���� �Լ��� ������� �ؾ��Ѵ�. ��Ŀ�� �κ��丮 �迭�� �Ǿ��־� ��� ������ �� x��ǥ�� �����ؼ� �����ؾ� �Ѵ�.
			// �������� ��ǥ�� �̸� ó���ؼ� ���� ���� �ִµ�, ���� ��뵵 �ְ�, ���������� �ٸ� ���� ó������ �κ�Ÿ��ó�� ���׼� �׳� �������� �ϱ� ���ؼ�
			// ���⿡�� ó���Ѵ�. Ŭ��â�� �κ��丮 �迭�� �Ǿ��ִٸ� �̷��� ó���ؾ� �Ѵ�. ���� Ŭ�� â�� �۾� ���� ����
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
		// ������ ��ȯ ����������, �Ϸ�Ǿ�����, ������ �ִ� Ȯ���Ѵ�.

		// ���⿡ ��Ȳ üũ, ����� ó�� �߰� �ʿ�
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
			// ������Ŀ ������ ��ǥ ó��, ���߿� ä���̶�� Į���� ��� �߰��ϴ���
			// ���� �Լ��� ������� �ؾ��Ѵ�. ��Ŀ�� �κ��丮 �迭�� �Ǿ��־� ��� ������ �� x��ǥ�� �����ؼ� �����ؾ� �Ѵ�.
			// �������� ��ǥ�� �̸� ó���ؼ� ���� ���� �ִµ�, ���� ��뵵 �ְ�, ���������� �ٸ� ���� ó������ �κ�Ÿ��ó�� ���׼� �׳� �������� �ϱ� ���ؼ�
			// ���⿡�� ó���Ѵ�. Ŭ��â�� �κ��丮 �迭�� �Ǿ��ִٸ� �̷��� ó���ؾ� �Ѵ�. ���� Ŭ�� â�� �۾� ���� ����
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
			// ������Ŀ ������ ��ǥ ó��, ���߿� ä���̶�� Į���� ��� �߰��ϴ���
			// ���� �Լ��� ������� �ؾ��Ѵ�. ��Ŀ�� �κ��丮 �迭�� �Ǿ��־� ��� ������ �� x��ǥ�� �����ؼ� �����ؾ� �Ѵ�.
			// �������� ��ǥ�� �̸� ó���ؼ� ���� ���� �ִµ�, ���� ��뵵 �ְ�, ���������� �ٸ� ���� ó������ �κ�Ÿ��ó�� ���׼� �׳� �������� �ϱ� ���ؼ�
			// ���⿡�� ó���Ѵ�. Ŭ��â�� �κ��丮 �迭�� �Ǿ��ִٸ� �̷��� ó���ؾ� �Ѵ�. ���� Ŭ�� â�� �۾� ���� ����
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