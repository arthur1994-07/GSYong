#include "stdafx.h"
#include "../../../SigmaCore/String/StringFormat.h"
#include "../../../RanLogic/RANPARAM.h"
#include "../../../RanLogic/HideSet/GLHideSet.h"

#include "../EngineLib/DxTools/DxViewPort.h"

#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicVarTextBox.h"

#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLogicDataMan.h"
#include "../../../RanLogic/GLUseFeatures.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Stage/DxGlobalStage.h"
#include "../../../RanLogicClient/SNS/GLSNSClient.h"
#include "../../../RanLogicClient/Club/GLClubClient.h"

#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLogicDataMan.h"
#include "../../../RanLogicClient/ReferChar/ReferChar.h"
#include "../../../RanLogicClient/Char/CharNameDbNumCache.h"
#include "../../../RanLogicClient/Tournament/GLTournamentClient.h"
#include "../../../Ranlogicclient/GLGaeaClient.h"

#include "../../InnerInterface.h"
#include "../Interface/BasicChat.h"
#include "../Confront/ConftModalWindow.h"
#include "../Party/PartyModalWindow.h"
#include "PtoPWindow.h"

#include "../GroupChat/ChatGroupWindow.h"
// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CPtoPWindow::CPtoPWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
	: CUIWindow(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
	, m_bSetThisFrame(FALSE)
	, m_pNameBox(NULL)
	, m_pInfo(NULL)
    , m_nOverMenu(NO_ID)
    , m_dwDefaultButtonCount(0)
    , m_pTradeButton(NULL)
    , m_pWhisperButton(NULL)
    , m_pPartyButton(NULL)
    , m_pConflictButton(NULL)
    , m_pFriendButton(NULL)
	, m_pGuildButton(NULL)
	, m_pUnionButton( NULL )
	, m_pGuildBattleButton( NULL )
	, m_pUnionBattleButton( NULL )
	, m_pBikeButton( NULL )
	, m_pBikeButtonPosPrev( NULL )
    , m_pBikeButtonPosNext( NULL )
    , m_pFaceBookButton ( NULL )
    , m_pTwitterButton ( NULL )
	, m_pReferCharButton( NULL )
    , m_pGroupChatButton( NULL )
	, m_CharacterHeight(0)
{
}

CPtoPWindow::~CPtoPWindow()
{
}

void CPtoPWindow::ResizeControl( char* szWindowKeyword )
{
    CUIControl TempControl(m_pEngineDevice);
	TempControl.Create( 1, szWindowKeyword );
	const UIRECT& rcParentOldPos = GetLocalPos();
	const UIRECT& rcParentNewPos = TempControl.GetLocalPos();
	AlignSubControl( rcParentOldPos, rcParentNewPos );

	SetLocalPos( D3DXVECTOR2 ( rcParentNewPos.left, rcParentNewPos.top ) );
}

void CPtoPWindow::SetEnableButton( const bool bEnable, CBasicButton* pButton )
{
    if ( !pButton )
        return;

    if ( bEnable )
    {
        pButton->SetDiffuse   ( cCOLOR_ENABLE );
        pButton->SetNoMessage ( FALSE );

    }
    else
    {
        pButton->SetDiffuse   ( cCOLOR_DISABLE );
        pButton->SetNoMessage ( TRUE );
    }
}

void CPtoPWindow::CreateSubControl()
{
	CreateTitle( "PTOPMENU_WINDOW_TITLE", "PTOPMENU_WINDOW_TITLE_LEFT", "PTOPMENU_WINDOW_TITLE_MID", "PTOPMENU_WINDOW_TITLE_RIGHT", "PTOPMENU_CHARNAME", "" );
//	CreateCloseButton( "PTOPMENU_WINDOW_CLOSE", "PTOPMENU_WINDOW_CLOSE_F", "PTOPMENU_WINDOW_CLOSE_OVER" );
	CreateBody( "PTOPMENU_WINDOW_BODY", "PTOPMENU_WINDOW_BODY_LEFT", "PTOPMENU_WINDOW_BODY_UP", "PTOPMENU_WINDOW_BODY_MAIN", "PTOPMENU_WINDOW_BODY_DOWN", "PTOPMENU_WINDOW_BODY_RIGHT" );
	ResizeControl( "PTOPMENU_WINDOW" );

    WORD wFlag = GetAlignFlag();
	wFlag &= ~UI_FLAG_XSIZE;
	wFlag &= ~UI_FLAG_YSIZE;
	SetAlignFlag( wFlag );

	m_pTradeButton    	= CreateFlipButton( "PTOPMENU_TRADE", "PTOPMENU_TRADE_F", "PTOPMENU_TRADE_OVER",	PTOPMENU_TRADE );
	m_pWhisperButton  	= CreateFlipButton( "PTOPMENU_WHISPER", "PTOPMENU_WHISPER_F", "PTOPMENU_WHISPER_OVER", PTOPMENU_WHISPER );
	m_pPartyButton    	= CreateFlipButton( "PTOPMENU_PARTY", "PTOPMENU_PARTY_F", "PTOPMENU_PARTY_OVER",	PTOPMENU_PARTY );
//	m_pExpeditionButton = CreateFlipButton( "PTOPMENU_EXPEDITION", "PTOPMENU_EXPEDITION_F", "PTOPMENU_EXPEDITION_OVER",	PTOPMENU_EXPEDITION );			// paran
	m_pConflictButton	= CreateFlipButton( "PTOPMENU_CONFLICT", "PTOPMENU_CONFLICT_F", "PTOPMENU_CONFLICT_OVER", PTOPMENU_CONFLICT );
	m_pFriendButton		= CreateFlipButton( "PTOPMENU_FRIEND", "PTOPMENU_FRIEND_F", "PTOPMENU_FRIEND_OVER", PTOPMENU_FRIEND );

    m_dwDefaultButtonCount += 5;

	m_pGuildButton = CreateFlipButton( "PTOPMENU_GUILD", "PTOPMENU_GUILD_F", "PTOPMENU_GUILD_OVER", PTOPMENU_GUILD );
	m_pGuildButton->SetVisibleSingle( FALSE );
	m_pUnionButton = CreateFlipButton( "PTOPMENU_UNION", "PTOPMENU_UNION_F", "PTOPMENU_UNION_OVER", PTOPMENU_UNION );
	m_pUnionButton->SetVisibleSingle( FALSE );
	m_pGuildBattleButton = CreateFlipButton( "PTOPMENU_GUILD_BATTLE", "PTOPMENU_GUILD_BATTLE_F", "PTOPMENU_GUILD_BATTLE_OVER", PTOPMENU_GUILD_BATTLE );
	m_pGuildBattleButton->SetVisibleSingle( FALSE );
	m_pUnionBattleButton = CreateFlipButton( "PTOPMENU_UNION_BATTLE", "PTOPMENU_UNION_BATTLE_F", "PTOPMENU_UNION_BATTLE_OVER", PTOPMENU_UNION_BATTLE );
	m_pUnionBattleButton->SetVisibleSingle( FALSE );

	m_pBikeButton = CreateFlipButton( "PTOPMENU_BIKE_BUTTON", "PTOPMENU_BIKE_BUTTON_F", "PTOPMENU_BIKE_BUTTON_OVER", PTOPMENU_BIKE_BUTTON );
	m_pBikeButton->SetVisibleSingle( FALSE );
	m_pBikeButtonPosPrev = CreateControl( "PTOPMENU_BIKE_BUTTON_PREV" );
	m_pBikeButtonPosPrev->SetVisibleSingle( FALSE );
	m_pBikeButtonPosNext = CreateControl( "PTOPMENU_BIKE_BUTTON_NEXT" );
	m_pBikeButtonPosPrev->SetVisibleSingle( FALSE );

    m_pFaceBookButton = CreateFlipButton( "PTOPMENU_FACEBOOK_BUTTON", "PTOPMENU_FACEBOOK_BUTTON_F", "PTOPMENU_FACEBOOK_BUTTON_OVER", PTOPMENU_FACEBOOK_BUTTON );
    m_pFaceBookButton->SetVisibleSingle( FALSE );
    m_pTwitterButton = CreateFlipButton( "PTOPMENU_TWITTER_BUTTON", "PTOPMENU_TWITTER_BUTTON_F", "PTOPMENU_TWITTER_BUTTON_OVER", PTOPMENU_TWITTER_BUTTON );
    m_pTwitterButton->SetVisibleSingle( FALSE );

	m_pReferCharButton = CreateFlipButton( "PTOPMENU_REFER_CHAR", "PTOPMENU_REFER_CHAR_F", "PTOPMENU_REFER_CHAR_OVER", PTOPMENU_REFER_CHAR );
	m_pReferCharButton->SetVisibleSingle( FALSE );

    m_pGroupChatButton = CreateFlipButton( "PTOPMENU_GROUP_CHAT", "PTOPMENU_GROUP_CHAT_F", "PTOPMENU_GROUP_CHAT_OVER", PTOPMENU_GROUPCHAT_BUTTON );
    m_pGroupChatButton->SetVisibleSingle( FALSE );

	CBasicVarTextBox* pInfo = new CBasicVarTextBox(m_pEngineDevice);
	pInfo->CreateSub( this, "BASIC_VAR_TEXT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pInfo->CreateSubControl();
	pInfo->SetVisibleSingle( FALSE );
	RegisterControl( pInfo );
	m_pInfo = pInfo;
}

CUIControl*	CPtoPWindow::CreateControl( const char* szControl )
{
	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub( this, szControl );
	RegisterControl( pControl );

	return pControl;
}

CBasicButton* CPtoPWindow::CreateFlipButton( char* szButton, char* szButtonFlip, char* szButtonOver, UIGUID ControlID )
{
	CBasicButton* pButton = new CBasicButton(m_pEngineDevice);
	pButton->CreateSub( this, szButton, UI_FLAG_DEFAULT, ControlID );
	pButton->CreateFlip( szButtonFlip, CBasicButton::CLICK_FLIP );
	pButton->CreateMouseOver( szButtonOver );
	RegisterControl( pButton );
	return pButton;
}

void CPtoPWindow::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case PTOPMENU_TRADE:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
			{
				if ( m_nOverMenu != ControlID )
				{
					m_pInfo->SetTextNoSplit( ID2GAMEWORD ("P2PMENU",0), NS_UITEXTCOLOR::SILVER );							
				}
				m_pInfo->SetVisibleSingle( TRUE );
				m_nOverMenu = ControlID;

				if ( dwMsg & UIMSG_LB_UP )
				{
					std::tr1::shared_ptr<GLCharClient> pCharClient = m_pGaeaClient->GetChar( m_dwGaeaID );
					if ( !pCharClient )
                        return;

					if ( m_pGaeaClient->GetCharacter()->IsPostBoxWindowOpen() )
					{
						m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("NO_TRADE_POSTBOX") );
						break;
					}

					if( m_pGaeaClient->GetCharacter()->VehicleState() || pCharClient->VehicleState() )
					{
						m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("NO_TRADE_VEHICLE") );
						break;
					}

					if ( m_pGaeaClient->GetCharacter()->m_sPMarket.IsOpen() )
					{
						m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("NO_TRADE_VEHICLE") );
					}
					else
					{
						GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
						if ( NULL == pCharacter )
						{
							return;
						}
						// 거리 체크
						float fDistance = D3DXVec3Length ( &D3DXVECTOR3( pCharacter->GetPosition() - pCharClient->GetPosition() ) );
						float fTradeRange = static_cast< float >( pCharClient->GETBODYRADIUS() + pCharacter->GETBODYRADIUS() + 30 );
						float fTradeableDistance = fTradeRange + 15.0f;
						if ( fDistance > fTradeableDistance ) 
						{
							return;
						}

						if ( GLUseFeatures::GetInstance().IsUsingWorldBattle() )
						{
							if ( m_pInterface )
							{
								m_pInterface->PrintMsgText(
									NS_UITEXTCOLOR::NEGATIVE,
									ID2GAMEINTEXT( "LIMITED_REQUEST_TRADE_MESSAGE" ) );
							}

							return;
						}
					
						GLMSG::SNET_TRADE NetMsg(m_dwGaeaID);
						NetMsg.dwChaNum = pCharacter->CharDbNum();
                        NetMsg.m_Crc32 = sc::string::getCrc32(&NetMsg, NetMsg.Size()-sizeof(unsigned int));
						m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
					}
				}
			}
		}
		break;

	case PTOPMENU_WHISPER:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
			{
				if ( m_nOverMenu != ControlID )
				{
					m_pInfo->SetTextNoSplit( ID2GAMEWORD ("P2PMENU",1), NS_UITEXTCOLOR::SILVER );	
				}
				m_pInfo->SetVisibleSingle( TRUE );
				m_nOverMenu = ControlID;

				if ( dwMsg & UIMSG_LB_UP )
				{
					m_pInterface->GetBasicChatWindow()->BeginPrivateChat( UI::ToString( m_szPlayerName ) );
					m_pInterface->ADD_FRIEND_NAME_TO_EDITBOX( m_szPlayerName );
				}
			}
		}
		break;

	case PTOPMENU_PARTY:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
			{
				if ( m_nOverMenu != ControlID )
				{
					m_pInfo->SetTextNoSplit( ID2GAMEWORD("P2PMENU",2), NS_UITEXTCOLOR::SILVER );	
				}
				m_pInfo->SetVisibleSingle( TRUE );
				m_nOverMenu = ControlID;

				if ( dwMsg & UIMSG_LB_UP )
				{
					std::string strTemp = sc::string::format( ID2GAMEINTEXT("PARTY_QUESTION"), m_szPlayerName.GetString() ).c_str();
					m_pInterface->DoPartyModal( strTemp, UI::MODAL_PARTY_REQ, EMPARTY_MODE_PARTY );
				}
			}
		}
		break;

	case PTOPMENU_CONFLICT:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
			{
				if ( m_nOverMenu != ControlID )
				{
					m_pInfo->SetTextNoSplit( ID2GAMEWORD("P2PMENU",3), NS_UITEXTCOLOR::SILVER );	
				}
				m_pInfo->SetVisibleSingle( TRUE );
				m_nOverMenu = ControlID;

				if ( dwMsg & UIMSG_LB_UP )
				{
					std::tr1::shared_ptr<GLCharClient> pCharClient = m_pGaeaClient->GetChar( m_dwGaeaID );
					if ( !pCharClient )
                        return;

					if( m_pGaeaClient->GetCharacter()->VehicleState() || pCharClient->VehicleState() )
					{
						m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("NO_CONFT_VEHICLE") );
						break;
					}

					if ( m_pGaeaClient->GetCharacter()->IsPostBoxWindowOpen() )
					{
						break;
					}

					GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
					CString strMyName = pCharacter->GetName();
					EMCONFT_TYPE emTYPE = pCharacter->GetConftType( m_dwGaeaID );
					UIGUID MODAL_CONFT_TYPE = NO_ID;

					std::string strCombine;
					switch ( emTYPE )
					{
					case EMCONFT_NONE:
						{
//							GASSERT ( 0 && "오류." );
							return ;
						}
						break;

					case EMCONFT_ONE:
						{
							strCombine = sc::string::format( ID2GAMEINTEXT("CONFLICT_QUESTION"), m_szPlayerName.GetString() );
							MODAL_CONFT_TYPE = UI::MODAL_CONFLICT_ONE_REQ;
						}
						break;

					case EMCONFT_PARTY:
						{
							strCombine = sc::string::format( ID2GAMEINTEXT("CONFLICT_QUESTION_PARTY"), m_szPlayerName.GetString() );
							MODAL_CONFT_TYPE = UI::MODAL_CONFLICT_PARTY_REQ;
						}
						break;

					case EMCONFT_GUILD:
						{
							strCombine = sc::string::format( ID2GAMEINTEXT("CONFLICT_QUESTION_GUILD"), m_szPlayerName.GetString() );
							MODAL_CONFT_TYPE = UI::MODAL_CONFLICT_GUILD_REQ;
						}
						break;
					}

					m_pInterface->DoConftModal( strCombine, MODAL_CONFT_TYPE, strMyName, m_szPlayerName );
				}
			}
		}
		break;

	case PTOPMENU_FRIEND:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
			{
				if ( m_nOverMenu != ControlID )
				{
					m_pInfo->SetTextNoSplit( ID2GAMEWORD ("P2PMENU",4), NS_UITEXTCOLOR::SILVER );	
				}
				m_pInfo->SetVisibleSingle( TRUE );
				m_nOverMenu = ControlID;

				if ( dwMsg & UIMSG_LB_UP )
				{
					m_pInterface->ADD_FRIEND ( m_szPlayerName );
				}
			}
		}
		break;

	case PTOPMENU_GUILD:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
			{
				if ( m_nOverMenu != ControlID )
				{
					m_pInfo->SetTextNoSplit( ID2GAMEWORD ("P2PMENU",5), NS_UITEXTCOLOR::SILVER );	
				}
				m_pInfo->SetVisibleSingle( TRUE );
				m_nOverMenu = ControlID;

				if ( dwMsg & UIMSG_LB_UP )
				{
					//	모달띄우기
					std::string strCombine = sc::string::format( ID2GAMEINTEXT("CLUB_JOIN"), m_szPlayerName );
					m_pInterface->DoModal( strCombine, UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_CLUB_JOIN );					
				}
			}
		}
		break;

	case PTOPMENU_UNION:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
			{
				if ( m_nOverMenu != ControlID )
				{
					m_pInfo->SetTextNoSplit( ID2GAMEWORD ("P2PMENU",6), NS_UITEXTCOLOR::SILVER );	
				}
				m_pInfo->SetVisibleSingle( TRUE );
				m_nOverMenu = ControlID;

				if ( dwMsg & UIMSG_LB_UP )
				{
					if ( GLUseFeatures::GetInstance().IsUsingWorldBattle() )
					{
						return;
					}

					//	모달띄우기
					std::string strCombine = sc::string::format( ID2GAMEINTEXT("UNION_JOIN"), m_szPlayerName );
					m_pInterface->DoModal( strCombine, UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_UNION_JOIN );					
				}
			}
		}
		break;

	case PTOPMENU_GUILD_BATTLE:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
			{
				if ( m_nOverMenu != ControlID )
				{
					m_pInfo->SetTextNoSplit( ID2GAMEWORD ("P2PMENU",7), NS_UITEXTCOLOR::SILVER );	
				}
				m_pInfo->SetVisibleSingle( TRUE );
				m_nOverMenu = ControlID;

				if ( dwMsg & UIMSG_LB_UP )
				{
					if ( GLUseFeatures::GetInstance().IsUsingWorldBattle() )
					{
						return;
					}

					std::tr1::shared_ptr<GLCharClient> pCharClient = m_pGaeaClient->GetChar( m_dwGaeaID );
					if ( !pCharClient )
                        return;
					//	모달띄우기
					m_pInterface->DoBattleModal( pCharClient->GetClubName() );
				}
			}
		}
		break;

	case PTOPMENU_UNION_BATTLE:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( m_nOverMenu != ControlID )
				{
					m_pInfo->SetTextNoSplit( ID2GAMEWORD ("P2PMENU",8), NS_UITEXTCOLOR::SILVER );	
				}
				m_pInfo->SetVisibleSingle( TRUE );
				m_nOverMenu = ControlID;

				if ( dwMsg & UIMSG_LB_UP )
				{
					std::tr1::shared_ptr<GLCharClient> pCharClient = m_pGaeaClient->GetChar( m_dwGaeaID );
					if ( !pCharClient )
                        return;
					//	모달띄우기
					m_pInterface->DoBattleModal( pCharClient->GetClubName(), true );
				}
			}
		}
		break;
	
	case PTOPMENU_BIKE_BUTTON:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
			{
				if ( m_nOverMenu != ControlID )
				{
					m_pInfo->SetTextNoSplit( ID2GAMEWORD ("P2PMENU",9), NS_UITEXTCOLOR::SILVER );	
				}
				m_pInfo->SetVisibleSingle( TRUE );
				m_nOverMenu = ControlID;

				if ( dwMsg & UIMSG_LB_UP )
				{
					if( m_pGaeaClient->GetCharacter()->VehicleState() != EMVEHICLE_DRIVER )
					{
						m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("BIKE_NOT_BOARDING") );
						return;
					}

					if( m_pGaeaClient->GetCharacter()->IsProducting() )
					{
						m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT( "PRODUCT_DONT_T_INVEN" ) ); 
						return;
					}

					INT nIndex = 0;
					for (INT i = 0; i < MAX_PASSENGER_COUNT; i++ )
					{
						if (m_pGaeaClient->GetCharacter()->VehiclePassenger(i) == GAEAID_NULL)
						{
							continue;
						}
						nIndex++;						
					}
					if( nIndex >= PASSENGER_COUNT )
					{
						m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("BIKE_PASSENGER_FULL") );
						return;
					}

					const SITEM* pVehicleItem = GLogicData::GetInstance().GetItem(m_pGaeaClient->GetCharacter()->VehicleId());
					if( !pVehicleItem )
					{
						return;
					}
					if ( pVehicleItem->sVehicle.bNotUseParts == TRUE )
					{
						if ( pVehicleItem->sVehicle.emVehicleType == VEHICLE_TYPE_BOARD )
						{
							m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("BIKE_NOT_SUBSEAT") );
							return;
						}
					}
					else
					{
						if (m_pGaeaClient->GetCharacter()->VehiclePutOnItem(ACCE_TYPE_PARTS_E) == NATIVEID_NULL())
						{
							m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("BIKE_NOT_SUBSEAT") );
							return;
						}
					}

					GLMSG::SNET_WITH_VEHICLE NetMsg;
					NetMsg.dwTargetID = m_dwGaeaID;
					m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
				}
			}
		}
		break;

    case PTOPMENU_FACEBOOK_BUTTON:
        {
            if ( CHECK_MOUSE_IN ( dwMsg ) )
            {
                if ( m_nOverMenu != ControlID )
                {
                    m_pInfo->SetTextNoSplit ( ID2GAMEINTEXT ("PTOPMENU_SNS_FACEBOOK_FRIEND_ADD"), NS_UITEXTCOLOR::SILVER );	
                }
                m_pInfo->SetVisibleSingle ( TRUE );
                m_nOverMenu = ControlID;

                if ( dwMsg & UIMSG_LB_UP )
                {
                    std::tr1::shared_ptr<GLCharClient> pCharClient = m_pGaeaClient->GetChar( m_dwGaeaID );
                    if ( !pCharClient )
                        break;

                    if ( pCharClient->m_byFB && m_pGaeaClient->GetSNSClient()->FBIsAuthed() )
                    {
                        std::string strCombine = ID2GAMEINTEXT( "MODAL_SNS_FACEBOOK_FRIEND_ADD" );
                        m_pInterface->ModalWindowSetModalData   ( (int)m_dwGaeaID, 0 );
                        m_pInterface->DoModal		            ( strCombine, UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_SNS_FACEBOOK_ADDFRIEND );
                    }
                }
            }
        }
        break;

    case PTOPMENU_TWITTER_BUTTON:
        {
            if ( CHECK_MOUSE_IN ( dwMsg ) )
            {
                if ( m_nOverMenu != ControlID )
                {
                    m_pInfo->SetTextNoSplit ( ID2GAMEINTEXT ("PTOPMENU_SNS_TWITTER_FOLLOW"), NS_UITEXTCOLOR::SILVER );	
                }
                m_pInfo->SetVisibleSingle ( TRUE );
                m_nOverMenu = ControlID;

                if ( dwMsg & UIMSG_LB_UP )
                {
                    std::tr1::shared_ptr<GLCharClient> pCharClient = m_pGaeaClient->GetChar( m_dwGaeaID );
                    if ( !pCharClient )
                        break;

                    if ( pCharClient->m_byTW && m_pGaeaClient->GetSNSClient()->TTIsAuthed() )
                    {
                        std::string strCombine = ID2GAMEINTEXT( "MODAL_SNS_TWITTER_FOLLOW" );
                        m_pInterface->ModalWindowSetModalData   ( (int)m_dwGaeaID, 0 );
                        m_pInterface->DoModal		            ( strCombine, UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_SNS_TWITTER_FOLLOW );
                        //GLSNSClient::GetInstance().FBFriendAdd( pCharClient ); 
                    }
                }
            }
        }
        break;

	case PTOPMENU_REFER_CHAR:
		{
			if( CHECK_MOUSE_IN( dwMsg ) )
			{
				if( m_nOverMenu != ControlID )
					m_pInfo->SetTextNoSplit( ID2GAMEWORD ( "P2PMENU", 10 ), NS_UITEXTCOLOR::SILVER );

				m_pInfo->SetVisibleSingle( TRUE );
				m_nOverMenu = ControlID;

				if( dwMsg & UIMSG_LB_UP )
				{
// 					ReferChar::CClient* pReferChar = m_pGaeaClient->GetReferChar();
// 					if( !pReferChar )
// 						return;
// 
// 					if( pReferChar->Request( m_pGaeaClient->GetConnServerGroup(), m_szPlayerName.GetString() ) )
// 						m_pInterface->UiShowGroupFocus( REFERCHAR_WINDOW );
                    m_pInterface->ReferCharWindowOpen( m_szPlayerName.GetString() );
				}
			}
		}
		break;

    case PTOPMENU_GROUPCHAT_BUTTON:
        {
            if ( CHECK_MOUSE_IN( dwMsg ) )
            {
                if ( m_nOverMenu != ControlID )
                {
                    m_pInfo->SetTextNoSplit( ID2GAMEWORD("P2PMENU",11), NS_UITEXTCOLOR::SILVER );
                }
				m_pInfo->SetVisibleSingle( TRUE );
				m_nOverMenu = ControlID;

				if ( dwMsg & UIMSG_LB_UP )
				{
					std::string strTemp = sc::string::format( ID2GAMEWORD("INVITE_GROUP_CHAT_WINDOW_INVITE_MSG") , m_szPlayerName.GetString() ).c_str();
					DWORD dwDbnum = m_pGaeaClient->FindChaDbNumAtCache(std::string(m_szPlayerName));
					m_pInterface->DoChatGroupModal( strTemp, UI::MODAL_CHAT_GROUP_REQ, EMPARTY_MODE_PARTY, dwDbnum );
				}
			}
        }
        break;
	}
}

