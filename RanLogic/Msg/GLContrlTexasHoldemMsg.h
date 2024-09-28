#pragma once

#include "../TexasHoldem/GLTexasHoldem.h"

#include "GLContrlBaseMsg.h"

namespace TexasHoldem
{
	enum eMessageType
	{
		ChipMapVersion, 
		ChipMap, 
		ChipMapEnd, 
		UseCardGameCardCF,
		OpenMainMenuCF, 
		CloseMainMenuCF, 
		CloseMainMenuFC, 
		CheckRequirementFB, 
		ChipUpdate, 
		ChipMapRequest, 
		ChipPurchaseInGameMoney, 
		ChipPurchaseInPoint, 
		ChipPurchaseFB, 
		StartGame, 
		EndGame, 
		ChipMapRequestInner, 
		StartGameFB, 
		ChipInner, 
		EndGameFB, 
		AuthKeyInner, 
	};
}

namespace GLMSG
{
	// 텍사스홀덤 기본 메시지
	struct NET_TEXAS_HOLD_EM_MESSAGE : public NET_MSG_GENERIC
	{
		TexasHoldem::eMessageType eType;

		NET_TEXAS_HOLD_EM_MESSAGE( const DWORD nSize, TexasHoldem::eMessageType eMessage )
			: NET_MSG_GENERIC( NET_MSG_GCTRL_TEXAS_HOLD_EM, nSize )
			, eType( eMessage )
		{
		}
	};

	struct NET_TEXAS_HOLD_EM_CHIP_VERSION : public NET_TEXAS_HOLD_EM_MESSAGE
	{
		LONGLONG Version;

		NET_TEXAS_HOLD_EM_CHIP_VERSION( LONGLONG Version )
			: NET_TEXAS_HOLD_EM_MESSAGE( sizeof( NET_TEXAS_HOLD_EM_CHIP_VERSION ), TexasHoldem::ChipMapVersion )
		{
			this->Version = Version;
			MIN_STATIC_ASSERT( sizeof( NET_TEXAS_HOLD_EM_CHIP_VERSION ) <= NET_DATA_BUFSIZE );
		}
	};

	struct NET_TEXAS_HOLD_EM_CHIP_MAP : public NET_TEXAS_HOLD_EM_MESSAGE
	{
		TexasHoldem::sChipPrice sChip;

		NET_TEXAS_HOLD_EM_CHIP_MAP( TexasHoldem::sChipPrice sChip )
			: NET_TEXAS_HOLD_EM_MESSAGE( sizeof( NET_TEXAS_HOLD_EM_CHIP_MAP ), TexasHoldem::ChipMap )
		{
			this->sChip = sChip;
			MIN_STATIC_ASSERT( sizeof( NET_TEXAS_HOLD_EM_CHIP_MAP ) <= NET_DATA_BUFSIZE );
		}
	};

	struct NET_TEXAS_HOLD_EM_CHIP_MAP_END : public NET_TEXAS_HOLD_EM_MESSAGE
	{
		// 필드서버에 처음 접속할 경우 true, 버전이 틀려서 다시 받는 경우 false
		// 클라이언트에서만 이 값이 필요하다.
		bool bFirst;

		NET_TEXAS_HOLD_EM_CHIP_MAP_END()
			: NET_TEXAS_HOLD_EM_MESSAGE( sizeof( NET_TEXAS_HOLD_EM_CHIP_MAP_END ), TexasHoldem::ChipMapEnd )
			, bFirst( false )
		{
			MIN_STATIC_ASSERT( sizeof( NET_TEXAS_HOLD_EM_CHIP_MAP_END ) <= NET_DATA_BUFSIZE );
		}
	};

	struct NET_TEXAS_HOLD_EM_USE_CARDGAME_CARD : public NET_TEXAS_HOLD_EM_MESSAGE
	{
		WORD m_PosX; //! 인벤토리 위치 X
		WORD m_PosY; //! 인벤토리 위치 Y

