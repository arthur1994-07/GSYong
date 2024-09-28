#pragma once
#include "afxwin.h"

#include "../EngineLib/G-Logic/GLDefine.h"
#include "../RanLogic/Land/GLMapList.h"


// CMapDialog ��ȭ �����Դϴ�.

class CMapDialog : public CDialog
{
	DECLARE_DYNAMIC(CMapDialog)

public:
	CMapDialog(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CMapDialog();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_MAPDIALOG };

public:
	SNATIVEID	m_sMapID;
	GLMapList	m_sMapList;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_cListMap;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonFind();
	afx_msg void OnBnClickedOk();
};
