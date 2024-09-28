#include "pch.h"
#include "BasicScrollBarNoTHUMB.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CBasicScrollBarNoTHUMB::CBasicScrollBarNoTHUMB (EngineDeviceMan* pEngineDevice)
: CBasicScrollBar(pEngineDevice)
{
}

CBasicScrollBarNoTHUMB::~CBasicScrollBarNoTHUMB ()
{
}

void CBasicScrollBarNoTHUMB::CreateBaseScrollBar ( char* szBaseScrollBar, const bool& bVERTICAL )
{
	if ( bVERTICAL )
	{
	//	CreateThumb ( "BASIC_SCROLLBAR_THUMB_FRAME", "BASIC_SCROLLBAR_THUMB_MAIN", "BASIC_SCROLLBAR_THUMB_BODY", "BASIC_SCROLLBAR_THUMB_UP", "BASIC_SCROLLBAR_THUMB_DN", bVERTICAL );
		CreateThumb ( "", "", "", "", "", bVERTICAL );
		CreateButtonEX ( "BASIC_SCROLLBAR_NOTHUMB_UP",
						"BASIC_SCROLLBAR_NOTHUMB_UP_OVER", 
						"BASIC_SCROLLBAR_NOTHUMB_UPF", 
						"BASIC_SCROLLBAR_NOTHUMB_DN",
						"BASIC_SCROLLBAR_NOTHUMB_DN_OVER",
						"BASIC_SCROLLBAR_NOTHUMB_DNF",
						bVERTICAL );
	}
	else
	{
		//차후 구연
		//CreateThumb ( "BASIC_SCROLLBAR_THUMB_FRAME_H", "BASIC_SCROLLBAR_THUMB_MAIN_H", "BASIC_SCROLLBAR_THUMB_BODY_H", "BASIC_SCROLLBAR_THUMB_UP_H", "BASIC_SCROLLBAR_THUMB_DN_H", bVERTICAL );
		//CreateButton ( "BASIC_SCROLLBAR_BUTTONUP_H", "BASIC_SCROLLBAR_BUTTONUP_F_H", "BASIC_SCROLLBAR_BUTTONDN_H", "BASIC_SCROLLBAR_BUTTONDN_F_H", bVERTICAL );
	}

	CUIControl TempControl(m_pEngineDevice);
	TempControl.Create ( 1, szBaseScrollBar );
	const UIRECT& rcParentOldPos = GetLocalPos ();
	const UIRECT& rcParentNewPos = TempControl.GetLocalPos ();
	AlignSubControl ( rcParentOldPos, rcParentNewPos );

	SetLocalPos ( D3DXVECTOR2 ( rcParentNewPos.left, rcParentNewPos.top ) );
}

void CBasicScrollBarNoTHUMB::SetVisibleSingleUp(bool bVisible)
{
	CUIControl*	pCuiConrrol = FindControl( ET_CONTROL_UP );
	pCuiConrrol->SetVisibleSingle( bVisible );
}

void CBasicScrollBarNoTHUMB::SetVisibleSingleDowon(bool bVisible)
{
	CUIControl*	pCuiConrrol = FindControl( ET_CONTROL_DN );
	pCuiConrrol->SetVisibleSingle( bVisible );
}
