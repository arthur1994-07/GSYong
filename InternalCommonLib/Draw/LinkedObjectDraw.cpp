#include "stdafx.h"

#include "../Core/CommonUtil.h"
#include "../Interface/RenderInterface.h"

#include "./LinkedObjectDraw.h"

#include "../DebugInclude.h"

namespace ic
{
    const INT CaptionHeight = 22;
    const INT ConnectorWidth = 8;
    const INT ConnectorLength = 10;
    const INT DescPaddingX = 8;
    const INT DescPaddingY = 8;
    const INT TextBorder = 3;
    const INT MinShapeSize = 64;
    const INT DiamondSize = MinShapeSize / 2;
    const FLOAT ArrowheadLength = 14.0f;
    const FLOAT ArrowheadWidth = 4.0f;

    VOID CLinkedObjDrawUtils::DrawNGon(
        IRenderInterface* pRI,
        const Int2& sCenter,
        const DWORD nColor,
        INT nNumSides,
        INT nRadius )
    {
        Int2 sVerts[ 256 ];

        nNumSides = Clamp< INT >( nNumSides, 3, 255 );

        for( INT i=0; i<nNumSides+1; ++i )
        {
            FLOAT fAngle = ( D3DX_PI * 2.0f ) * FLOAT( i ) / FLOAT( nNumSides );

            sVerts[ i ].x = sCenter.x + INT( FLOAT( nRadius ) * cosf( fAngle ) );
            sVerts[ i ].y = sCenter.y + INT( FLOAT( nRadius ) * sinf( fAngle ) );
        }

        for( INT i=0; i<nNumSides; ++i )
        {
            pRI->DrawTriangle(
                sCenter,
                D3DXVECTOR2( 0.0f, 0.0f ),
                sVerts[ i ],
                D3DXVECTOR2( 0.0f, 0.0f ),
                sVerts[ i + 1 ],
                D3DXVECTOR2( 0.0f, 0.0f ),
                nColor );
        }
    }

    VOID CLinkedObjDrawUtils::DrawDiamond(
        IRenderInterface* pRI,
        const Int2& sPos,
        const Int2& sSize,
        const DWORD nColor )
    {
        pRI->DrawTriangle(
            Int2( sPos.x, sPos.y + INT( DiamondSize * 0.5f ) ),
            D3DXVECTOR2( 0.0f, 0.0f ),
            Int2( sPos.x + INT( sSize.x * 0.5f ), sPos.y ),
            D3DXVECTOR2( 0.0f, 0.0f ),
            Int2( sPos.x + sSize.x, sPos.y + INT( DiamondSize * 0.5f ) ),
            D3DXVECTOR2( 0.0f, 0.0f ),
            nColor );

        pRI->DrawTile(
            Int2( sPos.x, sPos.y + INT( DiamondSize * 0.5f ) ),
            sSize,
            D3DXVECTOR2( 0.0f, 0.0f ),
            D3DXVECTOR2( 0.0f, 0.0f ),
            nColor );

        pRI->DrawTriangle(
            Int2( sPos.x + sSize.x, sPos.y + sSize.y + INT( DiamondSize * 0.5f ) ),
            D3DXVECTOR2( 0.0f, 0.0f ),
            Int2( sPos.x + INT( sSize.x * 0.5f ), sPos.y + sSize.y + DiamondSize ),
            D3DXVECTOR2( 0.0f, 0.0f ),
            Int2( sPos.x, sPos.y + sSize.y + INT( DiamondSize * 0.5f ) ),
            D3DXVECTOR2( 0.0f, 0.0f ),
            nColor );
    }

