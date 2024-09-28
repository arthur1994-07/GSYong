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

	// (��Ȱ ǥ�ýð��� �ݿø��� �߻��Ͽ� ���������� ���ؼ� �ø����� ����);
    std::string strText = "";
    strText = sc::string::format( "%.0f", (m_fTime + 0.49f) );

	m_pWatchIconText->ClearText();
	m_pWatchIconText->AddText( strText.c_str(), NS_UITEXTCOLOR::RED );
	m_pWatchIconText->SetDiffuseAlpha( (WORD)(255.0f * 0.67f) );

    m_pRebirthText->ClearText();
	// ī���ý� �ٷ� 9���Ǿ������ ������ +1�߰���;
    strText = sc::string::format( "%d%s", (int)(m_fTime + 1.0f), ID2GAMEINTEXT("NOTIFY_FORCE_REBIRTH"));
    m_pRebirthText->AddText( strText.c_str(), NS_UITEXTCOLOR::WHITE );

    if ( m_fTime <= 0.0f )
    {
        if ( m_pGaeaClient->GetCharacter()->IsDie() )
        {
            // WPE Pro�� ��Ŷ������ ���� ���� ��Ȱ�̽��� ���� Ŭ���̾�Ʈ ������ �����ϰ� ���������� ��Ȱ��Ű���� ����  [3/31/2014 hsshin]
            // ��Ȱ�ϱ�
            //m_pGaeaClient->GetCharacter()->ReqResurrect(EM_RESURRECTION_FORCED);

            // ������ â�� �ݱ�
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
		// ���� ��Ȱ�ð��� ������ �ٸ� �κ� ���� [8/21/2014 hsshin];
		// ������Ȱ�� ��� ���������� ������ ���ÿ� ī��Ʈ�� ����������
		// Ŭ���̾�Ʈ�� ��� Falling(�������� ���)�Ŀ� Die(�׾��ִ� ����)�� ���� ��������
		// ī��Ʈ�� ���۵ǹǷ� ��Ȱ �ð��� ���̰� ���� �ȴ�.
		// ���� �̷����·� ������ ����;

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