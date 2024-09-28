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

	// TODO : �ӽ� ���� �ڵ� ���� �����ؾ���
	bool m_bCompostioning;
	bool m_bCopostionCheck;

public :
	virtual HRESULT OnInit( LPDIRECT3DDEVICE9 pDevice );
	virtual HRESULT OnDestroyDevice();
	virtual HRESULT OnFrameMove( float fElapsedTime );

public :
	virtual HRESULT LoadMovie( LPDIRECT3DDEVICE9 pDevice );

public :
	// �̸� ó���� ������ �޽��� ó��
	void PreProcessEvent( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	// ������ �޽��� ó��, true ��ȯ�� ������ �⺻ ���Ͻ��� ��ŵ
	bool ProcessEvent( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHit, bool& bIsDragging );

private :
	// ���콺 �̺�Ʈ ó��
	bool ProcessMouseEvent( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHit, bool& bIsDragging );
	// Ű �̺�Ʈ ó��
	bool ProcessKeyEvent( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	// IME �̺�Ʈ ó��
	bool ProcessIMEEvent( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	// Ű �Է� �÷��� ������Ʈ
	KeyModifiers UpdateModifiers( bool extendedKeyFlag = false );
	// ���� Ű �ڵ� -> gfx Ű �ڵ�� ����
	void ProcessKey( unsigned vkCode, bool downFlag, bool extendedKeyFlag );
	// AS�� Ű �Է� �̺�Ʈ ������.
	void OnKey( unsigned controllerIndex, KeyCode keyCode, unsigned wcharCode, bool downFlag, KeyModifiers mods );

public :
	bool GetCapslockKeyState();
	void SetCapslockKeyState(bool bDown);

public :
	// ���콺 ��ġ ���
	void GetMousePos( float& fMouseX, float& fMouseY ) { fMouseX = m_fMouseX; fMouseY = m_fMouseY; }

	CLIKDataBindingAdapter* GetDataBinder() { return m_pDataBinder; }
};
