#pragma once

#include "../../RanLogic/InstanceSystem/GLInstanceSystemDefine.h"
#include "../../RanLogic/Msg/GLContrlMatching.h"
#include "../../RanLogic/Msg/GLContrlInstanceMsg.h"
#include "../../RanLogicServer/Party/GLPartyAgent.h"
#include "../../RanLogicServer/InstanceSystem/InstanceSystem.h"

#include <boost/unordered/unordered_map.hpp>
#include "./MatchingCallbackTriggerDefine.h"

#include "../../SigmaCore/Lua/MinLua.h"
#include "../../SigmaCore/String/StringUtil.h"
#include <string>
#include <map>

namespace MatchingSystem
{
	class CopyTrigger;
	class RoomTrigger;
	class MatchingGameBase;
	class MatchingInstanceBase;
	class MatchingCoreScript;
	class RoomManagerBase;
	class LobbyManagerbase;
	class CompetitionRatingManager;
}

typedef std::vector<MatchingSystem::CopyTrigger*>			VEC_P_COPYCOMMAND_LIST;
typedef VEC_P_COPYCOMMAND_LIST::iterator					VEC_P_COPYCOMMAND_LIST_ITOR;
typedef std::vector<MatchingSystem::MatchingGameBase*>		VEC_P_MATCHING_LAND_LIST;
typedef VEC_P_MATCHING_LAND_LIST::iterator					VEC_P_MATCHING_LAND_LIST_ITOR;
enum
{
	ANSWER_OK = 0,
	ANSWER_CANSEL = 1,
	ANSWER_NO_RESPONS = 2,

};


typedef std::deque<DWORD> GROUP_VEC;		///< 그룹 벡터
typedef GROUP_VEC::iterator GROUP_VEC_ITOR;	///< 그룹 이터레이터

typedef std::deque<DWORD> GROUP_QUE;		///< 그룹 벡터
typedef GROUP_QUE::iterator GROUP_QUE_ITOR;	///< 그룹 이터레이터

// 메칭 유니크 아이디 만들기위해 사용.
// 외부 사용 금지
typedef std::map<DWORD,MatchingSystem::MatchingInstanceBase*> MATCHING_MAP;
typedef MATCHING_MAP::iterator					MATCHING_MAP_ITER;
typedef MATCHING_MAP::const_iterator			MATCHING_MAP_CITER;
typedef MATCHING_MAP::value_type				MATCHING_MAP_VALUE;

struct CLINET_TOURNAMENT_INFO
{
    DWORD dwTournamentID;
    DWORD emTournamentState;
};

typedef std::vector<CLINET_TOURNAMENT_INFO> TOURNAMENTID_VEC;
typedef TOURNAMENTID_VEC::iterator			TOURNAMENTID_VEC_ITER;
typedef TOURNAMENTID_VEC::const_iterator	TOURNAMENTID_VEC_CITER;

typedef std::map<DWORD,TOURNAMENTID_VEC> ClientCoreMAP;
typedef ClientCoreMAP::iterator				   ClientCoreMAP_ITER;
typedef ClientCoreMAP::const_iterator		   ClientCoreMAP_CITER;
typedef ClientCoreMAP::value_type              ClientCoreMAP_VALUE;


typedef std::map<DWORD,MatchingSystem::EMCUSTOMER_STATUS_TYPE> ClientCoreUserInfoMAP;
typedef ClientCoreUserInfoMAP::iterator				   ClientCoreUserInfoMAP_ITER;
typedef ClientCoreUserInfoMAP::const_iterator		   ClientCoreUserInfoMAP_CITER;
typedef ClientCoreUserInfoMAP::value_type              ClientCoreUserInfoMAP_VALUE;

// 남는 ID 만들기.
typedef std::deque<DWORD>						MATCHINGID;

typedef std::vector<MatchingSystem::MatchingCoreScript> InstanceScriptVector;
typedef InstanceScriptVector::const_iterator InstanceScriptVectorCIter;
typedef InstanceScriptVector::iterator InstanceScriptVectorIter;

typedef std::map<unsigned int, MatchingSystem::MatchingCoreScript> TournamentScriptMap;
typedef TournamentScriptMap::const_iterator TournamentScriptMapCIter;
typedef TournamentScriptMap::iterator TournamentScriptMapIter;