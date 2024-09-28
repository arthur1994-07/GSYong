#include "stdafx.h"

#include "../../../enginelib/DeviceMan.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/Party/GLPartyManagerClient.h"
#include "../../InnerInterface.h"
#include "CaptureTheFlagEffectMan.h"

#define BLUE_FLAG_GUIDE_ARROW_EFFECT_FILENAME "flag_direction_blue.egp"
#define RED_FLAG_GUIDE_ARROW_EFFECT_FILENAME "flag_direction_red.egp"
#define FLAG_GUIDE_ARROW_EFFECT_MIN_DISTANCE 200.0f
#define FLAG_GUIDE_ARROW_BLINK_TIME 0.3f

CaptureTheFlagEffectMan::CaptureTheFlagEffectMan(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pEngineDevice(pEngineDevice)
    , m_fTime(0.0f)
    , m_bBlinkBlueFlag(false)
    , m_bBlinkRedFlag(false)
    , m_fBlinkTimer(0.0f)
{
}

CaptureTheFlagEffectMan::~CaptureTheFlagEffectMan()
{
}

void CaptureTheFlagEffectMan::CreateSubControl()
{
    CreateFlagArrowEffect(m_pEngineDevice->GetDevice());
}

void CaptureTheFlagEffectMan::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    if ( (m_spBlueFlagArrowEffect.get() == NULL) | (m_spRedFlagArrowEffect.get() == NULL) )
        return;

    GLCharacter* pChar = m_pGaeaClient->GetCharacter();
    m_spBlueFlagArrowEffect->SetTargetID ( &STARGETID(CROW_PC,pChar->GetGaeaID()) );
    m_spRedFlagArrowEffect->SetTargetID ( &STARGETID(CROW_PC,pChar->GetGaeaID()) );

    m_fTime += fElapsedTime;
    m_spRedFlagArrowEffect->FrameMove( m_fTime, fElapsedTime);
    m_spBlueFlagArrowEffect->FrameMove( m_fTime, fElapsedTime);

    m_fBlinkTimer += fElapsedTime;
    if ( m_fBlinkTimer > FLAG_GUIDE_ARROW_BLINK_TIME )
    {
        m_bBlinkState = !m_bBlinkState;
        m_fBlinkTimer = 0.0f;
    }
}

HRESULT CaptureTheFlagEffectMan::CreateFlagArrowEffect( LPDIRECT3DDEVICEQ pd3dDevice )
{
    if ( pd3dDevice == NULL )
        return E_FAIL;

    //	Note : Prop Group 을 가져옴.
    DxEffSinglePropGroupResource BluePropGroup = DxEffSinglePropGMan::GetInstance().LoadEffectGProp ( BLUE_FLAG_GUIDE_ARROW_EFFECT_FILENAME, false, false );
    if ( BluePropGroup.IsValid() == NULL )
    {
        return E_FAIL;
    }

    DxEffSinglePropGroupResource RedPropGroup = DxEffSinglePropGMan::GetInstance().LoadEffectGProp ( RED_FLAG_GUIDE_ARROW_EFFECT_FILENAME, false, false );
    if ( RedPropGroup.IsValid() == NULL )
    {
        return E_FAIL;
    }

    //	Note : Eff Group instance 생성.
    //
    m_spBlueFlagArrowEffect = BluePropGroup.get()->NEWEFFGROUP ();
    if ( m_spBlueFlagArrowEffect.get() == NULL )	
        return E_FAIL;

    m_spRedFlagArrowEffect = RedPropGroup.get()->NEWEFFGROUP ();
    if ( m_spRedFlagArrowEffect.get() == NULL )	
        return E_FAIL;

    GLCharacter* pChar = m_pGaeaClient->GetCharacter();
    //	Note : 타겟지정.
    //
    m_spBlueFlagArrowEffect->SetTargetID ( &STARGETID(CROW_PC,pChar->GetGaeaID()) );
    m_spRedFlagArrowEffect->SetTargetID ( &STARGETID(CROW_PC,pChar->GetGaeaID()) );

    //	Note : Eff Group의 매트릭스 트랜스폼 지정.
    //
    
    D3DXMatrixIdentity(&m_spBlueFlagArrowEffect->m_matWorld);
    D3DXMatrixIdentity(&m_spRedFlagArrowEffect->m_matWorld);

    //	Note : Eff Group Create Dev.
    //
    m_spBlueFlagArrowEffect->Create ( pd3dDevice );
    m_spRedFlagArrowEffect->Create ( pd3dDevice );

    return S_OK;
}

