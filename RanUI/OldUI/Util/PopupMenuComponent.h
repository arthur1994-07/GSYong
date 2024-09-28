#pragma once

#include "./PopupDefine.h"

namespace popupmenu
{

//! Component
class PopupMenuComponent
{
public:
    typedef std::map< unsigned int, SP_POPUP_MENU > MAP_MENU_COMPONENT;
    typedef MAP_MENU_COMPONENT::iterator ITER_MENU_COMPONENT;
    typedef MAP_MENU_COMPONENT::const_iterator CITER_MENU_COMPONENT;

public:
    virtual void addComponent( SP_POPUP_MENU PopupComponent ) {}
    virtual void execute( int Index ) {}

    virtual SP_POPUP_MENU FindInAllComponents( unsigned int MenuKey ) { return SP_POPUP_MENU(); }
    virtual bool IsSubComponent() { return false; }
    virtual void GetSubComponent( MAP_MENU_COMPONENT& _OutVec ) {}

    virtual const std::string& GetDescription() = 0;
    virtual void SetDescription( const std::string& _strDescription) = 0;
    virtual bool IsEnable() = 0;
    virtual void SetEnable( const bool _bEnable ) = 0;
    virtual POPUP_FUNCTION GetCallBackFunc() { return NULL; }

    virtual void SetKey( unsigned int MenuKey, SP_POPUP_MENU PopupMenu ) {};
};
//////////////////////////////////////////////////////////////////////////

//! Composite
class PopupMenuComposite : public PopupMenuComponent
{
private:
    POPUP_NODE m_Data;

    MAP_MENU_COMPONENT m_mapComponent;

public:
    PopupMenuComposite( const std::string& _Description, bool _Enable );
    ~PopupMenuComposite();

public:
    virtual void addComponent( SP_POPUP_MENU PopupComponent ) override;

    virtual SP_POPUP_MENU FindInAllComponents( unsigned int MenuKey ) override;
    virtual bool IsSubComponent() override;
    virtual void GetSubComponent( MAP_MENU_COMPONENT& _OutMap ) override;

    virtual const std::string& GetDescription() override;
    virtual void SetDescription( const std::string& _strDescription) override;
    virtual bool IsEnable() override;
    virtual void SetEnable( const bool _bEnable ) override;
    virtual void SetKey( unsigned int MenuKey, SP_POPUP_MENU PopupMenu ) override;

private:
    SP_POPUP_MENU FindInAllComponents( unsigned int MenuKey, MAP_MENU_COMPONENT& InMap );
};
//////////////////////////////////////////////////////////////////////////

//! Item
class PopupMenuItem : public PopupMenuComponent
{
private:
    POPUP_NODE m_Data;
    POPUP_FUNCTION CallbackFunc;

public:
    PopupMenuItem( const std::string& _Description, bool _Enable, POPUP_FUNCTION _CallbackFunc );
    ~PopupMenuItem();

public:
    virtual void execute( int Index ) override;

    virtual const std::string& GetDescription() override;
    virtual void SetDescription( const std::string& _strDescription) override;
    virtual bool IsEnable() override;
    virtual void SetEnable( const bool _bEnable ) override;
    virtual POPUP_FUNCTION GetCallBackFunc() override;
};
//////////////////////////////////////////////////////////////////////////


}