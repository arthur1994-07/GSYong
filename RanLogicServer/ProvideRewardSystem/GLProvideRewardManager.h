#pragma once

#include "../../SigmaCore/Util/GLSingletone.h"

#include "./GLProvideRewardDefine.h"

class GLGaeaServer;

namespace ProvideRewardSystem
{

	// ���� ���� ������;
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

		// ĳ���� �α��� �� ����Ǵ� �ݹ�;
		// -> ĳ������ ���� ����� �θ���;
		void CALLBACK		OnLoginCharacter ( DWORD _dwChaDBNum );

		// �˻��� ���� ����� Queue�� �ִ´�;
		void CALLBACK		OnRegisterProvideReward ( const PROVIDE_REWARD_DATA& _ProvideRewardData );

		// ť�� ��ϵ� �������� �ϳ��� ó���Ѵ�;
		void CALLBACK		OnFrameMove ( float fElapsedTime );

	public:
		/// Invoke Function;
		// ������ �����Ѵ�;
		void				ProvideReward ( const PROVIDE_REWARD_DATA& _ProvideRewardData );

	private:
		/// Inner Function;
		// ��� ����;
		void				_ProvideRewardImmediately ( const PROVIDE_REWARD_DATA& _ProvideRewardData );

		// ĳ���� �α��� �� ���� ( table_ProvideReward �� ��� );
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