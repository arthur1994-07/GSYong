#include "StdAfx.h"
#include "./RnCDMTopRanking.h"

//#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/DxTools/DxClubMan.h"
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
#include "../../../RanLogic/Msg/ClubMsg.h"

#include "../../../EngineLib/DxTools/d3dfont.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

RnCDMTopRanking::RnCDMTopRanking(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
: CUIWindowEx(pInterface, pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_pTextBase_Rank(NULL)
, m_pTextBase_ClubMark_Name(NULL)
, m_pTextBase_Class(NULL)
, m_pTextBase_ClubMasterName(NULL)
, m_pCloseButton(NULL)
, m_pText_Time(NULL)
, m_lnStartTime(0)
, m_lnEndTime(0)
{
	//Blank
}

RnCDMTopRanking::~RnCDMTopRanking()
{
	//Blank
}

void RnCDMTopRanking::CreateSubControl ()
{
	std::string strBaseControl = GetControlNameEx();
	const int nAlignCenter = TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y;	

	//! 라인박스
	{
		CBasicLineBoxSmart* pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
		pLineBox->CreateSub( this, "COMPETITION_TOPRANKING_LINEBOX", UI_FLAG_DEFAULT );
		pLineBox->CreateSubControl( "COMPETITION_TOPRANKING_LINEBOX_TEXINFO" );
		RegisterControl( pLineBox );
	}

	//! 기본 텍스트
	{
		CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
		CD3DFontPar* pFont_Shadow = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

		m_pTextBase_Rank	= CreateStaticControl("RNCDM_TOPRANKING_TEXT_BASE_RANK", pFont_Shadow, NS_UITEXTCOLOR::ENABLE, nAlignCenter);
		m_pTextBase_ClubMark_Name	= CreateStaticControl("RNCDM_TOPRANKING_TEXT_BASE_CLUBMARK_NAME", pFont_Shadow, NS_UITEXTCOLOR::ENABLE, nAlignCenter);
		m_pTextBase_ClubMasterName	= CreateStaticControl("RNCDM_TOPRANKING_TEXT_BASE_CLUB_MASTER_NAME", pFont_Shadow, NS_UITEXTCOLOR::ENABLE, nAlignCenter);

		m_pTextBase_Rank->SetOneLineText( ID2GAMEWORD("RN_CMD_RANK_TEXT"), NS_UITEXTCOLOR::LIGHTGREY );
		m_pTextBase_ClubMark_Name->SetOneLineText( ID2GAMEWORD("RN_CDM_CLUB_NAME_TEXT"), NS_UITEXTCOLOR::LIGHTGREY );
		m_pTextBase_ClubMasterName->SetOneLineText( ID2GAMEWORD("RN_CDM_CLUB_MASTER_NAME_TEXT"), NS_UITEXTCOLOR::LIGHTGREY );
	}

	//! 랭킹 슬롯
	{
		m_vecSlot.resize(nMAX_SLOT);

		for ( UINT i=0; i<nMAX_SLOT; ++i )
		{
			RANKSLOT* &pSlot = m_vecSlot[i];

			pSlot = new RANKSLOT(m_pGaeaClient, m_pEngineDevice);
			pSlot->CreateSub( this, "COMPETITION_TOPRANKING_SLOT_BASE", UI_FLAG_DEFAULT );
			pSlot->CreateSubControl( this );
			RegisterControl( pSlot );

			const UIRECT& rcSlotRect = pSlot->GetLocalPos();
			pSlot->SetLocalPos( D3DXVECTOR2(rcSlotRect.left, rcSlotRect.top + (rcSlotRect.sizeY * static_cast<float>(i)) ) );
		}
	}

	{
		CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );
		m_pText_Time = new CBasicTextBox(m_pEngineDevice);
		m_pText_Time->CreateSub ( this, "RNCOMPETITION_CTF_RANK_TIME", UI_FLAG_DEFAULT );
		m_pText_Time->SetFont ( pFont );
		m_pText_Time->SetTextAlign ( TEXT_ALIGN_RIGHT | TEXT_ALIGN_CENTER_Y );	
		RegisterControl ( m_pText_Time );
		m_pText_Time->SetOneLineText("(--:--~--:--)", NS_UITEXTCOLOR::WHITE);
	}

	//! 닫기 버튼
	{
		m_pCloseButton = new CBasicTextButton(m_pEngineDevice);
		m_pCloseButton->CreateSub(this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, RNCDM_TOP_RANKING_BUTTON);
		m_pCloseButton->CreateBaseButton( "RNCDM_TOPRANKING_BUTTON_CLOSE", CBasicTextButton::SIZE19, 
			CBasicButton::CLICK_FLIP, ID2GAMEWORD("COMPETITION_TOPRANKING", 0) );
		RegisterControl( m_pCloseButton );
	}
}

void RnCDMTopRanking::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIWindowEx::Update(x,y,LB,MB,RB,nScroll,fElapsedTime,bFirstControl);

	//Update();
}

