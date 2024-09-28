#include "StdAfx.h"

#include "CDMSafeTimeDisplay.h"
#include "../../../EngineLib/GUInterface/BasicProgressBar.h"
#include "../../InnerInterface.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CCDMSafeTimeDisplay::CCDMSafeTimeDisplay(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
{
}

CCDMSafeTimeDisplay::~CCDMSafeTimeDisplay ()
{
}

void CCDMSafeTimeDisplay::CreateSubControl ()
{
	CUIControl* pImage = new CUIControl(m_pEngineDevice);
	pImage->CreateSub ( this, "CDM_SAFETIME_IMAGE" );	
	pImage->SetUseRender ( TRUE );
	RegisterControl ( pImage );

	CUIControl* pProgressBack = new CUIControl(m_pEngineDevice);
	pProgressBack->CreateSub ( this, "CDM_SAFETIME_BACK" );
	RegisterControl ( pProgressBack );

	CBasicProgressBar* pProgressBar = new CBasicProgressBar(m_pEngineDevice);
	pProgressBar->CreateSub ( this, "CDM_SAFETIME_PROGRESS" );
	pProgressBar->CreateOverImage ( "CDM_SAFETIME_PROGRESS_OVERIMAGE" );
	pProgressBar->SetType ( CBasicProgressBar::VERTICAL );
	RegisterControl ( pProgressBar );
	m_pProgressBar = pProgressBar;
}

void CCDMSafeTimeDisplay::SetVisibleSingle ( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle( bVisible );
    bool bVisibleTemp = (0!=bVisible);
	m_pInterface->CalcSkillDisplayPos( bVisibleTemp );
}

HRESULT CCDMSafeTimeDisplay::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = CUIGroup::RestoreDeviceObjects( pd3dDevice );
	if ( IsVisible() ) m_pInterface->CalcSkillDisplayPos( true );
	
	return hr;
}

void CCDMSafeTimeDisplay::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return ;

	GLCharacter* const pCharacter = m_pGaeaClient->GetCharacter();
	if ( !pCharacter ) return;

	m_fLEFT_TIME = pCharacter->m_fCDMSafeTime;

//	CDebugSet::ToView( 1, 5, "CDMSafeTime %2.2f", pCharacter->m_fCDMSafeTime );

	if ( m_fLEFT_TIME < 0.0f ) 
	{
		m_pInterface->VisibleCDMSafeTime( false, 0.0f );
	}

	if ( m_pProgressBar )
	{
		const float fPercent = m_fLEFT_TIME/m_fLIFE_TIME;
		m_pProgressBar->SetPercent ( fPercent );
	}

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( CHECK_MOUSE_IN ( GetMessageEx () ) )
	{
		m_pInterface->SHOW_COMMON_LINEINFO ( ID2GAMEWORD("CDM_SAFE_TIME"), NS_UITEXTCOLOR::GREENYELLOW );
	}
}