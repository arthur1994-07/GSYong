#pragma once
#include "afxcmn.h"

#include "../../RanLogic/Character/GLCharData.h"
#include "../../RanLogic/Inventory/GLInventory.h"

#include <vector>

namespace gmce
{
	class IDBManager;
}

// CPropertyDeletedItem ��ȭ �����Դϴ�.

class CPropertyDeletedItem : public CPropertyPage
{
	DECLARE_DYNAMIC(CPropertyDeletedItem)

public:
	CPropertyDeletedItem();
	virtual ~CPropertyDeletedItem();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_PROPERTY_CHAR_DELETEDITEM };

	gmce::IDBManager* m_pDBMan;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

public:
	void SetData( SCHARDATA2* pChar );
	void GetData( SCHARDATA2* pChar );
	void InitListHead();

	std::vector< SITEMCUSTOM > m_vItem;

	DECLARE_MESSAGE_MAP()
	afx_msg void OnNMDblclkSellitemList(NMHDR *pNMHDR, LRESULT *pResult);
};
