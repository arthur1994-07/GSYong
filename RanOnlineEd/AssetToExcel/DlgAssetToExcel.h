#pragma once


// DlgAssetToExcel ��ȭ �����Դϴ�.

class DlgAssetToExcel : public CDialog
{
	DECLARE_DYNAMIC(DlgAssetToExcel)

public:
	DlgAssetToExcel(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~DlgAssetToExcel();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_ASSET_TO_EXCEL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
    void AssetToExcelThreadProc();

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonAssetToExcelStart();
};
