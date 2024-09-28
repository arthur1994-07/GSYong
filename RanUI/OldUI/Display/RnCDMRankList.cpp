#include "StdAfx.h"
//#include "../../../SigmaCore/String/StringFormat.h"

#include "../Interface/SkillFunc.h"
#include "../../InnerInterface.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/CapturedTheField/GLCaptureTheFieldClient.h"
#include "../../../RanLogic/GLogicDataMan.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../Item/ItemImageEx.h"
#include "../../../RanLogic/GLCompetition.h"
#include "../../../RanLogic/Msg/GLContrlInstanceContentsMsg.h"
#include "../../../EngineLib/DxTools/DxClubMan.h"
#include "RnCDMRankList.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

RnCDMRankList::RnCDMRankList ( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice, std::vector<InstanceSystem::RNCDM_RANK_SLOT>* pvecRnCdmRankSlot  )
: CUIGroup(pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_pInterface(pInterface)
, m_pvecRnCdmRankSlot(pvecRnCdmRankSlot)
, m_pTextBase_Rank(NULL)
, m_pTextBase_Club_Name(NULL)
, m_pTextBase_Club_Master(NULL)
, m_pTextBase_Get_Point(NULL)
, m_pTextBase_Kill_Num(NULL)
, m_pTextBase_Death_Num(NULL)
, m_pTextBase_Rebirth(NULL)
, m_pTextBase_Reward(NULL)
, m_pMyRank(NULL)
, m_pScroll(NULL)
, m_nScrollPos(-1)
, m_emSortType(EMMAINSORTTYPE_TOTAL)
{
	//Blank
}

RnCDMRankList::~RnCDMRankList()
{
	m_vecSlot.clear();
}

void RnCDMRankList::CreateSubControl ()
{
	std::string strBaseControl = GetControlNameEx();
	const int nAlignCenter = TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y;	
	const int nAlignRight = TEXT_ALIGN_RIGHT | TEXT_ALIGN_CENTER_Y;	

	//! 라인박스
	{
		CBasicLineBoxSmart* pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
		pLineBox->CreateSub( this, "RNCDM_RANK_LINEBOX", UI_FLAG_DEFAULT );
		pLineBox->CreateSubControl( "CTF_RESULT_RANKING_LINEBOX_TEXINFO" );
		RegisterControl( pLineBox );

		//! 라인
		CreateColorBackgroundControl( "RNCDM_RANK_LINEBOX_HEAD", NS_UITEXTCOLOR::WHITE );
	}

	//! 기본 텍스트
	{
		CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
		CD3DFontPar* pFont_Shadow = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

		m_pTextBase_Rank		 = CreateStaticControl("RNCDM_RANK_TEXTBOX", pFont_Shadow, nAlignRight);
		m_pTextBase_Club_Name		 = CreateStaticControl("RNCDM_CLUB_NAME_TEXTBOX", pFont_Shadow, nAlignCenter);
		m_pTextBase_Club_Master		 = CreateStaticControl("RNCDM_CLUB_MASTER_TEXTBOX", pFont_Shadow, nAlignCenter);
		m_pTextBase_Get_Point		 = CreateStaticControl("RNCDM_GET_POINT_TEXTBOX", pFont_Shadow, nAlignCenter);
		m_pTextBase_Kill_Num		 = CreateStaticControl("RNCDM_KINN_NUM_TEXTBOX", pFont_Shadow, nAlignCenter);
		m_pTextBase_Death_Num		 = CreateStaticControl("RNCDM_DEATH_NUM_TEXTBOX", pFont_Shadow, nAlignCenter);
		m_pTextBase_Rebirth		 = CreateStaticControl("RNCDM_REBIRTH_TEXTBOX", pFont_Shadow, nAlignCenter);
		m_pTextBase_Reward		 = CreateStaticControl("RNCDM_REWARD_TEXTBOX", pFont_Shadow, nAlignCenter);

		//순위
		m_pTextBase_Rank->SetOneLineText( ID2GAMEWORD("RN_CMD_RANK_TEXT"), NS_UITEXTCOLOR::LIGHTGREY );
		
		//클럽마트/명
		m_pTextBase_Club_Name->SetOneLineText( ID2GAMEWORD("RN_CDM_CLUB_NAME_TEXT"), NS_UITEXTCOLOR::LIGHTGREY );
		
		//클럽마스터명
		m_pTextBase_Club_Master->SetOneLineText( ID2GAMEWORD("RN_CDM_CLUB_MASTER_NAME_TEXT"), NS_UITEXTCOLOR::LIGHTGREY );
		
		//획득포인트
		m_pTextBase_Get_Point->SetOneLineText( ID2GAMEWORD("RN_CDM_GET_POINT_TEXT"), NS_UITEXTCOLOR::LIGHTGREY );
		
		//사살
		m_pTextBase_Kill_Num->SetOneLineText( ID2GAMEWORD("RN_CDM_KILL_NUM_TEXT"), NS_UITEXTCOLOR::LIGHTGREY );
		
		//죽음
		m_pTextBase_Death_Num->SetOneLineText( ID2GAMEWORD("RN_CDM_DEATH_NUM_TEXT"), NS_UITEXTCOLOR::LIGHTGREY );
		
		//부활
		m_pTextBase_Rebirth->SetOneLineText( ID2GAMEWORD("RN_CDM_REBIRTH_TEXT"), NS_UITEXTCOLOR::LIGHTGREY );
		
		//보상
		m_pTextBase_Reward->SetOneLineText( ID2GAMEWORD("RN_CDM_REWARD_TEXT"), NS_UITEXTCOLOR::LIGHTGREY );
	}

	//! 랭킹 슬롯
	{
		m_vecSlot.resize(nMAX_SLOT);

		for ( UINT i=0; i<nMAX_SLOT; ++i )
		{
			RANKSLOT* &pSlot = m_vecSlot[i];

			pSlot = new RANKSLOT(m_pInterface, m_pGaeaClient, m_pEngineDevice);
			pSlot->CreateSub( this, "CTF_RESULT_RANKSLOT_BASE", UI_FLAG_DEFAULT );
			pSlot->CreateSubControl( this );
			RegisterControl( pSlot );

			const UIRECT& rcSlotRect = pSlot->GetLocalPos();
			pSlot->SetLocalPos( D3DXVECTOR2(rcSlotRect.left, rcSlotRect.top + (rcSlotRect.sizeY * static_cast<float>(i)) ) );
			pSlot->m_nSlotIndex = i;
		}
	}

	//! 자신의 랭킹
	{
		m_pMyRank = new RANKSLOT(m_pInterface, m_pGaeaClient, m_pEngineDevice);
		m_pMyRank->CreateSub( this, "CTF_RESULT_RANKSLOT_BASE", UI_FLAG_DEFAULT );
		m_pMyRank->CreateSubControl( this );
		m_pMyRank->SetMyRank();
		RegisterControl( m_pMyRank );

		const UIRECT& rcSlotRect = m_pMyRank->GetLocalPos();
		m_pMyRank->SetLocalPos( D3DXVECTOR2(rcSlotRect.left, rcSlotRect.top + (rcSlotRect.sizeY * static_cast<float>(nMAX_SLOT)) + 6.0f) );

		//! 라인
		CreateColorBackgroundControl( "RNCDM_RANK__MYRANK_LINE_HEAD", NS_UITEXTCOLOR::BLACK );
		CreateColorBackgroundControl( "RNCDM_RANK__MYRANK_LINE_TAIL", NS_UITEXTCOLOR::BLACK );
	}

	//! 스크롤 바
	{
		//! 배경
		CreateColorBackgroundControl( "RNCDM_RANK_SCROLLBAR_BACKGROUND", NS_UITEXTCOLOR::BLACK, UI_FLAG_RIGHT | UI_FLAG_YSIZE );

		//! 스크롤
		CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
		pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, RNCDM_RESULT_RANK_SCROLLBAR );
		pScrollBar->CreateBaseScrollBar ( "RNCDM_RANK_SCROLLBAR" );
		pScrollBar->GetThumbFrame()->SetState ( 150, nMAX_SLOT );

		//  해당 컨트롤 위에서만 마우스 휠이 동작한다.
		pScrollBar->GetThumbFrame()->SetScrollParent( this ); 
		RegisterControl ( pScrollBar );  
		m_pScroll = pScrollBar;
	}
}

