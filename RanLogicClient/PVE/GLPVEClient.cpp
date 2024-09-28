#include "pch.h"
#include "./GLPVEClient.h"
#include <boost/timer.hpp>

#include "../GLGaeaClient.h"
#include "../Char/GLCharacter.h"
#include "../Land/GLLandManClient.h"
#include "../Party/GLPartyClient.h"
#include "../../RanLogic/TriggerSystem/TriggerSystem.h"
#include "../../RanLogic/PVE/PVEDB.h"
#include "../TriggerSystem/ClientReadyMessage.h"
#include "../TriggerSystem/ClientMonsterClearMessage.h"
#include "../TriggerSystem/ClientStageMessage.h"
#include "../TriggerSystem/ClientStartStageMessage.h"
#include "../TriggerSystem/ClientMsgMessage.h"
#include "../TriggerSystem/ClientPositioningClearMessage.h"
#include "../EngineLib/GUInterface/GameTextControl.h"
#include "../EngineLib/GUInterface/UITextControl.h"

#include "../../RanLogicClient/Competition/Competition.h"

using namespace TriggerSystem;

GLPVEClient::GLPVEClient(GLGaeaClient* pGaeaClent)
:	m_pGaeaClient(pGaeaClent)
,   m_stage(0)
,   m_last(false)
,   m_timer(10)
,   m_portal(false)
,	m_update(false)
,	m_nThisStage(0)
{
}

GLPVEClient::~GLPVEClient()
{
	
}

void GLPVEClient::Init()
{
    SClientMapInfoMessage kMapInfo;
    TriggerSystem::SendMessage(&kMapInfo);
    m_kMapInfo = kMapInfo.mapinfo;
	m_kUpdateit = m_kMapInfo.begin();
    m_portal = false;
}

void GLPVEClient::ReqMapTable()
{
	if (m_pGaeaClient)
	{
		GLCharacter* pChar = m_pGaeaClient->GetCharacter();

		if (pChar)
		{
			GLMSG::NET_PVE_MAPTABLE_REQ_CA NetMsg;
			NetMsg.dwGaeaID = pChar->GetGaeaID();
			msgpack::sbuffer PackBuffer;
			msgpack::pack(PackBuffer, NetMsg);
			m_pGaeaClient->NETSENDTOAGENT(NET_MSG_PVE_MAPTABLE_REQ_CA, PackBuffer);			
		}
	}
}

void GLPVEClient::AckPVEMapTable(NET_MSG_GENERIC* pMsg)
{
	GLMSG::NET_PVE_MAPTABLE_ACK_AC NetMsg;

	if (CastNetMsg(pMsg, NetMsg))
	{
		if( m_pGaeaClient )
		{
            m_tables = NetMsg.tables;
			m_table = NetMsg.table;

			UpdatePossible();
		}
	}
}

void GLPVEClient::ReqEntrance(const DWORD dwCharDbNum, const DWORD dwMapID, const DWORD dwGateID, const int nLevel)
{
    if (m_pGaeaClient)
    {
		bool bEntrance = false;
        GLCharacter* pChar = m_pGaeaClient->GetCharacter();
		GLLandManClient* pLandMan = m_pGaeaClient->GetLandManClient();

		if (pLandMan && pLandMan->GetPVEIndunID().IsVaild())
		{
			/**
				인던 안에서는
				입장을 또 할수 없다.

			 */
			return;
		}

        if (pChar == 0)
        {
			return;
		}

		MAP_TABLE* table = 0;
        TriggerMapInfo* info = 0;

        if (FindInfo(dwMapID, info) && info->levelinfo[nLevel].possible)
        {
			FindTable(dwMapID, nLevel, table);

			GLMSG::NET_PVE_ENTRANCE_REQ_CA NetMsg;
			NetMsg.sInDunID.dwCharDbNum = dwCharDbNum;
			NetMsg.sInDunID.dwMapID = dwMapID;
			NetMsg.sInDunID.nLevel = nLevel;
			NetMsg.sInDunID.nNum = table ? (table->tGenTime > 0 ? table->nGenCount - 1 : table->nGenCount) : 0;
			NetMsg.dwGateID = dwGateID;

			msgpack::sbuffer PackBuffer;
			msgpack::pack(PackBuffer, NetMsg);
			m_pGaeaClient->NETSENDTOAGENT(NET_MSG_PVE_ENTRANCE_REQ_CA, PackBuffer);
        }
    }
}

