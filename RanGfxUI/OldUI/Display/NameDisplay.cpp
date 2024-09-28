#include "StdAfx.h"
#include "NameDisplay.h"
#include "../../../enginelib/DxTools/d3dfont.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxEx.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicProgressBar.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../OldInterface.h"
#include "../../../EngineLib/DxTools/DxClubMan.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogic/GLUseFeatures.h"
#include "../../../SigmaCore/String/StringFormat.h"

#include "../../GfxInterface.h"
// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------
CNameDisplay::CNameDisplay(GLGaeaClient* pGaeaClient, OldInterface* pInterface, EngineDeviceMan* pEngineDevice)
	: CUIGroup(pEngineDevice)
    , m_pGaeaClient ( pGaeaClient )
    , m_pInterface ( pInterface )
	, m_bUsedMemPool( false )
	, m_bDISP( true )
#ifndef DEF_IMPROVE_DISPLAY_NAME
	, m_pNameLineBox(NULL)
	, m_pNameLineBoxCD(NULL)
#endif
	, m_bClub( false )
    , m_bTitle( false )
	, m_pClubMarkBackground( NULL )
	, m_pCountryMark ( NULL )
	, m_bShowCountry ( false )
{
	memset( m_pSchoolMark, 0, sizeof ( CUIControl* ) * nSCHOOLMARK );
	memset( m_pPartyMark, 0, sizeof ( CUIControl* ) * nPARTYMARK );
	memset( m_pBuffIcon, 0, sizeof ( CUIControl* ) * nSkillBuff );	
}

CNameDisplay::~CNameDisplay ()
{
}

void CNameDisplay::INITIALIZE()
{
	m_bDISP = true;
	m_bClub = false;
    m_bTitle = false;

    m_pNickNameBox->ClearText();
    m_pNickNameBox->SetLocalPos( m_rcNameBox );
    m_pNickNameBox->SetVisibleSingle( FALSE );

	m_pNameBox->ClearText();
	m_pNameBox->SetLocalPos( m_rcNameBox );

    m_pTitleBox->ClearText();
    m_pTitleBox->SetLocalPos( m_rcNameBox );
    m_pTitleBox->SetVisibleSingle( FALSE );

	for ( int i = 0; i < nSCHOOLMARK; ++i )
	{
		m_pSchoolMark[i]->SetVisibleSingle( FALSE );
		m_pSchoolMark[i]->SetLocalPos( m_rcSchoolMark[i] );
	}

	for ( int i = 0; i < nPARTYMARK; ++i )
	{
		m_pPartyMark[i]->SetVisibleSingle( FALSE );
		m_pPartyMark[i]->SetLocalPos( m_rcPartyMark[i] );
	}

    for ( int i = 0; i < nSNSMARK; ++i )
    {
        m_pSNSMark[i]->SetVisibleSingle( FALSE );
        m_pSNSMark[i]->SetLocalPos( m_rcSNSMark[i] );
    }

	m_pClubMark->SetVisibleSingle( FALSE );

	if ( m_pCountryMark )
	{
		m_pCountryMark->SetVisibleSingle( FALSE );
		m_pCountryMark->SetLocalPos( m_rcSchoolMark[ 0 ] );
	}

#ifndef DEF_IMPROVE_DISPLAY_NAME
	m_pNameLineBox->SetVisibleSingle( TRUE );
	m_pNameLineBoxCD->SetVisibleSingle( FALSE );
#endif

	for ( int i = 0; i < nSkillBuff; ++i )
	{
		m_pBuffIcon[i]->SetVisibleSingle ( FALSE );
	}

    m_pMiniHP->SetVisibleSingle(FALSE);
}

void CNameDisplay::CreateSubControl ()
{
#ifdef DEF_IMPROVE_DISPLAY_NAME
	CD3DFontPar* pFont8 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG | D3DFONT_SHADOW);
#else
	CD3DFontPar* pFont8 = m_pEngineDevice->LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
#endif

