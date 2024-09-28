#include "pch.h"

#include "../../FieldServer/GLGaeaServer.h"
#include "../../Club/GLClubManField.h"
#include "../../Club/GLClubField.h"
#include "../../Party/GLPartyField.h"
#include "../../Party/GLPartyManagerField.h"
#include "../../Util/GLItemLimit.h"
#include "../../../RanLogic/GLogicDataMan.h"
#include "../../../RanLogic/GLUseFeatures.h"
#include "../../../enginelib/GUInterface/GameTextControl.h"
#include "../../../enginelib/GUInterface/UITextControl.h"
#include "../InstanceSystem.h"
#include "InstanceSystemField.h"
#include "InvokeFunctionField.h"

// matching system
#include "../../MatchSystem/InvokeFunctionFieldMatching.h"

// Provide Reward System
#include "../../ProvideRewardSystem/GLProvideRewardManager.h"

// added Contents
#include "../Contents/ClubDeathMatch/RnClubDeathMatch.h"
#include "../Contents/CaptureTheFlag/CaptureTheFlag.h"
#include "../Contents/GuidanceClubBattle/GuidanceClubBattle.h"
#include "../Contents/CountryBattle/CountryBattle.h"

#include "../InvokeUI/InvokeFunctionUI.h"
#include "../InvokeDBLog/InvokeFunctionDBLog.h"

namespace InstanceSystem
{
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Field Public Function;
    void InitializeFieldPublicInvokeFunction(void)
    {
        InstanceSystem::ScriptManager* const _pScriptManager(InstanceSystem::ScriptManager::getInstance());

		{ // common function;
			registFunctionCommon(_pScriptManager);
		}

		{ // field function;
			registFunctionField(_pScriptManager);
		}

		{ // matching function;
			registFunction_FieldMatching(_pScriptManager);
		}

        // 초기화 함수 호출;
        _pScriptManager->pushCallFunction("luaFieldEventInitialize");
    }
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Field Instance Fuction;
    void InitializeFieldInstanceInvokeFunction(sc::LuaInstance& _luaInstance)
    {
		{ // common function;
			registFunctionCommon(_luaInstance);
		}

        { // field function;
            registFunctionField(_luaInstance);
            registFunctionFieldInstance(_luaInstance);
        }

		{ // matching function;
			registFunction_FieldMatching(_luaInstance);
		}
	}

    void registFunctionField(InstanceSystem::ScriptManager* const _pScriptManager)
    {
		if ( NULL == _pScriptManager )
			return;

		_pScriptManager->registFunctionCommon( registFunctionField );
    }

    void registFunctionField(sc::LuaInstance& _luaInstance)
    {
        // regist fucnction for added contents;
        {
            registFunction_CDM(_luaInstance);
            registFunction_CaptureTheFlag(_luaInstance);
            registFunction_Guidance(_luaInstance);
			registFunction_CTI_Field(_luaInstance);

			RegistInvokeFunction_UI_Field( _luaInstance );
			RegistInvokeFunction_DBLog_Field( _luaInstance );
        }

        { // getXXX;
            _luaInstance.RegistFunction("GetLevel", InstanceSystem::FieldScriptFunction::GetLevel);
            _luaInstance.RegistFunction("GetName", InstanceSystem::FieldScriptFunction::GetName);
            _luaInstance.RegistFunction("GetSchool", InstanceSystem::FieldScriptFunction::GetSchool);
			_luaInstance.RegistFunction("GetClass", InstanceSystem::FieldScriptFunction::GetClass);
            _luaInstance.RegistFunction("GetClubID", InstanceSystem::FieldScriptFunction::GetClubID);
            _luaInstance.RegistFunction("GetClubName", InstanceSystem::FieldScriptFunction::GetClubName);
            _luaInstance.RegistFunction("GetClubMasterName", InstanceSystem::FieldScriptFunction::GetClubMasterName);
            _luaInstance.RegistFunction("GetClubRank", InstanceSystem::FieldScriptFunction::GetClubRank);
			_luaInstance.RegistFunction("IsAllianceClub", InstanceSystem::FieldScriptFunction::IsAllianceClub);
            _luaInstance.RegistFunction("GetParty", InstanceSystem::FieldScriptFunction::GetParty);
            _luaInstance.RegistFunction("GetPartyID", InstanceSystem::FieldScriptFunction::GetPartyID);
            _luaInstance.RegistFunction("GetPartyMember", InstanceSystem::FieldScriptFunction::GetPartyMember);
            _luaInstance.RegistFunction("GetPartyMemberNum", InstanceSystem::FieldScriptFunction::GetPartyMemberNum);
            _luaInstance.RegistFunction("GetPartyMaster", InstanceSystem::FieldScriptFunction::GetPartyMaster);
            _luaInstance.RegistFunction("GetFieldSvrID",InstanceSystem::FieldScriptFunction::GetFieldSvrID);
            _luaInstance.RegistFunction("GetServerChannel",InstanceSystem::FieldScriptFunction::GetServerChannel);
            _luaInstance.RegistFunction("GetPosFromMapPos", InstanceSystem::FieldScriptFunction::GetPosFromMapPos);
            _luaInstance.RegistFunction("GetMapPosFromPos", InstanceSystem::FieldScriptFunction::GetMapPosFromPos);
			_luaInstance.RegistFunction("GetCountry", InstanceSystem::FieldScriptFunction::GetCountry);
			_luaInstance.RegistFunction("GetCountryName", InstanceSystem::FieldScriptFunction::GetCountryName);
        }
		{ // Set;
			_luaInstance.RegistFunction("ActiveFactionChatting", InstanceSystem::FieldScriptFunction::ActiveFactionChatting);
			_luaInstance.RegistFunction("InactiveFactionChatting", InstanceSystem::FieldScriptFunction::InactiveFactionChatting);
			_luaInstance.RegistFunction("SetCountryMark", InstanceSystem::FieldScriptFunction::SetCountryMark);
		}
		{ // Exp;
			_luaInstance.RegistFunction("AddExp", InstanceSystem::FieldScriptFunction::AddExp);
		}
        { // sendXXX;
            _luaInstance.RegistFunction("SendSystemMail", InstanceSystem::FieldScriptFunction::SendSystemMail);
			_luaInstance.RegistFunction("SendMessage", InstanceSystem::FieldScriptFunction::SendMessage);
        }
        { // DoXXX;
            _luaInstance.RegistFunction("DoEffectPosition", InstanceSystem::FieldScriptFunction::DoEffectPosition);
        }
        { // party
            _luaInstance.RegistFunction("JoinParty", InstanceSystem::FieldScriptFunction::JoinParty);
            _luaInstance.RegistFunction("OutParty", InstanceSystem::FieldScriptFunction::OutParty);
            _luaInstance.RegistFunction("ClearParty", InstanceSystem::FieldScriptFunction::ClearParty);
        }
		{ // Item;
			_luaInstance.RegistFunction("GetItemCount", InstanceSystem::FieldScriptFunction::GetItemCount);
			_luaInstance.RegistFunction("AddItem", InstanceSystem::FieldScriptFunction::AddItem);
			_luaInstance.RegistFunction("RemoveItem", InstanceSystem::FieldScriptFunction::RemoveItem);
		}
        { // UI;
            _luaInstance.RegistFunction("PrintChatMsg", InstanceSystem::FieldScriptFunction::PrintChatMsg);
            _luaInstance.RegistFunction("PrintChatXMLMsg", InstanceSystem::FieldScriptFunction::PrintChatXMLMsg);

            _luaInstance.RegistFunction("PrintSimpleMsg", InstanceSystem::FieldScriptFunction::PrintSimpleMsg);
            _luaInstance.RegistFunction("PrintSimpleXMLMsg", InstanceSystem::FieldScriptFunction::PrintSimpleXMLMsg);

            _luaInstance.RegistFunction("PrintTextDlgMsg", InstanceSystem::FieldScriptFunction::PrintTextDlgMsg);
            _luaInstance.RegistFunction("PrintTextDlgXMLMsg", InstanceSystem::FieldScriptFunction::PrintTextDlgXMLMsg);
        }
    }

