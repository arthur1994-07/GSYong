#include "pch.h"
#include "GLLoginClient.h"

#include "../../SigmaCore/String/StringUtils.h"
#include "../../enginelib/ServiceProviderDefine.h"
#include "../../enginelib/GUInterface/GameTextControl.h"
#include "../../enginelib/GUInterface/UIInfoCfg.h"
#include "../../enginelib/DxTools/RENDERPARAM.h"
#include "../../RanLogic/RANPARAM.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../RanLogic/Msg/LoginMsg.h"

#include "../Stage/DxGlobalStage.h"
#include "../GLGaeaClient.h"

GLLoginClient::GLLoginClient(void)
:	m_bChaRemainExtreme( 0 )
,	m_bChaRemainActor( false )
,	m_uChaRemain( 0 )
,	m_nSelectServerIndex( -1 )
,	m_nServerGroup( -1 )
,	m_nServerChannel( -1 )
{
}

GLLoginClient::GLLoginClient( const GLLoginClient& value )
{
}

GLLoginClient::~GLLoginClient(void)
{
}

GLLoginClient& GLLoginClient::GetInstance()
{
	static GLLoginClient Instance;
	return Instance;
}

void GLLoginClient::MsgProcess( LPVOID pBuffer )
{
	if( NULL == pBuffer )
		return;

	NET_MSG_GENERIC* nmg = (NET_MSG_GENERIC*) pBuffer;

	GLGaeaClient* pGaeaClient = GLWidgetScript::GetInstance().m_pGaeaClient;
	EMSERVICE_PROVIDER sp = pGaeaClient->GetServiceProvider();

	switch( nmg->nType )
	{
	case NET_MSG_LOGIN_FB :
		{
			DxGlobalStage* pGlobalStage = pGaeaClient->GetGlobalStage();
			NET_LOGIN_FEEDBACK_DATA* pPacket = (NET_LOGIN_FEEDBACK_DATA *) nmg;

			int nServerPatchProgramVer = pPacket->m_LauncherVersion;
			int nServerGameProgramVer = pPacket->m_GameVersion;

			SetCharRemainEx( pPacket->m_Extreme );
			SetCharRemainActor( pPacket->m_bActor );
			SetCharRemain( pPacket->m_ChaRemain );
			SetCountry( pPacket->m_Country );

			pGlobalStage->SetWorldBattleID( pPacket->m_szDaumGID );

			/*
			if( pPacket->m_Result == EM_LOGIN_FB_SUB_OK )
			{
				if ( GetCountry() == wb::UNKNOWN_COUNTRY )
					ToSelectCountryPage();
			}
			*/

			int nClientPatchProgramVer;
			int nClientGameProgramVer;

			if( FALSE == LoadClientVersion( nClientPatchProgramVer, nClientGameProgramVer ) )
			{
				GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Login_FailLoadClientVersion );
				return;
			}

			BOOL bDifferentVer = FALSE;
			if( nClientPatchProgramVer < nServerPatchProgramVer || nClientGameProgramVer < nServerGameProgramVer )
				bDifferentVer = TRUE;

			if( EM_LOGIN_FB_SUB_OK == pPacket->m_Result || pPacket->m_Result == EM_LOGIN_FB_KR_OK_USE_PASS || pPacket->m_Result == EM_LOGIN_FB_KR_OK_NEW_PASS )
			{
				// ������ �ٸ� ���
				if( TRUE == bDifferentVer )
				{
					GLWidgetScript::GetInstance().LuaCallFunc< void >(
						NSWIDGET_SCRIPT::g_strFunc_Login_DifferentVersion );
					return;
				}

				if( pGaeaClient->GetServiceProvider() == SP_WORLD_BATTLE )
					return;

				// 0 : 2����� ������. 1 : 2����� ��� , 2 : 2����� �űԼ���
				if(pPacket->m_Result == EM_LOGIN_FB_KR_OK_USE_PASS)
					pGaeaClient->SetUse2ndPass( 1 );
				else if(pPacket->m_Result == EM_LOGIN_FB_KR_OK_NEW_PASS)
					pGaeaClient->SetUse2ndPass( 2 );
				else
					pGaeaClient->SetUse2ndPass( 0 );

				//	�α��� ���� & ĳ���� ���� ȭ������ �̵�
				pGlobalStage->OnInitLogin();

				if( pGaeaClient )
				{
					pGaeaClient->SetUserCountry( pPacket->m_Country );
					pGaeaClient->SetCountryInfo( pPacket->m_sCountryInfo );
				}

				pGlobalStage->GetNetClient()->SndChaAllBasicInfo();
				// �α��� ���μ��� Ȯ���� ���� �α�
				if( pGlobalStage->m_bLoginProcessLog) sc::writeLogError(std::string("LOGIN PROCESS. Request Basic Char Info (SF)"));

				GLWidgetScript::GetInstance().LuaCallFunc< void >(
					NSWIDGET_SCRIPT::g_strFunc_EnterCharSelectStage );
			}
			else
			{
				GLWidgetScript::GetInstance().LuaCallFunc< void >(
					NSWIDGET_SCRIPT::g_strFunc_Login_FailLogin,
					"-n", pPacket->m_Result );
			}
		}
		break;
	case GS_NET_MSG_WB_LOGIN_FB :
		if( sp == SP_GS )
		{
			GS_NET_WB_LOGIN_DATA_FB* pPacket = (GS_NET_WB_LOGIN_DATA_FB *) nmg;

			// ���弭���� ������ ���� �ʰų� ���弭���� �����ִ°��
			if( pPacket->bLoginWB == false || pPacket->bWBOff == true )
			{
				DxGlobalStage* pGlobalStage = GLWidgetScript::GetInstance().m_pGaeaClient->GetGlobalStage();
				if ( NULL == pGlobalStage )
					return;

				CNetClient* pNetClient = pGlobalStage->GetNetClient();

				if( strcmp( pGlobalStage->GetWorldBattleID(), pPacket->szUserid ) == 0 )
				{
					pNetClient->GsSndLogin( pGlobalStage->GetWorldBattleID(), pGlobalStage->GetPWDForWB(), pPacket->nChannel );
				}
				else
				{
					//sc::writeLogError( "============================================  [SF] LOGIN_FAILED_ID_WB" );
					//DoModal ( ID2GAMEEXTEXT ("LOGIN_FAILED_ID_WB"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_CLOSEGAME );
					return;
				}
			}
			else
			{
				//sc::writeLogError( "============================================  [SF] LOGIN_FAILED_WBS" );
				//DoModal ( ID2GAMEEXTEXT ("LOGIN_FAILED_WB"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_CLOSEGAME );
				return ;
			}

		}
		else
		{
			sc::writeLogError( "WB Login can use only GS_SF" );
		}
		break;
	}
}

