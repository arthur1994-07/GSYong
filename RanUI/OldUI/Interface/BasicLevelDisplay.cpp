#include "StdAfx.h"
#include "BasicLevelDisplay.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/DxTools/d3dfont.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CBasicLevelDisplay::CBasicLevelDisplay(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice)
	: CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pLevelText(NULL)
	, m_wLevelBACK(0)
{
}

CBasicLevelDisplay::~CBasicLevelDisplay ()
{
	m_wLevelBACK = 0;
}

void CBasicLevelDisplay::CreateSubControl ()
{	
	CD3DFontPar* pFont8 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 8, D3DFONT_SHADOW | D3DFONT_ASCII );

	CBasicTextBox* pLevelText = new CBasicTextBox(m_pEngineDevice);
	pLevelText->CreateSub ( this, "BASIC_LEVEL_DISPLAY_TEXT" );
    pLevelText->SetFont ( pFont8 );
	pLevelText->SetTextAlign ( TEXT_ALIGN_CENTER_X );
	RegisterControl ( pLevelText );
	m_pLevelText = pLevelText;	
}

void CBasicLevelDisplay::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	const WORD& wLevel = m_pGaeaClient->GetCharacter()->m_wLevel;

	if( m_wLevelBACK < wLevel )
	{
		m_wLevelBACK = wLevel;

		CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

		CString strLevel;
		strLevel.Format( "%d", m_wLevelBACK );	
		m_pLevelText->SetOneLineText( strLevel, NS_UITEXTCOLOR::GOLD /*PLUSOPTION*/ );
	}
}