#include "pch.h"

#include "../../SigmaCore/File/AutoTxtFile.h"
#include "../../SigmaCore/File/FileUtil.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/Util/Lock.h"
#include "../../SigmaCore/Util/SystemInfo.h"
#include "../../SigmaCore/Util/DateTime.h"
#include "../../SigmaCore/String/StringUtil.h"

#include "../../EngineLib/G-Logic/DxMsgServer.h"

#include "../../RanLogic/Msg/GLContrlBaseMsg.h"
#include "../../RanLogic/Msg/GLContrlCharJoinMsg.h"
#include "../../RanLogic/GLogicData.h"
#include "../../RanLogic/Character/GLCharDefine.h"
#include "../../RanLogic/Character/GLCharData.h"
#include "../../RanLogic/Guidance/GLGuidanceDefine.h"
#include "../../RanLogic/Club/GLClubDefine.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/GLogicDataMan.h"

#include "../Log/LogItemMan.h"
#include "../Net/NetLogDefine.h"
#include "../Database/DbDefine.h"
#include "../Database/DbManager.h"
#include "../Database/DbAction/DbActionGame.h"
#include "../Database/DbAction/DbActionLog.h"
#include "./TxtToLogDbInserter.h"
#include "./GLItemLimit.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

GLItemLimit::GLItemLimit( 
    DxMsgServer* pMsgServer,
    db::IDbManager* pDbMan,
    db::IDbManager* pAdoMan,
    DWORD dwFieldID /*= UINT_MAX*/ )
    : m_nSGNum( 0 )
    , m_nSvrNum( 0 )
    , m_dwFieldID( dwFieldID )
    , m_pDBMan( pDbMan )
    , m_pAdoMan( pAdoMan )
    , m_pMsgServer( pMsgServer )    
{
    InitializeCriticalSectionAndSpinCount( &m_Lock, sc::CRITICAL_SECTION_SPIN_COUNT );
    InitializeCriticalSectionAndSpinCount( &m_LogSqlLock, sc::CRITICAL_SECTION_SPIN_COUNT );
	InitializeCriticalSectionAndSpinCount( &m_GuidLock, sc::CRITICAL_SECTION_SPIN_COUNT );

    m_pLogItem = new gamelog::LogItemMan;
    
    std::string AppPath( sc::getAppPath() );
    AppPath.append( "\\DbLog" );
    sc::file::createDirectory( AppPath );

    m_pLogDbInserter = new TxtToLogDbInserter( m_pDBMan, m_pAdoMan, AppPath );

    std::string LogFile1( AppPath + "\\GameLog.sql" );
    m_pLogDbInserter->AddLogFile( "GameLog.sql" );
    
    const DWORD NewFileSize = 1000;

    m_pLogSqlFile = new sc::AutoTxtFile( LogFile1, NewFileSize, sc::TxtFile::ANSI );

	m_Data3 = 0;

	struct tm *newtime;
	__int64 ltime;
	::_time64( &ltime );
	newtime = _localtime64( &ltime );

	m_Data4[0] = newtime->tm_year;
	m_Data4[1] = newtime->tm_mon;
	m_Data4[2] = newtime->tm_mday;
	m_Data4[3] = newtime->tm_hour;
	m_Data4[4] = newtime->tm_min;
	m_Data4[5] = newtime->tm_sec; 	
}

GLItemLimit::~GLItemLimit( void )
{
    while ( m_pLogDbInserter->IsRunning() )
        Sleep( 100 );

    delete m_pLogDbInserter;
    delete m_pLogSqlFile;
    delete m_pLogItem;

    DeleteCriticalSection( &m_LogSqlLock );
    DeleteCriticalSection( &m_Lock );
	DeleteCriticalSection( &m_GuidLock );
}

void GLItemLimit::TxtLogToDB()
{
    m_pLogDbInserter->Run();
}

void GLItemLimit::TxtLogToDBForce()
{
    m_pLogSqlFile->ForceRenew();
    m_pLogDbInserter->Run();
}

void GLItemLimit::ReadMaxKey()
{
    if ( m_pMsgServer )
    {
        m_nSGNum = m_pMsgServer->GetServerGroup();
        m_nSvrNum = m_pMsgServer->GetServerNum();
    }
}

void GLItemLimit::ReadLogItem()
{
    if ( !m_pDBMan )
    {
        sc::writeLogError( std::string( "ReadLogItem m_pDBMan NULL" ) );
        return;
    }

    m_pLogItem->Reset();

    // 로그를 남길 아이템 정보를 가져온다
    std::vector< DWORD > LogItemData;
    m_pDBMan->GetLogItem( LogItemData );
    
    if ( LogItemData.empty() )
        sc::writeLogWarn( std::string( "ReadLogItem no data" ) );
	else
		sc::writeLogInfo( sc::string::format( "ReadLogItem size %1%", LogItemData.size() ) );

    for ( size_t i=0; i<LogItemData.size(); ++i )
        m_pLogItem->AddItem( LogItemData[i] );
}

//	Note : 생성 가능한지 검사하는 함수.
//
bool GLItemLimit::DoCheckItemGen( const SNATIVEID& sNID, EMITEMGEN emTYPE )
{
    const SITEM* pITEM = GLogicData::GetInstance().GetItem( sNID );
    if ( !pITEM )
        return false;
    else
        return true;
}

