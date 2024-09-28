#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"
#include "../../../EngineLib/GUInterface/BasicProgressBar.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"

#include "../../../RanLogic/GLogicEx.h"
#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../InnerInterface.h"
#include "BasicInfoView.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CBasicInfoView::CBasicInfoView(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
	, m_pInterface(pInterface)
    , m_pGaeaClient( pGaeaClient)
    , m_pHP( NULL )
    , m_pMP( NULL )
    , m_pSP( NULL )
    , m_pEXP( NULL )
    , m_pCP( NULL )
    , m_pHPText( NULL )
    , m_pMPText( NULL )
    , m_pSPText( NULL )
    , m_pEXPText( NULL )
    , m_pCPText( NULL )
    , m_bFirstGap( FALSE )
    , m_nHPBACK( -1 )
    , m_nMPBACK( -1 )
    , m_nSPBACK( -1 )
    , m_wCPBACK( USHRT_MAX )
    , m_lEXPBACK( -1 )
	, m_wLevelBACK( -1 )
{
}

CBasicInfoView::~CBasicInfoView()
{
}

void CBasicInfoView::CreateSubControl()
{
	CD3DFontPar* pFont8 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 8, D3DFONT_SHADOW | D3DFONT_ASCII );

	m_pHP = new CBasicProgressBar(m_pEngineDevice);
	m_pHP->CreateSub( this, "BASIC_INFO_VIEW_HP" );
	m_pHP->CreateOverImage( "BASIC_INFO_VIEW_HP_OVERIMAGE" );
	m_pHP->SetControlNameEx( "HP 프로그래스바" );
	RegisterControl( m_pHP );

	m_pMP = new CBasicProgressBar(m_pEngineDevice);
	m_pMP->CreateSub( this, "BASIC_INFO_VIEW_MP" );
	m_pMP->CreateOverImage( "BASIC_INFO_VIEW_MP_OVERIMAGE" );
	m_pMP->SetControlNameEx( "MP 프로그래스바" );
	RegisterControl( m_pMP );

	m_pSP = new CBasicProgressBar(m_pEngineDevice);
	m_pSP->CreateSub( this, "BASIC_INFO_VIEW_SP" );
	m_pSP->CreateOverImage( "BASIC_INFO_VIEW_SP_OVERIMAGE" );
	m_pSP->SetControlNameEx( "SP 프로그래스바" );
	RegisterControl( m_pSP );

	m_pEXP = new CBasicProgressBar(m_pEngineDevice);
	m_pEXP->CreateSub( this, "BASIC_INFO_VIEW_EXP" );
	m_pEXP->CreateOverImage( "BASIC_INFO_VIEW_EXP_OVERIMAGE" );
	m_pEXP->SetControlNameEx( "EXP 프로그래스바" );
	RegisterControl( m_pEXP );

	m_pCP = new CBasicProgressBar(m_pEngineDevice);
	m_pCP->CreateSub( this, "BASIC_INFO_VIEW_CP" );
	m_pCP->CreateOverImage( "BASIC_INFO_VIEW_CP_OVERIMAGE" );
	m_pCP->SetControlNameEx( "CP 프로그래스바" );
	RegisterControl( m_pCP );

	m_pHPText = new CBasicTextBox(m_pEngineDevice);
	m_pHPText->CreateSub( this, "BASIC_INFO_VIEW_HP_TEXT", UI_FLAG_DEFAULT, HP_TEXT );
    m_pHPText->SetFont( pFont8 );
	m_pHPText->SetTextAlign( TEXT_ALIGN_RIGHT );
	m_pHPText->SetControlNameEx( "HP 텍스트박스" );	
	RegisterControl( m_pHPText );

	m_pMPText = new CBasicTextBox(m_pEngineDevice);
	m_pMPText->CreateSub( this, "BASIC_INFO_VIEW_MP_TEXT", UI_FLAG_DEFAULT, MP_TEXT );
    m_pMPText->SetFont( pFont8 );
	m_pMPText->SetTextAlign( TEXT_ALIGN_RIGHT );
	m_pMPText->SetControlNameEx( "MP 텍스트박스" );	
	RegisterControl( m_pMPText );

	m_pSPText = new CBasicTextBox(m_pEngineDevice);
	m_pSPText->CreateSub( this, "BASIC_INFO_VIEW_SP_TEXT", UI_FLAG_DEFAULT, SP_TEXT );
    m_pSPText->SetFont( pFont8 );
	m_pSPText->SetTextAlign( TEXT_ALIGN_RIGHT );
	m_pSPText->SetControlNameEx( "SP 텍스트박스" );	
	RegisterControl( m_pSPText );

	m_pEXPText = new CBasicTextBox(m_pEngineDevice);
	m_pEXPText->CreateSub( this, "BASIC_INFO_VIEW_EXP_TEXT", UI_FLAG_DEFAULT, EXP_TEXT );
    m_pEXPText->SetFont( pFont8 );
	m_pEXPText->SetTextAlign( TEXT_ALIGN_RIGHT );
	m_pEXPText->SetControlNameEx( "EXP 텍스트박스" );	
	RegisterControl( m_pEXPText );

	m_pCPText = new CBasicTextBox(m_pEngineDevice);
	m_pCPText->CreateSub( this, "BASIC_INFO_VIEW_CP_TEXT", UI_FLAG_DEFAULT, CP_TEXT );
	m_pCPText->SetFont( pFont8 );
	m_pCPText->SetTextAlign( TEXT_ALIGN_RIGHT );
	m_pCPText->SetControlNameEx( "CP 텍스트박스" );	
	RegisterControl( m_pCPText );
}

