#include "pch.h"
#include "../../../SigmaCore/Log/LogMan.h"
#include "../../../SigmaCore/String/StringUtil.h"
#include "../../../RanLogic/Msg/GLContrlTexasHoldemMsg.h"
#include "../../Server/AgentServer.h"
#include "../../Server/s_CFieldServer.h"

#include "../DbManager.h"
#include "./DbActionGameTexasHoldem.h"



// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------



namespace db
{


	TexasHoldemChipMapLoad::TexasHoldemChipMapLoad()
	{
	}

	TexasHoldemChipMapLoad::~TexasHoldemChipMapLoad()
	{
	}

	int TexasHoldemChipMapLoad::Execute( NetServer* pServer )
	{
		if ( !pServer )
			return NET_ERROR;

		CAgentServer* pAgentServer = reinterpret_cast< CAgentServer* >( pServer );
		if ( !pAgentServer )
			return NET_ERROR;

		if ( db::ADO != m_pDbManager->GetConnectionType() )
		{
			sc::writeLogError( "TexasHoldemChipMapLoad::Execute. Use ADO" );
			return NET_ERROR;
		}

		int nReturn = NET_ERROR;
		nReturn = m_pDbManager->TexasHoldemChipMapSelect( m_vecChip );
		if ( sc::db::DB_OK != nReturn )
		{
			// error
			return NET_ERROR;
		}

		// agent 서버 메시지에 넣어준다.
		size_t ChipSize = m_vecChip.size();
		for ( size_t loop = 0; loop < ChipSize; loop++ )
		{
			GLMSG::NET_TEXAS_HOLD_EM_CHIP_MAP NetMsg( m_vecChip[loop] );
			pAgentServer->InsertMsg( DBACTION_CLIENT_ID, &NetMsg );
		}

		GLMSG::NET_TEXAS_HOLD_EM_CHIP_MAP_END NetMsg;
		pAgentServer->InsertMsg( DBACTION_CLIENT_ID, &NetMsg );

		return NET_OK;
	}


	TexasHoldemChipMapVersionLoad::TexasHoldemChipMapVersionLoad()
	{
	}

	TexasHoldemChipMapVersionLoad::~TexasHoldemChipMapVersionLoad()
	{
	}

	int TexasHoldemChipMapVersionLoad::Execute( NetServer* pServer )
	{
		if ( !pServer )
			return NET_ERROR;

		CAgentServer* pAgentServer = reinterpret_cast< CAgentServer* >( pServer );
		if ( !pAgentServer )
			return NET_ERROR;

		if ( db::ADO != m_pDbManager->GetConnectionType() )
		{
			sc::writeLogError( "TexasHoldemChipMapVersionLoad::Execute. Use ADO" );
			return NET_ERROR;
		}

		int nReturn = NET_ERROR;
		nReturn = m_pDbManager->TexasHoldemChipMapVersionSelect( m_Version );
		if ( sc::db::DB_OK != nReturn )
		{
			// error
			return NET_ERROR;
		}

		// agent 서버 메시지에 넣어준다.
		GLMSG::NET_TEXAS_HOLD_EM_CHIP_VERSION NetMsg( m_Version );
		pAgentServer->InsertMsg( DBACTION_CLIENT_ID, &NetMsg );

		return NET_OK;
	}



	TexasHoldemChipSave::TexasHoldemChipSave( DWORD dwChaDbNum, LONGLONG chip, bool bPlus /*= false*/ )
		: m_dwChaDbNum( dwChaDbNum )
		, m_chip( chip )
		, m_bPlus( bPlus )
	{
	}

	TexasHoldemChipSave::~TexasHoldemChipSave()
	{
	}

	int TexasHoldemChipSave::Execute( NetServer* pServer )
	{
		if ( db::ADO != m_pDbManager->GetConnectionType() )
		{
			sc::writeLogError( "TexasHoldemChipSave::Execute. Use ADO" );
			return NET_ERROR;
		}

		return m_pDbManager->TexasHoldemChipUpdate( m_dwChaDbNum, m_chip, m_bPlus );
	}


	TexasHoldemChipLogSave::TexasHoldemChipLogSave( DWORD dwChaDbNum, LONGLONG chip, LONGLONG GameMoney, LONGLONG Point )
		: m_dwChaDbNum( dwChaDbNum )
		, m_chip( chip )
		, m_GameMoney( GameMoney )
		, m_Point( Point )
	{
	}

	TexasHoldemChipLogSave::~TexasHoldemChipLogSave()
	{
	}

	int TexasHoldemChipLogSave::Execute( NetServer* pServer )
	{
		if ( db::ADO != m_pDbManager->GetConnectionType() )
		{
			sc::writeLogError( "TexasHoldemChipLogSave::Execute. Use ADO" );
			return NET_ERROR;
		}

		return m_pDbManager->TexasHoldemChipLogInsert( m_dwChaDbNum, m_chip, m_GameMoney, m_Point );
	}


