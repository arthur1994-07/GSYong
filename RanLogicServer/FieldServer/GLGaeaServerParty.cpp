#include "../pch.h"
#include "../../SigmaCore/gassert.h"
#include "../Party/GLPartyField.h"
#include "../Party/GLPartyManagerField.h"
#include "../Party/GLPartyTenderManagerField.h"
#include "./GLGaeaServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

BOOL GLGaeaServer::MsgParty(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	return m_pPartyFieldMan->MsgProcess(nmg, dwGaeaID);
}

BOOL GLGaeaServer::MsgPartyTender(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	return m_pPartyTenderManager->MsgProcess(nmg);
}

void GLGaeaServer::FrameMovePartyTender(float fTime, float fElapsedTime)
{
	if ( m_pPartyTenderManager == NULL )
		return;

	m_pPartyTenderManager->FrameMove(fTime, fElapsedTime);
}

GLPartyField* const GLGaeaServer::GetParty(const GLPartyID& PartyID)
{
	return m_pPartyFieldMan->GetParty(PartyID);
}

GLPartyManagerField* GLGaeaServer::GetPartyManager()
{ 
    return m_pPartyFieldMan;
}

GLPartyTenderManagerField* const GLGaeaServer::GetPartyTenderItemManager(void) const
{
	return m_pPartyTenderManager;
}

void GLGaeaServer::SENDTOPARTYCLIENT(const GLPartyID& PartyID, const MapID& mapID, NET_MSG_GENERIC* nmg)
{
    m_pPartyFieldMan->SendMsgToMember(PartyID, mapID, nmg);
	
}

void GLGaeaServer::SENDTOPARTYCLIENT(const GLPartyID& PartyID, const MapID& mapID, EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc)
{
    m_pPartyFieldMan->SendMsgToMember(PartyID, mapID, MsgType, Buffer, UseCrc);
}
void GLGaeaServer::SENDTOPARTYCLIENT(const GLPartyID& PartyID, const MapID& mapID, const D3DXVECTOR3& vReferencePosition, const float fRange, NET_MSG_GENERIC* nmg)
{
	m_pPartyFieldMan->SendMsgToMember(PartyID, mapID, vReferencePosition, fRange, nmg);
}
void GLGaeaServer::SENDTOPARTYCLIENT(const GLPartyID& PartyID, const MapID& mapID, const D3DXVECTOR3& vReferencePosition, const float fRange, EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc)
{
	m_pPartyFieldMan->SendMsgToMember(PartyID, mapID, vReferencePosition, fRange, MsgType, Buffer, UseCrc);
}


