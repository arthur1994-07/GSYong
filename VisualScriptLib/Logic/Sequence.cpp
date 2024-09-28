#include "stdafx.h"

#include <boost/bind.hpp>
#include <boost/mem_fn.hpp>

#include "../../InternalCommonLib/Interface/RenderInterface.h"

#include "./SequenceEvent.h"
#include "./SequenceVariable.h"
#include "./SequenceAction.h"

#include "./Sequence.h"

#include "../../InternalCommonLib/DebugInclude.h"

namespace vs
{
    CSequence::CSequence()
        : bEnabled( TRUE )
        , fDefaultViewZoom( 1.0f )
    {
        strName = _T( "Sequence" );
    }

    CSequence::~CSequence()
    {
        ClearObject();
    }

    VOID CSequence::RegistProperty()
    {
        Super::RegistProperty();

        {
            ic::CPropertyString* pProperty = ic::CPropertyFactory::New< ic::CPropertyString >();
            pProperty->SetPtr( &strName );
            pProperty->SetCall( boost::bind( boost::mem_fn( &CSequence::OnChangedName ), this ) );
            InsertProperty( _T( "Name" ), pProperty );
        }
        {
            ic::CPropertyBool* pProperty = ic::CPropertyFactory::New< ic::CPropertyBool >();
            pProperty->SetPtr( &bEnabled );
            InsertProperty( _T( "Enabled" ), pProperty );
        }
    }

    VOID CSequence::PrevSave()
    {
        SequenceObjectVec vecSaveList;

        BOOST_FOREACH( const SequenceObjectMapValue& it, mapObjects )
            vecSaveList.push_back( it.second );

        std::sort( vecSaveList.begin(), vecSaveList.end(), CompSequenceObject() );

        DWORD nIdentity = 0;

        BOOST_FOREACH( CSequenceObject* pObject, vecSaveList )
            pObject->SetIdentityForIO( nIdentity++ );

        BOOST_FOREACH( SequenceObjectMapValue& it, mapObjects )
        {
            CSequence* pSequence = dynamic_cast< CSequence* >( it.second );
            if( pSequence )
                pSequence->PrevSave();
        }
    }

    VOID CSequence::PostSave()
    {
        BOOST_FOREACH( SequenceObjectMapValue& it, mapObjects )
            it.second->SetIdentity();

        BOOST_FOREACH( SequenceObjectMapValue& it, mapObjects )
        {
            CSequence* pSequence = dynamic_cast< CSequence* >( it.second );
            if( pSequence )
                pSequence->PostSave();
        }
    }

    VOID CSequence::PostLoad()
    {
        BOOST_FOREACH( SequenceObjectMapValue& it, mapObjects )
        {
            CSequenceOp* pSequenceOp = dynamic_cast< CSequenceOp* >( it.second );
            if( pSequenceOp )
                pSequenceOp->LinkFromIdentity( mapObjects );
        }

        SequenceObjectMap mapTemp = mapObjects;
        mapObjects.clear();

        BOOST_FOREACH( SequenceObjectMapValue& it, mapTemp )
        {
            CSequenceObject* pObject = it.second;

            pObject->SetIdentity();

            InsertObject( pObject );
        }

        UpdateConnectors();

        BOOST_FOREACH( SequenceObjectMapValue& it, mapObjects )
        {
            CSequenceObject* pObject = it.second;

            pObject->PostLoad();

            CSequence* pSequence = dynamic_cast< CSequence* >( pObject );
            if( pSequence )
                pSequence->PostLoad();
        }
    }

    VOID CSequence::CheckForErrors()
    {
        SequenceObjectMapIter it = mapObjects.begin();
        while( it != mapObjects.end() )
        {
            CSequenceObject* pObject = it->second;

            if( pObject )
            {
                if( pObject->GetParent() != this )
                    pObject->SetParent( this );
                pObject->CheckForErrors();

                ++it;
            }
            else
            {
                it = mapObjects.erase( it );
            }
        }
    }

    VOID CSequence::OnCreated()
    {
        Super::OnCreated();

        UpdateConnectors();
    }

