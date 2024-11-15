#pragma once


// Apex 利侩( 全尼 )
#pragma	pack(1)

struct	IpCmd_st
{
	char cFlag;
	int nIp;
};

struct KillDataSt
{
    int nOriData;
};


#pragma pack()


// Apex 利侩
namespace APEX_SERVER
{
	int TickDataFromApex();
	int GetDataFromApex(void *pMyParaData, char* pData, char cDataType, unsigned int nSendID, int nRetLen);

	long NetSendToGameClient( signed int nSendId,const char * pBuffer,int nLen );
	// ApexProxy.h
	// typedef long (*_FUNC_S_KILLUSER) (signed int nId ,int Action);
	long GameServerKillUser( signed int nId, char* pData );

	int StartApexProxy();
	int StopApexProxy();

	int NoticeApexProxy_UserLogin( int nSendId, const char * szUserID );
	int NoticeApexProxy_UserLogout( int nSendId, const char * szUserID );
	int NoticeApexProxy_UserData( int nSendId,const char * pBuf,int nBufLen );

	// Apex 利侩( 全尼 )
	int NoticeApexProxy_UserIP( int nSendId, int nIP );
	int NoticeApexProxy_UserReturn ( int nSendId, int nReturn );
};