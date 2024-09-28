
#include "pch.h"

#include "../../EngineLib/DxTools/DxViewPort.h"
#include "../../Enginelib/DxTools/DxInputDevice.h"

#include "../GLGaeaClient.h"
#include "../../RanLogic/FlyCamera/FlyCameraDefine.h"
#include "./GLFlyCameraBaseMode.h"
#include "./GLFlyCameraControl.h"
#include "./GLFlyCameraFixedMode.h"

GLFlyCameraFixedMode::GLFlyCameraFixedMode( GLGaeaClient* pGaeaClient, GLFlyCameraControl* pParentCtrl )
: GLFlyCameraBaseMode(pGaeaClient, pParentCtrl, FlyCameraControl::MODETYPE_FIXEDMODE)
{
}

GLFlyCameraFixedMode::~GLFlyCameraFixedMode()
{
}

void GLFlyCameraFixedMode::Init(void)
{
}

void GLFlyCameraFixedMode::MsgProcedures ( DWORD dwType, DWORD dwMsg  )
{
}

void GLFlyCameraFixedMode::Update( float fTime, float fElapsedTime )
{
	const D3DXVECTOR3 vMoveInterValue = m_pParentCtrl->GetMoveInterValue()
		, vRotateInterValue = m_pParentCtrl->GetRotateInterValue();
	D3DXVECTOR3 vMove, vRotate;
	const float fMoveVelo = (m_pParentCtrl->GetMoveInterTime() == 0.0f) ? 1.0f : max( max( fabs(vMoveInterValue.x), fabs(vMoveInterValue.y) ), fabs(vMoveInterValue.z) )
		, fRotateVelo = (m_pParentCtrl->GetRotateInterTime() == 0.0f) ? 1.0f : max( max( fabs(vRotateInterValue.x), fabs(vRotateInterValue.y) ), fabs(vRotateInterValue.z) );

	// 捞悼 贸府;
	if ( vMoveInterValue.x != 0.0f || vMoveInterValue.y != 0.0f || vMoveInterValue.z != 0.0f )
	{
		D3DXVec3Normalize(&vMove, &vMoveInterValue);
		vMove = vMove * fMoveVelo * m_pParentCtrl->GetMoveVelo() * fElapsedTime;
		m_vReservMove = vMove;
		m_pParentCtrl->AddAfterAction(FlyCameraControl::EMFLYCAMERA_AFTER_ACTION_MOVE_FIXEDMODE);
	}

	// 雀傈 贸府;
	if ( vRotateInterValue.x != 0.0f || vRotateInterValue.y != 0.0f || vRotateInterValue.z != 0.0f )
	{
		D3DXVec3Normalize(&vRotate, &vRotateInterValue);
		vRotate = vRotate * fRotateVelo * D3DXToRadian(m_pParentCtrl->GetRotateVelo()) * fElapsedTime;
		m_vReservRotate = vRotate; //DxViewPort::GetInstance().CameraRotation(vRotate, true);
		m_pParentCtrl->AddAfterAction(FlyCameraControl::EMFLYCAMERA_AFTER_ACTION_ROTATE_FIXEDMODE);
	}
}