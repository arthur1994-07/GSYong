#include "../pch.h"
#include "../../SigmaCore/String/StringUtils.h"
#include "../../SigmaCore/Log/LogMan.h"

#include "../../RanLogic/RANPARAM.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"

#include "../../EngineLib/Common/IniLoader.h"
#include "../../EngineLib/G-Logic/GLOGIC.h"

#include "./GLAutoLevelMan.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const char* GLAutoLevelMan::_FILEHEAD = "GLAUTO_LEVLE_MAN";

std::string m_strCommand_Old[CMD_OLD_SIZE] = 
{
	"msg",
	"warning",
	"countdown",
	"call_map",
	"del_map",
	"call_hall_map",
	"del_hall_map",
	"move_cha_hall_map"
};

GLAutoLevelMan::GLAutoLevelMan(GLAgentServer* pServer)
    : m_pServer(pServer)
{
	memset(m_szFileName, 0, sizeof(char) * (MAX_PATH));

	for ( int i = 0; i < CMD_OLD_SIZE; ++i )
	{
		m_listCommand_Old.push_back( m_strCommand_Old[i] );
	}
}

GLAutoLevelMan::~GLAutoLevelMan()
{
	CleanUp();
}

void GLAutoLevelMan::CleanUp()
{
	m_autoLevelVector.clear();
	m_listFileName.clear();
}

HRESULT GLAutoLevelMan::LOAD( std::string & strFileName )
{
	CleanUp();
	
	StringCchCopy(m_szFileName,MAX_PATH,strFileName.c_str());

	char szFullPath[MAX_PATH] = {0};
	StringCchCopy ( szFullPath, MAX_PATH, GLOGIC::GetServerPath() );
	StringCchCat ( szFullPath, MAX_PATH, m_szFileName );

	std::auto_ptr<sc::BaseStream> pBStream(
        GLOGIC::openfile_basestream(
            GLOGIC::UseLogicZipFile(), 
            GLOGIC::strGLOGIC_SERVER_ZIPFILE,
            szFullPath, 
            strFileName,
            false,
            GLOGIC::UseLogicPackFile()));

	
	if ( !pBStream.get() )
		return E_FAIL;

	sc::BaseStream &SFile = *pBStream;
	
	
	DWORD dwFILEVER;
	char szFILETYPE[_MAX_FNAME];
	SFile.GetFileType( szFILETYPE, _MAX_FNAME, dwFILEVER );
	SFile.SetEncode ( true );

	DWORD dwNum=0;

	SFile >> dwNum;

	// 자동 레벨 파일을 읽는다.
	for (DWORD i=0; i<dwNum; ++i)
	{
		GLAutoLevel glAutoLevel;
		if ( glAutoLevel.LOAD(SFile) == false )
		{
            sc::writeLogError(
                std::string(
                    "GLAutoLevelMan::LOAD(), File Open"));
			return E_FAIL;
		}

		m_autoLevelVector.push_back(glAutoLevel);
	}

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() == TRUE )
	{
		CGameTextMan::GetInstance().LoadText(RANPARAM::strAutoLevelSetText.GetString(), CGameTextMan::EM_AUTOLEVELSET_TEXT, RANPARAM::strCountry, CGameTextMan::GAMETEXTMAN_LOADOP_REFACT|CGameTextMan::GAMETEXTMAN_LOADOP_SERVEREXE|CGameTextMan::GAMETEXTMAN_LOADOP_CONVUTF8);
	}

	return S_OK;
}

HRESULT GLAutoLevelMan::SAVE( std::string & strFileName )
{
	StringCchCopy(m_szFileName,MAX_PATH,strFileName.c_str());

	char szFullPath[MAX_PATH];
	StringCchCopy ( szFullPath, MAX_PATH, GLOGIC::GetServerPath() );
	StringCchCat ( szFullPath, MAX_PATH, m_szFileName );

	sc::SerialFile SFile;
	SFile.SetFileType ( _FILEHEAD, VERSION );
	if ( !SFile.OpenFile ( FOT_WRITE, szFullPath ) )	return E_FAIL;
	SFile.SetEncode ( true );

	DWORD dwNum = 0;
	dwNum = (DWORD)m_autoLevelVector.size();

	SFile << dwNum;

	for ( GLAutoLevelVectorIter _iteratorLevel(m_autoLevelVector.begin()); _iteratorLevel != m_autoLevelVector.end(); ++_iteratorLevel )
	{
		GLAutoLevel& glAutoLevel = *_iteratorLevel;
		glAutoLevel.SAVE( SFile );
	}

	return S_OK;
}

