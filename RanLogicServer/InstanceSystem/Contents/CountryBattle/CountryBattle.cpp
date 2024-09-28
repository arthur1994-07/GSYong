#include "pch.h"

#include "../../../FieldServer/GLGaeaServer.h"

#include "../../../../RanLogic/Msg/GLContrlInstanceContentsMsg.h"
#include "../../InstanceSystem.h"
#include "../../LogicField/InstanceSystemField.h"

#include "../../../Database/DBAction/DbActionLogInstantDungeon.h"

#include "../../../../EngineLib/GUInterface/GameTextControl.h"

#include "../../../Country/GLCountryManServer.h"
#include "../../../Country/GLVictoriousCountryManServer.h"
#include "../../../Country/GLVictoriousCountryManField.h"

#include "CountryBattle.h"

namespace InstanceSystem
{
	void registFunction_CTI_Instance ( sc::LuaInstance& _luaInstance )
	{
		_luaInstance.RegistFunction( "UI_CTI_Auth", InstanceSystem::UI_CTI_Auth );
		_luaInstance.RegistFunction( "UI_CTI_Progress", InstanceSystem::UI_CTI_Progress );
		_luaInstance.RegistFunction( "UI_CTI_Progress_All", InstanceSystem::UI_CTI_Progress_All);
		_luaInstance.RegistFunction( "UI_CTI_Rank", InstanceSystem::UI_CTI_Rank );
		_luaInstance.RegistFunction( "UI_CTI_Rank_MyInfo", InstanceSystem::UI_CTI_Rank_MyInfo );
		_luaInstance.RegistFunction( "UI_CTI_MiniMap", InstanceSystem::UI_CTI_MiniMap );
		_luaInstance.RegistFunction( "UI_CTI_MiniMap_All", InstanceSystem::UI_CTI_MiniMap_All );
		_luaInstance.RegistFunction( "UI_CompetitionResult_CTI_Begin", InstanceSystem::UI_CompetitionResult_CTI_Begin );
		_luaInstance.RegistFunction( "UI_CompetitionResult_CTI_Info", InstanceSystem::UI_CompetitionResult_CTI_Info );
		_luaInstance.RegistFunction( "UI_CompetitionResult_CTI_End", InstanceSystem::UI_CompetitionResult_CTI_End );
		_luaInstance.RegistFunction( "Log_CTI_In", InstanceSystem::Log_CTI_In );
		_luaInstance.RegistFunction( "Log_CTI_Out", InstanceSystem::Log_CTI_Out );
		_luaInstance.RegistFunction( "Log_CTI_Result", InstanceSystem::Log_CTI_Result );
	}

	void registFunction_CTI_Instance ( InstanceSystem::ScriptManager* const _pScriptManager )
	{
		if ( NULL == _pScriptManager )
			return;

		_pScriptManager->registFunctionCommon( registFunction_CTI_Instance );
	}

	void registFunction_CTI_Agent ( sc::LuaInstance& _luaInstance )
	{
		_luaInstance.RegistFunction ( "ResetOldCTICountry", InstanceSystem::ResetOldCTICountryAgent );
		_luaInstance.RegistFunction ( "SetCTICountry", InstanceSystem::SetCTICountryAgent );
		_luaInstance.RegistFunction ( "GetCTIVCountry", InstanceSystem::GetCTIVCountryAgent );
	}

	void registFunction_CTI_Agent ( InstanceSystem::ScriptManager* const _pScriptManager )
	{
		if ( NULL == _pScriptManager )
			return;

		_pScriptManager->registFunctionCommon( registFunction_CTI_Agent );
	}

	void registFunction_CTI_Field ( sc::LuaInstance& _luaInstance )
	{
		_luaInstance.RegistFunction ( "ResetOldCTICountry", InstanceSystem::ResetOldCTICountryField );
		_luaInstance.RegistFunction ( "SetCTICountry", InstanceSystem::SetCTICountryField );
		_luaInstance.RegistFunction ( "GetCTIVCountry", InstanceSystem::GetCTIVCountryField );
	}

	void registFunction_CTI_Field ( InstanceSystem::ScriptManager* const _pScriptManager )
	{
		if ( NULL == _pScriptManager )
			return;

		_pScriptManager->registFunctionCommon( registFunction_CTI_Field );
	}

