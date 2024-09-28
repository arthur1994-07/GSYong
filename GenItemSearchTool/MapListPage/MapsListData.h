#pragma once

#include "../../RanLogic/Land/GLMapList.h"

// MapsListData ��ȭ �����Դϴ�.

extern std::string strInstanceType[SMAPNODE::EMINSTANCE_NSIZE];


class CMapsListData : public CDialog
{
	DECLARE_DYNAMIC(CMapsListData)

public:
	CMapsListData(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CMapsListData();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_MAPSLISTDIALOG };

public:
	SMAPNODE m_sMapNode;
	CComboBox	m_comboInstanceType;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();

protected:
	virtual void OnOK();
};
