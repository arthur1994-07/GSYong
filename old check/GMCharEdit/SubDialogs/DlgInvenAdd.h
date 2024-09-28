#pragma once
#include "afxcmn.h"

#include <vector>

#include "../MainFrm.h"
#include "../../RanLogic/Item/GLItem.h"

// CDlgInvenAdd ��ȭ �����Դϴ�.

enum DIALOGUE_TYPE
{
    INVEN_ADD,
    GEN_TYPE,
    EXCHANGE_FLAG,
    EXCHANGE_ITEM_HISTORY,
};

class CDlgInvenAdd : public CDialog
{
	DECLARE_DYNAMIC(CDlgInvenAdd)

public:
	CDlgInvenAdd(int _Type = INVEN_ADD, CWnd* pParent = NULL, std::vector<ExchangeItemHistory>* InExChangeHistory = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgInvenAdd();

private:
    int Type;

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DLG_INVEN_ADD };
public:
	void InitListHead();
    void SetData();
    void SetGenTypeData();
    void SetExchangeFlagData();
    void SetExchangeItemHistoryData( const std::vector<ExchangeItemHistory>& InExChangeHistory );

	std::vector<SITEM> m_vItem;
    std::vector<int> m_vSelected;
	int	m_nSelected;
    std::vector<ExchangeItemHistory> vecExchangeItemHistory;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListCtrl m_ListInvenAdd;
	afx_msg void OnBnClickedOk();
	afx_msg void OnNMDblclkListSkillAdd(NMHDR *pNMHDR, LRESULT *pResult);
};
