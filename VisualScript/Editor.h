/**
 * \date	2011/01/25
 * \author	Jun-Hyuk Choi
 */
#pragma once

#include "../VisualScriptLib/Logic/Sequence.h"

#include "../EngineLib/DxBase/D3DObject.h"
#include <string>

class EngineDeviceMan;

class CEditor : public CD3DObject
{
public:
    vs::CSequence* m_pSequenceRoot;
    vs::CSequence* m_pSequence;
    vs::CSequenceObject* m_pFocusObject;
    INT m_nFocusConnType;
    INT m_nFocusConnIndex;
    vs::SequenceObjectVec m_vecSequenceSelected;
    TSTRING m_strSequenceCopy;
    ic::Int2 m_sSequenceCopyOrigin;
    INT m_nSequencePastCount;

public:
    CEditor();
    virtual ~CEditor();

protected:
    EngineDeviceMan* m_pEngineDevice;

public:
    VOID NewSequenceRoot();
    VOID SaveSequence( const TSTRING& strPath );
    BOOL LoadSequence( const TSTRING& strPath );

    BOOL CopyObjects( ic::Int2& sOrigin );
    BOOL PasteObjects( ic::Int2& sOrigin, ic::Int2* pAtPos = NULL );

    vs::CSequenceObject* NewSequenceObject(
        size_t nIndex,
        const ic::Int2& sPos,
        const TSTRING& strName = _T( "" ) );

    BOOL SetFocusInfo( vs::CSequenceObject* pObject, INT nConnType, INT nConnIndex );
    VOID BreakAllLinksOfFocusObject();
    VOID BreakLinksOfFocusConnector( INT nLinkIndex = INT_MIN );

    VOID ActionObject( vs::CSequenceObject* pObject, INT nConnType, INT nConnIndex );
    BOOL ActionNamedVar( ic::CBaseObject* pObject );

    VOID EmptySelectedSequence();
    VOID DeleteSelectedSequence();
    VOID InsertSelectedSequence( vs::CSequenceObject* pObject );
    VOID InsertSelectedSequence( ic::SHitProxy* pHitProxy );
    VOID EraseSelectedSequence( vs::CSequenceObject* pObject );
    VOID MoveSelectedSequence( ic::Int2& sDist );
    BOOL IsSelectedSequence( vs::CSequenceObject* pObject );

    VOID MakeConnectionToConnector(
        vs::CSequenceOp* pSrcObject,
        INT nSrcConnType,
        INT nSrcConnIndex,
        ic::CBaseObject* pDestObject,
        INT nDestConnType,
        INT nDestConnIndex );
    VOID MakeLogicConnection(
        vs::CSequenceOp* pSrcObject,
        INT nSrcConnIndex,
        vs::CSequenceOp* pDestObject,
        INT nDestConnIndex );

    VOID MakeConnectionToObject(
        vs::CSequenceOp* pSrcObject,
        INT nSrcConnType,
        INT nSrcConnIndex,
        ic::CBaseObject* pDestObject );

    VOID CalcBoundingBox( const vs::CSequenceObject* pObject, ic::Int4& sBox );
    VOID CalcBoundingBox( const vs::SequenceObjectMap& mapObjects, ic::Int4& sBox );
    VOID CalcBoundingBox( const vs::SequenceObjectVec& vecObjects, ic::Int4& sBox );

    VOID UpdateSequence( FLOAT fElapsedTime );
    VOID DrawSequence( FLOAT fElapsedTime );

public:
    VOID RenderTarget( FLOAT fElapsedTime );

public:
    virtual HRESULT OnDeviceCreated(
        IDirect3DDevice9* pd3dDevice,
        const D3DSURFACE_DESC* pBackBufferSurfaceDesc );

    virtual VOID OnDeviceDestroyed();

    virtual HRESULT OnDeviceReset(
        IDirect3DDevice9* pd3dDevice,
        const D3DSURFACE_DESC* pBackBufferSurfaceDesc );

    virtual VOID OnDeviceLost();

    virtual VOID OnFrameMove(
        IDirect3DDevice9* pd3dDevice,
        DOUBLE fTime,
        FLOAT fElapsedTime );

    virtual VOID OnFrameRender(
        IDirect3DDevice9* pd3dDevice,
        DOUBLE fTime,
        FLOAT fElapsedTime );
};

extern CEditor* g_pEditor;