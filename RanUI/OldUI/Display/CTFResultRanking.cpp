#include "StdAfx.h"
#include "./CTFResultRanking.h"

//#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/CapturedTheField/GLCaptureTheFieldClient.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"

#include "../../../EngineLib/DxTools/d3dfont.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CCTFResultRanking::CCTFResultRanking ( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CUIGroup(pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_pInterface(pInterface)
, m_pTextBase_Rank(NULL)
, m_pTextBase_School(NULL)
, m_pTextBase_Class(NULL)
, m_pTextBase_Name(NULL)
, m_pTextBase_Damage(NULL)
, m_pTextBase_Heal(NULL)
, m_pTextBase_Kill(NULL)
, m_pTextBase_Death(NULL)
, m_pTextBase_Rebirth(NULL)
, m_pTextBase_Contribution(NULL)
, m_pMyRank(NULL)
, m_pScroll(NULL)
, m_nScrollPos(-1)
, m_emSortType(EMMAINSORTTYPE_TOTAL)
{
	//Blank
}

CCTFResultRanking::~CCTFResultRanking()
{
	m_vecSlot.clear();
}

void CCTFResultRanking::CreateSubControl ()
{
	std::string strBaseControl = GetControlNameEx();
	const int nAlignCenter = TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y;	

	//! 라인박스
	{
		CBasicLineBoxSmart* pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
		pLineBox->CreateSub( this, "CTF_RESULT_RANKING_LINEBOX", UI_FLAG_DEFAULT );
		pLineBox->CreateSubControl( "CTF_RESULT_RANKING_LINEBOX_TEXINFO" );
		RegisterControl( pLineBox );

		//! 라인
		CreateColorBackgroundControl( "CTF_RESULT_RANKING_LINEBOX_HEAD", NS_UITEXTCOLOR::WHITE );
	}

	//! 기본 텍스트
	{
		CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
		CD3DFontPar* pFont_Shadow = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

		m_pTextBase_Rank		 = CreateStaticControl("CTF_RESULT_TEXT_BASE_RANK", pFont_Shadow, nAlignCenter);
		m_pTextBase_School		 = CreateStaticControl("CTF_RESULT_TEXT_BASE_SCHOOL", pFont_Shadow, nAlignCenter);
		m_pTextBase_Class		 = CreateStaticControl("CTF_RESULT_TEXT_BASE_CLASS", pFont_Shadow, nAlignCenter);
		m_pTextBase_Name		 = CreateStaticControl("CTF_RESULT_TEXT_BASE_NAME", pFont_Shadow, nAlignCenter);
		m_pTextBase_Damage		 = CreateStaticControl("CTF_RESULT_TEXT_BASE_DAMAGE", pFont_Shadow, nAlignCenter);
		m_pTextBase_Heal		 = CreateStaticControl("CTF_RESULT_TEXT_BASE_HEAL", pFont_Shadow, nAlignCenter);
		m_pTextBase_Kill		 = CreateStaticControl("CTF_RESULT_TEXT_BASE_KILL", pFont_Shadow, nAlignCenter);
		m_pTextBase_Death		 = CreateStaticControl("CTF_RESULT_TEXT_BASE_DEATH", pFont_Shadow, nAlignCenter);
		m_pTextBase_Rebirth		 = CreateStaticControl("CTF_RESULT_TEXT_BASE_REBIRTH", pFont_Shadow, nAlignCenter);
		m_pTextBase_Contribution = CreateStaticControl("CTF_RESULT_TEXT_BASE_CONTRIBUTION", pFont_Shadow, nAlignCenter);

		m_pTextBase_Rank->SetOneLineText( ID2GAMEWORD("CTF_RESULT_TEXT_BASE", 0), NS_UITEXTCOLOR::LIGHTGREY );
		m_pTextBase_School->SetOneLineText( ID2GAMEWORD("CTF_RESULT_TEXT_BASE", 1), NS_UITEXTCOLOR::LIGHTGREY );
		m_pTextBase_Class->SetOneLineText( ID2GAMEWORD("CTF_RESULT_TEXT_BASE", 2), NS_UITEXTCOLOR::LIGHTGREY );
		m_pTextBase_Name->SetOneLineText( ID2GAMEWORD("CTF_RESULT_TEXT_BASE", 3), NS_UITEXTCOLOR::LIGHTGREY );
		m_pTextBase_Damage->SetOneLineText( ID2GAMEWORD("CTF_RESULT_TEXT_BASE", 9), NS_UITEXTCOLOR::LIGHTGREY );
		m_pTextBase_Heal->SetOneLineText( ID2GAMEWORD("CTF_RESULT_TEXT_BASE", 6), NS_UITEXTCOLOR::LIGHTGREY );
		m_pTextBase_Kill->SetOneLineText( ID2GAMEWORD("CTF_RESULT_TEXT_BASE", 4), NS_UITEXTCOLOR::LIGHTGREY );
		m_pTextBase_Death->SetOneLineText( ID2GAMEWORD("CTF_RESULT_TEXT_BASE", 5), NS_UITEXTCOLOR::LIGHTGREY );
		m_pTextBase_Rebirth->SetOneLineText( ID2GAMEWORD("CTF_RESULT_TEXT_BASE", 7), NS_UITEXTCOLOR::LIGHTGREY );
		m_pTextBase_Contribution->SetOneLineText( ID2GAMEWORD("CTF_RESULT_TEXT_BASE", 8), NS_UITEXTCOLOR::LIGHTGREY );
	}

	//! 랭킹 슬롯
	{
		m_vecSlot.resize(nMAX_SLOT);

		for ( UINT i=0; i<nMAX_SLOT; ++i )
		{
			RANKSLOT* &pSlot = m_vecSlot[i];

			pSlot = new RANKSLOT(m_pGaeaClient, m_pEngineDevice);
			pSlot->CreateSub( this, "CTF_RESULT_RANKSLOT_BASE", UI_FLAG_DEFAULT );
			pSlot->CreateSubControl( this );
			RegisterControl( pSlot );

			const UIRECT& rcSlotRect = pSlot->GetLocalPos();
			pSlot->SetLocalPos( D3DXVECTOR2(rcSlotRect.left, rcSlotRect.top + (rcSlotRect.sizeY * static_cast<float>(i)) ) );
		}
	}

	//! 자신의 랭킹
	{
		m_pMyRank = new RANKSLOT(m_pGaeaClient, m_pEngineDevice);
		m_pMyRank->CreateSub( this, "CTF_RESULT_RANKSLOT_BASE", UI_FLAG_DEFAULT );
		m_pMyRank->CreateSubControl( this );
		m_pMyRank->SetMyRank();
		RegisterControl( m_pMyRank );

		const UIRECT& rcSlotRect = m_pMyRank->GetLocalPos();
		m_pMyRank->SetLocalPos( D3DXVECTOR2(rcSlotRect.left, rcSlotRect.top + (rcSlotRect.sizeY * static_cast<float>(nMAX_SLOT)) + 6.0f) );

		//! 라인
		CreateColorBackgroundControl( "CTF_RESULT_MYRANK_LINE_HEAD", NS_UITEXTCOLOR::BLACK );
		CreateColorBackgroundControl( "CTF_RESULT_MYRANK_LINE_TAIL", NS_UITEXTCOLOR::BLACK );
	}

	//! 스크롤 바
	{
		//! 배경
		CreateColorBackgroundControl( "CTF_RESULT_RANK_SCROLLBAR_BACKGROUND", NS_UITEXTCOLOR::BLACK, UI_FLAG_RIGHT | UI_FLAG_YSIZE );

		//! 스크롤
		CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
		pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, CTF_RESULT_RANK_SCROLLBAR );
		pScrollBar->CreateBaseScrollBar ( "CTF_RESULT_RANK_SCROLLBAR" );
		pScrollBar->GetThumbFrame()->SetState ( 150, nMAX_SLOT );

		//  해당 컨트롤 위에서만 마우스 휠이 동작한다.
		pScrollBar->GetThumbFrame()->SetScrollParent( this ); 
		RegisterControl ( pScrollBar );  
		m_pScroll = pScrollBar;
	}
}

