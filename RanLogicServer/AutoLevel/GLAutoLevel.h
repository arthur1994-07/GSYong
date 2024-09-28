#ifndef _GL_AUTO_LEVEL_H_
#define _GL_AUTO_LEVEL_H_

#pragma once

#include <queue>
#include "../../SigmaCore/File/StringFile.h"
#include "../../SigmaCore/File/SerialFile.h"
#include "../../EngineLib/Common/ByteStream.h"
#include "../../RanLogic/AutoLevel/GLAutoLevelDefine.h"

class GLAgentServer;

class GLAutoLevel
{
public:
	enum { MAX_DAY = 7 };
	enum { VERSION = 0x0002 };

public:
	std::string	m_strName;				// �̺�Ʈ ��Ī
	
	bool		m_bUse;
	bool		m_bPVPFlag;				// 0:NonPVP, 1:PVP=������	
	DWORD		m_dwMapID;
	DWORD		m_dwGateID;
	DWORD		m_dwHallMapID;
	GLLEV_POS	m_dwOutPos[MAX_EVENT_POS];	// ĳ���Ͱ� �̺�Ʈ Ȧ�� ���ư� ��ǥ
	DWORD		m_dwDay[MAX_DAY];		// [��,ȭ,��,��,��,��,��]
	GLLEV_TIME	m_StartTime;
	GLLEV_TIME	m_EndTime;
	bool		m_bEvent;
	bool		m_bBeginBySchedule;
	DWORD		m_dwLastEventDay;
	GLLEV_TIME m_customBeginTime;
	GLLEV_TIME m_customEndTime;
	

	VEC_LEV_SCRIPT				m_vecLevScript;		// ��ũ��Ʈ ����Ʈ
	VEC_LEV_SCRIPT::size_type	m_LevScriptPos;

	QUEUE_LEV_SCRIPT			m_queueLevScript;

public:
	GLAutoLevel ();
	~GLAutoLevel ();

	void CleanUp();

	GLAutoLevel& operator = ( const GLAutoLevel& rvalue );
	

public:
	bool IsUse()				{ return m_bUse; }
	void SetUse( bool bUse )	{ m_bUse = bUse; }
	bool IsEvent()									{ return m_bEvent; }
	bool IsEventStartTime( int nDayOfWeek, int nDay, int nHour, int nMinute );
	bool IsEventEndTime( int nHour, int nMinute );

public:
	void DoEventProcess(const unsigned int nMinute)
	{
		m_customBeginTime -= nMinute;
		m_customEndTime -= nMinute;
	}
	void DoEventStart(const unsigned int nDay, const unsigned int nHour, const unsigned int nMinute, const bool bBeginBySchedule = true)								
	{ 
		m_LevScriptPos = 0;
		m_dwLastEventDay = nDay;
		m_customBeginTime = GLLEV_TIME(nHour, nMinute);		
		m_customEndTime = m_customBeginTime + (m_EndTime - m_StartTime);
		m_bBeginBySchedule = bBeginBySchedule;
		m_bEvent = true; 		
	}
	void DoEventEnd()								{ m_bEvent = false; }

public:
	void DoEventScript(GLAgentServer* pServer, int nHour, int nMinute);
	void RunScriptCommand(GLAgentServer* pServer, GLLEV_SCRIPT& glLevScript);
	
	bool ConvertScriptCommnad( GLLEV_SCRIPT& glLevelScript, GLLEV_SCRIPT_OLD& glLevelScriptOld  );
	std::string GetScriptTextCommnad( GLLEV_SCRIPT& glLevScript );

	bool AddScript( GLLEV_SCRIPT & glLevScript );
	bool DelScript( int nIndex );
	bool CheckScript ( GLLEV_SCRIPT& glLevScript );

	int	CheckCommandTime();

public:
	bool LOAD_OLD(const std::string& strFileName, STRING_LIST& listCommand, bool bToolMode);
	
	bool SAVE( sc::SerialFile &SFile );
	bool LOAD( sc::BaseStream &SFile );
	bool LOAD_0x0001( sc::BaseStream &SFile );
	bool LOAD_0x0002( sc::BaseStream &SFile );

	bool CSV_LOAD( sc::CStringFile& StrFile );
	bool CSV_SAVE( std::fstream &SFile );
	
	bool CSV_SAVE_TIME( std::fstream &SFile );

	VOID SaveCsvHead ( std::fstream &SFile );	
	VOID SaveCsvCommandHead ( std::fstream &SFile );

	VOID SaveCsv ( std::fstream &SFile );
	VOID SaveCsvCommand ( std::fstream &SFile );	
	
	VOID LoadCsv ( CStringArray &StrArray );
	VOID LoadCsvCommand ( CStringArray &StrArray, GLLEV_SCRIPT& sLevScript );
};

#endif // _GL_AUTO_LEVEL_H_