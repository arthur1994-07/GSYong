
#include "stdafx.h"

#include "RnCharacterInfoTap.h"

#include "RnStatInfoTap.h"
#include "RnHistoryInfoTap.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/UITextUtil.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"

#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../enginelib/DxTools/DxFontMan.h"

#include "../../../enginelib/GUInterface/BasicLineBoxSmart.h"


#include "../../../RanLogic/GLogicEx.h"
#include "../../../RanLogic/Character/GLCharDefine.h"
#include "../../../RanLogic/GLUseFeatures.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/Char/GLCharClient.h"
#include "../../../RanLogicClient/Club/GLClubClient.h"

#include "../../../EngineLib/GUInterface/SmartComboBoxRollOver.h"

#include "../../InnerInterface.h"


#include "RnCharacterWearWindow.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


namespace
{
	const char* ID2GAMEINTEXT_EMPTY_CLUB = "소속된 클럽이 없습니다";
	const char* ID2GAMEINTEXT_BADGE = "뱃지";

	const char* ID2GAMEINTEXT_STAT = "능력";
	const char* ID2GAMEINTEXT_HISTORY = "기록";
	const char* ID2GAMEINTEXT_DEPARTMENT = "소속";
//	const char* ID2GAMEINTEXT_BRIGHT = "성향";
//	const char* ID2GAMEINTEXT_ACTIVITY_POINT = "생활점수";
	const char* ID2GAMEINTEXT_CHECK = "조회수";
	const char* ID2GAMEINTEXT_CONTRIBUTION = "기여도";


	const char* ID2GAMEINTEXT_ACADEMY_QUESTION_WORD[2] = {"성향","생활점수"};

	const char* ID2GAMEINTEXT_ACADEMY_QUESTION_SENTENCE[2] = {  
		"성향은 적이 아닌 대상을 PK할 시 어둠 성향으로 이동하고 적을 처지할 시 빛 성향으로 이동합니다."
		"성향에 따라 일부 아이템을 사용할 수 없게되거나 일부 지역에 진입할 수 없게 될 수 있고, 각지에 위치한"
		"선도부원에게 공격당할 수도 있습니다.",

		"대련 또는 퀘스트, 출석부 과제를 수행함으로써 얻게되는 점수입니다."
		"생활점수로 일부 아이템을 구입할 수 있고, 일부 지역에 입장할 수 있는 자격이 주어집니다."};
	
}

RnCharacterInfoTap::RnCharacterInfoTap( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, 
									   EngineDeviceMan* pEngineDevice) :
	CUIGroupHelper(pEngineDevice)
{
	m_pGaeaClient = pGaeaClient;
	m_pInterface  = pInterface;

	m_pCurrentTap = NULL;
	m_pStatTap = NULL;
	m_pHistoryTap = NULL;

	ID2GAMEINTEXT_EMPTY_CLUB = ID2GAMEINTEXT("CHARACTER_WINDOW_EMPTY_CLUB");
	ID2GAMEINTEXT_BADGE = ID2GAMEINTEXT("CHARACTER_WINDOW_BADGE");
	ID2GAMEINTEXT_STAT = ID2GAMEINTEXT("CHARACTER_WINDOW_STAT");
	ID2GAMEINTEXT_HISTORY = ID2GAMEINTEXT("CHARACTER_WINDOW_HISTORY");
	ID2GAMEINTEXT_DEPARTMENT = ID2GAMEINTEXT("CHARACTER_WINDOW_DEPARTMENT");
	ID2GAMEINTEXT_CHECK = ID2GAMEINTEXT("CHARACTER_WINDOW_CHECK");
	
	ID2GAMEINTEXT_ACADEMY_QUESTION_WORD[0] = ID2GAMEINTEXT("CHARACTER_WINDOW_ACADEMY_QUESTION_WORD",0);
	ID2GAMEINTEXT_ACADEMY_QUESTION_WORD[1] = ID2GAMEINTEXT("CHARACTER_WINDOW_ACADEMY_QUESTION_WORD",1);
	
	ID2GAMEINTEXT_ACADEMY_QUESTION_SENTENCE[0] = ID2GAMEINTEXT("CHARACTER_WINDOW_ACADEMY_QUESTION_SENTENCE_1");
	ID2GAMEINTEXT_ACADEMY_QUESTION_SENTENCE[1] = ID2GAMEINTEXT("CHARACTER_WINDOW_ACADEMY_QUESTION_SENTENCE_2");

	ID2GAMEINTEXT_CONTRIBUTION = ID2GAMEWORD("CHARACTER_SOCIAL_STATIC", 7);

	m_pCountryMark = NULL;
}
	