void CCTFResultRanking::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	{
		CBasicScrollThumbFrame* const pThumbFrame = m_pScroll->GetThumbFrame ();
		const int nTotal = pThumbFrame->GetTotal ();
		const int nViewPerPage = pThumbFrame->GetViewPerPage ();

		if ( nViewPerPage < nTotal )
		{
			int nCurPos = 0;
			const int nMovableLine = nTotal - nViewPerPage;
			float fPercent = pThumbFrame->GetPercent ();

			nCurPos = (int)floor(fPercent * nMovableLine);
			if ( nCurPos < 0 ) nCurPos = 0;

			if ( m_nScrollPos == nCurPos ) return;

			m_nScrollPos = nCurPos;

			Update();
		}
	}
}

void CCTFResultRanking::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	//switch ( ControlID )
	//{
	//default:
	//	break;
	//};
}

void CCTFResultRanking::Update()
{
	using namespace PVP;
	GLCaptureTheFieldClient* pCTFClient = m_pGaeaClient->GetCaptureTheField();

	PVP::VEC_RESULTDATA* pvecResult = NULL;

	switch ( m_emSortType )
	{
	case EMMAINSORTTYPE_TOTAL: pvecResult = &pCTFClient->GetResultDataTotal(); break;
	case EMMAINSORTTYPE_SM: pvecResult = &pCTFClient->GetResultDataSM(); break;
	case EMMAINSORTTYPE_HA: pvecResult = &pCTFClient->GetResultDataHA(); break;
	case EMMAINSORTTYPE_BH: pvecResult = &pCTFClient->GetResultDataBH(); break;
	}

	UINT nMaxCount = (UINT)pvecResult->size();

	if ( nMaxCount<= nMAX_SLOT )
	{
		for ( UINT i=0; i<nMAX_SLOT; ++i )
		{
			m_vecSlot[i]->SetEnable( false );
		}
	}

	if ( nMaxCount == 0 )
	{
		m_pScroll->GetThumbFrame()->SetState ( 1, 1 );
	}
	else
	{
		m_pScroll->GetThumbFrame()->SetState ( nMaxCount, nMAX_SLOT );
	}

	if ( !pvecResult->empty() )
	{
		if ( nMaxCount <= m_nScrollPos )
		{
			m_nScrollPos = 0;
			m_pScroll->GetThumbFrame()->SetPercent(0.0f);
		}

		UINT nResultIndex = m_nScrollPos;

		for ( UINT i=0; i<nMAX_SLOT; ++i )
		{
			if ( nMaxCount <= nResultIndex )
			{
				break;
			}

			PVP::SCTFRESULTDATA* pResultData = (*pvecResult)[nResultIndex++];
			GASSERT(pResultData);

			m_vecSlot[i]->SetData( nResultIndex, pResultData );
			m_vecSlot[i]->SetEnable( true );
		}
	}
	else
	{
		m_nScrollPos = 0;
		m_pScroll->GetThumbFrame()->SetPercent(0.0f);
	}

	//! 나의 정보를 세팅
	PVP::SCTFRESULTDATA& MyData = pCTFClient->GetMyResultData();
	UINT nMyRank = 0;

	switch ( m_emSortType )
	{
	case EMMAINSORTTYPE_TOTAL: 
		{
			nMyRank = pCTFClient->GetMyResultRanking() + 1;
		}
		break;

	case EMMAINSORTTYPE_SM:
		{
			if ( m_pGaeaClient->GetCharacter()->m_wSchool != SCHOOL_SM )
			{
				break;
			}

			nMyRank = pCTFClient->GetMyResultRankingSchool() + 1;
		}
		break;

	case EMMAINSORTTYPE_HA:
		{
			if ( m_pGaeaClient->GetCharacter()->m_wSchool != SCHOOL_HA )
			{
				break;
			}

			nMyRank = pCTFClient->GetMyResultRankingSchool() + 1;
		}
		break;

	case EMMAINSORTTYPE_BH:
		{
			if ( m_pGaeaClient->GetCharacter()->m_wSchool != SCHOOL_BH )
			{
				break;
			}

			nMyRank = pCTFClient->GetMyResultRankingSchool() + 1;
		}
		break;
	}

	m_pMyRank->SetData( nMyRank, &MyData );
}

