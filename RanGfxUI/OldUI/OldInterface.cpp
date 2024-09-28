#include "StdAfx.h"
#include "OldInterface.h"

#include "../../enginelib/DeviceMan.h"
#include "../../enginelib/GUInterface/BasicTextBox.h"
#include "../../enginelib/GUInterface/UIMan.h"
#include "../../enginelib/GUInterface/UIMutuallyExclusive.h"
#include "../../enginelib/DxTools/DxViewPort.h"
#include "../../RanLogicClient/GLGaeaClient.h"
#include "../../RanLogicClient/Land/GLLandManClient.h"
#include "../../RanLogicClient/Char/GLCharacter.h"
#include "../../RanLogicClient/Tool/DxParamSet.h"

#include "./Display/DamageDisplayMan.h"
#include "./Display/NameDisplayMan.h"
#include "./Display/SimpleHP.h"
#include "./Display/PrivateMarketShowMan.h"
#include "./Display/HeadChatDisplayMan.h"
#include "./Display/TargetInfoDisplay.h"
#include "./Display/CursorTargetInfo.h"

namespace uiman
{
	CUIMan s_innerUIMan;
	CUIMan s_outerUIMan;

	CUIMan& GetInnerUIMan()
	{
		return s_innerUIMan;
	}

	CUIMan& GetOuterUIMan()
	{
		return s_outerUIMan;
	}
};

OldInterface::OldInterface( GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice )
: m_pGaeaClient( pGaeaClient )
, m_pEngineDevice( pEngineDevice )
, m_pDamageDisplayMan( NULL )
{
}

OldInterface::~OldInterface()
{
}

void OldInterface::UiSetResolution( long lResolution )
{
	uiman::GetInnerUIMan().SetResolution( lResolution );
}

long OldInterface::UiGetResolution()
{
	return CUIMan::GetResolution();
}

HRESULT OldInterface::UiFinalCleanup()
{
	return FinalCleanup();
}

HRESULT OldInterface::UiFrameMove( LPDIRECT3DDEVICEQ pd3dDevice, float fElapsedTime )
{
	return FrameMove(pd3dDevice, fElapsedTime);
}

HRESULT OldInterface::UiRender( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return uiman::GetInnerUIMan().Render( pd3dDevice );
}

HRESULT OldInterface::UiInitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return InitDeviceObjects(pd3dDevice);
}

HRESULT OldInterface::UiRestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return uiman::GetInnerUIMan().RestoreDeviceObjects( pd3dDevice );
}

HRESULT OldInterface::UiInvalidateDeviceObjects()
{
	return uiman::GetInnerUIMan().InvalidateDeviceObjects();
}

HRESULT OldInterface::UiDeleteDeviceObjects()
{
	return DeleteDeviceObjects();
}

BOOL OldInterface::UiRegisterControl( CUIControl* pNewControl, bool bPosisionKeep )
{
	return uiman::GetInnerUIMan().RegisterControl( pNewControl, bPosisionKeep );
}

BOOL OldInterface::UiIsVisibleGroup( UIGUID ControlID )
{
	return uiman::GetInnerUIMan().IsVisibleGroup( ControlID );
}

void OldInterface::UiShowGroupTop(UIGUID ControlID, bool bMakeMsg)
{ 
	uiman::GetInnerUIMan().ShowGroupTop(ControlID, bMakeMsg); 
}

void OldInterface::UiShowGroupBottom(UIGUID ControlID, bool bMakeMsg)
{ 
	uiman::GetInnerUIMan().ShowGroupBottom(ControlID, bMakeMsg); 
}

void OldInterface::UiShowGroupUpdate(UIGUID ControlID, bool bMakeMsg)
{ 
	uiman::GetInnerUIMan().ShowGroupUpdate(ControlID, bMakeMsg); 
}