void GLPVEClient::AckPVEEntrance(NET_MSG_GENERIC* pMsg)
{
    GLMSG::NET_PVE_ENTRANCE_ACK_AC NetMsg;

    if (CastNetMsg(pMsg, NetMsg))
    {
        m_indun = NetMsg.sInDunID;

        if( m_pGaeaClient )
        {
			if (NetMsg.byResult == GLMSG::ENTRANCE_ACK_NONE)
				Competition::GetInstance().AckPVEEntrance( true, m_indun.dwMapID );
			else
				Competition::GetInstance().AckPVEEntrance( false, m_indun.dwMapID );
        }
    }
}

void GLPVEClient::ReqPVEEntranceConfirm(bool bOk)
{
    if (m_pGaeaClient)
    {
        GLMSG::NET_PVE_ENTRANCE_CONFIRM_REQ NetMsg;
        NetMsg.byOk = bOk;
        NetMsg.sInDunID = m_indun;
        msgpack::sbuffer PackBuffer;
        msgpack::pack(PackBuffer, NetMsg);
        m_pGaeaClient->NETSENDTOFIELD(NET_MSG_PVE_ENTRANCE_CONFIRM_REQ, PackBuffer);
    }
}

void GLPVEClient::ReqPVERetry(bool bOk)
{
    if (m_pGaeaClient)
    {
        GLMSG::NET_PVE_RETRY_REQ_CF NetMsg;
        NetMsg.bOk = bOk;        
        msgpack::sbuffer PackBuffer;
        msgpack::pack(PackBuffer, NetMsg);
        m_pGaeaClient->NETSENDTOFIELD(NET_MSG_PVE_RETRY_REQ_CF, PackBuffer);
    }
}

void GLPVEClient::ReqPVEExit()
{
    if (m_pGaeaClient)
    {
        GLCharacter* pChar = m_pGaeaClient->GetCharacter();

        if (pChar)
        {
            GLMSG::NET_PVE_EXIT_REQ_CF NetMsg;
            NetMsg.dwCharDbNum = pChar->GetCharID();
            NetMsg.sInDunID = m_indun;
            msgpack::sbuffer PackBuffer;
            msgpack::pack(PackBuffer, NetMsg);
            m_pGaeaClient->NETSENDTOFIELD(NET_MSG_PVE_EXIT_REQ_CF, PackBuffer);
        }
    }
}

void GLPVEClient::AckPVEEntranceConfirm(NET_MSG_GENERIC* pMsg)
{
    GLMSG::NET_PVE_ENTRANCE_CONFIRM_ACK_AC NetMsg;

    if (CastNetMsg(pMsg, NetMsg))
    {
        if (m_pGaeaClient)
        {
            std::string strChaName;

            GLPartyClient* pPartyClient = m_pGaeaClient->GetMyPartyClient();
            if( pPartyClient )
            {
                GLPartySlotClient* pPartySlot = pPartyClient->GetMemberByChaDbNum( NetMsg.dwChaDbNum );
                if( pPartySlot )
                    strChaName = pPartySlot->m_szName;
            }

            switch (NetMsg.byResult)
            {
            case GLMSG::ENTRANCE_ACK_CONFIRM_TIMEOVER:
				m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("COMPETITION_PVE_TIMEOVER_ENTRANCE_CONFIRM"));
                break;
            case GLMSG::ENTRANCE_ACK_CONFIRM_CANCEL:
				{
					Competition::GetInstance().AckPVEEntrance( false, -1 );

					if( strChaName.empty() )
						m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("COMPETITION_PVE_CANCEL_ENTRANCE_CONFIRM"));
					else
						m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::DISABLE, 
							sc::string::format( ID2GAMEINTEXT( "PVE_ENTRANCE_ACK_CONFIRM_TEXT", 0 ), strChaName ) );
				}
                break;
            case GLMSG::ENTRANCE_ACK_CONFIRM_IMPOSSIBLE:
                if( strChaName.empty() == false )
                {
                    m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::DISABLE, 
                        sc::string::format( ID2GAMEINTEXT( "PVE_ENTRANCE_ACK_CONFIRM_TEXT", 1 ), strChaName ) );
                    return;
                }
                break;
                break;
            case GLMSG::ENTRANCE_ACK_CONFIRM_OK:
                if( strChaName.empty() == false )
                {
                    m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::FORESTGREEN, 
                        sc::string::format( ID2GAMEINTEXT( "PVE_ENTRANCE_ACK_CONFIRM_TEXT", 2 ), strChaName ) );
                    return;
                }
                break;
            }

			Competition::GetInstance().AckPVEEntranceConfirm(NetMsg.byResult, strChaName);
        }
    }
}

