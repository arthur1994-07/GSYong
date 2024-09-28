/**
 * \date	2011/01/27
 * \author	Jun-Hyuk Choi
 * \ref     Unreal3
 */
#pragma once

#include "./SequenceOp.h"

namespace vs
{
    class CSequenceEvent : public ic::CRttiObject< CSequenceEvent, CSequenceOp, 1, ic::EInternal >
    {
    public:
        BOOL bEnabled;
        INT nTriggerCount;
        INT nMaxTriggerCount;
        FLOAT fReTriggerDelay;
        FLOAT fActivationTime;
        INT nMaxWidth;
        BOOL bActive;

    public:
        CSequenceEvent();

        VOID RegistProperty();

    public:
        virtual VOID ProcessInput();

        virtual VOID InsertInputImpulse( size_t nIndex = 0 );
        virtual BOOL UpdateInputImpulse( FLOAT fElapsedTime ) { return bActive; }

        virtual VOID DrawObject(
            ic::IRenderInterface* pRI,
            BOOL bSelected,
            BOOL bMouseOver,
            INT nMouseOverConnType,
            INT nMouseOverConnIndex );

        virtual ic::Int4 GetBoundingBox() const;

        ic::Int2 GetConnectionLocation();

    private:
        SERIALIZE_DEFAULT;
    };

    class CSeqEvent_LevelLoaded : public ic::CRttiObject< CSeqEvent_LevelLoaded, CSequenceEvent, 1, 0 >
    {
    public:
        CSeqEvent_LevelLoaded();

        VOID PostNew();

    private:
        SERIALIZE_EMPTY( CSequenceEvent );
    };

    class CSeqEvent_SequenceActivated : public ic::CRttiObject< CSeqEvent_SequenceActivated, CSequenceEvent, 1, 0 >
    {
    public:
        TSTRING strLabel;

    public:
        CSeqEvent_SequenceActivated();

        VOID PostNew();
        VOID RegistProperty();

    public:
        virtual TSTRING GetObjectName();

        virtual VOID OnCreated();

        VOID OnChangedLabel();

    private:
        SERIALIZE_DEFAULT;
    };

    class CSeqEvent_Touch : public ic::CRttiObject< CSeqEvent_Touch, CSequenceEvent, 1, 0 >
    {
    public:
        CSeqEvent_Touch();

        VOID PostNew();

    public:
        virtual VOID DeActivated() {}

    private:
        SERIALIZE_EMPTY( CSequenceEvent );
    };

    class CSeqEvent_RemoteEvent : public ic::CRttiObject< CSeqEvent_RemoteEvent, CSequenceEvent, 1, 0 >
    {
    public:
        TSTRING strLabel;
        BOOL bStatusIsOk;

    public:
        CSeqEvent_RemoteEvent();

        VOID PostNew();
        VOID RegistProperty();

    public:
        virtual TSTRING GetObjectName();

        virtual VOID PostLoad();

        virtual VOID OnCreated();

        virtual VOID DrawExtraInfo(
            ic::IRenderInterface* pRI,
            const ic::Int2& sCenter );

        VOID OnChangedLabel();

    private:
        SERIALIZE_DEFAULT;
    };
}