bool GLItemLimit::MustWriteLogItem( EMITEM_TYPE emTYPE )
{
    switch ( emTYPE )
    {
    case ITEM_RECALL:
    case ITEM_KEY:
    case ITEM_GRINDING:
    case ITEM_TICKET:
    case ITEM_SKP_RESET:
    case ITEM_STAT_RESET:
    case ITEM_SKP_STAT_RESET_A:
    case ITEM_LOUDSPEAKER:
    case ITEM_FIRECRACKER:
    case ITEM_CHARACTER_CARD:
    case ITEM_INVEN_CARD:
    case ITEM_STORAGE_CARD:
    case ITEM_STORAGE_CONNECT:
    case ITEM_PREMIUMSET:
    case ITEM_PRIVATEMARKET:
    case ITEM_DISJUNCTION:
    case ITEM_HAIR:
    case ITEM_FACE:
    case ITEM_CD:
    case ITEM_2FRIEND:
    case ITEM_CLUBCALL:
    case ITEM_HAIRSHOP:
    case ITEM_RENAME:
    case ITEM_HAIR_STYLE:
    case ITEM_HAIR_COLOR:
    case ITEM_REVIVE:
    case ITEM_PET_FOOD:
    case ITEM_PET_RENAME:
    case ITEM_PET_COLOR:
    case ITEM_PET_STYLE:
    case ITEM_PET_SKILL:
    case ITEM_SMS:
    case ITEM_ANTI_DISAPPEAR:
    case ITEM_REMODEL:
    case ITEM_VEHICLE_OIL:
    case ITEM_GENDER_CHANGE:
    case ITEM_GARBAGE_CARD:
    case ITEM_TELEPORT_CARD:
    case ITEM_PET_SKIN_PACK:
    case ITEM_FACE_STYLE:
    case ITEM_TAXI_CARD:    
    case ITEM_MATERIALS:
    case ITEM_NPC_RECALL:
    case ITEM_LUNCHBOX:
    case ITEM_PET_DUALSKILL:
    case ITEM_INC_GRINDINGRATE:
    case ITEM_ANTI_RESET:
    case ITEM_VEHICLE_COLOR:
    case ITEM_BIKE_BOOST_CARD:
    case ITEM_SKP_STAT_RESET_B:
    case ITEM_COSTUM_COLOR_CH_CD:
    case ITEM_POSTBOX_CARD:
        return true;

    default:
        return false;
    };

    return false;
}

// 통계 및 로그 수정
void GLItemLimit::LogItemExchange( 
    const SITEMCUSTOM& sITEM,
    gamelog::EMIDTYPE emFROME,
    DWORD dwFROMID,
    gamelog::EMIDTYPE emTO,
    DWORD dwTOID,
    EMITEM_ROUTE emROUTE,
    int nNum,
	const MapID &sMapID,
	EMITEM_ROUTE_DETAIL emFlagDetail,
    bool bForceLog )
{
	SNATIVEID sOriITEM;
	if( sITEM.IsBoxWrapped() )
	{
		sOriITEM = sITEM.m_PetDbNum;
	}

    // 아이템 정보를 가져옴.
    const SITEM* pITEM = GLogicData::GetInstance().GetItem( sITEM.sNativeID );
    if (!pITEM)
        return;

	int nBaseMainMapID = 0;
	int nBaseSubMapID = 0;
	int nGaeaMainMapID = 0;
	int nGaeaSubMapID = 0;

	if( sMapID.IsNULL() == FALSE )
	{
		nBaseMainMapID = sMapID.getBaseMapID().wMainID;
		nBaseSubMapID = sMapID.getBaseMapID().wSubID;
		nGaeaMainMapID = sMapID.getGaeaMapID().wMainID;
		nGaeaSubMapID = sMapID.getGaeaMapID().wSubID;
	}

    sc::CriticalSectionOwnerLock OwnerLock( m_LogSqlLock );

    if ( !m_pLogItem->IsLogItem( sITEM.sNativeID.Id() ) && GLogicData::GetInstance().GetServiceProvider() != SP_OFFICE_TEST )
    {
        if ( !MustWriteLogItem( pITEM->BasicType() ) || pITEM->ISPILE() )
            return;

		std::string strGuid = sc::string::uuidToString( ( UUID )sITEM.guid );
		sc::string::upperString( strGuid );

        // Text file        
		// 통계 및 로그 수정
        std::string SqlText( sc::string::format(
			"INSERT INTO dbo.ItemLog( \
			NIDMain,NIDSub,SGNum,SvrNum,FldNum,\
			MakeType,ItemUUID,ItemAmount,ItemFromFlag,ItemFrom,\
			ItemToFlag,ItemTo,ExchangeFlag,ExchangeDate,Damage,\
			Defense,Fire,Ice,Poison,Electric,\
			Spirit,CostumeMID,CostumeSID,TradePrice,baseMainMapID,\
			baseSubMapID, gaeaMainMapID, gaeaSubMapID, OriNIDMain, OriNIDSub,\
			ExchangeFlagDetail ) VALUES( \
			%1%,%2%,%3%,%4%,%5%,\
			%6%,'%7%',%8%,%9%,%10%,\
			%11%,%12%,%13%,'%14%',%15%,\
			%16%,%17%,%18%,%19%,%20%,\
			%21%,%22%,%23%,%24%,%25%,\
			%26%,%27%,%28%,%29%,%30%,\
			%31% );",

            sITEM.sNativeID.wMainID, 
			sITEM.sNativeID.wSubID, 
			static_cast< int >( m_nSGNum ), 
			m_nSvrNum, 
			m_dwFieldID,
            
			static_cast< int >( sITEM.cGenType ), 
			strGuid.c_str(), 
			nNum, 
			static_cast< int >( emFROME ), 
			static_cast< int >( dwFROMID ), 
            
			static_cast< int >( emTO ), 
			static_cast< int >( dwTOID ), 
			static_cast< int >( emROUTE ), 
			sc::string::GetCurrentTime(), 
			static_cast< int >( sITEM.grade_damage() ), 
            
			static_cast< int >( sITEM.grade_defense() ), 
			static_cast< int >( sITEM.grade_fire() ), 
			static_cast< int >( sITEM.grade_ice() ), 
			static_cast< int >( sITEM.grade_poison() ), 
			static_cast< int >( sITEM.grade_elec() ), 

            static_cast< int >( sITEM.grade_spirit() ), 
            sITEM.nidDISGUISE.wMainID, 
            sITEM.nidDISGUISE.wSubID, 
            0, 
			nBaseMainMapID, 
			
			nBaseSubMapID, 
			nGaeaMainMapID, 
			nGaeaSubMapID, 
			sOriITEM.wMainID, 
			sOriITEM.wSubID,

			emFlagDetail ) );

        m_pLogSqlFile->write( SqlText );
    }
    else
    {            
        // 2011-04-20 jgkim
        // Cache 서버로 전송
        if ( m_pMsgServer )
        {
            NET_LOG_ITEM_EXCHANGE Msg( 
                LOG_ITEM_EXCHANGE( 
                    sITEM.sNativeID.wMainID,
                    sITEM.sNativeID.wSubID,
                    m_nSGNum,
					m_nSvrNum,
					m_dwFieldID,
                    sITEM.cGenType,
                    sITEM.guid,
                    emFROME,
                    dwFROMID,
                    emTO,
                    dwTOID,
                    emROUTE,
                    nNum,
                    sITEM.nidDISGUISE.wMainID,
                    sITEM.nidDISGUISE.wSubID,
                    sITEM.grade_damage(),
                    sITEM.grade_defense(),
                    sITEM.grade_fire(),
                    sITEM.grade_ice(),
                    sITEM.grade_poison(),
                    sITEM.grade_elec(),
                    sITEM.grade_spirit(),
					nBaseMainMapID, nBaseSubMapID, nGaeaMainMapID, nGaeaSubMapID,
					sOriITEM.wMainID, sOriITEM.wSubID,
					emFlagDetail,
                    sc::time::GetCurrentTime() ) );
            m_pMsgServer->SendCache( &Msg );
        }
    }
}

