#include "../pch.h"
#include <boost/lexical_cast.hpp>

#include "../../SigmaCore/File/TxtFile.h"
#include "../../SigmaCore/Lua/MinLua.h"
#include "../../SigmaCore/String/StringUtils.h"
#include "../../SigmaCore/String/basestring.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringFormatW.h"
#include "../../SigmaCore/Util/SystemInfo.h"
#include "../../SigmaCore/Util/MinMessageBox.h"

#include "../../EngineLib/Common/IniLoader.h"
#include "../../EngineLib/G-Logic/GLogic.h"

#include "../GLogicDataMan.h"
#include "../GLLevelFile.h"
#include "./MapAxisInfoMan.h"
#include "./GLMapList.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const char* GLMapList::_FILEHEAD = "GLMAPS_LIST";

//------------------------------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------------------------------//

GLMapList::GLMapList(void)
{
    m_pAxisMan = new MapAxisInfoMan;
}

GLMapList::~GLMapList(void)
{
    delete m_pAxisMan;
    m_pAxisMan = NULL;
}

BOOL GLMapList::IsRegMapNode(const SMAPNODE& MapCur) const
{
	FIELDMAP_CITER iter = m_MapList.begin();
	FIELDMAP_CITER iter_end = m_MapList.end();
	for ( ; iter!=iter_end; ++iter)
	{
		const SMAPNODE& sMapNode = (*iter).second;
		if (sMapNode.GetId() == MapCur.GetId())
            return TRUE;
		if (MapCur.m_LevelFile == sMapNode.m_LevelFile)
            return TRUE;
	}
	return FALSE;
}

HRESULT GLMapList::LoadMapList(BOOL bTool, LPF_SERVER_INFO pServerInfo, int nServerNum, const bool bEmulator)
{
    m_MapList.clear();
	std::string strMapsListFile;
	std::string strMapsListCompileFile;

	if ( false == GLUseFeatures::GetInstance().IsUsingWorldBattle() )
	{
		strMapsListFile = GLogicData::GetInstance().GetMapListFileName();
		strMapsListCompileFile = GLogicData::GetInstance().GetMapListCompileFileName();
	}
	else
	{
		strMapsListFile = GLogicData::GetInstance().GetMapListWBFileName();
		strMapsListCompileFile = GLogicData::GetInstance().GetMapListWBCompileFileName();
	}

	if (LoadLua(strMapsListCompileFile, bTool, pServerInfo, nServerNum, bEmulator))
	{
		std::string strMsg(sc::string::format("Mapslist Load Lua-%1%.", strMapsListCompileFile));
		sc::writeLogInfo(strMsg);
	}
	else if (LoadLua(strMapsListFile, bTool, pServerInfo, nServerNum, bEmulator))
	{
		std::string strMsg(sc::string::format("Mapslist Load Lua-%1%.", strMapsListFile));
		sc::writeLogInfo(strMsg);
	}
	else
	{
		std::string strMsg(sc::string::format("%1% file open failed.", strMapsListCompileFile));
		sc::ErrorMessageBox(strMsg, false);
		sc::writeLogError(strMsg);
		return E_FAIL;
	}

    // Axis info load
    for (FIELDMAP_CITER iter=m_MapList.begin(); iter!=m_MapList.end(); ++iter)
    {
        const SMAPNODE& MapNode = iter->second;
        m_pAxisMan->Load(MapNode.m_LevelFile);
    }

	return S_OK;
}

namespace SAVECSV
{
	typedef std::pair<DWORD,const SMAPNODE*> PAIR_FIELDMAP;
	typedef std::vector<PAIR_FIELDMAP>       VEC_SORT;
	typedef VEC_SORT::iterator               VEC_SORT_ITER;
	static VEC_SORT                          SortedVec;

	bool VEC_SORT_FUNCTOR ( const PAIR_FIELDMAP& lvalue, const PAIR_FIELDMAP& rvalue )
	{
		return lvalue.first < rvalue.first;
	}
};

void GLMapList::SaveCsvHead ( std::fstream &SFile )
{
	SFile<< "MID"	<< ",";
	SFile<< "SID"	<< ",";

	SFile<< "MapName"				<< ",";
	SFile<< "Used"					<< ",";
	SFile<< "FileName"				<< ",";
	SFile<< "FieldServer"			<< ",";
	SFile<< "Bgm"					<< ",";
	SFile<< "LoadingImg"			<< ",";
	SFile<< "Piece"					<< ",";
	SFile<< "CommissionFree"		<< ",";
	SFile<< "PKEnable"				<< ",";
	SFile<< "FreePK"				<< ",";
	SFile<< "ItemDrop"				<< ",";
	SFile<< "FriendCard"			<< ",";
	SFile<< "Resurrection Forbid"	<< ",";
	SFile<< "PatActivity"			<< ",";
	SFile<< "ExpDrop"				<< ",";
	SFile<< "VehicleActivity"		<< ",";
	SFile<< "ClubBattleZone"		<< ",";
	SFile<< "InstantMap"			<< ",";
	SFile<< "QBoxDrop"				<< ",";
	SFile<< "LunchBox Forbid"		<< ",";
	SFile<< "CPRest"				<< ",";
	SFile<< "PK Map"				<< ",";
	SFile<< "UIMapSelect"			<< ",";
	SFile<< "UIMapInfo"				<< ",";
	SFile<< "PartyMap"				<< ",";
	SFile<< "ExpeditionMap"			<< ",";
    SFile<< "ClubPkRecord"      	<< ",";
    SFile<< "PrivateMarketOpenable" << ",";
    SFile<< "Sparring"				<< ",";
	SFile<< "CullByObjectMap"		<< ",";
	SFile<< "DropBlockItem"			<< ",";	
	SFile<< "ChangeSchool"			<< ",";

	for (int i = 0; i < PROHIBIT_ITEM_SIZE; ++i)
	{
		// 아래의 COMMENT는 데이터에서 읽어오지않는다;
		SFile << COMMENT::ITEMPROHIBIT[i].c_str() << ",";
	}

	SFile<< "RebirthBlock"			<< ",";
	SFile<< "EnableSwitchMob"		<< ",";
	SFile<< "RateExp"					<< ",";
	SFile<< "RateItemDrop"			<< ",";

	SFile<< "Macro"					<< ",";
	SFile<< "Prison"				<< ",";
	SFile<< "ChangeCharType"		<< ",";

	SFile << std::endl;
}

