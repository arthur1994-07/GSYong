#include "stdafx.h"

#include "./LottoBuyLottoUI.h"

#include "./LottoBuyLottoPopupUIComp.h"

#include "./LottoUIDateType.h"

#include "./GLLottoSystemManUI.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/InterfaceBaseDefine.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../EngineLib/GUInterface/UITextUtil.h"
#include "../../../EngineLib/GUInterface/UIEditBox.h"
#include "../../../EngineLib/GUInterface/UIEditBoxMan.h"
#include "../../../EngineLib/GUInterface/UIKeyCheck.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"

#include "../../InnerInterface.h"

#include "../Util/RnButton.h"

#include "../../../SigmaCore/DebugInclude.h"

// Logic;
#include "../../../RanLogicClient/LottoSystem/LottoSystemClient.h"
#include "../../../RanLogicClient/LottoSystem/GLLottoSystemManClient.h"
#include "../../../RanLogicClient/LottoSystem/GLLottoGlobalDataClient.h"

#include <boost/bind.hpp>

CLottoBuyLottoUI::CLottoBuyLottoUI( CInnerInterface* pInterface
								   , EngineDeviceMan* pEngineDevice )
								   : CUIWindowEx( pInterface
								   , pEngineDevice )
								   , m_pPopup( NULL )
{

}

void CLottoBuyLottoUI::CreateSubControl()
{
	// 팝업;
	{
		m_pPopup = new CLottoBuyLottoPopupUIComp( m_pInterface, m_pEngineDevice );
		m_pPopup->CreateSub( this, "LOTTO_BUY_LOTTO_UI", UI_FLAG_DEFAULT, 1000 );
		m_pPopup->CreateSubControl();
		RegisterControl( m_pPopup );
	}
}

void CLottoBuyLottoUI::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIWindowEx::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CLottoBuyLottoUI::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage( ControlID, dwMsg );
}

void CLottoBuyLottoUI::SetVisibleSingle( BOOL bVisible )
{
	CUIWindowEx::SetVisibleSingle( bVisible );

	m_pPopup->SetVisibleSingle( bVisible );

	// UI가 꺼지는 경우 LottoNum을 초기화한다;
	if( !bVisible && m_pPopup )
		m_pPopup->ClearLottoNum();
}

void CLottoBuyLottoUI::UpdateHaveMoney( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	if( !pLottoSystem )
		return;

	if( m_pPopup )
		m_pPopup->UpdateHaveMoney( pLottoSystem );
}

MyLottoBuyLottoUI::MyLottoBuyLottoUI( CInnerInterface* pInterface
									 , EngineDeviceMan* pEngineDevice )
									 : CLottoBuyLottoUI( pInterface
									 , pEngineDevice )
{
}

void MyLottoBuyLottoUI::CreateUIWindowAndRegisterOwnership()
{
	CLottoBuyLottoUI::Create( LOTTO_BUY_LOTTO_UI, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	CLottoBuyLottoUI::CreateBaseWindowLightGray( "LOTTO_BUY_LOTTO_UI", (char*)ID2GAMEINTEXT("LOTTO_BUY_TITLE_TEXT") );
	CLottoBuyLottoUI::CreateSubControl();
	CLottoBuyLottoUI::SetAlignFlag( UI_FLAG_CENTER_Y | UI_FLAG_RIGHT );
	CLottoBuyLottoUI::m_pInterface->UiRegisterControl( this, true );
	CLottoBuyLottoUI::m_pInterface->UiShowGroupFocus( LOTTO_BUY_LOTTO_UI );
}

bool MyLottoBuyLottoUI::IsVisible()
{
	return CLottoBuyLottoUI::IsVisible();
}

void MyLottoBuyLottoUI::UpdateHaveMoney( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	CLottoBuyLottoUI::UpdateHaveMoney( pLottoSystem );
}