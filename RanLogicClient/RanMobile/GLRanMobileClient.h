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
	// ĳ���� ����
	struct SMobileCharInfo
	{
		DWORD dwChaNum;				// ĳ���� DB Num
		std::string strName;		// ����� �г���
		__time64_t tStartDate;		// ���� ���� �ð�
		__time64_t tExpiredDate;	// ���� ���� �ð�
		int nRMPoint;				// M����Ʈ
		int nRMJewel;				// ����
		bool bDeleted;				// �����Ƴ�?

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

	// �� ���� ĳ���� ����, ĳ���� ����â ���Խ� �ѹ��� �ޱ� ������ ���� ������ �ȸ��� �� �ִ�.
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