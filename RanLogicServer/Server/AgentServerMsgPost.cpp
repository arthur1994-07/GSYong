#include "../pch.h"
#include <boost/tr1/memory.hpp>
#include "../../RanLogic/GLUseFeatures.h"

#include "../Character/CharNameCache.h"
#include "../Database/DBAction/DbActionGamePost.h"
#include "../Client/NetAgentClientMan.h"
#include "./AgentServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

//
//mjeon.Post
//

void CAgentServer::MsgPostReturn(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	if (!GLUseFeatures::GetInstance().IsUsingPost())
	{
		sc::writeLogError(std::string("MsgPostReturn() - Wrong use of postsystem while it is OFF!"));
		return;
	}

	GLMSG::SNET_POST_RETURN *pmsg = (GLMSG::SNET_POST_RETURN *)nmg;

	LONGLONG llPostID = pmsg->llPostID;

	GLCharAG* pChar = m_pGLAgentServer->GetChar(dwGaeaID);

	BOOL bFound = FALSE;

	if ( !pChar )
	{
		GLMSG::SNET_POST_RETURN_FB msgFB;

		msgFB.Result	= EMPOST_RECVER_NOTEXIST;
		msgFB.llPostID  = llPostID;

		SendClient(dwClient, &msgFB);

		return;
	}


	if ( pChar->m_pvPost->empty() )
	{
		GLMSG::SNET_POST_RETURN_FB msgFB;

		msgFB.Result	= EMPOST_EMPTY_AGENTSIDE;
		msgFB.llPostID  = llPostID;

		SendClient(dwClient, &msgFB);

		return;
	}

	VPOST &vp = *(pChar->m_pvPost);
	VPOST_ITER i;

	for (i = vp.begin(); i != vp.end(); i++)
	{
		if (llPostID == i->m_llPostID)
		{
			bFound = TRUE;
			break;
		}
	}

	if (bFound == FALSE)
	{
		GLMSG::SNET_POST_RETURN_FB msgFB;

		msgFB.Result	= EMPOST_POST_NOT_EXIST;
		msgFB.llPostID  = llPostID;

		SendClient(dwClient, &msgFB);

		return;
	}

	//
	// System Post cannot be returned. (Post of receving Charge for attach01)
	//
	if (i->m_byPostClass == EMPOST_SYSTEM)
	{
		GLMSG::SNET_POST_RETURN_FB msgFB;

		msgFB.Result	= EMPOST_SYSTEMPOST_RETURN;
		msgFB.llPostID  = llPostID;

		SendClient(dwClient, &msgFB);

		return;
	}

	//
	// Deleted Post and Returned Post cannnot be returned.
	//
	if ( i->m_byPostState != EMPOST_UNREAD && i->m_byPostState != EMPOST_READ )
	{
		GLMSG::SNET_POST_RETURN_FB msgFB;

		msgFB.Result	= EMPOST_INVALID_RETURN;
		msgFB.llPostID  = llPostID;

		SendClient(dwClient, &msgFB);

		return;
	}

	//
	// The post that it's attachment was taken cannot be returned.
	//
	if ( i->m_byMoneyAccept || i->m_byAttach01Accept )
	{
		GLMSG::SNET_POST_RETURN_FB msgFB;
		
		msgFB.Result	= EMPOST_EMPTY_RETURN;
		msgFB.llPostID	= llPostID;

		SendClient(dwClient, &msgFB);

		return;
	}


	SYSTEMTIME st;
	GetLocalTime(&st);	
	CTime cTime(st);	

    i->m_ReturnDate = cTime.GetTime();
	i->m_byPostState = EMPOST_RETURN;

	AddGameAdoJob(
        db::DbActionPtr(
            new CPostReturn(dwClient, llPostID)));
}

void CAgentServer::MsgPostDelete(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	if (!GLUseFeatures::GetInstance().IsUsingPost())
	{
		sc::writeLogError(std::string("MsgPostDelete() - Wrong use of postsystem while it is OFF!"));
		return;
	}

	GLMSG::SNET_POST_DELETE *pmsg = (GLMSG::SNET_POST_DELETE *)nmg;

	LONGLONG llPostID = pmsg->llPostID;

	AddGameAdoJob(
        db::DbActionPtr(
            new CPostDelete(dwClient, llPostID)));
}

