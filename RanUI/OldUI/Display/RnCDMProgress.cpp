#include "StdAfx.h"

#include "../../InnerInterface.h"
#include "../../../EngineLib/DeviceMan.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"

#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/Club/GLClubClient.h"
#include "../../../EngineLib/DxTools/DxClubMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"

#include "RnCDMProgress.h"

RnCDMProgress::RnCDMProgress( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
:CUIGroup(pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_pInterface(pInterface)
, m_pBGLineBox(NULL)
, m_fTimer(0)
, m_dwNumberOneClubDbNum(-1)
{

}

RnCDMProgress::~RnCDMProgress()
{

}

void RnCDMProgress::CreateSubControl()
{
	//배경 라인박스
	{
		m_pBGLineBox = new CBasicLineBoxSmart( m_pEngineDevice );
		m_pBGLineBox->CreateSub( this, "RNCDMPROGRESS_BG", UI_FLAG_XSIZE | UI_FLAG_YSIZE, ID_BG_LINEBOX );
		m_pBGLineBox->CreateSubControl( "RNCDMPROGRESS_BG_LINE" );
		RegisterControl( m_pBGLineBox );
	}

	CD3DFontPar* pFont = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

	//1위 클럽
	{
		m_pTheFirstRank = new CBasicTextBox( m_pEngineDevice );
		m_pTheFirstRank->CreateSub( this, "RNCDMPROGRESS_FIRST_RANK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		m_pTheFirstRank->SetFont( pFont );
		m_pTheFirstRank->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
		RegisterControl( m_pTheFirstRank );
	}

	//1위 클럽 마크 이미지
	{
		m_pTheFirstClubImage = new CUIControl( m_pEngineDevice );
		m_pTheFirstClubImage->CreateSub( this, "RNCDMPROGRESS_FIRST_RANK_CLUB_IMAGE", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		m_pTheFirstClubImage->SetUseRender( TRUE );
		RegisterControl( m_pTheFirstClubImage );
	}

	//1위 클럽 이름 텍스트 박스
	{
		m_pTheFirstClubName = new CBasicTextBox( m_pEngineDevice );
		m_pTheFirstClubName->CreateSub( this, "RNCDMPROGRESS_FIRST_RANK_CLUB_NAME", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		m_pTheFirstClubName->SetFont( pFont );
		m_pTheFirstClubName->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
		RegisterControl( m_pTheFirstClubName );
	}

	//1위 클럽 점수
	{
		m_pTheFirstClubScore = new CBasicTextBox( m_pEngineDevice );
		m_pTheFirstClubScore->CreateSub( this, "RNCDMPROGRESS_FIRST_RANK_CLUB_SCORE", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		m_pTheFirstClubScore->SetFont( pFont );
		m_pTheFirstClubScore->SetTextAlign( TEXT_ALIGN_RIGHT | TEXT_ALIGN_CENTER_Y );
		RegisterControl( m_pTheFirstClubScore );
	}

	//MY 클럽
	{
		m_pMyRank = new CBasicTextBox( m_pEngineDevice );
		m_pMyRank->CreateSub( this, "RNCDMPROGRESS_MY_RANK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		m_pMyRank->SetFont( pFont );
		m_pMyRank->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
		RegisterControl( m_pMyRank );
	}

	////MY 클럽 마크 이미지
	//{
	//	m_pMyClubImage = new CSwapImage( m_pEngineDevice );
	//	m_pMyClubImage->CreateSub( this, "RNCDMPROGRESS_MY_CLUB_IMAGE", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	//	RegisterControl( m_pMyClubImage );
	//}
	//MY 클럽 마크 이미지
	{
		m_pMyClubImage = new CUIControl( m_pEngineDevice );
		m_pMyClubImage->CreateSub( this, "RNCDMPROGRESS_MY_CLUB_IMAGE", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		m_pMyClubImage->SetUseRender( TRUE );
		RegisterControl( m_pMyClubImage );
	}

	//MY 클럽 이름 텍스트 박스
	{
		m_pMyClubName = new CBasicTextBox( m_pEngineDevice );
		m_pMyClubName->CreateSub( this, "RNCDMPROGRESS_MY_CLUB_NAME", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		m_pMyClubName->SetFont( pFont );
		m_pMyClubName->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
		RegisterControl( m_pMyClubName );
	}

	//MY 클럽 점수
	{
		m_pMyClubScore = new CBasicTextBox( m_pEngineDevice );
		m_pMyClubScore->CreateSub( this, "RNCDMPROGRESS_MY_SCORE", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		m_pMyClubScore->SetFont( pFont );
		m_pMyClubScore->SetTextAlign( TEXT_ALIGN_RIGHT | TEXT_ALIGN_CENTER_Y );
		RegisterControl( m_pMyClubScore );
	}

	// CDM 상태표시 ex) 클럽데스매치 전투중
	{
		m_pCDMStateTEXT = new CBasicTextBox( m_pEngineDevice );
		m_pCDMStateTEXT->CreateSub( this, "RNCDMPROGRESS_STATE_TEXT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		m_pCDMStateTEXT->SetFont( pFont );
		m_pCDMStateTEXT->SetTextAlign( TEXT_ALIGN_RIGHT | TEXT_ALIGN_CENTER_Y );
		RegisterControl( m_pCDMStateTEXT );
	}

	// CDM 상태표시 ex) 클럽데스매치 전투중
	{
		m_pCDMProgessTime = new CBasicTextBox( m_pEngineDevice );
		m_pCDMProgessTime->CreateSub( this, "RNCDMPROGRESS_STATE_TIME", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		m_pCDMProgessTime->SetFont( pFont );
		m_pCDMProgessTime->SetTextAlign( TEXT_ALIGN_RIGHT | TEXT_ALIGN_CENTER_Y );
		RegisterControl( m_pCDMProgessTime );
	}

    // 1위 탈환 상단 텍스트;
    {
        CD3DFontPar* pTextFont = m_pEngineDevice->LoadFont(_DEFAULT_FONT, 18, _DEFAULT_FONT_SHADOW_FLAG);
        m_pTopChangeText = new CBasicTextBox(m_pEngineDevice);
        m_pTopChangeText->CreateSub(this, "RNCDMPROGRESS_TOP_CHAGNE_TEXT", UI_FLAG_CENTER_X | UI_FLAG_CENTER_Y );
        m_pTopChangeText->SetFont(pTextFont);
        m_pTopChangeText->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
        m_pTopChangeText->SetVisibleSingle(FALSE);
        RegisterControl(m_pTopChangeText);
    }
}

void RnCDMProgress::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
	DragMessage( CUIControl::GetMessageEx() );
	DragUpdate( x, y, m_vPos );
	UpdateTime(fElapsedTime);
}

void RnCDMProgress::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage( ControlID, dwMsg );
}

void RnCDMProgress::RefreashCdmProgress( DWORD	dwNumberOneClubMarkVER,
										DWORD	dwNumberOneClubDbNum,
										char*	szNumberOneName,
										int	nNumberOneScore,
										int	nMyRank,
										DWORD	dwMyScore,
										int	nHour,
										int	nMin,
										int	nSecond )
{

	m_sProgessTime.nHour = nHour;
	m_sProgessTime.nMin = nMin;
    if ( abs(nSecond - static_cast<int>(m_sProgessTime.fSecond)) > 1 ) // 핑차이를 고려해서 1초정도의 차이는 무시한다.;
    {
	    m_sProgessTime.fSecond = static_cast<float>(nSecond);
        SetCDMProgessTime( 	m_sProgessTime.nHour, m_sProgessTime.nMin, m_sProgessTime.fSecond );
    }

    // 1위가 변경되었을때;
    if ( m_dwNumberOneClubDbNum != dwNumberOneClubDbNum && m_dwNumberOneClubDbNum != -1 )
    {
        const char* pSystemMsg =ID2GAMEINTEXT("RNCDM_SYSTEM_MESSAGE");
        if ( pSystemMsg )
        {
            std::string pStr(sc::string::format(pSystemMsg, szNumberOneName));
            //m_pGaeaClient->GetInterface()->PrintSimpleSystemMessage( pStr.c_str(), NS_UITEXTCOLOR::YELLOW, 18, 5.0f);
            m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::YELLOW, pStr);
        }
        m_fTopChangeMessageTime = 0;
    }

    m_dwNumberOneClubDbNum = dwNumberOneClubDbNum;

	const clubmark::DXDATA& sMarkDataNumberOne = m_pGaeaClient->GetClubMark( dwNumberOneClubDbNum, dwNumberOneClubMarkVER );
	SetTheFirstRank( ID2GAMEWORD("RN_CMD_RANK_RANKING_1") );  ;
	SetTheFirstClubName( szNumberOneName );
	SetTheFirstClubScore( nNumberOneScore );
	SetMyRank( nMyRank );

	GLClubClient* pMyClub = m_pGaeaClient->GetMyClub();
	DWORD dwVer = pMyClub->m_dwMarkVER;
	DWORD dwClubDbNum = pMyClub->DbNum();
	const clubmark::DXDATA& sMarkDataMy = m_pGaeaClient->GetClubMark( dwClubDbNum, dwVer );

	SetMyClubName( pMyClub->Name() );
	SetMyClubScore( dwMyScore );

	SetCDMStateTEXT( ID2GAMEWORD("RN_CDM_PROEGSS_TEXT") );
}

void RnCDMProgress::SetTheFirstRank( const CString& strText )
{
	m_pTheFirstRank->ClearText();
	m_pTheFirstRank->AddText( strText, NS_UITEXTCOLOR::WHITE);
}

void RnCDMProgress::SetTheFirstClubName( const CString& strText )
{
	m_pTheFirstClubName->ClearText();
	m_pTheFirstClubName->AddText( strText, NS_UITEXTCOLOR::WHITE);
}

void RnCDMProgress::SetTheFirstClubScore( const DWORD dwScore )
{
	m_pTheFirstClubScore->ClearText();
	CString strText = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( dwScore, 3, "," );
	m_pTheFirstClubScore->AddText( strText + " " + ID2GAMEWORD("RN_CDM_POINT_TEXT"), NS_UITEXTCOLOR::PALEGREEN);
}

void RnCDMProgress::SetMyRank( const int nRank )
{
	m_pMyRank->ClearText();
	CString strText;
	strText.Format( "%d %s",nRank , ID2GAMEWORD("RN_CDM_RANKING_TEXT"));
	m_pMyRank->AddText( strText, NS_UITEXTCOLOR::PALEGREEN);
}

void RnCDMProgress::SetMyClubName( const CString& strText )
{
	m_pMyClubName->ClearText();
	m_pMyClubName->AddText( strText, NS_UITEXTCOLOR::PALEGREEN);
}

void RnCDMProgress::SetMyClubScore( const DWORD dwScore )
{
	m_pMyClubScore->ClearText();
	CString strText = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( dwScore, 3, "," );
	m_pMyClubScore->AddText( strText + " " + ID2GAMEWORD("RN_CDM_POINT_TEXT"), NS_UITEXTCOLOR::PALEGREEN);
}

void RnCDMProgress::SetCDMStateTEXT( const CString& strText )
{
	m_pCDMStateTEXT->ClearText();
	m_pCDMStateTEXT->AddText( strText, NS_UITEXTCOLOR::YELLOW);
}

void RnCDMProgress::SetCDMProgessTime( int nHour, int nMin, float fSecond )
{
	m_pCDMProgessTime->ClearText();
	CString strText;
	strText.Format("%d : %d : %d", nHour, nMin, static_cast<int>(fSecond));
	m_pCDMProgessTime->AddText( strText, NS_UITEXTCOLOR::YELLOW);
    m_fTimer = 0;
}

void RnCDMProgress::UpdateTime( float fElapsedTime )
{
	m_fTimer += fElapsedTime;
	if (m_fTimer > 1.0f)
	{
		m_sProgessTime.fSecond -= m_fTimer;
		if (m_sProgessTime.fSecond < 0.0f)
		{
			m_sProgessTime.fSecond = 59;
			m_sProgessTime.nMin--;
			if (m_sProgessTime.nMin < 0)
			{
				m_sProgessTime.nMin = 59;
				m_sProgessTime.nHour--;
                if ( m_sProgessTime.nHour < 0 )
                {
                    m_sProgessTime.nHour = 0;
                    m_sProgessTime.nMin = 0;
                    m_sProgessTime.fSecond = 0;
                }
			}
		}
        m_fTimer = 0;
		SetCDMProgessTime( 	m_sProgessTime.nHour, m_sProgessTime.nMin, m_sProgessTime.fSecond );
	}
    if ( m_pTopChangeText->IsVisible() )
    {
        m_fTopChangeMessageTime += fElapsedTime;
        if ( m_fTopChangeMessageTime > 5.0f )
        {
            m_pTopChangeText->SetVisibleSingle(FALSE);
            m_fTopChangeMessageTime = 0;
        }
    }
}

HRESULT RnCDMProgress::Render( LPDIRECT3DDEVICEQ pd3dDevice )
{

	if (m_dwNumberOneClubDbNum == -1)
		return S_FALSE;

	HRESULT hr = S_OK;
	m_pTheFirstClubImage->SetVisibleSingle ( FALSE );
	m_pMyClubImage->SetVisibleSingle ( FALSE );
	{
		hr = CUIGroup::Render ( pd3dDevice );
		if ( FAILED ( hr ) )
			return hr;
	}
	m_pTheFirstClubImage->SetVisibleSingle ( TRUE );
	m_pMyClubImage->SetVisibleSingle ( TRUE );

	int nServer = static_cast<int>(m_pGaeaClient->GetCharacter()->m_dwServerID);
	int nID = static_cast<int>(m_dwNumberOneClubDbNum);

	const clubmark::DXDATA& sMarkData = DxClubMan::GetInstance().GetClubData ( m_pEngineDevice->GetDevice(), nServer, nID );


	if (&sMarkData)
	{
		m_pTheFirstClubImage->SetTexturePos ( 0, sMarkData.vTex_1_LU );
		m_pTheFirstClubImage->SetTexturePos ( 1, sMarkData.vTex_2_RU );
		m_pTheFirstClubImage->SetTexturePos ( 3, sMarkData.vTex_3_LD );
		m_pTheFirstClubImage->SetTexturePos ( 2, sMarkData.vTex_4_RD );

		//	만들어야함
		m_pTheFirstClubImage->SetTexture ( sMarkData.pddsTexture );

		hr = S_OK;
		hr = m_pTheFirstClubImage->Render ( pd3dDevice );
	}

	GLClubClient* pMyClub = m_pGaeaClient->GetMyClub();
	DWORD dwVer = pMyClub->m_dwMarkVER;
	DWORD dwClubDbNum = pMyClub->DbNum();
	const clubmark::DXDATA& sMarkDataMy = DxClubMan::GetInstance().GetClubData ( m_pEngineDevice->GetDevice(), nServer, dwClubDbNum );

	if (&sMarkData)
	{
		m_pMyClubImage->SetTexturePos ( 0, sMarkData.vTex_1_LU );
		m_pMyClubImage->SetTexturePos ( 1, sMarkData.vTex_2_RU );
		m_pMyClubImage->SetTexturePos ( 3, sMarkData.vTex_3_LD );
		m_pMyClubImage->SetTexturePos ( 2, sMarkData.vTex_4_RD );

		//	만들어야함
		m_pMyClubImage->SetTexture ( sMarkData.pddsTexture );

		hr = S_OK;
		hr = m_pMyClubImage->Render ( pd3dDevice );
		return hr;
	}
	return hr;
}

void RnCDMProgress::SetVisibleSingle( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle( bVisible );
	if ( !bVisible )
		m_dwNumberOneClubDbNum = -1;
}

void MyRnCDMProgress::CreateUIWindowAndRegisterOwnership()
{
	RnCDMProgress::Create( RNCDM_PROGRESS, "RNCDMPROGRESS_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	RnCDMProgress::CreateSubControl();
	RnCDMProgress::SetAlignFlag( UI_FLAG_CENTER_X );		
	RnCDMProgress::m_pInterface->UiRegisterControl( this, true );
	RnCDMProgress::m_pInterface->UiShowGroupFocus( GetWndID() );
}

void MyRnCDMProgress::RefreashCdmProgress( DWORD dwNumberOneClubMarkVER, DWORD dwNumberOneClubDbNum, char* szNumberOneName, int nNumberOneScore, int nMyRank, DWORD dwMyScore, int nHour, int nMin, int nSecond )
{
	RnCDMProgress::RefreashCdmProgress( dwNumberOneClubMarkVER, dwNumberOneClubDbNum, szNumberOneName, nNumberOneScore, nMyRank, dwMyScore, nHour, nMin, nSecond );

}
