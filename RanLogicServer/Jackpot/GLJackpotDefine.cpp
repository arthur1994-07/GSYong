#include "pch.h"

#include "../../SigmaCore/Lua/MinLua.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringUtil.h"
#include "../../SigmaCore/Util/MinMessageBox.h"

#include "../../SigmaCore/gassert.h"

#include "../../RanLogic/Msg/ServerMsg.h"

#include "../Server/AgentServer.h"

#include "../AgentServer/GLAgentServer.h"

#include "./GLJackpotDefine.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


GLJackpotMgr::GLJackpotMgr( GLAgentServer* pServer )
: m_pServer( pServer )
, m_Use( false )
{
}

GLJackpotMgr::~GLJackpotMgr()
{

}

bool GLJackpotMgr::Load( const std::string& strFileName )
{
	m_strFilePath = strFileName;

	try
	{   
		sc::lua_init();
		if ( !sc::lua_doFile( m_strFilePath ) )
		{
			std::string strErrMsg( sc::string::format( "%1% file missing.(error 1247)", m_strFilePath ) );
			sc::ErrorMessageBox( strErrMsg, false );
			return false;
		}

		loadCommon();
		loadServer();

		return true;
	}
	catch ( const LuaPlus::LuaException& e )
	{
		std::string strErrMsg( sc::string::format( "%1%, %2%", m_strFilePath, e.GetErrorMessage() ) );
		sc::ErrorMessageBox( strErrMsg, false );
		return false;
	} 
}

void GLJackpotMgr::AddUserDefinedJackpot( JACKPOT& Jackpot )
{
	Jackpot.FromFileLoad = false;
	adjustTimer( Jackpot.DelaySeconds, Jackpot.Timer );
	m_vecJackpot.push_back( Jackpot );
}

void GLJackpotMgr::DeleteUserDefinedJackpot()
{
	for ( std::vector< JACKPOT >::iterator iter = m_vecJackpot.begin(); iter != m_vecJackpot.end(); )
	{
		JACKPOT& Jackpot = *iter;
		if ( false == Jackpot.FromFileLoad  )
			iter = m_vecJackpot.erase( iter );
		else
			++iter;
	}
}

void GLJackpotMgr::FrameMove( float ElapsedAppTime )
{
	if ( false == m_Use )
	{
		return;
	}

	for ( std::vector< JACKPOT >::size_type i = 0; i < m_vecJackpot.size(); ++i )
	{
		JACKPOT& Jackpot = m_vecJackpot[i];
		if ( Jackpot.Timer > Jackpot.DelaySeconds )
		{
			// 잭팟 시간 조정
			adjustTimer( Jackpot.DelaySeconds, Jackpot.Timer );

			GLMSG::NET_JACKPOT_PUSH NetMsg;
			NetMsg.ItemID = Jackpot.ItemID;
			NetMsg.RequiredLevel = Jackpot.RequiredLevel;
			NetMsg.GameMoney = Jackpot.GameMoney;
			NetMsg.bMain = Jackpot.bMain;

			CAgentServer* pServer = static_cast< CAgentServer* >( m_pServer->GetMsgServer() );
			DWORD FieldSlot = pServer->GetRandomFieldSlot();
			if ( net::INVALID_SLOT != FieldSlot )
			{
				m_pServer->SENDTOCLIENT( FieldSlot, &NetMsg );
			}
			else
			{
				sc::writeLogError( "GLJackpotMgr::FrameMove FieldSlot is net::INVALID_SLOT" );
			}
		}

		Jackpot.Timer += ElapsedAppTime;
	}
}

bool GLJackpotMgr::loadCommon()
{
	return true;
}

