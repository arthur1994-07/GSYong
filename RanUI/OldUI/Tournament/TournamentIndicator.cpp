#include "StdAfx.h"


#include "../../../RanLogic/Tournament/GLTournamentClientInfo.h"
#include "../../../RanLogic/Land/FactionManager/FactionManagerDefine.h"
#include "../../../RanLogic/Land/FactionManager/FactionManager.h"

#include "../../../RanLogicClient/Tournament/GLTournamentClient.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"


#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicProgressBar.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../RanLogicClient/Char/GLCharClient.h"
#include "../../../RanLogicClient/Land/GLLandManClient.h"

#include "../Competition/PVEIndicator.h"
#include "../Competition/UINumberTimer.h"
#include "../Competition/UIStageIndicator.h"
#include "TournamentIndicator.h"
#include "TournamentExpeditionInfoGroup.h"
#include "TournamentMiniExpeditionWindow.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CTournamentIndicator::CTournamentIndicator(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice) :
CPVEIndicator(pEngineDevice)
,	m_pGaeaClient(pGaeaClient)
,	m_pInterface(pInterface)
,	m_pLeftHP(0)
,	m_pRightHP(0)
,	pLeftIMAGECLASS(0)
,	pRightIMAGECLASS(0)
,	pRightIMAGESCHOOL(0)
,	pLeftIMAGESCHOOL(0)
,	pLftBOX(0)
,	pRightBOX(0)
,	pLftBOXText(0)
,	pRightBOXText(0)
,	m_bNotFound(true)
,   pLeftTeamIcon(0)
,   pRightTeamIcon(0)
,   pLeftTeamImage(0)
,   pRightTeamImage(0)
,   pTeamObeject(0)
,   fBlinkTimer(0.0f)
,   pTeamVsTeamWindowBox(0)
{
	
}

CTournamentIndicator::~CTournamentIndicator()
{

}

