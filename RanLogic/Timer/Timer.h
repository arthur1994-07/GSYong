#pragma once

#include "./TimerDefine.h"

namespace Timer
{
	class Parameter
	{
	public:
		Parameter(void);
		Parameter(const Timer::Parameter& _rhs);

	public:
		union
		{
			struct
			{
				int valueInteger0;
				int valueInteger1;
				int valueInteger2;
				int valueInteger3;
			};
			struct
			{
				float valueFloat0;
				float valueFloat1;
				float valueFloat2;
				float valueFloat3;
			};
			struct
			{
				void* valuePointer0;
				void* valuePointer1;
				void* valuePointer2;
				void* valuePointer3;
			};
		};
	};
	class Entity
	{
	private:
		__forceinline const Timer::handle _getHandleTimer(void) const;
		__forceinline const bool _update(const float _fTime, const float _fTimeElapsed);
		__forceinline const bool _addTimer(void);
		__forceinline const bool _suspendTimer(void);
		__forceinline const bool _resumeTimer(void);
		__forceinline const bool _eraseTimer(void);

	private:
		__forceinline const bool _callFunction(const Timer::MESSAGE _typeMessage);

	public:
		Entity(const Timer::Entity& _rhs);
		Entity(void);

	private:
		__forceinline const Timer::Entity& operator =(const Timer::Entity& _rhs){}
		Entity(const Timer::handle _handleTimer, const callbackTimerFunction& _functionCallbackTimer, const Timer::Parameter& _parameter, const float _fTimeIntervalCall);

	public:
		friend class Manager;

		Timer::handle handleTimer;
		const callbackTimerFunction functionCallbackTimer;
		Timer::Parameter parameter;
		float fTimeIntervalCall;
		float fTimeElapsed;
		bool bRunning;
	};
	typedef QTL::arrayList<Timer::Entity> EntityArray;
	typedef EntityArray::const_iterator EntityArrayConstIterator;
	typedef EntityArray::iterator EntityArrayIterator;

	class Manager
	{
	public:
		void update(const float _fTime, const float _fTimeElapsed);

		const Timer::handle registTimer(const callbackTimerFunction& _functionCallbackTimer, const Timer::Parameter& _parameter, const float _fTimeIntervalCall);
		void suspendTimer(const Timer::handle _handleTimer);
		void resumeTimer(const Timer::handle _handleTimer);
		void eraseTimer(const Timer::handle _handleTimer);

	private:

	public:
		static Timer::Manager& getInstance(void);
		~Manager(void);

	private:
		__forceinline const Timer::Manager& operator =(const Timer::Manager&){}
		Manager(void);

	private:
		Timer::EntityArray arrayTimerEntity;
		Timer::HandleVector vectorHandleTimerErase;
		Timer::HandleQueue queueHandleFree;
		Timer::handle handleTimerMaximum;
	};
}

#include "./Timer.hpp"