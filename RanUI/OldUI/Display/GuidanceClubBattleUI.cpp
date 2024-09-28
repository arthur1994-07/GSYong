#include "StdAfx.h"
#include "GuidanceClubBattleUI.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicProgressBar.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/UIWindowBody.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Club/GLClubClient.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

GuidanceAuthProgressUI::GuidanceAuthProgressUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CUIGroup(pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_pInterface(pInterface)
, fAuthTime(0.0f)
, fCurrentTime(0.0f)
{
}

GuidanceAuthProgressUI::~GuidanceAuthProgressUI ()
{
}

void GuidanceAuthProgressUI::CreateSubControl ()
{    
    fAuthTime = 0.0f;
    fCurrentTime = 0.0f;

    CBasicProgressBar* pProgress = new CBasicProgressBar(m_pEngineDevice);
    pProgress->CreateSub ( this, "GUIDANCE_AUTH_PROGRESS_IMAGE", UI_FLAG_DEFAULT);
    pProgress->CreateOverImage ( "GUIDANCE_AUTH_PROGRESS_OVER_IMAGE" );
    RegisterControl ( pProgress );
    m_pProgress = pProgress;
}

void GuidanceAuthProgressUI::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    if ( fAuthTime > 0.0f )
    {
        fCurrentTime += fElapsedTime;
        if ( fAuthTime <= fCurrentTime )
        {
            fCurrentTime = 0.0f;
            fAuthTime = 0.0f;
            SetVisibleSingle(FALSE);
        }
        else
        {
            const float _timerPercent = fCurrentTime / fAuthTime;
            m_pProgress->SetPercent(_timerPercent);
            m_pProgress->UpdateProgress();
        }
    }
}

void GuidanceAuthProgressUI::SetAuthTime(const float _fAuthTime, const float _fCurrentTime)
{
    fAuthTime = _fAuthTime;
    fCurrentTime = _fCurrentTime;
}

GuidanceProgressInfoUI::GuidanceProgressInfoUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CUIGroup(pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_pInterface(pInterface)
, m_pTopText(NULL)
, m_pMiddleText(NULL)
, m_pBottomText(NULL)
, m_pBackGround(NULL)
{
}

GuidanceProgressInfoUI::~GuidanceProgressInfoUI ()
{
}

void GuidanceProgressInfoUI::CreateSubControl()
{
    CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG);
    const int nAlign = TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y;	
    {
        CUIWindowBody* pContorl = new CUIWindowBody(m_pEngineDevice);
        pContorl->CreateSub( this, "GUIDANCE_PROGRESS_INFO_BACKGROUND", UI_FLAG_CENTER_X);
        pContorl->CreateBody("GUIDANCE_PROGRESS_INFO_BACKGROUND_LEFT",
                             "GUIDANCE_PROGRESS_INFO_BACKGROUND_UP",
                             "GUIDANCE_PROGRESS_INFO_BACKGROUND_MAIN",
                             "GUIDANCE_PROGRESS_INFO_BACKGROUND_DOWN",
                             "GUIDANCE_PROGRESS_INFO_BACKGROUND_RIGHT");
        RegisterControl ( pContorl );
        m_pBackGround = pContorl;
    }
    {
        CBasicTextBox* pText = new CBasicTextBox(m_pEngineDevice);
        pText->CreateSub ( this, "GUIDANCE_PROGRESS_INFO_TOP_TEXTBOX", UI_FLAG_DEFAULT);
        pText->SetFont ( pFont );
        pText->SetTextAlign ( nAlign );
        RegisterControl ( pText );
        m_pTopText = pText;
    }
    {
        CBasicTextBox* pText = new CBasicTextBox(m_pEngineDevice);
        pText->CreateSub ( this, "GUIDANCE_PROGRESS_INFO_MIDDLE_TEXTBOX", UI_FLAG_DEFAULT);
        pText->SetFont ( pFont );
        pText->SetTextAlign ( nAlign );
        RegisterControl ( pText );
        m_pMiddleText = pText;
    }
    {
        CBasicTextBox* pText = new CBasicTextBox(m_pEngineDevice);
        pText->CreateSub ( this, "GUIDANCE_PROGRESS_INFO_BOTTOM_TEXTBOX", UI_FLAG_DEFAULT);
        pText->SetFont ( pFont );
        pText->SetTextAlign ( nAlign );
        RegisterControl ( pText );
        m_pBottomText = pText;
    }
}

