#include "StdAfx.h"
#include "./TournamentMoreInfo.h"

//#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../RanLogic/Tournament/GLTournamentClientInfo.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/CapturedTheField/GLCaptureTheFieldClient.h"
#include "../../../RanLogicClient/Tournament/GLTournamentClient.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../RanLogicClient/Tournament/GLTournamentClient.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"

#include "../../../EngineLib/DxTools/d3dfont.h"

#include "../../InnerInterface.h"


// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

using namespace MatchingSystem;

TournamentMoreInfo::TournamentMoreInfo(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
: CUIWindowEx(pInterface, pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_pTextBase_Class(NULL)
, m_pTextBase_Name(NULL)
, m_pTextBase_Level(NULL)
, m_pCloseButton(NULL)
, m_emCTFType(PVP::ENUM_CTF_TYPE_NULL)
{
	//Blank
}

TournamentMoreInfo::~TournamentMoreInfo()
{
	//Blank
}

void TournamentMoreInfo::CreateSubControl ()
{
	std::string strBaseControl = GetControlNameEx();
	const int nAlignCenter = TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y;
	const int nAlignLeftCenter = TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y;	

	CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
	CD3DFontPar* pFont_Shadow = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );
	//! 라인박스
	{
		CBasicLineBoxSmart* pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
		pLineBox->CreateSub( this, "TOUR_MINFO_BACKGROUND", UI_FLAG_DEFAULT );
		pLineBox->CreateSubControl( "COMPETITION_TOPRANKING_LINEBOX_TEXINFO" );
		RegisterControl( pLineBox );
	}

	//! 안에 라인박스
	{
		CBasicLineBoxSmart* pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
		pLineBox->CreateSub( this, "TOUR_MINFO_INBOX", UI_FLAG_DEFAULT );
		pLineBox->CreateSubControl( "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );
		RegisterControl( pLineBox );
	}

	//! 기본 텍스트
	{
		

		m_pTextBase_Level	= CreateStaticControl("TOUR_MINFO_LEV_TEXT", pFont_Shadow, NS_UITEXTCOLOR::ENABLE, nAlignCenter);
		m_pTextBase_Class	= CreateStaticControl("TOUR_MINFO_CLASS_TEXT", pFont_Shadow, NS_UITEXTCOLOR::ENABLE, nAlignCenter);
		m_pTextBase_Name	= CreateStaticControl("TOUR_MINFO_CHRNAME_TEXT", pFont_Shadow, NS_UITEXTCOLOR::ENABLE, nAlignCenter);

		m_pTextBase_Level->SetOneLineText( ID2GAMEWORD("SKILL_SLOT", 3), NS_UITEXTCOLOR::LIGHTGREY );
		m_pTextBase_Class->SetOneLineText( ID2GAMEWORD("CTF_RESULT_TEXT_BASE", 2), NS_UITEXTCOLOR::LIGHTGREY );
		m_pTextBase_Name->SetOneLineText( ID2GAMEWORD("CTF_RESULT_TEXT_BASE", 3), NS_UITEXTCOLOR::LIGHTGREY );
	}

	//! 랭킹 슬롯
	{
		m_vecSlot.resize(nMAX_SLOT);

		for ( UINT i=0; i<nMAX_SLOT; ++i )
		{
			RANKSLOT* &pSlot = m_vecSlot[i];

			pSlot = new RANKSLOT(m_pGaeaClient, m_pEngineDevice);
			pSlot->CreateSub( this, "TOUR_MINFO_SLOT_BASE", UI_FLAG_DEFAULT );
			pSlot->CreateSubControl( this );
			RegisterControl( pSlot );

			const UIRECT& rcSlotRect = pSlot->GetLocalPos();
			pSlot->SetLocalPos( D3DXVECTOR2(rcSlotRect.left, rcSlotRect.top + (rcSlotRect.sizeY * static_cast<float>(i)) ) );
		}
	}
	//TOUR_MINFO_INBOX

	//! 닫기 버튼
	{
		m_pCloseButton = new CBasicTextButton(m_pEngineDevice);
		m_pCloseButton->CreateSub(this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, COMPETITION_TOP_RANKING_BUTTON);
		m_pCloseButton->CreateBaseButton( "TOUR_MINFO_CLOSE_BUTTON", CBasicTextButton::SIZE19, 
			CBasicButton::CLICK_FLIP, ID2GAMEWORD("COMPETITION_TOPRANKING", 0) );
		RegisterControl( m_pCloseButton );
	}

	// "<" 버튼

	{

		RnButton::CreateArg arg;
		//////////////////////////////////////////////////////////////////////////
		arg.defaultTextureKey = "TOUR_MINFO_PAGE_BUTTON_L";
		arg.mouseOverTextureKey = "TOUR_MINFO_PAGE_BUTTON_L_F";
		arg.mouseClickTextureKey = "TOUR_MINFO_PAGE_BUTTON_L";
		//////////////////////////////////////////////////////////////////////////

		m_RnButton_LeftBox = new RnButton(m_pEngineDevice);
		m_RnButton_LeftBox->CreateSub(this, "TOUR_MINFO_PAGE_LEFT", UI_FLAG_DEFAULT, TOURNAMENT_MINFO_LEFT_BUTTON );
		m_RnButton_LeftBox->CreateSubControl(arg);
		m_RnButton_LeftBox->SetVisibleSingle(FALSE);
		RegisterControl(m_RnButton_LeftBox);
	}
	
	// 페이지 번호
	{
		CreateLineBox("TOUR_MINFO_PAGE_NUM","GRAY_BACKGROUND_BLACK_LINE_TEXTURE");

		m_pTextBase_PageCount = CreateStaticControl("TOUR_MINFO_PAGE_NUM", pFont_Shadow,0, nAlignCenter);
		m_pTextBase_PageCount->SetOneLineText("12", NS_UITEXTCOLOR::WHITE);
		m_pTextBase_PageCount->SetVisibleSingle(FALSE);
	}

	// ">" 버튼

	{
		RnButton::CreateArg arg;
		//////////////////////////////////////////////////////////////////////////
		arg.defaultTextureKey = "TOUR_MINFO_PAGE_BUTTON_R";
		arg.mouseOverTextureKey = "TOUR_MINFO_PAGE_BUTTON_R_F";
		arg.mouseClickTextureKey = "TOUR_MINFO_PAGE_BUTTON_R";
		//////////////////////////////////////////////////////////////////////////

		m_RnButton_RightBox = new RnButton(m_pEngineDevice);
		m_RnButton_RightBox->CreateSub(this, "TOUR_MINFO_PAGE_RIGHT", UI_FLAG_DEFAULT, TOURNAMENT_MINFO_RIGHT_BUTTON );
		m_RnButton_RightBox->CreateSubControl(arg);
		m_RnButton_RightBox->SetVisibleSingle(FALSE);
		RegisterControl(m_RnButton_RightBox);
	}
}

void TournamentMoreInfo::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIWindowEx::Update(x,y,LB,MB,RB,nScroll,fElapsedTime,bFirstControl);

	Update();
}

void TournamentMoreInfo::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage( ControlID, dwMsg );

	switch ( ControlID )
	{
	case COMPETITION_TOP_RANKING_BUTTON:	
		{
			if( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				m_pInterface->UiHideGroup( GetWndID() );
			}
		}
		break;
	};
}

