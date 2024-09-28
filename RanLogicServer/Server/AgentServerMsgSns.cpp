#include "../pch.h"

#include "../../RanLogic/GLUseFeatures.h"
#include "../Database/DBAction/DbActionGameSNS.h"
#include "../Client/NetAgentClientMan.h"
#include "./AgentServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

//
//mjeon.SNS
//


//
// Facebook
//

void CAgentServer::MsgSNSFBAuthed(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if (!GLUseFeatures::GetInstance().IsUsingSNS())
    {
        sc::writeLogError(std::string("MsgSNSFBAuthed() - Wrong use of SNS-system while it is OFF!"));
        return;
    }

    GLMSG::SNET_SNS_FB_AUTHED *pmsg = (GLMSG::SNET_SNS_FB_AUTHED *)nmg;

    GLCharAG* pChaAG = m_pGLAgentServer->GetChar(dwGaeaID);

    int nChaNum = m_pClientManager->ChaDbNum(dwClient);


    if ( !pChaAG )
    {
        GLMSG::SNET_SNS_FB_AUTHED_FB msgFB;
        msgFB.result = EMSNS_RES_CHA_NOTEXIST;

        SendClient(dwClient, &msgFB);

        return;
    }

    if( strlen(pmsg->SKey) <= 0 )
    {
        GLMSG::SNET_SNS_FB_AUTHED_FB msgFB;
        msgFB.result = EMSNS_RES_WRONG_SKEY;

        SendClient(dwClient, &msgFB);		

        return;
    }

    if( strlen(pmsg->UID) <= 0 )
    {
        GLMSG::SNET_SNS_FB_AUTHED_FB msgFB;
        msgFB.result = EMSNS_RES_WRONG_UID;

        SendClient(dwClient, &msgFB);

        return;
    }
    
    AddGameAdoJob(
        db::DbActionPtr(
            new CSNSAuthedFB(dwClient, dwGaeaID, nChaNum, pmsg->SKey, pmsg->UID)));
}


void CAgentServer::MsgSNSFBUnAuthed(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if (!GLUseFeatures::GetInstance().IsUsingSNS())
    {
        sc::writeLogError(std::string("MsgSNSFBUnAuthed() - Wrong use of SNS-system while it is OFF!"));
        return;
    }

    GLMSG::SNET_SNS_FB_UNAUTHED *pmsg = (GLMSG::SNET_SNS_FB_UNAUTHED *)nmg;

    GLCharAG* pChaAG = m_pGLAgentServer->GetChar(dwGaeaID);

    int nChaNum = m_pClientManager->ChaDbNum(dwClient);


    if ( !pChaAG )
    {
        GLMSG::SNET_SNS_FB_UNAUTHED_FB msgFB;
        msgFB.result = EMSNS_RES_CHA_NOTEXIST;

        SendClient(dwClient, &msgFB);

        return;
    }

    AddGameAdoJob(
        db::DbActionPtr(
            new CSNSUnAuthedFB(dwClient, dwGaeaID,nChaNum)));
}


void CAgentServer::MsgSNSFBGetUID(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if (!GLUseFeatures::GetInstance().IsUsingSNS())
    {
        sc::writeLogError(std::string("MsgSNSFBGetUID() - Wrong use of SNS-system while it is OFF!"));
        return;
    }

    GLMSG::SNET_SNS_FB_GETUID *pmsg = (GLMSG::SNET_SNS_FB_GETUID *)nmg;


    DWORD dwTargetGaeaID = pmsg->dwTargetGaeaID;
    GLCharAG* pTargetChaAG = m_pGLAgentServer->GetChar(dwTargetGaeaID);

    if ( !pTargetChaAG )
    {
        GLMSG::SNET_SNS_FB_GETUID_FB msgFB;
        msgFB.result = EMSNS_RES_TARGET_NOTEXIST;

        SendClient(dwClient, &msgFB);

        return;
    }

    DWORD dwTargetClientID = pTargetChaAG->ClientSlot();
    int nTargetChaNum = m_pClientManager->ChaDbNum(dwTargetClientID);

    AddGameDbJob(
        db::DbActionPtr(
            new CSNSGetUidFB(dwClient, dwGaeaID, dwTargetGaeaID, nTargetChaNum)));
}



//
// Twitter
//

