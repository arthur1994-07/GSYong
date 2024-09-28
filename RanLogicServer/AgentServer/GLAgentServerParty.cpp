#include "../pch.h"
#include "../../SigmaCore/Hash/CHash.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../RanLogic/Msg/PartyMsg.h"
#include "../Party/GLPartyManagerAgent.h"
#include "./GLAgentServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const GLPartyAgent* const GLAgentServer::GetParty(const GLPartyID& PartyID) const
{ 
    return m_pPartyManager->GetConstParty(PartyID);
}

const GLExpeditionAgent* const GLAgentServer::GetExpedition(const GLPartyID& PartyID) const
{
	return m_pPartyManager->GetConstExpedition(PartyID);
}

void GLAgentServer::SENDTOPARTY(const GLPartyID& PartyID, NET_MSG_GENERIC* nmg)
{
    m_pPartyManager->SendMsgToMember(PartyID, nmg);
}

void GLAgentServer::SENDTOPARTY(DWORD dwGaeaID, const GLPartyID& PartyID, NET_MSG_GENERIC* nmg)
{
    m_pPartyManager->SendMsgToMember(dwGaeaID, PartyID, nmg);
}

void GLAgentServer::SENDTOPARTY(const GLPartyID& PartyID, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc)
{
    m_pPartyManager->SendMsgToMember(PartyID, MsgType, PackBuffer, UseCrc);
}