void GLPVEClient::ReqClearOfBelonging(DWORD dwCharDbNum, DWORD dwMapID, int nLevel)
{
	MAP_TABLE* table = 0;

    if (m_pGaeaClient && FindTable(dwMapID, nLevel, table))
    {
        GLMSG::NET_PVE_CLEAROFBELONGING_REQ_CA NetMsg;
        NetMsg.sInDunID.dwCharDbNum = dwCharDbNum;
        NetMsg.sInDunID.dwMapID = dwMapID;
        NetMsg.sInDunID.nLevel = nLevel;
		NetMsg.sInDunID.nNum = table->nGenCount - 1;

        msgpack::sbuffer PackBuffer;
        msgpack::pack(PackBuffer, NetMsg);
        m_pGaeaClient->NETSENDTOAGENT(NET_MSG_PVE_CLEAROFBELONGING_REQ_CA, PackBuffer);
    }
}

void GLPVEClient::AckPVEClearOfBelonging(NET_MSG_GENERIC* pMsg)
{
	GLMSG::NET_PVE_CLEAROFBELONGING_ACK_AC NetMsg;

	if (CastNetMsg(pMsg, NetMsg))
	{
		MAP_TABLE* table = 0;

		if (FindTable(NetMsg.sInDunID.dwMapID, NetMsg.sInDunID.nLevel, table))
		{
			table->tGenTime = 0;
		}

		TriggerMapInfo* info = 0;

		if (FindInfo(table->dwMapID, info))
		{
			std::string strCombine = sc::string::format("%s %s",
				ID2GAMEWORD(info->mapname.c_str(), 0),
				ID2GAMEINTEXT(_T("COMPETITION_PVE_CLEAR_OF_BELONGING"), 0));

			m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::DISABLE, strCombine.c_str());
		}
	}
}

void GLPVEClient::AckPVEAckReload(NET_MSG_GENERIC* pMsg)
{
	GLMSG::NET_PVE_GM_RELOAD_TRIGGER_ACK_AC NetMsg;

	if (CastNetMsg(pMsg, NetMsg))
	{
		TCHAR temp[256] = {0};
		_sntprintf_s(temp, 256, _T("%s 서버(%d)의 트리거 재로딩에 %s 했습니다."), 
			NetMsg.bAgent ? _T("에이전트") : _T("필드"),
			NetMsg.dwFieldSvr,
			NetMsg.bResult ? _T("성공") : _T("실패"));
		m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::DISABLE, temp);
	}
}

void GLPVEClient::AckPVERetry(NET_MSG_GENERIC* pMsg)
{
	GLMSG::NET_PVE_RETRY_ACK_FC NetMsg;

	if (CastNetMsg(pMsg, NetMsg))
	{
		std::string strChaName;

		GLPartyClient* pPartyClient = m_pGaeaClient->GetMyPartyClient();
		if( pPartyClient )
		{
			GLPartySlotClient* pPartySlot = pPartyClient->GetMemberByChaDbNum( NetMsg.dwChaDbNum );
			if( pPartySlot )
				strChaName = pPartySlot->m_szName;
		}

		switch (NetMsg.nResultCode)
		{
		case GLMSG::RESULT_CODE_CANCEL:
			if( strChaName.empty() )
				m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("COMPETITION_PVE_RETRY_CANCEL"));
			else
				m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::DISABLE, 
				sc::string::format( ID2GAMEINTEXT( "PVE_RESULT_CODE_CANCEL_TEXT", 0 ), strChaName ) );
			break;
		case GLMSG::RESULT_CODE_NOT_ENOUGH:
			if( strChaName.empty() )
				m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("COMPETITION_PVE_RETRY_NOT_ENOUGH"));
			else
				m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::DISABLE, 
				sc::string::format( ID2GAMEINTEXT( "PVE_RESULT_CODE_CANCEL_TEXT", 1 ), strChaName ) );
			break;
		case GLMSG::RESULT_CODE_ANOTHER_NOT_ENOUGH:
			m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("COMPETITION_PVE_RETRY_ANOTHER_NOT_ENOUGH"));
			break;
		case GLMSG::RESULT_CODE_TIMEOVER:
			m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("COMPETITION_PVE_RETRY_TIMEROVER"));
			break;
		case GLMSG::RESULT_CODE_OK:
			if( strChaName.empty() == false )
				m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::FORESTGREEN, 
				sc::string::format( ID2GAMEINTEXT( "PVE_RESULT_CODE_CANCEL_TEXT", 2 ), strChaName ) );
			break;

		}
	}
}

