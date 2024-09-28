#include "pch.h"

#include "InstanceSystem.h"
#include "../../enginelib/GUInterface/GameTextControl.h"
#include "../../enginelib/G-Logic/GLogic.h"
#include "../../RanLogic/Msg/GLContrlInstanceMsg.h"

namespace InstanceSystem
{
	sc::LuaInstance* const getScript(const SNATIVEID& _keyMapID)
	{
		InstanceSystem::ScriptManager* const _pScriptManager(InstanceSystem::ScriptManager::getInstance());
		return _pScriptManager->getScript(_keyMapID);
	}	   

    void registFunctionCommon(InstanceSystem::ScriptManager* const _pScriptManager)
    {
        if ( _pScriptManager == NULL )
            return;

		_pScriptManager->registFunctionCommon( registFunctionCommon );
    }

    void registFunctionCommon(sc::LuaInstance& _luaInstance)
    {
        _luaInstance.RegistFunction("GetTime_TIME64", getTime_TIME64);
        _luaInstance.RegistFunction("GetTime_YMD", getTime_YMD);
        _luaInstance.RegistFunction("GetTime_HMS", getTime_HMS);
        _luaInstance.RegistFunction("GetYear", getYear);
        _luaInstance.RegistFunction("GetMonth", getMonth);
        _luaInstance.RegistFunction("GetDay", getDay);
        _luaInstance.RegistFunction("GetHour", getHour);
        _luaInstance.RegistFunction("GetMinute", getMinute);
        _luaInstance.RegistFunction("GetSec", getSec);
		_luaInstance.RegistFunction("GetWeekDay",getWeekDay);
        _luaInstance.RegistFunction("GetSNATIVEID",getSNATIVEID);

        _luaInstance.RegistFunction("ServerLog", serverLog);
        _luaInstance.RegistFunction("GetGameText",getGameText);

		_luaInstance.RegistFunction("ID2GAMEWORD", luaID2GAMEWORD);
		_luaInstance.RegistFunction("ID2GAMEINTEXT", luaID2GAMEINTEXT);
		_luaInstance.RegistFunction("ID2GAMEEXTEXT", luaID2GAMEEXTEXT);
		_luaInstance.RegistFunction("ID2SERVERTEXT", luaID2SERVERTEXT);

        _luaInstance.RegistFunction("TableToString", luaTableToString);
    }

    int getTime_TIME64(lua_State* pState)
    {
        CTime cCurTime = CTime::GetCurrentTime();
        lua_pushnumber(pState, (lua_Number)cCurTime.GetTime());

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
		//sc::writeLogInfo( "[ Instance Log ] [ Invoke - GetTime_TIME64 ]" );
#endif

        return 1;
    }

    int getTime_YMD(lua_State* pState)
    {
        CTime cCurTime = CTime::GetCurrentTime();
        lua_pushinteger(pState, cCurTime.GetYear());
        lua_pushinteger(pState, cCurTime.GetMonth());
        lua_pushinteger(pState, cCurTime.GetDay());

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
		//sc::writeLogInfo( "[ Instance Log ] [ Invoke - GetTime_YMD ]" );
#endif

        return 3;
    }

    int getTime_HMS(lua_State* pState)
    {
        CTime cCurTime = CTime::GetCurrentTime();
        lua_pushinteger(pState, cCurTime.GetHour());
        lua_pushinteger(pState, cCurTime.GetMinute());
        lua_pushinteger(pState, cCurTime.GetSecond());

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
		//sc::writeLogInfo( "[ Instance Log ] [ Invoke - GetTime_HMS ]" );
#endif

        return 3;
    }        

    int getYear(lua_State* pState)
    {
        CTime cCurTime = CTime::GetCurrentTime();
        lua_pushinteger(pState, cCurTime.GetYear());

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
		//sc::writeLogInfo( "[ Instance Log ] [ Invoke - GetYear ]" );
#endif

        return 1;
    }

