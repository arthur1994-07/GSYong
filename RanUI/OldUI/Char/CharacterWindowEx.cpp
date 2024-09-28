#include "StdAfx.h"
#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicProgressBar.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/SmartComboBoxRollOver.h"

#include "../../../RanLogic/GLUseFeatures.h"
#include "../../../RanLogic/GLogicData.h"
#include "../../../RanLogic/Activity/Activity.h"

#include "../../../RanLogic/GLUseFeatures.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"

#include "CharacterWindow.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


//	레벨
void CCharacterWindow::SetLevel ( const GLCHARLOGIC& sCharData )
{
	const WORD& wLevel = sCharData.m_wLevel;

    CString strCombine;
	strCombine.Format ( "%d", wLevel );
	if ( m_pLevelText )
	{
		m_pLevelText->SetOneLineText ( strCombine, NS_UITEXTCOLOR::GOLD );
	}
}

void CCharacterWindow::SetName ( const GLCHARLOGIC& sCharData )
{
	if ( m_pNameText )
	{
		m_pNameText->SetOneLineText ( sCharData.m_szName, NS_UITEXTCOLOR::ORANGE );

        std::string strName = sCharData.m_szName;

        //  Title 위치 갱신
        if ( m_srtNameBack != strName ) 
        {
            m_srtNameBack = strName;
            UpdateTitlePos();
        }
	}
}

//	경험치
void CCharacterWindow::SetEXP( const GLCHARLOGIC& sCharData )
{
	LONGLONG lNOW = sCharData.m_sExperience.lnNow;
	LONGLONG lMAX = sCharData.m_sExperience.lnMax;

	int nIndex = 0;
	
	if ( m_pEXPText ) m_pEXPText->ClearText ();

	if( lNOW > lMAX ) lNOW = lMAX;

	CString strCombine;
	strCombine.Format ( "%I64d", lNOW );
	if ( m_pEXPText ) nIndex = m_pEXPText->AddTextNoSplit ( strCombine, NS_UITEXTCOLOR::WHITE );

	strCombine.Format ( " / " );
	if ( m_pEXPText ) m_pEXPText->AddString ( nIndex, strCombine, NS_UITEXTCOLOR::LIGHTGREY );

	strCombine.Format ( "%I64d", lMAX );
	if ( m_pEXPText ) m_pEXPText->AddString ( nIndex, strCombine, NS_UITEXTCOLOR::WHITE );

	const float fPercent = float(lNOW) / float(lMAX);
	if ( m_pEXP ) m_pEXP->SetPercent ( fPercent );
}

//	Stat	
void CCharacterWindow::SetPow ( const GLCHARLOGIC& sCharData )
{
	m_pPowText->ClearText ();

	CString strCombine;
	strCombine.Format ( "%d", sCharData.m_sSUMSTATS_SKILL.wPow );
	int nIndex = m_pPowText->AddText ( strCombine, NS_UITEXTCOLOR::WHITE );

	SCHARSTATS sADD = sCharData.GETSTATS_ADD();
	SCHARSTATS sITEM = sCharData.GETSTATS_ITEM();

	if ( !sADD.wPow ) return ;
	
	strCombine.Format ( "(+%d)", sADD.wPow );

	D3DCOLOR dwColor = NS_UITEXTCOLOR::WHITE;
	if ( sITEM.wPow ) dwColor = NS_UITEXTCOLOR::PLUSOPTION;

	m_pPowText->AddString ( nIndex, strCombine, dwColor );	
}

void CCharacterWindow::SetDex ( const GLCHARLOGIC& sCharData )
{
	m_pDexText->ClearText ();

	CString strCombine;
	strCombine.Format ( "%d", sCharData.m_sSUMSTATS_SKILL.wDex );

	int nIndex = m_pDexText->AddText ( strCombine, NS_UITEXTCOLOR::WHITE );

	SCHARSTATS sADD = sCharData.GETSTATS_ADD();
	SCHARSTATS sITEM = sCharData.GETSTATS_ITEM();

	if ( !sADD.wDex ) return ;

	strCombine.Format ( "(+%d)", sADD.wDex );

	D3DCOLOR dwColor = NS_UITEXTCOLOR::WHITE;
	if ( sITEM.wDex ) dwColor = NS_UITEXTCOLOR::PLUSOPTION;

	m_pDexText->AddString ( nIndex, strCombine, dwColor );
}

