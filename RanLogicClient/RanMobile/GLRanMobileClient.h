#pragma once

#include "../../RanLogic/MSG/GLContrlMsg.h"
#include "../Widget/GLWidgetScript.h"

class GLGaeaClient;

class GLRanMobileClient
{
private :
	GLRanMobileClient();
	~GLRanMobileClient();

public :
	static GLRanMobileClient& GetInstance();

private :
	// 캐릭터 정보
	struct SMobileCharInfo
	{
		DWORD dwChaNum;				// 캐릭터 DB Num
		std::string strName;		// 모바일 닉네임
		__time64_t tStartDate;		// 연동 시작 시간
		__time64_t tExpiredDate;	// 남은 연동 시간
		int nRMPoint;				// M포인트
		int nRMJewel;				// 보석
		bool bDeleted;				// 삭제됐나?

		SMobileCharInfo()
			: tExpiredDate( 0 )
			, nRMPoint( 0 )
			, nRMJewel( 0 )
			, bDeleted( false )
		{
		}
	};

private :
	GLGaeaClient* m_pClient;

	std::vector< SMobileCharInfo > m_vecCharInfo;

	// 총 연동 캐릭터 개수, 캐릭터 선택창 진입시 한번만 받기 때문에 실제 개수와 안맞을 수 있다.
	int m_nTotalCharCount;

public :
	std::string GetHomePageURL();
	std::string GetAndroidDownlaodURL();
	std::string GetIOSDownloadURL();

	LONGLONG GetRecognizePrice();
	LONGLONG GetExtendPrice();
	LuaTable GetRewardItem();
	int GetMaxSendJewelCount();

	int GetTotalCharCount() { return m_nTotalCharCount; }

public :
	void ReqCreateMoney( const std::string& strNickName );
	void ReqCreateCash( const std::string& strNickName );
	void ReqExtendTime();
	void ReqInfo();
	void ReqInfoAll();
	void ReqSendJewel( WORD wCount );
	void ReqGetDailyItem();
	void ReqChangeItem( WORD wIndex );

	void PrintMsgResult( GLMSG::SNET_RANMOBILE_COMMAND_RESPONSE* pNetMsg );

	void MsgCommandResponse( NET_MSG_GENERIC* nmg );

public :
	LuaTable GetCharInfo( WORD wIndex );
	LuaTable GetCharInfoAll();
};