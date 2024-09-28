
#include "StdAfx.h"

#include <boost/lexical_cast.hpp>

#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UIEditBoxMan.h"
#include "../../../EngineLib/GUInterface/UIEditBox.h"
#include "../../../EngineLib/GUInterface/UIKeyCheck.h"
#include "../../InnerInterface.h"
#include "../../RanLogicClient/GLGaeaClient.h"
#include "../../RanLogicClient/FlyCameraControl/GLFlyCameraControl.h"
#include "../../RanLogicClient/FlyCameraControl/GLFlyCameraKeyFrameControl.h"
#include "../../RanLogic/FlyCamera/FlyCameraDefine.h"

#include "../Util/UIGroupHelper.h"
#include "../Util/SmartComboBoxEx.h"
#include "./FlyCameraControlUIDefine.h"
#include "./FlyCameraControlDetailKeyControl.h"

FlyCameraControlDetailKeyControl::FlyCameraControlDetailKeyControl(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice)
: CUIGroupHelper(pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_CbxMovetoType(NULL)
, m_pEdtMan(NULL)
, m_pBtnSave(NULL)
, m_vLastInputPos(0.0f, 0.0f, 0.0f)
{
}

void FlyCameraControlDetailKeyControl::CreateSubControl(void)
{
	CD3DFontPar* pFont10 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 10, _DEFAULT_FONT_FLAG );
	CreateLineBox("FLYCAMERACONTROL_KEYCONTROLDETAIL", "PARTY_INFO_LINEBOX_TEXTUREINFO");

	CString strText;
	// Move to
	{
		m_CbxMovetoType = new CSmartComboBoxBasicEx(m_pEngineDevice);
		m_CbxMovetoType->CreateSub(this
			, "BASIC_LINE_BOX_SMART"
			, UI_FLAG_XSIZE | UI_FLAG_YSIZE
			, IDD_FLYCAMERADETAILCTRL_MOVETO_TYPE_COMBO );
		m_CbxMovetoType->CreateSmartComboBoxCustom(
			"FLYCAMERACONTROL_KEYCONTROLDETAIL_MOVETO_COMBOBOX"
			, "RAN_COMBO_BOX_BACKGROUND"
			, "FLYCAMERACONTROL_KEYCONTROLDETAIL_MOVETO_COMBO_BTN"
			, "RAN_COMBO_BOX_BUTTON_DEFAULT_TEXTURE"
			, "RAN_COMBO_BOX_BUTTON_OVER_TEXTURE"
			, "RAN_COMBO_BOX_BUTTON_CLICK_TEXTURE" );

		int i;
		for ( i=0; i < FlyCameraControl::EMFLYCAMERA_MOVETO_SIZE; ++i )
			m_CbxMovetoType->SetRollOverAddText(FlyCameraControl::strMOVETOTYPE[i].c_str(), i);

		strText = "";
			//CGameTextMan::GetInstance().GetGameWord(
			//"FLYCAMERACONTROL_DETAIL_MOVETOEXPLAIN", m_emMovetoType);
		m_pTbxMovetoExplain = CreateText( strText.GetString()
			, "FLYCAMERACONTROL_KEYCONTROLDETAIL_MOVETO_EXPLAIN_TEXTBOX"
			, TEXT_ALIGN_CENTER_X|TEXT_ALIGN_CENTER_Y
			, IDD_FLYCAMERADETAILCTRL_MOVETO_EXPLAIN_TEXT);
	}

	const std::string strEditSize[FLYCAMERADETAILCTRL_EDITSIZE] = {
		"FLYCAMERACONTROL_KEYCONTROLDETAIL_MOVETO_VALUE_EDITBOX",
		"FLYCAMERACONTROL_KEYCONTROLDETAIL_MOVETO_VALUE_EDITBOX",
	};

	m_pEdtMan = new CUIEditBoxMan(m_pEngineDevice);
	m_pEdtMan->CreateSub(this, "FLYCAMERACONTROL_KEYCONTROLDETAIL");
	int i;
	for ( i = 0; i < 1/*FLYCAMERADETAILCTRL_EDITSIZE*/; ++i )
	{
		CreateLineBox(strEditSize[i], "GRAY_BACKGROUND_BLACK_LINE_TEXTURE");

		CUIEditBox* pEditBox = new CUIEditBox( m_pEngineDevice );
		pEditBox->CreateSub( this
			, strEditSize[i].c_str(), UI_FLAG_DEFAULT
			, IDD_FLYCAMERADETAILCTRL_MOVETO_VALUE_EDIT+i);
		pEditBox->CreateCarrat("FLYCAMERACONTROL_SLOT_EDITCARRAT", TRUE, UINT_MAX);
		pEditBox->SetFont(pFont10);
		pEditBox->SetLimitInput(22);
		pEditBox->SetAlign( EDIT_CENTER_XY_ALIGN );
		pEditBox->SetTextColor( NS_UITEXTCOLOR::WHITE );

		m_pEdtMan->AddEditBox(pEditBox);
	}

	RnButton::CreateArg Arg;
	Arg.defaultTextureKey = "PARTY_INFO_BUTTON_NORMAL";
	Arg.mouseOverTextureKey = "PARTY_INFO_BUTTON_OVER";
	Arg.mouseClickTextureKey = "PARTY_INFO_BUTTON_CLICK";
	Arg.text = "Save";
	m_pBtnSave = this->CreateRnButton(Arg
		, "FLYCAMERACONTROL_KEYCONTROLDETAIL_SAVE_BTN"
		, UI_FLAG_DEFAULT
		, IDD_FLYCAMERADETAILCTRL_SAVE_BTN);

	RegisterControl(m_pEdtMan);
	RegisterControl(m_CbxMovetoType);
}

