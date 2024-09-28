#pragma once

#include "./ServerConfigXml.h"


class MatchServerConfigXml : public ServerConfigXml
{
public:
    MatchServerConfigXml();
    virtual ~MatchServerConfigXml();

	A_SERVER_INFO* GetAgentServerInfo(DWORD n);

protected:
    ServerConfig::SERVER_IP_PORT m_SessionServer;
    A_SERVER_INFO m_AgentServerToMatch[MAXNUM_OF_AGENTSERVER_TO_MATCH];

    virtual bool Parse() override;    
    bool ParseAgentServerToMatch();	
    bool ParseExcessXpProcessType();    
    bool ParsePkServer();

public:    
    const char* GetSessionServerIP() const { return m_SessionServer.Address.c_str(); }
    int GetSessionServerPort() const { return m_SessionServer.Port; }
};

