#include "StdAfx.h"
#include "../../../SigmaCore/String/StringUtils.h"
#include "../../../SigmaCore/File/StringFile.h"

#include "../../../EngineLib/DxTools/TextureManager.h"
#include "../../../EngineLib/DxLand/DxLandMan.h"
#include "../../../EngineLib/DxTools/DxRenderStates.h"
#include "../../../EngineLib/DxTools/DxViewPort.h"
#include "../../../EngineLib/GUInterface/InterfaceCfg.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/G-Logic/GLPeriod.h"
#include "../../../EngineLib/GUInterface/BasicProgressBar.h"
#include "../../../EngineLib/DxTools/DxClubMan.h"

#include "../../../RanLogic/Land/GLMapAxisInfo.h"
#include "../../../RanLogic/RANPARAM.h"
#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLogicDataMan.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Stage/DxGlobalStage.h"
#include "../../../RanLogicClient/Tutorial/GLBaseTutorial.h" // by luxes.

#include "../../InnerInterface.h"
#include "MiniMapInfo.h"
#include "./MiniMap.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const int CMiniMap::nTIME_MINUTE = 60;

CMiniMap::CMiniMap(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
	: CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface( pInterface )
	, m_nMap_X(1)
	, m_nMap_Y(1)
	, m_nMapSize(1)
	, m_nMin_X(1)
	, m_nMin_Y(1)

	, m_bTEST( true )
	, m_wAmmoCount( 0 )

	, m_pMimiMapInfo(NULL)
	, m_pMiniMapInfoDummy(NULL)
	, m_pMiniMapInfoLeftDummy(NULL)
	, m_pMap(NULL)
	, m_pUserMark(NULL)
	, m_pXPos(NULL)
	, m_pYPos(NULL)
	, m_pMiniMapName(NULL)
	, m_pZoomOut(NULL)
	, m_pZoomIn(NULL)
	, m_pTime(NULL)
	, m_pTimeText(NULL)
	, m_pServerTimeText(NULL)
	, m_pDirection(NULL)
	, m_pArrowCountBack(NULL)
	, m_pArrowCountText(NULL)
	, m_pPK_ATTACK(NULL)
	, m_pPK_DEFENSE(NULL)
	, m_pFullScreenButton(NULL)
	, m_pFullScreenButtonDummy(NULL)
	, m_pAmmoText(NULL)
	, m_pCenterPoint(NULL)
	, m_pClubBattleTitle_Text(NULL)
    , m_pClubBattleTime_Text(NULL)
    , m_pClubBattleName_Text(NULL)
{
}

CMiniMap::~CMiniMap(void)
{
}

void CMiniMap::CreateSubControl()
{
	CD3DFontPar* pFont = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

	CBasicTextBox* pTimeText = new CBasicTextBox(m_pEngineDevice);
	pTimeText->CreateSub( this, "MINIMAP_TIME_TEXT" );
	pTimeText->SetFont( pFont );
	pTimeText->SetTextAlign( TEXT_ALIGN_CENTER_X );
	RegisterControl( pTimeText );
	m_pTimeText = pTimeText;
	
	
	CBasicTextBox* pServerTimeText = new CBasicTextBox(m_pEngineDevice);
	pServerTimeText->CreateSub( this, "MINIMAP_SERVER_TIME_TEXT" );
	pServerTimeText->SetFont( pFont );
	pServerTimeText->SetTextAlign( TEXT_ALIGN_CENTER_X );
	//{
	//	pServerTimeText->SetUseRender( TRUE );
	//	pServerTimeText->SetDiffuse( NS_UIDEBUGSET::PINK );
	//}
	RegisterControl( pServerTimeText );
	m_pServerTimeText = pServerTimeText;
	

	CBasicTextBox* pAmmoText = new CBasicTextBox(m_pEngineDevice);
	pAmmoText->CreateSub( this, "MINIMAP_AMMO_TEXT" );
	pAmmoText->SetFont( pFont );
	pAmmoText->SetTextAlign( TEXT_ALIGN_CENTER_X );
	RegisterControl( pAmmoText );
	m_pAmmoText = pAmmoText;

    CBasicTextBox* pClubBattleTitle_Text = new CBasicTextBox(m_pEngineDevice);
    pClubBattleTitle_Text->CreateSub( this, "MINIMAP_CLUB_TIME_TITLE_TEXT" );
    pClubBattleTitle_Text->SetFont( pFont );
    pClubBattleTitle_Text->SetTextAlign( TEXT_ALIGN_CENTER_X );
    RegisterControl( pClubBattleTitle_Text );
    m_pClubBattleTitle_Text = pClubBattleTitle_Text;
    m_pClubBattleTitle_Text->SetVisibleSingle( FALSE );

	CBasicTextBox* pClubBattleTime_Text = new CBasicTextBox(m_pEngineDevice);
	pClubBattleTime_Text->CreateSub( this, "MINIMAP_CLUB_TIME_TEXT" );
	pClubBattleTime_Text->SetFont( pFont );
	pClubBattleTime_Text->SetTextAlign( TEXT_ALIGN_CENTER_X );
    RegisterControl( pClubBattleTime_Text );
	m_pClubBattleTime_Text = pClubBattleTime_Text;
    m_pClubBattleTime_Text->SetVisibleSingle( FALSE );

    CBasicTextBox* pClubBattleName_Text = new CBasicTextBox(m_pEngineDevice);
    pClubBattleName_Text->CreateSub( this, "MINIMAP_CLUB_NAME_TEXT" );
    pClubBattleName_Text->SetFont( pFont );
    pClubBattleName_Text->SetTextAlign( TEXT_ALIGN_CENTER_X );
    RegisterControl( pClubBattleName_Text );
    m_pClubBattleName_Text = pClubBattleName_Text;
    m_pClubBattleName_Text->SetVisibleSingle( FALSE );

	CUIControl* pBack = new CUIControl(m_pEngineDevice);
	pBack->CreateSub( this, "MINIMAP_BACK", UI_FLAG_RIGHT );
	RegisterControl( pBack );	
	m_pBack = pBack; // by luxes.

	CUIControl* pDirection = new CUIControl(m_pEngineDevice);
	pDirection->CreateSub( this, "MINIMAP_DIRECTION", UI_FLAG_RIGHT );
	RegisterControl( pDirection );
	m_pDirection = pDirection;

	CUIControl* pCenterPoint = new CUIControl(m_pEngineDevice);
	pCenterPoint->CreateSub( this, "MINIMAP_CENTER_POINT", UI_FLAG_RIGHT );
	RegisterControl( pCenterPoint );
	m_pCenterPoint = pCenterPoint;

	CUIControl* pFullScreen = new CUIControl(m_pEngineDevice);
	pFullScreen->CreateSub( this, "MINIMAP_FULLSCREEN", UI_FLAG_RIGHT, MINIMAP_FULLSCREEN );
	RegisterControl( pFullScreen );
	m_pFullScreenButton = pFullScreen;

	CUIControl* pFullScreenDummy = new CUIControl(m_pEngineDevice);
	pFullScreenDummy->CreateSub( this, "MINIMAP_FULLSCREEN", UI_FLAG_RIGHT );
	pFullScreenDummy->SetVisibleSingle( FALSE );
	RegisterControl( pFullScreenDummy );
	m_pFullScreenButtonDummy = pFullScreenDummy;

	/////////////// 맴정보 컨트롤 생성 ////////////////
	CMiniMapInfo* pMiniMapInfo = new CMiniMapInfo(m_pGaeaClient, m_pInterface, m_pEngineDevice);
	pMiniMapInfo->CreateSub( this, "MINIMAP_INFO" );
	pMiniMapInfo->CreateSubControl();
	pMiniMapInfo->SetVisibleSingle( TRUE );
	RegisterControl( pMiniMapInfo );
	m_pMimiMapInfo = pMiniMapInfo;

	CMiniMapInfo* pMiniMapInfoDummy = new CMiniMapInfo(m_pGaeaClient, m_pInterface, m_pEngineDevice);
	pMiniMapInfoDummy->CreateSub( this, "MINIMAP_INFO" );
	pMiniMapInfoDummy->SetVisibleSingle( FALSE );
	RegisterControl( pMiniMapInfoDummy );
	m_pMiniMapInfoDummy = pMiniMapInfoDummy;

	CMiniMapInfo* pMiniMapInfoLeftDummy = new CMiniMapInfo(m_pGaeaClient, m_pInterface, m_pEngineDevice);
	pMiniMapInfoLeftDummy->CreateSub( this, "MINIMAP_INFO_LEFT" );
	pMiniMapInfoLeftDummy->SetVisibleSingle( FALSE );
	RegisterControl( pMiniMapInfoLeftDummy );
	m_pMiniMapInfoLeftDummy = pMiniMapInfoLeftDummy;
	///////////////////////////////////////////////////
}

void CMiniMap::SetMapAxisInfo(const GLMapAxisInfo &sInfo, const CString& strMapName )
{
	if( NULL == m_pMimiMapInfo ) return;

	if ( m_pMimiMapInfo->GetMapName() != strMapName )
	{
        m_pMimiMapInfo->SetMapName( strMapName );
		m_pMimiMapInfo->UpdatePostion();
	}
}

void CMiniMap::RotateMap()
{
	D3DXVECTOR3 vLookatPt = DxViewPort::GetInstance().GetLookatPt();
	D3DXVECTOR3 vFromPt = DxViewPort::GetInstance().GetFromPt();
	D3DXVECTOR3 vCameraDir = vLookatPt - vFromPt;

	static	D3DXVECTOR3 vLookatPtBACK;
	static	D3DXVECTOR3 vFromPtBACK;
	static	D3DXVECTOR3 vCameraDirBACK;

	if ( vLookatPtBACK == vLookatPt &&
		vFromPtBACK == vFromPt &&
		vCameraDirBACK == vCameraDir )
	{
		return ;
	}

	//	Note : 현제의 회전값.
	//
	float LengthXZ;
	float thetaY;

	LengthXZ = (float) sqrt( vCameraDir.x*vCameraDir.x + vCameraDir.z*vCameraDir.z );
	
	//	Note : 수식의 특성상 LengthXZ가... 0가되면... 문제가 발생한다.
	//
	if ( LengthXZ == 0 )	LengthXZ = 0.001f;

	thetaY = (float) acos( vCameraDir.x / LengthXZ );
	if ( vCameraDir.z>0 )	thetaY = -thetaY;

	thetaY += D3DX_PI / 2.f;

	{
		UIRECT	rcGlobalPosDir = m_pDirection->GetGlobalPos();
		D3DXVECTOR2	vCenterPos = D3DXVECTOR2( rcGlobalPosDir.left + (rcGlobalPosDir.right - rcGlobalPosDir.left)/2.f, rcGlobalPosDir.top + (rcGlobalPosDir.bottom - rcGlobalPosDir.top)/2.f );
		
		D3DXVECTOR2 vGlobalPosDir[4];
		vGlobalPosDir[0].x = rcGlobalPosDir.left - vCenterPos.x;
		vGlobalPosDir[0].y = rcGlobalPosDir.top - vCenterPos.y;
		vGlobalPosDir[1].x = rcGlobalPosDir.right - vCenterPos.x;
		vGlobalPosDir[1].y = rcGlobalPosDir.top - vCenterPos.y;
		vGlobalPosDir[2].x = rcGlobalPosDir.right - vCenterPos.x;
		vGlobalPosDir[2].y = rcGlobalPosDir.bottom - vCenterPos.y;
		vGlobalPosDir[3].x = rcGlobalPosDir.left - vCenterPos.x;
		vGlobalPosDir[3].y = rcGlobalPosDir.bottom - vCenterPos.y;

		D3DXVECTOR2 vOutTexturePos[4];
		for ( int i = 0; i < 4; i++ )
		{
			vOutTexturePos[i].x = (float) (cos(-thetaY)* vGlobalPosDir[i].x + sin(-thetaY)* vGlobalPosDir[i].y  ) + vCenterPos.x;
			vOutTexturePos[i].y = (float) (-sin(-thetaY)* vGlobalPosDir[i].x + cos(-thetaY)* vGlobalPosDir[i].y)  + vCenterPos.y;
			m_pDirection->SetGlobalPos(i , vOutTexturePos[i]);		
		}		
	}
}

void CMiniMap::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage( ControlID, dwMsg );

	switch ( ControlID )
	{
	case MINIMAP_FULLSCREEN:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
			{
				AddMessageEx( UIMSG_MOUSEIN_FULLSCREENBUTTON );

				if ( CHECK_LB_DOWN_LIKE( dwMsg ) )
				{
					const float fBUTTON_CLICK = CBasicButton::fBUTTON_CLICK;

					const UIRECT& rcPos = m_pFullScreenButtonDummy->GetGlobalPos();				
					m_pFullScreenButton->SetGlobalPos( D3DXVECTOR2 ( rcPos.left + fBUTTON_CLICK, rcPos.top + fBUTTON_CLICK ) );
				}

				if ( CHECK_LB_UP_LIKE( dwMsg ) )
				{
					if ( m_pGaeaClient->GetTutorial()->IsTutorial() )
					{
						m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("TUTORIAL_CANNOT_USE") );
					}
					else
					{
						//	함수 호출
                        DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
						pGlobalStage->ChangeWndMode();
					}
				}
			}
		}
		break;
	}
}

