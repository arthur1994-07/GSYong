#include "../pch.h"
#include "../../SigmaCore/String/StringUtils.h"
#include "../../SigmaCore/File/StringFile.h"
#include "../../SigmaCore/File/SerialFile.h"
#include "../../SigmaCore/Log/LogMan.h"

#include "../../EngineLib/Common/IniLoader.h"
#include "../../EngineLib/G-Logic/GLogic.h"

#include "../GLogicData.h"
#include "../GLogicDataMan.h"
#include "./GLTaxiStation.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

BOOL STAXI_STATION::LoadFile( sc::BaseStream &SFile )
{
	DWORD dwVersion;
	
	SFile >> dwVersion;
	if ( dwVersion != STAXI_STATION::VERSION ) return false;

	SFile >> dwLINKID;

	SFile >> bUsed;

	SFile >> dwMAPID;
	SFile >> wPosX;
	SFile >> wPosY;
	SFile >> dwNPCID;
	SFile >> dwMapCharge;

	SFile >> strMAP;
	SFile >> strSTATION;

	return true;
}

BOOL STAXI_STATION::SaveFile( sc::SerialFile &SFile )
{
	SFile << DWORD(VERSION);
	
	SFile << dwLINKID;

	SFile << bUsed;

	SFile << dwMAPID;
	SFile << wPosX;
	SFile << wPosY;
	SFile << dwNPCID;
	SFile << dwMapCharge;

	SFile << strMAP;
	SFile << strSTATION;

	return true;
}

void STAXI_STATION::SaveCsvHead ( std::fstream &SFile )
{
	SFile << "Id"         << ",";

	SFile << "Used"      << ",";

	SFile << "Map MID"    << ",";
	SFile << "Map SID"    << ",";

	SFile << "PosX"       << ",";
	SFile << "PosY"       << ",";

	SFile << "Npc MID"    << ",";
	SFile << "Npc SID"    << ",";

	SFile << "Map Charge" << ",";

	SFile << "Map Name"   << ",";
	SFile << "Npc Name"   << ",";

	SFile << std::endl;
}

bool STAXI_STATION::SaveCsv ( std::fstream &SFile )
{
	SNATIVEID sTempID;

	SFile << dwLINKID        << ",";
	SFile << ((INT)bUsed)    << ",";

	sTempID.dwID = dwMAPID;
	SFile << sTempID.wMainID << ",";
	SFile << sTempID.wSubID  << ",";

	SFile << wPosX           << ",";
	SFile << wPosY           << ",";

	sTempID.dwID = dwNPCID;
	SFile << sTempID.wMainID << ",";
	SFile << sTempID.wSubID  << ",";

	SFile << dwMapCharge     << ",";

	STRUTIL::OutputStrCsv( SFile, strMAP );
	STRUTIL::OutputStrCsv( SFile, strSTATION );

	SFile << std::endl;

	return true;
}

bool STAXI_STATION::LoadCsv ( CStringArray &StrArray )
{
	SNATIVEID   sTempID;
	std::string strTemp;
	int iCsvCur = 0;

	dwLINKID = (DWORD) atoi( StrArray[ iCsvCur++ ] );
	bUsed    = (0!=atoi( StrArray[ iCsvCur++ ] ) );

	sTempID.wMainID = (WORD) atoi( StrArray[ iCsvCur++ ] );
	sTempID.wSubID  = (WORD) atoi( StrArray[ iCsvCur++ ] );
	dwMAPID = sTempID.dwID;

	wPosX = (WORD) atoi( StrArray[ iCsvCur++ ] );
	wPosY = (WORD) atoi( StrArray[ iCsvCur++ ] );

	sTempID.wMainID = (WORD) atoi( StrArray[ iCsvCur++ ] );
	sTempID.wSubID  = (WORD) atoi( StrArray[ iCsvCur++ ] );
	dwNPCID = sTempID.dwID;

	dwMapCharge = (DWORD) atoi( StrArray[ iCsvCur++ ] );

	STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strMAP );
	STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strSTATION );

	return true;
}

