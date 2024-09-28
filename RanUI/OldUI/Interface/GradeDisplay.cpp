#include "StdAfx.h"
#include "GradeDisplay.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxEx.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CGradeDisplay::CGradeDisplay(EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
{
}

CGradeDisplay::~CGradeDisplay ()
{
}

void CGradeDisplay::CreateSubControl ()
{
	CBasicLineBoxEx* pBasicLineBox = new CBasicLineBoxEx(m_pEngineDevice);
	pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_EX_ESC", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pBasicLineBox->CreateBaseBoxESCMenu ( "GRADE_DISPLAY_BACK" );
	RegisterControl ( pBasicLineBox );

	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub ( this, "GRADE_DISPLAY_IMAGE1" );
	RegisterControl ( pControl );

	pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub ( this, "GRADE_DISPLAY_IMAGE2" );
	RegisterControl ( pControl );
}

