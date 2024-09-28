// PropertyVehicle.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../../GMCharEdit.h"

#include "../../../RanLogic/Item/GLItemCustom.h"
#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/Item/GLItem.h"
#include "../../../RanLogic/GLogicDataMan.h"

#include "../../Util/CGmCfg.h"

#include "../../Database/DatabaseTable.h"
#include "../../Database/ADO/ADOManager.h"
#include "../../Database/ODBC/CGMOdbcManager.h"

#include "../PropertySheetCharEdit.h"
#include "PropertyVehicle.h"


// CPropertyVehicle 대화 상자입니다.

IMPLEMENT_DYNAMIC(CPropertyVehicle, CPropertyPage)

CPropertyVehicle::CPropertyVehicle()
	: CPropertyPage(CPropertyVehicle::IDD)	
	, m_pPS(NULL)
{
	m_pDBMan = CGmCfg::GetInstance()->GetDBMan();//CADOManager::GetInstance();
}

CPropertyVehicle::~CPropertyVehicle()
{
}

void CPropertyVehicle::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_VEHICLE_NUM, m_lbVehicleNum);
	DDX_Control(pDX, IDC_EDIT_VEHICLE_NAME, m_ebName);	
	DDX_Control(pDX, IDC_EDIT_VEHICLE_TYPE, m_ebType);
	DDX_Control(pDX, IDC_EDIT_VEHICLE_BATTERY, m_ebBattery);
	DDX_Control(pDX, IDC_EDIT_VEHICLE_MID, m_ebMID);
	DDX_Control(pDX, IDC_EDIT_VEHICLE_SID, m_ebSID);
	DDX_Control(pDX, IDC_EDIT_VEHICLE_BOOSTER, m_ebBooster);
	DDX_Control(pDX, IDC_EDIT_VEHICLE_CREATEDATE, m_ebCreateDate);
	DDX_Control(pDX, IDC_EDIT_VEHICLE_DELETED, m_ebDeleted);
	DDX_Control(pDX, IDC_EDIT_VEHICLE_DELETEDATE, m_ebDeleteDate);
	DDX_Control(pDX, IDC_EDIT_VEHICLE_EQUIP, m_ebEquip);	
	DDX_Control(pDX, IDC_EDIT_VEHICLE_PARTA_MID, m_ebA_MID);
	DDX_Control(pDX, IDC_EDIT_VEHICLE_PARTA_SID, m_ebA_SID);
	DDX_Control(pDX, IDC_EDIT_VEHICLE_PARTA_NAME, m_ebA_Name);
	DDX_Control(pDX, IDC_EDIT_VEHICLE_PARTB_NAME, m_ebB_Name);
	DDX_Control(pDX, IDC_EDIT_VEHICLE_PARTB_MID, m_ebB_MID);
	DDX_Control(pDX, IDC_EDIT_VEHICLE_PARTB_SID, m_ebB_SID);
	DDX_Control(pDX, IDC_EDIT_VEHICLE_PARTC_NAME, m_ebC_Name);
	DDX_Control(pDX, IDC_EDIT_VEHICLE_PARTC_MID, m_ebC_MID);
	DDX_Control(pDX, IDC_EDIT_VEHICLE_PARTC_SID, m_ebC_SID);
	DDX_Control(pDX, IDC_EDIT_VEHICLE_PARTD_NAME, m_ebD_Name);
	DDX_Control(pDX, IDC_EDIT_VEHICLE_PARTD_MID, m_ebD_MID);
	DDX_Control(pDX, IDC_EDIT_VEHICLE_PARTD_SID, m_ebD_SID);
	DDX_Control(pDX, IDC_EDIT_VEHICLE_PARTE_NAME, m_ebE_Name);
	DDX_Control(pDX, IDC_EDIT_VEHICLE_PARTE_MID, m_ebE_MID);
	DDX_Control(pDX, IDC_EDIT_VEHICLE_PARTE_SID, m_ebE_SID);
	DDX_Control(pDX, IDC_EDIT_VEHICLE_PARTF_NAME, m_ebF_Name);
	DDX_Control(pDX, IDC_EDIT_VEHICLE_PARTF_MID, m_ebF_MID);
	DDX_Control(pDX, IDC_EDIT_VEHICLE_PARTF_SID, m_ebF_SID);
	DDX_Control(pDX, IDC_EDIT_VEHICLE_PARTA_NAME2, m_ebColorMain1);
	DDX_Control(pDX, IDC_EDIT_VEHICLE_PARTA_MID2, m_ebColorMain2);
	DDX_Control(pDX, IDC_EDIT_VEHICLE_PARTA_SID2, m_ebColorMain3);
	DDX_Control(pDX, IDC_EDIT_VEHICLE_PARTA_NAME3, m_ebColorMain4);
	DDX_Control(pDX, IDC_EDIT_VEHICLE_PARTA_MID3, m_ebColorMain5);
	DDX_Control(pDX, IDC_EDIT_VEHICLE_PARTA_SID3, m_ebColorMain6);
	DDX_Control(pDX, IDC_EDIT_VEHICLE_PARTB_NAME2, m_ebColorSub1);
	DDX_Control(pDX, IDC_EDIT_VEHICLE_PARTB_MID2, m_ebColorSub2);
	DDX_Control(pDX, IDC_EDIT_VEHICLE_PARTB_SID2, m_ebColorSub3);
	DDX_Control(pDX, IDC_EDIT_VEHICLE_PARTB_NAME3, m_ebColorSub4);
	DDX_Control(pDX, IDC_EDIT_VEHICLE_PARTB_MID3, m_ebColorSub5);
	DDX_Control(pDX, IDC_EDIT_VEHICLE_PARTB_SID3, m_ebColorSub6);
}


