#include "pch.h"

#include "../../../EngineLib/GUInterface/GameTextControl.h"

#include "../../../RanLogic/Msg/GLContrlInstanceContentsMsg.h"

#include "../../FieldServer/GLGaeaServer.h"
#include "../../Database/DBAction/DbActionLogInstantDungeon.h"

#include "../LogicField/InstanceSystemField.h"

#include "../InstanceSystem.h"

#include "./InvokeFunctionDBLog.h"

namespace InstanceSystem
{
	void RegistInvokeFunction_DBLog_Instance ( sc::LuaInstance& _sLua )
	{
		_sLua.RegistFunction( "Log_EnterUser", InstanceSystem::InvokeFunction_DBActionEnterUser );
		_sLua.RegistFunction( "Log_ReenterUser", InstanceSystem::InvokeFunction_DBActionReenterUser );
		_sLua.RegistFunction( "Log_OutUser", InstanceSystem::InvokeFunction_DBActionOutUser );
		_sLua.RegistFunction( "Log_NextStage", InstanceSystem::InvokeFunction_DBActionNextStage );
		_sLua.RegistFunction( "Log_Retry", InstanceSystem::InvokeFunction_DBActionRetry );
		_sLua.RegistFunction( "Log_ResultAccumulate", InstanceSystem::InvokeFunction_DBActionResultAccumulate );
		_sLua.RegistFunction( "Log_Reward", InstanceSystem::InvokeFunction_DBActionReward );
	}

	void RegistInvokeFunction_DBLog_Agent ( sc::LuaInstance& _sLua )
	{
	}

	void RegistInvokeFunction_DBLog_Agent ( InstanceSystem::ScriptManager* const _pScriptManager )
	{
		if ( NULL == _pScriptManager )
			return;

		_pScriptManager->registFunctionCommon( RegistInvokeFunction_DBLog_Agent );
	}

	void RegistInvokeFunction_DBLog_Field ( sc::LuaInstance& _sLua )
	{
	}

	void RegistInvokeFunction_DBLog_Field ( InstanceSystem::ScriptManager* const _pScriptManager )
	{
		if ( NULL == _pScriptManager )
			return;

		_pScriptManager->registFunctionCommon( RegistInvokeFunction_DBLog_Field );
	}


	/************************************************************************/
	/* Progress Log;                                                        */
	/************************************************************************/

	int _DBAction_EnterAndOut ( db::DBAction_ProgressStage::EMSUBTYPE_LOG _emLogType, lua_State* _pState )
	{
		if ( NULL == gpGaeaServer )
			return 0;

		int nCurrentStack = 1;

		const char* strContentsName = NULL;
		strContentsName = sc::GetStringToLuaState( _pState, nCurrentStack++ );

		const DWORD dwContentsID = static_cast< DWORD >( lua_tonumber( _pState, nCurrentStack++ ) );
		const DWORD dwCharDbNum = static_cast< DWORD >( lua_tonumber( _pState, nCurrentStack++ ) );
		const DWORD dwStageNum = static_cast< DWORD >( lua_tonumber( _pState, nCurrentStack++ ) );
		const DWORD dwAllUserCount = static_cast< DWORD >( lua_tonumber( _pState, nCurrentStack++ ) );
		const DWORD dwProgressUserCount = static_cast< DWORD >( lua_tonumber( _pState, nCurrentStack++ ) );
		const DWORD dwRetryCount = static_cast< DWORD >( lua_tonumber( _pState, nCurrentStack++ ) );

		GLChar* const pChar = gpGaeaServer->GetCharByDbNum( dwCharDbNum );
		if ( NULL == pChar )
			return 0;

		lua_getglobal( _pState, "instanceMapID" );
		const InstanceMapID nInstanceMapID =
			static_cast< DWORD >( lua_tonumber( _pState, -1 ) );
		lua_pop( _pState, 1 );

		GLLandManager* const pLandMan = gpGaeaServer->GetLandManager( nInstanceMapID );
		if ( NULL == pLandMan )
			return 0;

		db::DBAction_ProgressStage* pDBAction =
			new db::DBAction_ProgressStage(
			pLandMan->getGUID(),
			strContentsName,
			dwContentsID,
			_emLogType,
			pChar->GetName(),
			dwCharDbNum,
			pChar->GetLevel(),
			static_cast< DWORD >( pChar->m_emClass ),
			dwStageNum,
			dwAllUserCount,
			dwProgressUserCount,
			dwRetryCount );
		if ( NULL == pDBAction )
			return 0;

		gpGaeaServer->AddLogAdoJob( db::DbActionPtr( pDBAction ) );

		return 0;
	}

	int InvokeFunction_DBActionEnterUser ( lua_State* _pState )
	{
		db::DBAction_ProgressStage::EMSUBTYPE_LOG emLogType = db::DBAction_ProgressStage::EMTYPE_ENTER;

		return _DBAction_EnterAndOut( emLogType, _pState );
	}

