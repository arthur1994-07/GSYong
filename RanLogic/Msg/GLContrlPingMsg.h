#pragma once

#include "./GLContrlBaseMsg.h"
#include "../PingManager/PingManager.h"

namespace Ping
{
	enum MESSAGE
	{
		MESSAGE_REQUEST,
		MESSAGE_ANSWER,
		MESSAGE_REQUEST_INFORMATION_DETAIL,
		MESSAGE_ANSWER_INFORMATION_DETAIL,
		MESSAGE_NSIZE,
	};
}

namespace GLMSG
{
	struct NET_PING_MESSAGE : public NET_MSG_GENERIC
	{
		const Ping::MESSAGE typeMessage;

		NET_PING_MESSAGE(const DWORD _sizeMessage, const Ping::MESSAGE _typeMessage)
			: NET_MSG_GENERIC(NET_MSG_GCTRL_PING, _sizeMessage)
			, typeMessage(_typeMessage)
		{
		}
	};

	struct NET_PING_REQUEST : public GLMSG::NET_PING_MESSAGE
	{
		const unsigned int timeClient;
		const unsigned int pingCurrent;

		NET_PING_REQUEST(const unsigned int _timeClient, const unsigned int _pingCurrent)
			: GLMSG::NET_PING_MESSAGE(sizeof(GLMSG::NET_PING_REQUEST), Ping::MESSAGE_REQUEST)
			, timeClient(_timeClient)
			, pingCurrent(_pingCurrent)
		{
			MIN_STATIC_ASSERT(sizeof(GLMSG::NET_PING_REQUEST) <= NET_DATA_BUFSIZE);
		}
	};

	struct NET_PING_ANSWER : public GLMSG::NET_PING_MESSAGE
	{
		const unsigned int timeClient;

		NET_PING_ANSWER(const unsigned int _timeClient)
			: GLMSG::NET_PING_MESSAGE(sizeof(GLMSG::NET_PING_ANSWER), Ping::MESSAGE_ANSWER)
			, timeClient(_timeClient)
		{
			MIN_STATIC_ASSERT(sizeof(GLMSG::NET_PING_ANSWER) <= NET_DATA_BUFSIZE);
		}
	};

	struct NET_PING_REQUEST_INFORMATION_DETAIL : public GLMSG::NET_PING_MESSAGE
	{
		NET_PING_REQUEST_INFORMATION_DETAIL(void)
			: GLMSG::NET_PING_MESSAGE(sizeof(GLMSG::NET_PING_REQUEST_INFORMATION_DETAIL), Ping::MESSAGE_REQUEST_INFORMATION_DETAIL)
		{
			MIN_STATIC_ASSERT(sizeof(GLMSG::NET_PING_REQUEST_INFORMATION_DETAIL) <= NET_DATA_BUFSIZE);
		}
	};

	struct NET_PING_ANSWER_INFORMATION_DETAIL : public GLMSG::NET_PING_MESSAGE
	{
		enum
		{
			ARRAY_NSIZE = (NET_DATA_BUFSIZE - sizeof(GLMSG::NET_PING_MESSAGE) - sizeof(unsigned int)) / sizeof(Ping::Information),
		};
		unsigned int sizeEntity;
		Ping::Information informationPing[ARRAY_NSIZE];
		
		__forceinline void reset(void)
		{
			this->dwSize = sizeof(GLMSG::NET_PING_MESSAGE) + sizeof(unsigned int);
			this->sizeEntity = 0;
		}
		__forceinline const bool isValid(void) const
		{
			return this->sizeEntity != 0;
		}
		__forceinline const bool addEntity(const Ping::Information& _rhs)
		{
			this->informationPing[this->sizeEntity++] = _rhs;
			this->dwSize += sizeof(Ping::Information);

			if ( this->sizeEntity >= ARRAY_NSIZE )
				return false;
			return true;
		}

		NET_PING_ANSWER_INFORMATION_DETAIL(void)
			: GLMSG::NET_PING_MESSAGE(sizeof(GLMSG::NET_PING_MESSAGE) + sizeof(unsigned int), Ping::MESSAGE_ANSWER_INFORMATION_DETAIL)
			, sizeEntity(0)
		{
			MIN_STATIC_ASSERT(sizeof(GLMSG::NET_PING_ANSWER_INFORMATION_DETAIL) <= NET_DATA_BUFSIZE);
		}
	};
}

