#include "pch.h"
#include "./DbActionGameLottoSystem.h"

#include "../../Server/AgentServer.h"
#include "../../Server/s_CFieldServer.h"
#include "../../FieldServer/GLGaeaServer.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// Load

CLottoSystemLoadAction::CLottoSystemLoadAction( unsigned int systemId )
: m_systemId( systemId )
{

}

CLottoSystemLoadAction::~CLottoSystemLoadAction() { }

int CLottoSystemLoadAction::Execute( NetServer* pServer )
{
	if( !pServer )
		return NET_ERROR;

	CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*>( pServer );
	if( !pAgentServer )
	{
		sc::writeLogError( std::string("DbAction(CLottoSystemLoadAction): pAgentServer is NULL.") );
		return sc::db::DB_ERROR;
	}

	if( m_pDbManager->GetConnectionType() != db::ADO )
	{
		sc::writeLogError( "CLottoSystemLoadAction::Execute. Use ADO" );
		return NET_ERROR;
	}

	int nReturn = NET_ERROR;
	nReturn = m_pDbManager->LottoSystemLoad( m_systemId );
	if( nReturn == sc::db::DB_ERROR )
	{
		sc::writeLogError( std::string("DbAction(CLottoSystemLoadAction) failed to call CLottoSystemLoadAction().") );
	}

	return nReturn;
}

// Select
CLottoDrawingMachineNumSelectAction::CLottoDrawingMachineNumSelectAction(
	unsigned int lottoNum,
	unsigned int systemId )
	: m_lottoNum( lottoNum )
	, m_systemId( systemId )
{

}

CLottoDrawingMachineNumSelectAction::~CLottoDrawingMachineNumSelectAction() { }

int CLottoDrawingMachineNumSelectAction::Execute( NetServer* pServer )
{
	if( !pServer )
		return NET_ERROR;

	CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*>( pServer );
	if( !pAgentServer )
	{
		sc::writeLogError( std::string("DbAction(CLottoDrawingMachineNumSelectAction): pAgentServer is NULL.") );
		return sc::db::DB_ERROR;
	}

	if( m_pDbManager->GetConnectionType() != db::ADO )
	{
		sc::writeLogError( "CLottoDrawingMachineNumSelectAction::Execute. Use ADO" );
		return NET_ERROR;
	}

	int nReturn = NET_ERROR;
	nReturn = m_pDbManager->LottoDrawingMachineNumSelect(
		m_lottoNum, m_systemId );
	if( nReturn == sc::db::DB_ERROR )
	{
		sc::writeLogError( std::string("DbAction(CLottoDrawingMachineNumSelectAction) failed to call CLottoDrawingMachineNumSelectAction().") );
	}

	return nReturn;
}

CLottoSystemCurTurnSelectAction::CLottoSystemCurTurnSelectAction(
	unsigned int lottoSystemId )
	: m_lottoSystemId( lottoSystemId )
{

}

CLottoSystemCurTurnSelectAction::~CLottoSystemCurTurnSelectAction() { }

int CLottoSystemCurTurnSelectAction::Execute( NetServer* pServer )
{
	if( !pServer )
		return NET_ERROR;

	CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*>( pServer );
	if( !pAgentServer )
	{
		sc::writeLogError( std::string("DbAction(CLottoSystemCurTurnSelectAction): pAgentServer is NULL.") );
		return sc::db::DB_ERROR;
	}

	if( m_pDbManager->GetConnectionType() != db::ADO )
	{
		sc::writeLogError( "CLottoSystemCurTurnSelectAction::Execute. Use ADO" );
		return NET_ERROR;
	}

	int nReturn = NET_ERROR;
	nReturn = m_pDbManager->LottoSystemCurTurnSelect(
		m_lottoSystemId );
	if( nReturn == sc::db::DB_ERROR )
	{
		sc::writeLogError( std::string("DbAction(CLottoSystemCurTurnSelectAction) failed to call CLottoSystemCurTurnSelectAction().") );
	}

	return nReturn;
}

CLottoAccumulateMoneySelectAction::CLottoAccumulateMoneySelectAction(
	DWORD dwDbNum,
	DWORD dwServerGroup,
	unsigned int turnNum,
	unsigned int systemId )
	: m_dwDbNum( dwDbNum )
	, m_dwServerGroup( dwServerGroup )
	, m_turnNum( turnNum )
	, m_systemId( systemId )
{

}

