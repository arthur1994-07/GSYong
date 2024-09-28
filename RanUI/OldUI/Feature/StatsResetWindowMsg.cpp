#include "StdAfx.h"

#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"

//#include "../../ModalCallerID.h"
#include "../../InnerInterface.h"

#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "StatsResetWindow.h"
#include "StatsInfo.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void CStatsResetWindow::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage ( ControlID, dwMsg );

	const float fBUTTON_CLICK = CBasicButton::fBUTTON_CLICK;

	switch ( ControlID )
	{
	case ET_CONTROL_BUTTON:
		{
			if( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				Close ();
			}
		}
		break;

	case STATSRESET_BUTTON_OK:
		{
			if( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
                std::string strText = "";

                if (m_emType == UI::EMSTATSRESETWINDOW_TYPE_SKILL ||
                    m_emType == UI::EMSTATSRESETWINDOW_TYPE_STATS )
                    strText = sc::string::format( "%1%", ID2GAMEINTEXT("MODAL_RESET_SKILLSTATS_ALL") );
                else
                    strText = sc::string::format( ID2GAMEINTEXT("MODAL_RESET_SKILLSTATS"), (m_wReturnStatsMax - m_wReturnStats) );

                m_pInterface->DoModal( strText,  UI::MODAL_INFOMATION, UI::YESNO, UI::MODAL_RESET_SKILLSTATS );
			}
		}
		break;

	case STATSRESET_BUTTON_CANCEL:
		{
			if( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				Close ();
			}
		}
		break;

	case STATSRESET_BUTTON_RESET:
		{
			if( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				ResetStatsControl ();
			}
		}
		break;

		///////////////////////////////////////////////////////////
	case ET_CONTROL_TITLE:
	case ET_CONTROL_TITLE_F:
		{
			if ( (dwMsg & UIMSG_LB_DUP) && CHECK_MOUSE_IN ( dwMsg ) )
			{
				m_pInterface->SetDefaultPosInterface( STATSRESET_WINDOW );
			}
		}
		break;

	case STATSRESET_STATSINFO_PREVIEW:
		{
			if ( dwMsg & UIMSG_BUTTON_DOWN_STATSITEM )
			{
				if ( m_wReturnStats < m_wReturnStatsMax )
				{
					++m_wReturnStats;
				}
			}
			else if ( dwMsg & UIMSG_BUTTON_UP_STATSITEM )
			{
				if ( 0 < m_wReturnStats )
				{
					--m_wReturnStats;
				}
			}
		}
		break;
		/*
	case STATSRESET_POW_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				//m_sStatsText.pPow->SetUseTextColor ( 0, TRUE );
				//m_sStatsText.pPow->SetTextColor ( 0, NS_UITEXTCOLOR::GOLD );

				//m_sStatsText.pPowStatic->SetUseTextColor ( 0, TRUE );
				//m_sStatsText.pPowStatic->SetTextColor ( 0, NS_UITEXTCOLOR::GOLD );

				if ( CHECK_LB_DOWN_LIKE ( dwMsg ) )
				{
					const UIRECT& rcPos = m_pPowDisableButton->GetGlobalPos ();				
					m_pPowButton->SetGlobalPos ( D3DXVECTOR2 ( rcPos.left + fBUTTON_CLICK, rcPos.top + fBUTTON_CLICK ) );
				}

				if ( CHECK_LB_UP_LIKE ( dwMsg ) )
				{					
					m_wResetStats[EMPOW]++;
				}
			}
		}
		break;

	case STATSRESET_DEX_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				//m_sStatsText.pDex->SetUseTextColor ( 0, TRUE );
				//m_sStatsText.pDex->SetTextColor ( 0, NS_UITEXTCOLOR::GOLD  );

				//m_sStatsText.pDexStatic->SetUseTextColor ( 0, TRUE );
				//m_sStatsText.pDexStatic->SetTextColor ( 0, NS_UITEXTCOLOR::GOLD  );

				if ( CHECK_LB_DOWN_LIKE ( dwMsg ) )
				{
					const UIRECT& rcPos = m_pDexDisableButton->GetGlobalPos ();				
					m_pDexButton->SetGlobalPos ( D3DXVECTOR2 ( rcPos.left + fBUTTON_CLICK, rcPos.top + fBUTTON_CLICK ) );
				}

				if ( CHECK_LB_UP_LIKE ( dwMsg ) )
				{					
					m_wResetStats[EMDEX]++;
				}
			}
		}
		break;

	case STATSRESET_SPI_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				//m_sStatsText.pSpi->SetUseTextColor ( 0, TRUE );
				//m_sStatsText.pSpi->SetTextColor ( 0, NS_UITEXTCOLOR::GOLD  );

				//m_sStatsText.pSpiStatic->SetUseTextColor ( 0, TRUE );
				//m_sStatsText.pSpiStatic->SetTextColor ( 0, NS_UITEXTCOLOR::GOLD  );

				if ( CHECK_LB_DOWN_LIKE ( dwMsg ) )
				{
					const UIRECT& rcPos = m_pSpiDisableButton->GetGlobalPos ();				
					m_pSpiButton->SetGlobalPos ( D3DXVECTOR2 ( rcPos.left + fBUTTON_CLICK, rcPos.top + fBUTTON_CLICK ) );
				}

				if ( CHECK_LB_UP_LIKE ( dwMsg ) )
				{					
					m_wResetStats[EMSPI]++;
				}
			}
		}
		break;

	//case CHARACTER_INT_BUTTON:
	//	{
	//		if ( CHECK_MOUSE_IN ( dwMsg ) )
	//		{
	//			m_sStatsText.pInt->SetUseTextColor ( 0, TRUE );
	//			m_sStatsText.pInt->SetTextColor ( 0, NS_UITEXTCOLOR::GOLD  );

	//			m_sStatsText.pIntStatic->SetUseTextColor ( 0, TRUE );
	//			m_sStatsText.pIntStatic->SetTextColor ( 0, NS_UITEXTCOLOR::GOLD  );

	//			if ( CHECK_LB_DOWN_LIKE ( dwMsg ) )
	//			{
	//				const UIRECT& rcPos = m_pIntDisableButton->GetGlobalPos ();				
	//				m_pIntButton->SetGlobalPos ( D3DXVECTOR2 ( rcPos.left + fBUTTON_CLICK, rcPos.top + fBUTTON_CLICK ) );
	//			}

	//			if ( CHECK_LB_UP_LIKE ( dwMsg ) )
	//			{					
	//				m_pGaeaClient->GetCharacter()->ReqStatsUp ( EMINT );
	//			}
	//		}
	//	}
	//	break;

	case STATSRESET_STR_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				//m_sStatsText.pStr->SetUseTextColor ( 0, TRUE );
				//m_sStatsText.pStr->SetTextColor ( 0, NS_UITEXTCOLOR::GOLD  );

				//m_sStatsText.pStrStatic->SetUseTextColor ( 0, TRUE );
				//m_sStatsText.pStrStatic->SetTextColor ( 0, NS_UITEXTCOLOR::GOLD  );

				if ( CHECK_LB_DOWN_LIKE ( dwMsg ) )
				{
					const UIRECT& rcPos = m_pStrDisableButton->GetGlobalPos ();				
					m_pStrButton->SetGlobalPos ( D3DXVECTOR2 ( rcPos.left + fBUTTON_CLICK, rcPos.top + fBUTTON_CLICK ) );
				}

				if ( CHECK_LB_UP_LIKE ( dwMsg ) )
				{					
					m_wResetStats[EMSTR]++;
				}
			}
		}
		break;

	case STATSRESET_STA_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				//m_sStatsText.pSta->SetUseTextColor ( 0, TRUE );
				//m_sStatsText.pSta->SetTextColor ( 0, NS_UITEXTCOLOR::GOLD  );

				//m_sStatsText.pStaStatic->SetUseTextColor ( 0, TRUE );
				//m_sStatsText.pStaStatic->SetTextColor ( 0, NS_UITEXTCOLOR::GOLD  );

				if ( CHECK_LB_DOWN_LIKE ( dwMsg ) )
				{
					const UIRECT& rcPos = m_pStaDisableButton->GetGlobalPos ();				
					m_pStaButton->SetGlobalPos ( D3DXVECTOR2 ( rcPos.left + fBUTTON_CLICK, rcPos.top + fBUTTON_CLICK ) );
				}

				if ( CHECK_LB_UP_LIKE ( dwMsg ) )
				{				
					m_wResetStats[EMSTA]++;	
				}
			}
		}
		break;
		*/
	}
}