void CCTFResultRanking::ResetScrollBar()
{
	m_nScrollPos = 0;
	m_pScroll->GetThumbFrame ()->SetPercent( 0.0f );
	Update();
}

void CCTFResultRanking::Update_SortTotal()
{
	m_nScrollPos = 0;
	m_emSortType = EMMAINSORTTYPE_TOTAL;
	m_pScroll->GetThumbFrame ()->SetPercent( 0.0f );

	for ( UINT i=0; i<nMAX_SLOT; ++i )
	{
		m_vecSlot[i]->Reset();
	}

	Update();
}

void CCTFResultRanking::Update_SortSM()
{
	m_nScrollPos = 0;
	m_emSortType = EMMAINSORTTYPE_SM;
	m_pScroll->GetThumbFrame ()->SetPercent( 0.0f );

	for ( UINT i=0; i<nMAX_SLOT; ++i )
	{
		m_vecSlot[i]->Reset();
	}

	Update();
}

void CCTFResultRanking::Update_SortHA()
{
	m_nScrollPos = 0;
	m_emSortType = EMMAINSORTTYPE_HA;
	m_pScroll->GetThumbFrame ()->SetPercent( 0.0f );

	for ( UINT i=0; i<nMAX_SLOT; ++i )
	{
		m_vecSlot[i]->Reset();
	}

	Update();
}