void OldInterface::UiShowGroupFocus(UIGUID ControlID, bool bMakeMsg)
{ 
	UIGUID uiID = UIMutuallyExclusive::GetInstance().GetMutually( ControlID );
	if( uiman::GetInnerUIMan().IsVisibleGroup(uiID) )
		uiman::GetInnerUIMan().HideGroup( ControlID );
	else
		uiman::GetInnerUIMan().ShowGroupFocus(ControlID, bMakeMsg);
}

void OldInterface::UiSetGroupLock(UIGUID ControlID, bool bLock)
{
	uiman::GetInnerUIMan().SetGroupLock(ControlID, bLock);
}

void OldInterface::UISetModalGroup(UIGUID ControlID, bool bModal)
{
	uiman::GetInnerUIMan().SetModalGroup(ControlID, bModal);
}

void OldInterface::UiHideGroup(UIGUID ControlID, bool bMakeMsg)
{
	uiman::GetInnerUIMan().HideGroup(ControlID, bMakeMsg); 
}

void OldInterface::UiRefresh( UIGUID ControlID )
{
	uiman::GetInnerUIMan().Refresh( ControlID ); 
}

HRESULT OldInterface::InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{

	{
		m_pSimpleHP = new CSimpleHP ( m_pGaeaClient, m_pEngineDevice );
		m_pSimpleHP->Create ( SIMPLE_HP, "SIMPLE_HP" );
		m_pSimpleHP->CreateSubControl();
		UiRegisterControl ( m_pSimpleHP );
		UiShowGroupBottom ( SIMPLE_HP );
		//UiHideGroup( SIMPLE_HP );
	}


	m_pNameDisplayMan = new CNameDisplayMan( m_pGaeaClient, this, m_pEngineDevice );
	m_pNameDisplayMan->Create( NAME_DISPLAY_MAN, "NAME_DISPLAY_MAN" );
	UiRegisterControl( m_pNameDisplayMan );
	UiShowGroupBottom( NAME_DISPLAY_MAN );
	//UiHideGroup( NAME_DISPLAY_MAN );

	m_pDamageDisplayMan = new CDamageDisplayMan( m_pEngineDevice );
	m_pDamageDisplayMan->Create( DAMAGE_MAN, "DAMAGE_MAN" );
	m_pDamageDisplayMan->CreateSubControl();
	UiRegisterControl( m_pDamageDisplayMan );
	UiShowGroupUpdate( DAMAGE_MAN );

	/*m_pPrivateMarketShowMan = new CPrivateMarketShowMan( m_pGaeaClient, this, m_pEngineDevice );
	m_pPrivateMarketShowMan->Create( PRIVATE_MARKET_SHOW_MAN, "NAME_DISPLAY_MAN" );
	UiRegisterControl( m_pPrivateMarketShowMan );
	UiShowGroupBottom( NAME_DISPLAY_MAN  );

	m_pHeadChatDisplayMan = new CHeadChatDisplayMan( m_pGaeaClient, this, m_pEngineDevice );
	m_pHeadChatDisplayMan->Create( HEADCHAT_MAN, "HEADCHAT_MAN" );
	UiRegisterControl( m_pHeadChatDisplayMan );
	UiShowGroupBottom( HEADCHAT_MAN  );*/

	m_pTargetInfoDisplay = new CTargetInfoDisplay( m_pGaeaClient, this, m_pEngineDevice );
	m_pTargetInfoDisplay->Create( TARGETINFO_DISPLAY, "TARGETINFO_DISPLAY" );
	m_pTargetInfoDisplay->CreateSubControl();
	UiRegisterControl( m_pTargetInfoDisplay );
	UiShowGroupBottom( TARGETINFO_DISPLAY  );

	m_pCursorTargetInfo = new CCursorTargetInfo( m_pGaeaClient, this, m_pEngineDevice );
	m_pCursorTargetInfo->Create( CURSOR_TARGET_INFO, "BASIC_VAR_TEXT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	m_pCursorTargetInfo->CreateSubControl();
	m_pCursorTargetInfo->RightBottomGap();
	m_pCursorTargetInfo->SetBlockMouseTracking( true );
	m_pCursorTargetInfo->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
	UiRegisterControl( m_pCursorTargetInfo );

	return uiman::GetInnerUIMan().InitDeviceObjects( pd3dDevice );
}

HRESULT OldInterface::DeleteDeviceObjects()
{
	ResetControl();

	return uiman::GetInnerUIMan().DeleteDeviceObjects();
}

HRESULT OldInterface::FrameMove( LPDIRECT3DDEVICEQ pd3dDevice, float fElapsedTime )
{
	ResetTargetInfoMouseOver();

	if( m_pSimpleHP != NULL )
		UpdateStateSimpleHP();

	if( m_pCursorTargetInfo )
		m_pCursorTargetInfo->UpdateInfo();


	HRESULT hr = uiman::GetInnerUIMan().FrameMove( fElapsedTime, this );

	return hr;
}

HRESULT OldInterface::FinalCleanup()
{
	return uiman::GetInnerUIMan().FinalCleanup();
}

void OldInterface::ResetControl()
{
	m_pDamageDisplayMan = NULL;
	m_pNameDisplayMan = NULL;
}

void OldInterface::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
}

