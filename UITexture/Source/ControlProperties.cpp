#include "stdafx.h"
#include "../../InternalCommonLib/dxstdafx.h"

#include <boost/bind.hpp>

#include "./Common/PropertiesItem.h"
#include "./Frame.h"
#include "./Editor.h"
#include "./ControlProperties.h"

#include "../../SigmaCore/DebugInclude.h"

namespace
{
    static bool s_bPropertiesFirst = true;
}

CControlProperties::CControlProperties()
    : m_pFrame( NULL )
{
}

CControlProperties::~CControlProperties()
{
}

LRESULT CControlProperties::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT lr = CXTPPropertyGrid::WindowProc( message, wParam, lParam );

    switch( message )
    {
    case WM_CREATE:
        {
            m_pFrame = DYNAMIC_DOWNCAST( CFrame, GetParent() );
        }
        break;

    case WM_SHOWWINDOW:
        {
            if( s_bPropertiesFirst )
            {
				s_bPropertiesFirst = false;

                SetTheme( xtpGridThemeWhidbey );
                SetPropertySort( xtpGridSortCategorized );
                ShowToolBar( TRUE );
                ShowHelp( FALSE );
                EnableToolTips( FALSE );
                SetShowInplaceButtonsAlways( TRUE );
                HighlightChangedItems( FALSE );
                SetVariableItemsHeight( TRUE );
                EnableMultiSelect( FALSE );
            }

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

void CControlProperties::Refresh()
{
    if( !g_pEditor )
        return;

    ResetContent();

	std::string strSelectedID = g_pEditor->GetSelectedID();
	UITextureList::ControlMap& mapControl = g_pEditor->GetControlMap();

	UITextureList::ControlMapIter it = mapControl.find( strSelectedID );
	if( it == mapControl.end() )
		return;

	UITextureList::SControl& sControl = it->second;

	// CONTROL
	{
		m_strEditID = strSelectedID;

		CXTPPropertyGridItem* pCategory = AddCategory( _T( "CONTROL" ) );

		CPropertyGridItemString* pString = new CPropertyGridItemString( _T( "ID" ) );
		pString->BindPtr( &m_strEditID, boost::bind( boost::mem_fn( &CControlProperties::OnChangedID ), this ) );
		pCategory->AddChildItem( pString );

		pCategory->Expand();
	}

	// TEXTURE
	{
		CXTPPropertyGridItem* pCategory = AddCategory( _T( "TEXTURE" ) );

		CPropertyGridItemFloat* pFloat = new CPropertyGridItemFloat( _T( "SizeX" ), _T( "%.f" ) );
		pFloat->BindPtr( &sControl.m_vSize.x, NULL );
		pCategory->AddChildItem( pFloat );

		pFloat = new CPropertyGridItemFloat( _T( "SizeY" ), _T( "%.f" ) );
		pFloat->BindPtr( &sControl.m_vSize.y, NULL );
		pCategory->AddChildItem( pFloat );

		CPropertyGridItemString* pString = new CPropertyGridItemString( _T( "File" ) );
		pString->BindPtr( &sControl.m_strFileName, NULL );
		pCategory->AddChildItem( pString );

		pCategory->Expand();
	}

	// TEXTURE_POS
	{
		CXTPPropertyGridItem* pCategory = AddCategory( _T( "TEXTURE_POS" ) );

		CPropertyGridItemFloat* pFloat = new CPropertyGridItemFloat( _T( "X" ), _T( "%.f" ) );
		pFloat->BindPtr( &sControl.m_vPos.x, NULL );
		pCategory->AddChildItem( pFloat );

		pFloat = new CPropertyGridItemFloat( _T( "Y" ), _T( "%.f" ) );
		pFloat->BindPtr( &sControl.m_vPos.y, NULL );
		pCategory->AddChildItem( pFloat );

		pFloat = new CPropertyGridItemFloat( _T( "Width" ), _T( "%.f" ) );
		pFloat->BindPtr( &sControl.m_vPos.z, NULL );
		pCategory->AddChildItem( pFloat );

		pFloat = new CPropertyGridItemFloat( _T( "Height" ), _T( "%.f" ) );
		pFloat->BindPtr( &sControl.m_vPos.w, NULL );
		pCategory->AddChildItem( pFloat );

		pCategory->Expand();
	}

	// BORDER
	{
		CXTPPropertyGridItem* pCategory = AddCategory( _T( "BORDER" ) );

		CPropertyGridItemFloat* pFloat = new CPropertyGridItemFloat( _T( "Left" ), _T( "%.f" ) );
		pFloat->BindPtr( &sControl.m_vBorder.x, NULL );
		pCategory->AddChildItem( pFloat );

		pFloat = new CPropertyGridItemFloat( _T( "Top" ), _T( "%.f" ) );
		pFloat->BindPtr( &sControl.m_vBorder.y, NULL );
		pCategory->AddChildItem( pFloat );

		pFloat = new CPropertyGridItemFloat( _T( "Right" ), _T( "%.f" ) );
		pFloat->BindPtr( &sControl.m_vBorder.z, NULL );
		pCategory->AddChildItem( pFloat );

		pFloat = new CPropertyGridItemFloat( _T( "Bottom" ), _T( "%.f" ) );
		pFloat->BindPtr( &sControl.m_vBorder.w, NULL );
		pCategory->AddChildItem( pFloat );

		pCategory->Expand();
	}
}

void CControlProperties::OnChangedID()
{
	if( !g_pEditor )
        return;

	std::string strSelectedID = g_pEditor->GetSelectedID();
	UITextureList::ControlMap& mapControl = g_pEditor->GetControlMap();

	UITextureList::ControlMapIter it = mapControl.find( strSelectedID );
	if( it == mapControl.end() )
		return;

	if( m_strEditID.empty() )
		goto goto_refresh;

	if( m_strEditID == strSelectedID )
		goto goto_refresh;

	// Update
	{
		UITextureList::SControl sControl = it->second;

		ResetContent();

		mapControl.erase( it );

		it = mapControl.find( m_strEditID );
		if( it == mapControl.end() )
		{
			mapControl.insert( UITextureList::ControlMapValue( m_strEditID, sControl ) );

			g_pEditor->SetSelectedID( m_strEditID );

			m_pFrame->RefreshEditListPage();
		}
		else
		{
			mapControl.insert( UITextureList::ControlMapValue( strSelectedID, sControl ) );

			MessageBox( _T( "ID가 중복 되었습니다." ), _T( "에러" ), MB_OK );
		}
	}

goto_refresh:
	Refresh();
}