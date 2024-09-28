#include "pch.h"
#include "../SigmaCore/MsgPack/MinMsgPack.h"
#include "../SigmaCore/String/StringFormat.h"
#include "../SigmaCore/Json/MinJson.h"
#include "../SigmaCore/Util/DateTime.h"

#include "../EngineLib/DxTools/DxClubMan.h"
#include "../EngineLib/DxTools/RENDERPARAM.h"
#include "../EngineLib/GUInterface/GameTextControl.h"

#include "../MfcExLib/RanFilter.h"

#include "../RanLogic/Msg/ClubMsg.h"
#include "../RanLogic/Msg/JsonMsgDefine.h"
#include "../RanLogic/Club/ClubLogManClient.h"
#include "../RanLogic/Msg/GuidanceMsg.h"

#include "./Char/CharNameDbNumCache.h"
#include "./Land/GLLandManClient.h"
#include "./Stage/DxGlobalStage.h"
#include "./GLGaeaClient.h"

#include "../RanLogic/GLUseFeatures.h"
#include "./Notify/NotifyClientFactory.h"
#include "./Notify/NotifyClientMan.h"

#include "../../RanLogicClient/Competition/Competition.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void GLGaeaClient::MsgClubMarkInfoFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_CLUB_MARK_INFO_FB *pNetMsg = (GLMSG::SNET_CLUB_MARK_INFO_FB *)pMsg;

    //	Note : 이미지에 적용.
    DWORD dwServerID = GetCharacter()->m_dwServerID;
    DxClubMan::GetInstance().SetClubData(
        m_pGlobalStage->GetD3dDevice(),
        dwServerID,
        pNetMsg->dwClubID,
        pNetMsg->dwMarkVER,
        pNetMsg->aryMark);

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_Club_UpdateAlliance );

	// 이름 디스플레이 클럽 아이콘 변경
	GfxInterfaceBase::GetInstance()->ChangeClubIcon( pNetMsg->dwClubID );
}

void GLGaeaClient::MsgClubCdCertifyBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_CLUB_CD_CERTIFY_BRD *pNetMsg = (GLMSG::SNET_CLUB_CD_CERTIFY_BRD *)pMsg;

    if (strlen(pNetMsg->szAlliance) != 0)
    {				
        PrintConsoleTextDlg(
            sc::string::format(
            ID2GAMEINTEXT("CLUB_BATTLE_CERTIFY_START_EX"),
            pNetMsg->szZone,
            pNetMsg->szAlliance,
            pNetMsg->szClub,
            pNetMsg->szName));
    }
    else
    {				
        PrintConsoleTextDlg(
            sc::string::format(
            ID2GAMEINTEXT("CLUB_BATTLE_CERTIFY_START"),
            pNetMsg->szZone,
            pNetMsg->szClub,
            pNetMsg->szName));
    }
}


void GLGaeaClient::MsgClubCdCertyfingBrd(NET_MSG_GENERIC* pMsg)
{
    NET_MSG_PACK* pPacekt = (NET_MSG_PACK*) pMsg;
    GLMSG::NET_CLUB_CD_CERTIFY_ING_BRD Data;
    if (!msgpack::BufferToObject(pPacekt->DataBuffer(), pPacekt->DataSize(), Data))
        return;

    switch (Data.emFB)
    {
    case EMCDCERTIFY_ING_DIE:
        {
            if (!Data.m_AllianceName.empty())
            {						
                PrintConsoleTextDlg(
                    sc::string::format(
                    ID2GAMEINTEXT("EMCDCERTIFY_ING_DIE_EX_ALLI"),
                    Data.m_ZoneName.c_str(),
                    Data.m_AllianceName.c_str(),
                    Data.m_ClubName.c_str(),
                    Data.m_ChaName.c_str()));
            }
            else
            {						
                PrintConsoleTextDlg(
                    sc::string::format(
                    ID2GAMEINTEXT("EMCDCERTIFY_ING_DIE_EX"),
                    Data.m_ZoneName.c_str(),
                    Data.m_ClubName.c_str(),
                    Data.m_ChaName.c_str()));
            }
        }
        break;
    case EMCDCERTIFY_ING_TIMEOUT:
        {
            if (!Data.m_AllianceName.empty())
            {						
                PrintConsoleTextDlg(
                    sc::string::format(
                    ID2GAMEINTEXT("EMCDCERTIFY_ING_TIMEOUT_EX_ALLI"),
                    Data.m_ZoneName.c_str(),
                    Data.m_AllianceName.c_str(),
                    Data.m_ClubName.c_str(),
                    Data.m_ChaName.c_str()));
            }
            else
            {						
                PrintConsoleTextDlg(
                    sc::string::format(
                    ID2GAMEINTEXT("EMCDCERTIFY_ING_TIMEOUT_EX"),
                    Data.m_ZoneName.c_str(),
                    Data.m_ClubName.c_str(),
                    Data.m_ChaName.c_str()));
            }
        }
        break;
    case EMCDCERTIFY_ING_DISTANCE:
        {
            if (!Data.m_AllianceName.empty())
            {						
                PrintConsoleTextDlg(
                    sc::string::format(
                    ID2GAMEINTEXT("EMCDCERTIFY_ING_DISTANCE_EX_ALLI"),
                    Data.m_ZoneName.c_str(),
                    Data.m_AllianceName.c_str(),
                    Data.m_ClubName.c_str(),
                    Data.m_ChaName.c_str()));
            }
            else
            {						
                PrintConsoleTextDlg(
                    sc::string::format(
                    ID2GAMEINTEXT("EMCDCERTIFY_ING_DISTANCE_EX"),
                    Data.m_ZoneName.c_str(),
                    Data.m_ClubName.c_str(),
                    Data.m_ChaName.c_str()));
            }
        }
        break;
    }
}

void GLGaeaClient::MsgServerPkEventInfo(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_SERVER_PKEVENT_INFO *pNetMsg = (GLMSG::SNETPC_SERVER_PKEVENT_INFO *)pMsg;

    if ( m_bSCHOOL_FREEPK != pNetMsg->bSCHOOL_FREEPK )
    {
        if ( pNetMsg->bSCHOOL_FREEPK )
        {
            PrintConsoleTextDlg ( ID2GAMEINTEXT("OTHER_SCHOOL_FREEPK_ON") );
        }
        else
        {
            PrintConsoleTextDlg ( ID2GAMEINTEXT("OTHER_SCHOOL_FREEPK_OFF") );

            // 적대관계 모두 삭제
            GetCharacter()->DEL_PLAYHOSTILE_ALL();
        }

        m_bSCHOOL_FREEPK = pNetMsg->bSCHOOL_FREEPK;
    }

    m_bBRIGHTEVENT = pNetMsg->bBRIGHTEVENT;
    m_bPKServer = pNetMsg->bPKServer;
}

void GLGaeaClient::MsgClubCommissionBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::NET_CLUB_GUID_COMMISSION_BRD *pNetMsg = (GLMSG::NET_CLUB_GUID_COMMISSION_BRD *)pMsg;

    GLLandManClient* pLand = GetActiveMap();
    if (pLand)
    {
        pLand->m_fCommission = pNetMsg->fCommission;				
        PrintConsoleTextDlg(
            sc::string::format(ID2GAMEINTEXT("LAND_TRADE_NEW_COMMISSION"), pLand->m_fCommission));
    }
}

void GLGaeaClient::MsgClubCommissionRvBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_CLUB_GUID_COMMISSION_RESERVE_BRD *pNetMsg = (GLMSG::SNET_CLUB_GUID_COMMISSION_RESERVE_BRD *)pMsg;
    PrintConsoleTextDlg(
        sc::string::format(ID2GAMEINTEXT("LAND_TRADE_NEW_RV_COMMISSION"), pNetMsg->fCommission));
}
/*
void GLGaeaClient::MsgClubChaLevelChangeAC(NET_MSG_GENERIC* pMsg)
{
    NET_CLUB_CHAR_LEVEL_CHANGE_AC* pPacket = (NET_CLUB_CHAR_LEVEL_CHANGE_AC*) pMsg;
    m_pMyClub->SetMemberState(pPacket->m_ChaDbNum, pPacket->m_ChaLevel);
    // UI Refresh
    m_pInterface->REFRESH_CLUB_LIST();
    m_pInterface->REFRESH_CLUB_STATE();
}
*/
void GLGaeaClient::GetClubMember(
    std::vector<club::GLCLUBMEMBER_CLIENT>& vResult,
    const std::vector<club::ALIGN_ORDER>& Align,
    bool bOnline)
{
    m_pMyClub->GetList(vResult, Align, bOnline);
    for (size_t i=0; i<vResult.size(); ++i)
        vResult[i].SetMapName(GetMapNameStr(vResult[i].m_CurrentMap.getBaseMapID()));    
}

bool GLGaeaClient::ReqClubNoticeChange(const char* _Noice)
{
	if (!m_pMyClub->IsValid())
	{
		PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_NOTICE_FB_FAIL"));
		return false;
	}

	if (!m_pMyClub->IsMemberFlgNotice(m_pCharacter->CharDbNum()))
	{
		PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_NOTICE_FB_NOTMATER"));
		return false;
	}

	GLMSG::SNET_CLUB_NOTICE_REQ NetMsg(_Noice);
	NETSENDTOAGENT(&NetMsg);

	return true;
}

LuaTable GLGaeaClient::GetMemberInfoList( club::ALIGN_ORDER_TYPE emType,  club::ALIGN_ORDER_DES_ASC emDesc, bool bOnline )
{
	std::vector< club::ALIGN_ORDER > vecAlign;

	club::ALIGN_ORDER sAlignOrder( emType, emDesc );
	vecAlign.push_back( sAlignOrder );

	std::vector< club::GLCLUBMEMBER_CLIENT > vecMember;
	m_pMyClub->GetList( vecMember, vecAlign, false );

	LuaTable tbMemberInfo( GLWidgetScript::GetInstance().GetLuaState() );

	int nTableIndex = 1;
	for( size_t i = 0; i < vecMember.size(); ++i )
	{
		const club::GLCLUBMEMBER_CLIENT& sMember = vecMember[ i ];

		if( true == bOnline && false == sMember.m_Online )
			continue;

		LuaTable tb( GLWidgetScript::GetInstance().GetLuaState() );
		tb.set( 1, sMember.GetChaClass() );
		tb.set( 2, sMember.m_ChaLevel );
		tb.set( 3, sMember.GetName() );
		tb.set( 4, sMember.m_Channel );
		tb.set( 5, GetMapNameStr( sMember.m_CurrentMap.getBaseMapID() ).c_str() );
		tb.set( 6, sMember.m_Grade );
		tb.set( 7, sMember.m_ChaDbNum );
		tb.set( 8, sMember.m_Online );
		tbMemberInfo.set( nTableIndex++, tb );
	}

	return tbMemberInfo;
}

std::string GLGaeaClient::GetMyClubMasterGradeName()
{
	if ( m_pMyClub == NULL )
		return "";

	std::string strCDCertifier = "";
	DWORD dwCDCertifier(m_pMyClub->GetCdCertifier());
	if ( dwCDCertifier == m_pMyClub->MasterCharDbNum() )
		strCDCertifier = sc::string::format( "%1%", ID2GAMEWORD("WB_CLUB_BASIC_INFO_MASTER_MARK", 2) );

	std::string strMasterGradeName = "";
	strMasterGradeName = sc::string::format( "%1%%2%%3%",
		ID2GAMEWORD("WB_CLUB_BASIC_INFO_MASTER_MARK", 0), strCDCertifier, ID2GAMEWORD("WB_CLUB_BASIC_INFO_MASTER_MARK", 3) );

	return strMasterGradeName;
}

void GLGaeaClient::DelClubMember(const char* _Name)
{
	DWORD MemberDbNum = GetMemberDBNum(_Name);
	ReqClubMemberDel(MemberDbNum);	
}

void GLGaeaClient::ClubMasterChange(int nDBNum)
{
	ReqClubMasterChange(nDBNum);
}

