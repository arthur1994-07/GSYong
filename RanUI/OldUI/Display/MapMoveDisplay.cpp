#include "StdAfx.h"
#include "MapMoveDisplay.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Stage/DxGlobalStage.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CMapMoveDisplay::CMapMoveDisplay(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient ( pGaeaClient )
    , m_pTextBox ( NULL )
    , m_pMoveOKButton ( NULL )
{
}

CMapMoveDisplay::~CMapMoveDisplay ()
{
}

void CMapMoveDisplay::CreateSubControl ()
{
	CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	CBasicLineBox* pLineBox = new CBasicLineBox(m_pEngineDevice);
	pLineBox->CreateSub ( this, "BASIC_LINE_BOX_MAPMOVE", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLineBox->CreateBaseBoxMapMove ( "BASIC_MAPMOVE_LINE_BOX" );
	RegisterControl ( pLineBox );	

	CBasicTextBox* pMapMoveTextBox = new CBasicTextBox(m_pEngineDevice);
	pMapMoveTextBox->CreateSub ( this, "BASIC_MAPMOVE_TEXTBOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    pMapMoveTextBox->SetFont ( pFont9 );
	pMapMoveTextBox->SetTextAlign ( TEXT_ALIGN_CENTER_X );
	RegisterControl ( pMapMoveTextBox );
	m_pTextBox = pMapMoveTextBox;

	m_pMoveOKButton = CreateTextButton ( "BASIC_MAPMOVE_OK", MAPMOVE_OKBUTTON, (char*)ID2GAMEWORD ( "MAPMOVE_OKBUTTON" ) );
	m_pMoveOKButton->SetShortcutKey ( DIK_SPACE );
}

CBasicTextButton* CMapMoveDisplay::CreateTextButton ( char* szButton, UIGUID ControlID , char* szText )
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE14;
	CBasicTextButton* pButton = new CBasicTextButton(m_pEngineDevice);
	pButton->CreateSub ( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, ControlID );
	pButton->CreateBaseButton ( szButton, nBUTTONSIZE, CBasicButton::CLICK_FLIP, szText );
	RegisterControl ( pButton );
	return pButton;
}

void CMapMoveDisplay::DoMAPMOVE ()
{
	m_pGaeaClient->GetCharacter()->ReqGateOut();
}

void CMapMoveDisplay::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case MAPMOVE_OKBUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) || CHECK_KEYFOCUSED ( dwMsg ) )
			{
				//	Note : 출구 나가기 요청 메시지 발생.
				//
				DoMAPMOVE ();
			}
		}
        break;
	}
}

void	CMapMoveDisplay::SetMapName ( CString strName )
{
	if ( m_pTextBox )
	{
		m_pTextBox->SetText ( strName );
	}
}








CMultiMapMoveDisplay::CMultiMapMoveDisplay(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice)
: CUIGroupHelper(pEngineDevice)
, m_pGaeaClient ( pGaeaClient )
, m_pMoveOKButton ( NULL )
, m_nSelectIndex(0)
, m_nViewIndex(0)
, m_bDone(false)
{
	for ( DWORD _i(0); _i < 3; ++_i )
	{
		m_pTextBox[_i] = NULL;
	}
}

CMultiMapMoveDisplay::~CMultiMapMoveDisplay ()
{
	for ( DWORD _i(0); _i < 3; ++_i )
	{
		m_pTextBox[_i] = NULL;
	}	
}

void CMultiMapMoveDisplay::CreateSubControl ()
{
	CreateLineBox( "MULTIMAPMOVE_WINDOW_BOX", "PARTY_INFO_LINEBOX_TEXTUREINFO" );
	CreateLineBox( "MULTIMAPMOVE_LIST_BOX", "PARTY_INFO_LINEBOX_TEXTUREINFO" );

	{ // 스크롤 바;
		CBasicScrollBarEx* pScrollBar;
		pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
		pScrollBar->CreateSub(this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE);
		pScrollBar->CreateBaseScrollBar("MULTIMAPMOVE_LIST_SCROLLBAR");
		pScrollBar->GetThumbFrame()->SetScrollParent(this);
		pScrollBar->GetThumbFrame()->SetScrollMoveLine(15);
		RegisterControl(pScrollBar);
		pScrollBar->SetVisibleSingle(FALSE);
		m_pScrollBar = pScrollBar;
	}	

	{
		CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
		const char* const szTextBox[3] = 
		{
			"MULTIMAPMOVE_TEXTBOX_0",
			"MULTIMAPMOVE_TEXTBOX_1",
			"MULTIMAPMOVE_TEXTBOX_2",
		};
		const char* const szSelectBox[3] = 
		{
			"MULTIMAPMOVE_SELECTBOX_0",
			"MULTIMAPMOVE_SELECTBOX_1",
			"MULTIMAPMOVE_SELECTBOX_2",
		};

		CBasicTextBox* pInformationTextBox = new CBasicTextBox(m_pEngineDevice);
		pInformationTextBox->CreateSub ( this, "MULTIMAPMOVE_INFORMATION_TEXT", UI_FLAG_XSIZE | UI_FLAG_YSIZE);
		pInformationTextBox->SetFont ( pFont9 );
		pInformationTextBox->SetTextAlign ( TEXT_ALIGN_CENTER_X );
		RegisterControl ( pInformationTextBox );
		pInformationTextBox->SetText(ID2GAMEINTEXT("MULTI_GATE_INFORMATIOIN_TEXT"));
		
		for ( DWORD _i(0); _i < MULTIMAPMOVE_VISIBLE_NLINE; ++_i )
		{
			CBasicTextBox* pMapMoveTextBox = new CBasicTextBox(m_pEngineDevice);
			pMapMoveTextBox->CreateSub ( this, szTextBox[_i], UI_FLAG_XSIZE | UI_FLAG_YSIZE, MULTIMAPMOVE_TEXT_BOX_0 + _i);
			pMapMoveTextBox->SetFont ( pFont9 );
			pMapMoveTextBox->SetTextAlign ( TEXT_ALIGN_CENTER_X );
			RegisterControl ( pMapMoveTextBox );
			m_pTextBox[_i] = pMapMoveTextBox;

			m_pSelectBox[_i] = CreateLineBox(szSelectBox[_i], "PARTY_INFO_SELECTBOX_TEXTUREINFO");
		}	
	}

	{
		const int nBUTTONSIZE = CBasicTextButton::SIZE14;
		CBasicTextButton* pOKButton = new CBasicTextButton(m_pEngineDevice);
		pOKButton->CreateSub ( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, MULTIMAPMOVE_OKBUTTON );
		pOKButton->CreateBaseButton ( "MULTIMAPMOVE_WINDOW_OK_BUTTON", nBUTTONSIZE, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD ( "MAPMOVE_OKBUTTON" ) );
		pOKButton->SetShortcutKey ( DIK_SPACE );
		RegisterControl ( pOKButton );
		m_pMoveOKButton = pOKButton;
	}	
}



