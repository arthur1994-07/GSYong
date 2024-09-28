/**
 * \date	2011/01/27
 * \author	Jun-Hyuk Choi
 * \ref     Unreal3
 */
#pragma once

#include "../../InternalCommonLib/Core/BaseObject.h"
#include "../../InternalCommonLib/Draw/LinkedObjectDraw.h"

namespace ic
{
    class IRenderInterface;
}

namespace vs
{
    class CSequence;

    extern const TSTRING TickMaterial;
    extern const TSTRING CrossMaterial;
    extern const DWORD SelectedColor;
    extern const DWORD MouseOverLogicColor;
    extern const FLOAT MouseOverColorScale;
    extern const DWORD TitleBackColor;
    extern const DWORD SequenceTitleBackColor;
    extern const FLOAT SequenceImpulseEnable;
    extern const FLOAT SequenceImpulseDisable;

    class CSequenceObject : public ic::CRttiObject< CSequenceObject, ic::CBaseObject, 1, ic::EInternal >
    {
    public:
        TSTRING strName;
        TSTRING strComment;
        ic::Int2 sPos;
        ic::Int2 sDrawSize;
        DWORD nColor;
        BOOL bDrawFirst;
        BOOL bDrawLast;

    public:
        CSequenceObject();

        VOID RegistProperty();

    public:
        TSTRING GetFullName();
        CSequence* GetRootSequence();
        ic::Int2 GetTitleBarSize( ic::IRenderInterface* pRI );
        DWORD GetBorderColor( BOOL bSelected, BOOL bMouseOver );

    public:
        virtual const TSTRING& GetClassName() const { return strName; }
        virtual TSTRING GetObjectName() { return strName; }

        virtual VOID PrevSave() {}
        virtual VOID PostSave() {}
        virtual VOID PostLoad() {}

        virtual VOID CheckForErrors() {};
        virtual VOID OnCreated() {}
        virtual VOID OnSelected() {}

        virtual VOID OnLinkVariable( CSequenceObject* pObject, INT nConnIndex ) {}
        virtual VOID OnLinkFrom( CSequenceObject* pObject, INT nConnIndex ) {}
        virtual VOID OnPostBreakVariable( INT nConnIndex ) {}

        virtual VOID DrawObject(
            ic::IRenderInterface* pRI,
            BOOL bSelected,
            BOOL bMouseOver,
            INT nMouseOverConnType,
            INT nMouseOverConnIndex ) {}

        virtual VOID DrawExtraInfo(
            ic::IRenderInterface* pRI,
            const ic::Int2& sCenter ) {}

        VOID DrawTitleBar(
            ic::IRenderInterface* pRI,
            BOOL bSelected,
            BOOL bMouseOver,
            const ic::Int2& sPos_,
            const ic::Int2& sSize );

        VOID DrawStatusMark(
            ic::IRenderInterface* pRI,
            const ic::Int2& sCenter,
            BOOL bStatusIsOk );

        virtual VOID DrawLogicLinks( ic::IRenderInterface* pRI ) {}
        virtual VOID DrawVariableLinks( ic::IRenderInterface* pRI ) {}

        virtual ic::Int4 GetBoundingBox() const;

    private:
        SERIALIZE_DEFAULT;
    };

    typedef std::map< DWORD_PTR, CSequenceObject* > SequenceObjectMap;
    typedef SequenceObjectMap::value_type SequenceObjectMapValue;
    typedef SequenceObjectMap::iterator SequenceObjectMapIter;
    typedef SequenceObjectMap::const_iterator SequenceObjectMapCIter;

    typedef std::vector< CSequenceObject* > SequenceObjectVec;
    typedef SequenceObjectVec::iterator SequenceObjectVecIter;
    typedef SequenceObjectVec::const_iterator SequenceObjectVecCIter;

    struct CompSequenceObject
    {
        bool operator () ( const CSequenceObject* pLeft, const CSequenceObject* pRight )
        {
            INT nCompare = pLeft->GetClassName().compare( pRight->GetClassName() );
            if( nCompare == 0 )
            {
                if( pLeft->sPos.x == pRight->sPos.x )
                    return pLeft->sPos.y < pRight->sPos.y;
                return pLeft->sPos.x < pRight->sPos.x;
            }
            return nCompare < 0;
        }
    };
}