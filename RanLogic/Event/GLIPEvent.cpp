#include "pch.h"

#pragma warning(disable:4267)
#include <boost/lexical_cast.hpp>
#include <boost/spirit/include/classic.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringUtil.h"
#include "../../SigmaCore/Util/MinMessageBox.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/Lua/MinLua.h"

#include "../../EngineLib/G-Logic/Glogic.h"

#include "./GLIPEvent.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const DWORD ran_event::GLIPEvent::MAX_BOUNDARY = 200;
const DWORD ran_event::GLIPEvent::MIN_BOUNDARY = 5;

ran_event::GLIPEvent::GLIPEvent()
{
    Init();
}

ran_event::GLIPEvent::~GLIPEvent()
{
}

void ran_event::GLIPEvent::Init()
{
    m_AttackValue = 0;
    m_AttackSpeed = 0;
    m_MoveSpeed = 0;
    m_ItemDrop = 0;
    m_MoneyDrop = 0;
    m_Exp = 0;
    m_HPRecovery = 0;
    m_MPRecovery = 0;
    m_SPRecovery = 0;
    m_AllRecovery = 0;
    m_CriticalRate = 0;

	m_EventIP.clear();
}

bool ran_event::GLIPEvent::Load( const std::string& strFileName, bool bServer )
{
    Init();

    std::string strPath(GLOGIC::GetServerPath());
    strPath.append(strFileName);

    m_strFilePath = strPath;

    try
    {   
        sc::lua_init();
        if ( !sc::lua_doFile( m_strFilePath ) )
        {
            std::string strErrMsg(
                sc::string::format(
                "%1% file missing.(error 9421)",
                m_strFilePath));
            sc::writeLogError( strErrMsg );
            return false;
        }

        LoadOptions();
        LoadIPs();

        return true;
    }
    catch ( const LuaPlus::LuaException& e )
    {
        std::string strErrMsg(
            sc::string::format(
            "%1%, %2%",
            m_strFilePath,
            e.GetErrorMessage()));
        sc::writeLogError( strErrMsg );

        return false;
    }

    return true;
}

bool ran_event::GLIPEvent::LoadOptions()
{
    try
	{        
		LuaPlus::LuaObject common = sc::lua_getGlobalFromName( "options" );
		for (LuaPlus::LuaTableIterator Iter( common ); Iter; Iter.Next() )
		{
			std::string strKey( ( const char* ) Iter.GetKey().GetString() );

			if ( strKey == "attack" )
            {
				m_AttackValue = static_cast< DWORD >( Iter.GetValue().GetFloat() );
                CheckBoundary( m_AttackValue );
            }
            else if ( strKey == "attack_speed" )
            {
                m_AttackSpeed = static_cast< DWORD >( Iter.GetValue().GetFloat() );
                CheckBoundary( m_AttackSpeed );
            }
            else if ( strKey == "move_speed" )
            {
                m_MoveSpeed = static_cast< DWORD >( Iter.GetValue().GetFloat() );
                CheckBoundary( m_MoveSpeed );
            }
            else if ( strKey == "item_drop" )
            {
                m_ItemDrop = static_cast< DWORD >( Iter.GetValue().GetFloat() );
                CheckBoundary( m_ItemDrop );
            }
            else if ( strKey == "money_drop" )
            {
                m_MoneyDrop = static_cast< DWORD >( Iter.GetValue().GetFloat() );
                CheckBoundary( m_MoneyDrop );
            }
            else if ( strKey == "exp" )
            {
                m_Exp = static_cast< DWORD >( Iter.GetValue().GetFloat() );
                CheckBoundary( m_Exp );
            }
            else if ( strKey == "hp_recovery" )
            {
                m_HPRecovery = static_cast< DWORD >( Iter.GetValue().GetFloat() );
                CheckBoundary( m_HPRecovery );
            }
            else if ( strKey == "mp_recovery" )
            {
                m_MPRecovery = static_cast< DWORD >( Iter.GetValue().GetFloat() );
                CheckBoundary( m_MPRecovery );
            }
            else if ( strKey == "sp_recovery" )
            {
                m_SPRecovery = static_cast< DWORD >( Iter.GetValue().GetFloat() );
                CheckBoundary( m_SPRecovery );
            }
            else if ( strKey == "all_recovery" )
            {
                m_AllRecovery = static_cast< DWORD >( Iter.GetValue().GetFloat() );
                CheckBoundary( m_AllRecovery );
            }
            else if ( strKey == "critical_rate" )
            {
                m_CriticalRate = static_cast< DWORD >( Iter.GetValue().GetFloat() );
                CheckBoundary( m_CriticalRate );
            }
		}

		return true;
	}
	catch ( const LuaPlus::LuaException& e )
	{
		std::string strErrMsg(
			sc::string::format(
			"%1%, %2%",
			m_strFilePath,
			e.GetErrorMessage()));
		sc::writeLogError( strErrMsg );

		return false;
	}

    return true;
}

