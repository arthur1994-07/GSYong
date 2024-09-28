#pragma once

namespace Ping
{
	__forceinline const Ping::Information& Information::operator =(const Ping::Information& _rhs)
	{
		this->codeCountry = _rhs.codeCountry; // key;
		this->pingAverage = _rhs.pingAverage; // ping 평균;
		this->pingHighest = _rhs.pingHighest; // ping 최고점;
		this->pingLowest = _rhs.pingLowest; // ping 최저점;
		this->pingDeviation = _rhs.pingDeviation; // ping 편차;

		return *this;
	}

	__forceinline const Ping::Entity& Entity::operator =(const Ping::Entity& _rhs)
	{
		this->information = _rhs.information; // ping 정보;

		this->vectorPing = _rhs.vectorPing;
		this->indexVectorPing = _rhs.indexVectorPing;

		return *this;
	}	
}