#include "pch.h"
#include "GLLobyCharClient.h"

#include "../../enginelib/DxTools/RENDERPARAM.h"
#include "../../enginelib/DxTools/DebugSet.h"
#include "../../RanLogic/RANPARAM.h"
#include "../../RanLogic/GLogicDataMan.h"

#include "../GLGaeaClient.h"
#include "../Stage/DxGlobalStage.h"
#include "../Stage/DxLobyStage.h"

GLLobyCharClient::GLLobyCharClient(void)
{
}

GLLobyCharClient::GLLobyCharClient( const GLLobyCharClient& value )
{
}

GLLobyCharClient::~GLLobyCharClient(void)
{
}

GLLobyCharClient& GLLobyCharClient::GetInstance()
{
	static GLLobyCharClient Instance;
	return Instance;
}

void GLLobyCharClient::MsgProcess( NET_MSG_GENERIC* nmg )
{
	switch( nmg->nType )
	{
	case NET_MSG_CHA_DEL_FB_OK :
		{
			NET_CHA_DEL_FB* pCharDelFb = (NET_CHA_DEL_FB*) nmg;

			DxGlobalStage* pGlobalStage = GLWidgetScript::GetInstance().m_pGaeaClient->GetGlobalStage();
			pGlobalStage->GetLobyStage()->DelChar( pCharDelFb->nChaNum );

			GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_CharSelect_SucceedDelChar );
		}
		break;

	case NET_MSG_LOBBY_GAME_COMPLETE:
		{
			CDebugSet::ToListView ( "NET_MSG_LOBBY_GAME_COMPLETE" );

			DxGlobalStage* pGlobalStage = GLWidgetScript::GetInstance().m_pGaeaClient->GetGlobalStage();
			if ( pGlobalStage )
			{
				pGlobalStage->ToChangeStage(DxGlobalStage::EM_STAGE_GAME);
			}
			else
			{
				sc::writeLogError( "Failed Join Game...!!!" );
			}
		}
		break;

	case NET_MSG_LOBBY_CHAR_JOIN_FB :
		{
			GLMSG::SNETLOBBY_CHARJOIN_FB* pNetMsg = (GLMSG::SNETLOBBY_CHARJOIN_FB*) nmg;

			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_CharSelect_JoinFail,
				"-n", pNetMsg->emCharJoinFB );
		}
		break;

	case NET_MSG_LOBBY_CHARJOIN_2NDPASS :
		{
			GLMSG::SNETLOBBY_CHARJOIN_2NDPASS* pNetMsg = (GLMSG::SNETLOBBY_CHARJOIN_2NDPASS*) nmg;

			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_SecondPassword_Result,
				"-n", pNetMsg->nResult );
		}
		break;

	case NET_MSG_CHA_DEL_FB_CLUB :
		{
			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_CharSelect_FailDelChar,
				"-b", true );
		}
		break;

	case NET_MSG_CHA_DEL_FB_ERROR :
		{
			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_CharSelect_FailDelChar,
				"-b", false );
		}
		break;

	case NET_MSG_LOBBY_CHINA_ERROR :
		{
			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_LoginGainTime );
		}
		break;

	case NET_MSG_PASSCHECK_FB:
		break;

	case JAPAN_NET_MSG_UUID:
		{
			JAPAN_NET_LOGIN_UUID* netMsg = (JAPAN_NET_LOGIN_UUID*) nmg;
			RANPARAM::SETJAPANUUID( netMsg->szUUID );
		}
		break;
	}
}