bool CTournamentIndicator::CreateSubControl()
{

	CD3DFontPar* pFont_Shadow = m_pEngineDevice->LoadFont(_DEFAULT_FONT, 9 , _DEFAULT_FONT_SHADOW_FLAG);
	CD3DFontPar* pFontBIG_Shadow = m_pEngineDevice->LoadFont(_DEFAULT_FONT, 12 , _DEFAULT_FONT_SHADOW_FLAG);

	const int nAlignLeftTop     = TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP;	
	const int nAlignLeftCenter  = TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y;	
	const int nAlignRightCenter = TEXT_ALIGN_RIGHT | TEXT_ALIGN_CENTER_Y;	
	const int nAlignRightTop    = TEXT_ALIGN_RIGHT | TEXT_ALIGN_TOP;	
	const int nAlignCenter	    = TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y;	



	if (m_pTimerIndicator == 0)
	{
		m_pTimerIndicator = new CUINumberTimer(m_pEngineDevice);

		if (m_pTimerIndicator == 0)
		{
			return false;
		}

		RegisterControl(m_pTimerIndicator);
		m_pTimerIndicator->CreateSub(this, _T("TOURNAMENT_CONTROL_INDICATOR_TIMER"), UI_FLAG_CENTER_X | UI_FLAG_CENTER_Y);
		m_pTimerIndicator->CreateSubControl();
		m_pTimerIndicator->SetTimerImage(false);
		m_pTimerIndicator->SetVisibleSingle(TRUE);
	}

	if ( m_pLeftHP == 0 )
	{
		CBasicProgressBar*	pProgressBar = new CBasicProgressBar(m_pEngineDevice);
		pProgressBar->CreateSub ( this, "HP_HUD_LEFT_SLOT_HP", UI_FLAG_DEFAULT, HP_BAR_LEFT );
		pProgressBar->CreateOverImage ( "HP_HUD_LEFT_SLOT_HP_OVERIMAGE" );
		pProgressBar->SetPercent ( 1.0f );
		pProgressBar->SetVisibleSingle( FALSE );
		RegisterControl ( pProgressBar );
		m_pLeftHP = pProgressBar;
	}

	if ( m_pRightHP == 0 )
	{
		CBasicProgressBar*	pProgressBar = new CBasicProgressBar(m_pEngineDevice);
		pProgressBar->CreateSub ( this, "HP_HUD_RIGHT_SLOT_HP", UI_FLAG_DEFAULT, HP_BAR_RIGHT );
		pProgressBar->CreateOverImage ( "HP_HUD_RIGHT_SLOT_HP_OVERIMAGE" );
		pProgressBar->SetPercent ( 1.0f );
		pProgressBar->SetVisibleSingle( FALSE );
		//pProgressBar->SetType(CBasicProgressBar::HORIZONTAL_REVERS);
		RegisterControl ( pProgressBar );
		m_pRightHP = pProgressBar;
	}

	


	if ( pLeftIMAGECLASS == 0 )
	{
		pLeftIMAGECLASS = new CSwapImage(m_pEngineDevice);
		pLeftIMAGECLASS->CreateSub ( this, "TOURNAMENT_RIGHT_SLOT_IMAGE_BASE_CLASS", UI_FLAG_DEFAULT );
		pLeftIMAGECLASS->SetImageClass(1);
		RegisterControl ( pLeftIMAGECLASS );

	}
	if ( pRightIMAGECLASS == 0 )
	{
		pRightIMAGECLASS = new CSwapImage(m_pEngineDevice);
		pRightIMAGECLASS->CreateSub ( this, "TOURNAMENT_LEFT_SLOT_IMAGE_BASE_CLASS", UI_FLAG_DEFAULT );
		pRightIMAGECLASS->SetImageClass(1);
		RegisterControl ( pRightIMAGECLASS );
	}


	if( pLeftIMAGESCHOOL == 0)
	{
		pLeftIMAGESCHOOL = new CSwapImage(m_pEngineDevice);
		pLeftIMAGESCHOOL->CreateSub ( this, "TOURNAMENT_LEFT_IMAGE_BASE_SCHOOL", UI_FLAG_DEFAULT );
		pLeftIMAGESCHOOL->SetImage( "CTF_RESULT_RANKSLOT_IMAGE_SCHOOL0" );
		RegisterControl ( pLeftIMAGESCHOOL );
	}
	
	if( pRightIMAGESCHOOL == 0)
	{
		pRightIMAGESCHOOL = new CSwapImage(m_pEngineDevice);
		pRightIMAGESCHOOL->CreateSub ( this, "TOURNAMENT_RIGHT_IMAGE_BASE_SCHOOL", UI_FLAG_DEFAULT );
		pRightIMAGESCHOOL->SetImage( "CTF_RESULT_RANKSLOT_IMAGE_SCHOOL0" );
		RegisterControl ( pRightIMAGESCHOOL );
	}

	for(int i=0; i<5;i++)
	{
		CString strTemp;
		strTemp.Format("HP_HUD_LEFT_SLOT_WINCOUNT_%d", i );
		m_pTournamentLeft[i] = new CSwapImage(m_pEngineDevice);
		m_pTournamentLeft[i]->CreateSub ( this, strTemp.GetString(), UI_FLAG_DEFAULT);
		m_pTournamentLeft[i]->SetImage( "HP_HUD_LEFT_SLOT_WINCOUNT" );
		m_pTournamentLeft[i]->SetVisibleSingle( FALSE );
		RegisterControl(m_pTournamentLeft[i]);
	}
	for(int i=0; i<5;i++)
	{
		CString strTemp;
		strTemp.Format("HP_HUD_RIGHT_SLOT_WINCOUNT_%d", i );
		m_pTournamentRight[i] = new CSwapImage(m_pEngineDevice);
		m_pTournamentRight[i]->CreateSub ( this, strTemp.GetString(),UI_FLAG_DEFAULT);
		m_pTournamentRight[i]->SetImage( "HP_HUD_LEFT_SLOT_WINCOUNT" );

		m_pTournamentRight[i]->SetVisibleSingle( FALSE );
		RegisterControl(m_pTournamentRight[i]);
	}



	if( pLftBOX == 0 )
	{
		pLftBOX = CreateLineBox("TOURNAMENT_LEFT_NAME", "GRAY_BACKGROUND_BLACK_LINE_TEXTURE");
	}

	if( pRightBOX == 0 )
	{
		pRightBOX = CreateLineBox("TOURNAMENT_RIGHT_NAME", "GRAY_BACKGROUND_BLACK_LINE_TEXTURE");
	}

	if( pLftBOXText == 0 )
	{
		pLftBOXText = CreateStaticControl("TOURNAMENT_LEFT_NAME", pFont_Shadow, nAlignLeftCenter,SINGLE_LEFT_ID_TEXT_BOX);
		pLftBOXText->SetOneLineText("TESTOBJ_LEFT", NS_UITEXTCOLOR::WHITE);
	}

	if( pRightBOXText == 0 )
	{
		pRightBOXText = CreateStaticControl("TOURNAMENT_RIGHT_NAME", pFont_Shadow, nAlignRightCenter,SINGLE_RIGHT_ID_TEXT_BOX);
		
		pRightBOXText->SetOneLineText("TESTOBJ_RIGHT", NS_UITEXTCOLOR::WHITE);
	}

	
	TOURNAMENT_LEFT_TEAMNAME_BOX = CreateLineBox("TOURNAMENT_LEFT_TEAMNAME_BOX", "POINT_SHOP_UP_TEXT_BOX_BACK_LINE");
	TOURNAMENT_LEFT_TEAMNAME_BOX_BG = CreateLineBox("TOURNAMENT_LEFT_TEAMNAME_BOX_BG", "RNCDMPROGRESS_BG_LINE");
	TOURNAMENT_LEFT_TEAMNAME_BOX_DOWN = CreateLineBox("TOURNAMENT_LEFT_TEAMNAME_BOX_DOWN", "GRAY_BACKGROUND_BLACK_LINE_TEXTURE");
	//
	//TOURNAMENT_RIGHT_TEAMNAME_BOX_UP

	TOURNAMENT_RIGHT_TEAMNAME_BOX = CreateLineBox("TOURNAMENT_RIGHT_TEAMNAME_BOX", "POINT_SHOP_UP_TEXT_BOX_BACK_LINE");
	TOURNAMENT_RIGHT_TEAMNAME_BOX_BG = CreateLineBox("TOURNAMENT_RIGHT_TEAMNAME_BOX_BG", "RNCDMPROGRESS_BG_LINE");
	TOURNAMENT_RIGHT_TEAMNAME_BOX_DOWN = CreateLineBox("TOURNAMENT_RIGHT_TEAMNAME_BOX_DOWN", "GRAY_BACKGROUND_BLACK_LINE_TEXTURE");

	TOURNAMENT_LEFT_TEAMNAME_BOX_UP_TB = CreateStaticControl("TOURNAMENT_LEFT_TEAMNAME_BOX_UP", pFont_Shadow, nAlignCenter,TEAM_LEFT_UP_ID_TEXT_BOX);
	TOURNAMENT_LEFT_TEAMNAME_BOX_UP_TB->SetOneLineText("TESTOBJ_RIGHT", NS_UITEXTCOLOR::WHITE);
	TOURNAMENT_LEFT_TEAMNAME_BOX_DOWN_TB = CreateStaticControl("TOURNAMENT_LEFT_TEAMNAME_BOX_DOWN", pFontBIG_Shadow, nAlignCenter,TEAM_LEFT_DWON_ID_TEXT_BOX);
	TOURNAMENT_LEFT_TEAMNAME_BOX_DOWN_TB->SetOneLineText("20", NS_UITEXTCOLOR::WHITE);

	TOURNAMENT_RIGHT_TEAMNAME_BOX_UP_TB = CreateStaticControl("TOURNAMENT_RIGHT_TEAMNAME_BOX_UP", pFont_Shadow, nAlignCenter,TEAM_RIGHT_UP_ID_TEXT_BOX);
	TOURNAMENT_RIGHT_TEAMNAME_BOX_UP_TB->SetOneLineText("TESTOBJ_RIGHT", NS_UITEXTCOLOR::WHITE);
	TOURNAMENT_RIGHT_TEAMNAME_BOX_DOWN_TB = CreateStaticControl("TOURNAMENT_RIGHT_TEAMNAME_BOX_DOWN", pFontBIG_Shadow, nAlignCenter,TEAM_RIGHT_DWON_ID_TEXT_BOX);
	TOURNAMENT_RIGHT_TEAMNAME_BOX_DOWN_TB->SetOneLineText("20", NS_UITEXTCOLOR::WHITE);

    if( pLeftTeamIcon == 0)
    {
        pLeftTeamIcon = new CSwapImage(m_pEngineDevice);
        pLeftTeamIcon->CreateSub ( this, "TOURNAMENT_LEFT_TEAM_ICON", UI_FLAG_DEFAULT );
        //pLeftTeamIcon->SetImage( "CTF_RESULT_RANKSLOT_IMAGE_SCHOOL0" );
        RegisterControl ( pLeftTeamIcon );
    }

    if( pRightTeamIcon == 0)
    {
        pRightTeamIcon = new CSwapImage(m_pEngineDevice);
        pRightTeamIcon->CreateSub ( this, "TOURNAMENT_RIGHT_TEAM_ICON", UI_FLAG_DEFAULT );
        //pRightTeamIcon->SetImage( "CTF_RESULT_RANKSLOT_IMAGE_SCHOOL0" );
        RegisterControl ( pRightTeamIcon );
    }

    if( pLeftTeamImage == 0)
    {
        pLeftTeamImage = new CSwapImage(m_pEngineDevice);
        pLeftTeamImage->CreateSub ( this, "TOURNAMENT_LEFT_TEAM_IMAGE", UI_FLAG_DEFAULT );
        //pLeftTeamIcon->SetImage( "CTF_RESULT_RANKSLOT_IMAGE_SCHOOL0" );
        RegisterControl ( pLeftTeamImage );
    }

    if( pRightTeamImage == 0)
    {
        pRightTeamImage = new CSwapImage(m_pEngineDevice);
        pRightTeamImage->CreateSub ( this, "TOURNAMENT_RIGHT_TEAM_IMAGE", UI_FLAG_DEFAULT );
        //pRightTeamIcon->SetImage( "CTF_RESULT_RANKSLOT_IMAGE_SCHOOL0" );
        RegisterControl ( pRightTeamImage );
    }

    if ( pTeamVsTeamWindowBox == 0 )   
    {
        pTeamVsTeamWindowBox = new CUIGroup(m_pEngineDevice);
        pTeamVsTeamWindowBox->CreateSub ( this, "TEAM_VS_TEAM_INDICATOR_WINDOW", UI_FLAG_DEFAULT );
        RegisterControl ( pTeamVsTeamWindowBox );
    }

    if ( pTeamObeject == 0 )
    {
        pTeamObeject = CreateStaticControl("TOURNAMENT_TEAM_OBJECT_TEXT", pFont_Shadow, nAlignLeftCenter);
    }

	SetTime(0,false,false);
	return true;
}



