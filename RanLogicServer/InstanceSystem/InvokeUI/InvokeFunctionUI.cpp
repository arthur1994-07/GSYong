#include "pch.h"

#include "../../../EngineLib/GUInterface/GameTextControl.h"

#include "../../../RanLogic/Msg/GLContrlInstanceContentsMsg.h"

#include "../../FieldServer/GLGaeaServer.h"

#include "../LogicField/InstanceSystemField.h"

#include "../InstanceSystem.h"

#include "./InvokeFunctionUI.h"

namespace InstanceSystem
{
	void RegistInvokeFunction_UI_Instance ( sc::LuaInstance& _sLua )
	{
		_sLua.RegistFunction( "ShowAuthUI", InstanceSystem::InvokeFunction_ShowAuthUI );
		_sLua.RegistFunction( "ShowResultUI", InstanceSystem::InvokeFunction_ShowResultUI );
		_sLua.RegistFunction( "ShowTimeUI", InstanceSystem::InvokeFunction_ShowTimeUI );
		_sLua.RegistFunction( "ShowEntranceStateUI", InstanceSystem::InvokeFunction_ShowEntranceStateUI );
		

		_sLua.RegistFunction( "UI_Auth", InstanceSystem::InvokeFunction_ShowAuthUI );
		_sLua.RegistFunction( "UI_Result", InstanceSystem::InvokeFunction_ShowResultUI );
		_sLua.RegistFunction( "UI_Timer", InstanceSystem::InvokeFunction_ShowTimeUI );
		_sLua.RegistFunction( "UI_EntranceState", InstanceSystem::InvokeFunction_ShowEntranceStateUI );
		
		/*_sLua.registFunction( "PrintChatMsgFaction", InstanceSystem::FieldScriptFunction::PrintChatMsgFaction );
		_sLua.registFunction( "PrintChatXMLMsgFaction", InstanceSystem::FieldScriptFunction::PrintChatXMLMsgFaction );
		_sLua.registFunction( "PrintChatMsgAll", InstanceSystem::FieldScriptFunction::PrintChatMsgAll );
		_sLua.registFunction( "PrintChatXMLMsgAll", InstanceSystem::FieldScriptFunction::PrintChatXMLMsgAll );

		_sLua.registFunction( "PrintSimpleMsgFaction", InstanceSystem::FieldScriptFunction::PrintSimpleMsgFaction );
		_sLua.registFunction( "PrintSimpleXMLMsgFaction", InstanceSystem::FieldScriptFunction::PrintSimpleXMLMsgFaction );
		_sLua.registFunction( "PrintSimpleMsgAll", InstanceSystem::FieldScriptFunction::PrintSimpleMsgAll );
		_sLua.registFunction( "PrintSimpleXMLMsgAll", InstanceSystem::FieldScriptFunction::PrintSimpleXMLMsgAll );

		_sLua.registFunction( "UI_TimerMessageBox", InstanceSystem::FieldScriptFunction::UI_TimerMessageBox );
		_sLua.registFunction( "UI_TimerMessageBoxAll", InstanceSystem::FieldScriptFunction::UI_TimerMessageBoxAll );
		_sLua.registFunction( "UI_XmlTimerMessageBox", InstanceSystem::FieldScriptFunction::UI_XmlTimerMessageBox );
		_sLua.registFunction( "UI_XmlTimerMessageBoxAll", InstanceSystem::FieldScriptFunction::UI_XmlTimerMessageBoxAll );*/
	}

	void RegistInvokeFunction_UI_Agent ( sc::LuaInstance& _sLua )
	{
	}

	void RegistInvokeFunction_UI_Agent ( InstanceSystem::ScriptManager* const _pScriptManager )
	{
		if ( NULL == _pScriptManager )
			return;

		_pScriptManager->registFunctionCommon( RegistInvokeFunction_UI_Agent );
	}

	void RegistInvokeFunction_UI_Field ( sc::LuaInstance& _sLua )
	{
	}

	void RegistInvokeFunction_UI_Field ( InstanceSystem::ScriptManager* const _pScriptManager )
	{
		if ( NULL == _pScriptManager )
			return;

		_pScriptManager->registFunctionCommon( RegistInvokeFunction_UI_Field );
	}


	/************************************************************************/
	/* 인증 UI 관련 Invoke 함수 목록;                                     */
	/************************************************************************/

