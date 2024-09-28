#include "StdAfx.h"
#include "./RnFriendSlot.h"
#include "./RnFriendTab.h"

#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"

#include "../../../RanLogic/Friend/Friend.h"
#include "../../../RanLogic/Character/GLCharDefine.h"

#include "../../../RanLogicClient/GLGaeaClient.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------
namespace
{
    const char* strGroupNameNORMAL = "일반";
}

RnFriendSlot::RnFriendSlot( GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice )
    : CUIGroupHelper( pEngineDevice )
    , m_pGaeaClient( pGaeaClient )
    , m_pItemBG( NULL )
    , m_pIcon_PartyState( NULL )
    , m_pButton_GroupClose( NULL )
    , m_pButton_GroupOpen( NULL )
    , m_pIcon_GroupImgOpen( NULL )
    , m_pIcon_GroupImgClose( NULL )
    , m_pText_GroupName( NULL )
    , m_pText_Level( NULL )
    , m_pText_CharName( NULL )
    , m_pText_MapName( NULL )
    , m_pText_Coord( NULL )
    , m_pText_Channel( NULL )
    , m_eSlotState( SLOT_EMPTY )
    , m_sDataPos( SDATA_POS() )
{
    strGroupNameNORMAL   = ID2GAMEWORD("RNFRIEND_FRIENDTAB_SLOT_NORMAL", 0);
    for( int i=0 ; i<GLCI_NUM_TRICKER ; i++)
    {
        m_pClassIcon[i][ICON_ONLINE] = NULL;
        m_pClassIcon[i][ICON_OFFLINE] = NULL;
    }
}

