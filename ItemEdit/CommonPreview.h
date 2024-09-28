#pragma once

#include "../EngineLib/DxBase/D3DViewport.h"

class CCommonPreview : public CWnd, public CD3DViewport
{
public:
	static CCommonPreview& GetInstance();

public:
	bool Create( CWnd* pParent );
	void Destroy();

protected:
	virtual BOOL PreCreateWindow( CREATESTRUCT& cs );
    virtual LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam );

public:
    virtual VOID OnFrameMove(
        IDirect3DDevice9* pd3dDevice,
        DOUBLE fTime,
        FLOAT fElapsedTime );

    virtual VOID OnFrameRender(
        IDirect3DDevice9* pd3dDevice,
        DOUBLE fTime,
        FLOAT fElapsedTime );

	virtual HWND GetTargetWnd() { return CWnd::GetSafeHwnd(); }
};