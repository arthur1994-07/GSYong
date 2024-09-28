#include "stdafx.h"

#include "./LottoWinNumUIComp.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"

#include "../../../RanLogic/GLogicEx.h"
#include "../../../RanLogic/GLUseFeatures.h"

#include "../../InnerInterface.h"

#include "../Util/RnButton.h"
#include "../Util/CheckBox.h"

#include "../../../SigmaCore/DebugInclude.h"

#include "../../../RanLogicClient/LottoSystem/GLLottoGlobalDataClient.h"
#include "../../../RanLogicClient/LottoSystem/GLLottoSystemManClient.h"

CLottoWinNumUIComp::CLottoWinNumUIComp( CInnerInterface* pInterface
									   , EngineDeviceMan* pEngineDevice )
									   : CUIGroupHelper( pEngineDevice )
									   , m_pInterface( pInterface )
									   , m_pBonusSlot( NULL )
{
	memset( m_pSlot, 0, sizeof(CSwapImage*)*LottoSystem::LOTTO_SLOT_NUM_CONT );
}

CLottoWinNumUIComp::~CLottoWinNumUIComp() { }

void CLottoWinNumUIComp::CreateSubControl()
{
	// Num Slot Line;
	{
		static std::string strSlotSizeId[ LottoSystem::LOTTO_SLOT_NUM_CONT ] = {
			"LOTTO_WIN_NUM_COMP_MAIN_NUM_SLOT1",
			"LOTTO_WIN_NUM_COMP_MAIN_NUM_SLOT2",
			"LOTTO_WIN_NUM_COMP_MAIN_NUM_SLOT3",
			"LOTTO_WIN_NUM_COMP_MAIN_NUM_SLOT4",
			"LOTTO_WIN_NUM_COMP_MAIN_NUM_SLOT5",
			"LOTTO_WIN_NUM_COMP_MAIN_NUM_SLOT6",
		};

		for( unsigned int i=0; i<LottoSystem::LOTTO_SLOT_NUM_CONT; ++i )
		{	
			CSwapImage* pImage = new CSwapImage( m_pEngineDevice );
			pImage->CreateSub ( this, strSlotSizeId[ i ].c_str(), UI_FLAG_DEFAULT );
			pImage->SetImage( "BLACK_LINE_DARKGRAY_BACKGROUND" );
			RegisterControl( pImage );
		}

		{
			CSwapImage* pImage = new CSwapImage( m_pEngineDevice );
			pImage->CreateSub ( this, "LOTTO_WIN_NUM_COMP_BONUS_NUM_SLOT1", UI_FLAG_DEFAULT );
			pImage->SetImage( "BLACK_LINE_DARKGRAY_BACKGROUND" );
			RegisterControl( pImage );
		}
	}

	// Num Slot;
	{
		static std::string strSlotSizeId[ LottoSystem::LOTTO_SLOT_NUM_CONT ] = {
			"LOTTO_WIN_NUM_COMP_MAIN_NUM_SLOT_BALL1",
			"LOTTO_WIN_NUM_COMP_MAIN_NUM_SLOT_BALL2",
			"LOTTO_WIN_NUM_COMP_MAIN_NUM_SLOT_BALL3",
			"LOTTO_WIN_NUM_COMP_MAIN_NUM_SLOT_BALL4",
			"LOTTO_WIN_NUM_COMP_MAIN_NUM_SLOT_BALL5",
			"LOTTO_WIN_NUM_COMP_MAIN_NUM_SLOT_BALL6",
		};

		for( unsigned int i=0; i<LottoSystem::LOTTO_SLOT_NUM_CONT; ++i )
		{	
			m_pSlot[ i ] = new CSwapImage( m_pEngineDevice );
			m_pSlot[ i ]->CreateSub ( this, strSlotSizeId[ i ].c_str(), UI_FLAG_DEFAULT );
			m_pSlot[ i ]->SetImage( "BLACK_LINE_DARKGRAY_BACKGROUND" );
			m_pSlot[ i ]->SetVisibleSingle( FALSE );
			RegisterControl( m_pSlot[ i ] );
		}

		{
			m_pBonusSlot = new CSwapImage( m_pEngineDevice );
			m_pBonusSlot->CreateSub ( this, "LOTTO_WIN_NUM_COMP_BONUS_NUM_SLOT_BALL1", UI_FLAG_DEFAULT );
			m_pBonusSlot->SetImage( "BLACK_LINE_DARKGRAY_BACKGROUND" );
			m_pBonusSlot->SetVisibleSingle( FALSE );
			RegisterControl( m_pBonusSlot );
		}
	}
}

void CLottoWinNumUIComp::SetVisibleSingle( BOOL bVisible )
{
	CUIGroupHelper::SetVisibleSingle( bVisible );
}

void CLottoWinNumUIComp::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroupHelper::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CLottoWinNumUIComp::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroupHelper::TranslateUIMessage( ControlID, dwMsg );
}

static void SetNumImageId( CSwapImage* pImage, unsigned int num )
{
	if( !pImage )
		return;

	std::string numTextureId = "LOTTO_BALL_";
	numTextureId += sc::string::format( "%u", num );

	if( num <= 0 )
	{
		pImage->SetImage( "BLACK_LINE_DARKGRAY_BACKGROUND" );
		pImage->SetVisibleSingle( FALSE );
	}
	else
	{
		pImage->SetImage( numTextureId.c_str() );
		pImage->SetVisibleSingle( TRUE );
	}
}

void CLottoWinNumUIComp::SetNumImage( int idx, unsigned int num )
{
	LottoSystem::LOTTO_SYSTEM_CLIENT_VALUE sValue =
		LottoSystem::GLLottoGlobalDataClient::Instance()
		->GetLottoSystemValue(
		GLLottoSystemManUI::Instance()->GetCurTabIndex() );

	if( idx >= 0 && idx < LottoSystem::LOTTO_SLOT_NUM_CONT )
		SetNumImageId( m_pSlot[ idx ], num );

	if( idx >= static_cast<int>( LottoSystem::LOTTO_SLOT_NUM_CONT ) && 
		idx < static_cast<int>( LottoSystem::LOTTO_SLOT_NUM_CONT+sValue.nDrawingBonus ) )
		SetNumImageId( m_pBonusSlot, num );
}