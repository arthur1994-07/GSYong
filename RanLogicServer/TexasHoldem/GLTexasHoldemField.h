#pragma once

#include "../../RanLogic/TexasHoldem/GLTexasHoldem.h"

class GLChar;
class GLGaeaServer;

namespace db
{
	class IDbManager;
}

namespace TexasHoldem
{
	class GLTexasHoldemField : public GLTexasHoldemMgr
	{
	public:
		GLTexasHoldemField( GLGaeaServer* pServer );
		virtual ~GLTexasHoldemField();

	public:
		virtual void FrameMove( float fElapsedTime ) override;
		virtual void MessageProcess( NET_MSG_GENERIC* pMessage, DWORD ClientID = 0, DWORD GaeaID = 0 ) override;

	public:
		void SendChipMap( DWORD ClientID, bool bFirst = false );

	protected:
		void MsgChipMapVersion( NET_MSG_GENERIC* pMessage, DWORD ClientID, DWORD GaeaID );
		void MsgChipMap( NET_MSG_GENERIC* pMessage, DWORD ClientID, DWORD GaeaID );
		void MsgChipMapEnd( NET_MSG_GENERIC* pMessage, DWORD ClientID, DWORD GaeaID );
		void MsgUseCardGameCardCF( NET_MSG_GENERIC* pMessage, DWORD ClientID, DWORD GaeaID );
		void MsgOpenMainMenuCF( NET_MSG_GENERIC* pMessage, DWORD ClientID, DWORD GaeaID );
		void MsgCloseMainMenuCF( NET_MSG_GENERIC* pMessage, DWORD ClientID, DWORD GaeaID );
		void MsgChipMapRequest( NET_MSG_GENERIC* pMessage, DWORD ClientID, DWORD GaeaID );
		void MsgChipPurchaseInGameMoney( NET_MSG_GENERIC* pMessage, DWORD ClientID, DWORD GaeaID );
		void MsgChipPurchaseInPoint( NET_MSG_GENERIC* pMessage, DWORD ClientID, DWORD GaeaID );
		void MsgChipMapRequestInner( NET_MSG_GENERIC* pMessage, DWORD ClientID, DWORD GaeaID );		
		void MsgStartGame( NET_MSG_GENERIC* pMessage, DWORD ClientID, DWORD GaeaID );
		void MsgEndGame( NET_MSG_GENERIC* pMessage, DWORD ClientID, DWORD GaeaID );
		void MsgChipInner( NET_MSG_GENERIC* pMessage, DWORD ClientID, DWORD GaeaID );
		void MsgAuthKeyInner( NET_MSG_GENERIC* pMessage, DWORD ClientID, DWORD GaeaID );

	protected:
		void SaveChip( DWORD dwChaDbNum, LONGLONG& chip, bool bPlus = false );
		void SaveChipPurchaseLogGameMoney( DWORD dwChaDbNum, LONGLONG& chip, LONGLONG& GameMoney );
		void SaveChipPurchaseLogPoint( DWORD dwChaDbNum, LONGLONG& chip, LONGLONG& Point );
		int CheckNpc( GLChar* pCharacter, DWORD dwNpcGaeaID );
		int CheckInvenItem( GLChar* pCharacter, DWORD wPosX, DWORD wPosY );
		void LoadChip( DWORD dwChaDbNum, bool bEndGameNotify = false );
		void SaveAuthKey( DWORD dwChaDbNum, UUID& sGUID );
		void SaveGameTime( DWORD dwChaDbNum, DWORD dwUserNum, int nSecond );

	protected:
		GLGaeaServer* m_pServer;
		GLTexasHoldemStartGameDelayMgr m_TexasHoldemStartGameDelayMgr;
		GLTexasHoldemGameTime m_TexasHoldemGameTimeMgr;

	};
}

