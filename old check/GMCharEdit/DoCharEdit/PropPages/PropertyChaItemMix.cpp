#include "stdafx.h"
#include "../../GMCharEdit.h"
#include "../../MainFrm.h"
#include "./PropertyChaItemMix.h"
#include "../PropertySheetCharEdit.h"

#include "../../SubDialogs/DlgItemMixType.h"
#include "../../SubDialogs/DlgItemMixProductItem.h"

#include "../../DataManager/CDataManagerProduct.h"

#include "../../enginelib/G-Logic/GLogic.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/Product/GLProductRecipeMan.h"
#include "../../RanLogic/Item/GLItemMan.h"

#include "../../Util/CGmCfg.h"

#include "../../Database/ADO/ADOManager.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(CPropertyChaItemMix, CPropertyPage)

CPropertyChaItemMix::CPropertyChaItemMix()
: CPropertyPage( CPropertyChaItemMix::IDD )
, m_childDlgItemMixType( NULL )
, m_childDlgItemProductItem( NULL )
{
	m_pDBMan = CGmCfg::GetInstance()->GetDBMan();
}

CPropertyChaItemMix::~CPropertyChaItemMix() { }

void CPropertyChaItemMix::EnableProperties( HTREEITEM hItem )
{
	if( !m_childDlgItemMixType )
		return;

	if( !m_childDlgItemProductItem )
		return;

	if( !hItem )
		return;

	// ���� Dlg�� ���� ����� �ʼ������� �ʱ�ȭ ������� �Ѵ�;
	m_childDlgItemMixType->ClearProductType();
	m_childDlgItemProductItem->ClearProductType();

	// ���� ���ý�;
	if( m_treeSelect.GetSelectedCount() > 1 )
	{
		// ������ �������� Ÿ��;
		EMPRODUCT_TREE_TYPE type = EMPRODUCT_NONE;

		// ��ü ���ø���� ó���Ѵ�;
		bool bLearnAllType = true;
		HTREEITEM hTempItem = m_treeSelect.GetFirstSelectedItem();
		while( hTempItem )
		{
			for( unsigned int i=0; i<m_vHProductType.size(); ++i )
			{
				// Type;
				if( m_vHProductType.at( i ).hItem == hTempItem )
				{
					type = EMPRODUCT_TYPE;

					// ������ �������� �߰��� �� �⺻��ȭ���ڷ� �����Ѵ�;
					// ���� �����̱⶧���� Ư����Ҹ� ��Ÿ���� �ȵȴ�;
					m_childDlgItemMixType->PushProductType( i );
					m_childDlgItemMixType->UpdateDefaultProductType();

					// �ϳ��� �ȹ���� ��� �����°� �ƴϴ�;
					if( !dm::CDataManagerProduct::Instance()
						->GetTypeLearnStateInfo( i ) )
						bLearnAllType = false;

					break;
				}

				// Product Item;
				bool bBreak = false;
				ITEM_MIX_TREE_ITEM tempTreeItem = m_vHProductType.at( i );
				for( unsigned int j=0; j<tempTreeItem.childItem.size(); ++j )
				{
					// �켱������ Type�̹Ƿ� �̹� Type�� ���õǾ������� �������� �ʴ´�;
					if( tempTreeItem.childItem.at( j ).hItem == hTempItem &&
						type != EMPRODUCT_TYPE )
					{
						type = EMPRODUCT_ITEM;
						PRODUCT_ITEM productItem;
						productItem.dwProductType = i;
						productItem.dwProductID =
							tempTreeItem.childItem.at( j ).dwProductID;

						m_childDlgItemProductItem->PushProductItem( productItem );
						m_childDlgItemProductItem->UpdateDefaultProductItem();

						bBreak = true;

						break;
					}
				}

				if( bBreak )
					break;
			}

			hTempItem = m_treeSelect.GetNextSelectedItem( hTempItem );
		}

		// ������ �������� ��� �������� ���;
		// �����·� ���Ž�Ų��;
		if( bLearnAllType )
			m_childDlgItemMixType->UpdateLearnAllType();

		// �켱������ Ÿ�Դ�ȭ�����̴�;
		switch( type )
		{
		case EMPRODUCT_TYPE:
			m_childDlgItemProductItem->ShowWindow( SW_HIDE );
			m_childDlgItemMixType->ShowWindow( SW_SHOW );
			break;
		case EMPRODUCT_ITEM:
			m_childDlgItemMixType->ShowWindow( SW_HIDE );
			m_childDlgItemProductItem->ShowWindow( SW_SHOW );
			break;
		}
	}
	// �ϳ��� ���ý�;
	else
	{
		for( unsigned int i=0; i<m_vHProductType.size(); ++i )
		{
			// Type;
			if( m_vHProductType.at( i ).hItem == hItem )
			{
				m_childDlgItemMixType->PushProductType( i );
				m_childDlgItemMixType->UpdateProductType( i );
				m_childDlgItemProductItem->ShowWindow( SW_HIDE );
				m_childDlgItemMixType->ShowWindow( SW_SHOW );

				break;
			}

			// Product Item;
			bool bBreak = false;
			ITEM_MIX_TREE_ITEM tempTreeItem = m_vHProductType.at( i );
			for( unsigned int j=0; j<tempTreeItem.childItem.size(); ++j )
			{
				if( tempTreeItem.childItem.at( j ).hItem == hItem )
				{
					PRODUCT_ITEM productItem;
					productItem.dwProductType = i;
					productItem.dwProductID =
						tempTreeItem.childItem.at( j ).dwProductID;

					m_childDlgItemProductItem->PushProductItem( productItem );
					m_childDlgItemProductItem->UpdateProductItem(
						i, tempTreeItem.childItem.at( j ).dwProductID );
					m_childDlgItemMixType->ShowWindow( SW_HIDE );
					m_childDlgItemProductItem->ShowWindow( SW_SHOW );

					bBreak = true;

					break;
				}
			}

			if( bBreak )
				break;
		}
	}
}

