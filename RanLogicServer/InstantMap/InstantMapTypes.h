#ifndef InstantMapTypes_H
#define InstantMapTypes_H

#ifdef _USE_TBB
#include "../../=TBB/include/tbb/concurrent_queue.h"
#endif

#include <queue>
#include "../../RanLogic/Party/GLPartyDefine.h"
#include "../../RanLogic/PVE/PVEInDunID.h"

struct SENTRYINSTMAPINFO
{
	SNATIVEID	sBaseMapID;
	DWORD		instanceMapID;
	GLPartyID	PartyID;	
	DWORD		dwGaeaID;
	DWORD		dwGateID;	
	DWORD		dwGateToID;
	DWORD		dwChaNum;
	BOOL		bCreate;
	BOOL		bCreateTest;
	DWORD		dwReqTime;    

	SENTRYINSTMAPINFO() 
		: sBaseMapID(NATIVEID_NULL())
		, instanceMapID(GAEAID_NULL)
		, dwGaeaID(GAEAID_NULL)		
		, dwChaNum(MAXDWORD)
		, dwGateID(GAEAID_NULL)
		, dwGateToID(0)
		, bCreate(FALSE)
		, bCreateTest(FALSE)
		, dwReqTime(0)
	{
	};
};

struct SPVEENTRYINSTMAPINFO
{
	SNATIVEID	sBaseMapID;
	DWORD	instanceMapID;
	GLPartyID	PartyID;
	SPVEInDunID sInDunID;
	int			nStage;
    std::string strGuid;

	SPVEENTRYINSTMAPINFO() 
		: sBaseMapID(NATIVEID_NULL())
		, instanceMapID(GAEAID_NULL)
		, nStage(1)
	{
	};
};

struct InstanceMapInformation
{
	SNATIVEID baseMapID; // ������ �⺻ ��ID;
	SNATIVEID keyMapID; // �����ϰ��� �ϴ� �δ� ����;
	DWORD instanceMapID; // Agent���� ������ �δ��� ���� ID;	
	DWORD instanceType; // �ν��Ͻ� ���� ����(�Ϲ�, ��ʸ�Ʈ);

	InstanceMapInformation(void)
	{
	}
	InstanceMapInformation(const SNATIVEID& _baseMapID, const DWORD _instanceMapID, const SNATIVEID& _keyMapID, const DWORD _instanceType)
		: baseMapID(_baseMapID)
		, keyMapID(_keyMapID)
		, instanceMapID(_instanceMapID)		
		, instanceType(_instanceType)
	{
	}
	InstanceMapInformation(const InstanceMapInformation& _rhs)
		: baseMapID(_rhs.baseMapID)
		, instanceMapID(_rhs.instanceMapID)
		, keyMapID(_rhs.keyMapID)
		, instanceType(_rhs.instanceType)
	{
	}	
};

typedef std::queue<SENTRYINSTMAPINFO>		QUEUE_INSTANT_INFO;
typedef std::queue<SPVEENTRYINSTMAPINFO>	PVE_QUEUE_INSTANT_INFO;

// std::queue -> tbb::concurrent_queue �� ���� (empty, push, try_pop) thread safe queue, �� �ܿ��� treade unsafe
#ifdef _USE_TBB
typedef tbb::concurrent_queue<InstanceMapInformation>	InstanceMapInformationQueue;
typedef tbb::concurrent_queue<DWORD>					DestroyInstanceMapQueue;
#else
typedef std::queue<InstanceMapInformation>				InstanceMapInformationQueue;
typedef std::queue<DWORD>								DestroyInstanceMapQueue;
#endif

#endif