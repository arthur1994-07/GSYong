#pragma once

#include <boost/exception/exception.hpp>
#include <boost/smart_ptr/weak_ptr.hpp>
#include "GLTournamentTreeDefine.h"

#include "../Character/GLCharDefine.h"
#include "../Character/GLCharData.h"
#include "../InstanceSystem/GLInstanceSystemDefine.h"
#include "../Msg/GLContrlMatching.h"

#include "../GLogicData.h"

#include <vector>


namespace MatchingSystem
{
	struct Tournament_UserInfo_BASIC
	{	
		WORD		wClass;
		DWORD		dwLevel;
		char		szName[CHAR_SZNAME];

		Tournament_UserInfo_BASIC()
			: wClass(GLCI_NUM_ACTOR)
			,dwLevel(0)
		{
			ZeroMemory(szName, sizeof(szName));
		}

		void SetName(const char *_szName)
		{
			GASSERT(_szName);

			StringCchCopy(szName, CHAR_SZNAME, _szName);
		}
	};
	struct Bettinginfo
	{
		LONGLONG MinimumBettingSize;
		LONGLONG MaximumBettingSize;
		DWORD ScriptID;
		MatchingSystem::BettingTable MyBetting;
		std::vector<MatchingSystem::BettingRatio> m_bettingTable;
	};

	struct GroupInforEployee
	{
		DWORD dwLevel;
		char  szSetName[CHAR_SZNAME];
		DWORD wClass;
		DWORD GroupID;
		DWORD LeageId;

		void SetName(const char *_szName)
		{
			GASSERT(szSetName);

			StringCchCopy(szSetName, CHAR_SZNAME, _szName);
		}
	};

	struct TournamentTeamInfo
	{
		DWORD TeamScore;
		DWORD TeamFactionID;
		std::vector<DWORD> TeamGaeaID;

		TournamentTeamInfo(DWORD _TeamScore,DWORD _TeamFactionID)
			:	TeamScore(_TeamScore),TeamFactionID(_TeamFactionID)
		{
		}
	};

	class TournamentGameInfo
	{
	public :
		std::vector<TournamentTeamInfo*> Team;
		DWORD GameWinConditionScore;
		DWORD leftTime;
		InstanceSystem::InstanceMapID TornanmentMapID;
	};

	class MatchingClientInfo
	{
	public :
		DWORD ScriptID;
		DWORD ClientStatus;
	};

}