void RnCDMTopRanking::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage( ControlID, dwMsg );

	switch ( ControlID )
	{
	case RNCDM_TOP_RANKING_BUTTON:	
		{
			if( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				SetVisibleSingle(FALSE);
			}
		}
	};
}

void RnCDMTopRanking::SetCDMTopList(std::vector<DWORD> vecClubId,
									LONGLONG lnStartTime,
									LONGLONG lnEndTime,
									std::vector<DWORD> vecRank )
{
	m_vecClubId.clear();
	m_vecRank.clear(); 

	m_vecClubId = vecClubId;
	m_lnStartTime = lnStartTime;
	m_lnEndTime = lnEndTime;
	m_vecRank = vecRank; 

	for ( UINT i=0; i<nMAX_SLOT; ++i )
	{
		m_vecSlot[i]->SetEnable( false );
	}

         // CDM 이전 기록이 없다고 해도 CDM기록을 보여달라고함 (CDM참가자가 없었다는 정보를 알리기 위함);
	if ( //m_vecClubId.size() < 1 ||
		 //m_vecRank.size() < 1 ||
		 m_vecClubId.size()	!= m_vecRank.size() )
	{
		return;
	}

	GLMSG::SNET_CLUB_ID_2MASTERNAME_REQ_CA SendData;

	BOOST_FOREACH(DWORD dwClubId, m_vecClubId)
	{
		bool bFull = SendData.ADDID(dwClubId);

		if(!bFull)
			return;
	}

	msgpack::sbuffer SendBuffer;
	msgpack::pack(SendBuffer, SendData);
	m_pGaeaClient->NETSENDTOAGENT(NET_MSG_CLUB_ID_2MASTERNAME_REQ_CA, SendBuffer);
}

void RnCDMTopRanking::SetCDMProgessTime( LONGLONG lnStartTime, LONGLONG lnEndTime )
{
	CTime cStartTime( lnStartTime );
	CTime cEndTime( lnEndTime );

	if ( lnStartTime != 0 && lnEndTime != 0 )
	{

		m_pGaeaClient->GetConvertServerTime( cStartTime );
		m_pGaeaClient->GetConvertServerTime( cEndTime );

		std::string strFormat = sc::string::format("(%02d:%02d~%02d:%02d)", 
			cStartTime.GetHour(), cStartTime.GetMinute(),
			cEndTime.GetHour(), cEndTime.GetMinute()
			);

		m_pText_Time->SetOneLineText(strFormat.c_str(), NS_UITEXTCOLOR::WHITE);
	}
	else
	{
		m_pText_Time->SetOneLineText("(--:--~--:--)", NS_UITEXTCOLOR::WHITE);
	}
}

void RnCDMTopRanking::OpenCDMTopRanking()
{
	SetCDMProgessTime( m_lnStartTime, m_lnEndTime);

	if (m_vecClubId.size() < 1 ||
		m_vecRank.size() < 1 ||
		m_vecClubName.size() < 1 ||
		m_vecMasterName.size() < 1 )
		return;

	struct STOPRANKDATA 
	{
		DWORD m_dwClubId;
		std::string m_strClubName;
		std::string m_strMasterName;
		STOPRANKDATA::STOPRANKDATA( DWORD dwClubId, std::string strClubName, std::string strMasterName )
			: m_dwClubId(dwClubId)
			, m_strClubName(strClubName)
			, m_strMasterName(strMasterName)
		{

		}
	};

	std::multimap<DWORD,STOPRANKDATA> mapTopRankData;

	for ( unsigned i=0; i < m_vecClubId.size(); ++i )
	{
		mapTopRankData.insert( std::make_pair( m_vecRank[i],STOPRANKDATA( m_vecClubId[i], m_vecClubName[i], m_vecMasterName[i] )));
	}

	int i = 0;
	std::multimap<DWORD,STOPRANKDATA>::iterator iter;
	for (  iter = mapTopRankData.begin(); iter != mapTopRankData.end(); ++iter )
	{	
		m_vecSlot[i]->SetData( iter->first, 
			iter->second.m_dwClubId, 
			iter->second.m_strClubName, 
			iter->second.m_strMasterName, 
			0);
		m_vecSlot[i]->SetEnable( true );
		++i;
	}
}