void GuidanceProgressInfoUI::SetTopParam(const int _iParam1, const int _iParam2)
{
    std::string _strName = sc::string::format(ID2GAMEWORD("RENEWAL_GUIDANCE_INFO_NAME_WORD", _iParam1), _iParam1);  // 성문 선도클럽 결정전;
    std::string _strProgress = sc::string::format(ID2GAMEWORD("RENEWAL_GUIDANCE_INFO_PROGRESS_WORD", _iParam2), _iParam1);  // 진행중;
    m_pTopText->SetOneLineText(sc::string::format("%1% %2%", _strName, _strProgress).c_str());
}

void GuidanceProgressInfoUI::SetMiddleParam(const int _iParam1)
{
    std::string strMiddle = sc::string::format(ID2GAMEWORD("RENEWAL_GUIDANCE_INFO_WORD", 0), _iParam1);  // 목표점수 : %1%;
    m_pMiddleText->SetOneLineText(strMiddle.c_str());
}

void GuidanceProgressInfoUI::SetBottomParam(const float _fParam1)
{
    m_fRemainTime = _fParam1;
}

void GuidanceProgressInfoUI::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update(x, y , LB, MB, RB, nScroll, fElapsedTime, bFirstControl);
    if ( m_fRemainTime > 0.0f )
    {
        m_fRemainTime -= fElapsedTime;
        const int _iRemainTime = (int)m_fRemainTime;
        std::string strBottom = sc::string::format(ID2GAMEWORD("RENEWAL_GUIDANCE_INFO_WORD", 1), (_iRemainTime / 60),  (_iRemainTime % 60));  // 남은 시간 : %1%:%2%;
        m_pBottomText->SetOneLineText(strBottom.c_str());
    }
    if ( MouseUpdate( x, y ) )
    {
        m_pBackGround->SetVisibleSingle(TRUE);
    }
    else
    {
        m_pBackGround->SetVisibleSingle(FALSE);
    }
}

GuidanceRankInfoUI::GuidanceRankInfoUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CUIGroup(pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_pInterface(pInterface)
, m_pClubRankText(NULL)
, m_pClubNameText(NULL)
, m_pClubPointText(NULL)
, m_pMyRankText(NULL)
, m_pMyNameText(NULL)
, m_pMyPointText(NULL)
, m_pBackGround(NULL)
{
}

GuidanceRankInfoUI::~GuidanceRankInfoUI ()
{
}

void GuidanceRankInfoUI::CreateSubControl()
{
    CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );
    const int nAlignCenter = TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y;
    {
        CUIWindowBody* pContorl = new CUIWindowBody(m_pEngineDevice);
        pContorl->CreateSub( this, "GUIDANCE_RANK_INFO_BACKGROUND", UI_FLAG_XSIZE | UI_FLAG_YSIZE);
        pContorl->CreateBody("GUIDANCE_RANK_INFO_BACKGROUND_LEFT",
                             "GUIDANCE_RANK_INFO_BACKGROUND_UP",
                             "GUIDANCE_RANK_INFO_BACKGROUND_MAIN",
                             "GUIDANCE_RANK_INFO_BACKGROUND_DOWN",
                             "GUIDANCE_RANK_INFO_BACKGROUND_RIGHT");
        RegisterControl ( pContorl );
        m_pBackGround = pContorl;
    }
    {
        CBasicTextBox* pText = CreateBasicTextControl(this, "GUIDANCE_RANK_INFO_TITLE_RANK_TEXTBOX", pFont, nAlignCenter);
        const char* strText = ID2GAMEWORD("RENEWAL_GUIDANCE_RANK_WORD", 0);
        //const char* strText = "순위";
        pText->SetOneLineText(strText);
    }
    {
        CBasicTextBox* pText = CreateBasicTextControl(this, "GUIDANCE_RANK_INFO_TITLE_NAME_TEXTBOX", pFont, nAlignCenter);
        const char* strText = ID2GAMEWORD("RENEWAL_GUIDANCE_RANK_WORD", 1);
        //const char* strText = "클럽명";
        pText->SetOneLineText(strText);
    }
    {
        CBasicTextBox* pText = CreateBasicTextControl(this, "GUIDANCE_RANK_INFO_TITLE_POINT_TEXTBOX", pFont, nAlignCenter);
        const char* strText = ID2GAMEWORD("RENEWAL_GUIDANCE_RANK_WORD", 2);
        //const char* strText = "누적포인트";
        pText->SetOneLineText(strText);
    }
    {
        CBasicTextBox* pText = CreateBasicTextControl(this, "GUIDANCE_RANK_INFO_HEADLINE_TEXTBOX", pFont, nAlignCenter);
        const char* strText = ID2GAMEWORD("RENEWAL_GUIDANCE_RANK_WORD", 3);
        //const char* strText = "==============================";
        pText->SetOneLineText(strText);        
    }
    {
        CBasicTextBox* pText = CreateBasicTextControl(this, "GUIDANCE_RANK_INFO_RANK_TEXTBOX", pFont, TEXT_ALIGN_CENTER_X | TEXT_ALIGN_TOP);
        pText->SetPartInterval(5.0f);
        m_pClubRankText = pText;
    }
    {
        CBasicTextBox* pText = CreateBasicTextControl(this, "GUIDANCE_RANK_INFO_NAME_TEXTBOX", pFont, TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP);
        pText->SetPartInterval(5.0f);
        m_pClubNameText = pText;
    }
    {
        CBasicTextBox* pText = CreateBasicTextControl(this, "GUIDANCE_RANK_INFO_POINT_TEXTBOX", pFont, TEXT_ALIGN_RIGHT | TEXT_ALIGN_TOP);
        pText->SetPartInterval(5.0f);
        m_pClubPointText = pText;
    }
    {
        CBasicTextBox* pText = CreateBasicTextControl(this, "GUIDANCE_RANK_INFO_TAILLINE_TEXTBOX", pFont, nAlignCenter);
        const char* strText = ID2GAMEWORD("RENEWAL_GUIDANCE_RANK_WORD", 3);
        //const char* strText = "==============================";
        pText->SetOneLineText(strText);        
    }
    {
        CBasicTextBox* pText = CreateBasicTextControl(this, "GUIDANCE_RANK_INFO_MY_RANK_TEXTBOX", pFont, TEXT_ALIGN_CENTER_X | TEXT_ALIGN_TOP);
        m_pMyRankText = pText;
    }
    {
        CBasicTextBox* pText = CreateBasicTextControl(this, "GUIDANCE_RANK_INFO_MY_NAME_TEXTBOX", pFont, TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP);
        m_pMyNameText = pText;
    }
    {
        CBasicTextBox* pText = CreateBasicTextControl(this, "GUIDANCE_RANK_INFO_MY_POINT_TEXTBOX", pFont, TEXT_ALIGN_RIGHT | TEXT_ALIGN_TOP);
        m_pMyPointText = pText;
    }
}