void CAgentServer::MsgPostOpenAndTakeCA(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	if (!GLUseFeatures::GetInstance().IsUsingPost())
	{
		sc::writeLogError(std::string("MsgPostOpenAndTakeCA() - Wrong use of postsystem while it is OFF!"));
		return;
	}

	GLMSG::SNET_POST_OPEN_TAKE *pmsg = (GLMSG::SNET_POST_OPEN_TAKE *) nmg;

	GLCharAG* pChar = m_pGLAgentServer->GetChar(dwGaeaID);

	int iRecver				= pChar->ChaDbNum();//pmsg->iRecver;		// Recver's charID	
	LONGLONG llPostID		= pmsg->llPostID;
	
	BOOL bMoneyAccept		= pmsg->byMoneyAccept > 0;	// Byte -> BOOL
	BOOL bAttach01Accept	= pmsg->byAttach01Accept > 0;// Byte -> BOOL

	//GLCharAG* pChar = m_pGLAgentServer->GetCharByDbNum(iRecver);

	BOOL bFound = FALSE;
	
	if ( !pChar )
	{
		GLMSG::SNET_POST_OPEN_TAKE_FB msgFB;

		msgFB.byAttach01Accept	= (BYTE)FALSE;
		msgFB.byMoneyAccept		= (BYTE)FALSE;
		msgFB.result			= EMPOST_RECVER_NOTEXIST;

		SendClient(dwClient, &msgFB);

		return;
	}


	if ( pChar->m_pvPost->empty() )
	{
		GLMSG::SNET_POST_OPEN_TAKE_FB msgFB;

		msgFB.byAttach01Accept	= (BYTE)FALSE;
		msgFB.byMoneyAccept		= (BYTE)FALSE;
		msgFB.result			= EMPOST_POST_NOT_EXIST;

		SendClient(dwClient, &msgFB);

		return;
	}


	VPOST &vp = *(pChar->m_pvPost);
	VPOST_ITER i;

	for (i = vp.begin(); i != vp.end(); ++i)
	{
		if ( llPostID == i->m_llPostID )
		{
			bFound = TRUE;
			break;
		}
	}

	if (bFound == FALSE)
	{
		GLMSG::SNET_POST_OPEN_TAKE_FB msgFB;

		msgFB.byAttach01Accept	= (BYTE)FALSE;
		msgFB.byMoneyAccept		= (BYTE)FALSE;
		msgFB.result			= EMPOST_POST_NOT_EXIST;

		SendClient(dwClient, &msgFB);

		return;
	}
/*
#ifdef _RELEASED
	sc::writeLogInfo(sc::string::format("(TRACE#1)PostAttach - bMoney:%1%, bAttach01:%2%, i->bMoney:%3%, i->bAttach01:%4%", bMoneyAccept, bAttach01Accept, (BOOL)i->byMoneyAccept, (BOOL)i->byAttach01Accept));
#endif
*/
	if (bMoneyAccept)
	{
		if (i->m_byMoneyAccept > 0)	// Accept on already accepted money
		{
			GLMSG::SNET_POST_OPEN_TAKE_FB msgFB;

			msgFB.byAttach01Accept	= (BYTE)FALSE;
			msgFB.byMoneyAccept		= (BYTE)FALSE;
			msgFB.result			= EMPOST_REACCEPT_MONEY;

			i->m_byAttach01Accept		= (BYTE)TRUE;//mjeon.현재 시스템은 첨부 아이템과 첨부 게임머니를 동시에 취하게 되어 있다.

			SendClient(dwClient, &msgFB);

			return;
		}

		// update current post's status
		i->m_byMoneyAccept = (BYTE)TRUE;
	}

	if (bAttach01Accept)
	{
		if (i->m_byAttach01Accept > 0)	// Accept on already accepted item
		{
			GLMSG::SNET_POST_OPEN_TAKE_FB msgFB;

			msgFB.byAttach01Accept	= (BYTE)FALSE;
			msgFB.byMoneyAccept		= (BYTE)FALSE;
			msgFB.result			= EMPOST_REACCEPT_ITEM;

			i->m_byMoneyAccept		= (BYTE)TRUE;//mjeon.현재 시스템은 첨부 아이템과 첨부 게임머니를 동시에 취하게 되어 있다.

			SendClient(dwClient, &msgFB);

			return;
		}		

		// update current post's status
		i->m_byAttach01Accept = (BYTE)TRUE;//bAttach01Accept;
	}

/*
#ifdef _RELEASED
	sc::writeLogInfo(sc::string::format("(TRACE#2)PostAttach - bMoney:%1%, bAttach01:%2%, i->bMoney:%3%, i->bAttach01:%4%", bMoneyAccept, bAttach01Accept, (BOOL)i->byMoneyAccept, (BOOL)i->byAttach01Accept));
#endif
*/

	BYTE byPostState = i->m_byPostState;

	//
	// Deleted Post is accessed.
	//
	if ( byPostState == EMPOST_AUTO_DELETE || byPostState == EMPOST_USER_DELETE )
	{
		GLMSG::SNET_POST_OPEN_TAKE_FB msgFB;

		msgFB.result	= EMPOST_INVALID_ACCESS;
		msgFB.llPostID  = llPostID;

		SendClient(dwClient, &msgFB);

		return;
	}

	if ( byPostState != EMPOST_RETURN )
	{	
		i->m_byPostState = EMPOST_READ;
	}	


	GLMSG::SNET_POST_OPEN_TAKE_AF msg;

	msg.byMoneyAccept	= (BYTE)bMoneyAccept;//pmsg->bMoneyAccept;
	msg.byAttach01Accept = (BYTE)bAttach01Accept;//pmsg->bAttach01Accept;

	msg.aPOST = *i;	// Current status of the POST.

	//SendField(dwClient, &msg);
	SendFieldSvr(pChar->m_dwCurFieldSvr, &msg, pChar->GetChannel());
}


