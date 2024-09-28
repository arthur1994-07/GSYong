#include "stdafx.h"

#include "../../InternalCommonLib/Interface/RenderInterface.h"

#include "./SequenceEvent.h"
#include "./SequenceVariable.h"
#include "./SequenceOp.h"

#include "../../InternalCommonLib/DebugInclude.h"

namespace vs
{
    CSequenceOp::CSequenceOp()
    {
        strName = _T( "Op" );
    }

    VOID CSequenceOp::CheckForErrors()
    {
        BOOST_FOREACH( SSeqOpOutputLink& sOutputLink, vecOutputLinks )
        {
            SeqOutputInputVecIter it = sOutputLink.vecLinks.begin();
            while( it != sOutputLink.vecLinks.end() )
            {
                if( ( *it ).pLinkedOp == NULL )
                    it = sOutputLink.vecLinks.erase( it );
                else
                    ++it;
            }
        }

        BOOST_FOREACH( SSeqVarLink& sVarLink, vecVariableLinks )
        {
            SeqVariableVecIter it = sVarLink.vecLinks.begin();
            while( it != sVarLink.vecLinks.end() )
            {
                if( ( *it ) == NULL )
                    it = sVarLink.vecLinks.erase( it );
                else
                    ++it;
            }
        }
    }

    VOID CSequenceOp::ProcessInput()
    {
        INT nIndex = 0;

        BOOST_FOREACH( SSeqOpInputLink& sLink, vecInputLinks )
        {
            size_t nSizeImpulse = sLink.vecImpulse.size();
            for( size_t j=0; j<nSizeImpulse; ++j )
            {
                if( sLink.vecImpulse[ j ] != SequenceImpulseEnable )
                    continue;

                sLink.vecImpulse[ j ] = SequenceImpulseDisable;

                Activated( nIndex );
            }

            ++nIndex;
        }
    }

    VOID CSequenceOp::ProcessOutput( SequenceOpVec& vecOp )
    {
        BOOST_FOREACH( SSeqOpOutputLink& sLink, vecOutputLinks )
        {
            BOOST_FOREACH( FLOAT& fImpulse, sLink.vecImpulse )
            {
                if( fImpulse != SequenceImpulseEnable )
                    continue;

                fImpulse = SequenceImpulseDisable;

                BOOST_FOREACH( SSeqOpOutputInputLink& sOutInLink, sLink.vecLinks )
                {
                    CSequenceOp* pOp = sOutInLink.pLinkedOp;

                    SequenceOpVecIter fi = std::find( vecOp.begin(), vecOp.end(), pOp );
                    if( fi != vecOp.end() )
                        continue;

                    pOp->InsertInputImpulse( sOutInLink.nInputLinkIndex );
                    vecOp.insert( vecOp.begin(), pOp );
                }
            }
        }
    }

    VOID CSequenceOp::DeActivated()
    {
        size_t nSize = vecOutputLinks.size();
        for( size_t i=0; i<nSize; ++i )
            InsertOutputImpulse( i );
    }

    VOID CSequenceOp::InsertInputImpulse( size_t nIndex )
    {
        if( nIndex < 0 || nIndex >= vecInputLinks.size() )
            return;

        vecInputLinks[ nIndex ].vecImpulse.push_back( vecInputLinks[ nIndex ].fDelay );
    }

    VOID CSequenceOp::InsertOutputImpulse( size_t nIndex )
    {
        if( nIndex < 0 || nIndex >= vecOutputLinks.size() )
            return;

        vecOutputLinks[ nIndex ].vecImpulse.push_back( vecOutputLinks[ nIndex ].fDelay );
    }

    BOOL CSequenceOp::UpdateInputImpulse( FLOAT fElapsedTime )
    {
        BOOL bImpulse = FALSE;

        BOOST_FOREACH( SSeqOpInputLink& sLink, vecInputLinks )
        {
            SeqImpulseVecIter it = sLink.vecImpulse.begin();
            while( it != sLink.vecImpulse.end() )
            {
                FLOAT& fDelay = *it;

                if( fDelay == SequenceImpulseDisable )
                {
                    it = sLink.vecImpulse.erase( it );
                }
                else
                {
                    if( fDelay > fElapsedTime )
                    {
                        fDelay -= fElapsedTime;
                    }
                    else
                    {
                        fDelay = SequenceImpulseEnable;
                        bImpulse = TRUE;
                    }
                    ++it;
                }
            }
        }

        return bImpulse;
    }

