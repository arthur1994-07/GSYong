#pragma once

class CFrame;

class CEditListPage : public CXTPFrameWnd
{
protected:
	DECLARE_DYNCREATE( CEditListPage )
	DECLARE_MESSAGE_MAP()

private:
	CFrame* m_pFrame;
	CXTPListCtrl m_listCtrl;
	CXTPHeaderCtrl m_header;
    int m_nSortedCol;
    BOOL m_bAscending;
	BOOL m_bSorting;

public:
	CEditListPage();
	~CEditListPage();

protected:
	virtual LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam );
	virtual BOOL OnNotify( WPARAM wParam, LPARAM lParam, LRESULT* pResult );

protected:
	afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
	afx_msg void OnLvnChanged( NMHDR* pNMHDR, LRESULT* pResult );

public:
	void Refresh();
	void SortColumn( int nSortedCol, BOOL bAscending );

	int GetSelectItem();
	void SetSelectItem( int nItem );
    void SetSelectItem( const std::string& strItem );

public:
    virtual CFrame* GetFrame() { return m_pFrame; }
};