#ifndef _IVERSION_MAN_CONFIG_H_
#define _IVERSION_MAN_CONFIG_H_

#pragma once

#include <vector>
#include <string>

class IVersionManConfig
{
public:
    IVersionManConfig();
    virtual ~IVersionManConfig();

protected:
    std::string m_OdbcName;
    std::string m_DbName;
    std::string m_DbUserId;
    std::string m_DbUserPassword;
    std::string m_DbSource;

    std::vector<CString> m_vFtpIP;
    // char m_szFtpIP[100];

    int  m_nFtpPort;
    std::string m_FtpUserName;
    std::string m_FtpPassword;
    std::string m_TargetPath;

public:
    virtual bool Load(const char* filename) = 0;

    const char* GetOdbcName() const { return m_OdbcName.c_str(); }
    const char* GetDbName() const { return m_DbName.c_str(); }
    const char* GetDbUserName() const { return m_DbUserId.c_str(); }
    const char* GetDbPassword() const { return m_DbUserPassword.c_str(); }
    std::string GetDbSource() const { return m_DbSource; }

    size_t GetFtpSize() const { return m_vFtpIP.size(); }
    CString GetFtpAddress(size_t Index) { return m_vFtpIP[Index]; }
    int GetFtpPort() const { return m_nFtpPort; }
    const char* GetFtpUserName() const { return m_FtpUserName.c_str(); }
    const char* GetFtpPassword() const { return m_FtpPassword.c_str(); }
    const char* GetTargetPath() const { return m_TargetPath.c_str(); }

};

#endif // _IVERSION_MAN_CONFIG_H_