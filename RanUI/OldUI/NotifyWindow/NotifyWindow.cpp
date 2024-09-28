#include "StdAfx.h"

#include "../../InnerInterface.h"
#include "./NotifyWindow.h"

#include "../../../EngineLib/GUInterface/NSGUI.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicTextBoxEx.h"
#include "../../../enginelib/GUInterface/BasicScrollBarEx.h"
#include "../../../enginelib/GUInterface/BasicScrollThumbFrame.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"

#include "../Util/MessageBox.h"

#include "../../../RanLogicClient\GLGaeaClient.h"
#include "../../../RanLogicClient\Notify\NotifyClientBase.h"
#include "../../../RanLogicClient\Notify\NotifyClientMan.h"
#include "../../../RanLogicClient/Tournament/GLTournamentClient.h"
#include "../../../Ranlogicclient/GLGaeaClient.h"
#include "../../../RanLogic/RANPARAM.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------




#include "../../../RanLogicClient\Notify\NotifyClientFriendAdd.h"

namespace
{
    const char* TEXT_TITLE = "알림정보";

}

CNotifyWindow::CNotifyWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CUIGroup( pEngineDevice )
    , m_pGaeaClient( pGaeaClient )
    , m_pInterface( pInterface )
    , m_nViewPosOld( -1 )
    , m_fNewMessageTime( -1 )
    , m_pNotiftMan( NULL )
    , m_pTitleText( NULL )
    , m_pTitleBackGround( NULL )
    , m_pBackGroundA( NULL )
    , m_pBackGroundB( NULL )
    , m_pMessageBox( NULL )
    , m_pLineBoxSeleted( NULL )
    , m_pScrollBar( NULL )
    , m_onMouse(0)
{
    for( int i = 0; i < LineTextControlCount ; i ++ )
    {
        m_pLineTextControl[i] = NULL;
    }
    TEXT_TITLE = ID2GAMEWORD( "NOTIFY_WINDOW_TITLE", 0 );
   
}

void CNotifyWindow::CreateSubControl()
{    
    CD3DFontPar* pFont10 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 10, _DEFAULT_FONT_SHADOW_FLAG );

    m_pTitleBackGround = new CBasicLineBoxSmart( m_pEngineDevice );
    m_pTitleBackGround->CreateSub( this, "NOTIFY_MESSAGE_WINDOW_TITLE", UI_FLAG_DEFAULT, ID_WINDOW_TITLE );
    m_pTitleBackGround->CreateSubControl( "UIUTILE_WINDOW_TITLE_TEXTURE" );
    RegisterControl( m_pTitleBackGround );

    m_pTitleText = new CBasicTextBox( m_pEngineDevice );
    m_pTitleText->CreateSub( this, "NOTIFY_MESSAGE_WINDOW_TITLE" );
    m_pTitleText->SetFont ( pFont10 );
    m_pTitleText->SetTextAlign ( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );	
    RegisterControl( m_pTitleText );


    m_pBackGroundA = new CBasicLineBoxSmart( m_pEngineDevice );
    m_pBackGroundA->CreateSub( this, "NOTIFY_MESSAGE_WINDOW_BG", UI_FLAG_DEFAULT );
    m_pBackGroundA->CreateSubControl( "PARTY_WINDOW_RENEWAL_TEXTURE_INFO" );
    RegisterControl( m_pBackGroundA );

    m_pBackGroundB = new CBasicLineBoxSmart( m_pEngineDevice );
    m_pBackGroundB->CreateSub( this, "NOTIFY_MESSAGE_WINDOW_BG", UI_FLAG_DEFAULT, ID_WINDOW_BG );
    m_pBackGroundB->CreateSubControl( "UIUTILE_WINDOW_BG_TEXTURE" );
    RegisterControl( m_pBackGroundB );


    CString strControlIDList[LineTextControlCount] = 
    {
        "NOTIFY_MESSAGE_WINDOW_SLOT_0",
        "NOTIFY_MESSAGE_WINDOW_SLOT_1",
        "NOTIFY_MESSAGE_WINDOW_SLOT_2",
        "NOTIFY_MESSAGE_WINDOW_SLOT_3",
        "NOTIFY_MESSAGE_WINDOW_SLOT_4",
        "NOTIFY_MESSAGE_WINDOW_SLOT_5",
        "NOTIFY_MESSAGE_WINDOW_SLOT_6",
    };

