//
//mjeon.activities
//
#include "../../pch.h"
#include "DbActionGameActivity.h"

#include <boost/smart_ptr/weak_ptr.hpp>

#include "../../Server/AgentServer.h"
#include "../../Server/s_CFieldServer.h"
#include "../../FieldServer/GLGaeaServer.h"


// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


class Activity;

CActivityLoad::CActivityLoad(std::tr1::shared_ptr<ActivityManager> &pActivityMan, std::tr1::shared_ptr<TitleManager> &pTitleMan, UINT nChaNum)
	:m_nChaNum(nChaNum)
{
	m_pActivityMan		= pActivityMan;
	m_pTitleMan			= pTitleMan;

	m_pllRewardPoint	= m_pActivityMan->GetRewardPoint();
}

int CActivityLoad::Execute(NetServer* pServer)
{
    if (!pServer)
        return NET_ERROR;

    CFieldServer* pFieldServer = reinterpret_cast<CFieldServer*> (pServer);
    if (!pFieldServer)
        return NET_ERROR;

    GLGaeaServer* pGaeaServer = pFieldServer->GetGLGaeaServer();
    if (!pGaeaServer)
        return NET_ERROR;

    if (m_pDbManager->GetConnectionType() != db::ADO)
    {
        sc::writeLogError("CActivityLoad::Execute. Use ADO");
        return NET_ERROR;
    }

	int nReturn = NET_ERROR;

	MMACTIVITIES& mmActivities = m_pActivityMan->GetData();

	nReturn = m_pDbManager->ActivityLoad(mmActivities, m_nChaNum);

	if (nReturn == sc::db::DB_OK)
	{
		UINT						cnt = 0;
		ACTIVITY_SYNC				sync;
		std::vector<ACTIVITY_SYNC>	vecSync;

		ITER_MMACTIVITIES			iter;
		Activity					*pactv = NULL;

		ActivityBaseData			*pabd = ActivityBaseData::GetInstance();

		if (!pabd)
		{
			sc::writeLogError(std::string("DbAction(CActivityLoad) failed. ActivityBaseData is NULL."));
			return NET_ERROR;
		}

		BOOL						bSyncFlag	= FALSE;
		BOOL						bClosed		= FALSE;

		for(iter = mmActivities.begin(); iter != mmActivities.end(); ++iter)
		{
			pactv = &(iter->second);

			if(pactv)
			{
				//
				// 이 캐릭터가 접속해있지 않은 동안, 다른 캐릭터들에 의해 Closed된 Activity들이 있을 수 있다.-> 동기화
				//
				bClosed		= pabd->IsClosedActivity(pactv->m_id.dwID);
				bSyncFlag	= pactv->GetSyncFlag();

#ifdef _TEMP_TRACING_LOG
				sc::writeLogDebug(sc::string::format("[특별활동] bClosed : %1%, bSyncFlag : %2%, pactv->m_id.dwID : %3%, pactv->GetStatus() : %4%, pactv->GetValue() : %5%", bClosed, bSyncFlag, pactv->m_id.dwID, (DWORD)pactv->GetStatus(), (DWORD)pactv->GetValue()));
#endif

				if(bClosed || bSyncFlag)
				{
					if(bClosed)
					{
						pactv->SetStatus(ACTIVITY_STATUS_CLOSED);	//Update
					}

					sync.dwID		= pactv->m_id.dwID;				//4-bytes
					sync.dwStatus	= (DWORD)pactv->GetStatus();	//4-bytes
					sync.dwValue	= (DWORD)pactv->GetValue();		//4-bytes

					vecSync.push_back(sync);
				}
			}
			else
			{
#ifdef _TEMP_TRACING_LOG
				sc::writeLogDebug(std::string("[특별활동] pactv is NULL"));
#endif
				continue;
			}
		}
		
		cnt = (UINT)vecSync.size();

		WORD wLastSeq = cnt / HOW_MANY_ACTVITITIES_PER_ONE_SYNC + ((cnt % HOW_MANY_ACTVITITIES_PER_ONE_SYNC) > 0);

#ifdef _TEMP_TRACING_LOG
		sc::writeLogDebug(sc::string::format("[특별활동] cnt : %1%, wLastSeq[cnt / 50 + ((cnt / 50) > 0)] : %2%", cnt, wLastSeq));
#endif


		GLMSG::SNET_ACTIVITY_SYNC msgSync;
		msgSync.nChaNum = m_nChaNum;


		if (wLastSeq == 1)
		{
			msgSync.SetData(vecSync, 1, wLastSeq);
			pGaeaServer->SENDTOAGENT(&msgSync);

#ifdef _TEMP_TRACING_LOG
			sc::writeLogDebug(sc::string::format("[특별활동] TOP - seq : 1, wLastSeq %1%", wLastSeq));
#endif
		}
		else
		{
			std::vector<ACTIVITY_SYNC>::iterator iterCurrent = vecSync.begin();
			UINT i = 1;

			//
			// 1 ~ (n-1)
			//
			for (WORD wSeq = 1; wSeq < wLastSeq; wSeq++)
			{	
				msgSync.SetData(iterCurrent, iterCurrent + HOW_MANY_ACTVITITIES_PER_ONE_SYNC, wSeq, wLastSeq);
				pGaeaServer->SENDTOAGENT(&msgSync);

#ifdef _TEMP_TRACING_LOG
				sc::writeLogDebug(sc::string::format("[특별활동] MIDDLE - seq : %1%, wLastSeq %2%", wSeq, wLastSeq));
#endif

				iterCurrent = iterCurrent + HOW_MANY_ACTVITITIES_PER_ONE_SYNC;
			}
			//
			// n (last one)
			//
			msgSync.SetData(iterCurrent, vecSync.end(), wLastSeq, wLastSeq);
			pGaeaServer->SENDTOAGENT(&msgSync);

#ifdef _TEMP_TRACING_LOG
			sc::writeLogDebug(sc::string::format("[특별활동] BOTTOM - seq : %1%, wLastSeq %2%", wLastSeq, wLastSeq));
#endif
		}
	}
	else
	{
		sc::writeLogError(std::string("DbAction(CActivityLoad) failed to call ActivityLoad()."));

		return nReturn;
	}

	//
	// Reward Point
	//
	nReturn = m_pDbManager->ActivityGetPoint(m_pllRewardPoint, m_nChaNum);

	if (nReturn == sc::db::DB_OK)
	{
		GLMSG::SNET_ACTIVITY_REWARD_POINT_SYNC msgPoint;
		msgPoint.nChaNum = m_nChaNum;

		msgPoint.llRewardPointHigh	= m_pllRewardPoint[ACTIVITY_CLASS_HIGH];
		msgPoint.llRewardPointMedium= m_pllRewardPoint[ACTIVITY_CLASS_MEDIUM];
		msgPoint.llRewardPointLow	= m_pllRewardPoint[ACTIVITY_CLASS_LOW];

		pGaeaServer->SENDTOAGENT(&msgPoint);
	}
	else
	{
		sc::writeLogError(std::string("DbAction(CActivityLoad) failed to call ActivityGetPoint()."));

		return nReturn;
	}

	//
	// Reward Badge
	//
	nReturn = m_pDbManager->ActivityLoadBadge(m_nChaNum, *m_pTitleMan);

	if (nReturn == sc::db::DB_OK)
	{
		UINT					cnt = 0;
		TITLE_SYNC				sync;
		std::vector<TITLE_SYNC>	vecSync;

		MAPTITLE&				mapTitles = m_pTitleMan->GetData();
		ITER_MAPTITLE			iter;
		TITLEINFO				*pti = NULL;

		for(iter = mapTitles.begin(); iter != mapTitles.end(); ++iter)
		{
			pti = &(iter->second);

			if(pti)
			{
				sync.nIndex		= pti->nIndex;											//4-bytes
				sync.dwClass	= pti->nClass;											//4-bytes
				sync.bSelected	= pti->bSelected;										//4-bytes
				StringCchCopy(sync.szTitle, EM_TITLEID_MAXLENGTH, pti->strTitle.c_str());	//16-bytes

				vecSync.push_back(sync);
			}
			else
			{
				continue;
			}
		}

		cnt = (UINT)vecSync.size();		

		WORD wLastSeq = cnt / HOW_MANY_TITLES_PER_ONE_SYNC + ((cnt % HOW_MANY_TITLES_PER_ONE_SYNC) > 0);


		GLMSG::SNET_ACTIVITY_REWARD_BADGE_SYNC msgSync;
		msgSync.nChaNum = m_nChaNum;


		if (wLastSeq == 1)
		{
			msgSync.SetData(vecSync, 1, wLastSeq);
			pGaeaServer->SENDTOAGENT(&msgSync);
		}
		else
		{
			std::vector<TITLE_SYNC>::iterator iterCurrent = vecSync.begin();
			UINT i = 1;

			//
			// 1 ~ (n-1)
			//
			for (WORD wSeq = 1; wSeq < wLastSeq; wSeq++)
			{	
				msgSync.SetData(iterCurrent, iterCurrent + HOW_MANY_TITLES_PER_ONE_SYNC, wSeq, wLastSeq);

				pGaeaServer->SENDTOAGENT(&msgSync);

				iterCurrent = iterCurrent + HOW_MANY_TITLES_PER_ONE_SYNC;

				GASSERT(iterCurrent < vecSync.end());
			}
			//
			// n (last one)
			//
			msgSync.SetData(iterCurrent, vecSync.end(), wLastSeq, wLastSeq);
			pGaeaServer->SENDTOAGENT(&msgSync);
		}
	}
	else
	{
		sc::writeLogError(std::string("DbAction(CActivityLoad) failed to call ActivityLoadBadge()."));

		return nReturn;
	}

	//
	//Notify the FieldServer that ActivityLoad() has been completed.
	// and the receiving FieldServer will call CheckAfterLoad().
	//
	//CFieldServer* pFieldServer = reinterpret_cast<CFieldServer*> (pServer);

	GLMSG::SNET_ACTIVITY_LOAD_COMPLETED_FF msg;
	msg.nChaNum = m_nChaNum;
	pFieldServer->InsertMsg(DBACTION_CLIENT_ID, &msg);	//m_dwClient는 아직 유효하지 않다. 위의 SYNC 메시지들을 Agent를 통해서 Relay하는 이유이기도 하다.
	

	return nReturn;
}