lua_tinker::table GLLobyCharClient::GetLobyCharInfoTable()
{
	DxGlobalStage* pGlobalStage = GLWidgetScript::GetInstance().m_pGaeaClient->GetGlobalStage();

	int nCharListCount = 1;
	lua_tinker::table tbCharList( GLWidgetScript::GetInstance().GetLuaState() );
	
	const int nCharNum = pGlobalStage->GetLobyStage()->GetChaSNum();
	for( int i = 0; i < nCharNum; ++i )
	{
		SCHARINFO_LOBBY* pCharInfo = pGlobalStage->GetLobyStage()->GetCharInfo( i );
		if( NULL == pCharInfo )
			continue;

		float fExpRatio = float( pCharInfo->m_sExperience.lnNow ) / float( pCharInfo->m_sExperience.lnMax );
		float fAtbRatio = float( pCharInfo->m_nBright );

		lua_tinker::table tbCharInfo( GLWidgetScript::GetInstance().GetLuaState() );
		tbCharInfo.set( 1, pCharInfo->m_ChaName.c_str() );
		tbCharInfo.set( 2, pCharInfo->m_wLevel );
		tbCharInfo.set( 3, pCharInfo->m_wSchool );
		tbCharInfo.set( 4, pCharInfo->m_emClass );
		tbCharInfo.set( 5, GLWidgetScript::GetInstance().m_pGaeaClient->GetMapName( pCharInfo->m_sSaveMapID ) );
		tbCharInfo.set( 6, fExpRatio );
		tbCharInfo.set( 7, fAtbRatio );
		tbCharInfo.set( 8, pCharInfo->m_ClubName.c_str() );
		tbCharInfo.set( 9, pCharInfo->m_ClubRank );
		tbCharInfo.set( 10, true == pCharInfo->m_bRanMobile ? 1 : 0 );
		tbCharInfo.set( 11, pCharInfo->m_dwCharID );

		tbCharList.set( nCharListCount, tbCharInfo );
		++nCharListCount;
	}

	return tbCharList;
}

void GLLobyCharClient::SelectLobyChar( int nSelectPage, int nSelectNum, int nIndex )
{
	DxGlobalStage* pGlobalStage = GLWidgetScript::GetInstance().m_pGaeaClient->GetGlobalStage();
	
	SCHARINFO_LOBBY* pCharInfo = pGlobalStage->GetLobyStage()->GetCharInfo( nIndex );
	if( NULL == pCharInfo )
		return;

	pGlobalStage->GetGameStage()->SetSelectChar( pCharInfo );

	pGlobalStage->GetLobyStage()->ShiftCharIndex( nSelectPage );
	pGlobalStage->GetLobyStage()->SelectCharacter( nSelectNum );
}

int GLLobyCharClient::Get2ndPassType()
{
	return GLWidgetScript::GetInstance().m_pGaeaClient->GetUse2ndPass();
}

bool GLLobyCharClient::JoinGame( int nIndex )
{
	DxGlobalStage* pGlobalStage = GLWidgetScript::GetInstance().m_pGaeaClient->GetGlobalStage();

	SCHARINFO_LOBBY* pCharInfo = pGlobalStage->GetLobyStage()->GetCharInfo( nIndex );
	if( NULL == pCharInfo )
		return false;

	pGlobalStage->GetGameStage()->SetSelectChar( pCharInfo );

	if( NULL == pCharInfo || true == pCharInfo->IsLock() )
	{
		pGlobalStage->GetLobyStage()->m_bGameJoin = FALSE;
		return false;
	}
	else
	{
		
		EMSERVICE_PROVIDER sp = GLogicData::GetInstance().GetServiceProvider();
		if ( SP_TAIWAN == sp )
		{
			GLWidgetScript::GetInstance().LuaCallFunc< void >( 
				NSWIDGET_SCRIPT::g_strFunc_SecondPassword_Input );
		}
		else
		{
			int n2ndPass = GLWidgetScript::GetInstance().m_pGaeaClient->GetUse2ndPass();
			switch( n2ndPass )
			{
			case 0 :	// 2차 비번 사용 안함. 일반 로그인
				{
					pGlobalStage->GetLobyStage()->m_bGameJoin = TRUE;
					pGlobalStage->GetNetClient()->SndGameJoin( pCharInfo->m_dwCharID );

					GLWidgetScript::GetInstance().LuaCallFunc< void >( 
						NSWIDGET_SCRIPT::g_strFunc_CharSelect_DClickJoin );
				}
				break;

			case 1 :	// 2차 비번 사용. 입력 필요
				{
					GLWidgetScript::GetInstance().LuaCallFunc< void >( 
						NSWIDGET_SCRIPT::g_strFunc_SecondPassword_Input );
				}
				break;

			case 2 :	// 2차 비번 사용. 신규 설정
				{
					GLWidgetScript::GetInstance().LuaCallFunc< void >( 
						NSWIDGET_SCRIPT::g_strFunc_SecondPassword_New );
				}
				break;
			}
		}

		return true;
	}
}

