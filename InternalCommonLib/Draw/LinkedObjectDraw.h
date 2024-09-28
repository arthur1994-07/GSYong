/**
 * \date	2011/01/27
 * \author	Jun-Hyuk Choi
 * \ref     Unreal3
 */
#pragma once

#include <string>
#include <vector>

#include "../Core/HitProxy.h"

namespace ic
{
    class CBaseObject;
    class IRenderInterface;

    extern const INT CaptionHeight;
    extern const INT ConnectorWidth;
    extern const INT ConnectorLength;
    extern const INT DescPaddingX;
    extern const INT DescPaddingY;
    extern const INT TextBorder;
    extern const INT MinShapeSize;
    extern const INT DiamondSize;

    enum
    {
        EInput,
        EOutput,
        EVariable,
    };

    struct SLinkedObjProxy : public SHitProxy
    {
	    CBaseObject* pObject;

	    SLinkedObjProxy( CBaseObject* pObject_ )
            : pObject( pObject_ )
	    {
        }
    };

    struct SLinkedObjProxySpecial : public SHitProxy
    {
	    CBaseObject* pObject;

	    SLinkedObjProxySpecial( CBaseObject* pObject_ )
            : pObject( pObject_ )
	    {
        }
    };

    struct SLinkedObjConnectorProxy : public SHitProxy
    {
	    CBaseObject* pObject;
	    INT nConnType;
	    INT nConnIndex;

	    SLinkedObjConnectorProxy( CBaseObject* pObject_, INT nConnType_, INT nConnIndex_ )
            : pObject( pObject_ )
            , nConnType( nConnType_ )
            , nConnIndex( nConnIndex_ )
	    {
        }
    };

    struct SLinkedObjConnInfo
    {
        const TCHAR* pName;
        BOOL bWriteable;
        FLOAT fDelay;
        DWORD nColor;

        SLinkedObjConnInfo()
            : pName( NULL )
            , bWriteable( FALSE )
            , fDelay( 0.0f )
            , nColor( D3DCOLOR_XRGB( 0, 0, 0 ) )
        {
        }
    };

    struct SLinkedObjDrawInfo
    {
        typedef std::vector< SLinkedObjConnInfo > LinkedObjConnInfoVec;
        typedef std::vector< INT > LinkVec;

        LinkedObjConnInfoVec vecInputs;
        LinkedObjConnInfoVec vecOutputs;
        LinkedObjConnInfoVec vecVariables;

        CBaseObject* pObject;

        LinkVec vecInputY;
        LinkVec vecOutputY;
        LinkVec vecVariableX;
        Int2 sDrawSize;

        SLinkedObjDrawInfo()
            : pObject( NULL )
        {
        }
    };

    class CLinkedObjDrawUtils
    {
    public:
        static VOID DrawNGon(
            IRenderInterface* pRI,
            const Int2& sCenter,
            const DWORD nColor,
            INT nNumSides,
            INT nRadius );

        static VOID DrawDiamond(
            IRenderInterface* pRI,
            const Int2& sPos,
            const Int2& sSize,
            const DWORD nColor );

        static VOID DrawSpline(
            IRenderInterface* pRI,
            const Int2& Start,
            const D3DXVECTOR2& StartDir,
            const Int2& End,
            const D3DXVECTOR2& EndDir,
            const DWORD LineColor,
            BOOL bArrowhead );

        static VOID DrawArrowhead(
            IRenderInterface* pRI,
            const Int2& sPos,
            const D3DXVECTOR2& vDir,
            const DWORD nColor,
            BOOL bAlphaBlend );

	    static Int2 GetTitleBarSize(
            IRenderInterface* pRI,
            const TCHAR* pName );

        static VOID DrawTitleBar(
            IRenderInterface* pRI,
            const Int2& sPos,
            const Int2& sSize,
            const DWORD nBorderColor,
            const DWORD nBkgColor,
            const TCHAR* pName,
            const TCHAR* pComment = NULL );

	    static Int2 GetLogicConnectorsSize(
            IRenderInterface* pRI,
            const SLinkedObjDrawInfo& sObjInfo,
            INT* pInputY = NULL,
            INT* pOutputY = NULL );

        static VOID DrawLogicConnectors(
            IRenderInterface* pRI,
            SLinkedObjDrawInfo& sObjInfo,
            const Int2& sPos,
            const Int2& sSize );

	    static Int2 GetVariableConnectorsSize(
            IRenderInterface* pRI,
            const SLinkedObjDrawInfo& sObjInfo );

        static VOID DrawVariableConnectors(
            IRenderInterface* pRI,
            SLinkedObjDrawInfo& sObjInfo,
            const Int2& sPos,
            const Int2& Size );

	    static VOID DrawLinkedObj(
            IRenderInterface* pRI,
            SLinkedObjDrawInfo& sObjInfo,
            const TCHAR* pName,
            const TCHAR* pComment,
            const DWORD nBorderColor,
            const DWORD nTitleBkgColor,
            const Int2& sPos );
    };
}