    int getMonth(lua_State* pState)
    {
        CTime cCurTime = CTime::GetCurrentTime();
        lua_pushinteger(pState, cCurTime.GetMonth());

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
		//sc::writeLogInfo( "[ Instance Log ] [ Invoke - GetMonth ]" );
#endif

        return 1;
    }

    int getDay(lua_State* pState)
    {
        CTime cCurTime = CTime::GetCurrentTime();
        lua_pushinteger(pState, cCurTime.GetDay());

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
		//sc::writeLogInfo( "[ Instance Log ] [ Invoke - GetDay ]" );
#endif

        return 1;
    }

    int getWeekDay(lua_State* pState)
    {
        CTime cCurTime = CTime::GetCurrentTime();
        lua_pushinteger(pState, cCurTime.GetDayOfWeek());

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
		//sc::writeLogInfo( "[ Instance Log ] [ Invoke - GetWeekDay ]" );
#endif

        return 1;
    }

    int getHour(lua_State* pState)
    {
        CTime cCurTime = CTime::GetCurrentTime();
        lua_pushinteger(pState, cCurTime.GetHour());

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
		//sc::writeLogInfo( "[ Instance Log ] [ Invoke - GetHour ]" );
#endif

        return 1;
    }

    int getMinute(lua_State* pState)
    {
        CTime cCurTime = CTime::GetCurrentTime();
        lua_pushinteger(pState, cCurTime.GetMinute());

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
		//sc::writeLogInfo( "[ Instance Log ] [ Invoke - GetMinute ]" );
#endif

        return 1;
    }

    int getSec(lua_State* pState)
    {
        CTime cCurTime = CTime::GetCurrentTime();
        lua_pushinteger(pState, cCurTime.GetSecond());

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
		//sc::writeLogInfo( "[ Instance Log ] [ Invoke - GetSec ]" );
#endif

        return 1;
    }

    int getGameText(lua_State* pState)
    {
        int currentStack(1);

        const unsigned int  _iIndex(lua_tointeger(pState, currentStack++));

		const char* _szKeyword(sc::GetStringToLuaState(pState, currentStack++));

        const char* _str = ID2SERVERTEXT(_szKeyword, _iIndex);
        lua_pushstring(pState, _str);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
		//sc::writeLogInfo( "[ Instance Log ] [ Invoke - GetGameText ]" );
#endif

        return 1;
    }
//     int getGameWord(lua_State* pState)
//     {
//         const unsigned int  _iIndex(lua_tointeger(pState, -1));
//         lua_pop(pState, 1);
// 
//         int _ilen;
//         const unsigned int  _szKeyword(lua_tolstring(pState, -1, _ilen));
//         lua_pop(pState, 1);
// 
//         const char* _str = ID2GAMEWORD(_szKeyword, _iIndex);
//         lua_pushstring(pState, _str);
// 
//         return 1;
//     }
    
    int getSNATIVEID(lua_State* pState)
    {
        int currentStack(1);

        const unsigned int  _wMID(lua_tointeger(pState, currentStack++));
        const unsigned int  _wSID(lua_tointeger(pState, currentStack++));

        lua_pushinteger(pState, SNATIVEID(_wMID,_wSID).dwID);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
		//sc::writeLogInfo( "[ Instance Log ] [ Invoke - GetSNATIVEID ]" );
#endif

        return 1;
    }

    int serverLog(lua_State* pState)
    {
        int currentStack(1);

        const char* _szKeyword(sc::GetStringToLuaState(pState, currentStack++));

        sc::writeLogInfo(_szKeyword);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
		//sc::writeLogInfo( "[ Instance Log ] [ Invoke - ServerLog ]" );
#endif

        return 0;
    }

    



