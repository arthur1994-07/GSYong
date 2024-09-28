#include "StdAfx.h"
#include "./RnCDMHistory.h"
#include "../../InnerInterface.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/CapturedTheField/GLCaptureTheFieldClient.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogic/Skill/GLSkill.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../EngineLib/GUInterface/NSGUI.h"

#include "../Skill/SkillImage.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

using namespace PVP;

// ----------------------------------------------------------------------------

RnCDMHistory::RnCDMHistory(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
: CUIGroup(pEngineDevice)
, m_ppd3dDevice(NULL)
, m_pGaeaClient(pGaeaClient)
, m_pInterface(pInterface)
, m_pText_Time(NULL)
, m_pText_Ranking(NULL)
, m_pAuthSchoolControl(NULL)
, m_pBuffControl(NULL)
, m_pBuffGapControl(NULL)
, m_pBuffItemControl(NULL)
, m_pButton_Ranking(NULL)
{

}

RnCDMHistory::~RnCDMHistory ()
{

}

void RnCDMHistory::CreateSubControl ( LPDIRECT3DDEVICEQ* ppd3dDevice )
{
	m_ppd3dDevice = ppd3dDevice;

	//! 라인박스
	{
		CBasicLineBoxSmart* pLineBox = NULL;
		pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
		pLineBox->CreateSub( this, "COMPETITION_CTF_PAGE_HISTORY_LINBOX" );
		pLineBox->CreateSubControl( "COMPETITION_CTF_PAGE_HISTORY_LINBOX_TEXINFO" );
		RegisterControl( pLineBox );
	}

	//! 텍스트
	{
		CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

		const int nAlignLeftTop    = TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP;	
		const int nAlignLeftCenter = TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y;	
		const int nAlignCenter	   = TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y;	

		CBasicTextBox* pTextBox = NULL;
		pTextBox = CreateStaticControl( "COMPETITION_CTF_PAGE_HISTORY_TITLE_STATICTEXT", pFont, nAlignLeftTop );
		pTextBox->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_HISTORY", 0), NS_UITEXTCOLOR::WHITE);

		pTextBox = CreateStaticControl( "COMPETITION_CTF_PAGE_HISTORY_TIME_STATICTEXT", pFont, nAlignCenter );
		pTextBox->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_HISTORY", 1), NS_UITEXTCOLOR::WHITE);

		pTextBox = CreateStaticControl( "COMPETITION_CTF_PAGE_HISTORY_AUTHSCHOOL_STATICTEXT", pFont, nAlignCenter );
		pTextBox->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_HISTORY", 2), NS_UITEXTCOLOR::WHITE);

		pTextBox = CreateStaticControl( "COMPETITION_CTF_PAGE_HISTORY_REWARDBUFF_STATICTEXT", pFont, nAlignCenter );
		pTextBox->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_HISTORY", 3), NS_UITEXTCOLOR::WHITE);

		pTextBox = CreateStaticControl( "COMPETITION_CTF_PAGE_HISTORY_RANKING_STATICTEXT", pFont, nAlignCenter );
		pTextBox->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_HISTORY", 4), NS_UITEXTCOLOR::WHITE);

		m_pText_Time	= CreateStaticControl( "COMPETITION_CTF_PAGE_HISTORY_TIME", pFont, nAlignCenter );
		m_pText_Ranking	= CreateStaticControl( "COMPETITION_CTF_PAGE_HISTORY_RANKING_TEXT", pFont, nAlignCenter );
		m_pText_Buff	= CreateStaticControl( "COMPETITION_CTF_PAGE_HISTORY_REWARDBUFF", pFont, nAlignCenter );

		m_pText_Time->SetOneLineText("-- : -- ~ -- : --");
		m_pText_Ranking->SetOneLineText("");
		m_pText_Buff->SetOneLineText(ID2GAMEWORD("COMPETITION_CTF_HISTORY", 5));
	}

	//! 이미지
	{
		m_pImage_AuthSchool = new CSwapImage(m_pEngineDevice);
		m_pImage_AuthSchool->CreateSub( this, "COMPETITION_CTF_PAGE_HISTORY_AUTHSCHOOL_IMAGE" );
		RegisterControl( m_pImage_AuthSchool );
	}

	//! 컨트롤
	{
		m_pAuthSchoolControl = new CUIControl(m_pEngineDevice);
		m_pAuthSchoolControl->CreateSub( this, "COMPETITION_CTF_PAGE_HISTORY_AUTHSCHOOL" );
		RegisterControl( m_pAuthSchoolControl );

		m_pBuffControl = new CUIControl(m_pEngineDevice);
		m_pBuffControl->CreateSub( this, "COMPETITION_CTF_PAGE_HISTORY_REWARDBUFF" );
		RegisterControl( m_pBuffControl );

		m_pBuffGapControl = new CUIControl(m_pEngineDevice);
		m_pBuffGapControl->CreateSub( this, "COMPETITION_CTF_PAGE_HISTORY_REWARDBUFF_GAP" );
		RegisterControl( m_pBuffGapControl );

		m_pBuffItemControl = new CUIControl(m_pEngineDevice);
		m_pBuffItemControl->CreateSub( this, "COMPETITION_CTF_PAGE_HISTORY_REWARDBUFF_ITEM" );
		RegisterControl( m_pBuffItemControl );

	}

	//! 버튼
	{
		m_pButton_Ranking = new CBasicButton(m_pEngineDevice);
		m_pButton_Ranking->CreateSub( this, "COMPETITION_CTF_PAGE_HISTORY_RANKING_BUTTON", UI_FLAG_DEFAULT, RNCDM_HISTORY_RANKING_BUTTON );
		m_pButton_Ranking->CreateFlip( "COMPETITION_CTF_PAGE_HISTORY_RANKING_BUTTON_FLIP", CBasicButton::MOUSEIN_FLIP );
		RegisterControl( m_pButton_Ranking );
	}
}

void RnCDMHistory::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	if ( ControlID >= RNCDM_HISTORY_BUFF_BEGIN )
	{
		if ( CHECK_MOUSE_IN ( dwMsg ) )
		{
			GLCaptureTheFieldClient* pCTFClient = m_pGaeaClient->GetCaptureTheField();
			GLCaptureTheFieldClient::CTFPREVSTATE& sState = pCTFClient->GetPrevState( m_emCTFType );

			NSGUI::SetCharMoveBlock();

			UINT nBuffIndex = (UINT)(ControlID - RNCDM_HISTORY_BUFF_BEGIN);
			CTF_REWARD_BUFF& sRewardBuff = sState.vecREWARDBUFF[nBuffIndex];

			GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();

			SNATIVEID sNativeID ( sRewardBuff.dwID );
			PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sNativeID );

			if ( pCharacter && pSkill )
			{	
				CString strName;
				strName = pSkill->GetName();

				CString strDesc;
				pSkill->GetDescFormatByLevel( strDesc, sRewardBuff.wLevel, GLSKILL::DESC_FORMAT_IMPACT | GLSKILL::DESC_FORMAT_SPEC );

				m_pInterface->BEGIN_COMMON_LINEINFO_MULTI( TEXT_ALIGN_LEFT );
				m_pInterface->ADD_COMMON_LINEINFO_MULTI( strName, NS_UITEXTCOLOR::GREENYELLOW );
				m_pInterface->ADD_COMMON_LINEINFO_MULTI( strDesc, NS_UITEXTCOLOR::WHITE );
				m_pInterface->END_COMMON_LINEINFO_MULTI();
			}
		}
	}

	switch ( ControlID )
	{
	case RNCDM_HISTORY_RANKING_BUTTON:
		{
			if( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_pInterface->OpenCDMTopRanking();
			}
		}
		break;
	}
}

