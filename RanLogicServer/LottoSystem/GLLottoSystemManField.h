#pragma once

#include "../../SigmaCore/Util/GLSingletone.h"

#include "../../RanLogic/LottoSystem/ILottoSystemMan.h"

#include "../../RanLogic/Msg/GLContrlLottoSystemMsg.h"

class GLGaeaServer;

namespace LottoSystem
{

	/*-------------------------------------------------------------------
	## 로또 시스템 관리 매니저 Field;
	1. GLGaeaServer 와 연동되는 Manager;

	2. GLGaeaServerLottoSystem.cpp 파일에 GLGaeaServer와 이어지는;
	모든 인터페이스 함수가 정의되어 있다;
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

		// Field 서버로 오는 패킷을 처리함;
		virtual bool OnMessage( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID );

	public:
		// Field와 클라이언트와의 통신;
		void SendToClientFromField( DWORD dwClientID, GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg );

		// Field와 Agent의 통신;
		void SendToAgentFromField( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg );

		// Field 브로드 캐스트;
		void BroadcastFromField( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg );

	public:
		// Msg Procedure;
		void SendReqLottoToBuyFA( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID );
		void SendConfirmBuyListFA( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID );
		void SendReqLottoToBuyFC( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID );
		void SendPostWinMoneyFC( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID );
		void SendWinManListFC( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID );

	public:
		// Field 서버;
		inline void SetGaeaServer( GLGaeaServer* pServer ) { m_pGaeaServer = pServer; }

	private:
		// Gaea Server;
		GLGaeaServer* m_pGaeaServer;
	};

}