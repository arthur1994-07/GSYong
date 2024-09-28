#include "pch.h"

#include "./EventClient.h"

namespace GLEvent
{
	ManagerClient::ManagerClient(GLGaeaClient* const _pGaeaClient)
		: pGaeaClient(_pGaeaClient)
	{
	}
	ManagerClient::~ManagerClient(void)
	{
	}
	void ManagerClient::frameMove(const float _fTime, const float _fTimeElapsed)
	{
	}
	void ManagerClient::reset(void)
	{
	}
}