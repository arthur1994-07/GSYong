#include "pch.h"
#include <algorithm>
#include "../../EngineLib/G-Logic/GLDefine.h"
#include "../../EngineLib/Common/gltexfile.h"
#include "../../EngineLib/Common/IniLoader.h"
#include "../../EngineLib/Common/profile.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"

#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/GLogicDataMan.h"

#include "../Land/GLLandMan.h"
#include "../FieldServer/GLGaeaServer.h"
#include "../AgentServer/GLAgentServer.h"
#include "../Club/GLClubManField.h"
#include "../Club/GLClubField.h"
#include "../Util/GLItemLimit.h"
#include "../Database/DBAction/DbActionLog.h"
#include "../Database/DBAction/DbActionGameItem.h"
#include "./GLClubDeathMatch.h"


#include "../../SigmaCore/DebugInclude.h"


const float fLOG_TIME = 300.0f;

GLClubDeathMatch::GLClubDeathMatch () :
	m_dwID(UINT_MAX),
	
	m_dwClubMap(0),
	m_dwCLubMapGate(0),
	m_dwClubHallMap(0),
	m_dwClubHallGate(0),
	m_dwBattleTime(3600),
	m_bLog(FALSE),

	m_bNotifyOneHour(false),
	m_bNotifyHalfHour(false),
	m_bNotify10MinHour(false),
	
	m_emBattleState(BATTLE_NOSTART),
	m_dwBattleOrder(0),
	m_dwLastBattleDay(0),
	m_fBattleTimer(0.0f),
    m_fNotifyRemainTimer(0.0f),

	m_fCHECK_TIMER(0.0f),
	m_fCHECK_TIME(0.0f),
	m_fRankingUpdate(0.0f),
	m_bFieldUsed(false),
	m_fReadyTimer(0.0f),
	m_bGMReady(false),
	m_fLog_TIMER(0.0f),
	m_fMemberCheck(10.0f),
	m_fMemberCheckTimer(0.0f)
{
}

GLClubDeathMatch& GLClubDeathMatch::operator= ( const GLClubDeathMatch& value )
{
	m_dwID = value.m_dwID;
	m_strName = value.m_strName;

	m_dwClubMap = value.m_dwClubMap;
	m_dwCLubMapGate = value.m_dwCLubMapGate;
	m_dwClubHallMap = value.m_dwClubHallMap;
	m_dwClubHallGate = value.m_dwClubHallGate;

	m_dwBattleTime = value.m_dwBattleTime;

	m_bLog = value.m_bLog;

	for ( int i=0; i<MAX_TIME; ++i )
		m_sCdmTIME[i] = value.m_sCdmTIME[i];


	m_sCdmAwardItem = value.m_sCdmAwardItem;

	m_bNotifyOneHour = value.m_bNotifyOneHour;
	m_bNotifyHalfHour = value.m_bNotifyHalfHour;
	m_bNotify10MinHour = value.m_bNotify10MinHour;

	m_emBattleState = value.m_emBattleState;
	m_dwLastBattleDay = value.m_dwLastBattleDay;
	m_fBattleTimer = value.m_fBattleTimer;

	m_fCHECK_TIMER = value.m_fCHECK_TIMER;
	m_fCHECK_TIME = value.m_fCHECK_TIME;

	m_vecCdmAwardChar = value.m_vecCdmAwardChar;

	m_fRankingUpdate = value.m_fRankingUpdate;

	m_fMemberCheck = value.m_fMemberCheck;
	m_fMemberCheckTimer = value.m_fMemberCheckTimer;

	m_fLog_TIMER = value.m_fLog_TIMER;

	m_mapKillLog = value.m_mapKillLog;
	m_mapCharLog = value.m_mapCharLog;

	m_mapAccrueLog = value.m_mapAccrueLog;

	m_fReadyTimer = value.m_fReadyTimer;
	m_bGMReady = value.m_bGMReady;

	return *this;
}

bool GLClubDeathMatch::Load(const std::string& strFile, bool bToolMode)
{
	if (strFile.empty())
        return FALSE;

	std::string strPath(GLOGIC::GetServerPath());
	strPath += strFile;

	CIniLoader cFILE(",", bToolMode);

	if (GLOGIC::UseLogicZipFile())
		cFILE.SetZipFile(GLOGIC::strGLOGIC_SERVER_ZIPFILE);

	if( !cFILE.open ( strPath, true ) )
	{
        sc::writeLogError(
            sc::string::format(
                "GLClubDeathMatch::Load File Open %1%", strFile));
		return false;
	}
	
	cFILE.getflag( "CLUB_DM", "ID", 0, 1, m_dwID );
	cFILE.getflag( "CLUB_DM", "NAME", 0, 1, m_strName );

	SNATIVEID nidMAP;
	cFILE.getflag( "CLUB_DM", "CLUB_MAP", 0, 2, nidMAP.wMainID );
	cFILE.getflag( "CLUB_DM", "CLUB_MAP", 1, 2, nidMAP.wSubID );
	m_dwClubMap = nidMAP.dwID;

	cFILE.getflag( "CLUB_DM", "CLUB_GATE", 0, 1, m_dwCLubMapGate );

	cFILE.getflag( "CLUB_DM", "CLUB_HALL_MAP", 0, 2, nidMAP.wMainID );
	cFILE.getflag( "CLUB_DM", "CLUB_HALL_MAP", 1, 2, nidMAP.wSubID );
	m_dwClubHallMap = nidMAP.dwID;

	cFILE.getflag( "CLUB_DM", "CLUB_HALL_GATE", 0, 1, m_dwClubHallGate );

	cFILE.getflag( "CLUB_DM", "CLUB_LOG", 0, 1, m_bLog );

	cFILE.getflag( "CLUB_DM", "CLUB_MEMBER_CHECK_TIME", 0, 1, m_fMemberCheck );

	DWORD dwNUM = cFILE.GetKeySize( "CLUB_DM", "BATTLE_TIME" );
	if( dwNUM > 4 )
	{
        sc::writeLogError(
            sc::string::format(
                "GLClubDeathMatch::Load %1% It was wrong size of BATTLE_TIME", strFile));
		return false;
	}

	for( DWORD i=0; i<dwNUM; ++i )
	{
		cFILE.getflag( i, "CLUB_DM", "BATTLE_TIME", 0, 2, m_sCdmTIME[i].dwWeekDay );
		cFILE.getflag( i, "CLUB_DM", "BATTLE_TIME", 1, 2, m_sCdmTIME[i].dwStartTime );
	}

	cFILE.getflag( "CLUB_DM", "BATTLE_THE_TIME", 0, 1, m_dwBattleTime );

	cFILE.getflag( "CLUB_DM", "AWARD_ITEM_LIMIT", 0, 1, m_sCdmAwardItem.dwAwardLimit );

	if ( m_sCdmAwardItem.dwAwardLimit > RANKING_NUM ) m_sCdmAwardItem.dwAwardLimit = RANKING_NUM;

	for ( DWORD i = 0; i < m_sCdmAwardItem.dwAwardLimit; ++i )
	{
		CString strTemp;
		strTemp.Format( "AWARD_ITEM_%d", i+1 );

		cFILE.getflag( "CLUB_DM", strTemp.GetString(), 0, 2, m_sCdmAwardItem.nAwardItem[i].wMainID );
		cFILE.getflag( "CLUB_DM", strTemp.GetString(), 1, 2, m_sCdmAwardItem.nAwardItem[i].wSubID );

	}

	return true;
}

DWORD GLClubDeathMatch::IsBattleWeekDay ( int nDayOfWeek, int nDay )
{
	if ( IsBattle() )	return UINT_MAX;

	//	Note : �������� �ִ� ������ �˻�. �������� ���� �־����� �˻�.
	for ( DWORD i=0; i<MAX_TIME; ++i )
	{
		if ( (nDayOfWeek==m_sCdmTIME[i].dwWeekDay) && (m_dwLastBattleDay!=nDay) )
		{
			return i;
		}
	}
	return UINT_MAX;
}

// ����Ŭ�� ���۽ð��ΰ�?
bool GLClubDeathMatch::IsBattleHour ( DWORD dwORDER, int nHour )
{
	// ����Ŭ�� �������� ��¥�� �ٸ� ���� �� 4���� �����ϴ�.
	if ( dwORDER>=MAX_TIME )
	{
		return false;
	}
	
	if (m_sCdmTIME[dwORDER].dwStartTime == nHour)
	{
		return true;
	}
	else
	{
		return false;
	}	
}

// ���������� ����Ŭ���������� �Ͼ ���� �����Ѵ�.
// �Ϸ翡 �ι� �Ͼ�� �ʱ� ���ؼ��̴�.
void GLClubDeathMatch::UpdateBattleDay ( int nDay )
{
	m_dwLastBattleDay = nDay;
}