    const bool _makeChatMsgPacket(lua_State* pState, OUT int& currentStack, GLMSG::NET_INSTANCE_UI_CHATMSG& _msg)
    {
        const char* pStr = sc::GetStringToLuaState(pState, currentStack++);
        if ( pStr == NULL )
            return false;

        _msg.strText = pStr;

        WORD _wColorRed(0xFF);
        WORD _wColorGreen(0xFF);
        WORD _wColorBlue(0xFF);
        // 
        LuaPlus::LuaObject _colorTable(LuaPlus::LuaState::CastState(pState), currentStack++);
        if ( _colorTable.IsTable() )
        {
            LuaPlus::LuaTableIterator _colorIterator(_colorTable);
            if ( _colorIterator.IsValid() )
            {
                LuaPlus::LuaObject _colorR = _colorIterator.GetValue();
                if ( _colorR.IsInteger() )
                    _wColorRed = _colorR.GetInteger();
            }
            if ( _colorIterator.Next() )
            {
                LuaPlus::LuaObject _colorG = _colorIterator.GetValue();
                if ( _colorG.IsInteger() )
                    _wColorGreen = _colorG.GetInteger();
            }
            if ( _colorIterator.Next() )
            {
                LuaPlus::LuaObject _colorB = _colorIterator.GetValue();
                if ( _colorB.IsInteger() )
                    _wColorBlue = _colorB.GetInteger();            
            }
        }

        _msg.dwColor = (DWORD)D3DCOLOR_XRGB(_wColorRed, _wColorGreen, _wColorBlue);
        return true;
    }

    const bool _makeChatXmlMsgPacket(lua_State* pState, int& currentStack, GLMSG::NET_INSTANCE_UI_CHATXMLMSG& _msg)
    {
        if ( _luaTableToXmlString(pState, currentStack, _msg.xmlString) == false )
            return false;

        D3DCOLOR _msgColor;
        WORD _wColorRed(0xFF);
        WORD _wColorGreen(0xFF);
        WORD _wColorBlue(0xFF);
        // 
        LuaPlus::LuaObject _colorTable(LuaPlus::LuaState::CastState(pState), currentStack++);
        if ( _colorTable.IsTable() )
        {
            LuaPlus::LuaTableIterator _colorIterator(_colorTable);

            if ( _colorIterator.IsValid() )
            {
                LuaPlus::LuaObject _colorR = _colorIterator.GetValue();
                if ( _colorR.IsInteger() )
                    _wColorRed = _colorR.GetInteger();
            }
            if ( _colorIterator.Next() )
            {
                LuaPlus::LuaObject _colorG = _colorIterator.GetValue();
                if ( _colorG.IsInteger() )
                    _wColorGreen = _colorG.GetInteger();
            }
            if ( _colorIterator.Next() )
            {
                LuaPlus::LuaObject _colorB = _colorIterator.GetValue();
                if ( _colorB.IsInteger() )
                    _wColorBlue = _colorB.GetInteger();
            }
        }

        _msgColor = D3DCOLOR_XRGB(_wColorRed, _wColorGreen, _wColorBlue);

        _msg.dwColor = _msgColor;

        return true;
    }

    const bool _makeSimpleMsgPacket(lua_State* pState, OUT int& currentStack, GLMSG::NET_INSTANCE_UI_SIMPLE_MSG& _msg)
    {
        const float _fShowTime((float)lua_tonumber(pState, currentStack++));

        const char* pStr = sc::GetStringToLuaState(pState, currentStack++);
        if ( pStr == NULL )
            return false;

        _msg.fShowTime = _fShowTime;
        _msg.strText = pStr;

        WORD _wColorRed(0xFF);
        WORD _wColorGreen(0xFF);
        WORD _wColorBlue(0xFF);
        // 
        LuaPlus::LuaObject _colorTable(LuaPlus::LuaState::CastState(pState), currentStack++);
        if ( _colorTable.IsTable() )
        {
            LuaPlus::LuaTableIterator _colorIterator(_colorTable);
            if ( _colorIterator.IsValid() )
            {
                LuaPlus::LuaObject _colorR = _colorIterator.GetValue();
                if ( _colorR.IsInteger() )
                    _wColorRed = _colorR.GetInteger();
            }
            if ( _colorIterator.Next() )
            {
                LuaPlus::LuaObject _colorG = _colorIterator.GetValue();
                if ( _colorG.IsInteger() )
                    _wColorGreen = _colorG.GetInteger();
            }
            if ( _colorIterator.Next() )
            {
                LuaPlus::LuaObject _colorB = _colorIterator.GetValue();
                if ( _colorB.IsInteger() )
                    _wColorBlue = _colorB.GetInteger();            
            }
        }
        else if ( _colorTable.IsNumber() )
        {
            _msg.dwFontSize = (DWORD)_colorTable.GetInteger();
            return true;
        }

        _msg.dwColor = (DWORD)D3DCOLOR_XRGB(_wColorRed, _wColorGreen, _wColorBlue);
        _msg.dwFontSize = (DWORD)lua_tointeger(pState, currentStack++);

        return true;
    }