CLottoAccumulateMoneySelectAction::~CLottoAccumulateMoneySelectAction() { }

int CLottoAccumulateMoneySelectAction::Execute( NetServer* pServer )
{
	if( !pServer )
		return NET_ERROR;

	CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*>( pServer );
	if( !pAgentServer )
	{
		sc::writeLogError( std::string("DbAction(CLottoAccumulateMoneySelectAction): pAgentServer is NULL.") );
		return sc::db::DB_ERROR;
	}

	if( m_pDbManager->GetConnectionType() != db::ADO )
	{
		sc::writeLogError( "CLottoAccumulateMoneySelectAction::Execute. Use ADO" );
		return NET_ERROR;
	}

	int nReturn = NET_ERROR;
	nReturn = m_pDbManager->LottoAccumulateMoneySelect(
		m_dwDbNum, m_dwServerGroup, m_turnNum, m_systemId );
	if( nReturn == sc::db::DB_ERROR )
	{
		sc::writeLogError( std::string("DbAction(CLottoAccumulateMoneySelectAction) failed to call CLottoAccumulateMoneySelectAction().") );
	}

	return nReturn;
}

CLottoWinNumSelectAction::CLottoWinNumSelectAction(
	DWORD dwDbNum,
	DWORD dwServerGroup,
	unsigned int turnNum,
	unsigned int systemId )
	: m_dwDbNum( dwDbNum )
	, m_turnNum( turnNum )
	, m_dwServerGroup( dwServerGroup )
	, m_systemId( systemId )
{

}

CLottoWinNumSelectAction::~CLottoWinNumSelectAction() { }

int CLottoWinNumSelectAction::Execute( NetServer* pServer )
{
	if( !pServer )
		return NET_ERROR;

	CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*>( pServer );
	if( !pAgentServer )
	{
		sc::writeLogError( std::string("DbAction(CLottoWinNumSelectAction): pAgentServer is NULL.") );
		return sc::db::DB_ERROR;
	}

	if( m_pDbManager->GetConnectionType() != db::ADO )
	{
		sc::writeLogError( "CLottoWinNumSelectAction::Execute. Use ADO" );
		return NET_ERROR;
	}

	int nReturn = NET_ERROR;
	nReturn = m_pDbManager->LottoWinNumSelect(
		m_dwDbNum, m_dwServerGroup, m_turnNum, m_systemId );
	if( nReturn == sc::db::DB_ERROR )
	{
		sc::writeLogError( std::string("DbAction(CLottoWinNumSelectAction) failed to call CLottoWinNumSelectAction().") );
	}

	return nReturn;
}

CLottoCharBuyListSelectAction::CLottoCharBuyListSelectAction(
							  DWORD dwDbNum,
							  DWORD dwServerGroup,
							  unsigned int turnNum,
							  unsigned int systemId )
							  : m_dwDbNum( dwDbNum )
							  , m_dwServerGroup( dwServerGroup )
							  , m_turnNum( turnNum )
							  , m_systemId( systemId )
{

}

CLottoCharBuyListSelectAction::~CLottoCharBuyListSelectAction() { }

int CLottoCharBuyListSelectAction::Execute( NetServer* pServer )
{
	if( !pServer )
		return NET_ERROR;

	CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*>( pServer );
	if( !pAgentServer )
	{
		sc::writeLogError( std::string("DbAction(CLottoCharBuyListSelectAction): pAgentServer is NULL.") );
		return sc::db::DB_ERROR;
	}

	if( m_pDbManager->GetConnectionType() != db::ADO )
	{
		sc::writeLogError( "CLottoCharBuyListSelectAction::Execute. Use ADO" );
		return NET_ERROR;
	}

	int nReturn = NET_ERROR;
	nReturn = m_pDbManager->LottoCharBuyListSelect(
		m_dwDbNum, m_dwServerGroup, m_turnNum, m_systemId );
	if( nReturn == sc::db::DB_ERROR )
	{
		sc::writeLogError( std::string("DbAction(CLottoCharBuyListSelectAction) failed to call CLottoCharBuyListSelectAction().") );
	}

	return nReturn;
}

