#include "../pch.h"


#include "../../SigmaCore/Lua/MinLua.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringUtil.h"
#include "../../SigmaCore/Loading/NSLoadingDirect.h"
#include "../../SigmaCore/File/TxtFile.h"
#include "../../SigmaCore/Util/SystemInfo.h"

#include "../../EngineLib/G-Logic/GLogic.h"
#include "../../EngineLib/Common/GLTexFile.h"
#include "../../EngineLib/Common/SUBPATH.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../RanLogic/RANPARAM.h"

#include "../GLogicDataMan.h"
#include "GLQuestMan.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

static GLQuestMan* g_pInstance(NULL);

GLQuestMan& GLQuestMan::GetInstance()
{
	if ( !g_pInstance )
	{
		g_pInstance = new GLQuestMan;
	}
	return *g_pInstance;
}

void GLQuestMan::ReleaseInstance()
{
	SAFE_DELETE( g_pInstance );
}

GLQuestMan::GLQuestMan()
    : m_bModify(false)
	, m_poolGLQuestProg(_T("GLQUESTPROG"))
{
	m_QuestByteStreamID = 0; // QUEST_BUFFER_OVER
}

GLQuestMan::~GLQuestMan()
{
	Clean ();
}

GLQUEST* GLQuestMan::Find(DWORD dwQuestID)
{    
	MAPQUEST_ITER iter = m_mapQuestMap.find(dwQuestID);
	if (iter != m_mapQuestMap.end())
	{
		SNODE& sNODE = (*iter).second;		
		if (!sNODE.pQUEST)
		{
			sNODE.pQUEST = LoadQuest(sNODE.strFILE, dwQuestID);
// 			if (!sNODE.pQUEST)
// 			{
//                 SNATIVEID QuestID(dwQuestID);
//                 std::string ErrMsg(
//                     sc::string::format(
//                         "QUEST load fail %1%/%2% %3%",
//                         QuestID.wMainID,
//                         QuestID.wSubID,
//                         sNODE.strFILE));				
//                 sc::writeLogError(ErrMsg);
// 			}
		}
		return sNODE.pQUEST;
	}
    else
    {
//         SNATIVEID QuestID(dwQuestID);
//         std::string ErrMsg(
//             sc::string::format(
//                 "QUEST load fail %1%/%2%",
//                 QuestID.wMainID,
//                 QuestID.wSubID));
//         sc::writeLogError(ErrMsg);
	    return NULL;
    }
}

// 해당 기능은 학원 변경기능을 사용할때 퀘스트를 맞춰서 변경해 주기 위해서 사용됨
// 현재 보유중인 퀘스트와 매칭되는 퀘스트 아이디를 확인
// 반환되는 퀘스트 아이디가 0인경우는 실패
int GLQuestMan::GetConvertQuestGroup(DWORD dwQuestID, WORD wBeforeSchool, DWORD dwClass, WORD wAfterSchool)
{
	int dwConvertedQuestID = -1;

	for( int i = 0; i < m_vecGroupQuest.size(); i++ )
	{
		dwConvertedQuestID = m_vecGroupQuest[i].Check(dwQuestID, wBeforeSchool, dwClass, wAfterSchool);

		if( dwConvertedQuestID != -1 ) break;
	}

	return dwConvertedQuestID;
}

const std::string& GLQuestMan::GetQuestName(WORD MainId, WORD SubId)
{
    SNATIVEID QuestId(MainId, SubId);
    return GetQuestName(QuestId.Id());
}

const std::string& GLQuestMan::GetQuestName(const SNATIVEID& QuestId)
{
    return GetQuestName(QuestId.Id());
}

const std::string& GLQuestMan::GetQuestName(DWORD QuestId)
{
    GLQUEST* pQuestData = Find(QuestId);
    if (pQuestData)
    {
		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			CString strKey;
			strKey.Format("QN_%03d_%03d", pQuestData->m_sNID.wMainID, pQuestData->m_sNID.wSubID);
			return CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString();
		}
		else
		{
			return pQuestData->GetTitleStr();
		}
    }
    else
    {
        static std::string TempStr("");
        return TempStr;
    }
}

DWORD GLQuestMan::MakeNewQNID()
{
	DWORD dwQuestID(0);
	while (1)
	{
		GLQUEST* pQuest = Find(dwQuestID);
		if (!pQuest)
            return dwQuestID;
		dwQuestID++;
	};
	return UINT_MAX;
}