void CCharacterWindow::SetSpi ( const GLCHARLOGIC& sCharData )
{
	m_pSpiText->ClearText ();

	CString strCombine;
	strCombine.Format ( "%d", sCharData.m_sSUMSTATS_SKILL.wSpi );
	int nIndex = m_pSpiText->AddText ( strCombine, NS_UITEXTCOLOR::WHITE );

	SCHARSTATS sADD = sCharData.GETSTATS_ADD();
	SCHARSTATS sITEM = sCharData.GETSTATS_ITEM();

	if ( !sADD.wSpi ) return ;

	strCombine.Format ( "(+%d)", sADD.wSpi );

	D3DCOLOR dwColor = NS_UITEXTCOLOR::WHITE;
	if ( sITEM.wSpi ) dwColor = NS_UITEXTCOLOR::PLUSOPTION;

	m_pSpiText->AddString ( nIndex, strCombine, dwColor );
}

void CCharacterWindow::SetInt ( const GLCHARLOGIC& sCharData )
{
	//m_pIntText->ClearText ();

	//CString strCombine;
	//strCombine.Format ( "%d", sCharData.m_sSUMSTATS.wInt );
	//int nIndex = m_pIntText->AddText ( strCombine, NS_UITEXTCOLOR::WHITE );

	//SCHARSTATS sADD = sCharData.GETSTATS_ADD();
	//SCHARSTATS sITEM = sCharData.GETSTATS_ITEM();

	//if ( !sADD.wInt ) return ;

	//strCombine.Format ( "(+%d)", sADD.wInt );

	//D3DCOLOR dwColor = NS_UITEXTCOLOR::WHITE;
	//if ( sITEM.wInt ) dwColor = NS_UITEXTCOLOR::PLUSOPTION;

	//m_pIntText->AddString ( nIndex, strCombine, dwColor );
}

void CCharacterWindow::SetStr ( const GLCHARLOGIC& sCharData )
{
	m_pStrText->ClearText ();

	CString strCombine;
	strCombine.Format ( "%d", sCharData.m_sSUMSTATS_SKILL.wStr );
	int nIndex = m_pStrText->AddText ( strCombine, NS_UITEXTCOLOR::WHITE );

	SCHARSTATS sADD = sCharData.GETSTATS_ADD();
	SCHARSTATS sITEM = sCharData.GETSTATS_ITEM();

	if ( !sADD.wStr ) return ;

	strCombine.Format ( "(+%d)", sADD.wStr );

	D3DCOLOR dwColor = NS_UITEXTCOLOR::WHITE;
	if ( sITEM.wStr ) dwColor = NS_UITEXTCOLOR::PLUSOPTION;

	m_pStrText->AddString ( nIndex, strCombine, dwColor );
}

void CCharacterWindow::SetSta ( const GLCHARLOGIC& sCharData )
{
	m_pStaText->ClearText ();

	CString strCombine;
	strCombine.Format ( "%d", sCharData.m_sSUMSTATS_SKILL.wSta );
	int nIndex = m_pStaText->AddText ( strCombine, NS_UITEXTCOLOR::WHITE );

	SCHARSTATS sADD = sCharData.GETSTATS_ADD();
	SCHARSTATS sITEM = sCharData.GETSTATS_ITEM();

	if ( !sADD.wSta ) return ;

	strCombine.Format ( "(+%d)", sADD.wSta );

	D3DCOLOR dwColor = NS_UITEXTCOLOR::WHITE;
	if ( sITEM.wSta ) dwColor = NS_UITEXTCOLOR::PLUSOPTION;

	m_pStaText->AddString ( nIndex, strCombine, dwColor );
}

void CCharacterWindow::SetPointControl ( BOOL bPointUsable )
{
	//	Point
	if ( m_pPointDisable ) m_pPointDisable->SetVisibleSingle ( !bPointUsable );
	if ( m_pPointEnable ) m_pPointEnable->SetVisibleSingle ( bPointUsable );

	//	Stats Button
	if ( m_pPowButton ) m_pPowButton->SetVisibleSingle ( bPointUsable );
	if ( m_pDexButton ) m_pDexButton->SetVisibleSingle ( bPointUsable );
	if ( m_pSpiButton ) m_pSpiButton->SetVisibleSingle ( bPointUsable );

	if ( m_pStrButton ) m_pStrButton->SetVisibleSingle ( bPointUsable );
	if ( m_pStaButton ) m_pStaButton->SetVisibleSingle ( bPointUsable );

	if ( m_pPowDiaableButton ) m_pPowDiaableButton->SetVisibleSingle ( !bPointUsable );
	if ( m_pDexDiaableButton ) m_pDexDiaableButton->SetVisibleSingle ( !bPointUsable );
	if ( m_pSpiDiaableButton ) m_pSpiDiaableButton->SetVisibleSingle ( !bPointUsable );

	if ( m_pStrDiaableButton ) m_pStrDiaableButton->SetVisibleSingle ( !bPointUsable );
	if ( m_pStaDiaableButton ) m_pStaDiaableButton->SetVisibleSingle ( !bPointUsable );
}

