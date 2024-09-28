#include "stdafx.h"

#include "./LottoWinManListUIComp.h"

#include "./LottoWinManListBoxUIComp.h"
#include "./LottoUIDateType.h"

#include "./GLLottoSystemManUI.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/UITextUtil.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"

#include "../../../RanLogic/GLogicEx.h"
#include "../../../RanLogic/GLUseFeatures.h"

#include "../../InnerInterface.h"

#include "../Util/RnButton.h"
#include "../Util/CheckBox.h"

#include "../../../SigmaCore/DebugInclude.h"

// Logic;
#include "../../../RanLogicClient/LottoSystem/GLLottoSystemManClient.h"
#include "../../../RanLogicClient/LottoSystem/GLLottoGlobalDataClient.h"

CLottoWinManListUIComp::CLottoWinManListUIComp( CInnerInterface* pInterface
											   , EngineDeviceMan* pEngineDevice )
											   : CUIGroupHelper( pEngineDevice )
											   , m_pInterface( pInterface )
											   , m_pListBox( NULL )
{

}

CLottoWinManListUIComp::~CLottoWinManListUIComp() { }

void CLottoWinManListUIComp::CreateSubControl()
{
	// Line Box;
	{
		CreateLineBox( "LOTTO_WIN_MAN_LIST_UI_COMP_RANK_TEXT_REGION", "WHITE_LINE_BLACK_BACKGROUND" );
		CreateLineBox( "LOTTO_WIN_MAN_LIST_UI_COMP_WIN_MAN_TEXT_REGION", "WHITE_LINE_BLACK_BACKGROUND" );
		CreateLineBox( "LOTTO_WIN_MAN_LIST_UI_COMP_WIN_COUNT_TEXT_REGION", "WHITE_LINE_BLACK_BACKGROUND" );
		CreateLineBox( "LOTTO_WIN_MAN_LIST_UI_COMP_WIN_MONEY_TEXT_REGION", "WHITE_LINE_BLACK_BACKGROUND" );

		CreateLineBox( "LOTTO_WIN_MAN_LIST_UI_COMP_RANK_CONTENT_REGION", "WHITE_LINE_DARKGRAY_BACKGROUND" );
		CreateLineBox( "LOTTO_WIN_MAN_LIST_UI_COMP_WIN_MAN_CONTENT_REGION", "WHITE_LINE_DARKGRAY_BACKGROUND" );
		CreateLineBox( "LOTTO_WIN_MAN_LIST_UI_COMP_WIN_COUNT_CONTENT_REGION", "WHITE_LINE_DARKGRAY_BACKGROUND" );
		CreateLineBox( "LOTTO_WIN_MAN_LIST_UI_COMP_WIN_MONEY_CONTENT_REGION", "WHITE_LINE_DARKGRAY_BACKGROUND" );
	}

	// Text Box;
	{
		{
			CBasicTextBox* pTextBox = new CBasicTextBox( m_pEngineDevice );
			pTextBox->CreateSub( this, "LOTTO_WIN_MAN_LIST_UI_COMP_RANK_TEXT_REGION", UI_FLAG_YSIZE );
			pTextBox->SetFont( DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_BOLD_FLAG ) );
			pTextBox->SetTextAlign( TEXT_ALIGN_CENTER_Y|TEXT_ALIGN_CENTER_X );
			pTextBox->AddText(
				ID2GAMEWORD( "LOTTO_WIN_MAIN_LIST_INFO_COLUMN_NAME_TEXT", 0 ),
				NS_UITEXTCOLOR::WHITE );
			RegisterControl( pTextBox );
		}

		{
			CBasicTextBox* pTextBox = new CBasicTextBox( m_pEngineDevice );
			pTextBox->CreateSub( this, "LOTTO_WIN_MAN_LIST_UI_COMP_WIN_MAN_TEXT_REGION", UI_FLAG_YSIZE );
			pTextBox->SetFont( DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_BOLD_FLAG ) );
			pTextBox->SetTextAlign( TEXT_ALIGN_CENTER_Y|TEXT_ALIGN_CENTER_X );
			pTextBox->AddText(
				ID2GAMEWORD( "LOTTO_WIN_MAIN_LIST_INFO_COLUMN_NAME_TEXT", 1 ),
				NS_UITEXTCOLOR::WHITE );
			RegisterControl( pTextBox );
		}

		{
			CBasicTextBox* pTextBox = new CBasicTextBox( m_pEngineDevice );
			pTextBox->CreateSub( this, "LOTTO_WIN_MAN_LIST_UI_COMP_WIN_COUNT_TEXT_REGION", UI_FLAG_YSIZE );
			pTextBox->SetFont( DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_BOLD_FLAG ) );
			pTextBox->SetTextAlign( TEXT_ALIGN_CENTER_Y|TEXT_ALIGN_CENTER_X );
			pTextBox->AddText(
				ID2GAMEWORD( "LOTTO_WIN_MAIN_LIST_INFO_COLUMN_NAME_TEXT", 2 ),
				NS_UITEXTCOLOR::WHITE );
			RegisterControl( pTextBox );
		}

		{
			CBasicTextBox* pTextBox = new CBasicTextBox( m_pEngineDevice );
			pTextBox->CreateSub( this, "LOTTO_WIN_MAN_LIST_UI_COMP_WIN_MONEY_TEXT_REGION", UI_FLAG_YSIZE );
			pTextBox->SetFont( DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_BOLD_FLAG ) );
			pTextBox->SetTextAlign( TEXT_ALIGN_CENTER_Y|TEXT_ALIGN_CENTER_X );
			pTextBox->AddText(
				ID2GAMEWORD( "LOTTO_WIN_MAIN_LIST_INFO_COLUMN_NAME_TEXT", 3 ),
				NS_UITEXTCOLOR::WHITE );
			RegisterControl( pTextBox );
		}
	}

	// List Box;
	{
		m_pListBox = new CLottoWinManListBoxUIComp( m_pInterface, m_pEngineDevice );
		m_pListBox->CreateSub( this, "LOTTO_WIN_MAN_LIST_BOX_REGION", UI_FLAG_DEFAULT );
		m_pListBox->Create( "LOTTO_WIN_MAN_LIST_BOX_PROPERTY_REGION", 0, 0, 0, 0, 0 );
		RegisterControl( m_pListBox );

		LottoSystem::LOTTO_SYSTEM_CLIENT_VALUE sValue =
			LottoSystem::GLLottoGlobalDataClient::Instance()
			->GetLottoSystemValue( GLLottoSystemManUI::Instance()->GetCurTabIndex() );

		for( int i=0; i<sValue.winTypeCount; ++i )
			m_pListBox->AddItem( "" );
	}
}