//     m_pLineBoxSeleted = new CBasicLineBoxSmart( m_pEngineDevice );
//     m_pLineBoxSeleted->CreateSub( this, "NOTIFY_MESSAGE_WINDOW_SLOT_0", UI_FLAG_DEFAULT );
//     m_pLineBoxSeleted->CreateSubControl( "LINEBOX_Y" );
//     RegisterControl( m_pLineBoxSeleted );

    for( int i = 0 ; i < LineTextControlCount ; i++ )
    {
        m_pLineTextControl[i] = new CBasicTextBox( m_pEngineDevice );
        m_pLineTextControl[i]->CreateSub( this, strControlIDList[i].GetString(), UI_FLAG_DEFAULT, ID_TEXTBOX_START+i );
        m_pLineTextControl[i]->SetFont ( pFont10 );
        m_pLineTextControl[i]->SetTextAlign ( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );	
        RegisterControl( m_pLineTextControl[i] );
    }


    { // 스크롤바
        CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
        pScrollBar->CreateSub( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, ID_SCROOLBAR );
        pScrollBar->CreateBaseScrollBar( "NOTIFY_MESSAGE_MESSAGE_SCROOL" );
        //pScrollBar->GetThumbFrame()->SetScrollParent( m_pInterface );
        RegisterControl ( pScrollBar );
        m_pScrollBar = pScrollBar;
    }

    CMessageBox* pMessageBox = new CMessageBox( m_pEngineDevice );
    pMessageBox->CreateSub( this, "NOTIFY_MESSAGE_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE, ID_MESSAGEBOX );
    pMessageBox->CreateSubControl();
    RegisterControl( pMessageBox );
    m_pMessageBox = pMessageBox;

    m_pNotiftMan = m_pGaeaClient->GetNotifyClientMan();

    Refresh();
}

void CNotifyWindow::SetVisibleSingle( BOOL bVisible )
{
    if( bVisible == TRUE )
    {
        m_fNewMessageTime = -1;
        SetVisibleAllControl( TRUE );
        m_pMessageBox->SetDiffuseAlpha( 255 );
        m_pMessageBox->SetVisibleSingleFalse();
    }
      

    CUIGroup::SetVisibleSingle( bVisible );
}

void CNotifyWindow::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

    if( 0 <= m_fNewMessageTime && m_fNewMessageTime < NewMessageAlarmTime )
    {
        m_fNewMessageTime += fElapsedTime*1000;

        float fPercent = m_fNewMessageTime / NewMessageAlarmTime;
        WORD wDiffuse = static_cast<WORD>( 255 - ( 254 * fPercent * fPercent ) );
        m_pMessageBox->SetDiffuseAlpha( wDiffuse );

    }

    
    if( NewMessageAlarmTime < m_fNewMessageTime)
    {
        m_fNewMessageTime = -1;
        SetVisibleSingle( FALSE );
    }


    unsigned int nTotal = (int) m_vecNotifyMessage.size();

    m_pScrollBar->GetThumbFrame()->SetState( nTotal, LineTextControlCount );
    int nVisibleLine = LineTextControlCount;


    CBasicScrollThumbFrame* const pThumbFrame = m_pScrollBar->GetThumbFrame();

    const int nMovableLine = nTotal - nVisibleLine;
    float fPercent = pThumbFrame->GetPercent();

    unsigned int m_nViewPos = (int)floor( fPercent * nMovableLine );
    if( m_nViewPosOld != m_nViewPos)
    {
        m_nViewPosOld = m_nViewPos;
        SetTextBoxList( m_nViewPos );
    }


}

