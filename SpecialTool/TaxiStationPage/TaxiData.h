#pragma once

#include "../Resource.h"
#include "../../RanLogic/Transport/GLTaxiStation.h"

// CTaxiData 대화 상자입니다.

class CTaxiData : public CDialog
{
	DECLARE_DYNAMIC(CTaxiData)

public:
	CTaxiData(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTaxiData();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TAXIDATA };

public:
	STAXI_STATION	m_sTaxiStation;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonMapEdit();
	afx_msg void OnBnClickedButtonNpcEdit();
	afx_msg void OnBnClickedOk();
};