// ����Ŭ�� ������ ���� �ð��� Ŭ���̾�Ʈ���� �˸���.
void GLClubDeathMatch::UpdateNotifyBattle(GLAgentServer* pServer, DWORD dwORDER, int nHour, int nMinute)
{
	if (dwORDER >= MAX_TIME)
        return;

	DWORD dwOTime = m_sCdmTIME[dwORDER].dwStartTime;
	if (dwOTime <= 0)
        return;
	
	dwOTime -= 1;
	if ( nHour == dwOTime )
	{
		//	Note : 
		if ( !m_bNotifyOneHour )
		{
			m_bNotifyOneHour = true;

			//	Note : ��� Ŭ���̾�Ʈ�� �˸�.
			GLMSG::SNET_CLUB_DEATHMATCH_START_BRD NetMsgBrd;
			NetMsgBrd.nTIME = 60-nMinute;
			NetMsgBrd.SetText(m_strName);
            pServer->SENDTOALLCLIENT(&NetMsgBrd);
		}

		if ( nMinute == 30 )
		{
			if ( !m_bNotifyHalfHour )
			{
				m_bNotifyHalfHour = true;

				//	Note : ��� Ŭ���̾�Ʈ�� �˸�.
				GLMSG::SNET_CLUB_DEATHMATCH_START_BRD NetMsgBrd;
				NetMsgBrd.nTIME = 30;
				NetMsgBrd.SetText(m_strName);
				pServer->SENDTOALLCLIENT(&NetMsgBrd);
			}
		}

		if ( nMinute == 50 )
		{
			if ( !m_bNotify10MinHour )
			{
				m_bNotify10MinHour = true;
				m_emBattleState = BATTLE_READY;

				//	Note : ��� Ŭ���̾�Ʈ�� �˸�.
				GLMSG::SNET_CLUB_DEATHMATCH_START_BRD NetMsgBrd;
				NetMsgBrd.nTIME = 10;
				NetMsgBrd.SetText(m_strName);
				pServer->SENDTOALLCLIENT(&NetMsgBrd);

				GLMSG::SNET_CLUB_DEATHMATCH_READY_FLD NetMsgFld;
				NetMsgFld.dwID = m_dwID;
				pServer->SENDTOCHANNEL(&NetMsgFld, 0);
			}
		}
	}

}

// CDM�� �����Ѵ�.
void GLClubDeathMatch::DoBattleStart(GLAgentServer* pServer, DWORD dwORDER, int nDay)
{
	//	Note : �������� ���۽�Ŵ.
	m_emBattleState = BATTLE_START;
	m_dwBattleOrder = dwORDER;
    m_fNotifyRemainTimer = 0.0f;
	UpdateBattleDay ( nDay );

	//	Note : ���� ������ �˸�.
	//	CDM�� 0��ä�ο����� �����Ѵ�.
	GLMSG::SNET_CLUB_DEATHMATCH_START_FLD NetMsgFld;
	NetMsgFld.dwID = m_dwID;
	pServer->SENDTOCHANNEL ( &NetMsgFld, 0 );

	//	Note : ��� Ŭ���̾�Ʈ�� �˸�.
	GLMSG::SNET_CLUB_DEATHMATCH_START_BRD NetMsgBrd;
	NetMsgBrd.nTIME = 0;
	NetMsgBrd.SetText(m_strName);
	pServer->SENDTOALLCLIENT(&NetMsgBrd);
}

void GLClubDeathMatch::DoBattleEnd(GLAgentServer* pServer)
{
	//	Note : �������� ����.
	m_emBattleState = BATTLE_END;
	m_dwBattleOrder = UINT_MAX;

	m_bNotifyOneHour = false;
	m_bNotifyHalfHour = false;
	m_bNotify10MinHour = false;
	m_fBattleTimer = 0.0f;
    m_fNotifyRemainTimer = 0.0f;


	//	Note : ���� ���Ḧ �˸�.
	GLMSG::SNET_CLUB_DEATHMATCH_END_FLD NetMsgFld;
	NetMsgFld.dwID = m_dwID;
	pServer->SENDTOCHANNEL(&NetMsgFld, 0);
}
/*
void GLClubDeathMatch::CheckAwardClub( float fElaps )
{
	m_fCHECK_TIMER += fElaps;
	if ( m_fCHECK_TIMER > m_fCHECK_TIME )
	{
		//	30�� ������ ��������
		if ( m_fCHECK_TIME < 30.0f )
		{
			//���� ����
			CDMAwardItem();
			m_fCHECK_TIME += 10.0f;
			return;
		}
		else if ( m_fCHECK_TIME == 300.0f )
		{
			SetBattle( GLClubDeathMatch::BATTLE_END );
			m_fCHECK_TIMER = 0.0f;
			m_fCHECK_TIME = 0.0f;
			return;
		}
		else m_fCHECK_TIME = 300.0f;
	}
}
*/
/*
void GLClubDeathMatch::CheckExtraClub( float fElaps )
{
	m_fCHECK_TIMER += fElaps;
	if ( m_fCHECK_TIMER > m_fCHECK_TIME )
	{
		//	30�� ������ ��������
		if ( m_fCHECK_TIME < 30.0f )
		{
			//���� ����
			GLLandMan *pLandMan = m_pGLGaeaServer->GetLand ( m_dwClubMap );
			if ( pLandMan )
			{
				pLandMan->DoGateOutPCAll ( m_dwCLubMapGate );
			}
			
			m_fCHECK_TIME += 10.0f;
			return;
		}
		else
		{
			m_fCHECK_TIMER = 0.0f;
            m_fCHECK_TIME = 0.0f;
			SetBattle( GLClubDeathMatch::BATTLE_NOSTART );
		}
	}
}
*/
bool GLClubDeathMatch::IsEnterMap( DWORD dwClubID )
{
	if ( dwClubID == CLUB_NULL )
        return false;
	if ( IsBattle() )
        return true;
	if ( IsBattleReady() )
        return true;

	return false;
}

SCDM_RANK_INFO* GLClubDeathMatch::GetCDMRankInof( DWORD dwClubID )
{
	CDM_RANK_INFO_MAP_ITER pos = m_mapCdmScore.find( dwClubID );
	if( pos == m_mapCdmScore.end() )
	{
		return NULL;
	}
	
	return &(*pos).second;;
}
/*
void GLClubDeathMatch::AddCDMScore( DWORD dwKillClub, DWORD dwDeathClub, DWORD dwKillChar, DWORD dwDeathChar )
{
	SCDM_RANK_INFO* pKillCdmRankInfo = GetCDMRankInof( dwKillClub );
	if ( pKillCdmRankInfo )
	{
		pKillCdmRankInfo->wKillNum++;
	}
	else
	{

		SCDM_RANK_INFO sCdmRankInfo;		
		sCdmRankInfo.dwClubID = dwKillClub;
		sCdmRankInfo.wKillNum++;

		std::tr1::shared_ptr<GLClubServer> pClub = m_pGLGaeaServer->GetClub( dwKillClub );
		if ( pClub )
		{
			StringCchCopy ( sCdmRankInfo.szClubName, CLUB_NAME+1, pClub->m_szName );
			sCdmRankInfo.szClubName[CLUB_NAME] = '\0';
		}	
	
		m_mapCdmScore[sCdmRankInfo.dwClubID] = sCdmRankInfo;
	}

	SCDM_RANK_INFO* pDeathCdmRankInfo = GetCDMRankInof( dwDeathClub );
	if ( pDeathCdmRankInfo )
	{
		pDeathCdmRankInfo->wDeathNum++;
	}
	else
	{
		
		SCDM_RANK_INFO sCdmRankInfo;		
		sCdmRankInfo.dwClubID = dwDeathClub;
		sCdmRankInfo.wDeathNum++;

		std::tr1::shared_ptr<GLClubServer> pClub = m_pGLGaeaServer->GetClub( dwDeathClub );
		if ( pClub )
		{
			StringCchCopy ( sCdmRankInfo.szClubName, CLUB_NAME+1, pClub->m_szName );
			sCdmRankInfo.szClubName[CLUB_NAME] = '\0';
		}
		
	
		m_mapCdmScore[sCdmRankInfo.dwClubID] = sCdmRankInfo;
	}



	//	Kill Log �Է�
	CDM_KILL_LOG_MAP_ITER pos = m_mapKillLog.find( dwKillChar );
	
	//	�ԷµǾ� �ִٸ�.
	if ( pos != m_mapKillLog.end() ) 
	{
		SCDM_KILL_LOG& sKillLog = pos->second;
		sKillLog.wKill++;
	}
	else
	{
		SCDM_KILL_LOG sKillLogAdd;
		sKillLogAdd.wKill++;

		m_mapKillLog.insert( std::pair<DWORD,SCDM_KILL_LOG>(dwKillChar,sKillLogAdd) );
	}


	pos = m_mapKillLog.find( dwDeathChar );
	
	//	�ԷµǾ� �ִٸ�.
	if ( pos != m_mapKillLog.end() ) 
	{
		SCDM_KILL_LOG& sKillLog = pos->second;
		sKillLog.wDeath++;
	}
	else
	{
		SCDM_KILL_LOG sKillLogAdd;
		sKillLogAdd.wDeath++;

		m_mapKillLog.insert( std::pair<DWORD,SCDM_KILL_LOG>(dwDeathChar,sKillLogAdd) );
	}


	//	���� ���� ī��Ʈ üũ
	CDM_ACCRUE_COUNT_MAP_ITER pos2 = m_mapAccrueLog.find( dwDeathChar );

	if ( pos2 != m_mapAccrueLog.end() ) 
	{
		SCDM_ACCRUE_COUNT& sLog = pos2->second;
		
		//	�ٸ� Ŭ���� ������ ���� ī��Ʈ�� ���½�Ų��.
		if ( sLog.dwKillClub == dwKillClub )
		{
			sLog.dwKillCount++;
		}
		else
		{
			sLog.dwKillClub = dwKillClub;
			sLog.dwKillCount = 1;
		}
	}
	else
	{
		SCDM_ACCRUE_COUNT sLog;
		sLog.dwKillClub = dwKillClub;
		sLog.dwKillCount = 1;

		m_mapAccrueLog.insert( std::pair<DWORD,SCDM_ACCRUE_COUNT>(dwDeathChar,sLog) );
	}
}
*/
DWORD GLClubDeathMatch::GetAccrueCount( DWORD dwCharID )
{
	CDM_ACCRUE_COUNT_MAP_ITER pos = m_mapAccrueLog.find( dwCharID );
	if ( pos != m_mapAccrueLog.end() ) 
	{
		const SCDM_ACCRUE_COUNT& sLog = pos->second;
		return sLog.dwKillCount;
	}
	
	//	������Ȳ�ε� �׳� 1�� ó��
	return 1;
}


