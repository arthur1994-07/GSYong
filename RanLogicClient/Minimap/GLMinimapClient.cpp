#include "pch.h"
#include "GLMinimapClient.h"

#include "../../enginelib/DeviceMan.h"
#include "../../enginelib/GUInterface/GameTextControl.h"
#include "../../RanLogic/Crow/GLCrowDataMan.h"
#include "../../RanLogic/Crow/GLMobSchedule.h"
#include "../../RanLogic/Quest/GLQuestMan.h"

#include "../GLGaeaClient.h"
#include "../Level/GLLevelFileClient.h"
#include "../Party/GLPartyClient.h"
#include "../Char/GLCharacter.h"
#include "../CapturedTheField/GLCaptureTheFieldClient.h"

GLMinimapClient::GLMinimapClient(void)
:	m_pMinimap( NULL )
{
}

GLMinimapClient::GLMinimapClient( const GLMinimapClient& value )
{
}

GLMinimapClient::~GLMinimapClient(void)
{
	SAFE_DELETE( m_pMinimap );
}

GLMinimapClient& GLMinimapClient::GetInstance()
{
	static GLMinimapClient Instance;
	return Instance;
}

LuaTable GLMinimapClient::GetMapList()
{
	LuaTable tbMapList( GLWidgetScript::GetInstance().GetLuaState() );

	const GLMapList::FIELDMAP& mapList = GLWidgetScript::GetInstance().m_pGaeaClient->GetMapList();

	int nTableIndex = 1;
	GLMapList::FIELDMAP::const_iterator iter = mapList.begin();
	for( ; iter != mapList.end(); ++iter )
	{
		const SMAPNODE& sNode = iter->second;
		if( false == sNode.IsUIMapSelect() )
			continue;

		DWORD dwMapID = iter->first;

		LuaTable tbMap( GLWidgetScript::GetInstance().GetLuaState() );
		tbMap.set( "strName", GLWidgetScript::GetMapName( dwMapID ).c_str() );
		tbMap.set( "dwID", dwMapID );
		tbMap.set( "bUIMapInfo", sNode.m_bUIMapInfo );

		tbMapList.set( nTableIndex++, tbMap );
	}

	return tbMapList;
}

LuaTable GLMinimapClient::ImageToMap( DWORD dwMapID, int nPosX, int nPosY )
{
	LuaTable tbPos( GLWidgetScript::GetInstance().GetLuaState() );

	GLLevelFileClient* pLevelFile = GetLevelClient( dwMapID );
	if( NULL == pLevelFile || true == pLevelFile->IsInstantMap() )
		return tbPos;

	const GLMapAxisInfo& sMapAxisInfo = pLevelFile->GetMapAxisInfo();

	int nMapPosX = 0;
	int nMapPosY = 0;
	ConvertImgToMapPos( sMapAxisInfo, nPosX, nPosY, nMapPosX, nMapPosY );

	tbPos.set( "x", nMapPosX );
	tbPos.set( "y", nMapPosY );

	return tbPos;
}

LuaTable GLMinimapClient::PositionToMap( DWORD dwMapID, float fPosX, float fPosY, float fPosZ )
{
	LuaTable tbPos( GLWidgetScript::GetInstance().GetLuaState() );

	GLLevelFileClient* pLevelFile = GetLevelClient( dwMapID );
	if( NULL == pLevelFile )
		return tbPos;

	const GLMapAxisInfo& sMapAxisInfo = pLevelFile->GetMapAxisInfo();

	int nPosX = 0;
	int nPosY = 0;
	ConvertWorldToMap( sMapAxisInfo, D3DXVECTOR3( fPosX, fPosY, fPosZ ), nPosX, nPosY );

	tbPos.set( "x", nPosX );
	tbPos.set( "y", nPosY );

	return tbPos;
}

std::string GLMinimapClient::GetMapImgFile( DWORD dwMapID )
{
	GLLevelFileClient* pLevelFile = GetLevelClient( dwMapID );
	if( NULL == pLevelFile/* || true == pLevelFile->IsInstantMap()*/ )
		return "";

	const GLMapAxisInfo& sMapAxisInfo = pLevelFile->GetMapAxisInfo();

	std::string strText = sMapAxisInfo.GetMinMapTex();
	return strText;
}

LuaTable GLMinimapClient::GetMapZoneName( DWORD dwMapID )
{
	LuaTable tbZone( GLWidgetScript::GetInstance().GetLuaState() );

	GLLevelFileClient* pLevelFile = GetLevelClient( dwMapID );
	if( NULL == pLevelFile )
		return tbZone;

	std::vector< GLMobSchedule* > vecCrow;
	GetCrowDataFromNpcType( pLevelFile, NPC_TYPE_NORMAL, vecCrow );

	const GLMapAxisInfo& sMapAxisInfo = pLevelFile->GetMapAxisInfo();

	int nTableIndex = 1;
	for( size_t i = 0; i < vecCrow.size(); ++i )
	{
		SCROWDATA* pCrowData = GLCrowDataMan::GetInstance().GetCrowData( vecCrow[ i ]->m_CrowID );
		if( CROW_ZONENAME == pCrowData->m_emCrow )
		{
			int nPosX = 0;
			int nPosY = 0;
			ConvertWorldToMap( sMapAxisInfo, vecCrow[ i ]->m_pAffineParts->vTrans, nPosX, nPosY );

			LuaTable tbZoneData( GLWidgetScript::GetInstance().GetLuaState() );
			tbZoneData.set( "strName", pCrowData->GetName() );

			tbZoneData.set( "x", nPosX );
			tbZoneData.set( "y", nPosY );

			tbZone.set( nTableIndex++, tbZoneData );
		}
	}

	return tbZone;
}