void RnFriendSlot::CreateSubControl()
{
    CD3DFontPar* pFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

    m_pItemBG = CreateLineBox( "RNFRIEND_FRIEND_ROW_SIZE", "RNFRIEND_TEXTURE_NOLINE_GRAY_BG" );

    {
        // 파티여부 아이콘
        m_pIcon_PartyState = new CBasicLineBoxSmart(GetEngineDeviceMan());
        m_pIcon_PartyState->CreateSub( this, "RNFRIEND_ITEM_PARTYSTATE", UI_FLAG_DEFAULT );
        m_pIcon_PartyState->CreateSubControl( "party_lga" );
        RegisterControl(m_pIcon_PartyState);
       
        CString strClassOff[GLCI_NUM_TRICKER] =
        {
            "gyouk_msg_lgc",
            "gum_msg_lgc",
            "yang_wsg_lgc",
            "gi_wsg_lgc",
            "guk_msg_lgc",
            "guk_wsg_lgc",
            "gyouk_wsg_lgc",
            "gum_wsg_lgc",
            "yang_msg_lgc",
            "gi_msg_lgc",
            "gua_msg_lgc",
            "gua_wsg_lgc",
            "nin_msg_lgc",
            "nin_wsg_lgc",
			"tri_msg_lgc",
			"tri_wsg_lgc",
        };
        CString strClassON[GLCI_NUM_TRICKER] =
        {
            "gyouk_ms_lgc",
            "gum_ms_lgc",
            "yang_ws_lgc",
            "gi_ws_lgc",
            "guk_ms_lgc",
            "guk_ws_lgc",
            "gyouk_ws_lgc",
            "gum_ws_lgc",
            "yang_ms_lgc",
            "gi_ms_lgc",
            "gua_ms_lgc",
            "gua_ws_lgc",
            "nin_ms_lga",
            "nin_ws_lga",
			"tri_ms_lga",
			"tri_ws_lga",
        };

        for( int i=0 ; i<GLCI_NUM_TRICKER ; i++)
        {
            // 캐릭터 직업 아이콘 오프라인
            CBasicLineBoxSmart* m_pIcon = new CBasicLineBoxSmart( GetEngineDeviceMan() );
            m_pIcon->CreateSub( this, "RNFRIEND_ITEM_CLASS", UI_FLAG_DEFAULT );
            m_pIcon->CreateSubControl( strClassOff[i].GetString() );
            RegisterControl(m_pIcon);
            m_pClassIcon[i][ICON_OFFLINE] = m_pIcon;
        }
        for( int i=0 ; i<GLCI_NUM_TRICKER ; i++)
        {
            // 캐릭터 직업 아이콘 온라인
            CBasicLineBoxSmart* m_pIcon = new CBasicLineBoxSmart( GetEngineDeviceMan() );
            m_pIcon->CreateSub( this, "RNFRIEND_ITEM_CLASS", UI_FLAG_DEFAULT );
            m_pIcon->CreateSubControl( strClassON[i].GetString() );
            RegisterControl(m_pIcon);
            m_pClassIcon[i][ICON_ONLINE] = m_pIcon;
        }




    }

    {
        // 축소 상태
        RnButton::CreateArg arg;
        arg.defaultTextureKey = "reduction_lga";
        arg.mouseOverTextureKey = "reduction_over_lga";
        arg.mouseClickTextureKey = "reduction_lga";
        
        m_pButton_GroupClose = new RnButton( GetEngineDeviceMan() );
        m_pButton_GroupClose->CreateSub(this,"RNFRIEND_ITEM_GROUPARROW",UI_FLAG_DEFAULT, ID_BTN_REDUCTION );
        m_pButton_GroupClose->CreateSubControl(arg);
        RegisterControl(m_pButton_GroupClose);

        //확장 상태
        arg.defaultTextureKey = "expansion_lga";
        arg.mouseOverTextureKey = "expansion_over_lga";
        arg.mouseClickTextureKey = "expansion_lga";

        m_pButton_GroupOpen = new RnButton( GetEngineDeviceMan() );
        m_pButton_GroupOpen->CreateSub(this,"RNFRIEND_ITEM_GROUPARROW",UI_FLAG_DEFAULT, ID_BTN_EXPANSION );
        m_pButton_GroupOpen->CreateSubControl(arg);
        RegisterControl(m_pButton_GroupOpen);

        // 그룹 아이콘 열림
        m_pIcon_GroupImgOpen = new CBasicLineBoxSmart(GetEngineDeviceMan());
        m_pIcon_GroupImgOpen->CreateSub( this, "RNFRIEND_ITEM_GROUPIMG",UI_FLAG_DEFAULT );
        m_pIcon_GroupImgOpen->CreateSubControl( "group_lga" );
        RegisterControl(m_pIcon_GroupImgOpen);

        // 그룹 아이콘 닫힘
        m_pIcon_GroupImgClose = new CBasicLineBoxSmart(GetEngineDeviceMan());
        m_pIcon_GroupImgClose->CreateSub( this, "RNFRIEND_ITEM_GROUPIMG",UI_FLAG_DEFAULT );
        m_pIcon_GroupImgClose->CreateSubControl( "group_open_lga" );
        RegisterControl(m_pIcon_GroupImgClose);

        // 그룹 이름
        m_pText_GroupName = new CBasicTextBox(GetEngineDeviceMan());
        m_pText_GroupName->CreateSub ( this, "RNFRIEND_ITEM_GROUPNAME", UI_FLAG_DEFAULT );
        m_pText_GroupName->SetFont ( pFont9 );
        m_pText_GroupName->SetTextAlign ( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );	
        RegisterControl ( m_pText_GroupName );
    }


    {
        // 레벨
        m_pText_Level = new CBasicTextBox(GetEngineDeviceMan());
        m_pText_Level->CreateSub ( this, "RNFRIEND_ITEM_LEVEL", UI_FLAG_DEFAULT );
        m_pText_Level->SetFont ( pFont9 );
        m_pText_Level->SetTextAlign ( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );	
        RegisterControl ( m_pText_Level );
    }


    {
        // 캐릭터 명
        m_pText_CharName = new CBasicTextBox(GetEngineDeviceMan());
        m_pText_CharName->CreateSub ( this, "RNFRIEND_ITEM_CHARNAME", UI_FLAG_DEFAULT );
        m_pText_CharName->SetFont ( pFont9 );
        m_pText_CharName->SetTextAlign ( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );	
        RegisterControl ( m_pText_CharName );
    }

    {
        // 맵 이름
        m_pText_MapName = new CBasicTextBox(GetEngineDeviceMan());
        m_pText_MapName->CreateSub ( this, "RNFRIEND_ITEM_MAPNAME", UI_FLAG_DEFAULT );
        m_pText_MapName->SetFont ( pFont9 );
        m_pText_MapName->SetTextAlign ( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );	
        RegisterControl ( m_pText_MapName );
    }

    {
        // 좌표
        m_pText_Coord = new CBasicTextBox(GetEngineDeviceMan());
        m_pText_Coord->CreateSub ( this, "RNFRIEND_ITEM_COORD", UI_FLAG_DEFAULT );
        m_pText_Coord->SetFont ( pFont9 );
        m_pText_Coord->SetTextAlign ( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );	
        RegisterControl ( m_pText_Coord );
    }

    {
        // 채널
        m_pText_Channel = new CBasicTextBox(GetEngineDeviceMan());
        m_pText_Channel->CreateSub ( this, "RNFRIEND_ITEM_CHANNEL", UI_FLAG_DEFAULT );
        m_pText_Channel->SetFont ( pFont9 );
        m_pText_Channel->SetTextAlign ( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );	
        RegisterControl ( m_pText_Channel );
    }

    Reset();

}