BEGIN_MESSAGE_MAP(CPropertyVehicle, CPropertyPage)
	ON_LBN_SELCHANGE(IDC_LIST_VEHICLE_NUM, &CPropertyVehicle::OnLbnSelchangeListVehicleNum)
END_MESSAGE_MAP()


// CPropertyVehicle 메시지 처리기입니다.

BOOL CPropertyVehicle::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	//CPropertySheetCharEdit* pSheet = (CPropertySheetCharEdit *) GetParent();

	 m_pPS = (CPropertySheetCharEdit *) GetParent();

	 if (m_pPS == NULL)
	 {
		 return FALSE;
	 }


	 GLCHARLOGIC* pChar = m_pPS->m_pChar;

	 if (pChar == NULL)
	 {
		 return FALSE;
	 }

	 SITEMCUSTOM& itemcustomVehicle = pChar->m_PutOnItems[SLOT_VEHICLE];
	 SNATIVEID idEquip = itemcustomVehicle.GetNativeID();
	 BOOL bEquipped = idEquip == NATIVEID_NULL() ? FALSE : TRUE;

	 if (bEquipped == FALSE)
	 {
		 m_ebEquip.SetWindowText(_T("None"));
	 }
	
	if( m_pDBMan->GetVehicleInfo(m_vecVehicles, m_pPS->m_nChaNum) == DB_OK )
	{
		gmce::ITER_VVEHICLE iter;
		gmce::VEHICLEINFO aVEHICLE;

		for ( iter = m_vecVehicles.begin(); iter != m_vecVehicles.end(); iter++)
		{
			aVEHICLE = *(iter);

			CString csNum;
			csNum.Format(_T("%d"), aVEHICLE.nNum );
			
			if (idEquip == aVEHICLE.glvehicle.m_sVehicleID)
			{
				m_ebEquip.SetWindowText(csNum.GetBuffer());
				csNum.Append(_T("(*)"));
			}

			m_lbVehicleNum.AddString(csNum.GetBuffer());
		}

		m_lbVehicleNum.SetCurSel(0);
		OnLbnSelchangeListVehicleNum();
	}



	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CPropertyVehicle::OnLbnSelchangeListVehicleNum()
{
	int idx = m_lbVehicleNum.GetCurSel();

	if (idx == LB_ERR )	// the listbox is empty.
		return;

	gmce::VEHICLEINFO aVEHICLE = m_vecVehicles[idx];
	
	m_ebName.SetWindowText(aVEHICLE.csName.GetBuffer());
	
	
	CString csType;
	CString csBattery;
	CString csMID;
	CString csSID;
	CString csBooster;
	CString csCreateDate;
	CString csDeleted;
	CString csDeleteDate;

	switch( aVEHICLE.glvehicle.m_emTYPE )
	{
	case VEHICLE_TYPE_BIKE:
		{
			csType = _T("BIKE");
		}
		break;

	case VEHICLE_TYPE_BOARD:
		{
			csType = _T("BOARD");
		}
		break;

	default:
		{
			csType = _T("WrongType");
		}
		break;
	}	
	m_ebType.SetWindowText(csType.GetBuffer());


	csBattery.Format(_T("%d"), aVEHICLE.glvehicle.m_nFull);
	m_ebBattery.SetWindowText(csBattery.GetBuffer());


	csMID.Format(_T("%d"), (int)(aVEHICLE.glvehicle.m_sVehicleID.wMainID));
	m_ebMID.SetWindowText( csMID.GetBuffer() );

	csSID.Format(_T("%d"), (int)(aVEHICLE.glvehicle.m_sVehicleID.wSubID));
	m_ebSID.SetWindowText( csSID.GetBuffer() );

	csBooster.Format(_T("%d"), (int)(aVEHICLE.glvehicle.m_dwBoosterID));
	m_ebBooster.SetWindowText(csBooster.GetBuffer());

	csCreateDate = aVEHICLE.ctCreateDate.Format(_T("%Y-%m-%d %H:%M:%S"));
	m_ebCreateDate.SetWindowText(csCreateDate.GetBuffer());

	csDeleted = aVEHICLE.bDeleted ? _T("DELETED") : _T("NOT Deleted");
	m_ebDeleted.SetWindowText(csDeleted.GetBuffer());

	if( aVEHICLE.bDeleted )
	{
		csDeleteDate = aVEHICLE.ctDeleteDate.Format(_T("%Y-%m-%d %H:%M:%S"));
	}
	else
	{
		csDeleteDate = _T("");
	}
	m_ebDeleteDate.SetWindowText(csDeleteDate.GetBuffer());

	
	
	//
	// output Parts information
	//	
	SITEMCUSTOM itemcustomA = aVEHICLE.glvehicle.m_PutOnItems[ACCE_TYPE_PARTS_A];	
	OutputPartInfo(itemcustomA, m_ebA_Name, m_ebA_MID, m_ebA_SID);

	SITEMCUSTOM itemcustomB = aVEHICLE.glvehicle.m_PutOnItems[ACCE_TYPE_PARTS_B];
	OutputPartInfo(itemcustomB, m_ebB_Name, m_ebB_MID, m_ebB_SID);

	SITEMCUSTOM itemcustomC = aVEHICLE.glvehicle.m_PutOnItems[ACCE_TYPE_PARTS_C];	
	OutputPartInfo(itemcustomC, m_ebC_Name, m_ebC_MID, m_ebC_SID);

	SITEMCUSTOM itemcustomD = aVEHICLE.glvehicle.m_PutOnItems[ACCE_TYPE_PARTS_D];	
	OutputPartInfo(itemcustomD, m_ebD_Name, m_ebD_MID, m_ebD_SID);

	SITEMCUSTOM itemcustomE = aVEHICLE.glvehicle.m_PutOnItems[ACCE_TYPE_PARTS_E];	
	OutputPartInfo(itemcustomE, m_ebE_Name, m_ebE_MID, m_ebE_SID);

	SITEMCUSTOM itemcustomF = aVEHICLE.glvehicle.m_PutOnItems[ACCE_TYPE_PARTS_F];	
	OutputPartInfo(itemcustomF, m_ebF_Name, m_ebF_MID, m_ebF_SID);


	//
	// output Color information
	//
	SVEHICLE_COLOR color1 = aVEHICLE.glvehicle.m_sColor[1];
	OutputColorInfo(color1.dwMainColor, color1.dwSubColor, m_ebColorMain1, m_ebColorSub1);

	SVEHICLE_COLOR color2 = aVEHICLE.glvehicle.m_sColor[2];
	OutputColorInfo(color2.dwMainColor, color2.dwSubColor, m_ebColorMain2, m_ebColorSub2);

	SVEHICLE_COLOR color3 = aVEHICLE.glvehicle.m_sColor[3];
	OutputColorInfo(color3.dwMainColor, color3.dwSubColor, m_ebColorMain3, m_ebColorSub3);

	SVEHICLE_COLOR color4 = aVEHICLE.glvehicle.m_sColor[4];
	OutputColorInfo(color4.dwMainColor, color4.dwSubColor, m_ebColorMain4, m_ebColorSub4);

	SVEHICLE_COLOR color5 = aVEHICLE.glvehicle.m_sColor[5];
	OutputColorInfo(color5.dwMainColor, color5.dwSubColor, m_ebColorMain5, m_ebColorSub5);

	SVEHICLE_COLOR color6 = aVEHICLE.glvehicle.m_sColor[6];
	OutputColorInfo(color6.dwMainColor, color6.dwSubColor, m_ebColorMain6, m_ebColorSub6);
}