LuaTable GLMinimapClient::GetMapGateName( DWORD dwMapID )
{
	LuaTable tbGate( GLWidgetScript::GetInstance().GetLuaState() );

	GLLevelFileClient* pLevelFile = GetLevelClient( dwMapID );
	if( NULL == pLevelFile )
		return tbGate;

	DxLandGateMan* pGateMan = pLevelFile->GetLandGateMan();
	if( NULL == pGateMan )
		return tbGate;

	const GLMapAxisInfo& sMapAxisInfo = pLevelFile->GetMapAxisInfo();

	int nTableIndex = 1;
	for( PDXLANDGATE pGate = pGateMan->GetListLandGate(); pGate != NULL; pGate = pGate->m_pNext )
	{
		if( pGate->GetFlags() & DxLandGate::GATE_OUT )
		{
			D3DXVECTOR3* pvStartPos = pGate->GetStartPos();
			if( NULL == pvStartPos )
				continue;

			int nPosX = 0;
			int nPosY = 0;
			ConvertWorldToMap( sMapAxisInfo, *pvStartPos, nPosX, nPosY );

			LuaTable tbGateData( GLWidgetScript::GetInstance().GetLuaState() );
			tbGateData.set( "strName", GLWidgetScript::GetMapName( pGate->GetToMapID().dwID ) );
			tbGateData.set( "x", nPosX );
			tbGateData.set( "y", nPosY );
			tbGateData.set( "GateID", pGate->GetGateID() );
			tbGateData.set( "ToMapID", pGate->GetToMapID().dwID );
			tbGateData.set( "CrowType", (int)CROW_GATE);
			tbGateData.set( "ID", pGate->GetGateID());

			tbGate.set( nTableIndex++, tbGateData );
		}
	}

	return tbGate;
}

LuaTable GLMinimapClient::GetMapNormalNpc( DWORD dwMapID )
{ 
	LuaTable tbNpc( GLWidgetScript::GetInstance().GetLuaState() );

	GLLevelFileClient* pLevelFile = GetLevelClient( dwMapID );
	if( NULL == pLevelFile || true == pLevelFile->IsInstantMap() )
		return tbNpc;

	int nTableIndex = 1;

	std::vector< std::string > vecNpcName;
	std::vector< GLMobSchedule* > vecCrow;

	// 퀘스트 시작
	GetCrowDataFromQuest( pLevelFile, QUEST_START, vecCrow, &vecNpcName );
	SetQuestDataInTable( pLevelFile, tbNpc, vecCrow, QUEST_START, &nTableIndex );
	vecCrow.clear();

	// 퀘스트 진행
	GetCrowDataFromQuest( pLevelFile, QUEST_STEP, vecCrow, &vecNpcName );
	SetQuestDataInTable( pLevelFile, tbNpc, vecCrow, QUEST_STEP, &nTableIndex );
	vecCrow.clear();

	// 퀘스트 끝
	GetCrowDataFromQuest( pLevelFile, QUEST_FINISH, vecCrow, &vecNpcName );
	SetQuestDataInTable( pLevelFile, tbNpc, vecCrow, QUEST_FINISH, &nTableIndex );
	vecCrow.clear();

	// 마켓
	GetCrowDataFromNpcInven( pLevelFile, vecCrow );
	DeleteSameName( vecNpcName, vecCrow );		// 퀘스트 NPC와 중복 데이터 제거
	SetMarketDataInTable( pLevelFile, tbNpc, vecCrow, &nTableIndex );

	// 일반 NPC 중복 데이터 삭제를 위해 마켓 NPC 이름 넣어주기
	for( size_t i = 0; i < vecCrow.size(); ++i )
	{
		SCROWDATA* pCrowData = GLCrowDataMan::GetInstance().GetCrowData( vecCrow[ i ]->m_CrowID );
		if( NULL == pCrowData )
			continue;

		vecNpcName.push_back( std::string( pCrowData->GetName() ) );
	}
	vecCrow.clear();

	// 일반 NPC
	GetCrowDataFromNpcType( pLevelFile, NPC_TYPE_NORMAL, vecCrow );
	DeleteSameName( vecNpcName, vecCrow );		// 퀘스트/마켓 NPC와 중복 데이터 제거

	// 대화 가능한 놈들만 넣기
	std::vector< GLMobSchedule* > vecNormalNpcCrow;
	for( size_t i = 0; i < vecCrow.size(); ++i )
	{
		SCROWDATA* pCrowData = GLCrowDataMan::GetInstance().GetCrowData( vecCrow[ i ]->m_CrowID );
		if( 0 < strlen( pCrowData->GetTalkFile() ) )
			vecNormalNpcCrow.push_back( vecCrow[ i ] );
	}
	SetDataInTable( pLevelFile, tbNpc, vecNormalNpcCrow, &nTableIndex );

	return tbNpc;
}

LuaTable GLMinimapClient::GetMapPost( DWORD dwMapID )
{
	LuaTable tbPost( GLWidgetScript::GetInstance().GetLuaState() );

	GLLevelFileClient* pLevelFile = GetLevelClient( dwMapID );
	if( NULL == pLevelFile || true == pLevelFile->IsInstantMap() )
		return tbPost;

	std::vector< GLMobSchedule* > vecCrow;
	GetCrowDataFromNpcType( pLevelFile, NPC_TYPE_POSTBOX, vecCrow );
	SetDataInTable( pLevelFile, tbPost, vecCrow );

	return tbPost;
}

