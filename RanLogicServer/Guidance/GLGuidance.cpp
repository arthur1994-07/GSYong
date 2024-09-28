#include "pch.h"
#include <algorithm>

#include "../../SigmaCore/String/StringFormat.h"

#include "../../EngineLib/Common/IniLoader.h"
#include "../../EngineLib/G-Logic/GLogic.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"

#include "../../RanLogic/Msg/WorldBattleMsg.h"

#include "../AgentServer/GLAgentServer.h"
#include "../Club/GLClubAgent.h"
#include "../Database/DBAction/DbActionGameClub.h"
#include "../FieldServer/GLGaeaServer.h"
#include "./GLGuidance.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

GLGuidance::GLGuidance() :
	m_dwID(UINT_MAX),
	
	m_dwClubMap(0),
	m_dwCLubMapGate(0),
	m_dwClubHallMap(0),

	m_fItemDropRate(0),
	m_fDefaultCommissionPer(5),

	m_dwGuidanceClub(CLUB_NULL),
	m_fCommissionPercent(0.0f),

	m_dwBattleTime(3600),
	m_bLog(FALSE),
	
	m_bNotifyOneHour(false),
	m_bNotifyHalfHour(false),
	m_bNotify10MinHour(false),
	
	m_bBattle(false),
	m_dwBattleOrder(0),
	m_dwLastBattleDay(0),
	m_fBattleTimer(0.0f),
    m_fNotifyRemainTimer(0.0f),

	m_dwCERTIFY_CHARID(0),
	m_dwCERTIFY_NPCID(0),
	m_fCERTIFY_TIMER(0),
	m_vCERTIFY_POS(0,0,0),

	m_bBattleEndCheck(false),
	m_fCHECK_TIMER(0.0f),
	m_fCHECK_TIME(0.0f),
	m_fLog_TIMER(0.0f),
	m_bMapEffectUse(false),
	m_fHoldCertifyTime(0.0f),
	m_nMapEffectIndex(-1)
{
}

GLGuidance::~GLGuidance()
{
}

GLGuidance& GLGuidance::operator= ( const GLGuidance& value )
{
	m_dwID = value.m_dwID;
	m_strName = value.m_strName;
	m_vecMaps = value.m_vecMaps;

	m_dwClubMap = value.m_dwClubMap;
	m_dwCLubMapGate = value.m_dwCLubMapGate;
	m_dwClubHallMap = value.m_dwClubHallMap;

	m_dwBattleTime = value.m_dwBattleTime;
	m_bLog = value.m_bLog;

	for ( int i=0; i<MAX_TIME; ++i )
		m_sTIME[i] = value.m_sTIME[i];

	m_vecAwardMob.clear();
	for ( size_t n=0; n<value.m_vecAwardMob.size(); ++n )
	{
		m_vecAwardMob.push_back( value.m_vecAwardMob[n] );
	}
	
	m_fItemDropRate = value.m_fItemDropRate;
	m_fDefaultCommissionPer = value.m_fDefaultCommissionPer;

	m_dwGuidanceClub = value.m_dwGuidanceClub;
	m_fCommissionPercent = value.m_fCommissionPercent;

	m_NewCommission = value.m_NewCommission;
	
	m_bNotifyOneHour = value.m_bNotifyOneHour;
	m_bNotifyHalfHour = value.m_bNotifyHalfHour;
	m_bNotify10MinHour = value.m_bNotify10MinHour;

	m_bBattle = value.m_bBattle;
	m_dwLastBattleDay = value.m_dwLastBattleDay;
	m_fBattleTimer = value.m_fBattleTimer;
    m_fNotifyRemainTimer = value.m_fNotifyRemainTimer;

	m_dwCERTIFY_CHARID = value.m_dwCERTIFY_CHARID;
	m_dwCERTIFY_NPCID = value.m_dwCERTIFY_NPCID;
	m_fCERTIFY_TIMER = value.m_fCERTIFY_TIMER;
	m_vCERTIFY_POS = value.m_vCERTIFY_POS;

	m_bBattleEndCheck = value.m_bBattleEndCheck;
	m_fCHECK_TIMER = value.m_fCHECK_TIMER;
	m_fCHECK_TIME = value.m_fCHECK_TIME;
	m_fLog_TIMER = value.m_fLog_TIMER;

	m_bMapEffectUse = value.m_bMapEffectUse;
	m_fHoldCertifyTime = value.m_fHoldCertifyTime;
	m_nMapEffectIndex = value.m_nMapEffectIndex;

	m_mapKillLog = value.m_mapKillLog;
	m_vecMapEffect = value.m_vecMapEffect;

	return *this;
}