void TournamentMoreInfo::Update()
{
	GLTouranmentClient* pTourClient = m_pGaeaClient->GetTournamentClient();

	for ( UINT i=0; i<nMAX_SLOT; ++i )
	{
		m_vecSlot[i]->SetEnable( false );
	}

	for ( UINT i=0; i<(UINT)pTourClient->m_MyBettingEployeeInfo.size(); ++i )
	{
		if(pTourClient->m_MyBettingEployeeInfo.size() == 0)
			continue;
		
		MatchingSystem::GroupInforEployee* sResultEployeeData = pTourClient->m_MyBettingEployeeInfo[i];
		m_vecSlot[i]->SetData( sResultEployeeData );
		m_vecSlot[i]->SetEnable( true );
	}
}

//----------------------------------------------------------------------------------------------------//

void TournamentMoreInfo::RANKSLOT::CreateSubControl ( TournamentMoreInfo* pBase )
{
	GASSERT(pBase);

	CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
	CD3DFontPar* pFont_Shadow = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

	const int nAlignCenter = TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y;	

	{
		pTEXTLEVEL = CreateStaticControl("TOUR_MINFO_LEV_TEXT_LIST_POS", pFont_Shadow, nAlignCenter);
		pTEXTNAME = CreateStaticControl("TOUR_MINFO_CHRNAME_TEXT_LIST_POS", pFont_Shadow, nAlignCenter);

		pIMAGECLASS = new CSwapImage(m_pEngineDevice);
		pIMAGECLASS->CreateSub ( this, "COMPETITION_TOPRANKING_SLOT_IMAGE_BASE_CLASS", UI_FLAG_DEFAULT );
		RegisterControl ( pIMAGECLASS );
	}

	{
		SetTextPosition( pTEXTLEVEL, pBase->GetBaseRectLevel() );
		SetTextPosition( pTEXTNAME, pBase->GetBaseRectName() );
		SetImagePosition( pIMAGECLASS, pBase->GetBaseRectClass(), GetLocalPos() );
	}

	{
		pTEXTLEVEL->SetOneLineText( "", NS_UITEXTCOLOR::WHITE );
		pTEXTNAME->SetOneLineText( "", NS_UITEXTCOLOR::WHITE );

		pIMAGECLASS->SetImage( "CREATE_CHAR_CLASS_BRAWLER_MALE" );
	}

	bENABLE = true;


	
}