LuaTable GLMinimapClient::GetMapTalkNpc( DWORD dwMapID, int nTalkType )
{
	LuaTable tbNpc( GLWidgetScript::GetInstance().GetLuaState() );

	GLLevelFileClient* pLevelFile = GetLevelClient( dwMapID );
	if( NULL == pLevelFile || true == pLevelFile->IsInstantMap() )
		return tbNpc;

	std::vector< GLMobSchedule* > vecCrow;
	GetCrowDataFromNpcTalk( pLevelFile, nTalkType, vecCrow );
	SetDataInTable( pLevelFile, tbNpc, vecCrow );

	return tbNpc;
}

LuaTable GLMinimapClient::GetMapMob( DWORD dwMapID )
{
	LuaTable tbMob( GLWidgetScript::GetInstance().GetLuaState() );

	GLLevelFileClient* pLevelFile = GetLevelClient( dwMapID );
	if( NULL == pLevelFile || true == pLevelFile->IsInstantMap() )
		return tbMob;

	std::vector< GLMobSchedule* > vecCrow;
	GetMobData( pLevelFile, vecCrow );
	SetMobDataInTable( pLevelFile, tbMob, vecCrow );

	return tbMob;
}

LuaTable GLMinimapClient::GetMapMobList( DWORD dwMapID )
{
	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	LuaTable tbMob( GLWidgetScript::GetInstance().GetLuaState() );

	GLLevelFileClient* pLevelFile = GetLevelClient( dwMapID );
	if( NULL == pLevelFile || true == pLevelFile->IsInstantMap() )
		return tbMob;

	std::vector< GLMobSchedule* > vecCrow;
	GetMobData( pLevelFile, vecCrow );

	std::map< DWORD, GLMobSchedule* > mapMob;
	for( size_t i = 0; i < vecCrow.size(); ++i )
		mapMob[ vecCrow[ i ]->m_CrowID.Id() ] = vecCrow[ i ];

	int nTableIndex = 1;
	std::map< DWORD, GLMobSchedule* >::iterator iter = mapMob.begin();
	for( ; iter != mapMob.end(); ++iter )
	{
		SCROWDATA* pCrowData = GLCrowDataMan::GetInstance().GetCrowData( iter->first );
		if( NULL == pCrowData )
			continue;
		
		DWORD dwMobStrong = GetMobStrongType( pCrowData );
		
		const GLQuestPlay::QUESTMOB_MAP& mapQuestMob = pChar->m_cQuestPlay.GetQuestMobProc();
		GLQuestPlay::QUESTMOB_MAP::const_iterator iter = mapQuestMob.find( pCrowData->GetId().dwID );
		if( iter != mapQuestMob.end() )
			dwMobStrong |= MONSTER_TYPE_QUEST;

		LuaTable tbMobInfo( GLWidgetScript::GetInstance().GetLuaState() );
		tbMobInfo.set( "dwID", pCrowData->GetId().dwID );
		tbMobInfo.set( "strName", pCrowData->GetName() );
		tbMobInfo.set( "nType", (INT) dwMobStrong );
		tbMobInfo.set( "nLevel", (INT)pCrowData->GetLevel() );
		tbMob.set( nTableIndex++, tbMobInfo );
	}

	return tbMob;
}

LuaTable GLMinimapClient::GetMobGenItemList( LuaTable tbMobID )
{
	LuaTable tbGenItemList( GLWidgetScript::GetInstance().GetLuaState() );

	std::set< DWORD > setItem;

	int nGenItemTableIndex = 1;
	for( int i = 1; i <= tbMobID.TableLen(); ++i )
	{
		DWORD dwID = tbMobID.get< DWORD >( i );

		SCROWDATA* pCrowData = GLCrowDataMan::GetInstance().GetCrowData( dwID );
		if( NULL == pCrowData )
			continue;

		LuaTable tbGenItemList( GLWidgetScript::GetInstance().GetLuaState() );

		CGENITEM::VEC_SGENINFO& vecGenInfo = pCrowData->m_cGenItem.m_vecGenInfo;

		CGENITEM::VEC_SGENINFO_ITER iterInfo = vecGenInfo.begin();
		for( ; iterInfo != vecGenInfo.end(); ++iterInfo )
		{
			SGENINFO& sInfo = (*iterInfo);
			SGENINFO::VEC_SPGENITEM& vecGenSpec = sInfo.vecGenItem;

			for( size_t i = 0; i < vecGenSpec.size(); ++i )
			{
				if( EMGENITEMTYPE_ITEM != vecGenSpec[ i ]->emType )
					continue;

				SGENITEMSPEC* pSpec = dynamic_cast< SGENITEMSPEC* >( vecGenSpec[ i ].get() );
				if( NULL != pSpec )
				{
					const GLItemMan::SGENITEMS& sGenItem = GLItemMan::GetInstance().GetGenSpecItems( pSpec->dwSpecID );
					const std::vector< SNATIVEID >& vecGenItem = sGenItem.m_vecItems;
					for( size_t j = 0; j < vecGenItem.size(); ++j )
					{
						if( vecGenItem[ j ] != NATIVEID_NULL() )
							setItem.insert( vecGenItem[ j ].dwID );
					}
				}
				else
				{
					SGENITEMID* pItem = dynamic_cast< SGENITEMID* >( vecGenSpec[ i ].get() );
					GASSERT( pItem );
					if( pItem->sItemID != NATIVEID_NULL() )
						setItem.insert( pItem->sItemID.dwID );
				}
			}
		}
	}

	std::set< DWORD >::iterator iterItem = setItem.begin();
	for( ; iterItem != setItem.end(); ++iterItem )
	{
		SNATIVEID sID( (*iterItem) );

		LuaTable tbGenItem( GLWidgetScript::GetInstance().GetLuaState() );
		tbGenItem.set( "MID", sID.Mid() );
		tbGenItem.set( "SID", sID.Sid() );
		tbGenItemList.set( nGenItemTableIndex++, tbGenItem );
	}

	return tbGenItemList;
}

