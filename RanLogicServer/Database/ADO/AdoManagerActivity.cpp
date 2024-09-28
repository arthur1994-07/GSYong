#include "../../pch.h"
#include "../../../SigmaCore/gassert.h"
#include "../../../RanLogic/Activity/TitleManager.h"
#include "./AdoManager.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace db
{

int AdoManager::ActivityLoad(MMACTIVITIES &mmActivities, UINT nChaNum)
{
    CTime ctCurrentTime = CTime::GetCurrentTime().GetTime();	
    CString csCurrentTime = ctCurrentTime.Format("%Y-%m-%d %H:%M:%S");

    //1. Sync the status of activities that belong to this character.
    //  in case of closed activity,
    //		INPROGRESS or ORDERING -------------------> CLOSED
    //  in case of that this character is a ActivityCloser,
    //		INPROGRESS or ORDERING -------------------> COMPLETED
    sc::db::AdoExt AdoGame1(m_GameDBConnString);
    APPEND_RPARAM_INT(AdoGame1);

    APPEND_IPARAM_INT    (AdoGame1, "@ChaNum", nChaNum);
    APPEND_IPARAM_VARCHAR(AdoGame1, "@LastUpdateDate", csCurrentTime.GetString(), csCurrentTime.GetLength());

    APPEND_OPARAM_INT(AdoGame1, "@nReturn");
    int nRet = ExecuteStoredProcedureIntReturn("dbo.sp_ActivitySyncStatus", AdoGame1, "@nReturn");
    if (nRet != sc::db::DB_OK)
    {
        //just Warning. NOT an Error.
        sc::writeLogWarn(std::string("ActivityLoad(): Sync failure."));
    }

    // Load Activities for the character    
    sc::db::AdoExt AdoGame2(m_GameDBConnString);
    APPEND_IPARAM_INT(AdoGame2, "@ChaNum", nChaNum);
    if (!AdoGame2.ExecuteStoredProcedure("dbo.ActivityInfoSelect"))
        return sc::db::DB_ERROR;

    if (AdoGame2.GetEOF())
        return sc::db::DB_OK;    	

    do
    { 
        int iActivityID = 0;
        int iStatus = 0;
        int iValue = 0;

        AdoGame2.GetCollect("ActivityID", iActivityID);
        AdoGame2.GetCollect("Status",     iStatus);
        AdoGame2.GetCollect("Value",      iValue);

        WORD wMID = (WORD) (0xffff & (iActivityID));

        ITER_MMACTIVITIES iter;
        Activity *pactv = NULL;
        // if this iteration takes high load, 
        // then let's make and use a separate map<ActivityID, Activity *> or map<ActivityID, iterator> based on m_mmActivities.
        for (iter = mmActivities.lower_bound(wMID); iter != mmActivities.upper_bound(wMID); ++iter)
        {
            pactv = &(iter->second);

            if (pactv  && (pactv->m_id.dwID == iActivityID))
            {
                // FALSE: the activity should not change the updated flag.
                pactv->SetValue(iValue, FALSE);
                pactv->SetStatus((ENUM_ACTIVITY_STATUS)iStatus, FALSE);
                pactv->SetSyncFlag();
            }
        }
    } while (AdoGame2.Next());
    
    return sc::db::DB_OK;
}

int AdoManager::ActivitySave(MMACTIVITIES &mmActivities, UINT nChaNum, UINT nChaLevel)
{    
    UINT nRowCount = 0;    
    if (mmActivities.empty())
        return nRowCount;
    
    CTime ctCurrentTime = CTime::GetCurrentTime().GetTime();	
    CString csCurrentTime = ctCurrentTime.Format("%Y-%m-%d %H:%M:%S");


    // Save Activities for the character into DB.
    LONGLONG llSeq = 0;
    CString csQuery;
    ITER_MMACTIVITIES iter;
    Activity *pa = NULL;
    ENUM_ACTIVITY_STATUS stats;

    for (iter = mmActivities.begin(); iter != mmActivities.end(); ++iter)
    {
        pa =&(iter->second);

        if (pa && pa->IsUpdated())
        {            
            _variant_t vartBLOB;
            sc::db::AdoExt AdoGame(m_GameDBConnString);
            APPEND_RPARAM_INT(AdoGame);

            APPEND_IPARAM_INT    (AdoGame, "@ChaNum", nChaNum);
            APPEND_IPARAM_INT    (AdoGame, "@ActivityID", pa->m_id.dwID); 
            APPEND_IPARAM_INT    (AdoGame, "@Status", pa->GetStatus());
            APPEND_IPARAM_INT    (AdoGame, "@Value", pa->GetValue());
            APPEND_IPARAM_INT    (AdoGame, "@ChaLevel", nChaLevel);
            APPEND_IPARAM_VARCHAR(AdoGame, "@LastUpdateDate", csCurrentTime.GetString(), csCurrentTime.GetLength());

            stats = pa->GetStatus();
            if (stats == ACTIVITY_STATUS_COMPLETED)
            {
                APPEND_IPARAM_VARCHAR(AdoGame, "@CompleteDate", pa->GetCompleteDate().c_str(), pa->GetCompleteDate().length());
            }
            else
            {                
                vartBLOB.vt = VT_NULL;
                APPEND_IPARAM_VARCHAR(AdoGame, "@CompleteDate", vartBLOB, sizeof(_variant_t));
            }

            APPEND_OPARAM_BIGINT(AdoGame, "@nReturn");

            llSeq = ExecuteStoredProcedureLonglongReturn("dbo.sp_ActivitySave", AdoGame, "@nReturn");

            if (llSeq != sc::db::DB_ERROR)
                nRowCount++;
        }
    }

    //now we can compare the nRowCount with mmActivities.count() in the caller.
    return nRowCount;
}

int AdoManager::ActivityOrdering(Activity &actv, UINT nChaNum, UINT nChaLevel)
{
    INT	nRet = 5;
	
	CString csQuery;
		
	CTime ctCurrentTime = CTime::GetCurrentTime().GetTime();	
	CString csCurrentTime = ctCurrentTime.Format("%Y-%m-%d %H:%M:%S");
	
    sc::db::AdoExt AdoGame(m_GameDBConnString);
    APPEND_RPARAM_INT(AdoGame);

    APPEND_IPARAM_INT    (AdoGame, "@ChaNum", nChaNum);
    APPEND_IPARAM_INT    (AdoGame, "@ActivityID", actv.m_id.dwID);
    APPEND_IPARAM_INT    (AdoGame, "@Value", actv.GetValue());
    APPEND_IPARAM_INT    (AdoGame, "@ChaLevel", nChaLevel);
    APPEND_IPARAM_INT    (AdoGame, "@Order", actv.m_nByOrder);
    APPEND_IPARAM_VARCHAR(AdoGame, "@LastUpdateDate", csCurrentTime.GetString(), csCurrentTime.GetLength());

    APPEND_OPARAM_INT(AdoGame, "@nReturn");

	nRet = ExecuteStoredProcedureIntReturn("dbo.sp_ActivityByOrder", AdoGame, "@nReturn");
	
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
}

int AdoManager::ActivityGetPoint(OUT LONGLONG *pllRewardPoint, UINT nChaNum)
{
    __int64 llPointHigh = 0;
    __int64 llPointMedium = 0;
    __int64 llPointLow = 0;

    pllRewardPoint[ACTIVITY_CLASS_HIGH]	  = llPointHigh;
    pllRewardPoint[ACTIVITY_CLASS_MEDIUM] = llPointMedium;
    pllRewardPoint[ACTIVITY_CLASS_LOW]	  = llPointLow;

    sc::db::AdoExt AdoGame(m_GameDBConnString);
    APPEND_IPARAM_INT(AdoGame, "@nChaNum", nChaNum);
    
    int nRet = AdoGame.ExecuteStoredProcedure("dbo.ActivityRewardPointSelect");
    if (nRet == sc::db::DB_ERROR)
        return sc::db::DB_ERROR;
    
    if (AdoGame.GetEOF())
        return sc::db::DB_OK;

    do 
    {
        AdoGame.GetCollect("PointHigh",   llPointHigh);
        AdoGame.GetCollect("PointMedium", llPointMedium);
        AdoGame.GetCollect("PointLow",    llPointLow);
    } while (AdoGame.Next());

    pllRewardPoint[ACTIVITY_CLASS_HIGH]	  = llPointHigh;
    pllRewardPoint[ACTIVITY_CLASS_MEDIUM] = llPointMedium;
    pllRewardPoint[ACTIVITY_CLASS_LOW]	  = llPointLow;
    
    return sc::db::DB_OK;
}

int AdoManager::ActivityLoadBadge(UINT nChaNum, TitleManager &TitleMan)
{
    sc::db::AdoExt AdoGame(m_GameDBConnString);
    APPEND_IPARAM_INT(AdoGame, "@nChaNum", nChaNum);

    int nRet = AdoGame.ExecuteStoredProcedure("dbo.ActivityRewardBadgeSelect");
    if (nRet == sc::db::DB_ERROR)
        return sc::db::DB_ERROR;

    if (AdoGame.GetEOF())
        return sc::db::DB_OK;
    
    BOOL bInserted = FALSE;

    // Load XML-IDs of badge, but not real badge string.    
    std::string Title;
    int nTitleClass	= 0;
    int bSelected	= 0;

    do
    {        
        // Bind data
        AdoGame.GetCollect("Title", Title);
        AdoGame.GetCollect("TitleClass", nTitleClass);
        AdoGame.GetCollect("Selected", bSelected);

        if (!Title.empty())
        {
            bInserted = TitleMan.Add(Title, (ENUM_ACTIVITY_CLASS) nTitleClass);
            if (bInserted && bSelected)
                TitleMan.Select();
        }
    } while (AdoGame.Next());

    return sc::db::DB_OK;
}

int AdoManager::ActivitySetPoint(IN LONGLONG *pllRewardPoint, UINT nChaNum)
{
    CTime ctCurrentTime = CTime::GetCurrentTime().GetTime();	
    CString csCurrentTime = ctCurrentTime.Format("%Y-%m-%d %H:%M:%S");

    sc::db::AdoExt AdoGame(m_GameDBConnString);
    APPEND_RPARAM_INT(AdoGame);

    APPEND_IPARAM_INT    (AdoGame, "@ChaNum",         nChaNum);
    APPEND_IPARAM_BIGINT (AdoGame, "@PointHigh",      pllRewardPoint[ACTIVITY_CLASS_HIGH]);
    APPEND_IPARAM_BIGINT (AdoGame, "@PointMedium",    pllRewardPoint[ACTIVITY_CLASS_MEDIUM]);
    APPEND_IPARAM_BIGINT (AdoGame, "@PointLow",       pllRewardPoint[ACTIVITY_CLASS_LOW]);
    APPEND_IPARAM_VARCHAR(AdoGame, "@LastUpdateDate", csCurrentTime.GetString(), csCurrentTime.GetLength());
        
    APPEND_OPARAM_BIGINT(AdoGame, "@nReturn");

    __int64 nRet = ExecuteStoredProcedureLonglongReturn("dbo.sp_ActivitySetRewardPoint", AdoGame, "@nReturn");
    if (nRet != sc::db::DB_OK)
    {	
        sc::writeLogError(std::string("ActivitySetPoint(): sp_ActivitySetRewardPoint failure."));
        return sc::db::DB_ERROR;
    }
    else
    {
        return sc::db::DB_OK;
    }
}

int AdoManager::ActivitySaveBadge(UINT nChaNum, TitleManager &TitleMan)
{
	VECTITLE& vecTitleToUpdate = TitleMan.GetDataToUpdate();
	if (vecTitleToUpdate.empty())
		return sc::db::DB_OK;
	
	CTime ctCurrentTime = CTime::GetCurrentTime().GetTime();	
	CString csCurrentTime = ctCurrentTime.Format("%Y-%m-%d %H:%M:%S");	
	ITER_VECTITLE iter;
	for (iter=vecTitleToUpdate.begin(); iter != vecTitleToUpdate.end(); ++iter)
	{
        sc::db::AdoExt AdoGame(m_GameDBConnString);
        APPEND_RPARAM_INT(AdoGame);

        APPEND_IPARAM_INT    (AdoGame, "@ChaNum", nChaNum);
        APPEND_IPARAM_VARCHAR(AdoGame, "@Title", iter->strTitle.c_str(), iter->strTitle.length());
        APPEND_IPARAM_INT    (AdoGame, "@Class", iter->nClass);
        APPEND_IPARAM_VARCHAR(AdoGame, "@LastUpdateDate", csCurrentTime.GetString(), csCurrentTime.GetLength());

        APPEND_OPARAM_INT(AdoGame, "@nReturn");

        ExecuteStoredProcedureIntReturn("dbo.sp_ActivitySaveBadge", AdoGame, "@nReturn");
	}

    const std::string strTile = TitleMan.GetCurrentTitle();
    sc::db::AdoExt AdoGame2(m_GameDBConnString);
    APPEND_IPARAM_INT(AdoGame2, "@ChaNum", nChaNum);
    if (!strTile.empty())
    {
        APPEND_IPARAM_VARCHAR(AdoGame2, "@Title", strTile.c_str(), strTile.length());
    }
    else
    {
        _variant_t varTitle;
        varTitle = VT_NULL;
        APPEND_IPARAM_VARCHAR(AdoGame2, "@Title", varTitle, sizeof(_variant_t));
    }

    if (ExecuteStoredProcedure("dbo.ActivityRewardBadgeUpdate", AdoGame2) == sc::db::DB_ERROR)
    {
        sc::writeLogError(
            sc::string::format(
                "AdoManager::ActivitySaveBadge dbo.ActivityRewardBadgeUpdate %1% %2%",
                nChaNum,
                strTile));
    }
        
	return sc::db::DB_OK;
}

int AdoManager::ActivityGetClosedActivityID(OUT ActivityBaseData &abd)
{
    sc::db::AdoExt Ado(m_GameDBConnString);

    if (!Ado.Execute4Cmd("dbo.ActivityClosedSelect", adCmdStoredProc))
        return sc::db::DB_ERROR;
    
    if (Ado.GetEOF())
        return sc::db::DB_OK;

    do
    {
        int iActivityID = 0;
        // Bind data
        Ado.GetCollect("ActivityID", iActivityID);
        abd.CloseActivity(iActivityID);
    } while (Ado.Next()); 
    
    return sc::db::DB_OK;
}






// ----------------------------------------------------------------------------
} // namespace db
// ----------------------------------------------------------------------------