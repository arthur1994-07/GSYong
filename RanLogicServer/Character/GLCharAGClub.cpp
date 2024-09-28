#include "../pch.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/Util/DateTime.h"

#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/DxTools/DebugSet.h"

#include "../../RanLogic/Item/GLITEMLMT.h"
#include "../../RanLogic/GLUseFeatures.h"

#include "../../MfcExLib/RanFilter.h"

#include "../Club/GLClubManAgent.h"
#include "../Club/GLClubAgent.h"
#include "../AgentServer/GLAgentServer.h"
#include "../Util/GLItemLimit.h"
#include "../ClubDeathMatch/GLClubDeathMatch.h"
#include "../Guidance/GLGuidance.h"
#include "../Database/DBAction/DbActionGameClub.h"
#include "./GLCharAG.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------
/*
HRESULT GLCharAG::MsgClubMemberReqAgt (NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_CLUB_MEMBER_REQ_2AGT *pNetMsg = (GLMSG::SNET_CLUB_MEMBER_REQ_2AGT *) nmg;

	GLMSG::SNET_CLUB_MEMBER_REQ_FB NetMsgFB;
	
	GLCharAG* pTarChar = m_pServer->GetChar(pNetMsg->dwReqGID);
	if (!pTarChar)
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_FAIL;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

	NetMsgFB.SetTarChaName(pTarChar->m_szName);

	// Ż���� �����ð� ����.
	if ( pTarChar->m_tSECEDE!=0 )
	{
		CTime cSECEDE(pTarChar->m_tSECEDE);
		CTimeSpan sSpan(GLCONST_CHAR::dwCLUB_JOINBLOCK_DAY,0,10,0);

		cSECEDE += sSpan;

		CTime cCurTime = CTime::GetCurrentTime();
		if ( cCurTime < cSECEDE )
		{
			NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_SECEDED;
			m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
			return S_FALSE;
		}
	}
	
	std::tr1::shared_ptr<GLClubAgent> pClub = m_pServer->GetClub(m_ClubDbNum);
	if ( !pClub )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_FAIL;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

	if ( pClub->GetAllBattleNum() > 0 )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_CLUBBATTLE;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

	if ( !pClub->HaveMemberJoinAuthority(m_CharDbNum) )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_FAIL;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

	bool bFOUND = pClub->IsMember ( pTarChar->m_CharDbNum );
	if ( bFOUND )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_ALREADY;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

	const GLCLUBRANK &sRANK_DATA = GLogicData::GetInstance().GetClubRankData(pClub->m_dwRank);
	if ( pClub->GetMemberNum() >= sRANK_DATA.m_dwMember )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_MAXNUM;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}


	// Ŭ�� ���� �α� ���.    
	LogCharAction
	(
		pClub->MasterCharDbNum(),		//	�����.
		gamelog::ACT_CLUB_JOIN_MEMBER,	//	����.
		gamelog::ID_CHAR, pTarChar->m_CharDbNum,	//	����.
		0,							//	exp
		0,							//	bright
		0,							//	life
		0							//	money
	);

	// Ŭ���� ���Խ�Ŵ.
	//
	pClub->AddMember(pTarChar->m_CharDbNum, pTarChar->m_szName, 0, pTarChar->GetChaLevel());
	pTarChar->m_ClubDbNum = pClub->m_DbNum;

	// DB�� Ŭ�� ��� �߰�.
	AddGameDbJob(
        db::DbActionPtr(
            new db::ClubMemberAdd(pClub->m_DbNum,pTarChar->m_CharDbNum)));	

    // Club log/%1%���� %2%���� Ŭ���� ���Խ��׽��ϴ�.
    std::string ClubLog(
        sc::string::format(
            ID2GAMEINTEXT("CLUB_LOG_2"),
            m_szName,
            pTarChar->GetChaName()));
    SENDCLUBLOG(club::LOG_MEMBER, ClubLog);

	// ���� ���� FB.
	NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_OK;
	m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );

	// Field�� ���� ����.
	GLMSG::SNET_CLUB_MEMBER_ADD_2FLD NetMsgAdd(
        pClub->m_DbNum,
        pTarChar->m_CharDbNum,
        pTarChar->GetChaLevel(),
        pTarChar->m_szName);
	m_pServer->SENDTOALLCHANNEL(&NetMsgAdd);

	return S_OK;
}
*/
HRESULT GLCharAG::MsgClubMemberDel(NET_MSG_GENERIC* nmg)
{
	GLMSG::NET_CLUB_MEMBER_DEL_CA* pPacket = (GLMSG::NET_CLUB_MEMBER_DEL_CA*) nmg;
	DWORD DelChaDbNum = pPacket->m_ChaDbNum;
    GLMSG::NET_CLUB_MEMBER_DEL_AC NetMsgFB;
	if (m_ClubDbNum == CLUB_NULL)
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_DEL_FB_FAIL;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}

    std::tr1::shared_ptr<GLClubAgent> spClub = m_pServer->GetClub(m_ClubDbNum);
	if (!spClub)
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_DEL_FB_FAIL;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}

	if (spClub->MasterCharDbNum() == DelChaDbNum)
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_DEL_FB_FAIL;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}

	if (spClub->IsSubMaster(DelChaDbNum) && !spClub->IsMaster(m_CharDbNum))
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_DEL_FB_FAIL;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}

	if (!spClub->HaveMemberKickAuthority(m_CharDbNum))
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_DEL_FB_FAIL;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}
	
	if (!spClub->IsMember(DelChaDbNum))
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_DEL_FB_FAIL;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}

    const GLCLUBMEMBER* pDelMember = spClub->GetMember(DelChaDbNum);
    if (!pDelMember)
        return S_FALSE;

    const GLCLUBMEMBER* pMyMember = spClub->GetMember(m_CharDbNum);
    if (!pMyMember)
        return S_FALSE;

    
    if (m_CharDbNum != spClub->MasterCharDbNum() && pMyMember->GetGrade() < pDelMember->GetGrade())
    {
        NetMsgFB.emFB = EMCLUB_MEMBER_DEL_FB_GRADE;
        m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
        return S_FALSE;
    }

	const bool _bResult(spClub->DelMember(DelChaDbNum, m_CharDbNum, std::string(m_szName)));
	if ( !_bResult )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_DEL_FB_FAIL;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}

	// ��� ���ܻ�Ȳ�� �Ѱ����� �߹� ���� �˸�;
	NetMsgFB.emFB = EMCLUB_MEMBER_DEL_FB_OK;
	m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);

	return S_OK;
}

HRESULT GLCharAG::MsgClubMemberSecede(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_CLUB_MEMBER_SECEDE* pPacket = (GLMSG::SNET_CLUB_MEMBER_SECEDE*) nmg;
	GLMSG::SNET_CLUB_MEMBER_SECEDE_FB NetMsgFB;

	if (m_ClubDbNum == CLUB_NULL)
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_SECEDE_FB_FAIL;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}

	std::tr1::shared_ptr<GLClubAgent> pClub = m_pServer->GetClub(m_ClubDbNum);
	if (!pClub)
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_SECEDE_FB_FAIL;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}

	if (pClub->MasterCharDbNum() == m_CharDbNum)
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_SECEDE_FB_FAIL;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}

	bool bFOUND = pClub->IsMember(m_CharDbNum);
	if (!bFOUND)
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_SECEDE_FB_FAIL;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}

	const bool _bResult(pClub->DelMember(m_CharDbNum, m_CharDbNum, std::string(m_szName)));
    if ( !_bResult )
    {
		NetMsgFB.emFB = EMCLUB_MEMBER_SECEDE_FB_FAIL;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
    }

	// ��� ���ܻ�Ȳ�� �Ѱ����� ���� ���� �˸�;
	NetMsgFB.emFB = EMCLUB_MEMBER_SECEDE_FB_OK;
	m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);

	return S_OK;
}

HRESULT GLCharAG::MsgClubAuthorityReq (NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_CLUB_AUTHORITY_REQ *pNetMsg = (GLMSG::SNET_CLUB_AUTHORITY_REQ *) nmg;
	GLMSG::SNET_CLUB_AUTHORITY_REQ_FB NetMsgFB;
	NetMsgFB.dwCharID = pNetMsg->dwMember;

	if ( m_pServer->IsClubMasterMandate() )
	{
		if ( m_ClubDbNum==CLUB_NULL )
		{
			NetMsgFB.emFB = EMCLUB_AUTHORITY_REQ_FB_FAIL;
			m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
			return S_FALSE;
		}

		std::tr1::shared_ptr<GLClubAgent> pCLUB = m_pServer->GetClub(m_ClubDbNum);
		if ( !pCLUB )
		{
			NetMsgFB.emFB = EMCLUB_AUTHORITY_REQ_FB_FAIL;
			m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
			return S_FALSE;
		}

		if ( pCLUB->MasterCharDbNum()==pNetMsg->dwMember )
		{
			NetMsgFB.emFB = EMCLUB_AUTHORITY_REQ_FB_FAIL;
			m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
			return S_FALSE;
		}

		if ( !pCLUB->IsMaster(m_CharDbNum) )
		{
			NetMsgFB.emFB = EMCLUB_AUTHORITY_REQ_FB_NOMASTER;
			m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
			return S_FALSE;
		}

		bool bFOUND = pCLUB->IsMember ( pNetMsg->dwMember );
		if ( !bFOUND )
		{
			NetMsgFB.emFB = EMCLUB_AUTHORITY_REQ_FB_FAIL;
			m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
			return S_FALSE;
		}

		//	������ ����
		GLCharAG* pChar = m_pServer->GetCharByDbNum ( pNetMsg->dwMember );
		if ( !pChar )
		{
			NetMsgFB.emFB = EMCLUB_AUTHORITY_REQ_FB_NOONLINE;
			m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
			return S_FALSE;
		}

		//	���� ����
		if ( pCLUB->IsAlliance() )
		{
			NetMsgFB.emFB = EMCLUB_AUTHORITY_REQ_FB_ALLIANCE;
			m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
			return S_FALSE;
		}

		//	Ŭ����Ʋ ����
		if ( pCLUB->GetAllBattleNum() > 0 )
		{
			NetMsgFB.emFB = EMCLUB_AUTHORITY_REQ_FB_CLUBBATTLE;
			m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
			return S_FALSE;
		}

		//	��� ����
		if ( m_sCONFTING.emTYPE != EMCONFT_NONE || pChar->m_sCONFTING.emTYPE != EMCONFT_NONE )
		{
			NetMsgFB.emFB = EMCLUB_AUTHORITY_REQ_FB_CONFING;
			m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
			return S_FALSE;
		}

		//	�ð� ����
		if ( pCLUB->IsAuthority() )
		{
			NetMsgFB.emFB = EMCLUB_AUTHORITY_REQ_FB_TIME;
			NetMsgFB.tAuthority = pCLUB->m_tAuthority;
			m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
			return S_FALSE;
		}

		//	������ ����    
		if ( m_pServer->GuidanceIsBattle() )
		{
			NetMsgFB.emFB = EMCLUB_AUTHORITY_REQ_FB_GUIDNBATTLE;
			m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
			return S_FALSE;
		}

		//	������ ����
		if ( m_pServer->ClubDeathMatchIsBattle() )
		{
			NetMsgFB.emFB = EMCLUB_AUTHORITY_REQ_FB_CDMBATTLE;
			m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
			return S_FALSE;
		}

		pCLUB->m_dwReqClubID = pCLUB->m_DbNum;

		// ���� ���� ��û.
		GLMSG::SNET_CLUB_AUTHORITY_REQ_ASK NetMsgAsk;
		m_pServer->SENDTOCLIENT ( pChar->m_ClientSlot, &NetMsgAsk );		

		return S_OK;
	}
	else
	{
		// Ŭ�������� ���� ON/OFF
		NetMsgFB.emFB = EMCLUB_AUTHORITY_REQ_FB_OFF;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}
}

