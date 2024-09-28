#pragma once

#include "../Event/EventDefine.h"
#include "./GLContrlBaseMsg.h"

namespace GLEvent
{
	enum MESSAGE
	{
		MESSAGE_LIST,
		MESSAGE_LIST_FB,
		MESSAGE_END,
		MESSAGE_END_FB,
		MESSAGE_ITEM_BEGIN,
		MESSAGE_ITEM_INFORMATION_BASIC,
		MESSAGE_ITEM_INFORMATION_SET,
		MESSAGE_ITEM_INFORMATION_DONE,
		MESSAGE_ITEM_TRANSFER_MESSAGE,
		MESSAGE_ITEM_TRANSFER_ITEM,
		MESSAGE_ITEM_ONLINE,

		MESSAGE_NSIZE,
	};
}

namespace GLMSG
{
	struct NET_EVENT_MESSAGE : public NET_MSG_GENERIC
	{
		const GLEvent::MESSAGE typeMessage;
		char cNameFileScript[GLEvent::LENGTH_NAME];

		NET_EVENT_MESSAGE(const unsigned int _sizeMessage, const GLEvent::MESSAGE _typeMessage, const char* const _cNameFileScript = 0)
			: NET_MSG_GENERIC(NET_MSG_GCTRL_EVENT, _sizeMessage)
			, typeMessage(_typeMessage)
		{
			if ( _cNameFileScript == 0 )
				return;

			::StringCchCopy(this->cNameFileScript, GLEvent::LENGTH_NAME, _cNameFileScript);
			MIN_STATIC_ASSERT(sizeof(GLMSG::NET_EVENT_MESSAGE) <= NET_DATA_BUFSIZE);
		}
	};

	////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////	
	struct NET_EVENT_LIST : public GLMSG::NET_EVENT_MESSAGE
	{
		NET_EVENT_LIST(void)
			: GLMSG::NET_EVENT_MESSAGE(sizeof(GLMSG::NET_EVENT_LIST), GLEvent::MESSAGE_LIST)
		{
			MIN_STATIC_ASSERT(sizeof(GLMSG::NET_EVENT_LIST) <= NET_DATA_BUFSIZE);
		}
	};
	struct NET_EVENT_LIST_FB : public GLMSG::NET_EVENT_MESSAGE
	{
		enum
		{
			LENGTH = 32,
			NSIZE = ((NET_DATA_BUFSIZE - sizeof(GLMSG::NET_EVENT_MESSAGE) - sizeof(unsigned int)) / LENGTH),
			HEADER_NSIZE = sizeof(GLMSG::NET_EVENT_MESSAGE) + sizeof(unsigned int),
		};
		unsigned int sizeList;
		char cNameFileScript[NSIZE][LENGTH];

		const bool addItem(const std::string& _stringScriptEvent)
		{
			::StringCchCopy(this->cNameFileScript[this->sizeList], LENGTH, _stringScriptEvent.c_str());
			this->sizeList += 1;
			this->dwSize += LENGTH;

			if ( this->sizeList == NSIZE )
				return false;
			return true;
		}
		const bool isValid(void) const
		{
			return this->sizeList != 0;
		}
		void reset(void)
		{
			this->sizeList = 0;
			this->dwSize = HEADER_NSIZE;
		}

		NET_EVENT_LIST_FB(void)
			: GLMSG::NET_EVENT_MESSAGE(sizeof(GLMSG::NET_EVENT_LIST_FB), GLEvent::MESSAGE_LIST_FB)
			, sizeList(0)
		{
			this->dwSize = HEADER_NSIZE;
			MIN_STATIC_ASSERT(sizeof(GLMSG::NET_EVENT_LIST_FB) <= NET_DATA_BUFSIZE);
		}
	};	
	struct NET_EVENT_END : public GLMSG::NET_EVENT_MESSAGE
	{
		char cNameFileScript[GLEvent::LENGTH_NAME];