    namespace FieldScriptFunction
    {
        int GetName(lua_State* pState)
        {
            int currentStack(1);

            ServerActor* const pActor = _getActor(pState, currentStack);
            if ( pActor == NULL )
            {
                return 0;
            }

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Invoke - GetName ]" );
#endif

            switch( pActor->GetCrow() )
            {
            case CROW_PC:
                {
                    GLChar* const _pChar = (GLChar*)pActor;
                    lua_pushlstring(pState, _pChar->GetName(), CHAR_SZNAME);
                    return 1;
                }
                break;
            case CROW_MOB:
            case CROW_NPC:
            case CROW_MATERIAL:
                {
                    GLCrow* const _pCrow = (GLCrow*)pActor;
                    lua_pushlstring(pState, _pCrow->GETNAME(), CHAR_SZNAME);
                    return 1;
                }
                break;
            default:
                break;
            }

            return 0;
        }

        int GetLevel(lua_State* pState)
        {
            int currentStack(1);

            ServerActor* const pActor = _getActor(pState, currentStack);
            if ( pActor == NULL )
            {
                return 0;
            }

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Invoke - GetLevel ]" );
#endif

            lua_pushinteger(pState, pActor->GetLevel());
            return 1;
        }

	    int GetSchool(lua_State* pState)
        {
            int currentStack(1);

            const DBNUM _dbNum((DWORD)lua_tonumber(pState, currentStack++));

            const GLChar* const pChar(gpGaeaServer->GetCharByDbNum(_dbNum));
            if ( pChar == NULL )
            {
                return 0;
            }

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Invoke - ServerLog ]" );
#endif

            const int _nSchool = (int)pChar->GetSchool();
            lua_pushinteger(pState, _nSchool);
            return 1;
        }

		int GetClass(lua_State* pState)
		{
			int currentStack(1);

			const DBNUM _dbNum((DWORD)lua_tonumber(pState, currentStack++));

			const GLChar* const pChar(gpGaeaServer->GetCharByDbNum(_dbNum));
			if ( pChar == NULL )
			{
				return 0;
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Invoke - GetClass ]" );
#endif

			const int _nSchool = (int)pChar->GETCHARINDEX();
			lua_pushinteger(pState, _nSchool);
			return 1;
		}




        //////////////////// Club ///////////////////////////////
	    int GetClubID(lua_State* pState)
	    {
            int currentStack(1);

		    const DBNUM _dbNum((DWORD)lua_tonumber(pState, currentStack++));

		    const GLChar* const pChar(gpGaeaServer->GetCharByDbNum(_dbNum));
		    if ( pChar == NULL )
		    {
                return 0;
            }

            const DWORD _dwClubID = pChar->GetClubDbNum();
            if ( _dwClubID == CLUB_NULL )
            {
                return 0;
            }

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Invoke - GetClubID ]" );
#endif

            lua_pushinteger(pState, _dwClubID);
            return 1;
	    }

	    int GetClubName(lua_State* pState)
	    {
            int currentStack(1);

            std::tr1::shared_ptr<GLClubField> _spClub = _getClub(pState, currentStack);
            if ( _spClub == NULL )
            {
                return  0;
            }

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Invoke - GetClubName ]" );
#endif

            lua_pushlstring(pState, _spClub->Name(), CLUB_NAME_LENGTH);
            return 1;
	    }

        int GetClubMasterName(lua_State* pState)
        {
            int currentStack(1);

            std::tr1::shared_ptr<GLClubField> _spClub = _getClub(pState, currentStack);
            if ( _spClub == NULL )
            {
                return  0;
            }

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Invoke - GetClubMasterName ]" );
#endif

            lua_pushlstring(pState, _spClub->GetMasterName(), CHAR_SZNAME);
            return 1;
        }

        int GetClubRank(lua_State* pState)
        {
            int currentStack(1);

            std::tr1::shared_ptr<GLClubField> _spClub = _getClub(pState, currentStack);
            if ( _spClub == NULL )
            {
                return  0;
            }

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Invoke - GetClubRank ]" );
#endif

            lua_pushinteger(pState, _spClub->GetRank());
            return 1;
        }

		int IsAllianceClub ( lua_State* _pState )
		{
			int n_CurrentStack = 1;

			std::tr1::shared_ptr< GLClubField > spClub = _getClub( _pState, n_CurrentStack );
			std::tr1::shared_ptr< GLClubField > spTarClub = _getClub( _pState, n_CurrentStack );

			if ( NULL == spClub )
			{
				lua_pushboolean( _pState, false );

				return 1;
			}

			if ( NULL == spTarClub )
			{
				lua_pushboolean( _pState, false );

				return 1;
			}

			// 동맹클럽인지 확인한다;
			if ( false == spClub->IsAlliance() )
			{
				lua_pushboolean( _pState, false );

				return 1;
			}

			if ( spClub->IsAllianceGuild_COMMON( spTarClub->m_DbNum, spTarClub->m_dwAlliance ) )
			{
				lua_pushboolean( _pState, true );

				return 1;
			}

			lua_pushboolean( _pState, false );

			return 1;
		}

        ///////////////////////// Party ///////////////////////////////
        int GetParty(lua_State* pState)
        {
            int currentStack(1);
            const unsigned int _charDbNum((DWORD)lua_tonumber(pState, currentStack++));

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Invoke - GetParty ]" );
#endif

            const GLChar* pChar(gpGaeaServer->GetCharByDbNum(_charDbNum));
            if ( pChar )
            {
                GLPartyID _partyID = pChar->GetPartyID();
                const GLPartyField* pParty = gpGaeaServer->GetParty(_partyID);
    			
			    if ( pParty )
			    {
                    if ( pParty->m_cMEMBER.size() == 0 )
                        return 0;

                    lua_newtable(pState);
                    int top = lua_gettop(pState);

				    GLPartyField::MEMBER_CITER _memberIter = pParty->m_cMEMBER.begin();
				    GLPartyField::MEMBER_CITER _endIter = pParty->m_cMEMBER.end();

				    for ( int i = 1; _memberIter != _endIter; _memberIter++, i++) 
				    {
                        const GLPARTY_FNET& _partyMember = _memberIter->second;
                        lua_pushinteger(pState, _partyMember.GetChaDbNum());
                        lua_rawseti(pState, top, i);
				    }
				    return 1;
			    }
            }
            return 0;
        }

        int GetPartyID(lua_State* pState)
        {
            int currentStack(1);
            const unsigned int _charDbNum((DWORD)lua_tonumber(pState, currentStack++));

            GLChar* pChar = gpGaeaServer->GetCharByDbNum(_charDbNum);
            if ( pChar )
            {
                const GLPartyID _sPartyID = pChar->GetPartyID();
                if ( _sPartyID.isValidParty() )
                {
#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
					sc::writeLogInfo( "[ Instance Log ] [ Invoke - GetPartyID ]" );
#endif

                    lua_pushinteger(pState, _sPartyID.GetID());            
                    return 1;
                }
            }

            return 0;
        }

        int GetPartyMemberNum(lua_State* pState)
        {
            int currentStack(1);
            GLPartyField* pParty = _getParty(pState, currentStack);
            if ( pParty == NULL )
            {
                return 0;
            }

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Invoke - GetPartyMemberNum ]" );
#endif

            const int nPartyMember = pParty->GetNMember();
            lua_pushinteger(pState, nPartyMember);
            return 1;
        }

        int GetPartyMaster(lua_State* pState)
        {
            int currentStack(1);
            GLPartyField* pParty = _getParty(pState, currentStack);
            if ( pParty == NULL )
            {
                return 0;
            }

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Invoke - GetPartyMaster ]" );
#endif

            const int nPartyMember = pParty->m_cMEMBER[0].second.m_dwDbNum;
            lua_pushinteger(pState, nPartyMember);
            return 1;
        }

        int GetPartyMember(lua_State* pState)
        {
            int currentStack(1);
            GLPartyField* pParty = _getParty(pState, currentStack);
            if ( pParty == NULL )
            {
                return 0;
            }

            const DWORD _dwMemberNum((DWORD)lua_tonumber(pState, currentStack++));
            lua_pop(pState, 1);

            if ( pParty->GetNMember() <= _dwMemberNum )
            {
                return 0;
            }

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Invoke - ServerLog ]" );
#endif

            lua_pushinteger(pState, pParty->m_cMEMBER[_dwMemberNum].second.m_dwDbNum);
            return 1;
        }

        int GetFieldSvrID(lua_State* pState)
        {
#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Invoke - GetFieldSvrID ]" );
#endif

            lua_pushinteger(pState, gpGaeaServer->GetFieldSvrID());
            return 1;
        }

        int GetServerChannel(lua_State* pState)
        {
            const DWORD _nDbNum((DWORD)lua_tonumber(pState, -1));
            GLChar* const _pChar = gpGaeaServer->GetCharByDbNum(_nDbNum);
            if ( _pChar == NULL )
                return 0;

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Invoke - GetServerChannel ]" );
#endif

            lua_pushinteger(pState, _pChar->GetServerChannel());
            return 1;
        }

		int GetPosFromMapPos(lua_State* pState)
		{
			int currentStack(1);

			const int _mapMid(lua_tointeger(pState, currentStack++ ));
			const int _mapSid(lua_tointeger(pState, currentStack++ ));
			const int _mapXPos(lua_tointeger(pState, currentStack++ ));
			const int _mapZPos(lua_tointeger(pState, currentStack++ ));

			D3DXVECTOR3 _worldPos;
			gpGaeaServer->ConvertMapToWorld(MapID(SNATIVEID(_mapMid, _mapSid)), _mapXPos, _mapZPos, _worldPos);

			lua_pushnumber(pState, _worldPos.x);
			lua_pushnumber(pState, _worldPos.z);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Invoke - GetPosFromMapPos ]" );