void RnCharacterInfoTap::CreateSubControl()
{
	CreateLineBox( "CHARACTER_WINDOW_EQUIP_REGION", "CHARACTER_WINDOW_LINEBOX_TEXTUREINFO" );
	CreateLineBox( "CHARACTER_WINDOW_SCHOOL_REGION", "CHARACTER_WINDOW_LINEBOX_TEXTUREINFO" );

	CBasicLineBoxSmart* pLineBox = new CBasicLineBoxSmart(GetEngineDeviceMan());
	pLineBox->CreateSub( this, "CHARACTER_WINDOW_TITLE_NAME_REGION",UI_FLAG_DEFAULT,TITLE_REGION);
	pLineBox->CreateSubControl( "CHARACTER_WINDOW_LINEBOX_TEXTUREINFO" );
	RegisterControl( pLineBox );
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	RnCharacterWearWindow* pCharWearWindow = new RnCharacterWearWindow(m_pGaeaClient,m_pInterface,m_pEngineDevice);
	pCharWearWindow->CreateSub(this,"CHARACTER_WINDOW_WEAR_WINDOW");
	pCharWearWindow->CreateSubControl();
	RegisterControl(pCharWearWindow);
	m_pCharacterWearWindow = pCharWearWindow;

	//////////////////////////////////////////////////////////////////////////

	if ( GLUseFeatures::GetInstance().IsUsingSelectCountry() )
	{
		m_pCountryMark = new CSwapImage( m_pEngineDevice );;
		m_pCountryMark->CreateSub( this, "CHARACTER_WINDOW_COUNTRY_MARK", UI_FLAG_DEFAULT, NAME_COUNTRY_MARK );
		RegisterControl( m_pCountryMark );
		m_pCountryMark->SetVisibleSingle( FALSE );
	}

	m_pCharName = CreateText("CHARACTER_WINDOW_CHAR_NAME_TEXT","CHARACTER_WINDOW_CHARACTER_NAME_TEXT",TEXT_ALIGN_CENTER_X|TEXT_ALIGN_CENTER_Y);
	m_pClubName = CreateText("CHARACTER_WINDOW_CLUB_NAME_TEXT","CHARACTER_WINDOW_CLUB_NAME_TEXT",TEXT_ALIGN_CENTER_X|TEXT_ALIGN_CENTER_Y);
	//m_pTitleName = CreateText("CHARACTER_WINDOW_TITLE_NAME_TEXT","CHARACTER_WINDOW_TITLE_NAME_TEXT",TEXT_ALIGN_CENTER_X|TEXT_ALIGN_CENTER_Y);
    m_pTitleName = CreateText( ID2GAMEWORD( "CAHRWINDOW_TITLE", 0 ),"CHARACTER_WINDOW_TITLE_NAME_TEXT",TEXT_ALIGN_CENTER_X|TEXT_ALIGN_CENTER_Y);
	m_pTitleButton = CreateFlipButton("CHARACTER_WINDOW_TITLE_BUTTON","CHARACTER_WINDOW_TITLE_BUTTON_OVER",TITLE_BUTTON,CBasicButton::MOUSEIN_FLIP);
	m_pTitleButton->SetNoUpdate(TRUE);

	if ( !GLUseFeatures::GetInstance().IsUsingActivity() ) 
	{
		m_pTitleName->SetVisibleSingle( FALSE );
		m_pTitleButton->SetVisibleSingle( FALSE );
	}

	CSmartComboBoxRollOver* pComboBoxRollOver = new CSmartComboBoxRollOver(m_pEngineDevice);
	pComboBoxRollOver->CreateSub ( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, TITLE_COMBO_ROLLOVER );
	pComboBoxRollOver->CreateSmartComboBoxRollOver( "CHARACTER_WINDOW_TITLE_COMBO_ROLLOVER", "CHARACTER_WINDOW_TITLE_COMBO_ROLLOVER_TEXINFO" );
	pComboBoxRollOver->SetDiffuse( NS_UITEXTCOLOR::DARKGRAY );
	pComboBoxRollOver->SetVisibleSingle ( FALSE );
	RegisterControl ( pComboBoxRollOver );
	m_pComboBoxTitleRollOver = pComboBoxRollOver;


	//////////////////////////////////////////////////////////////////////////
	CreateFlipButton("CHARACTER_WINDOW_ACADEMY_QUESTION_BUTTON","CHARACTER_WINDOW_ACADEMY_QUESTION_BUTTON_OVER",ACADEMY_QUESTION_BUTTON,CBasicButton::MOUSEIN_FLIP);

	m_pAcademyDepartment = CreateText("ACADEMY_DEPARTMENT_TEXT","CHARACTER_WINDOW_ACADEMY_DEPARTMENT_TEXT",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);
	CreateText(ID2GAMEWORD("CHARACTER_SOCIAL_STATIC", 5),"CHARACTER_WINDOW_BRIGHT_TEXT",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);
	m_pActivityPointText = CreateText(ID2GAMEWORD("CHARACTER_SOCIAL_STATIC", 2),"CHARACTER_WINDOW_ACTIVITY_POINT_TEXT",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);
	m_pActivityPointValue = CreateText("0","CHARACTER_WINDOW_ACTIVITY_POINT_VALUE",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);
//	m_pCheckText = CreateText(ID2GAMEINTEXT_CHECK,"CHARACTER_WINDOW_CHECK_TEXT",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);


	CreateText(ID2GAMEINTEXT_CONTRIBUTION,"CHARACTER_WINDOW_CONTRIBUTION_POINT_TEXT",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);
	m_pContributionValue = CreateText("0","CHARACTER_WINDOW_CONTRIBUTION_POINT_VALUE",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);


	m_pBrightFrame = CreateControl("CHARACTER_WINDOW_BRIGHTBAR",BRIGHT_FRAME );
	m_pBrightSlider = CreateControl("CHARACTER_WINDOW_BRIGHTBAR_THUMB",BRIGHT_SLIDER );

	//////////////////////////////////////////////////////////////////////////

	m_pStatTap	= new RnStatInfoTap(m_pGaeaClient,m_pInterface,m_pEngineDevice);
	m_pStatTap->CreateSub(this,"CHARACTER_WINDOW_STAT_REGION");
	m_pStatTap->CreateSubControl();
	RegisterControl(m_pStatTap);

	m_pHistoryTap = new RnHistoryInfoTap(m_pGaeaClient,m_pInterface,m_pEngineDevice);
	m_pHistoryTap->CreateSub(this,"CHARACTER_WINDOW_STAT_REGION");
	m_pHistoryTap->CreateSubControl();
	RegisterControl(m_pHistoryTap);

	DEFAULT_RADIO_BUTTON* pStatRadio = new DEFAULT_RADIO_BUTTON(m_pEngineDevice);
	pStatRadio->CreateSub(this,"CHARACTER_WINDOW_STAT_RADIO_BUTTON", UI_FLAG_LEFT | UI_FLAG_CENTER_Y, STAT_RADIO);
	pStatRadio->CreateSubControl(ID2GAMEINTEXT_STAT, TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y ,"CHARACTER_WINDOW_STAT_RADIO_BUTTON_TEXT",NS_UITEXTUTIL::BLACK,"CHARACTER_WINDOW_RADIO_BUTTON_CHECK_BOX","CHARACTER_WINDOW_RADIO_ON_TEXTUREINFO","CHARACTER_WINDOW_RADIO_OFF_TEXTUREINFO");


	RegisterTapButton(pStatRadio);
	RegisterControl(pStatRadio);

    //미구현 기능 숨김
// 	DEFAULT_RADIO_BUTTON* pHistoryRadio = new DEFAULT_RADIO_BUTTON(m_pEngineDevice);
// 	pHistoryRadio->CreateSub(this,"CHARACTER_WINDOW_HISTORY_RADIO_BUTTON",UI_FLAG_DEFAULT,HISTORY_RADIO);
// 	pHistoryRadio->CreateSubControl(ID2GAMEINTEXT_HISTORY, TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y,"CHARACTER_WINDOW_HISTORY_RADIO_BUTTON_TEXT",NS_UITEXTUTIL::BLACK,"CHARACTER_WINDOW_RADIO_BUTTON_CHECK_BOX","CHARACTER_WINDOW_RADIO_ON_TEXTUREINFO","CHARACTER_WINDOW_RADIO_OFF_TEXTUREINFO");
// 
// 	RegisterTapButton(pHistoryRadio);
// 	RegisterControl(pHistoryRadio);

	EventSelectedTap(STAT_RADIO);
}