HRESULT GLAutoLevelMan::CSV_SAVE( std::string & strFileName )
{
	std::fstream streamFILE;
	streamFILE.open ( strFileName.c_str(), std::ios_base::out );

	CsvHead(streamFILE);
	CsvSave(streamFILE);

	for ( GLAutoLevelVectorIter _iteratorLevel(m_autoLevelVector.begin()); _iteratorLevel != m_autoLevelVector.end(); ++_iteratorLevel )
	{
		GLAutoLevel& glAutoLevel = *_iteratorLevel;
		
		if ( glAutoLevel.CSV_SAVE(streamFILE) == false )
			return E_FAIL;		
	}

	streamFILE.close();
	
	return S_OK;
}

HRESULT GLAutoLevelMan::CSV_LOAD( std::string & strFileName )
{
	sc::CStringFile StrFile( 10240 );
	if( !StrFile.Open ( strFileName.c_str() ) )
		return S_FALSE;

	STRUTIL::ClearSeparator();
	STRUTIL::RegisterSeparator( "," );

	CString strLine;
	CStringArray StrArray;

	// Csv Head
	StrFile.GetNextLine( strLine );
	STRUTIL::StringSeparate( strLine, StrArray );
	int iHead = (int)StrArray.GetCount();	

	char szError[ 256 ];
	
	StrFile.GetNextLine( strLine );

	STRUTIL::StringSeparate( strLine, StrArray );

	int iCount = (int)StrArray.GetCount();
	if( iCount < iHead )
	{
		StringCchPrintf( szError, 256, " GLAutoLevelMan::CSV_LOAD include blank column, next item load failed" );
		MessageBox( NULL, _T(szError), _T("Fail"), MB_OK );
		return S_FALSE;
	}

	CleanUp();

	int nNum = atoi( StrArray[0] );

	for ( int i = 0; i < nNum; ++i )
	{
		GLAutoLevel glAutoLevel;
		if( glAutoLevel.CSV_LOAD( StrFile ) == false )
		{
			MessageBox( NULL, "GLAutoLevel::LOAD_CSV Error", "", MB_OK );
			return E_FAIL;
		}

		m_autoLevelVector.push_back( glAutoLevel );
		
	}

	return S_OK;
}


void GLAutoLevelMan::CsvHead( std::fstream &SFile )
{
	SFile << "Event Num " << ",";
	SFile << std::endl; 
}

void GLAutoLevelMan::CsvSave( std::fstream &SFile )
{
	SFile << m_autoLevelVector.size(); 
	SFile << std::endl;
}

void GLAutoLevelMan::CsvLoad( CStringArray &StrArray )
{
}


