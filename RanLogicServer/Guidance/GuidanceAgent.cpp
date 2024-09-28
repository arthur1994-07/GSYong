#include "pch.h"
#include <algorithm>
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../RanLogic/Msg/GuidanceMsg.h"

#include "../AgentServer/GLAgentServer.h"
#include "../Club/GLClubAgent.h"
#include "./GuidanceAgent.h"

#include "../Database/DBAction/DbActionLogInstantDungeon.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


GLGuidanceAgentMan::GLGuidanceAgentMan(GLAgentServer* pServer)
    : m_pServer(pServer)
    , m_bDisableOldGuidance(false)
{
}

GLGuidanceAgentMan::~GLGuidanceAgentMan()
{
}

GLGuidance* GLGuidanceAgentMan::FindByClubID(DWORD dwClubID)
{
    for (GUIDANCE_VEC::size_type i=0; i<m_vecGuidance.size(); ++i)
    {
        GLGuidance &sGuidance = m_vecGuidance[i];
        if (sGuidance.m_dwGuidanceClub == dwClubID)
            return &sGuidance;
    }
    return NULL;
}

DWORD GLGuidanceAgentMan::GetGuidID(DWORD dwClubID) const
{
    for (GUIDANCE_VEC::size_type i=0; i<m_vecGuidance.size(); ++i)
    {
        const GLGuidance& sGuidance = m_vecGuidance[i];
        if (sGuidance.m_dwGuidanceClub == dwClubID)
            return sGuidance.m_dwID;
    }
    return UINT_MAX;
}

bool GLGuidanceAgentMan::SetMapState ()
{
    for ( GUIDANCE_VEC::size_type i=0; i<m_vecGuidance.size(); ++i )
    {
        GLGuidance &sGuidance = m_vecGuidance[i];

        // 선도 클럽이 관리하는 멥.
        for ( std::vector<DWORD>::size_type m=0; m<sGuidance.m_vecMaps.size(); ++m )
        {
            SNATIVEID nidMAP ( sGuidance.m_vecMaps[m] );
            GLAGLandMan *pLandMan = m_pServer->GetLand(nidMAP);
            if (!pLandMan)
                continue;

            pLandMan->SetGuidance(sGuidance.m_dwID);
        }

        // 선도 클럽 전투 입구존.
        SNATIVEID nidHallMAP ( sGuidance.m_dwClubHallMap );
        GLAGLandMan* pLandMan = m_pServer->GetLand(nidHallMAP);
        if (pLandMan)
            pLandMan->SetGuidance(sGuidance.m_dwID);

        // 선도 클럽 전투존.
        SNATIVEID nidMAP ( sGuidance.m_dwClubMap );
        pLandMan = m_pServer->GetLand(nidMAP);
        if (pLandMan)
        {
            pLandMan->SetGuidance(sGuidance.m_dwID);
            pLandMan->SetClubBattleMap(true);
        }
    }

    return true;
}

bool GLGuidanceAgentMan::ChangeGuidClub_RenewalGuidance(DWORD dwID, DWORD dwClubID)  // 신규 선도전에서 선도클럽 설정 함수;
{
    GLGuidance* pGuid = Find(dwID);
    if (!pGuid)
        return false;

	bool bReturn = true;
	if ( UINT_MAX != GetGuidID( dwClubID ) )
	{
		dwClubID = 0;
		bReturn = false;
	}

    std::string Msg(
        sc::string::format(
        "Change Guidance : ID %1% ClubDbNum %2%",
        dwID,
        dwClubID));
    sc::writeLogInfo(Msg);

    pGuid->ChangeGuidClub(m_pServer, dwClubID);

    // 필드서버에 알림
    GLMSG::NET_CLUB_CERTIFIED_AF NetMsgFld;
    NetMsgFld.m_GuidanceId = dwID;
    NetMsgFld.m_ClubDbNum  = dwClubID;
    m_pServer->SENDTOALLCHANNEL(&NetMsgFld);

    const char* MapName = m_pServer->GetMapName(pGuid->m_dwClubMap);
    if (MapName)
    {            
        std::string LogMsg(
            sc::string::format(
            ID2GAMEINTEXT("CLUB_LOG_GUIDANCE"),
            MapName));
        m_pServer->SENDCLUBLOG(pGuid->m_dwGuidanceClub, club::LOG_GUIDANCE, LogMsg);
    }

    db::LogInstantGuidance* pDbAction = new db::LogInstantGuidance(
        db::LogInstantGuidance::EM_REWARD,
        sc::MGUID());

    pDbAction->strGuidanceName = pGuid->GetName();
    pDbAction->dwCharDbNum = dwID;
    pDbAction->dwClubDbNum = dwClubID;
    pDbAction->nClubPoint = (int)pGuid->m_fCommissionPercent;

    m_pServer->AddLogAdoJob( db::DbActionPtr(pDbAction) );

    return bReturn;
}

