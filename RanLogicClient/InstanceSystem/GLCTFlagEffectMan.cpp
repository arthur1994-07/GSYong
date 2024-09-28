#include "pch.h"
#include "GLCTFlagEffectMan.h"

#include "../GLGaeaClient.h"
#include "../Char/GLCharacter.h"
#include "../Party/GLPartyClient.h"

#define BLUE_FLAG_GUIDE_ARROW_EFFECT_FILENAME "flag_direction_blue.egp"
#define RED_FLAG_GUIDE_ARROW_EFFECT_FILENAME "flag_direction_red.egp"
#define FLAG_GUIDE_ARROW_EFFECT_MIN_DISTANCE 200.0f
#define FLAG_GUIDE_ARROW_BLINK_TIME 0.3f

GLCTFlagEffectMan::GLCTFlagEffectMan( GLGaeaClient* pClient )
: m_pClient( pClient )
, m_fTime( 0.0f )
, m_bVisibleBlueFlag( false )
, m_bVisibleRedFlag( false )
, m_bBlinkBlueFlag (false )
, m_bBlinkRedFlag( false )
, m_fBlinkTimer( 0.0f )
, m_bUse ( false )
{
}

GLCTFlagEffectMan::~GLCTFlagEffectMan()
{
}

void GLCTFlagEffectMan::OnInit( LPDIRECT3DDEVICEQ pDevice )
{
	m_pDevice = pDevice;
	if( NULL == m_pDevice )
		return;

	//	Note : Prop Group 을 가져옴.
	DxEffSinglePropGroupResource BluePropGroup = DxEffSinglePropGMan::GetInstance().LoadEffectGProp( 
		BLUE_FLAG_GUIDE_ARROW_EFFECT_FILENAME, false, false );
	if( NULL == BluePropGroup.IsValid() )
		return;

	DxEffSinglePropGroupResource RedPropGroup = DxEffSinglePropGMan::GetInstance().LoadEffectGProp( 
		RED_FLAG_GUIDE_ARROW_EFFECT_FILENAME, false, false );
	if( NULL == RedPropGroup.IsValid() )
		return;

	//	Note : Eff Group instance 생성.
	m_spBlueFlagArrowEffect = BluePropGroup.get()->NEWEFFGROUP();
	if( NULL == m_spBlueFlagArrowEffect.get() )	
		return;

	m_spRedFlagArrowEffect = RedPropGroup.get()->NEWEFFGROUP();
	if( NULL == m_spRedFlagArrowEffect.get() )
		return;

	GLCharacter* pChar = m_pClient->GetCharacter();
	//	Note : 타겟지정.
	m_spBlueFlagArrowEffect->SetTargetID( &STARGETID( CROW_PC, pChar->GetGaeaID() ) );
	m_spRedFlagArrowEffect->SetTargetID( &STARGETID( CROW_PC, pChar->GetGaeaID() ) );

	//	Note : Eff Group의 매트릭스 트랜스폼 지정.
	D3DXMatrixIdentity( &m_spBlueFlagArrowEffect->m_matWorld );
	D3DXMatrixIdentity( &m_spRedFlagArrowEffect->m_matWorld );

	//	Note : Eff Group Create Dev.
	m_spBlueFlagArrowEffect->Create( m_pDevice );
	m_spRedFlagArrowEffect->Create( m_pDevice );
}

void GLCTFlagEffectMan::OnFrameMove( float fElapsedTime )
{
	if( false == m_bVisibleBlueFlag && false == m_bVisibleRedFlag )
		return;

	if( NULL == m_spBlueFlagArrowEffect.get() || NULL == m_spRedFlagArrowEffect.get() )
		return;

	if ( false == m_bUse )
		return;

	GLCharacter* pChar = m_pClient->GetCharacter();
	m_spBlueFlagArrowEffect->SetTargetID( &STARGETID( CROW_PC, pChar->GetGaeaID() ) );
	m_spRedFlagArrowEffect->SetTargetID( &STARGETID( CROW_PC, pChar->GetGaeaID() ) );

	m_fTime += fElapsedTime;
	m_spRedFlagArrowEffect->FrameMove( m_fTime, fElapsedTime);
	m_spBlueFlagArrowEffect->FrameMove( m_fTime, fElapsedTime);

	m_fBlinkTimer += fElapsedTime;
	if( FLAG_GUIDE_ARROW_BLINK_TIME < m_fBlinkTimer )
	{
		m_bBlinkState = !m_bBlinkState;
		m_fBlinkTimer = 0.0f;
	}
}

