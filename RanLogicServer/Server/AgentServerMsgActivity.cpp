#include "../pch.h"

#include "../../RanLogic/GLUseFeatures.h"
#include "./AgentServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void CAgentServer::MsgActivityCompletedFA(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if (!GLUseFeatures::GetInstance().IsUsingActivity())
    {
        sc::writeLogError(std::string("MsgActivityCompletedFA() - Wrong use of Activity while it is OFF!"));
        return;
    }

    GLMSG::SNET_ACTIVITY_COMPLETED_FA *pmsg = (GLMSG::SNET_ACTIVITY_COMPLETED_FA *)nmg;

// sckim : Activity
//sc::writeLogError(sc::string::format("CAgentServer::MsgActivityCompletedFA, pmsg->dwNotifyLevel : %1%", pmsg->dwNotifyLevel));

    switch( pmsg->dwNotifyLevel )
    {
    case ACTIVITY_NOTIFY_BROADCAST:
        {
            GLCharAG* pCharAG = m_pGLAgentServer->GetCharByDbNum(pmsg->dwChaNumCloser);

            if (!pCharAG)
            {
                // not an error. just ignore this notify.
                sc::writeLogWarn(std::string("CAgentServer::MsgActivityCompletedFA: Character does not exist."));

                break;
            }

// sckim : Activity
//sc::writeLogError(std::string("CAgentServer::MsgActivityCompletedFA: ACTIVITY_NOTIFY_BROADCAST."));

            GLMSG::SNET_ACTIVITY_NOTIFY_COMPLETED msg;	//to Clients
            msg.dwActivityID  = pmsg->dwActivityID;			
            StringCchCopy(msg.szChaName, CHR_ID_LENGTH+1, pCharAG->m_szName);

            SendAllClient(&msg);

            break;
        }
    case ACTIVITY_NOTIFY_CLUB:
        {
// sckim : Activity
//sc::writeLogError(std::string("CAgentServer::MsgActivityCompletedFA: ACTIVITY_NOTIFY_CLUB."));
            //relay to GLAgentServer
            m_pGLAgentServer->MsgActivityCompletedFA(nmg, dwClient, dwGaeaID);


            break;
        }
    case ACTIVITY_NOTIFY_CLUBANDAROUND:
        {
// sckim : Activity
//sc::writeLogError(std::string("CAgentServer::MsgActivityCompletedFA: ACTIVITY_NOTIFY_CLUBANDAROUND."));
            //
            //Around-messages were sent by DbAction.
            //

            //relay to GLAgentServer
            m_pGLAgentServer->MsgActivityCompletedFA(nmg, dwClient, dwGaeaID);

            break;
        }
    default:	//WRONG!!
        {
            sc::writeLogError(std::string("Wrong ActivityCompleted Notify-Type !!"));
            break;
        }
    }
}


//
// Closed-Notify MUST be broadcast to sync the activity. 
//
void CAgentServer::MsgActivityClosedFA(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if (!GLUseFeatures::GetInstance().IsUsingActivity())
    {
        sc::writeLogError(std::string("MsgActivityClosedFA() - Wrong use of Activity while it is OFF!"));
        return;
    }

    GLMSG::SNET_ACTIVITY_CLOSED_FA *pmsg = (GLMSG::SNET_ACTIVITY_CLOSED_FA *)nmg;

    GLCharAG* pCharAG = m_pGLAgentServer->GetCharByDbNum(pmsg->dwChaNumCloser);

    GLMSG::SNET_ACTIVITY_NOTIFY_CLOSED msg;	//to Clients
    msg.dwActivityID  = pmsg->dwActivityID;


    if (!pCharAG)
    {
        // not an error. just ignore this notify.
        sc::writeLogWarn(std::string("CAgentServer::MsgActivityCompletedFA: Character does not exist."));

        //
        //in case of ActivityClosed, the notify msg MUST be sent to all client even though there's no closer-character online.
        // in this case, client should ignore the character name in the msg.
        //
        StringCchCopy(msg.szChaName, CHR_ID_LENGTH+1, "");
    }
    else
    {
        StringCchCopy(msg.szChaName, CHR_ID_LENGTH+1, pCharAG->m_szName);
    }	

    //
    //just broadcast it.
    //
    SendAllClient(&msg);
}
