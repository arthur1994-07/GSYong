//
//mjeon.activities
//
#include "pch.h"
#include "../../RanLogic/Activity/ActivityBase.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "./ActivityManager.h"
#include "../Database/DBAction/DbActionGameActivity.h"
#include "../Character/GLChar.h"
#include "../FieldServer/GLGaeaServer.h"

#include "../../SigmaCore/Log/LogMan.h"


// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------



ActivityManager::ActivityManager(GLGaeaServer* pGaeaServer, ActivityBaseData &abd, DWORD &dwOwerChaNum)
    : m_pGaeaServer(pGaeaServer)
    , m_abd(abd)
    , m_dwOwnerChaNum(dwOwerChaNum)
    , m_pCharOwner(NULL)
    , m_bLoaded(FALSE)
{	
	//
	//copy the data from ActivityBaseData.
	//
	m_mmActivities = abd.GetData();	//copy-by-value is required. only once per an entire game-session.
}


ActivityManager::~ActivityManager()
{
	m_pCharOwner = NULL;
}


void ActivityManager::Reset()
{
	m_dwOwnerChaNum = 0;
	m_mmActivities.clear();
}


//
//Load() from DB
//  called when the character comes into the game(or another field from one of the others).
//
void ActivityManager::Load(GLChar *pChar, BOOL bTransfer/*DB�� ������� �ʰ� ĳ���� ����*/)
{
	if (!GLUseFeatures::GetInstance().IsUsingActivity())
	{
		//sc::writeLogError(std::string("Load() bypass. Activity is turned OFF!"));
		return;
	}

	m_pCharOwner = pChar;

	if (bTransfer)
	{
#ifdef _TEMP_TRACING_LOG
		sc::writeLogDebug(std::string("[Ư��Ȱ��] bTransfer == TRUE"));
#endif
		return;
	}
	else
	{
#ifdef _TEMP_TRACING_LOG
		sc::writeLogDebug(std::string("[Ư��Ȱ��] bTransfer == FALSE"));
#endif
	}

	if (pChar)
	{
		pChar->AddGameAdoJob(
			db::DbActionPtr(
				new CActivityLoad(
                    pChar->m_pActivityMan,
                    pChar->m_pTitleMan,
                    pChar->m_CharDbNum)));
	}
	else
	{
		sc::writeLogError(std::string("ActivityManager::Load(): Invalid GLChar"));
	}
}


//
//to do when a character moves into a new fieldserver.
//
void ActivityManager::CheckAfterLoad()
{
	//
	//mjeon.activity
	//
	m_pCharOwner->ActivityCheckMap();
	m_pCharOwner->ActivityCheckLevel();
}


//
// Save() to DB
//  called when the character moves to other field or close the game.
//
void ActivityManager::Save(GLChar *pChar)
{
	if (!GLUseFeatures::GetInstance().IsUsingActivity())
	{
		//sc::writeLogError(std::string("Save() bypass. Activity is turned OFF!"));
		return;
	}

	if (pChar)
	{
		pChar->AddGameAdoJob(
			db::DbActionPtr(
				new CActivitySave(pChar->ClientSlot(), pChar->m_pActivityMan, pChar->m_pTitleMan, pChar->m_CharDbNum, (UINT)pChar->GetLevel() ) ));
	}
	else
	{
		sc::writeLogError(std::string("ActivityManager::Save(): Invalid GLChar"));
	}
}


UINT ActivityManager::GetBaseValue(DWORD dwActivityID)
{
	Activity *pactv = m_abd.GetActivity(dwActivityID);

	if(!pactv)
		return 0;

	return pactv->m_nValueBase;
}


//
//returns how many activities have been updated if the action on the target is valid. 
//  otherwise, returns 0.
//
DWORD ActivityManager::CheckLevel(DWORD dwLevel)
{
	//
	// Level-up does not happen frequently. just go ahead.
	//
	//if (!m_abd.IsValidLevel(dwLevel))
	//	return 0;
	
	return UpdateActivity(ACTIVITY_MID_LEVEL, dwLevel);
}

DWORD ActivityManager::CheckMobKill(GLChar* pKillChar, GLChar* pChar, DWORD dwTargetID)
{
	if( !pKillChar )
		return 0;

	if (!m_abd.IsValidMob(dwTargetID))
		return 0;

	if ( !pKillChar->IsValidBody() )			
		return 0;

	if ( pKillChar->GetCurrentMap() != pChar->GetCurrentMap() )	
		return 0;

	if ( pChar->GetPartyID().isValidExpedition() == true )
		return 0; // ��ȹ�� ��û���� ����; ���� �������� ó�� �� �� �ְ� �ؾ� ��;

	D3DXVECTOR3 vDis = pChar->GetPosition() - pKillChar->GetPosition();
	float fDis = D3DXVec3Length ( &vDis );
	if ( fDis > GLCONST_CHAR::fPARTYEXPRANGE )	
		return 0;

	return UpdateActivity(ACTIVITY_MID_KILLMOB, dwTargetID);
}

