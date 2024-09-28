#pragma once

#include "../../SigmaCore/Util/GLSingletone.h"
#include "../../RanLogic/Club/GLClub.h"

#include <vector>

namespace gmce
{
	class IDBManager;
}

namespace dm
{

	class CDataManagerClubInven : public CSingletone<CDataManagerClubInven>
	{
	protected:
		friend CSingletone;

	public:
		CDataManagerClubInven();
		~CDataManagerClubInven();

	public:
		void Initialize();

		void SetValidStorage( bool bStorage );

		bool SETSTORAGE_BYBUF( se::ByteStream& ByteStream );
		bool GETSTORAGE_BYBUF( se::ByteStream& ByteStream ) const;

		bool SETSTORAGE_BYVECTOR( std::vector< SINVENITEM_SAVE >& vecItems );
		bool GETSTORAGE_BYVECTOR( std::vector< SINVENITEM_SAVE >& vecItems );

		bool DeleteItem( unsigned int nChannel, WORD wX, WORD wY );

		void SaveClubInvenItemToDB();

		void Clear();

	public:
		inline void SetDBManager( gmce::IDBManager* pDBMan ) { m_pDBMan = pDBMan; }
		inline void SetChaNum( int nChaNum ) { m_nChaNum = nChaNum; }

		inline const int GetChaNum() const { return m_nChaNum; }
		inline const DWORD GetGuNum() const { return m_GuNum; }

		inline GLInventoryStorage* GetStorage( unsigned int idx )
		{
			if( idx >= 0 && idx < MAX_CLUBSTORAGE )
				return &m_cStorage[ idx ];

			return NULL;
		}

		inline bool GetUseStorageState() { return m_bStorage; }

		inline void SetClubInvenMoney( LONGLONG llMoney ) { m_clubInvenMoney = llMoney; }
		inline LONGLONG GetClubInvenMoney() const { return m_clubInvenMoney; }
		inline void SetGuNum( DWORD GuNum ) { m_GuNum = GuNum; }

	public:
		// 클럽 창고 인벤토리;
		GLInventoryStorage m_cStorage[ MAX_CLUBSTORAGE ];
		bool m_bStorage;
		LONGLONG m_clubInvenMoney;

		int m_nChaNum;
		DWORD m_GuNum;

		// DB Manager;
		gmce::IDBManager* m_pDBMan;
	};
}