CBasicLineBoxSmart* CTournamentIndicator::CreateLineBox ( const std::string& strControlName, const std::string& strTexInfo, const UIGUID cID )
{
	CBasicLineBoxSmart* pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
	pLineBox->CreateSub( this, strControlName.c_str(), UI_FLAG_DEFAULT, cID );
	pLineBox->CreateSubControl( strTexInfo.c_str() );
	RegisterControl( pLineBox );
	return pLineBox;
}

CBasicTextBox* CTournamentIndicator::CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID cID )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, szControlKeyword, UI_FLAG_DEFAULT, cID );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );	
	RegisterControl ( pStaticText );

	return pStaticText;
}


int	CTournamentIndicator::GetTime() 
{ 
	return m_pTimerIndicator->GetTime(); 
}

void CTournamentIndicator::SetOneByOne()
{
	MatchingSystem::GLTouranmentClient* glTour = m_pGaeaClient->GetTournamentClient();

	SetPlayerCount(1,FALSE);
	SetPlayerCount(2,FALSE);
	SetPlayerCount(3,FALSE);

	SetPlayerCount(2,TRUE);

	

	if(glTour->m_dw1FactionScroe < 5 && glTour->m_dw1FactionScroe >= 0)
	{
		for(unsigned int i = 0 ; i < 5 ; i ++)
		{
			if( i < glTour->m_dw1FactionScroe)
				m_pTournamentRight[i]->SetVisibleSingle(TRUE);
			else
				m_pTournamentRight[i]->SetVisibleSingle(FALSE);
		}
	}

	if(glTour->m_dw2FactionScroe < 5 && glTour->m_dw2FactionScroe >= 0)
	{
		for(unsigned int i = 0 ; i < 5 ; i ++)
		{
			if( i < glTour->m_dw2FactionScroe)
				m_pTournamentLeft[i]->SetVisibleSingle(TRUE);
			else
				m_pTournamentLeft[i]->SetVisibleSingle(FALSE);
		}
	}


	{
		Faction::FactionID _FactionIdInstance(Faction::EMFACTION_TEAM,glTour->m_dw1FactionID);
		const Faction::FactionMemberVector* FactionMember = 
			m_pGaeaClient->GetLandManClient()->GetFactionManager()->getFactionMembers(_FactionIdInstance);

		if(FactionMember == NULL)
		{
			if(pLeftIMAGECLASS)
				pLeftIMAGECLASS->SetVisibleSingle(FALSE);
			if(pLeftIMAGESCHOOL)
				pLeftIMAGESCHOOL->SetVisibleSingle(FALSE);
			if(m_pLeftHP)
				m_pLeftHP->SetVisibleSingle(FALSE);
			if(pLftBOXText)
				pLftBOXText->SetVisibleSingle(FALSE);
			if(pLftBOX)
				pLftBOX->SetVisibleSingle(FALSE);
			return;
		}
		else
		{

			if(pLeftIMAGECLASS)
				pLeftIMAGECLASS->SetVisibleSingle(TRUE);
			if(pLeftIMAGESCHOOL)
				pLeftIMAGESCHOOL->SetVisibleSingle(TRUE);
			if(m_pLeftHP)
				m_pLeftHP->SetVisibleSingle(TRUE);
			if(pLftBOXText)
				pLftBOXText->SetVisibleSingle(TRUE);
			if(pLftBOX)
				pLftBOX->SetVisibleSingle(TRUE);


		}

		//1번 캐릭터 모양을 정하자.
		BOOST_FOREACH( Faction::GLActorID actorid, *FactionMember )
		{
			if(actorid.actorType == CROW_PC)
			{
				if( m_pGaeaClient->GetCharacter()->CharDbNum() == actorid.actorID_Num)
				{
					GLCharacter* spChar = m_pGaeaClient->GetCharacter();

					if(spChar == NULL)
						break;

					//GLCharClient* spChar;
					LeftDBID = actorid.actorID_Num;

					pLeftIMAGECLASS->SetImageClass(CharClassToIndex(spChar->m_emClass));
					pLeftIMAGESCHOOL->SetImageSchool(spChar->GetCharSchool());
					const int nNow(spChar->GetHp().nNow);
					const int nMax(spChar->GetHp().nMax);

					LeftIdPersent = float(nNow) / float(nMax);
					m_pLeftHP->SetPercent ( LeftIdPersent );


					pLftBOXText->SetText(spChar->GetName(), NS_UITEXTCOLOR::WHITE);

				}
				else
				{
					std::tr1::shared_ptr<GLCharClient> spChar = m_pGaeaClient->GetCharByDBnum(actorid.actorID_Num);

					if(spChar != NULL)
					{
						//GLCharClient* spChar;
						LeftDBID = actorid.actorID_Num;

						pLeftIMAGECLASS->SetImageClass(CharClassToIndex(spChar->GetClass()));
						pLeftIMAGESCHOOL->SetImageSchool(spChar->GetCharSchool());
						const int nNow(spChar->GetHp().nNow);
						const int nMax(spChar->GetHp().nMax);

						LeftIdPersent = float(nNow) / float(nMax);

						if(spChar->IsDie() == true)
							m_pLeftHP->SetPercent ( 0 );
						else
							m_pLeftHP->SetPercent ( LeftIdPersent);



						pLftBOXText->SetText(spChar->GetName(), NS_UITEXTCOLOR::WHITE);
					}
					else
					{
						if(pLeftIMAGECLASS)
							pLeftIMAGECLASS->SetVisibleSingle(FALSE);
						if(pLeftIMAGESCHOOL)
							pLeftIMAGESCHOOL->SetVisibleSingle(FALSE);
						if(m_pLeftHP)
							m_pLeftHP->SetVisibleSingle(FALSE);
						if(pLftBOXText)
							pLftBOXText->SetVisibleSingle(FALSE);
						if(pLftBOX)
							pLftBOX->SetVisibleSingle(FALSE);
					}


				}
				
				break;
			}
		}
	}

	
	{
		Faction::FactionID _FactionIdInstance(Faction::EMFACTION_TEAM,glTour->m_dw2FactionID);
		const Faction::FactionMemberVector* FactionMember = 
			m_pGaeaClient->GetLandManClient()->GetFactionManager()->getFactionMembers(_FactionIdInstance);

		if(FactionMember == NULL || FactionMember->size() == 0)
		{
			if(pRightIMAGECLASS)
				pRightIMAGECLASS->SetVisibleSingle(FALSE);
			if(pRightIMAGESCHOOL)
				pRightIMAGESCHOOL->SetVisibleSingle(FALSE);
			if(m_pRightHP)
				m_pRightHP->SetVisibleSingle(FALSE);
			if(pRightBOXText)
				pRightBOXText->SetVisibleSingle(FALSE);
			if(pRightBOX)
				pRightBOX->SetVisibleSingle(FALSE);
			return;
		}
		else
		{
			if(pRightIMAGECLASS)
				pRightIMAGECLASS->SetVisibleSingle(TRUE);
			if(pRightIMAGESCHOOL)
				pRightIMAGESCHOOL->SetVisibleSingle(TRUE);
			if(m_pRightHP)
				m_pRightHP->SetVisibleSingle(TRUE);
			if(pRightBOXText)
				pRightBOXText->SetVisibleSingle(TRUE);
			if(pRightBOX)
				pRightBOX->SetVisibleSingle(TRUE);
		}
			

		//2번 캐릭터 모양을 정하자.
		BOOST_FOREACH( Faction::GLActorID actorid, *FactionMember )
		{
			if(actorid.actorType == CROW_PC)
			{

				if( m_pGaeaClient->GetCharacter()->CharDbNum() == actorid.actorID_Num)
				{
					GLCharacter* spChar = m_pGaeaClient->GetCharacter();

					if(spChar == NULL)
						break;

					RightDBID = actorid.actorID_Num;

					pRightIMAGECLASS->SetImageClass(CharClassToIndex(spChar->m_emClass));
					pRightIMAGESCHOOL->SetImageSchool(spChar->m_wSchool);

					const int nNow(spChar->GetHp().nNow);
					const int nMax(spChar->GetHp().nMax);

					RightIdPersent= float(nNow) / float(nMax);
					m_pRightHP->SetPercent ( RightIdPersent);


					pRightBOXText->SetText(spChar->GetName(), NS_UITEXTCOLOR::WHITE);

				}
				else
				{
					std::tr1::shared_ptr<GLCharClient> spChar = m_pGaeaClient->GetCharByDBnum(actorid.actorID_Num);
					if(spChar != NULL)
					{
						RightDBID = actorid.actorID_Num;

						pRightIMAGECLASS->SetImageClass(CharClassToIndex(spChar->GetClass()));
						pRightIMAGESCHOOL->SetImageSchool(spChar->m_wSchool);

						const int nNow(spChar->GetHp().nNow);
						const int nMax(spChar->GetHp().nMax);

						RightIdPersent= float(nNow) / float(nMax);

						if(spChar->IsDie() == true)
							m_pRightHP->SetPercent ( 0 );
						else
							m_pRightHP->SetPercent ( RightIdPersent);


						pRightBOXText->SetText(spChar->GetName(), NS_UITEXTCOLOR::WHITE);
					}
					else
					{
						if(pRightIMAGECLASS)
							pRightIMAGECLASS->SetVisibleSingle(FALSE);
						if(pRightIMAGESCHOOL)
							pRightIMAGESCHOOL->SetVisibleSingle(FALSE);
						if(m_pRightHP)
							m_pRightHP->SetVisibleSingle(FALSE);
						if(pRightBOXText)
							pRightBOXText->SetVisibleSingle(FALSE);
						if(pRightBOX)
							pRightBOX->SetVisibleSingle(FALSE);
					}

					
				}
				
				
				break;
			}
		}
	}

	m_bNotFound = false;

}
void CTournamentIndicator::SetNbyN()
{
	SetPlayerCount(1,FALSE);
	SetPlayerCount(2,FALSE);
	SetPlayerCount(3,FALSE);

	SetPlayerCount(3,TRUE);

	MatchingSystem::GLTouranmentClient* glTour = m_pGaeaClient->GetTournamentClient();

	CString strTemp1;
	strTemp1.Format("%d", glTour->m_dw2FactionScroe );

	TOURNAMENT_LEFT_TEAMNAME_BOX_DOWN_TB->SetOneLineText(strTemp1.GetString(), NS_UITEXTCOLOR::WHITE);

	CString strTemp2;
	strTemp2.Format("%d", glTour->m_dw1FactionScroe );
	TOURNAMENT_RIGHT_TEAMNAME_BOX_DOWN_TB->SetOneLineText(strTemp2.GetString(), NS_UITEXTCOLOR::WHITE);

	{
		Faction::FactionID _FactionIdInstance(Faction::EMFACTION_TEAM,glTour->m_dw1FactionID);
		const Faction::FactionMemberVector* FactionMember = 
			m_pGaeaClient->GetLandManClient()->GetFactionManager()->getFactionMembers(_FactionIdInstance);

		if(FactionMember == NULL)
			return;

		//1번 캐릭터 모양을 정하자.
		BOOST_FOREACH( Faction::GLActorID actorid, *FactionMember )
		{
			if(actorid.actorType == CROW_PC)
			{
				if( m_pGaeaClient->GetCharacter()->CharDbNum() == actorid.actorID_Num)
				{
					GLCharacter* spChar = m_pGaeaClient->GetCharacter();

					if(spChar == NULL)
						break;


					TOURNAMENT_LEFT_TEAMNAME_BOX_UP_TB->SetOneLineText(spChar->GetName(), NS_UITEXTCOLOR::WHITE);
				}
				else
				{
					std::tr1::shared_ptr<GLCharClient> spChar = m_pGaeaClient->GetCharByDBnum(actorid.actorID_Num);

					if(spChar == NULL)
						break;


					TOURNAMENT_LEFT_TEAMNAME_BOX_UP_TB->SetOneLineText(spChar->GetName(), NS_UITEXTCOLOR::WHITE);
				}
				

				break;
			}
		}
	}


	{
		Faction::FactionID _FactionIdInstance(Faction::EMFACTION_TEAM,glTour->m_dw2FactionID);
		const Faction::FactionMemberVector* FactionMember = 
			m_pGaeaClient->GetLandManClient()->GetFactionManager()->getFactionMembers(_FactionIdInstance);

		if(FactionMember == NULL)
			return;

		//2번 캐릭터 모양을 정하자.
		BOOST_FOREACH( Faction::GLActorID actorid, *FactionMember )
		{
			if(actorid.actorType == CROW_PC)
			{
				if( m_pGaeaClient->GetCharacter()->CharDbNum() == actorid.actorID_Num)
				{
					GLCharacter* spChar = m_pGaeaClient->GetCharacter();
					if(spChar == NULL)
						break;

					TOURNAMENT_RIGHT_TEAMNAME_BOX_UP_TB->SetOneLineText(spChar->GetName(), NS_UITEXTCOLOR::WHITE);
				}
				else
				{
					std::tr1::shared_ptr<GLCharClient> spChar = m_pGaeaClient->GetCharByDBnum(actorid.actorID_Num);
					if(spChar == NULL)
						break;

					TOURNAMENT_RIGHT_TEAMNAME_BOX_UP_TB->SetOneLineText(spChar->GetName(), NS_UITEXTCOLOR::WHITE);
				}

				break;
			}
		}
	}


	
	
}