void OldInterface::SetDamage( D3DXVECTOR3 vPos, int nDamage, DWORD dwDamageFlag, BOOL bAttack )
{
	//	카메라 프리즘 공간안에 데미지 위치가 있는지 검사.
	//
	const CLIPVOLUME &CV = DxViewPort::GetInstance().GetClipVolume();
	BOOL bOK = COLLISION::IsCollisionVolume(CV,vPos,vPos);
	if ( !bOK )				return;

	//	지형에 가려지는 부분에서 데미지가 위치하는지 검사.
	//
	GLLandManClient* pLandClient = m_pGaeaClient->GetActiveMap();
	GASSERT(pLandClient&&"m_pGaeaClient->GetActiveMap()");
	if ( !pLandClient )		return;

	DxLandMan *pLandMan = pLandClient->GetLandMan();
	GASSERT(pLandMan&&"pLandClient->GetLandMan()");
	if ( !pLandMan )		return;

	//카메라의 시야로 인한 컬링
	D3DXVECTOR3 vPoint1 = DxViewPort::GetInstance().GetFromPt();
	D3DXVECTOR3 vPoint2 = vPos;
	D3DXVECTOR3 vCollision;
	BOOL bCollision(FALSE);
	LPDXFRAME pDxFrame;
	pLandMan->IsCollision( vPoint1, vPoint2, vCollision, bCollision, pDxFrame, EMCC_CULL_CCW );
	if ( bCollision )		return;

	//오브젝트 컬링 옵션이 켜져있는 맵일 경우
	if ( pLandClient->IsCullByObjectMap() == true )
	{
		//캐릭터의 시야로 인한 컬링
		bCollision = FALSE;
		vPoint1 = m_pGaeaClient->GetCharacter()->GetPosition();
		pLandMan->IsCollision( vPoint1, vPoint2, vCollision, bCollision, pDxFrame, EMCC_CULL_CCW );
		if ( bCollision )		return;
	}

	D3DXVECTOR3 vScreen = DxViewPort::GetInstance().ComputeVec3Project ( &vPos, NULL );
	const UIRECT& rcOriginPos = m_pDamageDisplayMan->GetGlobalPos ();

	//	Note : 데미지 좌우로 흩어져서 나오도록.
	//	20 이하의 값에서 설정
	int nPosX = (int)( vScreen.x - ( rcOriginPos.sizeX * 0.5f ));
	int nPosY = (int)( vScreen.y - rcOriginPos.sizeY );

	static const int nMAXINTERVAL = 20;
	static const int nBASE = int(nMAXINTERVAL / 2);

	int xInterval = rand()%nMAXINTERVAL - nBASE;
	int yInterval = rand()%nMAXINTERVAL - nBASE;
	nPosX += xInterval;
	nPosY += yInterval;

	long lResolution = uiman::GetInnerUIMan().GetResolution ();
	WORD X_RES = HIWORD(lResolution);
	WORD Y_RES = LOWORD(lResolution);
	if ( ( nPosX < 0.0f ) || ( nPosY < 0.0f )  || 
		( float(X_RES) < nPosX )  || ( float(Y_RES) < nPosY ) )
	{
		return ;
	}

	m_pDamageDisplayMan->SetDamage ( nPosX, nPosY, nDamage, dwDamageFlag, bAttack );
}

