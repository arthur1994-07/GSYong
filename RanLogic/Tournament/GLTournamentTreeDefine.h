#pragma once

#include <boost/unordered/unordered_map.hpp>
#include <boost/smart_ptr/weak_ptr.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>

#include <map>

namespace MatchingSystem
{
	class GLMatchingGroup;

	// MBID ( Matching Base Identify ) 매칭 Base 식별 번호;
	typedef DWORD											MatchingBaseID;

	// MID ( Matching IDentify ) 매칭 식별 번호;
	typedef DWORD											MatchingID;

	// MGID ( Matching Group IDentify ) 매칭 그룹 식별 번호;
	typedef DWORD											MatchingGroupID;

	// Constant;
	const MatchingBaseID	INVALID_MATCHING_BASE_ID		=	-1;
	const MatchingGroupID	INVALID_MATCHING_GROUP_ID		=	0;
	const MatchingID		INVALID_MATCHING_ID				=	-1;

	// Character DBNum List;
	typedef std::vector< DWORD >							VEC_CHARACTER_DBNUM;
	typedef VEC_CHARACTER_DBNUM::iterator					VEC_CHARACTER_DBNUM_ITER;

	// Matching Group ID List
	typedef std::vector< MatchingGroupID >					VEC_MATCHING_GROUP_ID;
	typedef VEC_MATCHING_GROUP_ID::iterator					VEC_MATCHING_GROUP_ID_ITER;
	typedef VEC_MATCHING_GROUP_ID::const_iterator			VEC_MATCHING_GROUP_ID_CITER;

	// Matching Group ID Queue;
	typedef std::deque< MatchingGroupID >					QUEUE_MATCHING_GROUP_ID;
	typedef QUEUE_MATCHING_GROUP_ID::iterator				QUEUE_MATCHING_GROUP_ID_ITER;
	typedef QUEUE_MATCHING_GROUP_ID::const_iterator			QUEUE_MATCHING_GROUP_ID_CITER;

	// Matching Group Map;
	typedef std::map< MatchingGroupID, GLMatchingGroup* >	MAP_MATCHING_GROUP;
	typedef MAP_MATCHING_GROUP::iterator					MAP_MATCHING_GROUP_ITER;
	typedef MAP_MATCHING_GROUP::const_iterator				MAP_MATCHING_GROUP_CITER;
	typedef MAP_MATCHING_GROUP::value_type					MAP_MATCHING_GROUP_VALUE;

	// Matching Map;
	typedef std::map< MatchingID, VEC_MATCHING_GROUP_ID >	MAP_MATCHING;
	typedef MAP_MATCHING::iterator							MAP_MATCHING_ITER;
	typedef MAP_MATCHING::const_iterator					MAP_MATCHING_CITER;
	typedef MAP_MATCHING::value_type						MAP_MATCHING_VALUE;


	// 각 Matching 입장횟수;
	struct SJOIN_COUNT
	{
		MatchingBaseID	m_dwMatchingBaseID;
		DWORD			m_dwJoinCount;

		SJOIN_COUNT ()
			: m_dwMatchingBaseID ( INVALID_MATCHING_BASE_ID )
			, m_dwJoinCount ( 0 )
		{

		}
	};

	// Join Count List;
	typedef std::vector< SJOIN_COUNT >					VEC_JOIN_COUNT;
	typedef VEC_JOIN_COUNT::iterator					VEC_JOIN_COUNT_ITER;

	// Character Join Count Map;
	typedef std::map< DWORD, VEC_JOIN_COUNT >			MAP_CHAR_JOIN_COUNT_LIST;
	typedef MAP_CHAR_JOIN_COUNT_LIST::iterator			MAP_CHAR_JOIN_COUNT_LIST_ITER;
	typedef MAP_CHAR_JOIN_COUNT_LIST::const_iterator	MAP_CHAR_JOIN_COUNT_LIST_CITER;
	typedef MAP_CHAR_JOIN_COUNT_LIST::value_type		MAP_CHAR_JOIN_COUNT_LIST_VALUE;

/*
	// Character Map;
	typedef std::map< DWORD,  >	MAP_MATCHING_GROUP;
	typedef MAP_MATCHING_GROUP::iterator					MAP_MATCHING_GROUP_ITER;
	typedef MAP_MATCHING_GROUP::const_iterator				MAP_MATCHING_GROUP_CITER;
	typedef MAP_MATCHING_GROUP::value_type					MAP_MATCHING_GROUP_VALUE;
*/
	class TournamentTree;

	typedef boost::shared_ptr<TournamentTree> SPTR_TTREE;
	// 너무길어서 위크 포인터를 축약했다.

	typedef boost::weak_ptr<TournamentTree> WPTR_TTREE;
}