bool GLMapList::SaveCsv ( std::fstream &SFile )
{
	using namespace SAVECSV;

	FIELDMAP_ITER iter     = m_MapList.begin();
	FIELDMAP_ITER iter_end = m_MapList.end();

	// Note : Vector에 넘긴뒤 정렬을 한다.
	SortedVec.clear();
	//SortedVec._Mylast = SortedVec._Myfirst;
	for ( ; iter != iter_end; ++iter ) { SortedVec.push_back( PAIR_FIELDMAP((*iter).first, &(*iter).second) ); }
	std::sort(SortedVec.begin(), SortedVec.end(), VEC_SORT_FUNCTOR);

	// Note : 세이브 시작
	GLMapList::SaveCsvHead ( SFile );

	VEC_SORT_ITER veciter     = SortedVec.begin();
	VEC_SORT_ITER veciter_end = SortedVec.end();

	for ( ; veciter != veciter_end; ++veciter )
	{		
		const SMAPNODE* pMapNode = (*veciter).second;
		if ( !pMapNode ) continue;

		SFile << pMapNode->m_NativeID.wMainID << ",";
		SFile << pMapNode->m_NativeID.wSubID  << ",";

		STRUTIL::OutputStrCsv( SFile, std::string(pMapNode->m_MapID) );

		SFile << ((INT)pMapNode->m_bUse) << ",";

		STRUTIL::OutputStrCsv( SFile, std::string(pMapNode->m_LevelFile) );

		SFile << pMapNode->m_FieldServerID << ",";

		STRUTIL::OutputStrCsv( SFile, std::string(pMapNode->m_BgmName) );

		STRUTIL::OutputStrCsv( SFile, std::string(pMapNode->m_LoadingImageName) );
		
		SFile << ((INT)pMapNode->m_bPeaceZone)				<< ",";
		SFile << ((INT)!pMapNode->m_bCommission)			<< ",";
		SFile << ((INT)pMapNode->m_bPKZone)					<< ",";
		SFile << ((INT)pMapNode->m_bFreePK)					<< ",";
		SFile << ((INT)pMapNode->m_bItemDrop)				<< ",";
		SFile << ((INT)pMapNode->m_bFriendMove)				<< ",";
		SFile << ((INT)pMapNode->m_bRestart)				<< ",";
		SFile << ((INT)pMapNode->m_bPetActivity)			<< ",";
		SFile << ((INT)pMapNode->m_bDecExp)					<< ",";
		SFile << ((INT)pMapNode->m_bVehicleActivity)		<< ",";
		SFile << ((INT)pMapNode->m_bClubBattleZone)			<< ",";
		SFile << ((INT)pMapNode->m_emInstanceType)			<< ",";
		SFile << ((INT)pMapNode->m_bQBoxDrop)				<< ",";
		SFile << ((INT)pMapNode->m_bLunchBoxForbid)			<< ",";
		SFile << ((INT)pMapNode->m_bCPReset)				<< ",";
		SFile << ((INT)pMapNode->m_bPKMap)					<< ",";
		SFile << ((INT)pMapNode->m_bUIMapSelect)			<< ",";
		SFile << ((INT)pMapNode->m_bUIMapInfo)				<< ",";
		SFile << ((INT)pMapNode->m_bPartyMap)				<< ",";
		SFile << ((INT)pMapNode->m_bExpeditionMap)			<< ",";
		SFile << ((INT)pMapNode->m_bClubPkRecord)			<< ",";
        SFile << ((INT)pMapNode->m_bPrivateMarketOpenable)	<< ",";
        SFile << ((INT)pMapNode->m_bSparring)				<< ",";
		SFile << ((INT)pMapNode->m_bCullByObjectMap)		<< ",";
		SFile << ((INT)pMapNode->m_bDropBlockItem)		<< ",";		
		SFile << ((INT)pMapNode->m_bChangeSchool)			<< ",";

		for (int i = 0; i < PROHIBIT_ITEM_SIZE; ++i)
		{
			SFile << ( (INT)(pMapNode->IsBlockDrugItem( (SMAPNODE::ItemTypeValue)PROHIBIT_ITEM_LIST[i]) ) )	<< ",";
		}

		SFile << ((INT)pMapNode->m_bRebirthBlock)			<< ",";
		SFile << ((INT)pMapNode->m_bDisableSwitchMob)	<< ",";
		SFile << (pMapNode->m_fRateExpDrop)				<< ",";
		SFile << (pMapNode->m_fRateItemDrop)				<< ",";
		SFile << ((INT)pMapNode->m_bMacro)				<< ",";
		SFile << ((INT)pMapNode->m_bPrison)				<< ",";

		SFile << ((INT)pMapNode->m_bChangeCharSlot)		<< ",";

		SFile << std::endl;
	}

	return true;
}

