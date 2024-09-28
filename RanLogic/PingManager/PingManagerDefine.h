#pragma once

#include <map>
#include <vector>

#define INTERVAL_PING_CHECK 30.0f // ping  �˻� �ֱ�(��);

struct NET_MSG_GENERIC;
namespace Ping
{
	enum
	{
		SIZE_BIT_SAMPLE_PING = 6, // ǥ�������� ���ϱ� ���� ���ø� ����;
		SIZE_SAMPLE_PING = (1 << Ping::SIZE_BIT_SAMPLE_PING),
	};
	typedef std::vector<unsigned int> PingVector;
}