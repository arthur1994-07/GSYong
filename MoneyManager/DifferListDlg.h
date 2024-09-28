#pragma once
#include "afxcmn.h"

enum VIEWTYPE
{
	VIEW_CORRECT, VIEW_CHECK, VIEW_UPDATE, VIEW_NEWENCRYPT
};

// CDifferListDlg ��ȭ �����Դϴ�.

class CDifferListDlg : public CDialog
{
	DECLARE_DYNAMIC(CDifferListDlg)

public:
	CDifferListDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDifferListDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_CHECKLIST };

	void UpdateDifferList( const std::vector<SMoneyInfo>& vecGameMoneyInfo, const std::vector<SMoneyInfo>& vecSTOMoneyInfo, const VIEWTYPE emViewType );
	BOOL SaveEditList( std::vector<SMoneyInfo> vecEditChaInfo, BOOL bGameMoney = TRUE );
	CString	GetAppPath();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