void GLGaeaClient::ClubCDAuth( const char* _Name )
{
	DWORD MemberDbNum = GetMemberDBNum( _Name );
	ReqClubMemberCd( MemberDbNum, !ClubCDAuthCheck( MemberDbNum ) );
}

bool GLGaeaClient::ClubCDAuthCheck( DWORD dwDBNum )
{
	DWORD dwCdCertifier = GetMyClub()->GetCdCertifier();
	if( dwDBNum == dwCdCertifier )
		return true;
	else
		return false;
}

DWORD GLGaeaClient::GetMemberDBNum(const char* _Name)
{
	std::vector<club::GLCLUBMEMBER_CLIENT> vResult;	
	club::ALIGN_ORDER temp(club::ORDER_CHA_NAME);
	std::vector<club::ALIGN_ORDER> Align;

	m_pMyClub->GetList(vResult, Align, false);
	
	for (size_t i=0; i<vResult.size(); ++i)
	{
		if(!strcmp(_Name, vResult[i].GetName()))
			return vResult[i].GetChaDbNum();
	}

	return NATIVEID_NULL().dwID;
}

void GLGaeaClient::ReqAuthorityChange(lua_tinker::table _tbAuthData)
{
	size_t ClubGradeSize = GetClubGradeSize();
	size_t ClubAuthoSize = GetClubAuthDescSize();
	std::vector <club::GRADE_FLAG> vFlag;

	for(size_t i = 0; i < ClubGradeSize; ++i)
	{
		lua_tinker::table _tbGradeAuth = _tbAuthData.get<lua_tinker::table>(i + 1);
		DWORD dwGradeFlag = club::AUTHORITY_NONE;
		for(int j = 0; j < _tbGradeAuth.TableLen(); ++j)
		{
			if(_tbGradeAuth.get<bool>(j+1))
			{	
				dwGradeFlag |= GetFlagByAuthIndex(j);
			}
		}

		club::GRADE_FLAG Data(i, dwGradeFlag);
		vFlag.push_back(Data);
	}

	ClubGradeFlagChangeCA(vFlag);	
}

club::AUTHORITY GLGaeaClient::GetFlagByAuthIndex( size_t nAuthorityIndex )
{
	if ( nAuthorityIndex >= GetClubAuthDescSize() )
		return club::AUTHORITY_NONE;

	if ( nAuthorityIndex == 0 )
		return club::AUTHORITY_MEMBER_JOIN; //! 클럽에 가입 시킬 수 있는 권한
	if ( nAuthorityIndex == 1 )
		return club::AUTHORITY_MEMBER_KICK; //! 클럽에서 제명 시킬 수 있는 권한
	if ( nAuthorityIndex == 2 )
		return club::AUTHORITY_NOTICE_CHANGE; //! 클럽 공지를 작성할 수 있는 권한    
	if ( nAuthorityIndex == 3 )
		return club::AUTHORITY_MARK_CHANGE; //! 클럽 마크를 변경할 수 있는 권한
	if ( nAuthorityIndex == 4 )
		return club::AUTHORITY_DEATH_MATCH; //! CDM 참가 자격
	if ( nAuthorityIndex == 5 )
		return club::AUTHORITY_RANK_UP; //! 클럽 랭크업
	if ( nAuthorityIndex == 6 )
		return club::AUTHORITY_CHANGE; //! 클럽원 계급 편집/변경
	if ( nAuthorityIndex == 7 )
		return club::AUTHORITY_ITEM; //! 클럽 창고/아이템 넣기/꺼내기
	if ( nAuthorityIndex == 8 )
		return club::AUTHORITY_MONEY; //! 클럽 창고/입출금
	if ( nAuthorityIndex == 9 )
		return club::AUTHORITY_ALLIANCE; //! 클럽 동맹 신청하기/받기
	if ( nAuthorityIndex == 10 )
		return club::AUTHORITY_ALLIANCE_DIS; //! 클럽 동맹 해지
	if ( nAuthorityIndex == 11 )
		return club::AUTHORITY_CLUB_BATTLE; //! 클럽 배틀 신청 하기/받기/항복/휴전

	return club::AUTHORITY_NONE;
}

lua_tinker::table GLGaeaClient::GetClubAuthority()
{	
	size_t ClubAuthoSize = GetClubAuthDescSize();
	size_t ClubGradeSize = GetClubGradeSize();

	lua_tinker::table tbAuthList( GLWidgetScript::GetInstance().GetLuaState() );

	for ( size_t i = 0; i < ClubGradeSize; i++ )
	{		
		tbAuthList.set(i + 1, CheckAuthorities(i));
	}

	return tbAuthList;
}

lua_tinker::table GLGaeaClient::CheckAuthorities( size_t nGradeIndex )
{
	DWORD dwGradeFlag = ClubGradeFlagGet( nGradeIndex );
	lua_tinker::table tbAuth( GLWidgetScript::GetInstance().GetLuaState() );

	size_t ClubAuthoSize = GetClubAuthDescSize();
	for ( size_t nAuthorityIndex = 0; nAuthorityIndex < ClubAuthoSize; nAuthorityIndex++ )
	{
		if ( dwGradeFlag & GetFlagByAuthIndex(nAuthorityIndex) )
			tbAuth.set(nAuthorityIndex + 1, true);
		else
			tbAuth.set(nAuthorityIndex + 1, false);
	}

	return tbAuth;
}
LuaTable GLGaeaClient::GetDisolutiontime()
{
	CTime cTime;
	GetMyClub()->GetDissolution( cTime );

	LuaTable tb( GLWidgetScript::GetInstance().GetLuaState() );
	tb.set( 1, cTime.GetYear() );
	tb.set( 2, cTime.GetMonth() );
	tb.set( 3, cTime.GetDay() );
	tb.set( 4, cTime.GetHour() );
	tb.set( 5, cTime.GetMinute() );
	return tb;
}

void GLGaeaClient::GetClubMemberOnOffline(club::MEMBER_ON_OFFLINE& Result) const
{
    m_pMyClub->GetMemberOnOffline(Result);
}

bool GLGaeaClient::ReqClubMemberDel(DWORD MemberDbNum)
{    
    if (!m_pMyClub->IsValid())
        return false;

    if (!m_pMyClub->HaveMemberKickAuthority(m_pCharacter->CharDbNum()))
        return false;

    DWORD MyChaDbNum = m_pCharacter->CharDbNum();
    DWORD MasterChaDbNum = m_pMyClub->MasterCharDbNum();
    if (MasterChaDbNum == MemberDbNum)
        return false;

    const GLCLUBMEMBER* pDelMember = m_pMyClub->GetMember(MemberDbNum);
    if (!pDelMember)
        return false;

    const GLCLUBMEMBER* pMyCharMember = m_pMyClub->GetMember(MyChaDbNum);
    if (!pMyCharMember)
        return false;

    DWORD MyGrade  = pMyCharMember->GetGrade();
    DWORD DelGrade = pDelMember->GetGrade();

    // 클럽 마스터가 아니면 자기보다 높은 계급의 멤버를 탈퇴 시킬 수 없다.
    if (MasterChaDbNum != MyChaDbNum && MyGrade < DelGrade)
    {
        PrintMsgText(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("CLUB_MEMBER_DEL_GRADE_ERROR"));
        return false;
    }

    GLMSG::NET_CLUB_MEMBER_DEL_CA NetMsg;
    NetMsg.m_ChaDbNum = MemberDbNum;
    NETSENDTOAGENT(&NetMsg);
    return true;
}

void GLGaeaClient::ReqClubMemberCd(DWORD MemberDbNum, bool bCD)
{
    if (!m_pMyClub->IsMaster(m_pCharacter->CharDbNum()))
        return; // Club master 만 설정가능

    if (m_pMyClub->IsMaster(MemberDbNum))
        return; // 설정하려는 Club member가 master라면 설정불가

    GLCLUBMEMBER* pMember = m_pMyClub->GetMember(MemberDbNum);
    if (!pMember)
        return; // Club member 가 아님

    // CD 인증권한 회수가 가능한지 체크
    if (!bCD && pMember->m_ChaDbNum != m_pMyClub->GetCdCertifier())
        return;

    json_spirit::Object JsonObj;
    JsonObj.push_back(json_spirit::Pair("M", jm::MSG_CLUB_MEMBER_CD_CERTIFY_CA));
    JsonObj.push_back(json_spirit::Pair("C", static_cast<int> (MemberDbNum)));
    JsonObj.push_back(json_spirit::Pair("D", bCD));

    NET_JSON MsgCA;
    if (MsgCA.SetData(json_spirit::write(JsonObj)))
        NETSENDTOAGENT(&MsgCA);
}

bool GLGaeaClient::ReqClubMasterChange(DWORD MemberDbNum)
{
    if (!m_pMyClub->IsValid())
        return false;

    if (!m_pMyClub->IsMaster(m_pCharacter->CharDbNum()))
        return false;

    if (m_pMyClub->MasterCharDbNum() == MemberDbNum)
        return false;

    // 클럽원 확인
    const GLCLUBMEMBER* pMember = m_pMyClub->GetMember(MemberDbNum);
    if (!pMember)
        return false;

    //	접속 여부 확인
    if (!pMember->m_Online) 
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_AUTHORITY_REQ_FB_NOONLINE"));
        return false;
    }

    // 동맹 여부
    if (m_pMyClub->IsAlliance())
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_AUTHORITY_REQ_FB_ALLIANCE"));
        return false;
    }

    // 클럽배틀 여부
    if (m_pMyClub->GetAllBattleNum() > 0)
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_AUTHORITY_REQ_FB_CLUBBATTLE"));
        return false;
    }

    //	대련 여부
    if (m_pCharacter->IsConfting())
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_AUTHORITY_REQ_FB_CONFING"));
        return false;
    }

    GLMSG::SNET_CLUB_AUTHORITY_REQ NetMsg;
    NetMsg.dwMember = MemberDbNum;
    NETSENDTOAGENT(&NetMsg);

    return true;
}

void GLGaeaClient::MsgClubMasterChangeAC(NET_MSG_GENERIC* pMsg)
{
    GLMSG::NET_CLUB_MASTER_CHNAGE_AC* pPacket = (GLMSG::NET_CLUB_MASTER_CHNAGE_AC*) pMsg;
    m_pMyClub->SetMasterName(pPacket->m_ChaName);
    m_pMyClub->SetMasterCharDbNum(pPacket->m_NewMasterDbNum);
    m_pMyClub->SetMasterCharLevel(pPacket->m_NewMasterLevel);
    
    std::string strText(
        sc::string::format(
            ID2GAMEINTEXT("EMCLUB_AUTHORITY_REQ_FB_OK"),
            pPacket->m_ChaName,
            m_pMyClub->Name()));
    PrintMsgTextDlg(NS_UITEXTCOLOR::PALEGREEN, strText);

    // UI Refresh
	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_Club_UpdateMember );
}

void GLGaeaClient::MsgClubCdCertifierChangeAC(NET_MSG_GENERIC* pMsg)
{
    GLMSG::NET_CLUB_CD_CERTIFIER_CHANGE_AC* pPacekt = (GLMSG::NET_CLUB_CD_CERTIFIER_CHANGE_AC*) pMsg;    
    m_pMyClub->SetCdCertifier(pPacekt->m_NewCdCertifier);    
    GLCLUBMEMBER* pMember = m_pMyClub->GetMember(pPacekt->m_NewCdCertifier);
    if (pMember)
    {
        // %1%님이 CD인증권한을 가집니다.
        std::string Msg(
            sc::string::format(
                ID2GAMEINTEXT("CLUB_CD_CERTIFIER_CHANGE_AC"),
                pMember->GetName()));
        PrintMsgDlg(NS_UITEXTCOLOR::DEFAULT, Msg);
    }

    // UI Refresh
	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_Club_UpdateMember );
}

void GLGaeaClient::RequestClubJoin()
{
	ReqClubJoin(m_pCharacter->GetTarPtoP());
}