HRESULT CaptureTheFlagEffectMan::Render( LPDIRECT3DDEVICEQ pd3dDevice )
{
    if ( pd3dDevice == NULL )
        return E_FAIL;

    CUIGroup::Render(pd3dDevice);

    if ( IsVisible() )
    {
        if ( (m_spBlueFlagArrowEffect.get() == NULL) | (m_spRedFlagArrowEffect.get() == NULL) )
            return S_OK;

        if ( (bVisibleBlueFlag == false) && (bVisibleRedFlag == false) )
            return S_OK;

        GLCharacter* pChar = m_pGaeaClient->GetCharacter();
        const D3DXVECTOR3& vPos = pChar->GetPosition();

		ClientActor* _pBlueActor = m_pGaeaClient->GetCrow(m_vBlueFlagGuideArrowTarget.emCrow, m_vBlueFlagGuideArrowTarget.GaeaId);
		if ( _pBlueActor )
		{
			m_vBlueFlagGuideArrowTarget.vPos = _pBlueActor->GetPosition();
		}
        else
        {
             GLPartyClient* _pParty = m_pGaeaClient->GetMyPartyClient();
             if ( _pParty )
             {
                 GLPartySlotClient* _pSlot = _pParty->GetMemberByGaeaID( m_vBlueFlagGuideArrowTarget.GaeaId );
                 if ( _pSlot )
                     m_vBlueFlagGuideArrowTarget.vPos = _pSlot->m_vPosition;
             }
        }

         D3DXVECTOR3 vBlueTarDir = m_vBlueFlagGuideArrowTarget.vPos - vPos;
        vBlueTarDir.y = 0;
        const bool bBlueRender(D3DXVec3LengthSq(&vBlueTarDir) > FLAG_GUIDE_ARROW_EFFECT_MIN_DISTANCE);
        if ( bBlueRender )
        {
            D3DXVec3Normalize(&vBlueTarDir, &vBlueTarDir);
            D3DXVECTOR3 vOriginDir(0.0f, 0.0f, 1.0f);
            const float fDot = D3DXVec3Dot(&vBlueTarDir, &vOriginDir);
            float fRadian = acos(fDot);
            D3DXVECTOR3 vCross;
            D3DXVec3Cross(&vCross, &vOriginDir, &vBlueTarDir);
            if ( vCross.y < 0.0f )
                fRadian *= -1;

            D3DXMATRIX matRotate;
            D3DXMatrixRotationY(&matRotate, fRadian);
            m_spBlueFlagArrowEffect->m_matWorld = matRotate;

            m_spBlueFlagArrowEffect->m_matWorld._41 = vPos.x;
            m_spBlueFlagArrowEffect->m_matWorld._42 = vPos.y;
            m_spBlueFlagArrowEffect->m_matWorld._43 = vPos.z;
        }

        ClientActor* _pRedActor = m_pGaeaClient->GetCrow(m_vRedFlagGuideArrowTarget.emCrow, m_vRedFlagGuideArrowTarget.GaeaId);
        if ( _pRedActor )
        {
            m_vRedFlagGuideArrowTarget.vPos = _pRedActor->GetPosition();
        }
        else
        {
            GLPartyClient* _pParty = m_pGaeaClient->GetMyPartyClient();
            if ( _pParty )
            {
                GLPartySlotClient* _pSlot = _pParty->GetMemberByGaeaID( m_vRedFlagGuideArrowTarget.GaeaId );
                if ( _pSlot )
                    m_vRedFlagGuideArrowTarget.vPos = _pSlot->m_vPosition;
            }
        }

        D3DXVECTOR3 vRedTarDir = m_vRedFlagGuideArrowTarget.vPos - vPos;
        vRedTarDir.y = 0;
        const bool bRedRender(D3DXVec3LengthSq(&vRedTarDir) > FLAG_GUIDE_ARROW_EFFECT_MIN_DISTANCE);
        if ( bRedRender )
        {
            D3DXVec3Normalize(&vRedTarDir, &vRedTarDir);
            D3DXVECTOR3 vOriginDir(0.0f, 0.0f, 1.0f);
            const float fDot = D3DXVec3Dot(&vRedTarDir, &vOriginDir);
            float fRadian = acos(fDot);
            D3DXVECTOR3 vCross;
            D3DXVec3Cross(&vCross, &vOriginDir, &vRedTarDir);
            if ( vCross.y < 0.0f )
                fRadian *= -1;

            D3DXMATRIX matRotate;
            D3DXMatrixRotationY(&matRotate, fRadian);
            m_spRedFlagArrowEffect->m_matWorld = matRotate;

            m_spRedFlagArrowEffect->m_matWorld._41 = vPos.x;
            m_spRedFlagArrowEffect->m_matWorld._42 = vPos.y;
            m_spRedFlagArrowEffect->m_matWorld._43 = vPos.z;
        }

        DWORD bZenable;
        DWORD bZwriteEable;
        DWORD dwZfunc;
        pd3dDevice->GetRenderState( D3DRS_ZENABLE, &bZenable);
        pd3dDevice->GetRenderState( D3DRS_ZWRITEENABLE, &bZwriteEable);
        pd3dDevice->GetRenderState( D3DRS_ZFUNC, &dwZfunc);

        pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
        pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
        pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

        if ( bBlueRender && bVisibleBlueFlag && ( !m_bBlinkBlueFlag || m_bBlinkState ) )
            m_spBlueFlagArrowEffect->Render(pd3dDevice, D3DXVECTOR3(1.0f,1.0f,1.0f), 0.86f);

        if ( bRedRender && bVisibleRedFlag && ( !m_bBlinkRedFlag || m_bBlinkState ) )
            m_spRedFlagArrowEffect->Render(pd3dDevice, D3DXVECTOR3(1.0f,1.0f,1.0f), 0.86f);

        pd3dDevice->SetRenderState( D3DRS_ZENABLE, bZenable );
        pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, bZwriteEable );
        pd3dDevice->SetRenderState( D3DRS_ZFUNC, dwZfunc);
    }

    return S_OK;
}

