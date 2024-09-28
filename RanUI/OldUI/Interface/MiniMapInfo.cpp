#include "StdAfx.h"
#include "../../../SigmaCore/String/StringUtils.h"
#include "../../../SigmaCore/File/StringFile.h"

#include "minimapinfo.h"
#include "../../../EngineLib/DxTools/TextureManager.h"
#include "../../../EngineLib/DxLand/DxLandMan.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../EngineLib/DxTools/DxRenderStates.h"
#include "../../../EngineLib/DxTools/DxViewPort.h"
#include "../../../EngineLib/GUInterface/InterfaceCfg.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../RanLogic/Land/GLMapAxisInfo.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"

#include "../../../RanLogic/RANPARAM.h"
#include "../../../RanLogicClient/Stage/DxGlobalStage.h"
#include "../../../RanLogicClient/CapturedTheField/GLCaptureTheFieldClient.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/G-Logic/GLPeriod.h"
#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLUseFeatures.h"
#include "../../../EngineLib/GUInterface/BasicProgressBar.h"
#include "../../../EngineLib/DxTools/DxClubMan.h"
#include "../../../RanLogicClient/Land/GLLandManClient.h"
#include "../../InnerInterface.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CMiniMapInfo::CMiniMapInfo(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
	: CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
	, m_nMiniPosX(0)
	, m_nMiniPosY(0)
	, m_pChannelTextBox(NULL)
	, m_pMinimapTextBox(NULL)
	, m_pClubMark(NULL)
	, m_pCTIMark(NULL)
{
}

CMiniMapInfo::~CMiniMapInfo(void)
{
}

void CMiniMapInfo::UpdatePostion ()
{
	CString strText;

	if ( m_pInterface->IsCHANNEL () )
	{
        DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
		strText.Format("[%d]", pGlobalStage->GetChannel());
		m_pChannelTextBox->SetOneLineText( strText, NS_UITEXTCOLOR::WHITE );

		strText.Format ( "%s %d/%d", m_strMapName, m_nMiniPosX, m_nMiniPosY );
		m_pMinimapTextBox->SetOneLineText ( strText, NS_UITEXTCOLOR::WHITE );
	}
	else
	{
		strText.Format ( "%s %d/%d", m_strMapName, m_nMiniPosX, m_nMiniPosY );
		m_pMinimapTextBox->SetOneLineText ( strText, NS_UITEXTCOLOR::WHITE );
	}
}

void CMiniMapInfo::SetMapPos ( float fCurX, float fCurY, int nMiniX, int nMiniY )
{
	if ( m_nMiniPosX != nMiniX || m_nMiniPosY != nMiniY )
	{
		m_nMiniPosX = nMiniX;
		m_nMiniPosY = nMiniY;

        UpdatePostion ();
	}
}

void CMiniMapInfo::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
    case MINIMAP_CTFMARK:
        {			
            if ( CHECK_MOUSE_IN ( dwMsg ) )
            {				
                std::string strDesc = ID2GAMEINTEXT("MINIMAP_INFO_CTFMARK");
                m_pInterface->SHOW_COMMON_LINEINFO ( strDesc.c_str(), NS_UITEXTCOLOR::DEFAULT );
            }
        }
        break;

	case MINIMAP_CTIMARK:
		{			
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{				
				CD3DFontPar* pFont = m_pEngineDevice->LoadFont (
					_DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

				m_pInterface->BEGIN_COMMON_LINEINFO_MULTI( TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP );
					
				CString strCombine;
				std::string strCountryName;

				Country::GLVictoriousCountryManClient* pVCountryMan =
					Country::GLVictoriousCountryManClient::Instance();
				if ( pVCountryMan )
				{
					strCountryName = pVCountryMan->GetVCountryNameFirst(
						Country::CAPTURE_THE_ISLANDS );
				}

				strCombine.Format( "%s", strCountryName.c_str() );
				m_pInterface->ADD_COMMON_LINEINFO_MULTI( strCombine, NS_UITEXTCOLOR::GREEN );

				m_pInterface->ADD_COMMON_LINEINFO_MULTI(
					ID2GAMEINTEXT("MINIMAP_INFO_CTIMARK"),
					NS_UITEXTUTIL::ENABLE );

				m_pInterface->END_COMMON_LINEINFO_MULTI();
			}
		}
		break;

	case MINIMAP_CLUBMARK:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{				
				m_pInterface->SHOW_COMMON_LINEINFO ( m_strClubName, NS_UITEXTCOLOR::DEFAULT );
			}
		}
		break;
	}
}