void CPropertyChaItemMix::UpdateProductItemListChild()
{
	PRODUCT_RECIPE_KEYMAP_ITER pos =
		GLProductRecipeMan::GetInstance().m_mapKeyProductRecipe.begin();
	PRODUCT_RECIPE_KEYMAP_ITER end =
		GLProductRecipeMan::GetInstance().m_mapKeyProductRecipe.end();

	for( unsigned int i=0; i<Product::ETypeTotal; ++i )
	{
		std::string strTemp;
		if( dm::CDataManagerProduct::Instance()->GetTypeLearnStateInfo( i ) )
		{
			strTemp = sc::string::format( "%d %s Learn ON / CP %d",
				i,
				dm::CDataManagerProduct::strTypeName[ i ],
				dm::CDataManagerProduct::Instance()->GetTypeTypeTotalExpInfo( i ) );
		}
		else if( dm::CDataManagerProduct::Instance()->IsOpenProductType( i ) )
		{
			strTemp = sc::string::format( "%d %s Learn Basic / CP %d",
				i,
				dm::CDataManagerProduct::strTypeName[ i ],
				dm::CDataManagerProduct::Instance()->GetTypeTypeTotalExpInfo( i ) );
		}
		else
		{
			strTemp = sc::string::format( "%d %s Learn OFF / CP %d",
				i,
				dm::CDataManagerProduct::strTypeName[ i ],
				dm::CDataManagerProduct::Instance()->GetTypeTypeTotalExpInfo( i ) );
		}
		

		
		if( i >= 0 && i < m_vHProductType.size() )
		{
			ITEM_MIX_TREE_ITEM tempTreeItem
				= m_vHProductType.at( i );
			HTREEITEM hItem = tempTreeItem.hItem;

			UpdateData();

			TV_ITEM tvi;
			tvi.mask = TVIF_HANDLE;
			tvi.hItem = hItem;
			m_treeSelect.GetItem( &tvi );
			tvi.mask |= TVIF_TEXT;

			tvi.pszText = (LPTSTR)(LPCTSTR)strTemp.c_str();
			m_treeSelect.SetItem( &tvi );
		}
	}

	for( ; pos != end; ++pos )
	{
		const SProductRecipe* pProductRecipe = &(*pos).second;
		if( !pProductRecipe )
			continue;

		DWORD dwProductType = static_cast<DWORD>(
			pProductRecipe->eProductType );
		DWORD dwProductID = pProductRecipe->dwKey;

		dm::CDataManagerProduct::PRODUCT_ITEM_INFO itemInfo
			= dm::CDataManagerProduct::Instance()->GetProductItemInfo(
			dwProductType, dwProductID );

		std::string strItemName;

		SNATIVEID sNativeID = pProductRecipe->sResultItem.sNID;
		SITEM* pItem = GLogicData::GetInstance().GetItem( sNativeID );
		if( pItem )
		{
			strItemName = sc::string::format(
				"%3d %s / CP [%s]",
				pProductRecipe->dwKey,
				pItem->GetName(),
				itemInfo.bCP ? "U" : "A" );

			// ���ۼ��� �ʿ��� ���;
			if( itemInfo.bUseMP )
			{
				strItemName += sc::string::format(
					" IMS [%s]", itemInfo.bMP ? "U" : "A" );
			}

			if( dwProductType >= 0 && dwProductType < m_vHProductType.size() )
			{
				ITEM_MIX_TREE_ITEM tempTreeItem =
					m_vHProductType.at( dwProductType );

				for( unsigned int i=0; i<tempTreeItem.childItem.size(); ++i )
				{
					ITEM_MIX_TREE_ITEM_PRODUCT tempTreeItemProduct =
						tempTreeItem.childItem.at( i );

					if( dwProductID == tempTreeItemProduct.dwProductID )
					{
						HTREEITEM hItem = tempTreeItemProduct.hItem;

						UpdateData();

						TV_ITEM tvi;
						tvi.mask = TVIF_HANDLE;
						tvi.hItem = hItem;
						m_treeSelect.GetItem( &tvi );
						tvi.mask |= TVIF_TEXT;

						tvi.pszText = (LPTSTR)(LPCTSTR)strItemName.c_str();
						m_treeSelect.SetItem( &tvi );
					}
				}
			}
		}
	}
}

