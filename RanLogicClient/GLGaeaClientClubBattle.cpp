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

//=================================================================
//             클럽 데스매치 관련 메시지 처리     [2/19/2013 hsshin] 가독성을 위해 코드 위치 정리
//=================================================================
/*
    데스메치는 1개만 진행가능하므로 종료시간 계산이 float 변수 하나로 되어있음
    (기획 변경으로 인해 동시에 진행되는 데스메치가 존재하게 될 경우 선도전을 참고 할것.)
    선도전은 여러개가 진행가능하므로 각 선도전의 이름과 종료시간을 벡터로 관리하고 있음
*/

void GLGaeaClient::MsgclubDeathMatchStartBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_CLUB_DEATHMATCH_START_BRD *pNetMsg = (GLMSG::SNET_CLUB_DEATHMATCH_START_BRD *)pMsg;

    if (pNetMsg->nTIME==0)
    {				
        PrintConsoleTextDlg(
            sc::string::format(ID2GAMEINTEXT("CLUB_DEATHMATCH_START"), pNetMsg->szName));
    }
    else
    {				
        PrintConsoleTextDlg(
            sc::string::format(
            ID2GAMEINTEXT("CLUB_DEATHMATCH_MIN_START"),
            pNetMsg->szName,
            pNetMsg->nTIME));
    }
}

void GLGaeaClient::MsgClubDeathMatchEndBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_CLUB_DEATHMATCH_END_BRD *pNetMsg = (GLMSG::SNET_CLUB_DEATHMATCH_END_BRD *)pMsg;	
    PrintConsoleTextDlg(
        sc::string::format(ID2GAMEINTEXT("CLUB_DEATHMATCH_END"), pNetMsg->szName));
}

void GLGaeaClient::MsgClubDeathMatchRemainBrd(NET_MSG_GENERIC* pMsg)
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*)pMsg;
    GLMSG::NET_CLUB_DEATHMATCH_REMAIN_BRD RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return;

    m_fClubDeathMatchTimer = (float)RecvData.dwTime;
}

void GLGaeaClient::MsgClubDeathMatchPointUpdate(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_CLUB_DEATHMATCH_POINT_UPDATE *pNetMsg = (GLMSG::SNET_CLUB_DEATHMATCH_POINT_UPDATE*)pMsg;	
    if ( pNetMsg->bKillPoint )
        m_sMyCdmRank.wKillNum++;
    else
        m_sMyCdmRank.wDeathNum++;

    //	UI Refrash
	//!! SF_TODO
    //m_pInterface->VisibleCDMRanking(true);
    //m_pInterface->RefreashCDMRanking();
}

void GLGaeaClient::MsgClubDeathMatchMyRankUpdate(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_CLUB_DEATHMATCH_MYRANK_UPDATE *pNetMsg = (GLMSG::SNET_CLUB_DEATHMATCH_MYRANK_UPDATE*)pMsg;	

    m_sMyCdmRank = pNetMsg->sMyCdmRank;

    //	UI Refrash 및 열기
	//!! SF_TODO
    //m_pInterface->VisibleCDMRanking(true);
    //m_pInterface->RefreashCDMRanking();
}

void GLGaeaClient::MsgServerClubDeathMatchInfo(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_SERVER_CLUB_DEATHMATCH_INFO* pNetMsg = ( GLMSG::SNETPC_SERVER_CLUB_DEATHMATCH_INFO* )pMsg;
    m_bClubDeathMatch = pNetMsg->bClubDeathMatch;
    if ( m_bClubDeathMatch )
    {
		//!! SF_TODO
        //m_pInterface->SetVisibleMiniMapClubDeathMatchTime( TRUE );
    }
    else
    {
        m_fClubDeathMatchTimer = 0.0f;
		//!! SF_TODO
        //m_pInterface->SetVisibleMiniMapClubDeathMatchTime( FALSE );
        // 적대관계 모두 삭제
        GetCharacter()->DEL_PLAYHOSTILE_ALL();
    }
}

void GLGaeaClient::MsgClubDeathMatchRankingUpdate(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_CLUB_DEATHMATCH_RANKING_UPDATE *pNetMsg = (GLMSG::SNET_CLUB_DEATHMATCH_RANKING_UPDATE*)pMsg;	

    int nRankNum = pNetMsg->wRankNum;

    for ( int i = 0; i < nRankNum; ++i )
    {
        int nIndex = pNetMsg->sCdmRank[i].nIndex;
        if ( nIndex < 0 )
            continue;

        int nSize = (int)m_vecCdmRank.size();				
        if ( nIndex < nSize )
        {
            m_vecCdmRank[nIndex] = pNetMsg->sCdmRank[i];
        }
        else
        {
            m_vecCdmRank.resize( nIndex+1 );
            m_vecCdmRank[nIndex] = pNetMsg->sCdmRank[i];
        }
    }

    //	UI Refrash 및 열기
	//!! SF_TODO
    //m_pInterface->VisibleCDMRanking(true);
    //m_pInterface->RefreashCDMRanking();
}

//! 클럽 데스매치 전적 순위-킬-데스/ club death match
void GLGaeaClient::GetClubDeathMatchRecord(DWORD& Ranking, DWORD& Kill, DWORD& Death) const
{
    // 아직 구현되지 않음
    Ranking = m_sMyCdmRank.wClubRanking;
    Kill = m_sMyCdmRank.wKillNum;
    Death = m_sMyCdmRank.wDeathNum;
}

lua_tinker::table GLGaeaClient::GetCDMRecord()
{
	lua_tinker::table tbCDMRecord( GLWidgetScript::GetInstance().GetLuaState() );

	tbCDMRecord.set(1, m_sMyCdmRank.wClubRanking);
	tbCDMRecord.set(2, m_sMyCdmRank.wKillNum);
	tbCDMRecord.set(3, m_sMyCdmRank.wDeathNum);

	return tbCDMRecord;
}

