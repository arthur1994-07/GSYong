/**
 * \date	2011/01/25
 * \author	Jun-Hyuk Choi
 */
#pragma once

#include "./ClientView.h"

class CTargetView : public CClientView
{
protected:
    DECLARE_MESSAGE_MAP()

public:
    CTargetView();
    virtual ~CTargetView();

protected:
    virtual BOOL PreCreateWindow( CREATESTRUCT& cs );

protected:
    afx_msg VOID OnButtonLoad();

public:
    virtual VOID CalcHitRect() {}

public:
    VOID ProcessInput( FLOAT fElapsedTime );

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