void GuidanceRankInfoUI::SetRankInfo(GLMSG::NET_GUIDANCE_RANK_VEC& _RankInfo)
{
    m_pClubRankText->ClearText();
    m_pClubNameText->ClearText();
    m_pClubPointText->ClearText();

    const char* strPointXml = ID2GAMEWORD("RENEWAL_GUIDANCE_RANK_WORD", 4);
    //const char* strPointXml = "%1%P";
    for ( GLMSG::NET_GUIDANCE_RANK_VEC::iterator iter = _RankInfo.begin(); iter != _RankInfo.end(); iter++)
    {
        m_pClubRankText->AddText(sc::string::format("%1%", iter->iRank).c_str());
        m_pClubNameText->AddText(sc::string::format("%1%", iter->clubName).c_str());
        m_pClubPointText->AddText(sc::string::format(strPointXml, iter->iPoint).c_str());
    }
}

void GuidanceRankInfoUI::SetRankMyInfo(const int _iRank, const int _iPoint)
{
    m_pMyRankText->SetOneLineText(sc::string::format("%1%", _iRank).c_str());

    GLClubClient* pClub = m_pGaeaClient->GetMyClub();
    if ( pClub )
    {   
        m_pMyNameText->SetOneLineText( pClub->Name() );
    }
    const char* strPointXml = ID2GAMEWORD("RENEWAL_GUIDANCE_RANK_WORD", 4);
    //const char* strPointXml = "%1%P";
    m_pMyPointText->SetOneLineText(sc::string::format(strPointXml, _iPoint).c_str());
}

void GuidanceRankInfoUI::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update(x, y , LB, MB, RB, nScroll, fElapsedTime, bFirstControl);
    if ( MouseUpdate( x, y ) )
    {
        m_pBackGround->SetVisibleSingle(TRUE);
    }
    else
    {
        m_pBackGround->SetVisibleSingle(FALSE);
    }
}

void GuidanceRankInfoUI::SetVisibleSingle( BOOL bVisible )
{
    if ( bVisible )
    {
        // UI 위치가 겹쳐서 퀘스트 도우미UI를 끈다;
        m_pInterface->UiHideGroup( QUEST_HELPER_DISPLAY );
    }
    else
    {
        // 꺼놓았던 퀘스트 도우미UI를 켠다;
        m_pInterface->UiShowGroupBottom( QUEST_HELPER_DISPLAY );
    }
    CUIGroup::SetVisibleSingle(bVisible);
}




