void RnCharacterInfoTap::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
// 	switch ( ControlID )
// 	{
// 	case STAT_RADIO:
// 	case HISTORY_RADIO:
// 		{
// 			EMTAPSELECTORRESULT emResult = EMTAPSELECTORRESULT_NONE;
// 			TapSelectorUIMessage( ControlID, dwMsg, emResult );
// 			if ( emResult == EMTAPSELECTORRESULT_SELECT )
// 			{
// 				SetTap(ControlID);
// 			}
// 		}
// 		break;
// 	}

	const GLCHARLOGIC& sCharData = m_pGaeaClient->GetCharacterLogic ();	
	BOOL bPointUsable = (sCharData.m_wStatsPoint)?TRUE:FALSE;

	switch(ControlID)
	{
	case TITLE_REGION:
		{
			m_pTitleButton->SetFlip( CHECK_MOUSE_IN( dwMsg ) );

			if( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
			{
				m_pComboBoxTitleRollOver->SetVisibleSingle(!m_pComboBoxTitleRollOver->IsVisible());
// 				if( m_pComboBoxTitleRollOver->IsVisible() )
// 					UpdateTitleData();
			}

			break;
		}

	case TITLE_COMBO_ROLLOVER:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{	
				int nIndex = m_pComboBoxTitleRollOver->GetSelectIndex ();
				if ( nIndex < 0 ) 
					return ;


				DWORD dwData = m_pComboBoxTitleRollOver->GetTextData( nIndex );

				if ( S_OK == m_pGaeaClient->GetCharacter()->ReqChangeTitle ( dwData ) ) 
				{
					SetTextTitleCombo( dwData );
				}

				m_pComboBoxTitleRollOver->SetVisibleSingle( FALSE );	
			}
			break;
		}
	case BRIGHT_FRAME:
	case BRIGHT_SLIDER:
		{
			if( CHECK_MOUSE_IN( dwMsg ) )
			{
				CString strCombine;
				strCombine.Format( "%d", sCharData.GETBRIGHTPER() );
				m_pInterface->SHOW_COMMON_LINEINFO(strCombine,NS_UITEXTCOLOR::LIGHTSKYBLUE);
			}
			break;
		}
	case ACADEMY_QUESTION_BUTTON:
		{
			if( CHECK_MOUSE_IN( dwMsg ) )
			{
				CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

				DWORD BLUE_COLOR = D3DCOLOR_ARGB(255,50,50,255);
				m_pInterface->BEGIN_COMMON_LINEINFO_MULTI(TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);

				for (int i=0; i < 2; ++i)
				{					
					CString strCombine;
					strCombine.Format( "%s", ID2GAMEINTEXT_ACADEMY_QUESTION_WORD[i] );
					m_pInterface->ADD_COMMON_LINEINFO_MULTI(strCombine, NS_UITEXTCOLOR::LIGHTSKYBLUE );

					NS_UITEXTUTIL::MULTILINETEXT multiLine = NS_UITEXTUTIL::GetMLTextWithoutCRLF(ID2GAMEINTEXT_ACADEMY_QUESTION_SENTENCE[i],350,pFont);

					for (size_t i=0; i < multiLine.size(); ++i)
						m_pInterface->ADD_COMMON_LINEINFO_MULTI(multiLine[i].strLine, NS_UITEXTUTIL::ENABLE);

					m_pInterface->ADD_COMMON_LINEINFO_MULTI("", NS_UITEXTUTIL::ENABLE);
				}

				m_pInterface->END_COMMON_LINEINFO_MULTI();
			}
			break;
		}
	case NAME_COUNTRY_MARK:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
			{
				if ( false == GLUseFeatures::GetInstance().IsUsingSelectCountry() )
					break;

				Country::GLCountryManClient* pCountryMan =
					Country::GLCountryManClient::Instance();
				if ( NULL == pCountryMan )
					return;

				Country::GLCountry* pCountry =
					pCountryMan->GetCountryUsingID( m_pGaeaClient->GetCountryInfo().dwID );
				if ( NULL == pCountry )
					return;

				m_pInterface->SHOW_COMMON_LINEINFO(
					sc::string::format( "%1%", pCountry->GetCountryName() ).c_str(),
					NS_UITEXTCOLOR::LIGHTSKYBLUE );
			}
			break;
		}
	}


	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );

	EMTAPSELECTORRESULT emResult = EMTAPSELECTORRESULT_NONE;
	TapSelectorUIMessage( ControlID, dwMsg, emResult );
}