CBasicTextBox* RnCDMHistory::CreateStaticControl(const char* szControlKeyword, CD3DFontPar* pFont, int nAlign )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, szControlKeyword, UI_FLAG_DEFAULT );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );	
	RegisterControl ( pStaticText );

	return pStaticText;
}

void RnCDMHistory::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
	Update();
}

void RnCDMHistory::Update()
{
	GLCaptureTheFieldClient* pCTFClient = m_pGaeaClient->GetCaptureTheField();
	GLCaptureTheFieldClient::CTFPREVSTATE& sState = pCTFClient->GetPrevState( m_emCTFType );

	// 진행시간
	{
		if ( !sState.IsNULL() && sState.tSTARTTIME && sState.tENDTIME )
		{
			CTime cStartTime( sState.tSTARTTIME );
			CTime cEndTime( sState.tENDTIME );

			m_pGaeaClient->GetConvertServerTime( cStartTime );
			m_pGaeaClient->GetConvertServerTime( cEndTime );

			std::string strFormat = sc::string::format("%02d : %02d ~ %02d : %02d", 
				cStartTime.GetHour(), cStartTime.GetMinute(),
				cEndTime.GetHour(), cEndTime.GetMinute()
				);

			m_pText_Time->SetOneLineText(strFormat.c_str(), NS_UITEXTCOLOR::WHITE);
		}
		else
		{
			m_pText_Time->SetOneLineText("-- : -- ~ -- : --", NS_UITEXTCOLOR::WHITE);
		}
	}

	// 승리학원
	{
		if ( !sState.IsNULL() )
		{
			std::string strControl;

			if ( sState.emAUTHSCHOOL == SCHOOL_NONE )
			{
				strControl = "COMPETITION_CTF_PAGE_HISTORY_AUTHSCHOOL_IMAGE_SCHOOL_NONE";
			}
			else
			{
				strControl = sc::string::format("COMPETITION_CTF_PAGE_HISTORY_AUTHSCHOOL_IMAGE_SCHOOL%1%", sState.emAUTHSCHOOL);
			}

			m_pImage_AuthSchool->SetImage( strControl.c_str() );
		}
		else
		{
			m_pImage_AuthSchool->SetImage( "COMPETITION_CTF_PAGE_HISTORY_AUTHSCHOOL_IMAGE_SCHOOL_NONE" );
		}

		const UIRECT& rcLocalBase  = m_pAuthSchoolControl->GetLocalPos();
		const UIRECT& rcGlobalBase = m_pAuthSchoolControl->GetGlobalPos();

		float fHalfX = m_pImage_AuthSchool->GetLocalPos().sizeX/2.0f;
		float fHalfY = m_pImage_AuthSchool->GetLocalPos().sizeY/2.0f;

		m_pImage_AuthSchool->SetLocalPos( D3DXVECTOR2(
			(rcLocalBase.left + rcLocalBase.right)/2.0f - fHalfX,
			(rcLocalBase.top  + rcLocalBase.bottom)/2.0f - fHalfY
			) );

		m_pImage_AuthSchool->SetGlobalPos( D3DXVECTOR2(
			(rcGlobalBase.left + rcGlobalBase.right)/2.0f - fHalfX,
			(rcGlobalBase.top  + rcGlobalBase.bottom)/2.0f - fHalfY
			) );
	}

	// 보상버프
	{
		UINT nMaxBuff = (UINT)sState.vecREWARDBUFF.size();

		VEC_SKILLIMAGE_ITER iter = m_vecSkillImage.begin();
		for (; iter!=m_vecSkillImage.end(); ++iter)
		{
			SKILLIMAGE& sSkillImage = (*iter);
			sSkillImage.pImage->SetVisibleSingle( FALSE );
			sSkillImage.pLine->SetVisibleSingle( FALSE );
		}

		if ( nMaxBuff != 0 )
		{
			m_pText_Buff->SetVisibleSingle( FALSE );

			const UIRECT& rcParant		= GetGlobalPos();
			const UIRECT& rcBase		= m_pBuffControl->GetLocalPos();
			const UIRECT& rcGap			= m_pBuffGapControl->GetLocalPos();
			const UIRECT& rcItem		= m_pBuffItemControl->GetLocalPos();

			float fBeginX = (rcBase.left + rcBase.right)/2.0f - ((rcItem.sizeX*(float)nMaxBuff) + (rcGap.sizeX*(float)(nMaxBuff-1)))/2.0f;
			float fPosX	  = fBeginX;
			float fPosY	  = (rcBase.top + rcBase.bottom)/2.0f - rcItem.sizeY/2.0f;

			VCTF_REWARD_BUFF_ITER RewardIter = sState.vecREWARDBUFF.begin();
			for ( UINT i=0; RewardIter!=sState.vecREWARDBUFF.end(); ++RewardIter, ++i )
			{
				CTF_REWARD_BUFF& sRewardBuff = (*RewardIter);

				CSkillImage*		pSkillImage = NULL;
				CBasicLineBoxSmart* pLineBox	= NULL;

				if ( i >= (UINT)m_vecSkillImage.size() )
				{
					GASSERT(m_ppd3dDevice);
					pSkillImage = new CSkillImage(m_pGaeaClient, m_pEngineDevice);
					pSkillImage->CreateSub ( this, "COMPETITION_CTF_PAGE_HISTORY_REWARDBUFF_ITEM", UI_FLAG_DEFAULT, RNCDM_HISTORY_BUFF_BEGIN + i );	
					pSkillImage->InitDeviceObjects ( (*m_ppd3dDevice) );
					pSkillImage->RestoreDeviceObjects ( (*m_ppd3dDevice) );
					pSkillImage->SetUseRender ( TRUE );
					RegisterControl ( pSkillImage );

					pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
					pLineBox->CreateSub( this, "COMPETITION_CTF_PAGE_HISTORY_REWARDBUFF_LINEBOX" );
					pLineBox->CreateSubControl( "COMPETITION_CTF_PAGE_INFO_LINEBOX_TEXINFO_W_BLACK" );
					pLineBox->InitDeviceObjects ( (*m_ppd3dDevice) );
					pLineBox->RestoreDeviceObjects ( (*m_ppd3dDevice) );
					RegisterControl( pLineBox );

					m_vecSkillImage.push_back(SKILLIMAGE(pSkillImage, pLineBox));
				}
				else
				{
					pSkillImage = m_vecSkillImage[i].pImage;
					pLineBox	= m_vecSkillImage[i].pLine;
				}

				pSkillImage->SetSkill( SNATIVEID(sRewardBuff.dwID) );
				pSkillImage->SetLocalPos( D3DXVECTOR2(fPosX, fPosY) );
				pSkillImage->SetGlobalPos( D3DXVECTOR2(rcParant.left + fPosX, rcParant.top + fPosY) );
				pSkillImage->SetVisibleSingle( TRUE );

				pLineBox->SetLocalPos( D3DXVECTOR2(fPosX-1.0f, fPosY-1.0f) );
				pLineBox->SetGlobalPos( D3DXVECTOR2(rcParant.left + fPosX - 1.0f, rcParant.top + fPosY - 1.0f) );
				pLineBox->SetVisibleSingle( TRUE );

				fPosX += (rcItem.sizeX + rcGap.sizeX);
			}
		}
		else
		{
			m_pText_Buff->SetVisibleSingle( TRUE );
		}
	}

	// 랭킹
	{
		if ( sState.vecRESULTDATA.empty() )
		{
			m_pText_Ranking->SetOneLineText(ID2GAMEWORD("COMPETITION_CTF_HISTORY", 5), NS_UITEXTCOLOR::WHITE);
		}
		else
		{
			std::string strFormat = sc::string::format(ID2GAMEWORD("COMPETITION_CTF_HISTORY", 6), 1, sState.vecRESULTDATA[0].szName);
			m_pText_Ranking->SetOneLineText(strFormat.c_str(), NS_UITEXTCOLOR::WHITE);
		}
	}
}