    static VOID AddNamedVariableToLink( SSeqVarLink& sLink, CSeqVar_Named* pNamedVar )
    {
	    if( !pNamedVar || !pNamedVar->pExpected )
		    return;

	    if( pNamedVar->strFindVarName.empty() )
		    return;

        CSequence* pRoot = pNamedVar->GetRootSequence();
        __gassert_( pRoot );

        SeqVariableVec vecVars;
        pRoot->FindNamedVariable( pNamedVar->strFindVarName, FALSE, vecVars );

        if( vecVars.size() != 1 )
            return;

        if( !vecVars[ 0 ]->IsChildOf( pNamedVar->pExpected ) )
            return;

        SeqVariableVecIter fi = std::find(
            sLink.vecLinks.begin(),
            sLink.vecLinks.end(),
            vecVars[ 0 ] );

        if( fi != sLink.vecLinks.end() )
            sLink.vecLinks.push_back( vecVars[ 0 ] );
    }

    VOID CSequence::BeginPlay()
    {
        BOOST_FOREACH( SequenceObjectMapValue& it, mapObjects )
        {
            CSequenceObject* pObject = it.second;

            if( pObject->IsA( CSeqVar_Named::Rtti() ) )
            {
                CSeqVar_Named* pNamedVar = dynamic_cast< CSeqVar_Named* >( pObject );

                BOOST_FOREACH( SequenceObjectMapValue& jt, mapObjects )
                {
                    CSequenceOp* pOp = dynamic_cast< CSequenceOp* >( it.second );
                    if( !pOp )
                        continue;

                    BOOST_FOREACH( SSeqVarLink& sLink, pOp->vecVariableLinks )
                    {
                        SeqVariableVecIter fi = std::find(
                            sLink.vecLinks.begin(),
                            sLink.vecLinks.end(),
                            pNamedVar );

                        if( fi != sLink.vecLinks.end() )
                        {
                            sLink.vecLinks.erase( fi );
                            AddNamedVariableToLink( sLink, pNamedVar );
                        }
				    }
			    }
		    }
	    }

        BOOST_FOREACH( SequenceObjectMapValue& it, mapObjects )
        {
            CSequence* pSequence = dynamic_cast< CSequence* >( it.second );
            if( pSequence )
		        pSequence->BeginPlay();
        }
    }

    VOID CSequence::Activated( size_t nInput )
    {
        if( nInput < 0 || nInput >= vecInputLinks.size() )
            return;

        BOOST_FOREACH( SequenceObjectMapValue& it, mapObjects )
        {
            CSeqEvent_SequenceActivated* pEvent = dynamic_cast< CSeqEvent_SequenceActivated* >( it.second );

            if( pEvent && pEvent->strName.compare( vecInputLinks[ nInput ].strLinkAction ) == 0 )
                pEvent->InsertInputImpulse();
        }
    }

    VOID CSequence::ExecuteOp( FLOAT fElapsedTime )
    {
        size_t nSize = mapObjects.size();
        size_t nProcess = 0;

        SequenceOpVec vecOp;
        vecOp.reserve( nSize );

        BOOST_FOREACH( SequenceObjectMapValue& it, mapObjects )
        {
            CSequenceOp* pOp = dynamic_cast< CSequenceOp* >( it.second );
            if( !pOp )
                continue;

            BOOL bImpulse = pOp->UpdateInputImpulse( fElapsedTime );
            bImpulse = pOp->UpdateOutputImpulse( fElapsedTime ) || bImpulse;
            if( bImpulse )
                vecOp.insert( vecOp.begin(), pOp );
        }

        while( !vecOp.empty() )
        {
            CSequenceOp* pOp = vecOp.back();
            pOp->ProcessInput();
            pOp->ProcessOutput( vecOp );
            vecOp.pop_back();

            if( ++nProcess > nSize )
                break;
        }
    }

    VOID CSequence::UpdateOp( FLOAT fElapsedTime )
    {
        if( mapObjects.empty() )
            return;

	    ExecuteOp( fElapsedTime );

        BOOST_FOREACH( SequenceObjectMapValue& it, mapObjects )
	    {
		    CSequence* pSequence = dynamic_cast< CSequence* >( it.second );
		    if( pSequence )
			    pSequence->UpdateOp( fElapsedTime );
	    }
    }

