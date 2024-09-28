#include "pch.h"
#include "../../RanLogicClient/Transport/GLBusStationClient.h"

#include "../../RanLogicClient/GLGaeaClient.h"
#include "../../RanLogic/Transport/GLBusStation.h"
#include "../Land/GLLandManClient.h"

#include "../RanGfxUI/GfxInterface.h"
#include "../../RanLogicClient/NPCTalk/GLNpcTalkClient.h"

GLBusStationClient::GLBusStationClient(void)
:m_dwNPCGlobalID(0), m_eCrow( CROW_NPC )
{

}

GLBusStationClient::GLBusStationClient( const GLBusStationClient& value )
:m_dwNPCGlobalID(0)
{

}

GLBusStationClient::~GLBusStationClient(void)
{

}

GLBusStationClient& GLBusStationClient::GetInstance ()
{
	static GLBusStationClient _instance;
	return _instance;
}

const bool GLBusStationClient::OpenBusStation( /*const SNATIVEID& sNPCID,*/
											   const DWORD dwNPCGlobalID,
											   const DWORD dwTalkID,
											   std::tr1::shared_ptr<SNpcTalk> spNpcTalk,
											   bool bToolMode )
{
	if (!spNpcTalk)
		return false;

	if( spNpcTalk->m_strBusFile.empty() )
	{
		//m_pGlBusData = &GLBusStation::GetInstance();
		GASSERT( 0 && "CBusWindow::SetBusData(), m_strBusFile is empty!" );
	}
	else
	{
		if (!m_glBusList.LOAD(spNpcTalk->m_strBusFile, bToolMode))
		{
			GASSERT(0 && "CBusWindow::SetBusData(), m_glBusList.LOAD()");
			return false;
		}
		m_eCrow			= GLNpcTalkClient::GetInstance().GetTalkCrow();
		m_dwNPCGlobalID = dwNPCGlobalID;
		GLWidgetScript::GetInstance().LuaCallFunc< bool >( NSWIDGET_SCRIPT::g_strFunc_BusStation_Open );
	}

	return true;
}

const DWORD GLBusStationClient::GetMapInfoSize(void)
{
	return m_glBusList.GetVillageNum();
}

lua_tinker::table GLBusStationClient::GetMapInfo(void)
{
	lua_tinker::table tbBusMapInfo( GLWidgetScript::GetInstance().GetLuaState() );
	
	const DWORD dwNum = m_glBusList.GetVillageNum();
	
	for ( DWORD i = 0; i < dwNum; i++ )
	{
		SVILLAGE* pVILLAGE = m_glBusList.GetVillage(i);
		if ( !pVILLAGE )
		{
			GASSERT ( 0 && "VILLAGE를 찾을수 없습니다." );
			continue;
		}
		
		// 맵 데이터 입력
		lua_tinker::table tbMapInfo( GLWidgetScript::GetInstance().GetLuaState() );
		tbMapInfo.set("dwMAPID", pVILLAGE->dwMAPID);
		tbMapInfo.set("strMAP", pVILLAGE->strMAP );
		
		DWORD nStationInfo = pVILLAGE->vecSTATION.size();
		tbMapInfo.set("nStationInfo", nStationInfo );

		for ( DWORD j = 0; j < nStationInfo; j++ )
		{
			DWORD dwID = pVILLAGE->GetStation(j);
			const SSTATION* pSTATION = m_glBusList.GetStation(dwID);
			if ( !pSTATION )
			{
				GASSERT ( 0 && "STATION을 찾을 수 없습니다." );
				continue;
			}
			
			// 정류장 데이터 입력
			lua_tinker::table tbStationInfo( GLWidgetScript::GetInstance().GetLuaState() );
			
			tbStationInfo.set("dwMAPID", pSTATION->dwMAPID);
			tbStationInfo.set("dwGATEID", pSTATION->dwGATEID);
			tbStationInfo.set("strMAP", pSTATION->strMAP);
			tbStationInfo.set("strSTATION", pSTATION->strSTATION);
			tbStationInfo.set("dwLINKID", pSTATION->dwLINKID);
			tbStationInfo.set("dwProbability", pSTATION->dwProbability);

			tbMapInfo.set( j + 1, tbStationInfo );
		}	
		
		tbBusMapInfo.set( i + 1, tbMapInfo );
	}
	
	return tbBusMapInfo;
}

const bool GLBusStationClient::ReqBusStation( const DWORD dwStationID )
{
	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();
	if(pClient)
	{
		pClient->GetCharacter()->ReqBusStation( GLNpcTalkClient::GetInstance().GetNpcGlobalID(), dwStationID, GLNpcTalkClient::GetInstance().GetTalkCrow() );
		return true;
	}
	return false;
	//const DWORD dwStationID = m_pStopList->GetTextData ( m_nSelectStop );
}

const bool GLBusStationClient::IsUseable()
{
	GLGaeaClient* pClient = GfxInterfaceBase::GetInstance()->GetGaeaClient();

	GLLandManClient* pLandMan = pClient->GetActiveMap();

	if ( !pLandMan ) return false;

	const GLCharacter* const pChar = pClient->GetCharacter();

	if ( pChar == NULL ) return false;

	const D3DXVECTOR3 vCharPos( pChar->GetPosition() );
	const float fCharRadius( pChar->GETBODYRADIUS() );

	switch( m_eCrow )
	{
	case CROW_SUMMON :
		{
			std::tr1::shared_ptr< GLSummonClient > spCrow = pLandMan->GetSummon( m_dwNPCGlobalID );

			if( NULL == spCrow || CROW_SUMMON != spCrow->GETCROW() )
				return false;

			if ( spCrow->IsSummonNpcActionable(vCharPos, fCharRadius) == false )
				return false;
		}
		break;

	case CROW_NPC :
		{
			std::tr1::shared_ptr< GLCrowClient > spCrow = pLandMan->GetCrow( m_dwNPCGlobalID );

			if( NULL == spCrow || CROW_NPC != spCrow->GETCROW() )
				return false;

			if ( spCrow->IsNpcActionable(vCharPos, fCharRadius) == false )
				return false;
		}
		break;
	}

	return true;
}