DWORD ActivityManager::CheckPK(DWORD dwTargetID)
{
	if (!m_abd.IsValidPKMap(dwTargetID))
		return 0;

	if (m_abd.IsValidMob(dwTargetID))
		return 0;
	
	return UpdateActivity(ACTIVITY_MID_KILLPC, dwTargetID);
}


DWORD ActivityManager::CheckMap(DWORD dwTargetID)
{
	if (!m_abd.IsValidMap(dwTargetID))
		return 0;
	
	return UpdateActivity(ACTIVITY_MID_MAP, dwTargetID);
}


DWORD ActivityManager::CheckItemUse(DWORD dwTargetID)
{
	if (!m_abd.IsValidItemUse(dwTargetID))
		return 0;
	
	return UpdateActivity(ACTIVITY_MID_USEITEM, dwTargetID);
}


DWORD ActivityManager::CheckItemTake(DWORD dwTargetID, DWORD dwActivityID)
{
	if (!m_abd.IsValidItemTake(dwTargetID))
		return 0;
	
	return UpdateActivity(ACTIVITY_MID_TAKE, dwTargetID, dwActivityID);
}


//
//check and update closed activity
//
void ActivityManager::CloseActivity(DWORD dwActivityID)
{
	return m_abd.CloseActivity(dwActivityID);
}

BOOL ActivityManager::IsClosedActivity(DWORD dwActivityID)
{
	return m_abd.IsClosedActivity(dwActivityID);
}


//
//completed an activity: process for rewards
//
void ActivityManager::CompletedActivity(DWORD dwActivityID, ENUM_ACTIVITY_CLASS eClass, UINT nRewardPoint, std::string &strRewardTitleID)
{
	if (!GLUseFeatures::GetInstance().IsUsingActivity())
	{
		sc::writeLogError(std::string("CompletedActivity() - Wrong use of Activity while it is OFF!"));
		return;
	}

	// GM�� Ư��Ȱ�� �޼��� �ٸ��������� �˸��� �ʴ´�
	if ( m_pCharOwner->UserLevel() >= USER_USER_GM )
		return;

// sckim : Activity
//sc::writeLogError(sc::string::format("ActivityManager::CompletedActivity m_pCharOwner->GetGaeaID() : %1%", m_pCharOwner->GetGaeaID()));

	//
	// msg for PopUp
	//
	GLMSG::SNET_ACTIVITY_POPUP_COMPLETE msgPopUp;
	msgPopUp.dwActivityID	= dwActivityID;
	msgPopUp.dwGaeaID		= m_pCharOwner->GetGaeaID();

	m_pCharOwner->SendMsgViewAround(&msgPopUp);


	//
	//process for reward-title
	//
	m_pCharOwner->m_pTitleMan->AcquireNewTitle(strRewardTitleID, eClass);


	//
	//process for reward-point
	//
	AddUpRewardPoint(nRewardPoint, eClass);
}


void ActivityManager::AddUpRewardPoint(UINT nRewardPoint, ENUM_ACTIVITY_CLASS eClass)
{
	m_llRewardPoint[eClass] += nRewardPoint;
}