void CPropertyChaItemMix::DoDataExchange( CDataExchange* pDX )
{
	CPropertyPage::DoDataExchange( pDX );
	DDX_Control(pDX, IDC_TREE1, m_treeSelect);
}

void CPropertyChaItemMix::UpdateProductItemList()
{
	m_treeSelect.DeleteAllItems();

	PRODUCT_RECIPE_KEYMAP_ITER pos =
		GLProductRecipeMan::GetInstance().m_mapKeyProductRecipe.begin();
	PRODUCT_RECIPE_KEYMAP_ITER end =
		GLProductRecipeMan::GetInstance().m_mapKeyProductRecipe.end();

	m_vHProductType.clear();
	dm::CDataManagerProduct::Instance()->ClearInfo();

	for( unsigned int i=0; i<Product::ETypeTotal; ++i )
	{
		std::string strTemp;
		if( dm::CDataManagerProduct::Instance()->GetTypeLearnStateFromDB( i ) )
		{
			strTemp = sc::string::format( "%d %s Learn ON / CP %d",
				i,
				dm::CDataManagerProduct::strTypeName[ i ],
				dm::CDataManagerProduct::Instance()->GetTypeTotalExpFromDB( i ) );
		}
		else if( dm::CDataManagerProduct::Instance()->IsOpenProductType( i ) )
		{
			strTemp = sc::string::format( "%d %s Learn Basic / CP %d",
				i,
				dm::CDataManagerProduct::strTypeName[ i ],
				dm::CDataManagerProduct::Instance()->GetTypeTotalExpFromDB( i ) );
		}
		else
		{
			strTemp = sc::string::format( "%d %s Learn OFF / CP %d",
				i,
				dm::CDataManagerProduct::strTypeName[ i ],
				dm::CDataManagerProduct::Instance()->GetTypeTotalExpFromDB( i ) );
		}

		HTREEITEM hTemp;
		hTemp = m_treeSelect.InsertItem(
			strTemp.c_str(),
			0, 1,
			TVI_ROOT, TVI_LAST ) ;
		ITEM_MIX_TREE_ITEM tempTreeItem;
		tempTreeItem.dwProductType = static_cast<DWORD>( i );
		tempTreeItem.hItem = hTemp;
		m_vHProductType.push_back( tempTreeItem );

		dm::CDataManagerProduct::Instance()->SetTypeInfo(
			i, dm::CDataManagerProduct::Instance()->GetTypeLearnStateFromDB( i ) );
	}

	for( ; pos != end; ++pos )
	{
		const SProductRecipe* pProductRecipe = &(*pos).second;
		if( !pProductRecipe )
			continue;

		std::string strItemName;

		SNATIVEID sNativeID = pProductRecipe->sResultItem.sNID;
		SITEM* pItem = GLogicData::GetInstance().GetItem( sNativeID );
		if( pItem )
		{
			bool bCP = false;
			bool bUseCP = true;
			bool bMP = false;

			dm::CDataManagerProduct::PRODUCT_EXP product = 
				dm::CDataManagerProduct::Instance()->GetExpProductIDDB( 
				pProductRecipe->dwKey );

			// �������� �ʾҰų� ���� ���õ��� ���� �� ���� ���;
			if( product.dwExp == 0 )
			{
				if( pProductRecipe->wWspPer == 0 )
				{
					bCP = true;
					bUseCP = false;
				}
				else
					bCP = false;
			}
			// �������� ���;
			else
				bCP = true;

			strItemName = sc::string::format(
				"%3d %s / CP [%s]",
				pProductRecipe->dwKey,
				pItem->GetName(),
				bCP ? "U" : "A" );

			// ���ۼ��� �ʿ��� ���;
			if( pProductRecipe->bNeedLearn )
			{
				dm::CDataManagerProduct::PRODUCT_BOOK book = 
					dm::CDataManagerProduct::Instance()->GetBookProductBookDB( 
					pProductRecipe->dwKey );

				// ���ۼ��� �������� �ʾ��� ���;
				if( book.dwProductBook == 0 )
					bMP = false;
				// ���ۼ��� �������� ���;
				else
					bMP = true;

				strItemName += sc::string::format(
					" IMS [%s]", bMP ? "U" : "A" );
			}		

			HTREEITEM hRoot;
			int tempProductType = static_cast<int>( pProductRecipe->eProductType );
			if( tempProductType >= 0 && tempProductType < Product::ETypeTotal )
			{
				hRoot = m_vHProductType.at( tempProductType ).hItem;

				HTREEITEM hTemp;
				hTemp = m_treeSelect.InsertItem(
					strItemName.c_str(),
					0, 1,
					hRoot, TVI_SORT ) ;

				ITEM_MIX_TREE_ITEM_PRODUCT tempTreeItem;
				tempTreeItem.dwProductID =
					static_cast<DWORD>( pProductRecipe->dwKey );
				tempTreeItem.hItem = hTemp;
				m_vHProductType.at( tempProductType )
					.childItem.push_back( tempTreeItem );

				dm::CDataManagerProduct::Instance()->SetItemInfo(
					tempProductType,
					static_cast<DWORD>( pProductRecipe->dwKey ),
					bCP, bUseCP, bMP, pProductRecipe->bNeedLearn );
			}
		}
	}
}

