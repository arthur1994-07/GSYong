#include "pch.h"
#include "GLLobyCharCreateClient.h"

#include "../../SigmaCore/String/StringUtils.h"
#include "../../enginelib/DxTools/RENDERPARAM.h"
#include "../../MfcExLib/RanFilter.h"

#include "../GLGaeaClient.h"
#include "../Stage/DxGlobalStage.h"
#include "../Stage/DxLobyStage.h"
#include "../Login/GLLoginClient.h"

GLLobyCharCreateClient::GLLobyCharCreateClient(void)
:	m_nSelectSchool( -1 )
,	m_nSelectSex( -1 )
,	m_nSelectClass( -1 )
{
}

GLLobyCharCreateClient::GLLobyCharCreateClient( const GLLobyCharCreateClient& vallue )
{
}

GLLobyCharCreateClient::~GLLobyCharCreateClient(void)
{
}

GLLobyCharCreateClient& GLLobyCharCreateClient::GetInstance()
{
	static GLLobyCharCreateClient Instance;
	return Instance;
}

void GLLobyCharCreateClient::MsgProcess( NET_MSG_GENERIC* nmg )
{
	DxGlobalStage* pGlobalStage = GLWidgetScript::GetInstance().m_pGaeaClient->GetGlobalStage();

	switch( nmg->nType )
	{
	case NET_MSG_CHA_NEW_FB :
		{
			pGlobalStage->GetLobyStage()->m_bGameJoin = FALSE;

			NET_NEW_CHA_FB* pCharNewFb = (NET_NEW_CHA_FB*) nmg;

			if( EM_CHA_NEW_FB_SUB_OK == pCharNewFb->nResult )
				GLLoginClient::GetInstance().SetCharRemain( static_cast< USHORT >( pCharNewFb->wChaRemain ) );

			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_CharCreate_NewCharResult,
				"-n", pCharNewFb->nResult );
		}
		break;
	}
}

void GLLobyCharCreateClient::SelectCharSchool( int nSchool )
{
	DxGlobalStage* pGlobalStage = GLWidgetScript::GetInstance().m_pGaeaClient->GetGlobalStage();
	pGlobalStage->GetLobyStage()->SelCharSchool( nSchool );
}

void GLLobyCharCreateClient::SelectCharClass( int nClass )
{
	DxGlobalStage* pGlobalStage = GLWidgetScript::GetInstance().m_pGaeaClient->GetGlobalStage();
	pGlobalStage->GetLobyStage()->SelCharClass( CharClassToIndex( EMCHARCLASS( nClass ) ) );
}

void GLLobyCharCreateClient::SelectCharFace( int nFace )
{
	DxGlobalStage* pGlobalStage = GLWidgetScript::GetInstance().m_pGaeaClient->GetGlobalStage();
	pGlobalStage->GetLobyStage()->SelCharFace( nFace );
}

void GLLobyCharCreateClient::SelectCharHair( int nHair )
{
	DxGlobalStage* pGlobalStage = GLWidgetScript::GetInstance().m_pGaeaClient->GetGlobalStage();
	pGlobalStage->GetLobyStage()->SelCharHair( nHair );
	pGlobalStage->GetLobyStage()->SelCharHairColor( HAIRCOLOR::GetHairColor( pGlobalStage->GetLobyStage()->GetCreateClass(), nHair ) );
}

void GLLobyCharCreateClient::ShiftCharFace( bool bRight )
{
	DxGlobalStage* pGlobalStage = GLWidgetScript::GetInstance().m_pGaeaClient->GetGlobalStage();

	DxLobyStage::EMSHIFT emShift;
	if( true == bRight )
		emShift = DxLobyStage::EM_NEXT;
	else
		emShift = DxLobyStage::EM_PREV;

	pGlobalStage->GetLobyStage()->ShiftCharFace( emShift );
}

