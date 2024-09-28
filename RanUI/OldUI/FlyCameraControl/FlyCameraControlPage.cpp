
#include "StdAfx.h"

#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicVarTextBox.h"
#include "../../../EngineLib/GUInterface/UIEditBox.h"
#include "../../../EngineLib/GUInterface/UIEditBoxMan.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Input/GLInputManager.h"
#include "../../../EngineLib/DxTools/DxViewPort.h"
#include "../../InnerInterface.h"

#include "../Util/UIGroupHelper.h"

#include "FlyCameraControlUIDefine.h"
#include "FlyCameraControlSlot.h"
#include "FlyCameraControlPage.h"

FlyCameraControlPage::FlyCameraControlPage(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice)
: CUIGroupHelper(pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_pEditMan(NULL)
, m_vCamPos(0.f, 0.f, 0.f)
{
	ZeroMemory(m_pFlyCameraControlSlot, sizeof(m_pFlyCameraControlSlot) );
}

void FlyCameraControlPage::CreateSubControl(void)
{
	CD3DFontPar* pFont10 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 10, _DEFAULT_FONT_FLAG );

	// Slot
	int i;
	for ( i = 0; i < FLYCAMERACONTROLSLOT_SIZE; ++i )
	{
		m_pFlyCameraControlSlot[i] = new FlyCameraControlSlot(m_pEngineDevice, i);
		CString strFCCSlot;
		strFCCSlot.Format("FLYCAMERACONTROL_SLOT_%d", i);
		m_pFlyCameraControlSlot[i]->CreateSub(this
			, strFCCSlot.GetString()
			, UI_FLAG_DEFAULT
			, IDD_FLYCAMERACONTROLSLOT_MOVE_SLOT + i);
		m_pFlyCameraControlSlot[i]->CreateSubControl(ID2GAMEWORD("FLYCAMERACONTROL_SLOT_NAME", i));
		RegisterControl(m_pFlyCameraControlSlot[i]);
	}

	// page ctrl
	{
		// CreateText( "Camera Pos:", "FLYCAMERACONTROL_PAGE_CAMERAPOS_TEXT", TEXT_ALIGN_CENTER_Y);
		m_pEditMan = new CUIEditBoxMan(m_pEngineDevice);
		m_pEditMan->CreateSub(this, "FLYCAMERACONTROL_PAGE_EDITMAN");

		const std::string strEditSize[FLYCAMERACONTROLPAGE_EDITSIZE] = {
			"FLYCAMERACONTROL_PAGE_CAMERAPOS_EDIT",
		};

		for ( int i = 0; i < FLYCAMERACONTROLPAGE_EDITSIZE; ++i )	
		{
			CUIEditBox* pEditBox = new CUIEditBox( m_pEngineDevice );
			pEditBox->CreateSub( this
				, strEditSize[i].c_str(), UI_FLAG_DEFAULT
				, IDD_FLYCAMERACONTROLPAGE_CAMERAPOS_INFO_EDIT+i);
			pEditBox->CreateCarrat("FLYCAMERACONTROL_SLOT_EDITCARRAT", TRUE, UINT_MAX);
			pEditBox->SetFont(pFont10);
			// 최대 표현 숫자 0000.0, 0000.0, 0000.0;
			pEditBox->SetLimitInput(21);
			pEditBox->SetAlign( EDIT_CENTER_XY_ALIGN );
			pEditBox->SetTextColor( NS_UITEXTCOLOR::WHITE );

			m_pEditMan->AddEditBox( pEditBox );
		}
		RegisterControl(m_pEditMan);
	}

	// 도움말;
	CreateFlipButton( "FLYCAMERACONTROL_PAGE_HELP_BUTTON", "INVENTORY_WINDOW_HELP_BUTTON_OVER", IDD_FLYCAMERACONTROLPAGE_HELP_BUTTON, CBasicButton::MOUSEIN_FLIP);
	CreateInfoDisplayBox();
}

void FlyCameraControlPage::TranslateUIMessage(UIGUID ControlID, DWORD dwMsg)
{
	CUIGroupHelper::TranslateUIMessage(ControlID, dwMsg);
	switch ( ControlID )
	{
	case IDD_FLYCAMERACONTROLSLOT_MOVE_SLOT:
		{
			if ( dwMsg & FLYCAMERA_UI_MSG_DEFINE::dwFLYCAMERA_UIMSG_VALUE_UPDATE )
				m_pGaeaClient->SetFlyCameraControl_MoveVelo(m_pFlyCameraControlSlot[FLYCAMERACONTROLSLOT_MOVE_SLOT_INDEX]->GetValue());
		}
		break;
	case IDD_FLYCAMERACONTROLSLOT_ROTATE_SLOT:
		{
			if ( dwMsg & FLYCAMERA_UI_MSG_DEFINE::dwFLYCAMERA_UIMSG_VALUE_UPDATE )
				m_pGaeaClient->SetFlyCameraControl_RotateVelo(m_pFlyCameraControlSlot[FLYCAMERACONTROLSLOT_ROTATE_SLOT_INDEX]->GetValue());
		}
		break;
	case IDD_FLYCAMERACONTROLPAGE_HELP_BUTTON:
		if( CHECK_MOUSE_IN( dwMsg ) )
		{
			m_pInfoDisplay->SetVisibleSingle(TRUE);
		}	
		else
		{
			m_pInfoDisplay->SetVisibleSingle(FALSE);
		}
		break;
	}
}