bool GLClubDeathMatchMan::Load(const std::vector<std::string>& vecFiles, bool bToolMode)
{
	std::vector<std::string>::size_type i = 0, j = vecFiles.size();
	for( ; i < j; ++i )
	{
		GLClubDeathMatch sClubDM;
		bool bOK = sClubDM.Load(vecFiles[i], bToolMode);
		if (bOK)
        {
            m_vecClubDM.push_back ( sClubDM );
        }
        else
        {
            sc::writeLogError(
                sc::string::format(
                    "sClubDeathMatch::Load fail %1%", vecFiles[i]));
        }		
	}

	std::sort( m_vecClubDM.begin(), m_vecClubDM.end() );

	return true;
}

GLClubDeathMatch* GLClubDeathMatchMan::Find ( DWORD dwID )
{
	GLClubDeathMatch cObj;
	cObj.m_dwID = dwID;

	CDM_VEC_ITER pos = std::lower_bound ( m_vecClubDM.begin(), m_vecClubDM.end(), cObj );
	if ( pos==m_vecClubDM.end() )
	{
		return NULL;
	}
	else
	{
		return &(*pos);
	}
}

bool GLClubDeathMatchMan::IsBattle() const
{
	for ( CDM_VEC::size_type i=0; i<m_vecClubDM.size(); ++i )
	{
		const GLClubDeathMatch& sClubDM = m_vecClubDM[i];	
		if ( sClubDM.IsBattle() )
            return true;
	}
	return false;
}

bool GLClubDeathMatchMan::IsBattleReady() const
{
    for ( CDM_VEC::size_type i=0; i<m_vecClubDM.size(); ++i )
    {
        const GLClubDeathMatch& sClubDM = m_vecClubDM[i];	
        if ( sClubDM.IsBattleReady() )
            return true;
    }
    return false;
}

bool GLClubDeathMatchMan::IsBattle ( DWORD dwID )
{
	GLClubDeathMatch* pClubDM = Find( dwID );
	if ( !pClubDM ) return false;
	
	if ( pClubDM->IsBattle() ) return true;

	return false;
}


bool GLClubDeathMatchMan::IsBattleEnd ( DWORD dwID )
{
	GLClubDeathMatch* pClubDM = Find( dwID );
	if ( !pClubDM ) return false;
	
	if ( pClubDM->IsBattleEnd() ) return true;

	return false;
}
/*
void GLClubDeathMatch::UpdateCDMRanking()
{
	if ( !IsBattle() ) return;
	if ( !IsFieldUsed() ) return;

	CDM_RANK_INFO_VEC	m_vecCdmRankNew;
	m_vecCdmRankNew.reserve( m_mapCdmScore.size() );

	CDM_RANK_INFO_MAP_ITER	pos = m_mapCdmScore.begin();
	CDM_RANK_INFO_MAP_ITER	end = m_mapCdmScore.end();

	for ( ; pos != end; pos++ )
	{
		const SCDM_RANK_INFO&	sRankInfo = pos->second;					
		m_vecCdmRankNew.push_back( sRankInfo );
	}
	
	std::sort( m_vecCdmRankNew.begin(), m_vecCdmRankNew.end() );	

	// ��ŷ ���
	int nSize = (int)m_vecCdmRankNew.size();

	for ( int i = 0; i < nSize; ++i )
	{
		m_vecCdmRankNew[i].wClubRanking = i+1;
		m_vecCdmRankNew[i].nIndex = i;
	}

	//	������Ȳ ��ŷ ��� �Ʒ� ��ŷ�� ����
	for ( int i = nSize-1; i > 0; --i )
	{
		if ( m_vecCdmRankNew[i] == m_vecCdmRankNew[i-1] )
		{
			m_vecCdmRankNew[i-1].wClubRanking = m_vecCdmRankNew[i].wClubRanking;			
		}
	}	
	
	
	GLMSG::SNET_CLUB_DEATHMATCH_RANKING_UPDATE	NetMsg;

	for ( size_t i = 0; i < m_vecCdmRankNew.size(); ++i )
	{
		CDM_RANK_INFO_MAP_ITER iter = m_mapCdmScore.find( m_vecCdmRankNew[i].dwClubID );
		if ( iter == m_mapCdmScore.end() )	continue;	//	�̷����� ����� ��

		SCDM_RANK_INFO&	sRankInfo = iter->second;
		
		if ( m_vecCdmRankNew[i].nIndex != sRankInfo.nIndex || 
			 m_vecCdmRankNew[i].wClubRanking != sRankInfo.wClubRanking )
		{
			sRankInfo.wClubRanking = m_vecCdmRankNew[i].wClubRanking;
			sRankInfo.nIndex = m_vecCdmRankNew[i].nIndex;
			
			//	9�� Ŭ���� ������.
			if ( m_vecCdmRankNew[i].nIndex < RANKING_NUM ) 
			{
				SCDM_RANK sCdmRank = sRankInfo;
				NetMsg.ADDCLUB( sCdmRank );
			}
		}		
	}

	if ( NetMsg.wRankNum > 0 )	
		m_pGLGaeaServer->SENDTOCLIENT_ONMAP( m_dwClubMap, &NetMsg );


	GLMSG::SNET_CLUB_DEATHMATCH_MYRANK_UPDATE	NetMsgMy;

	pos = m_mapCdmScore.begin();
	end = m_mapCdmScore.end();

	for ( ; pos != end; pos++ )
	{
		const SCDM_RANK_INFO&	sRankInfo = pos->second;
		NetMsgMy.sMyCdmRank.wClubRanking = sRankInfo.wClubRanking;
		NetMsgMy.sMyCdmRank.wKillNum = sRankInfo.wKillNum;
		NetMsgMy.sMyCdmRank.wDeathNum = sRankInfo.wDeathNum;
		
		m_pGLGaeaServer->SENDTOCLUBCLIENT_ONMAP( m_dwClubMap, sRankInfo.dwClubID, &NetMsgMy );
	}

}
*/
/*
void GLClubDeathMatch::CDMAwardItem()
{
	if ( !IsBattleEndAward() ) return;	
	if ( !IsFieldUsed() ) return;

	GLLandMan *pLandMan = m_pGLGaeaServer->GetLand ( m_dwClubMap );
	if ( !pLandMan ) return;

	//	�ش� �ʿ� �ִ� ������ ���� ����
	{
		GLClubMan& cClubMan = m_pGLGaeaServer->GetClubMan();

		GLCHARNODE* pCharNode = pLandMan->m_GlobPCList.m_pHead;
		for ( ; pCharNode; pCharNode = pCharNode->pNext )
		{
			PGLCHAR pChar = m_pGLGaeaServer->GetChar( pCharNode->Data->m_dwGaeaID );
			if ( !pChar ) continue;

			std::tr1::shared_ptr<GLClubServer> pClub = cClubMan.GetClub( pChar->m_dwGuild );
			if ( !pClub ) continue;

			CDM_RANK_INFO_MAP_ITER iter = m_mapCdmScore.find( pClub->m_dwID );
			if ( iter != m_mapCdmScore.end() ) 
			{
				SCDM_RANK_INFO& sCdmRankInfo = iter->second;

				// ( ���� ���� ĳ�������� ���� ) 
				CDM_AWARD_CHAR_ITER iter = m_vecCdmAwardChar.find( pChar->m_dwCharID );
				if ( iter != m_vecCdmAwardChar.end() )	continue;

				int nAwardIndex;
				
				if ( sCdmRankInfo.wClubRanking > m_sCdmAwardItem.dwAwardLimit ) continue;
				nAwardIndex = sCdmRankInfo.wClubRanking - 1;
				
				SNATIVEID sNativeID = m_sCdmAwardItem.nAwardItem[nAwardIndex];
				
				const SITEM* pItem = GLogicData::GetInstance().GetItem( sNativeID );
				if ( !pItem ) continue;

				SITEMCUSTOM sITEM_NEW;
				sITEM_NEW.sNativeID = sNativeID;
				sITEM_NEW.tBORNTIME = CTime::GetCurrentTime().GetTime();
				sITEM_NEW.cGenType = EMGEN_SYSTEM;
				sITEM_NEW.cChnID = (BYTE)m_pGLGaeaServer->GetServerChannel();
				sITEM_NEW.cFieldID = (BYTE)m_pGLGaeaServer->GetFieldSvrID();
				sITEM_NEW.lnGenNum = pItemLimit->RegItemGen ( sITEM_NEW.guid );

				CItemDrop cDropItem;
				cDropItem.sItemCustom = sITEM_NEW;
				if ( pChar->IsInsertToInvenEx ( &cDropItem ) )
				{
					pChar->InsertToInvenEx ( &cDropItem );
					
					pItemLimit->ReqItemRoute ( sITEM_NEW, ID_CLUB, pChar->m_dwGuild, ID_CHAR, pChar->m_dwCharID, 
						EMITEM_ROUTE_SYSTEM, sITEM_NEW.wTurnNum );
				}
				else
				{
					pLandMan->DropItem ( pChar->m_vPos, &(cDropItem.sItemCustom), EMGROUP_ONE, pChar->m_dwGaeaID );
				}

				m_vecCdmAwardChar.insert( pChar->m_dwCharID );
			}
		}
	}

}
*/

