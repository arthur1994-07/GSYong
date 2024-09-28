
#include "StdAfx.h"

#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"
#include "../../../EngineLib/GUInterface/UIEditBoxMan.h"
#include "../../../EngineLib/GUInterface/UIEditBox.h"
#include "../../../EngineLib/GUInterface/UIKeyCheck.h"
#include "../../InnerInterface.h"
#include "../../RanLogicClient/GLGaeaClient.h"
#include "../../RanLogicClient/FlyCameraControl/GLFlyCameraControl.h"

#include "../Util/UIGroupHelper.h"
#include "../Util/RnButton.h"
#include "../Util/RnProgressBar.h"
#include "./FlyCameraControlUIDefine.h"
#include "./FlyCameraControlKeyframeTimeLine.h"

#include <boost/bind.hpp>

FlyCameraControlKeyframeTimeLine::FlyCameraControlKeyframeTimeLine(EngineDeviceMan* pEngineDevice, GLGaeaClient* pGaeaClient)
: CUIGroupHelper(pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_pEdtMan(NULL)
, m_pBtnKeyAdd(NULL)
, m_pBtnKeyDel(NULL)
, m_pBtnPlay(NULL)
, m_pBtnStop(NULL)
, m_pBtnRewind(NULL)
, m_pProgTimeLine(NULL)
, m_nFPS(60)
, m_nCurrentKeyFrame(0)
, m_nTotalKeyFrame(300)
{
}

void FlyCameraControlKeyframeTimeLine::CreateSubControl(void)
{
	CD3DFontPar* pFont10 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 10, _DEFAULT_FONT_FLAG );

	CreateLineBox("FLYCAMERACONTROL_KEYFRAMETIMELINE_LINEBOX", "PARTY_INFO_LINEBOX_TEXTUREINFO");

	const std::string strTextSize[EDITBOX_COUNT] = {
		"FLYCAMERACONTROL_KEYFRAMETIMELINE_FPSTEXT_SIZE",
		"FLYCAMERACONTROL_KEYFRAMETIMELINE_KEYTEXT_SIZE",
		"FLYCAMERACONTROL_KEYFRAMETIMELINE_TOTALTEXT_SIZE"
	};
	
	const std::string strEditSize[EDITBOX_COUNT] = {
		"FLYCAMERACONTROL_KEYFRAMETIMELINE_FPSEDIT",
		"FLYCAMERACONTROL_KEYFRAMETIMELINE_KEYEDIT",
		"FLYCAMERACONTROL_KEYFRAMETIMELINE_TOTALEDIT"
	};
	const int nDefaulatString[EDITBOX_COUNT] = {m_nFPS, m_nCurrentKeyFrame, m_nTotalKeyFrame};

	m_pGaeaClient->FlyCamera_AddMsg(FlyCameraControl::EMFLYCAMERA_SET_TOTALFRAME, m_nTotalKeyFrame);

	m_pEdtMan = new CUIEditBoxMan(m_pEngineDevice);
	m_pEdtMan->CreateSub(this, "FLYCAMERACONTROL_KEYFRAMETIMELINE_UIEDITBOXMAN_MAN");
	m_pEdtMan->AddConfirmFunc(boost::bind(
		boost::mem_fn(&FlyCameraControlKeyframeTimeLine::_ExpansionEditBoxMsgCheck), this, _1));

	for ( int i = 0; i < EDITBOX_COUNT; ++i )
	{
		// 설명 TextBox, Linebox of EditBox;
		CString strText = CGameTextMan::GetInstance().GetGameWord("FLYCAMERACONTROL_TIMELINE_TEXT", i);
		CreateText( strText.GetString()
			, strTextSize[i]
			, TEXT_ALIGN_CENTER_X|TEXT_ALIGN_CENTER_Y );
		CreateLineBox(strEditSize[i], "GRAY_BACKGROUND_BLACK_LINE_TEXTURE");

		// Edit Box
		// default text
		CString strDefaultText;
		strDefaultText.Format("%d", nDefaulatString[i]);

		CUIEditBox* pEditBox = new CUIEditBox( m_pEngineDevice );
		pEditBox->CreateSub( this
			, strEditSize[i].c_str(), UI_FLAG_DEFAULT
			, IDD_FLYCAMERATIMELINE_FPS_EDITBOX+i);
		pEditBox->CreateCarrat("FLYCAMERACONTROL_SLOT_EDITCARRAT", TRUE, UINT_MAX);
		pEditBox->SetFont(pFont10);
		pEditBox->SetLimitInput(7);
		pEditBox->DoUSE_DECIMALNUMBER(true);
		pEditBox->SetAlign( EDIT_CENTER_XY_ALIGN );
		pEditBox->SetTextColor( NS_UITEXTCOLOR::WHITE );		
		pEditBox->SetEditString(strDefaultText);

		m_pEdtMan->AddEditBox( pEditBox );
	}

	// FPS는 수정안되도록한다;
	// 수정되도록한다;
	//m_pEdtMan->GetEditBox(IDD_FLYCAMERATIMELINE_FPS_EDITBOX)->DisableKeyInput();

	// Button
	{
		m_pBtnKeyAdd = new RnButton(m_pEngineDevice);
		m_pBtnKeyAdd->CreateSub( this
			, "FLYCAMERACONTROL_KEYFRAMETIMELINE_BUTTONKEYADD"
			, UI_FLAG_DEFAULT
			, IDD_FLYCAMERATIMELINE_ADD_BUTTON);

		m_pBtnKeyDel = new RnButton(m_pEngineDevice);
		m_pBtnKeyDel->CreateSub( this
			, "FLYCAMERACONTROL_KEYFRAMETIMELINE_BUTTONKEYDEL"
			, UI_FLAG_DEFAULT
			, IDD_FLYCAMERATIMELINE_DEL_BUTTON);

		m_pBtnPlay = new RnButton(m_pEngineDevice);
		m_pBtnPlay->CreateSub( this
			, "FLYCAMERACONTROL_KEYFRAMETIMELINE_BUTTONPLAY"
			, UI_FLAG_DEFAULT
			, IDD_FLYCAMERATIMELINE_PLAY_BUTTON);

		m_pBtnStop = new RnButton(m_pEngineDevice);
		m_pBtnStop->CreateSub( this
			, "FLYCAMERACONTROL_KEYFRAMETIMELINE_BUTTONSTOP"
			, UI_FLAG_DEFAULT
			, IDD_FLYCAMERATIMELINE_STOP_BUTTON);

		m_pBtnRewind = new RnButton(m_pEngineDevice);
		m_pBtnRewind->CreateSub( this
			, "FLYCAMERACONTROL_KEYFRAMETIMELINE_BUTTONREWIND"
			, UI_FLAG_DEFAULT
			, IDD_FLYCAMERATIMELINE_REWIND_BUTTON);

		RnButton::CreateArg Arg;
		Arg.defaultTextureKey = "PARTY_INFO_BUTTON_NORMAL";
		Arg.mouseOverTextureKey = "PARTY_INFO_BUTTON_OVER";
		Arg.mouseClickTextureKey = "PARTY_INFO_BUTTON_CLICK";
		Arg.text = ID2GAMEWORD("FLYCAMERACONTROL_TIMELINE_BUTTON", 0);
		m_pBtnKeyAdd->CreateSubControl(Arg);
		Arg.text = ID2GAMEWORD("FLYCAMERACONTROL_TIMELINE_BUTTON", 1);
		m_pBtnKeyDel->CreateSubControl(Arg);
		Arg.text = ID2GAMEWORD("FLYCAMERACONTROL_TIMELINE_BUTTON", 2);
		m_pBtnPlay->CreateSubControl(Arg);
		Arg.text = ID2GAMEWORD("FLYCAMERACONTROL_TIMELINE_BUTTON", 3);
		m_pBtnStop->CreateSubControl(Arg);
		Arg.text = ID2GAMEWORD("FLYCAMERACONTROL_TIMELINE_BUTTON", 4);
		m_pBtnRewind->CreateSubControl(Arg);
	}
	{		
		// UIInnerCFG01.xml		
		RnProgressBar::CreateArg args;
		args.controlName = "FLYCAMERACONTROL_KEYFRAMETIMELINE_PROGRESSBAR";
		args.backgroundTextureName = "PARTY_TENDER_ITEM_PROGRESS_BG_TEXTURE";
		args.overTextureName = "PARTY_TENDER_ITEM_PROGRESS_OVER_TEXTURE";

		m_pProgTimeLine = new RnProgressBar(m_pEngineDevice);
		m_pProgTimeLine->CreateSub(this
			, "FLYCAMERACONTROL_KEYFRAMETIMELINE_PROGRESSBARMAN"
			, UI_FLAG_DEFAULT
			, IDD_FLYCAMERATIMELINE_TIMELINE_PROGRESSBAR);
		m_pProgTimeLine->CreateSubControl(args);
		m_pProgTimeLine->SetMax(m_nTotalKeyFrame);
	}

	RegisterControl(m_pEdtMan);
	RegisterControl(m_pBtnKeyAdd);
	RegisterControl(m_pBtnKeyDel);
	RegisterControl(m_pBtnPlay);
 	RegisterControl(m_pBtnStop);
	RegisterControl(m_pBtnRewind);
 	RegisterControl(m_pProgTimeLine);
}