void CAgentServer::MsgPostOpen(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	if (!GLUseFeatures::GetInstance().IsUsingPost())
	{
		sc::writeLogError(std::string("MsgPostOpen() - Wrong use of postsystem while it is OFF!"));
		return;
	}

	GLMSG::SNET_POST_OPEN *pmsg = (GLMSG::SNET_POST_OPEN *) nmg;

	LONGLONG llPostID = pmsg->llPostID;

	GLCharAG* pChar = m_pGLAgentServer->GetChar(dwGaeaID);

	BOOL bFound = FALSE;

	if ( !pChar )
	{
		GLMSG::SNET_POST_OPEN_TAKE_FB msgFB;

		msgFB.byAttach01Accept	= (BYTE)FALSE;
		msgFB.byMoneyAccept		= (BYTE)FALSE;
		msgFB.result			= EMPOST_RECVER_NOTEXIST;

		SendField(dwClient, &msgFB);

		return;
	}


	if ( pChar->m_pvPost->empty() )
	{
		GLMSG::SNET_POST_OPEN_TAKE_FB msgFB;

		msgFB.byAttach01Accept	= (BYTE)FALSE;
		msgFB.byMoneyAccept		= (BYTE)FALSE;
		msgFB.result			= EMPOST_POST_NOT_EXIST;

		SendField(dwClient, &msgFB);

		return;
	}

	VPOST &vp = *(pChar->m_pvPost);
	VPOST_ITER i;

	for (i = vp.begin(); i != vp.end(); i++)
	{
		if ( llPostID == i->m_llPostID )
		{
			bFound = TRUE;
			break;
		}
	}

	if (bFound == FALSE)
	{
		GLMSG::SNET_POST_OPEN_TAKE_FB msgFB;

		msgFB.byAttach01Accept	= (BYTE)FALSE;
		msgFB.byMoneyAccept		= (BYTE)FALSE;
		msgFB.result			= EMPOST_POST_NOT_EXIST;

		SendField(dwClient, &msgFB);

		return;
	}

	BYTE byPostState = i->m_byPostState;

	AddGameAdoJob(
        db::DbActionPtr(
            new CPostOpen(llPostID)));
}


void CAgentServer::MsgPostNotify(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	if (!GLUseFeatures::GetInstance().IsUsingPost())
	{
		sc::writeLogError(std::string("MsgPostNotify() - Wrong use of postsystem while it is OFF!"));
		return;
	}

	//mjeon
	//Just Relay the msg directly.
	//
	//Because PostNotify does not need to access to DB.
	//
	m_pGLAgentServer->MsgPostNotify(nmg, dwClient, dwGaeaID);
}


