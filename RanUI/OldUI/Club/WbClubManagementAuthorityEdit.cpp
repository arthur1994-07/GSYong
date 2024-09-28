#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../InnerInterface.h"
#include "./WbClubAuthorityEditRow.h"
#include "./WbClubManagementAuthorityEdit.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace
{
    const char* ID2WORD_WB_CLUB_WINDOW_AUTHORITY_EDIT_TITLE = "클럽 권한설정";
}

const float CWbClubManagementAuthorityEdit::fROW_OFFSET = 0.0f;
const float CWbClubManagementAuthorityEdit::fCOL_OFFSET = 0.0f;

CWbClubManagementAuthorityEdit::CWbClubManagementAuthorityEdit(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pOkBtn(NULL)
    , m_pCancelBtn(NULL)
{
    m_pVecAuthorityEditRow.clear();

    ID2WORD_WB_CLUB_WINDOW_AUTHORITY_EDIT_TITLE = ID2GAMEWORD("WB_CLUB_WINDOW_AUTHORITY_EDIT_TITLE", 0);

    for ( int i = 0; i < club::AUTHORITY_GRADE; i++ )
        m_pGradeName[i] = NULL;
}

CWbClubManagementAuthorityEdit::~CWbClubManagementAuthorityEdit()
{
}

void CWbClubManagementAuthorityEdit::CreateSubControl()
{
    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_AUTHORITY_EDIT_BOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "ITEM_INFOR_TOOLTIP_LINE" );
        RegisterControl( pLinBoxSmart );
    }

    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "WB_CLUB_WINDOW_AUTHORITY_EDIT_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "WB_CLUB_WINDOW_TAB_BASIC_INFO_BACK_LINE" );
        RegisterControl( pLinBoxSmart );
    }

    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "WB_CLUB_WINDOW_AUTHORITY_EDIT_BACK_1", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "POINT_SHOP_ITEM_SEARCH_BACK_LINE" );
        //pLinBoxSmart->CreateSubControl( "SELCET_CLUB_LIST_FIELD_LINE" );
        RegisterControl( pLinBoxSmart );

        pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "WB_CLUB_WINDOW_AUTHORITY_EDIT_BACK_2", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "POINT_SHOP_ITEM_SEARCH_BACK_LINE" );
        RegisterControl( pLinBoxSmart );
    }

    {
        CD3DFontPar* pFont9Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "WB_CLUB_WINDOW_AUTHORITY_EDIT_TITLE" );
        pTextBox->SetFont( pFont9Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        pTextBox->SetText( ID2WORD_WB_CLUB_WINDOW_AUTHORITY_EDIT_TITLE, NS_UITEXTCOLOR::WHITE );
    }

    {
        CD3DFontPar* pFont8Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 8, _DEFAULT_FONT_SHADOW_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "WB_CLUB_WINDOW_AUTHORITY_EDIT_GRADE_NAME" );
        pTextBox->SetFont( pFont8Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pGradeName[0] = pTextBox;

        for ( int i = 1; i < club::AUTHORITY_GRADE; i++ )
        {
            CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
            pTextBox->CreateSub( this, "WB_CLUB_WINDOW_AUTHORITY_EDIT_GRADE_NAME" );
            pTextBox->SetFont( pFont8Shadow );
            pTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
            RegisterControl( pTextBox );
            m_pGradeName[i] = pTextBox;

            D3DXVECTOR2 vPos;
            vPos.x = m_pGradeName[0]->GetLocalPos().left;
            vPos.y = m_pGradeName[0]->GetLocalPos().top;

            vPos.x += (m_pGradeName[0]->GetLocalPos().sizeX + fCOL_OFFSET) * i;
            m_pGradeName[i]->SetLocalPos( vPos );
        }
    }

    {
        CWbClubAuthorityEditRow* pAuthorityEditRow = new CWbClubAuthorityEditRow( m_pGaeaClient, m_pInterface, m_pEngineDevice );
        pAuthorityEditRow->CreateSub( this, "WB_CLUB_WINDOW_AUTHORITY_EDIT_ROW" );
        pAuthorityEditRow->CreateSubControl();
        RegisterControl( pAuthorityEditRow );
        m_pVecAuthorityEditRow.push_back( pAuthorityEditRow );

        size_t ClubAuthoSize = m_pGaeaClient->GetClubAuthDescSize();
        for ( size_t i = 1; i < ClubAuthoSize; i++ )
        {
            CWbClubAuthorityEditRow* pAuthorityEditRow = new CWbClubAuthorityEditRow( m_pGaeaClient, m_pInterface, m_pEngineDevice );
            pAuthorityEditRow->CreateSub( this, "WB_CLUB_WINDOW_AUTHORITY_EDIT_ROW" );
            pAuthorityEditRow->CreateSubControl();
            RegisterControl( pAuthorityEditRow );
            m_pVecAuthorityEditRow.push_back( pAuthorityEditRow );

            D3DXVECTOR2 vPos;
            vPos.x = m_pVecAuthorityEditRow[0]->GetLocalPos().left;
            vPos.y = m_pVecAuthorityEditRow[0]->GetLocalPos().top;

            vPos.y += (m_pVecAuthorityEditRow[0]->GetLocalPos().sizeY + fROW_OFFSET) * i;
            m_pVecAuthorityEditRow[i]->SetLocalPos( vPos );
        }
    }

    {
        RnButton* pRnButton = NULL;
        CD3DFontPar* pFont8Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 8, _DEFAULT_FONT_SHADOW_FLAG );

        RnButton::CreateArg arg;
        //////////////////////////////////////////////////////////////////////////
        arg.defaultTextureKey = "RNBUTTON_DEFAULT_TEXTURE";
        arg.mouseOverTextureKey = "RNBUTTON_OVER_TEXTURE";
        arg.mouseClickTextureKey = "RNBUTTON_CLICK_TEXTURE";

        arg.pFont = pFont8Shadow;
        arg.dwColor = NS_UITEXTCOLOR::WHITE;
        //////////////////////////////////////////////////////////////////////////

        arg.text = ID2GAMEWORD("WB_CLUB_WINDOW_MANAGEMENT_TAB", 8);
        pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this, "WB_CLUB_WINDOW_AUTHORITY_EDIT_OK_BTN", UI_FLAG_DEFAULT, OK_BTN );
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
        m_pOkBtn = pRnButton;

        arg.text = ID2GAMEWORD("WB_CLUB_WINDOW_MANAGEMENT_TAB", 7);
        pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this, "WB_CLUB_WINDOW_AUTHORITY_EDIT_CANCEL_BTN", UI_FLAG_DEFAULT, CANCEL_BTN );
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
        m_pCancelBtn = pRnButton;
    }
}