	// 현재 점령국 관련 Invoke;
	int ResetOldCTICountryAgent ( lua_State* pState )
	{
		Country::GLVictoriousCountryManServer* pVCountryMan =
			Country::GLVictoriousCountryManServer::Instance();

		if ( pVCountryMan )
			pVCountryMan->ResetDBAction( Country::CAPTURE_THE_ISLANDS );

		return 1;
	}

	int SetCTICountryAgent ( lua_State* pState )
	{
		int currentStack = 1;

		Country::SVICTORIOUS_COUNTRY sVCountry;
		sVCountry.emFlag = Country::CAPTURE_THE_ISLANDS;

		const WORD wMapMID(
			static_cast< WORD >( lua_tonumber( pState, currentStack++ ) ) );
		const WORD wMapSID(
			static_cast< WORD >( lua_tonumber( pState, currentStack++ ) ) );

		const DWORD dwCountryID =
			static_cast< DWORD >(
			lua_tonumber( pState, currentStack++ ) );

		sVCountry.sMapID = SNATIVEID( wMapMID, wMapSID );
		sVCountry.sCountryInfo.dwID = dwCountryID;

		Country::GLVictoriousCountryManServer* pVCountryMan =
			Country::GLVictoriousCountryManServer::Instance();

		if ( pVCountryMan )
		{
			pVCountryMan->AddDBAction( sVCountry );
		}

		return 1;
	}

	int GetCTIVCountryAgent ( lua_State* pState )
	{
		int currentStack = 1;

		Country::GLVictoriousCountryManServer* pVCountryMan =
			Country::GLVictoriousCountryManServer::Instance();

		if ( NULL == pVCountryMan )
			return 0;

		const DWORD dwCountryID =
			pVCountryMan->GetVCountryFirst(
			Country::CAPTURE_THE_ISLANDS );

		lua_pushinteger( pState, dwCountryID );

		return 1;
	}

	int ResetOldCTICountryField ( lua_State* pState )
	{
		int currentStack = 1;

		bool bTempVCountry = false;
		bTempVCountry = lua_toboolean( pState, currentStack++ );

		Country::GLVictoriousCountryManField* pVCountryMan =
			Country::GLVictoriousCountryManField::Instance();

		if ( pVCountryMan )
		{
			pVCountryMan->SendResetVCountryFA(
				Country::CAPTURE_THE_ISLANDS,
				bTempVCountry );
		}

		return 1;
	}

	int SetCTICountryField ( lua_State* pState )
	{
		int currentStack = 1;

		Country::SVICTORIOUS_COUNTRY sVCountry;
		sVCountry.emFlag = Country::CAPTURE_THE_ISLANDS; 

		const WORD wMapMID(
			static_cast< WORD >( lua_tonumber( pState, currentStack++ ) ) );
		const WORD wMapSID(
			static_cast< WORD >( lua_tonumber( pState, currentStack++ ) ) );

		const DWORD dwCountryID =
			static_cast< DWORD >(
			lua_tonumber( pState, currentStack++ ) );

		bool bTempVCountry = false;
		bTempVCountry = lua_toboolean( pState, currentStack++ );

		sVCountry.sMapID = SNATIVEID( wMapMID, wMapSID );
		sVCountry.sCountryInfo.dwID = dwCountryID;

		Country::GLVictoriousCountryManField* pVCountryMan =
			Country::GLVictoriousCountryManField::Instance();

		if ( pVCountryMan )
		{
			pVCountryMan->SendAddVCountryFA( sVCountry, bTempVCountry );
		}

		return 1;
	}

	int GetCTIVCountryField ( lua_State* pState )
	{
		int currentStack = 1;

		Country::GLVictoriousCountryManField* pVCountryMan =
			Country::GLVictoriousCountryManField::Instance();

		if ( NULL == pVCountryMan )
			return 0;
		
		const DWORD dwCountryID =
			pVCountryMan->GetVCountryFirst(
			Country::CAPTURE_THE_ISLANDS );

		lua_pushinteger( pState, dwCountryID );

		return 1;
	}


