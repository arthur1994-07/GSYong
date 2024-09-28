#pragma once

#include <string>
#include <boost/tr1/unordered_map.hpp>

#include "../Market/MarketDefine.h"

namespace ran_event
{
    class GLIPEvent : public CSingletone<GLIPEvent>
    {
	private:
		typedef std::tr1::unordered_map< std::string, std::string > MAP_EVENT_IP;
		typedef MAP_EVENT_IP::iterator MAP_EVENT_IP_ITER;
		typedef MAP_EVENT_IP::value_type MAP_EVENT_IP_VALUE;
		typedef MAP_EVENT_IP::const_iterator MAP_EVENT_IP_CITER;

    private:
        static const DWORD MIN_BOUNDARY;
        static const DWORD MAX_BOUNDARY;

    protected:
        friend CSingletone;

    public:
        GLIPEvent();
        ~GLIPEvent();

    private:
        std::string m_strFilePath;

        DWORD m_AttackValue;
        DWORD m_AttackSpeed;
        DWORD m_MoveSpeed;
        DWORD m_ItemDrop;
        DWORD m_MoneyDrop;
        DWORD m_Exp;
        DWORD m_HPRecovery;
        DWORD m_MPRecovery;
        DWORD m_SPRecovery;
        DWORD m_AllRecovery;
        DWORD m_CriticalRate;

		MAP_EVENT_IP m_EventIP;

    public:
        bool Load( const std::string& strFileName, bool bServer );

    public:
        bool IsAbleToEvent( std::string& strIP );

        void SetValues( DWORD _AttackValue,
                        DWORD _AttackSpeed,
                        DWORD _MoveSpeed,
                        DWORD _ItemDrop,
                        DWORD _MoneyDrop,
                        DWORD _Exp,
                        DWORD _HPRecovery,
                        DWORD _MPRecovery,
                        DWORD _SPRecovery,
                        DWORD _AllRecovery,
                        DWORD _CriticalRate );

        inline DWORD GetAttackValue() { return m_AttackValue; }
        inline DWORD GetAttackSpeed() { return m_AttackSpeed; }
        inline DWORD GetMoveSpeed() { return m_AttackSpeed; }

        inline DWORD GetHPRecovery() { return m_HPRecovery; }
        inline DWORD GetMPRecovery() { return m_MPRecovery; }
        inline DWORD GetSPRecovery() { return m_SPRecovery; }
        inline DWORD GetAllRecovery() { return m_AllRecovery; }

        inline DWORD GetItempDrop() { return m_ItemDrop; }
        inline DWORD GetMoneyDrop() { return m_MoneyDrop; }
        inline DWORD GetExp() { return m_Exp; }

        inline DWORD GetCriticalRate() { return m_CriticalRate; }

    private:
        void Init();

        bool LoadOptions();
        bool LoadIPs();
        void CheckBoundary( DWORD& _Value );

		bool MakeIpList( std::string& IpStart, std::string& IpEnd );
    };

}