BOOL GLAgentServer::JoinParty(const DWORD dwMasterGaeaID, const DWORD dwTargetGaeaID, const SPartyOption& PartyOption, const bool bUseScript /*= true*/)
{	
    GLCharAG* const pTargetChar = GetChar(dwTargetGaeaID);
    if ( pTargetChar == NULL )
        return FALSE;

    GLCharAG* const pMasterChar = GetChar(dwMasterGaeaID);
    if ( pMasterChar == NULL )
        return FALSE;	

    const DWORD dwMasterClientSlot = pMasterChar->ClientSlot();
    const DWORD dwTargetClientSlot = pTargetChar->ClientSlot();	

    const GLPartyID& MasterPartyID = pMasterChar->GetPartyID();
    const GLPartyID& TargetPartyID = pTargetChar->GetPartyID();
    if ( TargetPartyID.isValidParty() == true )
    {
        GLMSG::SNET_PARTY_LURE_FB NetMsgFB(dwTargetGaeaID, EMPARTY_LURE_OTHER);
        SENDTOCLIENT(dwMasterClientSlot, &NetMsgFB);
        return FALSE;
    }

    // CTF 참여 중일때 다른 학원끼리는 파티 불가
    if( TRUE == pTargetChar->IsCtfPlayer() || TRUE == pMasterChar->IsCtfPlayer() )
    {
        if( pTargetChar->GetSchool() != pMasterChar->GetSchool() )
        {
            GLMSG::SNET_PARTY_LURE_FB NetMsgFB( 0, EMPARTY_LURE_OTHERSCHOOL );
            SENDTOCLIENT( dwMasterClientSlot, &NetMsgFB );
            SENDTOCLIENT( dwTargetClientSlot, &NetMsgFB );

            return FALSE;
        }
    }

    if ( bUseScript == true )
    {  // 인던 스크립트에서 파티 가입을 제어하도록 함;
        // 현재는 마스터와 가입멤버의 인던에만 이벤트호출하도록 되어있음;
        const MapID& _targetCharMapID = pTargetChar->GetCurrentMap();
        GLAGLandManager* _pTargetCharLandMan(GetLandManager(_targetCharMapID));
        if ( _pTargetCharLandMan == NULL )
            return FALSE;

        const MapID& _masterCharMapID = pMasterChar->GetCurrentMap();
        GLAGLandManager* _pMasterCharLandMan(GetLandManager(_masterCharMapID));    
        if ( _pMasterCharLandMan == NULL )
            return FALSE;

        const DWORD dwMasterCharDbNum = pMasterChar->ChaDbNum();
        const DWORD dwTargetCharDbNum = pTargetChar->ChaDbNum();

        // 멤버의 인던 스크립트 이벤트 호출;
        InstanceSystem::InstanceAgent* const _pTargetCharInstance = _pTargetCharLandMan->getScriptInstance();
        if ( _pTargetCharInstance )
        {
            if ( _pTargetCharInstance->eventPartyJoin(dwMasterCharDbNum, dwTargetCharDbNum) == false)
                return FALSE;
        }

        // 마스터와 멤버가 서로 다른 맵에 있다면 마스터의 인던에도 이벤트 호출;
        if ( _pTargetCharLandMan != _pMasterCharLandMan )
        {
            InstanceSystem::InstanceAgent* const _pMasterCharInstance = _pMasterCharLandMan->getScriptInstance();
            if ( _pMasterCharInstance )
            {
                if ( _pMasterCharInstance->eventPartyJoin(dwMasterCharDbNum, dwTargetCharDbNum) == false)
                    return FALSE;
            }
        }
    }

    // 파티가 최초로 구성될때;
    if ( MasterPartyID.isValidParty() == false )
    {
		/*// 친구카드사용 하여 맵이동중 파티모집시 
		if( pTargetChar->m_bMapMoveFCard )
		{
			pTargetChar->m_bMapMoveFCard = FALSE;

			GLMSG::SNET_PARTY_LURE_FB NetPartyError( dwMasterGaeaID, EMPARTY_LURE_CREATE_FAIL );

			SENDTOCLIENT( pMasterChar->ClientSlot(), &NetPartyError );

			return FALSE;
		}*/

        // 파티 생성;
        GLPartyAgent* pParty = m_pPartyManager->NewParty(dwMasterGaeaID);

        if ( pParty == NULL )
            return FALSE;

        // 파티 옵션 지정;
        pParty->m_PartyOption = PartyOption;

        // 파티 구성원에 추가;
        pParty->addPartyMember(this, dwTargetGaeaID);

        pTargetChar->SetPartyID(pParty->m_PartyID, true);

        // 파티 생성을 필드에 알림(필드 서버는 파티 정보을 받으면 새로이 파티를 구성한다);
        //
        GLMSG::SNET_PARTY_FNEW NetPartyInfo(pParty->m_PartyID, dwMasterGaeaID, pParty->m_PartyOption);

        GLPartyAgent::MEMBER_ITER iter = pParty->m_Member.begin();
        GLPartyAgent::MEMBER_ITER iter_end = pParty->m_Member.end();

        for ( ; iter!=iter_end; ++iter )
        {
            GLCharAG* pCharMem = GetChar ( (*iter) );
            if ( pCharMem == NULL )
                continue;
            NetPartyInfo.ADDMEMBER(GLPARTY_FNET(pCharMem->GaeaId(), pCharMem->ChaDbNum(), pCharMem->m_szName, pCharMem->GetSchool(), pCharMem->m_wLevel, pCharMem->m_emClass, pCharMem->GetCurrentMap()));
        }

        SENDTOALLCHANNEL(&NetPartyInfo);

       /* if( pTargetChar->m_bTracingUser || pMasterChar->m_bTracingUser )
        {
            CDebugSet::ToTracingFile( pTargetChar->m_szUserName, "Create Party Link Lure OK, [%s][%s], Master Char: [%s][%s]",
                pTargetChar->m_szUserName, pTargetChar->m_szName, pMasterChar->m_szUserName, pMasterChar->m_szName );
        }*/

        return TRUE;
    }    
    else 
    { // 파티원을 추가할때;
        DWORD dwPartySlotIndex = MASTER_PARTY;
        if ( MasterPartyID.isValidExpedition() == true )
        {
            GLExpeditionAgent* const pExpedition = m_pPartyManager->GetExpedition(MasterPartyID);
            if ( pExpedition == NULL )
            {
                GLMSG::SNET_PARTY_ERROR_TO_CLIENT ErrorMsg(MasterPartyID);
                ErrorMsg.emPartyErrorCode = EMPARTY_ERROR_LINK_FULLPARTY;
                SENDTOCLIENT(dwMasterClientSlot, &ErrorMsg);
                pMasterChar->ResetPartyID();
                return FALSE;
            }

            if ( pExpedition->deletePreMember(dwTargetGaeaID) == false )
                return FALSE;

            bool bFullExpedition = true;
            for ( DWORD _i = 0; _i < MAX_PARTYGROUP; ++_i )
            {
                const GLPartyID& ExpeditionPartyID = pExpedition->GetPartyID(_i);
                const DWORD dwSlotIndex = m_pPartyManager->AddPartyMember(ExpeditionPartyID, dwTargetGaeaID);

                if ( dwSlotIndex == DEFSLOT_NULL )
                    continue;
                else if ( dwSlotIndex == MASTER_PARTY )
                    pExpedition->SetMasterGaeaID(dwTargetGaeaID, _i);

                bFullExpedition = false;
                pTargetChar->SetPartyID(ExpeditionPartyID, true);

                GLMSG::SNET_EXPEDITION_NEW_FB NewExpeditionAC;				
                NewExpeditionAC.dwMasterGaeaID = dwMasterGaeaID;
                NewExpeditionAC.dwMyPartyIndex = _i;

                for ( DWORD _j = MAX_PARTYGROUP; _j; --_j )	
                    NewExpeditionAC.PartyIDs[_j - 1] = pExpedition->GetPartyID(_j - 1);
                dwPartySlotIndex = _i;

                SENDTOCLIENT(dwTargetClientSlot, &NewExpeditionAC);
                break;
            }

            if ( bFullExpedition == true )
            {
                GLMSG::SNET_PARTY_ERROR_TO_CLIENT NetErrMsg(MasterPartyID);
                NetErrMsg.emPartyErrorCode = EMPARTY_ERROR_LINK_FULLPARTY;
                SENDTOCLIENT ( dwTargetClientSlot, &NetErrMsg );
                return FALSE;
            }
        }
        else
        {
            GLPartyAgent* pParty = m_pPartyManager->GetParty(MasterPartyID);

            if ( pParty == NULL )
            {
                GLMSG::SNET_PARTY_ERROR_TO_CLIENT NetErrMsg( MasterPartyID );
                NetErrMsg.emPartyErrorCode = EMPARTY_ERROR_NOPARTY;
                SENDTOCLIENT( dwTargetClientSlot, &NetErrMsg );

                return FALSE;
            }

            if ( pParty->isFull() == true )
            {
                GLMSG::SNET_PARTY_ERROR_TO_CLIENT NetErrMsg(MasterPartyID);
                NetErrMsg.emPartyErrorCode = EMPARTY_ERROR_LINK_FULLPARTY;
                SENDTOCLIENT(dwTargetClientSlot, &NetErrMsg );
                return FALSE;
            }

            const DWORD dwSlotIndex = m_pPartyManager->AddPartyMember(MasterPartyID, dwTargetGaeaID);
            if ( dwSlotIndex == DEFSLOT_NULL )
            {
                GLMSG::SNET_PARTY_ERROR_TO_CLIENT ErrorMsg(MasterPartyID);
                ErrorMsg.emPartyErrorCode = EMPARTY_ERROR_NOPARTY;
                SENDTOCLIENT(dwMasterClientSlot, &ErrorMsg);
                pMasterChar->ResetPartyID();
                return FALSE;
            }

            pTargetChar->SetPartyID(MasterPartyID, true);
        }


        // 필드 서버에 파티원이 추가됨을 알림.
        //
        GLMSG::SNET_PARTY_ADD NetPartyAdd(
            pTargetChar->GetPartyID(),
            GLPARTY_FNET(
            dwTargetGaeaID,
            pTargetChar->ChaDbNum(),
            pTargetChar->m_szName,
            pTargetChar->GetSchool(),
            pTargetChar->GetChaLevel(),
            pTargetChar->m_emClass,
            pTargetChar->GetCurrentMap()));

        SENDTOALLCHANNEL ( &NetPartyAdd );

        /*if( pTargetChar->m_bTracingUser || pMasterChar->m_bTracingUser )
        {
            CDebugSet::ToTracingFile( pTargetChar->m_szUserName, "Add Party Answer OK, [%s][%s], Master Char: [%s][%s]",
                pTargetChar->m_szUserName, pTargetChar->m_szName, pMasterChar->m_szUserName, pMasterChar->m_szName );
        }*/
    }

    return TRUE;
}

void GLAgentServer::QuitParty(GLCharAG *pCharAG, const bool bUseScript /*= true*/, EMPARTY_ACTION_TYPE eType )
{
    if ( pCharAG == NULL )
    {
        sc::writeLogError(std::string("GLAgentServer::QuitParty() - Character does not exist."));
        return;
    }

    const GLPartyID& PartyID = pCharAG->GetPartyID();
    if ( PartyID.isValidParty() == false )
        return;

    if ( bUseScript == true )
    {  // 인던 스크립트에서 파티 탈퇴를 제어하도록 함;
        // 현재는 탈퇴멤버의 인던에만 이벤트호출하도록 되어있음;
        const MapID& _MapID = pCharAG->GetCurrentMap();
        GLAGLandManager* _pLandMan(GetLandManager(_MapID));
        if ( _pLandMan == NULL )
            return;

        // 멤버의 인던 스크립트 이벤트 호출;
        InstanceSystem::InstanceAgent* const _pInstance = _pLandMan->getScriptInstance();
        if ( _pInstance )
        {
            if ( _pInstance->eventPartyOut(pCharAG->ChaDbNum()) == false)
                return;
        }
    }

    const DWORD dwGaeaID = pCharAG->GaeaId();
    if ( m_pPartyManager->removePartyMember(PartyID, dwGaeaID, static_cast<BYTE>(eType)) == false )
    {
        sc::writeLogError(std::string("GLAgentServer::QuitParty() - Party does not exist."));
        m_pPartyManager->SendErrorMsg(PartyID, pCharAG->GaeaId(), EMPARTY_ERROR_NOPARTY, "QuitParty()");
        pCharAG->ResetPartyID();
        pCharAG->ResetPartyOption();

        QuitPartyMap( pCharAG );
    }
}

