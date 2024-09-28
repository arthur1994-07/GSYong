#include "pch.h"

#include "../Database/DBAction/DbActionGameCountry.h"

#include "../AgentServer/GLAgentServer.h"

#include "../../RanLogic/GLUseFeatures.h"

#include "./GLVictoriousCountryManServer.h"

namespace Country
{

	const float GLVictoriousCountryManServer::CONFIRM_VCOUNTRY_TIME = 3600.f;

	void GLVictoriousCountryManServer::OnFrameMove ( float fElapsedTime )
	{
		// ���弭���� �ƴ� ��쿡�� �����Ѵ�;
		if ( GLUseFeatures::GetInstance().IsUsingWorldBattle() )
			return;

		// ���弭�� ���� ������ ��쿡�� �����Ѵ�;
		if ( false == GLUseFeatures::GetInstance().IsUsingJoinWorldBattle() )
			return;

		static float fTime = 0.f;
		fTime += fElapsedTime;
		if ( fTime >= CONFIRM_VCOUNTRY_TIME )
			fTime = 0.f;
		else
			return;

		// ���弭���� ������ ������ ����ȭ�ϱ� ���ؼ� �����Ѵ�;
		for ( int i=0; i<EMCOUNTRY_SYSTEM_FLAG_SIZE; ++i )
		{
			EMCOUNTRY_SYSTEM_FLAG emFlag =
				static_cast< EMCOUNTRY_SYSTEM_FLAG >( i );

			// ���弭���� ������ ������ ����ȭ �ϱ� ���� ����ؼ� SP�� �����Ѵ�;
			m_pServer->AddGameAdoJob( db::DbActionPtr(
				new CConfirmStateVictoriousCountryAction(
				emFlag ) ) );
		}
	}

	void GLVictoriousCountryManServer::LoadDBAction ()
	{
		if ( m_pServer )
		{
			m_pServer->AddGameAdoJob( db::DbActionPtr(
				new CLoadAgentVictoriousCountryAction() ) );
		}
	}

	void GLVictoriousCountryManServer::AddDBAction ( const Country::SVICTORIOUS_COUNTRY& sVCountry )
	{
		if ( m_pServer )
		{
			m_pServer->AddGameAdoJob( db::DbActionPtr(
				new CAddVictoriousCountryAction( sVCountry ) ) );
		}
	}

	void GLVictoriousCountryManServer::ResetDBAction ( Country::EMCOUNTRY_SYSTEM_FLAG emFlag )
	{
		if ( m_pServer )
		{
			m_pServer->AddGameAdoJob( db::DbActionPtr(
				new CResetVictoriousCountryAction( emFlag ) ) );
		}
	}

	void GLVictoriousCountryManServer::AddVCountry ( const Country::SVICTORIOUS_COUNTRY& sVCountry )
	{
		m_vecVCountry.push_back( sVCountry );
	}

	void GLVictoriousCountryManServer::ResetVCountry ( Country::EMCOUNTRY_SYSTEM_FLAG emFlag )
	{
		for ( VCOUNTRY_VEC_ITER iter = m_vecVCountry.begin();
			iter != m_vecVCountry.end(); )
		{
			SVICTORIOUS_COUNTRY sVCountry = *iter;
			if ( sVCountry.emFlag == emFlag )
				iter = m_vecVCountry.erase( iter );
			else
				++iter;
		}
	}

	const DWORD GLVictoriousCountryManServer::GetVCountryFirst ( EMCOUNTRY_SYSTEM_FLAG emFlag ) const
	{
		for ( VCOUNTRY_VEC_CITER iter = m_vecVCountry.begin();
			iter != m_vecVCountry.end(); ++iter )
		{
			SVICTORIOUS_COUNTRY sVCountry = *iter;
			if ( sVCountry.emFlag == emFlag )
				return sVCountry.sCountryInfo.dwID;
		}

		return SCOUNTRY_INFO().dwID;
	}

	void GLVictoriousCountryManServer::MsgVCountryADDFAC ( NET_MSG_GENERIC* pMsg )
	{
		GLMSG::NET_VCOUNTRY_ADD_FAC* pAddMsg =
			static_cast< GLMSG::NET_VCOUNTRY_ADD_FAC* >( pMsg );

		if ( pAddMsg )
		{
			if ( false == pAddMsg->m_bTempVCountry )
			{
				AddDBAction( pAddMsg->m_sVCountry );
			}
			else
			{
				ResetVCountry( pAddMsg->m_sVCountry.emFlag );

				AddVCountry( pAddMsg->m_sVCountry );

				if ( m_pServer )
				{
					// Field���� ���� Ȯ�� �޽����� ������;
					GLMSG::NET_VCOUNTRY_ADD_DAF sAddMsgAF( pAddMsg->m_sVCountry );
					m_pServer->SENDTOALLCHANNEL( &sAddMsgAF );
				}			
			}
		}
	}