void CTournamentIndicator::SetTeamVSTeam()
{
    SetPlayerCount(1,FALSE);
    SetPlayerCount(2,FALSE);
    //SetPlayerCount(3,FALSE);
    SetPlayerCount(3,TRUE);

    if ( pLeftTeamIcon )
        pLeftTeamIcon->SetVisibleSingle(TRUE);
    if ( pRightTeamIcon )
        pRightTeamIcon->SetVisibleSingle(TRUE);
    if ( pLeftTeamImage )
        pLeftTeamImage->SetVisibleSingle(TRUE);
    if ( pRightTeamImage )
        pRightTeamImage->SetVisibleSingle(TRUE);
    if ( pTeamObeject )
        pTeamObeject->SetVisibleSingle(TRUE);

    MatchingSystem::GLTouranmentClient* glTour = m_pGaeaClient->GetTournamentClient();

    if ( glTour->m_fIndicatorTime > 0.0f )
        m_pTimerIndicator->SetVisibleSingle(TRUE);

    TOURNAMENT_LEFT_TEAMNAME_BOX_UP_TB->SetOneLineText(glTour->m_strLeftTeamName.c_str(), glTour->m_dwLeftTeamNameColor);
    TOURNAMENT_RIGHT_TEAMNAME_BOX_UP_TB->SetOneLineText(glTour->m_strRightTeamName.c_str(), glTour->m_dwRightTeamNameColor);
    CString strTemp1;
    strTemp1.Format("%d", (int)glTour->m_fLeftTeamScore );
    TOURNAMENT_LEFT_TEAMNAME_BOX_DOWN_TB->SetOneLineText(strTemp1, glTour->m_dwLeftTeamScoreColor);
    CString strTemp2;
    strTemp2.Format("%d", (int)glTour->m_fRightTeamScore );
    TOURNAMENT_RIGHT_TEAMNAME_BOX_DOWN_TB->SetOneLineText(strTemp2, glTour->m_dwRightTeamScoreColor);

    pLeftTeamIcon->SetImage( glTour->m_strLeftTeamIconXmlKeyword.c_str() );
    pRightTeamIcon->SetImage( glTour->m_strRightTeamIconXmlKeyword.c_str() );
    pLeftTeamImage->SetImage( glTour->m_strLeftTeamImageXmlKeyword.c_str() );
    pRightTeamImage->SetImage( glTour->m_strRightTeamImageXmlKeyword.c_str() );

    pTeamObeject->SetOneLineText( glTour->m_strTeamObjectText.c_str(), glTour->m_dwTeamObjectTextColor);
}

