#include "../pch.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../EngineLib/G-Logic/GLogic.h"
#include "../../EngineLib/Common/IniLoader.h"

#include "../GLogicData.h"
#include "../GLogicDataMan.h"
#include "./GLBusStation.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

GLBusStation& GLBusStation::GetInstance ()
{
	static GLBusStation _instance;
	return _instance;
}

GLBusStation::GLBusStation(void)
{
}

GLBusStation::~GLBusStation(void)
{
}

void GLBusStation::insert ( SSTATION &sSTATION )
{
	// 정차역을 등록
	m_vecSTATION.push_back ( sSTATION );
}

bool GLBusStation::LOAD(const std::string& strFILE, bool bToolMode)
{
	// 이전 정보 삭제
	m_vecSTATION.clear();

	std::string strLIST_FILE = m_strPATH + strFILE;

	CIniLoader cFILE(",", bToolMode);

	if (GLOGIC::UseLogicZipFile())
		cFILE.SetZipFile(GLogicData::GetInstance().GetGlogicZipFile());

	std::string strSep( ",[]\t" );
	cFILE.reg_sep( strSep );

	if (!cFILE.open(strLIST_FILE, true, GLOGIC::UseLogicPackFile()))
	{
        std::string ErrorMsg(
            sc::string::format(
                "GLBusStation::LOAD File Open %1%", strFILE));
		sc::writeLogError(ErrorMsg);
		return false;
	}

	INT nStationKeySize(0);

	cFILE.getflag( "STATION_LIST_INFO", "StationKeySize", 0, 1, nStationKeySize );

	DWORD dwNUM = cFILE.GetKeySize( "STATION_LIST", "STATION" );
	for ( DWORD i=0; i<dwNUM; ++i )
	{
		bool bUsed(false);
		cFILE.getflag( i, "STATION_LIST", "STATION", 0, nStationKeySize, bUsed );

		if( !bUsed ) continue;

		SNATIVEID sMAPID;
		SSTATION sSTATION;

		cFILE.getflag( i, "STATION_LIST", "STATION", 1, nStationKeySize, sSTATION.dwLINKID );

		cFILE.getflag( i, "STATION_LIST", "STATION", 2, nStationKeySize, sMAPID.wMainID );
		cFILE.getflag( i, "STATION_LIST", "STATION", 3, nStationKeySize, sMAPID.wSubID );
		sSTATION.dwMAPID = sMAPID.dwID;

		cFILE.getflag( i, "STATION_LIST", "STATION", 4, nStationKeySize, sSTATION.dwGATEID );
		cFILE.getflag( i, "STATION_LIST", "STATION", 5, nStationKeySize, sSTATION.strMAP );
		cFILE.getflag( i, "STATION_LIST", "STATION", 6, nStationKeySize, sSTATION.strSTATION );

		insert( sSTATION );
	}

	return true;
}

SSTATION* GLBusStation::GetStation ( DWORD dwID )
{
	int nSize = (int) m_vecSTATION.size();

	for( int i=0; i<nSize; ++i )
	{
		if( m_vecSTATION[i].dwLINKID == dwID )
		{
			return &m_vecSTATION[i];
		}
	}

	return NULL;
}