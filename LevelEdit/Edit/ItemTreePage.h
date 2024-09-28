#pragma once
#include "afxcmn.h"
#include "../resource.h"
#include "../../RanLogic/Item/GLItemMan.h"

// CItemTreePage 대화 상자입니다.
class	CsheetWithTab;
class CItemTreePage : public CPropertyPage
{
	DECLARE_DYNAMIC(CItemTreePage)

public:
	CItemTreePage();
	virtual ~CItemTreePage();

// 대화 상자 데이터입니다.
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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_ctrlItemTree;
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedButtonOk();
};