    VOID CLinkedObjDrawUtils::DrawSpline(
        IRenderInterface* pRI,
        const Int2& sStart,
        const D3DXVECTOR2& vStartDir,
        const Int2& sEnd,
        const D3DXVECTOR2& vEndDir,
        const DWORD nLineColor,
        BOOL bArrowhead )
    {
        const FLOAT	MaxPixelsPerStep = 10.0f;

        D3DXVECTOR2 vStart( FLOAT( sStart.x ), FLOAT( sStart.y ) );
	    D3DXVECTOR2 vEnd( FLOAT( sEnd.x ), FLOAT( sEnd.y ) );

	    FLOAT fDirectLength = D3DXVec2Length( &D3DXVECTOR2( vEnd - vStart ) );
	    FLOAT fHandleLength = D3DXVec2Length( &D3DXVECTOR2( ( vEnd - vEndDir ) - ( vStart + vStartDir ) ) );

        INT nNumSteps = INT( ceilf( max( fDirectLength, fHandleLength ) / MaxPixelsPerStep ) );
        D3DXVECTOR2 vOldPos = vStart;

	    for( INT i=0; i<nNumSteps; ++i )
	    {
		    FLOAT fAlpha = ( FLOAT( i ) + 1.0f ) / FLOAT( nNumSteps );
		    D3DXVECTOR2 vNewPos = CubicInterp( vStart, vStartDir, vEnd, vEndDir, fAlpha );

		    pRI->DrawLine(
                Int2( INT( vOldPos.x ), INT( vOldPos.y ) ),
                Int2( INT( vNewPos.x ), INT( vNewPos.y ) ),
                nLineColor );

		    if( bArrowhead && ( i == nNumSteps - 1 ) && ( i >= 2 ) )
		    {
			    FLOAT fArrowStartAlpha = ( FLOAT( i ) - 2.0f ) / FLOAT( nNumSteps );
			    D3DXVECTOR2 vArrowStartPos = CubicInterp( vStart, vStartDir, vEnd, vEndDir, fArrowStartAlpha );

			    D3DXVECTOR2 vStepDir = vNewPos - vArrowStartPos;
                D3DXVec2Normalize( &vStepDir, &vStepDir );
			    DrawArrowhead(
                    pRI,
                    Int2( INT( vNewPos.x ), INT( vNewPos.y ) ),
                    vStepDir,
                    nLineColor,
                    FALSE );
		    }

		    vOldPos = vNewPos;
	    }
    }

    VOID CLinkedObjDrawUtils::DrawArrowhead(
        IRenderInterface* pRI,
        const Int2& sPos,
        const D3DXVECTOR2& vDir,
        const DWORD nColor,
        BOOL bAlphaBlend )
    {
        D3DXVECTOR2 vOrth( vDir.y, -vDir.x );
	    D3DXVECTOR2 vPosVec( FLOAT( sPos.x ), FLOAT( sPos.y ) );
	    D3DXVECTOR2 vP2 = vPosVec - ( vDir * ArrowheadLength ) - ( vOrth * ArrowheadWidth );
	    D3DXVECTOR2 vP1 = vPosVec;
	    D3DXVECTOR2 vP3 = vPosVec - ( vDir * ArrowheadLength ) + ( vOrth * ArrowheadWidth );

        pRI->DrawTriangle(
            Int2( INT( vP1.x ), INT( vP1.y ) ),
            D3DXVECTOR2( 0.0f, 0.0f ),
            Int2( INT( vP2.x ), INT( vP2.y ) ),
            D3DXVECTOR2( 0.0f, 0.0f ),
            Int2( INT( vP3.x ), INT( vP3.y ) ),
            D3DXVECTOR2( 0.0f, 0.0f ),
		    nColor,
            NULL,
            bAlphaBlend );
    }

    Int2 CLinkedObjDrawUtils::GetTitleBarSize(
        IRenderInterface* pRI,
        const TCHAR* pName )
    {
        SIZE sTextSize = { 0, 0 };
        pRI->GetTextExtent(
            sTextSize,
            pName,
            pRI->DefaultFontName(),
            pRI->DefaultFontSize(),
            pRI->DefaultFontZoom() );

	    INT nLabelWidth = sTextSize.cx + ( TextBorder * 2 ) + 4;

	    return Int2( max( nLabelWidth, MinShapeSize ), CaptionHeight );
    }

