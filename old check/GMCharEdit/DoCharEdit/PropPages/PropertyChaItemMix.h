#pragma once

#include "afxcmn.h"

#include "../../resource.h"

#include "../../MultiSelectedTree/SelectTreeCtrl.h"

#include <vector>

namespace gmce
{
	class IDBManager;
}

class CDlgItemMixType;
class CDlgItemMixProductItem;

class CPropertyChaItemMix : public CPropertyPage
{
	DECLARE_DYNAMIC(CPropertyChaItemMix)

private:
	enum EMPRODUCT_TREE_TYPE
	{
		EMPRODUCT_NONE,
		EMPRODUCT_TYPE,
		EMPRODUCT_ITEM
	};

	// Tree Item ( Product Item )
	struct ITEM_MIX_TREE_ITEM_PRODUCT
	{
		DWORD dwProductID;			// 제작템 ID;
		HTREEITEM hItem;

		ITEM_MIX_TREE_ITEM_PRODUCT()
			: dwProductID( 0 )
			, hItem( NULL )
		{

		}
	};

	// Tree Item ( Product Type )
	struct ITEM_MIX_TREE_ITEM
	{
		DWORD dwProductType;		// 제작 Type;
		HTREEITEM hItem;
		std::vector<ITEM_MIX_TREE_ITEM_PRODUCT> childItem;

		ITEM_MIX_TREE_ITEM()
			: dwProductType( 0 )
			, hItem( NULL )
		{

		}

		~ITEM_MIX_TREE_ITEM()
		{
			childItem.clear();
		}
	};

public:
	CPropertyChaItemMix();
	virtual ~CPropertyChaItemMix();

	enum { IDD = IDD_PROPERTY_CHAR_ITEMMIX };

	// Tree
	CSelectTreeCtrl m_treeSelect;

	// Child Dlg
	CDlgItemMixType* m_childDlgItemMixType;
	CDlgItemMixProductItem* m_childDlgItemProductItem;

public:
	// Product Type, Item 여부에 따른 Dlg 변경;
	void EnableProperties( HTREEITEM hItem );

	// 자식 프로퍼티의 갱신에 의한 갱신;
	// 계속해서 갱신될 때 갱신 ( 즉, 저장하기 전까지는 진짜 데이터가 아님 );
	void UpdateProductItemListChild();

protected:
	virtual void DoDataExchange( CDataExchange* pDX );

private:
	// 제조 아이템 리스트 갱신;
	// DB 기반으로 최초 로딩함;
	void UpdateProductItemList();

	void ProductChangeSetDBAction();

private:
	// DB Manager;
	gmce::IDBManager *m_pDBMan;

	// Tree Item 관리;
	std::vector<ITEM_MIX_TREE_ITEM> m_vHProductType;

public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
};