bool GLGaeaClient::ReqClubJoin(DWORD dwGaeaID)
{
	if ( GLUseFeatures::GetInstance().IsUsingWorldBattle() )
	{
		PrintMsgText( NS_UITEXTCOLOR::NEGATIVE,
			ID2GAMEINTEXT( "LIMITED_REQUEST_JOIN_CLUB_MESSAGE" ) );

		return false;
	}

    if (!m_pMyClub->IsValid())
        return false;

    DWORD MyChaDbNum = m_pCharacter->CharDbNum();
    // 권한 체크
    if (!m_pMyClub->HaveMemberJoinAuthority(MyChaDbNum))
        return false;

    if (m_pMyClub->GetAllBattleNum() > 0)
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_MEMBER_REQ_FB_CLUBBATTLE"));
        return false;
    }

    GLMSG::NET_CLUB_MEMBER_REQ_CA NetMsg(dwGaeaID);
    //NETSENDTOFIELD(&NetMsg);
	NETSENDTOAGENT(&NetMsg);
    return true;
}

bool GLGaeaClient::ReqClubJoinByCharname(const char* szCharName)
{
	if( NULL == szCharName )
		return false;

	if ( m_pLandMClient == NULL)
		return false;

	DWORD dwTargetGaeaID(-1);
	std::tr1::shared_ptr<GLCharClient> spChar = m_pLandMClient->FindChar(szCharName);
	if( NULL != spChar )
		dwTargetGaeaID = spChar->GetGaeaID();

	if ( GLUseFeatures::GetInstance().IsUsingWorldBattle() )
	{
		PrintMsgText( NS_UITEXTCOLOR::NEGATIVE,
			ID2GAMEINTEXT( "LIMITED_REQUEST_JOIN_CLUB_MESSAGE" ) );

		return false;
	}

	if (!m_pMyClub->IsValid())
		return false;

	DWORD MyChaDbNum = m_pCharacter->CharDbNum();
	// 권한 체크;
	if (!m_pMyClub->HaveMemberJoinAuthority(MyChaDbNum))
		return false;

	if (m_pMyClub->GetAllBattleNum() > 0)
	{
		PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_MEMBER_REQ_FB_CLUBBATTLE"));
		return false;
	}

	GLMSG::NET_CLUB_MEMBER_REQ_CA NetMsg(dwTargetGaeaID);
	strncpy_s( NetMsg.m_szChaName,sizeof(NetMsg.m_szChaName),szCharName, strlen(szCharName) );
	//NETSENDTOFIELD(&NetMsg);
	NETSENDTOAGENT(&NetMsg);
	return true;
}

void GLGaeaClient::JoinClubLobby(DWORD ClubDbNum, DWORD ChaDbNum, const std::string& ChaName, EMCHARCLASS ChaClass)
{
    GLMSG::NET_CLUB_MENBER_LOBBY_CA MsgCA(ClubDbNum, ChaDbNum, ChaName.c_str(), ChaClass);
    NETSENDTOAGENT(&MsgCA);
}

void GLGaeaClient::ClubMemberReqFB(NET_MSG_GENERIC* pMsg)
{
    GLMSG::NET_CLUB_MEMBER_REQ_FB_AC* pPacket = (GLMSG::NET_CLUB_MEMBER_REQ_FB_AC*) pMsg;

    bool bLobby = pPacket->m_bLobby;
    std::string strText;
    DWORD UiColor = NS_UITEXTCOLOR::DEFAULT;
    UIGUID UiID = OUTER_MODAL_JOIN_CLUB_FAIL;

    switch (pPacket->emFB)
    {
    case EMCLUB_MEMBER_REQ_FB_FAIL:
        strText = sc::string::format( ID2GAMEINTEXT("EMCLUB_MEMBER_REQ_FB_FAIL"), pPacket->szReqName ).c_str();
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
        break;

    case EMCLUB_MEMBER_REQ_FB_OK:
        strText = sc::string::format( ID2GAMEINTEXT("EMCLUB_MEMBER_REQ_FB_OK"), pPacket->szReqName ).c_str();
        UiColor = NS_UITEXTCOLOR::PALEGREEN;
        UiID = OUTER_MODAL_JOIN_CLUB_SUCCESS;
        break;

    case EMCLUB_MEMBER_REQ_FB_REFUSE:
        strText = sc::string::format( ID2GAMEINTEXT("EMCLUB_MEMBER_REQ_FB_REFUSE"), pPacket->szReqName ).c_str();
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
        break;

    case EMCLUB_MEMBER_REQ_FB_ALREADY:
        strText = sc::string::format( ID2GAMEINTEXT("EMCLUB_MEMBER_REQ_FB_ALREADY"), pPacket->szReqName ).c_str();
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
        break;

    case EMCLUB_MEMBER_REQ_FB_MAXNUM:
        strText = ID2GAMEINTEXT("EMCLUB_MEMBER_REQ_FB_MAXNUM"); 
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
        break;

    case EMCLUB_MEMBER_REQ_FB_SECEDED:
        strText = sc::string::format( ID2GAMEINTEXT("EMCLUB_MEMBER_REQ_FB_SECEDED"), GLCONST_CHAR::dwCLUB_JOINBLOCK_DAY ).c_str();
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
        break;

    case EMCLUB_MEMBER_REQ_FB_OTHERSCHOOL:
        strText = sc::string::format( ID2GAMEINTEXT("EMCLUB_MEMBER_REQ_FB_OTHERSCHOOL"), pPacket->szReqName ).c_str();
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
        break;

    case EMCLUB_MEMBER_REQ_FB_CLUBBATTLE:
        strText = ID2GAMEINTEXT("EMCLUB_MEMBER_REQ_FB_CLUBBATTLE");
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
        break;

    case EMCLUB_MEMBER_REQ_FB_NOT_SAME_COUNTRY:
        strText = ID2GAMEINTEXT("EMCLUB_MEMBER_REQ_FB_NOT_SAME_COUNTRY");
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
        break;

	case EMCLUB_MEMBER_REQ_FB_NONEXISTENT:
		strText = ID2GAMEINTEXT("EMCLUB_MEMBER_REQ_FB_NONEXISTENT");
		UiColor = NS_UITEXTCOLOR::NEGATIVE;
		break;

    default:
        strText = ID2GAMEINTEXT("EMCLUB_MEMBER_REQ_FB_FAIL");
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
        break;
    };

    if (bLobby)
        DoModalOutInterface(strText, UI::MODAL_NOTITLE, UI::OK, UiID );
    else
        PrintMsgTextDlg(UiColor, strText);
}

void GLGaeaClient::MakeNewClubLobby(DWORD ChaDbNum, const std::string& ClubName, const std::string& ChaName)
{
    GLMSG::NET_CLUB_NEW_LOBBY_CA MsgCA(ChaDbNum, ClubName, ChaName);
    NETSENDTOAGENT(&MsgCA);
}

void GLGaeaClient::MsgClubNewFB(NET_MSG_GENERIC* nmg)
{
    GLMSG::NET_CLUB_NEW_FB_FC* pNetMsg = (GLMSG::NET_CLUB_NEW_FB_FC*) nmg;

    bool bLobby = pNetMsg->m_bLobby;
    std::string strText;
    DWORD UiColor = NS_UITEXTCOLOR::DEFAULT;
    UIGUID UiID = OUTER_MODAL_MAKE_CLUB_FAIL;

    switch (pNetMsg->emFB)
    {
    case EMCLUB_NEW_FB_FAIL:
        strText = ID2GAMEINTEXT("EMCLUB_NEW_FB_FAIL");
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
        break;

    case EMCLUB_NEW_FB_OK:
        strText = sc::string::format(ID2GAMEINTEXT("EMCLUB_NEW_FB_OK"), pNetMsg->szClubName);            
        UiColor = NS_UITEXTCOLOR::PALEGREEN;
        UiID = OUTER_MODAL_MAKE_CLUB_SUCCESS;
        break;

    case EMCLUB_NEW_FB_ALREADY:
        strText = ID2GAMEINTEXT("EMCLUB_NEW_FB_ALREADY");
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
        break;

    case EMCLUB_NEW_FB_NOTMASTER:
        strText = ID2GAMEINTEXT("EMCLUB_NEW_FB_NOTMASTER");            
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
        break;

    case EMCLUB_NEW_FB_NOTMEMBER:
        strText = sc::string::format(ID2GAMEINTEXT("EMCLUB_NEW_FB_NOTMEMBER"), GLCONST_CHAR::dwCLUB_PARTYNUM);            
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
        break;

    case EMCLUB_NEW_FB_NOTEXIST_MEM:
        strText = sc::string::format(ID2GAMEINTEXT("EMCLUB_NEW_FB_NOTEXIST_MEM"), GLCONST_CHAR::dwCLUB_PARTYNUM);            
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
        break;

    case EMCLUB_NEW_FB_MEM_ALREADY:
        strText = ID2GAMEINTEXT("EMCLUB_NEW_FB_MEM_ALREADY");
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
        break;

    case EMCLUB_NEW_FB_MASTER_LVL:
        strText = sc::string::format(
            ID2GAMEINTEXT("EMCLUB_NEW_FB_MASTER_LVL"),
            GLogicData::GetInstance().GetClubRankData(0).m_dwMasterLvl);
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
        break;

    case EMCLUB_NEW_FB_AVER_LIVING:
        strText = sc::string::format(
            ID2GAMEINTEXT("EMCLUB_NEW_FB_AVER_LIVING"),
            GLogicData::GetInstance().GetClubRankData(0).m_dwLivingPoint);
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
        break;

    case EMCLUB_NEW_FB_NOTMONEY:
        strText = sc::string::format(
            ID2GAMEINTEXT("EMCLUB_NEW_FB_NOTMONEY"),
            GLogicData::GetInstance().GetClubRankData(0).m_dwPay);
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
        break;

    case EMCLUB_NEW_FB_NAME:
        strText = sc::string::format(ID2GAMEINTEXT("EMCLUB_NEW_FB_NAME"), pNetMsg->szClubName);
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
        break;

    case EMCLUB_NEW_FB_BADNAME:
        strText = sc::string::format(ID2GAMEINTEXT("EMCLUB_NEW_FB_BADNAME"), pNetMsg->szClubName);
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
        break;

    case EMCLUB_NEW_FB_OTHERSCHOOL:
        strText = ID2GAMEINTEXT("EMCLUB_NEW_FB_OTHERSCHOOL");
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
        break;
        //#ifdef TH_PARAM
    case EMCLUB_NEW_FB_THAICHAR_ERROR:
        strText = ID2GAMEINTEXT("EMCLUB_NEW_FB_THAICHAR_ERROR");
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
        break;
        //#endif
        //#ifdef VN_PARAM
    case EMCLUB_NEW_FB_VNCHAR_ERROR:
        strText = ID2GAMEINTEXT("EMCLUB_NEW_FB_VNCHAR_ERROR");
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
        break;
        //#endif
    default:
        strText = sc::string::format("Unknownw error. MakeNewClubLobby %1%", pNetMsg->emFB);
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
        break;
    };

    if (bLobby)
        DoModalOutInterface(strText, UI::MODAL_NOTITLE, UI::OK, UiID );
    else
        PrintMsgText(UiColor, strText);
}

//! 클럽 공지 변경 요청
bool GLGaeaClient::ReqClubNotice(const std::string& ClubNotice)
{    
    if (!m_pMyClub->IsValid())
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_NOTICE_FB_FAIL"));
        return false;
    }
    
    if (!m_pMyClub->IsMemberFlgNotice(m_pCharacter->CharDbNum()))
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_NOTICE_FB_NOTMATER"));
        return false;
    }

    GLMSG::SNET_CLUB_NOTICE_REQ NetMsg(ClubNotice);
    NETSENDTOAGENT(&NetMsg);

    return true;
}


