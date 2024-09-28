#pragma once

#include <map>
#include <vector>

#define INTERVAL_PING_CHECK 30.0f // ping  검사 주기(초);

struct NET_MSG_GENERIC;
namespace Ping
{
	enum
	{
		SIZE_BIT_SAMPLE_PING = 6, // 표준편차를 구하기 위한 샘플링 수량;
		SIZE_SAMPLE_PING = (1 << Ping::SIZE_BIT_SAMPLE_PING),
	};
	typedef std::vector<unsigned int> PingVector;
}