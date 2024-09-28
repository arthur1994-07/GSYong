#include "pch.h"
#include "../SigmaCore/MsgPack/MinMsgPack.h"
#include "../SigmaCore/String/StringFormat.h"
#include "../SigmaCore/Json/MinJson.h"
#include "../SigmaCore/Util/DateTime.h"

#include "../EngineLib/DxTools/DxClubMan.h"
#include "../EngineLib/DxTools/RENDERPARAM.h"
#include "../EngineLib/GUInterface/GameTextControl.h"
#include "../EngineLib/GUInterface/UITextControl.h"

#include "../MfcExLib/RanFilter.h"

#include "../RanLogic/Msg/ClubMsg.h"
#include "../RanLogic/Msg/JsonMsgDefine.h"
#include "../RanLogic/Club/ClubLogManClient.h"

#include "./Club/GLClubClient.h"
#include "./Land/GLLandManClient.h"
#include "./Stage/DxGlobalStage.h"
#include "./GLGaeaClient.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

//! 동맹 클럽장 정보
bool GLGaeaClient::IsChief() const
{
	return m_pMyClub->IsChief();
}

// 동맹 클럽장 아이디 [12/11/2012 gbgim]
const DWORD GLGaeaClient::GetChiefID() const
{
	return m_pMyClub->GetChiefID();
}

//! 동맹 클럽 정보
void GLGaeaClient::GetClubAlliance(std::vector<GLCLUBALLIANCE>& vData) const
{    
    m_pMyClub->GetAllianceData(vData);
    
    /*
    // club mark image
    const DxClubMan::DXDATA& sMarkData = DxClubMan::GetInstance().GetClubData(
        pd3dDevice,
        m_pCharacter->m_dwServerID,
        ,
        );
    */

}

LuaTable GLGaeaClient::GetAllianceClubInfo()
{
	LuaTable tbAlliance( GLWidgetScript::GetInstance().GetLuaState() );

	std::vector< GLCLUBALLIANCE > vecData;
	m_pMyClub->GetAllianceData( vecData );

	std::vector< GLCLUBALLIANCE >::iterator iter = vecData.begin();
	for( int i = 1; iter != vecData.end(); ++iter, ++i )
	{
		LuaTable tbData( GLWidgetScript::GetInstance().GetLuaState() );
		tbData.set( 1, iter->m_dwID );
		tbData.set( 2, iter->GetClubName() );
		tbData.set( 3, iter->GetClubMasterName() );
		tbData.set( 4, iter->m_dwID == GetChiefID() );

		tbAlliance.set( i, tbData );
	}

	return tbAlliance;
}

LuaTable GLGaeaClient::GetHostileClubInfo()
{
	GLGaeaClient* pClient = GLWidgetScript::GetInstance().m_pGaeaClient;

	LuaTable tbHotile( GLWidgetScript::GetInstance().GetLuaState() );

	std::vector< GLCLUBBATTLE > vecData;
	m_pMyClub->GetClubBattleData( vecData );

	std::vector< GLCLUBBATTLE >::iterator iter = vecData.begin();
	for( int i = 1; iter != vecData.end(); ++iter, ++i )
	{
		CString strStart, strEnd, strSurrender;

		// 시작 시간 -------------------------------
		// 서버 타임으로 변환( TimeZone 계산 )
		CTime cTime = iter->m_tStartTime;
		pClient->GetConvertServerTime( cTime );

		strStart = sc::string::format( ID2GAMEINTEXT("CLUB_BATTLE_INFO",1), cTime.GetYear (), cTime.GetMonth (), 
			cTime.GetDay (), cTime.GetHour (), cTime.GetMinute () ).c_str();

		// 종료 시간 -------------------------------
		// 서버 타임으로 변환( TimeZone 계산 )
		cTime = iter->m_tEndTime;
		pClient->GetConvertServerTime( cTime );

		strEnd = sc::string::format(  ID2GAMEINTEXT("CLUB_BATTLE_INFO",2), cTime.GetYear (), cTime.GetMonth (), 
			cTime.GetDay (), cTime.GetHour (), cTime.GetMinute () ).c_str();

		// 항복 가능 시간 ---------------------------
		// 서버 타임으로 변환( TimeZone 계산 )
		cTime = iter->m_tStartTime;		
		pClient->GetConvertServerTime( cTime );

		// 시작시간 + a
		CTimeSpan tDisTimeSpan( 0, 0, GLCONST_CHAR::dwCLUB_BATTLE_DIS_TIME, 0 );
		cTime += tDisTimeSpan;

		strSurrender = sc::string::format( ID2GAMEINTEXT("CLUB_BATTLE_INFO",3), cTime.GetYear (), cTime.GetMonth (), 
			cTime.GetDay (), cTime.GetHour (), cTime.GetMinute () ).c_str();

		LuaTable tbData( GLWidgetScript::GetInstance().GetLuaState() );
		tbData.set( 1, iter->m_dwCLUBID );
		tbData.set( 2, iter->m_szClubName );
		tbData.set( 3, iter->m_wKillPoint );
		tbData.set( 4, iter->m_wDeathPoint );
		tbData.set( 5, strStart.GetBuffer() );
		tbData.set( 6, strEnd.GetBuffer() );
		tbData.set( 7, strSurrender.GetBuffer() );

		tbHotile.set( i, tbData );
	}

	return tbHotile;
}