void CMiniMapInfo::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{	
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
	
	// Player Position Update 
	static D3DXVECTOR3 vPlayerPosBACK;
	const D3DXVECTOR3 &vPlayerPos = m_pGaeaClient->GetCharacterPos ();
	if ( vPlayerPos != vPlayerPosBACK )
	{
		vPlayerPosBACK = vPlayerPos;

		int nPosX(0), nPosY(0);
		GLLandManClient* pLandClient = m_pGaeaClient->GetActiveMap();
		if ( pLandClient )
		{
			const GLMapAxisInfo &sMapAxisInfo = pLandClient->GetMapAxisInfo();
			sMapAxisInfo.Convert2MapPos ( vPlayerPos.x, vPlayerPos.z, nPosX, nPosY );

			SetMapPos ( vPlayerPos.x, vPlayerPos.z, nPosX, nPosY );
		}
	}

    DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
	const CBasicTextBox::STEXTPART & sTEXTPART = m_pMinimapTextBox->GetTextPart ( 0 );
	INT nChannel = pGlobalStage->GetChannel();
	D3DXVECTOR2 vPos = D3DXVECTOR2(sTEXTPART.m_rcPart.left, sTEXTPART.m_rcPart.top);

	//	NOTE
	//		클럽
	GLLandManClient* pLand = m_pGaeaClient->GetActiveMap ();

	if ( pLand && pLand->m_dwGuidClub!=CLUB_NULL )
	{
		m_dwVer = pLand->m_dwGuidClubMarkVer;
		m_nID = pLand->m_dwGuidClub;
		m_nServer = static_cast<int>(m_pGaeaClient->GetCharacter()->m_dwServerID);
		m_strClubName = pLand->m_szGuidClubName;
		
		const UIRECT& rcGlobalPosClubMark = m_pClubMark->GetGlobalPos();
		vPos.x = vPos.x - (rcGlobalPosClubMark.sizeX+3.0f);

//#if defined( VN_PARAM ) 
		if (m_pGaeaClient->GetServiceProvider() == SP_VIETNAM)
		{
			vPos.y = sTEXTPART.m_rcPart.top + 2.0f;
		}
		else
		{
//#else
			vPos.y = sTEXTPART.m_rcPart.top + 1.0f;
		}
//#endif

		m_pClubMark->SetGlobalPos ( vPos );
		m_pClubMark->SetVisibleSingle ( TRUE );

	}
    else
    {
        m_pClubMark->SetVisibleSingle ( FALSE );
    }


	//! CTI 승리국가;
	if ( GLUseFeatures::GetInstance().IsUsingSelectCountry() )
	{
		Country::GLVictoriousCountryManClient* pVCountryMan =
			Country::GLVictoriousCountryManClient::Instance();
		if ( pVCountryMan )
		{
			// 이미지 ID 받아온다;
			std::string strMiniImageID;
			strMiniImageID = pVCountryMan->GetVCountryMiniImageIDFirst(
				Country::CAPTURE_THE_ISLANDS );

			if ( strMiniImageID.empty() )
			{
				m_pCTIMark->SetVisibleSingle( FALSE );
			}
			else
			{
				const UIRECT& rcGlobalPosMark = m_pCTIMark->GetGlobalPos();
				vPos.x = vPos.x - ( rcGlobalPosMark.sizeX + 3.0f );

				m_pCTIMark->SetImage( strMiniImageID.c_str() );
				m_pCTIMark->SetGlobalPos( vPos );
				m_pCTIMark->SetVisibleSingle( TRUE );
			}
		}
	}


    //! CTF 승리학원
    if ( GLUseFeatures::GetInstance().IsUsingCaptureTheField() )
    {
        using namespace PVP;
        GLCaptureTheFieldClient* pCTFClient = m_pGaeaClient->GetCaptureTheField();

        // (bjju.capturethefield) 현재 발전소 점령전의 상태를 가져온다
        GLCaptureTheFieldClient::CTFGLOBALSTATE& sCTFState = pCTFClient->GetGlobalState( PVP::ENUM_CTF_TYPE_POWERSTATION );

        if( sCTFState.emAUTHSCHOOL != SCHOOL_NONE )
        {
            const UIRECT& rcGlobalPosMark = m_pCTFMark->GetGlobalPos();
            vPos.x = vPos.x - ( rcGlobalPosMark.sizeX + 3.0f );

            std::string strControl = sc::string::format("MINIMAP_CTFMARK_SCHOOL%1%", sCTFState.emAUTHSCHOOL);

            m_pCTFMark->SetImage( strControl.c_str() );
            m_pCTFMark->SetGlobalPos( vPos );
            m_pCTFMark->SetVisibleSingle( TRUE );
        }
        else
        {
            m_pCTFMark->SetVisibleSingle( FALSE );
        }
    }

    //! 채널
    if( nChannel != -1 ) 
    {
        const CBasicTextBox::STEXTPART & sCTEXTPART = m_pChannelTextBox->GetTextPart ( 0 );
        vPos.x = vPos.x - ( sCTEXTPART.m_rcPart.sizeX + 3.0f );

        m_pChannelTextBox->SetGlobalPos( vPos );
        m_pChannelTextBox->SetVisibleSingle( TRUE );
    }
    else
    {
        m_pChannelTextBox->SetVisibleSingle( FALSE );
    }

}