    CSequenceObject* CSequence::NewObject( size_t nIndex, const ic::Int2& sPos )
    {
        ic::CBaseObject* pStatic = ic::CRttiMgr::FindStatic( nIndex );
        CSequenceObject* pObject = dynamic_cast< CSequenceObject* >( pStatic );
        if( !pObject )
            return NULL;

        ic::CBaseObject* pNew = NULL;
        pObject->VNew( pNew );

        pObject = dynamic_cast< CSequenceObject* >( pNew );
        if( pObject->strName.empty() )
            pObject->strName = pObject->VName();
        pObject->sPos = sPos;

        return pObject;
    }

    VOID CSequence::InsertObject( CSequenceObject* pTarget )
    {
        if( !pTarget )
            return;

        pTarget->SetParent( this );
        pTarget->VRegistProperty();

        SequenceObjectMapIter it = mapObjects.find( pTarget->GetIdentity() );
        __gassert_( it == mapObjects.end() );

        mapObjects.insert( SequenceObjectMapValue( pTarget->GetIdentity(), pTarget ) );
    }

    VOID CSequence::PasteObject( SequenceObjectVec& vecObjects )
    {
        BOOST_FOREACH( CSequenceObject* pObject, vecObjects )
        {
            PastePostObject( pObject );
            InsertObject( pObject );
        }
    }

    VOID CSequence::PastePostObject( CSequenceObject* pTarget )
    {
        if( !pTarget )
            return;

        pTarget->SetIdentity();

        BreakLinkAll( pTarget );

        if( pTarget->IsA( CSequence::Rtti() ) )
        {
            MekeUniqueName( pTarget );

            CSequence* pSequence = dynamic_cast< CSequence* >( pTarget );
            pSequence->PastePostInSequence();
        }
    }

    VOID CSequence::PastePostInSequence()
    {
        BOOST_FOREACH( SequenceObjectMapValue& it, mapObjects )
            PastePostObject( it.second );
    }

    VOID CSequence::DeleteObject( CSequenceObject* pTarget )
    {
        if( !pTarget )
            return;

        BreakLinkAll( pTarget );

        SequenceObjectMapIter it = mapObjects.find( pTarget->GetIdentity() );
        if( it != mapObjects.end() )
            mapObjects.erase( it );

        SAFE_DELETE( pTarget );

        UpdateConnectors();
        UpdateRemoteEventStatus();
        UpdateNamedVarStatus();
        UpdateActivateRemoteEventStatus();
    }

    VOID CSequence::MekeUniqueName( CSequenceObject* pTarget )
    {
        if( !pTarget )
            return;

        CSequence* pRoot = GetRootSequence();
        __gassert_( pRoot );

        TSTRING strUniqueName = pTarget->strName;

        do
        {
            SequenceObjectVec vecObjects;
            pRoot->FindObjectByName( strUniqueName, vecObjects );

            SequenceObjectVecIter fi = std::find( vecObjects.begin(), vecObjects.end(), this );
            if( fi != vecObjects.end() )
                vecObjects.erase( fi );

            if( vecObjects.empty() )
                break;

            strUniqueName.append( _T( "_" ) );
        }
        while( TRUE );

        pTarget->strName = strUniqueName;
    }

    VOID CSequence::ClearObject()
    {
        BOOST_FOREACH( SequenceObjectMapValue& it, mapObjects )
            SAFE_DELETE( it.second );

        mapObjects.clear();
    }

    VOID CSequence::BreakLinkOutput( CSequenceObject* pTarget )
    {
        CSequenceOp* pTargetOp = dynamic_cast< CSequenceOp* >( pTarget );
        if( !pTargetOp )
            return;

        BOOST_FOREACH( SequenceObjectMapValue& it, mapObjects )
        {
            CSequenceOp* pSequenceOp = dynamic_cast< CSequenceOp* >( it.second );
            if( !pSequenceOp )
                continue;

            BOOST_FOREACH( SSeqOpOutputLink& sOutputLink, pSequenceOp->vecOutputLinks )
            {
                SeqOutputInputVecIter itLink = sOutputLink.vecLinks.begin();
                while( itLink != sOutputLink.vecLinks.end() )
                {
                    SSeqOpOutputInputLink& sOutputInput = *itLink;

                    if( sOutputInput.pLinkedOp == pTargetOp )
                        itLink = sOutputLink.vecLinks.erase( itLink );
                    else
                        ++itLink;
                }
            }
        }
    }