// 통계 및 로그 수정
void GLItemLimit::LogMoneyExchange( 
    gamelog::EMIDTYPE emFROM, DWORD dwFrom,
    gamelog::EMIDTYPE emTO, DWORD dwTo,
    const LONGLONG& lnPrice,
    EMITEM_ROUTE nFlag, const MapID &sMapID, EMITEM_ROUTE_DETAIL emFlagDetail )
{    
    if ( m_pMsgServer )
    {
		int nBaseMainMapID = 0;
		int nBaseSubMapID = 0;
		int nGaeaMainMapID = 0;
		int nGaeaSubMapID = 0;

		if ( sMapID.IsNULL() == FALSE )
		{
			nBaseMainMapID = sMapID.getBaseMapID().wMainID;
			nBaseSubMapID = sMapID.getBaseMapID().wSubID;
			nGaeaMainMapID = sMapID.getGaeaMapID().wMainID;
			nGaeaSubMapID = sMapID.getGaeaMapID().wSubID;
		}

        sc::CriticalSectionOwnerLock OwnerLock( m_LogSqlLock );

        LONGLONG lnPriceAbsValue = lnPrice;
        if ( lnPriceAbsValue < 0 )
            lnPriceAbsValue = -lnPriceAbsValue;

        if ( lnPriceAbsValue < 1000000LL && nFlag != ITEM_ROUTE_ODD_EVEN && GLogicData::GetInstance().GetServiceProvider() != SP_OFFICE_TEST )
        {
			sc::MGUID guid;
			std::string strGuid = sc::string::uuidToString( ( UUID )guid );
			sc::string::upperString( strGuid );

			// Text file
			// 통계 및 로그 수정
			std::string SqlText( sc::string::format( 
				"INSERT INTO dbo.ItemLog( \
				NIDMain,NIDSub,SGNum,SvrNum,FldNum,\
				MakeType,ItemUUID,ItemAmount,ItemFromFlag,ItemFrom,\
				ItemToFlag,ItemTo,ExchangeFlag,ExchangeDate,Damage,\
				Defense,Fire,Ice,Poison,Electric,\
				Spirit,CostumeMID,CostumeSID,TradePrice, baseMainMapID,\
				baseSubMapID, gaeaMainMapID, gaeaSubMapID, OriNIDMain, OriNIDSub,\
				ExchangeFlagDetail ) VALUES( \
				%1%,%2%,%3%,%4%,%5%,\
				%6%,'%7%',%8%,%9%,%10%,\
				%11%,%12%,%13%,'%14%',%15%,\
				%16%,%17%,%18%,%19%,%20%,\
				%21%,%22%,%23%,%24%,%25%,\
				%26%,%27%,%28%,%29%,%30%,\
				%31% );",

				0,
				0,
				m_nSGNum,
				m_nSvrNum,
				sc::db::AdjustTinyInt( m_dwFieldID ),

				0,
				strGuid.c_str(),
				0,
				static_cast< int >( emFROM ),
				static_cast< int >( dwFrom ),

				static_cast< int >( emTO ),
				static_cast< int >( dwTo ),
				static_cast< int >( nFlag ),
				sc::string::GetCurrentTime(),
				0,

				0, 0, 0, 0, 0,

				0, 0, 0, lnPrice, nBaseMainMapID, 

				nBaseSubMapID, nGaeaMainMapID, nGaeaSubMapID, 0, 0, 

				emFlagDetail ) );
            m_pLogSqlFile->write( SqlText );
        }
        else
        {
			sc::MGUID guid;
            NET_LOG_MONEY_EXCHANGE Msg(
                LOG_MONEY_EXCHANGE(
                    m_nSGNum,
                    m_nSvrNum,
                    m_dwFieldID,
                    emFROM,
                    dwFrom,
                    emTO,
                    dwTo,
                    lnPrice,
                    nFlag,
					nBaseMainMapID, nBaseSubMapID, nGaeaMainMapID, nGaeaSubMapID,
					0, 0,
					emFlagDetail,
                    sc::time::GetCurrentTime(),
					guid ) );
            m_pMsgServer->SendCache( &Msg );
        }
    }
}