void CAgentServer::MsgPostGet(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	if (!GLUseFeatures::GetInstance().IsUsingPost())
	{
		sc::writeLogError(std::string("MsgPostGet() - Wrong use of postsystem while it is OFF!"));
		return;
	}

	GLMSG::SNET_POST_GET *pmsg = (GLMSG::SNET_POST_GET *) nmg;

	GLCharAG* pChaAG = m_pGLAgentServer->GetChar(dwGaeaID);

	if (pChaAG == NULL)
		return;


	BOOL bGetOnlyInfo = (BOOL)pmsg->bGetOnlyInfo;
	int iRecver = pChaAG->ChaDbNum();//pmsg->iRecver;

	if ( !bGetOnlyInfo )
	{
		if ( !pChaAG->m_bPostGotData )
		{
			pChaAG->m_bPostGotData = TRUE;
		}
		else
		{
			GLMSG::SNET_POST_GET_DATA_FB msgFB;
			ZeroMemory ( &msgFB, sizeof (GLMSG::SNET_POST_GET_DATA_FB) );
			msgFB.nRowCount = POST_ON_PROCESSING;
			msgFB.nPostNum  = POST_ON_PROCESSING;
			SendClient(dwClient, &msgFB);			

			return;
		}


		DWORD *pdwLastPostGetData = &(pChaAG->m_dwLastPostGetData);
		DWORD dwCurrentPostGetData = ::GetTickCount();

		//
		// 1. Has the PostGetData cooltime passed since last postGetData ? (10 sec)
		//
		if ( *pdwLastPostGetData + GLCONST_POST::dwPOST_RECV_COOL_TIME_MS > dwCurrentPostGetData )
		{
			sc::writeLogError( std::string("[ERR] PostGetData: PostGetData cooltime remains") );

			GLMSG::SNET_POST_GET_DATA_FB msgFB;
			ZeroMemory ( &msgFB, sizeof (GLMSG::SNET_POST_GET_INFO_FB) );
			msgFB.nRowCount = POST_COOTIME_REMAINS;
			msgFB.nPostNum  = POST_COOTIME_REMAINS;		

			SendClient(dwClient, &msgFB);

			return;
		}

		//
		// mjeon.post
		// cooltime reset
		//
		*pdwLastPostGetData = dwCurrentPostGetData;	// for cooltime calculation
	}	

	AddGameAdoJob(
        db::DbActionPtr(
            new CPostGet(iRecver, dwClient, dwGaeaID, bGetOnlyInfo, pChaAG->m_pvPost, pChaAG->UserDbNum())));
}

void CAgentServer::MsgPostGetState(DWORD dwClient, DWORD dwGaeaID,  NET_MSG_GENERIC* nmg)
{
	if (!GLUseFeatures::GetInstance().IsUsingPost())
	{
		sc::writeLogError(std::string("MsgPostGetState() - Wrong use of postsystem while it is OFF!"));
		return;
	}

	GLMSG::SNET_POST_GET_STATE *pmsg = (GLMSG::SNET_POST_GET_STATE *) nmg;

	GLCharAG* pChaAG = m_pGLAgentServer->GetChar(dwGaeaID);

	if (pChaAG == NULL)
		return;


	DWORD *pdwLastPostGetState = &(pChaAG->m_dwLastPostGetState);
	DWORD dwCurrentPostGetState = ::GetTickCount();

	//
	// 1. Has the PostGetState cooltime passed since last postGetState ? (10 sec)
	//
	if ( *pdwLastPostGetState + GLCONST_POST::dwPOST_RECV_COOL_TIME_MS > dwCurrentPostGetState )
	{
		sc::writeLogError( std::string("[ERR] PostGetData: PostGetData cooltime remains") );

		GLMSG::SNET_POST_GET_STATE_FB msgFB;
		ZeroMemory ( &msgFB, sizeof (GLMSG::SNET_POST_GET_STATE_FB) );
		msgFB.nRowCount = POST_COOTIME_REMAINS;
		msgFB.nPostNum  = POST_COOTIME_REMAINS;		

		SendClient(dwClient, &msgFB);

		return;
	}

	
	int iRecver = pmsg->iRecver;

	if ( !pChaAG->m_bPostGotState )
	{
		pChaAG->m_bPostGotState = TRUE;
	}
	else
	{
		GLMSG::SNET_POST_GET_STATE_FB msgFB;
		ZeroMemory ( &msgFB, sizeof (GLMSG::SNET_POST_GET_STATE_FB) );
		msgFB.nRowCount = POST_ON_PROCESSING;
		msgFB.nPostNum  = POST_ON_PROCESSING;
		SendClient(dwClient, &msgFB);

		return;
	}

	//
	// mjeon.post
	// cooltime reset
	//
	*pdwLastPostGetState = dwCurrentPostGetState;	// for cooltime calculation

	AddGameAdoJob(
        db::DbActionPtr(
            new CPostGetState(iRecver, dwClient, pChaAG->ChaDbNum())));
}


