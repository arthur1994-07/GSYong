#include "pch.h"

#include "../../../../RanLogic/Msg/GLContrlEventMsg.h"
#include "../../../../RanLogic/Timer/Timer.h"
#include "../../../AgentServer/GLAgentServer.h"
#include "./EventAgentItem.h"

namespace GLEvent
{
	ItemSet::ItemSet(const GLEvent::ItemSet& _rhs)
		: itemID(_rhs.itemID)
		, fRate(_rhs.fRate)
	{
	}
	ItemSet::ItemSet(const SNATIVEID& _itemID, const float _fRate)
		: itemID(_itemID)
		, fRate(_fRate)
	{
	}
	ItemSet::ItemSet(void)
		: itemID(SNATIVEID(false))
		, fRate(0.0f)
	{
	}



	ItemTable::ItemTable(const GLEvent::ItemTable& _rhs)
		: stringMessage(_rhs.stringMessage)
		, timeInterval(_rhs.timeInterval)
		, vectorEventItemSet(_rhs.vectorEventItemSet)
	{
	}
	ItemTable::ItemTable(const std::string _stringMessage, const int _timeInterval)
		: stringMessage(_stringMessage)
		, timeInterval(_timeInterval)
	{
	}
	ItemTable::ItemTable(void)
		: timeInterval(0)
	{
	}



