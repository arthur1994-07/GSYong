#pragma once

//--------------------------------------------------------------------------------------------//

#include <string>
#include <boost/tuple/tuple.hpp>
#include <boost/ref.hpp>

//--------------------------------------------------------------------------------------------//

namespace NSUIINFOCFG
{
    // ATTR --------------------------------------------------
    template <typename T>
    class CATTR
    {
    private:
        typedef boost::tuple<std::string, boost::reference_wrapper<T> > _TYPE;
        _TYPE _DATA;

    public:
        CATTR ( const std::string& _strID, const T& _Value )
            : _DATA( _strID, boost::reference_wrapper<T>(const_cast<T&>(_Value)) )
        {
        }

    public:
        std::string& GetID    () { return boost::tuples::get<0>(_DATA); }
        T&           GetValue () { return boost::tuples::get<1>(_DATA); }

        const std::string& GetcID    () const { return boost::tuples::get<0>(_DATA); }
        const  T&          GetcValue () const { return boost::tuples::get<1>(_DATA); }
    };

    template <typename T> inline
    NSUIINFOCFG::CATTR<T> ATTR( const std::string& _strID, const T& _Value )
    {
        return NSUIINFOCFG::CATTR<T>(_strID, _Value);
    }

    // ATTRLIST --------------------------------------------------
    typedef boost::tuples::null_type ATTRNULL;
    template<int N> struct NType {};

    template <
        typename ATTR0=ATTRNULL, 
        typename ATTR1=ATTRNULL, 
        typename ATTR2=ATTRNULL, 
        typename ATTR3=ATTRNULL, 
        typename ATTR4=ATTRNULL, 
        typename ATTR5=ATTRNULL, 
        typename ATTR6=ATTRNULL
    >
    class CATTRLIST
    {
    public:
        typedef boost::tuple<ATTR0, ATTR1, ATTR2, ATTR3, ATTR4, ATTR5, ATTR6>                   TUPLE;

    public:
        template<int N> struct                                                                  ELEMENT 
        { typedef typename boost::tuples::element<N, TUPLE>::type type; };

        template<typename T> struct                                                             ACCESS_TRAITS
        { typedef typename boost::tuples::access_traits<T>::parameter_type parameter_type; };

    public:
        static const int Length = boost::tuples::length<TUPLE>::value;

    public:
        CATTRLIST ( typename ACCESS_TRAITS<ATTR0>::parameter_type a0 ) 
                    : _DATA(a0) {}

        CATTRLIST ( typename ACCESS_TRAITS<ATTR0>::parameter_type a0, 
                    typename ACCESS_TRAITS<ATTR1>::parameter_type a1 )                                                                                  
                    : _DATA(a0,a1) {}

        CATTRLIST ( typename ACCESS_TRAITS<ATTR0>::parameter_type a0, 
                    typename ACCESS_TRAITS<ATTR1>::parameter_type a1, 
                    typename ACCESS_TRAITS<ATTR2>::parameter_type a2 )                                                                       
                    : _DATA(a0,a1,a2) {}

        CATTRLIST ( typename ACCESS_TRAITS<ATTR0>::parameter_type a0, 
                    typename ACCESS_TRAITS<ATTR1>::parameter_type a1,
                    typename ACCESS_TRAITS<ATTR2>::parameter_type a2, 
                    typename ACCESS_TRAITS<ATTR3>::parameter_type a3 )                                                      
                    : _DATA(a0,a1,a2,a3) {}

        CATTRLIST ( typename ACCESS_TRAITS<ATTR0>::parameter_type a0, 
                    typename ACCESS_TRAITS<ATTR1>::parameter_type a1, 
                    typename ACCESS_TRAITS<ATTR2>::parameter_type a2, 
                    typename ACCESS_TRAITS<ATTR3>::parameter_type a3, 
                    typename ACCESS_TRAITS<ATTR4>::parameter_type a4 )                                     
                    : _DATA(a0,a1,a2,a3,a4) {}

        CATTRLIST ( typename ACCESS_TRAITS<ATTR0>::parameter_type a0, 
                    typename ACCESS_TRAITS<ATTR1>::parameter_type a1, 
                    typename ACCESS_TRAITS<ATTR2>::parameter_type a2, 
                    typename ACCESS_TRAITS<ATTR3>::parameter_type a3, 
                    typename ACCESS_TRAITS<ATTR4>::parameter_type a4, 
                    typename ACCESS_TRAITS<ATTR5>::parameter_type a5 )                    
                    : _DATA(a0,a1,a2,a3,a4,a5) {}

