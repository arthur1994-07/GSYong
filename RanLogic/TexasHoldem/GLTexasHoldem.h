#pragma once

#include <vector>
#include <map>
#include "../Market/GLSearchDelay.h"

struct NET_MSG_GENERIC;

namespace TexasHoldem
{
	enum eCommonError
	{
		eSuccess = 300, 
		eCharacterIsNull,
		eRequireLevel,	// 게임을 시작하려는데 레벨 조건이 만족되지 않는다. 
		eLandIsNull, 
		eCrowIsNull, 
		eItemIsNull,
		eWrongItemType,
		eItemCoolTime,
		eCrowType, 
		eCrowDistance, 
		eWrongChip, 
		eLackOfGameMoney, 
		eLackOfPoint, 
		ePostArgumentEmpty, 
		ePostArgumentSizeError, 
		ePostArgumentFail, 
		eWebErrorUser, 
		eWebErrorAuth, 
		eWebErrorChip, 
		eWebErrorUnknown, 
		eCantPurchaseChipInGameMoney, 
		eCantPurchaseChipInPoint, 
	};

	enum eExchange_Type
	{
		eExchange_None,
		eExchange_Gamemoney,
		eExchange_Ranpoint
	};

	enum
	{
		ePostArgumentSize = 768, 
	};

	struct sChipPrice
	{
		LONGLONG Chip;
		LONGLONG GameMoney;
		LONGLONG Point;

		sChipPrice()
			: Chip( 0 )
			, GameMoney( 0 )
			, Point( 0 )
		{
		}

		sChipPrice( LONGLONG Chip, LONGLONG GameMoney, LONGLONG Point )
		{
			this->Chip = Chip;
			this->GameMoney = GameMoney;
			this->Point = Point;
		}

		bool operator == ( const sChipPrice& rvalue ) const
		{
			if ( Chip != rvalue.Chip )
				return false;

			return true;
		}

		bool operator != ( const sChipPrice& rvalue ) const
		{
			return !( *this == rvalue );
		}
	};

	// 칩 가격
	class GLTexasHoldemChipMap
	{
	public:
		GLTexasHoldemChipMap();
		~GLTexasHoldemChipMap();

	public:
		LONGLONG GetVersion() { return m_Version; }
		void SetVersion( LONGLONG Version ) { m_Version = Version; }

		bool CheckVersion( LONGLONG Version );
		
		void Reset();

		void InsertChipPrice( sChipPrice& sChip );

		bool GetChipPriceOfGameMoney( LONGLONG& Chip, LONGLONG& GameMoney OUT );
		bool GetChipPriceOfRanPoint( LONGLONG& Chip, LONGLONG& RanPoint OUT );

		const std::vector< sChipPrice >& GetChipMap() { return m_Container; }

	protected:
		// 칩 판매 리스트 버전
		LONGLONG m_Version;

		// 칩 / 게임 머니 / 란 포인트
		// map 두개 혹은 sqlite 를 사용하려고 했으나, 데이터도 10개 미만이고
		// 그냥 vector를 사용한다.
		std::vector< sChipPrice > m_Container;

	};

	// 게임을 할 수 있는 조건 확인: 현재는 레벨만
	// 추후에 추가가 되면 함수를 추가하면 된다.
	// CheckRequirement에 파라미터도 추가하면 된다.
	class GLTexasHoldemPlayRequirement
	{
	public:
		GLTexasHoldemPlayRequirement();
		~GLTexasHoldemPlayRequirement();

	public:
		int CheckRequirement( DWORD CharacterLevel );

	protected:
		bool checkLevel( DWORD CharacterLevel );

	};

	class GLTexasHoldemMgr
	{
	public:
		GLTexasHoldemMgr();
		virtual ~GLTexasHoldemMgr();

	public:
		LONGLONG GetVersion();
		void SetVersion( LONGLONG Version );

		bool CheckVersion( LONGLONG Version );

		void Reset();

		void InsertChipPrice( sChipPrice& sChip );

		bool GetChipPriceOfGameMoney( LONGLONG& Chip, LONGLONG& GameMoney OUT );
		bool GetChipPriceOfRanPoint( LONGLONG& Chip, LONGLONG& RanPoint OUT );

		const std::vector< sChipPrice >& GetChipMap();

	public:
		virtual void FrameMove( float fElapsedTime ) = 0;
		virtual void MessageProcess( NET_MSG_GENERIC* pMessage, DWORD ClientID = 0, DWORD GaeaID = 0 ) = 0;

	public:
		int CheckRequirement( DWORD CharacterLevel );

	protected:
		GLTexasHoldemChipMap m_ChipMap;
		GLTexasHoldemPlayRequirement m_Requirement;

	};

	class GLTexasHoldemStartGameDelayMgr : public private_market::GLSearchDelayMgr
	{
	public:
		GLTexasHoldemStartGameDelayMgr();
		~GLTexasHoldemStartGameDelayMgr();
	};

	class GLTexasHoldemGameTime
	{
	public:
		GLTexasHoldemGameTime();
		virtual ~GLTexasHoldemGameTime();

	public:
		void AddUser( DWORD dwChaDbNum );
		int GetGameTime( DWORD dwChaDbNum );
		void DeleteUser( DWORD dwChaDbNum );

	protected:
		// 캐릭터번호, 게임시작한 시간
		std::map< DWORD, __time64_t > m_GameTime;

	};

}


