#include "pch.h"

#include "../../EngineLib/G-Logic/GLOGIC.h"
#include "../../SigmaCore/Lua/MinLua.h"
#include "../../SigmaCore/String/StringUtil.h"
#include "../../RanLogic/Msg/GLContrlEventMsg.h"
#include "../GLGaeaClient.h"
#include "./EventClient.h"

namespace GLEvent
{
	const bool ManagerClient::messageProcedure(NET_MSG_GENERIC* const _pMessage)
	{
		GLMSG::NET_EVENT_MESSAGE* const _pMessageEvent((GLMSG::NET_EVENT_MESSAGE* const)_pMessage);

		const GLEvent::MESSAGE _typeMessage(_pMessageEvent->typeMessage);
		switch ( _typeMessage )
		{
		case GLEvent::MESSAGE_END_FB:
			return ManagerClient::_messageProcedureEndFB(_pMessage);
		case GLEvent::MESSAGE_LIST_FB:
			return ManagerClient::_messageProcedureListFB(_pMessage);
		case GLEvent::MESSAGE_ITEM_BEGIN:
			return ManagerClient::_messageProcedureBegin(_pMessage);
		case GLEvent::MESSAGE_ITEM_TRANSFER_MESSAGE:
			return ManagerClient::_messageProcedureTransferMessage(_pMessage);
		}

		return true;
	}
	const bool ManagerClient::_messageProcedureEndFB(NET_MSG_GENERIC* const _pMessage)
	{
		GLMSG::NET_EVENT_END_FB* const _pMessageEndFB((GLMSG::NET_EVENT_END_FB* const)_pMessage);

		const unsigned int _colorNormal(D3DCOLOR_XRGB(239, 228, 176));
		this->pGaeaClient->PrintMsgText(_colorNormal, std::string("terminate event-script"));

		return true;
	}
	const bool ManagerClient::_messageProcedureTransferMessage(NET_MSG_GENERIC* const _pMessage)
	{
		GLMSG::NET_EVENT_ITEM_TRANSFER_MESSAGE* _pMessageTransferMessage((GLMSG::NET_EVENT_ITEM_TRANSFER_MESSAGE* const)_pMessage);

		const unsigned int _colorNormal(D3DCOLOR_XRGB(239, 228, 176));
		this->pGaeaClient->PrintMsgText(_colorNormal, std::string(_pMessageTransferMessage->cMessageOut));

		return true;
	}	
	const bool ManagerClient::_messageProcedureBegin(NET_MSG_GENERIC* const _pMessage)
	{
		GLMSG::NET_EVENT_BEGIN_ITEM* const _pMessageEventBegin((GLMSG::NET_EVENT_BEGIN_ITEM* const)_pMessage);

		const std::string _stringNameFileScript(_pMessageEventBegin->cNameFileScript);

		const std::string _stringFullPathFileScript(std::string(GLOGIC::GetPath()) + _stringNameFileScript);

		GLGaeaClient* const _pGaeaClient(this->pGaeaClient);

		const unsigned int _colorTomato(D3DCOLOR_XRGB(239, 132, 84));

		sc::lua_init();

		if ( sc::lua_doFile(_stringFullPathFileScript) == false )
			return false;

		LuaPlus::LuaObject _objectTableBasic(sc::lua_getGlobalFromName("Basic"));
		if ( _objectTableBasic.IsNil() == true )
			return false;
		
		LuaPlus::LuaObject _objectTypeEvent(_objectTableBasic.GetByName("typeEvent"));
		if ( _objectTypeEvent.IsNil() == true )
		{
			_pGaeaClient->PrintMsgText(_colorTomato, sc::string::format("can't find [typeEvent] in %1%;", _stringFullPathFileScript));
			return false;
		}

		const std::string _stringTypeEvent(sc::string::unicodeToAnsi((const wchar_t* const)_objectTypeEvent.GetWString()));
		if ( _stringTypeEvent != std::string("eventItem") )
		{
			_pGaeaClient->PrintMsgText(_colorTomato, sc::string::format("%1% is not type of [eventItem];", _stringFullPathFileScript));
			return false;
		}

		LuaPlus::LuaObject _objectLoopBack(_objectTableBasic.GetByName("bLoopBack"));
		const bool _bLoopBack(_objectLoopBack.IsNil() == true ? false : _objectLoopBack.GetBoolean());

		LuaPlus::LuaObject _objectShowInUI(_objectTableBasic.GetByName("bShowInUI"));
		const bool _bShowInUI(_objectShowInUI.IsNil() == true ? true : _objectShowInUI.GetBoolean());

		LuaPlus::LuaObject _objectTimeDuration(_objectTableBasic.GetByName("timeDuration"));
		if ( _objectTimeDuration.IsNil() == true )
		{
			_pGaeaClient->PrintMsgText(_colorTomato, sc::string::format("can't find [timeDuration] in %1%;", _stringFullPathFileScript));
			return false;
		}
		const unsigned int _timeDuration(_objectTimeDuration.GetInteger());

		const char* const _cNameFileScript(_pMessageEventBegin->cNameFileScript);

		GLMSG::NET_EVENT_ITEM_INFORMATION_BASIC _messageInformationBasic(_cNameFileScript, _timeDuration, _bLoopBack, _bShowInUI);
		_pGaeaClient->NETSENDTOAGENT(&_messageInformationBasic);

		LuaPlus::LuaObject _objectTableItem(sc::lua_getGlobalFromName("itemTable"));
		for ( LuaPlus::LuaTableIterator _iteratorItemSet(_objectTableItem); _iteratorItemSet; _iteratorItemSet.Next() )
		{
			LuaPlus::LuaTableIterator _iteratorSet(_iteratorItemSet.GetValue());

			const LuaPlus::LuaObject _objectMessage(_iteratorSet.GetValue());
			const std::string _stringMessageOut(sc::string::unicodeToAnsi((const wchar_t* const)_objectMessage.GetWString()));
			_iteratorSet.Next();

			const LuaPlus::LuaObject _objectTimeInterval(_iteratorSet.GetValue());
			const int _timeInterval(_objectTimeInterval.GetInteger());
			_iteratorSet.Next();

			GLMSG::NET_EVENT_ITEM_INFORMATION_SET _messageInformationSet(_cNameFileScript, _stringMessageOut, _timeInterval);
			for ( ; _iteratorSet; )
			{
				LuaPlus::LuaObject _objectItemIDMain(_iteratorSet.GetValue());
				if ( _objectItemIDMain.IsNil() == true )
				{
					_pGaeaClient->PrintMsgText(_colorTomato, sc::string::format("invalid item-table in %1%;", _stringFullPathFileScript));
					_pGaeaClient->NETSENDTOAGENT(&GLMSG::NET_EVENT_ITEM_INFORMATION_DONE(_cNameFileScript, false));
					return false;
				}
				const unsigned int _itemIDMain(_objectItemIDMain.GetInteger());
				_iteratorSet.Next();

				LuaPlus::LuaObject _objectItemIDSub(_iteratorSet.GetValue());
				if ( _objectItemIDSub.IsNil() == true )
				{
					_pGaeaClient->PrintMsgText(_colorTomato, sc::string::format("invalid item-table in %1%;", _stringFullPathFileScript));
					_pGaeaClient->NETSENDTOAGENT(&GLMSG::NET_EVENT_ITEM_INFORMATION_DONE(_cNameFileScript, false));
					return false;
				}
				const unsigned int _itemIDSub(_objectItemIDSub.GetInteger());
				_iteratorSet.Next();

				LuaPlus::LuaObject _objectRate(_iteratorSet.GetValue());
				if ( _objectRate.IsNil() == true )
				{
					_pGaeaClient->PrintMsgText(_colorTomato, sc::string::format("invalid item-table in %1%;", _stringFullPathFileScript));
					_pGaeaClient->NETSENDTOAGENT(&GLMSG::NET_EVENT_ITEM_INFORMATION_DONE(_cNameFileScript, false));
					return false;
				}
				const float _fRate(_objectRate.GetFloat());
				_iteratorSet.Next();

				if ( _messageInformationSet.addItem(_itemIDMain, _itemIDSub, _fRate) == false )
				{
					_pGaeaClient->NETSENDTOAGENT(&_messageInformationSet);
					_messageInformationSet.reset();
				}				
			}
			if ( _messageInformationSet.isValid() == true )
			{
				_pGaeaClient->NETSENDTOAGENT(&_messageInformationSet);
				_messageInformationSet.reset();
			}
		}

		_pGaeaClient->NETSENDTOAGENT(&GLMSG::NET_EVENT_ITEM_INFORMATION_DONE(_cNameFileScript, true));

		const unsigned int _colorNormal(D3DCOLOR_XRGB(239, 228, 176));

		this->pGaeaClient->PrintMsgText(_colorNormal, std::string("Event-Script Loaded;"));

		return true;
	}
	const bool ManagerClient::_messageProcedureListFB(NET_MSG_GENERIC* const _pMessage)
	{
		GLMSG::NET_EVENT_LIST_FB* const _pMessageListFB((GLMSG::NET_EVENT_LIST_FB* const)_pMessage);

		const unsigned int _colorNormal(D3DCOLOR_XRGB(239, 228, 176));

		const unsigned int _sizeList(_pMessageListFB->sizeList);

		for ( unsigned int _index(0); _index < _sizeList; ++_index )
		{
			this->pGaeaClient->PrintMsgText(_colorNormal,
				sc::string::format("[%1%] %2%;", _index + 1, _pMessageListFB->cNameFileScript[_index]));			
		}

		return true;
	}
}