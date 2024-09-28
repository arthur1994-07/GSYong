#include "../pch.h"
#include <boost/lexical_cast.hpp>
#include "../../EngineLib/G-Logic/GLogic.h"
#include "../../EngineLib/Common/IniLoader.h"
#include "../../EngineLib/G-Logic/GLDefine.h"

#include "../AgentServer/GLAgentServer.h"
#include "../Land/GLLandMan.h"
#include "../FieldServer/GLGaeaServer.h"

#include "../../RanLogic/GLUseFeatures.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"

#include "./GLAutoLevel.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

std::string strCommandList[CMD_SIZE] =
{
	"MSG",
	"WARNING",
	"COUNTDOWN",
	"CALL_MAP",
	"DEL_MAP",
	"CALL_HALL_MAP",
	"DEL_HALL_MAP",
	"MOVE_CHA_HALL_MAP",
	"EVENTITEM_BEGIN",
	"EVENTITEM_END",
	"EVENTMONEY_BEGIN",
	"EVENTMONEY_END",
	"EVENTEXP_BEGIN",
	"EVENTEXP_END",
	"LIMIT_EVENT_BEGIN",
	"LIMIT_EVENT_END",
	"EVENT_EX_BEGIN",
	"EVENT_EX_END",
	"EVENT_GRADE_BEGIN",
	"EVENT_GRADE_END",
};

std::string strCommandHelp[CMD_SIZE] =
{
	"Input Msg",
	"on or off",
	"Input Time(sec)",
	"Input Level File",
	"",
	"Input Level File",
	"",
	"",
	"Input Rate ( 1 ~ 10 )",
	"",
	"Input Rate ( 1 ~ 10 )",
	"",
	"Input Rate ( 1 ~ 4 )",
	"",
	"startLV EndLV PlayTime BusterTime ExpRate ItemRate",
	"",
	"EventType Rate ( 0 ~ 50 )",
	"EventType",
	"Input Rate ( 0 ~ 90 )",
	""
};

GLAutoLevel::GLAutoLevel()
	: m_bPVPFlag(false)
	, m_dwMapID(0)
	, m_dwGateID(0)
	, m_dwHallMapID(0)
	, m_bEvent(false)
	, m_LevScriptPos(0)
	, m_dwLastEventDay(0)
	, m_bUse(false)	
{
	SecureZeroMemory( m_dwDay, sizeof(m_dwDay) );
}

GLAutoLevel::~GLAutoLevel()
{
}

void GLAutoLevel::CleanUp()
{
	m_bPVPFlag = false;
	m_dwMapID = 0;
	m_dwGateID = 0;
	m_dwHallMapID = 0;
	m_bEvent = false;
	m_LevScriptPos = 0;
	m_dwLastEventDay =0;
	m_bUse = false;

	for ( int i = 0; i < MAX_EVENT_POS; ++i )
	{
		m_dwOutPos[i] = GLLEV_POS();
	}

	for ( int i = 0; i < MAX_DAY; ++i )
	{
		m_dwDay[i] = 0;
	}

	m_vecLevScript.clear();
	m_queueLevScript = QUEUE_LEV_SCRIPT();
}

GLAutoLevel& GLAutoLevel::operator = ( const GLAutoLevel& rvalue )
{
	m_strName = rvalue.m_strName;

	m_bPVPFlag = rvalue.m_bPVPFlag;
	m_dwMapID = rvalue.m_dwMapID;
	m_dwGateID = rvalue.m_dwGateID;
	m_dwHallMapID = rvalue.m_dwHallMapID;
	
	for ( int i = 0; i < MAX_EVENT_POS; ++i )
	{
		m_dwOutPos[i] = rvalue.m_dwOutPos[i];
	}

	for ( int i = 0; i < MAX_DAY; ++i )
	{
		m_dwDay[i] = rvalue.m_dwDay[i];
	}

	m_StartTime = rvalue.m_StartTime;
	m_EndTime = rvalue.m_EndTime;
	m_bEvent = rvalue.m_bEvent;
	m_dwLastEventDay = rvalue.m_dwLastEventDay;
	m_bUse = rvalue.m_bUse;

	m_vecLevScript = rvalue.m_vecLevScript;
	m_LevScriptPos = rvalue.m_LevScriptPos;
	m_queueLevScript = rvalue.m_queueLevScript;

	return *this;
}