    VOID CSequence::BreakLinkVariable( CSequenceObject* pTarget )
    {
        CSequenceVariable* pTargetVariable = dynamic_cast< CSequenceVariable* >( pTarget );
        if( !pTargetVariable )
            return;

        BOOST_FOREACH( SequenceObjectMapValue& it, mapObjects )
        {
            CSequenceOp* pSequenceOp = dynamic_cast< CSequenceOp* >( it.second );
            if( !pSequenceOp )
                continue;

            INT nIndex = 0;
            std::vector< INT > vecBreakIndex;

            BOOST_FOREACH( SSeqVarLink& sVarLink, pSequenceOp->vecVariableLinks )
            {
                SeqVariableVecIter itLink = sVarLink.vecLinks.begin();
                while( itLink != sVarLink.vecLinks.end() )
                {
                    CSequenceVariable* pLinkVariable = *itLink;

                    if( pLinkVariable == pTargetVariable )
                    {
                        itLink = sVarLink.vecLinks.erase( itLink );
                        vecBreakIndex.push_back( nIndex );
                    }
                    else
                    {
                        ++itLink;
                    }
                }

                ++nIndex;
            }

            BOOST_FOREACH( nIndex, vecBreakIndex )
                pSequenceOp->OnPostBreakVariable( nIndex );
        }
    }

    VOID CSequence::BreakLink( CSequenceObject* pTarget, INT nConnType, INT nConnIndex, INT nLinkIndex )
    {
        CSequenceOp* pTargetOp = dynamic_cast< CSequenceOp* >( pTarget );
        if( !pTargetOp )
            return;

        if( nConnIndex < 0 )
            return;

        BOOL bLinkAll = nLinkIndex < 0 ? TRUE : FALSE;

        switch( nConnType )
        {
        case ic::EInput:
            {
                if( nConnIndex >= (INT)pTargetOp->vecInputLinks.size() )
                    return;

                INT nCount = 0;

                BOOST_FOREACH( SequenceObjectMapValue& it, mapObjects )
                {
                    CSequenceOp* pSequenceOp = dynamic_cast< CSequenceOp* >( it.second );
                    if( !pSequenceOp )
                        continue;

                    BOOST_FOREACH( SSeqOpOutputLink& sOutputLink, pSequenceOp->vecOutputLinks )
                    {
                        SeqOutputInputVecIter itLink = sOutputLink.vecLinks.begin();
                        while( itLink != sOutputLink.vecLinks.end() )
                        {
                            SSeqOpOutputInputLink& sOutputInput = *itLink;

                            if( sOutputInput.pLinkedOp == pTargetOp &&
                                sOutputInput.nInputLinkIndex == nConnIndex )
                            {
                                if( bLinkAll )
                                {
                                    itLink = sOutputLink.vecLinks.erase( itLink );
                                }
                                else
                                {
                                    if( nLinkIndex == nCount++ )
                                    {
                                        itLink = sOutputLink.vecLinks.erase( itLink );
                                        return;
                                    }

                                    ++itLink;
                                }
                            }
                            else
                            {
                                ++itLink;
                            }
                        }
                    }
                }
            }
            break;

        case ic::EOutput:
            {
                if( nConnIndex >= (INT)pTargetOp->vecOutputLinks.size() )
                    return;

                if( bLinkAll )
                {
                    pTargetOp->vecOutputLinks[ nConnIndex ].vecLinks.clear();
                }
                else
                {
                    INT nLinkSize = pTargetOp->vecOutputLinks[ nConnIndex ].vecLinks.size();
                    if( nLinkIndex >= 0 && nLinkIndex < nLinkSize )
                    {
                        pTargetOp->vecOutputLinks[ nConnIndex ].vecLinks.erase(
                            pTargetOp->vecOutputLinks[ nConnIndex ].vecLinks.begin() + nLinkIndex );
                    }
                }
            }
            break;

        case ic::EVariable:
            {
                if( nConnIndex >= (INT)pTargetOp->vecVariableLinks.size() )
                    return;

                if( bLinkAll )
                {
                    pTargetOp->vecVariableLinks[ nConnIndex ].vecLinks.clear();
                }
                else
                {
                    INT nLinkSize = pTargetOp->vecVariableLinks[ nConnIndex ].vecLinks.size();
                    if( nLinkIndex >= 0 && nLinkIndex < nLinkSize )
                    {
                        pTargetOp->vecVariableLinks[ nConnIndex ].vecLinks.erase(
                            pTargetOp->vecVariableLinks[ nConnIndex ].vecLinks.begin() + nLinkIndex );
                    }
                }

                pTargetOp->OnPostBreakVariable( nConnIndex );
            }
            break;
        }
    }

