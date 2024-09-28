#include "../../pch.h"
#include "DbActionGameSNS.h"

#include "../../Server/AgentServer.h"
#include "../../Server/s_CFieldServer.h"

//#include "../RanClientLib/G-Logic/GLCharData.h"
//#include "../RanClientLib/G-Logic/GLogicEx.h"



// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------



//
//mjeon.sns
//


//
// FACEBOOK
//
CSNSAuthedFB::CSNSAuthedFB(DWORD dwClient, DWORD dwGaeaID, int nChaNum, char *pSKey, char *pUID)
:m_dwGaeaID(dwGaeaID)
{
	m_ClientSlot = dwClient;

	::StringCchCopy(m_AUTHFB.SKey, SNS_MAX_SKEY_LENGTH, pSKey);
	::StringCchCopy(m_AUTHFB.UID, SNS_MAX_UID_LENGTH, pUID);
	m_AUTHFB.ChaNum = nChaNum;
}


int CSNSAuthedFB::Execute(NetServer *pServer)
{
	if (pServer == NULL)
		return NET_ERROR;

	CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*> (pServer);


	if (m_pDbManager->SNSAuthedFB(m_AUTHFB) == sc::db::DB_ERROR)
	{
		GLMSG::SNET_SNS_FB_AUTHED_FB msg;
		msg.result = EMSNS_RES_DB_ERROR;
		pAgentServer->SendClient(m_ClientSlot, &msg);

		return NET_ERROR;
	}
	else
	{
        GLAgentServer* pGLAgentServer = pAgentServer->GetGLAgentServer();
		PGLCHARAG pChaAG = pGLAgentServer->GetChar(m_dwGaeaID);

		if ( !pChaAG )
		{
			GLMSG::SNET_SNS_FB_AUTHED_FB msg;
			msg.result = EMSNS_RES_CHA_NOTEXIST;
			pAgentServer->SendClient(m_ClientSlot, &msg);

			return NET_ERROR;
		}

		//
		//1.Update AgentS
		//
		pChaAG->m_AUTHFB = m_AUTHFB;


		//
		//2.Update FieldS - Send the information to Field for update.
		//
		GLMSG::SNET_SNS_FB_UPDATE_AF msgField;
		msgField.AUTHFB = m_AUTHFB;
		pAgentServer->SendField(m_ClientSlot, &msgField);


		//
		//3.Update Client - Send the posts to the Client
		//
		GLMSG::SNET_SNS_FB_AUTHED_FB msg;
		msg.result = EMSNS_RES_OK;
		pAgentServer->SendClient(m_ClientSlot, &msg);		
	}

	return NET_OK;
}



CSNSUnAuthedFB::CSNSUnAuthedFB(DWORD dwClient, DWORD dwGaeaID, int nChaNum)
:m_dwGaeaID(dwGaeaID)
{
	m_ClientSlot = dwClient;	
	m_nChaNum = nChaNum;
}


int CSNSUnAuthedFB::Execute(NetServer *pServer)
{
	if (pServer == NULL)
		return NET_ERROR;

	CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*> (pServer);


	if (m_pDbManager->SNSUnAuthedFB(m_nChaNum) == sc::db::DB_ERROR)
	{
		GLMSG::SNET_SNS_FB_UNAUTHED_FB msg;
		msg.result = EMSNS_RES_DB_ERROR;
		pAgentServer->SendClient(m_ClientSlot, &msg);

		return NET_ERROR;
	}
	else
	{
        GLAgentServer* pGLAgentServer = pAgentServer->GetGLAgentServer();
		PGLCHARAG pChaAG = pGLAgentServer->GetChar(m_dwGaeaID);

		if ( !pChaAG )
		{
			GLMSG::SNET_SNS_FB_UNAUTHED_FB msg;
			msg.result = EMSNS_RES_CHA_NOTEXIST;
			pAgentServer->SendClient(m_ClientSlot, &msg);

			return NET_ERROR;
		}

		//
		//1.Update AgentS
		//
		ZeroMemory(&pChaAG->m_AUTHFB, sizeof(pChaAG->m_AUTHFB));
		pChaAG->m_AUTHFB.ChaNum = m_nChaNum;


		//
		//2.Update FieldS - Send the information to Field for update.
		//
		GLMSG::SNET_SNS_FB_UPDATE_AF msgField;
		msgField.AUTHFB = pChaAG->m_AUTHFB;
		pAgentServer->SendField(m_ClientSlot, &msgField);


		//
		//3.Update Client - Send the posts to the Client
		//
		GLMSG::SNET_SNS_FB_UNAUTHED_FB msg;
		msg.result = EMSNS_RES_OK;
		pAgentServer->SendClient(m_ClientSlot, &msg);
	}

	return NET_OK;
}