	/************************************************************************/
	/* 인증 UI 관련 Invoke 함수 목록;                                     */
	/************************************************************************/
	int UI_CTI_Auth ( lua_State* pState )
	{
		int currentStack = 1;

		const DWORD _CharDbNum(
			static_cast< DWORD >( lua_tonumber( pState, currentStack++ ) ) );
		const bool _bVisible(
			lua_toboolean( pState, currentStack++ ) );
		const float _fAuthTime(
			static_cast< float >( lua_tonumber( pState, currentStack++ ) ) );
		//const int _iResult(lua_tointeger(pState, currentStack++));

		GLChar* _pChar = gpGaeaServer->GetCharByDbNum( _CharDbNum );
		if ( NULL == _pChar )
			return 0;

		GLMSG::NET_INSTANCE_CTI_UI_AUTH_FC netMsg;
		netMsg.bVisible = _bVisible;
		netMsg.fAuthTime = _fAuthTime;

		gpGaeaServer->SENDTOCLIENT( _pChar->ClientSlot(), &netMsg );

		return 1;
	}


	/************************************************************************/
	/* 프로그래스바 UI 관련 Invoke 함수 목록;                            */
	/************************************************************************/
	int UI_CTI_Progress ( lua_State* pState )
	{
		int currentStack = 1;

		const DWORD _CharDbNum(
			static_cast< DWORD >( lua_tonumber( pState, currentStack++ ) ) );
		const int _iNameIndex( lua_tointeger( pState, currentStack++ ) );
		const int _iProgressIndex( lua_tointeger( pState, currentStack++ ) );
		const int _iPoint( lua_tointeger( pState, currentStack++ ) );
		const float _fRemainTime(
			static_cast< float >( lua_tonumber(pState, currentStack++) ) );

		GLChar* _pChar = gpGaeaServer->GetCharByDbNum( _CharDbNum );
		if ( NULL == _pChar )
			return 0;

		GLMSG::NET_INSTANCE_CTI_UI_PROGRESS_FC netMsg; 
		netMsg.iNameIndex = _iNameIndex;
		netMsg.iProgressIndex = _iProgressIndex;
		netMsg.iPoint = _iPoint;
		netMsg.fRemainTime = _fRemainTime;

		gpGaeaServer->SENDTOCLIENT( _pChar->ClientSlot(), &netMsg );

		return 1;
	}

	int UI_CTI_Progress_All ( lua_State* pState )
	{
		int currentStack = 1;

		lua_getglobal(pState, "instanceMapID");
		const InstanceMapID _instanceMapID(
			static_cast< DWORD >( lua_tonumber( pState, -1 ) ) );
		lua_pop(pState, 1);

		GLLandManager* const _pLandMan =
			gpGaeaServer->GetLandManager( _instanceMapID );
		if ( NULL == _pLandMan )
			return 0;

		const int _iNameIndex( lua_tointeger( pState, currentStack++ ) );
		const int _iProgressIndex( lua_tointeger( pState, currentStack++ ) );
		const int _iPoint( lua_tointeger( pState, currentStack++ ) );
		const float _fRemainTime(
			static_cast< float >( lua_tonumber(pState, currentStack++) ) );

		GLMSG::NET_INSTANCE_CTI_UI_PROGRESS_FC netMsg; 
		netMsg.iNameIndex = _iNameIndex;
		netMsg.iProgressIndex = _iProgressIndex;
		netMsg.iPoint = _iPoint;
		netMsg.fRemainTime = _fRemainTime;

		_pLandMan->sendMsgPC( &netMsg );

		return 1;
	}


