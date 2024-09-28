#pragma once

#include "../../RanLogic/TexasHoldem/GLTexasHoldem.h"

class GLAgentServer;

namespace db
{
	class IDbManager;
}

namespace TexasHoldem
{
	class GLTexasHoldemAgent : public GLTexasHoldemMgr
	{
	public:
		GLTexasHoldemAgent( GLAgentServer* pServer );
		virtual ~GLTexasHoldemAgent();

	public:
		virtual void FrameMove( float fElapsedTime ) override;
		virtual void MessageProcess( NET_MSG_GENERIC* pMessage, DWORD ClientID = 0, DWORD GaeaID = 0 ) override;

	public:
		void LoadChipMapVersion();
		void LoadChipMap();

		// 서버가 처음 시작할때 한번 호출된다.
		void LoadChipMap( db::IDbManager* pAdo );

	protected:
		void MsgChipMapVersion( NET_MSG_GENERIC* pMessage, DWORD ClientID, DWORD GaeaID );
		void MsgChipMap( NET_MSG_GENERIC* pMessage, DWORD ClientID, DWORD GaeaID );
		void MsgChipMapEnd( NET_MSG_GENERIC* pMessage, DWORD ClientID, DWORD GaeaID );
		void MsgChipMapRequest( NET_MSG_GENERIC* pMessage, DWORD ClientID, DWORD GaeaID );

	protected:
		GLAgentServer* m_pServer;
		float m_Timer;

	};
}