void TournamentMoreInfo::RANKSLOT::Reset()
{
	SetEnable( false );
	strNAME = "";
}

void TournamentMoreInfo::RANKSLOT::SetEnable( const bool bEnable )
{
	bENABLE = bEnable;

	if ( bEnable )
	{
		SetVisibleSingle( TRUE );
	}
	else
	{
		SetVisibleSingle( FALSE );
	}
}

void TournamentMoreInfo::RANKSLOT::SetData( const MatchingSystem::GroupInforEployee* sResultData )
{
	static const std::string strClass[GLCI_NUM_ETC] = 
	{
		"CREATE_CHAR_CLASS_BRAWLER_MALE",
		"CREATE_CHAR_CLASS_SWORDMAN_MALE",
		"CREATE_CHAR_CLASS_ARCHER_FEMALE",
		"CREATE_CHAR_CLASS_SHAMAN_FEMALE",
		"CREATE_CHAR_CLASS_EXTREME_MALE",
		"CREATE_CHAR_CLASS_EXTREME_FEMALE",
		"CREATE_CHAR_CLASS_BRAWLER_FEMALE",
		"CREATE_CHAR_CLASS_SWORDMAN_FEMALE",
		"CREATE_CHAR_CLASS_ARCHER_MALE",
		"CREATE_CHAR_CLASS_SHAMAN_MALE",
		"CREATE_CHAR_CLASS_SCIENTIST_MALE",
		"CREATE_CHAR_CLASS_SCIENTIST_FEMALE",
		"CREATE_CHAR_CLASS_ASSASSIN_MALE",
		"CREATE_CHAR_CLASS_ASSASSIN_FEMALE",
		"CREATE_CHAR_CLASS_TRICKER_MALE",
		"CREATE_CHAR_CLASS_TRICKER_FEMALE",
		"CREATE_CHAR_CLASS_ETC_MALE",
		"CREATE_CHAR_CLASS_ETC_FEMALE",
	};

	//! 나머지는 아님
	if ( strNAME == sResultData->szSetName)
	{
		return;
	}

	strNAME = sResultData->szSetName;

	//EMCHARINDEX emIndex = CharClassToIndex(EMCHARCLASS());

	pTEXTLEVEL->SetOneLineText( sc::string::format("%d", sResultData->dwLevel).c_str(), NS_UITEXTCOLOR::WHITE);
	pTEXTNAME->SetOneLineText( sResultData->szSetName, NS_UITEXTCOLOR::WHITE);

	DWORD s_data = (DWORD)CharClassToIndex((EMCHARCLASS)sResultData->wClass);
	GASSERT(s_data < GLCI_NUM_ETC);

	pIMAGECLASS->SetImage( strClass[s_data].c_str() );
}