void CNotifyWindow::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    TranslateMessageBox( m_pMessageBox->GetIdentifier(), dwMsg );
    CUIGroup::TranslateUIMessage( ControlID, dwMsg);

    bool bIsClick = false;
    int nOnMouseIndex = -1;

    if( ControlID == ID_MESSAGEBOX )
    {
        if( CHECK_MOUSE_IN( dwMsg ) )
        {
            if( 0 <= m_fNewMessageTime && m_fNewMessageTime < NewMessageAlarmTime )
                m_fNewMessageTime = 0;
        }
        return;
    }
    

    if( ID_TEXTBOX_START <= ControlID && ControlID <= ID_TEXTBOX_END )
    {

        if( m_pMessageBox->IsVisible() == TRUE )
            return;

        unsigned int unIndex = ControlID - ID_TEXTBOX_START;

        CBasicTextBox* pLineText = m_pLineTextControl[unIndex];
        if( pLineText == NULL )
            return;

        DWORD dwColor = D3DCOLOR_ARGB( 255,86,177,202 );
        if( CHECK_MOUSE_IN( dwMsg ) )
        {
            dwColor =  D3DCOLOR_ARGB( 255,255,192,0 );
            nOnMouseIndex = (int)unIndex;

            if( CHECK_LB_UP_LIKE( dwMsg ) )
            {
                unsigned int nSelected = unIndex + m_nViewPosOld;

                if( nSelected < m_vecNotifyMessage.size() )
                {    
                    m_spSeleted = m_pNotiftMan->GetData( m_vecNotifyMessage[nSelected] );
                    
                    bIsClick = true;
                    SetVisibleAllControl( FALSE );

                    if( m_pNotiftMan == NULL )
                        return;

                    const std::tr1::shared_ptr<INotifyClientBase>& NotifyBase 
                        = m_pNotiftMan->GetData( m_vecNotifyMessage[nSelected] );


					// 토너먼트중 파티를 할수가 없다.
					// TOURNAMENT_UNIQ_DISABLE
					bool bTOURNAMNET = m_pGaeaClient->GetTournamentClient()->GetIsTournamentView();

					//	Note : 파티 자동 거부.
					//
					
					if( NotifyBase->GetType() == gamelogic::notify::PARTY_INVITE && (RANPARAM::bDIS_PARTY || bTOURNAMNET ))
					{
						NotifyBase->SendRefused();
					}
					else
					{
						m_pMessageBox->OpenMessageBox( 
							CMessageBox::TITLE_QUESTION, 
							NotifyBase->GetMessageBoxText(), 
							CMessageBox::TYPE_YESNO, NotifyBase->GetType() );
					}

                    
                    //m_pLineBoxSeleted->SetVisibleSingle( FALSE );
                }
            }
        }

		if(!pLineText->empty())
		{
			pLineText->SetUseTextColor( 0, TRUE );
			pLineText->SetTextColor(0, dwColor );
		}

    }

//     if( 0 <= nOnMouseIndex && nOnMouseIndex < LineTextControlCount )
//     {
//         UIRECT rcPos = m_pLineTextControl[nOnMouseIndex]->GetGlobalPos();
//         rcPos.sizeY += 1;
//         //m_pLineBoxSeleted->SetGlobalPos( rcPos );
//     }
//     
}

void CNotifyWindow::TranslateMessageBox( unsigned int nIdentity, DWORD dwMsg )
{
    if( m_pNotiftMan == NULL )
        return;

    if( m_spSeleted == NULL )
        return;
    
    if( dwMsg & UIMSG_MESSAGEBOX_POSITIVE )
    {
        m_spSeleted->SendAccept();
        DeleteNotifyMessage();
        SetVisibleAllControl( TRUE );
        
        if( m_vecNotifyMessage.size() <= 0)
            SetVisibleSingle( FALSE );

    }
    else if( dwMsg & UIMSG_MESSAGEBOX_NEGATIVE )
    {
        m_spSeleted->SendRefused();
        DeleteNotifyMessage();
        SetVisibleAllControl( TRUE );

        if( m_vecNotifyMessage.size() <= 0)
            SetVisibleSingle( FALSE );
    }

    
}