//! 클럽 동맹 해지 요청.
bool GLGaeaClient::ReqClubAllianceDel_Targetname(const char* szClubName)
{
	if (!m_pMyClub->IsValid())
	{
		PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_DEL_FB_NOTMASTER"));
		return false;
	}
	
	std::vector<GLCLUBALLIANCE> vData;
	m_pMyClub->GetAllianceData(vData);
	int Size(vData.size());

	for(int i = 0; i < Size; ++i)
	{
		if ( strcmp(szClubName, vData[i].GetClubName()) == 0 )
		{
			ReqClubAllianceDel(vData[i].m_dwID);
			break;
		}
	}
}

bool GLGaeaClient::ReqClubAllianceDel(DWORD dwClubID)
{
    if (!m_pMyClub->IsValid())
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_DEL_FB_NOTMASTER"));
        return false;
    }
    
    //if (!m_pMyClub->IsMaster(m_pCharacter->CharDbNum()))
    //{
    //    PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_DEL_FB_NOTMASTER"));
    //    return false;
    //}

    if (!m_pMyClub->HaveAllianceDisAuthority(m_pCharacter->CharDbNum()))
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("CLUB_NOT_HAVE_AUTHRITY"));
        return false;
    }    

    if (!m_pMyClub->IsAlliance())
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_DEL_FB_NOTCHIEF"));
        return false;
    }

	// 클럽 동맹장을 체크하는 코드 해제
    /*if (!m_pMyClub->IsChief())
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_DEL_FB_NOTCHIEF"));
        return false;
    }*/

    if (m_pMyClub->GetAllBattleNum() > 0)
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_DEL_FB_BATTLE"));
        return false;
    }

    if ( m_pMyClub->m_DbNum == dwClubID )
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_DEL_FB_NO_MYCLUB"));
        return false;
    }

    GLMSG::SNET_CLUB_ALLIANCE_DEL_REQ NetMsg(dwClubID);
    NETSENDTOAGENT(&NetMsg);

    return true;
}

//! 클럽 동맹장이 아닐 경우 탈퇴 요청.
bool GLGaeaClient::ReqClubAllianceWithdraw(DWORD dwClubID)
{
	// 클럽 체크
	if (!m_pMyClub->IsValid())
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_DEL_FB_NOTMASTER"));
        return false;
    }

	// 권한 체크
    if (!m_pMyClub->HaveAllianceDisAuthority(m_pCharacter->CharDbNum()))
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("CLUB_NOT_HAVE_AUTHRITY"));
        return false;
    }    

	// 동맹 인지 체크
    if (!m_pMyClub->IsAlliance())
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_DEL_FB_NOTCHIEF"));
        return false;
    }

    if (m_pMyClub->GetAllBattleNum() > 0)
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_DEL_FB_BATTLE"));
        return false;
    }

	// 자신의 클럽 체크
    if ( m_pMyClub->m_DbNum != dwClubID )
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_WITHDRAW_FB_NO_MYCLUB"));
        return false;
    }

    GLMSG::SNET_CLUB_ALLIANCE_DEL_REQ NetMsg(dwClubID);
    NETSENDTOAGENT(&NetMsg);

    return true;
}

