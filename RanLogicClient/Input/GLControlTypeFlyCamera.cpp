
#include "pch.h"

#include "../../EngineLib/DeviceMan.h"
#include "../../EngineLib/DxTools/DxInputDevice.h"
#include "../../EngineLib/DxTools/DxViewPort.h"

#include "./GLControlTypeFlyCamera.h"


GLControlTypeFlyCamera::GLControlTypeFlyCamera( GLGaeaClient* pGaeaClient )
: GLControlBase(pGaeaClient)
{
}

GLControlTypeFlyCamera::~GLControlTypeFlyCamera()
{
}

void GLControlTypeFlyCamera::DoInit()
{
	m_mInputCaps[EMSHORTCUTS_CAMERA_MOVE_FRONT]	= boost::bind(&GLControlBase::Proc_CameraMove, _1, _2, MOVEROTATESHORTCUTS_FRONT);
	m_mInputCaps[EMSHORTCUTS_CAMERA_MOVE_BACK]	= boost::bind(&GLControlBase::Proc_CameraMove, _1, _2, MOVEROTATESHORTCUTS_BACK);
	m_mInputCaps[EMSHORTCUTS_CAMERA_MOVE_LEFT]	= boost::bind(&GLControlBase::Proc_CameraMove, _1, _2, MOVEROTATESHORTCUTS_LEFT);
	m_mInputCaps[EMSHORTCUTS_CAMERA_MOVE_RIGHT]	= boost::bind(&GLControlBase::Proc_CameraMove, _1, _2, MOVEROTATESHORTCUTS_RIGHT);
	m_mInputCaps[EMSHORTCUTS_CAMERA_MOVE_UP]	= boost::bind(&GLControlBase::Proc_CameraMove, _1, _2, MOVEROTATESHORTCUTS_UP);
	m_mInputCaps[EMSHORTCUTS_CAMERA_MOVE_DOWN]	= boost::bind(&GLControlBase::Proc_CameraMove, _1, _2, MOVEROTATESHORTCUTS_DOWN);

	m_mInputCaps[EMSHORTCUTS_CAMERA_UP]		= boost::bind(&GLControlBase::Proc_CameraRotate, _1, _2, MOVEROTATESHORTCUTS_UP);
	m_mInputCaps[EMSHORTCUTS_CAMERA_DOWN]	= boost::bind(&GLControlBase::Proc_CameraRotate, _1, _2, MOVEROTATESHORTCUTS_DOWN);
	m_mInputCaps[EMSHORTCUTS_CAMERA_LEFT]	= boost::bind(&GLControlBase::Proc_CameraRotate, _1, _2, MOVEROTATESHORTCUTS_LEFT);
	m_mInputCaps[EMSHORTCUTS_CAMERA_RIGHT]	= boost::bind(&GLControlBase::Proc_CameraRotate, _1, _2, MOVEROTATESHORTCUTS_RIGHT);

	m_mInputCaps[EMSHORTCUTS_FLYCAMERACONTROL] = boost::bind(&GLControlBase::Proc_FlyCameraControl, _1, _2);
	
	m_mInputCaps[EMSHORTCUTS_CAMERA_INCREASE_MOVESPEED] = boost::bind(&GLControlBase::Proc_CameraMoveSpeed_Variation, _1, _2, VARIATION_INCREASE);
	m_mInputCaps[EMSHORTCUTS_CAMERA_DECREASE_MOVESPEED] = boost::bind(&GLControlBase::Proc_CameraMoveSpeed_Variation, _1, _2, VARIATION_DECREASE);
	m_mInputCaps[EMSHORTCUTS_CAMERA_INCREASE_ROTATESPEED] = boost::bind(&GLControlBase::Proc_CameraRotateSpeed_Variation, _1, _2, VARIATION_INCREASE);
	m_mInputCaps[EMSHORTCUTS_CAMERA_DECREASE_ROTATESPEED] = boost::bind(&GLControlBase::Proc_CameraRotateSpeed_Variation, _1, _2, VARIATION_DECREASE);
	m_mInputCaps[EMSHORTCUTS_TARGET_SELECT] = boost::bind(&GLControlBase::Proc_SelectTarget, _1, _2);

	m_mInputCaps[EMSHORTCUTS_CAMERA_INCREASE_INTERPOLATION_MOVETIME] = boost::bind(&GLControlBase::Proc_CameraInterpMoveTVariation, _1, _2, VARIATION_INCREASE);
	m_mInputCaps[EMSHORTCUTS_CAMERA_DECREASE_INTERPOLATION_MOVETIME] = boost::bind(&GLControlBase::Proc_CameraInterpMoveTVariation, _1, _2, VARIATION_DECREASE);
	m_mInputCaps[EMSHORTCUTS_CAMERA_CLEAR_INTERPOLATION_TIME] = boost::bind(&GLControlBase::Proc_CameraInterpClear, _1, _2);
	m_mInputCaps[EMSHORTCUTS_CAMERA_INCREASE_INTERPOLATION_ROTATETIME] = boost::bind(&GLControlBase::Proc_CameraInterpRotateTVariation, _1, _2, VARIATION_INCREASE);
	m_mInputCaps[EMSHORTCUTS_CAMERA_DECREASE_INTERPOLATION_ROTATETIME] = boost::bind(&GLControlBase::Proc_CameraInterpRotateTVariation, _1, _2, VARIATION_DECREASE);
	m_mInputCaps[EMSHORTCUTS_CAMERA_INCREASE_INTERPOLATION_TARGETSELECTTIME] = boost::bind(&GLControlBase::Proc_CameraInterpTargetSelectTVariation, _1, _2, VARIATION_INCREASE);
	m_mInputCaps[EMSHORTCUTS_CAMERA_DECREASE_INTERPOLATION_TARGETSELECTTIME] = boost::bind(&GLControlBase::Proc_CameraInterpTargetSelectTVariation, _1, _2, VARIATION_DECREASE);

	m_mInputCaps[EMSHORTCUTS_CAMERA_PLAY] = boost::bind(&GLControlBase::Proc_CameraPlay, _1, _2);
	m_mInputCaps[EMSHORTCUTS_CAMERA_STOP] = boost::bind(&GLControlBase::Proc_CameraStop, _1, _2);
	m_mInputCaps[EMSHORTCUTS_CAMERA_REWIND_PLAY] = boost::bind(&GLControlBase::Proc_CameraRewind, _1, _2);
}	