GLQuestMan::QUESTGEN_RANGE GLQuestMan::FindStartFromGetITEM(SNATIVEID nidITEM)
{
	return m_mapStartItem.equal_range(nidITEM.dwID);
}

GLQuestMan::QUESTGEN_RANGE GLQuestMan::FindStartFromGetSKILL(SNATIVEID nidSKILL)
{
	return m_mapStartSkill.equal_range(nidSKILL.dwID);
}

GLQuestMan::QUESTGEN_RANGE GLQuestMan::FindStartFromGetLEVEL(WORD wLEVEL)
{
	return m_mapStartLevel.equal_range(wLEVEL);
}

GLQuestMan::QUESTGEN_RANGE GLQuestMan::FindStartFromGetCharJOIN(int nFlag)
{
	return m_mapStartJoin.equal_range(nFlag);
}

void GLQuestMan::ShuffleAttendanceQuest( unsigned unSeed, std::vector<DWORD> &vecQuestID )
{
	vecQuestID = m_vecAttendanceQuest;
	//srand( unSeed );
	random_shuffle(vecQuestID.begin(), vecQuestID.end());
}

std::vector<DWORD> GLQuestMan::FindAttendanceQuest( WORD wLevel, int nQuestNum, unsigned unSeed )
{
	WORD wCharLevel = wLevel;
	WORD wMinLevel = 0;
	WORD wMaxLevel = USHRT_MAX;
	//int nQuestNum = nQuestNum;
	std::vector<DWORD> vecSelectQuest;
	vecSelectQuest.clear();
	CTime TToday = CTime::GetCurrentTime();

	int nDay = TToday.GetDay();
	int nDayOfWeek = TToday.GetDayOfWeek() + 1; // 비교대상이 플래그 여서 1을 더해준다
	int nMonth = TToday.GetMonth();
	int nYear = TToday.GetYear();

	std::vector<DWORD> vecAttendanceQuest;
	ShuffleAttendanceQuest(unSeed, vecAttendanceQuest);
	BOOST_FOREACH( DWORD dwQuestID, vecAttendanceQuest)
	{
		GLQUEST* pQuest = Find(dwQuestID);

		if ( pQuest && !pQuest->m_sAttendanceOption.bAdditionalQuest )
		{
			//요일 검사
			if ( pQuest->m_sAttendanceOption.dwDayofWeek & (1 << nDayOfWeek) )
			{
				wMinLevel = pQuest->m_sAttendanceOption.wMinLevel;
				wMaxLevel = pQuest->m_sAttendanceOption.wMaxLevel;
				//래밸 검사
				if ( ( wMinLevel <= wCharLevel ) && ( wMaxLevel >= wCharLevel ) )
				{
					if ( static_cast<int>(vecSelectQuest.size()) < nQuestNum )
					{
						//타입 중복 검사
						bool bSameType = true;
						bSameType = IsDuplicateType(vecSelectQuest,dwQuestID,pQuest->m_sAttendanceOption.nType);
						if (!bSameType)
						{
							vecSelectQuest.push_back(dwQuestID);
						}
					}
					else
						return vecSelectQuest;
				}
			}
		}
	}

	if ( static_cast<int>(vecSelectQuest.size()) < nQuestNum )
	{
		std::vector<DWORD> vecAttendanceQuest;
		ShuffleAttendanceQuest(unSeed, vecAttendanceQuest);
		BOOST_FOREACH( DWORD dwQuestID, vecAttendanceQuest)
		{
			GLQUEST* pQuest = Find(dwQuestID);

			if ( pQuest && !pQuest->m_sAttendanceOption.bAdditionalQuest )
			{
				wMinLevel = pQuest->m_sAttendanceOption.wMinLevel;
				wMaxLevel = pQuest->m_sAttendanceOption.wMaxLevel;

				if ( ( wMinLevel <= wCharLevel ) && ( wMaxLevel >= wCharLevel ) )
				{
					if ( static_cast<int>(vecSelectQuest.size()) < nQuestNum )
					{
						bool bSameType = true;
						bSameType = IsDuplicateType(vecSelectQuest,dwQuestID,pQuest->m_sAttendanceOption.nType);
						if (!bSameType)
						{
							vecSelectQuest.push_back(dwQuestID);
						}
					}
					else
						return vecSelectQuest;
				}
			}
		}
	}

	return vecSelectQuest;
}


