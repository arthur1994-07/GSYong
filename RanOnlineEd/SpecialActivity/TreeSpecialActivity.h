#pragma once
#include "afxcmn.h"
#include "../resource.h"
#include <string>

#include "../../RanLogic/Activity/Activity.h"

// CQuestTree 대화 상자입니다.
class	CsheetWithTab;
class TreeSpecialActivity : public CPropertyPage
{
	DECLARE_DYNAMIC(TreeSpecialActivity)

public:
	TreeSpecialActivity( LOGFONT logfont );
	virtual ~TreeSpecialActivity();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SPECIAL_ACTIVITY_TREE };
private:
	CsheetWithTab*	m_pSheetTab;
	BOOL			m_bDlgInit;
	HTREEITEM		m_TreeRoot;
    CFont*			m_pFont;
    CEdit			m_ebSalesFilename;

    SNATIVEID       m_ItemNodeID;
    SNATIVEID	    m_DummyItemNodeID;
    BOOL        	m_bDummyHasItem;

    ENUM_ACTIVITY_CLASS m_ActivityClass;

    CTreeCtrl m_ctrlSpcialTree;



public:
	void	SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }

public:
    void    InitData( ENUM_ACTIVITY_CLASS _ActivityClass );

    void        UpdateName ();
    void        UpdateSalesList ();
    void        AddItem();
    void        ModifyItem();
    void        DelItem ();
    void        CopyItem ();
    void        PasteItemNew ();
    
    HRESULT     UpdateTree();
    void        CleanAllItem();
    std::string	GetFormatName ( Activity* pActivity );
    int         GetSelectMenu ();


    BOOL	    ISROOT ();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    virtual BOOL OnSetActive();
    afx_msg void OnNMDblclkTreeItem(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMRclickTreeItem(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMKillFocusCrowsalesEditbox();
};

