#include "pch.h"
#include "../../RanLogic/Msg/GLContrlClubMsg.h"

#include "../Club/GLClubAgent.h"
#include "../Database/DBAction/DbActionGameClub.h"
#include "./GLAgentServer.h"

#include "../../SigmaCore/DebugInclude.h"

BOOL GLAgentServer::MsgClubAllianceReq(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	if ( GLUseFeatures::GetInstance().IsUsingWorldBattle() )
	{
		sc::writeLogError( "The WorldBattle Server don't request to join alliance club." );

		return FALSE;
	}

    GLCharAG* pMyChar = GetChar(dwGaeaID);
    if (!pMyChar)
        return FALSE;

    GLMSG::NET_CLUB_ALLIANCE_REQ_CA* pNetMsg = (GLMSG::NET_CLUB_ALLIANCE_REQ_CA*) nmg;
	GLMSG::SNET_CLUB_ALLIANCE_REQ_FB NetMsgFB;

    GLCharAG* pTarChar = GetCharByDbNum(pNetMsg->m_TarCharDbNum);
    if (!pTarChar)
    {
        NetMsgFB.emFB = EMCLUB_ALLIANCE_REQ_FB_FAIL;
        SENDTOCLIENT(pMyChar->ClientSlot(), &NetMsgFB);
        return FALSE;
    }

	// 선도클럽존에서 동맹가입 안됨
	GLAGLandMan* pLandMan = GetLand(pMyChar->GetCurrentMap().getGaeaMapID());
    if (!pLandMan)
        return FALSE;

	if (pLandMan->IsClubBattleMap())
	{
		NetMsgFB.emFB = EMCLUB_ALLIANCE_REQ_FB_GUIDMAP;
		SENDTOCLIENT(pMyChar->ClientSlot(), &NetMsgFB);
		return FALSE;
	}

	if (!pMyChar->IsValidClub())
	{
		NetMsgFB.emFB = EMCLUB_ALLIANCE_REQ_FB_NOTMASTER;
		SENDTOCLIENT(pMyChar->ClientSlot(), &NetMsgFB);
		return FALSE;
	}

    // CTF 참여중이면 취소
    if (pMyChar->IsCtfPlayer())
    {
        NetMsgFB.emFB = EMCLUB_ALLIANCE_REQ_FB_FAIL;
        SENDTOCLIENT(pMyChar->ClientSlot(), &NetMsgFB);
        return FALSE;
    }

	std::tr1::shared_ptr<GLClubAgent> pMyClub = GetClub(pMyChar->GetClubDbNum());
	if (!pMyClub)
	{
		NetMsgFB.emFB = EMCLUB_ALLIANCE_REQ_FB_FAIL;
		SENDTOCLIENT(pMyChar->ClientSlot(), &NetMsgFB);
		return FALSE;
	}

    if (!pMyClub->HaveAllianceAuthority(pMyChar->ChaDbNum()))
    {
        NetMsgFB.emFB = EMCLUB_ALLIANCE_REQ_FB_NO_AUTHORITY;
        SENDTOCLIENT(pMyChar->ClientSlot(), &NetMsgFB);
        return FALSE;
    }

	/*
    if (!pMyClub->IsMaster(pMyChar->ChaDbNum()))
	{
		NetMsgFB.emFB = EMCLUB_ALLIANCE_REQ_FB_NOTMASTER;
		SENDTOCLIENT(pMyChar->ClientSlot(), &NetMsgFB);
		return FALSE;
	}
    */

	if (pMyClub->IsAlliance() && !pMyClub->IsChief())
	{
		NetMsgFB.emFB = EMCLUB_ALLIANCE_REQ_FB_NOTCHIEF;
		SENDTOCLIENT(pMyChar->ClientSlot(), &NetMsgFB);
		return FALSE;
	}

	// 클럽 배틀이 진행중일때 동맹을 맺을수 없다.
	if (pMyClub->GetAllBattleNum() > 0)
	{
		NetMsgFB.emFB = EMCLUB_ALLIANCE_REQ_FB_CLUBBATTLE;
		SENDTOCLIENT(pMyChar->ClientSlot(), &NetMsgFB);
	}

	// 최초 결성일때.
	if (!pMyClub->IsAlliance())
	{
		// 결성 제한 시간 검사
		if (pMyClub->IsAllianceDis())
		{
			NetMsgFB.emFB = EMCLUB_ALLIANCE_REQ_FB_DISTIME;
			NetMsgFB.tBlock = pMyClub->GetAllianceDisTime();
			SENDTOCLIENT(pMyChar->ClientSlot(), &NetMsgFB);
			return FALSE;
		}
	}

	if (pMyClub->IsRegDissolution())
	{
		NetMsgFB.emFB = EMCLUB_ALLIANCE_REQ_FB_DISSOLUTION;
		SENDTOCLIENT(pMyChar->ClientSlot(), &NetMsgFB);
		return FALSE;
	}

	// 가입 제한시간 (제명시간)
	if (pMyClub->IsAllianceSec())
	{
		NetMsgFB.emFB = EMCLUB_ALLIANCE_REQ_FB_SECTIME;
		NetMsgFB.tBlock = pMyClub->GetAllianceSec();
		SENDTOCLIENT(pMyChar->ClientSlot(), &NetMsgFB);
		return FALSE;
	}

	//	클럽 동맹 최대 숫자 검사.
	if (pMyClub->IsAlliance() && pMyClub->m_setAlliance.size() >= GLCONST_CHAR::dwCLUB_ALLIANCE_NUM)
	{
		NetMsgFB.emFB = EMCLUB_ALLIANCE_REQ_FB_MAX;
		SENDTOCLIENT(pMyChar->ClientSlot(), &NetMsgFB);
		return FALSE;
	}

	if (!pTarChar->IsValidClub())
	{
		NetMsgFB.emFB = EMCLUB_ALLIANCE_REQ_FB_TARNOTMASTER;
		SENDTOCLIENT(pMyChar->ClientSlot(), &NetMsgFB);
		return FALSE;
	}

    // 학원체크
	if (!GLCONST_CHAR::bCLUB_2OTHERSCHOOL && pTarChar->GetSchool() != pMyChar->GetSchool())
	{
		NetMsgFB.emFB = EMCLUB_ALLIANCE_REQ_FB_OTHERSCHOOL;
		SENDTOCLIENT(pMyChar->ClientSlot(), &NetMsgFB);
		return FALSE;
	}

	std::tr1::shared_ptr<GLClubAgent> pTarClub = GetClub(pTarChar->GetClubDbNum());
	if (!pTarClub)
	{
		NetMsgFB.emFB = EMCLUB_ALLIANCE_REQ_FB_TARNOTMASTER;
		SENDTOCLIENT(pMyChar->ClientSlot(), &NetMsgFB);
		return FALSE;
	}
	
	if (pTarClub->IsAllianceSec())
	{
		NetMsgFB.emFB = EMCLUB_ALLIANCE_REQ_FB_TOSECTIME;
		NetMsgFB.tBlock = pTarClub->GetAllianceSec();
		SENDTOCLIENT(pMyChar->ClientSlot(), &NetMsgFB);
		return FALSE;
	}

	if (!pTarClub->HaveAllianceAuthority(pTarChar->ChaDbNum()))
	{
		NetMsgFB.emFB = EMCLUB_ALLIANCE_REQ_FB_TARNOTMASTER;
		SENDTOCLIENT(pMyChar->ClientSlot(), &NetMsgFB);
		return FALSE;
	}

	if (pTarClub->IsAlliance())
	{
		NetMsgFB.emFB = EMCLUB_ALLIANCE_REQ_FB_ALREADY;
		SENDTOCLIENT(pMyChar->ClientSlot(), &NetMsgFB);
		return FALSE;
	}

	if (pMyClub == pTarClub)
	{
		NetMsgFB.emFB = EMCLUB_ALLIANCE_REQ_FB_SAMECLUB;
		SENDTOCLIENT(pMyChar->ClientSlot(), &NetMsgFB);
		return FALSE;
	}

	if (pTarClub->IsRegDissolution())
	{
		NetMsgFB.emFB = EMCLUB_ALLIANCE_REQ_FB_DISSOLUTION2;
		SENDTOCLIENT(pMyChar->ClientSlot(), &NetMsgFB);
		return FALSE;
	}

	// 클럽 배틀이 진행중일때 동맹을 맺을수 없다.
	if (pTarClub->GetAllBattleNum() > 0)
	{
		NetMsgFB.emFB = EMCLUB_ALLIANCE_REQ_FB_TARCLUBBATTLE;
		SENDTOCLIENT(pMyChar->ClientSlot(), &NetMsgFB);
	}

	DWORD dwCHIEFGuidID = GuidanceId(pMyClub->DbNum());
	DWORD dwINDIANGuidID = GuidanceId(pTarClub->DbNum());

	// 두 클럽이 모두 선도클럽이면 동맹 불가능
	if (dwCHIEFGuidID != UINT_MAX && dwINDIANGuidID != UINT_MAX)
	{
		NetMsgFB.emFB = EMCLUB_ALLIANCE_REQ_FB_BOTHGUID;
		SENDTOCLIENT(pMyChar->ClientSlot(), &NetMsgFB);
		return FALSE;
	}
	
	//	가입 요청한 클럽 id 저장.
	pMyClub->SetReqClubId(pTarClub->DbNum());

	// 동맹 요청
	GLMSG::NET_CLUB_ALLIANCE_REQ_ASK_AC NetMsgAsk;
	NetMsgAsk.m_ChiefCharID = pMyChar->ChaDbNum();
	NetMsgAsk.SetName(pMyClub->Name());
	SENDTOCLIENT(pTarChar->ClientSlot(), &NetMsgAsk);
	return TRUE;
}


