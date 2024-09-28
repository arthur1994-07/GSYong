#pragma once
#include "afxcmn.h"

enum VIEWTYPE
{
	VIEW_CORRECT, VIEW_CHECK, VIEW_UPDATE, VIEW_NEWENCRYPT
};

// CDifferListDlg 대화 상자입니다.

class CDifferListDlg : public CDialog
{
	DECLARE_DYNAMIC(CDifferListDlg)

public:
	CDifferListDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDifferListDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_CHECKLIST };

	void UpdateDifferList( const std::vector<SMoneyInfo>& vecGameMoneyInfo, const std::vector<SMoneyInfo>& vecSTOMoneyInfo, const VIEWTYPE emViewType );
	BOOL SaveEditList( std::vector<SMoneyInfo> vecEditChaInfo, BOOL bGameMoney = TRUE );
	CString	GetAppPath();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListCtrl m_list_GameMoney;

	std::vector<SMoneyInfo> m_vecGameMoneyInfo;
	std::vector<SMoneyInfo> m_vecSTOMoneyInfo;

	VIEWTYPE		   m_emViewType;
	bool			   m_bGameMoney;
	afx_msg void OnBnClickedButtonSave();
	CListCtrl m_list_STOMoney;
	afx_msg void OnBnClickedButtonSave2();
};