//
// mmActivities MUST be copied(or use smart-pointer) but not referenced because the owner character could be destructed before
//  CActivitySave is poped and CActivitySave::Execute() runs.
//
CActivitySave::CActivitySave(DWORD dwClientID, std::tr1::shared_ptr<ActivityManager> &pActivityMan, std::tr1::shared_ptr<TitleManager> &pTitleMan, UINT nChaNum, UINT nChaLevel)
	:m_nChaNum(nChaNum)
	,m_nChaLevel(nChaLevel)
{
	m_pActivityMan	= pActivityMan;
	m_pTitleMan		= pTitleMan;

	m_ClientSlot = dwClientID;
}

int CActivitySave::Execute(NetServer* pServer)
{
	int nReturn = 0;
	
	if (!pServer)
		return NET_ERROR;

    if (m_pDbManager->GetConnectionType() != db::ADO)
    {
        sc::writeLogError("CActivitySave::Execute. Use ADO");
        return NET_ERROR;
    }

	//
	//save activity info
	//
	nReturn = m_pDbManager->ActivitySave(m_pActivityMan->GetData(), m_nChaNum, m_nChaLevel);

	if (nReturn > 0)	//nReturn is the number of affected rows.
	{
		;
	}
	else
	{
		//
		// Not an error. This situation makes sense but it's better to check it out.
		//
		sc::writeLogWarn(std::string("DbAction(CActivitySave): saved 0 activities."));

		return nReturn;
	}

	//
	//save reward-point
	//
	nReturn = m_pDbManager->ActivitySetPoint(m_pActivityMan->GetRewardPoint(), m_nChaNum);

	if (nReturn == sc::db::DB_OK)
	{
		;
	}
	else
	{
		//
		// Not an error. This situation makes sense but it's better to check it out.
		//
		sc::writeLogWarn(std::string("DbAction(CActivitySave): ActivitySetPoint failure."));

		return nReturn;
	}


	//
	//save reward-titles
	//
	nReturn = m_pDbManager->ActivitySaveBadge(m_nChaNum, *m_pTitleMan);

	if (nReturn == sc::db::DB_OK)
	{
		;
	}
	else
	{
		//
		// Not an error. This situation makes sense but it's better to check it out.
		//
		sc::writeLogWarn(std::string("DbAction(CActivitySave): ActivitySaveBadge failure."));

		return nReturn;
	}


	return nReturn;
}