    VOID CSequence::BreakLinkAll( CSequenceObject* pTarget )
    {
        CSequenceOp* pTargetOp = dynamic_cast< CSequenceOp* >( pTarget );
        if( pTargetOp )
        {
            BOOST_FOREACH( SSeqOpOutputLink& sOutputLink, pTargetOp->vecOutputLinks )
                sOutputLink.vecLinks.clear();

            INT nSizeVar = (INT)pTargetOp->vecVariableLinks.size();

            BOOST_FOREACH( SSeqVarLink& sVarLink, pTargetOp->vecVariableLinks )
                sVarLink.vecLinks.clear();

            for( INT i=0; i<nSizeVar; ++i )
                pTargetOp->OnPostBreakVariable( i );
        }

        BreakLinkOutput( pTarget );
        BreakLinkVariable( pTarget );
    }

    VOID CSequence::BreakLinkAll()
    {
        BOOST_FOREACH( SequenceObjectMapValue& it, mapObjects )
            BreakLinkAll( it.second );
    }

    VOID CSequence::FindObjectByClass( ic::CBaseObject* pDesired, SequenceObjectVec& vecObjects )
    {
        BOOST_FOREACH( SequenceObjectMapValue& it, mapObjects )
        {
            CSequenceObject* pObject = it.second;

            if( pObject->IsA( pDesired ) )
                vecObjects.push_back( pObject );
        }

        BOOST_FOREACH( SequenceObjectMapValue& it, mapObjects )
        {
            CSequence* pSequence = dynamic_cast< CSequence* >( it.second );
            if( pSequence )
                pSequence->FindObjectByClass( pDesired, vecObjects );
        }
    }

    VOID CSequence::FindObjectByName( const TSTRING& strName, SequenceObjectVec& vecObjects )
    {
        BOOST_FOREACH( SequenceObjectMapValue& it, mapObjects )
        {
            CSequenceObject* pObject = it.second;

            if( pObject->strName.compare( strName ) == 0 )
                vecObjects.push_back( pObject );
        }

        BOOST_FOREACH( SequenceObjectMapValue& it, mapObjects )
        {
            CSequence* pSequence = dynamic_cast< CSequence* >( it.second );
            if( pSequence )
                pSequence->FindObjectByName( strName, vecObjects );
        }
    }

    VOID CSequence::FindRemoteEvent( const TSTRING& strLabel, SequenceObjectVec& vecObjects )
    {
        if( strLabel.empty() )
            return;

        BOOST_FOREACH( SequenceObjectMapValue& it, mapObjects )
        {
            CSeqEvent_RemoteEvent* pRemoteEvent = dynamic_cast< CSeqEvent_RemoteEvent* >( it.second );
            if( pRemoteEvent && pRemoteEvent->strLabel.compare( strLabel ) == 0 )
                vecObjects.push_back( pRemoteEvent );
        }

        BOOST_FOREACH( SequenceObjectMapValue& it, mapObjects )
        {
            CSequence* pSequence = dynamic_cast< CSequence* >( it.second );
            if( pSequence )
                pSequence->FindRemoteEvent( strLabel, vecObjects );
        }
    }

    VOID CSequence::FindNamedVariable( const TSTRING& strVarName, BOOL bFindUses, SeqVariableVec& vecVars )
    {
        if( strVarName.empty() )
            return;

        BOOST_FOREACH( SequenceObjectMapValue& it, mapObjects )
        {
            CSequenceObject* pObject = it.second;

            if( bFindUses )
            {
                CSeqVar_Named* pNamedVar = dynamic_cast< CSeqVar_Named* >( pObject );
                if( pNamedVar && pNamedVar->strFindVarName.compare( strVarName ) == 0 )
                {
                    SeqVariableVecIter fi = std::find(
                        vecVars.begin(),
                        vecVars.end(),
                        pNamedVar );

                    if( fi == vecVars.end() )
                        vecVars.push_back( pNamedVar );
                }
            }
            else
            {
                CSequenceVariable* pVariable = dynamic_cast< CSequenceVariable* >( pObject );
                if( pVariable && pVariable->strVarName.compare( strVarName ) == 0 )
                {
                    SeqVariableVecIter fi = std::find(
                        vecVars.begin(),
                        vecVars.end(),
                        pVariable );

                    if( fi == vecVars.end() )
                        vecVars.push_back( pVariable );
                }
            }
        }

        BOOST_FOREACH( SequenceObjectMapValue& it, mapObjects )
        {
            CSequence* pSequence = dynamic_cast< CSequence* >( it.second );
            if( pSequence )
                pSequence->FindNamedVariable( strVarName, bFindUses, vecVars );
        }
    }