MyGuidanceAuthProgressUI::MyGuidanceAuthProgressUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : GuidanceAuthProgressUI(pGaeaClient, pInterface, pEngineDevice)
{
}

void MyGuidanceAuthProgressUI::CreateUIWindowAndRegisterOwnership()
{
    GuidanceAuthProgressUI::Create(GUIDANCE_AUTH_PROGRESS_UI, "GUIDANCE_AUTH_PROGRESS_UI", UI_FLAG_CENTER_X | UI_FLAG_BOTTOM );
    GuidanceAuthProgressUI::CreateSubControl();
    GuidanceAuthProgressUI::m_pInterface->UiRegisterControl( this );
    GuidanceAuthProgressUI::m_pInterface->UiShowGroupFocus( CUIGroup::GetWndID() );
    GuidanceAuthProgressUI::SetVisibleSingle(FALSE);
}

void MyGuidanceAuthProgressUI::SetAuthTime(const bool _bVisible, const float _fAuthTime, const float _fCurrentTime /* = 0.0f */)
{
    SetVisibleSingle(_bVisible);
    if ( _bVisible == true )
    {
        GuidanceAuthProgressUI::SetAuthTime(_fAuthTime, _fCurrentTime);
    }
}

MyGuidanceProgressInfoUI::MyGuidanceProgressInfoUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: GuidanceProgressInfoUI(pGaeaClient, pInterface, pEngineDevice)
{
}

void MyGuidanceProgressInfoUI::CreateUIWindowAndRegisterOwnership()
{
    GuidanceProgressInfoUI::Create(GUIDANCE_PROGRESS_INFO_UI, "GUIDANCE_PROGRESS_INFO_UI", UI_FLAG_XRESRATE);
    GuidanceProgressInfoUI::CreateSubControl();
    GuidanceProgressInfoUI::m_pInterface->UiRegisterControl( this );
    GuidanceProgressInfoUI::m_pInterface->UiShowGroupFocus( CUIGroup::GetWndID() );
    GuidanceProgressInfoUI::SetVisibleSingle(FALSE);
}

void MyGuidanceProgressInfoUI::SetProgressInfo(const NET_MSG_GENERIC* _pMessag)
{
    GLMSG::NET_INSTANCE_GUIDANCE_UI_PROGRESS_FC* pMsg = (GLMSG::NET_INSTANCE_GUIDANCE_UI_PROGRESS_FC*)_pMessag;
    GuidanceProgressInfoUI::SetVisibleSingle(TRUE);
    GuidanceProgressInfoUI::SetTopParam(pMsg->iNameIndex, pMsg->iProgressIndex);
    GuidanceProgressInfoUI::SetMiddleParam(pMsg->iPoint);
    GuidanceProgressInfoUI::SetBottomParam(pMsg->fRemainTime);
}

MyGuidanceRankInfoUI::MyGuidanceRankInfoUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: GuidanceRankInfoUI(pGaeaClient, pInterface, pEngineDevice)
{
}

void MyGuidanceRankInfoUI::CreateUIWindowAndRegisterOwnership()
{
    GuidanceRankInfoUI::Create(GUIDANCE_RANK_INFO_UI, "GUIDANCE_RANK_INFO_UI", UI_FLAG_RIGHT);
    GuidanceRankInfoUI::CreateSubControl();
    GuidanceRankInfoUI::m_pInterface->UiRegisterControl( this );
    GuidanceRankInfoUI::m_pInterface->UiShowGroupFocus( CUIGroup::GetWndID() );
    GuidanceRankInfoUI::SetVisibleSingle(FALSE);
}

void MyGuidanceRankInfoUI::SetRankInfo(const NET_MSG_GENERIC* _pMessag)
{
    GLMSG::NET_INSTANCE_GUIDANCE_UI_RANK_FC netMsg;
    if ( CastNetMsg(_pMessag, netMsg) == false )
        return;

    GuidanceRankInfoUI::SetVisibleSingle(TRUE);
    GuidanceRankInfoUI::SetRankInfo(netMsg.rankVector);
}

void MyGuidanceRankInfoUI::SetRankMyInfo(const NET_MSG_GENERIC* _pMessag)
{
    GLMSG::NET_INSTANCE_GUIDANCE_UI_RANK_MY_INFO_FC* pMsg = (GLMSG::NET_INSTANCE_GUIDANCE_UI_RANK_MY_INFO_FC*)_pMessag;
    GuidanceRankInfoUI::SetRankMyInfo(pMsg->iMyClubRank, pMsg->iMyClubPoint);
}