void GLItemLimit::LogMoneyExchangeWithItemGuid( 
	gamelog::EMIDTYPE emFROM, DWORD dwFrom,
	gamelog::EMIDTYPE emTO, DWORD dwTo,
	const LONGLONG& lnPrice,
	EMITEM_ROUTE nFlag, const MapID &sMapID, EMITEM_ROUTE_DETAIL emFlagDetail, sc::MGUID& guid )
{    
	if ( m_pMsgServer )
	{
		int nBaseMainMapID = 0;
		int nBaseSubMapID = 0;
		int nGaeaMainMapID = 0;
		int nGaeaSubMapID = 0;

		if ( sMapID.IsNULL() == FALSE )
		{
			nBaseMainMapID = sMapID.getBaseMapID().wMainID;
			nBaseSubMapID = sMapID.getBaseMapID().wSubID;
			nGaeaMainMapID = sMapID.getGaeaMapID().wMainID;
			nGaeaSubMapID = sMapID.getGaeaMapID().wSubID;
		}

		sc::CriticalSectionOwnerLock OwnerLock( m_LogSqlLock );

		LONGLONG lnPriceAbsValue = lnPrice;
		if ( lnPriceAbsValue < 0 )
			lnPriceAbsValue = -lnPriceAbsValue;

		if ( lnPriceAbsValue < 1000000LL && nFlag != ITEM_ROUTE_ODD_EVEN && GLogicData::GetInstance().GetServiceProvider() != SP_OFFICE_TEST )
		{
			std::string strGuid = sc::string::uuidToString( ( UUID )guid );
			sc::string::upperString( strGuid );

			// Text file
			// 통계 및 로그 수정
			std::string SqlText( sc::string::format( 
				"INSERT INTO dbo.ItemLog( \
				NIDMain,NIDSub,SGNum,SvrNum,FldNum,\
				MakeType,ItemUUID,ItemAmount,ItemFromFlag,ItemFrom,\
				ItemToFlag,ItemTo,ExchangeFlag,ExchangeDate,Damage,\
				Defense,Fire,Ice,Poison,Electric,\
				Spirit,CostumeMID,CostumeSID,TradePrice, baseMainMapID,\
				baseSubMapID, gaeaMainMapID, gaeaSubMapID, OriNIDMain, OriNIDSub,\
				ExchangeFlagDetail ) VALUES( \
				%1%,%2%,%3%,%4%,%5%,\
				%6%,'%7%',%8%,%9%,%10%,\
				%11%,%12%,%13%,'%14%',%15%,\
				%16%,%17%,%18%,%19%,%20%,\
				%21%,%22%,%23%,%24%,%25%,\
				%26%,%27%,%28%,%29%,%30%,\
				%31% );",

				0,
				0,
				m_nSGNum,
				m_nSvrNum,
				sc::db::AdjustTinyInt( m_dwFieldID ),

				0,
				strGuid.c_str(),
				0,
				static_cast< int >( emFROM ),
				static_cast< int >( dwFrom ),

				static_cast< int >( emTO ),
				static_cast< int >( dwTo ),
				static_cast< int >( nFlag ),
				sc::string::GetCurrentTime(),
				0,

				0, 0, 0, 0, 0,

				0, 0, 0, lnPrice, nBaseMainMapID, 

				nBaseSubMapID, nGaeaMainMapID, nGaeaSubMapID, 0, 0, 

				emFlagDetail ) );
			m_pLogSqlFile->write( SqlText );
		}
		else
		{
			NET_LOG_MONEY_EXCHANGE Msg(
				LOG_MONEY_EXCHANGE(
				m_nSGNum,
				m_nSvrNum,
				m_dwFieldID,
				emFROM,
				dwFrom,
				emTO,
				dwTo,
				lnPrice,
				nFlag,
				nBaseMainMapID, nBaseSubMapID, nGaeaMainMapID, nGaeaSubMapID,
				0, 0,
				emFlagDetail,
				sc::time::GetCurrentTime(),
				guid ) );
			m_pMsgServer->SendCache( &Msg );
		}
	}
}

void GLItemLimit::LogItemConversion( 
									const SITEMCUSTOM& sITEM, 
									gamelog::EMIDTYPE emFROM, 
									DWORD dwFrom, 
									EMITEM_ROUTE emFlag, 
									EMITEM_ROUTE_DETAIL emFlagDetail )
{
	SNATIVEID sOriITEM;

	if( sITEM.IsBoxWrapped() )
	{
		sOriITEM = sITEM.m_PetDbNum;
	}

    if ( m_pMsgServer )
    {
        sc::CriticalSectionOwnerLock OwnerLock( m_LogSqlLock );
        
        if( GLogicData::GetInstance().GetServiceProvider() != SP_OFFICE_TEST )
        {
			std::string strGuid = sc::string::uuidToString( ( UUID )sITEM.guid );
			sc::string::upperString( strGuid );

			// Text file        
			std::string SqlText( sc::string::format( 
				"INSERT INTO dbo.ItemLog( \
				NIDMain,NIDSub,SGNum,SvrNum,FldNum,\
				MakeType,ItemUUID,ItemAmount,ItemFromFlag,ItemFrom,\
				ItemToFlag,ItemTo,ExchangeFlag,ExchangeDate,Damage,\
				Defense,Fire,Ice,Poison,Electric,\
				Spirit,CostumeMID,CostumeSID,TradePrice, baseMainMapID,\
				baseSubMapID,gaeaMainMapID,gaeaSubMapID,OriNIDMain,OriNIDSub,\
				ExchangeFlagDetail ) VALUES( \
				%1%,%2%,%3%,%4%,%5%,\
				%6%,'%7%',%8%,%9%,%10%,\
				%11%,%12%,%13%,'%14%',%15%,\
				%16%,%17%,%18%,%19%,%20%,\
				%21%,%22%,%23%,%24%,%25%,\
				%26%,%27%,%28%,%29%,%30%,\
				%31% );",
				sITEM.sNativeID.wMainID,
				sITEM.sNativeID.wSubID,
				m_nSGNum,
				m_nSvrNum,
				sc::db::AdjustTinyInt( m_dwFieldID ),

				static_cast< int >( sITEM.cGenType ),
				strGuid.c_str(), 
				0,
				static_cast< int >( emFROM ),
				static_cast< int >( dwFrom ),

				0,
				0,
				static_cast< int >( emFlag ),
				sc::string::GetCurrentTime(),
				static_cast< int >( sITEM.grade_damage() ),

				static_cast< int >( sITEM.grade_defense() ),
				static_cast< int >( sITEM.grade_fire() ),
				static_cast< int >( sITEM.grade_ice() ),
				static_cast< int >( sITEM.grade_poison() ),
				static_cast< int >( sITEM.grade_elec() ),

				static_cast< int >( sITEM.grade_spirit() ),
				sITEM.nidDISGUISE.wMainID,
				sITEM.nidDISGUISE.wSubID,
				0, 
				0,

				0, 
				0, 
				0,
				sOriITEM.wMainID,
				sOriITEM.wSubID,

				emFlagDetail ) );
            m_pLogSqlFile->write( SqlText );
        }
        else
        {
            NET_LOG_ITEM_CONVERSION Msg(
                LOG_ITEM_CONVERSION(
                    sITEM.sNativeID.wMainID,
                    sITEM.sNativeID.wSubID,
					m_nSGNum,
					m_nSvrNum,
					m_dwFieldID,
                    sITEM.cGenType,
                    sITEM.guid,
                    emFROM,
                    dwFrom,
                    sITEM.nidDISGUISE.wMainID,
                    sITEM.nidDISGUISE.wSubID,
                    sITEM.grade_damage(),
                    sITEM.grade_defense(),
                    sITEM.grade_elec(),
                    sITEM.grade_fire(),
                    sITEM.grade_ice(),
                    sITEM.grade_poison(),
                    sITEM.grade_spirit(),
					0, 0, 0, 0,
					sOriITEM.wMainID,
					sOriITEM.wSubID,
					emFlagDetail,
                    sc::time::GetCurrentTime(),
					emFlag ) );
            m_pMsgServer->SendCache( &Msg );
        }
    }
}