	int InvokeFunction_ShowAuthUI ( lua_State* pState )
	{
		int nCurrentStack = 1;

		const DWORD dwChaDBNum = static_cast< DWORD >( lua_tonumber( pState, nCurrentStack++ ) );
		const bool bVisible = lua_toboolean( pState, nCurrentStack++ );
		const float fAuthTime = static_cast< float >( lua_tonumber( pState, nCurrentStack++ ) );

		GLChar* pChar = gpGaeaServer->GetCharByDbNum( dwChaDBNum );
		if ( NULL == pChar )
			return 0;

		GLMSG::NET_INSTANCE_CTI_UI_AUTH_FC netMsg;
		netMsg.bVisible = bVisible;
		netMsg.fAuthTime = fAuthTime;

		gpGaeaServer->SENDTOCLIENT( pChar->ClientSlot(), &netMsg );

		return 0;
	}


	/************************************************************************/
	/* CompetitionResult 관련 Invoke 함수 목록;                            */
	/************************************************************************/

	int InvokeFunction_ShowResultUI ( lua_State* pState )
	{
		int nCurrentStack = 1;

		const float fOutTime = static_cast< float >( lua_tonumber( pState, nCurrentStack++ ) );
		const DWORD dwCloseButtonParam = static_cast< DWORD >( lua_tonumber( pState, nCurrentStack++ ) );

		lua_getglobal( pState, "instanceMapID" );
		const InstanceMapID _instanceMapID = static_cast< DWORD >( lua_tonumber( pState, -1 ) );
		lua_pop( pState, 1 );

		GLLandManager* const pLandMan = gpGaeaServer->GetLandManager( _instanceMapID );
		if ( NULL == pLandMan )
			return 0;

		GLMSG::NET_INSTANCE_COMPETITION_RESULT_END_INFO_FC netMsg;
		netMsg.fOutTime = fOutTime;
		netMsg.nCloseButtonParam = dwCloseButtonParam;

		msgpack::sbuffer buf;
		msgpack::pack( buf, netMsg );

		pLandMan->sendMsgPC( NET_MSG_GCTRL_INSTANCE_CONTENTS_MSGPACK, buf );

		return 1;
	}


	/************************************************************************/
	/* Timer 관련 Invoke 함수 목록;                                        */
	/************************************************************************/

	int InvokeFunction_ShowTimeUI ( lua_State* pState )
	{
		int currentStack = 1;

		const DWORD dwChaDBNum = static_cast< DWORD >( lua_tonumber( pState, currentStack++ ) );
		const DWORD dwTime = static_cast< DWORD >(
			lua_tonumber( pState, currentStack++ ) );
		const bool bShow = lua_toboolean( pState, currentStack++ );

		// Instance ID;
		lua_getglobal( pState, "instanceMapID" );
		const InstanceMapID sInstanceMapID = static_cast< DWORD >( lua_tonumber( pState, -1 ) );
		lua_pop( pState, 1 );

		GLLandManager* pLandManager = gpGaeaServer->GetLandManager( sInstanceMapID );
		if ( NULL == pLandManager )
			return 0;

		InstanceSystem::InstanceField* pInstance =
			pLandManager->GetScriptInstance();
		if ( NULL == pInstance )
			return 0;

		pInstance->ShowTimeUI( dwChaDBNum, static_cast< const float >( dwTime ), bShow );

		return 0;
	}


	/************************************************************************/
	/* Information UI 관련 Invoke 함수 목록;                               */
	/************************************************************************/

	int InvokeFunction_ShowEntranceStateUI ( lua_State* pState )
	{
		int currentStack = 1;

		// Faction ID;
		Faction::FactionID sFactionID;
		const DWORD emFactionType = static_cast< DWORD >(
			lua_tonumber( pState, currentStack++ ) );
		const DWORD dwFactionID = static_cast< DWORD >(
			lua_tonumber( pState, currentStack++ ) );

		sFactionID.factionType = static_cast< Faction::EMFACTION_TYPE >( emFactionType );
		sFactionID.factionID_Num = dwFactionID;

		const DWORD dwCurCount = static_cast< DWORD >(
			lua_tonumber( pState, currentStack++ ) );
		const DWORD dwMaxCount = static_cast< DWORD >(
			lua_tonumber( pState, currentStack++ ) );
		const DWORD dwTime = static_cast< DWORD >(
			lua_tonumber( pState, currentStack++ ) );
		const bool bShow = lua_toboolean( pState, currentStack++ );

		// Instance ID;
		lua_getglobal( pState, "instanceMapID" );
		const InstanceMapID sInstanceMapID = static_cast< DWORD >( lua_tonumber( pState, -1 ) );
		lua_pop( pState, 1 );

		GLLandManager* pLandManager = gpGaeaServer->GetLandManager( sInstanceMapID );
		if ( NULL == pLandManager )
			return 0;

		InstanceSystem::InstanceField* pInstance =
			pLandManager->GetScriptInstance();
		if ( NULL == pInstance )
			return 0;

		pInstance->ShowEntranceStateUI( sFactionID, dwCurCount, dwMaxCount, static_cast< const float >( dwTime ), bShow );

		return 0;
	}

}