bool GLGuidance::Load(const std::string& strFile, bool bToolMode)
{
	if (strFile.empty())
        return FALSE;

	std::string strPath(GLOGIC::GetServerPath());
	strPath += strFile;

	CIniLoader cFILE(",", bToolMode);

	if (GLOGIC::UseLogicZipFile())
		cFILE.SetZipFile( GLOGIC::strGLOGIC_SERVER_ZIPFILE );

	if( !cFILE.open ( strPath, true ) )
	{
		sc::writeLogError(
            sc::string::format(
                "GLGuidance::Load File Open %1%", strFile));
		return false;
	}
	
	cFILE.getflag( "GUIDANCE", "ID", 0, 1, m_dwID );
	cFILE.getflag( "GUIDANCE", "NAME", 0, 1, m_strName );

	SNATIVEID nidMAP;
	cFILE.getflag( "GUIDANCE", "CLUB_MAP", 0, 2, nidMAP.wMainID );
	cFILE.getflag( "GUIDANCE", "CLUB_MAP", 1, 2, nidMAP.wSubID );
	m_dwClubMap = nidMAP.dwID;

	cFILE.getflag( "GUIDANCE", "CLUB_GATE", 0, 1, m_dwCLubMapGate );

	cFILE.getflag( "GUIDANCE", "CLUB_HALL_MAP", 0, 2, nidMAP.wMainID );
	cFILE.getflag( "GUIDANCE", "CLUB_HALL_MAP", 1, 2, nidMAP.wSubID );
	m_dwClubHallMap = nidMAP.dwID;

	cFILE.getflag( "GUIDANCE", "CLUB_LOG", 0, 1, m_bLog );

	cFILE.getflag( "GUIDANCE", "COMMISSION", 0, 1, m_fCommissionPercent );
	cFILE.getflag( "GUIDANCE", "ITEM_DROPRATE", 0, 1, m_fItemDropRate );

	DWORD dwNUM = cFILE.GetKeySize( "GUIDANCE", "BATTLE_TIME" );
	if( dwNUM > 4 )
	{
		sc::writeLogError(
            sc::string::format(
                "GLGuidance::Load %1%, It was wrong size of BATTLE_TIME", strFile));
		return false;
	}

	for( DWORD i=0; i<dwNUM; ++i )
	{
		cFILE.getflag( i, "GUIDANCE", "BATTLE_TIME", 0, 2, m_sTIME[i].dwWeekDay );
		cFILE.getflag( i, "GUIDANCE", "BATTLE_TIME", 1, 2, m_sTIME[i].dwStartTime );
	}

	cFILE.getflag( "GUIDANCE", "BATTLE_THE_TIME", 0, 1, m_dwBattleTime );

	m_vecMaps.clear ();

	dwNUM = cFILE.GetKeySize( "GUIDANCE", "GUIDANCE_MAP" );
	for ( DWORD i=0; i<dwNUM; ++i )
	{
		cFILE.getflag( i, "GUIDANCE", "GUIDANCE_MAP", 0, 2, nidMAP.wMainID );
		cFILE.getflag( i, "GUIDANCE", "GUIDANCE_MAP", 1, 2, nidMAP.wSubID );

		m_vecMaps.push_back ( nidMAP.dwID );
	}

	m_vecAwardMob.clear();

	dwNUM = cFILE.GetKeySize( "GUIDANCE", "AWARD_MOB" );
	for ( DWORD i=0; i<dwNUM; ++i )
	{
		SAWARD_MOB sAwardMob;

		cFILE.getflag( i, "GUIDANCE", "AWARD_MOB", 0, 4, sAwardMob.nidGenMob.wMainID );
		cFILE.getflag( i, "GUIDANCE", "AWARD_MOB", 1, 4, sAwardMob.nidGenMob.wSubID );
		cFILE.getflag( i, "GUIDANCE", "AWARD_MOB", 2, 4, sAwardMob.wGenPosX );
		cFILE.getflag( i, "GUIDANCE", "AWARD_MOB", 3, 4, sAwardMob.wGenPosY );

		m_vecAwardMob.push_back( sAwardMob );
	}

	BOOL bUse = FALSE;

	cFILE.getflag( "GUIDANCE", "MAP_EFFECT_USE", 0, 1, bUse );

	m_bMapEffectUse = (0!=bUse);

	if ( m_bMapEffectUse ) 
	{
		DWORD dwLandEffect = 0;

		cFILE.getflag( "GUIDANCE", "MAP_EFFECT_NUM", 0, 1, dwLandEffect );

		if ( dwLandEffect > EMMAP_EFFECT_MAX ) dwLandEffect = EMMAP_EFFECT_MAX;

		for ( DWORD i = 0; i < dwLandEffect; ++i )
		{
			CString strTemp;
			strTemp.Format( "MAP_EFFECT_%d", i+1 );

			GLGUID_MAP_EFFECT sGuidMapEffect;

			cFILE.getflag( "GUIDANCE", strTemp.GetString(), 0, 3, sGuidMapEffect.fTime );
			cFILE.getflag( "GUIDANCE", strTemp.GetString(), 1, 3, sGuidMapEffect.sMapEffect.emType );
			cFILE.getflag( "GUIDANCE", strTemp.GetString(), 2, 3, sGuidMapEffect.sMapEffect.fRate );

			if ( sGuidMapEffect.sMapEffect.emType > EMMAP_EFFECT_NONE || sGuidMapEffect.sMapEffect.emType < EMMAP_EFFECT_SIZE ) 
			{
				m_vecMapEffect.push_back( sGuidMapEffect );
			}
		}
	}

	return true;
}