//=================================================================
//              클럽 배틀 관련 메시지 처리     [2/19/2013 hsshin] 가독성을 위해 코드 위치 정리
//=================================================================

//! 클럽 배틀/club battle
void GLGaeaClient::GetClubBattleData(std::vector<GLCLUBBATTLE>& vData) const
{
    m_pMyClub->GetClubBattleData(vData);
    /*
    // club mark image
    const DxClubMan::DXDATA& sMarkData = DxClubMan::GetInstance().GetClubData(
        pd3dDevice,
        m_pCharacter->m_dwServerID,
        ,
        );
    */
}

void GLGaeaClient::MsgClubBattlePointUpdate( NET_MSG_GENERIC* nmg ) 
{
    GLMSG::SNET_CLUB_BATTLE_POINT_UPDATE *pNetMsg = (GLMSG::SNET_CLUB_BATTLE_POINT_UPDATE *)nmg;

    GLCLUBBATTLE* pClubBattle = m_pMyClub->GetClubBattle( pNetMsg->dwBattleClubID );
    if ( !pClubBattle ) return;

    if ( pNetMsg->bKillPoint )
    {
        pClubBattle->m_wKillPoint++;
        pClubBattle->m_wKillPointTemp++;
    }
    else
    {
        pClubBattle->m_wDeathPoint++;
        pClubBattle->m_wDeathPointTemp++;
    }

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_Club_UpdateBattleClubList );
}

void GLGaeaClient::MsgClubBattleKillUpdate( NET_MSG_GENERIC* nmg ) 
{
    GLMSG::SNET_CLUB_BATTLE_KILL_UPDATE *pNetMsg = (GLMSG::SNET_CLUB_BATTLE_KILL_UPDATE *)nmg;

    GLCLUBBATTLE* pClubBattle = m_pMyClub->GetClubBattle( pNetMsg->dwBattleClubID );
    if ( !pClubBattle ) return;

    pClubBattle->m_wKillPoint = pNetMsg->wKillPoint;
    pClubBattle->m_wDeathPoint = pNetMsg->wDeathPoint;

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_Club_UpdateBattleClubList );
}

void GLGaeaClient::MsgClubBattleSubmissionReqFB(NET_MSG_GENERIC* nmg)
{
    GLMSG::SNET_CLUB_BATTLE_SUBMISSION_REQ_FB* pNetMsg = (GLMSG::SNET_CLUB_BATTLE_SUBMISSION_REQ_FB*) nmg;

    switch (pNetMsg->emFB)
    {
    case EMCLUB_BATTLE_SUBMISSION_FB_FAIL:
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_SUBMISSION_FB_FAIL"));
        break;
    case EMCLUB_BATTLE_SUBMISSION_FB_OK:
        PrintMsgTextDlg(NS_UITEXTCOLOR::ENABLE, ID2GAMEINTEXT("EMCLUB_BATTLE_SUBMISSION_FB_OK"));
        break;
    case EMCLUB_BATTLE_SUBMISSION_FB_NOTMASTER:
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_SUBMISSION_FB_NOTMASTER"));
        break;
    case EMCLUB_BATTLE_SUBMISSION_FB_GUIDMAP:
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_SUBMISSION_FB_GUIDMAP"));
        break;
    case EMCLUB_BATTLE_SUBMISSION_FB_NOBATTLE:
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_SUBMISSION_FB_NOBATTLE"));
        break;
    case EMCLUB_BATTLE_SUBMISSION_FB_DISTIME:
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_SUBMISSION_FB_DISTIME"));
        break;
    case EMCLUB_BATTLE_SUBMISSION_FB_CDMMAP:
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_SUBMISSION_FB_CDMMAP"));
        break;				
    case EMCLUB_BATTLE_SUBMISSION_FB_NO_AUTHORITY:
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("CLUB_NOT_HAVE_AUTHRITY"));
        break;
    default:
        PrintMsgTextDlg(
            NS_UITEXTCOLOR::NEGATIVE,
            sc::string::format(
            "Unknown type. MsgClubBattleSubmissionReqFB %1%", pNetMsg->emFB));
        break;
    }
}

//! Agent->Client 클럽 배틀 휴전요청
void GLGaeaClient::MsgClubBattleArmisticeReqAsk(NET_MSG_GENERIC* nmg) 
{
    GLMSG::SNET_CLUB_BATTLE_ARMISTICE_REQ_ASK* pNetMsg = (GLMSG::SNET_CLUB_BATTLE_ARMISTICE_REQ_ASK*) nmg;

    GLCLUBBATTLE* pClubBattle = m_pMyClub->GetClubBattle(pNetMsg->dwClubID);
	if( NULL != pClubBattle )
	{
		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_Club_ArmisticeAsk,
			"-dw, -s, -dw, -b",
			pNetMsg->dwClubID,
			pClubBattle->m_szClubName,
			pNetMsg->m_ReqChaDbNum,
			false );
	}
}