void CTournamentIndicator::SetTeamVsTeamInfo( const NET_MSG_GENERIC* const _pMessage )
{
    const GLMSG::NET_TOURNAMENT_INDICATOR_UI_INFO_FC* _pMsg = (const GLMSG::NET_TOURNAMENT_INDICATOR_UI_INFO_FC*)_pMessage;

    MatchingSystem::GLTouranmentClient* glTour = m_pGaeaClient->GetTournamentClient();

    switch ( _pMsg->emInfoType )
    {
    case GLMSG::NET_TOURNAMENT_INDICATOR_UI_INFO_FC::Time:
        glTour->m_fIndicatorTime = _pMsg->fFloatData;
        m_pTimerIndicator->SetTime((int)glTour->m_fIndicatorTime);
        break;
    case GLMSG::NET_TOURNAMENT_INDICATOR_UI_INFO_FC::LeftName:
        glTour->m_strLeftTeamName = _pMsg->strText;
        glTour->m_dwLeftTeamNameColor = _pMsg->dwFontColor;
        TOURNAMENT_LEFT_TEAMNAME_BOX_UP_TB->SetOneLineText(glTour->m_strLeftTeamName.c_str(), _pMsg->dwFontColor);
        break;
    case GLMSG::NET_TOURNAMENT_INDICATOR_UI_INFO_FC::RightName:
        glTour->m_strRightTeamName = _pMsg->strText;
        glTour->m_dwRightTeamNameColor = _pMsg->dwFontColor;
        TOURNAMENT_RIGHT_TEAMNAME_BOX_UP_TB->SetOneLineText(glTour->m_strRightTeamName.c_str(), _pMsg->dwFontColor);
        break;
    case GLMSG::NET_TOURNAMENT_INDICATOR_UI_INFO_FC::LeftIcon:
        glTour->m_strLeftTeamIconXmlKeyword = _pMsg->strText;
        pLeftTeamIcon->SetImage( glTour->m_strLeftTeamIconXmlKeyword.c_str() );
        break;
    case GLMSG::NET_TOURNAMENT_INDICATOR_UI_INFO_FC::RightIcon:
        glTour->m_strRightTeamIconXmlKeyword = _pMsg->strText;
        pRightTeamIcon->SetImage( glTour->m_strRightTeamIconXmlKeyword.c_str() );
        break;
    case GLMSG::NET_TOURNAMENT_INDICATOR_UI_INFO_FC::LeftScore:
        {
            //glTour->m_strLeftTeamScore = _pMsg->strText;
            glTour->m_fLeftTeamScore = _pMsg->fFloatData;
            CString strTemp;
            strTemp.Format("%d", (int)glTour->m_fLeftTeamScore );
            glTour->m_dwLeftTeamScoreColor = _pMsg->dwFontColor;
            TOURNAMENT_LEFT_TEAMNAME_BOX_DOWN_TB->SetOneLineText(strTemp, _pMsg->dwFontColor);
        }
        break;
    case GLMSG::NET_TOURNAMENT_INDICATOR_UI_INFO_FC::RightScore:
        {
            //glTour->m_strRightTeamScore = _pMsg->strText;
            glTour->m_fRightTeamScore = _pMsg->fFloatData;
            CString strTemp;
            strTemp.Format("%d", (int)glTour->m_fRightTeamScore );
            glTour->m_dwRightTeamScoreColor = _pMsg->dwFontColor;
            TOURNAMENT_RIGHT_TEAMNAME_BOX_DOWN_TB->SetOneLineText(strTemp, _pMsg->dwFontColor);
        }
        break;
    case GLMSG::NET_TOURNAMENT_INDICATOR_UI_INFO_FC::LeftImage:
        glTour->m_strLeftTeamImageXmlKeyword = _pMsg->strText;
        pLeftTeamImage->SetImage( glTour->m_strLeftTeamImageXmlKeyword.c_str() );
        glTour->m_bLeftTeamImageBlink = _pMsg->bBlink;

        break;
    case GLMSG::NET_TOURNAMENT_INDICATOR_UI_INFO_FC::RightImage:
        glTour->m_strRightTeamImageXmlKeyword = _pMsg->strText;
        glTour->m_bRightTeamImageBlink = _pMsg->bBlink;
        pRightTeamImage->SetImage( glTour->m_strRightTeamImageXmlKeyword.c_str() );
        break;
    case GLMSG::NET_TOURNAMENT_INDICATOR_UI_INFO_FC::Object:
        glTour->m_strTeamObjectText = _pMsg->strText;
        glTour->m_dwTeamObjectTextColor = _pMsg->dwFontColor;
        pTeamObeject->SetOneLineText( glTour->m_strTeamObjectText.c_str(), _pMsg->dwFontColor );
        break;
    case GLMSG::NET_TOURNAMENT_INDICATOR_UI_INFO_FC::Show:
        {
            if ( _pMsg->fFloatData == 1.0f )
                m_pInterface->SetTournamentIndicatorType(MatchingSystem::TOURNAMENT_UI_TEAM_VS_TEAM, 0, 0);
            else
                m_pInterface->UiHideGroup(COMPETITION_TOURNAMENTENT_INDICATOR);
        }
        break;
    }
}