bool GLGuidance::IsGuidClub(DWORD dwClubID) const
{
	if (m_dwGuidanceClub==CLUB_NULL || m_dwGuidanceClub!=dwClubID)
		return false;
    else
        return true;
}

DWORD GLGuidance::IsBattleWeekDay ( int nDayOfWeek, int nDay )
{
	if (IsBattle())
        return UINT_MAX;

	// 결정전이 있는 날인지 검사. 결정전이 오늘 있었는지 검사.
	// 과거에 결정전이 하루 두번 진행되는 문제가 있어서
	// 최종 선도전이 있었던 날을 검사한다.
	for ( DWORD i=0; i<MAX_TIME; ++i )
	{
		if ( (nDayOfWeek==m_sTIME[i].dwWeekDay) && (m_dwLastBattleDay!=nDay) )
		{
			return i;
		}
	}
	return UINT_MAX;
}

// 선도클럽 시작시간인가?
bool GLGuidance::IsBattleHour ( DWORD dwORDER, int nHour )
{
	// 선도클럽 결정전은 날짜가 다른 요일 총 4번이 가능하다.
	if ( dwORDER>=MAX_TIME )
	{
		return false;
	}
	
	if (m_sTIME[dwORDER].dwStartTime == nHour)
	{
		return true;
	}
	else
	{
		return false;
	}	
}

// 최종적으로 선도클럽결정전이 일어난 날을 세팅한다.
// 하루에 두번 일어나지 않기 위해서이다.
void GLGuidance::UpdateBattleDay ( int nDay )
{
	m_dwLastBattleDay = nDay;
}

