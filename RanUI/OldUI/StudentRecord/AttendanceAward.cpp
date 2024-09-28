#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"

#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLogicDataMan.h"
#include "../../../RanLogic/Attendance/TaskN.h"

#include "../Item/ItemImageEx.h"
#include "../../InnerInterface.h"

#include "./AttendanceAward.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CAttendanceAward::CAttendanceAward(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient ( pGaeaClient )
    , m_pInterface  ( pInterface )
    , m_pAward      (NULL)
    , m_pAwardItem  (NULL)
    , m_nEXP        (0)
    , m_nActivity   (0)
    , m_llMoney     (0)
    , m_nNumItem    (0)
    , m_emState     (EMSTATE_DISABLE)
{
}

CAttendanceAward::~CAttendanceAward ()
{
    //Blank
}

void CAttendanceAward::CreateSubControl ()
{
    CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

    // 보상 점수
    {
        CBasicTextBox* pTextBox;

        pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "ATTENDANCE_PAGE_CONTENTS_TASK_AWARD_TEXT_POINT", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pAward = pTextBox;
    }

    // 보상 아이템
    {
        CBasicTextBox* pTextBox;

        pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "ATTENDANCE_PAGE_CONTENTS_TASK_AWARD_TEXT_ITEM_BASE", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont );
        pTextBox->SetTextAlign( TEXT_ALIGN_RIGHT | TEXT_ALIGN_CENTER_Y );
        pTextBox->SetOneLineText( ID2GAMEWORD("ATTENDANCE_AWARD_TEXT", 0) );
        RegisterControl( pTextBox );
        m_pAwardItem = pTextBox;

        for ( UINT i=0; i<MAX_ITEMS_PER_ONE_TASK; ++i )
        {
            SITEMSLOT* pItemSlot = CreateItemSlot (i);
            m_ItemList.push_back( pItemSlot );
        }

        ResetItem ();
    }
}

void CAttendanceAward::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CAttendanceAward::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
    CUIGroup::TranslateUIMessage ( ControlID, dwMsg );
}

void CAttendanceAward::Update ( TaskN* pTask )
{
    // 점수 설정
    SetPoint ( pTask->GetRewardExp(), pTask->GetRewardPoint(), (LONGLONG)pTask->GetRewardMoney() );

    // 아이템 리셋
    ResetItem ();

	INT nMaxItem = (INT)pTask->GetNumberOfRewardItems();

    for ( INT i=nMaxItem; i>0; --i )
    {
        SNATIVEID sItemID = (DWORD)pTask->GetRewardItem( i-1 );

        if ( sItemID == NATIVEID_NULL() )
        {
            continue;
        }

        AddItem( sItemID );
    }
}

void CAttendanceAward::SetState ( const EMSTATE emState )
{
    m_emState = emState;
    UpdateState ();
}

void CAttendanceAward::UpdateState ()
{
    SetVisibleSingle( TRUE );

    // 상태에 따른 업데이트
    switch ( m_emState )
    {
    case EMSTATE_ENABLE:
        {
            m_pAward->SetTextColor( 0, NS_UITEXTCOLOR::WHITE );
            m_pAward->SetUseTextColor( 0, TRUE );

            m_pAwardItem->SetTextColor( 0, NS_UITEXTCOLOR::WHITE );
            m_pAwardItem->SetUseTextColor( 0, TRUE );

            ITEMVEC_ITER iter = m_ItemList.begin();
            for ( ; iter != m_ItemList.end(); ++iter ) { (*iter)->SetEnable ( true ); }
        }
        break;

    case EMSTATE_COMPLETE:
    case EMSTATE_DISABLE:
        {
            m_pAward->SetTextColor( 0, NS_UITEXTCOLOR::DIMGREY );
            m_pAward->SetUseTextColor( 0, TRUE );

            m_pAwardItem->SetTextColor( 0, NS_UITEXTCOLOR::DIMGREY );
            m_pAwardItem->SetUseTextColor( 0, TRUE );

            ITEMVEC_ITER iter = m_ItemList.begin();
            for ( ; iter != m_ItemList.end(); ++iter ) { (*iter)->SetEnable ( false ); }
        }
        break;

    case EMSTATE_EMPTY:
        {
            SetVisibleSingle( FALSE );
        }
        break;
    };
}

