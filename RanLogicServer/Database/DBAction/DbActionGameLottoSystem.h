#pragma once

#include "DbAction.h"

#include "../../LottoSystem/GLLottoDataTypeServer.h"

// Load;
class CLottoSystemLoadAction : public db::DbAction
{
public:
	CLottoSystemLoadAction( unsigned int systemId );
	virtual ~CLottoSystemLoadAction();
	virtual int Execute( NetServer* pServer ) override;

private:
	unsigned int m_systemId;
};

// Select;
class CLottoDrawingMachineNumSelectAction : public db::DbAction
{
public:
	CLottoDrawingMachineNumSelectAction(
		unsigned int lottoNum,
		unsigned int systemId );
	virtual ~CLottoDrawingMachineNumSelectAction();
	virtual int Execute( NetServer* pServer ) override;

private:
	unsigned int m_lottoNum;
	unsigned int m_systemId;
};

class CLottoSystemCurTurnSelectAction : public db::DbAction
{
public:
	CLottoSystemCurTurnSelectAction( unsigned int lottoSystemId );
	virtual ~CLottoSystemCurTurnSelectAction();
	virtual int Execute( NetServer* pServer ) override;

private:
	unsigned int m_lottoSystemId;
};

class CLottoAccumulateMoneySelectAction : public db::DbAction
{
public:
	CLottoAccumulateMoneySelectAction(
		DWORD dwDbNum,
		DWORD dwServerGroup,
		unsigned int turnNum,
		unsigned int systemId );
	virtual ~CLottoAccumulateMoneySelectAction();
	virtual int Execute( NetServer* pServer ) override;

private:
	DWORD m_dwDbNum;
	DWORD m_dwServerGroup;
	unsigned int m_turnNum;
	unsigned int m_systemId;
};

class CLottoWinNumSelectAction : public db::DbAction
{
public:
	CLottoWinNumSelectAction(
		DWORD dwDbNum,
		DWORD dwServerGroup,
		unsigned int turnNum,
		unsigned int systemId );
	virtual ~CLottoWinNumSelectAction();
	virtual int Execute( NetServer* pServer ) override;

private:
	DWORD m_dwDbNum;
	DWORD m_dwServerGroup;
	unsigned int m_turnNum;
	unsigned int m_systemId;
};

class CLottoCharBuyListSelectAction : public db::DbAction
{
public:
	CLottoCharBuyListSelectAction(
		DWORD dwDbNum,
		DWORD dwServerGroup,
		unsigned int turnNum,
		unsigned int systemId );
	virtual ~CLottoCharBuyListSelectAction();
	virtual int Execute( NetServer* pServer ) override;

private:
	DWORD m_dwDbNum;
	DWORD m_dwServerGroup;
	unsigned int m_turnNum;
	unsigned int m_systemId;
};

class CLottoCharNonConfirmBuyListSelectAction : public db::DbAction
{
public:
	CLottoCharNonConfirmBuyListSelectAction(
		DWORD dwDbNum,
		DWORD dwServerGroup,
		unsigned int systemId,
		int confirmTicket );
	virtual ~CLottoCharNonConfirmBuyListSelectAction();
	virtual int Execute( NetServer* pServer ) override;

private:
	DWORD m_dwDbNum;
	DWORD m_dwServerGroup;
	unsigned int m_systemId;
	int m_confirmTicket;
};

class CLottoWinManListSelectAction : public db::DbAction
{
public:
	CLottoWinManListSelectAction(
		DWORD dwDbNum,
		DWORD dwServerGroup,
		unsigned int turnNum,
		unsigned int systemId );
	virtual ~CLottoWinManListSelectAction();
	virtual int Execute( NetServer* pServer ) override;

private:
	DWORD m_dwDbNum;
	DWORD m_dwServerGroup;
	unsigned int m_turnNum;
	unsigned int m_systemId;
};

