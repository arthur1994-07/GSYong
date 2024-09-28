#include "StdAfx.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/DxInputDevice.h"
#include "../../../EngineLib/DxTools/DxViewPort.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../RanLogic/GLUseFeatures.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../RanLogic/Land/GLMapAxisInfo.h"

#include "../../../RanLogicClient/Club/GLClubClient.h"
#include "../../../RanLogicClient/Tutorial/GLBaseTutorial.h" // by luxes.

#include "../../StaticUIManager.h"
#include "../../InnerInterface.h"
#include "../Tutorial/MinimapTarget.h"

//////////////////////////////////////////////////////////////////////////

#include "../../../RanLogicClient/Party/GLPartyClient.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Stage/DxGlobalStage.h"
#include "../../../RanLogicClient/Land/GLLandManClient.h"
#include "../../../RanLogicClient/CapturedTheField/GLCaptureTheFieldClient.h"
#include "../../../RanLogicClient/Input/GLInputManager.h"


#include "RnOldLargeMapWindow.h"

#include "RnMapHelper.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"

// ----------------------------------------------------------------------------




//boost::pool<> RnOldLargeMapWindow::Mark::s_pool( sizeof(RnOldLargeMapWindow::Mark) );




	//////////////////////////////////////////////////////////////////////////

RnOldLargeMapWindow::RnOldLargeMapWindow(GLGaeaClient* pGaeaClient,
										 CInnerInterface* pInterface,
										 EngineDeviceMan* pEngineDevice,
										 RnMapPrivate::ISettingInterface* pSettingInterface,
										 RnMapPrivate::IMapInterface* pChangeMap,
										 RnMapPrivate::IMapInfoInterface* pInfoInterface,
										 RnMapPrivate::IMapSearchInterface* pSearchInterface) 
	: CUIGroupHelper(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
	, m_pInterface(pInterface)
	, m_pMousePosText(NULL)
	, m_pMousePosBack(NULL)
	, m_pScrollBarH(NULL)
	, m_pScrollBarV(NULL)
	, m_pMap(NULL)
	, m_pPlayerMark(NULL)
	, m_pPlayerDirection(NULL)
	, m_pLBThumb(NULL)
	, m_pRBThumb(NULL)
	, m_pTargetMark(NULL)
	, m_iTargetX( INT_MIN )
	, m_iTargetY( INT_MIN )
{
//	SecureZeroMemory( m_PartyMarkVec, sizeof(m_PartyMarkVec) );
	Initialize();

    m_pLevelFile = new GLLevelFileClient(m_pGaeaClient);
	m_pLevelFile->SetBaseMapID(SNATIVEID(false));
	m_pLevelFile->SetPeaceZone(m_pLevelFile->IsPeaceZone());
	m_pLevelFile->SetPKZone(m_pLevelFile->IsPKZone());

	m_isEnableLButton = true;

	m_pSettingInterface = pSettingInterface;
	m_pChangeMap = pChangeMap;
	m_pInfoInterface = pInfoInterface; 
	m_pSearchInterface = pSearchInterface;

	m_pSearchCharacter = NULL;

	
}

RnOldLargeMapWindow::~RnOldLargeMapWindow ()
{
	m_vecGuild.clear();
    m_vecCTF.clear();
    m_vecCTFAuthenticator.clear();

    delete m_pLevelFile;
    m_pLevelFile = NULL;
}

void RnOldLargeMapWindow::ConvIntDataFloatData()
{
	m_vWORLD_START = D3DXVECTOR2( (float)m_nWORLD_START_X, (float)m_nWORLD_START_Y );
	m_vWORLD_SIZE = D3DXVECTOR2( (float)m_nWORLD_SIZE_X, (float)m_nWORLD_SIZE_Y );
	m_vMAP_TEXSIZE = D3DXVECTOR2( (float)m_nMAP_TEXSIZE_X, (float)m_nMAP_TEXSIZE_Y );
}

void RnOldLargeMapWindow::CreateSubControl ()
{
	{	//	가로 스크롤바
		CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
		pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR_H", UI_FLAG_BOTTOM | UI_FLAG_XSIZE, LARGEMAP_SCROLLBAR_H );
		pScrollBar->CreateBaseScrollBar ( "LARGEMAP_SCROLLBAR_H", false );
		RegisterControl ( pScrollBar );
		m_pScrollBarH = pScrollBar;
		m_pScrollBarH->SetVisibleSingle ( FALSE );
	}

	{	//	세로 스크롤바
		CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
		pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, LARGEMAP_SCROLLBAR_V );
		pScrollBar->CreateBaseScrollBar ( "LARGEMAP_SCROLLBAR_V" );
		RegisterControl ( pScrollBar );
		m_pScrollBarV = pScrollBar;
		m_pScrollBarV->SetVisibleSingle ( FALSE );
	}

	{
		CUIControl*	pMapImage = new CUIControl(m_pEngineDevice);
		pMapImage->CreateSub ( this, "MAP_WINDOW_MAP_IMAGE", UI_FLAG_XSIZE | UI_FLAG_YSIZE, LARGEMAP_IMAGE );
		RegisterControl ( pMapImage );
		m_pMap = pMapImage;
	}



	CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 8, D3DFONT_SHADOW | D3DFONT_ASCII );
	{
		CUIControl* pMousePosBack = new CUIControl(m_pEngineDevice);
		pMousePosBack->CreateSub ( this, "MAP_WINDOW_MOUSEPOS_BACK", UI_FLAG_BOTTOM | UI_FLAG_RIGHT );
		RegisterControl ( pMousePosBack );		
		m_pMousePosBack = pMousePosBack;

		CBasicTextBox*	pTextBox = new CBasicTextBox(m_pEngineDevice);
		pTextBox->CreateSub ( this, "MAP_WINDOW_MOUSEPOS_TEXT", UI_FLAG_BOTTOM | UI_FLAG_RIGHT );
		pTextBox->SetFont ( pFont );
		pTextBox->SetTextAlign ( TEXT_ALIGN_CENTER_X );
		RegisterControl ( pTextBox );
		m_pMousePosText = pTextBox;
	}

	{
		CUIControl* pDirection = new CUIControl(m_pEngineDevice);
		pDirection->CreateSub ( this, "MAP_WINDOW_DIRECTION", UI_FLAG_BOTTOM, LARGEMAP_DIRECTION );		
		RegisterControl ( pDirection );
	}

	{
		CBasicButton* pRBThumb = new CBasicButton(m_pEngineDevice);
		pRBThumb->CreateSub ( this, "LARGEMAP_RB_THUMB", UI_FLAG_RIGHT | UI_FLAG_BOTTOM, LARGEMAP_RB_THUMB );
		pRBThumb->CreateFlip ( "LARGEMAP_RB_THUMB_F", CBasicButton::MOUSEIN_FLIP );
		RegisterControl ( pRBThumb );
		m_pRBThumb = pRBThumb;
		m_pRBThumb->SetVisibleSingle(FALSE);

		CBasicButton* pLBThumb = new CBasicButton(m_pEngineDevice);
		pLBThumb->CreateSub ( this, "LARGEMAP_LB_THUMB", UI_FLAG_BOTTOM, LARGEMAP_LB_THUMB );
		pLBThumb->CreateFlip ( "LARGEMAP_LB_THUMB_F", CBasicButton::MOUSEIN_FLIP );
		RegisterControl ( pLBThumb );
		m_pLBThumb = pLBThumb;
		m_pLBThumb->SetVisibleSingle(FALSE);

	}



	// 미니맵 마우스 좌클릭 목표지점 표시
	{
		m_pTargetMark = new CUIControl(m_pEngineDevice);
		m_pTargetMark->CreateSub( this, "TARGET_MARK" );
		m_pTargetMark->SetVisibleSingle( FALSE );
		RegisterControl( m_pTargetMark );
	}


	// 항상 최상위여야하므로
	// 플레이어 위치 및 방향
	{
		CUIControl* pPlayerMark = new CUIControl(m_pEngineDevice);
		pPlayerMark->CreateSub ( this, "MAP_WINDOW_SETTING_MY_PLAYER_MARK", UI_FLAG_DEFAULT, PLAYER_MARK );		
		RegisterControl ( pPlayerMark );
		m_pPlayerMark = pPlayerMark;

		CUIControl* pPlayerDirection = new CUIControl( m_pEngineDevice );
		pPlayerDirection->CreateSub( this, "MAP_WINDOW_SETTING_MY_PLAYER_MARK_DIRECTION", UI_FLAG_DEFAULT, PLAYER_MARK_DIRECTION );
		RegisterControl( pPlayerDirection );
		m_pPlayerDirection = pPlayerDirection;
	}
}

HRESULT RnOldLargeMapWindow::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT( pd3dDevice );
	m_pd3dDevice = pd3dDevice;

	HRESULT hr = S_OK;
	hr = CUIGroupHelper::InitDeviceObjects ( pd3dDevice );
	if ( FAILED ( hr ) ) return hr;

	return S_OK;
}