CLottoCharNonConfirmBuyListSelectAction::CLottoCharNonConfirmBuyListSelectAction(
	DWORD dwDbNum,
	DWORD dwServerGroup,
	unsigned int systemId,
	int confirmTicket)
	: m_dwDbNum( dwDbNum )
	, m_dwServerGroup( dwServerGroup )
	, m_systemId( systemId )
	, m_confirmTicket( confirmTicket )
{

}

CLottoCharNonConfirmBuyListSelectAction::~CLottoCharNonConfirmBuyListSelectAction() { }

int CLottoCharNonConfirmBuyListSelectAction::Execute( NetServer* pServer )
{
	if( !pServer )
		return NET_ERROR;

	CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*>( pServer );
	if( !pAgentServer )
	{
		sc::writeLogError( std::string("DbAction(CLottoCharNonConfirmBuyListSelectAction): pAgentServer is NULL.") );
		return sc::db::DB_ERROR;
	}

	if( m_pDbManager->GetConnectionType() != db::ADO )
	{
		sc::writeLogError( "CLottoCharNonConfirmBuyListSelectAction::Execute. Use ADO" );
		return NET_ERROR;
	}

	int nReturn = NET_ERROR;
	nReturn = m_pDbManager->LottoCharNonConfirmBuyListSelect(
		m_dwDbNum, m_dwServerGroup, m_systemId, m_confirmTicket );
	if( nReturn == sc::db::DB_ERROR )
	{
		sc::writeLogError( std::string("DbAction(CLottoCharNonConfirmBuyListSelectAction) failed to call CLottoCharNonConfirmBuyListSelectAction().") );
	}

	return nReturn;
}

CLottoWinManListSelectAction::CLottoWinManListSelectAction(
	DWORD dwDbNum,
	DWORD dwServerGroup,
	unsigned int turnNum,
	unsigned int systemId )
	: m_dwDbNum( dwDbNum )
	, m_dwServerGroup( dwServerGroup )
	, m_turnNum( turnNum )
	, m_systemId( systemId )
{

}

CLottoWinManListSelectAction::~CLottoWinManListSelectAction()
{

}

int CLottoWinManListSelectAction::Execute( NetServer* pServer )
{
	if( !pServer )
		return NET_ERROR;

	CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*>( pServer );
	if( !pAgentServer )
	{
		sc::writeLogError( std::string("DbAction(CLottoWinManListSelectAction): pAgentServer is NULL.") );
		return sc::db::DB_ERROR;
	}

	if( m_pDbManager->GetConnectionType() != db::ADO )
	{
		sc::writeLogError( "CLottoWinManListSelectAction::Execute. Use ADO" );
		return NET_ERROR;
	}

	int nReturn = NET_ERROR;
	nReturn = m_pDbManager->LottoWinManListSelect(
		m_dwDbNum, m_dwServerGroup, m_turnNum, m_systemId );
	if( nReturn == sc::db::DB_ERROR )
	{
		sc::writeLogError( std::string("DbAction(CLottoWinManListSelectAction) failed to call CLottoWinManListSelectAction().") );
	}

	return nReturn;
}

CLottoInventWinNumSelectAction::CLottoInventWinNumSelectAction(
	DWORD dwDbNum,
	DWORD dwServerGroup,
	unsigned int turnNum,
	unsigned int systemId )
	: m_dwDbNum( dwDbNum )
	, m_dwServerGroup( dwServerGroup )
	, m_turnNum( turnNum )
	, m_systemId( systemId )
{

}
CLottoInventWinNumSelectAction::~CLottoInventWinNumSelectAction() { }

int CLottoInventWinNumSelectAction::Execute( NetServer* pServer )
{
	if( !pServer )
		return NET_ERROR;

	CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*>( pServer );
	if( !pAgentServer )
	{
		sc::writeLogError( std::string("DbAction(CLottoInventWinNumSelectAction): pAgentServer is NULL.") );
		return sc::db::DB_ERROR;
	}

	if( m_pDbManager->GetConnectionType() != db::ADO )
	{
		sc::writeLogError( "CLottoInventWinNumSelectAction::Execute. Use ADO" );
		return NET_ERROR;
	}

	int nReturn = NET_ERROR;
	nReturn = m_pDbManager->LottoInventWinNumSelect(
		m_dwDbNum, m_dwServerGroup, m_turnNum, m_systemId );
	if( nReturn == sc::db::DB_ERROR )
	{
		sc::writeLogError( std::string("DbAction(CLottoInventWinNumSelectAction) failed to call CLottoInventWinNumSelectAction().") );
	}

	return nReturn;
}

