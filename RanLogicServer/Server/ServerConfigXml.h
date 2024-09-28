#ifndef _SERVER_CONFIG_XML_H_
#define _SERVER_CONFIG_XML_H_

#pragma once

#include "../../SigmaCore/Database/DbDefine.h"
#include "../../EngineLib/DxTools/DxFontDefine.h"
#include "../../RanLogic/s_NetGlobal.h"
#include "./ServerConfigDefine.h"

namespace ticpp
{
    class Document;
    class Exception;
}

//! class ServerConfigXml
//! 2011-04-05 Jgkim
//! Copyright(c) MIN COMMUNICATIONS, INC.
class ServerConfigXml
{
public:
    ServerConfigXml();
    virtual ~ServerConfigXml();

protected:
	friend class CAgentServer;
    std::string m_XmlFileName;
    ticpp::Document* m_pXml;
    bool m_bPkLess; //! ����Ŭ��, PK, �п��� PK �� ��� �������� Disable
    bool m_bPK; //! true:PK ����, false:Non PK ����
    
    bool m_bAdultCheck; //! ����üũ    
    bool m_bUseEventThread; //! �̺�Ʈ ������ ��� ����
	UINT m_nFPS;		//mjeon.Server FPS
    bool m_bTestServer; //! �׽�Ʈ ���� ��/��
    bool m_bHeartBeat; //! HeartBeat �� ����ؼ� Ŭ���̾�Ʈ ���Ӳ����� ����    
    int  m_nServerMaxClient; //! �ִ� ���� ���� Ŭ���̾�Ʈ ��
    int  m_nMaxChannelUser; //! ä�δ� �ִ� ���� ���� Ŭ���̾�Ʈ ��    
    int  m_nPortService; //! ���񽺿� ��Ʈ
    int	 m_nPortControl; //! ��Ʈ�ѿ� ��Ʈ
    net::EMSERVERTYPE m_ServerType; //! ���� ����
    int  m_nServerGroup; //! ���� �׷�
    int  m_nServerNumber; //! ���� ��ȣ        
    EMSERVICE_PROVIDER m_ServiceProvider; //! ���� ���� ȸ������
    
    int m_nMaxThread; //! �ִ� Work Thread ����
    language::LANGFLAG m_LanguageSet; //! Language Set
    
    int	 m_PatchVersion; //! ���� ����, Ŭ���̾�Ʈ ������ ����
    int  m_LauncherVersion; //! ��ġ ���α׷� ����
    int  m_PatchCutVersion;  //! ������ ���� ���ϴ� ���ķ� ���� �Ҵ�
       
    std::string m_ServerIp;

    unsigned int m_ConnectionPerIpAddress; //! �ϳ��� IP �� ���� ���� ���ɰ���

    bool m_UseCustomOdbcPool; //! ���� ������ Odbc Pool �� ����� ���ΰ�?

	int m_nReferenceCountMax;

	int m_nAdoMaxPoolSize;
	int m_nDatabaseTcpPacketSize;
    
protected:
    virtual bool Parse();

    bool ParseServerIp();
    bool ParseServerVersion();
    bool ParsePatchVersion();
    bool ParseVersionGap();

    void WriteErrorLog(ticpp::Exception& ex);
    void WriteWarningLog(ticpp::Exception& ex);
    void WriteDebugLog(ticpp::Exception& ex);

    bool ParsePkLess();
    bool ParsePk();    
    bool ParseAdult();
    //bool ParseIntelTbb();
    bool ParseEventThread();
	bool ParseFPS();
    bool ParseTestServer();
    bool ParseHeartbeat();
    
    bool ParseMaxClient();
    bool ParseMaxChannelClient();
    bool ParseServicePort();
    bool ParseControlPort();
    bool ParseServerType();
    bool ParseServerGroup();
    bool ParseServerNumber();        
    bool ParseServiceProvider();    
    bool ParseMaxThreadNumber();
    bool ParseLanguageSet();
    bool ParseConnectionPerIpAddress();
    bool ParseUseCustomOdbcPool();
	bool ParseReferenceCountMax();
	bool ParseAdoMaxPoolSize();
	bool ParseDatabaseTcpPacketSize();
                
public:
    bool Load(const std::string& ConfigXmlFile);

    const char* GetServerIP() const { return m_ServerIp.c_str(); }
    int GetPatchVersion() const { return m_PatchVersion; }
    int GetLauncherVersion() const { return m_LauncherVersion; }
    int GetPatchCutVersion() const { return m_PatchCutVersion; }

    // G_SERVER_INFO*		GetFTPServer();
    int GetServicePort() const { return m_nPortService; }
    int GetControlPort() const { return m_nPortControl; }

    int GetServerGroup() const { return m_nServerGroup; }
    int GetServerNumber() const { return m_nServerNumber; }    
    net::EMSERVERTYPE GetServerType() const { return m_ServerType; }
    int GetServerMaxClient() const { return m_nServerMaxClient; }    
    
    int GetServerChannelMaxClient() const { return m_nMaxChannelUser; }

    EMSERVICE_PROVIDER GetServiceProvider() const { return m_ServiceProvider; }
    int	GetMaxThread() const { return m_nMaxThread; }

    //const char* GetServerName() { return m_szServerName; }

    int	GetMaxClient() const { return GetServerMaxClient(); }

    bool HeartBeatCheck() const { return m_bHeartBeat; }    
    bool IsTestServer() const { return m_bTestServer; }
    bool AdultCheck() const { return m_bAdultCheck; }
    bool IsPKServer() const { return m_bPK; }	//	�̰� PK ���ɿ���
    
    BOOL IsUserEventThread() const { return m_bUseEventThread; }

	UINT GetFPS() const { return m_nFPS; }

    language::LANGFLAG GetLangSet() const { return m_LanguageSet; }

    //bool IsAllowFileLoad() const { return m_bAllowFileLoad; }
    //void SetAllowFileLoad(bool bAllowFileLoad) { m_bAllowFileLoad = bAllowFileLoad; }

    //! ����Ŭ��, PK, �п��� PK �� ��� �������� Disable
    bool IsPKLessServer() const { return m_bPkLess; }
    
    //! �ϳ��� IP �� ���� ���� ���ɰ���
    unsigned int GetConnectionPerIpAddress() const { return m_ConnectionPerIpAddress; }

    //! ���� ������ Odbc Pool �� ����� ���ΰ�?
    bool UseCustomDbPool() const { return m_UseCustomOdbcPool; }

	int GetReferenceCountMax() const { return m_nReferenceCountMax; }

	int GetAdoMaxPoolSize() const { return m_nAdoMaxPoolSize; }
	int GetDatabaseTcpPacketSize() const { return m_nDatabaseTcpPacketSize; }

	void SetVersion( int nLauncherVersion, int nPatchVersion, int nPatchCutVersion );
	void Save();
};

#endif // _SERVER_CONFIG_XML_H_