GLClubDeathMatchAgentMan::GLClubDeathMatchAgentMan(GLAgentServer* pServer)
    : m_pServer(pServer)
{
}

GLClubDeathMatchAgentMan::~GLClubDeathMatchAgentMan()
{
}

bool GLClubDeathMatchAgentMan::SetMapState ()
{
	
	for ( CDM_VEC::size_type i=0; i<m_vecClubDM.size(); ++i )
	{
		GLClubDeathMatch &sCDM= m_vecClubDM[i];

		//	Note : CDM �Ա���.
		SNATIVEID nidHallMAP ( sCDM.m_dwClubHallMap );
		GLAGLandMan *pLandMan = m_pServer->GetLand ( nidHallMAP );
		if ( pLandMan )
		{
			pLandMan->SetGuidance(sCDM.m_dwID);
		}

		//	Note : CDM ������.
		SNATIVEID nidMAP ( sCDM.m_dwClubMap );
		pLandMan = m_pServer->GetLand ( nidMAP );
		if ( pLandMan )
		{
			pLandMan->SetGuidance(sCDM.m_dwID);
			pLandMan->SetClubDeathMatchMap(true);
		}
	}

	return true;
}

// �� CDM�����߿� Ŭ�� Ż�� �ȵǵ��� �ش޶�� ��û ���� ����[8/1/2014 hsshin];
// �� CDM���� �ش� ���尡 CDM������������ �˻��ϴ� �ڵ带 �߰���;
// �������� �ʵ弭���� ��� Land�����Ϳ� bool���� �־���� �˻縦 �ϴ� ����� ���������;
// �׷� ������� �ϸ� ������Ʈ Land�����Ϳ� ��CDM���� �����͸� �߰��ϴ� �һ�簡 �߻�������;
// �ӵ��鿡�� ��ȿ���������� �Ź� ��� CDM�����͸� �˻��ϵ��� �ۼ���.;
// �ִ��� �� CDM�� ���� ���������� �� �ڵ带 �����ϴ°� ����.;
const bool GLClubDeathMatchAgentMan::IsClubDeathMatchMap(const SNATIVEID& _baseMapID)
{
	for ( CDM_VEC::size_type i=0; i<m_vecClubDM.size(); ++i )
	{
		GLClubDeathMatch &sCDM= m_vecClubDM[i];
		//	Note : CDM ������.
		SNATIVEID nidMAP ( sCDM.m_dwClubMap );
		if ( _baseMapID == nidMAP)
			return true;
	}
	return false;
}


bool GLClubDeathMatchAgentMan::FrameMove(float fElapsedAppTime)
{
    if (!GLCONST_CHAR::bCLUB_DEATHMATCH)
        return false;
	
	// CDM�� �������� �ʴ´�.
	if (GLCONST_CHAR::bPKLESS)
        return false;

    const bool bPreviousBattle = IsBattle();
    bool bNeedNotify = false;

	for (CDM_VEC::size_type i=0; i<m_vecClubDM.size(); ++i)
	{
		GLClubDeathMatch &sClubDM = m_vecClubDM[i];

		if (sClubDM.m_bGMReady == true)
		{
			if ( sClubDM.m_emBattleState == GLClubDeathMatch::BATTLE_NOSTART)
			{
				sClubDM.m_emBattleState = GLClubDeathMatch::BATTLE_READY;

				//	Note : ��� Ŭ���̾�Ʈ�� �˸�.
				GLMSG::SNET_CLUB_DEATHMATCH_START_BRD NetMsgBrd;
				NetMsgBrd.nTIME = 10;
				NetMsgBrd.SetText(sClubDM.m_strName);
				m_pServer->SENDTOALLCLIENT(&NetMsgBrd);

				GLMSG::SNET_CLUB_DEATHMATCH_READY_FLD NetMsgFld;
				NetMsgFld.dwID = i;
				m_pServer->SENDTOCHANNEL(&NetMsgFld, 0);
			}

			sClubDM.m_fReadyTimer += fElapsedAppTime;
		}

        if (sClubDM.IsBattle())
        {
            if ( sClubDM.m_fNotifyRemainTimer > 600.0f )
            {
                bNeedNotify = true;
                sClubDM.m_fNotifyRemainTimer = 0.0f;
            }
            sClubDM.m_fBattleTimer += fElapsedAppTime;
            sClubDM.m_fNotifyRemainTimer += fElapsedAppTime;
            if ( sClubDM.m_fBattleTimer > float ( sClubDM.m_dwBattleTime ) )
            {
                sClubDM.DoBattleEnd( m_pServer );
            }
        }
        else
        {
            CTime cCurTime = CTime::GetCurrentTime();
            int nDayOfWeek = cCurTime.GetDayOfWeek ();
            int nDay = cCurTime.GetDay ();
            int nHour = cCurTime.GetHour();
            int nMinute = cCurTime.GetMinute ();

            DWORD dwORDER = sClubDM.IsBattleWeekDay(nDayOfWeek,nDay);
            if ( dwORDER!=UINT_MAX || sClubDM.m_fReadyTimer > 100.0f )
            {
                //	Note : CDM ���� ����.
                if ( sClubDM.IsBattleHour ( dwORDER, nHour ) || sClubDM.m_fReadyTimer > 100.0f )
                {
					sClubDM.m_fReadyTimer = 0;
					sClubDM.m_bGMReady = false;

                    //	Note : CDM ����.
                    sClubDM.DoBattleStart(m_pServer, dwORDER, nDay);
                    bNeedNotify = true;
                }
                else
                {
                    //	Note : CDM ���� ����.
                    sClubDM.UpdateNotifyBattle(m_pServer, dwORDER, nHour, nMinute);
                }
            }
        }
	}
    if ( bNeedNotify )   //���� ������ ����ð� ������ �ִٸ� (ex �������� �����߰ų� ������ �� 10�а����)
    {
        SendClubDeathMatch_RemainTime_BRD();
    }
    const bool CurrentBattle = IsBattle();
    if ( bPreviousBattle != CurrentBattle )
    {
        GLMSG::SNETPC_SERVER_CLUB_DEATHMATCH_INFO NetMsgClubDM;
        NetMsgClubDM.bClubDeathMatch = CurrentBattle;
        m_pServer->SENDTOALLCLIENT(&NetMsgClubDM );
    }
	return true;
}