void CBasicInfoView::SetHP( const int nNOW, const int nMAX )
{
	GASSERT( m_pHPText && m_pHP );

	if ( m_nHPBACK != nNOW )
	{
		m_nHPBACK = nNOW;

		CString strTemp;
		strTemp.Format( "%d", nNOW );
		m_pHPText->SetOneLineText( strTemp, NS_UITEXTCOLOR::WHITE );

		const float fPercent = float(nNOW) / float(nMAX);
		m_pHP->SetPercent( fPercent );
	}
}

void CBasicInfoView::SetMP( const int nNOW, const int nMAX )
{
	GASSERT( m_pMPText && m_pMP );

	if ( m_nMPBACK != nNOW )
	{
		m_nMPBACK = nNOW;

		CString strTemp;
		strTemp.Format( "%d", nNOW );
		m_pMPText->SetOneLineText( strTemp, NS_UITEXTCOLOR::WHITE );

		const float fPercent = float(nNOW) / float(nMAX);
		m_pMP->SetPercent( fPercent );
	}
}

void CBasicInfoView::SetSP( const int nNOW, const int nMAX )
{
	GASSERT( m_pSPText && m_pSP );

	if ( m_nSPBACK != nNOW )
	{
		m_nSPBACK = nNOW;
        
		CString strTemp;
		strTemp.Format( "%d", nNOW );
		m_pSPText->SetOneLineText( strTemp, NS_UITEXTCOLOR::WHITE );

		const float fPercent = float(nNOW) / float(nMAX);
		m_pSP->SetPercent( fPercent );
	}
}

void CBasicInfoView::SetEXP( LONGLONG lNOW, LONGLONG lMAX, WORD wLevel )
{
	GASSERT( m_pEXPText && m_pEXP );

	if ( m_lEXPBACK != lNOW || m_wLevelBACK != wLevel )
	{
		m_lEXPBACK = lNOW;
		m_wLevelBACK = wLevel;

		CString strTemp;
		float fPercent(0.0f);
		if( lNOW )
		{
			fPercent = float(lNOW) / float(lMAX);	
		}

		if( fPercent > 1 ) fPercent = 1;

		strTemp.Format( "%2.2f%%", fPercent * 100.0f );
		m_pEXPText->SetOneLineText( strTemp, NS_UITEXTCOLOR::WHITE );

		m_pEXP->SetPercent( fPercent );
	}
}