void RnFriendSlot::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    switch( ControlID )
    {
    case ID_BTN_EXPANSION:
        if( CHECK_MOUSE_IN ( dwMsg ) && CHECK_LB_UP_LIKE ( dwMsg ))
            AddMessageEx( UIMSG_EXPANSION_CLICK );
        break;
    case ID_BTN_REDUCTION:
        if( CHECK_MOUSE_IN ( dwMsg ) && CHECK_LB_UP_LIKE ( dwMsg ))
            AddMessageEx( UIMSG_REDUCTION_CLICK );
        break;
    }
}


void RnFriendSlot::Reset()
{
    m_pText_GroupName->ClearText();
    m_pText_Level->ClearText();
    m_pText_CharName->ClearText();
    m_pText_MapName->ClearText();
    m_pText_Coord->ClearText();
    m_pText_Channel->ClearText();

    m_pItemBG->SetDiffuseAlpha( 255 );

    m_pIcon_PartyState->SetVisibleSingle( FALSE );
    m_pButton_GroupClose->SetVisibleSingle( FALSE );
    m_pButton_GroupOpen->SetVisibleSingle( FALSE );
    m_pIcon_GroupImgClose->SetVisibleSingle( FALSE );
    m_pIcon_GroupImgOpen->SetVisibleSingle( FALSE );

    m_sDataPos = SDATA_POS();

    m_eSlotState = SLOT_EMPTY;
    m_PartyID.Reset();
    for( int i=0 ; i<GLCI_NUM_TRICKER ; i++)
    {
        m_pClassIcon[i][ICON_ONLINE]->SetVisibleSingle( FALSE );
        m_pClassIcon[i][ICON_OFFLINE]->SetVisibleSingle( FALSE );
    }
}

RnFriendSlot::eSlotState RnFriendSlot::GetSlotState()
{
    return m_eSlotState;
}

RnFriendSlot::SDATA_POS RnFriendSlot::GetDataPos()
{
    return m_sDataPos;
}

WORD RnFriendSlot::GetGroupNum()
{
    return m_sDataPos.wGroupId;
}

std::string RnFriendSlot::GetGroupName()
{
    std::string strGroupName = m_pGaeaClient->FriendGetGroupName( m_sDataPos.wGroupId );
    if( strGroupName.empty() == true )
        strGroupName = strGroupNameNORMAL;

    return strGroupName;
}

const char* RnFriendSlot::GetCharName()
{
    if( m_eSlotState == SLOT_FRIEND )
        return m_strCharName.c_str();

    return NULL;
}