/*
void GLGaeaClient::MsgClubMemberState(NET_MSG_GENERIC* nmg)
{
    GLMSG::SNET_CLUB_MEMBER_STATE* pNetMsg = (GLMSG::SNET_CLUB_MEMBER_STATE*) nmg;

    for (DWORD i=0; i<pNetMsg->dwNumber; ++i)
    {
        const GLMSG::SMEMSTATE& sSTATE = pNetMsg->sSTATE[i];
        m_pMyClub->SetMemberState(sSTATE.dwCHARID, sSTATE.bONLINE, sSTATE.nCHANNEL, sSTATE.nidMAP);
    }
    // UI Refresh
    m_pInterface->REFRESH_CLUB_LIST();
    //m_pInterface->REFRESH_CLUB_STATE();
}
*/
void GLGaeaClient::MsgClubInfo(NET_MSG_GENERIC* nmg)
{
    GLMSG::SNET_CLUB_INFO_2CLT* pNetMsg = (GLMSG::SNET_CLUB_INFO_2CLT*) nmg;

    m_pCharacter->m_ClubDbNum = pNetMsg->dwClubID;
    m_pMyClub->m_DbNum = pNetMsg->dwClubID;
    m_pMyClub->SetName(pNetMsg->szClubName);

    m_pMyClub->SetMasterCharDbNum(pNetMsg->dwMasterID);
    m_pMyClub->SetMasterName(pNetMsg->szMasterName);
    m_pMyClub->m_dwCDCertifior = pNetMsg->dwCDCertifior;

    m_pMyClub->m_dwMarkVER = pNetMsg->dwMarkVER;
    m_pMyClub->m_dwRank = pNetMsg->dwRank;

    m_pMyClub->m_tOrganize = pNetMsg->tOrganize;
    m_pMyClub->m_tDissolution = pNetMsg->tDissolution;

    m_pMyClub->SetNotice(pNetMsg->szNotice);

    m_pCharacter->SetNickName(pNetMsg->szNickName);

    // 클럽마크 버전 확인
    ReqClubMarkInfo(m_pMyClub->DbNum(), m_pMyClub->m_dwMarkVER);
	
	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_Club_UpdateMember );
}

HRESULT GLGaeaClient::ReqClubMarkInfo(DWORD dwClubID, DWORD dwMarkVer)
{
    if (dwMarkVer==0)
        return S_FALSE;

    //! 클라이언트 버전과 비교.
    //
    DWORD dwServerID = m_pCharacter->m_dwServerID;
    bool bVALID = DxClubMan::GetInstance().IsValidData(dwServerID, dwClubID, dwMarkVer);
    if (bVALID)
        return S_FALSE;

    //! 클럽 마크 갱신 요청.
    GLMSG::SNET_CLUB_MARK_INFO NetMsgInfo;
    NetMsgInfo.dwClubID = dwClubID;
    NETSENDTOAGENT(&NetMsgInfo);
    return S_OK;
}

void GLGaeaClient::MsgClubDel(NET_MSG_GENERIC* nmg)
{
    m_pCharacter->m_ClubDbNum = CLUB_NULL;
    m_pMyClub->RESET();

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_Club_UpdateMember );
}

void GLGaeaClient::MsgClubInfoDissolution(NET_MSG_GENERIC* nmg)
{
    GLMSG::SNET_CLUB_INFO_DISSOLUTION* pNetMsg = (GLMSG::SNET_CLUB_INFO_DISSOLUTION*) nmg;
    m_pMyClub->m_tDissolution = pNetMsg->tDissolution;
	
	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_Club_UpdateMember );
}

void GLGaeaClient::MsgClubMember(NET_MSG_GENERIC* nmg)
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) nmg;
    GLMSG::SNET_CLUB_MEMBER_2CLT RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return;

    for (size_t i=0; i<RecvData.m_Member.size(); ++i)
    {
        m_pMyClub->SetMember(RecvData.m_Member[i]);
        m_pChaDbNumNameCache->Add(RecvData.m_Member[i].m_ChaName, RecvData.m_Member[i].m_ChaDbNum);
    }
	
	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_Club_UpdateMember );
}

void GLGaeaClient::MsgClubMemberReqAsk(NET_MSG_GENERIC* nmg)
{
    if ( GLUseFeatures::GetInstance().IsUsingNotifyWindow() )
    {
        NotifyFactory factory;

        GetNotifyClientMan()->Add( 
            factory.MakeNotifyClient( this, gamelogic::notify::CLUB_INVITE, nmg ) );

		GLMSG::NET_CLUB_MEMBER_REQ_ASK_AC *pNetMsg = (GLMSG::NET_CLUB_MEMBER_REQ_ASK_AC *) nmg;
		GLWidgetScript::GetInstance().LuaCallFunc< void >(NSWIDGET_SCRIPT::g_strFunc_Club_JoinAsk, "-s, -s, -dw", 
			pNetMsg->szMasterName, pNetMsg->szClubName, pNetMsg->dwMaster);
    }
    else
    {
        GLMSG::NET_CLUB_MEMBER_REQ_ASK_AC *pNetMsg = (GLMSG::NET_CLUB_MEMBER_REQ_ASK_AC *) nmg;

		// 토너먼트중 파티를 할수가 없다.
		// TOURNAMENT_UNIQ_DISABLE
		bool bTOURNAMNET = GetTournamentClient()->GetIsTournamentView();

		if(bTOURNAMNET)
		{
			ReqClubJoinAns(pNetMsg->dwMaster, false);
			return;
		}

		GLWidgetScript::GetInstance().LuaCallFunc< void >(NSWIDGET_SCRIPT::g_strFunc_Club_JoinAsk, "-s, -s, -dw", 
			pNetMsg->szMasterName, pNetMsg->szClubName, pNetMsg->dwMaster);    
    }	
}

void GLGaeaClient::MsgClubRank(NET_MSG_GENERIC* nmg)
{
    GLMSG::NET_CLUB_RANK_CHANGE_FC* pNetMsg = (GLMSG::NET_CLUB_RANK_CHANGE_FC*) nmg;
    m_pMyClub->m_dwRank = pNetMsg->dwRank;
	
	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_Club_UpdateRank );
}

void GLGaeaClient::MsgClubRankFB(NET_MSG_GENERIC* nmg)
{
    GLMSG::SNET_CLUB_RANK_FB* pNetMsg = (GLMSG::SNET_CLUB_RANK_FB*) nmg;

    std::string strText;
    DWORD UiColor = NS_UITEXTCOLOR::DEFAULT;
    
    switch (pNetMsg->emFB)
    {
    case EMCLUB_RANK_FB_FAIL:
        strText = ID2GAMEINTEXT("EMCLUB_RANK_FB_FAIL");
        UiColor = NS_UITEXTCOLOR::NEGATIVE;        
        break;
    case EMCLUB_RANK_FB_OK:
        strText = ID2GAMEINTEXT("EMCLUB_RANK_FB_OK");
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
        break;
    case EMCLUB_RANK_FB_MAX:
        strText = ID2GAMEINTEXT("EMCLUB_RANK_FB_MAX");
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
        break;
    case EMCLUB_RANK_FB_MASTER_LVL:
        strText = sc::string::format(ID2GAMEINTEXT("EMCLUB_RANK_FB_MASTER_LVL"), pNetMsg->dwValue);
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
        break;
    case EMCLUB_RANK_FB_AVER_LIVING:
        strText = sc::string::format(ID2GAMEINTEXT("EMCLUB_RANK_FB_AVER_LIVING"), pNetMsg->dwValue);
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
        break;
    case EMCLUB_RANK_FB_NOTMONEY:
        strText = sc::string::format( ID2GAMEINTEXT("EMCLUB_RANK_FB_NOTMONEY"), pNetMsg->dwValue);        
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
        break;
    default:
        strText = sc::string::format("MsgClubRankFB %1%", pNetMsg->emFB);
        break;
    };

    PrintMsgTextDlg(UiColor, strText);
}
/*
void GLGaeaClient::MsgClubMemberPos(NET_MSG_GENERIC* nmg)
{
    GLMSG::SNET_CLUB_MEMBER_POS* pNetMsg = (GLMSG::SNET_CLUB_MEMBER_POS*) nmg;

    for (DWORD i=0; i<pNetMsg->dwNumber; ++i)
    {
        const GLMSG::SMEMPOS& sPOS = pNetMsg->sPOS[i];
        m_pMyClub->SetMemberPos(sPOS.dwCHARID, D3DXVECTOR3(sPOS.vecPOS.x,0,sPOS.vecPOS.y));
    }
    m_pInterface->REFRESH_CLUB_LIST();
}
*/
void GLGaeaClient::MsgClubDissolutionFB(NET_MSG_GENERIC* nmg)
{
    GLMSG::SNET_CLUB_DISSOLUTION_FB* pNetMsg = (GLMSG::SNET_CLUB_DISSOLUTION_FB*) nmg;

    std::string strText; 

    DWORD UiColor = NS_UITEXTCOLOR::DEFAULT;
    switch (pNetMsg->emFB)
    {
    case EMCLUB_DIS_FB_FAIL:
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
        strText = ID2GAMEINTEXT("EMCLUB_DIS_FB_FAIL");
        break;
    case EMCLUB_DIS_FB_OK:
		{
			m_pMyClub->DoDissolution(pNetMsg->tDissolution);
			strText = sc::string::format(ID2GAMEINTEXT("EMCLUB_DIS_FB_OK"), GLCONST_CHAR::dwCLUB_DISSOLUTION_DAY);
			UiColor = NS_UITEXTCOLOR::NEGATIVE;

			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_Club_Dissolution,
				"-b", true );
		}
        break;
    case EMCLUB_DIS_FB_CANCEL:
		{
			m_pMyClub->DoDissolutionCancel();
			strText = ID2GAMEINTEXT("EMCLUB_DIS_FB_CANCEL");
			UiColor = NS_UITEXTCOLOR::NEGATIVE;

			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_Club_Dissolution,
				"-b", false );
		}
        break;
    case EMCLUB_DIS_FB_CLUBBATTLE:
        strText = ID2GAMEINTEXT("EMCLUB_DIS_FB_CLUBBATTLE");
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
        break;
    default:
        strText = sc::string::format("MsgClubDissolutionFB %1%", pNetMsg->emFB);
        break;
    };

    PrintMsgTextDlg(UiColor, strText);
}

void GLGaeaClient::MsgClubMemberDel(NET_MSG_GENERIC* nmg)
{
    GLMSG::NET_CLUB_MEMBER_DEL_FC* pNetMsg = (GLMSG::NET_CLUB_MEMBER_DEL_FC*) nmg;
    if (m_pMyClub->DbNum() == pNetMsg->dwClub)
        m_pMyClub->DelMember(pNetMsg->dwMember);
	
	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_Club_UpdateMember );
}

void GLGaeaClient::MsgClubMemberDelFB(NET_MSG_GENERIC* nmg)
{
    GLMSG::NET_CLUB_MEMBER_DEL_AC* pNetMsg = (GLMSG::NET_CLUB_MEMBER_DEL_AC*) nmg;
    DWORD UiColor = NS_UITEXTCOLOR::DEFAULT;
    std::string Msg;
    switch (pNetMsg->emFB)
    {
    case EMCLUB_MEMBER_DEL_FB_FAIL:
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
        Msg = ID2GAMEINTEXT("EMCLUB_MEMBER_DEL_FB_FAIL");
        break;
    case EMCLUB_MEMBER_DEL_FB_OK:
        UiColor = NS_UITEXTCOLOR::DEFAULT;
        Msg = ID2GAMEINTEXT("EMCLUB_MEMBER_DEL_FB_OK");
		GLWidgetScript::GetInstance().LuaCallFunc< void >( 
			NSWIDGET_SCRIPT::g_strFunc_Club_UpdateMember );
        break;
    case EMCLUB_MEMBER_DEL_FB_GRADE:
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
        Msg = ID2GAMEINTEXT("CLUB_MEMBER_DEL_GRADE_ERROR");
        break;
    default:
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
        Msg = sc::string::format("MsgClubMemberDelFB UNKNOWN TYPE %1%", pNetMsg->emFB);
        break;
    };

    PrintMsgTextDlg(UiColor, Msg);
}