void CBasicInfoView::SetCP( WORD wNOW, WORD wMAX )
{
	GASSERT( m_pCPText && m_pCP );

	// CP를 사용하지 않음
	if( !GLCONST_CHAR::bCOMBAT_POINT ) 
	{
		m_pCP->SetVisibleSingle(FALSE);
		m_pCPText->SetVisibleSingle(FALSE);
		return;
	}

	if ( m_wCPBACK != wNOW )
	{
		m_wCPBACK = wNOW;

		CString strTemp;
		strTemp.Format( "%d", wNOW );
		m_pCPText->SetOneLineText( strTemp, NS_UITEXTCOLOR::WHITE );

		const float fPercent = float(wNOW) / float(wMAX);
		m_pCP->SetPercent( fPercent );
	}
}

void CBasicInfoView::CheckMouseState()
{
	const DWORD dwMsg = GetMessageEx();
	if ( CHECK_MOUSE_IN( dwMsg ) )
	{
		if( UIMSG_LB_DUP & dwMsg )
		{
			// 윈도우를 디폴트 포지션으로 이동
			AddMessageEx( UIMSG_BASICINFOVIEW_LBDUP );
			SetDiffuse( D3DCOLOR_ARGB ( 0xFF, 0xFF, 0xFF, 0xFF ) );
			return ;
		}
		
		if ( dwMsg & UIMSG_LB_DOWN )
		{
			//	단독 컨트롤로 등록하고,
			SetExclusiveControl();	

			//	최초 포지션 갭을 기록한다.
			if ( !m_bFirstGap )
			{
				UIRECT rcPos = GetGlobalPos();
				m_vGap.x = m_PosX - rcPos.left;
				m_vGap.y = m_PosY - rcPos.top;
				m_bFirstGap = TRUE;

				SetDiffuse( D3DCOLOR_ARGB( 0xB4, 0xFF, 0xFF, 0xFF ) );
			}
		}
		else if ( CHECK_LB_UP_LIKE( dwMsg ) )
		{
			//	단독 컨트롤을 해제하고
			ResetExclusiveControl();
			//	최초 포지션갭을 해제한다.
			m_bFirstGap = FALSE;				

			SetDiffuse( D3DCOLOR_ARGB( 0xFF, 0xFF, 0xFF, 0xFF ) );
		}
	}
	//	컨트롤 영역밖에서 버튼을 떼는 경우가 발생하더라도
	else if ( CHECK_LB_UP_LIKE( dwMsg ) )		
	{								
		//	단독 컨트롤을 해제하고,
		ResetExclusiveControl();
		//	최초 포지션갭을 해제한다.
		m_bFirstGap = FALSE;					

		SetDiffuse( D3DCOLOR_ARGB( 0xFF, 0xFF, 0xFF, 0xFF ) );
	}
}


void CBasicInfoView::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{	
	m_bCHECK_MOUSE_STATE = false;

	m_PosX = x;
	m_PosY = y;

	CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( IsExclusiveSelfControl() )
	{
		SetGlobalPos( D3DXVECTOR2( x - m_vGap.x, y - m_vGap.y ) );
	}

	const GLCHARLOGIC& sCharData = m_pGaeaClient->GetCharacterLogic();
	SetHP( sCharData.m_sHP.nNow, sCharData.m_sHP.nMax );
	SetMP( sCharData.m_sMP.nNow, sCharData.m_sMP.nMax );
	SetSP( sCharData.m_sSP.nNow, sCharData.m_sSP.nMax );
	SetEXP( sCharData.m_sExperience.lnNow, sCharData.m_sExperience.lnMax, sCharData.m_wLevel );
	SetCP( sCharData.m_sCP.wNow, sCharData.m_sCP.wMax );
}

