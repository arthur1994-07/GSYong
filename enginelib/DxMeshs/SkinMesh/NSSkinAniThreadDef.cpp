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
	// [shhan][2015.06.25] �þ߰� ���� ���, �̸� ǥ�ø� �׳� ���ֵ��� ����.
	//						���� ��ȹ������ �����浹 �ǰ� �ش޶�� �� �� �ִ�. 
	//						�ӵ��� ���� ����� ���ϴ� �Ŵ� �� ������ �ʿ���.
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

// Char �ʿ��� ���̴� ��.
void ActorWorldLight::InitData( const boost::shared_ptr<DxLandMan>& spLandMan )
{
	//// [shhan][2014.11.10] ���̵��� �ϸ� ���� ������ ���󰡴� ������ �ִ�.
	//// ���� �����ڵ��.
	//// ������ �ٲ���.
	//BOOL bDeleteOldMap(FALSE);
	//if ( m_spLandMan[m_nGetIndex].use_count() == 1 && 
	//	m_spLandMan[m_nGetIndex].get() != spLandMan.get() )
	//{
	//	bDeleteOldMap = TRUE;
	//}

	// 
	m_wpLandMan[m_nGetIndex] = spLandMan;

	//// [shhan][2015.03.24] ó�� �ʿ� �п����忡 �̵��ϸ� ���� �α��� ������ ���� m_spLandMan[m_nGetIndex] = spLandMan; ���� 
	////						NSLandThread ���� Delete �� �߻��ϰ� �׷� ���� ������ ���װ� �����. �׷��� spLandMan->SetNSLandThread( pd3dDevice ); �� �߰����ش�.
	//// [shhan][2014.11.10] ���̵��� �ϸ� ���� ������ ���󰡴� ������ �ִ�.
	//// ���� �����ڵ��.
	//// ������ �ٲ���.
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