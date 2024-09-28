
#include "pch.h"

#include "../../enginelib/DxTools/DxInputDevice.h"
#include "../../enginelib/DxTools/DxViewPort.h"
#include "../Char/GLCharacter.h"
#include "../GLGaeaClient.h"
#include "./GLInputManager_KeySetting.h"
#include "./GLControlTypeFlyCamera.h"

#include "../../RanLogic/FlyCamera/FlyCameraDefine.h"

void GLControlTypeFlyCamera::DoProc_CameraMove(const SKEYCAPS& cKeyCaps, const UINT nMoveDir)
{
	GASSERT(m_pGaeaClient);

	if ( (cKeyCaps.dwKeyState & DXKEY_DOWN) || (cKeyCaps.dwKeyState & DXKEY_UP) )
	{
		// SendMsg
		m_pGaeaClient->FlyCamera_AddMsg(FlyCameraControl::EMFLYCAMERA_MOVE, cKeyCaps.emShortcuts);
	}
}

void GLControlTypeFlyCamera::DoProc_CameraRotate(const SKEYCAPS& cKeyCaps, const UINT nRotateDir)
{
	GASSERT(m_pGaeaClient);

	if ( (cKeyCaps.dwKeyState & DXKEY_DOWN) || (cKeyCaps.dwKeyState & DXKEY_UP) )
	{
		// SendMsg
		m_pGaeaClient->FlyCamera_AddMsg(FlyCameraControl::EMFLYCAMERA_MOVE, cKeyCaps.emShortcuts);
	}
}

void GLControlTypeFlyCamera::DoProc_CameraMoveSpeed_Variation(const SKEYCAPS& cKeyCaps, const UINT nUpNDown)
{
	GASSERT(m_pGaeaClient);

	if ( (cKeyCaps.dwKeyState & DXKEY_DOWN) || (cKeyCaps.dwKeyState & DXKEY_UP) )
	{
		// SendMsg
		m_pGaeaClient->FlyCamera_AddMsg(FlyCameraControl::EMFLYCAMERA_MOVE, cKeyCaps.emShortcuts);
	}
}

void GLControlTypeFlyCamera::DoProc_CameraRotateSpeed_Variation(const SKEYCAPS& cKeyCaps, const UINT nUpNDown)
{
	GASSERT(m_pGaeaClient);

	if ( (cKeyCaps.dwKeyState & DXKEY_DOWN) || (cKeyCaps.dwKeyState & DXKEY_UP) )
	{
		// SendMsg
		m_pGaeaClient->FlyCamera_AddMsg(FlyCameraControl::EMFLYCAMERA_MOVE, cKeyCaps.emShortcuts);
	}
}

void GLControlTypeFlyCamera::DoProc_SelectTarget(const SKEYCAPS& cKeyCaps)
{
	GASSERT(m_pGaeaClient);

	if ( (cKeyCaps.dwKeyState & DXKEY_DOWN) == 0 )
		return ;

	D3DXVECTOR3 vTargetPt, vFromPt;
	vFromPt = DxViewPort::GetInstance().GetFromPt();
	if ( !DxViewPort::GetInstance().GetMouseTargetPosWnd(vTargetPt) )
	{
		return;
	}

	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();

	SACTIONTAR sCursorTar = SACTIONTAR();
	sCursorTar = pCharacter->FindActionTarget ( vTargetPt, vFromPt, 0, ENFIND_TAR_NULL );

	if ( sCursorTar.emACTAR == EMACTAR_NULL )
	{
		// 에뮬일 경우 'ENFIND_TAR_NULL'것만으로는 자신을 검색하지 못하는 경우가 있다;
		sCursorTar = pCharacter->FindActionTarget ( vTargetPt, vFromPt, 0, EMFIND_TAR_OUR );

		m_pGaeaClient->FlyCamera_AddMsg(FlyCameraControl::EMFLYCAMERA_SET_FOLLOWTARGET, sCursorTar.emACTAR != EMACTAR_NULL?sCursorTar.sTARID.GaeaId:-1);		
	}
	else
	{
		m_pGaeaClient->FlyCamera_AddMsg(FlyCameraControl::EMFLYCAMERA_SET_FOLLOWTARGET, sCursorTar.sTARID.GaeaId);
	}
}