void CPtoPWindow::SetPtoPMenu( std::tr1::shared_ptr<GLCharClient> pCharClient, DWORD dwGaeaID, GLClubClient* pMyClub )
{
	GASSERT( pCharClient ); 

	m_szPlayerName = pCharClient->GetName();
	m_CharacterHeight = pCharClient->GetBodyHeight();
	m_dwGaeaID = dwGaeaID;

	SetTitleName( m_szPlayerName ); 

	m_bSetThisFrame = FALSE;
	m_nOverMenu = NO_ID;

	DWORD dwCharID = m_pGaeaClient->GetCharacter()->m_CharDbNum;

	// 클럽에 가입킬 권한이 있는가?
	// 기존에 가입권한 유무에 따라 동맹, 클럽배틀, 동맹배틀 메뉴를 보여지도록 되어있었는데 기획팀 강홍근씨 요청에 따라 구분하지 않도록 변경
	// 추가적으로 가입권한 유무에 따라 버튼 UI 좌표 계산 방식이 다르게 되어있음.
	//if (pMyClub->HaveMemberJoinAuthority(dwCharID))
	{
        FLOAT fSizeX(0.0);
        DWORD dwCount = m_dwDefaultButtonCount;

		if ( !m_pGuildButton->IsVisible() )
		{
			m_pGuildButton->SetVisibleSingle( TRUE );
			const UIRECT& rcButtonGlobalPos = m_pGuildButton->GetGlobalPos();
			fSizeX += rcButtonGlobalPos.sizeX;
		}

        ++dwCount;

		// 동맹 메뉴
		{
			if (pMyClub->HaveAllianceAuthority(dwCharID) && pCharClient->IsClubMaster() )
			{
				if ( !pMyClub->IsAlliance() || pMyClub->IsChief() )
				{
					if( !m_pUnionButton->IsVisible() )
					{
						m_pUnionButton->SetVisibleSingle( TRUE );
						const UIRECT& rcButtonGlobalPos = m_pGuildButton->GetGlobalPos();
						fSizeX += rcButtonGlobalPos.sizeX;
					}

                    ++dwCount;
				}
			}
			else
			{
				if( m_pUnionButton->IsVisible() )
				{
					m_pUnionButton->SetVisibleSingle( FALSE );
					const UIRECT& rcButtonGlobalPos = m_pGuildButton->GetGlobalPos();
					fSizeX -= rcButtonGlobalPos.sizeX;
				}
			}
		}

		// 클럽 배틀 메뉴
		{
			if( GLCONST_CHAR::bCLUB_BATTLE && pMyClub->HaveClubBattleAuthority(dwCharID) && pCharClient->IsClubMaster() )
			{
				if( !m_pGuildBattleButton->IsVisible() )
				{
					m_pGuildBattleButton->SetVisibleSingle( TRUE );
					
					const UIRECT& rcButtonGlobalPos = m_pGuildBattleButton->GetGlobalPos();
					fSizeX += rcButtonGlobalPos.sizeX;					
				}

                ++dwCount;

				const UIRECT& rcUnionLocalPos = m_pUnionButton->GetLocalPos();
				UIRECT rcBattleLocalPos = m_pGuildBattleButton->GetLocalPos();					

				if ( !m_pUnionButton->IsVisible() )
				{
					rcBattleLocalPos.left = rcUnionLocalPos.left;
					rcBattleLocalPos.right = rcUnionLocalPos.right;
					m_pGuildBattleButton->SetLocalPos( rcBattleLocalPos );
				}
				else
				{
					rcBattleLocalPos.left = rcUnionLocalPos.left + rcUnionLocalPos.sizeX;
					rcBattleLocalPos.right = rcBattleLocalPos.left + rcUnionLocalPos.sizeX;
					m_pGuildBattleButton->SetLocalPos( rcBattleLocalPos );
				}
			}
			else
			{
				if( m_pGuildBattleButton->IsVisible() )
				{
					m_pGuildBattleButton->SetVisibleSingle( FALSE );
					
					const UIRECT& rcButtonGlobalPos = m_pUnionButton->GetGlobalPos();
					fSizeX -= rcButtonGlobalPos.sizeX;					
				}
			}
		}
		
		//	동맹 배틀 메뉴
		{
			if ( GLCONST_CHAR::bCLUB_BATTLE_ALLIANCE 
				&& pMyClub->IsChief() && pCharClient->IsAllianceMaster() ) 
			{
				if( !m_pUnionBattleButton->IsVisible() )
				{
					m_pUnionBattleButton->SetVisibleSingle( TRUE );
					
					const UIRECT& rcButtonGlobalPos = m_pUnionBattleButton->GetGlobalPos();
					fSizeX += rcButtonGlobalPos.sizeX;					
				}

                ++dwCount;

				const UIRECT& rcLeftLocalPos = m_pGuildBattleButton->GetLocalPos();
				UIRECT rcBattleLocalPos = m_pUnionBattleButton->GetLocalPos();					

				rcBattleLocalPos.left = rcLeftLocalPos.left + rcLeftLocalPos.sizeX;
				rcBattleLocalPos.right = rcBattleLocalPos.left + rcLeftLocalPos.sizeX;
				m_pUnionBattleButton->SetLocalPos( rcBattleLocalPos );

			}
			else
			{
				if( m_pUnionBattleButton->IsVisible() )
				{
					m_pUnionBattleButton->SetVisibleSingle( FALSE );
					
					const UIRECT& rcButtonGlobalPos = m_pUnionButton->GetGlobalPos();
					fSizeX -= rcButtonGlobalPos.sizeX;					
				}
			}
		}

        const UIRECT& rcGlobalPosOld = GetGlobalPos();
        const UIRECT& rcLocalPosOld  = GetLocalPos();
		
		{
			if( m_pGaeaClient->GetCharacter()->VehicleState() == EMVEHICLE_DRIVER )
			{
				if( !m_pBikeButton->IsVisible() )
				{
					m_pBikeButton->SetVisibleSingle( TRUE );

					const UIRECT& rcButtonGlobalPos = m_pBikeButton->GetGlobalPos();
					fSizeX += rcButtonGlobalPos.sizeX;	
                }

                UIRECT rcTempLocalPos = m_pBikeButton->GetLocalPos();
                rcTempLocalPos.left   = (FLOAT)(dwCount*dwBUTTON_SIZE_X) + 4;
                rcTempLocalPos.right  = rcGlobalPosOld.sizeX + rcTempLocalPos.left;

                m_pBikeButton->SetLocalPos( rcTempLocalPos );

                ++dwCount;

				/*if ( !m_pUnionButton->IsVisible() )
				{
					const UIRECT& rcUnionLocalPos = m_pUnionButton->GetLocalPos();
					UIRECT rcBattleLocalPos = m_pGuildBattleButton->GetLocalPos();			

					rcBattleLocalPos.left = rcUnionLocalPos.left;
					rcBattleLocalPos.right = rcUnionLocalPos.right;
					m_pBikeButton->SetLocalPos( rcBattleLocalPos );
				}
				else if( !m_pGuildBattleButton->IsVisible() )
				{
					const UIRECT& rcUnionLocalPos = m_pUnionButton->GetLocalPos();
					UIRECT rcBattleLocalPos = m_pGuildBattleButton->GetLocalPos();

					rcBattleLocalPos.left = rcUnionLocalPos.left + rcUnionLocalPos.sizeX;
					rcBattleLocalPos.right = rcBattleLocalPos.left + rcUnionLocalPos.sizeX;
					m_pBikeButton->SetLocalPos( rcBattleLocalPos );
				}
				else if( !m_pUnionBattleButton->IsVisible() )
				{
					const UIRECT& rcLeftLocalPos = m_pGuildBattleButton->GetLocalPos();
					UIRECT rcBattleLocalPos = m_pUnionBattleButton->GetLocalPos();

					rcBattleLocalPos.left = rcLeftLocalPos.left + rcLeftLocalPos.sizeX;
					rcBattleLocalPos.right = rcBattleLocalPos.left + rcLeftLocalPos.sizeX;
					m_pBikeButton->SetLocalPos( rcBattleLocalPos );
				}
				else
				{
					const UIRECT& rcUnionLocalPos = m_pUnionButton->GetLocalPos();
					UIRECT rcBattleLocalPos = m_pBikeButton->GetLocalPos();			

					rcBattleLocalPos.left = rcUnionLocalPos.left + rcUnionLocalPos.sizeX;
					rcBattleLocalPos.right = rcBattleLocalPos.left + rcUnionLocalPos.sizeX;
					m_pBikeButton->SetLocalPos( rcBattleLocalPos );
				}*/
			}
			else
			{
				if( m_pBikeButton->IsVisible() )
				{
					m_pBikeButton->SetVisibleSingle( FALSE );

					const UIRECT& rcButtonGlobalPos = m_pBikeButton->GetGlobalPos();
					fSizeX -= rcButtonGlobalPos.sizeX;					
				}
			}
		}

        // Note : FaceBook 버튼
        if ( pCharClient && pCharClient->m_byFB )
        {
            if ( !m_pFaceBookButton->IsVisible() )
            {
                m_pFaceBookButton->SetVisibleSingle( TRUE );

                const UIRECT& rcButtonGlobalPos = m_pFaceBookButton->GetGlobalPos ();
                fSizeX += rcButtonGlobalPos.sizeX;
            }

            UIRECT rcTempLocalPos = m_pFaceBookButton->GetLocalPos();
            rcTempLocalPos.left   = (FLOAT)(dwCount*dwBUTTON_SIZE_X) + 4;
            rcTempLocalPos.right  = rcGlobalPosOld.sizeX + rcTempLocalPos.left;

            m_pFaceBookButton->SetLocalPos( rcTempLocalPos );

            ++dwCount;

            // Note : 나의 사용 여부
            if ( m_pGaeaClient->GetSNSClient()->FBIsAuthed() )
            {
                SetEnableButton( true, m_pFaceBookButton );
            }
            else
            {
                SetEnableButton( false, m_pFaceBookButton );
            }
        }
        // 상대방이 사용중이지 않음
        else
        {
            if( m_pFaceBookButton->IsVisible() )
            {
                m_pFaceBookButton->SetVisibleSingle( FALSE );

                const UIRECT& rcButtonGlobalPos = m_pFaceBookButton->GetGlobalPos ();
                fSizeX -= rcButtonGlobalPos.sizeX;					
            }
        }

        // Note : Twitter 버튼
        if ( pCharClient && pCharClient->m_byTW )
        {
            if ( !m_pTwitterButton->IsVisible() )
            {
                m_pTwitterButton->SetVisibleSingle( TRUE );

                const UIRECT& rcButtonGlobalPos = m_pTwitterButton->GetGlobalPos ();
                fSizeX += rcButtonGlobalPos.sizeX;
            }

            UIRECT rcTempLocalPos = m_pTwitterButton->GetLocalPos();
            rcTempLocalPos.left   = (FLOAT)(dwCount*dwBUTTON_SIZE_X) + 4;
            rcTempLocalPos.right  = rcGlobalPosOld.sizeX + rcTempLocalPos.left;

            m_pTwitterButton->SetLocalPos( rcTempLocalPos );

            ++dwCount;

            // Note : 나의 사용 여부
            if ( m_pGaeaClient->GetSNSClient()->TTIsAuthed() )
            {
                SetEnableButton( true, m_pTwitterButton );
            }
            else
            {
                SetEnableButton( false, m_pTwitterButton );
            }
        }
        // 상대방이 사용중이지 않음
        else
        {
            if( m_pTwitterButton->IsVisible() )
            {
                m_pTwitterButton->SetVisibleSingle( FALSE );

                const UIRECT& rcButtonGlobalPos = m_pTwitterButton->GetGlobalPos ();
                fSizeX -= rcButtonGlobalPos.sizeX;					
            }
        }

        // 그룹채팅
        if( GLUseFeatures::GetInstance().IsUsingGroupChat() )
        {
            if( !m_pGroupChatButton->IsVisible() )
            {
                m_pGroupChatButton->SetVisibleSingle( TRUE );

                const UIRECT& rcButtonGlobalPos = m_pGroupChatButton->GetGlobalPos ();
                fSizeX += rcButtonGlobalPos.sizeX;
            }

            UIRECT rcTempLocalPos = m_pGroupChatButton->GetLocalPos();
            rcTempLocalPos.left   = (FLOAT)(dwCount*dwBUTTON_SIZE_X) + 4;
            rcTempLocalPos.right  = rcGlobalPosOld.sizeX + rcTempLocalPos.left;

            m_pGroupChatButton->SetLocalPos( rcTempLocalPos );

            ++dwCount;
        }
        else
        {
            if( m_pGroupChatButton->IsVisible() )
            {
                m_pGroupChatButton->SetVisibleSingle( FALSE );

                const UIRECT& rcButtonGlobalPos = m_pGroupChatButton->GetGlobalPos ();
                fSizeX -= rcButtonGlobalPos.sizeX;					
            }
        }

		// 캐릭터 정보보기
		if( GLUseFeatures::GetInstance().IsUsingReferChar() )
		{
			if( !m_pReferCharButton->IsVisible() )
			{
				m_pReferCharButton->SetVisibleSingle( TRUE );

				const UIRECT& rcButtonGlobalPos = m_pReferCharButton->GetGlobalPos ();
				fSizeX += rcButtonGlobalPos.sizeX;
			}

			UIRECT rcTempLocalPos = m_pReferCharButton->GetLocalPos();
			rcTempLocalPos.left   = (FLOAT)(dwCount*dwBUTTON_SIZE_X) + 4;
			rcTempLocalPos.right  = rcGlobalPosOld.sizeX + rcTempLocalPos.left;

			m_pReferCharButton->SetLocalPos( rcTempLocalPos );

			++dwCount;
		}
		else
		{
			if( m_pReferCharButton->IsVisible() )
			{
				m_pReferCharButton->SetVisibleSingle( FALSE );

				const UIRECT& rcButtonGlobalPos = m_pReferCharButton->GetGlobalPos ();
				fSizeX -= rcButtonGlobalPos.sizeX;					
			}
		}

		WORD wFlagBack = GetAlignFlag ();
		SetAlignFlag ( UI_FLAG_XSIZE );

		UIRECT rcGlobalPosNew = UIRECT( rcGlobalPosOld.left, rcGlobalPosOld.top,
										rcGlobalPosOld.sizeX + fSizeX, rcGlobalPosOld.sizeY );

		AlignSubControl( rcGlobalPosOld, rcGlobalPosNew );

		SetLocalPos( D3DXVECTOR2( rcLocalPosOld.left, rcLocalPosOld.top ) );
		SetGlobalPos( rcGlobalPosNew);

		SetAlignFlag( wFlagBack );
	}
	/*
	else
	{
        const UIRECT& rcGlobalPosOld = GetGlobalPos ();
        const UIRECT& rcLocalPosOld  = GetLocalPos ();

        FLOAT fSizeX(0.0);
        DWORD dwCount = m_dwDefaultButtonCount;

		if ( m_pGuildButton->IsVisible() )
		{
			m_pGuildButton->SetVisibleSingle( FALSE );
			const UIRECT& rcButtonGlobalPos = m_pGuildButton->GetGlobalPos();
			fSizeX += rcButtonGlobalPos.sizeX;
		}

		if ( m_pUnionButton->IsVisible() )
		{
			m_pUnionButton->SetVisibleSingle( FALSE );
			const UIRECT& rcButtonGlobalPos = m_pUnionButton->GetGlobalPos();
			fSizeX += rcButtonGlobalPos.sizeX;
		}

		if ( m_pGuildBattleButton->IsVisible() )
		{
			m_pGuildBattleButton->SetVisibleSingle( FALSE );
			const UIRECT& rcButtonGlobalPos = m_pGuildBattleButton->GetGlobalPos();
			fSizeX += rcButtonGlobalPos.sizeX;
		}

		if ( m_pUnionBattleButton->IsVisible() )
		{
			m_pUnionBattleButton->SetVisibleSingle( FALSE );
			const UIRECT& rcButtonGlobalPos = m_pUnionBattleButton->GetGlobalPos();
			fSizeX += rcButtonGlobalPos.sizeX;
		}

		if( m_pGaeaClient->GetCharacter()->VehicleState() == EMVEHICLE_DRIVER )
		{
			if( !m_pBikeButton->IsVisible() )
			{
				m_pBikeButton->SetVisibleSingle( TRUE );

				const UIRECT& rcButtonGlobalPos = m_pBikeButton->GetGlobalPos();
				fSizeX -= rcButtonGlobalPos.sizeX;

				UIRECT rcBikeLocalPos = m_pBikeButtonPosPrev->GetLocalPos();					
				m_pBikeButton->SetLocalPos( rcBikeLocalPos );
			}

            ++dwCount;
		}
		else
		{
			if( m_pBikeButton->IsVisible() )
			{
				m_pBikeButton->SetVisibleSingle( FALSE );

				const UIRECT& rcButtonGlobalPos = m_pBikeButton->GetGlobalPos();
				fSizeX += rcButtonGlobalPos.sizeX;					
			}
		}

		// Note : FaceBook 버튼
        if ( pCharClient && pCharClient->m_byFB )
        {
            if ( !m_pFaceBookButton->IsVisible() )
            {
                m_pFaceBookButton->SetVisibleSingle( TRUE );

                const UIRECT& rcButtonGlobalPos = m_pFaceBookButton->GetGlobalPos ();
                fSizeX -= rcButtonGlobalPos.sizeX;
            }

            UIRECT rcTempLocalPos = m_pFaceBookButton->GetLocalPos();
            rcTempLocalPos.left   = (FLOAT)(dwCount*dwBUTTON_SIZE_X) + 4;
            rcTempLocalPos.right  = rcTempLocalPos.sizeX + rcTempLocalPos.left;

            m_pFaceBookButton->SetLocalPos( rcTempLocalPos );

            ++dwCount;

            // Note : 나의 사용 여부
            if ( m_pGaeaClient->GetSNSClient()->FBIsAuthed() )
            {
                SetEnableButton( true, m_pFaceBookButton );
            }
            else
            {
                SetEnableButton( false, m_pFaceBookButton );
            }
        }
        // 상대방이 사용중이지 않음
        else
        {
            if( m_pFaceBookButton->IsVisible() )
            {
                m_pFaceBookButton->SetVisibleSingle( FALSE );

                const UIRECT& rcButtonGlobalPos = m_pFaceBookButton->GetGlobalPos ();
                fSizeX += rcButtonGlobalPos.sizeX;					
            }
        }

        // Note : Twitter 버튼
        if ( pCharClient && pCharClient->m_byTW )
        {
            if ( !m_pTwitterButton->IsVisible() )
            {
                m_pTwitterButton->SetVisibleSingle( TRUE );

                const UIRECT& rcButtonGlobalPos = m_pTwitterButton->GetGlobalPos ();
                fSizeX -= rcButtonGlobalPos.sizeX;
            }

            UIRECT rcTempLocalPos = m_pTwitterButton->GetLocalPos();
            rcTempLocalPos.left   = (FLOAT)(dwCount*dwBUTTON_SIZE_X) + 4;
            rcTempLocalPos.right  = rcTempLocalPos.sizeX + rcTempLocalPos.left;

            m_pTwitterButton->SetLocalPos( rcTempLocalPos );

            ++dwCount;

            // Note : 나의 사용 여부
            if ( m_pGaeaClient->GetSNSClient()->TTIsAuthed() )
            {
                SetEnableButton( true, m_pTwitterButton );
            }
            else
            {
                SetEnableButton( false, m_pTwitterButton );
            }
        }
        // 상대방이 사용중이지 않음
        else
        {
            if( m_pTwitterButton->IsVisible() )
            {
                m_pTwitterButton->SetVisibleSingle( FALSE );

                const UIRECT& rcButtonGlobalPos = m_pTwitterButton->GetGlobalPos ();
                fSizeX += rcButtonGlobalPos.sizeX;					
            }
        }

        // 그룹채팅
        if( GLUseFeatures::GetInstance().IsUsingGroupChat() )
        {
            if( !m_pGroupChatButton->IsVisible() )
            {
                m_pGroupChatButton->SetVisibleSingle( TRUE );

                const UIRECT& rcButtonGlobalPos = m_pGroupChatButton->GetGlobalPos ();
                fSizeX -= rcButtonGlobalPos.sizeX;
            }

            UIRECT rcTempLocalPos = m_pGroupChatButton->GetLocalPos();
            rcTempLocalPos.left   = (FLOAT)(dwCount*dwBUTTON_SIZE_X) + 4;
            rcTempLocalPos.right  = rcTempLocalPos.sizeX + rcTempLocalPos.left;

            m_pGroupChatButton->SetLocalPos( rcTempLocalPos );

            ++dwCount;
        }
        else
        {
            if( m_pGroupChatButton->IsVisible() )
            {
                m_pGroupChatButton->SetVisibleSingle( FALSE );

                const UIRECT& rcButtonGlobalPos = m_pGroupChatButton->GetGlobalPos ();
                fSizeX += rcButtonGlobalPos.sizeX;					
            }
        }

		// 캐릭터 정보보기
		if( GLUseFeatures::GetInstance().IsUsingReferChar() )
		{
			if( !m_pReferCharButton->IsVisible() )
			{
				m_pReferCharButton->SetVisibleSingle( TRUE );

				const UIRECT& rcButtonGlobalPos = m_pReferCharButton->GetGlobalPos ();
				fSizeX -= rcButtonGlobalPos.sizeX;
			}

			UIRECT rcTempLocalPos = m_pReferCharButton->GetLocalPos();
			rcTempLocalPos.left   = (FLOAT)(dwCount*dwBUTTON_SIZE_X) + 4;
			rcTempLocalPos.right  = rcTempLocalPos.sizeX + rcTempLocalPos.left;

			m_pReferCharButton->SetLocalPos( rcTempLocalPos );

			++dwCount;
		}
		else
		{
			if( m_pReferCharButton->IsVisible() )
			{
				m_pReferCharButton->SetVisibleSingle( FALSE );

				const UIRECT& rcButtonGlobalPos = m_pReferCharButton->GetGlobalPos ();
				fSizeX += rcButtonGlobalPos.sizeX;					
			}
		}

		WORD wFlagBack = GetAlignFlag ();
		SetAlignFlag ( UI_FLAG_XSIZE );

		UIRECT rcGlobalPosNew = UIRECT( rcGlobalPosOld.left, rcGlobalPosOld.top,
										rcGlobalPosOld.sizeX - fSizeX, rcGlobalPosOld.sizeY );

		AlignSubControl( rcGlobalPosOld, rcGlobalPosNew );

		SetLocalPos( D3DXVECTOR2 ( rcLocalPosOld.left, rcLocalPosOld.top ) );
		SetGlobalPos( rcGlobalPosNew);

		SetAlignFlag( wFlagBack );
	}
	*/

    //! CTF 플레이 중이라면
    if ( m_pGaeaClient->IsCTFPlayer() )
    {
        GLCharacter* pChar = m_pGaeaClient->GetCharacter();

        SetEnableButton( false, m_pTradeButton );
        SetEnableButton( false, m_pConflictButton );
        SetEnableButton( false, m_pGuildButton );
        SetEnableButton( false, m_pUnionButton );
        SetEnableButton( false, m_pGuildBattleButton );
        SetEnableButton( false, m_pUnionBattleButton );

        if ( pChar->m_wSchool != pCharClient->GetCharSchool() )
        {
            SetEnableButton( false, m_pPartyButton );
            SetEnableButton( false, m_pBikeButton );
        }
        else
        {
            SetEnableButton( true, m_pPartyButton );
            SetEnableButton( true, m_pBikeButton );
        }
    }
    else
    {
        SetEnableButton( true, m_pTradeButton );
        SetEnableButton( true, m_pPartyButton );
        SetEnableButton( true, m_pConflictButton );
        SetEnableButton( true, m_pGuildButton );
        SetEnableButton( true, m_pUnionButton );
        SetEnableButton( true, m_pGuildBattleButton );
        SetEnableButton( true, m_pUnionBattleButton );
        SetEnableButton( true, m_pBikeButton );
    }

	if( GLUseFeatures::GetInstance().IsUsingReferChar() )
		SetEnableButton( true, m_pReferCharButton );
	else
		SetEnableButton( false, m_pReferCharButton );
}

