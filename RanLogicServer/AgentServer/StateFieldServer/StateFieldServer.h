#pragma once

#include "./StateFieldServerDefine.h"


namespace StateFieldServer
{
	class Manager
	{
	public:
		const bool messageProcedure(const NET_MSG_GENERIC* const _pMessage);

		// 메모리에 여유가 있는 서버의 ID를 요청한다;
		const SFIELDSERVER_INFO RequestFreeServerInfo ();
		const unsigned int requestFreeServerID(const unsigned int _nFieldChannel);


	private:
		const bool _messageProcedure_memory(const NET_MSG_GENERIC* const _pMessage);
		const bool _messageProcedure_memoryPerLand(const NET_MSG_GENERIC* const _pMessage);

	public:
		const Manager& operator =(const Manager&){}
		Manager(GLAgentServer* const _pAgentServer);
		~Manager(void);

	private:
		GLAgentServer* const m_pAgentServer;
		int m_nUsedMemory[MAX_CHANNEL_NUMBER][FIELDSERVER_MAX];
		UsedMemoryPerLandMap m_mapUsedMemoryPerLand;
	};
}