void GLControlTypeFlyCamera::DoInput(float fTime, float fElapsedTime)
{
	
}

void GLControlTypeFlyCamera::DoInputCamera( float fTime, float fElapsedTime )
{
	GASSERT(m_pKeySetting);
}

void GLControlTypeFlyCamera::DoUpdate( float fTime, float fElapsedTime )
{
	/*
	D3DXVECTOR3 vMove(0.f, 0.f, 0.f), vRotate(0.f, 0.f, 0.f);

	// 이동 회전 변화량 처리;
	{
		// 따로 데이터로 빼지않고 초당 5식 증가하도록 한다;
 		m_fMoveAmount += fElapsedTime * 5 * m_fMoveOffset;
 		m_fRotateAmount += fElapsedTime * 5 * m_fRotateOffset;
 
 		m_fMoveInterT += fElapsedTime * 5 * m_fMoveInterTOffset;
 		m_fRotateInterT += fElapsedTime * 5 * m_fRotateInterTOffset;
 
 		// 0 미만이 되지않도록한다;	
 		if ( m_fRotateAmount < 0.0f )			m_fRotateAmount = 0.0f;
 		if ( m_fMoveAmount < 0.0f )				m_fMoveAmount = 0.0f;
 		if ( m_fMoveInterT < 0.0f )				m_fMoveInterT = 0.0f;
 		if ( m_fRotateInterT < 0.0f )			m_fRotateInterT = 0.0f;
	}

	{
		/*
		D3DXVec3Normalize(&vMove, &m_vMoveNormal);
		// 키 입력에 따른 이동 값 증가하는 구간;
		D3DXVECTOR3 vMoveInterValue = m_vMoveInterValue;
		
		// 단순 이동;
		if ( m_fMoveInterT == 0 )
		{
			vMoveInterValue = vMove;
		}
		// 증감 이동;
		else
		{
			
			const float fMoveVelo = fElapsedTime/m_fMoveInterT;
			vMoveInterValue += vMove * fMoveVelo;
			// 키가 때졌을 때 0으로 돌아가는 구간;
			if ( m_vMoveNormal.x == 0.0f && vMoveInterValue.x != 0.0f )
			{
				vMoveInterValue.x += ((vMoveInterValue.x > 0.0f) ? -1.0f:1.0f) * fMoveVelo;
				if ( fabs(vMoveInterValue.x) < 0.01f )	// 0.016, 60fps
					vMoveInterValue.x = 0.0f;
			}
			if ( m_vMoveNormal.y == 0.0f && vMoveInterValue.y != 0.0f )
			{
				vMoveInterValue.y += ((vMoveInterValue.y > 0.0f) ? -1.0f:1.0f) * fMoveVelo;
				if ( fabs(vMoveInterValue.y) < 0.01f )
					vMoveInterValue.y = 0.0f;
			}
			if ( m_vMoveNormal.z == 0.0f && vMoveInterValue.z != 0.0f )
			{
				vMoveInterValue.z += ((vMoveInterValue.z > 0.0f) ? -1.0f:1.0f) * fMoveVelo;
				if ( fabs(vMoveInterValue.z) < 0.01f )
					vMoveInterValue.z = 0.0f;
			}
		}

		// min max
// 		max(vMoveInterValue.x,-1.0f);
// 		min(vMoveInterValue.x, 1.0f);
// 		max(vMoveInterValue.y,-1.0f);
// 		min(vMoveInterValue.y, 1.0f);
// 		max(vMoveInterValue.z,-1.0f);
// 		min(vMoveInterValue.z, 1.0f);
		if ( vMoveInterValue.x < -1.0f )			vMoveInterValue.x = -1.0f;
		else if ( vMoveInterValue.x > 1.0f  )		vMoveInterValue.x = 1.0f ;

		if ( vMoveInterValue.y < -1.0f )			vMoveInterValue.y = -1.0f;
		else if ( vMoveInterValue.y > 1.0f  )		vMoveInterValue.y = 1.0f ;

		if ( vMoveInterValue.z < -1.0f  )			vMoveInterValue.z = -1.0f;
		else if ( vMoveInterValue.z > 1.0f  )		vMoveInterValue.z = 1.0f ;

		const float fVelo = max( max( fabs(vMoveInterValue.x), fabs(vMoveInterValue.y) ), fabs(vMoveInterValue.z) );
		D3DXVec3Normalize(&vMove, &vMoveInterValue);
		vMove = vMove * fVelo * m_fMoveAmount * fElapsedTime;
		DxViewPort::GetInstance().CameraMove ( vMove );

		m_vMoveInterValue = vMoveInterValue;
		//vMove = fElapsedTime*m_fMoveAmount*vMove;
		//DxViewPort::GetInstance().CameraMove ( vMove );

		// 이동 처리;
 		this->InterpolateValueOperation(fElapsedTime, m_fMoveInterT, m_vMoveNormal, m_vMoveInterValue);
 		const float fMoveVelo = (m_fMoveInterT == 0.0f) ? 1.0f : max( max( fabs(m_vMoveInterValue.x), fabs(m_vMoveInterValue.y) ), fabs(m_vMoveInterValue.z) );
 		D3DXVec3Normalize(&vMove, &m_vMoveInterValue);
 		vMove = vMove * fMoveVelo * m_fMoveAmount * fElapsedTime;
 		DxViewPort::GetInstance().CameraMove ( vMove );
	
		// 회전 처리;
 		this->InterpolateValueOperation(fElapsedTime, m_fRotateInterT, m_vRotateNormal, m_vRotateInterValue);
 		const float fRotateVelo = (m_fRotateInterT == 0.0f) ? 1.0f : max( max( fabs(m_vRotateInterValue.x), fabs(m_vRotateInterValue.y) ), fabs(m_vRotateInterValue.z) );
 		D3DXVec3Normalize(&vRotate, &m_vRotateInterValue);
 		vRotate = vRotate * fRotateVelo * D3DXToRadian(m_fRotateAmount) * fElapsedTime;
 		DxViewPort::GetInstance().CameraRotation ( vRotate, !m_pGaeaClient->IsTargetFixedMode() );
	}	
	*/
}