#endif

			return 2;
		}

		int GetMapPosFromPos(lua_State* pState)
		{
			int currentStack(1);

			const int _mapMid(lua_tointeger(pState, currentStack++ ));
			const int _mapSid(lua_tointeger(pState, currentStack++ ));
			const float _worldXPos((float)lua_tonumber(pState, currentStack++ ));
			const float _worldZPos((float)lua_tonumber(pState, currentStack++ ));

			int _mapXPos;
			int _mapZPos;
			gpGaeaServer->ConvertWorldToMap(MapID(SNATIVEID(_mapMid, _mapSid)), D3DXVECTOR3(_worldXPos,0.0f,_worldZPos), _mapXPos, _mapZPos);

			lua_pushinteger(pState, _mapXPos);
			lua_pushinteger(pState, _mapZPos);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Invoke - GetMapPosFromPos ]" );
#endif

			return 2;
		}

		int GetCountry(lua_State* pState)
		{
			int currentStack = 1;

			const DBNUM dwDbNum = 
				static_cast< DBNUM >( lua_tonumber( pState, currentStack++ ) );

			GLChar* pChar = gpGaeaServer->GetCharByDbNum( dwDbNum );
			if ( NULL == pChar )
				return 0;

			const Country::SCOUNTRY_INFO& sCountryInfo =
				pChar->GetCountryInfo();

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Invoke - GetCountry ]" );
#endif

			lua_pushinteger( pState, sCountryInfo.dwID );

			return 1;
		}

		int GetCountryName(lua_State* pState)
		{
			int currentStack = 1;

			const DWORD dwCountryID = 
				static_cast< DBNUM >( lua_tonumber( pState, currentStack++ ) );

			Country::GLCountryManServer* pCountryMan =
				Country::GLCountryManServer::Instance();
			if ( NULL == pCountryMan )
				return 0;

			Country::GLCountry* pCountry =
				pCountryMan->GetCountryUsingID( dwCountryID );
			if ( NULL == pCountry )
				return 0;

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Invoke - GetCountryName ]" );
#endif

			lua_pushlstring( pState, pCountry->GetCountryName().c_str(), CHAR_SZNAME );

			return 1;
		}


		// Set;
		int ActiveFactionChatting(lua_State* pState)
		{
			lua_getglobal( pState, "instanceMapID" );
			const InstanceMapID instanceMapID(
				static_cast< DWORD >( lua_tonumber( pState, -1 ) ) );
			lua_pop( pState, 1 );

			GLLandManager* const pLandManager = 
				gpGaeaServer->GetLandManager( instanceMapID );
			if ( NULL == pLandManager )
				return 0;

			pLandManager->ActiveFactionChatting();

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Invoke - ActiveFactionChatting ]" );
#endif

			return 0;
		}

		int InactiveFactionChatting(lua_State* pState)
		{
			lua_getglobal( pState, "instanceMapID" );
			const InstanceMapID instanceMapID(
				static_cast< DWORD >( lua_tonumber( pState, -1 ) ) );
			lua_pop( pState, 1 );

			GLLandManager* const pLandManager = 
				gpGaeaServer->GetLandManager( instanceMapID );
			if ( NULL == pLandManager )
				return 0;

			pLandManager->InactiveFactionChatting();

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Invoke - InactiveFactionChatting ]" );
#endif

			return 0;
		}

		int SetCountryMark(lua_State* pState)
		{
			int currentStack = 1;

			if ( NULL == gpGaeaServer )
				return 0;

			const DBNUM dwDbNum = 
				static_cast< DBNUM >( lua_tonumber( pState, currentStack++ ) );

			GLChar* pChar = gpGaeaServer->GetCharByDbNum( dwDbNum );
			if ( NULL == pChar )
				return 0;

			LuaPlus::LuaObject bShowCountryMarkObj(
				LuaPlus::LuaState::CastState( pState ),
				currentStack++ );

			bool bShowCountryMark = false;
			if ( bShowCountryMarkObj.IsBoolean() )
				bShowCountryMark = bShowCountryMarkObj.GetBoolean();

			// 캐릭터에게 Country Mark 정보를 보낸다;
			GLMSG::NET_SHOW_COUNTRY_MARK_FC sMsg;
			sMsg.m_bShowCountryMark = bShowCountryMark;

			gpGaeaServer->SENDTOCLIENT( pChar->ClientSlot(), &sMsg );

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Invoke - SetCountryMark ]" );
#endif

			return 0;
		}


		// Exp;
		int AddExp(lua_State* pState)
		{
			int currentStack(1);

			const DWORD _dbNum((DWORD)lua_tonumber(pState, currentStack++));
			GLChar* const pChar(gpGaeaServer->GetCharByDbNum(_dbNum));
			if ( pChar == NULL )
				return 0;

			LuaPlus::LuaObject _genExpObj(LuaPlus::LuaState::CastState(pState), currentStack++);
			if ( _genExpObj.IsNumber() == false )
				return 0;

			const int _genExp = _genExpObj.GetInteger();

			bool _expNotify(false);
			LuaPlus::LuaObject _expNotifyObj(LuaPlus::LuaState::CastState(pState), currentStack++);
			if ( _expNotifyObj.IsBoolean() )
			{
				_expNotify = _expNotifyObj.GetBoolean();
			}

			// 유의! 이벤트와 경험치 증감효과에 영향을 받는다.;
			pChar->ReceiveExpApply(_genExp, _expNotify);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Invoke - AddExp ]" );
