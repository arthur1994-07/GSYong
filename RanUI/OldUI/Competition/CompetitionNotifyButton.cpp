#include "StdAfx.h"
#include "./CompetitionNotifyButton.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/UITextUtil.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"

#include "../../../RanLogic/GLUseFeatures.h"
#include "../../../RanLogicClient/Level/GLLevelFileClient.h"
#include "../../../RanLogic/Land/GLMapAxisInfo.h"
#include "../../../RanLogicClient/CapturedTheField/GLCaptureTheFieldClient.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Tournament/GLTournamentClient.h"
#include "../../../enginelib/GUInterface/BasicLineBoxSmart.h"

#include "../../InnerInterface.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

using namespace PVP;

// ----------------------------------------------------------------------------

const float CCompetitionNotifyButton::s_fAutoUpdateTime = 3.0f;
const DWORD dwCTF_MatchingNum = -1;


CCompetitionNotifyButton::CCompetitionNotifyButton(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
: CUIGroup(pEngineDevice)
, m_pGaeaClient ( pGaeaClient )
, m_pInterface  ( pInterface )
, m_pButtonImage( NULL )
, m_pAlarm_Arrow( NULL )
, m_pAlarm_Text ( NULL )
, m_pLock		( NULL )
, m_bAlarm      ( false )
, m_nPrevCTFState  ( CTF_STATE_STOP )
, m_fUpdateTime ( 0.0f )
, m_pAlarm_LineBox(NULL)
, m_bChangeAlarm(false)
{
}

CCompetitionNotifyButton::~CCompetitionNotifyButton ()
{
}

HRESULT CCompetitionNotifyButton::RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = CUIGroup::RestoreDeviceObjects( pd3dDevice );

	UpdateAlarm ();
	return hr;
}

void CCompetitionNotifyButton::CreateSubControl ()
{
	CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

    CBasicButton* pButton = new CBasicButton(m_pEngineDevice);
    pButton->CreateSub ( this, "COMPETITION_NOTIFY_BUTTON_IMAGE", UI_FLAG_DEFAULT, NOTIFY_BUTTON );
    pButton->SetUseGlobalAction ( TRUE );
    RegisterControl ( pButton );
	m_pButtonImage = pButton;


	m_pLock = new CUIControl(m_pEngineDevice);
	m_pLock->CreateSub( this, "COMPETITION_NOTIFY_LOCK" );
	m_pLock->SetVisibleSingle( FALSE );
	RegisterControl ( m_pLock );

	m_pAlarm_LineBox = new CBasicLineBoxSmart(m_pEngineDevice);
	m_pAlarm_LineBox->CreateSub( this, "COMPETITION_NOTIFY_ALARM_LINE");
	m_pAlarm_LineBox->CreateSubControl( "COMPETITION_NOTIFY_ALARM_LINE_BOX" );
	m_pAlarm_LineBox->SetVisibleSingle( FALSE );
	RegisterControl( m_pAlarm_LineBox );

	m_pAlarm_Arrow = new CUIControl(m_pEngineDevice);
	m_pAlarm_Arrow->CreateSub( this, "COMPETITION_NOTIFY_ALARM_ARROW" );
	m_pAlarm_Arrow->SetVisibleSingle( FALSE );
	RegisterControl ( m_pAlarm_Arrow );

	m_pAlarm_Text = new CBasicTextBox(m_pEngineDevice);
	m_pAlarm_Text->CreateSub ( this, "COMPETITION_NOTIFY_ALARM_TEXT", UI_FLAG_DEFAULT );
	m_pAlarm_Text->SetFont ( pFont );
	m_pAlarm_Text->SetTextAlign ( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );	
	RegisterControl ( m_pAlarm_Text );
	
    Update ();

	if ( !GLUseFeatures::GetInstance().IsUsingCaptureTheField() && 
		 !GLUseFeatures::GetInstance().IsUsingInfinityStairs() )
    {
        SetVisibleSingle( FALSE );
    }
}

