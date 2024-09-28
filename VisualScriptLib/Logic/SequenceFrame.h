/**
 * \date	2011/01/27
 * \author	Jun-Hyuk Choi
 * \ref     Unreal3
 */
#pragma once

#include "./SequenceObject.h"

namespace vs
{
    class CSequenceFrame : public ic::CRttiObject< CSequenceFrame, CSequenceObject, 1, ic::EInternal >
    {
    public:
        ic::Int2 sSize;
        INT nBorderWidth;
        BOOL bDrawBox;
        BOOL bFilled;
        BOOL bTileFill;
        DWORD nBorderColor;
        DWORD nFillColor;

    public:
        CSequenceFrame();

        VOID RegistProperty();

    public:
        virtual VOID DrawObject(
            ic::IRenderInterface* pRI,
            BOOL bSelected,
            BOOL bMouseOver,
            INT nMouseOverConnType,
            INT nMouseOverConnIndex );

    private:
        SERIALIZE_DEFAULT;
    };
}