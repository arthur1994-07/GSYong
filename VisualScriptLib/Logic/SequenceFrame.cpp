#include "stdafx.h"

#include "../../InternalCommonLib/Interface/RenderInterface.h"

#include "./SequenceFrame.h"

#include "../../InternalCommonLib/DebugInclude.h"

namespace vs
{
    CSequenceFrame::CSequenceFrame()
        : sSize( 128, 64 )
        , nBorderWidth( 1 )
        , bDrawBox( FALSE )
        , bFilled( TRUE )
        , bTileFill( FALSE )
        , nBorderColor( D3DCOLOR_XRGB( 0, 0, 0 ) )
        , nFillColor( D3DCOLOR_ARGB( 16, 255, 255, 255 ) )
    {
        strName = _T( "Comment" );
        strComment = _T( "Comment" );
        bDrawFirst = TRUE;
    }

    VOID CSequenceFrame::RegistProperty()
    {
        Super::RegistProperty();

        {
            ic::CPropertyInt* pProperty = ic::CPropertyFactory::New< ic::CPropertyInt >();
            pProperty->SetPtr( &sSize.x );
            InsertProperty( _T( "Size X" ), pProperty );
        }
        {
            ic::CPropertyInt* pProperty = ic::CPropertyFactory::New< ic::CPropertyInt >();
            pProperty->SetPtr( &sSize.y );
            InsertProperty( _T( "Size Y" ), pProperty );
        }
        {
            ic::CPropertyInt* pProperty = ic::CPropertyFactory::New< ic::CPropertyInt >();
            pProperty->SetPtr( &nBorderWidth );
            InsertProperty( _T( "Border Width" ), pProperty );
        }
        {
            ic::CPropertyBool* pProperty = ic::CPropertyFactory::New< ic::CPropertyBool >();
            pProperty->SetPtr( &bDrawBox );
            InsertProperty( _T( "Draw Box" ), pProperty );
        }
        {
            ic::CPropertyBool* pProperty = ic::CPropertyFactory::New< ic::CPropertyBool >();
            pProperty->SetPtr( &bFilled );
            InsertProperty( _T( "Filled" ), pProperty );
        }
        {
            ic::CPropertyBool* pProperty = ic::CPropertyFactory::New< ic::CPropertyBool >();
            pProperty->SetPtr( &bTileFill );
            InsertProperty( _T( "Tile Fill" ), pProperty );
        }
        {
            ic::CPropertyColor* pProperty = ic::CPropertyFactory::New< ic::CPropertyColor >();
            pProperty->SetPtr( &nBorderColor );
            InsertProperty( _T( "Border Color" ), pProperty );
        }
        {
            ic::CPropertyColor* pProperty = ic::CPropertyFactory::New< ic::CPropertyColor >();
            pProperty->SetPtr( &nFillColor );
            InsertProperty( _T( "Fill Color" ), pProperty );
        }
    }

    VOID CSequenceFrame::DrawObject(
        ic::IRenderInterface* pRI,
        BOOL bSelected,
        BOOL bMouseOver,
        INT nMouseOverConnType,
        INT nMouseOverConnIndex )
    {
        if( bDrawBox )
        {
            if( bFilled )
            {
                pRI->DrawTile(
                    sPos,
                    sSize,
                    D3DXVECTOR2( 0.0f, 0.0f ),
                    D3DXVECTOR2( 1.0f, 1.0f ),
                    nFillColor );
            }

            DWORD nFrameColor = bSelected ? SelectedColor : nBorderColor;

            INT nUseBoarderWidth = max( nBorderWidth, 0 );
            nUseBoarderWidth = min( nUseBoarderWidth, ( min( sSize.x, sSize.y ) / 2 ) - 3 );

            for( INT i=0; i<nUseBoarderWidth; ++i )
            {
                pRI->DrawLine(
                    ic::Int2( sPos.x, sPos.y + i ),
                    ic::Int2( sPos.x + sSize.x, sPos.y + i ),
                    nFrameColor );

                pRI->DrawLine(
                    ic::Int2( sPos.x + sSize.x - i, sPos.y ),
                    ic::Int2( sPos.x + sSize.x - i, sPos.y + sSize.y ),
                    nFrameColor );

                pRI->DrawLine(
                    ic::Int2( sPos.x + sSize.x, sPos.y + sSize.y - i ),
                    ic::Int2( sPos.x, sPos.y + sSize.y - i ),
                    nFrameColor );

                pRI->DrawLine(
                    ic::Int2( sPos.x + i, sPos.y + sSize.y ),
                    ic::Int2( sPos.x + i, sPos.y - 1 ),
                    nFrameColor );
            }

            // Handle
            {
                INT HandleSize = bSelected ? 17 : 15;

                if( pRI->IsHitTesting() )
                    pRI->SetHitProxy( new ic::SLinkedObjProxySpecial( this ) );

                pRI->DrawTriangle(
                    ic::Int2( sPos.x + sSize.x, sPos.y + sSize.y ),
                    D3DXVECTOR2( 0.0f, 0.0f ),
                    ic::Int2( sPos.x + sSize.x - HandleSize, sPos.y + sSize.y ),
                    D3DXVECTOR2( 0.0f, 0.0f ),
                    ic::Int2( sPos.x + sSize.x, sPos.y + sSize.y - HandleSize ),
                    D3DXVECTOR2( 0.0f, 0.0f ),
                    bSelected ? D3DCOLOR_XRGB( 0, 0, 0 ) : D3DCOLOR_ARGB( 100, 0, 0, 0 ) );

                if( pRI->IsHitTesting() )
                    pRI->SetHitProxy( NULL );
            }
        }

        if( strComment.empty() )
            strComment = _T( "Comment" );

        SIZE sTextSize = { 0, 0 };
        pRI->GetTextExtent(
            sTextSize,
            strComment,
            pRI->DefaultFontName(),
            pRI->DefaultFontSize(),
            pRI->DefaultFontFlags() );

        FLOAT fZoom = pRI->GetZoom();
        pRI->SetZoom( 1.0f );

        if( pRI->IsHitTesting() )
            pRI->SetHitProxy( new ic::SLinkedObjProxy( this ) );

        pRI->DrawText(
            FLOAT( sPos.x ) * fZoom,
            FLOAT( sPos.y ) * fZoom - sTextSize.cy - 2,
            strComment,
            D3DCOLOR_XRGB( 64, 64, 192 ),
            pRI->DefaultFontName(),
            pRI->DefaultFontSize(),
            pRI->DefaultFontFlags() );

        if( pRI->IsHitTesting() )
            pRI->SetHitProxy( NULL );

        pRI->SetZoom( fZoom );

        sDrawSize = sSize;
    }
}