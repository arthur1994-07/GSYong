#pragma once

#include "./MoveSystemServerDefine.h"

class GLAgentServer;

namespace MoveSystem
{

	class CMoveManagerServer : public CSingletone< CMoveManagerServer >
	{
	public:
		/// Callback Function;

		// Field 에서 이동명령이 올 때 호출되는 Callback Function;
		// 즉, Field <-> Field 이동일 경우에 호출되는 콜백이다;

		// Instance 던전으로 이동할 때는 호출되지 않는다;
		// Field <-> Instance, Instance <-> Instance 관련 이동은 InstanceSystem 에서 MoveMap 을 호출한다;
		const bool	CALLBACK	OnOutServer ( GLMSG::SNETPC_REQ_MOVEMAP_FA* pMoveMsg, DWORD dwGaeaID );

		// 실제로 필드서버로 접속시킨다;
		void		CALLBACK	OnJoinServer ( GLMSG::SNETPC_FIELDSVR_OUT_FB* pOutMsg );

		// 같은 필드 내 이동의 경우에도 Agent 에서도 알도록 한다;
		void		CALLBACK	OnMoveMap_SameServer ( GLMSG::SNETPC_NOTICE_MOVEMAP_RESULT_AFB* pMoveMsg );

	public:
		/// Invoke Function;

		// 이동을 요청한다;
		const bool				MoveMap ( const SMOVE_INFO_AGENT& _sMoveInfo );

	public:
		inline void				SetServer ( GLAgentServer* _pServer ) { m_pServer = _pServer; }

	private:
		/// Inner Function;

		void					_SendToClient ( DWORD dwClientID, NET_MSG_GENERIC* pMsg );
		void					_SendToField ( DWORD dwClientID, NET_MSG_GENERIC* pMsg );
		void					_SendToField ( DWORD dwChannel, DWORD dwFieldSvrNum, NET_MSG_GENERIC* pMsg );
		void					_SendToAgent ( NET_MSG_GENERIC* pMsg );
		void					_SendToMyself ( NET_MSG_GENERIC* pMsg );

	private:
		// Agent Server;
		GLAgentServer*			m_pServer;

	private:
		CMoveManagerServer () : m_pServer ( NULL ) { }
		~CMoveManagerServer () { }

	protected:
		friend CSingletone;
	};

}