		NET_EVENT_END(const std::string& _stringNameFileScript)
			: GLMSG::NET_EVENT_MESSAGE(sizeof(GLMSG::NET_EVENT_END), GLEvent::MESSAGE_END)
		{
			::StringCchCopy(this->cNameFileScript, 128, _stringNameFileScript.c_str());
			MIN_STATIC_ASSERT(sizeof(GLMSG::NET_EVENT_END) <= NET_DATA_BUFSIZE);
		}
	};
	struct NET_EVENT_END_FB : public GLMSG::NET_EVENT_MESSAGE
	{
		NET_EVENT_END_FB(void)
			: GLMSG::NET_EVENT_MESSAGE(sizeof(GLMSG::NET_EVENT_END_FB), GLEvent::MESSAGE_END)
		{
			MIN_STATIC_ASSERT(sizeof(GLMSG::NET_EVENT_END_FB) <= NET_DATA_BUFSIZE);
		}
	};

	struct NET_EVENT_BEGIN_ITEM : public GLMSG::NET_EVENT_MESSAGE
	{
		NET_EVENT_BEGIN_ITEM(const char* const _cNameFileScript)
			: GLMSG::NET_EVENT_MESSAGE(sizeof(GLMSG::NET_EVENT_BEGIN_ITEM), GLEvent::MESSAGE_ITEM_BEGIN, _cNameFileScript)
		{
			MIN_STATIC_ASSERT(sizeof(GLMSG::NET_EVENT_BEGIN_ITEM) <= NET_DATA_BUFSIZE);
		}
	};
	struct NET_EVENT_ITEM_INFORMATION_BASIC : public GLMSG::NET_EVENT_MESSAGE
	{
		const __time64_t timeDuration;
		const bool bLoopBack;
		const bool bShowInUI;

		NET_EVENT_ITEM_INFORMATION_BASIC(const char* const _cNameFileScript, const __time64_t _timeDuration, const bool _bLoopBack, const bool _bShowInUI)
			: GLMSG::NET_EVENT_MESSAGE(sizeof(GLMSG::NET_EVENT_ITEM_INFORMATION_BASIC), GLEvent::MESSAGE_ITEM_INFORMATION_BASIC, _cNameFileScript)
			, timeDuration(_timeDuration)
			, bLoopBack(_bLoopBack)
			, bShowInUI(_bShowInUI)
		{
			MIN_STATIC_ASSERT(sizeof(GLMSG::NET_EVENT_BEGIN_ITEM) <= NET_DATA_BUFSIZE);
		}
	};
	struct NET_EVENT_ITEM_INFORMATION_SET : public GLMSG::NET_EVENT_MESSAGE
	{
		struct Entity
		{
			SNATIVEID itemID;
			float fRate;
		};
		enum
		{
			NSIZE = ((NET_DATA_BUFSIZE - sizeof(GLMSG::NET_EVENT_MESSAGE) - sizeof(char) * GLEvent::LENGTH_NAME - sizeof(unsigned int) - sizeof(int)) / sizeof(Entity)),
			HEADER_NSIZE = sizeof(GLMSG::NET_EVENT_MESSAGE) + sizeof(char) * GLEvent::LENGTH_NAME + sizeof(unsigned int) + sizeof(int),
		};
		char cMessageOut[GLEvent::LENGTH_NAME];
		const int timeInterval;
		unsigned int sizeItem;
		Entity entityItem[NSIZE];

		const bool addItem(const unsigned int _itemIDMain, const unsigned int _itemIDSub, const float _fRate)
		{
			this->entityItem[this->sizeItem].itemID = SNATIVEID(_itemIDMain, _itemIDSub);
			this->entityItem[this->sizeItem].fRate = _fRate;

			this->sizeItem += 1;
			this->dwSize += sizeof(Entity);

			if ( this->sizeItem == NSIZE )
				return false;
			return true;
		}
		const bool isValid(void) const
		{
			return this->sizeItem != 0;
		}
		void reset(void)
		{
			this->sizeItem = 0;
			this->dwSize = HEADER_NSIZE;
		}

