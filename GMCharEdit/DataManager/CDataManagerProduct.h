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
	 * brief: GMCharEdit�� Product Data ����;
	**/
	class CDataManagerProduct : public CSingletone<CDataManagerProduct>
	{
	protected:
		friend CSingletone;

	public:
		// ���� Type ( ������ Type );
		struct PRODUCT_TYPE
		{
			DWORD dwProductType;	// Type ID
			DWORD dwTotalExp;		// Type ��ü ���� ���õ�;

			PRODUCT_TYPE()
				: dwProductType( 0 )
				, dwTotalExp( 0 )
			{

			}
		};

		// ����ǰ ( ������ ����ǰ );
		struct PRODUCT_EXP
		{
			DWORD dwProductID;		// Product Item ID
			DWORD dwExp;			// Item �� ���õ�;

			PRODUCT_EXP()
				: dwProductID( 0 )
				, dwExp( 0 )
			{

			}
		};

		// ���ۼ� ( ����� ���ۼ� );
		struct PRODUCT_BOOK
		{
			DWORD dwProductBook;	// Product Book ID;

			PRODUCT_BOOK()
				: dwProductBook( 0 )
			{

			}
		};

		// ��ü Item�� ���� Data ����;
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

		// ��ü Type�� ���� Data ����;
		struct PRODUCT_TYPE_INFO
		{
			DWORD dwProductType;
			bool bLearn;
			DWORD dwMaxWS;		// Type�� ��ü ������ ���� �ְ� WS ��;
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
			EMLEARN_TYPE,			// Type ���;
			EMLEARN_BOOK,			// ���ۼ� ���;
			EMPICKUP_EXP			// ���õ� ����;
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
		/// �ʱ�ȭ;
		void Initialize();

		void Clear();

		/// DB���� �о�� ���� Type, Exp, Book ����;
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

		// Type ���� ���� �� CP ����;
		bool GetTypeLearnStateFromDB( DWORD dwProductType );
		int GetTypeTotalExpFromDB( DWORD dwProductType );

		// Type �� ������ ������ ����;
		unsigned int GetExpCountTypeFromDB( const DWORD dwProductType );

	public:
		/// default.charclass
		// ���� Ÿ�� ���� �� ���� ����;
		bool IsOpenProductType( const WORD wProductType );		// �ش� ���ۺо߰� �����о��ΰ�?;
		bool IsUnableProductType( const WORD wProductType );	// �ش� ���ۺо߰� �����о��ΰ�?;

	public:
		/// Tool���� �����Ǵ� Data ����;
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
		// ��ü ������ �ƴϴ�;
		// �����߰ų� ������ ����̴�;
		std::vector<PRODUCT_TYPE> m_vecProductType;		// ������ Type ���;
		std::vector<PRODUCT_EXP> m_vecProductExp;		// ������ Item ���;
		std::vector<PRODUCT_BOOK> m_vecProductBook;		// ���ۼ� ���;
		int m_nChaNum;

		// ��ü ���� ����;
		std::vector<PRODUCT_TYPE_INFO> m_vecProductTypeInfo;

		// ���� Change Set;
		// Change Set�� �������� ���� �� DBAction;
		std::vector<PRODUCT_CHANGE_SET> m_vecChangeSet;

		// DB Manager;
		gmce::IDBManager* m_pDBMan;

	public:
		// Type �̸�;
		static const std::string strTypeName[ Product::ETypeTotal ];

		// �ִ� Type ����;
		static const WORD studyPointDefault;
	};

}