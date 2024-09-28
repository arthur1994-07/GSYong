#pragma once

#include "../resource.h"
#include "afxwin.h"

#include "../DataManager/CDataManagerProduct.h"
#include "../../RanLogic/Product/GLProductDataType.h"

class CDlgItemMixProductItem : public CDialog
{
	DECLARE_DYNAMIC( CDlgItemMixProductItem )

public:
	CDlgItemMixProductItem( CWnd* pParent = NULL );
	virtual ~CDlgItemMixProductItem();

	enum { IDD = IDD_DLG_ITEMMIX_PRODUCT_ITEM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

public:
	// �⺻ ����;
	void UpdateDefaultProductItem();

	// Product Item�� ���� ����;
	void UpdateProductItem( DWORD dwProductType, DWORD dwProductID );

	// Multi Select �� ���� �����̳� ����;
	void PushProductItem( const PRODUCT_ITEM& productItem );
	void DelProductItem( const PRODUCT_ITEM& productItem );
	void ClearProductType();

public:
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
private:
	CEdit m_viewType;
	CEdit m_viewProductID;
	CEdit m_viewProductMID;
	CEdit m_viewProductSID;
	CEdit m_viewProductName;
	CEdit m_viewTypeName;
	CEdit m_viewCP;
	CButton m_checkLearn;
	CEdit m_viewWS;
	CEdit m_viewMaxWS;
	CEdit m_viewWPS;
	CEdit m_viewWSPPer;
	CButton m_CPAbleButton;
	CButton m_CPUnableButton;
	CButton m_MPAbleButton;
	CButton m_MPUnableButton;

	// Type ���� ���;
	bool m_bMultiSelect;
	std::vector<PRODUCT_ITEM> m_vecProductItem;

public:
	afx_msg void OnBnClickedButtonAbleCp();
	afx_msg void OnBnClickedButtonUnableCp();
	afx_msg void OnBnClickedButtonAbleMp();
	afx_msg void OnBnClickedButtonUnableMp();
};