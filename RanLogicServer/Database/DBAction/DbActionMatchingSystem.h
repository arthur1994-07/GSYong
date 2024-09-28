#ifndef _DBACTIONMATCHINGSYSTEM_H_
#define _DBACTIONMATCHINGSYSTEM_H_
#include "../../../SigmaCore/String/MinGuid.h"
#include "../../../SigmaCore/Util/DateTime.h"
#include "../../../RanLogic/GLogicData.h"
#include "../DbDefineLog.h"
#include "./DbAction.h"
#include "../DbManager.h"

namespace db
{
	class MatchingOnloadAction : public DbAction
	{
	public:
		MatchingOnloadAction(	std::string Matching_Content_Type,
								std::string Betting_Script,
								std::string Room_Admin_Script,
								std::string Matching_Script,
								std::string Lobby_Field_Manager);
		virtual ~MatchingOnloadAction();
		virtual int Execute( NetServer* pServer ) override;

	private:
		std::string m_Matching_Content_Type;
		std::string m_Betting_Script;
		std::string m_Room_Admin_Script;
		std::string m_Matching_Script;
		std::string m_Lobby_Field_Manager;
	};

	class MatchingGroupAction : public DbAction
	{
	public:
		MatchingGroupAction( 
								DWORD GroupID,
								DWORD GameScriptID,
								bool  IsDeleted );
		virtual ~MatchingGroupAction();
		virtual int Execute( NetServer* pServer ) override;

	private:
		DWORD m_GroupID;
		DWORD m_GameScriptID;
		bool  m_IsDeleted;
	};

	class MatchingGroupUserAction : public DbAction
	{
	public:
		MatchingGroupUserAction(	DWORD GroupID,
									DWORD GameScriptID,
									DWORD UserDBNum,
									bool  IsDeleted );
		virtual ~MatchingGroupUserAction();
		virtual int Execute( NetServer* pServer ) override;

	private:
		DWORD m_GroupID;
		DWORD m_GameScriptID;
		DWORD m_UserDBNum;
		bool  m_IsDeleted;
	};

	class MatchingQueueInAction : public DbAction
	{
	public:
		MatchingQueueInAction(	DWORD ClientID,
								DWORD GameScriptID,
								bool  IsDeleted  );
		virtual ~MatchingQueueInAction();
		virtual int Execute( NetServer* pServer ) override;

	private:
		DWORD m_ClientID;
		DWORD m_GameScriptID;
		bool  m_IsDeleted;
	};

	class MatchingInstanceInAction : public DbAction
	{
	public:
		MatchingInstanceInAction(	DWORD GroupID,
			DWORD GameScriptID,
			DWORD  MatchingScriptID  );
		virtual ~MatchingInstanceInAction();
		virtual int Execute( NetServer* pServer ) override;

	private:
		DWORD  m_GroupID;
		DWORD  m_GameScriptID;
		DWORD  m_MatchingScriptID;
	};

	class MatchingInstanceJoinAction : public DbAction
	{
	public:
		MatchingInstanceJoinAction(	DWORD GroupID,
			DWORD MapID,
			DWORD GameScriptID,
			DWORD  MatchingScriptID  );
		virtual ~MatchingInstanceJoinAction();
		virtual int Execute( NetServer* pServer ) override;

	private:
		DWORD  m_GroupID;
		DWORD  m_MapID;
		DWORD  m_GameScriptID;
		DWORD  m_MatchingScriptID;
	};

	class MatchingInstanceOnStartAction : public DbAction
	{
	public:
		MatchingInstanceOnStartAction(	DWORD GroupReadySize,
			DWORD GameScriptID,
			DWORD MatchingScriptID);
		virtual ~MatchingInstanceOnStartAction();
		virtual int Execute( NetServer* pServer ) override;

	private:
		DWORD  m_GroupReadySize;
		DWORD  m_GameScriptID;
		DWORD  m_MatchingScriptID;
	};

	class MatchingInstanceOutAction : public DbAction
	{
	public:
		MatchingInstanceOutAction(	DWORD GroupID,
			DWORD GameScriptID,
			DWORD MatchingScriptID);
		virtual ~MatchingInstanceOutAction();
		virtual int Execute( NetServer* pServer ) override;

	private:
		DWORD  m_GroupID;
		DWORD  m_GameScriptID;
		DWORD  m_MatchingScriptID;
	};

