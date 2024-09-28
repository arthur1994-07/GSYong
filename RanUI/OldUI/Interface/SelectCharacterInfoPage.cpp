#include "StdAfx.h"
#include "SelectCharacterInfoPage.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../RanLogicClient/Stage/DxGlobalStage.h"
#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicProgressBar.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CSelectCharacterInfoPage::CSelectCharacterInfoPage(GLGaeaClient* pGaeaClient, COuterInterface* pInterface, EngineDeviceMan* pEngineDevice)
	: CUIOuterWindow(pInterface, pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
	, m_pInfo0Box(NULL)
	, m_pInfo1Box(NULL)
	, m_pInfo2Box(NULL)
	, m_pBrightFrame(NULL)
	, m_pBrightSlider(NULL)
	, m_pEXP(NULL)
	, m_pEXPText(NULL)
	, m_pCharInfoBack(NULL)
    , m_pCountryFlag(NULL)
{
}

CSelectCharacterInfoPage::~CSelectCharacterInfoPage ()
{
}

void CSelectCharacterInfoPage::CreateSubControl ()
{
    if ( m_pGaeaClient->GetServiceProvider() == SP_WORLD_BATTLE )
    {
        CUIControl* pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub( this, "CHARACTER_COUNTRY_FLAG" );
        pControl->SetVisibleSingle( FALSE );
        RegisterControl( pControl );
        m_pCountryFlag = pControl;
        SetCountryInfo( m_pGaeaClient->wbGetData( wb::KOREA_REPUBLIC_OF ) );
    }

	CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
	
	//CreateControl ( "SELECT_CHARACTER_INFO_BACK" );
    if ( m_pGaeaClient->GetServiceProvider() == SP_WORLD_BATTLE )
    {
        m_pInfo0Box = CreateStaticControl( "WB_SELECT_CHARACTER_INFO_0", pFont9, NS_UITEXTCOLOR::WHITE, TEXT_ALIGN_CENTER_X );
        m_pInfo1Box = CreateStaticControl( "WB_SELECT_CHARACTER_INFO_1", pFont9, NS_UITEXTCOLOR::WHITE, TEXT_ALIGN_CENTER_X );
        m_pInfo2Box = CreateStaticControl( "WB_SELECT_CHARACTER_INFO_2", pFont9, NS_UITEXTCOLOR::WHITE, TEXT_ALIGN_CENTER_X );
    }
    else
    {
	    m_pInfo0Box = CreateStaticControl( "SELECT_CHARACTER_INFO_0", pFont9, NS_UITEXTCOLOR::WHITE, TEXT_ALIGN_CENTER_X );
	    m_pInfo1Box = CreateStaticControl( "SELECT_CHARACTER_INFO_1", pFont9, NS_UITEXTCOLOR::WHITE, TEXT_ALIGN_CENTER_X );
	    m_pInfo2Box = CreateStaticControl( "SELECT_CHARACTER_INFO_2", pFont9, NS_UITEXTCOLOR::WHITE, TEXT_ALIGN_CENTER_X );
    }

	{
		CBasicTextBox* pTextBox = NULL;
		pTextBox = CreateStaticControl ( "SELECT_CHARACTER_EXP_STATIC", pFont9, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
		pTextBox->SetOneLineText ( (char*)ID2GAMEWORD ("SELECT_CHARACTER_STATIC", 0)  );
		pTextBox = CreateStaticControl ( "SELECT_CHARACTER_BRIGHT_STATIC", pFont9, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
		pTextBox->SetOneLineText ( (char*)ID2GAMEWORD ("SELECT_CHARACTER_STATIC", 1) );
	}

	{
		CD3DFontPar* pFont8 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 8, D3DFONT_SHADOW | D3DFONT_ASCII );

		CBasicProgressBar * pBar = new CBasicProgressBar(m_pEngineDevice);
		pBar->CreateSub ( this, "SELECT_CHARACTER_EXP" );
		pBar->CreateOverImage ( "SELECT_CHARACTER_EXP_OVERIMAGE" );
		pBar->SetControlNameEx ( "EXP 프로그래스바" );
		RegisterControl ( pBar );
		m_pEXP = pBar;

		m_pEXPText = CreateStaticControl( "SELECT_CHARACTER_EXP_TEXT", pFont8, NS_UITEXTCOLOR::WHITE, TEXT_ALIGN_RIGHT );
	}

	m_pBrightFrame = CreateControl ( "SELECT_CHARACTER_BRIGHTBAR" );        
	m_pBrightSlider = CreateControl ( "SELECT_CHARACTER_BRIGHTBAR_THUMB" );
}

void	CSelectCharacterInfoPage::ResetAll ()
{
	if ( m_pInfo0Box ) m_pInfo0Box->ClearText();
	if ( m_pInfo1Box ) m_pInfo1Box->ClearText();
	if ( m_pInfo2Box ) m_pInfo2Box->ClearText();

	if ( m_pEXPText ) m_pEXPText->ClearText();
	if ( m_pEXP ) m_pEXP->SetPercent( 0.0f );

	if( m_pBrightFrame && m_pBrightSlider)
	{
		const UIRECT& rcFramePos = m_pBrightFrame->GetLocalPos ();
		const UIRECT& rcThumbPos = m_pBrightSlider->GetLocalPos ();

		float fMOVE = (rcFramePos.sizeX - rcThumbPos.sizeX) * 0.5f;
		float fLeft = rcFramePos.left + fMOVE;

		m_pBrightSlider->SetLocalPos( D3DXVECTOR2( fLeft, rcThumbPos.top ) );
	}

	m_pCharInfoBack = NULL;
}

void CSelectCharacterInfoPage::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIOuterWindow::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

    bool bWorldBattle = false;

    if ( m_pGaeaClient->GetServiceProvider() == SP_WORLD_BATTLE )
    {
        if ( m_pInterface->GetCountry() != wb::UNKNOWN_COUNTRY )
            bWorldBattle = true;
    }

    if ( bWorldBattle )
    {
        SetCountryInfo( m_pGaeaClient->wbGetData( m_pInterface->GetCountry() ) );
        m_pCountryFlag->SetVisibleSingle( TRUE );
    }

    DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
	SCHARINFO_LOBBY* pCharInfo = pGlobalStage->GetLobyStage()->GetSelectCharInfo();
	if( pCharInfo )
	{
		if( pCharInfo != m_pCharInfoBack )
		{
            m_pInterface->ShowCharacterClubInfo();

			if( m_pInfo0Box && m_pInfo1Box )
			{
				CString strClassName;
				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					strClassName = CGameTextMan::GetInstance().GetCommentText("CHARCLASS", CharClassToIndex(static_cast<EMCHARCLASS>(pCharInfo->m_emClass))).GetString();
				else
					strClassName = COMMENT::CHARCLASS[CharClassToIndex(static_cast<EMCHARCLASS> (pCharInfo->m_emClass))].c_str();

				CString strCombine;
				strCombine.Format( "%s Lv.%d", pCharInfo->m_ChaName.c_str(), pCharInfo->m_wLevel );
				m_pInfo0Box->SetOneLineText( strCombine );

                if ( bWorldBattle )
                {
                    strCombine.Format( "%s/%s/%s",
                        m_pGaeaClient->wbGetData( m_pInterface->GetCountry() ).m_Alpha3Code.c_str(),
                        ID2GAMEWORD("ACADEMY_NAME", pCharInfo->m_wSchool), strClassName );
                    m_pInfo1Box->SetOneLineText( strCombine );
                }
                else
                {
				    strCombine.Format( "%s/%s", ID2GAMEWORD("ACADEMY_NAME", pCharInfo->m_wSchool), strClassName );
				    m_pInfo1Box->SetOneLineText( strCombine );
                }

				CString strName( m_pGaeaClient->GetMapName( pCharInfo->m_sSaveMapID ) );
				if( strName == _T("(null)") ) strName.Empty();

				strCombine.Format( "%s:%s", ID2GAMEWORD("SELECT_CHARACTER_STATIC", 2), strName );
				m_pInfo2Box->SetOneLineText( strCombine );
			}

			if( m_pEXP )
			{
				__int64 lnNow = pCharInfo->m_sExperience.lnNow;
				__int64 lnMax = pCharInfo->m_sExperience.lnMax;
				if ( !lnMax ) lnMax = 1;
				if ( lnNow <= 0 ) lnNow = 1;
				if( lnNow > lnMax ) lnNow = lnMax;

				float fPercent = float(lnNow) / float(lnMax);
				m_pEXP->SetPercent ( fPercent );

				if( m_pEXPText )
				{
					CString strText;
					//float fPercent(0.0f);
					//if( lnNow )
					//{
					//	fPercent = float(lnNow) / float(lnMax);
					//}
					// 경험치가 100%가 넘어도 화면에는 최대 100까지 표시한다.
					if( fPercent > 1 )
						fPercent = 1;

					strText.Format( "%.2f%%", fPercent * 100.0f );
					m_pEXPText->SetOneLineText( strText, NS_UITEXTCOLOR::WHITE );
				}
			}

			if( m_pBrightFrame && m_pBrightSlider)
			{
				const int nMIN = -100;
				const int nMAX = 100;
				const int nRANGE = nMAX - nMIN;

				int nPercent = pCharInfo->m_nBright;
				// 이미지가 반대로 뒤집혀있어서.
				nPercent = -(nPercent);
				if ( nPercent < nMIN ) nPercent = nMIN;
				if ( nPercent > nMAX ) nPercent = nMAX;

				nPercent += nMAX;

				float fPercent = static_cast<float>(nPercent) / static_cast<float>(nRANGE);

				const UIRECT& rcFramePos = m_pBrightFrame->GetLocalPos();
				const UIRECT& rcThumbPos = m_pBrightSlider->GetLocalPos();

				float fMOVE = (rcFramePos.sizeX - rcThumbPos.sizeX) * fPercent;
				float fLeft = rcFramePos.left + fMOVE;

				m_pBrightSlider->SetLocalPos( D3DXVECTOR2( fLeft, rcThumbPos.top ) );
			}

			m_pCharInfoBack = pCharInfo;
		}
	}
	else
	{
		ResetAll();
	}
}

void CSelectCharacterInfoPage::SetCountryInfo( const wb::COUNTRY_INFO& CountryInfo )
{
    if ( !m_pCountryFlag )
        return;

    m_pCountryFlag->SetTextureName( CountryInfo.m_Pos.m_TextureName.c_str() );
    float fTextureSizeX = static_cast<float>(GLogicData::GetInstance().wbGetTextureWidth());
    float fTextureSizeY = static_cast<float>(GLogicData::GetInstance().wbGetTextureHeight());

    float fFlagSizeX = static_cast<float>(m_pGaeaClient->wbGetFlagWidth());
    float fFlagSizeY = static_cast<float>(m_pGaeaClient->wbGetFlagHeight());

    m_pCountryFlag->SetTexturePos( UIRECT(
        (CountryInfo.m_Pos.m_Left+0.25f)/fTextureSizeX,
        (CountryInfo.m_Pos.m_Top+0.25f)/fTextureSizeY,
        fFlagSizeX/fTextureSizeX,
        fFlagSizeY/fTextureSizeY ) );
}