const char* GLTaxiStation::_FILEHEAD = "GLTAXI_STATION";

GLTaxiStation& GLTaxiStation::GetInstance ()
{
	static GLTaxiStation _instance;
	return _instance;
}

GLTaxiStation::GLTaxiStation(void)
: m_dwBasicCharge ( 0 )
{
	memset(m_szFileName, 0, sizeof(char) * (MAX_PATH));
}

GLTaxiStation::GLTaxiStation( const GLTaxiStation& value)
{
	
}

GLTaxiStation::~GLTaxiStation(void)
{
	CleanUp();
}

void GLTaxiStation::CleanUp(void)
{
	m_vecTaxiMap.clear();
	m_dwBasicCharge = 0;
}



void GLTaxiStation::insert ( STAXI_STATION &sSTATION )
{
	//	Note : 기존에 등록된 맵 목록이 있는지 검사
	//
	DWORD dwSIZE = (DWORD) m_vecTaxiMap.size();
	for ( DWORD i=0; i<dwSIZE; ++i )
	{
		STAXI_MAP &sTaxiMap = m_vecTaxiMap[i];
		if ( sTaxiMap.dwMAPID == sSTATION.dwMAPID )
		{
			//	Note : 기존 마을에 등록.
			//
			sTaxiMap.vecTaxiStation.push_back ( sSTATION );
			return;
		}
	}

	//	Note : 새로운 마을 등록.
	//
	STAXI_MAP sTaxiMap_NEW;
	sTaxiMap_NEW.dwMAPID = sSTATION.dwMAPID;
	sTaxiMap_NEW.strMAP = sSTATION.strMAP;
	sTaxiMap_NEW.vecTaxiStation.push_back ( sSTATION );
	m_vecTaxiMap.push_back ( sTaxiMap_NEW );

	return;
}

HRESULT GLTaxiStation::LoadFile_OLD(const char* szFile, bool bToolMode)
{
	//	Note : 이전 정보 삭제.
	m_vecTaxiMap.clear ();

	std::string strLIST_FILE = m_strPATH + szFile;

	CIniLoader cFILE(",", bToolMode);

	if (GLOGIC::UseLogicZipFile())
		cFILE.SetZipFile(GLogicData::GetInstance().GetGlogicZipFile());

	std::string strSep( ",[]\t" );
	cFILE.reg_sep( strSep );

	if (!cFILE.open(strLIST_FILE, true, GLOGIC::UseLogicPackFile()))
	{
        std::string ErrorMsg(
            sc::string::format(
                "GLTaxiStation::LOAD File Open %1%", szFile));
		sc::writeLogError(ErrorMsg);
		return S_FALSE;
	}

	INT nStationKeySize(0);

	cFILE.getflag( "STATION_LIST_INFO", "StationKeySize", 0, 1, nStationKeySize );
	cFILE.getflag( "STATION_LIST_INFO", "StationBasicCharge", 0, 1, m_dwBasicCharge );
	

	DWORD dwNUM = cFILE.GetKeySize( "STATION_LIST", "STATION" );
	for ( DWORD i=0; i<dwNUM; ++i )
	{
		SNATIVEID sMAPID;
		SNATIVEID sNPCID;
		STAXI_STATION sSTATION;

		cFILE.getflag( i, "STATION_LIST", "STATION", 0, nStationKeySize, sSTATION.bUsed );

		cFILE.getflag( i, "STATION_LIST", "STATION", 1, nStationKeySize, sSTATION.dwLINKID );

		cFILE.getflag( i, "STATION_LIST", "STATION", 2, nStationKeySize, sMAPID.wMainID );
		cFILE.getflag( i, "STATION_LIST", "STATION", 3, nStationKeySize, sMAPID.wSubID );
		sSTATION.dwMAPID = sMAPID.dwID;

		cFILE.getflag( i, "STATION_LIST", "STATION", 4, nStationKeySize, sSTATION.wPosX );
		cFILE.getflag( i, "STATION_LIST", "STATION", 5, nStationKeySize, sSTATION.wPosY );

		cFILE.getflag( i, "STATION_LIST", "STATION", 6, nStationKeySize, sNPCID.wMainID );
		cFILE.getflag( i, "STATION_LIST", "STATION", 7, nStationKeySize, sNPCID.wSubID );
		sSTATION.dwNPCID = sNPCID.dwID;

		cFILE.getflag( i, "STATION_LIST", "STATION", 8, nStationKeySize, sSTATION.dwMapCharge );

		cFILE.getflag( i, "STATION_LIST", "STATION", 9, nStationKeySize, sSTATION.strMAP );
		cFILE.getflag( i, "STATION_LIST", "STATION", 10, nStationKeySize, sSTATION.strSTATION );

		insert( sSTATION );
	}

	return S_OK;
}