void CPtoPWindow::showGroupChat()
{
    const UIRECT& rcGlobalPosOld = GetGlobalPos ();
    const UIRECT& rcLocalPosOld  = GetLocalPos ();

    UIRECT rcTempLocalPos = m_pGroupChatButton->GetLocalPos();
    rcTempLocalPos.left   = (FLOAT)(m_dwDefaultButtonCount*dwBUTTON_SIZE_X) + 4;
    rcTempLocalPos.right  = rcGlobalPosOld.sizeX + rcTempLocalPos.left;
    m_pGroupChatButton->SetLocalPos( rcTempLocalPos );

    m_pGroupChatButton->SetVisibleSingle( TRUE );
}

void CPtoPWindow::SetVisibleSingle(BOOL visible)
{

	if( m_pGaeaClient->GetTournamentClient()->GetIsTournamentView() == true)
	{
		if ( visible == TRUE)
		{
			SetVisibleSingle(FALSE);
			return;
		}
	}

	CUIGroup::SetVisibleSingle(visible);
}

void CPtoPWindow::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	// 토너먼트중 파티를 할수가 없다.
	// TOURNAMENT_UNIQ_DISABLE
	if( m_pGaeaClient->GetTournamentClient()->GetIsTournamentView() == true)
	{
		if ( IsVisible() == TRUE)
		{
			SetVisibleSingle(FALSE);
			return;
		}
	}

	m_pInfo->SetVisibleSingle( FALSE );

	CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	DWORD dwMsg = GetMessageEx();
	if ( CHECK_LB_UP_LIKE( dwMsg ) )
	{
		//if ( m_bSetThisFrame )
		{
			m_pInterface->UiHideGroup( GetWndID () );
		}
		m_bSetThisFrame = TRUE;
	}

	//////////////////////////////////////////////////////////////////////////
	
	D3DXVECTOR3 vPos = *m_pGaeaClient->FindCharPos( m_szPlayerName.GetString() );
	vPos.y += m_CharacterHeight;

	D3DXVECTOR3 vScreen = DxViewPort::GetInstance().ComputeVec3Project( &vPos, NULL );

	const UIRECT& rcOriginPos = GetGlobalPos();
	int nPosX = (int)( vScreen.x - ( rcOriginPos.sizeX * 0.5f ));
	int nPosY = (int)( vScreen.y - rcOriginPos.sizeY );
	SetGlobalPos( D3DXVECTOR2((float)nPosX,(float)nPosY) );
}

