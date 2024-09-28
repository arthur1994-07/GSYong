//
//mjeon.activities
//
#include "../../pch.h"
#include "./OdbcStmt.h"
#include "s_COdbcManager.h"

#include "../../../RanLogic/Activity/ActivityBase.h"
#include "../../../RanLogic/Activity/TitleManager.h"

#include "../../../SigmaCore/Log/LogMan.h"


// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


//
//called directly to load Closed-Activities into ActivityBaseData by GaeaServer::Create() but not by DbAction.
//
int COdbcManager::ActivityGetClosedActivityID(OUT ActivityBaseData &abd)
{
    sc::writeLogError("COdbcManager::ActivityGetClosedActivityID");
    return sc::db::DB_ERROR;
/*
	SQLRETURN sReturn = 0;	
	UINT	nRet = 0;

    std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pGameDB->GetConnection();

	if (!spConn)
	{
		sc::writeLogError(std::string("ActivityLoad(): GetConnection() failure."));
		
		return sc::db::DB_ERROR;
	}
		
	CString csQuery;

	csQuery.Format(_T( "SELECT ActivityID FROM dbo.ActivityClosed" ));

	sReturn = ::SQLExecDirect(spConn->hStmt,	(SQLCHAR*)csQuery.GetString(), SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
		Print(csQuery.GetString());		
		Print(GetErrorString(spConn->hStmt));
		//m_pGameDB->FreeConnection(spConn);
		return sc::db::DB_ERROR;
	}
	

	SQLINTEGER	iActivityID		= 0;	SQLINTEGER	cbActivityID	= SQL_NTS;


	while (true)
	{
		sReturn = ::SQLFetch(spConn->hStmt);

		if (sReturn == SQL_ERROR ) //|| sReturn == SQL_SUCCESS_WITH_INFO)
		{
			Print(csQuery.GetString());		
			Print(GetErrorString(spConn->hStmt));
			//m_pGameDB->FreeConnection(spConn);
			return sc::db::DB_ERROR;
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{	
			// Bind data

			::SQLGetData(spConn->hStmt,  1, SQL_C_LONG,	&iActivityID,	0, &cbActivityID);

			abd.CloseActivity(iActivityID);
		}
		else
		{
			break;
		}
	}

	//m_pGameDB->FreeConnection(spConn);
	return sc::db::DB_OK;
*/
}



