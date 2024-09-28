/**
 * \date	2011/01/25
 * \author	Jun-Hyuk Choi
 */
#pragma once

class CFrame;

class CSequenceTree : public CXTTreeCtrl
{
protected:
    DECLARE_MESSAGE_MAP()

protected:
    typedef std::map< DWORD_PTR, HTREEITEM > TreeItemMap;
    typedef TreeItemMap::value_type TreeItemMapValue;
    typedef TreeItemMap::iterator TreeItemMapIter;

protected:
    CFrame* m_pFrame;
    CImageList m_cImageList;
    TreeItemMap m_mapTreeItem;

public:
    CSequenceTree();
    virtual ~CSequenceTree();

protected:
	virtual LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam );

public:
    afx_msg VOID OnSelChanged( NMHDR* pNMHDR, LRESULT* pResult );
    afx_msg VOID OnEndLabelEdit( NMHDR* pNMHDR, LRESULT* pResult );

public:
    VOID RebuildTreeControl();

public:
    virtual CFrame* GetFrame() { return m_pFrame; }
};