		NET_TEXAS_HOLD_EM_USE_CARDGAME_CARD(WORD PosX, WORD PosY)
			: NET_TEXAS_HOLD_EM_MESSAGE( sizeof( NET_TEXAS_HOLD_EM_USE_CARDGAME_CARD ), TexasHoldem::UseCardGameCardCF )
			, m_PosX(PosX)
			, m_PosY(PosY)
		{
			MIN_STATIC_ASSERT( sizeof( NET_TEXAS_HOLD_EM_USE_CARDGAME_CARD ) <= NET_DATA_BUFSIZE );
		}
	};

	struct NET_TEXAS_HOLD_EM_OPEN_MAIN_MENU_CF : public NET_TEXAS_HOLD_EM_MESSAGE
	{
		DWORD dwNpcGaeaID;

		NET_TEXAS_HOLD_EM_OPEN_MAIN_MENU_CF()
			: NET_TEXAS_HOLD_EM_MESSAGE( sizeof( NET_TEXAS_HOLD_EM_OPEN_MAIN_MENU_CF ), TexasHoldem::OpenMainMenuCF )
			, dwNpcGaeaID( GAEAID_NULL )
		{
			MIN_STATIC_ASSERT( sizeof( NET_TEXAS_HOLD_EM_OPEN_MAIN_MENU_CF ) <= NET_DATA_BUFSIZE );
		}
	};

	struct NET_TEXAS_HOLD_EM_CLOSE_MAIN_MENU_CF : public NET_TEXAS_HOLD_EM_MESSAGE
	{
		NET_TEXAS_HOLD_EM_CLOSE_MAIN_MENU_CF()
			: NET_TEXAS_HOLD_EM_MESSAGE( sizeof( NET_TEXAS_HOLD_EM_CLOSE_MAIN_MENU_CF ), TexasHoldem::CloseMainMenuCF )
		{
			MIN_STATIC_ASSERT( sizeof( NET_TEXAS_HOLD_EM_CLOSE_MAIN_MENU_CF ) <= NET_DATA_BUFSIZE );
		}
	};

	struct NET_TEXAS_HOLD_EM_CLOSE_MAIN_MENU_FC : public NET_TEXAS_HOLD_EM_MESSAGE
	{
		NET_TEXAS_HOLD_EM_CLOSE_MAIN_MENU_FC()
			: NET_TEXAS_HOLD_EM_MESSAGE( sizeof( NET_TEXAS_HOLD_EM_CLOSE_MAIN_MENU_FC ), TexasHoldem::CloseMainMenuFC )
		{
			MIN_STATIC_ASSERT( sizeof( NET_TEXAS_HOLD_EM_CLOSE_MAIN_MENU_FC ) <= NET_DATA_BUFSIZE );
		}
	};

	struct NET_TEXAS_HOLD_EM_CHECK_REQUIREMENT_FB : public NET_TEXAS_HOLD_EM_MESSAGE
	{
		int nResult;
		LONGLONG Version;

		NET_TEXAS_HOLD_EM_CHECK_REQUIREMENT_FB()
			: NET_TEXAS_HOLD_EM_MESSAGE( sizeof( NET_TEXAS_HOLD_EM_CHECK_REQUIREMENT_FB ), TexasHoldem::CheckRequirementFB )
			, nResult( TexasHoldem::eSuccess )
			, Version( -1 )
		{
			MIN_STATIC_ASSERT( sizeof( NET_TEXAS_HOLD_EM_CHECK_REQUIREMENT_FB ) <= NET_DATA_BUFSIZE );
		}
	};

	struct NET_TEXAS_HOLD_EM_CHIP_UPDATE : public NET_TEXAS_HOLD_EM_MESSAGE
	{
		LONGLONG Chip;

		NET_TEXAS_HOLD_EM_CHIP_UPDATE( LONGLONG Chip )
			: NET_TEXAS_HOLD_EM_MESSAGE( sizeof( NET_TEXAS_HOLD_EM_CHIP_UPDATE ), TexasHoldem::ChipUpdate )
		{
			this->Chip = Chip;
			MIN_STATIC_ASSERT( sizeof( NET_TEXAS_HOLD_EM_CHIP_UPDATE ) <= NET_DATA_BUFSIZE );
		}
	};