// 통계 및 로그 수정
void GLItemLimit::LogCharAction( 
    DWORD dwCI,
    gamelog::EMACTION emACT,
    gamelog::EMIDTYPE emTAR,
    DWORD dwTAR,
    const __int64& nEXP,
    int nBRIGHT,
    int nLIFE,
    int nMONEY,
	const MapID &sMapID )
{
    if ( m_pMsgServer )
    {
		int nBaseMainMapID = 0;
		int nBaseSubMapID = 0;
		int nGaeaMainMapID = 0;
		int nGaeaSubMapID = 0;

		if( sMapID.IsNULL() == FALSE )
		{
			nBaseMainMapID = sMapID.getBaseMapID().wMainID;
			nBaseSubMapID = sMapID.getBaseMapID().wSubID;
			nGaeaMainMapID = sMapID.getGaeaMapID().wMainID;
			nGaeaSubMapID = sMapID.getGaeaMapID().wSubID;
		}

        sc::CriticalSectionOwnerLock OwnerLock( m_LogSqlLock );

        if( GLogicData::GetInstance().GetServiceProvider() != SP_OFFICE_TEST )
        {
			// Text file        
			std::string SqlText( sc::string::format( 
				"INSERT INTO dbo.LogAction( \
				ChaNum,Type,TargetNum,TargetType,BrightPoint,\
				LifePoint,ExpPoint,ActionMoney,ActionDate, baseMainMapID,\
				baseSubMapID, gaeaMainMapID, gaeaSubMapID ) VALUES ( \
				%1%,%2%,%3%,%4%,%5%,\
				%6%,%7%,%8%,'%9%',%10%,\
				%11%,%12%,%13% );",
				dwCI,
				static_cast< int >( emACT ),
				dwTAR,
				static_cast< int >( emTAR ),
				nBRIGHT,

				nLIFE,
				nEXP,
				nMONEY,
				sc::string::GetCurrentTime(),
				nBaseMainMapID, 

				nBaseSubMapID, 
				nGaeaMainMapID, 
				nGaeaSubMapID ) );
            m_pLogSqlFile->write( SqlText );
        }
        else
        {
            NET_LOG_ACTION Msg( 
                LOG_ACTION( 
                    dwCI, emACT,
                    emTAR,dwTAR,
                    nEXP, nBRIGHT, nLIFE, nMONEY,
					nBaseMainMapID, nBaseSubMapID, nGaeaMainMapID, nGaeaSubMapID,
                    sc::time::GetCurrentTime() ) );
            m_pMsgServer->SendCache( &Msg );
        }
    }
}

void GLItemLimit::LogItemRandomOption( const SITEMCUSTOM& sITEM, int InsertOption /*= 0*/ )
{
    if ( m_pMsgServer )
    {
        sc::CriticalSectionOwnerLock OwnerLock( m_LogSqlLock );
        
        if( GLogicData::GetInstance().GetServiceProvider() != SP_OFFICE_TEST )
        {
			std::string strGuid = sc::string::uuidToString( ( UUID )sITEM.guid );
			sc::string::upperString( strGuid );

			// Text file
			std::string SqlText( sc::string::format( 
				"INSERT INTO dbo.ItemLogRandom( \
				NIDMain,NIDSub,SGNum,SvrNum,FldNum,\
				MakeType,ItemUUID,RandomType1,RandomValue1,RandomStamp1,\
				RandomType2,RandomValue2,RandomStamp2,RandomType3,RandomValue3,\
				RandomStamp3,RandomType4,RandomValue4,RandomStamp4,RandomDate,\
				InsertOption,RandomType5,RandomValue5,RandomStamp5 ) VALUES ( \
				%1%,%2%,%3%,%4%,%5%,\
				%6%,'%7%',%8%,%9%,%10%,\
				%11%,%12%,%13%,%14%,%15%,\
				%16%,%17%,%18%,%19%,'%20%',\
				%21%,%22%,%23%,%24% );",
				sITEM.sNativeID.wMainID,
				sITEM.sNativeID.wSubID,
				m_nSGNum,
				m_nSvrNum,
				sc::db::AdjustTinyInt( m_dwFieldID ),

				static_cast< int >( sITEM.cGenType ),
				strGuid.c_str(), 
				static_cast< int >( sITEM.GETOptTYPE1() ),
				sITEM.randomOption.getValue( 0 ),
				sITEM.getOptCount( 0 ),

				static_cast< int >( sITEM.GETOptTYPE2() ),
				sITEM.randomOption.getValue( 1 ),
				sITEM.getOptCount( 1 ),
				static_cast< int >( sITEM.GETOptTYPE3() ),
				sITEM.randomOption.getValue( 2 ),

				sITEM.getOptCount( 2 ),
				static_cast< int >( sITEM.GETOptTYPE4() ),
				sITEM.randomOption.getValue( 3 ),
				sITEM.getOptCount( 3 ), 
				sc::string::GetCurrentTime(), 

				InsertOption, 
				
				static_cast< int >( sITEM.GETOptTYPE5() ),
				sITEM.randomOption.getValue( 4 ),
				sITEM.getOptCount( 4 ) ) );
            m_pLogSqlFile->write( SqlText );
        }
        else
        {
            NET_LOG_RANDOM_OPTION Msg( 
                LOG_RANDOM_OPTION( 
                    sITEM.GetNativeID().wMainID,
					sITEM.GetNativeID().wSubID,
					m_nSGNum,
					m_nSvrNum,
					m_dwFieldID,
                    sITEM.cGenType,
					sITEM.guid,
                    sITEM.GETOptTYPE1(),
					sITEM.GETOptTYPE2(),
					sITEM.GETOptTYPE3(),
					sITEM.GETOptTYPE4(),
					sITEM.GETOptTYPE5(),
                    sITEM.randomOption.getValue( 0 ),
					sITEM.randomOption.getValue( 1 ),
					sITEM.randomOption.getValue( 2 ),
					sITEM.randomOption.getValue( 3 ),
					sITEM.randomOption.getValue( 4 ),
                    static_cast< BYTE >( sITEM.getOptCount( 0 ) ),
					static_cast< BYTE >( sITEM.getOptCount( 1 ) ),
					static_cast< BYTE >( sITEM.getOptCount( 2 ) ),
					static_cast< BYTE >( sITEM.getOptCount( 3 ) ), 
					static_cast< BYTE >( sITEM.getOptCount( 4 ) ), 
                    sc::time::GetCurrentTime(),
					InsertOption ) );
            m_pMsgServer->SendCache( &Msg );
        }
    }
}

