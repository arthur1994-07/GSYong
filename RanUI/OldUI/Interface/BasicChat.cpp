#include "StdAfx.h"
#include "BasicChat.h"
#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "BasicChatLeftBar.h"
#include "BasicChatRightBody.h"
#include "ChatShowFlag.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"

#include "../../StaticUIManager.h"
#include "../../InnerInterface.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CBasicChat::CBasicChat(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
	, m_pBasicChatLeftBar( NULL )
	, m_pBasicChatRightBody( NULL )
{
}

CBasicChat::~CBasicChat()
{
}

void CBasicChat::CreateSubControl()
{
	CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	CBasicChatLeftBar* pBasicChatLeftBar = new CBasicChatLeftBar(m_pEngineDevice);
	pBasicChatLeftBar->CreateSub( this, "CHAT_LEFT_BAR", UI_FLAG_YSIZE, LEFT_BAR );
	pBasicChatLeftBar->CreateSubControl();
	pBasicChatLeftBar->SetProtectSizePoint();
	RegisterControl( pBasicChatLeftBar );
	m_pBasicChatLeftBar = pBasicChatLeftBar;

	CBasicChatRightBody* pBasicChatRightBody = new CBasicChatRightBody(m_pGaeaClient, m_pInterface, m_pEngineDevice);
    pBasicChatRightBody->CreateSub( this, "CHAT_RIGHT_BODY", UI_FLAG_YSIZE, RIGHT_BODY );
	pBasicChatRightBody->CreateSubControl( pFont9 );
	pBasicChatRightBody->SetProtectSizePoint();
	RegisterControl( pBasicChatRightBody );
	m_pBasicChatRightBody = pBasicChatRightBody;

	CBasicButton* pButton = new CBasicButton(m_pEngineDevice);
	pButton->CreateSub( this, "CHAT_FLAG_BUTTON", UI_FLAG_BOTTOM, CHAT_FLAG_BUTTON );
	pButton->CreateFlip( "CHAT_FLAG_BUTTON_F", CBasicButton::CLICK_FLIP );
	pButton->CreateMouseOver( "CHAT_FLAG_BUTTON_F" );
	pButton->SetUseGlobalAction( TRUE );
	RegisterControl( pButton );
	m_pChatShowFlagButton = pButton;
}

void CBasicChat::AddChat( const CString& strName, const CString& strMsg, const WORD& wType )
{
	if ( m_pBasicChatRightBody )
		m_pBasicChatRightBody->AddChat( strName, strMsg, wType );
}

void CBasicChat::AddStringToNormal( CString strTemp, D3DCOLOR dwColor )
{
	if ( m_pBasicChatRightBody )
		m_pBasicChatRightBody->AddStringToNORMALEx( strTemp, dwColor );
}

void CBasicChat::AddStringToChatEx( CString strTemp, WORD wType, bool bGM )
{
	if ( m_pBasicChatRightBody )
		m_pBasicChatRightBody->AddStringToChatEx( strTemp, wType, bGM );
}

void CBasicChat::AddStringToChatLink( CString strTemp, WORD wType, VEC_LINK_TEXT_DATA& vecLinkTextData, bool bGM )
{
	if ( m_pBasicChatRightBody )
		m_pBasicChatRightBody->AddStringToChatLink( strTemp, wType, vecLinkTextData, bGM );
}

void CBasicChat::AddStringToSystemMessage( CString strTemp, D3DCOLOR dwColor )
{
	if ( m_pBasicChatRightBody )
		m_pBasicChatRightBody->AddStringToSystemMessage( strTemp, dwColor );
}

void CBasicChat::AddChatMacro(CString strMsg )
{
	if ( m_pBasicChatRightBody )
		m_pBasicChatRightBody->AddChatMacro(strMsg);
}