int COdbcManager::ActivityLoad(OUT MMACTIVITIES &mmActivities, UINT nChaNum)
{
    sc::writeLogError("COdbcManager::ActivityLoad. Use ADO");
    return sc::db::DB_ERROR;
/*
	SQLRETURN sReturn = 0;	
	UINT	nRet = 0;

		
	CString csQuery;

	CTime ctCurrentTime = CTime::GetCurrentTime().GetTime();	
	CString csCurrentTime = ctCurrentTime.Format("%Y-%m-%d %H:%M:%S");



	//
	//1. Sync the status of activities that belong to this character.
	//  in case of closed activity,
	//		INPROGRESS or ORDERING -------------------> CLOSED
	//  in case of that this character is a ActivityCloser,
	//		INPROGRESS or ORDERING -------------------> COMPLETED
	//
	csQuery.Format(_T( "{call sp_ActivitySyncStatus(%d, '%s',?)}" )
					,nChaNum, csCurrentTime.GetString());


	nRet = m_pGameDB->ExecuteSpInt(csQuery.GetString());

	if (nRet != sc::db::DB_OK)
	{
		//
		//just Warning. NOT an Error.
		//
		sc::writeLogWarn(std::string("ActivityLoad(): Sync failure."));
	}


	std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pGameDB->GetConnection();
	if (!spConn)
	{
		sc::writeLogError(std::string("ActivityLoad(): GetConnection() failure."));
		
		return sc::db::DB_ERROR;
	}
	
	//
    // Load Activities for the character
	//
	csQuery.Format(_T(
			"SELECT ActivityID, Status, Value FROM dbo.ActivityInfo WHERE ChaNum = %d"
		), nChaNum);


	sReturn = ::SQLExecDirect(spConn->hStmt,	(SQLCHAR*)csQuery.GetString(), SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
		Print(csQuery.GetString());		
		Print(GetErrorString(spConn->hStmt));
		//m_pGameDB->FreeConnection(spConn);
		return sc::db::DB_ERROR;
	}


	SQLINTEGER	iActivityID = 0;	SQLINTEGER	cbActivityID= SQL_NTS;
	SQLINTEGER	iStatus		= 0;	SQLINTEGER	cbStatus	= SQL_NTS;
	SQLINTEGER	iValue		= 0;	SQLINTEGER	cbValue		= SQL_NTS;

	
	ITER_MMACTIVITIES iter;
	Activity *pactv = NULL;	


	while (true)
	{
		sReturn = ::SQLFetch(spConn->hStmt);

		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			Print(csQuery.GetString());		
			Print(GetErrorString(spConn->hStmt));
			//m_pGameDB->FreeConnection(spConn);
			return sc::db::DB_ERROR;
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			::SQLGetData(spConn->hStmt,  1, SQL_C_LONG,	&iActivityID,	0, &cbActivityID);
			::SQLGetData(spConn->hStmt,  2, SQL_C_LONG,	&iStatus,		0, &cbStatus);
			::SQLGetData(spConn->hStmt,  3, SQL_C_LONG,	&iValue,		0, &cbValue);
			
			
			WORD wMID = (WORD)(0xffff & (iActivityID));

			//
			//if this iteration takes high load, 
			// then let's make and use a separate map<ActivityID, Activity *> or map<ActivityID, iterator> based on m_mmActivities.
			//
			for (iter = mmActivities.lower_bound(wMID); iter != mmActivities.upper_bound(wMID); ++iter)
			{
				pactv = &(iter->second);
				
				if ( pactv  && (pactv->m_id.dwID == iActivityID) )
				{
					//					
					// FALSE: the activity should not change the updated flag.
					//
					pactv->SetValue(iValue, FALSE);
					pactv->SetStatus((ENUM_ACTIVITY_STATUS)iStatus, FALSE);
					pactv->SetSyncFlag();

					continue;
				}
			}			
		}
		else
		{
			break;
		}
	}

	//m_pGameDB->FreeConnection(spConn);
	return sc::db::DB_OK;
*/
}


int COdbcManager::ActivitySave(MMACTIVITIES &mmActivities, UINT nChaNum, UINT nChaLevel)
{
    sc::writeLogError("COdbcManager::ActivitySave. Use ADO");
    return sc::db::DB_ERROR;
/*
	LONGLONG	llSeq = 0;
	UINT		nRowCount = 0;
	

	CString csQuery;

	if (mmActivities.empty())
	{
		return nRowCount;
	}
	
	
	CTime ctCurrentTime = CTime::GetCurrentTime().GetTime();	
	CString csCurrentTime = ctCurrentTime.Format("%Y-%m-%d %H:%M:%S");
	

	//
    // Save Activities for the character into DB.
	//
	ITER_MMACTIVITIES iter;
	Activity *pa = NULL;
	ENUM_ACTIVITY_STATUS stats;

	for (iter = mmActivities.begin(); iter != mmActivities.end(); ++iter)
	{
		pa =&(iter->second);

		if (pa && pa->IsUpdated())
		{
			stats = pa->GetStatus();
			if (stats == ACTIVITY_STATUS_COMPLETED)
			{
				csQuery.Format(_T( "{call sp_ActivitySave(%d, %d, %d, %d, %d, '%s', '%s', ?)}" )
				,nChaNum, pa->m_id.dwID, (UINT)pa->GetStatus(), pa->GetValue()
				,nChaLevel, csCurrentTime.GetString(), pa->GetCompleteDate().c_str());
			}
			else
			{
				csQuery.Format(_T( "{call sp_ActivitySave(%d, %d, %d, %d, %d, '%s', NULL, ?)}" )
				,nChaNum, pa->m_id.dwID, (UINT)pa->GetStatus(), pa->GetValue()
				,nChaLevel, csCurrentTime.GetString());
			}

			llSeq = m_pGameDB->ExecuteSpLONGLONG(csQuery.GetString());


			if (llSeq != sc::db::DB_ERROR)
			{
				nRowCount++;
			}
		}
	}
	
	//
	//now we can compare the nRowCount with mmActivities.count() in the caller.
	//
	return nRowCount;
*/
}