void GLPVEClient::MsgPVEEntranceState(NET_MSG_GENERIC* pMsg)
{
    GLMSG::NET_PVE_ENTRANCE_STATE_FC NetMsg;

    if (CastNetMsg(pMsg, NetMsg))
    {
        TriggerSystem::SClientReadyMessage msg;
        msg.nCur = NetMsg.nCur;
        msg.nAll = NetMsg.nAll;
        msg.nTime = NetMsg.nTime;

		m_nThisStage = NetMsg.nStage;

        if (m_pGaeaClient)
        {
            GLLandManClient* pLandMan = m_pGaeaClient->GetActiveMap();

            if (pLandMan)
            {
                pLandMan->SendTriggerMessage(&msg);
            }
        }
    }
}

void GLPVEClient::MsgPVEMonsterClearState(NET_MSG_GENERIC* pMsg)
{
    GLMSG::NET_PVE_MONSTERCLEAR_STATE_FC NetMsg;

    if (CastNetMsg(pMsg, NetMsg))
    {
        TriggerSystem::SClientMonsterClearMessage msg;
        msg.nCur = NetMsg.nCur;
        msg.nAll = NetMsg.nAll;

        if (m_pGaeaClient)
        {
            GLLandManClient* pLandMan = m_pGaeaClient->GetActiveMap();

            if (pLandMan)
            {
                pLandMan->SendTriggerMessage(&msg);
            }
        }
    }
}

void GLPVEClient::MsgPVEPositioningClearState(NET_MSG_GENERIC* pMsg)
{
    GLMSG::NET_PVE_POSITIONINGCLEAR_STATE_FC NetMsg;

    if (CastNetMsg(pMsg, NetMsg))
    {
        TriggerSystem::SClientPositioningClearMessage msg;
        msg.nX = NetMsg.nX;
        msg.nY = NetMsg.nY;
        msg.nNow = NetMsg.nNow;
        msg.nNeed = NetMsg.nNeed;
		msg.bCompleted = NetMsg.bCompleted;

        if (m_pGaeaClient)
        {
            GLLandManClient* pLandMan = m_pGaeaClient->GetActiveMap();

            if (pLandMan)
            {
                pLandMan->SendTriggerMessage(&msg);
            }
        }
    }
}

void GLPVEClient::MsgPVEStageState(NET_MSG_GENERIC* pMsg)
{
    GLMSG::NET_PVE_STAGE_STATE_FC NetMsg;

    if (CastNetMsg(pMsg, NetMsg))
    {
        TriggerSystem::SClientStageMessage msg;
        msg.nStage = NetMsg.nStage;
        msg.nTime = NetMsg.nTime;
        msg.nState = NetMsg.nState;

		m_nThisStage = NetMsg.nStage;

        if (m_pGaeaClient)
        {
            GLLandManClient* pLandMan = m_pGaeaClient->GetActiveMap();

            if (pLandMan)
            {
                pLandMan->SendTriggerMessage(&msg);
            }
        }
    }
}

void GLPVEClient::MsgPVEStageResult(NET_MSG_GENERIC* pMsg)
{
    GLMSG::NET_PVE_STAGE_RESULT_FC NetMsg;

    if (CastNetMsg(pMsg, NetMsg))
    {
		m_results = NetMsg.results;
		GLWidgetScript::GetInstance().LuaCallFunc< void >( 
			NSWIDGET_SCRIPT::g_strFunc_PVE_StageResult 
			, "-n", NetMsg.nTimer);
    }
}