void CAgentServer::MsgSNSTWAuthed(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if (!GLUseFeatures::GetInstance().IsUsingSNS())
    {
        sc::writeLogError(std::string("MsgSNSTWAuthed() - Wrong use of SNS-system while it is OFF!"));
        return;
    }

    GLMSG::SNET_SNS_TW_AUTHED *pmsg = (GLMSG::SNET_SNS_TW_AUTHED *)nmg;

    GLCharAG* pChaAG = m_pGLAgentServer->GetChar(dwGaeaID);

    int nChaNum = m_pClientManager->ChaDbNum(dwClient);


    if ( !pChaAG )
    {
        GLMSG::SNET_SNS_TW_AUTHED_FB msgFB;
        msgFB.result = EMSNS_RES_CHA_NOTEXIST;

        SendClient(dwClient, &msgFB);

        return;
    }

    if( strlen(pmsg->AToken) <= 0 )
    {
        GLMSG::SNET_SNS_TW_AUTHED_FB msgFB;
        msgFB.result = EMSNS_RES_WRONG_ATOKEN;

        SendClient(dwClient, &msgFB);		

        return;
    }

    if( strlen(pmsg->ATokenS) <= 0 )
    {
        GLMSG::SNET_SNS_TW_AUTHED_FB msgFB;
        msgFB.result = EMSNS_RES_WRONG_ATOKENS;

        SendClient(dwClient, &msgFB);		

        return;
    }

    if( strlen(pmsg->UID) <= 0 )
    {
        GLMSG::SNET_SNS_TW_AUTHED_FB msgFB;
        msgFB.result = EMSNS_RES_WRONG_UID;

        SendClient(dwClient, &msgFB);

        return;
    }

    AddGameAdoJob(
        db::DbActionPtr(
            new CSNSAuthedTW(dwClient, dwGaeaID, nChaNum, pmsg->AToken, pmsg->ATokenS, pmsg->UID)));
}

void CAgentServer::MsgSNSTWUnAuthed(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if (!GLUseFeatures::GetInstance().IsUsingSNS())
    {
        sc::writeLogError(std::string("MsgSNSTWUnAuthed() - Wrong use of SNS-system while it is OFF!"));
        return;
    }

    GLMSG::SNET_SNS_TW_UNAUTHED *pmsg = (GLMSG::SNET_SNS_TW_UNAUTHED *)nmg;

    GLCharAG* pChaAG = m_pGLAgentServer->GetChar(dwGaeaID);

    int nChaNum = m_pClientManager->ChaDbNum(dwClient);


    if ( !pChaAG )
    {
        GLMSG::SNET_SNS_TW_UNAUTHED_FB msgFB;
        msgFB.result = EMSNS_RES_CHA_NOTEXIST;

        SendClient(dwClient, &msgFB);

        return;
    }

    AddGameAdoJob(
        db::DbActionPtr(
            new CSNSUnAuthedTW(dwClient, dwGaeaID,nChaNum)));
}


void CAgentServer::MsgSNSTWGetUID(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if (!GLUseFeatures::GetInstance().IsUsingSNS())
    {
        sc::writeLogError(std::string("MsgSNSTWGetUID() - Wrong use of SNS-system while it is OFF!"));
        return;
    }

    GLMSG::SNET_SNS_TW_GETUID* pmsg = (GLMSG::SNET_SNS_TW_GETUID*) nmg;


    DWORD dwTargetGaeaID = pmsg->dwTargetGaeaID;
    GLCharAG* pTargetChaAG = m_pGLAgentServer->GetChar(dwTargetGaeaID);

    if ( !pTargetChaAG )
    {
        GLMSG::SNET_SNS_TW_GETUID_FB msgFB;
        msgFB.result = EMSNS_RES_TARGET_NOTEXIST;

        SendClient(dwClient, &msgFB);

        return;
    }

    DWORD dwTargetClientID = pTargetChaAG->ClientSlot();
    int nTargetChaNum = m_pClientManager->ChaDbNum(dwTargetClientID);

    AddGameDbJob(
        db::DbActionPtr(
            new CSNSGetUidTW(dwClient, dwGaeaID, dwTargetGaeaID, nTargetChaNum)));
}


//
// SNS Common
//

void CAgentServer::MsgSNSGetInfo(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if (!GLUseFeatures::GetInstance().IsUsingSNS())
    {
        //sc::writeLogError(std::string("MsgSNSGetInfo() - Wrong use of SNS-system while it is OFF!"));
        return;
    }

    GLMSG::SNET_SNS_GETINFO *pmsg = (GLMSG::SNET_SNS_GETINFO *)nmg;

    GLCharAG* pChaAG = m_pGLAgentServer->GetChar(dwGaeaID);

    int nChaNum = m_pClientManager->ChaDbNum(dwClient);


    if ( !pChaAG )
    {
        GLMSG::SNET_SNS_GETINFO_FB msgFB;
        msgFB.result = EMSNS_RES_CHA_NOTEXIST;

        SendClient(dwClient, &msgFB);

        return;
    }

    AddGameDbJob(
        db::DbActionPtr(
            new CSNSGetInfo(dwClient, dwGaeaID, nChaNum)));
}