// 선도클럽 결정전 남은 시간을 클라이언트에게 알린다.
void GLGuidance::UpdateNotifyBattle(GLAgentServer* pGLAgentServer, DWORD dwORDER, int nHour, int nMinute)
{
	if ( dwORDER>=MAX_TIME )	return;

	DWORD dwOTime = m_sTIME[dwORDER].dwStartTime;
	if ( dwOTime > 0 )
	{
		dwOTime -= 1;
		if ( nHour == dwOTime )
		{
			// 
			if ( !m_bNotifyOneHour )
			{
				m_bNotifyOneHour = true;

				// 모든 클라이언트에 알림.
				GLMSG::SNET_CLUB_GUID_BATTLE_START_BRD NetMsgBrd;
				NetMsgBrd.nTIME = 60-nMinute;
				NetMsgBrd.SetText(m_strName);
				NetMsgBrd.dwGuidanceID = m_dwID;
				pGLAgentServer->SENDTOALLCLIENT(&NetMsgBrd);
			}

			if ( nMinute == 30 )
			{
				if ( !m_bNotifyHalfHour )
				{
					m_bNotifyHalfHour = true;

					// 모든 클라이언트에 알림.
					GLMSG::SNET_CLUB_GUID_BATTLE_START_BRD NetMsgBrd;
					NetMsgBrd.nTIME = 30;
					NetMsgBrd.SetText(m_strName);
					NetMsgBrd.dwGuidanceID = m_dwID;
					pGLAgentServer->SENDTOALLCLIENT(&NetMsgBrd);
				}
			}

			if ( nMinute == 50 )
			{
				if ( !m_bNotify10MinHour )
				{
					m_bNotify10MinHour = true;

					// 모든 클라이언트에 알림.
					GLMSG::SNET_CLUB_GUID_BATTLE_START_BRD NetMsgBrd;
					NetMsgBrd.nTIME = 10;
					NetMsgBrd.SetText(m_strName);
					NetMsgBrd.dwGuidanceID = m_dwID;
					pGLAgentServer->SENDTOALLCLIENT(&NetMsgBrd);
				}
			}
		}
	}
}

void GLGuidance::UpdateCommission(GLAgentServer* pGLAgentServer, float fElaps)
{
	m_NewCommission.AddCommissionTimer(fElaps);
	
	if (m_NewCommission.CommissionTimer() > 600.0f)
	{
		// 수수료 변경.
		ChangeCommission(pGLAgentServer, m_NewCommission.NewCommissionPercent());

		// 수수료 변경 필드에 알림.
		GLMSG::NET_CLUB_GUID_COMMISSION_FLD Msg(m_dwID, m_fCommissionPercent);
		pGLAgentServer->SENDTOALLCHANNEL(&Msg);

        // 수수료 변경 로그
		// 통계 및 로그 수정
        pGLAgentServer->LogCharAction(
            m_NewCommission.ReqChaDbNum(),
            gamelog::ACT_CLUB_COMMISSION_CHANGE,
            gamelog::ID_CLUB,
            m_dwGuidanceClub,
            0,
            0,
            m_dwID,
            static_cast<int> (m_fCommissionPercent), MapID( SNATIVEID( false ), SNATIVEID( false ) ));

		// 클라이언트들에게 알림.
		GLMSG::NET_CLUB_GUID_COMMISSION_BRD NetMsgBrd(m_fCommissionPercent);

		std::vector<DWORD>::size_type nSize = m_vecMaps.size();
		for (std::vector<DWORD>::size_type i=0; i<nSize; ++i)
		{
			SNATIVEID nidMAP = m_vecMaps[i];
			GLAGLandMan* pLand = pGLAgentServer->GetLand(nidMAP);
			if (pLand)
			    pLand->SENDTOALLCLIENT(&NetMsgBrd);
		}

		// 리셋.
        m_NewCommission.SetData(0, false, 0.0f, 0.0f);
	}
}

