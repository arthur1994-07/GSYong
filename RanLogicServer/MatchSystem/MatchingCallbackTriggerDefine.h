#pragma once

#include <vector>

#include "../../RanLogic/MatchSystem/GLCallbackTrigger.h"
#include "../../RanLogic/MatchSystem/MatchModel.h"
#include "../../RanLogic/InstanceSystem/GLInstanceSystemDefine.h"

using namespace CallbackUtility;

namespace MatchingSystem
{
	// 스크립트를 복사하는 트리거;
	class CopyTrigger : public CallbackUtility::CallBackTrigger<CopyScript>
	{
	public:
		CopyTrigger(DWORD _id) : CallBackTrigger(_id){}
		DWORD ScriptID;
	};

	// 인던 생성 트리거 ( 사용되지 않는다 );
	class RoomTrigger : public CallbackUtility::CallBackTrigger<RoomReceipt>
	{

	public:
		RoomTrigger(DWORD _id) : CallBackTrigger(_id){}
		SNATIVEID _instanceID;
		DWORD Channel;
	};

	// 여러개의 인던을 생성할 수 있는 트리거;
	class RoomMultyTrigger : public CallbackUtility::CallBackTrigger<RoomMultyReceipt>
	{

	public:
		RoomMultyTrigger(DWORD _id) : CallBackTrigger(_id), m_frameMove(0), m_faildCounter(0) {}
		SNATIVEID _instanceID;
		DWORD _MapNeedSize;
		float m_frameMove;
		DWORD m_faildCounter;
		lua_State* pState;
		std::vector<bool> _bCreateComplete;
		std::vector<DWORD> _vecInstanceMapID;
	};

	// 인던 파괴 트리거;
	class DestroyTrigger : public CallbackUtility::CallBackTrigger<DestroyReceipt>
	{

	public:
		DestroyTrigger(DWORD _id) : CallBackTrigger(_id){}
		InstanceSystem::InstanceMapID _instanceID;
		DWORD _MatchingInstancceID;
	};

	// 입장 트리거;
	class JoinTrigger : public CallbackUtility::CallBackTrigger<JoinReceipt>
	{

	public:
		JoinTrigger(DWORD _id) : CallBackTrigger(_id), m_frameMove(0), m_faildCounter(0) {}
		InstanceSystem::InstanceMapID _instanceMapID;
		DWORD _ClientDBID;
		float m_frameMove;
		float m_faildCounter;
	};

	// 준비 트리거;
	class ReadyTrigger : public CallbackUtility::CallBackTrigger<ReadyReceipt>
	{

	public:
		ReadyTrigger(DWORD _id) : CallBackTrigger(_id){}
		DWORD _MatchingInstanceid;
	};

	


	

	// 메칭을 위한 트리거 관리
	typedef std::vector<JoinTrigger*> JOINTRIGGER;
	typedef JOINTRIGGER::const_iterator JOINTRIGGER_CITER;
	typedef JOINTRIGGER::iterator JOINTRIGGER_ITER;

	typedef std::vector<RoomTrigger*> RoomTriggerVector;
	typedef RoomTriggerVector::const_iterator RoomTriggerVectorCIter;
	typedef RoomTriggerVector::iterator RoomTriggerVectorIter;

	typedef std::vector<RoomMultyTrigger*> RoomMultyTriggerVector;
	typedef RoomMultyTriggerVector::const_iterator RoomMultyTriggerVectorCIter;
	typedef RoomMultyTriggerVector::iterator RoomMultyTriggerVectorIter;

	typedef std::vector<CopyTrigger*> CopyTriggerVector;
	typedef CopyTriggerVector::const_iterator CopyTriggerVectorCIter;
	typedef CopyTriggerVector::iterator CopyTriggerVectorrIter;

	typedef std::vector<DestroyTrigger*> DestroyTriggerVector;
	typedef DestroyTriggerVector::const_iterator DestroyTriggerVectorCIter;
	typedef DestroyTriggerVector::iterator DestroyTriggerVectorIter;


	typedef std::vector<ReadyTrigger*> ReadyTriggerVector;
	typedef ReadyTriggerVector::const_iterator ReadyTriggerVectorCIter;
	typedef ReadyTriggerVector::iterator ReadyTriggerVectorIter;
	
}