void CCharacterWindow::SetPoint ( const GLCHARLOGIC& sCharData )
{
	CString strCombine;
	strCombine.Format ( "%d", sCharData.m_wStatsPoint );
	if ( m_pPointText ) m_pPointText->SetOneLineText ( strCombine, NS_UITEXTCOLOR::GOLD );
}

//	Status
void CCharacterWindow::SetDamage ( const GLCHARLOGIC& sCharData )
{	
	const int& nLow = sCharData.GETFORCE_LOW ();
	const int& nHigh = sCharData.GETFORCE_HIGH ();

	CString strCombine;
	strCombine.Format ( "%d~%d", nLow, nHigh );
	if ( m_pDamageText ) m_pDamageText->SetOneLineText ( strCombine, NS_UITEXTCOLOR::WHITE );
}

void CCharacterWindow::SetDefense ( const GLCHARLOGIC& sCharData )
{
	CString strCombine;
	strCombine.Format ( "%d", sCharData.GETDEFENSE () );
	if ( m_pDefenseText ) m_pDefenseText->SetOneLineText ( strCombine, NS_UITEXTCOLOR::WHITE );
}

void CCharacterWindow::SetReqPa ( const GLCHARLOGIC& sCharData )
{
	CString strCombine;
	strCombine.Format ( "%d", sCharData.m_sumPowerAttribute[SKILL::EMAPPLY_MELEE] );
	if ( m_pReqPaText ) m_pReqPaText->SetOneLineText ( strCombine, NS_UITEXTCOLOR::WHITE );
}

void CCharacterWindow::SetReqSa ( const GLCHARLOGIC& sCharData )
{
	CString strCombine;
	strCombine.Format ( "%d", sCharData.m_sumPowerAttribute[SKILL::EMAPPLY_RANGE] );
	if ( m_pReqSaText ) m_pReqSaText->SetOneLineText ( strCombine, NS_UITEXTCOLOR::WHITE );
}

void CCharacterWindow::SetReqMa ( const GLCHARLOGIC& sCharData )
{
	CString strCombine;
	strCombine.Format ( "%d", sCharData.m_sumPowerAttribute[SKILL::EMAPPLY_MAGIC] );
	if ( m_pReqMaText ) m_pReqMaText->SetOneLineText ( strCombine, NS_UITEXTCOLOR::WHITE );
}

void CCharacterWindow::SetHitRate ( const GLCHARLOGIC& sCharData )
{
	CString strCombine;
	strCombine.Format ( "%d", sCharData.GETHIT () );
	if ( m_pHitRateText ) m_pHitRateText->SetOneLineText ( strCombine, NS_UITEXTCOLOR::WHITE );
}

void CCharacterWindow::SetHP ( const GLCHARLOGIC& sCharData )
{
	CString strCombine;
	strCombine.Format ( "%d/%d", sCharData.GETHP(), sCharData.GETMAXHP());
	if ( m_pHPText ) m_pHPText->SetOneLineText ( strCombine, NS_UITEXTCOLOR::WHITE );
}

void CCharacterWindow::SetMP ( const GLCHARLOGIC& sCharData )
{
	CString strCombine;
	strCombine.Format ( "%d/%d", sCharData.GETMP(), sCharData.GETMAXMP());
	if ( m_pMPText ) m_pMPText->SetOneLineText ( strCombine, NS_UITEXTCOLOR::WHITE );
}

void CCharacterWindow::SetSP ( const GLCHARLOGIC& sCharData )
{
	CString strCombine;
	strCombine.Format ( "%d/%d", sCharData.GETSP(), sCharData.GETMAXSP());
	if ( m_pSPText ) m_pSPText->SetOneLineText ( strCombine, NS_UITEXTCOLOR::WHITE );
}

