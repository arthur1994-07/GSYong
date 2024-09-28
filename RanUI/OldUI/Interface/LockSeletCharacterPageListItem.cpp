#include "StdAfx.h"
#include "../../../EngineLib/GUInterface/UIControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../OuterInterface.h"
#include "./LockSeletCharacterPageListItem.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CLockSelectCharacterPageListItem::CLockSelectCharacterPageListItem ( COuterInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CUIOuterWindow ( pInterface, pEngineDevice )
, m_pLineBox(NULL )
, m_pLockImage(NULL)
{
}

CLockSelectCharacterPageListItem::~CLockSelectCharacterPageListItem ()
{
}

void CLockSelectCharacterPageListItem::CreateSubControl ()
{
    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart(m_pEngineDevice);
        pLinBoxSmart->CreateSub( this, "LOCK_CHARACTER_PAGE_BOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "LOCK_CHARACTER_PAGE_BOX_LINE" );
        RegisterControl( pLinBoxSmart );
        m_pLineBox = pLinBoxSmart;
    }

    {
        CUIControl* pControl = new CUIControl( m_pEngineDevice );
        pControl->CreateSub( this, "LOCK_CHARACTER_PAGE_SMALL_IMAGE", UI_FLAG_DEFAULT );
        RegisterControl( pControl );
        m_pLockImage = pControl;
    }
}

void CLockSelectCharacterPageListItem::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
    CUIGroup::TranslateUIMessage ( ControlID, dwMsg );
}

void CLockSelectCharacterPageListItem::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}