BOOL GLGaeaServer::RequestConfrontPartyChkMbr(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    GLMSG::SNETPC_CONFRONTPTY_CHECKMBR2_FLD *pNetMsg = (GLMSG::SNETPC_CONFRONTPTY_CHECKMBR2_FLD *)nmg;
    const SNATIVEID &sMapID = pNetMsg->sMapID;

    GLLandMan* pLandMan = GetLand ( sMapID );
    if (!pLandMan)
        return FALSE;

    GLPartyField* const pPartyA = GetParty ( pNetMsg->PartyID_A );
    if (!pPartyA)
        return FALSE;

    GLPartyField* const pPartyB = GetParty ( pNetMsg->PartyID_B );
    if (!pPartyB)
        return FALSE;

    GLChar* pCharA = GetChar(pPartyA->GetMasterGaeaID());
    GLChar* pCharB = GetChar(pPartyB->GetMasterGaeaID());

    if ( !pCharA || !pCharB )
    {
        GLMSG::SNETPC_REQ_CONFRONT_FB NetMsgFB(EMCONFT_PARTY, pNetMsg->PartyID_A.GetID(), EMCONFRONT_FAIL, 0.0f);

        if (pCharA)
		{
			NetMsgFB.dwChaNum = pCharA->CharDbNum();
            SENDTOAGENT(&NetMsgFB);	//pCharA->ClientSlot()
		}
        if (pCharB)
		{
			NetMsgFB.dwChaNum = pCharB->CharDbNum();
            SENDTOAGENT(&NetMsgFB);	//pCharB->ClientSlot()
		}

        return FALSE;
    }

    D3DXVECTOR3 vPos = ( pCharA->GetPosition() + pCharB->GetPosition() ) * 0.5f;
    GLMSG::SNETPC_CONFRONTPTY_CHECKMBR2_AGT NetMsgCheckMbr( pNetMsg->sMapID, vPos, pNetMsg->PartyID_A, pNetMsg->PartyID_B, pNetMsg->sOption );

    bool bSCHOOL_CONFT_LEVEL(true);
    int nCheck(0);
    GLPartyField::MEMBER_ITER iter, end;

	PartyMemberGaeaIDs arPartyMemberGaeaIDs;
	const DWORD nMemberA = pPartyA->GetPartyMemberGaeaIDs(arPartyMemberGaeaIDs);    
	for ( DWORD _i = 0; _i < nMemberA; ++_i )
    {
        GLChar* pMember = GetChar(arPartyMemberGaeaIDs[_i]);
        if (!pMember)
            continue;
        if (pNetMsg->sMapID != pMember->GetCurrentMap().getGaeaMapID())
            continue;

        if (pMember->m_wLevel < GLCONST_CHAR::wCONFRONT_SCHOOL_LIMIT_LEVEL)
            bSCHOOL_CONFT_LEVEL = false;

        const D3DXVECTOR3 vDistance = pMember->GetPosition() - NetMsgCheckMbr.vPosition;
        const float fDistance = D3DXVec3Length ( &vDistance );

        if ( fDistance < GLCONST_CHAR::fCONFRONT_PY_DIST )
        {
            NetMsgCheckMbr.dwPARTY_A_MEMBER[nCheck++] = pMember->GetGaeaID();
        }
    }

    nCheck = 0;
	const DWORD nMemberB = pPartyB->GetPartyMemberGaeaIDs(arPartyMemberGaeaIDs);    
	for ( DWORD _i = 0; _i < nMemberB; ++_i )
    {
        GLChar* pMember = GetChar(arPartyMemberGaeaIDs[_i]);
        if (!pMember)
            continue;
        if (pNetMsg->sMapID != pMember->GetCurrentMap().getGaeaMapID())
            continue;

        if (pMember->m_wLevel < GLCONST_CHAR::wCONFRONT_SCHOOL_LIMIT_LEVEL)
            bSCHOOL_CONFT_LEVEL = false;

        const D3DXVECTOR3 vDistance = pMember->GetPosition() - NetMsgCheckMbr.vPosition;
        const float fDistance = D3DXVec3Length ( &vDistance );

        if ( fDistance < GLCONST_CHAR::fCONFRONT_PY_DIST )
        {
            NetMsgCheckMbr.dwPARTY_B_MEMBER[nCheck++] = pMember->GetGaeaID();
        }
    }

    NetMsgCheckMbr.sOption.bSCHOOL = bSCHOOL_CONFT_LEVEL;

    SENDTOAGENT(&NetMsgCheckMbr);

    return TRUE;
}