	/************************************************************************/
	/* 순위 UI 관련 Invoke 함수 목록;                                     */
	/************************************************************************/
	int UI_CTI_Rank ( lua_State* pState )
	{
		int currentStack = 1;

		lua_getglobal( pState, "instanceMapID" );
		const InstanceMapID _instanceMapID(
			static_cast< DWORD >( lua_tonumber( pState, -1 ) ) );
		lua_pop( pState, 1 );

		GLLandManager* const _pLandMan =
			gpGaeaServer->GetLandManager( _instanceMapID );
		if ( NULL == _pLandMan )
			return 0;

		LuaPlus::LuaObject _CountryTable(
			LuaPlus::LuaState::CastState( pState ),
			currentStack++ );

		if ( false == _CountryTable.IsTable() )
			return 0;

		GLMSG::NET_INSTANCE_CTI_UI_RANK_FC netMsg;

		const int MAX_SHOW_RANK = 5; // 최대 표시 순위 (현재 5위까지 );
		int i = 0;
		for ( LuaPlus::LuaTableIterator _iter( _CountryTable ); _iter && i < MAX_SHOW_RANK; _iter.Next(), i++ )
		{
			LuaPlus::LuaObject _valueObj = _iter.GetValue();
			if ( false == _valueObj.IsTable() )
				return 0;

			LuaPlus::LuaTableIterator _dataIter( _valueObj );
			LuaPlus::LuaObject _countryObj = _dataIter.GetValue();
			if ( false == _countryObj.IsInteger() )
				return 0;
			DWORD _dwCountryID = (DWORD)_countryObj.GetNumber();

			if ( false == _dataIter.Next() )
				return 0;

			LuaPlus::LuaObject _pointObj = _dataIter.GetValue();
			if ( false == _pointObj.IsInteger() )
				return 0;
			int _iClubPoint = _pointObj.GetInteger();

			if ( false == _dataIter.Next() )
				return 0;

			LuaPlus::LuaObject _rankObj = _dataIter.GetValue();
			if ( false == _rankObj.IsInteger() )
				return 0;
			int _iClubRank = _rankObj.GetInteger();

			Country::GLCountry* pCountry = Country::GLCountryManServer::Instance()
				->GetCountryUsingID( _dwCountryID );
			if ( NULL == pCountry )
				return 0;

			GLMSG::NET_INSTANCE_CTI_UI_RANK_FC::RankInfo _rankinfo;
			_rankinfo.countryName = pCountry->GetCountryName();
			_rankinfo.iPoint = _iClubPoint;
			_rankinfo.iRank = _iClubRank;

			netMsg.rankVector.push_back( _rankinfo );
		}

		msgpack::sbuffer buf;
		msgpack::pack( buf, netMsg );

		_pLandMan->sendMsgPC( NET_MSG_GCTRL_INSTANCE_CONTENTS_MSGPACK, buf );

		return 1;
	}

	int UI_CTI_Rank_MyInfo(lua_State* pState)
	{
		int currentStack = 1;

		lua_getglobal( pState, "instanceMapID" );
		const InstanceMapID _instanceMapID(
			static_cast< DWORD >( lua_tonumber( pState, -1 ) ) );
		lua_pop( pState, 1 );

		GLLandManager* const _pLandMan =
			gpGaeaServer->GetLandManager( _instanceMapID );
		if ( NULL == _pLandMan )
			return 0;

		const int _factionType( static_cast< DWORD >(
			lua_tonumber( pState, currentStack++ ) ) );
		const int _factionID( static_cast< DWORD >(
			lua_tonumber( pState, currentStack++ ) ) );
		const int _iPoint( lua_tointeger( pState, currentStack++ ) );
		const int _iRank( lua_tointeger( pState, currentStack++ ) );

		GLMSG::NET_INSTANCE_CTI_UI_RANK_MY_INFO_FC netMsg;
		netMsg.iMyCountryPoint = _iPoint;
		netMsg.iMyCountryRank = _iRank;

		_pLandMan->sendMsgFaction(
			Faction::FactionID(_factionType, _factionID),
			&netMsg );

		return 1;
	}


