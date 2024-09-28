#include "StdAfx.h"
#include "./ActivityListSlot.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/UITextUtil.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"

#include "../../../RanLogic/GLogicEx.h"
#include "../../../RanLogicClient/GLGaeaClient.h"


// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const float ActivityListSlot::fDESC_ANIMATION_BEGIN_DALAY = 2.0f;
const float ActivityListSlot::fDESC_ANIMATION_SPEED_DALAY = 0.5f;

ActivityListSlot::ActivityListSlot(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_Activity ( UINT_MAX, UINT_MAX, 0 )
    , m_pGaeaClient     ( pGaeaClient )
    , m_emSlotType      ( SPEC_ACT_SLOT_TYPE_IDLE )
    , m_bAniDesc        ( false )
    , m_nAniDescCur     ( 0 )
    , m_nAniDescMax     ( 0 )
    , m_fAniTime        ( 0.0f )
    , m_bMouseOver      ( false )
    , m_fMouseOverTime  ( 0.0f )
{
}

ActivityListSlot::~ActivityListSlot()
{
}
    
void ActivityListSlot::CreateSubControl ()
{
    CBasicLineBoxSmart* pLineControl = new CBasicLineBoxSmart(m_pEngineDevice);
    pLineControl->CreateSub( this, "SPEC_ACT_SLOT_ARRIVAL_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLineControl->CreateSubControl( "SPEC_ACT_SLOT_ARRIVAL_BACK_TEXINFO" );
	RegisterControl( pLineControl );

    m_pArrivalImage = new CUIControl(m_pEngineDevice);
    m_pArrivalImage->CreateSub( this, "SPEC_ACT_SLOT_ARRIVAL" );
    RegisterControl( m_pArrivalImage );
    m_pArrivalImage->SetVisibleSingle( FALSE );

    m_pAllImage = new CUIControl(m_pEngineDevice);
    m_pAllImage->CreateSub( this, "SPEC_ACT_SLOT_ALL" );
    RegisterControl( m_pAllImage );
    m_pAllImage->SetVisibleSingle( FALSE );


    for ( int i = 0; i < SPEC_ACT_SLOT_TYPE_SIZE; ++i ) 
    {
        CString strKey;
        CString strKeyTex;
        
        strKey.Format( "SPEC_ACT_SLOT_PRESENT_BACK%d", i );
        strKeyTex.Format( "SPEC_ACT_SLOT_PRESENT_BACK%d_TEXINFO", i );

        m_pPresentBack[i] = new CBasicLineBoxSmart(m_pEngineDevice);
        m_pPresentBack[i]->CreateSub( this, strKey, UI_FLAG_XSIZE | UI_FLAG_YSIZE, SPEC_ACT_SLOT_PRESENT + i );
        m_pPresentBack[i]->CreateSubControl( strKeyTex );
        RegisterControl( m_pPresentBack[i] );
        m_pPresentBack[i]->SetVisibleSingle( FALSE );
    }

    for ( int i = 0; i < SPEC_ACT_SLOT_TYPE_SIZE; ++i ) 
    {
        CString strKey;
        CString strKeyTex;

        strKey.Format( "SPEC_ACT_SLOT_REWARD_BACK%d", i );
        strKeyTex.Format( "SPEC_ACT_SLOT_REWARD_BACK%d_TEXINFO", i );

        m_pRewardBack[i] = new CBasicLineBoxSmart(m_pEngineDevice);
        m_pRewardBack[i]->CreateSub( this, strKey, UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        m_pRewardBack[i]->CreateSubControl( strKeyTex );
        RegisterControl( m_pRewardBack[i] );
        m_pRewardBack[i]->SetVisibleSingle( FALSE );
    }

    //  하이라이트 부분

    CUIControl* pControl = new CUIControl(m_pEngineDevice);
    pControl->CreateSub( this, "SPEC_ACT_SLOT_PRESENT_BACK_TOP" );
    RegisterControl( pControl );

    pControl = new CUIControl(m_pEngineDevice);
    pControl->CreateSub( this, "SPEC_ACT_SLOT_REWARD_BACK_TOP" );
    RegisterControl( pControl );


    pLineControl = new CBasicLineBoxSmart(m_pEngineDevice);
    pLineControl->CreateSub( this, "SPEC_ACT_SLOT_REWARD_BACK_INNER1", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLineControl->CreateSubControl( "SPEC_ACT_SLOT_REWARD_BACK_INNER_TEXINFO" );
    RegisterControl( pLineControl );
    m_pRewardTitleLineBox = pLineControl;
    
    pLineControl = new CBasicLineBoxSmart(m_pEngineDevice);
    pLineControl->CreateSub( this, "SPEC_ACT_SLOT_REWARD_BACK_INNER2", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLineControl->CreateSubControl( "SPEC_ACT_SLOT_REWARD_BACK_INNER_TEXINFO" );
	RegisterControl( pLineControl );
    m_pRewardPointLineBox = pLineControl;

    pControl = new CUIControl(m_pEngineDevice);
    pControl->CreateSub( this, "SPEC_ACT_SLOT_REWARD_TITLE_IMAGE" );
    RegisterControl( pControl );
    m_pRewardTitleImage = pControl;

    pControl = new CUIControl(m_pEngineDevice);
    pControl->CreateSub( this, "SPEC_ACT_SLOT_REWARD_POINT_IMAGE" );
    RegisterControl( pControl );
    m_pRewardPointImage = pControl;

    m_pComplateImage = new CUIControl(m_pEngineDevice);
    m_pComplateImage->CreateSub( this, "SPEC_ACT_SLOT_COMP_IMAGE" );
    RegisterControl( m_pComplateImage );
    m_pComplateImage->SetVisibleSingle( FALSE );
    

    CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
    
    m_pCompText = CreateStaticControl ( "SPEC_ACT_SLOT_COMP_TEXT", pFont9, NS_UITEXTCOLOR::BLACK, TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
    m_pCompText->SetOneLineText ( (char*)ID2GAMEWORD ( "SPEC_ACT_SLOT_TEXT", 1  ) );   

    m_pArrivalText = CreateStaticControl ( "SPEC_ACT_SLOT_ARRIVAL_TEXT", pFont9, NS_UITEXTCOLOR::BLACK, TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );

    m_pTitleText = CreateStaticControl ( "SPEC_ACT_SLOT_TITLE_TEXT", pFont9, NS_UITEXTCOLOR::ORANGE, TEXT_ALIGN_CENTER_X | TEXT_ALIGN_TOP );
    m_pDescText = CreateStaticControl ( "SPEC_ACT_SLOT_DESC_TEXT", pFont9, NS_UITEXTCOLOR::AQUA, TEXT_ALIGN_CENTER_X | TEXT_ALIGN_TOP );
    m_pProgressText = CreateStaticControl ( "SPEC_ACT_SLOT_PROGRESS_TEXT", pFont9, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_CENTER_X | TEXT_ALIGN_TOP );
    
    m_pRewardTitleText = CreateStaticControl ( "SPEC_ACT_SLOT_REWARD_TITLE_TEXT", pFont9, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
    m_pRewardPointText = CreateStaticControl ( "SPEC_ACT_SLOT_REWARD_POINT_TEXT", pFont9, NS_UITEXTCOLOR::GOLD, TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );

}


CBasicTextBox*	ActivityListSlot::CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR dwColor, int nAlign, UIGUID nWndID )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, szControlKeyword, nAlign, nWndID );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );
	RegisterControl ( pStaticText );

	return pStaticText;
}

void  ActivityListSlot::SetSpecAct( const Activity& cActivity )
{
    m_Activity = cActivity;

    SetSlotType( m_Activity );
    SetArrival( m_Activity.m_nByOrder );
    SetComplate( m_emSlotType );
    SetProgress( m_Activity );
    SetTitle( m_Activity.GetTitle().c_str() );
    SetDesc( m_Activity.GetDesc().c_str() );
    SetReward( m_Activity.GetRewardTitle().c_str(), m_Activity.GetRewardPoint() );
}

void ActivityListSlot::SetSlotType( Activity& cActivity )
{
    switch ( m_Activity.GetStatus() ) 
    {
    case ACTIVITY_STATUS_INPROGRESS:
        {
            m_emSlotType = SPEC_ACT_SLOT_TYPE_IDLE;

            //  레벨 달성일 경우
            if ( cActivity.m_id.wMainID == ACTIVITY_MID_LEVEL )
            {
                m_emSlotType = SPEC_ACT_SLOT_TYPE_CONTINUE;
            }
            else if ( cActivity.GetValue() )
            {
                m_emSlotType = SPEC_ACT_SLOT_TYPE_CONTINUE;
            }
        }
        break;
    case ACTIVITY_STATUS_COMPLETED:
        m_emSlotType = SPEC_ACT_SLOT_TYPE_COMP;
        break;
    case ACTIVITY_STATUS_ORDERING:
        m_emSlotType = SPEC_ACT_SLOT_TYPE_COMP;
        break;
    case ACTIVITY_STATUS_CLOSED:
        m_emSlotType = SPEC_ACT_SLOT_TYPE_NOTCOMP;
        break;
    }  
}

void  ActivityListSlot::SetArrival( int nCount )
{
    if ( nCount == 0 )
    {
        m_pAllImage->SetVisibleSingle( TRUE );
        m_pArrivalImage->SetVisibleSingle( FALSE );
        m_pArrivalText->SetOneLineText("");
    }
    else
    {
        m_pAllImage->SetVisibleSingle( FALSE );
        m_pArrivalImage->SetVisibleSingle( TRUE );

        m_pArrivalText->SetOneLineText ( sc::string::format("%1%",nCount).c_str(), NS_UITEXTCOLOR::BLACK ); 
    }
}

void  ActivityListSlot::SetComplate( SPEC_ACT_SLOT_TYPE emSlotType )
{

    for ( int i = 0; i < SPEC_ACT_SLOT_TYPE_SIZE; ++i ) 
    {
        m_pPresentBack[i]->SetVisibleSingle( FALSE );
        m_pRewardBack[i]->SetVisibleSingle( FALSE );            
    }
    
    m_pPresentBack[emSlotType]->SetVisibleSingle( TRUE );
    m_pRewardBack[emSlotType]->SetVisibleSingle( TRUE );
    
    m_pComplateImage->SetVisibleSingle ( FALSE );
    m_pCompText->SetVisibleSingle( FALSE );

    switch ( emSlotType )
    {
    case SPEC_ACT_SLOT_TYPE_IDLE:
        {
            //Blank
        }
        break;

    case SPEC_ACT_SLOT_TYPE_CONTINUE:
        {
            m_pCompText->SetVisibleSingle ( TRUE );
            m_pCompText->SetOneLineText ( (char*)ID2GAMEWORD ( "SPEC_ACT_SLOT_TEXT", 1  ), NS_UITEXTCOLOR::LIGHTSKYBLUE ); 
        }
        break;
    case SPEC_ACT_SLOT_TYPE_NOTCOMP:
        {
            m_pCompText->SetVisibleSingle ( TRUE );
            m_pCompText->SetOneLineText ( (char*)ID2GAMEWORD ( "SPEC_ACT_SLOT_TEXT", 2  ), NS_UITEXTCOLOR::LIGHTYELLOW ); 
        }
        break;
    case SPEC_ACT_SLOT_TYPE_COMP:
        {
            m_pComplateImage->SetVisibleSingle ( TRUE );
        }
        break;
    }
}

void ActivityListSlot::SetTitle( const CString& strTitle )
{
    std::string strText = sc::string::format( "<%1%>", strTitle );
    m_pTitleText->SetOneLineText ( strText.c_str(), NS_UITEXTCOLOR::DARKORANGE ); 
}

void ActivityListSlot::SetReward( const CString& strTitle, int nPoint )
{
    const UIRECT& rcParent = GetGlobalPos();

    // 타이틀
    if ( strTitle != "" )
    {
        m_pRewardTitleLineBox ->SetVisibleSingle( TRUE );
        m_pRewardTitleImage   ->SetVisibleSingle( TRUE );
        m_pRewardTitleText    ->SetVisibleSingle( TRUE );

        m_pRewardTitleText->SetOneLineText( strTitle );
    }
    else
    {
        m_pRewardTitleLineBox ->SetVisibleSingle( FALSE );
        m_pRewardTitleImage   ->SetVisibleSingle( FALSE );
        m_pRewardTitleText    ->SetVisibleSingle( FALSE );

        m_pRewardTitleText->SetOneLineText( "" );
    }

    // 생활점수
    if ( nPoint != 0 )
    {
        m_pRewardPointLineBox ->SetVisibleSingle( TRUE );
        m_pRewardPointImage   ->SetVisibleSingle( TRUE );
        m_pRewardPointText    ->SetVisibleSingle( TRUE );

        m_pRewardPointText->SetOneLineText( sc::string::format( "%1%", nPoint ).c_str(), NS_UITEXTCOLOR::GOLD );
    }
    else
    {
        m_pRewardPointLineBox ->SetVisibleSingle( FALSE );
        m_pRewardPointImage   ->SetVisibleSingle( FALSE );
        m_pRewardPointText    ->SetVisibleSingle( FALSE );

        m_pRewardPointText->SetOneLineText( "" );
    }

    if ( strTitle != "" && nPoint != 0 )
    {
        m_pRewardTitleLineBox ->ResetControl( rcParent, "SPEC_ACT_SLOT_REWARD_BACK_INNER1" );
        m_pRewardTitleImage   ->ResetControl( rcParent, "SPEC_ACT_SLOT_REWARD_TITLE_IMAGE" );
        m_pRewardTitleText    ->ResetControl( rcParent, "SPEC_ACT_SLOT_REWARD_TITLE_TEXT" );

        m_pRewardPointLineBox ->ResetControl( rcParent, "SPEC_ACT_SLOT_REWARD_BACK_INNER2" );
        m_pRewardPointImage   ->ResetControl( rcParent, "SPEC_ACT_SLOT_REWARD_POINT_IMAGE" );
        m_pRewardPointText    ->ResetControl( rcParent, "SPEC_ACT_SLOT_REWARD_POINT_TEXT" );
    }
    else
    {
        m_pRewardTitleLineBox ->ResetControl( rcParent, "SPEC_ACT_SLOT_REWARD_BACK_CENTER" );
        m_pRewardTitleImage   ->ResetControl( rcParent, "SPEC_ACT_SLOT_REWARD_IMAGE_CENTER" );
        m_pRewardTitleText    ->ResetControl( rcParent, "SPEC_ACT_SLOT_REWARD_TEXT_CENTER" );

        m_pRewardPointLineBox ->ResetControl( rcParent, "SPEC_ACT_SLOT_REWARD_BACK_CENTER" );
        m_pRewardPointImage   ->ResetControl( rcParent, "SPEC_ACT_SLOT_REWARD_IMAGE_CENTER" );
        m_pRewardPointText    ->ResetControl( rcParent, "SPEC_ACT_SLOT_REWARD_TEXT_CENTER" );
    }
}

void ActivityListSlot::SetDesc( const CString& strText )
{	
    m_strDesc     = strText;
    m_nAniDescCur = 0;
    m_nAniDescMax = 0;

    // 텍스트가 잘리는지 검사 한다
    {
        NS_UITEXTUTIL::ClearCRLFText ();
        NS_UITEXTUTIL::MULTILINETEXT& MLText = NS_UITEXTUTIL::GetMLTextWithoutCRLF ( m_strDesc, m_pDescText->GetLocalPos().sizeX, m_pDescText->GetFont() );

        if ( 1 < MLText.size() )
        {
            // 텍스트 이동을 해야할 최대 인덱스를 구한다.
            NS_UITEXTUTIL::MULTILINETEXT::iterator iter = MLText.begin() + 1;
            for ( ; iter != MLText.end(); ++iter )
            {
                NS_UITEXTUTIL::SSTRING& sString = (*iter);
                m_nAniDescMax += (UINT)sString.strLine.GetLength();
            }
            
            m_bAniDesc = true;
        }
        else
        {
            m_bAniDesc = false;
        }
    }

    m_pDescText->SetOneLineText ( m_strDesc, NS_UITEXTCOLOR::AQUA ); 
}

void ActivityListSlot::SetProgress( Activity& cActivity )
{
    std::string strText = ID2GAMEWORD ( "ACTIVITY_STATUS_TEXT", 0 );

    switch ( cActivity.GetStatus() )
    {
    case ACTIVITY_STATUS_INPROGRESS:
        {
            //  레벨 달성일 경우
            if ( cActivity.m_id.wMainID == ACTIVITY_MID_LEVEL )
            {
                int nValue = m_pGaeaClient->GetCharacterLogic().m_wLevel;
                std::string strTemp = sc::string::format( "%1%%2%", nValue, ID2GAMEWORD ( "ACTIVITY_STATUS_TEXT", 3 ) );
                strText = sc::string::format( strText, strTemp );
            }
            else
            {
                int nValue = cActivity.GetValue();
                std::string strTemp = sc::string::format( "%1%%2%", nValue, ID2GAMEWORD ( "ACTIVITY_STATUS_TEXT", 4 ) );
                strText = sc::string::format( strText, strTemp );
            }
            
            //if ( cActivity.m_nByOrder != 0 ) 
            //{
            //    // 수정 요망
            //    int nByOrder = 0;
            //    strText += sc::string::format( ID2GAMEWORD ( "ACTIVITY_STATUS_TEXT", 5 ), nByOrder, cActivity.m_nByOrder );
            //}            
        }
        break;
    case ACTIVITY_STATUS_ORDERING:
    case ACTIVITY_STATUS_COMPLETED:
        {
            strText = sc::string::format( strText, ID2GAMEWORD ( "ACTIVITY_STATUS_TEXT", 1 ) );
        }
        break;
    case ACTIVITY_STATUS_CLOSED:
        {
            strText = sc::string::format( strText, ID2GAMEWORD ( "ACTIVITY_STATUS_TEXT", 2 ) );
        }
        break;
    }

    m_pProgressText->SetOneLineText ( strText.c_str(), NS_UITEXTCOLOR::DEFAULT ); 
}

void ActivityListSlot::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    // 애니메이션
    if ( m_bAniDesc && m_bMouseOver )
    {
        if ( fDESC_ANIMATION_BEGIN_DALAY < m_fMouseOverTime )
        {
            if ( fDESC_ANIMATION_SPEED_DALAY < m_fAniTime && m_nAniDescCur < m_nAniDescMax )
            {
                const char* pDesc = m_strDesc.GetString();

                if ( NS_UITEXTUTIL::IsDBCS( *(pDesc + m_nAniDescCur) ) )
                {
                    m_nAniDescCur += 2;
                }
                else
                {
                    m_nAniDescCur += 1;
                }

                m_pDescText->SetOneLineText( m_strDesc.GetString() + m_nAniDescCur, NS_UITEXTCOLOR::AQUA );
                m_fAniTime = 0;
            }

            m_fAniTime += fElapsedTime;
        }

        m_fMouseOverTime += fElapsedTime;
    }
    else
    {
        // 원상복귀
        if ( m_nAniDescCur )
        {
            m_pDescText->SetOneLineText( m_strDesc, NS_UITEXTCOLOR::AQUA );
            m_nAniDescCur = 0;
        }

        m_fMouseOverTime = 0.0f;
        m_fAniTime       = 0.0f;
    }

    CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void ActivityListSlot::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
    if ( SPEC_ACT_SLOT_PRESENT <= ControlID && ControlID < SPEC_ACT_SLOT_PRESENT+SPEC_ACT_SLOT_TYPE_SIZE )
    {
        if ( CHECK_MOUSE_IN ( dwMsg ) )
        {
            m_bMouseOver = true;
        }
        else
        {
            m_bMouseOver = false;
        }
    }

    CUIGroup::TranslateUIMessage ( ControlID, dwMsg );
}