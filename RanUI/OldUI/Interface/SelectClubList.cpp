#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../InnerInterface.h"
#include "./SelectClubListField.h"
#include "./SelectClubList.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CSelectClubList::CSelectClubList(GLGaeaClient* pGaeaClient, COuterInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pNameField(NULL)
    , m_pRankField(NULL)
    , m_pLevelField(NULL)
    , m_pMasterField(NULL)
    , m_pMemberField(NULL)
    , m_pClubListText(NULL)
    , m_pListScroll(NULL)
{
    m_vecClubInfo.clear();
}

CSelectClubList::~CSelectClubList()
{
}

void CSelectClubList::CreateSubControl()
{
    {
        CSelectClubListField* pClubListField = new CSelectClubListField( m_pGaeaClient, m_pInterface, m_pEngineDevice );
        pClubListField->CreateSub( this, "SELCET_CLUB_LIST_NAME_FIELD", UI_FLAG_DEFAULT, NAME_FIELD );
        pClubListField->CreateSubControl( "SELCET_CLUB_LIST_NAME_FIELD_BACK",
            "SELCET_CLUB_LIST_NAME_FIELD_TEXT",
            "SELCET_CLUB_LIST_NAME_FIELD_ORDER",
            "SELCET_CLUB_LIST_NAME_FIELD_LIST" );
        RegisterControl( pClubListField );
        m_pNameField = pClubListField;
        m_pNameField->SetFieldType( wb::search::CLUB_NAME_SEARCH );
        m_pNameField->SetFiledText( ID2GAMEWORD( "SELECT_CLUB_FIELD", 0 ) );
    }

    {
        CSelectClubListField* pClubListField = new CSelectClubListField( m_pGaeaClient, m_pInterface, m_pEngineDevice );
        pClubListField->CreateSub( this, "SELCET_CLUB_LIST_RANK_FIELD", UI_FLAG_DEFAULT, RANK_FIELD );
        pClubListField->CreateSubControl( "SELCET_CLUB_LIST_RANK_FIELD_BACK",
            "SELCET_CLUB_LIST_RANK_FIELD_TEXT",
            "SELCET_CLUB_LIST_RANK_FIELD_ORDER",
            "SELCET_CLUB_LIST_RANK_FIELD_LIST" );
        RegisterControl( pClubListField );
        m_pRankField = pClubListField;
        m_pRankField->SetFieldType( wb::search::CLUB_RANK );
        m_pRankField->SetFiledText( ID2GAMEWORD( "SELECT_CLUB_FIELD", 1 ) );
    }

    {
        CSelectClubListField* pClubListField = new CSelectClubListField( m_pGaeaClient, m_pInterface, m_pEngineDevice );
        pClubListField->CreateSub( this, "SELCET_CLUB_LIST_LEVEL_FIELD", UI_FLAG_DEFAULT, LEVEL_FIELD );
        pClubListField->CreateSubControl( "SELCET_CLUB_LIST_LEVEL_FIELD_BACK",
            "SELCET_CLUB_LIST_LEVEL_FIELD_TEXT",
            "SELCET_CLUB_LIST_LEVEL_FIELD_ORDER",
            "SELCET_CLUB_LIST_LEVEL_FIELD_LIST" );
        RegisterControl( pClubListField );
        m_pLevelField = pClubListField;
        m_pLevelField->SetFieldType( wb::search::CLUB_MASTER_LEVEL );
        m_pLevelField->SetFiledText( ID2GAMEWORD( "SELECT_CLUB_FIELD", 2 ) );
    }

    {
        CSelectClubListField* pClubListField = new CSelectClubListField( m_pGaeaClient, m_pInterface, m_pEngineDevice );
        pClubListField->CreateSub( this, "SELCET_CLUB_LIST_MASTER_FIELD", UI_FLAG_DEFAULT, MASTER_FIELD );
        pClubListField->CreateSubControl( "SELCET_CLUB_LIST_MASTER_FIELD_BACK",
            "SELCET_CLUB_LIST_MASTER_FIELD_TEXT",
            "SELCET_CLUB_LIST_MASTER_FIELD_ORDER",
            "SELCET_CLUB_LIST_MASTER_FIELD_LIST" );
        RegisterControl( pClubListField );
        m_pMasterField = pClubListField;
        m_pMasterField->SetFieldType( wb::search::CLUB_MASTER_NAME );
        m_pMasterField->SetFiledText( ID2GAMEWORD( "SELECT_CLUB_FIELD", 3 ) );
    }

    {
        CSelectClubListField* pClubListField = new CSelectClubListField( m_pGaeaClient, m_pInterface, m_pEngineDevice );
        pClubListField->CreateSub( this, "SELCET_CLUB_LIST_MEMBER_FIELD", UI_FLAG_DEFAULT, MEMBER_FIELD );
        pClubListField->CreateSubControl( "SELCET_CLUB_LIST_MEMBER_FIELD_BACK",
            "SELCET_CLUB_LIST_MEMBER_FIELD_TEXT",
            "SELCET_CLUB_LIST_MEMBER_FIELD_ORDER",
            "SELCET_CLUB_LIST_MEMBER_FIELD_LIST" );
        RegisterControl( pClubListField );
        m_pMemberField = pClubListField;
        m_pMemberField->SetFieldType( wb::search::CLUB_MEMBER_NUM );
        m_pMemberField->SetFiledText( ID2GAMEWORD( "SELECT_CLUB_FIELD", 4 ) );
    }

    {
        CUIControl* pControl = new CUIControl( m_pEngineDevice );
        pControl->CreateSub( this, "SELCET_CLUB_WINDOW_LIST_SEPLINE_0" );
        RegisterControl( pControl );

        pControl = new CUIControl( m_pEngineDevice );
        pControl->CreateSub( this, "SELCET_CLUB_WINDOW_LIST_SEPLINE_1" );
        RegisterControl( pControl );

        pControl = new CUIControl( m_pEngineDevice );
        pControl->CreateSub( this, "SELCET_CLUB_WINDOW_LIST_SEPLINE_2" );
        RegisterControl( pControl );

        pControl = new CUIControl( m_pEngineDevice );
        pControl->CreateSub( this, "SELCET_CLUB_WINDOW_LIST_SEPLINE_3" );
        RegisterControl( pControl );
    }

    {
        CUIControl* pControl = new CUIControl( m_pEngineDevice );
        pControl->CreateSub( this, "SELCET_CLUB_WINDOW_LIST_BACK" );
        RegisterControl( pControl );

        //	스크롤바
        CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
        pScrollBar->CreateSub( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE );
        pScrollBar->CreateBaseScrollBar( "SELCET_CLUB_WINDOW_LIST_BACK_SCROLLBAR" );
        pScrollBar->GetThumbFrame()->SetState( 1, 0 );
        pScrollBar->GetThumbFrame()->SetScrollParent( pControl );
        pScrollBar->GetThumbFrame()->SetScrollMoveLine( 2 );
        RegisterControl( pScrollBar );
        m_pListScroll = pScrollBar;
    }

    SetClubListOrder( m_pNameField->GetFieldType(), wb::search::ASC );
}

