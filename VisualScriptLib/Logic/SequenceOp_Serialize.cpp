#include "stdafx.h"

#include "./SequenceEvent.h"
#include "./SequenceVariable.h"
#include "./SequenceOp.h"

#include "../../InternalCommonLib/DebugInclude.h"

namespace vs
{
    VOID SSeqOpInputLink::serialize( SERIALIZE_POARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_NVP( strLinkDesc );
        ar & SERIALIZE_NVP( strLinkAction );
        ar & SERIALIZE_NVP( fDelay );
    }

    VOID SSeqOpInputLink::serialize( SERIALIZE_PIARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_NVP( strLinkDesc );
        ar & SERIALIZE_NVP( strLinkAction );
        ar & SERIALIZE_NVP( fDelay );
    }

    VOID SSeqOpOutputInputLink::serialize( SERIALIZE_POARCHIVE& ar, const unsigned int version )
    {
        DWORD nIdentity = pLinkedOp->GetIdentityForIO();
        ar & SERIALIZE_NVP( nIdentity );
        ar & SERIALIZE_NVP( nInputLinkIndex );
    }

    VOID SSeqOpOutputInputLink::serialize( SERIALIZE_PIARCHIVE& ar, const unsigned int version )
    {
        DWORD nIdentity;
        ar & SERIALIZE_NVP( nIdentity );
        pLinkedOp = reinterpret_cast< CSequenceOp* >( nIdentity );

        ar & SERIALIZE_NVP( nInputLinkIndex );
    }

    VOID SSeqOpOutputLink::serialize( SERIALIZE_POARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_NVP( strLinkDesc );
        ar & SERIALIZE_NVP( strLinkAction );
        ar & SERIALIZE_NVP( fDelay );
        ar & SERIALIZE_NVP( vecLinks );
    }

    VOID SSeqOpOutputLink::serialize( SERIALIZE_PIARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_NVP( strLinkDesc );
        ar & SERIALIZE_NVP( strLinkAction );
        ar & SERIALIZE_NVP( fDelay );
        ar & SERIALIZE_NVP( vecLinks );
    }

    VOID SSeqVarLink::serialize( SERIALIZE_POARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_NVP( strLinkDesc );
        ar & SERIALIZE_NVP( strLinkVar );
        ar & SERIALIZE_NVP( nMaxVars );
        ar & SERIALIZE_NVP( bWriteable );

        TSTRING strExpected;
        if( pExpected )
            strExpected = pExpected->VName();

        ar & SERIALIZE_NVP( strExpected );

        size_t nSize = vecLinks.size();
        ar & SERIALIZE_NVP( nSize );

        BOOST_FOREACH( const CSequenceVariable* pVar, vecLinks )
        {
            DWORD nIdentity = pVar->GetIdentityForIO();
            ar & SERIALIZE_NVP( nIdentity );
        }
    }

    VOID SSeqVarLink::serialize( SERIALIZE_PIARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_NVP( strLinkDesc );
        ar & SERIALIZE_NVP( strLinkVar );
        ar & SERIALIZE_NVP( nMaxVars );
        ar & SERIALIZE_NVP( bWriteable );

        TSTRING strExpected;
        ar & SERIALIZE_NVP( strExpected );

        if( !strExpected.empty() )
            pExpected = ic::CRttiMgr::FindStatic( strExpected );

        size_t nSize;
        ar & SERIALIZE_NVP( nSize );

        for( size_t j=0; j<nSize; ++j )
        {
            DWORD nIdentity;
            ar & SERIALIZE_NVP( nIdentity );

            CSequenceVariable* pLink = reinterpret_cast< CSequenceVariable* >( nIdentity );
            vecLinks.push_back( pLink );
        }
    }

    VOID CSequenceOp::serialize( SERIALIZE_POARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceObject );

        ar & SERIALIZE_NVP( vecInputLinks );
        ar & SERIALIZE_NVP( vecOutputLinks );
        ar & SERIALIZE_NVP( vecVariableLinks );
    }

    VOID CSequenceOp::serialize( SERIALIZE_PIARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceObject );

        ar & SERIALIZE_NVP( vecInputLinks );
        ar & SERIALIZE_NVP( vecOutputLinks );
        ar & SERIALIZE_NVP( vecVariableLinks );
    }
}