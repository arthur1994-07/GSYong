#include "pch.h"

#include "./PingManager.h"

namespace Ping
{
	Information::Information(const unsigned int _codeCountry)
		: codeCountry(_codeCountry)
		, pingAverage(0)
		, pingHighest(0)
		, pingLowest(unsigned int(-1))
		, pingDeviation(0)
	{
	}
	Information::Information(const Ping::Information& _rhs)
		: codeCountry(_rhs.codeCountry)
		, pingAverage(_rhs.pingAverage)
		, pingHighest(_rhs.pingHighest)
		, pingLowest(_rhs.pingLowest)
		, pingDeviation(_rhs.pingDeviation)
	{
	}
	Information::Information(void)
		: codeCountry(0)
		, pingAverage(0)
		, pingHighest(0)
		, pingLowest(unsigned int(-1))
		, pingDeviation(0)
	{
	}





	Entity::Entity(void)
		: indexVectorPing(0)
	{
	}
	Entity::Entity(const unsigned int _codeCountry)
		: indexVectorPing(0)
	{
	}
	Entity::Entity(const Ping::Entity& _rhs)
		: information(_rhs.information)
		, vectorPing(_rhs.vectorPing)
		, indexVectorPing(_rhs.indexVectorPing)
	{
	}

	void Entity::addPing(const unsigned int _ping)
	{
		const unsigned int _pingCurrent(information.pingAverage);
		this->information.pingAverage = _pingCurrent == 0 ? _ping : ((_pingCurrent + _ping) >> 1);

		if ( _ping > this->information.pingHighest )
			this->information.pingHighest = _ping;

		if ( _ping < this->information.pingLowest )
			this->information.pingLowest = _ping;

		if ( this->vectorPing.size() < Ping::SIZE_SAMPLE_PING )
			this->vectorPing.push_back(_ping);
		else
		{
			this->vectorPing[this->indexVectorPing] = _ping;
			this->indexVectorPing = (this->indexVectorPing + 1) & (Ping::SIZE_SAMPLE_PING - 1);
		}
	}
	const Ping::Information& Entity::getPingInformation(void)
	{
		const int _pingAverage(int(this->information.pingAverage));

		Ping::PingVector& _vectorPing(this->vectorPing);

		const unsigned int _sizeSample(_vectorPing.size());
		if ( _sizeSample <= 1 )
			return this->information;

		unsigned int _sumSamplePing(0);		
		for ( unsigned int _index(_sizeSample); _index--; )
		{
			const int _pingSample((int)_vectorPing[_index]);
			const int _pingGap(_pingSample - _pingAverage);
			_sumSamplePing += (_pingGap * _pingGap);			
		}

		_sumSamplePing /= (_sizeSample - 1);

		this->information.pingDeviation = unsigned int(::sqrt(float(_sumSamplePing)));
		return this->information;
	}
}