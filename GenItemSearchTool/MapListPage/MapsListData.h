#pragma once

#include "../../RanLogic/Land/GLMapList.h"

// MapsListData 대화 상자입니다.

extern std::string strInstanceType[SMAPNODE::EMINSTANCE_NSIZE];


class CMapsListData : public CDialog
{
	DECLARE_DYNAMIC(CMapsListData)

public:
	CMapsListData(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CMapsListData();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_MAPSLISTDIALOG };

public:
	SMAPNODE m_sMapNode;
	CComboBox	m_comboInstanceType;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();

protected:
	virtual void OnOK();
};