void CCompetitionNotifyButton::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
    switch ( ControlID )
    {
    case NOTIFY_BUTTON:
        {   
            if ( CHECK_MOUSE_IN ( dwMsg ) )
            {
                if ( m_pInterface->BEGIN_COMMON_LINEINFO_MULTI( TEXT_ALIGN_LEFT ) )
                {
                    //for ( ITER_VEC_STRING iter = m_vecInfo.begin(); iter != m_vecInfo.end(); ++iter )
                    for ( ITER_MAP_STRING iter = m_mapTootip.begin(); iter != m_mapTootip.end(); ++iter )
                    {
                        m_pInterface->ADD_COMMON_LINEINFO_MULTI( (iter->second).c_str(), NS_UITEXTCOLOR::WHITE );
                    }

                    m_pInterface->END_COMMON_LINEINFO_MULTI();
                }           

                if ( CHECK_LB_UP_LIKE ( dwMsg ) )
                {
					//Old
					//m_pInterface->CompetitionWindowOpen();
					//  [12/28/2012 gbgim]
                    if(m_pInterface->CompetitionWindowIsOpen())
						m_pInterface->CompetitionWindowClose();
					else
						m_pInterface->CompetitionWindowOpen();
                }
            }
        }
    }
}

void CCompetitionNotifyButton::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    if ( !IsVisible () ) return ;

    m_fMousePosX = static_cast<float>(x);
    m_fMousePosY = static_cast<float>(y);

    m_fUpdateTime += fElapsedTime;

    if ( s_fAutoUpdateTime <= m_fUpdateTime )
    {
        Update();
        m_fUpdateTime = 0.0f;
    }

    CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CCompetitionNotifyButton::Update ()
{
    SetLock(false);

    UpdateCTF();

    MatchingSystem::GLTouranmentClient* pTournament =  m_pGaeaClient->GetTournamentClient();
    if ( pTournament == NULL )
        return;

    const DWORD _infoSize = pTournament->m_ClientMatchingStatus.size();

    for ( int i(_infoSize); i--; )
    {
        const MatchingSystem::MatchingClientInfo* _info = (pTournament->m_ClientMatchingStatus[i]);
        switch ( _info->ClientStatus )
        {
        case MatchingSystem::EMSTATUS_GROUP_IN:
        case MatchingSystem::EMSTATUS_GROUP_PLAYING:
            SetLock(true);
            break;
        }
    }  

    if ( m_bChangeAlarm )
    {
        const bool bEnable(m_mapAlarm.size());
        m_pAlarm_LineBox->SetVisibleSingle(bEnable);
        m_pAlarm_Arrow->SetVisibleSingle(bEnable);
        m_pAlarm_Text->SetVisibleSingle(bEnable);

        if ( bEnable )
            UpdateAlarm();

        m_bChangeAlarm = false;
    }
}

void CCompetitionNotifyButton::UpdateAlarm()
{
	// 꼬리
	{
		const UIRECT& rcBase  = m_pButtonImage->GetGlobalPos();
		const UIRECT& rcArrow = m_pAlarm_Arrow->GetGlobalPos();
		const UIRECT& rcLineBoxLocal  = m_pAlarm_LineBox->GetLocalPos();
		m_pAlarm_LineBox->SetGlobalPos( D3DXVECTOR2(rcBase.right, rcBase.top - rcArrow.sizeY - rcLineBoxLocal.sizeY + 3) );
		m_pAlarm_LineBox->SetLocalPos( D3DXVECTOR2(rcBase.right, rcBase.top - rcArrow.sizeY - rcLineBoxLocal.sizeY + 3) );	
	}

	// 몸통, 텍스트
	{
		SIZE strSize;
        if ( m_mapAlarm.size() )
		{
			SIZE strSizeOld = {0,0};
			SIZE strSizeNew = {0,0};

			CD3DFontPar* pFont = m_pAlarm_Text->GetFont();

            for ( ITER_MAP_STRING iter = m_mapAlarm.begin(); iter != m_mapAlarm.end(); ++iter )
			{
				pFont->GetTextExtent ( (iter->second).c_str(), strSizeNew );

				if ( strSizeNew.cx > strSizeOld.cx )
				{
					strSize = strSizeNew;
				}
				else
				{
					strSize = strSizeOld;
				}
				strSizeOld = strSizeNew;
			}

		}
		else
		{
			strSize.cx = 1;
			strSize.cy = 1;
		}

		const UIRECT& rcLineBoxLocal = m_pAlarm_LineBox->GetLocalPos();	
        int nCountAlarm= static_cast<int>(m_mapAlarm.size());

		m_pAlarm_LineBox->SetGlobalPos(UIRECT(rcLineBoxLocal.left - (float)strSize.cx + 2.0f - 8.0f, rcLineBoxLocal.top - (rcLineBoxLocal.sizeY * (nCountAlarm - 1)), (float)strSize.cx + 2.0f  + 14.0f, rcLineBoxLocal.sizeY* nCountAlarm ));

		m_pAlarm_Text->SetLocalPos(m_pAlarm_LineBox->GetGlobalPos());
		m_pAlarm_Text->SetGlobalPos( m_pAlarm_LineBox->GetGlobalPos());
		m_pAlarm_Text->ClearText();
		 
        for ( ITER_MAP_STRING iter = m_mapAlarm.begin(); iter != m_mapAlarm.end(); ++iter )
		{
			if ( (iter->second).size() )
			{
				m_pAlarm_Text->AddText( (iter->second).c_str(), NS_UITEXTCOLOR::WHITE );
			}
		}
	}

	// 화살표
	{
		const UIRECT& rcBase  = m_pButtonImage->GetGlobalPos();
		const UIRECT& rcArrow = m_pAlarm_Arrow->GetGlobalPos();
		m_pAlarm_Arrow->SetGlobalPos( D3DXVECTOR2((rcBase.left + rcBase.right)/2.0f, rcBase.top - rcArrow.sizeY + 2) );
	}
}