void RnCharacterInfoTap::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	// 이걸 매번 실시간으로 업데이트 해야하는지는 좀 의심이 가지만 
	// 기존 코드 형태가 이러니 그냥 가져다 쓰자
	/*const GLCHARLOGIC& sCharData = m_pGaeaClient->GetCharacterLogic();	


	SetName(sCharData);
	SetClubName();
	SetBadge(sCharData);

//	m_pCharacterView->SetUpdateCharacter(m_pGaeaClient->GetCharacter());

	//////////////////////////////////////////////////////////////////////////

	SetAcademyDepartment( sCharData );
	SetBright( sCharData );
	SetActivityPoint( sCharData );
	SetContributionPoint(sCharData);*/
	
	CUIGroupHelper::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}


CUIGroup* RnCharacterInfoTap::GetTapWindow( UIGUID controlID )
{
	switch ( controlID )
	{
	case STAT_RADIO:
		return m_pStatTap;
	case HISTORY_RADIO:
//		return m_pHistoryTap;

		// 일단 m_pHistoryTap가 구현안됐으므로 에러메세지를 주고  m_pStatTap를 리턴
		m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::RED, ID2GAMEINTEXT("CHARACTER_WINDOW_TAP_NOT_IMPLEMENT_MSG") );
		return m_pCurrentTap;

	}

	GASSERT(false);
	return NULL;
}

