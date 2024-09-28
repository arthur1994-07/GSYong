#include "pch.h"

#include "../../RanLogic/Msg/GLContrlInstanceContentsMsg.h"
#include "../GLGaeaClient.h"
#include "../Land/GLLandManClient.h"
#include "../Minimap/GLMinimapClient.h"
#include "../Widget/GLWidgetScript.h"

#include "InstanceSystemClient.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace InstanceSystem
{
	const bool CALLBACK ManagerClient::_msgProc_CTI_UI_Auth ( const NET_MSG_GENERIC* const pMsg )
	{
		const GLMSG::NET_INSTANCE_CTI_UI_AUTH_FC* const pAuthMsg =
			static_cast< const GLMSG::NET_INSTANCE_CTI_UI_AUTH_FC* const >( pMsg );

		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_CTI_Auth,
			"-b, -f", pAuthMsg->bVisible, pAuthMsg->fAuthTime );

		return true;
	}

	const bool CALLBACK ManagerClient::_msgProc_CTI_UI_Progress ( const NET_MSG_GENERIC* const pMsg )
	{
		GLMSG::NET_INSTANCE_GUIDANCE_UI_PROGRESS_FC* pNetMsg = (GLMSG::NET_INSTANCE_GUIDANCE_UI_PROGRESS_FC*) pMsg;

		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_CTI_UpdateCountryProgressInfo,
			"-n, -n, -n, -f, -n",
			pNetMsg->iNameIndex, pNetMsg->iProgressIndex, pNetMsg->iPoint, pNetMsg->fRemainTime, (int) COMPETITION_CTI );

		return true;
	}

	const bool CALLBACK ManagerClient::_msgProc_CTI_UI_Rank ( const NET_MSG_GENERIC* const pMsg )
	{
		GLMSG::NET_INSTANCE_CTI_UI_RANK_FC netMsg;
		if( false == CastNetMsg( pMsg, netMsg ) )
			return false;

		const GLMSG::NET_CTI_RANK_VEC& vecRank = netMsg.rankVector;

		LuaTable tbRankList( GLWidgetScript::GetInstance().GetLuaState() );

		GLMSG::NET_CTI_RANK_VEC::const_iterator iter = vecRank.begin();
		for( int i = 1; iter != vecRank.end(); ++iter, ++i )
		{
			LuaTable tbRank( GLWidgetScript::GetInstance().GetLuaState() );
			tbRank.set( 1, iter->iRank );
			tbRank.set( 2, iter->countryName );
			tbRank.set( 3, iter->iPoint );
			tbRankList.set( i, tbRank );
		}

		GLWidgetCallFuncValue args[ 2 ];
		args[ 0 ].SetTable( &tbRankList );
		args[ 1 ].SetInt( (int) COMPETITION_CTI );

		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_CTI_UpdateRank, args, 2 );

		return true;
	}

	const bool CALLBACK ManagerClient::_msgProc_CTI_UI_Rank_MyInfo ( const NET_MSG_GENERIC* const pMsg )
	{
		const GLMSG::NET_INSTANCE_CTI_UI_RANK_MY_INFO_FC* pNetMsg = 
			static_cast< const GLMSG::NET_INSTANCE_CTI_UI_RANK_MY_INFO_FC* >( pMsg );

		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_CTI_UpdateMyRank,
			"-n, -n", pNetMsg->iMyCountryRank, pNetMsg->iMyCountryPoint );

		return true;
	}

	const bool CALLBACK ManagerClient::_msgProc_CTI_UI_AuthInfo ( const NET_MSG_GENERIC* const pMsg )
	{
		GLMSG::NET_INSTANCE_CTI_UI_MINIMAP_FC netMsg;
		if( false == CastNetMsg( pMsg, netMsg ) )
			return false;

		GLLandManClient* pLandClient = pGaeaClient->GetActiveMap();
		if( NULL == pLandClient )
			return false;

		enum
		{
			NONE = 0,
			MINE,
			OTHER,
			ACCELERATOR,
		};

		LuaTable tbList( GLWidgetScript::GetInstance().GetLuaState() );

		const GLMapAxisInfo& sMapAxisInfo = pLandClient->GetMapAxisInfo();
		const Country::SCOUNTRY_INFO& sCountryInfo = pGaeaClient->GetCountryInfo();

		int nTableIndex = 1;
		for( size_t i = 0; i < netMsg.vecAuthCountry.size(); ++i )
		{
			int nType = -1;
			if( true == netMsg.vecAuthCountry[ i ].sCountryInfo.ISNONCOUNTRY() )
			{
				nType = NONE;
			}
			else if( netMsg.vecAuthCountry[ i ].sCountryInfo == sCountryInfo )
			{
				nType = MINE;
			}
			else
			{
				nType = OTHER;
			}

			int nPosX = 0, nPosY = 0;
			GLMinimapClient::GetInstance().ConvertWorldToMap(
				sMapAxisInfo, 
				D3DXVECTOR3( netMsg.vecAuthCountry[ i ].fXpos, 0.0f, netMsg.vecAuthCountry[ i ].fZpos ),
				nPosX, nPosY );

			LuaTable tbData( GLWidgetScript::GetInstance().GetLuaState() );
			tbData.set( 1, nType );
			tbData.set( 2, nPosX );
			tbData.set( 3, nPosY );
			tbList.set( nTableIndex++, tbData );
		}

		for( size_t i = 0; i < netMsg.vecAccelerator.size(); ++i )
		{
			int nPosX = 0, nPosY = 0;
			GLMinimapClient::GetInstance().ConvertWorldToMap(
				sMapAxisInfo, 
				D3DXVECTOR3( netMsg.vecAccelerator[ i ].fXpos, 0.0f, netMsg.vecAccelerator[ i ].fZpos ),
				nPosX, nPosY );

			LuaTable tbData( GLWidgetScript::GetInstance().GetLuaState() );
			tbData.set( 1, (int) ACCELERATOR );
			tbData.set( 2, nPosX );
			tbData.set( 3, nPosY );
			tbList.set( nTableIndex++, tbData );
		}

		GLWidgetCallFuncValue arg;
		arg.SetTable( &tbList );

		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_CTI_MinimapAuthPos, &arg, 1 );

		return true;
	}
}
