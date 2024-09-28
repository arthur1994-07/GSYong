
#include "pch.h"

#include "../../RanLogic/Msg/GLContrlServerMsg.h"
#include "../../EngineLib/G-Logic/GLDefine.h"
#include "../../EngineLib/DxTools/DxViewPort.h"
#include "../../Enginelib/DxTools/DxInputDevice.h"

#include "../RanLogicClient/ClientActor.h"
#include "../Input/GLInputManager.h"
#include "../Input/GLInputManager_KeySetting.h"
#include "../Input/GLControlTypeFlyCamera.h"
#include "../GLGaeaClient.h"

#include "../../RanLogic/FlyCamera/FlyCameraDefine.h"
#include "./GLFlyCameraControl.h"
#include "./GLFlyCameraBaseMode.h"
#include "./GLFlyCameraFixedMode.h"
#include "./GLFlyCameraFollowMode.h"
#include "./GLFlyCameraKeyFrameControl.h"

// 
// struct FINDINDEX_FLYCAMERAKEY
// {
// 	int nKey;
// 	FINDINDEX_FLYCAMERAKEY(const int Key):nKey(Key){}
// 	bool operator()(GLFlyCameraControl::KEY_VALUE& key) const		
// 	{
// 		if ( nKey == key.nKey )	return true;
// 		return false;
// 	}
// };
// 
// 
// struct SORTINDEX_FLYCAMERAKEY
// {
// public:
// 	bool operator()(const GLFlyCameraControl::KEY_VALUE& lhs, const GLFlyCameraControl::KEY_VALUE& rhs)
// 	{
// 		return lhs.nKey < rhs.nKey;
// 	}
// };

GLFlyCameraControl& GLFlyCameraControl::GetInstance(void)
{
	static GLFlyCameraControl sFlyCameraControl;
	return sFlyCameraControl;
}

GLFlyCameraControl::GLFlyCameraControl(void)
: m_fLastMsgMoveSendT(0.0f)
, m_fMoveAmount(25.0f)
, m_fMoveInterT(0.0f)
, m_fMoveInterValue(0.0f)
, m_vMoveInterValue(0.0f, 0.0f, 0.0f)
, m_fRotateAmount(25.0f)
, m_fRotateInterT(0.0f)
, m_fRotateInterValue(0.0f)
, m_vRotateInterValue(0.0f, 0.0f, 0.0f)
, m_fTargetChangeInterT(0.0f)
, m_pCameraMode(NULL)
, m_pKeyFrameControl(NULL)
{
}

GLFlyCameraControl::~GLFlyCameraControl(void)
{
	SAFE_DELETE(m_pCameraMode);
	SAFE_DELETE(m_pKeyFrameControl);
}

void GLFlyCameraControl::InitFlyCameraControl(GLGaeaClient* const pGaeaClient)
{
	GASSERT(pGaeaClient);
	m_pGaeaClient = pGaeaClient;
	m_pCameraMode = new GLFlyCameraFixedMode( m_pGaeaClient, this );	
	m_pKeyFrameControl = new GLFlyCameraKeyFrameControl();
}