void CAttendanceAward::SetPoint ( const UINT nEXP, const UINT nActivity, const ULONGLONG llMoney )
{
    m_nEXP      = nEXP;
    m_nActivity = nActivity;
    m_llMoney   = llMoney;

    std::string strAward("");

    if ( m_nEXP )
    {
        strAward += sc::string::format( ID2GAMEWORD("ATTENDANCE_AWARD_TEXT", 1), m_nEXP);
        strAward += "  ";
    }

    if ( m_nActivity )
    {
        strAward += sc::string::format( ID2GAMEWORD("ATTENDANCE_AWARD_TEXT", 2), m_nActivity);
        strAward += "  ";
    }

    if ( m_llMoney )
    {
        CString strMoney = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT ( (LONGLONG)m_llMoney, 3, "," );
        strAward += sc::string::format( ID2GAMEWORD("ATTENDANCE_AWARD_TEXT", 3), strMoney.GetString() );
        strAward += "  ";
    }

    m_pAward->ClearText();
    m_pAward->SetOneLineText( strAward.c_str() );
}

void CAttendanceAward::ResetItem ()
{
    const UIRECT& rcGlobalPos = GetGlobalPos();

    // Note : 아이템과 위치를 리셋
    {
        m_nNumItem = 0;

        ITEMVEC_ITER iter = m_ItemList.begin();
        for ( ; iter != m_ItemList.end(); ++iter )
        {
            SITEMSLOT* pItemSlot = (*iter);
            pItemSlot->Reset();
            pItemSlot->ResetControl( rcGlobalPos, "ATTENDANCE_PAGE_CONTENTS_TASK_AWARD_ITEMSLOT_BASE" );
            pItemSlot->SetVisibleSingle( FALSE );
        }
    }

    // Note : 텍스트의 위치를 리셋
    {
        m_pAwardItem->ResetControl( rcGlobalPos, "ATTENDANCE_PAGE_CONTENTS_TASK_AWARD_TEXT_ITEM_BASE" );
        m_pAwardItem->SetVisibleSingle( FALSE );
    }
}

const bool CAttendanceAward::AddItem ( const SNATIVEID sItemID )
{
    if ( (UINT)m_ItemList.size() <= m_nNumItem )
    {
        return false;
    }

    const SITEM* pItemData = GLogicData::GetInstance().GetItem ( sItemID );

    if ( !pItemData )
    {
        return false;
    }

    const UIRECT& rcGlobalPos = GetGlobalPos();

    // Gap Size의 컨트롤을 읽는다.
    CUIControl GapControl(m_pEngineDevice); 
    GapControl.Create ( 1, "ATTENDANCE_PAGE_CONTENTS_TASK_AWARD_ITEMSLOT_GAP" );
    const UIRECT& rcGapLocal = GapControl.GetLocalPos();

    // 아이템 설정
    SITEMSLOT* pItemSlot = m_ItemList[ m_nNumItem ];
    pItemSlot->ResetControl( rcGlobalPos, "ATTENDANCE_PAGE_CONTENTS_TASK_AWARD_ITEMSLOT_BASE" );
    pItemSlot->AddPosition( D3DXVECTOR2( -rcGapLocal.sizeX*(float)m_nNumItem, 0.0f ) );
    pItemSlot->SetVisibleSingle ( TRUE );

    CItemImageEx* pItem = pItemSlot->ITEM;
    pItem->SetItem    ( sItemID );
    pItem->SetTurnNum ( pItemData->GETAPPLYNUM() );

    pItemSlot->ITEMDATA.SetDefaultValue( pItemData );

    // 텍스트 설정
    m_pAwardItem->ResetControl ( rcGlobalPos, "ATTENDANCE_PAGE_CONTENTS_TASK_AWARD_TEXT_ITEM_BASE" );
    m_pAwardItem->AddPosition ( D3DXVECTOR2( -rcGapLocal.sizeX*(float)m_nNumItem, 0.0f ) );
    m_pAwardItem->SetVisibleSingle( TRUE );

    ++m_nNumItem;

    return true;
}

