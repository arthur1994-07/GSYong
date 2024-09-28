/**
 * \date	2011/04/19
 * \author	Jun-Hyuk Choi
 */
#pragma once

namespace ic
{
    class CBaseObject;
}

class CEnumPropertyDlg : public CDialog
{
protected:
	DECLARE_DYNAMIC( CEnumPropertyDlg )
    DECLARE_MESSAGE_MAP()

private:
    CXTPListCtrl m_listCtrl;
    CXTPHeaderCtrl m_header;
    INT m_nSortedCol;
    BOOL m_bAscending;

public:
    ic::CBaseObject* m_pObject;
    TSTRING m_strKey;

public:
	CEnumPropertyDlg( CWnd* pParent = NULL );
    virtual ~CEnumPropertyDlg();

public:
    VOID DoModalLess();

protected:
	virtual VOID DoDataExchange( CDataExchange* pDX );
    virtual BOOL OnInitDialog();
    virtual BOOL OnNotify( WPARAM wParam, LPARAM lParam, LRESULT* pResult );

public:
    afx_msg VOID OnClickList( NMHDR* pNMHDR, LRESULT* pResult );

protected:
    VOID SortColumn( INT nSortedCol, BOOL bAscending );
};