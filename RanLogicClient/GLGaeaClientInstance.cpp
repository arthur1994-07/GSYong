#include "pch.h"

#include "../SigmaCore/String/StringFormat.h"
#include "../enginelib/GUInterface/UITextControl.h"
#include "../RanLogic/InstanceSystem/GLInstanceSystem.h"
#include "InterfaceBase.h"
#include "GLGaeaClient.h"

void GLGaeaClient::clearInstanceInformation(void)
{
	m_instanceInformationMap.clear();
}
void GLGaeaClient::addInstanceInformation(const InstanceSystem::InstanceInformationClient& _instanceInformation)
{
	m_instanceInformationMap.insert(InstanceSystem::InstanceInformationValue(_instanceInformation.keyMapID, _instanceInformation));
}

void GLGaeaClient::showInstanceType(void)
{
	const unsigned int _colorGuideLine(D3DCOLOR_XRGB(239, 132, 84));
	const unsigned int _colorNormal(D3DCOLOR_XRGB(239, 228, 176));
	PrintMsgText(_colorGuideLine, std::string("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"));
	for ( InstanceSystem::InstanceInformationMapCIter _iteratorInformation(m_instanceInformationMap.begin()); _iteratorInformation != m_instanceInformationMap.end(); ++_iteratorInformation )
	{
		const InstanceSystem::InstanceInformationClient& _information = _iteratorInformation->second;

		PrintMsgText(_colorNormal,
            sc::string::format("name : %1%(%2%/%3%) [%4%]", _information.instanceName, _information.keyMapID.wMainID, _information.keyMapID.wSubID, (_information.bOpen ? "Open" : "Close") ));
	}
}

const InstanceSystem::InstanceInformationClient* const GLGaeaClient::getInstanceInformation(const SNATIVEID& _keyMapID) const
{
	InstanceSystem::InstanceInformationMapCIter _iteratorInformation(m_instanceInformationMap.find(_keyMapID));
	if ( _iteratorInformation == m_instanceInformationMap.end() )
		return NULL;

	const InstanceSystem::InstanceInformationClient& _information = _iteratorInformation->second;
	return &_information;
}
