/**
 * \date    2009/11/23
 * \author	Jun-Hyuk Choi
 * \author	Sung-Hwan Han
 *
 * Point �������� OcclusionCulling �� �� �� �ִ�.
 * �������� ������ �ǽð����� ����� ������ �ִ�. 
 * Zone ������� �ϱ� ���� ���� namespace
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