void CBasicChat::ChatLog( bool bChatLog, int nChatLogType )
{
	if ( m_pBasicChatRightBody )
		m_pBasicChatRightBody->ChatLog( bChatLog, nChatLogType );
}
CString CBasicChat::GET_RECORD_CHAT()
{
	if ( !m_pBasicChatRightBody ) return NULL;

	return m_pBasicChatRightBody->GET_RECORD_CHAT();
}

void CBasicChat::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	m_PosX = x;
	m_PosY = y;

	CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( IsExclusiveSelfControl() )
	{
		const float fTOP = y + m_vGap.y;
		if ( fTOP <= 0.0f ) return ;

		const UIRECT& rcGlobalOldPos = GetGlobalPos();

		UIRECT rcGlobalNewPos = rcGlobalOldPos;
		rcGlobalNewPos.top = fTOP;
		rcGlobalNewPos.sizeY = rcGlobalOldPos.bottom - fTOP;

		const D3DXVECTOR2& vProtectSize = GetProtectSize();
		if( rcGlobalNewPos.sizeY <= vProtectSize.y ) return;

		WORD wFlag = GetAlignFlag();
		SetAlignFlag( UI_FLAG_YSIZE );

		AlignSubControl( rcGlobalOldPos, rcGlobalNewPos );

		SetGlobalPos( rcGlobalNewPos );

		SetAlignFlag( wFlag );
	}

	DWORD dwChatType = m_pBasicChatRightBody->GetCHATTYPE();
	
	int nDisplayType = m_pBasicChatRightBody->GetDISPLAYTYPE();
	switch ( nDisplayType )
	{
	case CBasicChatRightBody::CHAT_CHANNEL_TOTAL:		m_pChatShowFlagButton->SetVisibleSingle( TRUE ); break;
	case CBasicChatRightBody::CHAT_CHANNEL_PARTY:
	case CBasicChatRightBody::CHAT_CHANNEL_CLUB:
	case CBasicChatRightBody::CHAT_CHANNEL_ALLIANCE:
	case CBasicChatRightBody::CHAT_CHANNEL_CHANNEL:
	case CBasicChatRightBody::CHAT_CHANNEL_SYSTEM:		m_pChatShowFlagButton->SetVisibleSingle( FALSE ); break;
	}
}

void CBasicChat::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case LEFT_BAR:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )		//	컨트롤 내부에 마우스가 위치하고,
			{
				//	캐릭터 움직임 막음
				AddMessageEx( UIMSG_MOUSEIN_BLOCK_CHARACTER_MOVE );

				if ( (dwMsg & UIMSG_MOUSEIN_LEFTBAR_DRAG) )	//	마우스를 좌클릭으로 눌렀을 때,
				{			
					SetExclusiveControl();	//	단독 컨트롤로 등록하고,
					if ( !m_bFirstGap )				//	최초 포지션 갭을 기록한다.
					{
						UIRECT rcPos = GetGlobalPos();
						m_vGap.x = rcPos.left - m_PosX;
						m_vGap.y = rcPos.top - m_PosY;
						m_bFirstGap = TRUE;
					}
				}
				else if ( CHECK_LB_UP_LIKE( dwMsg ) )	//	좌클릭했던것을 놓으면
				{
					ResetExclusiveControl();	//	단독 컨트롤을 해제하고
					m_bFirstGap = FALSE;				//	최초 포지션갭을 해제한다.
				}

				if ( UIMSG_MOUSEIN_CHAT_TYPE_CHANGE & dwMsg )
				{
                    m_pInterface->SetChatTypeListOpen();

                    DWORD dwMsg = GetMessageEx();
                    dwMsg &= ~UIMSG_LB_UP; 
                    SetMessageEx( dwMsg );
				}
			}
			else if ( CHECK_LB_UP_LIKE( dwMsg ) )		//	컨트롤 영역밖에서 버튼을 떼는 경우가
			{											//	발생하더라도
				ResetExclusiveControl();		//	단독 컨트롤을 해제하고,
				m_bFirstGap = FALSE;					//	최초 포지션갭을 해제한다.
			}
		}
		break;

	case CHAT_FLAG_BUTTON:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
			{
				if ( UIMSG_LB_DOWN & dwMsg )
				{
					if ( !m_pInterface->UiIsVisibleGroup( CHAT_OPTION_WINDOW ) )
					{
						m_pInterface->UiShowGroupFocus( CHAT_OPTION_WINDOW );
					}
				}
			}
		}	
		break;
	case RIGHT_BODY:
		{
			if ( UIMSG_MOUSEIN_RIGHTBODY & dwMsg )
			{
				//	캐릭터 움직임 막음
				AddMessageEx( UIMSG_MOUSEIN_BLOCK_CHARACTER_MOVE );
			}
		}
		break;
	}

	return ;
}