void OldInterface::NameDisplaySetCount( DWORD Count )
{
	m_pNameDisplayMan->SET_COUNT( Count );
}

void OldInterface::NameDisplayAddName( CROWREN &sDISP_NAME, const char * szOwnerName )
{
	m_pNameDisplayMan->ADD_DISP_NAME( sDISP_NAME, szOwnerName );
}

void OldInterface::NameDisplayShowCountryMark( bool bShowCountry )
{
	m_pNameDisplayMan->SetShowCountryMark( bShowCountry );
}


void	OldInterface::SetPrivateMarketMake ()
{	
	if( !UiIsVisibleGroup ( PRIVATE_MARKET_MAKE ) )
	{
		// Scaleform 개인상점이 열리도록 수정해야함
		/*
		if ( m_pPrivateMarketMake )
		{
			m_pPrivateMarketMake->Init ();
		}
		*/
	}

	UiShowGroupFocus ( PRIVATE_MARKET_MAKE, true ); // 
}

void OldInterface::SetPrivateMarketOpen( const bool& bOPENER, const DWORD& dwGaeaID )
{
	if ( UiIsVisibleGroup( PRIVATE_MARKET_WINDOW ) )
		return;

	// Scaleform 개인상점이 열리도록 수정해야함 ㅇㅇ

	/*if ( !m_pPrivateMarketWindow )
		return;

	m_pPrivateMarketWindow->InitPrivateMarket( bOPENER, dwGaeaID );

	if( m_pTradeInventoryWindow )
		m_pTradeInventoryWindow->SetOpener( bOPENER );

	CloseAllWindow();

	const long lResolution = uiman::GetInnerUIMan().GetResolution();
	WORD X_RES = HIWORD ( lResolution );
	WORD Y_RES = LOWORD ( lResolution );

	{
		const UIRECT& rcPrivateMarketWindow = m_pPrivateMarketWindow->GetGlobalPos();

		D3DXVECTOR2 vPos;
		vPos.x = ((X_RES) / 2.0f) - rcPrivateMarketWindow.sizeX;
		vPos.y = (Y_RES - rcPrivateMarketWindow.sizeY) / 2.0f;
		m_pPrivateMarketWindow->SetGlobalPos ( vPos );

		UiShowGroupFocus( PRIVATE_MARKET_WINDOW );
	}

	if( GLUseFeatures::GetInstance().IsUsingRenewInvenWindow() )
	{
		m_pInventory->CallTapSelect( CRnInventoryWindow::ETapInven );
		UiShowGroupFocus( INVENTORY_WINDOW_RENEW );
	}
	else
	{
		const UIRECT& rcTradeInventoryWindow = GetTradeInventoryWindow()->GetUIWindowGlobalPos();

		D3DXVECTOR2 vPos;
		vPos.x = (X_RES) / 2.0f;
		vPos.y = (Y_RES - rcTradeInventoryWindow.sizeY) / 2.0f;
		GetTradeInventoryWindow()->SetUIWindowGlobalPos( vPos );

		UiShowGroupFocus( TRADEINVENTORY_WINDOW );
	}*/
}

void OldInterface::ADD_MARKET(const CROWREN& sDispName )
{
	m_pPrivateMarketShowMan->ADD_MARKET(sDispName);
}