	Item::Item(GLAgentServer* const _pAgentServer, const std::string& _stringScriptEvent, const __time64_t _timeDuration, const __time64_t& _timeCurrent, const bool _bLoopBack, const bool _bShowInUI)
		: GLEvent::Interface(GLEvent::TYPE_ITEM, _stringScriptEvent, _timeDuration, _timeCurrent)
		, pAgentServer(_pAgentServer)
		, bLoopBack(_bLoopBack)
		, bShowInUI(_bShowInUI)
	{
	}
	Item::Item(const GLEvent::Item& _rhs)
		: GLEvent::Interface(_rhs)
		, pAgentServer(_rhs.pAgentServer)
		, bLoopBack(_rhs.bLoopBack)
		, bShowInUI(_rhs.bShowInUI)
		, vectorEventItemTable(_rhs.vectorEventItemTable)
	{
	}
	Item::Item(void)
		: GLEvent::Interface()
		, pAgentServer(0)
		, bLoopBack(false)
		, bShowInUI(false)
	{
	}
	Item::~Item(void)
	{
	}
	void Item::vf_frameMove(const float _fTime, const float _fTimeElapsed, const __time64_t& _timeCurrent)
	{		
	}
	const bool Item::vf_messageProcedure(const unsigned int _clientID, const unsigned int _gaeaID, const NET_MSG_GENERIC* const _pMessage)
	{
		const GLMSG::NET_EVENT_MESSAGE* const _pMessageEvent((const GLMSG::NET_EVENT_MESSAGE* const)_pMessage);

		const GLEvent::MESSAGE _typeMessage(_pMessageEvent->typeMessage);
		switch ( _typeMessage )
		{
		case GLEvent::MESSAGE_ITEM_INFORMATION_SET:
			return Item::_messageProcedureItemInformationSet(_clientID, _gaeaID, _pMessage);
		case GLEvent::MESSAGE_ITEM_INFORMATION_DONE:
			return Item::_messageProcedureItemInformationDone(_clientID, _gaeaID, _pMessage);		
		}

		return true;
	}	
	void Item::vf_online(const unsigned int _dbNum)
	{
		GLCharAG* _pCharacter(this->pAgentServer->GetCharByDbNum(_dbNum));
		if ( _pCharacter == NULL )
			return;		

		GLEvent::ItemTableVector& _vectorEventItemTable(this->vectorEventItemTable);
		if ( _vectorEventItemTable.size() == 0 )
			return;

		const GLEvent::ItemTable& _tableItem(_vectorEventItemTable[0]);
		const float _fTimeInterval(float(_tableItem.getTimeInterval()));

		// valueInteger2는 케릭터 dbNum으로 사용 되므로 사용하지 말아야 한다;
		Timer::Parameter _parameter;
		_parameter.valuePointer0 = this->pAgentServer;
		_parameter.valueInteger1 = this->hashKeyScriptEvent;		
		_parameter.valueInteger3 = 0;

		_pCharacter->registTimerEvent(GLEvent::callbackTimerEventItemFunction, _parameter, _fTimeInterval);
	}
	void Item::vf_offline(const unsigned int _dbNum)
	{
	}
	const bool Item::_messageProcedureItemInformationSet(const unsigned int _clientID, const unsigned int _gaeaID, const NET_MSG_GENERIC* const _pMessage)
	{
		const GLMSG::NET_EVENT_ITEM_INFORMATION_SET* const _pMessageInformationSet((const GLMSG::NET_EVENT_ITEM_INFORMATION_SET* const)_pMessage);

		const std::string _stringMessageOut(_pMessageInformationSet->cMessageOut);
		const int _timeInterval(_pMessageInformationSet->timeInterval);

		GLEvent::ItemTableVector& _vectorEventItemTable(this->vectorEventItemTable);
		const unsigned int _indexItemTable(_vectorEventItemTable.size());

		const GLEvent::ItemTable _tableItemData(_stringMessageOut, _timeInterval);	
		_vectorEventItemTable.push_back(_tableItemData);

		GLEvent::ItemTable& _tableItem(_vectorEventItemTable[_indexItemTable]);

		const unsigned int _sizeItemSet(_pMessageInformationSet->sizeItem);
		for ( unsigned int _indexItemSet(0); _indexItemSet < _sizeItemSet; ++_indexItemSet )
		{
			const GLMSG::NET_EVENT_ITEM_INFORMATION_SET::Entity& _entity(_pMessageInformationSet->entityItem[_indexItemSet]);
			const SNATIVEID _itemID(_entity.itemID);
			const float _fRate(_entity.fRate);

			const GLEvent::ItemSet _itemSet(_itemID, _fRate);
			_tableItem.push_back(_itemSet);
		}

		return true;
	}
	const bool Item::_messageProcedureItemInformationDone(const unsigned int _clientID, const unsigned int _gaeaID, const NET_MSG_GENERIC* const _pMessage)
	{
		const GLMSG::NET_EVENT_ITEM_INFORMATION_DONE* const _pMessageInformationDone((const GLMSG::NET_EVENT_ITEM_INFORMATION_DONE* const)_pMessage);

		if ( _pMessageInformationDone->bComplete == false )
			return false;

		GLEvent::ItemTableVector& _vectorEventItemTable(this->vectorEventItemTable);
		if ( _vectorEventItemTable.size() == 0 )
			return false;

		const GLEvent::ItemTable& _tableItem(_vectorEventItemTable[0]);
		const float _fTimeInterval(float(_tableItem.getTimeInterval()));

		// valueInteger2는 케릭터 dbNum으로 사용 되므로 사용하지 말아야 한다;
		Timer::Parameter _parameter;
		_parameter.valuePointer0 = this->pAgentServer;
		_parameter.valueInteger1 = this->hashKeyScriptEvent;		
		_parameter.valueInteger3 = 0;

		this->pAgentServer->registTimerEvent(GLEvent::callbackTimerEventItemFunction, _parameter, _fTimeInterval);		

		return true;
	}
	const bool Item::_callbackTimerEventItemFunction(const Timer::MESSAGE _typeMessage, Timer::Entity& _entity, const Timer::Parameter& _parameter)
	{
		GLAgentServer* const _pAgentServer((GLAgentServer* const)_parameter.valuePointer0);
		const unsigned int _dbNum(unsigned int(_parameter.valueInteger2));
		const unsigned int _indexItem(unsigned int(_parameter.valueInteger3));

		GLCharAG* const _pCharacter(_pAgentServer->GetCharByDbNum(_dbNum));
		if ( _pCharacter == 0 )
			return false; // 해당 케릭터가 존재 하지 않으므로 종료;

		const unsigned int _sizeEventItemTable(this->vectorEventItemTable.size());
		if ( _indexItem >= _sizeEventItemTable )
		{
			_pCharacter->eraseTimerEvent(_entity.handleTimer);
			return false;
		}

		unsigned int _indexItemNext(_indexItem + 1);
		if ( _indexItemNext >= _sizeEventItemTable )
		{ // 반복 여부에 대한 처리를 한다;			
			if ( this->bLoopBack == true )
				_indexItemNext = 0; // 반복 기능이 켜져 있다면 처음부터 다시;
			else
			{
				_pCharacter->eraseTimerEvent(_entity.handleTimer);
				return false; // 반복 기능이 꺼져 있다면 종료;
			}
		}		

		{ // 다음 지급될 아이템을 위한 설정을 한다;
			const GLEvent::ItemTable& _tableItemNext(this->vectorEventItemTable[_indexItemNext]);
			const int _timeIntervalNext(_tableItemNext.getTimeInterval());

			Timer::Manager& _managerTimer(Timer::Manager::getInstance());
			Timer::Parameter _parameterNew(_parameter);
			_parameterNew.valueInteger3= _indexItemNext;
			_pCharacter->eraseTimerEvent(_entity.handleTimer);
			_pCharacter->registTimerEvent(GLEvent::callbackTimerEventItemFunction, _parameterNew, float(_timeIntervalNext));

			sc::Random& _random(sc::Random::getInstance());			

			{ // 아이템 지급 처리;
				const GLEvent::ItemTable& _tableItem(this->vectorEventItemTable[_indexItem]);
				const std::string& _stringMessageOut(_tableItem.getMessage());
				const GLEvent::ItemSetVector& _vectorItemSet(_tableItem.getItemSetVector());

				GLMSG::NET_EVENT_ITEM_TRANSFER_MESSAGE _messageTransferMessage(_stringMessageOut);
				this->pAgentServer->SENDTOCLIENT(_pCharacter->ClientSlot(), &_messageTransferMessage);
				
				GLMSG::NET_EVENT_ITEM_TRANSFER_ITEM _messageTransferItem(_dbNum);

				const unsigned int _sizeItemSet(_vectorItemSet.size());
				for ( unsigned int _index(_sizeItemSet); _index--; )
				{
					const GLEvent::ItemSet& _itemSet(_vectorItemSet[_index]);
					if ( _itemSet.fRate <= _random.GetFloatRate() )
						continue;

					if ( _messageTransferItem.addItem(_itemSet.itemID) == false )
						break;
				}

				const unsigned int _fieldChannel(_pCharacter->GetChannel());
				const unsigned int _fieldServer(_pCharacter->GetCurrentFieldServer());
				this->pAgentServer->SENDTOFIELD(_fieldChannel, _fieldServer, &_messageTransferItem);
			}
		}

		return false;
	}

	const bool __stdcall callbackTimerEventItemFunction(const Timer::MESSAGE _typeMessage, Timer::Entity& _entity, const Timer::Parameter& _parameter)
	{
		if ( _typeMessage != Timer::MESSAGE_UPDATE )
			return true;

		GLAgentServer* const _pAgentServer((GLAgentServer* const)_parameter.valuePointer0);

		GLEvent::ManagerAgent* const _pManagerEvent(_pAgentServer->getEventManager());		
		const unsigned int _keyEventInterface(unsigned int(_parameter.valueInteger1));
		GLEvent::Interface* const _pEventInterface(_pManagerEvent->findInterface(_keyEventInterface));
		if ( _pEventInterface == 0 )
			return false;

		if ( _pEventInterface->getTypeEvent() != GLEvent::TYPE_ITEM )
			return false;

		GLEvent::Item* const _pEventItem((GLEvent::Item* const)_pEventInterface);
		return _pEventItem->_callbackTimerEventItemFunction(_typeMessage, _entity, _parameter);
	}
}