CSNSGetUidFB::CSNSGetUidFB(DWORD dwClient, DWORD dwGaeaID, DWORD dwTargetGaeaID, int nTargetChaNum)
:m_dwGaeaID(dwGaeaID)
{
	m_ClientSlot = dwClient;
	m_TargetChaNum = nTargetChaNum;
	m_TargetGaeaID = dwTargetGaeaID;
}


int CSNSGetUidFB::Execute(NetServer *pServer)
{
	if (pServer == NULL)
		return NET_ERROR;

	CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*> (pServer);


	if (m_pDbManager->SNSGetUidFB(m_TargetChaNum, m_UID) == sc::db::DB_ERROR)	
	{
		GLMSG::SNET_SNS_FB_GETUID_FB msgFB;

		msgFB.result = EMSNS_RES_DB_ERROR;
		msgFB.dwTargetGaeaID = m_TargetGaeaID;		
		ZeroMemory(msgFB.TargetUID, sizeof(msgFB.TargetUID));

		pAgentServer->SendClient(m_ClientSlot, &msgFB);

		return NET_ERROR;
	}
	else
	{
		/*
		PGLCHARAG pChaAG = GLAgentServer::GetInstance().GetChar(m_dwGaeaID);

		if ( !pChaAG )
		{			
			GLMSG::SNET_SNS_FB_GETUID_FB msgFB;
			msgFB.result = EMSNS_RES_CHA_NOTEXIST;
			msgFB.dwTargetGaeaID = m_TargetChaNum;
			ZeroMemory(msgFB.TargetUID, sizeof(msgFB.TargetUID));

			pAgentServer->SendClient(dwClient, &msgFB);

			return NET_ERROR;
		}

		pChaAG->m_AUTHFB = m_AUTHFB;
		*/
		
		
		GLMSG::SNET_SNS_FB_GETUID_FB msgFB;		

		msgFB.result = EMSNS_RES_OK;
		msgFB.dwTargetGaeaID = m_TargetGaeaID;
		::StringCchCopy(msgFB.TargetUID,	SNS_MAX_UID_LENGTH, m_UID.c_str());

		pAgentServer->SendClient(m_ClientSlot, &msgFB);
	}

	return NET_OK;
}



//
// TWITTER
//
CSNSAuthedTW::CSNSAuthedTW(DWORD dwClient, DWORD dwGaeaID, int nChaNum, char *pAToken, char *pATokenS, char *pUID)
:m_dwGaeaID(dwGaeaID)
{
	m_ClientSlot = dwClient;

	::StringCchCopy(m_AUTHTW.AToken, SNS_MAX_TOKEN_LENGTH, pAToken);
	::StringCchCopy(m_AUTHTW.ATokenS, SNS_MAX_TOKEN_LENGTH, pATokenS);
	::StringCchCopy(m_AUTHTW.UID, SNS_MAX_UID_LENGTH, pUID);
	
	m_AUTHTW.ChaNum = nChaNum;
}


int CSNSAuthedTW::Execute(NetServer *pServer)
{
	if (pServer == NULL)
		return NET_ERROR;

	CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*> (pServer);


	if (m_pDbManager->SNSAuthedTW(m_AUTHTW) == sc::db::DB_ERROR)
	{
		GLMSG::SNET_SNS_TW_AUTHED_FB msg;
		msg.result = EMSNS_RES_DB_ERROR;
		pAgentServer->SendClient(m_ClientSlot, &msg);

		return NET_ERROR;
	}
	else
	{
        GLAgentServer* pGLAgentServer = pAgentServer->GetGLAgentServer();
		PGLCHARAG pChaAG = pGLAgentServer->GetChar(m_dwGaeaID);

		if ( !pChaAG )
		{
			GLMSG::SNET_SNS_TW_AUTHED_FB msg;
			msg.result = EMSNS_RES_CHA_NOTEXIST;
			pAgentServer->SendClient(m_ClientSlot, &msg);

			return NET_ERROR;
		}

		//
		//1.Update AgentS
		//
		pChaAG->m_AUTHTW = m_AUTHTW;


		//
		//2.Update FieldS - Send the information to Field for update.
		//
		GLMSG::SNET_SNS_TW_UPDATE_AF msgField;
		msgField.AUTHTW = m_AUTHTW;		
		pAgentServer->SendField(m_ClientSlot, &msgField);


		//
		//3.Update Client - Send the posts to the Client
		//
		GLMSG::SNET_SNS_TW_AUTHED_FB msg;
		msg.result = EMSNS_RES_OK;
		pAgentServer->SendClient(m_ClientSlot, &msg);
	}

	return NET_OK;
}