void RnCDMRankList::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
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

			UpdateList();
		}
	}
}

void RnCDMRankList::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case RNCDM_INFOBUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
			{
				int aaa = 11;
			}
		}
	default:
		break;
	};
}

void RnCDMRankList::UpdateList()
{
	using namespace PVP;
	//GLCaptureTheFieldClient* pCTFClient = m_pGaeaClient->GetCaptureTheField();


	if (!m_pvecRnCdmRankSlot)
		return;

	UINT nMaxCount = (UINT)m_pvecRnCdmRankSlot->size();

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


 	if ( !m_pvecRnCdmRankSlot->empty() )
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

			RNCDM_RANK_SLOT sRnCdmRankSlot = m_pvecRnCdmRankSlot->at(nResultIndex);
			int nMyRank = nResultIndex;

			if(m_pGaeaClient->GetCharacter()->GetClubDbNum() == sRnCdmRankSlot.dwClubDBNum )
			{
				//! 나의 정보를 세팅
				m_pMyRank->m_nSlotIndex = nMyRank;
				m_pMyRank->SetData( nMyRank, &sRnCdmRankSlot );
			}

			m_vecSlot[i]->SetData( nResultIndex, &m_pvecRnCdmRankSlot->at(nResultIndex++) );
			m_vecSlot[i]->SetEnable( true );
		}
	}
	else
	{
		m_nScrollPos = 0;
		m_pScroll->GetThumbFrame()->SetPercent(0.0f);
	}

	////! 나의 정보를 세팅
	//PVP::SCTFRESULTDATA& MyData = pCTFClient->GetMyResultData();
	//UINT nMyRank = 0;

	//switch ( m_emSortType )
	//{
	//case EMMAINSORTTYPE_TOTAL: 
	//	{
	//		nMyRank = pCTFClient->GetMyResultRanking() + 1;
	//	}
	//	break;
	//}

	//m_pMyRank->SetData( nMyRank, MyData );
}