void CAgentServer::MsgPostGetSendState(DWORD dwClient, DWORD dwGaeaID,  NET_MSG_GENERIC* nmg)
{
    if (!GLUseFeatures::GetInstance().IsUsingPost())
    {
        sc::writeLogError(std::string("MsgPostGetSendState() - Wrong use of postsystem while it is OFF!"));
        return;
    }

    GLMSG::SNET_POST_GET_SEND_STATE *pmsg = (GLMSG::SNET_POST_GET_SEND_STATE *) nmg;

    GLCharAG* pChaAG = m_pGLAgentServer->GetChar(dwGaeaID);

    if (pChaAG == NULL)
        return;


    DWORD *pdwLastPostGetState = &(pChaAG->m_dwLastPostGetSendState);
    DWORD dwCurrentPostGetState = ::GetTickCount();

    //
    // 1. Has the PostGetState cooltime passed since last postGetState ? (10 sec)
    //
    if ( *pdwLastPostGetState + GLCONST_POST::dwPOST_RECV_COOL_TIME_MS > dwCurrentPostGetState )
    {
        sc::writeLogError( std::string("[ERR] PostGetSendData: PostGetSendData cooltime remains") );

        GLMSG::SNET_POST_GET_SEND_STATE_FB msgFB;
        ZeroMemory ( &msgFB, sizeof (GLMSG::SNET_POST_GET_SEND_STATE_FB) );
        msgFB.nRowCount = POST_COOTIME_REMAINS;
        msgFB.nPostNum  = POST_COOTIME_REMAINS;		

        SendClient(dwClient, &msgFB);

        return;
    }


    int iRecver = pmsg->iRecver;

    if ( !pChaAG->m_bPostGotSendState )
    {
        pChaAG->m_bPostGotSendState = TRUE;
    }
    else
    {
        GLMSG::SNET_POST_GET_SEND_STATE_FB msgFB;
        ZeroMemory ( &msgFB, sizeof (GLMSG::SNET_POST_GET_SEND_STATE_FB) );
        msgFB.nRowCount = POST_ON_PROCESSING;
        msgFB.nPostNum  = POST_ON_PROCESSING;
        SendClient(dwClient, &msgFB);

        return;
    }

    //
    // mjeon.post
    // cooltime reset
    //
    *pdwLastPostGetState = dwCurrentPostGetState;	// for cooltime calculation

    AddGameAdoJob(
        db::DbActionPtr(
        new CPostGetSendState(iRecver, dwClient, pChaAG->ChaDbNum())));
}



void CAgentServer::MsgPostGetRecvState(DWORD dwClient, DWORD dwGaeaID,  NET_MSG_GENERIC* nmg)
{
    if (!GLUseFeatures::GetInstance().IsUsingPost())
    {
        sc::writeLogError(std::string("MsgPostGetRecvState() - Wrong use of postsystem while it is OFF!"));
        return;
    }

    GLMSG::SNET_POST_GET_RECV_STATE *pmsg = (GLMSG::SNET_POST_GET_RECV_STATE *) nmg;

    GLCharAG* pChaAG = m_pGLAgentServer->GetChar(dwGaeaID);

    if (pChaAG == NULL)
        return;


    DWORD *pdwLastPostGetState = &(pChaAG->m_dwLastPostGetRecvState);
    DWORD dwCurrentPostGetState = ::GetTickCount();

    //
    // 1. Has the PostGetState cooltime passed since last postGetState ? (10 sec)
    //
    if ( *pdwLastPostGetState + GLCONST_POST::dwPOST_RECV_COOL_TIME_MS > dwCurrentPostGetState )
    {
        sc::writeLogError( std::string("[ERR] PostGetRecvData: PostGetRecvData cooltime remains") );

        GLMSG::SNET_POST_GET_RECV_STATE_FB msgFB;
        ZeroMemory ( &msgFB, sizeof (GLMSG::SNET_POST_GET_RECV_STATE_FB) );
        msgFB.nRowCount = POST_COOTIME_REMAINS;
        msgFB.nPostNum  = POST_COOTIME_REMAINS;		

        SendClient(dwClient, &msgFB);

        return;
    }


    int iRecver = pmsg->iRecver;

    if ( !pChaAG->m_bPostGotRecvState )
    {
        pChaAG->m_bPostGotRecvState = TRUE;
    }
    else
    {
        GLMSG::SNET_POST_GET_RECV_STATE_FB msgFB;
        ZeroMemory ( &msgFB, sizeof (GLMSG::SNET_POST_GET_RECV_STATE_FB) );
        msgFB.nRowCount = POST_ON_PROCESSING;
        msgFB.nPostNum  = POST_ON_PROCESSING;
        SendClient(dwClient, &msgFB);

        return;
    }

    //
    // mjeon.post
    // cooltime reset
    //
    *pdwLastPostGetState = dwCurrentPostGetState;	// for cooltime calculation

    AddGameAdoJob(
        db::DbActionPtr(
        new CPostGetRecvState(iRecver, dwClient, pChaAG->ChaDbNum())));
}