class CLottoInventWinNumSelectAction : public db::DbAction
{
public:
	CLottoInventWinNumSelectAction(
		DWORD dwDbNum,
		DWORD dwServerGroup,
		unsigned int turnNum,
		unsigned int systemId );
	virtual ~CLottoInventWinNumSelectAction();
	virtual int Execute( NetServer* pServer ) override;

private:
	DWORD m_dwDbNum;
	DWORD m_dwServerGroup;
	unsigned int m_turnNum;
	unsigned int m_systemId;
};

// Save & Insert;
class CLottoDrawingMachineNumSaveAction : public db::DbAction
{
public:
	CLottoDrawingMachineNumSaveAction(
		unsigned int lottoNum,
		unsigned int accumulateCount,
		unsigned int addCount,
		unsigned int systemId,
		bool bUpdate = true );
	virtual ~CLottoDrawingMachineNumSaveAction();
	virtual int Execute( NetServer* pServer ) override;

private:
	unsigned int m_lottoNum;
	unsigned int m_accumulateCount;
	unsigned int m_addCount;
	unsigned int m_systemId;
	bool m_bUpdate;
};

class CLottoSystemTurnSaveAction : public db::DbAction
{
public:
	CLottoSystemTurnSaveAction(
		unsigned int turnNum,
		unsigned int systemId );
	virtual ~CLottoSystemTurnSaveAction();
	virtual int Execute( NetServer* pServer ) override;

private:
	unsigned int m_turnNum;
	unsigned int m_systemId;
};

class CLottoCharBuyListInsertAction : public db::DbAction
{
public:
	CLottoCharBuyListInsertAction(
		LONGLONG accumulateMoney,
		LONGLONG charge,
		int buyType,
		const std::string& drawingNum,
		LONGLONG drawingNumInt,
		int confirmTicket,
		DWORD dwUserNum,
		DWORD dwDbNum,
		DWORD dwServerGroup,
		char* szChaName,
		unsigned int turnNum,
		unsigned int systemId );
	virtual ~CLottoCharBuyListInsertAction();
	virtual int Execute( NetServer* pServer ) override;

private:
	LONGLONG m_accumulateMoney;
	LONGLONG m_charge;
	int m_buyType;
	std::string m_drawingNum;
	LONGLONG m_drawingNumInt;
	int m_confirmTicket;
	DWORD m_dwUserNum;
	DWORD m_dwDbNum;
	DWORD m_dwServerGroup;
	char m_szChaName[ CHAR_SZNAME ];
	unsigned int m_turnNum;
	unsigned int m_systemId;
};

class CLottoWinManListUpdateAction : public db::DbAction
{
public:
	CLottoWinManListUpdateAction(
		unsigned int ranking,
		LONGLONG winMoney,
		SNATIVEID sItemID,
		DWORD dwItemCount,
		unsigned int turnNum,
		unsigned int systemId );
	virtual ~CLottoWinManListUpdateAction();
	virtual int Execute( NetServer* pServer ) override;

private:
	unsigned int m_ranking;
	LONGLONG m_winMoney;
	SNATIVEID m_sItemID;
	DWORD m_dwItemCount;
	unsigned int m_turnNum;
	unsigned int m_systemId;
};

class CLottoWinManListInsertAction : public db::DbAction
{
public:
	CLottoWinManListInsertAction(
		unsigned int ranking,
		LONGLONG winNumInt,
		LONGLONG bonusNumInt,
		unsigned int systemId );
	virtual ~CLottoWinManListInsertAction();
	virtual int Execute( NetServer* pServer ) override;

private:
	unsigned int m_ranking;
	LONGLONG m_winNumInt;
	LONGLONG m_bonusNumInt;
	unsigned int m_systemId;
};