bool GLLobyCharClient::JoinGame2ndPassword( const char* szPassword )
{
	if( NULL == szPassword )
		return false;

	DxGlobalStage* pGlobalStage = GLWidgetScript::GetInstance().m_pGaeaClient->GetGlobalStage();

	DWORD dwSelectIndex = pGlobalStage->GetLobyStage()->GetCharSelectIndex();

	SCHARINFO_LOBBY* pCharInfo = pGlobalStage->GetLobyStage()->GetCharInfo( dwSelectIndex );
	if( NULL == pCharInfo )
		return false;

	BOOL bNew = 2 == GLWidgetScript::GetInstance().m_pGaeaClient->GetUse2ndPass() ? TRUE : FALSE;
	pGlobalStage->GetNetClient()->SndGameJoin( pCharInfo->m_dwCharID, szPassword, bNew );

	GLWidgetScript::GetInstance().LuaCallFunc< void >( 
		NSWIDGET_SCRIPT::g_strFunc_CharSelect_DClickJoin );

	return true;
}

void GLLobyCharClient::DisconnectServer()
{
	DxGlobalStage* pGlobalStage = GLWidgetScript::GetInstance().m_pGaeaClient->GetGlobalStage();

	CNetClient* pNetClient = pGlobalStage->GetNetClient();
	if( pNetClient->IsOnline() == true )
		pNetClient->CloseConnect();
}

void GLLobyCharClient::DeleteChar( int nIndex, const char* strPassNum )
{
	DxGlobalStage* pGlobalStage = GLWidgetScript::GetInstance().m_pGaeaClient->GetGlobalStage();
	CNetClient* pNetClient = pGlobalStage->GetNetClient();

	SCHARINFO_LOBBY* pCharInfo = pGlobalStage->GetLobyStage()->GetCharInfo( nIndex );
	if( NULL == pCharInfo )
		return;

	switch( RENDERPARAM::emSERVICE_TYPE )
	{
	case RENDERPARAM::EMSERVICE_KOREA :
		pNetClient->DaumSndDelCharacter( pCharInfo->m_dwCharID, strPassNum );
		break;

	case RENDERPARAM::EMSERVICE_INDONESIA :
		pNetClient->IdnSndDelCharacter( pCharInfo->m_dwCharID, strPassNum );
		break;

	case RENDERPARAM::EMSERVICE_JAPAN :
		pNetClient->JapanSndDelCharacter( pCharInfo->m_dwCharID );
		break;

	case RENDERPARAM::EMSERVICE_MALAYSIA_CN :
	case RENDERPARAM::EMSERVICE_MALAYSIA_EN :
	case RENDERPARAM::EMSERVICE_PHILIPPINES :
	case RENDERPARAM::EMSERVICE_VIETNAM :
		pNetClient->TerraSndDelCharacter( pCharInfo->m_dwCharID );
		break;

	case RENDERPARAM::EMSERVICE_CHINA :
		pNetClient->ChinaSndDelCharacter( pCharInfo->m_dwCharID, strPassNum );
		break;

	case RENDERPARAM::EMSERVICE_GLOBAL :
		pNetClient->GsSndDelCharacter( pCharInfo->m_dwCharID, strPassNum );
		break;

	case RENDERPARAM::EMSERVICE_EU :
		pNetClient->GmSndDelCharacter( pCharInfo->m_dwCharID );
		break;

	case RENDERPARAM::EMSERVICE_US :
		pNetClient->UsSndDelCharacter( pCharInfo->m_dwCharID );
		break;

	default : // 대만 태국
		pNetClient->SndDelCharacter( pCharInfo->m_dwCharID, strPassNum );
		break;
	};
}