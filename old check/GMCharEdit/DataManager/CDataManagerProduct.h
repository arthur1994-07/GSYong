#pragma once

#include "../../SigmaCore/Util/GLSingletone.h"

#include "../../RanLogic/Product/GLProductRecipeMan.h"

#include <vector>

namespace gmce
{
	class IDBManager;
}

namespace dm
{
	/*
	 * brief: GMCharEdit의 Product Data 관리;
	**/
	class CDataManagerProduct : public CSingletone<CDataManagerProduct>
	{
	protected:
		friend CSingletone;

	public:
		// 제작 Type ( 습득한 Type );
		struct PRODUCT_TYPE
		{
			DWORD dwProductType;	// Type ID
			DWORD dwTotalExp;		// Type 전체 습득 숙련도;

			PRODUCT_TYPE()
				: dwProductType( 0 )
				, dwTotalExp( 0 )
			{

			}
		};

		// 제작품 ( 습득한 제작품 );
		struct PRODUCT_EXP
		{
			DWORD dwProductID;		// Product Item ID
			DWORD dwExp;			// Item 별 숙련도;

			PRODUCT_EXP()
				: dwProductID( 0 )
				, dwExp( 0 )
			{

			}
		};

		// 제작서 ( 사용한 제작서 );
		struct PRODUCT_BOOK
		{
			DWORD dwProductBook;	// Product Book ID;

			PRODUCT_BOOK()
				: dwProductBook( 0 )
			{

			}
		};

		// 전체 Item의 변경 Data 정보;
		struct PRODUCT_ITEM_INFO
		{
			DWORD dwProductID;
			bool bCP;
			bool bUseCP;
			bool bMP;
			bool bUseMP;

			PRODUCT_ITEM_INFO()
				: dwProductID( 0 )
				, bCP( false )
				, bUseCP( true )
				, bMP( false )
				, bUseMP( false )
			{

			}
		};

		// 전체 Type의 변경 Data 정보;
		struct PRODUCT_TYPE_INFO
		{
			DWORD dwProductType;
			bool bLearn;
			DWORD dwMaxWS;		// Type의 전체 아이템 중의 최고 WS 값;
			std::vector<PRODUCT_ITEM_INFO> vecItemInfo;

			PRODUCT_TYPE_INFO()
				: dwProductType( 0 )
				, bLearn( false )
				, dwMaxWS( 0 )
			{

			}
		};

		// Tool Change Set;
		enum EMPRODUCT_CHANGE_ACTION
		{
			NONE,
			EMLEARN_TYPE,			// Type 배움;
			EMLEARN_BOOK,			// 제작서 배움;
			EMPICKUP_EXP			// 숙련도 얻음;
		};

		struct PRODUCT_CHANGE_SET
		{
			DWORD dwProductType;
			DWORD dwProductID;
			bool bState;
			EMPRODUCT_CHANGE_ACTION action;

			PRODUCT_CHANGE_SET()
				: dwProductType( 0 )
				, dwProductID( 0 )
				, bState( false )
				, action( NONE )
			{

			}
		};

	public:
		CDataManagerProduct();
		~CDataManagerProduct() { }

	public:
		/// 초기화;
		void Initialize();

		void Clear();

		/// DB에서 읽어온 현재 Type, Exp, Book 정보;
		void AddTypeDB( const DWORD dwProductType );
		void AddExpDB( const DWORD dwProductID );
		void AddBookDB( const DWORD dwProductBook );

		bool IsTypeDB( const DWORD dwProductType );
		bool IsExpDB( const DWORD dwProductID );
		bool IsBookDB( const DWORD dwProductBook );

		PRODUCT_TYPE GetTypeDB( unsigned int idx ) const;
		PRODUCT_EXP GetExpDB( unsigned int idx ) const;
		PRODUCT_BOOK GetBookDB( unsigned int idx ) const;

		PRODUCT_TYPE GetTypeProductTypeDB( const DWORD dwProductType ) const;
		PRODUCT_EXP GetExpProductIDDB( const DWORD dwProductID ) const;
		PRODUCT_BOOK GetBookProductBookDB( const DWORD dwProductBook ) const;

		// Type 습득 상태 및 CP 정보;
		bool GetTypeLearnStateFromDB( DWORD dwProductType );
		int GetTypeTotalExpFromDB( DWORD dwProductType );

		// Type 별 습득한 아이템 개수;
		unsigned int GetExpCountTypeFromDB( const DWORD dwProductType );

	public:
		/// default.charclass
		// 제작 타입 공개 및 제한 여부;
		bool IsOpenProductType( const WORD wProductType );		// 해당 제작분야가 공개분야인가?;
		bool IsUnableProductType( const WORD wProductType );	// 해당 제작분야가 금지분야인가?;

	public:
		/// Tool에서 관리되는 Data 정보;
		void ClearInfo();
		void SetTypeInfo( const DWORD dwProductType, bool bLearn );
		void SetItemInfo( const DWORD dwProductType, const DWORD dwProductID, bool bCP, bool bUseCP, bool bMP, bool bUseMP );
		void SetItemInfoCP( const DWORD dwProductType, const DWORD dwProductID, bool bCP );
		void SetItemInfoMP( const DWORD dwProductType, const DWORD dwProductID, bool bMP );

		bool GetTypeLearnStateInfo( const DWORD dwProductType ) const;
		DWORD GetTypeMaxWSInfo( const DWORD dwProductType ) const;
		DWORD GetTypeTypeTotalExpInfo( const DWORD dwProductType ) const;
		unsigned int GetExpCountType( const DWORD dwProductType ) const;
		PRODUCT_ITEM_INFO GetProductItemInfo( const DWORD dwProductType, const DWORD dwProductID );
		unsigned int GetTypeLearnCount() const;

	public:
		/// Change Set;
		void AddChangeSet( const PRODUCT_CHANGE_SET& changeSet );
		PRODUCT_CHANGE_SET GetChangeSet( unsigned int idx );
		const unsigned int GetChangeSetCount();
		void ClearChangeSet();

	private:
		void ChangeToolDataChangeSet( const PRODUCT_CHANGE_SET& changeSet );

	public:
		inline void SetDBManager( gmce::IDBManager* pDBMan ) { m_pDBMan = pDBMan; }
		inline void SetChaNum( int nChaNum ) { m_nChaNum = nChaNum; }
		inline const int GetChaNum() const { return m_nChaNum; }

		// DB
		inline unsigned int GetTypeCountDB() const { return m_vecProductType.size(); }
		inline unsigned int GetExpCountDB() const { return m_vecProductExp.size(); }
		inline unsigned int GetBookCountDB() const { return m_vecProductBook.size(); }

	private:
		// 전체 정보가 아니다;
		// 습득했거나 제작한 목록이다;
		std::vector<PRODUCT_TYPE> m_vecProductType;		// 습득한 Type 목록;
		std::vector<PRODUCT_EXP> m_vecProductExp;		// 제작한 Item 목록;
		std::vector<PRODUCT_BOOK> m_vecProductBook;		// 제작서 목록;
		int m_nChaNum;

		// 전체 가능 여부;
		std::vector<PRODUCT_TYPE_INFO> m_vecProductTypeInfo;

		// 변경 Change Set;
		// Change Set을 바탕으로 저장 시 DBAction;
		std::vector<PRODUCT_CHANGE_SET> m_vecChangeSet;

		// DB Manager;
		gmce::IDBManager* m_pDBMan;

	public:
		// Type 이름;
		static const std::string strTypeName[ Product::ETypeTotal ];

		// 최대 Type 개수;
		static const WORD studyPointDefault;
	};

}