	/************************************************************************/
	/* Minimap 관련 Invoke 함수 목록;                                      */
	/************************************************************************/
	const bool Make_CTI_MiniMap_Message (
		lua_State* pState,
		int& _currentStack,
		msgpack::sbuffer& buf )
	{
		// Lua쪽으로부터 정의된 형태의 인증기정보Table과 가속기정보Table을 받는것을 전제로 하고 있다;
		// 인증기 정보 테이블 : { 인증국가ID, 인증기 X좌표, 인증기 Y좌표 };
		// 가속기 정보 테이블 : { 가속기활성화 여부, 가속기 X좌표, 가속기 X좌표 };

		const WORD _wMapMID(
			static_cast< WORD >( lua_tonumber( pState, _currentStack++ ) ) );
		const WORD _wMapSID(
			static_cast< WORD >( lua_tonumber( pState, _currentStack++ ) ) );

		GLMSG::NET_INSTANCE_CTI_UI_MINIMAP_FC netMsg;
		netMsg.dwGuidanceMapID = SNATIVEID( _wMapMID, _wMapSID ).dwID;

		LuaPlus::LuaObject AuthInfoTable(
			LuaPlus::LuaState::CastState( pState ),
			_currentStack++ );

		if ( AuthInfoTable.IsTable() )
		{
			for ( LuaPlus::LuaTableIterator _iter( AuthInfoTable ); _iter; _iter.Next() )
			{
				LuaPlus::LuaObject _valueObj = _iter.GetValue();
				if ( false == _valueObj.IsTable() )
					return false;

				LuaPlus::LuaTableIterator _dataIter( _valueObj );
				LuaPlus::LuaObject _countryObj = _dataIter.GetValue();
				if ( false == _countryObj.IsNumber() )
					return false;

				DWORD _dwCountryID = static_cast< DWORD >( _countryObj.GetNumber() );

				if ( false == _dataIter.Next() )
					return false;

				LuaPlus::LuaObject _xPosObj = _dataIter.GetValue();
				if ( false == _xPosObj.IsNumber() )
					return false;

				float _iXpos = static_cast< float >( _xPosObj.GetNumber() );

				if ( false == _dataIter.Next() )
					return false;

				LuaPlus::LuaObject _yPosObj = _dataIter.GetValue();
				if ( false == _yPosObj.IsNumber() )
					return false;

				float _iYpos = static_cast< float >( _yPosObj.GetNumber() );

				GLMSG::NET_INSTANCE_CTI_UI_MINIMAP_FC::AuthCountryInfo _authCountryInfo;
				_authCountryInfo.sCountryInfo.dwID = _dwCountryID;
				_authCountryInfo.fXpos = _iXpos;
				_authCountryInfo.fZpos = _iYpos;

				netMsg.vecAuthCountry.push_back( _authCountryInfo );
			}
		}

		// 인증 가속기 현황표시;
		LuaPlus::LuaObject AcceleratorInfoTable(
			LuaPlus::LuaState::CastState(pState),
			_currentStack++ );
		if ( AcceleratorInfoTable.IsTable() )
		{
			for ( LuaPlus::LuaTableIterator _iter( AcceleratorInfoTable ); _iter; _iter.Next() )
			{
				LuaPlus::LuaObject _valueObj = _iter.GetValue();
				if ( false == _valueObj.IsTable() )
					return false;

				LuaPlus::LuaTableIterator _dataIter( _valueObj );
				LuaPlus::LuaObject _countryObj = _dataIter.GetValue();
				if ( false == _countryObj.IsBoolean() )
					return false;

				const bool _bAcivateAccelerator = _countryObj.GetBoolean();
				if ( false == _bAcivateAccelerator )
					continue;

				if ( false == _dataIter.Next() )
					return false;

				LuaPlus::LuaObject _xPosObj = _dataIter.GetValue();
				if ( false == _xPosObj.IsNumber() )
					return false;

				float _iXpos = static_cast< float >( _xPosObj.GetNumber() );

				if ( false == _dataIter.Next() )
					return false;

				LuaPlus::LuaObject _yPosObj = _dataIter.GetValue();
				if ( false == _yPosObj.IsNumber() )
					return false;

				float _iYpos = static_cast< float >( _yPosObj.GetNumber() );

				GLMSG::NET_INSTANCE_CTI_UI_MINIMAP_FC::AcceleratorInfo _AcceleratorInfo;
				_AcceleratorInfo.fXpos = _iXpos;
				_AcceleratorInfo.fZpos = _iYpos;

				netMsg.vecAccelerator.push_back( _AcceleratorInfo );
			}
		}

		msgpack::pack( buf, netMsg );

		return true;
	}

	int UI_CTI_MiniMap ( lua_State* pState )
	{
		int currentStack = 1;

		msgpack::sbuffer buf;

		const DWORD _CharDbNum(
			static_cast< DWORD >( lua_tonumber( pState, currentStack++ ) ) );

		GLChar* _pChar = gpGaeaServer->GetCharByDbNum( _CharDbNum );
		if ( NULL == _pChar )
			return 0;

		if ( false == Make_CTI_MiniMap_Message( pState, currentStack, buf ) )
			return 0;

		gpGaeaServer->SENDTOCLIENT(
			_pChar->ClientSlot(),
			NET_MSG_GCTRL_INSTANCE_CONTENTS_MSGPACK,
			buf );

		return 1;
	}