bool GLJackpotMgr::loadServer()
{
	try
	{
		m_vecJackpot.clear();

		LuaPlus::LuaObject server = sc::lua_getGlobalFromName( "server" );
		for (LuaPlus::LuaTableIterator Iter( server ); Iter; Iter.Next() )
		{
			std::string strKey( ( const char* ) Iter.GetKey().GetString() );

			if ( strKey == "Use" )
				m_Use = Iter.GetValue().GetBoolean();
			else if ( strKey == "MainItem" )
			{
				for ( LuaPlus::LuaTableIterator Iter2( Iter.GetValue() ); Iter2; Iter2.Next() )
				{
					for ( LuaPlus::LuaTableIterator Iter3( Iter2.GetValue() ); Iter3; Iter3.Next() )
					{
						/*
						float DelaySeconds;
						SNATIVEID ItemID;
						SNATIVEID RequiredLevel;
						DWORD GameMoney;
						*/
						JACKPOT Jackpot;

						Jackpot.DelaySeconds = Iter3.GetValue().GetFloat();
						Iter3.Next();
						
						Jackpot.ItemID.wMainID = static_cast< WORD >( Iter3.GetValue().GetInteger() );
						Iter3.Next();
						Jackpot.ItemID.wSubID = static_cast< WORD >( Iter3.GetValue().GetInteger() );
						Iter3.Next();
						
						Jackpot.RequiredLevel.wMainID = static_cast< WORD >( Iter3.GetValue().GetInteger() );
						Iter3.Next();
						Jackpot.RequiredLevel.wSubID = static_cast< WORD >( Iter3.GetValue().GetInteger() );
						Iter3.Next();

						Jackpot.GameMoney = static_cast< DWORD >( Iter3.GetValue().GetInteger64() );
						
						// 잭팟 시간 조정
						adjustTimer( Jackpot.DelaySeconds, Jackpot.Timer );

						m_vecJackpot.push_back( Jackpot );
					}
				}
			}
			else if ( strKey == "SubItem" )
			{
				for ( LuaPlus::LuaTableIterator Iter2( Iter.GetValue() ); Iter2; Iter2.Next() )
				{
					for ( LuaPlus::LuaTableIterator Iter3( Iter2.GetValue() ); Iter3; Iter3.Next() )
					{
						/*
						float DelaySeconds;
						SNATIVEID ItemID;
						SNATIVEID RequiredLevel;
						DWORD GameMoney;
						*/
						JACKPOT Jackpot;

						Jackpot.DelaySeconds = Iter3.GetValue().GetFloat();
						Iter3.Next();
						
						Jackpot.ItemID.wMainID = static_cast< WORD >( Iter3.GetValue().GetInteger() );
						Iter3.Next();
						Jackpot.ItemID.wSubID = static_cast< WORD >( Iter3.GetValue().GetInteger() );
						Iter3.Next();
						
						Jackpot.RequiredLevel.wMainID = static_cast< WORD >( Iter3.GetValue().GetInteger() );
						Iter3.Next();
						Jackpot.RequiredLevel.wSubID = static_cast< WORD >( Iter3.GetValue().GetInteger() );
						Iter3.Next();

						Jackpot.GameMoney = static_cast< DWORD >( Iter3.GetValue().GetInteger64() );
						
						// 잭팟 시간 조정
						adjustTimer( Jackpot.DelaySeconds, Jackpot.Timer );

						Jackpot.bMain = false;

						m_vecJackpot.push_back( Jackpot );
					}
				}
			}
		}
		return true;
	}
	catch ( const LuaPlus::LuaException& e )
	{
		std::string strErrMsg( sc::string::format( "%1%, %2%", m_strFilePath, e.GetErrorMessage() ) );
		sc::ErrorMessageBox( strErrMsg, false );
		return false;
	}
}

void GLJackpotMgr::adjustTimer( float& Delay, float& Timer )
{
	/*
	// 다음 잭팟 시간을 일정 간격으로 랜덤하게 조정하기 위한 용도
	// RandomSecond 는 0.1 ~ 100.0 사이의 값이 나온다.
	// 그 값을 일정 확률로 - 혹은 + 시킨다.
	float RandomSecond = ( sc::Random::getInstance().GetFloatPercent() + 0.1f );
	if ( sc::Random::getInstance().IsSucceed( 50.0f ) )
	{
		Timer += RandomSecond;
	}
	else
	{
		Timer -= RandomSecond;
	}
	*/

	Timer = 0.0f;

	// 딜레이 시간의 1%~5% 값으로 타이머 초기값을 설정한다. 그 값은 +일수도 -일수도 있다. 
	float RandomPercent = sc::Random::getInstance().RandomNumber( 0.01f, 0.05f );
	if ( RandomPercent > Delay )
	{
		return;
	}

	float RandomSecond = Delay * RandomPercent;
	if ( sc::Random::getInstance().IsSucceed( 50.0f ) )
	{
		Timer += RandomSecond;
	}
	else
	{
		Timer -= RandomSecond;
	}

	/*sc::writeLogInfo( sc::string::format( "adjustTimer timer is %1%", Timer ) );*/
}