void CMiniMap::UpdateTime( float fElapsedTime )
{
	const float fTextureSize = 512.0f;		//	텍스처 크기
	const float fINTERPOLIATION = 0.5f;		//	12시간
	const float fViewSize = 102.0f;			//	한번에 보여지는 사이즈
	static const float fHalfUVRatio = (fViewSize / fTextureSize)/2;
	static const float fOneDaySec = GLPeriod::DAY_HOUR * GLPeriod::TIME_SEC;	

	const float fCurTime = (( GLPeriod::GetInstance().GetHour() * GLPeriod::TIME_SEC ) + 
		GLPeriod::GetInstance().GetSecond()) / fOneDaySec;

	UIRECT TextureRect = m_pTime->GetTexturePos();

	TextureRect.left = fCurTime - fHalfUVRatio + fINTERPOLIATION;
	TextureRect.right= fCurTime + fHalfUVRatio + fINTERPOLIATION;
	TextureRect.sizeX = TextureRect.right - TextureRect.left;

	m_pTime->SetTexturePos( TextureRect );
}

void CMiniMap::UpdateClubGuidBattleTime( const std::vector<float> &vecTime )
{
    // 선도전 갯수만큼 표시할 수 있게 변경 [2/19/2013 hsshin]
    std::string strCombine;
    const DWORD dwVecSize = vecTime.size();
    m_pClubBattleTime_Text->ClearText();
    for ( DWORD i = 0; i < dwVecSize; ++i )
    {
        strCombine = sc::string::format( "%02d : %02d", (DWORD)vecTime[i]/60, (DWORD)vecTime[i]%60 );
        m_pClubBattleTime_Text->AddText( strCombine.c_str() , NS_UITEXTCOLOR::RED );
    }
}

