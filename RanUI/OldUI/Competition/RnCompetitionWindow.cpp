#include "StdAfx.h"

#include "../../../RanLogic/GLUseFeatures.h"

#include "../../../enginelib/DeviceMan.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"

#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/TipBox.h"
#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"
#include "./RnCompetitionGrade.h"
#include "./RnCompetitionWindow.h"
#include "./RnCompetitionObject.h"
#include "./CTFPage.h"
#include "./PVEPage.h"
#include "./RnCDMPage.h"
#include "./RnCTF.h"
#include "WailBuilding.h"

#include "../../../RanLogicClient/Tournament/GLTournamentClient.h"
#include "../../../RanLogicClient/Tournament/MatchingJoinManagerClient.h"
#include "../../../RanLogicClient/GLGaeaClient.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const int RnCompetitionWindow::nMaxReportSpecAct = 18;

RnCompetitionWindow::RnCompetitionWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CUIWindowObjectController(pInterface, pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_pUIObject(NULL)
, m_pSelect(NULL)
, m_nReportScrollPos(0)
, m_nReportSelect(0)
, m_pSelectSubType(NULL)
, m_pRnCTFPage( NULL )
, m_pRnWailBuinding( NULL )
{
	memset(m_pPages, 0, sizeof(m_pPages));
}

RnCompetitionWindow::~RnCompetitionWindow()
{
	//Blank
}

void RnCompetitionWindow::CreateSubControl ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//Note : 윈도우 오브젝트
	{
		CUIWindowObject* pUIObject = new CUIWindowObject    ( m_pInterface, m_pEngineDevice );
		pUIObject->Create					    ( COMPETITION_WINDOW_OBJECT, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pUIObject->CreateBaseWindowLightGray  ( "RNCOMPETITION_OBJECT", (char*)ID2GAMEWORD("COMPETITION_WINDOW", 0) );
		pUIObject->CreateSubControl	        ( pd3dDevice );
		pUIObject->SetAlignFlag			    ( UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pUIObject->SetVisibleSingle		    ( FALSE );
		pUIObject->SetFlag                    ( CUIWindowObject::FLAG_TITLE_NAME_NONCHANGE );

		WindowRegister( pUIObject, COMPETITION_WINDOW_OBJECT );
	}

	{
		m_pUIObject = new RnCompetitionObject( m_pGaeaClient, m_pInterface, m_pEngineDevice );
		m_pUIObject->CreateSub(this, "RNCOMPETITION_OBJECT", NULL, COMPETITION_WINDOW_COMMON);
		m_pUIObject->CreateSubControl(pd3dDevice);
		RegisterControl( m_pUIObject );
		m_pUIObject->SetVisibleSingle(FALSE);
	}

	//각 인던별 입장 페이지
	{
		// CaptureTheField 점령전
		if ( GLUseFeatures::GetInstance().IsUsingCaptureTheField() )
		{
			m_pRnCTFPage = new RnCTFPage( m_pGaeaClient, m_pInterface, m_pEngineDevice );
			m_pRnCTFPage->CreateSub(this, "RNCOMPETITION_OBJECT", NULL, COMPETITION_WINDOW_PAGE_CTF);
			m_pRnCTFPage->CreateSubControl();
			RegisterControl( m_pRnCTFPage );
			m_pRnCTFPage->SetVisibleSingle(FALSE);
		}

		// 무한의 계단
		if ( GLUseFeatures::GetInstance().IsUsingInfinityStairs() )
		{
			
			m_pRnWailBuinding = new RnWailBuinding( m_pGaeaClient, m_pInterface, m_pEngineDevice );
			m_pRnWailBuinding->CreateSub(this, "RNCOMPETITION_OBJECT", NULL, COMPETITION_WINDOW_PAGE_PVE);
			m_pRnWailBuinding->CreateSubControl();
			RegisterControl( m_pRnWailBuinding );
			m_pRnWailBuinding->SetVisibleSingle(FALSE);
		}
	}

	CD3DFontPar* pFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	{
		CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
		pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE );
		pScrollBar->CreateBaseScrollBar ( "RNCOMPETITION_INSTANCE_TYPE_LIST_SCROLL" );
		pScrollBar->GetThumbFrame()->SetState ( EM_INSTANCE_TYPE_SIZE, nMaxReportSpecAct );

		CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxQuestList ( "RNCOMPETITION_INSTANCE_TYPE_LIST_BG" );
		RegisterControl ( pBasicLineBox );

		CUIControl* pScrollParent = pBasicLineBox;
		//  해당 컨트롤 위에서만 마우스 휠이 동작한다.
		pScrollBar->GetThumbFrame()->SetScrollParent( pScrollParent ); 
		RegisterControl ( pScrollBar );
		m_pReportListScroll = pScrollBar;  
	}

	m_pSelect = new CTipBox(m_pEngineDevice);
	m_pSelect->CreateSub( this, "QUEST_WINDOW_CHILD_BOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE, INSTANCE_LIST_SELECT );
	m_pSelect->CreateQuestSelect( "RNCOMPETITION_TYPE_SUB" );
	RegisterControl ( m_pSelect );
	m_pSelect->SetVisibleSingle( FALSE );

	UpdateReportPosControl();
	SetOpenWindow ( COMPETITION_WINDOW_OBJECT );
	//SetOpenPage	  ( m_pPages[nDefault] );

	if(m_pRnCTFPage)
	{
		m_pRnCTFPage->SetVisibleSingle(TRUE);

		if ( m_pUIObject )
			m_pUIObject->SetVisibleSingle(FALSE);

		if(m_pRnWailBuinding)
			m_pRnWailBuinding->SetVisibleSingle(FALSE);
	}
	else if(m_pRnWailBuinding)
	{
		m_pRnWailBuinding->SetVisibleSingle(TRUE);

		if ( m_pUIObject )
			m_pUIObject->SetVisibleSingle(FALSE);

		if(m_pRnCTFPage)
			m_pRnCTFPage->SetVisibleSingle(FALSE);
	}
	else
	{
		if ( m_pUIObject )
			m_pUIObject->SetVisibleSingle(TRUE);
	}
}

// 인던 리스트에 표시되는 내용
void RnCompetitionWindow::SetInstanceList(SINSTANCE_TYPE sInstanceType)
{
	std::string strMainType = sInstanceType.strManTypeName;
	std::string strSubType = sInstanceType.strSubTypeName;

	bool bIsUsingTypeMain = false;
	BOOST_FOREACH(RnCompetitionInstanceTypeMain* pInstanceMain, m_vecInstanceTypeMain )
	{
		if ( strMainType == pInstanceMain->m_strType )
		{
			bIsUsingTypeMain = true;
			break;
		}
	}

	if ( !bIsUsingTypeMain )
	{
		RnCompetitionInstanceTypeMain* pInstanceTypeMain = new RnCompetitionInstanceTypeMain( m_pInterface, m_pEngineDevice );
		pInstanceTypeMain->CreateSub ( this, "RNCOMPETITION_TYPE", UI_FLAG_DEFAULT, ( INSTANCE_TYPE + m_vecInstanceTypeMain.size() ) );
		pInstanceTypeMain->CreateSubControl();

        pInstanceTypeMain->SetText( ID2GAMEINTEXT( strMainType.c_str() ) );

		pInstanceTypeMain->m_strType = strMainType;
		RegisterControl (pInstanceTypeMain );
		m_vecInstanceTypeMain.push_back(pInstanceTypeMain);
		bIsUsingTypeMain = false;

        pInstanceTypeMain->InitDeviceObjects(m_pEngineDevice->GetDevice()); 
        pInstanceTypeMain->RestoreDeviceObjects(m_pEngineDevice->GetDevice());
	}

	
	bool bIsUsingTypeSub = false;
	BOOST_FOREACH(SINSTANCE_TYPE_SUB &sInstanceMain, m_vecInstanceTypeSub )
	{
		if ( sInstanceMain.sInstanceType.strSubTypeName == strSubType )
		{
			
			bIsUsingTypeSub = true;
			break;
		}
	}

	if (  !bIsUsingTypeSub )
	{
		CD3DFontPar* pFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
		CBasicTextBox* pInstanceTypeSub = CreateStaticControl ( "RNCOMPETITION_TYPE_SUB", pFont9, NS_UITEXTCOLOR::WHITE, 
			TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y, INSTANCE_TYPE_SUB + m_vecInstanceTypeSub.size() );

        pInstanceTypeSub->SetText( sc::string::format( "   %s", ID2GAMEINTEXT( strSubType.c_str() )).c_str() );
		pInstanceTypeSub->SetUseTextColor ( 0, TRUE );

		SINSTANCE_TYPE_SUB sInstanceTypeSub;
		sInstanceTypeSub.sInstanceType.strManTypeName = strMainType;
		sInstanceTypeSub.sInstanceType.strSubTypeName = strSubType;
		sInstanceTypeSub.pInstanceSubTypeName = pInstanceTypeSub;
		sInstanceTypeSub.vecInstanceNameId.push_back(sInstanceType.sInstanceNameId);
		sInstanceTypeSub.dwUIID = INSTANCE_TYPE_SUB + m_vecInstanceTypeSub.size() ;
		m_vecInstanceTypeSub.push_back(sInstanceTypeSub); 
	}	
	else
	{
		BOOST_FOREACH(SINSTANCE_TYPE_SUB &sInstanceMain, m_vecInstanceTypeSub )
		{
			if ( sInstanceMain.sInstanceType.strSubTypeName == strSubType )
			{
				bool Exsist = false;
				BOOST_FOREACH(SINSTANCE_NAME_ID &sInstanceName, sInstanceMain.vecInstanceNameId )
				{
					if(sInstanceName.dwInstanceID == sInstanceType.sInstanceNameId.dwInstanceID)
					{
						Exsist = true;
						break;
					}
				}
				if(!Exsist)
				{
					sInstanceMain.vecInstanceNameId.push_back(sInstanceType.sInstanceNameId);
				}
				break;
			}
		}
		
	}
   
}

void RnCompetitionWindow::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIWindowObjectController::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	DWORD dwID = m_pUIObject->GetSelectInstanceID();
    if ( dwID != DWORD_MAX )
        m_pUIObject->SelectContents(dwID);

	{
		CBasicScrollThumbFrame* const pThumbFrame = m_pReportListScroll->GetThumbFrame ();
		const int nTotal = pThumbFrame->GetTotal ();
		const int nViewPerPage = pThumbFrame->GetViewPerPage ();

		if ( nViewPerPage < nTotal )
		{
			int nCurPos = 0;
			const int nMovableLine = nTotal - nViewPerPage;
			float fPercent = pThumbFrame->GetPercent ();

			nCurPos = (int)floor(fPercent * nMovableLine);
			if ( nCurPos < 0 ) nCurPos = 0;

			if ( m_nReportScrollPos == nCurPos ) return;

			m_nReportScrollPos = nCurPos;

			UpdateReportPosControl();
		}
	}

}

void RnCompetitionWindow::TranslateUIMessage(UIGUID ControlID, DWORD dwMsg)
{
	CUIWindowObjectController::TranslateUIMessage(ControlID, dwMsg);

	if ( ControlID >= INSTANCE_TYPE && ControlID < INSTANCE_TYPE_SUB ) 
	{
		if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
		{
			ReCalcReportTotalLine();
			UpdateReportPosControl();
		}
	}

	if ( ControlID >= INSTANCE_TYPE_SUB && ControlID <= INSTANCE_TYPE_SUB + static_cast<int>(m_vecInstanceTypeSub.size()) ) 
	{
		if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
		{
			int nSelect = ControlID - INSTANCE_TYPE_SUB - 1;
			SINSTANCE_TYPE_SUB sSelectTypeSub;

			BOOST_FOREACH(SINSTANCE_TYPE_SUB sInstanceTypeSub, m_vecInstanceTypeSub )
			{
				sInstanceTypeSub.pInstanceSubTypeName->SetTextColor(0, NS_UITEXTCOLOR::WHITE );
				if ( sInstanceTypeSub.dwUIID == ControlID )
				{
					sSelectTypeSub = sInstanceTypeSub;
					m_pSelectSubType = sInstanceTypeSub.pInstanceSubTypeName;
				}
			}
			m_nReportSelect = nSelect;                

			m_pSelect->SetGlobalPos( m_pSelectSubType->GetGlobalPos() );
			m_pSelect->SetLocalPos( m_pSelectSubType->GetLocalPos() );
			m_pSelect->SetVisibleSingle( TRUE );

			m_pSelectSubType->SetTextColor(0, NS_UITEXTCOLOR::ORANGE );


			if(sSelectTypeSub.sInstanceType.strSubTypeName == "RNCOMPETITION_SUB_TYPE_CTF")
			{
				if ( m_pUIObject )
					m_pUIObject->SetVisibleSingle(FALSE);

				if ( m_pRnCTFPage )
					m_pRnCTFPage->SetVisibleSingle(TRUE);

				if ( m_pRnWailBuinding )
					m_pRnWailBuinding->SetVisibleSingle(FALSE);
			}
			else if(sSelectTypeSub.sInstanceType.strSubTypeName == "RNCOMPETITION_SUB_TYPE_WAIL")
			{
				if ( m_pUIObject )
					m_pUIObject->SetVisibleSingle(FALSE);

				if ( m_pRnCTFPage )
					m_pRnCTFPage->SetVisibleSingle(FALSE);

				if ( m_pRnWailBuinding )
					m_pRnWailBuinding->SetVisibleSingle(TRUE);
			}
			else
			{
				if ( m_pUIObject )
					m_pUIObject->ClearInstanceList();

				BOOL bIsMenmber = FALSE;
				BOOST_FOREACH(SINSTANCE_NAME_ID sInstanceNameId, sSelectTypeSub.vecInstanceNameId )
				{
					if ( m_pUIObject )
					{

						const DWORD dwJoinCount = MatchingSystem::CMatchingJoinManagerClient::Instance()->GetJoinCount(
							sInstanceNameId.dwInstanceID );
						DWORD dwCurJoinCount = sInstanceNameId.dwMaxJoinCount - dwJoinCount;
						if ( dwCurJoinCount < 0 )
							dwCurJoinCount = 0;

						std::string strText = ID2GAMEINTEXT( sInstanceNameId.strInstanceName.c_str() );
						if ( sInstanceNameId.dwMaxJoinCount > 0 )
						{
							strText += sc::string::format( " ( %1% )", dwCurJoinCount );
						}

						m_pUIObject->AddInstanceList(
							strText.c_str(), sInstanceNameId.dwInstanceID );



						bIsMenmber = TRUE;
					}
				}

				if ( bIsMenmber )
				{
					m_pUIObject->SetCurSel(0);
				}

				if ( m_pUIObject )
					m_pUIObject->SetVisibleSingle(TRUE);

				if ( m_pRnCTFPage )
					m_pRnCTFPage->SetVisibleSingle(FALSE);

				if ( m_pRnWailBuinding )
					m_pRnWailBuinding->SetVisibleSingle(FALSE);
			}
					
		}
	}
}

const bool RnCompetitionWindow::IsOpen()
{
	return IsVisible() ? true : false;
}

void RnCompetitionWindow::Open()
{
	if (IsVisible() == false)
	{		

		m_pInterface->UiShowGroupFocus( RNCOMPETITION_WINDOW );
		if(m_pRnCTFPage)
			m_pRnCTFPage->BeginSubControl();

        InitializeMatchingInfo();
	}
}

void RnCompetitionWindow::Close()
{
	if (IsVisible())
	{
		m_pInterface->UiHideGroup( RNCOMPETITION_WINDOW );
	}
}

void RnCompetitionWindow::Update(int nPage)
{
	if (nPage >= 0 && nPage < EM_INSTANCE_TYPE_ALL_SIZE)
	{
		//m_pPages[nPage]->Update();
	}
}

CBasicTextBox* RnCompetitionWindow::CreateStaticControl( const char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR dwColor, int nAlign, UIGUID nWndID /*= NO_ID */ )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, szControlKeyword, nAlign, nWndID );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );
	RegisterControl ( pStaticText );

	return pStaticText;
}