//************************************
// Method:    SetDefaultPosition
// FullName:  RnOldLargeMapWindow::SetDefaultPosition
// Access:    public 
// Returns:   void
// Qualifier: kdhong
//************************************
void RnOldLargeMapWindow::SetDefaultPosition()
{
	const UIRECT& rcGlobalPos = GetGlobalPos ();
	UIRECT rcTemp;

	WORD wAlignFlag = GetAlignFlag ();			
	SetAlignFlag ( UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	{
		AlignSubControl( rcGlobalPos, rcTemp );

		long lResolution = CUIMan::GetResolution ();
		rcTemp.left = HIWORD(lResolution) - rcTemp.sizeX;

		SetGlobalPos( rcTemp );
	}
	SetAlignFlag( wAlignFlag );
}

void RnOldLargeMapWindow::SetMapLevelFile(const GLLevelFileClient& levFile, const CString& strMapName )
{
	if( m_pLevelFile->GetBaseMapID() == levFile.GetBaseMapID() )
		return;

    delete m_pLevelFile;
    m_pLevelFile = new GLLevelFileClient(m_pGaeaClient);
	*m_pLevelFile = levFile;

	const GLMapAxisInfo& sInfo = levFile.GetMapAxisInfo();

	Initialize();

	m_pMap->UnLoadTexture ();
	m_pMap->SetTextureName (NULL);
	m_pMap->SetUseRender ( FALSE );

	// 맵 이동시 미니맵의 크기와 위치를 초기화 시킨다.
	// 이유는 미니맵의 사이즈가 1024와 512 두가지가 존재하기 때문에
	// 큰 맵에서 작은 맵으로의 이동시 문제가 생긴다.
//	SetDefaultPosition();

	const D3DXVECTOR4 vMapTex = sInfo.GetMiniMapTexPos();	 
	const UIRECT& rcGlobalPos = m_pMap->GetGlobalPos ();

	{	//	맵 텍스쳐 크기
		const D3DXVECTOR2 vMinimapTexSize = sInfo.GetMiniMapTexSize();
		m_nMAP_TEXSIZE_X = static_cast<int>(vMinimapTexSize.x);
		m_nMAP_TEXSIZE_Y = static_cast<int>(vMinimapTexSize.y);
	}

	{	//	가로
		CBasicScrollThumbFrame* pThumbFrame = m_pScrollBarH->GetThumbFrame ();
		pThumbFrame->SetState ( m_nMAP_TEXSIZE_X, static_cast<int>(rcGlobalPos.sizeX) );
		pThumbFrame->SetPercent ( 0.0f );
	}

	{	//	세로
		CBasicScrollThumbFrame* pThumbFrame = m_pScrollBarV->GetThumbFrame ();
		pThumbFrame->SetState ( m_nMAP_TEXSIZE_Y, static_cast<int>(rcGlobalPos.sizeY) );
		pThumbFrame->SetPercent ( 0.0f );
	}

	{
		m_nWORLD_START_X = sInfo.GetMapStartX ();
		m_nWORLD_START_Y = sInfo.GetMapStartY ();
		m_nWORLD_SIZE_X = sInfo.GetMapSizeX ();
		m_nWORLD_SIZE_Y = sInfo.GetMapSizeY ();
	}

	//	Exception
	{
		m_nMAP_TEXSIZE_X = (m_nMAP_TEXSIZE_X) ? m_nMAP_TEXSIZE_X : 1;
		m_nMAP_TEXSIZE_Y = (m_nMAP_TEXSIZE_Y) ? m_nMAP_TEXSIZE_Y : 1;

		m_nWORLD_SIZE_X = (m_nWORLD_SIZE_X) ? m_nWORLD_SIZE_X : 1;
		m_nWORLD_SIZE_Y = (m_nWORLD_SIZE_Y) ? m_nWORLD_SIZE_Y : 1;
	}

	m_vRenderSize.x = rcGlobalPos.sizeX / m_nMAP_TEXSIZE_X;
	m_vRenderSize.y = rcGlobalPos.sizeY / m_nMAP_TEXSIZE_Y;

	m_vRenderSize.x *= 10000;
	m_vRenderSize.x = floor(m_vRenderSize.x);
	m_vRenderSize.x *= 0.0001f;

	m_vRenderSize.y *= 10000;
	m_vRenderSize.y = floor(m_vRenderSize.y);
	m_vRenderSize.y *= 0.0001f;

	// 맵 텍스쳐 체인지
	UIRECT rcTexture( 0.0f, 0.0f, m_vRenderSize.x, m_vRenderSize.y );
	m_pMap->SetTextureName ( sInfo.GetMinMapTex() );
	m_pMap->SetTexturePos ( rcTexture );

	// 텍스쳐 만들기
	m_pMap->LoadTexture ( m_pd3dDevice );
	m_pMap->SetUseRender ( TRUE );

// 	m_vScrollGap.x = m_nMAP_TEXSIZE_X - rcGlobalPos.sizeX;
// 	m_vScrollGap.y = m_nMAP_TEXSIZE_Y - rcGlobalPos.sizeY;
	MakeScrollGap();

	m_nVIEW_SIZE_X = static_cast<int>(rcGlobalPos.sizeX);
	m_nVIEW_SIZE_Y = static_cast<int>(rcGlobalPos.sizeY);

	ConvIntDataFloatData();

	//////////////////////////////////////////////////////////////////////////

	MakeNpcInfo();
	

	//////////////////////////////////////////////////////////////////////////
	MakeMonsterInfo();


	//////////////////////////////////////////////////////////////////////////
	MakeSearchCharacter();

}

void RnOldLargeMapWindow::Update ( int x ,int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return ;

	m_PosX = x;
	m_PosY = y;

	nScroll = 0;
	CUIGroupHelper::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
	
	UpdateWithManualFor_NoID_UI(x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	Update(bFirstControl);	
}

void RnOldLargeMapWindow::Update(BOOL bFirstControl)
{	
	int x= m_PosX,y=m_PosY;

	if ( m_pLevelFile->GetBaseMapID() == NATIVEID_NULL() )
		return;

	if( m_pGaeaClient->IsInstantMap() )
	{
		SetVisibleSingle( FALSE );
		return;
	}
	
	//m_pGaeaClient->GetActiveMap() == ;


	if ( bFirstControl )
	{
		if ( m_pMap->IsExclusiveSelfControl() ) //	마우스로 드래깅시 이미지 움직이기
		{	
			D3DXVECTOR2 vMOVE ( m_vMOUSE_BACK.x - x, m_vMOUSE_BACK.y - y );

			if ( 0.0f != vMOVE.x )
			{	
				const float fMovePercent = vMOVE.x / m_vScrollGap.x;
				CBasicScrollThumbFrame* pThumbFrame = m_pScrollBarH->GetThumbFrame ();
				const float& fPercent = pThumbFrame->GetPercent ();
				float fNewPercent = fPercent + fMovePercent;
				fNewPercent = max ( 0.0f, fNewPercent );
				fNewPercent = min ( 1.0f, fNewPercent );
				pThumbFrame->SetPercent ( fNewPercent );
			}

			if ( 0.0f != vMOVE.y )
			{
				const float fMovePercent = vMOVE.y / m_vScrollGap.y;
				CBasicScrollThumbFrame* pThumbFrame = m_pScrollBarV->GetThumbFrame ();
				const float& fPercent = pThumbFrame->GetPercent ();
				float fNewPercent = fPercent + fMovePercent;
				fNewPercent = max ( 0.0f, fNewPercent );
				fNewPercent = min ( 1.0f, fNewPercent );
				pThumbFrame->SetPercent ( fNewPercent );
			}

			SetAutoMove ( false );

			m_vMOUSE_BACK.x = (float)x;
			m_vMOUSE_BACK.y = (float)y;
		}

		{	//	세로 스크롤
			CBasicScrollThumbFrame* pThumbFrame = m_pScrollBarV->GetThumbFrame ();		
			const float fPercentV = pThumbFrame->GetPercent ();		
			if ( fPercentV != m_fPercentV_BACK )
			{
				const float fMOVE = m_vScrollGap.y * fPercentV;
				m_vRenderPos.y = fMOVE / m_nMAP_TEXSIZE_Y;

				m_vRenderPos.y *= 10000.0f;
				m_vRenderPos.y = floor(m_vRenderPos.y);
				m_vRenderPos.y *= 0.0001f;

				UIRECT rcTexture( m_vRenderPos.x, m_vRenderPos.y, m_vRenderSize.x, m_vRenderSize.y );
				m_pMap->SetTexturePos ( rcTexture );				

				m_fPercentV_BACK = fPercentV;

				m_nVIEW_START_Y = static_cast<int>(fMOVE);

				if ( m_nVIEW_START_Y + m_nVIEW_SIZE_Y > m_nMAP_TEXSIZE_Y ) m_nVIEW_START_Y = m_nMAP_TEXSIZE_Y - m_nVIEW_SIZE_Y;
				if ( m_nVIEW_START_Y < 0 ) m_nVIEW_START_Y = 0;

			}
		}

		{	//	가로 스크롤
			CBasicScrollThumbFrame* pThumbFrame = m_pScrollBarH->GetThumbFrame ();
			const float fPercentH = pThumbFrame->GetPercent ();
			if ( fPercentH != m_fPercentH_BACK )
			{
				const float fMOVE = m_vScrollGap.x * fPercentH;
				m_vRenderPos.x = fMOVE / m_nMAP_TEXSIZE_X;

				m_vRenderPos.x *= 10000.0f;
				m_vRenderPos.x = floor(m_vRenderPos.x);
				m_vRenderPos.x *= 0.0001f;

				UIRECT rcTexture( m_vRenderPos.x, m_vRenderPos.y, m_vRenderSize.x, m_vRenderSize.y );
				m_pMap->SetTexturePos ( rcTexture );

				m_fPercentH_BACK = fPercentH;

				m_nVIEW_START_X = static_cast<int>(fMOVE);
				if ( m_nVIEW_START_X + m_nVIEW_SIZE_X > m_nMAP_TEXSIZE_X ) m_nVIEW_START_X = m_nMAP_TEXSIZE_X - m_nVIEW_SIZE_X;
				if ( m_nVIEW_START_X < 0 ) m_nVIEW_START_X = 0;

			}
		}

		if ( m_pRBThumb->IsExclusiveSelfControl() )  // 우측 미니맵 늘이기
		{
			const UIRECT& rcGlobalPos = GetGlobalPos ();
			D3DXVECTOR2 vDELTA;

			vDELTA.x = static_cast<float>(x) - rcGlobalPos.left;			
			vDELTA.y = static_cast<float>(y) - rcGlobalPos.top;

			const UIRECT& rcGlobalMapPos = m_pMap->GetGlobalPos ();
			float fGAP_SIZEX = (rcGlobalPos.sizeX - rcGlobalMapPos.sizeX);
			float fGAP_SIZEY = (rcGlobalPos.sizeY - rcGlobalMapPos.sizeY);

			long lResolution = CUIMan::GetResolution ();
			WORD X_RES = HIWORD(lResolution);
			WORD Y_RES = LOWORD(lResolution);

			{				
				float fMAP_DELTAX = m_nMAP_TEXSIZE_X + fGAP_SIZEX;
				if ( fMAP_DELTAX <= vDELTA.x )
				{
					vDELTA.x = fMAP_DELTAX;
				}
				if ( vDELTA.x >= X_RES )
				{
					vDELTA.x = X_RES;
				}
			}

			{
				float fMAP_DELTAY = (float)m_nMAP_TEXSIZE_Y + fGAP_SIZEY;
				if ( fMAP_DELTAY <= vDELTA.y )
				{
					vDELTA.y = fMAP_DELTAY;
				}

				if ( vDELTA.y >= Y_RES )
				{
					vDELTA.y = Y_RES;
				}
			}

			//
			//float fMAP_DELTAY = m_nMAP_TEXSIZE_Y + fGAP_SIZEY;
			//if ( fMAP_DELTAX <= vDELTA.x )
			//{
			//	vDELTA.x = fMAP_DELTAX;
			//}
			//if ( fMAP_DELTAY <= vDELTA.y )
			//{
			//	vDELTA.y = fMAP_DELTAY;
			//}

			WORD wAlignFlag = GetAlignFlag ();			
			SetAlignFlag ( UI_FLAG_XSIZE | UI_FLAG_YSIZE );
			{
				UIRECT rcNewPos( rcGlobalPos.left, rcGlobalPos.top, vDELTA.x, vDELTA.y );
				AlignSubControl( rcGlobalPos, rcNewPos );
				SetGlobalPos( rcNewPos );
			}
			SetAlignFlag ( wAlignFlag );

			{
				const UIRECT& rcGlobalPos = m_pMap->GetGlobalPos ();

				m_vRenderSize.x = rcGlobalPos.sizeX / m_nMAP_TEXSIZE_X;
				m_vRenderSize.y = rcGlobalPos.sizeY / m_nMAP_TEXSIZE_Y;

				m_nVIEW_SIZE_X = static_cast<int>(rcGlobalPos.sizeX);
				m_nVIEW_SIZE_Y = static_cast<int>(rcGlobalPos.sizeY);

				if ( m_nVIEW_START_X + m_nVIEW_SIZE_X > m_nMAP_TEXSIZE_X ) m_nVIEW_START_X = m_nMAP_TEXSIZE_X - m_nVIEW_SIZE_X;
				if ( m_nVIEW_START_X < 0 ) m_nVIEW_START_X = 0;

				if ( m_nVIEW_START_Y + m_nVIEW_SIZE_Y > m_nMAP_TEXSIZE_Y ) m_nVIEW_START_Y = m_nMAP_TEXSIZE_Y - m_nVIEW_SIZE_Y;
				if ( m_nVIEW_START_Y < 0 ) m_nVIEW_START_Y = 0;

				m_vRenderPos.x = static_cast<float>(m_nVIEW_START_X) / static_cast<float>(m_nMAP_TEXSIZE_X);
				m_vRenderPos.y = static_cast<float>(m_nVIEW_START_Y) / static_cast<float>(m_nMAP_TEXSIZE_Y);

				m_vRenderSize.x *= 10000;
				m_vRenderSize.x = floor(m_vRenderSize.x);
				m_vRenderSize.x *= 0.0001f;

				m_vRenderSize.y *= 10000;
				m_vRenderSize.y = floor(m_vRenderSize.y);
				m_vRenderSize.y *= 0.0001f;

				// 맵 텍스쳐 체인지
				UIRECT rcTexture( m_vRenderPos.x, m_vRenderPos.y, m_vRenderSize.x, m_vRenderSize.y );
				m_pMap->SetTexturePos ( rcTexture );

// 				m_vScrollGap.x = m_nMAP_TEXSIZE_X - rcGlobalPos.sizeX;
// 				m_vScrollGap.y = m_nMAP_TEXSIZE_Y - rcGlobalPos.sizeY;
				MakeScrollGap();

				m_pScrollBarH->GetThumbFrame()->SetState ( m_nMAP_TEXSIZE_X, m_nVIEW_SIZE_X );
				m_pScrollBarV->GetThumbFrame()->SetState ( m_nMAP_TEXSIZE_Y, m_nVIEW_SIZE_Y );
			}
		}

		if ( m_pLBThumb->IsExclusiveSelfControl() )  // 좌측 미니맵 늘이기
		{
			const UIRECT& rcGlobalPos = GetGlobalPos ();
			D3DXVECTOR2 vORIGINPOS( rcGlobalPos.right, rcGlobalPos.top );
			D3DXVECTOR2 vDELTA;
			D3DXVECTOR2 vSTART;

			vDELTA.x = fabs(static_cast<float>(x) - rcGlobalPos.right);
			vSTART.x = rcGlobalPos.right - vDELTA.x;
			vSTART.y = rcGlobalPos.top;
			//			vDELTA.x = static_cast<float>(x) - rcGlobalPos.left;			
			vDELTA.y = static_cast<float>(y) - rcGlobalPos.top;

			const UIRECT& rcGlobalMapPos = m_pMap->GetGlobalPos ();

			//bool bNO_RESIZE = false;
			//if ( rcGlobalMapPos.sizeX >= m_nMAP_TEXSIZE_X && fabs(vDELTA.x) >= rcGlobalPos.sizeX )
			//{
			//	bNO_RESIZE = true;
			//}

			//         D3DXVECTOR2 vProtectSize = GetProtectSize();
			//if ( vProtectSize.x >= rcGlobalPos.sizeX )
			//{
			//	if ( fabs(vDELTA.x) <= rcGlobalPos.sizeX )
			//		bNO_RESIZE = true;
			//}

			//if ( !bNO_RESIZE )
			{
				float fGAP_SIZEX = (rcGlobalPos.sizeX - rcGlobalMapPos.sizeX);
				float fGAP_SIZEY = (rcGlobalPos.sizeY - rcGlobalMapPos.sizeY);

				long lResolution = CUIMan::GetResolution ();
				WORD X_RES = HIWORD(lResolution);
				WORD Y_RES = LOWORD(lResolution);			

				{
					float fMAP_DELTAX = (float)m_nMAP_TEXSIZE_X + fGAP_SIZEX;
					if ( fMAP_DELTAX <= vDELTA.x )
					{
						vDELTA.x = fMAP_DELTAX;
					}

					if ( vDELTA.x >= X_RES )
					{
						vDELTA.x = X_RES;
					}
				}

				{
					float fMAP_DELTAY = (float)m_nMAP_TEXSIZE_Y + fGAP_SIZEY;
					if ( fMAP_DELTAY <= vDELTA.y )
					{
						vDELTA.y = fMAP_DELTAY;
					}

					if ( vDELTA.y >= Y_RES )
					{
						vDELTA.y = Y_RES;
					}
				}
				//float fMAP_DELTAX = m_nMAP_TEXSIZE_X + fGAP_SIZEX;
				//float fMAP_DELTAY = m_nMAP_TEXSIZE_Y + fGAP_SIZEY;
				//if ( fMAP_DELTAX <= vDELTA.x )
				//{
				//	vDELTA.x = fMAP_DELTAX;
				//}
				//if ( fMAP_DELTAY <= vDELTA.y )
				//{
				//	vDELTA.y = fMAP_DELTAY;
				//}

				WORD wAlignFlag = GetAlignFlag ();			
				SetAlignFlag ( UI_FLAG_XSIZE | UI_FLAG_YSIZE );
				{
					UIRECT rcNewPos( vSTART.x, vSTART.y, vDELTA.x, vDELTA.y );
					AlignSubControl ( rcGlobalPos, rcNewPos );
					SetGlobalPos ( rcNewPos );
				}
				SetAlignFlag ( wAlignFlag );

				SetGlobalPos ( D3DXVECTOR2(vORIGINPOS.x-GetGlobalPos().sizeX,vORIGINPOS.y ) );

				{
					const UIRECT& rcGlobalPos = m_pMap->GetGlobalPos ();

					m_vRenderSize.x = rcGlobalPos.sizeX / m_nMAP_TEXSIZE_X;
					m_vRenderSize.y = rcGlobalPos.sizeY / m_nMAP_TEXSIZE_Y;

					m_nVIEW_SIZE_X = static_cast<int>(rcGlobalPos.sizeX);
					m_nVIEW_SIZE_Y = static_cast<int>(rcGlobalPos.sizeY);

					if ( m_nVIEW_START_X + m_nVIEW_SIZE_X > m_nMAP_TEXSIZE_X ) m_nVIEW_START_X = m_nMAP_TEXSIZE_X - m_nVIEW_SIZE_X;
					if ( m_nVIEW_START_X < 0 ) m_nVIEW_START_X = 0;

					if ( m_nVIEW_START_Y + m_nVIEW_SIZE_Y > m_nMAP_TEXSIZE_Y ) m_nVIEW_START_Y = m_nMAP_TEXSIZE_Y - m_nVIEW_SIZE_Y;
					if ( m_nVIEW_START_Y < 0 ) m_nVIEW_START_Y = 0;

					m_vRenderPos.x = static_cast<float>(m_nVIEW_START_X) / static_cast<float>(m_nMAP_TEXSIZE_X);
					m_vRenderPos.y = static_cast<float>(m_nVIEW_START_Y) / static_cast<float>(m_nMAP_TEXSIZE_Y);

					m_vRenderSize.x *= 10000;
					m_vRenderSize.x = floor(m_vRenderSize.x);
					m_vRenderSize.x *= 0.0001f;

					m_vRenderSize.y *= 10000;
					m_vRenderSize.y = floor(m_vRenderSize.y);
					m_vRenderSize.y *= 0.0001f;

					// 맵 텍스쳐 체인지
					UIRECT rcTexture( m_vRenderPos.x, m_vRenderPos.y, m_vRenderSize.x, m_vRenderSize.y );
					m_pMap->SetTexturePos ( rcTexture );

// 					m_vScrollGap.x = m_nMAP_TEXSIZE_X - rcGlobalPos.sizeX;
// 					m_vScrollGap.y = m_nMAP_TEXSIZE_Y - rcGlobalPos.sizeY;
					MakeScrollGap();

					m_pScrollBarH->GetThumbFrame()->SetState ( m_nMAP_TEXSIZE_X, m_nVIEW_SIZE_X );
					m_pScrollBarV->GetThumbFrame()->SetState ( m_nMAP_TEXSIZE_Y, m_nVIEW_SIZE_Y );
				}
			}
		}

		{

			//////////////////////////////////////////////////////////////////////////

			UpdateGate();
			UpdateNormalNpc();
			UpdateZoneName();
            UpdateGateName();
			UpdateBusStation();
			UpdateLookerRoom();
			UpdateStartPosition();
			UpdateClubManager();
// 			UpdateItemSearch();
// 			UpdateItemMix();	
			UpdatePostBox();
			UpdatePartyMember();
			//////////////////////////////////////////////////////////////////////////
			UpdateMonsterInfo();
			//////////////////////////////////////////////////////////////////////////
			UpdateSearchCharacter();
			//////////////////////////////////////////////////////////////////////////

			const D3DXVECTOR3 &vPlayerPos = m_pGaeaClient->GetCharacterPos ();

			//			m_pPlayerMark->SetVisibleSingle( FALSE );
			UPDATE_CHAR_POS( m_pPlayerMark, vPlayerPos, m_pGaeaClient->GetActiveMap()->GetBaseMapID());

			D3DXVECTOR3 vLookatPt = DxViewPort::GetInstance().GetLookatPt();
			D3DXVECTOR3 vFromPt = DxViewPort::GetInstance().GetFromPt();
			D3DXVECTOR3 vCameraDir = vLookatPt - vFromPt;
			vCameraDir.y = 0.f;
			D3DXVec3Normalize( &vCameraDir, &vCameraDir );
			UPDATE_CHAR_DIRECTION( m_pPlayerDirection, vPlayerPos, vCameraDir, m_pGaeaClient->GetActiveMap()->GetBaseMapID() );
			UPDATE_CHAR_ROTATE ( vCameraDir );

			// UpdatePartyMember에서 호출해줌
			//UPDATE_PARTY ();
			UPDATE_GUILD ();
			UPDATE_CTF ();
			
			UPDATE_MOUSE_POS ( x, y );
		}
	}

	{
		CBasicScrollThumbFrame* pThumbFrame = m_pScrollBarH->GetThumbFrame ();
		const float& fPercent = pThumbFrame->GetPercent ();
		pThumbFrame->SetPercent ( fPercent );
	}

	{
		CBasicScrollThumbFrame* pThumbFrame = m_pScrollBarV->GetThumbFrame ();
		const float& fPercent = pThumbFrame->GetPercent ();
		pThumbFrame->SetPercent ( fPercent );
	}

	if ( m_bAUTO_MOVE )
	{
		//	자기 위치 표시
		const D3DXVECTOR3 &vPlayerPos = m_pGaeaClient->GetCharacterPos ();

		int nMAP_X(0), nMAP_Y(0);
		CONVERT_WORLD2MAP ( vPlayerPos, nMAP_X, nMAP_Y );
		DoCHARPOS_OnCENTER_InVIEW ( nMAP_X, nMAP_Y );
	}

	// 미니맵 마우스 좌클릭 목표지점 갱신
	{
		UPDATE_TARGET_POS( m_iTargetX, m_iTargetY );
	}


	
}

void RnOldLargeMapWindow::TranslateUIMessage ( UIGUID cID, DWORD dwMsg )
{
	CUIGroupHelper::TranslateUIMessage ( cID, dwMsg );

	switch ( cID )
	{
// 	case ET_CONTROL_TITLE:
// 	case ET_CONTROL_TITLE_F:
// 		{
// 			if ( (dwMsg & UIMSG_LB_DUP) && CHECK_MOUSE_IN ( dwMsg ) )
// 			{
// 				m_pInterface->SetDefaultPosInterface( MAP_WINDOW );
// 			}
// 		}
// 		break;
	case LARGEMAP_IMAGE:	TranslateMeg( cID, dwMsg, m_pMap );				break;
	case LARGEMAP_LB_THUMB:	TranslateMegThumb( cID, dwMsg, m_pLBThumb );	break;
	case LARGEMAP_RB_THUMB:	TranslateMegThumb( cID, dwMsg, m_pRBThumb );	break;
	case LARGEMAP_CLOSE_BUTTON:
		{
			if ( (dwMsg & UIMSG_LB_UP) && CHECK_MOUSE_IN ( dwMsg ) )
			{
				m_pInterface->UiHideGroup( MAP_WINDOW );
				break;
			}

			break;
		}
	}
}

void RnOldLargeMapWindow::TranslateMeg( UIGUID cID, DWORD dwMsg, CUIControl * pUIControl )
{
	GASSERT( pUIControl );

	if ( CHECK_MOUSE_IN ( dwMsg ) )					//	컨트롤 내부에 마우스가 위치하고,
	{
		if ( CHECK_RB_DOWN_LIKE ( dwMsg ) || CHECK_MB_DOWN_LIKE ( dwMsg ) )
		{
			pUIControl->SetExclusiveControl();		//	단독 컨트롤로 등록하고,
			if ( !m_bFirstGap )						//	최초 포지션 갭을 기록한다.
			{
				UIRECT rcPos = pUIControl->GetGlobalPos ();
				m_vGap.x = m_PosX - rcPos.left;
				m_vGap.y = m_PosY - rcPos.top;

				if( cID == LARGEMAP_IMAGE )
				{
					m_vMOUSE_BACK.x = (float)m_PosX;
					m_vMOUSE_BACK.y = (float)m_PosY;
				}

				m_bFirstGap = TRUE;
			}
		}
		else if ( CHECK_RB_UP_LIKE ( dwMsg ) || CHECK_MB_UP_LIKE ( dwMsg ) )
		{
			pUIControl->ResetExclusiveControl();	//	단독 컨트롤을 해제하고
			m_bFirstGap = FALSE;					//	최초 포지션갭을 해제한다.
		}

		if( CHECK_LB_UP_LIKE( dwMsg ) )				// 미니맵 좌클릭해서 캐릭터 이동
			OnLButtonUp();
	}
	else if ( CHECK_RB_UP_LIKE ( dwMsg ) || CHECK_MB_UP_LIKE ( dwMsg ) )
	{												//	컨트롤 영역밖에서 버튼을 떼는 경우 발생하더라도
		pUIControl->ResetExclusiveControl();		//	단독 컨트롤을 해제하고,
		m_bFirstGap = FALSE;						//	최초 포지션갭을 해제한다.
	}

	if ( ( m_pGaeaClient->GetTutorial()->IsMouseContNMoveStep()
		&& m_pGaeaClient->GetTutorial()->IsSubFifteenStep() ) )
	{
		CONVERT_WORLD2MAP( m_pGaeaClient->GetTutorial()->m_vMinimapTargetPos
			, m_pGaeaClient->GetTutorial()->m_iMinimapTargetX, m_pGaeaClient->GetTutorial()->m_iMinimapTargetY );

		int nPOINT_X = m_pGaeaClient->GetTutorial()->m_iMinimapTargetX - m_nVIEW_START_X;
		int nPOINT_Y = m_pGaeaClient->GetTutorial()->m_iMinimapTargetY - m_nVIEW_START_Y;

		const UIRECT& rcGlobalPos = m_pMap->GetGlobalPos();
		const UIRECT& rcMarkPos = m_pInterface->MinimapTargetGetGlobalPos();

		D3DXVECTOR2 vNewMarkPos( nPOINT_X + rcGlobalPos.left - (rcMarkPos.sizeX/2.f), nPOINT_Y + rcGlobalPos.top - (rcMarkPos.sizeY/2.f) );
		m_pInterface->MinimapTargetSetLocalPos(vNewMarkPos);
		m_pInterface->MinimapTargetSetGlobalPos(vNewMarkPos);

		// 화면을 벗어난 위치에 있다
		if( nPOINT_X < 0 || nPOINT_Y < 0 )
		{
			m_pInterface->MinimapTargetSetEnable(false);
			return;
		}

		// 화면을 벗어난 위치에 있다
		else if( m_nVIEW_SIZE_X < nPOINT_X || m_nVIEW_SIZE_Y < nPOINT_Y )
		{
			m_pInterface->MinimapTargetSetEnable(false);
			return;
		}
		else
		{
			m_pInterface->MinimapTargetSetEnable(true);
		}
	}
	else if ( ( m_pGaeaClient->GetTutorial()->IsQuestNHuntStep()
		&& m_pGaeaClient->GetTutorial()->IsSubThreeStep() ) )
	{
		CONVERT_WORLD2MAP( m_pGaeaClient->GetTutorial()->m_vMinimapTargetPos
			, m_pGaeaClient->GetTutorial()->m_iMinimapTargetX, m_pGaeaClient->GetTutorial()->m_iMinimapTargetY );

		int nPOINT_X = m_pGaeaClient->GetTutorial()->m_iMinimapTargetX - m_nVIEW_START_X;
		int nPOINT_Y = m_pGaeaClient->GetTutorial()->m_iMinimapTargetY - m_nVIEW_START_Y;

		const UIRECT& rcGlobalPos = m_pMap->GetGlobalPos();
		const UIRECT& rcMarkPos = m_pInterface->MinimapTargetGetGlobalPos();

		D3DXVECTOR2 vNewMarkPos( nPOINT_X + rcGlobalPos.left - (rcMarkPos.sizeX/2.f), nPOINT_Y + rcGlobalPos.top - (rcMarkPos.sizeY/2.f) );
		m_pInterface->MinimapTargetSetLocalPos(vNewMarkPos);
		m_pInterface->MinimapTargetSetGlobalPos(vNewMarkPos);

		// 화면을 벗어난 위치에 있다
		if( nPOINT_X < 0 || nPOINT_Y < 0 )
		{
			m_pInterface->MinimapTargetSetEnable(false);
			return;
		}

		// 화면을 벗어난 위치에 있다
		else if( m_nVIEW_SIZE_X < nPOINT_X || m_nVIEW_SIZE_Y < nPOINT_Y )
		{
			m_pInterface->MinimapTargetSetEnable(false);
			return;
		}
		else
		{
			m_pInterface->MinimapTargetSetEnable(true);
		}
	}
}

void RnOldLargeMapWindow::TranslateMegThumb( UIGUID cID, DWORD dwMsg, CUIControl * pUIControl )
{
	GASSERT( pUIControl );

	if ( CHECK_MOUSE_IN ( dwMsg ) )					//	컨트롤 내부에 마우스가 위치하고,
	{
		if ( CHECK_LB_DOWN_LIKE ( dwMsg ) )			//	마우스를 좌클릭으로 눌렀을 때,
		{
			if( CHECK_MOUSE_IN( m_pMap->GetMessageEx() ) )
				return;

			pUIControl->SetExclusiveControl();		//	단독 컨트롤로 등록하고,
			if ( !m_bFirstGap )						//	최초 포지션 갭을 기록한다.
			{
				UIRECT rcPos = pUIControl->GetGlobalPos ();
				m_vGap.x = m_PosX - rcPos.left;
				m_vGap.y = m_PosY - rcPos.top;

				m_bFirstGap = TRUE;
			}
		}
		else if ( CHECK_LB_UP_LIKE ( dwMsg ) )		//	좌클릭했던것을 놓으면
		{
			pUIControl->ResetExclusiveControl();	//	단독 컨트롤을 해제하고
			m_bFirstGap = FALSE;					//	최초 포지션갭을 해제한다.
		}
	}
	else if ( CHECK_LB_UP_LIKE ( dwMsg ) )			//	컨트롤 영역밖에서 버튼을 떼는 경우가
	{												//	발생하더라도
		pUIControl->ResetExclusiveControl();		//	단독 컨트롤을 해제하고,
		m_bFirstGap = FALSE;						//	최초 포지션갭을 해제한다.
	}
}

HRESULT	RnOldLargeMapWindow::Render ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT( pd3dDevice );
// 
// 	UIRECT rect = m_pPlayerMark->GetGlobalPos();
// 	CString strDebugRect;
// 	strDebugRect.Format("%f,%f\n",rect.left,rect.top);
// 	OutputDebugString(strDebugRect);




	return CUIGroupHelper::Render ( pd3dDevice );

// 	if( !IsVisible() ) return S_OK;
// 
// 	HRESULT hr = S_OK;
// 
// 	BOOL bPlayerMarkVisible = m_pPlayerMark->IsVisible ();
// 	m_pPlayerMark->SetVisibleSingle ( FALSE );
// 	{
// 		hr = CUIGroupHelper::Render ( pd3dDevice );
// 		if ( FAILED ( hr ) ) return hr;
// 	}
// 	m_pPlayerMark->SetVisibleSingle ( bPlayerMarkVisible );	
// 
// 	//	NOTE
// 	//		RENDER STATES BEGIN
// 	DWORD dwMinFilter(0), dwMagFilter(0), dwMipFilter(0);
// 	pd3dDevice->GetSamplerState( 0, D3DSAMP_MINFILTER, &dwMinFilter );
// 	pd3dDevice->GetSamplerState( 0, D3DSAMP_MAGFILTER, &dwMagFilter );
// 	pd3dDevice->GetSamplerState( 0, D3DSAMP_MIPFILTER, &dwMipFilter );
// 
// 	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
// 	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
// 	pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
// 
// 	hr = m_pPlayerMark->Render ( pd3dDevice );
// 
// 	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, dwMinFilter );
// 	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, dwMagFilter );
// 	pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, dwMipFilter );
// 
// 	return hr;
}

void RnOldLargeMapWindow::SetVisibleSingle ( BOOL bVisible )
{
	CUIGroupHelper::SetVisibleSingle ( bVisible );

    bool bVisibleTemp = (0!=bVisible);
	UPDATE_GUILD_INFO( bVisibleTemp );

	if ( bVisible )
	{
		//	자기 위치 표시
		const D3DXVECTOR3 &vPlayerPos = m_pGaeaClient->GetCharacterPos ();

		/*int nMAP_X(0), nMAP_Y(0);
		CONVERT_WORLD2MAP ( vPlayerPos, nMAP_X, nMAP_Y );
		DoCHARPOS_OnCENTER_InVIEW ( nMAP_X, nMAP_Y );*/

		UPDATE_CHAR_POS ( m_pPlayerMark, vPlayerPos, m_pGaeaClient->GetActiveMap()->GetBaseMapID());

		D3DXVECTOR3 vLookatPt = DxViewPort::GetInstance().GetLookatPt();
		D3DXVECTOR3 vFromPt = DxViewPort::GetInstance().GetFromPt();
		D3DXVECTOR3 vCameraDir = vLookatPt - vFromPt;
		vCameraDir.y = 0.f;
		D3DXVec3Normalize( &vCameraDir, &vCameraDir );
		UPDATE_CHAR_DIRECTION( m_pPlayerDirection, vPlayerPos, vCameraDir, m_pGaeaClient->GetActiveMap()->GetBaseMapID() );
		UPDATE_CHAR_ROTATE ( m_pInterface->GetCharDir () );

		m_pPlayerMark->SetVisibleSingle ( TRUE );
		m_pPlayerDirection->SetVisibleSingle( TRUE );

		//	NOTE
		//		맵 자동 이동
		SetAutoMove ( true );		
	}
}

void RnOldLargeMapWindow::OnLButtonUp()
{
	if( m_pInterface->IsOpenWindowToMoveBlock() )
		return;

	const DWORD dwKeyStateControl(GLInputManager::GetInstance().GetKeyState(EMSHORTCUTS_DOMINATE_CONTROL));

	if ( (m_pGaeaClient->GetCharacter()->UserLevel() >= USER_USER_GM) && (dwKeyStateControl & DXKEY_DOWNED) )
	{ // GM이고, Ctrl Key를 누른 상태로 이동시 m2p 명령으로 대체 한다;
		const SNATIVEID mapID(m_pLevelFile->GetBaseMapID());
		GLMSG::SNET_GM_MOVE2MAPPOS NetMsg(mapID, m_visiblePosX, m_visiblePosY);
		m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
		return;
	}

	if( !IsMyCharacterMap() )
		return;

	if( m_isEnableLButton == false )
		return;	

	INT iMapX( 0 ), iMapY( 0 );
	CONVERT_MOUSE2MAP( m_PosX, m_PosY, iMapX, iMapY );

	D3DXVECTOR3 vWorldPos( 0.0f, 0.0f, 0.0f );
	CONVERT_MAP2WORLD( iMapX, iMapY, vWorldPos );

	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	if( !pCharacter )
		return;

	if( m_pSettingInterface->IsAutoRideVehicle() )
	{
		if(!pCharacter->VehicleState())
			pCharacter->ReqSetVehicle(true);
	}

	if( !pCharacter->LargeMapMoveTo( &vWorldPos, FALSE ) )
		return;

	m_iTargetX = iMapX;
	m_iTargetY = iMapY;

	UPDATE_TARGET_POS( m_iTargetX, m_iTargetY );
}

void RnOldLargeMapWindow::UPDATE_CHAR_POS ( CUIControl * pControl, const D3DXVECTOR3& vPlayerPos, const SNATIVEID& id)
{
	GASSERT( pControl );

//	if( m_pPlayerMark == pControl || std::find(m_PartyMarkVec.begin(),m_PartyMarkVec.end(),pControl) != m_PartyMarkVec.end() )
	{
		if( m_pLevelFile->GetBaseMapID() != id)
		{
			pControl->SetVisibleSingle(FALSE);
			return;
		}

//		if( !IsMyCharacterMap() )
// 		{
// 		}
	}
	//CUIControl* pMark = FindControl ( cID ); // MEMO : FindControl은 NULL을 넘긴다.
	//if ( !pMark ) return ;

	int nMapPosX(0), nMapPosY(0);
	CONVERT_WORLD2MAP ( vPlayerPos, nMapPosX, nMapPosY );

	int nPOINT_X = nMapPosX - m_nVIEW_START_X;
	int nPOINT_Y = nMapPosY - m_nVIEW_START_Y;

	//pMark->SetVisibleSingle ( FALSE );

    if ( nPOINT_X < 0 || nPOINT_Y < 0 || m_nVIEW_SIZE_X < nPOINT_X || m_nVIEW_SIZE_Y < nPOINT_Y )
	{
		pControl->SetVisibleSingle(FALSE);
		return;
	}

	const UIRECT& rcGlobalPos = m_pMap->GetGlobalPos ();
	const UIRECT& rcMarkPos = pControl->GetGlobalPos ();
    float fCenterX = rcGlobalPos.left - (rcMarkPos.sizeX/2.f);
    float fCenterY = rcGlobalPos.top - (rcMarkPos.sizeY/2.f);

	D3DXVECTOR2 vNewMarkPos ( nPOINT_X + fCenterX, nPOINT_Y + fCenterY);
	pControl->SetGlobalPos ( vNewMarkPos );

       
    if( rcMarkPos.left     < rcGlobalPos.left  ||
        rcMarkPos.top      < rcGlobalPos.top   ||
        rcGlobalPos.right  < rcMarkPos.right   ||
        rcGlobalPos.bottom < rcMarkPos.bottom  )
    {
        pControl->SetVisibleSingle ( FALSE );
    }
    else
    {
        pControl->SetVisibleSingle ( TRUE );
    }
	
}

void RnOldLargeMapWindow::UPDATE_CHAR_DIRECTION ( CUIControl* pControl, const D3DXVECTOR3& vPlayerPos, const D3DXVECTOR3& vDirection, const SNATIVEID& id )
{
	GASSERT( pControl );
	
	if( m_pLevelFile->GetBaseMapID() != id)
	{
		pControl->SetVisibleSingle(FALSE);
		return;
	}
	
	int nMapPosX = 0;
	int nMapPosY = 0;
	CONVERT_WORLD2MAP ( vPlayerPos, nMapPosX, nMapPosY );

	int nPOINT_X = nMapPosX - m_nVIEW_START_X;
	int nPOINT_Y = nMapPosY - m_nVIEW_START_Y;

	if ( nPOINT_X < 0 || nPOINT_Y < 0 || m_nVIEW_SIZE_X < nPOINT_X || m_nVIEW_SIZE_Y < nPOINT_Y )
	{
		pControl->SetVisibleSingle( FALSE );
		return;
	}

	float fZ = 1.f;
	float fPOINT_Y = nPOINT_Y - fZ*8;
	fPOINT_Y += 0.5f;

	const UIRECT& rcGlobalPos = m_pMap->GetGlobalPos();
	const UIRECT& rcMarkPos = pControl->GetGlobalPos();
	D3DXVECTOR2 vNewMarkPos ( nPOINT_X + rcGlobalPos.left - (rcMarkPos.sizeX/2.f), fPOINT_Y + rcGlobalPos.top - (rcMarkPos.sizeY/2.f) );
	pControl->SetGlobalPos ( vNewMarkPos );
	pControl->SetVisibleSingle ( TRUE );
}

void RnOldLargeMapWindow::UPDATE_CHAR_ROTATE ( const D3DXVECTOR3& vCharDir )
{
	//	Note : 현제의 회전값.
	//
	float LengthXZ;
	float thetaY;

	LengthXZ = (float) sqrt ( vCharDir.x*vCharDir.x + vCharDir.z*vCharDir.z );			
	
	//	Note : 수식의 특성상 LengthXZ가... 0가되면... 문제가 발생한다.
	//	
	if ( LengthXZ == 0 )	LengthXZ = 0.001f;

	thetaY = (float) acos ( vCharDir.x / LengthXZ );
	if ( vCharDir.z>0 )	thetaY = -thetaY;

	thetaY += D3DX_PI / 2.f;

	{
		UIRECT	rcGlobalPosDir = m_pPlayerDirection->GetGlobalPos();
		UIRECT	rcGlobalPos = m_pPlayerMark->GetGlobalPos();
		D3DXVECTOR2	vCenterPos( floor(rcGlobalPos.left + (rcGlobalPos.sizeX)/2.f), floor(rcGlobalPos.top + (rcGlobalPos.sizeY)/2.f) );
		
		D3DXVECTOR2 vGlobalPosDir[4];
		vGlobalPosDir[0].x = rcGlobalPosDir.left - vCenterPos.x;
		vGlobalPosDir[0].y = rcGlobalPosDir.top - vCenterPos.y;
		vGlobalPosDir[1].x = rcGlobalPosDir.right - vCenterPos.x;
		vGlobalPosDir[1].y = rcGlobalPosDir.top - vCenterPos.y;
		vGlobalPosDir[2].x = rcGlobalPosDir.right - vCenterPos.x;
		vGlobalPosDir[2].y = rcGlobalPosDir.bottom - vCenterPos.y;
		vGlobalPosDir[3].x = rcGlobalPosDir.left - vCenterPos.x;
		vGlobalPosDir[3].y = rcGlobalPosDir.bottom - vCenterPos.y;
		
		for ( int i = 0; i < 4; ++i )
		{
			D3DXVECTOR2 vOutPos;
			vOutPos.x = (float) (cos(-thetaY) * vGlobalPosDir[i].x + sin(-thetaY)* vGlobalPosDir[i].y) + vCenterPos.x;
			vOutPos.y = (float) (-sin(-thetaY) * vGlobalPosDir[i].x + cos(-thetaY)* vGlobalPosDir[i].y)  + vCenterPos.y;
			m_pPlayerDirection->SetGlobalPos(i , vOutPos);		
		}		
	}
}

void RnOldLargeMapWindow::UPDATE_TARGET_POS( int iX, int iY )
{
	if( !m_pTargetMark )
		return;

	// 일단 숨긴다
	m_pTargetMark->SetVisibleSingle( FALSE );

	// 현재 맵이 아니면 표시 안함
	if( !IsMyCharacterMap() )
	{
		return;
	}

	// 목표 지점이 초기화 되었다
	if( m_iTargetX == INT_MIN || m_iTargetY == INT_MIN )
		return;

	int nPOINT_X = iX - m_nVIEW_START_X;
	int nPOINT_Y = iY - m_nVIEW_START_Y;

	// 화면을 벗어난 위치에 있다
    if( nPOINT_X < 0 || nPOINT_Y < 0 )
		return;

	// 화면을 벗어난 위치에 있다
	if( m_nVIEW_SIZE_X < nPOINT_X || m_nVIEW_SIZE_Y < nPOINT_Y )
		return;

	// 목표 지점 도달
	{
		const D3DXVECTOR3& vPlayerPos = m_pGaeaClient->GetCharacterPos();

		int nMAP_X( 0 ), nMAP_Y( 0 );
		CONVERT_WORLD2MAP( vPlayerPos, nMAP_X, nMAP_Y );

		// 18 18 ㅋㅋ
		if( abs( nMAP_X - m_iTargetX ) < 18 && abs( nMAP_Y - m_iTargetY ) < 18 )
		{
            VISIBLE_TARGET_POS(FALSE);
			return;
		}
	}

	// 위치 갱신
	{
		const UIRECT& rcGlobalPos = m_pMap->GetGlobalPos();
		const UIRECT& rcMarkPos = m_pTargetMark->GetGlobalPos();
		D3DXVECTOR2 vNewMarkPos( nPOINT_X + rcGlobalPos.left - (rcMarkPos.sizeX/2.f), nPOINT_Y + rcGlobalPos.top - (rcMarkPos.sizeY/2.f) );
		m_pTargetMark->SetGlobalPos( vNewMarkPos );
	}

	// 화면에 보여준다
	m_pTargetMark->SetVisibleSingle( TRUE );
}

void RnOldLargeMapWindow::VISIBLE_TARGET_POS( BOOL bVisible )
{
	if( !m_pTargetMark )
		return;

	if( !bVisible )
	{
		m_iTargetX = INT_MIN;
		m_iTargetY = INT_MIN;
	}

	m_pTargetMark->SetVisibleSingle( bVisible );
}

void RnOldLargeMapWindow::DoCHARPOS_OnCENTER_InVIEW ( int nMapPosX, int nMapPosY )
{
	//	NOTE
	//		뷰의 이동
	m_nVIEW_START_X = nMapPosX - static_cast<int>((static_cast<float>(m_nVIEW_SIZE_X)/2.0f));
	m_nVIEW_START_Y = nMapPosY - static_cast<int>((static_cast<float>(m_nVIEW_SIZE_Y)/2.0f));

	//	뷰 영역 체크
	if ( m_nVIEW_START_X < 0 ) m_nVIEW_START_X = 0;
	if ( m_nVIEW_START_Y < 0 ) m_nVIEW_START_Y = 0;
	
	if ( m_nMAP_TEXSIZE_X - m_nVIEW_SIZE_X < m_nVIEW_START_X ) m_nVIEW_START_X = m_nMAP_TEXSIZE_X - m_nVIEW_SIZE_X;
	if ( m_nMAP_TEXSIZE_Y - m_nVIEW_SIZE_Y < m_nVIEW_START_Y ) m_nVIEW_START_Y = m_nMAP_TEXSIZE_Y - m_nVIEW_SIZE_Y;
	
	if ( m_nVIEW_START_X < 0 ) m_nVIEW_START_X = 0;
	if ( m_nVIEW_START_Y < 0 ) m_nVIEW_START_Y = 0;

	{
		CBasicScrollThumbFrame* pThumbFrame = m_pScrollBarH->GetThumbFrame ();
		const float& fPercent = pThumbFrame->GetPercent ();

		if ( m_vScrollGap.x == 0.0f )	pThumbFrame->SetPercent ( 0.0f );
//		else							pThumbFrame->SetPercent ( m_nVIEW_START_X/m_vScrollGap.x );
		else							pThumbFrame->SetPercent ( m_nVIEW_START_X/GetScrollGap().x );


	}

	{
		CBasicScrollThumbFrame* pThumbFrame = m_pScrollBarV->GetThumbFrame ();
		const float& fPercent = pThumbFrame->GetPercent ();

		if ( m_vScrollGap.y == 0.0f )	pThumbFrame->SetPercent ( 0.0f );
//		else							pThumbFrame->SetPercent ( m_nVIEW_START_Y/m_vScrollGap.y );
		else							pThumbFrame->SetPercent ( m_nVIEW_START_Y/GetScrollGap().y );

	}
}

void RnOldLargeMapWindow::CONVERT_WORLD2MAP ( const D3DXVECTOR3& vPos, int& nPosX, int& nPosY )
{
	//	NOTE
	//		월드 좌표의 기준을 재설정, Y값이 계산되는 것은 캐릭터가 진행하는 방향과
	//		맵이 진행하는 방향이 반대로 되어 있기 때문임.
	int nCur_X = static_cast<int>(vPos.x) - m_nWORLD_START_X;
	int nCur_Y = (m_nWORLD_SIZE_Y-static_cast<int>(vPos.z)) + m_nWORLD_START_Y;

	//	NOTE
	//		재정의된 월드 좌표를 맵 포지션으로 바꿈
	nPosX = static_cast<int>(m_nMAP_TEXSIZE_X * nCur_X / m_nWORLD_SIZE_X);
	nPosY = static_cast<int>(m_nMAP_TEXSIZE_Y * nCur_Y / m_nWORLD_SIZE_Y);
}

void RnOldLargeMapWindow::CONVERT_MOUSE2MAP ( const int nMousePosX, const int nMousePosY, int& nPosX, int& nPosY )
{
	const UIRECT& rcMapGlobalPos = m_pMap->GetGlobalPos ();
	const int nMouseX_RELATIVE = nMousePosX - static_cast<int>(rcMapGlobalPos.left);
	const int nMouseY_RELATIVE = nMousePosY - static_cast<int>(rcMapGlobalPos.top);	

	nPosX = nMouseX_RELATIVE + m_nVIEW_START_X;
	nPosY = nMouseY_RELATIVE + m_nVIEW_START_Y;
}

void RnOldLargeMapWindow::CONVERT_MAP2WORLD ( const int nPosX, const int nPosY, D3DXVECTOR3& vPos )
{
	float fPosX = m_vWORLD_SIZE.x / m_vMAP_TEXSIZE.x * (float)nPosX;
	float fPosY = m_vWORLD_SIZE.y / m_vMAP_TEXSIZE.y * (float)nPosY;

	vPos.x = fPosX + m_vWORLD_START.x;
	vPos.z = m_vWORLD_SIZE.y - ( fPosY - m_vWORLD_START.y );
}

void RnOldLargeMapWindow::UPDATE_MOUSE_POS ( const int nMouseX, const int nMouseY )
{
	if ( !CHECK_MOUSE_IN ( m_pMap->GetMessageEx () ) )
	{
		m_pMousePosText->SetVisibleSingle ( FALSE );
		m_pMousePosBack->SetVisibleSingle ( FALSE );
		return ;
	}

	const UIRECT& rcMapGlobalPos = m_pMap->GetGlobalPos ();
	const int nMouseX_RELATIVE = nMouseX - static_cast<int>(rcMapGlobalPos.left);
	const int nMouseY_RELATIVE = nMouseY - static_cast<int>(rcMapGlobalPos.top);	

	const int nX = nMouseX_RELATIVE + m_nVIEW_START_X;
	const int nY = m_nMAP_TEXSIZE_Y - (nMouseY_RELATIVE + m_nVIEW_START_Y);

	m_visiblePosX = (int)(((m_nWORLD_SIZE_X * nX) / m_nMAP_TEXSIZE_X) / 50.f);
	m_visiblePosY = (int)(((m_nWORLD_SIZE_Y * nY) / m_nMAP_TEXSIZE_Y) / 50.f);	
    
	CString strMousePos;
	strMousePos.Format ( "%03d %03d", m_visiblePosX, m_visiblePosY );
	m_pMousePosText->SetOneLineText ( strMousePos, NS_UITEXTCOLOR::WHITE );
	m_pMousePosText->SetVisibleSingle ( TRUE );
	m_pMousePosBack->SetVisibleSingle ( TRUE );
}




//////////////////////////////////////////////////////////////////////////


void RnOldLargeMapWindow::UPDATE_PARTY ()
{
	UPDATE_PARTY_DEL();

	// CTF중일떄는 보여주지 않는다.
	if ( IS_CTF_UPDATE() )
	{
		return;
	}

	GLPartySlotClient* pSelf = FindSelfClient ();

	if( pSelf )
	{
		UPDATE_PARTY_INS( pSelf ); // 추가
	}
	//else // Note : 파티가 아니라면 맵 상에 존재하는 모든 컨트롤을 지운다.
	//{
	//	PARTY_ONMAP_MAP_ITER iter;

	//	for( iter = m_mapPARTY.begin(); iter != m_mapPARTY.end(); )
	//	{
	//		PARTY_ONMAP_MAP_ITER cur = iter++;

	//		CUIControl * pMark = (*cur).second;
	//		if( pMark ) DeleteControl( pMark->GetWndID(), 0 );

	//		m_mapPARTY.erase( cur );
	//	}
	//}
}

void RnOldLargeMapWindow::UPDATE_PARTY_DEL()
{
	//PARTY_ONMAP_MAP_ITER iter;

	//for( iter = m_mapPARTY.begin(); iter != m_mapPARTY.end(); )
	//{
	//	PARTY_ONMAP_MAP_ITER cur = iter++;

	//	const DWORD& dwGaeaID = (*cur).first;
	//	GLPARTY_CLIENT *pMember = m_pGaeaClient->GetPartyClient()->FindMember ( dwGaeaID );
	//	if ( pMember && IsSameMap ( pSelf, pMember ) ) continue;		

	//	CUIControl* pMark = (*cur).second;
	//	if( !pMark ) return ;

	//	DeleteControl( pMark->GetWndID(), 0 );
	//	m_mapPARTY.erase( cur );
	//}

	for( size_t i=0; i < m_PartyMarkVec.size(); ++i )
	{
		m_PartyMarkVec[i]->SetVisibleSingle( FALSE );
	}
}

void RnOldLargeMapWindow::UPDATE_PARTY_INS( GLPartySlotClient* const pSelf)
{
	GLPartyClient* const pMyParty = m_pGaeaClient->GetMyPartyClient();
	if ( pMyParty->isValid() == false )
		return;

	GLPartySlotClient* const pMaster = pMyParty->GetMember(MASTER_PARTY);	//	마스터	
	if ( pSelf != pMaster )
	{
		m_PartyMarkVec[0]->m_Name = sc::string::format("Lv.%1% %2%",pMaster->m_Level,pMaster->m_szName);

		if( m_PartyMarkVec[0]->m_CommentVec.empty() )
			m_PartyMarkVec[0]->m_CommentVec.resize(1);

		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			m_PartyMarkVec[0]->m_CommentVec[0] = Mark::Comment( CGameTextMan::GetInstance().GetCommentText("CHARCLASS", CharClassToIndex(pMaster->m_emClass)).GetString(), D3DCOLOR_XRGB(255,255,0) );
		else
			m_PartyMarkVec[0]->m_CommentVec[0] = Mark::Comment( COMMENT::CHARCLASS[CharClassToIndex(pMaster->m_emClass)] , D3DCOLOR_XRGB(255,255,0) );

		UPDATE_CHAR_POS ( m_PartyMarkVec[0], pMaster->m_vPosition, pMaster->m_sMapID.getBaseMapID() );
	}

	DWORD nMEMBER_NUM = pMyParty->GetNMember();
	if( nMEMBER_NUM > 1 )
	{
		// Master Index -> 0, 1~7 member; 마스터는 위에서 그려줌;
		for ( DWORD nIndex = 1; nIndex < nMEMBER_NUM; ++nIndex )
		{
			GLPartySlotClient* const pMember = pMyParty->GetMember( nIndex );

			if ( !pMember )				continue;
			if ( pSelf == pMember )		continue;

			m_PartyMarkVec[nIndex]->m_Name = sc::string::format("Lv.%1% %2%",pMember->m_Level,pMember->m_szName);
			if ( m_PartyMarkVec[nIndex]->m_CommentVec.empty() )
				m_PartyMarkVec[nIndex]->m_CommentVec.resize(1);

			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
				m_PartyMarkVec[nIndex]->m_CommentVec[0] = Mark::Comment( CGameTextMan::GetInstance().GetCommentText("CHARCLASS", CharClassToIndex(pMember->m_emClass)).GetString(), D3DCOLOR_XRGB(255,255,0) );
			else
				m_PartyMarkVec[nIndex]->m_CommentVec[0] = Mark::Comment( COMMENT::CHARCLASS[CharClassToIndex(pMember->m_emClass)] , D3DCOLOR_XRGB(255,255,0) );

			UPDATE_CHAR_POS( m_PartyMarkVec[nIndex], pMember->m_vPosition,pMember->m_sMapID.getBaseMapID() );
		}
	}
}

BOOL RnOldLargeMapWindow::IsSameMap ( GLPartySlotClient *pSelf, GLPartySlotClient *pMember )
{
	if ( !pSelf || !pMember ) return FALSE;

	return pSelf->m_sMapID == pMember->m_sMapID;
}

GLPartySlotClient* const RnOldLargeMapWindow::FindSelfClient(void)
{
	GLPartyClient* const pMyParty = m_pGaeaClient->GetMyPartyClient();
	if ( pMyParty->isValid() == false )
		return NULL;

	//	마스터가 아니면, 자식들을 찾아본다.
	GLPartySlotClient* const pMaster = pMyParty->GetMember(MASTER_PARTY);	//	마스터
	if ( pMaster->isOneSelf(m_pGaeaClient) )
		return pMaster;
	else
	{ // Note : 자기찾기
		DWORD nMEMBER_NUM = pMyParty->GetNMember();
		if( nMEMBER_NUM > 1 )
		{
			nMEMBER_NUM -= 1; // Note : 마스터는 제외
			for ( DWORD _i = 0; _i < nMEMBER_NUM; ++_i )
			{
				// +1 은 의도된 것, 마스터는 0, 파티원은 1~7
				GLPartySlotClient* const pMember = pMyParty->GetMember( _i+1 );

				if (pMember && pMember->isOneSelf(m_pGaeaClient))
				{
					return pMember;				
				}
			}
		}
	}

	return NULL;
}

void RnOldLargeMapWindow::UPDATE_GUILD()
{
	RESET_GUILD();

	if ( !IS_GUILD_UPDATE() ) return;

	//	클럽 확인
	GLClubClient* pClub = m_pGaeaClient->GetMyClub();

	// 클럽 배틀존에서만 보임
	GLLandManClient* pLand = m_pGaeaClient->GetActiveMap();
	if (!pLand)
		return;

	DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
	int nMyChannel = pGlobalStage->GetChannel();
	GLCharacter* pChar = m_pGaeaClient->GetCharacter();
	if (!pChar)
		return;

	DWORD dwCharID = pChar->GetCharID();

	CLUBMEMBERS& mapMembers = pClub->GetMembers();
	CLUBMEMBERS_ITER iter_mem = mapMembers.begin ();
	CLUBMEMBERS_ITER iter_mem_end = mapMembers.end ();

	GUILD_POS_VECTOR_ITER iter_guild = m_vecGuild.begin();
	GUILD_POS_VECTOR_ITER iter_guild_end = m_vecGuild.end();


	// 길드 순회
	for ( ; iter_mem != iter_mem_end; ++iter_mem )
	{
		GLCLUBMEMBER& sClubMember = (*iter_mem).second;
		const bool bOnline = sClubMember.m_Online;
		const int nChannel = sClubMember.m_Channel;
		const DWORD dwID = sClubMember.m_ChaDbNum;

		// 접속중
		if (!bOnline || nMyChannel != nChannel)
			continue;		

		// 자신인가
		if (dwID == dwCharID)
			continue;		

		// 같은 파티인가
		if (m_pGaeaClient->GetMyPartyClient()->isMember( sClubMember.m_ChaName.c_str() ) )
			continue;

		// 같은 맵이면
		if ( pLand->GetGaeaMapID() != sClubMember.m_CurrentMap.getGaeaMapID() )
			continue;; 

		D3DXVECTOR3 vPos = sClubMember.m_CurrentPos;

		if ( iter_guild != iter_guild_end )
		{
			CUIControl* pControl = (CUIControl*) (*iter_guild);
			UPDATE_CHAR_POS( pControl, vPos,sClubMember.m_CurrentMap.getBaseMapID());

			iter_guild++;
		}
		else
		{
			// 만들어서 넣음
			CUIControl* pControl = new CUIControl(m_pEngineDevice);
			pControl->CreateSub( this, "MINIMAP_GUILD_MARK" );
			pControl->InitDeviceObjects ( m_pd3dDevice );
			pControl->RestoreDeviceObjects ( m_pd3dDevice );
			pControl->SetVisibleSingle( FALSE );
			RegisterControl( pControl );


			m_vecGuild.push_back ( pControl );
			UPDATE_CHAR_POS( pControl, vPos ,sClubMember.m_CurrentMap.getBaseMapID());

			iter_guild = m_vecGuild.end();
			iter_guild_end = m_vecGuild.end();
		}
	}
}

void RnOldLargeMapWindow::RESET_GUILD()
{
	GUILD_POS_VECTOR_ITER iter_guild = m_vecGuild.begin();
	GUILD_POS_VECTOR_ITER iter_guild_end = m_vecGuild.end();

	for ( ; iter_guild != iter_guild_end; ++iter_guild )
	{
		CUIControl* pControl = (CUIControl*)(*iter_guild);
		pControl->SetVisibleSingle( FALSE );
	}	
}
bool RnOldLargeMapWindow::IS_GUILD_UPDATE()
{
	//	클럽 확인
	GLClubClient* pMyClub = m_pGaeaClient->GetMyClub();
	if (!pMyClub->IsValid())
        return false;

	if ( pMyClub->GetBattleNum() <= 0 )
        return false;

	// 클럽 배틀존에서만 보임
	GLLandManClient* pLand = m_pGaeaClient->GetActiveMap();
	if ( pLand && !pLand->IsClubBattleZone() )
        return false;

	return true;
}

void RnOldLargeMapWindow::UPDATE_GUILD_INFO( bool bUpdate )
{
	if ( IS_GUILD_UPDATE() ) 
	{
		//m_pGaeaClient->ReqClubInfoUpdate( bUpdate );
	}
}

void RnOldLargeMapWindow::UPDATE_CTF()
{
	RESET_CTF();

	if ( !IS_CTF_UPDATE() )
	{
		return;
	}

	using namespace PVP;
	GLCaptureTheFieldClient* pCTFClient = m_pGaeaClient->GetCaptureTheField();

	if( !pCTFClient->GetCurrentGlobalState().IsSAMEMAP(m_pLevelFile->GetBaseMapID()) )
		return;

	// 동맹 정보 순회
	{
		GLCaptureTheFieldClient::VEC_ALLIANCEINFO& vecAllianceInfo = pCTFClient->GetAllianceInfo();
		GLCaptureTheFieldClient::VEC_ALLIANCEINFO_ITER iterInfo = vecAllianceInfo.begin();
		CTF_POS_VECTOR_ITER iterVector = m_vecCTF.begin();

		for ( ; iterInfo != vecAllianceInfo.end(); ++iterInfo )
		{
			SALLIANCEINFO& sAllianceInfo = (*iterInfo);

			if ( sAllianceInfo.IsNull() )
			{
				break;
			}
			else if ( sAllianceInfo.bMyInfo == true )
			{
				continue;
			}

			if ( iterVector != m_vecCTF.end() )
			{
				CUIControl* pControl = (CUIControl*) (*iterVector);
				UPDATE_CHAR_POS( pControl, sAllianceInfo.vPos , m_pLevelFile->GetBaseMapID() );

				iterVector++;
			}
			else
			{
				// 만들어서 넣음
				CUIControl* pControl = new CUIControl(m_pEngineDevice);
				pControl->CreateSub( this, "MINIMAP_CTFALLIANCE_MARK" );
				pControl->InitDeviceObjects ( m_pd3dDevice );
				pControl->RestoreDeviceObjects ( m_pd3dDevice );
				pControl->SetVisibleSingle( FALSE );
				RegisterControl( pControl );

				m_vecCTF.push_back ( pControl );
				UPDATE_CHAR_POS( pControl, sAllianceInfo.vPos, m_pLevelFile->GetBaseMapID()  );

				iterVector = m_vecCTF.end();
			}
		}
	}

	// 인증기 정보 순회
	{
		GLCaptureTheFieldClient::VEC_AUTHENTICATORINFO& vecAuthInfo = pCTFClient->GetCurrentGlobalState().vecAUTHENTICATOR;
		GLCaptureTheFieldClient::VEC_AUTHENTICATORINFO_ITER iterAuth = vecAuthInfo.begin();
		CTF_AUTHENTICATOR_VECTOR_ITER iterVector = m_vecCTFAuthenticator.begin();

		for ( ; iterAuth != vecAuthInfo.end(); ++iterAuth )
		{
			SAUTHENTICATORINFO& sAuthInfo = (*iterAuth);

			GASSERT( sAuthInfo.emAuthSchool != SCHOOL_NUM );

			if ( sAuthInfo.IsNull() )
			{
				break;
			}

			if ( iterVector != m_vecCTFAuthenticator.end() )
			{
				std::string strControl; 

				if( sAuthInfo.emAuthSchool == SCHOOL_NONE )
				{
					strControl = "LARGEMAP_CTFMARK_SCHOOL_NONE";
				}
				else
				{
					strControl = sc::string::format("LARGEMAP_CTFMARK_SCHOOL%1%", sAuthInfo.emAuthSchool);
				}

				CUIControl* pControl = (CUIControl*) (*iterVector);

				if ( pControl->GetControlNameEx() != strControl )
				{
					pControl->ResetControl( GetGlobalPos(), strControl.c_str() );
				}

				UPDATE_CHAR_POS( pControl, sAuthInfo.vPos , m_pLevelFile->GetBaseMapID() );
				iterVector++;
			}
			else
			{
				std::string strControl; 

				if( sAuthInfo.emAuthSchool == SCHOOL_NONE )
				{
					strControl = "LARGEMAP_CTFMARK_SCHOOL_NONE";
				}
				else
				{
					strControl = sc::string::format("LARGEMAP_CTFMARK_SCHOOL%1%", sAuthInfo.emAuthSchool);
				}

				// 만들어서 넣음
				CUIControl* pControl = new CUIControl(m_pEngineDevice);
				pControl->CreateSub( this, strControl.c_str() );
				pControl->InitDeviceObjects ( m_pd3dDevice );
				pControl->RestoreDeviceObjects ( m_pd3dDevice );
				pControl->SetVisibleSingle( FALSE );
				RegisterControl( pControl );

				m_vecCTFAuthenticator.push_back ( pControl );
				UPDATE_CHAR_POS( pControl, sAuthInfo.vPos , m_pLevelFile->GetBaseMapID() );

				iterVector = m_vecCTFAuthenticator.end();
			}
		}
	}
}

void RnOldLargeMapWindow::RESET_CTF()
{
	CTF_POS_VECTOR_ITER iterAlliance = m_vecCTF.begin();

	for ( ; iterAlliance != m_vecCTF.end(); ++iterAlliance )
	{
		CUIControl* pControl = (CUIControl*)(*iterAlliance);
		pControl->SetVisibleSingle( FALSE );
	}	

	CTF_AUTHENTICATOR_VECTOR_ITER iterAuth = m_vecCTFAuthenticator.begin();

	for ( ; iterAuth != m_vecCTFAuthenticator.end(); ++iterAuth )
	{
		CUIControl* pControl = (CUIControl*)(*iterAuth);
		pControl->SetVisibleSingle( FALSE );
	}	
}

bool RnOldLargeMapWindow::IS_CTF_UPDATE()
{
	// CTF 진행 중일때만 보임
	if ( !m_pGaeaClient->IsCTFPlayer() )
	{
		return false;
	}

	return true;
}

bool RnOldLargeMapWindow::IsMyCharacterMap()
{
	// 아이디로 하면 안되서 파일명으로 한다
	return strcmp(m_pGaeaClient->GetActiveMap()->GetFileName(),m_pLevelFile->GetFileName()) == 0;
}

void RnOldLargeMapWindow::MakeScrollGap()
{
	const UIRECT& rcGlobalPos = m_pMap->GetGlobalPos ();

	m_vScrollGap.x = m_nMAP_TEXSIZE_X - rcGlobalPos.sizeX;
	m_vScrollGap.y = m_nMAP_TEXSIZE_Y - rcGlobalPos.sizeY;

	if( m_vScrollGap.x < 0 )
		m_vScrollGap.x = 0;

	if( m_vScrollGap.y < 0 )
		m_vScrollGap.y = 0;
}

const D3DXVECTOR2 RnOldLargeMapWindow::GetScrollGap()
{
	D3DXVECTOR2 temp = m_vScrollGap;
	if( temp.x == 0 )
		temp.x = 1;
	if( temp.y == 0 )
		temp.y = 1;

	return temp;
}

const bool RnOldLargeMapWindow::Convert_WordlPos2MapRenderPos ( const D3DXVECTOR3& vPos, int& nPosX, int& nPosY, const int nRederHalfSizeX, const int nRederHalfSizeY )
{
    int nMapPosX(0), nMapPosY(0);
    CONVERT_WORLD2MAP ( vPos, nMapPosX, nMapPosY );

    nPosX = nMapPosX - m_nVIEW_START_X;
    nPosY = nMapPosY - m_nVIEW_START_Y;

    if ( nPosX < nRederHalfSizeX || nPosY < nRederHalfSizeY ) return false;
    if ( ((m_nVIEW_SIZE_X - nRederHalfSizeX) < nPosX) || ((m_nVIEW_SIZE_Y - nRederHalfSizeY) < nPosY) ) return false;

    return true;
}