void GLGaeaClient::MsgClubBattleArmisticeReqFB( NET_MSG_GENERIC* nmg ) 
{
    GLMSG::SNET_CLUB_BATTLE_ARMISTICE_REQ_FB *pNetMsg = (GLMSG::SNET_CLUB_BATTLE_ARMISTICE_REQ_FB *)nmg;

    switch (pNetMsg->emFB)
    {
    case EMCLUB_BATTLE_ARMISTICE_FB_FAIL:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_ARMISTICE_FB_FAIL") );
        break;
    case EMCLUB_BATTLE_ARMISTICE_FB_OK:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::ENABLE, ID2GAMEINTEXT("EMCLUB_BATTLE_ARMISTICE_FB_OK") );
        break;
    case EMCLUB_BATTLE_ARMISTICE_FB_NOTMASTER:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_ARMISTICE_FB_NOTMASTER") );
        break;
    case EMCLUB_BATTLE_ARMISTICE_FB_OFFMASTER:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_ARMISTICE_FB_OFFMASTER") );
        break;
    case EMCLUB_BATTLE_ARMISTICE_FB_REFUSE:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_ARMISTICE_FB_REFUSE") );
        break;
    case EMCLUB_BATTLE_ARMISTICE_FB_GUIDMAP:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_ARMISTICE_FB_GUIDMAP") );
        break;
    case EMCLUB_BATTLE_ARMISTICE_FB_NOBATTLE:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_ARMISTICE_FB_NOBATTLE") );
        break;
    case EMCLUB_BATTLE_ARMISTICE_FB_DISTIME:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_ARMISTICE_FB_DISTIME") );
        break;
    case EMCLUB_BATTLE_ARMISTICE_FB_CDMMAP:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_ARMISTICE_FB_CDMMAP") );
        break;
    case EMCLUB_BATTLE_NO_AUTHORITY:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("CLUB_NOT_HAVE_AUTHRITY") );
        break;
    default:
        PrintMsgTextDlg(
            NS_UITEXTCOLOR::NEGATIVE,
            sc::string::format(
            "Unknown type. MsgClubBattleArmisticeReqFB %1%", pNetMsg->emFB));
        break;
    }			
}

void GLGaeaClient::MsgClubBattleOver( NET_MSG_GENERIC* nmg ) 
{
    GLMSG::SNET_CLUB_BATTLE_OVER_CLT *pNetMsg = (GLMSG::SNET_CLUB_BATTLE_OVER_CLT *)nmg;

    GLCLUBBATTLE* pClubBattle = m_pMyClub->GetClubBattle( pNetMsg->dwClubID );
    if ( !pClubBattle )
        return;

    CString strClubName(pClubBattle->m_szClubName);
    if ( pClubBattle->m_bAlliance )
    {
        std::string strText;
        switch( pNetMsg->emFB )
        {
        case EMCLUB_BATTLE_OVER_DRAW:
            {
                m_pMyClub->m_dwAllianceBattleDraw++;
                strText = sc::string::format( ID2GAMEINTEXT("ALLIANCE_BATTLE_OVER_DRAW"), m_pMyClub->Name(), strClubName ).c_str();	

                PrintMsgTextDlg ( NS_UITEXTCOLOR::IDCOLOR, strText );

            }
            break;
        case EMCLUB_BATTLE_OVER_WIN:
            {
                m_pMyClub->m_dwAllianceBattleWin++;
                strText = sc::string::format( ID2GAMEINTEXT("ALLIANCE_BATTLE_OVER_WIN"), 
                    strClubName, pNetMsg->wKillPoint, pNetMsg->wDeathPoint ).c_str();

                PrintMsgTextDlg ( NS_UITEXTCOLOR::IDCOLOR, strText );
            }
            break;
        case EMCLUB_BATTLE_OVER_LOSE:
            {
                m_pMyClub->m_dwAllianceBattleLose++;
                strText = sc::string::format( ID2GAMEINTEXT("ALLIANCE_BATTLE_OVER_LOSE"), 
                    strClubName, pNetMsg->wKillPoint, pNetMsg->wDeathPoint ).c_str();

                PrintMsgTextDlg ( NS_UITEXTCOLOR::IDCOLOR, strText );
            }
            break;
        case EMCLUB_BATTLE_OVER_ARMISTICE:
            {
                m_pMyClub->m_dwAllianceBattleDraw++;

                strText = sc::string::format( ID2GAMEINTEXT("ALLIANCE_BATTLE_OVER_ARMISTICE"), strClubName ).c_str();
                PrintMsgTextDlg ( NS_UITEXTCOLOR::IDCOLOR, strText );
            }
            break;
        case EMCLUB_BATTLE_OVER_SUBMISSION:
            {
                m_pMyClub->m_dwAllianceBattleLose++;

                strText = sc::string::format( ID2GAMEINTEXT("ALLIANCE_BATTLE_OVER_SUBMISSION"), strClubName ).c_str();
                PrintMsgTextDlg ( NS_UITEXTCOLOR::IDCOLOR, strText );
            }
            break;
        case EMCLUB_BATTLE_OVER_TARSUBMISSION:
            {
                m_pMyClub->m_dwAllianceBattleWin++;

                strText = sc::string::format( ID2GAMEINTEXT("ALLIANCE_BATTLE_OVER_TARSUBMISSION"), strClubName ).c_str();
                PrintMsgTextDlg ( NS_UITEXTCOLOR::IDCOLOR, strText );
            }
            break;
        }

    }
    else
    {
        std::string strText;
        switch( pNetMsg->emFB )
        {
        case EMCLUB_BATTLE_OVER_DRAW:
            {
                m_pMyClub->m_dwBattleDraw++;

                strText = sc::string::format( ID2GAMEINTEXT("CLUB_BATTLE_OVER_DRAW"), m_pMyClub->Name(), strClubName ).c_str();
                PrintMsgTextDlg ( NS_UITEXTCOLOR::IDCOLOR, strText );
            }
            break;
        case EMCLUB_BATTLE_OVER_WIN:
            {
                m_pMyClub->m_dwBattleWin++;

                strText = sc::string::format( ID2GAMEINTEXT("CLUB_BATTLE_OVER_WIN"), strClubName, pNetMsg->wKillPoint, pNetMsg->wDeathPoint ).c_str();
                PrintMsgTextDlg ( NS_UITEXTCOLOR::IDCOLOR, strText );
            }
            break;
        case EMCLUB_BATTLE_OVER_LOSE:
            {
                m_pMyClub->m_dwBattleLose++;

                strText = sc::string::format( ID2GAMEINTEXT("CLUB_BATTLE_OVER_LOSE"), strClubName, pNetMsg->wKillPoint, pNetMsg->wDeathPoint ).c_str();
                PrintMsgTextDlg ( NS_UITEXTCOLOR::IDCOLOR, strText );
            }
            break;
        case EMCLUB_BATTLE_OVER_ARMISTICE:
            {
                m_pMyClub->m_dwBattleDraw++;

                strText = sc::string::format( ID2GAMEINTEXT("CLUB_BATTLE_OVER_ARMISTICE"), strClubName ).c_str();
                PrintMsgTextDlg ( NS_UITEXTCOLOR::IDCOLOR, strText );
            }
            break;
        case EMCLUB_BATTLE_OVER_SUBMISSION:
            {
                m_pMyClub->m_dwBattleLose++;

                strText = sc::string::format( ID2GAMEINTEXT("CLUB_BATTLE_OVER_SUBMISSION"), strClubName ).c_str();
                PrintMsgTextDlg ( NS_UITEXTCOLOR::IDCOLOR, strText );
            }
            break;
        case EMCLUB_BATTLE_OVER_TARSUBMISSION:
            {
                m_pMyClub->m_dwBattleWin++;

                strText = sc::string::format( ID2GAMEINTEXT("CLUB_BATTLE_OVER_TARSUBMISSION"), strClubName ).c_str();
                PrintMsgTextDlg ( NS_UITEXTCOLOR::IDCOLOR, strText );
            }
            break;
        }
    }



    m_pMyClub->DELBATTLECLUB( pNetMsg->dwClubID );

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_Club_UpdateBattleClubList );

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_Club_UpdateBattleRecords );

	//!! SF_TODO
    //m_pInterface->REFRESH_CLUB_STATE();
}