void TournamentMoreInfo::RANKSLOT::SetTextPosition( CBasicTextBox* pTextBox, const UIRECT& rcBase )
{
	GASSERT(pTextBox);

	UIRECT rcTextBox = pTextBox->GetLocalPos();
	rcTextBox.left  = rcBase.left;
	rcTextBox.right = rcBase.right;
	rcTextBox.sizeX = rcBase.sizeX;

	pTextBox->SetLocalPos( rcTextBox );
}

void TournamentMoreInfo::RANKSLOT::SetImagePosition( CSwapImage* pImage, const UIRECT& rcBase, const UIRECT& rcParent )
{
	GASSERT(pImage);

	UIRECT rcImage = pImage->GetLocalPos();
	rcImage.left   = (rcBase.left + rcBase.right)/2.0f - rcImage.sizeX/2.0f;
	rcImage.right  = rcImage.left + rcImage.sizeX;
	rcImage.top    = rcImage.top + rcParent.sizeY/2.0f - rcImage.sizeY/2.0f;
	rcImage.bottom = rcImage.top + rcImage.sizeY;

	pImage->SetLocalPos( rcImage );
}

CBasicTextBox* TournamentMoreInfo::RANKSLOT::CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, szControlKeyword, UI_FLAG_DEFAULT, cID );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );	
	RegisterControl ( pStaticText );

	return pStaticText;
}

void TournamentMoreInfo::SetPage(DWORD dwPageNum)
{

	GLTouranmentClient * Tourclient = m_pGaeaClient->GetTournamentClient();
	MatchingSystem::GLMatChingGroupInfo* _Bettingclient =	Tourclient->GetGroupInfo(dwPageNum,Tourclient->m_SelectScriptID);

	if(_Bettingclient == NULL)
		return;

	CString strMatchUp;
	strMatchUp.Format("%s(%d/%d)",_Bettingclient->GroupName.c_str(), _Bettingclient->GroupSize,_Bettingclient->GroupMaxSize);

	this->SetTitleName(strMatchUp.GetString());

	//Tourclient->m_MyBettingInfo
}


//----------------------------------------------------------------------------------------------------//

MyTournamentMoreInfoWindow::MyTournamentMoreInfoWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: TournamentMoreInfo( pGaeaClient, pInterface, pEngineDevice )
{
}


void MyTournamentMoreInfoWindow::OnCreate()
{
	TournamentMoreInfo::Create( TOURNAMENT_MORE_INFO, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	TournamentMoreInfo::CreateBaseWindowLightGray  ( "TOUR_MINFO_WINDOW", (char*)ID2GAMEWORD("COMPETITION_TOPRANKING", 1) );
	TournamentMoreInfo::CreateSubControl();
	TournamentMoreInfo::SetAlignFlag( UI_FLAG_CENTER_Y | UI_FLAG_CENTER_X );
	TournamentMoreInfo::m_pInterface->UiRegisterControl( this, true );
	TournamentMoreInfo::m_pInterface->UiShowGroupFocus( TOURNAMENT_MORE_INFO );
}

void MyTournamentMoreInfoWindow::OnVisible( bool visible )
{


	MatchingSystem::GLTouranmentClient* m_pLogic = m_pGaeaClient->GetTournamentClient();
	//m_pLogic->GetBettingInfoEployee();


	if(visible == false)
	{
		TournamentMoreInfo::SetVisibleSingle(FALSE);
		return;
	}

	UIRECT rcPos   = TournamentMoreInfo::m_pInterface->GetTournamentDashBoardWindowPos(TOURNAMENT_TOTO);
	UIRECT rcMyPos = TournamentMoreInfo::GetGlobalPos();
	
	rcPos = UIRECT(rcPos.right, rcPos.top, rcMyPos.sizeX, rcMyPos.sizeY);

	TournamentMoreInfo::SetGlobalPos( rcPos );
	//TournamentMoreInfo::SetVisibleSingle(TRUE);
	TournamentMoreInfo::m_pInterface->UiShowGroupFocus( TOURNAMENT_MORE_INFO );
	
}

void MyTournamentMoreInfoWindow::SetPage(DWORD dwPageNum)
{
	TournamentMoreInfo::SetPage(dwPageNum);
}


const UIRECT& MyTournamentMoreInfoWindow::GetGlobalPos()
{
	return TournamentMoreInfo::GetGlobalPos();
}