void FlyCameraControlKeyframeTimeLine::TranslateUIMessage(UIGUID ControlID, DWORD dwMsg)
{
	CUIGroupHelper::TranslateUIMessage(ControlID, dwMsg);

	switch ( ControlID )
	{
	case IDD_FLYCAMERATIMELINE_TIMELINE_PROGRESSBAR:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )	// 컨트롤 내부에 마우스가 있는 경우;
			{
				if ( CHECK_LB_DOWN_LIKE ( dwMsg ) )
				{			
					m_pProgTimeLine->SetExclusiveControl();		// 단독 컨트롤로 등록;
				}
				else if ( CHECK_LB_UP_LIKE ( dwMsg ) )
				{
					m_pProgTimeLine->ResetExclusiveControl();	// 단독 컨트롤을 해제;
				}
			}
			//	컨트롤 영역밖에서 버튼을 떼는 경우 처리;
			else if ( CHECK_LB_UP_LIKE ( dwMsg ) )		
			{											
				m_pProgTimeLine->ResetExclusiveControl();	// 단독 컨트롤을 해제;
			}
		}
		break;

	case IDD_FLYCAMERATIMELINE_ADD_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
				AddMessageEx(FLYCAMERA_UI_MSG_DEFINE::dwFLYCAMERA_UIMSG_KEY_ADD | UIMSG_TOTOPPARENT);
		}
		break;

	case IDD_FLYCAMERATIMELINE_DEL_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
				AddMessageEx(FLYCAMERA_UI_MSG_DEFINE::dwFLYCAMERA_UIMSG_KEY_DEL | UIMSG_TOTOPPARENT);
		}
		break;

	case IDD_FLYCAMERATIMELINE_KEY_EDITBOX:
	case IDD_FLYCAMERATIMELINE_TOTAL_EDITBOX:
		{
		}
		break;

	case IDD_FLYCAMERATIMELINE_PLAY_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
				m_pGaeaClient->FlyCamera_AddMsg(FlyCameraControl::EMFLYCAMERA_PLAY_ANIMATION, 0);

			// GLFlyCameraControl::GetInstance().PlayVideo();
		}
		break;
	case IDD_FLYCAMERATIMELINE_STOP_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
				m_pGaeaClient->FlyCamera_AddMsg(FlyCameraControl::EMFLYCAMERA_STOP_ANIMATION, 0);
		}
		break;
	case IDD_FLYCAMERATIMELINE_REWIND_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
				m_pGaeaClient->FlyCamera_AddMsg(FlyCameraControl::EMFLYCAMERA_REWIND_ANIMATION, 0);	
		}
		break;
	}
}

