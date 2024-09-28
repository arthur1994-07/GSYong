#pragma once

#include "../../SigmaCore/Util/GLSingletone.h"

#include "../../RanLogic/Msg/GLContrlCountry.h"

#include "./GLCountryManClient.h"

class GLGaeaClient;

namespace Country
{

	/************************************************************************/
	/* Client Victorious Country Manager                                     */
	/************************************************************************/
	class GLVictoriousCountryManClient : public CSingletone< GLVictoriousCountryManClient >
	{
	protected:
		friend CSingletone;

	private:
		GLVictoriousCountryManClient () { }
		~GLVictoriousCountryManClient () { }

	private:
		// 그냥 추가 ( 꼭 Agent Message에 반응하여서만 사용하여야 한다 );
		// 그렇지 않으면 동기화가 맞지 않게 될 수가 있다;
		void AddVCountry ( const SVICTORIOUS_COUNTRY& sVCountry );
		void ResetVCountry ( EMCOUNTRY_SYSTEM_FLAG emFlag );

	public:
		void Initialize ();

	public:
		/// 특정 시스템의 승전국 정보를 불러온다;
		/// 승전국은 여러나라가 될 수 있지만 첫번째나라만 불러온다;
		/// 승전국은 여러나라가 될 수 있기때문에 후에 여러나라를 부를 수 있도록;
		/// 수정되어야 한다;

		// 승전국 ID;
		const DWORD GetVCountryFirst ( EMCOUNTRY_SYSTEM_FLAG emFlag ) const;

		// 승전국 File Name;
		const std::string GetVCountryFileNameFirst ( EMCOUNTRY_SYSTEM_FLAG emFlag ) const;

		// 승전국 Image ID;
		const std::string GetVCountryImageIDFirst ( EMCOUNTRY_SYSTEM_FLAG emFlag ) const;

		// 승전국 Mini Image ID;
		const std::string GetVCountryMiniImageIDFirst ( EMCOUNTRY_SYSTEM_FLAG emFlag ) const;

		// 승전국 Name;
		const std::string GetVCountryNameFirst ( EMCOUNTRY_SYSTEM_FLAG emFlag ) const;

	public:
		/// Receive Message;
		// 새로운 승전국 추가 메시지의 결과를 받는다;
		void MsgVCountryADD ( NET_MSG_GENERIC* pMsg );

		// 특정 시스템의 승전국 정보 리셋의 결과를 받는다;
		void MsgVCountryReset ( NET_MSG_GENERIC* pMsg );

		// 동기화 정보를 받는다;
		void MsgSyncVictoriousCountry ( NET_MSG_GENERIC* pMsg );

	public:
		// 클라이언트 정보 세팅;
		inline void SetClient ( GLGaeaClient* pClient ) { m_pClient = pClient; }
		inline GLGaeaClient* GetClient () { return m_pClient; }

	private:
		// 승리 국가 목록;
		VCOUNTRY_VEC m_vecVCountry;

		// Client;
		GLGaeaClient* m_pClient;
	};

}