void GLControlTypeFlyCamera::DoProc_CameraInterpMoveTVariation(const SKEYCAPS& cKeyCaps, const UINT nUpNDown)
{
	GASSERT(m_pGaeaClient);

	if ( (cKeyCaps.dwKeyState & DXKEY_DOWN) || (cKeyCaps.dwKeyState & DXKEY_UP) )
	{
		// SendMsg
		m_pGaeaClient->FlyCamera_AddMsg(FlyCameraControl::EMFLYCAMERA_MOVE, cKeyCaps.emShortcuts);
	}
}

void GLControlTypeFlyCamera::DoProc_CameraInterpRotateTVariation(const SKEYCAPS& cKeyCaps, const UINT nUpNDown)
{
	GASSERT(m_pGaeaClient);

	if ( (cKeyCaps.dwKeyState & DXKEY_DOWN) || (cKeyCaps.dwKeyState & DXKEY_UP) )
	{
		// SendMsg
		m_pGaeaClient->FlyCamera_AddMsg(FlyCameraControl::EMFLYCAMERA_MOVE, cKeyCaps.emShortcuts);
	}
}

void GLControlTypeFlyCamera::DoProc_CameraInterpTargetSelectTVariation(const SKEYCAPS& cKeyCaps, const UINT nUpNDown)
{
	GASSERT(m_pGaeaClient);

	if ( (cKeyCaps.dwKeyState & DXKEY_DOWN) || (cKeyCaps.dwKeyState & DXKEY_UP) )
	{
		// SendMsg
		m_pGaeaClient->FlyCamera_AddMsg(FlyCameraControl::EMFLYCAMERA_MOVE, cKeyCaps.emShortcuts);
	}
}

void GLControlTypeFlyCamera::DoProc_CameraInterpClear(const SKEYCAPS& cKeyCaps)
{
	GASSERT(m_pGaeaClient);

	if ( (cKeyCaps.dwKeyState & DXKEY_DOWN) || (cKeyCaps.dwKeyState & DXKEY_UP) )
	{
		// SendMsg
		m_pGaeaClient->FlyCamera_AddMsg(FlyCameraControl::EMFLYCAMERA_MOVE, cKeyCaps.emShortcuts);
	}
}

void GLControlTypeFlyCamera::DoProc_CameraPlay(const SKEYCAPS& cKeyCaps)
{
	GASSERT(m_pGaeaClient);

	if ( (cKeyCaps.dwKeyState & DXKEY_DOWN) )
	{
		m_pGaeaClient->FlyCamera_AddMsg(FlyCameraControl::EMFLYCAMERA_PLAY_ANIMATION, 0 );
	}
}

void GLControlTypeFlyCamera::DoProc_CameraStop(const SKEYCAPS& cKeyCaps)
{
	GASSERT(m_pGaeaClient);

 	if ( (cKeyCaps.dwKeyState & DXKEY_DOWN) )
 	{
 		m_pGaeaClient->FlyCamera_AddMsg(FlyCameraControl::EMFLYCAMERA_STOP_ANIMATION, 0);
 	}
}

void GLControlTypeFlyCamera::DoProc_CameraRewind(const SKEYCAPS& cKeyCaps)
{
	GASSERT(m_pGaeaClient);

	if ( (cKeyCaps.dwKeyState & DXKEY_DOWN) )
	{
		m_pGaeaClient->FlyCamera_AddMsg(FlyCameraControl::EMFLYCAMERA_REWIND_ANIMATION, 0);
	}
}