void CWbClubManagementAuthorityEdit::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CWbClubManagementAuthorityEdit::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    CUIGroup::TranslateUIMessage( ControlID, dwMsg );

    switch ( ControlID )
    {
    case OK_BTN:
        {
            if ( dwMsg & UIMSG_RNBUTTON_CLICK )
            {
                SaveAuthorities();
                SetVisibleSingle( FALSE );
            }
        }
        break;

    case CANCEL_BTN:
        {
            if ( dwMsg & UIMSG_RNBUTTON_CLICK )
                SetVisibleSingle( FALSE );
        }
        break;
    };
}

void CWbClubManagementAuthorityEdit::SetVisibleSingle( BOOL bVisible )
{
    CUIGroup::SetVisibleSingle( bVisible );

    if ( bVisible == TRUE )
    {
        Refresh();
    }
}

void CWbClubManagementAuthorityEdit::Refresh()
{
    size_t ClubAuthoSize = m_pGaeaClient->GetClubAuthDescSize();
    size_t ClubGradeSize = m_pGaeaClient->GetClubGradeSize();

    for ( size_t i = 0; i < m_pVecAuthorityEditRow.size() && i < ClubAuthoSize; i++ )
    {
        const std::string& strAuthorityName = m_pGaeaClient->GetClubAuthDesc( i );

        CWbClubAuthorityEditRow* pAuthorityEditRow = m_pVecAuthorityEditRow[i];
        pAuthorityEditRow->InitAuthorityData( strAuthorityName );
    }

    for ( size_t i = 0; i < ClubGradeSize; i++ )
    {
        const std::string& strGrameName = m_pGaeaClient->GetClubGradeName( i );
        m_pGradeName[i]->SetText( strGrameName.c_str(), NS_UITEXTCOLOR::WHITE );

        CheckAuthorities( i );
    }
}

