#include "stdafx.h"

#include <boost/foreach.hpp>
#include "./Frame.h"
#include "./Editor.h"
#include "./EditListPage.h"

#include "../../SigmaCore/DebugInclude.h"

IMPLEMENT_DYNCREATE( CEditListPage, CFrameWnd )

BEGIN_MESSAGE_MAP( CEditListPage, CFrameWnd )
	ON_WM_CREATE()
	ON_NOTIFY( LVN_ITEMCHANGED, AFX_IDW_PANE_FIRST, OnLvnChanged )
END_MESSAGE_MAP()

CEditListPage::CEditListPage()
	: m_pFrame( NULL )
	, m_nSortedCol( 0 )
    , m_bAscending( TRUE )
	, m_bSorting( FALSE )
{
}

CEditListPage::~CEditListPage()
{
}

LRESULT CEditListPage::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT lr = CWnd::WindowProc( message, wParam, lParam );

    switch( message )
    {
    case WM_CREATE:
        {
            m_pFrame = DYNAMIC_DOWNCAST( CFrame, GetParent() );
        }
        break;

    case WM_SHOWWINDOW:
        {
            if( m_pFrame )
                m_pFrame->ChangeActiveView( GetSafeHwnd() );
        }
        break;

    case WM_ACTIVATE:
    case WM_SETFOCUS:
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

BOOL CEditListPage::OnNotify( WPARAM wParam, LPARAM lParam, LRESULT* pResult )
{
	HD_NOTIFY* pHDNotify = (HD_NOTIFY*)lParam;

    if( pHDNotify->hdr.code == HDN_ITEMCLICKA ||
	    pHDNotify->hdr.code == HDN_ITEMCLICKW )
    {
	    if( pHDNotify->iItem == m_nSortedCol )
		    SortColumn( pHDNotify->iItem, !m_bAscending );
	    else
		    SortColumn( pHDNotify->iItem, m_header.GetAscending() );
    }

	return CXTPFrameWnd::OnNotify( wParam, lParam, pResult );
}

int CEditListPage::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if( CFrameWnd::OnCreate( lpCreateStruct ) == -1 )
		return -1;

	BOOL bReturn = m_listCtrl.CreateEx(
		0,
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS,
		CRect( 0, 0, 0, 0 ),
		this,
		AFX_IDW_PANE_FIRST );

	if( !bReturn )
		return -1;

	RECT rect;
	m_listCtrl.GetClientRect( &rect );
	int nWidth = ( rect.right - rect.left );
	int nCol[] = { int( nWidth * 0.4f ), int( nWidth * 0.6f ) };

	m_listCtrl.InsertColumn( 0, _T( "ID" ), LVCFMT_LEFT, nCol[ 0 ] );
	m_listCtrl.InsertColumn( 1, _T( "File" ), LVCFMT_LEFT, nCol[ 1 ] );

	m_listCtrl.ModifyExtendedStyle( 0, LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP );

	HWND hWndHeader = m_listCtrl.GetDlgItem( 0 )->GetSafeHwnd();
	m_header.SubclassWindow( hWndHeader );
	m_header.EnableAutoSize( TRUE );
    m_header.EnablePopupMenus( TRUE );
    m_header.InitializeHeader( TRUE );
	m_header.ResizeColumnsToFit();

	CXTPHeaderCtrlTheme* pTheme = m_header.GetTheme();
	if( pTheme )
	{
        DWORD dwStyle = HDR_XTP_WINDEF | HDR_XTP_HOTTRACKING | HDR_XTP_SORTARROW;
        pTheme->SetDrawStyle( dwStyle, &m_header );
        m_header.RedrawWindow();
	}

	// Test
	{
		for( int i=0; i<10; ++i )
		{
			CString strText;
			strText.Format( _T( "Test %d" ), i + 1 );

			m_listCtrl.InsertItem( i, strText.GetString(), 0 );
		    m_listCtrl.SetItem( i, 1, LVIF_TEXT, "", 0, NULL, NULL, NULL );
		}
	}

	return 0;
}

void CEditListPage::OnLvnChanged( NMHDR* pNMHDR, LRESULT* pResult )
{
	if( m_bSorting )
		return;

    int nItem = GetSelectItem();
	if( nItem < 0 )
		return;

	CString strID = m_listCtrl.GetItemText( nItem, 0 );

	if( g_pEditor )
		g_pEditor->SetSelectedID( strID.GetString() );

	if( m_pFrame )
		m_pFrame->RefreshControlProperties();

	*pResult = 0;
}

void CEditListPage::Refresh()
{
	if( !g_pEditor )
        return;

	m_listCtrl.DeleteAllItems();

	int nSelectedIndex = -1;
	int nIndex = 0;

	const std::string& strSelectedID = g_pEditor->GetSelectedID();
	UITextureList::ControlMap& mapControl = g_pEditor->GetControlMap();

	BOOST_FOREACH( UITextureList::ControlMapValue& it, mapControl )
	{
		std::string strID = it.first;
		if( strID.empty() )
			continue;

		UITextureList::SControl& sControl = it.second;

		m_listCtrl.InsertItem( nIndex, strID.c_str(), 0 );
		m_listCtrl.SetItem( nIndex, 1, LVIF_TEXT, sControl.m_strFileName.c_str(), 0, NULL, NULL, NULL );

		if( strID == strSelectedID )
			nSelectedIndex = nIndex;

		++nIndex;
	}

	SetSelectItem( nSelectedIndex );

	SortColumn( m_nSortedCol, m_bAscending );
}

void CEditListPage::SortColumn( int nSortedCol, BOOL bAscending )
{
	m_nSortedCol = nSortedCol;
    m_bAscending = bAscending;
	m_bSorting = TRUE;

    CXTPSortClass csc( &m_listCtrl, m_nSortedCol );
	csc.Sort( m_bAscending ? true : false, xtpSortString );

	m_bSorting = FALSE;

	SetSelectItem( GetSelectItem() );
}

int CEditListPage::GetSelectItem()
{
	POSITION pPos = m_listCtrl.GetFirstSelectedItemPosition();
    return m_listCtrl.GetNextSelectedItem( pPos );
}

void CEditListPage::SetSelectItem( int nItem )
{
	if( nItem < 0 )
		return;

	m_listCtrl.SetSelectionMark( nItem );
    m_listCtrl.SetItemState( nItem, LVIS_SELECTED, LVIS_SELECTED );
    m_listCtrl.EnsureVisible( nItem, FALSE );
    m_listCtrl.SetFocus();
}

void CEditListPage::SetSelectItem( const std::string& strItem )
{
    LVFINDINFO find_item;

    find_item.flags = LVFI_STRING;
    find_item.psz = strItem.c_str();

    int nItem = m_listCtrl.FindItem( &find_item );

    SetSelectItem( nItem );
}