void GLGaeaClient::MsgClubBattleBegin2( NET_MSG_GENERIC* nmg ) 
{
    GLMSG::SNET_CLUB_BATTLE_BEGIN_CLT2 *pNetMsg = (GLMSG::SNET_CLUB_BATTLE_BEGIN_CLT2 *)nmg;

    GLCLUBBATTLE sClubBattle;
    sClubBattle.m_dwCLUBID =pNetMsg->dwClubID;
    sClubBattle.m_tStartTime = pNetMsg->tStartTime;
    sClubBattle.m_tEndTime = pNetMsg->tEndTime;
    sClubBattle.m_bAlliance = pNetMsg->bAlliance;
    sClubBattle.SetName(pNetMsg->szClubName);
    m_pMyClub->ADDBATTLECLUB(sClubBattle);

    std::string strText;
    if ( pNetMsg->bAlliance )
    {
        strText = sc::string::format( ID2GAMEINTEXT("ALLIANCE_BATTLE_BEGIN"), m_pMyClub->Name(), pNetMsg->szClubName ).c_str();
        PrintMsgTextDlg ( NS_UITEXTCOLOR::IDCOLOR, strText );
    }
    else
    {
        strText = sc::string::format( ID2GAMEINTEXT("CLUB_BATTLE_BEGIN"), m_pMyClub->Name(), pNetMsg->szClubName ).c_str();
        PrintMsgTextDlg ( NS_UITEXTCOLOR::IDCOLOR, strText );
    }

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_Club_UpdateBattleClubList );
}

void GLGaeaClient::MsgClubBattleBegin( NET_MSG_GENERIC* nmg ) 
{
    GLMSG::SNET_CLUB_BATTLE_BEGIN_CLT *pNetMsg = (GLMSG::SNET_CLUB_BATTLE_BEGIN_CLT *)nmg;

    std::string strText;
    if ( pNetMsg->bAlliance ) 
    {
        strText = sc::string::format( ID2GAMEINTEXT("ALLIANCE_BATTLE_BEGIN_READY"), m_pMyClub->Name(), pNetMsg->szClubName ).c_str();
        PrintMsgTextDlg ( NS_UITEXTCOLOR::IDCOLOR, strText );
    }
    else
    {	
        strText = sc::string::format( ID2GAMEINTEXT("CLUB_BATTLE_BEGIN_READY"), m_pMyClub->Name(), pNetMsg->szClubName ).c_str();
        PrintMsgTextDlg ( NS_UITEXTCOLOR::IDCOLOR, strText );
    }
}

void GLGaeaClient::MsgClubBattleReqAsk( NET_MSG_GENERIC* nmg ) 
{
    GLMSG::SNET_CLUB_BATTLE_REQ_ASK *pNetMsg = (GLMSG::SNET_CLUB_BATTLE_REQ_ASK *)nmg;
	
	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_Club_BattleAsk,
		"-dw, -s, -dw, -b",
		pNetMsg->dwClubCharID,
		pNetMsg->szClubName,
		pNetMsg->dwBattleTime,
		false );
}