bool GLAutoLevelMan::LOAD_OLD(std::string& strFileName, bool bToolMode)
{
	m_listCommand_Old.clear();
	CleanUp();
	
	if( strFileName.empty() )	return false;

	std::string strPath(GLOGIC::GetServerPath());
	strPath += strFileName;

	CIniLoader cFILE(",", bToolMode);

	if (GLOGIC::UseLogicZipFile())
		cFILE.SetZipFile(GLOGIC::strGLOGIC_SERVER_ZIPFILE);

	if( !cFILE.open ( strPath, true ) )
	{
        sc::writeLogError(
            sc::string::format(
                "GLAutoLevelMan::LOAD File Open %1%", strFileName));
		return false;
	}

	DWORD dwNUM = cFILE.GetKeyDataSize( "AUTO_LEVEL_MAN_SET", "COMMAND_LIST" );
	for( DWORD i=0; i<dwNUM; ++i )
	{
		std::string strCommand;
		cFILE.getflag( "AUTO_LEVEL_MAN_SET", "COMMAND_LIST", i, dwNUM, strCommand );

		m_listCommand_Old.push_back( strCommand );
	}

	dwNUM = cFILE.GetKeyDataSize( "AUTO_LEVEL_MAN_SET", "FILE_LIST" );
	for( DWORD i=0; i<dwNUM; ++i )
	{
		std::string strFileName;
		cFILE.getflag( "AUTO_LEVEL_MAN_SET", "FILE_LIST", i, dwNUM, strFileName );

		m_listFileName.push_back( strFileName );
	}

	// Note : 자동 레벨 파일을 읽는다.
	STRING_LIST_ITER iter = m_listFileName.begin();
	STRING_LIST_ITER iter_end = m_listFileName.end();
	
	for( ; iter != iter_end; ++iter)
	{
		GLAutoLevel glAutoLevel;
		if ( glAutoLevel.LOAD_OLD(*iter, m_listCommand_Old, bToolMode) == false )
		{
            sc::writeLogError(
                sc::string::format(
                    "GLAutoLevelMan::LOAD File Open %1%", *iter));
			return false;
		}

		glAutoLevel.SetUse(true);
		m_autoLevelVector.push_back(glAutoLevel);
	}

	return true;
}


bool GLAutoLevelMan::FrameMove()
{
	CTime cCurTime = CTime::GetCurrentTime();

	const int nDayOfWeek = cCurTime.GetDayOfWeek() - 1;
	const int nDay = cCurTime.GetDay ();
	const int nHour = cCurTime.GetHour();
	const int nMinute(cCurTime.GetMinute());

	for ( AutoLevelReserveListIter _iteratorReserve(m_autoLevelReserveList.begin()); _iteratorReserve != m_autoLevelReserveList.end(); )
	{
		AutoLevelReserve& _autoLevelReserve = *_iteratorReserve;
		if ( _autoLevelReserve.isEventStartTime(nHour, nMinute) == true )
		{
			const unsigned int _nIndex(_autoLevelReserve.nIndex);

			_iteratorReserve = m_autoLevelReserveList.erase(_iteratorReserve);
			GLAutoLevel* const _pAutoLevel(GetAutoLevel(_nIndex));
			if ( _pAutoLevel == NULL )
				continue;

			_pAutoLevel->DoEventStart(nDay, nHour, nMinute, false);
		}
		else
			++_iteratorReserve;		
	}

	for ( GLAutoLevelVectorIter _iteratorLevel(m_autoLevelVector.begin()); _iteratorLevel != m_autoLevelVector.end(); ++_iteratorLevel )
	{
		GLAutoLevel& glAutoLevel = *_iteratorLevel;

		if ( glAutoLevel.IsEvent() == true )
		{ // GM Command로 실행한 auto-Level도 있으므로 진행 중인 이벤트에 대해선,
			// 갱신을 시켜준다;
			if ( glAutoLevel.IsEventEndTime( nHour, nMinute) == true )
				glAutoLevel.DoEventEnd();
			else
				glAutoLevel.DoEventScript(m_pServer, nHour, nMinute);
		}

		if ( glAutoLevel.IsUse() == false )
			continue;

		if ( glAutoLevel.IsEventStartTime( nDayOfWeek, nDay, nHour, nMinute ) == true )
			glAutoLevel.DoEventStart(nDay, nHour, nMinute);		
	}

	return true;
}

GLAutoLevel* GLAutoLevelMan::GetAutoLevel( int nIndex )
{
	if ( nIndex < 0 || nIndex >= (int)m_autoLevelVector.size() )
		return NULL;

	return &m_autoLevelVector[nIndex];
}

bool GLAutoLevelMan::DelAutoLevel( int nIndex )
{
	if ( nIndex < 0 || nIndex >= (int)m_autoLevelVector.size() )
		return false;

	m_autoLevelVector.erase(m_autoLevelVector.begin() + nIndex);
	return true;
}