    BOOL CSequenceOp::UpdateOutputImpulse( FLOAT fElapsedTime )
    {
        BOOL bImpulse = FALSE;

        BOOST_FOREACH( SSeqOpOutputLink& sLink, vecOutputLinks )
        {
	        SeqImpulseVecIter it = sLink.vecImpulse.begin();
            while( it != sLink.vecImpulse.end() )
            {
                FLOAT& fDelay = *it;

                if( fDelay == SequenceImpulseDisable )
                {
                    it = sLink.vecImpulse.erase( it );
                }
                else
                {
                    if( fDelay > fElapsedTime )
                    {
                        fDelay -= fElapsedTime;
                    }
                    else
                    {
                        fDelay = SequenceImpulseEnable;
                        bImpulse = TRUE;
                    }
                    ++it;
                }
            }
        }

        return bImpulse;
    }

    ic::Int2 CSequenceOp::GetConnectionLocation( INT nConnType, INT nConnIndex )
    {
        if( nConnIndex < 0 )
            return ic::Int2( 0, 0 );

        switch( nConnType )
        {
        case ic::EInput:
            {
                if( nConnIndex < (INT)vecInputLinks.size() )
                {
                    return ic::Int2(
                        sPos.x - ic::ConnectorLength,
                        vecInputLinks[ nConnIndex ].nDrawY );
                }
            }
            break;

        case ic::EOutput:
            {
                if( nConnIndex < (INT)vecOutputLinks.size() )
                {
                    return ic::Int2(
                        sPos.x + sDrawSize.x + ic::ConnectorLength,
                        vecOutputLinks[ nConnIndex ].nDrawY );
                }
            }
            break;

        case ic::EVariable:
            {
                if( nConnIndex < (INT)vecVariableLinks.size() )
                {
                    return ic::Int2(
                        vecVariableLinks[ nConnIndex ].nDrawX,
                        sPos.y + sDrawSize.y + ic::ConnectorLength );
                }
            }
            break;
        }

        return ic::Int2( 0, 0 );
    }

    VOID CSequenceOp::DrawObject(
        ic::IRenderInterface* pRI,
        BOOL bSelected,
        BOOL bMouseOver,
        INT nMouseOverConnType,
        INT nMouseOverConnIndex )
    {
        ic::Int2 sTitleSize = GetTitleBarSize( pRI );
        ic::Int2 sLogicSize = GetLogicConnectorsSize( pRI );
        ic::Int2 sVarSize = GetVariableConnectorsSize( pRI );

        INT nWidth = max( max( sTitleSize.x, sLogicSize.x ), sVarSize.x );

        if( pRI->IsHitTesting() )
            pRI->SetHitProxy( new ic::SLinkedObjProxy( this ) );

        DrawTitleBar(
            pRI,
            bSelected,
            bMouseOver,
            sPos,
            ic::Int2( nWidth, sTitleSize.y ) );

        pRI->DrawTile(
            ic::Int2( sPos.x, sPos.y + sTitleSize.y + 1 ),
            ic::Int2( nWidth, sLogicSize.y + sVarSize.y ),
            D3DXVECTOR2( 0.0f, 0.0f ),
            D3DXVECTOR2( 0.0f, 0.0f ),
            GetBorderColor( bSelected, bMouseOver ) );

        pRI->DrawTile(
            ic::Int2( sPos.x + 1, sPos.y + sTitleSize.y + 2 ),
            ic::Int2( nWidth - 2, sLogicSize.y + sVarSize.y - 2 ),
            D3DXVECTOR2( 0.0f, 0.0f ),
            D3DXVECTOR2( 0.0f, 0.0f ),
            D3DCOLOR_XRGB( 140, 140, 140 ) );

        ic::Int2 sLogicCenter;
        sLogicCenter.x = sPos.x + INT( nWidth * 0.5f );
        sLogicCenter.y = sPos.y + sTitleSize.y + INT( sLogicSize.y * 0.5f ) + 1;

        DrawExtraInfo( pRI, sLogicCenter );

        if( pRI->IsHitTesting() )
            pRI->SetHitProxy( NULL );

        DrawLogicConnectors(
            pRI,
            ic::Int2( sPos.x, sPos.y + sTitleSize.y + 1 ),
            ic::Int2( nWidth, sLogicSize.y ),
            bMouseOver ? nMouseOverConnType : INT_MIN,
            bMouseOver ? nMouseOverConnIndex : INT_MIN );

        DrawVariableConnectors(
            pRI,
            ic::Int2( sPos.x, sPos.y + sTitleSize.y + sLogicSize.y + 1 ),
            ic::Int2( nWidth, sVarSize.y ),
            bMouseOver ? nMouseOverConnType : INT_MIN,
            bMouseOver ? nMouseOverConnIndex : INT_MIN );
    }