#endif

			return 0;
		}

        // SendXXX;
        int SendSystemMail(lua_State* pState)
        {
            int currentStack(1);

            const DWORD _dbNum((DWORD)lua_tonumber(pState, currentStack++));
            const GLChar* const pChar(gpGaeaServer->GetCharByDbNum(_dbNum));
            if ( pChar == NULL )
                return 0;

            std::string _strPostTitle;
            if ( _luaTableToString(pState, currentStack, _strPostTitle) == false )
                return 0;

            std::string _strPostContent;
            if ( _luaTableToString(pState, currentStack, _strPostContent) == false )
                return 0;
            
            const DWORD _dwAttachedMoney((DWORD)lua_tonumber(pState, currentStack++)); 

            DWORD _dwItemMainID(0);
            DWORD _dwItemSubID(0);
            DWORD _dwItemNum(0);
			bool bResult(false);
            LuaPlus::LuaObject _AttachedItem(LuaPlus::LuaState::CastState(pState), currentStack++);

			DWORD dwContentTextIndex = 0;
			dwContentTextIndex =
				static_cast< DWORD >( lua_tonumber( pState, currentStack++ ) );

			ProvideRewardSystem::GLProvideRewardManager* pProvideRewardMan =
				ProvideRewardSystem::GLProvideRewardManager::Instance();
			if ( NULL == pProvideRewardMan )
				return 0;

			if (_AttachedItem.IsNil() == false)
			{
				if ( _AttachedItem.IsTable() )
				{
					LuaPlus::LuaObject _Obj;
					LuaPlus::LuaTableIterator _ItemIterator(_AttachedItem);
					{
						_Obj = _ItemIterator.GetValue();
                        if ( _Obj.IsNumber() )
						    _dwItemMainID = _Obj.GetInteger();
					}
					if ( _ItemIterator.Next() )
					{
						_Obj = _ItemIterator.GetValue();
                        if ( _Obj.IsNumber() )
						    _dwItemSubID = _Obj.GetInteger();
					}
					if ( _ItemIterator.Next() )
					{
						_Obj = _ItemIterator.GetValue();
                        if ( _Obj.IsNumber() )
						    _dwItemNum = _Obj.GetInteger();
					}
				}		

				ProvideRewardSystem::PROVIDE_REWARD_DATA provideRewardData;
				if ( GLUseFeatures::GetInstance().IsUsingWorldBattle() )
				{
					provideRewardData.SetCharacter( pChar->GetCharID() );
					provideRewardData.SetProvideType( ProvideRewardSystem::PROVIDE_REWARD_DATA::EMPROVIDE_TYPE_LOGIN );
					provideRewardData.SetRewardType( ProvideRewardSystem::PROVIDE_REWARD_DATA::EMREWARD_TYPE_MONEY_AND_ITEM );
					provideRewardData.SetRewardValue( _dwAttachedMoney, _dwItemMainID, _dwItemSubID, _dwItemNum );
					provideRewardData.SetTextIndex( dwContentTextIndex );
				}
				else
				{
					/*provideRewardData.SetCharacter( pChar->GetCharID() );
					provideRewardData.SetProvideType( ProvideRewardSystem::PROVIDE_REWARD_DATA::EMPROVIDE_TYPE_IMMEDIATELY );
					provideRewardData.SetRewardType( ProvideRewardSystem::PROVIDE_REWARD_DATA::EMREWARD_TYPE_MONEY_AND_ITEM );
					provideRewardData.SetRewardValue( _dwAttachedMoney, _dwItemMainID, _dwItemSubID, _dwItemNum );
					provideRewardData.SetTextIndex( dwContentTextIndex );*/
					
					const SNATIVEID receiveItemID(_dwItemMainID, _dwItemSubID);        
					SITEMCUSTOM newItem;
					gpGaeaServer->NewItemMake(newItem, receiveItemID, EMGEN_SYSTEM);
					if ( newItem.GetNativeID() != SNATIVEID(false) )
					{
						const SITEM* pITEM = GLogicData::GetInstance().GetItem( newItem.GetNativeID() );
						if ( pITEM == NULL )
							return 0;

						const int maxPileNum = pITEM->sDrugOp.GetPileNum();
						newItem.TurnNumSet( min(maxPileNum, (int)(_dwItemNum ? _dwItemNum : 1)) );

						newItem.GenerateBasicStat( false );
						newItem.GenerateLinkSkill();
						newItem.GenerateAddOption();

						gpGaeaServer->GetItemLimit()->LogItemBasicStatOption( newItem );
						gpGaeaServer->GetItemLimit()->LogItemLinkSkillOption( newItem );
						gpGaeaServer->GetItemLimit()->LogItemAddonOption( newItem );

						if ( newItem.GENERATE_RANDOM_OPT( false ) )
						{
							GLItemLimit* pItemLimit = gpGaeaServer->GetItemLimit();
							pItemLimit->LogItemRandomOption(newItem);
						}
					}

					// 우편을 보냄;
					bResult = gpGaeaServer->PostSendSystemMail(
						pChar->GetCharID(),
						std::string(pChar->m_szName),
						newItem,
						(LONGLONG)_dwAttachedMoney,
						ATTENDANCE_REWARD_POST_DESIGN,
						ATTENDANCE_REWARD_POST_SENDER,
						std::string( ID2SERVERTEXT("SYSTEM_POST_SENDER_NAME") ),
						std::string( _strPostTitle ),
						std::string( _strPostContent )
						);
				}

				pProvideRewardMan->ProvideReward( provideRewardData );
			}
			else
			{
				ProvideRewardSystem::PROVIDE_REWARD_DATA provideRewardData;
				if ( GLUseFeatures::GetInstance().IsUsingWorldBattle() )
				{
					provideRewardData.SetCharacter( pChar->GetCharID() );
					provideRewardData.SetProvideType( ProvideRewardSystem::PROVIDE_REWARD_DATA::EMPROVIDE_TYPE_LOGIN );
					provideRewardData.SetRewardType( ProvideRewardSystem::PROVIDE_REWARD_DATA::EMREWARD_TYPE_MONEY );
					provideRewardData.SetRewardValue( _dwAttachedMoney );
					provideRewardData.SetTextIndex( 0 );
				}
				else
				{
					provideRewardData.SetCharacter( pChar->GetCharID() );
					provideRewardData.SetProvideType( ProvideRewardSystem::PROVIDE_REWARD_DATA::EMPROVIDE_TYPE_IMMEDIATELY );
					provideRewardData.SetRewardType( ProvideRewardSystem::PROVIDE_REWARD_DATA::EMREWARD_TYPE_MONEY );
					provideRewardData.SetRewardValue( _dwAttachedMoney );
					provideRewardData.SetTextIndex( 0 );

					/*bResult = gpGaeaServer->PostSendSystemMail(
						pChar->GetCharID(),
						std::string(pChar->m_szName),
						SNATIVEID( false ).dwID,
						(LONGLONG)_dwAttachedMoney,
						ATTENDANCE_REWARD_POST_DESIGN,
						ATTENDANCE_REWARD_POST_SENDER,
						std::string( ID2SERVERTEXT("SYSTEM_POST_SENDER_NAME") ),
						std::string( _strPostTitle ),
						std::string( _strPostContent )
						);*/
				}
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Invoke - SendSystemMail ]" );
#endif     

            if ( bResult == false )
                return 0; // 우편 보내기 실패;

            return 0;
        }
        


        /////////////////////////  Do XXX  /////////////////////////////
        int DoEffectPosition(lua_State* pState)
        {
            int currentStack(1);

            const DWORD _nDbNum((DWORD)lua_tonumber(pState, currentStack++));
            GLChar* const _pChar = gpGaeaServer->GetCharByDbNum(_nDbNum);
            if ( _pChar == NULL )
                return 0;

            LuaPlus::LuaObject _effectNameObj(LuaPlus::LuaState::CastState(pState), currentStack++);
            if ( _effectNameObj.IsString() == false )
                return 0;

            const char* _effectName = _effectNameObj.GetString();
            const WORD _wMapMID((WORD)lua_tonumber(pState, currentStack++));
            const WORD _wMapSID((WORD)lua_tonumber(pState, currentStack++));
            const float _fXPos((float)lua_tonumber(pState, currentStack++));
            const float _fYPos((float)lua_tonumber(pState, currentStack++));
            const float _fZPos((float)lua_tonumber(pState, currentStack++));

            // 현재 해당 맵에 있지 않으면 이펙트 뿌리지 않음;
            if ( _pChar->GetCurrentMap().getBaseMapID() != SNATIVEID(_wMapMID, _wMapSID) )
                return 0;

            GLMSG::NET_INSTANCE_EFFECT NetMsg;
            NetMsg._strEffectName = _effectName;
            NetMsg._fEffectZonePosX = _fXPos;
            NetMsg._fEffectZonePosY = _fYPos;
            NetMsg._fEffectZonePosZ = _fZPos;

            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, NetMsg);
            gpGaeaServer->SENDTOCLIENT(_pChar->GETCLIENTID(), NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK, SendBuffer);
            //DxEffGroupPlayer::GetInstance().NewEffGroup ( pSkill->m_sEXT_DATA.GETTARGZONE01(emELMT), matTrans, &TARGETID_NULL, TRUE, FALSE  );

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Invoke - DoEffectPosition ]" );
#endif

            return 0;
        }


        //////////////////////// Party ////////////////////////////
        int JoinParty(lua_State* pState)  // 파티 생성 or 파티원 추가;
        {
            int currentStack(1);

            const DWORD _nMasterDbNum((DWORD)lua_tonumber(pState, currentStack++));
            GLChar* const _pMasterChar = gpGaeaServer->GetCharByDbNum(_nMasterDbNum);
            if ( _pMasterChar == NULL )
                return 0;

            const DWORD _nMemberDbNum((DWORD)lua_tonumber(pState, currentStack++));
            GLChar* const _pMemberChar = gpGaeaServer->GetCharByDbNum(_nMemberDbNum);
            if ( _pMemberChar == NULL )
                return 0;

            GLMSG::NET_INSTANCE_JOIN_PARTY_FA netMsg(_pMasterChar->GetGaeaID(), _pMemberChar->GetGaeaID(), SPartyOption());
            gpGaeaServer->SENDTOAGENT(&netMsg);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Invoke - JoinParty ]" );
