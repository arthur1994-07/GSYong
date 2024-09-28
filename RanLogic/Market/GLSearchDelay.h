#pragma once

#include <string>
#include <vector>

namespace private_market
{
	class GLSearchDelay
	{
	public:
		GLSearchDelay();
		GLSearchDelay( DWORD dwChaDbNum, DWORD dwPlusMilliseconds );
		virtual ~GLSearchDelay();

	public:
		void SetDelay( DWORD dwMilliseconds ) { m_dwPlusMilliseconds = dwMilliseconds; }

		DWORD GetChaDbNum() { return m_dwChaDbNum; }
		DWORD GetLastRequest() { return m_dwLastRequestTime; }

	public:
		bool IsSearchLimitTime();
		DWORD GetRemainSearchLimitTimeClient();
		void SetDelay();

		bool operator == ( const GLSearchDelay& rvalue ) const
		{
			if ( m_dwChaDbNum == rvalue.m_dwChaDbNum )
				return true;

			return false;
		}

	protected:
		DWORD m_dwChaDbNum;
		DWORD m_dwLastRequestTime;
		DWORD m_dwPlusMilliseconds;

	};

	class GLSearchDelayMgr
	{
	public:
		GLSearchDelayMgr();
		~GLSearchDelayMgr();

	public:
		void SetDelay( DWORD dwMilliseconds );
		bool IsSearchLimitTime( DWORD dwChaDbNum );

	protected:
		std::vector< GLSearchDelay > m_vecSearchDelay;
		DWORD m_dwPlusMilliseconds;

	};

}
