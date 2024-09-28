#pragma once

#include "./ArrayList/ArrayList.h"
#include <queue>
#include <vector>
#include <list>

namespace Timer
{
	typedef unsigned int handle;
	typedef std::queue<Timer::handle> HandleQueue;
	typedef std::vector<Timer::handle> HandleVector;
	typedef HandleVector::const_iterator HandleVectorConstIterator;
	typedef HandleVector::iterator HandleVectorIterator;
	typedef std::list<Timer::handle> HandleList;
	typedef HandleList::const_iterator HandleListConstIterator;
	typedef HandleList::iterator HandleListIterator;

	enum MESSAGE
	{
		MESSAGE_ADD,
		MESSAGE_SUSPEND,
		MESSAGE_RESUME,
		MESSAGE_ERASE,
		MESSAGE_UPDATE,
		MESSAGE_NSIZE,
	};

	class Parameter;
	class Entity;
	class Information
	{
	public:
		Information(const Timer::handle _handleTimer, const float _fTime, const float _fTimeElapsed)
			: handleTimer(_handleTimer)
			, fTime(_fTime)
			, fTimeElapsed(_fTimeElapsed)
		{
		}
		const Timer::handle handleTimer;
		const float fTime;
		const float fTimeElapsed;
	};	
}

typedef const bool (__stdcall *callbackTimerFunction)(const Timer::MESSAGE _typeMessage, Timer::Entity& _entity, const Timer::Parameter& _parameter);