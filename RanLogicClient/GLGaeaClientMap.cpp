
#include "pch.h"

#include "./GLGaeaClient.h"

#include "../../RanLogic/Msg/MapMsg.h"
#include "./Land/GLLandManClient.h"

#include "../enginelib/GUInterface/GameTextControl.h"
#include "../enginelib/GUInterface/UITextControl.h"
#include "./Widget/GLWidgetScript.h"
#include "./Level/GLLevelFileClient.h"
#include "./Minimap/GLMinimapClient.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace
{
	int s_mapNameCount = 0;
    std::vector<std::pair<int, std::string> > s_mapNameVec;
	//////////////////////////////////////////////////////////////////////////
	int   s_mapPartyInfoListBoardCount = 0;
	bool  s_mapPartyInfoListBoardIsExitMyBoard = false;
	DWORD s_mapPartyInfoListBoardMapID = (DWORD)-1;

	std::vector<std::string> s_mapPartyInfoListBoardNameVec;
	std::vector<DWORD>       s_mapPartyInfoListBoardMaxPlayerVec;
	std::vector<DWORD>       s_mapPartyInfoListBoardCurrentPlayerVec;
	std::vector<std::string> s_mapPartyInfoListBoardTitleVec;

	//////////////////////////////////////////////////////////////////////////
	int   s_mapPartyInfoDetailCount = 0;
	DWORD s_mapPartyInfoDetailMapID = (DWORD)-1;
	std::vector<GLPARTY_LINK_MEM> s_mapPartyInfoDetailMemberVec;

}

void GLGaeaClient::MsgMapSearchNameAck1(NET_MSG_GENERIC* nmg)
{
	GLMSG::NET_MAP_SEARCH_NAME_1_ACK_AC* pNetMsg = (GLMSG::NET_MAP_SEARCH_NAME_1_ACK_AC*)nmg;
	s_mapNameCount = pNetMsg->m_NameCount;
	s_mapNameVec.clear();

	GASSERT(s_mapNameCount >=0);

	if( s_mapNameCount == 0 )
	{

	}
}

void GLGaeaClient::MsgMapSearchNameAck2(NET_MSG_GENERIC* nmg)
{
	NET_MSG_PACK* pPacket = (NET_MSG_PACK*) nmg;
    GLMSG::NET_MAP_SEARCH_NAME_2_ACK_AC Data;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), Data))
        return;

    s_mapNameVec.push_back(std::pair<int, std::string> (Data.m_Level, Data.m_ChaName));
	--s_mapNameCount;
	if (s_mapNameCount == 0)
	{
		LuaTable tbResult( GLWidgetScript::GetInstance().GetLuaState() );

		int nTableIndex = 1;
		std::vector< std::pair< int, std::string > >::iterator iter = s_mapNameVec.begin();
		for( ; iter != s_mapNameVec.end(); ++iter )
		{
			LuaTable tbChar( GLWidgetScript::GetInstance().GetLuaState() );
			tbChar.set( "nLevel", iter->first );
			tbChar.set( "strName", iter->second.c_str() );
			tbResult.set( nTableIndex++, tbChar );
		}

		VEC_WIDGET_CALLFUNCVALUE vecValue;

		GLWidgetCallFuncValue sValue;
		sValue.SetTable( &tbResult );
		vecValue.push_back( sValue );

		GLWidgetScript::GetInstance().LuaCallFunc< void >( 
			NSWIDGET_SCRIPT::g_strFunc_LargeMap_SearchCharResult, vecValue );
	}
}

void GLGaeaClient::MsgMapSearchNameDetailInfo(NET_MSG_GENERIC* nmg)
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) nmg;
    GLMSG::NET_MAP_SEARCH_NAME_DETAIL_INFO_ACK_AC Data;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), Data))
        return;
	// params : int nLevel, int nSchool, int nClass, DWORD dwGaeaID, bool bParty,
	//			string strName, string strTitle, strClubName
	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_LargeMap_SearchCharDeatil,
		"-n, -n, -n, -dw, -b, -s, -s, -s",
		Data.m_Level, Data.m_wSchool, Data.m_emClass, Data.m_dwGaeaID,
		Data.m_isParty, Data.m_ChaName.c_str(), Data.m_Title.c_str(), Data.m_ClubName.c_str() );
}