void RnCompetitionWindow::UpdateReportPosControl()
{
	//  현재 화면에 표시해야되는 갯수를 체크한다.

	CBasicScrollThumbFrame* const pThumbFrame = m_pReportListScroll->GetThumbFrame ();
	const int nTotal = pThumbFrame->GetTotal ();
	const int nViewPerPage = pThumbFrame->GetViewPerPage ();

	// UI 모두 숨긴다.

	BOOST_FOREACH(RnCompetitionInstanceTypeMain* pInstanceTypeMain, m_vecInstanceTypeMain)
	{
		pInstanceTypeMain->SetVisibleSingle(FALSE);
	}

	BOOST_FOREACH(SINSTANCE_TYPE_SUB sInstanceTypeSub, m_vecInstanceTypeSub)
	{
		sInstanceTypeSub.pInstanceSubTypeName->SetVisibleSingle(FALSE);
	}

	m_pSelect->SetVisibleSingle( FALSE );

	//  화면에 보여질 UI를 리스트를 만든다.
	std::vector<CUIControl*> vecControl;


	for ( unsigned i = 0; i < m_vecInstanceTypeMain.size(); ++i ) 
	{
		vecControl.push_back( (CUIControl*) m_vecInstanceTypeMain[i] );

		if( m_vecInstanceTypeMain[i]->IsOpen() )
		{
			for( unsigned j = 0; j < m_vecInstanceTypeSub.size(); ++j  )
			{
				if (m_vecInstanceTypeMain[i]->m_strType == m_vecInstanceTypeSub[j].sInstanceType.strManTypeName )
					vecControl.push_back( (CUIControl*) m_vecInstanceTypeSub[j].pInstanceSubTypeName );
			}
		}
	}

	if (m_vecInstanceTypeMain.size() < 1)
		return;

	const UIRECT& rcGlobalPos = m_vecInstanceTypeMain[0]->GetGlobalPos();
	const UIRECT& rcLocalPos = m_vecInstanceTypeMain[0]->GetLocalPos();

	// 스크롤바를 계산하여 ResizeControl 한다.
	for ( int i = 0; i < (int)vecControl.size(); ++i ) 
	{
		if ( i < m_nReportScrollPos ) continue;
		if ( i >= m_nReportScrollPos + nViewPerPage ) break;

		int nCount = i - m_nReportScrollPos;

		// resize
		CUIControl* pControl = vecControl[i];

		UIRECT rcGlobalPosNew = rcGlobalPos;
		rcGlobalPosNew.top = rcGlobalPosNew.top + ( (rcGlobalPosNew.sizeY + 2) * nCount );

		UIRECT rcLocalPosNew = rcLocalPos;
		rcLocalPosNew.top = rcLocalPosNew.top + ( (rcLocalPosNew.sizeY + 2) * nCount );

		pControl->SetGlobalPos( rcGlobalPosNew );
		pControl->SetLocalPos( rcLocalPosNew );
		pControl->SetVisibleSingle( TRUE );
	}   

	BOOST_FOREACH(SINSTANCE_TYPE_SUB sInstanceTypeSub, m_vecInstanceTypeSub )
	{
		sInstanceTypeSub.pInstanceSubTypeName->SetTextColor(0, NS_UITEXTCOLOR::WHITE );
	}

	if ( m_pSelectSubType && m_pSelectSubType->IsVisible() ) 
	{
		m_pSelectSubType->SetTextColor(0, NS_UITEXTCOLOR::ORANGE );
		m_pSelect->SetGlobalPos( m_pSelectSubType->GetGlobalPos() );
		m_pSelect->SetLocalPos( m_pSelectSubType->GetLocalPos() );
		m_pSelect->SetVisibleSingle( TRUE );
	}

	return;
}

