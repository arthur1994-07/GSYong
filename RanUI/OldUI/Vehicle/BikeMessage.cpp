#include "StdAfx.h"
#include "BikeMessage.h"
#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Stage/DxGlobalStage.h"
#include "../../InnerInterface.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CBikeMessageWindow::CBikeMessageWindow(EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pTextBox ( NULL )
{
}

CBikeMessageWindow::~CBikeMessageWindow ()
{
}

void CBikeMessageWindow::CreateSubControl ()
{
	CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	CBasicLineBox* pLineBox = new CBasicLineBox(m_pEngineDevice);
	pLineBox->CreateSub ( this, "BASIC_LINE_BOX_SYSTEM_MESSAGE", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLineBox->CreateBaseBoxSystemMessage ( "BASIC_SYSTEM_MESSAGE_LINE_BOX" );
	pLineBox->ResetAlignFlag ();
	RegisterControl ( pLineBox );	

	pLineBox = new CBasicLineBox(m_pEngineDevice);
	pLineBox->CreateSub ( this, "BASIC_LINE_BOX_MINIPARTY", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLineBox->CreateBaseBoxSystemMessageUp ( "BASIC_SYSTEM_MESSAGE_LINE_TEXTBOX" );
	pLineBox->ResetAlignFlag ();
	RegisterControl ( pLineBox );

	CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
	pTextBox->CreateSub ( this, "BASIC_SYSTEM_MESSAGE_TEXTBOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pTextBox->SetFont ( pFont9 );
	pTextBox->SetTextAlign ( TEXT_ALIGN_BOTH_X );
	pTextBox->ResetAlignFlag ();
	RegisterControl ( pTextBox );
	m_pTextBox = pTextBox;
	m_pTextBox->SetOneLineText( ID2GAMEINTEXT ( "BIKE_MESSAGE_PASSENGER" ), NS_UITEXTCOLOR::DISABLE );
}