void CCharacterWindow::SetAvoidRate ( const GLCHARLOGIC& sCharData )
{
	CString strCombine;
	strCombine.Format ( "%d", sCharData.GETAVOID () );
	if ( m_pAvoidRateText ) m_pAvoidRateText->SetOneLineText ( strCombine, NS_UITEXTCOLOR::WHITE );
}

//	저항력
void CCharacterWindow::SetFireResi ( const GLCHARLOGIC& sCharData )
{
	CString strCombine;
	strCombine.Format ( "%d", sCharData.m_sSUMRESIST_SKILL.nFire );
	if ( m_pFireText ) m_pFireText->SetOneLineText ( strCombine, NS_UITEXTCOLOR::WHITE );
}

void CCharacterWindow::SetColdResi ( const GLCHARLOGIC& sCharData )
{
	CString strCombine;
	strCombine.Format ( "%d", sCharData.m_sSUMRESIST_SKILL.nIce );
	if ( m_pColdText ) m_pColdText->SetOneLineText ( strCombine, NS_UITEXTCOLOR::WHITE );
}

void CCharacterWindow::SetElectricResi ( const GLCHARLOGIC& sCharData )
{
	CString strCombine;
	strCombine.Format ( "%d", sCharData.m_sSUMRESIST_SKILL.nElectric );
	if ( m_pElectricText ) m_pElectricText->SetOneLineText ( strCombine, NS_UITEXTCOLOR::WHITE );
}

void CCharacterWindow::SetPoisonResi ( const GLCHARLOGIC& sCharData )
{
	CString strCombine;
	strCombine.Format ( "%d", sCharData.m_sSUMRESIST_SKILL.nPoison );
	if ( m_pPoisonText ) m_pPoisonText->SetOneLineText ( strCombine, NS_UITEXTCOLOR::WHITE );
}

void CCharacterWindow::SetSpiritResi ( const GLCHARLOGIC& sCharData )
{
	CString strCombine;
	strCombine.Format ( "%d", sCharData.m_sSUMRESIST_SKILL.nSpirit );
	if ( m_pSpiritText ) m_pSpiritText->SetOneLineText ( strCombine, NS_UITEXTCOLOR::WHITE );
}

//	사회성향
//void CCharacterWindow::SetPK ( const GLCHARLOGIC& sCharData )
//{
//	CString strCombine;
//	strCombine.Format ( "%d", sCharData.m_wPK );
//	if ( m_pPKText ) m_pPKText->SetOneLineText ( strCombine );
//}

void CCharacterWindow::SetBright ( const GLCHARLOGIC& sCharData )
{
	const int nMIN = -100;
	const int nMAX = 100;
	const int nRANGE = nMAX - nMIN;

	int nPercent = sCharData.GETBRIGHTPER ();
	nPercent = -(nPercent);	//	이미지가 반대로 뒤집혀있어서.
	if ( nPercent < nMIN ) nPercent = nMIN;
	if ( nPercent > nMAX ) nPercent = nMAX;

	nPercent += nMAX;

	float fPercent = static_cast<float>(nPercent) / static_cast<float>(nRANGE);

	const UIRECT& rcFramePos = m_pBrightFrame->GetGlobalPos ();
	const UIRECT& rcThumbPos = m_pBrightSlider->GetGlobalPos ();

	float fMOVE = (rcFramePos.sizeX - rcThumbPos.sizeX) * fPercent;
	float fLeft = rcFramePos.left + fMOVE;

	m_pBrightSlider->SetGlobalPos ( D3DXVECTOR2 ( fLeft, rcThumbPos.top ) );
}

void CCharacterWindow::SetCP ( const GLCHARLOGIC& sCharData )
{

	if( !GLCONST_CHAR::bCOMBAT_POINT ) 
	{
		m_pCPTextStatic->SetVisibleSingle(FALSE);
		m_pCPText->SetVisibleSingle(FALSE);
		return;
	}

	CString strCombine;
	strCombine.Format ( "%d/%d", sCharData.GETCP(), sCharData.GETMAXCP() );
	if ( m_pCPText ) m_pCPText->SetOneLineText ( strCombine, NS_UITEXTCOLOR::WHITE );
}

void CCharacterWindow::SetPKState(const GLCHARLOGIC& sCharData)
{
	if (m_pPKStateText)
	{		
		m_pPKStateText->SetOneLineText(
            m_pGaeaClient->GetCharacter()->GET_PK_NAME().c_str(),
            NS_UITEXTCOLOR::WHITE);
	}
}