void GLGaeaClient::MsgClubMemberSecedeFB(NET_MSG_GENERIC* nmg)
{
    GLMSG::SNET_CLUB_MEMBER_SECEDE_FB* pNetMsg = (GLMSG::SNET_CLUB_MEMBER_SECEDE_FB*) nmg;
    switch (pNetMsg->emFB)
    {
    case EMCLUB_MEMBER_SECEDE_FB_FAIL:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_MEMBER_SECEDE_FB_FAIL") );
        break;
    case EMCLUB_MEMBER_SECEDE_FB_OK:
        {
            m_pCharacter->m_ClubDbNum = CLUB_NULL;
            m_pMyClub->RESET();

			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_Club_UpdateMember );

            std::string strText = sc::string::format(ID2GAMEINTEXT("EMCLUB_MEMBER_SECEDE_FB_OK"), GLCONST_CHAR::dwCLUB_JOINBLOCK_DAY);
            PrintConsoleTextDlg(strText);
        }
        break;
    };
}

void GLGaeaClient::MsgClubAuthorityReqAsk(NET_MSG_GENERIC* nmg)
{
	GLWidgetScript::GetInstance().LuaCallFunc< void >( 
		NSWIDGET_SCRIPT::g_strFunc_Club_AuthorityMemberAsk,
		"-s", m_pMyClub->Name() );
}

void GLGaeaClient::MsgClubAuthorityReqFB(NET_MSG_GENERIC* nmg)
{
    GLMSG::SNET_CLUB_AUTHORITY_REQ_FB* pNetMsg = (GLMSG::SNET_CLUB_AUTHORITY_REQ_FB*) nmg;

    GLCLUBMEMBER* pMember = m_pMyClub->GetMember(pNetMsg->dwCharID);
    if (!pMember)
        return;

    switch (pNetMsg->emFB)
    {
    case EMCLUB_AUTHORITY_REQ_FB_FAIL:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_AUTHORITY_REQ_FB_FAIL") );
        break;
    case EMCLUB_AUTHORITY_REQ_FB_OK:
        {
            std::string strText = sc::string::format( ID2GAMEINTEXT("EMCLUB_AUTHORITY_REQ_FB_OK"), pMember->m_ChaName.c_str(), m_pMyClub->Name() ).c_str();
            PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, strText );
        }
        break;
    case EMCLUB_AUTHORITY_REQ_FB_REFUSE:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_AUTHORITY_REQ_FB_REFUSE") );
        break;
    case EMCLUB_AUTHORITY_REQ_FB_NOMASTER:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_AUTHORITY_REQ_FB_NOMASTER") );
        break;
    case EMCLUB_AUTHORITY_REQ_FB_NOONLINE:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_AUTHORITY_REQ_FB_NOONLINE") );
        break;
    case EMCLUB_AUTHORITY_REQ_FB_ALLIANCE:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_AUTHORITY_REQ_FB_ALLIANCE") );
        break;
    case EMCLUB_AUTHORITY_REQ_FB_CLUBBATTLE:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_AUTHORITY_REQ_FB_CLUBBATTLE") );
        break;
    case EMCLUB_AUTHORITY_REQ_FB_CONFING:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_AUTHORITY_REQ_FB_CONFING") );
        break;
    case EMCLUB_AUTHORITY_REQ_FB_TIME:
        {
            CTime cTime(pNetMsg->tAuthority);

            //	서버 타임으로 변환( TimeZone 계산 )
            GetConvertServerTime( cTime );

            CString strAuthorityDate;

            strAuthorityDate = sc::string::format(
                ID2GAMEWORD("ITEM_EXPIRE_DATE"),
                cTime.GetYear(),
                cTime.GetMonth(),
                cTime.GetDay(),
                cTime.GetHour(),
                cTime.GetMinute() ).c_str();

            std::string strText = sc::string::format( ID2GAMEINTEXT("EMCLUB_AUTHORITY_REQ_FB_TIME"), strAuthorityDate ).c_str();
            PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, strText );
        }
        break;
    case EMCLUB_AUTHORITY_REQ_FB_GUIDNBATTLE:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_AUTHORITY_REQ_FB_GUIDNBATTLE") );
        break;
    case EMCLUB_AUTHORITY_REQ_FB_CDMBATTLE:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_AUTHORITY_REQ_FB_CDMBATTLE") );
        break;
	case EMCLUB_AUTHORITY_REQ_FB_OFF:
		PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_AUTHORITY_REQ_FB_OFF") );
		break;
    }
}

void GLGaeaClient::MsgClubAutority(NET_MSG_GENERIC* nmg)
{
    GLMSG::SNET_CLUB_AUTHORITY_CLT *pNetMsg = (GLMSG::SNET_CLUB_AUTHORITY_CLT *)nmg;

    GLCLUBMEMBER* pMember = m_pMyClub->GetMember( pNetMsg->dwCharID );
    if (!pMember)
        return;

    m_pMyClub->SetMasterCharDbNum(pNetMsg->dwCharID);
    m_pMyClub->SetMasterName(pMember->m_ChaName);

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_Club_UpdateMember );

    std::string strText = sc::string::format(ID2GAMEINTEXT("EMCLUB_AUTHORITY_CLT"), pMember->m_ChaName.c_str());
    PrintMsgTextDlg(NS_UITEXTCOLOR::PALEGREEN, strText);
}

void GLGaeaClient::MsgClubCD_CertifyFB(NET_MSG_GENERIC* nmg)
{
    GLMSG::SNET_CLUB_CD_CERTIFY_FB* pNetMsg = (GLMSG::SNET_CLUB_CD_CERTIFY_FB*) nmg;

    DWORD UiColor = NS_UITEXTCOLOR::DEFAULT;
    std::string Msg;
    switch (pNetMsg->emFB)
    {
    case EMCDCERTIFY_FAIL:
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
        Msg = ID2GAMEINTEXT("EMCDCERTIFY_FAIL");
        break;

    case EMCDCERTIFY_OK:
        UiColor = NS_UITEXTCOLOR::PALEGREEN;
        Msg = ID2GAMEINTEXT("EMCDCERTIFY_OK");
        m_pCharacter->m_bCLUB_CERTIFY = TRUE;
        m_pCharacter->m_fCLUB_CERTIFY_TIMER = 0.0f;
		//!! SF_TODO
        //m_pInterface->SET_CONFT_CONFIRM_VISIBLE(true);
        break;

    case EMCDCERTIFY_BADNPC:
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
        Msg = ID2GAMEINTEXT("EMCDCERTIFY_BADNPC");
        break;

    case EMCDCERTIFY_NOTMASTER:
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
        Msg = ID2GAMEINTEXT("EMCDCERTIFY_NOTMASTER");
        break;

    case EMCDCERTIFY_AREADYCERTIFY:
        UiColor = NS_UITEXTCOLOR::PALEGREEN;
        Msg = ID2GAMEINTEXT("EMCDCERTIFY_AREADYCERTIFY");
        break;

    case EMCDCERTIFY_HAVEGUIDCLUB:
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
        Msg = ID2GAMEINTEXT("EMCDCERTIFY_HAVEGUIDCLUB");
        break;

    case EMCDCERTIFY_OTHERCERTIFING:
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
        Msg = ID2GAMEINTEXT("EMCDCERTIFY_OTHERCERTIFING");
        break;

    case EMCDCERTIFY_AREADYGUID:
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
        Msg = ID2GAMEINTEXT("EMCDCERTIFY_AREADYGUID");
        break;

    case EMCDCERTIFY_NOTTIME:
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
        Msg = ID2GAMEINTEXT("EMCDCERTIFY_NOTTIME");
        break;

    case EMCDCERTIFY_DISTANCE:
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
        Msg = ID2GAMEINTEXT("EMCDCERTIFY_DISTANCE");
        break;

    case EMCDCERTIFY_NOTITEM:
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
        Msg = ID2GAMEINTEXT("EMCDCERTIFY_NOTITEM");
        break;

    case EMCDCERTIFY_NOTCONDITION:
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
        Msg = ID2GAMEINTEXT("EMCDCERTIFY_NOTCONDITION");
        break;

    case EMCDCERTIFY_AREADY:
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
        Msg = ID2GAMEINTEXT("EMCDCERTIFY_AREADY");
        break;

    case EMCDCERTIFY_ING_TIMEOUT:
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
        Msg = ID2GAMEINTEXT("EMCDCERTIFY_ING_TIMEOUT");
        m_pCharacter->m_bCLUB_CERTIFY = FALSE;
		//!! SF_TODO
        //m_pInterface->SET_CONFT_CONFIRM_VISIBLE(false);
        break;

    case EMCDCERTIFY_ING_DISTANCE:
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
        Msg = ID2GAMEINTEXT("EMCDCERTIFY_ING_DISTANCE");
        m_pCharacter->m_bCLUB_CERTIFY = FALSE;
		//!! SF_TODO
        //m_pInterface->SET_CONFT_CONFIRM_VISIBLE ( false );
        break;

    case EMCDCERTIFY_ING_DIE:
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
        Msg = ID2GAMEINTEXT("EMCDCERTIFY_ING_DIE");
        m_pCharacter->m_bCLUB_CERTIFY = FALSE;
		//!! SF_TODO
        //m_pInterface->SET_CONFT_CONFIRM_VISIBLE ( false );
        break;

    case EMCDCERTIFY_COMPLETE:
        UiColor = NS_UITEXTCOLOR::PALEGREEN;
        Msg = ID2GAMEINTEXT("EMCDCERTIFY_COMPLETE");
        m_pCharacter->m_bCLUB_CERTIFY = FALSE;
		//!! SF_TODO
        //m_pInterface->SET_CONFT_CONFIRM_VISIBLE ( false );
        break;
    default:
        Msg = sc::string::format("MsgClubCD_CertifyFB %1%", pNetMsg->emFB);
        break;
    };

    PrintMsgTextDlg(UiColor, Msg);
}

void GLGaeaClient::MsgClubCommissionFB(NET_MSG_GENERIC* nmg)
{
    GLMSG::SNET_CLUB_GUID_COMMISSION_FB* pNetMsg = (GLMSG::SNET_CLUB_GUID_COMMISSION_FB*) nmg;

    DWORD UiColor = NS_UITEXTCOLOR::DEFAULT;
    std::string Msg;
    switch (pNetMsg->emFB)
    {
    case EMGUIDCOMMISSION_FB_FAIL:
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
        Msg = ID2GAMEINTEXT("EMGUIDCOMMISSION_FB_FAIL");
        break;
    case EMGUIDCOMMISSION_FB_OK:
        UiColor = NS_UITEXTCOLOR::PALEGREEN;
        Msg = sc::string::format(ID2GAMEINTEXT("EMGUIDCOMMISSION_FB_OK"), pNetMsg->fCommission);
        break;
    case EMGUIDCOMMISSION_FB_RANGE:
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
        Msg = ID2GAMEINTEXT("EMGUIDCOMMISSION_FB_RANGE");
        break;
    case EMGUIDCOMMISSION_FB_NOTCLUB:
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
        Msg = ID2GAMEINTEXT("EMGUIDCOMMISSION_FB_NOTCLUB");
        break;
    case EMGUIDCOMMISSION_FB_ALREADY:
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
        Msg = ID2GAMEINTEXT("EMGUIDCOMMISSION_FB_ALREADY");
        break;
    case EMGUIDCOMMISSION_FB_BATTLE:
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
        Msg = ID2GAMEINTEXT("EMGUIDCOMMISSION_FB_BATTLE");
        break;
    default:
        Msg = sc::string::format("MsgClubCommissionFB %1%", pNetMsg->emFB);
    };

    PrintMsgTextDlg(UiColor, Msg);
}

void GLGaeaClient::MsgClubNoticeFB(NET_MSG_GENERIC* nmg)
{
    GLMSG::SNET_CLUB_NOTICE_FB *pNetMsg = (GLMSG::SNET_CLUB_NOTICE_FB *)nmg;

    switch ( pNetMsg->emFB )
    {
    case EMCLUB_NOTICE_FB_FAIL:
        {
            PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_NOTICE_FB_FAIL") );
        }
        break;
    case EMCLUB_NOTICE_FB_OK:
        {
            PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMCLUB_NOTICE_FB_OK") );
        }
        break;
    case EMCLUB_NOTICE_FB_NOTMATER:
        {
            PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_NOTICE_FB_NOTMATER") );
        }
        break;
    };
}

