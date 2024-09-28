#include "StdAfx.h"


#include "../../InnerInterface.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../EngineLib/GUInterface/BasicProgressBar.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"

#include "../../../RanLogicClient/GLGaeaClient.h"

#include "TournamentMulty.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CTournamentMulty::CTournamentMulty(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice) :
	CUIGroupHelperNonClose(pEngineDevice,pInterface)
,	m_pGaeaClient(pGaeaClient)
,TOURNAMENT_RIGHT_MULTY_TEAM_BG(NULL)
,TOURNAMENT_RIGHT_MULTY_TEAM_BG_INNER(NULL)
,TOURNAMENT_RIGHT_MULTY_TEAM_RESUALT(NULL)

,TOURNAMENT_RIGHT_MULTY_TEAM_TOP_RANK(NULL)
,TOURNAMENT_RIGHT_MULTY_TEAM_TOP_NAME(NULL)
,TOURNAMENT_RIGHT_MULTY_TEAM_TOP_SCORE(NULL)
,TOURNAMENT_RIGHT_MULTY_TEAM_MYTEAM_TOP_RANK(NULL)
,TOURNAMENT_RIGHT_MULTY_TEAM_MYTEAM_TOP_NAME(NULL)
,TOURNAMENT_RIGHT_MULTY_TEAM_MYTEAM_TOP_SCORE(NULL)

,TOURNAMENT_RIGHT_MULTY_TEAM_RESUALT_TB(NULL)

{
	
}

CTournamentMulty::~CTournamentMulty()
{
}

bool CTournamentMulty::CreateSubControl()
{
	CUIGroupHelperNonClose::CreateLightGrayMove("TOURNAMENT_RIGHT_MULTY_TEAM_BG");
	CD3DFontPar* pFont_Shadow = m_pEngineDevice->LoadFont(_DEFAULT_FONT, 9 , _DEFAULT_FONT_SHADOW_FLAG);
	CD3DFontPar* pFontBIG_Shadow = m_pEngineDevice->LoadFont(_DEFAULT_FONT, 12 , _DEFAULT_FONT_SHADOW_FLAG);

	const int nAlignLeftTop     = TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP;	
	const int nAlignLeftCenter  = TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y;	
	const int nAlignRightCenter = TEXT_ALIGN_RIGHT | TEXT_ALIGN_CENTER_Y;	
	const int nAlignRightTop    = TEXT_ALIGN_RIGHT | TEXT_ALIGN_TOP;	
	const int nAlignCenter	    = TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y;	

	//TOURNAMENT_RIGHT_MULTY_TEAM_BG = CreateLineBox("TOURNAMENT_RIGHT_MULTY_TEAM_BG", "POINT_SHOP_UP_TEXT_BOX_BACK_LINE",CONTROL_ID_BACK);
	TOURNAMENT_RIGHT_MULTY_TEAM_BG_INNER = CreateLineBox("TOURNAMENT_RIGHT_MULTY_TEAM_BG_INNER", "RNCDMPROGRESS_BG_LINE");

	//TOURNAMENT_RIGHT_MULTY_TEAM_TOP = CreateLineBox("TOURNAMENT_RIGHT_MULTY_TEAM_TOP", "GRAY_BACKGROUND_BLACK_LINE_TEXTURE");

	//TOURNAMENT_RIGHT_MULTY_TEAM_MYTEAM = CreateLineBox("TOURNAMENT_RIGHT_MULTY_TEAM_MYTEAM", "GRAY_BACKGROUND_BLACK_LINE_TEXTURE");

	TOURNAMENT_RIGHT_MULTY_TEAM_RESUALT = CreateLineBox("TOURNAMENT_RIGHT_MULTY_TEAM_RESUALT", "GRAY_BACKGROUND_BLACK_LINE_TEXTURE");

	TOURNAMENT_RIGHT_MULTY_TEAM_TOP_RANK = CreateStaticControl("TOURNAMENT_RIGHT_MULTY_TEAM_TOP_RANK", pFont_Shadow, nAlignRightCenter,NO_ID);
	TOURNAMENT_RIGHT_MULTY_TEAM_TOP_RANK->SetOneLineText("NOW RANK 1", NS_UITEXTCOLOR::WHITE);

	TOURNAMENT_RIGHT_MULTY_TEAM_TOP_NAME = CreateStaticControl("TOURNAMENT_RIGHT_MULTY_TEAM_TOP_NAME", pFont_Shadow, nAlignCenter,NO_ID);
	TOURNAMENT_RIGHT_MULTY_TEAM_TOP_NAME->SetOneLineText("USERNAME_1", NS_UITEXTCOLOR::WHITE);

	TOURNAMENT_RIGHT_MULTY_TEAM_TOP_SCORE = CreateStaticControl("TOURNAMENT_RIGHT_MULTY_TEAM_TOP_SCORE", pFont_Shadow, nAlignCenter,NO_ID);
	TOURNAMENT_RIGHT_MULTY_TEAM_TOP_SCORE->SetOneLineText("69,515", NS_UITEXTCOLOR::WHITE);

	TOURNAMENT_RIGHT_MULTY_TEAM_MYTEAM_TOP_RANK = CreateStaticControl("TOURNAMENT_RIGHT_MULTY_TEAM_MYTEAM_TOP_RANK", pFont_Shadow, nAlignRightCenter,NO_ID);
	TOURNAMENT_RIGHT_MULTY_TEAM_MYTEAM_TOP_RANK->SetOneLineText("RANK 84", NS_UITEXTCOLOR::GREEN);

	TOURNAMENT_RIGHT_MULTY_TEAM_MYTEAM_TOP_NAME = CreateStaticControl("TOURNAMENT_RIGHT_MULTY_TEAM_MYTEAM_TOP_NAME", pFont_Shadow, nAlignCenter,NO_ID);
	TOURNAMENT_RIGHT_MULTY_TEAM_MYTEAM_TOP_NAME->SetOneLineText("USERNAME_2", NS_UITEXTCOLOR::GREEN);

	TOURNAMENT_RIGHT_MULTY_TEAM_MYTEAM_TOP_SCORE = CreateStaticControl("TOURNAMENT_RIGHT_MULTY_TEAM_MYTEAM_TOP_SCORE", pFont_Shadow, nAlignCenter,NO_ID);
	TOURNAMENT_RIGHT_MULTY_TEAM_MYTEAM_TOP_SCORE->SetOneLineText("69,515", NS_UITEXTCOLOR::GREEN);

	TOURNAMENT_RIGHT_MULTY_TEAM_RESUALT_TB = CreateStaticControl("TOURNAMENT_RIGHT_MULTY_TEAM_RESUALT", pFont_Shadow, nAlignCenter,NO_ID);
	TOURNAMENT_RIGHT_MULTY_TEAM_RESUALT_TB->SetOneLineText("Club Deth Match Progress 2round 00:00:06", NS_UITEXTCOLOR::YELLOW);
	return true;
}


CBasicLineBoxSmart* CTournamentMulty::CreateLineBox ( const std::string& strControlName, const std::string& strTexInfo, const UIGUID cID )
{
	CBasicLineBoxSmart* pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
	pLineBox->CreateSub( this, strControlName.c_str(), UI_FLAG_DEFAULT, cID );
	pLineBox->CreateSubControl( strTexInfo.c_str() );
	RegisterControl( pLineBox );
	return pLineBox;
}

CBasicTextBox* CTournamentMulty::CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, szControlKeyword, UI_FLAG_DEFAULT, cID );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );	
	RegisterControl ( pStaticText );

	return pStaticText;
}