void RnCDMTopRanking::SetClubMasterClubName( std::vector<std::string> vecMasterName, std::vector<std::string> vecClubName )
{
	m_vecClubName.clear();
	m_vecMasterName.clear();
	m_vecClubName = vecClubName;
	m_vecMasterName = vecMasterName;
}

//----------------------------------------------------------------------------------------------------//

void RnCDMTopRanking::RANKSLOT::CreateSubControl ( RnCDMTopRanking* pBase )
{
	GASSERT(pBase);

	CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
	CD3DFontPar* pFont_Shadow = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

	const int nAlignCenter = TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y;	

	{
		pTEXTRANK = CreateStaticControl("RNCDM_TOPRANKING_SLOT_TEXT_BASE_RANK", pFont_Shadow, nAlignCenter);
		pTEXTCLUBNAME = CreateStaticControl("RNCDM_TOPRANKING_SLOT_TEXT_BASE_CLUB_NAME", pFont_Shadow, nAlignCenter);
		pTEXTMASTERNAME = CreateStaticControl("RNCDM_TOPRANKING_SLOT_TEXT_BASE_NAME", pFont_Shadow, nAlignCenter);
	}

	{
		pIMAGECLUBMARK = new CUIControl(m_pEngineDevice);
		pIMAGECLUBMARK->CreateSub( this, "RNCDM_TOPRANKING_SLOT_IMAGE_CLUB_MARK",UI_FLAG_DEFAULT );
		pIMAGECLUBMARK->SetUseRender(TRUE);
		RegisterControl( pIMAGECLUBMARK );
	}

	{
		SetTextPosition( pTEXTRANK, pBase->GetBaseRectRank() );
		SetTextPosition( pTEXTMASTERNAME, pBase->GetBaseRectName() );
		SetImagePosition( pIMAGECLUBMARK, pBase->GetBaseRectCLubName(), GetLocalPos() );
	}

	{
		pTEXTRANK->SetOneLineText( "", NS_UITEXTCOLOR::WHITE );
		pTEXTCLUBNAME->SetOneLineText( "", NS_UITEXTCOLOR::WHITE );
		pTEXTMASTERNAME->SetOneLineText( "", NS_UITEXTCOLOR::WHITE );
	}

	bENABLE = true;
}

void RnCDMTopRanking::RANKSLOT::Reset()
{
	SetEnable( false );
	strNAME = "";
}

void RnCDMTopRanking::RANKSLOT::SetEnable( const bool bEnable )
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

void RnCDMTopRanking::RANKSLOT::SetData( const DWORD nRank, 
										DWORD dwClubDbNum, 
										std::string strClubName, 
										std::string strClubMasterName,
										DWORD dwClubMarkVer )
{
	pTEXTRANK->SetOneLineText( sc::string::format("%2d", nRank).c_str(), NS_UITEXTCOLOR::WHITE);
	pTEXTCLUBNAME->SetOneLineText( strClubName.c_str(), NS_UITEXTCOLOR::WHITE);
	pTEXTMASTERNAME->SetOneLineText( strClubMasterName.c_str(), NS_UITEXTCOLOR::WHITE);
	m_dwClubDbnum = dwClubDbNum;
}

void RnCDMTopRanking::RANKSLOT::SetTextPosition( CBasicTextBox* pTextBox, const UIRECT& rcBase )
{
	GASSERT(pTextBox);

	UIRECT rcTextBox = pTextBox->GetLocalPos();
	rcTextBox.left  = rcBase.left;
	rcTextBox.right = rcBase.right;
	rcTextBox.sizeX = rcBase.sizeX;

	pTextBox->SetLocalPos( rcTextBox );
}

void RnCDMTopRanking::RANKSLOT::SetImagePosition( CSwapImage* pImage, const UIRECT& rcBase, const UIRECT& rcParent )
{
	GASSERT(pImage);

	UIRECT rcImage = pImage->GetLocalPos();
	rcImage.left   = (rcBase.left + rcBase.right)/2.0f - rcImage.sizeX/2.0f;
	rcImage.right  = rcImage.left + rcImage.sizeX;
	rcImage.top    = rcImage.top + rcParent.sizeY/2.0f - rcImage.sizeY/2.0f;
	rcImage.bottom = rcImage.top + rcImage.sizeY;

	pImage->SetLocalPos( rcImage );
}