CAttendanceAward::SITEMSLOT* CAttendanceAward::CreateItemSlot ( const UINT nIndex )
{
    SITEMSLOT* pItemSlot = new SITEMSLOT( m_pGaeaClient, m_pInterface, m_pEngineDevice );
    pItemSlot->CreateSub ( this, "ATTENDANCE_PAGE_CONTENTS_TASK_AWARD_ITEMSLOT_BASE", UI_FLAG_DEFAULT, ATTENDANCE_AWARD_ITEMSLOT+nIndex );
    pItemSlot->CreateSubControl ();
    RegisterControl( pItemSlot );

    return pItemSlot;
}

//-----------------------------------------------------------------------------------------------//

void CAttendanceAward::SITEMSLOT::CreateSubControl ()
{
    LINEBOX = new CBasicLineBoxSmart(m_pEngineDevice);
    LINEBOX->CreateSub ( this, "ATTENDANCE_PAGE_CONTENTS_TASK_AWARD_ITEMSLOT_LINEBOX" );
    LINEBOX->CreateSubControl ( "ATTENDANCE_PAGE_CONTENTS_TASK_AWARD_ITEMSLOT_LINEBOX_TEXINFO" );
    RegisterControl ( LINEBOX );

    ITEM = new CItemImageEx ( GAEACLIENT, m_pEngineDevice );
    ITEM->CreateSub ( this, "ATTENDANCE_PAGE_CONTENTS_TASK_AWARD_ITEMSLOT_ITEM", UI_FLAG_DEFAULT, ID_ITEM );
    ITEM->CreateSubControl ();
    RegisterControl ( ITEM );

    Reset();
}

void CAttendanceAward::SITEMSLOT::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
    switch ( ControlID )
    {
    case ID_ITEM:
        {
            if ( CHECK_MOUSE_IN ( dwMsg ) )
            {
                if ( ITEMDATA.GetNativeID() == NATIVEID_NULL() )
                {
                    break;
                }

                INNERINTERFACE->SHOW_ITEM_INFO(
					ITEMDATA,
					FALSE, FALSE, FALSE, TRUE, FALSE,
					USHRT_MAX, USHRT_MAX,
					NATIVEID_NULL() );

                if ( CHECK_LB_UP_LIKE( dwMsg ) && (DxInputDevice::GetInstance().GetKeyState ( DIK_LALT ) & DXKEY_PRESSED) )
                {
                    SITEMCUSTOM& sItemcustom = ITEMDATA;

                    if ( INNERINTERFACE->ItemPreviewWindowIsPreviewItem( sItemcustom ) )
                    {
                        INNERINTERFACE->ItemPreviewWindowOpen( sItemcustom );
                    }                        
                }
                else if ( CHECK_RB_UP_LIKE( dwMsg ) && (DxInputDevice::GetInstance().GetKeyState ( DIK_LALT ) & DXKEY_PRESSED) )
                {
                    SITEMCUSTOM& sItemCustom = ITEMDATA;

                    const SITEM* pItem = GLItemMan::GetInstance().GetItem(sItemCustom.GetNativeID());
                    if (pItem->BasicType() == ITEM_BOX || pItem->BasicType() == ITEM_RANDOMITEM )
                    {
                        INNERINTERFACE->GetInvenItemTooltipGroup()->ShowItemToolTipMulti( sItemCustom, DxInputDevice::GetInstance().GetMouseLocateX(), DxInputDevice::GetInstance().GetMouseLocateY() );

                        ResetMessageEx();
                    }
                }
            }
            break;
        }
    }

    CUIGroup::TranslateUIMessage ( ControlID, dwMsg );
}

void CAttendanceAward::SITEMSLOT::SetEnable ( bool bEnable )
{
    ENABLE = bEnable;

    if ( ENABLE )
    {
        ITEM->SetDiffuse( NS_UITEXTCOLOR::WHITE );
    }
    else
    {
        ITEM->SetDiffuse( NS_UITEXTCOLOR::DIMGREY );
    }
}

void CAttendanceAward::SITEMSLOT::Reset ()
{
    ITEMDATA = SITEMCUSTOM( NATIVEID_NULL() );

    ITEM->SetTurnNum( 0 );
    ITEM->ResetItem();

    SetEnable ( false );
}

