// DlgAssetToExcel.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include <boost/thread.hpp>
#include "../RanOnlineEd.h"
#include "DlgAssetToExcel.h"


// DlgAssetToExcel 대화 상자입니다.

IMPLEMENT_DYNAMIC(DlgAssetToExcel, CDialog)

DlgAssetToExcel::DlgAssetToExcel(CWnd* pParent /*=NULL*/)
	: CDialog(DlgAssetToExcel::IDD, pParent)
{

}

DlgAssetToExcel::~DlgAssetToExcel()
{
}

void DlgAssetToExcel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DlgAssetToExcel, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_ASSET_TO_EXCEL_START, &DlgAssetToExcel::OnBnClickedButtonAssetToExcelStart)
END_MESSAGE_MAP()


// DlgAssetToExcel 메시지 처리기입니다.

void DlgAssetToExcel::OnBnClickedButtonAssetToExcelStart()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    boost::thread AssetToExcelThread(boost::bind(&DlgAssetToExcel::AssetToExcelThreadProc, this));
}

void DlgAssetToExcel::AssetToExcelThreadProc()
{

}