void GLFlyCameraControl::MsgProcedures()
{
	FlyCameraControl::SMsg sMsg;
	
	while ( PopMessage(&sMsg) )
	{
		DWORD type(sMsg.type), dwMsg(sMsg.dwMsg);
		switch ( type )
		{
		case FlyCameraControl::EMFLYCAMERA_PRINT_MOVEINTERTIME:	
			m_pGaeaClient->PrintMsgText(D3DCOLOR_XRGB(229, 12 ,35), sc::string::format("Current Move Interpolation Time: %f", m_fMoveInterT) );
			break;
		case FlyCameraControl::EMFLYCAMERA_PRINT_ROTATEINTERTIME:
			m_pGaeaClient->PrintMsgText(D3DCOLOR_XRGB(229, 12 ,35), sc::string::format("Current Rotate Interpolation Time: %f", m_fRotateInterT) );
			break;
		case FlyCameraControl::EMFLYCAMERA_PRINT_TARGETMOVEINTERTIME:
			m_pGaeaClient->PrintMsgText(D3DCOLOR_XRGB(229, 12 ,35), sc::string::format("Current Target Move Interpolation Time: %f", m_fTargetChangeInterT) );
			break;

		case FlyCameraControl::EMFLYCAMERA_MOVE:
			{
				if ( GLInputManager::GetInstance().GetKeyState((EMSHORTCUTS)dwMsg) & DXKEY_DOWNED )
				{
					switch ( (EMSHORTCUTS)dwMsg )
					{
					case EMSHORTCUTS_CAMERA_MOVE_FRONT:
					case EMSHORTCUTS_CAMERA_MOVE_BACK:
					case EMSHORTCUTS_CAMERA_MOVE_LEFT:
					case EMSHORTCUTS_CAMERA_MOVE_RIGHT:
					case EMSHORTCUTS_CAMERA_MOVE_UP:
					case EMSHORTCUTS_CAMERA_MOVE_DOWN:
					case EMSHORTCUTS_CAMERA_UP:
					case EMSHORTCUTS_CAMERA_DOWN:
					case EMSHORTCUTS_CAMERA_LEFT:
					case EMSHORTCUTS_CAMERA_RIGHT:
					case EMSHORTCUTS_CAMERA_INCREASE_MOVESPEED: 
					case EMSHORTCUTS_CAMERA_DECREASE_MOVESPEED:	
					case EMSHORTCUTS_CAMERA_INCREASE_INTERPOLATION_MOVETIME:
					case EMSHORTCUTS_CAMERA_DECREASE_INTERPOLATION_MOVETIME:
					case EMSHORTCUTS_CAMERA_INCREASE_ROTATESPEED:
					case EMSHORTCUTS_CAMERA_DECREASE_ROTATESPEED:
					case EMSHORTCUTS_CAMERA_INCREASE_INTERPOLATION_ROTATETIME:
					case EMSHORTCUTS_CAMERA_DECREASE_INTERPOLATION_ROTATETIME:
					case EMSHORTCUTS_CAMERA_INCREASE_INTERPOLATION_TARGETSELECTTIME:
					case EMSHORTCUTS_CAMERA_DECREASE_INTERPOLATION_TARGETSELECTTIME:
						{
							if ( m_lstReservAction.end() == std::find(m_lstReservAction.begin(), m_lstReservAction.end(), dwMsg) )
								m_lstReservAction.push_back(dwMsg);
						}
						break;
					
					}
				}
				else	// DXKEY_UP
				{
					switch ( (EMSHORTCUTS)dwMsg )
					{
					case EMSHORTCUTS_CAMERA_MOVE_FRONT:
					case EMSHORTCUTS_CAMERA_MOVE_BACK:
					case EMSHORTCUTS_CAMERA_MOVE_LEFT:
					case EMSHORTCUTS_CAMERA_MOVE_RIGHT:
					case EMSHORTCUTS_CAMERA_MOVE_UP:
					case EMSHORTCUTS_CAMERA_MOVE_DOWN:
					case EMSHORTCUTS_CAMERA_UP:	
					case EMSHORTCUTS_CAMERA_DOWN:
					case EMSHORTCUTS_CAMERA_LEFT:
					case EMSHORTCUTS_CAMERA_RIGHT:
						break;

					case EMSHORTCUTS_CAMERA_INCREASE_MOVESPEED:
					case EMSHORTCUTS_CAMERA_DECREASE_MOVESPEED:
						{
							GLWidgetScript::GetInstance().LuaCallFunc< void >( 
								NSWIDGET_SCRIPT::g_strFunc_FlyCamCtrl_SetMoveVelo,
								"-f", m_fMoveAmount );
						}
						break;

					case EMSHORTCUTS_CAMERA_INCREASE_INTERPOLATION_MOVETIME:
					case EMSHORTCUTS_CAMERA_DECREASE_INTERPOLATION_MOVETIME: 
						{
							m_pGaeaClient->PrintMsgText(D3DCOLOR_XRGB(229, 12 ,35), sc::string::format("Current Move Interpolation Time: %f", m_fMoveInterT) );
						}
						break;

					case EMSHORTCUTS_CAMERA_INCREASE_ROTATESPEED:
					case EMSHORTCUTS_CAMERA_DECREASE_ROTATESPEED:
						{
							GLWidgetScript::GetInstance().LuaCallFunc< void >( 
								NSWIDGET_SCRIPT::g_strFunc_FlyCamCtrl_SetRotateVelo,
								"-f", m_fRotateAmount );
						}
						break;

					case EMSHORTCUTS_CAMERA_INCREASE_INTERPOLATION_ROTATETIME:
					case EMSHORTCUTS_CAMERA_DECREASE_INTERPOLATION_ROTATETIME:	
						{
							m_pGaeaClient->PrintMsgText(D3DCOLOR_XRGB(229, 12 ,35), sc::string::format("Current Rotate Interpolation Time: %f", m_fRotateInterT) );
						}
						break;

					case EMSHORTCUTS_CAMERA_INCREASE_INTERPOLATION_TARGETSELECTTIME:
					case EMSHORTCUTS_CAMERA_DECREASE_INTERPOLATION_TARGETSELECTTIME:
						{
							m_pGaeaClient->PrintMsgText(D3DCOLOR_XRGB(229, 12 ,35), sc::string::format("Current Target Chase Interpolation Time: %f", m_fTargetChangeInterT) );
						}
						break;

					case EMSHORTCUTS_CAMERA_CLEAR_INTERPOLATION_TIME:
						{
							m_fMoveInterT = 0.0f;
							m_fRotateInterT = 0.0f;
							m_pGaeaClient->PrintMsgText(D3DCOLOR_XRGB(229, 12 ,35), "Move, Rotate Interpolation Time Reset!");
						}
						break;
					}
					std::list<DWORD>::iterator itr = std::find(m_lstReservAction.begin(), m_lstReservAction.end(), dwMsg);
					if ( m_lstReservAction.end() != itr )
						m_lstReservAction.erase(itr);
				}
			}
			break;

		case FlyCameraControl::EMFLYCAMERA_SET_FOLLOWTARGET:
			{
				if ( dwMsg == -1 && m_pCameraMode->GetModeType() != FlyCameraControl::MODETYPE_FIXEDMODE )
				{
					SAFE_DELETE(m_pCameraMode);
					m_pCameraMode = new GLFlyCameraFixedMode(m_pGaeaClient, this);
					m_pCameraMode->Init();
				}
				else if ( dwMsg != -1 )
				{
					if ( m_pCameraMode->GetModeType() == FlyCameraControl::MODETYPE_FIXEDMODE )
					{
						SAFE_DELETE(m_pCameraMode);
						m_pCameraMode = new GLFlyCameraFollowMode(m_pGaeaClient, this);
						m_pCameraMode->Init();
					}
				}
			}
			break;
		case FlyCameraControl::EMFLYCAMERA_ACTIVE:
			{
				if ( dwMsg )
					DxViewPort::GetInstance().SetFlyCameraFlag();
				else
					DxViewPort::GetInstance().SetGameCamera();
			}
			break;

		case FlyCameraControl::EMFLYCAMERA_SET_KEYPOSITION:
			{
				LuaTable tbPos = GLWidgetScript::GetInstance().LuaCallFunc< LuaTable >( 
					NSWIDGET_SCRIPT::g_strFunc_FlyCamCtrl_GetKeyFramePos );
				D3DXVECTOR3 vPos( tbPos.get< float >( 1 ), tbPos.get< float >( 2 ), tbPos.get< float >( 3 ) );
				sMsg.vec = D3DXVECTOR4(vPos.x, vPos.y, vPos.z, m_pKeyFrameControl->GetCurrentFrame());
			}
			break;
		case FlyCameraControl::EMFLYCAMERA_SET_KEYLOOKAT:
			{
			}
			break;
		}

		m_pCameraMode->MsgProcedures(type, dwMsg);
		// m_pKeyFrameControl->MsgProcedures(sMsg);
		m_pKeyFrameControl->AddMessage(&sMsg);
	}
}