void OldInterface::GetPrivateMarketInfo ( bool& bOPENER, DWORD& dwGaeaID )
{	
	// 스케일폼 UI로 수정해야함

	/*
	bOPENER = m_pPrivateMarketWindow->IsOPENER ();
	dwGaeaID = m_pPrivateMarketWindow->GetPrivateMarketID ();;
	*/
}

void OldInterface::SetPrivateMarketClose ()
{
	// 스케일폼 UI로 수정해야함

	UiHideGroup ( PRIVATE_MARKET_WINDOW );

	if( GLUseFeatures::GetInstance().IsUsingRenewInvenWindow() )
		UiHideGroup ( INVENTORY_WINDOW_RENEW );
	else
		UiHideGroup ( TRADEINVENTORY_WINDOW );

	/*
	if ( m_pPrivateMarketWindow->IsOPENER () )
		m_pGaeaClient->GetCharacter()->ReqPMarketClose ();
	else
		m_pGaeaClient->GetCharacter()->ReqPMarketInfoRelease ( m_pPrivateMarketWindow->GetPrivateMarketID () );
	*/
}

void OldInterface::ResetTargetInfoMouseOver()
{
	if ( !m_pTargetInfoDisplay )
	{
		return;
	}

	m_pTargetInfoDisplay->SetMouseOverVisibleOff();
}


void OldInterface::ResetTargetInfoSelect()
{
	if ( !m_pTargetInfoDisplay )
	{
		return;
	}

	m_pTargetInfoDisplay->SetSelectVisibleOff();
}


void OldInterface::SetTargetInfoMouseOver ( STARGETID sTargetID )
{
	SetTargetInfo( sTargetID, UI::EMTARGETINFO_TYPE_MOUSEOVER );
}


void OldInterface::SetTargetInfoSelect ( STARGETID sTargetID )
{
	SetTargetInfo( sTargetID, UI::EMTARGETINFO_TYPE_SELECT );
}