    VOID CSequenceOp::DrawLogicLinks( ic::IRenderInterface* pRI )
    {
        INT nIndex = 0;

        BOOST_FOREACH( const SSeqOpOutputLink& sOutLink, vecOutputLinks )
        {
            ic::Int2 sStart = GetConnectionLocation( ic::EOutput, nIndex );

            BOOST_FOREACH( const SSeqOpOutputInputLink& sInLink, sOutLink.vecLinks )
            {
                if( sInLink.pLinkedOp == NULL ||
                    sInLink.nInputLinkIndex < 0 ||
                    sInLink.nInputLinkIndex >= (INT)sInLink.pLinkedOp->vecInputLinks.size() )
                {
                    continue;
                }

                ic::Int2 sEnd = sInLink.pLinkedOp->GetConnectionLocation( ic::EInput, sInLink.nInputLinkIndex );

                D3DXVECTOR2 vDir( FLOAT( abs( sStart.x - sEnd.x ) ), 0.0f );

                ic::CLinkedObjDrawUtils::DrawSpline(
                    pRI,
                    sStart,
                    vDir,
                    sEnd,
                    vDir,
                    D3DCOLOR_XRGB( 0, 0, 0 ),
                    TRUE );
            }

            ++nIndex;
        }
    }

    VOID CSequenceOp::DrawVariableLinks( ic::IRenderInterface* pRI )
    {
        INT nIndex = 0;

        BOOST_FOREACH( SSeqVarLink& sVarLink, vecVariableLinks )
        {
            ic::Int2 sStart = GetConnectionLocation( ic::EVariable, nIndex );

            BOOST_FOREACH( CSequenceVariable* pVariable, sVarLink.vecLinks )
            {
                ic::Int2 sEnd = pVariable->GetConnectionLocation();

                D3DXCOLOR sLinkColor( pVariable->nColor );
                sLinkColor *= MouseOverColorScale;

                D3DXVECTOR2 vDir( 0.0f, FLOAT( abs( sStart.y - sEnd.y ) ) );

                if( sVarLink.bWriteable )
                {
                    ic::CLinkedObjDrawUtils::DrawSpline(
                        pRI,
                        sStart,
                        vDir,
                        sEnd,
                        D3DXVECTOR2( 0.0f, 0.0f ),
                        sLinkColor,
                        TRUE );
                }
                else
                {
                    ic::CLinkedObjDrawUtils::DrawSpline(
                        pRI,
                        sEnd,
                        D3DXVECTOR2( 0.0f, 0.0f ),
                        sStart,
                        D3DXVECTOR2( vDir.x, vDir.y * -1.0f ),
                        sLinkColor,
                        TRUE );
                }
            }

            ++nIndex;
        }
    }

    INT CSequenceOp::FindConnectorIndex( const TSTRING& strConnName, INT nConnType )
    {
        switch( nConnType )
        {
        case ic::EInput:
            {
                size_t nSize = vecInputLinks.size();
                for( size_t i=0; i<nSize; ++i )
		        {
			        if( vecInputLinks[ i ].strLinkDesc == strConnName )
				        return INT( i );
		        }
            }
            break;

        case ic::EOutput:
            {
                size_t nSize = vecOutputLinks.size();
                for( size_t i=0; i<nSize; ++i )
		        {
			        if( vecOutputLinks[ i ].strLinkDesc == strConnName )
				        return INT( i );
		        }
            }
            break;

        case ic::EVariable:
            {
                size_t nSize = vecVariableLinks.size();
                for( size_t i=0; i<nSize; ++i )
		        {
			        if( vecVariableLinks[ i ].strLinkDesc == strConnName )
				        return INT( i );
		        }
            }
            break;
        }

	    return INT_MIN;
    }