#endif

            return 0;
        }

        int OutParty(lua_State* pState)  // 파티 탈퇴;
        {
            int currentStack(1);

            const DWORD _nCharDbNum((DWORD)lua_tonumber(pState, currentStack++));
            GLChar* const _pChar = gpGaeaServer->GetCharByDbNum(_nCharDbNum);
            if ( _pChar == NULL )
                return 0;

            GLMSG::NET_INSTANCE_OUT_PARTY_FA netMsg(_pChar->GetGaeaID());
            gpGaeaServer->SENDTOAGENT(&netMsg);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Invoke - OutParty ]" );
#endif

            return 0;
        }

        int ClearParty(lua_State* pState)  // 파티 해산;
        {
            int currentStack(1);

            const DWORD _partyID((DWORD)lua_tonumber(pState, currentStack++));

            GLMSG::NET_INSTANCE_CLEAR_PARTY_FA netMsg(_partyID);
            gpGaeaServer->SENDTOAGENT(&netMsg);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Invoke - ClearParty ]" );
#endif

            return 0;
        }

		/////////////////////////  Item  /////////////////////////////
		int GetItemCount(lua_State* pState)
		{
			int currentStack(1);

			const DWORD _charDbNum((DWORD)lua_tonumber(pState, currentStack++ ));
			const int _itemMID(lua_tointeger(pState, currentStack++ )); 
			const int _itemSID(lua_tointeger(pState, currentStack++ )); 
			const SNATIVEID _itemID((WORD)_itemMID,(WORD)_itemSID);

			GLChar* _pChar = gpGaeaServer->GetCharByDbNum(_charDbNum);
			if ( _pChar == NULL )
				return 0;

			const SITEM* pItem = GLogicData::GetInstance().GetItem( _itemID );
			if( !pItem )
				return 0;

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Invoke - GetItemCount ]" );
#endif

			const DWORD nItemCount = _pChar->m_cInventory.CountTurnItem( _itemID ); //가진 재료의 수량을 얻어온다
			lua_pushinteger(pState, nItemCount);
			return 1;
		}

		// 인벤토리에 아이템을 넣어주는 기능을 한다.
		// 빈공간이 없으면 발밑에 아이템을 드랍한다. 이 함수가 호출되는 시점이 맵에서 나갈 때 호출된다면 바닥에 떨어뜨려도 소용없을것
		int AddItem(lua_State* pState)
		{
			int currentStack(1);

			const DWORD _charDbNum((DWORD)lua_tonumber(pState, currentStack++ ));
			const int _itemMID(lua_tointeger(pState, currentStack++ )); 
			const int _itemSID(lua_tointeger(pState, currentStack++ )); 
			const int _numAdd(lua_tointeger(pState, currentStack++ ));

			const int _nGradeAttack(lua_tointeger(pState, currentStack++ )); 
			const int _nGradeDefense(lua_tointeger(pState, currentStack++ )); 
			const bool _bRandomOption(lua_toboolean(pState, currentStack++ )); 


			const SNATIVEID _itemID((WORD)_itemMID,(WORD)_itemSID);

			GLChar* _pChar = gpGaeaServer->GetCharByDbNum(_charDbNum);
			if ( _pChar == NULL )
				return 0;

			const SITEM* pItem = GLogicData::GetInstance().GetItem( _itemID );
			if( !pItem )
				return 0;

			if( pItem->ISPILE() )
			{
				int _itemCount(_numAdd);
				SITEMCUSTOM sITEM_NEW( _itemID );
				sITEM_NEW.tBORNTIME = CTime::GetCurrentTime().GetTime();
				sITEM_NEW.wTurnNum = _itemCount;
				sITEM_NEW.cGenType = EMGEN_ITEMMIX;
				//sITEM_NEW.cChnID = (BYTE)_pChar->GetServerChannel();
				//sITEM_NEW.cFieldID = (BYTE)_pChar->GetFieldSvrID();
				sc::SeqUniqueGUID::Instance()->getGUID( sITEM_NEW.guid );
				sITEM_NEW.GenerateBasicStat( false );
				sITEM_NEW.GenerateLinkSkill();
				sITEM_NEW.GenerateAddOption();

				_pChar->LogItemBasicStatOption( sITEM_NEW );
				_pChar->LogItemLinkSkillOption( sITEM_NEW );
				_pChar->LogItemAddonOption( sITEM_NEW );

				sITEM_NEW.cDAMAGE = _nGradeAttack;
				sITEM_NEW.cDEFENSE = _nGradeDefense;

				if( _bRandomOption )
				{
					if( sITEM_NEW.GENERATE_RANDOM_OPT( false ) )
						_pChar->LogItemRandomOption( sITEM_NEW );
				}

				CItemDrop cDropItem;
				cDropItem.sItemCustom = sITEM_NEW;
				if ( _pChar->IsInsertToInven( &cDropItem ) )
				{
					_pChar->InsertToInven( &cDropItem, true, false, false, false, true );
				}
				else
				{
					// 우편으로 아이템을 보낸다;
					gpGaeaServer->PostSendSystemMail(
						_pChar->CharDbNum(),
						std::string( _pChar->GetName() ),
						sITEM_NEW,
						0,
						ATTENDANCE_REWARD_POST_DESIGN,
						ATTENDANCE_REWARD_POST_SENDER,
						std::string( ID2SERVERTEXT( "SYSTEM_POST_SENDER_NAME", 0 ) ),
						std::string( ID2SERVERTEXT( "TEXT_FULL_INVENTORY_POST_TITLE", 0 ) ),
						std::string( ID2SERVERTEXT( "TEXT_FULL_INVENTORY_POST_CONTENTS", 0 ) ) );

					// 시스템 메시지를 출력한다;
					GLMSG::NET_INSTANCE_UI_CHATXMLMSG sNetMsg;
					sNetMsg.xmlString.emXmlType = InstanceSystem::InstanceXmlString::EMXMLGAME_IN_TEXT;
					sNetMsg.xmlString.strXmlKey = "TEXT_FULL_INVENTORY_POST_PRINT_MESSAGE";
					sNetMsg.xmlString.index = 0;
					sNetMsg.dwColor = NS_UITEXTCOLOR::RED;

					msgpack::sbuffer SendBuffer;
					msgpack::pack( SendBuffer, sNetMsg );
					gpGaeaServer->SENDTOCLIENT(
						_pChar->ClientSlot(),
						NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK,
						SendBuffer );

					sc::writeLogInfo( sc::string::format(
						"[ Check ] [ Post a Item!! ChaNum : %1% ]",
						_pChar->CharDbNum() ) );

					//_pChar->DropItem( cDropItem.sItemCustom, _pChar->GetPosition() );
				}
			}
			else
			{
				for( int i = 0; i < _numAdd; i++ )
				{
					SITEMCUSTOM sITEM_NEW( _itemID );
					sITEM_NEW.tBORNTIME = CTime::GetCurrentTime().GetTime();
					sITEM_NEW.cGenType = EMGEN_ITEMMIX;
					//sITEM_NEW.cChnID = (BYTE)_pChar->GetServerChannel();
					//sITEM_NEW.cFieldID = (BYTE)_pChar->GetFieldSvrID();
					sc::SeqUniqueGUID::Instance()->getGUID( sITEM_NEW.guid/*sITEM_NEW.lnGenNum, sITEM_NEW.GetNativeID(), (EMITEMGEN)sITEM_NEW.cGenType*/ );
					sITEM_NEW.GenerateBasicStat( false );
					sITEM_NEW.GenerateLinkSkill();
					sITEM_NEW.GenerateAddOption();

					_pChar->LogItemBasicStatOption( sITEM_NEW );
					_pChar->LogItemLinkSkillOption( sITEM_NEW );
					_pChar->LogItemAddonOption( sITEM_NEW );

					sITEM_NEW.cDAMAGE = _nGradeAttack;
					sITEM_NEW.cDEFENSE = _nGradeDefense;

					if( _bRandomOption )
					{
						if( sITEM_NEW.GENERATE_RANDOM_OPT( false ) )
							_pChar->LogItemRandomOption( sITEM_NEW );
					}

					CItemDrop cDropItem;
					cDropItem.sItemCustom = sITEM_NEW;
					if ( _pChar->IsInsertToInven( &cDropItem ) )
					{
						_pChar->InsertToInven( &cDropItem, true, false, false, false, true );
					}
					else
					{
						// 우편으로 아이템을 보낸다;
						gpGaeaServer->PostSendSystemMail(
							_pChar->CharDbNum(),
							std::string( _pChar->GetName() ),
							sITEM_NEW,
							0,
							ATTENDANCE_REWARD_POST_DESIGN,
							ATTENDANCE_REWARD_POST_SENDER,
							std::string( ID2SERVERTEXT( "SYSTEM_POST_SENDER_NAME", 0 ) ),
							std::string( ID2SERVERTEXT( "TEXT_FULL_INVENTORY_POST_TITLE", 0 ) ),
							std::string( ID2SERVERTEXT( "TEXT_FULL_INVENTORY_POST_CONTENTS", 0 ) ) );

						// 시스템 메시지를 출력한다;
						GLMSG::NET_INSTANCE_UI_CHATXMLMSG sNetMsg;
						sNetMsg.xmlString.emXmlType = InstanceSystem::InstanceXmlString::EMXMLGAME_IN_TEXT;
						sNetMsg.xmlString.strXmlKey = "TEXT_FULL_INVENTORY_POST_PRINT_MESSAGE";
						sNetMsg.xmlString.index = 0;
						sNetMsg.dwColor = NS_UITEXTCOLOR::RED;

						msgpack::sbuffer SendBuffer;
						msgpack::pack( SendBuffer, sNetMsg );
						gpGaeaServer->SENDTOCLIENT(
							_pChar->ClientSlot(),
							NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK,
							SendBuffer );

						sc::writeLogInfo( sc::string::format(
							"[ Check ] [ Post a Item!! ChaNum : %1% ]",
							_pChar->CharDbNum() ) );

						//_pChar->DropItem( cDropItem.sItemCustom, _pChar->GetPosition() );
					}
				}
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Invoke - AddItem ]" );
#endif

			return 0;
		}

		int RemoveItem(lua_State* pState)
		{
			int currentStack(1);

			const DWORD _charDbNum((DWORD)lua_tonumber(pState, currentStack++ ));
			const int _itemMID(lua_tointeger(pState, currentStack++ )); 
			const int _itemSID(lua_tointeger(pState, currentStack++ )); 
			const int _numRemove(lua_tointeger(pState, currentStack++ )); 

			const SNATIVEID _itemID((WORD)_itemMID,(WORD)_itemSID);

			GLChar* _pChar = gpGaeaServer->GetCharByDbNum(_charDbNum);
			if ( _pChar == NULL )
			{
				// 통계 및 로그 수정
				// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
				sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on RemoveItem ", _charDbNum));
				return 0;
			}

			const SITEM* pItem = GLogicData::GetInstance().GetItem( _itemID );
			if( !pItem )
				return 0;

			// 겹침가능한 아이템의 경우;
			if( pItem->ISPILE() )
			{
				SINVENITEM* pInvenItem = _pChar->m_cInventory.FindItem( _itemID);
				if( !pInvenItem )
					return 0;

				if( !_pChar->DeletePileItem( _itemID, _numRemove ) )
					return 0;

				// 통계 및 로그 수정
				_pChar->LogItemExchange(
					pInvenItem->sItemCustom,
					gamelog::ID_CHAR,
					_pChar->m_CharDbNum,
					gamelog::ID_CHAR,
					0, 
					ITEM_ROUTE_SYSTEM,
					pInvenItem->sItemCustom.wTurnNum,
					(_pChar == NULL) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : _pChar->GetCurrentMap(), LOG_ITEM_DEL_ITEM_BY_LUA,
					true);
			}
			// 겹침 불가능한 아이템의 경우
			else
			{
				for (int i = 0; i < _numRemove; i++)
				{
					SINVENITEM* pInvenItem = _pChar->m_cInventory.FindItem( _itemID );
					if( !pInvenItem )
						break;

					if( !_pChar->InvenDeleteItem( pInvenItem->wPosX, pInvenItem->wPosY, true, FALSE, 1897, true ) )
						return 0;

					// 통계 및 로그 수정
					_pChar->LogItemExchange(
						pInvenItem->sItemCustom,
						gamelog::ID_CHAR,
						_pChar->m_CharDbNum,
						gamelog::ID_CHAR,
						0, 
						ITEM_ROUTE_SYSTEM,
						1,
						(_pChar == NULL) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : _pChar->GetCurrentMap(), LOG_ITEM_DEL_ITEM_BY_LUA,
						true);
				}
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Invoke - RemoveItem ]" );
#endif

			return 0;
		}

        int PrintChatMsg(lua_State* pState)
        {
            int currentStack(1);

            const DWORD _nDbNum((DWORD)lua_tonumber(pState, currentStack++));
            GLChar* const _pChar = gpGaeaServer->GetCharByDbNum(_nDbNum);
            if ( _pChar == NULL )
                return 0;

            GLMSG::NET_INSTANCE_UI_CHATMSG NetMsg;
            if ( _makeChatMsgPacket(pState, currentStack, NetMsg) == false )
                return 0;

            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, NetMsg);
            gpGaeaServer->SENDTOCLIENT(_pChar->GETCLIENTID(), NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK, SendBuffer);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Invoke - PrintChatMsg ]" );
