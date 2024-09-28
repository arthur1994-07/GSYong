#pragma once
#include "afxcmn.h"
#include "../resource.h"
#include "../../RanLogic/Item/GLItemMan.h"

// CItemTreePage ��ȭ �����Դϴ�.
class	CsheetWithTab;
class CItemTreePage : public CPropertyPage
{
	DECLARE_DYNAMIC(CItemTreePage)

public:
	CItemTreePage();
	virtual ~CItemTreePage();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_ITEMTREE };

protected:
	CsheetWithTab*	m_pSheetTab;
	BOOL			m_bDlgInit;	

public:
	BOOL	UpdateTreeItem ();

public:
	void	SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }
	void	SetPropertyData ( int CalledPage, SNATIVEID NativeID, int iBtnNum );
	void	CleanAllItem ();

protected:
	BOOL		m_bNativeIDInit;
	int			m_CalledPage;	
	int			m_BtnCallNum;
	SNATIVEID	m_NativeID;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_ctrlItemTree;
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedButtonOk();
};