void CMiniMap::UpdateClubDeathMatchTime( const float fTime )
{
    std::string strCombine;
    strCombine = sc::string::format( "%02d : %02d", (DWORD)fTime/60, (DWORD)fTime%60 );
    m_pClubBattleTime_Text->SetText( strCombine.c_str() , NS_UITEXTCOLOR::RED );
}

void CMiniMap::SetClubGuidBattleName( const std::vector<std::string> &vecName )
{
    // 선도전 갯수만큼 이름 표시
    const DWORD dwVecSize = vecName.size();
    m_pClubBattleName_Text->ClearText();
    for ( DWORD i = 0; i < dwVecSize; ++i )
    {
        m_pClubBattleName_Text->AddText( vecName[i].c_str() , NS_UITEXTCOLOR::RED );
    }
}

void CMiniMap::StartClubGuidBattleTime()
{
    if ( m_pClubBattleTitle_Text )
    {
        m_pClubBattleTitle_Text->SetText( ID2GAMEWORD("CLUB_BATTLE_TIME", 1) , NS_UITEXTCOLOR::RED );
        m_pClubBattleTitle_Text->AddText( ID2GAMEWORD("CLUB_BATTLE_TIME", 0) , NS_UITEXTCOLOR::RED );
        m_pClubBattleTitle_Text->SetVisibleSingle( TRUE );
    }
    if ( m_pClubBattleTime_Text )
    {
        m_pClubBattleTime_Text->SetVisibleSingle( TRUE );
    }
    if ( m_pClubBattleName_Text )
    {
        m_pClubBattleName_Text->SetVisibleSingle( TRUE );
    }
}