bool GLAutoLevel::LOAD_OLD(const std::string& strFileName, STRING_LIST& listCommand, bool bToolMode)
{
	if (strFileName.empty())
        return false;

	std::string strPath;
	strPath = GLOGIC::GetServerPath ();
	strPath += strFileName;

	CIniLoader cFILE(",", bToolMode);

	if (GLOGIC::UseLogicZipFile())
		cFILE.SetZipFile(GLOGIC::strGLOGIC_SERVER_ZIPFILE);

	if (!cFILE.open(strPath, true))
	{
        sc::writeLogError(
            sc::string::format(
                "GLAutoLevel::LOAD() File Open %1%", strFileName));
		return false;
	}

	CleanUp();
	
	cFILE.getflag( "AUTO_LEVEL_SET", "EVENT_PVP", 0, 1, m_bPVPFlag );
	cFILE.getflag( "AUTO_LEVEL_SET", "EVENT_NAME", 0, 1, m_strName );

	SNATIVEID nidMAP;
	cFILE.getflag( "AUTO_LEVEL_SET", "EVENT_MAP", 0, 2, nidMAP.wMainID );
	cFILE.getflag( "AUTO_LEVEL_SET", "EVENT_MAP", 1, 2, nidMAP.wSubID );
	m_dwMapID = nidMAP.dwID;

	cFILE.getflag( "AUTO_LEVEL_SET", "EVENT_GATE", 0, 1, m_dwGateID );

	cFILE.getflag( "AUTO_LEVEL_SET", "EVENT_HALL_MAP", 0, 2, nidMAP.wMainID );
	cFILE.getflag( "AUTO_LEVEL_SET", "EVENT_HALL_MAP", 1, 2, nidMAP.wSubID );
	m_dwHallMapID = nidMAP.dwID;

	DWORD dwNUM = cFILE.GetKeySize( "AUTO_LEVEL_SET", "EVENT_POS" );
	dwNUM = ( dwNUM > MAX_EVENT_POS ) ? MAX_EVENT_POS : dwNUM;
	for( DWORD i=0; i<dwNUM; ++i )
	{
		GLLEV_POS levPos;

		cFILE.getflag( i, "AUTO_LEVEL_SET", "EVENT_POS", 0, 2, levPos.m_dwPOS_X );
		cFILE.getflag( i, "AUTO_LEVEL_SET", "EVENT_POS", 1, 2, levPos.m_dwPOS_Y );

		m_dwOutPos[i] = levPos;
	}

	cFILE.getflag( "AUTO_LEVEL_SET", "EVENT_START_TIME", 0, 2, m_StartTime.nHour );
	cFILE.getflag( "AUTO_LEVEL_SET", "EVENT_START_TIME", 1, 2, m_StartTime.nMinute );

	cFILE.getflag( "AUTO_LEVEL_SET", "EVENT_END_TIME", 0, 2, m_EndTime.nHour );
	cFILE.getflag( "AUTO_LEVEL_SET", "EVENT_END_TIME", 1, 2, m_EndTime.nMinute );

	for( DWORD i=0; i<MAX_DAY; ++i )
	{
		cFILE.getflag( "AUTO_LEVEL_SET", "EVENT_DAY", i, MAX_DAY, m_dwDay[i] );
	}

	dwNUM = cFILE.GetKeySize( "AUTO_LEVEL_SCRIPT", "ALS" );
	for( DWORD i=0; i<dwNUM; ++i )
	{
		GLLEV_SCRIPT levScript;
		GLLEV_SCRIPT_OLD levScriptOld;

		cFILE.getflag( i, "AUTO_LEVEL_SCRIPT", "ALS", 0, 4, levScriptOld.levTime.nHour );
		cFILE.getflag( i, "AUTO_LEVEL_SCRIPT", "ALS", 1, 4, levScriptOld.levTime.nMinute );
		cFILE.getflag( i, "AUTO_LEVEL_SCRIPT", "ALS", 2, 4, levScriptOld.strCommand );
		{
			bool bCommand(false);

			STRING_LIST_ITER iter = listCommand.begin();
			STRING_LIST_ITER iter_end = listCommand.end();
			for( ; iter != iter_end; ++iter )
			{
				if( *iter == levScriptOld.strCommand )
				{
					bCommand = true;
					break;
				}
			}

			if( !bCommand )
			{
				// Note : 등록되어 있는 명령어가 아닙니다.
				TCHAR szErrorMsg[255] = {0};
				StringCchPrintf( szErrorMsg, sizeof(szErrorMsg), "[GLAutoLevel::LOAD] File : %s, Command : %s",
								strFileName.c_str(), levScriptOld.strCommand.c_str() );
				MessageBox( NULL, szErrorMsg, "COMMAND ERROR", MB_OK );
				return false;
			}
		}
		cFILE.getflag( i, "AUTO_LEVEL_SCRIPT", "ALS", 3, 4, levScriptOld.strArgument );

		if ( !ConvertScriptCommnad( levScript, levScriptOld ) )
		{
			// Note : 등록되어 있는 명령어가 아닙니다.
			TCHAR szErrorMsg[255] = {0};
			StringCchPrintf( szErrorMsg, sizeof(szErrorMsg), "[GLAutoLevel::LOAD] File : %s, Command : %s",
							strFileName.c_str(), levScriptOld.strCommand.c_str() );
			MessageBox( NULL, szErrorMsg, "COMMAND Convert ERROR", MB_OK );
			return false;
		}

		m_vecLevScript.push_back( levScript );
	}

	m_bUse = true;

	return true;
}

