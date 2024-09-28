#include "StdAfx.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../InnerInterface.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../enginelib/GUInterface/BasicLineBoxSmart.h"
#include "../../../enginelib/DxMeshs/SkinMesh/DxSkinAnimation.h"

#include "../../../RanLogicClient/Char/GLCharacter.h"

#include "./ForceRebirth.h"

CForceRebirth::CForceRebirth( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CUIGroup( pEngineDevice )
, m_pRebirthText(NULL)
, m_pWatchIcon(NULL)
, m_pBlank(NULL)
, m_pGauge(NULL)
, m_bForceRebirth(false)
, m_fMaxTime(0.0f)
, m_pGaeaClient(pGaeaClient)
, m_pInterface(pInterface)
{
}

void CForceRebirth::CreateSubControl()
{
    {
        CUIControl* pControl = new CUIControl( m_pEngineDevice );
        pControl->CreateSub( this, "FORCE_REBIRTH_WATCH_ICON", UI_FLAG_DEFAULT );
        RegisterControl( pControl );
        m_pWatchIcon = pControl;
    }

    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );
        pLinBoxSmart->CreateSub( this, "FORCE_REBIRTH_BLANK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "FORCE_REBIRTH_BLANK_LINE" );
        RegisterControl( pLinBoxSmart );
        m_pBlank = pLinBoxSmart;
    }

    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );
        pLinBoxSmart->CreateSub( this, "FORCE_REBIRTH_GAUGE", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "FORCE_REBIRTH_GAUGE_LINE" );
        RegisterControl( pLinBoxSmart );
        m_pGauge = pLinBoxSmart;
    }

    {
        CD3DFontPar* pFont16Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 16, _DEFAULT_FONT_SHADOW_FLAG | D3DFONT_BOLD );

        CBasicTextBox* pTextBox = new CBasicTextBox( m_pEngineDevice );
        pTextBox->CreateSub( this, "FORCE_REBIRTH_TEXT", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont16Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pRebirthText = pTextBox;
    }

	{
		CD3DFontPar* pFont16Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 18, _DEFAULT_FONT_SHADOW_FLAG | D3DFONT_BOLD );

		CBasicTextBox* pTextBox = new CBasicTextBox( m_pEngineDevice );
		pTextBox->CreateSub( this, "FORCE_REBIRTH_WATCH_ICON", UI_FLAG_DEFAULT );
		pTextBox->SetFont( pFont16Shadow );
		pTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
		pTextBox->SetTransparentOption(TRUE);
		RegisterControl( pTextBox );
		m_pWatchIconText = pTextBox;
	}
}

void CForceRebirth::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

    m_fTime -= fElapsedTime;

    if ( m_fTime <= 0.0f )
        m_fTime = 0.0f;

    {
        float fPercent = m_fTime / m_fMaxTime;

        UIRECT rcGaugePos = m_pGauge->GetGlobalPos();
        UIRECT rcBlankPos = m_pBlank->GetGlobalPos();

        UIRECT rcNewPos = rcBlankPos;
		const float _fGaugeSize = rcNewPos.sizeX - 10.0f;

        rcNewPos.sizeX = floor(_fGaugeSize * fPercent) + 10.0f;
        rcNewPos.right = rcNewPos.left + rcNewPos.sizeX;

        m_pGauge->SetLocalPos( D3DXVECTOR2( 0.0f, 0.0f ) );
        m_pGauge->ReSizeControl( rcGaugePos, rcNewPos );

        D3DXVECTOR2 vPos;
        vPos.x = m_pBlank->GetLocalPos().left;
        vPos.y = m_pBlank->GetLocalPos().top;

        m_pGauge->SetLocalPos( vPos );
    }

	// (부활 표시시간이 반올림이 발생하여 직관적이지 못해서 올림으로 변경);
    std::string strText = "";
    strText = sc::string::format( "%.0f", (m_fTime + 0.49f) );

	m_pWatchIconText->ClearText();
	m_pWatchIconText->AddText( strText.c_str(), NS_UITEXTCOLOR::RED );
	m_pWatchIconText->SetDiffuseAlpha( (WORD)(255.0f * 0.67f) );

    m_pRebirthText->ClearText();
	// 카운팅시 바로 9가되어버리기 때문에 +1추가함;
    strText = sc::string::format( "%d%s", (int)(m_fTime + 1.0f), ID2GAMEINTEXT("NOTIFY_FORCE_REBIRTH"));
    m_pRebirthText->AddText( strText.c_str(), NS_UITEXTCOLOR::WHITE );

    if ( m_fTime <= 0.0f )
    {
        if ( m_pGaeaClient->GetCharacter()->IsDie() )
        {
            // WPE Pro등 패킷조작을 통한 강제 부활이슈로 인해 클라이언트 정보는 무시하고 서버측에서 부활시키도록 변경  [3/31/2014 hsshin]
            // 부활하기
            //m_pGaeaClient->GetCharacter()->ReqResurrect(EM_RESURRECTION_FORCED);

            // 열려진 창들 닫기
            m_pInterface->CloseAllWindow();				
        }
    }
}

bool CForceRebirth::IsForceRebirth()
{
    return m_bForceRebirth;
}

void CForceRebirth::SetForceRebirth( const float _fTime )
{
    if ( _fTime > 0.0f )
    {
        m_bForceRebirth = true;
		// 강제 부활시간이 서버와 다른 부분 보정 [8/21/2014 hsshin];
		// 강제부활의 경우 서버에서는 죽음과 동시에 카운트를 시작하지만
		// 클라이언트의 경우 Falling(쓰러지는 모션)후에 Die(죽어있는 상태)로 변한 시점에서
		// 카운트가 시작되므로 부활 시간이 차이가 나게 된다.
		// 따라서 이런형태로 보정을 해줌;

		float _fallingAniTime = 0.0f;
		GLCharacter* _pChar = m_pGaeaClient->GetCharacter();
		if ( _pChar )
		{
			DxSkinChar* _pSkinChar = _pChar->GetSkinChar();
			if ( _pSkinChar )
			{
				const PANIMCONTNODE _pAnim = _pSkinChar->GETANI( AN_DIE, AN_SUB_NONE );
				DxSkinAnimation* _pSkinAni = _pAnim->m_pDxSkinAni;
				if ( _pSkinAni )
				{
					const DWORD _startTime = _pSkinAni->m_dwSTime;
					const DWORD _endTime = _pSkinAni->m_dwETime;
					const DWORD _unitTime = _pSkinAni->m_dwETime;

					_fallingAniTime = ((float)(_endTime - _startTime)) / _unitTime;
				}
			}
		}

        m_fTime = static_cast<float>( _fTime ) - _fallingAniTime;
    }
    else
        m_bForceRebirth = false;

    m_fMaxTime = m_fTime;

    m_pGauge->ResetControl( GetGlobalPos(), "FORCE_REBIRTH_GAUGE" );
}