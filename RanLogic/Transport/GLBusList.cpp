#include "../pch.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../EngineLib/Common/IniLoader.h"
#include "../../EngineLib/G-Logic/GLogic.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../GLUseFeatures.h"
#include "../GLogicData.h"
#include "../GLogicDataMan.h"
#include "./GLBusList.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

GLBusList::GLBusList(void)
{
}

GLBusList::~GLBusList(void)
{
}

void GLBusList::insert(DWORD dwSTATIONID, SSTATION &sSTATION)
{
	// 정차역을 등록.
	m_vecSTATION.push_back(sSTATION);

	// 기존에 등록된 마을 목록이 있는지 검사.
	DWORD dwSIZE = (DWORD) m_vecVILLAGE.size();
	for (DWORD i=0; i<dwSIZE; ++i)
	{
		SVILLAGE &sVILLAGE = m_vecVILLAGE[i];
		if (sVILLAGE.dwMAPID == sSTATION.dwMAPID)
		{
			// 기존 마을에 등록.
			sVILLAGE.vecSTATION.push_back(dwSTATIONID);
			return;
		}
	}

	// 새로운 마을 등록
	SVILLAGE sVILLAGE_NEW;
	sVILLAGE_NEW.dwMAPID = sSTATION.dwMAPID;
	sVILLAGE_NEW.strMAP = sSTATION.strMAP;
	sVILLAGE_NEW.vecSTATION.push_back ( dwSTATIONID );
	m_vecVILLAGE.push_back ( sVILLAGE_NEW );

	return;
}

bool GLBusList::LOAD(const std::string& strFILE, bool bToolMode)
{
	//	Note : 이전 정보 삭제.
	m_vecVILLAGE.clear ();
	m_vecSTATION.clear ();

	std::string strPATH( GLOGIC::GetPath() );
	std::string strLIST_FILE = strPATH + strFILE;

	CIniLoader cFILE(",", bToolMode);

	if (GLOGIC::UseLogicZipFile())
		cFILE.SetZipFile(GLogicData::GetInstance().GetGlogicZipFile());

	if (!cFILE.open(strLIST_FILE, true, GLOGIC::UseLogicPackFile()))
	{
        std::string ErrorMsg(
            sc::string::format(
                "GLBusList::LOAD File Open %1%", strFILE));
		sc::writeLogError(ErrorMsg);
		return false;
	}

	bool bRandomFlag(false);
	cFILE.getflag( "STATION_INFO", "Random_Flag", 0, 1, bRandomFlag );
	DWORD nRandomCount(0);
	cFILE.getflag( "STATION_INFO", "Random_Count", 0, 1, nRandomCount );

	DWORD nNum = cFILE.GetKeySize( "STATION_INFO", "Statin_Info" );

	if( nRandomCount > nNum )
	{
		GASSERT( 0 && "GLBusList::LOAD, Random_Count가 Station 수보다 많습니다." );
		return false;
	}

	std::vector<SSTATION> vecStationAll;

	for( DWORD i=0; i<nNum; ++i )
	{
		SNATIVEID sMAPID;
		SSTATION sSTATION;

		cFILE.getflag( i, "STATION_INFO", "Statin_Info", 0, 6, sMAPID.wMainID );
		cFILE.getflag( i, "STATION_INFO", "Statin_Info", 1, 6, sMAPID.wSubID );
		sSTATION.dwMAPID = sMAPID.dwID;

		cFILE.getflag( i, "STATION_INFO", "Statin_Info", 2, 6, sSTATION.dwProbability );
		cFILE.getflag( i, "STATION_INFO", "Statin_Info", 3, 6, sSTATION.dwLINKID );

		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() == TRUE)
		{
			CString strKEY;
			strKEY.Format("STATION_INFO_MAP_%d_%d_%d_%d", sMAPID.wMainID, sMAPID.wSubID, sSTATION.dwProbability, sSTATION.dwLINKID);
			sSTATION.strMAP = CGameTextMan::GetInstance().GetGameInText(strKEY.GetString());
			const INT iReplacePos = strKEY.Find("MAP");
			strKEY.Delete(iReplacePos, strlen("MAP"));
			strKEY.Insert(iReplacePos, "GATE");
			sSTATION.strSTATION = CGameTextMan::GetInstance().GetGameInText(strKEY.GetString());
		}
		else
		{
			cFILE.getflag( i, "STATION_INFO", "Statin_Info", 4, 6, sSTATION.strMAP );
			cFILE.getflag( i, "STATION_INFO", "Statin_Info", 5, 6, sSTATION.strSTATION );
		}

		vecStationAll.push_back( sSTATION );
	}

	if( vecStationAll.empty() ) return false;

	if( bRandomFlag )
	{
		std::vector<SSTATION> vecStationRand;

		//srand( (unsigned)time( NULL ) );

		std::vector<SSTATION>::iterator iter = vecStationAll.begin();

		while(1)
		{
			if( vecStationRand.size() >= nRandomCount ) break;

			SSTATION & sSTATION = (*iter);

			DWORD nRand = ( rand() % 10 ) + 1; // 1 ~ 10
			if( nRand <= sSTATION.dwProbability )
			{
				vecStationRand.push_back( sSTATION );
				iter = vecStationAll.erase( iter );
			}
			else ++iter;

			if( iter == vecStationAll.end() )
			{
				iter = vecStationAll.begin();
			}
		}

		DWORD nSize = static_cast<DWORD> ( vecStationRand.size() );
		for( DWORD i=0; i<nSize; ++i )
		{
			insert( i, vecStationRand[i] );
		}
	}
	else
	{
		DWORD nSize = static_cast<DWORD> ( vecStationAll.size() );
		for( DWORD i=0; i<nSize; ++i )
		{
			insert( i, vecStationAll[i] );
		}
	}

	return true;
}

DWORD GLBusList::GetVillageNum ()
{
	return (DWORD)m_vecVILLAGE.size();
}

SVILLAGE* GLBusList::GetVillage ( DWORD dwID )
{
	if ( m_vecVILLAGE.size() <= dwID )	
		return NULL;

	return &m_vecVILLAGE[dwID];
}

SSTATION* GLBusList::GetStation ( DWORD dwID )
{
	if ( m_vecSTATION.size() <= dwID )	
		return NULL;

	return &m_vecSTATION[dwID];
}