bool GLQuestMan::IsDuplicateType( std::vector<DWORD> vecQuestID, DWORD dwQuestID, int emATTENDANCE_TYPE )
{
	BOOST_FOREACH( DWORD dwQuestID, vecQuestID)
	{
		GLQUEST* pSelectQuest = Find(dwQuestID);
		if ( pSelectQuest->m_sAttendanceOption.nType == emATTENDANCE_TYPE )
		{
			return true;
		}
	}
	return false;
}


void GLQuestMan::Insert ( DWORD dwID, SNODE sNODE )
{
	if ( sNODE.pQUEST )
	{
		sNODE.pQUEST->m_sNID.dwID = dwID;

		// 자동 시작 퀘스트
		GLQUEST_START &sSTART = sNODE.pQUEST->m_sSTARTOPT;
		if ( sSTART.nidITEM!=SNATIVEID(false) )
			m_mapStartItem.insert ( std::make_pair(sSTART.nidITEM.dwID,dwID) );
		
		if ( sSTART.nidSKILL!=SNATIVEID(false) )
			m_mapStartSkill.insert ( std::make_pair(sSTART.nidSKILL.dwID,dwID) );

		if ( sSTART.wLEVEL!=USHRT_MAX )
			m_mapStartLevel.insert ( std::make_pair(sSTART.wLEVEL,dwID) );

		if ( sSTART.nJOIN > 0 )
			m_mapStartJoin.insert( std::make_pair(sSTART.nJOIN,dwID) );

		// 출석부 퀘스트
		if ( sNODE.pQUEST->m_sAttendanceOption.nType != EMATTENDANCE_TYPE_NONE &&
			sNODE.pQUEST->m_sSTARTOPT.nJOIN != EMQJ_ATTENDANCE &&
			!sNODE.pQUEST->m_sAttendanceOption.bAdditionalQuest )
		{
			m_vecAttendanceQuest.push_back( dwID );
		}
		else if (sNODE.pQUEST->m_sAttendanceOption.bAdditionalQuest)
		{
			m_vecAttendancAdditionalQuest.push_back( dwID );
		}

		if (sNODE.pQUEST->m_sSTARTOPT.nJOIN == EMQJ_ATTENDANCE )
		{
			m_dwRnattendanceQuestId = dwID;
		}

	}

	MAPQUEST_ITER iter = m_mapQuestMap.find ( dwID );
	if ( iter!=m_mapQuestMap.end() )
	{
        std::string ErrorMsg(
            sc::string::format(
                "QUEST ID Duplication : %1%", dwID));
		sc::writeLogError(ErrorMsg);

		SNODE &sNODE = (*iter).second;
		SAFE_DELETE(sNODE.pQUEST);
		m_mapQuestMap.erase( iter );
	}

	m_mapQuestMap.insert ( std::make_pair(dwID,sNODE) );
}

bool GLQuestMan::Delete ( DWORD dwQuestID )
{
	MAPQUEST_ITER iter = m_mapQuestMap.find ( dwQuestID );
	if ( iter!=m_mapQuestMap.end() )
	{
		SNODE &sNODE = (*iter).second;
		SAFE_DELETE ( sNODE.pQUEST );
		m_mapQuestMap.erase ( iter );

		return true;
	}

	return false;
}

void GLQuestMan::Clean ()
{
	MAPQUEST_ITER iter = m_mapQuestMap.begin();
	MAPQUEST_ITER iter_end = m_mapQuestMap.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SNODE& sNODE = iter->second;
		SAFE_DELETE(sNODE.pQUEST);
	}

	m_mapQuestMap.clear();
}

GLQUEST* GLQuestMan::LoadQuest(const std::string& FileName, DWORD dwQuestID)
{
    GLQUEST* pQUEST = new GLQUEST;
    if (!pQUEST)
        return NULL;

    bool bLOAD = pQUEST->LOAD(FileName);
    pQUEST->m_sNID.dwID = dwQuestID;
    if (bLOAD)
    {
        return pQUEST;
    }
    else
    {
        SAFE_DELETE(pQUEST);
        return NULL;
    }    
}

