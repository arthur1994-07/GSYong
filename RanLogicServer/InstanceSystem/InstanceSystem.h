#pragma once

#include "InstanceSystemDefine.h"

/*
 * ������ �� �δ��� �� ������� �����ؾ� �ϹǷ�,
 * ���� GaeaID�� ������� �ʴ´�;
*/

namespace GLMSG
{
    struct NET_INSTANCE_UI_CHATMSG;
    struct NET_INSTANCE_UI_CHATXMLMSG;
    struct NET_INSTANCE_UI_SIMPLE_MSG;
    struct NET_INSTANCE_UI_SIMPLE_XMLMSG;
    struct NET_INSTANCE_UI_TEXT_DLG_MSG;
    struct NET_INSTANCE_UI_TEXT_DLG_XMLMSG;
    struct NET_INSTANCE_UI_TIMER_MSGBOX;
}

namespace InstanceSystem
{
	sc::LuaInstance* const getScript(const SNATIVEID& _mapID);
    
    void registFunctionCommon(InstanceSystem::ScriptManager* const _pScriptManager);
    void registFunctionCommon(sc::LuaInstance& _luaInstance);

    /////////////////// Common Invoke Funtion ////////////////////////////////
    int getTime_TIME64(lua_State* pState);
    int getTime_YMD(lua_State* pState);
    int getTime_HMS(lua_State* pState);
    int getYear(lua_State* pState);
    int getMonth(lua_State* pState);
    int getDay(lua_State* pState);
    int getWeekDay(lua_State* pState);
    int getHour(lua_State* pState);
    int getMinute(lua_State* pState);
    int getSec(lua_State* pState);

    int getGameText(lua_State* pState);
    int getGameWord(lua_State* pState);

    int getSNATIVEID(lua_State* pState);

    int serverLog(lua_State* pState);

	int luaID2GAMEWORD(lua_State* pState);
	int luaID2GAMEINTEXT(lua_State* pState);
	int luaID2GAMEEXTEXT(lua_State* pState);
	int luaID2SERVERTEXT(lua_State* pState);
    int luaTableToString(lua_State* pState);

    // ���� ��� �ڵ�;
    const bool _makeChatMsgPacket(lua_State* pState, OUT int& currentStack, GLMSG::NET_INSTANCE_UI_CHATMSG& _msg);
    const bool _makeChatXmlMsgPacket(lua_State* pState, OUT int& currentStack, GLMSG::NET_INSTANCE_UI_CHATXMLMSG& _msg);

    const bool _makeSimpleMsgPacket(lua_State* pState, OUT int& currentStack, GLMSG::NET_INSTANCE_UI_SIMPLE_MSG& _msg);
    const bool _makeSimpleXmlMsgPacket(lua_State* pState, OUT int& currentStack, GLMSG::NET_INSTANCE_UI_SIMPLE_XMLMSG& _msg);

    const bool _makeTextDlgMsgPacket(lua_State* pState, OUT int& currentStack, GLMSG::NET_INSTANCE_UI_TEXT_DLG_MSG& _msg);
    const bool _makeTextDlgXmlMsgPacket(lua_State* pState, OUT int& currentStack, GLMSG::NET_INSTANCE_UI_TEXT_DLG_XMLMSG& _msg);

    const bool _makeTimerMsgBoxPacket(lua_State* pState, OUT int& currentStack, GLMSG::NET_INSTANCE_UI_TIMER_MSGBOX& _msg);
    const bool _makeXmlTimerMsgBoxPacket(lua_State* pState, OUT int& currentStack, GLMSG::NET_INSTANCE_UI_TIMER_MSGBOX& _msg);

    const bool _luaTableToString(lua_State* pState, OUT int& currentStack, std::string& _OUTstring OUT);
    const bool _luaTableToXmlString(lua_State* pState, OUT int& currentStack, InstanceSystem::InstanceXmlString& _OUTxmlString OUT, InstanceXmlString::EMXML_TYPE _emXmlType = InstanceXmlString::EMXMLGAME_IN_TEXT);
};