HRESULT GLCharAG::MsgClubAuthorityReqAns(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_CLUB_AUTHORITY_REQ_ANS* pPacket = (GLMSG::SNET_CLUB_AUTHORITY_REQ_ANS*) nmg;
	GLMSG::SNET_CLUB_AUTHORITY_REQ_FB NetMsgFB;
	NetMsgFB.dwCharID = m_CharDbNum;

	if (m_ClubDbNum == CLUB_NULL)
        return S_FALSE;
	
	std::tr1::shared_ptr<GLClubAgent> spClub = m_pServer->GetClub(m_ClubDbNum);
	if (!spClub)
        return S_FALSE;
	if (!spClub->IsMember(m_CharDbNum))
        return S_FALSE;
	if (m_ClubDbNum != spClub->m_dwReqClubID)
        return S_FALSE;
	if (spClub->IsMaster(m_CharDbNum))
        return S_FALSE;
	
	GLCharAG* pChar = m_pServer->GetCharByDbNum(spClub->MasterCharDbNum());
	if (!pChar)
	{
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgClubAuthorityReqAns", m_CharDbNum ) );
        return S_FALSE; // �����Ͱ� ����
	}

	if (!pPacket->bOK)
	{
		NetMsgFB.emFB = EMCLUB_AUTHORITY_REQ_FB_REFUSE;
		m_pServer->SENDTOCLIENT(pChar->m_ClientSlot, &NetMsgFB);
		return S_OK;
	}

	//	���� ����
	if (spClub->IsAlliance())
        return S_FALSE;
	
	//	Ŭ����Ʋ ����
	if (spClub->GetAllBattleNum() > 0)
        return S_FALSE;

    //	��� ����
	if (m_sCONFTING.emTYPE != EMCONFT_NONE || pChar->m_sCONFTING.emTYPE != EMCONFT_NONE)
        return S_FALSE;

	//	�ð� ����
	if (spClub->IsAuthority())
        return S_FALSE;

	//	������ ����
	if (m_pServer->GuidanceIsBattle())
        return S_FALSE;

	if (m_pServer->ClubDeathMatchIsBattle())
        return S_FALSE;

	// Ŭ�� ������ ���� 
	if (!spClub->SetAuthorityMaster(m_CharDbNum))
        return S_FALSE;

	// Ŭ�������� ���� �α� ���    
	// ��� �� �α� ����
	LogCharAction
	(
		pChar->ChaDbNum(),		//	�����.
		gamelog::ACT_CLUB_AUTHORITY,	//	����.
		gamelog::ID_CHAR, m_CharDbNum,	//	����.
		0,							//	exp
		0,							//	bright
		0,							//	life
		0,							//	money
		( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap()
	);

	// DB���� �����ͺ���
	AddGameDbJob(
        db::DbActionPtr(
            new db::ClubAuthoritySet(spClub->m_DbNum, m_CharDbNum)));	

	//	���� ���� �ð� ����
	spClub->SetAuthority();

    AddGameDbJob(
        db::DbActionPtr(
            new db::ClubAuthorityTimeSet(spClub->m_DbNum, spClub->m_tAuthority)));

	// ���� Ŭ������ ����
	NetMsgFB.emFB = EMCLUB_AUTHORITY_REQ_FB_OK;
	m_pServer->SENDTOCLIENT(pChar->m_ClientSlot, &NetMsgFB);

	// �ʵ弭���� ���� ����
	GLMSG::NET_CLUB_MASTER_CHANGE_AF MsgAF(m_ClubDbNum, m_CharDbNum, m_wLevel);
	m_pServer->SENDTOALLCHANNEL(&MsgAF);

	// �ڱ� Ŭ�������� �˸�
    GLMSG::NET_CLUB_MASTER_CHNAGE_AC MsgAC(m_CharDbNum, m_wLevel, m_szName);
	m_pServer->SENDTOCLUBCLIENT(spClub->DbNum(), &MsgAC);
	return S_OK;
}

HRESULT GLCharAG::MsgClubMarkInfo (NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_CLUB_MARK_INFO *pNetMsg = (GLMSG::SNET_CLUB_MARK_INFO *) nmg;
	GLMSG::SNET_CLUB_MARK_INFO_FB NetMsgFB;
	
	std::tr1::shared_ptr<GLClubAgent> pCLUB = m_pServer->GetClub ( pNetMsg->dwClubID );
	if ( !pCLUB )
	{
		NetMsgFB.emFB = EMCLUB_MARK_INFO_FB_FAIL;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

	NetMsgFB.dwClubID = pNetMsg->dwClubID;
	NetMsgFB.emFB = EMCLUB_MARK_INFO_FB_OK;
	NetMsgFB.dwMarkVER = pCLUB->m_dwMarkVER;
	memcpy ( NetMsgFB.aryMark, pCLUB->m_aryMark, sizeof(DWORD)*EMCLUB_MARK_SX*EMCLUB_MARK_SY );
	m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );

	return S_OK;
}

HRESULT GLCharAG::MsgClubMarkChange(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_CLUB_MARK_CHANGE* pNetMsg = (GLMSG::SNET_CLUB_MARK_CHANGE*) nmg;
	GLMSG::SNET_CLUB_MARK_CHANGE_FB NetMsgFB;

	if (m_ClubDbNum == CLUB_NULL)
	{
		NetMsgFB.emFB = EMCLUB_MARK_CHANGE_FB_FAIL;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}
	
	std::tr1::shared_ptr<GLClubAgent> pCLUB = m_pServer->GetClub(m_ClubDbNum);
	if (!pCLUB)
	{
		NetMsgFB.emFB = EMCLUB_MARK_CHANGE_FB_FAIL;
		m_pServer->SENDTOCLIENT( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

    DWORD CurTime = ::GetTickCount();
    DWORD ChangeClubMarkTime = pCLUB->GetChangeClubMarkTime();

    if ( CurTime - ChangeClubMarkTime < (GLCONST_CHAR::dwCLUB_CHANGE_CLUB_MARK_LIMIT_TIME*1000) )
    {
        NetMsgFB.emFB = EMCLUB_CHANGE_MARK_LIMIT_TIME;
        m_pServer->SENDTOCLIENT( m_ClientSlot, &NetMsgFB );

        return S_FALSE;
    }

	if (!pCLUB->HaveMarkChangeAuthority(m_CharDbNum))
	{
		NetMsgFB.emFB = EMCLUB_MARK_CHANGE_FB_FAIL;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}

	// �̹��� ���� ����.
	++pCLUB->m_dwMarkVER;
	memcpy(pCLUB->m_aryMark, pNetMsg->aryMark, sizeof(DWORD)*EMCLUB_MARK_SX*EMCLUB_MARK_SY);

	// DB�� ����.	
    AddGameDbJob(
        db::DbActionPtr(
            new db::ClubMarkImageWrite(
                pCLUB->m_DbNum,
                pCLUB->m_dwMarkVER,
                (BYTE*) pCLUB->m_aryMark,
                sizeof(DWORD)*EMCLUB_MARK_SX*EMCLUB_MARK_SY)));

    pCLUB->SetChangeClubMarkTime( CurTime );

	// �ʵ� ������ �˸�?
	GLMSG::SNET_CLUB_MARK_CHANGE_2FLD NetMsgFld;
	NetMsgFld.dwClub = m_ClubDbNum;
	NetMsgFld.dwMarkVER = pCLUB->m_dwMarkVER;
	memcpy ( NetMsgFld.aryMark, pCLUB->m_aryMark, sizeof(DWORD)*EMCLUB_MARK_SX*EMCLUB_MARK_SY );
	m_pServer->SENDTOALLCHANNEL( &NetMsgFld );

	// Ŭ���̾�Ʈ�鿡�� �˸�.
	GLMSG::SNET_CLUB_MARK_CHANGE_2CLT NetMsgClt;
	NetMsgClt.dwMarkVER = pCLUB->m_dwMarkVER;
	memcpy ( NetMsgClt.aryMark, pCLUB->m_aryMark, sizeof(DWORD)*EMCLUB_MARK_SX*EMCLUB_MARK_SY );

	GLCharAG* pMASTER = m_pServer->GetCharByDbNum(pCLUB->MasterCharDbNum());
	if ( pMASTER )
	{
		m_pServer->SENDTOCLIENT( pMASTER->m_ClientSlot, &NetMsgClt );
	}

    m_pServer->SENDTOCLUBCLIENT(pCLUB->DbNum(), &NetMsgClt);	

	return S_OK;
}

HRESULT GLCharAG::MsgClubRank2Agt (NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_CLUB_RANK_FB NetMsgFB;

	if ( m_ClubDbNum==CLUB_NULL )
	{
		NetMsgFB.emFB = EMCLUB_RANK_FB_FAIL;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

	std::tr1::shared_ptr<GLClubAgent> pCLUB = m_pServer->GetClub(m_ClubDbNum);
	if ( !pCLUB )
	{
		NetMsgFB.emFB = EMCLUB_RANK_FB_FAIL;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}
/*
	if ( pCLUB->MasterCharDbNum() != m_CharDbNum )
	{
		NetMsgFB.emFB = EMCLUB_RANK_FB_FAIL;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}
*/
    if (!pCLUB->HaveRankUpAuthority(m_CharDbNum))
    {
        NetMsgFB.emFB = EMCLUB_RANK_FB_FAIL;
        m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
        return S_FALSE;
    }

	if (pCLUB->m_dwRank >= (GLCONST_CHAR::dwMAX_CLUBRANK-1))
	{
		NetMsgFB.emFB = EMCLUB_RANK_FB_MAX;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}

	++pCLUB->m_dwRank;

	// db�� ����.
    AddGameAdoJob(
        db::DbActionPtr(
            new db::ClubRankSet(pCLUB->m_DbNum, pCLUB->m_dwRank)));	

	// �ʵ� ������ �˸�.
	GLMSG::NET_CLUB_RANK_CHANGE_AF NetMsgFld(pCLUB->m_DbNum, pCLUB->m_dwRank, m_CharDbNum);
	m_pServer->SENDTOALLCHANNEL(&NetMsgFld);

	// ó�� ��� �˸�.
	NetMsgFB.emFB = EMCLUB_RANK_FB_OK;
	m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);

    // Ŭ���α� ����    
    std::string ClubLog(
        sc::string::format(ID2GAMEINTEXT("CLUB_LOG_8"), m_szName));
    SENDCLUBLOG(club::LOG_CLUB_RANK_UP, ClubLog);

	return S_OK;
}

HRESULT GLCharAG::MsgClubGuidCommission (NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_CLUB_GUID_COMMISSION *pNetMsg = (GLMSG::SNET_CLUB_GUID_COMMISSION *) nmg;
	GLMSG::SNET_CLUB_GUID_COMMISSION_FB NetMsgFB;

	if ( m_ClubDbNum == CLUB_NULL )
	{
		NetMsgFB.emFB = EMGUIDCOMMISSION_FB_NOTCLUB;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

	std::tr1::shared_ptr<GLClubAgent> pCLUB = m_pServer->GetClub(m_ClubDbNum);
	if ( !pCLUB )
	{
		NetMsgFB.emFB = EMGUIDCOMMISSION_FB_NOTCLUB;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

	if ( pCLUB->MasterCharDbNum()!=m_CharDbNum )
	{
		NetMsgFB.emFB = EMGUIDCOMMISSION_FB_NOTCLUB;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}
    
	GLGuidance *pGuid = m_pServer->GuidanceFindByClubID ( m_ClubDbNum );
	if ( !pGuid )
	{
		NetMsgFB.emFB = EMGUIDCOMMISSION_FB_NOTCLUB;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

	if ( pGuid->m_bBattle )
	{
		NetMsgFB.emFB = EMGUIDCOMMISSION_FB_BATTLE;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

	if ( pNetMsg->fCommission>GLCONST_CHAR::fMAX_COMMISSION )
	{
		NetMsgFB.emFB = EMGUIDCOMMISSION_FB_RANGE;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

	if (pNetMsg->fCommission < 0.0f)
	{
		NetMsgFB.emFB = EMGUIDCOMMISSION_FB_RANGE;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

	if (pGuid->IsNewCommission())
	{
		NetMsgFB.emFB = EMGUIDCOMMISSION_FB_ALREADY;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;	
	}

	pGuid->m_NewCommission.SetData(m_CharDbNum, true, 0.0f, pNetMsg->fCommission);

	// ������ ���� ���� �˸�.
	NetMsgFB.emFB = EMGUIDCOMMISSION_FB_OK;
	NetMsgFB.fCommission = pNetMsg->fCommission;
	m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );

	// Ŭ���̾�Ʈ�鿡�� �˸�.
	GLMSG::SNET_CLUB_GUID_COMMISSION_RESERVE_BRD NetMsgBrd;
	NetMsgBrd.fCommission = pNetMsg->fCommission;

	std::vector<DWORD>::size_type nSize = pGuid->m_vecMaps.size();
	for ( std::vector<DWORD>::size_type i=0; i<nSize; ++i )
	{
		SNATIVEID nidMAP = (pGuid->m_vecMaps[i]);
		GLAGLandMan* pLand = m_pServer->GetLand ( nidMAP );
		if (pLand)
		    pLand->SENDTOALLCLIENT(&NetMsgBrd);
	}

    // Club log    
    std::string LogMsg(
        sc::string::format(
            ID2GAMEINTEXT("CLUB_LOG_GUIDANCE2"),
            m_szName,
            static_cast<int> (pNetMsg->fCommission)));
    SENDCLUBLOG(club::LOG_GUIDANCE, LogMsg);

	return S_OK;
}

//! ���Ϳ��� Ŭ�� �����û ó��
HRESULT GLCharAG::MsgClubAllianceDelReq (NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_CLUB_ALLIANCE_DEL_REQ *pNetMsg = (GLMSG::SNET_CLUB_ALLIANCE_DEL_REQ *) nmg;
	GLMSG::SNET_CLUB_ALLIANCE_DEL_FB NetMsgFB;

	GLCharAG* pChar = m_pServer->GetCharByDbNum( m_CharDbNum );
	if (!pChar)
        return S_FALSE;

	// ����Ŭ�������� ����Ż�� �ȵ�
	GLAGLandMan* pLandMan(m_pServer->GetLand(pChar->GetCurrentMap().getGaeaMapID()));
	if (!pLandMan || pLandMan->IsClubBattleMap())
	{
		NetMsgFB.emFB = EMCLUB_ALLIANCE_DEL_FB_GUIDMAP;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}

	if (m_ClubDbNum == CLUB_NULL)
	{
		NetMsgFB.emFB = EMCLUB_ALLIANCE_DEL_FB_NOTMASTER;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}

	std::tr1::shared_ptr<GLClubAgent> pCLUB_ALLIANCE_MEMBER = m_pServer->GetClub(m_ClubDbNum);
	if (!pCLUB_ALLIANCE_MEMBER)
	{
		NetMsgFB.emFB = EMCLUB_ALLIANCE_DEL_FB_NOTMASTER;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}

	//if (!pCLUB_CHIEF->IsMaster(m_CharDbNum))
	//{
	//	NetMsgFB.emFB = EMCLUB_ALLIANCE_DEL_FB_NOTMASTER;
	//	m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
	//	return S_FALSE;
	//}
    if (!pCLUB_ALLIANCE_MEMBER->HaveAllianceDisAuthority(m_CharDbNum))
    {
        NetMsgFB.emFB = EMCLUB_ALLIANCE_DEL_FB_NO_AUTH;
        m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
        return S_FALSE;
    }

	if (!pCLUB_ALLIANCE_MEMBER->IsAlliance())
	{
		NetMsgFB.emFB = EMCLUB_ALLIANCE_DEL_FB_NOTCHIEF;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}

	// Ŭ�� ���Ϳ��� Ż���� Ŭ���� ID Get
	DWORD dwDelClubID = pNetMsg->dwDelClubID;

	// Ŭ�� �������� �ƴ϶��
	std::tr1::shared_ptr<GLClubAgent> pCLUB_CHIEF = m_pServer->GetClub(m_ClubDbNum);
	bool bChief = true;
	if (!pCLUB_ALLIANCE_MEMBER->IsChief())
	{
		// Ŭ�� �������� �ƴϴ�
		bChief = false;

		// �����θ� Ż���� Ŭ������ ����
		dwDelClubID = m_ClubDbNum;

		// Ŭ�� ������ Get
		DWORD clubID = pCLUB_ALLIANCE_MEMBER->GetChiefID();
		pCLUB_CHIEF = m_pServer->GetClub(clubID);
	}

	if( !pCLUB_CHIEF )
	{
		NetMsgFB.emFB = EMCLUB_ALLIANCE_DEL_FB_FAIL;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}

	if (dwDelClubID == pCLUB_CHIEF->m_DbNum)
	{
		NetMsgFB.emFB = EMCLUB_ALLIANCE_DEL_FB_NO_MYCLUB;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}
	
	std::tr1::shared_ptr<GLClubAgent> pCLUB_INDIAN = m_pServer->GetClub(dwDelClubID);
	if (!pCLUB_INDIAN)
	{
		NetMsgFB.emFB = EMCLUB_ALLIANCE_DEL_FB_FAIL;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}


	if (pCLUB_CHIEF->GetAllBattleNum() > 0)
	{
		NetMsgFB.emFB = EMCLUB_ALLIANCE_DEL_FB_BATTLE;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}

	//	���Ϳ��� ����.
    if (!pCLUB_CHIEF->DelAlliance(dwDelClubID))
    {
        NetMsgFB.emFB = EMCLUB_ALLIANCE_DEL_FB_FAIL;
        m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
        return S_FALSE;
    }
	
	//	����� ���ѽð� ����.
	pCLUB_CHIEF->SetAllianceSec();
	pCLUB_INDIAN->SetAllianceSec();
	
	AddGameDbJob(
        db::DbActionPtr(
            new db::ClubAllianceTimeSet(pCLUB_CHIEF->m_DbNum,pCLUB_CHIEF->m_tAllianceSec)));	

	AddGameDbJob(
        db::DbActionPtr(
            new db::ClubAllianceTimeSet(pCLUB_INDIAN->m_DbNum,pCLUB_INDIAN->m_tAllianceSec)));	
    
	GLMSG::SNET_CLUB_ALLIANCE_DEL_CLT NetMsgClt;
	NetMsgClt.dwDelClubID = pCLUB_INDIAN->m_DbNum;
	StringCchCopy(NetMsgClt.szDelClub, CHAR_SZNAME, pCLUB_INDIAN->Name());

	//	�����Ǵ� Ŭ���� ����.
	m_pServer->SENDTOCLUBCLIENT(pCLUB_INDIAN->m_DbNum, &NetMsgClt);

	//	�ش� Ŭ�� �ʱ�ȭ.
	pCLUB_INDIAN->ClearAlliance();

	CLUB_ALLIANCE_ITER pos = pCLUB_CHIEF->m_setAlliance.begin();
	CLUB_ALLIANCE_ITER end = pCLUB_CHIEF->m_setAlliance.end();
	for ( ; pos!=end; ++pos )
	{
		const GLCLUBALLIANCE &sALLIANCE = pos->second;

		//	��Ÿ ���� Ŭ���� ����.
		m_pServer->SENDTOCLUBCLIENT ( sALLIANCE.m_dwID, &NetMsgClt );
	}

	//	���� Ŭ�� ���� �ʵ忡 �˸�.
	GLMSG::SNET_CLUB_ALLIANCE_DEL_FLD NetMsgFld;
	NetMsgFld.dwAlliance = pCLUB_CHIEF->m_dwAlliance;
	NetMsgFld.dwDelClubID = dwDelClubID;
	m_pServer->SENDTOALLCHANNEL ( &NetMsgFld );

	//	���� ���� ��� �˸�.
	NetMsgFB.emFB = bChief ? EMCLUB_ALLIANCE_DEL_FB_OK : EMCLUB_ALLIANCE_DEL_FB_NOTCHIEF_OK;
	m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
	
	//	db�� ����.
	AddGameDbJob(
        db::DbActionPtr(
            new db::ClubAllianceDelete(pCLUB_CHIEF->m_DbNum, pCLUB_INDIAN->m_DbNum)));

	// ���� ���Ϳ� Ŭ������ ������
	if ( pCLUB_CHIEF->m_setAlliance.size() < 2 )
	{
		//	���� ��ü
		GLMSG::SNET_CLUB_ALLIANCE_DIS_CLT NetMsgClt;
		NetMsgClt.SetName(pCLUB_CHIEF->Name());

		//	��ü�� ���ѽð� ����.
		pCLUB_CHIEF->SetAllianceDis();

		AddGameDbJob(
            db::DbActionPtr(
                new db::ClubAllianceDisolveTimeSet(pCLUB_CHIEF->m_DbNum,pCLUB_CHIEF->m_tAllianceDis)));		

		//	Ŭ������ ���� ���� ����.
		pCLUB_CHIEF->ClearAlliance();

		AddGameDbJob(
            db::DbActionPtr(
                new db::AllianceBattleReSet(pCLUB_CHIEF->m_DbNum)));		

		// Ŭ���̾�Ʈ�� ����.
		m_pServer->SENDTOCLUBCLIENT ( pCLUB_CHIEF->m_DbNum, &NetMsgClt );

		//	���� Ŭ�� ���� �ʵ忡 �˸�.
		GLMSG::SNET_CLUB_ALLIANCE_DIS_FLD NetMsgDisFld;
		NetMsgDisFld.dwChiefClubID = pCLUB_CHIEF->m_DbNum;
		m_pServer->SENDTOALLCHANNEL ( &NetMsgDisFld );
	}

	return S_OK;
}

// *****************************************************
// Desc : ���� Ż���û ó��. (�������� ���� ������ ��)
// *****************************************************
HRESULT GLCharAG::MsgClubAllianceSecReq (NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_CLUB_ALLIANCE_SEC_REQ *pNetMsg = (GLMSG::SNET_CLUB_ALLIANCE_SEC_REQ *) nmg;
	GLMSG::SNET_CLUB_ALLIANCE_SEC_FB NetMsgFB;

	GLCharAG*  pChar = m_pServer->GetCharByDbNum ( m_CharDbNum );
	if ( !pChar )
        return S_FALSE;

	// ����Ŭ�������� ����Ż�� �ȵ�
	GLAGLandMan* pLandMan = m_pServer->GetLand(pChar->GetCurrentMap().getGaeaMapID());
	if (pLandMan->IsClubBattleMap())
	{
		NetMsgFB.emFB = EMCLUB_ALLIANCE_SEC_FB_GUIDMAP;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

	if ( m_ClubDbNum==CLUB_NULL )
	{
		NetMsgFB.emFB = EMCLUB_ALLIANCE_SEC_FB_NOTMASTER;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

	std::tr1::shared_ptr<GLClubAgent> pCLUB_INDIAN = m_pServer->GetClub(m_ClubDbNum);
	if ( !pCLUB_INDIAN )
	{
		NetMsgFB.emFB = EMCLUB_ALLIANCE_SEC_FB_NOTMASTER;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

	if ( !pCLUB_INDIAN->IsMaster(m_CharDbNum) )
	{
		NetMsgFB.emFB = EMCLUB_ALLIANCE_SEC_FB_NOTMASTER;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

	if ( !pCLUB_INDIAN->IsAlliance() )
	{
		NetMsgFB.emFB = EMCLUB_ALLIANCE_SEC_FB_ALLIANCE;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}
		
	if ( pCLUB_INDIAN->IsChief() )
	{
		NetMsgFB.emFB = EMCLUB_ALLIANCE_SEC_FB_FAIL;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

	std::tr1::shared_ptr<GLClubAgent> pCLUB_CHIEF = m_pServer->GetClub(pCLUB_INDIAN->m_dwAlliance);
	if ( !pCLUB_CHIEF )
	{
		NetMsgFB.emFB = EMCLUB_ALLIANCE_SEC_FB_FAIL;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

	CLUB_ALLIANCE_ITER del = pCLUB_CHIEF->m_setAlliance.find(pCLUB_INDIAN->m_DbNum);
	if ( del==pCLUB_CHIEF->m_setAlliance.end() )
	{
		NetMsgFB.emFB = EMCLUB_ALLIANCE_SEC_FB_FAIL;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

	if ( pCLUB_CHIEF->GetAllBattleNum() > 0 )
	{
		NetMsgFB.emFB = EMCLUB_ALLIANCE_SEC_FB_BATTLE;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

	//	Ż��� ���ѽð� ����.
	pCLUB_INDIAN->SetAllianceSec();
	
	AddGameDbJob(
        db::DbActionPtr(
            new db::ClubAllianceTimeSet(pCLUB_INDIAN->m_DbNum,pCLUB_INDIAN->m_tAllianceSec)));

	//	Ŭ���̾�Ʈ�� �˸� msg.
	GLMSG::SNET_CLUB_ALLIANCE_DEL_CLT NetMsgClt;
	NetMsgClt.dwDelClubID = pCLUB_INDIAN->m_DbNum;
	StringCchCopy ( NetMsgClt.szDelClub, CHAR_SZNAME, pCLUB_INDIAN->Name() );

	//	���� ���� ���� ����.
	pCLUB_INDIAN->ClearAlliance();

	//	���͵鿡�� ���� Ż�� �˸�.
	CLUB_ALLIANCE_ITER pos = pCLUB_CHIEF->m_setAlliance.begin();
	CLUB_ALLIANCE_ITER end = pCLUB_CHIEF->m_setAlliance.end();
	for ( ; pos!=end; ++pos )
	{
		const GLCLUBALLIANCE &sALLIANCE = pos->second;

		//	��Ÿ ���� Ŭ���� ����.
		m_pServer->SENDTOCLUBCLIENT ( sALLIANCE.m_dwID, &NetMsgClt );
	}

	// ���Ϳ��� ����
	pCLUB_CHIEF->m_setAlliance.erase ( del );

	//	���� Ŭ�� ���� �ʵ忡 �˸�.
	GLMSG::SNET_CLUB_ALLIANCE_DEL_FLD NetMsgFld;
	NetMsgFld.dwAlliance = pCLUB_CHIEF->m_dwAlliance;
	NetMsgFld.dwDelClubID = pCLUB_INDIAN->m_DbNum;
	m_pServer->SENDTOALLCHANNEL ( &NetMsgFld );

	//	���� Ż�� ��� �˸�.
	NetMsgFB.emFB = EMCLUB_ALLIANCE_SEC_FB_OK;
	m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );

	//	db�� ����.
	AddGameDbJob(
        db::DbActionPtr(
            new db::ClubAllianceDelete(pCLUB_CHIEF->m_DbNum, pCLUB_INDIAN->m_DbNum)));	

	// ���� ���Ϳ� Ŭ������ ������
	if ( pCLUB_CHIEF->m_setAlliance.size() < 2 )
	{
		//	���� ��ü
		GLMSG::SNET_CLUB_ALLIANCE_DIS_CLT NetMsgClt;
		NetMsgClt.SetName(pCLUB_CHIEF->Name());

		//	��ü�� ���ѽð� ����.
		pCLUB_CHIEF->SetAllianceDis();

		AddGameDbJob(
            db::DbActionPtr(
                new db::ClubAllianceDisolveTimeSet(pCLUB_CHIEF->m_DbNum,pCLUB_CHIEF->m_tAllianceDis)));
		
		//	Ŭ������ ���� ���� ����.
		pCLUB_CHIEF->ClearAlliance();

		AddGameDbJob(
            db::DbActionPtr(
                new db::AllianceBattleReSet(pCLUB_CHIEF->m_DbNum)));
		

		// Ŭ���̾�Ʈ�� ����.
		m_pServer->SENDTOCLUBCLIENT ( pCLUB_CHIEF->m_DbNum, &NetMsgClt );

		//	���� Ŭ�� ���� �ʵ忡 �˸�.
		GLMSG::SNET_CLUB_ALLIANCE_DIS_FLD NetMsgDisFld;
		NetMsgDisFld.dwChiefClubID = pCLUB_CHIEF->m_DbNum;
		m_pServer->SENDTOALLCHANNEL ( &NetMsgDisFld );		

	}

	return S_OK;
}

HRESULT GLCharAG::MsgClubAllianceDisReq(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_CLUB_ALLIANCE_DIS_REQ *pNetMsg = (GLMSG::SNET_CLUB_ALLIANCE_DIS_REQ *) nmg;
	GLMSG::SNET_CLUB_ALLIANCE_DIS_FB NetMsgFB;
	
	// ����Ŭ�������� ����Ż�� �ȵ�
	GLAGLandMan* pLandMan = m_pServer->GetLand(m_mapIDCurrent.getGaeaMapID());
	if (pLandMan->IsClubBattleMap())
	{
		NetMsgFB.emFB = EMCLUB_ALLIANCE_DIS_FB_GUIDMAP;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

	if ( m_ClubDbNum==CLUB_NULL )
	{
		NetMsgFB.emFB = EMCLUB_ALLIANCE_DIS_FB_NOTMASTER;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

	std::tr1::shared_ptr<GLClubAgent> pMyClub = m_pServer->GetClub(m_ClubDbNum);
	if ( !pMyClub )
	{
		NetMsgFB.emFB = EMCLUB_ALLIANCE_DIS_FB_NOTMASTER;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

	if ( !pMyClub->IsMaster(m_CharDbNum) )
	{
		NetMsgFB.emFB = EMCLUB_ALLIANCE_DIS_FB_NOTMASTER;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

	if ( !pMyClub->IsAlliance() )
	{
		NetMsgFB.emFB = EMCLUB_ALLIANCE_DIS_FB_NOTCHIEF;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}
		
	if ( !pMyClub->IsChief() )
	{
		NetMsgFB.emFB = EMCLUB_ALLIANCE_DIS_FB_NOTCHIEF;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

	if ( pMyClub->GetAllBattleNum() > 0 )
	{
		NetMsgFB.emFB = EMCLUB_ALLIANCE_DIS_FB_BATTLE;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

	GLMSG::SNET_CLUB_ALLIANCE_DIS_CLT NetMsgClt;
	NetMsgClt.SetName(pMyClub->Name());

	//	���͵鿡�� ���� ��ü �˸�.
	CLUB_ALLIANCE_ITER pos = pMyClub->m_setAlliance.begin();
	CLUB_ALLIANCE_ITER end = pMyClub->m_setAlliance.end();
	for ( ; pos!=end; ++pos)
	{
		const GLCLUBALLIANCE &sALLIANCE = pos->second;
		if (sALLIANCE.m_dwID==pMyClub->m_DbNum)
            continue;	//	Ŭ������ ���߿� ����.

		std::tr1::shared_ptr<GLClubAgent> pAllianceClub = m_pServer->GetClub(sALLIANCE.m_dwID);
		if (pAllianceClub)
        {
		    pAllianceClub->ClearAlliance();
		    // ��Ÿ ���� Ŭ���� Ŭ���̾�Ʈ�� ����
		    m_pServer->SENDTOCLUBCLIENT(sALLIANCE.m_dwID, &NetMsgClt);
		    // db�� ����
		    AddGameDbJob(
                db::DbActionPtr(
                    new db::ClubAllianceDelete(pMyClub->m_DbNum, pAllianceClub->DbNum())));
            
            // Club log
            std::string ClubLog(ID2GAMEINTEXT("CLUB_LOG_ALLIANCE"));
            SENDCLUBLOG(pAllianceClub->DbNum(), club::LOG_CLUB_ALLIANCE, ClubLog);
        }
	}

	//	��ü�� ���ѽð� ����.
	pMyClub->SetAllianceDis();

	AddGameDbJob(
        db::DbActionPtr(
            new db::ClubAllianceDisolveTimeSet(pMyClub->DbNum(), pMyClub->m_tAllianceDis)));	

	//	Ŭ������ ���� ���� ����.
	pMyClub->ClearAlliance();

    AddGameDbJob(
        db::DbActionPtr(
            new db::AllianceBattleReSet(pMyClub->DbNum())));

    // Club log
    std::string ClubLog(ID2GAMEINTEXT("CLUB_LOG_ALLIANCE"));
    SENDCLUBLOG(pMyClub->DbNum(), club::LOG_CLUB_ALLIANCE, ClubLog);

	// Ŭ���̾�Ʈ�� ����.
	m_pServer->SENDTOCLUBCLIENT ( pMyClub->m_DbNum, &NetMsgClt );

	// ���� Ŭ�� ���� �ʵ忡 �˸�
	GLMSG::SNET_CLUB_ALLIANCE_DIS_FLD NetMsgFld;
	NetMsgFld.dwChiefClubID = pMyClub->m_DbNum;
	m_pServer->SENDTOALLCHANNEL(&NetMsgFld);

	//	���� ��ü ��� �˸�.
	NetMsgFB.emFB = EMCLUB_ALLIANCE_DIS_FB_OK;
	m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);

	return S_OK;
}

HRESULT GLCharAG::MsgClubBattleReq(NET_MSG_GENERIC* nmg)
{
	if ( GLUseFeatures::GetInstance().IsUsingWorldBattle() )
	{
		sc::writeLogError( "The WorldBattle Server don't request battle of club." );

		return S_FALSE;
	}

	GLMSG::SNET_CLUB_BATTLE_REQ *pNetMsg = (GLMSG::SNET_CLUB_BATTLE_REQ *) nmg;
	GLMSG::SNET_CLUB_BATTLE_REQ_FB NetMsgFB;	
	
	if ( !GLCONST_CHAR::bCLUB_BATTLE )
	{
		NetMsgFB.emFB = EMCLUB_BATTLE_REQ_FB_FAIL;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

	GLCharAG*  pChar = m_pServer->GetCharByDbNum ( m_CharDbNum );
	if (!pChar)
        return S_FALSE;

	if ( m_sCONFTING.emTYPE != EMCONFT_NONE )
	{
		NetMsgFB.emFB = EMCLUB_BATTLE_REQ_FB_CONFT;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;		
	}

	if ( m_pServer->GuidanceIsBattle() )
	{
		NetMsgFB.emFB = EMCLUB_BATTLE_REQ_FB_GUIDNBATTLE;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;		
	}

	if ( m_pServer->ClubDeathMatchIsBattle() )
	{
		NetMsgFB.emFB = EMCLUB_BATTLE_REQ_FB_CDMBATTLE;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;		
	}

	if ( pNetMsg->dwBattleTime < GLCONST_CHAR::dwCLUB_BATTLE_TIMEMIN )
	{
		NetMsgFB.emFB = EMCLUB_BATTLE_REQ_FB_TIMEMIN;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;		
	}

	if ( pNetMsg->dwBattleTime > GLCONST_CHAR::dwCLUB_BATTLE_TIMEMAX )
	{
		NetMsgFB.emFB = EMCLUB_BATTLE_REQ_FB_TIMEMAX;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;		
	}
	

	// ����Ŭ�������� ��Ʋ �ȵ�
	GLAGLandMan* pLandMan(m_pServer->GetLand(pChar->GetCurrentMap().getGaeaMapID()));
	if (pLandMan->IsClubBattleMap())
	{
		NetMsgFB.emFB = EMCLUB_BATTLE_REQ_FB_GUIDMAP;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

	if (pLandMan->IsClubDeathMatchMap())
	{
		NetMsgFB.emFB = EMCLUB_BATTLE_REQ_FB_CDMMAP;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

	if ( m_ClubDbNum==CLUB_NULL )
	{
		NetMsgFB.emFB = EMCLUB_BATTLE_REQ_FB_NOTMASTER;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

	std::tr1::shared_ptr<GLClubAgent> pCLUB_CHIEF = m_pServer->GetClub ( m_ClubDbNum );
	if ( !pCLUB_CHIEF )
	{
		NetMsgFB.emFB = EMCLUB_BATTLE_REQ_FB_FAIL;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

    if ( !pCLUB_CHIEF->HaveClubBattleAuthority(m_CharDbNum) )
    {
        NetMsgFB.emFB = EMCLUB_BATTLE_REQ_FB_NO_AUTHORITY;
        m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
        return S_FALSE;
    }

	/*if ( !pCLUB_CHIEF->IsMaster(m_CharDbNum) )
	{
		NetMsgFB.emFB = EMCLUB_BATTLE_REQ_FB_NOTMASTER;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}*/

	if ( pCLUB_CHIEF->IsRegDissolution() )
	{
		NetMsgFB.emFB = EMCLUB_BATTLE_REQ_FB_DISSOLUTION;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

	// �ִ� ���� ���� ���� Ȯ��
	if ( pCLUB_CHIEF->GetAllBattleNum() >= GLCONST_CHAR::dwCLUB_BATTLE_MAX )
	{
		NetMsgFB.emFB = EMCLUB_BATTLE_REQ_FB_MAX;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

	if ( pCLUB_CHIEF->m_dwRank+1 < GLCONST_CHAR::dwCLUB_BATTLE_RANK )
	{
		NetMsgFB.emFB = EMCLUB_BATTLE_REQ_FB_RANK;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

	GLCharAG* pCHAR = m_pServer->GetCharByDbNum ( pNetMsg->dwCharID );
	if ( !pCHAR )
	{
		NetMsgFB.emFB = EMCLUB_BATTLE_REQ_FB_FAIL;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

	if ( pCHAR->GETCONFRONTING().emTYPE != EMCONFT_NONE )
	{
		NetMsgFB.emFB = EMCLUB_BATTLE_REQ_FB_TARCONFT;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;		
	}

	if ( pCHAR->m_ClubDbNum==CLUB_NULL )
	{
		NetMsgFB.emFB = EMCLUB_BATTLE_REQ_FB_TARNOTMASTER;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

	if ( pCLUB_CHIEF->IsAllianceGuild_CHIEF( pCHAR->m_ClubDbNum ) )
	{
		NetMsgFB.emFB = EMCLUB_BATTLE_REQ_FB_ALLIANCE;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}	

	if ( pCLUB_CHIEF->IsBattle( pCHAR->m_ClubDbNum ) || pCLUB_CHIEF->IsBattleReady( pCHAR->m_ClubDbNum ))
	{
		NetMsgFB.emFB = EMCLUB_BATTLE_REQ_FB_ALREADY;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

	std::tr1::shared_ptr<GLClubAgent> pCLUB_INDIAN = m_pServer->GetClub ( pCHAR->m_ClubDbNum );
	if ( !pCLUB_INDIAN )
	{
		NetMsgFB.emFB = EMCLUB_BATTLE_REQ_FB_TARNOTMASTER;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

	if ( !pCLUB_INDIAN->IsMaster(pCHAR->m_CharDbNum) )
	{
		NetMsgFB.emFB = EMCLUB_BATTLE_REQ_FB_TARNOTMASTER;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

	if ( pCLUB_INDIAN->IsBattle( m_ClubDbNum ) || pCLUB_INDIAN->IsBattleReady( m_ClubDbNum ))
	{
		NetMsgFB.emFB = EMCLUB_BATTLE_REQ_FB_ALREADY;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

	if ( pCLUB_CHIEF->IsBattleAlliance( pCLUB_INDIAN->m_dwAlliance ) || pCLUB_CHIEF->IsBattleReady( pCLUB_INDIAN->m_dwAlliance ))
	{
		NetMsgFB.emFB = EMCLUB_BATTLE_REQ_FB_ALREADY2;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

	if ( pCLUB_INDIAN->IsRegDissolution() )
	{
		NetMsgFB.emFB = EMCLUB_BATTLE_REQ_FB_DISSOLUTION2;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;

	}

    //! CTF �������̸� ���
    if ( pChar->m_bCTFPlayer )
    {
        NetMsgFB.emFB = EMCLUB_BATTLE_REQ_FB_FAIL;
        m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
        return S_FALSE;
    }

	// �ִ� ���� ���� ���� Ȯ��
	if ( pCLUB_INDIAN->GetAllBattleNum() >= GLCONST_CHAR::dwCLUB_BATTLE_MAX )
	{
		NetMsgFB.emFB = EMCLUB_BATTLE_REQ_FB_TARMAX;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}	

	//	Ŭ�� ��ũ Ȯ��
	if ( pCLUB_INDIAN->m_dwRank+1 < GLCONST_CHAR::dwCLUB_BATTLE_RANK )
	{
		NetMsgFB.emFB = EMCLUB_BATTLE_REQ_FB_TARRANK;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

	//	��Ʋ ��û�� Ŭ�� id ����.
	pCLUB_CHIEF->m_dwReqClubID = pCLUB_INDIAN->m_DbNum;
	pCLUB_CHIEF->m_dwBattleTime = pNetMsg->dwBattleTime;

	//
	GLMSG::SNET_CLUB_BATTLE_REQ_ASK NetMsgAsk;
	NetMsgAsk.dwClubCharID = m_CharDbNum;
	NetMsgAsk.dwBattleTime = pNetMsg->dwBattleTime;
	NetMsgAsk.SetClubName(pCLUB_CHIEF->Name());
	m_pServer->SENDTOCLIENT(pCHAR->m_ClientSlot, &NetMsgAsk);

	return S_OK;
}

HRESULT	GLCharAG::MsgAllianceBattleReq (NET_MSG_GENERIC* nmg)
{
	if ( GLUseFeatures::GetInstance().IsUsingWorldBattle() )
	{
		sc::writeLogError( "The WorldBattle Server don't request battle of alliance club." );

		return S_FALSE;
	}

	GLMSG::SNET_ALLIANCE_BATTLE_REQ *pNetMsg = (GLMSG::SNET_ALLIANCE_BATTLE_REQ *) nmg;
	GLMSG::SNET_ALLIANCE_BATTLE_REQ_FB NetMsgFB;
	
	if ( !GLCONST_CHAR::bCLUB_BATTLE || !GLCONST_CHAR::bCLUB_BATTLE_ALLIANCE )
	{
		NetMsgFB.emFB = EMALLIANCE_BATTLE_REQ_FB_FAIL;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

	GLCharAG*  pChar = m_pServer->GetCharByDbNum ( m_CharDbNum );
	if ( !pChar ) 
        return S_FALSE;

	if ( m_sCONFTING.emTYPE != EMCONFT_NONE )
	{
		NetMsgFB.emFB = EMALLIANCE_BATTLE_REQ_FB_CONFT;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;		
	}

	if ( m_pServer->GuidanceIsBattle() )
	{
		NetMsgFB.emFB = EMALLIANCE_BATTLE_REQ_FB_GUIDNBATTLE;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;		
	}

	if ( m_pServer->ClubDeathMatchIsBattle() )
	{
		NetMsgFB.emFB = EMALLIANCE_BATTLE_REQ_FB_CDMBATTLE;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;		
	}

	if ( pNetMsg->dwBattleTime < GLCONST_CHAR::dwCLUB_BATTLE_TIMEMIN )
	{
		NetMsgFB.emFB = EMALLIANCE_BATTLE_REQ_FB_TIMEMIN;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;		
	}

	if ( pNetMsg->dwBattleTime > GLCONST_CHAR::dwCLUB_BATTLE_TIMEMAX )
	{
		NetMsgFB.emFB = EMALLIANCE_BATTLE_REQ_FB_TIMEMAX;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;		
	}
	

	// ����Ŭ�������� ��Ʋ �ȵ�
	GLAGLandMan* pLandMan = m_pServer->GetLand(pChar->GetCurrentMap().getGaeaMapID());
	if (pLandMan->IsClubBattleMap())
	{
		NetMsgFB.emFB = EMALLIANCE_BATTLE_REQ_FB_GUIDMAP;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

	if (pLandMan->IsClubDeathMatchMap())
	{
		NetMsgFB.emFB = EMALLIANCE_BATTLE_REQ_FB_CDMMAP;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

	if ( m_ClubDbNum==CLUB_NULL )
	{
		NetMsgFB.emFB = EMALLIANCE_BATTLE_REQ_FB_NOTMASTER;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

	std::tr1::shared_ptr<GLClubAgent> pCLUB_CHIEF = m_pServer->GetClub ( m_ClubDbNum );
	if ( !pCLUB_CHIEF )
	{
		NetMsgFB.emFB = EMALLIANCE_BATTLE_REQ_FB_FAIL;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

    if ( !pCLUB_CHIEF->HaveClubBattleAuthority(m_CharDbNum) )
    {
        NetMsgFB.emFB = EMALLIANCE_BATTLE_REQ_FB_NO_AUTHORITY;
        m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
        return S_FALSE;
    }

	/*if ( !pCLUB_CHIEF->IsMaster(m_CharDbNum) )
	{
		NetMsgFB.emFB = EMALLIANCE_BATTLE_REQ_FB_NOTMASTER;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}*/

	if ( !pCLUB_CHIEF->IsAlliance() || !pCLUB_CHIEF->IsChief() )
	{
		NetMsgFB.emFB = EMALLIANCE_BATTLE_REQ_FB_NOTMASTER;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

	GLCharAG* pCHAR = m_pServer->GetCharByDbNum ( pNetMsg->dwCharID );
	if ( !pCHAR )
	{
		NetMsgFB.emFB = EMALLIANCE_BATTLE_REQ_FB_FAIL;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

	if ( pCHAR->GETCONFRONTING().emTYPE != EMCONFT_NONE )
	{
		NetMsgFB.emFB = EMALLIANCE_BATTLE_REQ_FB_TARCONFT;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;		
	}

	if ( pCHAR->m_ClubDbNum==CLUB_NULL )
	{
		NetMsgFB.emFB = EMALLIANCE_BATTLE_REQ_FB_TARNOTMASTER;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

	if ( pCLUB_CHIEF->IsAllianceGuild_CHIEF( pCHAR->m_ClubDbNum ) )
	{
		NetMsgFB.emFB = EMALLIANCE_BATTLE_REQ_FB_ALLIANCE;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

	std::tr1::shared_ptr<GLClubAgent> pCLUB_INDIAN = m_pServer->GetClub ( pCHAR->m_ClubDbNum );
	if ( !pCLUB_INDIAN )
	{
		NetMsgFB.emFB = EMALLIANCE_BATTLE_REQ_FB_TARNOTMASTER;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

	if ( !pCLUB_INDIAN->IsMaster(pCHAR->m_CharDbNum) )
	{
		NetMsgFB.emFB = EMALLIANCE_BATTLE_REQ_FB_TARNOTMASTER;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

	if ( !pCLUB_INDIAN->IsAlliance() || !pCLUB_INDIAN->IsChief() )
	{
		NetMsgFB.emFB = EMALLIANCE_BATTLE_REQ_FB_TARNOTMASTER;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}


	if ( pCLUB_CHIEF->IsBattleAlliance( pCLUB_INDIAN->m_dwAlliance ) || pCLUB_CHIEF->IsBattleReady( pCLUB_INDIAN->m_dwAlliance ))
	{
		NetMsgFB.emFB = EMALLIANCE_BATTLE_REQ_FB_ALREADY2;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_FALSE;
	}

    //! CTF �������̸� ���
    if ( pChar->m_bCTFPlayer )
    {
        NetMsgFB.emFB = EMALLIANCE_BATTLE_REQ_FB_FAIL;
        m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
        return S_FALSE;
    }

	// �ڽ� Ŭ�� Ȯ��
	CLUB_ALLIANCE_ITER pos = pCLUB_CHIEF->m_setAlliance.begin();
	CLUB_ALLIANCE_ITER end = pCLUB_CHIEF->m_setAlliance.end();
	for ( ; pos!=end; ++pos )
	{

		const GLCLUBALLIANCE &sALLIANCE = pos->second;
		std::tr1::shared_ptr<GLClubAgent> pCLUB = m_pServer->GetClub ( sALLIANCE.m_dwID );
		if ( !pCLUB )
            continue;

		if ( pCLUB->IsRegDissolution() )
		{
			NetMsgFB.emFB = EMALLIANCE_BATTLE_REQ_FB_DISSOLUTION;
			m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
			return S_FALSE;
		}

		// �ִ� ���� ���� ���� Ȯ��
		if ( pCLUB->GetAllBattleNum() >= GLCONST_CHAR::dwCLUB_BATTLE_MAX )
		{
			NetMsgFB.emFB = EMALLIANCE_BATTLE_REQ_FB_MAX;
			m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
			return S_FALSE;
		}

		if ( pCLUB->m_dwRank+1 < GLCONST_CHAR::dwCLUB_BATTLE_RANK )
		{
			NetMsgFB.emFB = EMALLIANCE_BATTLE_REQ_FB_RANK;
			m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
			return S_FALSE;
		}

	}


	//  ��� Ŭ�� Ȯ��
	pos = pCLUB_INDIAN->m_setAlliance.begin();
	end = pCLUB_INDIAN->m_setAlliance.end();
	for ( ; pos!=end; ++pos )
	{
		const GLCLUBALLIANCE &sALLIANCE = pos->second;
		std::tr1::shared_ptr<GLClubAgent> pCLUB = m_pServer->GetClub ( sALLIANCE.m_dwID );

		if ( !pCLUB )
            continue;

		if ( pCLUB->IsRegDissolution() )
		{
			NetMsgFB.emFB = EMALLIANCE_BATTLE_REQ_FB_DISSOLUTION2;
			m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
			return S_FALSE;
		}

		// �ִ� ���� ���� ���� Ȯ��
		if ( pCLUB->GetAllBattleNum() >= GLCONST_CHAR::dwCLUB_BATTLE_MAX )
		{
			NetMsgFB.emFB = EMALLIANCE_BATTLE_REQ_FB_TARMAX;
			m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
			return S_FALSE;
		}	

		//	Ŭ�� ��ũ Ȯ��
		if ( pCLUB->m_dwRank+1 < GLCONST_CHAR::dwCLUB_BATTLE_RANK )
		{
			NetMsgFB.emFB = EMALLIANCE_BATTLE_REQ_FB_TARRANK;
			m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
			return S_FALSE;
		}
	}



	//	���ͳ� Ŭ���鳢�� ��Ʋ������ Ȯ��
	{		
		// �ڽ� Ŭ�� Ȯ��
		CLUB_ALLIANCE_ITER pos = pCLUB_CHIEF->m_setAlliance.begin();
		CLUB_ALLIANCE_ITER end = pCLUB_CHIEF->m_setAlliance.end();
		for ( ; pos!=end; ++pos )
		{
			const GLCLUBALLIANCE &sALLIANCE = pos->second;
			std::tr1::shared_ptr<GLClubAgent> pCLUB = m_pServer->GetClub ( sALLIANCE.m_dwID );
			if( !pCLUB )
                continue;

			CLUB_ALLIANCE_ITER posTar = pCLUB_CHIEF->m_setAlliance.begin();
			CLUB_ALLIANCE_ITER endTar = pCLUB_CHIEF->m_setAlliance.end();
			for ( ; posTar!=endTar; ++posTar )
			{
				const GLCLUBALLIANCE &sALLIANCE = pos->second;

				if ( pCLUB->IsBattle( sALLIANCE.m_dwID ) || 
					pCLUB->IsBattleReady( sALLIANCE.m_dwID ))
				{
					NetMsgFB.emFB = EMALLIANCE_BATTLE_REQ_FB_ALREADY;
					m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
					return S_FALSE;
				}
			}
		}
	}

	//	��Ʋ ��û�� Ŭ�� id ����.
	pCLUB_CHIEF->m_dwReqClubID = pCLUB_INDIAN->m_DbNum;
	pCLUB_CHIEF->m_dwBattleTime = pNetMsg->dwBattleTime;

	// ���� ��û.
	GLMSG::SNET_ALLIANCE_BATTLE_REQ_ASK NetMsgAsk;
	NetMsgAsk.dwClubCharID = m_CharDbNum;
	NetMsgAsk.dwBattleTime = pNetMsg->dwBattleTime;
	NetMsgAsk.SetClubName(pCLUB_CHIEF->Name());
	m_pServer->SENDTOCLIENT(pCHAR->m_ClientSlot, &NetMsgAsk);

	return S_OK;
}

HRESULT GLCharAG::MsgClubBattleReqAns(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_CLUB_BATTLE_REQ_ANS *pNetMsg = (GLMSG::SNET_CLUB_BATTLE_REQ_ANS *) nmg;
	GLMSG::SNET_CLUB_BATTLE_REQ_FB NetMsgFB;

	if ( !GLCONST_CHAR::bCLUB_BATTLE )
	{
		return S_FALSE;
	}

	if ( m_ClubDbNum==CLUB_NULL )
	{
		return S_FALSE;
	}

	if ( m_sCONFTING.emTYPE != EMCONFT_NONE )
	{
		return S_FALSE;
	}

	if ( m_pServer->GuidanceIsBattle() )
	{
		return S_FALSE;		
	}

	if ( m_pServer->ClubDeathMatchIsBattle() )
	{
		return S_FALSE;		
	}

	std::tr1::shared_ptr<GLClubAgent> pIndianClub = m_pServer->GetClub ( m_ClubDbNum );
	if ( !pIndianClub )
	{
		return S_FALSE;
	}

	if ( !pIndianClub->IsMaster(m_CharDbNum) )
	{
		return S_FALSE;
	}

	GLCharAG* pChiefChar = m_pServer->GetCharByDbNum ( pNetMsg->dwClubCharID );
	if ( !pChiefChar )
	{
		return S_FALSE;
	}	

	if ( pChiefChar->m_ClubDbNum==CLUB_NULL )
	{
		return S_FALSE;
	}

	if ( pChiefChar->m_sCONFTING.emTYPE != EMCONFT_NONE )
	{
		return S_FALSE;
	}

	std::tr1::shared_ptr<GLClubAgent> pChiefClub = m_pServer->GetClub ( pChiefChar->m_ClubDbNum );
	if ( !pChiefClub )
	{
		return S_FALSE;
	}
	
	if ( !pChiefClub->HaveClubBattleAuthority(pChiefChar->m_CharDbNum) )
	{
		return S_FALSE;
	}

	if ( !pNetMsg->bOK )
	{
		NetMsgFB.emFB = EMCLUB_BATTLE_REQ_FB_REFUSE;
		m_pServer->SENDTOCLIENT ( pChiefChar->m_ClientSlot, &NetMsgFB );

        std::string strText = sc::string::format(ID2SERVERTEXT("CLUB_BATTLE_BEGIN_REFUSE"), pIndianClub->Name(), pChiefClub->Name());

		GLMSG::SNET_SERVER_GENERALCHAT SendData;
		if (SendData.SETTEXT(strText))
        {
            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, SendData);
		    m_pServer->SENDTOALLCLIENT(NET_MSG_SERVER_GENERALCHAT, SendBuffer);
        }

		return S_FALSE;
	}

	//	��û�� Ŭ������ �˻�.
	if ( pChiefClub->m_dwReqClubID!=pIndianClub->m_DbNum )
	{
		return S_FALSE;
	}

	// �ð� �˻�
	if ( pChiefClub->m_dwBattleTime < GLCONST_CHAR::dwCLUB_BATTLE_TIMEMIN 
		|| pChiefClub->m_dwBattleTime > GLCONST_CHAR::dwCLUB_BATTLE_TIMEMAX  )
	{
		return S_FALSE;
	}

	NetMsgFB.emFB = EMCLUB_BATTLE_REQ_FB_OK;
	m_pServer->SENDTOCLIENT ( pChiefChar->m_ClientSlot, &NetMsgFB );

	GLCLUBBATTLE	sClubBattle;
	CTime cCurTime = CTime::GetCurrentTime();
	CTimeSpan cDelayTime(0,0,1,0);	
	CTime cStartTime = cCurTime + cDelayTime;
	CTimeSpan cBattleTime(0,0,pChiefClub->m_dwBattleTime,0);
	CTime cEndTime = cStartTime + cBattleTime;

	// Ŭ�� ��Ʋ ��⿭�� ���( ��û�� ���� ) 
	sClubBattle.m_dwCLUBID = pIndianClub->m_DbNum;
	sClubBattle.m_tStartTime = cStartTime.GetTime();
	sClubBattle.m_tEndTime = cEndTime.GetTime();
	sClubBattle.SetName(pIndianClub->Name());
    pChiefClub->m_mapBattleReady.insert(std::make_pair(sClubBattle.m_dwCLUBID, sClubBattle));	

	//	Ŭ���̾�Ʈ�鿡 �˸�. ( ���Ϳ��鿡�� �˸� )
	GLMSG::SNET_CLUB_BATTLE_BEGIN_CLT NetMsgClt;
	NetMsgClt.SetName(pChiefClub->Name());
	m_pServer->SENDTOCLUBCLIENT(pIndianClub->m_DbNum, &NetMsgClt);
	
	NetMsgClt.SetName(pIndianClub->Name());
	m_pServer->SENDTOCLUBCLIENT(pChiefClub->m_DbNum, &NetMsgClt);

	// db�� ����
	AddGameDbJob(
        db::DbActionPtr(
            new db::ClubBattleSet(pChiefClub->m_DbNum, pIndianClub->m_DbNum, pChiefClub->m_dwBattleTime)));

	AddGameDbJob(
        db::DbActionPtr(
            new db::ClubBattleSet(pIndianClub->m_DbNum, pChiefClub->m_DbNum, pChiefClub->m_dwBattleTime)));

    // Club log/Club battle/%1%���� %2% Ŭ���� ��Ʋ�� ��û�ϼ̽��ϴ�.
    std::string ClubLog1(
        sc::string::format(
            ID2GAMEINTEXT("CLUB_LOG_BATTLE"),
            m_szName,
            pChiefClub->Name()));
    SENDCLUBLOG(club::LOG_CLUB_BATTLE, ClubLog1);

    std::string ClubLog2(
        sc::string::format(
            ID2GAMEINTEXT("CLUB_LOG_BATTLE"),
            pChiefChar->GetChaName(),
            pIndianClub->Name()));
    SENDCLUBLOG(pChiefClub->m_DbNum, club::LOG_CLUB_BATTLE, ClubLog2);

	return S_OK;
}

// *****************************************************
// Desc: Agent���� ��Ʋ��û ��� FeedBack. ( ���� ) 
// *****************************************************
HRESULT	GLCharAG::MsgAllianceBattleReqAns (NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_ALLIANCE_BATTLE_REQ_ANS *pNetMsg = (GLMSG::SNET_ALLIANCE_BATTLE_REQ_ANS *) nmg;
	GLMSG::SNET_ALLIANCE_BATTLE_REQ_FB NetMsgFB;

	if ( !GLCONST_CHAR::bCLUB_BATTLE || !GLCONST_CHAR::bCLUB_BATTLE_ALLIANCE )
	{
		return S_FALSE;
	}

	if ( m_ClubDbNum==CLUB_NULL )
	{
		return S_FALSE;
	}

	if ( m_sCONFTING.emTYPE != EMCONFT_NONE )
	{
		return S_FALSE;
	}

	if ( m_pServer->GuidanceIsBattle() )
	{
		return S_FALSE;		
	}

	if ( m_pServer->ClubDeathMatchIsBattle() )
	{
		return S_FALSE;		
	}

	std::tr1::shared_ptr<GLClubAgent> pMyClub = m_pServer->GetClub ( m_ClubDbNum );
	if ( !pMyClub )
	{
		return S_FALSE;
	}

	if ( !pMyClub->IsMaster(m_CharDbNum) )
	{
		return S_FALSE;
	}

	if ( !pMyClub->IsAlliance() || !pMyClub->IsChief() )
	{
		return S_FALSE;
	}

	GLCharAG* pTarChar = m_pServer->GetCharByDbNum ( pNetMsg->dwClubCharID );
	if ( !pTarChar )
	{
		return S_FALSE;
	}	

	if ( pTarChar->m_ClubDbNum==CLUB_NULL )
	{
		return S_FALSE;
	}

	if ( pTarChar->m_sCONFTING.emTYPE != EMCONFT_NONE )
	{
		return S_FALSE;
	}

	std::tr1::shared_ptr<GLClubAgent> pTarClub = m_pServer->GetClub ( pTarChar->m_ClubDbNum );
	if ( !pTarClub )
	{
		return S_FALSE;
	}
	
	if ( !pTarClub->HaveClubBattleAuthority(pTarChar->m_CharDbNum) )
	{
		return S_FALSE;
	}

	if ( !pTarClub->IsAlliance() || !pTarClub->IsChief() )
	{
		return S_FALSE;
	}

	if ( !pNetMsg->bOK )
	{
		NetMsgFB.emFB = EMALLIANCE_BATTLE_REQ_FB_REFUSE;
		m_pServer->SENDTOCLIENT ( pTarChar->m_ClientSlot, &NetMsgFB );

        std::string strText(
            sc::string::format(
                ID2SERVERTEXT("ALLIANCE_BATTLE_BEGIN_REFUSE"),
                pMyClub->Name(),
                pTarClub->Name()));

		GLMSG::SNET_SERVER_GENERALCHAT SendData;
		if (SendData.SETTEXT(strText))
        {
            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, SendData);
		    m_pServer->SENDTOALLCLIENT(NET_MSG_SERVER_GENERALCHAT, SendBuffer);
        }

		return S_FALSE;
	}

	//	��û�� Ŭ������ �˻�.
	if ( pTarClub->m_dwReqClubID!=pMyClub->m_DbNum )
	{
		return S_FALSE;
	}

	// �ð� �˻�
	if ( pTarClub->m_dwBattleTime < GLCONST_CHAR::dwCLUB_BATTLE_TIMEMIN 
		|| pTarClub->m_dwBattleTime > GLCONST_CHAR::dwCLUB_BATTLE_TIMEMAX  )
	{
		return S_FALSE;
	}

	NetMsgFB.emFB = EMALLIANCE_BATTLE_REQ_FB_OK;
	m_pServer->SENDTOCLIENT ( pTarChar->m_ClientSlot, &NetMsgFB );

	GLCLUBBATTLE	sClubBattle;
	CTime cCurTime = CTime::GetCurrentTime();
	CTimeSpan cDelayTime(0,0,1,0);	
	CTime cStartTime = cCurTime + cDelayTime;
	CTimeSpan cBattleTime(0,0,pTarClub->m_dwBattleTime,0);
	CTime cEndTime = cStartTime + cBattleTime;

	// Ŭ�� ��Ʋ ��⿭�� ���( ��û�� ���� ) 
	sClubBattle.m_dwCLUBID = pMyClub->m_DbNum;
	sClubBattle.m_tStartTime = cStartTime.GetTime();
	sClubBattle.m_tEndTime = cEndTime.GetTime();
	sClubBattle.m_bAlliance = true;
	sClubBattle.SetName(pMyClub->Name());
	pTarClub->m_mapBattleReady.insert(std::make_pair(sClubBattle.m_dwCLUBID, sClubBattle));

	
	//	Ŭ���̾�Ʈ�鿡 �˸�. ( ���Ϳ��鿡�� �˸� )
	GLMSG::SNET_CLUB_BATTLE_BEGIN_CLT NetMsgClt;

	NetMsgClt.SetName(pTarClub->Name());
	NetMsgClt.bAlliance = true;
	m_pServer->SENDTOALLIANCECLIENT(pMyClub->m_DbNum, &NetMsgClt);
	
	NetMsgClt.SetName(pMyClub->Name());
	m_pServer->SENDTOALLIANCECLIENT(pTarClub->m_DbNum, &NetMsgClt);

	// Database save
	AddGameDbJob(
        db::DbActionPtr(
            new db::ClubBattleSet(pTarClub->m_DbNum, pMyClub->m_DbNum, pTarClub->m_dwBattleTime, true)));

    // Club log
    std::string ClubLog1(
        sc::string::format(
            ID2GAMEINTEXT("CLUB_LOG_BATTLE2"),            
            m_szName,
            pTarClub->Name()));
    SENDCLUBLOG(m_ClubDbNum, club::LOG_CLUB_BATTLE, ClubLog1);

    // Database save
	AddGameDbJob(
        db::DbActionPtr(
            new db::ClubBattleSet(pMyClub->m_DbNum, pTarClub->m_DbNum, pTarClub->m_dwBattleTime, true)));    

    // Club log
    std::string ClubLog2(
        sc::string::format(
            ID2GAMEINTEXT("CLUB_LOG_BATTLE2"),            
            pTarChar->GetChaName(),
            pMyClub->Name()));
    SENDCLUBLOG(pTarClub->m_DbNum, club::LOG_CLUB_BATTLE, ClubLog2);

	return S_OK;
}

//! Ŭ�� ��Ʋ ������û Client->Agent
HRESULT GLCharAG::MsgClubBattleArmisticeReq(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_CLUB_BATTLE_ARMISTICE_REQ* pNetMsg = (GLMSG::SNET_CLUB_BATTLE_ARMISTICE_REQ*) nmg;
	GLMSG::SNET_CLUB_BATTLE_ARMISTICE_REQ_FB NetMsgFB;

	//GLCharAG* pChar = m_pServer->GetCharByDbNum( m_CharDbNum );
	//if ( !pChar )
    //    return S_FALSE;

	// ����Ŭ�������� ���� �ȵ�
	GLAGLandMan* pLandMan = m_pServer->GetLand(m_mapIDCurrent.getGaeaMapID());
	if (!pLandMan || pLandMan->IsClubBattleMap())
	{
		NetMsgFB.emFB = EMCLUB_BATTLE_ARMISTICE_FB_GUIDMAP;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}

	if (pLandMan->IsClubDeathMatchMap())
	{
		NetMsgFB.emFB = EMCLUB_BATTLE_ARMISTICE_FB_CDMMAP;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}

	if (!IsValidClub())
	{
		NetMsgFB.emFB = EMCLUB_BATTLE_ARMISTICE_FB_NOTMASTER;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}

	std::tr1::shared_ptr<GLClubAgent> pCLUB_CHIEF = m_pServer->GetClub(m_ClubDbNum);
	if (!pCLUB_CHIEF)
	{
		NetMsgFB.emFB = EMCLUB_BATTLE_ARMISTICE_FB_FAIL;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}

    /*
	if (!pCLUB_CHIEF->IsMaster(m_CharDbNum))
	{
		NetMsgFB.emFB = EMCLUB_BATTLE_ARMISTICE_FB_NOTMASTER;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}
    */
    if (!pCLUB_CHIEF->HaveClubBattleAuthority(m_CharDbNum))
    {
        NetMsgFB.emFB = EMCLUB_BATTLE_NO_AUTHORITY;
        m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
        return S_FALSE;
    }

	if (!pCLUB_CHIEF->IsBattle(pNetMsg->dwClubID))
	{
		NetMsgFB.emFB = EMCLUB_BATTLE_ARMISTICE_FB_NOBATTLE;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}

	if (!pCLUB_CHIEF->IsBattleStop(pNetMsg->dwClubID))
	{
		NetMsgFB.emFB = EMCLUB_BATTLE_ARMISTICE_FB_DISTIME;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}
	
	std::tr1::shared_ptr<GLClubAgent> pCLUB_INDIAN = m_pServer->GetClub(pNetMsg->dwClubID);
	if (!pCLUB_INDIAN)
	{
		NetMsgFB.emFB = EMCLUB_BATTLE_ARMISTICE_FB_FAIL;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}	

	GLCharAG* pTarMaster = m_pServer->GetCharByDbNum(pCLUB_INDIAN->MasterCharDbNum());
	if (!pTarMaster)
	{
		NetMsgFB.emFB = EMCLUB_BATTLE_ARMISTICE_FB_OFFMASTER;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}

	//	��Ʋ ��û�� Ŭ�� id ����.
	pCLUB_CHIEF->m_dwBattleArmisticeReq = pNetMsg->dwClubID;

	// ���� ��û.
	GLMSG::SNET_CLUB_BATTLE_ARMISTICE_REQ_ASK NetMsgAsk(pCLUB_CHIEF->m_DbNum, m_CharDbNum);
	m_pServer->SENDTOCLIENT(pTarMaster->m_ClientSlot, &NetMsgAsk);
	return S_OK;
}

//! Agent���� ��Ʋ ������û (����) Client->Agent
HRESULT	GLCharAG::MsgAllianceBattleArmisticeReq(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_ALLIANCE_BATTLE_ARMISTICE_REQ* pNetMsg = (GLMSG::SNET_ALLIANCE_BATTLE_ARMISTICE_REQ*) nmg;
	GLMSG::SNET_ALLIANCE_BATTLE_ARMISTICE_REQ_FB NetMsgFB;

	//GLCharAG* pChar = m_pServer->GetCharByDbNum( m_CharDbNum );
	//if (!pChar)
    //    return S_FALSE;

	// ����Ŭ�������� ���� �ȵ�
	GLAGLandMan* pLandMan = m_pServer->GetLand(m_mapIDCurrent.getGaeaMapID());
	if (!pLandMan || pLandMan->IsClubBattleMap())
	{
		NetMsgFB.emFB = EMALLIANCE_BATTLE_ARMISTICE_FB_GUIDMAP;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}

	// ���� �ȵ�
	if (pLandMan->IsClubDeathMatchMap())
	{
		NetMsgFB.emFB = EMALLIANCE_BATTLE_ARMISTICE_FB_CDMMAP;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}

	if (!IsValidClub())
	{
		NetMsgFB.emFB = EMALLIANCE_BATTLE_ARMISTICE_FB_NOTMASTER;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}

	std::tr1::shared_ptr<GLClubAgent> pCLUB_CHIEF = m_pServer->GetClub(m_ClubDbNum);
	if (!pCLUB_CHIEF)
	{
		NetMsgFB.emFB = EMALLIANCE_BATTLE_ARMISTICE_FB_FAIL;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}

    /*
	if (!pCLUB_CHIEF->IsMaster(m_CharDbNum))
	{
		NetMsgFB.emFB = EMALLIANCE_BATTLE_ARMISTICE_FB_NOTMASTER;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}
    */
    if (!pCLUB_CHIEF->HaveClubBattleAuthority(m_CharDbNum))
    {
        NetMsgFB.emFB = EMALLIANCE_BATTLE_NO_AUTHORITY;
        m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
        return S_FALSE;
    }

	if (!pCLUB_CHIEF->IsAlliance() || !pCLUB_CHIEF->IsChief()) 
	{
		NetMsgFB.emFB = EMALLIANCE_BATTLE_ARMISTICE_FB_NOTMASTER;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}

	if (!pCLUB_CHIEF->IsBattleAlliance(pNetMsg->dwClubID))
	{
		NetMsgFB.emFB = EMALLIANCE_BATTLE_ARMISTICE_FB_NOBATTLE;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}

	if (!pCLUB_CHIEF->IsBattleStop(pNetMsg->dwClubID))
	{
		NetMsgFB.emFB = EMALLIANCE_BATTLE_ARMISTICE_FB_DISTIME;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}
	
	std::tr1::shared_ptr<GLClubAgent> pCLUB_INDIAN = m_pServer->GetClub(pNetMsg->dwClubID);
	if (!pCLUB_INDIAN)
	{
		NetMsgFB.emFB = EMALLIANCE_BATTLE_ARMISTICE_FB_FAIL;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}
    
	GLCharAG* pCHAR = m_pServer->GetCharByDbNum(pCLUB_INDIAN->MasterCharDbNum());
	if (!pCHAR)
	{
		NetMsgFB.emFB = EMALLIANCE_BATTLE_ARMISTICE_FB_OFFMASTER;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}

	//	��Ʋ ��û�� Ŭ�� id ����.
	pCLUB_CHIEF->m_dwBattleArmisticeReq = pNetMsg->dwClubID;

	// ���� ��û.
	GLMSG::SNET_ALLIANCE_BATTLE_ARMISTICE_REQ_ASK NetMsgAsk(pCLUB_CHIEF->m_DbNum, m_CharDbNum);
	m_pServer->SENDTOCLIENT(pCHAR->m_ClientSlot, &NetMsgAsk);
	return S_OK;
}

//! Ŭ�� ��Ʋ ���� �亯 Client->Agent
HRESULT GLCharAG::MsgClubBattleArmisticeReqAns(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_CLUB_BATTLE_ARMISTICE_REQ_ANS* pNetMsg = (GLMSG::SNET_CLUB_BATTLE_ARMISTICE_REQ_ANS*) nmg;
	GLMSG::SNET_CLUB_BATTLE_ARMISTICE_REQ_FB NetMsgFB;

	if (!IsValidClub())
		return S_FALSE;

	std::tr1::shared_ptr<GLClubAgent> pMyClub = m_pServer->GetClub(m_ClubDbNum);
	if (!pMyClub)
		return S_FALSE;

	//if (!pMyClub->IsMaster(m_CharDbNum))
	//	return S_FALSE;
    if (!pMyClub->HaveClubBattleAuthority(m_CharDbNum))
        return S_FALSE;

	std::tr1::shared_ptr<GLClubAgent> pTarClub = m_pServer->GetClub(pNetMsg->dwClubID);
	if (!pTarClub)
		return S_FALSE;

	GLCharAG* pTarChar = m_pServer->GetCharByDbNum(pNetMsg->m_ReqChaDbNum);
	if (!pTarChar)
		return S_FALSE;

    // Check authority
    if (!pTarClub->HaveClubBattleAuthority(pNetMsg->m_ReqChaDbNum))
        return S_FALSE;

	if (!pNetMsg->bOK)
	{
		NetMsgFB.emFB = EMCLUB_BATTLE_ARMISTICE_FB_REFUSE;
		m_pServer->SENDTOCLIENT(pTarChar->m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}

	//	��û�� Ŭ������ �˻�.
	if (pTarClub->m_dwBattleArmisticeReq != pMyClub->DbNum())
		return S_FALSE;

	GLCLUBBATTLE* pClubBattle = pTarClub->GetClubBattle(pMyClub->DbNum());
	if (!pClubBattle)
        return S_FALSE;

	int nKill = pClubBattle->m_wKillPoint;
	int nDeath = pClubBattle->m_wDeathPoint;

	//	���� ��û ���.
	NetMsgFB.emFB = EMCLUB_BATTLE_ARMISTICE_FB_OK;
	m_pServer->SENDTOCLIENT(pTarChar->m_ClientSlot, &NetMsgFB);

    pTarClub->m_tLastBattle = sc::time::GetCurrentTime();
	pTarClub->m_dwBattleDraw++;
	pTarClub->DELBATTLECLUB(pMyClub->DbNum());

	pMyClub->m_tLastBattle = sc::time::GetCurrentTime();
	pMyClub->m_dwBattleDraw++;
	pMyClub->DELBATTLECLUB(pTarClub->DbNum());

	//	�ʵ忡 �˸�.
	GLMSG::SNET_CLUB_BATTLE_OVER_FLD NetMsgFld;
	NetMsgFld.emFB = EMCLUB_BATTLE_OVER_ARMISTICE;
    NetMsgFld.dwWinClubID = pTarClub->DbNum();
	NetMsgFld.dwLoseClubID = pMyClub->DbNum();
	m_pServer->SENDTOALLCHANNEL(&NetMsgFld);


	//	Ŭ���̾�Ʈ�鿡 �˸�. ( ���Ϳ��鿡�� �˸� )
	GLMSG::SNET_CLUB_BATTLE_OVER_CLT NetMsgClt;
	NetMsgClt.emFB = EMCLUB_BATTLE_OVER_ARMISTICE;
	
	NetMsgClt.dwClubID = pTarClub->DbNum();	
	m_pServer->SENDTOCLUBCLIENT(pMyClub->DbNum(), &NetMsgClt);
	
	NetMsgClt.dwClubID = pMyClub->m_DbNum;	
	m_pServer->SENDTOCLUBCLIENT(pTarClub->DbNum(), &NetMsgClt);

    std::string strText = sc::string::format(ID2SERVERTEXT("CLUB_BATTLE_OVER_ARMISTICE"), pTarClub->Name(), pMyClub->Name());
	
	GLMSG::SNET_SERVER_GENERALCHAT SendData;
	if (SendData.SETTEXT(strText))
    {
        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, SendData);
	    m_pServer->SENDTOALLCLIENT(NET_MSG_SERVER_GENERALCHAT, SendBuffer);
    }

	strText = sc::string::format(ID2SERVERTEXT("CLUB_BATTLE_OVER_ARMISTICE_RESULT"), nKill, nDeath);	

    if (SendData.SETTEXT(strText))
    {
        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, SendData);
        m_pServer->SENDTOALLCLIENT(NET_MSG_SERVER_GENERALCHAT, SendBuffer);
    }

	strText = sc::string::format(
        ID2SERVERTEXT("CLUB_BATTLE_RESULT"),
        pTarClub->Name(), 
        pTarClub->m_dwBattleWin,
        pTarClub->m_dwBattleLose,
        pTarClub->m_dwBattleDraw);

    if (SendData.SETTEXT(strText))
    {
        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, SendData);
        m_pServer->SENDTOALLCLIENT(NET_MSG_SERVER_GENERALCHAT, SendBuffer);
    }
	
	strText = sc::string::format(
        ID2SERVERTEXT("CLUB_BATTLE_RESULT"),
        pMyClub->Name(), 
        pMyClub->m_dwBattleWin,
        pMyClub->m_dwBattleLose,
        pMyClub->m_dwBattleDraw);

    if (SendData.SETTEXT(strText))
    {
        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, SendData);
        m_pServer->SENDTOALLCLIENT(NET_MSG_SERVER_GENERALCHAT, SendBuffer);
    }

	// database save
	AddGameDbJob(
        db::DbActionPtr(
            new db::ClubBattleEnd(
                pTarClub->m_DbNum,
                pMyClub->m_DbNum, 
                GLCLUBBATTLE::CLUB_BATTLE_ARMISTICE,
                0,
                0)));

    AddGameDbJob(
        db::DbActionPtr(
            new db::ClubBattleEnd(
                pMyClub->m_DbNum,
                pTarClub->m_DbNum,
                GLCLUBBATTLE::CLUB_BATTLE_ARMISTICE,
                0,
                0)));

    // Club log/%1% Ŭ���� �����Ͽ����ϴ�.
    std::string ClubLog1(
        sc::string::format(
            ID2GAMEINTEXT("CLUB_LOG_BATTLE6"),
            pTarClub->Name()));
    SENDCLUBLOG(m_ClubDbNum, club::LOG_CLUB_BATTLE, ClubLog1);
    
    std::string ClubLog2(
        sc::string::format(
            ID2GAMEINTEXT("CLUB_LOG_BATTLE6"),
            pMyClub->Name()));
    SENDCLUBLOG(pTarClub->DbNum(), club::LOG_CLUB_BATTLE, ClubLog2);
    
	return S_OK;
}

HRESULT GLCharAG::MsgAllianceBattleArmisticeReqAns(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_ALLIANCE_BATTLE_ARMISTICE_REQ_ANS* pNetMsg = (GLMSG::SNET_ALLIANCE_BATTLE_ARMISTICE_REQ_ANS*) nmg;
	GLMSG::SNET_ALLIANCE_BATTLE_ARMISTICE_REQ_FB NetMsgFB;

	if (!IsValidClub())
		return S_FALSE;

	std::tr1::shared_ptr<GLClubAgent> pMyClub = m_pServer->GetClub(m_ClubDbNum);
	if (!pMyClub)
		return S_FALSE;

	//if (!pMyClub->IsMaster(m_CharDbNum))
	//	return S_FALSE;
    if (!pMyClub->HaveClubBattleAuthority(m_CharDbNum))
        return S_FALSE;

	if (!pMyClub->IsAlliance() || !pMyClub->IsChief())
		return S_FALSE;

	std::tr1::shared_ptr<GLClubAgent> pTarClub = m_pServer->GetClub(pNetMsg->dwClubID);
	if (!pTarClub)
		return S_FALSE;

	GLCharAG* pTarChar = m_pServer->GetCharByDbNum(pNetMsg->m_ReqChaDbNum);
	if (!pTarChar)
		return S_FALSE;

	if (!pNetMsg->bOK)
	{
		NetMsgFB.emFB = EMALLIANCE_BATTLE_ARMISTICE_FB_REFUSE;
		m_pServer->SENDTOCLIENT(pTarChar->m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}

	//	��û�� Ŭ������ �˻�.
	if (pTarClub->m_dwBattleArmisticeReq != pMyClub->m_DbNum)
		return S_FALSE;

	GLCLUBBATTLE* pClubBattle = pTarClub->GetClubBattle(pMyClub->m_DbNum);
	if (!pClubBattle)
        return S_FALSE;

	int nKill = pClubBattle->m_wKillPoint;
	int nDeath = pClubBattle->m_wDeathPoint;

	//	���� ��û ���.
	NetMsgFB.emFB = EMALLIANCE_BATTLE_ARMISTICE_FB_OK;
	m_pServer->SENDTOCLIENT(pTarChar->m_ClientSlot, &NetMsgFB);

	CLUB_ALLIANCE_ITER pos = pTarClub->m_setAlliance.begin();
	CLUB_ALLIANCE_ITER end = pTarClub->m_setAlliance.end();
	for ( ; pos!=end; ++pos)
	{
		const GLCLUBALLIANCE& sALLIANCE = pos->second;
		std::tr1::shared_ptr<GLClubAgent> pTarAllianceClub = m_pServer->GetClub ( sALLIANCE.m_dwID );
		if (pTarAllianceClub)
        {
		    pTarAllianceClub->m_tLastBattle = CTime::GetCurrentTime().GetTime();
		    pTarAllianceClub->m_dwAllianceBattleDraw++;
		    pTarAllianceClub->DELBATTLECLUB(pMyClub->m_DbNum);
        }
	}

	pos = pMyClub->m_setAlliance.begin();
	end = pMyClub->m_setAlliance.end();
	for ( ; pos!=end; ++pos)
	{
		const GLCLUBALLIANCE& sALLIANCE = pos->second;
		std::tr1::shared_ptr<GLClubAgent> pMyAllianceClub = m_pServer->GetClub(sALLIANCE.m_dwID);
		if (pMyAllianceClub)
        {
		    pMyAllianceClub->m_tLastBattle = CTime::GetCurrentTime().GetTime();
		    pMyAllianceClub->m_dwAllianceBattleDraw++;
		    pMyAllianceClub->DELBATTLECLUB(pTarClub->m_DbNum);
        }
	}

	//	�ʵ忡 �˸�.
	GLMSG::SNET_CLUB_BATTLE_OVER_FLD NetMsgFld;
	NetMsgFld.emFB = EMCLUB_BATTLE_OVER_ARMISTICE;
    NetMsgFld.dwWinClubID = pTarClub->m_DbNum;
	NetMsgFld.dwLoseClubID = pMyClub->m_DbNum;
	m_pServer->SENDTOALLCHANNEL(&NetMsgFld);

	//	Ŭ���̾�Ʈ�鿡 �˸�. ( ���Ϳ��鿡�� �˸� )
	GLMSG::SNET_CLUB_BATTLE_OVER_CLT NetMsgClt;
	NetMsgClt.emFB = EMCLUB_BATTLE_OVER_ARMISTICE;
	
	NetMsgClt.dwClubID = pTarClub->m_DbNum;	
	m_pServer->SENDTOALLIANCECLIENT(pMyClub->m_DbNum, &NetMsgClt);
	
	NetMsgClt.dwClubID = pMyClub->m_DbNum;	
	m_pServer->SENDTOALLIANCECLIENT(pTarClub->m_DbNum, &NetMsgClt);

    std::string strText = sc::string::format(ID2SERVERTEXT("ALLIANCE_BATTLE_OVER_ARMISTICE"), pTarClub->Name(), pMyClub->Name());

	GLMSG::SNET_SERVER_GENERALCHAT SendData;
    if (SendData.SETTEXT(strText))
    {
        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, SendData);
        m_pServer->SENDTOALLCLIENT(NET_MSG_SERVER_GENERALCHAT, SendBuffer);
    }

    strText = sc::string::format(ID2SERVERTEXT("ALLIANCE_BATTLE_OVER_ARMISTICE_RESULT"), nKill, nDeath);	
    if (SendData.SETTEXT(strText))
    {
        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, SendData);
        m_pServer->SENDTOALLCLIENT(NET_MSG_SERVER_GENERALCHAT, SendBuffer);
    }

	strText = sc::string::format(
        ID2SERVERTEXT("ALLIANCE_BATTLE_RESULT"),
        pTarClub->Name(), 
        pTarClub->m_dwAllianceBattleWin,
        pTarClub->m_dwAllianceBattleLose,
        pTarClub->m_dwAllianceBattleDraw);

    if (SendData.SETTEXT(strText))
    {
        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, SendData);
        m_pServer->SENDTOALLCLIENT(NET_MSG_SERVER_GENERALCHAT, SendBuffer);
    }

	strText = sc::string::format(
        ID2SERVERTEXT("ALLIANCE_BATTLE_RESULT"),
        pMyClub->Name(), 
        pMyClub->m_dwAllianceBattleWin,
        pMyClub->m_dwAllianceBattleLose,
        pMyClub->m_dwAllianceBattleDraw);

    if (SendData.SETTEXT(strText))
    {
        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, SendData);
        m_pServer->SENDTOALLCLIENT(NET_MSG_SERVER_GENERALCHAT, SendBuffer);
    }

	// Database save
	AddGameDbJob(
        db::DbActionPtr(
            new db::ClubBattleEnd(
                pTarClub->m_DbNum,
                pMyClub->m_DbNum, 
                GLCLUBBATTLE::CLUB_BATTLE_ARMISTICE,
                0,
                0,
                true)));

    // Club log/%1%���� %2%Ŭ���� ������ �����߽��ϴ�.
    std::string ClubLog1(
        sc::string::format(
            ID2GAMEINTEXT("CLUB_LOG_BATTLE3"),
            m_szName,
            pTarClub->Name()));
    SENDCLUBLOG(m_ClubDbNum, club::LOG_CLUB_BATTLE, ClubLog1);

    // Database save
    AddGameDbJob(
        db::DbActionPtr(
            new db::ClubBattleEnd(
                pMyClub->m_DbNum,
                pTarClub->m_DbNum,
                GLCLUBBATTLE::CLUB_BATTLE_ARMISTICE,
                0,
                0,
                true)));

    // Club log/%1%���� %2%Ŭ���� ������ �����߽��ϴ�.
    std::string ClubLog2(
        sc::string::format(
            ID2GAMEINTEXT("CLUB_LOG_BATTLE3"),
            pTarChar->GetChaName(),
            pMyClub->Name()));
    SENDCLUBLOG(pTarClub->DbNum(), club::LOG_CLUB_BATTLE, ClubLog2);

	return S_OK;
}

HRESULT GLCharAG::MsgClubBattleSubmissionReq(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_CLUB_BATTLE_SUBMISSION_REQ* pNetMsg = (GLMSG::SNET_CLUB_BATTLE_SUBMISSION_REQ*) nmg;
	GLMSG::SNET_CLUB_BATTLE_SUBMISSION_REQ_FB NetMsgFB;	
	
    // ����Ŭ�������� �׺� �ȵ�
	GLAGLandMan* pLandMan = m_pServer->GetLand(m_mapIDCurrent.getGaeaMapID());
	if (!pLandMan || pLandMan->IsClubBattleMap())
	{
		NetMsgFB.emFB = EMCLUB_BATTLE_SUBMISSION_FB_GUIDMAP;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}

	if (pLandMan->IsClubDeathMatchMap())
	{
		NetMsgFB.emFB = EMCLUB_BATTLE_SUBMISSION_FB_CDMMAP;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}	

	if (!IsValidClub())
	{
		NetMsgFB.emFB = EMCLUB_BATTLE_SUBMISSION_FB_NOTMASTER;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}

	std::tr1::shared_ptr<GLClubAgent> pMyClub = m_pServer->GetClub(m_ClubDbNum);
	if (!pMyClub)
	{
		NetMsgFB.emFB = EMCLUB_BATTLE_SUBMISSION_FB_FAIL;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}
    /*
	if (!pMyClub->IsMaster(m_CharDbNum))
	{
		NetMsgFB.emFB = EMCLUB_BATTLE_SUBMISSION_FB_NOTMASTER;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}
    */
    if (!pMyClub->HaveClubBattleAuthority(m_CharDbNum))
    {
        NetMsgFB.emFB = EMCLUB_BATTLE_SUBMISSION_FB_NO_AUTHORITY;
        m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
        return S_FALSE;
    }

	if (!pMyClub->IsBattle(pNetMsg->dwClubID))
	{
		NetMsgFB.emFB = EMCLUB_BATTLE_SUBMISSION_FB_NOBATTLE;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}

	if (!pMyClub->IsBattleStop(pNetMsg->dwClubID))
	{
		NetMsgFB.emFB = EMCLUB_BATTLE_SUBMISSION_FB_DISTIME;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}
	
	std::tr1::shared_ptr<GLClubAgent> pTarClub = m_pServer->GetClub(pNetMsg->dwClubID);
	if (!pTarClub)
	{
		NetMsgFB.emFB = EMCLUB_BATTLE_SUBMISSION_FB_FAIL;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}	
	
	NetMsgFB.emFB = EMCLUB_BATTLE_SUBMISSION_FB_OK;
	m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);

    pMyClub->m_tLastBattle = sc::time::GetCurrentTime();
	pMyClub->m_dwBattleLose++;
	pMyClub->DELBATTLECLUB(pTarClub->m_DbNum);

	pTarClub->m_tLastBattle = sc::time::GetCurrentTime();
	pTarClub->m_dwBattleWin++;
	pTarClub->DELBATTLECLUB(pMyClub->m_DbNum);

	// �ʵ忡 �˸�.
	GLMSG::SNET_CLUB_BATTLE_OVER_FLD NetMsgFld;
	NetMsgFld.emFB = EMCLUB_BATTLE_OVER_SUBMISSION;
    NetMsgFld.dwWinClubID = pTarClub->m_DbNum;
	NetMsgFld.dwLoseClubID = pMyClub->m_DbNum;
	m_pServer->SENDTOALLCHANNEL(&NetMsgFld);

	//	Ŭ���̾�Ʈ�鿡 �˸�. ( ���Ϳ��鿡�� �˸� )
	GLMSG::SNET_CLUB_BATTLE_OVER_CLT NetMsgClt;
	NetMsgClt.emFB = EMCLUB_BATTLE_OVER_SUBMISSION;	
	NetMsgClt.dwClubID = pTarClub->m_DbNum;	
	m_pServer->SENDTOCLUBCLIENT(pMyClub->m_DbNum, &NetMsgClt);

	NetMsgClt.emFB = EMCLUB_BATTLE_OVER_TARSUBMISSION;		
	NetMsgClt.dwClubID = pMyClub->m_DbNum;	
	m_pServer->SENDTOCLUBCLIENT(pTarClub->m_DbNum, &NetMsgClt);

    std::string strText = sc::string::format(ID2SERVERTEXT("CLUB_BATTLE_OVER_SUBMISSION"), pMyClub->Name(), pTarClub->Name());

	GLMSG::SNET_SERVER_GENERALCHAT SendData;
    if (SendData.SETTEXT(strText))
    {
        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, SendData);
        m_pServer->SENDTOALLCLIENT(NET_MSG_SERVER_GENERALCHAT, SendBuffer);
    }
	
	strText = sc::string::format(
        ID2SERVERTEXT("CLUB_BATTLE_RESULT"),
        pMyClub->Name(), 
        pMyClub->m_dwBattleWin,
        pMyClub->m_dwBattleLose,
        pMyClub->m_dwBattleDraw);

    if (SendData.SETTEXT(strText))
    {
        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, SendData);
        m_pServer->SENDTOALLCLIENT(NET_MSG_SERVER_GENERALCHAT, SendBuffer);
    }
	
	strText = sc::string::format(
        ID2SERVERTEXT("CLUB_BATTLE_RESULT"),
        pTarClub->Name(), 
        pTarClub->m_dwBattleWin,
        pTarClub->m_dwBattleLose,
        pTarClub->m_dwBattleDraw);

    if (SendData.SETTEXT(strText))
    {
        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, SendData);
        m_pServer->SENDTOALLCLIENT(NET_MSG_SERVER_GENERALCHAT, SendBuffer);
    }

	// Database save
	AddGameDbJob(
        db::DbActionPtr(
            new db::ClubBattleEnd(
                pMyClub->m_DbNum,
                pTarClub->m_DbNum, 
                GLCLUBBATTLE::CLUB_BATTLE_SUBMISSION,
                0,
                0)));

    // Database save
    AddGameDbJob(
        db::DbActionPtr(
            new db::ClubBattleEnd(
                pTarClub->m_DbNum,
                pMyClub->m_DbNum,
                GLCLUBBATTLE::CLUB_BATTLE_SUBMISSION_WIN,
                0,
                0)));

    // Club log/Lost/
    std::string ClubLog1(
        sc::string::format(
            ID2GAMEINTEXT("CLUB_LOG_BATTLE5"),
            pTarClub->Name()));
    SENDCLUBLOG(pMyClub->DbNum(), club::LOG_CLUB_BATTLE, ClubLog1);
    
    // Club log/Win/%1% Ŭ���� �¸��Ͽ����ϴ�.
    std::string ClubLog2(
        sc::string::format(
            ID2GAMEINTEXT("CLUB_LOG_BATTLE4"),
            pMyClub->Name()));
    SENDCLUBLOG(pTarClub->DbNum(), club::LOG_CLUB_BATTLE, ClubLog2);

	return S_OK;
}

HRESULT GLCharAG::MsgAllianceBattleSubmissionReq(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_ALLIANCE_BATTLE_SUBMISSION_REQ* pNetMsg = (GLMSG::SNET_ALLIANCE_BATTLE_SUBMISSION_REQ*) nmg;
	GLMSG::SNET_ALLIANCE_BATTLE_SUBMISSION_REQ_FB NetMsgFB;
	
	// ����Ŭ�������� �׺� �ȵ�
	GLAGLandMan* pLandMan = m_pServer->GetLand(m_mapIDCurrent.getGaeaMapID());
	if (pLandMan->IsClubBattleMap())
	{
		NetMsgFB.emFB = EMALLIANCE_BATTLE_SUBMISSION_FB_GUIDMAP;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}

	if (pLandMan->IsClubDeathMatchMap())
	{
		NetMsgFB.emFB = EMALLIANCE_BATTLE_SUBMISSION_FB_CDMMAP;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}

	if (!IsValidClub())
	{
		NetMsgFB.emFB = EMALLIANCE_BATTLE_SUBMISSION_FB_NOTMASTER;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}

	std::tr1::shared_ptr<GLClubAgent> pMyClub = m_pServer->GetClub(m_ClubDbNum);
	if (!pMyClub)
	{
		NetMsgFB.emFB = EMALLIANCE_BATTLE_SUBMISSION_FB_FAIL;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}
/*
	if (!pMyClub->IsMaster(m_CharDbNum))
	{
		NetMsgFB.emFB = EMALLIANCE_BATTLE_SUBMISSION_FB_NOTMASTER;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}
*/
    if (!pMyClub->HaveClubBattleAuthority(m_CharDbNum))
    {
        NetMsgFB.emFB = EMALLIANCE_BATTLE_SUBMISSION_FB_NO_AUTHORITY;
        m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
        return S_FALSE;
    }

	if (!pMyClub->IsAlliance() || !pMyClub->IsChief())
	{
		NetMsgFB.emFB = EMALLIANCE_BATTLE_SUBMISSION_FB_NOTMASTER;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}

	if (!pMyClub->IsBattleAlliance(pNetMsg->dwClubID))
	{
		NetMsgFB.emFB = EMALLIANCE_BATTLE_SUBMISSION_FB_NOBATTLE;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}

	if (!pMyClub->IsBattleStop(pNetMsg->dwClubID))
	{
		NetMsgFB.emFB = EMALLIANCE_BATTLE_SUBMISSION_FB_DISTIME;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}
	
	std::tr1::shared_ptr<GLClubAgent> pTarClub = m_pServer->GetClub(pNetMsg->dwClubID);
	if (!pTarClub)
	{
		NetMsgFB.emFB = EMALLIANCE_BATTLE_SUBMISSION_FB_FAIL;
		m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
		return S_FALSE;
	}	

	NetMsgFB.emFB = EMALLIANCE_BATTLE_SUBMISSION_FB_OK;
	m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);

	CLUB_ALLIANCE_ITER pos = pMyClub->m_setAlliance.begin();
	CLUB_ALLIANCE_ITER end = pMyClub->m_setAlliance.end();
	for ( ; pos!=end; ++pos)
	{

		const GLCLUBALLIANCE &sALLIANCE = pos->second;
		std::tr1::shared_ptr<GLClubAgent> pCLUB = m_pServer->GetClub ( sALLIANCE.m_dwID );
		if (!pCLUB)
            continue;

		pCLUB->m_tLastBattle = CTime::GetCurrentTime().GetTime();
		pCLUB->m_dwAllianceBattleLose++;
		pCLUB->DELBATTLECLUB( pTarClub->m_DbNum );

	}

	pos = pTarClub->m_setAlliance.begin();
	end = pTarClub->m_setAlliance.end();
	for ( ; pos!=end; ++pos )
	{

		const GLCLUBALLIANCE &sALLIANCE = pos->second;
		std::tr1::shared_ptr<GLClubAgent> pCLUB = m_pServer->GetClub ( sALLIANCE.m_dwID );
		if ( !pCLUB )
            continue;

		pCLUB->m_tLastBattle = CTime::GetCurrentTime().GetTime();
		pCLUB->m_dwAllianceBattleWin++;
		pCLUB->DELBATTLECLUB( pMyClub->m_DbNum );
	}

	//	�ʵ忡 �˸�.
	GLMSG::SNET_CLUB_BATTLE_OVER_FLD NetMsgFld;
	NetMsgFld.emFB = EMCLUB_BATTLE_OVER_SUBMISSION;
    NetMsgFld.dwWinClubID = pTarClub->m_DbNum;
	NetMsgFld.dwLoseClubID = pMyClub->m_DbNum;
	m_pServer->SENDTOALLCHANNEL ( &NetMsgFld );
	
    //	Ŭ���̾�Ʈ�鿡 �˸�. ( ���Ϳ��鿡�� �˸� )
	GLMSG::SNET_CLUB_BATTLE_OVER_CLT NetMsgClt;
	NetMsgClt.emFB = EMCLUB_BATTLE_OVER_SUBMISSION;	
	NetMsgClt.dwClubID = pTarClub->m_DbNum;	
	m_pServer->SENDTOALLIANCECLIENT ( pMyClub->m_DbNum, &NetMsgClt );

	NetMsgClt.emFB = EMCLUB_BATTLE_OVER_TARSUBMISSION;		
	NetMsgClt.dwClubID = pMyClub->m_DbNum;	
	m_pServer->SENDTOALLIANCECLIENT ( pTarClub->m_DbNum, &NetMsgClt );

    std::string strText(
        sc::string::format(
            ID2SERVERTEXT("ALLIANCE_BATTLE_OVER_SUBMISSION"),
            pMyClub->Name(),
            pTarClub->Name()));

	GLMSG::SNET_SERVER_GENERALCHAT SendData;
    if (SendData.SETTEXT(strText))
    {
        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, SendData);
        m_pServer->SENDTOALLCLIENT(NET_MSG_SERVER_GENERALCHAT, SendBuffer);
    }
	
	strText = sc::string::format(
        ID2SERVERTEXT("ALLIANCE_BATTLE_RESULT"),
        pMyClub->Name(), 
        pMyClub->m_dwAllianceBattleWin,
        pMyClub->m_dwAllianceBattleLose,
        pMyClub->m_dwAllianceBattleDraw);

    if (SendData.SETTEXT(strText))
    {
        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, SendData);
        m_pServer->SENDTOALLCLIENT(NET_MSG_SERVER_GENERALCHAT, SendBuffer);
    }
	
	strText = sc::string::format(
        ID2SERVERTEXT("ALLIANCE_BATTLE_RESULT"),
        pTarClub->Name(), 
        pTarClub->m_dwAllianceBattleWin,
        pTarClub->m_dwAllianceBattleLose,
        pTarClub->m_dwAllianceBattleDraw);

    if (SendData.SETTEXT(strText))
    {
        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, SendData);
        m_pServer->SENDTOALLCLIENT(NET_MSG_SERVER_GENERALCHAT, SendBuffer);
    }


	//	db�� ���� ( A����, B���� ���� )
	AddGameDbJob(
        db::DbActionPtr(
            new db::ClubBattleEnd(
                pMyClub->m_DbNum,
                pTarClub->m_DbNum, 
                GLCLUBBATTLE::CLUB_BATTLE_SUBMISSION,
                0,
                0,
                true)));

    AddGameDbJob(
        db::DbActionPtr(
            new db::ClubBattleEnd(
                pTarClub->m_DbNum,
                pMyClub->m_DbNum,
                GLCLUBBATTLE::CLUB_BATTLE_SUBMISSION_WIN,
                0,
                0,
                true)));

    // Club log/Lost/
    std::string ClubLog1(
        sc::string::format(
        ID2GAMEINTEXT("CLUB_LOG_BATTLE5"),
        pTarClub->Name()));
    SENDCLUBLOG(pMyClub->DbNum(), club::LOG_CLUB_BATTLE, ClubLog1);

    // Club log/Win/%1% Ŭ���� �¸��Ͽ����ϴ�.
    std::string ClubLog2(
        sc::string::format(
        ID2GAMEINTEXT("CLUB_LOG_BATTLE4"),
        pMyClub->Name()));
    SENDCLUBLOG(pTarClub->DbNum(), club::LOG_CLUB_BATTLE, ClubLog2);

	return S_OK;
}


HRESULT GLCharAG::MsgClubId2MasterNameReqCA(NET_MSG_GENERIC* nmg)
{
	NET_MSG_PACK* pPacket = (NET_MSG_PACK*) nmg;

	GLMSG::SNET_CLUB_ID_2MASTERNAME_REQ_CA RecvData;
	if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
		return S_FALSE;    


	GLMSG::SNET_CLUB_ID_2MASTERNAME_ANS_AC SendData;

	BOOST_FOREACH(DWORD dwClubId, RecvData.m_vecClubId)
	{
		std::tr1::shared_ptr<GLClubAgent> pClub = m_pServer->GetClub(dwClubId);
		if (pClub)
		{
			SendData.ADDCLUBNAME(pClub->m_szName);
			SendData.ADDMASTERNAME(pClub->m_szMasterName);
		}
	}

	msgpack::sbuffer SendBuffer;
	msgpack::pack(SendBuffer, SendData);
	m_pServer->SENDTOCLIENT(m_ClientSlot,NET_MSG_CLUB_ID_2MASTERNAME_ANS_AC, SendBuffer);
	return S_OK;
}

bool GLCharAG::CheckSelectClubCondition()
{
    if ( GLUseFeatures::GetInstance().IsUsingEssentialSelectClub() )
    {
        if ( GetClubDbNum() == 0 )
            return true;
        else
            return false;
    }
    else
    {
        if ( GetChaLevel() == 1 && GetClubDbNum() == 0 )
            return true;
        else
            return false;
    }

    return false;
}