BOOL GLLoginClient::LoadClientVersion( int& nPatchVer, int& nGameVer )
{
	//	��ó���� ������ �ε��ϴ� �κ��� �ֽ��ϴ�.
	//	����� ��� �ݵ�� üũ �ؾ��մϴ�.
	//	AutoPatchManDlgEx.cpp!!!

	const CString strVersionFileName = "cVer.bin";

	DxGlobalStage* pGlobalStage = GLWidgetScript::GetInstance().m_pGaeaClient->GetGlobalStage();
	CString strTemp;
	strTemp.Format ( "%s\\%s", pGlobalStage->GetAppPath(), strVersionFileName );
	FILE* cfp = _fsopen ( strTemp.GetString(), "rb", _SH_DENYNO );
	if( NULL == cfp )
		return FALSE;

	if( 1 != fread( &nPatchVer, sizeof( int ), 1, cfp ) )
	{
		fclose( cfp );
		return FALSE;
	}

	if( 1 != fread( &nGameVer, sizeof( int ), 1, cfp ) )
	{
		fclose( cfp );
		return FALSE;
	}

	fclose( cfp );

	return TRUE;
}

void GLLoginClient::SetCharRemainEx( const bool bChaRemain )
{
	if( GLUseFeatures::GetInstance().IsUsingExtremeClass() )
		m_bChaRemainExtreme = bChaRemain;
	else
		m_bChaRemainExtreme = 0;
}

const bool GLLoginClient::GetCharRemainEx() const
{
	if ( GLUseFeatures::GetInstance().IsUsingExtremeClass() )
		return m_bChaRemainExtreme;

	return 0; 
}

void GLLoginClient::SetCharRemainActor ( const bool bChaRemain )
{
	m_bChaRemainActor = bChaRemain;
}

const bool GLLoginClient::GetCharRemainActor () const
{
	return m_bChaRemainActor;
}

void GLLoginClient::SetCountry( wb::EM_COUNTRY Country )
{
	m_Country = Country;

	if( m_Country != wb::UNKNOWN_COUNTRY )
	{
		//	�α��� ���� & ĳ���� ���� ȭ������ �̵�
		DxGlobalStage* pGlobalStage = GLWidgetScript::GetInstance().m_pGaeaClient->GetGlobalStage();
		
		pGlobalStage->OnInitLogin();
		pGlobalStage->GetNetClient()->SndChaAllBasicInfo();
		// �α��� ���μ��� Ȯ���� ���� �α�
		if( pGlobalStage->m_bLoginProcessLog) sc::writeLogError(std::string("LOGIN PROCESS. Request Basic Char Info (SF Country)"));
	}
}

