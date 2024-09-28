#pragma once

#include "./MoveSystemServerDefine.h"

class GLGaeaServer;

namespace MoveSystem
{

	class CMoveManagerField : public CSingletone< CMoveManagerField >
	{
	public:
		/// Callback Function;

		// Queue 를 처리한다;
		void		CALLBACK			OnFrameMove ( const float _fElapsedTime );

		// Field Server 에 입장할 때 실행되는 Callback Function;
		void		CALLBACK			OnJoinServer ( GLChar* const _pChar, GLLandMan* const _pLand );

		// Field Server 와 Client 의 동기화가 완료될 때 실행되는 Callback Function;
		const bool	CALLBACK			OnJoinClient ( const DWORD _dwChaDBNum, const MapID& _sPreviousMapID, InstanceSystem::EMAUTHORITY _emAuthority, EMGAME_JOINTYPE _emGameJoinType );

		// Move Map Callback Function;
		const bool	CALLBACK			OnMoveMap ( GLMSG::SNETPC_MOVEMAP_AF* pOutMsg );

		// Field Server 이동을 실패하면 실행되는 Callback Function;
		void		CALLBACK			OnFailToMove ( GLMSG::SNETPC_NOTICE_MOVEMAP_RESULT_AFB* pStateMsg );

		// 실제 모든 이동관련 프로세스가 완료되면 실행되는 Callback Function;
		// Client 에서 서버에 이동이 완료되었음을 알린다;
		const bool	CALLBACK			OnCompleteMove ( GLMSG::SNETREQ_LANDIN* pCompleteMsg );

	public:
		/// Invoke Function;

		// 이동이 가능한지 확인한다;
		const EMREQ_MOVEMAP_RESULT_AFB	IsUseMoveItem ( const DWORD _dwChaDBNum, const MapID _sMapID, SINVENITEM* const _pINVENITEM );
		const bool						IsValidCharacter ( const DWORD _dwChaDBNum );
		const EMREQ_MOVEMAP_RESULT_AFB	IsCanMoveMap ( const SMOVE_INFO& _sMoveInfo );

		// 이동을 요청한다 ( Queue 에 넣는다 );
		const bool						MoveMap ( SMOVE_INFO& _sMoveInfo );
		const bool						MoveMap_UseItem ( SMOVE_INFO& _sMoveInfo, SINVENITEM* _pINVENITEM );
		const bool						Resurrection ( SMOVE_INFO& _sMoveInfo );

	public:
		inline void						SetServer ( GLGaeaServer* _pServer ) { m_pServer = _pServer; }

	private:
		/// Inner Function;

		// 이동 로직을 실행한다;
		const bool						_RetryMoveMapProc ( SMOVE_INFO _sMoveInfo );
		const bool						_MoveMapProc ( const SMOVE_INFO& _sMoveInfo );

		// 같은 Field Server 로 이동시 수행한다;
		const bool						_RequestMoveMap_ThisFieldServer ( const SMOVE_INFO& _sMoveInfo );

		// Land 에 입장시 호출된다 ( Map 이동 시 무조건 호출된다 );
		void							_EnterLand ( GLChar* const _pChar, GLLandMan* const _pLand );

		void							_SendToClient ( DWORD dwClientID, NET_MSG_GENERIC* pMsg );
		void							_SendToAgent ( NET_MSG_GENERIC* pMsg );
		void							_SendToMyself ( NET_MSG_GENERIC* pMsg );

	private:
		// Move Map 되어질 정보 큐;
		//QUEUE_MOVE_MAP				m_queueMoveMap;

		// 재시도 해야 할 Move Map 정보 컨테이너;
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