HRESULT CBasicChat::RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	hr = CUIGroup::RestoreDeviceObjects( pd3dDevice );
	if ( FAILED ( hr ) ) return hr;

	const long lResolution = CUIMan::GetResolution();
	WORD X_RES = HIWORD( lResolution );
	WORD Y_RES = LOWORD( lResolution );

	static INT nY_RES_BACK;

	/*
#ifdef CH_PARAM // 중국 인터페이스 변경

	UIRECT rcOldPos = GetGlobalPos();
	UIRECT rcNewPos = rcOldPos;

	WORD CH_Y_RES = Y_RES - 41;

	if( ( CH_Y_RES < rcOldPos.sizeY ) && ( CH_Y_RES > 60 ) )
	{ 
		WORD wAlignFlag = GetAlignFlag ();
		SetAlignFlag ( UI_FLAG_YSIZE );

		rcNewPos.sizeY = rcOldPos.sizeY - ( nY_RES_BACK - Y_RES );

		AlignSubControl( rcOldPos, rcNewPos );

		SetAlignFlag( wAlignFlag );
	}

	rcNewPos.top = CH_Y_RES - rcNewPos.sizeY;
	rcNewPos.bottom = rcNewPos.top + rcNewPos.sizeY;

	SetGlobalPos( rcNewPos );

#else
*/
	const UIRECT& rcWindow = GetGlobalPos();

	if ( ( Y_RES < rcWindow.sizeY ) && ( Y_RES > 60 ) ) // Note : 리사이즈, 60은 채팅 박스의 최소 사이즈
	{ 
		WORD wAlignFlag = GetAlignFlag();
		SetAlignFlag( UI_FLAG_YSIZE );

		const UIRECT& rcOldPos = GetLocalPos();
		UIRECT rcNewPos = rcOldPos;
		rcNewPos.sizeY = rcOldPos.sizeY - ( nY_RES_BACK - Y_RES );

		AlignSubControl( rcOldPos, rcNewPos );
		SetLocalPos( D3DXVECTOR2( rcNewPos.left, rcNewPos.top ) );

		SetGlobalPos( rcNewPos );
		SetAlignFlag( wAlignFlag );
	}
	else
	{
		D3DXVECTOR2 vPos( 0.0f, 0.0f );
		vPos.y = Y_RES - rcWindow.sizeY;
		SetGlobalPos( vPos );
	}

//#endif

	nY_RES_BACK = Y_RES;

	return S_OK;
}

void	CBasicChat::BEGIN_NORMAL_CHAT( const CString& strMessage )
{	
	m_pBasicChatRightBody->SetIgnoreState( true );
	m_pBasicChatRightBody->BEGIN_NORMAL_CHAT ( strMessage );
	m_pBasicChatRightBody->SetIgnoreState( false );
}

void	CBasicChat::BEGIN_PRIVATE_CHAT( const CString& strName, const CString& strMessage )
{
	m_pBasicChatRightBody->SetIgnoreState( true );
	m_pBasicChatRightBody->BEGIN_PRIVATE_CHAT( strName, strMessage );
	m_pBasicChatRightBody->SetIgnoreState( false );
}

