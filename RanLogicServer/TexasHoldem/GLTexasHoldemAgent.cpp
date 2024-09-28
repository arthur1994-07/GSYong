#include "pch.h"

#include "../../RanLogic/GLUseFeatures.h"
#include "../../RanLogic/Msg/GLContrlTexasHoldemMsg.h"
#include "../../RanLogic/TexasHoldem/GLTexasHoldemSetting.h"

#include "../AgentServer/GLAgentServer.h"
#include "../Database/DBAction/DbActionGameTexasHoldem.h"

#include "./GLTexasHoldemAgent.h"
#include "../../SigmaCore/DebugInclude.h"

namespace TexasHoldem
{
	GLTexasHoldemAgent::GLTexasHoldemAgent( GLAgentServer* pServer )
		: m_pServer( pServer )
		, m_Timer( 0.0f )
	{
	}

	GLTexasHoldemAgent::~GLTexasHoldemAgent()
	{

	}

	void GLTexasHoldemAgent::FrameMove( float fElapsedTime )
	{
		if( GLUseFeatures::GetInstance().IsUsingMiniGameTexasHoldem() == false )
			return;

		// 60초마다 버전 확인
		m_Timer += fElapsedTime;
		if ( m_Timer > 60.0f )
		{
			m_Timer = 0.0f;

			// 버전 확인
			LoadChipMapVersion();
		}
	}

	void GLTexasHoldemAgent::MessageProcess( NET_MSG_GENERIC* pMessage, DWORD ClientID /*= 0*/, DWORD GaeaID /*= 0 */ )
	{
		if( GLUseFeatures::GetInstance().IsUsingMiniGameTexasHoldem() == false )
			return;

		if ( NULL == pMessage )
		{
			return;
		}

		GLMSG::NET_TEXAS_HOLD_EM_MESSAGE* nmg = ( GLMSG::NET_TEXAS_HOLD_EM_MESSAGE* ) pMessage;

		switch ( nmg->eType )
		{
		case ChipMapVersion:
			MsgChipMapVersion( pMessage, ClientID, GaeaID );
			break;
		case ChipMap:
			MsgChipMap( pMessage, ClientID, GaeaID );
			break;
		case ChipMapEnd:
			MsgChipMapEnd( pMessage, ClientID, GaeaID );
			break;
		case ChipMapRequest:
			MsgChipMapRequest( pMessage, ClientID, GaeaID );
			break;
		default:
			break;
		}
	}

	void GLTexasHoldemAgent::LoadChipMapVersion()
	{
		m_pServer->AddUserAdoJob( db::DbActionPtr( new db::TexasHoldemChipMapVersionLoad() ) );
	}

	void GLTexasHoldemAgent::LoadChipMap()
	{
		m_pServer->AddUserAdoJob( db::DbActionPtr( new db::TexasHoldemChipMapLoad() ) );
	}

	void GLTexasHoldemAgent::LoadChipMap( db::IDbManager* pAdo )
	{
		if ( NULL == pAdo )
		{
			return;
		}

		LONGLONG nVersion = 0;
		int nResult = pAdo->TexasHoldemChipMapVersionSelect( nVersion );
		if ( sc::db::DB_OK != nResult )
		{
			// error
			return;
		}

		GLTexasHoldemMgr::SetVersion( nVersion );

		std::vector< sChipPrice > vecChip;
		nResult = pAdo->TexasHoldemChipMapSelect( vecChip );
		if ( sc::db::DB_OK != nResult )
		{
			// error
			return;
		}

		size_t ChipMapSize = vecChip.size();
		for ( size_t loop = 0; loop < ChipMapSize; loop++  )
		{
			// 수수료 추가
			sChipPrice& sChip = vecChip[loop];
			LONGLONG GameMoneyCommision = 
				sChip.GameMoney * static_cast<LONGLONG>(GLTexasHoldemSetting::getInstance()->GetCommision());
			LONGLONG PointCommision = 
				sChip.Point * static_cast<LONGLONG>(GLTexasHoldemSetting::getInstance()->GetCommision());
			sChip.GameMoney += GameMoneyCommision;
			sChip.GameMoney = sChip.GameMoney / 100 * 100;
			sChip.Point += PointCommision;
			sChip.Point = sChip.Point / 100 * 100;

			GLTexasHoldemMgr::InsertChipPrice( vecChip[loop] );
		}
	}