//! 클럽 동맹 탈퇴 요청.
bool GLGaeaClient::ReqClubAllianceSec()
{
    if (!m_pMyClub->IsValid())
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_SEC_FB_NOTMASTER"));
        return false;
    }

    //if (!m_pMyClub->IsMaster(m_CharDbNum))
    //{
    //    PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_SEC_FB_NOTMASTER") );
    //    return false;
    //}

    if (!m_pMyClub->HaveAllianceDisAuthority(m_pCharacter->CharDbNum()))
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("CLUB_NOT_HAVE_AUTHRITY"));
        return false;
    }

    if (!m_pMyClub->IsAlliance())
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_SEC_FB_ALLIANCE"));
        return false;
    }

	// 클럽 동맹장 체크 해제
    /*if (m_pMyClub->IsChief())
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_SEC_FB_FAIL"));
        return false;
    }*/

    if (m_pMyClub->GetAllBattleNum() > 0)
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_SEC_FB_BATTLE"));
        return false;
    }

    GLMSG::SNET_CLUB_ALLIANCE_SEC_REQ NetMsg;
    NETSENDTOAGENT(&NetMsg);
    return true;
}

// 동맹 요청 관련 서버 메시지 반응
void GLGaeaClient::MsgClubAllianceReqFB( NET_MSG_GENERIC* nmg ) 
{
    GLMSG::SNET_CLUB_ALLIANCE_REQ_FB *pNetMsg = (GLMSG::SNET_CLUB_ALLIANCE_REQ_FB *)nmg;

    switch ( pNetMsg->emFB )
    {
    case EMCLUB_ALLIANCE_REQ_FB_FAIL:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_REQ_FB_FAIL") );
        break;
    case EMCLUB_ALLIANCE_REQ_FB_OK:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::ENABLE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_REQ_FB_OK") );
        break;
    case EMCLUB_ALLIANCE_REQ_FB_NOTMASTER:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_REQ_FB_NOTMASTER") );
        break;
    case EMCLUB_ALLIANCE_REQ_FB_NOTCHIEF:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_REQ_FB_NOTCHIEF") );
        break;
    case EMCLUB_ALLIANCE_REQ_FB_TARNOTMASTER:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_REQ_FB_TARNOTMASTER") );
        break;
	case EMCLUB_ALLIANCE_REQ_FB_ALREADY:
		PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_REQ_FB_ALREADY") );
		break;
	case EMCLUB_ALLIANCE_REQ_FB_SAMECLUB:
		PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_REQ_FB_SAMECLUB") );
		break;
    case EMCLUB_ALLIANCE_REQ_FB_DISSOLUTION:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_REQ_FB_DISSOLUTION") );
        break;
    case EMCLUB_ALLIANCE_REQ_FB_DISSOLUTION2:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_REQ_FB_DISSOLUTION2") );
        break;
    case EMCLUB_ALLIANCE_REQ_FB_MAX:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_REQ_FB_MAX") );
        break;
    case EMCLUB_ALLIANCE_REQ_FB_REFUSE:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_REQ_FB_REFUSE") );
        break;

    case EMCLUB_ALLIANCE_REQ_FB_BOTHGUID:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_REQ_FB_BOTHGUID") );
        break;

    case EMCLUB_ALLIANCE_REQ_FB_GUIDMAP:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_REQ_FB_GUIDMAP") );
        break;

    case EMCLUB_ALLIANCE_REQ_FB_DISTIME:
        {
            CTime cTime(pNetMsg->tBlock);

            //	서버 타임으로 변환( TimeZone 계산 )
            GetConvertServerTime( cTime );

            std::string strExpireDate = sc::string::format(
                ID2GAMEWORD("ITEM_EXPIRE_DATE"),
                cTime.GetYear(),
                cTime.GetMonth(),
                cTime.GetDay(),
                cTime.GetHour(),
                cTime.GetMinute());

            PrintMsgTextDlg(
                NS_UITEXTCOLOR::NEGATIVE,
                sc::string::format(
                ID2GAMEINTEXT("EMCLUB_ALLIANCE_REQ_FB_DISTIME"),
                strExpireDate.c_str()));
        }
        break;

    case EMCLUB_ALLIANCE_REQ_FB_SECTIME:
        {
            CTime cTime(pNetMsg->tBlock);

            //	서버 타임으로 변환( TimeZone 계산 )
            GetConvertServerTime( cTime );

            std::string strExpireDate = sc::string::format(
                ID2GAMEWORD("ITEM_EXPIRE_DATE"),
                cTime.GetYear(),
                cTime.GetMonth(),
                cTime.GetDay(),
                cTime.GetHour(),
                cTime.GetMinute());				

            PrintMsgTextDlg(
                NS_UITEXTCOLOR::NEGATIVE,
                sc::string::format(
                ID2GAMEINTEXT("EMCLUB_ALLIANCE_REQ_FB_SECTIME"),
                strExpireDate.c_str()));
        }
        break;

    case EMCLUB_ALLIANCE_REQ_FB_TOSECTIME:
        {
            CTime cTime(pNetMsg->tBlock);

            //	서버 타임으로 변환( TimeZone 계산 )
            GetConvertServerTime( cTime );

            std::string strExpireDate = sc::string::format(
                ID2GAMEWORD("ITEM_EXPIRE_DATE"),
                cTime.GetYear(),
                cTime.GetMonth(),
                cTime.GetDay(),
                cTime.GetHour(),
                cTime.GetMinute());

            PrintMsgTextDlg(
                NS_UITEXTCOLOR::NEGATIVE,
                sc::string::format(
                ID2GAMEINTEXT("EMCLUB_ALLIANCE_REQ_FB_TOSECTIME"),
                strExpireDate.c_str()));
        }
        break;

    case EMCLUB_ALLIANCE_REQ_FB_OTHERSCHOOL:
        {
            PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_REQ_FB_OTHERSCHOOL") );
        }
        break;
    case EMCLUB_ALLIANCE_REQ_FB_CLUBBATTLE:
        {	
            PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_REQ_FB_CLUBBATTLE") );
        }
        break;
    case EMCLUB_ALLIANCE_REQ_FB_TARCLUBBATTLE:
        {
            PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_REQ_FB_TARCLUBBATTLE") );
        }
        break;

    case EMCLUB_ALLIANCE_REQ_FB_NO_AUTHORITY:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_REQ_FB_NO_AUTHORITY") );
        break;
    };
}