CSNSUnAuthedTW::CSNSUnAuthedTW(DWORD dwClient, DWORD dwGaeaID, int nChaNum)
:m_dwGaeaID(dwGaeaID)
{
	m_ClientSlot = dwClient;	
	m_nChaNum = nChaNum;
}


int CSNSUnAuthedTW::Execute(NetServer *pServer)
{
	if (pServer == NULL)
		return NET_ERROR;

	CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*> (pServer);


	if (m_pDbManager->SNSUnAuthedTW(m_nChaNum) == sc::db::DB_ERROR)
	{
		GLMSG::SNET_SNS_TW_UNAUTHED_FB msg;
		msg.result = EMSNS_RES_DB_ERROR;
		pAgentServer->SendClient(m_ClientSlot, &msg);

		return NET_ERROR;
	}
	else
	{
        GLAgentServer* pGLAgentServer = pAgentServer->GetGLAgentServer();
		PGLCHARAG pChaAG = pGLAgentServer->GetChar(m_dwGaeaID);

		if ( !pChaAG )
		{
			GLMSG::SNET_SNS_TW_UNAUTHED_FB msg;
			msg.result = EMSNS_RES_CHA_NOTEXIST;
			pAgentServer->SendClient(m_ClientSlot, &msg);

			return NET_ERROR;
		}

		//
		//1.Update AgentS
		//
		ZeroMemory(&pChaAG->m_AUTHTW, sizeof(pChaAG->m_AUTHTW));
		pChaAG->m_AUTHTW.ChaNum = m_nChaNum;


		//
		//2.Update FieldS - Send the information to Field for update.
		//
		GLMSG::SNET_SNS_TW_UPDATE_AF msgField;
		msgField.AUTHTW = pChaAG->m_AUTHTW;
		pAgentServer->SendField(m_ClientSlot, &msgField);


		//
		//3.Update Client - Send the posts to the Client
		//
		GLMSG::SNET_SNS_TW_UNAUTHED_FB msg;
		msg.result = EMSNS_RES_OK;
		pAgentServer->SendClient(m_ClientSlot, &msg);
	}

	return NET_OK;
}


CSNSGetUidTW::CSNSGetUidTW(DWORD dwClient, DWORD dwGaeaID, DWORD dwTargetGaeaID, int nTargetChaNum)
:m_dwGaeaID(dwGaeaID)
{
	m_ClientSlot = dwClient;
	m_TargetChaNum = nTargetChaNum;
	m_TargetGaeaID = dwTargetGaeaID;
}


int CSNSGetUidTW::Execute(NetServer *pServer)
{
	if (pServer == NULL)
		return NET_ERROR;

	CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*> (pServer);


	if (m_pDbManager->SNSGetUidTW(m_TargetChaNum, m_UID) == sc::db::DB_ERROR)	
	{
		GLMSG::SNET_SNS_TW_GETUID_FB msgFB;

		msgFB.result = EMSNS_RES_DB_ERROR;
		msgFB.dwTargetGaeaID = m_TargetGaeaID;		
		ZeroMemory(msgFB.TargetUID, sizeof(msgFB.TargetUID));

		pAgentServer->SendClient(m_ClientSlot, &msgFB);

		return NET_ERROR;
	}
	else
	{
		/*
		PGLCHARAG pChaAG = GLAgentServer::GetInstance().GetChar(m_dwGaeaID);

		if ( !pChaAG )
		{			
			GLMSG::SNET_SNS_FB_GETUID_FB msgFB;
			msgFB.result = EMSNS_RES_CHA_NOTEXIST;
			msgFB.dwTargetGaeaID = m_TargetChaNum;
			ZeroMemory(msgFB.TargetUID, sizeof(msgFB.TargetUID));

			pAgentServer->SendClient(dwClient, &msgFB);

			return NET_ERROR;
		}

		pChaAG->m_AUTHFB = m_AUTHFB;
		*/
		
		
		GLMSG::SNET_SNS_TW_GETUID_FB msgFB;		

		msgFB.result = EMSNS_RES_OK;
		msgFB.dwTargetGaeaID = m_TargetGaeaID;
		::StringCchCopy(msgFB.TargetUID,	SNS_MAX_UID_LENGTH, m_UID.c_str());

		pAgentServer->SendClient(m_ClientSlot, &msgFB);
	}

	return NET_OK;
}



