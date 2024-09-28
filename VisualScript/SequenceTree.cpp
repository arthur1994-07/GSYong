#include "stdafx.h"
#include "../InternalCommonLib/dxstdafx.h"

#include "./Frame.h"
#include "./Editor.h"
#include "./SequenceTree.h"

#include "../SigmaCore/DebugInclude.h"

BEGIN_MESSAGE_MAP( CSequenceTree, CXTTreeCtrl )
	ON_NOTIFY_REFLECT( TVN_SELCHANGED, OnSelChanged )
    ON_NOTIFY_REFLECT( TVN_ENDLABELEDIT, OnEndLabelEdit )
END_MESSAGE_MAP()

CSequenceTree::CSequenceTree()
    : m_pFrame( NULL )
{
}

CSequenceTree::~CSequenceTree()
{
}

LRESULT CSequenceTree::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT lr = CXTTreeCtrl::WindowProc( message, wParam, lParam );

    switch( message )
    {
    case WM_CREATE:
        {
            m_pFrame = DYNAMIC_DOWNCAST( CFrame, GetParent() );

            CBitmap cBitmap;
            cBitmap.LoadBitmap( IDB_TREE_SEQUENCE );

            m_cImageList.Create( 16, 16, ILC_COLOR24 | ILC_MASK, 2, 1 );
            m_cImageList.Add( &cBitmap, RGB( 0, 255, 0 ) );
            SetImageList( &m_cImageList, TVSIL_NORMAL );
        }
        break;

    case WM_ACTIVATE:
    case WM_SETFOCUS:
    case WM_SHOWWINDOW:
    case WM_MOUSEACTIVATE:
    case WM_CHILDACTIVATE:
        {
             if( m_pFrame )
                m_pFrame->ChangeActiveView( GetSafeHwnd() );
        }
        break;
    }

    return lr;
}

VOID CSequenceTree::OnSelChanged( NMHDR* pNMHDR, LRESULT* pResult )
{
    NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
    UNREFERENCED_PARAMETER( pNMTreeView );

    DWORD_PTR pItem = GetItemData( pNMTreeView->itemNew.hItem );
    m_pFrame->ChangeActiveSequnce( reinterpret_cast< vs::CSequence* >( pItem ) );
    m_pFrame->RefreshPropertiesControl();

    *pResult = 0;
}

VOID CSequenceTree::OnEndLabelEdit( NMHDR* pNMHDR, LRESULT* pResult )
{
    if( !g_pEditor )
        return;

    TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;

    if( pTVDispInfo->item.pszText )
    {
        g_pEditor->m_pSequence->strName = pTVDispInfo->item.pszText;
        SetItemText( pTVDispInfo->item.hItem, g_pEditor->m_pSequence->strName.c_str() );
    }

	*pResult = 0;
}

VOID CSequenceTree::RebuildTreeControl()
{
    if( !g_pEditor )
        return;

    DeleteAllItems();
    m_mapTreeItem.clear();

    HTREEITEM hItem = InsertItem( g_pEditor->m_pSequenceRoot->strName.c_str(), 1, 0 );
    SetItemData( hItem, reinterpret_cast< DWORD_PTR >( g_pEditor->m_pSequenceRoot ) );
    m_mapTreeItem.insert( std::make_pair( g_pEditor->m_pSequenceRoot->GetIdentity(), hItem ) );

    vs::SequenceObjectVec vecSequences;
    g_pEditor->m_pSequenceRoot->FindObjectByClass( vs::CSequence::Static(), vecSequences );

    BOOST_FOREACH( vs::CSequenceObject* pObject, vecSequences )
    {
        vs::CSequence* pSequence = dynamic_cast< vs::CSequence* >( pObject );
        vs::CSequence* pParent = dynamic_cast< vs::CSequence* >( pSequence->GetParent() );
        TreeItemMapIter it = m_mapTreeItem.find( pParent->GetIdentity() );

        hItem = InsertItem( pSequence->strName.c_str(), 1, 0, it->second );
        SetItemData( hItem, reinterpret_cast< DWORD_PTR >( pSequence ) );
        m_mapTreeItem.insert( std::make_pair( pSequence->GetIdentity(), hItem ) );
    }

    BOOST_FOREACH( TreeItemMapValue& it, m_mapTreeItem )
        SortChildren( it.second );

    TreeItemMapIter it = m_mapTreeItem.find( g_pEditor->m_pSequence->GetIdentity() );
    Expand( it->second, TVE_EXPAND );
    SelectItem( it->second );
}