void CCompetitionNotifyButton::SetWaitJoin( int nType )
{
	bool bIsSameNumber = false;

	for( size_t i = 0; i < m_vecWaitJoin.size(); i++ ) 
	{ 
		if( nType == m_vecWaitJoin[i] ) 
		{ 
			bIsSameNumber = true;
		} 
	}

	if ( bIsSameNumber )
	{
		m_vecWaitJoin.erase( remove( m_vecWaitJoin.begin(), m_vecWaitJoin.end(), nType ), m_vecWaitJoin.end() );
	}
	else
	{
		m_vecWaitJoin.push_back( nType );
	}
}



void CCompetitionNotifyButton::SetLock(const bool _bLock)
{
    m_pLock->SetVisibleSingle(_bLock);
}

void CCompetitionNotifyButton::SetAlarm(const DWORD _dwMatchingNum, const char* szText)  // 전장 버튼위에 조그맣게 뜨는 알람 메시지 (ex. 점령전 신청가능);
{
    if ( szText == NULL )  // 알람 삭제시;
    {
        ITER_MAP_STRING _iter = m_mapAlarm.find(_dwMatchingNum);
        if ( _iter != m_mapAlarm.end() )
        {
            m_mapAlarm.erase(_iter);
        }
    }
    else  // 알람 추가시;
    {
        ITER_MAP_STRING _iter = m_mapAlarm.find(_dwMatchingNum);
        if ( _iter != m_mapAlarm.end() )
        {
            (_iter->second) = szText;
        }
        else
        {
            m_mapAlarm.insert(MAP_STRING_VALUE(_dwMatchingNum, szText));
        }
    }
    m_bChangeAlarm = true;
}

void CCompetitionNotifyButton::ClearAlarm()
{
    m_mapAlarm.clear();
    m_bChangeAlarm = true;
}

void CCompetitionNotifyButton::SetTooltip(const DWORD _dwMatchingNum, const char* szText)  // 전장 버튼을 마우스 오버시 나오는 전장의 현 상태 (ex. 2시간후 점령전이 시작됩니다.);
{
    if ( szText == NULL )  // 알람 삭제시;
    {
        ITER_MAP_STRING _iter = m_mapTootip.find(_dwMatchingNum);
        if ( _iter != m_mapTootip.end() )
        {
            m_mapTootip.erase(_iter);
        }
    }
    else  // 알람 추가시;
    {
        ITER_MAP_STRING _iter = m_mapTootip.find(_dwMatchingNum);
        if ( _iter != m_mapTootip.end() )
        {
            (_iter->second) = szText;
        }
        else
        {
            m_mapTootip.insert(MAP_STRING_VALUE(_dwMatchingNum, szText));
        }
    }
}

