#include "stdafx.h"
#include "../String/StringFormat.h"
#include "../Log/LogMan.h"
#include "./SeqUniqueGUID.h"

#include "../DebugInclude.h"

namespace sc
{
    SeqUniqueGUID::SeqUniqueGUID()
        : m_ucServerGroup(0)
    {
        m_Data3 = 0;
        reset();
    }

    SeqUniqueGUID::~SeqUniqueGUID()
    {
    }

    DWORD SeqUniqueGUID::getRandomDword()
    {
        WORD hi = ( WORD ) ( rand() % RAND_MAX );
        WORD lo = ( WORD ) ( rand() % RAND_MAX );
        return ( ( DWORD ) hi << 16 ) | lo;
    }

    void SeqUniqueGUID::reset()
    {
        struct tm *newtime;
        __int64 ltime;
        ::_time64( &ltime );
        newtime = _localtime64( &ltime );

        m_Data4[0] = newtime->tm_year;
        m_Data4[1] = newtime->tm_mon;
        m_Data4[2] = newtime->tm_mday;
        m_Data4[3] = newtime->tm_hour;
        m_Data4[4] = newtime->tm_min;
        m_Data4[5] = newtime->tm_sec; 	
    }

    void SeqUniqueGUID::getGUID( GUID& guid )
    {
		RPC_STATUS result = UuidCreateSequential( &guid );
        if ( RPC_S_OK != result )
        {
            m_Data3 = InterlockedIncrement( &m_Data3 );
            if ( ( USHRT_MAX - 1 ) <= m_Data3 )
                m_Data3 = 0;

            guid.Data1 = getRandomDword();
            guid.Data2 = ( USHORT ) ( rand() % RAND_MAX );
            guid.Data3 = ( USHORT ) m_Data3;

            guid.Data4[0] = ( unsigned char ) GetTickCount();
            guid.Data4[1] = m_ucServerGroup;
			
			::memcpy( &guid.Data4[2], m_Data4, sizeof( unsigned char ) * 6 );

			sc::writeLogError( 
				sc::string::format( "critical error SeqUniqueGUID::getGUID fail %1%", result ) );
        }
    }

} // namespace sc