bool GLQuestMan::LoadGroupQuest(const std::string& FileName)
{
	std::string strPath(GLOGIC::GetServerPath());
    strPath.append(FileName);

	try
    {   
        sc::lua_init();
        if ( !sc::lua_doFile( strPath ) )
        {
			sc::writeLogError( sc::string::format("%1% file missing.(error 9314)", strPath) );
            return false;
        }

		LuaPlus::LuaObject common = sc::lua_getGlobalFromName( "QuestGroup" );

		for (LuaPlus::LuaTableIterator Iter( common ); Iter; Iter.Next() )
		{
			for ( LuaPlus::LuaTableIterator IterData( Iter.GetValue() ); IterData; IterData.Next() )
			{
				SQUESTGROUP sQuestGroup;
				SNATIVEID sQuestIDFrom;
				SNATIVEID sQuestIDTo;

				// FromQuestID(MID, SID), BeforeSchool, CurrentClass, ToQuestID(MID, SID), AfterSchool

				// FromQuestID
				sQuestIDFrom.wMainID = static_cast< WORD >( IterData.GetValue().GetFloat() );
				IterData.Next();
				sQuestIDFrom.wSubID = static_cast< WORD >( IterData.GetValue().GetFloat() );
				IterData.Next();
				// school
				sQuestGroup.m_nBeforeSchool = static_cast< WORD >( IterData.GetValue().GetFloat() );			
				IterData.Next();
				// class
				sQuestGroup.m_dwClass = static_cast< DWORD >( IterData.GetValue().GetFloat() );			
				IterData.Next();

				// ToQuestID
				sQuestIDTo.wMainID = static_cast< WORD >( IterData.GetValue().GetFloat() );
				IterData.Next();
				sQuestIDTo.wSubID = static_cast< WORD >( IterData.GetValue().GetFloat() );
				IterData.Next();
				// school
				sQuestGroup.m_nAfterSchool = static_cast< WORD >( IterData.GetValue().GetFloat() );			
				//IterData.Next();


				sQuestGroup.m_dwFromQuestID = sQuestIDFrom.dwID;
				sQuestGroup.m_dwToQuestID = sQuestIDTo.dwID;

				m_vecGroupQuest.push_back(sQuestGroup);
			}
		}

    }
    catch ( const LuaPlus::LuaException& e )
    {
		sc::writeLogError( sc::string::format( "%1%, %2%", strPath, e.GetErrorMessage()) );
        return false;
    }	

	sc::writeLogInfo( std::string( "QUEST GROUP MAPPING SUCCESS") );

	return true;
}

bool GLQuestMan::LoadFile(const std::string& FileName, bool bQUEST_LOAD)
{
	// 전장서버에서는 로드하지 않는다;
	if ( GLUseFeatures::GetInstance().IsUsingWorldBattle() )
		return true;

	ResetModified();

	std::string strLIST_FILE = std::string(GetPath()) + FileName;

	gltexfile cFILE;
	cFILE.reg_sep("\t ,");

	if (GLOGIC::UseLogicZipFile()) // by 경대
		cFILE.SetZipFile(GLogicData::GetInstance().GetQuestZipFile()); // by 경대, zip파일 사용

	if (!cFILE.open(strLIST_FILE, false, GLOGIC::UseLogicPackFile()))
	{
        sc::writeLogError(sc::string::format("GLQuestMan::LoadFile %1%", strLIST_FILE));		
		return false;
	}

	DWORD dwNUM = cFILE.getflagnum ( "QUEST" );
	for ( DWORD i=0; i<dwNUM; ++i )
	{
		DWORD dwID;
		SNODE sNODE;

		cFILE.getflag ( i, "QUEST", 1, 2, dwID, true );
		cFILE.getflag ( i, "QUEST", 2, 2, sNODE.strFILE, true );

		if ( bQUEST_LOAD )
		{
            //---------------------------------------------------//
            NSLoadingDirect::FrameMoveRender( FALSE );
            //---------------------------------------------------//

// 			if ( strcmp("QUEST_2083.qst", sNODE.strFILE.c_str()) == 0 )
// 				break;

			sNODE.pQUEST = LoadQuest(sNODE.strFILE, dwID);
			if (!sNODE.pQUEST)
				MessageBox ( NULL, sNODE.strFILE.c_str(), "quest file not found.", MB_OK );
		}		

		Insert(dwID, sNODE);
	}
	return true;
}
 