// Save & Insert;
CLottoDrawingMachineNumSaveAction::CLottoDrawingMachineNumSaveAction(
	unsigned int lottoNum,
	unsigned int accumulateCount,
	unsigned int addCount,
	unsigned int systemId,
	bool bUpdate )
	: m_lottoNum( lottoNum )
	, m_accumulateCount( accumulateCount )
	, m_addCount( addCount )
	, m_systemId( systemId )
	, m_bUpdate( bUpdate )
{

}

CLottoDrawingMachineNumSaveAction::~CLottoDrawingMachineNumSaveAction() { }

int CLottoDrawingMachineNumSaveAction::Execute( NetServer* pServer )
{
	if( !pServer )
		return NET_ERROR;

	CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*>( pServer );
	if( !pAgentServer )
	{
		sc::writeLogError( std::string("DbAction(CLottoDrawingMachineNumSaveAction): pAgentServer is NULL.") );
		return sc::db::DB_ERROR;
	}

	if( m_pDbManager->GetConnectionType() != db::ADO )
	{
		sc::writeLogError( "CLottoDrawingMachineNumSaveAction::Execute. Use ADO" );
		return NET_ERROR;
	}

	int nReturn = NET_ERROR;
	nReturn = m_pDbManager->LottoDrawingMachineNumSave(
		m_lottoNum, m_accumulateCount, m_addCount, m_systemId, m_bUpdate );
	if( nReturn == sc::db::DB_ERROR )
	{
		sc::writeLogError( std::string("DbAction(CLottoDrawingMachineNumSaveAction) failed to call CLottoDrawingMachineNumSaveAction().") );
	}

	return nReturn;
}

CLottoSystemTurnSaveAction::CLottoSystemTurnSaveAction(
	unsigned int turnNum,
	unsigned int systemId )
	: m_turnNum( turnNum )
	, m_systemId( systemId )
{

}

CLottoSystemTurnSaveAction::~CLottoSystemTurnSaveAction() { }

int CLottoSystemTurnSaveAction::Execute( NetServer* pServer )
{
	if( !pServer )
		return NET_ERROR;

	CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*>( pServer );
	if( !pAgentServer )
	{
		sc::writeLogError( std::string("DbAction(CLottoSystemTurnSaveAction): pAgentServer is NULL.") );
		return sc::db::DB_ERROR;
	}

	if( m_pDbManager->GetConnectionType() != db::ADO )
	{
		sc::writeLogError( "CLottoSystemTurnSaveAction::Execute. Use ADO" );
		return NET_ERROR;
	}

	int nReturn = NET_ERROR;
	nReturn = m_pDbManager->LottoSystemTurnSave(
		m_turnNum, m_systemId );
	if( nReturn == sc::db::DB_ERROR )
	{
		sc::writeLogError( std::string("DbAction(CLottoSystemTurnSaveAction) failed to call CLottoSystemTurnSaveAction().") );
	}

	return nReturn;
}

CLottoCharBuyListInsertAction::CLottoCharBuyListInsertAction(
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
	unsigned int systemId )
	: m_accumulateMoney( accumulateMoney )
	, m_charge( charge )
	, m_buyType( buyType )
	, m_drawingNum( drawingNum )
	, m_drawingNumInt( drawingNumInt )
	, m_confirmTicket( confirmTicket )
	, m_dwUserNum( dwUserNum )
	, m_dwDbNum( dwDbNum )
	, m_dwServerGroup( dwServerGroup )
	, m_turnNum( turnNum )
	, m_systemId( systemId )
{
	StringCchCopy( m_szChaName, CHAR_SZNAME, szChaName );
}

CLottoCharBuyListInsertAction::~CLottoCharBuyListInsertAction() { }

