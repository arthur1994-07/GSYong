#ifndef _GL_AUTO_LEVEL_MAN_H_
#define _GL_AUTO_LEVEL_MAN_H_

#pragma once

#include "./GLAutoLevel.h"

typedef std::list<GLAutoLevel*>		AUTOLEVEL_LIST;
typedef AUTOLEVEL_LIST::iterator	AUTOLEVEL_LIST_ITER;

typedef std::vector<GLAutoLevel> GLAutoLevelVector;
typedef GLAutoLevelVector::const_iterator GLAutoLevelVectorCIter;
typedef GLAutoLevelVector::iterator GLAutoLevelVectorIter;

struct AutoLevelReserve
{
	AutoLevelReserve(const GLLEV_TIME& _reserveTime, const unsigned int _nIndex, const int _nRemainMinute)
		: reserveTime(_reserveTime), nIndex(_nIndex), nRemainMinute(_nRemainMinute)
	{
	}
	AutoLevelReserve(const AutoLevelReserve& _rhs)
		: reserveTime(_rhs.reserveTime), nIndex(_rhs.nIndex), nRemainMinute(_rhs.nRemainMinute)
	{
	}

	const bool isEventStartTime(const int _nHour, const int _nMinute)
	{
		const GLLEV_TIME _processTime(GLLEV_TIME(_nHour, _nMinute) - reserveTime);
		const int _processMinute(_processTime.nHour * 60 + _processTime.nMinute);
		nRemainMinute -= _processMinute;
		
		return nRemainMinute > 0 ? false : true;
	}

	const GLLEV_TIME reserveTime;
	const unsigned int nIndex;
	int nRemainMinute;
};

typedef std::list<AutoLevelReserve> AutoLevelReserveList;
typedef AutoLevelReserveList::const_iterator AutoLevelReserveListCIter;
typedef AutoLevelReserveList::iterator AutoLevelReserveListIter;

extern std::string m_strCommand_Old[CMD_OLD_SIZE];

struct NET_MSG_GENERIC;
class GLAgentServer;
class GLAutoLevelMan
{
public:
	enum
	{
		VERSION	= 0x0100,		
	};

	static const char* _FILEHEAD;

public:
    GLAutoLevelMan(GLAgentServer* pServer);
    ~GLAutoLevelMan();

public:
	AUTOLEVEL_LIST	m_listAutoLevel;
	GLAutoLevelVector m_autoLevelVector;
	AutoLevelReserveList m_autoLevelReserveList;

protected:
    GLAgentServer* m_pServer;
	STRING_LIST		m_listFileName;	
	STRING_LIST		m_listCommand_Old;
	
	std::string				m_strPATH;
	char					m_szFileName[MAX_PATH];

public:
	void SetPath ( std::string strPATH )	{ m_strPATH = strPATH; }
	std::string GetPath ()					{ return m_strPATH.c_str(); }

	const DWORD GetNAutoLevel(void) const { return m_autoLevelVector.size(); }
	GLAgentServer* GetServer(void) { }
	GLAutoLevel* GetAutoLevel( int nIndex );
	bool DelAutoLevel( int nIndex );
	STRING_LIST& GetCommandList()	{ return m_listCommand_Old; }

	bool LOAD_OLD(std::string& strFileName, bool bToolMode);
	HRESULT LOAD( std::string & strFileName );
	HRESULT SAVE( std::string & strFileName );
	
	HRESULT CSV_SAVE( std::string & strFileName );
	HRESULT	CSV_LOAD( std::string & strFileName );

	void CsvHead( std::fstream &SFile );	
	void CsvSave( std::fstream &SFile );	
	void CsvLoad( CStringArray &StrArray );

	bool FrameMove();
	void CleanUp();

public:
	const bool messageProcedure(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID);

private:
	const bool _messageProcedureList(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID, const unsigned int _nClientSlot);	
	const bool _messageProcedureToday(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID, const unsigned int _nClientSlot);
	const bool _messageProcedureBegin(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID, const unsigned int _nClientSlot);
	const bool _messageProcedureEnd(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID, const unsigned int _nClientSlot);
	const bool _messageProcedureProcess(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID, const unsigned int _nClientSlot);
	const bool _messageProcedureReserve(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID, const unsigned int _nClientSlot);
	const bool _messageProcedureOn(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID, const unsigned int _nClientSlot);
	const bool _messageProcedureOff(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID, const unsigned int _nClientSlot);	
	const bool _messageProcedureInformation(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID, const unsigned int _nClientSlot);
};

#endif // _GL_AUTO_LEVEL_MAN_H_
