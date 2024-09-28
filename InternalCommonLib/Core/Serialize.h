/**
 * \date	2011/01/27
 * \author	Jun-Hyuk Choi
 */
#pragma once

#include <boost/archive/polymorphic_text_iarchive.hpp>
#include <boost/archive/polymorphic_text_oarchive.hpp>
#include <boost/archive/polymorphic_xml_iarchive.hpp>
#include <boost/archive/polymorphic_xml_oarchive.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/version.hpp>

#define SERIALIZE_POARCHIVE boost::archive::polymorphic_oarchive
#define SERIALIZE_PIARCHIVE boost::archive::polymorphic_iarchive
#define SERIALIZE_POARCHIVE_TEXT boost::archive::polymorphic_text_oarchive
#define SERIALIZE_PIARCHIVE_TEXT boost::archive::polymorphic_text_iarchive
#define SERIALIZE_POARCHIVE_XML boost::archive::polymorphic_xml_oarchive
#define SERIALIZE_PIARCHIVE_XML boost::archive::polymorphic_xml_iarchive

#define SERIALIZE_ACCESS \
    friend class boost::serialization::access

#define SERIALIZE_VERSION( TClass ) \
    BOOST_CLASS_VERSION( TClass, TClass::EVersion )

#define SERIALIZE_REGIST( TArchive, TClass ) \
    TArchive.register_type< TClass >();

#define SERIALIZE_BASE_NVP_TAG( TTag, TVariable ) \
    boost::serialization::make_nvp( \
        BOOST_PP_STRINGIZE( TTag ), \
        boost::serialization::base_object< TVariable >( *this ) )

#define SERIALIZE_BASE_NVP( TVariable ) \
    SERIALIZE_BASE_NVP_TAG( TVariable, TVariable )

#define SERIALIZE_NVP_TAG( TTag, TVariable ) \
    boost::serialization::make_nvp( \
        BOOST_PP_STRINGIZE( TTag ), \
        TVariable )

#define SERIALIZE_NVP( TVariable ) \
    SERIALIZE_NVP_TAG( TVariable, TVariable )

#define SERIALIZE_EMPTY_TAG( TTag, TVariable ) \
    SERIALIZE_ACCESS; \
    VOID serialize( SERIALIZE_POARCHIVE& ar, const unsigned int version ) \
    { \
        ar & SERIALIZE_BASE_NVP_TAG( TTag, TVariable ); \
    } \
    VOID serialize( SERIALIZE_PIARCHIVE& ar, const unsigned int version ) \
    { \
        ar & SERIALIZE_BASE_NVP_TAG( TTag, TVariable ); \
    } \
    VOID serialize( SERIALIZE_POARCHIVE_TEXT& ar, const unsigned int version ) {} \
    VOID serialize( SERIALIZE_PIARCHIVE_TEXT& ar, const unsigned int version ) {} \
    VOID serialize( SERIALIZE_POARCHIVE_XML& ar, const unsigned int version ) {} \
    VOID serialize( SERIALIZE_PIARCHIVE_XML& ar, const unsigned int version ) {}

#define SERIALIZE_EMPTY( TVariable ) \
    SERIALIZE_EMPTY_TAG( TVariable, TVariable )

#define SERIALIZE_DEFAULT \
    SERIALIZE_ACCESS; \
    VOID serialize( SERIALIZE_POARCHIVE& ar, const unsigned int version ); \
    VOID serialize( SERIALIZE_PIARCHIVE& ar, const unsigned int version ); \
    VOID serialize( SERIALIZE_POARCHIVE_TEXT& ar, const unsigned int version ) {} \
    VOID serialize( SERIALIZE_PIARCHIVE_TEXT& ar, const unsigned int version ) {} \
    VOID serialize( SERIALIZE_POARCHIVE_XML& ar, const unsigned int version ) {} \
    VOID serialize( SERIALIZE_PIARCHIVE_XML& ar, const unsigned int version ) {}