bool GLQuestMan::SaveFile ( const char* szFile )
{
	MAPQUEST_ITER iter_pos;
	SNODE sNode;

	std::string strLIST_FILE = std::string(GetPath()) + szFile;

	FILE* file=NULL;
	file = _fsopen ( strLIST_FILE.c_str(), "wt", _SH_DENYNO );
	if ( !file )	return FALSE;

	fprintf( file , "//Quest List\n" );
	fprintf( file , "//\tID\tfile\t\tquest name\n" );


	for ( iter_pos = m_mapQuestMap.begin(); iter_pos != m_mapQuestMap.end(); iter_pos++ )
	{
		sNode = (SNODE) iter_pos->second;

		fprintf ( file, "QUEST\t%d\t%s", sNode.pQUEST->m_sNID.dwID, sNode.strFILE.c_str() );
		fprintf ( file, "\t//%s\n", sNode.pQUEST->m_strTITLE.c_str() );
	}
	
	fclose( file );

	// String Data Save;
	SaveStrTable( szFile );
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		CGameTextMan::GetInstance().SaveXML(RANPARAM::strQuestText, CGameTextMan::EM_QUEST_TEXT, FALSE);
	}
	return true;
}

std::string GLQuestMan::SaveSqlFile()
{
	if ( m_mapQuestMap.empty() )
		return std::string( "Quest data null" );

	std::string Path( sc::getAppPath() );
	Path.append("\\QuestList.sql");    

	sc::TxtFile SqlFile( Path, true, sc::TxtFile::ANSI );

	SqlFile.write("DELETE dbo.QuestList");
	SqlFile.write("GO");

	MAPQUEST_ITER iter = m_mapQuestMap.begin();
	MAPQUEST_ITER iter_end = m_mapQuestMap.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SNODE& sNODE = iter->second;
		GLQUEST* pQuest = sNODE.pQUEST;

		if ( pQuest )
		{
			WORD wMID = pQuest->m_sNID.Mid();
			WORD wSID = pQuest->m_sNID.Sid();
			std::string strQuestTitle = pQuest->GetTitleStr();
			sc::string::replaceForDb( strQuestTitle );
			//sc::string::replace( strQuestTitle, "'", "''" );

			SqlFile.write(
				sc::string::format(
				"INSERT INTO QuestList ( QuestMain, QuestSub, QuestName ) VALUES( %1%, %2%, '%3%' )",
				wMID,
				wSID,
				strQuestTitle ) );
		}
	}

	SqlFile.write("GO");
	std::string FileName = SqlFile.FileName();
	return FileName;
}

void GLQuestMan::SaveStrTable ( const char* szFile )
{
// 	std::string strLIST_FILE = std::string(GetPath()) + szFile;
// 	FILE* file=NULL;
// 	file = _fsopen ( strLIST_FILE.c_str(), "wt", _SH_DENYNO );
// 	if ( !file )	return;

	CString strBuffer, strName, strData, strTemp, strStepName, strStepDesc;
	strBuffer.ReleaseBuffer(NULL);
	std::string strFind = "\r\n";
	std::string strReplace = "\\r\\n";

	MAPQUEST_ITER iter_pos;
	SNODE sNode;

	for ( iter_pos = m_mapQuestMap.begin(); iter_pos != m_mapQuestMap.end(); iter_pos++ )
	{
		sNode = (SNODE) iter_pos->second;

		int nFind;
		strTemp = sNode.pQUEST->m_strCOMMENT.c_str();
		while( ( nFind = strTemp.Find( strFind.c_str() ) ) != -1 )
		{
			strTemp.Delete( nFind, strFind.length() );
			strTemp.Insert( nFind, strReplace.c_str() );
		}

		strName.Format( _T("QN_%03d_%03d\t%s\r\n"), sNode.pQUEST->m_sNID.wMainID, sNode.pQUEST->m_sNID.wSubID, sNode.pQUEST->m_strTITLE.c_str() );
		strData.Format( _T("QD_%03d_%03d\t%s\r\n"), sNode.pQUEST->m_sNID.wMainID, sNode.pQUEST->m_sNID.wSubID, strTemp );
		strBuffer += strName;
		strBuffer += strData;

		// Quest Step
		for ( size_t i = 0; i < sNode.pQUEST->m_vecProgStep.size(); ++i )
		{
			strStepName.Format( _T("QN_%03d_%03d_P_%d\t%s\r\n")
				, sNode.pQUEST->m_sNID.wMainID
				,  sNode.pQUEST->m_sNID.wSubID
				, i, sNode.pQUEST->m_vecProgStep[i].GetTITLE() );
			strTemp = sNode.pQUEST->m_vecProgStep[i].GetCOMMENT();
			nFind = 0;
			while( ( nFind = strTemp.Find( strFind.c_str() ) ) != -1 )
			{
				strTemp.Delete( nFind, strFind.length() );
				strTemp.Insert( nFind, strReplace.c_str() );
			}
			strStepDesc.Format( _T("QD_%03d_%03d_D_%d\t%s\r\n")
				, sNode.pQUEST->m_sNID.wMainID
				, sNode.pQUEST->m_sNID.wSubID
				, i, strTemp );
			strBuffer += strStepName;
			strBuffer += strStepDesc;
		}
	}

//	fclose( file );

	sc::CRijndael oRijndael;
	oRijndael.Initialize( sc::CRijndael::sm_Version[sc::CRijndael::VERSION-1].c_str(), sc::CRijndael::DEFAULT_KEY_LENGTH );

	int nLen = oRijndael.GetEncryptStringLength( strBuffer );
	char * szBuffer = new char[nLen];

	oRijndael.EncryptEx( strBuffer.GetString(), szBuffer, strBuffer.GetLength() );
	int nVersion = sc::CRijndael::VERSION;

	CFile Cfile;
	std::string szFullPathFileName = std::string(GetPath()) + "strstrstr.txt";

	Cfile.Open( _T(szFullPathFileName.c_str()), CFile::modeCreate|CFile::modeWrite );
	Cfile.Write( &nVersion, sizeof(int) );
	Cfile.Write( szBuffer, nLen );


	CFile fileDefult;
	szFullPathFileName = std::string(GetPath()) + "strstr.txt";

	fileDefult.Open( _T(szFullPathFileName.c_str()), CFile::modeCreate|CFile::modeWrite );
	fileDefult.Write( &nVersion, sizeof(int) );
	fileDefult.Write( strBuffer, nLen );
}

