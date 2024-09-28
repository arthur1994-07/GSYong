#pragma once

#include "./MoveSystemServerDefine.h"

class GLAgentServer;

namespace MoveSystem
{

	class CMoveManagerServer : public CSingletone< CMoveManagerServer >
	{
	public:
		/// Callback Function;

		// Field ���� �̵������ �� �� ȣ��Ǵ� Callback Function;
		// ��, Field <-> Field �̵��� ��쿡 ȣ��Ǵ� �ݹ��̴�;

		// Instance �������� �̵��� ���� ȣ����� �ʴ´�;
		// Field <-> Instance, Instance <-> Instance ���� �̵��� InstanceSystem ���� MoveMap �� ȣ���Ѵ�;
		const bool	CALLBACK	OnOutServer ( GLMSG::SNETPC_REQ_MOVEMAP_FA* pMoveMsg, DWORD dwGaeaID );

		// ������ �ʵ弭���� ���ӽ�Ų��;
		void		CALLBACK	OnJoinServer ( GLMSG::SNETPC_FIELDSVR_OUT_FB* pOutMsg );

		// ���� �ʵ� �� �̵��� ��쿡�� Agent ������ �˵��� �Ѵ�;
		void		CALLBACK	OnMoveMap_SameServer ( GLMSG::SNETPC_NOTICE_MOVEMAP_RESULT_AFB* pMoveMsg );

	public:
		/// Invoke Function;

		// �̵��� ��û�Ѵ�;
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