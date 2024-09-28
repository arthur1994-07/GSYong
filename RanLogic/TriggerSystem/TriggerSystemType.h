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
        트리거 스크립트 파일의 루트 트리거 명
        우리는 스크립트에서 이명을 가지는 트리거가
        가장 처음 시작하는 트리거 라고 정의 한다.

     */
	static const TCHAR*	STRING_SYMBOL_ROOT_TRIGGER =		_T("root");

    /**
        우리는 트리거 리스트 스크립트 파일을 통해
        개별 트리거 스크립트들을 로딩하는데
        이때 파일들을 얻기 위해 사용되는 루트명칭

     */
	static const TCHAR*	STRING_SYMBOL_ROOT_TRIGGERLIST =	_T("TriggerList");

} // end namespace

#endif