const bool GLAutoLevelMan::messageProcedure(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID)
{
	GLCharAG* _pGM(m_pServer->GetChar(_dwGaeaID));
	if ( _pGM == NULL )
		return FALSE;
	if ( _pGM->m_dwUserLvl < USER_USER_GM )
		return FALSE;

	const unsigned int _nClientSlot(_pGM->ClientSlot());

	const GLMSG::SNET_GM_AUTO_LEVEL* const _pAutoLevelMessage((const GLMSG::SNET_GM_AUTO_LEVEL* const)_pMessage);
	switch ( _pAutoLevelMessage->nSubType )
	{
	case GLMSG::SNET_GM_AUTO_LEVEL::EMAUTO_LEVEL_LIST:
		return GLAutoLevelMan::_messageProcedureList(_pMessage, _dwGaeaID, _nClientSlot);
	case GLMSG::SNET_GM_AUTO_LEVEL::EMAUTO_LEVEL_TODAY:
		return GLAutoLevelMan::_messageProcedureToday(_pMessage, _dwGaeaID, _nClientSlot);
	case GLMSG::SNET_GM_AUTO_LEVEL::EMAUTO_LEVEL_ON:
		return GLAutoLevelMan::_messageProcedureOn(_pMessage, _dwGaeaID, _nClientSlot);
	case GLMSG::SNET_GM_AUTO_LEVEL::EMAUTO_LEVEL_OFF:
		return GLAutoLevelMan::_messageProcedureOff(_pMessage, _dwGaeaID, _nClientSlot);
	case GLMSG::SNET_GM_AUTO_LEVEL::EMAUTO_LEVEL_INFORMATION:
		return GLAutoLevelMan::_messageProcedureInformation(_pMessage, _dwGaeaID, _nClientSlot);
	case GLMSG::SNET_GM_AUTO_LEVEL::EMAUTO_LEVEL_BEGIN:
		return GLAutoLevelMan::_messageProcedureBegin(_pMessage, _dwGaeaID, _nClientSlot);
	case GLMSG::SNET_GM_AUTO_LEVEL::EMAUTO_LEVEL_END:
		return GLAutoLevelMan::_messageProcedureEnd(_pMessage, _dwGaeaID, _nClientSlot);
	case GLMSG::SNET_GM_AUTO_LEVEL::EMAUTO_LEVEL_PROCESS:
		return GLAutoLevelMan::_messageProcedureProcess(_pMessage, _dwGaeaID, _nClientSlot);
	case GLMSG::SNET_GM_AUTO_LEVEL::EMAUTO_LEVEL_RESERVE:
		return GLAutoLevelMan::_messageProcedureReserve(_pMessage, _dwGaeaID, _nClientSlot);
	}
	return false;
}

const bool GLAutoLevelMan::_messageProcedureInformation(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID, const unsigned int _nClientSlot)
{
	const GLMSG::SNET_GM_AUTO_LEVEL* const _pInformationMessage((const GLMSG::SNET_GM_AUTO_LEVEL* const)_pMessage);

	const unsigned int _indexAutoLevel(_pInformationMessage->nParameter0);
	GLAutoLevel* const _pAutoLevel(GetAutoLevel(_indexAutoLevel));
	if ( _pAutoLevel == NULL )
	{
		m_pServer->SENDTOCLIENT(_nClientSlot, &GLMSG::SNET_GM_AUTO_LEVEL_RESULT(GLMSG::SNET_GM_AUTO_LEVEL_RESULT::EMRESULT_WRONG_INDEX));
		return false;
	}
	
	GLMSG::SNET_GM_AUTO_LEVEL_INFORMATION _levelInformationMessage(_indexAutoLevel, _pAutoLevel->m_dwDay, _pAutoLevel->m_strName, _pAutoLevel->m_dwHallMapID, _pAutoLevel->m_dwMapID, _pAutoLevel->m_StartTime, _pAutoLevel->m_EndTime);
	m_pServer->SENDTOCLIENT(_nClientSlot, &_levelInformationMessage);

	return true;
}

