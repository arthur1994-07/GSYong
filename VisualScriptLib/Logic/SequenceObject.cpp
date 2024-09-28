#include "stdafx.h"

#include "../../InternalCommonLib/Interface/RenderInterface.h"
#include "../../InternalCommonLib/Interface/TextureManagerInterface.h"

#include "./Sequence.h"
#include "./SequenceObject.h"

#include "../../InternalCommonLib/DebugInclude.h"

namespace vs
{
    const TSTRING TickMaterial = _T( "TickMaterial.png" );
    const TSTRING CrossMaterial = _T( "CrossMaterial.png" );
    const DWORD SelectedColor = D3DCOLOR_XRGB( 255, 255, 0 );
    const DWORD MouseOverLogicColor = D3DCOLOR_XRGB( 255, 255, 0 );
    const FLOAT MouseOverColorScale = 0.77f;
    const DWORD TitleBackColor = D3DCOLOR_XRGB( 112, 112, 112 );
    const DWORD SequenceTitleBackColor = D3DCOLOR_XRGB( 112, 112, 200 );
    const FLOAT SequenceImpulseEnable = 0.0f;
    const FLOAT SequenceImpulseDisable = -1.0f;

    CSequenceObject::CSequenceObject()
        : nColor( D3DCOLOR_XRGB( 255, 255, 255 ) )
        , bDrawFirst( FALSE )
        , bDrawLast( FALSE )
    {
        strName = _T( "Object" );
    }

    VOID CSequenceObject::RegistProperty()
    {
        Super::RegistProperty();

        {
            ic::CPropertyString* pProperty = ic::CPropertyFactory::New< ic::CPropertyString >();
            pProperty->SetPtr( &strComment );
            InsertProperty( _T( "Comment" ), pProperty );
        }
    }

    TSTRING CSequenceObject::GetFullName()
    {
        TSTRING strReturn = GetClassName();

        CSequence* pParent = dynamic_cast< CSequence* >( GetParent() );
        while( pParent )
        {
            TSTRING strParent = pParent->GetClassName();
            strParent.append( _T( "." ) );
            strParent.append( strReturn );
            strReturn = strParent;
            pParent = dynamic_cast< CSequence* >( pParent->GetParent() );
        }

        return strReturn;
    }

    CSequence* CSequenceObject::GetRootSequence()
    {
        CSequence* pRoot = dynamic_cast< CSequence* >( this );

        CSequence* pParent = dynamic_cast< CSequence* >( GetParent() );
        while( pParent )
        {
            pRoot = pParent;
            pParent = dynamic_cast< CSequence* >( pParent->GetParent() );
        }

        return pRoot;
    }

    ic::Int2 CSequenceObject::GetTitleBarSize( ic::IRenderInterface* pRI )
    {
        SIZE sTextSize = { 0, 0 };
        pRI->GetTextExtent(
            sTextSize,
            GetObjectName().c_str(),
            pRI->DefaultFontName(),
            pRI->DefaultFontSize(),
            pRI->DefaultFontZoom() );

        return ic::Int2( INT( sTextSize.cx ), INT( sTextSize.cy ) );
    }

    DWORD CSequenceObject::GetBorderColor( BOOL bSelected, BOOL bMouseOver )
    {
        if( bSelected )
            return SelectedColor;

        if( bMouseOver )
            return nColor;

        D3DXCOLOR sColor( nColor );
        sColor *= MouseOverColorScale;

        return sColor;
    }

    VOID CSequenceObject::DrawTitleBar(
        ic::IRenderInterface* pRI,
        BOOL bSelected,
        BOOL bMouseOver,
        const ic::Int2& sPos_,
        const ic::Int2& sSize )
    {
        DWORD nBackColor = TitleBackColor;
        if( IsA( CSequence::Rtti() ) )
            nBackColor = SequenceTitleBackColor;

        ic::CLinkedObjDrawUtils::DrawTitleBar(
            pRI,
            sPos_,
            sSize,
            GetBorderColor( bSelected, bMouseOver ),
            nBackColor,
            GetObjectName().c_str(),
            strComment.c_str() );
    }

    VOID CSequenceObject::DrawStatusMark(
        ic::IRenderInterface* pRI,
        const ic::Int2& sCenter,
        BOOL bStatusIsOk )
    {
        ic::TextureResourceInterface textureRes = ic::TextureManagerInterface::GetInstance()->LoadTexture(
            bStatusIsOk ? TickMaterial : CrossMaterial,
            false,
            TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
            0,
            D3DFMT_UNKNOWN,
            TEXTURE_RESIZE_NONE,
            TEXTURE_ETC,
            false,
			FALSE );

        if( !textureRes.GetTexture() )
            return;

        pRI->DrawTile(
            ic::Int2( sCenter.x - 16, sCenter.y - 16 ),
            ic::Int2( 32, 32 ),
            D3DXVECTOR2( 0.f, 0.f ),
            D3DXVECTOR2( 1.f, 1.f ),
            D3DCOLOR_XRGB( 255, 255, 255 ),
            textureRes.GetTexture() );
        pRI->FlushBatchedPrimitives();
    }

    ic::Int4 CSequenceObject::GetBoundingBox() const
    {
        return ic::Int4(
            sPos.x,
            sPos.y,
            sPos.x + sDrawSize.x,
            sPos.y + sDrawSize.y );
    }
}