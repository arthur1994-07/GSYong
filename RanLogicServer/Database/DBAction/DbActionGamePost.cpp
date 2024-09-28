#include "../../pch.h"

#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/GLogicDataMan.h"

#include "../../RanLogic/Msg/PostRnMsg.h"

#include "../../Server/AgentServer.h"
#include "../../Server/s_CFieldServer.h"
#include "../../FieldServer/GLGaeaServer.h"
#include "DbActionGamePost.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

//
//mjeon.Post
//
CPostOpenAndTake::CPostOpenAndTake( GLMSG::SNET_POST_OPEN_TAKE_AF *msg /*current post status*/, BOOL bMoneyAccept /*user's request*/, BOOL bAttach01Accept /*user's request*/, DWORD dwOpenAndTakeUserNum )
    :m_aPOST(msg->aPOST)
    ,m_bMoneyAccept(bMoneyAccept)
    ,m_bAttach01Accept(bAttach01Accept)
	,m_dwOpenAndTakeUserNum( dwOpenAndTakeUserNum )
{
/*
#ifdef _RELEASED
	sc::writeLogInfo(sc::string::format("(TRACE#6)PostAttach - bMoney:%1%, bAttach01:%2% in DbAction", bMoneyAccept, bAttach01Accept));
#endif
*/
}


int CPostOpenAndTake::Execute(NetServer *pServer)
{
    CFieldServer* pFieldServer = reinterpret_cast<CFieldServer*> (pServer);
    if (!pFieldServer)
        return NET_ERROR;

    GLGaeaServer* pGaeaServer = pFieldServer->GetGLGaeaServer();
    if (!pGaeaServer)
        return NET_ERROR;

    if (m_pDbManager->GetConnectionType() != db::ADO)
    {
        sc::writeLogError("CPostOpenAndTake::Execute. Use ADO");
        return NET_ERROR;
    }

	if (pServer == NULL)
	{
		GLMSG::SNET_POST_OPEN_TAKE_AF_FB msg;

		msg.llPostID = m_aPOST.m_llPostID;
		msg.iRecver = m_aPOST.m_iRecver;
		msg.byAttach01Accept= (BYTE)FALSE;
		msg.byMoneyAccept	= (BYTE)FALSE;
		msg.result			= EMPOST_ETC;

		pGaeaServer->SENDTOAGENT(&msg);

		return FALSE;
	}

    if (m_bAttach01Accept)
    {
        if (m_aPOST.m_llCharge > 0)
        {
            if ( m_aPOST.m_byPostState != EMPOST_RETURN )
            {
                m_pDbManager->PostSendCharge( m_aPOST, m_dwOpenAndTakeUserNum );
            }
        }
    }

	if ( m_bMoneyAccept || m_bAttach01Accept )
	{
		BOOL bChargePayment = (BOOL)( m_bAttach01Accept && (m_aPOST.m_llCharge > (LONGLONG)0 ) );

		m_pDbManager->PostOpenAndTake(m_aPOST.m_llPostID, m_bMoneyAccept, m_bAttach01Accept, (BYTE)bChargePayment, (m_aPOST.m_byPostState == EMPOST_RETURN) );
	}

	return TRUE;
}


//
//mjeon.Post.Send
//DBAction for PostSend
//
CPostSend::CPostSend( DWORD dwClientID, SPOSTINFO PostInfo, const SITEMCUSTOM& sItem )
:m_PostInfo(PostInfo)
,m_sItem(sItem)
{
	m_ClientSlot = dwClientID;
}


int CPostSend::Execute(NetServer* pServer)
{
    CFieldServer* pFieldServer = reinterpret_cast<CFieldServer*> (pServer);
    if (!pFieldServer)
        return NET_ERROR;

    GLGaeaServer* pGaeaServer = pFieldServer->GetGLGaeaServer();
    if (!pGaeaServer)
        return NET_ERROR;

    if (m_pDbManager->GetConnectionType() != db::ADO)
    {
        sc::writeLogError("CPostSend::Execute. Use ADO");
        return NET_ERROR;
    }

	int nReturn = 0;
	TCHAR szQuery[256] = {0,};

	if (m_PostInfo.bAttach01)
	{
		if (m_sItem.GetNativeID() == NATIVEID_NULL())
		{
			sc::writeLogError(std::string("CPostSend(): m_pItem(Attach01) is NULL."));
			return sc::db::DB_ERROR;
		}
	}

	nReturn = m_pDbManager->PostSend(m_PostInfo, m_sItem);

	GLMSG::SNET_POST_TODO_AFTER_SEND_FF msg;
	msg.nReturn	 = nReturn;
	msg.postinfo = m_PostInfo;
	pFieldServer->InsertMsg(m_ClientSlot, &msg);

	return nReturn;
}