void RnCDMRankList::ResetScrollBar()
{
	m_nScrollPos = 0;
	m_pScroll->GetThumbFrame ()->SetPercent( 0.0f );
	UpdateList();
}

void RnCDMRankList::Update_SortTotal()
{
	m_nScrollPos = 0;
	m_emSortType = EMMAINSORTTYPE_TOTAL;
	m_pScroll->GetThumbFrame ()->SetPercent( 0.0f );

	for ( UINT i=0; i<nMAX_SLOT; ++i )
	{
		m_vecSlot[i]->Reset();
	}

	UpdateList();
}


CBasicTextBox* RnCDMRankList::CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, szControlKeyword, UI_FLAG_DEFAULT, cID );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );	
	RegisterControl ( pStaticText );

	return pStaticText;
}

CUIControl* RnCDMRankList::CreateColorBackgroundControl ( char* szControlKeyword, const D3DCOLOR& cColor, int nAlign )
{
	CUIControl* pStaticBackground = new CUIControl(m_pEngineDevice);
	pStaticBackground->CreateSub ( this, szControlKeyword, nAlign );
	pStaticBackground->SetDiffuse( cColor );
	pStaticBackground->SetUseRender( TRUE );
	RegisterControl ( pStaticBackground );

	return pStaticBackground;
}

void RnCDMRankList::ClearCdmRankList()
{
	m_pvecRnCdmRankSlot->clear();
}

//----------------------------------------------------------------------------------------------------//