// 선도클럽 결정전을 시작한다.
void GLGuidance::DoBattleStart (GLAgentServer* pGLAgentServer, DWORD dwORDER, int nDay )
{
	// 결정전을 시작시킴.
	m_bBattle = true;
	m_dwBattleOrder = dwORDER;
    m_fBattleTimer = 0.0f;
    m_fNotifyRemainTimer = 0.0f;

	UpdateBattleDay ( nDay );

	// 선도 클럽 변경을 필드서버에 알림.
	//
	GLMSG::NET_CLUB_CERTIFIED_AF NetMsgGuidFld;
	NetMsgGuidFld.m_GuidanceId = m_dwID;
	NetMsgGuidFld.m_ClubDbNum = m_dwGuidanceClub;
	pGLAgentServer->SENDTOALLCHANNEL ( &NetMsgGuidFld );

	// 수수료 변경 필드에 알림.
	GLMSG::NET_CLUB_GUID_COMMISSION_FLD NetMsgCommissionFld(m_dwID, m_fCommissionPercent);
	pGLAgentServer->SENDTOALLCHANNEL(&NetMsgCommissionFld);

	// 클라이언트들에게 알림.
	GLMSG::NET_CLUB_GUID_COMMISSION_BRD NetMsgCmBrd(m_fCommissionPercent);

	std::vector<DWORD>::size_type nSize = m_vecMaps.size();
	for ( std::vector<DWORD>::size_type i=0; i<nSize; ++i )
	{
		SNATIVEID nidMAP = (m_vecMaps[i]);
		GLAGLandMan *pLand = pGLAgentServer->GetLand ( nidMAP );
		if ( !pLand )	continue;

		pLand->SENDTOALLCLIENT ( &NetMsgCmBrd );
	}

	// 전투 시작을 알림.
	//	0번 채널에만 시작하는것을 알려준다.
	//	선도전은 0번 채널에서만 하니깐...
	GLMSG::SNET_CLUB_GUID_BATTLE_START_FLD NetMsgFld;
	NetMsgFld.dwID = m_dwID;
	pGLAgentServer->SENDTOCHANNEL ( &NetMsgFld, 0 );

	// 모든 클라이언트에 알림.
	GLMSG::SNET_CLUB_GUID_BATTLE_START_BRD NetMsgBrd;
	NetMsgBrd.nTIME = 0;
	NetMsgBrd.SetText(m_strName);
	NetMsgBrd.dwGuidanceID = m_dwID;
	pGLAgentServer->SENDTOALLCLIENT ( &NetMsgBrd );
}

void GLGuidance::DoBattleEnd(GLAgentServer* pGLAgentServer)
{
	// 결정전이 종료.
	m_bBattle = (false);
	m_dwBattleOrder = UINT_MAX;

	m_bNotifyOneHour = (false);
	m_bNotifyHalfHour = (false);
	m_bNotify10MinHour = (false);
	m_fBattleTimer = 0.0f;
    m_fNotifyRemainTimer = 0.0f;
	// 결정전 종료를 알림
    std::string strClubName;
    DWORD GuidClubDbNum = 0;
	std::tr1::shared_ptr<GLClubAgent> spGuidClub = pGLAgentServer->GetClub(m_dwGuidanceClub);
    if (spGuidClub)
    {
        // 로그:선도클럽 선정됨
		// 통계 및 로그 수정
        pGLAgentServer->LogCharAction(
            spGuidClub->MasterCharDbNum(),
            gamelog::ACT_CLUB_GUIDANCE,
            gamelog::ID_CLUB,
            spGuidClub->DbNum(),
            0,
            0,
            0,
            static_cast<int> (m_fCommissionPercent), MapID( SNATIVEID( false ), SNATIVEID( false ) ));
        strClubName = spGuidClub->Name();

        GuidClubDbNum = spGuidClub->DbNum();
        // Club log
        //pGLAgentServer->SENDCLUBLOG(spGuidClub->DbNum(), club::LOG_GUIDANCE, );
    }

	// 전투 종료를 알림.
	GLMSG::SNET_CLUB_GUID_BATTLE_END_FLD NetMsgFld;
	NetMsgFld.dwID = m_dwID;
	pGLAgentServer->SENDTOCHANNEL(&NetMsgFld, 0);

	// 모든 클라이언트에 알림.
	GLMSG::SNET_CLUB_GUID_BATTLE_END_BRD NetMsgBrd;
	NetMsgBrd.SetName(m_strName.c_str());
	NetMsgBrd.SetClubName(strClubName.c_str());
	NetMsgBrd.dwGuidanceID = m_dwID;
	pGLAgentServer->SENDTOALLCLIENT(&NetMsgBrd);

	// 모든 클라이언트에게 현재 선도구역은 무슨 국기로 변경된다고 알림.
	{
		wb::SNET_WB_CLUB_BATTLE_SET_FLAG_FAC_BRD sFlagNetMsgBrd;
		sFlagNetMsgBrd.dwGuidanceID = m_dwID;
		sFlagNetMsgBrd.dwCountryCode = GuidClubDbNum;
		pGLAgentServer->SENDTOALLCLIENT(&sFlagNetMsgBrd);
	}
}