void RnCharacterInfoTap::EventSelectedTap( UIGUID controlID )
{
	if( m_pCurrentTap )
		m_pCurrentTap->SetVisibleSingle(FALSE);

	m_pCurrentTap = GetTapWindow(controlID);
	m_pCurrentTap->SetVisibleSingle(TRUE);
}

//////////////////////////////////////////////////////////////////////////



void RnCharacterInfoTap::SetName( const GLCHARLOGIC& sCharData )
{
	CString strCombine;
	strCombine.Format ( "Lv. %d %s", sCharData.m_wLevel,sCharData.m_szName);
	m_pCharName->SetText(strCombine);

	if ( GLUseFeatures::GetInstance().IsUsingSelectCountry() )
	{
		const CBasicTextBox::STEXTPART& sPART = m_pCharName->GetTextPart( 0 );
		if ( sPART.dequeLine.empty() )
			return;

		if ( sPART.dequeLine[0].vectorWord.empty() )
			return;

		float fTextBoxPos = sPART.dequeLine[0].vectorWord.at( 0 ).x;

		const UIRECT& rcCharName = m_pCharName->GetGlobalPos();
		const UIRECT& rcCountryMark = m_pCountryMark->GetLocalPos();

		D3DXVECTOR2 vPosition =
			D3DXVECTOR2( fTextBoxPos - rcCharName.left - rcCountryMark.sizeX,
			rcCountryMark.top );

		Country::GLCountryManClient* pCountryMan =
			Country::GLCountryManClient::Instance();
		if ( NULL == pCountryMan )
			return;

		Country::GLCountry* pCountry =
			pCountryMan->GetCountryUsingID( m_pGaeaClient->GetCountryInfo().dwID );
		if ( NULL == pCountry )
		{
			if ( m_pCountryMark )
				m_pCountryMark->SetVisibleSingle(FALSE);

			return;
		}

		if ( m_pCountryMark )
		{
			m_pCountryMark->SetLocalPos( vPosition );
			m_pCountryMark->SetImage( pCountry->GetCountryMiniImageID().c_str() );
			m_pCountryMark->SetVisibleSingle( TRUE );
		}
	}
}

void RnCharacterInfoTap::SetClubName()
{
	CString clubName(m_pGaeaClient->GetMyClub()->Name());
	if(clubName.IsEmpty())
		clubName = ID2GAMEINTEXT_EMPTY_CLUB;

	m_pClubName->SetText("[" + clubName + "]" , NS_UITEXTCOLOR::YELLOW);
}

