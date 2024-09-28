#include "pch.h"

#include "./PingManagerClient.h"
#include "../../RanLogic/Msg/GLContrlPingMsg.h"
#include "../GLGaeaClient.h"

namespace Ping
{
	ManagerClient::ManagerClient(GLGaeaClient* const _pGaeaClient)
		: pGaeaClient(_pGaeaClient)
		, fTimeElapsed(0.0f)
		, pingCurrent(0)
	{
	}
	ManagerClient::~ManagerClient(void)
	{
	}

	void ManagerClient::_frameMove(const float _fTime, const float _fTimeElapsed)
	{
		this->fTimeElapsed += _fTimeElapsed;
		if ( this->fTimeElapsed < INTERVAL_PING_CHECK )
			return;

		this->fTimeElapsed  = 0.0f;

		const unsigned int _timeCurrent(::clock());

		GLMSG::NET_PING_REQUEST _messageRequest(_timeCurrent, this->pingCurrent);
		this->pGaeaClient->NETSENDTOFIELD(&_messageRequest);
	}

	const bool CALLBACK ManagerClient::_messageProcedure(const NET_MSG_GENERIC* const _pMessage)
	{
		const GLMSG::NET_PING_MESSAGE* const _pMessagePing((const GLMSG::NET_PING_MESSAGE* const)_pMessage);

		const Ping::MESSAGE _typeMessage(_pMessagePing->typeMessage);
		switch ( _typeMessage )
		{
		case Ping::MESSAGE_ANSWER:
			return ManagerClient::_messageProcedureAnswer(_pMessage);
		case Ping::MESSAGE_ANSWER_INFORMATION_DETAIL:
			return ManagerClient::_messageProcedureAnswerInformationDetail(_pMessage);
		}
		return false;
	}

	const bool CALLBACK ManagerClient::_messageProcedureAnswerInformationDetail(const NET_MSG_GENERIC* const _pMessage)
	{
		const GLMSG::NET_PING_ANSWER_INFORMATION_DETAIL* const _pMessageAnswerInformationDetail((const GLMSG::NET_PING_ANSWER_INFORMATION_DETAIL* const)_pMessage);

		const unsigned int _sizeEntity(_pMessageAnswerInformationDetail->sizeEntity);
		
		const unsigned int _colorNormal(D3DCOLOR_XRGB(239, 228, 176));
		const unsigned int _colorTomato(D3DCOLOR_XRGB(239, 132, 84));

		this->pGaeaClient->PrintMsgText(_colorTomato, std::string("~~~~~~~~~~~~~~~~~~~~~~~~"));
		this->pGaeaClient->PrintMsgText(_colorNormal, sc::string::format("- my-ping-average : %1%ms;", this->pingCurrent));

		for ( unsigned int _index(_sizeEntity); _index--; )
		{
			const Ping::Information& _informationPing(_pMessageAnswerInformationDetail->informationPing[_index]);
			
			const unsigned int _codeCountry(_informationPing.codeCountry & 0xffffff00);
			const unsigned int _pingAverage(_informationPing.pingAverage);
			const unsigned int _pingHighest(_informationPing.pingHighest);
			const unsigned int _pingLowest(_informationPing.pingLowest);
			const unsigned int _pingDeviation(_informationPing.pingDeviation);

			this->pGaeaClient->PrintMsgText(_colorNormal, sc::string::format
				(
				"- [%1%] : average[%2%ms], high[%3%ms], low[%4%ms], deviation[%5%ms];"
				, (const char* const)&_codeCountry, _pingAverage, _pingHighest, _pingLowest, _pingDeviation
				));
		}
		this->pGaeaClient->PrintMsgText(_colorTomato, std::string("~~~~~~~~~~~~~~~~~~~~~~~~"));

		return true;
	}

	const bool CALLBACK ManagerClient::_messageProcedureAnswer(const NET_MSG_GENERIC* const _pMessage)
	{
		const GLMSG::NET_PING_ANSWER* const _pMessageAnswer((const GLMSG::NET_PING_ANSWER* const)_pMessage);

		const unsigned int _timeClient(_pMessageAnswer->timeClient);
		const unsigned int _timeCurrent(::clock());

		this->pingCurrent = _timeCurrent - _timeClient;

		return true;
	}
}