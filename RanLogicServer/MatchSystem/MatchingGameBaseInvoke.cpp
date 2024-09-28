#include "pch.h"
#include "../TournamentManager/GLMatchingScriptManager.h"
#include "../../RanLogic/Tournament/GLGroupManager.h"

#include "./InvokeFunctionAgentMatching.h"
#include "./MatchingManager.h"
#include "./MatchingDefine.h"

#include "./MatchingGameBase.h"
#include "./MatchingInstanceBase.h"

namespace MatchingSystem
{
	/// 클라이언트에게 메시지;
	void MatchingGameBase::SendToClientMessageBox (
		const lua_WChar* message,
		DWORD dbnum )
	{
		const std::wstring wMessage( (const wchar_t*)message );
		std::string the_string( sc::string::unicodeToAnsi( wMessage ) );

		GLMSG::NET_MATCHING_NOTIFY_MESSAGEBOX_AC _MessageBox(
			dbnum, the_string );

		MatchingCore::getinstance()->_sendMessageToClient(
			dbnum, &_MessageBox );
	}

	void MatchingGameBase::SendToClientMessageBoxGROUP (
		const lua_WChar* message,
		DWORD GroupID )
	{
		std::vector< DWORD > clientlist;

		MatchingSystem::GLMatchingGroupManager* pGroupManager =
			getGroupManager();
		if ( NULL == pGroupManager )
			return;

		pGroupManager->SearchCharacters_MatchingGroupID(
			clientlist,
			GroupID );

		BOOST_FOREACH ( DWORD ClientDBnum, clientlist )
		{
			SendToClientMessageBox( message, ClientDBnum );
		}

		clientlist.clear();
	}

	void MatchingGameBase::SendToClientMessageBoxALL ( 
		const lua_WChar* message )
	{
		std::vector< DWORD > clientlist;

		MatchingSystem::GLMatchingGroupManager* pGroupManager =
			getGroupManager();
		if ( NULL == pGroupManager )
			return;

		pGroupManager->SearchMatchingGroup_MatchingID(
			clientlist,
			GetScriptID() );

		BOOST_FOREACH ( DWORD ClientDBnum, clientlist )
		{
			SendToClientMessageBox( message, ClientDBnum );
		}

		clientlist.clear();
	}


	/// 클라이언트에게 메시지 XML;
	int MatchingGameBase::SendToClientMessageXML ( lua_State* pState )
	{
		int currentStack(1);

		const lua_Integer _nDbNum(lua_tointeger(pState, currentStack++));
		GLCharAG* const _pChar = gpAgentServer->GetCharByDbNum(_nDbNum);
		if ( _pChar == NULL )
			return 0;

		GLMSG::NET_INSTANCE_UI_CHATXMLMSG NetMsg;
		if ( InstanceSystem::_makeChatXmlMsgPacket(pState, currentStack, NetMsg) == false )
			return 0;

		msgpack::sbuffer SendBuffer;
		msgpack::pack(SendBuffer, NetMsg);
		gpAgentServer->SENDTOCLIENT(_pChar->ClientSlot(), NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK, SendBuffer);

		return 0;
	}

	int MatchingGameBase::SendToClientMessageXMLALL ( lua_State* pState )
	{
		int currentStack(1);

		GLMSG::NET_INSTANCE_UI_CHATXMLMSG NetMsg;
		if ( InstanceSystem::_makeChatXmlMsgPacket(pState, currentStack, NetMsg) == false )
			return 0;

		std::vector<DWORD> clientlist;
		MatchingSystem::GLMatchingGroupManager* groupmanager = getGroupManager();
		groupmanager->SearchCharacters_MatchingID(clientlist,GetScriptID());

		BOOST_FOREACH(DWORD ClientDBnum,clientlist)
		{
			GLCharAG* const _pChar = gpAgentServer->GetCharByDbNum(ClientDBnum);
			if ( _pChar == NULL )
				return 0;

			msgpack::sbuffer SendBuffer;
			msgpack::pack(SendBuffer, NetMsg);

			gpAgentServer->SENDTOCLIENT(_pChar->ClientSlot(), NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK, SendBuffer);
		}

		clientlist.clear();

		return 0;
	}

	int MatchingGameBase::SendToClientMessageXMLGROUP ( lua_State* pState )
	{
		int currentStack(1);

		const lua_Integer GroupID(lua_tointeger(pState, currentStack++));

		std::vector<DWORD> clientlist;
		MatchingSystem::GLMatchingGroupManager* groupmanager = getGroupManager();
		groupmanager->SearchCharacters_MatchingGroupID(clientlist,GroupID);


		GLMSG::NET_INSTANCE_UI_CHATXMLMSG NetMsg;
		if ( InstanceSystem::_makeChatXmlMsgPacket(pState, currentStack, NetMsg) == false )
			return 0;

		BOOST_FOREACH(DWORD ClientDBnum,clientlist)
		{
			msgpack::sbuffer SendBuffer;
			msgpack::pack(SendBuffer, NetMsg);

			GLCharAG* const _pChar = gpAgentServer->GetCharByDbNum(ClientDBnum);
			if ( _pChar == NULL )
				return 0;

			gpAgentServer->SENDTOCLIENT(_pChar->ClientSlot(), NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK, SendBuffer);

		}

		clientlist.clear();

		return 0;
	}

	/// 서버 로그;
	void MatchingGameBase::Serverlog ( const char* message )
	{
		sc::writeLogInfo( message );
	}

}