void CMiniMap::StartClubDeathMatchTime()
{
    if ( m_pClubBattleTitle_Text )
    {
        m_pClubBattleTitle_Text->SetText( ID2GAMEWORD("CLUB_BATTLE_TIME", 2) , NS_UITEXTCOLOR::RED );
        m_pClubBattleTitle_Text->AddText( ID2GAMEWORD("CLUB_BATTLE_TIME", 0) , NS_UITEXTCOLOR::RED );
        m_pClubBattleTitle_Text->SetVisibleSingle( TRUE );
    }
    if ( m_pClubBattleTime_Text )
    {
        m_pClubBattleTime_Text->SetVisibleSingle( TRUE );
    }
    if ( m_pClubBattleName_Text )
    {
        m_pClubBattleName_Text->SetVisibleSingle( FALSE );
    }
}

void CMiniMap::EndClubBattleTime()
{
    if ( m_pClubBattleTitle_Text )
    {
        m_pClubBattleTitle_Text->SetVisibleSingle( FALSE );
    }
    if ( m_pClubBattleTime_Text )
    {
        m_pClubBattleTime_Text->SetVisibleSingle( FALSE );
    }
    if ( m_pClubBattleName_Text )
    {
        m_pClubBattleName_Text->SetVisibleSingle( FALSE );
    }
}