bool GLMapList::LoadCsv ( std::string& strFileName )
{
	sc::CStringFile StrFile( 10240 );
	if( !StrFile.Open ( strFileName.c_str() ) )
		return false;

	m_MapList.clear();

	STRUTIL::ClearSeparator();
	STRUTIL::RegisterSeparator( "," );

	CString      strLine;
	CStringArray StrArray;

	//	Basic Head
	StrFile.GetNextLine( strLine );
	STRUTIL::StringSeparate( strLine, StrArray );
	int iHead = (int)StrArray.GetCount();

	std::string strTemp;
	char szError[ 256 ];

	while( StrFile.GetNextLine( strLine ) )
	{
		int iCsvCur = 0;

		STRUTIL::StringSeparate( strLine, StrArray );

		int iCount = (int)StrArray.GetCount();
		if( iCount < iHead )
		{
			StringCchPrintf( szError, 256, "GLMapList::LoadCsv include blank column, next item load failed" );
			MessageBox( NULL, _T(szError), _T("Fail"), MB_OK );
			return false;
		}

		SMAPNODE sMapNode;

		sMapNode.m_NativeID.wMainID = (WORD)atoi( StrArray[ iCsvCur++ ] );
		sMapNode.m_NativeID.wSubID  = (WORD)atoi( StrArray[ iCsvCur++ ] );

		STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strTemp );
		sMapNode.m_MapID = strTemp;

		sMapNode.m_bUse = (0!=atoi( StrArray[ iCsvCur++ ] ));

		STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strTemp );
		sMapNode.m_LevelFile = strTemp;

		sMapNode.m_FieldServerID = (DWORD)atoi( StrArray[ iCsvCur++ ] );

		STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strTemp );
		sMapNode.m_BgmName = strTemp;

		STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strTemp );
		sMapNode.m_LoadingImageName = strTemp;

		sMapNode.m_bPeaceZone			= (0!=atoi( StrArray[ iCsvCur++ ] ));
		// 헷갈리지 말자 아래에 반대로 대입한다;
		sMapNode.m_bCommission			= (0!=atoi( StrArray[ iCsvCur++ ] ));
		sMapNode.m_bPKZone				= (0!=atoi( StrArray[ iCsvCur++ ] ));
		sMapNode.m_bFreePK				= (0!=atoi( StrArray[ iCsvCur++ ] ));
		sMapNode.m_bItemDrop			= (0!=atoi( StrArray[ iCsvCur++ ] ));
		sMapNode.m_bFriendMove			= (0!=atoi( StrArray[ iCsvCur++ ] ));
		sMapNode.m_bRestart				= (0!=atoi( StrArray[ iCsvCur++ ] ));
		sMapNode.m_bPetActivity			= (0!=atoi( StrArray[ iCsvCur++ ] ));
		sMapNode.m_bDecExp				= (0!=atoi( StrArray[ iCsvCur++ ] ));
		sMapNode.m_bVehicleActivity		= (0!=atoi( StrArray[ iCsvCur++ ] ));
		sMapNode.m_bClubBattleZone		= (0!=atoi( StrArray[ iCsvCur++ ] ));
		sMapNode.m_emInstanceType	= SMAPNODE::EMINSTANCE_TYPE(atoi( StrArray[ iCsvCur++ ] ));
		sMapNode.m_bQBoxDrop			= (0!=atoi( StrArray[ iCsvCur++ ] ));
		sMapNode.m_bLunchBoxForbid		= (0!=atoi( StrArray[ iCsvCur++ ] ));
		sMapNode.m_bCPReset				= (0!=atoi( StrArray[ iCsvCur++ ] ));
		sMapNode.m_bPKMap				= (0!=atoi( StrArray[ iCsvCur++ ] ));
		sMapNode.m_bUIMapSelect			= (0!=atoi( StrArray[ iCsvCur++ ] ));
		sMapNode.m_bUIMapInfo			= (0!=atoi( StrArray[ iCsvCur++ ] ));
		sMapNode.m_bPartyMap			= (0!=atoi( StrArray[ iCsvCur++ ] ));
		sMapNode.m_bExpeditionMap		= (0!=atoi( StrArray[ iCsvCur++ ] ));
		sMapNode.m_bClubPkRecord		= (0!=atoi( StrArray[ iCsvCur++ ] ));
        sMapNode.m_bPrivateMarketOpenable	= (0!=atoi( StrArray[ iCsvCur++ ] ));
        sMapNode.m_bSparring				= (0!=atoi( StrArray[ iCsvCur++ ] ));
		sMapNode.m_bCullByObjectMap	= (0!=atoi( StrArray[ iCsvCur++ ] ));
		sMapNode.m_bDropBlockItem		= (0!=atoi( StrArray[ iCsvCur++ ] ));
		sMapNode.m_bChangeSchool		= (0!=atoi( StrArray[ iCsvCur++ ] ));

		if (iCount > iCsvCur)
		{
			for (int i = 0; i < PROHIBIT_ITEM_SIZE; ++i)
			{
				if ( 0 != atoi(StrArray[iCsvCur++]) )
				{
					sMapNode.m_prohibiteditems.push_back(SMAPNODE::ItemTypeValue(PROHIBIT_ITEM_LIST[i]));
				}
			}
		}

		sMapNode.m_bCommission = !sMapNode.m_bCommission;
		
		sMapNode.m_bRebirthBlock= (0!=atoi( StrArray[ iCsvCur++ ] ));
		sMapNode.m_bDisableSwitchMob = (0!=atoi( StrArray[iCsvCur++ ]));
		sMapNode.m_fRateExpDrop = static_cast<float>(atof(StrArray[ iCsvCur++ ]));
		sMapNode.m_fRateItemDrop = static_cast<float>(atof(StrArray[ iCsvCur++ ]));
		sMapNode.m_bMacro = (0!=atoi( StrArray[iCsvCur++ ]));
		sMapNode.m_bPrison = (0!=atoi( StrArray[iCsvCur++ ]));
		sMapNode.m_bChangeCharSlot = (0!=atoi( StrArray[iCsvCur++]));

		FIELDMAP_ITER iterfind = m_MapList.find(sMapNode.m_NativeID.dwID);
		if ( m_MapList.end() != iterfind )
		{
			iterfind->second = sMapNode;
		}
		else
		{
			m_MapList.insert(std::make_pair(sMapNode.m_NativeID.dwID, sMapNode));
		}
	}

	return true;
}