void CCompetitionNotifyButton::UpdateCTF()    // 점령전은 아직 컨버팅 되지않은 전장 컨텐츠이므로 예외적으로 Update처리를 해준다.;
{
    GLCaptureTheFieldClient* pCTFClient = m_pGaeaClient->GetCaptureTheField();
    GLCaptureTheFieldClient::CTFGLOBALSTATE& sCTFState = pCTFClient->GetGlobalState( PVP::ENUM_CTF_TYPE_POWERSTATION );

    //	서버 타임으로 변환( TimeZone 계산 )
    CTime tCurTime( m_pGaeaClient->GetCurrentTime() );
    m_pGaeaClient->GetConvertServerTime( tCurTime );

    switch( sCTFState.emSTATE )
    {
    case CTF_STATE_STOP:
    case CTF_STATE_READY:
        {
            CTimeSpan sTimeLeft( sCTFState.tSTARTTIME - tCurTime.GetTime() );

            if ( sTimeLeft < 0 )
            {
                sTimeLeft = 0;
            }

            sTimeLeft += 60;

            if ( sTimeLeft.GetTotalHours() )
            {
                SetTooltip(dwCTF_MatchingNum, sc::string::format(ID2GAMEWORD("COMPETITIONNOTIFY_BUTTON_TOOLTIP", 0), sTimeLeft.GetTotalHours()).c_str());
            }
            else
            {
                SetTooltip(dwCTF_MatchingNum, sc::string::format(ID2GAMEWORD("COMPETITIONNOTIFY_BUTTON_TOOLTIP", 1), sTimeLeft.GetTotalMinutes()).c_str());
            }
        }
        break;

    case CTF_STATE_START:
        {
            CTimeSpan sTimeLeft( (sCTFState.tSTARTTIME + sCTFState.tPLAYTIME) - tCurTime.GetTime() );

            if ( sTimeLeft < 0 )
            {
                sTimeLeft = 0;
            }

            sTimeLeft += 60;
            SetTooltip(dwCTF_MatchingNum, sc::string::format( ID2GAMEWORD("COMPETITIONNOTIFY_BUTTON_TOOLTIP", 2), sTimeLeft.GetTotalMinutes()).c_str());
        }
        break;

    case CTF_STATE_DONE:
    case CTF_STATE_REWARD:
        {
            SetTooltip(dwCTF_MatchingNum, ID2GAMEWORD("COMPETITIONNOTIFY_BUTTON_TOOLTIP", 3));
        }
        break;
    };

    //m_vecInfo.clear();
    //m_vecInfo.push_back( sc::string::format(ID2GAMEWORD("COMPETITIONNOTIFY_BUTTON_TOOLTIP", 4), m_pInterface->GetdwKeyToString(DIK_U).GetString()) );
    if ( sCTFState.IsREADY() )
    {
        if ( m_nPrevCTFState != static_cast<UINT>(sCTFState.emSTATE) )
        {
            // 알람
            GLLevelFileClient* pLevelFile = sCTFState.spPVPMAPLEVELDATA.get();
            if ( pLevelFile )
            {
                SetAlarm(dwCTF_MatchingNum, sc::string::format(ID2GAMEWORD("COMEPTITION_NOTIFY_BUTTON", 0), m_pGaeaClient->GetMapName(pLevelFile->GetBaseMapID())).c_str());
            }
        }
    }
    else
    {
        SetAlarm(dwCTF_MatchingNum, NULL);
    }
    m_nPrevCTFState  = static_cast<UINT>(sCTFState.emSTATE);
}



MyCompetitionNotifyButton::MyCompetitionNotifyButton( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CCompetitionNotifyButton( pGaeaClient, pInterface, pEngineDevice )
{
}

void MyCompetitionNotifyButton::CreateUIWindowAndRegisterOwnership()
{
    CCompetitionNotifyButton::Create( COMPETITION_NOTIFY_BUTTON, "COMPETITION_NOTIFY_BUTTON", UI_FLAG_RIGHT | UI_FLAG_BOTTOM );
    CCompetitionNotifyButton::CreateSubControl();
    CCompetitionNotifyButton::m_pInterface->UiRegisterControl( this );
    CCompetitionNotifyButton::m_pInterface->UiShowGroupBottom( COMPETITION_NOTIFY_BUTTON );
}

void MyCompetitionNotifyButton::Update()
{
    CCompetitionNotifyButton::Update();
}

void MyCompetitionNotifyButton::SetLock(const bool _bLock)
{
    CCompetitionNotifyButton::SetLock(_bLock);
    CCompetitionNotifyButton::Update();
}

void MyCompetitionNotifyButton::ClearAlarm()
{
    CCompetitionNotifyButton::ClearAlarm();
    CCompetitionNotifyButton::Update();
}

void MyCompetitionNotifyButton::SetAlarm(const DWORD _dwMatchingNum, const char* szText)  // 전장 버튼위에 조그맣게 뜨는 알람 메시지 (ex. 점령전 신청가능);
{
    CCompetitionNotifyButton::SetAlarm(_dwMatchingNum, szText);
    CCompetitionNotifyButton::Update();
}

void MyCompetitionNotifyButton::SetTooltip(const DWORD _dwMatchingNum, const char* szText)  // 전장 버튼을 마우스 오버시 나오는 전장의 현 상태 (ex. 2시간후 점령전이 시작됩니다.);
{
    CCompetitionNotifyButton::SetTooltip(_dwMatchingNum, szText);
    CCompetitionNotifyButton::Update();
}