void CPropertyChaItemMix::ProductChangeSetDBAction()
{
	if( m_pDBMan->SaveProductChangeSet() == DB_ERROR )
		MessageBox( _T( "Database Error" ) );
}

BOOL CPropertyChaItemMix::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_treeSelect.EnableMultiSelect();

	// Tree ����;
	UpdateProductItemList();

	// Product Type Dlg ����;
	m_childDlgItemMixType = new CDlgItemMixType( this );
	if( m_childDlgItemMixType )
	{
		m_childDlgItemMixType->Create( CDlgItemMixType::IDD, this );
		m_childDlgItemMixType->ShowWindow( SW_SHOW );
		RECT rect;
		SetRect( &rect, 280, 0, 810, 530 );
		m_childDlgItemMixType->MoveWindow( &rect );
	}

	// Product Item Dlg ����;
	m_childDlgItemProductItem = new CDlgItemMixProductItem( this );
	if( m_childDlgItemProductItem )
	{
		m_childDlgItemProductItem->Create( CDlgItemMixProductItem::IDD, this );
		m_childDlgItemProductItem->ShowWindow( SW_SHOW );
		RECT rect;
		SetRect( &rect, 280, 0, 810, 530 );
		m_childDlgItemProductItem->MoveWindow( &rect );
	}

	return TRUE;
}

void CPropertyChaItemMix::OnOK()
{
	ProductChangeSetDBAction();

	dm::CDataManagerProduct::Instance()->Initialize();

	UpdateProductItemList();

	CPropertyPage::OnOK();
}

BEGIN_MESSAGE_MAP(CPropertyChaItemMix, CPropertyPage)
END_MESSAGE_MAP()