    VOID CSequence::FindActivateRemoteEvent( const TSTRING& strLabel, SequenceObjectVec& vecObjects )
    {
        if( strLabel.empty() )
            return;

        BOOST_FOREACH( SequenceObjectMapValue& it, mapObjects )
        {
            CSeqAct_ActivateRemoteEvent* pActivateRemoteEvent = dynamic_cast< CSeqAct_ActivateRemoteEvent* >( it.second );
            if( pActivateRemoteEvent && pActivateRemoteEvent->strLabel.compare( strLabel ) == 0 )
                vecObjects.push_back( pActivateRemoteEvent );
        }

        BOOST_FOREACH( SequenceObjectMapValue& it, mapObjects )
        {
            CSequence* pSequence = dynamic_cast< CSequence* >( it.second );
            if( pSequence )
                pSequence->FindActivateRemoteEvent( strLabel, vecObjects );
        }
    }

    VOID CSequence::UpdateConnectors()
    {
        std::vector< TSTRING > vecInputNames;
        std::vector< TSTRING > vecOutputNames;

        BOOST_FOREACH( SequenceObjectMapValue& it, mapObjects )
	    {
            CSequenceObject* pObject = it.second;

            if( pObject->IsA( CSeqEvent_SequenceActivated::Rtti() ) )
            {
                CSeqEvent_SequenceActivated* pEvent = dynamic_cast< CSeqEvent_SequenceActivated* >( pObject );

                BOOL bFound = FALSE;

                BOOST_FOREACH( SSeqOpInputLink& sLink, vecInputLinks )
                {
                    if( sLink.strLinkAction.compare( pEvent->strName ) == 0 )
                    {
                        bFound = TRUE;
                        sLink.strLinkDesc = pEvent->strLabel;
                        vecInputNames.push_back( pEvent->strName );
                    }
                }

                if( !bFound )
                {
                    SSeqOpInputLink sPush;
                    sPush.strLinkDesc = pEvent->strLabel;
                    sPush.strLinkAction = pEvent->strName;
                    vecInputLinks.push_back( sPush );
                    vecInputNames.push_back( pEvent->strName );
                }
            }
            else if( pObject->IsA( CSeqAct_FinishSequence::Rtti() ) )
            {
                CSeqAct_FinishSequence* pAction = dynamic_cast< CSeqAct_FinishSequence* >( pObject );

                BOOL bFound = FALSE;

                BOOST_FOREACH( SSeqOpOutputLink& sLink, vecOutputLinks )
                {
                    if( sLink.strLinkAction.compare( pAction->strName ) == 0 )
                    {
                        bFound = TRUE;
                        sLink.strLinkDesc = pAction->strLabel;
                        vecOutputNames.push_back( pAction->strName );
                    }
                }

                if( !bFound )
                {
                    SSeqOpOutputLink sPush;
                    sPush.strLinkDesc = pAction->strLabel;
                    sPush.strLinkAction = pAction->strName;
                    vecOutputLinks.push_back( sPush );
                    vecOutputNames.push_back( pAction->strName );
                }
            }
        }

        // Input
        {
            InputVec::iterator it = vecInputLinks.begin();
            while( it != vecInputLinks.end() )
            {
                std::vector< TSTRING >::iterator fi = std::find(
                    vecInputNames.begin(),
                    vecInputNames.end(),
                    it->strLinkAction );

                if( fi == vecInputNames.end() )
                    it = vecInputLinks.erase( it );
                else
                    ++it;
            }
        }

        // Output
        {
            OutputVec::iterator it = vecOutputLinks.begin();
            while( it != vecOutputLinks.end() )
            {
                std::vector< TSTRING >::iterator fi = std::find(
                    vecOutputNames.begin(),
                    vecOutputNames.end(),
                    it->strLinkAction );

                if( fi == vecOutputNames.end() )
                    it = vecOutputLinks.erase( it );
                else
                    ++it;
            }
        }
    }

