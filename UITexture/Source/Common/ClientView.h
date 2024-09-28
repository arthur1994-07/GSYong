/**
 * \date	2011/01/25
 * \author	Jun-Hyuk Choi
 */
#pragma once

#include "../../../EngineLib/DxBase/D3DViewport.h"

class CFrame;

class CClientView : public CWnd, public CD3DViewport
{
protected:
    CFrame* m_pFrame;
    INT m_nToolBarID;
    CXTPToolBar m_wndToolBar;

public:
    CClientView();
    virtual ~CClientView();

protected:
    virtual LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam );

public:
    virtual VOID CalcSourceRect();
    virtual VOID CalcDestRect();

public:
    virtual ic::Int2 GetClientPos( const ic::Int2& sScreen );
    virtual ic::Int2 GetClientMousePos();
    virtual BOOL InViewClientPos( const ic::Int2& sClient );
    virtual BOOL InViewClientMousePos();

public:
    virtual HWND GetTargetWnd() { return CWnd::GetSafeHwnd(); }
    virtual CFrame* GetFrame() { return m_pFrame; }
};