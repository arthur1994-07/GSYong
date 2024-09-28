#include "../../pch.h"
#include "../../../SigmaCore/gassert.h"
#include "../../../SigmaCore/Log/LogMan.h"
#include "../../../SigmaCore/String/StringFormat.h"
#include "../../../SigmaCore/Util/DateTime.h"
#include "../DbDefineLog.h"
#include "./ADOManager.h"

#include "../../../RanLogic/GLogicData.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace db
{
// 통계 및 로그 수정
// int AdoManager::LogItemExchangeInsert(
//     int	Mid, // 주번호
//     int Sid, // 부번호
//     int ServerGroup, // 서버그룹
//     int ServerNum, // 서버번호
//     int FieldNum, // 필드번호
//     int	MakeType, // 생성타입
//     LONGLONG lnMakeNum, // 생성번호
//     int FromType, // from의 종류 ( 유저, 케릭터, 클럽 )
//     int ItemFrom, // 아이템 출발지
//     int ToType,  // to의 종류 ( 유저, 케릭터, 클럽 )
//     int ItemTo, // 아이템 목적지
//     int ExchangeFlag, // 교환 타입
//     int CountNum, // 개수    
//     int CostumeMid, // 코스툼 변환 정보
//     int CostumeSid, // 코스툼 변환 정보    
//     short Damage, // 연마 정보
//     short Defense, // 연마 정보
//     short Fire, // 연마 정보
//     short Ice, // 연마 정보
//     short Poison, // 연마 정보
//     short Electric, // 연마 정보
//     short Spirit, // 연마 정보
//     LONGLONG TradePrice, // 교환가격
// 	int BaseMainMapID,
// 	int BaseSubMapID,
// 	int GaeaMainMapID,
// 	int GaeaSubMapID,
// 	short nOriNIDMain,
// 	short nOriNIDSub,
// 	int nExchangeFlagDetail,
//     const __time64_t& LogDate // 로그시간
//     )
// {
//     sc::db::AdoExt Ado(m_LogDBConnString);
//     //APPEND_RPARAM_INT(Ado);
// 
//     APPEND_IPARAM_INT  (Ado, "@NIDMain", Mid);
//     APPEND_IPARAM_INT  (Ado, "@NIDSub", Sid);
//     APPEND_IPARAM_INT  (Ado, "@SGNum", ServerGroup);
//     APPEND_IPARAM_INT  (Ado, "@SvrNum", ServerNum);
//     APPEND_IPARAM_INT  (Ado, "@FldNum", sc::db::AdjustTinyInt(FieldNum));
// 
//     APPEND_IPARAM_INT  (Ado, "@MakeType", MakeType);
//     
//     lnMakeNum = sc::db::AdjustMoney(lnMakeNum);
//     APPEND_IPARAM_MONEY(Ado, "@MakeNum", lnMakeNum);
// 
//     APPEND_IPARAM_INT  (Ado, "@ItemAmount", CountNum);
//     APPEND_IPARAM_INT  (Ado, "@ItemFromFlag", FromType);
//     APPEND_IPARAM_INT  (Ado, "@ItemFrom", ItemFrom);
// 
//     APPEND_IPARAM_INT  (Ado, "@ItemToFlag", ToType);
//     APPEND_IPARAM_INT  (Ado, "@ItemTo", ItemTo);
//     APPEND_IPARAM_INT  (Ado, "@ExchangeFlag", ExchangeFlag);
//     APPEND_IPARAM_INT  (Ado, "@Damage",  static_cast<byte> (Damage));
//     APPEND_IPARAM_INT  (Ado, "@Defense", static_cast<byte> (Defense));
// 
//     APPEND_IPARAM_INT  (Ado, "@Fire",     static_cast<byte> (Fire));
//     APPEND_IPARAM_INT  (Ado, "@Ice",      static_cast<byte> (Ice));
//     APPEND_IPARAM_INT  (Ado, "@Poison",   static_cast<byte> (Poison));
//     APPEND_IPARAM_INT  (Ado, "@Electric", static_cast<byte> (Electric));
//     APPEND_IPARAM_INT  (Ado, "@Spirit",   static_cast<byte> (Spirit));
// 
//     APPEND_IPARAM_INT  (Ado, "@CostumeMID", CostumeMid);
//     APPEND_IPARAM_INT  (Ado, "@CostumeSID", CostumeSid);
// 
//     TradePrice = sc::db::AdjustMoney(TradePrice);
//     APPEND_IPARAM_MONEY(Ado, "@TradePrice", TradePrice);
// 
//     std::string DateStr = sc::time::DateTimeFormat(LogDate, true, true);
//     APPEND_IPARAM_VARCHAR(Ado, "@ExchangeDate", DateStr.c_str(), DateStr.length());
// 
// 	APPEND_IPARAM_INT     (Ado, "@nBaseMainMapID", BaseMainMapID);
// 	APPEND_IPARAM_INT     (Ado, "@nBaseSubMapID", BaseSubMapID);
// 	APPEND_IPARAM_INT     (Ado, "@nGaeaMainMapID", GaeaMainMapID);
// 	APPEND_IPARAM_INT     (Ado, "@nGaeaSubMapID", GaeaSubMapID);
// 
// 	APPEND_IPARAM_INT     (Ado, "@nOriNIDMain", nOriNIDMain);
// 	APPEND_IPARAM_INT     (Ado, "@nOriNIDSub", nOriNIDSub);
// 
// 	APPEND_IPARAM_INT     (Ado, "@nExchangeFlagDetail", nExchangeFlagDetail);
// 
//     //APPEND_OPARAM_INT  (Ado, "@nReturn");
// 
//     //return ExecuteStoredProcedureIntReturn("dbo.sp_logitemexchange_insert", Ado, "@nReturn");
// 	// 통계 및 로그 수정
//     return ExecuteStoredProcedure("dbo.LogItemExchangeInsertNew_20140829", Ado);
// }

int AdoManager::LogItemExchange( const LOG_ITEM_EXCHANGE& sEXCHANGE )
{
	// 값 복사 줄이기 위해서 각 행위마다 함수 따로 분리한다.
	sc::db::AdoExt Ado( m_LogDBConnString );

	APPEND_IPARAM_INT( Ado, "@NIDMain", sEXCHANGE.m_nNID_M );
	APPEND_IPARAM_INT( Ado, "@NIDSub", sEXCHANGE.m_nNID_S );
	APPEND_IPARAM_INT( Ado, "@SGNum", sEXCHANGE.m_nSGNum );
	APPEND_IPARAM_INT( Ado, "@SvrNum", sEXCHANGE.m_nSvrNum );
	APPEND_IPARAM_INT( Ado, "@FldNum", sc::db::AdjustTinyInt( sEXCHANGE.m_nFldNum ) );

	APPEND_IPARAM_INT( Ado, "@MakeType", sEXCHANGE.m_nMakeType );

	std::string strGuid = sc::string::uuidToString( ( UUID )sEXCHANGE.m_guid );
	sc::string::upperString( strGuid );
	APPEND_IPARAM_VARCHAR( Ado, "@ItemUUID", strGuid.c_str(), strGuid.length() );

	APPEND_IPARAM_INT( Ado, "@ItemAmount", sEXCHANGE.m_nNum );
	APPEND_IPARAM_INT( Ado, "@ItemFromFlag", sEXCHANGE.m_nFromType );
	APPEND_IPARAM_INT( Ado, "@ItemFrom", sEXCHANGE.m_nItemFrom );

	APPEND_IPARAM_INT( Ado, "@ItemToFlag", sEXCHANGE.m_nToType );
	APPEND_IPARAM_INT( Ado, "@ItemTo", sEXCHANGE.m_nItemTo );
	APPEND_IPARAM_INT( Ado, "@ExchangeFlag", sEXCHANGE.m_nExchangeFlag );
	APPEND_IPARAM_INT( Ado, "@Damage", static_cast< byte >( sEXCHANGE.m_nDamage ) );
	APPEND_IPARAM_INT( Ado, "@Defense", static_cast< byte >( sEXCHANGE.m_nDefense ) );

	APPEND_IPARAM_INT( Ado, "@Fire", static_cast< byte >( sEXCHANGE.m_nFire ) );
	APPEND_IPARAM_INT( Ado, "@Ice", static_cast< byte >( sEXCHANGE.m_nIce ) );
	APPEND_IPARAM_INT( Ado, "@Poison", static_cast< byte >( sEXCHANGE.m_nPoison ) );
	APPEND_IPARAM_INT( Ado, "@Electric", static_cast< byte >( sEXCHANGE.m_nElectric ) );
	APPEND_IPARAM_INT( Ado, "@Spirit", static_cast< byte >( sEXCHANGE.m_nSpirit ) );

	APPEND_IPARAM_INT( Ado, "@CostumeMID", sEXCHANGE.m_nCOSTUME_MID );
	APPEND_IPARAM_INT( Ado, "@CostumeSID", sEXCHANGE.m_nCOSTUME_SID );

	APPEND_IPARAM_MONEY( Ado, "@TradePrice", 0 );

	std::string DateStr = sc::time::DateTimeFormat( sEXCHANGE.m_LogDate, true, true );
	APPEND_IPARAM_VARCHAR( Ado, "@ExchangeDate", DateStr.c_str(), DateStr.length() );

	APPEND_IPARAM_INT( Ado, "@nBaseMainMapID", sEXCHANGE.m_nBaseMainMapID );
	APPEND_IPARAM_INT( Ado, "@nBaseSubMapID", sEXCHANGE.m_nBaseSubMapID );
	APPEND_IPARAM_INT( Ado, "@nGaeaMainMapID", sEXCHANGE.m_nGaeaMainMapID );
	APPEND_IPARAM_INT( Ado, "@nGaeaSubMapID", sEXCHANGE.m_nGaeaSubMapID );

	APPEND_IPARAM_INT( Ado, "@nOriNIDMain", sEXCHANGE.m_nOriNIDMain );
	APPEND_IPARAM_INT( Ado, "@nOriNIDSub", sEXCHANGE.m_nOriNIDSub );

	APPEND_IPARAM_INT( Ado, "@nExchangeFlagDetail", sEXCHANGE.m_nExchangeFlagDetail );

	return ExecuteStoredProcedure( "dbo.ItemLogInsert", Ado );

}

// 통계 및 로그 수정
int AdoManager::LogMoneyExchange( const LOG_MONEY_EXCHANGE& _sLOG )
{
	// 값 복사 줄이기 위해서 각 행위마다 함수 따로 분리한다.
	sc::db::AdoExt Ado( m_LogDBConnString );

	APPEND_IPARAM_INT( Ado, "@NIDMain", 0 );
	APPEND_IPARAM_INT( Ado, "@NIDSub", 0 );
	APPEND_IPARAM_INT( Ado, "@SGNum", _sLOG.m_nSGNum );
	APPEND_IPARAM_INT( Ado, "@SvrNum", _sLOG.m_nSvrNum );
	APPEND_IPARAM_INT( Ado, "@FldNum", sc::db::AdjustTinyInt( _sLOG.m_nFldNum ) );

	APPEND_IPARAM_INT( Ado, "@MakeType", 0 );

	std::string strGuid = sc::string::uuidToString( ( UUID )_sLOG.m_guid );
	sc::string::upperString( strGuid );
	APPEND_IPARAM_VARCHAR( Ado, "@ItemUUID", strGuid.c_str(), strGuid.length() );

	APPEND_IPARAM_INT( Ado, "@ItemAmount", 0 );
	APPEND_IPARAM_INT( Ado, "@ItemFromFlag", _sLOG.m_FromType );
	APPEND_IPARAM_INT( Ado, "@ItemFrom", _sLOG.m_nItemFrom );

	APPEND_IPARAM_INT( Ado, "@ItemToFlag", _sLOG.m_ToType );
	APPEND_IPARAM_INT( Ado, "@ItemTo", _sLOG.m_nItemTo );
	APPEND_IPARAM_INT( Ado, "@ExchangeFlag", _sLOG.m_nExchangeFlag );
	APPEND_IPARAM_INT( Ado, "@Damage", 0 );
	APPEND_IPARAM_INT( Ado, "@Defense", 0 );

	APPEND_IPARAM_INT( Ado, "@Fire", 0 );
	APPEND_IPARAM_INT( Ado, "@Ice", 0 );
	APPEND_IPARAM_INT( Ado, "@Poison", 0 );
	APPEND_IPARAM_INT( Ado, "@Electric", 0 );
	APPEND_IPARAM_INT( Ado, "@Spirit", 0 );

	APPEND_IPARAM_INT( Ado, "@CostumeMID", 0 );
	APPEND_IPARAM_INT( Ado, "@CostumeSID", 0 );

	APPEND_IPARAM_MONEY( Ado, "@TradePrice", sc::db::AdjustMoney( _sLOG.m_lnPrice ) );

	std::string DateStr = sc::time::DateTimeFormat( _sLOG.m_LogDate, true, true );
	APPEND_IPARAM_VARCHAR( Ado, "@ExchangeDate", DateStr.c_str(), DateStr.length() );

	APPEND_IPARAM_INT( Ado, "@nBaseMainMapID", _sLOG.m_nBaseMainMapID );
	APPEND_IPARAM_INT( Ado, "@nBaseSubMapID", _sLOG.m_nBaseSubMapID );
	APPEND_IPARAM_INT( Ado, "@nGaeaMainMapID", _sLOG.m_nGaeaMainMapID );
	APPEND_IPARAM_INT( Ado, "@nGaeaSubMapID", _sLOG.m_nGaeaSubMapID );

	APPEND_IPARAM_INT( Ado, "@nOriNIDMain", _sLOG.m_nOriNIDMain );
	APPEND_IPARAM_INT( Ado, "@nOriNIDSub", _sLOG.m_nOriNIDSub );

	APPEND_IPARAM_INT( Ado, "@nExchangeFlagDetail", _sLOG.m_nExchangeFlagDetail );

	return ExecuteStoredProcedure( "dbo.ItemLogInsert", Ado );
}

int AdoManager::LogPointShopExchange( int nFromType, DWORD ChaDbNum, int nExchangeFlag, WORD ItemMid, WORD ItemSid, int nPoint, int nExchangeFlagDetail )
{
	// 값 복사 줄이기 위해서 각 행위마다 함수 따로 분리한다.
	sc::db::AdoExt Ado( m_LogDBConnString );

	APPEND_IPARAM_INT( Ado, "@NIDMain", ItemMid );
	APPEND_IPARAM_INT( Ado, "@NIDSub", ItemSid );
	APPEND_IPARAM_INT( Ado, "@SGNum", 0 );
	APPEND_IPARAM_INT( Ado, "@SvrNum", 0 );
	APPEND_IPARAM_INT( Ado, "@FldNum", 0 );

	APPEND_IPARAM_INT( Ado, "@MakeType", 0 );

	sc::MGUID guid;
	std::string strGuid = sc::string::uuidToString( guid );
	sc::string::upperString( strGuid );
	APPEND_IPARAM_VARCHAR( Ado, "@ItemUUID", strGuid.c_str(), strGuid.length() );

	APPEND_IPARAM_INT( Ado, "@ItemAmount", 0 );
	APPEND_IPARAM_INT( Ado, "@ItemFromFlag", nFromType );
	APPEND_IPARAM_INT( Ado, "@ItemFrom", ChaDbNum );

	APPEND_IPARAM_INT( Ado, "@ItemToFlag", 0 );
	APPEND_IPARAM_INT( Ado, "@ItemTo", 0 );
	APPEND_IPARAM_INT( Ado, "@ExchangeFlag", nExchangeFlag );
	APPEND_IPARAM_INT( Ado, "@Damage", 0 );
	APPEND_IPARAM_INT( Ado, "@Defense", 0 );

	APPEND_IPARAM_INT( Ado, "@Fire", 0 );
	APPEND_IPARAM_INT( Ado, "@Ice", 0 );
	APPEND_IPARAM_INT( Ado, "@Poison", 0 );
	APPEND_IPARAM_INT( Ado, "@Electric", 0 );
	APPEND_IPARAM_INT( Ado, "@Spirit", 0 );

	APPEND_IPARAM_INT( Ado, "@CostumeMID", 0 );
	APPEND_IPARAM_INT( Ado, "@CostumeSID", 0 );

	APPEND_IPARAM_MONEY( Ado, "@TradePrice", sc::db::AdjustMoney( nPoint ) );

	std::string DateStr = sc::time::DateTimeFormat( sc::time::GetCurrentTime(), true, true );
	APPEND_IPARAM_VARCHAR( Ado, "@ExchangeDate", DateStr.c_str(), DateStr.length() );

	APPEND_IPARAM_INT( Ado, "@nBaseMainMapID", 0 );
	APPEND_IPARAM_INT( Ado, "@nBaseSubMapID", 0 );
	APPEND_IPARAM_INT( Ado, "@nGaeaMainMapID", 0 );
	APPEND_IPARAM_INT( Ado, "@nGaeaSubMapID", 0 );

	APPEND_IPARAM_INT( Ado, "@nOriNIDMain", 0 );
	APPEND_IPARAM_INT( Ado, "@nOriNIDSub", 0 );

	APPEND_IPARAM_INT( Ado, "@nExchangeFlagDetail", nExchangeFlagDetail );

	return ExecuteStoredProcedure( "dbo.ItemLogInsert", Ado );
}


int AdoManager::LogItemConversion(const LOG_ITEM_CONVERSION &_sLOG)
{
	// 값 복사 줄이기 위해서 각 행위마다 함수 따로 분리한다.
	sc::db::AdoExt Ado( m_LogDBConnString );

	APPEND_IPARAM_INT( Ado, "@NIDMain", _sLOG.m_nNIDMain );
	APPEND_IPARAM_INT( Ado, "@NIDSub", _sLOG.m_nNIDSub );
	APPEND_IPARAM_INT( Ado, "@SGNum", _sLOG.m_nSGNum );
	APPEND_IPARAM_INT( Ado, "@SvrNum", _sLOG.m_nSvrNum );
	APPEND_IPARAM_INT( Ado, "@FldNum", sc::db::AdjustTinyInt( _sLOG.m_nFldNum ) );

	APPEND_IPARAM_INT( Ado, "@MakeType", _sLOG.m_nMakeType );

	std::string strGuid = sc::string::uuidToString( ( UUID )_sLOG.m_guid );
	sc::string::upperString( strGuid );
	APPEND_IPARAM_VARCHAR( Ado, "@ItemUUID", strGuid.c_str(), strGuid.length() );

	APPEND_IPARAM_INT( Ado, "@ItemAmount", 0 );
	APPEND_IPARAM_INT( Ado, "@ItemFromFlag", _sLOG.m_nFromType );
	APPEND_IPARAM_INT( Ado, "@ItemFrom", _sLOG.m_nItemFrom );

	APPEND_IPARAM_INT( Ado, "@ItemToFlag", 0 );
	APPEND_IPARAM_INT( Ado, "@ItemTo", 0 );
	APPEND_IPARAM_INT( Ado, "@ExchangeFlag", _sLOG.m_nExchangeFlag );
	APPEND_IPARAM_INT( Ado, "@Damage", _sLOG.m_nDamage );
	APPEND_IPARAM_INT( Ado, "@Defense", _sLOG.m_nDefense );

	APPEND_IPARAM_INT( Ado, "@Fire", _sLOG.m_nFire );
	APPEND_IPARAM_INT( Ado, "@Ice", _sLOG.m_nIce );
	APPEND_IPARAM_INT( Ado, "@Poison", _sLOG.m_nPoison );
	APPEND_IPARAM_INT( Ado, "@Electric", _sLOG.m_nElectric );
	APPEND_IPARAM_INT( Ado, "@Spirit", _sLOG.m_nSpirit );

	APPEND_IPARAM_INT( Ado, "@CostumeMID", _sLOG.m_nCOSTUME_MID );
	APPEND_IPARAM_INT( Ado, "@CostumeSID", _sLOG.m_nCOSTUME_SID );

	APPEND_IPARAM_MONEY( Ado, "@TradePrice", 0 );

	std::string DateStr = sc::time::DateTimeFormat( _sLOG.m_LogDate, true, true );
	APPEND_IPARAM_VARCHAR( Ado, "@ExchangeDate", DateStr.c_str(), DateStr.length() );

	APPEND_IPARAM_INT( Ado, "@nBaseMainMapID", 0 );
	APPEND_IPARAM_INT( Ado, "@nBaseSubMapID", 0 );
	APPEND_IPARAM_INT( Ado, "@nGaeaMainMapID", 0 );
	APPEND_IPARAM_INT( Ado, "@nGaeaSubMapID", 0 );

	APPEND_IPARAM_INT( Ado, "@nOriNIDMain", 0 );
	APPEND_IPARAM_INT( Ado, "@nOriNIDSub", 0 );

	APPEND_IPARAM_INT( Ado, "@nExchangeFlagDetail", _sLOG.m_nExchangeFlagDetail );

	return ExecuteStoredProcedure( "dbo.ItemLogInsert", Ado );
}

int AdoManager::LogAction(const LOG_ACTION& LogData)
{
    sc::db::AdoExt Ado(m_LogDBConnString);
    //APPEND_RPARAM_INT(Ado);

    APPEND_IPARAM_INT     (Ado, "@nChaNum",      LogData.m_nCHARID);
    APPEND_IPARAM_INT     (Ado, "@nType",        static_cast<int> (LogData.m_ActionType));
    APPEND_IPARAM_INT     (Ado, "@nTargetNum",   LogData.m_TargetId);
    APPEND_IPARAM_INT     (Ado, "@nTargetType",  static_cast<int> (LogData.m_TargetType));
    
    LONGLONG Exp = sc::db::AdjustMoney(LogData.m_nEXP);
    APPEND_IPARAM_MONEY   (Ado, "@nExpPoint",    Exp);

    APPEND_IPARAM_INT     (Ado, "@nBrightPoint", LogData.m_nBRIGHT_POINT);
    APPEND_IPARAM_INT     (Ado, "@nLifePoint",   LogData.m_nLIFE_POINT);
    
    LONGLONG ActionMoney = sc::db::AdjustMoney(LogData.m_nMONEY);
    APPEND_IPARAM_MONEY   (Ado, "@nMoney",       ActionMoney);
    
    std::string DateStr = sc::time::DateTimeFormat(LogData.m_LogDate, true, true);
    APPEND_IPARAM_VARCHAR(Ado, "@ActionDate", DateStr.c_str(), DateStr.length());

	APPEND_IPARAM_INT     (Ado, "@nBaseMainMapID", LogData.m_nBaseMainMapID);
	APPEND_IPARAM_INT     (Ado, "@nBaseSubMapID", LogData.m_nBaseSubMapID);
	APPEND_IPARAM_INT     (Ado, "@nGaeaMainMapID", LogData.m_nGaeaMainMapID);
	APPEND_IPARAM_INT     (Ado, "@nGaeaSubMapID", LogData.m_nGaeaSubMapID);

    //return ExecuteStoredProcedure("dbo.sp_LogAction_Insert", Ado);
	// 통계 및 로그 수정
    return ExecuteStoredProcedure("dbo.LogActionInsertNew_20140829", Ado);
}

int AdoManager::LogRandomItem(const LOG_RANDOM_OPTION &_sLOG)
{
    sc::db::AdoExt Ado(m_LogDBConnString);
    //APPEND_RPARAM_INT(Ado);

    APPEND_IPARAM_INT  (Ado, "@NIDMain", _sLOG.m_Mid);
    APPEND_IPARAM_INT  (Ado, "@NIDSub",  _sLOG.m_Sid);

    APPEND_IPARAM_INT  (Ado, "@SGNum",  _sLOG.m_ServerGroup);
    APPEND_IPARAM_INT  (Ado, "@SvrNum", _sLOG.m_ServerNumber);
    APPEND_IPARAM_INT  (Ado, "@FldNum", _sLOG.m_FieldNumber);

    APPEND_IPARAM_INT  (Ado, "@MakeType", _sLOG.m_MakeType);

	std::string strGuid = sc::string::uuidToString( ( UUID )_sLOG.m_guid );
	sc::string::upperString( strGuid );
	APPEND_IPARAM_VARCHAR( Ado, "@ItemUUID", strGuid.c_str(), strGuid.length() );

    APPEND_IPARAM_INT  (Ado, "@RandomType1",  static_cast<int> (_sLOG.m_OptionType1));
    APPEND_IPARAM_INT  (Ado, "@RandomValue1", static_cast<int> (_sLOG.m_OptionValue1));
    APPEND_IPARAM_INT  (Ado, "@RandomStamp1", static_cast<int> (_sLOG.m_OptionStamp1));
    APPEND_IPARAM_INT  (Ado, "@RandomType2",  static_cast<int> (_sLOG.m_OptionType2));
    APPEND_IPARAM_INT  (Ado, "@RandomValue2", static_cast<int> (_sLOG.m_OptionValue2));
    APPEND_IPARAM_INT  (Ado, "@RandomStamp2", static_cast<int> (_sLOG.m_OptionStamp2));
    APPEND_IPARAM_INT  (Ado, "@RandomType3",  static_cast<int> (_sLOG.m_OptionType3));
    APPEND_IPARAM_INT  (Ado, "@RandomValue3", static_cast<int> (_sLOG.m_OptionValue3));
    APPEND_IPARAM_INT  (Ado, "@RandomStamp3", static_cast<int> (_sLOG.m_OptionStamp3));
    APPEND_IPARAM_INT  (Ado, "@RandomType4",  static_cast<int> (_sLOG.m_OptionType4));
    APPEND_IPARAM_INT  (Ado, "@RandomValue4", static_cast<int> (_sLOG.m_OptionValue4));
    APPEND_IPARAM_INT  (Ado, "@RandomStamp4", static_cast<int> (_sLOG.m_OptionStamp4));
	APPEND_IPARAM_INT  (Ado, "@RandomType5",  static_cast<int> (_sLOG.m_OptionType5));
	APPEND_IPARAM_INT  (Ado, "@RandomValue5", static_cast<int> (_sLOG.m_OptionValue5));
	APPEND_IPARAM_INT  (Ado, "@RandomStamp5", static_cast<int> (_sLOG.m_OptionStamp5));

    std::string DateStr = sc::time::DateTimeFormat(_sLOG.m_LogDate, true, true);

    APPEND_IPARAM_VARCHAR(Ado, "@RandomDate", DateStr.c_str(), DateStr.length());

	APPEND_IPARAM_INT( Ado, "@InsertOption", _sLOG.m_InsertOption );

    return ExecuteStoredProcedure("dbo.ItemLogRandomInsert", Ado);
}

int AdoManager::LogAddonItem( const LOG_ADDON_OPTION &_sLOG )
{
	sc::db::AdoExt Ado( m_LogDBConnString );

	APPEND_IPARAM_INT( Ado, "@NIDMain", _sLOG.m_Mid );
	APPEND_IPARAM_INT( Ado, "@NIDSub",  _sLOG.m_Sid );
	APPEND_IPARAM_INT( Ado, "@SGNum",  _sLOG.m_ServerGroup );
	APPEND_IPARAM_INT( Ado, "@SvrNum", _sLOG.m_ServerNumber );
	APPEND_IPARAM_INT( Ado, "@FldNum", _sLOG.m_FieldNumber );
	APPEND_IPARAM_INT( Ado, "@MakeType", _sLOG.m_MakeType );
	std::string strGuid = sc::string::uuidToString( ( UUID ) _sLOG.m_guid );
	sc::string::upperString( strGuid );
	APPEND_IPARAM_VARCHAR( Ado, "@ItemUUID", strGuid.c_str(), strGuid.length() );

	APPEND_IPARAM_SMALL( Ado, "@AddonType1",  static_cast< short >( _sLOG.m_OptionType1 ) );
	APPEND_IPARAM_INT( Ado, "@AddonValue1", _sLOG.m_OptionValue1 );
	APPEND_IPARAM_FLOAT( Ado, "@AddonValueF1", _sLOG.m_OptionValueF1 );
	APPEND_IPARAM_SMALL( Ado, "@AddonType2",  static_cast< short >( _sLOG.m_OptionType2 ) );
	APPEND_IPARAM_INT( Ado, "@AddonValue2", _sLOG.m_OptionValue2 );
	APPEND_IPARAM_FLOAT( Ado, "@AddonValueF2", _sLOG.m_OptionValueF2 );
	APPEND_IPARAM_SMALL( Ado, "@AddonType3",  static_cast< short >( _sLOG.m_OptionType3 ) );
	APPEND_IPARAM_INT( Ado, "@AddonValue3", _sLOG.m_OptionValue3 );
	APPEND_IPARAM_FLOAT( Ado, "@AddonValueF3", _sLOG.m_OptionValueF3 );
	APPEND_IPARAM_SMALL( Ado, "@AddonType4",  static_cast< short >( _sLOG.m_OptionType4 ) );
	APPEND_IPARAM_INT( Ado, "@AddonValue4", _sLOG.m_OptionValue4 );
	APPEND_IPARAM_FLOAT( Ado, "@AddonValueF4", _sLOG.m_OptionValueF4 );
	APPEND_IPARAM_SMALL( Ado, "@AddonType5",  static_cast< short >( _sLOG.m_OptionType5 ) );
	APPEND_IPARAM_INT( Ado, "@AddonValue5", _sLOG.m_OptionValue5 );
	APPEND_IPARAM_FLOAT( Ado, "@AddonValueF5", _sLOG.m_OptionValueF5 );
	APPEND_IPARAM_SMALL( Ado, "@AddonType6",  static_cast< short >( _sLOG.m_OptionType6 ) );
	APPEND_IPARAM_INT( Ado, "@AddonValue6", _sLOG.m_OptionValue6 );
	APPEND_IPARAM_FLOAT( Ado, "@AddonValueF6", _sLOG.m_OptionValueF6 );

	std::string DateStr = sc::time::DateTimeFormat( _sLOG.m_LogDate, true, true );

	APPEND_IPARAM_VARCHAR( Ado, "@AddonDate", DateStr.c_str(), DateStr.length() );

	return ExecuteStoredProcedure( "dbo.ItemLogAddonInsert", Ado );
}

int AdoManager::LogLinkSkillItem( const LOG_LINKSKILL_OPTION &_sLOG )
{
	sc::db::AdoExt Ado( m_LogDBConnString );

	APPEND_IPARAM_INT( Ado, "@NIDMain", _sLOG.m_Mid );
	APPEND_IPARAM_INT( Ado, "@NIDSub",  _sLOG.m_Sid );
	APPEND_IPARAM_INT( Ado, "@SGNum",  _sLOG.m_ServerGroup );
	APPEND_IPARAM_INT( Ado, "@SvrNum", _sLOG.m_ServerNumber );
	APPEND_IPARAM_INT( Ado, "@FldNum", _sLOG.m_FieldNumber );
	APPEND_IPARAM_INT( Ado, "@MakeType", _sLOG.m_MakeType );
	std::string strGuid = sc::string::uuidToString( ( UUID ) _sLOG.m_guid );
	sc::string::upperString( strGuid );
	APPEND_IPARAM_VARCHAR( Ado, "@ItemUUID", strGuid.c_str(), strGuid.length() );

	APPEND_IPARAM_SMALL( Ado, "@LinkSkillMID",  static_cast< short >( _sLOG.m_LinkSkillMid ) );
	APPEND_IPARAM_SMALL( Ado, "@LinkSkillSID", static_cast< short >( _sLOG.m_LinkSkillSid ) );
	APPEND_IPARAM_SMALL( Ado, "@LinkSkillLevel",  static_cast< short >( _sLOG.m_LinkSkillLevel ) );
	APPEND_IPARAM_SMALL( Ado, "@LinkSkillTarget", static_cast< short >( _sLOG.m_LinkSkillTarget ) );
	APPEND_IPARAM_FLOAT( Ado, "@LinkSkillOccurRate",  _sLOG.m_LinkSkillOccurRate );

	std::string DateStr = sc::time::DateTimeFormat( _sLOG.m_LogDate, true, true );

	APPEND_IPARAM_VARCHAR( Ado, "@LinkSkillDate", DateStr.c_str(), DateStr.length() );

	return ExecuteStoredProcedure( "dbo.ItemLogLinkSkillInsert", Ado );
}

int AdoManager::LogBasicStatItem( const LOG_BASICSTAT_OPTION &_sLOG )
{
	sc::db::AdoExt Ado( m_LogDBConnString );

	APPEND_IPARAM_INT( Ado, "@NIDMain", _sLOG.m_Mid );
	APPEND_IPARAM_INT( Ado, "@NIDSub",  _sLOG.m_Sid );
	APPEND_IPARAM_INT( Ado, "@SGNum",  _sLOG.m_ServerGroup );
	APPEND_IPARAM_INT( Ado, "@SvrNum", _sLOG.m_ServerNumber );
	APPEND_IPARAM_INT( Ado, "@FldNum", _sLOG.m_FieldNumber );
	APPEND_IPARAM_INT( Ado, "@MakeType", _sLOG.m_MakeType );
	std::string strGuid = sc::string::uuidToString( ( UUID ) _sLOG.m_guid );
	sc::string::upperString( strGuid );
	APPEND_IPARAM_VARCHAR( Ado, "@ItemUUID", strGuid.c_str(), strGuid.length() );

	APPEND_IPARAM_SMALL( Ado, "@BasicAttackDamageMin",  _sLOG.m_BasicAttackDamageMin );
	APPEND_IPARAM_SMALL( Ado, "@BasicAttackDamageMax",  _sLOG.m_BasicAttackDamageMax );
	APPEND_IPARAM_SMALL( Ado, "@BasicDefence", static_cast< short >( _sLOG.m_BasicDefence ) );

	std::string DateStr = sc::time::DateTimeFormat( _sLOG.m_LogDate, true, true );

	APPEND_IPARAM_VARCHAR( Ado, "@BasicStatDate", DateStr.c_str(), DateStr.length() );

	return ExecuteStoredProcedure( "dbo.ItemLogBasicStatInsert", Ado );
}

int AdoManager::LogItemCostumeStat(const LOG_COSTUME_STATS &_sLOG)
{
	sc::db::AdoExt Ado(m_LogDBConnString);
	//APPEND_RPARAM_INT(Ado);

	APPEND_IPARAM_INT  (Ado, "@ChaDbNum"  , _sLOG.m_iCharDbNum);
	APPEND_IPARAM_INT  (Ado, "@NIDMain", _sLOG.m_Mid);
	APPEND_IPARAM_INT  (Ado, "@NIDSub",  _sLOG.m_Sid);

	APPEND_IPARAM_INT  (Ado, "@SGNum",  _sLOG.m_ServerGroup);
	APPEND_IPARAM_INT  (Ado, "@SvrNum", _sLOG.m_ServerNumber);
	APPEND_IPARAM_INT  (Ado, "@FldNum", _sLOG.m_FieldNumber);

	APPEND_IPARAM_INT  (Ado, "@MakeType", _sLOG.m_MakeType);

	std::string strGuid = sc::string::uuidToString( ( UUID )_sLOG.m_guid );
	sc::string::upperString( strGuid );
	APPEND_IPARAM_VARCHAR( Ado, "@ItemUUID", strGuid.c_str(), strGuid.length() );

	std::string FireDateStr = sc::time::DateTimeFormat(_sLOG.m_tFireData, true, true);

	APPEND_IPARAM_VARCHAR(Ado, "@FireDate", FireDateStr.c_str(), FireDateStr.length());

	APPEND_IPARAM_INT   (Ado, "@StatType1",  static_cast<int> (_sLOG.m_Type1));
	APPEND_IPARAM_FLOAT (Ado, "@StatValue1", _sLOG.m_point1);
	APPEND_IPARAM_INT   (Ado, "@StatType2",  static_cast<int> (_sLOG.m_Type2));
	APPEND_IPARAM_FLOAT (Ado, "@StatValue1", _sLOG.m_point2);
	APPEND_IPARAM_INT   (Ado, "@StatType3",  static_cast<int> (_sLOG.m_Type3));
	APPEND_IPARAM_FLOAT (Ado, "@StatValue1", _sLOG.m_point3);
	APPEND_IPARAM_INT   (Ado, "@StatType4",  static_cast<int> (_sLOG.m_Type4));
	APPEND_IPARAM_FLOAT (Ado, "@StatValue1", _sLOG.m_point4);
	APPEND_IPARAM_INT   (Ado, "@StatType5",  static_cast<int> (_sLOG.m_Type5));
	APPEND_IPARAM_FLOAT (Ado, "@StatValue1", _sLOG.m_point5);
	APPEND_IPARAM_INT   (Ado, "@StatType6",  static_cast<int> (_sLOG.m_Type6));
	APPEND_IPARAM_FLOAT (Ado, "@StatValue1", _sLOG.m_point6);
	APPEND_IPARAM_INT   (Ado, "@StatType7",  static_cast<int> (_sLOG.m_Type7));
	APPEND_IPARAM_FLOAT (Ado, "@StatValue1", _sLOG.m_point7);
	
	std::string DateStr = sc::time::DateTimeFormat(_sLOG.m_LogDate, true, true);

	APPEND_IPARAM_VARCHAR(Ado, "@CsDate", DateStr.c_str(), DateStr.length());

	return ExecuteStoredProcedure("dbo.ItemLogCostumeStatsInsert", Ado);
}

int AdoManager::LogPetAction(const LOG_PET_ACTION &_sLOG)
{    
    sc::db::AdoExt Ado(m_LogDBConnString);
    //APPEND_RPARAM_INT(Ado);
        
    APPEND_IPARAM_INT(Ado, "@PetNum",     _sLOG.m_PetNum);
    APPEND_IPARAM_INT(Ado, "@ItemMID",    _sLOG.m_ItemMID);
    APPEND_IPARAM_INT(Ado, "@ItemSID",    _sLOG.m_ItemSID);
    APPEND_IPARAM_INT(Ado, "@ActionType", static_cast<int> (_sLOG.m_ActionType));
    APPEND_IPARAM_INT(Ado, "@PetFull",    _sLOG.m_PetFull);
    
    std::string DateStr = sc::time::DateTimeFormat(_sLOG.m_LogDate, true, true);

    APPEND_IPARAM_VARCHAR(Ado, "@LogDate", DateStr.c_str(), DateStr.length());

    //APPEND_OPARAM_INT(Ado, "@nReturn");

    //return ExecuteStoredProcedureIntReturn("dbo.sp_LogPetAction_Insert", Ado, "@nReturn");
    return ExecuteStoredProcedure("dbo.LogPetActionInsertNew", Ado);
}

int AdoManager::LogVehicleAction(const LOG_VEHICLE_ACTION &_sLOG)
{
    sc::db::AdoExt Ado(m_LogDBConnString);
    //APPEND_RPARAM_INT(Ado);

    APPEND_IPARAM_INT(Ado, "@VehicleNum",     _sLOG.m_VehicleNum);
    APPEND_IPARAM_INT(Ado, "@ItemMID",        _sLOG.m_ItemMID);
    APPEND_IPARAM_INT(Ado, "@ItemSID",        _sLOG.m_ItemSID);
    APPEND_IPARAM_INT(Ado, "@ActionType",     static_cast<int> (_sLOG.m_ActionType));
    APPEND_IPARAM_INT(Ado, "@VehicleBattery", _sLOG.m_VehicleFull);
    
    std::string DateStr = sc::time::DateTimeFormat(_sLOG.m_LogDate, true, true);

    APPEND_IPARAM_VARCHAR(Ado, "@LogDate", DateStr.c_str(), DateStr.length());
    //APPEND_OPARAM_INT(Ado, "@nReturn");

    //return ExecuteStoredProcedureIntReturn("dbo.sp_LogVehicleAction_Insert", Ado, "@nReturn");
    return ExecuteStoredProcedure("dbo.LogVehicleActionInsertNew", Ado);
}

int AdoManager::LogPointShopInsert(DWORD ChaDbNum, const std::string& LogMessage)
{
    sc::db::AdoExt Ado(m_LogDBConnString);

    char szMessage[256] = {0};
    StringCchCopyA(szMessage, 256, LogMessage.c_str());
    size_t len = strnlen(szMessage, 256);

    APPEND_IPARAM_INT    (Ado, "@ChaNum",     ChaDbNum);
    APPEND_IPARAM_VARCHAR(Ado, "@LogMessage", szMessage, static_cast<long> (len));
    
    return ExecuteStoredProcedure("dbo.sp_LogPointShop_Insert", Ado);
}

int AdoManager::LogPointShopInsert( DWORD ChaDbNum, int LogType, __int64& BeforeRefundable, __int64& BeforeNotRefundable, __int64& AfterRefundable, __int64& AfterNotRefundable, WORD ItemMid, WORD ItemSid )
{
	sc::db::AdoExt Ado(m_LogDBConnString);

	APPEND_IPARAM_INT    (Ado, "@ChaNum", ChaDbNum);
	APPEND_IPARAM_INT	 (Ado, "@LogType", LogType);
	APPEND_IPARAM_BIGINT (Ado, "@BeforeRefundable", BeforeRefundable);
	APPEND_IPARAM_BIGINT (Ado, "@BeforeNotRefundable", BeforeNotRefundable);
	APPEND_IPARAM_BIGINT (Ado, "@AfterRefundable", AfterRefundable);
	APPEND_IPARAM_BIGINT (Ado, "@AfterNotRefundable", AfterNotRefundable);
	APPEND_IPARAM_SMALL  (Ado, "@ItemMid", ItemMid);
	APPEND_IPARAM_SMALL  (Ado, "@ItemSid", ItemSid);

	return ExecuteStoredProcedure("dbo.LogPointShopInsertRenew", Ado);
}

//
//mjeon.CaptureTheField
//
int AdoManager::CTFStatLog(UINT *arrPlayerNum, UINT state)
{
	sc::db::AdoExt ado(m_LogDBConnString);

	APPEND_IPARAM_INT(ado, "@State",		state);
	APPEND_IPARAM_INT(ado, "@PlayerNumSM",	arrPlayerNum[SCHOOL_SM]);
	APPEND_IPARAM_INT(ado, "@PlayerNumHA",	arrPlayerNum[SCHOOL_HA]);
	APPEND_IPARAM_INT(ado, "@PlayerNumBH",	arrPlayerNum[SCHOOL_BH]);

	return ExecuteStoredProcedure("dbo.sp_CTFStatLog", ado);
}


int AdoManager::CTFResultLog(WORD wSchoolAuthed, CTF_REWARD_BUFF *arrReward, UINT nHowMany)
{
	if (nHowMany > 4)
	{
		sc::writeLogError(sc::string::format("AdoManager::CTFResultLog() - Number of reward buffs is bigger than 4.(%1%)", nHowMany));

		return sc::db::DB_ERROR;
	}

	sc::db::AdoExt ado(m_LogDBConnString);

	SNATIVEID	id1(false);
	SNATIVEID	id2(false);
	SNATIVEID	id3(false);
	SNATIVEID	id4(false);

	int			lev1 = 0;
	int			lev2 = 0;
	int			lev3 = 0;
	int			lev4 = 0;

	if (nHowMany > 0)	
	{
		id1 = SNATIVEID(arrReward[0].dwID);
		id2 = SNATIVEID(arrReward[1].dwID);
		id3 = SNATIVEID(arrReward[2].dwID);
		id4 = SNATIVEID(arrReward[3].dwID);

		lev1 = (int)arrReward[0].wLevel;
		lev2 = (int)arrReward[1].wLevel;
		lev3 = (int)arrReward[2].wLevel;
		lev4 = (int)arrReward[3].wLevel;
	}


	APPEND_IPARAM_INT(ado,	"@SchoolAuthed",	wSchoolAuthed);
	APPEND_IPARAM_INT(ado,	"@NumOfRewards",	nHowMany);
	APPEND_IPARAM_SMALL(ado,"@Reward1MID",		id1.Mid());
	APPEND_IPARAM_SMALL(ado,"@Reward1SID",		id1.Sid());
	APPEND_IPARAM_INT(ado,	"@Reward1Lev",		lev1);
	APPEND_IPARAM_SMALL(ado,"@Reward2MID",		id2.Mid());
	APPEND_IPARAM_SMALL(ado,"@Reward2SID",		id2.Sid());
	APPEND_IPARAM_INT(ado,	"@Reward2Lev",		lev2);
	APPEND_IPARAM_SMALL(ado,"@Reward3MID",		id3.Mid());
	APPEND_IPARAM_SMALL(ado,"@Reward3SID",		id3.Sid());
	APPEND_IPARAM_INT(ado,	"@Reward3Lev",		lev3);
	APPEND_IPARAM_SMALL(ado,"@Reward4MID",		id4.Mid());
	APPEND_IPARAM_SMALL(ado,"@Reward4SID",		id4.Sid());
	APPEND_IPARAM_INT(ado,	"@Reward4Lev",		lev4);
	

	return ExecuteStoredProcedure("dbo.sp_CTFResultLog", ado);

	return 0;
}

int AdoManager::CTFPlayLog(UINT nChaNum, WORD wSchool, int nTotalPoint, int nRewardPoint)
{    
    sc::db::AdoExt Ado(m_LogDBConnString);
    Ado.AppendIParamInteger("@ChaNum", static_cast<int> (nChaNum));
    Ado.AppendIParamInteger("@School", static_cast<int> (wSchool));
    Ado.AppendIParamInteger("@TotalPoint", nTotalPoint);
    Ado.AppendIParamInteger("@RewardPoint", nRewardPoint);

    if (Ado.ExecuteStoredProcedure("dbo.sp_CTFPlayLog"))
        return sc::db::DB_OK;
    else
        return sc::db::DB_ERROR;
}

int AdoManager::GetCTFInfo(UINT rowcount ,std::vector<DWORD >& vAuthSchool)
{    
	sc::db::AdoExt Ado(m_LogDBConnString);

	std::string Query(sc::string::format( "dbo.sp_CTFSelect %1%", rowcount));

	if (!Ado.Execute4Cmd(Query))
		return sc::db::DB_ERROR;
	if (Ado.GetEOF())
		return sc::db::DB_OK;

	do
	{   
		DWORD dwAuthSchool;
		Ado.GetCollect("SchoolAuthed", dwAuthSchool);

		//sc::writeLogInfo(sc::string::format("NotifySelect %1% %2%", strGuid, szGuid));
		vAuthSchool.push_back(dwAuthSchool);

	} while (Ado.Next());

	return sc::db::DB_OK;

}

int AdoManager::AddLogPing(int nUserNum, int nRTT, const char* szIP, __time64_t tTime)
{
    if (!szIP)
        return sc::db::DB_ERROR;

    CTime cTime(tTime);
    CString strTime = cTime.Format("%Y-%m-%d %H:%M:%S");
    
    sc::db::AdoExt Ado(m_LogDBConnString);
    Ado.AppendRParamInteger();

    Ado.AppendIParamInteger    ("@nUserNum", nUserNum);
    Ado.AppendIParamInteger    ("@nRTT", nRTT);
    Ado.AppendIParamVarchar("@IPAddress", szIP, strlen(szIP));
    Ado.AppendIParamVarchar("@PingDatetime", strTime.GetString(), strTime.GetLength());

    Ado.AppendOParamInteger("@nReturn");

    return Ado.ExecuteStoredProcedureIntReturn("dbo.sp_InsertPing", "@nReturn");
}

//! 학교vs학교 파티대련결과저장
int AdoManager::LogPartyMatch(int nSGNum, int nSvrNum, WORD wWin, WORD wLost)
{  
    sc::db::AdoExt AdoGame(m_GameDBConnString);
    APPEND_IPARAM_INT(AdoGame, "@nSGNum",  nSGNum);
    APPEND_IPARAM_INT(AdoGame, "@nSvrNum", nSvrNum);
    APPEND_IPARAM_INT(AdoGame, "@nWin",    static_cast<int> (wWin));
    APPEND_IPARAM_INT(AdoGame, "@nLost",   static_cast<int> (wLost));

    if (AdoGame.ExecuteStoredProcedure("dbo.InsertPartyMatch"))
        return sc::db::DB_OK;
    else
        return sc::db::DB_ERROR;
}

//! 선도전 로그
int AdoManager::LogGuidance(const LOG_GUIDANCE &sLog)
{
    sc::db::AdoExt Ado(m_LogDBConnString);
    Ado.AppendRParamInteger();

    Ado.AppendIParamSmall  ("@MapMID",     static_cast<unsigned short> (sLog.wMapMID));
    Ado.AppendIParamSmall  ("@MapSID",     static_cast<unsigned short> (sLog.wMapSID));
    Ado.AppendIParamInteger    ("@ClubID",     static_cast<int>            (sLog.dwClubID));
    Ado.AppendIParamTinyint("@Certify",    static_cast<BYTE>           (sLog.nCertify));
    Ado.AppendIParamInteger    ("@AllianceID", static_cast<int>            (sLog.dwAllianceID));
    Ado.AppendIParamInteger    ("@Alliance1",  static_cast<int>            (sLog.dwAlliance[0]));
    Ado.AppendIParamInteger    ("@Alliance2",  static_cast<int>            (sLog.dwAlliance[1]));
    Ado.AppendIParamInteger    ("@CharID",     static_cast<int>            (sLog.dwCharID));
    Ado.AppendIParamInteger    ("@Class",      static_cast<int>            (sLog.dwClass));
    Ado.AppendIParamSmall  ("@nLevel",     static_cast<unsigned short> (sLog.wLevel));
    Ado.AppendIParamSmall  ("@nKill",      static_cast<unsigned short> (sLog.wKill));
    Ado.AppendIParamSmall  ("@nDeath",     static_cast<unsigned short> (sLog.wDeath));
    Ado.AppendIParamVarchar("@LogDate",    sLog.strTime.GetString(), sLog.strTime.GetLength());

    Ado.AppendOParamInteger("@nReturn");

    return Ado.ExecuteStoredProcedureIntReturn("dbo.sp_InsertLogGuidance", "@nReturn");
}

//! CDM 로그
int AdoManager::LogCDM(const LOG_CDM &sLog)
{
    sc::db::AdoExt Ado(m_LogDBConnString);
    Ado.AppendRParamInteger();

    Ado.AppendIParamSmall  ("@MapMID",    static_cast<unsigned short> (sLog.wMapMID));
    Ado.AppendIParamSmall  ("@MapSID",    static_cast<unsigned short> (sLog.wMapSID));
    Ado.AppendIParamInteger    ("@ClubID",    static_cast<int>            (sLog.dwClubID));
    Ado.AppendIParamInteger    ("@CharID",    static_cast<int>            (sLog.dwCharID));
    Ado.AppendIParamInteger    ("@Class",     static_cast<int>            (sLog.dwClass));
    Ado.AppendIParamSmall  ("@nLevel",    static_cast<unsigned short> (sLog.wLevel));
    Ado.AppendIParamSmall  ("@nKill",     static_cast<unsigned short> (sLog.wKill));
    Ado.AppendIParamSmall  ("@nDeath",    static_cast<unsigned short> (sLog.wDeath));
    Ado.AppendIParamSmall  ("@ClubKill",  static_cast<unsigned short> (sLog.wClubKill));
    Ado.AppendIParamSmall  ("@ClubDeath", static_cast<unsigned short> (sLog.wClubDeath));
    Ado.AppendIParamVarchar("@LogDate", sLog.strTime.GetString(), sLog.strTime.GetLength());
        
    Ado.AppendOParamInteger("@nReturn");

    return Ado.ExecuteStoredProcedureIntReturn("dbo.sp_InsertlogCDM", "@nReturn");
}

//! CDM 로그(최종) 
int AdoManager::LogCDMReuslt(const LOG_CDM_RESULT &sLog)
{
    sc::db::AdoExt Ado(m_LogDBConnString);
    Ado.AppendRParamInteger();
    
    Ado.AppendIParamSmall  ("@MapMID",    static_cast<unsigned short> (sLog.wMapMID));
    Ado.AppendIParamSmall  ("@MapSID",    static_cast<unsigned short> (sLog.wMapSID));
    Ado.AppendIParamSmall  ("@ClubRank",  static_cast<unsigned short> (sLog.wClubRank));
    Ado.AppendIParamInteger    ("@ClubID",    static_cast<int>            (sLog.dwClubID));
    Ado.AppendIParamVarchar("@ClubName",  sLog.strClubName.GetString(), sLog.strClubName.GetLength());
    Ado.AppendIParamInteger    ("@CharID",    static_cast<int>            (sLog.dwCharID));
    Ado.AppendIParamInteger    ("@Class",     static_cast<int>            (sLog.dwClass));
    Ado.AppendIParamSmall  ("@nLevel",    static_cast<unsigned short> (sLog.wLevel));
    Ado.AppendIParamSmall  ("@nKill",     static_cast<unsigned short> (sLog.wKill));
    Ado.AppendIParamSmall  ("@nDeath",    static_cast<unsigned short> (sLog.wDeath));
    Ado.AppendIParamSmall  ("@ClubKill",  static_cast<unsigned short> (sLog.wClubKill));
    Ado.AppendIParamSmall  ("@ClubDeath", static_cast<unsigned short> (sLog.wClubDeath));
    Ado.AppendIParamVarchar("@LogDate",   sLog.strTime.GetString(), sLog.strTime.GetLength());
        
    Ado.AppendOParamInteger("@nReturn");

    return Ado.ExecuteStoredProcedureIntReturn("dbo.sp_InsertResultLogCDM", "@nReturn");
}

//! 게임내에서 진정이나 신고로 신고한 내용을 DB에 기록한다.    
//! \param nSGNum 서버그룹
//! \param ChaNum 신고한 캐릭터번호
//! \param strMsg 신고내용(최대크기는 CHAT_MSG_SIZE)
int AdoManager::LogAppeal(int nSGNum, int nChaNum, CString strMsg)
{
    sc::writeLogError("Do not call this function. LogAppeal");
    return sc::db::DB_ERROR;
}

//! 불법프로그램 사용내역을 기록한다.
//! \param nSGNum 서버그룹
//! \param nSvrNum 서버번호
//! \param nUserNum 유저번호
//! \param nChaNum 캐릭터번호
//! \param nHackProgramNum 해킹프로그램번호
//! \param szComment 콤멘트
int AdoManager::LogHackProgram(
    int nSGNum, 
    int nSvrNum, 
    int nUserNum, 
    int nChaNum, 
    int nHackProgramNum,
    const TCHAR* szComment)
{
    if (!szComment)
        return sc::db::DB_ERROR;

    CString strComment(szComment);
    strComment.Trim(_T(" ")); // 앞뒤 공백제거
    strComment.Replace("'", "''"); // ' -> ''	

	// 에러 확인 용도
	strComment = "comment temp message.";

    sc::db::AdoExt Ado(m_LogDBConnString);
    Ado.AppendRParamInteger();

    Ado.AppendIParamInteger    ("@nSGNum", nSGNum);
    Ado.AppendIParamInteger    ("@nSvrNum", nSvrNum);
    Ado.AppendIParamInteger    ("@nUserNum", nUserNum);
    Ado.AppendIParamInteger    ("@nChaNum", nChaNum);
    Ado.AppendIParamInteger    ("@nHackProgramNum", nHackProgramNum);
    Ado.AppendIParamVarchar("@strComment", strComment.GetString(), strComment.GetLength());
        
    Ado.AppendOParamInteger("@nReturn");

    return Ado.ExecuteStoredProcedureIntReturn("dbo.InsertLogHackProgram", "@nReturn");
}

//! SMS 문자메시지를 발송한다.
//! \param nSendChaNum SMS 를 발송하는 캐릭터 번호	
//! \param nReceiveChaNum SMS 를 받을 캐릭터 번호
//! \param szReceivePhone SMS 를 받을 캐릭터의 전화번호
//! \param szSmsMsg SMS 메시지 내용
int AdoManager::LogSMS(
    int nSendChaNum,					
    int nReceiveChaNum,
    const TCHAR* szReceivePhone,
    const TCHAR* szSmsMsg)
{
    if (nSendChaNum < 0 || nReceiveChaNum < 0 || szReceivePhone == NULL || szSmsMsg == NULL)
        return sc::db::DB_ERROR;

    sc::db::AdoExt Ado(m_LogDBConnString);
    Ado.AppendRParamInteger();

    Ado.AppendIParamInteger    ("@nChaFromNum", nSendChaNum);    
    std::string FromPhone("System");
    Ado.AppendIParamVarchar("@szChaFromPhone", FromPhone.c_str(), FromPhone.length());
    Ado.AppendIParamInteger    ("@nChaToNum",      nReceiveChaNum);
    Ado.AppendIParamVarchar("@szChaToPhone",   szReceivePhone, strlen(szReceivePhone));
    Ado.AppendIParamVarchar("@szSmsMessage",   szSmsMsg, strlen(szSmsMsg));
        
    Ado.AppendOParamInteger("@nReturn");

    return Ado.ExecuteStoredProcedureIntReturn("dbo.sp_LogSms_Insert", "@nReturn");
}

//! 출석부 이벤트 
int AdoManager::LogUserAttend(int nUserNum, std::vector<USER_ATTEND_INFO> &vecUserAttend)
{
    return sc::db::DB_OK;

    sc::db::AdoExt Ado(m_ShopDBConnString);

    Ado.AppendIParamInteger("@UserNum", nUserNum);

    if (!Ado.ExecuteStoredProcedure("dbo.AttendLog_GetList"))
        return sc::db::DB_ERROR;

    if (Ado.GetEOF())
        return sc::db::DB_OK;

    do
    {
        _variant_t varDate;
        int nCount = 0;
        int nAttendReward = 0;

        USER_ATTEND_INFO sAttend;

        // Bind data
        Ado.GetCollect("LoginDate", varDate);
        Ado.GetCollect("LogCnt",     nCount);
        //::SQLGetData(spConn->hStmt, 3, SQL_C_LONG, &nAttendReward, 0, &cbAttendReward);

        CTime cTemp(sc::time::GetTime(varDate));

        sAttend.tAttendTime = cTemp.GetTime();
        sAttend.nComboAttend = nCount;
        sAttend.bAttendReward = true; //(nAttendReward == 1) ? true : false;

        vecUserAttend.push_back(sAttend);
    } while (Ado.Next());

    return sc::db::DB_OK;
}

int AdoManager::AddLogServerState(const __time64_t& LogDate, int nSGNum, int nSvrNum, int nUserNum, int nUserMax)	
{
    sc::db::AdoExt Ado(m_LogDBConnString);
    Ado.AppendIParamInteger("@usernum", nUserNum);
    Ado.AppendIParamInteger("@usermax", nUserMax);
    Ado.AppendIParamInteger("@svrnum", nSvrNum);
    Ado.AppendIParamInteger("@sgnum", nSGNum);

    std::string LogTime = sc::time::DateTimeFormat(LogDate, true, true);
    Ado.AppendIParamVarchar("@LogDate", LogTime.c_str(), LogTime.length());

    if (Ado.ExecuteStoredProcedure("dbo.LogServerStateInsertNew"))
        return sc::db::DB_OK;
    else
        return sc::db::DB_ERROR;
}
/*
int AdoManager::LogLottery(CString strLottery, CString strIP, CString strUserUID, int nUserNum)
{
    MIN_ASSERT(0 && "아직 만들어지지 않았습니다.");
    return sc::db::DB_ERROR;
}
*/
/*
int AdoManager::LogPungPungWrite(CString strIP, CString strUserUID, int nUserNum, int nPrize)
{
    MIN_ASSERT(0 && "아직 만들어지지 않았습니다.");
    return sc::db::DB_ERROR;
}
*/
/*
int AdoManager::LogPungPungCheck(CString strIP, int nUserNum)
{
    MIN_ASSERT(0 && "아직 만들어지지 않았습니다.");
    return sc::db::DB_ERROR;
}
*/

int AdoManager::LogItemListInsertUpdate(int ItemMid, int ItemSid, const std::string& ItemName)
{
    std::string ItemNameTemp(ItemName);
    sc::string::replaceForDb(ItemNameTemp);
    char szItemName[100] = {0};
    size_t NameLen = ItemNameTemp.length();
    StringCchCopy(szItemName, 100, ItemNameTemp.c_str());

    sc::db::AdoExt Ado(m_LogDBConnString);
    APPEND_IPARAM_INT    (Ado, "@ItemMain", ItemMid);
    APPEND_IPARAM_INT    (Ado, "@ItemSub",  ItemSid);
    APPEND_IPARAM_VARCHAR(Ado, "@ItemName", szItemName, static_cast<long> (NameLen));

    return ExecuteStoredProcedure("dbo.ItemListInsertUpdate", Ado);
}

int AdoManager::LogQuestListInsertUpdate( int Mid, int Sid, const std::string& Name )
{
	std::string NameTemp( Name );
	sc::string::replaceForDb( NameTemp );
	char szName[100] = { 0, };
	size_t NameLen = NameTemp.length();
	StringCchCopy( szName, 100, NameTemp.c_str() );

	sc::db::AdoExt Ado( m_LogDBConnString );
	APPEND_IPARAM_INT    ( Ado, "@Main", Mid );
	APPEND_IPARAM_INT    ( Ado, "@Sub",  Sid );
	APPEND_IPARAM_VARCHAR( Ado, "@Name", szName, static_cast< long >( NameLen ) );

	return ExecuteStoredProcedure( "dbo.QuestListInsertUpdate", Ado );
}

int AdoManager::LogSkillListInsertUpdate( int Mid, int Sid, const std::string& Name, int Grade )
{
	std::string NameTemp( Name );
	sc::string::replaceForDb( NameTemp );
	char szName[100] = { 0, };
	size_t NameLen = NameTemp.length();
	StringCchCopy( szName, 100, NameTemp.c_str() );

	sc::db::AdoExt Ado( m_LogDBConnString );
	APPEND_IPARAM_INT    ( Ado, "@Main", Mid );
	APPEND_IPARAM_INT    ( Ado, "@Sub", Sid );
	APPEND_IPARAM_VARCHAR( Ado, "@Name", szName, static_cast< long >( NameLen ) );
	APPEND_IPARAM_INT    ( Ado, "@Grade", Grade );

	return ExecuteStoredProcedure( "dbo.SkillListInsertUpdate", Ado );
}

int AdoManager::LogGmCommandInsert( int ChaNum, const std::string& Command, const std::string& Param )
{
	std::string CommandTemp( Command );
	sc::string::replaceForDb( CommandTemp );
	char szCommand[30] = { 0, };
	size_t CommandLen = CommandTemp.length();
	StringCchCopy( szCommand, 30, CommandTemp.c_str() );

	std::string ParamTemp( Param );
	sc::string::replaceForDb( ParamTemp );
	char szParam[48] = { 0, };
	size_t ParamLen = ParamTemp.length();
	StringCchCopy( szParam, 48, ParamTemp.c_str() );

	sc::db::AdoExt Ado( m_LogDBConnString );
	APPEND_IPARAM_INT    ( Ado, "@ChaNum", ChaNum );
	APPEND_IPARAM_VARCHAR( Ado, "@Command", szCommand, static_cast< long >( CommandLen ) );
	APPEND_IPARAM_VARCHAR( Ado, "@Param", szParam, static_cast< long >( ParamLen ) );

	return ExecuteStoredProcedure( "dbo.sp_LogGmCommandInsert", Ado );
}

int AdoManager::LogGMChat( int nChatType, int nGMType, const std::string& SendName, const std::string& RecvName, const std::string& ChatMsg )
{
	std::string SendNameTemp( SendName );
	sc::string::replaceForDb( SendNameTemp );
	char szSendName[50] = { 0, };
	size_t SendNameLen = SendNameTemp.length();
	StringCchCopy( szSendName, 50, SendNameTemp.c_str() );

	std::string RecvNameTemp( RecvName );
	sc::string::replaceForDb( RecvNameTemp );
	char szRecvName[50] = { 0, };
	size_t RecvNameLen = RecvNameTemp.length();
	StringCchCopy( szRecvName, 50, RecvNameTemp.c_str() );

	std::string ChatMsgTemp( ChatMsg );
	sc::string::replaceForDb( ChatMsgTemp );
	char szChatMsg[512] = { 0, };
	size_t ChatMsgLen = ChatMsgTemp.length() >= 500 ? 500 : ChatMsgTemp.length();
	StringCchCopy( szChatMsg, 512, ChatMsgTemp.c_str() );

	sc::db::AdoExt Ado( m_LogDBConnString );
	APPEND_IPARAM_INT    ( Ado, "@nChatType", nChatType );
	APPEND_IPARAM_INT    ( Ado, "@nGMType", nGMType );
	APPEND_IPARAM_VARCHAR( Ado, "@strSendName", szSendName, static_cast< long >( SendNameLen ) );
	APPEND_IPARAM_VARCHAR( Ado, "@strRecvName", szRecvName, static_cast< long >( RecvNameLen ) );
	APPEND_IPARAM_VARCHAR( Ado, "@strChatMsg", szChatMsg, static_cast< long >( ChatMsgLen ) );

	return ExecuteStoredProcedure( "dbo.sp_LogGMChat", Ado );
}


int AdoManager::ExecuteSqlLogDb(const std::string& SqlStr)
{
    sc::db::AdoExt Ado(m_LogDBConnString);
    return Ado.Execute(SqlStr);
}

int AdoManager::LogMiniGameOddEvenInsert(
    DWORD UserDbNum,
    DWORD CharDbNum,        
    BYTE  CurrentRound,
    BYTE  WinLost,
    __int64 BattingMoney,
    __int64 DividendMoney,
    __int64 ChaMoney)
{
    sc::db::AdoExt Ado(m_LogDBConnString);
    APPEND_IPARAM_INT    (Ado, "@UserNum",  static_cast<int> (UserDbNum));
    APPEND_IPARAM_INT    (Ado, "@ChaNum",   static_cast<int> (CharDbNum));
    APPEND_IPARAM_TINYINT(Ado, "@Ordinal",  CurrentRound);
    APPEND_IPARAM_TINYINT(Ado, "@Win",      WinLost);
    APPEND_IPARAM_BIGINT (Ado, "@Actual",   BattingMoney);
    APPEND_IPARAM_BIGINT (Ado, "@Dividen",  DividendMoney);
    
    ChaMoney = sc::db::AdjustMoney(ChaMoney);
    APPEND_IPARAM_BIGINT (Ado, "@ChaMoney", ChaMoney);

    return ExecuteStoredProcedure("dbo.MiniGameOddEvenInsert", Ado);
}

LONGLONG AdoManager::GetItemMaxNum(int nSGNum, int nSvrNum,	int nFldNum, std::vector<VIEWLOGITEMEXCHANGEMAX> &v)
{
    sc::db::AdoExt AdoLog(m_LogDBConnString);

    std::string Query(
        sc::string::format(
            "SELECT MaxNum, NIDMain, NIDSub, MakeType FROM dbo.LogItemMax WITH (NOLOCK) WHERE SGNum=%1% AND SvrNum=%2% AND FldNum=%3%",
            nSGNum, nSvrNum, nFldNum));

    if (!AdoLog.Execute4Cmd(Query))
        return sc::db::DB_ERROR;

    if (AdoLog.GetEOF())
        return sc::db::DB_OK;

    do
    {
        LONGLONG llMaxNum = 0;
        int nNIDMain = 0;
        int nNIDSub  = 0;
        int nMakeType= 0;

        AdoLog.GetCollect("MaxNum",   llMaxNum);
        AdoLog.GetCollect("NIDMain",  nNIDMain);
        AdoLog.GetCollect("NIDSub",   nNIDSub);
        AdoLog.GetCollect("MakeType", nMakeType);
        
        VIEWLOGITEMEXCHANGEMAX sTemp;

        sTemp.llMaxNum  = llMaxNum;
        sTemp.nMakeType = nMakeType;
        sTemp.wItemMain = static_cast<WORD> (nNIDMain);
        sTemp.wItemSub  = static_cast<WORD> (nNIDSub);

        v.push_back(sTemp);
    } while (AdoLog.Next());
    return sc::db::DB_OK;
}

int AdoManager::MoneyNotifyInsert( DWORD dwChaDbNum, __time64_t tDate, LONGLONG llOldMoney, LONGLONG llNewMoney, int nCause, int nChannel, int nField )
{
	sc::db::AdoExt Ado( m_LogDBConnString );
	std::string strDate = sc::time::DateTimeFormat( tDate, true, true );

	APPEND_IPARAM_INT	 ( Ado, "@ChaNum", dwChaDbNum );
	APPEND_IPARAM_VARCHAR( Ado, "@Date", strDate.c_str(), static_cast< long > ( strDate.length() ) );	
	APPEND_IPARAM_BIGINT ( Ado, "@OldMoney", llOldMoney );
	APPEND_IPARAM_BIGINT ( Ado, "@NewMoney", llNewMoney );
	APPEND_IPARAM_INT	 ( Ado, "@Cause", nCause );
	APPEND_IPARAM_INT	 ( Ado, "@Channel", nChannel );
	APPEND_IPARAM_INT	 ( Ado, "@FieldID", nField );
	APPEND_OPARAM_INT    ( Ado, "@Return");

	return ExecuteStoredProcedureIntReturn( "dbo.sp_MoneyNotifyInsert", Ado, "@Return" );
}



int AdoManager::ConsignmentSaleLogInsert( sc::MGUID& Guid, DWORD dwChaDbNum, WORD wType, WORD wState, LONGLONG llPrice, LONGLONG llFee, WORD wItemMid, WORD wItemSid, LONGLONG llItemMakeNum, int nItemAmount, WORD TradeCount, WORD wRemainingTime )
{

	std::string strGuid = sc::string::uuidToString(Guid);
	sc::string::upperString(strGuid);

	sc::db::AdoExt Ado(m_LogDBConnString);
	APPEND_RPARAM_INT(Ado);
	APPEND_IPARAM_VARCHAR(Ado, "@ItemGuid", strGuid.c_str(), static_cast<long> (strGuid.length()));
	APPEND_IPARAM_INT    (Ado, "@ChaNum"        , dwChaDbNum);
	APPEND_IPARAM_TINYINT(Ado, "@Type"			, wType);
	APPEND_IPARAM_TINYINT(Ado, "@State"			, wState);
	APPEND_IPARAM_BIGINT (Ado, "@Price"			, llPrice);
	APPEND_IPARAM_BIGINT (Ado, "@GoldFee"       , llFee);
	APPEND_IPARAM_SMALL  (Ado, "@ItemMid"       , wItemMid);
	APPEND_IPARAM_SMALL  (Ado, "@ItemSid"       , wItemSid);
	APPEND_IPARAM_BIGINT (Ado, "@ItemMakeNum"   , llItemMakeNum);
	APPEND_IPARAM_SMALL  (Ado, "@ItemAmount"    , nItemAmount);
	APPEND_IPARAM_TINYINT(Ado, "@TradeCount"    , TradeCount);
	APPEND_IPARAM_SMALL  (Ado, "@RemainingTime" , wRemainingTime);

	APPEND_OPARAM_BIGINT (Ado, "@nReturn");

	return ExecuteStoredProcedure("dbo.LogCongignmentSaleInsert", Ado);
}

int AdoManager::MacroLogInsert(DWORD dwChaDbNum, WORD  wState)
{
	sc::db::AdoExt Ado(m_LogDBConnString);
	APPEND_IPARAM_INT    (Ado, "@ChaNum"        , dwChaDbNum);
	APPEND_IPARAM_TINYINT(Ado, "@State"			, wState);
	APPEND_OPARAM_BIGINT (Ado, "@nReturn");
	return ExecuteStoredProcedure("dbo.LogMacroInsert", Ado);
}

int AdoManager::ItemPeriodExLogInsert(DWORD  chaNum
								  , WORD itemMid
								  , WORD itemSid	
								  , WORD remainNum
								  , __time64_t previousPeriod
								  , __time64_t currentPeriod)
{

	std::string strPrevTime = sc::time::DateTimeFormat( previousPeriod, true, true )
		, strCurrTime = sc::time::DateTimeFormat( currentPeriod, true, true );

	sc::db::AdoExt Ado(m_LogDBConnString);
	APPEND_IPARAM_INT(Ado, "@ChaNum"        , chaNum);
	APPEND_IPARAM_INT(Ado, "@ItemMid"		, itemMid);
	APPEND_IPARAM_INT(Ado, "@ItemSid"		, itemSid);
	APPEND_IPARAM_TINYINT(Ado, "@RemainNum"		, remainNum);
	APPEND_IPARAM_VARCHAR(Ado, "@PreviousPeriod", strPrevTime.c_str(), static_cast< long > ( strPrevTime.length() ));
	APPEND_IPARAM_VARCHAR(Ado, "@CurrentPeriod"	, strCurrTime.c_str(), static_cast< long > ( strCurrTime.length() ));

	APPEND_OPARAM_BIGINT (Ado, "@nReturn");
	return ExecuteStoredProcedure("dbo.LogItemPeriodExInsert", Ado);
}

/*
 * redmine : #1161 Randombox 확률제어 시스템 처리
 * created : sckim, 2015.12.17, Randombox 확률제어 항목 완료로그 저장
 * modified : 
 */
int AdoManager::LogRandomboxChanceComplete(const SNATIVEID& BoxId,  /* RandomBox ID */
                                           const SNATIVEID& ItemId, /* 당첨 Item ID */
                                           INT32 ReqCount,          /* 목표 오픈 수량 */
                                           DWORD ChaNum,            /* 목표 캐릭터 번호 */
                                           DWORD GMNum,             /* 등록 GM Number */
                                           INT32 OpenCount,         /* 실제 오픈 수량 */
                                           DWORD Winner             /* 당첨자 */
                                          )
{
    sc::db::AdoExt Ado(m_LogDBConnString);

    APPEND_IPARAM_INT     (Ado, "@nBoxMID",      BoxId.wMainID);
    APPEND_IPARAM_INT     (Ado, "@nBoxSID",      BoxId.wSubID);
    APPEND_IPARAM_INT     (Ado, "@nItemMID",     ItemId.wMainID);
    APPEND_IPARAM_INT     (Ado, "@nItemSID",     ItemId.wSubID);
    APPEND_IPARAM_INT     (Ado, "@nReqCount",    ReqCount);
    APPEND_IPARAM_INT     (Ado, "@nChaNum",      static_cast<int>(ChaNum));
    APPEND_IPARAM_INT     (Ado, "@nGMNum",       static_cast<int>(GMNum));
    APPEND_IPARAM_INT     (Ado, "@nOpenCount",   OpenCount);
    APPEND_IPARAM_INT     (Ado, "@nWinChaNum",   static_cast<int>(Winner));

    return ExecuteStoredProcedure("dbo.usp_LogRandomboxChanceComplete", Ado);
}


int AdoManager::MoneyLogInsert ( const int _nMainType, const int _nSubType, const LONGLONG _llMoney )
{
	sc::db::AdoExt Ado( m_LogDBConnString );

	APPEND_IPARAM_INT( Ado, "@MainType", _nMainType );
	APPEND_IPARAM_INT( Ado, "@SubType", _nSubType );
	APPEND_IPARAM_BIGINT( Ado, "@LogMoney", _llMoney );

	return ExecuteStoredProcedure( "dbo.sp_InsertLogMoney", Ado );
}


} // namespace db
