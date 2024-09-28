#include "../pch.h"

#include "../../RanLogic/LandEvent/GLLandEventGenerator.h"
#include "./GLLandEventNodeClient.h"

#include "../../EngineLib/DxEffect/Single/DxEffGroupPlayer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//-------------------------------------------------------------------//

void CEVENT_NODE_CLIENT_SUMMONGATE::Restore()
{
	SLANDEVENT_GENERATOR<CEVENT_NODE_CLIENT_SUMMONGATE>::Restore(this);
}

void CEVENT_NODE_CLIENT_SUMMONGATE::Clean()
{
	DxEffGroupPlayer::GetInstance().DeletePassiveEffect(m_sEvent);
}

void CEVENT_NODE_CLIENT_SUMMONGATE::FrameStart()
{
	D3DXMATRIX matTrans;

	// NewEffGroup - bBackUpCache 내자신인지 다른 캐릭터가 했는지 몰라서 TRUE를 줬다.
	//							원래는 내자신일 경우 TRUE, 그 외는 FALSE 를 주고 싶었음.
	
    if ( m_strEffectName.empty() )  //이펙트이름이 없을시 이펙트가 없는것으로 간주 ( ex. 컬링된 지점에서 소환문이펙트가 열릴 때 )
    {
        return;
    }
	D3DXMatrixTranslation( &matTrans, m_sEvent.vPos.x, m_sEvent.vPos.y + D3DX_16F_EPSILON, m_sEvent.vPos.z );
	DxEffGroupPlayer::GetInstance().NewEffGroup( m_strEffectName.c_str(), matTrans, &m_sEvent, FALSE, FALSE );
}

void CEVENT_NODE_CLIENT_SUMMONGATE::FrameEvent()
{
	//Blank
}

void CEVENT_NODE_CLIENT_SUMMONGATE::FrameEnd()
{
	//Blank
}

//-------------------------------------------------------------------//