void GLFlyCameraControl::FrameMove(const float fTime, const float fElapsedTime)
{
	this->MsgProcedures();
	this->BeginUpdate(fTime, fElapsedTime);
	m_pCameraMode->Update(fTime, fElapsedTime);
	m_pKeyFrameControl->Update(fTime, fElapsedTime);
	this->Update(fTime, fElapsedTime);
	if ( GLInputManager::GetInstance().GetCurrentControlType() == EM_CONTROLTYPE::EM_CONTROLTYPE_FLYCAMERA )
		DxViewPort::GetInstance().FrameMove_FlyCamera();
}

void GLFlyCameraControl::BeginUpdate(const float fTime, const float fElapsedTime)
{
	const BOOL bFlyCameraUse = GLInputManager::GetInstance().GetCurrentControlType() == EM_CONTROLTYPE::EM_CONTROLTYPE_FLYCAMERA;
	const float fElapsMove = 0.3f;
	// 초당 3번 카메라 위치 갱신;
	if ( bFlyCameraUse && ((m_fLastMsgMoveSendT+fElapsMove) < fTime) )
	{
		m_fLastMsgMoveSendT = fTime;
		GLMSG::SNET_GM_FLYCAMERACONTROL_MOVE_CF Msg;
		Msg.vPosition = DxViewPort::GetInstance().GetFromPt();
		m_pGaeaClient->NETSENDTOFIELD(&Msg);
	}
	float fMoveOffset, fMoveInterTOffset, fRotateOffset, fRotateInterTOffset, fTargetChangeInterOffset;
	fMoveOffset = fMoveInterTOffset = fRotateOffset = fRotateInterTOffset = fTargetChangeInterOffset = 0.0f;

	D3DXVECTOR3 vMoveNormal(0.0f, 0.0f, 0.0f), vRotateNormal(0.0f, 0.0f, 0.0f);
	for ( std::list<DWORD>::iterator itr = m_lstReservAction.begin(); 
		itr != m_lstReservAction.end(); ++itr )
	{
		switch ( (*itr) )
		{
		case EMSHORTCUTS_CAMERA_MOVE_FRONT:		vMoveNormal.z+=1.0f;	break;
		case EMSHORTCUTS_CAMERA_MOVE_BACK:		vMoveNormal.z-=1.0f;	break;
		case EMSHORTCUTS_CAMERA_MOVE_LEFT:		vMoveNormal.x-=1.0f;	break;
		case EMSHORTCUTS_CAMERA_MOVE_RIGHT:		vMoveNormal.x+=1.0f;	break;
		case EMSHORTCUTS_CAMERA_MOVE_UP:		vMoveNormal.y+=1.0f;	break;
		case EMSHORTCUTS_CAMERA_MOVE_DOWN:		vMoveNormal.y-=1.0f;	break;
		case EMSHORTCUTS_CAMERA_UP:		vRotateNormal.x+=1.0f;			break;
		case EMSHORTCUTS_CAMERA_DOWN:	vRotateNormal.x-=1.0f;			break;
		case EMSHORTCUTS_CAMERA_LEFT:	vRotateNormal.y-=1.0f;			break;
		case EMSHORTCUTS_CAMERA_RIGHT:	vRotateNormal.y+=1.0f;			break;

		case EMSHORTCUTS_CAMERA_INCREASE_MOVESPEED: fMoveOffset = 1.0f;	break;
		case EMSHORTCUTS_CAMERA_DECREASE_MOVESPEED:	fMoveOffset =-1.0f;	break;
		case EMSHORTCUTS_CAMERA_INCREASE_INTERPOLATION_MOVETIME: fMoveInterTOffset = 1.0f; break;
		case EMSHORTCUTS_CAMERA_DECREASE_INTERPOLATION_MOVETIME: fMoveInterTOffset =-1.0f; break;
		case EMSHORTCUTS_CAMERA_INCREASE_ROTATESPEED:	fRotateOffset = 1.0f;	break;
		case EMSHORTCUTS_CAMERA_DECREASE_ROTATESPEED:	fRotateOffset =-1.0f;	break;
		case EMSHORTCUTS_CAMERA_INCREASE_INTERPOLATION_ROTATETIME:	fRotateInterTOffset = 1.0f; break;
		case EMSHORTCUTS_CAMERA_DECREASE_INTERPOLATION_ROTATETIME:	fRotateInterTOffset =-1.0f; break;
		case EMSHORTCUTS_CAMERA_INCREASE_INTERPOLATION_TARGETSELECTTIME:	fTargetChangeInterOffset = 1.0f;break;
		case EMSHORTCUTS_CAMERA_DECREASE_INTERPOLATION_TARGETSELECTTIME:	fTargetChangeInterOffset =-1.0f;break;
		}
	}

	// 단축키를 통한 수치값 증가, 초당 5식 증가하도록 한다;
	m_fMoveAmount += fElapsedTime * 5 * fMoveOffset;
	m_fMoveInterT += fElapsedTime * 5 * fMoveInterTOffset;

	m_fRotateAmount += fElapsedTime * 5 * fRotateOffset;
	m_fRotateInterT += fElapsedTime * 5 * fRotateInterTOffset;

	m_fTargetChangeInterT += fElapsedTime * 5 * fTargetChangeInterOffset;

	if ( m_fMoveAmount < 0.0f )				m_fMoveAmount = 0.0f;
	if ( m_fMoveInterT < 0.0f )				m_fMoveInterT = 0.0f;
	if ( m_fRotateAmount < 0.0f )			m_fRotateAmount = 0.0f;
	if ( m_fRotateInterT < 0.0f )			m_fRotateInterT = 0.0f;

	//D3DXVECTOR3 vMove, vRotate;
	// 경과 시간에 따라 보간값 수치가 결정되므로 여기서 수치값을 구해준다;
	// 이동 수치 처리;
	this->InterpolateValueOperation(fElapsedTime, m_fMoveInterT, vMoveNormal, m_vMoveInterValue);

	// 회전 수치 처리;
	this->InterpolateValueOperation(fElapsedTime, m_fRotateInterT, vRotateNormal, m_vRotateInterValue);
}

