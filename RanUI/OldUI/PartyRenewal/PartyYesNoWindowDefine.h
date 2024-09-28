#pragma once

typedef boost::function<void(bool)> PARTY_YESNO_FUNCTION;
class PartyYesNoNode
{
public:
	PartyYesNoNode(const std::string& strDescription, PARTY_YESNO_FUNCTION Function)
		: m_strDescription(strDescription), m_Function(Function)
	{
	}
public:
	std::string m_strDescription;
	PARTY_YESNO_FUNCTION m_Function;
};