    VOID CLinkedObjDrawUtils::DrawTitleBar(
        IRenderInterface* pRI,
        const Int2& sPos,
        const Int2& sSize,
        const DWORD nBorderColor,
        const DWORD nBkgColor,
        const TCHAR* pName,
        const TCHAR* pComment )
    {
	    pRI->DrawTile(
            sPos,
            sSize,
            D3DXVECTOR2( 0.0f, 0.0f ),
            D3DXVECTOR2( 0.0f, 0.0f ),
            nBorderColor );

        pRI->DrawTile(
            Int2( sPos.x + 1, sPos.y + 1 ),
            Int2( sSize.x - 2, sSize.y - 2 ),
            D3DXVECTOR2( 0.0f, 0.0f ),
            D3DXVECTOR2( 0.0f, 0.0f ),
            nBkgColor );

	    SIZE sTextSize = { 0, 0 };
        pRI->GetTextExtent(
            sTextSize,
            pName,
            pRI->DefaultFontName(),
            pRI->DefaultFontSize(),
            pRI->DefaultFontZoom() );

        pRI->DrawText(
            FLOAT( sPos.x + ( ( sSize.x - sTextSize.cx ) / 2 ) ),
            FLOAT( sPos.y + ( ( sSize.y - sTextSize.cy ) / 2 ) + 1 ),
            pName,
            D3DCOLOR_XRGB( 255, 255, 128 ),
            pRI->DefaultFontName(),
            pRI->DefaultFontSize(),
            pRI->DefaultFontZoom() );

	    if( pComment )
	    {
		    pRI->GetTextExtent(
                sTextSize,
                pComment,
                pRI->DefaultFontName(),
                pRI->DefaultFontSize(),
                pRI->DefaultFontFlags() );

            if( sTextSize.cx > 0 && sTextSize.cy > 0 )
            {
                FLOAT fX = FLOAT( sPos.x ) + 2.0f;
		        FLOAT fY = FLOAT( sPos.y - sTextSize.cy ) - 2.0f;

                pRI->DrawText(
                    fX,
                    fY,
                    pComment,
                    D3DCOLOR_XRGB( 64, 64, 192 ),
                    pRI->DefaultFontName(),
                    pRI->DefaultFontSize(),
                    pRI->DefaultFontFlags() );
            }
	    }
    }

    Int2 CLinkedObjDrawUtils::GetLogicConnectorsSize(
        IRenderInterface* pRI,
        const SLinkedObjDrawInfo& sObjInfo,
        INT* pInputY,
        INT* pOutputY )
    {
        INT nMaxInputDescX = 0;
	    INT nMaxInputDescY = 0;

        size_t nSize = sObjInfo.vecInputs.size();
        for( size_t i=0; i<nSize; ++i )
	    {
		    SIZE sTextSize = { 0, 0 };
		    pRI->GetTextExtent(
                sTextSize,
                sObjInfo.vecInputs[ i ].pName,
                pRI->DefaultFontName(),
                pRI->DefaultFontSize(),
                pRI->DefaultFontZoom() );

		    nMaxInputDescX = max( sTextSize.cx, nMaxInputDescX );

		    if( i > 0 )
                nMaxInputDescY += DescPaddingY;
		    nMaxInputDescY += max( sTextSize.cy, ConnectorWidth );
	    }

	    INT nMaxOutputDescX = 0;
	    INT nMaxOutputDescY = 0;

        nSize = sObjInfo.vecOutputs.size();
        for( size_t i=0; i<nSize; ++i )
	    {
		    SIZE sTextSize = { 0, 0 };
		    pRI->GetTextExtent(
                sTextSize,
                sObjInfo.vecOutputs[ i ].pName,
                pRI->DefaultFontName(),
                pRI->DefaultFontSize(),
                pRI->DefaultFontZoom() );

		    nMaxOutputDescX = max( sTextSize.cx, nMaxOutputDescX );

		    if( i > 0 )
                nMaxOutputDescY += DescPaddingY;
		    nMaxOutputDescY += max( sTextSize.cy, ConnectorWidth );
	    }

	    INT nNeededX = nMaxInputDescX + nMaxOutputDescX + DescPaddingX + ( TextBorder * 2 );
	    INT nNeededY = max( nMaxInputDescY, nMaxOutputDescY ) + ( TextBorder * 2 );

	    if( pInputY )
            *pInputY = nMaxInputDescY + ( TextBorder * 2 );

	    if( pOutputY )
            *pOutputY = nMaxOutputDescY + ( TextBorder * 2 );

	    return Int2( nNeededX, nNeededY );
    }