int COdbcManager::ActivityOrdering(Activity &actv, UINT nChaNum, UINT nChaLevel)
{
    sc::writeLogError("COdbcManager::ActivityOrdering. Use ADO");
    return sc::db::DB_ERROR;
/*
	INT	nRet = 5;
	
	CString csQuery;
		
	CTime ctCurrentTime = CTime::GetCurrentTime().GetTime();	
	CString csCurrentTime = ctCurrentTime.Format("%Y-%m-%d %H:%M:%S");
	

	csQuery.Format(_T( "{call sp_ActivityByOrder(%d, %d, %d, %d, %d, '%s',?)}" )
				,nChaNum, actv.m_id.dwID, actv.GetValue(), nChaLevel, actv.m_nByOrder, csCurrentTime.GetString());

	
	nRet = m_pGameDB->ExecuteSpInt(csQuery.GetString());

	// nRet is one of the ACTIVITY_STATUS_COMPLETED, ACTIVITY_STATUS_CLOSED, ACTIVITY_STATUS_ORDERING
	// in case of ACTIVITY_STATUS_ORDERING, it's an weird case and MUST be checked.	
    if (nRet == sc::db::DB_ERROR)
	{
		sc::writeLogError(std::string("ActivityOrdering(): StoredProcedure returned DB_ERROR."));
	}
	else if (nRet == ACTIVITY_STATUS_COMPLETED || nRet == ACTIVITY_CLOSER)
	{
		actv.SetStatus(ACTIVITY_STATUS_COMPLETED);
	}
	else if (nRet == ACTIVITY_STATUS_CLOSED)
	{
		actv.SetStatus(ACTIVITY_STATUS_CLOSED);
	}
	else	//ACTIVITY_STATUS_ORDERING
	{
		//
		//MUST NOT come into here!!
		//
		sc::writeLogError(std::string("ActivityOrdering(): weird case of ACTIVITY_STATUS_ORDERING. Check it out."));
	}

	return nRet;
*/
}


int COdbcManager::ActivityGetPoint(OUT LONGLONG *pllRewardPoint, UINT nChaNum)
{
    sc::writeLogError("COdbcManager::ActivityGetPoint. Use ADO");
    return sc::db::DB_ERROR;
/*
	SQLRETURN sReturn = 0;	
	UINT	nRet = 0;
		
	CString csQuery;

	std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pGameDB->GetConnection();
	if (!spConn)
	{
		sc::writeLogError(std::string("ActivityGetPoint(): GetConnection() failure."));
		
		return sc::db::DB_ERROR;
	}
	
	
	csQuery.Format(_T(
			"SELECT PointHigh, PointMedium, PointLow FROM dbo.ActivityRewardPoint WHERE ChaNum = %d"
		), nChaNum);


	sReturn = ::SQLExecDirect(spConn->hStmt,	(SQLCHAR*)csQuery.GetString(), SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{	
		Print(csQuery.GetString());		
		Print(GetErrorString(spConn->hStmt));
		//m_pGameDB->FreeConnection(spConn);
		return sc::db::DB_ERROR;
	}
	

	SQLBIGINT	llPointHigh		= 0;	SQLINTEGER	cbPointHigh		= SQL_NTS;
	SQLBIGINT	llPointMedium	= 0;	SQLINTEGER	cbPointMedium	= SQL_NTS;
	SQLBIGINT	llPointLow		= 0;	SQLINTEGER	cbPointLow		= SQL_NTS;


	sReturn = ::SQLFetch(spConn->hStmt);

	if (sReturn == SQL_ERROR ) //|| sReturn == SQL_SUCCESS_WITH_INFO)
	{
		Print(csQuery.GetString());		
		Print(GetErrorString(spConn->hStmt));
		//m_pGameDB->FreeConnection(spConn);
		return sc::db::DB_ERROR;
	}

	if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
	{
		::SQLGetData(spConn->hStmt,  1, SQL_C_SBIGINT,	&llPointHigh,	0, &cbPointHigh);
		::SQLGetData(spConn->hStmt,  2, SQL_C_SBIGINT,	&llPointMedium,	0, &cbPointMedium);
		::SQLGetData(spConn->hStmt,  3, SQL_C_SBIGINT,	&llPointLow,	0, &cbPointLow);
	}
	else
	{
		if (sReturn == SQL_NO_DATA)
		{
			sc::writeLogWarn(std::string("ActivityGetPoint(): No Data!"));
		}
		else
		{
			sc::writeLogError(std::string("ActivityGetPoint(): SQLFetch() failure."));
		}
	}

	pllRewardPoint[ACTIVITY_CLASS_HIGH]		= llPointHigh;
	pllRewardPoint[ACTIVITY_CLASS_MEDIUM]	= llPointMedium;
	pllRewardPoint[ACTIVITY_CLASS_LOW]		= llPointLow;

	//m_pGameDB->FreeConnection(spConn);
	return sc::db::DB_OK;
*/
}