void RnCDMRankList::RANKSLOT::CreateSubControl ( RnCDMRankList* pBase )
{
	GASSERT(pBase);

	CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
	CD3DFontPar* pFont_Shadow = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

	const int nAlignCenter = TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y;	

	{
		pTEXTRANK		= CreateStaticControl("RNCDM_RANK_TEXTBOX", pFont_Shadow, nAlignCenter);

		pIMAGECLUBMARK = new CUIControl(m_pEngineDevice);
		pIMAGECLUBMARK->CreateSub ( this, "RN_CDM_RANKSLOT_IMAGE_CLUB_MARK_POS", UI_FLAG_DEFAULT );
		pIMAGECLUBMARK->SetUseRender(TRUE);
		RegisterControl ( pIMAGECLUBMARK );

		pTEXTCLUBNAME	= CreateStaticControl("RNCDM_CLUB_NAME_TEXTBOX", pFont_Shadow, nAlignCenter);
		pTEXTMASTERNAME	= CreateStaticControl("RNCDM_CLUB_MASTER_TEXTBOX", pFont_Shadow, nAlignCenter);

		//참가인원 정보 출력 버튼

		pINFO = new CBasicButton(m_pEngineDevice);
		pINFO->CreateSub( this, "RNCMD_INFO_BUTTON", UI_FLAG_DEFAULT,RNCDM_INFOBUTTON );
		pINFO->CreateFlip( "RNCMD_INFO_BUTTON_FLIP", CBasicButton::MOUSEIN_FLIP );
		RegisterControl( pINFO );


		pTEXTGETPOINT	= CreateStaticControl("RNCDM_GET_POINT_TEXTBOX", pFont_Shadow, nAlignCenter);
		pTEXTKILLNUM	= CreateStaticControl("RNCDM_KINN_NUM_TEXTBOX", pFont_Shadow, nAlignCenter);
		pTEXTDEATHNUM	= CreateStaticControl("RNCDM_DEATH_NUM_TEXTBOX", pFont_Shadow, nAlignCenter);
		pTEXTREBIRTHNUM	= CreateStaticControl("RNCDM_REBIRTH_TEXTBOX", pFont_Shadow, nAlignCenter);

		for ( int i = 0; i < nRewardItem; i++ )
		{

			pIMAGEREWARDITEM[i] = new CUIControl( m_pEngineDevice );
			pIMAGEREWARDITEM[i]->CreateSub( this, sc::string::format("RN_CDM_RANKSLOT_IMAGE_CLUB_REWARD_BUFF_%d", i).c_str(), UI_FLAG_DEFAULT );
			pIMAGEREWARDITEM[i]->SetUseRender(TRUE);
			RegisterControl( pIMAGEREWARDITEM[i] );
			pIMAGEREWARDITEM[i]->SetVisibleSingle(FALSE);
		}
	}

	{
		SetTextPosition( pTEXTRANK, pBase->GetBaseRectRank() );
		SetTextPosition( pTEXTGETPOINT, pBase->GetBaseRectGetPoint() );
		SetTextPosition( pTEXTKILLNUM, pBase->GetBaseRectKillNum() );
		SetTextPosition( pTEXTDEATHNUM, pBase->GetBaseRectDeathNum() );
		SetTextPosition( pTEXTREBIRTHNUM, pBase->GetBaseRectRebirth() );

		SetImagePositionCenterY( pIMAGECLUBMARK, pBase->GetBaseRectClubName(), GetLocalPos() );
		SetButtonPositionCenterY( pINFO, pBase->GetBaseRectClubName(), GetLocalPos() );
	}

	{
		pTEXTRANK->SetOneLineText( "", NS_UITEXTCOLOR::WHITE );
		pTEXTMASTERNAME->SetOneLineText( "", NS_UITEXTCOLOR::WHITE );
		pTEXTGETPOINT->SetOneLineText( "", NS_UITEXTCOLOR::WHITE );
		pTEXTKILLNUM->SetOneLineText( "", NS_UITEXTCOLOR::WHITE );
		pTEXTDEATHNUM->SetOneLineText( "", NS_UITEXTCOLOR::WHITE );
		pTEXTREBIRTHNUM->SetOneLineText( "", NS_UITEXTCOLOR::WHITE );

		for ( int i = 0; i < nRewardItem; i++ )
		{
			SetImagePositionCenterY( pIMAGEREWARDITEM[i], pBase->GetBaseRectReward(), GetLocalPos() );
		}
	}

	bENABLE = true;
}

void RnCDMRankList::RANKSLOT::Reset()
{
	SetEnable( false );
	strNAME = "";
}

