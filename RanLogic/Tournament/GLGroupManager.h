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
	/* �ϳ��� Matching ���� �������� Matching Group �� ���� �� �ִ�;     */
	/* �ϳ��� Matching Game �� �ϳ��� Group Manager �� �����Ѵ�;          */
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

		/// Matching Group ���� �� ����;
		// ���ο� Matching Group �� �����Ѵ�;
		MatchingGroupID		CreateNewMatchingGroup ( DWORD dwMatchingID );

		// Matching Group �� �����Ѵ�;
		void				DeleteMatchingGroup ( MatchingGroupID dwMatchingGroupID );
		void				DeleteMatchingGroupAll ( );
		void				DeleteMatchingGroupAll_Matching ( MatchingID dwMatchingID );

		/// Matching Group ����;
		// ��⿭�� �ִ� Matching Group �� Matching �� ������Ų��;
		MatchingGroupID		PushMatching ( MatchingID dwMatchingID );

		// ��⿭�� �ִ´�;
		bool				PushWaitingQueue ( MatchingGroupID dwMatchingGroupID );
		MatchingGroupID		PushWaitingQueue_MatchingFirst ( MatchingID dwMatchingID );

		/// Matching Group �� ĳ���� ����;
		// Matching Group �� ĳ���͸� ����ϰų� �����Ѵ�;
		bool				JoinCharacter ( MatchingGroupID dwMatchingGroupID, DWORD dwChaDBNum );
		bool				DropOutCharacter_MatchingGroupID ( MatchingGroupID dwMatchingGroupID, DWORD dwChaDBNum );
		bool				DropOutCharacter ( DWORD dwChaDBNum );

		// �ΰ��� Matching Group�� ���ļ� �ϳ��� Group���� �����;
		//DWORD				MergeMatchingGroup ( DWORD dwMatchingGroupID1, DWORD dwMatchingGroupID2 );

	private:
		// Matching Group �� Matching ID�� �����Ѵ�;
		bool				SetMatching ( MatchingGroupID dwMatchingGroupID, DWORD dwMatchingID );

	public:
		/************************************************************************/
		/* Getting;                                                             */
		/************************************************************************/
		// Matching Group �����̳��� ����� �޾ƿ´�;
		DWORD				GetSizeOfMatchingGroupContainer ( );

		// Matching �����̳��� ����� �޾ƿ´�;
		DWORD				GetSizeOfMatchingContainer ( );

		// ��⿭�� ��� �׷��� �ִ��� Ȯ���Ѵ�;
		DWORD				GetSizeOfWaitingQueue ( );

		/// private �� �Ǿ�� �Ѵ�;
		// Matching Group ������ �޾ƿ´�;
		GLMatchingGroup*	GetMatchingGroup ( MatchingGroupID dwMatchingGroupID );

		// MatchingGroupID �� ��ȿ���� Ȯ���Ѵ�;
		bool				IsValidMatchingGroup ( MatchingGroupID dwMatchingGroupID );

		// ĳ���Ͱ� Matching �� �����ִ��� Ȯ���Ѵ�;
		bool				IsRegistCharacter ( DWORD Clientdbnum );

		// �ϳ��� Matching �� ĳ���� ���� �޾ƿ´�;
		DWORD				GetCharCountOfMatching ( MatchingID dwMatchingID );

		// �ϳ��� Matching �� �׷� ���� �޾ƿ´�;
		DWORD				GetMatchingGroupCountOfMatching ( MatchingID dwMatchingID );

		// Matching Group �� ����� �޾ƿ´� ( Group �� ���ϴ� ĳ���� �� );
		DWORD				GetCharCountOfMatchingGroup ( MatchingGroupID dwMatchingGroupID );

	public:
		/************************************************************************/
		/* Searching;                                                           */
		/************************************************************************/
		// MatchingGroupID �� �޾ƿ´�;
		MatchingGroupID		SearchMatchingGroupID_ChaDBNum ( DWORD dwChaDBNum );

		// Matching �� ���ϴ� Matching Group ���� ��� �޾ƿ´�;
		bool				SearchMatchingGroup_MatchingID ( VEC_MATCHING_GROUP_ID& refVecMatchingGroupID, MatchingID dwMatchingID );

		// Matching �� ���ϴ� Character ���� ��� �޾ƿ´�;
		bool				SearchCharacters_MatchingID ( VEC_CHARACTER_DBNUM& refVecCharacter, MatchingID dwMatchingID );

		// Matching Group �� ���ϴ� Character ���� ��� �޾ƿ´�;
		bool				SearchCharacters_MatchingGroupID ( VEC_CHARACTER_DBNUM& refVecCharacter, DWORD dwMatchingGroupID );

	private:
		/// ��ü Matching Group ���� �����̳�;
		// Mathcing Group ���� �����̳�;
		MAP_MATCHING_GROUP				m_mapMatchingGroup;


		/// Matching Group ID ���� �����̳�;
		// ��Ī�� ���ϴ� Mathcing Group ID �����̳� ( Matching );
		MAP_MATCHING					m_mapMatching;

		// ��⿭�� Matching Group ID �����̳� ( Lobby );
		QUEUE_MATCHING_GROUP_ID			m_queueWaitingMatchingGroupID;

	private:
		/// ���� ������ ���� Matching Group ID �����̳�;
		QUEUE_MATCHING_GROUP_ID			m_queueUnassignedMatchingGroupID;

	public:
		// Matching Group �� �ִ������;
		static const DWORD				MATCHING_GROUP_SIZE;
	};
}