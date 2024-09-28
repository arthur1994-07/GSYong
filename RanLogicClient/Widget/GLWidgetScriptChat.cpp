#include "pch.h"
#include "GLWidgetScript.h"

#include "../../SigmaCore/Util/SystemInfo.h"
#include "../../enginelib/DxTools/DxViewPort.h"
#include "../../RanGfxUI/GfxInterface.h"

#include "../GLGaeaClient.h"
#include "../Chat/GLChatMan.h"
#include "../Char/GLCharacter.h"
#include "../Party/GLPartyClient.h"

#include "./GLWidgetsList.h"

void GLWidgetScript::AddChatMessage( int nLogID, DWORD dwType, bool bOnlyStore, const char* strMsg, 
									 DWORD dwColor, LuaTable tbCustomColor, LuaTable tbNameData )
{
	NULL_INTERFACE_RETURN;

	if( 0 == strcmp( strMsg, "" ) )
		return;

	GLCHAT_DEF::VEC_CHAT_CUSTOM_COLOR vecColor;
	for( int i = 1; i <= tbCustomColor.TableLen(); ++i )
	{
		LuaTable tbColor = tbCustomColor.get< LuaTable >( i );
		if( false == IS_LUATABLE( tbColor ) )
			continue;

		GLCHAT_DEF::SCHAT_CUSTOM_COLOR sColor;
		sColor.dwColor = tbColor.get< DWORD >( 1 );
		sColor.nBegin = tbColor.get< int >( 2 );
		sColor.nEnd = tbColor.get< int >( 3 );

		vecColor.push_back( sColor );
	}

	GLCHAT_DEF::SCHAT_REFERCHAR_NAME sReferName;
	if( 3 == tbNameData.TableLen() )
	{
		sReferName.strName = tbNameData.get< const char* >( 1 );
		sReferName.nBegin = tbNameData.get< int >( 2 );
		sReferName.nEnd = tbNameData.get< int >( 3 );
	}

	GLChatMan::GetInstance().AddChatMessage( nLogID, dwType, strMsg, dwColor, vecColor, sReferName );

	if( false == bOnlyStore )
	{
		GLWidgetScript::GetInstance().m_pInterface->AddChatMessage( 
			nLogID, strMsg, dwColor, vecColor, sReferName );
	}
}

void GLWidgetScript::AddLinkMessage( int nLogID, DWORD dwType, bool bOnlyStore, const char* strMsg, 
									 DWORD dwColor, LuaTable tbCustomColor, LuaTable tbData, LuaTable tbNameData )
{
	NULL_INTERFACE_RETURN;

	if( 0 == strcmp( strMsg, "" ) )
		return;

	GLCHAT_DEF::VEC_CHAT_CUSTOM_COLOR vecColor;
	for( int i = 1; i <= tbCustomColor.TableLen(); ++i )
	{
		LuaTable tbColor = tbCustomColor.get< LuaTable >( i );
		if( false == IS_LUATABLE( tbColor ) )
			continue;

		GLCHAT_DEF::SCHAT_CUSTOM_COLOR sColor;
		sColor.dwColor = tbColor.get< DWORD >( 1 );
		sColor.nBegin = tbColor.get< int >( 2 );
		sColor.nEnd = tbColor.get< int >( 3 );

		vecColor.push_back( sColor );
	}

	VEC_LINK_TEXT_DATA vecLinkDataText;
	for( int i = 1; i <= tbData.TableLen(); ++i )
	{
		lua_tinker::table tbLinkData = tbData.get< lua_tinker::table >( i );

		SLINK_TEXT_DATA sLinkData;
		sLinkData.nLinkTypeIndex = tbLinkData.get< int >( "nLinkTypeIndex" );
		sLinkData.sLinkPos.nStart = tbLinkData.get< int >( "nStart" );
		sLinkData.sLinkPos.nEnd = tbLinkData.get< int >( "nEnd" );
		sLinkData.dwTextColor = tbLinkData.get< DWORD >( "dwColor" );
		vecLinkDataText.push_back( sLinkData );
	}

	GLCHAT_DEF::SCHAT_REFERCHAR_NAME sReferName;
	if( 3 == tbNameData.TableLen() )
	{
		sReferName.strName = tbNameData.get< const char* >( 1 );
		sReferName.nBegin = tbNameData.get< int >( 2 );
		sReferName.nEnd = tbNameData.get< int >( 3 );
	}

	GLChatMan::GetInstance().AddLinkMessage( nLogID, dwType, strMsg, dwColor,
		vecColor, vecLinkDataText, sReferName );

	if( false == bOnlyStore )
	{
		GLWidgetScript::GetInstance().m_pInterface->AddLinkMessage( 
			nLogID, strMsg, dwColor, vecColor, vecLinkDataText, sReferName );
	}
}

void GLWidgetScript::AddStoredChat( int nLogID )
{
	LIST_CHAT listChat;
	GLChatMan::GetInstance().GetChatList( nLogID, listChat );

	GLWidgetScript::GetInstance().m_pInterface->PrintChatList( nLogID, listChat );
}

void GLWidgetScript::AddStoredChatType( int nLogID, DWORD dwType )
{
	LIST_CHAT listChat;
	GLChatMan::GetInstance().GetChatList( nLogID, dwType, listChat );

	GLWidgetScript::GetInstance().m_pInterface->PrintChatList( nLogID, listChat );
}

void GLWidgetScript::ClearStoredChat( int nLogID )
{
	GLChatMan::GetInstance().ClearMessage( nLogID );
}

void GLWidgetScript::AddHeadChat( const char* strName, const char* strMsg )
{
	GLGaeaClient* pClient = GLWidgetScript::GetInstance().m_pGaeaClient;
	GLCharacter* pChar = pClient->GetCharacter();

	D3DXVECTOR3* pvHeadPos = pClient->FindCharHeadPos( strName );
	if( NULL != pvHeadPos )
	{
		D3DXVECTOR3 vPos = *pvHeadPos;
		D3DXVECTOR3 vScreen = DxViewPort::GetInstance().ComputeVec3Project( &vPos, NULL );

		int nPosX = (int) vScreen.x;
		int nPosY = (int) vScreen.y;

		D3DXVECTOR2 vChatPos = D3DXVECTOR2( (float) nPosX, (float) nPosY );

		DWORD dwIDColor = NS_UITEXTCOLOR::IDCOLOR;
		if( 0 == strcmp( strName, pChar->m_szName ) && true == pClient->GetMyPartyClient()->isValid() )
			dwIDColor = NS_UITEXTCOLOR::GREENYELLOW;

		GLWidgetScript::GetInstance().m_pInterface->AddHeadChatDisplay( 
			strName, dwIDColor, strMsg, NS_UITEXTCOLOR::DEFAULT, vChatPos );
	}
}