void RnCharacterInfoTap::SetBadge( const GLCHARLOGIC& sCharData )
{

	TitleManager& titleManager = m_pGaeaClient->GetCharacter()->m_TitleManager;
	MAPTITLE& mapTitle = titleManager.GetData();
	if( !mapTitle.empty() && !m_pComboBoxTitleRollOver->IsVisible() )
	{
		UpdateTitleData();
	}
//	m_pTitleName->SetText(ID2GAMEWORD( "CAHRWINDOW_TITLE", 0 ) ,NS_UITEXTCOLOR::PALEGREEN);
}


void RnCharacterInfoTap::SetAcademyDepartment( const GLCHARLOGIC& sCharData )
{
	CString strCombine;
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		strCombine.Format ( "%s, %s" ,ID2GAMEWORD("FULL_ACADEMY_NAME",sCharData.m_wSchool),
			CGameTextMan::GetInstance().GetCommentText("CHARCLASS", CharClassToIndex(sCharData.m_emClass)).GetString() );
	}
	else
	{
		strCombine.Format ( "%s, %s" ,ID2GAMEWORD("FULL_ACADEMY_NAME",sCharData.m_wSchool),
			COMMENT::CHARCLASS[CharClassToIndex(sCharData.m_emClass)].c_str() );
	}
	m_pAcademyDepartment->SetOneLineText ( strCombine, NS_UITEXTCOLOR::WHITE );
}

void RnCharacterInfoTap::SetBright( const GLCHARLOGIC& sCharData )
 {
	const int nMIN = -100;
	const int nMAX = 100;
	const int nRANGE = nMAX - nMIN;

	int nPercent = sCharData.GETBRIGHTPER ();
	nPercent = -(nPercent);	//	이미지가 반대로 뒤집혀있어서.
	if ( nPercent < nMIN ) 
		nPercent = nMIN;
	if ( nPercent > nMAX ) 
		nPercent = nMAX;

	nPercent += nMAX;

	float fPercent = static_cast<float>(nPercent) / static_cast<float>(nRANGE);

	const UIRECT& rcFramePos = m_pBrightFrame->GetLocalPos ();
	const UIRECT& rcThumbPos = m_pBrightSlider->GetLocalPos();

	const UIRECT& rcLocalPos = m_pBrightSlider->GetLocalPos();

	float fMOVE = (rcFramePos.sizeX - rcThumbPos.sizeX) * fPercent;
	float fLeft = rcFramePos.left + fMOVE;

	//m_pBrightSlider->SetGlobalPos( D3DXVECTOR2 ( fLeft, rcThumbPos.top ) );
	m_pBrightSlider->SetLocalPos( D3DXVECTOR2 ( fLeft, rcLocalPos.top ) );
}

void RnCharacterInfoTap::SetActivityPoint( const GLCHARLOGIC& sCharData )
{
	CString strCombine;
	strCombine.Format ( "%d", sCharData.m_nLiving );
	m_pActivityPointValue->SetOneLineText ( strCombine, NS_UITEXTCOLOR::WHITE );
}

void RnCharacterInfoTap::SetContributionPoint( const GLCHARLOGIC& sCharData )
{
	CString strCombine;
	strCombine.Format ( "%d", sCharData.m_nContributionPoint );
	m_pContributionValue->SetOneLineText ( strCombine, NS_UITEXTCOLOR::WHITE );
}

