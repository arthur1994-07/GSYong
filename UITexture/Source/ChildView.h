#pragma once

#include "./Common/ClientView.h"

class CChildView : public CClientView
{
protected:
	DECLARE_MESSAGE_MAP()

private:
    VOID FindControl();
    
	ic::Int2 m_sMouseDownPos;
    BOOL m_bAlphaChannelUse;

public:
	CChildView();
	virtual ~CChildView();

protected:
	virtual BOOL PreCreateWindow( CREATESTRUCT& cs );

protected:
	afx_msg VOID OnButtonNew();
    afx_msg VOID OnButtonLoad();
    afx_msg VOID OnButtonSave();
    afx_msg VOID OnButtonPsdLoad();

    afx_msg VOID OnButtonUseAlpha();
    afx_msg VOID OnButtonBgColor();

    afx_msg VOID OnButtonAdd();
    afx_msg VOID OnButtonDel();

public:
	VOID OnViewChanged();

	VOID CaptureMousePos();
    BOOL DiffCaptureMousePosVsCurrent();

	VOID ProcessInput( FLOAT fElapsedTime );
	VOID ApplyDragToOrigin();
    VOID ApplyWheelToZoom();

	VOID DrawTextureControl( FLOAT fElapsedTime );\

public:
    virtual VOID OnFrameMove(
        IDirect3DDevice9* pd3dDevice,
        DOUBLE fTime,
        FLOAT fElapsedTime );

    virtual VOID OnFrameRender(
        IDirect3DDevice9* pd3dDevice,
        DOUBLE fTime,
        FLOAT fElapsedTime );
};