void GLGaeaClient::MsgClubBattleReqFB( NET_MSG_GENERIC* nmg ) 
{
    GLMSG::SNET_CLUB_BATTLE_REQ_FB *pNetMsg = (GLMSG::SNET_CLUB_BATTLE_REQ_FB *)nmg;

    switch ( pNetMsg->emFB )
    {

    case EMCLUB_BATTLE_REQ_FB_FAIL:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_FAIL") );
        break;
    case EMCLUB_BATTLE_REQ_FB_OK:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::ENABLE, ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_OK") );
        break;
    case EMCLUB_BATTLE_REQ_FB_NOTMASTER:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_NOTMASTER") );
        break;
    case EMCLUB_BATTLE_REQ_FB_TARNOTMASTER:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_TARNOTMASTER") );
        break;
    case EMCLUB_BATTLE_REQ_FB_ALREADY:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_ALREADY") );
        break;
    case EMCLUB_BATTLE_REQ_FB_ALREADY2:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_ALREADY2") );
        break;
    case EMCLUB_BATTLE_REQ_FB_DISSOLUTION:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_DISSOLUTION") );
        break;
    case EMCLUB_BATTLE_REQ_FB_DISSOLUTION2:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_DISSOLUTION2") );
        break;
    case EMCLUB_BATTLE_REQ_FB_MAX:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_MAX") );
        break;
    case EMCLUB_BATTLE_REQ_FB_TARMAX:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_TARMAX") );
        break;
    case EMCLUB_BATTLE_REQ_FB_GUIDMAP:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_GUIDMAP") );
        break;
    case EMCLUB_BATTLE_REQ_FB_REFUSE:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_REFUSE") );
        break;
    case EMCLUB_BATTLE_REQ_FB_RANK:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_RANK") );
        break;
    case EMCLUB_BATTLE_REQ_FB_TARRANK:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_TARRANK") );
        break;
    case EMCLUB_BATTLE_REQ_FB_CONFT:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_CONFT") );
        break;
    case EMCLUB_BATTLE_REQ_FB_TARCONFT:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_TARCONFT") );
        break;
    case EMCLUB_BATTLE_REQ_FB_GUIDNBATTLE:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_GUIDNBATTLE") );
        break;
    case EMCLUB_BATTLE_REQ_FB_TIMEMIN:
        {
            std::string strText = sc::string::format( ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_TIMEMIN")
                , GLCONST_CHAR::dwCLUB_BATTLE_TIMEMIN / 60
                , GLCONST_CHAR::dwCLUB_BATTLE_TIMEMIN % 60).c_str();

            PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, strText );
        }
        break;
    case EMCLUB_BATTLE_REQ_FB_TIMEMAX:
        {
            std::string strText = sc::string::format( ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_TIMEMAX")
                , GLCONST_CHAR::dwCLUB_BATTLE_TIMEMAX / 60
                , GLCONST_CHAR::dwCLUB_BATTLE_TIMEMAX % 60).c_str();

            PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, strText );
        }
        break;
    case EMCLUB_BATTLE_REQ_FB_CDMBATTLE:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_CDMBATTLE") );
        break;
    case EMCLUB_BATTLE_REQ_FB_CDMMAP:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_CDMMAP") );
        break;
    case EMCLUB_BATTLE_REQ_FB_NO_AUTHORITY:
        PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_NO_AUTHORITY") );
        break;

    }
}

//! 클럽 배틀 전적 승-무-패/ club battle
void GLGaeaClient::GetClubBattleRecord(DWORD& Win, DWORD& Draw, DWORD& Lost) const
{
    Win  = m_pMyClub->GetBattleWin();
    Draw = m_pMyClub->GetBattleDraw();
    Lost = m_pMyClub->GetBattleLost();
}

lua_tinker::table GLGaeaClient::GetClubBattleRecords()
{
	lua_tinker::table tbClubbattleRcd( GLWidgetScript::GetInstance().GetLuaState() );

	tbClubbattleRcd.set(1, m_pMyClub->GetBattleWin());
	tbClubbattleRcd.set(2, m_pMyClub->GetBattleDraw());
	tbClubbattleRcd.set(3, m_pMyClub->GetBattleLost());

	return tbClubbattleRcd;
}

//! 클럽 World battle 점수, 순위
void GLGaeaClient::GetClubWorldBattleRecord(DWORD& Point, DWORD& Rank) const
{
    // 아직 구현되지 않음
    Point = 0;
    Rank = 0;
}

//! 클럽 배틀 요청.
bool GLGaeaClient::ReqClubBattle(DWORD dwGaeaID, DWORD dwTime)
{
    if (!GLCONST_CHAR::bCLUB_BATTLE)
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_FAIL"));
        return false;
    }

    std::tr1::shared_ptr<GLCharClient> pCHAR_REQ = GetChar(dwGaeaID);
    if (!pCHAR_REQ)
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_FAIL"));
        return false;
    }

    if (m_pCharacter->IsConfting())
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_CONFT"));
        return false;
    }

    DWORD dwCharID = pCHAR_REQ->GetCharData().m_Base.dwCharID;

    if (!m_pMyClub->IsValid())
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_NOTMASTER"));
        return false;
    }

    if (!m_pMyClub->HaveClubBattleAuthority(m_pCharacter->CharDbNum()))
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_NO_AUTHORITY"));
        return false;
    }
    /*if (!m_pMyClub->IsMaster(m_pCharacter->CharDbNum()))
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_NOTMASTER"));
        return false;
    }*/

    if (!pCHAR_REQ->IsClubMaster())
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_TARNOTMASTER"));
        return false;
    }

    if (m_pMyClub->IsAllianceGuild_CHIEF(pCHAR_REQ->GETCLUBID()))
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_ALLIANCE"));
        return false;
    }

    if (m_pMyClub->IsRegDissolution())
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_DISSOLUTION"));
        return false;
    }

    if (m_pMyClub->IsBattle(pCHAR_REQ->GETCLUBID()) || m_pMyClub->IsBattleReady(pCHAR_REQ->GETCLUBID()))
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_ALREADY"));
        return false;
    }

    if (m_pMyClub->IsBattleAlliance(pCHAR_REQ->GETALLIANCEID()) || m_pMyClub->IsBattleReady(pCHAR_REQ->GETALLIANCEID()))
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_ALREADY2"));
        return false;
    }

    CString strText;

    if (dwTime < GLCONST_CHAR::dwCLUB_BATTLE_TIMEMIN)
    {
        PrintMsgTextDlg(
            NS_UITEXTCOLOR::NEGATIVE,
            sc::string::format(
                ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_TIMEMIN"), 
                GLCONST_CHAR::dwCLUB_BATTLE_TIMEMIN / 60,
                GLCONST_CHAR::dwCLUB_BATTLE_TIMEMIN % 60));
        return false;
    }

    if (dwTime > GLCONST_CHAR::dwCLUB_BATTLE_TIMEMAX)
    {		
        PrintMsgTextDlg(
            NS_UITEXTCOLOR::NEGATIVE,
            sc::string::format(
                ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_TIMEMAX"),
                GLCONST_CHAR::dwCLUB_BATTLE_TIMEMAX / 60,
                GLCONST_CHAR::dwCLUB_BATTLE_TIMEMAX % 60));
        return false;
    }

    GLMSG::SNET_CLUB_BATTLE_REQ NetMsg;
    NetMsg.dwCharID = dwCharID;
    NetMsg.dwBattleTime = dwTime;
    NETSENDTOAGENT(&NetMsg);

    return true;
}