void CCTFResultRanking::Update_SortBH()
{
	m_nScrollPos = 0;
	m_emSortType = EMMAINSORTTYPE_BH;
	m_pScroll->GetThumbFrame ()->SetPercent( 0.0f );

	for ( UINT i=0; i<nMAX_SLOT; ++i )
	{
		m_vecSlot[i]->Reset();
	}

	Update();
}

CBasicTextBox* CCTFResultRanking::CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, szControlKeyword, UI_FLAG_DEFAULT, cID );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );	
	RegisterControl ( pStaticText );

	return pStaticText;
}

CUIControl* CCTFResultRanking::CreateColorBackgroundControl ( char* szControlKeyword, const D3DCOLOR& cColor, int nAlign )
{
	CUIControl* pStaticBackground = new CUIControl(m_pEngineDevice);
	pStaticBackground->CreateSub ( this, szControlKeyword, nAlign );
	pStaticBackground->SetDiffuse( cColor );
	pStaticBackground->SetUseRender( TRUE );
	RegisterControl ( pStaticBackground );

	return pStaticBackground;
}

//----------------------------------------------------------------------------------------------------//

void CCTFResultRanking::RANKSLOT::CreateSubControl ( CCTFResultRanking* pBase )
{
	GASSERT(pBase);

	CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
	CD3DFontPar* pFont_Shadow = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

	const int nAlignCenter = TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y;	

	{
		pTEXTRANK         = CreateStaticControl("CTF_RESULT_RANKSLOT_TEXT_BASE_RANK", pFont_Shadow, nAlignCenter);
		pTEXTNAME         = CreateStaticControl("CTF_RESULT_RANKSLOT_TEXT_BASE_NAME", pFont_Shadow, nAlignCenter);
		pTEXTDAMAGE		  = CreateStaticControl("CTF_RESULT_RANKSLOT_TEXT_BASE_DAMAGE", pFont_Shadow, nAlignCenter);
		pTEXTHEAL		  = CreateStaticControl("CTF_RESULT_RANKSLOT_TEXT_BASE_HEAL", pFont_Shadow, nAlignCenter);
		pTEXTKILL         = CreateStaticControl("CTF_RESULT_RANKSLOT_TEXT_BASE_KILL", pFont_Shadow, nAlignCenter);
		pTEXTDEATH        = CreateStaticControl("CTF_RESULT_RANKSLOT_TEXT_BASE_DEATH", pFont_Shadow, nAlignCenter);
		pTEXTREBIRTH      = CreateStaticControl("CTF_RESULT_RANKSLOT_TEXT_BASE_REBIRTH", pFont_Shadow, nAlignCenter);
		pTEXTCONTRIBUTION = CreateStaticControl("CTF_RESULT_RANKSLOT_TEXT_BASE_CONTRIBUTION", pFont_Shadow, nAlignCenter);

		pIMAGESCHOOL = new CSwapImage(m_pEngineDevice);
		pIMAGESCHOOL->CreateSub ( this, "CTF_RESULT_RANKSLOT_IMAGE_BASE_SCHOOL", UI_FLAG_DEFAULT );
		RegisterControl ( pIMAGESCHOOL );

		pIMAGECLASS = new CSwapImage(m_pEngineDevice);
		pIMAGECLASS->CreateSub ( this, "CTF_RESULT_RANKSLOT_IMAGE_BASE_CLASS", UI_FLAG_DEFAULT );
		RegisterControl ( pIMAGECLASS );
	}

	{
		SetTextPosition( pTEXTRANK, pBase->GetBaseRectRank() );
		SetTextPosition( pTEXTNAME, pBase->GetBaseRectName() );
		SetTextPosition( pTEXTDAMAGE, pBase->GetBaseRectDamage() );
		SetTextPosition( pTEXTHEAL, pBase->GetBaseRectHeal() );
		SetTextPosition( pTEXTKILL, pBase->GetBaseRectKill() );
		SetTextPosition( pTEXTDEATH, pBase->GetBaseRectDeath() );
		SetTextPosition( pTEXTREBIRTH, pBase->GetBaseRectRebirth() );
		SetTextPosition( pTEXTCONTRIBUTION, pBase->GetBaseRectContribution() );

		SetImagePosition( pIMAGESCHOOL, pBase->GetBaseRectSchool(), GetLocalPos() );
		SetImagePosition( pIMAGECLASS, pBase->GetBaseRectClass(), GetLocalPos() );
	}

	{
		pTEXTRANK->SetOneLineText( "", NS_UITEXTCOLOR::WHITE );
		pTEXTNAME->SetOneLineText( "", NS_UITEXTCOLOR::WHITE );
		pTEXTDAMAGE->SetOneLineText( "", NS_UITEXTCOLOR::WHITE );
		pTEXTHEAL->SetOneLineText( "", NS_UITEXTCOLOR::WHITE );
		pTEXTKILL->SetOneLineText( "", NS_UITEXTCOLOR::WHITE );
		pTEXTDEATH->SetOneLineText( "", NS_UITEXTCOLOR::WHITE );
		pTEXTREBIRTH->SetOneLineText( "", NS_UITEXTCOLOR::WHITE );
		pTEXTCONTRIBUTION->SetOneLineText( "", NS_UITEXTCOLOR::WHITE );

		pIMAGESCHOOL->SetImage( "CTF_RESULT_RANKSLOT_IMAGE_SCHOOL0" );
		pIMAGECLASS->SetImage( "CREATE_CHAR_CLASS_BRAWLER_MALE" );
	}

	bENABLE = true;
}