void GLFlyCameraControl::Update(const float fTime, const float fElapsedTime)
{
	D3DXVECTOR3 vNextPos(0.0f, 0.0f, 0.0f),vRotate_Head(0.0f, 0.0f, 0.0f), vRotate_From(0.0f, 0.0f, 0.0f);
	
	for ( std::list<DWORD>::iterator itr = m_lstAfterAction.begin(); 
		itr != m_lstAfterAction.end(); )
	{
		switch ( (*itr) )
		{
		case FlyCameraControl::EMFLYCAMERA_AFTER_ACTION_MOVE_FIXEDMODE:
			{
				vNextPos += m_pCameraMode->GetReserveMove();
			}
			break;
		case FlyCameraControl::EMFLYCAMERA_AFTER_ACTION_MOVE_FOLLOWMODE:
			{
 				GLFlyCameraFollowMode* pFollowMode = static_cast<GLFlyCameraFollowMode*>(m_pCameraMode);
				vRotate_From += pFollowMode->GetReserveMove();
			}
			break;
		case FlyCameraControl::EMFLYCAMERA_AFTER_ANIMATION_MOVE:
			{
				vNextPos += m_pKeyFrameControl->GetReserveMove();
			}
			break;
		case FlyCameraControl::EMFLYCAMERA_AFTER_ACTION_ROTATE_FIXEDMODE:
			{
				vRotate_Head += m_pCameraMode->GetReserveRotation();
			}
			break;
		}
		itr = m_lstAfterAction.erase(itr);
	}

	DxViewPort::GetInstance().CameraMove(vNextPos, TRUE, m_pCameraMode->GetModeType() != FlyCameraControl::MODETYPE_FOLLOWMODE);
	DxViewPort::GetInstance().CameraRotation(vRotate_From, FALSE);
	DxViewPort::GetInstance().CameraRotation(vRotate_Head, TRUE);

	// Zoom 내부에서 지형 충돌이라던가 거리제한 체크등을 계속해서 체크함으로 줌하는 당시에만;
	if ( vRotate_From.z != 0.0f )
		DxViewPort::GetInstance().CameraZoom(vRotate_From.z);
}