	int InvokeFunction_DBActionReenterUser ( lua_State* _pState )
	{
		db::DBAction_ProgressStage::EMSUBTYPE_LOG emLogType = db::DBAction_ProgressStage::EMTYPE_REENTER;

		return _DBAction_EnterAndOut( emLogType, _pState );
	}

	int InvokeFunction_DBActionOutUser ( lua_State* _pState )
	{
		db::DBAction_ProgressStage::EMSUBTYPE_LOG emLogType = db::DBAction_ProgressStage::EMTYPE_OUT;

		return _DBAction_EnterAndOut( emLogType, _pState );
	}

	int InvokeFunction_DBActionNextStage ( lua_State* _pState )
	{
		if ( NULL == gpGaeaServer )
			return 0;

		int nCurrentStack = 1;

		const char* strContentsName = NULL;
		strContentsName = sc::GetStringToLuaState( _pState, nCurrentStack++ );

		const DWORD dwContentsID = static_cast< DWORD >( lua_tonumber( _pState, nCurrentStack++ ) );
		const DWORD dwStageNum = static_cast< DWORD >( lua_tonumber( _pState, nCurrentStack++ ) );
		const DWORD dwAllUserCount = static_cast< DWORD >( lua_tonumber( _pState, nCurrentStack++ ) );
		const DWORD dwProgressUserCount = static_cast< DWORD >( lua_tonumber( _pState, nCurrentStack++ ) );
		const DWORD dwRetryCount = static_cast< DWORD >( lua_tonumber( _pState, nCurrentStack++ ) );

		lua_getglobal( _pState, "instanceMapID" );
		const InstanceMapID nInstanceMapID =
			static_cast< DWORD >( lua_tonumber( _pState, -1 ) );
		lua_pop( _pState, 1 );

		GLLandManager* const pLandMan = gpGaeaServer->GetLandManager( nInstanceMapID );
		if ( NULL == pLandMan )
			return 0;

		db::DBAction_ProgressStage* pDBAction =
			new db::DBAction_ProgressStage(
			pLandMan->getGUID(),
			strContentsName,
			dwContentsID,
			dwStageNum,
			dwAllUserCount,
			dwProgressUserCount,
			dwRetryCount );
		if ( NULL == pDBAction )
			return 0;

		gpGaeaServer->AddLogAdoJob( db::DbActionPtr( pDBAction ) );

		return 0;
	}

	int InvokeFunction_DBActionRetry ( lua_State* _pState )
	{
		if ( NULL == gpGaeaServer )
			return 0;

		int nCurrentStack = 1;

		const char* strContentsName = NULL;
		strContentsName = sc::GetStringToLuaState( _pState, nCurrentStack++ );

		const DWORD dwContentsID = static_cast< DWORD >( lua_tonumber( _pState, nCurrentStack++ ) );
		const LONGLONG llRetryMoney = static_cast< LONGLONG >( lua_tonumber( _pState, nCurrentStack++ ) );
		const DWORD dwStageNum = static_cast< DWORD >( lua_tonumber( _pState, nCurrentStack++ ) );
		const DWORD dwAllUserCount = static_cast< DWORD >( lua_tonumber( _pState, nCurrentStack++ ) );
		const DWORD dwProgressUserCount = static_cast< DWORD >( lua_tonumber( _pState, nCurrentStack++ ) );
		const DWORD dwRetryCount = static_cast< DWORD >( lua_tonumber( _pState, nCurrentStack++ ) );

		lua_getglobal( _pState, "instanceMapID" );
		const InstanceMapID nInstanceMapID =
			static_cast< DWORD >( lua_tonumber( _pState, -1 ) );
		lua_pop( _pState, 1 );

		GLLandManager* const pLandMan = gpGaeaServer->GetLandManager( nInstanceMapID );
		if ( NULL == pLandMan )
			return 0;

		db::DBAction_ProgressStage* pDBAction =
			new db::DBAction_ProgressStage(
			pLandMan->getGUID(),
			strContentsName,
			dwContentsID,
			llRetryMoney,
			dwStageNum,
			dwAllUserCount,
			dwProgressUserCount,
			dwRetryCount );
		if ( NULL == pDBAction )
			return 0;

		gpGaeaServer->AddLogAdoJob( db::DbActionPtr( pDBAction ) );

		return 0;
	}


	/************************************************************************/
	/* Result Log;                                     */
	/************************************************************************/

