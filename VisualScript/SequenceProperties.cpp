#include "stdafx.h"
#include "../InternalCommonLib/dxstdafx.h"

#include "./Frame.h"
#include "./Editor.h"
#include "./PropertiesItem.h"
#include "./SequenceProperties.h"

#include "../SigmaCore/DebugInclude.h"

namespace
{
    static CSequenceProperties* s_pSequenceProperties = NULL;
    static BOOL s_bSequencePropertiesFirst = TRUE;

    VOID CALLBACK OnExpandButtonKey( CXTPPropertyGridItem* pItem );
    VOID CALLBACK OnChangedSequenceName( CXTPPropertyGridItem* pItem );
}

CSequenceProperties::CSequenceProperties()
    : m_pFrame( NULL )
{
    s_pSequenceProperties = this;
}

CSequenceProperties::~CSequenceProperties()
{
}

LRESULT CSequenceProperties::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
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
            if( s_bSequencePropertiesFirst )
            {
                s_bSequencePropertiesFirst = FALSE;

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

VOID CSequenceProperties::RefreshPropertiesControl()
{
    if( !g_pEditor )
        return;

    ResetContent();
    m_mapPropertiesClass.clear();

    if( g_pEditor->m_vecSequenceSelected.empty() )
        return;

    BOOST_FOREACH( vs::CSequenceObject* pThis, g_pEditor->m_vecSequenceSelected )
    {
        const ic::CRtti* pRtti = pThis->VRtti();
        while( pRtti )
        {
            PropertiesClassMapIter fi = m_mapPropertiesClass.find( pRtti->Index() );
            if( fi == m_mapPropertiesClass.end() )
                m_mapPropertiesClass.insert( std::make_pair( pRtti->Index(), 1 ) );
            else
                ++fi->second;
            pRtti = pRtti->Base();
        }
    }

    size_t nSize = g_pEditor->m_vecSequenceSelected.size();

    PropertiesClassMapIter it = m_mapPropertiesClass.begin();
    while( it != m_mapPropertiesClass.end() )
    {
        if( it->second == nSize )
            ++it;
        else
            it = m_mapPropertiesClass.erase( it );
    }

    BOOST_REVERSE_FOREACH( PropertiesClassMapValue& ri, m_mapPropertiesClass )
        AddClassCategory( ri.first );
}

VOID CSequenceProperties::AddClassCategory( size_t nIndex )
{
    ic::CBaseObject* pStatic = ic::CRttiMgr::FindStatic( nIndex );
    vs::CSequenceObject* pObject = dynamic_cast< vs::CSequenceObject* >( pStatic );
    if( !pObject )
        return;

    std::vector< ic::PropertyMap* > vecPropertyMap;

    BOOST_FOREACH( vs::CSequenceObject* pSelected, g_pEditor->m_vecSequenceSelected )
    {
        ic::PropertyMap* pPropertyMap = pSelected->VPropertyMap( nIndex );
        vecPropertyMap.push_back( pPropertyMap );
    }

    ic::PropertyMap* pDefaultPropertyMap = vecPropertyMap[ 0 ];
    if( pDefaultPropertyMap->empty() )
        return;

    CXTPPropertyGridItem* pCategory = NULL;
    size_t nSize = g_pEditor->m_vecSequenceSelected.size();

    BOOST_FOREACH( ic::PropertyMapValue& it, *pDefaultPropertyMap )
    {
        const TSTRING& strDefaultKey = it.first;
        ic::CProperty* pPropertyBase = it.second;

        switch( pPropertyBase->VType() )
        {
        case ic::Ebool:
            {
                if( !pCategory )
                    pCategory = AddCategory( pObject->GetClassName().c_str() );

                CPropertyGridItembool* pItem = new CPropertyGridItembool( strDefaultKey );
                pCategory->AddChildItem( pItem );

                for( size_t i=0; i<nSize; ++i )
                {
                    ic::PropertyMapIter jt = vecPropertyMap[ i ]->find( strDefaultKey );
                    ic::CPropertybool* pProperty = dynamic_cast< ic::CPropertybool* >( jt->second );
                    __gassert_( pProperty );
                    pItem->BindPtr( pProperty->GetPtr(), pProperty->GetCall() );
                }
            }
            break;

        case ic::EBool:
            {
                if( !pCategory )
                    pCategory = AddCategory( pObject->GetClassName().c_str() );

                CPropertyGridItemBool* pItem = new CPropertyGridItemBool( strDefaultKey );
                pCategory->AddChildItem( pItem );

                for( size_t i=0; i<nSize; ++i )
                {
                    ic::PropertyMapIter jt = vecPropertyMap[ i ]->find( strDefaultKey );
                    ic::CPropertyBool* pProperty = dynamic_cast< ic::CPropertyBool* >( jt->second );
                    __gassert_( pProperty );
                    pItem->BindPtr( pProperty->GetPtr(), pProperty->GetCall() );
                }
            }
            break;

        case ic::EColor:
            {
                if( !pCategory )
                    pCategory = AddCategory( pObject->GetClassName().c_str() );

                CPropertyGridItemColor* pItem = new CPropertyGridItemColor( strDefaultKey );
                pItem->SetEditorStyle( xtpGridItemColorPopup );
                pCategory->AddChildItem( pItem );

                for( size_t i=0; i<nSize; ++i )
                {
                    ic::PropertyMapIter jt = vecPropertyMap[ i ]->find( strDefaultKey );
                    ic::CPropertyColor* pProperty = dynamic_cast< ic::CPropertyColor* >( jt->second );
                    __gassert_( pProperty );
                    pItem->BindPtr( pProperty->GetPtr(), pProperty->GetCall() );
                }
            }
            break;

        case ic::EDouble:
            {
                if( !pCategory )
                    pCategory = AddCategory( pObject->GetClassName().c_str() );

                CPropertyGridItemDouble* pItem = new CPropertyGridItemDouble( strDefaultKey, _T( "%.3f" ) );
                pCategory->AddChildItem( pItem );

                for( size_t i=0; i<nSize; ++i )
                {
                    ic::PropertyMapIter jt = vecPropertyMap[ i ]->find( strDefaultKey );
                    ic::CPropertyDouble* pProperty = dynamic_cast< ic::CPropertyDouble* >( jt->second );
                    __gassert_( pProperty );
                    pItem->BindPtr( pProperty->GetPtr(), pProperty->GetCall() );
                }
            }
            break;

        case ic::EFlags:
            {
                if( !pCategory )
                    pCategory = AddCategory( pObject->GetClassName().c_str() );

                ic::CPropertyFlags* pPropertyDefault = dynamic_cast< ic::CPropertyFlags* >( pPropertyBase );
                __gassert_( pPropertyDefault );
                ic::CPropertyFlags::DataPairVec& vecData = pPropertyDefault->GetData();

                CPropertyGridItemFlags* pItem = new CPropertyGridItemFlags( strDefaultKey );
                pItem->SetCheckBoxStyle();
                for( size_t j=0; j<vecData.size(); ++j )
                {
                    pItem->GetConstraints()->AddConstraint(
                        vecData[ j ].second.c_str(),
                        vecData[ j ].first );
                }
                pCategory->AddChildItem( pItem );

                for( size_t i=0; i<nSize; ++i )
                {
                    ic::PropertyMapIter jt = vecPropertyMap[ i ]->find( strDefaultKey );
                    ic::CPropertyFlags* pProperty = dynamic_cast< ic::CPropertyFlags* >( jt->second );
                    __gassert_( pProperty );
                    pItem->BindPtr( pProperty->GetPtr(), pProperty->GetCall() );
                }
            }
            break;

        case ic::EFloat:
            {
                if( !pCategory )
                    pCategory = AddCategory( pObject->GetClassName().c_str() );

                CPropertyGridItemFloat* pItem = new CPropertyGridItemFloat( strDefaultKey, _T( "%.3f" ) );
                pCategory->AddChildItem( pItem );

                for( size_t i=0; i<nSize; ++i )
                {
                    ic::PropertyMapIter jt = vecPropertyMap[ i ]->find( strDefaultKey );
                    ic::CPropertyFloat* pProperty = dynamic_cast< ic::CPropertyFloat* >( jt->second );
                    __gassert_( pProperty );
                    pItem->BindPtr( pProperty->GetPtr(), pProperty->GetCall() );
                }
            }
            break;

        case ic::EInt:
            {
                if( !pCategory )
                    pCategory = AddCategory( pObject->GetClassName().c_str() );

                CPropertyGridItemInt* pItem = new CPropertyGridItemInt( strDefaultKey );
                pCategory->AddChildItem( pItem );

                for( size_t i=0; i<nSize; ++i )
                {
                    ic::PropertyMapIter jt = vecPropertyMap[ i ]->find( strDefaultKey );
                    ic::CPropertyInt* pProperty = dynamic_cast< ic::CPropertyInt* >( jt->second );
                    __gassert_( pProperty );
                    pItem->BindPtr( pProperty->GetPtr(), pProperty->GetCall() );
                }
            }
            break;

        case ic::ESelect:
            {
                if( !pCategory )
                    pCategory = AddCategory( pObject->GetClassName().c_str() );

                ic::CPropertySelect* pPropertyDefault = dynamic_cast< ic::CPropertySelect* >( pPropertyBase );
                __gassert_( pPropertyDefault );
                ic::CPropertySelect::DataPairVec& vecData = pPropertyDefault->GetData();

                CPropertyGridItemSelect* pItem = new CPropertyGridItemSelect( strDefaultKey );
                for( size_t j=0; j<vecData.size(); ++j )
                {
                    pItem->GetConstraints()->AddConstraint(
                        vecData[ j ].second.c_str(),
                        vecData[ j ].first );
                }
                pCategory->AddChildItem( pItem );

                for( size_t i=0; i<nSize; ++i )
                {
                    ic::PropertyMapIter jt = vecPropertyMap[ i ]->find( strDefaultKey );
                    ic::CPropertySelect* pProperty = dynamic_cast< ic::CPropertySelect* >( jt->second );
                    __gassert_( pProperty );
                    pItem->BindPtr( pProperty->GetPtr(), pProperty->GetCall() );
                }
            }
            break;

        case ic::EString:
            {
                if( !pCategory )
                    pCategory = AddCategory( pObject->GetClassName().c_str() );

                CPropertyGridItemString* pItem = new CPropertyGridItemString( strDefaultKey );
                pCategory->AddChildItem( pItem );

                for( size_t i=0; i<nSize; ++i )
                {
                    ic::PropertyMapIter jt = vecPropertyMap[ i ]->find( strDefaultKey );
                    ic::CPropertyString* pProperty = dynamic_cast< ic::CPropertyString* >( jt->second );
                    __gassert_( pProperty );
                    pItem->BindPtr( pProperty->GetPtr(), pProperty->GetCall() );
                }

                if( strDefaultKey.compare( _T( "Key" ) ) == 0 )
                {
                    pItem->SetFlags( pItem->GetFlags() | xtpGridItemHasExpandButton );
                    pItem->SetOnButton( OnExpandButtonKey );
                }

                if( pObject->IsA( vs::CSequence::Rtti() ) &&
                    strDefaultKey.compare( _T( "Name" ) ) == 0 )
                {
                    pItem->SetCallBack( OnChangedSequenceName );
                }
            }
            break;

        case ic::EVector2:
            {
                if( !pCategory )
                    pCategory = AddCategory( pObject->GetClassName().c_str() );

                CPropertyGridItemVector2* pItem = new CPropertyGridItemVector2( strDefaultKey, _T( "%.3f" ) );
                pCategory->AddChildItem( pItem );

                for( size_t i=0; i<nSize; ++i )
                {
                    ic::PropertyMapIter jt = vecPropertyMap[ i ]->find( strDefaultKey );
                    ic::CPropertyVector2* pProperty = dynamic_cast< ic::CPropertyVector2* >( jt->second );
                    __gassert_( pProperty );
                    pItem->BindPtr( pProperty->GetPtr(), pProperty->GetCall() );
                }

                pItem->Expand();
            }
            break;

        case ic::EVector3:
            {
                if( !pCategory )
                    pCategory = AddCategory( pObject->GetClassName().c_str() );

                CPropertyGridItemVector3* pItem = new CPropertyGridItemVector3( strDefaultKey, _T( "%.3f" ) );
                pCategory->AddChildItem( pItem );

                for( size_t i=0; i<nSize; ++i )
                {
                    ic::PropertyMapIter jt = vecPropertyMap[ i ]->find( strDefaultKey );
                    ic::CPropertyVector3* pProperty = dynamic_cast< ic::CPropertyVector3* >( jt->second );
                    __gassert_( pProperty );
                    pItem->BindPtr( pProperty->GetPtr(), pProperty->GetCall() );
                }

                pItem->Expand();
            }
            break;

        case ic::EVector4:
            {
                if( !pCategory )
                    pCategory = AddCategory( pObject->GetClassName().c_str() );

                CPropertyGridItemVector4* pItem = new CPropertyGridItemVector4( strDefaultKey, _T( "%.3f" ) );
                pCategory->AddChildItem( pItem );

                for( size_t i=0; i<nSize; ++i )
                {
                    ic::PropertyMapIter jt = vecPropertyMap[ i ]->find( strDefaultKey );
                    ic::CPropertyVector4* pProperty = dynamic_cast< ic::CPropertyVector4* >( jt->second );
                    __gassert_( pProperty );
                    pItem->BindPtr( pProperty->GetPtr(), pProperty->GetCall() );
                }

                pItem->Expand();
            }
            break;
        }
    }

    if( pCategory )
        pCategory->Expand();
}

namespace
{
    VOID CALLBACK OnExpandButtonKey( CXTPPropertyGridItem* pItem )
    {
        __gassert_( s_pSequenceProperties );

        CFrame* pFrame = s_pSequenceProperties->GetFrame();
        __gassert_( pFrame );

        TSTRING strKey = pFrame->GetKeyFromEnumPropertyDlg();
        pItem->OnValueChanged( strKey.c_str() );
    }

    VOID CALLBACK OnChangedSequenceName( CXTPPropertyGridItem* pItem )
    {
        __gassert_( s_pSequenceProperties );

        CFrame* pFrame = s_pSequenceProperties->GetFrame();
        __gassert_( pFrame );

        pFrame->RebuildTreeControl();
    }
}