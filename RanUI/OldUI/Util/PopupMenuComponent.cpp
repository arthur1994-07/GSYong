#include "./PopupMenuComponent.h"

namespace popupmenu
{

//! Composite
PopupMenuComposite::PopupMenuComposite( const std::string& _Description, bool _Enable )
    : m_Data( _Description, _Enable )
{
    m_mapComponent.clear();
}

PopupMenuComposite::~PopupMenuComposite()
{
    m_mapComponent.clear();
}

void PopupMenuComposite::addComponent( SP_POPUP_MENU PopupComponent )
{
    unsigned int MenuKey = m_mapComponent.size() + 1;
    m_mapComponent.insert( std::make_pair( MenuKey, PopupComponent ) );
}

const std::string& PopupMenuComposite::GetDescription()
{
    return m_Data.Description;
}

void PopupMenuComposite::SetDescription( const std::string& _strDescription )
{
    m_Data.Description = _strDescription;
}

bool PopupMenuComposite::IsEnable()
{
    return m_Data.Enable;
}

void PopupMenuComposite::SetEnable(  const bool _bEnable )
{
    m_Data.Enable = _bEnable;
}

SP_POPUP_MENU PopupMenuComposite::FindInAllComponents( unsigned int MenuKey )
{
    return FindInAllComponents( MenuKey, m_mapComponent );
}

SP_POPUP_MENU PopupMenuComposite::FindInAllComponents( unsigned int MenuKey, MAP_MENU_COMPONENT& InMap )
{
    ITER_MENU_COMPONENT FindIter = InMap.find( MenuKey );

    if ( FindIter != InMap.end() )
        return FindIter->second;
    else
    {
        for ( ITER_MENU_COMPONENT iter = InMap.begin(); iter != InMap.end(); iter++ )
        {
            SP_POPUP_MENU TempComponent = iter->second;

            if ( TempComponent->IsSubComponent() )
            {
                MAP_MENU_COMPONENT TempMapComponent;
                TempComponent->GetSubComponent( TempMapComponent );

                SP_POPUP_MENU FindComponent = FindInAllComponents( MenuKey, TempMapComponent );
                if ( FindComponent )
                    return FindComponent;
            }

        }

        return SP_POPUP_MENU();
    }
}

bool PopupMenuComposite::IsSubComponent()
{
    return ( m_mapComponent.size() != 0 );
}

void PopupMenuComposite::GetSubComponent( MAP_MENU_COMPONENT& _OutMap )
{
    _OutMap = m_mapComponent;
}

void PopupMenuComposite::SetKey( unsigned int MenuKey, SP_POPUP_MENU PopupMenu )
{
    unsigned int CurMenuKey = MenuKey + 1;
    unsigned int SubMenuKey = MenuKey + m_mapComponent.size() + 1;

    MAP_MENU_COMPONENT TempMap = m_mapComponent;
    m_mapComponent.clear();

    for ( ITER_MENU_COMPONENT iter = TempMap.begin(); iter != TempMap.end(); iter++ )
    {
        SP_POPUP_MENU PopupComponent = iter->second;
        m_mapComponent.insert( std::make_pair( CurMenuKey++, PopupComponent ) );

        if ( PopupComponent->IsSubComponent() )
        {
            PopupComponent->SetKey( SubMenuKey, PopupComponent );

            MAP_MENU_COMPONENT SubMap;
            PopupComponent->GetSubComponent( SubMap );
            SubMenuKey += ( SubMap.size() + 1 );
        }
    }
}

//////////////////////////////////////////////////////////////////////////

//! Item
PopupMenuItem::PopupMenuItem( const std::string& _Description, bool _Enable, POPUP_FUNCTION _CallbackFunc )
    : m_Data( _Description, _Enable )
    , CallbackFunc(_CallbackFunc)
{
}

PopupMenuItem::~PopupMenuItem()
{
}

void PopupMenuItem::execute( int Index )
{
    CallbackFunc( Index );
}

const std::string& PopupMenuItem::GetDescription()
{
    return m_Data.Description;
}

void PopupMenuItem::SetDescription( const std::string& _strDescription )
{
    m_Data.Description = _strDescription;
}

bool PopupMenuItem::IsEnable()
{
    return m_Data.Enable;
}

void PopupMenuItem::SetEnable( const bool _bEnable )
{
    m_Data.Enable = _bEnable;
}

POPUP_FUNCTION PopupMenuItem::GetCallBackFunc() 
{
    return CallbackFunc;
}

//////////////////////////////////////////////////////////////////////////

SP_POPUP_MENU CreateRoot()
{
    return SP_POPUP_MENU(new popupmenu::PopupMenuComposite( "", false));
}

SP_POPUP_MENU CreatePopupMenuComposite( const std::string& _Description, bool _Enable )
{
    return SP_POPUP_MENU( new PopupMenuComposite( _Description, _Enable ) );
}

SP_POPUP_MENU CreatePopupMenuItem( const std::string& _Description, POPUP_FUNCTION CallbackFunc /*= NULL */ )
{
    bool Enable = false;
    if ( CallbackFunc )
        Enable = true;

    return SP_POPUP_MENU( new PopupMenuItem( _Description, Enable, CallbackFunc ) );
}

}