void CCTFResultRanking::RANKSLOT::Reset()
{
	SetEnable( false );
	strNAME = "";
}

void CCTFResultRanking::RANKSLOT::SetMyRank()
{
	bMYRANK = true;

	pTEXTRANK->SetUseOverColor( TRUE );
	pTEXTNAME->SetUseOverColor( TRUE );
	pTEXTDAMAGE->SetUseOverColor( TRUE );
	pTEXTHEAL->SetUseOverColor( TRUE );
	pTEXTKILL->SetUseOverColor( TRUE );
	pTEXTDEATH->SetUseOverColor( TRUE );
	pTEXTREBIRTH->SetUseOverColor( TRUE );
	pTEXTCONTRIBUTION->SetUseOverColor( TRUE );

	pTEXTRANK->SetOverColor( 0, NS_UITEXTCOLOR::GREENYELLOW );
	pTEXTNAME->SetOverColor( 0, NS_UITEXTCOLOR::GREENYELLOW );
	pTEXTDAMAGE->SetOverColor( 0, NS_UITEXTCOLOR::GREENYELLOW );
	pTEXTHEAL->SetOverColor( 0, NS_UITEXTCOLOR::GREENYELLOW );
	pTEXTKILL->SetOverColor( 0, NS_UITEXTCOLOR::GREENYELLOW );
	pTEXTDEATH->SetOverColor( 0, NS_UITEXTCOLOR::GREENYELLOW );
	pTEXTREBIRTH->SetOverColor( 0, NS_UITEXTCOLOR::GREENYELLOW );
	pTEXTCONTRIBUTION->SetOverColor( 0, NS_UITEXTCOLOR::GREENYELLOW );
}

void CCTFResultRanking::RANKSLOT::SetEnable( const bool bEnable )
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