void CNotifyWindow::NewMessage()
{
    if( m_pNotiftMan == NULL )
        return;

    m_spSeleted = m_pNotiftMan->GetLatelyData();

    if( m_spSeleted == NULL )
        return;
    
    Refresh();
    SetVisibleSingle( TRUE );
    SetVisibleAllControl( FALSE );

    

    m_pMessageBox->OpenMessageBox( 
        CMessageBox::TITLE_QUESTION, 
        m_spSeleted->GetMessageBoxText(), 
        CMessageBox::TYPE_YESNO, m_spSeleted->GetType() );

    m_fNewMessageTime = 0;

}

void CNotifyWindow::Refresh()
{
	m_vecNotifyMessage.clear();
    GetData();
    SetTextBoxList( m_nViewPosOld );
    
    std::string strTemp = sc::string::format( "%s(%d)", TEXT_TITLE, m_vecNotifyMessage.size() );
    m_pTitleText->SetText( strTemp.c_str(), NS_UITEXTCOLOR::YELLOW );

    //m_pLineBoxSeleted->SetVisibleSingle( TRUE );
}

void CNotifyWindow::SetVisibleAllControl( BOOL bVisible )
{
    m_pTitleText->SetVisibleSingle( bVisible );
    m_pTitleBackGround->SetVisibleSingle( bVisible );
    m_pBackGroundA->SetVisibleSingle( bVisible );
    m_pBackGroundB->SetVisibleSingle( bVisible );
    //m_pLineBoxSeleted->SetVisibleSingle( bVisible );
    m_pScrollBar->SetVisibleSingle( bVisible );
    for( int i = 0 ; i < LineTextControlCount ; i++ )
        m_pLineTextControl[i]->SetVisibleSingle( bVisible );

}

void CNotifyWindow::GetData()
{
    if( m_pNotiftMan == NULL )
        return;



    BOOST_FOREACH( const NotifyClientMan::NOTIFY_DATA_VALUE& value, m_pNotiftMan->GetData() )
    {
        const std::tr1::shared_ptr<INotifyClientBase>& NotifyBase = value.second;

        if( NotifyBase == NULL )
            continue;

        m_vecNotifyMessage.push_back( value.first );        
    }

}

void CNotifyWindow::SetTextBoxList( unsigned int nViewPos )
{
    if( m_pNotiftMan == NULL )
        return;

    for( unsigned int nIndex=0; nIndex < m_vecNotifyMessage.size(); nIndex++ )
    {
        int nControlIndex = nIndex-nViewPos;
        if( 0 <= nControlIndex && nControlIndex < LineTextControlCount )
        {
            const std::tr1::shared_ptr<INotifyClientBase>& NotifyBase 
                = m_pNotiftMan->GetData( m_vecNotifyMessage[nIndex] );

            std::string strTemp = sc::string::format(
                "[%1%] %2$-14s %3%",
                NotifyBase->GetDateStr().c_str(), 
                NotifyBase->GetTypeStr().c_str(), 
                NotifyBase->Description().c_str() );

            m_pLineTextControl[nControlIndex]->SetText( strTemp.c_str(), D3DCOLOR_ARGB( 255,86,177,202 ) );
        }

    }

    for( int nIndex=m_vecNotifyMessage.size(); nIndex < LineTextControlCount; nIndex++ )
        m_pLineTextControl[nIndex]->ClearText();

}


void CNotifyWindow::DeleteNotifyMessage()
{
    if( m_spSeleted != NULL )
    {
        if( m_pNotiftMan == FALSE )
            return;

        m_pNotiftMan->Del( m_spSeleted->GetGuid() );
    }

    Refresh();
}