void RnCDMRankList::RANKSLOT::SetMyRank()
{
	bMYRANK = true;

	pTEXTRANK->SetUseOverColor( TRUE );
	pTEXTGETPOINT->SetUseOverColor( TRUE );
	pTEXTKILLNUM->SetUseOverColor( TRUE );
	pTEXTDEATHNUM->SetUseOverColor( TRUE );
	pTEXTREBIRTHNUM->SetUseOverColor( TRUE );
	//pTEXTREWARD->SetUseOverColor( TRUE );


	pTEXTRANK->SetOverColor( 0, NS_UITEXTCOLOR::GREENYELLOW );
	pTEXTGETPOINT->SetOverColor( 0, NS_UITEXTCOLOR::GREENYELLOW );
	pTEXTKILLNUM->SetOverColor( 0, NS_UITEXTCOLOR::GREENYELLOW );
	pTEXTDEATHNUM->SetOverColor( 0, NS_UITEXTCOLOR::GREENYELLOW );
	pTEXTREBIRTHNUM->SetOverColor( 0, NS_UITEXTCOLOR::GREENYELLOW );
	//pTEXTREWARD->SetOverColor( 0, NS_UITEXTCOLOR::GREENYELLOW );
}

void RnCDMRankList::RANKSLOT::SetEnable( const bool bEnable )
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

void RnCDMRankList::RANKSLOT::SetData( const UINT nRank,  InstanceSystem::RNCDM_RANK_SLOT* pResultData )
{
	GASSERT(pResultData);

	pTEXTRANK->SetOneLineText( sc::string::format("%3d", nRank + 1).c_str(), NS_UITEXTCOLOR::WHITE );
    //pIMAGECLUBMARK->SetImage( sc::string::format("CTF_RESULT_RANKSLOT_IMAGE_SCHOOL%1%", pGAEACLIENT->GetCharacter()->m_wSchool ).c_str() );
	SetMyClubImage( pResultData->dwClubDBNum, pResultData->dwClubMarkVER );
	pTEXTCLUBNAME->SetOneLineText( pResultData->strClubName.c_str(), NS_UITEXTCOLOR::WHITE);
	pTEXTMASTERNAME->SetOneLineText( pResultData->strClubMasterName.c_str(), NS_UITEXTCOLOR::WHITE);

    CString strPoint = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( pResultData->dwPoint, 3, "," );
    pTEXTGETPOINT->SetOneLineText( strPoint+"pt", NS_UITEXTCOLOR::WHITE );

	pTEXTKILLNUM->SetOneLineText( sc::string::format("%d", pResultData->dwKill).c_str(), NS_UITEXTCOLOR::WHITE );	
    pTEXTDEATHNUM->SetOneLineText( sc::string::format("%d", pResultData->dwDeath).c_str(), NS_UITEXTCOLOR::WHITE );	
	pTEXTREBIRTHNUM->SetOneLineText( sc::string::format("%d", pResultData->dwRebirth).c_str(), NS_UITEXTCOLOR::WHITE );	


	for ( int i = 0; i < nRewardItem; i++ )
	{
		pIMAGEREWARDITEM[i]->SetVisibleSingle(FALSE);
	}

	for ( unsigned i = 0; i < pResultData->vecReWard.size(); i++ )
	{
		pIMAGEREWARDITEM[i]->Reset();

		const SITEM* pItemData = GLogicData::GetInstance().GetItem( pResultData->vecReWard.at(i) );
		if ( !pItemData )
			return;
		pIMAGEREWARDITEM[i]->SetVisibleSingle(TRUE);
		pIMAGEREWARDITEM[i]->SetTexturePos ( NS_SKILL::GetIconTexurePos ( pItemData->sBasicOp.sICONID ) );
		pIMAGEREWARDITEM[i]->SetTextureName ( pItemData->GetInventoryFile() );
		pIMAGEREWARDITEM[i]->InitDeviceObjects ( m_pEngineDevice->GetDevice() );
	}
}

void RnCDMRankList::RANKSLOT::SetMyClubImage( const DWORD dwClubDbNum, const DWORD dwVer )
{
	const clubmark::DXDATA& sMarkData = pGAEACLIENT->GetClubMark( dwClubDbNum, dwVer );

	pIMAGECLUBMARK->SetTexturePos ( 0, sMarkData.vTex_1_LU );
	pIMAGECLUBMARK->SetTexturePos ( 1, sMarkData.vTex_2_RU );
	pIMAGECLUBMARK->SetTexturePos ( 3, sMarkData.vTex_3_LD );
	pIMAGECLUBMARK->SetTexturePos ( 2, sMarkData.vTex_4_RD );

	//	만들어야함
	pIMAGECLUBMARK->SetTexture ( sMarkData.pddsTexture );

	//HRESULT hr = S_OK;
	//hr = pIMAGECLUBMARK->Render ( m_pEngineDevice->GetDevice() );
}