SMAPNODE* GLMapList::FindMapNode(const SNATIVEID& sNativeID)
{
	SNATIVEID sFindID;
	sFindID.wMainID = sNativeID.wMainID;
	sFindID.wSubID  = 0;
	FIELDMAP_ITER iter = m_MapList.find(sFindID.dwID);
	
	if (iter != m_MapList.end())
		return &(*iter).second;
    else
	    return NULL;
}

const SMAPNODE* GLMapList::FindMapNode(const SNATIVEID& sNativeID) const
{
    SNATIVEID sFindID;
    sFindID.wMainID = sNativeID.wMainID;
    sFindID.wSubID  = 0;
    FIELDMAP_CITER iter = m_MapList.find(sFindID.dwID);
    if (iter != m_MapList.end())
        return &(*iter).second;
    else
        return NULL;
}

const char* GLMapList::GetMapName(const SNATIVEID& sNativeID)
{
	SMAPNODE* pMap = FindMapNode(sNativeID);
	if (pMap)
        return pMap->m_MapID.c_str();
    else
		return NULL;
}

std::string GLMapList::GetMapNameStr(const SNATIVEID& MapID) const
{
    const SMAPNODE* pMap = FindMapNode(MapID);
    if (pMap)
        return pMap->m_MapID;
    else
        return std::string("MAP_DEFAULT");
}

const SLEVEL_REQUIRE* GLMapList::GetLevelRequire(const SNATIVEID& sNativeID)
{
	SMAPNODE* pMap = FindMapNode(sNativeID);
	if (pMap)
        return pMap->m_pLevelRequire;
    else
		return NULL;
}

const SLEVEL_ETC_FUNC* GLMapList::GetLevelEtcFunc ( const SNATIVEID &sNativeID )
{
	SMAPNODE* pMap = FindMapNode(sNativeID);
	if (pMap)
        return pMap->m_pLevelEtcFunc;
    else
		return NULL;
}

HRESULT GLMapList::CleanUpMapList()
{
	m_MapList.clear();
    m_pAxisMan->Clear();
	return S_OK;
}

void GLMapList::ConvertWorldToMap(const SNATIVEID& baseMapID, const D3DXVECTOR3& vPos, int& nPosX, int& nPosY)
{
    const SMAPNODE* pMapNode = FindMapNode(baseMapID);
    if (!pMapNode)
        return;
    std::tr1::shared_ptr<GLMapAxisInfo> spAxisInfo = m_pAxisMan->GetInfo(pMapNode->m_LevelFile);
    if (spAxisInfo)
        spAxisInfo->Convert2MapPos(vPos.x, vPos.z, nPosX, nPosY);
}


void GLMapList::ConvertMapToWorld(const SNATIVEID& baseMapID, int PosX, int PosY, D3DXVECTOR3& vPos)
{
    const SMAPNODE* pMapNode = FindMapNode(baseMapID);
    if (!pMapNode)
        return;
    std::tr1::shared_ptr<GLMapAxisInfo> spAxisInfo = m_pAxisMan->GetInfo(pMapNode->m_LevelFile);
    if (spAxisInfo)
        spAxisInfo->MapPos2MiniPos(PosX, PosY, vPos.x, vPos.z);
}

std::tr1::shared_ptr<GLMapAxisInfo> GLMapList::GetAxisInfo(const std::string& FileName)
{
    return m_pAxisMan->GetInfo(FileName);
}