void CLottoWinManListUIComp::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroupHelper::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CLottoWinManListUIComp::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroupHelper::TranslateUIMessage( ControlID, dwMsg );
}

void CLottoWinManListUIComp::UpdateWinManList( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	if( !pLottoSystem )
		return;

	LottoSystem::LOTTO_GLOBAL_VALUE gValue = 
		LottoSystem::GLLottoGlobalDataClient::Instance()->GetLottoGlobalValue();

	LottoSystem::LOTTO_SYSTEM_CLIENT_VALUE sValue =
		LottoSystem::GLLottoGlobalDataClient::Instance()->GetLottoSystemValue(
		pLottoSystem->GetLottoSystemId() );

	if( m_pListBox )
	{
		m_pListBox->ClearItem();

		unsigned int itemCount = 0;
		for( int i=0; i<LottoSystem::MAX_RANKING_NUM; ++i )
		{
			// 설정된 최대 구매리스트 개수 만큼 추가;
			if( m_pListBox->GetTotalLine() < i+1 )
				m_pListBox->AddItem( "" );			// 기본 형식이 있으므로 값을 넣을 필요 없음;

			const LottoSystem::LOTTO_SYSTEM_WINNER_MAN_LIST winManList = 
				pLottoSystem->GetWinnerManList();

			LottoSystem::LOTTO_WINMAN_LIST_PROPERTY_MEMBER winManInfo;
			for( unsigned int j=0; j<winManList.winnerManList.size(); ++j )
			{
				LottoSystem::LOTTO_SYSTEM_WINNER_MAN winTicket =
					winManList.winnerManList.at( j );
				std::string winCharName = winManList.winnerManNameList.at( j );
				winCharName = winCharName.substr( 0, gValue.nViewCharName );
				winCharName = sc::string::format(
					ID2GAMEINTEXT("LOTTO_WIN_MAN_LIST_HIDDEN_NAME_TEXT"), winCharName );

				// 랭킹에 해당하는 Ticket이 존재할 경우;
				if( winTicket.ranking == i+1 )
				{
					winManInfo.ranking = sc::string::format(
						ID2GAMEINTEXT("LOTTO_WIN_MAN_LIST_RANKING_TEXT"),
						winTicket.ranking );
					if( winTicket.winCount > 1 )
					{
						winManInfo.name = sc::string::format(
							ID2GAMEINTEXT("LOTTO_WIN_MAN_LIST_NAME_TEXT"), winCharName );
					}
					else if( winTicket.winCount == 1 )
					{
						winManInfo.name = winCharName;
					}
					else if( winTicket.winCount == 0 )
					{
						winManInfo.name = ID2GAMEINTEXT("LOTTO_WIN_MAN_LIST_DONTHAVE_NAME_TEXT");
					}

					CString strText = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT(
						winTicket.winMoney, 3, "," );
					winManInfo.winMoney = strText.GetString();
					winManInfo.count = sc::string::format( "%d", winTicket.winCount );
				}
				
			}

			// 당첨자 정보 가져오기;			
			m_pListBox->SetItem( itemCount, "", winManInfo );
			itemCount++;
		}
	}
}