bool GLFlyCameraControl::IsPlayVideo(void)
{
	if ( !m_pKeyFrameControl )	return false;
	else		return m_pKeyFrameControl->isPlayVideo();
}

int GLFlyCameraControl::GetKeyFrameSize(void) const
{
	if ( !m_pKeyFrameControl )		return -1;
	else	return m_pKeyFrameControl->GetKeyFrameSize();
}

int GLFlyCameraControl::GetAtIndex(int nAt) const
{
	if ( !m_pKeyFrameControl )	return -1;
	else	return m_pKeyFrameControl->GetKeyFrame(nAt);
}

int GLFlyCameraControl::GetCurrentFrame(void) const
{
	if ( !m_pKeyFrameControl )	return -1;
	else		return m_pKeyFrameControl->GetCurrentFrame();
}

const FlyCameraControl::FlyCameraKey& GLFlyCameraControl::GetKeyData(int nKey) const
{
	return m_pKeyFrameControl->GetKeyData(nKey);
}

//  [8/27/2014 gbgim];
// - 보간 시간에 따라 증가된 비율만 적용함;
void GLFlyCameraControl::InterpolateValueOperation( IN const float fElapsedTime
													   , IN const float fInterpTime
													   , IN const D3DXVECTOR3& vInterpNormal
													   , IN OUT D3DXVECTOR3& vInterpValue)
{
	D3DXVECTOR3 vNormal = vInterpNormal;
	D3DXVECTOR3 vValue = vInterpValue;

	D3DXVECTOR3 vMove = vNormal;
	D3DXVec3Normalize(&vMove, &vMove);

	// 단순 증가;
	if ( fInterpTime == 0.0f )
	{
		vValue = vMove;
	}
	// 보간에 따른 증가;
	else
	{
		const float fVelo = fElapsedTime/fInterpTime;
		vValue += vMove * fVelo;
		// 증가할 방향이없는데 힘이 남아있다면 감쇄시킨다 x, y, z;
		if ( vInterpNormal.x == 0.0f && vValue.x != 0.0f )
		{
			vValue.x += ((vValue.x>0.0f)? -1.0f:1.0f) * fVelo;
			if ( fabs(vValue.x) < 0.016f ) // 60fps
				vValue.x = 0.0f;
		}
		if ( vInterpNormal.y == 0.0f && vValue.y != 0.0f )
		{
			vValue.y += ((vValue.y>0.0f)? -1.0f:1.0f) * fVelo;
			if ( fabs(vValue.y) < 0.016f ) // 60fps
				vValue.y = 0.0f;
		}
		if ( vInterpNormal.z == 0.0f && vValue.z != 0.0f )
		{
			vValue.z += ((vValue.z>0.0f)? -1.0f:1.0f) * fVelo;
			if ( fabs(vValue.z) < 0.016f ) // 60fps
				vValue.z = 0.0f;
		}

		// 최소 최대 검사;
		if ( vValue.x < -1.0f )			vValue.x =-1.0f;
		else if ( vValue.x > 1.0f  )	vValue.x = 1.0f;

		if ( vValue.y < -1.0f )			vValue.y =-1.0f;
		else if ( vValue.y > 1.0f  )	vValue.y = 1.0f;

		if ( vValue.z < -1.0f  )		vValue.z =-1.0f;
		else if ( vValue.z > 1.0f  )	vValue.z = 1.0f;
	}

	vInterpValue = vValue;
}