class CLottoAccumulateMoneyUpdateAction : public db::DbAction
{
public:
	CLottoAccumulateMoneyUpdateAction(
		LONGLONG accumulateMoney,
		unsigned int turnNum,
		unsigned int systemId );
	virtual ~CLottoAccumulateMoneyUpdateAction();
	virtual int Execute( NetServer* pServer ) override;

private:
	LONGLONG m_accumulateMoney;
	unsigned int m_turnNum;
	unsigned int m_systemId;
};

class CLottoWinNumUpdateAction : public db::DbAction
{
public:
	CLottoWinNumUpdateAction(
		const std::string& winNum,
		LONGLONG winNumInt,
		LONGLONG bonusNumInt,
		unsigned int turnNum,
		unsigned int systemId );
	virtual ~CLottoWinNumUpdateAction();
	virtual int Execute( NetServer* pServer ) override;

private:
	std::string m_winNum;
	LONGLONG m_winNumInt;
	LONGLONG m_bonusNumInt;
	unsigned int m_turnNum;
	unsigned int m_systemId;
};

class CLottoBuyListNonConfirmUpdateAction : public db::DbAction
{
public:
	CLottoBuyListNonConfirmUpdateAction(
		DWORD dwDbNum,
		DWORD dwServerGroup,
		LONGLONG invenMoney,
		unsigned int systemId );
	virtual ~CLottoBuyListNonConfirmUpdateAction();
	virtual int Execute( NetServer* pServer ) override;

private:
	DWORD m_dwDbNum;
	DWORD m_dwServerGroup;
	LONGLONG m_invenMoney;
	unsigned int m_systemId;
};

class CLottoSystemNextTurnAction : public db::DbAction
{
public:
	CLottoSystemNextTurnAction(
		unsigned int systemId );
	virtual ~CLottoSystemNextTurnAction();
	virtual int Execute( NetServer* pServer ) override;

private:
	unsigned int m_systemId;
};

class CLottoConfirmWinManAction : public db::DbAction
{
public:
	CLottoConfirmWinManAction( unsigned int systemId );
	virtual ~CLottoConfirmWinManAction();
	virtual int Execute( NetServer* pServer ) override;

private:
	unsigned int m_systemId;
};

class CLottoChangeInventWinNumAction : public db::DbAction
{
public:
	CLottoChangeInventWinNumAction(
		unsigned int turnNum,
		unsigned int systemId );
	virtual ~CLottoChangeInventWinNumAction();
	virtual int Execute( NetServer* pServer ) override;

private:
	unsigned int m_turnNum;
	unsigned int m_systemId;
};

class CLottoWinMoneyUpdateAction : public db::DbAction
{
public:
	CLottoWinMoneyUpdateAction(
		unsigned int turnNum,
		unsigned int systemId );
	virtual ~CLottoWinMoneyUpdateAction();
	virtual int Execute( NetServer* pServer ) override;

private:
	unsigned int m_turnNum;
	unsigned int m_systemId;
};

class CLottoSystemBuyLog : public db::DbAction
{
public:
	CLottoSystemBuyLog(
		const LottoSystem::LOTTO_SYSTEM_BUY_LOG& logInfo );
	virtual ~CLottoSystemBuyLog();
	virtual int Execute( NetServer* pServer ) override;

private:
	LottoSystem::LOTTO_SYSTEM_BUY_LOG m_log;
};

class CLottoSystemWinManLog : public db::DbAction
{
public:
	CLottoSystemWinManLog(
		unsigned int turnNum,
		unsigned int systemId );
	virtual ~CLottoSystemWinManLog();
	virtual int Execute( NetServer* pServer ) override;

private:
	unsigned int m_turnNum;
	unsigned int m_systemId;
};

class CLottoSystemTurnInfoLog : public db::DbAction
{
public:
	CLottoSystemTurnInfoLog(
		const LottoSystem::LOTTO_SYSTEM_TURN_LOG& logInfo );
	virtual ~CLottoSystemTurnInfoLog();
	virtual int Execute( NetServer* pServer ) override;

private:
	LottoSystem::LOTTO_SYSTEM_TURN_LOG m_log;
};