bool GLLoginClient::IsOnline()
{
	DxGlobalStage* pGlobalStage = GLWidgetScript::GetInstance().m_pGaeaClient->GetGlobalStage();
	CNetClient* pNetClient = pGlobalStage->GetNetClient();

	return pNetClient->IsOnline();
}

bool GLLoginClient::IsGameServerInfoEnd()
{
	DxGlobalStage* pGlobalStage = GLWidgetScript::GetInstance().m_pGaeaClient->GetGlobalStage();
	CNetClient* pNetClient = pGlobalStage->GetNetClient();

	return pNetClient->IsGameSvrInfoEnd();
}

lua_tinker::table GLLoginClient::GetGameServerList()
{
	DxGlobalStage* pGlobalStage = GLWidgetScript::GetInstance().m_pGaeaClient->GetGlobalStage();
	CNetClient* pNetClient = pGlobalStage->GetNetClient();

	LPG_SERVER_CUR_INFO_LOGIN pServerCurInfo = pNetClient->GetSeverCurInfoLogin();

	struct S_SERVER_INFO
	{
		bool	bUSE;
		int		nIndex;
		int		nUserAmount;
		bool    bNewServer;

		S_SERVER_INFO()
			: bUSE( false )
			, nIndex( -1 )
			, nUserAmount( 0 )
			, bNewServer( false )
		{
		}
	};
	S_SERVER_INFO sSERVERINFO[ MAX_SERVER_GROUP ];

	bool bEmptyServerList = true;

	//	ī��Ʈ
	for( int i = 0; i < MAX_SERVER_GROUP; ++i )
	{
		sSERVERINFO[ i ].nIndex = i;
		for( int j = 0, index = 0; j < MAX_SERVER_NUMBER; ++j )
		{
			LPG_SERVER_CUR_INFO_LOGIN pServerInfo = pServerCurInfo + ( i * MAX_SERVER_NUMBER ) + j;
			if( NULL == pServerInfo || 0 == pServerInfo->nServerMaxClient )
				continue;

			sSERVERINFO[ i ].bUSE = true;
			sSERVERINFO[ i ].nUserAmount += pServerInfo->nServerCurrentClient;
			sSERVERINFO[ i ].bNewServer = pServerInfo->bNewServer;

			bEmptyServerList = false;
		}
	}

	lua_tinker::table tbServerList( GLWidgetScript::GetInstance().GetLuaState() );
	if( true == bEmptyServerList )
		return tbServerList;

	// �ѱ��� ��� �����ڰ� ���������� ����
	switch( RENDERPARAM::emSERVICE_TYPE )
	{
	// GSServer�� ���������ʴ´�;
	case RENDERPARAM::EMSERVICE_GLOBAL :	break;
	case RENDERPARAM::EMSERVICE_DEFAULT :
	case RENDERPARAM::EMSERVICE_KOREA :
		{
			for( int i = 0; i < MAX_SERVER_GROUP; ++i )
			{
				for( int j = 0; j < MAX_SERVER_GROUP - 1; ++j )
				{
					if( sSERVERINFO[ j ].nUserAmount < sSERVERINFO[ j + 1 ].nUserAmount )
					{
						S_SERVER_INFO temp = sSERVERINFO[ j ];
						sSERVERINFO[ j ] = sSERVERINFO[ j + 1 ];
						sSERVERINFO[ j + 1 ] = temp;
					}
				}
			}
		}
		break;

	default :
		{
			for( int i = 0; i < MAX_SERVER_GROUP; ++i )
			{
				for( int j = 0; j < MAX_SERVER_GROUP - 1; ++j )
				{
					if( sSERVERINFO[ j ].nUserAmount > sSERVERINFO[ j + 1 ].nUserAmount )
					{
						S_SERVER_INFO temp = sSERVERINFO[ j ];
						sSERVERINFO[ j ] = sSERVERINFO[ j + 1 ];
						sSERVERINFO[ j + 1 ] = temp;
					}
				}
			}
		}
		break;
	}

	EMSERVICE_PROVIDER sp =	GLWidgetScript::GetInstance().m_pGaeaClient->GetServiceProvider();

	// ���̺� ����
	int nTableCount = 1;
	for( int i = 0; i < MAX_SERVER_GROUP; ++i )
	{
		if( false == sSERVERINFO[ i ].bUSE )
			continue;

		lua_tinker::table tbServerObj( GLWidgetScript::GetInstance().GetLuaState() );
		tbServerObj.set( 1, sSERVERINFO[ i ].nIndex );		// �׷� �ε���
		tbServerObj.set( 2, sSERVERINFO[ i ].bNewServer );	// �ű� ����?

		tbServerList.set( nTableCount, tbServerObj );
		++nTableCount;
	}

	return tbServerList;
}