void GLFlyCameraControl::AddMessage(DWORD type, DWORD dwMsg)
{
	FlyCameraControl::SMsg msg;
	msg.type = (FlyCameraControl::EMMSG)type;
	msg.dwMsg = dwMsg;
	m_MsgQueue.push(msg);
}

bool GLFlyCameraControl::PopMessage(DWORD* type, DWORD* dwMsg)
{
	if ( !type || !dwMsg )	return false;
	if ( m_MsgQueue.empty() )	return false;
	const FlyCameraControl::SMsg& msg = m_MsgQueue.front();
	*type = msg.type;
	*dwMsg = msg.dwMsg;
	m_MsgQueue.pop();
	return true;
}

bool GLFlyCameraControl::PopMessage(FlyCameraControl::SMsg* sMsg)
{
	if ( !sMsg )	return false;
	if ( m_MsgQueue.empty() )	return false;
	*sMsg = m_MsgQueue.front();
	m_MsgQueue.pop();
	return true;
}

void GLFlyCameraControl::AddReserveAction(DWORD type)
{
	m_lstReservAction.push_back(type);
}

void GLFlyCameraControl::AddAfterAction(DWORD type)
{
	m_lstAfterAction.push_back(type);
}

void GLFlyCameraControl::SetFPS( int nFPS )
{
	AddMessage( FlyCameraControl::EMFLYCAMERA_SET_FPS, nFPS );
}

