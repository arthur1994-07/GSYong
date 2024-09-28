#include "../../pch.h"
#include "../../../SigmaCore/String/StringUtil.h"
#include "../../../SigmaCore/String/StringFormat.h"
#include "../../../SigmaCore/Log/LogMan.h"

#include "../DBAction/DbAction.h"
#include "./OdbcStmt.h"
#include "./s_COdbcManager.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

int COdbcManager::CreateVehicle(
    int nCharNum,
    const TCHAR* szVehicleName,
    int nVehicleType,
    int nVehicleCardMID,
    int nVehicleCardSID)
{
    sc::writeLogError("COdbcManager::CreateVehicle. Use ADO");
    return sc::db::DB_ERROR;
/*
	if (nCharNum <=0 || !szVehicleName)
	{
		Print(_T("COdbcManager::CreateVehicle"));
		return sc::db::DB_ERROR;
	}

    std::string VehicleName(szVehicleName);
    sc::string::replaceForDb(VehicleName);
    std::string Query(
        sc::string::format(
            "{call sp_InsertVehicle('%1%',%2%,%3%,%4%,%5%,?)}",
		    VehicleName,
		    nCharNum,
		    nVehicleType,
		    nVehicleCardMID,
		    nVehicleCardSID));
	return m_pGameDB->ExecuteSpInt(Query);
*/
}

int COdbcManager::SetVehicleBattery(int nVehicleNum, int nCharNum, int nVehicleBttery)
{
    sc::writeLogError("COdbcManager::SetVehicleBattery. Use ADO");
    return sc::db::DB_ERROR;
/*
	if (nVehicleNum <= 0 ||	nVehicleBttery < 0)
	{
		Print("COdbcManager::SetVehicleBttery");
		return sc::db::DB_ERROR;
	}

    std::string Query(
        sc::string::format(
            "{call sp_UpdateVehicleBattery(%d,%d,%d,?)}",
            nVehicleNum,
            nCharNum,
            nVehicleBttery));
	return m_pGameDB->ExecuteSpInt(Query);
*/
}

int COdbcManager::GetVehicleBattery(int nVehicleNum, int nCharNum)
{
    sc::writeLogError("COdbcManager::GetVehicleBattery. Use ADO");
    return sc::db::DB_ERROR;
/*
    std::string Query(
        sc::string::format(
            "{call sp_GetVehicleBattery(%d,%d,?)}",
            nVehicleNum, nCharNum));
	return m_pGameDB->ExecuteSpInt(Query);
*/
}

int COdbcManager::DeleteVehicle(int nVehicle, int nCharNum)
{
    sc::writeLogError("COdbcManager:: DeleteVehicle. Use ADO");
    return sc::db::DB_ERROR;
/*
	if (nVehicle <= 0)
	{
		Print(_T("COdbcManager::DeleteVehicle"));
		return sc::db::DB_ERROR;
	}	

    std::string Query(
        sc::string::format(
	        "{call sp_DeleteVehicle(%d, %d,?)}",
            nVehicle, nCharNum));
	return m_pGameDB->ExecuteSpInt(Query);
*/
}

int COdbcManager::RebuyVehicle(int nVehicle, int nCharNum)
{
    sc::writeLogError("COdbcManager:: RebuyVehicle. Use ADO");
    return sc::db::DB_ERROR;
}

