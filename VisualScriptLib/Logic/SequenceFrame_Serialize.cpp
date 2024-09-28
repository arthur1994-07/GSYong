#include "stdafx.h"

#include "./SequenceFrame.h"

#include "../../InternalCommonLib/DebugInclude.h"

namespace vs
{
    VOID CSequenceFrame::serialize( SERIALIZE_POARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceObject );

        ar & SERIALIZE_NVP( sSize.x );
        ar & SERIALIZE_NVP( sSize.y );
        ar & SERIALIZE_NVP( nBorderWidth );
        ar & SERIALIZE_NVP( bDrawBox );
        ar & SERIALIZE_NVP( bFilled );
        ar & SERIALIZE_NVP( bTileFill );
        ar & SERIALIZE_NVP( nBorderColor );
        ar & SERIALIZE_NVP( nFillColor );
    }

    VOID CSequenceFrame::serialize( SERIALIZE_PIARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceObject );

        ar & SERIALIZE_NVP( sSize.x );
        ar & SERIALIZE_NVP( sSize.y );
        ar & SERIALIZE_NVP( nBorderWidth );
        ar & SERIALIZE_NVP( bDrawBox );
        ar & SERIALIZE_NVP( bFilled );
        ar & SERIALIZE_NVP( bTileFill );
        ar & SERIALIZE_NVP( nBorderColor );
        ar & SERIALIZE_NVP( nFillColor );
    }
}