//
// Activities based on the order of completion -----> DB transaction
//
CActivityByOrder::CActivityByOrder(DWORD dwClientID, std::tr1::shared_ptr<ActivityManager> &pActivityMan, Activity &actv, UINT nChaNum, UINT nChaLevel)
:m_Activity(actv)
,m_nChaNum(nChaNum)
,m_nChaLevel(nChaLevel)
{
	m_pActivityMan  = pActivityMan;

	m_ClientSlot = dwClientID;
}


int CActivityByOrder::Execute(NetServer* pServer)
{
    if (!pServer)
        return NET_ERROR;

    CFieldServer* pFieldServer = reinterpret_cast<CFieldServer*> (pServer);
    if (!pFieldServer)
        return NET_ERROR;

    GLGaeaServer* pGaeaServer = pFieldServer->GetGLGaeaServer();
    if (!pGaeaServer)
        return NET_ERROR;

    if (m_pDbManager->GetConnectionType() != db::ADO)
    {
        sc::writeLogError("CActivityByOrder::Execute. Use ADO");
        return NET_ERROR;
    }

	int nReturn = 0;	

	nReturn = m_pDbManager->ActivityOrdering(m_Activity, m_nChaNum, m_nChaLevel);

	if (nReturn == ACTIVITY_STATUS_COMPLETED)	//nReturn is the NEW STATUS of the Activity.
	{
		m_Activity.SetStatus(ACTIVITY_STATUS_COMPLETED);


		//
		//Notify the update of status to the client who has done the activity.
		//
		GLMSG::SNET_ACTIVITY_UPDATE_COMPLETED msgUpdateStatus;
		msgUpdateStatus.arrdwActivityID[0] = m_Activity.m_id.dwID;
		msgUpdateStatus.dwArrSize = 1;
		
		pGaeaServer->SENDTOCLIENT(m_ClientSlot, &msgUpdateStatus);

		//Notify to clients based on the activity's notify-level.
		
		//
		// Field -> Agent 
		//
		GLMSG::SNET_ACTIVITY_COMPLETED_FA msgRelay;
		msgRelay.dwActivityID  = m_Activity.m_id.dwID;
		msgRelay.dwNotifyLevel = (DWORD)m_Activity.m_nNotifyLevel;
		msgRelay.dwChaNumCloser= m_nChaNum;


		/*
		//
		// PopUp message may replace this notify.
		//
		//
		// Field -> Clients
		//
		GLMSG::SNET_ACTIVITY_NOTIFY_COMPLETED msgNotify;
		msgNotify.dwActivityID	= m_Activity.m_id.dwID;
		msgNotify.dwChaNumWho	= m_nChaNum;
		*/

		/*
		//
		// msg for PopUp
		//
		PGLCHAR pCha = (PGLCHAR) pGaeaServer->GetCharID(m_nChaNum);
		if (pCha != NULL)
		{
			GLMSG::SNET_ACTIVITY_POPUP_COMPLETE msgPopUp;
			msgPopUp.dwActivityID	= m_Activity.m_id.dwID;
			msgPopUp.dwGaeaID = m_ .dwChaNumWho	= m_nChaNum;

			pCha->SendMsgViewAround(&msgPopUp);
		}
		*/
		

		//
		//insert a CompletionMsg into Field (Field -> Field)
		//
		CFieldServer* pFieldServer = reinterpret_cast<CFieldServer*> (pServer);

		if (pFieldServer)
		{
			GLMSG::SNET_ACTIVITY_COMPLETED_FF msg;
			msg.dwChaNum		= m_nChaNum;
			msg.dwActivityID	= m_Activity.m_id.dwID;
			msg.dwClass			= (DWORD)m_Activity.m_nClass;
			msg.dwRewardPoint	= (DWORD)m_Activity.GetRewardPoint();
			
			StringCchCopy(msg.szRewardTitleID, EM_TITLEID_MAXLENGTH, m_Activity.GetRewardTitleID().c_str());

			pFieldServer->InsertMsg(m_ClientSlot, &msg);
		}
		
		switch( m_Activity.m_nNotifyLevel )
		{
			case ACTIVITY_NOTIFY_AROUND:
			{
				//
				// PopUp message may replace this notify.
				//
				/*
				PGLCHAR pCha = (PGLCHAR) pGaeaServer->GetCharID(m_nChaNum);
				if (pCha != NULL)
				{
					pCha->SendMsgViewAround(&msgNotify);
				}
				*/
				break;
			}
			case ACTIVITY_NOTIFY_BROADCAST:
			{
				pGaeaServer->SENDTOAGENT(&msgRelay);
				break;
			}
			case ACTIVITY_NOTIFY_CLUB:
			{
				pGaeaServer->SENDTOAGENT(&msgRelay);
				break;
			}
			case ACTIVITY_NOTIFY_CLUBANDAROUND:
			{
				//
				// PopUp message may replace this notify.
				//
				/*
				//Around
				PGLCHAR pCha = (PGLCHAR) pGaeaServer->GetCharID(m_nChaNum);
				if (pCha != NULL)
				{
					pCha->SendMsgViewAround(&msgNotify);
				}
				*/

				//Club
				pGaeaServer->SENDTOAGENT(&msgRelay);
				break;
			}
			default:	//case ACTIVITY_NOTIFY_NONE:
			{
				break;
			}
		}
	}	
	else if (nReturn == ACTIVITY_CLOSER)
	{
		m_Activity.SetStatus(ACTIVITY_STATUS_COMPLETED);
		//
		//this character is the activity's closer. (i.e.last one who has completed this activity by order of completion).
		//  inform to all clients that this activity is not valid anymore.
		//  --> sync by ActivityBaseData::CloseActivity().
		//  --> IsClosedActivity() is used by all ActiviyManagers to check the activity is closed.
		//
		//All characters will be updated when the next Update() or ActivityManager::Load() is called.
		//  --> It's better for performance than iteration for all characters at the same time.		
		//
		ActivityBaseData::GetInstance()->CloseActivity(m_Activity.m_id.dwID);


		//
		//Notify the update of status to the client who has done the activity.
		//
		GLMSG::SNET_ACTIVITY_UPDATE_COMPLETED msgUpdateStatus;
		msgUpdateStatus.arrdwActivityID[0] = m_Activity.m_id.dwID;
		msgUpdateStatus.dwArrSize = 1;
		
		pGaeaServer->SENDTOCLIENT(m_ClientSlot, &msgUpdateStatus);



		//Notify to clients based on the activity's notify-level.
		
		//
		// Field -> Agent 
		//
		// Completed
		GLMSG::SNET_ACTIVITY_COMPLETED_FA msgRelayCompleted;
		msgRelayCompleted.dwActivityID  = m_Activity.m_id.dwID;
		msgRelayCompleted.dwNotifyLevel = (DWORD)m_Activity.m_nNotifyLevel;
		msgRelayCompleted.dwChaNumCloser= m_nChaNum;

		// Closed - broadcast
		GLMSG::SNET_ACTIVITY_CLOSED_FA msgRelayClosed;
		msgRelayClosed.dwActivityID  = m_Activity.m_id.dwID;		
		msgRelayClosed.dwChaNumCloser= m_nChaNum;

		pGaeaServer->SENDTOAGENT(&msgRelayClosed);
		
		/*
		//
		// Field -> Clients
		//
		// Completed
		GLMSG::SNET_ACTIVITY_NOTIFY_COMPLETED msgNotifyCompleted;
		msgNotifyCompleted.dwActivityID	= m_Activity.m_id.dwID;
		msgNotifyCompleted.dwChaNumWho	= m_nChaNum;
		*/

		/*
		//
		// msg for PopUp
		//
		PGLCHAR pCha = (PGLCHAR) pGaeaServer->GetCharID(m_nChaNum);
		if (pCha != NULL)
		{
			GLMSG::SNET_ACTIVITY_POPUP_COMPLETE msgPopUp;
			msgPopUp.dwActivityID	= m_Activity.m_id.dwID;
			msgPopUp.dwChaNumWho = m_nChaNum;

			pCha->SendMsgViewAround(&msgPopUp);
		}
		*/

		//
		//insert a CompletionMsg into Field (Field -> Field)
		//
		CFieldServer* pFieldServer = reinterpret_cast<CFieldServer*> (pServer);

		if (pFieldServer)
		{
			GLMSG::SNET_ACTIVITY_COMPLETED_FF msg;
			msg.dwChaNum		= m_nChaNum;
			msg.dwActivityID	= m_Activity.m_id.dwID;
			msg.dwClass			= (DWORD)m_Activity.m_nClass;
			msg.dwRewardPoint	= (DWORD)m_Activity.GetRewardPoint();
			
			StringCchCopy(msg.szRewardTitleID, EM_TITLEID_MAXLENGTH, m_Activity.GetRewardTitleID().c_str());
			//msg.strRewardTitle	= m_Activity.GetRewardTitle();

			pFieldServer->InsertMsg(m_ClientSlot, &msg);
		}


		switch( m_Activity.m_nNotifyLevel )
		{
			case ACTIVITY_NOTIFY_AROUND:
			{
				//
				// PopUp message may replace this notify.
				//
				/*
				PGLCHAR pCha = (PGLCHAR) pGaeaServer->GetCharID(m_nChaNum);
				if (pCha != NULL)
				{
					pCha->SendMsgViewAround(&msgNotify);
				}
				*/
				break;
			}
			case ACTIVITY_NOTIFY_BROADCAST:
			{
				pGaeaServer->SENDTOAGENT(&msgRelayCompleted);
				
				break;
			}
			case ACTIVITY_NOTIFY_CLUB:
			{
				pGaeaServer->SENDTOAGENT(&msgRelayCompleted);
				
				break;
			}
			case ACTIVITY_NOTIFY_CLUBANDAROUND:
			{
				//
				// PopUp message may replace this notify.
				//
				/*
				//Around
				PGLCHAR pCha = (PGLCHAR) pGaeaServer->GetCharID(m_nChaNum);
				if (pCha != NULL)
				{
					pCha->SendMsgViewAround(&msgNotify);
				}
				*/

				//Club
				pGaeaServer->SENDTOAGENT(&msgRelayCompleted);
				
				break;
			}
			default:	//case ACTIVITY_NOTIFY_NONE:
			{
				//Nothing to do.
				break;
			}
		}
	}	
	else if (nReturn == ACTIVITY_STATUS_CLOSED)
	{
		//this character is the next one after the activity's closer.
		// This is abnormal case. MUST NOT come into here but it's not a problem because it can be handled properly.
		//
		if (m_Activity.GetStatus() != ACTIVITY_STATUS_COMPLETED)
		{
			m_Activity.SetStatus(ACTIVITY_STATUS_CLOSED);
		}
	}
	else	//ACTIVITY_STATUS_ORDERING: ORDERING Failure !!!
	{
		//
		//MUST NOT come into here. Anyway ACTIVITY_STATUS_ORDERING will be handled in next sp_ActivitySyncStatus during Load().
		//
		//  ACTIVITY_STATUS_ORDERING  --->  ACTIVITY_STATUS_COMPLETED or ACTIVITY_STATUS_CLOSED
		//
		sc::writeLogError(std::string("DbAction(CActivityByOrder): Ordering Failure!! Check it out."));
	}

	return sc::db::DB_OK;
}