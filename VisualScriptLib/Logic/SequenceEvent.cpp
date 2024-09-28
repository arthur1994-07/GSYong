#include "stdafx.h"

#include <boost/bind.hpp>
#include <boost/mem_fn.hpp>

#include "../../InternalCommonLib/Interface/RenderInterface.h"

#include "./SequenceVariable.h"
#include "./Sequence.h"

#include "./SequenceEvent.h"

#include "../../InternalCommonLib/DebugInclude.h"

namespace vs
{
    CSequenceEvent::CSequenceEvent()
        : bEnabled( TRUE )
        , nTriggerCount( 0 )
        , nMaxTriggerCount( 1 )
        , fReTriggerDelay( 0.0f )
        , fActivationTime( 0.0f )
        , nMaxWidth( 0 )
        , bActive( FALSE )
    {
        strName = _T( "Event" );
        nColor = D3DCOLOR_XRGB( 255, 0, 0 );
    }

    VOID CSequenceEvent::RegistProperty()
    {
        Super::RegistProperty();

        {
            ic::CPropertyBool* pProperty = ic::CPropertyFactory::New< ic::CPropertyBool >();
            pProperty->SetPtr( &bEnabled );
            InsertProperty( _T( "Enabled" ), pProperty );
        }
        {
            ic::CPropertyInt* pProperty = ic::CPropertyFactory::New< ic::CPropertyInt >();
            pProperty->SetPtr( &nMaxTriggerCount );
            InsertProperty( _T( "Max Trigger Count" ), pProperty );
        }
        {
            ic::CPropertyFloat* pProperty = ic::CPropertyFactory::New< ic::CPropertyFloat >();
            pProperty->SetPtr( &fReTriggerDelay );
            InsertProperty( _T( "Re Trigger Delay" ), pProperty );
        }
    }

    VOID CSequenceEvent::ProcessInput()
    {
        if( bActive )
        {
            bActive = FALSE;
            DeActivated();
        }
    }

    VOID CSequenceEvent::InsertInputImpulse( size_t nIndex )
    {
        if( bEnabled )
            bActive = TRUE;
    }

    VOID CSequenceEvent::DrawObject(
        ic::IRenderInterface* pRI,
        BOOL bSelected,
        BOOL bMouseOver,
        INT nMouseOverConnType,
        INT nMouseOverConnIndex )
    {
        ic::Int2 sTitleSize = GetTitleBarSize( pRI );
        ic::Int2 sLogicSize = GetLogicConnectorsSize( pRI );
	    ic::Int2 sVarSize = GetVariableConnectorsSize( pRI );

	    nMaxWidth = max( max( sTitleSize.x, sLogicSize.x ), sVarSize.x );

        if( pRI->IsHitTesting() )
            pRI->SetHitProxy( new ic::SLinkedObjProxy( this ) );

	    DrawTitleBar(
            pRI,
            bSelected,
            bMouseOver,
            sPos,
            ic::Int2( nMaxWidth, sTitleSize.y ) );

        ic::Int2 sPosDraw;
        sPosDraw.x = sPos.x + INT( nMaxWidth * 0.5f ) - INT( sLogicSize.x * 0.5f );
        sPosDraw.y = sPos.y + sTitleSize.y + 1;

        ic::CLinkedObjDrawUtils::DrawDiamond(
            pRI,
            sPosDraw,
            sLogicSize,
            GetBorderColor( bSelected, bMouseOver ) );

        ic::CLinkedObjDrawUtils::DrawDiamond(
            pRI,
            ic::Int2( sPosDraw.x + 1, sPosDraw.y + 1 ),
            ic::Int2( sLogicSize.x - 2, sLogicSize.y - 2 ),
            D3DCOLOR_XRGB( 140, 140, 140 ) );

        sPosDraw.y += INT( ic::DiamondSize * 0.5f );

        ic::Int2 sLogicCenter;
        sLogicCenter.x = sPosDraw.x + INT( sLogicSize.x * 0.5f );
        sLogicCenter.y = sPosDraw.y + INT( sLogicSize.y * 0.5f );

        DrawExtraInfo( pRI, sLogicCenter );

        if( pRI->IsHitTesting() )
            pRI->SetHitProxy( NULL );

        DrawLogicConnectors(
            pRI,
            ic::Int2( sPosDraw.x, sPosDraw.y ),
            ic::Int2( sLogicSize.x, sLogicSize.y ),
            bMouseOver ? nMouseOverConnType : INT_MIN,
            bMouseOver ? nMouseOverConnIndex : INT_MIN );

	    if( !vecVariableLinks.empty() )
	    {
            sPosDraw.y += sLogicSize.y + INT( ic::DiamondSize * 0.5f ) + 1;

		    pRI->DrawTile(
                ic::Int2( sPos.x, sPosDraw.y ),
                ic::Int2( nMaxWidth, sVarSize.y ),
                D3DXVECTOR2( 0.0f, 0.0f ),
                D3DXVECTOR2( 0.0f, 0.0f ),
                GetBorderColor( bSelected, bMouseOver ) );

            pRI->DrawTile(
                ic::Int2( sPos.x + 1, sPosDraw.y + 1 ),
                ic::Int2( nMaxWidth - 2, sVarSize.y - 2 ),
                D3DXVECTOR2( 0.0f, 0.0f ),
                D3DXVECTOR2( 0.0f, 0.0f ),
                D3DCOLOR_XRGB( 140, 140, 140 ) );

            DrawVariableConnectors(
                pRI,
                ic::Int2( sPos.x, sPosDraw.y ),
                ic::Int2( nMaxWidth, sVarSize.y ),
                bMouseOver ? nMouseOverConnType : INT_MIN,
                bMouseOver ? nMouseOverConnIndex : INT_MIN );
	    }

        sDrawSize.x = INT( nMaxWidth * 0.5f ) + INT( sLogicSize.x * 0.5f );
    }

