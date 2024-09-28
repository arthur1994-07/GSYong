/**
 * \date	2011/01/27
 * \author	Jun-Hyuk Choi
 * \ref     Unreal3
 */
#pragma once

#include "./SequenceOp.h"

namespace vs
{
    class CSequenceAction : public ic::CRttiObject< CSequenceAction, CSequenceOp, 1, ic::EInternal >
    {
    public:
        CSequenceAction();

    private:
        SERIALIZE_EMPTY( CSequenceOp );
    };

    class CSeqAct_FinishSequence : public ic::CRttiObject< CSeqAct_FinishSequence, CSequenceAction, 1, 0 >
    {
    public:
        TSTRING strLabel;

    public:
        CSeqAct_FinishSequence();

        VOID PostNew();
        VOID RegistProperty();

    public:
        virtual TSTRING GetObjectName();

        virtual VOID OnCreated();
        virtual VOID Activated( size_t nInput = 0 );

        VOID OnChangedLabel();

    private:
        SERIALIZE_DEFAULT;
    };

    class CSeqAct_ActivateRemoteEvent : public ic::CRttiObject< CSeqAct_ActivateRemoteEvent, CSequenceAction, 1, 0 >
    {
    public:
        TSTRING strLabel;
        BOOL bStatusIsOk;

    public:
        CSeqAct_ActivateRemoteEvent();

        VOID PostNew();
        VOID RegistProperty();

    public:
        virtual TSTRING GetObjectName();

        virtual VOID PostLoad();

        virtual VOID OnCreated();
        virtual VOID Activated( size_t nInput = 0 );

        virtual VOID DrawExtraInfo(
            ic::IRenderInterface* pRI,
            const ic::Int2& sCenter );

        VOID OnChangedLabel();

    private:
        SERIALIZE_DEFAULT;
    };

    class CSeqAct_SetBool : public ic::CRttiObject< CSeqAct_SetBool, CSequenceAction, 1, 0 >
    {
    public:
        BOOL bDefaultValue;

    public:
        CSeqAct_SetBool();

        VOID PostNew();
        VOID RegistProperty();

    public:
        virtual VOID Activated( size_t nInput = 0 );

    private:
        SERIALIZE_DEFAULT;
    };

    class CSeqAct_SetDouble : public ic::CRttiObject< CSeqAct_SetDouble, CSequenceAction, 1, 0 >
    {
    public:
        DOUBLE nDefaultValue;

    public:
        CSeqAct_SetDouble();

        VOID PostNew();
        VOID RegistProperty();

    public:
        virtual VOID Activated( size_t nInput = 0 );

    private:
        SERIALIZE_DEFAULT;
    };

    class CSeqAct_SetString : public ic::CRttiObject< CSeqAct_SetString, CSequenceAction, 1, 0 >
    {
    public:
        TSTRING strDefaultValue;

    public:
        CSeqAct_SetString();

        VOID PostNew();
        VOID RegistProperty();

    public:
        virtual VOID Activated( size_t nInput = 0 );

    private:
        SERIALIZE_DEFAULT;
    };

    class CSeqAct_SetVector3 : public ic::CRttiObject< CSeqAct_SetVector3, CSequenceAction, 1, 0 >
    {
    public:
        D3DXVECTOR3 vDefaultValue;

    public:
        CSeqAct_SetVector3();

        VOID PostNew();
        VOID RegistProperty();

    public:
        virtual VOID Activated( size_t nInput = 0 );

    private:
        SERIALIZE_DEFAULT;
    };

    class CSeqAct_SetVector3Components : public ic::CRttiObject< CSeqAct_SetVector3Components, CSequenceAction, 1, 0 >
    {
    public:
        CSeqAct_SetVector3Components();

        VOID PostNew();

    public:
        virtual VOID Activated( size_t nInput = 0 );

    private:
        SERIALIZE_EMPTY( CSequenceAction );
    };

    class CSeqAct_GetVector3Components : public ic::CRttiObject< CSeqAct_GetVector3Components, CSequenceAction, 1, 0 >
    {
    public:
        CSeqAct_GetVector3Components();

        VOID PostNew();

    public:
        virtual VOID Activated( size_t nInput = 0 );

    private:
        SERIALIZE_EMPTY( CSequenceAction );
    };

    class CSeqAct_SetObject : public ic::CRttiObject< CSeqAct_SetObject, CSequenceAction, 1, 0 >
    {
    public:
        ic::CBaseObject* pDefaultObject;

