#include "StdAfx.h"

#include "../../../EngineLib/DxTools/DxViewPort.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogic/RANPARAM.h"

#include "../../StaticUIManager.h"
#include "../../OuterInterface.h"

#include "./LockSelectCharacterPreviewPage.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CLockSelectCharacterPreviewPage::CLockSelectCharacterPreviewPage(GLGaeaClient* pGaeaClient, COuterInterface* pInterface, EngineDeviceMan* pEngineDevice)
: CUIGroup(pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_pInterface(pInterface)
, m_pLockImageDummy(NULL)
{
    for ( int i = 0; i < MAX_SELECTCHAR; i++ )
        m_pLockImage[ i ] = NULL;
}

CLockSelectCharacterPreviewPage::~CLockSelectCharacterPreviewPage ()
{
}

void CLockSelectCharacterPreviewPage::CreateSubControl ()
{
    for ( int i = 0; i < MAX_SELECTCHAR; i++ )
    {
        CUIControl* pControl = new CUIControl( m_pEngineDevice );
        pControl->CreateSub( this, "LOCK_CHARACTER_PREVIEW_PAGE", UI_FLAG_DEFAULT );
        RegisterControl( pControl );
        pControl->SetVisibleSingle( FALSE );
        m_pLockImage[ i ] = pControl;
    }

    {
        CUIControl* pControl = new CUIControl( m_pEngineDevice );
        pControl->CreateSub( this, "LOCK_CHARACTER_PREVIEW_PAGE", UI_FLAG_DEFAULT );
        RegisterControl( pControl );
        pControl->SetVisibleSingle( FALSE );
        m_pLockImageDummy = pControl;
    }
}

void CLockSelectCharacterPreviewPage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
    CUIGroup::TranslateUIMessage ( ControlID, dwMsg );
}

void CLockSelectCharacterPreviewPage::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CLockSelectCharacterPreviewPage::SetLockImagePos( const D3DXVECTOR3& vCharPos, int nIndex )
{
    if ( nIndex < 0 || nIndex >= MAX_SELECTCHAR ) return;

    D3DXVECTOR3 vScreen;

    if ( IsVisiblePos( vScreen, vCharPos ) )
    {
        const UIRECT& rcOriginPos = m_pLockImageDummy->GetGlobalPos();

        D3DXVECTOR2 vpos;
        vpos.x = floor( vScreen.x - ( rcOriginPos.sizeX * 0.5f ) ); // MEMO
        vpos.y = floor( vScreen.y - rcOriginPos.sizeY * 1.2f );

        if ( abs( rcOriginPos.left - vpos.x ) > 1 || abs( rcOriginPos.top - vpos.y ) > 1 )
        {
            m_pLockImage[ nIndex ]->SetGlobalPos( vpos );
        }
    }
}

bool CLockSelectCharacterPreviewPage::IsVisiblePos( D3DXVECTOR3& vOutScreen, const D3DXVECTOR3& vPosition )
{
    D3DXVECTOR3 vPos = vPosition;
    vOutScreen = DxViewPort::GetInstance().ComputeVec3Project( &vPos, NULL );

    long lResolution = CUIMan::GetResolution();
    WORD X_RES = HIWORD(lResolution) - 30;
    WORD Y_RES = LOWORD(lResolution) - 30;

    if ( (vOutScreen.x<40) || (vOutScreen.y<40) || (X_RES<vOutScreen.x) || (Y_RES<vOutScreen.y) )
    {
        return false;
    }

    return true;
}

void CLockSelectCharacterPreviewPage::SetVisibleLockImage( bool bVisible, int nIndex )
{
    if ( nIndex < 0 || nIndex >= MAX_SELECTCHAR ) return;

    m_pLockImage[ nIndex ]->SetVisibleSingle( bVisible );
}