bool GLLoginClient::ConnectLoginServer()
{
	DxGlobalStage* pGlobalStage = GLWidgetScript::GetInstance().m_pGaeaClient->GetGlobalStage();
	CNetClient* pNetClient = pGlobalStage->GetNetClient();

	if( NET_ERROR < pNetClient->ConnectLoginServer( RANPARAM::LoginAddress, RANPARAM::LoginServerPort ) )
	{
		pNetClient->SndReqServerInfo();
		return true;
	}

	return false;
}

lua_tinker::table GLLoginClient::GetServerChannelList( int nGroupIndex )
{
	DxGlobalStage* pGlobalStage = GLWidgetScript::GetInstance().m_pGaeaClient->GetGlobalStage();
	CNetClient* pNetClient = pGlobalStage->GetNetClient();

	LPG_SERVER_CUR_INFO_LOGIN pServerCurInfo = pNetClient->GetSeverCurInfoLogin();

	lua_tinker::table tbChannelList( GLWidgetScript::GetInstance().GetLuaState() );

	int nChannelIndex = 1;
	for( int i = 0; i < MAX_SERVER_NUMBER; ++i )
	{
		LPG_SERVER_CUR_INFO_LOGIN pServerInfo = pServerCurInfo + ( nGroupIndex * MAX_SERVER_NUMBER ) + i;
		if( NULL == pServerInfo || 0 == pServerInfo->nServerMaxClient )
			continue;

		//	���� ����
		float fPercent = (float) pServerInfo->nServerCurrentClient / (float) pServerInfo->nServerMaxClient;

		lua_tinker::table tbChannelObj( GLWidgetScript::GetInstance().GetLuaState() );
		tbChannelObj.set( 1, i );					// ä�� �ε���
		tbChannelObj.set( 2, pServerInfo->bPK );	// pk ����
		tbChannelObj.set( 3, fPercent );			// ���� ����

		tbChannelList.set( nChannelIndex, tbChannelObj );
		++nChannelIndex;
	}

	return tbChannelList;
}

bool GLLoginClient::ConnectServerChannel( int nServerIndex, int nServerGroup, int nChannelIndex )
{
	if( 0 > nServerIndex || 0 > nChannelIndex )
		return false;

	DxGlobalStage* pGlobalStage = GLWidgetScript::GetInstance().m_pGaeaClient->GetGlobalStage();
	pGlobalStage->SetChannel( nChannelIndex );

	GLWidgetScript::GetInstance().m_pGaeaClient->SetConnServerGroup( nServerGroup );
	GLWidgetScript::GetInstance().m_pGaeaClient->SetConnChannel( nChannelIndex );

	// ������ ���� ������ ��Ʈ
	CNetClient* pNetClient = pGlobalStage->GetNetClient();
	if( NET_ERROR >= pNetClient->ConnectGameServer( nServerGroup, nChannelIndex ) )
		return false;

	GLWidgetScript::GetInstance().m_pGaeaClient->SetServerGroup( nServerGroup );

	return true;
}

bool GLLoginClient::WaitKeyAndTryLogin()
{
	DxGlobalStage* pGlobalStage = GLWidgetScript::GetInstance().m_pGaeaClient->GetGlobalStage();
	CNetClient* pNetClient = pGlobalStage->GetNetClient();

	// ���� ������ ��ȣ Ű�� ���� ������ ����Ѵ�.
	if( TRUE == pNetClient->IsKeyReceived() )
	{
		switch( RENDERPARAM::emSERVICE_TYPE )
		{
		case RENDERPARAM::EMSERVICE_KOREA :
			pNetClient->DaumSndLogin( RANPARAM::GETIDPARAM(), m_nServerChannel );
			break;

		case RENDERPARAM::EMSERVICE_MALAYSIA_CN :
		case RENDERPARAM::EMSERVICE_MALAYSIA_EN :
		case RENDERPARAM::EMSERVICE_PHILIPPINES :
		case RENDERPARAM::EMSERVICE_VIETNAM :
			pNetClient->TerraSndLogin( RANPARAM::GETIDPARAM(), m_nServerChannel );
			break;
		};

		return true;
	}

	return false;
}

