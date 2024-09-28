#ifndef TriggerSystemType_H
#define TriggerSystemType_H

#include <boost/tr1/memory.hpp>
#include <list>
#include <map>

namespace TriggerSystem
{
	enum
	{
		TRIGGER_STATE_RUN,
		TRIGGER_STATE_END,
	};

	typedef int TriggerState;
	typedef int MessageType;
	typedef int TriggerType;
	typedef int TriggerID;

	typedef	std::tr1::shared_ptr<class CTrigger>			SpTrigger;
	typedef	std::tr1::shared_ptr<class CTriggerScript>		SpTriggerScript;
	typedef	std::tr1::shared_ptr<class CTriggerFinger>		SpTriggerFinger;
	typedef	std::tr1::shared_ptr<class CTriggerInstance>	SpTriggerInstance;
	typedef	std::tr1::shared_ptr<struct SMessage>			SpMessage;
	typedef	std::tr1::shared_ptr<struct SSystemMessage>		SpSystemMessage;

	typedef std::list<SpTriggerInstance>					TriggerInstanceContainer;
	typedef TriggerInstanceContainer::iterator				TriggerInstanceContainerIterator;
	typedef std::map<TSTRING, SpTriggerScript>				TriggerScriptContainer;
	typedef TriggerScriptContainer::iterator				TriggerScriptContainerIterator;
	typedef std::list<SpMessage>							MessageContainer;
	typedef MessageContainer::iterator						MessageContainerIterator;

    /**
        Ʈ���� ��ũ��Ʈ ������ ��Ʈ Ʈ���� ��
        �츮�� ��ũ��Ʈ���� �̸��� ������ Ʈ���Ű�
        ���� ó�� �����ϴ� Ʈ���� ��� ���� �Ѵ�.

     */
	static const TCHAR*	STRING_SYMBOL_ROOT_TRIGGER =		_T("root");

    /**
        �츮�� Ʈ���� ����Ʈ ��ũ��Ʈ ������ ����
        ���� Ʈ���� ��ũ��Ʈ���� �ε��ϴµ�
        �̶� ���ϵ��� ��� ���� ���Ǵ� ��Ʈ��Ī

     */
	static const TCHAR*	STRING_SYMBOL_ROOT_TRIGGERLIST =	_T("TriggerList");

} // end namespace

#endif