int COdbcManager::ActivityLoadBadge(UINT nChaNum, TitleManager &TitleMan)
{
    sc::writeLogError("COdbcManager::ActivityLoadBadge. Use ADO");
    return sc::db::DB_ERROR;
/*
	SQLRETURN sReturn	= 0;	
	UINT	cntRow		= 0;
	UINT	nRet		= 0;
	BOOL	bInserted	= FALSE;

		
	CString csQuery;


	std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pGameDB->GetConnection();
	if (!spConn)
	{
		sc::writeLogError(std::string("ActivityLoad(): GetConnection() failure."));
		
		return sc::db::DB_ERROR;
	}
	
	//
	// Load XML-IDs of badge, but not real badge string.
	//
	csQuery.Format(_T(
			"SELECT Title, TitleClass, Selected FROM dbo.ActivityRewardBadge WHERE ChaNum = %d"
		), nChaNum);


	sReturn = ::SQLExecDirect(spConn->hStmt,	(SQLCHAR*)csQuery.GetString(), SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
		Print(csQuery.GetString());		
		Print(GetErrorString(spConn->hStmt));
		//m_pGameDB->FreeConnection(spConn);
		return sc::db::DB_ERROR;
	}

	SQLCHAR		szTitle[EM_TITLEID_MAXLENGTH]	={0,};	SQLINTEGER	cbTitle		= SQL_NTS;
	SQLINTEGER	nTitleClass						= 0;	SQLINTEGER	cbTitleClass= SQL_NTS;	
	SQLINTEGER	bSelected						= 0;	SQLINTEGER	cbSelected	= SQL_NTS;
	

	while (true)
	{
		sReturn = ::SQLFetch(spConn->hStmt);

		if (sReturn == SQL_ERROR ) //|| sReturn == SQL_SUCCESS_WITH_INFO)
		{
			Print(csQuery.GetString());		
			Print(GetErrorString(spConn->hStmt));
			//m_pGameDB->FreeConnection(spConn);
			return sc::db::DB_ERROR;
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{	
			// Bind data

			::SQLGetData(spConn->hStmt,  1, SQL_C_CHAR,	szTitle,		EM_TITLEID_MAXLENGTH,	&cbTitle);
			::SQLGetData(spConn->hStmt,  2, SQL_C_LONG,	&nTitleClass,	0,						&cbTitleClass);
			::SQLGetData(spConn->hStmt,  3, SQL_C_LONG,	&bSelected,		0,						&cbSelected);

			if (cbTitle != 0 && cbTitle != -1)
			{
				bInserted = TitleMan.Add(std::string((const char *)szTitle), (ENUM_ACTIVITY_CLASS)nTitleClass);

				if (bInserted && bSelected)
					TitleMan.Select();
			}

			cntRow++;
		}
		else
		{
			if (sReturn == SQL_NO_DATA)
		{
				if (cntRow == 0)
					sc::writeLogWarn(std::string("ActivityLoadBadge(): No Data!"));
			}
			else
			{
				sc::writeLogError(std::string("ActivityLoadBadge(): SQLFetch() failure."));
			}

			break;
		}
	}

	//m_pGameDB->FreeConnection(spConn);
	return sc::db::DB_OK;
*/
}