void OldInterface::SetTargetInfo( STARGETID sTargetID, const UI::EMTARGETINFO_TYPE emTargetInfoType )
{
	if ( sTargetID == STARGETID() )
	{
		return;
	}

	if ( !m_pTargetInfoDisplay ) 
	{
		return;
	}

	if ( !m_pTargetInfoDisplay->CheckPriority( emTargetInfoType, sTargetID ) )
	{
		return;
	}

	boost::function<void (CTargetInfoDisplay*, const STARGETID&, float, CString, D3DCOLOR)>		 fnSetTargetInfo;
	boost::function<void (CTargetInfoDisplay*, const STARGETID&, float, float, float, ENUM_SCHOOL)>  fnSetAuthenticatorInfo;
	boost::function<void (CTargetInfoDisplay*, const D3DXVECTOR2&)>									 fnSetGlobalPos;
	boost::function<const UIRECT& (CTargetInfoDisplay*)>											 fnGetGlobalPos;

	switch(emTargetInfoType)
	{
	case UI::EMTARGETINFO_TYPE_MOUSEOVER:
		{
			fnSetTargetInfo			= &CTargetInfoDisplay::SetMouseOverTargetInfo;
			fnSetAuthenticatorInfo	= &CTargetInfoDisplay::SetMouseOverAuthenticatorInfo;
			fnSetGlobalPos			= &CTargetInfoDisplay::SetMouseOverGlobalPos;
			fnGetGlobalPos			= &CTargetInfoDisplay::GetMouseOverGlobalPos;
		}
		break;

	case UI::EMTARGETINFO_TYPE_SELECT:
		{
			fnSetTargetInfo			= &CTargetInfoDisplay::SetSelectTargetInfo;
			fnSetAuthenticatorInfo	= &CTargetInfoDisplay::SetSelectAuthenticatorInfo;
			fnSetGlobalPos			= &CTargetInfoDisplay::SetSelectGlobalPos;
			fnGetGlobalPos			= &CTargetInfoDisplay::GetSelectGlobalPos;
		}
		break;

	default:
		return;
	};

	CString strName;
	D3DCOLOR dwColor = NS_UITEXTCOLOR::DEFAULT;
	GLDWDATA sHP;

	ClientActor *pCOPY = m_pGaeaClient->GetCopyActor ( sTargetID );
	if ( !pCOPY ) return;

	strName = pCOPY->GetName();
	sHP = pCOPY->GetHp();

	switch ( pCOPY->GetCrow() )
	{
	case CROW_MOB:
	case CROW_NPC:
		{
#ifndef INCLUDE_SCALEFORM
			GLCrowClient* pCrow = dynamic_cast<GLCrowClient*>(pCOPY);

			if ( !pCrow )
			{
				return;
			}

			dwColor = m_pGaeaClient->GetMobNameColor ( sTargetID.GaeaId );

			switch ( pCrow->m_pCrowData->m_emNPCType )
			{
			case NPC_TYPE_AUTHENTICATOR:
				// 인증기의 경우 HP 표시를 다르게 한다.;
				{
					float fPercentSM = float(pCrow->m_arrHP4School[SCHOOL_SM]) / float(sHP.dwMax);
					float fPercentHA = float(pCrow->m_arrHP4School[SCHOOL_HA]) / float(sHP.dwMax);
					float fPercentBH = float(pCrow->m_arrHP4School[SCHOOL_BH]) / float(sHP.dwMax);

					fnSetAuthenticatorInfo( m_pTargetInfoDisplay, sTargetID, fPercentSM, fPercentHA, fPercentBH, ENUM_SCHOOL(pCrow->m_wSchoolAuthed) );
				}
				break;

			case NPC_TYPE_NORMAL:
			default:
				{
					float fPercent = float(sHP.dwNow) / float(sHP.dwMax);

					if ( !pCrow->IsValidBody() )
					{
						fPercent = 0.0f;
					}

					fnSetTargetInfo( m_pTargetInfoDisplay, sTargetID, fPercent, CString( strName ), dwColor );
				}
				break;
			}
#endif
		}
		break;

	case CROW_PC:
		{
			GLLandManClient* pLand = m_pGaeaClient->GetActiveMap();
			if ( pLand == NULL )
				return;

			std::tr1::shared_ptr<GLCharClient> pCHAR = m_pGaeaClient->GetChar ( sTargetID.GaeaId );
			if ( !pCHAR )
				return;

			bool bOldVersion(false);
			bool bHpVisible(true);

			Faction::ManagerClient* const pFactionMan = pLand->GetFactionManager();
			if ( pFactionMan )
			{
				const Faction::EMRELATION_FLAG_TYPE _emHpVIsible = pFactionMan->getHPVisible();
				if ( _emHpVIsible == Faction::EMRELATION_FLAG_ERROR )
				{
					bOldVersion = true;
				}
				else
				{
					const Faction::EMRELATION_TYPE _emRelation = pFactionMan->getRelationClient(m_pGaeaClient->GetCharacter(), pCHAR.get());

					switch ( _emRelation )
					{
					case Faction::EMRELATION_ENEMY:
						bHpVisible = (_emHpVIsible & Faction::EM_RELATION_FLAG_ENEMY);
						break;
					case Faction::EMRELATION_NEUTRAL_ENEMY:
					case Faction::EMRELATION_NEUTRAL_ALLY:
						bHpVisible = (_emHpVIsible & Faction::EM_RELATION_FLAG_NEUTRAL);
						break;
					case Faction::EMRELATION_ALLY:
						bHpVisible = (_emHpVIsible & Faction::EM_RELATION_FLAG_ALLY);
						break;
					default:
						bOldVersion = true;
						break;
					}
				}
			}
			else
			{
				bOldVersion = true;
			}

			if ( bOldVersion )
			{
				// 구버전 HP표시 방식 : 대련,특정 PVP 컨텐츠이용시 예외처리로 HP 표시기능을 제한시킴;
				bool bPKServer		= m_pGaeaClient->IsPKServer();
				bool bBRIGHTEVENT   = m_pGaeaClient->IsBRIGHTEVENT();
				bool bSCHOOL_FREEPK = m_pGaeaClient->IsSchoolFreePk();
				bool bCTFPlayer	    = m_pGaeaClient->IsCTFPlayer();
				bool bPKMap         = pLand->IsPKMap();
				bool bGuidBattleMap = ( pLand->m_bClubBattle || pLand->m_bClubDeathMatch );

				bHpVisible = true;
				//	대련 또는 PK가 아니면, 피를 표시하지 않는다.
				if ( !m_pGaeaClient->GetCharacter()->ISCONFRONT_TAR ( sTargetID ) &&
					!m_pGaeaClient->GetCharacter()->IS_PLAYHOSTILE ( pCOPY->GetCharID() ) )
				{
					if ( !(bBRIGHTEVENT || bSCHOOL_FREEPK || bGuidBattleMap || bPKServer || bPKMap || bCTFPlayer) )
					{
						bHpVisible = false;
					}
				}
			}

			if ( bHpVisible == false )
			{
				//	피표시만 막음 ( 맥시멈 )
				sHP.TO_FULL ();
			}

			//	이름 색 (PK_COLOR)
			//dwColor = pCHAR->GET_PK_OR_GM_COLOR();
			if( pCHAR->m_dwUserLvl < USER_USER_GM )
				dwColor = pCHAR->GET_PK_COLOR();
			else
				dwColor = NS_UITEXTCOLOR::GM_CHAR_NAME;

			if( pCHAR->GetCharData().m_Base.m_bGM )
			{
				dwColor = NS_UITEXTCOLOR::GM_CHAR_NAME;
			}

			float fPercent = float(sHP.dwNow) / float(sHP.dwMax);

			if ( !pCHAR->IsValidBody() )
			{
				fPercent = 0.0f;
			}

			fnSetTargetInfo( m_pTargetInfoDisplay, sTargetID, fPercent, CString( strName ) , dwColor );
		}
		break;

	case CROW_SUMMON:
		{
			std::tr1::shared_ptr<GLSummonClient> spSummon = m_pGaeaClient->GetSummon(sTargetID.GaeaId);
			if (!spSummon)
				return;

			std::tr1::shared_ptr<GLCharClient> pCHAR = m_pGaeaClient->GetChar(spSummon->m_dwOwnerGaeaID);
			if (!pCHAR)
				return;

			//dwColor = pCHAR->GET_PK_OR_GM_COLOR();
			if( pCHAR->m_dwUserLvl < USER_USER_GM )
				dwColor = pCHAR->GET_PK_COLOR();
			else
				dwColor = NS_UITEXTCOLOR::GM_CHAR_NAME;

			if( pCHAR->GetCharData().m_Base.m_bGM )
			{
				dwColor = NS_UITEXTCOLOR::GM_CHAR_NAME;
			}

			float fPercent = float(sHP.dwNow) / float(sHP.dwMax);

			if (!spSummon->IsValidBody())
				fPercent = 0.0f;

			fnSetTargetInfo( m_pTargetInfoDisplay, sTargetID, fPercent, CString( strName ), dwColor );
		}
		break;
	};


	//	NOTE
	//		포지션 설정
	{
		UiHideGroup( TARGETINFO_DISPLAY );

		// Note : 이름표시 높이조절
		D3DXVECTOR3 vPos = pCOPY->GetPosBodyHeight();
		D3DXVECTOR3 vScreen = DxViewPort::GetInstance().ComputeVec3Project( &vPos, NULL );

		bool bNameVisible = true;

		long lResolution = CUIMan::GetResolution();
		WORD X_RES = HIWORD(lResolution) - 30;
		WORD Y_RES = LOWORD(lResolution) - 30;
		if ( (vScreen.x<40) || (vScreen.y<40) || (X_RES<vScreen.x) || (Y_RES<vScreen.y) )
			bNameVisible = false;

		D3DXVECTOR3 LookAtPt = m_pGaeaClient->GetCharacter()->GetPosition() - DxViewPort::GetInstance().GetFromPt();
		LookAtPt.y = 0.0f;
		D3DXVECTOR3 TargetPos = pCOPY->GetPosBodyHeight() - DxViewPort::GetInstance().GetFromPt();
		TargetPos.y = 0.0f;
		float LookAtLength = D3DXVec3Length( &LookAtPt );
		float PosLength = D3DXVec3Length( &TargetPos );
		float CosTheta = D3DXVec3Dot( &LookAtPt, &TargetPos ) / ( LookAtLength * PosLength );
		if ( CosTheta < 0 )
			bNameVisible = false;

		if ( bNameVisible )
		{
			D3DXVECTOR2 vNamePos;
			const UIRECT& rcOriginPos = fnGetGlobalPos( m_pTargetInfoDisplay );
			vNamePos.x = floor( vScreen.x - ( rcOriginPos.sizeX * 0.5f ) + 0.5f );
			vNamePos.y = floor( vScreen.y - rcOriginPos.sizeY + 0.5f );
			if ( m_pTargetInfoDisplay )
			{
				if ( abs( rcOriginPos.left - vNamePos.x ) > 1 || abs( rcOriginPos.top - vNamePos.y ) > 1 )
					fnSetGlobalPos( m_pTargetInfoDisplay, vNamePos );
			}
			UiShowGroupBottom( TARGETINFO_DISPLAY );
		}

		//	중복 이름 지우기
		if ( m_pNameDisplayMan )	
			m_pNameDisplayMan->VisibleTargetInfoCtrl ( sTargetID.emCrow, sTargetID.GaeaId, FALSE );
	}
}