bool GLGaeaClient::ReqClubBattle( const std::string& strCharName, WORD wHour, WORD wMin )
{
	std::tr1::shared_ptr< GLCharClient > spChar = m_pLandMClient->FindChar( strCharName );
	if( NULL == spChar )
		return false;

	return ReqClubBattle( spChar->GetGaeaID(), wHour * 60 + wMin );
}

//! 동맹 배틀 요청.
bool GLGaeaClient::ReqAllianceBattle(DWORD dwGaeaID, DWORD dwTime)
{
    DWORD dwCharID;
    std::tr1::shared_ptr<GLCharClient> pCHAR_REQ;
    if (!GLCONST_CHAR::bCLUB_BATTLE || !GLCONST_CHAR::bCLUB_BATTLE_ALLIANCE)
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_FAIL"));
        return false;
    }

    pCHAR_REQ = GetChar(dwGaeaID);
    if (!pCHAR_REQ)
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_FAIL"));
        return false;
    }

    if (m_pCharacter->IsConfting())
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_CONFT"));
        return false;
    }

    dwCharID = pCHAR_REQ->GetCharData().m_Base.dwCharID;

    if (!m_pMyClub->IsValid())
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_NOTMASTER"));
        return false;
    }

    if (!m_pMyClub->HaveClubBattleAuthority(m_pCharacter->CharDbNum()))
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_NO_AUTHORITY"));
        return false;
    }

    /*if (!m_pMyClub->IsMaster(m_pCharacter->CharDbNum()))
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_NOTMASTER"));
        return false;
    }*/

    if (!pCHAR_REQ->IsClubMaster())
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_TARNOTMASTER"));
        return false;
    }

    if (m_pMyClub->IsAllianceGuild_CHIEF( pCHAR_REQ->GETCLUBID()))
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_ALLIANCE"));
        return false;
    }

    if (!pCHAR_REQ->IsAllianceMaster())
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_TARNOTMASTER"));
        return false;
    }

    if (m_pMyClub->IsRegDissolution())
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_DISSOLUTION"));
        return false;
    }

    if (m_pMyClub->IsBattle(pCHAR_REQ->GETCLUBID()) || m_pMyClub->IsBattleReady(pCHAR_REQ->GETCLUBID()))
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_ALREADY"));
        return false;
    }

    if (m_pMyClub->IsBattleAlliance(pCHAR_REQ->GETALLIANCEID()) || m_pMyClub->IsBattleReady(pCHAR_REQ->GETALLIANCEID()))
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_ALREADY2"));
        return false;
    }

    if (dwTime < GLCONST_CHAR::dwCLUB_BATTLE_TIMEMIN)
    {
        PrintMsgTextDlg(
            NS_UITEXTCOLOR::NEGATIVE,
            sc::string::format(
                ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_TIMEMIN"),
                GLCONST_CHAR::dwCLUB_BATTLE_TIMEMIN / 60,
                GLCONST_CHAR::dwCLUB_BATTLE_TIMEMIN % 60));
        return false;
    }

    if (dwTime > GLCONST_CHAR::dwCLUB_BATTLE_TIMEMAX)
    {
        PrintMsgTextDlg(
            NS_UITEXTCOLOR::NEGATIVE,
            sc::string::format(
                ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_TIMEMAX"),
                GLCONST_CHAR::dwCLUB_BATTLE_TIMEMAX / 60,
                GLCONST_CHAR::dwCLUB_BATTLE_TIMEMAX % 60));
        return false;
    }

    GLMSG::SNET_ALLIANCE_BATTLE_REQ NetMsg;
    NetMsg.dwCharID = dwCharID;
    NetMsg.dwBattleTime = dwTime;
    NETSENDTOAGENT(&NetMsg);

    return true;
}

bool GLGaeaClient::ReqAllianceBattle( const std::string& strCharName, WORD wHour, WORD wMin )
{
	std::tr1::shared_ptr< GLCharClient > spChar = m_pLandMClient->FindChar( strCharName );
	if( NULL == spChar )
		return false;

	return ReqAllianceBattle( spChar->GetGaeaID(), wHour * 60 + wMin );
}

//! 클럽 배틀 요청 답변.
bool GLGaeaClient::ReqClubBattleAns(DWORD dwChiefCharID, bool bOK)
{
    GLMSG::SNET_CLUB_BATTLE_REQ_ANS NetMsgAns;
    NetMsgAns.dwClubCharID = dwChiefCharID;
    NetMsgAns.bOK = bOK;
    NETSENDTOAGENT(&NetMsgAns);
    return true;
}

//! 클럽 배틀 요청 답변.
bool GLGaeaClient::ReqAllianceBattleAns(DWORD dwChiefCharID, bool bOK)
{
    GLMSG::SNET_ALLIANCE_BATTLE_REQ_ANS NetMsgAns;
    NetMsgAns.dwClubCharID = dwChiefCharID;
    NetMsgAns.bOK = bOK;
    NETSENDTOAGENT(&NetMsgAns);
    return true;
}

bool GLGaeaClient::ReqClubBattleArmistice_TargetName(const char* szClubName)
{
	std::vector<GLCLUBALLIANCE> vecAlliance;
	this->GetClubAlliance( vecAlliance );
	int nAllianceSize(vecAlliance.size());

	std::vector<GLCLUBBATTLE> vecClubBattle;
	this->GetClubBattleData(vecClubBattle);
	int nHostileSize(vecClubBattle.size());
	
	for(int i = 0; i < nHostileSize; ++i)
	{
		if ( strcmp(szClubName, vecClubBattle[i].m_szClubName) == 0 )
		{
			if ( nAllianceSize == 0)
				return this->ReqClubBattleArmistice( vecClubBattle[i].m_dwCLUBID );
			else
				return this->ReqAllianceBattleArmistice( vecClubBattle[i].m_dwCLUBID );
			break;
		}
	}
	return false;
}