    const bool _makeSimpleXmlMsgPacket(lua_State* pState, OUT int& currentStack, GLMSG::NET_INSTANCE_UI_SIMPLE_XMLMSG& _msg)
    {
        const float _fShowTime((float)lua_tonumber(pState, currentStack++));

        if ( _luaTableToXmlString(pState, currentStack, _msg.xmlString) == false )
            return false;

        _msg.fShowTime = _fShowTime;

        WORD _wColorRed(0xFF);
        WORD _wColorGreen(0xFF);
        WORD _wColorBlue(0xFF);
        // 
        LuaPlus::LuaObject _colorTable(LuaPlus::LuaState::CastState(pState), currentStack++);
        if ( _colorTable.IsTable() )
        {
            LuaPlus::LuaTableIterator _colorIterator(_colorTable);
            if ( _colorIterator.IsValid() )
            {
                LuaPlus::LuaObject _colorR = _colorIterator.GetValue();
                if ( _colorR.IsInteger() )
                    _wColorRed = _colorR.GetInteger();
            }
            if ( _colorIterator.Next() )
            {
                LuaPlus::LuaObject _colorG = _colorIterator.GetValue();
                if ( _colorG.IsInteger() )
                    _wColorGreen = _colorG.GetInteger();
            }
            if ( _colorIterator.Next() )
            {
                LuaPlus::LuaObject _colorB = _colorIterator.GetValue();
                if ( _colorB.IsInteger() )
                    _wColorBlue = _colorB.GetInteger();            
            }
        }
        else if ( _colorTable.IsNumber() )
        {
            _msg.dwFontSize = (DWORD)_colorTable.GetInteger();
            return true;
        }

        _msg.dwColor = (DWORD)D3DCOLOR_XRGB(_wColorRed, _wColorGreen, _wColorBlue);
        _msg.dwFontSize = (DWORD)lua_tointeger(pState, currentStack++);

        return true;
    }

    const bool _makeTextDlgMsgPacket(lua_State* pState, OUT int& currentStack, GLMSG::NET_INSTANCE_UI_TEXT_DLG_MSG& _msg)
    {
        const char* pStr = sc::GetStringToLuaState(pState, currentStack++);
        if ( pStr == NULL )
            return false;
        
        _msg.strText = pStr;

        WORD _wColorRed(0xFF);
        WORD _wColorGreen(0xFF);
        WORD _wColorBlue(0xFF);
        // 
        LuaPlus::LuaObject _colorTable(LuaPlus::LuaState::CastState(pState), currentStack++);
        if ( _colorTable.IsTable() )
        {
            LuaPlus::LuaTableIterator _colorIterator(_colorTable);
            if ( _colorIterator.IsValid() )
            {
                LuaPlus::LuaObject _colorR = _colorIterator.GetValue();
                if ( _colorR.IsInteger() )
                    _wColorRed = _colorR.GetInteger();
            }
            if ( _colorIterator.Next() )
            {
                LuaPlus::LuaObject _colorG = _colorIterator.GetValue();
                if ( _colorG.IsInteger() )
                    _wColorGreen = _colorG.GetInteger();
            }
            if ( _colorIterator.Next() )
            {
                LuaPlus::LuaObject _colorB = _colorIterator.GetValue();
                if ( _colorB.IsInteger() )
                    _wColorBlue = _colorB.GetInteger();            
            }
        }

        _msg.dwColor = (DWORD)D3DCOLOR_XRGB(_wColorRed, _wColorGreen, _wColorBlue);

        return true;
    }

