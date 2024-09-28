#pragma once

namespace Ping
{
	__forceinline const Ping::Information& Information::operator =(const Ping::Information& _rhs)
	{
		this->codeCountry = _rhs.codeCountry; // key;
		this->pingAverage = _rhs.pingAverage; // ping ���;
		this->pingHighest = _rhs.pingHighest; // ping �ְ���;
		this->pingLowest = _rhs.pingLowest; // ping ������;
		this->pingDeviation = _rhs.pingDeviation; // ping ����;

		return *this;
	}

	__forceinline const Ping::Entity& Entity::operator =(const Ping::Entity& _rhs)
	{
		this->information = _rhs.information; // ping ����;

		this->vectorPing = _rhs.vectorPing;
		this->indexVectorPing = _rhs.indexVectorPing;

		return *this;
	}	
}