void GLGuidance::DoCertifyEnd()
{
	m_dwCERTIFY_CHARID = 0;
	m_dwCERTIFY_NPCID = 0;
	m_fCERTIFY_TIMER = 0.0f;
	m_vCERTIFY_POS = D3DXVECTOR3(0,0,0);	
}

void GLGuidance::CheckExtraGuild(GLGaeaServer* pGLGaeaServer, float fElaps)
{
	m_fCHECK_TIMER += fElaps;
	if ( m_fCHECK_TIMER > m_fCHECK_TIME )
	{
		if ( m_fCHECK_TIME >= 30.0f )
		{
			m_bBattleEndCheck = false;
			m_fCHECK_TIMER = 0.0f;
		}
		GLLandMan *pLandMan = pGLGaeaServer->GetLand ( m_dwClubMap );
		if ( pLandMan )
		{
			pLandMan->DoGateOutPCAll ( m_dwGuidanceClub, m_dwCLubMapGate );
		}
		m_fCHECK_TIME += 10.0f;
	}
}

bool GLGuidance::ChangeCommission(GLAgentServer* pGLAgentServer, float fRate)
{
	if (0 > fRate)
        fRate = 0.0f;
	if (GLCONST_CHAR::fMAX_COMMISSION < fRate)
        fRate = GLCONST_CHAR::fMAX_COMMISSION;

	m_fCommissionPercent = fRate;

	// DB에 반영 필요.
	pGLAgentServer->AddGameAdoJob(
        db::DbActionPtr(
            new db::ClubRegionSet(m_dwID, m_dwGuidanceClub, m_fCommissionPercent)));	
	return true;
}

bool GLGuidance::ChangeGuidClub(GLAgentServer* pGLAgentServer, DWORD dwClubID)
{
	m_dwGuidanceClub = dwClubID;
	m_fCommissionPercent = m_fDefaultCommissionPer;

	// DB에 반영 필요.
	pGLAgentServer->AddGameAdoJob(
        db::DbActionPtr(
            new db::ClubRegionSet(m_dwID, m_dwGuidanceClub, m_fCommissionPercent)));
	return true;
}

void GLGuidance::AddKillLog( DWORD dwKillChar, DWORD dwDeathChar )
{
	GUID_KILL_LOG_MAP_ITER pos = m_mapKillLog.find( dwKillChar );
	
	//	입력되어 있다면.
	if ( pos != m_mapKillLog.end() ) 
	{
		SGUID_KILL_LOG& sKillLog = pos->second;
		sKillLog.wKill++;
	}
	else
	{
		SGUID_KILL_LOG sKillLogAdd;
		sKillLogAdd.wKill++;

		m_mapKillLog.insert( std::pair<DWORD,SGUID_KILL_LOG>(dwKillChar,sKillLogAdd) );
	}

	pos = m_mapKillLog.find( dwDeathChar );
	
	//	입력되어 있다면.
	if ( pos != m_mapKillLog.end() ) 
	{
		SGUID_KILL_LOG& sKillLog = pos->second;
		sKillLog.wDeath++;
	}
	else
	{
		SGUID_KILL_LOG sKillLogAdd;
		sKillLogAdd.wDeath++;

		m_mapKillLog.insert( std::pair<DWORD,SGUID_KILL_LOG>(dwDeathChar,sKillLogAdd) );
	}
}