    const bool _makeTextDlgXmlMsgPacket(lua_State* pState, OUT int& currentStack, GLMSG::NET_INSTANCE_UI_TEXT_DLG_XMLMSG& _msg)
    {
        if ( _luaTableToXmlString(pState, currentStack, _msg.xmlString) == false )
            return false;

        WORD _wColorRed(0xFF);
        WORD _wColorGreen(0xFF);
        WORD _wColorBlue(0xFF);
        // 
        LuaPlus::LuaObject _colorTable(LuaPlus::LuaState::CastState(pState), currentStack++);
        if ( _colorTable.IsTable() )
        {
            LuaPlus::LuaTableIterator _colorIterator(_colorTable);
            if ( _colorIterator.IsValid() )
            {
                LuaPlus::LuaObject _colorR = _colorIterator.GetValue();
                if ( _colorR.IsInteger() )
                    _wColorRed = _colorR.GetInteger();
            }
            if ( _colorIterator.Next() )
            {
                LuaPlus::LuaObject _colorG = _colorIterator.GetValue();
                if ( _colorG.IsInteger() )
                    _wColorGreen = _colorG.GetInteger();
            }
            if ( _colorIterator.Next() )
            {
                LuaPlus::LuaObject _colorB = _colorIterator.GetValue();
                if ( _colorB.IsInteger() )
                    _wColorBlue = _colorB.GetInteger();            
            }
        }

        _msg.dwColor = (DWORD)D3DCOLOR_XRGB(_wColorRed, _wColorGreen, _wColorBlue);

        return true;
    }

    const bool _makeTimerMsgBoxPacket(lua_State* pState, OUT int& currentStack, GLMSG::NET_INSTANCE_UI_TIMER_MSGBOX& _msg)
    {
        const char* _szContentText(sc::GetStringToLuaState(pState,currentStack++));
        const char* _szOkText(sc::GetStringToLuaState(pState,currentStack++));
        const char* _szCancleText(sc::GetStringToLuaState(pState,currentStack++));

        const float _fTimer((float)lua_tonumber(pState,currentStack++));
        const DWORD _dwResponseMsgParam1(lua_tointeger(pState,currentStack++));
        const DWORD _dwResponseMsgParam2(lua_tointeger(pState,currentStack++));

        _msg._strContentText = _szContentText;
        _msg._strOkText = _szOkText;
        _msg._strCancelText = _szCancleText;
        _msg.fTimer = _fTimer;
        _msg.dwResponseMsgParam1 = _dwResponseMsgParam1;
        _msg.dwResponseMsgParam2 = _dwResponseMsgParam2;

        return true;
    }

    const bool _makeXmlTimerMsgBoxPacket(lua_State* pState, int& currentStack, GLMSG::NET_INSTANCE_UI_TIMER_MSGBOX& _msg)
    {
        std::string _strContentText;
        if ( _luaTableToString(pState, currentStack, _strContentText) == false )
            return false;

        std::string _strOkText;
        if ( _luaTableToString(pState, currentStack, _strOkText) == false )
            return false;

        std::string _strCancelText;
        if ( _luaTableToString(pState, currentStack, _strCancelText) == false )
            return false;

        const float _fTimer((float)lua_tonumber(pState,currentStack++));
        const DWORD _dwResponseMsgParam1(lua_tointeger(pState,currentStack++));
        const DWORD _dwResponseMsgParam2(lua_tointeger(pState,currentStack++));

        _msg._strContentText = _strContentText.c_str();
        _msg._strOkText = _strOkText.c_str();
        _msg._strCancelText = _strCancelText.c_str();
        _msg.fTimer = _fTimer;
        _msg.dwResponseMsgParam1 = _dwResponseMsgParam1;
        _msg.dwResponseMsgParam2 = _dwResponseMsgParam2;

        return true;
    }