void GLClubDeathMatchAgentMan::SendClubDeathMatch_RemainTime(DWORD ClientSlot)
{
    msgpack::sbuffer PackBuffer;
    if ( MakeClubDeathMatch_RemainTime_Packet(PackBuffer) ) //�������� �������� �ִٸ� ��Ŷ�� ������� ����
    {
        m_pServer->SENDTOCLIENT( ClientSlot, NET_MSG_GCTRL_CLUB_DEATHMATCH_REMAIN_BRD, PackBuffer );
    }
}

GLClubDeathMatchAgentMan::CDM_VEC& GLClubDeathMatchAgentMan::GetVecClubDeathMatch()
{
	return m_vecClubDM;
}

void GLClubDeathMatchAgentMan::SendClubDeathMatch_RemainTime_BRD()
{
    msgpack::sbuffer PackBuffer;
    if ( MakeClubDeathMatch_RemainTime_Packet(PackBuffer) ) //�������� �������� �ִٸ� ��Ŷ�� ������� ����
    {
        m_pServer->SENDTOALLCLIENT( NET_MSG_GCTRL_CLUB_DEATHMATCH_REMAIN_BRD, PackBuffer );
    }
}

bool GLClubDeathMatchAgentMan::MakeClubDeathMatch_RemainTime_Packet( msgpack::sbuffer& PackBuffer)
{ 
    //������ġ�� 1�� �ۿ� ������ Ŭ���������� ���¸� ���� ����� ���ÿ�
    //���� ������ �������� �����ϰ� ¥�����ִ� ���¶� ���� ������ġ�� ������ ������ �� �ִ�
    //���ɼ��� �����Ͽ� ����. ������ �̹� ���ߵ�����ġ�� �����ϰ� ��Ŷ�ʰ� Ŭ��,UI���� ������ �ϸ��.
    //������ ����.
    GLMSG::NET_CLUB_DEATHMATCH_REMAIN_BRD SendData;
    SendData.dwTime = m_vecClubDM[0].m_dwBattleTime - (DWORD)m_vecClubDM[0].m_fBattleTimer;
    msgpack::pack( PackBuffer, SendData );
    return true;
}
/*
GLClubDeathMatchFieldMan& GLClubDeathMatchFieldMan::GetInstance()
{
	static GLClubDeathMatchFieldMan cInstance;
	return cInstance;
}
*/

GLClubDeathMatchFieldMan::GLClubDeathMatchFieldMan(GLGaeaServer* pGaeaServer)
    : m_pGLGaeaServer(pGaeaServer)
{
}

bool GLClubDeathMatchFieldMan::SetMapState ()
{
	for ( CDM_VEC::size_type i=0; i<m_vecClubDM.size(); ++i )
	{
		GLClubDeathMatch &sCDM = m_vecClubDM[i];

		//	Note : CDM ���� �Ա���.
		SNATIVEID nidHallMAP ( sCDM.m_dwClubHallMap );
		GLLandMan *pLandMan = m_pGLGaeaServer->GetLand ( nidHallMAP );
		if ( pLandMan )
		{
			pLandMan->m_dwClubMapID = sCDM.m_dwID;
			pLandMan->m_bClubDeathMatchMapHall = true;			
		}

		//	Note : CDM ������.
		SNATIVEID nidMAP ( sCDM.m_dwClubMap );
		SMAPNODE *pMAPNODE = m_pGLGaeaServer->FindMapNode ( nidMAP );
		if ( pMAPNODE )
		{
			pMAPNODE->m_bCDMZone = TRUE;
		}

		pLandMan = m_pGLGaeaServer->GetLand ( nidMAP );
		if ( pLandMan )
		{
			pLandMan->m_dwClubMapID = sCDM.m_dwID;
			pLandMan->m_bClubDeathMatchMap = true;	
			sCDM.m_bFieldUsed = true;
		}
	}

	return true;
}
bool GLClubDeathMatchFieldMan::ReadyBattle( DWORD dwID )
{
	GLClubDeathMatch *pCDM = Find ( dwID );
	if ( !pCDM )	return false;

	pCDM->SetBattle( GLClubDeathMatch::BATTLE_READY );
	pCDM->m_fReadyTimer = 600.0f;

	return true;
}

bool GLClubDeathMatchFieldMan::BeginBattle ( DWORD dwID )
{
	GLClubDeathMatch *pCDM = Find ( dwID );
	if ( !pCDM )	return false;

	pCDM->SetBattle( GLClubDeathMatch::BATTLE_START );

	pCDM->m_fRankingUpdate = 0.0f;
	pCDM->m_vecCdmAwardChar.clear();
	pCDM->m_mapCdmScore.clear();	
	
	pCDM->m_fLog_TIMER = 0.0f;
	pCDM->m_mapKillLog.clear();
	pCDM->m_mapCharLog.clear();
	pCDM->m_mapAccrueLog.clear();

	return true;
}

bool GLClubDeathMatchFieldMan::EndBattle ( DWORD dwID )
{
	GLClubDeathMatch *pCDM = Find ( dwID );
	if ( !pCDM )	return false;

	UpdateCDMRanking(pCDM);

	//	������ �α� ���
	//	5�и��� ����ε� 2�� 30�� �̻� �����ٸ� ������ ����� 
	//	��ŵ�Ȱ����� ���� ����Ѵ�.
	if ( pCDM->m_fLog_TIMER > ( fLOG_TIME / 2.0f ) ) 
	{
		DoLogPrint( dwID );
		pCDM->m_fLog_TIMER = 0.0f;
	}

	//	��� �α� ����
	DoResultLog( dwID );

	pCDM->m_mapKillLog.clear();
	pCDM->m_mapCharLog.clear();	
	pCDM->m_mapAccrueLog.clear();

	pCDM->SetBattle( GLClubDeathMatch::BATTLE_END_AWARD );

	pCDM->m_fCHECK_TIME = 10.0f;
	
	CDMAwardItem(pCDM);
	
	pCDM->m_fRankingUpdate = 0.0f;


	//	Agent�� ��� ����( ��� ���� ) 

	if ( !pCDM->IsFieldUsed() ) return false;

	GLMSG::SNET_CLUB_DEATHMATCH_END_AGT NetMsg;

	NetMsg.dwCDMID = pCDM->m_dwID;

	CDM_RANK_INFO_MAP_ITER	pos = pCDM->m_mapCdmScore.begin();
	CDM_RANK_INFO_MAP_ITER	end = pCDM->m_mapCdmScore.end();

	for ( ; pos != end; pos++ )
	{
		const SCDM_RANK_INFO&	sRankInfo = pos->second;					
		
		if ( sRankInfo.nIndex < RANKING_NUM && sRankInfo.wClubRanking <= RANKING_NUM )
		{
			NetMsg.dwClubID[sRankInfo.nIndex] = sRankInfo.dwClubID;
			NetMsg.wClubRanking[sRankInfo.nIndex] = sRankInfo.wClubRanking;
			NetMsg.wKillNum[sRankInfo.nIndex] = sRankInfo.wKillNum;
			NetMsg.wDeathNum[sRankInfo.nIndex] = sRankInfo.wDeathNum;
		}
	}

	m_pGLGaeaServer->SENDTOAGENT( &NetMsg );
	return true;
}


const std::string GLClubDeathMatchFieldMan::GetName ( DWORD dwID )
{
	GLClubDeathMatch *pCDM = Find ( dwID );
	if ( !pCDM )				return "";

	return pCDM->m_strName;
}

bool GLClubDeathMatchFieldMan::FrameMove ( float fElaps )
{
	if ( !GLCONST_CHAR::bCLUB_DEATHMATCH ) return false;

	for ( CDM_VEC::size_type i=0; i<m_vecClubDM.size(); ++i )
	{
		GLClubDeathMatch &sClubDM = m_vecClubDM[i];

		const bool bBattle(sClubDM.IsBattle());
		const bool bBattleReady(sClubDM.IsBattleReady());

		if ( bBattle || bBattleReady )
		{
			sClubDM.m_fMemberCheckTimer += fElaps;

			if ( sClubDM.m_fMemberCheckTimer > sClubDM.m_fMemberCheck )
			{
				sClubDM.m_fMemberCheckTimer = 0.0f;

				CheckCDMMemberLimited(&sClubDM);
			}
		}

		if( bBattle )
		{
			sClubDM.m_fRankingUpdate += fElaps;

			//	1�и��� ��ŷ ����
			if ( sClubDM.m_fRankingUpdate > 60.0f )
			{
				UpdateCDMRanking(&sClubDM);
				sClubDM.m_fRankingUpdate = 0.0f;
			}

			sClubDM.m_fLog_TIMER += fElaps;	

			if ( sClubDM.m_fLog_TIMER > fLOG_TIME ) 
			{
				//	Log���
				DoLogPrint( sClubDM.m_dwID );
				sClubDM.m_fLog_TIMER -= fLOG_TIME;
			}
		}

		if ( sClubDM.IsBattleEndAward() )
		{
			//sClubDM.CheckAwardClub( fElaps );
            CheckAwardClub(&sClubDM, fElaps);
		}

		if ( sClubDM.IsBattleEnd() )
		{
			//sClubDM.CheckExtraClub( fElaps );
            CheckExtraClub(sClubDM, fElaps);
		}
	}

	return true;
}

