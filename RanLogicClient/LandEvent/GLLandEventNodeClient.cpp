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

	// NewEffGroup - bBackUpCache ���ڽ����� �ٸ� ĳ���Ͱ� �ߴ��� ���� TRUE�� ���.
	//							������ ���ڽ��� ��� TRUE, �� �ܴ� FALSE �� �ְ� �;���.
	
    if ( m_strEffectName.empty() )  //����Ʈ�̸��� ������ ����Ʈ�� ���°����� ���� ( ex. �ø��� �������� ��ȯ������Ʈ�� ���� �� )
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
