#include "StdAfx.h"
#include "UIEditButton.h"
#include "../../EngineLib/GUInterface/BasicLineBox.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CUIEditButton::CUIEditButton(EngineDeviceMan* pEngineDevice, CInterfaceAdmin* pInterfaceAdmin)
    : CUIEditGroup(pEngineDevice, pInterfaceAdmin)
    , m_pMouseInFlip(NULL)
    , m_pMouseClickFlip(NULL)
{
}

CUIEditButton::~CUIEditButton ()
{
}

void CUIEditButton::CreateWindowControl (const TCHAR* szControl)
{
	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub(this,_T("BASIC_EDIT_CONTROL"));
	RegisterControl( pControl );
	m_pMouseInFlip = pControl;
	m_pMouseInFlip->SetVisibleSingle(FALSE);

	pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub(this,_T("BASIC_EDIT_CONTROL"));
	RegisterControl( pControl );
	m_pMouseClickFlip = pControl;
	m_pMouseClickFlip->SetVisibleSingle(FALSE);

	CUIEditGroup::CreateWindowControl(szControl);
}

void CUIEditButton::ResizeWindow(const UIRECT& rcNew)
{
	CUIEditGroup::ResizeWindow(rcNew);

	UIRECT rcChange;

	rcChange.sizeX	= rcNew.sizeX;
	rcChange.sizeY	= rcNew.sizeY;
	rcChange.left	= 0;
	rcChange.top	= 0;
	rcChange.right	= rcChange.left + rcChange.sizeX;
	rcChange.bottom	= rcChange.top + rcChange.sizeY;

	if(m_pMouseInFlip)
	{
		m_pMouseInFlip->SetLocalPos(rcChange);
		m_pMouseInFlip->ResizeWindow(rcChange);
	}

	if(m_pMouseClickFlip)
	{
		m_pMouseClickFlip->SetLocalPos(rcChange);
		m_pMouseClickFlip->ResizeWindow(rcChange);
	}

}

void CUIEditButton::CheckMousestate( )
{
	CUIEditGroup::CheckMousestate();

	const DWORD dwMsg = GetMessageEx();

	if ( CHECK_MOUSE_IN ( dwMsg ) )		//	컨트롤 내부에 마우스가 위치하고,
	{
		if(m_pMouseInFlip) m_pMouseInFlip->SetVisibleSingle(TRUE);

		if ( CHECK_LB_DOWN_LIKE( dwMsg ) )	//	좌클릭했던것을 놓으면
		{
			if(m_pMouseClickFlip) m_pMouseClickFlip->SetVisibleSingle(TRUE);
		}
		else
		{
			if(m_pMouseClickFlip) m_pMouseClickFlip->SetVisibleSingle(FALSE);
		}
	}
	else
	{
		if(m_pMouseInFlip) m_pMouseInFlip->SetVisibleSingle(FALSE);
	}


}