// 동맹 해지 관련 서버 메시지 반응
void GLGaeaClient::MsgClubAllianceDelFB( NET_MSG_GENERIC* nmg )
{
    GLMSG::SNET_CLUB_ALLIANCE_DEL_FB *pNetMsg = (GLMSG::SNET_CLUB_ALLIANCE_DEL_FB *)nmg;

    switch (pNetMsg->emFB)
    {
    case EMCLUB_ALLIANCE_DEL_FB_FAIL:
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_DEL_FB_FAIL"));
        break;
    case EMCLUB_ALLIANCE_DEL_FB_OK:
        PrintMsgTextDlg(NS_UITEXTCOLOR::ENABLE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_DEL_FB_OK"));
        break;
	case EMCLUB_ALLIANCE_DEL_FB_NOTCHIEF_OK:
		PrintMsgTextDlg(NS_UITEXTCOLOR::ENABLE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_DEL_FB_NOTCHIEF_OK"));
		break;
    case EMCLUB_ALLIANCE_DEL_FB_NOTMASTER:
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_DEL_FB_NOTMASTER"));
        break;
    case EMCLUB_ALLIANCE_DEL_FB_NOTCHIEF:
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_DEL_FB_NOTCHIEF"));
        break;
    case EMCLUB_ALLIANCE_DEL_FB_GUIDMAP:
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_DEL_FB_GUIDMAP"));
        break;
    case EMCLUB_ALLIANCE_DEL_FB_BATTLE:
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_DEL_FB_BATTLE"));
        break;
    case EMCLUB_ALLIANCE_DEL_FB_NO_AUTH:
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("CLUB_NOT_HAVE_AUTHRITY"));
        break;
    case EMCLUB_ALLIANCE_DEL_FB_NO_MYCLUB:
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_DEL_FB_NO_MYCLUB"));
        break;
    default:
        PrintMsgTextDlg(
            NS_UITEXTCOLOR::NEGATIVE,
            sc::string::format("unknown message. MsgClubAllianceDelFB %1%", static_cast<int> (pNetMsg->emFB)));
        break;
    };
}


void GLGaeaClient::MsgClubAllianceSecFB( NET_MSG_GENERIC* nmg )
{
    GLMSG::SNET_CLUB_ALLIANCE_SEC_FB *pNetMsg = (GLMSG::SNET_CLUB_ALLIANCE_SEC_FB *)nmg;

    switch ( pNetMsg->emFB )
    {
    case EMCLUB_ALLIANCE_SEC_FB_FAIL:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_SEC_FB_FAIL") );
        break;
    case EMCLUB_ALLIANCE_SEC_FB_OK:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::ENABLE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_SEC_FB_OK") );
        break;
    case EMCLUB_ALLIANCE_SEC_FB_NOTMASTER:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_SEC_FB_NOTMASTER") );
        break;
    case EMCLUB_ALLIANCE_SEC_FB_ALLIANCE:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_SEC_FB_ALLIANCE") );
        break;
    case EMCLUB_ALLIANCE_SEC_FB_GUIDMAP:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_SEC_FB_GUIDMAP") );
        break;
    case EMCLUB_ALLIANCE_SEC_FB_BATTLE:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_SEC_FB_BATTLE") );
        break;				
    };
}


void GLGaeaClient::MsgClubAllianceDisFB( NET_MSG_GENERIC* nmg )
{
    GLMSG::SNET_CLUB_ALLIANCE_DIS_FB *pNetMsg = (GLMSG::SNET_CLUB_ALLIANCE_DIS_FB *)nmg;

    switch ( pNetMsg->emFB )
    {
    case EMCLUB_ALLIANCE_DIS_FB_FAIL:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_DIS_FB_FAIL") );
        break;
    case EMCLUB_ALLIANCE_DIS_FB_OK:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::ENABLE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_DIS_FB_OK") );
        break;
    case EMCLUB_ALLIANCE_DIS_FB_NOTMASTER:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_DIS_FB_NOTMASTER") );
        break;
    case EMCLUB_ALLIANCE_DIS_FB_NOTCHIEF:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_DIS_FB_NOTCHIEF") );
        break;
    case EMCLUB_ALLIANCE_DIS_FB_GUIDMAP:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_DIS_FB_GUIDMAP") );
        break;
    case EMCLUB_ALLIANCE_DIS_FB_BATTLE:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_DIS_FB_BATTLE") );
        break;				
    };
}

void GLGaeaClient::MsgClubAllianceReqAsk(NET_MSG_GENERIC* nmg)
{
    GLMSG::NET_CLUB_ALLIANCE_REQ_ASK_AC* pNetMsg = (GLMSG::NET_CLUB_ALLIANCE_REQ_ASK_AC*) nmg;

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_Club_AllianceJoinAsk,
		"-dw, -s", pNetMsg->m_ChiefCharID, pNetMsg->m_szChiefName );
}

