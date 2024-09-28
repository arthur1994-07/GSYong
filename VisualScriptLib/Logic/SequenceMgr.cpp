#include "stdafx.h"

#include <strstream>
#include <boost/filesystem.hpp>

#include "./SequenceEvent.h"
#include "./SequenceVariable.h"
#include "./SequenceAction.h"
#include "./SequenceFrame.h"
#include "./Sequence.h"
#include "./SequenceImplement.h"
#include "./SequenceArchive.h"

#include "./SequenceMgr.h"

#include "../../InternalCommonLib/DebugInclude.h"

namespace vs
{
    const DWORD XmlFileStreamFlags = boost::archive::no_xml_tag_checking;
    const DWORD TxtFileStreamFlags = boost::archive::no_header;
    const DWORD StrStreamFlags = boost::archive::no_header;

    BOOL CSequenceMgr::Save( CSequence* pSequence, const TSTRING& strPath )
    {
        if( !pSequence )
            return FALSE;

        if( strPath.empty() )
            return FALSE;

        std::ofstream cStream( strPath.c_str() );
        if( !cStream.is_open() )
            return FALSE;

        pSequence->PrevSave();

        boost::filesystem::path cPath( strPath );
		if( cPath.extension().compare( std::string(".xml") ) == 0 )
        {
            polymorphic_xml_oarchive cArchive;
            cArchive.Open( cStream, XmlFileStreamFlags );

            cArchive & SERIALIZE_NVP_TAG( SeqRoot, pSequence );
        }
        else
        {
            polymorphic_text_oarchive cArchive;
            cArchive.Open( cStream, TxtFileStreamFlags );

            cArchive & SERIALIZE_NVP_TAG( SeqRoot, pSequence );
        }

        pSequence->PostSave();

        return TRUE;
    }

    BOOL CSequenceMgr::Load( CSequence*& pSequence, const TSTRING& strPath )
    {
        if( !pSequence )
            return FALSE;

        if( strPath.empty() )
            return FALSE;

        std::ifstream cStream( strPath.c_str() );
        if( !cStream.is_open() )
            return FALSE;

        SAFE_DELETE( pSequence );

        boost::filesystem::path cPath( strPath );
        if( cPath.extension().compare( std::string(".xml") ) == 0 )
        {
            polymorphic_xml_iarchive cArchive;
            cArchive.Open( cStream, XmlFileStreamFlags );

            cArchive & SERIALIZE_NVP_TAG( SeqRoot, pSequence );
        }
        else
        {
            polymorphic_text_iarchive cArchive;
            cArchive.Open( cStream, TxtFileStreamFlags );

            cArchive & SERIALIZE_NVP_TAG( SeqRoot, pSequence );
        }

        if( !pSequence )
            return TRUE;

        pSequence->PostLoad();

        return TRUE;
    }

    VOID CSequenceMgr::OStream( TSTRING& strStream, const SequenceObjectVec& vecObjects )
    {
        std::ostrstream cStream;
        {
            polymorphic_text_oarchive cArchive;
            cArchive.Open( cStream, StrStreamFlags );

            cArchive & vecObjects;
        }

        strStream = cStream.str();
    }

    VOID CSequenceMgr::IStream( const TSTRING& strStream, SequenceObjectVec& vecObjects )
    {
        std::istrstream cStream( strStream.c_str() );
        {
            polymorphic_text_iarchive cArchive;
            cArchive.Open( cStream, StrStreamFlags );

            cArchive & vecObjects;
        }
    }
}