const bool GLAgentServer::DeleteParty(const GLPartyID _partyID, const bool bUseScript /*= true*/)
{
    const GLPartyAgent* const pParty = GetParty(_partyID);
    if ( pParty == NULL )
        return false;

    GLCharAG* pCharAG = GetChar(pParty->getMaster());
    if ( pCharAG == NULL )
        return false;

    if ( bUseScript == true )
    {  // 인던 스크립트에서 파티 해산를 제어하도록 함;
        // 현재는 해산파티의 마스터의 인던에만 이벤트호출하도록 되어있음;
        const MapID& _MapID = pCharAG->GetCurrentMap();
        GLAGLandManager* _pLandMan(GetLandManager(_MapID));
        if ( _pLandMan == NULL )
            return false;

        // 멤버의 인던 스크립트 이벤트 호출;
        InstanceSystem::InstanceAgent* const _pInstance = _pLandMan->getScriptInstance();
        if ( _pInstance )
        {
            if ( _pInstance->eventPartyDissolution(pCharAG->ChaDbNum()) == false)
                return false;
        }
    }

    return m_pPartyManager->DeleteParty(_partyID);
}

void GLAgentServer::QuitPartyMap( GLCharAG* pChar )
{
    // 파티맵일 경우 학원으로 이동시킨다.
    if( NULL == pChar )
        return;

    // GM 캐릭터 제외
    if( USER_USER_GM <= pChar->m_dwUserLvl )
        return;

    const SMAPNODE* pMapNode = m_sMapList.FindMapNode( pChar->GetCurrentMap().getBaseMapID() );
    if( NULL == pMapNode )
        return;

    if( false == pMapNode->IsPartyMap() )
        return;

    WORD wSchool = pChar->GetSchool();
    if( MAX_SCHOOL <= wSchool )
        return;

    GLMSG::SNETPC_REQ_MOVE_MAP NetMsg;
    NetMsg.dwChaNum = pChar->ChaDbNum();
    NetMsg.sMapID = GLCONST_CHAR::nidSTARTMAP[ wSchool ];
    NetMsg.dwGata = GLCONST_CHAR::dwSTARTGATE[ wSchool ];
    SENDTOFIELD( pChar->ClientSlot(), &NetMsg );
}


BOOL GLAgentServer::MsgPartySecede(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
    GLMSG::SNET_PARTY_SECEDE* pNetMsg = (GLMSG::SNET_PARTY_SECEDE*) nmg;
	const GLPartyID& PartyID = pNetMsg->PartyID;
	const GLPartyAgent* const pParty = GetParty(PartyID);
	if ( pParty == NULL )
		return FALSE;

	const GLExpeditionAgent* const pExpedition = PartyID.isValidExpedition() ? GetExpedition(PartyID) : NULL;
	const bool bExpedition = pExpedition != NULL;   //원정대인가?

	const bool bRequesterIsMaster = bExpedition ? pExpedition->isMaster(dwGaeaID) : pParty->isMaster(dwGaeaID); //마스터인가?

	const DWORD dwTargetGaeaID = pNetMsg->dwGaeaID; //추방/탈퇴 ID

    // 자기 자신을 추방한 방식 -> 탈퇴로 동작한다.

    if ( pNetMsg->dwGaeaID != dwGaeaID )    
	{
        //자신자신이 아니므로 추방 (마스터만 가능)
        if ( bExpedition )  
        {
            //원정대라면 원정대장만 추방 가능
            if ( pExpedition->isMaster(dwGaeaID) == false )
                return FALSE;
        }
        else
        {
            //파티라면 파티장만 추방 가능
            if ( pParty->isMaster(dwGaeaID) == false )
                return FALSE;
        }
	}

    GLCharAG* _pChar = GetChar(pNetMsg->dwGaeaID);
    QuitParty(_pChar, true, pNetMsg->eType );

    return TRUE;
}

BOOL GLAgentServer::MsgPartyAuthority( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
    GLMSG::SNET_PARTY_AUTHORITY* pNetMsg = (GLMSG::SNET_PARTY_AUTHORITY*) nmg;	

	GLCharAG* pCurrentMaster = GetChar(dwGaeaID);
	if ( pCurrentMaster == NULL )
		return FALSE;

	const GLPartyID& PartyID = pNetMsg->PartyID;
	GLPartyAgent* const pParty = m_pPartyManager->GetParty(PartyID);
	if ( pParty == NULL )
	{
		m_pPartyManager->SendErrorMsg(PartyID, dwGaeaID, EMPARTY_ERROR_NOPARTY, "MsgPartyAuthority");
		pCurrentMaster->ResetPartyID();
		return FALSE;
	}

	const DWORD dwNewMasterGaeaID = pNetMsg->dwNewMasterGaeaID;
	if ( pParty->isPartyMember(dwNewMasterGaeaID) == false )
		return FALSE;

	if ( dwGaeaID == dwNewMasterGaeaID )
		return FALSE;

	GLCharAG* pNewMaster = GetChar(dwNewMasterGaeaID);
	if ( pNewMaster == NULL )
		return FALSE;

	pNewMaster->m_sReqPartyOpt = pCurrentMaster->m_sReqPartyOpt;

	if ( PartyID.isValidExpedition() == true )
	{
		GLExpeditionAgent* const pExpedition = m_pPartyManager->GetExpedition(PartyID);
		if ( pExpedition == NULL )
			return FALSE;

		if ( pExpedition->GetPartyID(pNetMsg->dwPartyIndex) != PartyID )
			return FALSE;		

		if ( pExpedition->GetMasterGaeaID(MASTER_PARTY) != dwGaeaID )
			return FALSE;

		const GLSwapSlotInformation CurrentMaster(dwGaeaID, MASTER_PARTY);
		const GLSwapSlotInformation NewMaster(dwNewMasterGaeaID, pNetMsg->dwPartyIndex);
		m_pPartyManager->SwapSlot(PartyID, CurrentMaster, NewMaster);

//		pParty->changeMaster(dwNewMasterGaeaID);
	}
	else
	{
		if ( pParty->isMaster(dwGaeaID) == false )
			return FALSE;

		pParty->changeMaster(this, dwNewMasterGaeaID);

		// 파티장이 바뀐 것을을 필드 서버에 알림;
		SENDTOALLCHANNEL(pNetMsg);
	}   
    
    return TRUE;
}