void RnCharacterInfoTap::UpdateTitleData()
{
	std::vector<TITLEINFO>   vecTitle;
	TITLEINFO sTitle;

	m_pComboBoxTitleRollOver->ClearText ();

	//  뺏지없음
	sTitle.nIndex = UINT_MAX;
	sTitle.nClass = ACTIVITY_CLASS_SIZE;
	sTitle.strTitle = ID2GAMEWORD( "CAHRWINDOW_TITLE", 0 ) ;
	vecTitle.push_back( sTitle );


	TitleManager& titleManager = m_pGaeaClient->GetCharacter()->m_TitleManager;
	MAPTITLE& mapTitle = titleManager.GetData();

	ITER_MAPTITLE pos = mapTitle.begin();
	ITER_MAPTITLE end = mapTitle.end();

	for ( ; pos != end; ++pos )
	{
		TITLEINFO& title = pos->second;

		vecTitle.push_back( title );
	}

	std::sort( vecTitle.begin(), vecTitle.end() );

	D3DCOLOR dwTextColor = NS_UITEXTUTIL::ENABLE;
	DWORD nSelect = 0;
	for ( size_t i = 0; i < vecTitle.size(); ++i ) 
	{
		const TITLEINFO& title = vecTitle[i];

		dwTextColor = NS_UITEXTCONTROL::GetActivityColor( title.nClass );

		int nIndex = 0;

		if ( title.nIndex == UINT_MAX )
		{
			nIndex = m_pComboBoxTitleRollOver->AddText( title.strTitle.c_str(), FALSE, FALSE, dwTextColor );
		}
		else
		{
			nIndex = m_pComboBoxTitleRollOver->AddText( ID2GAMEINTEXT( title.strTitle.c_str() ), FALSE, FALSE, dwTextColor );
		}

		m_pComboBoxTitleRollOver->SetTextData( nIndex, title.nIndex );

		if (title.bSelected)
			nSelect = static_cast<int> (i);
	}

	SetTextTitleCombo(vecTitle[nSelect].nIndex);    
}

void RnCharacterInfoTap::SetArmSwapTabButton()
{
	m_pCharacterWearWindow->SetArmSwapTabButton();
}

void RnCharacterInfoTap::SetTextTitleCombo( DWORD dwIndex )
{
	std::string strText;
	ENUM_ACTIVITY_CLASS emClass = ACTIVITY_CLASS_SIZE;

	m_pTitleName->ClearText();

	// 뺏지없음
	if ( dwIndex == UINT_MAX )
	{
		emClass = ACTIVITY_CLASS_SIZE;
		strText = ID2GAMEWORD( "CAHRWINDOW_TITLE", 0 );
		m_pTitleName->AddText( strText.c_str() ,NS_UITEXTCONTROL::GetActivityColor( emClass ));
	}
	else
	{
		int index = m_pTitleName->AddText( CString(ID2GAMEINTEXT_BADGE) + " " );

		m_pGaeaClient->GetCharacter()->m_TitleManager.GetTitle( dwIndex, strText, emClass );
		strText = sc::string::format( "%1%", ID2GAMEINTEXT( strText.c_str() ) );
		m_pTitleName->AddString( index, strText.c_str(), NS_UITEXTCONTROL::GetActivityColor( emClass ) );
	}

	UpdateTitlePos();
}