void CBasicInfoView::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	if ( !m_bCHECK_MOUSE_STATE )
	{
		CheckMouseState();
		m_bCHECK_MOUSE_STATE = true;
	}

	switch ( ControlID )
	{
	case HP_TEXT:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
			{
				const GLCHARLOGIC& sCharData = m_pGaeaClient->GetCharacterLogic();

				CString strText;
				strText.Format ( "%d/%d", sCharData.m_sHP.nNow, sCharData.m_sHP.nMax );
	
				m_pInterface->SHOW_COMMON_LINEINFO( strText, NS_UITEXTCOLOR::DEFAULT );
			}
		}
		break;

	case MP_TEXT:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				const GLCHARLOGIC& sCharData = m_pGaeaClient->GetCharacterLogic();

				CString strText;
				strText.Format ( "%d/%d", sCharData.m_sMP.nNow, sCharData.m_sMP.nMax );
	
				m_pInterface->SHOW_COMMON_LINEINFO( strText, NS_UITEXTCOLOR::DEFAULT );
			}
		}
		break;

	case SP_TEXT:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
			{
				const GLCHARLOGIC& sCharData = m_pGaeaClient->GetCharacterLogic();

				CString strText;
				strText.Format( "%d/%d", sCharData.m_sSP.nNow, sCharData.m_sSP.nMax );
	
				m_pInterface->SHOW_COMMON_LINEINFO( strText, NS_UITEXTCOLOR::DEFAULT );
			}
		}
		break;

	case EXP_TEXT:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
			{
				const GLCHARLOGIC& sCharData = m_pGaeaClient->GetCharacterLogic();

				//std::string strCurExp = sc::string::format( "%I64d", sCharData.m_sExperience.lnNow );
				//std::string strMaxExp = sc::string::format( "%I64d", sCharData.m_sExperience.lnMax );
				std::string strCurExp;
				std::string strMaxExp = sc::string::format( "%1%", sCharData.m_sExperience.lnMax );

				// 보유 경험치가 최대 경험치를 넘더라도 최대값으로 표시
				if( sCharData.m_sExperience.lnNow > sCharData.m_sExperience.lnMax )
					strCurExp = sc::string::format( "%1%", sCharData.m_sExperience.lnMax );
				else
					strCurExp = sc::string::format( "%1%", sCharData.m_sExperience.lnNow );
				
				char szCurExp[30] = {0, };
				char szMaxExp[30] = {0, };
			
				sc::string::AddCommaToNum( strCurExp.c_str(), szCurExp );
				sc::string::AddCommaToNum( strMaxExp.c_str(), szMaxExp );

				CString strText;
				strText.Format("%s/%s", szCurExp, szMaxExp );
				//CString strText;
				//strText.Format( "%I64d/%I64d", sCharData.m_sExperience.lnNow, sCharData.m_sExperience.lnMax );
			
				m_pInterface->SHOW_COMMON_LINEINFO( strText, NS_UITEXTCOLOR::DEFAULT );
			}
		}
		break;
	}
}

MyBasicInfoView::MyBasicInfoView( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CBasicInfoView( pGaeaClient, pInterface, pEngineDevice )
{
}

void MyBasicInfoView::CreateUIWindowAndRegisterOwnership()
{
    CBasicInfoView::Create( BASIC_INFO_VIEW, "BASIC_INFO_VIEW" );
    CBasicInfoView::CreateSubControl();	
    CBasicInfoView::m_pInterface->UiRegisterControl( this, true );	
    CBasicInfoView::m_pInterface->UiShowGroupFocus( BASIC_INFO_VIEW );
}

const UIRECT& MyBasicInfoView::GetUIWindowGlobalPos()
{
    return CBasicInfoView::GetGlobalPos();
}

void MyBasicInfoView::SetUIWindowGlobalPos( const D3DXVECTOR2& vPos )
{
    CBasicInfoView::SetGlobalPos( vPos );
}