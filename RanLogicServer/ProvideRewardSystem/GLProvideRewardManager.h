#pragma once

#include "../../SigmaCore/Util/GLSingletone.h"

#include "./GLProvideRewardDefine.h"

class GLGaeaServer;

namespace ProvideRewardSystem
{

	// 포상 지급 관리자;
	class GLProvideRewardManager : public CSingletone< GLProvideRewardManager >
	{
	protected:
		friend CSingletone;

	private:
		GLProvideRewardManager ( );
		~GLProvideRewardManager ( );

	public:
		/// Callback Function;
		void CALLBACK		OnInitialize ();

		// 캐릭터 로그인 시 실행되는 콜백;
		// -> 캐릭터의 보상 목록을 부른다;
		void CALLBACK		OnLoginCharacter ( DWORD _dwChaDBNum );

		// 검색된 보상 목록을 Queue에 넣는다;
		void CALLBACK		OnRegisterProvideReward ( const PROVIDE_REWARD_DATA& _ProvideRewardData );

		// 큐에 등록된 보상목록을 하나씩 처리한다;
		void CALLBACK		OnFrameMove ( float fElapsedTime );

	public:
		/// Invoke Function;
		// 포상을 지급한다;
		void				ProvideReward ( const PROVIDE_REWARD_DATA& _ProvideRewardData );

	private:
		/// Inner Function;
		// 즉시 지급;
		void				_ProvideRewardImmediately ( const PROVIDE_REWARD_DATA& _ProvideRewardData );

		// 캐릭터 로그인 시 지급 ( table_ProvideReward 에 등록 );
		void				_ProvideRewardLoginTime ( const PROVIDE_REWARD_DATA& _ProvideRewardData );

	public:
		inline void			SetServer ( GLGaeaServer* _pServer ) { m_pServer = _pServer; }

	private:
		// Provide Reward Queue;
		QUEUE_PROVIDE_REWARD	m_queueProvideReward;

		// Server;
		GLGaeaServer*			m_pServer;

	private:
		static const DWORD		PROVIDE_REWARD_LIMIT_COUNT;
		static const float		PROVIDE_REWARD_CLEAR_TIME;
	};

}