void GLGaeaClient::MsgClubAllianceAdd( NET_MSG_GENERIC* nmg )
{
    GLMSG::NET_CLUB_ALLIANCE_ADD_AC *pNetMsg = (GLMSG::NET_CLUB_ALLIANCE_ADD_AC *)nmg;

    m_pMyClub->m_dwAlliance = pNetMsg->dwChiefClubID;
    m_pMyClub->AddAlliance(pNetMsg->dwChiefClubID,  pNetMsg->szChiefClub,  pNetMsg->m_szChiefClubMasterName,  pNetMsg->m_ChiefMarkVersion);
    m_pMyClub->AddAlliance(pNetMsg->dwIndianClubID, pNetMsg->szIndianClub, pNetMsg->m_szIndianClubMasterName, pNetMsg->m_IndianMarkVersion);

    m_pMyClub->m_dwAllianceBattleWin = pNetMsg->dwAllianceBattleWin;
    m_pMyClub->m_dwAllianceBattleLose = pNetMsg->dwAllianceBattleLose;
    m_pMyClub->m_dwAllianceBattleDraw = pNetMsg->dwAllianceBattleDraw;

    PrintMsgTextDlg(
        NS_UITEXTCOLOR::NEGATIVE,
        sc::string::format(
            ID2GAMEINTEXT("ALLIANCE_ADD"),
            pNetMsg->szChiefClub,
            pNetMsg->szIndianClub));

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_Club_UpdateMember );
    
	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_Club_UpdateAlliance );
}


void GLGaeaClient::MsgClubAllianceDel( NET_MSG_GENERIC* nmg )
{
    GLMSG::SNET_CLUB_ALLIANCE_DEL_CLT *pNetMsg = (GLMSG::SNET_CLUB_ALLIANCE_DEL_CLT *)nmg;

    if (m_pMyClub->DbNum() == pNetMsg->dwDelClubID)
    {
        // 삭제될 클럽이 자신이면 모든 동맹정보 삭제함.
        m_pMyClub->ClearAlliance();
    }    
    else
    {
        // 아니면 동맹들 중에서 찾아서 삭제함.
        m_pMyClub->DelAlliance(pNetMsg->dwDelClubID);        
    }			

    PrintMsgTextDlg(
        NS_UITEXTCOLOR::NEGATIVE,
        sc::string::format(ID2GAMEINTEXT("ALLIANCE_DEL"), pNetMsg->szDelClub));

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_Club_UpdateMember );
    
	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_Club_UpdateAlliance );
}

