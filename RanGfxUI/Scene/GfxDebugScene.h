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
	// ������ �޽��� ó��, true ��ȯ�� ������ �⺻ ���Ͻ��� ��ŵ
	bool ProcessEvent( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHit, bool& bIsDragging );

private :
	// ���콺 �̺�Ʈ ó��
	bool ProcessMouseEvent( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHit, bool& bIsDragging );
	// Ű �̺�Ʈ ó��
	bool ProcessKeyEvent( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	// Ű �Է� �÷��� ������Ʈ
	KeyModifiers UpdateModifiers( bool extendedKeyFlag = false );
	// ���� Ű �ڵ� -> gfx Ű �ڵ�� ����
	void ProcessKey( unsigned vkCode, bool downFlag, bool extendedKeyFlag );
	// AS�� Ű �Է� �̺�Ʈ ������.
	void OnKey( unsigned controllerIndex, KeyCode keyCode, unsigned wcharCode, bool downFlag, KeyModifiers mods );
};
