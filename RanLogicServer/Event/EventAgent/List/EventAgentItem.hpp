#pragma once

namespace GLEvent
{
	inline const GLEvent::ItemSet& ItemSet::operator =(const GLEvent::ItemSet& _rhs)
	{
		this->itemID;
		this->fRate;
		::memcpy(this, &_rhs, sizeof(GLEvent::ItemSet));

		return *this;
	}



	inline const GLEvent::ItemTable& ItemTable::operator =(const GLEvent::ItemTable& _rhs)
	{
		this->stringMessage = _rhs.stringMessage;
		this->timeInterval = _rhs.timeInterval;
		this->vectorEventItemSet = _rhs.vectorEventItemSet;

		return *this;
	}
	inline const int ItemTable::getTimeInterval(void) const
	{
		return this->timeInterval;
	}
	inline const unsigned int ItemTable::push_back(const GLEvent::ItemSet& _setItem)
	{
		const unsigned int _indexItemSet(this->vectorEventItemSet.size());
		this->vectorEventItemSet.push_back(_setItem);

		return _indexItemSet;
	}
	inline const std::string& ItemTable::getMessage(void) const
	{
		return this->stringMessage;
	}
	inline const GLEvent::ItemSetVector& ItemTable::getItemSetVector(void) const
	{
		return this->vectorEventItemSet;
	}



	inline const GLEvent::Item& Item::operator =(const GLEvent::Item& _rhs)
	{
		Interface::operator =(_rhs);

		this->pAgentServer = _rhs.pAgentServer;
		this->bLoopBack = _rhs.bLoopBack;
		this->bShowInUI = _rhs.bShowInUI;

		this->vectorEventItemTable = _rhs.vectorEventItemTable;

		return *this;
	}
}