void GLGaeaClient::MsgMapSearchNamePosition(NET_MSG_GENERIC* nmg)
{
	GLMSG::NET_MAP_SEARCH_NAME_POSITION_ACK_AC* pNetMsg = (GLMSG::NET_MAP_SEARCH_NAME_POSITION_ACK_AC*)nmg;
	pNetMsg->m_szChaName[CHR_ID_LENGTH] = '\0';

	GLLevelFileClient* pLevelFile = GLMinimapClient::GetInstance().GetLevelClient( pNetMsg->m_mapID );
	if( NULL == pLevelFile || true == pLevelFile->IsInstantMap() )
		return;

	int nPosX = 0;
	int nPosY = 0;

	const GLMapAxisInfo& sMapAxisInfo = pLevelFile->GetMapAxisInfo();
	GLMinimapClient::GetInstance().ConvertWorldToMap( sMapAxisInfo, pNetMsg->m_vPosition, nPosX, nPosY );

	LuaTable tbMapPos( GLWidgetScript::GetInstance().GetLuaState() );
	tbMapPos.set( "nMapID", pNetMsg->m_mapID );
	tbMapPos.set( "nPosX", nPosX );
	tbMapPos.set( "nPosY", nPosY );

	GLWidgetCallFuncValue sValue;
	sValue.SetTable( &tbMapPos );

	VEC_WIDGET_CALLFUNCVALUE vecValue;
	vecValue.push_back( sValue );

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_LargeMap_SearchCharMapPos, vecValue );
}



void GLGaeaClient::MsgMapPartyInfoListBoard_1(NET_MSG_GENERIC* nmg)
{
	GLMSG::NET_MAP_PARTY_INFO_LIST_BOARD_1_ACK_AC* pNetMsg = (GLMSG::NET_MAP_PARTY_INFO_LIST_BOARD_1_ACK_AC*)nmg;
	s_mapPartyInfoListBoardCount = pNetMsg->m_Count;
	s_mapPartyInfoListBoardIsExitMyBoard = pNetMsg->m_isExistMyBoard;
	s_mapPartyInfoListBoardMapID = pNetMsg->m_MapID;

	s_mapPartyInfoListBoardNameVec.clear();
	s_mapPartyInfoListBoardMaxPlayerVec.clear();
	s_mapPartyInfoListBoardCurrentPlayerVec.clear();
	s_mapPartyInfoListBoardTitleVec.clear();

	//!! SF_TODO
	/*if( s_mapPartyInfoListBoardCount == 0 )
	{
		m_pInterface->MsgMapPartyInfoListBoard(s_mapPartyInfoListBoardIsExitMyBoard,s_mapPartyInfoListBoardMapID,
			s_mapPartyInfoListBoardNameVec,s_mapPartyInfoListBoardMaxPlayerVec,
			s_mapPartyInfoListBoardCurrentPlayerVec,s_mapPartyInfoListBoardTitleVec);
	}*/
}

void GLGaeaClient::MsgMapPartyInfoListBoard_2(NET_MSG_GENERIC* nmg)
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) nmg;
    GLMSG::NET_MAP_PARTY_INFO_LIST_BOARD_2_ACK_AC Data;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), Data))
        return;

	s_mapPartyInfoListBoardNameVec.push_back(Data.m_ChaName);
	s_mapPartyInfoListBoardMaxPlayerVec.push_back(Data.m_MaxPlayer);
	s_mapPartyInfoListBoardCurrentPlayerVec.push_back(Data.m_CurrentPlayer);
	s_mapPartyInfoListBoardTitleVec.push_back(Data.m_Title);

	s_mapPartyInfoListBoardCount--;
	//!! SF_TODO
	/*if (s_mapPartyInfoListBoardCount == 0)
	{
		m_pInterface->MsgMapPartyInfoListBoard(
			s_mapPartyInfoListBoardIsExitMyBoard,
			s_mapPartyInfoListBoardMapID,
			s_mapPartyInfoListBoardNameVec,
			s_mapPartyInfoListBoardMaxPlayerVec,
			s_mapPartyInfoListBoardCurrentPlayerVec,
			s_mapPartyInfoListBoardTitleVec);
	}*/
}