void	CBasicChat::BEGIN_PARTY_CHAT( const CString& strMessage )
{
	m_pBasicChatRightBody->SetIgnoreState( true );
	m_pBasicChatRightBody->BEGIN_PARTY_CHAT( strMessage );
	m_pBasicChatRightBody->SetIgnoreState( false );
}

void	CBasicChat::BEGIN_TOALL_CHAT( const CString& strMessage )
{
	m_pBasicChatRightBody->SetIgnoreState( true );
	m_pBasicChatRightBody->BEGIN_TOALL_CHAT( strMessage );
}

void	CBasicChat::BEGIN_GUILD_CHAT( const CString& strMessage )
{
	m_pBasicChatRightBody->SetIgnoreState( true );
	m_pBasicChatRightBody->BEGIN_GUILD_CHAT( strMessage );
	m_pBasicChatRightBody->SetIgnoreState( false );
}

void	CBasicChat::BEGIN_ALLIANCE_CHAT( const CString& strMessage )
{
	m_pBasicChatRightBody->SetIgnoreState( true );
	m_pBasicChatRightBody->BEGIN_ALLIANCE_CHAT( strMessage );
	m_pBasicChatRightBody->SetIgnoreState( false );
}

void	CBasicChat::BEGIN_AREA_CHAT( const CString& strMessage )
{
	m_pBasicChatRightBody->SetIgnoreState( true );
	m_pBasicChatRightBody->BEGIN_AREA_CHAT( strMessage );
	m_pBasicChatRightBody->SetIgnoreState( false );
}

void	CBasicChat::BEGIN_PARTY_RECRUIT_CHAT( const CString& strMessage )
{
	m_pBasicChatRightBody->SetIgnoreState( true );
	m_pBasicChatRightBody->BEGIN_PARTY_RECRUIT_CHAT( strMessage );
	m_pBasicChatRightBody->SetIgnoreState( false );
}

void	CBasicChat::BEGIN_FACTION_CHAT( const CString& strMessage )
{
	m_pBasicChatRightBody->SetIgnoreState( true );
	m_pBasicChatRightBody->BEGIN_FACTION_CHAT( strMessage );
	m_pBasicChatRightBody->SetIgnoreState( false );
}


void CBasicChat::REFRESH_FRIEND_LIST()
{
	if( m_pBasicChatRightBody )
		m_pBasicChatRightBody->ADD_FRIEND_LIST();
}

void CBasicChat::FRIEND_LIST( CONST CString & strName, bool bOnline )
{
	if( !m_pBasicChatRightBody ) return ;

	if( bOnline )	m_pBasicChatRightBody->ADD_FRIEND( strName );
	else			m_pBasicChatRightBody->DEL_FRIEND( strName );
}

bool CBasicChat::IsCHAT_BEGIN()
{
	if (!m_pBasicChatRightBody)
        return false;
    else
	    return m_pBasicChatRightBody->IsEDIT_BEGIN();
}

void CBasicChat::SetChatOption(WORD ChatFlag)
{
    if (m_pBasicChatRightBody)
	    m_pBasicChatRightBody->SetChatFlagState( static_cast<WORD>(ChatFlag));
}

void CBasicChat::SetChatType(DWORD ChatType)
{
    if (m_pBasicChatRightBody)
        m_pBasicChatRightBody->CHANGE_CHAT_TYPE(ChatType);
}

