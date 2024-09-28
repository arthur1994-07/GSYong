#include "StdAfx.h"

#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UIControlMessage.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../InnerInterface.h"

#include "../Util/UIGroupHelper.h"
#include "../Util/RnEditBox.h"
#include "./CSetitemPreviewlistSlot.h"

CSetitemPreviewlistSlot::CSetitemPreviewlistSlot(EngineDeviceMan* pEngineDevice, const DWORD nSlotIndex)
: CUIGroupHelper(pEngineDevice)
, m_dwSlotIndex(nSlotIndex)
{
}

void CSetitemPreviewlistSlot::CreateSubControl(void)
{
	CBasicTextBox* pTB = CreateText( "-"
		, "SETITEMPREVIEW_LIST_SLOT_RECTANGLE"
		, TEXT_ALIGN_LEFT|TEXT_ALIGN_CENTER_Y
		, IDD_NAMEBOX);
}

void CSetitemPreviewlistSlot::TranslateUIMessage(UIGUID ControlID, DWORD dwMsg)
{
	CUIGroupHelper::TranslateUIMessage(ControlID, dwMsg);
	switch ( ControlID )
	{
	case IDD_NAMEBOX:
		{
			if( CHECK_MOUSE_IN ( dwMsg ) )
			{
				AddMessageEx(SETITEM_UIMSG_DEFINE::dwSETITEM_UIMSG_CHECK_MOUSE_IN|UIMSG_TOTOPPARENT);
			}
		}
		break;
	}
}

void CSetitemPreviewlistSlot::Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl)
{
	CUIGroupHelper::Update(x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl);
}

void CSetitemPreviewlistSlot::SetKeySlotInfo(const DWORD dwSlot, const std::string strItemName, const BOOL bWoreSetitem)
{
	m_dwSlotIndex = dwSlot;
	CBasicTextBox* pTB = static_cast<CBasicTextBox*>(this->FindControl(IDD_NAMEBOX));
	pTB->SetUseTextColor(0, TRUE);
	pTB->SetTextToLimit(strItemName.c_str(), TRUE, bWoreSetitem == TRUE? ITEM_INFOR_COLOR::dwDefault:ITEM_INFOR_COLOR::dwTrade);
}