    VOID CLinkedObjDrawUtils::DrawLogicConnectors(
        IRenderInterface* pRI,
        SLinkedObjDrawInfo& sObjInfo,
        const Int2& sPos,
        const Int2& sSize )
    {
	    SIZE sTextSize = { 0, 0 };
	    pRI->GetTextExtent(
            sTextSize,
            _T( "?????" ),
            pRI->DefaultFontName(),
            pRI->DefaultFontSize(),
            pRI->DefaultFontZoom() );

	    INT nConnectorRangeY = sSize.y - TextBorder * 2;
	    INT nCenterY = sPos.y + TextBorder + INT( nConnectorRangeY * 0.5f );

	    if( !sObjInfo.vecInputs.empty() )
	    {
            size_t nSize = sObjInfo.vecInputs.size();

		    INT nSpacingY = nConnectorRangeY / INT( nSize );
		    INT nStartY = nCenterY - INT( INT( nSize - 1 ) * nSpacingY * 0.5f );

            for( size_t i=0; i<nSize; ++i )
		    {
			    INT nLinkY = nStartY + INT( i ) * nSpacingY;
                sObjInfo.vecInputY.push_back( nLinkY );

                Int2 sBegin;
                sBegin.x = sPos.x - ConnectorLength;
                sBegin.y = nLinkY - INT( ConnectorWidth * 0.5f );

                if( pRI->IsHitTesting() )
                    pRI->SetHitProxy( new SLinkedObjConnectorProxy( sObjInfo.pObject, EInput, INT( i ) ) );

			    pRI->DrawTile(
                    sBegin,
                    Int2( ConnectorLength, ConnectorWidth ),
                    D3DXVECTOR2( 0.0f, 0.0f ),
                    D3DXVECTOR2( 0.0f, 0.0f ),
                    sObjInfo.vecInputs[ i ].nColor );

                if( pRI->IsHitTesting() )
                    pRI->SetHitProxy( NULL );

			    pRI->GetTextExtent(
                    sTextSize,
                    sObjInfo.vecInputs[ i ].pName,
                    pRI->DefaultFontName(),
                    pRI->DefaultFontSize(),
                    pRI->DefaultFontZoom() );

                pRI->DrawText(
                    FLOAT( sPos.x + TextBorder ),
                    FLOAT( nLinkY - INT( sTextSize.cy * 0.5f ) ),
                    sObjInfo.vecInputs[ i ].pName,
                    D3DCOLOR_XRGB( 255, 255, 255 ),
                    pRI->DefaultFontName(),
                    pRI->DefaultFontSize(),
                    pRI->DefaultFontZoom() );

                if( sObjInfo.vecInputs[ i ].fDelay > 0.0f )
                {
                    CString strText;
                    strText.Format( _T( "Delay %.2f" ), sObjInfo.vecInputs[ i ].fDelay );

                    pRI->GetTextExtent(
                        sTextSize,
                        strText.GetString(),
                        pRI->DefaultFontName(),
                        pRI->DefaultFontSize(),
                        pRI->DefaultFontFlags() );

                    sBegin.x -= sTextSize.cx;
                    sBegin.y -= sTextSize.cy;

                    pRI->DrawText(
                        FLOAT( sBegin.x ),
                        FLOAT( sBegin.y ),
                        strText.GetString(),
                        D3DCOLOR_XRGB( 255, 255, 255 ),
                        pRI->DefaultFontName(),
                        pRI->DefaultFontSize(),
                        pRI->DefaultFontFlags() );
                }
		    }
	    }

	    if( !sObjInfo.vecOutputs.empty() )
	    {
            size_t nSize = sObjInfo.vecOutputs.size();

		    INT nSpacingY = nConnectorRangeY / INT( nSize );
		    INT nStartY = nCenterY - INT( INT( nSize - 1 ) * nSpacingY * 0.5f );

		    for( size_t i=0; i<nSize; ++i )
		    {
			    INT nLinkY = nStartY + INT( i ) * nSpacingY;
                sObjInfo.vecOutputY.push_back( nLinkY );

                Int2 sBegin;
                sBegin.x = sPos.x + sSize.x;
                sBegin.y = nLinkY - INT( ConnectorWidth * 0.5f );

                if( pRI->IsHitTesting() )
                    pRI->SetHitProxy( new SLinkedObjConnectorProxy( sObjInfo.pObject, EOutput, INT( i ) ) );

			    pRI->DrawTile(
                    sBegin,
                    Int2( ConnectorLength, ConnectorWidth ),
                    D3DXVECTOR2( 0.0f, 0.0f ),
                    D3DXVECTOR2( 0.0f, 0.0f ),
                    sObjInfo.vecOutputs[ i ].nColor );

                if( pRI->IsHitTesting() )
                    pRI->SetHitProxy( NULL );

			    pRI->GetTextExtent(
                    sTextSize,
                    sObjInfo.vecOutputs[ i ].pName,
                    pRI->DefaultFontName(),
                    pRI->DefaultFontSize(),
                    pRI->DefaultFontZoom() );

                pRI->DrawText(
                    FLOAT( sPos.x + sSize.x - sTextSize.cx - TextBorder ),
                    FLOAT( nLinkY - INT( sTextSize.cy * 0.5f ) ),
                    sObjInfo.vecOutputs[ i ].pName,
                    D3DCOLOR_XRGB( 255, 255, 255 ),
                    pRI->DefaultFontName(),
                    pRI->DefaultFontSize(),
                    pRI->DefaultFontZoom() );

                if( sObjInfo.vecOutputs[ i ].fDelay > 0.0f )
                {
                    CString strText;
                    strText.Format( _T( "Delay %.2f" ), sObjInfo.vecOutputs[ i ].fDelay );

                    pRI->GetTextExtent(
                        sTextSize,
                        strText.GetString(),
                        pRI->DefaultFontName(),
                        pRI->DefaultFontSize(),
                        pRI->DefaultFontFlags() );

                    sBegin.x += ConnectorLength;
                    sBegin.y += ConnectorWidth;

                    pRI->DrawText(
                        FLOAT( sBegin.x ),
                        FLOAT( sBegin.y ),
                        strText.GetString(),
                        D3DCOLOR_XRGB( 255, 255, 255 ),
                        pRI->DefaultFontName(),
                        pRI->DefaultFontSize(),
                        pRI->DefaultFontFlags() );
                }
		    }
	    }
    }