void GLClubDeathMatchFieldMan::CheckExtraClub(GLClubDeathMatch& sClubDM, float fElaps)
{

    sClubDM.m_fCHECK_TIMER += fElaps;
    if (sClubDM.m_fCHECK_TIMER > sClubDM.m_fCHECK_TIME)
    {
        //	30�� ������ ��������
        if (sClubDM.m_fCHECK_TIME < 30.0f)
        {
            //���� ����
            GLLandMan* pLandMan = m_pGLGaeaServer->GetLand(sClubDM.m_dwClubMap);
            if (pLandMan)
            {
                pLandMan->DoGateOutPCAll(sClubDM.m_dwCLubMapGate);
            }
            sClubDM.m_fCHECK_TIME += 10.0f;
            return;
        }
        else
        {
            sClubDM.m_fCHECK_TIMER = 0.0f;
            sClubDM.m_fCHECK_TIME = 0.0f;
            sClubDM.SetBattle(GLClubDeathMatch::BATTLE_NOSTART);
        }
    }
}

void GLClubDeathMatchFieldMan::DoLogPrint( DWORD dwID )
{
	GLClubDeathMatch *pCDM = Find ( dwID );
	if ( !pCDM )				return;
	if ( !pCDM->IsBattle() )	return;
	if ( !pCDM->m_bLog )		return;
	

	GLLandMan *pLandMan = m_pGLGaeaServer->GetLand ( pCDM->m_dwClubMap );
	if ( !pLandMan ) return;

///////
	PROFILE_BEGIN( "CDM_LOG_PRINT" );
///////

	SNATIVEID sMapID = pCDM->m_dwClubMap;


	CTime cCurTime = CTime::GetCurrentTime();
    CString strTime = cCurTime.Format("%Y-%m-%d %H:%M:%S");
	
	GLCHARNODE* pCharNode = pLandMan->m_GlobPCList.m_pHead;
	for ( ; pCharNode; pCharNode = pCharNode->pNext )
	{
		PGLCHAR pChar = pCharNode->Data;
		if ( !pChar ) continue;

		LOG_CDM sLog;
		
		CDM_KILL_LOG_MAP_ITER pos = pCDM->m_mapKillLog.find( pChar->m_CharDbNum );
		if ( pos != pCDM->m_mapKillLog.end() ) 
		{
			const SCDM_KILL_LOG& sKillLog = pos->second;
			sLog.wKill = sKillLog.wKill;
			sLog.wDeath = sKillLog.wDeath;
		}	


		CDM_RANK_INFO_MAP_ITER pos2 = pCDM->m_mapCdmScore.find( pChar->m_ClubDbNum );
		if( pos2 != pCDM->m_mapCdmScore.end() )
		{
			const SCDM_RANK_INFO& sClubLog = pos2->second;
			sLog.wClubKill = sClubLog.wKillNum;
			sLog.wClubDeath = sClubLog.wDeathNum;	
		}
        
		sLog.wMapMID = sMapID.wMainID;
		sLog.wMapSID = sMapID.wSubID;
		sLog.dwClubID = pChar->m_ClubDbNum;
		sLog.dwCharID = pChar->m_CharDbNum;
		sLog.dwClass = (DWORD)CharClassToClassIndex( pChar->m_emClass );
		sLog.wLevel = pChar->m_wLevel;
		sLog.strTime = strTime;


		// ���� ��� �����
        if (m_pGLGaeaServer)
        {
            m_pGLGaeaServer->AddLogAdoJob(
                db::DbActionPtr(
                    new db::LogCDM(sLog)));
        }

		// ���� ���� ����Ʈ ����
		CDM_CHAR_LOG_MAP_ITER pos3 = pCDM->m_mapCharLog.find( pChar->m_CharDbNum );
		if ( pos3 == pCDM->m_mapCharLog.end() ) 
		{
			SCDM_CHAR_LOG sCharLog;	
			sCharLog.dwClubID = pChar->m_ClubDbNum;
			sCharLog.dwClass = (DWORD)CharClassToClassIndex( pChar->m_emClass );
			sCharLog.wLevel = pChar->m_wLevel;
			
			pCDM->m_mapCharLog.insert( std::pair<DWORD,SCDM_CHAR_LOG>(pChar->m_CharDbNum,sCharLog) );
		}
	}

	PROFILE_END( "CDM_LOG_PRINT" );
}

void GLClubDeathMatchFieldMan::DoResultLog(DWORD dwID)
{
	GLClubDeathMatch* pCDM = Find(dwID);
	if (!pCDM)
        return;
	if (!pCDM->IsBattle())
        return;
	if (!pCDM->m_bLog)
        return;

	const GLLandMan* pLandMan = m_pGLGaeaServer->GetLand(pCDM->m_dwClubMap);
	if (!pLandMan)
        return;

////////
	PROFILE_BEGIN( "CDM_RESULT_LOG_PRINT" );
///////

	SNATIVEID sMapID = pCDM->m_dwClubMap;

	CTime cCurTime = CTime::GetCurrentTime();
    CString strTime = cCurTime.Format("%Y-%m-%d");
	
	CDM_CHAR_LOG_MAP_ITER pos_end = pCDM->m_mapCharLog.end();

    std::set<SCDM_CHAR_RANK> ChaRank;

	for (CDM_CHAR_LOG_MAP_ITER pos=pCDM->m_mapCharLog.begin(); pos != pCDM->m_mapCharLog.end(); ++pos) 
	{
		const SCDM_CHAR_LOG& sCharLog = pos->second;

		LOG_CDM_RESULT sResultLog;

		sResultLog.wMapMID = sMapID.wMainID;
		sResultLog.wMapSID = sMapID.wSubID;
		sResultLog.strTime = strTime;

		sResultLog.dwClubID = sCharLog.dwClubID;		
		sResultLog.dwCharID = pos->first;
		sResultLog.dwClass = sCharLog.dwClass;
		sResultLog.wLevel = sCharLog.wLevel;

		CDM_KILL_LOG_MAP_ITER pos2 = pCDM->m_mapKillLog.find( sResultLog.dwCharID );
		if ( pos2 != pCDM->m_mapKillLog.end() ) 
		{
			const SCDM_KILL_LOG& sKillLog = pos2->second;
			sResultLog.wKill = sKillLog.wKill;
			sResultLog.wDeath = sKillLog.wDeath;
		}
		
		CDM_RANK_INFO_MAP_ITER pos3 = pCDM->m_mapCdmScore.find( sResultLog.dwClubID );
		if( pos3 != pCDM->m_mapCdmScore.end() )
		{
			const SCDM_RANK_INFO& sCdmRankInfo = pos3->second;;

			sResultLog.wClubRank = sCdmRankInfo.wClubRanking;
			sResultLog.strClubName = sCdmRankInfo.szClubName;
			sResultLog.wClubKill = sCdmRankInfo.wKillNum;
			sResultLog.wClubDeath = sCdmRankInfo.wDeathNum;
		}

		// ���� ��� �����
        if (m_pGLGaeaServer)
        {
		    m_pGLGaeaServer->AddLogAdoJob(
                db::DbActionPtr(
                    new db::ResultLogCDM(sResultLog)));
        }

        if (m_pGLGaeaServer)
        {
            std::string ChaName = m_pGLGaeaServer->GetChaNameByDbNum(sResultLog.dwCharID);
            if (!ChaName.empty())
                ChaRank.insert(SCDM_CHAR_RANK(sResultLog.dwClubID, sResultLog.dwCharID, sResultLog.wKill, sResultLog.wDeath, ChaName));
        }
	}

    // Ŭ�� ���� ��ġ�� ����ɶ� �������� ĳ���� �� Kill ��ŷ 10�� �ȿ� ���Ե� Ŭ������ ������ 
    // �ش� Ŭ������ ������ Ŭ���α׿� ����Ѵ�.
    // 1-10 ������ �α׸� �����
    size_t RankCount = ChaRank.size();
    if (RankCount >= 10)
        RankCount = 10;        

    size_t LoopCount = 0;
    for (std::set<SCDM_CHAR_RANK>::const_iterator iter=ChaRank.begin(); iter!=ChaRank.end(); ++iter)
    {
        if (LoopCount >= RankCount)
            break;

        const SCDM_CHAR_RANK& RankData = *iter;
        if (RankData.m_ClubDbNum != 0 && m_pGLGaeaServer)
        {
            // [%1%]���� Ŭ��������ġ ��� Kill:%2% / Death:%3%
            std::string ClubLog(
                sc::string::format(
                    ID2GAMEINTEXT("CLUB_LOG_23"),
                    RankData.m_ChaName,
                    RankData.m_Kill,
                    RankData.m_Death));
            m_pGLGaeaServer->SENDCLUBLOG(RankData.m_ClubDbNum , club::LOG_CDM, ClubLog);
        }
        ++LoopCount;
    }
    
	PROFILE_END( "CDM_RESULT_LOG_PRINT" );
}