void GLItemLimit::LogItemCostumeState( int iCharDbNum, const SITEMCUSTOM& sITEM )
{
	const SITEM* pITEM = GLogicData::GetInstance().GetItem( sITEM.GetNativeID() );

	if( pITEM == NULL ) return;

	if ( m_pMsgServer )
	{				
		sc::CriticalSectionOwnerLock OwnerLock( m_LogSqlLock );

		int iParts = static_cast< int >( pITEM->sSuitOp.emSuit );

		SITEMCUSTOM sTempData = sITEM;

		if( GLogicData::GetInstance().GetServiceProvider() != SP_OFFICE_TEST )
		{
			std::string FireDateStr = sc::time::DateTimeFormat( sITEM.costumeUser.tEndTime, true, true );

			std::string strGuid = sc::string::uuidToString( ( UUID )sITEM.guid );
			sc::string::upperString( strGuid );

			// Text file
			std::string SqlText(
				sc::string::format(
				"INSERT INTO dbo.ItemLogCostumeStats( \
				ChaDbNum,NIDMain,NIDSub,SGNum,SvrNum,\
				FldNum,MakeType,ItemUUID,FireDate,StatType1,\
				StatValue1,StatType2,StatValue2,StatType3,StatValue3,\
				StatType4,StatValue4,StatType5,StatValue5,StatType6,\
				StatValue6,StatType7,StatValue7 ) \
				VALUES ( \
				%1%,%2%,%3%,%4%,%5%,\
				%6%,%7%,'%8%','%9%',%10%,\
				%11%,%12%,%13%,%14%,%15%,\
				%16%,%17%,%18%,%19%,%20%,\
				%21%,%22%,%23% );",

				iCharDbNum,
				sITEM.sNativeID.wMainID,
				sITEM.sNativeID.wSubID,
				m_nSGNum,
				m_nSvrNum,

				sc::db::AdjustTinyInt( m_dwFieldID ),
				static_cast< int >( sITEM.cGenType ),
				strGuid.c_str(), 
				FireDateStr.c_str(),
				static_cast< int >( sITEM.costumeUser.Invest[0].cStatType ),

				sTempData.costumeUser.getValue( 0, iParts ),
				static_cast< int >( sITEM.costumeUser.Invest[1].cStatType ),
				sTempData.costumeUser.getValue( 1, iParts ),
				static_cast< int >( sITEM.costumeUser.Invest[2].cStatType ),
				sTempData.costumeUser.getValue( 2, iParts ),

				static_cast< int >( sITEM.costumeUser.Invest[3].cStatType ),
				sTempData.costumeUser.getValue( 3, iParts ),
				static_cast< int >( sITEM.costumeUser.Invest[4].cStatType ),
				sTempData.costumeUser.getValue( 4, iParts ),
				static_cast< int >( sITEM.costumeUser.Invest[5].cStatType ),

				sTempData.costumeUser.getValue( 5, iParts ),
				static_cast< int >( sITEM.costumeUser.Invest[6].cStatType ),
				sTempData.costumeUser.getValue( 6, iParts ) ) );
			m_pLogSqlFile->write(SqlText);
		}
		else
		{
			NET_LOG_COSTUME_STATS Msg( 
				LOG_COSTUME_STATS( 
				iCharDbNum, 
				sITEM.GetNativeID().wMainID,
				sITEM.GetNativeID().wSubID,
				m_nSGNum,
				m_nSvrNum,
				m_dwFieldID,
				sITEM.cGenType,
				sITEM.guid,
				sITEM.costumeUser.tEndTime,
				static_cast< int >( sITEM.costumeUser.Invest[0].cStatType ),
				static_cast< int >( sITEM.costumeUser.Invest[1].cStatType ),
				static_cast< int >( sITEM.costumeUser.Invest[2].cStatType ),
				static_cast< int >( sITEM.costumeUser.Invest[3].cStatType ),
				static_cast< int >( sITEM.costumeUser.Invest[4].cStatType ),
				static_cast< int >( sITEM.costumeUser.Invest[5].cStatType ),
				static_cast< int >( sITEM.costumeUser.Invest[6].cStatType ),

				sTempData.costumeUser.getValue( 0, iParts ),
				sTempData.costumeUser.getValue( 1, iParts ),
				sTempData.costumeUser.getValue( 2, iParts ),
				sTempData.costumeUser.getValue( 3, iParts ),
				sTempData.costumeUser.getValue( 4, iParts ),
				sTempData.costumeUser.getValue( 5, iParts ),
				sTempData.costumeUser.getValue( 6, iParts ),

				sc::time::GetCurrentTime() ) );

			m_pMsgServer->SendCache( &Msg );
		}
	}
}

