#pragma once

#include "./GfxScene.h"

class GfxGameScene : public GfxScene
{
public:
	GfxGameScene(void);
	virtual ~GfxGameScene(void);

protected :
	Ptr< CLIKDataBindingAdapter > m_pDataBinder;

	KeyModifiers		m_KeyMod;
	float				m_fMouseX;
	float				m_fMouseY;

	// TODO : 임시 방편 코드 추후 수정해야함
	bool m_bCompostioning;
	bool m_bCopostionCheck;

public :
	virtual HRESULT OnInit( LPDIRECT3DDEVICE9 pDevice );
	virtual HRESULT OnDestroyDevice();
	virtual HRESULT OnFrameMove( float fElapsedTime );

public :
	virtual HRESULT LoadMovie( LPDIRECT3DDEVICE9 pDevice );

public :
	// 미리 처리할 윈도우 메시지 처리
	void PreProcessEvent( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	// 윈도우 메시지 처리, true 반환시 윈도우 기본 프록시저 스킵
	bool ProcessEvent( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHit, bool& bIsDragging );

private :
	// 마우스 이벤트 처리
	bool ProcessMouseEvent( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHit, bool& bIsDragging );
	// 키 이벤트 처리
	bool ProcessKeyEvent( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	// IME 이벤트 처리
	bool ProcessIMEEvent( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	// 키 입력 플래그 업데이트
	KeyModifiers UpdateModifiers( bool extendedKeyFlag = false );
	// 가상 키 코드 -> gfx 키 코드로 매핑
	void ProcessKey( unsigned vkCode, bool downFlag, bool extendedKeyFlag );
	// AS로 키 입력 이벤트 날려줌.
	void OnKey( unsigned controllerIndex, KeyCode keyCode, unsigned wcharCode, bool downFlag, KeyModifiers mods );

public :
	bool GetCapslockKeyState();
	void SetCapslockKeyState(bool bDown);

public :
	// 마우스 위치 얻기
	void GetMousePos( float& fMouseX, float& fMouseY ) { fMouseX = m_fMouseX; fMouseY = m_fMouseY; }

	CLIKDataBindingAdapter* GetDataBinder() { return m_pDataBinder; }
};