    const bool _luaTableToString(lua_State* pState, int& currentStack, std::string& _OUTstring OUT)
    {
        LuaPlus::LuaObject _XmlStringTable(LuaPlus::LuaState::CastState(pState), currentStack++);
        if ( _XmlStringTable.IsTable() == false )
            return false;
        
        LuaPlus::LuaTableIterator _iter(_XmlStringTable);
        if ( _iter.IsValid() == false )
            return false;

        LuaPlus::LuaObject _obj;
        _obj = _iter.GetValue();
        if ( _obj.IsString() == false  )
            return false;
        const char* _strXmlKey = _obj.GetString();
        DWORD _dwXmlIndex = 0;
        std::vector<std::string> _paramVector;
        _paramVector.reserve(9);

        if ( _iter.Next() )
        {
            _obj = _iter.GetValue();
            if ( _obj.IsNumber() == false )
                return false;
            _dwXmlIndex = _obj.GetInteger();
            for ( _iter.Next(); _iter.IsValid(); _iter.Next() )
            {
                _obj = _iter.GetValue();
                switch ( _obj.Type() )
                {
                case LUA_TNUMBER:
                    {
                        const float _fParam = (float)_obj.GetNumber();
                        const int _iParam = (int)_fParam;
                        if ( _fParam == (float)_iParam )
                            _paramVector.push_back(sc::string::format("%1%", (int)_fParam));
                        else
                            _paramVector.push_back(sc::string::format("%1%", _fParam));
                    }                        
                    break;
                case LUA_TSTRING:
                    _paramVector.push_back(sc::string::format("%1%", _obj.GetString()).c_str());
                    break;
                }
            }
        }

        switch( _paramVector.size() )
        {
        case 0:
            _OUTstring = ID2SERVERTEXT(_strXmlKey, _dwXmlIndex);
            break;
        case 1:
            _OUTstring = sc::string::format(ID2SERVERTEXT(_strXmlKey, _dwXmlIndex), _paramVector[0]);
            break;
        case 2:
            _OUTstring = sc::string::format(ID2SERVERTEXT(_strXmlKey, _dwXmlIndex), _paramVector[0], _paramVector[1]);
            break;
        case 3:
            _OUTstring = sc::string::format(ID2SERVERTEXT(_strXmlKey, _dwXmlIndex), _paramVector[0], _paramVector[1], _paramVector[2]);
            break;
        case 4:
            _OUTstring = sc::string::format(ID2SERVERTEXT(_strXmlKey, _dwXmlIndex), _paramVector[0], _paramVector[1], _paramVector[2], _paramVector[3]);
            break;
        case 5:
            _OUTstring = sc::string::format(ID2SERVERTEXT(_strXmlKey, _dwXmlIndex), _paramVector[0], _paramVector[1], _paramVector[2], _paramVector[3],
                _paramVector[4]);
            break;
        case 6:
            _OUTstring = sc::string::format(ID2SERVERTEXT(_strXmlKey, _dwXmlIndex), _paramVector[0], _paramVector[1], _paramVector[2], _paramVector[3],
                _paramVector[4], _paramVector[5]);
            break;
        case 7:
            _OUTstring = sc::string::format(ID2SERVERTEXT(_strXmlKey, _dwXmlIndex), _paramVector[0], _paramVector[1], _paramVector[2], _paramVector[3],
                _paramVector[4], _paramVector[5], _paramVector[6]);
            break;
        case 8:
            _OUTstring = sc::string::format(ID2SERVERTEXT(_strXmlKey, _dwXmlIndex), _paramVector[0], _paramVector[1], _paramVector[2], _paramVector[3],
                _paramVector[4], _paramVector[5], _paramVector[6], _paramVector[7]);
            break;
        case 9:
        default:
            _OUTstring = sc::string::format(ID2SERVERTEXT(_strXmlKey, _dwXmlIndex), _paramVector[0], _paramVector[1], _paramVector[2], _paramVector[3],
                _paramVector[4], _paramVector[5], _paramVector[6], _paramVector[7], _paramVector[8]);
            break;
        }

        return true;
    }