    Int2 CLinkedObjDrawUtils::GetVariableConnectorsSize(
        IRenderInterface* pRI,
        const SLinkedObjDrawInfo& sObjInfo )
    {
        INT nMaxVarX = 0;
	    INT nMaxVarY = 0;

        size_t nSize = sObjInfo.vecVariables.size();
        for( size_t i=0; i<nSize; ++i )
	    {
		    SIZE sTextSize = { 0, 0 };
		    pRI->GetTextExtent(
                sTextSize,
                sObjInfo.vecVariables[ i ].pName,
                pRI->DefaultFontName(),
                pRI->DefaultFontSize(),
                pRI->DefaultFontZoom() );

		    nMaxVarX = max( INT( sTextSize.cx ), nMaxVarX );
		    nMaxVarY = max( INT( sTextSize.cy ), nMaxVarY );
	    }

	    INT nNeededX = ( ( nMaxVarX + DescPaddingX ) * INT( sObjInfo.vecVariables.size() ) ) + ( TextBorder * 2 );
	    INT nNeededY = nMaxVarY + TextBorder;

	    return Int2( nNeededX, nNeededY );
    }

    VOID CLinkedObjDrawUtils::DrawVariableConnectors(
        IRenderInterface* pRI,
        SLinkedObjDrawInfo& sObjInfo,
        const Int2& sPos,
        const Int2& sSize )
    {
	    if( sObjInfo.vecVariables.empty() )
		    return;

	    INT nMaxVarX = 0;
	    INT nMaxVarY = 0;

        size_t nSize = sObjInfo.vecVariables.size();
        for( size_t i=0; i<nSize; ++i )
	    {
		    SIZE sTextSize = { 0, 0 };
		    pRI->GetTextExtent(
                sTextSize,
                sObjInfo.vecVariables[ i ].pName,
                pRI->DefaultFontName(),
                pRI->DefaultFontSize(),
                pRI->DefaultFontZoom() );

		    nMaxVarX = max( INT( sTextSize.cx ), nMaxVarX );
		    nMaxVarY = max( INT( sTextSize.cy ), nMaxVarY );
	    }

        nMaxVarX = max( nMaxVarX, ConnectorWidth );

	    INT nVarWidth = nMaxVarX + DescPaddingX;
	    INT nVarRangeX = sSize.x - ( TextBorder * 2 );
	    INT nCenterX = sPos.x + TextBorder + INT( nVarRangeX * 0.5f );

	    INT nSpacingX = nVarRangeX / INT( sObjInfo.vecVariables.size() );
	    INT nStartX = nCenterX - INT( ( sObjInfo.vecVariables.size() - 1 ) * nSpacingX * 0.5f );
	    INT nLabelY = sPos.y + sSize.y - TextBorder - nMaxVarY;

        nSize = sObjInfo.vecVariables.size();
	    for( size_t i=0; i<nSize; ++i )
	    {
		    INT nVarX = nStartX + INT( i ) * nSpacingX;
            sObjInfo.vecVariableX.push_back( nVarX );

            if( pRI->IsHitTesting() )
                pRI->SetHitProxy( new SLinkedObjConnectorProxy( sObjInfo.pObject, EVariable, INT( i ) ) );

            if( sObjInfo.vecVariables[ i ].bWriteable )
            {
                DrawArrowhead(
                    pRI,
                    Int2( nVarX, sPos.y + sSize.y + INT( ArrowheadLength ) ),
                    D3DXVECTOR2( 0.0f, 1.0f ),
                    sObjInfo.vecVariables[ i ].nColor,
                    TRUE );
            }
            else
            {
                pRI->DrawTile(
                    Int2( nVarX - INT( ConnectorWidth * 0.5f ), sPos.y + sSize.y ),
                    Int2( ConnectorWidth, ConnectorLength ),
                    D3DXVECTOR2( 0.0f, 0.0f ),
                    D3DXVECTOR2( 0.0f, 0.0f ),
                    sObjInfo.vecVariables[ i ].nColor );
            }

            if( pRI->IsHitTesting() )
                pRI->SetHitProxy( NULL );

		    SIZE sTextSize = { 0, 0 };
		    pRI->GetTextExtent(
                sTextSize,
                sObjInfo.vecVariables[ i ].pName,
                pRI->DefaultFontName(),
                pRI->DefaultFontSize(),
                pRI->DefaultFontZoom() );

            pRI->DrawText(
                FLOAT( nVarX - sTextSize.cx * 0.5f ),
                FLOAT( nLabelY ),
                sObjInfo.vecVariables[ i ].pName,
                D3DCOLOR_XRGB( 255, 255, 255 ),
                pRI->DefaultFontName(),
                pRI->DefaultFontSize(),
                pRI->DefaultFontZoom() );
	    }
    }

