#include "StdAfx.h"
#//include "BasicChatLeftBar.h"

#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/SmartComboBoxRollOver.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"

#include "../Interface/ChatType.h"
#include "../Interface/ChatTypeList.h"

#include "../../InnerInterface.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const D3DCOLOR dwMOUSEOVERCOLOR = D3DCOLOR_ARGB(0xFF,0x49,0x49,0x49);

CChatTypeList::CChatTypeList( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CUIGroup(pEngineDevice)
, m_pInterface(pInterface)
{
}

CChatTypeList::~CChatTypeList ()
{
}

void CChatTypeList::CreateSubControl ()
{
    CSmartComboBoxRollOver* pComboBox;
    pComboBox = new CSmartComboBoxRollOver(m_pEngineDevice);
    pComboBox->CreateSub ( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, CHAT_COMBOBOX );
    pComboBox->CreateSmartComboBoxRollOver( "CHATTYPE_LIST_COMBOBOX_ROLLOVER", "CHATTYPE_LIST_COMBOBOX_ROLLOVER_TEXINFO" );
    pComboBox->SetMouseOverColor( dwMOUSEOVERCOLOR );
    pComboBox->SetScrollPercent( 0.0f );
    RegisterControl ( pComboBox );
    m_pComboBox = pComboBox;

    InitChatTypeList ();
}

void CChatTypeList::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

    DWORD dwMsg = GetMessageEx();

    if ( (dwMsg & UIMSG_MOUSEOUT) && (UIMSG_LB_DOWN & dwMsg) )
    {		
        m_pInterface->SetChatTypeListClose();
        return;
    }
}

void CChatTypeList::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
    switch ( ControlID )
    {	
    case CHAT_COMBOBOX :
        {
            if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
            {		
                int nIndex = m_pComboBox->GetSelectIndex ();

                if ( nIndex < 0 )
                {
                    break;
                }

                DWORD dwChatType = m_pComboBox->GetTextData( nIndex );

                m_pInterface->SetChatType( dwChatType );
                m_pInterface->SetChatTypeListClose();
            }
        }
        break;
    };
}

void CChatTypeList::InitChatTypeList ()
{
    int nIndex = 0;
    m_pComboBox->ClearText ();

    m_pComboBox->AddText		( ID2GAMEWORD("CHATTYPE_LIST_TEXT", 0), false, false, CHAT_INFOR_COLOR::CHAT_NORMAL );
    m_pComboBox->SetTextData	( nIndex++, CHAT_CONST::CHAT_NORMAL );

    m_pComboBox->AddText		( ID2GAMEWORD("CHATTYPE_LIST_TEXT", 1), false, false, CHAT_INFOR_COLOR::CHAT_PRIVATE );
    m_pComboBox->SetTextData	( nIndex++, CHAT_CONST::CHAT_PRIVATE );

    m_pComboBox->AddText		( ID2GAMEWORD("CHATTYPE_LIST_TEXT", 2), false, false, CHAT_INFOR_COLOR::CHAT_PARTY );
    m_pComboBox->SetTextData	( nIndex++, CHAT_CONST::CHAT_PARTY );

    m_pComboBox->AddText		( ID2GAMEWORD("CHATTYPE_LIST_TEXT", 3), false, false, CHAT_INFOR_COLOR::CHAT_GUILD );
    m_pComboBox->SetTextData	( nIndex++, CHAT_CONST::CHAT_GUILD );

    m_pComboBox->AddText		( ID2GAMEWORD("CHATTYPE_LIST_TEXT", 4), false, false, CHAT_INFOR_COLOR::CHAT_ALLIANCE );
    m_pComboBox->SetTextData	( nIndex++, CHAT_CONST::CHAT_ALLIANCE );

    m_pComboBox->AddText		( ID2GAMEWORD("CHATTYPE_LIST_TEXT", 5), false, false, CHAT_INFOR_COLOR::CHAT_AREA );
    m_pComboBox->SetTextData	( nIndex++, CHAT_CONST::CHAT_AREA );

    m_pComboBox->AddText		( ID2GAMEWORD("CHATTYPE_LIST_TEXT", 6), false, false, CHAT_INFOR_COLOR::CHAT_PARTY_RECRUIT );
    m_pComboBox->SetTextData	( nIndex++, CHAT_CONST::CHAT_PARTY_RECRUIT );

	m_pComboBox->AddText		( ID2GAMEWORD("CHATTYPE_LIST_TEXT", 8), false, false, CHAT_INFOR_COLOR::CHAT_FACTION );
	m_pComboBox->SetTextData	( nIndex++, CHAT_CONST::CHAT_FACTION );

    m_pComboBox->AddText		( ID2GAMEWORD("CHATTYPE_LIST_TEXT", 7), false, false, CHAT_INFOR_COLOR::CHAT_TOALL );
    m_pComboBox->SetTextData	( nIndex++, CHAT_CONST::CHAT_TOALL );
}

MyChatTypeList::MyChatTypeList( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CChatTypeList( pInterface, pEngineDevice )
{
}

void MyChatTypeList::CreateUIWindowAndRegisterOwnership()
{
    CChatTypeList::Create( CHATTYPE_LIST, "CHATTYPE_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    CChatTypeList::CreateSubControl();
    CChatTypeList::SetAlignFlag( UI_FLAG_BOTTOM | UI_FLAG_LEFT );
    CChatTypeList::m_pInterface->UiRegisterControl( this, true );
    CChatTypeList::m_pInterface->UiShowGroupFocus( CHATTYPE_LIST );
}

void MyChatTypeList::SetUIGlobalPos( const UIRECT& rcPosition )
{
    CChatTypeList::SetGlobalPos( rcPosition );
}

const UIRECT& MyChatTypeList::GetUIGlobalPos()
{
    return CChatTypeList::GetGlobalPos();
}