//
//mjeon.Post.Send
//DBAction for PostSend
//
CPostSendRollBack::CPostSendRollBack(LONGLONG &llPostID)
    : m_llPostID(llPostID)
{	
}

CPostSendRollBack::~CPostSendRollBack()
{
}

int CPostSendRollBack::Execute(NetServer* pServer)
{
    if (m_pDbManager->GetConnectionType() != db::ADO)
    {
        sc::writeLogError("CPostSendRollBack::Execute. Use ADO");
        return NET_ERROR;
    }

	int nReturn = m_pDbManager->PostSendRollBack(m_llPostID);
	return nReturn;
}


//
//mjeon.Post
//
CPostMaintenance::CPostMaintenance ()
{
	//Current Time
	__time64_t currentTime = CTime::GetCurrentTime().GetTime();				
	CTime cCurrentTime(currentTime);
	CString CurrentDate = cCurrentTime.Format("%Y-%m-%d %H:%M:%S");
	StringCchCopy(m_szCurrentDate, sizeof(m_szCurrentDate), CurrentDate.GetString());
}


int CPostMaintenance::Execute(NetServer *pServer)
{
    if (m_pDbManager->GetConnectionType() != db::ADO)
    {
        sc::writeLogError("CPostMaintenance::Execute use ADO");
        return NET_ERROR;
    }

	if (m_pDbManager->PostMaintenance() == sc::db::DB_ERROR)
	{
		sc::writeLogError(sc::string::format("PostMaintenance failure at (%1%)", m_szCurrentDate));
		return NET_ERROR;
	}
	else
	{
		sc::writeLogInfo(sc::string::format("OK:PostMaintenance succeed at (%1%)", m_szCurrentDate));

		// DO CleanUP!
        m_pDbManager->AddGameJob(
            db::DbActionPtr(
                new CPostClean));
	}
	return NET_OK;
}


//
//mjeon.Post
//
//PostClean will be called in CPostMaintenance after processing unhandled chargement.
//
CPostClean::CPostClean()
{
	// Because the time on DB server could be different from Agent(or Field),
	// the Time value MUST be o

	//Current Time
	__time64_t currentTime = CTime::GetCurrentTime().GetTime();				
	CTime cCurrentTime(currentTime);
	CString CurrentDate = cCurrentTime.Format("%Y-%m-%d %H:%M:%S");
	StringCchCopy(m_szCurrentDate, POST_CLEAN_STR_LENGTH, CurrentDate.GetString());

	
	//Target Time
	__time64_t targetTime = currentTime - (__time64_t)GLPost::llPOST_EXPIRE_DURATION_SEC;
	CTime cTargetTime(targetTime);
	CString TargetDate = cTargetTime.Format("%Y-%m-%d %H:%M:%S");
	StringCchCopy(m_szTargetDate, POST_CLEAN_STR_LENGTH, TargetDate.GetString());
}

int CPostClean::Execute(NetServer *pServer)
{
    if (m_pDbManager->GetConnectionType() != db::ADO)
    {
        sc::writeLogError("CPostClean::Execute use ADO");
        return NET_ERROR;
    }
	
	if (m_pDbManager->PostCleanUp(m_szCurrentDate, m_szTargetDate) == sc::db::DB_ERROR)
	{
		sc::writeLogError(sc::string::format("PostClean failure at (%1%).", m_szCurrentDate));
		return NET_ERROR;
	}
	else
	{
		sc::writeLogInfo(sc::string::format("OK:PostClean succeed at (%1%).", m_szCurrentDate));
        return NET_OK;
	}	
}


CPostReturn::CPostReturn(DWORD dwClientID, LONGLONG llPostID)
    : m_llPostID(llPostID)
{	
	m_ClientSlot = dwClientID;
}

CPostReturn::~CPostReturn()
{
}

int CPostReturn::Execute(NetServer *pServer)
{
	int iRecver = 0;

	if (m_llPostID < 0)
		return NET_ERROR;

    if (m_pDbManager->GetConnectionType() !=  db::ADO)
    {
        sc::writeLogError("CPostReturn must be use ADO");
        return NET_ERROR;
    }

	//
	//mjeon
	//iRecver is the character who will receive the returned-post. IOW, it was original Sender.
	//
	if ((iRecver = m_pDbManager->PostReturn(m_llPostID)) == sc::db::DB_ERROR)
	{
		return NET_ERROR;
	}
	else
	{
		CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*> (pServer);
        if (!pAgentServer)
            return NET_ERROR;

        GLAgentServer* pGLAgentServer = pAgentServer->GetGLAgentServer();
        if (!pGLAgentServer)
            return NET_ERROR;

		GLMSG::SNET_POST_RETURN_FB msg;

		msg.llPostID = m_llPostID;
		msg.Result	 = EMPOST_OK;

		pAgentServer->SendClient(m_ClientSlot, &msg);

		GLMSG::SNET_POST_NOTIFY_RELAY msgNotifyRelay;
		msgNotifyRelay.iRecver	= iRecver;
		msgNotifyRelay.llPostID	= m_llPostID;

		// 이렇게 쓰면 안됨 나중에 고쳐야
		GLCharAG* pChaAG = pGLAgentServer->GetCharByDbNum(iRecver);
		if (pChaAG)
            pAgentServer->SendClient(pChaAG->ClientSlot(), &msgNotifyRelay);
	}

	return NET_OK;
}