void RnCompetitionWindow::ReCalcReportTotalLine()
{
	int nTotalLine = 0;

	for ( unsigned i = 0; i < m_vecInstanceTypeMain.size(); ++i ) 
	{
		nTotalLine++;

		if ( m_vecInstanceTypeMain[i]->IsOpen() ) 
		{
			BOOST_FOREACH(SINSTANCE_TYPE_SUB sInstanceTypeSub, m_vecInstanceTypeSub)
			{
				if (m_vecInstanceTypeMain[i]->m_strType == sInstanceTypeSub.sInstanceType.strManTypeName)
					nTotalLine++;
			}
		}
	}

	CBasicScrollThumbFrame* const pThumbFrame = m_pReportListScroll->GetThumbFrame ();

	pThumbFrame->SetState( nTotalLine, nMaxReportSpecAct );

	const int nTotal = pThumbFrame->GetTotal ();
	const int nViewPerPage = pThumbFrame->GetViewPerPage ();

	int nCurPos = 0;
	const int nMovableLine = nTotal - nViewPerPage;

	if ( nMovableLine < 0 )
	{
		m_nReportScrollPos = 0;
		return;
	}

	if ( m_nReportScrollPos > nMovableLine ) 
	{
		m_nReportScrollPos = nMovableLine;
	}

	float fPercent = (float)m_nReportScrollPos / (float)nMovableLine;
	pThumbFrame->SetPercent (fPercent);
}

