/**
 * \date	2011/03/29
 * \author	Jun-Hyuk Choi
 */
#pragma once

#include <boost/tr1/memory.hpp>

namespace vs
{
    template< class TArchive >
    class CSequenceArchive
    {
    private:
        std::tr1::shared_ptr< TArchive > m_spArchive;

    public:
        template< class T >
        VOID Open( T& cStream, DWORD nFlags = 0 )
        {
            m_spArchive = std::tr1::shared_ptr< TArchive >( new TArchive( cStream, nFlags ) );

            TArchive& cArchive = *m_spArchive.get();
            IMPLEMENT_HELP_SEQUENCE_ALL( REGIST, cArchive );
        }

        template< class T >
        CSequenceArchive& operator & ( T& pData )
        {
            m_spArchive->operator & ( pData );
            return *this;
        }
    };

    typedef CSequenceArchive< SERIALIZE_PIARCHIVE_TEXT > polymorphic_text_iarchive;
    typedef CSequenceArchive< SERIALIZE_POARCHIVE_TEXT > polymorphic_text_oarchive;
    typedef CSequenceArchive< SERIALIZE_PIARCHIVE_XML > polymorphic_xml_iarchive;
    typedef CSequenceArchive< SERIALIZE_POARCHIVE_XML > polymorphic_xml_oarchive;
}