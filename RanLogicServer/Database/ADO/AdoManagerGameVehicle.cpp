#include "../../pch.h"
#include "../../../SigmaCore/gassert.h"
#include "./ADOManager.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace db
{

//! 새로운 탈것을 생성한다.
//! \param nChaNum  탈것 소유자 캐릭터 번호
//! \param szVehicleName 탈것 이름
//! \param nVehicleType 탈것의 타입
//! \param nVehicleMID 탈것 MID
//! \param nVehicleSID 탈것 SID
//! \param nVehicleCardMID 탈것의 아이템 MID
//! \param nVehicleCardSID 탈것의 아이템 SID
//! \param nBoosterID 탈것의 부스터 종류
//! \return sc::db::DB_ERROR 또는 성공했을때는 탈것의 고유번호
int AdoManager::CreateVehicle(
    int nCharNum,
    const TCHAR* szVehicleName,
    int nVehicleType,
    int nVehicleCardMID,
    int nVehicleCardSID)
{
    if (nCharNum <=0 || !szVehicleName)
    {
        sc::writeLogError("COdbcManager::CreateVehicle AdoManager::CreateVehicle");
        return sc::db::DB_ERROR;
    }

    std::string VehicleName(szVehicleName);
    sc::string::replaceForDb(VehicleName);

    sc::db::AdoExt AdoGame(m_GameDBConnString);
    APPEND_RPARAM_INT(AdoGame);

    APPEND_IPARAM_VARCHAR(AdoGame, "@szVehicleName",   VehicleName.c_str(), VehicleName.length());
    APPEND_IPARAM_INT    (AdoGame, "@nVehicleChaNum",  nCharNum);
    APPEND_IPARAM_INT    (AdoGame, "@nVehicleType",    nVehicleType);
    APPEND_IPARAM_INT    (AdoGame, "@nVehicleCardMID", nVehicleCardMID);
    APPEND_IPARAM_INT    (AdoGame, "@nVehicleCardSID", nVehicleCardSID);

    APPEND_OPARAM_INT(AdoGame, "@nReturn");

    return ExecuteStoredProcedureIntReturn("dbo.sp_InsertVehicle", AdoGame, "@nReturn");
}

//! 탈것의 배터리를 설정한다. 배터리는 0-1000 사이
//! \param nVehicleBttery 배터리를 설정할 탈것의 번호
//! \param nVehicleBttery 배터리
int AdoManager::SetVehicleBattery(
    int nVehicleNum,
    int nCharNum,
    int nVehicleBttery)
{
    if (nVehicleNum <= 0 || nVehicleBttery < 0)
    {
        sc::writeLogError("AdoManager::SetVehicleBattery nVehicleNum <= 0 || nVehicleBttery < 0");
        return sc::db::DB_ERROR;
    }

    sc::db::AdoExt AdoGame(m_GameDBConnString);
    APPEND_RPARAM_INT(AdoGame);

    APPEND_IPARAM_INT(AdoGame, "@nVehicleNum", nVehicleNum);
    APPEND_IPARAM_INT(AdoGame, "@nVehicleChaNum", nCharNum);
    APPEND_IPARAM_INT(AdoGame, "@nVehicleBattery", nVehicleBttery);

    APPEND_OPARAM_INT(AdoGame, "@nReturn");

    return ExecuteStoredProcedureIntReturn("dbo.sp_UpdateVehicleBattery", AdoGame, "@nReturn");
}

//! 탈것의 배터리를 가져온다.
//! \param nVehicleBttery 배터리를 설정할 탈것의 번호
int AdoManager::GetVehicleBattery(int nVehicleNum, int nCharNum)
{
    sc::db::AdoExt AdoGame(m_GameDBConnString);
    APPEND_RPARAM_INT(AdoGame);

    APPEND_IPARAM_INT(AdoGame, "@nVehicleNum", nVehicleNum);
    APPEND_IPARAM_INT(AdoGame, "@nVehicleChaNum", nCharNum);

    APPEND_OPARAM_INT(AdoGame, "@nReturn");

    return ExecuteStoredProcedureIntReturn("dbo.sp_GetVehicleBattery", AdoGame, "@nReturn");
}

//! 탈것을 삭제한다.
//! \param nVehicle 삭제할 탈것의 번호
int AdoManager::DeleteVehicle(int nVehicle, int nCharNum)
{
    if (nVehicle <= 0)
    {
        sc::writeLogError("AdoManager::DeleteVehicle nVehicle <= 0");
        return sc::db::DB_ERROR;
    }

    sc::db::AdoExt AdoGame(m_GameDBConnString);
    APPEND_RPARAM_INT(AdoGame);

    APPEND_IPARAM_INT(AdoGame, "@nVehicleNum", nVehicle);
    APPEND_IPARAM_INT(AdoGame, "@nVehicleChaNum", nCharNum);

    APPEND_OPARAM_INT(AdoGame, "@nReturn");
    
    return ExecuteStoredProcedureIntReturn("dbo.sp_DeleteVehicle", AdoGame, "@nReturn");
}

int AdoManager::RebuyVehicle(int nVehicle, int nCharNum)
{
    if (nVehicle <= 0)
    {
        sc::writeLogError("AdoManager::RebuyVehicle nVehicle <= 0");
        return sc::db::DB_ERROR;
    }

    sc::db::AdoExt AdoGame(m_GameDBConnString);
    APPEND_RPARAM_INT(AdoGame);

    APPEND_IPARAM_INT(AdoGame, "@nVehicleNum", nVehicle);
    APPEND_IPARAM_INT(AdoGame, "@nVehicleChaNum", nCharNum);

    APPEND_OPARAM_INT(AdoGame, "@nReturn");
    
    return ExecuteStoredProcedureIntReturn("dbo.sp_RebuyVehicle", AdoGame, "@nReturn");
}

//! 탈것의 정보를 가져온다.	
int AdoManager::GetVehicle(GLVEHICLE* pVehicle, int nVehicleNum, int nCharNum)
{
    if (!pVehicle)
    {
        sc::writeLogError("AdoManager::GetVehicle pVehicle=NULL");
        return sc::db::DB_ERROR;
    }
/*
    CREATE PROCEDURE dbo.VehicleInfoSelect
        @nVehicleChaNum int,
        @nVehicleNum	int	

        SELECT VehicleChaNum, VehicleCardMID, VehicleCardSID, VehicleType, VehicleBattery,
        VehicleBooster, VehiclePutOnItems, VehiclePartsColor
        FROM dbo.VehicleInfo
        WHERE VehicleChaNum = @nVehicleChaNum AND VehicleNum = @nVehicleNum 
*/

    sc::db::AdoExt AdoGame(m_GameDBConnString);
	APPEND_IPARAM_INT(AdoGame, "@nVehicleChaNum", nCharNum);
	APPEND_IPARAM_INT(AdoGame, "@nVehicleNum", nVehicleNum);
    /*APPEND_IPARAM_INT(AdoGame, "@nVehicleChaNum", nVehicleNum);
    APPEND_IPARAM_INT(AdoGame, "@nVehicleNum", nCharNum);*/

    if (!AdoGame.Execute4Cmd("dbo.VehicleInfoSelect", adCmdStoredProc))	
        return sc::db::DB_ERROR;

    if (AdoGame.GetEOF())
        return sc::db::DB_OK;    
    do
    {
        int nOwnerNum = 0;
        int nVehicleCardMID = 0;
        int nVehicleCardSID = 0;
        int nType = 0;
        int nFull = 0;
        int nBooster = 0;
		int nMigrationState = 1;

        AdoGame.GetCollect("VehicleChaNum", nOwnerNum);
        AdoGame.GetCollect("VehicleCardMID", nVehicleCardMID);
        AdoGame.GetCollect("VehicleCardSID", nVehicleCardSID);
        AdoGame.GetCollect("VehicleType", nType);
        AdoGame.GetCollect("VehicleBattery", nFull);
        AdoGame.GetCollect("VehicleBooster", nBooster);

        //std::vector<BYTE> vPutOnItems;
        //AdoGame.GetChunk("VehiclePutOnItems", vPutOnItems);
        
        std::vector<BYTE> vPartsColor;
        AdoGame.GetChunk("VehiclePartsColor", vPartsColor);

		AdoGame.GetCollect( "MigrationState", nMigrationState );

        pVehicle->m_OwnerDbNum			= (DWORD) nOwnerNum;
        pVehicle->m_sVehicleID.wMainID	= (WORD) nVehicleCardMID;
        pVehicle->m_sVehicleID.wSubID	= (WORD) nVehicleCardSID;
        pVehicle->m_emTYPE         = VEHICLE_TYPE(nType);			
        pVehicle->m_nFull		   = nFull;
        pVehicle->m_dwBoosterID	   = nBooster;

        //se::ByteStream PutOnItemsStream(vPutOnItems);
        //if (!PutOnItemsStream.IsEmpty())
		//{
		//	if ( 0 == nMigrationState )
		//	{
		//		SETPUTONITEMS_BYBUF_FOR_MIGRATION( pVehicle->m_PutOnItems, PutOnItemsStream );
		//	}
		//	else
		//	{
		//		SETPUTONITEMS_BYBUF(pVehicle->m_PutOnItems, PutOnItemsStream);
		//	}
		//}
        //se::ByteStream PutOnItemsStream(vPutOnItems);
        //if (!PutOnItemsStream.IsEmpty())
        //    SETPUTONITEMS_BYBUF(pVehicle->m_PutOnItems, PutOnItemsStream);

        se::ByteStream PartsColorStream(vPartsColor);
        if (!PartsColorStream.IsEmpty())
            SETITEMSCOLOR_BYBUF(pVehicle->m_sColor, PartsColorStream);        
    } while (AdoGame.Next());    
    
	if ( GetVehiclePutonItems( pVehicle, nVehicleNum, nCharNum ) != sc::db::DB_OK )
		return sc::db::DB_ERROR;

    return sc::db::DB_OK;
}

//! 탈것의 인벤토리 정보를 가져온다
/*
int AdoManager::GetVehicleInven(GLVEHICLE* pVehicle, int nVehicleNum, int nCharNum)
{
    sc::writeLogError("AdoManager::GetVehicleInven. Do not call this function");
    return sc::db::DB_ERROR;
}
*/
//! 탈것 인벤토리 업데이트
int AdoManager::SetVehicleInven(int nVehicle, int nCharNum, se::ByteStream& ByteStream)
{
    if (nVehicle <= 0)
    {
        sc::writeLogError("AdoManager::SetVehicleInven nVehicle <= 0");
        return sc::db::DB_ERROR;
    }

    LPBYTE pBuffer = NULL;
    DWORD dwSize = 0;
    int nResult = 0;
    ByteStream.GetBuffer(pBuffer, dwSize);
    if (pBuffer)
    {
        sc::db::AdoExt AdoGame(m_GameDBConnString);
        APPEND_IPARAM_INT(AdoGame, "@nVehicleChaNum", nCharNum);
        APPEND_IPARAM_INT(AdoGame, "@nVehicleNum", nVehicle);

        _variant_t varBlob;
        if (dwSize > 0)
        {
            if (!AdoGame.MakeChunk(varBlob, ByteStream, dwSize))
            {
                sc::writeLogError(
                    sc::string::format(
                        "SaveVehicleInven Makechunk ChaNum %1% Vehicle %2% Size %3%",
                        nCharNum,
                        nVehicle,
                        dwSize));
            }
        }
        else
        {
            varBlob = VT_NULL;
            dwSize = sizeof(_variant_t);

            sc::writeLogError(
                sc::string::format(
                    "SaveVehicleInven dwSize 0 ChaNum %1% Vehicle %2%",
                    nCharNum,
                    nVehicle));
        }

        APPEND_IPARAM_IMAGE(AdoGame, "@VehiclePutOnItems", varBlob, dwSize);

        if (AdoGame.ExecuteStoredProcedure("dbo.VehicleInfoVehiclePutOnItemsUpdate"))
            return sc::db::DB_OK;
        else
            return sc::db::DB_ERROR;
    }
    else
    {
        return sc::db::DB_ERROR;
    }
}

//! 탈것의 컬러 정보를 가져온다
/*
int AdoManager::GetVehicleColor(GLVEHICLE* pVehicle, int nVehicleNum, int nCharNum)
{
    sc::writeLogError("AdoManager::GetVehicleColor. Do not call this function");
    return sc::db::DB_ERROR;
}
*/
//! 탈것 컬러 업데이트
int AdoManager::SetVehicleColor(int nVehicle, int nCharNum, se::ByteStream &ByteStream)
{
    if (nVehicle <= 0)
    {
        sc::writeLogError("AdoManager::SetVehicleColor nVehicle <= 0");
        return sc::db::DB_ERROR;
    }

    LPBYTE pBuffer = NULL;
    DWORD dwSize = 0;    
    ByteStream.GetBuffer(pBuffer, dwSize);
    if (pBuffer)
    {        
        sc::db::AdoExt AdoGame(m_GameDBConnString);
        APPEND_IPARAM_INT(AdoGame, "@nVehicleChaNum", nCharNum);
        APPEND_IPARAM_INT(AdoGame, "@nVehicleNum", nVehicle);

        _variant_t varBlob;

        if (dwSize > 0)
        {
            if (!AdoGame.MakeChunk(varBlob, ByteStream, dwSize))
            {
                sc::writeLogError(
                    sc::string::format(
                        "SaveVehicleColor Makechunk ChaNum %1% Vehicle %2% Size %3%",
                        nCharNum,
                        nVehicle,
                        dwSize));
            }
        }
        else
        {
            varBlob = VT_NULL;
            dwSize = sizeof(_variant_t);

            sc::writeLogError(
                sc::string::format(
                    "SaveVehicleColor dwSize 0 ChaNum %1% Vehicle %2%",
                    nCharNum,
                    nVehicle));
        }

        APPEND_IPARAM_IMAGE(AdoGame, "@VehiclePartsColor", varBlob, dwSize);

        if (AdoGame.ExecuteStoredProcedure("dbo.VehicleInfoVehiclePartsColorUpdate"))
            return sc::db::DB_OK;
        else
            return sc::db::DB_ERROR;
    }
    else
    {
        return sc::db::DB_ERROR;
    }
}

//! 탈것의 부스터 스킬 업데이트
//! \param m_nVehicleBooster 배운 부스터 종류
int AdoManager::SetVehicleBooster(int nVehicleNum, int nCharNum, DWORD dwVehicleBooster)
{
    if (nVehicleNum <= 0 || dwVehicleBooster <= 0)
    {
        sc::writeLogError("AdoManager::SetVehicleBooster nVehicleNum <= 0 || dwVehicleBooster <= 0");
        return sc::db::DB_ERROR;
    }

    sc::db::AdoExt AdoGame(m_GameDBConnString);
    APPEND_RPARAM_INT(AdoGame);

    APPEND_IPARAM_INT(AdoGame, "@nVehicleNum", nVehicleNum);
    APPEND_IPARAM_INT(AdoGame, "@nVehicleChaNum", nCharNum);
    APPEND_IPARAM_INT(AdoGame, "@nVehicleBooster", dwVehicleBooster);
    
    APPEND_OPARAM_INT(AdoGame, "@nReturn");

    return ExecuteStoredProcedureIntReturn("dbo.sp_UpdateVehicleBooster", AdoGame, "@nReturn");
}

int AdoManager::GetVehiclePutonItems( GLVEHICLE* pVehicle, int nVehicleNum, int nCharNum )
{
	if ( !pVehicle )
		return sc::db::DB_ERROR;

	sc::db::AdoExt Ado( m_GameDBConnString );
	APPEND_IPARAM_INT( Ado, "@VehicleNum", nVehicleNum );
	if ( !Ado.Execute4Cmd( "dbo.sp_ItemGetItemVehiclePuton", adCmdStoredProc ) )
		return sc::db::DB_ERROR;

	if ( Ado.GetEOF() )
	{
		return sc::db::DB_OK;
	}

	do
	{
		std::string strGuid;
		SNATIVEID sItemID( false );
		_variant_t CreateDate, CostumeExpireDate, BasicPeriodEx, DisguisePeriodEx;
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

		pVehicle->SETPUTONITEM( static_cast< VEHICLE_ACCESSORYTYPE >( sItem.wPosX ), sItem.sItemCustom );

	} while ( Ado.Next() );

	return sc::db::DB_OK;
}

int AdoManager::GetAllVehiclePutonItems( DWORD ChaDbNum, std::vector< GLVEHICLE >& vecVehicles )
{
	sc::db::AdoExt AdoGame( m_GameDBConnString );
	APPEND_IPARAM_INT( AdoGame, "@VehicleChaNum", ChaDbNum );

	if ( !AdoGame.ExecuteStoredProcedure( "dbo.sp_GetAllVehiclePutOnItems" ) )
		return sc::db::DB_ERROR;

	if ( AdoGame.GetEOF() )
		return sc::db::DB_OK;

	do
	{
		GLVEHICLE sVehicle;
		AdoGame.GetCollect( "VehicleNum", sVehicle.m_DbNum );

		int nMigrationState = 1;
		AdoGame.GetCollect( "MigrationState", nMigrationState );

		std::vector< BYTE > vBuffer;
		AdoGame.GetChunk( "VehiclePutOnItems", vBuffer );

		// Put on items
		se::ByteStream VehicleItems( vBuffer );
		if ( !VehicleItems.IsEmpty() )
		{
			if ( 0 == nMigrationState )
			{
				SETPUTONITEMS_BYBUF_FOR_MIGRATION( sVehicle.m_PutOnItems, VehicleItems );
			}
			else
			{
				SETPUTONITEMS_BYBUF( sVehicle.m_PutOnItems, VehicleItems );
			}
		}

		vecVehicles.push_back( sVehicle );

	} while (AdoGame.Next());

	return sc::db::DB_OK;
}

int AdoManager::SetVehicleMigrationState( int ChaNum, int VehicleNum, int MigrationState )
{
	sc::db::AdoExt ado( m_GameDBConnString );
	APPEND_IPARAM_INT( ado, "@VehicleChaNum", ChaNum );
	APPEND_IPARAM_INT( ado, "@VehicleNum", VehicleNum );
	APPEND_IPARAM_INT( ado, "@MigrationState", MigrationState );

	if ( ExecuteStoredProcedure( "dbo.UpdateVehicleMigrationState", ado ) != sc::db::DB_OK )
	{
		sc::writeLogError( std::string( "AdoManager::SetVehicleMigrationState() - ExecuteStoredProcedure failure." ) );
		return sc::db::DB_ERROR;
	}

	return sc::db::DB_OK;	
}

} // namespace db
