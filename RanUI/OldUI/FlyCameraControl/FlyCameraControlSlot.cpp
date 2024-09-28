
#include "StdAfx.h"

#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UIControlMessage.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../InnerInterface.h"

#include "../Util/UIGroupHelper.h"
#include "../Util/RnButton.h"
#include "../Util/RnEditBox.h"

#include "FlyCameraControlUIDefine.h"
#include "FlyCameraControlSlot.h"

/*
GameWord - FLYCAMERACONTROL_SLOT_NAME
GameWord - FLYCAMERACONTROL_BUTTON_TEXT
UIInnerCfg01 - FLYCAMERACONTROL_EDITBOX_BACK_LINE_TEXTUREINFO, FLYCAMERACONTROL_SLOT_BUTTONBVALUEUP, FLYCAMERACONTROL_SLOT_BUTTONBVALUEDOWN
UiXmlList - Path
*/

FlyCameraControlSlot::FlyCameraControlSlot( EngineDeviceMan* pEngineDevice, const DWORD nSlotIndex)
: CUIGroupHelper(pEngineDevice)
, m_dwSlotIndex(nSlotIndex)
, m_fValue(0)
, m_pTxtName(NULL)
, m_pEdtValue(NULL)
, m_pBtnValueUp(NULL)
, m_pBtnValueDown(NULL)
{
}

void FlyCameraControlSlot::CreateSubControl(const std::string& strText)
{
	CD3DFontPar* pFont10 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 10, _DEFAULT_FONT_FLAG );

	CreateLineBox("FLYCAMERACONTROL_SLOT_BOX", "PARTY_INFO_LINEBOX_TEXTUREINFO");
	m_pTxtName = CreateText(
		 strText.c_str()
		, "FLYCAMERACONTROL_SLOT_NAME_SIZE"
		, TEXT_ALIGN_CENTER_X|TEXT_ALIGN_CENTER_Y);

	// Edit Box
	CreateLineBox("FLYCAMERACONTROL_SLOT_EDITLINEBOX_MAN", "FLYCAMERACONTROL_EDITBOX_BACK_LINE_TEXTUREINFO");

	m_pEdtValue = new RnEditBox(m_pEngineDevice);
	m_pEdtValue->Create( this
		, "FLYCAMERACONTROL_SLOT_EDITBOX_MAN"
		, "FLYCAMERACONTROL_SLOT_EDITBOX"
		, "FLYCAMERACONTROL_SLOT_EDITCARRAT"
		, IDD_FCC_EDITBOX_VALUE, true
		, D3DCOLOR_XRGB(255, 255, 255), pFont10, /*CHR_ID_LENGTH*/7 );
	
	// Button Box
	m_pBtnValueUp = new RnButton(m_pEngineDevice);
	m_pBtnValueUp->CreateSub( this
		, "FLYCAMERACONTROL_SLOT_BUTTONBVALUEUP"
		, UI_FLAG_DEFAULT
		, IDD_FCC_BUTTON_VALUEUP);

	m_pBtnValueDown = new RnButton(m_pEngineDevice);
	m_pBtnValueDown->CreateSub( this
		, "FLYCAMERACONTROL_SLOT_BUTTONBVALUEDOWN"
		, UI_FLAG_DEFAULT
		, IDD_FCC_BUTTON_VALUEDOWN/**/);

	RnButton::CreateArg Arg;
	Arg.defaultTextureKey = "PARTY_INFO_BUTTON_NORMAL";
	Arg.mouseOverTextureKey = "PARTY_INFO_BUTTON_OVER";
	Arg.mouseClickTextureKey = "PARTY_INFO_BUTTON_CLICK";
	Arg.text = ID2GAMEWORD("FLYCAMERACONTROL_BUTTON_TEXT", 0);
	m_pBtnValueUp->CreateSubControl(Arg);
	Arg.text = ID2GAMEWORD("FLYCAMERACONTROL_BUTTON_TEXT", 1);
	m_pBtnValueDown->CreateSubControl(Arg);

	RegisterControl(m_pEdtValue);
	RegisterControl(m_pBtnValueUp);
	RegisterControl(m_pBtnValueDown);
}

void FlyCameraControlSlot::TranslateUIMessage(UIGUID ControlID, DWORD dwMsg)
{
	CUIGroupHelper::TranslateUIMessage(ControlID, dwMsg);

	switch (ControlID)
	{
	case IDD_FCC_EDITBOX_VALUE:
		{
			if(dwMsg & UIMSG_RNEDITBOX_RETURN)	// Edit Box ¼öÁ¤½Ã;
			{
				AddMessageEx(FLYCAMERA_UI_MSG_DEFINE::dwFLYCAMERA_UIMSG_VALUE_UPDATE | UIMSG_TOTOPPARENT);
				const float fValue = static_cast<float>( atof(m_pEdtValue->GetEditString().c_str()) );
				SetValue(fValue);
			}
		}
		break;
	case IDD_FCC_BUTTON_VALUEUP:
	case IDD_FCC_BUTTON_VALUEDOWN:
		{
			if ( CHECK_MOUSE_IN_LBDOWNLIKE(dwMsg) )
			{
				AddMessageEx(FLYCAMERA_UI_MSG_DEFINE::dwFLYCAMERA_UIMSG_VALUE_UPDATE | UIMSG_TOTOPPARENT);
				const float fValue = ControlID == IDD_FCC_BUTTON_VALUEUP? m_fValue+1:m_fValue-1;
				SetValue(fValue);
			}
		}
		break;
	}
}

void FlyCameraControlSlot::Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl)
{
	CUIGroupHelper::Update(x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl);
}

void FlyCameraControlSlot::SetValue( const float fValue )
{
	m_fValue = fValue < 0 ? 0 : fValue;

	CString strValue;
	strValue.Format("%.1f", m_fValue);
	m_pEdtValue->SetEditString(strValue.GetString());
// 	m_strValue = fcvt(m_fValue, 1, &nDec, &nSign);
//	sprintf(m_strValue.c_str(), "%.1f", fValue);
}