#include "pch.h"
#include "BasicTextButtonImage.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CBasicTextButtonImage::CBasicTextButtonImage (EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
{
}

CBasicTextButtonImage::~CBasicTextButtonImage ()
{
}

void CBasicTextButtonImage::CreateButtonImage ( const char* szLeft, const char* szCenter, const char* szRight )
{
	CUIControl* pLeft = new CUIControl(m_pEngineDevice);
	pLeft->CreateSub ( this, szLeft );
	RegisterControl ( pLeft );

	CUIControl* pCenter = new CUIControl(m_pEngineDevice);
	pCenter->CreateSub ( this, szCenter, UI_FLAG_XSIZE );
	RegisterControl ( pCenter );

	CUIControl* pRight = new CUIControl(m_pEngineDevice);
	pRight->CreateSub ( this, szRight, UI_FLAG_RIGHT );
	RegisterControl ( pRight );
}