bool GLAutoLevel::LOAD_0x0001( sc::BaseStream &SFile )
{
	SFile >> m_bUse;

	SFile >> m_bPVPFlag;
	SFile >> m_strName;

	SFile >> m_dwMapID;
	SFile >> m_dwGateID;
	SFile >> m_dwHallMapID;

	for( DWORD i=0; i<MAX_EVENT_POS; ++i )
	{
		SFile >> m_dwOutPos[i].m_dwPOS_X;
		SFile >> m_dwOutPos[i].m_dwPOS_Y;
	}

	DWORD nHour(0);
	DWORD nMinute(0);
	SFile >> nHour;
	SFile >> nMinute;
	m_StartTime.nHour = nHour;
	m_StartTime.nMinute = nMinute;

	SFile >> nHour;
	SFile >> nMinute;
	m_EndTime.nHour = nHour;
	m_EndTime.nMinute = nMinute;	

	for( DWORD i=0; i<MAX_DAY; ++i )
	{
		SFile >> m_dwDay[i];
	}

	DWORD dwScriptSize;

	SFile >> dwScriptSize;

	for ( DWORD i =0; i < dwScriptSize; ++i )
	{
		GLLEV_SCRIPT levScript;
		
		SFile >> nHour;
		SFile >> nMinute;
		levScript.levTime.nHour = nHour;
		levScript.levTime.nMinute = nMinute;

		int nCommand;
		SFile >> nCommand;		

		if ( nCommand > CMD_SIZE || nCommand < CMD_MSG )
		{
			// Note : 등록되어 있는 명령어가 아닙니다.
			TCHAR szErrorMsg[255] = {0};
			StringCchPrintf( szErrorMsg, sizeof(szErrorMsg), "[GLAutoLevel::LOAD] Script : %s, Command : %d",
				m_strName.c_str(), nCommand );
			MessageBox( NULL, szErrorMsg, "COMMAND ERROR", MB_OK );
			return false;
		}

		levScript.emCommand = (GLLEV_COMMAND)nCommand;
		SFile >> levScript.strArgument;

		m_vecLevScript.push_back( levScript );
	}

	return true;
}
bool GLAutoLevel::LOAD_0x0002( sc::BaseStream &SFile )
{
	SFile >> m_bUse;

	SFile >> m_bPVPFlag;
	SFile >> m_strName;

	SFile >> m_dwMapID;
	SFile >> m_dwGateID;
	SFile >> m_dwHallMapID;

	for( DWORD i=0; i<MAX_EVENT_POS; ++i )
	{
		SFile >> m_dwOutPos[i].m_dwPOS_X;
		SFile >> m_dwOutPos[i].m_dwPOS_Y;
	}

	SFile >> m_StartTime.nHour;
	SFile >> m_StartTime.nMinute;

	SFile >> m_EndTime.nHour;
	SFile >> m_EndTime.nMinute;

	for( DWORD i=0; i<MAX_DAY; ++i )
	{
		SFile >> m_dwDay[i];
	}

	DWORD dwScriptSize;

	SFile >> dwScriptSize;

	for ( DWORD i =0; i < dwScriptSize; ++i )
	{
		GLLEV_SCRIPT levScript;

		SFile >> levScript.levTime.nHour;
		SFile >> levScript.levTime.nMinute;

		int nCommand;
		SFile >> nCommand;		

		if ( nCommand > CMD_SIZE || nCommand < CMD_MSG )
		{
			// Note : 등록되어 있는 명령어가 아닙니다.
			TCHAR szErrorMsg[255] = {0};
			StringCchPrintf( szErrorMsg, sizeof(szErrorMsg), "[GLAutoLevel::LOAD] Script : %s, Command : %d",
				m_strName.c_str(), nCommand );
			MessageBox( NULL, szErrorMsg, "COMMAND ERROR", MB_OK );
			return false;
		}

		levScript.emCommand = (GLLEV_COMMAND)nCommand;
		SFile >> levScript.strArgument;

		m_vecLevScript.push_back( levScript );
	}

	return true;
}

bool GLAutoLevel::LOAD( sc::BaseStream &SFile )
{
	DWORD dwVersion;
	
	SFile >> dwVersion;

	CleanUp();

	switch ( dwVersion )
	{
	case 0x0001L:
		return LOAD_0x0001(SFile);
	case GLAutoLevel::VERSION:
		return LOAD_0x0002(SFile);
	}

	return false;
}


bool GLAutoLevel::SAVE( sc::SerialFile &SFile )
{
	SFile << DWORD(VERSION);

	SFile << m_bUse;

	SFile << m_bPVPFlag;
	SFile << m_strName;
	
	SFile << m_dwMapID;
	SFile << m_dwGateID;
	SFile << m_dwHallMapID;

	for( DWORD i=0; i<MAX_EVENT_POS; ++i )
	{
		SFile << m_dwOutPos[i].m_dwPOS_X;
		SFile << m_dwOutPos[i].m_dwPOS_Y;
	}

	SFile << m_StartTime.nHour;
	SFile << m_StartTime.nMinute;

	SFile << m_EndTime.nHour;
	SFile << m_EndTime.nMinute;

	for( DWORD i=0; i<MAX_DAY; ++i )
	{
		SFile << m_dwDay[i];
	}

	DWORD dwScriptSize = (DWORD)m_vecLevScript.size();

	SFile << dwScriptSize;

	for ( DWORD i =0; i < dwScriptSize; ++i )
	{
		GLLEV_SCRIPT& levScript = m_vecLevScript[i];
		
		SFile << levScript.levTime.nHour;
		SFile << levScript.levTime.nMinute;

		int nCommand = (int) levScript.emCommand;
		SFile << nCommand;			
		
		SFile << levScript.strArgument;
	}

	return true;
}

bool GLAutoLevel::IsEventStartTime( int nDayOfWeek, int nDay, int nHour, int nMinute )
{
	if( IsEvent() || ( m_dwLastEventDay == nDay ) ) 
        return false;

	if ( m_dwDay[nDayOfWeek] && m_StartTime == GLLEV_TIME(nHour, nMinute) )
		return true;
	
	return false;
}