void GLGaeaClient::MsgClubAllianceDis( NET_MSG_GENERIC* nmg )
{
    GLMSG::SNET_CLUB_ALLIANCE_DIS_CLT *pNetMsg = (GLMSG::SNET_CLUB_ALLIANCE_DIS_CLT *)nmg;

    PrintMsgTextDlg(
        NS_UITEXTCOLOR::NEGATIVE,
        sc::string::format(ID2GAMEINTEXT("ALLIANCE_DIS"), pNetMsg->szChiefClub));

    m_pMyClub->ClearAlliance();

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_Club_UpdateMember );
    
	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_Club_UpdateAlliance );
}


void GLGaeaClient::MsgClubAllianceBattleSubmissionReqFB(NET_MSG_GENERIC* nmg)
{
    GLMSG::SNET_ALLIANCE_BATTLE_SUBMISSION_REQ_FB* pNetMsg = (GLMSG::SNET_ALLIANCE_BATTLE_SUBMISSION_REQ_FB*) nmg;

    switch (pNetMsg->emFB)
    {
    case EMALLIANCE_BATTLE_SUBMISSION_FB_FAIL:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_SUBMISSION_FB_FAIL") );
        break;
    case EMALLIANCE_BATTLE_SUBMISSION_FB_OK:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::ENABLE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_SUBMISSION_FB_OK") );
        break;
    case EMALLIANCE_BATTLE_SUBMISSION_FB_NOTMASTER:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_SUBMISSION_FB_NOTMASTER") );
        break;
    case EMALLIANCE_BATTLE_SUBMISSION_FB_GUIDMAP:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_SUBMISSION_FB_GUIDMAP") );
        break;
    case EMALLIANCE_BATTLE_SUBMISSION_FB_NOBATTLE:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_SUBMISSION_FB_NOBATTLE") );
        break;
    case EMALLIANCE_BATTLE_SUBMISSION_FB_DISTIME:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_SUBMISSION_FB_DISTIME") );
        break;
    case EMALLIANCE_BATTLE_SUBMISSION_FB_CDMMAP:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_SUBMISSION_FB_CDMMAP") );
        break;
    case EMALLIANCE_BATTLE_SUBMISSION_FB_NO_AUTHORITY:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("CLUB_NOT_HAVE_AUTHRITY") );
        break;
    default:
        PrintMsgTextDlg(
            NS_UITEXTCOLOR::NEGATIVE,
            sc::string::format(
            "Unknown type. MsgClubAllianceBattleSubmissionReqFB %1%", pNetMsg->emFB));
        break;
    }
}

void GLGaeaClient::MsgClubAllianceBattleArmisticeReqAsk(NET_MSG_GENERIC* nmg)
{
    GLMSG::SNET_ALLIANCE_BATTLE_ARMISTICE_REQ_ASK* pNetMsg = (GLMSG::SNET_ALLIANCE_BATTLE_ARMISTICE_REQ_ASK*) nmg;

    GLCLUBBATTLE* pClubBattle = m_pMyClub->GetClubBattle(pNetMsg->dwClubID);
	if( NULL != pClubBattle )
	{
		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_Club_ArmisticeAsk,
			"-dw, -s, -dw, -b",
			pNetMsg->dwClubID,
			pClubBattle->m_szClubName,
			pNetMsg->m_ReqChaDbNum,
			true );
	}
}