bool GLMapList::LoadLuaCurr(const std::string& FileName, BOOL bTool, F_SERVER_INFO* pServerInfo, int nServerNum, const bool bEmulator)
{
	try
	{		
		LuaPlus::LuaObject MapObj = sc::lua_getGlobalFromName("maplist");
		for ( LuaPlus::LuaTableIterator Iter(MapObj); Iter; Iter.Next() )
		{
			SMAPNODE NodeData;
			LuaPlus::LuaObject _object = Iter.GetValue();
			_GetIntager( _object, "wMID"				, (INT&)NodeData.m_NativeID.wMainID);
			_GetIntager( _object, "wSID"				, (INT&)NodeData.m_NativeID.wSubID);
			
			NodeData.m_MapID = sc::string::format( "MAP_%d_%d", NodeData.m_NativeID.wMainID, NodeData.m_NativeID.wSubID );

			_GetIntager( _object, "wFieldServerID"		, (INT&)NodeData.m_FieldServerID);
			_GetWString( _object, "strBGMName"			, NodeData.m_BgmName);
			_GetWString( _object, "strLodingImag"		, NodeData.m_LoadingImageName);
			_GetWString( _object, "strLevelFileName"	, NodeData.m_LevelFile);
			_GetBoolean( _object, "bUse"				, NodeData.m_bUse);
			_GetBoolean( _object, "bPeaceZone"			, NodeData.m_bPeaceZone);
			_GetBoolean( _object, "bCommission"			, NodeData.m_bCommission);
			_GetBoolean( _object, "bPKZone"				, NodeData.m_bPKZone);
			_GetBoolean( _object, "bFreePK"				, NodeData.m_bFreePK);
			_GetBoolean( _object, "bItemDrop"			, NodeData.m_bItemDrop);
			_GetBoolean( _object, "bFriendMove"			, NodeData.m_bFriendMove);
			_GetBoolean( _object, "bRestart"			, NodeData.m_bRestart);
			_GetBoolean( _object, "bPetActivity"		, NodeData.m_bPetActivity);
			_GetBoolean( _object, "bDecExp"				, NodeData.m_bDecExp);
			_GetBoolean( _object, "bVehicleActivity"	, NodeData.m_bVehicleActivity);
			_GetBoolean( _object, "bClubBattleZone"		, NodeData.m_bClubBattleZone );

			_GetBoolean( _object, "nInstantMap", (bool&)NodeData.m_emInstanceType );
			_GetIntager( _object, "nInstantMap", (INT&)NodeData.m_emInstanceType );
			
			_GetBoolean( _object, "bQBoxDrop"				, NodeData.m_bQBoxDrop);
			_GetBoolean( _object, "bLunchBoxForbid"			, NodeData.m_bLunchBoxForbid);
			_GetBoolean( _object, "bCPReset"				, NodeData.m_bCPReset);
			_GetBoolean( _object, "bPKMap"					, NodeData.m_bPKMap);
			_GetBoolean( _object, "bUIMapSelect"			, NodeData.m_bUIMapSelect);
			_GetBoolean( _object, "bUIMapInfo"				, NodeData.m_bUIMapInfo);
			_GetBoolean( _object, "bPartyMap"				, NodeData.m_bPartyMap);
			_GetBoolean( _object, "bExpeditionMap"			, NodeData.m_bExpeditionMap);
			_GetBoolean( _object, "bClubPKRecord"			, NodeData.m_bClubPkRecord);
			_GetBoolean( _object, "bPrivateMarketOpenable"	, NodeData.m_bPrivateMarketOpenable);
			_GetBoolean( _object, "bSparring"				, NodeData.m_bSparring);
			_GetBoolean( _object, "bCullByObjectMap"		, NodeData.m_bCullByObjectMap);
			_GetBoolean( _object, "bDropBlockItem"			, NodeData.m_bDropBlockItem);
			NodeData.m_bCommission = !NodeData.m_bCommission;
			INT i;
			LuaPlus::LuaObject luaObj = _object.GetByName( "ProhidibtedItems" );
			if ( luaObj.IsNil() == false )
			{
				bool bBool;
				for ( i = 0; i < PROHIBIT_ITEM_SIZE; ++i )
				{
					// 아래의 COMMENT는 데이터에서 읽어오지않는다;
					_GetBoolean( luaObj, COMMENT::ITEMPROHIBIT[i], bBool );
					if ( bBool == true )
						NodeData.m_prohibiteditems.push_back(SMAPNODE::ItemTypeValue(PROHIBIT_ITEM_LIST[i]));
				}
			}

			_GetBoolean( _object, "bRebirthBlock", NodeData.m_bRebirthBlock );
			_GetBoolean( _object, "bDisableSwitchMob", NodeData.m_bDisableSwitchMob );
			_GetBoolean( _object, "bChangeSchool", NodeData.m_bChangeSchool);
			_GetFloat(_object, "fRateExpDrop", NodeData.m_fRateExpDrop);
			_GetFloat(_object, "fRateItemDrop", NodeData.m_fRateItemDrop);
			_GetBoolean( _object, "bMacro", NodeData.m_bMacro);
			_GetBoolean( _object, "bPrison", NodeData.m_bPrison);
			_GetBoolean( _object, "bChangeCharSlot", NodeData.m_bChangeCharSlot );
			
			if (NodeData.m_NativeID.wMainID >= MAX_LAND_MID || NodeData.m_NativeID.wSubID >= MAX_LAND_SID )
			{
				std::string ErrorMsg(
					sc::string::format(
					"MAP Map MainID %1%>%2% or SubID %3% != 0",
					NodeData.m_NativeID.wMainID,
					MAX_LAND_MID,
					NodeData.m_NativeID.wSubID));
				sc::ErrorMessageBox(ErrorMsg, false);
				return false;
			}

			if ( bEmulator == true || GLOGIC::bGBStart == TRUE )
				NodeData.m_FieldServerID = 0;

			if (pServerInfo && nServerNum)
			{
				if (NodeData.m_FieldServerID >= (DWORD) nServerNum)
				{
					std::string ErrorMsg(
						sc::string::format(
						"FieldServer ID of Map is overflow %1%",
						NodeData.m_FieldServerID));
					sc::ErrorMessageBox(ErrorMsg, false);
				}

				if (!pServerInfo[0*MAX_CHANNEL_NUMBER+NodeData.m_FieldServerID].IsValid())
				{
					std::string ErrorMsg(
						sc::string::format(
						"Invalid FieldServer ID %1%",
						NodeData.m_FieldServerID));
					sc::ErrorMessageBox(ErrorMsg, false);

				}
			}

			if (!bTool && IsRegMapNode(NodeData))
			{
				MessageBox(NULL, "'GLGaeaClient::LoadMapList' Map Set Duplicate.", "ERROR", MB_OK);
				return false;
			}

			if (!bTool && !NodeData.m_bUse)
				continue;

			m_MapList.insert(FIELDMAP_VALUE(NodeData.m_NativeID.dwID, NodeData));
		}
		return true;
	}
	catch ( const LuaPlus::LuaException& e )
	{
		std::string ErrMsg(
			sc::string::format(
			"Plz check %1%, %2%",
			FileName,
			e.GetErrorMessage()));
		sc::ErrorMessageBox(ErrMsg, false);
		return false;
	}
}