int CLottoCharBuyListInsertAction::Execute( NetServer* pServer )
{
	if( !pServer )
		return NET_ERROR;

	CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*>( pServer );
	if( !pAgentServer )
	{
		sc::writeLogError( std::string("DbAction(CLottoCharBuyListInsertAction): pAgentServer is NULL.") );
		return sc::db::DB_ERROR;
	}

	if( m_pDbManager->GetConnectionType() != db::ADO )
	{
		sc::writeLogError( "CLottoCharBuyListInsertAction::Execute. Use ADO" );
		return NET_ERROR;
	}

	int nReturn = NET_ERROR;
	nReturn = m_pDbManager->LottoCharBuyListInsert(
		m_accumulateMoney, m_charge, m_buyType, m_drawingNum, m_drawingNumInt,
		m_confirmTicket, m_dwUserNum, m_dwDbNum, m_dwServerGroup, m_szChaName, m_turnNum, m_systemId );
	if( nReturn == sc::db::DB_ERROR )
	{
		sc::writeLogError( std::string("DbAction(CLottoCharBuyListInsertAction) failed to call CLottoCharBuyListInsertAction().") );
	}

	return nReturn;
}

CLottoWinManListUpdateAction::CLottoWinManListUpdateAction(
	unsigned int ranking,
	LONGLONG winMoney,
	SNATIVEID sItemID,
	DWORD dwItemCount,
	unsigned int turnNum,
	unsigned int systemId )
	: m_ranking( ranking )
	, m_winMoney( winMoney )
	, m_sItemID( sItemID )
	, m_dwItemCount( dwItemCount )
	, m_turnNum( turnNum )
	, m_systemId( systemId )
{

}

CLottoWinManListUpdateAction::~CLottoWinManListUpdateAction() { }

int CLottoWinManListUpdateAction::Execute( NetServer* pServer )
{
	if( !pServer )
		return NET_ERROR;

	CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*>( pServer );
	if( !pAgentServer )
	{
		sc::writeLogError( std::string("DbAction(CLottoWinManListSaveAction): pAgentServer is NULL.") );
		return sc::db::DB_ERROR;
	}

	if( m_pDbManager->GetConnectionType() != db::ADO )
	{
		sc::writeLogError( "CLottoWinManListSaveAction::Execute. Use ADO" );
		return NET_ERROR;
	}

	int nReturn = NET_ERROR;
	nReturn = m_pDbManager->LottoWinManListUpdate(
		m_ranking, m_winMoney, m_sItemID, m_dwItemCount, m_turnNum, m_systemId );
	if( nReturn == sc::db::DB_ERROR )
	{
		sc::writeLogError( std::string("DbAction(CLottoWinManListSaveAction) failed to call CLottoWinManListSaveAction().") );
	}

	return nReturn;
}

CLottoWinManListInsertAction::CLottoWinManListInsertAction(
	unsigned int ranking,
	LONGLONG winNumInt,
	LONGLONG bonusNumInt,
	unsigned int systemId )
	: m_ranking( ranking )
	, m_winNumInt( winNumInt )
	, m_bonusNumInt( bonusNumInt )
	, m_systemId( systemId )
{

}

CLottoWinManListInsertAction::~CLottoWinManListInsertAction() { }

int CLottoWinManListInsertAction::Execute( NetServer* pServer )
{
	if( !pServer )
		return NET_ERROR;

	CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*>( pServer );
	if( !pAgentServer )
	{
		sc::writeLogError( std::string("DbAction(CLottoWinManListSaveAction): pAgentServer is NULL.") );
		return sc::db::DB_ERROR;
	}

	if( m_pDbManager->GetConnectionType() != db::ADO )
	{
		sc::writeLogError( "CLottoWinManListSaveAction::Execute. Use ADO" );
		return NET_ERROR;
	}

	int nReturn = NET_ERROR;
	nReturn = m_pDbManager->LottoWinManListInsert(
		m_ranking, m_winNumInt, m_bonusNumInt, m_systemId );
	if( nReturn == sc::db::DB_ERROR )
	{
		sc::writeLogError( std::string("DbAction(CLottoWinManListSaveAction) failed to call CLottoWinManListSaveAction().") );
	}

	return nReturn;
}

CLottoAccumulateMoneyUpdateAction::CLottoAccumulateMoneyUpdateAction(
								  LONGLONG accumulateMoney,
								  unsigned int turnNum,
								  unsigned int systemId )
								  : m_accumulateMoney( accumulateMoney )
								  , m_turnNum( turnNum )
								  , m_systemId( systemId )
{

}