LuaTable GLMinimapClient::GetAuthenticatorInfo( DWORD dwMapID )
{
	lua_tinker::table tbAuthInfoTable( GLWidgetScript::GetInstance().GetLuaState() );

	GLLevelFileClient* pLevelFile = GetLevelClient( dwMapID );
	if( NULL == pLevelFile )//|| true == pLevelFile->IsInstantMap() )
		return tbAuthInfoTable;

	const GLMapAxisInfo& sMapAxisInfo = pLevelFile->GetMapAxisInfo();
	
	GLGaeaClient* pClient = GLWidgetScript::GetInstance().m_pGaeaClient;
	PVP::GLCaptureTheFieldClient* pCTFClient = pClient->GetCaptureTheField();
	if ( pCTFClient == NULL || pClient->IsCTFMap_PVP() == false )
		return tbAuthInfoTable;

	PVP::GLCaptureTheFieldClient::VEC_AUTHENTICATORINFO& vecAuthInfo = pCTFClient->GetCurrentGlobalState().vecAUTHENTICATOR;
	PVP::GLCaptureTheFieldClient::VEC_AUTHENTICATORINFO_ITER iterAuthInfo = vecAuthInfo.begin()
		, iterEnd = vecAuthInfo.end();

	int nTableIndex = 0;
	for ( ; iterAuthInfo != iterEnd; ++iterAuthInfo )
	{
		const PVP::SAUTHENTICATORINFO& sAuthinfo = (*iterAuthInfo);
		lua_tinker::table tbAuthInfo( GLWidgetScript::GetInstance().GetLuaState() );

		int nPosX(0), nPosY(0);
		ConvertWorldToMap(sMapAxisInfo, sAuthinfo.vPos, nPosX, nPosY);

		tbAuthInfo.set("ID", sAuthinfo.sID.dwID );
		tbAuthInfo.set("strName", sAuthinfo.strName );
		tbAuthInfo.set("emSchool", sAuthinfo.emAuthSchool );
		tbAuthInfo.set("PosX", nPosX );
		tbAuthInfo.set("PosY", nPosY );

		tbAuthInfoTable.set(++nTableIndex, tbAuthInfo);
	}
	return tbAuthInfoTable;
}

LuaTable GLMinimapClient::GetAllianceInfoList( DWORD dwMapID )
{
	lua_tinker::table tbAlllianceInfoList( GLWidgetScript::GetInstance().GetLuaState() );

	GLLevelFileClient* pLevelFile = GetLevelClient( dwMapID );
	if( NULL == pLevelFile )
		return tbAlllianceInfoList;

	const GLMapAxisInfo& sMapAxisInfo = pLevelFile->GetMapAxisInfo();

	GLGaeaClient* pClient = GLWidgetScript::GetInstance().m_pGaeaClient;
	PVP::GLCaptureTheFieldClient* pCTFClient = pClient->GetCaptureTheField();
	if ( pCTFClient == NULL || pClient->IsCTFMap_PVP() == false )
		return tbAlllianceInfoList;

	PVP::GLCaptureTheFieldClient::VEC_ALLIANCEINFO& vecAllianceInfo = pCTFClient->GetAllianceInfo();
	PVP::GLCaptureTheFieldClient::VEC_ALLIANCEINFO_ITER iterAllianceInfo = vecAllianceInfo.begin()
			, iterEnd = vecAllianceInfo.end();;

	int nAllianceIndex = 0;
	for ( ; iterAllianceInfo != iterEnd; ++iterAllianceInfo )
	{
		lua_tinker::table tbAllianceInfo( GLWidgetScript::GetInstance().GetLuaState() );

		 PVP::SALLIANCEINFO& sAllianceInfo = (*iterAllianceInfo);

		 if ( sAllianceInfo.IsNull() )
		 {
			 continue;
		 }

		 if( sAllianceInfo.bMyInfo == true )
			 continue ;

		 tbAllianceInfo.set( "PosX", sAllianceInfo.vPos.x );
		 tbAllianceInfo.set( "PosY", sAllianceInfo.vPos.y );
		 tbAllianceInfo.set( "PosZ", sAllianceInfo.vPos.z );


		 tbAlllianceInfoList.set( ++nAllianceIndex , tbAllianceInfo );
	}

	return tbAlllianceInfoList;
}

LuaTable GLMinimapClient::GetLevelCondition( DWORD dwMapID )
{
	LuaTable tbLevelCondition( GLWidgetScript::GetInstance().GetLuaState() );

	GLLevelFileClient* pLevelFile = GetLevelClient( dwMapID );
	if( NULL == pLevelFile )
		return tbLevelCondition;

	const char* szQuest = "";

	GLQUEST* pQuest = GLQuestMan::GetInstance().Find( pLevelFile->GetLevelRequire()->m_sComQuestID.Id() );
	if( NULL != pQuest )
	{
		if( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			CString strKey;
			strKey.Format( "QN_%03d_%03d", pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID );

			szQuest = CGameTextMan::GetInstance().GetQuestText( strKey.GetString() ).GetString();
		}
		else
		{
			szQuest = pQuest->GetTITLE();
		}
	}

	tbLevelCondition.set( 1, (int) pLevelFile->GetLevelRequire()->m_signLevel );
	tbLevelCondition.set( 2, pLevelFile->GetLevelRequire()->m_wLevel );
	tbLevelCondition.set( 3, pLevelFile->GetLevelRequire()->m_wLevel2 );
	tbLevelCondition.set( 4, szQuest );

	return tbLevelCondition;
}