EMCHARINDEX RnFriendSlot::GetGLCCToGLCI( EMCHARCLASS eGLCC )
{
    unsigned int eGLCI = 0;
    switch( eGLCC )
    {
    case GLCC_ETC_W:
        eGLCI++;
    case GLCC_ETC_M:
        eGLCI++;
	case GLCC_TRICKER_W:
		eGLCI++;
	case GLCC_TRICKER_M:
		eGLCI++;
    case GLCC_ASSASSIN_W:
        eGLCI++;
    case GLCC_ASSASSIN_M:
        eGLCI++;
    case GLCC_SCIENTIST_W:
        eGLCI++;
    case GLCC_SCIENTIST_M:
        eGLCI++;
    case GLCC_SPIRIT_M:
        eGLCI++;
    case GLCC_ARCHER_M:
        eGLCI++;
    case GLCC_ARMS_W:
        eGLCI++;
    case GLCC_FIGHTER_W:
        eGLCI++;
    case GLCC_EXTREME_W:
        eGLCI++;
    case GLCC_EXTREME_M:
        eGLCI++;
    case GLCC_SPIRIT_W:
        eGLCI++;
    case GLCC_ARCHER_W:
        eGLCI++;
    case GLCC_ARMS_M:
        eGLCI++;
    case GLCC_FIGHTER_M:
        break;
    default:
        return GLCI_FIGHTER_M;
    }

    return (EMCHARINDEX)eGLCI;
}

const GLPartyID& RnFriendSlot::GetPartyID(void) const
{
    return m_PartyID;
}

void RnFriendSlot::SetData( const SDATA_POS sDataPos, const FRIEND_PTR_VECTOR& vecFriendPtr, bool bOpen/* = true */ )
{
    Reset();
    m_eSlotState = SLOT_GROUP;
    m_sDataPos = sDataPos;

    int nMax=0, Online=0;
    BOOST_FOREACH( const std::tr1::shared_ptr<SFRIEND> pFriend, vecFriendPtr )
    {
        if( pFriend == NULL )
            continue;

        if( pFriend->m_Online == true )
            Online++;

        nMax++;
    }

    CString strTemp;
    strTemp.Format(" %s (%d/%d)", GetGroupName().c_str(), Online, nMax );

    // 그룹인경우 이름이 대체된다.
    m_pText_GroupName->SetText( strTemp.GetString() );



    if( bOpen == true )
    {
        if( 0 < nMax )
        {
            m_pButton_GroupClose->SetVisibleSingle( TRUE );
            m_pIcon_GroupImgOpen->SetVisibleSingle( TRUE );
        }
        else
        {
            m_pIcon_GroupImgClose->SetVisibleSingle( TRUE );
        }
    }
    else
    {
        m_pIcon_GroupImgClose->SetVisibleSingle( TRUE );

        if( 0 < nMax )
            m_pButton_GroupOpen->SetVisibleSingle( TRUE );
    }


    
    m_pItemBG->SetDiffuseAlpha( 90 );

    
}

void RnFriendSlot::SetData( const SDATA_POS sDataPos, const std::tr1::shared_ptr<SFRIEND> pFriend )
{

    Reset();
    m_eSlotState = SLOT_FRIEND;
    m_sDataPos = sDataPos;


    m_pItemBG->SetDiffuseAlpha( 255 );
    int nIConIndex = GetGLCCToGLCI( (EMCHARCLASS)pFriend->m_ChaClass );
    

    DWORD dwFontColor = NS_UITEXTCOLOR::WHITE;
    if (pFriend->m_Online)
    {
        m_pClassIcon[nIConIndex][1]->SetVisibleSingle( TRUE );
        
        if ( pFriend->m_PartyID.isValidParty() )
        {
            m_pIcon_PartyState->SetVisibleSingle( TRUE );
            m_PartyID = pFriend->m_PartyID;
        }
        else
        {
            m_pIcon_PartyState->SetVisibleSingle( FALSE );
        }
        m_pText_MapName->SetText( m_pGaeaClient->GetMapName( pFriend->m_ChaMap.getBaseMapID() ) );

        CString strCoord;
        strCoord.Format("%03d/%03d", pFriend->m_PosX, pFriend->m_PosY );
        m_pText_Coord->SetVisibleSingle( TRUE );
        m_pText_Coord->SetText( strCoord );

        CString strChannel;
        strChannel.Format("[%d]", pFriend->m_Channel);
        m_pText_Channel->SetText( strChannel );
    }
    else
    {
        dwFontColor = NS_UITEXTCOLOR::DARKGRAY;
        m_pClassIcon[nIConIndex][0]->SetVisibleSingle( TRUE );
    }


    CString strLevel;
    strLevel.Format("Lv%d", pFriend->m_ChaLevel );

    m_pText_Level->SetText( strLevel, dwFontColor );
    m_pText_CharName->SetText( pFriend->m_ChaName.c_str(), dwFontColor );
}
