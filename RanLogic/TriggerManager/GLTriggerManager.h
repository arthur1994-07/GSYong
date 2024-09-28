#pragma once

#include <map>
#include <vector>
#include <boost/function.hpp>

// --------------------------------------------------------------------------------------------------------------------------//

namespace triggermanager
{
    //! 특정조건에 발동
    enum EMTRIGGER_TYPE
    {
        EMTRIGGER_TYPE_NULL = 0,              // 없음
        EMTRIGGER_TYPE_CONNECT_TO_FIELD,      // 캐릭터 최초 접속시에
        EMTRIGGER_TYPE_CTF_BEGIN,             // 필드 점령전 시작시에
        EMTRIGGER_TYPE_CTF_END,               // 필드 점령전이 끝났을때
        EMTRIGGER_TYPE_CTF_NEW_CHARACTER,     // 필드 점령전에 유저가 들어왔을때
        EMTRIGGER_TYPE_CTF_QUIT_CHARACTER,    // 필드 점령전에 유저가 나갔을때
        EMTRIGGER_TYPE_CTF_CHANGED_USERCOUNT, // 필드 점령전의 유저 숫자가 바뀌었을때
		EMTRIGGER_TYPE_CTF_REBIRTH_CHARACTER, // 필드 점령전에서 유저가 부활할 때
    };

    // --------------------------------------------------------------------------------------------------------------------------//

    class GLTriggerManager
    {
    public:
        GLTriggerManager() ;
        virtual ~GLTriggerManager() = 0;

    };

    // --------------------------------------------------------------------------------------------------------------------------//

    struct NULLARG {};

    template<typename A1, typename A2, typename A3, typename A4>
    struct STriggerListArgumentTraits
    {
        typedef void (FNSIGNATURE)(A1,A2,A3,A4);
        typedef boost::function<FNSIGNATURE> FNCALLBACK;

        template<typename T>
        FNCALLBACK Bind( T& Functor )
        {
            return boost::bind( Functor, _1, _2, _3, _4 );
        }
    };

    template<typename A1, typename A2, typename A3>
    struct STriggerListArgumentTraits<A1,A2,A3,NULLARG>
    {
        typedef void (FNSIGNATURE)(A1,A2,A3);
        typedef boost::function<FNSIGNATURE> FNCALLBACK;

        template<typename T>
        FNCALLBACK Bind( T& Functor )
        {
            return boost::bind( Functor, _1, _2, _3 );
        }
    };

    template<typename A1, typename A2>
    struct STriggerListArgumentTraits<A1,A2,NULLARG,NULLARG>
    {
        typedef void (FNSIGNATURE)(A1,A2);
        typedef boost::function<FNSIGNATURE> FNCALLBACK;

        template<typename T>
        FNCALLBACK Bind( T& Functor )
        {
            return boost::bind( Functor, _1, _2 );
        }
    };

    template<typename A1>
    struct STriggerListArgumentTraits<A1,NULLARG,NULLARG,NULLARG>
    {
        typedef void (FNSIGNATURE)(A1);
        typedef boost::function<FNSIGNATURE> FNCALLBACK;

        template<typename T>
        FNCALLBACK Bind( T& Functor )
        {
            return boost::bind( Functor, _1 );
        }
    };

    template<>
    struct STriggerListArgumentTraits<NULLARG,NULLARG,NULLARG,NULLARG>
    {
        typedef void (FNSIGNATURE)();
        typedef boost::function<FNSIGNATURE> FNCALLBACK;

        template<typename T>
        FNCALLBACK Bind( T& Functor )
        {
            return boost::bind( Functor );
        }
    };

    typedef UINT STRIGGERKEY;
    const UINT STRIGGERKEY_NULL = UINT_MAX;

    template<EMTRIGGER_TYPE EMTYPE, typename A1=NULLARG, typename A2=NULLARG, typename A3=NULLARG, typename A4=NULLARG>
    class CTriggerList : private STriggerListArgumentTraits<A1,A2,A3,A4>
    {

    private:
        typedef typename STriggerListArgumentTraits<A1,A2,A3,A4>::FNSIGNATURE FNSIGNATURE;
        typedef typename STriggerListArgumentTraits<A1,A2,A3,A4>::FNCALLBACK  FNCALLBACK;
        typedef std::map<STRIGGERKEY,FNCALLBACK>                              MAP_FNCALLBACK;
        typedef typename MAP_FNCALLBACK::iterator                             MAP_FNCALLBACK_ITER;
        MAP_FNCALLBACK m_map;

    private:
        STRIGGERKEY m_CurKey;

    public:
        CTriggerList() : m_CurKey(0)
        {}

    public:
        EMTRIGGER_TYPE GetType() { return EMTYPE; }

    public:
        STRIGGERKEY Register( FNCALLBACK fnCallback )
        {
            if ( fnCallback.empty() )
            {
                return STRIGGERKEY_NULL;
            }

            STRIGGERKEY nKey = m_CurKey++;
            m_map.insert( std::make_pair(nKey, fnCallback) );
            return nKey;
        }

        template<typename T>
        STRIGGERKEY Register( T& Functor ) 
        {
            return Register( STriggerListArgumentTraits<A1,A2,A3,A4>::Bind( Functor ) );
        }

        const bool Erase( const STRIGGERKEY& Key ) 
        {
            MAP_FNCALLBACK_ITER finditer = m_map.find( Key );

            if ( finditer == m_map.end() )
            {
                return false;
            }

            m_map.erase( finditer );
            return true;
        }