void CBasicChat::CHANGE_CHAT_TYPE()
{
	DWORD dwChatType = m_pBasicChatRightBody->GetCHATTYPE();

	switch ( dwChatType ) 
	{
	case CHAT_CONST::CHAT_NORMAL:
		dwChatType = CHAT_CONST::CHAT_PRIVATE;
		break;
	case CHAT_CONST::CHAT_PRIVATE:
		dwChatType = CHAT_CONST::CHAT_PARTY;
		break;
	case CHAT_CONST::CHAT_PARTY:
		dwChatType = CHAT_CONST::CHAT_GUILD;
		break;
	case CHAT_CONST::CHAT_GUILD:
		dwChatType = CHAT_CONST::CHAT_ALLIANCE;
		break;
	case CHAT_CONST::CHAT_ALLIANCE:
		{
			if ( GLCONST_CHAR::bCHAT_EXTEND ) dwChatType = CHAT_CONST::CHAT_AREA;
			else dwChatType =CHAT_CONST::CHAT_TOALL;
		}
		break;
	case CHAT_CONST::CHAT_AREA:
		{
			if ( GLCONST_CHAR::bCHAT_EXTEND ) dwChatType = CHAT_CONST::CHAT_PARTY_RECRUIT;
			else dwChatType =CHAT_CONST::CHAT_TOALL;		
		}
		break;
	case CHAT_CONST::CHAT_PARTY_RECRUIT:
		dwChatType = CHAT_CONST::CHAT_TOALL;
		break;
	case CHAT_CONST::CHAT_TOALL:
		dwChatType = CHAT_CONST::CHAT_NORMAL;
		break;
	case CHAT_CONST::CHAT_SYSTEM:
		dwChatType = CHAT_CONST::CHAT_NORMAL;
		break;
	}

	m_pBasicChatRightBody->CHANGE_CHAT_TYPE( dwChatType );
}


void CBasicChat::ChatToLink( const SLINK_DATA_BASIC& sLinkDataBasic, const CString strLinkName, DWORD dwColor )
{
	if ( m_pBasicChatRightBody ) 
		m_pBasicChatRightBody->ChatToLink( sLinkDataBasic, strLinkName, dwColor );
}

void CBasicChat::SET_PRIVATE_NAME( const CString& strName )
{
	if ( m_pBasicChatRightBody )
		m_pBasicChatRightBody->SET_PRIVATE_NAME( strName );
}

void CBasicChat::SendReplyAwayState( const std::string& strPrivateName, const std::string& strMsg )
{
    SET_PRIVATE_NAME( strPrivateName.c_str() );

    if ( m_pBasicChatRightBody )
    {
        BEGIN_PRIVATE_CHAT( strPrivateName.c_str(), strMsg.c_str() );
        m_pBasicChatRightBody->SendReplyAwayState( strMsg );
    }
}

const UIRECT& CBasicChat::GetChangStateButtonGlobalPos ()
{
    if ( !m_pBasicChatLeftBar )
    {
        GASSERT(!"CBasicChat::GetBasicChatLeftBarGlobalPos - m_pBasicChatLeftBar is NULL");
    }

    return m_pBasicChatLeftBar->GetChangStateButtonGlobalPos();
}

