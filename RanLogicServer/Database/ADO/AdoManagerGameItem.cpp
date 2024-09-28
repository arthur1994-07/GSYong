#include "pch.h"

#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/Util/DateTime.h"
#include "../../SigmaCore/String/StringUtil.h"
#include "../../SigmaCore/Math/SeqUniqueGuid.h"

#include "../../RanLogic/Item/GLItem.h"
#include "../../RanLogic/Pet/GLPet.h"

#include "../../Server/NetServer.h"

#include "./AdoManager.h"

#include "../../SigmaCore/DebugInclude.h"

namespace db
{

int AdoManager::ItemInsert( DWORD ChaDbNum, INVEN_TYPE InvenType, SINVENITEM_SAVE* pItem, int CallType )
{
	if ( pItem == NULL )
		return sc::db::DB_ERROR;

	/*
	std::string strGUID = sc::string::uuidToString( pItem->sItemCustom.guid );
	strGUID = "{" + strGUID + "}";
	...
	APPEND_IPARAM_GUID( Ado, "@ItemUUID", strGUID.c_str() );
	*/

	std::string strGuid = sc::string::uuidToString( pItem->sItemCustom.guid );
	sc::string::upperString( strGuid );

	sc::db::AdoExt Ado( m_GameDBConnString );
	APPEND_RPARAM_INT( Ado );
	APPEND_IPARAM_VARCHAR( Ado, "@ItemUUID", strGuid.c_str(), strGuid.length() );
	APPEND_IPARAM_INT( Ado, "@ChaNum", ChaDbNum );
	APPEND_IPARAM_TINYINT( Ado, "@InvenType", InvenType );
	APPEND_IPARAM_SMALL( Ado, "@ItemMID", pItem->sItemCustom.GetRealNativeID().wMainID );
	APPEND_IPARAM_SMALL( Ado, "@ItemSID", pItem->sItemCustom.GetRealNativeID().wSubID );
	APPEND_IPARAM_TINYINT( Ado, "@ItemMakeType", pItem->sItemCustom.cGenType );
	APPEND_IPARAM_SMALL( Ado, "@ItemCostumeMID", pItem->sItemCustom.nidDISGUISE.wMainID );
	APPEND_IPARAM_SMALL( Ado, "@ItemCostumeSID", pItem->sItemCustom.nidDISGUISE.wSubID );
	APPEND_IPARAM_TINYINT( Ado, "@ItemPosX", pItem->wPosX );
	APPEND_IPARAM_TINYINT( Ado, "@ItemPosY", pItem->wPosY );
	APPEND_IPARAM_SMALL( Ado, "@ItemRemain", pItem->sItemCustom.wTurnNum );

	std::string ItemCreateDate = sc::time::DateTimeFormat( pItem->sItemCustom.tBORNTIME, true, true );
	APPEND_IPARAM_VARCHAR( Ado, "@ItemCreateDate", ItemCreateDate.c_str(), ItemCreateDate.length() );

	std::string CostumeExpireDate = sc::time::DateTimeFormat( pItem->sItemCustom.tDISGUISE, true, true );
	APPEND_IPARAM_VARCHAR( Ado, "@ItemCostumeExpireDate", CostumeExpireDate.c_str(), CostumeExpireDate.length() );

	APPEND_IPARAM_INT( Ado, "@ItemDurability", pItem->sItemCustom.wDurability );
	APPEND_IPARAM_INT( Ado, "@ItemDurabilityCount", pItem->sItemCustom.dwDurabilityCount );
	APPEND_IPARAM_INT( Ado, "@PointTradeCount", pItem->sItemCustom.wRanPointTradeCount );

	APPEND_IPARAM_TINYINT( Ado, "@ItemAttack", pItem->sItemCustom.cDAMAGE );
	APPEND_IPARAM_TINYINT( Ado, "@ItemDefense", pItem->sItemCustom.cDEFENSE );

	APPEND_IPARAM_TINYINT( Ado, "@ItemRandomType1", pItem->sItemCustom.randomOption.option[0].cOptType );
	APPEND_IPARAM_INT( Ado, "@ItemRandomValue1", pItem->sItemCustom.randomOption.option[0].nOptValue );
	APPEND_IPARAM_TINYINT( Ado, "@ItemRandomType2", pItem->sItemCustom.randomOption.option[1].cOptType );
	APPEND_IPARAM_INT( Ado, "@ItemRandomValue2", pItem->sItemCustom.randomOption.option[1].nOptValue );
	APPEND_IPARAM_TINYINT( Ado, "@ItemRandomType3", pItem->sItemCustom.randomOption.option[2].cOptType );
	APPEND_IPARAM_INT( Ado, "@ItemRandomValue3", pItem->sItemCustom.randomOption.option[2].nOptValue );
	APPEND_IPARAM_TINYINT( Ado, "@ItemRandomType4", pItem->sItemCustom.randomOption.option[3].cOptType );
	APPEND_IPARAM_INT( Ado, "@ItemRandomValue4", pItem->sItemCustom.randomOption.option[3].nOptValue );
	APPEND_IPARAM_TINYINT( Ado, "@ItemRandomType5", pItem->sItemCustom.randomOption.option[3].cOptType );
	APPEND_IPARAM_INT( Ado, "@ItemRandomValue5", pItem->sItemCustom.randomOption.option[3].nOptValue );

	APPEND_IPARAM_TINYINT( Ado, "@ItemResistFire", pItem->sItemCustom.cRESIST_FIRE );
	APPEND_IPARAM_TINYINT( Ado, "@ItemResistIce", pItem->sItemCustom.cRESIST_ICE );
	APPEND_IPARAM_TINYINT( Ado, "@ItemResistElec", pItem->sItemCustom.cRESIST_ELEC );
	APPEND_IPARAM_TINYINT( Ado, "@ItemResistPoison", pItem->sItemCustom.cRESIST_POISON );
	APPEND_IPARAM_TINYINT( Ado, "@ItemResistSpirit", pItem->sItemCustom.cRESIST_SPIRIT );

	APPEND_IPARAM_INT( Ado, "@ItemPetNum", pItem->sItemCustom.m_PetDbNum );
	APPEND_IPARAM_INT( Ado, "@ItemVehicleNum", pItem->sItemCustom.m_VehicleDbNum );

	APPEND_IPARAM_INT( Ado, "@ItemMainColor", pItem->sItemCustom.dwMainColor );
	APPEND_IPARAM_INT( Ado, "@ItemSubColor", pItem->sItemCustom.dwSubColor );

	APPEND_IPARAM_TINYINT( Ado, "@CostumeStatType1", pItem->sItemCustom.costumeUser.Invest[0].cStatType );
	APPEND_IPARAM_SMALL( Ado, "@CostumeInvestPoint1", pItem->sItemCustom.costumeUser.Invest[0].wInvestPt );
	APPEND_IPARAM_TINYINT( Ado, "@CostumeStatType2", pItem->sItemCustom.costumeUser.Invest[1].cStatType );
	APPEND_IPARAM_SMALL( Ado, "@CostumeInvestPoint2", pItem->sItemCustom.costumeUser.Invest[1].wInvestPt );
	APPEND_IPARAM_TINYINT( Ado, "@CostumeStatType3", pItem->sItemCustom.costumeUser.Invest[2].cStatType );
	APPEND_IPARAM_SMALL( Ado, "@CostumeInvestPoint3", pItem->sItemCustom.costumeUser.Invest[2].wInvestPt );
	APPEND_IPARAM_TINYINT( Ado, "@CostumeStatType4", pItem->sItemCustom.costumeUser.Invest[3].cStatType );
	APPEND_IPARAM_SMALL( Ado, "@CostumeInvestPoint4", pItem->sItemCustom.costumeUser.Invest[3].wInvestPt );
	APPEND_IPARAM_TINYINT( Ado, "@CostumeStatType5", pItem->sItemCustom.costumeUser.Invest[4].cStatType );
	APPEND_IPARAM_SMALL( Ado, "@CostumeInvestPoint5", pItem->sItemCustom.costumeUser.Invest[4].wInvestPt );
	APPEND_IPARAM_TINYINT( Ado, "@CostumeStatType6", pItem->sItemCustom.costumeUser.Invest[5].cStatType );
	APPEND_IPARAM_SMALL( Ado, "@CostumeInvestPoint6", pItem->sItemCustom.costumeUser.Invest[5].wInvestPt );
	APPEND_IPARAM_TINYINT( Ado, "@CostumeStatType7", pItem->sItemCustom.costumeUser.Invest[6].cStatType );
	APPEND_IPARAM_SMALL( Ado, "@CostumeInvestPoint7", pItem->sItemCustom.costumeUser.Invest[6].wInvestPt );
	std::string CostumeStatEndDate = sc::time::DateTimeFormat( pItem->sItemCustom.costumeUser.tEndTime, true, true );
	APPEND_IPARAM_VARCHAR( Ado, "@CostumeStatEndDate", CostumeStatEndDate.c_str(), CostumeStatEndDate.length() );

	APPEND_IPARAM_TINYINT( Ado, "@TempGrindValue", pItem->sItemCustom.sTLGrind.cGradeValue );
	std::string TempGrindFireDate = sc::time::DateTimeFormat( pItem->sItemCustom.sTLGrind.tFireTime, true, true );
	APPEND_IPARAM_VARCHAR( Ado, "@TempGrindFireDate", TempGrindFireDate.c_str(), TempGrindFireDate.length() );

	APPEND_IPARAM_INT( Ado, "@ItemVersion", SINVENITEM_SAVE::VERSION );
	APPEND_IPARAM_INT( Ado, "@ItemOption", 0 );

	APPEND_IPARAM_TINYINT( Ado, "@LockBox", pItem->sItemCustom.bVietnamGainItem == true ? 1 : 0 );

	APPEND_IPARAM_INT( Ado, "@BasicAttackDamage", pItem->sItemCustom.GetBasicAttackDamage().GetData() );
	APPEND_IPARAM_SMALL( Ado, "@BasicDefence", pItem->sItemCustom.GetBasicDefence() );

	APPEND_IPARAM_SMALL( Ado, "@ItemLinkSkillMID", pItem->sItemCustom.GetLinkSkillID().wMainID );
	APPEND_IPARAM_SMALL( Ado, "@ItemLinkSkillSID", pItem->sItemCustom.GetLinkSkillID().wSubID );
	APPEND_IPARAM_SMALL( Ado, "@ItemLinkSkillLevel", pItem->sItemCustom.GetLinkSkillLevel() );
	APPEND_IPARAM_SMALL( Ado, "@ItemLinkSkillTarget", static_cast< WORD >( pItem->sItemCustom.GetLinkSkillTarget() ) );
	APPEND_IPARAM_FLOAT( Ado, "@ItemLinkSkillOccurRate", pItem->sItemCustom.GetLinkSkillOccurRate() );
	
	APPEND_IPARAM_SMALL( Ado, "@ItemAddonType1", (WORD)pItem->sItemCustom.sAddonInfo.m_sADDON[0].emTYPE );
	APPEND_IPARAM_INT ( Ado, "@ItemAddonValue1", pItem->sItemCustom.sAddonInfo.m_sADDON[0].nVALUE );
	APPEND_IPARAM_SMALL( Ado, "@ItemAddonType2", (WORD)pItem->sItemCustom.sAddonInfo.m_sADDON[1].emTYPE );
	APPEND_IPARAM_INT ( Ado, "@ItemAddonValue2", pItem->sItemCustom.sAddonInfo.m_sADDON[1].nVALUE );
	APPEND_IPARAM_SMALL( Ado, "@ItemAddonType3", (WORD)pItem->sItemCustom.sAddonInfo.m_sADDON[2].emTYPE );
	APPEND_IPARAM_INT ( Ado, "@ItemAddonValue3", pItem->sItemCustom.sAddonInfo.m_sADDON[2].nVALUE );
	APPEND_IPARAM_SMALL( Ado, "@ItemAddonType4", (WORD)pItem->sItemCustom.sAddonInfo.m_sADDON[3].emTYPE );
	APPEND_IPARAM_INT ( Ado, "@ItemAddonValue4", pItem->sItemCustom.sAddonInfo.m_sADDON[3].nVALUE );
	APPEND_IPARAM_SMALL( Ado, "@ItemAddonType5", (WORD)pItem->sItemCustom.sAddonInfo.m_sADDON[4].emTYPE );
	APPEND_IPARAM_INT ( Ado, "@ItemAddonValue5", pItem->sItemCustom.sAddonInfo.m_sADDON[4].nVALUE );
	APPEND_IPARAM_SMALL( Ado, "@ItemAddonType6", (WORD)pItem->sItemCustom.sAddonInfo.m_sADDON[5].emTYPE );
	APPEND_IPARAM_INT ( Ado, "@ItemAddonValue6", pItem->sItemCustom.sAddonInfo.m_sADDON[5].nVALUE );

	APPEND_OPARAM_INT( Ado, "@Return");

	int Return = Ado.ExecuteStoredProcedureIntReturn( "dbo.sp_ItemCreate", "@Return" );
	if ( sc::db::DB_OK != Return )
	{
		sc::writeLogError( sc::string::format( "ItemInsert error calltype: %1% chanum: %2% guid: %3%", CallType, ChaDbNum, strGuid.c_str() ) );
	}

	return Return;
}

int AdoManager::DeletedItemInsert( DWORD ChaDbNum, INVEN_TYPE InvenType, SINVENITEM_SAVE* pItem, int CallType )
{
	if ( pItem == NULL )
		return sc::db::DB_ERROR;

	/*
	std::string strGUID = sc::string::uuidToString( pItem->sItemCustom.guid );
	strGUID = "{" + strGUID + "}";
	...
	APPEND_IPARAM_GUID( Ado, "@ItemUUID", strGUID.c_str() );
	*/

	std::string strGuid = sc::string::uuidToString( pItem->sItemCustom.guid );
	sc::string::upperString( strGuid );

	sc::db::AdoExt Ado( m_GameDBConnString );
	APPEND_RPARAM_INT( Ado );
	APPEND_IPARAM_VARCHAR( Ado, "@ItemUUID", strGuid.c_str(), strGuid.length() );
	APPEND_IPARAM_INT( Ado, "@ChaNum", ChaDbNum );
	APPEND_IPARAM_TINYINT( Ado, "@InvenType", InvenType );
	APPEND_IPARAM_SMALL( Ado, "@ItemMID", pItem->sItemCustom.GetRealNativeID().wMainID );
	APPEND_IPARAM_SMALL( Ado, "@ItemSID", pItem->sItemCustom.GetRealNativeID().wSubID );
	APPEND_IPARAM_TINYINT( Ado, "@ItemMakeType", pItem->sItemCustom.cGenType );
	APPEND_IPARAM_SMALL( Ado, "@ItemCostumeMID", pItem->sItemCustom.nidDISGUISE.wMainID );
	APPEND_IPARAM_SMALL( Ado, "@ItemCostumeSID", pItem->sItemCustom.nidDISGUISE.wSubID );
	APPEND_IPARAM_TINYINT( Ado, "@ItemPosX", pItem->wPosX );
	APPEND_IPARAM_TINYINT( Ado, "@ItemPosY", pItem->wPosY );
	APPEND_IPARAM_SMALL( Ado, "@ItemRemain", pItem->sItemCustom.wTurnNum );

	std::string ItemCreateDate = sc::time::DateTimeFormat( pItem->sItemCustom.tBORNTIME, true, true );
	APPEND_IPARAM_VARCHAR( Ado, "@ItemCreateDate", ItemCreateDate.c_str(), ItemCreateDate.length() );

	std::string CostumeExpireDate = sc::time::DateTimeFormat( pItem->sItemCustom.tDISGUISE, true, true );
	APPEND_IPARAM_VARCHAR( Ado, "@ItemCostumeExpireDate", CostumeExpireDate.c_str(), CostumeExpireDate.length() );

	APPEND_IPARAM_INT( Ado, "@ItemDurability", pItem->sItemCustom.wDurability );
	APPEND_IPARAM_INT( Ado, "@ItemDurabilityCount", pItem->sItemCustom.dwDurabilityCount );
	APPEND_IPARAM_INT( Ado, "@PointTradeCount", pItem->sItemCustom.wRanPointTradeCount );

	APPEND_IPARAM_TINYINT( Ado, "@ItemAttack", pItem->sItemCustom.cDAMAGE );
	APPEND_IPARAM_TINYINT( Ado, "@ItemDefense", pItem->sItemCustom.cDEFENSE );

	APPEND_IPARAM_TINYINT( Ado, "@ItemRandomType1", pItem->sItemCustom.randomOption.option[0].cOptType );
	APPEND_IPARAM_INT( Ado, "@ItemRandomValue1", pItem->sItemCustom.randomOption.option[0].nOptValue );
	APPEND_IPARAM_TINYINT( Ado, "@ItemRandomType2", pItem->sItemCustom.randomOption.option[1].cOptType );
	APPEND_IPARAM_INT( Ado, "@ItemRandomValue2", pItem->sItemCustom.randomOption.option[1].nOptValue );
	APPEND_IPARAM_TINYINT( Ado, "@ItemRandomType3", pItem->sItemCustom.randomOption.option[2].cOptType );
	APPEND_IPARAM_INT( Ado, "@ItemRandomValue3", pItem->sItemCustom.randomOption.option[2].nOptValue );
	APPEND_IPARAM_TINYINT( Ado, "@ItemRandomType4", pItem->sItemCustom.randomOption.option[3].cOptType );
	APPEND_IPARAM_INT( Ado, "@ItemRandomValue4", pItem->sItemCustom.randomOption.option[3].nOptValue );
	APPEND_IPARAM_TINYINT( Ado, "@ItemRandomType5", pItem->sItemCustom.randomOption.option[3].cOptType );
	APPEND_IPARAM_INT( Ado, "@ItemRandomValue5", pItem->sItemCustom.randomOption.option[3].nOptValue );

	APPEND_IPARAM_TINYINT( Ado, "@ItemResistFire", pItem->sItemCustom.cRESIST_FIRE );
	APPEND_IPARAM_TINYINT( Ado, "@ItemResistIce", pItem->sItemCustom.cRESIST_ICE );
	APPEND_IPARAM_TINYINT( Ado, "@ItemResistElec", pItem->sItemCustom.cRESIST_ELEC );
	APPEND_IPARAM_TINYINT( Ado, "@ItemResistPoison", pItem->sItemCustom.cRESIST_POISON );
	APPEND_IPARAM_TINYINT( Ado, "@ItemResistSpirit", pItem->sItemCustom.cRESIST_SPIRIT );

	APPEND_IPARAM_INT( Ado, "@ItemPetNum", pItem->sItemCustom.m_PetDbNum );
	APPEND_IPARAM_INT( Ado, "@ItemVehicleNum", pItem->sItemCustom.m_VehicleDbNum );

	APPEND_IPARAM_INT( Ado, "@ItemMainColor", pItem->sItemCustom.dwMainColor );
	APPEND_IPARAM_INT( Ado, "@ItemSubColor", pItem->sItemCustom.dwSubColor );

	APPEND_IPARAM_TINYINT( Ado, "@CostumeStatType1", pItem->sItemCustom.costumeUser.Invest[0].cStatType );
	APPEND_IPARAM_SMALL( Ado, "@CostumeInvestPoint1", pItem->sItemCustom.costumeUser.Invest[0].wInvestPt );
	APPEND_IPARAM_TINYINT( Ado, "@CostumeStatType2", pItem->sItemCustom.costumeUser.Invest[1].cStatType );
	APPEND_IPARAM_SMALL( Ado, "@CostumeInvestPoint2", pItem->sItemCustom.costumeUser.Invest[1].wInvestPt );
	APPEND_IPARAM_TINYINT( Ado, "@CostumeStatType3", pItem->sItemCustom.costumeUser.Invest[2].cStatType );
	APPEND_IPARAM_SMALL( Ado, "@CostumeInvestPoint3", pItem->sItemCustom.costumeUser.Invest[2].wInvestPt );
	APPEND_IPARAM_TINYINT( Ado, "@CostumeStatType4", pItem->sItemCustom.costumeUser.Invest[3].cStatType );
	APPEND_IPARAM_SMALL( Ado, "@CostumeInvestPoint4", pItem->sItemCustom.costumeUser.Invest[3].wInvestPt );
	APPEND_IPARAM_TINYINT( Ado, "@CostumeStatType5", pItem->sItemCustom.costumeUser.Invest[4].cStatType );
	APPEND_IPARAM_SMALL( Ado, "@CostumeInvestPoint5", pItem->sItemCustom.costumeUser.Invest[4].wInvestPt );
	APPEND_IPARAM_TINYINT( Ado, "@CostumeStatType6", pItem->sItemCustom.costumeUser.Invest[5].cStatType );
	APPEND_IPARAM_SMALL( Ado, "@CostumeInvestPoint6", pItem->sItemCustom.costumeUser.Invest[5].wInvestPt );
	APPEND_IPARAM_TINYINT( Ado, "@CostumeStatType7", pItem->sItemCustom.costumeUser.Invest[6].cStatType );
	APPEND_IPARAM_SMALL( Ado, "@CostumeInvestPoint7", pItem->sItemCustom.costumeUser.Invest[6].wInvestPt );
	std::string CostumeStatEndDate = sc::time::DateTimeFormat( pItem->sItemCustom.costumeUser.tEndTime, true, true );
	APPEND_IPARAM_VARCHAR( Ado, "@CostumeStatEndDate", CostumeStatEndDate.c_str(), CostumeStatEndDate.length() );

	APPEND_IPARAM_TINYINT( Ado, "@TempGrindValue", pItem->sItemCustom.sTLGrind.cGradeValue );
	std::string TempGrindFireDate = sc::time::DateTimeFormat( pItem->sItemCustom.sTLGrind.tFireTime, true, true );
	APPEND_IPARAM_VARCHAR( Ado, "@TempGrindFireDate", TempGrindFireDate.c_str(), TempGrindFireDate.length() );

	APPEND_IPARAM_INT( Ado, "@ItemVersion", SINVENITEM_SAVE::VERSION );
	APPEND_IPARAM_INT( Ado, "@ItemOption", 0 );

	APPEND_IPARAM_TINYINT( Ado, "@LockBox", pItem->sItemCustom.bVietnamGainItem == true ? 1 : 0 );

	APPEND_IPARAM_INT( Ado, "@BasicAttackDamage", pItem->sItemCustom.GetBasicAttackDamage().GetData() );
	APPEND_IPARAM_SMALL( Ado, "@BasicDefence", pItem->sItemCustom.GetBasicDefence() );

	APPEND_IPARAM_SMALL( Ado, "@ItemLinkSkillMID", pItem->sItemCustom.GetLinkSkillID().wMainID );
	APPEND_IPARAM_SMALL( Ado, "@ItemLinkSkillSID", pItem->sItemCustom.GetLinkSkillID().wSubID );
	APPEND_IPARAM_SMALL( Ado, "@ItemLinkSkillLevel", pItem->sItemCustom.GetLinkSkillLevel() );
	APPEND_IPARAM_SMALL( Ado, "@ItemLinkSkillTarget", static_cast< WORD >( pItem->sItemCustom.GetLinkSkillTarget() ) );
	APPEND_IPARAM_FLOAT( Ado, "@ItemLinkSkillOccurRate", pItem->sItemCustom.GetLinkSkillOccurRate() );
	
	APPEND_IPARAM_SMALL( Ado, "@ItemAddonType1", (WORD)pItem->sItemCustom.sAddonInfo.m_sADDON[0].emTYPE );
	APPEND_IPARAM_INT ( Ado, "@ItemAddonValue1", pItem->sItemCustom.sAddonInfo.m_sADDON[0].nVALUE );
	APPEND_IPARAM_SMALL( Ado, "@ItemAddonType2", (WORD)pItem->sItemCustom.sAddonInfo.m_sADDON[1].emTYPE );
	APPEND_IPARAM_INT ( Ado, "@ItemAddonValue2", pItem->sItemCustom.sAddonInfo.m_sADDON[1].nVALUE );
	APPEND_IPARAM_SMALL( Ado, "@ItemAddonType3", (WORD)pItem->sItemCustom.sAddonInfo.m_sADDON[2].emTYPE );
	APPEND_IPARAM_INT ( Ado, "@ItemAddonValue3", pItem->sItemCustom.sAddonInfo.m_sADDON[2].nVALUE );
	APPEND_IPARAM_SMALL( Ado, "@ItemAddonType4", (WORD)pItem->sItemCustom.sAddonInfo.m_sADDON[3].emTYPE );
	APPEND_IPARAM_INT ( Ado, "@ItemAddonValue4", pItem->sItemCustom.sAddonInfo.m_sADDON[3].nVALUE );
	APPEND_IPARAM_SMALL( Ado, "@ItemAddonType5", (WORD)pItem->sItemCustom.sAddonInfo.m_sADDON[4].emTYPE );
	APPEND_IPARAM_INT ( Ado, "@ItemAddonValue5", pItem->sItemCustom.sAddonInfo.m_sADDON[4].nVALUE );
	APPEND_IPARAM_SMALL( Ado, "@ItemAddonType6", (WORD)pItem->sItemCustom.sAddonInfo.m_sADDON[5].emTYPE );
	APPEND_IPARAM_INT ( Ado, "@ItemAddonValue6", pItem->sItemCustom.sAddonInfo.m_sADDON[5].nVALUE );

	APPEND_OPARAM_INT( Ado, "@Return");

	int Return = Ado.ExecuteStoredProcedureIntReturn( "dbo.sp_DeletedItemCreate", "@Return" );
	if ( sc::db::DB_OK != Return )
	{
		sc::writeLogError( sc::string::format( "DeletedItemInsert error calltype: %1% chanum: %2% guid: %3%", CallType, ChaDbNum, strGuid.c_str() ) );
	}

	return Return;
}

int AdoManager::ItemUpdate( DWORD ChaDbNum, INVEN_TYPE InvenType, SINVENITEM_SAVE* pItem, int CallType )
{
	if ( pItem == NULL )
		return sc::db::DB_ERROR;

	std::string strGuid = sc::string::uuidToString( pItem->sItemCustom.guid );
	sc::string::upperString( strGuid );

	sc::db::AdoExt Ado( m_GameDBConnString );
	APPEND_RPARAM_INT( Ado );

	APPEND_IPARAM_VARCHAR( Ado, "@ItemUUID", strGuid.c_str(), strGuid.length() );
	APPEND_IPARAM_INT( Ado, "@ChaNum", ChaDbNum );
	APPEND_IPARAM_TINYINT( Ado, "@InvenType", InvenType );
	APPEND_IPARAM_SMALL( Ado, "@ItemCostumeMID", pItem->sItemCustom.nidDISGUISE.wMainID );
	APPEND_IPARAM_SMALL( Ado, "@ItemCostumeSID", pItem->sItemCustom.nidDISGUISE.wSubID );
	APPEND_IPARAM_TINYINT( Ado, "@ItemPosX", pItem->wPosX );
	APPEND_IPARAM_TINYINT( Ado, "@ItemPosY", pItem->wPosY );
	APPEND_IPARAM_SMALL( Ado, "@ItemRemain", pItem->sItemCustom.wTurnNum );

	std::string CostumeExpireDate = sc::time::DateTimeFormat( pItem->sItemCustom.tDISGUISE, true, true );
	APPEND_IPARAM_VARCHAR( Ado, "@ItemCostumeExpireDate", CostumeExpireDate.c_str(), CostumeExpireDate.length() );

	std::string strExtension_basicTime = sc::time::DateTimeFormat( pItem->sItemCustom.GetPeriodExBasic_ConvertDBData(), true, true );
	APPEND_IPARAM_VARCHAR( Ado, "@ItemBasicPeriodExTime", strExtension_basicTime.c_str(), strExtension_basicTime.length() );

	std::string strExtension_disuiseTime = sc::time::DateTimeFormat( pItem->sItemCustom.GetPeriodExDisguise_ConvertDBData(), true, true );
	APPEND_IPARAM_VARCHAR( Ado, "@ItemDisguisePeriodExTime", strExtension_disuiseTime.c_str(), strExtension_disuiseTime.length() );

	APPEND_IPARAM_INT( Ado, "@ItemBasicPeriodExCount", pItem->sItemCustom.cPeriodExtensionCount );
	APPEND_IPARAM_INT( Ado, "@ItemDisuisePeriodExCount", pItem->sItemCustom.cCostumePeriodExtensionCount );

	APPEND_IPARAM_INT( Ado, "@ItemDurability", pItem->sItemCustom.wDurability );
	APPEND_IPARAM_INT( Ado, "@ItemDurabilityCount", pItem->sItemCustom.dwDurabilityCount );
	APPEND_IPARAM_INT( Ado, "@PointTradeCount", pItem->sItemCustom.wRanPointTradeCount );

	APPEND_IPARAM_TINYINT( Ado, "@ItemAttack", pItem->sItemCustom.cDAMAGE );
	APPEND_IPARAM_TINYINT( Ado, "@ItemDefense", pItem->sItemCustom.cDEFENSE );

	APPEND_IPARAM_TINYINT( Ado, "@ItemRandomType1", pItem->sItemCustom.randomOption.option[0].cOptType );
	APPEND_IPARAM_INT( Ado, "@ItemRandomValue1", pItem->sItemCustom.randomOption.option[0].nOptValue );
	APPEND_IPARAM_TINYINT( Ado, "@ItemRandomType2", pItem->sItemCustom.randomOption.option[1].cOptType );
	APPEND_IPARAM_INT( Ado, "@ItemRandomValue2", pItem->sItemCustom.randomOption.option[1].nOptValue );
	APPEND_IPARAM_TINYINT( Ado, "@ItemRandomType3", pItem->sItemCustom.randomOption.option[2].cOptType );
	APPEND_IPARAM_INT( Ado, "@ItemRandomValue3", pItem->sItemCustom.randomOption.option[2].nOptValue );
	APPEND_IPARAM_TINYINT( Ado, "@ItemRandomType4", pItem->sItemCustom.randomOption.option[3].cOptType );
	APPEND_IPARAM_INT( Ado, "@ItemRandomValue4", pItem->sItemCustom.randomOption.option[3].nOptValue );
	APPEND_IPARAM_TINYINT( Ado, "@ItemRandomType5", pItem->sItemCustom.randomOption.option[4].cOptType );
	APPEND_IPARAM_INT( Ado, "@ItemRandomValue5", pItem->sItemCustom.randomOption.option[4].nOptValue );

	APPEND_IPARAM_TINYINT( Ado, "@ItemResistFire", pItem->sItemCustom.cRESIST_FIRE );
	APPEND_IPARAM_TINYINT( Ado, "@ItemResistIce", pItem->sItemCustom.cRESIST_ICE );
	APPEND_IPARAM_TINYINT( Ado, "@ItemResistElec", pItem->sItemCustom.cRESIST_ELEC );
	APPEND_IPARAM_TINYINT( Ado, "@ItemResistPoison", pItem->sItemCustom.cRESIST_POISON );
	APPEND_IPARAM_TINYINT( Ado, "@ItemResistSpirit", pItem->sItemCustom.cRESIST_SPIRIT );

	APPEND_IPARAM_INT( Ado, "@ItemPetNum", pItem->sItemCustom.m_PetDbNum );
	APPEND_IPARAM_INT( Ado, "@ItemVehicleNum", pItem->sItemCustom.m_VehicleDbNum );

	APPEND_IPARAM_INT( Ado, "@ItemMainColor", pItem->sItemCustom.dwMainColor );
	APPEND_IPARAM_INT( Ado, "@ItemSubColor", pItem->sItemCustom.dwSubColor );

	APPEND_IPARAM_TINYINT( Ado, "@CostumeStatType1", pItem->sItemCustom.costumeUser.Invest[0].cStatType );
	APPEND_IPARAM_SMALL( Ado, "@CostumeInvestPoint1", pItem->sItemCustom.costumeUser.Invest[0].wInvestPt );
	APPEND_IPARAM_TINYINT( Ado, "@CostumeStatType2", pItem->sItemCustom.costumeUser.Invest[1].cStatType );
	APPEND_IPARAM_SMALL( Ado, "@CostumeInvestPoint2", pItem->sItemCustom.costumeUser.Invest[1].wInvestPt );
	APPEND_IPARAM_TINYINT( Ado, "@CostumeStatType3", pItem->sItemCustom.costumeUser.Invest[2].cStatType );
	APPEND_IPARAM_SMALL( Ado, "@CostumeInvestPoint3", pItem->sItemCustom.costumeUser.Invest[2].wInvestPt );
	APPEND_IPARAM_TINYINT( Ado, "@CostumeStatType4", pItem->sItemCustom.costumeUser.Invest[3].cStatType );
	APPEND_IPARAM_SMALL( Ado, "@CostumeInvestPoint4", pItem->sItemCustom.costumeUser.Invest[3].wInvestPt );
	APPEND_IPARAM_TINYINT( Ado, "@CostumeStatType5", pItem->sItemCustom.costumeUser.Invest[4].cStatType );
	APPEND_IPARAM_SMALL( Ado, "@CostumeInvestPoint5", pItem->sItemCustom.costumeUser.Invest[4].wInvestPt );
	APPEND_IPARAM_TINYINT( Ado, "@CostumeStatType6", pItem->sItemCustom.costumeUser.Invest[5].cStatType );
	APPEND_IPARAM_SMALL( Ado, "@CostumeInvestPoint6", pItem->sItemCustom.costumeUser.Invest[5].wInvestPt );
	APPEND_IPARAM_TINYINT( Ado, "@CostumeStatType7", pItem->sItemCustom.costumeUser.Invest[6].cStatType );
	APPEND_IPARAM_SMALL( Ado, "@CostumeInvestPoint7", pItem->sItemCustom.costumeUser.Invest[6].wInvestPt );
	std::string CostumeStatEndDate = sc::time::DateTimeFormat( pItem->sItemCustom.costumeUser.tEndTime, true, true );
	APPEND_IPARAM_VARCHAR( Ado, "@CostumeStatEndDate", CostumeStatEndDate.c_str(), CostumeStatEndDate.length() );

	APPEND_IPARAM_TINYINT( Ado, "@TempGrindValue", pItem->sItemCustom.sTLGrind.cGradeValue );
	std::string TempGrindFireDate = sc::time::DateTimeFormat( pItem->sItemCustom.sTLGrind.tFireTime, true, true );
	APPEND_IPARAM_VARCHAR( Ado, "@TempGrindFireDate", TempGrindFireDate.c_str(), TempGrindFireDate.length() );

	APPEND_IPARAM_INT( Ado, "@ItemVersion", SINVENITEM_SAVE::VERSION );
	APPEND_IPARAM_INT( Ado, "@ItemOption", 0 );

	APPEND_IPARAM_TINYINT( Ado, "@LockBox", pItem->sItemCustom.bVietnamGainItem == true ? 1 : 0 );

	APPEND_IPARAM_INT( Ado, "@BasicAttackDamage", pItem->sItemCustom.GetBasicAttackDamage().GetData() );
	APPEND_IPARAM_SMALL( Ado, "@BasicDefence", pItem->sItemCustom.GetBasicDefence() );

	APPEND_IPARAM_SMALL( Ado, "@ItemLinkSkillMID", pItem->sItemCustom.GetLinkSkillID().wMainID );
	APPEND_IPARAM_SMALL( Ado, "@ItemLinkSkillSID", pItem->sItemCustom.GetLinkSkillID().wSubID );
	APPEND_IPARAM_SMALL( Ado, "@ItemLinkSkillLevel", pItem->sItemCustom.GetLinkSkillLevel() );
	APPEND_IPARAM_SMALL( Ado, "@ItemLinkSkillTarget", static_cast< WORD >( pItem->sItemCustom.GetLinkSkillTarget() ) );
	APPEND_IPARAM_FLOAT( Ado, "@ItemLinkSkillOccurRate", pItem->sItemCustom.GetLinkSkillOccurRate() );

	APPEND_IPARAM_SMALL( Ado, "@ItemAddonType1", (WORD)pItem->sItemCustom.sAddonInfo.m_sADDON[0].emTYPE );
	APPEND_IPARAM_INT ( Ado, "@ItemAddonValue1", pItem->sItemCustom.sAddonInfo.m_sADDON[0].nVALUE );
	APPEND_IPARAM_SMALL( Ado, "@ItemAddonType2", (WORD)pItem->sItemCustom.sAddonInfo.m_sADDON[1].emTYPE );
	APPEND_IPARAM_INT ( Ado, "@ItemAddonValue2", pItem->sItemCustom.sAddonInfo.m_sADDON[1].nVALUE );
	APPEND_IPARAM_SMALL( Ado, "@ItemAddonType3", (WORD)pItem->sItemCustom.sAddonInfo.m_sADDON[2].emTYPE );
	APPEND_IPARAM_INT ( Ado, "@ItemAddonValue3", pItem->sItemCustom.sAddonInfo.m_sADDON[2].nVALUE );
	APPEND_IPARAM_SMALL( Ado, "@ItemAddonType4", (WORD)pItem->sItemCustom.sAddonInfo.m_sADDON[3].emTYPE );
	APPEND_IPARAM_INT ( Ado, "@ItemAddonValue4", pItem->sItemCustom.sAddonInfo.m_sADDON[3].nVALUE );
	APPEND_IPARAM_SMALL( Ado, "@ItemAddonType5", (WORD)pItem->sItemCustom.sAddonInfo.m_sADDON[4].emTYPE );
	APPEND_IPARAM_INT ( Ado, "@ItemAddonValue5", pItem->sItemCustom.sAddonInfo.m_sADDON[4].nVALUE );
	APPEND_IPARAM_SMALL( Ado, "@ItemAddonType6", (WORD)pItem->sItemCustom.sAddonInfo.m_sADDON[5].emTYPE );
	APPEND_IPARAM_INT ( Ado, "@ItemAddonValue6", pItem->sItemCustom.sAddonInfo.m_sADDON[5].nVALUE );

	APPEND_OPARAM_INT( Ado, "@Return");

	int Return = Ado.ExecuteStoredProcedureIntReturn( "dbo.sp_ItemSave", "@Return" );
	if ( sc::db::DB_OK != Return )
	{
		sc::writeLogError( sc::string::format( "ItemUpdate error calltype: %1% chanum: %2% guid: %3%", CallType, ChaDbNum, strGuid.c_str() ) );
	}

	return Return;
}

int AdoManager::EntireItemUpdate( DWORD ChaDbNum, INVEN_TYPE InvenType, SINVENITEM_SAVE* pItem, int CallType )
{
	if ( pItem == NULL )
		return sc::db::DB_ERROR;

	std::string strGuid = sc::string::uuidToString( pItem->sItemCustom.guid );
	sc::string::upperString( strGuid );

	sc::db::AdoExt Ado( m_GameDBConnString );
	APPEND_RPARAM_INT( Ado );

	APPEND_IPARAM_VARCHAR( Ado, "@ItemUUID", strGuid.c_str(), strGuid.length() );
	APPEND_IPARAM_INT( Ado, "@ChaNum", ChaDbNum );
	APPEND_IPARAM_TINYINT( Ado, "@InvenType", InvenType );
	APPEND_IPARAM_SMALL( Ado, "@ItemMID", pItem->sItemCustom.GetRealNativeID().wMainID );
	APPEND_IPARAM_SMALL( Ado, "@ItemSID", pItem->sItemCustom.GetRealNativeID().wSubID );
	APPEND_IPARAM_SMALL( Ado, "@ItemCostumeMID", pItem->sItemCustom.nidDISGUISE.wMainID );
	APPEND_IPARAM_SMALL( Ado, "@ItemCostumeSID", pItem->sItemCustom.nidDISGUISE.wSubID );
	APPEND_IPARAM_TINYINT( Ado, "@ItemPosX", pItem->wPosX );
	APPEND_IPARAM_TINYINT( Ado, "@ItemPosY", pItem->wPosY );
	APPEND_IPARAM_SMALL( Ado, "@ItemRemain", pItem->sItemCustom.wTurnNum );

	std::string CostumeExpireDate = sc::time::DateTimeFormat( pItem->sItemCustom.tDISGUISE, true, true );
	APPEND_IPARAM_VARCHAR( Ado, "@ItemCostumeExpireDate", CostumeExpireDate.c_str(), CostumeExpireDate.length() );

	APPEND_IPARAM_INT( Ado, "@ItemDurability", pItem->sItemCustom.wDurability );
	APPEND_IPARAM_INT( Ado, "@ItemDurabilityCount", pItem->sItemCustom.dwDurabilityCount );
	APPEND_IPARAM_INT( Ado, "@PointTradeCount", pItem->sItemCustom.wRanPointTradeCount );

	APPEND_IPARAM_TINYINT( Ado, "@ItemAttack", pItem->sItemCustom.cDAMAGE );
	APPEND_IPARAM_TINYINT( Ado, "@ItemDefense", pItem->sItemCustom.cDEFENSE );

	APPEND_IPARAM_TINYINT( Ado, "@ItemRandomType1", pItem->sItemCustom.randomOption.option[0].cOptType );
	APPEND_IPARAM_INT( Ado, "@ItemRandomValue1", pItem->sItemCustom.randomOption.option[0].nOptValue );
	APPEND_IPARAM_TINYINT( Ado, "@ItemRandomType2", pItem->sItemCustom.randomOption.option[1].cOptType );
	APPEND_IPARAM_INT( Ado, "@ItemRandomValue2", pItem->sItemCustom.randomOption.option[1].nOptValue );
	APPEND_IPARAM_TINYINT( Ado, "@ItemRandomType3", pItem->sItemCustom.randomOption.option[2].cOptType );
	APPEND_IPARAM_INT( Ado, "@ItemRandomValue3", pItem->sItemCustom.randomOption.option[2].nOptValue );
	APPEND_IPARAM_TINYINT( Ado, "@ItemRandomType4", pItem->sItemCustom.randomOption.option[3].cOptType );
	APPEND_IPARAM_INT( Ado, "@ItemRandomValue4", pItem->sItemCustom.randomOption.option[3].nOptValue );
	APPEND_IPARAM_TINYINT( Ado, "@ItemRandomType5", pItem->sItemCustom.randomOption.option[4].cOptType );
	APPEND_IPARAM_INT( Ado, "@ItemRandomValue5", pItem->sItemCustom.randomOption.option[4].nOptValue );

	APPEND_IPARAM_TINYINT( Ado, "@ItemResistFire", pItem->sItemCustom.cRESIST_FIRE );
	APPEND_IPARAM_TINYINT( Ado, "@ItemResistIce", pItem->sItemCustom.cRESIST_ICE );
	APPEND_IPARAM_TINYINT( Ado, "@ItemResistElec", pItem->sItemCustom.cRESIST_ELEC );
	APPEND_IPARAM_TINYINT( Ado, "@ItemResistPoison", pItem->sItemCustom.cRESIST_POISON );
	APPEND_IPARAM_TINYINT( Ado, "@ItemResistSpirit", pItem->sItemCustom.cRESIST_SPIRIT );

	APPEND_IPARAM_INT( Ado, "@ItemPetNum", pItem->sItemCustom.m_PetDbNum );
	APPEND_IPARAM_INT( Ado, "@ItemVehicleNum", pItem->sItemCustom.m_VehicleDbNum );

	APPEND_IPARAM_INT( Ado, "@ItemMainColor", pItem->sItemCustom.dwMainColor );
	APPEND_IPARAM_INT( Ado, "@ItemSubColor", pItem->sItemCustom.dwSubColor );

	APPEND_IPARAM_TINYINT( Ado, "@CostumeStatType1", pItem->sItemCustom.costumeUser.Invest[0].cStatType );
	APPEND_IPARAM_SMALL( Ado, "@CostumeInvestPoint1", pItem->sItemCustom.costumeUser.Invest[0].wInvestPt );
	APPEND_IPARAM_TINYINT( Ado, "@CostumeStatType2", pItem->sItemCustom.costumeUser.Invest[1].cStatType );
	APPEND_IPARAM_SMALL( Ado, "@CostumeInvestPoint2", pItem->sItemCustom.costumeUser.Invest[1].wInvestPt );
	APPEND_IPARAM_TINYINT( Ado, "@CostumeStatType3", pItem->sItemCustom.costumeUser.Invest[2].cStatType );
	APPEND_IPARAM_SMALL( Ado, "@CostumeInvestPoint3", pItem->sItemCustom.costumeUser.Invest[2].wInvestPt );
	APPEND_IPARAM_TINYINT( Ado, "@CostumeStatType4", pItem->sItemCustom.costumeUser.Invest[3].cStatType );
	APPEND_IPARAM_SMALL( Ado, "@CostumeInvestPoint4", pItem->sItemCustom.costumeUser.Invest[3].wInvestPt );
	APPEND_IPARAM_TINYINT( Ado, "@CostumeStatType5", pItem->sItemCustom.costumeUser.Invest[4].cStatType );
	APPEND_IPARAM_SMALL( Ado, "@CostumeInvestPoint5", pItem->sItemCustom.costumeUser.Invest[4].wInvestPt );
	APPEND_IPARAM_TINYINT( Ado, "@CostumeStatType6", pItem->sItemCustom.costumeUser.Invest[5].cStatType );
	APPEND_IPARAM_SMALL( Ado, "@CostumeInvestPoint6", pItem->sItemCustom.costumeUser.Invest[5].wInvestPt );
	APPEND_IPARAM_TINYINT( Ado, "@CostumeStatType7", pItem->sItemCustom.costumeUser.Invest[6].cStatType );
	APPEND_IPARAM_SMALL( Ado, "@CostumeInvestPoint7", pItem->sItemCustom.costumeUser.Invest[6].wInvestPt );
	std::string CostumeStatEndDate = sc::time::DateTimeFormat( pItem->sItemCustom.costumeUser.tEndTime, true, true );
	APPEND_IPARAM_VARCHAR( Ado, "@CostumeStatEndDate", CostumeStatEndDate.c_str(), CostumeStatEndDate.length() );

	APPEND_IPARAM_TINYINT( Ado, "@TempGrindValue", pItem->sItemCustom.sTLGrind.cGradeValue );
	std::string TempGrindFireDate = sc::time::DateTimeFormat( pItem->sItemCustom.sTLGrind.tFireTime, true, true );
	APPEND_IPARAM_VARCHAR( Ado, "@TempGrindFireDate", TempGrindFireDate.c_str(), TempGrindFireDate.length() );

	APPEND_IPARAM_INT( Ado, "@ItemVersion", SINVENITEM_SAVE::VERSION );
	APPEND_IPARAM_INT( Ado, "@ItemOption", 0 );

	APPEND_IPARAM_TINYINT( Ado, "@LockBox", pItem->sItemCustom.bVietnamGainItem == true ? 1 : 0 );

	APPEND_IPARAM_INT( Ado, "@BasicAttackDamage", pItem->sItemCustom.GetBasicAttackDamage().GetData() );
	APPEND_IPARAM_SMALL( Ado, "@BasicDefence", pItem->sItemCustom.GetBasicDefence() );

	APPEND_IPARAM_SMALL( Ado, "@ItemLinkSkillMID", pItem->sItemCustom.GetLinkSkillID().wMainID );
	APPEND_IPARAM_SMALL( Ado, "@ItemLinkSkillSID", pItem->sItemCustom.GetLinkSkillID().wSubID );
	APPEND_IPARAM_SMALL( Ado, "@ItemLinkSkillLevel", pItem->sItemCustom.GetLinkSkillLevel() );
	APPEND_IPARAM_SMALL( Ado, "@ItemLinkSkillTarget", static_cast< WORD >( pItem->sItemCustom.GetLinkSkillTarget() ) );
	APPEND_IPARAM_FLOAT( Ado, "@ItemLinkSkillOccurRate", pItem->sItemCustom.GetLinkSkillOccurRate() );

	APPEND_IPARAM_SMALL( Ado, "@ItemAddonType1", (WORD)pItem->sItemCustom.sAddonInfo.m_sADDON[0].emTYPE );
	APPEND_IPARAM_INT ( Ado, "@ItemAddonValue1", pItem->sItemCustom.sAddonInfo.m_sADDON[0].nVALUE );
	APPEND_IPARAM_SMALL( Ado, "@ItemAddonType2", (WORD)pItem->sItemCustom.sAddonInfo.m_sADDON[1].emTYPE );
	APPEND_IPARAM_INT ( Ado, "@ItemAddonValue2", pItem->sItemCustom.sAddonInfo.m_sADDON[1].nVALUE );
	APPEND_IPARAM_SMALL( Ado, "@ItemAddonType3", (WORD)pItem->sItemCustom.sAddonInfo.m_sADDON[2].emTYPE );
	APPEND_IPARAM_INT ( Ado, "@ItemAddonValue3", pItem->sItemCustom.sAddonInfo.m_sADDON[2].nVALUE );
	APPEND_IPARAM_SMALL( Ado, "@ItemAddonType4", (WORD)pItem->sItemCustom.sAddonInfo.m_sADDON[3].emTYPE );
	APPEND_IPARAM_INT ( Ado, "@ItemAddonValue4", pItem->sItemCustom.sAddonInfo.m_sADDON[3].nVALUE );
	APPEND_IPARAM_SMALL( Ado, "@ItemAddonType5", (WORD)pItem->sItemCustom.sAddonInfo.m_sADDON[4].emTYPE );
	APPEND_IPARAM_INT ( Ado, "@ItemAddonValue5", pItem->sItemCustom.sAddonInfo.m_sADDON[4].nVALUE );
	APPEND_IPARAM_SMALL( Ado, "@ItemAddonType6", (WORD)pItem->sItemCustom.sAddonInfo.m_sADDON[5].emTYPE );
	APPEND_IPARAM_INT ( Ado, "@ItemAddonValue6", pItem->sItemCustom.sAddonInfo.m_sADDON[5].nVALUE );

	APPEND_OPARAM_INT( Ado, "@Return");

	int Return = Ado.ExecuteStoredProcedureIntReturn( "dbo.sp_EntireItemSave", "@Return" );
	if ( sc::db::DB_OK != Return )
	{
		sc::writeLogError( sc::string::format( "EntireItemUpdate error calltype: %1% chanum: %2% guid: %3%", CallType, ChaDbNum, strGuid.c_str() ) );
	}

	return Return;
}

int AdoManager::ItemDelete( sc::MGUID& Guid, int State, int CallType )
{
	std::string strGuid = sc::string::uuidToString( Guid );
	sc::string::upperString( strGuid );

	sc::db::AdoExt Ado( m_GameDBConnString );
	APPEND_RPARAM_INT( Ado );
	APPEND_IPARAM_VARCHAR( Ado, "@ItemUUID", strGuid.c_str(), strGuid.length() );
	APPEND_OPARAM_INT( Ado, "@Return");

	int Return = Ado.ExecuteStoredProcedureIntReturn( "dbo.sp_ItemDel", "@Return" );
	if ( sc::db::DB_OK != Return )
	{
		sc::writeLogError( sc::string::format( "ItemDelete error calltype: %1% db state: %2% guid: %3%", CallType, State, strGuid.c_str() ) );
	}

	return Return;
}

int AdoManager::CompletelyItemDelete( sc::MGUID& Guid, int State, int CallType )
{
	std::string strGuid = sc::string::uuidToString( Guid );
	sc::string::upperString( strGuid );

	sc::db::AdoExt Ado( m_GameDBConnString );
	APPEND_RPARAM_INT( Ado );
	APPEND_IPARAM_VARCHAR( Ado, "@ItemUUID", strGuid.c_str(), strGuid.length() );
	APPEND_OPARAM_INT( Ado, "@Return");

	int Return = Ado.ExecuteStoredProcedureIntReturn( "dbo.sp_ItemDelCompletely", "@Return" );
	if ( sc::db::DB_OK != Return )
	{
		sc::writeLogError( sc::string::format( "CompletelyItemDelete error calltype: %1% db state: %2% guid: %3%", CallType, State, strGuid.c_str() ) );
	}

	return Return;
}

int AdoManager::ItemSaleToNpc( sc::MGUID& Guid, int State, int CallType )
{
	std::string strGuid = sc::string::uuidToString( Guid );
	sc::string::upperString( strGuid );

	sc::db::AdoExt Ado( m_GameDBConnString );
	APPEND_RPARAM_INT( Ado );
	APPEND_IPARAM_VARCHAR( Ado, "@ItemUUID", strGuid.c_str(), strGuid.length() );
	APPEND_OPARAM_INT( Ado, "@Return");

	int Return = Ado.ExecuteStoredProcedureIntReturn( "dbo.sp_ItemSaleToNpc", "@Return" );
	if ( sc::db::DB_OK != Return )
	{
		sc::writeLogError( sc::string::format( "ItemSaleToNpc error calltype: %1% db state: %2% guid: %3%", CallType, State, strGuid.c_str() ) );
	}

	return Return;
}

int AdoManager::ItemSelect( DWORD ChaDbNum, INVEN_TYPE InvenType, std::vector< SINVENITEM_SAVE >& vecItems )
{
	sc::db::AdoExt Ado( m_GameDBConnString );
	APPEND_IPARAM_INT( Ado, "@ChaNum", ChaDbNum );
	APPEND_IPARAM_TINYINT( Ado, "@InvenType", InvenType );
	if ( !Ado.Execute4Cmd( "dbo.sp_ItemGetItemList", adCmdStoredProc ) )
		return sc::db::DB_ERROR;

	if ( Ado.GetEOF() )
	{
		return sc::db::DB_OK;
	}

	do
	{
		std::string strGuid;
		SNATIVEID sItemID( false );
		_variant_t CreateDate, CostumeExpireDate, CostumeStatEndDate, TempGrindFireDate, BasicPeriodEx, DisguisePeriodEx;
		SINVENITEM_SAVE sItem;

		DWORD dwBasicAttackDamageData = 0;
		WORD wBasicDefence = 0;

		SNATIVEID sLinkSkillID( false );
		WORD wLinkSkillLevel = 0;
		WORD wLinkSkillTarget = 0;
		float fLinkSkillOccurRate = 0.f;

		Ado.GetCollect( "ItemUUID", strGuid );
		char szGuid[ sc::string::UUID_LENGTH_BRACES ] = { 0 };
		StringCchCopy( szGuid, sc::string::UUID_LENGTH_BRACES, strGuid.c_str() );
		sc::string::stringToUUID( szGuid, sItem.sItemCustom.guid );

		Ado.GetCollect( "ItemMID", sItemID.wMainID );
		Ado.GetCollect( "ItemSID", sItemID.wSubID );

		sItem.sItemCustom.SetNativeID( sItemID );

		Ado.GetCollect( "ItemMakeType", sItem.sItemCustom.cGenType );
		Ado.GetCollect( "ItemCostumeMID", sItem.sItemCustom.nidDISGUISE.wMainID );
		Ado.GetCollect( "ItemCostumeSID", sItem.sItemCustom.nidDISGUISE.wSubID );

		Ado.GetCollect( "ItemPosX", sItem.wPosX );
		Ado.GetCollect( "ItemPosY", sItem.wPosY );

		Ado.GetCollect( "ItemRemain", sItem.sItemCustom.wTurnNum );

		Ado.GetCollect( "ItemCreateDate", CreateDate );
		Ado.GetCollect( "ItemCostumeExpireDate", CostumeExpireDate );

		Ado.GetCollect( "ItemBasicPeriodExTime", BasicPeriodEx );
		Ado.GetCollect( "ItemDisguisePeriodExTime", DisguisePeriodEx );
		Ado.GetCollect( "ItemBasicPeriodExCount", sItem.sItemCustom.cPeriodExtensionCount );
		Ado.GetCollect( "ItemDisuisePeriodExCount", sItem.sItemCustom.cCostumePeriodExtensionCount );

		if ( CreateDate.GetVARIANT().vt == VT_DATE )
			sItem.sItemCustom.tBORNTIME = sc::time::GetTime( CreateDate );

		if ( CostumeExpireDate.GetVARIANT().vt == VT_DATE )
			sItem.sItemCustom.tDISGUISE = sc::time::GetTime( CostumeExpireDate );

		if ( BasicPeriodEx.GetVARIANT().vt == VT_DATE )
			sItem.sItemCustom.SetPeriodExBasic_FromDB(sc::time::GetTime( BasicPeriodEx ));

		if ( DisguisePeriodEx.GetVARIANT().vt == VT_DATE )
			sItem.sItemCustom.SetPeriodExDisguise_FromDB(sc::time::GetTime( DisguisePeriodEx ));

		Ado.GetCollect( "ItemDurability", sItem.sItemCustom.wDurability );
		Ado.GetCollect( "ItemDurabilityCount", sItem.sItemCustom.dwDurabilityCount );
		Ado.GetCollect( "PointTradeCount", sItem.sItemCustom.wRanPointTradeCount );
		Ado.GetCollect( "ItemAttack", sItem.sItemCustom.cDAMAGE );
		Ado.GetCollect( "ItemDefense", sItem.sItemCustom.cDEFENSE );
		Ado.GetCollect( "ItemRandomType1", sItem.sItemCustom.randomOption.option[0].cOptType );
		Ado.GetCollect( "ItemRandomValue1", sItem.sItemCustom.randomOption.option[0].nOptValue );
		Ado.GetCollect( "ItemRandomType2", sItem.sItemCustom.randomOption.option[1].cOptType );
		Ado.GetCollect( "ItemRandomValue2", sItem.sItemCustom.randomOption.option[1].nOptValue );
		Ado.GetCollect( "ItemRandomType3", sItem.sItemCustom.randomOption.option[2].cOptType );
		Ado.GetCollect( "ItemRandomValue3", sItem.sItemCustom.randomOption.option[2].nOptValue );
		Ado.GetCollect( "ItemRandomType4", sItem.sItemCustom.randomOption.option[3].cOptType );
		Ado.GetCollect( "ItemRandomValue4", sItem.sItemCustom.randomOption.option[3].nOptValue );
		Ado.GetCollect( "ItemRandomType5", sItem.sItemCustom.randomOption.option[4].cOptType );
		Ado.GetCollect( "ItemRandomValue5", sItem.sItemCustom.randomOption.option[4].nOptValue );
		Ado.GetCollect( "ItemResistFire", sItem.sItemCustom.cRESIST_FIRE );
		Ado.GetCollect( "ItemResistIce", sItem.sItemCustom.cRESIST_ICE );
		Ado.GetCollect( "ItemResistElec", sItem.sItemCustom.cRESIST_ELEC );
		Ado.GetCollect( "ItemResistPoison", sItem.sItemCustom.cRESIST_POISON );
		Ado.GetCollect( "ItemResistSpirit", sItem.sItemCustom.cRESIST_SPIRIT );
		Ado.GetCollect( "ItemPetNum", sItem.sItemCustom.m_PetDbNum );
		Ado.GetCollect( "ItemVehicleNum", sItem.sItemCustom.m_VehicleDbNum );
		Ado.GetCollect( "ItemMainColor", sItem.sItemCustom.dwMainColor );
		Ado.GetCollect( "ItemSubColor", sItem.sItemCustom.dwSubColor );

		Ado.GetCollect( "CostumeStatType1", sItem.sItemCustom.costumeUser.Invest[0].cStatType );
		Ado.GetCollect( "CostumeInvestPoint1", sItem.sItemCustom.costumeUser.Invest[0].wInvestPt );
		Ado.GetCollect( "CostumeStatType2", sItem.sItemCustom.costumeUser.Invest[1].cStatType );
		Ado.GetCollect( "CostumeInvestPoint2", sItem.sItemCustom.costumeUser.Invest[1].wInvestPt );
		Ado.GetCollect( "CostumeStatType3", sItem.sItemCustom.costumeUser.Invest[2].cStatType );
		Ado.GetCollect( "CostumeInvestPoint3", sItem.sItemCustom.costumeUser.Invest[2].wInvestPt );
		Ado.GetCollect( "CostumeStatType4", sItem.sItemCustom.costumeUser.Invest[3].cStatType );
		Ado.GetCollect( "CostumeInvestPoint4", sItem.sItemCustom.costumeUser.Invest[3].wInvestPt );
		Ado.GetCollect( "CostumeStatType5", sItem.sItemCustom.costumeUser.Invest[4].cStatType );
		Ado.GetCollect( "CostumeInvestPoint5", sItem.sItemCustom.costumeUser.Invest[4].wInvestPt );
		Ado.GetCollect( "CostumeStatType6", sItem.sItemCustom.costumeUser.Invest[5].cStatType );
		Ado.GetCollect( "CostumeInvestPoint6", sItem.sItemCustom.costumeUser.Invest[5].wInvestPt );
		Ado.GetCollect( "CostumeStatType7", sItem.sItemCustom.costumeUser.Invest[6].cStatType );
		Ado.GetCollect( "CostumeInvestPoint7", sItem.sItemCustom.costumeUser.Invest[6].wInvestPt );
		Ado.GetCollect( "CostumeStatEndDate", CostumeStatEndDate );
		if ( CostumeStatEndDate.GetVARIANT().vt == VT_DATE )
			sItem.sItemCustom.costumeUser.tEndTime = sc::time::GetTime( CostumeStatEndDate );

		Ado.GetCollect( "TempGrindValue", sItem.sItemCustom.sTLGrind.cGradeValue );
		Ado.GetCollect( "TempGrindFireDate", TempGrindFireDate );
		if ( TempGrindFireDate.GetVARIANT().vt == VT_DATE )
			sItem.sItemCustom.sTLGrind.tFireTime = sc::time::GetTime( TempGrindFireDate );

		BYTE LockBox = 0;
		Ado.GetCollect( "LockBox", LockBox );
		sItem.sItemCustom.bVietnamGainItem = LockBox != 0 ? true : false;

		Ado.GetCollect( "BasicAttackDamage", dwBasicAttackDamageData );
		Ado.GetCollect( "BasicDefence", wBasicDefence );

		if ( dwBasicAttackDamageData == MAXDWORD && wBasicDefence == MAXWORD )
		{
			// 두 값이 모두 세팅이 안되어 있는 상태라면 세팅해준다;
			// 이는 두 값이 0인 것과는 다른 의미이다;
			// 두 값이 모두 최고인 아이템은 존재하지도 않으며 존재해서는 안된다;
			sItem.sItemCustom.GenerateBasicStat();
		}
		else
		{
			sItem.sItemCustom.SetBasicAttackDamage( dwBasicAttackDamageData );
			sItem.sItemCustom.SetBasicDefence( wBasicDefence );
		}

		Ado.GetCollect( "ItemLinkSkillMID", sLinkSkillID.wMainID );
		Ado.GetCollect( "ItemLinkSkillSID", sLinkSkillID.wSubID );
		Ado.GetCollect( "ItemLinkSkillLevel", wLinkSkillLevel );
		Ado.GetCollect( "ItemLinkSkillTarget", wLinkSkillTarget );
		Ado.GetCollect( "ItemLinkSkillOccurRate", fLinkSkillOccurRate );

		if ( sLinkSkillID.dwID == MAXDWORD &&
			wLinkSkillLevel == MAXWORD &&
			wLinkSkillTarget == MAXWORD &&
			fLinkSkillOccurRate == -1.f )
		{
			sItem.sItemCustom.GenerateLinkSkill();
		}
		else
		{
			sItem.sItemCustom.SetLinkSkillID( sLinkSkillID );
			sItem.sItemCustom.SetLinkSkillLevel( wLinkSkillLevel );
			sItem.sItemCustom.SetLinkSkillTarget( static_cast< EMITEM_SKILL_LINK >( wLinkSkillTarget ) );
			sItem.sItemCustom.SetLinkSkillOccurRate( fLinkSkillOccurRate );
		}

		WORD wType(EMADD_NOTINIT);
		Ado.GetCollect( "ItemAddonType1", wType);sItem.sItemCustom.sAddonInfo.m_sADDON[0].emTYPE = (EMITEM_ADDON)wType;
		Ado.GetCollect( "ItemAddonType2", wType);sItem.sItemCustom.sAddonInfo.m_sADDON[1].emTYPE = (EMITEM_ADDON)wType;
		Ado.GetCollect( "ItemAddonType3", wType);sItem.sItemCustom.sAddonInfo.m_sADDON[2].emTYPE = (EMITEM_ADDON)wType;
		Ado.GetCollect( "ItemAddonType4", wType);sItem.sItemCustom.sAddonInfo.m_sADDON[3].emTYPE = (EMITEM_ADDON)wType;
		Ado.GetCollect( "ItemAddonType5", wType);sItem.sItemCustom.sAddonInfo.m_sADDON[4].emTYPE = (EMITEM_ADDON)wType;
		Ado.GetCollect( "ItemAddonType6", wType);sItem.sItemCustom.sAddonInfo.m_sADDON[5].emTYPE = (EMITEM_ADDON)wType;
		if ( sItem.sItemCustom.sAddonInfo.IsInit() == false )
		{
			sItem.sItemCustom.GenerateAddOption();
		}
		else
		{
			Ado.GetCollect( "ItemAddonValue1", sItem.sItemCustom.sAddonInfo.m_sADDON[0].nVALUE );
			Ado.GetCollect( "ItemAddonValue2", sItem.sItemCustom.sAddonInfo.m_sADDON[1].nVALUE );
			Ado.GetCollect( "ItemAddonValue3", sItem.sItemCustom.sAddonInfo.m_sADDON[2].nVALUE );
			Ado.GetCollect( "ItemAddonValue4", sItem.sItemCustom.sAddonInfo.m_sADDON[3].nVALUE );
			Ado.GetCollect( "ItemAddonValue5", sItem.sItemCustom.sAddonInfo.m_sADDON[4].nVALUE );
			Ado.GetCollect( "ItemAddonValue6", sItem.sItemCustom.sAddonInfo.m_sADDON[5].nVALUE );
		}

		sItem.sItemCustom.SetDbState( db::DB_UPDATE );

		vecItems.push_back( sItem );
	} while ( Ado.Next() );

	return sc::db::DB_OK;
}

int AdoManager::ItemSelect( DWORD ChaDbNum, INVEN_TYPE InvenType, std::multimap< DWORD, SINVENITEM_SAVE >& mapItems )
{
	sc::db::AdoExt Ado( m_GameDBConnString );
	APPEND_IPARAM_INT( Ado, "@ChaNum", ChaDbNum );
	APPEND_IPARAM_TINYINT( Ado, "@InvenType", InvenType );
	if ( !Ado.Execute4Cmd( "dbo.sp_ItemGetItemList", adCmdStoredProc ) )
		return sc::db::DB_ERROR;

	if ( Ado.GetEOF() )
	{
		return sc::db::DB_OK;
	}

	do
	{
		std::string strGuid;
		SNATIVEID sItemID( false );
		_variant_t CreateDate, CostumeExpireDate, CostumeStatEndDate, TempGrindFireDate, BasicPeriodEx, DisguisePeriodEx;
		SINVENITEM_SAVE sItem;

		DWORD dwBasicAttackDamageData = 0;
		WORD wBasicDefence = 0;

		SNATIVEID sLinkSkillID( false );
		WORD wLinkSkillLevel = 0;
		WORD wLinkSkillTarget = 0;
		float fLinkSkillOccurRate = 0.f;

		Ado.GetCollect( "ItemUUID", strGuid );
		char szGuid[ sc::string::UUID_LENGTH_BRACES ] = { 0 };
		StringCchCopy( szGuid, sc::string::UUID_LENGTH_BRACES, strGuid.c_str() );
		sc::string::stringToUUID( szGuid, sItem.sItemCustom.guid );

		Ado.GetCollect( "ItemMID", sItemID.wMainID );
		Ado.GetCollect( "ItemSID", sItemID.wSubID );

		sItem.sItemCustom.SetNativeID( sItemID );

		Ado.GetCollect( "ItemMakeType", sItem.sItemCustom.cGenType );
		Ado.GetCollect( "ItemCostumeMID", sItem.sItemCustom.nidDISGUISE.wMainID );
		Ado.GetCollect( "ItemCostumeSID", sItem.sItemCustom.nidDISGUISE.wSubID );

		Ado.GetCollect( "ItemPosX", sItem.wPosX );
		Ado.GetCollect( "ItemPosY", sItem.wPosY );

		Ado.GetCollect( "ItemRemain", sItem.sItemCustom.wTurnNum );

		Ado.GetCollect( "ItemCreateDate", CreateDate );
		Ado.GetCollect( "ItemCostumeExpireDate", CostumeExpireDate );

		Ado.GetCollect( "ItemBasicPeriodExTime", BasicPeriodEx );
		Ado.GetCollect( "ItemDisguisePeriodExTime", DisguisePeriodEx );
		Ado.GetCollect( "ItemBasicPeriodExCount", sItem.sItemCustom.cPeriodExtensionCount );
		Ado.GetCollect( "ItemDisuisePeriodExCount", sItem.sItemCustom.cCostumePeriodExtensionCount );

		if ( CreateDate.GetVARIANT().vt == VT_DATE )
			sItem.sItemCustom.tBORNTIME = sc::time::GetTime( CreateDate );

		if ( CostumeExpireDate.GetVARIANT().vt == VT_DATE )
			sItem.sItemCustom.tDISGUISE = sc::time::GetTime( CostumeExpireDate );

		if ( BasicPeriodEx.GetVARIANT().vt == VT_DATE )
			sItem.sItemCustom.SetPeriodExBasic_FromDB(sc::time::GetTime( BasicPeriodEx ));

		if ( DisguisePeriodEx.GetVARIANT().vt == VT_DATE )
			sItem.sItemCustom.SetPeriodExDisguise_FromDB(sc::time::GetTime( DisguisePeriodEx ));

		Ado.GetCollect( "ItemDurability", sItem.sItemCustom.wDurability );
		Ado.GetCollect( "ItemDurabilityCount", sItem.sItemCustom.dwDurabilityCount );
		Ado.GetCollect( "PointTradeCount", sItem.sItemCustom.wRanPointTradeCount );
		Ado.GetCollect( "ItemAttack", sItem.sItemCustom.cDAMAGE );
		Ado.GetCollect( "ItemDefense", sItem.sItemCustom.cDEFENSE );
		Ado.GetCollect( "ItemRandomType1", sItem.sItemCustom.randomOption.option[0].cOptType );
		Ado.GetCollect( "ItemRandomValue1", sItem.sItemCustom.randomOption.option[0].nOptValue );
		Ado.GetCollect( "ItemRandomType2", sItem.sItemCustom.randomOption.option[1].cOptType );
		Ado.GetCollect( "ItemRandomValue2", sItem.sItemCustom.randomOption.option[1].nOptValue );
		Ado.GetCollect( "ItemRandomType3", sItem.sItemCustom.randomOption.option[2].cOptType );
		Ado.GetCollect( "ItemRandomValue3", sItem.sItemCustom.randomOption.option[2].nOptValue );
		Ado.GetCollect( "ItemRandomType4", sItem.sItemCustom.randomOption.option[3].cOptType );
		Ado.GetCollect( "ItemRandomValue4", sItem.sItemCustom.randomOption.option[3].nOptValue );
		Ado.GetCollect( "ItemRandomType4", sItem.sItemCustom.randomOption.option[4].cOptType );
		Ado.GetCollect( "ItemRandomValue4", sItem.sItemCustom.randomOption.option[4].nOptValue );
		Ado.GetCollect( "ItemResistFire", sItem.sItemCustom.cRESIST_FIRE );
		Ado.GetCollect( "ItemResistIce", sItem.sItemCustom.cRESIST_ICE );
		Ado.GetCollect( "ItemResistElec", sItem.sItemCustom.cRESIST_ELEC );
		Ado.GetCollect( "ItemResistPoison", sItem.sItemCustom.cRESIST_POISON );
		Ado.GetCollect( "ItemResistSpirit", sItem.sItemCustom.cRESIST_SPIRIT );
		Ado.GetCollect( "ItemPetNum", sItem.sItemCustom.m_PetDbNum );
		Ado.GetCollect( "ItemVehicleNum", sItem.sItemCustom.m_VehicleDbNum );
		Ado.GetCollect( "ItemMainColor", sItem.sItemCustom.dwMainColor );
		Ado.GetCollect( "ItemSubColor", sItem.sItemCustom.dwSubColor );

		Ado.GetCollect( "CostumeStatType1", sItem.sItemCustom.costumeUser.Invest[0].cStatType );
		Ado.GetCollect( "CostumeInvestPoint1", sItem.sItemCustom.costumeUser.Invest[0].wInvestPt );
		Ado.GetCollect( "CostumeStatType2", sItem.sItemCustom.costumeUser.Invest[1].cStatType );
		Ado.GetCollect( "CostumeInvestPoint2", sItem.sItemCustom.costumeUser.Invest[1].wInvestPt );
		Ado.GetCollect( "CostumeStatType3", sItem.sItemCustom.costumeUser.Invest[2].cStatType );
		Ado.GetCollect( "CostumeInvestPoint3", sItem.sItemCustom.costumeUser.Invest[2].wInvestPt );
		Ado.GetCollect( "CostumeStatType4", sItem.sItemCustom.costumeUser.Invest[3].cStatType );
		Ado.GetCollect( "CostumeInvestPoint4", sItem.sItemCustom.costumeUser.Invest[3].wInvestPt );
		Ado.GetCollect( "CostumeStatType5", sItem.sItemCustom.costumeUser.Invest[4].cStatType );
		Ado.GetCollect( "CostumeInvestPoint5", sItem.sItemCustom.costumeUser.Invest[4].wInvestPt );
		Ado.GetCollect( "CostumeStatType6", sItem.sItemCustom.costumeUser.Invest[5].cStatType );
		Ado.GetCollect( "CostumeInvestPoint6", sItem.sItemCustom.costumeUser.Invest[5].wInvestPt );
		Ado.GetCollect( "CostumeStatType7", sItem.sItemCustom.costumeUser.Invest[6].cStatType );
		Ado.GetCollect( "CostumeInvestPoint7", sItem.sItemCustom.costumeUser.Invest[6].wInvestPt );
		Ado.GetCollect( "CostumeStatEndDate", CostumeStatEndDate );
		if ( CostumeStatEndDate.GetVARIANT().vt == VT_DATE )
			sItem.sItemCustom.costumeUser.tEndTime = sc::time::GetTime( CostumeStatEndDate );

		Ado.GetCollect( "TempGrindValue", sItem.sItemCustom.sTLGrind.cGradeValue );
		Ado.GetCollect( "TempGrindFireDate", TempGrindFireDate );
		if ( TempGrindFireDate.GetVARIANT().vt == VT_DATE )
			sItem.sItemCustom.sTLGrind.tFireTime = sc::time::GetTime( TempGrindFireDate );

		BYTE LockBox = 0;
		Ado.GetCollect( "LockBox", LockBox );
		sItem.sItemCustom.bVietnamGainItem = LockBox != 0 ? true : false;

		Ado.GetCollect( "BasicAttackDamage", dwBasicAttackDamageData );
		Ado.GetCollect( "BasicDefence", wBasicDefence );

		if ( dwBasicAttackDamageData == MAXDWORD && wBasicDefence == MAXWORD )
		{
			// 두 값이 모두 세팅이 안되어 있는 상태라면 세팅해준다;
			// 이는 두 값이 0인 것과는 다른 의미이다;
			// 두 값이 모두 최고인 아이템은 존재하지도 않으며 존재해서는 안된다;
			sItem.sItemCustom.GenerateBasicStat();
		}
		else
		{
			sItem.sItemCustom.SetBasicAttackDamage( dwBasicAttackDamageData );
			sItem.sItemCustom.SetBasicDefence( wBasicDefence );
		}

		Ado.GetCollect( "ItemLinkSkillMID", sLinkSkillID.wMainID );
		Ado.GetCollect( "ItemLinkSkillSID", sLinkSkillID.wSubID );
		Ado.GetCollect( "ItemLinkSkillLevel", wLinkSkillLevel );
		Ado.GetCollect( "ItemLinkSkillTarget", wLinkSkillTarget );
		Ado.GetCollect( "ItemLinkSkillOccurRate", fLinkSkillOccurRate );

		if ( sLinkSkillID.dwID == MAXDWORD &&
			wLinkSkillLevel == MAXWORD &&
			wLinkSkillTarget == MAXWORD &&
			fLinkSkillOccurRate == -1.f )
		{
			sItem.sItemCustom.GenerateLinkSkill();
		}
		else
		{
			sItem.sItemCustom.SetLinkSkillID( sLinkSkillID );
			sItem.sItemCustom.SetLinkSkillLevel( wLinkSkillLevel );
			sItem.sItemCustom.SetLinkSkillTarget( static_cast< EMITEM_SKILL_LINK >( wLinkSkillTarget ) );
			sItem.sItemCustom.SetLinkSkillOccurRate( fLinkSkillOccurRate );
		}

		WORD wType(EMADD_NOTINIT);
		Ado.GetCollect( "ItemAddonType1", wType);sItem.sItemCustom.sAddonInfo.m_sADDON[0].emTYPE = (EMITEM_ADDON)wType;
		Ado.GetCollect( "ItemAddonType2", wType);sItem.sItemCustom.sAddonInfo.m_sADDON[1].emTYPE = (EMITEM_ADDON)wType;
		Ado.GetCollect( "ItemAddonType3", wType);sItem.sItemCustom.sAddonInfo.m_sADDON[2].emTYPE = (EMITEM_ADDON)wType;
		Ado.GetCollect( "ItemAddonType4", wType);sItem.sItemCustom.sAddonInfo.m_sADDON[3].emTYPE = (EMITEM_ADDON)wType;
		Ado.GetCollect( "ItemAddonType5", wType);sItem.sItemCustom.sAddonInfo.m_sADDON[4].emTYPE = (EMITEM_ADDON)wType;
		Ado.GetCollect( "ItemAddonType6", wType);sItem.sItemCustom.sAddonInfo.m_sADDON[5].emTYPE = (EMITEM_ADDON)wType;
		if ( sItem.sItemCustom.sAddonInfo.IsInit() == false )
		{
			sItem.sItemCustom.GenerateAddOption();
		}
		else
		{
			Ado.GetCollect( "ItemAddonValue1", sItem.sItemCustom.sAddonInfo.m_sADDON[0].nVALUE );
			Ado.GetCollect( "ItemAddonValue2", sItem.sItemCustom.sAddonInfo.m_sADDON[1].nVALUE );
			Ado.GetCollect( "ItemAddonValue3", sItem.sItemCustom.sAddonInfo.m_sADDON[2].nVALUE );
			Ado.GetCollect( "ItemAddonValue4", sItem.sItemCustom.sAddonInfo.m_sADDON[3].nVALUE );
			Ado.GetCollect( "ItemAddonValue5", sItem.sItemCustom.sAddonInfo.m_sADDON[4].nVALUE );
			Ado.GetCollect( "ItemAddonValue6", sItem.sItemCustom.sAddonInfo.m_sADDON[5].nVALUE );
		}

		sItem.sItemCustom.SetDbState( db::DB_UPDATE );
		mapItems.insert( std::make_pair( sItem.sItemCustom.nidDISGUISE.dwID, sItem ) );
	} while ( Ado.Next() );

	return sc::db::DB_OK;
}

int AdoManager::GetPutonItems( DWORD ChaDbNum, SCHARDATA2* pChaData2 )
{
	if ( !pChaData2 )
		return sc::db::DB_ERROR;

	std::vector< SINVENITEM_SAVE > vecItems;
	if ( sc::db::DB_ERROR == ItemSelect( ChaDbNum, INVEN_PUTON, vecItems ) )
	{
		return sc::db::DB_ERROR;
	}

	size_t ItemSize = vecItems.size();
	for ( size_t loop = 0; loop < ItemSize; loop++ )
	{
		SINVENITEM_SAVE& sItem = vecItems[loop];
		pChaData2->SETPUTONITEM( static_cast< EMSLOT >( sItem.wPosX ), sItem.sItemCustom );
	}

	return sc::db::DB_OK;
}

int AdoManager::GetInvenItems( DWORD ChaDbNum, SCHARDATA2* pChaData2 )
{
	if ( !pChaData2 )
		return sc::db::DB_ERROR;

	std::vector< SINVENITEM_SAVE > vecItems;
	if ( sc::db::DB_ERROR == ItemSelect( ChaDbNum, INVEN_INVEN, vecItems ) )
	{
		return sc::db::DB_ERROR;
	}

	pChaData2->SETINVENTORY_BYVECTOR( vecItems );

	return sc::db::DB_OK;
}

int AdoManager::GetLockerItems( DWORD UserNum, SCHARDATA2* pChaData2 )
{
	// 예전에는 sp_ItemGetItemLocker 를 사용했으나 ItemSelect <sp_ItemGetItemList> 하나만 사용하도록 변경한다.
	// sp_ItemGetItemLocker 안에서 UserInvenOption 을 1로 변경하였으나 이제 따로 UpdateUserLockerOption 호출하도록 함
	if ( !pChaData2 )
		return sc::db::DB_ERROR;

	if ( sc::db::DB_ERROR == UpdateUserLockerOption( UserNum, 1 ) )
	{
		return sc::db::DB_ERROR;
	}

	std::vector< SINVENITEM_SAVE > vecItems;
	if ( sc::db::DB_ERROR == ItemSelect( UserNum, INVEN_LOCKER, vecItems ) )
	{
		// 유저락커옵션 다시 돌림
		UpdateUserLockerOption( UserNum, 0 );
		return sc::db::DB_ERROR;
	}

	pChaData2->SETSTORAGE_BYVECTOR( vecItems );

	return sc::db::DB_OK;
}

int AdoManager::GetClubLockerItems( DWORD ClubNum, std::vector< SINVENITEM_SAVE >& vecItems )
{
	return ItemSelect( ClubNum, INVEN_CLUB_LOCKER, vecItems );
}

int AdoManager::TransferLockerItems( DWORD UserNum /*= 0*/ )
{
	// 사용하지 않는다.
	return sc::db::DB_OK;
}

int AdoManager::GetChaNumToTransfer( std::vector< DWORD >& vecChaNum )
{
	sc::db::AdoExt AdoGame( m_GameDBConnString );
	if ( !AdoGame.Execute4Cmd( "dbo.sp_GetAllCharacterItemTransferStatus", adCmdStoredProc ) )
	{
		return sc::db::DB_ERROR;
	}

	if ( AdoGame.GetEOF() )
	{
		return sc::db::DB_OK;
	}

	do
	{
		DWORD Num = 0;
		AdoGame.GetCollect( "ChaNum", Num );
		vecChaNum.push_back( Num );
	} while ( AdoGame.Next() );

	return sc::db::DB_OK;
}

int AdoManager::TransferCharacterItems( DWORD ChaDbNum /*= 0*/ )
{
	// 사용하지 않는다.
	return sc::db::DB_OK;
}

int AdoManager::TransferItems( DWORD UserNum, std::vector< DWORD >& vecChaNum )
{
	// 유저락커
	{
		// 변환 상태 확인
		sc::db::AdoExt AdoGame( m_GameDBConnString );
		APPEND_IPARAM_INT( AdoGame, "@UserNum", UserNum );
		if ( !AdoGame.Execute4Cmd( "dbo.sp_GetUserItemTransferStatus", adCmdStoredProc ) )
		{
			sc::writeLogError( sc::string::format( "TransferItems sp_GetUserItemTransferStatus failed. usernum %d", UserNum ) );
			return sc::db::DB_ERROR;
		}

		// 테이블에 데이터가 있는 유저만 변환한다. 데이터가 없는 경우는 초기에 넣는 것을 빼먹었거나, 신규 유저이다.
		if ( FALSE == AdoGame.GetEOF() )
		{
			int InvenStatus = 0;
			do
			{
				AdoGame.GetCollect( "Inven", InvenStatus );
			} while ( AdoGame.Next() );
			
			// 0 초기 상태
			// 1 변환 완료
			// 그 외는 에러 상황
			if ( 1 != InvenStatus )
			{
				// 변환한다
				// 가져오기
				int TransferResult = 1;
				SCHARDATA2 sCharacter;
				sCharacter.SetUserID( UserNum );
				if ( sc::db::DB_ERROR == ReadUserInvenForItemTransfer( m_pServer->ServerGroup(), sCharacter.GetUserID(), &sCharacter ) )
				{
					TransferResult = 2;
					sc::writeLogError( sc::string::format( "TransferItems ReadUserInven failed. usernum %d", UserNum ) );
				}
				else
				{
					// 저장
					std::vector< SINVENITEM_SAVE > vecItems;
					sCharacter.GETSTORAGE_BYVECTOR( vecItems );
					size_t ItemSize = vecItems.size();
					for ( size_t loop = 0; loop < ItemSize; loop++ )
					{
						SINVENITEM_SAVE& sItem = vecItems[loop];
						sc::SeqUniqueGUID::Instance()->getGUID( sItem.sItemCustom.guid );
						sItem.sItemCustom.GenerateBasicStat( false );
						sItem.sItemCustom.GenerateLinkSkill();
						sItem.sItemCustom.GenerateAddOption();
						ItemInsert( UserNum, INVEN_LOCKER, &vecItems[loop], INSERT_USER_LOCKER_ITEM_FOR_TRANSFER );
					}
				}

				sc::db::AdoExt Ado( m_GameDBConnString );
				APPEND_RPARAM_INT( Ado );
				APPEND_IPARAM_INT( Ado, "@UserNum", UserNum );
				APPEND_IPARAM_INT( Ado, "@Inven", TransferResult );
				APPEND_OPARAM_INT( Ado, "@Return");
				if ( -1 == Ado.ExecuteStoredProcedureIntReturn( "dbo.sp_SetUserItemTransferStatus", "@Return" ) )
				{
					sc::writeLogError( sc::string::format( "AdoManager::TransferItems sp_SetUserItemTransferStatus error UserNum %1%", UserNum ) );
				}
			}
		}
	}

	size_t ChaNumSize = vecChaNum.size();
	for ( size_t loop = 0; loop < ChaNumSize; loop++ )
	{
		// GS 필리핀 마이그레이션 처리 추가
		// 유저락커 아이템은 우편으로 모두 전송되어 있는 상황
		// 퀘스트는 진행중인 퀘스트는 모두 삭제 처리, 완료 퀘스트도 GS 없는 퀘스트는 삭제
		// 마이그레이션 처리 먼저 하고, 아이템 변환 한다.
		int MigrationTarget = 0;

		switch ( m_ServiceProvider )
		{
		case SP_GS:
			{
				if ( 1 == IsMigrationTarget( vecChaNum[loop] ) )
				{
					MigrationTarget = 1;
				}
			}
			break;
		default:
			break;
		}

		SCHARDATA2 sCharacter;
		sCharacter.SetUserID( UserNum );
		DWORD ChaDbNum = vecChaNum[loop];
		
		// 변환 상태 확인
		sc::db::AdoExt AdoGame( m_GameDBConnString );
		APPEND_IPARAM_INT( AdoGame, "@ChaNum", ChaDbNum );
		if ( !AdoGame.Execute4Cmd( "dbo.sp_GetCharacterItemTransferStatus", adCmdStoredProc ) )
		{
			return sc::db::DB_ERROR;
		}

		int PutonStatus = 0;
		int InvenStatus = 0;
		int QuestStatus = 0;
		int PetStatus = 0;
		int VehicleStatus = 0;
		int PostStatus = 0;
		int PrivateMarketStatus = 0;

		int PutonTransferResult = 1;
		int InvenTransferResult = 1;
		int QuestTransferResult = 1;
		int PetTransferResult = 1;
		int VehicleTransferResult = 1;
		int PostTransferResult = 1;
		int PrivateMarketTransferResult = 1;

		if ( AdoGame.GetEOF() )
		{
			continue;
		}

		do
		{
			AdoGame.GetCollect( "Puton", PutonStatus );
			AdoGame.GetCollect( "Inven", InvenStatus );
			AdoGame.GetCollect( "Quest", QuestStatus );
			AdoGame.GetCollect( "Pet", PetStatus );
			AdoGame.GetCollect( "Vehicle", VehicleStatus );
			AdoGame.GetCollect( "Post", PostStatus );
			AdoGame.GetCollect( "PrivateMarket", PrivateMarketStatus );
		} while ( AdoGame.Next() );

		{
			// puton
			if ( 1 != PutonStatus )
			{
				// 착용 아이템 가져오기
				int Result = sc::db::DB_ERROR;
				if ( MigrationTarget )
				{
					Result = GetCharacterInfoPutOnItemsForMigration( ChaDbNum, &sCharacter );
				}
				else
				{
					Result = GetCharacterInfoPutOnItems( ChaDbNum, &sCharacter );
				}

				if ( sc::db::DB_ERROR == Result )
				{
					PutonTransferResult = 2;
					sc::writeLogError( sc::string::format( "TransferItems puton failed. chanum %d", ChaDbNum ) );
				}
				else
				{
					// 착용 아이템 저장
					std::vector< SINVENITEM_SAVE > vecItems;
					sCharacter.GETPUTONITEMS_BYVECTOR( vecItems );
					size_t ItemSize = vecItems.size();
					for ( size_t loop = 0; loop < ItemSize; loop++ )
					{
						SINVENITEM_SAVE& sItem = vecItems[loop];
						sc::SeqUniqueGUID::Instance()->getGUID( sItem.sItemCustom.guid );
						sItem.sItemCustom.GenerateBasicStat( false );
						sItem.sItemCustom.GenerateLinkSkill();

						ItemInsert( ChaDbNum, INVEN_PUTON, &vecItems[loop], INSERT_PUTON_ITEM_FOR_TRANSFER );
					}
				}
			}
		}

		{
			// inventory
			if ( 1 != InvenStatus )
			{
				// 인벤토리 가져오기
				int Result = sc::db::DB_ERROR;
				if ( MigrationTarget )
				{
					Result = GetCharacterInfoInvenForMigration( ChaDbNum, &sCharacter );
				}
				else
				{
					Result = GetCharacterInfoInven( ChaDbNum, &sCharacter );
				}

				if ( sc::db::DB_ERROR == Result )
				{
					InvenTransferResult = 2;
					sc::writeLogError( sc::string::format( "TransferItems inven failed. chanum %d", ChaDbNum ) );
				}
				else
				{
					// 인벤토리 저장
					std::vector< SINVENITEM_SAVE > vecItems;
					sCharacter.m_cInventory.GET_ITEM_BY_VECTOR( vecItems );
					size_t ItemSize = vecItems.size();
					for ( size_t loop = 0; loop < ItemSize; loop++ )
					{
						SINVENITEM_SAVE& sItem = vecItems[loop];
						sc::SeqUniqueGUID::Instance()->getGUID( sItem.sItemCustom.guid );
						sItem.sItemCustom.GenerateBasicStat( false );
						sItem.sItemCustom.GenerateLinkSkill();
						sItem.sItemCustom.GenerateAddOption();
						ItemInsert( ChaDbNum, INVEN_INVEN, &vecItems[loop], INSERT_INVEN_ITEM_FOR_TRANSFER );
					}
				}
			}
		}

		{
			// quest
			if ( 1 != QuestStatus )
			{
				// 퀘스트 가져오기
				int Result = sc::db::DB_ERROR;
				if ( MigrationTarget )
				{
					Result = GetCharacterInfoQuestForMigration( ChaDbNum, &sCharacter );
				}
				else
				{
					Result = GetCharacterInfoQuest( ChaDbNum, &sCharacter );
				}				

				if ( sc::db::DB_ERROR == Result )
				{
					QuestTransferResult = 2;
					sc::writeLogError( sc::string::format( "TransferItems quest failed. chanum %d", ChaDbNum ) );
				}
				else
				{
					// 퀘스트 아이템 저장하기
					{
						// 진행 퀘스트
						GLQuestPlay::MAPQUEST& quest = sCharacter.m_cQuestPlay.GetQuestProc();
						GLQuestPlay::MAPQUEST_CITER iter = quest.begin();
						GLQuestPlay::MAPQUEST_CITER iter_end = quest.end();
						for ( ; iter != iter_end; ++iter )
						{
							std::vector< SINVENITEM_SAVE > vecItems;
							GLQUESTPROG* pQuestProg = (*iter).second;
							if ( !pQuestProg )
							{
								continue;
							}
							pQuestProg->m_sINVENTORY.GET_ITEM_BY_VECTOR( vecItems );
							size_t ItemSize = vecItems.size();
							for ( size_t loop = 0; loop < ItemSize; loop++ )
							{
								// Quest Item 일 경우에 Costume 의 MID, SID 가 Quest 의 MID,SID 이다.
								SINVENITEM_SAVE& sItem = vecItems[loop];
								sc::SeqUniqueGUID::Instance()->getGUID( sItem.sItemCustom.guid );
								sItem.sItemCustom.GenerateBasicStat( false );
								sItem.sItemCustom.GenerateLinkSkill();
								sItem.sItemCustom.GenerateAddOption();
								sItem.sItemCustom.nidDISGUISE.wMainID = pQuestProg->m_sNID.wMainID;
								sItem.sItemCustom.nidDISGUISE.wSubID = pQuestProg->m_sNID.wSubID;
								ItemInsert( ChaDbNum, INVEN_QUEST_PROC, &vecItems[loop], INSERT_QUEST_ITEM_FOR_TRANSFER );
							}
						}
					}

					{
						// 완료 퀘스트
						GLQuestPlay::MAPQUEST& quest = sCharacter.m_cQuestPlay.GetQuestEnd();
						GLQuestPlay::MAPQUEST_CITER iter = quest.begin();
						GLQuestPlay::MAPQUEST_CITER iter_end = quest.end();
						for ( ; iter != iter_end; ++iter )
						{
							std::vector< SINVENITEM_SAVE > vecItems;
							GLQUESTPROG* pQuestProg = (*iter).second;
							if ( !pQuestProg )
							{
								continue;
							}
							pQuestProg->m_sINVENTORY.GET_ITEM_BY_VECTOR( vecItems );
							size_t ItemSize = vecItems.size();
							for ( size_t loop = 0; loop < ItemSize; loop++ )
							{
								// Quest Item 일 경우에 Costume 의 MID, SID 가 Quest 의 MID,SID 이다.
								SINVENITEM_SAVE& sItem = vecItems[loop];
								sc::SeqUniqueGUID::Instance()->getGUID( sItem.sItemCustom.guid );
								sItem.sItemCustom.GenerateBasicStat( false );
								sItem.sItemCustom.GenerateLinkSkill();
								sItem.sItemCustom.GenerateAddOption();
								sItem.sItemCustom.nidDISGUISE.wMainID = pQuestProg->m_sNID.wMainID;
								sItem.sItemCustom.nidDISGUISE.wSubID = pQuestProg->m_sNID.wSubID;
								ItemInsert( ChaDbNum, INVEN_QUEST_END, &vecItems[loop], INSERT_QUEST_END_ITEM_FOR_TRANSFER );
							}
						}
					}

					{
						// 새로운 버전으로 퀘스트 저장
						sCharacter.m_CharDbNum = ChaDbNum;
						if ( sc::db::DB_OK != SaveCharacterQuest( &sCharacter ) )
						{
							QuestTransferResult = 3;
							sc::writeLogError( sc::string::format( "TransferItems quest save failed. chanum %d", ChaDbNum ) );
						}
					}
				}
			}
		}

		{
			if ( 1 != PetStatus )
			{
				// Pet
				// 캐릭터에 속하는 모든 pet 을 불러와서 아이템 변환한다.
				// 캐릭터가 가지고 있는 모든 펫 가져오기
				std::vector< GLPET > vecPets;
				if ( sc::db::DB_ERROR == GetAllPetPutonItems( ChaDbNum, vecPets ) )
				{
					PetTransferResult = 2;
					sc::writeLogError( sc::string::format( "TransferItems pet failed. chanum %d", ChaDbNum ) );
				}
				else
				{
					// 저장하기
					size_t PetSize = vecPets.size();
					for ( size_t loop = 0; loop < PetSize; loop++ )
					{
						std::vector< SINVENITEM_SAVE > vecItems;
						GLPET& sPet = vecPets[loop];
						sPet.GETPUTONITEMS_BYVECTOR( vecItems );

						size_t ItemSize = vecItems.size();
						for ( size_t loop2 = 0; loop2 < ItemSize; loop2++ )
						{
							// Pet puton Item 일 경우 아이템 테이블에 ChaNum 이 PetNum 이다. 
							SINVENITEM_SAVE& sItem = vecItems[loop2];
							sc::SeqUniqueGUID::Instance()->getGUID( sItem.sItemCustom.guid );
							sItem.sItemCustom.GenerateBasicStat( false );
							sItem.sItemCustom.GenerateLinkSkill();
							sItem.sItemCustom.GenerateAddOption();
							ItemInsert( sPet.m_DbNum, INVEN_PET, &vecItems[loop2], INSERT_PET_ITEM_FOR_TRANSFER );
						}

						if ( SP_GS == m_ServiceProvider )
						{
							// 펫 마이그레이션 처리
							SetPetMigrationState( ChaDbNum, sPet.m_DbNum, 1 );
						}
					}
				}
			}
		}

		{
			if ( 1 != VehicleStatus )
			{
				// Vehicle
				// 캐릭터에 속하는 모든 Vehicle 을 불러와서 아이템 변환한다.
				// 캐릭터가 가지고 있는 모든 Vehicle 가져오기
				std::vector< GLVEHICLE > vecVehicles;
				if ( sc::db::DB_ERROR == GetAllVehiclePutonItems( ChaDbNum, vecVehicles ) )
				{
					VehicleTransferResult = 2;
					sc::writeLogError( sc::string::format( "TransferItems vehicle failed. chanum %d", ChaDbNum ) );
				}
				else
				{
					// 저장하기
					size_t VehicleSize = vecVehicles.size();
					for ( size_t loop = 0; loop < VehicleSize; loop++ )
					{
						std::vector< SINVENITEM_SAVE > vecItems;
						GLVEHICLE& sVehicle = vecVehicles[loop];
						sVehicle.GETPUTONITEMS_BYVECTOR( vecItems );

						size_t ItemSize = vecItems.size();
						for ( size_t loop2 = 0; loop2 < ItemSize; loop2++ )
						{
							// Pet puton Item 일 경우 아이템 테이블에 ChaNum 이 PetNum 이다. 
							SINVENITEM_SAVE& sItem = vecItems[loop2];
							sc::SeqUniqueGUID::Instance()->getGUID( sItem.sItemCustom.guid );
							sItem.sItemCustom.GenerateBasicStat( false );
							sItem.sItemCustom.GenerateLinkSkill();
							sItem.sItemCustom.GenerateAddOption();
							ItemInsert( sVehicle.m_DbNum, INVEN_VEHICLE, &vecItems[loop2], INSERT_VEHICLE_ITEM_FOR_TRANSFER );
						}

						if ( SP_GS == m_ServiceProvider )
						{
							// 탈것 마이그레이션 처리
							SetVehicleMigrationState( ChaDbNum, sVehicle.m_DbNum, 1 );
						}
					}
				}
			}
		}

		{
			// Post
			PostTransferResult = 0;
		}

		{
			// PrivateMarket
			PrivateMarketTransferResult = 0;
		}

		sc::db::AdoExt Ado( m_GameDBConnString );
		APPEND_RPARAM_INT( Ado );
		APPEND_IPARAM_INT( Ado, "@ChaNum", ChaDbNum );
		APPEND_IPARAM_INT( Ado, "@Puton", PutonTransferResult );
		APPEND_IPARAM_INT( Ado, "@Inven", InvenTransferResult );
		APPEND_IPARAM_INT( Ado, "@Quest", QuestTransferResult );
		APPEND_IPARAM_INT( Ado, "@Pet", PetTransferResult );
		APPEND_IPARAM_INT( Ado, "@Vehicle", VehicleTransferResult );
		APPEND_IPARAM_INT( Ado, "@Post", PostTransferResult );
		APPEND_IPARAM_INT( Ado, "@PrivateMarket", PrivateMarketTransferResult );
		APPEND_OPARAM_INT( Ado, "@Return");
		if ( -1 == Ado.ExecuteStoredProcedureIntReturn( "dbo.sp_SetCharacterItemTransferStatus", "@Return" ) )
		{
			sc::writeLogError( sc::string::format( "AdoManager::TransferItems sp_SetCharacterItemTransferStatus error ChaNum %1%", ChaDbNum ) );
		}

		// 캐릭터 MigrationState 업데이트 필요하다. 펫, 탈것 따로 있다. 디비 프로젝트에서 MigrationState 검색
		switch ( m_ServiceProvider )
		{
		case SP_GS:
			{
				// 캐릭터
				SetMigrationTarget( ChaDbNum );
			}
			break;
		default:
			break;
		}
	}

	return sc::db::DB_OK;
}

int AdoManager::ItemUpdatePos( DWORD ChaDbNum, INVEN_TYPE InvenType, sc::MGUID& Guid, WORD PosX, WORD PosY, int CallType )
{
	std::string strGuid = sc::string::uuidToString( Guid );
	sc::string::upperString( strGuid );

	sc::db::AdoExt Ado( m_GameDBConnString );
	APPEND_RPARAM_INT( Ado );

	APPEND_IPARAM_VARCHAR( Ado, "@ItemUUID", strGuid.c_str(), strGuid.length() );
	APPEND_IPARAM_INT( Ado, "@ChaNum", ChaDbNum );
	APPEND_IPARAM_TINYINT( Ado, "@InvenType", InvenType );
	APPEND_IPARAM_TINYINT( Ado, "@ItemPosX", PosX );
	APPEND_IPARAM_TINYINT( Ado, "@ItemPosY", PosY );

	APPEND_OPARAM_INT( Ado, "@Return");

	int Return = Ado.ExecuteStoredProcedureIntReturn( "dbo.sp_ItemSavePos", "@Return" );
	if ( sc::db::DB_OK != Return )
	{
		sc::writeLogError( sc::string::format( "ItemUpdatePos error calltype: %1% chanum: %2% guid: %3%", CallType, ChaDbNum, strGuid.c_str() ) );
	}

	return Return;
}

int AdoManager::ItemUpdateTurnNum( sc::MGUID& Guid, WORD TurnNum, int CallType )
{
	std::string strGuid = sc::string::uuidToString( Guid );
	sc::string::upperString( strGuid );

	sc::db::AdoExt Ado( m_GameDBConnString );
	APPEND_RPARAM_INT( Ado );

	APPEND_IPARAM_VARCHAR( Ado, "@ItemUUID", strGuid.c_str(), strGuid.length() );
	APPEND_IPARAM_SMALL( Ado, "@ItemRemain", TurnNum );
	APPEND_OPARAM_INT( Ado, "@Return");

	int Return = Ado.ExecuteStoredProcedureIntReturn( "dbo.sp_ItemSaveTurnNum", "@Return" );
	if ( sc::db::DB_OK != Return )
	{
		sc::writeLogError( sc::string::format( "ItemUpdateTurnNum error calltype: %1% turnnum: %2% guid: %3%", CallType, TurnNum, strGuid.c_str() ) );
	}

	return Return;
}

int AdoManager::ItemInsertError( sc::MGUID& Guid, int CallType, __time64_t& Date )
{
	sc::MGUID null;
	if ( null == Guid )
		return sc::db::DB_ERROR;

	std::string strGuid = sc::string::uuidToString( Guid );
	sc::string::upperString( strGuid );

	sc::db::AdoExt Ado( m_GameDBConnString );
	APPEND_RPARAM_INT( Ado );
	APPEND_IPARAM_VARCHAR( Ado, "@ItemUUID", strGuid.c_str(), strGuid.length() );
	APPEND_IPARAM_INT( Ado, "@CallType", CallType );

	std::string ErrorDate = sc::time::DateTimeFormat( Date, true, true );
	APPEND_IPARAM_VARCHAR( Ado, "@ErrorDate", ErrorDate.c_str(), ErrorDate.length() );
	
	APPEND_OPARAM_INT( Ado, "@Return");

	int Return = Ado.ExecuteStoredProcedureIntReturn( "dbo.sp_ItemErrorInsert", "@Return" );
	if ( sc::db::DB_OK != Return )
	{
		sc::writeLogError( sc::string::format( "ItemInsertError error calltype: %1% guid: %2%", CallType, strGuid.c_str() ) );
	}

	return Return;
}

int AdoManager::GetCharacterItemTransferTarget( int UserNum, std::vector< DWORD >& vecChaNum )
{
	sc::db::AdoExt AdoGame( m_GameDBConnString );
	APPEND_IPARAM_INT( AdoGame, "@UserNum", UserNum );
	if ( !AdoGame.Execute4Cmd( "dbo.sp_GetCharacterItemTransferTarget", adCmdStoredProc ) )
	{
		return sc::db::DB_ERROR;
	}

	if ( AdoGame.GetEOF() )
	{
		return sc::db::DB_OK;
	}

	do
	{
		DWORD ChaNum = 0;
		AdoGame.GetCollect( "ChaNum", ChaNum );
		vecChaNum.push_back( ChaNum );
	} while ( AdoGame.Next() );

	return sc::db::DB_OK;
}

int AdoManager::GetItemTransferStatus( DWORD UserNum, int& Result )
{
	// 변환 상태 확인
	sc::db::AdoExt AdoGame( m_GameDBConnString );
	APPEND_IPARAM_INT( AdoGame, "@UserNum", UserNum );
	if ( !AdoGame.Execute4Cmd( "dbo.sp_GetItemTransferStatus", adCmdStoredProc ) )
	{
		sc::writeLogError( sc::string::format( "GetItemTransferStatus sp_GetItemTransferStatus failed. usernum %d", UserNum ) );
		return sc::db::DB_ERROR;
	}

	if ( AdoGame.GetEOF() )
	{
		Result = 0;
	}

	do
	{
		AdoGame.GetCollect( "Result", Result );
	} while ( AdoGame.Next() );

	return sc::db::DB_OK;
}

}