	TexasHoldemChipLoad::TexasHoldemChipLoad( DWORD dwChaDbNum, bool bEndGameNotify /*= false*/ )
		: m_dwChaDbNum( dwChaDbNum )
		, m_bEndGameNotify( bEndGameNotify )
		, m_chip( 0 )
	{
	}

	TexasHoldemChipLoad::~TexasHoldemChipLoad()
	{
	}

	int TexasHoldemChipLoad::Execute( NetServer* pServer )
	{
		if ( db::ADO != m_pDbManager->GetConnectionType() )
		{
			sc::writeLogError( "TexasHoldemChipLoad::Execute. Use ADO" );
			return NET_ERROR;
		}

		CFieldServer* pFieldServer = reinterpret_cast< CFieldServer* >( pServer );
		if ( NULL == pFieldServer )
		{
			return NET_ERROR;
		}

		int nRetCode = m_pDbManager->TexasHoldemChipSelectFromWeb( pFieldServer->GetServerGroup(), m_dwChaDbNum, m_chip );
		if ( sc::db::DB_ERROR == nRetCode )
		{
			sc::writeLogError( sc::string::format( "DB:TexasHoldemChipLoad DB read failed. %d", m_dwChaDbNum ) );
			return sc::db::DB_ERROR;
		}

		// field 서버 메시지에 넣어준다.
		GLMSG::NET_TEXAS_HOLD_EM_CHIP_INNER NetMsg( m_dwChaDbNum, m_chip, m_bEndGameNotify );
		pFieldServer->InsertMsg( DBACTION_CLIENT_ID, &NetMsg );
		return NET_OK;
	}


	TexasHoldemAuthKeySave::TexasHoldemAuthKeySave( DWORD dwChaDbNum, UUID& sGUID )
		: m_dwChaDbNum( dwChaDbNum )
		, m_sGUID( sGUID )
	{
	}

	TexasHoldemAuthKeySave::~TexasHoldemAuthKeySave()
	{
	}

	int TexasHoldemAuthKeySave::Execute( NetServer* pServer )
	{
		if ( db::ADO != m_pDbManager->GetConnectionType() )
		{
			sc::writeLogError( "TexasHoldemAuthKeySave::Execute. Use ADO" );
			return NET_ERROR;
		}

		CFieldServer* pFieldServer = reinterpret_cast< CFieldServer* >( pServer );
		if ( NULL == pFieldServer )
		{
			return NET_ERROR;
		}

		// http://forums.devx.com/showthread.php?46121.html
		// ado 에 guid 값을 파라미터로 넘길때
		// { } 로 싸서 문자열을 넘긴다.
		std::string strOutput = "{";
		std::string strGUID = sc::string::uuidToString( m_sGUID );
		strOutput += strGUID;
		strOutput += "}";
		int nRetCode = m_pDbManager->TexasHoldemAuthKeyInsert( m_dwChaDbNum, strOutput );
		if ( sc::db::DB_ERROR == nRetCode )
		{
			sc::writeLogError( sc::string::format( "DB:TexasHoldemAuthKeyInsert failed. %d", m_dwChaDbNum ) );
		}

		// field 서버 메시지에 넣어준다. 실패해도 메시지는 전달한다.
		GLMSG::NET_TEXAS_HOLD_EM_AUTHKEY_INNER NetMsg( m_dwChaDbNum, m_sGUID );
		pFieldServer->InsertMsg( DBACTION_CLIENT_ID, &NetMsg );
		return NET_OK;
	}


	TexasHoldemGameTimeSave::TexasHoldemGameTimeSave( DWORD dwChaDbNum, DWORD dwUserNum, int nSecond )
		: m_dwChaDbNum( dwChaDbNum )
		, m_dwUserNum( dwUserNum )
		, m_nSecond( nSecond )
	{
	}

	TexasHoldemGameTimeSave::~TexasHoldemGameTimeSave()
	{
	}

	int TexasHoldemGameTimeSave::Execute( NetServer* pServer )
	{
		if ( db::ADO != m_pDbManager->GetConnectionType() )
		{
			sc::writeLogError( "TexasHoldemGameTimeSave::Execute. Use ADO" );
			return NET_ERROR;
		}

		int nRetCode = m_pDbManager->TexasHoldemGameTimeInsert( m_dwChaDbNum, m_dwUserNum, m_nSecond );
		if ( sc::db::DB_ERROR == nRetCode )
		{
			sc::writeLogError( sc::string::format( "DB:TexasHoldemGameTimeSave failed. %d", m_dwChaDbNum ) );
		}
		return nRetCode;
	}

}
