#include "StdAfx.h"

#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"

#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLogicDataMan.h"

#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Pet/GLPetClient.h"
#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../InnerInterface.h"
#include "../Item/ItemImage.h"
#include "PetHungry.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CPetHungry::CPetHungry(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pHungry(NULL)
    , m_pGauge(NULL)
    , m_bAlarm(false)
    , m_fBLINK_TIME(0.0f)
    , m_Percent(0.0f)
    , m_pSlotImage(NULL)
{
	//SecureZeroMemory( m_pSlotImage, sizeof(m_pSlotImage) );
}

CPetHungry::~CPetHungry(void)
{
}

void CPetHungry::CreateSubControl()
{
	CItemImage* pItemImage = new CItemImage(m_pGaeaClient, m_pEngineDevice);
	pItemImage->CreateSub ( this, "PET_HUNGRY_ICON_BOX_BOX", UI_FLAG_DEFAULT, PET_HUNGRY_IMAGE );
	pItemImage->CreateSubControl ();
	pItemImage->SetVisibleSingle(FALSE);
	RegisterControl ( pItemImage );
	m_pSlotImage = pItemImage;

	m_pGauge = new CMiniGauge( m_pGaeaClient, m_pEngineDevice );
	m_pGauge->CreateSub( this, "PET_HUNGRY_ICON_GAUGE", UI_FLAG_RIGHT | UI_FLAG_BOTTOM, PET_HUNGRY_GAUGE  );
	m_pGauge->CreateSubControl();
	m_pGauge->SetVisibleSingle( FALSE );
	RegisterControl( m_pGauge );

	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub( this, "PET_HUNGRY_ICON", PET_HUGNRY_HUNGRY );
	pControl->SetVisibleSingle ( FALSE );
	RegisterControl ( pControl );
	m_pHungry = pControl;
}

void CPetHungry::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return ;

	CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( m_bAlarm )
	{
		float fBLINK_TIME_LOOP = 1.0f;

		if ( m_Percent <= 0.05f )
			fBLINK_TIME_LOOP = 0.5f;

		m_fBLINK_TIME += fElapsedTime;
		if ( fBLINK_TIME_LOOP <= m_fBLINK_TIME )
		{
			m_fBLINK_TIME = 0.0f;

			m_pHungry->SetVisibleSingle( !m_pHungry->IsVisible() );
		}
	}
}


void CPetHungry::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case PET_HUNGRY_IMAGE:
	case PET_HUNGRY_GAUGE:
	case PET_HUGNRY_HUNGRY:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
			{
				CString str;
				str.Format( ID2GAMEINTEXT("PET_HUNGRY"), static_cast<int>(m_Percent * 100.0f) );
				m_pInterface->SHOW_COMMON_LINEINFO( str, NS_UITEXTCOLOR::WHITE  );
			}

			if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				if( m_pGaeaClient->GetPetClient()->IsVALID() )
				{
					if ( m_pGaeaClient->GetTradeClient()->Valid() ) return;

					if ( m_pInterface->UiIsVisibleGroup( PET_WINDOW ) )
						m_pInterface->UiHideGroup( PET_WINDOW );
					else
						m_pInterface->UiShowGroupFocus( PET_WINDOW );			
				}
			}
		}
		break;
	}
}

void CPetHungry::SetPetHungry()
{
	m_pSlotImage->SetVisibleSingle( TRUE );

	m_pGauge->SetVisibleSingle( TRUE );
	m_pGauge->SetAutoBar( MAX_PET_SATIATION, false );
    int Satiation = m_pGaeaClient->GetPetClient()->Satiation();
    m_pGauge->GaugeUpdate( static_cast<float>(Satiation) );
    m_Percent = Satiation / static_cast<float>(MAX_PET_SATIATION);

    m_pHungry->SetVisibleSingle( FALSE );
}

void CPetHungry::ResetPetHungry()
{
	m_pSlotImage->SetVisibleSingle( FALSE );
	m_pGauge->SetVisibleSingle( FALSE );
}

void CPetHungry::SetPetItem( SNATIVEID sPetItemID )
{
    m_pSlotImage->ResetItem();

	const SITEM* pItemData = GLogicData::GetInstance().GetItem( sPetItemID);
    if ( !pItemData )
    {
        return ;
    }
	m_pSlotImage->SetItem( sPetItemID );
}

void CPetHungry::PetSatiationUpdate( int Satiation )
{
    if ( !IsVisible() )
        return;

    m_Percent = Satiation / static_cast<float>(MAX_PET_SATIATION);

    if ( m_Percent <= 0.1f )
        m_bAlarm = true;
    else
    {
        m_bAlarm = false;
        m_pHungry->SetVisibleSingle( FALSE );
    }

    m_pGauge->GaugeUpdate( static_cast<float>(Satiation) );
}