//
//mjeon.Post.ChaNum
//Req: Client -> Server
//
void CAgentServer::MsgReqChaNum(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	if (!GLUseFeatures::GetInstance().IsUsingPost())
	{
		sc::writeLogError(std::string("MsgReqChaNum() - Wrong use of postsystem while it is OFF!"));
		return;
	}


	DWORD dwSndBytes = 0;
	
	GLMSG::SNET_POST_REQ_CHANUM *pmsg = (GLMSG::SNET_POST_REQ_CHANUM *) nmg;

	int nReqID = pmsg->nReqID;


	//INT nUsrNum = 0;
	//INT nSvrGrp = 0;

	//GLMSG::SNET_POST_REQ_CHANUM msg *pmsg = (GLMSG::SNET_POST_REQ_CHANUM msg *)pMsg;	


	// 인증을 통과하지 않은 상태에서 캐릭터 정보요청함
	if (m_pClientManager->IsAccountPass(dwClient) == false)
	{
		sc::writeLogInfo(std::string("INFO:MsgSndChaBasicInfo IsAccountPass false"));
		CloseClient(dwClient, net::KICKOUT_UNDEFINE);
		return;
	}
	

	//int nUserNum = m_pClientManager->GetUserNum(dwClient);
    if (m_pGLAgentServer && m_pAdoManager)
    {
        std::tr1::shared_ptr<CHAR_BASE_CACHE_DATA> spCacheData = m_pGLAgentServer->GetCharCacheData(pmsg->szChaName);        
        if (spCacheData)
        {
            // Cache 에 정보가 있다면 바로 보낸다
            if (spCacheData->UserType() >= USER_USER_GM)
                MsgReqChaNumBack(dwClient, dwGaeaID, -1, -1, nReqID);	// GM 에게는 보낼수 없다
            else
                MsgReqChaNumBack(dwClient, dwGaeaID, spCacheData->ChaDbNum(), spCacheData->UserDbNum(), nReqID);
        }
        else
        {
            // Cache 에 정보가 없다면 DB 에 정보를 요청한다
            AddGameAdoJob(
                db::DbActionPtr(
                    new CGetChaNum(dwClient, dwGaeaID, pmsg->szChaName, nReqID))); //m_pClientManager->GetClientIP(dwClient), m_pClientManager->GetClientPort(dwClient)
        }
    }
}

void CAgentServer::MsgReqChaNumBack(DWORD SenderClientSlot, DWORD SenderGaeaId, int ReceiverChaDbNum, int ReceiverUserDbNum, int nReqID)
{
	if (!GLUseFeatures::GetInstance().IsUsingPost())
	{
		sc::writeLogError(std::string("MsgReqChaNumBack() - Wrong use of postsystem while it is OFF!"));
		return;
	}

	//DWORD dwSndBytes = 0;

	GLMSG::SNET_POST_REQ_CHANUM_FB msg;
	msg.nReqID   = nReqID;
	msg.nChaNum  = ReceiverChaDbNum;
	msg.nUserNum = ReceiverUserDbNum;	
	SendClient(SenderClientSlot, &msg);
}

//! 캐릭터 이름/캐릭터 DB 번호, 캐릭터 DB 번호/캐릭터 이름
void CAgentServer::ChaNameDbNumAdd(const std::string& ChaName, DWORD ChaDbNum, DWORD UserDbNum, EMUSERTYPE UserType)
{
    if (m_pGLAgentServer)
		m_pGLAgentServer->ChaNameDbNumAdd(ChaName, ChaDbNum, UserDbNum, UserType );
}

void CAgentServer::ChaNameDbNumAdd(const std::string& ChaName, DWORD ChaDbNum, DWORD UserDbNum, EMUSERTYPE UserType, int iClass, int iLv)
{
	if (m_pGLAgentServer)
		m_pGLAgentServer->ChaNameDbNumAdd(ChaName, ChaDbNum, UserDbNum, UserType, iClass, iLv );
}