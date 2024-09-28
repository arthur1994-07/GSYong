#include "stdafx.h"

#include "./SequenceAction.h"

#include "../../InternalCommonLib/DebugInclude.h"

namespace vs
{
    VOID CSeqAct_FinishSequence::serialize( SERIALIZE_POARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceAction );

        ar & SERIALIZE_NVP( strLabel );
    }

    VOID CSeqAct_FinishSequence::serialize( SERIALIZE_PIARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceAction );

        ar & SERIALIZE_NVP( strLabel );
    }

    VOID CSeqAct_ActivateRemoteEvent::serialize( SERIALIZE_POARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceAction );

        ar & SERIALIZE_NVP( strLabel );
    }

    VOID CSeqAct_ActivateRemoteEvent::serialize( SERIALIZE_PIARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceAction );

        ar & SERIALIZE_NVP( strLabel );
    }

    VOID CSeqAct_SetBool::serialize( SERIALIZE_POARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceAction );

        ar & SERIALIZE_NVP( bDefaultValue );
    }

    VOID CSeqAct_SetBool::serialize( SERIALIZE_PIARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceAction );

        ar & SERIALIZE_NVP( bDefaultValue );
    }

    VOID CSeqAct_SetDouble::serialize( SERIALIZE_POARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceAction );

        ar & SERIALIZE_NVP( nDefaultValue );
    }

    VOID CSeqAct_SetDouble::serialize( SERIALIZE_PIARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceAction );

        ar & SERIALIZE_NVP( nDefaultValue );
    }

    VOID CSeqAct_SetString::serialize( SERIALIZE_POARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceAction );

        ar & SERIALIZE_NVP( strDefaultValue );
    }

    VOID CSeqAct_SetString::serialize( SERIALIZE_PIARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceAction );

        ar & SERIALIZE_NVP( strDefaultValue );
    }

    VOID CSeqAct_SetVector3::serialize( SERIALIZE_POARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceAction );

        ar & SERIALIZE_NVP( vDefaultValue.x );
        ar & SERIALIZE_NVP( vDefaultValue.y );
        ar & SERIALIZE_NVP( vDefaultValue.z );
    }

    VOID CSeqAct_SetVector3::serialize( SERIALIZE_PIARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceAction );

        ar & SERIALIZE_NVP( vDefaultValue.x );
        ar & SERIALIZE_NVP( vDefaultValue.y );
        ar & SERIALIZE_NVP( vDefaultValue.z );
    }

    VOID CSeqAct_CompareDouble::serialize( SERIALIZE_POARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceAction );

        ar & SERIALIZE_NVP( nDefaultA );
        ar & SERIALIZE_NVP( nDefaultB );
    }

    VOID CSeqAct_CompareDouble::serialize( SERIALIZE_PIARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceAction );

        ar & SERIALIZE_NVP( nDefaultA );
        ar & SERIALIZE_NVP( nDefaultB );
    }

    VOID CSeqAct_CompareString::serialize( SERIALIZE_POARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceAction );

        ar & SERIALIZE_NVP( strDefaultA );
        ar & SERIALIZE_NVP( strDefaultB );
    }

    VOID CSeqAct_CompareString::serialize( SERIALIZE_PIARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceAction );

        ar & SERIALIZE_NVP( strDefaultA );
        ar & SERIALIZE_NVP( strDefaultB );
    }

    VOID CSeqAct_CompareVector3::serialize( SERIALIZE_POARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceAction );

        ar & SERIALIZE_NVP( vDefaultA.x );
        ar & SERIALIZE_NVP( vDefaultA.y );
        ar & SERIALIZE_NVP( vDefaultA.z );
        ar & SERIALIZE_NVP( vDefaultB.x );
        ar & SERIALIZE_NVP( vDefaultB.y );
        ar & SERIALIZE_NVP( vDefaultB.z );
    }

    VOID CSeqAct_CompareVector3::serialize( SERIALIZE_PIARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceAction );

        ar & SERIALIZE_NVP( vDefaultA.x );
        ar & SERIALIZE_NVP( vDefaultA.y );
        ar & SERIALIZE_NVP( vDefaultA.z );
        ar & SERIALIZE_NVP( vDefaultB.x );
        ar & SERIALIZE_NVP( vDefaultB.y );
        ar & SERIALIZE_NVP( vDefaultB.z );
    }

    VOID CSeqAct_Switch::serialize( SERIALIZE_POARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceAction );

        ar & SERIALIZE_NVP( nLinkCount );
        ar & SERIALIZE_NVP( nIncrementAmount );
    }

    VOID CSeqAct_Switch::serialize( SERIALIZE_PIARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceAction );

        ar & SERIALIZE_NVP( nLinkCount );
        ar & SERIALIZE_NVP( nIncrementAmount );
    }

    VOID CSeqAct_OutputString::serialize( SERIALIZE_POARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceAction );

        ar & SERIALIZE_NVP( nTargets );
    }

    VOID CSeqAct_OutputString::serialize( SERIALIZE_PIARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceAction );

        ar & SERIALIZE_NVP( nTargets );
    }

    VOID CSeqAct_SetProperty::serialize( SERIALIZE_POARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceAction );

        ar & SERIALIZE_NVP( strKey );
    }

    VOID CSeqAct_SetProperty::serialize( SERIALIZE_PIARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceAction );

        ar & SERIALIZE_NVP( strKey );
    }

    VOID CSeqAct_GetProperty::serialize( SERIALIZE_POARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceAction );

        ar & SERIALIZE_NVP( strKey );
    }

    VOID CSeqAct_GetProperty::serialize( SERIALIZE_PIARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceAction );

        ar & SERIALIZE_NVP( strKey );
    }

    VOID CSeqAct_Function::serialize( SERIALIZE_POARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceAction );

        ar & SERIALIZE_NVP( strKey );
    }

    VOID CSeqAct_Function::serialize( SERIALIZE_PIARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceAction );

        ar & SERIALIZE_NVP( strKey );
    }
}