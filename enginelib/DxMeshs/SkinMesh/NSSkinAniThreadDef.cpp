#include "pch.h"

#include "../../DxTools/MaterialSkin/NSMaterialSkinManager.h"
#include "../../DxLand/DxLandMan.h"
#include "../FrameMesh/DxFrameMesh.h"

#include "NSSkinAniThreadDef.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------




//////////////////////////////////////////////////////////////////////////
//
void ActorNameCollision::Collision( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd )
{
	if ( m_nCalculateIndex == -1 )
		return;

	boost::shared_ptr<DxLandMan> spLandMan = m_wpLandMan[m_nCalculateIndex].lock();
	if ( !spLandMan )
		return;

	D3DXVECTOR3 vColl;
	DxFrame* pDxFrame(NULL);

	spLandMan->IsCollision( vStart, vEnd, vColl, m_bCollision[m_nCalculateIndex], pDxFrame, static_cast<EMCOLLMODE>(m_dwCollMode) );
}

BOOL ActorNameCollision::GetCollsion() const
{
	// [shhan][2015.06.25] 시야가 없을 경우, 이름 표시를 그냥 해주도록 하자.
	//						추후 기획팀에서 지형충돌 되게 해달라고 할 수 있다. 
	//						속도를 위해 계산을 안하는 거니 잘 생각이 필요함.
	if ( DxFogMan::GetInstance().GetFogRange() == EMFR_OFF )
		return FALSE;

	return m_bCollisionResult;
}




//////////////////////////////////////////////////////////////////////////
//
void ActorNameCollisionThread::Collision()
{
	m_spNameCollision->Collision( m_vStart, m_vEnd );
}

void ActorNameCollisionThread::SwapNameCollision()
{
	m_spNameCollision->SwapNameCollision();
}







//////////////////////////////////////////////////////////////////////////
//
ActorWorldLight::ActorWorldLight()
	: m_nCalculateIndex(-1)
	, m_nGetIndex(0)
	, m_vDiffuseResult(1.f,1.f,1.f)
	, m_vPointLightResult(0.f,0.f,0.f)
	, m_bNotCalculateColor(TRUE)
{
	m_vDiffuse[0] = D3DXVECTOR3(1.f,1.f,1.f);
	m_vDiffuse[1] = D3DXVECTOR3(1.f,1.f,1.f);

	m_vPointLight[0] = D3DXVECTOR3(0.f,0.f,0.f);
	m_vPointLight[1] = D3DXVECTOR3(0.f,0.f,0.f);
}

void ActorWorldLight::CalculateActorWorldLight( const D3DXVECTOR3& vPos )
{
	if ( m_nCalculateIndex == -1 )
		return;

	boost::shared_ptr<DxLandMan> spLandMan = m_wpLandMan[m_nCalculateIndex].lock();
	if ( !spLandMan )
		return;

	//if ( !m_spLandMan[m_nCalculateIndex] )
	//	return;

	spLandMan->CalculateActorWorldLight_Thread( m_vDiffuse[m_nCalculateIndex], m_vPointLight[m_nCalculateIndex], vPos );
}

// Char 쪽에서 쓰이는 것.
void ActorWorldLight::InitData( const boost::shared_ptr<DxLandMan>& spLandMan )
{
	//// [shhan][2014.11.10] 맵이동을 하면 이전 셋팅이 날라가는 문제가 있다.
	//// 완전 땜빵코드다.
	//// 구조를 바꾸자.
	//BOOL bDeleteOldMap(FALSE);
	//if ( m_spLandMan[m_nGetIndex].use_count() == 1 && 
	//	m_spLandMan[m_nGetIndex].get() != spLandMan.get() )
	//{
	//	bDeleteOldMap = TRUE;
	//}

	// 
	m_wpLandMan[m_nGetIndex] = spLandMan;

	//// [shhan][2015.03.24] 처음 맵에 학원광장에 이동하면 이전 로그인 맵으로 인해 m_spLandMan[m_nGetIndex] = spLandMan; 에서 
	////						NSLandThread 관련 Delete 가 발생하고 그로 인해 렌더링 버그가 생긴다. 그래서 spLandMan->SetNSLandThread( pd3dDevice ); 를 추가해준다.
	//// [shhan][2014.11.10] 맵이동을 하면 이전 셋팅이 날라가는 문제가 있다.
	//// 완전 땜빵코드다.
	//// 구조를 바꾸자.
	//if ( bDeleteOldMap )
	//{
	//	spLandMan->ActiveMap();
	//	spLandMan->SetNSLandThread();
	//}
}




//////////////////////////////////////////////////////////////////////////
//
void ActorWorldLightThread::CalculateActorWorldLight()
{
	m_spActorWorldLight->CalculateActorWorldLight( m_vPos );
}

void ActorWorldLightThread::SwapActorWorldLight()
{
	m_spActorWorldLight->SwapActorWorldLight();
}