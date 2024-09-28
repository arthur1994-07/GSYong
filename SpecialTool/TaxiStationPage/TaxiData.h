#pragma once

#include "../Resource.h"
#include "../../RanLogic/Transport/GLTaxiStation.h"

// CTaxiData ��ȭ �����Դϴ�.

class CTaxiData : public CDialog
{
	DECLARE_DYNAMIC(CTaxiData)

public:
	CTaxiData(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CTaxiData();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_TAXIDATA };

public:
	STAXI_STATION	m_sTaxiStation;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonMapEdit();
	afx_msg void OnBnClickedButtonNpcEdit();
	afx_msg void OnBnClickedOk();
};
