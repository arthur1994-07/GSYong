#include "StdAfx.h"
#include "ConftConfirm.h"

#include "../../../EngineLib/GUInterface/BasicProgressBar.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CConftConfirm::CConftConfirm(EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
{
}

CConftConfirm::~CConftConfirm ()
{
}

void	CConftConfirm::SetPercent ( const float fPercent )
{
	m_pProgress->SetPercent ( fPercent );
}

void CConftConfirm::CreateSubControl ()
{    
	CBasicProgressBar* pProgress = new CBasicProgressBar(m_pEngineDevice);
	pProgress->CreateSub ( this, "CONFT_CONFIRM_PROGRESS", UI_FLAG_DEFAULT, CONFT_CONFIRM );
	pProgress->CreateOverImage ( "CONFT_CONFIRM_PROGRESS_OVER" );
	RegisterControl ( pProgress );
	m_pProgress = pProgress;
}