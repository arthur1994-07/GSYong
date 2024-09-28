#include "stdafx.h"

#include "./SequenceEvent.h"

#include "../../InternalCommonLib/DebugInclude.h"

namespace vs
{
    VOID CSequenceEvent::serialize( SERIALIZE_POARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceOp );

        ar & SERIALIZE_NVP( bEnabled );
        ar & SERIALIZE_NVP( nMaxTriggerCount );
        ar & SERIALIZE_NVP( fReTriggerDelay );
    }

    VOID CSequenceEvent::serialize( SERIALIZE_PIARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceOp );

        ar & SERIALIZE_NVP( bEnabled );
        ar & SERIALIZE_NVP( nMaxTriggerCount );
        ar & SERIALIZE_NVP( fReTriggerDelay );
    }

    VOID CSeqEvent_SequenceActivated::serialize( SERIALIZE_POARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceEvent );

        ar & SERIALIZE_NVP( strLabel );
    }

    VOID CSeqEvent_SequenceActivated::serialize( SERIALIZE_PIARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceEvent );

        ar & SERIALIZE_NVP( strLabel );
    }

    VOID CSeqEvent_RemoteEvent::serialize( SERIALIZE_POARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceEvent );

        ar & SERIALIZE_NVP( strLabel );
    }

    VOID CSeqEvent_RemoteEvent::serialize( SERIALIZE_PIARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceEvent );

        ar & SERIALIZE_NVP( strLabel );
    }
}