void GLItemLimit::LogItemAddonOption( const SITEMCUSTOM& sITEM )
{
	const SITEM* pItemData = GLogicData::GetInstance().GetItem( sITEM.GetNativeID() );
	if ( !pItemData )
		return;

	// 겹침 아이템은 로그 남기지 않는다.
	if ( pItemData->ISPILE() )
		return;

	if ( m_pMsgServer )
	{
		sc::CriticalSectionOwnerLock OwnerLock( m_LogSqlLock );

		if ( GLogicData::GetInstance().GetServiceProvider() != SP_OFFICE_TEST )
		{
			std::string strGuid = sc::string::uuidToString( ( UUID )sITEM.guid );
			sc::string::upperString( strGuid );

			// Text file
			std::string SqlText( sc::string::format( 
				"INSERT INTO dbo.ItemLogAddon( \
				NIDMain,NIDSub,SGNum,SvrNum,FldNum, MakeType,ItemUUID, \
				AddonType1, AddonValue1, AddonValueF1,\
				AddonType2, AddonValue2, AddonValueF2,\
				AddonType3, AddonValue3, AddonValueF3,\
				AddonType4, AddonValue4, AddonValueF4,\
				AddonType5, AddonValue5, AddonValueF5,\
				AddonType6, AddonValue6, AddonValueF6,\
				AddonDate ) VALUES ( \
				%1%,%2%,%3%,%4%,%5%,%6%,'%7%',\
				%8%,%9%,%10%,\
				%11%,%12%,%13%,\
				%14%,%15%,%16%,\
				%17%,%18%,%19%,\
				%20%,%21%,%22%,\
				%23%,%24%,%25%,\
				'%26%' );",
				sITEM.sNativeID.wMainID,
				sITEM.sNativeID.wSubID,
				m_nSGNum,
				m_nSvrNum,
				sc::db::AdjustTinyInt( m_dwFieldID ),
				static_cast< int >( sITEM.cGenType ),
				strGuid.c_str(), 

				static_cast< short >( sITEM.sAddonInfo.m_sADDON[0].emTYPE ),
				sITEM.sAddonInfo.m_sADDON[0].nVALUE, 
				sITEM.sAddonInfo.m_sADDON[0].fVALUE, 

				static_cast< short >( sITEM.sAddonInfo.m_sADDON[1].emTYPE ),
				sITEM.sAddonInfo.m_sADDON[1].nVALUE, 
				sITEM.sAddonInfo.m_sADDON[1].fVALUE, 

				static_cast< short >( sITEM.sAddonInfo.m_sADDON[2].emTYPE ),
				sITEM.sAddonInfo.m_sADDON[2].nVALUE, 
				sITEM.sAddonInfo.m_sADDON[2].fVALUE, 

				static_cast< short >( sITEM.sAddonInfo.m_sADDON[3].emTYPE ),
				sITEM.sAddonInfo.m_sADDON[3].nVALUE, 
				sITEM.sAddonInfo.m_sADDON[3].fVALUE, 

				static_cast< short >( sITEM.sAddonInfo.m_sADDON[4].emTYPE ),
				sITEM.sAddonInfo.m_sADDON[4].nVALUE, 
				sITEM.sAddonInfo.m_sADDON[4].fVALUE, 

				static_cast< short >( sITEM.sAddonInfo.m_sADDON[5].emTYPE ),
				sITEM.sAddonInfo.m_sADDON[5].nVALUE, 
				sITEM.sAddonInfo.m_sADDON[5].fVALUE, 

				sc::string::GetCurrentTime() ) );
			m_pLogSqlFile->write( SqlText );
		}
		else
		{
			NET_LOG_ADDON_OPTION Msg( 
				LOG_ADDON_OPTION( 
				sITEM.GetNativeID().wMainID,
				sITEM.GetNativeID().wSubID,
				m_nSGNum,
				m_nSvrNum,
				m_dwFieldID,
				sITEM.cGenType,
				sITEM.guid,
				static_cast< int >( sITEM.sAddonInfo.m_sADDON[0].emTYPE ),
				static_cast< int >( sITEM.sAddonInfo.m_sADDON[1].emTYPE ),
				static_cast< int >( sITEM.sAddonInfo.m_sADDON[2].emTYPE ),
				static_cast< int >( sITEM.sAddonInfo.m_sADDON[3].emTYPE ),
				static_cast< int >( sITEM.sAddonInfo.m_sADDON[4].emTYPE ),
				static_cast< int >( sITEM.sAddonInfo.m_sADDON[5].emTYPE ),
				sITEM.sAddonInfo.m_sADDON[0].nVALUE, 
				sITEM.sAddonInfo.m_sADDON[1].nVALUE, 
				sITEM.sAddonInfo.m_sADDON[2].nVALUE, 
				sITEM.sAddonInfo.m_sADDON[3].nVALUE, 
				sITEM.sAddonInfo.m_sADDON[4].nVALUE, 
				sITEM.sAddonInfo.m_sADDON[5].nVALUE, 
				sITEM.sAddonInfo.m_sADDON[0].fVALUE, 
				sITEM.sAddonInfo.m_sADDON[1].fVALUE, 
				sITEM.sAddonInfo.m_sADDON[2].fVALUE, 
				sITEM.sAddonInfo.m_sADDON[3].fVALUE, 
				sITEM.sAddonInfo.m_sADDON[4].fVALUE, 
				sITEM.sAddonInfo.m_sADDON[5].fVALUE, 
				sc::time::GetCurrentTime() ) );
			m_pMsgServer->SendCache( &Msg );
		}
	}
}

void GLItemLimit::LogItemLinkSkillOption( const SITEMCUSTOM& sITEM )
{
	const SITEM* pItemData = GLogicData::GetInstance().GetItem( sITEM.GetNativeID() );
	if ( !pItemData )
		return;

	// 겹침 아이템은 로그 남기지 않는다.
	if ( pItemData->ISPILE() )
		return;

	if ( m_pMsgServer )
	{
		sc::CriticalSectionOwnerLock OwnerLock( m_LogSqlLock );

		if ( GLogicData::GetInstance().GetServiceProvider() != SP_OFFICE_TEST )
		{
			std::string strGuid = sc::string::uuidToString( ( UUID )sITEM.guid );
			sc::string::upperString( strGuid );

			// Text file
			std::string SqlText( sc::string::format( 
				"INSERT INTO dbo.ItemLogLinkSkill( \
				NIDMain,NIDSub,SGNum,SvrNum,FldNum, MakeType,ItemUUID, \
				LinkSkillMID,LinkSkillSID,LinkSkillLevel,LinkSkillTarget,LinkSkillOccurRate,\
				LinkSkillDate ) VALUES ( \
				%1%,%2%,%3%,%4%,%5%,%6%,'%7%',\
				%8%,%9%,%10%,%11%,%12%,\
				'%13%' );",
				sITEM.sNativeID.wMainID,
				sITEM.sNativeID.wSubID,
				m_nSGNum,
				m_nSvrNum,
				sc::db::AdjustTinyInt( m_dwFieldID ),
				static_cast< int >( sITEM.cGenType ),
				strGuid.c_str(), 

				static_cast< short >( sITEM.GetLinkSkillID().wMainID ),
				static_cast< short >( sITEM.GetLinkSkillID().wSubID ),
				static_cast< short >( sITEM.GetLinkSkillLevel() ),
				static_cast< short >( sITEM.GetLinkSkillTarget() ),
				sITEM.GetLinkSkillOccurRate(), 

				sc::string::GetCurrentTime() ) );
			m_pLogSqlFile->write( SqlText );
		}
		else
		{
			NET_LOG_LINKSKILL_OPTION Msg( 
				LOG_LINKSKILL_OPTION( 
				sITEM.GetNativeID().wMainID,
				sITEM.GetNativeID().wSubID,
				m_nSGNum,
				m_nSvrNum,
				m_dwFieldID,
				sITEM.cGenType,
				sITEM.guid,
				sITEM.GetLinkSkillTarget(),
				sITEM.GetLinkSkillID().wMainID,
				sITEM.GetLinkSkillID().wSubID,
				sITEM.GetLinkSkillLevel(),
				sITEM.GetLinkSkillOccurRate(), 
				sc::time::GetCurrentTime() ) );
			m_pMsgServer->SendCache( &Msg );
		}
	}
}