void GLCTFlagEffectMan::OnFrameRedner()
{
	if( false == m_bVisibleBlueFlag && false == m_bVisibleRedFlag )
		return;

	if( NULL == m_spBlueFlagArrowEffect.get() || NULL == m_spRedFlagArrowEffect.get() )
		return;

	if ( false == m_bUse )
		return;

	GLCharacter* pChar = m_pClient->GetCharacter();
	const D3DXVECTOR3& vPos = pChar->GetPosition();

	ClientActor* pBlueActor = m_pClient->GetCrow( m_vBlueFlagGuideArrowTarget.emCrow, m_vBlueFlagGuideArrowTarget.GaeaId );
	if( pBlueActor )
	{
		m_vBlueFlagGuideArrowTarget.vPos = pBlueActor->GetPosition();
	}
	else
	{
		GLPartyClient* pParty = m_pClient->GetMyPartyClient();
		if( pParty )
		{
			GLPartySlotClient* pSlot = pParty->GetMemberByGaeaID( m_vBlueFlagGuideArrowTarget.GaeaId );
			if( pSlot )
				m_vBlueFlagGuideArrowTarget.vPos = pSlot->m_vPosition;
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

	ClientActor* pRedActor = m_pClient->GetCrow(m_vRedFlagGuideArrowTarget.emCrow, m_vRedFlagGuideArrowTarget.GaeaId);
	if ( pRedActor )
	{
		m_vRedFlagGuideArrowTarget.vPos = pRedActor->GetPosition();
	}
	else
	{
		GLPartyClient* pParty = m_pClient->GetMyPartyClient();
		if ( pParty )
		{
			GLPartySlotClient* _pSlot = pParty->GetMemberByGaeaID( m_vRedFlagGuideArrowTarget.GaeaId );
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
	m_pDevice->GetRenderState( D3DRS_ZENABLE, &bZenable);
	m_pDevice->GetRenderState( D3DRS_ZWRITEENABLE, &bZwriteEable);
	m_pDevice->GetRenderState( D3DRS_ZFUNC, &dwZfunc);

	m_pDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	m_pDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	m_pDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

	if ( bBlueRender && m_bVisibleBlueFlag && ( !m_bBlinkBlueFlag || m_bBlinkState ) )
		m_spBlueFlagArrowEffect->Render(m_pDevice, D3DXVECTOR3(1.0f,1.0f,1.0f), 0.86f);

	if ( bRedRender && m_bVisibleRedFlag && ( !m_bBlinkRedFlag || m_bBlinkState ) )
		m_spRedFlagArrowEffect->Render(m_pDevice, D3DXVECTOR3(1.0f,1.0f,1.0f), 0.86f);

	m_pDevice->SetRenderState( D3DRS_ZENABLE, bZenable );
	m_pDevice->SetRenderState( D3DRS_ZWRITEENABLE, bZwriteEable );
	m_pDevice->SetRenderState( D3DRS_ZFUNC, dwZfunc);
}

void GLCTFlagEffectMan::SetFlagGuideArrow( const int nFlagType, const STARGETID& sTargetID, const bool bBlink )
{
	switch( nFlagType )
	{
	case GLMSG::NET_INSTANCE_CTF_FLAG_UPDATE_FC::EM_BLUE_FLAG_VISIBLE_ON :
		m_bVisibleBlueFlag = true;
		m_vBlueFlagGuideArrowTarget = sTargetID;
		m_bBlinkBlueFlag = bBlink;
		break;
	case GLMSG::NET_INSTANCE_CTF_FLAG_UPDATE_FC::EM_RED_FLAG_VISIBLE_ON :
		m_bVisibleRedFlag = true;
		m_vRedFlagGuideArrowTarget = sTargetID;
		m_bBlinkRedFlag = bBlink;
		break;
	case GLMSG::NET_INSTANCE_CTF_FLAG_UPDATE_FC::EM_BLUE_FLAG_VISIBLE_OFF :
		m_bVisibleBlueFlag = false;
		m_bBlinkBlueFlag = false;
		break;
	case GLMSG::NET_INSTANCE_CTF_FLAG_UPDATE_FC::EM_RED_FLAG_VISIBLE_OFF :
		m_bVisibleRedFlag = false;
		m_bBlinkRedFlag = false;
		break;
	case GLMSG::NET_INSTANCE_CTF_FLAG_UPDATE_FC::EM_ALL_FLAG_VISIBLE_OFF :
		m_bVisibleBlueFlag = false;
		m_bVisibleRedFlag = false;
		m_bBlinkBlueFlag = false;
		m_bBlinkRedFlag = false;
		break;

	case GLMSG::NET_INSTANCE_CTF_FLAG_UPDATE_FC::EM_NAVIGATION_ON:
		m_bUse = true;
		break;

	case GLMSG::NET_INSTANCE_CTF_FLAG_UPDATE_FC::EM_NAVIGATION_OFF:
		m_bUse = false;
		break;
	}
}

void GLCTFlagEffectMan::OnReset ( void )
{
	m_fTime = 0.0f;
	m_bVisibleBlueFlag = false;
	m_bVisibleRedFlag = false;
	m_bBlinkBlueFlag = false;
	m_bBlinkRedFlag = false;
	m_fBlinkTimer = 0.0f;
	m_bUse = false;
}