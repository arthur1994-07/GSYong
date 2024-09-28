
#include "StdAfx.h"

#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UIControlMessage.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../InnerInterface.h"

#include "../Util/UIGroupHelper.h"
#include "../Util/RnEditBox.h"

#include "FlyCameraControlUIDefine.h"
#include "FlyCameraControlKeySlot.h"

FlyCameraControlKeySlot::FlyCameraControlKeySlot(EngineDeviceMan* pEngineDevice, const DWORD nSlotIndex)
: CUIGroupHelper(pEngineDevice)
, m_dwSlotIndex(nSlotIndex)
, m_pTxtName(NULL)
, m_pHighlight(NULL)
{

}

void FlyCameraControlKeySlot::CreateSubControl(void)
{
	CD3DFontPar* pFont10 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 10, _DEFAULT_FONT_FLAG );
	CreateLineBox("FLYCAMERACONTROL_LIST_KEYSLOTLINEBOX", "PRODUCT_WINDOW_BASE_SMARTLINEBOX_GRAY");

	m_pHighlight = CreateLineBox("FLYCAMERACONTROL_LIST_KEYSLOTLINEBOX", "LINEBOX_Y");
	m_pHighlight->SetVisibleSingle(FALSE);

	CString strName;
	strName.Format("Keyframe %01d", m_dwSlotIndex);
	
	m_pTxtName = CreateText( strName.GetString()
		, "FLYCAMERACONTROL_LIST_KEYSLOTLINEBOX"
		, TEXT_ALIGN_CENTER_X|TEXT_ALIGN_CENTER_Y
		, IDD_FCCKEY_NAMEBOX);
}

void FlyCameraControlKeySlot::TranslateUIMessage(UIGUID ControlID, DWORD dwMsg)
{
	CUIGroupHelper::TranslateUIMessage(ControlID, dwMsg);
	switch ( ControlID )
	{
	case IDD_FCCKEY_NAMEBOX:
		{
			if( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( CHECK_LB_DOWN_LIKE( dwMsg ) )
				{
					AddMessageEx(FLYCAMERA_UI_MSG_DEFINE::dwFLYCAMERA_UIMSG_KEY_SELECT|UIMSG_TOTOPPARENT);
				}
			}
		}
		break;
	}
}

void FlyCameraControlKeySlot::Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl)
{
	CUIGroupHelper::Update(x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl);
}

void FlyCameraControlKeySlot::SetKeySlotInfo( const DWORD dwSlot, const BOOL bSelect )
{
	m_dwSlotIndex = dwSlot;
	CString strText;
	strText.Format("Keyframe %01d", dwSlot);
	m_pTxtName->SetText(strText.GetString());

	m_pHighlight->SetVisibleSingle(bSelect);
}