    public:
        CSeqAct_SetObject();

        VOID PostNew();

    public:
        virtual VOID Activated( size_t nInput = 0 );

    private:
        SERIALIZE_EMPTY( CSequenceAction );
    };

    class CSeqAct_AddDouble : public ic::CRttiObject< CSeqAct_AddDouble, CSequenceAction, 1, 0 >
    {
    public:
        CSeqAct_AddDouble();

        VOID PostNew();

    public:
        virtual VOID Activated( size_t nInput = 0 );

    private:
        SERIALIZE_EMPTY( CSequenceAction );
    };

    class CSeqAct_SubtractDouble : public ic::CRttiObject< CSeqAct_SubtractDouble, CSequenceAction, 1, 0 >
    {
    public:
        CSeqAct_SubtractDouble();

        VOID PostNew();

    public:
        virtual VOID Activated( size_t nInput = 0 );

    private:
        SERIALIZE_EMPTY( CSequenceAction );
    };

    class CSeqAct_MultiplyDouble : public ic::CRttiObject< CSeqAct_MultiplyDouble, CSequenceAction, 1, 0 >
    {
    public:
        CSeqAct_MultiplyDouble();

        VOID PostNew();

    public:
        virtual VOID Activated( size_t nInput = 0 );

    private:
        SERIALIZE_EMPTY( CSequenceAction );
    };

    class CSeqAct_DivideDouble : public ic::CRttiObject< CSeqAct_DivideDouble, CSequenceAction, 1, 0 >
    {
    public:
        CSeqAct_DivideDouble();

        VOID PostNew();

    public:
        virtual VOID Activated( size_t nInput = 0 );

    private:
        SERIALIZE_EMPTY( CSequenceAction );
    };

    class CSeqAct_CompareBool : public ic::CRttiObject< CSeqAct_CompareBool, CSequenceAction, 1, 0 >
    {
    public:
        CSeqAct_CompareBool();

        VOID PostNew();

    public:
        virtual VOID Activated( size_t nInput = 0 );

    private:
        SERIALIZE_EMPTY( CSequenceAction );
    };

    class CSeqAct_CompareDouble : public ic::CRttiObject< CSeqAct_CompareDouble, CSequenceAction, 1, 0 >
    {
    public:
        DOUBLE nDefaultA;
        DOUBLE nDefaultB;

    public:
        CSeqAct_CompareDouble();

        VOID PostNew();
        VOID RegistProperty();

    public:
        virtual VOID Activated( size_t nInput = 0 );

    private:
        SERIALIZE_DEFAULT;
    };

    class CSeqAct_CompareString : public ic::CRttiObject< CSeqAct_CompareString, CSequenceAction, 1, 0 >
    {
    public:
        TSTRING strDefaultA;
        TSTRING strDefaultB;

    public:
        CSeqAct_CompareString();

        VOID PostNew();
        VOID RegistProperty();

    public:
        virtual VOID Activated( size_t nInput = 0 );

    private:
        SERIALIZE_DEFAULT;
    };

    class CSeqAct_CompareVector3 : public ic::CRttiObject< CSeqAct_CompareVector3, CSequenceAction, 1, 0 >
    {
    public:
        D3DXVECTOR3 vDefaultA;
        D3DXVECTOR3 vDefaultB;

    public:
        CSeqAct_CompareVector3();

        VOID PostNew();
        VOID RegistProperty();

    public:
        virtual VOID Activated( size_t nInput = 0 );

    private:
        SERIALIZE_DEFAULT;
    };

    class CSeqAct_CompareObject : public ic::CRttiObject< CSeqAct_CompareObject, CSequenceAction, 1, 0 >
    {
    public:
        CSeqAct_CompareObject();

        VOID PostNew();

    public:
        virtual VOID Activated( size_t nInput = 0 );

    private:
        SERIALIZE_EMPTY( CSequenceAction );
    };

    class CSeqAct_Toggle : public ic::CRttiObject< CSeqAct_Toggle, CSequenceAction, 1, 0 >
    {
    public:
        CSeqAct_Toggle();

        VOID PostNew();

    public:
        virtual VOID Activated( size_t nInput = 0 );

    private:
        SERIALIZE_EMPTY( CSequenceAction );
    };

    class CSeqAct_Switch : public ic::CRttiObject< CSeqAct_Switch, CSequenceAction, 1, 0 >
    {
    public:
        INT nLinkCount;
        INT nIncrementAmount;

    public:
        CSeqAct_Switch();

