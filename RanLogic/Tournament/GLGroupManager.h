#pragma once

#include <math.h>

#include "../../SigmaCore/Lua/MinLua.h"

#include "./GLTournamentTree.h"
#include "./GLTournamentTreeDefine.h"
#include "./GLGroup.h"


namespace MatchingSystem
{
	class GLMatchingGroup;

	/************************************************************************/
	/* Matching Group Manager;                                              */
	/* 하나의 Matching 에는 여러개의 Matching Group 이 속할 수 있다;     */
	/* 하나의 Matching Game 에 하나의 Group Manager 가 존재한다;          */
	/************************************************************************/
	class GLMatchingGroupManager
	{
	public:
		GLMatchingGroupManager ( );
		~GLMatchingGroupManager ( );

	public:
		/************************************************************************/
		/* Script;                                                              */
		/************************************************************************/
		void				RegistCallBackFunction_Lua ( sc::LuaInstance* pScript );

		int					SearchCharacters_MatchingGroupID_Lua ( lua_State* pState );

	public:
		/************************************************************************/
		/* Invoke Function;                                                     */
		/************************************************************************/

		/// Matching Group 생성 및 제거;
		// 새로운 Matching Group 을 생성한다;
		MatchingGroupID		CreateNewMatchingGroup ( DWORD dwMatchingID );

		// Matching Group 을 삭제한다;
		void				DeleteMatchingGroup ( MatchingGroupID dwMatchingGroupID );
		void				DeleteMatchingGroupAll ( );
		void				DeleteMatchingGroupAll_Matching ( MatchingID dwMatchingID );

		/// Matching Group 관리;
		// 대기열에 있는 Matching Group 을 Matching 에 참여시킨다;
		MatchingGroupID		PushMatching ( MatchingID dwMatchingID );

		// 대기열에 넣는다;
		bool				PushWaitingQueue ( MatchingGroupID dwMatchingGroupID );
		MatchingGroupID		PushWaitingQueue_MatchingFirst ( MatchingID dwMatchingID );

		/// Matching Group 의 캐릭터 관리;
		// Matching Group 에 캐릭터를 등록하거나 제거한다;
		bool				JoinCharacter ( MatchingGroupID dwMatchingGroupID, DWORD dwChaDBNum );
		bool				DropOutCharacter_MatchingGroupID ( MatchingGroupID dwMatchingGroupID, DWORD dwChaDBNum );
		bool				DropOutCharacter ( DWORD dwChaDBNum );

		// 두개의 Matching Group을 합쳐서 하나의 Group으로 만든다;
		//DWORD				MergeMatchingGroup ( DWORD dwMatchingGroupID1, DWORD dwMatchingGroupID2 );

	private:
		// Matching Group 의 Matching ID를 세팅한다;
		bool				SetMatching ( MatchingGroupID dwMatchingGroupID, DWORD dwMatchingID );

	public:
		/************************************************************************/
		/* Getting;                                                             */
		/************************************************************************/
		// Matching Group 컨테이너의 사이즈를 받아온다;
		DWORD				GetSizeOfMatchingGroupContainer ( );

		// Matching 컨테이너의 사이즈를 받아온다;
		DWORD				GetSizeOfMatchingContainer ( );

		// 대기열에 몇개의 그룹이 있는지 확인한다;
		DWORD				GetSizeOfWaitingQueue ( );

		/// private 가 되어야 한다;
		// Matching Group 정보를 받아온다;
		GLMatchingGroup*	GetMatchingGroup ( MatchingGroupID dwMatchingGroupID );

		// MatchingGroupID 가 유효한지 확인한다;
		bool				IsValidMatchingGroup ( MatchingGroupID dwMatchingGroupID );

		// 캐릭터가 Matching 에 속해있는지 확인한다;
		bool				IsRegistCharacter ( DWORD Clientdbnum );

		// 하나의 Matching 의 캐릭터 수를 받아온다;
		DWORD				GetCharCountOfMatching ( MatchingID dwMatchingID );

		// 하나의 Matching 의 그룹 수를 받아온다;
		DWORD				GetMatchingGroupCountOfMatching ( MatchingID dwMatchingID );

		// Matching Group 의 사이즈를 받아온다 ( Group 에 속하는 캐릭터 수 );
		DWORD				GetCharCountOfMatchingGroup ( MatchingGroupID dwMatchingGroupID );

	public:
		/************************************************************************/
		/* Searching;                                                           */
		/************************************************************************/
		// MatchingGroupID 를 받아온다;
		MatchingGroupID		SearchMatchingGroupID_ChaDBNum ( DWORD dwChaDBNum );

		// Matching 에 속하는 Matching Group 들을 모두 받아온다;
		bool				SearchMatchingGroup_MatchingID ( VEC_MATCHING_GROUP_ID& refVecMatchingGroupID, MatchingID dwMatchingID );

		// Matching 에 속하는 Character 들을 모두 받아온다;
		bool				SearchCharacters_MatchingID ( VEC_CHARACTER_DBNUM& refVecCharacter, MatchingID dwMatchingID );

		// Matching Group 에 속하는 Character 들을 모두 받아온다;
		bool				SearchCharacters_MatchingGroupID ( VEC_CHARACTER_DBNUM& refVecCharacter, DWORD dwMatchingGroupID );

	private:
		/// 전체 Matching Group 정보 컨테이너;
		// Mathcing Group 정보 컨테이너;
		MAP_MATCHING_GROUP				m_mapMatchingGroup;


		/// Matching Group ID 관리 컨테이너;
		// 매칭에 속하는 Mathcing Group ID 컨테이너 ( Matching );
		MAP_MATCHING					m_mapMatching;

		// 대기열의 Matching Group ID 컨테이너 ( Lobby );
		QUEUE_MATCHING_GROUP_ID			m_queueWaitingMatchingGroupID;

	private:
		/// 아직 사용되지 않은 Matching Group ID 컨테이너;
		QUEUE_MATCHING_GROUP_ID			m_queueUnassignedMatchingGroupID;

	public:
		// Matching Group 의 최대사이즈;
		static const DWORD				MATCHING_GROUP_SIZE;
	};
}