void RnCDMRankList::RANKSLOT::SetTextPosition( CBasicTextBox* pTextBox, const UIRECT& rcBase )
{
	GASSERT(pTextBox);

	UIRECT rcTextBox = pTextBox->GetLocalPos();
	rcTextBox.left  = rcBase.left;
	rcTextBox.right = rcBase.right;
	rcTextBox.sizeX = rcBase.sizeX;

	pTextBox->SetLocalPos( rcTextBox );
}

void RnCDMRankList::RANKSLOT::SetImagePositionCenterY( CItemImageEx* pImage, const UIRECT& rcBase, const UIRECT& rcParent )
{
	GASSERT(pImage);

	UIRECT rcImage = pImage->GetLocalPos();
	rcImage.left   = rcImage.left;
	rcImage.right  = rcImage.left + rcImage.sizeX;
	rcImage.top    = rcImage.top + rcParent.sizeY/2.0f - rcImage.sizeY/2.0f;
	rcImage.bottom = rcImage.top + rcImage.sizeY;

	pImage->SetLocalPos( rcImage );
}

void RnCDMRankList::RANKSLOT::SetImagePositionCenterY( CSwapImage* pImage, const UIRECT& rcBase, const UIRECT& rcParent )
{
	GASSERT(pImage);

	UIRECT rcImage = pImage->GetLocalPos();
	rcImage.left   = rcImage.left;
	rcImage.right  = rcImage.left + rcImage.sizeX;
	rcImage.top    = rcImage.top + rcParent.sizeY/2.0f - rcImage.sizeY/2.0f;
	rcImage.bottom = rcImage.top + rcImage.sizeY;

	pImage->SetLocalPos( rcImage );
}

void RnCDMRankList::RANKSLOT::SetImagePositionCenterY( CUIControl* pImage, const UIRECT& rcBase, const UIRECT& rcParent )
{
	GASSERT(pImage);

	UIRECT rcImage = pImage->GetLocalPos();
	rcImage.left   = rcImage.left;
	rcImage.right  = rcImage.left + rcImage.sizeX;
	rcImage.top    = rcImage.top + rcParent.sizeY/2.0f - rcImage.sizeY/2.0f;
	rcImage.bottom = rcImage.top + rcImage.sizeY;

	pImage->SetLocalPos( rcImage );
}


void RnCDMRankList::RANKSLOT::SetButtonPositionCenterY( CBasicButton* pButton, const UIRECT& rcBase, const UIRECT& rcParent )
{
	GASSERT(pButton);

	UIRECT rcButton = pButton->GetLocalPos();
	rcButton.left   = rcButton.left;
	rcButton.right  = rcButton.left + rcButton.sizeX;
	rcButton.top    = rcButton.top + rcParent.sizeY/2.0f - rcButton.sizeY/2.0f;
	rcButton.bottom = rcButton.top + rcButton.sizeY;

	pButton->SetLocalPos( rcButton );
}

CBasicTextBox* RnCDMRankList::RANKSLOT::CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, szControlKeyword, UI_FLAG_DEFAULT, cID );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );	
	RegisterControl ( pStaticText );

	return pStaticText;
}

void RnCDMRankList::RANKSLOT::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case RNCDM_INFOBUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
			{
				pInterface->SetCLubMemberInfo( pInterface->GetRankSlotData( m_nSlotIndex ).vecCdmMemberInfo );
				int nMemberNum = static_cast<int>(pInterface->GetRankSlotData( m_nSlotIndex ).vecCdmMemberInfo.size());

				if( nMemberNum < 1 )
					return;

				pInterface->SetTitleClubNameMemberNum( pTEXTCLUBNAME->GetText(0), nMemberNum );
				pInterface->SetRnCDMClubInfoPos( nPosX, nPosY );
				pInterface->UiShowGroupFocus( RNCDM_CLUB_INFO );
			}
		}
	default:
		break;
	};
}

void RnCDMRankList::RANKSLOT::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	nPosY = y;
	nPosX = x;
	CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}