bool GLLoginClient::SetServerNumber( int nServerIndex )
{
	// ���� �ε��� ����
	RANPARAM::dwServerNum = (DWORD) nServerIndex;
	CUIInfoCfg::GetInstance().SetAddPathNameServerNum( RANPARAM::dwServerNum );

	if( true == RANPARAM::VALIDIDPARAM() )
		return false;

	return true;
}

int GLLoginClient::GetRandomPassNumber()
{
	DxGlobalStage* pGlobalStage = GLWidgetScript::GetInstance().m_pGaeaClient->GetGlobalStage();
	CNetClient* pNetClient = pGlobalStage->GetNetClient();

	return pNetClient->GetRandomPassNumber();
}

std::string GLLoginClient::SaveUserID()
{
	return RANPARAM::GETUSERID_DEC();
}

void GLLoginClient::SetSaveUserID( bool bSave )
{
	RANPARAM::bSAVE_USERID = true == bSave ? TRUE : FALSE;
	RANPARAM::SAVE( GLWidgetScript::GetInstance().m_pGaeaClient->GetServiceProvider() );
}

bool GLLoginClient::IsSaveUserID()
{
	return TRUE == RANPARAM::bSAVE_USERID ? true : false;
}

void GLLoginClient::Login( const char* strID, const char* strPW, const char* strRP, int nServerChannel )
{
	EMSERVICE_PROVIDER sp =	GLWidgetScript::GetInstance().m_pGaeaClient->GetServiceProvider();

	DxGlobalStage* pGlobalStage = GLWidgetScript::GetInstance().m_pGaeaClient->GetGlobalStage();
	CNetClient* pNetClient = pGlobalStage->GetNetClient();

	switch( sp )
	{
	case SP_CHINA :
		pNetClient->ChinaSndLogin( strID, strPW, strRP, nServerChannel );
		break;

	case SP_JAPAN :
		pNetClient->JapanSndLogin( strID, strPW, nServerChannel );
		break;

	case SP_THAILAND :
		pNetClient->ThaiSndLogin( strID, strPW, nServerChannel );
		break;

	case SP_GS :
		// GS�� ���弭���� ����ϱ� ������ ���弭���� ���ӿ��θ� ���� Ȯ���� �α����Ѵ�
		//pNetClient->GsSndLogin( strID, strPW, nServerChannel );
		pNetClient->GsSndWBLogin( strID, strPW, nServerChannel );
		break;

	case SP_INDONESIA :
		pNetClient->IdnSndLogin( strID, strPW, nServerChannel );
		break;

	case SP_WORLD_BATTLE :
		pNetClient->WorldBattleLogin( strID, strPW, nServerChannel );
		break;

	case SP_EU :
		pNetClient->GmSndLogin( strID, strPW, nServerChannel );
		break;

	case SP_US :
		pNetClient->UsSndLogin( strID, strPW, nServerChannel );
		break;

	default :
		pNetClient->SndLogin( strID, strPW, strRP, nServerChannel );		
		break;
	};

	// �������� ���� ������ ���ؼ� ���̵� ����
	pGlobalStage->SetWorldBattleID( strID );
	pGlobalStage->SetPWDForWB( strPW );

	// ���� id ����. ( ���� ���� ����, id ������ ���ؼ� �ʿ���. )
	if( TRUE == RANPARAM::bSAVE_USERID )
		RANPARAM::SETUSERID( strID );
	else
		RANPARAM::SETUSERID( "" );

	RANPARAM::SAVE( sp );
}

// �������� �α��� W.B.
BOOL GLLoginClient::LoginWorldBattle()
{
	int nServerGroup = 1;
	int nServerNumber = 0;
	int nServerChannel = 0;

	GLGaeaClient* pClient = GLWidgetScript::GetInstance().m_pGaeaClient;

	DxGlobalStage* pGlobalStage = pClient->GetGlobalStage();
	pGlobalStage->SetChannel( nServerChannel );

	m_nServerGroup = nServerGroup;
	m_nServerChannel = nServerNumber;

	pClient->SetConnServerGroup( nServerGroup );
	pClient->SetConnChannel( nServerNumber );

	// ������ ���� ������,��Ʈ
	CNetClient* pNetClient = pGlobalStage->GetNetClient();
	if( pNetClient->ConnectGameServer( nServerGroup, nServerNumber ) <= NET_ERROR )
	{
		// ���� ���� ����
		//DoModal ( ID2GAMEEXTEXT ("SERVERSTAGE_6"), UI::MODAL_INFOMATION );
		return FALSE;
	}

	pClient->SetServerGroup( nServerGroup );

	return TRUE;
}