void FlyCameraControlPage::Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl)
{
	CUIGroupHelper::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
	
	// ..;
	if ( m_pFlyCameraControlSlot[FLYCAMERACONTROLSLOT_MOVE_SLOT_INDEX]->GetValue() != m_pGaeaClient->GetFlyCameraControl_MoveVelo() )
	{
		m_pFlyCameraControlSlot[FLYCAMERACONTROLSLOT_MOVE_SLOT_INDEX]->SetValue(m_pGaeaClient->GetFlyCameraControl_MoveVelo());
	}

	if ( m_pFlyCameraControlSlot[FLYCAMERACONTROLSLOT_ROTATE_SLOT_INDEX]->GetValue() != m_pGaeaClient->GetFlyCameraControl_RotateVelo() )
	{
		m_pFlyCameraControlSlot[FLYCAMERACONTROLSLOT_ROTATE_SLOT_INDEX]->SetValue(m_pGaeaClient->GetFlyCameraControl_RotateVelo());
	}

	CUIEditBox* pCamPosEdit = m_pEditMan->GetEditBox(IDD_FLYCAMERACONTROLPAGE_CAMERAPOS_INFO_EDIT);
	if ( pCamPosEdit )
	{
		const D3DXVECTOR3 vCamFrom = DxViewPort::GetInstance().GetFromPt();
		if ( vCamFrom != m_vCamPos )
		{
			CString strPosText;
			strPosText.Format("%0.1f, %0.1f, %0.1f", vCamFrom.x, vCamFrom.y, vCamFrom.z);

			pCamPosEdit->SetEditString(strPosText);
			m_vCamPos = vCamFrom;
		}
	}
}