#endif

            return 0;
        }

        int PrintChatXMLMsg(lua_State* pState)
        {
            int currentStack(1);

            const DWORD _nDbNum((DWORD)lua_tonumber(pState, currentStack++));
            GLChar* const _pChar = gpGaeaServer->GetCharByDbNum(_nDbNum);
            if ( _pChar == NULL )
                return 0;

            GLMSG::NET_INSTANCE_UI_CHATXMLMSG NetMsg;
            if ( _makeChatXmlMsgPacket(pState, currentStack, NetMsg) == false )
                return 0;

            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, NetMsg);
            gpGaeaServer->SENDTOCLIENT(_pChar->GETCLIENTID(), NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK, SendBuffer);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Invoke - PrintChatXMLMsg ]" );
#endif

            return 0;
        }

        int PrintSimpleMsg(lua_State* pState)
        {
            int currentStack(1);

            const DWORD _nDbNum((DWORD)lua_tonumber(pState, currentStack++));
            GLChar* const _pChar = gpGaeaServer->GetCharByDbNum(_nDbNum);
            if ( _pChar == NULL )
                return 0;

            GLMSG::NET_INSTANCE_UI_SIMPLE_MSG NetMsg;
            if ( _makeSimpleMsgPacket(pState, currentStack, NetMsg) == false )
                return 0;

            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, NetMsg);
            gpGaeaServer->SENDTOCLIENT(_pChar->GETCLIENTID(), NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK, SendBuffer);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Invoke - PrintSimpleMsg ]" );
