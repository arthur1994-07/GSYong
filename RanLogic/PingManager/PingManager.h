#pragma once

#include "./PingManagerDefine.h"

namespace Ping
{
	class Information
	{
	public:
		__forceinline const Ping::Information& operator =(const Ping::Information& _rhs);
		Information(const unsigned int _codeCountry);
		Information(const Ping::Information& _rhs);
		Information(void);

	public:
		unsigned int codeCountry; // key;
		unsigned int pingAverage; // ping 평균;
		unsigned int pingHighest; // ping 최고점;
		unsigned int pingLowest; // ping 최저점;
		unsigned int pingDeviation; // ping 편차;
	};
	class Entity
	{
	public:
		void addPing(const unsigned int _ping);
		const Ping::Information& getPingInformation(void);

	public:
		__forceinline const Ping::Entity& operator =(const Ping::Entity& _rhs);
		Entity(void);
		Entity(const unsigned int _codeCountry);
		Entity(const Ping::Entity& _rhs);

	private:		
		Ping::Information information;
		Ping::PingVector vectorPing;
		unsigned int indexVectorPing;
	};

	typedef std::map<unsigned int, Ping::Entity> EntityMap;
	typedef EntityMap::const_iterator EntityMapConstIterator;
	typedef EntityMap::iterator EntityMapIterator;
	typedef EntityMap::value_type EntityValue;	
}

#include "./PingManager.hpp"