CPostDelete::CPostDelete(DWORD dwClientID, LONGLONG llPostID)
:m_llPostID(llPostID)
{
	m_ClientSlot = dwClientID;
}


int CPostDelete::Execute(NetServer *pServer)
{
	int nReturn = 0;

	if (m_llPostID < 0)
		return NET_ERROR;

    if (m_pDbManager->GetConnectionType() != db::ADO)
    {
        sc::writeLogError("CPostDelete::Execute. Use ADO");
        return NET_ERROR;
    }

	if ((nReturn = m_pDbManager->PostDelete(m_llPostID)) == sc::db::DB_ERROR)
	{
		return NET_ERROR;
	}
	else
	{
		CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*> (pServer);
		if (!pAgentServer)
            return NET_ERROR;
        
        GLAgentServer* pGLAgentServer = pAgentServer->GetGLAgentServer();

		GLMSG::SNET_POST_DELETE_FB msg;
		msg.llPostID = m_llPostID;
		pAgentServer->SendClient(m_ClientSlot, &msg);

		if ( nReturn != -2 )	//-2: sc::db::DB_OK but there's not Chargement.
		{
			GLMSG::SNET_POST_NOTIFY_RELAY msgNotifyRelay;
			msgNotifyRelay.iRecver	= nReturn;
			msgNotifyRelay.llPostID = m_llPostID;

			// 이렇게 쓰면 안됨 나중에 고쳐야
			PGLCHARAG pChaAG = pGLAgentServer->GetCharByDbNum(nReturn);
			if (pChaAG)
                pAgentServer->SendClient(pChaAG->ClientSlot(), &msgNotifyRelay);
		}
	}

	return NET_OK;
}


CPostOpen::CPostOpen(LONGLONG llPostID)
    : m_llPostID(llPostID)
{	
}


int CPostOpen::Execute(NetServer *pServer)
{
	if (m_llPostID < 0)
		return NET_ERROR;

    if (m_pDbManager->GetConnectionType() != db::ADO)
    {
        sc::writeLogError("CPostOpen::Execute. Use ADO");
        return NET_ERROR;
    }

	if (m_pDbManager->PostOpen(m_llPostID) == sc::db::DB_ERROR)
	{
		return NET_ERROR;
	}

	return NET_OK;
}


CPostGet::CPostGet(
    int iRecver,
    DWORD dwClientID,
    DWORD dwGaeaID,
    BOOL bGetOnlyInfo,
    std::tr1::shared_ptr<VPOST> &pvPost/*VPOST &vPost*/, 
	DWORD dwRecverUserNum )
    : m_iRecver(iRecver)
    , m_bGetOnlyInfo(bGetOnlyInfo)
    , m_pvPost(pvPost)	//construct new shared_ptr here to increase reference counter of it.
    , m_nRowCount(0)
    , m_cntAllPost(0)
    , m_cntNewPost(0)
    , m_dwGaeaID(dwGaeaID)
	, m_dwRecverUserNum( dwRecverUserNum )
{
	m_ClientSlot = dwClientID;
}