void GLLobyCharCreateClient::ShiftCharHair( bool bRight )
{
	DxGlobalStage* pGlobalStage = GLWidgetScript::GetInstance().m_pGaeaClient->GetGlobalStage();

	DxLobyStage::EMSHIFT emShift;
	if( true == bRight )
		emShift = DxLobyStage::EM_NEXT;
	else
		emShift = DxLobyStage::EM_PREV;

	pGlobalStage->GetLobyStage()->ShiftCharHair( emShift );
}

void GLLobyCharCreateClient::SetStyleStep( bool bStyle )
{
	DxGlobalStage* pGlobalStage = GLWidgetScript::GetInstance().m_pGaeaClient->GetGlobalStage();
	pGlobalStage->GetLobyStage()->SetStyleStep( bStyle );
}

void GLLobyCharCreateClient::RotateChar( bool bRight, float fDegree )
{
	DxGlobalStage* pGlobalStage = GLWidgetScript::GetInstance().m_pGaeaClient->GetGlobalStage();
	pGlobalStage->GetLobyStage()->RotateChar( !bRight, fDegree );
}

int GLLobyCharCreateClient::CreateChar( const char* strName )
{
	CString strTemp = strName;
	strTemp = strTemp.Trim();

	EMSERVICE_PROVIDER ServiceProvider = GLWidgetScript::GetInstance().m_pGaeaClient->GetServiceProvider();
	if( SP_INDONESIA == ServiceProvider )
	{
		if( strTemp.Find('`') != -1 )
			return 1;
	}
	else if( SP_WORLD_BATTLE == ServiceProvider )
	{
		if( !sc::string::checkEnglishOrNumber( std::string( strTemp.GetString() ) ) )
			return 2;
	}

	// 태국어 문자 조합 체크 
	if( RENDERPARAM::emLangSet == language::THAILAND ) 
	{
		if( !STRUTIL::CheckThaiString( strTemp ) )
			return 3;
	}
	// 베트남 문자 조합 체크 
	else if( RENDERPARAM::emLangSet == language::VIETNAM ) 
	{
		if( STRUTIL::CheckVietnamString( strTemp ) )
			return 4;
	}

	if( STRUTIL::CheckString( strTemp ) )
		return 5;

	if( CRanFilter::GetInstance().NameFilter( strName ) )
		return 6;

	DxGlobalStage* pGlobalStage = GLWidgetScript::GetInstance().m_pGaeaClient->GetGlobalStage();
	DxLobyStage* pLobyStage = pGlobalStage->GetLobyStage();

	EMSERVICE_PROVIDER emSP = GLWidgetScript::GetInstance().m_pGaeaClient->GetServiceProvider();
	std::string strSndName(strTemp);
	if ( emSP == SP_GS )//
		strSndName = GLWidgetScript::GetInstance().UTF8ToAnsi(strSndName.c_str());

	const GLCHAR_NEWINFO& sNEWINFO = pLobyStage->GetNewCharInfo ();
	if( sNEWINFO.CHECKVALID () )
	{
		CNetClient* pNetClient = pGlobalStage->GetNetClient();
		pNetClient->SndCreateChaInfo(
			sNEWINFO.m_emIndex,		/*부서*/
			sNEWINFO.m_wSchool,		/*학교*/
			sNEWINFO.m_wFace,		/*얼굴*/
			sNEWINFO.m_wHair,		/*헤어*/
			sNEWINFO.m_wHairColor,	/*헤어컬러*/
			sNEWINFO.m_wSex,		/*성별*/
			strSndName.c_str() );
	}

	return 0;
}

LuaTable GLLobyCharCreateClient::GetCharacterRect(void)
{
	DxGlobalStage* pGlobalStage = GLWidgetScript::GetInstance().m_pGaeaClient->GetGlobalStage();
	DxLobyStage* pLobyStage = pGlobalStage->GetLobyStage();

	RECT rc;
	pLobyStage->GetNewCharShanScreenRect(rc);

	LuaTable tbRect( GLWidgetScript::GetInstance().GetLuaState() );
	tbRect.set("left", rc.left);
	tbRect.set("right", rc.right);
	tbRect.set("top", rc.top);
	tbRect.set("bottom", rc.bottom);
	return tbRect;
}