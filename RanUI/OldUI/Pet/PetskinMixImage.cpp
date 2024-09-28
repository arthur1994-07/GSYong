#include "StdAfx.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../RanLogicClient/Stage/DxGlobalStage.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../InnerInterface.h"
#include "./petskinmiximage.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CPetSkinMixImage::CPetSkinMixImage(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIWindowEx(pInterface, pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pAniBox(NULL)
    , m_ItemPosX(0)
    , m_ItemPosY(0)
    , m_sItemID(NATIVEID_NULL())
{
	
}

CPetSkinMixImage::~CPetSkinMixImage(void)
{
}

void CPetSkinMixImage::CreateSubControl()
{

	CBasicAnimationBox* pAnibox = new CBasicAnimationBox(m_pEngineDevice);
	pAnibox->CreateSubEx(this,"PETSKIN_MIX_IMAGE_WINDOW",PETSKIN_MIX_IMAGE_WINDOW);
	pAnibox->CreateAni("PETMIX_ANIMATION_SHUFFLE1", CBasicAnimationBox::NORMAL_ANI);
	pAnibox->AddAni("PETMIX_ANIMATION_SHUFFLE2");
	pAnibox->AddAni("PETMIX_ANIMATION_SHUFFLE3");
	pAnibox->AddAni("PETMIX_ANIMATION_SHUFFLE4");
	pAnibox->AddAni("PETMIX_ANIMATION_SHUFFLE5");
	pAnibox->AddAni("PETMIX_ANIMATION_SHUFFLE6");
	pAnibox->AddAni("PETMIX_ANIMATION_SHUFFLE7");
	pAnibox->AddAni("PETMIX_ANIMATION_SHUFFLE8");
	pAnibox->SetAniTime(0.1f);
	pAnibox->SetEndTime(2.0f);
	pAnibox->SetLoop(FALSE);
	RegisterControl( pAnibox );
	m_pAniBox = pAnibox;

	CUIControl* pControl = CreateControl( "PETMIX_ANIMATION_IMAGE" );
//	pControl->SetAlignFlag( UI_FLAG_CENTER_X );



}


CBasicTextButton* CPetSkinMixImage::CreateTextButton ( char* szButton, UIGUID ControlID, char* szText )
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE14;
	CBasicTextButton* pTextButton = new CBasicTextButton(m_pEngineDevice);
	pTextButton->CreateSub ( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, ControlID );
	pTextButton->CreateBaseButton ( szButton, nBUTTONSIZE, CBasicButton::CLICK_FLIP, szText );
	RegisterControl ( pTextButton );
	return pTextButton;
}

void CPetSkinMixImage::SetVisibleSingle ( BOOL bVisible )
{
	CUIWindowEx::SetVisibleSingle ( bVisible );

	if( bVisible )
	{
		m_pAniBox->InitAnimateFrame();
		m_pAniBox->SetPause(FALSE);

	}
	else
	{	
		m_pInterface->UiHideGroup( GetWndID () );

	}
}

void CPetSkinMixImage::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIWindowEx::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if( m_pAniBox->GetEndAnimate() )
	{
		m_pInterface->UiHideGroup( GetWndID() );
//		m_pGaeaClient->PetSkinPackApplyEffect();

		if( m_sItemID != NATIVEID_NULL() )
		{
			GLMSG::SNETPET_SKINPACKOPEN NetMsg(m_ItemPosX,m_ItemPosY,m_sItemID);
            NetMsg.wHoldPosX = m_HoldPosX;
            NetMsg.wHoldPosY = m_HoldPosY;
			m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
		}
		
	}

}

void CPetSkinMixImage::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	if( m_pAniBox->GetPause() == FALSE ) return;

	/*if( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
	{
		switch ( ControlID )
		{

		case ET_CONTROL_BUTTON:
			m_pInterface->UiHideGroup( GetWndID() );
			break;
		}
	}*/

	//CUIWindowEx::TranslateUIMessage( ControlID, dwMsg );
}