void CWbClubManagementAuthorityEdit::CheckAuthorities( size_t nGradeIndex )
{
    DWORD dwGradeFlag = m_pGaeaClient->ClubGradeFlagGet( nGradeIndex );

    size_t ClubAuthoSize = m_pGaeaClient->GetClubAuthDescSize();
    for ( size_t nAuthorityIndex = 0; nAuthorityIndex < m_pVecAuthorityEditRow.size() && nAuthorityIndex < ClubAuthoSize; nAuthorityIndex++ )
    {
        if ( dwGradeFlag & GetFlagByAuthorityIndex(nAuthorityIndex) )
            m_pVecAuthorityEditRow[nAuthorityIndex]->CheckAuthority( nGradeIndex );
    }
}

club::AUTHORITY CWbClubManagementAuthorityEdit::GetFlagByAuthorityIndex( size_t nAuthorityIndex )
{
    if ( nAuthorityIndex >= m_pGaeaClient->GetClubAuthDescSize() )
        return club::AUTHORITY_NONE;

    if ( nAuthorityIndex == 0 )
        return club::AUTHORITY_MEMBER_JOIN; //! 클럽에 가입 시킬 수 있는 권한
    if ( nAuthorityIndex == 1 )
        return club::AUTHORITY_MEMBER_KICK; //! 클럽에서 제명 시킬 수 있는 권한
    if ( nAuthorityIndex == 2 )
        return club::AUTHORITY_NOTICE_CHANGE; //! 클럽 공지를 작성할 수 있는 권한    
    if ( nAuthorityIndex == 3 )
        return club::AUTHORITY_MARK_CHANGE; //! 클럽 마크를 변경할 수 있는 권한
    if ( nAuthorityIndex == 4 )
        return club::AUTHORITY_DEATH_MATCH; //! CDM 참가 자격
    if ( nAuthorityIndex == 5 )
        return club::AUTHORITY_RANK_UP; //! 클럽 랭크업
    if ( nAuthorityIndex == 6 )
        return club::AUTHORITY_CHANGE; //! 클럽원 계급 편집/변경
    if ( nAuthorityIndex == 7 )
        return club::AUTHORITY_ITEM; //! 클럽 창고/아이템 넣기/꺼내기
    if ( nAuthorityIndex == 8 )
        return club::AUTHORITY_MONEY; //! 클럽 창고/입출금
    if ( nAuthorityIndex == 9 )
        return club::AUTHORITY_ALLIANCE; //! 클럽 동맹 신청하기/받기
    if ( nAuthorityIndex == 10 )
        return club::AUTHORITY_ALLIANCE_DIS; //! 클럽 동맹 해지
    if ( nAuthorityIndex == 11 )
        return club::AUTHORITY_CLUB_BATTLE; //! 클럽 배틀 신청 하기/받기/항복/휴전

    return club::AUTHORITY_NONE;
}

void CWbClubManagementAuthorityEdit::SaveAuthorities()
{
    size_t ClubGradeSize = m_pGaeaClient->GetClubGradeSize();
    size_t ClubAuthoSize = m_pGaeaClient->GetClubAuthDescSize();

    std::vector <club::GRADE_FLAG> vFlag;
    for ( size_t nGradeIndex = 0; nGradeIndex < ClubGradeSize; nGradeIndex++ )
    {
        DWORD dwGradeFlag = club::AUTHORITY_NONE;

        for ( size_t nAuthorityIndex = 0; nAuthorityIndex < m_pVecAuthorityEditRow.size() && nAuthorityIndex < ClubAuthoSize; nAuthorityIndex++ )
        {
            if ( m_pVecAuthorityEditRow[nAuthorityIndex]->IsAuthorityChecked( nGradeIndex ) )
            {
                dwGradeFlag |= GetFlagByAuthorityIndex( nAuthorityIndex );
            }
        }
        club::GRADE_FLAG Data(nGradeIndex, dwGradeFlag);
        vFlag.push_back(Data);
    }

    m_pGaeaClient->ClubGradeFlagChangeCA(vFlag);
}