BOOL GLGaeaServer::RequestConfrontParty(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    GLMSG::SNETPC_CONFRONTPTY_START2_FLD *pNetMsg = (GLMSG::SNETPC_CONFRONTPTY_START2_FLD *)nmg;
    const SNATIVEID &sMapID = pNetMsg->sMapID;

    GLLandMan* pLandMan = GetLand(sMapID);
    if ( !pLandMan )
		return FALSE;

    GLPartyField* const pPartyA = GetParty(pNetMsg->PartyID_A);
    if ( !pPartyA )	
		return FALSE;

    GLPartyField* const  pPartyB = GetParty(pNetMsg->PartyID_B);
    if ( !pPartyB )
		return FALSE;

    GLChar* pCharA = GetChar(pPartyA->GetMasterGaeaID());
    GLChar* pCharB = GetChar(pPartyB->GetMasterGaeaID());

    if ( !pCharA || !pCharB )
    {
        GLMSG::SNETPC_REQ_CONFRONT_FB NetMsgFB(EMCONFT_PARTY, pNetMsg->PartyID_A.GetID(), EMCONFRONT_FAIL, 0.0f);

        if (pCharA)
		{
			NetMsgFB.dwChaNum = pCharA->CharDbNum();
            SENDTOAGENT(&NetMsgFB);	//pCharA->ClientSlot()
		}
        if (pCharB)
		{
			NetMsgFB.dwChaNum = pCharB->CharDbNum();
            SENDTOAGENT(&NetMsgFB);	//pCharB->ClientSlot()
		}

        return FALSE;
    }

    const D3DXVECTOR3& vPosition = pNetMsg->vPos;

    //	Note : 학교 대련.
    WORD wSCHOOL_A(0);
    WORD wSCHOOL_B(0);
    if ( pNetMsg->sOption.bSCHOOL )
    {
        wSCHOOL_A = pCharA->m_wSchool;
        wSCHOOL_B = pCharB->m_wSchool;
    }

    //	Note : 첫번째 파티원들에 '파티대련' 정보 설정.
    //
	pPartyA->SetConfrontOption(pNetMsg->sOption);
	pPartyA->SetConfrontPartyID(pNetMsg->PartyID_B);

    GLMSG::SNETPC_CONFRONTPTY_START2_CLT sNetMsgClt( pNetMsg->PartyID_B.GetID(), vPosition, pNetMsg->sOption );

    memcpy ( sNetMsgClt.dwCONFRONT_MY_MEMBER, pNetMsg->dwPARTY_A_MEMBER, sizeof(DWORD)*MAXPARTY );
    memcpy ( sNetMsgClt.dwCONFRONT_TAR_MEMBER, pNetMsg->dwPARTY_B_MEMBER, sizeof(DWORD)*MAXPARTY );
    sNetMsgClt.sOption.wMY_SCHOOL = wSCHOOL_A;  
    sNetMsgClt.sOption.wTAR_SCHOOL = wSCHOOL_B;

    if ( pCharB )
    {
        StringCbCopy(sNetMsgClt.szName, CHR_ID_LENGTH+1, pCharB->m_szName);		
    }

    int i(0);
    DWORD dwMember = 0;
    GLChar* pChar = NULL;
    for ( i=0; i<MAXPARTY; ++i )
    {
        dwMember = pNetMsg->dwPARTY_A_MEMBER[i];
        if ( GAEAID_NULL==dwMember )	continue;
        pChar = GetChar ( dwMember );
        if ( !pChar )
        {
            continue;
        }
        else
        {
            pPartyA->MBR_CONFRONT_JOIN ( dwMember );

            pChar->m_sCONFTING.RESET();
            pChar->m_sCONFTING.emTYPE = EMCONFT_PARTY;
            pChar->m_sCONFTING.dwTAR_ID = pPartyA->GetConfrontPartyID().GetPartyID();
            pChar->m_sCONFTING.vPosition = vPosition;
            pChar->m_sCONFTING.sOption = pNetMsg->sOption;

            //	Note : 클라이언트들에 '파티대련' 정보 전송.
            //
            SENDTOCLIENT ( pChar->ClientSlot(), &sNetMsgClt );

            //	Note : 적대행위자 삭제.
            //
            pChar->RemoveAllPlayHostiles();

            //	Note : 대련 체력 백업.
            //
            pChar->DoConftStateBackup();
			if ( GLHIDESET::CheckHideSet(EMHIDE_BEING_PARTY) )
			{
				pChar->RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
				pChar->RestoreActStateByInvisible();
			}
        }
    }


    //	Note : 두번쩨 파티원들에 '파티대련' 정보 설정.
    //
    pPartyB->SetConfrontOption(pNetMsg->sOption);
    pPartyB->SetConfrontPartyID(pNetMsg->PartyID_A);

    sNetMsgClt.ConfrontPartyID = pNetMsg->PartyID_A;
    memcpy ( sNetMsgClt.dwCONFRONT_MY_MEMBER, pNetMsg->dwPARTY_B_MEMBER, sizeof(DWORD)*MAXPARTY );
    memcpy ( sNetMsgClt.dwCONFRONT_TAR_MEMBER, pNetMsg->dwPARTY_A_MEMBER, sizeof(DWORD)*MAXPARTY );
    sNetMsgClt.vPosition = vPosition;
    sNetMsgClt.sOption = pNetMsg->sOption;
    sNetMsgClt.sOption.wMY_SCHOOL = wSCHOOL_B;
    sNetMsgClt.sOption.wTAR_SCHOOL = wSCHOOL_A;

    if ( pCharA )
    {
        StringCbCopy(sNetMsgClt.szName, CHR_ID_LENGTH+1, pCharA->m_szName);
    }

    for ( i=0; i<MAXPARTY; ++i )
    {
        dwMember = pNetMsg->dwPARTY_B_MEMBER[i];
        if ( GAEAID_NULL==dwMember )	continue;
        pChar = GetChar ( dwMember );
        if ( !pChar )
        {
            continue;
        }
        else
        {
            pPartyB->MBR_CONFRONT_JOIN ( dwMember );

            pChar->m_sCONFTING.RESET();
            pChar->m_sCONFTING.emTYPE = EMCONFT_PARTY;
            pChar->m_sCONFTING.dwTAR_ID = pPartyB->GetConfrontPartyID().GetPartyID();
            pChar->m_sCONFTING.vPosition = vPosition;
            pChar->m_sCONFTING.sOption = pNetMsg->sOption;

            //	Note : 클라이언트들에 '파티대련' 정보 전송.
            //
            SENDTOCLIENT ( pChar->ClientSlot(), &sNetMsgClt );

            //	Note : 적대행위자 삭제.
            //
            pChar->RemoveAllPlayHostiles();

            //	Note : 대련 체력 백업.
            //
            pChar->DoConftStateBackup();
        }
    }

    const DWORD dwLevelA = pPartyA->GETAVER_CONFT_LEVEL(this);
    const DWORD dwLevelB = pPartyB->GETAVER_CONFT_LEVEL(this);

    return TRUE;
}