void GLGaeaClient::MsgClubNotice(NET_MSG_GENERIC* nmg)
{
    GLMSG::SNET_CLUB_NOTICE_CLT *pNetMsg = (GLMSG::SNET_CLUB_NOTICE_CLT *)nmg;
    m_pMyClub->SetNotice(pNetMsg->szNotice);
    m_pMyClub->SetNoticeDate(pNetMsg->m_NoticeDate);
    m_pMyClub->SetNoticeChaName(pNetMsg->m_NoticeChaName);

    //	Note : 채팅창에 공지 출력.
    PrintMsgDlg (NS_UITEXTCOLOR::GUILD, ID2GAMEINTEXT("EMCLUB_NOTICE_FB_OK"));

	std::string strClubName = m_pMyClub->Name();

	if( this->GetServiceProvider() == SP_GS)
		strClubName = sc::string::ansi_to_utf8(strClubName);
	
    PrintMsgText(NS_UITEXTCOLOR::GUILD, sc::string::format(ID2GAMEINTEXT("CLUB_NOTICE"), strClubName.c_str() ));
    for ( int i = 0; i < EMCLUB_NOTICE_LEN; i++ )
    {
        if ( m_pMyClub->m_szNotice[i] == '\n' ||
            m_pMyClub->m_szNotice[i] == '\r' )
            m_pMyClub->m_szNotice[i] = ' ';
    }

	std::string strNotice = strNotice.c_str();

	if( this->GetServiceProvider() == SP_GS)
			strNotice = sc::string::ansi_to_utf8(strNotice);

	PrintMsgText(NS_UITEXTCOLOR::GUILD, sc::string::format("%s", strNotice.c_str()));

    //	Note : 클럽창 갱신.
	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Club_NoticeChange );
}

void GLGaeaClient::MsgClubMemberRename(NET_MSG_GENERIC* nmg)
{
    GLMSG::SNET_CLUB_MEMBER_RENAME_CLT *pNetMsg = (GLMSG::SNET_CLUB_MEMBER_RENAME_CLT *)nmg;

    GLCLUBMEMBER* pCLUBMember = m_pMyClub->GetMember( pNetMsg->dwCharID );
    GASSERT ( pCLUBMember );
    if ( pCLUBMember )
    {
        pCLUBMember->SetName(pNetMsg->szName);
    }
    if ( m_pMyClub->IsMaster ( pNetMsg->dwCharID ) )
    {
        m_pMyClub->SetMasterName(pNetMsg->szName);
    }

	// SQLite 갱신
	std::string szStrName( pNetMsg->szName );
	m_pMyClub->SetMemberName( pNetMsg->dwCharID, szStrName );

    // 인터페이스 갱신
	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_Club_UpdateMember );
}

void GLGaeaClient::MsgClubSubMasterFb(NET_MSG_GENERIC* nmg)
{
    GLMSG::SNET_CLUB_SUBMASTER_FB *pNetMsg = (GLMSG::SNET_CLUB_SUBMASTER_FB *)nmg;

    switch ( pNetMsg->emFB )
    {
    case EMCLUBSUBMASTER_FB_FAIL:
        {
            PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUBSUBMASTER_FB_FAIL") );
        }
        break;
    case EMCLUBSUBMASTER_FB_OK:
        {
            PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMCLUBSUBMASTER_FB_OK") );
        }
        break;

    case EMCLUBSUBMASTER_FB_RESET:
        {
            PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMCLUBSUBMASTER_FB_RESET") );
        }
        break;

    case EMCLUBSUBMASTER_FB_NOTMASTER:
        {
            PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUBSUBMASTER_FB_NOTMASTER") );
        }
        break;
    case EMCLUBSUBMASTER_FB_BATTLE:
        {
            PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUBSUBMASTER_FB_BATTLE") );
        }
        break;
    case EMCLUBSUBMASTER_FB_CDM:
        {
            PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUBSUBMASTER_FB_CDM") );
        }
        break;
    case EMCLUBSUBMASTER_FB_CDM_MEM:
        {
            std::string strText = sc::string::format( ID2GAMEINTEXT("EMCLUBSUBMASTER_FB_CDM_MEM"), GLCONST_CHAR::dwCLUB_DEATHMATCH_MEM ).c_str();
            PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, strText );
        }
        break;
    };
}


//! 클럽 마스터 권한 위임 답변
void GLGaeaClient::ReqClubAuthorityAns(bool bOK)
{
    GLMSG::SNET_CLUB_AUTHORITY_REQ_ANS NetMsgAns;
    NetMsgAns.bOK = bOK;
    NETSENDTOAGENT(&NetMsgAns);
}

//! CD 인증요청
void GLGaeaClient::ReqCDCertify(DWORD dwNpcGlobID)
{
    if (!m_pMyClub->IsValid())
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCDCERTIFY_NOTMASTER"));
        return;
    }

    if (!m_pMyClub->IsMemberFlgCDCertify(m_pCharacter->CharDbNum()))
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCDCERTIFY_NOTMASTER"));
        return;
    }
    
    if (!m_pLandMClient)
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCDCERTIFY_FAIL"));
        return;
    }

    std::tr1::shared_ptr<GLCrowClient> pCROW = m_pLandMClient->GetCrow(dwNpcGlobID);
    if (!pCROW)
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCDCERTIFY_FAIL"));
        return;
    }

    GLMSG::SNET_CLUB_CD_CERTIFY NetMsg;
    NetMsg.dwNpcGlobID = dwNpcGlobID;
    NETSENDTOFIELD(&NetMsg);
}

//! Club Rank up 요청/랭크업, 클럽
void GLGaeaClient::ReqClubRank(DWORD dwNpcGlobID)
{
    if (!m_pMyClub->IsValid())
        return;

    if (!m_pMyClub->HaveRankUpAuthority(m_pCharacter->CharDbNum()))
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("CLUB_NOT_HAVE_AUTHRITY"));
        return;
    }    

    if (m_pMyClub->GetRank() >= (GLCONST_CHAR::dwMAX_CLUBRANK-1))
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_RANK_FB_MAX"));
        return;
    }

    DWORD dwRANK = m_pMyClub->GetRank() + 1;

    CString strText;
    if (m_pMyClub->GetMasterLevel() < static_cast<int> (GLogicData::GetInstance().GetClubRankData(dwRANK).m_dwMasterLvl))
    {		
        PrintMsgTextDlg(
            NS_UITEXTCOLOR::NEGATIVE,
            sc::string::format(
                ID2GAMEINTEXT("EMCLUB_RANK_FB_MASTER_LVL"),
                GLogicData::GetInstance().GetClubRankData(dwRANK).m_dwMasterLvl));
        return;
    }
    /*
    // Master 의 Living Point 를 알 방법이 현재 없다. 2012-06-01 jgkim
    if (m_pCharacter->m_nLiving < int ( GLogicData::GetInstance().GetClubRankData(dwRANK).m_dwLivingPoint))
    {		
        PrintMsgTextDlg(
            NS_UITEXTCOLOR::NEGATIVE,
            sc::string::format(
                ID2GAMEINTEXT("EMCLUB_RANK_FB_AVER_LIVING"),
                GLogicData::GetInstance().GetClubRankData(dwRANK).m_dwLivingPoint));
        return;
    }
    */

    if (m_pCharacter->GetInvenMoney() < GLogicData::GetInstance().GetClubRankData(dwRANK).m_dwPay)
    {		
        PrintMsgTextDlg(
            NS_UITEXTCOLOR::NEGATIVE,
            sc::string::format(
                ID2GAMEINTEXT("EMCLUB_RANK_FB_NOTMONEY"),
                GLogicData::GetInstance().GetClubRankData(dwRANK).m_dwPay));
        return;
    }

    GLMSG::NET_CLUB_RANK_UP_CF NetMsg(dwNpcGlobID);
    NETSENDTOFIELD(&NetMsg);
}

//! 클럽 공지 작성 날짜
std::string GLGaeaClient::GetClubNoticeDate() const
{    
    return sc::time::DateTimeFormat(m_pMyClub->GetNoticeDate(), true, false);
}

//! 클럽 공지 작성 캐릭터
std::string GLGaeaClient::GetClubNoticeChaName() const
{
    return std::string(m_pMyClub->GetNoticeChaName());
}

//! 클럽 공지/Club notice
std::string GLGaeaClient::GetClubNotice() const
{
    return std::string(m_pMyClub->GetNotice());
}

//! 클럽 창설일
std::string GLGaeaClient::GetClubCreateDate() const
{
    return sc::time::DateTimeFormat(m_pMyClub->GetOrganizeDate(), false, false);
}

//! 클럽 랭크
std::string GLGaeaClient::GetClubRank() const
{
    return std::string(ID2GAMEWORD("CLUB_GRADE", m_pMyClub->GetRank()));
}

//! 클럽 특별활동 점수
DWORD GLGaeaClient::GetClubActivityPoint() const
{
    // 아직 구현되지 않음
    return 0;
}

//! 클럽 선도클럽 맵
void GLGaeaClient::GetClubGuidanceMap(std::vector<std::string>& vMap)
{
    if ( m_pMyClub )
        m_pMyClub->GetGuidanceMapName( vMap );
}

/*
//! 클럽 정보 업데이트 요청
bool GLGaeaClient::ReqClubInfoUpdate(bool bUpdate)
{
    if (!bUpdate) 
    {
        if (m_pInterface->UiIsVisibleGroup(CLUB_WINDOW) || m_pInterface->UiIsVisibleGroup(MAP_WINDOW)) 
            return false;
    }

    GLMSG::SNETPC_REQ_FRIEND_CLUB_OPEN NetMsg(EMCLUB_WINDOW, bUpdate);
    NETSENDTOAGENT(&NetMsg);
    return false;
}
*/

//! 클럽 멤버 별명 변경 요청
bool GLGaeaClient::ReqClubNick(const char* szNickName)
{
    if (!szNickName)
        return false;
    
    if (!m_pMyClub->IsValid())
        return false;
    

    CString strTEMP(szNickName);

    if (m_ServiceProvider == SP_WORLD_BATTLE)
    {
        std::string ClubNick(szNickName);
        if (!sc::string::checkEnglishOrNumber(ClubNick))
        {
            PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEEXTEXT("CREATE_CHARACTER_NAME_ERROR_WB"));
            return false;
        }
    }

    if (m_ServiceProvider == SP_KOREA || m_ServiceProvider == SP_KOREA_TEST)
    {
        std::string ClubNick(szNickName);
        if (!sc::string::goodChaNameKorea(ClubNick))
        {
            PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEEXTEXT("CREATE_CHARACTER_NAME_ERROR_KOREA"));
            return false;
        }
    }

    // 태국어 문자 조합 체크
    if (RENDERPARAM::emLangSet == language::THAILAND) 
    {
        if (!STRUTIL::CheckThaiString(strTEMP))
        {
            PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_MEMBER_NICK_FB_THAICHAR_ERROR"));
            return false;
        }
    }

    // 베트남 문자 조합 체크 
    if (RENDERPARAM::emLangSet == language::VIETNAM)
    {
        if (STRUTIL::CheckVietnamString(strTEMP))
        {
            PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_MEMBER_NICK_FB_VNCHAR_ERROR"));
            return false;
        }
    }

    BOOL bFILTER0 = STRUTIL::CheckString(strTEMP);
    BOOL bFILTER1 = CRanFilter::GetInstance().NameFilter(szNickName);
    if (bFILTER0 || bFILTER1)
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_MEMBER_NICK_FB_BADNAME"));
        return false;
    }

    //! 클럽 멤버 별명.
    GLMSG::SNET_CLUB_MEMBER_NICK NetMsg(szNickName);
    NETSENDTOAGENT(&NetMsg);
    return true;
}