	int InvokeFunction_DBActionResultAccumulate ( lua_State* _pState )
	{
		if ( NULL == gpGaeaServer )
			return 0;

		int nCurrentStack = 1;

		const char* strContentsName = NULL;
		strContentsName = sc::GetStringToLuaState( _pState, nCurrentStack++ );

		const DWORD dwContentsID = static_cast< DWORD >( lua_tonumber( _pState, nCurrentStack++ ) );
		const DWORD dwCharDbNum = static_cast< DWORD >( lua_tonumber( _pState, nCurrentStack++ ) );
		const DWORD dwStageNum = static_cast< DWORD >( lua_tonumber( _pState, nCurrentStack++ ) );
		const DWORD dwReviveCount = static_cast< DWORD >( lua_tonumber( _pState, nCurrentStack++ ) );
		const DWORD dwDamage = static_cast< DWORD >( lua_tonumber( _pState, nCurrentStack++ ) );
		const DWORD dwHeal = static_cast< DWORD >( lua_tonumber( _pState, nCurrentStack++ ) );
		const bool bMVP = static_cast< bool >( lua_tonumber( _pState, nCurrentStack++ ) );

		GLChar* const pChar = gpGaeaServer->GetCharByDbNum( dwCharDbNum );
		if ( NULL == pChar )
			return 0;

		lua_getglobal( _pState, "instanceMapID" );
		const InstanceMapID nInstanceMapID =
			static_cast< DWORD >( lua_tonumber( _pState, -1 ) );
		lua_pop( _pState, 1 );

		GLLandManager* const pLandMan = gpGaeaServer->GetLandManager( nInstanceMapID );
		if ( NULL == pLandMan )
			return 0;

		db::DBAction_ResultAccumulate* pDBAction =
			new db::DBAction_ResultAccumulate(
			pLandMan->getGUID(),
			strContentsName,
			dwContentsID,
			pChar->GetName(),
			dwCharDbNum,
			pChar->GetLevel(),
			static_cast< DWORD >( pChar->GETCHARINDEX() ),
			dwStageNum,
			dwReviveCount,
			dwDamage,
			dwHeal,
			bMVP );
		if ( NULL == pDBAction )
			return 0;

		gpGaeaServer->AddLogAdoJob( db::DbActionPtr( pDBAction ) );

		return 0;
	}


	/************************************************************************/
	/* Reward Log;                                     */
	/************************************************************************/

	int InvokeFunction_DBActionReward ( lua_State* _pState )
	{
		if ( NULL == gpGaeaServer )
			return 0;

		int nCurrentStack = 1;

		const char* strContentsName = NULL;
		strContentsName = sc::GetStringToLuaState( _pState, nCurrentStack++ );

		const DWORD dwContentsID = static_cast< DWORD >( lua_tonumber( _pState, nCurrentStack++ ) );
		const DWORD dwCharDbNum = static_cast< DWORD >( lua_tonumber( _pState, nCurrentStack++ ) );
		const LONGLONG llRewardMoney = static_cast< LONGLONG >( lua_tonumber( _pState, nCurrentStack++ ) );
		const DWORD dwRewardEXP = static_cast< DWORD >( lua_tonumber( _pState, nCurrentStack++ ) );
		const DWORD dwRewardItemCount = static_cast< DWORD >( lua_tonumber( _pState, nCurrentStack++ ) );

		const char* strRewardItem1 = NULL;
		strRewardItem1 = sc::GetStringToLuaState( _pState, nCurrentStack++ );

		const char* strRewardItem2 = NULL;
		strRewardItem2 = sc::GetStringToLuaState( _pState, nCurrentStack++ );

		const char* strRewardItem3 = NULL;
		strRewardItem3 = sc::GetStringToLuaState( _pState, nCurrentStack++ );

		const char* strRewardItem4 = NULL;
		strRewardItem4 = sc::GetStringToLuaState( _pState, nCurrentStack++ );

		const char* strRewardItem5 = NULL;
		strRewardItem5 = sc::GetStringToLuaState( _pState, nCurrentStack++ );

		GLChar* const pChar = gpGaeaServer->GetCharByDbNum( dwCharDbNum );
		if ( NULL == pChar )
			return 0;

		lua_getglobal( _pState, "instanceMapID" );
		const InstanceMapID nInstanceMapID =
			static_cast< DWORD >( lua_tonumber( _pState, -1 ) );
		lua_pop( _pState, 1 );

		GLLandManager* const pLandMan = gpGaeaServer->GetLandManager( nInstanceMapID );
		if ( NULL == pLandMan )
			return 0;

		db::DBAction_Reward* pDBAction =
			new db::DBAction_Reward(
			pLandMan->getGUID(),
			strContentsName,
			dwContentsID,
			pChar->GetName(),
			llRewardMoney,
			dwCharDbNum,
			pChar->GetLevel(),
			static_cast< DWORD >( pChar->GETCHARINDEX() ),
			dwRewardEXP,
			dwRewardItemCount );
		if ( NULL == pDBAction )
			return 0;

		pDBAction->AddRewardItem( 0, strRewardItem1 );
		pDBAction->AddRewardItem( 1, strRewardItem2 );
		pDBAction->AddRewardItem( 2, strRewardItem3 );
		pDBAction->AddRewardItem( 3, strRewardItem4 );
		pDBAction->AddRewardItem( 4, strRewardItem5 );

		gpGaeaServer->AddLogAdoJob( db::DbActionPtr( pDBAction ) );

		return 0;
	}

}