#include "StdAfx.h"
#include "MinimapTarget.h"
#include "../../../RanLogicClient/Tutorial/GLBaseTutorial.h"
#include "../../../RanLogicClient/GLGaeaClient.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CMinimapTarget::CMinimapTarget(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pMiniMapTargetMain(NULL)
    , m_pMiniMapTargetBody(NULL)
    , m_bEnable(false)
{
}

CMinimapTarget::~CMinimapTarget(void)
{
}

void CMinimapTarget::CreateSubControl ()
{
	{
		CUIControl* pMnimapMain = new CUIControl(m_pEngineDevice);
		pMnimapMain->CreateSub ( this, "TUTORIAL_INTERFACE_MINIMAP_TARGET_MAIN" );
		RegisterControl ( pMnimapMain );
		m_pMiniMapTargetMain = pMnimapMain;
		m_pMiniMapTargetMain->SetVisibleSingle( FALSE );
	}

	{
		CUIControl* pMinimapBody = new CUIControl(m_pEngineDevice);
		pMinimapBody->CreateSub ( this, "TUTORIAL_INTERFACE_MINIMAP_TARGET_BODY" );
		RegisterControl ( pMinimapBody );
		m_pMiniMapTargetBody = pMinimapBody;
		m_pMiniMapTargetBody->SetVisibleSingle( FALSE );
	}
}

void CMinimapTarget::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible() )	return;
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	m_fFlowTime += fElapsedTime;
	
	if ( ( m_pGaeaClient->GetTutorial()->IsMouseContNMoveStep()
		&& m_pGaeaClient->GetTutorial()->IsSubFifteenStep() )
		|| ( m_pGaeaClient->GetTutorial()->IsQuestNHuntStep()
		&& m_pGaeaClient->GetTutorial()->IsSubThreeStep() ) )
	{
		m_pMiniMapTargetMain->SetVisibleSingle( TRUE );
		//m_pMiniMapTargetBody->SetVisibleSingle( TRUE );

		if ( m_fFlowTime >= 0.8f )
		{
			m_fFlowTime = 0.0f;

			if ( m_pMiniMapTargetBody->IsVisible() )
			{
				m_pMiniMapTargetBody->SetVisibleSingle( FALSE );
			}
			else
			{
				m_pMiniMapTargetBody->SetVisibleSingle( TRUE );
			}
		}
	}
}

HRESULT CMinimapTarget::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	hr = CUIGroup::RestoreDeviceObjects ( pd3dDevice );
	if ( FAILED ( hr ) ) return hr;

	return S_OK;
}

void CMinimapTarget::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );
}

void CMinimapTarget::SetVisibleSingle( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle( bVisible );
}