//! 클럽 선도클럽 맵 요청.
void GLGaeaClient::ReqGuidanceMapName()
{
    GLMSG::NET_GUIDANCE_MAP_INFO_CA NetMsg( GetMyClub()->DbNum() );
    NetMsg.dwChaNum = GetCharacter()->CharDbNum();
    NETSENDTOAGENT(&NetMsg);
}

//! 클럽 선도클럽 맵 받음.
void GLGaeaClient::MsgGuidanceMapInfoAC(NET_MSG_GENERIC* pMsg)
{
    m_pMyClub->MsgGuidanceMapInfo( pMsg );
}

void GLGaeaClient::MsgClubMemberNickFB(NET_MSG_GENERIC* nmg)
{
    GLMSG::SNET_CLUB_MEMBER_NICK_FB* pNetMsg = (GLMSG::SNET_CLUB_MEMBER_NICK_FB*) nmg;
    std::string Msg;
    DWORD UiColor = NS_UITEXTCOLOR::DEFAULT;
    switch (pNetMsg->emFB)
    {
    case EMCLUB_MEMBER_NICK_FB_FAIL:
        Msg = ID2GAMEINTEXT("EMCLUB_MEMBER_NICK_FB_FAIL");
        UiColor = NS_UITEXTCOLOR::NEGATIVE;

		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Club_ChangeNickName, "-s", m_pCharacter->GetNickName());
        break;

    case EMCLUB_MEMBER_NICK_FB_OK:
        Msg = ID2GAMEINTEXT("EMCLUB_MEMBER_NICK_FB_OK");
        m_pCharacter->SetNickName(pNetMsg->szNick);
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Club_ChangeNickName, "-s", pNetMsg->szNick);
        break;

    case EMCLUB_MEMBER_NICK_FB_BADNAME:
        Msg = ID2GAMEINTEXT("EMCLUB_MEMBER_NICK_FB_BADNAME");
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Club_ChangeNickName, "-s", m_pCharacter->GetNickName());
        break;

    case EMCLUB_MEMBER_NICK_FB_THAICHAR_ERROR:
        Msg = ID2GAMEINTEXT("EMCLUB_MEMBER_NICK_FB_THAICHAR_ERROR");
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Club_ChangeNickName, "-s", m_pCharacter->GetNickName());
        break;

    case EMCLUB_MEMBER_NICK_FB_VNCHAR_ERROR:
        Msg = ID2GAMEINTEXT("EMCLUB_MEMBER_NICK_FB_VNCHAR_ERROR");
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Club_ChangeNickName, "-s", m_pCharacter->GetNickName());
        break;

    case EMCLUB_MEMBER_NICK_FB_WB_ERROR:
        Msg = ID2GAMEEXTEXT("CREATE_CHARACTER_NAME_ERROR_WB");
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Club_ChangeNickName, "-s", m_pCharacter->GetNickName());
        break;

    case EMCLUB_MEMBER_NICK_FB_KR_ERROR:
        Msg = ID2GAMEEXTEXT("CREATE_CHARACTER_NAME_ERROR_KOREA");
        UiColor = NS_UITEXTCOLOR::NEGATIVE;
		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Club_ChangeNickName, "-s", m_pCharacter->GetNickName());
        break;

    default:
        Msg = sc::string::format("MsgClubMemberNickFB %1%", pNetMsg->emFB);
		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Club_ChangeNickName, "-s", m_pCharacter->GetNickName());
        break;
    };

    PrintMsgTextDlg(UiColor, Msg);
    
}


std::string GLGaeaClient::GetClubNickName() const
{
    if (!m_pMyClub->IsValid())
        return std::string("");
    else
        return std::string(m_pCharacter->GetNickName());
}

const char* GLGaeaClient::GetClubNickName_char()
{
	if (!m_pMyClub->IsValid())
		return std::string("").c_str();
	else
		return m_pCharacter->GetNickName();
}

const clubmark::DXDATA& GLGaeaClient::GetClubMark(DWORD ClubDbNum, DWORD MarkVersion)
{
    if (!DxClubMan::GetInstance().IsValidData(m_pCharacter->m_dwServerID, ClubDbNum, MarkVersion))
        ReqClubMarkInfo(ClubDbNum, MarkVersion);

    return DxClubMan::GetInstance().GetClubData(
        m_pd3dDevice,
        m_pCharacter->m_dwServerID,
        ClubDbNum);
}

bool GLGaeaClient::ClubPublicTypeReq(DWORD PublicFlag)
{
    if (!m_pMyClub->IsValid())
        return false;

    if (!m_pMyClub->HaveAuthChangeAuthority(m_pCharacter->CharDbNum()))
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("CLUB_NOT_HAVE_AUTHRITY"));
        return false;
    }

    NET_CLUB_PULIC_FLAG_CHANGE_CA MsgCA(PublicFlag);
    NETSENDTOAGENT(&MsgCA);

    return true;
}

void GLGaeaClient::MsgClubPublicFlagChangeAC(NET_MSG_GENERIC* nmg)
{
    NET_CLUB_PULIC_FLAG_CHANGE_AC* pPacket = (NET_CLUB_PULIC_FLAG_CHANGE_AC*) nmg;
    
    m_pMyClub->PublicSet(pPacket->m_PublicFlag);

    std::string Msg(
        sc::string::format(
            ID2GAMEINTEXT("CLUB_PULIC_FLAG_CHANGE_AC"),
            pPacket->m_ChaName));
    PrintMsgTextDlg(NS_UITEXTCOLOR::ENABLE, Msg);
    
    // UI refresh
}

bool GLGaeaClient::ClubAutoKickReq(bool AutoKickUse, size_t GradeIndex, WORD Day)
{
    if (!m_pMyClub->IsValid())
        return false;

    if (!m_pMyClub->HaveMemberKickAuthority(m_pCharacter->CharDbNum()))
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("CLUB_NOT_HAVE_AUTHRITY"));
        return false;
    }

    if (GradeIndex >= m_pMyClub->GetGradeSize())
        return false;

    club::AUTO_KICK_OUT KickOut(AutoKickUse, GradeIndex, Day);
    NET_CLUB_AUTO_KICKC_CA MsgCA(KickOut);
    NETSENDTOAGENT(&MsgCA);

    return true;
}

void GLGaeaClient::MsgClubAutoKickAC(NET_MSG_GENERIC* nmg)
{
    // NET_MSG_CLUB_AUTO_KICK_AC
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) nmg;

    CLUB_AUTO_KICK_AC AutoKickOut;
    if (msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), AutoKickOut))
    {        
        PrintMsgTextDlg(
            NS_UITEXTCOLOR::ENABLE,
            sc::string::format(
                ID2GAMEINTEXT("NET_MSG_CLUB_AUTO_KICK_AC"), AutoKickOut.m_ChaName));
        m_pMyClub->AutoKickOutSet(AutoKickOut.m_KickOut);
		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Club_AutoBan);
    }
    else
    {
        sc::writeLogError("GLGaeaClient::MsgClubAutoKickAC");
    }

    // UI Refresh
}

//! 클럽 탈퇴 요청
bool GLGaeaClient::ReqClubSecede()
{
    if (!m_pMyClub->IsValid())
        return false;

    // 클럽장은 탈퇴를 할수 없다
    if (m_pMyClub->IsMaster(m_pCharacter->CharDbNum()))
        return false;

    GLMSG::SNET_CLUB_MEMBER_SECEDE NetMsg;
    NETSENDTOAGENT(&NetMsg);

    return true;
}

//! 새 클럽 마크로 변경 요청.
bool GLGaeaClient::ReqClubMarkChange(const char* szFileName)
{
    if (!szFileName)
        return false;

    if (!m_pMyClub->IsValid())
        return false;

    if (!m_pMyClub->HaveMarkChangeAuthority(m_pCharacter->CharDbNum()))
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("CLUB_NOT_HAVE_AUTHRITY"));
        return false;
    }

    // 파일에서 읽어오기.
    LPDWORD pMarkColor(NULL);
    BOOL bOK = DxClubMan::GetInstance().LoadClubMark(szFileName, pMarkColor);
    if (!bOK)
    {		
        PrintConsoleTextDlg(sc::string::format(ID2GAMEINTEXT("CLUB_LOADMARK_FAIL"), szFileName));
        return false;
    }

    // 클럽 마크 갱신 요청.
    GLMSG::SNET_CLUB_MARK_CHANGE NetMsgChange;
    memcpy(NetMsgChange.aryMark, pMarkColor, sizeof(DWORD)*EMCLUB_MARK_SX*EMCLUB_MARK_SY);
    NETSENDTOAGENT(&NetMsgChange);

    return true;
}

void GLGaeaClient::MsgClubMarkChangeFB(NET_MSG_GENERIC* nmg)
{
    GLMSG::SNET_CLUB_MARK_CHANGE_FB* pNetMsg = (GLMSG::SNET_CLUB_MARK_CHANGE_FB*) nmg;

    std::string strMsg = "";

    switch ( pNetMsg->emFB )
    {
    case EMCLUB_CHANGE_MARK_LIMIT_TIME:
        {
            strMsg = stringformat( ID2GAMEINTEXT("CLUB_CHANGE_MARK_LIMIT_TIME"), GLCONST_CHAR::dwCLUB_CHANGE_CLUB_MARK_LIMIT_TIME  );
            PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, strMsg );
        }
        break;
    };
}

void GLGaeaClient::MsgClubMarkChange(NET_MSG_GENERIC* nmg)
{
    GLMSG::SNET_CLUB_MARK_CHANGE_2CLT *pNetMsg = (GLMSG::SNET_CLUB_MARK_CHANGE_2CLT *) nmg;

    //	Note : 클럽 마크 변경.
    m_pMyClub->m_dwMarkVER = pNetMsg->dwMarkVER;
    memcpy(m_pMyClub->m_aryMark, pNetMsg->aryMark, sizeof(DWORD)*EMCLUB_MARK_SX*EMCLUB_MARK_SY);

    //	Note : 이미지에 적용.
    DWORD dwServerID = m_pCharacter->m_dwServerID;
    DxGlobalStage* pGlobalStage = GetGlobalStage();
    DxClubMan::GetInstance().SetClubData(
        pGlobalStage->GetD3dDevice(),
        dwServerID,
        m_pCharacter->m_ClubDbNum,
        pNetMsg->dwMarkVER,
        m_pMyClub->m_aryMark);

	// 이름 디스플레이 클럽 아이콘 변경
	GfxInterfaceBase::GetInstance()->ChangeClubIcon( m_pMyClub->GetReqClubId() );
}

std::string GLGaeaClient::ClubNewbieNotice() const
{
    return std::string(m_pMyClub->GetNoticeNewbie());
}

bool GLGaeaClient::ClubNewbieNoticeReq(const std::string& Notice)
{
    if (Notice.length() >= EMCLUB_NOTICE_LEN)
        return false;

    if (!m_pMyClub->IsValid())
        return false;

    if (!m_pMyClub->HaveNoticeChangeAuthority(m_pCharacter->CharDbNum()))
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("CLUB_NOT_HAVE_AUTHRITY"));
        return false;
    }

    NET_CLUB_NEWBIE_NOTICE_CA SendData;
    SendData.m_NoticeNewbie = Notice;

    msgpack::sbuffer SendBuffer;
    msgpack::pack(SendBuffer, SendData);

    NETSENDTOAGENT(NET_MSG_CLUB_NEWBIE_NOTICE_CA, SendBuffer);
    return true;
}

void GLGaeaClient::MsgClubNewbieNoticeAC(NET_MSG_GENERIC* nmg)
{
    // NET_MSG_CLUB_NEWBIE_NOTICE_AC
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) nmg;

    NET_CLUB_NEWBIE_NOTICE_AC RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return;

    m_pMyClub->SetNoticeNewbie(RecvData.m_NoticeNewbie);

    // '%1%'님이 신입생 소개글을 변경하였습니다.
    PrintMsgTextDlg(
        NS_UITEXTCOLOR::ENABLE,
            sc::string::format(
                ID2GAMEINTEXT("NET_CLUB_NEWBIE_NOTICE_AC"), RecvData.m_ChaName));

    // UI Refresh
	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_Club_NoticeChange );
}


