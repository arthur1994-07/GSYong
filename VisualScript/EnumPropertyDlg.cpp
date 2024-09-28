#include "stdafx.h"

#include "../InternalCommonLib/Core/BaseObject.h"

#include "./EnumPropertyDlg.h"

#include "../SigmaCore/DebugInclude.h"

IMPLEMENT_DYNAMIC( CEnumPropertyDlg, CDialog )

BEGIN_MESSAGE_MAP( CEnumPropertyDlg, CDialog )
	ON_NOTIFY( NM_CLICK, IDC_PROPERTY_DLG_LIST, OnClickList )
END_MESSAGE_MAP()

CEnumPropertyDlg::CEnumPropertyDlg( CWnd* pParent )
    : CDialog( IDD_PROPERTY_DLG, pParent )
    , m_nSortedCol( 0 )
    , m_bAscending( TRUE )
    , m_pObject( NULL )
{
}

CEnumPropertyDlg::~CEnumPropertyDlg()
{
    DestroyWindow();
}

VOID CEnumPropertyDlg::DoModalLess()
{
	Create( IDD_PROPERTY_DLG, m_pParentWnd );
	ShowWindow( SW_SHOW );
}

VOID CEnumPropertyDlg::DoDataExchange( CDataExchange* pDX )
{
	CDialog::DoDataExchange( pDX );

    DDX_Control( pDX, IDC_PROPERTY_DLG_LIST, m_listCtrl );
}

BOOL CEnumPropertyDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    if( !m_pObject )
        return FALSE;

    m_listCtrl.InsertColumn( 0, _T( "Class Name" ), LVCFMT_LEFT, 100 );
	m_listCtrl.InsertColumn( 1, _T( "Property Type" ), LVCFMT_LEFT, 100 );
	m_listCtrl.InsertColumn( 2, _T( "Property Key" ), LVCFMT_LEFT, 100 );

    std::vector< size_t > vecClass;

    const ic::CRtti* pRtti = m_pObject->VRtti();
    while( pRtti )
    {
        size_t nClass = pRtti->Index();
        pRtti = pRtti->Base();

        ic::PropertyMap* pPropertyMap = m_pObject->VPropertyMap( nClass );
        if( !pPropertyMap )
            continue;

        if( pPropertyMap->empty() )
            continue;

        vecClass.push_back( nClass );
    }

    BOOST_REVERSE_FOREACH( size_t nClass, vecClass )
    {
        ic::CBaseObject* pStatic = ic::CRttiMgr::FindStatic( nClass );
        ic::PropertyMap* pPropertyMap = m_pObject->VPropertyMap( nClass );

        INT nItem = 0;
        const TSTRING& strClass = pStatic->GetClassName();

        BOOST_FOREACH( ic::PropertyMapValue& it, *pPropertyMap )
        {
            const TSTRING& strKey = it.first;
            ic::CProperty* pProperty = it.second;

            TSTRING strName = typeid( *pProperty ).name();
            ic::TrimClassName::Trim( strName, strName );

            m_listCtrl.InsertItem( nItem, strClass.c_str(), 0 );
		    m_listCtrl.SetItem( nItem, 1, LVIF_TEXT, strName.c_str(), 0, NULL, NULL, NULL );
		    m_listCtrl.SetItem( nItem, 2, LVIF_TEXT, strKey.c_str(), 0, NULL, NULL, NULL );

            ++nItem;
        }
    }

    m_listCtrl.ModifyExtendedStyle( 0, LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | 
LVS_EX_HEADERDRAGDROP );

    HWND hWndHeader = m_listCtrl.GetDlgItem( 0 )->GetSafeHwnd();
	m_header.SubclassWindow( hWndHeader );
	m_header.EnableAutoSize( TRUE );
    m_header.EnablePopupMenus( TRUE );
    m_header.InitializeHeader( TRUE );
	m_header.ResizeColumnsToFit();
    m_header.SetTheme( new CXTPHeaderCtrlThemeExplorer() );
    CXTPPaintManager::SetTheme( xtpThemeOffice2000 );

	CXTPHeaderCtrlTheme* pTheme = m_header.GetTheme();
	if( pTheme )
	{
        DWORD dwStyle = HDR_XTP_WINDEF | HDR_XTP_HOTTRACKING | HDR_XTP_SORTARROW;
        pTheme->SetDrawStyle( dwStyle, &m_header );
        m_header.RedrawWindow();
	}

    SetWindowText( m_pObject->GetClassName().c_str() );
    CenterWindow();

    return TRUE;
}

BOOL CEnumPropertyDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
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

	return CDialog::OnNotify( wParam, lParam, pResult );
}

VOID CEnumPropertyDlg::OnClickList( NMHDR* pNMHDR, LRESULT* pResult )
{
    POSITION pPos = m_listCtrl.GetFirstSelectedItemPosition();
    INT nItem = m_listCtrl.GetNextSelectedItem( pPos );

    CString strKey = m_listCtrl.GetItemText( nItem, 2 );
    m_strKey = strKey.GetString();

    *pResult = 0;
}

VOID CEnumPropertyDlg::SortColumn( INT nSortedCol, BOOL bAscending )
{
    m_nSortedCol = nSortedCol;
    m_bAscending = bAscending;

    CXTPSortClass csc( &m_listCtrl, m_nSortedCol );
    csc.Sort( ic::BOOLTobool( m_bAscending ), xtpSortString );
}