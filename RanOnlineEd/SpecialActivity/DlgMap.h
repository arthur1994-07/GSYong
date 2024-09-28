#pragma once
#include "afxwin.h"

#include "../../EngineLib/G-Logic/GLDefine.h"

class GLMapList;

// CDlgMap ��ȭ �����Դϴ�.

class CDlgMap : public CDialog
{
	DECLARE_DYNAMIC(CDlgMap)

public:
	SNATIVEID	m_nidMAP;

public:
	CDlgMap(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgMap();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_MAP };

    GLMapList* m_pMapList;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
    void SelectItem( SNATIVEID sID );
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CListBox m_listMAP;
	afx_msg void OnBnClickedButtonFind();
};