MyBasicChat::MyBasicChat( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CBasicChat( pGaeaClient, pInterface, pEngineDevice )
{
}

void MyBasicChat::CreateUIWindowAndRegisterOwnership()
{
    CBasicChat::Create( BASIC_CHAT_BOX, "BASIC_CHAT_MINBOX", UI_FLAG_BOTTOM );
    CBasicChat::CreateSubControl();
    CBasicChat::SetProtectSizePoint();

    // 최초 채팅 박스의 크기를 최소 사이즈가 아니라 지정된 사이즈로 리사이즈 합니다.
    {
        WORD wAlignFlag = CBasicChat::GetAlignFlag();
        CBasicChat::SetAlignFlag( UI_FLAG_YSIZE );

        CUIControl TempControl(m_pEngineDevice);
        TempControl.Create( 1, "BASIC_CHAT_BOX" );
        const UIRECT& rcParentOldPos = CBasicChat::GetLocalPos();
        const UIRECT& rcParentNewPos = TempControl.GetLocalPos();
        CBasicChat::AlignSubControl( rcParentOldPos, rcParentNewPos );
        CBasicChat::SetLocalPos( D3DXVECTOR2( rcParentNewPos.left, rcParentNewPos.top ) );

        CBasicChat::SetAlignFlag( wAlignFlag );
    }		
    CBasicChat::m_pInterface->UiRegisterControl( this );
    CBasicChat::m_pInterface->UiShowGroupFocus( BASIC_CHAT_BOX );
}

void MyBasicChat::AddChatMacro( UI::String strMsg )
{
    CBasicChat::AddChatMacro( UI::ToString( strMsg ) );
}

void MyBasicChat::AddStringToSystemMessage( UI::String strTemp, D3DCOLOR dwColor )
{
    CBasicChat::AddStringToSystemMessage( UI::ToString( strTemp ), dwColor );
}

void MyBasicChat::AddStringToChatLink( UI::String strTemp, WORD wType, VEC_LINK_TEXT_DATA& vecLinkTextData, bool bGM )
{
    CBasicChat::AddStringToChatLink( UI::ToString( strTemp ), wType, vecLinkTextData, bGM );
}

void MyBasicChat::AddStringToChatEx( UI::String strTemp, WORD wType, bool bGM )
{
    CBasicChat::AddStringToChatEx( UI::ToString( strTemp ), wType, bGM );
}

void MyBasicChat::AddStringToNomal( UI::String strTemp, D3DCOLOR dwColor )
{
	CBasicChat::AddStringToNormal( UI::ToString( strTemp ), dwColor );
}

void MyBasicChat::RefreshFriendList()
{
    CBasicChat::REFRESH_FRIEND_LIST();
}

void MyBasicChat::ChatLog( bool bChatLog, int nChatLogType )
{
    CBasicChat::ChatLog( bChatLog, nChatLogType );
}

UI::String MyBasicChat::GetRecordChat()
{
    return UI::ToString( CBasicChat::GET_RECORD_CHAT() );
}

void MyBasicChat::SetFriendOnlineList( CONST UI::String & strName, bool bOnline )
{
    CBasicChat::FRIEND_LIST( UI::ToString( strName ), bOnline );
}

bool MyBasicChat::IsChatBegin()
{
    return CBasicChat::IsCHAT_BEGIN();
}

void MyBasicChat::SetChatOption( WORD ChatFlag )
{
    CBasicChat::SetChatOption( ChatFlag );
}

void MyBasicChat::SetChatType( DWORD ChatType )
{
    CBasicChat::SetChatType( ChatType );
}

void MyBasicChat::ChatToLink( const SLINK_DATA_BASIC& sLinkDataBasic, const UI::String strLinkName, DWORD dwColor )
{
    CBasicChat::ChatToLink( sLinkDataBasic, UI::ToString( strLinkName ), dwColor );
}

void MyBasicChat::BeginPrivateChat( const UI::String& strName /*= ""*/, const UI::String& strMessage /*= "" */ )
{
    CBasicChat::BEGIN_PRIVATE_CHAT( UI::ToString( strName ), UI::ToString( strMessage ) );
}

const UIRECT& MyBasicChat::GetUIWindowGlobalPos()
{
    return CBasicChat::GetGlobalPos();
}

const UIRECT& MyBasicChat::GetUIChangStateButtonGlobalPos()
{
    return CBasicChat::GetChangStateButtonGlobalPos();
}

void MyBasicChat::SetPrivateMsgCharName( const UI::String& strName )
{
    CBasicChat::SET_PRIVATE_NAME( UI::ToString( strName ) );
}

void MyBasicChat::SetNoUpdate( BOOL bNoUpdate )
{
    CBasicChat::SetNoUpdate( bNoUpdate );
}

void MyBasicChat::SendReplyAwayState( const std::string& strPrivateName, const std::string& strMsg )
{
    CBasicChat::SendReplyAwayState( strPrivateName, strMsg );
}