        VOID PostNew();
        VOID RegistProperty();

    public:
        virtual VOID Activated( size_t nInput = 0 );

        VOID OnChangedLinkCount();

    private:
        SERIALIZE_DEFAULT;
    };

    class CSeqAct_RandomSwitch : public ic::CRttiObject< CSeqAct_RandomSwitch, CSeqAct_Switch, 1, 0 >
    {
    public:
        CSeqAct_RandomSwitch();

        VOID PostNew();

    public:
        virtual VOID Activated( size_t nInput = 0 );

    private:
        SERIALIZE_EMPTY( CSeqAct_Switch );
    };

    class CSeqAct_PlaySound : public ic::CRttiObject< CSeqAct_PlaySound, CSequenceAction, 1, 0 >
    {
    public:
        CSeqAct_PlaySound();

        VOID PostNew();

    public:
        virtual VOID Activated( size_t nInput = 0 );

    private:
        SERIALIZE_EMPTY( CSequenceAction );
    };

    class CSeqAct_PlayEffect : public ic::CRttiObject< CSeqAct_PlayEffect, CSequenceAction, 1, 0 >
    {
    public:
        CSeqAct_PlayEffect();

        VOID PostNew();

    public:
        virtual VOID Activated( size_t nInput = 0 );

    private:
        SERIALIZE_EMPTY( CSequenceAction );
    };

    class CSeqAct_OutputString : public ic::CRttiObject< CSeqAct_OutputString, CSequenceAction, 1, 0 >
    {
    public:
        enum
        {
            EDebug    = 0x01,
            EFile     = 0x02,
            EChat     = 0x04,
            ENotice   = 0x08,
        };

    public:
        DWORD nTargets;

    public:
        CSeqAct_OutputString();

        VOID PostNew();
        VOID RegistProperty();

    public:
        virtual VOID Activated( size_t nInput = 0 );

    private:
        SERIALIZE_DEFAULT;
    };

    class CSeqAct_SetProperty : public ic::CRttiObject< CSeqAct_SetProperty, CSequenceAction, 1, 0 >
    {
    public:
        TSTRING strKey;
        BOOL bStatusIsOk;

    public:
        CSeqAct_SetProperty();

        VOID PostNew();
        VOID RegistProperty();

    public:
        virtual TSTRING GetObjectName();

        virtual VOID PostLoad();

        virtual VOID OnLinkVariable( CSequenceObject* pObject, INT nConnIndex );
        virtual VOID OnPostBreakVariable( INT nConnIndex );

    public:
        virtual VOID Activated( size_t nInput = 0 );

        virtual VOID DrawExtraInfo(
            ic::IRenderInterface* pRI,
            const ic::Int2& sCenter );

        VOID OnChangedKey();

    private:
        SERIALIZE_DEFAULT;
    };

    class CSeqAct_GetProperty : public ic::CRttiObject< CSeqAct_GetProperty, CSequenceAction, 1, 0 >
    {
    public:
        TSTRING strKey;
        BOOL bStatusIsOk;

    public:
        CSeqAct_GetProperty();

        VOID PostNew();
        VOID RegistProperty();

    public:
        virtual TSTRING GetObjectName();

        virtual VOID PostLoad();

        virtual VOID OnLinkVariable( CSequenceObject* pObject, INT nConnIndex );
        virtual VOID OnPostBreakVariable( INT nConnIndex );

    public:
        virtual VOID Activated( size_t nInput = 0 );

        virtual VOID DrawExtraInfo(
            ic::IRenderInterface* pRI,
            const ic::Int2& sCenter );

        VOID OnChangedKey();

    private:
        SERIALIZE_DEFAULT;
    };

    class CSeqAct_Function : public ic::CRttiObject< CSeqAct_Function, CSequenceAction, 1, 0 >
    {
    public:
        TSTRING strKey;
        BOOL bStatusIsOk;

    public:
        CSeqAct_Function();

        VOID PostNew();
        VOID RegistProperty();

    public:
        virtual TSTRING GetObjectName();

        virtual VOID PostLoad();

        virtual VOID OnLinkVariable( CSequenceObject* pObject, INT nConnIndex );
        virtual VOID OnPostBreakVariable( INT nConnIndex );

    public:
        virtual VOID Activated( size_t nInput = 0 );

        virtual VOID DrawExtraInfo(
            ic::IRenderInterface* pRI,
            const ic::Int2& sCenter );

        VOID OnChangedKey();

    private:
        SERIALIZE_DEFAULT;
    };
}