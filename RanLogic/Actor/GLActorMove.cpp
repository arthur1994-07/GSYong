#include "pch.h"
#include "../../EngineLib/NaviMesh/navigationpath.h"
#include "../../EngineLib/DxTools/DxMethods.h"

#include "../GLLevelFile.h"

#include "GLActor.h"

// ActorMove;
void GLActor::_ResetActorMove(void)
{
	m_vPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vDirect = D3DXVECTOR3(0.0f, 0.0f, -1.0f);	
	m_actorMove.Release();
	D3DXMatrixIdentity(&m_matTrans);
}

void GLActor::InitializeActorMove(const D3DXVECTOR3& vPosition, boost::shared_ptr<NavigationMesh> spNavi, const D3DXVECTOR3& vDirect)
{
	const float fThetaY = DXGetThetaYFromDirection(vDirect, m_vDirectOrig);

	D3DXMATRIX matTrans, matYRot;	
	D3DXMatrixTranslation ( &matTrans, vPosition.x, vPosition.y, vPosition.z );
	D3DXMatrixRotationY ( &matYRot, fThetaY );

	m_matTrans = matYRot * matTrans;
	m_vPosition = vPosition;
	m_vDirect = vDirect;

	if ( spNavi != NULL )
		m_actorMove.Create(spNavi, vPosition);	
}
void GLActor::InitializeActorMove(const D3DXVECTOR3& vPosition, boost::shared_ptr<NavigationMesh> spNavi)
{
	if ( spNavi )
		m_actorMove.Create(spNavi, vPosition);
	else
		m_actorMove.SetPosition(vPosition, -1);

	if ( m_actorMove.PathIsActive() )
		m_actorMove.Stop();	
	m_vPosition = vPosition;
}
void GLActor::InitializeActorMove(const D3DXVECTOR3& vPosition)
{
	m_actorMove.SetPosition(vPosition, -1);

	if ( m_actorMove.PathIsActive() )
		m_actorMove.Stop();	
	m_vPosition = vPosition;
}

void GLActor::RenderActorMove(LPDIRECT3DDEVICEQ pd3dDevice)
{
	m_actorMove.Render(pd3dDevice);
}
void GLActor::CreateActorMove(boost::shared_ptr<NavigationMesh> spNavi, const D3DXVECTOR3& vPosition, const bool bInstant /* = false */, const DWORD CurrentCellID /* = -1 */)
{
	m_actorMove.Create(spNavi, vPosition, bInstant, CurrentCellID);
}

HRESULT GLActor::Update( const std::vector<boost::shared_ptr<LayerCollision>>& vecLayerCollision, float fElapsedTime, BOOL bAccuracyUp )
{
	if ( m_bLockMoveMent == false )
		return m_actorMove.Update( vecLayerCollision, fElapsedTime, bAccuracyUp );

	m_actorMove.Stop();
    return S_OK;
}
void GLActor::StopActorMove(void)
{
	m_actorMove.Stop();
}
const bool GLActor::IsCollision(const D3DXVECTOR3& vPoint1, const D3DXVECTOR3& vPoint2, D3DXVECTOR3& vCollision, DWORD& CollisionID, BOOL& bCollision) const
{
	const NavigationMesh* const pNavi = m_actorMove.GetParentMesh();
	if ( pNavi == NULL )
		return false;

	pNavi->IsCollision( vPoint1, vPoint2, vCollision, CollisionID, bCollision );
	return true;
}
const bool GLActor::GotoLocation(const D3DXVECTOR3& vPoint, const float fDeviation)
{
	const D3DXVECTOR3 vDeviation(0.0f, fDeviation, 0.0f);
	return this->m_actorMove.GotoLocation(vPoint + vDeviation, vPoint - vDeviation) ? true : false;
}

const bool GLActor::LineOfSightTest( const D3DXVECTOR3& vPoint, const float fDeviation )
{
	const D3DXVECTOR3 vDeviation(0.0f, fDeviation, 0.0f);
	return this->m_actorMove.LineOfSightTest( vPoint + vDeviation, vPoint - vDeviation ) ? true : false;
}

const bool GLActor::PathIsActive(void) const 
{
	return m_actorMove.PathIsActive();
}
const bool GLActor::GetAABB(D3DXVECTOR3& vMax, D3DXVECTOR3& vMin)
{
	const NavigationMesh* const pNavi = m_actorMove.GetParentMesh();
	if ( pNavi == NULL )
		return false;

	pNavi->GetAABB(vMax, vMin);
	return true;
}
const D3DXVECTOR3& GLActor::RefreshPosition(const float fPrecisionY)
{	
	return (m_vPosition = this->m_actorMove.Position() + D3DXVECTOR3(0.0f, fPrecisionY, 0.0f));
}

const D3DXVECTOR3& GLActor::GetDirectOrig(void) const
{
	return m_vDirectOrig;
}
const D3DXVECTOR3& GLActor::GetDirect(void) const
{
	return m_vDirect;
}
const D3DXVECTOR3& GLActor::GetPosition(void) const
{
	return m_vPosition;
}
const D3DXVECTOR3& GLActor::GetNextPosition(void) const
{
	return m_actorMove.NextPosition();
}
const D3DXVECTOR3& GLActor::GetPositionActor(void) const
{
	return m_actorMove.Position();
}
const D3DXMATRIX& GLActor::GetTransMatrix(void) const
{
	return m_matTrans;
}
const DWORD GLActor::GetCurrentCellID(void) const
{
	return m_actorMove.CurrentCellID();
}
const NavigationMesh* const GLActor::GetParentMesh(void) const
{
	return m_actorMove.GetParentMesh();
}


void GLActor::SetNextWayPoint(const NavigationPath::WAYPOINT& WayPoint)
{
	m_actorMove.SetNextWayPoint(WayPoint);
}
void GLActor::SetDirect(const D3DXVECTOR3& vDirect)
{
	m_vDirect = vDirect;
}
const bool GLActor::SetPositionActor(const D3DXVECTOR3& vPosition, const DWORD dwCurrentCellID /* = -1 */)
{
	m_actorMove.SetPosition(vPosition, dwCurrentCellID);
    GLActor::SetPosition( vPosition );
	return m_actorMove.PathIsActive();
}
void GLActor::SetPosition(const D3DXVECTOR3& vPosition)
{
	m_vPosition = vPosition;
}
void GLActor::SetTransMatrix(const D3DXMATRIX& matTrans)
{
	m_matTrans = matTrans;
}
void GLActor::SetMaxSpeed(const float fMaxSpeed)
{
	m_actorMove.SetMaxSpeed(fMaxSpeed);
}

// ActorMove;
