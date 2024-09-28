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
		// �׳� �߰� ( �� Agent Message�� �����Ͽ����� ����Ͽ��� �Ѵ� );
		// �׷��� ������ ����ȭ�� ���� �ʰ� �� ���� �ִ�;
		void AddVCountry ( const SVICTORIOUS_COUNTRY& sVCountry );
		void ResetVCountry ( EMCOUNTRY_SYSTEM_FLAG emFlag );

	public:
		// Ư�� �ý����� ������ ������ �ҷ��´�;
		// �������� �������� �� �� ������ ù��°���� �ҷ��´�;
		const DWORD GetVCountryFirst ( EMCOUNTRY_SYSTEM_FLAG emFlag ) const;

	public:
		/// Send Message;
		// ���ο� ������ �߰� �޽����� ������;
		void SendAddVCountryFA ( const SVICTORIOUS_COUNTRY& sVCountry, bool bTempVCountry = false );

		// Ư�� �ý����� ������ ������ �����Ѵ�;
		void SendResetVCountryFA ( EMCOUNTRY_SYSTEM_FLAG emFlag, bool bTempVCountry = false );


		/// Receive Message;
		// ���ο� ������ �߰� �޽����� ����� �޴´�;
		void MsgVCountryADD ( NET_MSG_GENERIC* pMsg );

		// Ư�� �ý����� ������ ���� ������ ����� �޴´�;
		void MsgVCountryReset ( NET_MSG_GENERIC* pMsg );

	public:
		// DB Load�� �Ϸ��Ѵ�;
		inline void CompleteLoading ( ) { m_bLoad = true; }

		// Agent ����;
		inline void SetServer ( GLGaeaServer* pServer ) { m_pServer = pServer; }

		inline GLGaeaServer* GetServer () { return m_pServer; }

	private:
		// �¸� ���� ���;
		VCOUNTRY_VEC m_vecVCountry;

		bool m_bLoad;

		// Gaea Server;
		GLGaeaServer* m_pServer;
	};

}