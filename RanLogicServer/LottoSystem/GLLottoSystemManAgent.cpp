#include "pch.h"

#include "./GLLottoSystemManAgent.h"

#include "../AgentServer/GLAgentServer.h"
#include "../Database/DBAction/DbActionGameLottoSystem.h"

#include "./GLLottoGlobalData.h"

#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/GLUseFeatures.h"

#include "../../SigmaCore/Util/SystemInfo.h"

#include "../../enginelib/Common/SUBPATH.h"

namespace LottoSystem
{

	GLLottoSystemManAgent::GLLottoSystemManAgent()
		: m_pAgentServer( NULL ) { }
	GLLottoSystemManAgent::~GLLottoSystemManAgent() { }

	void GLLottoSystemManAgent::Initialize()
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return;

		if( !m_pAgentServer )
			return;

		if ( m_pAgentServer->IsSubServer() )
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

		// Script를 이용해 LottoSystem 추가;
		int lottoSystemCount = GLLottoGlobalData::Instance()->GetLottoSystemNum();
		for( int i=0; i<lottoSystemCount; ++i )
			AddLottoSystem( static_cast<LOTTO_SYSTEM_ID>( i ) );

		// Initialize Lotto System;
		for( unsigned int i=0; i<m_pLottoSystems.size(); ++i )
		{
			CLottoSystem* pLottoSystem = m_pLottoSystems.at( i );
			if( pLottoSystem )
			{
				pLottoSystem->LoadFromDB();
				pLottoSystem->Initialize();
			}
		}
	}

	void GLLottoSystemManAgent::Destroy()
	{
		if( !m_pAgentServer )
			return;

		if ( m_pAgentServer->IsSubServer() )
			return;

		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return;

		for( unsigned int i=0; i<m_pLottoSystems.size(); ++i )
		{
			CLottoSystem* pLottoSystem = m_pLottoSystems.at( i );
			if( pLottoSystem )
			{
				pLottoSystem->Destroy();
			}
		}

		for( unsigned int i=0; i<m_pLottoSystems.size(); ++i )
			SAFE_DELETE( m_pLottoSystems.at( i ) );

		m_pLottoSystems.clear();
	}

	bool GLLottoSystemManAgent::Update( float fElapsedTime, const CTime& currentTime )
	{
		if( !m_pAgentServer )
			return true;

		if ( m_pAgentServer->IsSubServer() )
			return true;

		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return true;

		// Update Lotto System;
		for( unsigned int i=0; i<m_pLottoSystems.size(); ++i )
		{
			CLottoSystem* pLottoSystem = m_pLottoSystems.at( i );
			if( pLottoSystem )
			{
				// 업데이트;
				if( !pLottoSystem->Update( fElapsedTime, currentTime ) )
					return false;
			}
		}

		return true;
	}

	bool GLLottoSystemManAgent::OnMessage( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID )
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return true;

		if( !m_pAgentServer )
			return true;

		// 동기화 메시지일 경우 처리한다;
		if ( pMsg->nType == NET_MSG_GCTRL_LOTTO_SYSTEM_SYNC_AGENT_MYSELF )
		{
			SyncState( dwClientID, pMsg->dwChaNum );

			return true;
		}

		if ( m_pAgentServer->IsSubServer() )
		{
			// Relay;
			m_pAgentServer->SENDTOAGENT( pMsg );

			return true;
		}

		LOTTO_SYSTEM_ID lottoSystemId;

		// 메시지에서 LottoSystemId를 가져온다;
		lottoSystemId = pMsg->m_lottoSystemId;

		// LottoSystemId 에 맞는 LottoSystem의 메시지를 처리한다;
		for( unsigned int i=0; i<m_pLottoSystems.size(); ++i )
		{
			CLottoSystem* pLottoSystem = m_pLottoSystems.at( i );
			if( pLottoSystem && pLottoSystem->GetLottoSystemId() == lottoSystemId )
			{
				if( !pLottoSystem->OnMessage( pMsg, dwClientID, dwGaeaID ) )
					return false;

				return true;
			}
		}

		return true;
	}

	void GLLottoSystemManAgent::SyncState( DWORD dwClientID, DWORD dwChaNum )
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return;

		if( !m_pAgentServer )
			return;

		if ( m_pAgentServer->IsSubServer() )
		{
			GLMSG::NET_LOTTO_MESSAGE_SYNC_AGENT_MYSELF sMsg;
			sMsg.dwChaNum = dwChaNum;
			m_pAgentServer->SENDTOAGENT( &sMsg );

			return;
		}

		// Global Value 클라이언트와 동기화;
		GLMSG::NET_LOTTO_MESSAGE_SYNC_GLOBAL_VALUE_AC msgSync( 0 );
		msgSync.m_emAC = LottoSystem::EMLOTTO_REQ_SYNC_STATUS_AC_OK;
		msgSync.m_golbalValue = GLLottoGlobalData::Instance()->GetLottoGlobalValue();
		SendToClientFromAgent( dwChaNum, dwClientID, &msgSync );

		// Sync State Lotto System;
		for( unsigned int i=0; i<m_pLottoSystems.size(); ++i )
		{
			CLottoSystem* pLottoSystem = m_pLottoSystems.at( i );
			if( pLottoSystem )
				pLottoSystem->SyncState( dwClientID, dwChaNum );
		}
	}

	bool GLLottoSystemManAgent::ChangeLottoSystemState( unsigned int lottoSystemId, EMLOTTO_STATE state )
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return true;

		if ( m_pAgentServer->IsSubServer() )
		{
			return true;
		}

		for( unsigned int i=0; i<m_pLottoSystems.size(); ++i )
		{
			CLottoSystem* pLottoSystem = m_pLottoSystems.at( i );
			if( pLottoSystem && pLottoSystem->GetLottoSystemId() == lottoSystemId )
			{
				if( !pLottoSystem->ChangeState( state ) )
					return false;

				return true;
			}
		}

		return true;
	}

	void GLLottoSystemManAgent::SendToClientFromAgent( DWORD dwDbNum, DWORD dwClientID, GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg )
	{
		if ( false ==GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return;

		if ( NULL == m_pAgentServer )
			return;

		if ( NULL == pMsg )
			return;

		m_pAgentServer->SENDTOCLIENT_RELAY( dwDbNum, dwClientID, pMsg );
	}

	void GLLottoSystemManAgent::SendToServerGroupClientFromAgent ( DWORD dwDbNum, DWORD dwServerGroup, GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg )
	{
		if ( false ==GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return;

		if ( NULL == m_pAgentServer )
			return;

		if ( NULL == pMsg )
			return;

		m_pAgentServer->SENDTOCLIENT_RELAY_BYSVRGROUP( dwDbNum, dwServerGroup, pMsg );
	}

	// Agent와 Field와의 통신;
	void GLLottoSystemManAgent::SendToFieldFromAgent( DWORD dwClientID, GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg )
	{
		if ( false == GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return;

		if ( NULL == m_pAgentServer )
			return;

		if ( NULL == pMsg )
			return;

		m_pAgentServer->SENDTOFIELD_RELAY( pMsg->dwChaNum, dwClientID, pMsg );
	}

	void GLLottoSystemManAgent::SendToServerGroupFieldFromAgent ( DWORD dwServerGroup, GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg )
	{
		if ( false == GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return;

		if ( NULL == m_pAgentServer )
			return;

		if ( NULL == pMsg )
			return;

		m_pAgentServer->SENDTOFIELD_RELAY_BYSVRGROUP( pMsg->dwChaNum, dwServerGroup, pMsg );		
	}

	void GLLottoSystemManAgent::BroadcastFromAgent( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg )
	{
		if ( false == GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return;

		if ( NULL == m_pAgentServer )
			return;

		if ( NULL == pMsg )
			return;

		m_pAgentServer->SENDTOALLCLIENT_RELAY( pMsg );
	}

	void GLLottoSystemManAgent::AddLottoSystem( LOTTO_SYSTEM_ID id )
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return;

		CLottoSystem* pLottoSystem = new CLottoSystem( id );
		m_pLottoSystems.push_back( pLottoSystem );
	}

	// Lotto System;
	CLottoSystem* GLLottoSystemManAgent::GetLottoSystem( LOTTO_SYSTEM_ID id )
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return NULL;

		if( id >= 0 && id < m_pLottoSystems.size() )
			return m_pLottoSystems.at( id );

		return NULL;
	}

	// DBAction;
	void GLLottoSystemManAgent::SystemLoad( unsigned int systemId )
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return;

		GLAgentServer* pServer =
			GLLottoSystemManAgent::Instance()->GetAgentServer();
		if( pServer )
		{
			pServer->AddGameAdoJob( db::DbActionPtr(
				new CLottoSystemLoadAction( systemId ) ) );
		}
	}

	void GLLottoSystemManAgent::DrawingMachineNumSelectAction(
		unsigned int lottoNum,
		unsigned int systemId )
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return;

		GLAgentServer* pServer =
			GLLottoSystemManAgent::Instance()->GetAgentServer();
		if( pServer )
		{
			pServer->AddGameAdoJob( db::DbActionPtr(
				new CLottoDrawingMachineNumSelectAction(
				lottoNum, systemId ) ) );
		}
	}

	void GLLottoSystemManAgent::CurTurnSelectAction( unsigned int systemId )
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return;

		GLAgentServer* pServer =
			GLLottoSystemManAgent::Instance()->GetAgentServer();
		if( pServer )
		{
			pServer->AddGameAdoJob( db::DbActionPtr(
				new CLottoSystemCurTurnSelectAction( systemId ) ) );
		}
	}

	void GLLottoSystemManAgent::AccumulateMoneySelectAction(
		DWORD dwDbNum,
		DWORD dwServerGroup,
		unsigned int turnNum,
		unsigned int systemId )
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return;

		GLAgentServer* pServer =
			GLLottoSystemManAgent::Instance()->GetAgentServer();
		if( pServer )
		{
			pServer->AddGameAdoJob( db::DbActionPtr(
				new CLottoAccumulateMoneySelectAction( dwDbNum, dwServerGroup, turnNum, systemId ) ) );
		}
	}

	void GLLottoSystemManAgent::WinNumSelectAction(
		DWORD dwDbNum,
		DWORD dwServerGroup,
		unsigned int turnNum,
		unsigned int systemId )
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return;

		GLAgentServer* pServer =
			GLLottoSystemManAgent::Instance()->GetAgentServer();
		if( pServer )
		{
			pServer->AddGameAdoJob( db::DbActionPtr(
				new CLottoWinNumSelectAction( dwDbNum, dwServerGroup, turnNum, systemId ) ) );
		}
	}

	void GLLottoSystemManAgent::BuyListSelectAction(
		DWORD dwDbNum,
		DWORD dwServerGroup,
		unsigned int turnNum,
		unsigned int systemId )
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return;

		GLAgentServer* pServer =
			GLLottoSystemManAgent::Instance()->GetAgentServer();
		if( pServer )
		{
			pServer->AddGameAdoJob( db::DbActionPtr(
				new CLottoCharBuyListSelectAction(
				dwDbNum, dwServerGroup, turnNum, systemId ) ) );
		}
	}

	void GLLottoSystemManAgent::BuyListNonConfirmSelectAction(
		DWORD dwDbNum,
		DWORD dwServerGroup,
		unsigned int systemId,
		int confirmTicket )
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return;

		GLAgentServer* pServer =
			GLLottoSystemManAgent::Instance()->GetAgentServer();
		if( pServer )
		{
			pServer->AddGameAdoJob( db::DbActionPtr(
				new CLottoCharNonConfirmBuyListSelectAction(
				dwDbNum, dwServerGroup, systemId, confirmTicket ) ) );
		}
	}

	void GLLottoSystemManAgent::WinManListSelectAction(
		DWORD dwDbNum,
		DWORD dwServerGroup,
		unsigned int turnNum,
		unsigned int systemId )
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return;

		GLAgentServer* pServer =
			GLLottoSystemManAgent::Instance()->GetAgentServer();
		if( pServer )
		{
			pServer->AddGameAdoJob( db::DbActionPtr(
				new CLottoWinManListSelectAction(
				dwDbNum, dwServerGroup, turnNum, systemId ) ) );
		}
	}

	void GLLottoSystemManAgent::InventWinNumSelectAction(
		DWORD dwDbNum,
		DWORD dwServerGroup,
		unsigned int turnNum,
		unsigned int systemId )
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return;

		GLAgentServer* pServer =
			GLLottoSystemManAgent::Instance()->GetAgentServer();
		if( pServer )
		{
			pServer->AddGameAdoJob( db::DbActionPtr(
				new CLottoInventWinNumSelectAction(
				dwDbNum, dwServerGroup, turnNum, systemId ) ) );
		}
	}

	void GLLottoSystemManAgent::DrawingMachineNumSaveAction(
		unsigned int lottoNum,
		unsigned int accumulateCount,
		unsigned int addCount,
		unsigned int systemId,
		bool bUpdate )
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return;

		GLAgentServer* pServer =
			GLLottoSystemManAgent::Instance()->GetAgentServer();
		if( pServer )
		{
			pServer->AddGameAdoJob( db::DbActionPtr(
				new CLottoDrawingMachineNumSaveAction(
				lottoNum, accumulateCount, addCount, systemId, bUpdate ) ) );
		}
	}

	void GLLottoSystemManAgent::SystemTurnSaveAction(
		unsigned int turnNum,
		unsigned int systemId )
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return;

		GLAgentServer* pServer =
			GLLottoSystemManAgent::Instance()->GetAgentServer();
		if( pServer )
		{
			pServer->AddGameAdoJob( db::DbActionPtr(
				new CLottoSystemTurnSaveAction( turnNum, systemId ) ) );
		}
	}

	void GLLottoSystemManAgent::BuyListInsertAction(
		LONGLONG accumulateMoney,
		LONGLONG charge,
		int buyType,
		const std::string& drawingNum,
		LONGLONG drawingNumInt,
		int confirmTicket,
		DWORD dwUserNum,
		DWORD dwDbNum,
		DWORD dwServerGroup,
		char* szChaName,
		unsigned int turnNum,
		unsigned int systemId )
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return;

		GLAgentServer* pServer =
			GLLottoSystemManAgent::Instance()->GetAgentServer();
		if( pServer )
		{
			pServer->AddGameAdoJob( db::DbActionPtr(
				new CLottoCharBuyListInsertAction(
				accumulateMoney, charge, buyType, drawingNum, drawingNumInt,
				confirmTicket, dwUserNum, dwDbNum, dwServerGroup, szChaName, turnNum, systemId ) ) );
		}
	}

	void GLLottoSystemManAgent::WinManListUpdateAction(
		unsigned int ranking,
		LONGLONG winMoney,
		SNATIVEID sItemID,
		DWORD dwItemCount,
		unsigned int turnNum,
		unsigned int systemId )
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return;

		GLAgentServer* pServer =
			GLLottoSystemManAgent::Instance()->GetAgentServer();
		if( pServer )
		{
			pServer->AddGameAdoJob( db::DbActionPtr(
				new CLottoWinManListUpdateAction(
				ranking, winMoney, sItemID, dwItemCount, turnNum, systemId ) ) );
		}
	}

	void GLLottoSystemManAgent::WinManListInsertAction(
		unsigned int ranking,
		LONGLONG winNumInt,
		LONGLONG bonusNumInt,
		unsigned int systemId )
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return;

		GLAgentServer* pServer =
			GLLottoSystemManAgent::Instance()->GetAgentServer();
		if( pServer )
		{
			pServer->AddGameAdoJob( db::DbActionPtr(
				new CLottoWinManListInsertAction(
				ranking, winNumInt, bonusNumInt, systemId ) ) );
		}
	}

	void GLLottoSystemManAgent::AccumulateMoneyUpdateAction(
		LONGLONG accumulateMoney,
		unsigned int turnNum,
		unsigned int systemId )
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return;

		GLAgentServer* pServer =
			GLLottoSystemManAgent::Instance()->GetAgentServer();
		if( pServer )
		{
			pServer->AddGameAdoJob( db::DbActionPtr(
				new CLottoAccumulateMoneyUpdateAction(
				accumulateMoney,
				turnNum, systemId ) ) );
		}
	}

	void GLLottoSystemManAgent::WinNumUpdateAction(
		const std::string& winNum,
		LONGLONG winNumInt,
		LONGLONG bonusNumInt,
		unsigned int turnNum,
		unsigned int systemId )
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return;

		GLAgentServer* pServer =
			GLLottoSystemManAgent::Instance()->GetAgentServer();
		if( pServer )
		{
			pServer->AddGameAdoJob( db::DbActionPtr(
				new CLottoWinNumUpdateAction(
				winNum, winNumInt, bonusNumInt, turnNum, systemId ) ) );
		}
	}

	void GLLottoSystemManAgent::BuyListNonConfirmUpdateAction(
		DWORD dwDbNum,
		DWORD dwServerGroup,
		LONGLONG invenMoney,
		unsigned int systemId )
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return;

		GLAgentServer* pServer =
			GLLottoSystemManAgent::Instance()->GetAgentServer();
		if( pServer )
		{
			pServer->AddGameAdoJob( db::DbActionPtr(
				new CLottoBuyListNonConfirmUpdateAction(
				dwDbNum, dwServerGroup, invenMoney, systemId ) ) );
		}
	}

	void GLLottoSystemManAgent::SystemNextTurnAction(
		unsigned int systemId )
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return;

		GLAgentServer* pServer =
			GLLottoSystemManAgent::Instance()->GetAgentServer();
		if( pServer )
		{
			pServer->AddGameAdoJob( db::DbActionPtr(
				new CLottoSystemNextTurnAction( systemId ) ) );
		}
	}

	void GLLottoSystemManAgent::ConfirmWinManAction(
		unsigned int systemId )
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return;

		GLAgentServer* pServer =
			GLLottoSystemManAgent::Instance()->GetAgentServer();
		if( pServer )
		{
			pServer->AddGameAdoJob( db::DbActionPtr(
				new CLottoConfirmWinManAction( systemId ) ) );
		}
	}

	void GLLottoSystemManAgent::ChangeInventWinNumAction(
		unsigned int turnNum,
		unsigned int systemId )
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return;

		GLAgentServer* pServer =
			GLLottoSystemManAgent::Instance()->GetAgentServer();
		if( pServer )
		{
			pServer->AddGameAdoJob( db::DbActionPtr(
				new CLottoChangeInventWinNumAction( turnNum, systemId ) ) );
		}
	}

	void GLLottoSystemManAgent::WinMoneyUpdateAction(
		unsigned int turnNum,
		unsigned int systemId )
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return;

		GLAgentServer* pServer =
			GLLottoSystemManAgent::Instance()->GetAgentServer();
		if( pServer )
		{
			pServer->AddGameAdoJob( db::DbActionPtr(
				new CLottoWinMoneyUpdateAction( turnNum, systemId ) ) );
		}
	}

	void GLLottoSystemManAgent::SystemBuyLogAction(
		const LOTTO_SYSTEM_BUY_LOG& logInfo )
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return;

		GLAgentServer* pServer =
			GLLottoSystemManAgent::Instance()->GetAgentServer();
		if( pServer )
		{
			pServer->AddGameAdoJob( db::DbActionPtr(
				new CLottoSystemBuyLog( logInfo ) ) );
		}
	}


	void GLLottoSystemManAgent::SystemWinManLogAction(
		unsigned int turnNum,
		unsigned int systemId )
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return;

		GLAgentServer* pServer =
			GLLottoSystemManAgent::Instance()->GetAgentServer();
		if( pServer )
		{
			pServer->AddGameAdoJob( db::DbActionPtr(
				new CLottoSystemWinManLog( turnNum, systemId ) ) );
		}
	}

	void GLLottoSystemManAgent::SystemTurnInfoLogAction(
		const LOTTO_SYSTEM_TURN_LOG& logInfo )
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return;

		GLAgentServer* pServer =
			GLLottoSystemManAgent::Instance()->GetAgentServer();
		if( pServer )
		{
			pServer->AddGameAdoJob( db::DbActionPtr(
				new CLottoSystemTurnInfoLog( logInfo ) ) );
		}
	}
}