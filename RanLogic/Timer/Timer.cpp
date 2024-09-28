#include "pch.h"

#include "./Timer.h"

namespace Timer
{
	Parameter::Parameter(void)
	{
	}
	Parameter::Parameter(const Timer::Parameter& _rhs)
		: valueInteger0(_rhs.valueInteger0)
		, valueInteger1(_rhs.valueInteger1)
		, valueInteger2(_rhs.valueInteger2)
		, valueInteger3(_rhs.valueInteger3)
	{
	}


	Entity::Entity(const Timer::handle _handleTimer, const callbackTimerFunction& _functionCallbackTimer, const Timer::Parameter& _parameter, const float _fTimeIntervalCall)
		: handleTimer(_handleTimer)
		, functionCallbackTimer(_functionCallbackTimer)
		, parameter(_parameter)
		, fTimeIntervalCall(_fTimeIntervalCall)
		, fTimeElapsed(0.0f)
		, bRunning(true)
	{
	}
	Entity::Entity(const Timer::Entity& _rhs)
		: handleTimer(_rhs.handleTimer)
		, functionCallbackTimer(_rhs.functionCallbackTimer)
		, parameter(_rhs.parameter)
		, fTimeIntervalCall(_rhs.fTimeIntervalCall)
		, fTimeElapsed(0.0f)
		, bRunning(_rhs.bRunning)
	{
	}
	Entity::Entity(void)
		: handleTimer(Timer::handle(-1))
		, functionCallbackTimer(0)
		, fTimeIntervalCall(0.0f)
		, fTimeElapsed(0.0f)
		, bRunning(false)
	{
	}


	Timer::Manager& Manager::getInstance(void)
	{
		static Timer::Manager _instance;
		return _instance;
	}
	Manager::~Manager(void)
	{
	}
	Manager::Manager(void)
		: handleTimerMaximum(Timer::handle(0))
	{
	}

	void Manager::update(const float _fTime, const float _fTimeElapsed)
	{
		Timer::HandleVector& _vectorHandleTimerErase(this->vectorHandleTimerErase);
		_vectorHandleTimerErase.clear();

		Timer::EntityArray& _arrayTimerEntity(this->arrayTimerEntity);
		for ( Timer::EntityArrayIterator _iteratorTimerEntity(_arrayTimerEntity.begin()); _iteratorTimerEntity != _arrayTimerEntity.end(); ++_iteratorTimerEntity )
		{
			const Timer::handle _handleTimer(_iteratorTimerEntity->first);
			Timer::Entity& _entityTimer(_iteratorTimerEntity->second);
			if ( _entityTimer._update(_fTime, _fTimeElapsed) == false )
				_vectorHandleTimerErase.push_back(_handleTimer);
		}

		for ( Timer::HandleVectorConstIterator _iteratorHandleTimer(_vectorHandleTimerErase.begin()); _iteratorHandleTimer != _vectorHandleTimerErase.end(); ++_iteratorHandleTimer )
		{
			const Timer::handle _handleTimer(*_iteratorHandleTimer);
			Manager::eraseTimer(_handleTimer);
		}
	}

	const Timer::handle Manager::registTimer(const callbackTimerFunction& _functionCallbackTimer, const Timer::Parameter& _parameter, const float _fTimeIntervalCall)
	{
		Timer::HandleQueue& _queueHandle(this->queueHandleFree);
		Timer::handle _handleTimer;
		if ( _queueHandle.size() == 0 )
			_handleTimer = this->handleTimerMaximum++;
		else
		{
			_handleTimer = _queueHandle.front();
			_queueHandle.pop();
		}

		const unsigned int _indexTimer = unsigned int(_handleTimer);

		Timer::EntityArray& _arrayTimerEntity(this->arrayTimerEntity);
		_arrayTimerEntity.erase(_indexTimer);

		const Timer::Entity _entityTimerNew(_handleTimer, _functionCallbackTimer, _parameter, _fTimeIntervalCall);
		_arrayTimerEntity.insert(_entityTimerNew, _indexTimer);

		Timer::Entity& _entityTimer(_arrayTimerEntity[_indexTimer]);
		if ( _entityTimer._addTimer() == false )
		{
			Manager::eraseTimer(_handleTimer);
			return Timer::handle(-1);
		}

		return _handleTimer;
	}
	void Manager::suspendTimer(const Timer::handle _handleTimer)
	{
		const unsigned int _indexTimer = unsigned int(_handleTimer);

		Timer::EntityArray& _arrayTimerEntity(this->arrayTimerEntity);
		Timer::EntityArrayIterator _iteratorTimerEntity(_arrayTimerEntity.find(_indexTimer));
		if ( _iteratorTimerEntity == _arrayTimerEntity.end() )
			return;

		Timer::Entity& _entityTimer(_iteratorTimerEntity->second);
		if ( _entityTimer._suspendTimer() == false )
			Manager::eraseTimer(_handleTimer);
	}
	void Manager::resumeTimer(const Timer::handle _handleTimer)
	{
		const unsigned int _indexTimer = unsigned int(_handleTimer);

		Timer::EntityArray& _arrayTimerEntity(this->arrayTimerEntity);
		Timer::EntityArrayIterator _iteratorTimerEntity(_arrayTimerEntity.find(_indexTimer));
		if ( _iteratorTimerEntity == _arrayTimerEntity.end() )
			return;

		Timer::Entity& _entityTimer(_iteratorTimerEntity->second);
		if ( _entityTimer._resumeTimer() == false )
			Manager::eraseTimer(_handleTimer);
	}
	void Manager::eraseTimer(const Timer::handle _handleTimer)
	{
		const unsigned int _indexTimer = unsigned int(_handleTimer);

		Timer::EntityArray& _arrayTimerEntity(this->arrayTimerEntity);
		Timer::EntityArrayIterator _iteratorTimerEntity(_arrayTimerEntity.find(_indexTimer));
		if ( _iteratorTimerEntity == _arrayTimerEntity.end() )
			return;

		Timer::Entity& _entityTimer(_iteratorTimerEntity->second);
		_entityTimer._eraseTimer();

		_arrayTimerEntity.erase(_indexTimer);

		Timer::HandleQueue& _queueHandle(this->queueHandleFree);
		_queueHandle.push(_handleTimer);
	}
}