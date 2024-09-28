#pragma once

#include "../../RanLogic/TexasHoldem/GLTexasHoldem.h"

class GLGaeaClient;

namespace TexasHoldem
{
	class GLTexasHoldemClient : public GLTexasHoldemMgr
	{
	public:
		GLTexasHoldemClient( GLGaeaClient* pGaeaClient );
		virtual ~GLTexasHoldemClient();

	public:
		virtual void FrameMove( float fElapsedTime ) override;
		virtual void MessageProcess( NET_MSG_GENERIC* pMessage, DWORD ClientID = 0, DWORD GaeaID = 0 ) override;

	public:
		void DisplayMsg( int nMsg );

	protected:
		void MsgChipMapVersion( NET_MSG_GENERIC* pMessage );
		void MsgChipMap( NET_MSG_GENERIC* pMessage );
		void MsgChipMapEnd( NET_MSG_GENERIC* pMessage );
		void MsgCheckRequirementFB( NET_MSG_GENERIC* pMessage );
		void MsgCloseMainMenu( NET_MSG_GENERIC* pMessage );
		void MsgChipUpdate( NET_MSG_GENERIC* pMessage );
		void MsgChipPurchaseFB( NET_MSG_GENERIC* pMessage );
		void MsgStartGameFB( NET_MSG_GENERIC* pMessage );
		void MsgEndGameFB( NET_MSG_GENERIC* pMessage );

		void RequestChipMap();
		int CheckNpc( DWORD dwNpcGaeaID );

	public:
		// 
		void OpenMainMenu( DWORD dwNpcGaeaID = NULL );
		void CloseMainMenu();
		void RequestChipPurchaseInGameMoney( LONGLONG Chip );
		void RequestChipPurchaseInPoint( LONGLONG Chip );
		void RequestInvenDrugCardGameCard(WORD PosX, WORD PosY);
		
		int StartGame();
		void EndGame();
		void ResetData();

		// °­Á¦ UI ´Ý±â;
		void Offline();

	protected:
		GLGaeaClient* m_pGaeaClient;
		float m_Timer;

	public:
		DWORD m_dwNpcGaeaID;

	};
}

