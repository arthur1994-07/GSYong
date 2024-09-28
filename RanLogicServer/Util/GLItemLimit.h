#ifndef _GL_ITEM_LIMIT_H_
#define _GL_ITEM_LIMIT_H_

#pragma once

#include <boost/tr1/unordered_map.hpp>
#include "../../SigmaCore/Math/Random.h"
#include "../../SigmaCore/String/MinGuid.h"
#include "../../EngineLib/G-Logic/TypeDefine.h"
#include "../../RanLogic/Item/GLItemLmt.h"
#include "../../RanLogic/Item/GLItemDef.h"

struct SITEMCUSTOM;

//
//mjeon.dbman
//
namespace db {
    class IDbManager;
}

namespace gamelog {
    class LogItemMan;
}

namespace sc {
    class AutoTxtFile;
}

class DxMsgServer;
class TxtToLogDbInserter;

class GLItemLimit
{
public:
    GLItemLimit( 
        DxMsgServer* pMsgServer,
        db::IDbManager* pDbMan,
        db::IDbManager* pAdoMan,
        DWORD dwFieldID = UINT_MAX );
    ~GLItemLimit();
    
private:
    enum { VERSION = 0x0001, };

protected:
    CRITICAL_SECTION m_Lock;
    CRITICAL_SECTION m_LogSqlLock;
	CRITICAL_SECTION m_GuidLock;
    
    db::IDbManager*	m_pDBMan;
    db::IDbManager*	m_pAdoMan;
    DxMsgServer*	m_pMsgServer;
    int				m_nSGNum;
    int				m_nSvrNum;
    DWORD			m_dwFieldID;

    sc::RandomInt m_RandomInt;
    gamelog::LogItemMan* m_pLogItem;

    //! Text log file
    sc::AutoTxtFile* m_pLogSqlFile;
    
    //! Text to log
    TxtToLogDbInserter* m_pLogDbInserter;

	LONG m_Data3;	
	unsigned char m_Data4[6];

public:
    void ReadMaxKey();
    void ReadLogItem();

    //! 발생 가능한지 점검. ( 발생 여유 갯수가 있는지 보는것. )
    bool DoCheckItemGen( const SNATIVEID& sNID, EMITEMGEN emTYPE = EMGEN_DEFAULT );

	// 통계 및 로그 수정
    void LogItemExchange( 
        const SITEMCUSTOM& sITEM,
        gamelog::EMIDTYPE emFROME, DWORD dwFROMID,
        gamelog::EMIDTYPE emTO, DWORD dwTOID,
        EMITEM_ROUTE emROUTE,
        int nNum, const MapID &sMapID, EMITEM_ROUTE_DETAIL emFlagDetail, bool bForceLog );
	
	// 통계 및 로그 수정
    void LogMoneyExchange( 
        gamelog::EMIDTYPE emFROM, DWORD dwFrom,
        gamelog::EMIDTYPE emTO, DWORD dwTo,
        const LONGLONG& lnPrice,
        EMITEM_ROUTE nFlag, const MapID &sMapID, EMITEM_ROUTE_DETAIL emFlagDetail );

	void LogMoneyExchangeWithItemGuid( 
		gamelog::EMIDTYPE emFROM, DWORD dwFrom,
		gamelog::EMIDTYPE emTO, DWORD dwTo,
		const LONGLONG& lnPrice,
		EMITEM_ROUTE nFlag, const MapID &sMapID, EMITEM_ROUTE_DETAIL emFlagDetail, sc::MGUID& guid );
    
    void LogItemConversion( const SITEMCUSTOM& sITEM, gamelog::EMIDTYPE emFROM, DWORD dwFrom, EMITEM_ROUTE emFlag, EMITEM_ROUTE_DETAIL emFlagDetail );

    void LogItemRandomOption( const SITEMCUSTOM& sITEM, int InsertOption = 0 );
	void LogItemCostumeState( int iCharDbNum, const SITEMCUSTOM& sITEM );

	void LogItemAddonOption( const SITEMCUSTOM& sITEM );
	void LogItemLinkSkillOption( const SITEMCUSTOM& sITEM );
	void LogItemBasicStatOption( const SITEMCUSTOM& sITEM );

	// 통계 및 로그 수정
    void LogCharAction(
        DWORD dwCI,
        gamelog::EMACTION emACT,
        gamelog::EMIDTYPE emTAR,
        DWORD dwTAR,
        const __int64& nEXP,
        int nBRIGHT,
        int nLIFE,
        int nMONEY,
		const MapID &sMapID );

    //! Pet Action Log
    void LogPetAction( int nPetNum, const SNATIVEID& nItemID, pet::EM_ACTION emAction, int nPetFull );

    //! Vehicle Action Log
    void LogVehicleAction( int nVehicleNum, const SNATIVEID& nItemID, EMVEHICLE_ACTION emAction, int nVehicleFull );

    //! Txt->DB thread work
    void TxtLogToDB();

    //! Txt->DB force thread work
    void TxtLogToDBForce();

    bool MustWriteLogItem( EMITEM_TYPE emTYPE );

	DWORD getRandomDword();
};

#endif // _GL_ITEM_LIMIT_H_