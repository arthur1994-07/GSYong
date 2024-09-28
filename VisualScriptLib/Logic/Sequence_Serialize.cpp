#include "stdafx.h"

#include "./Sequence.h"

#include "../../InternalCommonLib/DebugInclude.h"

namespace vs
{
    VOID CSequence::serialize( SERIALIZE_POARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceOp );

        ar & SERIALIZE_NVP( bEnabled );
        ar & SERIALIZE_NVP( sDefaultView.x );
        ar & SERIALIZE_NVP( sDefaultView.y );
        ar & SERIALIZE_NVP( fDefaultViewZoom );

        SequenceObjectVec vecList;

        BOOST_FOREACH( const SequenceObjectMapValue& it, mapObjects )
            vecList.push_back( it.second );

        std::sort( vecList.begin(), vecList.end(), CompSequenceObject() );

        size_t nSize = vecList.size();
        ar & SERIALIZE_NVP( nSize );

        BOOST_FOREACH( const CSequenceObject* pObject, vecList )
        {
            ar & SERIALIZE_NVP_TAG( SeqChild, pObject );

            DWORD nIdentity = pObject->GetIdentityForIO();
            ar & SERIALIZE_NVP( nIdentity );
        }
    }

    VOID CSequence::serialize( SERIALIZE_PIARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceOp );

        ar & SERIALIZE_NVP( bEnabled );
        ar & SERIALIZE_NVP( sDefaultView.x );
        ar & SERIALIZE_NVP( sDefaultView.y );
        ar & SERIALIZE_NVP( fDefaultViewZoom );

        size_t nSize = 0;
        ar & SERIALIZE_NVP( nSize );

        for( size_t i=0; i<nSize; ++i )
        {
            CSequenceObject* pObject = NULL;
            ar & SERIALIZE_NVP_TAG( SeqChild, pObject );

            DWORD nIdentity = 0;
            ar & SERIALIZE_NVP( nIdentity );

            if( pObject )
                pObject->SetIdentityForIO( nIdentity );

            InsertObject( pObject );
        }
    }
}