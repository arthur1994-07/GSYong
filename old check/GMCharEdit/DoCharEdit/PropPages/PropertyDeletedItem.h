#pragma once
#include "afxcmn.h"

#include "../../RanLogic/Character/GLCharData.h"
#include "../../RanLogic/Inventory/GLInventory.h"

#include <vector>

namespace gmce
{
	class IDBManager;
}

// CPropertyDeletedItem 대화 상자입니다.

class CPropertyDeletedItem : public CPropertyPage
{
	DECLARE_DYNAMIC(CPropertyDeletedItem)

public:
	CPropertyDeletedItem();
	virtual ~CPropertyDeletedItem();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_PROPERTY_CHAR_DELETEDITEM };

	gmce::IDBManager* m_pDBMan;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

public:
	void SetData( SCHARDATA2* pChar );
	void GetData( SCHARDATA2* pChar );
	void InitListHead();

	std::vector< SITEMCUSTOM > m_vItem;

	DECLARE_MESSAGE_MAP()
	afx_msg void OnNMDblclkSellitemList(NMHDR *pNMHDR, LRESULT *pResult);
};