void GLGaeaClient::MsgClubAllianceBattleArmisticeReqFB( NET_MSG_GENERIC* nmg ) 
{
    GLMSG::SNET_ALLIANCE_BATTLE_ARMISTICE_REQ_FB *pNetMsg = (GLMSG::SNET_ALLIANCE_BATTLE_ARMISTICE_REQ_FB *)nmg;

    switch ( pNetMsg->emFB )
    {
    case EMALLIANCE_BATTLE_ARMISTICE_FB_FAIL:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_ARMISTICE_FB_FAIL") );
        break;
    case EMALLIANCE_BATTLE_ARMISTICE_FB_OK:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::ENABLE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_ARMISTICE_FB_OK") );
        break;
    case EMALLIANCE_BATTLE_ARMISTICE_FB_NOTMASTER:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_ARMISTICE_FB_NOTMASTER") );
        break;
    case EMALLIANCE_BATTLE_ARMISTICE_FB_OFFMASTER:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_ARMISTICE_FB_OFFMASTER") );
        break;
    case EMALLIANCE_BATTLE_ARMISTICE_FB_REFUSE:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_ARMISTICE_FB_REFUSE") );
        break;
    case EMALLIANCE_BATTLE_ARMISTICE_FB_GUIDMAP:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_ARMISTICE_FB_GUIDMAP") );
        break;
    case EMALLIANCE_BATTLE_ARMISTICE_FB_NOBATTLE:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_ARMISTICE_FB_NOBATTLE") );
        break;
    case EMALLIANCE_BATTLE_ARMISTICE_FB_DISTIME:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_ARMISTICE_FB_DISTIME") );
        break;
    case EMALLIANCE_BATTLE_ARMISTICE_FB_CDMMAP:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_ARMISTICE_FB_CDMMAP") );
        break;
    case EMALLIANCE_BATTLE_NO_AUTHORITY:
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("CLUB_NOT_HAVE_AUTHRITY"));
        break;
    default:
        PrintMsgTextDlg(
            NS_UITEXTCOLOR::NEGATIVE,
            sc::string::format(
            "Unknown type. MsgClubAllianceBattleArmisticeReqFB %1%", pNetMsg->emFB));
        break;
    }
}

void GLGaeaClient::MsgClubAllianceBattleReqAsk( NET_MSG_GENERIC* nmg ) 
{
    GLMSG::SNET_ALLIANCE_BATTLE_REQ_ASK* pNetMsg = (GLMSG::SNET_ALLIANCE_BATTLE_REQ_ASK*) nmg;
	
	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_Club_BattleAsk,
		"-dw, -s, -dw, -b",
		pNetMsg->dwClubCharID,
		pNetMsg->szClubName,
		pNetMsg->dwBattleTime,
		true );
}

void GLGaeaClient::MsgClubAllianceBattleReqFB( NET_MSG_GENERIC* nmg ) 
{
    GLMSG::SNET_ALLIANCE_BATTLE_REQ_FB *pNetMsg = (GLMSG::SNET_ALLIANCE_BATTLE_REQ_FB *)nmg;

    std::string strText;

    switch ( pNetMsg->emFB )
    {

    case EMALLIANCE_BATTLE_REQ_FB_FAIL:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_FAIL") );
        break;
    case EMALLIANCE_BATTLE_REQ_FB_OK:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::ENABLE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_OK") );
        break;
    case EMALLIANCE_BATTLE_REQ_FB_NOTMASTER:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_NOTMASTER") );
        break;
    case EMALLIANCE_BATTLE_REQ_FB_TARNOTMASTER:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_TARNOTMASTER") );
        break;
    case EMALLIANCE_BATTLE_REQ_FB_ALREADY:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_ALREADY") );
        break;
    case EMALLIANCE_BATTLE_REQ_FB_ALREADY2:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_ALREADY2") );
        break;
    case EMALLIANCE_BATTLE_REQ_FB_DISSOLUTION:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_DISSOLUTION") );
        break;
    case EMALLIANCE_BATTLE_REQ_FB_DISSOLUTION2:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_DISSOLUTION2") );
        break;
    case EMALLIANCE_BATTLE_REQ_FB_MAX:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_MAX") );
        break;
    case EMALLIANCE_BATTLE_REQ_FB_TARMAX:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_TARMAX") );
        break;
    case EMALLIANCE_BATTLE_REQ_FB_GUIDMAP:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_GUIDMAP") );
        break;
    case EMALLIANCE_BATTLE_REQ_FB_REFUSE:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_REFUSE") );
        break;
    case EMALLIANCE_BATTLE_REQ_FB_RANK:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_RANK") );
        break;
    case EMALLIANCE_BATTLE_REQ_FB_TARRANK:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_TARRANK") );
        break;
    case EMALLIANCE_BATTLE_REQ_FB_CONFT:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_CONFT") );
        break;
    case EMALLIANCE_BATTLE_REQ_FB_TARCONFT:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_TARCONFT") );
        break;
    case EMALLIANCE_BATTLE_REQ_FB_GUIDNBATTLE:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_GUIDNBATTLE") );
        break;
    case EMALLIANCE_BATTLE_REQ_FB_TIMEMIN:

        strText = sc::string::format( ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_TIMEMIN")
            , GLCONST_CHAR::dwCLUB_BATTLE_TIMEMIN / 60
            , GLCONST_CHAR::dwCLUB_BATTLE_TIMEMIN % 60).c_str();
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, strText );

        break;
    case EMALLIANCE_BATTLE_REQ_FB_TIMEMAX:

        strText = sc::string::format( ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_TIMEMAX")
            , GLCONST_CHAR::dwCLUB_BATTLE_TIMEMAX / 60
            , GLCONST_CHAR::dwCLUB_BATTLE_TIMEMAX % 60).c_str();
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, strText );

        break;
    case EMALLIANCE_BATTLE_REQ_FB_CDMBATTLE:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_CDMBATTLE") );
        break;
    case EMALLIANCE_BATTLE_REQ_FB_CDMMAP:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_CDMMAP") );
        break;

    case EMALLIANCE_BATTLE_REQ_FB_NO_AUTHORITY:
        PrintMsgTextDlg( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_NO_AUTHORITY") );
        break;
    }
}


