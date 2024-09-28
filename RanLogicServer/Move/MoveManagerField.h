#pragma once

#include "./MoveSystemServerDefine.h"

class GLGaeaServer;

namespace MoveSystem
{

	class CMoveManagerField : public CSingletone< CMoveManagerField >
	{
	public:
		/// Callback Function;

		// Queue �� ó���Ѵ�;
		void		CALLBACK			OnFrameMove ( const float _fElapsedTime );

		// Field Server �� ������ �� ����Ǵ� Callback Function;
		void		CALLBACK			OnJoinServer ( GLChar* const _pChar, GLLandMan* const _pLand );

		// Field Server �� Client �� ����ȭ�� �Ϸ�� �� ����Ǵ� Callback Function;
		const bool	CALLBACK			OnJoinClient ( const DWORD _dwChaDBNum, const MapID& _sPreviousMapID, InstanceSystem::EMAUTHORITY _emAuthority, EMGAME_JOINTYPE _emGameJoinType );

		// Move Map Callback Function;
		const bool	CALLBACK			OnMoveMap ( GLMSG::SNETPC_MOVEMAP_AF* pOutMsg );

		// Field Server �̵��� �����ϸ� ����Ǵ� Callback Function;
		void		CALLBACK			OnFailToMove ( GLMSG::SNETPC_NOTICE_MOVEMAP_RESULT_AFB* pStateMsg );

		// ���� ��� �̵����� ���μ����� �Ϸ�Ǹ� ����Ǵ� Callback Function;
		// Client ���� ������ �̵��� �Ϸ�Ǿ����� �˸���;
		const bool	CALLBACK			OnCompleteMove ( GLMSG::SNETREQ_LANDIN* pCompleteMsg );

	public:
		/// Invoke Function;

		// �̵��� �������� Ȯ���Ѵ�;
		const EMREQ_MOVEMAP_RESULT_AFB	IsUseMoveItem ( const DWORD _dwChaDBNum, const MapID _sMapID, SINVENITEM* const _pINVENITEM );
		const bool						IsValidCharacter ( const DWORD _dwChaDBNum );
		const EMREQ_MOVEMAP_RESULT_AFB	IsCanMoveMap ( const SMOVE_INFO& _sMoveInfo );

		// �̵��� ��û�Ѵ� ( Queue �� �ִ´� );
		const bool						MoveMap ( SMOVE_INFO& _sMoveInfo );
		const bool						MoveMap_UseItem ( SMOVE_INFO& _sMoveInfo, SINVENITEM* _pINVENITEM );
		const bool						Resurrection ( SMOVE_INFO& _sMoveInfo );

	public:
		inline void						SetServer ( GLGaeaServer* _pServer ) { m_pServer = _pServer; }

	private:
		/// Inner Function;

		// �̵� ������ �����Ѵ�;
		const bool						_RetryMoveMapProc ( SMOVE_INFO _sMoveInfo );
		const bool						_MoveMapProc ( const SMOVE_INFO& _sMoveInfo );

		// ���� Field Server �� �̵��� �����Ѵ�;
		const bool						_RequestMoveMap_ThisFieldServer ( const SMOVE_INFO& _sMoveInfo );

		// Land �� ����� ȣ��ȴ� ( Map �̵� �� ������ ȣ��ȴ� );
		void							_EnterLand ( GLChar* const _pChar, GLLandMan* const _pLand );

		void							_SendToClient ( DWORD dwClientID, NET_MSG_GENERIC* pMsg );
		void							_SendToAgent ( NET_MSG_GENERIC* pMsg );
		void							_SendToMyself ( NET_MSG_GENERIC* pMsg );

	private:
		// Move Map �Ǿ��� ���� ť;
		//QUEUE_MOVE_MAP				m_queueMoveMap;

		// ��õ� �ؾ� �� Move Map ���� �����̳�;
		VEC_MOVE_MAP					m_vecMoveMap;

		// Field Server;
		GLGaeaServer*					m_pServer;

	private:
		CMoveManagerField () : m_pServer ( NULL ) { }
		~CMoveManagerField () { }

	protected:
		friend CSingletone;
	};

}