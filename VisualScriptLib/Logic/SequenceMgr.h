/**
 * \date	2011/03/29
 * \author	Jun-Hyuk Choi
 */
#pragma once

#include "./SequenceObject.h"

namespace vs
{
    class CSequence;

    class CSequenceMgr
    {
    public:
        static BOOL Save( CSequence* pSequence, const TSTRING& strPath );
        static BOOL Load( CSequence*& pSequence, const TSTRING& strPath );

        static VOID OStream( TSTRING& strStream, const SequenceObjectVec& vecObjects );
        static VOID IStream( const TSTRING& strStream, SequenceObjectVec& vecObjects );
    };
}