#endif

            return 0;
        }

        int PrintSimpleXMLMsg(lua_State* pState)
        {
            int currentStack(1);

            const DWORD _nDbNum((DWORD)lua_tonumber(pState, currentStack++));

            GLChar* const _pChar = gpGaeaServer->GetCharByDbNum(_nDbNum);
            if ( _pChar == NULL )
                return 0;

            GLMSG::NET_INSTANCE_UI_SIMPLE_XMLMSG NetMsg;
            if ( _makeSimpleXmlMsgPacket(pState, currentStack, NetMsg) == false )
                return 0;

            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, NetMsg);
            gpGaeaServer->SENDTOCLIENT(_pChar->GETCLIENTID(), NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK, SendBuffer);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Invoke - PrintSimpleXMLMsg ]" );
#endif

            return 0;
        }

        int PrintTextDlgMsg(lua_State* pState)  // 화면 상단 텍스트 다이얼로그 박스 출력;
        {
            int currentStack(1);

            const DWORD _nDbNum((DWORD)lua_tonumber(pState, currentStack++));
            GLChar* const _pChar = gpGaeaServer->GetCharByDbNum(_nDbNum);
            if ( _pChar == NULL )
                return 0;

            GLMSG::NET_INSTANCE_UI_TEXT_DLG_MSG NetMsg;
            if ( _makeTextDlgMsgPacket(pState, currentStack, NetMsg) == false )
                return 0;

            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, NetMsg);
            gpGaeaServer->SENDTOCLIENT(_pChar->GETCLIENTID(), NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK, SendBuffer);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Invoke - PrintTextDlgMsg ]" );