    VOID CSequence::UpdateRemoteEventStatus()
    {
        BOOST_FOREACH( SequenceObjectMapValue& it, mapObjects )
	    {
            CSeqEvent_RemoteEvent* pEvent = dynamic_cast< CSeqEvent_RemoteEvent* >( it.second );
            if( pEvent )
                pEvent->OnChangedLabel();
        }

        BOOST_FOREACH( SequenceObjectMapValue& it, mapObjects )
        {
            CSequence* pSequence = dynamic_cast< CSequence* >( it.second );
            if( pSequence )
	            pSequence->UpdateRemoteEventStatus();
        }
    }

    VOID CSequence::UpdateNamedVarStatus()
    {
        BOOST_FOREACH( SequenceObjectMapValue& it, mapObjects )
	    {
            CSeqVar_Named* pNamedVar = dynamic_cast< CSeqVar_Named* >( it.second );
            if( pNamedVar )
                pNamedVar->OnChangedFindVarName();
        }

        BOOST_FOREACH( SequenceObjectMapValue& it, mapObjects )
        {
            CSequence* pSequence = dynamic_cast< CSequence* >( it.second );
            if( pSequence )
	            pSequence->UpdateNamedVarStatus();
        }
    }

    VOID CSequence::UpdateActivateRemoteEventStatus()
    {
        BOOST_FOREACH( SequenceObjectMapValue& it, mapObjects )
	    {
            CSeqAct_ActivateRemoteEvent* pAction = dynamic_cast< CSeqAct_ActivateRemoteEvent* >( it.second );
            if( pAction )
                pAction->OnChangedLabel();
        }

        BOOST_FOREACH( SequenceObjectMapValue& it, mapObjects )
        {
            CSequence* pSequence = dynamic_cast< CSequence* >( it.second );
            if( pSequence )
	            pSequence->UpdateActivateRemoteEventStatus();
        }
    }

    VOID CSequence::DrawSequence(
        ic::IRenderInterface* pRI,
        const SequenceObjectVec& vecSelected,
        const CSequenceObject* pMouseOver,
        INT nMouseOverConnType,
        INT nMouseOverConnIndex )
    {
        BOOST_FOREACH( SequenceObjectMapValue& it, mapObjects )
        {
            CSequenceObject* pObject = it.second;

            if( !pObject->bDrawFirst )
                continue;

            SequenceObjectVecCIter fi = std::find( vecSelected.begin(), vecSelected.end(), pObject );

            pObject->DrawObject(
                pRI,
                ( fi != vecSelected.end() ) ? TRUE : FALSE,
                ( pObject == pMouseOver ) ? TRUE : FALSE,
                nMouseOverConnType,
                nMouseOverConnIndex );
        }

        BOOST_FOREACH( SequenceObjectMapValue& it, mapObjects )
        {
            CSequenceObject* pObject = it.second;

            if( pObject->bDrawFirst || pObject->bDrawLast )
                continue;

            SequenceObjectVecCIter fi = std::find( vecSelected.begin(), vecSelected.end(), pObject );

            pObject->DrawObject(
                pRI,
                ( fi != vecSelected.end() ) ? TRUE : FALSE,
                ( pObject == pMouseOver ) ? TRUE : FALSE,
                nMouseOverConnType,
                nMouseOverConnIndex );
        }

        BOOST_FOREACH( SequenceObjectMapValue& it, mapObjects )
        {
            CSequenceObject* pObject = it.second;

            pObject->DrawLogicLinks( pRI );
            pObject->DrawVariableLinks( pRI );
        }

        BOOST_FOREACH( SequenceObjectMapValue& it, mapObjects )
        {
            CSequenceObject* pObject = it.second;

            if( !pObject->bDrawLast )
                continue;

            SequenceObjectVecCIter fi = std::find( vecSelected.begin(), vecSelected.end(), pObject );

            pObject->DrawObject(
                pRI,
                ( fi != vecSelected.end() ) ? TRUE : FALSE,
                ( pObject == pMouseOver ) ? TRUE : FALSE,
                nMouseOverConnType,
                nMouseOverConnIndex );
        }
    }

    VOID CSequence::OnChangedName()
    {
        MekeUniqueName( this );
    }
}