    VOID CLinkedObjDrawUtils::DrawLinkedObj(
        IRenderInterface* pRI,
        SLinkedObjDrawInfo& sObjInfo,
        const TCHAR* pName,
        const TCHAR* pComment,
        const DWORD nBorderColor,
        const DWORD nTitleBkgColor,
        const Int2& sPos )
    {
	    Int2 sTitleSize = GetTitleBarSize( pRI, pName );
	    Int2 sLogicSize = GetLogicConnectorsSize( pRI, sObjInfo );
	    Int2 sVarSize = GetVariableConnectorsSize( pRI, sObjInfo );

	    sObjInfo.sDrawSize.x = max( max( sTitleSize.x, sLogicSize.x ), sVarSize.x );
	    sObjInfo.sDrawSize.y = sTitleSize.y + sLogicSize.y + sVarSize.y + 3;

        if( pRI->IsHitTesting() )
            pRI->SetHitProxy( new SLinkedObjProxy( sObjInfo.pObject ) );

	    DrawTitleBar(
            pRI,
            sPos,
            Int2( sObjInfo.sDrawSize.x, sTitleSize.y ),
            nBorderColor,
            nTitleBkgColor,
            pName,
            pComment );

	    pRI->DrawTile(
            Int2( sPos.x, sPos.y + sTitleSize.y + 1 ),
            Int2( sObjInfo.sDrawSize.x, sLogicSize.y + sVarSize.y ),
            D3DXVECTOR2( 0.0f, 0.0f ),
            D3DXVECTOR2( 0.0f, 0.0f ),
            nBorderColor );

        pRI->DrawTile(
            Int2( sPos.x + 1, sPos.y + sTitleSize.y + 2 ),
            Int2( sObjInfo.sDrawSize.x - 2, sLogicSize.y + sVarSize.y - 2 ),
            D3DXVECTOR2( 0.0f, 0.0f ),
            D3DXVECTOR2( 0.0f, 0.0f ),
            D3DCOLOR_XRGB( 140, 140, 140 ) );

        if( pRI->IsHitTesting() )
            pRI->SetHitProxy( NULL );

	    DrawLogicConnectors(
            pRI,
            sObjInfo,
            Int2( sPos.x, sPos.y + sTitleSize.y + 1 ),
            Int2( sObjInfo.sDrawSize.x, sLogicSize.y ) );

        DrawVariableConnectors(
            pRI,
            sObjInfo,
            Int2( sPos.x, sPos.y + sTitleSize.y + sLogicSize.y + 1 ),
            Int2( sObjInfo.sDrawSize.x, sVarSize.y ) );
    }
}