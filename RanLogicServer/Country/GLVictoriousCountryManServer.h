#pragma once

#include "../../SigmaCore/Util/GLSingletone.h"
#include "./GLCountryManServer.h"

class GLAgentServer;

namespace Country
{

	/************************************************************************/
	/* Server Victorious Country Manager                                    */
	/************************************************************************/
	class GLVictoriousCountryManServer : public CSingletone< GLVictoriousCountryManServer >
	{
	protected:
		friend CSingletone;

	private:
		GLVictoriousCountryManServer () : m_bLoad ( false ) { }
		~GLVictoriousCountryManServer () { }

	public:
		void CALLBACK OnFrameMove ( float fElapsedTime );

	public:
		// DBAction;
		void LoadDBAction ();
		void AddDBAction ( const SVICTORIOUS_COUNTRY& sVCountry );
		void ResetDBAction ( EMCOUNTRY_SYSTEM_FLAG emFlag );

	public:
		void AddVCountry ( const SVICTORIOUS_COUNTRY& sVCountry );
		void ResetVCountry ( EMCOUNTRY_SYSTEM_FLAG emFlag );

	public:
		// 특정 시스템의 승전국 정보를 불러온다;
		// 승전국은 여러나라가 될 수 있지만 첫번째나라만 불러온다;
		const DWORD GetVCountryFirst ( EMCOUNTRY_SYSTEM_FLAG emFlag ) const;

	public:
		/// Receive Message;
		// 새로운 승전국 추가 메시지를 받는다;
		void MsgVCountryADDFAC ( NET_MSG_GENERIC* pMsg );

		// 특정 시스템의 승전국 리셋 메시지를 받는다;
		void MsgVCountryResetFAC ( NET_MSG_GENERIC* pMsg );


		// DB에서 승전국 추가가 제대로 되었다는 메시지를 받는다;
		void MsgVCountryADDDA ( NET_MSG_GENERIC* pMsg );

		// DB에서 승전국 리셋이 제대로 되었다는 메시지를 받는다;
		void MsgVCountryResetDA ( NET_MSG_GENERIC* pMsg );


		// 다른 Agent로부터 승전국 추가가 제대로 되었다는 메시지를 받는다;
		void MsgVCountryADDAA ( NET_MSG_GENERIC* pMsg );

		// 다른 Agent로부터 승전국 리셋이 제대로 되었다는 메시지를 받는다;
		void MsgVCountryResetAA ( NET_MSG_GENERIC* pMsg );

	public:
		// Client와의 동기화;
		void SyncState ( DWORD dwClientID );

	public:
		// DB Load를 완료한다;
		inline void CompleteLoading ( ) { m_bLoad = true; }

		// Agent 서버;
		inline void SetServer ( GLAgentServer* pServer ) { m_pServer = pServer; }

		inline GLAgentServer* GetServer () { return m_pServer; }

	private:
		// 승리 국가 목록;
		VCOUNTRY_VEC m_vecVCountry;

		bool m_bLoad;

		// Agent Server;
		GLAgentServer* m_pServer;

	private:
		static const float CONFIRM_VCOUNTRY_TIME;
	};

}