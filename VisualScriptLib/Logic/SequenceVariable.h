/**
 * \date	2011/01/27
 * \author	Jun-Hyuk Choi
 * \ref     Unreal3
 */
#pragma once

#include "./SequenceObject.h"

namespace vs
{
    class CSequenceVariable : public ic::CRttiObject< CSequenceVariable, CSequenceObject, 1, ic::EInternal >
    {
    public:
        TSTRING strVarName;

    public:
        CSequenceVariable();

        VOID RegistProperty();

    public:
        virtual VOID DrawObject(
            ic::IRenderInterface* pRI,
            BOOL bSelected,
            BOOL bMouseOver,
            INT nMouseOverConnType,
            INT nMouseOverConnIndex );

        virtual ic::Int4 GetBoundingBox() const;

        ic::Int2 GetConnectionLocation();

        VOID OnChangedVarName();

    public:
        virtual ic::CProperty* GetProperty();
        virtual TSTRING GetValueText() { return _T( "" ); }

    private:
        SERIALIZE_DEFAULT;
    };

    class CSeqVar_Bool : public ic::CRttiObject< CSeqVar_Bool, CSequenceVariable, 1, 0 >
    {
    public:
        BOOL bValue;

    public:
        CSeqVar_Bool();

        VOID RegistProperty();

    public:
        virtual TSTRING GetValueText();

    private:
        SERIALIZE_DEFAULT;
    };

    class CSeqVar_Double : public ic::CRttiObject< CSeqVar_Double, CSequenceVariable, 1, 0 >
    {
    public:
        DOUBLE nValue;

    public:
        CSeqVar_Double();

        VOID RegistProperty();

    public:
        virtual TSTRING GetValueText();

    private:
        SERIALIZE_DEFAULT;
    };

    class CSeqVar_String : public ic::CRttiObject< CSeqVar_String, CSequenceVariable, 1, 0 >
    {
    public:
        TSTRING strValue;

    public:
        CSeqVar_String();

        VOID RegistProperty();

    public:
        virtual TSTRING GetValueText() { return strValue; }

    private:
        SERIALIZE_DEFAULT;
    };

    class CSeqVar_Vector3 : public ic::CRttiObject< CSeqVar_Vector3, CSequenceVariable, 1, 0 >
    {
    public:
        D3DXVECTOR3 vValue;

    public:
        CSeqVar_Vector3();

        VOID RegistProperty();

    public:
        virtual TSTRING GetValueText();

    private:
        SERIALIZE_DEFAULT;
    };

    class CSeqVar_Object : public ic::CRttiObject< CSeqVar_Object, CSequenceVariable, 1, 0 >
    {
    public:
        ic::CBaseObject* pValue;

    public:
        CSeqVar_Object();

        VOID RegistProperty();

    public:
        virtual TSTRING GetValueText();

    private:
        SERIALIZE_DEFAULT;
    };

    class CSeqVar_Player : public ic::CRttiObject< CSeqVar_Player, CSeqVar_Object, 1, 0 >
    {
    public:
        CSeqVar_Player();

        VOID PostNew();
        VOID RegistProperty();

    public:
        virtual TSTRING GetValueText();

    public:
        VOID TestFunction( ic::IPropertyValueVec& vecArgs );

    private:
        SERIALIZE_EMPTY( CSeqVar_Object );
    };

    class CSeqVar_Named : public ic::CRttiObject< CSeqVar_Named, CSequenceVariable, 1, 0 >
    {
    public:
        ic::CBaseObject* pExpected;
        TSTRING strFindVarName;
        BOOL bStatusIsOk;
        CSequenceVariable* pLinkVar;

    public:
        CSeqVar_Named();

        VOID RegistProperty();

    public:
        virtual VOID PostLoad();

        virtual VOID OnLinkFrom( CSequenceObject* pObject, INT nConnIndex );

        virtual VOID DrawExtraInfo(
            ic::IRenderInterface* pRI,
            const ic::Int2& sCenter );

        VOID OnChangedClass();
        VOID OnChangedFindVarName();

    public:
        virtual ic::CProperty* GetProperty();
        virtual TSTRING GetValueText();

    private:
        SERIALIZE_DEFAULT;
    };
}