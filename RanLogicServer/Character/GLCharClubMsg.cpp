#include "../pch.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringUtils.h"
#include "../../SigmaCore/String/StringFormat.h"

#include "../../MfcExLib/RanFilter.h"

#include "../../EngineLib/DxTools/RENDERPARAM.h"

#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/GLogicDataMan.h"

#include "../Club/GLClubServer.h"
#include "../Database/DBAction/DbActionGameClub.h"
#include "../Party/GLPartyField.h"
#include "../FieldServer/GLGaeaServer.h"
#include "../Guidance/GLGuidance.h"
#include "../Util/GLItemLimit.h"
#include "./GLChar.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

HRESULT GLChar::MsgClubNew(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_CLUB_NEW* pNetMsg = (GLMSG::SNET_CLUB_NEW*) nmg;

	CString strClubName(pNetMsg->szClubName)
		, strSaveClubName(sc::string::utf8_to_ansi(std::string(pNetMsg->szClubName)).c_str());

	GLMSG::NET_CLUB_NEW_FB_FC NetMsgFb(false);
	NetMsgFb.SetClubName(strSaveClubName.GetString());

	BOOL bFILTER0 = STRUTIL::CheckString(strClubName);
	BOOL bFILTER1 = CRanFilter::GetInstance().NameFilter(strClubName);
	if (bFILTER0 || bFILTER1)
	{
		NetMsgFb.emFB = EMCLUB_NEW_FB_BADNAME;
		SendToClient(&NetMsgFb);
		return S_FALSE;
	}

	// RM #5876
	const INT nCmpCnt(RENDERPARAM::emLangSet == language::LANGFLAG_TAIWAN ? club::CLUBNAME_LIMIT_TAIWAN : club::CLUBNAME_LIMIT );
	if ( strClubName.GetLength() == 0 || strClubName.GetLength() > nCmpCnt )
	{
		NetMsgFb.emFB = EMCLUB_NEW_FB_BADNAME;
		SendToClient(&NetMsgFb);
		return S_FALSE;
	}

	// �±��� ���� ���� üũ		
    if (RENDERPARAM::emLangSet == language::THAILAND)
    {
	    if (!STRUTIL::CheckThaiString(strClubName))
	    {
		    NetMsgFb.emFB = EMCLUB_NEW_FB_THAICHAR_ERROR;
		    SendToClient(&NetMsgFb);
		    return S_FALSE;
	    }
    }

	// ��Ʈ�� ���� ���� üũ 
    if (RENDERPARAM::emLangSet == language::VIETNAM)
    {
	    if (STRUTIL::CheckVietnamString(strClubName))
	    {
		    NetMsgFb.emFB = EMCLUB_NEW_FB_VNCHAR_ERROR;
		    SendToClient(&NetMsgFb);
		    return E_FAIL;
	    }
    }

	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
	{
		NetMsgFb.emFB = EMCLUB_NEW_FB_FAIL;
		SendToClient(&NetMsgFb);
		return S_FALSE;
	}

	GLCrow* pNpc = pLand->GetCrow(pNetMsg->dwNpcGlobID);
	if (!pNpc)
	{
		NetMsgFb.emFB = EMCLUB_NEW_FB_FAIL;
		SendToClient(&NetMsgFb);
		return S_FALSE;
	}

	if (!pNpc->IsBasicTalk(SNpcTalk::EM_CLUB_NEW))
	{
		NetMsgFb.emFB = EMCLUB_NEW_FB_FAIL;
		SendToClient(&NetMsgFb);
		return S_FALSE;
	}

	if (m_ClubDbNum != CLUB_NULL)
	{
		NetMsgFb.emFB = EMCLUB_NEW_FB_ALREADY;
		SendToClient(&NetMsgFb);
		return S_FALSE;
	}

	GLPartyField* const pParty = GetMyParty();
	if ( !pParty || !pParty->isMasterByGaeaID(m_dwGaeaID) )
	{
		NetMsgFb.emFB = EMCLUB_NEW_FB_NOTMASTER;
		SendToClient(&NetMsgFb);
		return S_FALSE;
	}

	if (pParty->GetNMember() < GLCONST_CHAR::dwCLUB_PARTYNUM)
	{
		NetMsgFb.emFB = EMCLUB_NEW_FB_NOTMEMBER;
		SendToClient(&NetMsgFb);
		return S_FALSE;
	}
		
	PartyMemberGaeaIDs arPartyMemberGaeaIDs;
	const DWORD nMember = pParty->GetPartyMemberGaeaIDs(arPartyMemberGaeaIDs);
	for ( DWORD _i = 0; _i < nMember; ++_i )
	{
		GLChar* pPartyMember = GetChar(arPartyMemberGaeaIDs[_i]);
		if (!pPartyMember)
		{
			NetMsgFb.emFB = EMCLUB_NEW_FB_NOTEXIST_MEM;
			SendToClient(&NetMsgFb);
			return S_FALSE;
		}

		if (pPartyMember->m_ClubDbNum != CLUB_NULL)
		{
			NetMsgFb.emFB = EMCLUB_NEW_FB_MEM_ALREADY;
			SendToClient(&NetMsgFb );
			return S_FALSE;
		}

		// �ٸ� �п����� Ŭ�� �Ἲ ���� �ɼ��� ���� ��� ������.
		if (!GLCONST_CHAR::bCLUB_2OTHERSCHOOL)
		{
			if (pPartyMember->m_wSchool != m_wSchool)
			{
				NetMsgFb.emFB = EMCLUB_NEW_FB_OTHERSCHOOL;
				SendToClient(&NetMsgFb);
				return S_FALSE;
			}
		}
	}

	if (m_wLevel < GLogicData::GetInstance().GetClubRankData(0).m_dwMasterLvl)
	{
		NetMsgFb.emFB = EMCLUB_NEW_FB_MASTER_LVL;
		SendToClient(&NetMsgFb);
		return S_FALSE;
	}

	if (m_nLiving < int(GLogicData::GetInstance().GetClubRankData(0).m_dwLivingPoint))
	{
		NetMsgFb.emFB = EMCLUB_NEW_FB_AVER_LIVING;
		SendToClient(&NetMsgFb);
		return S_FALSE;
	}

	if (GetInvenMoney() < GLogicData::GetInstance().GetClubRankData(0).m_dwPay)
	{
		NetMsgFb.emFB = EMCLUB_NEW_FB_NOTMONEY;
		SendToClient(&NetMsgFb);
		return S_FALSE;
	}	

	// Agent ���� Ŭ�� ������ ��û��
	GLMSG::SNET_CLUB_NEW_2AGT NetMsg2Agt;
	NetMsg2Agt.dwChaNum = CharDbNum();
	NetMsg2Agt.SetClubName(strSaveClubName.GetString());
	SendToAgent(&NetMsg2Agt);

	return S_OK;
}

