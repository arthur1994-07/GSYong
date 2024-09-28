#pragma once
#include "afxwin.h"

struct DXLANDEFF;

// CDlgEffectList 대화 상자입니다.

class CDlgEffectList : public CDialog
{
	DECLARE_DYNAMIC(CDlgEffectList)

public:
	CDlgEffectList(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgEffectList();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_EFFECTLIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
    DXLANDEFF* m_pSelEff;

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    CListBox m_kEffListBox;
    DXLANDEFF* GetSelEffect();
    afx_msg void OnBnClickedOk();
};