void CTournamentIndicator::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update(x,y,LB,MB,RB,nScroll,fElapsedTime,bFirstControl);

	if(this->IsVisible())
	{

		MatchingSystem::GLTouranmentClient* glTour = m_pGaeaClient->GetTournamentClient();

		for(int i = 0 ; i < 5 ; i ++)
		{
			m_pTournamentRight[i]->SetVisibleSingle(FALSE);
		}
	


		for(int i = 0 ; i < 5 ; i ++)
		{
			m_pTournamentLeft[i]->SetVisibleSingle(FALSE);
		}
		switch(glTour->m_dwObserveType)
		{
		case MatchingSystem::TOURNAMENT_UI_ONE_BY_ONE:
			SetOneByOne();
			break;
		case MatchingSystem::TOURNAMENT_UI_N_BY_N:
			SetNbyN();
			break;
        case MatchingSystem::TOURNAMENT_UI_TEAM_VS_TEAM:
            UpdateTeamVSTeam(x, y, fElapsedTime);
            break;
		case MatchingSystem::TOURNAMENT_UI_LEAGE_SCORE:
			break;
		default:
			break;
		}
	}

}

void CTournamentIndicator::UpdateOneByOne()
{

	MatchingSystem::GLTouranmentClient* glTour = m_pGaeaClient->GetTournamentClient();

	{
		Faction::FactionID _FactionIdInstance(Faction::EMFACTION_TEAM,glTour->m_dw1FactionID);
		const Faction::FactionMemberVector* FactionMember = 
			m_pGaeaClient->GetLandManClient()->GetFactionManager()->getFactionMembers(_FactionIdInstance);

		if(FactionMember == NULL)
			return;

		//1번 캐릭터 모양을 정하자.
		BOOST_FOREACH( Faction::GLActorID actorid, *FactionMember )
		{
			LeftDBID = actorid.actorID_Num;
			break;
		}
	}
	if( m_pGaeaClient->GetCharacter()->CharDbNum() == LeftDBID)
	{
		GLCharacter* spChar = m_pGaeaClient->GetCharacter();

		if(spChar != NULL)
		{

			pLeftIMAGECLASS->SetImageClass(CharClassToIndex(spChar->m_emClass));
			pLeftIMAGESCHOOL->SetImageSchool(spChar->GetCharSchool());

			float tempPersent = 0;
			tempPersent = ( (float)spChar->GETHP() / (float)spChar->GETMAXHP() );
			if(tempPersent != LeftIdPersent)
			{
				
				LeftIdPersent = tempPersent;
				m_pLeftHP->SetPercent ( LeftIdPersent );
			}
		}
	}
	else
	{
		std::tr1::shared_ptr<GLCharClient> spChar = m_pGaeaClient->GetCharByDBnum(LeftDBID);
		if(spChar != NULL)
		{

			pLeftIMAGECLASS->SetImageClass(CharClassToIndex(spChar->m_emClass));
			pLeftIMAGESCHOOL->SetImageSchool(spChar->GetCharSchool());

			float tempPersent = 0;
			tempPersent = ( (float)spChar->GETHP() / (float)spChar->GETMAXHP() );
			if(tempPersent != LeftIdPersent)
			{
				LeftIdPersent = tempPersent;
				m_pLeftHP->SetPercent ( LeftIdPersent );
			}
		}

		
	}
	

	{
		Faction::FactionID _FactionIdInstance(Faction::EMFACTION_TEAM,glTour->m_dw2FactionID);
		const Faction::FactionMemberVector* FactionMember = 
			m_pGaeaClient->GetLandManClient()->GetFactionManager()->getFactionMembers(_FactionIdInstance);

		if(FactionMember == NULL)
			return;

		//1번 캐릭터 모양을 정하자.
		BOOST_FOREACH( Faction::GLActorID actorid, *FactionMember )
		{
			RightDBID = actorid.actorID_Num;
			break;
		}
	}

	if( m_pGaeaClient->GetCharacter()->CharDbNum() == RightDBID)
	{
		GLCharacter* spChar = m_pGaeaClient->GetCharacter();

		if(spChar != NULL)
		{
			float tempPersent = 0;
			tempPersent = ( (float)spChar->GETHP() / (float)spChar->GETMAXHP() );
			if(tempPersent != RightIdPersent)
			{
				RightIdPersent = tempPersent;
				m_pLeftHP->SetPercent ( RightIdPersent );
			}
		}

	}
	else
	{
		std::tr1::shared_ptr<GLCharClient> spChar2 = m_pGaeaClient->GetCharByDBnum(RightDBID);
		if(spChar2 != NULL)
		{
			float tempPersent = 0;
			tempPersent = ( (float)spChar2->GETHP() / (float)spChar2->GETMAXHP() );
			if(tempPersent != RightIdPersent)
			{
				RightIdPersent = tempPersent;
				m_pRightHP->SetPercent ( RightIdPersent );
			}
		}


	}

	
}

