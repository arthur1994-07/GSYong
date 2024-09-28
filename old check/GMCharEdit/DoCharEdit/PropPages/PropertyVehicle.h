#pragma once
#include "afxwin.h"

#include "../../Database/DBBase.h"

namespace gmce
{
	class IDBManager;
}

class CPropertySheetCharEdit;
struct SITEMCUSTOM;


// CPropertyVehicle 대화 상자입니다.

class CPropertyVehicle : public CPropertyPage
{
	DECLARE_DYNAMIC(CPropertyVehicle)

public:
	CPropertyVehicle();
	virtual ~CPropertyVehicle();

	void OutputPartInfo(SITEMCUSTOM &itemcustom, CEdit &ebName, CEdit &ebMID, CEdit &ebSID);
	void OutputColorInfo(DWORD dwMainColor, DWORD dwSubColor, CEdit &ebMain, CEdit &ebSub);
	

// 대화 상자 데이터입니다.
	enum { IDD = IDD_PROPERTY_CHAR_VEHICLE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:	
	afx_msg void OnLbnSelchangeListVehicleNum();
	virtual BOOL OnInitDialog();

	gmce::IDBManager *m_pDBMan;
	gmce::VVEHICLE	  m_vecVehicles;

	CPropertySheetCharEdit* m_pPS;
	
	CListBox m_lbVehicleNum;
	CEdit m_ebName;
	CEdit m_ebType;
	CEdit m_ebBattery;
	CEdit m_ebMID;
	CEdit m_ebSID;
	CEdit m_ebBooster;
	CEdit m_ebCreateDate;
	CEdit m_ebDeleted;
	CEdit m_ebDeleteDate;
	CEdit m_ebEquip;	
	CEdit m_ebA_MID;
	CEdit m_ebA_SID;
	CEdit m_ebA_Name;
	CEdit m_ebB_Name;
	CEdit m_ebB_MID;
	CEdit m_ebB_SID;
	CEdit m_ebC_Name;
	CEdit m_ebC_MID;
	CEdit m_ebC_SID;
	CEdit m_ebD_Name;
	CEdit m_ebD_MID;
	CEdit m_ebD_SID;
	CEdit m_ebE_Name;
	CEdit m_ebE_MID;
	CEdit m_ebE_SID;
	CEdit m_ebF_Name;
	CEdit m_ebF_MID;
	CEdit m_ebF_SID;
	CEdit m_ebColorMain1;
	CEdit m_ebColorMain2;
	CEdit m_ebColorMain3;
	CEdit m_ebColorMain4;
	CEdit m_ebColorMain5;
	CEdit m_ebColorMain6;
	CEdit m_ebColorSub1;
	CEdit m_ebColorSub2;
	CEdit m_ebColorSub3;
	CEdit m_ebColorSub4;
	CEdit m_ebColorSub5;
	CEdit m_ebColorSub6;
};
