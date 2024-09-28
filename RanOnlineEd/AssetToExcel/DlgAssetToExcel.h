#pragma once


// DlgAssetToExcel 대화 상자입니다.

class DlgAssetToExcel : public CDialog
{
	DECLARE_DYNAMIC(DlgAssetToExcel)

public:
	DlgAssetToExcel(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~DlgAssetToExcel();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_ASSET_TO_EXCEL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
    void AssetToExcelThreadProc();

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonAssetToExcelStart();
};