void CCTFResultRanking::RANKSLOT::SetData( const UINT nRank, const PVP::SCTFRESULTDATA* pResultData )
{
	GASSERT(pResultData);

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

	//! 랭킹은 달라질 수 있다.
	if ( nRank )
	{
		pTEXTRANK->SetOneLineText( sc::string::format("%3d", nRank).c_str(), NS_UITEXTCOLOR::WHITE );
	}
	else
	{
		pTEXTRANK->SetOneLineText( "-", NS_UITEXTCOLOR::WHITE );
	}

	//! 만약 값이 없고 나의 랭크일 경우에만
	if ( pResultData->IsNull() && bMYRANK )
	{
		pTEXTNAME->SetOneLineText( pGAEACLIENT->GetCharacter()->GetName(), NS_UITEXTCOLOR::WHITE);
		pTEXTDAMAGE->SetOneLineText( "-", NS_UITEXTCOLOR::WHITE );	
		pTEXTHEAL->SetOneLineText( "-", NS_UITEXTCOLOR::WHITE );	
		pTEXTKILL->SetOneLineText( "-", NS_UITEXTCOLOR::WHITE );	
		pTEXTDEATH->SetOneLineText( "-", NS_UITEXTCOLOR::WHITE );	
		pTEXTREBIRTH->SetOneLineText( "-", NS_UITEXTCOLOR::WHITE );
		pTEXTCONTRIBUTION->SetOneLineText( "-", NS_UITEXTCOLOR::WHITE );
		pIMAGESCHOOL->SetImage( sc::string::format("CTF_RESULT_RANKSLOT_IMAGE_SCHOOL%1%", pGAEACLIENT->GetCharacter()->m_wSchool ).c_str() );
		pIMAGECLASS->SetImage( strClass[pGAEACLIENT->GetCharacter()->m_CHARINDEX].c_str() );
		return;
	}

	//! 나머지는 아님
	if ( strNAME == pResultData->strNAME )
	{
		return;
	}
	
	if ( pResultData->nHEAL )
	{
		pTEXTHEAL->SetOneLineText( sc::string::format("%d", pResultData->nHEAL).c_str(), NS_UITEXTCOLOR::WHITE );	
	}
	else
	{
		pTEXTHEAL->SetOneLineText( "-", NS_UITEXTCOLOR::WHITE );	
	}

	if ( pResultData->nREBIRTH )
	{
		pTEXTREBIRTH->SetOneLineText( sc::string::format("%d", pResultData->nREBIRTH).c_str(), NS_UITEXTCOLOR::WHITE );	
	}
	else
	{
		pTEXTREBIRTH->SetOneLineText( "-", NS_UITEXTCOLOR::WHITE );	
	}

	pTEXTNAME->SetOneLineText( pResultData->strNAME.c_str(), NS_UITEXTCOLOR::WHITE);
	pTEXTDAMAGE->SetOneLineText( sc::string::format("%d", pResultData->nDAMAGE).c_str(), NS_UITEXTCOLOR::WHITE );	
	pTEXTKILL->SetOneLineText( sc::string::format("%d", pResultData->nKILL).c_str(), NS_UITEXTCOLOR::WHITE );	
	pTEXTDEATH->SetOneLineText( sc::string::format("%d", pResultData->nDEATH).c_str(), NS_UITEXTCOLOR::WHITE );	
	pTEXTCONTRIBUTION->SetOneLineText( sc::string::format("+%d", pResultData->nCONTRIBUTION ).c_str(), NS_UITEXTCOLOR::WHITE );	


	GASSERT(pResultData->emSCHOOL < SCHOOL_NUM);
	GASSERT(pResultData->emCLASS < GLCI_NUM_ETC);

	pIMAGESCHOOL->SetImage( sc::string::format("CTF_RESULT_RANKSLOT_IMAGE_SCHOOL%1%", pResultData->emSCHOOL ).c_str() );
	pIMAGECLASS->SetImage( strClass[pResultData->emCLASS].c_str() );
}

void CCTFResultRanking::RANKSLOT::SetTextPosition( CBasicTextBox* pTextBox, const UIRECT& rcBase )
{
	GASSERT(pTextBox);

	UIRECT rcTextBox = pTextBox->GetLocalPos();
	rcTextBox.left  = rcBase.left;
	rcTextBox.right = rcBase.right;
	rcTextBox.sizeX = rcBase.sizeX;

	pTextBox->SetLocalPos( rcTextBox );
}

void CCTFResultRanking::RANKSLOT::SetImagePosition( CSwapImage* pImage, const UIRECT& rcBase, const UIRECT& rcParent )
{
	GASSERT(pImage);

	UIRECT rcImage = pImage->GetLocalPos();
	rcImage.left   = (rcBase.left + rcBase.right)/2.0f - rcImage.sizeX/2.0f;
	rcImage.right  = rcImage.left + rcImage.sizeX;
	rcImage.top    = rcImage.top + rcParent.sizeY/2.0f - rcImage.sizeY/2.0f;
	rcImage.bottom = rcImage.top + rcImage.sizeY;

	pImage->SetLocalPos( rcImage );
}

CBasicTextBox* CCTFResultRanking::RANKSLOT::CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, szControlKeyword, UI_FLAG_DEFAULT, cID );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );	
	RegisterControl ( pStaticText );

	return pStaticText;
}