bool GLMapList::LoadLua( const std::string& FileName, BOOL bTool, F_SERVER_INFO* pServerInfo, int nServerNum, const bool bEmulator)
{
	sc::lua_destroyState();
	sc::lua_init();

	std::string FullPath(GLOGIC::GetPath());
	FullPath.append(FileName);

	if (!sc::lua_doFile(FullPath))
	{
		return false;
	}

//	BOOL bPrevVersion = false;
//	LuaPlus::LuaObject MapObj = sc::lua_getGlobalFromName("VERSION");
// 	if ( MapObj.IsNil() )
// 		bPrevVersion = true;
// 	else
// 	{
// 		LuaPlus::LuaTableIterator verIterator( MapObj );
// 		bPrevVersion = verIterator.GetValue().IsNil() ? true : false;
// 	}
	// 이전 lua 버전체크안되던 버진인지 체크;
// 	WORD wVersion = 0;
// 	for (LuaPlus::LuaTableIterator itr( MapObj ); itr; itr.Next() )	
// 	{
// 		wVersion = itr.GetValue().GetInteger();
// 	}
// 	if ( wVersion <= 0 )
// 		bPrevVersion = true;
// 	
	bool bPrevVersion = true;
	WORD wText = 0; 
	LuaPlus::LuaObject MapObj = sc::lua_getGlobalFromName("VERSION");
	if ( MapObj.IsNil() == false )
	{
		LuaPlus::LuaTableIterator verIterator( MapObj );
		bPrevVersion = verIterator.GetValue().IsNil() ? true : false;

		if ( bPrevVersion == false )
		{
			wText = verIterator.GetValue().GetInteger();
		}
	}

	// LoadLuaPrev은 이제 안쓴다보면된다;
	if ( bPrevVersion == true )
	{
		std::string ErrorMsg("The previous version of the resource!");
		sc::ErrorMessageBox(ErrorMsg, false);
		//LoadLuaPrev( FileName, bTool, pServerInfo, nServerNum, bEmulator );
	}
	else
		LoadLuaCurr( FileName, bTool, pServerInfo, nServerNum, bEmulator );

	return true;
}