    const bool _luaTableToXmlString(lua_State* pState, OUT int& currentStack, InstanceSystem::InstanceXmlString& _OUTxmlString OUT, InstanceXmlString::EMXML_TYPE _emXmlType /*= InstanceXmlString::EMXMLGAME_IN_TEXT*/)
    {        
        _OUTxmlString.emXmlType = _emXmlType;

        LuaPlus::LuaObject _xmlStringTable(LuaPlus::LuaState::CastState(pState), currentStack++);
        if ( _xmlStringTable.IsTable() == false )
            return false;

        LuaPlus::LuaTableIterator _iter(_xmlStringTable);

        if ( _iter.IsValid() == false )
            return false;

        LuaPlus::LuaObject _Obj = _iter.GetValue();
        if ( _Obj.IsString() == false )
            return false;

        _OUTxmlString.strXmlKey = _Obj.GetString();
        if ( _iter.Next() == false )
        {
            _OUTxmlString.index = 0;
            return true;
        }

        _Obj = _iter.GetValue();
        if ( _Obj.IsInteger() == false )
        {
            _OUTxmlString.index = 0;
            return true;
        }

        _OUTxmlString.index = _Obj.GetInteger();
        for ( _iter.Next(); _iter.IsValid(); _iter.Next() )
        {
            _Obj = _iter.GetValue();
            if ( _Obj.IsString() )
            {
                _OUTxmlString.XmlParamVector.push_back(_Obj.GetString());
            }
            else if( _Obj.IsNumber() )
            {
                _OUTxmlString.XmlParamVector.push_back( sc::string::format("%1%", _Obj.GetNumber()) );
            }
        }
        return true;
    }


	int luaID2GAMEWORD(lua_State* pState)
	{
        int currentStack(1);

		std::string strTextString (sc::GetStringToLuaState(pState, currentStack++));
        int indexnum = lua_tointeger(pState, currentStack++);  // 만약 인자가 없거나 int가 아니라면 0;
		lua_pushlstring(pState, ID2GAMEWORD(strTextString.c_str(),indexnum), strTextString.length());

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
		sc::writeLogInfo( "[ Instance Log ] [ Invoke - ID2GAMEWORD ]" );
#endif

		return 1;
	}
	int luaID2GAMEINTEXT(lua_State* pState)
	{
        int currentStack(1);

        std::string strTextString (sc::GetStringToLuaState(pState, currentStack++));
        int indexnum = lua_tointeger(pState, currentStack++);
		lua_pushlstring(pState, ID2GAMEINTEXT(strTextString.c_str(),indexnum), strTextString.length());

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
		sc::writeLogInfo( "[ Instance Log ] [ Invoke - ID2GAMEINTEXT ]" );
#endif

		return 1;

	}
	int luaID2GAMEEXTEXT(lua_State* pState)
	{
        int currentStack(1);

        std::string strTextString (sc::GetStringToLuaState(pState, currentStack++));
        int indexnum = lua_tointeger(pState, currentStack++);
        lua_pushlstring(pState, ID2GAMEEXTEXT(strTextString.c_str(),indexnum), strTextString.length());

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
		sc::writeLogInfo( "[ Instance Log ] [ Invoke - ID2GAMEEXTEXT ]" );
#endif

		return 1;

	}
	int luaID2SERVERTEXT(lua_State* pState)
	{
        int currentStack(1);

        std::string strTextString (sc::GetStringToLuaState(pState, currentStack++));
        int indexnum = lua_tointeger(pState, currentStack++);
        lua_pushlstring(pState, ID2SERVERTEXT(strTextString.c_str(),indexnum), strTextString.length());

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
		sc::writeLogInfo( "[ Instance Log ] [ Invoke - ID2SERVERTEXT ]" );
#endif

		return 1;
	}
    int luaTableToString(lua_State* pState)
    {
        int currentStack(1);
        std::string _outString;
        if ( _luaTableToString(pState, currentStack, _outString) == false )
            return 0;

        lua_pushlstring(pState, _outString.c_str(), _outString.length());

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
		sc::writeLogInfo( "[ Instance Log ] [ Invoke - TableToString ]" );
#endif

        return 1;
    }
}