	void GLTexasHoldemAgent::MsgChipMapVersion( NET_MSG_GENERIC* pMessage, DWORD ClientID, DWORD GaeaID )
	{
		GLMSG::NET_TEXAS_HOLD_EM_CHIP_VERSION* nmg = ( GLMSG::NET_TEXAS_HOLD_EM_CHIP_VERSION* ) pMessage;
		if ( GLTexasHoldemMgr::GetVersion() != nmg->Version )
		{
			// 버전이 틀리다. 초기화하고 새로 판매리스트 받아온다.
			GLTexasHoldemMgr::Reset();
			GLTexasHoldemMgr::SetVersion( nmg->Version );
			LoadChipMap();
			m_pServer->SENDTOALLCHANNEL( pMessage );
		}
	}

	void GLTexasHoldemAgent::MsgChipMap( NET_MSG_GENERIC* pMessage, DWORD ClientID, DWORD GaeaID )
	{
		GLMSG::NET_TEXAS_HOLD_EM_CHIP_MAP* nmg = ( GLMSG::NET_TEXAS_HOLD_EM_CHIP_MAP* ) pMessage;

		// 수수료 추가
		sChipPrice& sChip = nmg->sChip;
		LONGLONG GameMoneyCommision = 
			sChip.GameMoney * static_cast<LONGLONG>(GLTexasHoldemSetting::getInstance()->GetCommision());
		LONGLONG PointCommision = 
			sChip.Point * static_cast<LONGLONG>(GLTexasHoldemSetting::getInstance()->GetCommision());
		sChip.GameMoney += GameMoneyCommision;
		sChip.GameMoney = sChip.GameMoney / 100 * 100;
		sChip.Point += PointCommision;
		sChip.Point = sChip.Point / 100 * 100;

		GLTexasHoldemMgr::InsertChipPrice( nmg->sChip );
		m_pServer->SENDTOALLCHANNEL( pMessage );
	}

	void GLTexasHoldemAgent::MsgChipMapEnd( NET_MSG_GENERIC* pMessage, DWORD ClientID, DWORD GaeaID )
	{
		GLMSG::NET_TEXAS_HOLD_EM_CHIP_MAP_END* nmg = ( GLMSG::NET_TEXAS_HOLD_EM_CHIP_MAP_END* ) pMessage;
		m_pServer->SENDTOALLCHANNEL( pMessage );
	}

	void GLTexasHoldemAgent::MsgChipMapRequest( NET_MSG_GENERIC* pMessage, DWORD ClientID, DWORD GaeaID )
	{
		GLMSG::NET_TEXAS_HOLD_EM_CHIP_VERSION NetMsgVersion( GLTexasHoldemMgr::GetVersion() );
		m_pServer->SENDTOCLIENT( ClientID, &NetMsgVersion );

		const std::vector< sChipPrice >& vecChip = GetChipMap();
		size_t ChipMapSize = vecChip.size();
		for ( size_t loop = 0; loop < ChipMapSize; loop++  )
		{
			GLMSG::NET_TEXAS_HOLD_EM_CHIP_MAP NetMsg( vecChip[loop] );
			m_pServer->SENDTOCLIENT( ClientID, &NetMsg );
		}

		GLMSG::NET_TEXAS_HOLD_EM_CHIP_MAP_END NetMsg;
		m_pServer->SENDTOCLIENT( ClientID, &NetMsg );
	}

}