int CPostGet::Execute(NetServer *pServer)
{
	if (!pServer)
		return NET_ERROR;

	CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*> (pServer);
    if (!pAgentServer)
        return NET_ERROR;

	if (m_iRecver < 0)
		return NET_ERROR;

    if (m_pDbManager->GetConnectionType() != db::ADO)
    {
        sc::writeLogError("CPostGet::Execute must be use ADO");
        return NET_ERROR;
    }

	if (m_bGetOnlyInfo)
	{
		if (m_pDbManager->PostGetInfo(m_iRecver, m_cntAllPost, m_cntNewPost) == sc::db::DB_ERROR)
		{
			return NET_ERROR;
		}
		else
		{
			GLMSG::SNET_POST_GET_INFO_FB msg;

			msg.iRecver	= m_iRecver;
			msg.cntAllPost = m_cntAllPost;
			msg.cntNewPost = m_cntNewPost;

			pAgentServer->SendClient(m_ClientSlot, &msg);
		}
	}
	else
	{
		if (m_pDbManager->PostGetData(m_iRecver, *(m_pvPost), m_nRowCount, m_dwRecverUserNum) == sc::db::DB_ERROR)
		{
			return NET_ERROR;
		}
		else
		{
			ASSERT( (m_pvPost->size() == m_nRowCount) || (m_pvPost->size() == GLCONST_POST::dwPOST_HOW_MANY_GET) );

			int nLoop = min(m_nRowCount, (int)(GLCONST_POST::dwPOST_HOW_MANY_GET));

            //GLAgentServer* pGLAgentServer = pAgentServer->GetGLAgentServer();
            //if (!pGLAgentServer)
            //    return NET_ERROR;

			//GLCharAG* pChaAG = pGLAgentServer->GetChar(m_dwGaeaID);
			//if (!pChaAG)
			//	return NET_ERROR;

			//pChaAG->m_bPostGotData = FALSE;

			GLMSG::SNET_POST_GET_DATA_CHANGE NetMsg;
			NetMsg.dwChaDbNum = m_iRecver;
			NetMsg.bPostGotData = FALSE;
			pAgentServer->InsertMsg( DBACTION_CLIENT_ID, ( char* )&NetMsg );

			//Send the posts to the Client

			GLMSG::SNET_POST_GET_DATA_FB msg;
			
			msg.iRecver	= m_iRecver;			
			msg.nRowCount = m_nRowCount;
			
			if (m_nRowCount > 0)
			{
				for (int i=nLoop-1; i>=0; --i)
				{
					msg.nPostNum = i;
					msg.aPOST = (*m_pvPost)[i];

					pAgentServer->SendClient(m_ClientSlot, &msg);
				}
			}
			else
			{
                msg.nPostNum = 0;
				
				pAgentServer->SendClient(m_ClientSlot, &msg);
			}	
		}
	}

	return NET_OK;
}


CPostGetState::CPostGetState(int iRecver, DWORD dwClientID, DWORD dwChaDbNum)
    : m_iRecver(iRecver)
    , m_nRowCount(0)
    , m_dwChaDbNum(dwChaDbNum)
{	
	m_ClientSlot = dwClientID;

    m_vPostState.reserve(GLCONST_POST::dwPOST_STATE_HOW_MANY_GET);
    m_vPostState.clear();
}

CPostGetState::~CPostGetState()
{
}

int CPostGetState::Execute(NetServer *pServer)
{
	if (!pServer)
		return NET_ERROR;
    
    CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*> (pServer);
    if (!pAgentServer)
        return NET_ERROR;

	if (m_iRecver < 0)
		return NET_ERROR;

    if (m_pDbManager->GetConnectionType() != db::ADO)
    {
        sc::writeLogError("CPostGetState::Execute must be ADO");
        return NET_ERROR;
    }

    if (m_pDbManager->PostGetState(m_iRecver, m_vPostState, m_nRowCount) == sc::db::DB_ERROR)
		return NET_ERROR;
	
    if (m_vPostState.size() != m_nRowCount)
    {
        /*
        sc::writeLogWarn(
            sc::string::format(
                "CPostGetState Size case 1 %1% != %2%",
                m_vPostState.size(),
                m_nRowCount));
        */
    }

    if (m_vPostState.size() != GLCONST_POST::dwPOST_STATE_HOW_MANY_GET)
    {
        /*
        sc::writeLogWarn(
            sc::string::format(
                "CPostGetState Size case 2 %1% != %2%",
                m_vPostState.size(),
                GLCONST_POST::dwPOST_STATE_HOW_MANY_GET));
        */
    }

	int nLoop = min(m_nRowCount, (int) (GLCONST_POST::dwPOST_STATE_HOW_MANY_GET));

    //GLAgentServer* pGLAgentServer = pAgentServer->GetGLAgentServer();
    //if (!pGLAgentServer)
    //    return NET_ERROR;

	//GLCharAG* pChaAG = pGLAgentServer->GetChar(m_dwGaeaID);
	//if (!pChaAG)
	//	return NET_ERROR;

    //pChaAG->m_bPostGotState = FALSE;

	GLMSG::SNET_POST_GET_STATE_CHANGE NetMsg;
	NetMsg.dwChaDbNum = m_dwChaDbNum;
	NetMsg.bPostGotState = FALSE;
	pAgentServer->InsertMsg( DBACTION_CLIENT_ID, ( char* )&NetMsg );

	GLMSG::SNET_POST_GET_STATE_FB msg;
    
	msg.iRecver   = m_iRecver;
	msg.nRowCount = m_nRowCount;

	if (m_nRowCount > 0)
	{
		for (int i=nLoop-1; i>=0; i--)
		{				
			msg.nPostNum = i;
			msg.aPOSTSTATE = m_vPostState[i];
			pAgentServer->SendClient(m_ClientSlot, &msg);
		}
	}
	else
	{
		msg.nPostNum = 0;
		pAgentServer->SendClient(m_ClientSlot, &msg);
	}

    return NET_OK;
}