const bool GLAutoLevelMan::_messageProcedureOn(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID, const unsigned int _nClientSlot)
{
	const GLMSG::SNET_GM_AUTO_LEVEL* const _pOnMessage((const GLMSG::SNET_GM_AUTO_LEVEL* const)_pMessage);
	
	const unsigned int _indexAutoLevel(_pOnMessage->nParameter0);
	GLAutoLevel* const _pAutoLevel(GetAutoLevel(_indexAutoLevel));
	if ( _pAutoLevel == NULL )
	{
		m_pServer->SENDTOCLIENT(_nClientSlot, &GLMSG::SNET_GM_AUTO_LEVEL_RESULT(GLMSG::SNET_GM_AUTO_LEVEL_RESULT::EMRESULT_WRONG_INDEX));
		return false;
	}

	_pAutoLevel->SetUse(true);

	return true;
}
const bool GLAutoLevelMan::_messageProcedureOff(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID, const unsigned int _nClientSlot)
{
	const GLMSG::SNET_GM_AUTO_LEVEL* const _pOffMessage((const GLMSG::SNET_GM_AUTO_LEVEL* const)_pMessage);

	const unsigned int _indexAutoLevel(_pOffMessage->nParameter0);
	GLAutoLevel* const _pAutoLevel(GetAutoLevel(_indexAutoLevel));
	if ( _pAutoLevel == NULL )
	{
		m_pServer->SENDTOCLIENT(_nClientSlot, &GLMSG::SNET_GM_AUTO_LEVEL_RESULT(GLMSG::SNET_GM_AUTO_LEVEL_RESULT::EMRESULT_WRONG_INDEX));
		return false;
	}

	if ( _pAutoLevel->IsEvent() == true )
	{
		m_pServer->SENDTOCLIENT(_nClientSlot, &GLMSG::SNET_GM_AUTO_LEVEL_RESULT(GLMSG::SNET_GM_AUTO_LEVEL_RESULT::EMRESULT_ALREADY_RUNNING));
		return false;
	}

	_pAutoLevel->SetUse(false);

	return true;
}

const bool GLAutoLevelMan::_messageProcedureReserve(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID, const unsigned int _nClientSlot)
{
	const GLMSG::SNET_GM_AUTO_LEVEL* const _pReserveMessage((const GLMSG::SNET_GM_AUTO_LEVEL* const)_pMessage);

	const unsigned int _indexAutoLevel(_pReserveMessage->nParameter0);
	GLAutoLevel* const _pAutoLevel(GetAutoLevel(_indexAutoLevel));
	if ( _pAutoLevel == NULL )
	{
		m_pServer->SENDTOCLIENT(_nClientSlot, &GLMSG::SNET_GM_AUTO_LEVEL_RESULT(GLMSG::SNET_GM_AUTO_LEVEL_RESULT::EMRESULT_WRONG_INDEX));
		return false;
	}

	if ( _pAutoLevel->IsEvent() == true )
	{
		m_pServer->SENDTOCLIENT(_nClientSlot, &GLMSG::SNET_GM_AUTO_LEVEL_RESULT(GLMSG::SNET_GM_AUTO_LEVEL_RESULT::EMRESULT_ALREADY_RUNNING));
		return false;
	}

	CTime cCurTime = CTime::GetCurrentTime();
	GLLEV_TIME _reserveTime(cCurTime.GetHour(), cCurTime.GetMinute());

	const unsigned int _nRemainMinute(_pReserveMessage->nParameter1);
	m_autoLevelReserveList.push_back(AutoLevelReserve(_reserveTime, _indexAutoLevel, _nRemainMinute));
	

	return true;
}
const bool GLAutoLevelMan::_messageProcedureProcess(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID, const unsigned int _nClientSlot)
{
	const GLMSG::SNET_GM_AUTO_LEVEL* const _pProcessMessage((const GLMSG::SNET_GM_AUTO_LEVEL* const)_pMessage);

	const unsigned int _indexAutoLevel(_pProcessMessage->nParameter0);	
	GLAutoLevel* const _pAutoLevel(GetAutoLevel(_indexAutoLevel));
	if ( _pAutoLevel == NULL )
	{
		m_pServer->SENDTOCLIENT(_nClientSlot, &GLMSG::SNET_GM_AUTO_LEVEL_RESULT(GLMSG::SNET_GM_AUTO_LEVEL_RESULT::EMRESULT_WRONG_INDEX));
		return false;
	}

	if ( _pAutoLevel->IsEvent() == false )
	{
		GLMSG::SNET_GM_AUTO_LEVEL_RESULT _resultMessage(GLMSG::SNET_GM_AUTO_LEVEL_RESULT::EMRESULT_NOT_RUNNING);
		m_pServer->SENDTOCLIENT(_nClientSlot, &_resultMessage);
		return false;
	}

	const unsigned int _nProcessMinute(_pProcessMessage->nParameter1);
	_pAutoLevel->DoEventProcess(_nProcessMinute);

	GLMSG::SNET_GM_AUTO_LEVEL_RESULT _resultMessage(GLMSG::SNET_GM_AUTO_LEVEL_RESULT::EMRESULT_SUCCESS_PROCESS);
	m_pServer->SENDTOCLIENT(_nClientSlot, &_resultMessage);

	return true;
}

