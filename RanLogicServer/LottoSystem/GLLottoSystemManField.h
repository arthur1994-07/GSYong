#pragma once

#include "../../SigmaCore/Util/GLSingletone.h"

#include "../../RanLogic/LottoSystem/ILottoSystemMan.h"

#include "../../RanLogic/Msg/GLContrlLottoSystemMsg.h"

class GLGaeaServer;

namespace LottoSystem
{

	/*-------------------------------------------------------------------
	## �ζ� �ý��� ���� �Ŵ��� Field;
	1. GLGaeaServer �� �����Ǵ� Manager;

	2. GLGaeaServerLottoSystem.cpp ���Ͽ� GLGaeaServer�� �̾�����;
	��� �������̽� �Լ��� ���ǵǾ� �ִ�;
	-------------------------------------------------------------------*/
	class GLLottoSystemManField : public ILottoSystemMan, public CSingletone<GLLottoSystemManField>
	{
	protected:
		friend CSingletone;

	public:
		GLLottoSystemManField();
		~GLLottoSystemManField();

	public:
		virtual void Initialize();
		virtual void Destroy();

		virtual bool Update( float fElapsedTime, const CTime& currentTime );

		// Field ������ ���� ��Ŷ�� ó����;
		virtual bool OnMessage( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID );

	public:
		// Field�� Ŭ���̾�Ʈ���� ���;
		void SendToClientFromField( DWORD dwClientID, GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg );

		// Field�� Agent�� ���;
		void SendToAgentFromField( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg );

		// Field ��ε� ĳ��Ʈ;
		void BroadcastFromField( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg );

	public:
		// Msg Procedure;
		void SendReqLottoToBuyFA( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID );
		void SendConfirmBuyListFA( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID );
		void SendReqLottoToBuyFC( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID );
		void SendPostWinMoneyFC( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID );
		void SendWinManListFC( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID );

	public:
		// Field ����;
		inline void SetGaeaServer( GLGaeaServer* pServer ) { m_pGaeaServer = pServer; }

	private:
		// Gaea Server;
		GLGaeaServer* m_pGaeaServer;
	};

}