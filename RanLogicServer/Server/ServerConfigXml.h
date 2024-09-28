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
    bool m_bPkLess; //! 선도클럽, PK, 학원간 PK 등 모든 적대행위 Disable
    bool m_bPK; //! true:PK 서버, false:Non PK 서버
    
    bool m_bAdultCheck; //! 성인체크    
    bool m_bUseEventThread; //! 이벤트 쓰레드 사용 여부
	UINT m_nFPS;		//mjeon.Server FPS
    bool m_bTestServer; //! 테스트 서버 유/무
    bool m_bHeartBeat; //! HeartBeat 를 사용해서 클라이언트 접속끊김을 조사    
    int  m_nServerMaxClient; //! 최대 접속 가능 클라이언트 수
    int  m_nMaxChannelUser; //! 채널당 최대 접속 가능 클라이언트 수    
    int  m_nPortService; //! 서비스용 포트
    int	 m_nPortControl; //! 컨트롤용 포트
    net::EMSERVERTYPE m_ServerType; //! 서버 유형
    int  m_nServerGroup; //! 서버 그룹
    int  m_nServerNumber; //! 서버 번호        
    EMSERVICE_PROVIDER m_ServiceProvider; //! 서비스 제공 회사유형
    
    int m_nMaxThread; //! 최대 Work Thread 갯수
    language::LANGFLAG m_LanguageSet; //! Language Set
    
    int	 m_PatchVersion; //! 서버 버전, 클라이언트 버전과 동일
    int  m_LauncherVersion; //! 패치 프로그램 버전
    int  m_PatchCutVersion;  //! 설정된 버전 이하는 런쳐로 접속 불능
       
    std::string m_ServerIp;

    unsigned int m_ConnectionPerIpAddress; //! 하나의 IP 당 서버 연결 가능개수

    bool m_UseCustomOdbcPool; //! 직접 제작한 Odbc Pool 을 사용할 것인가?

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
    bool IsPKServer() const { return m_bPK; }	//	이건 PK 가능여부
    
    BOOL IsUserEventThread() const { return m_bUseEventThread; }

	UINT GetFPS() const { return m_nFPS; }

    language::LANGFLAG GetLangSet() const { return m_LanguageSet; }

    //bool IsAllowFileLoad() const { return m_bAllowFileLoad; }
    //void SetAllowFileLoad(bool bAllowFileLoad) { m_bAllowFileLoad = bAllowFileLoad; }

    //! 선도클럽, PK, 학원간 PK 등 모든 적대행위 Disable
    bool IsPKLessServer() const { return m_bPkLess; }
    
    //! 하나의 IP 당 서버 연결 가능개수
    unsigned int GetConnectionPerIpAddress() const { return m_ConnectionPerIpAddress; }

    //! 직접 제작한 Odbc Pool 을 사용할 것인가?
    bool UseCustomDbPool() const { return m_UseCustomOdbcPool; }

	int GetReferenceCountMax() const { return m_nReferenceCountMax; }

	int GetAdoMaxPoolSize() const { return m_nAdoMaxPoolSize; }
	int GetDatabaseTcpPacketSize() const { return m_nDatabaseTcpPacketSize; }

	void SetVersion( int nLauncherVersion, int nPatchVersion, int nPatchCutVersion );
	void Save();
};

#endif // _SERVER_CONFIG_XML_H_