void RnCompetitionWindow::InitializeMatchingInfo()
{
    MatchingSystem::GLTouranmentClient* pTournament = m_pGaeaClient->GetTournamentClient();


	//m_vecInstanceTypeSub.clear();
    // 점령전 예외처리;
    SINSTANCE_TYPE sInstanceType;
    sInstanceType.strManTypeName = "RNCOMPETITION_MAIN_TYPE_PVP";
    sInstanceType.strSubTypeName = "RNCOMPETITION_SUB_TYPE_CTF";
    SetInstanceList(sInstanceType);

    // 통곡의 빌딩 예외처리;
    sInstanceType.strManTypeName = "RNCOMPETITION_MAIN_TYPE_PVE";
    sInstanceType.strSubTypeName = "RNCOMPETITION_SUB_TYPE_WAIL";
    SetInstanceList(sInstanceType);

    const unsigned int numInfo = pTournament->m_VecMatchingInfo.size();
    for ( unsigned int i = 0; i < numInfo; i++ )
    {
        const MatchingSystem::GLMatchingInfo* pInfo = pTournament->m_VecMatchingInfo[i];
        SINSTANCE_TYPE sInstanceType;
        sInstanceType.strManTypeName = pInfo->m_strContentsMainType;
        sInstanceType.strSubTypeName = pInfo->m_strContentsSubType;
        sInstanceType.sInstanceNameId.strInstanceName = pInfo->m_strContentsName;
        sInstanceType.sInstanceNameId.dwInstanceID = pInfo->m_ScriptID;
		sInstanceType.sInstanceNameId.dwMaxJoinCount = pInfo->m_dwMaxJoinCount;
        SetInstanceList(sInstanceType);
    }

    UpdateReportPosControl();

    InitDeviceObjects(m_pEngineDevice->GetDevice());
    RestoreDeviceObjects(m_pEngineDevice->GetDevice());
}