// *****************************************************
// Desc: Ŭ������ ��û�� ���� ��� ó��
// *****************************************************
HRESULT GLChar::MsgClubNew2FLD ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_CLUB_NEW_2FLD *pNetMsg = (GLMSG::SNET_CLUB_NEW_2FLD *) nmg;

	LONGLONG lnPAY = GLogicData::GetInstance().GetClubRankData(0).m_dwPay;

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgClubNew2FLD", m_CharDbNum ) );
	}

	//	Note : ���� �����ÿ� ���� ��� ����.
	if ( pNetMsg->emFB==EMCLUB_NEW_FB_OK )
	{
		CheckMoneyUpdate( GetInvenMoney(), lnPAY, FALSE, "Club Create Pay." );

		if ( GetInvenMoney() >= lnPAY )
		{
			SubtractInvenMoneyServer( lnPAY, SUBTRACT_INVEN_MONEY_CLUB_CREATE );
		}
		else
		{
			// ��ȹ �ǵ��ΰ�?
			SetInvenMoney( 0 );
		}
	}

	// �ݾ� ��ȭ �α�
	// ��� �� �α� ����
	LogMoneyExchange(gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CLUB, 0, -lnPAY, ITEM_ROUTE_DELETE, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CREATE_CLUB_CHARGE);
	// ��� �� �α� ����
	LogMoneyExchange(gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0, GetInvenMoney(), MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CHAR_INVEN_CURRENT);

	// �ݾ� ��ȭ �˸�.
	SendInvenMoneyUpdate();

	//	Note : ���� ��û ��� ��ȯ.
	//
	GLMSG::NET_CLUB_NEW_FB_FC NetMsgFb(false);
	NetMsgFb.emFB = pNetMsg->emFB;
	StringCchCopy ( NetMsgFb.szClubName, CHAR_SZNAME, pNetMsg->szClubName );
	SendToClient(&NetMsgFb );

	//	Note : Ŭ�� ���� �α� ���.
	// ��� �� �α� ����
	LogCharAction(
		m_CharDbNum,					//	�����.
		gamelog::ACT_CLUB_MAKE,			//	����.
		gamelog::ID_CLUB, pNetMsg->dwCLUBID,	//	����.
		0,							//	exp
		0,							//	bright
		0,							//	life
		(int)-lnPAY,					//	money
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap()
	);

	return S_OK;
}
/*
HRESULT GLChar::MsgClubMemberReq ( NET_MSG_GENERIC* nmg )
{
	GLMSG::NET_CLUB_MEMBER_REQ_CA *pNetMsg = (GLMSG::NET_CLUB_MEMBER_REQ_CA *) nmg;

	GLMSG::SNET_CLUB_MEMBER_REQ_FB NetMsgFB(false);

	GLChar* pTarChar = GetChar(pNetMsg->m_TarGaeaID);
	if ( !pTarChar )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_FAIL;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	NetMsgFB.SetTarChaName(pTarChar->m_szName);

    //! CTF �������̸� ���
    if ( pTarChar->IsCTFPlayer() )
    {
        NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_FAIL;
        SendToClient( &NetMsgFB );
        return S_FALSE;
    }

	// �ٸ� �п����� Ŭ�� �Ἲ ���� �ɼ��� ���� ��� ������.
	if ( !GLCONST_CHAR::bCLUB_2OTHERSCHOOL )
	{
		if ( pTarChar->m_wSchool!=m_wSchool )
		{
			NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_OTHERSCHOOL;
			SendToClient( &NetMsgFB );
			return S_FALSE;
		}
	}

    // ����üũ
    if (m_Country != pTarChar->GetCountry())
    {
        NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_NOT_SAME_COUNTRY;
        SendToClient( &NetMsgFB );
        return S_FALSE;
    }
	
	std::tr1::shared_ptr<GLClubField> pClub = GetMyClub();
	if (!pClub)
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_FAIL;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	if ( pClub->GetAllBattleNum() > 0 )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_CLUBBATTLE;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	if ( !pClub->HaveMemberJoinAuthority(m_CharDbNum) )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_FAIL;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	bool bFOUND = pClub->IsMember(pTarChar->m_CharDbNum);
	if ( bFOUND )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_ALREADY;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	const GLCLUBRANK &sRANK_DATA = GLogicData::GetInstance().GetClubRankData(pClub->m_dwRank);
	if ( pClub->GetMemberNum() >= sRANK_DATA.m_dwMember )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_MAXNUM;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}	
    if ( pTarChar->m_ClubDbNum!=CLUB_NULL )
    {
        NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_FAIL;
        SendToClient( &NetMsgFB );
        return S_FALSE;
    }


	GLMSG::SNET_CLUB_MEMBER_REQ_ASK NetMsgAsk;
	NetMsgAsk.SetClubName(pClub->Name());
	NetMsgAsk.dwMaster = m_CharDbNum;
	NetMsgAsk.SetMasterName(pClub->GetMasterName());

	pTarChar->SendToClient(&NetMsgAsk);
	
	return S_OK;
}
*/
/*
HRESULT GLChar::MsgClubMemberAns(NET_MSG_GENERIC* nmg)
{
	GASSERT(m_pGLGaeaServer->IsField());

	GLMSG::SNET_CLUB_MEMBER_REQ_ANS *pNetMsg = (GLMSG::SNET_CLUB_MEMBER_REQ_ANS *) nmg;

	GLChar* pMaster = GetCharByDbNum(pNetMsg->dwMaster);
	if (!pMaster)
		return S_FALSE;

	GLMSG::SNET_CLUB_MEMBER_REQ_FB NetMsgFB(false);
	NetMsgFB.SetTarChaName(m_szName);

	if (m_ClubDbNum != CLUB_NULL)
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_FAIL;
		pMaster->SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	if ( !pNetMsg->bOK )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_REFUSE;
		pMaster->SendToClient( &NetMsgFB );
		return S_FALSE;
	}
	
	std::tr1::shared_ptr<GLClubField> pCLUB = GetClub(pMaster->m_ClubDbNum);
	if ( !pCLUB )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_FAIL;
		pMaster->SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	if ( pCLUB->GetAllBattleNum() > 0 )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_CLUBBATTLE;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	if ( !pCLUB->HaveMemberJoinAuthority(pMaster->m_CharDbNum) )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_FAIL;
		pMaster->SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	bool bFOUND = pCLUB->IsMember(m_CharDbNum);
	if ( bFOUND )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_ALREADY;
		pMaster->SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	const GLCLUBRANK &sRANK_DATA = GLogicData::GetInstance().GetClubRankData(pCLUB->m_dwRank);
	if ( pCLUB->GetMemberNum() >= sRANK_DATA.m_dwMember )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_MAXNUM;
		pMaster->SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	GLMSG::SNET_CLUB_MEMBER_REQ_2AGT NetMsgAgt;
	NetMsgAgt.dwChaNum = pMaster->CharDbNum();
	NetMsgAgt.dwReqGID = m_dwGaeaID;
	SendToAgent(&NetMsgAgt);

	return S_OK;
}
*/
HRESULT GLChar::MsgClubRank(NET_MSG_GENERIC* nmg)
{
	GLMSG::NET_CLUB_RANK_UP_CF *pNetMsg = (GLMSG::NET_CLUB_RANK_UP_CF *) nmg;

	GLMSG::SNET_CLUB_RANK_FB NetMsgFB;

	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
	{
		NetMsgFB.emFB = EMCLUB_RANK_FB_FAIL;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	GLCrow* pCROW = pLand->GetCrow( pNetMsg->dwNpcGlobID );
	if ( !pCROW )
	{
		NetMsgFB.emFB = EMCLUB_RANK_FB_FAIL;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	if (!pCROW->IsBasicTalk(SNpcTalk::EM_CLUB_UP))
	{
		NetMsgFB.emFB = EMCLUB_RANK_FB_FAIL;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	if ( m_ClubDbNum==CLUB_NULL )
	{
		NetMsgFB.emFB = EMCLUB_RANK_FB_FAIL;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}
	
	std::tr1::shared_ptr<GLClubField> pCLUB = GetMyClub();
	if ( !pCLUB )
	{
		NetMsgFB.emFB = EMCLUB_RANK_FB_FAIL;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}
	/*
	if ( pCLUB->MasterCharDbNum() != m_CharDbNum )
	{
		NetMsgFB.emFB = EMCLUB_RANK_FB_FAIL;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}
    */
    if (!pCLUB->HaveRankUpAuthority(m_CharDbNum))
        return S_FALSE;

	if ( pCLUB->m_dwRank >= (GLCONST_CHAR::dwMAX_CLUBRANK-1) )
	{
		NetMsgFB.emFB = EMCLUB_RANK_FB_MAX;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	DWORD dwRANK = pCLUB->m_dwRank + 1;

	if (pCLUB->GetMasterLevel() < static_cast<int> (GLogicData::GetInstance().GetClubRankData(dwRANK).m_dwMasterLvl))
	{
		NetMsgFB.emFB = EMCLUB_RANK_FB_MASTER_LVL;
		NetMsgFB.dwValue = GLogicData::GetInstance().GetClubRankData(dwRANK).m_dwMasterLvl;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}
/*
    // Master �� Living point �� �� ����� ����
	if ( m_nLiving < int ( GLogicData::GetInstance().GetClubRankData(dwRANK).m_dwLivingPoint ) )
	{
		NetMsgFB.emFB = EMCLUB_RANK_FB_AVER_LIVING;
		NetMsgFB.dwValue = GLogicData::GetInstance().GetClubRankData(dwRANK).m_dwLivingPoint;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}
*/
	if ( GetInvenMoney() < GLogicData::GetInstance().GetClubRankData(dwRANK).m_dwPay )
	{
		NetMsgFB.emFB = EMCLUB_RANK_FB_NOTMONEY;
		NetMsgFB.dwValue = GLogicData::GetInstance().GetClubRankData(dwRANK).m_dwPay;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	// ��ũ ��ȭ ������Ʈ�� ��û.
	GLMSG::NET_CLUB_RANK_CHANGE_FA NetMsgAgt;
	NetMsgAgt.dwChaNum = CharDbNum();
	SendToAgent(&NetMsgAgt);

	return S_OK;
}

HRESULT GLChar::MsgClubMemberNick(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_CLUB_MEMBER_NICK* pNetMsg = (GLMSG::SNET_CLUB_MEMBER_NICK*) nmg;

    char szNick[CHAR_SZNAME] = {0};
    StringCchCopy(szNick, CHAR_SZNAME, pNetMsg->szNick);

	GLMSG::SNET_CLUB_MEMBER_NICK_FB NetMsgFB;

	CString strGuNick(szNick);

    if (m_ServiceProvider == SP_WORLD_BATTLE)
    {
        std::string ClubNick(szNick);
        if (!sc::string::checkEnglishOrNumber(ClubNick))
        {
            NetMsgFB.emFB = EMCLUB_MEMBER_NICK_FB_WB_ERROR;
            SendToClient( &NetMsgFB );
            return S_FALSE;
        }
    }

    if (m_ServiceProvider == SP_KOREA || m_ServiceProvider == SP_KOREA_TEST)
    {
        std::string ClubNick(szNick);
        if (!sc::string::goodChaNameKorea(ClubNick))
        {
            NetMsgFB.emFB = EMCLUB_MEMBER_NICK_FB_KR_ERROR;
            SendToClient( &NetMsgFB );
            return S_FALSE;
        }
    }

	BOOL bFILTER0 = STRUTIL::CheckString(strGuNick);
	BOOL bFILTER1 = CRanFilter::GetInstance().NameFilter(strGuNick);
	if (bFILTER0 || bFILTER1)
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_NICK_FB_BADNAME;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	// �±��� ���� ���� üũ 
    if (RENDERPARAM::emLangSet == language::THAILAND)
    {
	    if (!STRUTIL::CheckThaiString(strGuNick))
	    {
		    NetMsgFB.emFB = EMCLUB_MEMBER_NICK_FB_THAICHAR_ERROR;
		    SendToClient( &NetMsgFB );
		    return S_FALSE;
	    }
    }

	// ��Ʈ�� ���� ���� üũ 
    if (RENDERPARAM::emLangSet == language::VIETNAM)
    {
	    if (STRUTIL::CheckVietnamString(strGuNick))
	    {
		    NetMsgFB.emFB = EMCLUB_MEMBER_NICK_FB_VNCHAR_ERROR;
		    SendToClient( &NetMsgFB );
		    return E_FAIL;
	    }
    }

	std::tr1::shared_ptr<GLClubField> pCLUB = GetMyClub();
	if (!pCLUB)
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_NICK_FB_FAIL;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	std::string strConvNick(szNick);
	if ( m_ServiceProvider == SP_GS )//
		strConvNick = sc::string::utf8_to_ansi(strConvNick);

	SetNickName(strConvNick.c_str());

	AddGameDbJob(
		db::DbActionPtr(
		new db::ClubNickUpdate(m_CharDbNum, strConvNick)));

	GLMSG::SNET_CLUB_INFO_NICK_BRD NetMsgBrd;
	NetMsgBrd.dwGaeaID = m_dwGaeaID;
	StringCchCopy(NetMsgBrd.szNickName, CHAR_SZNAME, strConvNick.c_str());
	SendMsgViewAround(&NetMsgBrd);

	NetMsgFB.emFB = EMCLUB_MEMBER_NICK_FB_OK;
	StringCchCopy(NetMsgFB.szNick, CHAR_SZNAME, strConvNick.c_str());
	SendToClient( &NetMsgFB );

	return S_OK;
}

HRESULT GLChar::MsgClubCDCertify ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_CLUB_CD_CERTIFY *pNetMsg = (GLMSG::SNET_CLUB_CD_CERTIFY *)nmg;

	GLMSG::SNET_CLUB_CD_CERTIFY_FB NetMsgFB;

	GLMSG::SNET_CLUB_CD_CERTIFY_BRD  NetMsgBrd;

	//DxConsoleMsg* pConsoleMsg = m_pGLGaeaServer->GetConsoleMsg();

	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return S_FALSE;

	if ( m_ClubDbNum==CLUB_NULL )
	{
		NetMsgFB.emFB = EMCDCERTIFY_NOTMASTER;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	std::tr1::shared_ptr<GLClubField> pCLUB = GetMyClub();
	if ( !pCLUB || !pCLUB->IsMemberFlgCDCertify(m_CharDbNum) )
	{
		NetMsgFB.emFB = EMCDCERTIFY_NOTMASTER;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	// �����Ϸ��� Ŭ���� ���Ϳ� ���ԵǾ� ���� ��� ���� Ŭ���߿� ����Ŭ���� ������ ���� �Ұ�.
	if ( pCLUB->IsAlliance() )
	{
		// ���ͱ�� ������ ������ �常 �����ִ°� ����... ;;;
		std::tr1::shared_ptr<GLClubField> pCHIEFCLUB = GetClub(pCLUB->m_dwAlliance);		
		DWORD dwGuidID = 0;

		StringCchCopy ( NetMsgBrd.szAlliance, CHAR_SZNAME, pCHIEFCLUB->Name() );

		CLUB_ALLIANCE_ITER pos = pCHIEFCLUB->m_setAlliance.begin();
		CLUB_ALLIANCE_ITER end = pCHIEFCLUB->m_setAlliance.end();
		for ( ; pos!=end; ++pos )
		{
			const GLCLUBALLIANCE &sALLIANCE = pos->second;
			if ( sALLIANCE.m_dwID == pCLUB->m_DbNum ) continue;

			std::tr1::shared_ptr<GLClubField> pALLIANCECLUB = GetClub(sALLIANCE.m_dwID);
			if ( pALLIANCECLUB )
			{
				dwGuidID = m_pGaeaServer->GuidanceId ( pALLIANCECLUB->m_DbNum );
				if ( dwGuidID != UINT_MAX ) 
				{
					NetMsgFB.emFB = EMCDCERTIFY_HAVEGUIDCLUB;
					SendToClient( &NetMsgFB );
					return S_FALSE;
				}
			}
		}
	}

	PGLCROW pCROW = pLand->GetCrow(pNetMsg->dwNpcGlobID);
	if ( !pCROW )
	{
		NetMsgFB.emFB = EMCDCERTIFY_BADNPC;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	if (!pCROW->IsBasicTalk(SNpcTalk::EM_CD_CERTIFY))
	{
		NetMsgFB.emFB = EMCDCERTIFY_BADNPC;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	DWORD dwGUID = m_pGaeaServer->GuidanceId ( m_ClubDbNum );
	if (dwGUID != UINT_MAX)
	{
		if (dwGUID==pLand->m_dwClubMapID)
            NetMsgFB.emFB = EMCDCERTIFY_AREADY;
		else
            NetMsgFB.emFB = EMCDCERTIFY_AREADYGUID;

		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	EMCHECKCERTIFY emCERTIFY = m_pGaeaServer->GuidanceCheckCertify(pLand->m_dwClubMapID, m_CharDbNum);
	if ( emCERTIFY!=EMCHECKCERTIFY_OK )
	{
		NetMsgFB.emFB = EMCDCERTIFY_NOTCONDITION;

		switch ( emCERTIFY )
		{
		case EMCHECKCERTIFY_FAIL:
			NetMsgFB.emFB = EMCDCERTIFY_NOTCONDITION;
			break;
		case EMCHECKCERTIFY_DOING:
			NetMsgFB.emFB = EMCDCERTIFY_AREADYCERTIFY;
			break;
		case EMCHECKCERTIFY_OTHERDOING:
			NetMsgFB.emFB = EMCDCERTIFY_OTHERCERTIFING;
			break;
		case EMCHECKCERTIFY_NOTBATTLE:
			NetMsgFB.emFB = EMCDCERTIFY_NOTTIME;
			break;
		};

		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	D3DXVECTOR3 vDist = pCROW->GetPosition() - m_vPosition;
	float fDist = D3DXVec3Length ( &vDist );
	if ( fDist > GLCONST_CHAR::fCDCERTIFY_DIST )
	{
		NetMsgFB.emFB = EMCDCERTIFY_DISTANCE;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	SINVENITEM *pINVENITEM = m_cInventory.FindItem ( ITEM_CD );
	if ( !pINVENITEM )
	{
		NetMsgFB.emFB = EMCDCERTIFY_NOTITEM;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	WORD wPosX = pINVENITEM->wPosX;
	WORD wPosY = pINVENITEM->wPosY;

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{		
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgClubCDCertify", m_CharDbNum ) );
	}

	//	������ ���� �α�    
	// ��� �� �α� ����
	LogItemExchange(
        pINVENITEM->sItemCustom,
        gamelog::ID_CHAR, m_CharDbNum,
        gamelog::ID_CLUB, 0,
        ITEM_ROUTE_DELETE, pINVENITEM->sItemCustom.wTurnNum, 
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DEL_ITEM_BY_GUIDANCE_CERTIFICATION,
		false);

	// �κ����� ������ ����
    InvenDeleteItem( wPosX, wPosY, true, FALSE, 1, true );

	//	Note : ���� ����.
    m_pGaeaServer->GuidanceDoCertify(pLand->m_dwClubMapID, m_CharDbNum, pNetMsg->dwNpcGlobID, m_vPosition);

	//	Note : ���� ������ �˸� FB.
	NetMsgFB.emFB = EMCDCERTIFY_OK;
	SendToClient( &NetMsgFB );

	//	Note : ���� ������ �˸� BRD.
	std::string strZone = m_pGaeaServer->GuidanceGetName(pLand->m_dwClubMapID);
	StringCchCopy ( NetMsgBrd.szZone, GLMSG::SNET_CLUB_CD_CERTIFY_BRD::TEXT_LEN, strZone.c_str() );
	StringCchCopy ( NetMsgBrd.szName, CHAR_SZNAME, m_szName );
	StringCchCopy ( NetMsgBrd.szClub, CHAR_SZNAME, pCLUB->Name() );
	
	SendToAgent(&NetMsgBrd);

	//  [10/17/2013 gbgim];
	// ���� ���۽� ������ Ȱ��ȭ�Ǿ��ִٸ� ������ Ǯ�����Ѵ�;
	// GLHIDESET::CheckHideSet(EMHIDE_SPEEDUP_MAX) + ������Ͽ��� ����������� ������� ����
	// ���������Ҷ� ��ƿ� üũ�Ǿ��մ������� ����;
	// GLogicServer Folder - Script - HideSet.Lua;
	if ( GLHIDESET::CheckHideSet( EMHIDE_GUIDANCE_CERTIFY_HIDECANCLE ) )
	{
		RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
		RestoreActStateByInvisible();
	}

	return S_OK;
}
/*
HRESULT GLChar::MsgReqClubGetStorage ( NET_MSG_GENERIC* nmg )
{
	GLMSG::NET_CLUB_GETSTORAGE_CA *pNetMsg = (GLMSG::NET_CLUB_GETSTORAGE_CA *) nmg;

	std::tr1::shared_ptr<GLClubField> pCLUB = GetMyClub();
	if ( !pCLUB )
	{
		return S_OK;
	}

	if ( pCLUB->MasterCharDbNum() != m_CharDbNum )
	{
		return S_OK;
	}

	if ( !pCLUB->m_bVALID_STORAGE )
	{
		AddGameDbJob(
            db::DbActionPtr(
                new db::ClubStorageGet(m_ClientSlot,m_CharDbNum,m_ClubDbNum)));	
	}
	else
	{
		MsgClubStorage();
	}
	return S_OK;
}
*/
HRESULT GLChar::MsgReqClubStorageToInven( DWORD ClubDbNum, const SITEMCUSTOM& Item, WORD PosX, WORD PosY )
{
	// �������� �ִµ� �������� �˷����� �ʰ�, ��� ������Ʈ ���� �ִ´�.
    bool bOk = InvenInsertItem( Item, PosX, PosY, false, 1, false );
	if ( !bOk )
    {
        // ������ ����
        GLMSG::NET_CLUB_STORAGE_RESTORE_FA MsgFA( m_CharDbNum, ClubDbNum, Item );
        msgpack::sbuffer SendBuffer;
        msgpack::pack( SendBuffer, MsgFA );
        SendToAgent( NET_MSG_CLUB_STORAGE_RESTORE_FA, SendBuffer, true );
		return S_FALSE;
    }

	SINVENITEM* pInsertedItem = m_cInventory.GetItem( PosX, PosY );
	if ( !pInsertedItem )
	{
		// �̰��� �߻����� �ʴµ� �߻��� ��� �̹� ���� ������ �����ϰ� ������ ���� �޽��� ������.
		// ������Ʈ������ Ŭ����Ŀ���� �������� �޸𸮿����� �����ϰ� ��񿡼��� �����ϱ� �ʾҴ�.
		// �������� �޸𸮿��� ���� ��Ȳ
		sc::writeLogError( sc::string::format( "critical error MsgReqClubStorageToInven GetItem chanum: %1%", m_CharDbNum ) );

		// ������ ����
		InvenDeleteItem( PosX, PosY, false, FALSE, 1, false );

		// ������ ����
		GLMSG::NET_CLUB_STORAGE_RESTORE_FA MsgFA( m_CharDbNum, ClubDbNum, Item );
		msgpack::sbuffer SendBuffer;
		msgpack::pack( SendBuffer, MsgFA );
		SendToAgent( NET_MSG_CLUB_STORAGE_RESTORE_FA, SendBuffer, true );

		return S_FALSE;
	}

	// ������ ���� �˸�
	InvenInsertSend( pInsertedItem );

	// ��� ������Ʈ, ������ ��ü ������Ʈ�� �ƴ� ���� ��ġ ���游 �Ѵ�.
	InvenUpdateItem( pInsertedItem, true );

	// �������� ���� ���� ��� ���
	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{		
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqClubStorageToInven", m_CharDbNum ) );
	}

	// ��� �� �α� ����
	LogItemExchange( 
		Item,
		gamelog::ID_CLUB, ClubDbNum,
		gamelog::ID_CHAR, m_CharDbNum,
		ITEM_ROUTE_CHAR, Item.wTurnNum, 
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_CLUB_STORAGE_MOVE_CLUB_TO_CHAR,
		false );

	// RESET_XXX ���� �켱 CheckItemDuration �� ��ü�ϰ� ���� ������ �߻��ϸ� RESET_XXX �ΰ� �ٽ� ����. �������� ��� ���� �� �˻��ϰ�, 
	// ����â ���� �ִ� �����۱��� �˻��ϰ� �ִ�. �켱 ���� �ִ���� ���д�. RESET_TIMELMT_ITEM �̰� 3�� ���� ������ �Ǿ� �ְ�, GetViewAround ���� �Ѵ�.
	// �ű⿡ �ش���� ������ �Ҹ� üũ�� �ȵǾ �̷��� �ϴ� �� ����. RESET_TIMELMT_ITEM �̰� Ȯ���ؼ� ����� ���� �� �ֵ��� �Ѵ�.
	// �Ⱓ�� ����� �������� �Ҹ�.
	//CheckItemDuration( *pInsertedItem );
	RESET_TIMELMT_ITEM();
	RESET_CHECK_ITEM();

	return S_OK;
}

bool GLChar::IsKEEP_CLUB_STORAGE ( DWORD dwCHANNEL )
{
	std::tr1::shared_ptr<GLClubField> pCLUB = GetMyClub();
	if (!pCLUB)
        return false;
	if (pCLUB->MasterCharDbNum() != m_CharDbNum)
        return false;
	if (dwCHANNEL >= MAX_CLUBSTORAGE)
        return false;
	if (pCLUB->m_dwRank < dwCHANNEL)
        return false;
	return true;
}

HRESULT GLChar::MsgReqClubStorageExInven( DWORD ClubDbNum, const SITEMCUSTOM& ClubItem, WORD Channel, WORD ClubStoragePosX, WORD ClubStoragePosY, WORD InvenPosX, WORD InvenPosY )
{
	// ������ �ʴ´�.
	return S_OK;

	/*
    SINVENITEM* pInvenItem = m_cInventory.GetItem( InvenPosX, InvenPosY );

	if (!pInvenItem)
    {
        // ������ ����
        GLMSG::NET_CLUB_STORAGE_RESTORE_FA MsgFA(m_CharDbNum, ClubDbNum, ClubItem);
        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, MsgFA);
        SendToAgent(NET_MSG_CLUB_STORAGE_RESTORE_FA, SendBuffer, true);
		return S_FALSE;
    }

	const SITEM* pInvenItemData = GLogicData::GetInstance().GetItem(pInvenItem->Id());
	if (!pInvenItemData)
    {
        // ������ ����
        GLMSG::NET_CLUB_STORAGE_RESTORE_FA MsgFA(m_CharDbNum, ClubDbNum, ClubItem);
        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, MsgFA);
        SendToAgent(NET_MSG_CLUB_STORAGE_RESTORE_FA, SendBuffer, true);
        return S_FALSE;
    }
	
	//	�ŷ��ɼ�
	//if (!pInvenItemData->sBasicOp.IsEXCHANGE())
	if ( !( pInvenItem->sItemCustom.IsExchange() != NULL ) )
    {
        // ������ ����
        GLMSG::NET_CLUB_STORAGE_RESTORE_FA MsgFA(m_CharDbNum, ClubDbNum, ClubItem);
        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, MsgFA);
        SendToAgent(NET_MSG_CLUB_STORAGE_RESTORE_FA, SendBuffer, true);
        return S_FALSE;
    }
	
	// �κ� ������ ���
    SITEMCUSTOM sInvenItem = pInvenItem->CustomItem(); 
	if (m_ServiceProvider == SP_VIETNAM)
	{
		if (sInvenItem.bVietnamGainItem)
        {
            // ������ ����
            GLMSG::NET_CLUB_STORAGE_RESTORE_FA MsgFA(m_CharDbNum, ClubDbNum, ClubItem);
            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, MsgFA);
            SendToAgent(NET_MSG_CLUB_STORAGE_RESTORE_FA, SendBuffer, true);
			return S_FALSE;
        }
	}
	
	const SITEM* pClubItemData = GLogicData::GetInstance().GetItem(ClubItem.GetNativeID());	
	if (!pClubItemData)
    {
        // ������ ����
        GLMSG::NET_CLUB_STORAGE_RESTORE_FA MsgFA(m_CharDbNum, ClubDbNum, ClubItem);
        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, MsgFA);
        SendToAgent(NET_MSG_CLUB_STORAGE_RESTORE_FA, SendBuffer, true);
        return S_FALSE;
    }

    SINVENITEM sInven_BackUp = *pInvenItem;	//	�κ� ������ ���.
    WORD wInvenPosX = sInven_BackUp.wPosX;		//	�κ� ������ ��ġ.
    WORD wInvenPosY = sInven_BackUp.wPosY;
    	
	// �κ� ������ Ŭ��â���� ����
    GLMSG::NET_CLUB_STORAGE_EX_INVEN_FA MsgInsert(ClubDbNum, m_CharDbNum, sInvenItem, Channel, ClubStoragePosX, ClubStoragePosY);
    msgpack::sbuffer SendBufferFA;
    msgpack::pack(SendBufferFA, MsgInsert);
    SendToAgent(NET_MSG_CLUB_STORAGE_EX_INVEN_FA, SendBufferFA, true);

    // �κ� ó��.
    {
        //	Inventory�� �ִ� ������ ����.
        BOOL bOk = InvenDeleteItem(sInven_BackUp.wPosX, sInven_BackUp.wPosY, false, FALSE);

        // ���� �տ��� �����۰� ����� �������� ũ�Ⱑ Ʋ�� ���.
        //	��� �ִ� �������� �� ������ ã�´�.
        //	if ( !pInvenData->SAME_INVENSIZE ( *pHoldData ) )
        //	{
        //	������ġ�� ������ �ִ��� �˻�.
        bOk = m_cInventory.IsInsertable( wInvenPosX, wInvenPosY );
        if ( !bOk )
        {
            //	����ִ� ������ Inventory�� ���� ������ �ִ��� �˻�.
            bOk = m_cInventory.FindInsertable( wInvenPosX, wInvenPosY );
            if ( !bOk )
            {
                //	roll-back : Inventory���� �����ߴ� �������� �ٽ� ����.
                m_cInventory.InsertItem( sInven_BackUp.sItemCustom, sInven_BackUp.wPosX, sInven_BackUp.wPosY, true );

                return S_FALSE;
            }
        }
        //}

        //	Ŭ�� ������ Inventory�� ����.
        m_cInventory.InsertItem( ClubItem, wInvenPosX, wInvenPosY );
        SINVENITEM* pINSERT_IVEN_ITEM = m_cInventory.GetItem( wInvenPosX, wInvenPosY );
        if ( !pINSERT_IVEN_ITEM )
        {
            sc::writeLogError(sc::string::format("error MsgReqClubStorageExInven(), ClubItem, id(%d,%d) to pos(%d,%d)",
                ClubItem.GetNativeID().wMainID, ClubItem.GetNativeID().wSubID, wInvenPosX, wInvenPosY ));

            return S_FALSE;
        }

        GLMSG::SNETPC_INVEN_DEL_AND_INSERT NetMsg_Inven_Del_Insert;
        NetMsg_Inven_Del_Insert.wDelX = sInven_BackUp.wPosX;					//	������ ������.
        NetMsg_Inven_Del_Insert.wDelY = sInven_BackUp.wPosY;
        if (pINSERT_IVEN_ITEM)
            NetMsg_Inven_Del_Insert.sInsert = *pINSERT_IVEN_ITEM;	//	���ԵǴ� ������.

        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, NetMsg_Inven_Del_Insert);
        SendToClient(NET_MSG_GCTRL_INVEN_DEL_INSERT, SendBuffer);

        //	��ȿ�Ⱓ �ʰ� ����.	
        CheckItemDuration( *pINSERT_IVEN_ITEM );
    }

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{		
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqClubStorageExInven", m_CharDbNum ) );
	}

	// �������� ���� ���� ��� ���
	// ��� �� �α� ����
	LogItemExchange(
        sInvenItem,
        gamelog::ID_CHAR, m_CharDbNum,
        gamelog::ID_CLUB, m_ClubDbNum,
        ITEM_ROUTE_CLUB, sInvenItem.wTurnNum, 
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_CLUB_STORAGE_EXCHANGE_CHAR_TO_CLUB,
		false);	

	// �������� ���� ���� ��� ���
	// ��� �� �α� ����
	LogItemExchange(
        ClubItem,
        gamelog::ID_CLUB, m_ClubDbNum,
        gamelog::ID_CHAR, m_CharDbNum,
        ITEM_ROUTE_CHAR, ClubItem.wTurnNum, 
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_CLUB_STORAGE_EXCHANGE_CLUB_TO_CHAR,
		false);	
	
	return S_OK;
	*/
}

bool GLChar::MsgReqClubInvenToStorage( DWORD Channel, WORD PosX, WORD PosY, WORD HoldPosX, WORD HoldPosY)
{	
	std::tr1::shared_ptr< GLClubField > spClub = GetMyClub();
	if ( !spClub )
		return false;

	if ( Channel >= MAX_CLUBSTORAGE )
		return false;
	
    SINVENITEM* pInvenItem = m_cInventory.GetItem( HoldPosX, HoldPosY );
	if ( !pInvenItem )
		return false;

	// pInvenItem ������ ��� ���°� update ���¶�� �����ϰ� ����ȴ�.(none �̳� insert ���¶��? - �̷� ���� ���ٰ� ���� �Ѵ�.)
	// �κ��� ���� �� �̹� insert �ǰų�, �̹� insert �� �������̶�� update ���� �� �����̴�. 
	if ( db::DB_UPDATE != pInvenItem->GetDbState() )
	{
		InvenUpdateItem( pInvenItem );
	}

	//	�ŷ��ɼ�
	if ( !( pInvenItem->sItemCustom.IsExchange() != NULL ) )
        return false;

	// #3989 GmCharEdit ���� �ŷ��Ұ��� �־��� �������� Ŭ��â���� ���� �� �ִ� ���� ����
	if ( pInvenItem->sItemCustom.IsGM_GENITEM() )
		return false;

    if ( !spClub->HaveStorageAuthority( m_CharDbNum ) )
        return false;

// 	if ( m_ServiceProvider == SP_VIETNAM )
// 	{
// 		if ( pInvenItem->CustomItem().bVietnamGainItem )
// 			return false;
// 	}

    // ������ ���
    SITEMCUSTOM HoldItem = pInvenItem->CustomItem();

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{		
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqClubInvenToStorage", m_CharDbNum ) );
	}

	// ������ ����, ��������Ʈ�� ���� �ʴ´�. ���� ������Ʈ�������� Ŭ����Ŀ�� �κ�Ÿ�� �� ��ǥ�� ����ȴ�.
	if ( !InvenDeleteItem( HoldPosX, HoldPosY, true, HoldItem.bVietnamGainItem, 1, false ) )
	{
		return false;
	}

	// �������� ���� ���� ��� ���
	// ��� �� �α� ����
	LogItemExchange( 
        HoldItem, 
        gamelog::ID_CHAR, m_CharDbNum, 
        gamelog::ID_CLUB, spClub->DbNum(), 
        ITEM_ROUTE_CLUB, HoldItem.wTurnNum, 
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_CLUB_STORAGE_ADD_CHAR_TO_CLUB,
		false );

    // Ŭ��â���� ���� �޽���
    // ������ �����ϸ� �ٽ� �տ� ���� �Ѵ�.
    GLMSG::NET_CLUB_INVEN_TO_STORAGE_FA SendData( m_ClubDbNum, m_CharDbNum, Channel, PosX, PosY, HoldItem );
    msgpack::sbuffer SendBuffer;
    msgpack::pack( SendBuffer, SendData );
    SendToAgent( NET_MSG_CLUB_INVEN_TO_STORAGE_FA, SendBuffer, true );
	return S_OK;
}

//bool GLChar::HoldItemRestore(const SITEMCUSTOM& Item)
//{
//    // �̹� �������� ��� �ִ�. �κ��丮�� �־��ش�.    
//    // �����ζ� �־�� �Ѵ�.
//    if (InvenInsertItem(Item, true, true))
//        return true;
//    else
//        return false;
//}
/*
HRESULT GLChar::MsgReqClubStorageSplit ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_CLUB_STORAGE_SPLIT *pNetMsg = (GLMSG::SNET_CLUB_STORAGE_SPLIT *)nmg;
	
	std::tr1::shared_ptr<GLClubField> pCLUB = GetMyClub();
	if ( !pCLUB )
	{
		return S_OK;
	}

	if ( pCLUB->MasterCharDbNum() != m_CharDbNum )
	{
		return S_OK;
	}

	if ( pNetMsg->dwChannel >= MAX_CLUBSTORAGE )
	{
		return E_FAIL;
	}

	if ( !IsKEEP_CLUB_STORAGE(pNetMsg->dwChannel) )
	{
		return S_FALSE;
	}

	BOOL bOk(FALSE);

	GLInventory &sInventory = pCLUB->m_cStorage[pNetMsg->dwChannel];

	//	Note : �и� ��û ������ ã��.
	SINVENITEM* pInvenItem = sInventory.FindPosItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem )									return E_FAIL;

	//	Note : ������ ���� ��������.
	const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.sNativeID );
	if ( !pItem )
        return E_FAIL;

	//	Note : �и� ������ ���������� ����.
	bool bSPLIT(false);
	bSPLIT = ( pItem->ISPILE() );
	if ( !bSPLIT )													return E_FAIL;
	if ( pNetMsg->wSplit < 1 )										return E_FAIL;
	if ( pInvenItem->sItemCustom.wTurnNum <= pNetMsg->wSplit  )		return E_FAIL;

	//	Note : ���� �������� ��ȭ�� ����.
	WORD wORGIN_SPLIT = pInvenItem->sItemCustom.wTurnNum - pNetMsg->wSplit;

	//	Note : �и��Ǿ� ���� ����� ������.
	SITEMCUSTOM sNEW_ITEM = pInvenItem->sItemCustom;
	sNEW_ITEM.wTurnNum = pNetMsg->wSplit;

	//	Note : ���� ����� ������ ��ġ ã��.
	WORD wInvenPosX(0), wInvenPosY(0);
	bOk = sInventory.FindInsertable( wInvenPosX, wInvenPosY );
	if ( !bOk )	return E_FAIL;

	//	Note : ���� ���� ������ �ֱ�.
	bOk = sInventory.InsertItem( sNEW_ITEM, wInvenPosX, wInvenPosY );
	if ( !bOk )											return E_FAIL;

	//	Note : ���� ���� ������ ���� ��������.
	SINVENITEM* pNewItem = sInventory.GetItem( wInvenPosX, wInvenPosY );
	if ( !pNewItem )									return E_FAIL;

	//	Note : ���� �������� ���� ���� ��Ŵ.
	pInvenItem->sItemCustom.wTurnNum = wORGIN_SPLIT;

	//	Note : �κ��� ���� �и��Ǿ� ��� ���� ������.
	GLMSG::SNET_CLUB_STORAGE_INSERT NetMsgNew;
	NetMsgNew.dwChannel = pNetMsg->dwChannel;
	NetMsgNew.Data = *pNewItem;
	SendToClient(&NetMsgNew);

	//	Note : ���� ������ ���� ����.
	GLMSG::SNET_CLUB_STORAGE_UPDATE_ITEM NetMsgStorage;
	NetMsgStorage.dwChannel = pNetMsg->dwChannel;
	NetMsgStorage.wPosX = pNetMsg->wPosX;
	NetMsgStorage.wPosY = pNetMsg->wPosY;
	NetMsgStorage.wTurnNum = wORGIN_SPLIT;
	SendToClient( &NetMsgStorage );

	return S_OK;
}
*/

HRESULT GLChar::MsgReqClubStorageSaveMoney( NET_MSG_GENERIC* nmg )
{
    NET_MSG_PACK* pPacket = ( NET_MSG_PACK* ) nmg;
    GLMSG::NET_CLUB_STORAGE_SAVE_MONEY_CF RecvData;
    if ( !msgpack::BufferToObject( pPacket->DataBuffer(), pPacket->DataSize(), RecvData ) )
        return S_FALSE;

    if ( RecvData.m_MagicNum != GLMSG::NET_CLUB_STORAGE_SAVE_MONEY_CF::MAGIC_NUM )
    {
        sc::writeLogError( 
            sc::string::format( 
                "HACKING! MsgReqClubStorageSaveMoney ChaDbNum %1% Money %2% MagicNum %3%", 
                m_CharDbNum, RecvData.m_DepositMoney, RecvData.m_MagicNum ) );
        return S_FALSE;
    }

	std::tr1::shared_ptr< GLClubField > spClub = GetMyClub();
	if ( !spClub )
        return S_FALSE;

    if ( !spClub->HaveMoneyAuthority( m_CharDbNum ) )
        return S_FALSE;

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqClubStorageSaveMoney", m_CharDbNum ) );
	}

	if ( RecvData.m_DepositMoney <= 0 || GetInvenMoney() < RecvData.m_DepositMoney )
    {
        sc::writeLogError( 
            sc::string::format( 
                "HACKING! MsgReqClubStorageSaveMoney %1% < %2% ChaDbNum %3%", 
                GetInvenMoney(), 
                RecvData.m_DepositMoney, 
                m_CharDbNum ) );
        return S_FALSE;
    }

	CheckMoneyUpdate( GetInvenMoney(), RecvData.m_DepositMoney, FALSE, "Club Storage Save Money." );

	SubtractInvenMoneyServer( RecvData.m_DepositMoney, SUBTRACT_INVEN_MONEY_CLUB_STORAGE_SAVE );
	
	// �ݾ� �α�.
	// ��� �� �α� ����
	LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CLUB, m_ClubDbNum, RecvData.m_DepositMoney, ITEM_ROUTE_CLUB, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CLUB_STORAGE_SAVE );
	// ��� �� �α� ����
	LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CLUB,           0, GetInvenMoney(), MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CHAR_INVEN_CURRENT );
	
	// Ŭ���̾�Ʈ�� �� �׼� ��ȭ�� �˷���
	SendInvenMoneyUpdate();

    // Agent �� �Աݿ�û
    GLMSG::NET_CLUB_STORAGE_SAVE_MONEY_FA SendData( m_CharDbNum, m_ClubDbNum, RecvData.m_DepositMoney );
    msgpack::sbuffer SendBuffer;
    msgpack::pack( SendBuffer, SendData );
    SendToAgent( NET_MSG_CLUB_STORAGE_SAVE_MONEY_FA, SendBuffer );

	return S_OK;
}

void GLChar::MsgReqClubStorageSaveMoneyAF(LONGLONG Money)
{	
    if ( Money <= 0 )
        return;

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqClubStorageSaveMoneyAF", m_CharDbNum ) );
	}

	AddInvenMoneyServer( Money, ADD_INVEN_MONEY_CLUB_STORAGE_SAVE_ROLLBACK );

    SendInvenMoneyUpdate();
	
	// ��� �� �α� ����
    LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CLUB, 0, GetInvenMoney(), MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CHAR_INVEN_CURRENT );
}

HRESULT GLChar::MsgReqClubStorageDrawMoney( DWORD ClubDbNum, LONGLONG Money )
{		
	LONGLONG lnMoney = Money;
	if ( lnMoney <= 0 )
    {
         sc::writeLogError( sc::string::format( "HACKING! MsgReqClubStorageDrawMoney %1% < 0 ChaDbNum %3%", lnMoney, m_CharDbNum ) );
        return S_FALSE;
    }

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{		
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqClubStorageDrawMoney", m_CharDbNum ) );
		return S_FALSE;
	}

	CheckMoneyUpdate( GetInvenMoney(), lnMoney, TRUE, "Club Storage Draw Money." );

	AddInvenMoneyServer( lnMoney, ADD_INVEN_MONEY_CLUB_STORAGE_DRAW );
	
	// �ݾ� �α�    
	// ��� �� �α� ����
    LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CLUB, 0, GetInvenMoney(), MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CHAR_INVEN_CURRENT );
	
	// Ŭ���̾�Ʈ�� �κ��丮�� �ִ� �� �׼� ��ȭ�� �˷���.
	SendInvenMoneyUpdate();

	return S_OK;
}
/*
HRESULT GLChar::MsgReqClubInComeReNew(NET_MSG_GENERIC* nmg)
{
	// GLMSG::SNET_CLUB_INCOME_RENEW

	if (m_ClubDbNum == CLUB_NULL)
		return S_OK;

	std::tr1::shared_ptr<GLClubField> pCLUB = GetMyClub();
	if (!pCLUB)
		return S_OK;

	if (pCLUB->MasterCharDbNum() != m_CharDbNum)
		return S_OK;

	GLMSG::SNET_CLUB_INCOME_UP NetMsgUp;
	NetMsgUp.dwClubID = pCLUB->m_DbNum;
	SendToAgent(&NetMsgUp);
	return S_OK;
}
*/