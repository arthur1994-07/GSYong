#pragma once

#include "../../../EngineLib/GUInterface/GameTextControl.h"

#include "../../InnerInterface.h"

#include "../Util/UIWindowEx.h"
#include "../Util/TapSelectorEx.h"

class RnFriendTab;
class RnBlockTab;

class RnFriendWindow : public CUIWindowEx, private CTapSelectorEx<DEFAULT_TAPBUTTON>
{
private:
    typedef CTapSelectorEx::BUTTON STAPBUTTON;
    enum
    {
        ID_RNFRIEND_TAB = ET_CONTROL_NEXT + 1,
        
        ID_FRIEND_TAB,
        ID_BLOCK_TAB,

        ID_FRIEND_PAGE,
        ID_BLOCK_PAGE,
    };


public:
    RnFriendWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );

public:
    void	            CreateSubControl();
    virtual void        SetVisibleSingle( BOOL bVisible );
    void                SetWhisperName( const std::string& strCharName );

protected:
    virtual void        TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
    virtual void        Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    GLGaeaClient*       m_pGaeaClient;
    CInnerInterface*    m_pInterface;
    CUIGroup*           m_pCurrentPage;

private:
    CUIGroup*           GetTapWindow(UIGUID controlID);
    void                EventSelectedTap( UIGUID controlID );
    virtual void        Refresh();
    RnFriendTab*        m_pFriendPage;
    RnBlockTab*         m_pBlockPage;
};


class MyRnFriendWindow : public IFriendWindow, private RnFriendWindow
{
public:
    MyRnFriendWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
        : RnFriendWindow( pGaeaClient, pInterface, pEngineDevice )
    {

    }
    virtual ~MyRnFriendWindow()   {};

public:
    virtual void CreateUIWindowAndRegisterOwnership();;
    virtual void AddFriendName_To_EditBox( const UI::String& strName );
    virtual void AddFriend( const UI::String& strName );
    virtual void LoadFriendList();
    virtual UI::String GetFriendName() const;
    virtual UI::String GetSelectBlockFriendName() const;
    virtual void EndEditFriendList();
    virtual std::tr1::shared_ptr<SFRIEND> GetFriendSMSInfo();

    virtual void AddFriendBlock( const std::string& strName, bool bBlock );
};