/**
 * \date	2011/01/25
 * \author	Jun-Hyuk Choi
 */
#pragma once

#include "../InternalCommonLib/Core/BaseObject.h"

#include "./ClientView.h"

namespace vs
{
    class CSequenceOp;
    class CSequenceFrame;
    class CSequence;
}

class CSequenceView : public CClientView
{
protected:
    DECLARE_MESSAGE_MAP()

protected:
    ic::Int2 m_sMouseDownPos;

    BOOL m_bMakingLine;
    vs::CSequenceOp* m_pConnSeqOp;
    INT m_nConnType;
    INT m_nConnIndex;

    BOOL m_bSpecialDrag;
    vs::CSequenceFrame* m_pSpecialFrame;
    ic::Int2 m_sSpecialSize;
    ic::Int2 m_sSpecialBegin;

    BOOL m_bBoxSelecting;
    ic::Int4 m_sBoxRect;

    D3DXVECTOR2 m_sScrollAccum;
    D3DXVECTOR2 m_sScrollBorder;

public:
    CSequenceView();
    virtual ~CSequenceView();

protected:
    virtual BOOL PreCreateWindow( CREATESTRUCT& cs );

protected:
    afx_msg VOID OnButtonNew();
    afx_msg VOID OnButtonLoad();
    afx_msg VOID OnButtonSave();
    afx_msg VOID OnButtonZoom();
    afx_msg VOID OnButtonTest();

public:
    VOID ChangeActiveSequnce( vs::CSequence* pSequence );

    VOID OnViewChanged();

    VOID CaptureMousePos();
    BOOL DiffCaptureMousePosVsCurrent();

    VOID ProcessInput( FLOAT fElapsedTime );
    VOID ApplyDragToOrigin();
    VOID ApplyWheelToZoom();
    VOID ProcessCutCopyPaste();

    VOID OpenMenu();
    VOID OpenSequenceBackground();
    VOID OpenSequenceObject();
    VOID OpenSequenceConnector();

    VOID ActionObject();

    VOID SelectObject();
    VOID SelectConnector();
    VOID SelectSpecial();
    VOID MoveSelected();
    VOID DeleteSelected();

    VOID SetSelectedConnector( ic::CBaseObject* pObject, INT nConnType, INT nConnIndex );
    ic::Int2 GetSelectedConnLocation();
    DWORD GetMakingLinkColor();
    VOID EndMakingLine();
    VOID DrawMakingLine();

    VOID BeginSpecialDrag( vs::CSequenceFrame* pSeqFrame );
    VOID ContinueSpecialDrag();
    VOID EndSpecialDrag();

    VOID BeginBoxSelecting();
    VOID ContinueBoxSelecting();
    VOID EndBoxSelecting();
    VOID DrawBoxSelecting();

    BOOL ShouldHighlightConnector( ic::CBaseObject* pObject, INT nConnType, INT nConnIndex );
    VOID UpdateMouseOverObject();

    VOID DoScrollBorder( FLOAT fElapsedTime );

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