void GLPVEClient::MsgPVEPortalState(NET_MSG_GENERIC* pMsg)
{
    GLMSG::NET_PVE_PORTAL_STATE_FC NetMsg;

    if (CastNetMsg(pMsg, NetMsg))
    {
        GLLandManClient* pLandMan = m_pGaeaClient->GetActiveMap();

        if (pLandMan)
        {
            DxLandGateMan* pGateMan = pLandMan->GetLandGateMan();

            if (pGateMan)
            {
                PDXLANDGATE pGate = pGateMan->FindLandGate(NetMsg.dwGateID);

                if (pGate)
                {
                    pLandMan->SetLandEffect(NetMsg.dwGateID, NetMsg.bOpen);

                    pGate->SetEvent(NetMsg.bOpen);
                    pGate->SetToGateMapID(NetMsg.dwToMapID, NetMsg.dwToGateID, -1, " ", 0);
                }
            }
        }
    }
}

// TAG_PVEClientStartStage_1
void GLPVEClient::MsgPVEStartStage(NET_MSG_GENERIC* pMsg)
{
    GLMSG::NET_PVE_START_STAGE_FC NetMsg;

    if (CastNetMsg(pMsg, NetMsg))
    {
        if (m_pGaeaClient)
        {
            GLLandManClient* pLandMan = m_pGaeaClient->GetActiveMap();

            if (pLandMan)
            {
                pLandMan->SetPVEIndunID(NetMsg.sInDunID);
                pLandMan->StartTriggerFinger(NetMsg.nStage, NetMsg.sInDunID.nLevel);
            }
        }
    }
}

void GLPVEClient::MsgPVEMsgStartStage(NET_MSG_GENERIC* pMsg)
{
    GLMSG::NET_PVE_MSG_START_STAGE_FC NetMsg;

    if (CastNetMsg(pMsg, NetMsg))
    {
        TriggerSystem::SClientStartStageMessage msg;
        msg.nTime = NetMsg.nTime;

        if (m_pGaeaClient)
        {
            GLLandManClient* pLandMan = m_pGaeaClient->GetActiveMap();

            if (pLandMan)
            {
                pLandMan->SendTriggerMessage(&msg);
            }
        }
    }
}

void GLPVEClient::MsgPVEMsg(NET_MSG_GENERIC* pMsg)
{
    GLMSG::NET_PVE_MSG_FC NetMsg;

    if (CastNetMsg(pMsg, NetMsg))
    {
        TriggerSystem::SClientMsgMessage msg;
        msg.strMsg = NetMsg.strMsg;
        msg.nTime = NetMsg.nTime;

        if (m_pGaeaClient)
        {
            GLLandManClient* pLandMan = m_pGaeaClient->GetActiveMap();

            if (pLandMan)
            {
                pLandMan->SendTriggerMessage(&msg);
            }
        }
    }
}

void GLPVEClient::MsgPVERemoveIndun(NET_MSG_GENERIC* pMsg)
{
	GLMSG::NET_PVE_REMOVE_INDUN_AC NetMsg;

	if (CastNetMsg(pMsg, NetMsg))
	{
		GLCharacter* pAvatar = m_pGaeaClient->GetCharacter();

		if (pAvatar == 0)
		{
			return;
		}

		m_table.tGenTime = 0;
	}
}

void GLPVEClient::MsgPVERebirth(NET_MSG_GENERIC* pMsg)
{
	GLMSG::NET_PVE_REBIRTH_FC NetMsg;

	if (CastNetMsg(pMsg, NetMsg))
	{
		m_pGaeaClient->PVERebirth(NetMsg.dwGaeaID);
	}
}

void GLPVEClient::GetStageResult(GLMSG::StageResult& results, int& nStage, bool& bLast, int& nTimer)
{
    results = m_results;
    nStage = m_stage;
    bLast = m_last;
    nTimer = m_timer;
}