CPostGetSendState::CPostGetSendState(int iRecver, DWORD dwClientID, DWORD dwChaDbNum)
    : m_iRecver(iRecver)
    , m_nRowCount(0)
    , m_dwChaDbNum(dwChaDbNum)
{	
	m_ClientSlot = dwClientID;

    m_vPostState.reserve(GLCONST_POST::dwPOST_SEND_STATE_HOW_MANY_GET);
    m_vPostState.clear();
}

CPostGetSendState::~CPostGetSendState()
{
}

int CPostGetSendState::Execute(NetServer *pServer)
{
	if (!pServer)
		return NET_ERROR;
    
    CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*> (pServer);
    if (!pAgentServer)
        return NET_ERROR;

	if (m_iRecver < 0)
		return NET_ERROR;

    if (m_pDbManager->GetConnectionType() != db::ADO)
    {
        sc::writeLogError("CPostGetSendState::Execute must be ADO");
        return NET_ERROR;
    }
    
    if (m_pDbManager->PostGetSendState(m_iRecver, m_vPostState, m_nRowCount) == sc::db::DB_ERROR)
		return NET_ERROR;
	
    if (m_vPostState.size() != m_nRowCount)
    {
        /*
        sc::writeLogWarn(
            sc::string::format(
                "CPostGetState Size case 1 %1% != %2%",
                m_vPostState.size(),
                m_nRowCount));
        */
    }

    if (m_vPostState.size() != GLCONST_POST::dwPOST_SEND_STATE_HOW_MANY_GET)
    {
        /*
        sc::writeLogWarn(
            sc::string::format(
                "CPostGetState Size case 2 %1% != %2%",
                m_vPostState.size(),
                GLCONST_POST::dwPOST_STATE_HOW_MANY_GET));
        */
    }

	int nLoop = min(m_nRowCount, (int) (GLCONST_POST::dwPOST_SEND_STATE_HOW_MANY_GET));

    //GLAgentServer* pGLAgentServer = pAgentServer->GetGLAgentServer();
    //if (!pGLAgentServer)
    //    return NET_ERROR;

	//GLCharAG* pChaAG = pGLAgentServer->GetChar(m_dwGaeaID);
	//if (!pChaAG)
	//	return NET_ERROR;

    //pChaAG->m_bPostGotSendState = FALSE;

	GLMSG::SNET_POST_GET_SEND_STATE_CHANGE NetMsg;
	NetMsg.dwChaDbNum = m_dwChaDbNum;
	NetMsg.bPostGotSendState = FALSE;
	pAgentServer->InsertMsg( DBACTION_CLIENT_ID, ( char* )&NetMsg );

	//Send the posts to the Client
	GLMSG::SNET_POST_GET_SEND_STATE_FB msg;
    
	msg.iRecver   = m_iRecver;
	msg.nRowCount = m_nRowCount;

	if (m_nRowCount > 0)
	{
		for (int i=nLoop-1; i>=0; i--)
		{				
			msg.nPostNum = i;
			msg.aPOSTSTATE = m_vPostState[i];
			pAgentServer->SendClient(m_ClientSlot, &msg);
		}
	}
	else
	{
		msg.nPostNum = 0;
		pAgentServer->SendClient(m_ClientSlot, &msg);
	}

	return NET_OK;
}


CPostGetRecvState::CPostGetRecvState(int iRecver, DWORD dwClientID, DWORD dwChaDbNum)
    : m_iRecver(iRecver)
    , m_nRowCount(0)
    , m_dwChaDbNum(dwChaDbNum)
{	
	m_ClientSlot = dwClientID;

    m_vPostState.reserve(GLCONST_POST::dwPOST_RECV_STATE_HOW_MANY_GET);
    m_vPostState.clear();
}

CPostGetRecvState::~CPostGetRecvState()
{
}

