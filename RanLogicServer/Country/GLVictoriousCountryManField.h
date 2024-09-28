#pragma once

#include "../../SigmaCore/Util/GLSingletone.h"
#include "./GLCountryManServer.h"

class GLGaeaServer;

namespace Country
{

	/************************************************************************/
	/* Field Victorious Country Manager                                     */
	/************************************************************************/
	class GLVictoriousCountryManField : public CSingletone< GLVictoriousCountryManField >
	{
	protected:
		friend CSingletone;

	private:
		GLVictoriousCountryManField () : m_bLoad ( false ) { }
		~GLVictoriousCountryManField () { }

	public:
		// DBAction;
		void LoadDBAction ();

	public:
		// 그냥 추가 ( 꼭 Agent Message에 반응하여서만 사용하여야 한다 );
		// 그렇지 않으면 동기화가 맞지 않게 될 수가 있다;
		void AddVCountry ( const SVICTORIOUS_COUNTRY& sVCountry );
		void ResetVCountry ( EMCOUNTRY_SYSTEM_FLAG emFlag );

	public:
		// 특정 시스템의 승전국 정보를 불러온다;
		// 승전국은 여러나라가 될 수 있지만 첫번째나라만 불러온다;
		const DWORD GetVCountryFirst ( EMCOUNTRY_SYSTEM_FLAG emFlag ) const;

	public:
		/// Send Message;
		// 새로운 승전국 추가 메시지를 보낸다;
		void SendAddVCountryFA ( const SVICTORIOUS_COUNTRY& sVCountry, bool bTempVCountry = false );

		// 특정 시스템의 승전국 정보를 리셋한다;
		void SendResetVCountryFA ( EMCOUNTRY_SYSTEM_FLAG emFlag, bool bTempVCountry = false );


		/// Receive Message;
		// 새로운 승전국 추가 메시지의 결과를 받는다;
		void MsgVCountryADD ( NET_MSG_GENERIC* pMsg );

		// 특정 시스템의 승전국 정보 리셋의 결과를 받는다;
		void MsgVCountryReset ( NET_MSG_GENERIC* pMsg );

	public:
		// DB Load를 완료한다;
		inline void CompleteLoading ( ) { m_bLoad = true; }

		// Agent 서버;
		inline void SetServer ( GLGaeaServer* pServer ) { m_pServer = pServer; }

		inline GLGaeaServer* GetServer () { return m_pServer; }

	private:
		// 승리 국가 목록;
		VCOUNTRY_VEC m_vecVCountry;

		bool m_bLoad;

		// Gaea Server;
		GLGaeaServer* m_pServer;
	};

}