	int UI_CTI_MiniMap_All ( lua_State* pState )
	{
		int currentStack = 1;

		lua_getglobal( pState, "instanceMapID" );
		const InstanceMapID _instanceMapID(
			static_cast< DWORD >( lua_tonumber( pState, -1 ) ) );
		lua_pop( pState, 1 );

		GLLandManager* const _pLandMan =
			gpGaeaServer->GetLandManager( _instanceMapID );
		if ( NULL == _pLandMan )
			return 0;

		msgpack::sbuffer buf;
		if ( false == Make_CTI_MiniMap_Message( pState, currentStack, buf ) )
			return 0;

		_pLandMan->sendMsgPC( NET_MSG_GCTRL_INSTANCE_CONTENTS_MSGPACK, buf );

		return 1;
	}


	/************************************************************************/
	/* CompetitionResult 관련 Invoke 함수 목록;                            */
	/************************************************************************/
	int UI_CompetitionResult_CTI_Begin ( lua_State* pState )
	{
		lua_getglobal( pState, "instanceMapID" );
		const InstanceMapID _instanceMapID(
			static_cast< DWORD >( lua_tonumber( pState, -1 ) ) );
		lua_pop( pState, 1 );

		GLLandManager* const _pLandMan =
			gpGaeaServer->GetLandManager( _instanceMapID );
		if ( NULL == _pLandMan )
			return 0;

		GLMSG::NET_INSTANCE_COMPETITION_RESULT_TAB_FC netMsg;

		netMsg.typeCompetition = InstanceSystem::COMPETITION_CTI;

		// 전투 결과;
		netMsg.sTabInfo.strTabName = ID2SERVERTEXT( "RN_COMPETITION_RESULT_TAB",0 );
		netMsg.sTabInfo.emSortType = RnCompetitionResultTabInfo::EM_RESULT_SOR_ASC;
		netMsg.sTabInfo.wSortTitleNum = 0;
		netMsg.sTabInfo.emMyResultType = RnCompetitionResultTabInfo::EM_MY_RESULT_COUNTRY;

		netMsg.sTabInfo.sTitle.AddObj( RnCompetitionResultTitleObj::EM_CONTENT_INT, 34, ID2SERVERTEXT( "RN_COMPETITION_RESULT_TITLE", 0 ) );
		netMsg.sTabInfo.sTitle.AddObj( RnCompetitionResultTitleObj::EM_CONTENT_ICON_COUNTRYMARK, 34, ID2SERVERTEXT( "RN_COMPETITION_RESULT_TITLE", 19 ) );
		netMsg.sTabInfo.sTitle.AddObj( RnCompetitionResultTitleObj::EM_CONTENT_STRING, 220, ID2SERVERTEXT( "RN_COMPETITION_RESULT_TITLE", 20 ) );
		netMsg.sTabInfo.sTitle.AddObj( RnCompetitionResultTitleObj::EM_CONTENT_INT, 64, ID2SERVERTEXT( "RN_COMPETITION_RESULT_TITLE", 16 ) );
		netMsg.sTabInfo.sTitle.AddObj( RnCompetitionResultTitleObj::EM_CONTENT_INT, 64, ID2SERVERTEXT( "RN_COMPETITION_RESULT_TITLE", 17 ) );
		netMsg.sTabInfo.sTitle.AddObj( RnCompetitionResultTitleObj::EM_CONTENT_INT, 64, ID2SERVERTEXT( "RN_COMPETITION_RESULT_TITLE", 18 ) );

		msgpack::sbuffer buf;
		msgpack::pack( buf, netMsg );

		_pLandMan->sendMsgPC( NET_MSG_GCTRL_INSTANCE_CONTENTS_MSGPACK, buf );

		return 1;
	}