int CPostGetRecvState::Execute(NetServer *pServer)
{
	if (!pServer)
		return NET_ERROR;
    
    CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*> (pServer);
    if (!pAgentServer)
        return NET_ERROR;

	if (m_iRecver < 0)
		return NET_ERROR;

    if (m_pDbManager->GetConnectionType() != db::ADO)
    {
        sc::writeLogError("CPostGetState::Execute must be ADO");
        return NET_ERROR;
    }

    if (m_pDbManager->PostGetRecvState(m_iRecver, m_vPostState, m_nRowCount) == sc::db::DB_ERROR)
		return NET_ERROR;
	
    if (m_vPostState.size() != m_nRowCount)
    {
        /*
        sc::writeLogWarn(
            sc::string::format(
                "CPostGetState Size case 1 %1% != %2%",
                m_vPostState.size(),
                m_nRowCount));
        */
    }

    if (m_vPostState.size() != GLCONST_POST::dwPOST_RECV_STATE_HOW_MANY_GET)
    {
        /*
        sc::writeLogWarn(
            sc::string::format(
                "CPostGetState Size case 2 %1% != %2%",
                m_vPostState.size(),
                GLCONST_POST::dwPOST_STATE_HOW_MANY_GET));
        */
    }

	int nLoop = min(m_nRowCount, (int) (GLCONST_POST::dwPOST_RECV_STATE_HOW_MANY_GET));

    //GLAgentServer* pGLAgentServer = pAgentServer->GetGLAgentServer();
    //if (!pGLAgentServer)
    //    return NET_ERROR;

	//GLCharAG* pChaAG = pGLAgentServer->GetChar(m_dwGaeaID);
	//if (!pChaAG)
	//	return NET_ERROR;

    //pChaAG->m_bPostGotRecvState = FALSE;

	GLMSG::SNET_POST_GET_RECV_STATE_CHANGE NetMsg;
	NetMsg.dwChaDbNum = m_dwChaDbNum;
	NetMsg.bPostGotRecvState = FALSE;
	pAgentServer->InsertMsg( DBACTION_CLIENT_ID, ( char* )&NetMsg );

	//Send the posts to the Client
	GLMSG::SNET_POST_GET_RECV_STATE_FB msg;
    

	msg.iRecver   = m_iRecver;
	msg.nRowCount = m_nRowCount;

	if (m_nRowCount > 0)
	{
		for (int i=nLoop-1; i>=0; i--)
		{				
			msg.nPostNum = i;
			msg.aPOSTSTATE = m_vPostState[i];
			pAgentServer->SendClient(m_ClientSlot, &msg);
		}
	}
	else
	{
		msg.nPostNum = 0;
		pAgentServer->SendClient(m_ClientSlot, &msg);
	}

	return NET_OK;
}


//
//mjeon.Post.ChaNum
//
CGetChaNum::CGetChaNum(DWORD dwClient, DWORD dwGaeaID, const char *szReqChaName, int nReqID)
    : m_dwGaeaID(dwGaeaID)
{
	m_ClientSlot	= dwClient;
	m_nReqID	= nReqID;
	StringCchCopy(m_szChaName, sizeof(m_szChaName)/sizeof(TCHAR), szReqChaName);
}


int CGetChaNum::Execute(NetServer *pServer)
{
	if (pServer == NULL)
		return NET_ERROR;

	//if (m_nUserNum > 0 && m_nChaNum > 0)
	{
		CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*> (pServer);
		if (m_pDbManager->GetChaNum(m_szChaName, &m_nChaNum, &m_nUserNum) == sc::db::DB_ERROR)
		{
			pAgentServer->MsgReqChaNumBack(m_ClientSlot, m_dwGaeaID, -1, -1, m_nReqID);	//There does not exist the character.
			return NET_ERROR;
		}
		else
		{
			int nUserType = m_pDbManager->GetUserType(m_nUserNum);

			//
			// mjeon.Post.GetChaNum
			//
			// Users cannot send a post to GM.
			// Moreover, User cannot get the ChaNum of GM.
			//
			// Client-side usually handles this case before server-side do.
			// Therefore, normally GetChaNum process should not come here.
			//
			if (nUserType >= USER_GM3 && nUserType <= USER_MASTER)
			{
				pAgentServer->MsgReqChaNumBack(m_ClientSlot, m_dwGaeaID, -1, -1, m_nReqID);	//There does not exist the character.
				return NET_ERROR;
			}

			pAgentServer->MsgReqChaNumBack(m_ClientSlot, m_dwGaeaID, m_nChaNum, m_nUserNum, m_nReqID);
            pAgentServer->ChaNameDbNumAdd(std::string(m_szChaName), m_nChaNum, m_nUserNum, static_cast<EMUSERTYPE> (nUserType));

			return NET_OK;
		}
	}
}


CPostSendSystemMail::CPostSendSystemMail(SPOSTINFO PostInfo, SITEMCUSTOM &itemcustom)
    : m_PostInfo(PostInfo)
    , m_AttachedItem(itemcustom)
{	
}

int CPostSendSystemMail::Execute(NetServer* pServer)
{
    CFieldServer* pFieldServer = reinterpret_cast<CFieldServer*> (pServer);
    if (!pFieldServer)
        return NET_ERROR;

    GLGaeaServer* pGaeaServer = pFieldServer->GetGLGaeaServer();
    if (!pGaeaServer)
        return NET_ERROR;

    if (m_pDbManager->GetConnectionType() != db::ADO)
    {
        sc::writeLogError("CPostSendSystemMail::Execute. Use ADO");
        return NET_ERROR;
    }

    int nReturn = 0;
	TCHAR szQuery[256] = {0,};

	nReturn = m_pDbManager->PostSend(m_PostInfo, m_AttachedItem);

	// PostSend completed successfully.
	if (nReturn == sc::db::DB_OK)
	{	
		// 1. Send Notify to the Recver via AgentServer
		GLMSG::SNET_POST_NOTIFY msgNotify;
		msgNotify.llPostID = m_PostInfo.llPostID;	// completed postsend
		msgNotify.iRecver  = m_PostInfo.iRecver;
		pGaeaServer->SENDTOAGENT(&msgNotify);
	}
	else
	{
		sc::writeLogError(std::string("CPostSendSystemMail: PostSend() failure."));
	}
	return nReturn;
}