CBasicTextBox* CPtoPWindow::CreateStaticControl(const char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR dwColor, int nAlign )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub( this, szControlKeyword );
	pStaticText->SetFont( pFont );
	pStaticText->SetTextAlign( nAlign );
	RegisterControl( pStaticText );
	return pStaticText;
}

MyPtoPWindow::MyPtoPWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CPtoPWindow(pGaeaClient, pInterface, pEngineDevice)
{
}

void MyPtoPWindow::CreateUIWindowAndRegisterOwnership()
{
    CPtoPWindow::Create( PTOPMENU_WINDOW, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    CPtoPWindow::CreateSubControl();
    CPtoPWindow::ResetAlignFlag();
    CPtoPWindow::m_pInterface->UiRegisterControl( this );
    CPtoPWindow::m_pInterface->UiShowGroupFocus( PTOPMENU_WINDOW );
}

const UIRECT& MyPtoPWindow::GetUIWindowGlobalPos()
{
    return CPtoPWindow::GetGlobalPos();
}

void MyPtoPWindow::SetUIWindowGlobalPos( const D3DXVECTOR2& vPos )
{
    CPtoPWindow::SetGlobalPos( vPos );
}

void MyPtoPWindow::SetPtoPMenu( std::tr1::shared_ptr<GLCharClient> pCharClient, DWORD dwGaeaID, GLClubClient* pMyClub )
{
    CPtoPWindow::SetPtoPMenu( pCharClient, dwGaeaID, pMyClub );
}

DWORD MyPtoPWindow::GetPlayerID()
{
    return CPtoPWindow::GetPlayerID();
}

void MyPtoPWindow::SetPlayerID(DWORD dwGaeaID)
{
	CPtoPWindow::SetPlayerID(dwGaeaID);
}

void MyPtoPWindow::showGroupChat()
{
    CPtoPWindow::showGroupChat();
}
