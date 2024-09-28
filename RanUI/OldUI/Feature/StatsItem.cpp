#include "StdAfx.h"
#include "StatsInfo.h"

#include "../../InnerInterface.h"

#include "../../../RanLogic/GLogicEx.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"

#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/DxTools/d3dfont.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CStatsItem::CStatsItem(EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_dwType				    (0)
    , m_pLabel				    (NULL)
    , m_pStats				    (NULL)
    , m_pButton_Down		    (NULL)
    , m_pButton_Up			    (NULL)
    , m_pButton_Down_Disable    (NULL)
    , m_pButton_Up_Disable      (NULL)
    , m_nStatsBasic			    (0)
    , m_nStatsAdd			    (0)
    , m_nMin				    (0)
    , m_nMax				    (0)
    , m_bButton_Down_Disable    (FALSE)
    , m_bButton_Up_Disable      (FALSE)
    , m_dwButton_Down_PressTime (0)
    , m_dwButton_Up_PressTime	(0)
    , m_bButton_Down_Press		(FALSE)
    , m_bButton_Up_Press		(FALSE)
{
}

CStatsItem::~CStatsItem()
{
}

void CStatsItem::CreateSubControl( const CString& strLabel )
{
	CD3DFontPar* pFont8Shadow = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 8, D3DFONT_SHADOW | D3DFONT_ASCII );
	CD3DFontPar* pFont        = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	const int nAlignLeft    = TEXT_ALIGN_LEFT;
	const int nAlignCenter  = TEXT_ALIGN_CENTER_X;		
	const DWORD& dwSilver   = NS_UITEXTCOLOR::SILVER;
	const DWORD& dwSubTitle = NS_UITEXTCOLOR::PALEGOLDENROD;

	// Note : 정적 텍스트
	{
		m_pLabel = CreateStaticControl ( "STATSITEM_LABEL", pFont, nAlignLeft );
		m_pLabel->AddText ( strLabel, dwSilver );
	}

	// Note : 수치값
	{
		m_pStats = CreateStaticControl ( "STATSITEM_STATS", pFont8Shadow, nAlignLeft );
	}

	// Note : 버튼
	{
		// Note : 다운
		m_pButton_Down_Disable = new CUIControl(m_pEngineDevice);
		m_pButton_Down_Disable->CreateSub ( this, "STATSITEM_BUTTON_DOWN_DISABLE", UI_FLAG_DEFAULT, NO_ID );
		RegisterControl ( m_pButton_Down_Disable );

		m_pButton_Down = new CBasicButton(m_pEngineDevice);
		m_pButton_Down->CreateSub ( this, "STATSITEM_BUTTON_DOWN", UI_FLAG_DEFAULT, STATSINFO_BUTTON_DOWN );
		m_pButton_Down->CreateFlip ( "STATSITEM_BUTTON_DOWN_F", CBasicButton::MOUSEIN_FLIP );
		RegisterControl ( m_pButton_Down );

		// Note : 업
		m_pButton_Up_Disable = new CUIControl(m_pEngineDevice);
		m_pButton_Up_Disable->CreateSub ( this, "STATSITEM_BUTTON_UP_DISABLE", UI_FLAG_DEFAULT, NO_ID );
		RegisterControl ( m_pButton_Up_Disable );

		m_pButton_Up = new CBasicButton(m_pEngineDevice);
		m_pButton_Up->CreateSub ( this, "STATSITEM_BUTTON_UP", UI_FLAG_DEFAULT, STATSINFO_BUTTON_UP );
		m_pButton_Up->CreateFlip ( "STATSITEM_BUTTON_UP_F", CBasicButton::MOUSEIN_FLIP );
		RegisterControl ( m_pButton_Up );

		m_pButton_Down_Disable->SetVisibleSingle( FALSE );
		m_pButton_Up_Disable  ->SetVisibleSingle( FALSE );
		m_pButton_Down		  ->SetVisibleSingle( FALSE );
		m_pButton_Up		  ->SetVisibleSingle( FALSE );
	}
}

CBasicTextBox* CStatsItem::CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, szControlKeyword, UI_FLAG_DEFAULT, cID );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );	
	RegisterControl ( pStaticText );

	return pStaticText;
}

void CStatsItem::SetType ( const DWORD dwType )
{
	m_dwType = dwType;

	if ( m_dwType&EMSTATSITEMTYPE_CONTROLLER_BUTTON )
	{
		if ( m_nMin < m_nStatsAdd )
		{
			m_pButton_Down->SetVisibleSingle( TRUE );
		}
		else
		{
			m_pButton_Down->SetVisibleSingle( FALSE );
		}

		if ( m_nStatsAdd < m_nMax )
		{
			m_pButton_Up->SetVisibleSingle( TRUE );
		}
		else
		{
			m_pButton_Up->SetVisibleSingle( FALSE );
		}
	}
	else
	{
		m_pButton_Down->SetVisibleSingle( FALSE );
		m_pButton_Up  ->SetVisibleSingle( FALSE );
	}
}

