#ifndef _SERVER_CONFIG_DEFINE_H_
#define _SERVER_CONFIG_DEFINE_H_

#pragma once

#include <string>

//#define MAXNUM_OF_AGENTSERVER_TO_MATCH		8	//MatchServer가 지원하는 최대 서버군(AgentServer)의 갯수

namespace ServerConfig
{
    struct SERVER_IP_PORT
    {
        std::string Address;
        int Port;

        SERVER_IP_PORT()
            : Port(6000)
        {
        }
    };

    struct ODBC_DB_INFO
    {
        std::string OdbcName; //! ODBC name
        std::string DbUserName; //! 접속가능한 사용자 id
        std::string DbUserPassword; //! 비밀번호
        std::string DbDatabase; //! database 명
        int PoolSize; //! 연결에 사용할 pool 의 size
        int	ResponseTime; //! 쿼리 대기시간 (sec)

        ODBC_DB_INFO()
            : PoolSize(3)
            , ResponseTime(15)
        {
        }
    };

    struct ADO_DB_INFO
    {
        std::string	m_Source;
        std::string	m_User;
        std::string	m_Password;
        std::string	m_Database;
        int	m_ResponseTime;

        ADO_DB_INFO()
            : m_ResponseTime(15)
        {
        }
    };

    enum 
    { 
        DEFAULT_SERVICE_PORT = 5001,
        DEFAULT_CONTORL_PORT = 6001,
        DEFAULT_MAX_CLIENT = 1000,
        DEFAULT_SERVER_TYPE = 0,
        DEFAULT_SERVER_GROUP = 0,
        DEFAULT_SERVER_NUMBER = 0,
        DEFAULT_SERVER_FIELD = 0,
        DEFAULT_SERVER_CHANNEL = 0,
        DEFAULT_SERVER_CHANNEL_NUMBER = 0,
        DEFAULT_SERVICE_PROVIDER  = 0,
        DEFAULT_MAX_THREAD_NUMBER = 2,
        DEFAULT_LANGUAGE_SET = 0,
        DEFAULT_EXCESS_EXP_PROCESS_TYPE = 0,
        DEFAULT_WHITE_LOCK_PORT = 80,
        DEFAULT_CONNECTION_PER_IP_ADDRESS = 50,
		DEFAULT_REFERENCE_COUNT_MAX = 2000,
		DEFAULT_ADO_MAX_POOL_SIZE = 1500,
		DEFAULT_DB_PACKET_SIZE = 32767,
    };
}

#endif // _SERVER_CONFIG_DEFINE_H_