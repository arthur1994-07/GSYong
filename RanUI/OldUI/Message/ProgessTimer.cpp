#include "StdAfx.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../InnerInterface.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../enginelib/GUInterface/BasicLineBoxSmart.h"

#include "../../../RanLogicClient/Char/GLCharacter.h"

#include "./ProgessTimer.h"

ProgessTimer::ProgessTimer( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CUIGroup( pEngineDevice )
, m_pProgessText(NULL)
, m_pWatchIcon(NULL)
, m_pBlank(NULL)
, m_pGauge(NULL)
, m_fMaxTime(0.0f)
, m_pGaeaClient(pGaeaClient)
, m_pInterface(pInterface)
, m_nTextIndex(0)
{
}

void ProgessTimer::CreateSubControl()
{
	{
		CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );
		pLinBoxSmart->CreateSub( this, "PROGESS_TIMER_BLANK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pLinBoxSmart->CreateSubControl( "PROGESS_TIMER_BLANK_LINE" );
		RegisterControl( pLinBoxSmart );
		m_pBlank = pLinBoxSmart;
	}

	{
		CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );
		pLinBoxSmart->CreateSub( this, "PROGESS_TIMER_GAUGE", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pLinBoxSmart->CreateSubControl( "PROGESS_TIMER_GAUGE_LINE" );
		RegisterControl( pLinBoxSmart );
		m_pGauge = pLinBoxSmart;
	}

	{
		CD3DFontPar* pFont16Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 16, _DEFAULT_FONT_SHADOW_FLAG | D3DFONT_BOLD );

		CBasicTextBox* pTextBox = new CBasicTextBox( m_pEngineDevice );
		pTextBox->CreateSub( this, "PROGESS_TIMER_TEXT", UI_FLAG_DEFAULT );
		pTextBox->SetFont( pFont16Shadow );
		pTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
		RegisterControl( pTextBox );
		m_pProgessText = pTextBox;
	}

	{
		CUIControl* pControl = new CUIControl( m_pEngineDevice );
		pControl->CreateSub( this, "PROGESS_TIMER_WATCH_ICON", UI_FLAG_DEFAULT );
		RegisterControl( pControl );
		m_pWatchIcon = pControl;
	}
}

void ProgessTimer::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible() )  return;

	CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	m_fTime -= fElapsedTime;

	if ( m_fTime <= 0.0f )
		m_fTime = 0.0f;

	{
		float fPercent = m_fTime / m_fMaxTime;

		UIRECT rcGaugePos = m_pGauge->GetGlobalPos();
		UIRECT rcBlankPos = m_pBlank->GetGlobalPos();

		UIRECT rcNewPos = rcBlankPos;
		rcNewPos.sizeX = floor(rcNewPos.sizeX * fPercent);
		rcNewPos.right = rcNewPos.left + rcNewPos.sizeX;

		m_pGauge->SetLocalPos( D3DXVECTOR2( 0.0f, 0.0f ) );
		m_pGauge->ReSizeControl( rcGaugePos, rcNewPos );

		D3DXVECTOR2 vPos;
		vPos.x = m_pBlank->GetLocalPos().left;
		vPos.y = m_pBlank->GetLocalPos().top;

		m_pGauge->SetLocalPos( vPos );
	}

	std::string strText = "";
	strText = sc::string::format( "%.0f ", m_fTime );

	m_pProgessText->ClearText();
	int nIndex = m_pProgessText->AddText( strText.c_str(), NS_UITEXTCOLOR::RED );

	strText = sc::string::format( ID2GAMEINTEXT( m_strText.c_str(), m_nTextIndex ), m_fTime );
	m_pProgessText->AddString( nIndex, strText.c_str(), NS_UITEXTCOLOR::WHITE );

	//if ( m_fTime <= 0.0f )
	//{
	//	if ( m_pGaeaClient->GetCharacter()->IsDie() )
	//	{
	//		// 부활하기
	//		m_pGaeaClient->GetCharacter()->ReqReBirth();

	//		// 열려진 창들 닫기
	//		m_pInterface->CloseAllWindow();				
	//	}
	//}
}

void ProgessTimer::SetTime( UINT iTime )
{
	if ( iTime > 0 )
	{
		m_fTime = static_cast<float>( iTime );
	}

	m_fMaxTime = m_fTime;

	m_pGauge->ResetControl( GetGlobalPos(), "PROGESS_TIMER_GAUGE" );
}

void ProgessTimer::SetProgessTimer( UINT iTime,float fPosX, float fPosY, const char* szText, int nTextIndex )
{
	SetTime( iTime );
	m_strText = std::string( szText );
	m_nTextIndex = nTextIndex;
}

void MyProgessTimer::CreateUIWindowAndRegisterOwnership()
{
	ProgessTimer::Create( PROGESS_TIMER, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	ProgessTimer::CreateSubControl();
	ProgessTimer::SetAlignFlag( UI_FLAG_CENTER_Y | UI_FLAG_CENTER_X );		
	ProgessTimer::m_pInterface->UiRegisterControl( this, true );
	ProgessTimer::m_pInterface->UiShowGroupFocus( GetWndID() );
}

void MyProgessTimer::SetProgessTimer( UINT iTime,float fPosX, float fPosY, const char* szText, int nTextIndex )
{
	ProgessTimer::SetProgessTimer( iTime, fPosX, fPosY, szText, nTextIndex );
}
