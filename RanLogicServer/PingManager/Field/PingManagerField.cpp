#include "pch.h"

#include "./PingManagerField.h"

#include "../../../RanLogic/CountryCode/CountryCode.h"
#include "../../../RanLogic/Msg/GLContrlPingMsg.h"
#include "../../FieldServer/GLGaeaServer.h"

namespace Ping
{
	ManagerField::ManagerField(GLGaeaServer* const _pGaeaServer)
		: pGaeaServer(_pGaeaServer)
	{
	}
	ManagerField::~ManagerField(void)
	{
	}	
	const bool CALLBACK ManagerField::_messageProcedure(const DWORD _dwClientID, const DWORD _dwGaeaID, const NET_MSG_GENERIC* const _pMessage)
	{
		const GLMSG::NET_PING_MESSAGE* const _pMessagePing((const GLMSG::NET_PING_MESSAGE* const)_pMessage);

		const Ping::MESSAGE _typeMessage(_pMessagePing->typeMessage);
		switch ( _typeMessage )
		{
		case Ping::MESSAGE_REQUEST:
			return ManagerField::_messageProcedureRequest(_dwClientID, _dwGaeaID, _pMessage);
		case Ping::MESSAGE_REQUEST_INFORMATION_DETAIL:
			return ManagerField::_messageProcedureRequestInformationDetail(_dwClientID, _dwGaeaID, _pMessage);
		}
		return false;
	}
	const bool CALLBACK ManagerField::_messageProcedureRequestInformationDetail(const DWORD _dwClientID, const DWORD _dwGaeaID, const NET_MSG_GENERIC* const _pMessage)
	{
		const GLMSG::NET_PING_REQUEST_INFORMATION_DETAIL* const _pRequestInformationDetail((const GLMSG::NET_PING_REQUEST_INFORMATION_DETAIL* const)_pMessage);

		GLChar* const _pChar(this->pGaeaServer->GetChar(_dwGaeaID));
		if ( _pChar == 0 )
			return false;

		if ( _pChar->UserLevel() < USER_MASTER )
			return false;

		GLMSG::NET_PING_ANSWER_INFORMATION_DETAIL _messageAnswerInformationDetail;
		
		Ping::EntityMap& _mapPingEntity(this->mapPingEntity);
		for ( Ping::EntityMapIterator _iteratorPingEntity(_mapPingEntity.begin()); _iteratorPingEntity != _mapPingEntity.end(); ++_iteratorPingEntity )
		{
			Ping::Entity& _entityPing(_iteratorPingEntity->second);

			const Ping::Information& _informationPing(_entityPing.getPingInformation());

			if ( _messageAnswerInformationDetail.addEntity(_informationPing) == true )
				continue;

			this->pGaeaServer->SENDTOCLIENT(_dwClientID, &_messageAnswerInformationDetail);
			_messageAnswerInformationDetail.reset();
		}

		this->pGaeaServer->SENDTOCLIENT(_dwClientID, &_messageAnswerInformationDetail);

		return true;
	}
	const bool CALLBACK ManagerField::_messageProcedureRequest(const DWORD _dwClientID, const DWORD _dwGaeaID, const NET_MSG_GENERIC* const _pMessage)
	{
		const GLMSG::NET_PING_REQUEST* const _pMessageRequest((const GLMSG::NET_PING_REQUEST* const)_pMessage);

		const unsigned int _timeClient(_pMessageRequest->timeClient);
		const unsigned int _pingCurrent(_pMessageRequest->pingCurrent);

		GLMSG::NET_PING_ANSWER _messageAnswer(_timeClient);
		this->pGaeaServer->SENDTOCLIENT(_dwClientID, &_messageAnswer);

		if ( _pingCurrent == 0 )
			return true; // 접속 후 처음으로 보내는 ping의 경우 0이다;

		const std::string _stringAddressIP(this->pGaeaServer->GetClientIP(_dwClientID));

		const CountryCode::Manager& _managerCountryCode(CountryCode::Manager::getInstance());
		const unsigned int _codeCountry(_managerCountryCode.getCodeCountry(_stringAddressIP));

		Ping::EntityMapIterator _iteratorPingEntity(this->mapPingEntity.find(_codeCountry));
		if ( _iteratorPingEntity == this->mapPingEntity.end() )
		{
			this->mapPingEntity.insert(Ping::EntityValue(_codeCountry, Ping::Entity(_codeCountry)));
			_iteratorPingEntity = this->mapPingEntity.find(_codeCountry);
		}

		Ping::Entity& _entityPing(_iteratorPingEntity->second);
		_entityPing.addPing(_pingCurrent);

		return true;
	}
}
// 
// int main(void)
// {
// 	int a[]={26, 28, 30, 35, 35, 30,
// 		29, 39, 60, 31, 38, 36,
// 		33, 24, 34, 40, 29, 41, 
// 		40, 29, 30, 44, 32, 65, 
// 		35, 26, 38, 36, 37, 35};
// 	int n=sizeof(a)/sizeof(int), i;
// 
// 	double sum=0,mean,var;
// 
// 	for(i=0;i<n;i=i+1)
// 		sum+=a[i];
// 	mean=sum/n;
// 	sum=0;
// 	for(i=0;i<n;i=i+1)
// 		sum+=(a[i]-mean)*(a[i]-mean);
// 
// 	var=sum/(n-1);
// 
// 	printf("평균    =%8.3f\n", mean);
// 	printf("분산    =%8.3f\n", var);
// 	printf("표준편차=%8.3f\n", sqrt(var));
// 	return 0;
// }