	void GLVictoriousCountryManServer::MsgVCountryResetFAC ( NET_MSG_GENERIC* pMsg )
	{
		GLMSG::NET_VCOUNTRY_RESET_FAC* pResetMsg =
			static_cast< GLMSG::NET_VCOUNTRY_RESET_FAC* >( pMsg );

		if ( pResetMsg )
		{
			if ( false == pResetMsg->m_bTempVCountry )
			{
				ResetDBAction( pResetMsg->m_emFlag );
			}
			else
			{
				ResetVCountry( pResetMsg->m_emFlag );

				if ( m_pServer )
				{
					// Field���� ���� Ȯ�� �޽����� ������;
					GLMSG::NET_VCOUNTRY_RESET_DAF sResetMsgAF( pResetMsg->m_emFlag );
					m_pServer->SENDTOALLCHANNEL( &sResetMsgAF );
				}
			}
		}
	}

	void GLVictoriousCountryManServer::MsgVCountryADDDA ( NET_MSG_GENERIC* pMsg )
	{
		GLMSG::NET_VCOUNTRY_ADD_DAF* pAddMsg =
			static_cast< GLMSG::NET_VCOUNTRY_ADD_DAF* >( pMsg );

		if ( pAddMsg )
		{
			ResetVCountry( pAddMsg->m_sVCountry.emFlag );

			AddVCountry( pAddMsg->m_sVCountry );

			if ( m_pServer )
			{
				// Field���� ���� Ȯ�� �޽����� ������;
				m_pServer->SENDTOALLCHANNEL( pAddMsg );

				// ���弭���� ��� ���弭���� ����� �ٸ� Agent���� ������;
				if ( GLUseFeatures::GetInstance().IsUsingWorldBattle() )
				{
					GLMSG::NET_VCOUNTRY_ADD_AA sAddMsg( pAddMsg->m_sVCountry );
					m_pServer->SENDTOAGENT( &sAddMsg );
				}
			}			
		}
	}

	void GLVictoriousCountryManServer::MsgVCountryResetDA ( NET_MSG_GENERIC* pMsg )
	{
		GLMSG::NET_VCOUNTRY_RESET_DAF* pResetMsg =
			static_cast< GLMSG::NET_VCOUNTRY_RESET_DAF* >( pMsg );

		if ( pResetMsg )
		{
			ResetVCountry( pResetMsg->m_emFlag );

			if ( m_pServer )
			{
				// Field���� ���� Ȯ�� �޽����� ������;
				m_pServer->SENDTOALLCHANNEL( pResetMsg );

				// ���弭���� ��� ���弭���� ����� �ٸ� Agent���� ������;
				if ( GLUseFeatures::GetInstance().IsUsingWorldBattle() )
				{
					GLMSG::NET_VCOUNTRY_RESET_AA sResetMsg( pResetMsg->m_emFlag );
					m_pServer->SENDTOAGENT( &sResetMsg );
				}
			}
		}
	}

	void GLVictoriousCountryManServer::MsgVCountryADDAA ( NET_MSG_GENERIC* pMsg )
	{
		GLMSG::NET_VCOUNTRY_ADD_AA* pAddMsg =
			static_cast< GLMSG::NET_VCOUNTRY_ADD_AA* >( pMsg );

		if ( pAddMsg )
		{
			AddDBAction( pAddMsg->m_sVCountry );
		}
	}

	void GLVictoriousCountryManServer::MsgVCountryResetAA ( NET_MSG_GENERIC* pMsg )
	{
		GLMSG::NET_VCOUNTRY_RESET_AA* pResetMsg =
			static_cast< GLMSG::NET_VCOUNTRY_RESET_AA* >( pMsg );

		if ( pResetMsg )
		{
			ResetDBAction( pResetMsg->m_emFlag );
		}
	}

	void GLVictoriousCountryManServer::SyncState ( DWORD dwClientID )
	{
		for ( VCOUNTRY_VEC_ITER iter = m_vecVCountry.begin();
			iter != m_vecVCountry.end(); ++iter )
		{
			SVICTORIOUS_COUNTRY sVCountry = *iter;

			GLMSG::NET_VCOUNTRY_SYNC_AC sMsg( sVCountry );

			m_pServer->SENDTOCLIENT( dwClientID, &sMsg );
		}
	}
}