	class MatchingInstanceResualtAction : public DbAction
	{
	public:
		MatchingInstanceResualtAction(	DWORD Faction,
			bool IsLose,
			DWORD MapID,
			DWORD GameScriptID,
			DWORD MatchingScriptID);
		virtual ~MatchingInstanceResualtAction();
		virtual int Execute( NetServer* pServer ) override;

	private:
		DWORD  m_Faction;
		bool   m_IsLose;
		DWORD  m_MapID;
		DWORD  m_GameScriptID;
		DWORD  m_MatchingScriptID;
	};

	class MatchingRoomDestroyAction : public DbAction
	{
	public:
		MatchingRoomDestroyAction(	
			DWORD MapID,
			DWORD GameScriptID,
			DWORD MatchingScriptID);
		virtual ~MatchingRoomDestroyAction();
		virtual int Execute( NetServer* pServer ) override;

	private:
		DWORD  m_MapID;
		DWORD  m_GameScriptID;
		DWORD  m_MatchingScriptID;
	};

	class MatchingRoomReqAction : public DbAction
	{
	public:
		MatchingRoomReqAction(	
			DWORD MapID,
			DWORD GameScriptID,
			DWORD MatchingScriptID);
		virtual ~MatchingRoomReqAction();
		virtual int Execute( NetServer* pServer ) override;

	private:
		DWORD  m_MapID;
		DWORD  m_GameScriptID;
		DWORD  m_MatchingScriptID;
	};

	class MatchingFinalWinnerAction : public DbAction
	{
	public:
		MatchingFinalWinnerAction(	
			DWORD GroupID,
			DWORD GameScriptID,
			DWORD MatchingScriptID);
		virtual ~MatchingFinalWinnerAction();
		virtual int Execute( NetServer* pServer ) override;

	private:
		DWORD  m_GroupID;
		DWORD  m_GameScriptID;
		DWORD  m_MatchingScriptID;
	};

	class MatchingInstanceTournamentAction : public DbAction
	{
	public:
		MatchingInstanceTournamentAction(	
			int   tournamentPosition,
			const char* GroupName,
			int   GroupIndexPosition,
			int   GroupSize,
			int   GroupMaxSize,
			int   GroupHeadLevel,
			DWORD GroupID,
			DWORD GameScriptID,
			DWORD MatchingScriptID);
		virtual ~MatchingInstanceTournamentAction();
		virtual int Execute( NetServer* pServer ) override;

	private:
		int m_tournamentPosition;
		std::string m_GroupName;
		int   m_GroupIndexPosition;
		int   m_GroupSize;
		int   m_GroupMaxSize;
		int   m_GroupHeadLevel;
		DWORD  m_GroupID;
		DWORD  m_GameScriptID;
		DWORD  m_MatchingScriptID;
	};

	// Load Matching Join Count;
	class CMatchingJoinCountLoadAction : public db::DbAction
	{
	public:
		CMatchingJoinCountLoadAction ( MatchingSystem::MAP_CHAR_JOIN_COUNT_LIST& _refMapCharJoinCountList );
		virtual ~CMatchingJoinCountLoadAction ( );

	public:
		virtual int Execute ( NetServer* _pServer ) override;

	private:
		MatchingSystem::MAP_CHAR_JOIN_COUNT_LIST& m_refMapCharJoinCountList;
	};

	// Reset Matching Join Count;
	class CMatchingJoinCountResetAction : public db::DbAction
	{
	public:
		CMatchingJoinCountResetAction ( );
		virtual ~CMatchingJoinCountResetAction ( );

	public:
		virtual int Execute ( NetServer* _pServer ) override;
	};

	// Update Matching Join Count;
	class CMatchingJoinCountUpdateAction : public db::DbAction
	{
	public:
		CMatchingJoinCountUpdateAction ( DWORD _dwChaDBNum, const MatchingSystem::SJOIN_COUNT& _sJoinCount );
		virtual ~CMatchingJoinCountUpdateAction ( );

	public:
		virtual int Execute ( NetServer* _pServer ) override;

	private:
		DWORD m_dwChaDBNum;
		MatchingSystem::SJOIN_COUNT m_sJoinCount;
	};

}

#endif // _DBACTIONMATCHINGSYSTEM_H_