	int UI_CompetitionResult_CTI_Info ( lua_State* pState )
	{
		int currentStack = 1;

		lua_getglobal( pState, "instanceMapID" );
		const InstanceMapID _instanceMapID(
			static_cast< DWORD >( lua_tonumber( pState, -1 ) ) );
		lua_pop( pState, 1 );

		GLLandManager* const _pLandMan =
			gpGaeaServer->GetLandManager( _instanceMapID );
		if ( NULL == _pLandMan )
			return 0;

		GLMSG::NET_INSTANCE_COMPETITION_RESULT_INFO_FC netMsg;
		netMsg.typeCompetition = InstanceSystem::COMPETITION_CTI;
		netMsg.nTabNumber = 0;

		const int _keyID( static_cast< DWORD >( lua_tonumber( pState, currentStack++ ) ) );
		netMsg.sContents.dwKeyID = _keyID;

		netMsg.sContents.AddContent( lua_tointeger( pState, currentStack++ ) );
		netMsg.sContents.AddContent( lua_tointeger( pState, currentStack++ ) );
		netMsg.sContents.AddContent( sc::GetStringToLuaState( pState, currentStack++ ) );
		netMsg.sContents.AddContent( lua_tointeger( pState, currentStack++ ) );
		netMsg.sContents.AddContent( lua_tointeger( pState, currentStack++ ) );
		netMsg.sContents.AddContent( lua_tointeger( pState, currentStack++ ) );

		msgpack::sbuffer buf;
		msgpack::pack( buf, netMsg );

		_pLandMan->sendMsgPC( NET_MSG_GCTRL_INSTANCE_CONTENTS_MSGPACK, buf );

		return 1;
	}

	int UI_CompetitionResult_CTI_End ( lua_State* pState )
	{
		int currentStack = 1;

		// 강제 나가기 시간 ( 실제 강제 나가기는 따로 구현해야함. 단순 시간표시용 );
		const float _OutTime(
			static_cast< float >( lua_tonumber( pState, currentStack++ ) ) );

		// 나가기 버튼 클릭시 발생하는 커스텀 메시지 Param값;
		const DWORD _CloseButtonParam(
			static_cast< DWORD >( lua_tonumber( pState, currentStack++ ) ) );

		lua_getglobal( pState, "instanceMapID" );
		const InstanceMapID _instanceMapID(
			static_cast< DWORD >( lua_tonumber( pState, -1 ) ) );
		lua_pop( pState, 1 );

		GLLandManager* const _pLandMan =
			gpGaeaServer->GetLandManager( _instanceMapID );
		if ( NULL == _pLandMan )
			return 0;

		GLMSG::NET_INSTANCE_COMPETITION_RESULT_END_INFO_FC netMsg;
		netMsg.typeCompetition = InstanceSystem::COMPETITION_CTI;
		netMsg.fOutTime = _OutTime;
		netMsg.nCloseButtonParam = _CloseButtonParam;

		msgpack::sbuffer buf;
		msgpack::pack(buf, netMsg);

		_pLandMan->sendMsgPC( NET_MSG_GCTRL_INSTANCE_CONTENTS_MSGPACK, buf );

		return 1;
	}


	/************************************************************************/
	/* Log 관련 Invoke 함수 목록;                                          */
	/************************************************************************/
	int Log_CTI_In ( lua_State* pState )
	{
		int currentStack = 1;

		const char* strCTIName = NULL;
		strCTIName = sc::GetStringToLuaState( pState, currentStack++ );

		const DWORD dwCharDbNum =
			static_cast< DWORD >(
			lua_tonumber( pState, currentStack++ ) );

		const DWORD dwCountryID =
			static_cast< DWORD >(
			lua_tonumber( pState, currentStack++ ) );

		const int nPoint = 
			lua_tointeger( pState, currentStack++ );

		lua_getglobal( pState, "instanceMapID" );
		const InstanceMapID instanceMapID =
			static_cast< DWORD >( lua_tonumber( pState, -1 ) );
		lua_pop( pState, 1 );

		GLLandManager* const pLandMan =
			gpGaeaServer->GetLandManager( instanceMapID );
		if ( NULL == pLandMan )
			return 0;

		db::LogInstantCTI* pDbAction =
			new db::LogInstantCTI(
			db::LogInstantCTI::EM_IN,
			pLandMan->getGUID() );

		pDbAction->strCTIName = strCTIName;

		pDbAction->dwCharDbNum = dwCharDbNum;
		pDbAction->dwCountryID = dwCountryID;
		pDbAction->nCountryPoint = nPoint;

		gpGaeaServer->AddLogAdoJob( db::DbActionPtr( pDbAction ) );

		return 1;
	}

