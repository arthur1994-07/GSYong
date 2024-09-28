#ifndef _DB_ACTION_GAME_BETTING_H_
#define _DB_ACTION_GAME_BETTING_H_
#include "../../../RanLogic/Club/GLClubDefine.h"
#include "./DbAction.h"
#include "../DbManager.h"

namespace db
{
	class BettingSelectAction : public DbAction
	{
	public:
		BettingSelectAction( DWORD _ChaDbNum , DWORD _BettingStatus );
		virtual ~BettingSelectAction();
		virtual int Execute( NetServer* pServer ) override;

	private:
		DWORD ChaDbNum;
		DWORD BettingStatus;
	};

	class BettingInsertAction : public DbAction
	{
	public:
		BettingInsertAction( DWORD _ChaNum,LONGLONG _BettingCount, DWORD _BettingGameSciptid,DWORD _BettingGroup );
		virtual ~BettingInsertAction();
		virtual int Execute( NetServer* pServer ) override;

	private:
		DWORD ChaNum;
		LONGLONG BettingCount;
		DWORD BettingGameSciptid;
		DWORD BettingGroup;
	};

	class BettingUpdateAction : public DbAction
	{
	public:
		BettingUpdateAction( DWORD _ChaNum,DWORD _BettingGameSciptid,DWORD _BettingStatus );
		virtual ~BettingUpdateAction();
		virtual int Execute( NetServer* pServer ) override;

	private:
		DWORD ChaNum;
		DWORD BettingGameSciptid;
		DWORD BettingStatus;
	};

	class BettingDeleteAction : public DbAction
	{
	public:
		BettingDeleteAction( DWORD _ChaNum );
		virtual ~BettingDeleteAction();
		virtual int Execute( NetServer* pServer ) override;

	private:
		DWORD ChaNum;
		DWORD BettingGameSciptid;
		DWORD BettingGroup;
	};
}

#endif // _DB_ACTION_GAME_CLUB_H_