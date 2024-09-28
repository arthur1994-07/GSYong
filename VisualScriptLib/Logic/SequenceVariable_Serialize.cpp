#include "stdafx.h"

#include "./SequenceVariable.h"

#include "../../InternalCommonLib/DebugInclude.h"

namespace vs
{
    VOID CSequenceVariable::serialize( SERIALIZE_POARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceObject );

        ar & SERIALIZE_NVP( strVarName );
    }

    VOID CSequenceVariable::serialize( SERIALIZE_PIARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceObject );

        ar & SERIALIZE_NVP( strVarName );
    }

    VOID CSeqVar_Bool::serialize( SERIALIZE_POARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceVariable );

        ar & SERIALIZE_NVP( bValue );
    }

    VOID CSeqVar_Bool::serialize( SERIALIZE_PIARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceVariable );

        ar & SERIALIZE_NVP( bValue );
    }

    VOID CSeqVar_Double::serialize( SERIALIZE_POARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceVariable );

        ar & SERIALIZE_NVP( nValue );
    }

    VOID CSeqVar_Double::serialize( SERIALIZE_PIARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceVariable );

        ar & SERIALIZE_NVP( nValue );
    }

    VOID CSeqVar_String::serialize( SERIALIZE_POARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceVariable );

        ar & SERIALIZE_NVP( strValue );
    }

    VOID CSeqVar_String::serialize( SERIALIZE_PIARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceVariable );

        ar & SERIALIZE_NVP( strValue );
    }

    VOID CSeqVar_Vector3::serialize( SERIALIZE_POARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceVariable );

        ar & SERIALIZE_NVP( vValue.x );
        ar & SERIALIZE_NVP( vValue.y );
        ar & SERIALIZE_NVP( vValue.z );
    }

    VOID CSeqVar_Vector3::serialize( SERIALIZE_PIARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceVariable );

        ar & SERIALIZE_NVP( vValue.x );
        ar & SERIALIZE_NVP( vValue.y );
        ar & SERIALIZE_NVP( vValue.z );
    }

    VOID CSeqVar_Object::serialize( SERIALIZE_POARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceVariable );
    }

    VOID CSeqVar_Object::serialize( SERIALIZE_PIARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceVariable );
    }

    VOID CSeqVar_Named::serialize( SERIALIZE_POARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceVariable );

        TSTRING strExpected;
        if( pExpected )
            strExpected = pExpected->VName();

        ar & SERIALIZE_NVP( strExpected );
        ar & SERIALIZE_NVP( strFindVarName );
    }

    VOID CSeqVar_Named::serialize( SERIALIZE_PIARCHIVE& ar, const unsigned int version )
    {
        ar & SERIALIZE_BASE_NVP( CSequenceVariable );

        TSTRING strExpected;
        ar & SERIALIZE_NVP( strExpected );

        if( !strExpected.empty() )
            pExpected = ic::CRttiMgr::FindStatic( strExpected );

        ar & SERIALIZE_NVP( strFindVarName );
    }
}