bool GLMapList::SaveLua( const TCHAR* szMapListFile )
{
	std::string FilePath = sc::getAppPath();
	if ( szMapListFile )
	{
		FilePath = GLOGIC::GetPath();
		std::string strMapListFileName = sc::string::format( "%1%", szMapListFile );
		FilePath = FilePath + "\\" + strMapListFileName;
	}
	else
		FilePath = FilePath + "\\" + GLogicData::GetInstance().GetMapListFileName();

	sc::TxtFile LuaFile(FilePath, true, sc::TxtFile::UTF16LE);
	LuaFile.write(L"-------------------------------------------------------------------------------");
	LuaFile.write(L"-- Map 세팅 파일");
	LuaFile.write(L"-- 2012-08-13 Jgkim");
	LuaFile.write(L"--");
	LuaFile.write(L"-- 주의)");
	LuaFile.write(L"-- 1) 대/소 문자를 확실하게 구분해야 합니다.");
	LuaFile.write(L"-- 2) 1, 0, true, false, null 을 확실하게 구분해야 합니다.");
	LuaFile.write(L"-- 3) UCS-2 Little Endian 로 저장해야 합니다.");
	LuaFile.write(L"-- 4) 문자열을 작성할때는 L\"문자열\" 로 작성하십시오. (unicode)");
	LuaFile.write(L"-------------------------------------------------------------------------------");
	LuaFile.write( sc::string::wformat(L"VERSION = { %1% } ", VERSION ));
	LuaFile.write(L"maplist =");
	LuaFile.write(L"{");
	for (FIELDMAP_ITER iter=m_MapList.begin(); iter!=m_MapList.end(); ++iter)
	{
		LuaFile.write(L"    {");
		const SMAPNODE& MapNode = iter->second;

		LuaFile.write( sc::string::wformat(L"        wMID = %1%, --MID",MapNode.m_NativeID.Mid() ));
		LuaFile.write( sc::string::wformat(L"        wSID = %1%, --SID",MapNode.m_NativeID.Sid() ));
		LuaFile.write( sc::string::wformat(L"        wFieldServerID = %1%, --FieldID",MapNode.m_FieldServerID ));

		//! 이제 맵이름은 XML에서 읽습니다.
		//LuaFile.write(sc::string::wformat(L"        L\"%1%\", -- Map Name",      sc::string::ansiToUnicode(MapNode.m_MapName)));
		LuaFile.write(sc::string::wformat(L"        strBGMName = L\"%1%\", -- BGM",           sc::string::ansiToUnicode(MapNode.m_BgmName)));
		LuaFile.write(sc::string::wformat(L"        strLodingImag = L\"%1%\", -- Loading image", sc::string::ansiToUnicode(MapNode.m_LoadingImageName)));
		LuaFile.write(sc::string::wformat(L"        strLevelFileName = L\"%1%\", -- Level file",    sc::string::ansiToUnicode(MapNode.m_LevelFile)));

		LuaFile.write(sc::string::wformat(L"		fRateExpDrop = %1%,", MapNode.m_fRateExpDrop));
		LuaFile.write(sc::string::wformat(L"		fRateItemDrop = %1%,", MapNode.m_fRateItemDrop));

		if (MapNode.m_bUse)
			LuaFile.write(L"        bUse = true, -- Map used");
		else
			LuaFile.write(L"        bUse = false, -- Map used");

		if (MapNode.m_bPeaceZone)
			LuaFile.write(L"        bPeaceZone = true, -- Peace zone");
		else
			LuaFile.write(L"        bPeaceZone = false, -- Peace zone");

		if (!MapNode.m_bCommission)
			LuaFile.write(L"        bCommission = true, -- Commission free");
		else
			LuaFile.write(L"        bCommission = false, -- Commission free");

		if (MapNode.m_bPKZone)
			LuaFile.write(L"        bPKZone = true, -- PK enable");
		else
			LuaFile.write(L"        bPKZone = false, -- PK enable");

		if (MapNode.m_bFreePK)
			LuaFile.write(L"        bFreePK = true, -- Free pk");
		else
			LuaFile.write(L"        bFreePK = false, -- Free pk");

		if (MapNode.m_bItemDrop)
			LuaFile.write(L"        bItemDrop = true, -- Item drop");
		else
			LuaFile.write(L"        bItemDrop = false, -- Item drop");

		if (MapNode.m_bFriendMove)
			LuaFile.write(L"        bFriendMove = true, -- Friend card");
		else
			LuaFile.write(L"        bFriendMove = false, -- Friend card");

		if (MapNode.m_bRestart)
			LuaFile.write(L"        bRestart = true, -- Resurrection forbid, character restart");
		else
			LuaFile.write(L"        bRestart = false, -- Resurrection forbid, character restart");

		if (MapNode.m_bPetActivity)
			LuaFile.write(L"        bPetActivity = true, -- Pet activity");
		else
			LuaFile.write(L"        bPetActivity = false, -- Pet activity");

		if (MapNode.m_bDecExp)
			LuaFile.write(L"        bDecExp = true, -- Exp drop");
		else
			LuaFile.write(L"        bDecExp = false, -- Exp drop");

		if (MapNode.m_bVehicleActivity)
			LuaFile.write(L"        bVehicleActivity = true, -- Vehicle activity");
		else
			LuaFile.write(L"        bVehicleActivity = false, -- Vehicle activity");

		if (MapNode.m_bClubBattleZone)
			LuaFile.write(L"        bClubBattleZone = true, -- Club battle zone");
		else
			LuaFile.write(L"        bClubBattleZone = false, -- Club battle zone");

		LuaFile.write( sc::string::wformat(L"        nInstantMap = %1%, -- Instant map(0 : normal, 1 : old, 2 : new", MapNode.m_emInstanceType ));

		if (MapNode.m_bQBoxDrop)
			LuaFile.write(L"        bQBoxDrop = true, -- Qbox drop");
		else
			LuaFile.write(L"        bQBoxDrop = false, -- Qbox drop");

		if (MapNode.m_bLunchBoxForbid)
			LuaFile.write(L"        bLunchBoxForbid = true, -- Lunchbox forbid");
		else
			LuaFile.write(L"        bLunchBoxForbid = false, -- Lunchbox forbid");

		if (MapNode.m_bCPReset)
			LuaFile.write(L"        bCPReset = true, -- Combat point reset");
		else
			LuaFile.write(L"        bCPReset = false, -- Combat point reset");

		if (MapNode.m_bPKMap)
			LuaFile.write(L"        bPKMap = true, -- PK map");
		else
			LuaFile.write(L"        bPKMap = false, -- PK map");

		if (MapNode.m_bUIMapSelect)
			LuaFile.write(L"        bUIMapSelect = true, -- UI map select");
		else
			LuaFile.write(L"        bUIMapSelect = false, -- UI map select");

		if (MapNode.m_bUIMapInfo)
			LuaFile.write(L"        bUIMapInfo = true, -- UI map info");
		else
			LuaFile.write(L"        bUIMapInfo = false, -- UI map info");

		if ( MapNode.m_bPartyMap )
			LuaFile.write(L"        bPartyMap = true, -- Party Map");
		else
			LuaFile.write(L"        bPartyMap = false, -- Party Map");

		if ( MapNode.m_bExpeditionMap )
			LuaFile.write(L"        bExpeditionMap = true, -- Expedition Map");
		else
			LuaFile.write(L"        bExpeditionMap = false, -- Expedition Map");

		if (MapNode.m_bClubPkRecord)
			LuaFile.write(L"        bClubPKRecord = true, -- Club pk record");
		else
			LuaFile.write(L"        bClubPKRecord = false, -- Club pk record");		

		if (MapNode.m_bPrivateMarketOpenable)
			LuaFile.write(L"        bPrivateMarketOpenable = true, -- Private Market Openable");
		else
			LuaFile.write(L"        bPrivateMarketOpenable = false, -- Private Market Openable");		

		if (MapNode.m_bSparring)
			LuaFile.write(L"        bSparring = true, -- 1:1, Party Sparring");
		else
			LuaFile.write(L"        bSparring = false, -- 1:1, Party Sparring");

		if (MapNode.m_bCullByObjectMap)
			LuaFile.write(L"        bCullByObjectMap = true, -- be culled Actors at player's view - not camera by Geometry Objects;");
		else
			LuaFile.write(L"        bCullByObjectMap = false, -- be culled Actors at player's view not camera by Geometry Objects;");

// 		if ( MapNode.m_bTutorialMap )
// 			LuaFile.write(L"        bTutorialMap = true,");
// 		else
// 			LuaFile.write(L"        bTutorialMap = false,");

		LuaFile.write(L"        ProhidibtedItems =");
		LuaFile.write(L"        {");
		for (int i = 0; i < PROHIBIT_ITEM_SIZE; ++i)
		{
			bool bEnable = MapNode.IsBlockDrugItem( (const SMAPNODE::ItemTypeValue)PROHIBIT_ITEM_LIST[i]);
			// 아래의 COMMENT는 데이터에서 읽어오지않는다;
			LuaFile.write( sc::string::wformat(L"            %1% = %2%,", COMMENT::ITEMPROHIBIT[i].c_str(), bEnable ? L"true" : L"false" ) );
		}
		LuaFile.write(L"        },");

		if (MapNode.m_bRebirthBlock)
			LuaFile.write(L"        bRebirthBlock = true, -- Rebirth Block;");
		else
			LuaFile.write(L"        bRebirthBlock = false, -- Rebirth Block;");

		if (MapNode.m_bDropBlockItem )
			LuaFile.write(L"        bDropBlockItem = true, -- Item Drop Block;");
		else
			LuaFile.write(L"        bDropBlockItem = false, -- Item Drop Block;");

		if (MapNode.m_bDisableSwitchMob )
			LuaFile.write(L"        bDisableSwitchMob = true, -- Disable AI of Switch Mob;");
		else
			LuaFile.write(L"        bDisableSwitchMob = false, -- Enable AI of Switch Mob;");		

		if( MapNode.m_bChangeSchool )
			LuaFile.write(L"        bChangeSchool = true, -- Change School Card");
		else
			LuaFile.write(L"        bChangeSchool = false, -- Change School Card");

		if( MapNode.m_bMacro )
			LuaFile.write(L"        bMacro = true, -- Macro");
		else
			LuaFile.write(L"        bMacro = false, -- Macro");

		if( MapNode.m_bPrison )
			LuaFile.write(L"        bPrison = true, -- Macro");
		else
			LuaFile.write(L"        bPrison = false, -- Macro");

		if ( MapNode.m_bChangeCharSlot )
			LuaFile.write( L"		bChangeCharSlot = true,	-- Change Character Slot" );
		else
			LuaFile.write( L"		bChangeCharSlot = false,	-- Change Character Slot" );


		LuaFile.write(L"    },");
	}
	LuaFile.write(L"}");

	AfxMessageBox(FilePath.c_str(), MB_OK);

	return true;
}