BOOL GLAgentServer::MsgPartyDissolve ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
    GLMSG::SNET_PARTY_DISSOLVE* pNetMsg = (GLMSG::SNET_PARTY_DISSOLVE*) nmg;

	GLCharAG* const pChar = GetCharByGaeaID(dwGaeaID);
	if ( pChar == NULL )
		return FALSE; 

	const GLPartyID& PartyID = pNetMsg->PartyID;
	if ( PartyID.isValidExpedition() == true )
	{
		GLExpeditionAgent* pExpedition = m_pPartyManager->GetExpedition(PartyID);
		if ( pExpedition == NULL )
		{
			GLMSG::SNET_PARTY_ERROR_TO_CLIENT ErrorMsg(PartyID);
			ErrorMsg.emPartyErrorCode = EMPARTY_ERROR_NOEXPEDITION;
			SENDTOCLIENT(dwClientID, &ErrorMsg);
			pChar->ResetPartyID();
			pChar->ResetPartyOption();

			QuitPartyMap( pChar );

			return FALSE;
		}

		if ( pExpedition->isMaster(dwGaeaID) == false )
			return FALSE;


		return m_pPartyManager->DeleteExpedition(PartyID);
	}

    const GLPartyAgent* const pParty = GetParty(PartyID);
    if ( pParty == NULL )
	{
		GLMSG::SNET_PARTY_ERROR_TO_CLIENT ErrorMsg(PartyID);
		ErrorMsg.emPartyErrorCode = EMPARTY_ERROR_NOPARTY;
		SENDTOCLIENT(dwClientID, &ErrorMsg);
		pChar->ResetPartyID();
		pChar->ResetPartyOption();

		QuitPartyMap( pChar );

        return FALSE;
	}

    if ( pParty->isMaster(dwGaeaID) == false )
	{
		GLMSG::SNET_PARTY_ERROR_TO_CLIENT ErrorMsg(PartyID);
		ErrorMsg.emPartyErrorCode = EMPARTY_ERROR_NOMEMBER;
		SENDTOCLIENT(dwClientID, &ErrorMsg);
		pChar->ResetPartyID();
		pChar->ResetPartyOption();

		QuitPartyMap( pChar );

        return FALSE;
	}

    // 파티를 해체 한다.
    //
    DeleteParty(PartyID);

    return TRUE;
}

BOOL GLAgentServer::MsgPartyLinkRefuse( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
    GLMSG::SNET_PARTY_LINK_REFUSE* pNetMsg = (GLMSG::SNET_PARTY_LINK_REFUSE*) nmg;

    GLMSG::SNET_PARTY_LINK_REFUSE_FB NetMsgFB;
    GLCharAG* pChar = GetChar(pNetMsg->dwGaeaID);
    if (!pChar)
        return FALSE;

    GLCharAG* pMaster = GetChar(dwGaeaID);
    if (!pMaster)
        return FALSE;

    StringCchCopy(NetMsgFB.szName, CHAR_SZNAME, pMaster->m_szName);
    SENDTOCLIENT(pChar->ClientSlot(), &NetMsgFB);

    return TRUE;
}

BOOL GLAgentServer::MsgPartyInfoReqCA(NET_MSG_GENERIC* pMsg, DWORD dwClientID, DWORD dwGaeaID)
{
    // NET_MSG_PARTY_INFO_REQ_CA
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) pMsg;
    party::NET_PARTY_INFO_REQ_CA RecvData;    
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return FALSE;

    const GLPartyAgent* const pParty = GetParty(RecvData.PartyID);
    if (pParty)
    {
		const GLPartyID& PartyID = pParty->GetPartyID();
        party::NET_PARTY_INFO_REQ_AC SendData;
		SendData.PartyID = PartyID;

        const GLPartyAgent::MEMBER& PartyMember = pParty->GetMember();
        for (size_t i=0; i<PartyMember.size(); ++i)
        {
            GLCharAG* pChar = GetCharByGaeaID(PartyMember[i]);
            if (pChar)
                SendData.AddMember(pChar->ChaDbNum(), pChar->GetClass(), pChar->GetChaLevel(),  pChar->GetChaName());
        }

        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, SendData);
        SENDTOCLIENT(dwClientID, NET_MSG_PARTY_INFO_REQ_AC, SendBuffer);
    }
    else
    {
        party::NET_PARTY_INFO_REQ_AC SendData;
        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, SendData);
        SENDTOCLIENT(dwClientID, NET_MSG_PARTY_INFO_REQ_AC, SendBuffer);
    }
    
    return TRUE;
}

