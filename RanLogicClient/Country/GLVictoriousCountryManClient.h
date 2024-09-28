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
		// �׳� �߰� ( �� Agent Message�� �����Ͽ����� ����Ͽ��� �Ѵ� );
		// �׷��� ������ ����ȭ�� ���� �ʰ� �� ���� �ִ�;
		void AddVCountry ( const SVICTORIOUS_COUNTRY& sVCountry );
		void ResetVCountry ( EMCOUNTRY_SYSTEM_FLAG emFlag );

	public:
		void Initialize ();

	public:
		/// Ư�� �ý����� ������ ������ �ҷ��´�;
		/// �������� �������� �� �� ������ ù��°���� �ҷ��´�;
		/// �������� �������� �� �� �ֱ⶧���� �Ŀ� �������� �θ� �� �ֵ���;
		/// �����Ǿ�� �Ѵ�;

		// ������ ID;
		const DWORD GetVCountryFirst ( EMCOUNTRY_SYSTEM_FLAG emFlag ) const;

		// ������ File Name;
		const std::string GetVCountryFileNameFirst ( EMCOUNTRY_SYSTEM_FLAG emFlag ) const;

		// ������ Image ID;
		const std::string GetVCountryImageIDFirst ( EMCOUNTRY_SYSTEM_FLAG emFlag ) const;

		// ������ Mini Image ID;
		const std::string GetVCountryMiniImageIDFirst ( EMCOUNTRY_SYSTEM_FLAG emFlag ) const;

		// ������ Name;
		const std::string GetVCountryNameFirst ( EMCOUNTRY_SYSTEM_FLAG emFlag ) const;

	public:
		/// Receive Message;
		// ���ο� ������ �߰� �޽����� ����� �޴´�;
		void MsgVCountryADD ( NET_MSG_GENERIC* pMsg );

		// Ư�� �ý����� ������ ���� ������ ����� �޴´�;
		void MsgVCountryReset ( NET_MSG_GENERIC* pMsg );

		// ����ȭ ������ �޴´�;
		void MsgSyncVictoriousCountry ( NET_MSG_GENERIC* pMsg );

	public:
		// Ŭ���̾�Ʈ ���� ����;
		inline void SetClient ( GLGaeaClient* pClient ) { m_pClient = pClient; }
		inline GLGaeaClient* GetClient () { return m_pClient; }

	private:
		// �¸� ���� ���;
		VCOUNTRY_VEC m_vecVCountry;

		// Client;
		GLGaeaClient* m_pClient;
	};

}