void GLQuestMan::SetRnattenadenceReward(WORD wDayNum, WORD wMid, WORD wSid )
{
	SNATIVEID sItemId( wMid, wSid );

	m_mapRnattendanceReward.insert( std::make_pair (wDayNum,sItemId) );
}

bool GLQuestMan::LoadRnattenadenceReward(const std::string& pPath)
{
	m_mapRnattendanceReward.clear();

	try
	{
		sc::lua_init();
		if (!sc::lua_doFile(pPath.c_str()))
		{        
			::MessageBoxA(
				NULL,
				sc::lua_getError().c_str(),
				pPath.c_str(),
				MB_OK);
			return false;
		}

		LuaPlus::LuaObject RewardDayList = sc::lua_getGlobalFromName("RnattendanceReward");
		for (LuaPlus::LuaTableIterator DayIter(RewardDayList); DayIter; DayIter.Next())
		{
			int nRewardData[3] = {0,0,0};
			int nIndex = 0;
			for (LuaPlus::LuaTableIterator RewardIter(DayIter.GetValue()); RewardIter; RewardIter.Next())
			{
				if (nIndex < 0 || nIndex > 3)
				{
					std::string ErrorMsg("RnattenadenceReward Item wrong");
					AfxMessageBox(ErrorMsg.c_str(), MB_OK);                        
				}
				else
				{
					nRewardData[nIndex] = RewardIter.GetValue().GetInteger();
					nIndex++;
				}
			}
			WORD wDay = static_cast<WORD>(nRewardData[0]);
			int nItemMId = nRewardData[1];
			int nItemSId = nRewardData[2];

			SetRnattenadenceReward( wDay, nItemMId, nItemSId );
		}
		return true;
	}
	catch (const LuaPlus::LuaException& e)
	{        
		::MessageBoxA(
			NULL,
			e.GetErrorMessage(),
			pPath.c_str(),
			MB_OK);
		return false;
	}

	return true;
}

DWORD GLQuestMan::GetRnattendanceRewardNDay(int nday)
{
	if ( m_mapRnattendanceReward.end() == m_mapRnattendanceReward.find( nday ) )
	{
		return UINT_MAX;
	}
	else
	{
		RNATTENDANCEREWARD_ITER iter = m_mapRnattendanceReward.find( nday );
		return iter->second.dwID;
	}
	
}

void GLQuestMan::SetRnattendanceGQuestProg(GLQUESTPROG* pQuestProg)
{
	m_pRnattendanceQuestProg = pQuestProg;
}

GLQUESTPROG* GLQuestMan::GetRnattendanceQuestProg()
{
	return m_pRnattendanceQuestProg;
}

DWORD GLQuestMan::GetRnattendanceQuestId()
{
	return m_dwRnattendanceQuestId;
}

std::vector<DWORD> GLQuestMan::GetAttendancAdditionalQuest()
{
	return m_vecAttendancAdditionalQuest;
}