int GLClubDeathMatchFieldMan::GetAccrueCount( DWORD dwID, DWORD dwCharID )
{
	GLClubDeathMatch* pCDM = Find(dwID);
	if (pCDM)
		return pCDM->GetAccrueCount(dwCharID);
    else
	    return 1;
}

void GLClubDeathMatchFieldMan::AddGameDbJob(db::DbActionPtr spJob)
{
    m_pGLGaeaServer->AddGameDbJob(spJob);
}

void GLClubDeathMatchFieldMan::AddCDMScore(GLClubDeathMatch* pCDM, DWORD dwKillClub, DWORD dwDeathClub, DWORD dwKillChar, DWORD dwDeathChar)
{
    SCDM_RANK_INFO* pKillCdmRankInfo = pCDM->GetCDMRankInof( dwKillClub );
    if ( pKillCdmRankInfo )
    {
        pKillCdmRankInfo->wKillNum++;
    }
    else
    {

        SCDM_RANK_INFO sCdmRankInfo;		
        sCdmRankInfo.dwClubID = dwKillClub;
        sCdmRankInfo.wKillNum++;

        std::tr1::shared_ptr<GLClubField> pClub = m_pGLGaeaServer->GetClub( dwKillClub );
        if ( pClub )
        {
            StringCchCopy ( sCdmRankInfo.szClubName, CLUB_NAME+1, pClub->Name() );
            sCdmRankInfo.szClubName[CLUB_NAME] = '\0';
        }	

        pCDM->m_mapCdmScore[sCdmRankInfo.dwClubID] = sCdmRankInfo;
    }

    SCDM_RANK_INFO* pDeathCdmRankInfo = pCDM->GetCDMRankInof( dwDeathClub );
    if ( pDeathCdmRankInfo )
    {
        pDeathCdmRankInfo->wDeathNum++;
    }
    else
    {

        SCDM_RANK_INFO sCdmRankInfo;		
        sCdmRankInfo.dwClubID = dwDeathClub;
        sCdmRankInfo.wDeathNum++;

        std::tr1::shared_ptr<GLClubField> pClub = m_pGLGaeaServer->GetClub( dwDeathClub );
        if ( pClub )
        {
            StringCchCopy ( sCdmRankInfo.szClubName, CLUB_NAME+1, pClub->Name() );
            sCdmRankInfo.szClubName[CLUB_NAME] = '\0';
        }


        pCDM->m_mapCdmScore[sCdmRankInfo.dwClubID] = sCdmRankInfo;
    }



    //	Kill Log �Է�
    CDM_KILL_LOG_MAP_ITER pos = pCDM->m_mapKillLog.find( dwKillChar );

    //	�ԷµǾ� �ִٸ�.
    if ( pos != pCDM->m_mapKillLog.end() ) 
    {
        SCDM_KILL_LOG& sKillLog = pos->second;
        sKillLog.wKill++;
    }
    else
    {
        SCDM_KILL_LOG sKillLogAdd;
        sKillLogAdd.wKill++;

        pCDM->m_mapKillLog.insert( std::pair<DWORD,SCDM_KILL_LOG>(dwKillChar,sKillLogAdd) );
    }


    pos = pCDM->m_mapKillLog.find( dwDeathChar );

    //	�ԷµǾ� �ִٸ�.
    if ( pos != pCDM->m_mapKillLog.end() ) 
    {
        SCDM_KILL_LOG& sKillLog = pos->second;
        sKillLog.wDeath++;
    }
    else
    {
        SCDM_KILL_LOG sKillLogAdd;
        sKillLogAdd.wDeath++;

        pCDM->m_mapKillLog.insert( std::pair<DWORD,SCDM_KILL_LOG>(dwDeathChar,sKillLogAdd) );
    }


    //	���� ���� ī��Ʈ üũ
    CDM_ACCRUE_COUNT_MAP_ITER pos2 = pCDM->m_mapAccrueLog.find( dwDeathChar );

    if ( pos2 != pCDM->m_mapAccrueLog.end() ) 
    {
        SCDM_ACCRUE_COUNT& sLog = pos2->second;

        //	�ٸ� Ŭ���� ������ ���� ī��Ʈ�� ���½�Ų��.
        if ( sLog.dwKillClub == dwKillClub )
        {
            sLog.dwKillCount++;
        }
        else
        {
            sLog.dwKillClub = dwKillClub;
            sLog.dwKillCount = 1;
        }
    }
    else
    {
        SCDM_ACCRUE_COUNT sLog;
        sLog.dwKillClub = dwKillClub;
        sLog.dwKillCount = 1;

        pCDM->m_mapAccrueLog.insert( std::pair<DWORD,SCDM_ACCRUE_COUNT>(dwDeathChar,sLog) );
    }   
}

void GLClubDeathMatchFieldMan::UpdateCDMRanking(GLClubDeathMatch* pCDM)
{
    if ( !pCDM->IsBattle() ) return;
    if ( !pCDM->IsFieldUsed() ) return;

    CDM_RANK_INFO_VEC	vecCdmRankNew;
    vecCdmRankNew.reserve( pCDM->m_mapCdmScore.size() );

    CDM_RANK_INFO_MAP_ITER	pos = pCDM->m_mapCdmScore.begin();
    CDM_RANK_INFO_MAP_ITER	end = pCDM->m_mapCdmScore.end();

    for ( ; pos != end; pos++ )
    {
        const SCDM_RANK_INFO&	sRankInfo = pos->second;					
        vecCdmRankNew.push_back( sRankInfo );
    }

    std::sort( vecCdmRankNew.begin(), vecCdmRankNew.end() );	

    // ��ŷ ���
    int nSize = (int)vecCdmRankNew.size();

    for ( int i = 0; i < nSize; ++i )
    {
        vecCdmRankNew[i].wClubRanking = i+1;
        vecCdmRankNew[i].nIndex = i;
    }

    //	������Ȳ ��ŷ ��� �Ʒ� ��ŷ�� ����
    for ( int i = nSize-1; i > 0; --i )
    {
        if ( vecCdmRankNew[i] == vecCdmRankNew[i-1] )
        {
            vecCdmRankNew[i-1].wClubRanking = vecCdmRankNew[i].wClubRanking;			
        }
    }	


    GLMSG::SNET_CLUB_DEATHMATCH_RANKING_UPDATE	NetMsg;

    for ( size_t i = 0; i < vecCdmRankNew.size(); ++i )
    {
        CDM_RANK_INFO_MAP_ITER iter = pCDM->m_mapCdmScore.find( vecCdmRankNew[i].dwClubID );
        if ( iter == pCDM->m_mapCdmScore.end() )
            continue;	//	�̷����� ����� ��

        SCDM_RANK_INFO&	sRankInfo = iter->second;

        if ( vecCdmRankNew[i].nIndex != sRankInfo.nIndex || 
            vecCdmRankNew[i].wClubRanking != sRankInfo.wClubRanking )
        {
            sRankInfo.wClubRanking = vecCdmRankNew[i].wClubRanking;
            sRankInfo.nIndex = vecCdmRankNew[i].nIndex;

            //	9�� Ŭ���� ������.
            if ( vecCdmRankNew[i].nIndex < RANKING_NUM ) 
            {
                SCDM_RANK sCdmRank = sRankInfo;
                NetMsg.ADDCLUB( sCdmRank );
            }
        }		
    }

    if ( NetMsg.wRankNum > 0 )	
        m_pGLGaeaServer->SENDTOCLIENT_ONMAP( pCDM->m_dwClubMap, &NetMsg );


    GLMSG::SNET_CLUB_DEATHMATCH_MYRANK_UPDATE	NetMsgMy;

    pos = pCDM->m_mapCdmScore.begin();
    end = pCDM->m_mapCdmScore.end();

    for ( ; pos != end; pos++ )
    {
        const SCDM_RANK_INFO&	sRankInfo = pos->second;
        NetMsgMy.sMyCdmRank.wClubRanking = sRankInfo.wClubRanking;
        NetMsgMy.sMyCdmRank.wKillNum = sRankInfo.wKillNum;
        NetMsgMy.sMyCdmRank.wDeathNum = sRankInfo.wDeathNum;

        m_pGLGaeaServer->SENDTOCLUBCLIENT_ONMAP( pCDM->m_dwClubMap, sRankInfo.dwClubID, &NetMsgMy );
    }
}