void CStatsItem::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	// Note : 값 무결성 체크
	if ( m_nStatsAdd < m_nMin )
	{
		m_nStatsAdd = m_nMin;
	}
	else if ( m_nMax <= m_nStatsAdd )
	{
		m_nStatsAdd = m_nMax;
	}

	// Note : 텍스트/버튼 세팅
	m_pStats->ClearText();

	INT32 nTotal = m_nStatsBasic + m_nStatsAdd;

	CString strCombine;
	strCombine.Format ( "%d", nTotal );
	int nIndex = m_pStats->AddText ( strCombine, NS_UITEXTCOLOR::WHITE );

	if ( 0 <= m_nStatsAdd )
	{
		strCombine.Format ( "(+%d)", (INT32)abs(m_nStatsAdd) );
	}
	else
	{
		strCombine.Format ( "(-%d)", (INT32)abs(m_nStatsAdd) );
	}

	D3DCOLOR dwColor = NS_UITEXTCOLOR::WHITE;
	//if ( sITEM.wPow ) dwColor = NS_UITEXTCOLOR::PLUSOPTION;

	m_pStats->AddString ( nIndex, strCombine, dwColor );	

	if ( m_dwType&EMSTATSITEMTYPE_CONTROLLER_BUTTON )
	{
		const UIRECT& rcDownPos = m_pButton_Down_Disable->GetGlobalPos ();
		m_pButton_Down->SetGlobalPos ( D3DXVECTOR2 ( rcDownPos.left, rcDownPos.top ) );

		const UIRECT& rcUpPos = m_pButton_Up_Disable->GetGlobalPos ();
		m_pButton_Up->SetGlobalPos ( D3DXVECTOR2 ( rcUpPos.left, rcUpPos.top ) );

		// Note : 비활성화 상태이거나 최소/최대값이면 버튼을 끈다
		if  ( m_bButton_Down_Disable || (m_nStatsAdd <= m_nMin) )
		{
			m_pButton_Down->SetVisibleSingle( FALSE );
		}
		else
		{
			m_pButton_Down->SetVisibleSingle( TRUE );
		}

		if ( m_bButton_Up_Disable || (m_nStatsAdd >= m_nMax) )
		{
			m_pButton_Up->SetVisibleSingle( FALSE );
		}
		else
		{
			m_pButton_Up->SetVisibleSingle( TRUE );
		}
	}

	m_pLabel->SetUseTextColor ( 0, FALSE );
	m_pStats->SetUseTextColor ( 0, FALSE );

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CStatsItem::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	//m_sStatsText.pPow->SetUseTextColor ( 0, TRUE );
	//m_sStatsText.pPow->SetTextColor ( 0, NS_UITEXTCOLOR::GOLD );

	//m_sStatsText.pPowStatic->SetUseTextColor ( 0, TRUE );
	//m_sStatsText.pPowStatic->SetTextColor ( 0, NS_UITEXTCOLOR::GOLD );


	if ( !CHECK_LB_DOWN_LIKE ( dwMsg ) )
	{
		m_bButton_Down_Press = FALSE;
		m_bButton_Up_Press   = FALSE;
	}

	const float fBUTTON_CLICK = CBasicButton::fBUTTON_CLICK;

	switch ( ControlID )
	{
	case STATSINFO_BUTTON_DOWN:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				m_pLabel->SetUseTextColor ( 0, TRUE );
				m_pLabel->SetTextColor    ( 0, NS_UITEXTCOLOR::GOLD );

				m_pStats->SetUseTextColor ( 0, TRUE );
				m_pStats->SetTextColor    ( 0, NS_UITEXTCOLOR::GOLD );

				if ( CHECK_LB_DOWN_LIKE(dwMsg) )
				{
					const UIRECT& rcPos = m_pButton_Down_Disable->GetGlobalPos ();				
					m_pButton_Down->SetGlobalPos ( D3DXVECTOR2 ( rcPos.left + fBUTTON_CLICK, rcPos.top + fBUTTON_CLICK ) );
				}

				if ( dwMsg&UIMSG_LB_DOWN )
				{
					m_dwButton_Down_PressTime = timeGetTime();
					m_bButton_Down_Press      = TRUE;

					DecStatsAdd();
				}

				if ( m_bButton_Down_Press )
				{
					DWORD dwCurTime = timeGetTime();
					if ( dwPRESSDELAY_START <= (dwCurTime - m_dwButton_Down_PressTime) )
					{
						DecStatsAdd();
					}
				}
			}
		}
		break;

	case STATSINFO_BUTTON_UP:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				m_pLabel->SetUseTextColor ( 0, TRUE );
				m_pLabel->SetTextColor    ( 0, NS_UITEXTCOLOR::GOLD );

				m_pStats->SetUseTextColor ( 0, TRUE );
				m_pStats->SetTextColor    ( 0, NS_UITEXTCOLOR::GOLD );

				if ( CHECK_LB_DOWN_LIKE(dwMsg) )
				{
					const UIRECT& rcPos = m_pButton_Up_Disable->GetGlobalPos ();				
					m_pButton_Up->SetGlobalPos ( D3DXVECTOR2 ( rcPos.left + fBUTTON_CLICK, rcPos.top + fBUTTON_CLICK ) );
				}

				if ( dwMsg&UIMSG_LB_DOWN )
				{
					m_dwButton_Up_PressTime = timeGetTime();
					m_bButton_Up_Press      = TRUE;

					IncStatsAdd();
				}

				if ( m_bButton_Up_Press )
				{
					DWORD dwCurTime = timeGetTime();
					if ( dwPRESSDELAY_START <= (dwCurTime - m_dwButton_Up_PressTime) )
					{
						IncStatsAdd();
					}
				}
			}
		}
		break;
	};
}

void CStatsItem::IncStatsAdd()
{
	if ( m_nStatsAdd < m_nMax )
	{
		++m_nStatsAdd;
		AddMessageEx ( UIMSG_BUTTON_UP_STATSITEM );
	}
}

void CStatsItem::DecStatsAdd()
{
	if ( m_nStatsAdd > m_nMin )
	{
		--m_nStatsAdd;
		AddMessageEx ( UIMSG_BUTTON_DOWN_STATSITEM );
	}	
}