const bool GLAutoLevelMan::_messageProcedureToday(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID, const unsigned int _nClientSlot)
{
	const GLMSG::SNET_GM_AUTO_LEVEL* const _pListMessage((const GLMSG::SNET_GM_AUTO_LEVEL* const)_pMessage);

	CTime cCurTime = CTime::GetCurrentTime();

	const int _nDayOfWeek(cCurTime.GetDayOfWeek() - 1);

	unsigned int _indexAutoLevel(0);
	GLMSG::SNET_GM_AUTO_LEVEL_LIST _levelListMessage;
	for ( GLAutoLevelVectorCIter _iteratorLevel(m_autoLevelVector.begin()); _iteratorLevel != m_autoLevelVector.end(); ++_iteratorLevel, ++_indexAutoLevel )
	{
		const GLAutoLevel& _autoLevel = *_iteratorLevel;
		if ( _autoLevel.m_dwDay[_nDayOfWeek] )
		{
			const std::string& _levelName = _autoLevel.m_strName;
			const bool _bRunning(_autoLevel.m_bEvent);
			const bool _bUse(_autoLevel.m_bUse);
			if ( _levelListMessage.addLevel(_levelName, _indexAutoLevel, _bRunning, _bUse) == false )
			{
				m_pServer->SENDTOCLIENT(_nClientSlot, &_levelListMessage);
				_levelListMessage.reset();
			}
		}		
	}

	m_pServer->SENDTOCLIENT(_nClientSlot, &_levelListMessage);

	return true;
}
const bool GLAutoLevelMan::_messageProcedureList(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID, const unsigned int _nClientSlot)
{
	const GLMSG::SNET_GM_AUTO_LEVEL* const _pListMessage((const GLMSG::SNET_GM_AUTO_LEVEL* const)_pMessage);

	unsigned int _indexAutoLevel(0);
	GLMSG::SNET_GM_AUTO_LEVEL_LIST _levelListMessage;
	for ( GLAutoLevelVectorCIter _iteratorLevel(m_autoLevelVector.begin()); _iteratorLevel != m_autoLevelVector.end(); ++_iteratorLevel, ++_indexAutoLevel )
	{
		const GLAutoLevel& _autoLevel = *_iteratorLevel;
		const std::string& _levelName = _autoLevel.m_strName;
		const bool _bRunning(_autoLevel.m_bEvent);
		const bool _bUse(_autoLevel.m_bUse);
		if ( _levelListMessage.addLevel(_levelName, _indexAutoLevel, _bRunning, _bUse) == false )
		{
			m_pServer->SENDTOCLIENT(_nClientSlot, &_levelListMessage);
			_levelListMessage.reset();
		}
	}

	m_pServer->SENDTOCLIENT(_nClientSlot, &_levelListMessage);

	return true;
}
const bool GLAutoLevelMan::_messageProcedureBegin(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID, const unsigned int _nClientSlot)
{
	const GLMSG::SNET_GM_AUTO_LEVEL* const _pBeginMessage((const GLMSG::SNET_GM_AUTO_LEVEL* const)_pMessage);

	const unsigned int _indexAutoLevel(_pBeginMessage->nParameter0);
	GLAutoLevel* const _pAutoLevel(GetAutoLevel(_indexAutoLevel));
	if ( _pAutoLevel == NULL )
	{
		m_pServer->SENDTOCLIENT(_nClientSlot, &GLMSG::SNET_GM_AUTO_LEVEL_RESULT(GLMSG::SNET_GM_AUTO_LEVEL_RESULT::EMRESULT_WRONG_INDEX));
		return false;
	}
	
	if ( _pAutoLevel->IsEvent() == true )
	{
		GLMSG::SNET_GM_AUTO_LEVEL_RESULT _resultMessage(GLMSG::SNET_GM_AUTO_LEVEL_RESULT::EMRESULT_ALREADY_RUNNING);
		m_pServer->SENDTOCLIENT(_nClientSlot, &_resultMessage);
		return false;
	}

	CTime _currentTime(CTime::GetCurrentTime());
	const unsigned int _nDay(_currentTime.GetDay());
	const unsigned int _nHour(_currentTime.GetHour());
	const unsigned int _nMinute(_currentTime.GetMinute());

	_pAutoLevel->DoEventStart(_nDay, _nHour, _nMinute, false);
	GLMSG::SNET_GM_AUTO_LEVEL_RESULT _resultMessage(GLMSG::SNET_GM_AUTO_LEVEL_RESULT::EMRESULT_SUCCESS_BEGIN);
	m_pServer->SENDTOCLIENT(_nClientSlot, &_resultMessage);

	return true;
}
const bool GLAutoLevelMan::_messageProcedureEnd(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID, const unsigned int _nClientSlot)
{
	const GLMSG::SNET_GM_AUTO_LEVEL* const _pEndMessage((const GLMSG::SNET_GM_AUTO_LEVEL* const)_pMessage);
	
	const unsigned int _indexAutoLevel(_pEndMessage->nParameter0);
	GLAutoLevel* const _pAutoLevel(GetAutoLevel(_indexAutoLevel));
	if ( _pAutoLevel == NULL )
	{
		m_pServer->SENDTOCLIENT(_nClientSlot, &GLMSG::SNET_GM_AUTO_LEVEL_RESULT(GLMSG::SNET_GM_AUTO_LEVEL_RESULT::EMRESULT_WRONG_INDEX));
		return false;
	}

	if ( _pAutoLevel->IsEvent() == false )
	{
		GLMSG::SNET_GM_AUTO_LEVEL_RESULT _resultMessage(GLMSG::SNET_GM_AUTO_LEVEL_RESULT::EMRESULT_NOT_RUNNING);
		m_pServer->SENDTOCLIENT(_nClientSlot, &_resultMessage);
		return false;
	}

	_pAutoLevel->DoEventEnd();

	{
		GLMSG::SNET_GM_MOB_LEV_CLEAR NetMsg;
		SNATIVEID nidMAP( _pAutoLevel->m_dwMapID );
		NetMsg.wMAP_MID = nidMAP.wMainID;
		NetMsg.wMAP_SID = nidMAP.wSubID;
		m_pServer->SENDTOALLCHANNEL( &NetMsg );
	}

	{
		GLMSG::SNET_GM_MOB_LEV_CLEAR NetMsg;
		SNATIVEID nidMAP( _pAutoLevel->m_dwHallMapID );
		NetMsg.wMAP_MID = nidMAP.wMainID;
		NetMsg.wMAP_SID = nidMAP.wSubID;
		m_pServer->SENDTOALLCHANNEL( &NetMsg );
	}

	GLMSG::SNET_GM_AUTO_LEVEL_RESULT _resultMessage(GLMSG::SNET_GM_AUTO_LEVEL_RESULT::EMRESULT_SUCCESS_END);
	m_pServer->SENDTOCLIENT(_nClientSlot, &_resultMessage);

	return true;
}