bool GLAutoLevel::IsEventEndTime( int nHour, int nMinute )
{
	if ( m_bBeginBySchedule == true )
		return m_EndTime <= GLLEV_TIME(nHour, nMinute);


	if ( m_customEndTime.nHour < 24 )
		return m_customEndTime <= GLLEV_TIME(nHour, nMinute);

	return m_customEndTime <= GLLEV_TIME(nHour + 24, nMinute);
}

void GLAutoLevel::DoEventScript(GLAgentServer* pServer, int nHour, int nMinute)
{
	if( m_LevScriptPos >= m_vecLevScript.size() ) 
        return;

	GLLEV_TIME _processTime(GLLEV_TIME(nHour, nMinute) - m_customBeginTime);
	while ( m_LevScriptPos < m_vecLevScript.size() && m_vecLevScript[m_LevScriptPos].levTime <= _processTime )
	{
		// m_queueLevScript.push( m_vecLevScript[m_LevScriptPos] );
		RunScriptCommand( pServer, m_vecLevScript[m_LevScriptPos] );
		++m_LevScriptPos;

         if( m_LevScriptPos >= m_vecLevScript.size() ) 
             break;
	}

// 	while (!m_queueLevScript.empty())
// 	{
// 		RunScriptCommand(pServer, m_queueLevScript.front());
// 		m_queueLevScript.pop();
// 	}
}

bool GLAutoLevel::ConvertScriptCommnad( GLLEV_SCRIPT& glLevelScript, GLLEV_SCRIPT_OLD& glLevelScriptOld  )
{
	// Note : 스크립트 실행

	if( glLevelScriptOld.strCommand == _T( "msg" ) )
        glLevelScript.emCommand = CMD_MSG;
	else if( glLevelScriptOld.strCommand == _T( "warning" ) )
        glLevelScript.emCommand = CMD_WARNING;
	else if( glLevelScriptOld.strCommand == _T( "countdown" ) )
        glLevelScript.emCommand = CMD_COUNTDOWN;
	else if( glLevelScriptOld.strCommand == _T( "call_map" ) )
        glLevelScript.emCommand = CMD_CALL_MAP;
	else if( glLevelScriptOld.strCommand == _T( "del_map" ) )
        glLevelScript.emCommand = CMD_DEL_MAP;
	else if( glLevelScriptOld.strCommand == _T( "call_hall_map" ) )
        glLevelScript.emCommand = CMD_CALL_HALL_MAP;
	else if( glLevelScriptOld.strCommand == _T( "del_hall_map" ) )
        glLevelScript.emCommand = CMD_DEL_HALL_MAP;		
	else if( glLevelScriptOld.strCommand == _T( "move_cha_hall_map" ) )
        glLevelScript.emCommand = CMD_MOVE_CHA_HALL_MAP;		
	else
        return false;

	glLevelScript.strArgument = glLevelScriptOld.strArgument;
	glLevelScript.levTime = glLevelScriptOld.levTime;

	return true;
}