BOOL GLAgentServer::MsgClubAllianceReqAns(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{    
    GLMSG::NET_CLUB_ALLIANCE_REQ_ANS_CA* pNetMsg = (GLMSG::NET_CLUB_ALLIANCE_REQ_ANS_CA*) nmg;
    GLMSG::SNET_CLUB_ALLIANCE_REQ_FB NetMsgFB;

    GLCharAG* pMyChar = GetChar(dwGaeaID);
    if (!pMyChar)
        return FALSE;

    if (!pMyChar->IsValidClub())
        return FALSE;

    std::tr1::shared_ptr<GLClubAgent> pClubIndian = GetClub(pMyChar->GetClubDbNum());
    if (!pClubIndian)
        return FALSE;

    if (!pClubIndian->HaveAllianceAuthority(pMyChar->ChaDbNum()))
        return FALSE;

    if (pClubIndian->IsAlliance())
        return FALSE;

    if (pClubIndian->GetAllBattleNum() > 0)
        return FALSE;

    GLCharAG* pReqChar = GetCharByDbNum(pNetMsg->m_ReqChaDbNum);
    if (!pReqChar)
        return FALSE;

    if (!pNetMsg->m_bOk)
    {
        NetMsgFB.emFB = EMCLUB_ALLIANCE_REQ_FB_REFUSE;
        SENDTOCLIENT(pReqChar->ClientSlot(), &NetMsgFB);
        return FALSE;
    }

    if (!pReqChar->IsValidClub())
        return FALSE;

    std::tr1::shared_ptr<GLClubAgent> pClubChief = GetClub(pReqChar->GetClubDbNum());
    if (!pClubChief)
        return FALSE;

	if (pClubIndian == pClubChief)
		return FALSE;

    if (!pClubChief->HaveAllianceAuthority(pReqChar->ChaDbNum()))
        return FALSE;

    if (pClubChief->IsAlliance() && !pClubChief->IsChief())
        return FALSE;

    if (pClubChief->GetAllBattleNum() > 0)
        return FALSE;

    // 요청한 클럽인지 검사
    if (pClubChief->GetReqClubId() != pClubIndian->DbNum())
        return FALSE;

    // 동맹 요청 결과
    NetMsgFB.emFB = EMCLUB_ALLIANCE_REQ_FB_OK;
    SENDTOCLIENT(pReqChar->ClientSlot(), &NetMsgFB);

    // 동맹 결성 및 동맹 클럽 추가
    pClubChief->SetAlliance(pClubChief->DbNum());
    pClubChief->AddAlliance(pClubChief->DbNum(),  pClubChief->Name(),  pClubChief->GetMasterName(),  pClubChief->GetMarkVersion());
    pClubChief->AddAlliance(pClubIndian->DbNum(), pClubIndian->Name(), pClubIndian->GetMasterName(), pClubIndian->GetMarkVersion());

    //	추가되는 클럽에 동맹 설정.
    pClubIndian->SetAlliance(pClubChief->DbNum());
    pClubIndian->SetAllianceBattleWin(pClubChief->GetAllianceBattleWin());
    pClubIndian->SetAllianceBattleLose(pClubChief->GetAllianceBattleLose());
    pClubIndian->SetAllianceBattleDraw(pClubChief->GetAllianceBattleDraw());

    // 추가되는 동맹에 동맹 마스터 추가.
    // 동맹 마스터만 리스트를 관리한다.
    //	pCLUB_INDIAN->AddAlliance ( pCLUB_CHIEF->m_dwID, pCLUB_CHIEF->m_szName );

    //	필드에 알림.
    GLMSG::SNET_CLUB_ALLIANCE_ADD_FLD NetMsgFld;
    NetMsgFld.dwChiefClubID = pClubChief->DbNum();
    NetMsgFld.dwIndianClubID = pClubIndian->DbNum();
    SENDTOALLCHANNEL(&NetMsgFld);

    // 클라이언트들에 알림
    GLMSG::NET_CLUB_ALLIANCE_ADD_AC NetMsgClt(
        pClubChief->GetMasterName(),
        pClubIndian->GetMasterName(),
        pClubChief->GetMarkVersion(),
        pClubIndian->GetMarkVersion());
    NetMsgClt.dwChiefClubID = pClubChief->DbNum();
    NetMsgClt.dwIndianClubID = pClubIndian->DbNum();

    NetMsgClt.dwAllianceBattleWin = pClubChief->GetAllianceBattleWin();
    NetMsgClt.dwAllianceBattleLose = pClubChief->GetAllianceBattleLose();
    NetMsgClt.dwAllianceBattleDraw = pClubChief->GetAllianceBattleDraw();

    NetMsgClt.SetChiefClubName(pClubChief->Name());
    NetMsgClt.SetIndianClubName(pClubIndian->Name());

    //	신규로 가입한 클럽에 대한 정보를 보내준다.
    CLUB_ALLIANCE_ITER pos = pClubChief->m_setAlliance.begin();
    CLUB_ALLIANCE_ITER end = pClubChief->m_setAlliance.end();
    for ( ; pos!=end; ++pos)
    {
        const GLCLUBALLIANCE& sALLIANCE = pos->second;

        //	기존클럽에 대한 정보를 신규클럽에 전송한다.
        NetMsgClt.dwChiefClubID = pClubChief->DbNum();
        NetMsgClt.dwIndianClubID = sALLIANCE.m_dwID;
        NetMsgClt.SetChiefClubName(pClubChief->Name());
        NetMsgClt.SetChiefMasterName(pClubChief->GetMasterName());
        NetMsgClt.SetIndianClubName(sALLIANCE.m_szName);
        NetMsgClt.SetIndianMasterName(sALLIANCE.GetClubMasterName());
        SENDTOCLUBCLIENT(pClubIndian->DbNum(), &NetMsgClt);

        //	신규 클럽에 대한 정보를 전송한다.
        if (sALLIANCE.m_dwID == pClubIndian->m_DbNum)
            continue;

        NetMsgClt.dwChiefClubID = pClubChief->m_DbNum;
        NetMsgClt.dwIndianClubID = pClubIndian->m_DbNum;
        NetMsgClt.SetChiefClubName(pClubChief->Name());
        NetMsgClt.SetChiefMasterName(pClubChief->GetMasterName());
        NetMsgClt.SetIndianClubName(pClubIndian->Name());
        NetMsgClt.SetIndianMasterName(pClubIndian->GetMasterName());
        SENDTOCLUBCLIENT(sALLIANCE.m_dwID, &NetMsgClt);
    }

    //	db에 저장.
    AddGameAdoJob(
        db::DbActionPtr(
            new db::ClubAllianceSet(pClubChief->DbNum(), pClubIndian->DbNum())));
    return TRUE;
}