CLottoAccumulateMoneyUpdateAction::~CLottoAccumulateMoneyUpdateAction() { }

int CLottoAccumulateMoneyUpdateAction::Execute( NetServer* pServer )
{
	if( !pServer )
		return NET_ERROR;

	CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*>( pServer );
	if( !pAgentServer )
	{
		sc::writeLogError( std::string("DbAction(CLottoAccumulateMoneyUpdateAction): pAgentServer is NULL.") );
		return sc::db::DB_ERROR;
	}

	if( m_pDbManager->GetConnectionType() != db::ADO )
	{
		sc::writeLogError( "CLottoAccumulateMoneyUpdateAction::Execute. Use ADO" );
		return NET_ERROR;
	}

	int nReturn = NET_ERROR;
	nReturn = m_pDbManager->LottoAccumulateMoneyUpdate(
		m_accumulateMoney,
		m_turnNum, m_systemId );
	if( nReturn == sc::db::DB_ERROR )
	{
		sc::writeLogError( std::string("DbAction(CLottoAccumulateMoneyUpdateAction) failed to call CLottoAccumulateMoneyUpdateAction().") );
	}

	return nReturn;
}

CLottoWinNumUpdateAction::CLottoWinNumUpdateAction(
						 const std::string& winNum,
						 LONGLONG winNumInt,
						 LONGLONG bonusNumInt,
						 unsigned int turnNum,
						 unsigned int systemId )
						 : m_winNum( winNum )
						 , m_winNumInt( winNumInt )
						 , m_bonusNumInt( bonusNumInt )
						 , m_turnNum( turnNum )
						 , m_systemId( systemId )
{

}

CLottoWinNumUpdateAction::~CLottoWinNumUpdateAction() { }

int CLottoWinNumUpdateAction::Execute( NetServer* pServer )
{
	if( !pServer )
		return NET_ERROR;

	CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*>( pServer );
	if( !pAgentServer )
	{
		sc::writeLogError( std::string("DbAction(CLottoWinNumUpdateAction): pAgentServer is NULL.") );
		return sc::db::DB_ERROR;
	}

	if( m_pDbManager->GetConnectionType() != db::ADO )
	{
		sc::writeLogError( "CLottoWinNumUpdateAction::Execute. Use ADO" );
		return NET_ERROR;
	}

	int nReturn = NET_ERROR;
	nReturn = m_pDbManager->LottoWinNumUpdate(
		m_winNum, m_winNumInt, m_bonusNumInt,
		m_turnNum, m_systemId );
	if( nReturn == sc::db::DB_ERROR )
	{
		sc::writeLogError( std::string("DbAction(CLottoWinNumUpdateAction) failed to call CLottoWinNumUpdateAction().") );
	}

	return nReturn;
}


CLottoBuyListNonConfirmUpdateAction::CLottoBuyListNonConfirmUpdateAction(
	DWORD dwDbNum,
	DWORD dwServerGroup,
	LONGLONG invenMoney,
	unsigned int systemId )
	: m_dwDbNum( dwDbNum )
	, m_dwServerGroup( dwServerGroup )
	, m_invenMoney( invenMoney )
	, m_systemId( systemId )
{

}

CLottoBuyListNonConfirmUpdateAction::~CLottoBuyListNonConfirmUpdateAction() { }

int CLottoBuyListNonConfirmUpdateAction::Execute( NetServer* pServer )
{
	if( !pServer )
		return NET_ERROR;

	CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*>( pServer );
	if( !pAgentServer )
	{
		sc::writeLogError( std::string("DbAction(CLottoBuyListNonConfirmUpdateAction): pAgentServer is NULL.") );
		return sc::db::DB_ERROR;
	}

	if( m_pDbManager->GetConnectionType() != db::ADO )
	{
		sc::writeLogError( "CLottoBuyListNonConfirmUpdateAction::Execute. Use ADO" );
		return NET_ERROR;
	}

	int nReturn = NET_ERROR;
	nReturn = m_pDbManager->LottoCharNonConfirmBuyListUpdate(
		m_dwDbNum, m_dwServerGroup, m_invenMoney, m_systemId );
	if( nReturn == sc::db::DB_ERROR )
	{
		sc::writeLogError( std::string("DbAction(CLottoBuyListNonConfirmUpdateAction) failed to call CLottoBuyListNonConfirmUpdateAction().") );
	}

	return nReturn;
}