void GLAutoLevel::RunScriptCommand(GLAgentServer* pServer, GLLEV_SCRIPT& glLevScript)
{
	STRUTIL::ClearSeparator ();
	STRUTIL::RegisterSeparator ( "\t" );
	STRUTIL::RegisterSeparator ( " " );
	STRUTIL::RegisterSeparator ( "," );

	std::string strArgumentString(glLevScript.strArgument.c_str()), strArgumentMsg;
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		strArgumentMsg = CGameTextMan::GetInstance().GetAutoLevelSetText(m_strName.c_str(), m_LevScriptPos).GetString();
	}
	else
	{
		strArgumentMsg = glLevScript.strArgument.c_str();
	}

	// Note : 스크립트 실행
	// else if로 하면 매우 느리다;
	switch ( glLevScript.emCommand )
	{
	case CMD_MSG:
		{
			GLMSG::NET_CHAT_LINK_FB NetChatFB;
			NetChatFB.SetData(CHAT_TYPE_GLOBAL, strArgumentMsg.c_str());
			pServer->SENDTOALLCLIENT( &NetChatFB );
		}
		break;
	case CMD_WARNING:
		{
			GLMSG::SNET_LEVEL_EVENT_WARNING_FLD NetMsgBrd;
			NetMsgBrd.dwMapID = m_dwMapID;
			NetMsgBrd.bOn = ( glLevScript.strArgument == _T( "on" ) ) ? true : false;

			pServer->SENDTOALLCHANNEL( &NetMsgBrd );
		}
		break;
	case CMD_COUNTDOWN:
		{
			GLMSG::SNET_LEVEL_EVENT_COUNTDOWN_FLD NetMsgBrd;
			NetMsgBrd.dwMapID = m_dwMapID;
			NetMsgBrd.nCount = _ttoi( strArgumentString.c_str() );

			pServer->SENDTOALLCHANNEL( &NetMsgBrd );
		}
		break;
	case CMD_CALL_MAP:
		{
			GLMSG::SNET_GM_MOB_LEV_LAYER NetMsg;
			SNATIVEID nidMAP( m_dwMapID );
			NetMsg.wMAP_MID = nidMAP.wMainID;
			NetMsg.wMAP_SID = nidMAP.wSubID;
			NetMsg.unLayer = _ttoi(strArgumentString.c_str());

			pServer->SENDTOALLCHANNEL( &NetMsg );
		}
		break;
	case CMD_DEL_MAP:
		{
			GLMSG::SNET_GM_MOB_LEV_CLEAR NetMsg;
			SNATIVEID nidMAP( m_dwMapID );
			NetMsg.wMAP_MID = nidMAP.wMainID;
			NetMsg.wMAP_SID = nidMAP.wSubID;

			pServer->SENDTOALLCHANNEL( &NetMsg );
		}
		break;
	case CMD_CALL_HALL_MAP:
		{
			GLMSG::SNET_GM_MOB_LEV_LAYER NetMsg;
			SNATIVEID nidMAP( m_dwHallMapID );
			NetMsg.wMAP_MID = nidMAP.wMainID;
			NetMsg.wMAP_SID = nidMAP.wSubID;
			NetMsg.unLayer = _ttoi(strArgumentString.c_str());

			pServer->SENDTOALLCHANNEL( &NetMsg );
		}
		break;
	case CMD_DEL_HALL_MAP:
		{
			GLMSG::SNET_GM_MOB_LEV_CLEAR NetMsg;
			SNATIVEID nidMAP( m_dwHallMapID );
			NetMsg.wMAP_MID = nidMAP.wMainID;
			NetMsg.wMAP_SID = nidMAP.wSubID;

			pServer->SENDTOALLCHANNEL( &NetMsg );
		}
		break;
	case CMD_MOVE_CHA_HALL_MAP:
		{
			//	Note : 이벤트 종료를 알림.
			GLMSG::SNET_LEVEL_EVENT_END_FLD NetMsgFld;
			NetMsgFld.dwMapID = m_dwMapID;
			NetMsgFld.dwGateID = m_dwGateID;
			NetMsgFld.dwHallMapID = m_dwHallMapID;
			for( int i=0; i<MAX_EVENT_POS; ++i )
				NetMsgFld.levPos[i] = m_dwOutPos[i];

			pServer->SENDTOALLCHANNEL( &NetMsgFld );
		}
		break;
	case CMD_EVENTITEM_BEGIN:
		{
			float fRATE = (float) atof ( strArgumentString.c_str() );

			pServer->SetEventItem( true, fRATE );
		}
		break;
	case CMD_EVENTITEM_END:
		pServer->SetEventItem( false );
		break;
	case CMD_EVENTMONEY_BEGIN:
		{
			float fRATE = (float) atof ( strArgumentString.c_str() );
			pServer->SetEventMoney( true, fRATE );
		}
		break;
	case CMD_EVENTMONEY_END:
		pServer->SetEventMoney( false );
		break;
	case CMD_EVENTEXP_BEGIN:
		{
			float fRATE = (float) atof ( strArgumentString.c_str() );		
			pServer->SetEventExp( true, fRATE );
		}
		break;
	case CMD_EVENTEXP_END:
		pServer->SetEventExp( false );
		break;
	case CMD_LIMIT_EVENT_BEGIN:
		{
			CString strLINE(strArgumentString.c_str());

			CStringArray strArray ;
			STRUTIL::StringSeparate ( strLINE, strArray );

			if ( strArray.GetSize() != 6 )	return;

			CString strPARAM_01 = strArray.GetAt(0);
			CString strPARAM_02 = strArray.GetAt(1);
			CString strPARAM_03 = strArray.GetAt(2);
			CString strPARAM_04 = strArray.GetAt(3);
			CString strPARAM_05 = strArray.GetAt(4);
			CString strPARAM_06 = strArray.GetAt(5);

			SEventState sEventState;

			sEventState.MinEventLevel = (int)atoi(strPARAM_01.GetString());
			sEventState.MaxEventLevel = (int)atoi(strPARAM_02.GetString());
			sEventState.EventPlayTime = (int)atoi(strPARAM_03.GetString());
			sEventState.EventBusterTime = (int)atoi(strPARAM_04.GetString());
			sEventState.fItemGainRate = (float)atof(strPARAM_05.GetString());
			sEventState.fExpGainRate = (float)atof(strPARAM_06.GetString());		

			pServer->SetEventLimit( true, sEventState );	
		}
		break;
	case CMD_LIMIT_EVENT_END:
		pServer->SetEventLimit( false, SEventState() );
		break;
	case CMD_EVENT_EX_BEGIN:
		{
			CString strLINE(strArgumentString.c_str());
			CStringArray strArray ;
			STRUTIL::StringSeparate ( strLINE, strArray );
			if ( strArray.GetSize() != 2 )
				return;

			CString strPARAM_01 = strArray.GetAt(0);
			CString strPARAM_02 = strArray.GetAt(1);

			EMGM_EVENT_TYPE emType(EMGM_EVENT_NONE);
			WORD wValue = (WORD)atoi( strPARAM_02.GetString() );

			if ( strPARAM_01=="speed" )
				emType = EMGM_EVENT_SPEED;
			else if ( strPARAM_01=="aspeed" )
				emType = EMGM_EVENT_ASPEED;
			else if ( strPARAM_01=="attack" )
				emType = EMGM_EVENT_ATTACK;
			else
				return;

			pServer->SetEventEx( true, emType, wValue );
		}
		break;
	case CMD_EVENT_EX_END:
		{
			CString strPARAM_01 = strArgumentString.c_str();
			EMGM_EVENT_TYPE emType(EMGM_EVENT_NONE);

			if ( strPARAM_01=="speed" )
				emType = EMGM_EVENT_SPEED;
			else if ( strPARAM_01=="aspeed" )
				emType = EMGM_EVENT_ASPEED;
			else if ( strPARAM_01=="attack" )
				emType = EMGM_EVENT_ATTACK;
			else
				return;

			pServer->SetEventEx( false, emType );
		}
		break;
	case CMD_EVENT_GRADE_BEGIN:
		{
			float fRATE = (float) atof ( strArgumentString.c_str() );
			pServer->SetEventGrade( true, fRATE );
		}
		break;
	case CMD_EVENT_GRADE_END:
		pServer->SetEventGrade( false );
		break;
	}	
}