	struct NET_TEXAS_HOLD_EM_CHIP_MAP_REQUEST : public NET_TEXAS_HOLD_EM_MESSAGE
	{
		NET_TEXAS_HOLD_EM_CHIP_MAP_REQUEST()
			: NET_TEXAS_HOLD_EM_MESSAGE( sizeof( NET_TEXAS_HOLD_EM_CHIP_MAP_REQUEST ), TexasHoldem::ChipMapRequest )
		{
			MIN_STATIC_ASSERT( sizeof( NET_TEXAS_HOLD_EM_CHIP_MAP_REQUEST ) <= NET_DATA_BUFSIZE );
		}
	};

	struct NET_TEXAS_HOLD_EM_CHIP_PURCHASE_IN_GAMEMONEY : public NET_TEXAS_HOLD_EM_MESSAGE
	{
		LONGLONG Chip;
		DWORD dwNpcGaeaID;

		NET_TEXAS_HOLD_EM_CHIP_PURCHASE_IN_GAMEMONEY( LONGLONG Chip, DWORD dwNpcGaeaID )
			: NET_TEXAS_HOLD_EM_MESSAGE( sizeof( NET_TEXAS_HOLD_EM_CHIP_PURCHASE_IN_GAMEMONEY ), TexasHoldem::ChipPurchaseInGameMoney )
		{
			this->Chip = Chip;
			this->dwNpcGaeaID = dwNpcGaeaID;
			MIN_STATIC_ASSERT( sizeof( NET_TEXAS_HOLD_EM_CHIP_PURCHASE_IN_GAMEMONEY ) <= NET_DATA_BUFSIZE );
		}
	};

	struct NET_TEXAS_HOLD_EM_CHIP_PURCHASE_IN_POINT : public NET_TEXAS_HOLD_EM_MESSAGE
	{
		LONGLONG Chip;
		DWORD dwNpcGaeaID;

		NET_TEXAS_HOLD_EM_CHIP_PURCHASE_IN_POINT( LONGLONG Chip, DWORD dwNpcGaeaID )
			: NET_TEXAS_HOLD_EM_MESSAGE( sizeof( NET_TEXAS_HOLD_EM_CHIP_PURCHASE_IN_POINT ), TexasHoldem::ChipPurchaseInPoint )
		{
			this->Chip = Chip;
			this->dwNpcGaeaID = dwNpcGaeaID;
			MIN_STATIC_ASSERT( sizeof( NET_TEXAS_HOLD_EM_CHIP_PURCHASE_IN_POINT ) <= NET_DATA_BUFSIZE );
		}
	};

	struct NET_TEXAS_HOLD_EM_CHIP_PURCHASE_FB : public NET_TEXAS_HOLD_EM_MESSAGE
	{
		int nResult;

		NET_TEXAS_HOLD_EM_CHIP_PURCHASE_FB()
			: NET_TEXAS_HOLD_EM_MESSAGE( sizeof( NET_TEXAS_HOLD_EM_CHIP_PURCHASE_FB ), TexasHoldem::ChipPurchaseFB )
			, nResult( TexasHoldem::eSuccess )
		{
			MIN_STATIC_ASSERT( sizeof( NET_TEXAS_HOLD_EM_CHIP_PURCHASE_FB ) <= NET_DATA_BUFSIZE );
		}
	};

	struct NET_TEXAS_HOLD_EM_START_GAME : public NET_TEXAS_HOLD_EM_MESSAGE
	{
		DWORD dwNpcGaeaID;

		NET_TEXAS_HOLD_EM_START_GAME()
			: NET_TEXAS_HOLD_EM_MESSAGE( sizeof( NET_TEXAS_HOLD_EM_START_GAME ), TexasHoldem::StartGame )
			, dwNpcGaeaID( GAEAID_NULL )
		{
			MIN_STATIC_ASSERT( sizeof( NET_TEXAS_HOLD_EM_START_GAME ) <= NET_DATA_BUFSIZE );
		}
	};

	struct NET_TEXAS_HOLD_EM_START_GAME_FB : public NET_TEXAS_HOLD_EM_MESSAGE
	{
		int nResult;
		char szPostArgument[TexasHoldem::ePostArgumentSize];