void RnCDMTopRanking::RANKSLOT::SetImagePosition( CUIControl* pImage, const UIRECT& rcBase, const UIRECT& rcParent )
{
	GASSERT(pImage);

	UIRECT rcImage = pImage->GetLocalPos();
	//rcImage.left   = (rcBase.left + rcBase.right)/2.0f - rcImage.sizeX/2.0f;
	rcImage.left   = rcBase.left;
	//rcImage.right  = rcImage.left + rcImage.sizeX;
	rcImage.top    = rcImage.top + rcParent.sizeY/2.0f - rcImage.sizeY/2.0f;
	//rcImage.bottom = rcImage.top + rcImage.sizeY;

	pImage->SetLocalPos( rcImage );
}

CBasicTextBox* RnCDMTopRanking::RANKSLOT::CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, szControlKeyword, UI_FLAG_DEFAULT, cID );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );	
	RegisterControl ( pStaticText );

	return pStaticText;
}

HRESULT RnCDMTopRanking::RANKSLOT::Render( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if (m_dwClubDbnum == -1)
		return S_FALSE;

	HRESULT hr = S_OK;
	pIMAGECLUBMARK->SetVisibleSingle ( FALSE );
	{
		hr = CUIGroup::Render ( pd3dDevice );
		if ( FAILED ( hr ) )
			return hr;
	}
	pIMAGECLUBMARK->SetVisibleSingle ( TRUE );

	int nServer = static_cast<int>(pGAEACLIENT->GetCharacter()->m_dwServerID);
	int nID = static_cast<int>(m_dwClubDbnum);

	const clubmark::DXDATA& sMarkData = DxClubMan::GetInstance().GetClubData ( m_pEngineDevice->GetDevice(), nServer, nID );

	if (&sMarkData)
	{
		pIMAGECLUBMARK->SetTexturePos ( 0, sMarkData.vTex_1_LU );
		pIMAGECLUBMARK->SetTexturePos ( 1, sMarkData.vTex_2_RU );
		pIMAGECLUBMARK->SetTexturePos ( 3, sMarkData.vTex_3_LD );
		pIMAGECLUBMARK->SetTexturePos ( 2, sMarkData.vTex_4_RD );

		//	만들어야함
		pIMAGECLUBMARK->SetTexture ( sMarkData.pddsTexture );

		hr = S_OK;
		hr = pIMAGECLUBMARK->Render ( pd3dDevice );
		return hr;
	}
	return hr;
}

//----------------------------------------------------------------------------------------------------//

MyRnCDMTopRanking::MyRnCDMTopRanking( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: RnCDMTopRanking( pGaeaClient, pInterface, pEngineDevice )
{
}

void MyRnCDMTopRanking::CreateUIWindowAndRegisterOwnership()
{
	RnCDMTopRanking::Create( RNCDM_TOP_RANKING, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	RnCDMTopRanking::CreateBaseWindowLightGray  ( "COMPETITION_TOPRANKING", (char*)ID2GAMEWORD("COMPETITION_TOPRANKING", 1) );
	RnCDMTopRanking::CreateSubControl();
	RnCDMTopRanking::SetAlignFlag( UI_FLAG_CENTER_Y | UI_FLAG_CENTER_X );
	RnCDMTopRanking::m_pInterface->UiRegisterControl( this, true );
	RnCDMTopRanking::m_pInterface->UiShowGroupFocus( RNCDM_TOP_RANKING );
}

void MyRnCDMTopRanking::OpenCDMTopRanking()
{
	RnCDMTopRanking::OpenCDMTopRanking();
	UIRECT rcPos   = RnCDMTopRanking::m_pInterface->GetCompetitionWindowPos();
	UIRECT rcMyPos = RnCDMTopRanking::GetGlobalPos();

	rcPos = UIRECT(rcPos.right, rcPos.top, rcMyPos.sizeX, rcMyPos.sizeY);

	RnCDMTopRanking::SetGlobalPos( rcPos );
	//RnCDMTopRanking::SetCTFType( emType );
	//CCompetitionTopRanking::SetVisibleSingle(TRUE);
	RnCDMTopRanking::m_pInterface->UiShowGroupFocus( RNCDM_TOP_RANKING );
}

void MyRnCDMTopRanking::Close()
{
	RnCDMTopRanking::SetVisibleSingle(FALSE);
}

void MyRnCDMTopRanking::SetCDMTopList(std::vector<DWORD> vecClubId,
									  LONGLONG lnStartTime,
									  LONGLONG lnEndTime,
									  std::vector<DWORD> vecRank )
{
	RnCDMTopRanking::SetCDMTopList(vecClubId, lnStartTime, lnEndTime, vecRank );
}

void MyRnCDMTopRanking::SetClubMasterClubName( std::vector<std::string> vecMasterName, std::vector<std::string> vecClubName )
{
	RnCDMTopRanking::SetClubMasterClubName( vecMasterName, vecClubName );
}