#endif

            return 0;
        }

        int PrintTextDlgXMLMsg(lua_State* pState)
        {
            int currentStack(1);

            const DWORD _nDbNum((DWORD)lua_tonumber(pState, currentStack++));
            GLChar* const _pChar = gpGaeaServer->GetCharByDbNum(_nDbNum);
            if ( _pChar == NULL )
                return 0;

            GLMSG::NET_INSTANCE_UI_TEXT_DLG_XMLMSG NetMsg;
            if ( _makeTextDlgXmlMsgPacket(pState, currentStack, NetMsg) == false )
                return 0;

            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, NetMsg);
            gpGaeaServer->SENDTOCLIENT(_pChar->GETCLIENTID(), NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK, SendBuffer);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			sc::writeLogInfo( "[ Instance Log ] [ Invoke - PrintTextDlgXMLMsg ]" );
#endif

            return 0;
        }






        


        






        ServerActor* const _getActor(lua_State* pState, int& currentStack)
        {
            const EMCROW _ActorType((EMCROW)lua_tointeger(pState, currentStack++ ));
            const DWORD _ActorID_Num((DWORD)lua_tonumber(pState, currentStack++ ));

            lua_getglobal(pState, "instanceMapID");
            const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
            lua_pop(pState, 1);

            switch( _ActorType )
            {
            case CROW_PC:
                {
                    GLChar* const pChar(gpGaeaServer->GetCharByDbNum(_ActorID_Num));
                    if ( pChar == 0 )
                        break;
                    
                    return pChar;
                }
                break;

            case CROW_MOB:
            case CROW_NPC:
            case CROW_MATERIAL:
                {
                    GLLandManager* const _pLanManager(gpGaeaServer->GetLandManager(_instanceMapID));
                    if ( _pLanManager == 0 )
                        break;

                    GLCrow* const pCrow = _pLanManager->getCrow(_ActorID_Num);
                    if ( pCrow == 0 ) 
                        break;

                    return pCrow;
                }
                break;
            }

            return NULL;
        }

        std::tr1::shared_ptr<GLClubField> _getClub(lua_State* pState, int& currentStack)
        {
            const DBNUM _ClubDbNum((DWORD)lua_tonumber(pState, currentStack++));

            std::tr1::shared_ptr<GLClubField> _spClub;
            GLClubManField* _pClubMan = gpGaeaServer->GetClubMan();
            if ( _pClubMan == NULL )
            {
                return _spClub;
            }

            _spClub = _pClubMan->GetClub(_ClubDbNum);

            return _spClub;
        }

        GLPartyField* _getParty(lua_State* pState, int& currentStack)
        {
            const DWORD _dwPartyNum((DWORD)lua_tonumber(pState, currentStack++));

            const GLPartyID sPartyID(_dwPartyNum);

            return gpGaeaServer->GetParty(sPartyID);
        }

    }
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}