void CPropertyVehicle::OutputPartInfo(SITEMCUSTOM &itemcustom, CEdit &ebName, CEdit &ebMID, CEdit &ebSID)
{
	CString csName;
	CString csMID;
	CString csSID;

	SITEM* pItem = NULL;

	if (itemcustom.GetNativeID() != NATIVEID_NULL())
	{
		csMID.Format(_T("%d"), (int)itemcustom.Mid());
		ebMID.SetWindowText(csMID.GetBuffer());

		csSID.Format(_T("%d"), (int)itemcustom.Sid());
		ebSID.SetWindowText(csSID.GetBuffer());

		pItem = GLogicData::GetInstance().GetItem (itemcustom.GetNativeID());
		if (pItem)
		{
			const char *pName = pItem->GetName();
			ebName.SetWindowText(pName);
		}
	}
	else
	{
		ebName.SetWindowText(_T("None"));
		ebMID.SetWindowText(_T("None"));
		ebSID.SetWindowText(_T("None"));		
	}
}


void CPropertyVehicle::OutputColorInfo(DWORD dwMainColor, DWORD dwSubColor, CEdit &ebMain, CEdit &ebSub)
{
	if (dwMainColor != 0 && dwSubColor != 0)
	{
		CString Main;
		CString Sub;

		int	R,G,B;

		R = (dwMainColor&0xff0000)>>16;
		G = (dwMainColor&0xff00)>>8;
		B = dwMainColor&0xff;

		Main.Format(_T("(%.3d,%.3d,%.3d)"), R,G,B);
		ebMain.SetWindowText(Main.GetBuffer());


		R = (dwSubColor&0xff0000)>>16;
		G = (dwSubColor&0xff00)>>8;
		B = dwSubColor&0xff;

		Sub.Format(_T("(%.3d,%.3d,%.3d)"), R,G,B);
		ebSub.SetWindowText(Sub.GetBuffer());
	}
	else
	{
		ebMain.SetWindowText(_T("none"));
		ebSub.SetWindowText(_T("none"));
	}
}

