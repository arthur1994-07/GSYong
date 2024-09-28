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
		DWORD dwProductID;			// ������ ID;
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
		DWORD dwProductType;		// ���� Type;
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
	// Product Type, Item ���ο� ���� Dlg ����;
	void EnableProperties( HTREEITEM hItem );

	// �ڽ� ������Ƽ�� ���ſ� ���� ����;
	// ����ؼ� ���ŵ� �� ���� ( ��, �����ϱ� �������� ��¥ �����Ͱ� �ƴ� );
	void UpdateProductItemListChild();

protected:
	virtual void DoDataExchange( CDataExchange* pDX );

private:
	// ���� ������ ����Ʈ ����;
	// DB ������� ���� �ε���;
	void UpdateProductItemList();

	void ProductChangeSetDBAction();

private:
	// DB Manager;
	gmce::IDBManager *m_pDBMan;

	// Tree Item ����;
	std::vector<ITEM_MIX_TREE_ITEM> m_vHProductType;

public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
};