bool GLMapList::_GetBoolean( LuaPlus::LuaObject& LuaObj, const std::string& strKey, bool& bOutput )
{
	LuaPlus::LuaObject LuaData = LuaObj.GetByName( strKey.c_str() );
	if ( LuaData.IsNil() || LuaData.IsBoolean() == false )
	{
		bOutput = false;
		return false;
	}

	bOutput = LuaData.GetBoolean();
	return true;
}

bool GLMapList::_GetIntager( LuaPlus::LuaObject& LuaObj, const std::string& strKey, INT&  nOutput )
{
	LuaPlus::LuaObject LuaData = LuaObj.GetByName( strKey.c_str() );
	if ( LuaData.IsNil() || LuaData.IsInteger() == false )
	{
		nOutput = 0;
		return false;
	}

	nOutput = LuaData.GetInteger();
	return true;
}

bool GLMapList::_GetWString( LuaPlus::LuaObject& LuaObj, const std::string& strKey, std::string& szOutput )
{
	LuaPlus::LuaObject LuaData = LuaObj.GetByName( strKey.c_str() );
	if ( LuaData.IsNil() || LuaData.IsWString() == false )
		return false;
	
	szOutput = sc::string::unicodeToAnsi( (const wchar_t*)LuaData.GetWString() );
	return true;
}
bool GLMapList::_GetFloat( LuaPlus::LuaObject& LuaObj, const std::string& strKey, float& fOutput)
{
	LuaPlus::LuaObject LuaData = LuaObj.GetByName( strKey.c_str() );
	if ( LuaData.IsNil() || LuaData.IsNumber() == false )
	{
		fOutput = 1.0f;
		return false;
	}

	fOutput = static_cast<float>( LuaData.GetNumber() );
	return true;
}

HRESULT GLMapList::SAVE( const std::string& szMapListFile )
{
	// 확장자;
	CString szFileExt = sc::string::getFileExt( szMapListFile.c_str() );

	if ( szFileExt == "lua" )
		return SaveLua( szMapListFile.c_str() );
	
	return E_FAIL;
}
