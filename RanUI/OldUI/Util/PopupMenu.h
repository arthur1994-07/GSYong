#pragma once

#include "./UIGroupHelper.h"

#include "../../InnerInterface.h"

//-----------------------------------------------------------------------------------------------//

class GLGaeaClient;
class CInnerInterface;
class PopupDataComponent;

class CPopupMenu : public CUIGroupHelper
{
private:
    static const int nMAX_POPUP;
    static const float fPOSITION_OFFSET;
    static const float fTEXBOX_OFFSET;

    typedef std::tr1::shared_ptr<PopupDataComponent> SHARED_PTR_POPUP_DATA;

    typedef std::vector<CBasicLineBoxSmart*> VEC_LINEBOX;
    typedef std::vector<CBasicTextBoxEx*> VEC_TEXTBOX;

    enum
    {
        TEXT_BOX_START = NO_ID + 1,
    };

public:
    CPopupMenu( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual	~CPopupMenu();

protected:
    GLGaeaClient* m_pGaeaClient;
    CInnerInterface* m_pInterface;

public:
    void CreateSubControl();

public:
    virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
    virtual void SetVisibleSingle( BOOL bVisible );

public:
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
    void OpenPopup( popupmenu::SP_POPUP_MENU PopupMenu );

private:
    void CreatePopupList( int TextBoxID );
    void RePosition( float x, float y );

    bool SetPopupText( popupmenu::SP_POPUP_MENU pPopupComponent, CBasicTextBoxEx* pTextBox );

    bool IsSelected( CBasicTextBoxEx* pTextBox, int nIndex );
    void OnComponentClick( CBasicTextBoxEx* pTextBox );

    bool ExpandMenu( CBasicTextBoxEx* pTextBox, int nTextBoxID );
    void PopupMenuRePosition( int nTextBoxIndex, float x, float y );
    void SetVisiblePopupList( int nIndex, bool bVisible );

    void AllUnshow();

    void CheckUIBoundary( float& PosX, float& PosY, float SizeX, float SizeY, const UIRECT& ParentRect );

    bool IsEnableNode( unsigned int MenuKey );

private:
    VEC_LINEBOX m_vecLineBox;
    VEC_TEXTBOX m_vecTextBox;

    CUIControl* m_pDummyControl;
    CUIControl* m_pLineBoxDummyControl;
    CUIControl* m_pTextBoxDummyControl;

private:
    popupmenu::SP_POPUP_MENU m_PopupMenu;
    bool m_bInitPostion;

    float CurPosX, CurPosY;

    std::vector<D3DXVECTOR2> m_vecPos;
};

class MyPopupMenu : public IPopupMenu, private CPopupMenu
{
public:
    MyPopupMenu( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~MyPopupMenu()   {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() override;
    virtual void OpenPopup( popupmenu::SP_POPUP_MENU PopupMenu ) override;
};