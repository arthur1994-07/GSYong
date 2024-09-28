#pragma once

#include "../resource.h"
#include "../../RanLogic/Activity/Activity.h"
#include "../RanLogic/Land/GLMapList.h"

class GLMapList;

// DlgSpecialActivitySet ��ȭ �����Դϴ�.

class DlgSpecialActivitySet : public CDialog
{
	DECLARE_DYNAMIC(DlgSpecialActivitySet)

public:
	DlgSpecialActivitySet(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~DlgSpecialActivitySet();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DLG_SPECIAL_ACTIVITY_SET };

public:

    void SetModify( bool bModify ) { m_bModify = bModify; }

    bool        m_bModify;
    Activity    m_Activity;

    GLMapList   m_sMapList;
    CListCtrl   m_TargetList;


    void    InitDefaultCtrls();
    
    void    InverseUpdateItems( Activity& OutActv );
    void    UpdateItems();
    void    UpdateCondition();
    void    UpdateConditionControl();
    void    UpdateActivityIDControl();

    SNATIVEID FindValidActivityID( const WORD wMid );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();

protected:
	virtual void OnOK();
public:
    afx_msg void OnCbnSelchangeComboSpecActType();
    afx_msg void OnBnClickedButtonTargetAdd();
    afx_msg void OnBnClickedButtonTargetDel();
    afx_msg void OnEnChangeRewardTitle();

};