void GLClubDeathMatchFieldMan::CheckAwardClub(GLClubDeathMatch* pCDM, float fElaps)
{
    pCDM->m_fCHECK_TIMER += fElaps;
    if (pCDM->m_fCHECK_TIMER > pCDM->m_fCHECK_TIME)
    {
        //	30�� ������ ��������
        if (pCDM->m_fCHECK_TIME < 30.0f)
        {
            //���� ����
            CDMAwardItem(pCDM);
            pCDM->m_fCHECK_TIME += 10.0f;
            return;
        }
        else if (pCDM->m_fCHECK_TIME == 300.0f)
        {
            pCDM->SetBattle(GLClubDeathMatch::BATTLE_END);
            pCDM->m_fCHECK_TIMER = 0.0f;
            pCDM->m_fCHECK_TIME = 0.0f;
            return;
        }
        else
        {
            pCDM->m_fCHECK_TIME = 300.0f;
        }
    }
}

void GLClubDeathMatchFieldMan::CDMAwardItem(GLClubDeathMatch* pCDM)
{
    if (!pCDM->IsBattleEndAward())
        return;	
    if (!pCDM->IsFieldUsed())
        return;

    GLLandMan* pLandMan = m_pGLGaeaServer->GetLand(pCDM->m_dwClubMap);
    if (!pLandMan)
        return;

    //	�ش� �ʿ� �ִ� ������ ���� ����
    {
        GLItemLimit* pItemLimit = m_pGLGaeaServer->GetItemLimit();
        GLCHARNODE* pCharNode = pLandMan->m_GlobPCList.m_pHead;
        for ( ; pCharNode; pCharNode = pCharNode->pNext )
        {
            PGLCHAR pChar = m_pGLGaeaServer->GetChar( pCharNode->Data->GetGaeaID() );
            if ( !pChar ) continue;

            std::tr1::shared_ptr<GLClubField> pClub = m_pGLGaeaServer->GetClub( pChar->m_ClubDbNum );
            if (!pClub)
                continue;

            CDM_RANK_INFO_MAP_ITER iter = pCDM->m_mapCdmScore.find( pClub->m_DbNum );
            if ( iter != pCDM->m_mapCdmScore.end() ) 
            {
                SCDM_RANK_INFO& sCdmRankInfo = iter->second;

                // ( ���� ���� ĳ�������� ���� ) 
                CDM_AWARD_CHAR_ITER iter = pCDM->m_vecCdmAwardChar.find( pChar->m_CharDbNum );
                if ( iter != pCDM->m_vecCdmAwardChar.end() )
                    continue;

                int nAwardIndex;

                if ( sCdmRankInfo.wClubRanking > pCDM->m_sCdmAwardItem.dwAwardLimit )
                    continue;
                nAwardIndex = sCdmRankInfo.wClubRanking - 1;

                SNATIVEID sNativeID = pCDM->m_sCdmAwardItem.nAwardItem[nAwardIndex];

                const SITEM* pItem = GLogicData::GetInstance().GetItem( sNativeID );
                if (!pItem)
                    continue;

                SITEMCUSTOM sITEM_NEW(sNativeID);
                sITEM_NEW.tBORNTIME = CTime::GetCurrentTime().GetTime();
                sITEM_NEW.cGenType = EMGEN_SYSTEM;
                //sITEM_NEW.cChnID = (BYTE)m_pGLGaeaServer->GetServerChannel();
                //sITEM_NEW.cFieldID = (BYTE)m_pGLGaeaServer->GetFieldSvrID();
                sc::SeqUniqueGUID::Instance()->getGUID( sITEM_NEW.guid );

				sITEM_NEW.GenerateBasicStat( false );
				sITEM_NEW.GenerateLinkSkill();
				sITEM_NEW.GenerateAddOption();

                CItemDrop cDropItem;
                cDropItem.sItemCustom = sITEM_NEW;

				// ��� �� �α� ����
				pItemLimit->LogItemExchange(
					sITEM_NEW,
					gamelog::ID_CLUB,
					pChar->m_ClubDbNum,
					gamelog::ID_CHAR,
					pChar->m_CharDbNum, 
					ITEM_ROUTE_SYSTEM,
					sITEM_NEW.wTurnNum,
					( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap(), LOG_ITEM_CDM_REWARD_TO_CHAR_INVEN,
					false );

				if ( pChar->IsInsertToInven( &cDropItem, true ) )
                {
                    pChar->InsertToInven( &cDropItem, true, true, false, true, true );
                }
                else
                {
					// ��� ó��
					{
						SINVENITEM_SAVE sItem;
						sItem.sItemCustom = cDropItem.sItemCustom;
						int DbState = sItem.sItemCustom.GetDbState();
						if ( db::DB_NONE ==  DbState || db::DB_INSERT == DbState )
						{
							m_pGLGaeaServer->AddGameAdoJob(
								db::DbActionPtr(
								new db::ItemInsert( pChar->CharDbNum(), INVEN_DELETED, sItem, 797359 ) ) );

							cDropItem.sItemCustom.SetDbState( db::DB_UPDATE, true );
						}
						else
						{
							m_pGLGaeaServer->AddGameAdoJob(
								db::DbActionPtr(
								new db::ItemUpdate( pChar->CharDbNum(), INVEN_DELETED, sItem, 797359 ) ) );
						}
					}

                    pLandMan->DropItem( 
						pChar->GetPosition(), 
						&( cDropItem.sItemCustom ),
						EMGROUP_ONE, 
						pChar->GetGaeaID() );
                }

                pCDM->m_vecCdmAwardChar.insert( pChar->m_CharDbNum );
            }
        }
    }
}

void GLClubDeathMatchFieldMan::CheckCDMMemberLimited(GLClubDeathMatch* pCDM)
{
	GLLandMan* pLandMan = m_pGLGaeaServer->GetLand(pCDM->m_dwClubMap);
	if ( pLandMan )
	{
		std::map<DWORD,DWORD> memberCountMap;
		std::map<DWORD,DWORD>::iterator memberCountIter;

		const DWORD dwFieldServerID(m_pGLGaeaServer->GetFieldSvrID());
		//����Ʈ�� �������� ��ȸ�Ѵ�. ������ m_GlobPCList�� ���� ������ �������� ����Ǿ��ֱ� ����;
		GLCHARNODE* pCharNode = pLandMan->m_GlobPCList.m_pTail;
		for ( ; pCharNode; pCharNode = pCharNode->pPrev )
		{
			PGLCHAR pChar = m_pGLGaeaServer->GetChar( pCharNode->Data->GetGaeaID() );
			if ( !pChar )
				continue;

			const DWORD dwClubID = pChar->GetClubDbNum();
			memberCountIter = memberCountMap.find(dwClubID);
			if ( memberCountIter != memberCountMap.end() )
			{
				memberCountIter->second++;
				if ( memberCountIter->second > GLCONST_CHAR::dwCLUB_DEATHMATCH_MEM )
				{
					GLMSG::SNETPC_REQ_MOVEMAP_FA NetMsgAg;
					NetMsgAg.dwChaNum = pChar->CharDbNum();
					NetMsgAg.dwFieldServerIDTo = dwFieldServerID;
					NetMsgAg.dwChannelTo = pChar->GetServerChannel();
					NetMsgAg.mapIDTo = MapID(pCDM->m_dwClubHallMap, pCDM->m_dwClubHallMap);
					NetMsgAg.dwGATEID = pCDM->m_dwClubHallGate;
					m_pGLGaeaServer->SENDTOAGENT(&NetMsgAg);

					GLMSG::SNETREQ_GATEOUT_FB NetMsgFB;
					NetMsgFB.emFB = EMCHAR_GATEOUT_CDM_MEMS_LIMITED;
					m_pGLGaeaServer->SENDTOCLIENT(pChar->ClientSlot(),&NetMsgFB);
				}
			}
			else
			{
				memberCountMap.insert(std::pair<DWORD,DWORD>(dwClubID, 1));
			}
		}
	}
}