std::string	GLAutoLevel::GetScriptTextCommnad( GLLEV_SCRIPT& glLevScript )
{
	if ( glLevScript.emCommand == CMD_SIZE )	return "NULL";
	
	int nCmd = (int)glLevScript.emCommand;

	return strCommandList[nCmd].c_str();	
}

bool GLAutoLevel::CheckScript ( GLLEV_SCRIPT& glLevScript )
{
	STRUTIL::ClearSeparator ();
	STRUTIL::RegisterSeparator ( "\t" );
	STRUTIL::RegisterSeparator ( " " );
	STRUTIL::RegisterSeparator ( "," );

	if (glLevScript.emCommand == CMD_SIZE)
        return false;

	std::string strArgumentText;
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		strArgumentText = CGameTextMan::GetInstance().GetAutoLevelSetText(m_strName.c_str(), m_LevScriptPos);
	else
		strArgumentText = glLevScript.strArgument;

	const short nHour(glLevScript.levTime.nHour + m_StartTime.nHour);
	const short nMinute(glLevScript.levTime.nMinute + m_StartTime.nMinute);

	const short nOverHour(nMinute / 60);

	const short nAbsoluteHour(nHour + nOverHour);
	const short nAbsoluteMinute(nMinute % 60);	

	if ( (nAbsoluteHour > 23) || (nAbsoluteHour > m_EndTime.nHour) )
        return false;
	
	switch ( glLevScript.emCommand )
	{	
	case CMD_WARNING:
		{
			if (strArgumentText != _T("on") && strArgumentText != _T("off"))
				return false;
			if (strCommandHelp[glLevScript.emCommand] == strArgumentText)
				return false;
		}
		break;
	case CMD_COUNTDOWN:
		{	
			int nCount = _ttoi( strArgumentText.c_str() );
			if ( nCount < 0 || nCount > 1000 )
				return false;
			if ( strCommandHelp[glLevScript.emCommand] == strArgumentText )
				return false;
		}
		break;
	case CMD_DEL_MAP:
	case CMD_DEL_HALL_MAP:
	case CMD_MOVE_CHA_HALL_MAP:
	case CMD_EVENTITEM_END:
	case CMD_EVENTMONEY_END:
	case CMD_EVENTEXP_END:
	case CMD_LIMIT_EVENT_END:
	case CMD_EVENT_GRADE_END:
		strArgumentText = "";
		break;
	case CMD_MSG:
	case CMD_CALL_MAP:
	case CMD_CALL_HALL_MAP:
	case CMD_EVENTITEM_BEGIN:
	case CMD_EVENTMONEY_BEGIN:
	case CMD_EVENTEXP_BEGIN:
	case CMD_EVENT_GRADE_BEGIN:
		{
			if ( strArgumentText.empty() )
				return false;
			if ( strCommandHelp[glLevScript.emCommand] == strArgumentText )
				return false;
		}		
		break;
	case CMD_LIMIT_EVENT_BEGIN:
		{
			if ( strArgumentText.empty() )
				return false;
			if ( strCommandHelp[glLevScript.emCommand] == strArgumentText )
				return false;		

			CString strLINE(strArgumentText.c_str());

			CStringArray strArray ;
			STRUTIL::StringSeparate ( strLINE, strArray );

			//	Event 종료시간 설정 못함
			if ( strArray.GetSize() != 6 )
				return false;
		}
		break;
	case CMD_EVENT_EX_BEGIN:
		{
			if ( strArgumentText.empty() )
				return false;
			if ( strCommandHelp[glLevScript.emCommand] == strArgumentText )
				return false;		

			CString strLINE(strArgumentText.c_str());

			CStringArray strArray ;
			STRUTIL::StringSeparate ( strLINE, strArray );

			if ( strArray.GetSize() != 2 )
				return false;

			CString strPARAM_01 = strArray.GetAt(0);
			if ( (strPARAM_01!="speed") && ( strPARAM_01!="aspeed" ) && ( strPARAM_01!="attack" ) )
				return false;
		}
		break;
	case CMD_EVENT_EX_END:
		{
			if ( strArgumentText.empty() ) 
				return false;
			if ( strCommandHelp[glLevScript.emCommand] == strArgumentText )
				return false;		

			CString strPARAM_01 = strArgumentText.c_str();
			if ( (strPARAM_01!="speed") && ( strPARAM_01!="aspeed" ) && ( strPARAM_01!="attack" ) )
				return false;
		}
		break;	
	}

	return true;
}