CLottoSystemNextTurnAction::CLottoSystemNextTurnAction(
	unsigned int systemId )
	: m_systemId( systemId )
{

}

CLottoSystemNextTurnAction::~CLottoSystemNextTurnAction() { }

int CLottoSystemNextTurnAction::Execute( NetServer* pServer )
{
	if( !pServer )
		return NET_ERROR;

	CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*>( pServer );
	if( !pAgentServer )
	{
		sc::writeLogError( std::string("DbAction(CLottoSystemNextTurnAction): pAgentServer is NULL.") );
		return sc::db::DB_ERROR;
	}

	if( m_pDbManager->GetConnectionType() != db::ADO )
	{
		sc::writeLogError( "CLottoSystemNextTurnAction::Execute. Use ADO" );
		return NET_ERROR;
	}

	int nReturn = NET_ERROR;
	nReturn = m_pDbManager->LottoSystemNextTurn( m_systemId );
	if( nReturn == sc::db::DB_ERROR )
	{
		sc::writeLogError( std::string("DbAction(CLottoSystemNextTurnAction) failed to call CLottoSystemNextTurnAction().") );
	}

	return nReturn;
}

CLottoConfirmWinManAction::CLottoConfirmWinManAction(
	unsigned int systemId )
	: m_systemId( systemId ) { }

CLottoConfirmWinManAction::~CLottoConfirmWinManAction() { }

int CLottoConfirmWinManAction::Execute( NetServer* pServer )
{
	if( !pServer )
		return NET_ERROR;

	CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*>( pServer );
	if( !pAgentServer )
	{
		sc::writeLogError( std::string("DbAction(CLottoConfirmWinManAction): pAgentServer is NULL.") );
		return sc::db::DB_ERROR;
	}

	if( m_pDbManager->GetConnectionType() != db::ADO )
	{
		sc::writeLogError( "CLottoConfirmWinManAction::Execute. Use ADO" );
		return NET_ERROR;
	}

	int nReturn = NET_ERROR;
	nReturn = m_pDbManager->LottoConfirmWinMan( m_systemId );
	if( nReturn == sc::db::DB_ERROR )
	{
		sc::writeLogError( std::string("DbAction(CLottoConfirmWinManAction) failed to call CLottoConfirmWinManAction().") );
	}

	return nReturn;
}

CLottoChangeInventWinNumAction::CLottoChangeInventWinNumAction(	unsigned int turnNum
															   , unsigned int systemId )
															   : m_turnNum( turnNum )
															   , m_systemId( systemId )
{

}

CLottoChangeInventWinNumAction::~CLottoChangeInventWinNumAction() { }

int CLottoChangeInventWinNumAction::Execute( NetServer* pServer )
{
	if( !pServer )
		return NET_ERROR;

	CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*>( pServer );
	if( !pAgentServer )
	{
		sc::writeLogError( std::string("DbAction(CLottoChangeInventWinNumAction): pAgentServer is NULL.") );
		return sc::db::DB_ERROR;
	}

	if( m_pDbManager->GetConnectionType() != db::ADO )
	{
		sc::writeLogError( "CLottoChangeInventWinNumAction::Execute. Use ADO" );
		return NET_ERROR;
	}

	int nReturn = NET_ERROR;
	nReturn = m_pDbManager->LottoChangeInventWinNum( m_turnNum, m_systemId );
	if( nReturn == sc::db::DB_ERROR )
	{
		sc::writeLogError( std::string("DbAction(CLottoChangeInventWinNumAction) failed to call CLottoChangeInventWinNumAction().") );
	}

	return nReturn;
}

CLottoWinMoneyUpdateAction::CLottoWinMoneyUpdateAction(	unsigned int turnNum
													   , unsigned int systemId )
													   : m_turnNum( turnNum )
													   , m_systemId( systemId )
{

}

CLottoWinMoneyUpdateAction::~CLottoWinMoneyUpdateAction() { }