        CATTRLIST ( typename ACCESS_TRAITS<ATTR0>::parameter_type a0, 
                    typename ACCESS_TRAITS<ATTR1>::parameter_type a1, 
                    typename ACCESS_TRAITS<ATTR2>::parameter_type a2, 
                    typename ACCESS_TRAITS<ATTR3>::parameter_type a3, 
                    typename ACCESS_TRAITS<ATTR4>::parameter_type a4, 
                    typename ACCESS_TRAITS<ATTR5>::parameter_type a5, 
                    typename ACCESS_TRAITS<ATTR6>::parameter_type a6 )   
                    : _DATA(a0,a1,a2,a3,a4,a5,a6) {}

    public:
        template< int N, typename _ATTR >
        _ATTR& GetAttr() { return boost::tuples::get<N>(_DATA); }

        template< int N, typename _ATTR >
        const _ATTR& GetAttr() const { return boost::tuples::get<N>(_DATA); }

    private:
        TUPLE _DATA;
    };

    template <typename ATTR0> inline
    NSUIINFOCFG::CATTRLIST<ATTR0> ATTRLIST(const ATTR0& a0)
    {
        return NSUIINFOCFG::CATTRLIST<ATTR0>(a0);
    }

    template <typename ATTR0,typename ATTR1> inline
    NSUIINFOCFG::CATTRLIST<ATTR0,ATTR1> ATTRLIST(const ATTR0& a0, const ATTR1& a1)
    {
        return NSUIINFOCFG::CATTRLIST<ATTR0,ATTR1>(a0,a1);
    }

    template <typename ATTR0,typename ATTR1,typename ATTR2> inline
    NSUIINFOCFG::CATTRLIST<ATTR0,ATTR1,ATTR2> ATTRLIST(const ATTR0& a0, const ATTR1& a1, const ATTR2& a2)
    {
        return NSUIINFOCFG::CATTRLIST<ATTR0,ATTR1,ATTR2>(a0,a1,a2);
    }

    template <typename ATTR0,typename ATTR1,typename ATTR2,typename ATTR3> inline
    NSUIINFOCFG::CATTRLIST<ATTR0,ATTR1,ATTR2,ATTR3> ATTRLIST(const ATTR0& a0, const ATTR1& a1, const ATTR2& a2, const ATTR3& a3)
    {
        return NSUIINFOCFG::CATTRLIST<ATTR0,ATTR1,ATTR2,ATTR3>(a0,a1,a2,a3);
    }

    template <typename ATTR0,typename ATTR1,typename ATTR2,typename ATTR3,typename ATTR4> inline
    NSUIINFOCFG::CATTRLIST<ATTR0,ATTR1,ATTR2,ATTR3,ATTR4> ATTRLIST(const ATTR0& a0, const ATTR1& a1, const ATTR2& a2, const ATTR3& a3, const ATTR3& a4)
    {
        return NSUIINFOCFG::CATTRLIST<ATTR0,ATTR1,ATTR2,ATTR3,ATTR4>(a0,a1,a2,a3,a4);
    }

    template <typename ATTR0,typename ATTR1,typename ATTR2,typename ATTR3,typename ATTR4,typename ATTR5> inline
    NSUIINFOCFG::CATTRLIST<ATTR0,ATTR1,ATTR2,ATTR3,ATTR4,ATTR5> ATTRLIST(const ATTR0& a0, const ATTR1& a1, const ATTR2& a2, const ATTR3& a3, const ATTR4& a4, const ATTR5& a5)
    {
        return NSUIINFOCFG::CATTRLIST<ATTR0,ATTR1,ATTR2,ATTR3,ATTR4,ATTR5>(a0,a1,a2,a3,a4,a5);
    }

    template <typename ATTR0,typename ATTR1,typename ATTR2,typename ATTR3,typename ATTR4,typename ATTR5,typename ATTR6> inline
    NSUIINFOCFG::CATTRLIST<ATTR0,ATTR1,ATTR2,ATTR3,ATTR4,ATTR5,ATTR6> ATTRLIST(const ATTR0& a0, const ATTR1& a1, const ATTR2& a2, const ATTR3& a3, const ATTR4& a4, const ATTR5& a5, const ATTR6& a6)
    {
        return NSUIINFOCFG::CATTRLIST<ATTR0,ATTR1,ATTR2,ATTR3,ATTR4,ATTR5,ATTR6>(a0,a1,a2,a3,a4,a5,a6);
    }

};