void CTournamentIndicator::UpdateTeamVSTeam(const int _x, const int _y, const float _elapsedTime)
{
    // 이부분은 기본에 UI 활용법과는 상당히 이질적이게 제작되었음;
    // 자식 컨트롤이 드래그 이벤트를 캐치한 후 자신의 컨트롤을 이동시키는 방식;
    // 기존 토너먼트 UI가 형태가 일정하지 않게 되어있고 코드가 개판이라서 일정상 이런 방식을 사용하였음; 
    // 제대로 고치려면 인디케이터 타입마다 하나씩 윈도우를 다시 제작해야하지만 그럴바엔 차라리 스케일폼으로 다시 만드는게 훨씬 좋음;
    {
        CUIGroup::DragMessage(pTeamVsTeamWindowBox->GetMessageEx());
        CUIGroup::DragUpdate(_x, _y, m_vPos);

        float fDEF_X_RES = (float)HIWORD(m_pInterface->UiGetResolution());
        const float _leftPos = GetGlobalPos().left + pTeamVsTeamWindowBox->GetLocalPos().left;
        const float _rightPos = GetGlobalPos().left + pTeamVsTeamWindowBox->GetLocalPos().right;
        if (_leftPos < 0.0f)
        {
            CUIGroup::AddPosition( D3DXVECTOR2 ( -_leftPos, 0.0f) );
        }
        else if (_rightPos > fDEF_X_RES)
        {
            CUIGroup::AddPosition( D3DXVECTOR2 ( fDEF_X_RES - _rightPos, 0.0f) );
        }
    }
    

    MatchingSystem::GLTouranmentClient* glTour = m_pGaeaClient->GetTournamentClient();

    if ( glTour->m_fIndicatorTime > 0.0f )    
    {
        glTour->m_fIndicatorTime -= _elapsedTime;
        m_pTimerIndicator->SetTime((int)glTour->m_fIndicatorTime);
    }

    // CTournamentIndicator UI는 공용UI로 제작되었으나 그 기능이 제한적이여 공용UI로 사용이 힘들다;
    // 일정문제로 blink기능을 매우 비율적인 구조로 제작하였으므로 스케일폼등 추후 개편 작업시 공용적인 형태로 변경필요;
    // Blink처리를 할수 있게 SwapImage를 상속받은 클래스를 새로 만들면 좋을 것 같다.;
    fBlinkTimer -= _elapsedTime;
    if ( fBlinkTimer < 0.0f )
    {
        fBlinkTimer = 0.5f;
        if ( glTour->m_bLeftTeamImageBlink == true )
        {
            pLeftTeamImage->SetVisibleSingle(  !pLeftTeamImage->IsVisible() );
        }
        else
            pLeftTeamImage->SetVisibleSingle(true);

        if ( glTour->m_bRightTeamImageBlink == true )   
        {
            pRightTeamImage->SetVisibleSingle(  !pRightTeamImage->IsVisible() );
        }
        else
            pRightTeamImage->SetVisibleSingle(true);
    }

    //glTour->m_strLeftTeamIconXmlKeyword;
    //glTour->m_strRightTeamIconXmlKeyword;
    //glTour->m_strLeftTeamName;
    //glTour->m_strRightTeamName;
    //glTour->m_strLeftTeamScore;
    //glTour->m_strRightTeamScore;
    //glTour->m_strLeftTeamImageXmlKeyword;
    //glTour->m_strRightTeamImageXmlKeyword;
    //glTour->m_strTeamObjectText;
}

