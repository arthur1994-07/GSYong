#pragma once

#include "./EventAgentItemDefine.h"

namespace GLEvent
{
	class ItemSet
	{
	public:
		inline const GLEvent::ItemSet& operator =(const GLEvent::ItemSet& _rhs);
		ItemSet(const GLEvent::ItemSet& _rhs);
		ItemSet(const SNATIVEID& _itemID, const float _fRate);
		ItemSet(void);

		const SNATIVEID itemID;
		const float fRate;
	};
	typedef std::vector<GLEvent::ItemSet> ItemSetVector;
	typedef ItemSetVector::const_iterator ItemSetVectorConstIterator;
	typedef ItemSetVector::iterator ItemSetVectorIterator;


	class ItemTable
	{
	public:
		inline const int getTimeInterval(void) const;		
		inline const unsigned int push_back(const GLEvent::ItemSet& _setItem);

		inline const std::string& getMessage(void) const;
		inline const GLEvent::ItemSetVector& getItemSetVector(void) const;

	public:
		inline const GLEvent::ItemTable& operator =(const GLEvent::ItemTable& _rhs);
		ItemTable(const GLEvent::ItemTable& _rhs);
		ItemTable(const std::string _stringMessage, const int _timeInterval);
		ItemTable(void);

	private:
		std::string stringMessage;
		int timeInterval;

		GLEvent::ItemSetVector vectorEventItemSet;
	};
	typedef std::vector<GLEvent::ItemTable> ItemTableVector;
	typedef ItemTableVector::const_iterator ItemTableVectorConstIterator;
	typedef ItemTableVector::iterator ItemTableVectorIterator;


	class Item : public GLEvent::Interface
	{
	public:
		virtual void vf_online(const unsigned int _dbNum) override;
		virtual void vf_offline(const unsigned int _dbNum) override;
		virtual void vf_frameMove(const float _fTime, const float _fTimeElapsed, const __time64_t& _timeCurrent) override;
		virtual const bool vf_messageProcedure(const unsigned int _clientID, const unsigned int _gaeaID, const NET_MSG_GENERIC* const _pMessage) override;

	private:
		const bool _messageProcedureItemInformationSet(const unsigned int _clientID, const unsigned int _gaeaID, const NET_MSG_GENERIC* const _pMessage);
		const bool _messageProcedureItemInformationDone(const unsigned int _clientID, const unsigned int _gaeaID, const NET_MSG_GENERIC* const _pMessage);

		const bool _callbackTimerEventItemFunction(const Timer::MESSAGE _typeMessage, Timer::Entity& _entity, const Timer::Parameter& _parameter);

	public:
		inline const GLEvent::Item& operator =(const GLEvent::Item& _rhs);		
		Item(GLAgentServer* const _pAgentServer, const std::string& _stringScriptEvent, const __time64_t _timeDuration, const __time64_t& _timeCurrent, const bool _bLoopBack, const bool _bShowInUI);
		Item(const GLEvent::Item& _rhs);
		Item(void);
		virtual ~Item(void);

	private:
		friend const bool __stdcall callbackTimerEventItemFunction(const Timer::MESSAGE, Timer::Entity&, const Timer::Parameter&);

		GLAgentServer* pAgentServer;
		bool bLoopBack;
		bool bShowInUI;

		GLEvent::ItemTableVector vectorEventItemTable;
	};


	const bool __stdcall callbackTimerEventItemFunction(const Timer::MESSAGE _typeMessage, Timer::Entity& _entity, const Timer::Parameter& _parameter);
}

#include "./EventAgentItem.hpp"