void GLGuidance::CheckMapEffect(GLGaeaServer* pGLGaeaServer, float fElaps)
{
	m_fHoldCertifyTime += fElaps;

	if ( !m_bMapEffectUse )
        return;

	if ( m_dwGuidanceClub == CLUB_NULL )
        return;
	
	//	랜드 이펙트 추가되는지 체크
	for ( int i = m_nMapEffectIndex+1; i < (int)m_vecMapEffect.size(); ++i ) 
	{
		if ( m_vecMapEffect[i].fTime < m_fHoldCertifyTime ) 
		{
			m_nMapEffectIndex = i;


			//	지정된 클럽원 및 동맹원에게 버프를 걸어준다.
			GLLandMan *pLandMan = pGLGaeaServer->GetLand ( m_dwClubMap );
			if ( pLandMan )
			{
				pLandMan->DoMapEffectPC ( m_dwGuidanceClub, m_vecMapEffect[i].sMapEffect );
				
                std::string strText = sc::string::format(
                    ID2SERVERTEXT("APPLY_GUID_MAP_EFFECT"), 
					ID2SERVERTEXT("MAP_EFFECT", m_vecMapEffect[i].sMapEffect.emType-1 ),
                    m_vecMapEffect[i].sMapEffect.fRate * 100.0f);

				GLMSG::SNET_SERVER_GENERALCHAT SendData;
				if (SendData.SETTEXT(strText))
                {
                    msgpack::sbuffer SendBuffer;
                    msgpack::pack(SendBuffer, SendData);                         
				    pGLGaeaServer->SENDTOCLIENT_ONMAP(m_dwClubMap, NET_MSG_SERVER_GENERALCHAT, SendBuffer);
                }
			}
			
		}
		else
		{
			break;
		}
	}
}

GLGuidanceMan::GLGuidanceMan()
{
}

GLGuidanceMan::~GLGuidanceMan()
{
}

bool GLGuidanceMan::Load(std::vector<std::string>& vecFiles, bool bToolMode)
{
	std::vector<std::string>::size_type i = 0, j = vecFiles.size();
	for( ; i < j; ++i )
	{
		GLGuidance sGuidance;
		bool bOK = sGuidance.Load(vecFiles[i], bToolMode);
		if( !bOK )
		{
			sc::writeLogError(
                sc::string::format(
                    "sGuidance.Load() fail %1%", vecFiles[i]));
		}

		m_vecGuidance.push_back ( sGuidance );
	}

	std::sort( m_vecGuidance.begin(), m_vecGuidance.end() );

	return true;
}

GLGuidance* GLGuidanceMan::Find ( DWORD dwID )
{
	GLGuidance cObj;
	cObj.m_dwID = dwID;

	GUIDANCE_VEC_ITER pos = std::lower_bound ( m_vecGuidance.begin(), m_vecGuidance.end(), cObj );
	if ( pos==m_vecGuidance.end() )
	{
		return NULL;
	}
	else
	{
		return &(*pos);
	}
}

bool GLGuidanceMan::SetState(const VECGUID_DB& vecGUID)
{
	for (VECGUID_DB::size_type i=0; i<vecGUID.size(); ++i)
	{
		const GLGUID_DB& sGUID = vecGUID[i];
		
		// 선도클럽 데이터를 DB 에서 가져와서 선도클럽 맵이 존재하는지 검사한다.
		GLGuidance* pGuid = Find(sGUID.m_dwID);
		if (!pGuid)
		{            
            std::string Msg(
                sc::string::format(
                    "GLGuidance::Find() fail. Guidance ID %1%", sGUID.m_dwID));
			MessageBox(NULL, Msg.c_str(), "ERROR", MB_OK);
            sc::writeLogError(Msg);
			continue;
		}
		pGuid->m_dwGuidanceClub = sGUID.m_dwCLUBID;
		pGuid->m_fCommissionPercent = sGUID.m_fCommissionPer;
	}
	return true;
}

bool GLGuidanceMan::IsBattle() const
{
	for (GUIDANCE_VEC::size_type i=0; i<m_vecGuidance.size(); ++i)
	{
        const GLGuidance &sGuidance = m_vecGuidance[i];	
		if (sGuidance.m_bBattle)
            return true;
	}
	return false;
}

bool GLGuidanceMan::IsCheckExtraGuild()
{
	for (GUIDANCE_VEC::size_type i=0; i<m_vecGuidance.size(); ++i)
	{
		GLGuidance &sGuidance = m_vecGuidance[i];	
		if (sGuidance.m_bBattleEndCheck)
            return true;
	}
	return false;
}

std::vector<GLGuidance>& GLGuidanceMan::GetVecGLGuidance()
{
	return m_vecGuidance;
}

