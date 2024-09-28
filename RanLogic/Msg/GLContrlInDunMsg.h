#pragma once

#include "GLContrlBaseMsg.h"

const int MAX_INSTANT_TYPE = 10;

enum EMCHAR_CREATE_INSTANT_MAP_FB
{
	EMCHAR_CREATE_INSTANT_MAP_OK				= 0,	// �δ� ���� �Ϸ�
	EMCHAR_CREATE_INSTANT_MAP_FAIL				= 1, // �δ� ����
	EMCHAR_CREATE_INSTANT_MAP_FAIL_CREATE		= 2, // �δ� ���� ����
	EMCHAR_CREATE_INSTANT_MAP_FAIL_MAPID		= 3, // �δ� ���� �� ���̵� ã�� �� �������
	EMCHAR_CREATE_INSTANT_MAP_FAIL_GATEID		= 4, // �δ� ���� ����Ʈ ���̵� ã�� �� �������
	EMCHAR_CREATE_INSTANT_MAP_FAIL_TARMAPID		= 5, // �δ� Ÿ�� ���̵� ã���� �������
	EMCHAR_CREATE_INSTANT_MAP_FAIL_CONDITION	= 6, // �δ� ���� ���� ����
	EMCHAR_CREATE_INSTANT_MAP_FAIL_FIELDSVR		= 7, // �δ� ���� �ʵ弭�� ������ ����
	EMCHAR_CREATE_INSTANT_MAP_FAIL_NOT			= 8, // �δ��� �ƴѰ��
	EMCHAR_CREATE_INSTANT_MAP_FAIL_CREATEMAX	= 9, // �δ��� ������ �� �ִ� �ִ� ���� �ʰ�
	EMCHAR_CREATE_INSTANT_MAP_FAIL_REENTRYLIMIT = 10, // ������ �ð��� ���� ����
	EMCHAR_CREATE_INSTANT_MAP_FAIL_NOT_STATE = 11, // ���� ���°� �ٸ�
};

namespace GLMSG
{
	#pragma pack(1)

	
	//CF or FA
	struct SNET_CREATE_INSTANT_MAP_REQ : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		DWORD				dwGateID;
		DWORD				dwGateToIndex;
		DWORD				dwGaeaID;
        BOOL				bCreate;
        DWORD				instanceMapID;

		SNET_CREATE_INSTANT_MAP_REQ () 
			: dwGateID(0)
			, dwGateToIndex(0)
			, dwGaeaID(0)
			, instanceMapID(GAEAID_NULL)
			, bCreate(TRUE)
		{
			dwSize = sizeof(SNET_CREATE_INSTANT_MAP_REQ);
			nType = NET_MSG_GCTRL_CREATE_INSTANT_MAP_REQ;
			MIN_STATIC_ASSERT(sizeof(SNET_CREATE_INSTANT_MAP_REQ)<=NET_DATA_BUFSIZE);
		}
	};

	
	//AF
	struct SNET_CREATE_INSTANT_MAP_FLD : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		DWORD				dwGateID;
		DWORD				dwGateToIndex;
		DWORD				dwGaeaID;
        DWORD			    instanceMapID;
        SNATIVEID		    sBaseMapID;
		bool				bCreate;
		bool				bCreateTest;

		SNET_CREATE_INSTANT_MAP_FLD () 
			: dwGateID(0)
			, dwGateToIndex(0)
			, dwGaeaID(0)
			, instanceMapID(GAEAID_NULL)
			, sBaseMapID(NATIVEID_NULL())
			, bCreate(FALSE)
			, bCreateTest(FALSE)
		{
			dwSize = sizeof(SNET_CREATE_INSTANT_MAP_FLD);
			nType = NET_MSG_GCTRL_CREATE_INSTANT_MAP_FLD;
			MIN_STATIC_ASSERT(sizeof(SNET_CREATE_INSTANT_MAP_FLD)<=NET_DATA_BUFSIZE);
		}
	};

	//FA or AC or FC
	struct SNET_CREATE_INSTANT_MAP_FB : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		EMCHAR_CREATE_INSTANT_MAP_FB	emFB;

		MapID				mapID;
		D3DXVECTOR3		vPos;
		bool					bCullByObjectMap;
		__time64_t			tEntryTime;		

		SNET_CREATE_INSTANT_MAP_FB () 
			: emFB(EMCHAR_CREATE_INSTANT_MAP_OK)
			, vPos(0,0,0)
			, tEntryTime(0)
		{
			dwSize = sizeof(SNET_CREATE_INSTANT_MAP_FB);
			nType = NET_MSG_GCTRL_CREATE_INSTANT_MAP_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_CREATE_INSTANT_MAP_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CREATE_INSTANT_MAP_DEL : public NET_MSG_GENERIC
	{
		DWORD			instanceMapID;

		SNET_CREATE_INSTANT_MAP_DEL () 
			: instanceMapID(GAEAID_NULL)
		{
			dwSize = sizeof(SNET_CREATE_INSTANT_MAP_DEL);
			nType = NET_MSG_GCTRL_CREATE_INSTANT_MAP_DEL;
			MIN_STATIC_ASSERT(sizeof(SNET_CREATE_INSTANT_MAP_DEL)<=NET_DATA_BUFSIZE);
		}
	};
	
	struct SNET_INSTANT_MAP_REENTRY_CHECK_REQ : public NET_MSG_GENERIC
	{
		SNET_INSTANT_MAP_REENTRY_CHECK_REQ () 
		{
			dwSize = sizeof(SNET_INSTANT_MAP_REENTRY_CHECK_REQ);
			nType = NET_MSG_INSTANT_MAP_REENTRY_CHECK_REQ;
			MIN_STATIC_ASSERT(sizeof(SNET_INSTANT_MAP_REENTRY_CHECK_REQ)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_INSTANT_MAP_REENTRY_CHECK_FB : public NET_MSG_GENERIC
	{
		SNATIVEID			sMapID[MAX_INSTANT_TYPE];
		DWORD				nRemainMinute[MAX_INSTANT_TYPE];
		DWORD				nLimitCount;

		SNET_INSTANT_MAP_REENTRY_CHECK_FB () : nLimitCount(0)
		{
			ZeroMemory( sMapID, MAX_INSTANT_TYPE*sizeof(SNATIVEID) );
			ZeroMemory( nRemainMinute, MAX_INSTANT_TYPE*sizeof(DWORD) );
			dwSize = sizeof(SNET_INSTANT_MAP_REENTRY_CHECK_FB);
			nType = NET_MSG_INSTANT_MAP_REENTRY_CHECK_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_INSTANT_MAP_REENTRY_CHECK_FB)<=NET_DATA_BUFSIZE);
		}
	};



	// Revert to default structure packing
	#pragma pack()
};