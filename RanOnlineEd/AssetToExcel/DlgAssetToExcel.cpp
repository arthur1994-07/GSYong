// DlgAssetToExcel.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include <boost/thread.hpp>
#include "../RanOnlineEd.h"
#include "DlgAssetToExcel.h"


// DlgAssetToExcel ��ȭ �����Դϴ�.

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


// DlgAssetToExcel �޽��� ó�����Դϴ�.

void DlgAssetToExcel::OnBnClickedButtonAssetToExcelStart()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    boost::thread AssetToExcelThread(boost::bind(&DlgAssetToExcel::AssetToExcelThreadProc, this));
}

void DlgAssetToExcel::AssetToExcelThreadProc()
{

}