void GLMinimapClient::ConvertWorldToMap( const GLMapAxisInfo& sMapAxisInfo,
										 const D3DXVECTOR3& vPosition, int& nPosX, int& nPosY )
{
	int worldStartX = sMapAxisInfo.GetMapStartX();
	int worldStartY = sMapAxisInfo.GetMapStartY();
	int worldSizeX =  sMapAxisInfo.GetMapSizeX();
	int worldSizeY =  sMapAxisInfo.GetMapSizeY();

	const D3DXVECTOR2 vMinimapTexSize = sMapAxisInfo.GetMiniMapTexSize();
	int mapTextureSizeX = static_cast< int >( vMinimapTexSize.x );
	int mapTextureSizeY = static_cast< int >( vMinimapTexSize.y );

	mapTextureSizeX = 0 != mapTextureSizeX ? mapTextureSizeX : 1;
	mapTextureSizeY = 0 != mapTextureSizeY ? mapTextureSizeY : 1;

	worldSizeX = 0 != worldSizeX ? worldSizeX : 1;
	worldSizeY = 0 != worldSizeY ? worldSizeY : 1;

	int nCur_X = static_cast< int >( vPosition.x ) - worldStartX;
	int nCur_Y = ( worldSizeY - static_cast< int >( vPosition.z ) ) + worldStartY;

	nPosX = static_cast< int >( mapTextureSizeX * nCur_X / worldSizeX );
	nPosY = static_cast< int >( mapTextureSizeY * nCur_Y / worldSizeY );
}

void GLMinimapClient::ConvertImgMapToWorld( const GLMapAxisInfo& sMapAxisInfo,
											int nPosX, int nPosY, D3DXVECTOR3& vPosition )
{
	const D3DXVECTOR2& vTexSize = sMapAxisInfo.GetMiniMapTexSize();

	float fPosX = sMapAxisInfo.GetMapSizeX() / vTexSize.x * nPosX;
	float fPosY = sMapAxisInfo.GetMapSizeY() / vTexSize.y * nPosY;

	vPosition.x = fPosX + sMapAxisInfo.GetMapStartX();
	vPosition.z = sMapAxisInfo.GetMapSizeY() - ( fPosY - sMapAxisInfo.GetMapStartY() );
}

void GLMinimapClient::ConvertImgToMapPos( const GLMapAxisInfo& sMapAxisInfo,
										  int nPosX, int nPosY, int& nMapPosX, int& nMapPosY )
{
	const D3DXVECTOR2& vTexSize = sMapAxisInfo.GetMiniMapTexSize();

	// Y 값은 반대로 되어 있다.
	nPosY = (int)( vTexSize.y - nPosY );

	nMapPosX = (int)( sMapAxisInfo.GetMapSizeX() * nPosX / vTexSize.x / 50.f );
	nMapPosY = (int)( sMapAxisInfo.GetMapSizeY() * nPosY / vTexSize.y / 50.f );
}

GLLevelFileClient* GLMinimapClient::GetLevelClient( DWORD dwMapID )
{
	GLGaeaClient* pClient = GLWidgetScript::GetInstance().m_pGaeaClient;

	GLLevelFileClient* pRet = NULL;

	if( dwMapID == pClient->GetActiveMapID().getBaseMapID().dwID )
	{
		GLLandManClient* pLandMan = pClient->GetActiveMap();
		pRet = reinterpret_cast< GLLevelFileClient* >( pLandMan );
	}
	else if( NULL != m_pMinimap && dwMapID == m_pMinimap->GetBaseMapID().dwID )
	{
		pRet = m_pMinimap;
	}
	else
	{
		const SMAPNODE* pNode = pClient->FindMapNode( SNATIVEID( dwMapID ) );
		if( NULL != pNode )
		{
			SAFE_DELETE( m_pMinimap );

			pRet = new GLLevelFileClient( pClient );
			pRet->LoadFile( pNode->m_LevelFile, pClient->GetEngineDevice()->GetDevice() );
			pRet->SetBaseMapID( pNode->GetId() );
			pRet->SetPeaceZone( true );

			m_pMinimap = pRet;
		}
	}

	return pRet;
}

bool GLMinimapClient::haveInvenItem( const SCROWDATA* pCrowData )
{
	for( int i = 0; i < SCROWACTION::SALENUM; ++i )
	{
		if( false == pCrowData->m_sSaleItems[ i ].IsEmpty() )
			return true;
	}

	return false;
}