CPostSendSystemMailAgent::CPostSendSystemMailAgent( SPOSTINFO PostInfo, SITEMCUSTOM& itemcustom )
: m_PostInfo( PostInfo )
, m_AttachedItem( itemcustom )
{
}

int CPostSendSystemMailAgent::Execute( NetServer* pServer )
{
	if ( m_pDbManager->GetConnectionType() != db::ADO )
	{
		sc::writeLogError( "CPostSendSystemMailAgent::Execute. Use ADO" );
		return NET_ERROR;
	}

	int nReturn = 0;
	nReturn = m_pDbManager->PostSend( m_PostInfo, m_AttachedItem );
	if ( nReturn == sc::db::DB_OK )
	{
		CAgentServer* pAgentServer = reinterpret_cast< CAgentServer* >( pServer );
		if ( !pAgentServer )
			return sc::db::DB_OK;

		GLAgentServer* pGLAgentServer = pAgentServer->GetGLAgentServer();
		if ( !pGLAgentServer )
			return sc::db::DB_OK;

		GLMSG::SNET_POST_NOTIFY_RELAY msgRelay;
		msgRelay.iRecver = m_PostInfo.iRecver;
		msgRelay.llPostID = m_PostInfo.llPostID;

		GLCharAG* pCharAG = pGLAgentServer->GetCharByDbNum( m_PostInfo.iRecver );
		if ( pCharAG )
		{
			pGLAgentServer->SENDTOCLIENT( pCharAG->m_ClientSlot, &msgRelay );
		}
	}
	else
	{
		sc::writeLogError( std::string( "CPostSendSystemMail: PostSend() failure." ) );
	}
	return nReturn;
}

int db::CPostRnSelectAll::Execute( NetServer* m_pServer )
{
	if( m_pSubDbManager == FALSE )
		return sc::db::DB_ERROR;

	std::vector<PostInfo> vecPostList;
	if (m_pDbManager->PostRnSelectAll(vecPostList, m_dwRecvChaNum) == sc::db::DB_ERROR)
		return NET_ERROR;

	int nCount=0;
	GLMSG::NET_POST_RENEW_LIST_ACK SendData;

	for ( unsigned int i=0 ; i<vecPostList.size() ; i++ )
	{
		SendData.m_vecPostList.push_back( vecPostList[i] );
		nCount++;

		if( NET_DATA_BUFSIZE <= (nCount+1) * sizeof(PostInfo) || i == vecPostList.size()-1 )
		{
			msgpack::sbuffer SendBuffer;
			msgpack::pack(SendBuffer, SendData);

			CacheServer* pServer = reinterpret_cast<CacheServer*> (m_pServer);
			if (pServer)
				pServer->InsertMsg(DBACTION_CLIENT_ID, NET_MSG_POST_RENEW_LIST_ACK, SendBuffer);
			else
				return sc::db::DB_ERROR;

			SendData.m_vecPostList.clear();
			nCount=0;
		}
		
	}

	return sc::db::DB_OK;
}

int db::CPostRnSetRead::Execute( NetServer* m_pServer )
{
	if( m_pSubDbManager == FALSE )
		return sc::db::DB_ERROR;

	m_pDbManager->PostRnSetRead(m_nPostID);

	return sc::db::DB_OK;
}

int db::CPostRnDelAttach::Execute( NetServer* m_pServer )
{
	if( m_pSubDbManager == FALSE )
		return sc::db::DB_ERROR;

	int nReturn = m_pDbManager->PostRnDelAttach(m_Backup.dwPostID);

	GLMSG::NET_POST_RENEW_ATTACH_ACK& SendData = m_Backup;


	if( nReturn != sc::db::DB_OK )
	{
		SendData.emMoneyType = EMPOST_MONEY_NOT_USE;
		SendData.llMoney = 0;
		SendData.sItem = SITEMCUSTOM(NATIVEID_NULL());
	}

	SendData.emCode = (EMPOST_ATTACH_CODE)nReturn;

	msgpack::sbuffer SendBuffer;
	msgpack::pack(SendBuffer, SendData);

	CFieldServer* pFieldServer = reinterpret_cast<CFieldServer*> (m_pServer);
	if (pFieldServer)
		pFieldServer->InsertMsg(DBACTION_CLIENT_ID, NET_MSG_POST_RENEW_ATTACH_ACK, SendBuffer);
	else
		return sc::db::DB_ERROR;


	return NET_OK;
}