bool GLAutoLevel::AddScript( GLLEV_SCRIPT & glLevScript )
{
	m_vecLevScript.push_back( glLevScript );	

	return true;
}

bool GLAutoLevel::DelScript( int nIndex )
{
	if ( nIndex < 0 || nIndex >= (int)m_vecLevScript.size() )
        return false;

	m_vecLevScript.erase( m_vecLevScript.begin()+ nIndex );

	return true;
}

int	GLAutoLevel::CheckCommandTime()
{
	if ( m_vecLevScript.empty() )
        return -1;

	//	스크립트별 시간 정렬 체크
	for ( int i = 0; i < (int)m_vecLevScript.size()-1; ++i )
	{
		if ( m_vecLevScript[i].levTime.nHour > m_vecLevScript[i+1].levTime.nHour )
            return i+1;
		
		if ( ( m_vecLevScript[i].levTime.nHour == m_vecLevScript[i+1].levTime.nHour ) &&
			 ( m_vecLevScript[i].levTime.nMinute > m_vecLevScript[i+1].levTime.nMinute ) )
             return i+1;
	}	

	{
		const GLLEV_SCRIPT& glFirstLevScript = m_vecLevScript[0];
		const short nHour(glFirstLevScript.levTime.nHour + m_StartTime.nHour);
		const short nMinute(glFirstLevScript.levTime.nMinute + m_StartTime.nMinute);

		const short nOverHour(nMinute / 60);

		const short nAbsoluteHour(nHour + nOverHour);
		const short nAbsoluteMinute(nMinute % 60);	

		if ( (nAbsoluteHour > 23) || (nAbsoluteHour > m_EndTime.nHour) )
			return 1;
	}

	{
		const DWORD nIndex(m_vecLevScript.size() - 1);
		const GLLEV_SCRIPT& glFirstLevScript = m_vecLevScript[nIndex];
		const short nHour(glFirstLevScript.levTime.nHour + m_StartTime.nHour);
		const short nMinute(glFirstLevScript.levTime.nMinute + m_StartTime.nMinute);

		const short nOverHour(nMinute / 60);

		const short nAbsoluteHour(nHour + nOverHour);
		const short nAbsoluteMinute(nMinute % 60);	

		if ( (nAbsoluteHour > 23) || (nAbsoluteHour > m_EndTime.nHour) )
			return nIndex + 1;
	}	

	return -1;
}

bool GLAutoLevel::CSV_LOAD( sc::CStringFile& StrFile )
{
	STRUTIL::ClearSeparator();
	STRUTIL::RegisterSeparator( "," );
	
	CString strLine;
	CStringArray StrArray;

	//	Basic Head
	StrFile.GetNextLine( strLine );
	STRUTIL::StringSeparate( strLine, StrArray );

	int iHead = (int)StrArray.GetCount();

	char szError[ 256 ];

	StrFile.GetNextLine( strLine );

	STRUTIL::StringSeparate( strLine, StrArray );

	int iCount = (int)StrArray.GetCount();
	if( iCount < iHead )
	{
		StringCchPrintf( szError, 256, "GLAutoLevel::CSV_LOAD include blank column, next item load failed" );
		MessageBox( NULL, _T(szError), _T("Fail"), MB_OK );
		return false;
	}

	CleanUp();

	LoadCsv( StrArray );
	int nScriptNum = atoi( StrArray[StrArray.GetCount()-1] );

	//	Script Head
	StrFile.GetNextLine( strLine );
	STRUTIL::StringSeparate( strLine, StrArray );
	iHead = (int)StrArray.GetCount();

	for ( int i = 0; i < nScriptNum; ++i )
	{
		if ( !StrFile.GetNextLine( strLine ) ) 
            return false;
		
		STRUTIL::StringSeparate( strLine, StrArray );

		iCount = (int)StrArray.GetCount();
		if( iCount < iHead )
		{
			StringCchPrintf( szError, 256, "%d Script Num = include blank column, next item load failed", i );
			MessageBox( NULL, _T(szError), _T("Fail"), MB_OK );
			return false;
		}
		
		GLLEV_SCRIPT glLevScript;
		LoadCsvCommand( StrArray, glLevScript );

		m_vecLevScript.push_back( glLevScript );
	}
	
	return true;
}

bool GLAutoLevel::CSV_SAVE( std::fstream &SFile )
{
	SaveCsvHead( SFile );
	SaveCsv( SFile );
	SaveCsvCommandHead( SFile );
	SaveCsvCommand( SFile );
	return true;
}

VOID GLAutoLevel::SaveCsvHead ( std::fstream &SFile )
{
	SFile << "Event Name" << ",";
	SFile << "Use" << ",";
	SFile << "PVPFlag" << ",";
	
	SFile << "MAP MID" << ",";
	SFile << "MAP SID" << ",";
	SFile << "MAP GATE ID" << ",";

	SFile << "HALLMAP MID" << ",";
	SFile << "HALLMAP SID" << ",";

	SFile << "EVENT DAY" << ",";
	SFile << "Start Time Hour" << ",";
	SFile << "Start Time Min" << ",";

	SFile << "End Time Hour" << ",";
	SFile << "End Time Min" << ",";

	SFile << "OutPos1 X" << ",";
	SFile << "OutPos1 Y" << ",";

	SFile << "OutPos2 X" << ",";
	SFile << "OutPos2 Y" << ",";

	SFile << "OutPos3 X" << ",";
	SFile << "OutPos3 Y" << ",";

	SFile << "OutPos4 X" << ",";
	SFile << "OutPos4 Y" << ",";

	SFile << "OutPos5 X" << ",";
	SFile << "OutPos5 Y" << ",";

	SFile << "Script Num" << ",";

	SFile << std::endl;
}

