#include "../pch.h"

#include "../../SigmaCore/String/StringUtils.h"
#include "../../SigmaCore/String/basestring.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/Log/LogMan.h"

#include "../../EngineLib/Common/IniLoader.h"
#include "../../EngineLib/G-Logic/GLogic.h"

#include "../../RanLogic/Land/MapAxisInfoMan.h"

#include "../Level/GLLevelFileServer.h"
#include "./GLMapListServer.h"

#include "../../SigmaCore/DebugInclude.h"

GLMapListServer::GLMapListServer()
{
}

GLMapListServer::~GLMapListServer()
{
}

HRESULT GLMapListServer::LoadMapList ( BOOL bTool, LPF_SERVER_INFO pServerInfo, int nServerNum, const bool bEmulator )
{
    HRESULT hr = GLMapList::LoadMapList( bTool, pServerInfo, nServerNum, bEmulator );
    if ( FAILED(hr) ) 
        return E_FAIL;

    if ( nServerNum )
    {
        FIELDMAP_ITER iter = m_MapList.begin();
        for ( ; iter!=m_MapList.end(); ++iter )
        {
            SMAPNODE& NodeData = iter->second;
            if( !bTool && !NodeData.m_bUse )
			    continue;
        
            GLLevelFileServer cLevelFile;

            //
            //mjeon
            //Load LevelFile.

            BOOL bOK = cLevelFile.LoadFile(NodeData.m_LevelFile.c_str(), NULL, m_pAxisMan);
            if ( !bOK )
            {            
                sc::writeLogError(
                    sc::string::format(
                        "cLevelFile.LoadFile(), %1% fail", NodeData.m_LevelFile));
	            NodeData.m_bUse = false;

	            //return S_OK;
            }

            *NodeData.m_pLevelRequire = *cLevelFile.GetLevelRequire();
            *NodeData.m_pLevelEtcFunc = *cLevelFile.GetLevelEtcFunc();
        }
    }
    sc::writeLogInfo(sc::string::format("Map List count %1%", m_MapList.size()));
    sc::writeLogInfo(sc::string::format("Map Axis count %1%", m_pAxisMan->Size()));

	return S_OK;
}

const char* GLMapListServer::GetMapName( const SNATIVEID& sNativeID )
{
    if (!GLMapList::GetMapName(sNativeID))
    {
        static CString strEmpty; // MEMO : 에러시 반환되는 빈 스트링
        return strEmpty;
    }
    return ID2GAMEINTEXT( GLMapList::GetMapName(sNativeID) );
}

std::string GLMapListServer::GetMapNameStr( const SNATIVEID& MapID ) const
{

    return ID2GAMEINTEXT( GLMapList::GetMapNameStr(MapID).c_str() );
}