void GLPVEClient::UpdatePossible(double dTimer)
{
    GLCharacter* pAvatar = m_pGaeaClient->GetCharacter();
 
    if (pAvatar == 0)
    {
        return;
    }

	MAP_TABLE* table = 0;
	bool bEntrance = false;
    GLPartyClient* pParty = m_pGaeaClient->GetMyPartyClient();
	GLLandManClient* pLandMan = m_pGaeaClient->GetLandManClient();

	if (pLandMan && pLandMan->GetPVEIndunID().IsVaild())
	{
		// 인던 안에 있다.
		bEntrance = true;
	}

	if (m_kUpdateit == m_kMapInfo.end())
	{
		m_kUpdateit = m_kMapInfo.begin();
	}

	boost::timer timer;

    for ( ; m_kUpdateit != m_kMapInfo.end(); ++m_kUpdateit)
    {
		if (timer.elapsed() >= dTimer)
		{
			break;
		}

		SClientTriggerMapInfo& mapInfo = *m_kUpdateit;

        bool bMaster = false;	// 파티장 유무        
        bool bPerson = false;	// 인원수

        if (pParty && pParty->isValid())
        {
            // 파티장 체크
            if ( pParty->isMaster(pAvatar->GetGaeaID()) )
            {
                bMaster = true;
            }

            // 입장 인원 체크
            if (static_cast<DWORD>(mapInfo.person) <= pParty->GetNMember())
            {
                bPerson = true;
            }
        }

		for (TriggerLevelInfoIt pit = mapInfo.levelinfo.begin(); pit != mapInfo.levelinfo.end(); ++pit)
		{
			ClientLevelInfo& levelInfo = pit->second;

			bool bHaving = false;	// 귀속 유무
			bool bCount = true;		// 입장 카운트
			bool bLevel = false;	// 유저 레벨

			// 레벨 체크
			if (pParty && pParty->IsLevelPartyMember(levelInfo.minlevel, levelInfo.maxlevel))
			{
				bLevel = true;
			}

			if (FindTable(mapInfo.mapid, levelInfo.level, table))
			{
				bCount = false;

				// 입장 횟수 체크
				if (table->nInCount < mapInfo.dailylimit)
				{
					bCount = true;
				}

				// 귀속 인던 체크
				if (table->tGenTime > 0)
				{
					bHaving = true;
				}
			}

			if (bEntrance)
			{
				/**
					인던 안일 경우
					귀속 된 인던만 보여 준다.

				 */
				levelInfo.possible = bHaving;
			}
			else
			{
				/**
					인던 밖일 경우

					1. 귀속된 인던이 있을 경우
					 귀속된 인던만 보여 준다.

					2. 귀속된 인던이 없을 경우
					 모든 체크가 성공해야 보여 준다.

				 */
				if (m_table.tGenTime > 0)
				{
					levelInfo.possible = bHaving;
				}
				else
				{
					levelInfo.possible = (bMaster && bLevel && bPerson && bCount);
				}
			}
		}
    }
}

bool GLPVEClient::FindInfo(const DWORD dwMapID, TriggerMapInfo*& info)
{
    TriggerSystem::TriggerMapInfoListIt it = m_kMapInfo.begin();

    for ( ; it != m_kMapInfo.end(); ++it)
    {
        if (it->mapid.dwID == dwMapID)
        {
            info = &*it;
            return true;
        }
    }

    return false;
}

bool GLPVEClient::FindTable(const DWORD dwChaDbNum, const SNATIVEID mid, const int nLevel, MAP_TABLE*& table)
{
	for (MapTableContainerIterator it = m_tables.begin(); it != m_tables.end(); ++it)
	{
		if (it->dwMapID == mid.dwID && it->nLevel == nLevel && it->dwCharDbNum == dwChaDbNum )
		{
			if (m_table.dwMapID == mid.dwID && m_table.nLevel == nLevel && m_table.dwCharDbNum ==  dwChaDbNum )
			{
				table = &m_table;
			}
			else
			{
				table = &*it;
			}

			return true;
		}
	}

	return false;
}

bool GLPVEClient::FindTable(const SNATIVEID mid, const int nLevel, MAP_TABLE*& table)
{
    for (MapTableContainerIterator it = m_tables.begin(); it != m_tables.end(); ++it)
    {
        if (it->dwMapID == mid.dwID && it->nLevel == nLevel)
        {
			if (m_table.dwMapID == mid.dwID && m_table.nLevel == nLevel)
			{
				table = &m_table;
			}
			else
			{
				table = &*it;
			}

            return true;
        }
    }

    return false;
}

void GLPVEClient::ClearTable()
{
	m_tables.clear();
	m_update = true;
}

bool GLPVEClient::GetEntranceMapInfo(TriggerMapInfo*& info)
{
    return FindInfo(m_indun.dwMapID, info);
}