//! 클럽 배틀 휴전 요청
bool GLGaeaClient::ReqClubBattleArmistice(DWORD dwCLUBID)
{
    if (!m_pMyClub->IsValid())
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_ARMISTICE_FB_NOTMASTER"));
        return false;
    }

    /*
    if (!m_pMyClub->IsMaster(m_pCharacter->m_CharDbNum))
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_ARMISTICE_FB_NOTMASTER"));
        return false;
    }
    */
    if (!m_pMyClub->HaveClubBattleAuthority(m_pCharacter->CharDbNum()))
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("CLUB_NOT_HAVE_AUTHRITY"));
        return false;
    }

    if (!m_pMyClub->IsBattle(dwCLUBID))
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_ARMISTICE_FB_NOBATTLE"));
        return false;
    }

    // 클라이언트 시간 부정확성
    //	if ( !m_sCLUB.IsBattleStop( dwCLUBID ) )
    //	{
    //		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_ARMISTICE_FB_DISTIME") );
    //		return S_FALSE;
    //	}

    GLMSG::SNET_CLUB_BATTLE_ARMISTICE_REQ NetMsg(dwCLUBID);
    NETSENDTOAGENT(&NetMsg);

    return true;
}

//! 클럽 동맹 배틀 휴전 요청
bool GLGaeaClient::ReqAllianceBattleArmistice(DWORD dwCLUBID)
{
    if (!m_pMyClub->IsValid())
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_ARMISTICE_FB_NOTMASTER"));
        return false;
    }

    /*
    if (!m_pMyClub->IsMaster(m_pCharacter->CharDbNum()))
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_ARMISTICE_FB_NOTMASTER"));
        return false;
    }
    */
    if (!m_pMyClub->HaveClubBattleAuthority(m_pCharacter->CharDbNum()))
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("CLUB_NOT_HAVE_AUTHRITY"));
        return false;
    }

    if (!m_pMyClub->IsAlliance() || !m_pMyClub->IsChief()) 
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_ARMISTICE_FB_NOTMASTER"));
        return false;
    }
    
    if (!m_pMyClub->IsBattleAlliance(dwCLUBID))
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_ARMISTICE_FB_NOBATTLE"));
        return false;
    }

    // 클라이언트 시간 부정확성
    //	if ( !m_sCLUB.IsBattleStop( dwCLUBID ) )
    //	{
    //		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_ARMISTICE_FB_DISTIME") );
    //		return S_FALSE;
    //	}

    GLMSG::SNET_ALLIANCE_BATTLE_ARMISTICE_REQ NetMsg(dwCLUBID);
    NETSENDTOAGENT(&NetMsg);

    return true;
}

//! 클럽 배틀 휴전 답변 Client->Agent
bool GLGaeaClient::ReqClubBattleArmisticeAns(DWORD dwCLUBID, DWORD ReqChaDbNum, bool bOK)
{	
    GLMSG::SNET_CLUB_BATTLE_ARMISTICE_REQ_ANS NetMsgAns(dwCLUBID, ReqChaDbNum, bOK);
    NETSENDTOAGENT(&NetMsgAns);
    return true;
}

//! 클럽 동맹 배틀 휴전 답변 Client->Agent
bool GLGaeaClient::ReqAllianceBattleArmisticeAns(DWORD dwCLUBID, DWORD ReqChaDbNum, bool bOK)
{	
    GLMSG::SNET_ALLIANCE_BATTLE_ARMISTICE_REQ_ANS NetMsgAns(dwCLUBID, ReqChaDbNum, bOK);
    NETSENDTOAGENT(&NetMsgAns);
    return true;
}

bool GLGaeaClient::ReqClubBattleSubmission_TargetName(const char* szClubName)
{
	std::vector<GLCLUBALLIANCE> vecAlliance;
	this->GetClubAlliance( vecAlliance );
	int nAllianceSize(vecAlliance.size());

	std::vector<GLCLUBBATTLE> vecClubBattle;
	this->GetClubBattleData(vecClubBattle);
	int nHostileSize(vecClubBattle.size());

	for(int i = 0; i < nHostileSize; ++i)
	{
		if ( strcmp(szClubName, vecClubBattle[i].m_szClubName) == 0 )
		{
			if ( nAllianceSize == 0)
				return this->ReqClubBattleSubmission( vecClubBattle[i].m_dwCLUBID );
			else
				return this->ReqAllianceBattleSubmission( vecClubBattle[i].m_dwCLUBID );
			break;
		}
	}
	return false;
}

//! 클럽 배틀 항복 요청 Client->Agent
bool GLGaeaClient::ReqClubBattleSubmission(DWORD dwCLUBID)
{
    if (!m_pMyClub->IsValid())
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_SUBMISSION_FB_NOTMASTER"));
        return false;
    }

    //if (!m_pMyClub->IsMaster(m_pCharacter->m_CharDbNum))
    //{
    //    PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_SUBMISSION_FB_NOTMASTER"));
    //    return false;
    //}
    if (!m_pMyClub->HaveClubBattleAuthority(m_pCharacter->CharDbNum()))
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("CLUB_NOT_HAVE_AUTHRITY"));
        return false;
    }

    if (!m_pMyClub->IsBattle(dwCLUBID))
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_SUBMISSION_FB_NOBATTLE"));
        return false;
    }

    // 클라이언트 시간 부정확성
    //	if ( !m_sCLUB.IsBattleStop( dwCLUBID ) )
    //	{
    //		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_SUBMISSION_FB_DISTIME") );
    //		return S_FALSE;
    //	}

    GLMSG::SNET_CLUB_BATTLE_SUBMISSION_REQ NetMsg(dwCLUBID);
    NETSENDTOAGENT(&NetMsg);

    return true;
}


//! 동맹 배틀 항복 요청 Client->Agent
bool GLGaeaClient::ReqAllianceBattleSubmission(DWORD dwCLUBID)
{
    if (!m_pMyClub->IsValid())
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_SUBMISSION_FB_NOTMASTER"));
        return false;
    }