    ic::Int4 CSequenceEvent::GetBoundingBox() const
    {
        return ic::Int4(
            sPos.x,
            sPos.y,
            sPos.x + nMaxWidth,
            sPos.y + sDrawSize.y );
    }

    ic::Int2 CSequenceEvent::GetConnectionLocation()
    {
        return ic::Int2( sPos.x + INT( nMaxWidth * 0.5f ), sPos.y );
    }

    CSeqEvent_LevelLoaded::CSeqEvent_LevelLoaded()
    {
        strName = _T( "Level Loaded" );
    }

    VOID CSeqEvent_LevelLoaded::PostNew()
    {
        Super::PostNew();

        // Output
        {
            SSeqOpOutputLink sPush;
            sPush.strLinkDesc = _T( "Out" );
            vecOutputLinks.push_back( sPush );
        }

        // Variable
        {
            vecVariableLinks.clear();
        }
    }

    CSeqEvent_SequenceActivated::CSeqEvent_SequenceActivated()
        : strLabel( _T( "In" ) )
    {
        strName = _T( "Sequence Activated" );
    }

    VOID CSeqEvent_SequenceActivated::PostNew()
    {
        Super::PostNew();

        // Output
        {
            SSeqOpOutputLink sPush;
            sPush.strLinkDesc = _T( "Out" );
            vecOutputLinks.push_back( sPush );
        }
    }

    VOID CSeqEvent_SequenceActivated::RegistProperty()
    {
        Super::RegistProperty();

        {
            ic::CPropertyString* pProperty = ic::CPropertyFactory::New< ic::CPropertyString >();
            pProperty->SetPtr( &strLabel );
            pProperty->SetCall( boost::bind( boost::mem_fn( &CSeqEvent_SequenceActivated::OnChangedLabel ), this ) );
            InsertProperty( _T( "Label" ), pProperty );
        }
    }

    TSTRING CSeqEvent_SequenceActivated::GetObjectName()
    {
        CString strName_;
        strName_.Format( _T( "%s \"%s\"" ), strName.c_str(), strLabel.c_str() );

        return strName_.GetString();
    }

    VOID CSeqEvent_SequenceActivated::OnCreated()
    {
        OnChangedLabel();
    }

    VOID CSeqEvent_SequenceActivated::OnChangedLabel()
    {
        CSequence* pSequence = dynamic_cast< CSequence* >( GetParent() );
        if( pSequence )
            pSequence->UpdateConnectors();
    }

    CSeqEvent_Touch::CSeqEvent_Touch()
    {
        strName = _T( "Touch" );
    }

    VOID CSeqEvent_Touch::PostNew()
    {
        Super::PostNew();

        // Output
        {
            SSeqOpOutputLink sPush;
            sPush.strLinkDesc = _T( "Touched" );
            vecOutputLinks.push_back( sPush );

            sPush = SSeqOpOutputLink();
            sPush.strLinkDesc = _T( "UnTouched" );
            vecOutputLinks.push_back( sPush );

            sPush = SSeqOpOutputLink();
            sPush.strLinkDesc = _T( "Empty" );
            vecOutputLinks.push_back( sPush );
        }
    }

    CSeqEvent_RemoteEvent::CSeqEvent_RemoteEvent()
        : strLabel( _T( "Untitled" ) )
        , bStatusIsOk( FALSE )
    {
        strName = _T( "Remote Event" );
    }

    VOID CSeqEvent_RemoteEvent::PostNew()
    {
        Super::PostNew();

        // Output
        {
            SSeqOpOutputLink sPush;
            sPush.strLinkDesc = _T( "Out" );
            vecOutputLinks.push_back( sPush );
        }
    }

    VOID CSeqEvent_RemoteEvent::RegistProperty()
    {
        Super::RegistProperty();

        {
            ic::CPropertyString* pProperty = ic::CPropertyFactory::New< ic::CPropertyString >();
            pProperty->SetPtr( &strLabel );
            pProperty->SetCall( boost::bind( boost::mem_fn( &CSeqEvent_RemoteEvent::OnChangedLabel ), this ) );
            InsertProperty( _T( "Label" ), pProperty );
        }
    }

    TSTRING CSeqEvent_RemoteEvent::GetObjectName()
    {
        CString strName_;
        strName_.Format( _T( "%s \"%s\"" ), strName.c_str(), strLabel.c_str() );

        return strName_.GetString();
    }

    VOID CSeqEvent_RemoteEvent::PostLoad()
    {
        OnChangedLabel();
    }

    VOID CSeqEvent_RemoteEvent::OnCreated()
    {
        OnChangedLabel();

        CSequence* pRoot = GetRootSequence();
        __gassert_( pRoot );

        pRoot->UpdateRemoteEventStatus();
        pRoot->UpdateActivateRemoteEventStatus();
    }

    VOID CSeqEvent_RemoteEvent::DrawExtraInfo( ic::IRenderInterface* pRI, const ic::Int2& sCenter )
    {
        DrawStatusMark( pRI, sCenter, bStatusIsOk );
    }

    VOID CSeqEvent_RemoteEvent::OnChangedLabel()
    {
        bStatusIsOk = FALSE;

        if( strLabel.empty() )
            return;

        CSequence* pRoot = GetRootSequence();
        __gassert_( pRoot );

        SequenceObjectVec vecObjects;
        pRoot->FindActivateRemoteEvent( strLabel, vecObjects );

        if( !vecObjects.empty() )
            bStatusIsOk = TRUE;
    }
}