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
		// Ư�� �ý����� ������ ������ �ҷ��´�;
		// �������� �������� �� �� ������ ù��°���� �ҷ��´�;
		const DWORD GetVCountryFirst ( EMCOUNTRY_SYSTEM_FLAG emFlag ) const;

	public:
		/// Receive Message;
		// ���ο� ������ �߰� �޽����� �޴´�;
		void MsgVCountryADDFAC ( NET_MSG_GENERIC* pMsg );

		// Ư�� �ý����� ������ ���� �޽����� �޴´�;
		void MsgVCountryResetFAC ( NET_MSG_GENERIC* pMsg );


		// DB���� ������ �߰��� ����� �Ǿ��ٴ� �޽����� �޴´�;
		void MsgVCountryADDDA ( NET_MSG_GENERIC* pMsg );

		// DB���� ������ ������ ����� �Ǿ��ٴ� �޽����� �޴´�;
		void MsgVCountryResetDA ( NET_MSG_GENERIC* pMsg );


		// �ٸ� Agent�κ��� ������ �߰��� ����� �Ǿ��ٴ� �޽����� �޴´�;
		void MsgVCountryADDAA ( NET_MSG_GENERIC* pMsg );

		// �ٸ� Agent�κ��� ������ ������ ����� �Ǿ��ٴ� �޽����� �޴´�;
		void MsgVCountryResetAA ( NET_MSG_GENERIC* pMsg );

	public:
		// Client���� ����ȭ;
		void SyncState ( DWORD dwClientID );

	public:
		// DB Load�� �Ϸ��Ѵ�;
		inline void CompleteLoading ( ) { m_bLoad = true; }

		// Agent ����;
		inline void SetServer ( GLAgentServer* pServer ) { m_pServer = pServer; }

		inline GLAgentServer* GetServer () { return m_pServer; }

	private:
		// �¸� ���� ���;
		VCOUNTRY_VEC m_vecVCountry;

		bool m_bLoad;

		// Agent Server;
		GLAgentServer* m_pServer;

	private:
		static const float CONFIRM_VCOUNTRY_TIME;
	};

}