int db::CPostRnDel::Execute( NetServer* m_pServer )
{
	if( m_pSubDbManager == FALSE )
		return sc::db::DB_ERROR;

	int nReturn = m_pDbManager->PostRnDel(m_nPostID);

	return nReturn;
}

int db::CPostRnSend::Execute( NetServer* m_pServer )
{
	if( m_pSubDbManager == FALSE )
		return sc::db::DB_ERROR;




	int nPostID = m_pDbManager->PostRnSend(
		m_PostInfo.dwSendChaNum,
		m_PostInfo.strSendChaName,
		m_PostInfo.dwSendUserNum,
		m_PostInfo.dwRecvChaNum,
		m_PostInfo.wStyle,
		m_PostInfo.wMailType,
		m_PostInfo.wAttachType,
		m_PostInfo.wMoneyType,
		m_PostInfo.llMoney,
		m_PostInfo.sItemCutom,
		m_PostInfo.strTitle,
		m_PostInfo.strContent );

	int nErrorCode = (int)EMPOST_SEND_ERROR;
	if( 1 < nPostID )
		nErrorCode = EMPOST_SEND_OK;
	else
		nErrorCode = nPostID;



	if( nErrorCode == EMPOST_SEND_OK )
	{
		GLMSG::NET_POST_RENEW_SEND_ACK SendData;
		SendData.dwReqClientSlot = m_dwReqClientSlot;

		SendData.postInfo = m_PostInfo;
		SendData.postInfo.dwPostID = nPostID;

		SendData.emCode = (EMPOST_SEND_CODE)nErrorCode;

		msgpack::sbuffer SendBuffer;
		msgpack::pack(SendBuffer, SendData);

		CacheServer* pServer = reinterpret_cast<CacheServer*> (m_pServer);
		if (pServer)
			pServer->InsertMsg(DBACTION_CLIENT_ID, NET_MSG_POST_RENEW_SEND_ACK, SendBuffer);
		else
			return sc::db::DB_ERROR;

	}



	return NET_OK;
}

int db::CPostRnCharCheck::Execute( NetServer* m_pServer )
{
	if( m_pSubDbManager == FALSE )
		return sc::db::DB_ERROR;

	int nReturn = m_pSubDbManager->PostCharacterCheck( m_Backup.dwRecvChaNum );

	GLMSG::NET_POST_RENEW_CHA_CHECH_ACK SendData;

	SendData.postInfo.dwSendChaNum	= m_dwSendChaDbNum;
	SendData.postInfo.dwRecvChaNum	= m_Backup.dwRecvChaNum;
	SendData.postInfo.strTitle		= m_Backup.strTitle;
	SendData.postInfo.strContent	= m_Backup.strContent;
	SendData.postInfo.wStyle		= m_Backup.wStyle;
	SendData.postInfo.wMoneyType	= m_Backup.wMoneyType;
	SendData.postInfo.llMoney		= m_Backup.llMoney;
	SendData.postInfo.sItemCutom	= m_Backup.sItem;

	if ( nReturn == sc::db::DB_OK )
		SendData.emCode = (int)EMPOST_SEND_OK;
	else
		SendData.emCode = (int)EMPOST_SEND_RECV_CHAR_NOTEXIST;


	msgpack::sbuffer SendBuffer;
	msgpack::pack(SendBuffer, SendData);

	CFieldServer* pServer = reinterpret_cast<CFieldServer*> (m_pServer);
	if (pServer)
		pServer->InsertMsg(DBACTION_CLIENT_ID, NET_MSG_POST_RENEW_CHA_CHECK_ACK, SendBuffer);
	else
		return sc::db::DB_ERROR;

    return sc::db::DB_OK;
}

int db::CPostRnReturn::Execute( NetServer* m_pServer )
{
	if( m_pSubDbManager == FALSE )
		return sc::db::DB_ERROR;

	int nReturn = m_pDbManager->PostRnReturn( m_dwPostID, m_dwReturnChaNum, m_strReturnChaName, m_dwReturnUserNum, m_dwRecvChaNum );

	GLMSG::NET_POST_RENEW_ATTACH_RETURN_ACK SendData;
	SendData.dwReqClientSlot = m_dwFieldSlot;
	SendData.emCode = nReturn;
	SendData.postInfo.dwPostID = m_dwPostID;

	

	msgpack::sbuffer SendBuffer;
	msgpack::pack(SendBuffer, SendData);

	CFieldServer* pFieldServer = reinterpret_cast<CFieldServer*> (m_pServer);
	if (pFieldServer)
		pFieldServer->InsertMsg(DBACTION_CLIENT_ID, NET_MSG_POST_RENEW_RETURN_ACK, SendBuffer);
	else
		return sc::db::DB_ERROR;


	return NET_OK;
}