GLMinimapClient::MONSTER_TYPE GLMinimapClient::GetMobStrongType( SCROWDATA* pCrowData )
{
	DWORD dwMobStrong = MONSTER_TYPE_VERY_WEAK;
	if( pCrowData->IsBoss())
		dwMobStrong |= MONSTER_TYPE_BOSS;

	const int LEVEL_DIFF_STRONG = 30;
	int diff = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter()->GetLevel() - pCrowData->m_wLevel;
	if( diff >= LEVEL_DIFF_STRONG )
		dwMobStrong |= MONSTER_TYPE_VERY_WEAK;
	else if( diff >= 0  && diff <= LEVEL_DIFF_STRONG - 1  )
		dwMobStrong |= MONSTER_TYPE_WEAK;
	else if( diff < -30 )
		dwMobStrong |= MONSTER_TYPE_STRONG;
	else
		dwMobStrong |= MONSTER_TYPE_NORMAL;
	// 아래의 공식에는 MONSTER_TYPE_NORMAL과 MONSTER_TYPE_STRONG이 절대 나올 수 없는 구조이다;
// 	if( diff > LEVEL_DIFF_STRONG )
// 		dwMobStrong |= MONSTER_TYPE_VERY_WEAK;
// 	else if( diff <= LEVEL_DIFF_STRONG - 1  )
// 		dwMobStrong |= MONSTER_TYPE_WEAK;
// 	else if( diff < -29 )
// 		dwMobStrong |= MONSTER_TYPE_NORMAL;
// 	else if( diff > -30 )
// 		dwMobStrong |= MONSTER_TYPE_STRONG;
// 	else
// 		GASSERT(false);

	return (MONSTER_TYPE)dwMobStrong;
}

void GLMinimapClient::DeleteSameName( const std::vector< std::string >& vecName, std::vector< GLMobSchedule* >& vecCrow )
{
	std::vector< GLMobSchedule* >::iterator iter = vecCrow.begin();
	while( iter != vecCrow.end() )
	{
		SCROWDATA* pCrowData = GLCrowDataMan::GetInstance().GetCrowData( (*iter)->m_CrowID );
		if( NULL == pCrowData )
			continue;

		bool bSame = false;
		for( size_t i = 0; i < vecName.size(); ++i )
		{
			if( 0 == strcmp( pCrowData->GetName(), vecName[ i ].c_str() ) )
			{
				bSame = true;
				break;
			}
		}

		if( true == bSame )
			iter = vecCrow.erase( iter );
		else
			++iter;
	}
}

void GLMinimapClient::GetCrowDataFromNpcType( const GLLevelFileClient* pLevelFile, 
											  int nNpcType, std::vector< GLMobSchedule* >& vecCrow )
{
	const MOBSCHEDULELIST* pMobScheduleList = pLevelFile->GetMobSchMan()->GetMobSchList();

	MOBSCHEDULENODE* pCur = pMobScheduleList->m_pHead;
	while( NULL != pCur )
	{
		const SCROWDATA* pCrowData = GLCrowDataMan::GetInstance().GetCrowData( pCur->Data->m_CrowID );
		if( NULL == pCrowData )
		{
			pCur = pCur->pNext;
			continue;
		}

		if( pCrowData->m_emNPCType == nNpcType )
			vecCrow.push_back( pCur->Data );

		pCur = pCur->pNext;
	}
}

void GLMinimapClient::GetCrowDataFromQuest( const GLLevelFileClient* pLevelFile, QUEST_TYPE questType,
											std::vector< GLMobSchedule* >& vecQuestNpc,
											std::vector< std::string >* pvecNpcName )
{
	GLGaeaClient* pClient = GLWidgetScript::GetInstance().m_pGaeaClient;

	const DWORD nPartyMember = pClient->GetMyPartyClient()->GetNMember();

	const MOBSCHEDULELIST* pMobScheduleList = pLevelFile->GetMobSchMan()->GetMobSchList();
	MOBSCHEDULENODE* pCur = pMobScheduleList->m_pHead;

	GLCharacter* pChar = pClient->GetCharacter();

	CTime cCurSvrTime = pClient->GetCurrentTime();
	while( NULL != pCur )
	{
		SCROWDATA* pCrowData = GLCrowDataMan::GetInstance().GetCrowData( pCur->Data->m_CrowID );
		if( NULL == pCrowData )
		{
			pCur = pCur->pNext;
			continue;
		}

		CNpcTalkControl::NPCMAP* pTalkMap = pChar->FindDefaultTalk( pCrowData->GetTalkFile() );
		if( NULL == pTalkMap )
		{
			pCur = pCur->pNext;
			continue;
		}

		std::vector< std::string > questTitleVec;

		VEC_DWORD vecTalk;
		BOOST_FOREACH( CNpcTalkControl::NPCMAP_VALUE& it, *pTalkMap )
		{
			std::tr1::shared_ptr< SNpcTalk > spTalk = it.second;
			if( NULL == spTalk )
				continue;

			bool isOK = false;
			switch( questType )
			{
			case QUEST_START :
				{
					if( SNpcTalk::EM_QUEST_START == spTalk->m_nACTION )
					{
						if( TRUE == spTalk->DoTEST( pChar, nPartyMember, cCurSvrTime.GetHour() ) )
						{
							isOK = true;

							for( int i = 0; i < MAX_QUEST_START; ++i )
							{
								if( INT_MAX != spTalk->m_dwQuestStartID[ i ] )
								{
									GLQUEST* pQuest = GLQuestMan::GetInstance().Find( spTalk->m_dwQuestStartID[ i ] );
									if( NULL != pQuest )
										questTitleVec.push_back( pQuest->GetTITLE() );
								}
							}
						}
					}
				}
				break;

			case QUEST_STEP :
				{
					if( SNpcTalk::EM_QUEST_STEP == spTalk->m_nACTION )
					{
						if( TRUE == spTalk->DoTEST( pChar, nPartyMember, cCurSvrTime.GetHour() ) )
						{
							isOK = true;

							GLQUEST* pQuest = GLQuestMan::GetInstance().Find( spTalk->m_dwACTION_PARAM1 );
							if( NULL != pQuest )
								questTitleVec.push_back( pQuest->GetTITLE() );
						}
					}
				}
				break;

			case QUEST_FINISH:
				{
					if( SNpcTalk::EM_QUEST_STEP == spTalk->m_nACTION )
					{
						GLQUESTPROG* pQuestProg = pChar->m_cQuestPlay.FindProc( spTalk->m_dwACTION_PARAM1 );
						if( NULL != pQuestProg )
						{
							if( true == pQuestProg->IsLastStep() && TRUE == spTalk->DoTEST( pChar, nPartyMember, cCurSvrTime.GetHour() ) )
							{
								isOK = true;

								GLQUEST* pQuest = GLQuestMan::GetInstance().Find( spTalk->m_dwACTION_PARAM1 );
								if( NULL != pQuest )
									questTitleVec.push_back( pQuest->GetTITLE() );								
							}
						}
					}
				}
				break;
			}

			if( true == isOK )
			{
				if( vecQuestNpc.end() == std::find( vecQuestNpc.begin(), vecQuestNpc.end(), pCur->Data ) )
				{
					vecQuestNpc.push_back( pCur->Data );

					if( NULL != pvecNpcName )
						pvecNpcName->push_back( std::string( pCrowData->GetName() ) );
				}
			}
		}

		pCur = pCur->pNext;
	}
}

