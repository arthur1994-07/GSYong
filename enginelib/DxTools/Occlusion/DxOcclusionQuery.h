/**
 * \date    2009/11/23
 * \author	Jun-Hyuk Choi
 * \author	Sung-Hwan Han
 *
 * Point 개념으로 OcclusionCulling 을 할 수 있다.
 * 기존보다 느려서 실시간으로 쓰기는 문제가 있다. 
 * Zone 방식으로 하기 위한 기초 namespace
 */
#pragma once

namespace DxOcclusionQuery
{
	extern BOOL g_bForceDisable;
	extern DWORD g_dwCullingDelay;

	BOOL IsEnable();

	BOOL BeginScene();
	VOID ClearSurface();
	VOID CaptureState();
	VOID RollbackState();
	VOID EndScene();

	VOID DrawSurfaceForDebug();

	VOID InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	VOID RestoreDeviceObjects();
	VOID InvalidateDeviceObjects();
	VOID DeleteDeviceObjects();
}