void CTournamentIndicator::SetGameType( DWORD PlayerCount, DWORD FactionCount )
{

/*
	BOOL IsShow = FALSE;

	SetPlayerCount(1,IsShow);
	SetPlayerCount(2,IsShow);
	SetPlayerCount(3,IsShow);

	IsShow = TRUE;

	SetPlayerCount(PlayerCount,IsShow);
	*/
	
}

void CTournamentIndicator::SetTime(int nSec, bool bBlank, bool bShow)
{
	BOOL IsShow = bShow ? TRUE : FALSE;
 	if (m_pTimerIndicator)
 	{
		if(nSec < 0)
		{
			nSec *= -1;
		}
		m_pTimerIndicator->SetBlank(bBlank);
 		m_pTimerIndicator->SetTime(nSec);
 		m_pTimerIndicator->SetVisibleSingle(IsShow);
 	}
}

void CTournamentIndicator::SetPlayerCount(DWORD PlayerCount,BOOL IsShow)
{
	if(PlayerCount == 1)
	{
		if(pLeftIMAGECLASS)
			pLeftIMAGECLASS->SetVisibleSingle(IsShow);
		if(pLeftIMAGESCHOOL)
			pLeftIMAGESCHOOL->SetVisibleSingle(IsShow);
		if(m_pLeftHP)
			m_pLeftHP->SetVisibleSingle(IsShow);
		if(pLftBOXText)
			pLftBOXText->SetVisibleSingle(IsShow);
		if(pLftBOX)
			pLftBOX->SetVisibleSingle(IsShow);
	}
	else if(PlayerCount == 2)
	{
		if(pLeftIMAGECLASS)
			pLeftIMAGECLASS->SetVisibleSingle(IsShow);
		if(pRightIMAGECLASS)
			pRightIMAGECLASS->SetVisibleSingle(IsShow);

		if(pLeftIMAGESCHOOL)
			pLeftIMAGESCHOOL->SetVisibleSingle(IsShow);
		if(pRightIMAGESCHOOL)
			pRightIMAGESCHOOL->SetVisibleSingle(IsShow);

		if(m_pLeftHP)
			m_pLeftHP->SetVisibleSingle(IsShow);
		if(m_pRightHP)
			m_pRightHP->SetVisibleSingle(IsShow);

		if(pLftBOXText)
			pLftBOXText->SetVisibleSingle(IsShow);
		if(pRightBOXText)
			pRightBOXText->SetVisibleSingle(IsShow);

		if(pLftBOX)
			pLftBOX->SetVisibleSingle(IsShow);
		if(pRightBOX)
			pRightBOX->SetVisibleSingle(IsShow);
		SetPlayerCount(3,FALSE);
		return;
	}
	else
	{
		if(TOURNAMENT_LEFT_TEAMNAME_BOX)
			TOURNAMENT_LEFT_TEAMNAME_BOX->SetVisibleSingle(IsShow);
		if(TOURNAMENT_LEFT_TEAMNAME_BOX_BG)
			TOURNAMENT_LEFT_TEAMNAME_BOX_BG->SetVisibleSingle(IsShow);
		if(TOURNAMENT_LEFT_TEAMNAME_BOX_DOWN)
			TOURNAMENT_LEFT_TEAMNAME_BOX_DOWN->SetVisibleSingle(IsShow);

		if(TOURNAMENT_LEFT_TEAMNAME_BOX_UP_TB)
			TOURNAMENT_LEFT_TEAMNAME_BOX_UP_TB->SetVisibleSingle(IsShow);
		if(TOURNAMENT_LEFT_TEAMNAME_BOX_DOWN_TB)
			TOURNAMENT_LEFT_TEAMNAME_BOX_DOWN_TB->SetVisibleSingle(IsShow);

		if(TOURNAMENT_RIGHT_TEAMNAME_BOX)
			TOURNAMENT_RIGHT_TEAMNAME_BOX->SetVisibleSingle(IsShow);
		if(TOURNAMENT_RIGHT_TEAMNAME_BOX_BG)
			TOURNAMENT_RIGHT_TEAMNAME_BOX_BG->SetVisibleSingle(IsShow);
		if(TOURNAMENT_RIGHT_TEAMNAME_BOX_DOWN)
			TOURNAMENT_RIGHT_TEAMNAME_BOX_DOWN->SetVisibleSingle(IsShow);

		if(TOURNAMENT_RIGHT_TEAMNAME_BOX_UP_TB)
			TOURNAMENT_RIGHT_TEAMNAME_BOX_UP_TB->SetVisibleSingle(IsShow);
		if(TOURNAMENT_RIGHT_TEAMNAME_BOX_DOWN_TB)
			TOURNAMENT_RIGHT_TEAMNAME_BOX_DOWN_TB->SetVisibleSingle(IsShow);
	}

    if ( pLeftTeamIcon )
        pLeftTeamIcon->SetVisibleSingle(FALSE);
    if ( pRightTeamIcon )
        pRightTeamIcon->SetVisibleSingle(FALSE);
    if ( pLeftTeamImage )
        pLeftTeamImage->SetVisibleSingle(FALSE);
    if ( pRightTeamImage )
        pRightTeamImage->SetVisibleSingle(FALSE);
    if ( pTeamObeject )
        pTeamObeject->SetVisibleSingle(FALSE);

	return;
}