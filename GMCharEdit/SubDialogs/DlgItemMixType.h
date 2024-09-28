#pragma once

#include "../resource.h"
#include "afxwin.h"

#include "../DataManager/CDataManagerProduct.h"

class CDlgItemMixType : public CDialog
{
	DECLARE_DYNAMIC( CDlgItemMixType )

public:
	CDlgItemMixType( CWnd* pParent = NULL );
	virtual ~CDlgItemMixType();

	enum { IDD = IDD_DLG_ITEMMIX_TYPE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

public:
	// �⺻ ����;
	void UpdateDefaultProductType();

	// ������ Ÿ�� ����� ��� ����� ���;
	void UpdateLearnAllType();

	// Type�� ���� ����;
	void UpdateProductType( DWORD dwProductType );

	// Multi Select �� ���� �����̳� ����;
	void PushProductType( DWORD dwProductType );
	void DelProductType( DWORD dwProductType );
	void ClearProductType();
	bool GetProductType( DWORD dwProductType );

public:
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

private:
	CEdit m_viewType;
	CEdit m_viewTypeName;
	CEdit m_viewTypes[ Product::ETypeTotal ];
	CEdit m_viewLearns[ Product::ETypeTotal ];
	CEdit m_viewCPs[ Product::ETypeTotal ];
	CEdit m_viewCP;
	CButton m_checkLearn;
	CEdit m_viewMaxWS;
	CEdit m_viewLearnCount;

	// Type ���� ���;
	bool m_bMultiSelect;
	std::vector<DWORD> m_vecProductType;

public:
	afx_msg void OnBnClickedCheckLearnType();
};