BOOL GLGaeaServer::RequestConfrontPartyEnd(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    GLMSG::SNETPC_CONFRONTPTY_END2_FLD *pNetMsg = (GLMSG::SNETPC_CONFRONTPTY_END2_FLD *)nmg;
    if ( pNetMsg->PartyID_A.isValidParty() && pNetMsg->PartyID_B.isValidParty() )
    {
        GLPartyField* const pPartyA = GetParty(pNetMsg->PartyID_A);
        GLPartyField* const pPartyB = GetParty(pNetMsg->PartyID_B);

        //	Note : 파티 대련후 파티가 사라진 부분이 있으면 오류 처리.
        //
        if ( pPartyA && pPartyB )
        {
            DWORD dwLevelA = pPartyA->GETAVER_CONFT_LEVEL(this);
            DWORD dwLevelB = pPartyB->GETAVER_CONFT_LEVEL(this);
            const SCONFT_OPTION& sOption = pPartyB->GetConfrontOption();

            float fLIFE_P_A(0), fLIFE_P_B(0);

            if ( GLCONST_CHAR::nCONFRONT_WIN_LP!=0 && GLCONST_CHAR::nCONFRONT_LOSS_LP )
            {
                if ( pNetMsg->emEND_A==EMCONFRONT_END_PWIN )
                {
                    fLIFE_P_A = GLCONST_CHAR::nCONFRONT_WIN_LP + (dwLevelB-dwLevelA)/2.0f
                        + (sOption.fTAR_HP_RATE-sOption.fHP_RATE);
                    LIMIT ( fLIFE_P_A, 20.0f, 0.0f );

                    fLIFE_P_B = GLCONST_CHAR::nCONFRONT_LOSS_LP - (dwLevelB-dwLevelA)/2.0f
                        - (sOption.fTAR_HP_RATE-sOption.fHP_RATE);
                    LIMIT ( fLIFE_P_B, 0.0f, -20.0f );
                }
                else if ( pNetMsg->emEND_B==EMCONFRONT_END_PWIN )
                {
                    fLIFE_P_B = GLCONST_CHAR::nCONFRONT_WIN_LP + (dwLevelA-dwLevelB)/2.0f
                        + (sOption.fHP_RATE-sOption.fTAR_HP_RATE);
                    LIMIT ( fLIFE_P_B, 20.0f, 0.0f );

                    fLIFE_P_A = GLCONST_CHAR::nCONFRONT_LOSS_LP - (dwLevelA-dwLevelB)/2.0f
                        - (sOption.fHP_RATE-sOption.fTAR_HP_RATE);
                    LIMIT ( fLIFE_P_A, 0.0f, -20.0f );
                }
            }

            pPartyA->RECEIVELIVINGPOINT(this, (int)fLIFE_P_A);
            pPartyB->RECEIVELIVINGPOINT(this, (int)fLIFE_P_B);
        }
        /*
        else
        {
            if (pPartyA)
            {
                if (pNetMsg->emEND_A == EMCONFRONT_END_PWIN)
                    pPartyA->RECEIVELIVINGPOINT(0);
                else
                    pPartyA->RECEIVELIVINGPOINT(0);
            }
            if (pPartyB)
            {
                if (pNetMsg->emEND_B == EMCONFRONT_END_PWIN)
                    pPartyB->RECEIVELIVINGPOINT(0);
                else
                    pPartyB->RECEIVELIVINGPOINT(0);
            }
        }
        */
    }

    if ( pNetMsg->PartyID_A.isValidParty() )
    {
        GLPartyField* const pParty = GetParty(pNetMsg->PartyID_A);
        if (pParty)
            pParty->RESET_CONFRONT(this, pNetMsg->emEND_A);
    }

    if ( pNetMsg->PartyID_B.isValidParty() )
    {
        GLPartyField* const pParty = GetParty(pNetMsg->PartyID_B);
        if (pParty)
            pParty->RESET_CONFRONT(this, pNetMsg->emEND_B);
    }

    return TRUE;
}


