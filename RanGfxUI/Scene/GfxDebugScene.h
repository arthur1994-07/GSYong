#pragma once

#include "GfxScene.h"

class GfxDebugScene : public GfxScene
{
public :
	GfxDebugScene();
	virtual ~GfxDebugScene();

protected :
	KeyModifiers m_KeyMod;

public :
	virtual HRESULT OnInit( LPDIRECT3DDEVICE9 pDevice );

public :
	// 윈도우 메시지 처리, true 반환시 윈도우 기본 프록시저 스킵
	bool ProcessEvent( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHit, bool& bIsDragging );

private :
	// 마우스 이벤트 처리
	bool ProcessMouseEvent( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHit, bool& bIsDragging );
	// 키 이벤트 처리
	bool ProcessKeyEvent( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	// 키 입력 플래그 업데이트
	KeyModifiers UpdateModifiers( bool extendedKeyFlag = false );
	// 가상 키 코드 -> gfx 키 코드로 매핑
	void ProcessKey( unsigned vkCode, bool downFlag, bool extendedKeyFlag );
	// AS로 키 입력 이벤트 날려줌.
	void OnKey( unsigned controllerIndex, KeyCode keyCode, unsigned wcharCode, bool downFlag, KeyModifiers mods );
};
