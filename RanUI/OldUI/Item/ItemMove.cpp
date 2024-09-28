#include "StdAfx.h"
#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLogicDataMan.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/StaticHoldItem.h"

#include "../../InnerInterface.h"
#include "../Item/ItemImage.h"
#include "ItemMove.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const DWORD CItemMove::dwDEFAULT_TRANSPARENCY = D3DCOLOR_ARGB( 160, 255, 255, 255 );
const float CItemMove::fDEFAULT_MOUSE_INTERPOLIATION = 20.0f;

CItemMove::CItemMove(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
	, m_pInterface( pInterface )
    , m_pItemImage( NULL )
{
}

CItemMove::~CItemMove()
{
}

void CItemMove::CreateSubControl()
{
	CItemImage* pItemImage = new CItemImage(m_pGaeaClient, m_pEngineDevice);
	pItemImage->CreateSub( this, "SKILL_WINDOW_TO_TARY_IMAGE" );
	pItemImage->CreateSubControl();
	pItemImage->SetUseRender( TRUE );
	pItemImage->SetVisibleSingle( FALSE );
	RegisterControl( pItemImage );
    m_pItemImage = pItemImage;

	ResetItemIcon();
}

SNATIVEID	CItemMove::GetItem()
{
	//	Note : 트래이드에 등록할 아이템이 있을 경우.
	//
	SITEMCUSTOM sItemCustom(NATIVEID_NULL());
	
	//	Note : 트레이드에 등록할 아이템과 개조에 등록할 아이템 모두 없을 경우에는 손에든 아이템을 뿌려줌.
	//
	if ( m_pGaeaClient->GetCharacter()->GET_PRETRADE_ITEM( sItemCustom ) == FALSE )
    {
        // TODO : Hold Item 없애기.
        if ( holditem::HaveHoldItem() )
            sItemCustom = holditem::GetHoldItem();
        else
    		sItemCustom = m_pGaeaClient->GetCharacter()->GET_HOLD_ITEM();
    }

	return sItemCustom.GetNativeID();
}

void CItemMove::SetItemIcon( const SITEMCUSTOM& sItemCustom )
{	
	if ( m_pItemImage )
	{
		m_pItemImage->SetItem( sItemCustom );
		m_pItemImage->SetVisibleSingle( TRUE );
		m_pItemImage->SetDiffuse( dwDEFAULT_TRANSPARENCY );
	}
}

void  CItemMove::ResetItemIcon()
{
	if ( m_pItemImage )
	{
		m_pItemImage->SetVisibleSingle( FALSE );
		m_pItemImage->ResetItem();
	}
}

SNATIVEID	CItemMove::GetItemIcon()
{
	if ( m_pItemImage )
	{
		return m_pItemImage->GetItemImageSlotPos();
	}
	return NATIVEID_NULL();
}

void CItemMove::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );	
	
	if ( !m_pItemImage ) return ;


	SITEMCUSTOM sItemCustom( NATIVEID_NULL() );
	//	Note : 트레이드에 등록할 아이템과 개조에 등록할 아이템 모두 없을 경우에는 손에든 아이템을 뿌려줌.
	if ( m_pGaeaClient->GetCharacter()->GET_PRETRADE_ITEM( sItemCustom ) == FALSE )
		sItemCustom = m_pGaeaClient->GetCharacter()->GET_PREHOLD_ITEM();

	if ( sItemCustom.GetNativeID() == NATIVEID_NULL() )
    {
        // TODO : Hold Item 없애기.
        if ( holditem::HaveHoldItem() )
            sItemCustom = holditem::GetHoldItem();
        else
		    sItemCustom = m_pGaeaClient->GetCharacter()->GET_HOLD_ITEM();
    }

	if ( sItemCustom.GetNativeID() != NATIVEID_NULL() )
	{
		const SITEM* pItemData = GLogicData::GetInstance().GetItem( sItemCustom.GetNativeID() );
		if ( pItemData ) 
		{
			if ( pItemData->sBasicOp.sICONID != GetItemIcon() ||
				 pItemData->sBasicOp.strInventoryFile.c_str() != GetItemIconTexture() )
			{
				SetItemIcon( sItemCustom );
			}

			if ( !m_pInterface->IsSnapItem() )
			{
				const D3DXVECTOR2 vImagePos( x - fDEFAULT_MOUSE_INTERPOLIATION, y - fDEFAULT_MOUSE_INTERPOLIATION );
				SetGlobalPos( vImagePos );
			}

			//	만약, 모달이 떠 있는 상태라면
			//	손에 든 이미지를 잠시 꺼 둡니다.
			if ( m_pInterface->UiIsVisibleGroup( MODAL_WINDOW ) )
			{
				m_pItemImage->SetVisibleSingle( FALSE );			
			}
			else
			{
				m_pItemImage->SetVisibleSingle( TRUE );
				m_pItemImage->SetDiffuse( dwDEFAULT_TRANSPARENCY );
			}
		}
	}
	else
	{
		ResetItemIcon();
	}
}

const CString&	CItemMove::GetItemIconTexture() const
{
	if ( m_pItemImage )
	{
		return m_pItemImage->GetItemTextureName();
	}

	static CString strNULL;
	return strNULL;
}

MyItemMove::MyItemMove(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CItemMove(pGaeaClient, pInterface, pEngineDevice)
{
}

void MyItemMove::CreateUIWindowAndRegisterOwnership()
{
    CItemMove::Create( ITEM_MOVE, "ITEM_MOVE" );
    CItemMove::CreateSubControl();
    CItemMove::m_pInterface->UiRegisterControl( this );
    CItemMove::m_pInterface->UiShowGroupTop( ITEM_MOVE );
}

const UIRECT& MyItemMove::GetUIWindowGlobalPos()
{
    return CItemMove::GetGlobalPos();
}

void MyItemMove::SetUIWindowGlobalPos( const UIRECT& rcPos )
{
    CItemMove::SetGlobalPos( rcPos );
}

SNATIVEID MyItemMove::GetItem()
{
    return CItemMove::GetItem();
}