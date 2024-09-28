#pragma once
#include "afxcmn.h"
#include "../resource.h"
#include "../../RanLogic/Crow/GLCrowData.h"

// CCrowTree 대화 상자입니다.
class	CsheetWithTab;
class CCrowTree : public CPropertyPage
{
	DECLARE_DYNAMIC(CCrowTree)

public:
	CCrowTree();   // 표준 생성자입니다.
	virtual ~CCrowTree();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_CROWTREE };

protected:
	CsheetWithTab*	m_pSheetTab;
	BOOL			m_bDlgInit;	

public:
	BOOL	UpdateTreeItem ();

public:
	void	SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }
	void	SetPropertyData ( int CalledPage, SNATIVEID NativeID );
	void	CleanAllItem ();

protected:
	BOOL		m_bNativeIDInit;
	int			m_CalledPage;	
	SNATIVEID	m_NativeID;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_ctrlItemTree;
	virtual BOOL OnInitDialog();	
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonCancel();
};