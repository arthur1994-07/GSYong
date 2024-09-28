// ItemDataMergerToolDlg.h : ��� ����
//

#pragma once

#include "afxcmn.h"
#include "../Enginelib/ServiceProviderDefine.h"
#include "../RanLogic/Item/GLItemMan.h"

class EngineDeviceMan;
class SUBPATH;

// CItemDataMergerToolDlg ��ȭ ����
class CItemDataMergerToolDlg : public CDialog
{
// ����
public:
	CItemDataMergerToolDlg(CWnd* pParent = NULL);	// ǥ�� ������
    virtual ~CItemDataMergerToolDlg();

// ��ȭ ���� ������
	enum { IDD = IDD_ITEMDATAMERGERTOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ����

	char			m_szAppPath[MAX_PATH];
	CString			m_strAppPath;

	HTREEITEM		m_TreeRoot1;
	HTREEITEM		m_TreeRoot2;
	
	GLItemMan		*m_pItemManData1;
	GLItemMan		*m_pItemManData2;

	std::vector<DWORD> m_vecCheckItemList;

protected:
    EngineDeviceMan* m_pEngineDevice;
    SUBPATH* m_pPath;
    EMSERVICE_PROVIDER m_ServiceProvider;

// ����
protected:
	HICON m_hIcon;

	// �޽��� �� �Լ��� �����߽��ϴ�.
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonLoad1();
	afx_msg void OnBnClickedButtonLoad2();

	afx_msg void OnDestroy();

	void CleanAllItem( BOOL bType1 );
	BOOL UpdateTree( BOOL bType1 );
	void SearchCheckItem( CTreeCtrl *pTreeCtrl, HTREEITEM hRootItem );

	CTreeCtrl m_ctrlItemTree1;
	CTreeCtrl m_ctrlItemTree2;

	const char* GetFormatName ( SITEM* pItem );
	afx_msg void OnBnClickedButtonMerger();
	afx_msg void OnBnClickedButtonMerger2();
	afx_msg void OnBnClickedBtnCsvSave1();
	afx_msg void OnBnClickedBtnCsvLoad1();
	afx_msg void OnBnClickedBtnCsvSave2();
	afx_msg void OnBnClickedBtnCsvLoad2();
	afx_msg void OnBnClickedButtonSave1();
	afx_msg void OnBnClickedButtonSave2();
	afx_msg void OnBnClickedButtonNew1();
	afx_msg void OnBnClickedButtonNew2();
};