    VOID CSequenceOp::CleanupConnections()
    {
        BOOST_FOREACH( SSeqOpOutputLink& sOutputLink, vecOutputLinks )
        {
            SeqOutputInputVecIter it = sOutputLink.vecLinks.begin();
            while( it != sOutputLink.vecLinks.end() )
            {
                CSequenceOp* pLinkedOp = ( *it ).pLinkedOp;

                if( pLinkedOp && pLinkedOp->GetParent() != GetParent() )
                    it = sOutputLink.vecLinks.erase( it );
                else
                    ++it;
            }
        }

        BOOST_FOREACH( SSeqVarLink& sVarLink, vecVariableLinks )
        {
            SeqVariableVecIter it = sVarLink.vecLinks.begin();
            while( it != sVarLink.vecLinks.end() )
            {
                CSequenceVariable* pVariable = *it;

                if( pVariable && pVariable->GetParent() != GetParent() )
                    it = sVarLink.vecLinks.erase( it );
                else
                    ++it;
            }
        }
    }

    ic::Int2 CSequenceOp::GetLogicConnectorsSize(
        ic::IRenderInterface* pRI,
        INT* pInputY,
        INT* pOutputY )
    {
        ic::SLinkedObjDrawInfo sObjInfo;
        MakeLinkedObjDrawInfo( sObjInfo );

        return ic::CLinkedObjDrawUtils::GetLogicConnectorsSize(
            pRI,
            sObjInfo,
            pInputY,
            pOutputY );
    }

    ic::Int2 CSequenceOp::GetVariableConnectorsSize( ic::IRenderInterface* pRI )
    {
        ic::SLinkedObjDrawInfo sObjInfo;
        MakeLinkedObjDrawInfo( sObjInfo );

        return ic::CLinkedObjDrawUtils::GetVariableConnectorsSize( pRI, sObjInfo );
    }

    DWORD CSequenceOp::GetVarConnectorColor( INT nLinkIndex )
    {
        if( nLinkIndex < 0 || nLinkIndex >= (INT)vecVariableLinks.size() )
            return D3DCOLOR_XRGB( 0, 0, 0 );

        SSeqVarLink& sVarLink = vecVariableLinks[ nLinkIndex ];
        CSequenceVariable* pVariable = dynamic_cast< CSequenceVariable* >( sVarLink.pExpected );
        if( !pVariable )
            return D3DCOLOR_XRGB( 255, 0, 255 );

        return pVariable->nColor;
    }

    VOID CSequenceOp::DrawLogicConnectors(
        ic::IRenderInterface* pRI,
        const ic::Int2& sPos_,
        const ic::Int2& sSize,
        INT nMouseOverConnType,
        INT nMouseOverConnIndex )
    {
        sDrawSize.x = sSize.x;

        ic::SLinkedObjDrawInfo sObjInfo;
        MakeLinkedObjDrawInfo( sObjInfo, nMouseOverConnType, nMouseOverConnIndex );

        ic::CLinkedObjDrawUtils::DrawLogicConnectors( pRI, sObjInfo, sPos_, sSize );

        size_t nSize = vecInputLinks.size();
        for( size_t i=0; i<nSize; ++i )
            vecInputLinks[ i ].nDrawY = sObjInfo.vecInputY[ i ];

        nSize = vecOutputLinks.size();
        for( size_t i=0; i<nSize; ++i )
            vecOutputLinks[ i ].nDrawY = sObjInfo.vecOutputY[ i ];
    }