        void Do()
        {
            MAP_FNCALLBACK_ITER iter = m_map.begin();
            for ( ; iter != m_map.end(); ++iter )
            {
                (iter->second)();
            }
        }

        template<typename A1>
        void Do( A1 a1 )
        {
            MAP_FNCALLBACK_ITER iter = m_map.begin();
            for ( ; iter != m_map.end(); ++iter )
            {
                (iter->second)( a1 );
            }
        }

        template<typename A1, typename A2>
        void Do( A1 a1, A2 a2 )
        {
            MAP_FNCALLBACK_ITER iter = m_map.begin();
            for ( ; iter != m_map.end(); ++iter )
            {
                (iter->second)( a1, a2 );
            }
        }

        template<typename A1, typename A2, typename A3>
        void Do( A1 a1, A2 a2, A3 a3 )
        {
            MAP_FNCALLBACK_ITER iter = m_map.begin();
            for ( ; iter != m_map.end(); ++iter )
            {
                (iter->second)( a1, a2, a3 );
            }
        }

        template<typename A1, typename A2, typename A3, typename A4>
        void Do( A1 a1, A2 a2, A3 a3, A4 a4 )
        {
            MAP_FNCALLBACK_ITER iter = m_map.begin();
            for ( ; iter != m_map.end(); ++iter )
            {
                (iter->second)( a1, a2, a3, a4 );
            }
        }

    };

    //template<EMTRIGGER_TYPE EMTYPE, typename A1>
    //class CTriggerList : public ITriggerList
    //{
    //private:
    //    typedef boost::function<void (A1)> FNCALLBACK;
    //    typedef std::vector<FNCALLBACK>  VEC_FNCALLBACK;
    //    typedef VEC_FNCALLBACK::iterator VEC_FNCALLBACK_ITER;
    //    VEC_FNCALLBACK m_vec;

    //public:
    //    EMTRIGGER_TYPE GetType() { return EMTYPE; }

    //    void Register( FNCALLBACK fnCallback )
    //    {
    //        if ( !fnCallback.empty() )
    //            m_vec.push_back( fnCallback );
    //    }

    //    void Action( A1 a1 )
    //    {
    //        VEC_FNCALLBACK_ITER iter = m_vec.begin();
    //        for ( ; iter != m_vec.end(); ++iter )
    //        {
    //            (*iter)(a1);
    //        }
    //    }
    //};

    //template<EMTRIGGER_TYPE EMTYPE, typename A1, typename A2>
    //class CTriggerList : public ITriggerList
    //{
    //private:
    //    typedef boost::function<void (A1, A2)> FNCALLBACK;
    //    typedef std::vector<FNCALLBACK>  VEC_FNCALLBACK;
    //    typedef VEC_FNCALLBACK::iterator VEC_FNCALLBACK_ITER;
    //    VEC_FNCALLBACK m_vec;

    //public:
    //    EMTRIGGER_TYPE GetType() { return EMTYPE; }

    //    void Register( FNCALLBACK fnCallback )
    //    {
    //        if ( !fnCallback.empty() )
    //            m_vec.push_back( fnCallback );
    //    }

    //    void Action( A1 a1, A2 a2 )
    //    {
    //        VEC_FNCALLBACK_ITER iter = m_vec.begin();
    //        for ( ; iter != m_vec.end(); ++iter )
    //        {
    //            (*iter)(a1, a2);
    //        }
    //    }
    //};

    //template<EMTRIGGER_TYPE EMTYPE, typename A1, typename A2, typename A3>
    //class CTriggerList : public ITriggerList
    //{
    //private:
    //    typedef boost::function<void (A1, A2, A3)> FNCALLBACK;
    //    typedef std::vector<FNCALLBACK>  VEC_FNCALLBACK;
    //    typedef VEC_FNCALLBACK::iterator VEC_FNCALLBACK_ITER;
    //    VEC_FNCALLBACK m_vec;

    //public:
    //    EMTRIGGER_TYPE GetType() { return EMTYPE; }

    //    void Register( FNCALLBACK fnCallback )
    //    {
    //        if ( !fnCallback.empty() )
    //            m_vec.push_back( fnCallback );
    //    }

    //    void Action( A1 a1, A2 a2, A3 a3 )
    //    {
    //        VEC_FNCALLBACK_ITER iter = m_vec.begin();
    //        for ( ; iter != m_vec.end(); ++iter )
    //        {
    //            (*iter)(a1, a2, a3);
    //        }
    //    }
    //};

    //template<EMTRIGGER_TYPE EMTYPE, typename A1, typename A2, typename A3, typename A4>
    //class CTriggerList : public ITriggerList
    //{
    //private:
    //    EMTRIGGER_TYPE GetType() { return EMTYPE; }

    //    typedef boost::function<void (A1, A2, A3, A4)> FNCALLBACK;
    //    typedef std::vector<FNCALLBACK>  VEC_FNCALLBACK;
    //    typedef VEC_FNCALLBACK::iterator VEC_FNCALLBACK_ITER;
    //    VEC_FNCALLBACK m_vec;

    //public:
    //    void Register( FNCALLBACK fnCallback )
    //    {
    //        if ( !fnCallback.empty() )
    //            m_vec.push_back( fnCallback );
    //    }

    //    void Action( A1 a1, A2 a2, A3 a3, A4 a4 )
    //    {
    //        VEC_FNCALLBACK_ITER iter = m_vec.begin();
    //        for ( ; iter != m_vec.end(); ++iter )
    //        {
    //            (*iter)(a1, a2, a3, a4);
    //        }
    //    }
    //};

}
