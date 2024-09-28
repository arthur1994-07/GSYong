#include "stdafx.h"

#include "./CDataManagerProduct.h"
#include "../../RanLogic/GLogicData.h"
#include "../../RanLogic/GLogicDataMan.h"

#include "../Database/ADO/ADOManager.h"
#include "../Database/ODBC/CGMOdbcManager.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace dm
{
	const std::string CDataManagerProduct::strTypeName[ Product::ETypeTotal ] = 
	{
		"�Ӹ�",
		"����",
		"����",
		"�尩",
		"�Ź�",
		"����",
		"�����+�Ͱ���",
		"����+��Ʈ",
		"����+��ű�",
		"��Ÿ",
		"�����ڽ�",
		"�ڽ�Ƭ",
	};

	const WORD CDataManagerProduct::studyPointDefault = (WORD)GLCONST_CHAR::dwStudyPointDefault;

	CDataManagerProduct::CDataManagerProduct()
		: m_pDBMan( NULL )
		, m_nChaNum( 0 )
	{

	}

	void CDataManagerProduct::Initialize()
	{
		// GLProductRecipeMan �ʱ�ȭ �� �ε�;
		GLProductRecipeMan::GetInstance().CleanUp();
		GLProductRecipeMan::GetInstance().LoadFile(
			GLogicData::GetInstance().GetItemMixFileName(), true );

		// CDataManagerProduct �ʱ�ȭ �� �ε�;
		if( m_pDBMan->GetProductInfo( m_nChaNum ) == DB_ERROR )
			return;
	}

	/// DB���� �о�� ���� Type, Exp, Book ����;
	void CDataManagerProduct::Clear()
	{
		m_vecProductType.clear();
		m_vecProductExp.clear();
		m_vecProductBook.clear();
		m_vecProductTypeInfo.clear();
		m_vecChangeSet.clear();
	}

	void CDataManagerProduct::AddTypeDB( const DWORD dwProductType )
	{
		PRODUCT_TYPE temp;
		temp.dwProductType = dwProductType;
		m_vecProductType.push_back( temp );
	}

	void CDataManagerProduct::AddExpDB( const DWORD dwProductID )
	{
		PRODUCT_EXP temp;
		temp.dwProductID = dwProductID;

		PRODUCT_RECIPE_KEYMAP_ITER pos =
			GLProductRecipeMan::GetInstance().m_mapKeyProductRecipe.begin();
		PRODUCT_RECIPE_KEYMAP_ITER end =
			GLProductRecipeMan::GetInstance().m_mapKeyProductRecipe.end();

		for( ; pos != end; ++pos )
		{
			const SProductRecipe* pProductRecipe = &(*pos).second;
			if( !pProductRecipe )
				continue;

			if( dwProductID == pProductRecipe->dwKey )
			{
				// ���õ� ǥ��;
				temp.dwExp = pProductRecipe->wWsp;

				// Type ���� �ִ� ���õ� ����Ͽ� ����;
				DWORD tempProductType = static_cast<DWORD>(
					pProductRecipe->eProductType );

				for( unsigned int i=0; i<m_vecProductType.size(); ++i )
				{
					if( m_vecProductType.at( i ).dwProductType ==
						tempProductType )
						m_vecProductType.at( i ).dwTotalExp += temp.dwExp;
				}
				break;
			}
		}

		m_vecProductExp.push_back( temp );
	}

	void CDataManagerProduct::AddBookDB( const DWORD dwProductBook )
	{
		PRODUCT_BOOK temp;
		temp.dwProductBook = dwProductBook;
		m_vecProductBook.push_back( temp );
	}

	bool CDataManagerProduct::IsTypeDB( const DWORD dwProductType )
	{
		for( unsigned int i=0; i<m_vecProductType.size(); ++i )
		{
			if( m_vecProductType.at( i ).dwProductType == dwProductType )
				return true;
		}

		return false;
	}

	bool CDataManagerProduct::IsExpDB( const DWORD dwProductID )
	{
		for( unsigned int i=0; i<m_vecProductExp.size(); ++i )
		{
			if( m_vecProductExp.at( i ).dwProductID == dwProductID )
				return true;
		}

		return false;
	}

	bool CDataManagerProduct::IsBookDB( const DWORD dwProductBook )
	{
		for( unsigned int i=0; i<m_vecProductBook.size(); ++i )
		{
			if( m_vecProductBook.at( i ).dwProductBook == dwProductBook )
				return true;
		}

		return false;
	}

	CDataManagerProduct::PRODUCT_TYPE CDataManagerProduct::GetTypeDB( unsigned int idx ) const
	{
		if( idx >= 0 && idx < GetTypeCountDB() )
			return m_vecProductType.at( idx );

		return PRODUCT_TYPE();
	};

	CDataManagerProduct::PRODUCT_EXP CDataManagerProduct::GetExpDB( unsigned int idx ) const
	{
		if( idx >= 0 && idx < GetExpCountDB() )
			return m_vecProductExp.at( idx );

		return PRODUCT_EXP();
	};

	CDataManagerProduct::PRODUCT_BOOK CDataManagerProduct::GetBookDB( unsigned int idx ) const
	{
		if( idx >= 0 && idx < GetBookCountDB() )
			return m_vecProductBook.at( idx );

		return PRODUCT_BOOK();
	}

	CDataManagerProduct::PRODUCT_TYPE CDataManagerProduct::GetTypeProductTypeDB( const DWORD dwProductType ) const
	{
		for( unsigned int i=0; i<m_vecProductType.size(); ++i )
		{
			if( m_vecProductType.at( i ).dwProductType == dwProductType )
				return m_vecProductType.at( i );
		}

		return PRODUCT_TYPE();
	}

	CDataManagerProduct::PRODUCT_EXP CDataManagerProduct::GetExpProductIDDB( const DWORD dwProductID ) const
	{
		for( unsigned int i=0; i<m_vecProductExp.size(); ++i )
		{
			if( m_vecProductExp.at( i ).dwProductID == dwProductID )
				return m_vecProductExp.at( i );
		}

		return PRODUCT_EXP();
	}

	CDataManagerProduct::PRODUCT_BOOK CDataManagerProduct::GetBookProductBookDB( const DWORD dwProductBook ) const
	{
		for( unsigned int i=0; i<m_vecProductBook.size(); ++i )
		{
			if( m_vecProductBook.at( i ).dwProductBook == dwProductBook )
				return m_vecProductBook.at( i );
		}

		return PRODUCT_BOOK();
	}

	bool CDataManagerProduct::GetTypeLearnStateFromDB( DWORD dwProductType )
	{
		for( unsigned int i=0; i<m_vecProductType.size(); ++i )
		{
			if( m_vecProductType.at( i ).dwProductType == dwProductType )
				return true;
		}

		return false;
	}

	int CDataManagerProduct::GetTypeTotalExpFromDB( DWORD dwProductType )
	{
		for( unsigned int i=0; i<m_vecProductType.size(); ++i )
		{
			if( m_vecProductType.at( i ).dwProductType == dwProductType )
				return m_vecProductType.at( i ).dwTotalExp;
		}

		return 0;
	}

	unsigned int CDataManagerProduct::GetExpCountTypeFromDB( const DWORD dwProductType )
	{
		unsigned int count = 0;
		for( unsigned int i=0; i<m_vecProductType.size(); ++i )
		{
			if( m_vecProductType.at( i ).dwProductType == dwProductType )
				count++;
		}

		return count;
	}

	/// default.charclass
	bool CDataManagerProduct::IsOpenProductType( const WORD wProductType )
	{
		if( wProductType < Product::ETypeHeadGear || wProductType >= Product::ETypeTotal )
			return false;

		return GLCONST_CHAR::bPublicStudyProductType[ wProductType ];
	}

	bool CDataManagerProduct::IsUnableProductType( const WORD wProductType )
	{
		if( wProductType < Product::ETypeHeadGear || wProductType >= Product::ETypeTotal )
			return false;

		return GLCONST_CHAR::bUnableProductType[ wProductType ];
	}


	/// Tool���� �����Ǵ� Data ����;
	void CDataManagerProduct::ClearInfo()
	{
		m_vecProductTypeInfo.clear();
		m_vecChangeSet.clear();
	}

	void CDataManagerProduct::SetTypeInfo(
		const DWORD dwProductType, bool bLearn )
	{
		PRODUCT_TYPE_INFO typeInfo;
		typeInfo.bLearn = bLearn;
		typeInfo.dwProductType = dwProductType;

		bool bOK = false;
		for( unsigned int i=0; i<m_vecProductTypeInfo.size(); ++i )
		{
			PRODUCT_TYPE_INFO& tempTypeInfo =
				m_vecProductTypeInfo.at( i );
			if( tempTypeInfo.dwProductType == typeInfo.dwProductType )
			{
				bOK = true;
				tempTypeInfo.bLearn = bLearn;
			}
		}

		if( !bOK )
			m_vecProductTypeInfo.push_back( typeInfo );
	}

	void CDataManagerProduct::SetItemInfo(
		const DWORD dwProductType,
		const DWORD dwProductID,
		bool bCP, bool bUseCP, bool bMP, bool bUseMP )
	{
		PRODUCT_ITEM_INFO itemInfo;
		itemInfo.dwProductID = dwProductID;
		itemInfo.bCP = bCP;
		itemInfo.bUseCP = bUseCP;
		itemInfo.bMP = bMP;
		itemInfo.bUseMP = bUseMP;

		for( unsigned int i=0; i<m_vecProductTypeInfo.size(); ++i )
		{
			PRODUCT_TYPE_INFO& typeInfo = m_vecProductTypeInfo.at( i );
			if( typeInfo.dwProductType == dwProductType )
			{
				bool bOK = false;
				for( unsigned int j=0; j<typeInfo.vecItemInfo.size(); ++j )
				{
					PRODUCT_ITEM_INFO& tempItemInfo =
						typeInfo.vecItemInfo.at( j );
					if( tempItemInfo.dwProductID == itemInfo.dwProductID )
					{
						tempItemInfo.bCP = bCP;
						tempItemInfo.bUseCP = bUseCP;
						tempItemInfo.bMP = bMP;
						tempItemInfo.bUseMP = bUseMP;
					}
				}

				if( !bOK )
				{
					const SProductRecipe productRecipe =
						GLProductRecipeMan::GetInstance()
						.m_mapKeyProductRecipe[ itemInfo.dwProductID ];
					if( typeInfo.dwMaxWS < productRecipe.wWs )
						typeInfo.dwMaxWS = productRecipe.wWs;

					typeInfo.vecItemInfo.push_back( itemInfo );
				}
			}
		}
	}

	void CDataManagerProduct::SetItemInfoCP(
		const DWORD dwProductType,
		const DWORD dwProductID,
		bool bCP )
	{
		for( unsigned int i=0; i<m_vecProductTypeInfo.size(); ++i )
		{
			PRODUCT_TYPE_INFO& typeInfo = m_vecProductTypeInfo.at( i );
			if( typeInfo.dwProductType == dwProductType )
			{
				bool bOK = false;
				for( unsigned int j=0; j<typeInfo.vecItemInfo.size(); ++j )
				{
					PRODUCT_ITEM_INFO& tempItemInfo =
						typeInfo.vecItemInfo.at( j );
					if( tempItemInfo.dwProductID == dwProductID )
						tempItemInfo.bCP = bCP;
				}
			}
		}
	}

	void CDataManagerProduct::SetItemInfoMP(
		const DWORD dwProductType,
		const DWORD dwProductID,
		bool bMP )
	{
		for( unsigned int i=0; i<m_vecProductTypeInfo.size(); ++i )
		{
			PRODUCT_TYPE_INFO& typeInfo = m_vecProductTypeInfo.at( i );
			if( typeInfo.dwProductType == dwProductType )
			{
				for( unsigned int j=0; j<typeInfo.vecItemInfo.size(); ++j )
				{
					PRODUCT_ITEM_INFO& tempItemInfo =
						typeInfo.vecItemInfo.at( j );
					if( tempItemInfo.dwProductID == dwProductID )
						tempItemInfo.bMP = bMP;
				}
			}
		}
	}

	bool CDataManagerProduct::GetTypeLearnStateInfo( const DWORD dwProductType ) const
	{
		for( unsigned int i=0; i<m_vecProductTypeInfo.size(); ++i )
		{
			PRODUCT_TYPE_INFO typeInfo = m_vecProductTypeInfo.at( i );
			if( typeInfo.dwProductType == dwProductType )
			{
				return typeInfo.bLearn;
			}
		}

		return false;
	}

	DWORD CDataManagerProduct::GetTypeMaxWSInfo( const DWORD dwProductType ) const
	{
		for( unsigned int i=0; i<m_vecProductTypeInfo.size(); ++i )
		{
			PRODUCT_TYPE_INFO typeInfo = m_vecProductTypeInfo.at( i );
			if( typeInfo.dwProductType == dwProductType )
			{
				return typeInfo.dwMaxWS;
			}
		}

		return 0;
	}

	DWORD CDataManagerProduct::GetTypeTypeTotalExpInfo( const DWORD dwProductType ) const
	{
		DWORD dwTotalExp = 0;
		for( unsigned int i=0; i<m_vecProductTypeInfo.size(); ++i )
		{
			PRODUCT_TYPE_INFO typeInfo = m_vecProductTypeInfo.at( i );
			if( typeInfo.dwProductType == dwProductType )
			{
				for( unsigned int j=0; j<typeInfo.vecItemInfo.size(); ++j )
				{
					PRODUCT_ITEM_INFO itemInfo = typeInfo.vecItemInfo.at( j );
					if( itemInfo.bUseCP && itemInfo.bCP )
					{
						const SProductRecipe productRecipe =
							GLProductRecipeMan::GetInstance()
							.m_mapKeyProductRecipe[ itemInfo.dwProductID ];
						dwTotalExp += static_cast<DWORD>( productRecipe.wWsp );
					}
				}
			}
		}

		return dwTotalExp;
	}

	unsigned int CDataManagerProduct::GetExpCountType( const DWORD dwProductType ) const
	{
		unsigned int count = 0;
		for( unsigned int i=0; i<m_vecProductTypeInfo.size(); ++i )
		{
			PRODUCT_TYPE_INFO typeInfo = m_vecProductTypeInfo.at( i );
			if( typeInfo.dwProductType == dwProductType )
			{
				for( unsigned int j=0; j<typeInfo.vecItemInfo.size(); ++j )
				{
					PRODUCT_ITEM_INFO itemInfo = typeInfo.vecItemInfo.at( j );
					if( itemInfo.bUseCP && itemInfo.bCP )
						count++;
				}
			}
		}

		return count;
	}

	CDataManagerProduct::PRODUCT_ITEM_INFO CDataManagerProduct::GetProductItemInfo(
		const DWORD dwProductType, const DWORD dwProductID )
	{
		for( unsigned int i=0; i<m_vecProductTypeInfo.size(); ++i )
		{
			PRODUCT_TYPE_INFO typeInfo = m_vecProductTypeInfo.at( i );
			if( typeInfo.dwProductType == dwProductType )
			{
				for( unsigned int j=0; j<typeInfo.vecItemInfo.size(); ++j )
				{

					PRODUCT_ITEM_INFO itemInfo = typeInfo.vecItemInfo.at( j );
					if( itemInfo.dwProductID == dwProductID )
						return itemInfo;
				}
			}
		}

		return PRODUCT_ITEM_INFO();
	}

	unsigned int CDataManagerProduct::GetTypeLearnCount() const
	{
		unsigned int count = 0;
		for( unsigned int i=0; i<m_vecProductTypeInfo.size(); ++i )
		{
			PRODUCT_TYPE_INFO typeInfo = m_vecProductTypeInfo.at( i );
			if( typeInfo.bLearn )
				count++;
		}

		return count;
	}

	/// Change Set
	void CDataManagerProduct::AddChangeSet( const PRODUCT_CHANGE_SET& changeSet )
	{
		bool bErase = false;

		// �ִ� Learn Type ���� �̻����� ������ �ϸ�;
		if( GetTypeLearnCount() >= studyPointDefault &&
			changeSet.action == EMLEARN_TYPE &&
			changeSet.bState == true )
		{
			// �����Ѵ�;
			printf( "[ Warning ] To overflow product type's count!!!\n");
			return;
		}

		std::vector<PRODUCT_CHANGE_SET>::reverse_iterator r_iter = m_vecChangeSet.rbegin();
		std::vector<PRODUCT_CHANGE_SET>::reverse_iterator r_iter_end = m_vecChangeSet.rend();
		for( ; r_iter != r_iter_end; )
		{
			PRODUCT_CHANGE_SET tempChangeSet = *r_iter;

			// ���;
			if( tempChangeSet.action == changeSet.action &&
				tempChangeSet.dwProductType == changeSet.dwProductType &&
				tempChangeSet.dwProductID == changeSet.dwProductID &&
				tempChangeSet.bState != changeSet.bState )
			{
				m_vecChangeSet.erase( (++r_iter).base() );
				bErase = true;

				tempChangeSet.bState = !tempChangeSet.bState;
				ChangeToolDataChangeSet( tempChangeSet );
			}
			// �̹� �����Ƿ� ����;
			else if( tempChangeSet.action == changeSet.action &&
				tempChangeSet.dwProductType == changeSet.dwProductType &&
				tempChangeSet.dwProductID == changeSet.dwProductID &&
				tempChangeSet.bState == changeSet.bState )
			{
				bErase = true;

				++r_iter;
			}
			// Type Learn Change Set�� �� ���;
			// ���� Type���� ����� ��� Change Set ����;
			else if( changeSet.action == EMLEARN_TYPE &&
				!changeSet.bState &&
				( tempChangeSet.action == EMLEARN_BOOK ||
				tempChangeSet.action == EMPICKUP_EXP ) &&
				tempChangeSet.dwProductType == changeSet.dwProductType )
			{
				m_vecChangeSet.erase( (++r_iter).base() );

				tempChangeSet.bState = !tempChangeSet.bState;
				ChangeToolDataChangeSet( tempChangeSet );
			}
			// ���ۼ��� �ȹ�� ���·� �ٲ� ���;
			// ���� ID�� CP���� Change Set ����;
			else if( changeSet.action == EMLEARN_BOOK &&
				changeSet.bState &&
				tempChangeSet.action == EMPICKUP_EXP &&
				tempChangeSet.dwProductID == changeSet.dwProductID )
			{
				m_vecChangeSet.erase( (++r_iter).base() );

				tempChangeSet.bState = !tempChangeSet.bState;
				ChangeToolDataChangeSet( tempChangeSet );
			}
			else
			{
				r_iter++;
			}
		}

		// ������ �ʾ��� ��� ChangeSet �߰�;
		if( !bErase )
		{
			m_vecChangeSet.push_back( changeSet );

			ChangeToolDataChangeSet( changeSet );
		}
	}

	CDataManagerProduct::PRODUCT_CHANGE_SET CDataManagerProduct::GetChangeSet( unsigned int idx )
	{
		if( idx >= 0 && idx < m_vecChangeSet.size() )
			return m_vecChangeSet.at( idx );

		return PRODUCT_CHANGE_SET();
	}

	const unsigned int CDataManagerProduct::GetChangeSetCount()
	{
		return m_vecChangeSet.size();
	}

	void CDataManagerProduct::ClearChangeSet()
	{
		m_vecChangeSet.clear();
	}

	void CDataManagerProduct::ChangeToolDataChangeSet( const PRODUCT_CHANGE_SET& changeSet )
	{
		switch( changeSet.action )
		{
		case EMLEARN_TYPE:
			SetTypeInfo( changeSet.dwProductType, changeSet.bState );
			break;
		case EMLEARN_BOOK:
			SetItemInfoMP(
				changeSet.dwProductType,
				changeSet.dwProductID,
				changeSet.bState );
			break;
		case EMPICKUP_EXP:
			SetItemInfoCP(
				changeSet.dwProductType,
				changeSet.dwProductID,
				changeSet.bState );
			break;
		}
	}
}
