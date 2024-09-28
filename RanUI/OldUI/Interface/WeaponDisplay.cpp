#include "StdAfx.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"

#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLogicDataMan.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"

#include "../../InnerInterface.h"
#include "../Item/ItemImage.h"
#include "WeaponDisplay.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CWeaponDisplay::CWeaponDisplay(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice) 
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pWeaponImage ( NULL )
{
}

CWeaponDisplay::~CWeaponDisplay ()
{
}

void  CWeaponDisplay::CreateSubControl ()
{
	CItemImage* pItemImage = new CItemImage(m_pGaeaClient, m_pEngineDevice);
	pItemImage->CreateSub ( this, "WEAPON_DISPLAY_IMAGE" );
	pItemImage->CreateSubControl ();
	pItemImage->SetUseRender ( TRUE );
	pItemImage->SetVisibleSingle ( FALSE );
	RegisterControl ( pItemImage );
    m_pWeaponImage = pItemImage;

	CUIControl* pDummyControl = new CUIControl(m_pEngineDevice);
	pDummyControl->CreateSub ( this, "WEAPON_DISPLAY_DUMMY");
	RegisterControl ( pDummyControl );
	m_pDummyControl = pDummyControl;
}

void CWeaponDisplay::SetItem ( const SITEMCUSTOM& sItemCustom )
{
	if ( m_pWeaponImage )
	{
		m_pWeaponImage->SetItem ( sItemCustom );
		m_pWeaponImage->SetVisibleSingle ( TRUE );
	}
}

void CWeaponDisplay::ResetItem ()
{
	if ( m_pWeaponImage )
	{
		m_pWeaponImage->SetVisibleSingle ( FALSE );
		m_pWeaponImage->ResetItem ();
	}
}

void CWeaponDisplay::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
	
	EMSLOT emRHand = m_pGaeaClient->GetCharacter()->GetCurRHand();

	SITEMCUSTOM sItem =  m_pGaeaClient->GetCharacter()->GET_SLOT_ITEM(emRHand);

	if ( m_sNativeID != sItem.GetNativeID() ) 
	{
		m_sNativeID = sItem.GetNativeID();
		
		if ( m_sNativeID.IsValidNativeID() )
		{
			const SITEM* pItem = GLogicData::GetInstance().GetItem ( m_sNativeID );			
			if (!pItem)
			{
				GASSERT ( 0 && "등록되지 않은 ID입니다." );
				return;
			}

            m_sWeaponStr = sItem.GETNAME().c_str();
			ResetItem ();	
			SetItem ( m_sNativeID );
			m_pDummyControl->SetVisibleSingle( true );
		}
		else
		{
			m_sWeaponStr = "";
			ResetItem ();
			m_pDummyControl->SetVisibleSingle( false );
		}
	}

	if ( CHECK_MOUSE_IN ( GetMessageEx () ) )
	{
		m_pInterface->SHOW_COMMON_LINEINFO ( m_sWeaponStr, NS_UITEXTCOLOR::GREENYELLOW );
	}


}