#include "pch.h"

#include "../GLAgentServer.h"
#include "../../RanLogic/Msg/ServerMsg.h"
#include "./StateFieldServer.h"

namespace StateFieldServer
{
	Manager::Manager(GLAgentServer* const _pAgentServer)
		: m_pAgentServer(_pAgentServer)
	{
		for ( unsigned int _i(MAX_CHANNEL_NUMBER); _i--; )
		{
			for ( unsigned int _j(FIELDSERVER_MAX); _j--; )
				m_nUsedMemory[_i][_j] = 0;
		}
	}
	Manager::~Manager(void)
	{
	}

	const SFIELDSERVER_INFO Manager::RequestFreeServerInfo ()
	{
		SFIELDSERVER_INFO sFieldInfo;

		int nUsedLowest = 1024 * 1024 * 2; // 단위 : KB, 2GB;

		const VEC_FIELDSERVER_INFO& vecCreatableInstanceField = m_pAgentServer->GetVector_CreatableInstanceField();
		for ( VEC_FIELDSERVER_INFO_CITER citer = vecCreatableInstanceField.begin();
			citer != vecCreatableInstanceField.end(); ++citer )
		{
			SFIELDSERVER_INFO sTempCreatableInstanceField = *citer;

			const int* const pArrayChannel_UsedMemory = m_nUsedMemory[ sTempCreatableInstanceField.nChannelNum ];
			const int nUsedMemory = pArrayChannel_UsedMemory[ sTempCreatableInstanceField.nFieldNum ];
			if ( nUsedMemory < nUsedLowest )
			{
				nUsedLowest = nUsedMemory;

				if ( m_pAgentServer->IsFieldOnline(
					sTempCreatableInstanceField.nChannelNum,
					sTempCreatableInstanceField.nFieldNum ) )
				{
					sFieldInfo = sTempCreatableInstanceField;
				}
			}
		}

		return sFieldInfo;
	}

	const unsigned int Manager::requestFreeServerID(const unsigned int _nFieldChannel)
	{
		int nFieldServer = -1;

		int nUsedLowest = 1024 * 1024 * 2; // 단위 : KB, 2GB;

		const VEC_FIELDSERVER_INFO& vecCreatableInstanceField = m_pAgentServer->GetVector_CreatableInstanceField();
		for ( VEC_FIELDSERVER_INFO_CITER citer = vecCreatableInstanceField.begin();
			citer != vecCreatableInstanceField.end(); ++citer )
		{
			SFIELDSERVER_INFO sTempCreatableInstanceField = *citer;

			// 채널이 다르면 continue;
			if ( sTempCreatableInstanceField.nChannelNum != _nFieldChannel )
				continue;

			const int* const pArrayChannel_UsedMemory = m_nUsedMemory[ sTempCreatableInstanceField.nChannelNum ];
			const int nUsedMemory = pArrayChannel_UsedMemory[ sTempCreatableInstanceField.nFieldNum ];
			if ( nUsedMemory < nUsedLowest )
			{
				nUsedLowest = nUsedMemory;

				if ( m_pAgentServer->IsFieldOnline(
					sTempCreatableInstanceField.nChannelNum,
					sTempCreatableInstanceField.nFieldNum ) )
				{
					nFieldServer = sTempCreatableInstanceField.nFieldNum;
				}
			}
		}

		return nFieldServer;
	}


	const bool Manager::messageProcedure(const NET_MSG_GENERIC* const _pMessage)
	{
		const GLMSG::NET_FIELD_STATE* const _pStateMessage((const GLMSG::NET_FIELD_STATE* const)_pMessage);

		switch ( _pStateMessage->nSubType )
		{
		case GLMSG::NET_MSG_FIELD_STATE_MEMORY:
			return Manager::_messageProcedure_memory(_pMessage);
		case GLMSG::NET_MSG_FIELD_STATE_MEMORY_PER_LAND:
			return Manager::_messageProcedure_memoryPerLand(_pMessage);
		}
		return true;
	}

	const bool Manager::_messageProcedure_memoryPerLand(const NET_MSG_GENERIC* const _pMessage)
	{ // 여러 필드 서버에서 중복 된 정보가 오므로 감안하여 처리해야 한다;
		const GLMSG::NET_FIELD_STATE_MEMORY_PER_LAND* const _pMemoryPerLandMessage((const GLMSG::NET_FIELD_STATE_MEMORY_PER_LAND* const)_pMessage);		

		const unsigned int _nSize(_pMemoryPerLandMessage->nLand);
		for ( unsigned int _i(_nSize); _i--; )
		{
			const unsigned int _nMainMapID(_pMemoryPerLandMessage->memoryUsedPerLand[_i].nMainMapID);
			const unsigned int _nUsedMemory(_pMemoryPerLandMessage->memoryUsedPerLand[_i].nUsedMemory);

			UsedMemoryPerLandMapIterator _iteratorLand(m_mapUsedMemoryPerLand.find(_nMainMapID));
			if ( _iteratorLand == m_mapUsedMemoryPerLand.end() )
				m_mapUsedMemoryPerLand.insert(UsedMemoryPerLandValue(_nMainMapID, _nUsedMemory));
			else
			{
				int& _nPreviousUsedMemory(_iteratorLand->second);
				_nPreviousUsedMemory = (_nPreviousUsedMemory + _nUsedMemory) >> 1;
			}
		}

		return true;
	}

	const bool Manager::_messageProcedure_memory(const NET_MSG_GENERIC* const _pMessage)
	{
		const GLMSG::NET_FIELD_STATE_MEMORY* const _pMemoryMessage((const GLMSG::NET_FIELD_STATE_MEMORY* const)_pMessage);

		const unsigned int _nChannel(_pMemoryMessage->nFieldChannel);
		const unsigned int _nServerID(_pMemoryMessage->nFieldServer);
		const int _nUsedMemory(_pMemoryMessage->nUsedMemory);

		m_nUsedMemory[_nChannel][_nServerID] += _nUsedMemory;
		if ( m_nUsedMemory[_nChannel][_nServerID] < 0 )
			m_nUsedMemory[_nChannel][_nServerID] = 0;

		return true;
	}	
}