//
// SNS Common
//

CSNSGetInfo::CSNSGetInfo(DWORD dwClient, DWORD dwGaeaID, int nChaNum)
:m_dwGaeaID(dwGaeaID)
{
	ZeroMemory(&m_AUTHFB, sizeof(m_AUTHFB));
	ZeroMemory(&m_AUTHTW, sizeof(m_AUTHTW));

	m_ClientSlot = dwClient;

	m_AUTHFB.ChaNum = nChaNum;
	m_AUTHTW.ChaNum = nChaNum;
}


int CSNSGetInfo::Execute(NetServer *pServer)
{
	if (pServer == NULL)
		return NET_ERROR;

	CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*> (pServer);


	if (m_pDbManager->SNSGetInfo(m_AUTHFB, m_AUTHTW) == sc::db::DB_ERROR)
	{
		GLMSG::SNET_SNS_GETINFO_FB msg;
		msg.result = EMSNS_RES_DB_ERROR;
		pAgentServer->SendClient(m_ClientSlot, &msg);

		return NET_ERROR;
	}
	else
	{
        GLAgentServer* pGLAgentServer = pAgentServer->GetGLAgentServer();
		PGLCHARAG pChaAG = pGLAgentServer->GetChar(m_dwGaeaID);

		if ( !pChaAG )
		{
			GLMSG::SNET_SNS_GETINFO_FB msg;
			msg.result = EMSNS_RES_CHA_NOTEXIST;
			pAgentServer->SendClient(m_ClientSlot, &msg);

			return NET_ERROR;
		}

		//
		//1.Update AgentS
		//
		pChaAG->m_AUTHFB = m_AUTHFB;
		pChaAG->m_AUTHTW = m_AUTHTW;


		//
		//2.Update FieldS - Send the information to Field for update.
		//
		if (strlen(m_AUTHFB.SKey) > 0 && strlen(m_AUTHFB.UID) > 0)
		{
			GLMSG::SNET_SNS_FB_UPDATE_AF msgField;
			msgField.AUTHFB = m_AUTHFB;
			pAgentServer->SendField(m_ClientSlot, &msgField);
		}


		if (strlen(m_AUTHTW.AToken) > 0 && strlen(m_AUTHTW.ATokenS) && strlen(m_AUTHTW.UID) > 0)
		{
			GLMSG::SNET_SNS_TW_UPDATE_AF msgField;
			msgField.AUTHTW = m_AUTHTW;
			pAgentServer->SendField(m_ClientSlot, &msgField);
		}


		//
		//3.Update Client - Send the posts to the Client
		//
		GLMSG::SNET_SNS_GETINFO_FB msg;		

		msg.result = EMSNS_RES_OK;

		::StringCchCopy(msg.FBSKey,		SNS_MAX_SKEY_LENGTH,	m_AUTHFB.SKey);
		::StringCchCopy(msg.FBUID,		SNS_MAX_UID_LENGTH,		m_AUTHFB.UID);

		::StringCchCopy(msg.TWAToken,	SNS_MAX_TOKEN_LENGTH,	m_AUTHTW.AToken);
		::StringCchCopy(msg.TWATokenS,	SNS_MAX_TOKEN_LENGTH,	m_AUTHTW.ATokenS);
		::StringCchCopy(msg.TWUID,		SNS_MAX_UID_LENGTH,		m_AUTHTW.UID);

		pAgentServer->SendClient(m_ClientSlot, &msg);
	}

	return NET_OK;
}