void FlyCameraControlDetailKeyControl::TranslateUIMessage(UIGUID ControlID, DWORD dwMsg)
{
	CUIGroupHelper::TranslateUIMessage(ControlID, dwMsg);

	switch ( ControlID )
	{
	case IDD_FLYCAMERADETAILCTRL_MOVETO_TYPE_COMBO:
		{
			if ( dwMsg & UI_MSG_COMBOBOX_ROLLOVER_CHANGE )
			{
				m_emMovetoType = (FlyCameraControl::EMFLYCAMERA_MOVETO_TYPE)m_CbxMovetoType->GetSelectIndex();
				InitMovetoType();
			}
		}
		break;
	case IDD_FLYCAMERADETAILCTRL_SAVE_BTN:
		{
			if ( CHECK_MOUSE_IN_LBDOWNLIKE(dwMsg) )
			{
				STARGETID sTargetID;
				// sTargetID.vPos = DxViewPort::GetInstance().GetFromPt();
				CUIEditBox* pEditBox = m_pEdtMan->GetEditBox(IDD_FLYCAMERADETAILCTRL_MOVETO_VALUE_EDIT);
				std::string strEditString = pEditBox->GetEditString().GetString();

				std::vector<std::string> Tokens;
				boost::split(Tokens, strEditString, boost::is_any_of(","));
				if ( Tokens.size() != 3 )
				{
					std::string strErrMsg("위치 정보를 제대로 입력해주세요");
					//m_pGaeaClient->GetInterface()->DoModal(strErrMsg.c_str());
					return ;
				}
				/*
				sTargetID.vPos = D3DXVECTOR3( static_cast<float>( atof(Tokens[0].c_str()) )
											, static_cast<float>( atof(Tokens[1].c_str()) )
											, static_cast<float>( atof(Tokens[2].c_str()) ) );
				GLFlyCameraControl::GetInstance().SetKeyFramePosInfo(sTargetID);
				*/

				m_vLastInputPos = D3DXVECTOR3( static_cast<float>( atof(Tokens[0].c_str()) )
					, static_cast<float>( atof(Tokens[1].c_str()) )
					, static_cast<float>( atof(Tokens[2].c_str()) ) );
				m_pGaeaClient->FlyCamera_AddMsg(FlyCameraControl::EMFLYCAMERA_SET_KEYPOSITION, 0);
			}
		}
		break;
	}
}

void FlyCameraControlDetailKeyControl::Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl)
{
	CUIGroupHelper::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	// ComboBox update;
	UpdateCombo();
}

void FlyCameraControlDetailKeyControl::SetDetailSlot( const int nSetIndex )
{
	CString strPos;
	// FlyCameraControl::FlyCameraKey sKey = GLFlyCameraControl::GetInstance().GetKey(nSetIndex);
	
	GLFlyCameraKeyFrameControl::KEYFRAME_VEC_VALUE_T sKey = GLFlyCameraControl::GetInstance().GetKeyData(nSetIndex);

	// MOVE
	{
		const D3DXVECTOR3 vPos = sKey.sMoveToTargetID.vPos;
		strPos.Format("%0.1f, %0.1f, %0.1f", vPos.x, vPos.y, vPos.z);
		CUIEditBox* pEditBox = m_pEdtMan->GetEditBox(IDD_FLYCAMERADETAILCTRL_MOVETO_VALUE_EDIT);
		if ( pEditBox )
		{
			pEditBox->ClearEdit();
			pEditBox->SetEditString(strPos);
		}
	}
}

void FlyCameraControlDetailKeyControl::InitMovetoType(void)
{
	CUIEditBox* pEditBox = m_pEdtMan->GetEditBox(IDD_FLYCAMERADETAILCTRL_MOVETO_VALUE_EDIT);
	pEditBox->ClearEdit();

	const CString strExplainText = "";
	m_pTbxMovetoExplain->SetText(strExplainText.GetString());
}

void FlyCameraControlDetailKeyControl::InitLookatType(void)
{

}

void FlyCameraControlDetailKeyControl::UpdateCombo(void)
{
	// MoveTo
	// LookAt
}