void GLGaeaClient::MsgMapPartyInfoDetail_1(NET_MSG_GENERIC* nmg)
{
	GLMSG::NET_MAP_PARTY_INFO_DETAIL_1_ACK_AC* pNetMsg = (GLMSG::NET_MAP_PARTY_INFO_DETAIL_1_ACK_AC*)nmg;

	s_mapPartyInfoDetailMemberVec.clear();

	s_mapPartyInfoDetailCount = pNetMsg->m_Count;
	s_mapPartyInfoDetailMapID = pNetMsg->m_MapID;

	//!! SF_TODO
	/*if( s_mapPartyInfoDetailCount == 0 )
	{
		m_pInterface->MsgMapPartyInfoDetail(s_mapPartyInfoListBoardMapID,s_mapPartyInfoDetailMemberVec);
	}*/
}

void GLGaeaClient::MsgMapPartyInfoDetail_2(NET_MSG_GENERIC* nmg)
{
    GLMSG::NET_MAP_PARTY_INFO_DETAIL_2_ACK_AC* pPacket = (GLMSG::NET_MAP_PARTY_INFO_DETAIL_2_ACK_AC*) nmg;
		
	s_mapPartyInfoDetailMemberVec.push_back(pPacket->m_PartyMember);
	s_mapPartyInfoDetailCount--;

	//!! SF_TODO
	/*if (s_mapPartyInfoDetailCount == 0)
	{
		m_pInterface->MsgMapPartyInfoDetail(
			s_mapPartyInfoListBoardMapID,
			s_mapPartyInfoDetailMemberVec);
	} */
}

void GLGaeaClient::ReqSearchCharacter( const char* szName, bool bCurMap, DWORD dwCurMapID )
{
	std::string strName = szName;
	if( true == strName.empty() )
		return;

	if( 3 > strName.size() )
	{
		PrintMsgText( NS_UITEXTCOLOR::RED, ID2GAMEWORD( "MAP_SHORT_SEARCH_EXPLAIN" ) );
		return;
	}

	DWORD dwMapID = SNATIVEID( false ).Id();
	if( true == bCurMap )
		dwMapID = dwCurMapID;

	GLMSG::NET_MAP_SEARCH_NAME_REQ_CF msg;
	msg.SetName( strName );
	msg.m_mapID = dwMapID;
	NETSENDTOFIELD( &msg );
}

void GLGaeaClient::ReqSearchCharacterDetailInfo( const char* szName )
{
	std::string strName = szName;
	if( true == strName.empty() )
		return;

	GLMSG::NET_MAP_SEARCH_NAME_DETAIL_INFO_REQ_CA msg;
	strncpy_s( msg.m_szChaName, sizeof( msg.m_szChaName ), strName.c_str(), strName.size() );
	NETSENDTOAGENT( &msg );
}

void GLGaeaClient::ReqSearchCharacterPosition( const char* szName )
{
	std::string strName = szName;
	if( true == strName.empty() )
		return;

	GLMSG::NET_MAP_SEARCH_NAME_POSITION_REQ_CA msg;
	strncpy_s( msg.m_szChaName, sizeof( msg.m_szChaName ), strName.c_str(), strName.size() );
	NETSENDTOAGENT( &msg );
}

void GLGaeaClient::MsgLevelLayerFC( NET_MSG_GENERIC* pMsg )
{
	if ( !m_pLandMClient )
		return;

	GLMSG::SNET_LEVEL_LAYER_FC* pPacket = (GLMSG::SNET_LEVEL_LAYER_FC*) pMsg;
	switch ( pPacket->m_emActionType )
	{
	case GLMSG::SNET_LEVEL_LAYER_FC::LEVEL_LAYER_ACTIVE:
		m_pLandMClient->SetActiveLayer( pPacket->m_unLayerIndex );
		break;

	case GLMSG::SNET_LEVEL_LAYER_FC::LEVEL_LAYER_ONE_DEACTIVE:
		m_pLandMClient->SetDeActiveLayer( pPacket->m_unLayerIndex );
		break;

	case GLMSG::SNET_LEVEL_LAYER_FC::LEVEL_LAYER_ALL_DEACTIVE:
		m_pLandMClient->SetDeActiveAllLayer();
		break;

	default:
		break;
	};
}
