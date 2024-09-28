#pragma once
#include "afxwin.h"

struct DXLANDEFF;

// CDlgEffectList ��ȭ �����Դϴ�.

class CDlgEffectList : public CDialog
{
	DECLARE_DYNAMIC(CDlgEffectList)

public:
	CDlgEffectList(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgEffectList();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_EFFECTLIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
    DXLANDEFF* m_pSelEff;

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    CListBox m_kEffListBox;
    DXLANDEFF* GetSelEffect();
    afx_msg void OnBnClickedOk();
};