int CLottoWinMoneyUpdateAction::Execute( NetServer* pServer )
{
	if( !pServer )
		return NET_ERROR;

	CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*>( pServer );
	if( !pAgentServer )
	{
		sc::writeLogError( std::string("DbAction(CLottoWinMoneyUpdateAction): pAgentServer is NULL.") );
		return sc::db::DB_ERROR;
	}

	if( m_pDbManager->GetConnectionType() != db::ADO )
	{
		sc::writeLogError( "CLottoWinMoneyUpdateAction::Execute. Use ADO" );
		return NET_ERROR;
	}

	int nReturn = NET_ERROR;
	nReturn = m_pDbManager->LottoWinMoneyUpdate( m_turnNum, m_systemId );
	if( nReturn == sc::db::DB_ERROR )
	{
		sc::writeLogError( std::string("DbAction(CLottoWinMoneyUpdateAction) failed to call CLottoWinMoneyUpdateAction().") );
	}

	return nReturn;
}

CLottoSystemBuyLog::CLottoSystemBuyLog( const LottoSystem::LOTTO_SYSTEM_BUY_LOG& logInfo )
: m_log( logInfo )
{

}

CLottoSystemBuyLog::~CLottoSystemBuyLog() { }

int CLottoSystemBuyLog::Execute( NetServer* pServer )
{
	if( !pServer )
		return NET_ERROR;

	CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*>( pServer );
	if( !pAgentServer )
	{
		sc::writeLogError( std::string("DbAction(CLottoSystemBuyLog): pAgentServer is NULL.") );
		return sc::db::DB_ERROR;
	}

	if( m_pDbManager->GetConnectionType() != db::ADO )
	{
		sc::writeLogError( "CLottoSystemBuyLog::Execute. Use ADO" );
		return NET_ERROR;
	}

	int nReturn = NET_ERROR;
	nReturn = m_pDbManager->LottoSystemBuyLog( m_log );
	if( nReturn == sc::db::DB_ERROR )
	{
		sc::writeLogError( std::string("DbAction(CLottoSystemBuyLog) failed to call CLottoSystemBuyLog().") );
	}

	return nReturn;
}

CLottoSystemWinManLog::CLottoSystemWinManLog( unsigned int turnNum
											 , unsigned int systemId )
											 : m_turnNum( turnNum )
											 , m_systemId( systemId )
{

}

CLottoSystemWinManLog::~CLottoSystemWinManLog() { }

int CLottoSystemWinManLog::Execute( NetServer* pServer )
{
	if( !pServer )
		return NET_ERROR;

	CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*>( pServer );
	if( !pAgentServer )
	{
		sc::writeLogError( std::string("DbAction(CLottoSystemWinManLog): pAgentServer is NULL.") );
		return sc::db::DB_ERROR;
	}

	if( m_pDbManager->GetConnectionType() != db::ADO )
	{
		sc::writeLogError( "CLottoSystemWinManLog::Execute. Use ADO" );
		return NET_ERROR;
	}

	int nReturn = NET_ERROR;
	nReturn = m_pDbManager->LottoSystemWinManLog( m_turnNum, m_systemId );
	if( nReturn == sc::db::DB_ERROR )
	{
		sc::writeLogError( std::string("DbAction(CLottoSystemWinManLog) failed to call CLottoSystemWinManLog().") );
	}

	return nReturn;
}

CLottoSystemTurnInfoLog::CLottoSystemTurnInfoLog(
	const LottoSystem::LOTTO_SYSTEM_TURN_LOG& logInfo )
	: m_log( logInfo )
{

}

CLottoSystemTurnInfoLog::~CLottoSystemTurnInfoLog() { }

int CLottoSystemTurnInfoLog::Execute( NetServer* pServer )
{
	if( !pServer )
		return NET_ERROR;

	CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*>( pServer );
	if( !pAgentServer )
	{
		sc::writeLogError( std::string("DbAction(CLottoSystemTurnInfoLog): pAgentServer is NULL.") );
		return sc::db::DB_ERROR;
	}

	if( m_pDbManager->GetConnectionType() != db::ADO )
	{
		sc::writeLogError( "CLottoSystemTurnInfoLog::Execute. Use ADO" );
		return NET_ERROR;
	}

	int nReturn = NET_ERROR;
	nReturn = m_pDbManager->LottoSystemTurnInfoLog( m_log );
	if( nReturn == sc::db::DB_ERROR )
	{
		sc::writeLogError( std::string("DbAction(CLottoSystemTurnInfoLog) failed to call CLottoSystemTurnInfoLog().") );
	}

	return nReturn;
}