bool GLGaeaClient::ReqClubDissolution()
{
    if (m_pCharacter->GetClubID() == CLUB_NULL)
        return false;

    if (!m_pMyClub->IsMaster(m_pCharacter->CharDbNum()))
        return false;

    if (m_pMyClub->IsAlliance())
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_DIS_FB_ALLIANCE"));
        return false;
    }

    if (m_pMyClub->GetAllBattleNum() > 0)
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_DIS_FB_CLUBBATTLE"));
        return false;
    }

	if ( m_pMyClub->IsRegDissolution() == true )
	{
		PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_DISSOLUTION_REQ_FB_AGAIN"));
		return false;		
	}

    GLMSG::SNET_CLUB_DISSOLUTION NetMsg;
    NETSENDTOAGENT(&NetMsg);

    return false;
}

bool GLGaeaClient::ReqClubDissolutionCancel ()
{
    if (!m_pMyClub->IsValid())
        return false;

    if (!m_pMyClub->IsMaster(m_pCharacter->CharDbNum()))
        return false;

    GLMSG::SNET_CLUB_DISSOLUTION NetMsg;
    NetMsg.bCANCEL = true;
    NETSENDTOAGENT(&NetMsg);

    return true;
}

bool GLGaeaClient::ReqClubJoinAns(DWORD dwMaster, bool bOK)
{
    GLMSG::NET_CLUB_MEMBER_REQ_ANS_CA NetMsg;
    NetMsg.dwMaster = dwMaster;
    NetMsg.bOK = bOK;

//    NETSENDTOFIELD(&NetMsg);
	NETSENDTOAGENT(&NetMsg);

    return true;
}

//! 클럽 가입제한 시간 리셋
void GLGaeaClient::ResetClubRejoinTime(const std::string& ChaName)
{
    NET_CLUB_JOIN_TIME_RESET_CA SendData(ChaName);
    msgpack::sbuffer SendBuffer;
    msgpack::pack(SendBuffer, SendData);
    NETSENDTOAGENT(NET_MSG_CLUB_JOIN_TIME_RESET_CA, SendBuffer);
}

void GLGaeaClient::MsgClubJoinTimeResetAC(NET_MSG_GENERIC* pMsg)
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) pMsg;
    NET_CLUB_JOIN_TIME_RESET_AC RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return;
    
    // 604800 = 7day (sec)
    __time64_t NewSecedeTime = sc::time::GetCurrentTime() - 604800;
    std::string Msg;
    NET_CLUB_JOIN_TIME_RESET_AC::RESULT Result = static_cast<NET_CLUB_JOIN_TIME_RESET_AC::RESULT> (RecvData.m_Result);
    switch (Result)
    {
    case NET_CLUB_JOIN_TIME_RESET_AC::SUCCESS:
        Msg = sc::string::format(
            "[%1%] club secede time reset success.",
            RecvData.m_ChaName);
        m_pCharacter->SetClubSecedeTime(NewSecedeTime);
        break;
    case NET_CLUB_JOIN_TIME_RESET_AC::NOT_EXIST_CHAR:
        Msg = sc::string::format(
            "[%1%] club secede time reset failed. character not online or not exist name.",
            RecvData.m_ChaName);
        break;
    case NET_CLUB_JOIN_TIME_RESET_AC::NOT_PERMISSION:
        Msg = sc::string::format(
            "[%1%] club secede time reset failed. request character have not permission.",
            RecvData.m_ChaName);
        break;
    default:
        Msg = sc::string::format(
            "[%1%] club secede time reset failed. unknown error. %2%",
            RecvData.m_ChaName,
            static_cast<int> (Result));
        break;
    }

    PrintMsgText(NS_UITEXTCOLOR::DEFAULT, Msg);
}

void GLGaeaClient::ResetClubAllianceTime(const std::string& ClubName)
{
    std::string ReqClubName(ClubName);
    if (ReqClubName.empty())
        ReqClubName = m_pMyClub->Name();

    NET_CLUB_ALLIANCE_TIME_RESET_CA SendData(ReqClubName);
    msgpack::sbuffer SendBuffer;
    msgpack::pack(SendBuffer, SendData);
    NETSENDTOAGENT(NET_MSG_CLUB_ALLIANCE_TIME_RESET_CA, SendBuffer);
}

void GLGaeaClient::MsgClubAllianceTimeResetAC(NET_MSG_GENERIC* pMsg)
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) pMsg;
    NET_CLUB_ALLIANCE_TIME_RESET_AC RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return;

    // 604800 = 7day (sec)
    __time64_t NewTime = sc::time::GetCurrentTime() - 604800;
    std::string Msg;
    NET_CLUB_ALLIANCE_TIME_RESET_AC::RESULT Result = static_cast<NET_CLUB_ALLIANCE_TIME_RESET_AC::RESULT> (RecvData.m_Result);
    switch (Result)
    {
    case NET_CLUB_ALLIANCE_TIME_RESET_AC::SUCCESS:
        m_pMyClub->SetAllianceDis(NewTime);
        m_pMyClub->SetAllianceSec(NewTime);
        Msg = sc::string::format(
            "[%1%] club alliance time reset success.",
            RecvData.m_ClubName);
        break;

    case NET_CLUB_ALLIANCE_TIME_RESET_AC::NOT_EXIST_CLUB:
        Msg = sc::string::format(
            "[%1%] club alliance time reset failed. Not exist club.",
            RecvData.m_ClubName);
        break;

    case NET_CLUB_ALLIANCE_TIME_RESET_AC::HAVE_NOT_PERMISSION:
        Msg = sc::string::format(
            "[%1%] club alliance time reset failed. Have not permission.",
            RecvData.m_ClubName);
        break;

    default:
        Msg = sc::string::format(
            "[%1%] club alliance time reset failed. Error code %2%.",
            RecvData.m_ClubName,
            static_cast<int> (Result));
        break;
    }

    PrintMsgText(NS_UITEXTCOLOR::DEFAULT, Msg);
}

void GLGaeaClient::MsgClubId2MasterNameAnsAC(NET_MSG_GENERIC* pMsg)
{
	NET_MSG_PACK* pPacket = (NET_MSG_PACK*) pMsg;

	GLMSG::SNET_CLUB_ID_2MASTERNAME_ANS_AC RecvData;
	if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
		return;

	Competition::GetInstance().SetClubMasterClubName(RecvData.m_vecMasterName, RecvData.m_vecClubName);
	Competition::GetInstance().SendCDMRankInfo();
}


void GLGaeaClient::ClubNameReq()
{
    if( m_TempClubReqSendData.m_vecClubDbNum.size() <= 0 )
        return;

    if( m_TempClubReqSendData.m_vecGaeaId.size() <= 0 )
        return;

    // 패킷 분리 : DropPc들의 정보패킷전송이 전부 다 끝나고 난 후에 오는 패킷 ( 그들에 대한 클럽이름 정보에 대해서 요청을 한다. )
    msgpack::sbuffer SendBuffer;
    msgpack::pack(SendBuffer, m_TempClubReqSendData);
    NETSENDTOAGENT(NET_MSG_REQ_DROP_CHAR_CLUB_NAME_CA, SendBuffer);
    m_TempClubReqSendData.Clear();
}

std::string GLGaeaClient::GetOhterCharClubName( const char* szCharNameCharName )
{
	std::tr1::shared_ptr< GLCharClient > spChar = m_pLandMClient->FindChar( szCharNameCharName );
	if( NULL == spChar )
		return false;

	return spChar->GetClubName();
}

void GLGaeaClient::MsgClubStorageUseFB( NET_MSG_GENERIC* pMsg )
{
	NET_MSG_PACK* pPacekt = ( NET_MSG_PACK* )pMsg;
	GLMSG::SNET_CLUB_STORAGE_USER_FB Data;
	if ( !msgpack::BufferToObject( pPacekt->DataBuffer(), pPacekt->DataSize(), Data ))
		return;

	PrintMsgTextDlg( NS_UITEXTCOLOR::NEGATIVE, sc::string::format( ID2GAMEINTEXT( "CLUB_STORAGE_USE_FB" ), Data.ChaName.c_str() ) );
}

void GLGaeaClient::MsgClubStorageMoneyUseFB( NET_MSG_GENERIC* pMsg )
{
	NET_MSG_PACK* pPacekt = ( NET_MSG_PACK* )pMsg;
	GLMSG::SNET_CLUB_STORAGE_USER_FB Data;
	if ( !msgpack::BufferToObject( pPacekt->DataBuffer(), pPacekt->DataSize(), Data ))
		return;

	PrintMsgTextDlg( NS_UITEXTCOLOR::NEGATIVE, sc::string::format( ID2GAMEINTEXT( "CLUB_STORAGE_MONEY_USE_FB" ), Data.ChaName.c_str() ) );
}

void GLGaeaClient::MsgClubRename( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_CLUB_RENAME_2CLT *pNetMsg = ( GLMSG::SNET_CLUB_RENAME_2CLT* ) nmg;

	if ( pNetMsg->szClubName && m_pMyClub )
	{
		m_pMyClub->SetName( pNetMsg->szClubName );
		/*m_pCharacter->SetNickName( pNetMsg->szClubName );*/
	}

	PrintMsgTextDlg( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT( "CLUB_LOG_33" ) );

	// 이름표 변경					
	/*CROWREN sDISP_NAME;
	sDISP_NAME.SetData( this, m_pCharacter );

	GfxInterfaceBase::GetInstance()->AddNameDisplay( sDISP_NAME );*/

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Club_UpdateName, "-s", pNetMsg->szClubName );
}

void GLGaeaClient::MsgClubRenameAlliance( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_CLUB_RENAME_2ALLIANCE *pNetMsg = ( GLMSG::SNET_CLUB_RENAME_2ALLIANCE* ) nmg;

	if ( pNetMsg->szClubName && m_pMyClub )
	{
		m_pMyClub->UpdateAllianceClubName( pNetMsg->ClubDbNum, pNetMsg->szClubName );

		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_Club_UpdateAlliance );
	}
}

void GLGaeaClient::PrintClubRenameFB( int nResult )
{
	switch ( nResult )
	{
	case CLUB_RENAME_FB_WRONG_ITEM:
		{
			PrintMsgTextDlg( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT( "CLUB_RENAME_FB_WRONG_ITEM" ) );
		}
		break;
	case CLUB_RENAME_FB_AUTH:
		{
			PrintMsgTextDlg( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT( "CLUB_RENAME_FB_AUTH" ) );
		}
		break;
	case CLUB_RENAME_FB_NONAME:
		{
			PrintMsgTextDlg( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT( "CLUB_RENAME_FB_NONAME" ) );
		}
		break;
	case CLUB_RENAME_FB_TOO_SHORT:
		{
			PrintMsgTextDlg( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT( "CLUB_RENAME_FB_TOO_SHORT" ) );
		}
		break;
	case CLUB_RENAME_FB_TOO_LONG:
		{
			PrintMsgTextDlg( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT( "CLUB_RENAME_FB_TOO_LONG" ) );
		}
		break;
	case CLUB_RENAME_FB_BLANK:
		{
			PrintMsgTextDlg( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT( "CLUB_RENAME_FB_BLANK" ) );
		}
		break;
	case CLUB_RENAME_FB_BAD_NAME:
		{
			PrintMsgTextDlg( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT( "CLUB_RENAME_FB_BAD_NAME" ) );
		}
		break;
	case CLUB_RENAME_FB_NAME_DUP:
		{
			PrintMsgTextDlg( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT( "CLUB_RENAME_FB_NAME_DUP" ) );
		}
		break;
	case CLUB_RENAME_FB_CLUB_BATTLE:
		{
			PrintMsgTextDlg( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT( "CLUB_RENAME_FB_CLUB_BATTLE" ) );
		}
		break;
	}
}

void GLGaeaClient::MsgClubRenameFB( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_CLUB_RENAME_FB *pNetMsg = ( GLMSG::SNET_CLUB_RENAME_FB* ) nmg;
	PrintClubRenameFB( pNetMsg->nResult );
}