void RnCharacterInfoTap::UpdateTitlePos()
{
// 	if ( !GLUseFeatures::GetInstance().IsUsingActivity() ) 
// 		return;
// 
// 	if ( m_pCharName->GetCount() == 0 )
// 		return;
// 
// 	static UIRECT rcNameRectBack = m_pCharName->GetLocalPos();
// 
// 	UIRECT rcGlobalTitleBox = m_pTitleName->GetGlobalPos();
// 	UIRECT rcLocalTitleBox = m_pTitleName->GetLocalPos();
// 
// 	UIRECT rcGlobalTitleButton = m_pTitleButton->GetGlobalPos();
// 	UIRECT rcLocalTitleButton = m_pTitleButton->GetLocalPos();
// 
// 	UIRECT rcGlobalRollOver = m_pComboBoxTitleRollOver->GetGlobalPos();
// 	UIRECT rcLocalRollOver = m_pComboBoxTitleRollOver->GetLocalPos();    
// 
// 	const float fGap = 2.0f;
// 
// 	float fLenTitle = 0.0f;  // 기본Gap
// 
// 	// Button Gap
// 	fLenTitle += rcLocalTitleButton.sizeX + fGap;
// 
// 	// TitleBox Gap 
// 	const CBasicTextBox::STEXTPART& sPARTTitle = m_pTitleName->GetTextPart ( 0 );
// 	float fTitleTextSize = static_cast<float>(sPARTTitle.dequeLine[0].strSize.cx);
// 	fLenTitle += fTitleTextSize + fGap;
// 
// 
// 	// NameBox 이름 위치
// 	const CBasicTextBox::STEXTPART& sPART = m_pCharName->GetTextPart ( 0 );
// 	float fNameTextSize = static_cast<float>(sPART.dequeLine[0].strSize.cx);
// 	float fNameLeft = ( ( rcNameRectBack.sizeX - fNameTextSize ) * 0.5f ) + rcNameRectBack.left;
// 
// 	float fLeftTitle = fNameLeft - fLenTitle;
// 
// 	if ( fLeftTitle < 0.0f ) 
// 	{
// 		// NameText size 변경
// 		UIRECT rcGlobalNameRect = m_pCharName->GetGlobalPos();
// 		UIRECT rcLocalNameRect = m_pCharName->GetLocalPos();
// 
// 		// 이미 줄어든 사이즈
// 		float fSizeGap = rcLocalNameRect.sizeX - rcNameRectBack.sizeX;
// 		fSizeGap = ( fLeftTitle * 2.0f ) - fSizeGap;
// 
// 
// 		rcGlobalNameRect.left -= fSizeGap;
// 		rcGlobalNameRect.sizeX += fSizeGap;
// 
// 		rcLocalNameRect.left -= fSizeGap;
// 		rcLocalNameRect.sizeX += fSizeGap;
// 
// 		m_pCharName->SetGlobalPos( rcGlobalNameRect );
// 		m_pCharName->SetLocalPos( rcLocalNameRect );
// 
// 		// 위치 갱신
// 		fLeftTitle = 0.0f;        
// 	}
// 	else
// 	{
// 		// NameText Size 원상 복구
// 		UIRECT rcGlobalNameRect = m_pCharName->GetGlobalPos();
// 		UIRECT rcLocalNameRect = m_pCharName->GetLocalPos();
// 
// 		// 이미 줄어든 사이즈
// 		float fSizeGap = rcLocalNameRect.sizeX - rcNameRectBack.sizeX;
// 
// 		rcGlobalNameRect.left += fSizeGap;
// 		rcGlobalNameRect.sizeX = rcNameRectBack.sizeX;
// 
// 		rcLocalNameRect = rcNameRectBack;
// 
// 
// 		m_pCharName->SetGlobalPos( rcGlobalNameRect );
// 		m_pCharName->SetLocalPos( rcLocalNameRect );
// 	}
// 
// 	fLeftTitle += fGap;
// 
// 	rcGlobalTitleButton.left = m_rcGlobalPos.left + fLeftTitle;
// 	rcLocalTitleButton.left = fLeftTitle;
// 
// 	rcGlobalRollOver.left = m_rcGlobalPos.left + fLeftTitle;
// 	rcLocalRollOver.left = fLeftTitle;
// 
// 	fLeftTitle += rcLocalTitleButton.sizeX + fGap;
// 
// 	rcGlobalTitleBox.left = m_rcGlobalPos.left + fLeftTitle;
// 	rcLocalTitleBox.left = fLeftTitle;
// 
// 
// 	m_pTitleName->SetGlobalPos( rcGlobalTitleBox );
// 	m_pTitleName->SetLocalPos( rcLocalTitleBox );
// 
// 	m_pTitleButton->SetGlobalPos( rcGlobalTitleButton );
// 	m_pTitleButton->SetLocalPos( rcLocalTitleButton );   
// 
// 	m_pComboBoxTitleRollOver->SetGlobalPos( rcGlobalRollOver );
// 	m_pComboBoxTitleRollOver->SetLocalPos( rcLocalRollOver );
}

// void RnCharacterInfoTap::UpdateCharacterView()
// {
// 	m_pCharacterWearWindow->UpdateCharacterView();
// }

void RnCharacterInfoTap::SetVisibleSingle( BOOL isVisible )
{
	CUIGroupHelper::SetVisibleSingle(isVisible);
	if( isVisible )
	{
		m_pCharacterWearWindow->UpdateCharacterView();

		const GLCHARLOGIC& sCharData = m_pGaeaClient->GetCharacterLogic();	


		SetName(sCharData);
		SetClubName();
		SetBadge(sCharData);

		//	m_pCharacterView->SetUpdateCharacter(m_pGaeaClient->GetCharacter());

		//////////////////////////////////////////////////////////////////////////

		SetAcademyDepartment( sCharData );
		SetBright( sCharData );
		SetActivityPoint( sCharData );
		SetContributionPoint(sCharData);
	}
}

void RnCharacterInfoTap::DeleteUIMeshData()
{
	m_pCharacterWearWindow->DeleteUIMeshData();
}