/*
HRESULT GLMapListServer::LoadMapList_OLD(
    const TCHAR* szMapListFile,
    bool bToolMode,
    LPF_SERVER_INFO pServerInfo,
    int nServerNum)
{
	GASSERT(szMapListFile);

	m_MapList.clear ();

	std::string strPath(GLOGIC::GetPath());
	strPath += szMapListFile;

	CIniLoader cFILE(",", bToolMode);

	if (GLOGIC::UseLogicZipFile())
		cFILE.SetZipFile(GLogicData::GetInstance().GetGlogicZipFile());

	std::string strSep( ",[]\t" );
	cFILE.reg_sep( strSep );

	if (!cFILE.open(strPath, true, GLOGIC::UseLogicPackFile()))
	{
		sc::writeLogError(
            sc::string::format(
                "GLMapList::LoadMapList File Open %1%", szMapListFile));
		return false;
	}

	INT nMapKeySize(0);

	cFILE.getflag( "MAP_LIST_INFO", "MapKeySize", 0, 1, nMapKeySize );

	DWORD dwNUM = cFILE.GetKeySize( "MAP_LIST", "MAP" );
	for ( DWORD i=0; i<dwNUM; ++i )
	{
		bool bUsed(false);
		cFILE.getflag( i, "MAP_LIST", "MAP", 0, nMapKeySize, bUsed );

		if (!bToolMode && !bUsed)
            continue;
		SMAPNODE MapNode;
		MapNode.m_bUse = bUsed;

		cFILE.getflag( i, "MAP_LIST", "MAP", 1, nMapKeySize, MapNode.m_LevelFile );
		cFILE.getflag( i, "MAP_LIST", "MAP", 2, nMapKeySize, MapNode.m_NativeID.wMainID );
		cFILE.getflag( i, "MAP_LIST", "MAP", 3, nMapKeySize, MapNode.m_NativeID.wSubID );

		if ( !( MapNode.m_NativeID.wMainID<MAXLANDMID || MapNode.m_NativeID.wSubID!=0 ) )
		{
			CString strTemp;
			strTemp.Format( "Map ID GuideLine  is 0~%d / 0~%d.", MAXLANDMID, MAXLANDSID );
			MessageBox( NULL, strTemp.GetString(), "ERROR", MB_OK );
			return E_FAIL;
		}

		cFILE.getflag( i, "MAP_LIST", "MAP", 4, nMapKeySize, MapNode.m_FieldServerID );

		if ( pServerInfo && nServerNum )
		{
			if ( MapNode.m_FieldServerID >= (DWORD)nServerNum )
			{
				MessageBox ( NULL, "FieldServer ID of Map is Overflow.", "ERROR", MB_OK );
			}

			if ( !pServerInfo[0*MAX_CHANNEL_NUMBER+MapNode.m_FieldServerID].IsValid() )
			{
				MessageBox ( NULL, "Not valid FieldServer ID.", "ERROR", MB_OK );
			}
		}

		cFILE.getflag( i, "MAP_LIST", "MAP", 5, nMapKeySize, MapNode.m_bPeaceZone );
		
		bool bCommissionFree(false);
		cFILE.getflag( i, "MAP_LIST", "MAP", 6, nMapKeySize, bCommissionFree );
		MapNode.m_bCommission = !bCommissionFree;

		cFILE.getflag( i, "MAP_LIST", "MAP", 7, nMapKeySize, MapNode.m_bPKZone );
		cFILE.getflag( i, "MAP_LIST", "MAP", 8, nMapKeySize, MapNode.m_bFreePK );
		cFILE.getflag( i, "MAP_LIST", "MAP", 9, nMapKeySize, MapNode.m_bItemDrop );
		cFILE.getflag( i, "MAP_LIST", "MAP", 10, nMapKeySize, MapNode.m_bFriendMove );
		cFILE.getflag( i, "MAP_LIST", "MAP", 11, nMapKeySize, MapNode.m_bRestart );
		cFILE.getflag( i, "MAP_LIST", "MAP", 12, nMapKeySize, MapNode.m_bPetActivity );
		cFILE.getflag( i, "MAP_LIST", "MAP", 13, nMapKeySize, MapNode.m_bDecExp );
		cFILE.getflag( i, "MAP_LIST", "MAP", 14, nMapKeySize, MapNode.m_bVehicleActivity );
		cFILE.getflag( i, "MAP_LIST", "MAP", 15, nMapKeySize, MapNode.m_bClubBattleZone );				

		cFILE.getflag( i, "MAP_LIST", "MAP", 16, nMapKeySize, MapNode.m_MapName );
		if ( MapNode.m_MapName.length() > MAP_NAME_MAX )
		{
			MessageBox ( NULL, "Map Name Too Long.", MapNode.m_MapName.c_str(), MB_OK );
		}

		cFILE.getflag( i, "MAP_LIST", "MAP", 17, nMapKeySize, MapNode.m_BgmName );
		cFILE.getflag( i, "MAP_LIST", "MAP", 18, nMapKeySize, MapNode.m_LoadingImageName );

		cFILE.getflag( i, "MAP_LIST", "MAP", 19, nMapKeySize, MapNode.m_bInstantMap );
		 
		cFILE.getflag( i, "MAP_LIST", "MAP", 20, nMapKeySize, MapNode.m_bQBoxDrop );

		//cFILE.getflag( i, "MAP_LIST", "MAP", 21, nMapKeySize, MapNode.bTutorialMap ); // by luxes.

		if ( !bToolMode && IsRegMapNode(MapNode) )
		{
			MessageBox ( NULL, "'GLGaeaClient::LoadMapList' Map Set Duplicate.", "ERROR", MB_OK );
			return E_FAIL;
		}

		if ( nServerNum )
		{
			GLLevelFileServer cLevelFile;
			BOOL bOK = cLevelFile.LoadFile ( MapNode.m_LevelFile.c_str(), NULL, m_pAxisMan );
			if ( !bOK )
			{
                sc::writeLogError(
                    sc::string::format("cLevelFile.LoadFile(), %1% fail", MapNode.m_LevelFile));
				continue;
			}
			*MapNode.m_pLevelRequire = *cLevelFile.GetLevelRequire();
			*MapNode.m_pLevelEtcFunc = *cLevelFile.GetLevelEtcFunc();
		}

		
		FIELDMAP_ITER iter = m_MapList.find ( MapNode.m_NativeID.dwID );
		if( iter != m_MapList.end() )
		{

			CString strTemp;
			strTemp.Format( "Same Map ID is %d, %d ", MapNode.m_NativeID.wMainID, MapNode.m_NativeID.wSubID );
			MessageBox( NULL, strTemp.GetString(), "ERROR", MB_OK );
            
			return E_FAIL;
		}

		m_MapList.insert ( std::make_pair(MapNode.m_NativeID.dwID,MapNode) );
	}
	
	return S_OK;
}
*/