void RnCompetitionWindow::SetCDMMapinfo( std::string strMapName, 
												 int nTimeLimit, 
												 int nPlayerLimite, 
												 int nLevelLimit )
{
	if (!m_pPages[EM_CLUB_DEATH_MATH])
		return;
	dynamic_cast<RnCDMPage*>(m_pPages[EM_CLUB_DEATH_MATH])->
		SetMapinfo( strMapName, nTimeLimit, nPlayerLimite, nLevelLimit );
	//m_pPages[EM_CLUB_DEATH_MATH]->Update();
}

void RnCompetitionWindow::SetCDMMapImageName( std::string strMapImageName )
{
	if (!m_pPages[EM_CLUB_DEATH_MATH])
		return;
	dynamic_cast<RnCDMPage*>(m_pPages[EM_CLUB_DEATH_MATH])->
		SetMapImageName( strMapImageName );
	//m_pPages[EM_CLUB_DEATH_MATH]->Update();
}

void RnCompetitionWindow::SetCDMBattelState( int nStartTime, int nEndTime )
{
	if (!m_pPages[EM_CLUB_DEATH_MATH])
		return;
	dynamic_cast<RnCDMPage*>(m_pPages[EM_CLUB_DEATH_MATH])->
		BattelState( nStartTime, nEndTime );
	//m_pPages[EM_CLUB_DEATH_MATH]->Update();
}

