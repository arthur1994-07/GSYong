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

        // ���� Ŭ���� �����ϴ� ��.
        for ( std::vector<DWORD>::size_type m=0; m<sGuidance.m_vecMaps.size(); ++m )
        {
            SNATIVEID nidMAP ( sGuidance.m_vecMaps[m] );
            GLAGLandMan *pLandMan = m_pServer->GetLand(nidMAP);
            if (!pLandMan)
                continue;

            pLandMan->SetGuidance(sGuidance.m_dwID);
        }

        // ���� Ŭ�� ���� �Ա���.
        SNATIVEID nidHallMAP ( sGuidance.m_dwClubHallMap );
        GLAGLandMan* pLandMan = m_pServer->GetLand(nidHallMAP);
        if (pLandMan)
            pLandMan->SetGuidance(sGuidance.m_dwID);

        // ���� Ŭ�� ������.
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

bool GLGuidanceAgentMan::ChangeGuidClub_RenewalGuidance(DWORD dwID, DWORD dwClubID)  // �ű� ���������� ����Ŭ�� ���� �Լ�;
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

    // �ʵ弭���� �˸�
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

bool GLGuidanceAgentMan::ChangeGuidClub(DWORD dwID, DWORD dwClubID)  // �������� ���������� ����Ŭ�� ���� �Լ�;
{
    if ( m_bDisableOldGuidance == true )
	{
		// ���������� ��Ȱ��ȭ ������ ��� �ż������� �Լ��� �����Ѵ�;
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

    // �ʵ弭���� �˸�
    GLMSG::NET_CLUB_CERTIFIED_AF NetMsgFld;
    NetMsgFld.m_GuidanceId = dwID;
    NetMsgFld.m_ClubDbNum  = dwClubID;
    m_pServer->SENDTOALLCHANNEL(&NetMsgFld);

    // Ŭ���̾�ƮƲ���� �˸�
    std::string strGuidName = pGuid->m_strName;;
    std::string strClubName;

    std::tr1::shared_ptr<GLClubAgent> pCLUB = m_pServer->GetClub(dwClubID);
    if (pCLUB)
        strClubName = pCLUB->Name();

    // Ŭ���̾�Ʈ�� �˸�.
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
    // �������� �������� �ʴ´�.
    if ( GLCONST_CHAR::bPKLESS ) return false;

    GLMSG::NET_CLUB_GUID_BATTLE_REMAIN_BRD SendData;
    msgpack::sbuffer PackBuffer;

    bool previousIsBattle = IsBattle();
    bool bNeedNotify = false;

    for ( GUIDANCE_VEC::size_type i = 0; i < m_vecGuidance.size(); ++i )
    {
        GLGuidance &sGuidance = m_vecGuidance[i];

        if ( sGuidance.IsNewCommission() )  //���ο� ������ ������ ������
        {
            sGuidance.UpdateCommission(m_pServer, fElapsedAppTime); //������ ���濡 ���� ������ ���� ���� ������Ʈ
        }
        if ( sGuidance.IsBattle() ) //������ ���̶��
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
        else    //������ �������� �ƴ� ��
        {
            CTime cCurTime = CTime::GetCurrentTime();
            int nDayOfWeek = cCurTime.GetDayOfWeek ();
            int nDay = cCurTime.GetDay ();
            int nHour = cCurTime.GetHour();
            int nMinute = cCurTime.GetMinute ();

            DWORD dwORDER = sGuidance.IsBattleWeekDay(nDayOfWeek,nDay); //�������� ������ ���ΰ�?
            if ( dwORDER!=UINT_MAX )
            {
                // ������ ���� ����.
                if ( sGuidance.IsBattleHour ( dwORDER, nHour ) )    //������ �ð��ΰ�?
                {
                    // Ŭ�� ������ ����.
                    sGuidance.DoBattleStart(m_pServer, dwORDER, nDay);
                    bNeedNotify = true;
                }
                else
                {
                    // ������ ���� ����.
                    sGuidance.UpdateNotifyBattle(m_pServer, dwORDER, nHour, nMinute);   //������ ���۱��� ���� �ð� ��Ŷ
                }
            }
        }
    }
    if ( bNeedNotify )   //���� ������ ����ð� ������ �ִٸ� (ex �������� �����߰ų� ������ �� 10�а����)
    {
        SendGuidBattle_RemainTime_BRD();    //������ ������� ���� �ð� ��Ŷ�� ����
    }
    if ( previousIsBattle != IsBattle() )   //������ �����Ȳ�� ���� ������ �ִٸ� ��Ŷ ����
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
        SendData.m_Id = Info.m_dwID; // ���� ID
        SendData.m_ClubId = Info.m_dwGuidanceClub; // ���� Ŭ�� ID

        std::tr1::shared_ptr<GLClubAgent> spClub = m_pServer->GetClub(Info.m_dwGuidanceClub);
        if (spClub)
        {
            SendData.m_ClubName = spClub->Name();
            SendData.m_Country  = spClub->GetCountry();
        }

        SendData.m_GuidName = Info.m_strName; // ���� �̸�
        SendData.m_Commission = Info.m_fCommissionPercent; // ��������
        SendData.m_GuidMap = Info.m_vecMaps; // ���� ���� ��

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
		// ������ �̸�
		sMapInfo.strName = Info.m_strName;
		// ��������
		sMapInfo.fCommision = Info.m_fCommissionPercent;
		
		// ���� ���� ��
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
    if ( MakeGuidBattle_RemainTime_Packet(PackBuffer) ) //�������� �������� �ִٸ� ��Ŷ�� ������� ����
    {
        m_pServer->SENDTOCLIENT( ClientSlot, NET_MSG_GCTRL_CLUB_GUID_BATTLE_REMAIN_BRD, PackBuffer );
    }
}

void GLGuidanceAgentMan::SendGuidBattle_RemainTime_BRD()
{
    msgpack::sbuffer PackBuffer;
    if ( MakeGuidBattle_RemainTime_Packet(PackBuffer) ) //�������� �������� �ִٸ� ��Ŷ�� ������� ����
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
        if ( sGuidance.IsBattle() )  //������ ���̶�� ��Ŷ������ ����
        {
            GLMSG::NET_CLUB_GUID_BATTLE_REMAIN_BRD::SGUIDANCE_DATA sGuidData;   //������ ������� ���� �ð� ��Ŷ
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