#ifndef _LOGIN_SERVER_CONFIG_XML_H_
#define _LOGIN_SERVER_CONFIG_XML_H_

#pragma once

#include <boost/tr1/unordered_map.hpp>
#include "./ServerConfigXml.h"

class LoginServerConfigXml : public ServerConfigXml
{
private:
    typedef std::tr1::unordered_map<std::string, std::string> MAP_ALLOW_IP;
    typedef MAP_ALLOW_IP::iterator                         MAP_ALLOW_IP_ITER;
    typedef MAP_ALLOW_IP::value_type                       MAP_ALLOW_IP_VALUE;
    typedef MAP_ALLOW_IP::const_iterator                   MAP_ALLOW_IP_CITER;

public:
    LoginServerConfigXml();
    virtual ~LoginServerConfigXml();

private:
    MAP_ALLOW_IP m_AllowIPType1;
    ServerConfig::SERVER_IP_PORT m_SessionServer;

    int m_nServerChannel; //! 서버 채널 번호
    //MAP_ALLOW_IP m_vecAllowIPType2;
    //MAP_ALLOW_IP m_vecAllowIPType3;

protected:
    virtual bool Parse() override;
    
private:    
    bool ParseAllowIp();    
    bool ParseAllowIp(const std::string& ElmtName, MAP_ALLOW_IP& MapIp);
    bool MakeIpList(std::string IpStart, std::string IpEnd, MAP_ALLOW_IP& MapId);
    bool ParsePatchVersion();
    bool ParseServerVersion();

public:
    int GetServerChannel() const { return m_nServerChannel; }
    bool IsAllowIp(const std::string& strAllowIP) const;    
    void SetPatchVersion(int nVer) { m_PatchVersion = nVer; }
    void SetLauncherVersion(int nVer) { m_LauncherVersion = nVer; }
    void SetPatchCutVersion(int nGap) { m_PatchCutVersion = nGap; }
    const char* GetSessionServerIP() const { return m_SessionServer.Address.c_str(); }
    int GetSessionServerPort() const { return m_SessionServer.Port; }
};

#endif