int COdbcManager::GetVehicle(
    GLVEHICLE* pVehicle,
    int nVehicleNum,
    int nCharNum)
{
    sc::writeLogError("COdbcManager::GetVehicle. Use ADO");
    return sc::db::DB_ERROR;
/*
	if (!pVehicle)
	{
		Print("COdbcManager::GetVehicle pVehicle==NULL");
		return sc::db::DB_ERROR;
	}

    std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pGameDB->GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;

	// 탈것의 기본정보를 가져온다.
    std::string Query(
        sc::string::format(
	        "Exec sp_SelectVehicle %d,%d",
            nVehicleNum, nCharNum));
	
	int nRowCount = 0;
	SQLRETURN sReturn = ::SQLExecDirect(spConn->hStmt, (SQLCHAR*) Query.c_str(), SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
		Print(Query.c_str());
		Print(GetErrorString(spConn->hStmt));
		//m_pGameDB->FreeConnection(spConn);
		return sc::db::DB_ERROR;
	}

	SQLINTEGER nOwnerNum = 0, cbOwnerNum = SQL_NTS;	
	SQLINTEGER nVehicleCardMID = 0, cbVehicleCardMID = SQL_NTS;
	SQLINTEGER nVehicleCardSID = 0, cbVehicleCardSID = SQL_NTS;
	SQLINTEGER nType = 0, cbType = SQL_NTS;
	SQLINTEGER nFull = 0, cbFull = SQL_NTS;
	SQLINTEGER nBooster = 0, cbBooster = SQL_NTS;

	while (true)
	{
		sReturn = ::SQLFetch(spConn->hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			Print(Query.c_str());
			Print(GetErrorString(spConn->hStmt));
			//m_pGameDB->FreeConnection(spConn);
			return sc::db::DB_ERROR;
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{			
			::SQLGetData(spConn->hStmt, 1, SQL_C_LONG, &nOwnerNum, 0, &cbOwnerNum);
			::SQLGetData(spConn->hStmt, 2, SQL_C_LONG, &nVehicleCardMID, 0, &cbVehicleCardMID);
			::SQLGetData(spConn->hStmt, 3, SQL_C_LONG, &nVehicleCardSID, 0, &cbVehicleCardSID);
			::SQLGetData(spConn->hStmt, 4, SQL_C_LONG, &nType, 0, &cbType);
			::SQLGetData(spConn->hStmt, 5, SQL_C_LONG, &nFull, 0, &cbFull);
			::SQLGetData(spConn->hStmt, 6, SQL_C_LONG, &nBooster, 0, &cbBooster);

			pVehicle->m_OwnerDbNum				= (DWORD) nOwnerNum;
			pVehicle->m_sVehicleID.wMainID	= (WORD) nVehicleCardMID;
			pVehicle->m_sVehicleID.wSubID	= (WORD) nVehicleCardSID;
			pVehicle->m_emTYPE         = VEHICLE_TYPE(nType);			
			pVehicle->m_nFull		   = nFull;
			pVehicle->m_dwBoosterID	   = nBooster;
			nRowCount++;
		}
		else
		{
			break;
		}		
	}

	//m_pGameDB->FreeConnection(spConn);	

		// Row Count 가 1 이 아니면 에러이다.
	if (nRowCount == 1)	
	{
		nRowCount = 0;
	}
	else
	{
		sc::writeLogError(sc::string::format("DB:GetVehicle RowCount = 0, VehicleID = %d, CharID = %d",
			nVehicleNum, nCharNum  ));
		
		return sc::db::DB_ROWCNT_ERROR;
	}

	// 탈것 의 인벤토리를 가져온다.
	if (GetVehicleInven( pVehicle, nVehicleNum, nCharNum ) == sc::db::DB_ERROR)
        return sc::db::DB_ERROR;

	if (GetVehicleColor( pVehicle, nVehicleNum, nCharNum ) == sc::db::DB_ERROR)
        return sc::db::DB_ERROR;

	return sc::db::DB_OK;
*/
}
/*
int COdbcManager::GetVehicleInven( 
    GLVEHICLE* pVehicle,
    int nVehicleNum,
    int nCharNum)
{
	if (pVehicle == NULL)
	{
		Print(_T("COdbcManager::GetVehicleInven pVehicle==NULL"));
		return sc::db::DB_ERROR;
	}

	se::ByteStream ByteStream;
	int nReturn = 0;

	//  VehicleOnItem
	nReturn = m_pGameDB->ReadImageVehicle("VehicleInfo.VehiclePutOnItems", nVehicleNum, nCharNum,  ByteStream);

	if (nReturn == sc::db::DB_ERROR)
	{
		return sc::db::DB_ERROR;
	}
	else 
	{
		SETPUTONITEMS_BYBUF( pVehicle->m_PutOnItems, ByteStream );
		return sc::db::DB_OK;
	}
}
*/
int COdbcManager::SetVehicleInven(
    int nVehicle,
    int nCharNum,
    se::ByteStream &ByteStream)
{
    sc::writeLogError("COdbcManager::SetVehicleInven. Use ADO");
    return sc::db::DB_ERROR;
/*
    if (nVehicle <= 0)
	{
		Print(_T("COdbcManager::SetVehicleInven"));
		return sc::db::DB_ERROR;
	}

	LPBYTE pBuffer = NULL;
	DWORD dwSize = 0;
	int nResult = 0;
	ByteStream.GetBuffer( pBuffer, dwSize );
	if (pBuffer != NULL)
	{
		std::strstream strnVehicleInven;
		strnVehicleInven << _T("UPDATE VehicleInfo SET VehicleInfo.VehiclePutOnItems=?");
		strnVehicleInven <<" WHERE (VehicleNum=" <<  nVehicle << ") And( VehicleChaNum="<< nCharNum << ")";
		strnVehicleInven << std::ends;
		nResult = m_pGameDB->WriteImage( strnVehicleInven, (DWORD) nVehicle, (BYTE *) pBuffer, dwSize );

		strnVehicleInven.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
		pBuffer = NULL;
		return nResult;
	}
	else
	{
		return sc::db::DB_ERROR;
	}
*/
}
/*
int COdbcManager::GetVehicleColor( 
    GLVEHICLE* pVehicle,
    int nVehicleNum,
    int nCharNum)
{
	if (pVehicle == NULL)
	{
		Print(_T("COdbcManager::GetVehicleColor pVehicle==NULL"));
		return sc::db::DB_ERROR;
	}

	se::ByteStream ByteStream;
	int nReturn = 0;

	//  VehicleOnItem
	nReturn = m_pGameDB->ReadImageVehicle("VehicleInfo.VehiclePartsColor", nVehicleNum, nCharNum,  ByteStream);

	if (nReturn == sc::db::DB_ERROR)
	{
		return sc::db::DB_ERROR;
	}
	else 
	{
		SETITEMSCOLOR_BYBUF( pVehicle->m_sColor, ByteStream );
		return sc::db::DB_OK;
	}
}
*/
int COdbcManager::SetVehicleColor(
    int nVehicle,
    int nCharNum,
    se::ByteStream &ByteStream )
{
    sc::writeLogError("COdbcManager::SetVehicleColor. Use ADO");
    return sc::db::DB_ERROR;
/*
	if (nVehicle <= 0)
	{
		Print(_T("COdbcManager::SetVehicleColor"));
		return sc::db::DB_ERROR;
	}

	LPBYTE pBuffer = NULL;
	DWORD dwSize = 0;
	int nResult = 0;
	ByteStream.GetBuffer( pBuffer, dwSize );
	if (pBuffer != NULL)
	{
		std::strstream strnVehicleColor;
		strnVehicleColor << _T("UPDATE VehicleInfo SET VehicleInfo.VehiclePartsColor=?");
		strnVehicleColor <<" WHERE (VehicleNum=" <<  nVehicle << ") And( VehicleChaNum="<< nCharNum << ")";
		strnVehicleColor << std::ends;
		nResult = m_pGameDB->WriteImage( strnVehicleColor, (DWORD) nVehicle, (BYTE *) pBuffer, dwSize );

		strnVehicleColor.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
		pBuffer = NULL;
		return nResult;
	}
	else
	{
		return sc::db::DB_ERROR;
	}
*/
}

int COdbcManager::SetVehicleBooster(
    int nVehicleNum,
    int nCharNum,
    DWORD dwVehicleBooster)
{
    sc::writeLogError("COdbcManager::SetVehicleBooster. Use ADO");
    return sc::db::DB_ERROR;
/*
	if (nVehicleNum <= 0 ||	dwVehicleBooster <= 0)
	{
		Print("COdbcManager::SetVehicleBooster");
		return sc::db::DB_ERROR;
	}

    std::string Query(
        sc::string::format(
	        "{call sp_UpdateVehicleBooster(%d,%d,%d,?)}",
            nVehicleNum,
            nCharNum,
            dwVehicleBooster));
	return m_pGameDB->ExecuteSpInt(Query);
*/
}
