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
		eRequireLevel,	// ������ �����Ϸ��µ� ���� ������ �������� �ʴ´�. 
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

	// Ĩ ����
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
		// Ĩ �Ǹ� ����Ʈ ����
		LONGLONG m_Version;

		// Ĩ / ���� �Ӵ� / �� ����Ʈ
		// map �ΰ� Ȥ�� sqlite �� ����Ϸ��� ������, �����͵� 10�� �̸��̰�
		// �׳� vector�� ����Ѵ�.
		std::vector< sChipPrice > m_Container;

	};

	// ������ �� �� �ִ� ���� Ȯ��: ����� ������
	// ���Ŀ� �߰��� �Ǹ� �Լ��� �߰��ϸ� �ȴ�.
	// CheckRequirement�� �Ķ���͵� �߰��ϸ� �ȴ�.
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
		// ĳ���͹�ȣ, ���ӽ����� �ð�
		std::map< DWORD, __time64_t > m_GameTime;

	};

}