BOOL GLAgentServer::MsgExpeditionNew(NET_MSG_GENERIC* pMsg, DWORD dwClientID, DWORD dwGaeaID)
{
	if ( GLUseFeatures::GetInstance().IsUsingExpeditionWindowRenewal() == false )
		return FALSE;

	GLMSG::SNET_EXPEDITION_NEW_CA* pNetMsg = (GLMSG::SNET_EXPEDITION_NEW_CA*)pMsg;
	GLCharAG* pChar = GetChar(dwGaeaID);
	if ( pChar == NULL )
		return FALSE;	

	GLMSG::SNET_EXPEDITION_ERROR sMsgError;

	const GLPartyID& PartyID = pNetMsg->PartyID;
	if ( pChar->GetPartyID() != PartyID )
	{
		SENDTOCLIENT( pChar->ClientSlot(), &sMsgError );
		return FALSE;
	}

	if( PartyID.isValidExpedition() == true )
	{
		sMsgError.emError = EMEXPEDITION_ERROR_NEW_ALREADY;
		SENDTOCLIENT( pChar->ClientSlot(), &sMsgError );
		return FALSE;
	}

	GLPartyAgent* pParty(m_pPartyManager->GetParty(PartyID));
	if ( pParty == NULL )
	{
		pChar->ResetPartyID();
		sMsgError.emError = EMEXPEDITION_ERROR_NEW_PARTYMAP;
		SENDTOCLIENT( pChar->ClientSlot(), &sMsgError );
		return FALSE;
	}

	const GLPartyAgent::MEMBER& _members(pParty->GetMember());
	const DWORD _nMember(pParty->getNPartyMember());
	for ( unsigned int _index(_nMember); _index--; )
	{
		const DWORD dwMemberGaeaID(_members[_index]);
		const GLCharAG* const pMember(GetChar(dwMemberGaeaID));
		if ( pMember == NULL )
			continue;

		const MapID& _currentMapID(pMember->GetCurrentMap());
		const GLAGLandManager* const pLandManager(GetLandManager(_currentMapID.getGaeaMapID()));
		if ( pLandManager == NULL )
		{
			sMsgError.emError = EMEXPEDITION_ERROR_INVALID_LAND;
			SENDTOCLIENT( pChar->ClientSlot(), &sMsgError );
			return FALSE;
		}


		const SMAPNODE* const pMapNode(m_sMapList.FindMapNode( _currentMapID.getBaseMapID()));
		if( NULL == pMapNode )
		{
			sMsgError.emError = EMEXPEDITION_ERROR_INVALID_LAND;
			SENDTOCLIENT( pChar->ClientSlot(), &sMsgError );
			return FALSE;
		}

		const bool bPartyMap(pMapNode->IsPartyMap());
		const bool bExpeditionMap(pMapNode->IsExpeditionMap());
		const bool bInstantMap(pMapNode->IsInstantMap());
		if ( bPartyMap || bExpeditionMap || bInstantMap )		
		{
			sMsgError.emError = EMEXPEDITION_ERROR_NEW_PARTYMAP;
			SENDTOCLIENT( pChar->ClientSlot(), &sMsgError );
			return FALSE;
		}
	}
	

	GLExpeditionAgent* pExpedition = m_pPartyManager->NewExpedition(PartyID, dwGaeaID);
	if ( pExpedition == NULL )
	{
		SENDTOCLIENT( pChar->ClientSlot(), &sMsgError );
		return FALSE;
	}

	GLMSG::SNET_EXPEDITION_NEW_FB NewExpeditionMsg;	
	NewExpeditionMsg.dwMasterGaeaID = dwGaeaID;
	NewExpeditionMsg.dwMyPartyIndex = 0;
	for ( DWORD _i = MAX_PARTYGROUP; _i; --_i )	
		NewExpeditionMsg.PartyIDs[_i - 1] = pExpedition->GetPartyID(_i - 1);

	NewExpeditionMsg.MasterInfo = GLPARTY_FNET(
		dwGaeaID, 
		pChar->ChaDbNum(), 
		pChar->GetChaName(), 
		pChar->GetSchool(), 
		pChar->GetChaLevel(), 
		pChar->GetClass(), 
		pChar->GetCurrentMap());

	SENDTOALLCHANNEL(&NewExpeditionMsg);
	
	return TRUE;
}

BOOL GLAgentServer::MsgPartyChangeOption(NET_MSG_GENERIC* pMsg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLMSG::SNET_PARTY_CHANGE_OPTION* pNetMsg = (GLMSG::SNET_PARTY_CHANGE_OPTION*)pMsg;
	const GLPartyID& PartyID = pNetMsg->PartyID;

	if ( PartyID.isValidExpedition() == true )
	{
		const GLExpeditionAgent* const pExpedition = GetExpedition(PartyID);
		if ( pExpedition == NULL )
			return FALSE;

		if ( pExpedition->isMaster(dwGaeaID) == false )
			return FALSE;

		for ( DWORD _i = MAX_PARTYGROUP; _i; --_i )
		{
			const GLPartyID& ExpeditionPartyID = pExpedition->GetPartyID(_i - 1);
			GLPartyAgent* pParty = m_pPartyManager->GetParty(ExpeditionPartyID);
			GASSERT(pParty);
			pParty->m_PartyOption = pNetMsg->PartyOption;
		}		
	}
	else if ( PartyID.isValidParty() ==true )
	{
		GLPartyAgent* pParty = m_pPartyManager->GetParty(PartyID);
		if ( pParty == NULL )
			return FALSE;

		pParty->m_PartyOption = pNetMsg->PartyOption;
	}
	else
		return FALSE;

	SENDTOALLCHANNEL(pNetMsg);

	return TRUE;
}

BOOL GLAgentServer::MsgExpeditionCheckReadyRequest(NET_MSG_GENERIC* pMsg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLMSG::SNET_EXPEDITION_CHECK_READY_REQUEST_CA* pNetMsg = 
		(GLMSG::SNET_EXPEDITION_CHECK_READY_REQUEST_CA*)pMsg;

	const GLPartyID& PartyID = pNetMsg->PartyID;
	if ( PartyID.isValidExpedition() == false )
		return FALSE;	

	if ( pNetMsg->dwMasterGaeaID != dwGaeaID )
		return FALSE;

	const GLExpeditionAgent* const pExpedition = GetExpedition(PartyID);
	if ( pExpedition == NULL )
		return FALSE;

	if ( pExpedition->isMaster(pNetMsg->dwMasterGaeaID) == false )
		return FALSE;
	
	m_pPartyManager->SendMsgToMember(PartyID, pMsg);

	return TRUE;
}
BOOL GLAgentServer::MsgExpeditionCheckReadyAnswer(NET_MSG_GENERIC* pMsg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLMSG::SNET_EXPEDITION_CHECK_READY_ANSWER_CA* pNetMsg = 
		(GLMSG::SNET_EXPEDITION_CHECK_READY_ANSWER_CA*)pMsg;
	GLCharAG* const pChar = GetCharByGaeaID(pNetMsg->dwMasterGaeaID);		
	if ( pChar == NULL )
		return FALSE;

	SENDTOCLIENT(pChar->ClientSlot(), pMsg);

	return TRUE;
}
BOOL GLAgentServer::MsgExpeditionSwapSlot(NET_MSG_GENERIC* pMsg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLMSG::SNET_EXPEDITION_SWAP_SLOT* pNetMsg = (GLMSG::SNET_EXPEDITION_SWAP_SLOT*)pMsg;

	const GLPartyID& PartyID = pNetMsg->PartyID;
	if ( PartyID.isValidExpedition() == false )
		return FALSE;	

	GLExpeditionAgent* const pExpedition = m_pPartyManager->GetExpedition(PartyID);
	if ( pExpedition == NULL )
		return FALSE;

	if ( pExpedition->isMaster(dwGaeaID) == false )
		return FALSE;

	if ( m_pPartyManager->SwapSlot(pNetMsg->PartyID, pNetMsg->SwapSlotFrom, pNetMsg->SwapSlotTo) == false )
		return FALSE;

	return TRUE;
}