void GLItemLimit::LogItemBasicStatOption( const SITEMCUSTOM& sITEM )
{
	const SITEM* pItemData = GLogicData::GetInstance().GetItem( sITEM.GetNativeID() );
	if ( !pItemData )
		return;

	// 겹침 아이템은 로그 남기지 않는다.
	if ( pItemData->ISPILE() )
		return;

	if ( m_pMsgServer )
	{
		sc::CriticalSectionOwnerLock OwnerLock( m_LogSqlLock );

		if ( GLogicData::GetInstance().GetServiceProvider() != SP_OFFICE_TEST )
		{
			std::string strGuid = sc::string::uuidToString( ( UUID )sITEM.guid );
			sc::string::upperString( strGuid );

			// Text file
			std::string SqlText( sc::string::format( 
				"INSERT INTO dbo.ItemLogBasicStat( \
				NIDMain,NIDSub,SGNum,SvrNum,FldNum, MakeType,ItemUUID, \
				BasicAttackDamageMin,BasicAttackDamageMax,BasicDefence,\
				BasicStatDate ) VALUES ( \
				%1%,%2%,%3%,%4%,%5%,%6%,'%7%',\
				%8%,%9%,%10%,\
				'%11%' );",
				sITEM.sNativeID.wMainID,
				sITEM.sNativeID.wSubID,
				m_nSGNum,
				m_nSvrNum,
				sc::db::AdjustTinyInt( m_dwFieldID ),
				static_cast< int >( sITEM.cGenType ),
				strGuid.c_str(), 

				static_cast< short >( sITEM.GetBasicAttackDamage().wNow ),
				static_cast< short >( sITEM.GetBasicAttackDamage().wMax ),
				static_cast< short >( sITEM.GetBasicDefence() ),

				sc::string::GetCurrentTime() ) );
			m_pLogSqlFile->write( SqlText );
		}
		else
		{
			NET_LOG_BASICSTAT_OPTION Msg( 
				LOG_BASICSTAT_OPTION( 
				sITEM.GetNativeID().wMainID,
				sITEM.GetNativeID().wSubID,
				m_nSGNum,
				m_nSvrNum,
				m_dwFieldID,
				sITEM.cGenType,
				sITEM.guid,
				sITEM.GetBasicAttackDamage().wNow,
				sITEM.GetBasicAttackDamage().wMax,
				sITEM.GetBasicDefence(),
				sc::time::GetCurrentTime() ) );
			m_pMsgServer->SendCache( &Msg );
		}
	}
}

void GLItemLimit::LogPetAction( int nPetNum, const SNATIVEID& nItemID, pet::EM_ACTION emAction, int nPetFull )
{    
    if ( m_pMsgServer )
    {
        sc::CriticalSectionOwnerLock OwnerLock( m_LogSqlLock );
        
        if( GLogicData::GetInstance().GetServiceProvider() != SP_OFFICE_TEST )
        {
            // Text file
            std::string SqlText(
                sc::string::format(
                    "INSERT INTO dbo.LogPetAction( PetNum,ItemMID,ItemSID,ActionType,PetFull,LogDate ) VALUES ( %1%,%2%,%3%,%4%,%5%,'%6%' );",
                    nPetNum,
                    nItemID.wMainID,
                    nItemID.wSubID,
                    static_cast< int >( emAction ),
                    nPetFull,
                    sc::string::GetCurrentTime() ) );
            m_pLogSqlFile->write( SqlText );
        }
        else
        {
            NET_LOG_PET_ACTION Msg( 
                LOG_PET_ACTION( 
                    nPetNum,
                    nItemID.wMainID,
                    nItemID.wSubID,
                    emAction,
                    nPetFull,
                    sc::time::GetCurrentTime() ) );
            m_pMsgServer->SendCache( &Msg );
        }
    }
}

void GLItemLimit::LogVehicleAction( int nVehicleNum, const SNATIVEID& nItemID, EMVEHICLE_ACTION emAction, int nVehicleFull )
{
    if ( m_pMsgServer )
    {
        sc::CriticalSectionOwnerLock OwnerLock( m_LogSqlLock );

        if( GLogicData::GetInstance().GetServiceProvider() != SP_OFFICE_TEST )
        {
            // Text file
            std::string SqlText(
                sc::string::format(
                    "INSERT INTO dbo.LogVehicleAction( VehicleNum,ItemMID,ItemSID,ActionType,VehicleBattery,LogDate ) VALUES ( %1%,%2%,%3%,%4%,%5%,'%6%' );",
                    nVehicleNum,
                    nItemID.wMainID,
                    nItemID.wSubID,
                    emAction,
                    nVehicleFull,
                    sc::string::GetCurrentTime() ) );
            m_pLogSqlFile->write( SqlText );
        }
        else
        {
            NET_LOG_VEHICLE_ACTION Msg(
                LOG_VEHICLE_ACTION(
                    nVehicleNum,
                    nItemID.wMainID,
                    nItemID.wSubID,
                    emAction,
                    nVehicleFull,
                    sc::time::GetCurrentTime() ) );
            m_pMsgServer->SendCache( &Msg );
        }
    }
}

DWORD GLItemLimit::getRandomDword()
{
	WORD hi = ( WORD )( rand() % RAND_MAX );
	WORD lo = ( WORD )( rand() % RAND_MAX );
	return ( ( DWORD ) hi << 16 ) | lo;
}