#ifndef DEF_IMPROVE_DISPLAY_NAME
	CBasicLineBoxEx* pLineBox = new CBasicLineBoxEx(m_pEngineDevice);
	pLineBox->CreateSub ( this, "BASIC_LINE_BOX_EX_NAME", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLineBox->CreateBaseBoxName ( "BASIC_NAME_LINE_BOX" );
	RegisterControl ( pLineBox );
	m_pNameLineBox = pLineBox;

	CBasicLineBoxEx* pLineBoxCD = new CBasicLineBoxEx(m_pEngineDevice);
	pLineBoxCD->CreateSub ( this, "BASIC_LINE_BOX_EX_NAME", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLineBoxCD->CreateBaseBoxNameCD ( "BASIC_NAME_LINE_BOX_CD" );
	pLineBoxCD->SetVisibleSingle( FALSE );
	RegisterControl ( pLineBoxCD );
	m_pNameLineBoxCD = pLineBoxCD;
#endif

	CBasicTextBox* pNickNameBox = new CBasicTextBox(m_pEngineDevice);
	pNickNameBox->CreateSub ( this, "NAME_DISPLAY_NICKNAMEBOX", UI_FLAG_XSIZE );
    pNickNameBox->SetFont ( pFont8 );
	pNickNameBox->SetPartInterval ( 1.0f );
	pNickNameBox->SetLineInterval ( 0.0f );
//	pNickNameBox->SetUseRender( TRUE );
//	pNickNameBox->SetDiffuse( D3DCOLOR_ARGB( 128,255,0,0 ) );
	RegisterControl ( pNickNameBox );
	m_pNickNameBox = pNickNameBox;

	CBasicTextBox* pNameBox = new CBasicTextBox(m_pEngineDevice);
	pNameBox->CreateSub ( this, "NAME_DISPLAY_NAMEBOX", UI_FLAG_XSIZE );
    pNameBox->SetFont ( pFont8 );
	pNameBox->SetPartInterval ( 1.0f );
	pNameBox->SetLineInterval ( 0.0f );
//	pNameBox->SetUseRender( TRUE );
//	pNameBox->SetDiffuse( D3DCOLOR_ARGB( 128,0,255,0 ) );
	RegisterControl ( pNameBox );
	m_pNameBox = pNameBox;

	CBasicTextBox* pTitleBox = new CBasicTextBox(m_pEngineDevice);
	pTitleBox->CreateSub ( this, "NAME_DISPLAY_TITLEBOX", UI_FLAG_XSIZE );
    pTitleBox->SetFont ( pFont8 );
	pTitleBox->SetPartInterval ( 1.0f );
	pTitleBox->SetLineInterval ( 0.0f );
//	pTitleBox->SetUseRender( TRUE );
//	pTitleBox->SetDiffuse( D3DCOLOR_ARGB( 128,0,0,255 ) );
	RegisterControl ( pTitleBox );
	m_pTitleBox = pTitleBox;

	{

		CString strBuffIcon[] = 
		{
			"NAME_BUFF_ICON00",
			"NAME_BUFF_ICON01",
			"NAME_BUFF_ICON02",
			"NAME_BUFF_ICON03",
		};

		for ( int i = 0; i < nSkillBuff; ++i )
		{
			m_pBuffIcon[i] = CreateControl ( strBuffIcon[i].GetString(), UI_FLAG_LEFT );
			m_pBuffIcon[i]->SetVisibleSingle ( FALSE );
		}
	}

	m_rcNameBox = m_pNameBox->GetLocalPos(); // MEMO : 기본 로컬포지션 저장

	{
		CString strSchoolMark[] = 
		{
			"NAME_DISPLAY_SCHOOL1",
			"NAME_DISPLAY_SCHOOL2",
			"NAME_DISPLAY_SCHOOL3"
		};

		for ( int i = 0; i < nSCHOOLMARK; ++i )
		{
			m_pSchoolMark[i] = CreateControl ( strSchoolMark[i].GetString(), UI_FLAG_LEFT );
			m_pSchoolMark[i]->SetVisibleSingle ( FALSE );

			m_rcSchoolMark[i] = m_pSchoolMark[i]->GetLocalPos();
		}
	}

	{
		CString strPartyMark[] = 
		{
			"NAME_DISPLAY_PARTY_MEMBER",
			"NAME_DISPLAY_PARTY_MASTER",
			"NAME_DISPLAY_PARTY_MEMBER_TARGET",
			"NAME_DISPLAY_PARTY_MASTER_TARGET",
			"NAME_DISPLAY_PARTY_MASTER_GENERAL"
		};

		for ( int i = 0; i < nPARTYMARK; ++i )
		{
			m_pPartyMark[i] = CreateControl ( strPartyMark[i].GetString(), UI_FLAG_LEFT );
			m_pPartyMark[i]->SetVisibleSingle ( FALSE );

			m_rcPartyMark[i] = m_pPartyMark[i]->GetLocalPos();
		}
	}

    {
        CString strSNSMark[] = 
        {
            "NAME_DISPLAY_SNS_FACEBOOK",
            "NAME_DISPLAY_SNS_TWITTER",
        };

        for ( int i = 0; i < nSNSMARK; ++i )
        {
            m_pSNSMark[i] = CreateControl ( strSNSMark[i].GetString(), UI_FLAG_LEFT );
            m_pSNSMark[i]->SetVisibleSingle ( FALSE );

            m_rcSNSMark[i] = m_pSNSMark[i]->GetLocalPos();
        }
    }

	m_pClubMarkBackground = new CSwapImage( m_pEngineDevice );;
	m_pClubMarkBackground->CreateSub( this, "NAME_DISPLAY_CLUB_MARK_BACKGROUND", UI_FLAG_DEFAULT );
	RegisterControl( m_pClubMarkBackground );
	//m_pClubMarkBackground->SetImage( "WHITE_LINE_BACKGROUND1" );
	m_pClubMarkBackground->SetDiffuse( GLCONST_CHAR::dwClubMarkARGB );
	m_pClubMarkBackground->SetVisibleSingle( FALSE );

	m_pClubMark = CreateControl ( "NAME_DISPLAY_CLUB_MARK", UI_FLAG_DEFAULT );
	m_pClubMark->SetVisibleSingle ( FALSE );

	m_pCountryMark = new CSwapImage( m_pEngineDevice );;
	m_pCountryMark->CreateSub( this, "NAME_DISPLAY_SCHOOL1", UI_FLAG_DEFAULT );
	RegisterControl( m_pCountryMark );
	//m_pCountryMark->SetDiffuse( GLCONST_CHAR::dwClubMarkARGB );
	m_pCountryMark->SetImage( "MINI_COUNTRY_KOR" );
	m_pCountryMark->SetVisibleSingle( FALSE );

    m_pMiniHP = new CBasicProgressBar(m_pEngineDevice);
    m_pMiniHP->CreateSub( this, "NAME_DISPLAY_MINIHP" );
    m_pMiniHP->CreateOverImage( "NAME_DISPLAY_MINIHP_OVERIMAGE" );
    m_pMiniHP->SetAlignFlag( UI_FLAG_CENTER_X );
    m_pMiniHP->SetVisibleSingle( FALSE );
    RegisterControl( m_pMiniHP );
}

CUIControl*	CNameDisplay::CreateControl ( const char* szControl, WORD wAlignFlag )
{
	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub ( this, szControl, wAlignFlag );
	RegisterControl ( pControl );
	return pControl;
}

void CNameDisplay::SetName ( const CString& strName, const D3DCOLOR& dwColor, WORD wSchoolMark, DISP_PARTY emnPartyMark, DWORD dwSNSFlag )
{
    BOOL bPlayer = TRUE;
    BOOL bParty = FALSE;

    // SNS 를 쓰지않는다면 표시해주지 않는다.
    if ( !GLUseFeatures::GetInstance().IsUsingSNS() )
    {
        dwSNSFlag = 0;
    }

	if ( wSchoolMark == NODATA && emnPartyMark == NODATA )
	{
        m_pNameBox->SetTextAlign ( TEXT_ALIGN_CENTER_X );
        m_pNickNameBox->SetTextAlign ( TEXT_ALIGN_CENTER_X );
        bPlayer = FALSE;
	}

    const float fGapMarkName	   = 2.0f;
    const float fGapMarkNameAfter  = 4.0f;
    const float& fLineInterval = m_pNameBox->GetPartInterval ();
    const float fPartyMark	= m_pPartyMark[0]->GetLocalPos ().sizeX;
    const float fSchoolMark = m_pSchoolMark[0]->GetLocalPos ().sizeX + fGapMarkName;
   
    UIRECT rcNickNameBox; 

    //  NameBox 위치 계산용
    long iLineNameTopGap = 0;
    long lLineNameLeftGap = 0;


    long iLingNickNameLength = 0;

    //  클럽 닉네임 및 마크 설정
    if ( m_bClub )
    {
        long lLineNickName;
        const CBasicTextBox::STEXTPART& sPART = m_pNickNameBox->GetTextPart ( 0 );
		if ( false == sPART.dequeLine.empty() )	// _invalid_parameter deque 범의 넘어가서 crash 생기는 부분 수정함, crash 되지는 않고 이상하게 표시될 수도 있음 
		{
			lLineNickName = sPART.dequeLine[0].strSize.cx;
			m_pNickNameBox->SetVisibleSingle( TRUE );

			const UIRECT& rcClubMark = m_pClubMark->GetLocalPos();

			iLineNameTopGap += static_cast<long>( rcClubMark.sizeY + fLineInterval + 1.0f );

			//  유저일 경우
			if ( bPlayer ) 
			{
				rcNickNameBox = UIRECT( rcClubMark.right + fGapMarkName, rcClubMark.top, static_cast<float>(lLineNickName), rcClubMark.sizeY );
				iLingNickNameLength += static_cast<long>( rcClubMark.sizeX + fGapMarkName + lLineNickName );
			}
			//  Pet 및 Monster
			else
			{
				rcNickNameBox = UIRECT( rcClubMark.left, rcClubMark.top, static_cast<float>(lLineNickName), rcClubMark.sizeY );
				iLingNickNameLength = lLineNickName;

			}
		}
    }

    


    // School 마크
	if ( (NODATA != wSchoolMark) && (wSchoolMark < nSCHOOLMARK) )
	{
		m_pSchoolMark[wSchoolMark]->SetVisibleSingle ( TRUE );

        const UIRECT& rcMark = m_pSchoolMark[wSchoolMark]->GetLocalPos();
		D3DXVECTOR2 vPosition = D3DXVECTOR2(rcMark.left, rcMark.top + iLineNameTopGap );

		m_pSchoolMark[wSchoolMark]->SetLocalPos ( vPosition );

		if ( GLUseFeatures::GetInstance().IsUsingSelectCountry() )
		{
			if ( m_bShowCountry )
				m_pCountryMark->SetVisibleSingle( TRUE );

			m_pCountryMark->SetLocalPos ( vPosition );
		}

        lLineNameLeftGap += long ( fSchoolMark );
	}

    UIRECT rcTitleRect;
    UIRECT rcNameRect;
    
    if ( m_bTitle )
    {
        const CBasicTextBox::STEXTPART& sPART = m_pTitleBox->GetTextPart ( 0 );

        rcTitleRect = m_pTitleBox->GetLocalPos();
        rcTitleRect = UIRECT( rcTitleRect.left + lLineNameLeftGap, rcTitleRect.top + iLineNameTopGap, 
                                          static_cast<float>(sPART.dequeLine[0].strSize.cx), rcTitleRect.sizeY );

        lLineNameLeftGap += static_cast<long>(sPART.dequeLine[0].strSize.cx + fGapMarkName);        
    }

    m_pNameBox->AddTextNoSplit ( strName, dwColor ); 
    const CBasicTextBox::STEXTPART& sPART = m_pNameBox->GetTextPart ( 0 );
    
    rcNameRect = UIRECT ( m_rcNameBox.left + lLineNameLeftGap, m_rcNameBox.top + iLineNameTopGap, 
                          static_cast<float>(sPART.dequeLine[0].strSize.cx), m_rcNameBox.sizeY );


    lLineNameLeftGap += sPART.dequeLine[0].strSize.cx;

    if ( bPlayer )
    {
        lLineNameLeftGap += (long)fGapMarkNameAfter;
    }

    if ( PARTY_MASTER == emnPartyMark || PARTY_MASTER_TARGET == emnPartyMark || PARTY_MASTER_GENERAL == emnPartyMark )
    {
        bParty = TRUE;

        m_pPartyMark[emnPartyMark]->SetVisibleSingle ( TRUE );

        const UIRECT& rcMark	 = m_pPartyMark[emnPartyMark]->GetLocalPos();
        m_pPartyMark[emnPartyMark]->SetLocalPos ( D3DXVECTOR2((float)lLineNameLeftGap, rcMark.top + iLineNameTopGap ) );

        lLineNameLeftGap += long ( fPartyMark );
    }

    // bjjy.sns
    {
        for ( int i=0; i<nSNSMARK; ++i )
        {
            if ( dwSNSFlag & (1<<i) )
            {
                m_pSNSMark[i]->SetVisibleSingle ( TRUE );

                long lMarkSizeX	= (long)m_rcSNSMark[i].sizeX;

                const UIRECT& rcMark	 = m_pSNSMark[i]->GetLocalPos();
                m_pSNSMark[i]->SetLocalPos ( D3DXVECTOR2((float)lLineNameLeftGap, rcMark.top + iLineNameTopGap ) );

                lLineNameLeftGap += lMarkSizeX;
            }
        }
    }


    float fGapX = 0.0f;

    if ( lLineNameLeftGap > iLingNickNameLength )
    {
       fGapX  = m_rcNameBox.sizeX - float(lLineNameLeftGap);
    }
    else
    {
        fGapX  = m_rcNameBox.sizeX - float(iLingNickNameLength);
    }
    
 
	const UIRECT& rcLocalPosOld = GetLocalPos ();
	UIRECT rcLocalPosNew( rcLocalPosOld.left, rcLocalPosOld.top,
                          rcLocalPosOld.sizeX - fGapX + 2.0f,                    
                    rcLocalPosOld.sizeY + iLineNameTopGap );

	AlignSubControl ( rcLocalPosOld, rcLocalPosNew );
	SetLocalPos ( rcLocalPosNew );

    if ( m_bTitle )
    {
        m_pTitleBox->SetLocalPos( rcTitleRect );
    }


    if ( (NODATA != wSchoolMark) && (wSchoolMark < nSCHOOLMARK) )
    {
	    m_pNameBox->SetTextAlign ( TEXT_ALIGN_LEFT );
        m_pNameBox->SetLocalPos ( rcNameRect );

        if ( m_bClub )
        {
            m_pNickNameBox->SetLocalPos ( rcNickNameBox );
        }
    }
    else
    {
        rcNameRect.sizeX = m_pNickNameBox->GetLocalPos().sizeX;
        rcNickNameBox.sizeX = m_pNickNameBox->GetLocalPos().sizeX;
        
        m_pNameBox->SetLocalPos ( rcNameRect );
        
        if ( m_bClub )
        {
            m_pNickNameBox->SetLocalPos ( rcNickNameBox );
        }
    }

	SetGlobalPos ( rcLocalPosNew );
}

void CNameDisplay::SetClubName ( const CString& strName, const D3DCOLOR& dwColor )
{
	if ( m_pNameBox->GetCount () )
	{
		GASSERT ( 0 && "클럽 이름 설정 순서가 적절하지 않습니다." );
		return ;
	}

	m_pNickNameBox->AddTextNoSplit( strName, dwColor );

	m_pClubMark->SetVisibleSingle ( TRUE );
	m_bClub = true; // MEMO : 클럽으로 설정
}

void CNameDisplay::SetOwnerName( const CString& strName, const D3DCOLOR& dwColor )
{
	if ( m_pNameBox->GetCount () )
	{
		GASSERT ( 0 && "주인 이름 설정 순서가 적절하지 않습니다." );
		return ;
	}

	m_pNickNameBox->AddTextNoSplit( strName, dwColor );
	m_bClub = true; // MEMO : 클럽으로 설정
}

void CNameDisplay::SetTitleName( const CString& strTitle, const D3DCOLOR& dwColor )
{
	if ( m_pTitleBox->GetCount () )
	{
		GASSERT ( 0 && "Title 이름 설정 순서가 적절하지 않습니다." );
		return ;
	}

    if ( strTitle == "" )
    {
        m_pTitleBox->ClearText();
        m_bTitle = false; 
    }
    else
    {
        std::string strText = sc::string::format( "<%1%>", strTitle );

	    m_pTitleBox->AddTextNoSplit( strText.c_str(), dwColor );
	    m_bTitle = true; 
    }
}

void CNameDisplay::SetBuffIcon( int nIndex, bool bVisible )
{
	if ( bVisible && !m_pBuffIcon[nIndex]->IsVisible() ) 
	{
		m_pBuffIcon[nIndex]->SetVisibleSingle( TRUE );
	}
	else if ( !bVisible && m_pBuffIcon[nIndex]->IsVisible() ) 
	{
		m_pBuffIcon[nIndex]->SetVisibleSingle( FALSE );
	}
}

void CNameDisplay::SetHP ()
{
    if ( m_pNameBox->GetCount () == 0 )
    {
        GASSERT ( 0 && "HP 설정 순서가 적절하지 않습니다." );
        return ;
    }

    if ( !m_pMiniHP )
    {
        return;
    }

    m_pMiniHP->SetVisibleSingle( TRUE );

    const UIRECT& rcHPPos   = m_pMiniHP->GetLocalPos();
    const UIRECT& rcNamePos = m_pNameBox->GetLocalPos();

    const UIRECT& rcLocalPosOld = GetLocalPos ();
    UIRECT rcLocalPosNew;
    
    rcLocalPosNew.left  = rcLocalPosOld.left;
    rcLocalPosNew.top   = rcLocalPosOld.top;
    rcLocalPosNew.sizeX = (rcLocalPosOld.sizeX > rcHPPos.sizeX) ? rcLocalPosOld.sizeX : rcHPPos.sizeX;
    rcLocalPosNew.sizeY = (rcNamePos.bottom + rcHPPos.sizeY < rcLocalPosOld.sizeY ) ? rcLocalPosOld.sizeY : rcHPPos.sizeY + rcNamePos.bottom;

    rcLocalPosNew.right  = rcLocalPosOld.left + rcLocalPosOld.sizeX;
    rcLocalPosNew.bottom = rcLocalPosOld.top  + rcLocalPosOld.sizeY;

    m_pMiniHP->AlignSubControl( rcLocalPosNew, rcLocalPosNew );
    m_pMiniHP->SetLocalPos  ( D3DXVECTOR2(rcHPPos.left, rcNamePos.bottom) );
    m_pMiniHP->SetGlobalPos ( D3DXVECTOR2(rcHPPos.left, rcNamePos.bottom) );

    if ( rcLocalPosOld.sizeX < rcLocalPosNew.sizeX )
    {
        float fHalfGapX = (rcLocalPosNew.sizeX - rcLocalPosOld.sizeX)*0.5f;

        m_pNickNameBox->AddPosition( D3DXVECTOR2(fHalfGapX, 0.0f) );
        m_pTitleBox->AddPosition( D3DXVECTOR2(fHalfGapX, 0.0f) );

        m_pClubMark->AddPosition( D3DXVECTOR2(fHalfGapX, 0.0f) );

#ifndef DEF_IMPROVE_DISPLAY_NAME
        m_pNameLineBox->AddPosition( D3DXVECTOR2(fHalfGapX, 0.0f) );
        m_pNameLineBoxCD->AddPosition( D3DXVECTOR2(fHalfGapX, 0.0f) );
#endif
        m_pNameBox->AddPosition( D3DXVECTOR2(fHalfGapX, 0.0f) );
        m_pTitleBox ->AddPosition( D3DXVECTOR2(fHalfGapX, 0.0f) );

        for ( int i = 0; i < nSCHOOLMARK; ++i )
        {
            m_pSchoolMark[i]->AddPosition( D3DXVECTOR2(fHalfGapX, 0.0f) );
        }

        for ( int i = 0; i < nPARTYMARK; ++i )
        {
            m_pPartyMark[i]->AddPosition( D3DXVECTOR2(fHalfGapX, 0.0f) );
        }

        for ( int i = 0; i < nSNSMARK; ++i )
        {
            m_pSNSMark[i]->AddPosition( D3DXVECTOR2(fHalfGapX, 0.0f) );
        }

        for ( int i = 0; i < nSkillBuff; ++i )
        {
            m_pBuffIcon[i]->AddPosition( D3DXVECTOR2(fHalfGapX, 0.0f) );;
        }

		m_pCountryMark->AddPosition( D3DXVECTOR2(fHalfGapX, 0.0f) );
    }

    SetLocalPos  ( rcLocalPosNew );
    SetGlobalPos ( rcLocalPosNew );
}

void CNameDisplay::SetShowCountryMark ( bool bShowCountry )
{
	if ( GLUseFeatures::GetInstance().IsUsingSelectCountry() )
	{
		m_bShowCountry = bShowCountry;

		Country::GLCountryManClient* pCountryMan =
			Country::GLCountryManClient::Instance();
		if ( NULL == pCountryMan )
			return;

		Country::GLCountry* pCountry =
			pCountryMan->GetCountryUsingID( m_sDISP_INFO.m_dwCountryID );
		if ( NULL == pCountry )
			return;

		if ( m_pCountryMark )
		{
			m_pCountryMark->SetImage( pCountry->GetCountryMiniImageID().c_str() );

			if ( m_bShowCountry )
				m_pCountryMark->SetVisibleSingle( TRUE );
		}
	}
}

HRESULT CNameDisplay::Render ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT( pd3dDevice );	

//	if ( 이 케릭터가 현재 이름 표시 끔 상태면 )
//		return S_OK;

	if( !m_sDISP_INFO.IsCLUB() )
        return CUIGroup::Render ( pd3dDevice );

	HRESULT hr = S_OK;

	// MEMO : 아래 코드는 테스트 용입니다.
	// 나중에 메세지를 받을 때만 변경되도록 수정해야 합니다.
#ifndef DEF_IMPROVE_DISPLAY_NAME
	m_pNameLineBox->SetVisibleSingle( FALSE);
	m_pNameLineBoxCD->SetVisibleSingle( FALSE );
	if( IsCDCERTIFY() )
        m_pNameLineBoxCD->SetVisibleSingle( TRUE );
	else
        m_pNameLineBox->SetVisibleSingle( TRUE );
#endif

	m_pClubMark->SetVisibleSingle ( FALSE );
	{
		hr = CUIGroup::Render ( pd3dDevice );
		if ( FAILED ( hr ) )
            return hr;
	}
	m_pClubMark->SetVisibleSingle ( TRUE );

	const int nSERVER = m_pGaeaClient->GetCharacter()->m_dwServerID;
	const clubmark::DXDATA& sMarkData = DxClubMan::GetInstance().GetClubData(
        pd3dDevice,
        nSERVER,
        m_sDISP_INFO.m_dwCLUB );

	m_pClubMark->SetTexturePos ( 0, sMarkData.vTex_1_LU );
	m_pClubMark->SetTexturePos ( 1, sMarkData.vTex_2_RU );
	m_pClubMark->SetTexturePos ( 3, sMarkData.vTex_3_LD );
	m_pClubMark->SetTexturePos ( 2, sMarkData.vTex_4_RD );

	const UIRECT& rcGlobalPos = m_pClubMark->GetGlobalPos ();
	m_pClubMark->SetGlobalPos(
        UIRECT(
            ceil(rcGlobalPos.left),
            ceil(rcGlobalPos.top),
            ceil(rcGlobalPos.sizeX),
            ceil(rcGlobalPos.sizeY) ) );

	//	만들어야함
	m_pClubMark->SetTexture ( sMarkData.pddsTexture );
	hr = m_pClubMark->Render ( pd3dDevice );

	m_pClubMarkBackground->SetVisibleSingle( TRUE );
	m_pClubMarkBackground->Render( pd3dDevice );

	return hr;
}