const bool GLAgentServer::OfflinePartyMember(const DWORD dwGaeaID, const DWORD dwDbNum, const GLPartyID& PartyID)
{
	m_pPartyManager->removePartyMember(PartyID, dwGaeaID);

	return true;
}



// dwGaeaID = MasterGaeaID
BOOL GLAgentServer::MsgPartyLinkLure( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNET_PARTY_LINK_LURE* pNetMsg = (GLMSG::SNET_PARTY_LINK_LURE*) nmg;

	const DWORD dwGaeaIDMaster(dwGaeaID);
	const DWORD dwGaeaIDTarget(pNetMsg->dwGaeaID);
	GLCharAG* const pCharMaster(GetChar(dwGaeaIDMaster));
	if ( pCharMaster == NULL )
		return FALSE;

	GLCharAG* const pCharTarget(GetChar(dwGaeaIDTarget));
	if ( pCharTarget == NULL )
		return FALSE;

	const DWORD dwClientIDMaster(pCharMaster->ClientSlot());
	const DWORD dwClientIDTarget(pCharTarget->ClientSlot());	
	const DWORD dwTimeKey(pNetMsg->dwTimeKey + 120 * 1000); // 제한 시간 2분;

	EncryptPack encryptPack;
	encryptPack.dwMasterClientSlot = dwClientIDMaster;
	encryptPack.dwTimeKey = pNetMsg->dwTimeKey;
	encryptPack.dwTargetClientSlot = dwClientIDTarget;
	const DWORD dwHashKey(HASH::generateHashKey(&encryptPack, sizeof(EncryptPack)));
	if ( dwHashKey != pNetMsg->dwHashKey )
	{
		sc::writeLogError(sc::string::format("%1%(%2%) : Attempt for forcing party has been detected as a party invitation response has been forged.;", pCharTarget->GetChaName(), pCharTarget->ChaDbNum()));
		return FALSE;
	}

	if ( dwTimeKey < (DWORD)::clock() )
	{
		GLMSG::SNET_PARTY_LURE_FB NetMsgFB(dwGaeaID, EMPARTY_LURE_TIME_OVER );
		SENDTOCLIENT(dwClientIDMaster, &NetMsgFB );
		return FALSE;
	}


	const BOOL bResult = JoinParty(dwGaeaID, pNetMsg->dwGaeaID, pNetMsg->sOption);

	return bResult;
}

// dwGaeaID = TargetGaeaID;
BOOL GLAgentServer::MsgPartyLureTarAns ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNET_PARTY_LURE_TAR_ANS* pNetMsg = (GLMSG::SNET_PARTY_LURE_TAR_ANS*) nmg;

	const DWORD dwGaeaIDMaster(pNetMsg->dwMasterID);
	const DWORD dwGaeaIDTarget(dwGaeaID);
	GLCharAG* const pCharMaster(GetChar(dwGaeaIDMaster));
	if ( pCharMaster == NULL )
		return FALSE;

	GLCharAG* const pCharTarget(GetChar(dwGaeaIDTarget));
	if ( pCharTarget == NULL )
		return FALSE;

	const DWORD dwClientIDMaster(pCharMaster->ClientSlot());
	const DWORD dwClientIDTarget(pCharTarget->ClientSlot());
	if ( pNetMsg->emAnswer != EMPARTY_LURE_OK )
	{		
		if ( pCharMaster->m_sReqPartyOpt != pNetMsg->partyOption )
		{
			GLMSG::SNET_PARTY_LURE_FB NetMsgFB(dwGaeaID, EMPARTY_LURE_OPT );
			SENDTOCLIENT(dwClientIDMaster, &NetMsgFB );
			return FALSE;
		}

		GLMSG::SNET_PARTY_LURE_FB NetMsgFB(dwGaeaIDTarget, pNetMsg->emAnswer);
		SENDTOCLIENT(dwClientIDMaster, &NetMsgFB );
		return FALSE;
	}
	const DWORD dwTimeKey(pNetMsg->dwTimeKey + 300 * 1000); // 제한 시간 5분;

	EncryptPack encryptPack;
	encryptPack.dwMasterClientSlot = dwClientIDMaster;
	encryptPack.dwTimeKey = pNetMsg->dwTimeKey;
	encryptPack.dwTargetClientSlot = dwClientIDTarget;
	const DWORD dwHashKey(HASH::generateHashKey(&encryptPack, sizeof(EncryptPack)));
	if ( dwHashKey != pNetMsg->dwHashKey )
	{
		sc::writeLogError(sc::string::format("%1%(%2%) : Attempt for forcing party has been detected as a party invitation response has been forged.;", pCharTarget->GetChaName(), pCharTarget->ChaDbNum()));
		return FALSE;
	}

	if ( dwTimeKey < (DWORD)::clock() )
	{
		GLMSG::SNET_PARTY_LURE_FB NetMsgFB(dwGaeaID, EMPARTY_LURE_TIME_OVER );
		SENDTOCLIENT(dwClientIDTarget, &NetMsgFB );
		return FALSE;
	}

	const BOOL bResult = JoinParty(dwGaeaIDMaster, dwGaeaID, pNetMsg->partyOption);
	return bResult;
}


BOOL GLAgentServer::CompareTargetSlot( DWORD dwClientID, DWORD dwTargetGaeaID )
{
	GLCharAG*	const pTargetCharacter = GetChar( dwTargetGaeaID );
	if ( pTargetCharacter == NULL )	
		return FALSE;

	if ( dwClientID == pTargetCharacter->ClientSlot() )
	{
		sc::writeLogError(sc::string::format("파티 패킷 조작 의심 캐릭터 : %1%", pTargetCharacter->GetChaName()));
		return FALSE;
	}

	return TRUE;
}

BOOL GLAgentServer::MsgPartyLure ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNET_PARTY_LURE* pNetMsg = (GLMSG::SNET_PARTY_LURE*) nmg;	

	// 패킷 조작을 통한 강제 파티 초대를 막기위해서 확인한다.
	if ( CompareTargetSlot(dwClientID, pNetMsg->dwGaeaID) == FALSE ) return FALSE;

	return MsgPartyInviteRequest(dwGaeaID, pNetMsg->dwGaeaID, pNetMsg->sOption, false);
}

BOOL GLAgentServer::MsgPartyLureWithCharNum(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLMSG::SNET_PARTY_LURE_WITH_CHARNUM* pNetMsg = (GLMSG::SNET_PARTY_LURE_WITH_CHARNUM*) nmg;

	GLCharAG* pTargetChar = GetCharByDbNum( pNetMsg->dwChaNum );
	if ( pTargetChar == NULL )
		return FALSE;	

	return MsgPartyInviteRequest(dwGaeaID, pTargetChar->GaeaId(), pNetMsg->sOption, false);
}

