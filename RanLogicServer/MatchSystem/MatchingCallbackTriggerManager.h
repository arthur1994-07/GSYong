#pragma once

#include <vector>

#include "../../RanLogic/MatchSystem/GLCallbackTrigger.h"
#include "../../RanLogic/MatchSystem/MatchModel.h"
#include "../../RanLogic/InstanceSystem/GLInstanceSystemDefine.h"
#include "MatchingCallbackTriggerDefine.h"
using namespace CallbackUtility;

#define REMOVE_CALL_BACK_TRGGER( SOURCE_VECTOR , SOURCE_ITER , SOURCE_TYPE)\
if(SOURCE_VECTOR.size() != 0)	\
{\
	MatchingSystem::SOURCE_ITER biter = SOURCE_VECTOR.begin();\
	for (;biter!= SOURCE_VECTOR.end();  )\
	{\
		SOURCE_TYPE* node = NULL;\
		node = *biter;\
		if(node->IdentityName == _id)\
		{\
			biter = SOURCE_VECTOR.erase(biter);\
			SAFE_DELETE(node);\
		}\
		else\
		{\
			++biter;\
		}\
	}\
}

#define REMOVE_CALL_BACK_TRGGER_FUNCTION( SOURCE_VECTOR , SOURCE_ITER , SOURCE_TYPE)\
void RemoveCallback##SOURCE_TYPE( DWORD _id)\
{\
	MatchingSystem::SOURCE_ITER biter = SOURCE_VECTOR.begin();\
	for (;biter!= SOURCE_VECTOR.end();  )\
	{\
		SOURCE_TYPE* node = NULL;\
		node = *biter;\
		if(node->IdentityName == _id)\
		{\
			biter = SOURCE_VECTOR.erase(biter);\
			SAFE_DELETE(node);\
		}\
		else\
		{\
			++biter;\
		}\
	}\
}

#define REMOVE_ALL_CALL_BACK_TRGGER_FUNCTION( SOURCE_VECTOR , SOURCE_ITER , SOURCE_TYPE)\
MatchingSystem::SOURCE_ITER biter##SOURCE_VECTOR = SOURCE_VECTOR.begin();\
for (;biter##SOURCE_VECTOR!= SOURCE_VECTOR.end();  )\
{\
	SOURCE_TYPE* node = NULL;\
	node = *biter##SOURCE_VECTOR;\
	SAFE_DELETE(node);\
	biter##SOURCE_VECTOR = SOURCE_VECTOR.erase(biter##SOURCE_VECTOR);\
}



namespace MatchingSystem
{
	class MatchingCallbackTriggerManager
	{
	public:

	public:
		JOINTRIGGER m_JoinTriggerList;
		RoomTriggerVector m_RoomTriggerList;
		RoomMultyTriggerVector m_RoomMultyTriggerList;
		CopyTriggerVector m_CopyTriggerList;
		DestroyTriggerVector m_DestroyTriggerList;
		ReadyTriggerVector m_ReadyTriggerVector;
	public :
		static MatchingCallbackTriggerManager* GetInstance()
		{
			return &MatchingCallbackTriggerManager::instance;
		}

		void RemoveCallback( DWORD _id)
		{
			REMOVE_CALL_BACK_TRGGER(m_DestroyTriggerList,DestroyTriggerVectorIter,DestroyTrigger);
			REMOVE_CALL_BACK_TRGGER(m_CopyTriggerList,CopyTriggerVectorrIter,CopyTrigger);
			REMOVE_CALL_BACK_TRGGER(m_RoomTriggerList,RoomTriggerVectorIter,RoomTrigger);
			REMOVE_CALL_BACK_TRGGER(m_JoinTriggerList,JOINTRIGGER_ITER,JoinTrigger);
			REMOVE_CALL_BACK_TRGGER(m_ReadyTriggerVector,ReadyTriggerVectorIter,ReadyTrigger);
			REMOVE_CALL_BACK_TRGGER(m_RoomMultyTriggerList,RoomMultyTriggerVectorIter,RoomMultyTrigger);
		}
/*
		REMOVE_CALL_BACK_TRGGER_FUNCTION(m_DestroyTriggerList,DestroyTriggerVectorIter,DestroyTrigger);
		REMOVE_CALL_BACK_TRGGER_FUNCTION(m_CopyTriggerList,CopyTriggerVectorrIter,CopyTrigger);
		REMOVE_CALL_BACK_TRGGER_FUNCTION(m_RoomTriggerList,RoomTriggerVectorIter,RoomTrigger);
		REMOVE_CALL_BACK_TRGGER_FUNCTION(m_JoinTriggerList,JOINTRIGGER_ITER,JoinTrigger);
*/
		~MatchingCallbackTriggerManager() 
		{
			REMOVE_ALL_CALL_BACK_TRGGER_FUNCTION(m_DestroyTriggerList,DestroyTriggerVectorIter,DestroyTrigger);
			REMOVE_ALL_CALL_BACK_TRGGER_FUNCTION(m_CopyTriggerList,CopyTriggerVectorrIter,CopyTrigger);
			REMOVE_ALL_CALL_BACK_TRGGER_FUNCTION(m_RoomTriggerList,RoomTriggerVectorIter,RoomTrigger);
			REMOVE_ALL_CALL_BACK_TRGGER_FUNCTION(m_JoinTriggerList,JOINTRIGGER_ITER,JoinTrigger);
			REMOVE_ALL_CALL_BACK_TRGGER_FUNCTION(m_ReadyTriggerVector,ReadyTriggerVectorIter,ReadyTrigger);
			REMOVE_ALL_CALL_BACK_TRGGER_FUNCTION(m_RoomMultyTriggerList,RoomMultyTriggerVectorIter,RoomMultyTrigger);
		}

	private:

		static MatchingCallbackTriggerManager instance;



	};
}