void CMiniMap::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{	
	if ( m_pFullScreenButton->IsVisible() )
	{
		const UIRECT& rcGlobalPos = m_pFullScreenButtonDummy->GetGlobalPos();
		m_pFullScreenButton->SetGlobalPos( D3DXVECTOR2(rcGlobalPos.left,rcGlobalPos.top) );
	}

	CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
	
	RotateMap();

	if ( m_pTimeText )
	{
		CString strCombine;
		const int nMonth = (int)GLPeriod::GetInstance().GetMonth();
		const int nDay = (int)GLPeriod::GetInstance().GetMonthToday();
		const int nHour = (int)GLPeriod::GetInstance().GetHour();
		const int nMinute = (int)(nTIME_MINUTE * GLPeriod::GetInstance().GetSecond() / GLPeriod::TIME_SEC);
		static int nMinuteBACK;
		if ( nMinute != nMinuteBACK )
		{
			nMinuteBACK = nMinute;

			strCombine.Format( "%02d/%02d %02d:%02d", nMonth, nDay, nHour, nMinute );
			m_pTimeText->SetOneLineText( strCombine, NS_UITEXTCOLOR::WHITE );
		}
	}
	
	
	if ( m_pServerTimeText ) // Monster7j
	{
		CString strCombine;
	
		// 현재시각
		CTime cTime(m_pGaeaClient->GetCurrentTime());

		int nServerMinute = (int) cTime.GetMinute();
		static int nServerMinuteBACK;
		if ( nServerMinute != nServerMinuteBACK )
		{
			nServerMinuteBACK = nServerMinute;
			
			//	서버 타임으로 변환( TimeZone 계산 )
			m_pGaeaClient->GetConvertServerTime( cTime );

			const int nServerMonth = (int)cTime.GetMonth();
			const int nServerDay = (int)cTime.GetDay();
			const int nServerHour = (int)cTime.GetHour();
			nServerMinute = (int)cTime.GetMinute();

			strCombine.Format( "%02d/%02d %02d:%02d", nServerMonth, nServerDay, nServerHour, nServerMinute );
			m_pServerTimeText->SetOneLineText( strCombine, NS_UITEXTCOLOR::WHITE );
		}
	}
	
	m_pAmmoText->SetVisibleSingle( FALSE );

	//	화살, 부적 업데이트
	EMSLOT emLHand = m_pGaeaClient->GetCharacter()->GetCurLHand();
	if ( m_pGaeaClient->GetCharacter()->VALID_SLOT_ITEM(emLHand) )
	{
		SITEMCUSTOM sItemCustom = m_pGaeaClient->GetCharacter()->GET_SLOT_ITEM(emLHand);
		const SITEM* pItemData = GLogicData::GetInstance().GetItem( sItemCustom.GetNativeID() );

		EMITEM_TYPE emItemType = pItemData->sBasicOp.emItemType;
		if ( emItemType == ITEM_ARROW || emItemType == ITEM_CHARM || emItemType == ITEM_BULLET )
		{
			int nType = 0;
			if ( emItemType == ITEM_ARROW ) nType = 0;
			else if ( emItemType == ITEM_CHARM ) nType = 1;
			else if ( emItemType == ITEM_BULLET ) nType = 2;

			if ( sItemCustom.wTurnNum != m_wAmmoCount )
			{
				CString strCombine;
				strCombine.Format( "%s : %d", ID2GAMEWORD("ARROW_TYPE",nType), sItemCustom.wTurnNum );

                DWORD ArrowColor = NS_UITEXTCOLOR::WHITE;
				if ( sItemCustom.wTurnNum < 10 ) ArrowColor = NS_UITEXTCOLOR::RED;
				else if( sItemCustom.wTurnNum < 50 ) ArrowColor = NS_UITEXTCOLOR::YELLOW;

				m_pAmmoText->SetText( strCombine.GetString(), ArrowColor );

				m_wAmmoCount = sItemCustom.wTurnNum;
			}

			m_pAmmoText->SetVisibleSingle( TRUE );
		}
	}

	bool bFULL_SCREEN_ON_WINDOW = RANPARAM::bScrWindowed;	// && RANPARAM::bScrWndHalfSize;	// 필요없는 데이터
	if ( bFULL_SCREEN_ON_WINDOW )
	{
		const UIRECT& rcGlobalPosDummy = m_pMiniMapInfoLeftDummy->GetGlobalPos();
		const UIRECT& rcGlobalPos = m_pMimiMapInfo->GetGlobalPos();
		if ( rcGlobalPosDummy.left != rcGlobalPos.left || rcGlobalPosDummy.top != rcGlobalPos.top )
		{
			m_pMimiMapInfo->SetGlobalPos( rcGlobalPosDummy );
		}

		m_pFullScreenButton->SetVisibleSingle( TRUE );
	}
	else
	{
		const UIRECT& rcGlobalPosDummy = m_pMiniMapInfoDummy->GetGlobalPos();
		const UIRECT& rcGlobalPos = m_pMimiMapInfo->GetGlobalPos();
		if ( rcGlobalPosDummy.left != rcGlobalPos.left || rcGlobalPosDummy.top != rcGlobalPos.top )
		{
			m_pMimiMapInfo->SetGlobalPos( rcGlobalPosDummy );
		}

		m_pFullScreenButton->SetVisibleSingle( FALSE );
	}
}

