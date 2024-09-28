
#include "pch.h"

#include "../../EngineLib/DxTools/DxViewPort.h"
#include "../../Enginelib/DxTools/DxInputDevice.h"

#include "../../SigmaCore/Log/LogMan.h"

#include "../Char/GLCharacter.h"
#include "../Land/GLLandManClient.h"
#include "../ClientActor.h"
#include "../GLGaeaClient.h"

#include "../../RanLogic/FlyCamera/FlyCameraDefine.h"
#include "./GLFlyCameraBaseMode.h"
#include "./GLFlyCameraControl.h"
#include "./GLFlyCameraFollowMode.h"

GLFlyCameraFollowMode::GLFlyCameraFollowMode( GLGaeaClient* pGaeaClient, GLFlyCameraControl* pParentCtrl )
: GLFlyCameraBaseMode(pGaeaClient, pParentCtrl, FlyCameraControl::MODETYPE_FOLLOWMODE)
{
}

GLFlyCameraFollowMode::~GLFlyCameraFollowMode(void)
{
}

void GLFlyCameraFollowMode::Init(void)
{
}

void GLFlyCameraFollowMode::MsgProcedures( DWORD dwType, DWORD dwMsg )
{
	switch ( dwType )
	{
	case FlyCameraControl::EMFLYCAMERA_SET_FOLLOWTARGET:
		{
			const DWORD dwGaeaID = dwMsg;
			this->SetSelectTarget(dwGaeaID);

			//m_fInterpT = 2.0f;
			m_fAccumT = 0.0f;
		}
		break;
	}
}

void GLFlyCameraFollowMode::Update( float fTime, float fElapsedTime )
{
	if ( m_sTargetID.GaeaId == EMTARGET_NULL )
		return;

	STARGETID sTargetID = m_sTargetID;
	ClientActor* pActor = m_pGaeaClient->GetCrow(m_sTargetID.emCrow, m_sTargetID.GaeaId);
	if ( !pActor )
	{
		std::string ErrorMsg("GLFlyCameraFollowMode::Update, Actor is NULL");
		sc::writeLogError(ErrorMsg);
		return ;
	}
	m_fAccumT += fElapsedTime;

	m_sTargetID.vPos = pActor->GetPosition();

	const D3DXVECTOR3 vMoveInterValue = m_pParentCtrl->GetMoveInterValue()
		, vRotateInterValue = m_pParentCtrl->GetRotateInterValue();
	D3DXVECTOR3 vMove, vRotate;
	const float fMoveInter = (m_pParentCtrl->GetMoveInterTime() == 0.0f) ? 1.0f : max( max( fabs(vMoveInterValue.x), fabs(vMoveInterValue.y) ), fabs(vMoveInterValue.z) )
		, fRotateInter = (m_pParentCtrl->GetRotateInterTime() == 0.0f) ? 1.0f : max( max( fabs(vRotateInterValue.x), fabs(vRotateInterValue.y) ), fabs(vRotateInterValue.z) )
		, fMoveVelo = m_pParentCtrl->GetMoveVelo()
		, fRotateVelo = m_pParentCtrl->GetRotateVelo();

	// 이동 처리; Follow 모드에서는 움직일 시 타겟을 기준으로 뱅글뱅글 돔;
	D3DXVECTOR3 vNorMoveInter;
	D3DXVec3Normalize(&vNorMoveInter, &vMoveInterValue);
	D3DXVECTOR3 vMove02 = vNorMoveInter * fMoveInter  * fMoveVelo * fElapsedTime;

	D3DXVec3Normalize(&vMove, &vMoveInterValue);
	vMove = vMove * fMoveInter * D3DXToRadian(fMoveVelo) * fElapsedTime;

	m_sTargetID = sTargetID;
	m_vReservMove = D3DXVECTOR3( vMove.y, -vMove.x, -vMove02.z );

	//
	m_pParentCtrl->AddAfterAction(FlyCameraControl::EMFLYCAMERA_AFTER_ACTION_MOVE_FOLLOWMODE);

	// ClientActor* pActor = m_pGaeaClient->GetCrow(sTargetID.emCrow, sTargetID.GaeaId);
	D3DXVECTOR3 vJump;
	float fTargetChangeInterT = m_pParentCtrl->GetTargetChangeInterTime();
	if ( m_fAccumT >= fTargetChangeInterT )
	{
		m_fAccumT = fTargetChangeInterT;
		vJump = pActor->GetPosition();
		vJump.y += 10.0f;
	}
	else
	{
		D3DXVECTOR3 vLookPt = DxViewPort::GetInstance().GetLookatPt()
			, vActorPos = pActor->GetPosition() + D3DXVECTOR3(0.f, 10.0f, 0.f)
			, vLength = vActorPos - vLookPt
			, vDir;
		D3DXVec3Normalize(&vDir, &vLength);
		float fLength = D3DXVec3Length( &vLength );
		vJump = vLookPt + vDir * ( fLength * (fElapsedTime/(fTargetChangeInterT - m_fAccumT)) );
	}
// 	D3DXVECTOR3 vJump = pActor->GetPosition();
// 	vJump.y += 10.0f;
	DxViewPort::GetInstance().CameraJump(vJump);
}