void CSelectClubList::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

    {
        CBasicScrollThumbFrame* const pThumbFrame = m_pListScroll->GetThumbFrame();

        const int nTotalLine = static_cast<int>( m_vecClubInfo.size() );
        const int nLinePerOneView = m_pNameField->GetVisibleListLine();

        pThumbFrame->SetState( nTotalLine, nLinePerOneView );
        if ( nLinePerOneView < nTotalLine )
        {
            const int nMovableLine = nTotalLine - nLinePerOneView;
            float fPercent = pThumbFrame->GetPercent();
            int nPos = static_cast<int>(floor(fPercent * nMovableLine));

            m_pNameField->SetCurListLine( nPos );
            m_pRankField->SetCurListLine( nPos );
            m_pLevelField->SetCurListLine( nPos );
            m_pMasterField->SetCurListLine( nPos );
            m_pMemberField->SetCurListLine( nPos );
        }
    }
}

void CSelectClubList::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    CUIGroup::TranslateUIMessage( ControlID, dwMsg );
}

void CSelectClubList::SetClubListOrder( wb::search::EM_ORDER_TYPE Type, wb::search::EM_ORDER_ASC_DSC Order )
{
    if ( m_OrderType != Type )
        Order = wb::search::ASC;

    if ( m_OrderType != m_pNameField->GetFieldType() )
        SetSearchName( std::string("") );

    m_OrderType = Type;
    m_OrderASC_DSC = Order;

    m_pNameField->ResetOrder();
    m_pRankField->ResetOrder();
    m_pLevelField->ResetOrder();
    m_pMasterField->ResetOrder();
    m_pMemberField->ResetOrder();

    if ( m_pNameField->GetFieldType() == Type )
    {
        m_pNameField->SetFieldOrder( Order );
    }
    else if ( m_pRankField->GetFieldType() == Type )
    {
        m_pRankField->SetFieldOrder( Order );
    }
    else if ( m_pLevelField->GetFieldType() == Type )
    {
        m_pLevelField->SetFieldOrder( Order );
    }
    else if ( m_pMasterField->GetFieldType() == Type )
    {
        m_pMasterField->SetFieldOrder( Order );
    }
    else if ( m_pMemberField->GetFieldType() == Type )
    {
        m_pMemberField->SetFieldOrder( Order );
    }
}

void CSelectClubList::SearchClub()
{
    std::vector<wb::search::OrderBy> Order;
    wb::search::OrderBy sOrderBy;

    sOrderBy.m_Type = m_OrderType;
    sOrderBy.m_AscDsc = m_OrderASC_DSC;
    Order.push_back( sOrderBy );

    m_vecClubInfo.clear();
    m_pGaeaClient->SearchClubCountry( strSearchName, Order, m_vecClubInfo );

    m_pNameField->ClearList();
    m_pRankField->ClearList();
    m_pLevelField->ClearList();
    m_pMasterField->ClearList();
    m_pMemberField->ClearList();

    int nClubInfoSize = static_cast<int>( m_vecClubInfo.size() );
    for ( int i = 0; i < nClubInfoSize; i++ )
    {
        wb::CLUB_INFO_BASE& ClubInfo = m_vecClubInfo[ i ];

        std::string strText;
        m_pNameField->AddList( ClubInfo.m_ClubName );

        strText = sc::string::format( "%d", ClubInfo.m_ClubRank );
        m_pRankField->AddList( strText );

        strText = sc::string::format( "%d", ClubInfo.m_MasterLevel );
        m_pLevelField->AddList( strText );

        m_pMasterField->AddList( ClubInfo.m_MasterName );

        strText = sc::string::format( "%d", ClubInfo.m_MemberNum );
        m_pMemberField->AddList( strText );
    }
}

void CSelectClubList::SelectClubList( int nIndex )
{
    m_pNameField->SetSelectPos( nIndex );
    m_pRankField->SetSelectPos( nIndex );
    m_pLevelField->SetSelectPos( nIndex );
    m_pMasterField->SetSelectPos( nIndex );
    m_pMemberField->SetSelectPos( nIndex );
}

const std::vector<wb::CLUB_INFO_BASE>& CSelectClubList::GetClubList()
{
    return m_vecClubInfo;
}

void CSelectClubList::SetSearchName( std::string& strName )
{
    strSearchName = strName;
}