bool GLGuidanceAgentMan::ChangeGuidClub(DWORD dwID, DWORD dwClubID)  // 구버전의 선도전에서 선도클럽 설정 함수;
{
    if ( m_bDisableOldGuidance == true )
	{
		// 구선도전이 비활성화 상태일 경우 신선도전의 함수를 실행한다;
		ChangeGuidClub_RenewalGuidance( dwID, dwClubID );

        return false;
	}

    GLGuidance* pGuid = Find(dwID);
    if (!pGuid)
        return false;

    std::string Msg(
        sc::string::format(
            "Change Guidance : ID %1% ClubDbNum %2%",
            dwID,
            dwClubID));
    sc::writeLogInfo(Msg);

    pGuid->ChangeGuidClub(m_pServer, dwClubID);

    // 필드서버에 알림
    GLMSG::NET_CLUB_CERTIFIED_AF NetMsgFld;
    NetMsgFld.m_GuidanceId = dwID;
    NetMsgFld.m_ClubDbNum  = dwClubID;
    m_pServer->SENDTOALLCHANNEL(&NetMsgFld);

    // 클라이언트틀에게 알림
    std::string strGuidName = pGuid->m_strName;;
    std::string strClubName;

    std::tr1::shared_ptr<GLClubAgent> pCLUB = m_pServer->GetClub(dwClubID);
    if (pCLUB)
        strClubName = pCLUB->Name();

    // 클라이언트에 알림.
    std::string strText = sc::string::format(ID2SERVERTEXT("EMGUIDCLUB_CERTIFIED"), strClubName.c_str(), strGuidName.c_str());
    GLMSG::SNET_SERVER_GENERALCHAT SendData;
    if (SendData.SETTEXT(strText))
    {
        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, SendData);
        m_pServer->SENDTOALLCLIENT(NET_MSG_SERVER_GENERALCHAT, SendBuffer);
    }
    return true;
}

bool GLGuidanceAgentMan::ChangeCommission ( DWORD dwID, float fRate )
{
    GLGuidance* pGuid = Find(dwID);
    if (!pGuid)
        return false;
    pGuid->ChangeCommission(m_pServer, fRate);
    return true;
}

bool GLGuidanceAgentMan::FrameMove ( float fElapsedAppTime )
{
    // 선도전을 시작하지 않는다.
    if ( GLCONST_CHAR::bPKLESS ) return false;

    GLMSG::NET_CLUB_GUID_BATTLE_REMAIN_BRD SendData;
    msgpack::sbuffer PackBuffer;

    bool previousIsBattle = IsBattle();
    bool bNeedNotify = false;

    for ( GUIDANCE_VEC::size_type i = 0; i < m_vecGuidance.size(); ++i )
    {
        GLGuidance &sGuidance = m_vecGuidance[i];

        if ( sGuidance.IsNewCommission() )  //새로운 수수료 변경이 있을때
        {
            sGuidance.UpdateCommission(m_pServer, fElapsedAppTime); //수수료 변경에 대한 공지에 대한 로직 업데이트
        }
        if ( sGuidance.IsBattle() ) //선도전 중이라면
        {
            if ( sGuidance.m_fNotifyRemainTimer > 600.0f )
            {
                bNeedNotify = true;
                sGuidance.m_fNotifyRemainTimer = 0.0f;
            }
            sGuidance.m_fBattleTimer += fElapsedAppTime;
            sGuidance.m_fNotifyRemainTimer += fElapsedAppTime;
            if ( sGuidance.m_fBattleTimer > float(sGuidance.m_dwBattleTime) )
            {
                sGuidance.DoBattleEnd(m_pServer);
            }
        }
        else    //선도전 진행중이 아닐 때
        {
            CTime cCurTime = CTime::GetCurrentTime();
            int nDayOfWeek = cCurTime.GetDayOfWeek ();
            int nDay = cCurTime.GetDay ();
            int nHour = cCurTime.GetHour();
            int nMinute = cCurTime.GetMinute ();

            DWORD dwORDER = sGuidance.IsBattleWeekDay(nDayOfWeek,nDay); //선도전이 열리는 날인가?
            if ( dwORDER!=UINT_MAX )
            {
                // 결정전 시작 점검.
                if ( sGuidance.IsBattleHour ( dwORDER, nHour ) )    //선도전 시간인가?
                {
                    // 클럽 결정전 시작.
                    sGuidance.DoBattleStart(m_pServer, dwORDER, nDay);
                    bNeedNotify = true;
                }
                else
                {
                    // 결정전 시작 공지.
                    sGuidance.UpdateNotifyBattle(m_pServer, dwORDER, nHour, nMinute);   //선도전 시작까지 남은 시간 패킷
                }
            }
        }
    }
    if ( bNeedNotify )   //보낼 선도전 종료시간 정보가 있다면 (ex 선도전이 시작했거나 시작후 매 10분경과시)
    {
        SendGuidBattle_RemainTime_BRD();    //선도전 종료까지 남은 시간 패킷을 전송
    }
    if ( previousIsBattle != IsBattle() )   //선도전 진행상황에 대한 변동이 있다면 패킷 전송
    {
        GLMSG::SNETPC_SERVER_CLUB_GUID_BATTLE_INFO NetMsgClubBattle;
        NetMsgClubBattle.bClubBattle = IsBattle();
        m_pServer->SENDTOALLCLIENT( &NetMsgClubBattle );
    }
    return true;
}