void GLFlyCameraFollowMode::FollowModeMove(const D3DXVECTOR3& vMove)
{
	D3DXVECTOR3 vFromPt = DxViewPort::GetInstance().GetFromPt()
			  , vLookAt = DxViewPort::GetInstance().GetLookatPt()
			  , vCamDir = vFromPt-vLookAt;

	float fLegnth = D3DXVec3Length(&vCamDir);
	if ( fLegnth == 0.0f )
		fLegnth = 0.001f;
	
	float fThetaX = static_cast<float>(asin(vCamDir.y/fLegnth));
	float fLengthXZ = static_cast<float>(sqrt(vCamDir.x*vCamDir.x + vCamDir.z*vCamDir.z));
	if ( fLengthXZ == 0.0f )	
		fLengthXZ = 0.001f;

	float fThetaY = static_cast<float>(acos(vCamDir.x / fLengthXZ));
	if ( vCamDir.z > 0.0f )	
		fThetaY = -fThetaY;

	fThetaX += vMove.x;
	fThetaY += vMove.y;

// 	const static float fLIMIT = D3DX_PI/2.0f - 0.001f;
// 	if ( fLIMIT < fThetaX )			
// 		fThetaX = fLIMIT;
// 	else if ( -fLIMIT > fThetaX )	
// 		fThetaX = -fLIMIT;

	D3DXVECTOR3 vNDir;
	// 피치;
	vNDir.y = (float) sin(fThetaX);

	vNDir.x = 1-vNDir.y*vNDir.y;
	vNDir.x = ( vNDir.x < 0 ) ? 0 : (float) sqrt(vNDir.x);

	vNDir.z = (float) (-vNDir.x*sin(fThetaY) );
	vNDir.x = (float) ( vNDir.x*cos(fThetaY) );

	D3DXVECTOR3 vNextPt = vLookAt + vNDir * fLegnth;
	
// 	D3DXVECTOR3 vDir, vUp, vRight;
// 	DxViewPort::GetInstance().GetViewTrans(vDir,vRight,vUp);
// 	vDir = vCamDir;
// 	D3DXVec3Cross(&vRight, &vUp, &vDir);
// 	m_vReservMove;


	const D3DXMATRIX& matView = DxViewPort::GetInstance().GetMatView();
	D3DXVECTOR3
		vRight = D3DXVECTOR3(matView._11, matView._21, matView._31),
		vUpVec = D3DXVECTOR3(matView._12, matView._22, matView._32),
		vFront = D3DXVECTOR3(matView._13, matView._23, matView._33),
		vSub = vNextPt - vFromPt,
		vAtrb =   D3DXVECTOR3(vSub.x*vRight.x, vSub.y*vRight.y, vSub.z*vRight.z)
				+ D3DXVECTOR3(vSub.x*vUpVec.x, vSub.y*vUpVec.y, vSub.z*vUpVec.z)
				+ D3DXVECTOR3(vSub.x*vFront.x, vSub.y*vFront.y, vSub.z*vFront.z);

	if ( vMove.x != 0.0f || vMove.y != 0.0f || vMove.z != 0.0f )
	{
		// DxViewPort::GetInstance().CameraMove(vMove);
		m_vReservMove = vAtrb;
		m_pParentCtrl->AddAfterAction(FlyCameraControl::EMFLYCAMERA_AFTER_ACTION_MOVE_FOLLOWMODE);
	}
}

BOOL GLFlyCameraFollowMode::SetSelectTarget( const DWORD dwGaeaID )
{
	GLLandManClient* const pLand = m_pGaeaClient->GetLandManClient();
	STARGETID sTargetID(CROW_NULL, dwGaeaID);
	ClientActor* pActor = NULL;
	GLAnyPet* pPet = NULL;
	CItemClientDrop* pItem = NULL;
	CMoneyClientDrop* pMoney = NULL;

	if ( m_pGaeaClient->GetCharacter()->GetGaeaID() == dwGaeaID )
		sTargetID.emCrow = CROW_PC;
	else if ( (pActor = pLand->GetChar(dwGaeaID).get()) )
		sTargetID.emCrow = CROW_PC;
	else if ( (pActor = pLand->GetCrow(dwGaeaID).get()) )		// CROW_MOB | CROW_NPC
		sTargetID.emCrow = pActor->GetCrow();
	else if ( (pActor = pLand->GetSummon(dwGaeaID).get()) )
		sTargetID.emCrow = CROW_SUMMON;
	else
		return FALSE;
	
	m_sTargetID = sTargetID;
	return TRUE;
}