int GLFlyCameraControl::GetFPS()
{
	if( NULL == m_pKeyFrameControl )
		return -1;

	return m_pKeyFrameControl->GetFPS();
}

void GLFlyCameraControl::SetCurrentFrame( int nKey )
{
	AddMessage( FlyCameraControl::EMFLYCAMERA_SET_CURRENTKEYFRAME, nKey );
}

void GLFlyCameraControl::SetTotalFrame( int nTotal )
{
	AddMessage( FlyCameraControl::EMFLYCAMERA_SET_TOTALFRAME, nTotal );
}

int GLFlyCameraControl::GetTotalFrame()
{
	if( NULL == m_pKeyFrameControl )
		return -1;

	return m_pKeyFrameControl->GetTotalFrame();
}

void GLFlyCameraControl::AddKeyFrame( int nKey )
{
	AddMessage( FlyCameraControl::EMFLYCAMERA_ADD_KEYFRAMESLOT, nKey );
}

void GLFlyCameraControl::DeleteKeyFrame( int nKey )
{
	AddMessage( FlyCameraControl::EMFLYCAMERA_DEL_KEYFRAMESLOT, nKey );
}

void GLFlyCameraControl::Play()
{
	AddMessage( FlyCameraControl::EMFLYCAMERA_PLAY_ANIMATION, 0 );
}

void GLFlyCameraControl::Stop()
{
	AddMessage( FlyCameraControl::EMFLYCAMERA_STOP_ANIMATION, 0 );
}

void GLFlyCameraControl::Rewind()
{
	AddMessage( FlyCameraControl::EMFLYCAMERA_REWIND_ANIMATION, 0 );
}

void GLFlyCameraControl::SaveKeyFrame()
{
	AddMessage( FlyCameraControl::EMFLYCAMERA_SET_KEYPOSITION, 0 );
}

LuaTable GLFlyCameraControl::GetKeyFramePos( int nIndex )
{
	const FlyCameraControl::FlyCameraKey& sKey = GetKeyData( nIndex );

	LuaTable tbPos( GLWidgetScript::GetInstance().GetLuaState() );
	tbPos.set( "x", sKey.sMoveToTargetID.vPos.x );
	tbPos.set( "y", sKey.sMoveToTargetID.vPos.y );
	tbPos.set( "z", sKey.sMoveToTargetID.vPos.z );

	return tbPos;
}

LuaTable GLFlyCameraControl::GetMoveToTypeString()
{
	LuaTable tbString( GLWidgetScript::GetInstance().GetLuaState() );

	for( int i = 0; i < FlyCameraControl::EMFLYCAMERA_MOVETO_SIZE; ++i )
		tbString.set( i + 1, FlyCameraControl::strMOVETOTYPE[ i ].c_str() );

	return tbString;
}

void GLFlyCameraControl::ReqKeyFrameInfo()
{
	if( NULL == m_pKeyFrameControl )
		return;

	m_pKeyFrameControl->SendToWidgetKeyFrameInfo();
}