void GLGuidanceAgentMan::SendInfo(DWORD ClientSlot)
{
    if (!m_pServer)
        return;

    for (size_t i=0; i<m_vecGuidance.size(); ++i)
    {
        const GLGuidance& Info = m_vecGuidance[i];

        NET_GM_GUIDANCE_INFO_AC SendData;
        SendData.m_Id = Info.m_dwID; // 관리 ID
        SendData.m_ClubId = Info.m_dwGuidanceClub; // 선도 클럽 ID

        std::tr1::shared_ptr<GLClubAgent> spClub = m_pServer->GetClub(Info.m_dwGuidanceClub);
        if (spClub)
        {
            SendData.m_ClubName = spClub->Name();
            SendData.m_Country  = spClub->GetCountry();
        }

        SendData.m_GuidName = Info.m_strName; // 관리 이름
        SendData.m_Commission = Info.m_fCommissionPercent; // 수수료율
        SendData.m_GuidMap = Info.m_vecMaps; // 선도 관리 맵

        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, SendData);

        m_pServer->SENDTOCLIENT(ClientSlot, NET_MSG_GM_GUIDANCE_INFO_AC, SendBuffer);
    }
}

void GLGuidanceAgentMan::SendGuidanceMapInfo( DWORD ClientSlot, DWORD ClubDbNum )
{
	if( !m_pServer )
		return;

	NET_GUIDANCE_MAP_INFO_AC SendData;
	for( size_t i = 0; i < m_vecGuidance.size(); ++i )
	{
		const GLGuidance& Info = m_vecGuidance[ i ];

		NET_GUIDANCE_MAP_INFO_AC::SGuidanceInfo sMapInfo;
		// 선도전 이름
		sMapInfo.strName = Info.m_strName;
		// 수수료율
		sMapInfo.fCommision = Info.m_fCommissionPercent;
		
		// 선도 관리 맵
		if( ClubDbNum == Info.m_dwGuidanceClub )
			sMapInfo.vecMap = Info.m_vecMaps;

		SendData.m_GuidMap.push_back( sMapInfo );
	}

	msgpack::sbuffer SendBuffer;
	msgpack::pack( SendBuffer, SendData );

	m_pServer->SENDTOCLIENT( ClientSlot, NET_MSG_GUIDANCE_MAP_INFO_AC, SendBuffer );
}

void GLGuidanceAgentMan::SendGuidBattle_RemainTime(DWORD ClientSlot)
{
    msgpack::sbuffer PackBuffer;
    if ( MakeGuidBattle_RemainTime_Packet(PackBuffer) ) //진행중인 선도전이 있다면 패킷이 만들어져 나옴
    {
        m_pServer->SENDTOCLIENT( ClientSlot, NET_MSG_GCTRL_CLUB_GUID_BATTLE_REMAIN_BRD, PackBuffer );
    }
}

void GLGuidanceAgentMan::SendGuidBattle_RemainTime_BRD()
{
    msgpack::sbuffer PackBuffer;
    if ( MakeGuidBattle_RemainTime_Packet(PackBuffer) ) //진행중인 선도전이 있다면 패킷이 만들어져 나옴
    {
        m_pServer->SENDTOALLCLIENT( NET_MSG_GCTRL_CLUB_GUID_BATTLE_REMAIN_BRD, PackBuffer );
    }
}

bool GLGuidanceAgentMan::MakeGuidBattle_RemainTime_Packet( msgpack::sbuffer& PackBuffer)
{
    GLMSG::NET_CLUB_GUID_BATTLE_REMAIN_BRD SendData;

    const DWORD VecSize = m_vecGuidance.size();
    for ( DWORD i = 0; i < VecSize; ++i )
    {
        const GLGuidance &sGuidance = m_vecGuidance[i];
        if ( sGuidance.IsBattle() )  //선도전 중이라면 패킷정보를 담음
        {
            GLMSG::NET_CLUB_GUID_BATTLE_REMAIN_BRD::SGUIDANCE_DATA sGuidData;   //선도전 종료까지 남은 시간 패킷
            sGuidData.strName = sGuidance.m_strName;
            sGuidData.dwTime = sGuidance.m_dwBattleTime - (DWORD)sGuidance.m_fBattleTimer;
            SendData.vecGuid.push_back( sGuidData );
        }
    }
    if ( SendData.vecGuid.empty() == false) 
    {
        msgpack::pack( PackBuffer, SendData );
        return true;
    }
    return false;
}