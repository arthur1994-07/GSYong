#include "pch.h"
#include "BasicScrollBarEx.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CBasicScrollBarEx::CBasicScrollBarEx (EngineDeviceMan* pEngineDevice)
    : CBasicScrollBar(pEngineDevice)
{
}

CBasicScrollBarEx::~CBasicScrollBarEx ()
{
}

void CBasicScrollBarEx::CreateBaseScrollBar ( char* szBaseScrollBar, const bool& bVERTICAL )
{
	if ( bVERTICAL )
	{
		CreateThumb ( "BASIC_SCROLLBAR_THUMB_FRAME", "BASIC_SCROLLBAR_THUMB_MAIN", "BASIC_SCROLLBAR_THUMB_BODY", "BASIC_SCROLLBAR_THUMB_UP", "BASIC_SCROLLBAR_THUMB_DN", bVERTICAL );
		CreateButton ( "BASIC_SCROLLBAR_BUTTONUP", "BASIC_SCROLLBAR_BUTTONUP_F", "BASIC_SCROLLBAR_BUTTONDN", "BASIC_SCROLLBAR_BUTTONDN_F", bVERTICAL );
	}
	else
	{
		CreateThumb ( "BASIC_SCROLLBAR_THUMB_FRAME_H", "BASIC_SCROLLBAR_THUMB_MAIN_H", "BASIC_SCROLLBAR_THUMB_BODY_H", "BASIC_SCROLLBAR_THUMB_UP_H", "BASIC_SCROLLBAR_THUMB_DN_H", bVERTICAL );
		CreateButton ( "BASIC_SCROLLBAR_BUTTONUP_H", "BASIC_SCROLLBAR_BUTTONUP_F_H", "BASIC_SCROLLBAR_BUTTONDN_H", "BASIC_SCROLLBAR_BUTTONDN_F_H", bVERTICAL );
	}

    CUIControl TempControl(m_pEngineDevice);
	TempControl.Create ( 1, szBaseScrollBar );
	const UIRECT& rcParentOldPos = GetLocalPos ();
	const UIRECT& rcParentNewPos = TempControl.GetLocalPos ();
	AlignSubControl ( rcParentOldPos, rcParentNewPos );

	SetLocalPos ( D3DXVECTOR2 ( rcParentNewPos.left, rcParentNewPos.top ) );
}