void OldInterface::AddChatMsg( const CString& strName, const D3DCOLOR& dwIDColor, const CString& strChat, const D3DCOLOR& dwChatColor, const D3DXVECTOR2& vPos )
{
	//m_pHeadChatDisplayMan->AddChat( strName, dwIDColor, strChat, dwChatColor, vPos );
}

void OldInterface::ClearNameList ()
{
	if ( m_pNameDisplayMan )
		m_pNameDisplayMan->RemoveAll();
}

void	OldInterface::UpdateStateSimpleHP ()
{
	if ( DXPARAMSET::GetInstance().m_bSHOW_SIMPLEHP )
	{
		if ( !UiIsVisibleGroup ( SIMPLE_HP ) )
			UiShowGroupBottom ( SIMPLE_HP );
	}
	else
	{
		if ( UiIsVisibleGroup ( SIMPLE_HP ) )
			UiHideGroup ( SIMPLE_HP );
	}

	if ( UiIsVisibleGroup ( SIMPLE_HP ) )
	{
		const long lResolution = uiman::GetInnerUIMan().GetResolution();
		WORD X_RES = HIWORD ( lResolution );
		WORD Y_RES = LOWORD ( lResolution );

		D3DXVECTOR3 vPos = m_pGaeaClient->GetCharacter()->GetPosition ();	

		//	NOTE
		//		포지션 설정
		{
			vPos.y -= 2.0f;
			D3DXVECTOR3 vScreen = DxViewPort::GetInstance().ComputeVec3Project ( &vPos, NULL );	

			const UIRECT& rcOriginPos = m_pSimpleHP->GetGlobalPos ();
			D3DXVECTOR2 vNewPos;
			vNewPos.x = (X_RES - rcOriginPos.sizeX) / 2.0f;
			vNewPos.y = ( vScreen.y - rcOriginPos.sizeY );

			if ( fabs ( rcOriginPos.left - vNewPos.x ) > 1.0f ||
				fabs ( rcOriginPos.top - vNewPos.y ) > 1.0f ) 

			m_pSimpleHP->SetGlobalPos ( vNewPos );			
		}
	}

}