HRESULT CMiniMap::Render( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;

	m_pCenterPoint->SetVisibleSingle( FALSE );
	m_pDirection->SetVisibleSingle( FALSE );

	hr = CUIGroup::Render( pd3dDevice );
	if ( FAILED ( hr ) ) return hr;

	m_pDirection->SetVisibleSingle( TRUE );
	m_pCenterPoint->SetVisibleSingle( TRUE );

	{
		//	NOTE
		//		RENDER STATES BEGIN
		DWORD dwMinFilter, dwMagFilter, dwMipFilter;
		pd3dDevice->GetSamplerState( 0, D3DSAMP_MINFILTER, &dwMinFilter );
		pd3dDevice->GetSamplerState( 0, D3DSAMP_MAGFILTER, &dwMagFilter );
		pd3dDevice->GetSamplerState( 0, D3DSAMP_MIPFILTER, &dwMipFilter );

		pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
		pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

		hr = m_pDirection->Render ( pd3dDevice );

		//	NOTE
		//		RENDER STATES END
		pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, dwMinFilter );
		pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, dwMagFilter );
		pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, dwMipFilter );

		if ( FAILED ( hr ) )	return hr;
	}     

	return m_pCenterPoint->Render( pd3dDevice );
}

const UIRECT& CMiniMap::GetMinimapBackGlobalPos()
{
    return m_pBack->GetGlobalPos();
}

MyMiniMap::MyMiniMap( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CMiniMap( pGaeaClient, pInterface, pEngineDevice )
{
}

void MyMiniMap::CreateUIWindowAndRegisterOwnership()
{
    CMiniMap::Create( MINIMAP, "BASIC_MINIMAP", UI_FLAG_RIGHT );
    CMiniMap::CreateSubControl();
    CMiniMap::m_pInterface->UiRegisterControl( this );
    CMiniMap::m_pInterface->UiShowGroupBottom( MINIMAP );
}

void MyMiniMap::SetMapAxisInfo(const GLMapAxisInfo &sInfo, const UI::String& strMapName )
{
    CMiniMap::SetMapAxisInfo( sInfo, UI::ToString( strMapName ) );
}

void MyMiniMap::UpdateClubGuidBattleTime( const std::vector<float> &vecTime )
{
    CMiniMap::UpdateClubGuidBattleTime( vecTime );
}

void MyMiniMap::UpdateClubDeathMatchTime( const float fTime )
{
    CMiniMap::UpdateClubDeathMatchTime( fTime );
}

void MyMiniMap::SetClubGuidBattleName( const std::vector<std::string> &vecName )
{
    CMiniMap::SetClubGuidBattleName( vecName );
}

void MyMiniMap::StartClubGuidBattleTime()
{
    CMiniMap::StartClubGuidBattleTime();
}

void MyMiniMap::StartClubDeathMatchTime()
{
    CMiniMap::StartClubDeathMatchTime();
}

void MyMiniMap::EndClubBattleTime()
{
    CMiniMap::EndClubBattleTime();
}


const UIRECT& MyMiniMap::GetUIWindowGlobalPos()
{
    return CMiniMap::GetGlobalPos();
}

const UIRECT&MyMiniMap::GetMinimapBackGlobalPos()
{
    return CMiniMap::GetMinimapBackGlobalPos();
}