VOID GLAutoLevel::SaveCsv ( std::fstream &SFile )
{
	STRUTIL::OutputStrCsv( SFile, m_strName );
	
	SFile << m_bUse << ",";
	SFile << m_bPVPFlag << ",";

	SNATIVEID sNid = m_dwMapID;
	
	SFile << sNid.wMainID << ",";
	SFile << sNid.wSubID << ",";
	SFile << m_dwGateID << ",";

	sNid = m_dwHallMapID;

	SFile << sNid.wMainID << ",";
	SFile << sNid.wSubID << ",";

	CString strDay, strTemp;
	for ( int i = 0; i < MAX_DAY; ++i )
	{		
		if ( m_dwDay[i] == 1 ) 
		{
            std::string Buffer = boost::lexical_cast<std::string> (i+1);
			strDay += Buffer.c_str();
		}
	}


	SFile << strDay << ",";
	
	SFile << m_StartTime.nHour << ",";
	SFile << m_StartTime.nMinute << ",";


	SFile << m_EndTime.nHour << ",";
	SFile << m_EndTime.nMinute << ",";

	for ( int i = 0; i < MAX_EVENT_POS; ++i )
	{
		SFile << m_dwOutPos[i].m_dwPOS_X << ",";
		SFile << m_dwOutPos[i].m_dwPOS_Y << ",";
	}

	SFile << m_vecLevScript.size() << ",";

	SFile << std::endl;
}

VOID GLAutoLevel::SaveCsvCommandHead ( std::fstream &SFile )
{
	SFile << "Time Hour" << ",";
	SFile << "Time Min" << ",";

	SFile << "Command" << ",";
	SFile << "Msg" << ",";

	SFile << std::endl;
}

VOID GLAutoLevel::SaveCsvCommand ( std::fstream &SFile )
{
	for ( size_t i = 0; i < m_vecLevScript.size(); ++i )
	{
		SFile << m_vecLevScript[i].levTime.nHour << ",";
		SFile << m_vecLevScript[i].levTime.nMinute << ",";
		SFile << strCommandList[m_vecLevScript[i].emCommand] << ",";

		STRUTIL::OutputStrCsv( SFile, m_vecLevScript[i].strArgument );
		SFile << std::endl;
	}
}

VOID GLAutoLevel::LoadCsv ( CStringArray &StrArray )
{
	int nIndex = 0;

	STRUTIL::InputStrCsv( StrArray[ nIndex++ ], m_strName );
	m_bUse = (0!=atol( StrArray[nIndex++] ));
	m_bPVPFlag = (0!=atol( StrArray[nIndex++] ));

	SNATIVEID sNid;

	sNid.wMainID = (WORD)atol( StrArray[nIndex++] );
	sNid.wSubID = (WORD)atol( StrArray[nIndex++] );

	m_dwMapID = sNid.dwID;

	m_dwGateID = (DWORD)atol( StrArray[nIndex++] );

	sNid.wMainID = (WORD)atol( StrArray[nIndex++] );
	sNid.wSubID = (WORD)atol( StrArray[nIndex++] );
	m_dwHallMapID = sNid.dwID;

	CString strDay = StrArray[nIndex++];

	for ( int i = 0; i < strDay.GetLength(); ++i )
	{
		int nChar = strDay.GetAt(i) - '1';
		m_dwDay[nChar] = 1;
	}
	
	m_StartTime.nHour = (DWORD)atol( StrArray[nIndex++] );
	m_StartTime.nMinute = (DWORD)atol( StrArray[nIndex++] );

	m_EndTime.nHour = (DWORD)atol( StrArray[nIndex++] );
	m_EndTime.nMinute = (DWORD)atol( StrArray[nIndex++] );	

	for ( int i = 0; i < MAX_EVENT_POS; ++i )
	{
		m_dwOutPos[i].m_dwPOS_X = (DWORD)atol( StrArray[nIndex++] );
		m_dwOutPos[i].m_dwPOS_Y = (DWORD)atol( StrArray[nIndex++] );
	}
}

VOID GLAutoLevel::LoadCsvCommand ( CStringArray& StrArray, GLLEV_SCRIPT& sLevScript )
{	
	int nIndex = 0;

	sLevScript.levTime.nHour = (DWORD)atol( StrArray[nIndex++] );
	sLevScript.levTime.nMinute = (DWORD)atol( StrArray[nIndex++] );

	std::string strCommand = StrArray[nIndex++];
	for ( int i = 0; i < CMD_SIZE; ++i )
	{
		if ( strCommand == strCommandList[i] ) 
		{
			sLevScript.emCommand = (GLLEV_COMMAND)i;
			break;
		}
	}

	STRUTIL::InputStrCsv( StrArray[ nIndex++ ], sLevScript.strArgument );
}