void GLPVEClient::FrameMove(float fElapsedTime)
{
	static float fTimer = 0;

    fTimer += fElapsedTime;

    if (fTimer >= 1.f)
    {
		FrameMoveTable(1);
        fTimer -= 1.f;

		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_Competition_UpdatePVE );
    }

	UpdatePossible();

    ReqPortal();

	/**
		일정한 때에 DB 가 초기화 되는데 이때 모든 유저가
		서버에 정보를 요청하면 부하가 크므로 클라이언트 단에서
		DB 를 초기화 시켜 버린다.		

	 */
	if (IsPVEDBFile() == false)
	{
		GetPVEDBFile();
		ClearTable();
	}
}

void GLPVEClient::FrameMoveTable(_TIME_TYPE nElapsedTime)
{
	for (MapTableContainerIterator it = m_tables.begin(); it !=  m_tables.end(); ++it)
	{
		if (it->tGenTime > 0)
		{
			it->tGenTime -= nElapsedTime;

			if (it->tGenTime <= 0)
			{
				it->tGenTime = 0;
			}
		}
	}

	if (m_table.tGenTime > 0)
	{
		m_table.tGenTime -= nElapsedTime;

		if (m_table.tGenTime <= 0)
		{
			m_table.tGenTime = 0;
		}
	}
}

void GLPVEClient::ReqPortal()
{
    bool open = TRUE == m_pGaeaClient->GetGateOpen() ? true : false;

    if (open && m_portal == false)
    {
        if (m_pGaeaClient)
        {
            GLCharacter* pAvatar = m_pGaeaClient->GetCharacter();

            if (pAvatar)
            {
                DWORD dwGateID = pAvatar->DetectGate();
                PDXLANDGATE pGate = GetLandGate(dwGateID);

                if (pGate && pGate->GetEvent())
                {
                    GLMSG::NET_PVE_PORTAL_REQ_CF NetMsg;
                    NetMsg.dwFromMapID = m_pGaeaClient->GetActiveMap()->GetBaseMapID().dwID;
                    NetMsg.dwFromGateID = pGate->GetGateID();
                    NetMsg.dwToMapID = pGate->GetToMapID().dwID;
                    NetMsg.dwToGateID = pGate->GetToGateID();

                    msgpack::sbuffer PackBuffer;
                    msgpack::pack(PackBuffer, NetMsg);
                    m_pGaeaClient->NETSENDTOFIELD(NET_MSG_PVE_PORTAL_REQ_CF, PackBuffer);

                    m_portal = true;
                }
            }
        }        
    }

    if (m_portal && open == false)
    {
        m_portal = false;
    }
}

DxLandGate* GLPVEClient::GetLandGate(const DWORD dwGateID) const
{
    if (m_pGaeaClient)
    {
        GLLandManClient* pLandMan = m_pGaeaClient->GetActiveMap();

        if (pLandMan)
        {
            DxLandGateMan* pGateMan = pLandMan->GetLandGateMan();

            if (pGateMan)
            {
                return pGateMan->FindLandGate(dwGateID);
            }
        }
    }

    return 0;
}

void GLPVEClient::SendMessage(TriggerSystem::SMessage* msg)
{
	if (m_pGaeaClient)
	{
		GLLandManClient* pLandMan = m_pGaeaClient->GetActiveMap();

		if (pLandMan)
		{
			pLandMan->SendTriggerMessage(msg);
		}
	}
}

LuaTable GLPVEClient::GetResult(void) const
{
	LuaTable tbResult( GLWidgetScript::GetInstance().GetLuaState() );

	int nTableIndex = 1;
	for( UINT i = 0; i < m_results.size(); ++i )
	{
		lua_tinker::table tbRankInfo( GLWidgetScript::GetInstance().GetLuaState() );
		tbRankInfo.set( "strNAME", m_results[ i ].chaName );
		tbRankInfo.set( "emCLASS", m_results[ i ].dwClass );
		tbRankInfo.set( "nDAMAGE", m_results[ i ].dwDamage );
		tbRankInfo.set( "nHEAL", m_results[ i ].dwHeal );
		tbRankInfo.set( "bExp", m_results[ i ].bExp );
		tbRankInfo.set( "bMoney", m_results[ i ].bMoney );
		tbRankInfo.set( "bMVP", m_results[ i ].bMVP );
		tbResult.set( nTableIndex++ , tbRankInfo );
	}

	return tbResult;
}