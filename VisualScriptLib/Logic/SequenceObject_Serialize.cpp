#include "stdafx.h"

#include "./Sequence.h"
#include "./SequenceObject.h"

#include "../../InternalCommonLib/DebugInclude.h"

namespace vs
{
    VOID CSequenceObject::serialize( SERIALIZE_POARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_NVP( strName );
        ar & SERIALIZE_NVP( strComment );
        ar & SERIALIZE_NVP( sPos.x );
        ar & SERIALIZE_NVP( sPos.y );
        ar & SERIALIZE_NVP( nColor );
        ar & SERIALIZE_NVP( bDrawFirst );
        ar & SERIALIZE_NVP( bDrawLast );
    }

    VOID CSequenceObject::serialize( SERIALIZE_PIARCHIVE& ar, const unsigned int version )
    {
        TSTRING strName_ = strName;

        ar & SERIALIZE_NVP( strName );
        ar & SERIALIZE_NVP( strComment );
        ar & SERIALIZE_NVP( sPos.x );
        ar & SERIALIZE_NVP( sPos.y );
        ar & SERIALIZE_NVP( nColor );
        ar & SERIALIZE_NVP( bDrawFirst );
        ar & SERIALIZE_NVP( bDrawLast );

        if( !IsA( CSequence::Rtti() ) )
            strName = strName_;
    }
}