BOOL GLGaeaServer::MsgChatPartyRecruitFB(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    GLMSG::NET_CHAT_PARTY_RECRUIT_FB_AF* pPacket = (GLMSG::NET_CHAT_PARTY_RECRUIT_FB_AF *) nmg;    
    GLChar* pChar = GetCharByDbNum(pPacket->m_ChaDbNum);

	//LOG
	if (m_dwAgentSlot != dwClientID)
    {
		DWORD dwChaNum = MAXDWORD;
		std::string strChaName;

		if (pChar)
		{
			dwChaNum = pChar->CharDbNum();
			strChaName = std::string(pChar->m_szName);

			sc::writeLogError(sc::string::format("HACKING! MsgChatPartyRecruitFB m_dwAgentSlot != dwClientID by Char(%1%, %2%)", dwChaNum, strChaName));
		}
		else
		{
			sc::writeLogError(std::string("HACKING! MsgChatPartyRecruitFB m_dwAgentSlot != dwClientID by non-existing Char"));
		}
        
        return FALSE;
    }

    if (pChar)
    {
        if (pPacket->Crc32 != sc::string::getCrc32(pPacket, sizeof(GLMSG::NET_CHAT_PARTY_RECRUIT_FB_AF)-sizeof(unsigned int)))
        {
            sc::writeLogError(
                sc::string::format(
                    "HACKING! MsgChatPartyRecruitFB CRC ChaDbNum %1%", pPacket->m_ChaDbNum));
            return FALSE;
        }
        else
        {
            pChar->MsgChatPartyRecruitFB(pPacket->emFB, pPacket->fPartyRecruitTime);
            return TRUE;
        }
    }
    else
    {
        return FALSE;
    }
}