void CMultiMapMoveDisplay::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case MULTIMAPMOVE_OKBUTTON:
		if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) || CHECK_KEYFOCUSED ( dwMsg ) )
		{
			//	Note : 출구 나가기 요청 메시지 발생.
			//
			m_pGaeaClient->GetCharacter()->ReqGateOut(m_nSelectIndex);
		}
		break;
	case MULTIMAPMOVE_TEXT_BOX_0:
	case MULTIMAPMOVE_TEXT_BOX_1:
	case MULTIMAPMOVE_TEXT_BOX_2:
		if ( CHECK_MOUSE_IN_LBDOWNLIKE(dwMsg) == TRUE )
		{
			const DWORD nIndex(ControlID - MULTIMAPMOVE_TEXT_BOX_0);
			m_nSelectIndex = m_nViewIndex + nIndex;			
		}
		break;
	}
}
void CMultiMapMoveDisplay::SetVisibleSingle(BOOL bVisible)
{	
	if ( bVisible == TRUE && CUIGroupHelper::IsVisible() == FALSE )
	{
		m_bDone = false;
		m_mapNameVector.clear();
		m_nSelectIndex = 0;
		m_nViewIndex = 0;
	}
	CUIGroupHelper::SetVisibleSingle(bVisible);
}

void CMultiMapMoveDisplay::AddMapName(const CString& strName)
{
	if ( m_bDone == true )
		return;

	m_mapNameVector.push_back(strName);

	for ( DWORD _i(0); _i < 3; ++_i )
		m_pTextBox[_i]->SetVisibleSingle(FALSE);

	const DWORD nSize(m_mapNameVector.size() > 3 ? 3 : m_mapNameVector.size());
	for ( DWORD _i(0); _i < nSize; ++_i )
		m_pTextBox[_i]->SetVisibleSingle(TRUE);
}

void CMultiMapMoveDisplay::Done(void)
{
	if ( m_bDone == false )
		m_bDone = true;
	CBasicScrollThumbFrame* const pThumbFrame = m_pScrollBar->GetThumbFrame();

	const DWORD nNameSize(m_mapNameVector.size());
	//pThumbFrame->SetState(DxLandGate::GATETO_NSIZE, nNameSize);
	pThumbFrame->SetState(nNameSize, MULTIMAPMOVE_VISIBLE_NLINE);
	const int nMovableLine = nNameSize - MULTIMAPMOVE_VISIBLE_NLINE;
	const float fPercent = pThumbFrame->GetPercent();
	const DWORD dwPrevBegineIndex(m_nViewIndex);
	if ( nMovableLine > 0 )
	{
		m_nViewIndex =  (DWORD)floor(fPercent * float(nMovableLine));
		m_pScrollBar->SetVisibleSingle(TRUE);
	}
	else
	{
		m_nViewIndex = 0;
		m_pScrollBar->SetVisibleSingle(FALSE);
	}

	const DWORD nView(m_nViewIndex + MULTIMAPMOVE_VISIBLE_NLINE);
	for ( DWORD _i(m_nViewIndex); (_i < nNameSize) && (_i < nView); ++_i )
	{
		m_pTextBox[_i - m_nViewIndex]->SetText(m_mapNameVector[_i]);
	}
	const DWORD nIndex(m_nSelectIndex - m_nViewIndex);
	for ( DWORD _i(MULTIMAPMOVE_VISIBLE_NLINE); _i--; )
		m_pSelectBox[_i]->SetVisibleSingle(FALSE);
	if ( nIndex < MULTIMAPMOVE_VISIBLE_NLINE )
		m_pSelectBox[nIndex]->SetVisibleSingle(TRUE);

}