/*
    if (!m_pMyClub->IsMaster(m_pCharacter->m_CharDbNum))
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_SUBMISSION_FB_NOTMASTER"));
        return false;
    }
*/
    if (!m_pMyClub->HaveClubBattleAuthority(m_pCharacter->CharDbNum()))
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("CLUB_NOT_HAVE_AUTHRITY"));
        return false;
    }

    if (!m_pMyClub->IsAlliance() || !m_pMyClub->IsChief())
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_SUBMISSION_FB_NOTMASTER"));
        return false;
    }

    if (!m_pMyClub->IsBattleAlliance(dwCLUBID))
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_SUBMISSION_FB_NOBATTLE"));
        return false;
    }

    // 클라이언트 시간 부정확성
    //	if ( !m_sCLUB.IsBattleStop( dwCLUBID ) )
    //	{
    //		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_SUBMISSION_FB_DISTIME") );
    //		return S_FALSE;
    //	}

    GLMSG::SNET_ALLIANCE_BATTLE_SUBMISSION_REQ NetMsg(dwCLUBID);
    NETSENDTOAGENT(&NetMsg);
    
    return true;
}


//=================================================================
//                  선도전 관련 메시지 처리     [2/19/2013 hsshin]
//=================================================================
void GLGaeaClient::MsgClubGuidBattleStartBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_CLUB_GUID_BATTLE_START_BRD *pNetMsg = (GLMSG::SNET_CLUB_GUID_BATTLE_START_BRD *)pMsg;

    if (pNetMsg->nTIME==0)
    {
		PLANDMANCLIENT pLand = GetActiveMap();
		if ( pLand &&  pLand->m_bClubBattle && pLand->m_dwClubMapID == pNetMsg->dwGuidanceID )
		{
			pLand->m_bClubBattleInProgress = true;
		}

        PrintConsoleTextDlg(
            sc::string::format(
            ID2GAMEINTEXT("CLUB_BATTLE_START"), pNetMsg->szName));
    }
    else
    {				
        PrintConsoleTextDlg(
            sc::string::format(
            ID2GAMEINTEXT("CLUB_BATTLE_MIN_START"), pNetMsg->szName, pNetMsg->nTIME));
    }
}

void GLGaeaClient::MsgClubGuidBattleEndBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_CLUB_GUID_BATTLE_END_BRD *pNetMsg = (GLMSG::SNET_CLUB_GUID_BATTLE_END_BRD *)pMsg;			

	PLANDMANCLIENT pLand = GetActiveMap();
	if ( pLand &&  pLand->m_bClubBattle && pLand->m_dwClubMapID == pNetMsg->dwGuidanceID )
	{
		pLand->m_bClubBattleInProgress = false;
	}

    PrintConsoleTextDlg(
        sc::string::format(
        ID2GAMEINTEXT("CLUB_BATTLE_END"),
        pNetMsg->szName,
        pNetMsg->szClubName));
}

void GLGaeaClient::MsgClubGuidBattleRemainBrd(NET_MSG_GENERIC* pMsg)
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*)pMsg;
    GLMSG::NET_CLUB_GUID_BATTLE_REMAIN_BRD RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return;
    m_vecClubGuidBattleName.clear();
    m_vecClubGuidBattleTimer.clear();

    const DWORD dwVecSize = RecvData.vecGuid.size();
    for ( size_t i =0; i < dwVecSize; ++i)
    {
        m_vecClubGuidBattleName.push_back( RecvData.vecGuid[i].strName);
        m_vecClubGuidBattleTimer.push_back( (float) RecvData.vecGuid[i].dwTime );
    }
    for ( DWORD i = 0; i < dwVecSize - 1; ++i )
    {
        DWORD min = i;
        for ( DWORD j = i + 1; j < dwVecSize; ++j )
        {
            if ( m_vecClubGuidBattleTimer[j] < m_vecClubGuidBattleTimer[min] )
            {
                min = j;
            }
        }
        if ( min != i )
        {
            const float swapTime = m_vecClubGuidBattleTimer[min];
            m_vecClubGuidBattleTimer[min] = m_vecClubGuidBattleTimer[i];
            m_vecClubGuidBattleTimer[i] = swapTime;
            const std::string swapName = m_vecClubGuidBattleName[min];
            m_vecClubGuidBattleName[min] = m_vecClubGuidBattleName[i];
            m_vecClubGuidBattleName[i] = swapName;
        }
    }
	//!! SF_TODO
    //m_pInterface->SetClubGuidBattleName( m_vecClubGuidBattleName );
}

void GLGaeaClient::MsgServerClubGuidBattleInfo(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_SERVER_CLUB_GUID_BATTLE_INFO* pNetMsg = ( GLMSG::SNETPC_SERVER_CLUB_GUID_BATTLE_INFO* )pMsg;
    m_bCLUBBATTLE = pNetMsg->bClubBattle;
    if ( m_bCLUBBATTLE )
    {
		//!! SF_TODO
        //m_pInterface->SetVisibleMiniMapClubGuidBattleTime( TRUE );
    }
    else
    {
		//!! SF_TODO
        //m_pInterface->SetVisibleMiniMapClubGuidBattleTime( FALSE );
        // 적대관계 모두 삭제
        GetCharacter()->DEL_PLAYHOSTILE_ALL();
    }
}

lua_tinker::table GLGaeaClient::SfGetCurrentMapGuidClubInfo(void)
{
	lua_tinker::table tbGuidClubInfo( GLWidgetScript::GetInstance().GetLuaState() );

	PLANDMANCLIENT pLand = GetActiveMap();
	if ( pLand )
	{
		tbGuidClubInfo.set( 1, pLand->m_dwGuidClub );
		tbGuidClubInfo.set( 2, pLand->m_szGuidClubName );
	}
	return tbGuidClubInfo;
}