HRESULT CMiniMapInfo::Render ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT( pd3dDevice );

	HRESULT hr = S_OK;

    BOOL bClubMarkVisible = m_pClubMark->IsVisible ();
    BOOL bCTFMarkVisible = m_pCTFMark->IsVisible();
	BOOL bCTIMarkVisible = FALSE;
	
	if ( m_pCTIMark )
		bCTIMarkVisible = m_pCTIMark->IsVisible();

	BOOL bChannelVisible = m_pChannelTextBox->IsVisible();

	m_pClubMark->SetVisibleSingle ( FALSE ); // MEMO
    m_pCTFMark->SetVisibleSingle( FALSE );
	m_pChannelTextBox->SetVisibleSingle( FALSE );
	{
		hr = CUIGroup::Render ( pd3dDevice );
		if (FAILED(hr))
            return hr;
	}
    m_pClubMark->SetVisibleSingle ( bClubMarkVisible );
    m_pCTFMark->SetVisibleSingle( bCTFMarkVisible );

	if ( m_pCTIMark )
		m_pCTIMark->SetVisibleSingle( bCTIMarkVisible );

	m_pChannelTextBox->SetVisibleSingle( bChannelVisible );
	
	if (bClubMarkVisible)
    {
		const clubmark::DXDATA& sMarkData = DxClubMan::GetInstance().GetClubData(
            pd3dDevice,
            m_nServer,
            m_nID );

		m_pClubMark->SetTexturePos ( 0, sMarkData.vTex_1_LU );
		m_pClubMark->SetTexturePos ( 1, sMarkData.vTex_2_RU );
		m_pClubMark->SetTexturePos ( 3, sMarkData.vTex_3_LD );
		m_pClubMark->SetTexturePos ( 2, sMarkData.vTex_4_RD );
		
		m_pClubMark->SetTexture ( sMarkData.pddsTexture );

		hr = m_pClubMark->Render ( pd3dDevice );
		if (FAILED(hr))
            return hr;
	}

    if ( bCTFMarkVisible )
    {
        hr = m_pCTFMark->Render ( pd3dDevice );
        if (FAILED(hr))
            return hr;
    }

	if ( bCTIMarkVisible )
	{
		if ( m_pCTIMark )
		{
			hr = m_pCTIMark->Render ( pd3dDevice );
			if ( FAILED( hr ) )
				return hr;
		}
	}

	if( bChannelVisible )
	{
		hr = m_pChannelTextBox->Render ( pd3dDevice );
		if (FAILED(hr))
            return hr;
	}

	return S_OK;
}

void CMiniMapInfo::CreateSubControl ()
{
	CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

	CBasicTextBox* pChannelTextBox = new CBasicTextBox(m_pEngineDevice);
	pChannelTextBox->CreateSub( this, "MINIMAP_CHANNEL" );
	pChannelTextBox->SetFont( pFont );
	pChannelTextBox->SetTextAlign( TEXT_ALIGN_BOTH_X );
	pChannelTextBox->SetDiffuseAlpha( 0 );
	pChannelTextBox->SetVisibleSingle( FALSE );
	RegisterControl( pChannelTextBox );
	m_pChannelTextBox = pChannelTextBox;

	CUIControl* pClubMark = new CUIControl(m_pEngineDevice);
	pClubMark->CreateSub( this, "MINIMAP_CLUBMARK", UI_FLAG_DEFAULT, MINIMAP_CLUBMARK );
	pClubMark->SetUseRender( TRUE );
	pClubMark->SetVisibleSingle( FALSE );
	RegisterControl( pClubMark );
	m_pClubMark = pClubMark;

    CSwapImage* pCTFMark = new CSwapImage(m_pEngineDevice);
    pCTFMark->CreateSub( this, "MINIMAP_CTFMARK", UI_FLAG_DEFAULT, MINIMAP_CTFMARK );
    pCTFMark->SetVisibleSingle( FALSE );
    RegisterControl( pCTFMark );
    m_pCTFMark = pCTFMark;

	if ( GLUseFeatures::GetInstance().IsUsingSelectCountry() )
	{
		CSwapImage* pCTIMark = new CSwapImage(m_pEngineDevice);
		pCTIMark->CreateSub( this, "MINIMAP_CTFMARK", UI_FLAG_DEFAULT, MINIMAP_CTIMARK );
		pCTIMark->SetVisibleSingle( FALSE );
		RegisterControl( pCTIMark );
		m_pCTIMark = pCTIMark;
	}

	CBasicTextBox* pMinimapTextBox = new CBasicTextBox(m_pEngineDevice);
	pMinimapTextBox->CreateSub ( this, "MINIMAP_POSITION" );
	pMinimapTextBox->SetFont ( pFont );
	pMinimapTextBox->SetTextAlign ( TEXT_ALIGN_RIGHT );
	RegisterControl ( pMinimapTextBox );
	m_pMinimapTextBox = pMinimapTextBox;
}