int COdbcManager::ActivitySetPoint(IN LONGLONG *pllRewardPoint, UINT nChaNum)
{
    sc::writeLogError("COdbcManager::ActivitySetPoint. Use ADO");
    return sc::db::DB_ERROR;
/*
	SQLRETURN sReturn = 0;
	UINT	nRet = 0;
		
	CString csQuery;


	CTime ctCurrentTime = CTime::GetCurrentTime().GetTime();	
	CString csCurrentTime = ctCurrentTime.Format("%Y-%m-%d %H:%M:%S");
	

	csQuery.Format(_T( "{call sp_ActivitySetRewardPoint(%d, %I64d, %I64d, %I64d,'%s',?)}" )
					,nChaNum, pllRewardPoint[ACTIVITY_CLASS_HIGH], pllRewardPoint[ACTIVITY_CLASS_MEDIUM], pllRewardPoint[ACTIVITY_CLASS_LOW], csCurrentTime.GetString());
	
	
	nRet = m_pGameDB->ExecuteSpInt(csQuery.GetString());

	if (nRet != sc::db::DB_OK)
	{	
		sc::writeLogError(std::string("ActivitySetPoint(): sp_ActivitySetRewardPoint failure."));

		return sc::db::DB_ERROR;
	}

	return sc::db::DB_OK;
*/
}


int COdbcManager::ActivitySaveBadge(UINT nChaNum, TitleManager &TitleMan)
{
    sc::writeLogError("COdbcManager::ActivitySaveBadge. Use ADO");
    return sc::db::DB_ERROR;
/*
	SQLRETURN sReturn = 0;
	UINT	nRet = 0;
		
	CString csQuery;

	VECTITLE &vecTitleToUpdate = TitleMan.GetDataToUpdate();

	if (vecTitleToUpdate.empty())
	{
		return sc::db::DB_OK;
	}

	std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pGameDB->GetConnection();
	if (!spConn)
	{
		sc::writeLogError(std::string("ActivitySetPoint(): GetConnection() failure."));
		
		return sc::db::DB_ERROR;
	}

	CTime ctCurrentTime = CTime::GetCurrentTime().GetTime();	
	CString csCurrentTime = ctCurrentTime.Format("%Y-%m-%d %H:%M:%S");
	
	ITER_VECTITLE iter;

	for (iter = vecTitleToUpdate.begin(); iter != vecTitleToUpdate.end(); ++iter)
	{
		csQuery.Format(_T(
					"{call sp_ActivitySaveBadge(%d, '%s', %d, '%s',?)}" 
					),nChaNum, iter->strTitle.c_str(), (UINT)iter->nClass, csCurrentTime.GetString());
	
	
		nRet = m_pGameDB->ExecuteSpInt(csQuery.GetString());

		if (nRet != sc::db::DB_OK)
		{	
			sc::writeLogError(std::string("ActivitySaveBadge(): sp_ActivitySaveBadge failure."));

			continue;
			//return sc::db::DB_ERROR;
		}
	}

	csQuery.Format(_T(
				"UPDATE ActivityRewardBadge SET Selected = "\
					"CASE "									\
						"WHEN (Selected = 1) "				\
							"THEN 0 "						\
						"WHEN (Title = '%s') "				\
							"THEN 1 "						\
						"ELSE Selected "					\
					"END "									\
					"WHERE ChaNum = %d"
				), TitleMan.GetCurrentTitle().c_str(), nChaNum);

	sReturn = ::SQLExecDirect(spConn->hStmt,	(SQLCHAR*)csQuery.GetString(), SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
		Print(csQuery.GetString());
		Print(GetErrorString(spConn->hStmt));
		//m_pGameDB->FreeConnection(spConn);		
		return sc::db::DB_ERROR;
	}

	
	//m_pGameDB->FreeConnection(spConn);	
	return sc::db::DB_OK;
*/
}