void RnCompetitionWindow::UpDateWdilBuinding()
{
	if ( m_pRnWailBuinding )
		m_pRnWailBuinding->BeginSubControl();
}


//----------------------------------------------------------------------------------------------------//

MyRnCompetitionWindow::MyRnCompetitionWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: RnCompetitionWindow( pGaeaClient, pInterface, pEngineDevice )
{
}

void MyRnCompetitionWindow::CreateUIWindowAndRegisterOwnership( LPDIRECT3DDEVICEQ pd3dDevice )
{
	RnCompetitionWindow::Create( RNCOMPETITION_WINDOW, "RNCOMPETITION_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	RnCompetitionWindow::CreateSubControl( pd3dDevice );
	RnCompetitionWindow::SetAlignFlag( UI_FLAG_CENTER_Y | UI_FLAG_CENTER_X );
	RnCompetitionWindow::m_pInterface->UiRegisterControl( this, true );
	RnCompetitionWindow::m_pInterface->UiShowGroupFocus( RNCOMPETITION_WINDOW );
}

const bool MyRnCompetitionWindow::IsOpen()
{
	return RnCompetitionWindow::IsOpen();
}

void MyRnCompetitionWindow::Update(int nPage)
{
	RnCompetitionWindow::Update(nPage);
}

void MyRnCompetitionWindow::Open()
{
	RnCompetitionWindow::Open();
}

void MyRnCompetitionWindow::Close()
{
	RnCompetitionWindow::Close();
}

const UIRECT& MyRnCompetitionWindow::GetGlobalPos()
{
	return RnCompetitionWindow::GetGlobalPos();
}

void MyRnCompetitionWindow::SetCDMMapinfo( std::string strMapName, int nTimeLimit, int nPlayerLimite, int nLevelLimit )
{
	RnCompetitionWindow::SetCDMMapinfo( strMapName, nTimeLimit, nPlayerLimite, nLevelLimit );
}

void MyRnCompetitionWindow::SetCDMMapImageName( std::string strMapImageName )
{
	RnCompetitionWindow::SetCDMMapImageName( strMapImageName );
}

void MyRnCompetitionWindow::SetCDMBattelState( int nStartTime, int nEndTime )
{
	RnCompetitionWindow::SetCDMBattelState( nStartTime, nEndTime );
}

void MyRnCompetitionWindow::UpDateWdilBuinding()
{
	RnCompetitionWindow::UpDateWdilBuinding();
}