//! 클럽 동맹 해체 요청.
bool GLGaeaClient::ReqClubAllianceDis()
{
    if (!m_pMyClub->IsValid())
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_DIS_FB_NOTMASTER"));
        return false;
    }

    if (!m_pMyClub->IsMaster(m_pCharacter->CharDbNum()))
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_DIS_FB_NOTMASTER"));
        return false;
    }

    if (!m_pMyClub->IsAlliance())
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_DIS_FB_NOTCHIEF"));
        return false;
    }

    if (!m_pMyClub->IsChief())
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_DIS_FB_NOTCHIEF"));
        return false;
    }

    if (m_pMyClub->GetAllBattleNum() > 0)
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_DIS_FB_BATTLE"));
        return false;
    }

    GLMSG::SNET_CLUB_ALLIANCE_DIS_REQ NetMsg;
    NETSENDTOAGENT(&NetMsg);

    return true;
}


//! 클럽 연합 요청.
bool GLGaeaClient::ReqClubAlliance_PToPTarget()
{
	if (!m_pLandMClient)
		return false;

	return this->ReqClubAlliance(m_pCharacter->GetTarPtoP());
}

bool GLGaeaClient::ReqClubAlliance(DWORD dwGaeaID)
{
    std::tr1::shared_ptr<GLCharClient> pTarChar = GetChar(dwGaeaID);
    if (!pTarChar)
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_REQ_FB_FAIL"));
        return false;
    }

    DWORD dwCharID = pTarChar->GetCharData().m_Base.dwCharID;
    if (!m_pMyClub->IsValid())
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_REQ_FB_NOTMASTER"));
        return false;
    }

    if (!m_pMyClub->HaveAllianceAuthority(m_pCharacter->m_CharDbNum))
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_REQ_FB_NO_AUTHORITY"));
        return false;
    }

    /*if (!m_pMyClub->IsMaster(m_pCharacter->m_CharDbNum))
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_REQ_FB_NOTMASTER"));
        return false;
    }*/

    if (!pTarChar->IsClubMaster())
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_REQ_FB_TARNOTMASTER"));
        return false;
    }

    if (m_pMyClub->IsAlliance() && !m_pMyClub->IsChief())
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_REQ_FB_NOTCHIEF"));
        return false;
    }

    if (m_pMyClub->IsRegDissolution())
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_REQ_FB_DISSOLUTION"));
        return false;
    }	

    if (m_pMyClub->GetAllBattleNum() > 0)
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_REQ_FB_CLUBBATTLE"));
        return false;
    }	

    GLMSG::NET_CLUB_ALLIANCE_REQ_CA NetMsg;
    NetMsg.m_TarCharDbNum = dwCharID;
    NETSENDTOAGENT(&NetMsg);

    return true;
}

//! 클럽 동맹 요청 답변.
bool GLGaeaClient::ReqClubAllianceAns(DWORD dwChiefCharID, bool bOK)
{
    GLMSG::NET_CLUB_ALLIANCE_REQ_ANS_CA NetMsgAns;
    NetMsgAns.m_ReqChaDbNum = dwChiefCharID;
    NetMsgAns.m_bOk = bOK;
    NETSENDTOAGENT(&NetMsgAns);

    return true;
}