void FlyCameraControlKeyframeTimeLine::Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl)
{
	CUIGroupHelper::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( bFirstControl == TRUE )
	{
		if ( m_pProgTimeLine && m_pProgTimeLine->IsExclusiveSelfControl() )	// 단독 컨트롤 시;
		{
			const float fSize = m_pProgTimeLine->GetGlobalPos().sizeX;
			const float fPos = float(x) - m_pProgTimeLine->GetGlobalPos().left;
			const float fPercent = fPos/fSize;
			m_pProgTimeLine->SetRate(fPercent);
			m_nCurrentKeyFrame = m_pProgTimeLine->GetNow();

			CString strEditString;
			strEditString.Format("%d", m_nCurrentKeyFrame);
			m_pEdtMan->SetEditString(IDD_FLYCAMERATIMELINE_KEY_EDITBOX, strEditString.GetString());
		}
	}

	{
		// EnterKey;
		UIGUID uiId;
		if ( (uiId = m_pEdtMan->GetBeginEditBox()) != NO_ID )
		{
			if ( UIKeyCheck::GetInstance()->Check(DIK_RETURN, DXKEY_DOWN) 
				|| UIKeyCheck::GetInstance()->Check(DIK_NUMPADENTER,DXKEY_DOWN) )
			{
				_ExpansionEditBoxMsgCheck(uiId);
			}
		}

// 		CUIEditBox* pEditBox = m_pEdtMan->GetEditBox(IDD_FLYCAMERATIMELINE_FPS_EDITBOX);
// 		if ( pEditBox->IsBegin() )
// 		{
// 			if ( UIKeyCheck::GetInstance()->Check(DIK_RETURN,DXKEY_DOWN) 
// 				|| UIKeyCheck::GetInstance()->Check(DIK_NUMPADENTER,DXKEY_DOWN) )
// 			{
// 				int nFPS = atoi(pEditBox->GetEditString());
// 				m_pGaeaClient->FlyCamera_AddMsg(FlyCameraControl::EMFLYCAMERA_SET_FPS, nFPS);
// 				m_nFPS = nFPS;
// 			}
// 		}
	}

	// 플레이시 프로그레스바;
	{
		if ( GLFlyCameraControl::GetInstance().IsPlayVideo() && m_pProgTimeLine )
		{
			m_nCurrentKeyFrame = GLFlyCameraControl::GetInstance().GetCurrentFrame();
			m_pProgTimeLine->SetNow(m_nCurrentKeyFrame);
		}
	}
}