		NET_EVENT_ITEM_INFORMATION_SET(const char* const _cNameFileScript, const std::string& _stringMessageOut, const int _timeInterval)
			: GLMSG::NET_EVENT_MESSAGE(sizeof(GLMSG::NET_EVENT_ITEM_INFORMATION_SET), GLEvent::MESSAGE_ITEM_INFORMATION_SET, _cNameFileScript)
			, sizeItem(0)
			, timeInterval(_timeInterval)
		{
			this->dwSize = HEADER_NSIZE;
			::StringCchCopy(this->cMessageOut, GLEvent::LENGTH_NAME, _stringMessageOut.c_str());

			MIN_STATIC_ASSERT(sizeof(GLMSG::NET_EVENT_BEGIN_ITEM) <= NET_DATA_BUFSIZE);
		}
	};
	struct NET_EVENT_ITEM_INFORMATION_DONE : public GLMSG::NET_EVENT_MESSAGE
	{
		const bool bComplete;

		NET_EVENT_ITEM_INFORMATION_DONE(const char* const _cNameFileScript, const bool _bComplete)
			: GLMSG::NET_EVENT_MESSAGE(sizeof(GLMSG::NET_EVENT_ITEM_INFORMATION_DONE), GLEvent::MESSAGE_ITEM_INFORMATION_DONE, _cNameFileScript)
			, bComplete(_bComplete)
		{
			MIN_STATIC_ASSERT(sizeof(GLMSG::NET_EVENT_ITEM_INFORMATION_DONE) <= NET_DATA_BUFSIZE);
		}
	};
	struct NET_EVENT_ITEM_TRANSFER_MESSAGE : public GLMSG::NET_EVENT_MESSAGE
	{
		char cMessageOut[GLEvent::LENGTH_NAME];

		NET_EVENT_ITEM_TRANSFER_MESSAGE(const std::string& _stringMessageOut)
			: GLMSG::NET_EVENT_MESSAGE(sizeof(GLMSG::NET_EVENT_ITEM_TRANSFER_MESSAGE), GLEvent::MESSAGE_ITEM_TRANSFER_MESSAGE)
		{
			::StringCchCopy(this->cMessageOut, GLEvent::LENGTH_NAME, _stringMessageOut.c_str());
			MIN_STATIC_ASSERT(sizeof(GLMSG::NET_EVENT_ITEM_TRANSFER_MESSAGE) <= NET_DATA_BUFSIZE);
		}
	};
	struct NET_EVENT_ITEM_TRANSFER_ITEM : public GLMSG::NET_EVENT_MESSAGE
	{
		enum
		{
			NSIZE = 32,
		};		
		unsigned int dbNum;
		unsigned int sizeItem;
		SNATIVEID itemID[NSIZE];

		const bool addItem(const SNATIVEID& _itemID)
		{
			if ( this->sizeItem == NSIZE )
				return false;

			this->itemID[this->sizeItem] = _itemID;
			this->sizeItem += 1;
			
			return true;
		}
		const bool isValid(void) const
		{
			return this->sizeItem != 0;
		}
		void reset(void)
		{
			this->sizeItem = 0;
		}

		NET_EVENT_ITEM_TRANSFER_ITEM(const unsigned int _dbNum)
			: GLMSG::NET_EVENT_MESSAGE(sizeof(GLMSG::NET_EVENT_ITEM_TRANSFER_ITEM), GLEvent::MESSAGE_ITEM_TRANSFER_ITEM)
			, sizeItem(0)
			, dbNum(_dbNum)
		{
			MIN_STATIC_ASSERT(sizeof(GLMSG::NET_EVENT_ITEM_TRANSFER_ITEM) <= NET_DATA_BUFSIZE);
		}
	};
}