void CaptureTheFlagEffectMan::SetFlagGuideArrow(const int _flagType, const STARGETID& sTargetID, const bool bBlink )
{
    switch(_flagType)
    {
    case GLMSG::NET_INSTANCE_CTF_FLAG_UPDATE_FC::EM_BLUE_FLAG_VISIBLE_ON:
        bVisibleBlueFlag = true;
        m_vBlueFlagGuideArrowTarget = sTargetID;
        m_bBlinkBlueFlag = bBlink;
        break;
    case GLMSG::NET_INSTANCE_CTF_FLAG_UPDATE_FC::EM_RED_FLAG_VISIBLE_ON:
        bVisibleRedFlag = true;
        m_vRedFlagGuideArrowTarget = sTargetID;
        m_bBlinkRedFlag = bBlink;
        break;
    case GLMSG::NET_INSTANCE_CTF_FLAG_UPDATE_FC::EM_BLUE_FLAG_VISIBLE_OFF:
        bVisibleBlueFlag = false;
        m_bBlinkBlueFlag = false;
        break;
    case GLMSG::NET_INSTANCE_CTF_FLAG_UPDATE_FC::EM_RED_FLAG_VISIBLE_OFF:
        bVisibleRedFlag = false;
        m_bBlinkRedFlag = false;
        break;
    case GLMSG::NET_INSTANCE_CTF_FLAG_UPDATE_FC::EM_ALL_FLAG_VISIBLE_OFF:
        bVisibleBlueFlag = false;
        bVisibleRedFlag = false;
        m_bBlinkBlueFlag = false;
        m_bBlinkRedFlag = false;
        break;
    }   
}



MyCaptureTheFlagEffectMan::MyCaptureTheFlagEffectMan( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CaptureTheFlagEffectMan(pGaeaClient, pInterface, pEngineDevice)
{
}

void MyCaptureTheFlagEffectMan::CreateUIWindowAndRegisterOwnership()
{
    CaptureTheFlagEffectMan::Create( CAPTURE_THE_FLAG_EFFECT_MAN, "SIMPLE_SYSTEM_MESSAGE_TEXT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    CaptureTheFlagEffectMan::CreateSubControl();
    CaptureTheFlagEffectMan::SetAlignFlag( UI_FLAG_CENTER_Y | UI_FLAG_CENTER_X );		
    CaptureTheFlagEffectMan::m_pInterface->UiRegisterControl( this, true );
    CaptureTheFlagEffectMan::m_pInterface->UiShowGroupBottom( CUIGroup::GetWndID() );
    CaptureTheFlagEffectMan::SetVisibleSingle(FALSE);
}

void MyCaptureTheFlagEffectMan::SetFlagGuideArrow(const int _flagNum, const STARGETID& sTargetID, const bool bBlink )
{
    CaptureTheFlagEffectMan::SetFlagGuideArrow(_flagNum, sTargetID, bBlink);
    CaptureTheFlagEffectMan::SetVisibleSingle(TRUE);
}