void FlyCameraControlPage::CreateInfoDisplayBox()
{
	const std::string strMoveFront = GLInputManager::GetInstance().GetShortcutsToString(EMSHORTCUTS_CAMERA_MOVE_FRONT)
		, strMoveLeft = GLInputManager::GetInstance().GetShortcutsToString(EMSHORTCUTS_CAMERA_MOVE_LEFT)
		, strMoveBack = GLInputManager::GetInstance().GetShortcutsToString(EMSHORTCUTS_CAMERA_MOVE_BACK)
		, strMoveRight= GLInputManager::GetInstance().GetShortcutsToString(EMSHORTCUTS_CAMERA_MOVE_RIGHT);
	const std::string strMoveText = sc::string::format(ID2GAMEWORD ("FLYCAMERAWINDOW_HELPTEXT", 0), strMoveFront, strMoveLeft, strMoveBack, strMoveRight);

	const std::string strRotateUp = GLInputManager::GetInstance().GetShortcutsToString(EMSHORTCUTS_CAMERA_UP)
		, strRotateDown = GLInputManager::GetInstance().GetShortcutsToString(EMSHORTCUTS_CAMERA_DOWN)
		, strRotateLeft = GLInputManager::GetInstance().GetShortcutsToString(EMSHORTCUTS_CAMERA_LEFT)
		, strRotateRight= GLInputManager::GetInstance().GetShortcutsToString(EMSHORTCUTS_CAMERA_RIGHT);
	const std::string strRotateText = sc::string::format(ID2GAMEWORD ("FLYCAMERAWINDOW_HELPTEXT", 1), strRotateUp, strRotateDown, strRotateLeft, strRotateRight);

	const std::string strMoveInterInc = GLInputManager::GetInstance().GetShortcutsToString(EMSHORTCUTS_CAMERA_INCREASE_INTERPOLATION_MOVETIME),
		strMoveInterDec = GLInputManager::GetInstance().GetShortcutsToString(EMSHORTCUTS_CAMERA_DECREASE_INTERPOLATION_MOVETIME);
	const std::string strMoveInterText = sc::string::format(ID2GAMEWORD ("FLYCAMERAWINDOW_HELPTEXT", 2), strMoveInterInc, strMoveInterDec);

	const std::string strRotateInterInc = GLInputManager::GetInstance().GetShortcutsToString(EMSHORTCUTS_CAMERA_INCREASE_INTERPOLATION_ROTATETIME),
		strRotateInterDec = GLInputManager::GetInstance().GetShortcutsToString(EMSHORTCUTS_CAMERA_DECREASE_INTERPOLATION_ROTATETIME);
	const std::string strRotateInterText = sc::string::format(ID2GAMEWORD ("FLYCAMERAWINDOW_HELPTEXT", 3), strRotateInterInc, strRotateInterDec);

	const std::string strClearKeyText = GLInputManager::GetInstance().GetShortcutsToString(EMSHORTCUTS_CAMERA_CLEAR_INTERPOLATION_TIME);
	const std::string strInterTClear = sc::string::format(ID2GAMEWORD ("FLYCAMERAWINDOW_HELPTEXT", 4), strClearKeyText);

	const std::string strMoveInc = GLInputManager::GetInstance().GetShortcutsToString(EMSHORTCUTS_CAMERA_INCREASE_MOVESPEED),
		strMoveDec = GLInputManager::GetInstance().GetShortcutsToString(EMSHORTCUTS_CAMERA_DECREASE_MOVESPEED);
	const std::string strMoveSpeedText = sc::string::format(ID2GAMEWORD ("FLYCAMERAWINDOW_HELPTEXT", 5), strMoveInc, strMoveDec);

	const std::string strRotateInc = GLInputManager::GetInstance().GetShortcutsToString(EMSHORTCUTS_CAMERA_INCREASE_ROTATESPEED),
		strRotateDec = GLInputManager::GetInstance().GetShortcutsToString(EMSHORTCUTS_CAMERA_DECREASE_ROTATESPEED);
	const std::string strRotateSpeedText = sc::string::format(ID2GAMEWORD ("FLYCAMERAWINDOW_HELPTEXT", 6), strRotateInc, strRotateDec);

	const std::string strTargetChangeInc = GLInputManager::GetInstance().GetShortcutsToString(EMSHORTCUTS_CAMERA_INCREASE_INTERPOLATION_TARGETSELECTTIME),
		strTargetChangeDec = GLInputManager::GetInstance().GetShortcutsToString(EMSHORTCUTS_CAMERA_DECREASE_INTERPOLATION_TARGETSELECTTIME);
	const std::string strTargetSelectText = sc::string::format(ID2GAMEWORD ("FLYCAMERAWINDOW_HELPTEXT", 7), strTargetChangeInc, strTargetChangeDec);

	const std::string strPlay = GLInputManager::GetInstance().GetShortcutsToString(EMSHORTCUTS_CAMERA_PLAY);
	const std::string strPlayText = sc::string::format(ID2GAMEWORD ("FLYCAMERAWINDOW_HELPTEXT", 8), strPlay);

	const std::string strStop = GLInputManager::GetInstance().GetShortcutsToString(EMSHORTCUTS_CAMERA_STOP);
	std::string strStopText = sc::string::format(ID2GAMEWORD ("FLYCAMERAWINDOW_HELPTEXT", 9), strStop);

	const std::string strRewind = GLInputManager::GetInstance().GetShortcutsToString(EMSHORTCUTS_CAMERA_REWIND_PLAY);
	const std::string strRewindText = sc::string::format(ID2GAMEWORD ("FLYCAMERAWINDOW_HELPTEXT", 10), strRewind);

	m_pInfoDisplay = new CBasicVarTextBox(m_pEngineDevice);
	m_pInfoDisplay->CreateSub(this, "BASIC_VAR_TEXT", UI_FLAG_XSIZE|UI_FLAG_YSIZE, IDD_FLYCAMERACONTROLPAGE_HELP_TEXTBOX);
	m_pInfoDisplay->CreateSubControl();
	m_pInfoDisplay->SetTextAlign(TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);
	m_pInfoDisplay->SetVisibleSingle( FALSE );
	RegisterControl( m_pInfoDisplay );
	m_pInfoDisplay->AddTextNoSplit(strMoveText.c_str(),			NS_UITEXTCOLOR::LIGHTSKYBLUE);
	m_pInfoDisplay->AddTextNoSplit(strRotateText.c_str(),		NS_UITEXTCOLOR::LIGHTSKYBLUE);
	m_pInfoDisplay->AddTextNoSplit(strMoveInterText.c_str(),	NS_UITEXTCOLOR::LIGHTSKYBLUE);
	m_pInfoDisplay->AddTextNoSplit(strRotateInterText.c_str(),	NS_UITEXTCOLOR::LIGHTSKYBLUE);
	m_pInfoDisplay->AddTextNoSplit(strInterTClear.c_str(),		NS_UITEXTCOLOR::LIGHTSKYBLUE);
	m_pInfoDisplay->AddTextNoSplit(strMoveSpeedText.c_str(),	NS_UITEXTCOLOR::LIGHTSKYBLUE);
	m_pInfoDisplay->AddTextNoSplit(strRotateSpeedText.c_str(),	NS_UITEXTCOLOR::LIGHTSKYBLUE);
	m_pInfoDisplay->AddTextNoSplit(strTargetSelectText.c_str(),	NS_UITEXTCOLOR::LIGHTSKYBLUE);
	m_pInfoDisplay->AddTextNoSplit(strPlayText.c_str(),			NS_UITEXTCOLOR::LIGHTSKYBLUE);
	m_pInfoDisplay->AddTextNoSplit(strStopText.c_str(),			NS_UITEXTCOLOR::LIGHTSKYBLUE);
	m_pInfoDisplay->AddTextNoSplit(strRewindText.c_str(),		NS_UITEXTCOLOR::LIGHTSKYBLUE);
}