bool ran_event::GLIPEvent::LoadIPs()
{
    try
    {
		LuaPlus::LuaObject ip = sc::lua_getGlobalFromName( "ip" );
		for ( LuaPlus::LuaTableIterator Iter( ip ); Iter; Iter.Next() )
		{
			for ( LuaPlus::LuaTableIterator IterData( Iter.GetValue() ); IterData; IterData.Next() )
			{
				std::wstring wstrStartIP( ( const wchar_t* ) IterData.GetValue().GetWString() );
				std::string strStartIP = sc::string::unicodeToAnsi( wstrStartIP );
				IterData.Next();

				std::wstring wstrEndIP( ( const wchar_t* ) IterData.GetValue().GetWString() );
				std::string strEndIP = sc::string::unicodeToAnsi( wstrEndIP );

				MakeIpList( strStartIP, strEndIP );
			}
		}

		return true;
    }
    catch ( const LuaPlus::LuaException& e )
    {
        std::string strErrMsg(
            sc::string::format(
            "%1%, %2%",
            m_strFilePath,
            e.GetErrorMessage()));
        sc::writeLogError( strErrMsg );

        return false;
    }

    return true;
}

void ran_event::GLIPEvent::CheckBoundary( DWORD& _Value )
{
    if ( _Value < MIN_BOUNDARY )
        _Value = MIN_BOUNDARY;

    else if ( _Value > MAX_BOUNDARY )
        _Value = MAX_BOUNDARY;
}

bool ran_event::GLIPEvent::IsAbleToEvent( std::string& strIP )
{
	MAP_EVENT_IP_ITER it = m_EventIP.find( strIP );
	if ( m_EventIP.end() == it )
	{
		return false;
	}
	else
	{
		return true;
	}
}

void ran_event::GLIPEvent::SetValues( DWORD _AttackValue, DWORD _AttackSpeed, DWORD _MoveSpeed, DWORD _ItemDrop, DWORD _MoneyDrop, DWORD _Exp, DWORD _HPRecovery, DWORD _MPRecovery, DWORD _SPRecovery, DWORD _AllRecovery, DWORD _CriticalRate )
{
    m_AttackValue = _AttackValue;
    m_AttackSpeed = _AttackSpeed;
    m_MoveSpeed = _MoveSpeed;
    m_ItemDrop = _ItemDrop;
    m_MoneyDrop = _MoneyDrop;
    m_Exp = _Exp;
    m_HPRecovery = _HPRecovery;
    m_MPRecovery = _MPRecovery;
    m_SPRecovery = _SPRecovery;
    m_AllRecovery = _AllRecovery;
    m_CriticalRate = _CriticalRate;
}

bool ran_event::GLIPEvent::MakeIpList( std::string& IpStart, std::string& IpEnd )
{
	try
	{
		sc::string::trim( IpStart );
		sc::string::trim( IpEnd );

		typedef std::vector<std::string> splitVector;
		splitVector SplitStart;
		splitVector SplitEnd;

		// '.' 토큰 분리
		boost::split( SplitStart, IpStart, boost::algorithm::is_any_of( "." ) );
		boost::split( SplitEnd, IpEnd, boost::algorithm::is_any_of( "." ) );

		int Start = boost::lexical_cast< int >( SplitStart[3] );
		int End = boost::lexical_cast< int >( SplitEnd[3] );

		if ( Start > End )
		{
			int Temp = Start;
			Start = End;
			End = Temp;
		}

		std::string Info( sc::string::format( "event ip loaded %1%.%2%.%3%.%4% To %1%.%2%.%3%.%5%", SplitStart[0], SplitStart[1], SplitStart[2], Start, End ) );
		sc::writeLogInfo( Info );

		for ( int i = Start; i <= End; ++i )
		{
			std::string IpAddress( sc::string::format( "%1%.%2%.%3%.%4%", SplitStart[0], SplitStart[1], SplitStart[2], i ) );
			m_EventIP.insert( MAP_EVENT_IP_VALUE( IpAddress, IpAddress ) );
		}
		return true;
	}
	catch ( boost::bad_lexical_cast& e )
	{
		sc::writeLogError( std::string( e.what() ) );        
		return false;
	}
}