bool FlyCameraControlKeyframeTimeLine::_ExpansionEditBoxMsgCheck(UIGUID uiID)
{
	const CString strEditString = m_pEdtMan->GetEditString(uiID);
	const int nEditNum = atoi(strEditString.GetString());

	switch ( uiID )
	{
	case IDD_FLYCAMERATIMELINE_KEY_EDITBOX:
		{			
			m_nCurrentKeyFrame = nEditNum;
			m_pProgTimeLine->SetNow(m_nCurrentKeyFrame);
			m_pGaeaClient->FlyCamera_AddMsg(FlyCameraControl::EMFLYCAMERA_SET_CURRENTKEYFRAME, m_nCurrentKeyFrame);
		}
		return true;
	case IDD_FLYCAMERATIMELINE_TOTAL_EDITBOX:
		{
			m_nTotalKeyFrame = nEditNum;
			m_pProgTimeLine->SetMax(m_nTotalKeyFrame);
			m_pGaeaClient->FlyCamera_AddMsg(FlyCameraControl::EMFLYCAMERA_SET_TOTALFRAME, m_nTotalKeyFrame);
		}
		return true;
	case IDD_FLYCAMERATIMELINE_FPS_EDITBOX:
		{
			int nFPS = nEditNum;
			m_pGaeaClient->FlyCamera_AddMsg(FlyCameraControl::EMFLYCAMERA_SET_FPS, nFPS);
			m_nFPS = nFPS;
		}
	}
	return false;
}