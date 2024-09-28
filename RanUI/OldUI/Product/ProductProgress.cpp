#include "StdAfx.h"
#include "./ProductProgress.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/UIMan.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"

#include "../Item/InventoryWindow.h"
#include "../Item/InventoryInterface.h"

#include "../Util/RnProgressBar.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace
{
	const float g_fAccuracy = 100.0f;
    const unsigned int g_nTextSize = 20;
}

CProductProgress::CProductProgress( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CUIGroup( pEngineDevice )

	, m_pInterface( pInterface )
    , m_pGaeaClient( pGaeaClient )

	, m_pProductText( NULL )
	, m_pProgressBar( NULL )

	, m_nCount( 0 )
	, m_nCurCount( 0 )
	, m_fMixDelay( 0.0f )
    , m_fElapsedTime( 0.0f )

	, m_bUIClicked( FALSE )
	, m_vOldMousePos( 0.0f, 0.0f )
{
}

void CProductProgress::CreateSubControl()
{
    RnProgressBar::CreateArg arg;
    arg.controlName = "PRODUCT_PROGRESS";
    arg.backgroundTextureName = "PRODUCT_PROGRESS_BG_TEXTURE";
    arg.overTextureName = "PRODUCT_PROGRESS_OVER_TEXTURE";

    m_pProgressBar = new RnProgressBar( m_pEngineDevice );
    m_pProgressBar->CreateSub( this, "PRODUCT_PROGRESS", UI_FLAG_DEFAULT, ID_PRODUCT_PROGRESS );
    m_pProgressBar->CreateSubControl( arg );
    RegisterControl( m_pProgressBar );

	CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
    m_pProductText = new CBasicTextBox(m_pEngineDevice);
    m_pProductText->CreateSub( this, "PRODUCT_PROGRESS", UI_FLAG_DEFAULT, ID_PRODUCT_TEXT_BOX);
    m_pProductText->SetFont ( pFont );
    m_pProductText->SetTextAlign ( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
    RegisterControl( m_pProductText );
}

void CProductProgress::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if( !IsVisible() )
		return;

	if( m_pProgressBar->IsEndProgress() )
	{
		if ( m_nCurCount == m_nCount )
		{
			CString strText;
			strText.Format( "%d / %d", m_nCurCount, m_nCount );
			m_pProductText->SetText( m_strProductName, NS_UITEXTCOLOR::WHITESMOKE );
			m_pProductText->AddText( strText, NS_UITEXTCOLOR::WHITESMOKE );

			m_pInterface->UiHideGroup( GetWndID() );

			// 초기화 ( 제작 시작시에도 하지만 끝날 때도 해준다 );
			// 결과적으로 총 두번 초기화를 하게 되지만 확장성을 고려했다;
			m_nCount = 0;
			m_nCurCount = 0;
			m_fMixDelay = 0.f;
			m_fElapsedTime = 0.f;
			m_pProgressBar->SetNow( 0 );
			m_pProductText->ClearText();
		}

		return;
	}

    if( m_bUIClicked )
    {
        D3DXVECTOR2 vPos;
        vPos.x = m_vOldUIPos.left + x - m_vOldMousePos.x;
        vPos.y = m_vOldUIPos.top + y - m_vOldMousePos.y;
        this->SetGlobalPos( vPos );
    }
	else
	{
		m_vOldMousePos.x = static_cast<FLOAT>(x);
		m_vOldMousePos.y = static_cast<FLOAT>(y);
		m_vOldUIPos = this->GetGlobalPos();
	}

	m_fElapsedTime += fElapsedTime;

	int nShowCount = m_nCurCount;
	float fCurCountMaxTime = m_fMixDelay * ( m_nCurCount+1 );
	if ( m_fElapsedTime >= fCurCountMaxTime )
	{
		m_fElapsedTime = fCurCountMaxTime;
		nShowCount = m_nCurCount+1;
	}

	m_pProgressBar->SetNow( static_cast< DWORD >( m_fElapsedTime * g_fAccuracy ) );

	CString strText;
	strText.Format( "%d / %d", nShowCount, m_nCount );
	m_pProductText->SetText( m_strProductName, NS_UITEXTCOLOR::WHITESMOKE );
	m_pProductText->AddText( strText, NS_UITEXTCOLOR::WHITESMOKE );

	CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CProductProgress::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    switch( ControlID )
    {
    case ID_PRODUCT_PROGRESS:
		{
			if( CHECK_MOUSE_IN( dwMsg ) )
			{
				if( dwMsg & UIMSG_LB_DOWN )
				{
					SetExclusiveControl();
					m_bUIClicked = TRUE;
				}
				else if( CHECK_LB_UP_LIKE( dwMsg ) )
				{
					ResetExclusiveControl();
					m_bUIClicked = FALSE;
				}
			}
			else if( CHECK_LB_UP_LIKE( dwMsg ) )
			{
				ResetExclusiveControl();
				m_bUIClicked = FALSE;
			}
		}
		break;
    }
}

void CProductProgress::SetVisibleSingle( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle( bVisible );

	if( !bVisible )
	{
		IInventoryWindow* pInvenWindow = m_pInterface->GetInventoryWindow();
		if( pInvenWindow && pInvenWindow->IsLocked() )
			pInvenWindow->UnLockTrade();

		IInventory* pInventory = m_pInterface->GetInventory();
		if( pInventory && pInventory->IsLocked() )
			pInventory->UnLockTrade();
	}
}

void CProductProgress::StartProduct( DWORD nID, WORD nCount )
{
	const SProductRecipe* pProductRecipe = GLProductRecipeMan::GetInstance().GetProductRecipe( nID );
	if( !pProductRecipe || nCount <= 0 )
		return;

	if( pProductRecipe->fDelaySec <= 0.0f )
		return;

	SITEM* pItem = GLItemMan::GetInstance().GetItem( pProductRecipe->sResultItem.sNID );
	if( !pItem )
		return;

    IInventoryWindow* pInvenWindow = m_pInterface->GetInventoryWindow();
    if( pInvenWindow && !pInvenWindow->IsLocked() )
        pInvenWindow->LockTrade();

    IInventory* pInventory = m_pInterface->GetInventory();
    if( pInventory && !pInventory->IsLocked() )
        pInventory->LockTrade();

// 
//     if( g_nTextSize+2 < strlen( pItem->GetName() ) )
//     {
//         char szBuffer[g_nTextSize] = {0,};
//         strncpy( szBuffer, pItem->GetName(), g_nTextSize-2 );
// 
//         m_strProductName.Format( "%s.. %s", szBuffer, ID2GAMEINTEXT( "PRODUCT_PROGRESS_TEXT" ) );
//     }
//     else

    m_strProductName.Format( "%s %s", pItem->GetName(), ID2GAMEINTEXT( "PRODUCT_PROGRESS_TEXT" ) );

	
	m_nCount = nCount;
	m_nCurCount = 0;
	m_fMixDelay = pProductRecipe->fDelaySec;
	m_fElapsedTime = 0.0f;

	m_pProgressBar->SetMax( static_cast< DWORD >( m_fMixDelay * g_fAccuracy ) * m_nCount );
	m_pProgressBar->SetNow( 0 );

	m_pProductText->ClearText();

	SetVisibleSingle( true );
}

void CProductProgress::GoAlongProduct()
{
	if ( IsProdution() )
	{
		m_nCurCount++;

		IInventoryWindow* pInvenWindow = m_pInterface->GetInventoryWindow();
		if( pInvenWindow && !pInvenWindow->IsLocked() )
			pInvenWindow->LockTrade();

		IInventory* pInventory = m_pInterface->GetInventory();
		if( pInventory && !pInventory->IsLocked() )
			pInventory->LockTrade();
	}
}

void CProductProgress::EndProduct()
{
	m_nCurCount++;

	if ( m_nCurCount > m_nCount )
		m_nCurCount = m_nCount;

	IInventoryWindow* pInvenWindow = m_pInterface->GetInventoryWindow();
	if( pInvenWindow && pInvenWindow->IsLocked() )
		pInvenWindow->UnLockTrade();

	IInventory* pInventory = m_pInterface->GetInventory();
	if( pInventory && pInventory->IsLocked() )
		pInventory->UnLockTrade();
}

void CProductProgress::CancelProduct()
{
	m_pInterface->UiHideGroup( GetWndID() );

	IInventoryWindow* pInvenWindow = m_pInterface->GetInventoryWindow();
	if( pInvenWindow && pInvenWindow->IsLocked() )
		pInvenWindow->UnLockTrade();

	IInventory* pInventory = m_pInterface->GetInventory();
	if( pInventory && pInventory->IsLocked() )
		pInventory->UnLockTrade();

	// 초기화 ( 제작 시작시에도 하지만 끝날 때도 해준다 );
	// 결과적으로 총 두번 초기화를 하게 되지만 확장성을 고려했다;
	m_nCount = 0;
	m_nCurCount = 0;
	m_fMixDelay = 0.f;
	m_fElapsedTime = 0.f;
	m_pProgressBar->SetNow( 0 );
	m_pProductText->ClearText();
}

bool CProductProgress::IsProdution()
{
	if( IsVisible() )
		return true;
	else
		return false;
}

MyProductProgress::MyProductProgress( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CProductProgress( pGaeaClient, pInterface, pEngineDevice )
{
}

void MyProductProgress::CreateUIWindowAndRegisterOwnership()
{
    Create( PRODUCT_PROGRESS, "PRODUCT_PROGRESS", UI_FLAG_CENTER_X | UI_FLAG_BOTTOM );
    CreateSubControl();
    m_pInterface->UiRegisterControl( this );
    m_pInterface->UiShowGroupTop( GetWndID() );
	m_pInterface->UiHideGroup( GetWndID() );

    long lResolution = CUIMan::GetResolution();
    WORD X_RES = HIWORD( lResolution );
    WORD Y_RES = LOWORD( lResolution );

    UIRECT rcUI = this->GetLocalPos();
    int nHalfX = static_cast<int>(rcUI.sizeX * 0.5f);
    int nHalfY = static_cast<int>(rcUI.sizeY * 0.5f);
    this->SetLocalPos( D3DXVECTOR2( X_RES * 0.5f - nHalfX, Y_RES * 0.65f - nHalfY ) );
}

void MyProductProgress::StartProduct( DWORD nID, WORD nCount )
{
	CProductProgress::StartProduct( nID, nCount );
}

void MyProductProgress::GoAlongProduct()
{
	CProductProgress::GoAlongProduct();
}

void MyProductProgress::EndProduct()
{
	CProductProgress::EndProduct();
}

void MyProductProgress::CancelProduct()
{
	CProductProgress::CancelProduct();
}

bool MyProductProgress::IsProdution()
{
	return CProductProgress::IsProdution();
}
