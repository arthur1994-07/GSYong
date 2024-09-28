#pragma once

#include "GLContrlBaseMsg.h"

const int MAX_INSTANT_TYPE = 10;

enum EMCHAR_CREATE_INSTANT_MAP_FB
{
	EMCHAR_CREATE_INSTANT_MAP_OK				= 0,	// 인던 생성 완료
	EMCHAR_CREATE_INSTANT_MAP_FAIL				= 1, // 인던 실패
	EMCHAR_CREATE_INSTANT_MAP_FAIL_CREATE		= 2, // 인던 생성 실패
	EMCHAR_CREATE_INSTANT_MAP_FAIL_MAPID		= 3, // 인던 현재 맵 아이디를 찾을 수 없을경우
	EMCHAR_CREATE_INSTANT_MAP_FAIL_GATEID		= 4, // 인던 현재 게이트 아이디를 찾을 수 없을경우
	EMCHAR_CREATE_INSTANT_MAP_FAIL_TARMAPID		= 5, // 인던 타겟 아이디를 찾을수 없을경우
	EMCHAR_CREATE_INSTANT_MAP_FAIL_CONDITION	= 6, // 인던 진입 조건 실패
	EMCHAR_CREATE_INSTANT_MAP_FAIL_FIELDSVR		= 7, // 인던 생성 필드서버 문제로 실패
	EMCHAR_CREATE_INSTANT_MAP_FAIL_NOT			= 8, // 인던이 아닌경우
	EMCHAR_CREATE_INSTANT_MAP_FAIL_CREATEMAX	= 9, // 인던을 생성할 수 있는 최대 개수 초과
	EMCHAR_CREATE_INSTANT_MAP_FAIL_REENTRYLIMIT = 10, // 재입장 시간이 되지 않음
	EMCHAR_CREATE_INSTANT_MAP_FAIL_NOT_STATE = 11, // 입장 상태가 다름
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