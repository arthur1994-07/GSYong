#pragma once

#include "./StateFieldServerDefine.h"


namespace StateFieldServer
{
	class Manager
	{
	public:
		const bool messageProcedure(const NET_MSG_GENERIC* const _pMessage);

		// �޸𸮿� ������ �ִ� ������ ID�� ��û�Ѵ�;
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