//void	CNameDisplay::SetNameColor ( const D3DCOLOR& dwColor )
//{
//	if ( m_pNameBox->GetCount () > 1 )
//	{
//		m_pNameBox->SetUseTextColor ( 1, TRUE );
//		m_pNameBox->SetTextColor ( 1, dwColor );
//	}
//}
//
//void	CNameDisplay::SetClubNameColor ( const D3DCOLOR& dwColor )
//{
//	if ( m_pNameBox->GetCount () > 1 )
//	{
//		m_pNameBox->SetUseTextColor ( 0, TRUE );
//		m_pNameBox->SetTextColor ( 0, dwColor );
//	}
//}

bool CNameDisplay::DIFFERENT ( const CROWREN &sINFO )
{
	return m_sDISP_INFO.DIFFERENT(sINFO);
}

void CNameDisplay::SET_INFO ( const CROWREN &sINFO, const char *szOwnerName )	
{ 
    m_sDISP_INFO = sINFO;

    DWORD dwNICK_COLOR( UINT_MAX );
    if ( sINFO.m_dwTYPE & TYPE_TARGET ) 	dwNICK_COLOR = sINFO.m_dwCOLOR;
    if ( sINFO.m_dwTYPE & TYPE_CLUBBATTLE ) dwNICK_COLOR = sINFO.m_dwCOLOR;

    if ( sINFO.m_emCROW == CROW_PET )
    {
        if ( szOwnerName )
            SetOwnerName( szOwnerName, NS_UITEXTCOLOR::SILVER );
    }
    else if ( sINFO.m_emCROW == CROW_SUMMON )
    {
        if ( szOwnerName && !sINFO.m_bSIMPLE )
            SetOwnerName( szOwnerName, sINFO.m_dwCOLOR );
    }

    if ( sINFO.IsCLUB() )
    {
        if ( sINFO.m_szNICK[0] != _T('\0') )
        {
            SetClubName( sINFO.m_szNICK, dwNICK_COLOR );
        }
        else
        {
            SetClubName( " ", dwNICK_COLOR );
        }
    }

    DWORD dwTextColor = NS_UITEXTCONTROL::GetActivityColor( sINFO.m_nTitleClass );

    SetTitleName( sINFO.m_szTitle, dwTextColor );
    SetName( sINFO.m_szNAME, sINFO.m_dwCOLOR, sINFO.m_wSCHOOL, sINFO.m_emPARTY, sINFO.m_dwSNS );

    if ( sINFO.m_bHP )
    {
        SetHP ();
    }

	if ( GLUseFeatures::GetInstance().IsUsingSelectCountry() )
	{
		Country::GLCountryManClient* pCountryMan =
			Country::GLCountryManClient::Instance();
		if ( NULL == pCountryMan )
			return;

		Country::GLCountry* pCountry =
			pCountryMan->GetCountryUsingID( sINFO.m_dwCountryID );
		if ( NULL == pCountry )
			return;

		if ( m_pCountryMark )
			m_pCountryMark->SetImage( pCountry->GetCountryMiniImageID().c_str() );
	}
}

void CNameDisplay::UPDATE ( const CROWREN &sINFO )
{
	m_sDISP_INFO.m_dwCOUNT	= sINFO.m_dwCOUNT;
	m_sDISP_INFO.m_vPOS		= sINFO.m_vPOS;
    m_sDISP_INFO.m_fPERHP   = sINFO.m_fPERHP;

    m_pMiniHP->SetPercent( sINFO.m_fPERHP );
    m_pMiniHP->UpdateProgress();

    if ( !IsPMARKET() )
    {
        for ( int i = 0; i < SKILL::EMACTION_TYPE_NSIZE; ++i ) 
        {
            SetBuffIcon( i, sINFO.m_bActionType[i] );
        }   
    }
}