HRESULT GLTaxiStation::LoadFile(const std::string& FileName)
{
	CleanUp();
	
	StringCchCopy(m_szFileName, MAX_PATH, FileName.c_str());

	char szFullPath[MAX_PATH] = {0};
	StringCchCopy ( szFullPath, MAX_PATH, GLOGIC::GetPath() );
	StringCchCat ( szFullPath, MAX_PATH, m_szFileName );

	std::auto_ptr<sc::BaseStream> pBStream(
        GLOGIC::openfile_basestream(
            GLOGIC::UseLogicZipFile(), 
            GLogicData::GetInstance().GetGlogicZipFile(),
            szFullPath, 
            FileName,
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

	SFile >> m_dwBasicCharge;
	SFile >> dwNum;

	for (DWORD i=0; i<dwNum; i++)
	{
		STAXI_STATION sSTATION;
		if (!sSTATION.LoadFile(SFile))
            return E_FAIL;		
		insert(sSTATION);
	}
	return S_OK;
}

HRESULT GLTaxiStation::SaveFile ( const char* szFile )
{
	StringCchCopy(m_szFileName,MAX_PATH,szFile);

	char szFullPath[MAX_PATH];
	StringCchCopy ( szFullPath, MAX_PATH, GLOGIC::GetPath() );
	StringCchCat ( szFullPath, MAX_PATH, m_szFileName );

	sc::SerialFile SFile;
	SFile.SetFileType ( _FILEHEAD, VERSION );
	if ( !SFile.OpenFile ( FOT_WRITE, szFullPath ) )	return E_FAIL;
	SFile.SetEncode ( true );

	DWORD dwNum = 0;
	dwNum = StationCount();

	SFile << m_dwBasicCharge;

	SFile << dwNum;

	for ( size_t i = 0; i < m_vecTaxiMap.size(); ++i )
	{
		for ( size_t j = 0; j < m_vecTaxiMap[i].vecTaxiStation.size(); ++j )
		{
			STAXI_STATION& sTaxiStation = m_vecTaxiMap[i].vecTaxiStation[j];
			sTaxiStation.SaveFile( SFile );
		}

	}

	return S_OK;	
}

namespace LOADCSV
{
	class FIND_FUNCTOR
	{
	public:
		FIND_FUNCTOR( const DWORD dwID ) : MAPID(dwID) {}
		bool operator () ( const STAXI_MAP& sValue )
		{
			return sValue.dwMAPID == MAPID;
		}

	private:
		DWORD MAPID;
	};
};

bool GLTaxiStation::SaveCsv ( std::fstream &SFile )
{
	SFile << "Basic Charge" << ",";
	SFile << std::endl;

	SFile << m_dwBasicCharge << ",";
	SFile << std::endl;

	STAXI_STATION::SaveCsvHead ( SFile );

	for ( size_t i = 0; i < m_vecTaxiMap.size(); ++i )
	{
		for ( size_t j = 0; j < m_vecTaxiMap[i].vecTaxiStation.size(); ++j )
		{
			STAXI_STATION& sTaxiStation = m_vecTaxiMap[i].vecTaxiStation[j];
			sTaxiStation.SaveCsv( SFile );
		}
	}

	return true;
}

bool GLTaxiStation::LoadCsv ( std::string& strFileName )
{
	sc::CStringFile StrFile( 10240 );
	if( !StrFile.Open ( strFileName.c_str() ) )
		return false;

	m_vecTaxiMap.clear();

	STRUTIL::ClearSeparator();
	STRUTIL::RegisterSeparator( "," );

	CString      strLine;
	CStringArray StrArray;
	char         szError[ 256 ];

	//	Basic Head
	StrFile.GetNextLine( strLine );
	STRUTIL::StringSeparate( strLine, StrArray );
	int iHead = (int)StrArray.GetCount();

	StrFile.GetNextLine( strLine );
	STRUTIL::StringSeparate( strLine, StrArray );
	int iCount = (int)StrArray.GetCount();

	if ( iCount < iHead )
	{
		StringCchPrintf( szError, 256, "GLTaxiStation::LoadCsv include blank column, next item load failed" );
		MessageBox( NULL, _T(szError), _T("Fail"), MB_OK );
		return false;
	}

	int iCsvCur = 0;

	m_dwBasicCharge = (DWORD)atoi( StrArray[ iCsvCur++ ] );

	StrFile.GetNextLine( strLine );
	STRUTIL::StringSeparate( strLine, StrArray );
	iHead = (int)StrArray.GetCount();

	while( StrFile.GetNextLine( strLine ) )
	{
		STRUTIL::StringSeparate( strLine, StrArray );

		int iCount = (int)StrArray.GetCount();
		if( iCount < iHead )
		{
			StringCchPrintf( szError, 256, "GLTaxiStation::LoadCsv include blank column, next item load failed" );
			MessageBox( NULL, _T(szError), _T("Fail"), MB_OK );
			return false;
		}

		STAXI_STATION sTexistation;
		if ( !sTexistation.LoadCsv(StrArray) )
		{
			StringCchPrintf( szError, 256, "STAXI_STATION::LoadCsv load failed" );
			MessageBox( NULL, _T(szError), _T("Fail"), MB_OK );
			return false;
		}

		TAXI_VECTOR_ITER Finditer = std::find_if(m_vecTaxiMap.begin(), m_vecTaxiMap.end(), LOADCSV::FIND_FUNCTOR(sTexistation.dwMAPID));

		if ( m_vecTaxiMap.end() != Finditer )
		{
			Finditer->vecTaxiStation.push_back(sTexistation);
		}
		else
		{
			STAXI_MAP sTexiMap;

			sTexiMap.dwMAPID = sTexistation.dwMAPID;
			sTexiMap.strMAP  = sTexistation.strMAP;
			sTexiMap.vecTaxiStation.push_back(sTexistation);

			m_vecTaxiMap.push_back(sTexiMap);
		}
	}

	return true;
}

DWORD GLTaxiStation::GetTaxiMapNum ()
{
	return (DWORD)m_vecTaxiMap.size();
}

STAXI_MAP* GLTaxiStation::GetTaxiMap ( int nIndex )
{
	if ( (int)m_vecTaxiMap.size() <= nIndex )	
		return NULL;

	return &m_vecTaxiMap[nIndex];	
}

STAXI_STATION* GLTaxiStation::GetStation ( int nMapIndex, int nStationIndex )
{
	STAXI_MAP* pTaxiMap = GetTaxiMap( nMapIndex );
	
	if ( !pTaxiMap ) return NULL;

	if ( (int)pTaxiMap->vecTaxiStation.size() <= nStationIndex )	
		return NULL;

	if ( !pTaxiMap->vecTaxiStation[nStationIndex].bUsed )	return NULL;

	return &pTaxiMap->vecTaxiStation[nStationIndex];
}

int GLTaxiStation::StationCount()
{
	int nSize = 0;
	for ( size_t i = 0; i < m_vecTaxiMap.size(); ++i )
	{
		nSize += (int)m_vecTaxiMap[i].vecTaxiStation.size();	
	}

	return nSize;
}


