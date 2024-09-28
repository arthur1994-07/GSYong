#include "pch.h"

#include <time.h>
#include "../TexasHoldem/GLTexasHoldemSetting.h"
#include "./GLTexasHoldem.h"

#include "../../SigmaCore/DebugInclude.h"

namespace TexasHoldem
{


	GLTexasHoldemChipMap::GLTexasHoldemChipMap()
	{

	}

	GLTexasHoldemChipMap::~GLTexasHoldemChipMap()
	{

	}

	bool GLTexasHoldemChipMap::CheckVersion( LONGLONG Version )
	{
		return m_Version == Version ? true : false;
	}

	void GLTexasHoldemChipMap::Reset()
	{
		m_Version = 0;
		m_Container.clear();
	}

	void GLTexasHoldemChipMap::InsertChipPrice( sChipPrice& sChip )
	{
		m_Container.push_back( sChip );
	}

	bool GLTexasHoldemChipMap::GetChipPriceOfGameMoney( LONGLONG& Chip, LONGLONG& GameMoney OUT )
	{
		std::vector< sChipPrice >::iterator iter = std::find( m_Container.begin(), m_Container.end(), sChipPrice( Chip, 0, 0 ) );
		if ( iter != m_Container.end() )
		{
			sChipPrice& sChip = *iter;
			GameMoney = sChip.GameMoney;
			return true;
		}
		else
		{
			return false;
		}
	}

	bool GLTexasHoldemChipMap::GetChipPriceOfRanPoint( LONGLONG& Chip, LONGLONG& RanPoint OUT )
	{
		std::vector< sChipPrice >::iterator iter = std::find( m_Container.begin(), m_Container.end(), sChipPrice( Chip, 0, 0 ) );
		if ( iter != m_Container.end() )
		{
			sChipPrice& sChip = *iter;
			RanPoint = sChip.Point;
			return true;
		}
		else
		{
			return false;
		}
	}

	GLTexasHoldemPlayRequirement::GLTexasHoldemPlayRequirement()
	{

	}

	GLTexasHoldemPlayRequirement::~GLTexasHoldemPlayRequirement()
	{

	}

	int GLTexasHoldemPlayRequirement::CheckRequirement( DWORD CharacterLevel )
	{
		if ( false == checkLevel( CharacterLevel ) )
		{
			return eRequireLevel;
		}

		return eSuccess;
	}

	bool GLTexasHoldemPlayRequirement::checkLevel( DWORD CharacterLevel )
	{
		GLTexasHoldemSetting* const pSettings = GLTexasHoldemSetting::getInstance();
		if ( NULL == pSettings )
			return false;

		return CharacterLevel >= pSettings->GetRequirLevel() ? true : false;
	}

	GLTexasHoldemMgr::GLTexasHoldemMgr()
	{
	}

	GLTexasHoldemMgr::~GLTexasHoldemMgr()
	{
	}

	LONGLONG GLTexasHoldemMgr::GetVersion()
	{
		return m_ChipMap.GetVersion();
	}

	void GLTexasHoldemMgr::SetVersion( LONGLONG Version )
	{
		m_ChipMap.SetVersion( Version );
	}

	bool GLTexasHoldemMgr::CheckVersion( LONGLONG Version )
	{
		return m_ChipMap.CheckVersion( Version );
	}

	void GLTexasHoldemMgr::Reset()
	{
		m_ChipMap.Reset();
	}

	void GLTexasHoldemMgr::InsertChipPrice( sChipPrice& sChip )
	{
		m_ChipMap.InsertChipPrice( sChip );
	}

	bool GLTexasHoldemMgr::GetChipPriceOfGameMoney( LONGLONG& Chip, LONGLONG& GameMoney OUT )
	{
		return m_ChipMap.GetChipPriceOfGameMoney( Chip, GameMoney );
	}

	bool GLTexasHoldemMgr::GetChipPriceOfRanPoint( LONGLONG& Chip, LONGLONG& RanPoint OUT )
	{
		return m_ChipMap.GetChipPriceOfRanPoint( Chip, RanPoint );
	}

	const std::vector< sChipPrice >& GLTexasHoldemMgr::GetChipMap()
	{
		return m_ChipMap.GetChipMap();
	}

	int GLTexasHoldemMgr::CheckRequirement( DWORD CharacterLevel )
	{
		return m_Requirement.CheckRequirement( CharacterLevel );
	}

	GLTexasHoldemStartGameDelayMgr::GLTexasHoldemStartGameDelayMgr()
	{
	}

	GLTexasHoldemStartGameDelayMgr::~GLTexasHoldemStartGameDelayMgr()
	{
	}


	GLTexasHoldemGameTime::GLTexasHoldemGameTime()
	{
	}

	GLTexasHoldemGameTime::~GLTexasHoldemGameTime()
	{
	}

	void GLTexasHoldemGameTime::AddUser( DWORD dwChaDbNum )
	{
		CTime tCurTime = CTime::GetCurrentTime();
		std::map< DWORD, __time64_t >::iterator iter = m_GameTime.find( dwChaDbNum );
		if ( iter == m_GameTime.end() )
			m_GameTime.insert( std::map< DWORD, __time64_t >::value_type( dwChaDbNum, tCurTime.GetTime() ) );
		else
		{
			m_GameTime.erase( iter );
			m_GameTime.insert( std::map< DWORD, __time64_t >::value_type( dwChaDbNum, tCurTime.GetTime() ) );
		}
	}

	int GLTexasHoldemGameTime::GetGameTime( DWORD dwChaDbNum )
	{
		int nSeccond = -1;
		CTime tCurTime = CTime::GetCurrentTime();
		std::map< DWORD, __time64_t >::iterator iter = m_GameTime.find( dwChaDbNum );
		if ( iter != m_GameTime.end() )
		{
			__time64_t& tStartTime = iter->second;
			CTimeSpan tTimeSpan( tCurTime.GetTime() - tStartTime );
			nSeccond = (int)tTimeSpan.GetTotalSeconds();
		}
		return nSeccond;
	}

	void GLTexasHoldemGameTime::DeleteUser( DWORD dwChaDbNum )
	{
		std::map< DWORD, __time64_t >::iterator iter = m_GameTime.find( dwChaDbNum );
		if ( iter != m_GameTime.end() )
		{
			m_GameTime.erase( iter );
		}
	}

}