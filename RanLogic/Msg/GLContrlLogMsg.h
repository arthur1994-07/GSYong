#pragma once

#include <string>
#include <vector>

#include "GLContrlBaseMsg.h"

#include "../Money/MoneyLogDefine.h"

namespace GLMSG
{

	struct NET_LOG_MESSAGE_HEADER : public NET_MSG_GENERIC
	{
		DWORD m_dwChannelNum;
		DWORD m_dwFieldNum;

		NET_LOG_MESSAGE_HEADER ( const DWORD _nSize
			, const DWORD _dwChannelNum
			, const DWORD _dwFieldNum
			, const EMNET_MSG _emMsg )
			: m_dwChannelNum ( _dwChannelNum )
			, m_dwFieldNum ( _dwFieldNum )
		{
			nType = _emMsg;
			dwSize = _nSize;
		}
	};


	/// Field -> Cache;

	// Income Money Log;
	struct NET_LOG_MESSAGE_SYNC_CHANGEMONEY_LOG_FH : public NET_LOG_MESSAGE_HEADER
	{
		const EMGAME_MONEY_CHANGE m_emMoneyType;
		const int m_nMapID;
		const LONGLONG m_llChangeMoney;

		NET_LOG_MESSAGE_SYNC_CHANGEMONEY_LOG_FH ( const DWORD _dwChannelNum
			, const DWORD _dwFieldNum
			, const EMGAME_MONEY_CHANGE _emMoneyType
			, const int _nMapID
			, const LONGLONG _llChangeMoney )
			: NET_LOG_MESSAGE_HEADER( sizeof( NET_LOG_MESSAGE_SYNC_CHANGEMONEY_LOG_FH )
			, _dwChannelNum
			, _dwFieldNum
			, NET_MSG_GCTRL_LOG_CHANGEMONEY_FH )
			, m_emMoneyType ( _emMoneyType )
			, m_nMapID ( _nMapID )
			, m_llChangeMoney ( _llChangeMoney )
		{
		}
	};

}