		NET_TEXAS_HOLD_EM_START_GAME_FB()
			: NET_TEXAS_HOLD_EM_MESSAGE( sizeof( NET_TEXAS_HOLD_EM_START_GAME_FB ), TexasHoldem::StartGameFB )
			, nResult( TexasHoldem::eSuccess )
		{
			memset( szPostArgument, 0, sizeof( char ) * TexasHoldem::ePostArgumentSize );
			MIN_STATIC_ASSERT( sizeof( NET_TEXAS_HOLD_EM_START_GAME_FB ) <= NET_DATA_BUFSIZE );
		}

		void SetPostArgument( const char* szArgument )
		{
			if ( szArgument )
				StringCchCopy( szPostArgument, TexasHoldem::ePostArgumentSize, szArgument );
		}
	};

	struct NET_TEXAS_HOLD_EM_END_GAME : public NET_TEXAS_HOLD_EM_MESSAGE
	{
		NET_TEXAS_HOLD_EM_END_GAME()
			: NET_TEXAS_HOLD_EM_MESSAGE( sizeof( NET_TEXAS_HOLD_EM_END_GAME ), TexasHoldem::EndGame )
		{
			MIN_STATIC_ASSERT( sizeof( NET_TEXAS_HOLD_EM_END_GAME ) <= NET_DATA_BUFSIZE );
		}
	};

	struct NET_TEXAS_HOLD_EM_CHIP_MAP_REQUEST_INNER : public NET_TEXAS_HOLD_EM_MESSAGE
	{
		NET_TEXAS_HOLD_EM_CHIP_MAP_REQUEST_INNER()
			: NET_TEXAS_HOLD_EM_MESSAGE( sizeof( NET_TEXAS_HOLD_EM_CHIP_MAP_REQUEST_INNER ), TexasHoldem::ChipMapRequestInner )
		{
			MIN_STATIC_ASSERT( sizeof( NET_TEXAS_HOLD_EM_CHIP_MAP_REQUEST_INNER ) <= NET_DATA_BUFSIZE );
		}
	};

	struct NET_TEXAS_HOLD_EM_CHIP_INNER : public NET_TEXAS_HOLD_EM_MESSAGE
	{
		DWORD dwChaDbNum;
		LONGLONG Chip;
		bool bEndGameNotify;

		NET_TEXAS_HOLD_EM_CHIP_INNER( DWORD dwChaDbNum, LONGLONG Chip, bool bEndGameNotify )
			: NET_TEXAS_HOLD_EM_MESSAGE( sizeof( NET_TEXAS_HOLD_EM_CHIP_INNER ), TexasHoldem::ChipInner )
		{
			this->dwChaDbNum = dwChaDbNum;
			this->Chip = Chip;
			this->bEndGameNotify = bEndGameNotify;
			MIN_STATIC_ASSERT( sizeof( NET_TEXAS_HOLD_EM_CHIP_INNER ) <= NET_DATA_BUFSIZE );
		}
	};

	struct NET_TEXAS_HOLD_EM_END_GAME_FB : public NET_TEXAS_HOLD_EM_MESSAGE
	{
		NET_TEXAS_HOLD_EM_END_GAME_FB()
			: NET_TEXAS_HOLD_EM_MESSAGE( sizeof( NET_TEXAS_HOLD_EM_END_GAME_FB ), TexasHoldem::EndGameFB )
		{
			MIN_STATIC_ASSERT( sizeof( NET_TEXAS_HOLD_EM_END_GAME_FB ) <= NET_DATA_BUFSIZE );
		}
	};

	struct NET_TEXAS_HOLD_EM_AUTHKEY_INNER : public NET_TEXAS_HOLD_EM_MESSAGE
	{
		DWORD dwChaDbNum;
		UUID sGUID;

		NET_TEXAS_HOLD_EM_AUTHKEY_INNER( DWORD dwChaDbNum, UUID& sGUID )
			: NET_TEXAS_HOLD_EM_MESSAGE( sizeof( NET_TEXAS_HOLD_EM_AUTHKEY_INNER ), TexasHoldem::AuthKeyInner )
		{
			this->dwChaDbNum = dwChaDbNum;
			this->sGUID = sGUID;
			MIN_STATIC_ASSERT( sizeof( NET_TEXAS_HOLD_EM_AUTHKEY_INNER ) <= NET_DATA_BUFSIZE );
		}
	};
}
