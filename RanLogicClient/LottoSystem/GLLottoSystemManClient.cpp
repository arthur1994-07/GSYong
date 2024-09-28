#include "pch.h"

#include "../GLGaeaClient.h"

#include "./GLLottoSystemManClient.h"
#include "./GLLottoGlobalDataClient.h"

#include "../../RanLogic/GLUseFeatures.h"

#include "../../SigmaCore/Json/MinJson.h"

namespace LottoSystem
{

	GLLottoSystemManClient::GLLottoSystemManClient()
		: m_pGaeaClient( NULL )
		, m_gmCommandLottoSystemId( 0 ) { }
	GLLottoSystemManClient::~GLLottoSystemManClient() { }

	void GLLottoSystemManClient::Initialize()
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return;

		m_gmCommandLottoSystemId = 0;

		for( unsigned int i=0; i<m_pLottoSystems.size(); ++i )
		{
			CLottoSystemClient* pLottoSystem = m_pLottoSystems.at( i );
			if( pLottoSystem )
			{
				pLottoSystem->Initialize();
			}
		}
	}

	void GLLottoSystemManClient::Destroy()
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return;

		for( unsigned int i=0; i<m_pLottoSystems.size(); ++i )
		{
			CLottoSystemClient* pLottoSystem = m_pLottoSystems.at( i );
			if( pLottoSystem )
			{
				pLottoSystem->Destroy();
			}
		}

		for( unsigned int i=0; i<m_pLottoSystems.size(); ++i )
			SAFE_DELETE( m_pLottoSystems.at( i ) );

		m_pLottoSystems.clear();
	}

	bool GLLottoSystemManClient::Update( float fElapsedTime, const CTime& currentTime )
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return true;

		// Update Lotto System;
		for( unsigned int i=0; i<m_pLottoSystems.size(); ++i )
		{
			CLottoSystemClient* pLottoSystem = m_pLottoSystems.at( i );
			if( pLottoSystem )
			{
				// 업데이트;
				if( !pLottoSystem->Update( fElapsedTime, currentTime ) )
					return false;
			}
		}

		return true;
	}

	bool GLLottoSystemManClient::OnMessage( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID )
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return true;

		LOTTO_SYSTEM_ID lottoSystemId;

		// 메시지에서 LottoSystemId를 가져온다;
		lottoSystemId = pMsg->m_lottoSystemId;

		// 동기화 패킷;
		switch( pMsg->Type() )
		{
		case NET_MSG_GCTRL_LOTTO_SYSTEM_SYNC_GLOBAL_VALUE_AC:
			GLLottoGlobalDataClient::Instance()->SyncGlobalValue(
				static_cast<GLMSG::NET_LOTTO_MESSAGE_SYNC_GLOBAL_VALUE_AC*>( pMsg ) );
			return true;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_SYNC_STATUS_AC:
			if( lottoSystemId >= 0 && lottoSystemId < MAX_LOTTO_SYSTEM )
			{
				CLottoSystemClient* pLottoSystem =
					new CLottoSystemClient( lottoSystemId, m_pGaeaClient );
				bool bUse = false;
				for( unsigned int i=0; i<m_pLottoSystems.size(); ++i )
				{
					CLottoSystemClient* pLottoSystem = m_pLottoSystems.at( i );
					if( pLottoSystem && pLottoSystem->GetLottoSystemId() == lottoSystemId )
						bUse = true;
				}

				if( bUse )
				{
					SAFE_DELETE( pLottoSystem );
				}
				else
				{
					m_pLottoSystems.push_back( pLottoSystem );
				}
			}
			break;
		}

		// LottoSystemId 에 맞는 LottoSystem의 메시지를 처리한다;
		for( unsigned int i=0; i<m_pLottoSystems.size(); ++i )
		{
			CLottoSystemClient* pLottoSystem = m_pLottoSystems.at( i );
			if( pLottoSystem && pLottoSystem->GetLottoSystemId() == lottoSystemId )
			{
				if( !pLottoSystem->OnMessage( pMsg, dwClientID, dwGaeaID ) )
					return false;

				return true;
			}
		}

		return true;
	}

	void GLLottoSystemManClient::SendToAgentServer( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg )
	{
		if( !m_pGaeaClient )
			return;

		m_pGaeaClient->NETSENDTOAGENT( pMsg );
	}

	void GLLottoSystemManClient::SendToFieldServer( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg )
	{
		if( !m_pGaeaClient )
			return;

		m_pGaeaClient->NETSENDTOFIELD( pMsg );
	}

	const int GLLottoSystemManClient::GetConfirmBuyListCount()
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return 0;

		int nReturn = 0;
		for( unsigned int i=0; i<m_pLottoSystems.size(); ++i )
			nReturn += m_pLottoSystems.at( i )->GetConfirmBuyCount();

		return nReturn;
	}

	CLottoSystemClient* GLLottoSystemManClient::GetLottoSystemUsingTabIdx( unsigned int useIdx )
	{
		for( unsigned int i=0; i<m_pLottoSystems.size(); ++i )
		{
			CLottoSystemClient* pLottoSystem = m_pLottoSystems.at( i );
			if( pLottoSystem )
			{
				LOTTO_SYSTEM_CLIENT_VALUE sValue = GLLottoGlobalDataClient::Instance()
					->GetLottoSystemValue( pLottoSystem->GetLottoSystemId() );
				if( sValue.useIndex == useIdx )
					return pLottoSystem;
			}
		}

		return NULL;
	}

	bool GLLottoSystemManClient::GMCommandSetLottoSystemID( unsigned int lottoSystemId )
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return false;

		LOTTO_SYSTEM_CLIENT_VALUE sValue =
			GLLottoGlobalDataClient::Instance()
			->GetLottoSystemValue( lottoSystemId );

		// 사용 여부 조사;
		if( !sValue.bUse )
			return false;
		
		m_gmCommandLottoSystemId = lottoSystemId;

		return true;
	}

	bool GLLottoSystemManClient::GMCommandViewLottoSystemID()
	{

		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return false;

		const unsigned int lottoSystemId =
			LottoSystem::GLLottoSystemManClient::Instance()->
			GetGmCommandLottoSystemId();

		ConsoleLog( sc::string::format(
			"Lotto System Id is %d.", lottoSystemId ).c_str() );

		for( unsigned int i=0; i<m_pLottoSystems.size(); ++i )
		{
			CLottoSystemClient* pLottoSystem = m_pLottoSystems.at( i );
			if( pLottoSystem )
			{
				unsigned int systemId = pLottoSystem->GetLottoSystemId();

				LOTTO_SYSTEM_CLIENT_VALUE sValue =
					GLLottoGlobalDataClient::Instance()
					->GetLottoSystemValue( systemId );

				// 사용 여부 조사;
				if( sValue.bUse )
				{
					ConsoleLog( sc::string::format(
						"Lotto System Id %d can use.", systemId ).c_str() );
				}
			}
		}

		return true;
	}

	bool GLLottoSystemManClient::GMCommandChangeState()
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return false;

		LOTTO_SYSTEM_CLIENT_VALUE sValue =
			GLLottoGlobalDataClient::Instance()
			->GetLottoSystemValue( m_gmCommandLottoSystemId );

		// 사용 여부 조사;
		if( !sValue.bUse )
			return false;

		GLMSG::NET_LOTTO_MESSAGE_CHANGE_STATE_CA msg( m_gmCommandLottoSystemId );			
		SendToAgentServer( &msg );

		return true;
	}

	bool GLLottoSystemManClient::GMCommandNumDSP( unsigned int num )
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return false;

		LOTTO_SYSTEM_CLIENT_VALUE sValue =
			GLLottoGlobalDataClient::Instance()
			->GetLottoSystemValue( m_gmCommandLottoSystemId );

		// 사용 여부 조사;
		if( !sValue.bUse )
			return false;

		DWORD dwDbNum = m_pGaeaClient->GETMYDBNUM();

		GLMSG::NET_LOTTO_MESSAGE_NUM_DSP_CA msg(
			m_gmCommandLottoSystemId, dwDbNum, num );			
		SendToAgentServer( &msg );

		return true;
	}

	bool GLLottoSystemManClient::GMCommandNum( unsigned int num )
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return false;

		LOTTO_SYSTEM_CLIENT_VALUE sValue =
			GLLottoGlobalDataClient::Instance()
			->GetLottoSystemValue( m_gmCommandLottoSystemId );

		// 사용 여부 조사;
		if( !sValue.bUse )
			return false;

		if( sValue.drawingType == 1 )
		{
			m_pGaeaClient->PrintConsoleText(
				std::string( "Don't use this GMCommand, because Drawing type is invent." ) );
		}

		DWORD dwDbNum = m_pGaeaClient->GETMYDBNUM();

		GLMSG::NET_LOTTO_MESSAGE_NUM_CA msg(
			m_gmCommandLottoSystemId, dwDbNum, num );			
		SendToAgentServer( &msg );

		return true;
	}

	bool GLLottoSystemManClient::GMCommandNumInit()
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return false;

		LOTTO_SYSTEM_CLIENT_VALUE sValue =
			GLLottoGlobalDataClient::Instance()
			->GetLottoSystemValue( m_gmCommandLottoSystemId );

		// 사용 여부 조사;
		if( !sValue.bUse )
			return false;

		if( sValue.drawingType == 1 )
		{
			m_pGaeaClient->PrintConsoleText(
				std::string( "Don't use this GMCommand, because Drawing type is invent." ) );
		}

		DWORD dwDbNum = m_pGaeaClient->GETMYDBNUM();

		GLMSG::NET_LOTTO_MESSAGE_NUM_INIT_CA msg(
			m_gmCommandLottoSystemId, dwDbNum );			
		SendToAgentServer( &msg );

		return true;
	}

	bool GLLottoSystemManClient::GMCommandNumAdd( unsigned int num, unsigned int count )
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return false;

		LOTTO_SYSTEM_CLIENT_VALUE sValue =
			GLLottoGlobalDataClient::Instance()
			->GetLottoSystemValue( m_gmCommandLottoSystemId );

		// 사용 여부 조사;
		if( !sValue.bUse )
			return false;

		if( sValue.drawingType == 1 )
		{
			m_pGaeaClient->PrintConsoleText(
				std::string( "Don't use this GMCommand, because Drawing type is invent." ) );
		}

		DWORD dwDbNum = m_pGaeaClient->GETMYDBNUM();

		GLMSG::NET_LOTTO_MESSAGE_NUM_ADD_CA msg(
			m_gmCommandLottoSystemId, dwDbNum, num, count );
		SendToAgentServer( &msg );

		return true;
	}

	bool GLLottoSystemManClient::GMCommandNumDel( unsigned int num, unsigned int count )
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return false;

		LOTTO_SYSTEM_CLIENT_VALUE sValue =
			GLLottoGlobalDataClient::Instance()
			->GetLottoSystemValue( m_gmCommandLottoSystemId );

		// 사용 여부 조사;
		if( !sValue.bUse )
			return false;

		if( sValue.drawingType == 1 )
		{
			m_pGaeaClient->PrintConsoleText(
				std::string( "Don't use this GMCommand, because Drawing type is invent." ) );
		}

		DWORD dwDbNum = m_pGaeaClient->GETMYDBNUM();

		GLMSG::NET_LOTTO_MESSAGE_NUM_DEL_CA msg(
			m_gmCommandLottoSystemId, dwDbNum, num, count );
		SendToAgentServer( &msg );

		return true;
	}

	bool GLLottoSystemManClient::GMCommandViewDrawingType()
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return false;

		LOTTO_SYSTEM_CLIENT_VALUE sValue =
			GLLottoGlobalDataClient::Instance()
			->GetLottoSystemValue( m_gmCommandLottoSystemId );

		// 사용 여부 조사;
		if( !sValue.bUse )
			return false;

		CLottoSystemClient* pLottoSystem =
			GLLottoSystemManClient::Instance()
			->GetLottoSystem( m_gmCommandLottoSystemId );
		if( pLottoSystem )
		{
			switch( sValue.drawingType )
			{
			case 0:
				// 랜덤 타입의 경우;
				pLottoSystem->AddEvent( EVIEW_RANDOM_TYPE );
				break;
			case 1:
				// 조작 가능 타입의 경우;
				pLottoSystem->AddEvent( EVIEW_INVENT_TYPE );
				break;
			}
		}

		return true;
	}

	bool GLLottoSystemManClient::GMCommandSetWinNum( const std::string& strWinNum )
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return false;

		LOTTO_SYSTEM_CLIENT_VALUE sValue =
			GLLottoGlobalDataClient::Instance()
			->GetLottoSystemValue( m_gmCommandLottoSystemId );

		// 사용 여부 조사;
		if( !sValue.bUse )
			return false;

		LottoSystem::LOTTO_SYSTEM_LOTTO_NUM winNum;

		boost::char_separator<char> sep("|");
		tokenizer tok( strWinNum, sep );

		unsigned int count = 0;
		for( tokenizer::iterator it=tok.begin(); it!=tok.end(); ++it )
		{
			if( count >= 0 && count < LottoSystem::MAX_DRAWING_NUM_LIST )
				winNum.lottoNum[ count++ ] = atoi( (*it).c_str() );
		}


		if( count == sValue.nDrawingTotal )
		{
			GLMSG::NET_LOTTO_MESSAGE_SET_INVENT_WIN_NUM_CA msg(
				m_gmCommandLottoSystemId );
			msg.m_winnerNum = winNum;
			SendToAgentServer( &msg );

			// 올바른 입력;
			CLottoSystemClient* pLottoSystem = GLLottoSystemManClient::Instance()
				->GetLottoSystem( m_gmCommandLottoSystemId );
			if( pLottoSystem )
			{
				json_spirit::Object inputObj;
				inputObj.push_back( json_spirit::Pair( "id", static_cast<int>( EVIEW_CORRECT_SET_WIN_NUM ) ) );
				inputObj.push_back( json_spirit::Pair( "winNum", strWinNum ) );

				std::string inputString = json_spirit::write( inputObj );

				pLottoSystem->AddJsonEvent( inputString );
			}
		}
		else
		{
			// 잘못된 입력;
			CLottoSystemClient* pLottoSystem = GLLottoSystemManClient::Instance()
				->GetLottoSystem( m_gmCommandLottoSystemId );
			if( pLottoSystem )
			{
				pLottoSystem->AddEvent( EVIEW_INCORRECT_SET_WIN_NUM );
			}
		}

		return true;
	}

	bool GLLottoSystemManClient::GMCommandResetWinNum()
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return false;

		LOTTO_SYSTEM_CLIENT_VALUE sValue =
			GLLottoGlobalDataClient::Instance()
			->GetLottoSystemValue( m_gmCommandLottoSystemId );

		// 사용 여부 조사;
		if( !sValue.bUse )
			return false;

		GLMSG::NET_LOTTO_MESSAGE_RESET_INVENT_WIN_NUM_CA msg(
			m_gmCommandLottoSystemId );
		SendToAgentServer( &msg );

		return true;
	}

	bool GLLottoSystemManClient::GMCommandCantBuy()
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return false;

		LOTTO_SYSTEM_CLIENT_VALUE sValue =
			GLLottoGlobalDataClient::Instance()
			->GetLottoSystemValue( m_gmCommandLottoSystemId );

		// 사용 여부 조사;
		if( !sValue.bUse )
			return false;

		GLMSG::NET_LOTTO_MESSAGE_LOCK_BUY_LOTTO_CA msg(
			m_gmCommandLottoSystemId, true );
		SendToAgentServer( &msg );

		return true;
	}

	bool GLLottoSystemManClient::GMCommandCanBuy()
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return false;

		LOTTO_SYSTEM_CLIENT_VALUE sValue =
			GLLottoGlobalDataClient::Instance()
			->GetLottoSystemValue( m_gmCommandLottoSystemId );

		// 사용 여부 조사;
		if( !sValue.bUse )
			return false;

		GLMSG::NET_LOTTO_MESSAGE_LOCK_BUY_LOTTO_CA msg(
			m_gmCommandLottoSystemId, false );
		SendToAgentServer( &msg );

		return true;
	}

	bool GLLottoSystemManClient::GMCommandChangeTurnOn()
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return false;

		LOTTO_SYSTEM_CLIENT_VALUE sValue =
			GLLottoGlobalDataClient::Instance()
			->GetLottoSystemValue( m_gmCommandLottoSystemId );

		// 사용 여부 조사;
		if( !sValue.bUse )
			return false;

		GLMSG::NET_LOTTO_MESSAGE_LOCK_CHANGE_TURN_LOTTO_CA msg(
			m_gmCommandLottoSystemId, true );
		SendToAgentServer( &msg );

		return true;
	}

	bool GLLottoSystemManClient::GMCommandChangeTurnOff()
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return false;

		LOTTO_SYSTEM_CLIENT_VALUE sValue =
			GLLottoGlobalDataClient::Instance()
			->GetLottoSystemValue( m_gmCommandLottoSystemId );

		// 사용 여부 조사;
		if( !sValue.bUse )
			return false;

		GLMSG::NET_LOTTO_MESSAGE_LOCK_CHANGE_TURN_LOTTO_CA msg(
			m_gmCommandLottoSystemId, false );
		SendToAgentServer( &msg );

		return true;
	}

	bool GLLottoSystemManClient::GMCommandBuyTicket()
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return false;

		/*if( !GLUseFeatures::GetInstance().IsUsingLottoSystemDevelopMode() )
			return false;*/

		LOTTO_SYSTEM_CLIENT_VALUE sValue =
			GLLottoGlobalDataClient::Instance()
			->GetLottoSystemValue( m_gmCommandLottoSystemId );

		// 사용 여부 조사;
		if( !sValue.bUse )
			return false;

		for( unsigned int i=0; i<m_pLottoSystems.size(); ++i )
		{
			CLottoSystemClient* pLottoSystem = m_pLottoSystems.at( i );
			if( pLottoSystem && pLottoSystem->GetLottoSystemId() == m_gmCommandLottoSystemId )
				pLottoSystem->BuyLottoAuto();
		}

		return true;
	}

	bool GLLottoSystemManClient::GMCommandAccumulateMoney()
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return false;

		if( !GLUseFeatures::GetInstance().IsUsingLottoSystemDevelopMode() )
			return false;

		LOTTO_SYSTEM_CLIENT_VALUE sValue =
			GLLottoGlobalDataClient::Instance()
			->GetLottoSystemValue( m_gmCommandLottoSystemId );

		// 사용 여부 조사;
		if( !sValue.bUse )
			return false;

		for( unsigned int i=0; i<m_pLottoSystems.size(); ++i )
		{
			CLottoSystemClient* pLottoSystem = m_pLottoSystems.at( i );
			if( pLottoSystem && pLottoSystem->GetLottoSystemId() == m_gmCommandLottoSystemId )
				pLottoSystem->UpdateAccumulateMoney();
		}

		return true;
	}

	bool GLLottoSystemManClient::GMCommandViewBuyList()
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return false;

		if( !GLUseFeatures::GetInstance().IsUsingLottoSystemDevelopMode() )
			return false;

		LOTTO_SYSTEM_CLIENT_VALUE sValue =
			GLLottoGlobalDataClient::Instance()
			->GetLottoSystemValue( m_gmCommandLottoSystemId );

		// 사용 여부 조사;
		if( !sValue.bUse )
			return false;

		for( unsigned int i=0; i<m_pLottoSystems.size(); ++i )
		{
			CLottoSystemClient* pLottoSystem = m_pLottoSystems.at( i );
			if( pLottoSystem && pLottoSystem->GetLottoSystemId() == m_gmCommandLottoSystemId )
				pLottoSystem->UpdateBuyList();
		}

		return true;
	}

	bool GLLottoSystemManClient::GMCommandViewDrawingDate()
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return false;

		if( !GLUseFeatures::GetInstance().IsUsingLottoSystemDevelopMode() )
			return false;

		LOTTO_SYSTEM_CLIENT_VALUE sValue =
			GLLottoGlobalDataClient::Instance()
			->GetLottoSystemValue( m_gmCommandLottoSystemId );

		// 사용 여부 조사;
		if( !sValue.bUse )
			return false;

		for( unsigned int i=0; i<m_pLottoSystems.size(); ++i )
		{
			CLottoSystemClient* pLottoSystem = m_pLottoSystems.at( i );
			if( pLottoSystem && pLottoSystem->GetLottoSystemId() == m_gmCommandLottoSystemId )
				pLottoSystem->UpdateDrawingDate();
		}

		return true;
	}

	bool GLLottoSystemManClient::GMCommandViewWinNum()
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return false;

		if( !GLUseFeatures::GetInstance().IsUsingLottoSystemDevelopMode() )
			return false;

		LOTTO_SYSTEM_CLIENT_VALUE sValue =
			GLLottoGlobalDataClient::Instance()
			->GetLottoSystemValue( m_gmCommandLottoSystemId );

		// 사용 여부 조사;
		if( !sValue.bUse )
			return false;

		for( unsigned int i=0; i<m_pLottoSystems.size(); ++i )
		{
			CLottoSystemClient* pLottoSystem = m_pLottoSystems.at( i );
			if( pLottoSystem && pLottoSystem->GetLottoSystemId() == m_gmCommandLottoSystemId )
				pLottoSystem->UpdateWinNum();
		}

		return true;
	}

	bool GLLottoSystemManClient::GMCommandViewWinManList()
	{
		if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
			return false;

		if( !GLUseFeatures::GetInstance().IsUsingLottoSystemDevelopMode() )
			return false;

		LOTTO_SYSTEM_CLIENT_VALUE sValue =
			GLLottoGlobalDataClient::Instance()
			->GetLottoSystemValue( m_gmCommandLottoSystemId );

		// 사용 여부 조사;
		if( !sValue.bUse )
			return false;

		for( unsigned int i=0; i<m_pLottoSystems.size(); ++i )
		{
			CLottoSystemClient* pLottoSystem = m_pLottoSystems.at( i );
			if( pLottoSystem && pLottoSystem->GetLottoSystemId() == m_gmCommandLottoSystemId )
				pLottoSystem->UpdateWinManList();
		}

		return true;
	}

	CLottoSystemClient* GLLottoSystemManClient::FindSystem( unsigned int nID )
	{
		for( size_t i = 0; i < m_pLottoSystems.size(); ++i )
		{
			if( m_pLottoSystems[ i ]->GetLottoSystemId() == nID )
				return m_pLottoSystems[ i ];
		}

		return NULL;
	}

	LuaTable GLLottoSystemManClient::GetPrevTurnList( unsigned int nID )
	{
		LuaTable tbList( GLWidgetScript::GetInstance().GetLuaState() );

		CLottoSystemClient* pLottoSystem = FindSystem( nID );
		if( NULL == pLottoSystem )
			return tbList;		

		const LottoSystem::LOTTO_GLOBAL_VALUE& value = GLLottoGlobalDataClient::Instance()->GetLottoGlobalValue();

		int nPrevTurn = pLottoSystem->GetCurTurnNum() - 1;

		if( 0 <= nPrevTurn )
		{
			int nTurn = nPrevTurn - value.nViewList - 1;
			if( 0 > nTurn )
				nTurn = 0;

			int nTableIndex = 1;
			for( int i = nPrevTurn; i >= nTurn; --i )
				tbList.set( nTableIndex++, i );
		}

		return tbList;
	}

	LuaTable GLLottoSystemManClient::GetPrevTurnWinnerList( unsigned int nID )
	{		
		LuaTable tbList( GLWidgetScript::GetInstance().GetLuaState() );

		CLottoSystemClient* pLottoSystem = FindSystem( nID );
		if( NULL == pLottoSystem )
			return tbList;

		const LOTTO_SYSTEM_WINNER_MAN_LIST& winnerList = pLottoSystem->GetWinnerManList();

		int nTableIndex = 1;
		for( size_t i = 0; i < winnerList.winnerManList.size(); ++i )
		{
			LuaTable tbWinner( GLWidgetScript::GetInstance().GetLuaState() );
			tbWinner.set( 1, winnerList.winnerManList[ i ].ranking );

			if( i < winnerList.winnerManNameList.size() )
				tbWinner.set( 2, winnerList.winnerManNameList[ i ].c_str() );
			else
				tbWinner.set( 2, "" );

			tbWinner.set( 3, winnerList.winnerManList[ i ].winCount );
			tbWinner.set( 4, winnerList.winnerManList[ i ].winMoney );

			tbList.set( nTableIndex++, tbWinner );
		}

		return tbList;
	}

	LONGLONG GLLottoSystemManClient::GetAccumulateMoney( unsigned int nID )
	{
		CLottoSystemClient* pSystem = FindSystem( nID );
		if( NULL == pSystem )
			return 0;

		return pSystem->GetAccumulateMoney();
	}

	int GLLottoSystemManClient::GetCurTurnNum( unsigned int nID )
	{
		CLottoSystemClient* pSystem = FindSystem( nID );
		if( NULL == pSystem )
			return 0;

		return pSystem->GetCurTurnNum();
	}

	LuaTable GLLottoSystemManClient::GetStartTime( unsigned int nID )
	{	
		LuaTable tbTime( GLWidgetScript::GetInstance().GetLuaState() );

		CLottoSystemClient* pSystem = FindSystem( nID );
		if( NULL == pSystem )
			return tbTime;

		const LOTTO_DRAWING_DATE& sTime = pSystem->GetStartTime();
		tbTime.set( 1, sTime.sTime.GetDay() );
		tbTime.set( 2, sTime.sTime.GetHour() );
		tbTime.set( 3, sTime.sTime.GetMinute() );
		tbTime.set( 4, sTime.emDrawingDayOfTheWeek );

		return tbTime;
	}

	int GLLottoSystemManClient::GetBuyCountMax( unsigned int nID )
	{
		LottoSystem::LOTTO_SYSTEM_CLIENT_VALUE sValue =
			LottoSystem::GLLottoGlobalDataClient::Instance()->GetLottoSystemValue( nID );

		return sValue.buyMultiNum;
	}

	int GLLottoSystemManClient::GetTotalNum ( unsigned int nID )
	{
		LottoSystem::LOTTO_SYSTEM_CLIENT_VALUE sValue =
			LottoSystem::GLLottoGlobalDataClient::Instance()->GetLottoSystemValue( nID );

		return sValue.nLottoTotal;
	}

	int GLLottoSystemManClient::GetBuyCount( unsigned int nID, bool bConfirm )
	{
		CLottoSystemClient* pSystem = FindSystem( nID );
		if( NULL == pSystem )
			return 0;

		if( true == bConfirm )
			return pSystem->GetConfirmBuyCount();
		else
			return pSystem->GetBuyCount();
	}

	bool GLLottoSystemManClient::GetLockBuyState( unsigned int nID )
	{
		CLottoSystemClient* pSystem = FindSystem( nID );
		if( NULL == pSystem )
			return false;

		return pSystem->GetLockBuyState();
	}

	LONGLONG GLLottoSystemManClient::GetBuyCost( unsigned int nID )
	{
		LOTTO_SYSTEM_CLIENT_VALUE sValue =
			GLLottoGlobalDataClient::Instance()->GetLottoSystemValue( nID );	

		return sValue.lottoTicketMoney;
	}

	LuaTable GLLottoSystemManClient::GetAutoNum( unsigned int nID )
	{
		LuaTable tbNum( GLWidgetScript::GetInstance().GetLuaState() );

		CLottoSystemClient* pSystem = FindSystem( nID );
		if( NULL == pSystem )
			return tbNum;

		LOTTO_SYSTEM_LOTTO_NUM sAutoNum = pSystem->MakeAutoLottoNum();
		for( int i = 0; i < MAX_DRAWING_NUM_LIST; ++i )
			tbNum.set( i + 1, sAutoNum.lottoNum[ i ] );

		return tbNum;
	}
	
	LuaTable GLLottoSystemManClient::GetBuyList( unsigned int nID, bool bConfirm )
	{
		LuaTable tbList( GLWidgetScript::GetInstance().GetLuaState() );

		CLottoSystemClient* pSystem = FindSystem( nID );
		if( NULL == pSystem )
			return tbList;

		std::vector< LOTTO_SYSTEM_BUY > vecList;
		if( true == bConfirm )
			vecList = pSystem->GetConfirmBuyList().buyList;
		else
			vecList = pSystem->GetBuyList().buyList;
		
		std::vector< LOTTO_SYSTEM_BUY >::const_iterator iter = vecList.begin();
		std::vector< LOTTO_SYSTEM_BUY >::const_iterator iterEnd = vecList.end();

		int nTableIndex = 1;
		for( ; iter != iterEnd; ++iter )
		{
			const LOTTO_SYSTEM_BUY& buy = (*iter);

			LuaTable tbBuy( GLWidgetScript::GetInstance().GetLuaState() );

			LuaTable tbNumList( GLWidgetScript::GetInstance().GetLuaState() );
			for( int j = 0; j < MAX_DRAWING_NUM_LIST; ++j )
				tbNumList.set( j + 1, buy.drawingNum[ j ] );

			tbBuy.set( 1, EMLOTTO_BUY_TYPE::AUTO_BUY == buy.lottoBuyType ? true : false );
			tbBuy.set( 2, tbNumList );

			tbList.set( nTableIndex++, tbBuy );
		}

		return tbList;
	}

	LuaTable GLLottoSystemManClient::GetConfirmNum( unsigned int nID )
	{
		LuaTable tbNum( GLWidgetScript::GetInstance().GetLuaState() );

		CLottoSystemClient* pSystem = FindSystem( nID );
		if( NULL == pSystem )
			return tbNum;

		const LOTTO_SYSTEM_LOTTO_NUM& sNum = pSystem->GetConfirmWinnerNum();

		for( int i = 0; i < MAX_DRAWING_NUM_LIST; ++i )
			tbNum.set( i + 1, sNum.lottoNum[ i ] );

		return tbNum;
	}

	void GLLottoSystemManClient::ReqPreWinNum( unsigned int nID, unsigned int nTurnNum )
	{
		CLottoSystemClient* pSystem = FindSystem( nID );
		if( NULL == pSystem )
			return;

		pSystem->ReqPreWinNum( nTurnNum );
	}

	void GLLottoSystemManClient::ReqPreWinManList( unsigned int nID, unsigned int nTurnNum )
	{
		CLottoSystemClient* pSystem = FindSystem( nID );
		if( NULL == pSystem )
			return;

		pSystem->ReqPreWinManList( nTurnNum );
	}

	void GLLottoSystemManClient::ReqPreAccumulateMoney( unsigned int nID, unsigned int nTurnNum )
	{
		CLottoSystemClient* pSystem = FindSystem( nID );
		if( NULL == pSystem )
			return;

		pSystem->ReqPreAccumulateMoney( nTurnNum );
	}

	CLottoSystemClient::BUY_LOTTO_FLAG GLLottoSystemManClient::ReqBuyLotto( unsigned int nID, lua_tinker::table tbNum, bool bAuto )
	{
		LOTTO_SYSTEM_CLIENT_VALUE sValue =
			GLLottoGlobalDataClient::Instance()
			->GetLottoSystemValue( nID );

		CLottoSystemClient* pSystem = FindSystem( nID );
		if( NULL == pSystem )
			return CLottoSystemClient::BUY_LOTTO_FLAG_FAIL;

		unsigned int nNewCount = MAX_DRAWING_NUM_LIST - 1;
		if( nNewCount != tbNum.TableLen() )
			return CLottoSystemClient::BUY_LOTTO_FLAG_FAIL;

		LOTTO_SYSTEM_LOTTO_NUM sNum;
		for( int i = 0; i < nNewCount; ++i )
			sNum.lottoNum[ i ] = tbNum.get< unsigned int >( i + 1 );

		const unsigned int nCurBuyCount = pSystem->GetBuyCount();
		const unsigned int nMaxBuyCount = sValue.buyMultiNum;

		CLottoSystemClient::BUY_LOTTO_FLAG buyLottoFlag = CLottoSystemClient::BUY_LOTTO_FLAG_FAIL;
		if ( nCurBuyCount < nMaxBuyCount )
			buyLottoFlag = pSystem->BuyLottoTicket( sNum, true == bAuto ? EMLOTTO_BUY_TYPE::AUTO_BUY : EMLOTTO_BUY_TYPE::MANUAL_BUY );

		return buyLottoFlag;
	}

	CLottoSystemClient::BUY_LOTTO_FLAG GLLottoSystemManClient::ReqBuyLottoList ( unsigned int nID, unsigned int nCount )
	{
		LOTTO_SYSTEM_CLIENT_VALUE sValue =
			GLLottoGlobalDataClient::Instance()
			->GetLottoSystemValue( nID );

		if( false == sValue.bUse )
			return CLottoSystemClient::BUY_LOTTO_FLAG_FAIL;

		CLottoSystemClient* pSystem = FindSystem( nID );
		if( NULL == pSystem )
			return CLottoSystemClient::BUY_LOTTO_FLAG_FAIL;

		const unsigned int nCurBuyCount = pSystem->GetBuyCount();
		const unsigned int nMaxBuyCount = sValue.buyMultiNum;
		unsigned int nRemainBuyCount = nMaxBuyCount - nCurBuyCount;
		if ( nRemainBuyCount < nCount )
			nCount = nRemainBuyCount;

		CLottoSystemClient::BUY_LOTTO_FLAG buyLottoFlag = CLottoSystemClient::BUY_LOTTO_FLAG_FAIL;
		for ( unsigned int i=0; i<nCount; ++i )
		{
			// 구매에 실패하면 중지한다;
			buyLottoFlag = pSystem->BuyLottoAuto();
			if ( CLottoSystemClient::BUY_LOTTO_FLAG_OK != buyLottoFlag )
				break;
		}

		return buyLottoFlag;
	}

	void GLLottoSystemManClient::ReqPreBuyList( unsigned int nID, unsigned int nTurnNum )
	{
		CLottoSystemClient* pSystem = FindSystem( nID );
		if( NULL == pSystem )
			return;

		pSystem->ReqPreBuyList( nTurnNum );
	}

	void GLLottoSystemManClient::ReqConfirmBuyList(unsigned int nID)
	{
		CLottoSystemClient* pSystem = FindSystem( nID );
		if( NULL == pSystem )
			return;

		pSystem->ConfirmBuyList();
	}

	int GLLottoSystemManClient::GetLottoSystemState ( unsigned int nID )
	{
		CLottoSystemClient* pSystem = FindSystem( nID );
		if( NULL == pSystem )
			return 0;

		return pSystem->GetLottoSystemState();
	}

	int GLLottoSystemManClient::GetViewNumCharName ()
	{
		LottoSystem::LOTTO_GLOBAL_VALUE gValue =
			LottoSystem::GLLottoGlobalDataClient::Instance()->GetLottoGlobalValue();

		return gValue.nViewCharName;
	}
}