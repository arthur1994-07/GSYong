/**
 * \date	2011/01/27
 * \author	Jun-Hyuk Choi
 * \ref     Unreal3
 */
#pragma once

#include "./SequenceOp.h"

namespace vs
{
    class CSequence : public ic::CRttiObject< CSequence, CSequenceOp, 1, ic::EInternal >
    {
    public:
        BOOL bEnabled;
        ic::Int2 sDefaultView;
        FLOAT fDefaultViewZoom;
        SequenceObjectMap mapObjects;

    public:
        CSequence();
        virtual ~CSequence();

        VOID RegistProperty();

    public:
        virtual VOID PrevSave();
        virtual VOID PostSave();
        virtual VOID PostLoad();

        virtual VOID CheckForErrors();
        virtual VOID OnCreated();
        virtual VOID BeginPlay();
        virtual VOID Activated( size_t nInput = 0 );

        VOID ExecuteOp( FLOAT fElapsedTime );
        VOID UpdateOp( FLOAT fElapsedTime );

        CSequenceObject* NewObject( size_t nIndex, const ic::Int2& sPos );
        VOID InsertObject( CSequenceObject* pTarget );
        VOID PasteObject( SequenceObjectVec& vecObjects );
        VOID PastePostObject( CSequenceObject* pTarget );
        VOID PastePostInSequence();
        VOID DeleteObject( CSequenceObject* pTarget );
        VOID MekeUniqueName( CSequenceObject* pTarget );
        VOID ClearObject();

        VOID BreakLinkOutput( CSequenceObject* pTarget );
        VOID BreakLinkVariable( CSequenceObject* pTarget );
        VOID BreakLink( CSequenceObject* pTarget, INT nConnType, INT nConnIndex, INT nLinkIndex );
        VOID BreakLinkAll( CSequenceObject* pTarget );
        VOID BreakLinkAll();

        VOID FindObjectByClass( ic::CBaseObject* pDesired, SequenceObjectVec& vecObjects );
        VOID FindObjectByName( const TSTRING& strName, SequenceObjectVec& vecObjects );
        VOID FindRemoteEvent( const TSTRING& strLabel, SequenceObjectVec& vecObjects );
        VOID FindNamedVariable( const TSTRING& strVarName, BOOL bFindUses, SeqVariableVec& vecVars );
        VOID FindActivateRemoteEvent( const TSTRING& strLabel, SequenceObjectVec& vecObjects );

        virtual VOID UpdateConnectors();
        VOID UpdateRemoteEventStatus();
        VOID UpdateNamedVarStatus();
        VOID UpdateActivateRemoteEventStatus();

        virtual VOID DrawSequence(
            ic::IRenderInterface* pRI,
            const SequenceObjectVec& vecSelected,
            const CSequenceObject* pMouseOver,
            INT nMouseOverConnType,
            INT nMouseOverConnIndex );

        VOID OnChangedName();

    private:
        SERIALIZE_DEFAULT;
    };
}