//
//core-method used by other CheckXXX methods.
//
DWORD ActivityManager::UpdateActivity(WORD wMID, DWORD dwTargetValue, DWORD dwActivityID/*Mandatory for ItemTake, Optional for others*/)
{
	if (!GLUseFeatures::GetInstance().IsUsingActivity())
	{
		//sc::writeLogError(std::string("UpdateActivity() - Wrong use of Activity while it is OFF!"));
		return 0;
	}

	if(!m_bLoaded)
	{
		//When a character moves into other FieldServer, it may take some time to load activity. 
		// Therefore, first check the ActivityManager's ready.
		return 0;

		//sc::writeLogWarn(std::string("ActivityManager::UpdateActivity() - ActivityLoad() MUST be completed before the update."));
	}

	ITER_MMACTIVITIES iter;
	Activity *pactv = NULL;
	DWORD dwRet = 0;

// sckim : Activity
//sc::writeLogError(sc::string::format("ActivityManager::UpdateActivity for statement start : %1%", m_bLoaded));
	std::vector<DWORD>	vecUpdateCompleteID;

	for (iter = m_mmActivities.lower_bound(wMID); iter != m_mmActivities.upper_bound(wMID); ++iter)
	{
		pactv = &(iter->second);

		// GetValidate() == TRUE : ������� �ʵ��� ������ Ư��Ȱ���� �������� ó������ �ʵ��� ����
		if ( pactv  && pactv->IsMatched(dwTargetValue) && pactv->GetValidate() == TRUE)
		{
			// There can be several matches in the multi-map.

			//
			//Check closed-activity list maintained by server
			//
			if ( IsClosedActivity(pactv->m_id.dwID) )
			{
				if (pactv->GetStatus() == ACTIVITY_STATUS_INPROGRESS)
					pactv->SetStatus(ACTIVITY_STATUS_CLOSED);
				
				continue;
			}

			//
			//In case of ItemTake, DB does not maintain the status of value of it. just handle the completion for the specific Activity.
			//
			if (wMID == ACTIVITY_MID_TAKE)
			{
				if (dwActivityID != pactv->m_id.dwID)
					continue;
			}

			// ���̵����� Ư��Ȱ�� üũ�� ActivityCheckMap �� CheckAfterLoad �ִµ�
			// �ʵ弭�� �̵��ÿ��� CheckAfterLoad() ó���ؾ��ϴµ� �̶� ������ �����߾��� �ʵ弭���� �̹� �Ϸ� �޼����� ���۵� �����̹Ƿ� �̵��� �ʵ弭�������� �ش� �Ϸ� �޼����� ������ 
			// ���� ���°� �߻���. ���� �̷� ��츦 �������ؼ� ���¸� ������. Ŭ�󿡼��� �ش� �޼����� ������ �ڽ��� ���¸� ���ؼ� ���°� �ٸ� ��� �Ϸ�� ����
			BOOL bCompletedOnMap = pactv->IsCompletedOnMap();

			if( bCompletedOnMap )
			{
// sckim : Activity
//sc::writeLogError(sc::string::format("ActivityManager::UpdateActivity bCompletedOnMap is true, pactv->m_id : %1%, %2%", pactv->m_id.wMainID, pactv->m_id.wSubID));

				/*
				GLMSG::SNET_ACTIVITY_UPDATE_COMPLETED msgCompletedStatus;
				msgCompletedStatus.dwActivityID	= pactv->m_id.dwID;
				
				m_pGaeaServer->SENDTOCLIENT(m_pCharOwner->ClientSlot(), &msgCompletedStatus);
				*/
				vecUpdateCompleteID.push_back(pactv->m_id.dwID);
			}
			else
			{
				//
				//if this activity is not closed, let's Update.
				//
				BOOL bUpdated = pactv->Update();

				if (bUpdated)
				{
					//
					//Notify the update the value to the client.
					//
					GLMSG::SNET_ACTIVITY_UPDATE_VALUE msgUpdateValue;
					msgUpdateValue.dwActivityID	= pactv->m_id.dwID;
					msgUpdateValue.nValue		= pactv->GetValue();

					m_pGaeaServer->SENDTOCLIENT(m_pCharOwner->ClientSlot(), &msgUpdateValue);


					//This activity has been completed but required to be ordered by DB transaction.
					//
					// ACTIVITY_STATUS_INPROGRESS ---> ACTIVITY_STATUS_ORDERING
					//
					if (pactv->GetStatus() == ACTIVITY_STATUS_ORDERING)
					{
						if ( m_pCharOwner->UserLevel() >= USER_USER_GM )
						{
							pactv->SetStatus(ACTIVITY_STATUS_CLOSED);

							continue;
						}

// sckim : Activity
//sc::writeLogError(sc::string::format("ActivityManager::UpdateActivity UpdateDBByOrder : %1%", msgUpdateValue.dwActivityID));
						UpdateDBByOrder(*pactv);	//DB update right now.
					}
					else if (pactv->GetStatus() == ACTIVITY_STATUS_COMPLETED)
					{
// sckim : Activity
//sc::writeLogError(sc::string::format("ActivityManager::UpdateActivity ACTIVITY_STATUS_COMPLETED : %1%", pactv->GetStatus()));
                        /*
                         * redmine  : #2462 Ư��Ȱ�� ������ ��ü�� �˸� ��� �ȵǴ� ���� Ȯ�� �� ����
                         * modified : sckim, 2016.01.07
                         */
                        /*
						//
						//Notify the update of status to the client who has done the activity.
						//
						CompletedActivity(pactv->m_id.dwID, pactv->m_nClass, pactv->GetRewardPoint(), pactv->GetRewardTitleID());


						GLMSG::SNET_ACTIVITY_UPDATE_COMPLETED msgUpdateStatus;
						msgUpdateStatus.dwActivityID	= pactv->m_id.dwID;
						
						m_pGaeaServer->SENDTOCLIENT(m_pCharOwner->ClientSlot(), &msgUpdateStatus);
                        */
						/*
						GLMSG::SNET_ACTIVITY_UPDATE_COMPLETED msgUpdateStatus;
						msgUpdateStatus.dwActivityID	= pactv->m_id.dwID;
						
						m_pGaeaServer->SENDTOCLIENT(m_pCharOwner->ClientSlot(), &msgUpdateStatus);
						*/
						vecUpdateCompleteID.push_back(pactv->m_id.dwID);
                        // GM�� ��� �ֺ��Ǵ� ��ü ������ ���� �ʴ´�.
						if ( m_pCharOwner->UserLevel() >= USER_USER_GM )
						{
							continue;
						}

						//
						//Notify the update of status to the client who has done the activity.
						//
						CompletedActivity(pactv->m_id.dwID, pactv->m_nClass, pactv->GetRewardPoint(), pactv->GetRewardTitleID());

		                //
		                // Field -> Agent 
		                //
		                GLMSG::SNET_ACTIVITY_COMPLETED_FA msgRelay;
		                msgRelay.dwActivityID  = pactv->m_id.dwID;
		                msgRelay.dwNotifyLevel = (DWORD)pactv->m_nNotifyLevel;
		                msgRelay.dwChaNumCloser= m_dwOwnerChaNum;

		                switch(pactv->m_nNotifyLevel)
		                {
			                case ACTIVITY_NOTIFY_AROUND:
			                {
// sckim : Activity
//sc::writeLogError(sc::string::format("ActivityManager::UpdateActivity ACTIVITY_STATUS_COMPLETED->ACTIVITY_NOTIFY_AROUND : %1%", pactv->GetStatus()));
				                break;
			                }
			                case ACTIVITY_NOTIFY_BROADCAST:
			                {
// sckim : Activity
//sc::writeLogError(sc::string::format("ActivityManager::UpdateActivity ACTIVITY_STATUS_COMPLETED->ACTIVITY_NOTIFY_BROADCAST : %1%", pactv->GetStatus()));
				                m_pGaeaServer->SENDTOAGENT(&msgRelay);
				                break;
			                }
			                case ACTIVITY_NOTIFY_CLUB:
			                {
// sckim : Activity
//sc::writeLogError(sc::string::format("ActivityManager::UpdateActivity ACTIVITY_STATUS_COMPLETED->ACTIVITY_NOTIFY_CLUB : %1%", pactv->GetStatus()));
				                m_pGaeaServer->SENDTOAGENT(&msgRelay);
				                break;
			                }
			                case ACTIVITY_NOTIFY_CLUBANDAROUND:
			                {
// sckim : Activity
//sc::writeLogError(sc::string::format("ActivityManager::UpdateActivity ACTIVITY_STATUS_COMPLETED->ACTIVITY_NOTIFY_CLUBANDAROUND : %1%", pactv->GetStatus()));
				                m_pGaeaServer->SENDTOAGENT(&msgRelay);
				                break;
			                }
			                default:
			                {
				                break;
			                }
		                }
					}

					dwRet++;
				}
			}
		}
	}

	if ( vecUpdateCompleteID.empty() == false )
	{
		GLMSG::SNET_ACTIVITY_UPDATE_COMPLETED msgUpdateStatus;

		const INT nSize(vecUpdateCompleteID.size())
			, nSendSize(GLMSG::SNET_ACTIVITY_UPDATE_COMPLETED::MAXSIZE);

		INT nInput = 0;
		for ( int i = 0; i < nSize; ++i )
		{
			msgUpdateStatus.arrdwActivityID[nInput++] = vecUpdateCompleteID[i];
			// �����������̰ų�, �ѹ��� �������� �Ǹ� ������;
			if ( ((i+1) == nSize)|| ((nInput)%nSendSize == 0) )
			{
				msgUpdateStatus.dwArrSize = nInput;
				m_pGaeaServer->SENDTOCLIENT(m_pCharOwner->ClientSlot(), &msgUpdateStatus);
				msgUpdateStatus = GLMSG::SNET_ACTIVITY_UPDATE_COMPLETED();
				nInput = 0;
			}
		}
	}

	return dwRet;
}


void ActivityManager::UpdateDBByOrder(Activity &actv)
{
	if (m_pCharOwner)
	{
		m_pCharOwner->AddGameAdoJob(
			db::DbActionPtr(
				new CActivityByOrder(m_pCharOwner->ClientSlot(), m_pCharOwner->m_pActivityMan, actv, m_dwOwnerChaNum, m_pCharOwner->GetLevel())));
	}
	else
	{
		sc::writeLogError(std::string("ActivityManager::UpdateDBByOrder(): Invalid GLChar(m_pCharOwner) -> Load() was not be called properly."));
	}
}


/*
						  (copy)      (in GLChar)
Data  ->  ActivityBase  --------->  ActivityMananger  <-----------  DB

*/
