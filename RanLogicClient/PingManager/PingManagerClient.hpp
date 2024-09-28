#pragma once

namespace Ping
{
	__forceinline const unsigned int ManagerClient::getPing(void) const
	{
		return this->pingCurrent;
	}
}