BOOL GLAgentServer::MsgPartyLinkJoin(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLMSG::SNET_PARTY_LINK_JOIN* pNetMsg = (GLMSG::SNET_PARTY_LINK_JOIN*) nmg;

	GLCharAG*	const pMasterChar = GetChar(pNetMsg->dwMasterID);
	if ( pMasterChar == NULL )	
		return FALSE;

		// 패킷 조작을 통한 강제 파티 초대를 막기위해서 확인한다.
	if ( CompareTargetSlot(dwClientID, pNetMsg->dwMasterID) == FALSE ) return FALSE;

	return MsgPartyInviteRequest(pNetMsg->dwMasterID,  dwGaeaID, pMasterChar->m_sReqPartyOpt);
}

BOOL GLAgentServer::MsgPartyInviteRequest(
							const DWORD dwMasterGaeaID,
							const DWORD dwTargetGaeaID,
							const SPartyOption& PartyOption,
							const bool bClient2Master /*= true*/)
{
	GLCharAG*	const pTargetCharacter = GetChar( dwTargetGaeaID );
	if ( pTargetCharacter == NULL )	
		return FALSE;

	GLCharAG*	const pMasterCharacter = GetChar( dwMasterGaeaID );
	if ( pMasterCharacter == NULL )	
		return FALSE;

	const DWORD dwTargetClientSlot = pTargetCharacter->ClientSlot();
	const DWORD dwMasterClientSlot = pMasterCharacter->ClientSlot();
	const DWORD dwSlot = bClient2Master ? dwTargetClientSlot : dwMasterClientSlot;
	
	const GLPartyID& MasterPartyID = pMasterCharacter->GetPartyID();
	const GLPartyID& TargetPartyID = pTargetCharacter->GetPartyID();

	const SMAPNODE* pMapNode = FindMapNode( pTargetCharacter->GetCurrentMap().getGaeaMapID());

	if( PRISONLIMIT.IsLimitStatus(pMapNode, pTargetCharacter->m_dwUserLvl, PML_PARTY) )
	{
		GLMSG::SNET_PARTY_ERROR_TO_CLIENT NetErrMsg(TargetPartyID);
		NetErrMsg.emPartyErrorCode = EMPARTY_ERROR_PRISON;
		NetErrMsg.addCharName( pTargetCharacter->GetChaName() );
		SENDTOCLIENT(dwSlot, &NetErrMsg);
		return FALSE;
	}

	if ( TargetPartyID.isValidParty() == true )
	{
		GLMSG::SNET_PARTY_ERROR_TO_CLIENT NetErrMsg(TargetPartyID);
		NetErrMsg.emPartyErrorCode = EMPARTY_ERROR_LINK_REJOIN;
		NetErrMsg.addCharName( pTargetCharacter->GetChaName() );
		SENDTOCLIENT(dwSlot, &NetErrMsg);
		return FALSE;
	}

	std::string strMasterName = stringformat( "%1%", pMasterCharacter->GetChaName() );
	std::string strCharTarName = stringformat( "%1%", pTargetCharacter->GetChaName() );

	std::tr1::shared_ptr<GLClubAgent> pCLUB = m_pClubMan->GetClub ( pTargetCharacter->m_ClubDbNum );
	std::tr1::shared_ptr<GLClubAgent> pCLUB_M = m_pClubMan->GetClub ( pMasterCharacter->m_ClubDbNum );
	// 클럽 체크
	if ( pCLUB )
	{
		bool bClubBattle = false;
		// 파티가 없을경우 ( 자신만 체크 ) 				
		if ( MasterPartyID.isValidParty() == false )
		{
			if ( pCLUB->IsBattle( pMasterCharacter->m_ClubDbNum ) )
				bClubBattle = true;
			if ( pCLUB_M && pCLUB->IsBattleAlliance( pCLUB_M->m_dwAlliance ) )
				bClubBattle = true;
		}
		//	있을경우 모든 멤버랑 체크
		else
		{
			const GLPartyAgent* const pParty = GetParty(MasterPartyID);

			if( pParty && (pParty->isValidParty() == true) )
			{
				GLPartyAgent::MEMBER_CITER iter = pParty->m_Member.begin();
				GLPartyAgent::MEMBER_CITER iter_end = pParty->m_Member.end();
				GLCharAG* pMemChar = NULL;

				for ( ; iter!=iter_end; ++iter )
				{
					pMemChar = GetChar( (*iter) );
					if ( pMemChar == NULL )
						continue;

					if ( pCLUB->IsBattle(pMemChar->m_ClubDbNum) )
					{
						bClubBattle = true;
						break;
					}

					std::tr1::shared_ptr<GLClubAgent> pClub_Mem = m_pClubMan->GetClub( pMemChar->m_ClubDbNum );

					if( pClub_Mem && pCLUB->IsBattleAlliance( pClub_Mem->m_dwAlliance ))
					{
						bClubBattle = true;
						break;
					}
				}
			}
		}

		if ( bClubBattle ) 
		{
			GLMSG::SNET_PARTY_ERROR_TO_CLIENT NetErrMsg( pMasterCharacter->GetPartyID() );
			NetErrMsg.emPartyErrorCode = bClient2Master ? EMPARTY_ERROR_LINK_CLUBBATTLE : EMPARTY_ERROR_LURE_CLUBBATTLE;
			SENDTOCLIENT ( dwSlot, &NetErrMsg );
			return FALSE;
		}
	}

	//! CTF 진행중에는 다른 학원과 할 수 없음
	if ( pMasterCharacter->m_bCTFPlayer != pTargetCharacter->m_bCTFPlayer)
	{
		GLMSG::SNET_PARTY_ERROR_TO_CLIENT NetErrMsg( pMasterCharacter->GetPartyID() );
		NetErrMsg.emPartyErrorCode = bClient2Master ? EMPARTY_ERROR_LINK_CTF : EMPARTY_ERROR_LURE_CTF;
		SENDTOCLIENT(dwSlot, &NetErrMsg );
		return FALSE;
	}
	else if ( pMasterCharacter->m_bCTFPlayer && pTargetCharacter->m_bCTFPlayer )
	{
		if ( pMasterCharacter->m_wSchool != pTargetCharacter->m_wSchool )
		{
			GLMSG::SNET_PARTY_ERROR_TO_CLIENT NetErrMsg( pMasterCharacter->GetPartyID() );
			NetErrMsg.emPartyErrorCode = bClient2Master? EMPARTY_ERROR_LINK_CTF : EMPARTY_ERROR_OTHERSCHOOL;
			SENDTOCLIENT(dwSlot, &NetErrMsg );
			return FALSE;
		}
	}

	// 다른 학원생과 파티 결성 제한 옵션이 있을 경우 제한함
	if ( !GLCONST_CHAR::bPARTY_2OTHERSCHOOL )
	{
		if ( pTargetCharacter->m_wSchool != pMasterCharacter->m_wSchool )
		{
			GLMSG::SNET_PARTY_ERROR_TO_CLIENT NetErrMsg( MasterPartyID );
			NetErrMsg.emPartyErrorCode = EMPARTY_ERROR_OTHERSCHOOL;
			SENDTOCLIENT( dwSlot, &NetErrMsg );
			return FALSE;
		}
	}
	
	

	// 파티의 유효성 체크
	// !파티가 만들어지지 않았을 경우(마스터 -> 타겟일 경우에만)
	if ( (bClient2Master == false) && MasterPartyID.isValidParty() == false )
	{
		pMasterCharacter->m_sReqPartyOpt = PartyOption;
		
		const DWORD dwTimeKey(::clock());
		EncryptPack encryptPack;
		encryptPack.dwMasterClientSlot = dwMasterClientSlot;
		encryptPack.dwTimeKey = dwTimeKey;
		encryptPack.dwTargetClientSlot = dwTargetClientSlot;
		const DWORD dwHashKey(HASH::generateHashKey(&encryptPack, sizeof(EncryptPack)));

		GLMSG::SNET_PARTY_LURE_TAR_WITH_CHARNAME NetMsgLureTar( GLPartyID(), dwHashKey, dwMasterGaeaID, dwTimeKey, strMasterName, pMasterCharacter->m_sReqPartyOpt );
		SENDTOCLIENT( dwTargetClientSlot, &NetMsgLureTar );		
		return TRUE;
	}	

	if ( MasterPartyID.isValidExpedition() == true )
	{
		GLExpeditionAgent* const pExpedition = m_pPartyManager->GetExpedition(MasterPartyID);
		if ( pExpedition == NULL )
		{
			GLMSG::SNET_PARTY_ERROR_TO_CLIENT ErrorMsg(MasterPartyID);
			ErrorMsg.emPartyErrorCode = EMPARTY_ERROR_NOPARTY;
			SENDTOCLIENT(dwSlot, &ErrorMsg);
			pMasterCharacter->ResetPartyID();
			return FALSE;
		}

		GLMSG::SNET_PARTY_LINK_JOIN_FB NetMsgFB;
		if ( pExpedition->isMaster(dwMasterGaeaID) == false && pExpedition->isSubMaster(dwMasterGaeaID) == false)
		{
			NetMsgFB.emFB = EMPARTY_LINK_JOIN_FB_NOMASTER;
			SENDTOCLIENT ( dwTargetClientSlot, &NetMsgFB );

			return FALSE;
		}

		bool bFullExpedition = true;
		for ( DWORD _i = MAX_PARTYGROUP; _i; --_i )
		{
			const GLPartyID& ExpeditionPartyID = pExpedition->GetPartyID(_i - 1);
			GLPartyAgent* pParty = m_pPartyManager->GetParty(ExpeditionPartyID);
			if ( pParty == NULL )
				continue;

			if ( pParty->isFull() == false )
			{
				bFullExpedition = false;
				break;
			}
		}

		if ( bFullExpedition == true )
		{
			NetMsgFB.emFB = EMPARTY_LINK_JOIN_FB_FULL;
			SENDTOCLIENT ( dwTargetClientSlot, &NetMsgFB );
			return FALSE;
		}

		pExpedition->addPreMember(dwTargetGaeaID);			
	}
	else if ( MasterPartyID.isValidParty() == true )
	{
		GLPartyAgent* pParty = m_pPartyManager->GetParty(MasterPartyID);
		if ( pParty == NULL )
		{
			GLMSG::SNET_PARTY_ERROR_TO_CLIENT ErrorMsg(MasterPartyID);
			ErrorMsg.emPartyErrorCode = EMPARTY_ERROR_NOPARTY;
			SENDTOCLIENT(dwMasterClientSlot, &ErrorMsg);
			pMasterCharacter->ResetPartyID();
			return FALSE;
		}

		if ( pParty->isMaster(dwMasterGaeaID) == false )
			return FALSE;

		if ( pParty->isFull() == true )
		{
			GLMSG::SNET_PARTY_ERROR_TO_CLIENT NetErrMsg(MasterPartyID);
			NetErrMsg.emPartyErrorCode = bClient2Master ? EMPARTY_ERROR_LINK_FULLPARTY : EMPARTY_ERROR_LURE_FULLPARTY;

			if ( bClient2Master == false )
				NetErrMsg.addCharName(strCharTarName.c_str());

			SENDTOCLIENT( dwSlot, &NetErrMsg );
			return FALSE;
		}		
	}

	const DWORD dwTimeKey(::clock());
	EncryptPack encryptPack;
	encryptPack.dwMasterClientSlot = dwMasterClientSlot;
	encryptPack.dwTimeKey = dwTimeKey;
	encryptPack.dwTargetClientSlot = dwTargetClientSlot;
	const DWORD dwHashKey(HASH::generateHashKey(&encryptPack, sizeof(EncryptPack)));

	// 최종 확인 패킷 발사
	if( bClient2Master )
	{
		GLMSG::SNET_PARTY_LINK_JOIN_ANS NetMsgAns(dwHashKey, dwTargetGaeaID, dwTimeKey, pTargetCharacter->m_emClass );
		StringCchCopy( NetMsgAns.szName, CHAR_SZNAME, pTargetCharacter->m_szName );
		SENDTOCLIENT ( pMasterCharacter->ClientSlot(), &NetMsgAns );
	}
	else
	{
		pMasterCharacter->m_sReqPartyOpt = PartyOption;
		// 대상에게 파티 참가 요청
		GLMSG::SNET_PARTY_LURE_TAR_WITH_CHARNAME NetMsgLureTar(MasterPartyID, dwHashKey, dwMasterGaeaID, dwTimeKey, strMasterName, pMasterCharacter->m_sReqPartyOpt );

		SENDTOCLIENT( pTargetCharacter->ClientSlot(), &NetMsgLureTar );
	}

	/*if( pMasterCharacter->m_bTracingUser || pTargetCharacter->m_bTracingUser )
	{
		CDebugSet::ToTracingFile( pMasterCharacter->m_szUserName, "Party Request, [%s][%s], Target Char: [%s][%s]",
			pMasterCharacter->m_szUserName, pMasterCharacter->m_szName, pTargetCharacter->m_szUserName, pTargetCharacter->m_szName );
	}*/

	return TRUE;
}