	int Log_CTI_Out ( lua_State* pState )
	{
		int currentStack = 1;

		const char* strCTIName = NULL;
		strCTIName = sc::GetStringToLuaState( pState, currentStack++ );

		const DWORD dwCharDbNum =
			static_cast< DWORD >(
			lua_tonumber( pState, currentStack++ ) );

		const DWORD dwCountryID =
			static_cast< DWORD >(
			lua_tonumber( pState, currentStack++ ) );

		const int nPoint = 
			lua_tointeger( pState, currentStack++ );

		lua_getglobal( pState, "instanceMapID" );
		const InstanceMapID instanceMapID =
			static_cast< DWORD >( lua_tonumber( pState, -1 ) );
		lua_pop( pState, 1 );

		GLLandManager* const pLandMan =
			gpGaeaServer->GetLandManager( instanceMapID );
		if ( NULL == pLandMan )
			return 0;

		db::LogInstantCTI* pDbAction =
			new db::LogInstantCTI(
			db::LogInstantCTI::EM_OUT,
			pLandMan->getGUID() );

		pDbAction->strCTIName = strCTIName;

		pDbAction->dwCharDbNum = dwCharDbNum;
		pDbAction->dwCountryID = dwCountryID;
		pDbAction->nCountryPoint = nPoint;

		gpGaeaServer->AddLogAdoJob( db::DbActionPtr( pDbAction ) );

		return 1;
	}

	int Log_CTI_Result ( lua_State* pState )
	{
		int currentStack = 1;

		const char* strCTIName = NULL;
		strCTIName = sc::GetStringToLuaState( pState, currentStack++ );

		const DWORD dwCountryID = static_cast< DWORD >( lua_tonumber( pState, currentStack++ ) );
		const int nPoint = lua_tointeger( pState, currentStack++ );
		const int iCountryHaveAuthNum = lua_tointeger( pState, currentStack++ );
		const int iCountryAuthCount = lua_tointeger( pState, currentStack++ );

		const DWORD dwSecondCountryID = static_cast< DWORD >( lua_tonumber( pState, currentStack++ ) );
		const int nSecondPoint = lua_tointeger( pState, currentStack++ );
		const int iSecondCountryHaveAuthNum = lua_tointeger( pState, currentStack++ );
		const int iSecondCountryAuthCount = lua_tointeger( pState, currentStack++ );

		const DWORD dwThirdCountryID = static_cast< DWORD >( lua_tonumber( pState, currentStack++ ) );
		const int nThirdPoint = lua_tointeger( pState, currentStack++ );
		const int iThirdCountryHaveAuthNum = lua_tointeger( pState, currentStack++ );
		const int iThirdCountryAuthCount = lua_tointeger( pState, currentStack++ );

		lua_getglobal( pState, "instanceMapID" );
		const InstanceMapID instanceMapID =
			static_cast< DWORD >( lua_tonumber( pState, -1 ) );
		lua_pop( pState, 1 );

		GLLandManager* const pLandMan =
			gpGaeaServer->GetLandManager( instanceMapID );
		if ( NULL == pLandMan )
			return 0;

		db::LogInstantCTI* pDbAction =
			new db::LogInstantCTI(
			db::LogInstantCTI::EM_RESULT,
			pLandMan->getGUID() );

		pDbAction->strCTIName = strCTIName;

		pDbAction->dwCountryID = dwCountryID;
		pDbAction->nCountryPoint = nPoint;
		pDbAction->nCountryHaveAuthNum = iCountryHaveAuthNum;
		pDbAction->nCountryAuthCount = iCountryAuthCount;

		pDbAction->dwSecondCountryID = dwSecondCountryID;
		pDbAction->nSecondCountryPoint = nSecondPoint;
		pDbAction->nSecondCountryHaveAuthNum = iSecondCountryHaveAuthNum;
		pDbAction->nSecondCountryAuthCount = iSecondCountryAuthCount;

		pDbAction->dwThirdCountryID = dwThirdCountryID;
		pDbAction->nThirdCountryPoint = nThirdPoint;
		pDbAction->nThirdCountryHaveAuthNum = iThirdCountryHaveAuthNum;
		pDbAction->nThirdCountryAuthCount = iThirdCountryAuthCount;

		gpGaeaServer->AddLogAdoJob( db::DbActionPtr( pDbAction ) );

		return 1;
	}

}