void CCharacterWindow::SetGrade(const GLCHARLOGIC& sCharData)
{
	if (m_pGradeText)
	{
//		m_pGradeText->SetOneLineText ( ID2GAMEWORD("ACADEMY_NAME",sCharData.m_wSchool) );
	}
}

void CCharacterWindow::SetActivityPoint ( const GLCHARLOGIC& sCharData )
{
	if ( m_pActivityPoint )
	{
		CString strCombine;
		strCombine.Format ( "%d", sCharData.m_nLiving );
		m_pActivityPoint->SetOneLineText ( strCombine, NS_UITEXTCOLOR::WHITE );
	}
}

void CCharacterWindow::SetContributionPoint ( const GLCHARLOGIC& sCharData )
{
    if ( m_pContributionText )
    {
        CString strCombine;
        strCombine.Format ( "%d", sCharData.m_nContributionPoint );
        m_pContributionText->SetOneLineText ( strCombine, NS_UITEXTCOLOR::WHITE );
    }
}


void CCharacterWindow::SetAcademy ( const GLCHARLOGIC& sCharData )
{
	if ( m_pAcademy )
	{
		m_pAcademy->SetOneLineText ( ID2GAMEWORD("ACADEMY_NAME",sCharData.m_wSchool), NS_UITEXTCOLOR::WHITE );

		for ( int i = 0; i < nSCHOOL_MARK; ++i ) m_pSchoolMark[sCharData.m_wSchool]->SetVisibleSingle ( FALSE );
		m_pSchoolMark[sCharData.m_wSchool]->SetVisibleSingle ( TRUE );
	}
}

void CCharacterWindow::SetDepartment ( const GLCHARLOGIC& sCharData )
{	
	if ( m_pDepartment )
	{
		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			m_pDepartment->SetOneLineText ( CGameTextMan::GetInstance().GetCommentText("CHARCLASS", CharClassToIndex(sCharData.m_emClass)).GetString() );
		else
			m_pDepartment->SetOneLineText ( COMMENT::CHARCLASS[CharClassToIndex(sCharData.m_emClass)].c_str(), NS_UITEXTCOLOR::WHITE );
	}
}

void CCharacterWindow::SetPremium ()
{
	if ( m_pPremiumText )
	{
		m_pPremiumTextStatic->SetVisibleSingle ( FALSE );

		bool bPREMIUM = m_pGaeaClient->GetCharacter()->m_bPREMIUM;
		if ( bPREMIUM )
		{
			__time64_t tPREMIUM = m_pGaeaClient->GetCharacter()->m_tPREMIUM;
			CTime cTime ( tPREMIUM );

			//	서버 타임으로 변환( TimeZone 계산 )
			m_pGaeaClient->GetConvertServerTime( cTime );

			CString strExpireDate;
			strExpireDate.Format ( "%02d/%02d/%02d", cTime.GetYear()%2000, cTime.GetMonth(), cTime.GetDay () );
			m_pPremiumText->SetOneLineText ( strExpireDate, NS_UITEXTCOLOR::DARKORANGE );
			m_pPremiumTextStatic->SetVisibleSingle ( TRUE );
		}
	}
}

void CCharacterWindow::SetPhoneNumber( const CString & strPhoneNumber )
{
	if( m_pMobileText )
	{
		m_pMobileText->SetOneLineText( strPhoneNumber, NS_UITEXTCOLOR::SILVER );
	}
}

void CCharacterWindow::SetTextTitleCombo( DWORD dwIndex )
{
    // 여기 Index는 선택된 Title의 Index이다.

    std::string strText;
    ENUM_ACTIVITY_CLASS emClass = ACTIVITY_CLASS_SIZE;
    D3DCOLOR dwTextColor;

    // 뺏지없음
    if ( dwIndex == UINT_MAX )
    {
        emClass = ACTIVITY_CLASS_SIZE;
        strText = ID2GAMEWORD( "CAHRWINDOW_TITLE", 0 );
    }
    else
    {
        m_pGaeaClient->GetCharacter()->m_TitleManager.GetTitle( dwIndex, strText, emClass );
        strText = sc::string::format( "<%1%>", ID2GAMEINTEXT( strText.c_str() ) );
    }

    dwTextColor = NS_UITEXTCONTROL::GetActivityColor( emClass );

    m_pTitleTextBox->ClearText();
    m_pTitleTextBox->AddTextNoSplit ( strText.c_str(), dwTextColor );

    UpdateTitlePos();
}