    VOID CSequenceOp::DrawVariableConnectors(
        ic::IRenderInterface* pRI,
        const ic::Int2& sPos_,
        const ic::Int2& sSize,
        INT nMouseOverConnType,
        INT nMouseOverConnIndex )
    {
        sDrawSize.y = ( sPos_.y + sSize.y ) - sPos.y;

        ic::SLinkedObjDrawInfo sObjInfo;
        MakeLinkedObjDrawInfo( sObjInfo, nMouseOverConnType, nMouseOverConnIndex );

        ic::CLinkedObjDrawUtils::DrawVariableConnectors( pRI, sObjInfo, sPos_, sSize );

        size_t nSize = vecVariableLinks.size();
        for( size_t i=0; i<nSize; ++i )
            vecVariableLinks[ i ].nDrawX = sObjInfo.vecVariableX[ i ];
    }

    VOID CSequenceOp::MakeLinkedObjDrawInfo(
        ic::SLinkedObjDrawInfo& sObjInfo,
        INT nMouseOverConnType,
        INT nMouseOverConnIndex )
    {
        const DWORD ImpulseColor = D3DCOLOR_XRGB( 255, 0, 255 );

        INT nIndex = 0;

        BOOST_FOREACH( SSeqOpInputLink& sLink, vecInputLinks )
        {
            ic::SLinkedObjConnInfo sConnInfo;
            sConnInfo.pName = sLink.strLinkDesc.c_str();
            sConnInfo.fDelay = sLink.fDelay;

            if( nMouseOverConnType == ic::EInput && nMouseOverConnIndex == nIndex )
                sConnInfo.nColor = MouseOverLogicColor;
            else if( !sLink.vecImpulse.empty() )
                sConnInfo.nColor = ImpulseColor;

            sObjInfo.vecInputs.push_back( sConnInfo );

            ++nIndex;
        }

        nIndex = 0;

        BOOST_FOREACH( SSeqOpOutputLink& sLink, vecOutputLinks )
        {
            ic::SLinkedObjConnInfo sConnInfo;
            sConnInfo.pName = sLink.strLinkDesc.c_str();
            sConnInfo.fDelay = sLink.fDelay;

            if( nMouseOverConnType == ic::EOutput && nMouseOverConnIndex == nIndex )
                sConnInfo.nColor = MouseOverLogicColor;
            else if( !sLink.vecImpulse.empty() )
                sConnInfo.nColor = ImpulseColor;

            sObjInfo.vecOutputs.push_back( sConnInfo );

            ++nIndex;
        }

        nIndex = 0;

        BOOST_FOREACH( SSeqVarLink& sLink, vecVariableLinks )
        {
            ic::SLinkedObjConnInfo sConnInfo;
            sConnInfo.pName = sLink.strLinkDesc.c_str();
            sConnInfo.bWriteable = sLink.bWriteable;

            DWORD nColor = GetVarConnectorColor( nIndex );

            if( nMouseOverConnType == ic::EVariable && nMouseOverConnIndex == nIndex )
                sConnInfo.nColor = nColor;
            else
                sConnInfo.nColor = D3DXCOLOR( nColor ) * MouseOverColorScale;

            sObjInfo.vecVariables.push_back( sConnInfo );

            ++nIndex;
        }

        sObjInfo.pObject = this;
    }

    VOID CSequenceOp::LinkFromIdentity( const SequenceObjectMap& mapObjects )
    {
        BOOST_FOREACH( SSeqOpOutputLink& sLink, vecOutputLinks )
        {
            BOOST_FOREACH( SSeqOpOutputInputLink& sOutInLink, sLink.vecLinks )
            {
                DWORD nIdentity = reinterpret_cast< DWORD >( sOutInLink.pLinkedOp );
                SequenceObjectMapCIter it = mapObjects.find( nIdentity );
                __gassert_( it != mapObjects.end() );
                sOutInLink.pLinkedOp = dynamic_cast< CSequenceOp* >( it->second );
            }
        }

        BOOST_FOREACH( SSeqVarLink& sLink, vecVariableLinks )
        {
            BOOST_FOREACH( CSequenceVariable*& pVar, sLink.vecLinks )
            {
                DWORD nIdentity = reinterpret_cast< DWORD >( pVar );
                SequenceObjectMapCIter it = mapObjects.find( nIdentity );
                __gassert_( it != mapObjects.end() );
                pVar = dynamic_cast< CSequenceVariable* >( it->second );
            }
        }
    }
}