void GLMinimapClient::GetCrowDataFromNpcTalk( const GLLevelFileClient* pLevelFile, int m_talkType,
											  std::vector< GLMobSchedule* >& vecCrow )
{
	const MOBSCHEDULELIST* pMobScheduleList = pLevelFile->GetMobSchMan()->GetMobSchList();
	MOBSCHEDULENODE* pCur = pMobScheduleList->m_pHead;

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();

	while( NULL != pCur )
	{
		const SCROWDATA* pCrowData = GLCrowDataMan::GetInstance().GetCrowData( pCur->Data->m_CrowID );
		if( NULL == pCrowData )
		{
			pCur = pCur->pNext;
			continue;
		}

		CNpcTalkControl::NPCMAP* pTalkMap = pChar->FindDefaultTalk( pCrowData->GetTalkFile() );
		if( NULL == pTalkMap )
		{
			pCur = pCur->pNext;
			continue;
		}

		VEC_DWORD vecTalk;
		BOOST_FOREACH( CNpcTalkControl::NPCMAP_VALUE& it, *pTalkMap )
		{
			std::tr1::shared_ptr< SNpcTalk > spTalk = it.second;
			if( NULL == spTalk )
				continue;

			if( SNpcTalk::EM_BASIC != spTalk->m_nACTION )
				break;

			if( spTalk->m_dwACTION_NO == m_talkType )
			{
				vecCrow.push_back( pCur->Data );
				break;
			}
		}

		pCur = pCur->pNext;
	}
}

void GLMinimapClient::GetCrowDataFromNpcInven( const GLLevelFileClient* pLevelFile,
											   std::vector< GLMobSchedule* >& vecCrow,
											   std::vector< std::string >* pvecQuestNpc )
{
	const MOBSCHEDULELIST* pMobScheduleList = pLevelFile->GetMobSchMan()->GetMobSchList();
	MOBSCHEDULENODE* pCur = pMobScheduleList->m_pHead;

	while( NULL != pCur )
	{
		const SCROWDATA* pCrowData = GLCrowDataMan::GetInstance().GetCrowData(pCur->Data->m_CrowID);
		if( NULL == pCrowData )
		{
			pCur = pCur->pNext;
			continue;
		}

		if( true == haveInvenItem( pCrowData ) )
		{
			vecCrow.push_back( pCur->Data );

			if( NULL != pvecQuestNpc )
				pvecQuestNpc->push_back( pCur->Data->m_szName );
		}

		pCur = pCur->pNext;
	}
}

void GLMinimapClient::GetMobData( const GLLevelFileClient* pLevelFile, std::vector< GLMobSchedule* >& vecCrow )
{
	const MOBSCHEDULELIST* pMobScheduleList = pLevelFile->GetMobSchMan()->GetMobSchList();
	MOBSCHEDULENODE* pCur = pMobScheduleList->m_pHead;

	while( NULL != pCur )
	{
		const SCROWDATA* pCrowData = GLCrowDataMan::GetInstance().GetCrowData(pCur->Data->m_CrowID);
		if( NULL == pCrowData )
		{
			pCur = pCur->pNext;
			continue;
		}

		if( CROW_MOB == pCrowData->m_emCrow )
			vecCrow.push_back( pCur->Data );

		pCur = pCur->pNext;
	}
}

void GLMinimapClient::SetDataInTable( const GLLevelFileClient* pLevelFile, LuaTable& tbData,
									  std::vector< GLMobSchedule* > vecCrow, int* pnTableIndex )
{
	const GLMapAxisInfo& sMapAxisInfo = pLevelFile->GetMapAxisInfo();

	int nTableIndex = 1;
	if( NULL == pnTableIndex )
		pnTableIndex = &nTableIndex;

	for( size_t i = 0; i < vecCrow.size(); ++i )
	{
		SCROWDATA* pCrowData = GLCrowDataMan::GetInstance().GetCrowData( vecCrow[ i ]->m_CrowID );

		int nPosX = 0;
		int nPosY = 0;
		ConvertWorldToMap( sMapAxisInfo, vecCrow[ i ]->m_pAffineParts->vTrans, nPosX, nPosY );

		LuaTable tbNpcData( GLWidgetScript::GetInstance().GetLuaState() );
		tbNpcData.set( "strName", pCrowData->GetName() );
		tbNpcData.set( "x", nPosX );
		tbNpcData.set( "y", nPosY );

		//택시카드 위치 확인용 변수
		tbNpcData.set( "NpcID", vecCrow[ i ]->m_CrowID.dwID );
		tbNpcData.set( "NpcID1", pCrowData->GetId().dwID);
		tbNpcData.set( "CrowType", (int)pCrowData->m_sBasic.m_emCrow);
		tbNpcData.set( "ID", vecCrow[ i ]->m_CrowID.dwID);

		tbData.set( (*pnTableIndex)++, tbNpcData );
	}
}

