#include "pch.h"

#include "../../RanLogic/Msg/GLContrlInstanceContentsMsg.h"

#include "../GLGaeaClient.h"
#include "../Land/GLLandManClient.h"
#include "../Minimap/GLMinimapClient.h"
#include "../Club/GLClubClient.h"
#include "../Widget/GLWidgetScript.h"

#include "InstanceSystemClient.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace InstanceSystem
{
    const bool CALLBACK ManagerClient::_msgProc_Guidance_UI_Auth(const NET_MSG_GENERIC* const _pMessage)
    {
        const GLMSG::NET_INSTANCE_GUIDANCE_UI_AUTH_FC* const pMsg = (const GLMSG::NET_INSTANCE_GUIDANCE_UI_AUTH_FC* const) _pMessage;

		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_CTI_Auth,
			"-b, -f", pMsg->bVisible, pMsg->fAuthTime );

        return true;
    }

    const bool CALLBACK ManagerClient::_msgProc_Guidance_UI_Progress(const NET_MSG_GENERIC* const _pMessage)
    {
		 GLMSG::NET_INSTANCE_GUIDANCE_UI_PROGRESS_FC* pMsg = (GLMSG::NET_INSTANCE_GUIDANCE_UI_PROGRESS_FC*) _pMessage;

		 GLWidgetScript::GetInstance().LuaCallFunc< void >(
			 NSWIDGET_SCRIPT::g_strFunc_CTI_UpdateCountryProgressInfo,
			 "-n, -n, -n, -f, -n",
			 pMsg->iNameIndex, pMsg->iProgressIndex, pMsg->iPoint, pMsg->fRemainTime, (int) COMPETITION_GUIDANCE );

        return true;
    }

    const bool CALLBACK ManagerClient::_msgProc_Guidance_UI_Rank(const NET_MSG_GENERIC* const _pMessage)
    {
		GLMSG::NET_INSTANCE_GUIDANCE_UI_RANK_FC netMsg;
		if( CastNetMsg( _pMessage, netMsg ) == false )
			return false;

		const GLMSG::NET_GUIDANCE_RANK_VEC& vecRank = netMsg.rankVector;

		LuaTable tbRankList( GLWidgetScript::GetInstance().GetLuaState() );

		GLMSG::NET_GUIDANCE_RANK_VEC::const_iterator iter = vecRank.begin();
		for( int i = 1; iter != vecRank.end(); ++iter, ++i )
		{
			LuaTable tbRank( GLWidgetScript::GetInstance().GetLuaState() );
			tbRank.set( 1, iter->iRank );
			tbRank.set( 2, iter->clubName.c_str() );
			tbRank.set( 3, iter->iPoint );
			tbRankList.set( i, tbRank );
		}

		GLWidgetCallFuncValue args[ 2 ];
		args[ 0 ].SetTable( &tbRankList );
		args[ 1 ].SetInt( (int) COMPETITION_GUIDANCE );

		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_CTI_UpdateRank, args, 2 );

        return true;
    }

    const bool CALLBACK ManagerClient::_msgProc_Guidance_UI_Rank_MyInfo(const NET_MSG_GENERIC* const _pMessage)
    {
		GLMSG::NET_INSTANCE_GUIDANCE_UI_RANK_MY_INFO_FC* pMsg = (GLMSG::NET_INSTANCE_GUIDANCE_UI_RANK_MY_INFO_FC*) _pMessage;

		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_CTI_UpdateMyRank,
			"-n, -n", pMsg->iMyClubRank, pMsg->iMyClubPoint );

        return true;
    }

    const bool CALLBACK ManagerClient::_msgProc_Guidance_UI_AuthInfo(const NET_MSG_GENERIC* const _pMessage)
    {
		GLMSG::NET_INSTANCE_GUIDANCE_UI_MINIMAP_FC netMsg;
		if( CastNetMsg( _pMessage, netMsg ) == false )
			return false;

		GLLandManClient* pLandClient = pGaeaClient->GetActiveMap();
		if( NULL == pLandClient )
			return false;

		if ( netMsg.dwGuidanceMapID != pLandClient->GetMapID().getBaseMapID().dwID )
			return true;

		switch ( netMsg.dwIconType )
		{
		case 0:
			{
				enum
				{
					NONE = 0,
					MINE,
					NEUTRALITY,
					OTHER,
					ACCELERATOR,
				};

				LuaTable tbList( GLWidgetScript::GetInstance().GetLuaState() );

				const GLMapAxisInfo& sMapAxisInfo = pLandClient->GetMapAxisInfo();

				int nTableIndex = 1;
				for( size_t i = 0; i < netMsg.vecAuthClub.size(); ++i )
				{
					int nType = -1;
					if( CLUB_NULL == netMsg.vecAuthClub[ i ].clubID )
					{
						nType = NONE;
					}
					else if( pGaeaClient->GetMyClub()->DbNum() == netMsg.vecAuthClub[ i ].clubID )
					{
						nType = MINE;
					}
					else
					{
						nType = OTHER;

						// 동맹클럽일 경우 중립으로 바꾼다;
						if ( netMsg.bAllianceClub )
						{
							std::vector< GLCLUBALLIANCE > vecAllianceClub;
							pGaeaClient->GetClubAlliance( vecAllianceClub );
							for ( int j=0; j<vecAllianceClub.size(); ++j )
							{
								GLCLUBALLIANCE sAllianceClub = vecAllianceClub.at( j );
								if ( sAllianceClub.m_dwID == netMsg.vecAuthClub[ i ].clubID )
									nType = NEUTRALITY;
							}
						}
					}

					int nPosX = 0, nPosY = 0;
					GLMinimapClient::GetInstance().ConvertWorldToMap(
						sMapAxisInfo, 
						D3DXVECTOR3( netMsg.vecAuthClub[ i ].fXpos, 0.0f, netMsg.vecAuthClub[ i ].fZpos ),
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
					NSWIDGET_SCRIPT::g_strFunc_Guidance_MinimapAuthPos, &arg, 1 );
			}
			break;

		case 1:
			{
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
				for( size_t i = 0; i < netMsg.vecAuthClub.size(); ++i )
				{
					int nType = -1;
					if( Country::UNKNOWN_COUNTRY_INFO == netMsg.vecAuthClub[ i ].clubID )
					{
						nType = NONE;
					}
					else if( netMsg.vecAuthClub[ i ].clubID == sCountryInfo.dwID )
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
						D3DXVECTOR3( netMsg.vecAuthClub[ i ].fXpos, 0.0f, netMsg.vecAuthClub[ i ].fZpos ),
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
			}
			break;
		}

        return true;
    }
}