void CCharacterWindow::UpdateTitlePos()
{
    if ( !GLUseFeatures::GetInstance().IsUsingActivity() ) return;
    if ( m_pNameText->GetCount() == 0 ) return;

    static UIRECT rcNameRectBack = m_pNameText->GetLocalPos();

    UIRECT rcGlobalTitleBox = m_pTitleTextBox->GetGlobalPos();
    UIRECT rcLocalTitleBox = m_pTitleTextBox->GetLocalPos();
    
    UIRECT rcGlobalTitleButton = m_pTitleButton->GetGlobalPos();
    UIRECT rcLocalTitleButton = m_pTitleButton->GetLocalPos();
    
    UIRECT rcGlobalRollOver = m_pComboBoxTitleRollOver->GetGlobalPos();
    UIRECT rcLocalRollOver = m_pComboBoxTitleRollOver->GetLocalPos();    
    
    const float fGap = 2.0f;

    float fLenTitle = 0.0f;  // 기본Gap

    // Button Gap
    fLenTitle += rcLocalTitleButton.sizeX + fGap;

    // TitleBox Gap 
    const CBasicTextBox::STEXTPART& sPARTTitle = m_pTitleTextBox->GetTextPart ( 0 );
    float fTitleTextSize = static_cast<float>(sPARTTitle.dequeLine[0].strSize.cx);
    fLenTitle += fTitleTextSize + fGap;

    
    // NameBox 이름 위치
    const CBasicTextBox::STEXTPART& sPART = m_pNameText->GetTextPart ( 0 );
    float fNameTextSize = static_cast<float>(sPART.dequeLine[0].strSize.cx);
    float fNameLeft = ( ( rcNameRectBack.sizeX - fNameTextSize ) * 0.5f ) + rcNameRectBack.left;

    float fLeftTitle = fNameLeft - fLenTitle;

    if ( fLeftTitle < 0.0f ) 
    {
        // NameText size 변경
        UIRECT rcGlobalNameRect = m_pNameText->GetGlobalPos();
        UIRECT rcLocalNameRect = m_pNameText->GetLocalPos();

        // 이미 줄어든 사이즈
        float fSizeGap = rcLocalNameRect.sizeX - rcNameRectBack.sizeX;
        fSizeGap = ( fLeftTitle * 2.0f ) - fSizeGap;
        

        rcGlobalNameRect.left -= fSizeGap;
        rcGlobalNameRect.sizeX += fSizeGap;

        rcLocalNameRect.left -= fSizeGap;
        rcLocalNameRect.sizeX += fSizeGap;

        m_pNameText->SetGlobalPos( rcGlobalNameRect );
        m_pNameText->SetLocalPos( rcLocalNameRect );
        
        // 위치 갱신
        fLeftTitle = 0.0f;        
    }
    else
    {
        // NameText Size 원상 복구
        UIRECT rcGlobalNameRect = m_pNameText->GetGlobalPos();
        UIRECT rcLocalNameRect = m_pNameText->GetLocalPos();
        
        // 이미 줄어든 사이즈
        float fSizeGap = rcLocalNameRect.sizeX - rcNameRectBack.sizeX;

        rcGlobalNameRect.left += fSizeGap;
        rcGlobalNameRect.sizeX = rcNameRectBack.sizeX;

        rcLocalNameRect = rcNameRectBack;


        m_pNameText->SetGlobalPos( rcGlobalNameRect );
        m_pNameText->SetLocalPos( rcLocalNameRect );
    }

    fLeftTitle += fGap;

    rcGlobalTitleButton.left = m_rcGlobalPos.left + fLeftTitle;
    rcLocalTitleButton.left = fLeftTitle;

    rcGlobalRollOver.left = m_rcGlobalPos.left + fLeftTitle;
    rcLocalRollOver.left = fLeftTitle;

    fLeftTitle += rcLocalTitleButton.sizeX + fGap;

    rcGlobalTitleBox.left = m_rcGlobalPos.left + fLeftTitle;
    rcLocalTitleBox.left = fLeftTitle;

    
    m_pTitleTextBox->SetGlobalPos( rcGlobalTitleBox );
    m_pTitleTextBox->SetLocalPos( rcLocalTitleBox );

    m_pTitleButton->SetGlobalPos( rcGlobalTitleButton );
    m_pTitleButton->SetLocalPos( rcLocalTitleButton );   

    m_pComboBoxTitleRollOver->SetGlobalPos( rcGlobalRollOver );
    m_pComboBoxTitleRollOver->SetLocalPos( rcLocalRollOver );
}