void GLMinimapClient::SetQuestDataInTable( const GLLevelFileClient* pLevelFile, LuaTable& tbData,
										   std::vector< GLMobSchedule* > vecCrow, QUEST_TYPE questType,
										   int* pnTableIndex )
{
	const GLMapAxisInfo& sMapAxisInfo = pLevelFile->GetMapAxisInfo();

	int nTableIndex = 1;
	if( NULL == pnTableIndex )
		pnTableIndex = &nTableIndex;

	for( size_t i = 0; i < vecCrow.size(); ++i )
	{
		SCROWDATA* pCrowData = GLCrowDataMan::GetInstance().GetCrowData( vecCrow[ i ]->m_CrowID );

		int nPosX = 0;
		int nPosY = 0;
		ConvertWorldToMap( sMapAxisInfo, vecCrow[ i ]->m_pAffineParts->vTrans, nPosX, nPosY );

		LuaTable tbNpcData( GLWidgetScript::GetInstance().GetLuaState() );
		tbNpcData.set( "strName", pCrowData->GetName() );
		tbNpcData.set( "x", nPosX );
		tbNpcData.set( "y", nPosY );
		tbNpcData.set( "nQuestType", (int) questType );
		tbNpcData.set( "CrowType", (int)pCrowData->m_sBasic.m_emCrow );
		tbNpcData.set( "ID", (int)vecCrow[ i ]->m_CrowID.dwID );

		tbData.set( (*pnTableIndex)++, tbNpcData );
	}
}

void GLMinimapClient::SetMarketDataInTable( const GLLevelFileClient* pLevelFile, LuaTable& tbData,
											std::vector< GLMobSchedule* > vecCrow, int* pnTableIndex )
{
	const GLMapAxisInfo& sMapAxisInfo = pLevelFile->GetMapAxisInfo();

	int nTableIndex = 1;
	if( NULL == pnTableIndex )
		pnTableIndex = &nTableIndex;

	for( size_t i = 0; i < vecCrow.size(); ++i )
	{
		SCROWDATA* pCrowData = GLCrowDataMan::GetInstance().GetCrowData( vecCrow[ i ]->m_CrowID );

		int nPosX = 0;
		int nPosY = 0;
		ConvertWorldToMap( sMapAxisInfo, vecCrow[ i ]->m_pAffineParts->vTrans, nPosX, nPosY );

		LuaTable tbNpcData( GLWidgetScript::GetInstance().GetLuaState() );
		tbNpcData.set( "strName", pCrowData->GetName() );
		tbNpcData.set( "x", nPosX );
		tbNpcData.set( "y", nPosY );
		tbNpcData.set( "bMarket", true );
		tbNpcData.set( "CrowType", (int)pCrowData->m_sBasic.m_emCrow );
		tbNpcData.set( "ID", (int)vecCrow[ i ]->m_CrowID.dwID );

		tbData.set( (*pnTableIndex)++, tbNpcData );
	}
}

void GLMinimapClient::SetMobDataInTable( const GLLevelFileClient* pLevelFile, LuaTable& tbData,
										 std::vector< GLMobSchedule* > vecCrow )
{
	const GLMapAxisInfo& sMapAxisInfo = pLevelFile->GetMapAxisInfo();

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();

	int nTableIndex = 1;
	for( size_t i = 0; i < vecCrow.size(); ++i )
	{
		SCROWDATA* pCrowData = GLCrowDataMan::GetInstance().GetCrowData( vecCrow[ i ]->m_CrowID );

		DWORD dwMobStrong(0);

		const GLQuestPlay::QUESTMOB_MAP& mapQuestMob = pChar->m_cQuestPlay.GetQuestMobProc();
		GLQuestPlay::QUESTMOB_MAP::const_iterator iter = mapQuestMob.find( pCrowData->GetId().dwID );
		if( iter != mapQuestMob.end() )
			dwMobStrong = (DWORD)MONSTER_TYPE_QUEST;

		dwMobStrong |= (DWORD)GetMobStrongType( pCrowData );

		int nPosX = 0;
		int nPosY = 0;
		ConvertWorldToMap( sMapAxisInfo, vecCrow[ i ]->m_pAffineParts->vTrans, nPosX, nPosY );

		LuaTable tbMobData( GLWidgetScript::GetInstance().GetLuaState() );
		tbMobData.set( "dwID", pCrowData->GetId().dwID );
		tbMobData.set( "strName", pCrowData->GetName() );
		tbMobData.set( "CrowType", pCrowData->m_sBasic.m_emCrow );
		tbMobData.set( "ID", pCrowData->m_sBasic.sNativeID.dwID );
		tbMobData.set( "nType", (INT)dwMobStrong );
		tbMobData.set( "x", nPosX );
		tbMobData.set( "y", nPosY );

		tbData.set( nTableIndex++, tbMobData );
	}
}