#pragma once

namespace Timer
{
	__forceinline const Timer::handle Entity::_getHandleTimer(void) const
	{
		return this->handleTimer;
	}
	__forceinline const bool Entity::_update(const float _fTime, const float _fTimeElapsed)
	{
		if ( this->bRunning == false )
			return true;

		this->fTimeElapsed += _fTimeElapsed;
		if ( this->fTimeElapsed < this->fTimeIntervalCall )
			return true;
		this->fTimeElapsed = 0.0f;

		return Entity::_callFunction(Timer::MESSAGE_UPDATE);
	}
	__forceinline const bool Entity::_addTimer(void)
	{
		return Entity::_callFunction(Timer::MESSAGE_ADD);
	}
	__forceinline const bool Entity::_suspendTimer(void)
	{
		this->bRunning = false;
		return Entity::_callFunction(Timer::MESSAGE_SUSPEND);
	}
	__forceinline const bool Entity::_resumeTimer(void)
	{
		this->bRunning = true;
		return Entity::_callFunction(Timer::MESSAGE_RESUME);
	}
	__forceinline const bool Entity::_eraseTimer(void)
	{
		return Entity::_callFunction(Timer::MESSAGE_ERASE);
	}
	__forceinline const bool Entity::_callFunction(const Timer::MESSAGE _typeMessage)
	{
		return (*this->functionCallbackTimer)(_typeMessage, *this, this->parameter);
	}
}