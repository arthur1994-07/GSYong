#ifndef _SHARED_MEMORY_POOL_H_
#define _SHARED_MEMORY_POOL_H_

#pragma once

#include <boost/tr1/memory.hpp>
#include <boost/pool/object_pool.hpp>
#include "../Util/Lock.h"

namespace sc
{
    //! 2011-08-12 jgkim
    //! Singletone +  Shared Memory + Memory Pool + Lock
    template <class T, class LOCK_TYPE=sc::NoLock>
    class SharedPtrFactorySingletone
    {
    public:        
        SharedPtrFactorySingletone* SharedPtrFactorySingletone::getInstance()
        {
            if (!SelfInstance)
                SelfInstance = new SharedPtrFactorySingletone;
            return SelfInstance;
        }
        
        void SharedPtrFactorySingletone::releaseInstance()
        {
            delete SelfInstance;
            SelfInstance = NULL;
        }

    private:
        SharedPtrFactorySingletone() {}
        ~SharedPtrFactorySingletone() {}
        
        static SharedPtrFactorySingletone<T>* SelfInstance;
        boost::object_pool<T> m_allocator;
        LOCK_TYPE m_Lock;

    public:
        std::tr1::shared_ptr<T> create()
        {
            return std::tr1::shared_ptr<T>(
                m_allocator.construct(),
                boost::bind(&SharedPtrFactorySingletone::destroy, this, _1));
        }

        void destroy(T* p)
        {
            m_allocator.destroy(p);
        }
    };
 
    //! 2011-08-12 jgkim
    //! Shared Memory + Memory Pool + Lock
    template <class T, class LOCK_TYPE=sc::NoLock>
    class SharedPtrFactory
    {
    public:        
        SharedPtrFactory() {}
        ~SharedPtrFactory() {}

    private:
        boost::object_pool<T> m_allocator;
        LOCK_TYPE m_Lock;

    public:
        std::tr1::shared_ptr<T> create()
        {
            return std::tr1::shared_ptr<T>(
                m_allocator.construct(),
                boost::bind(&SharedPtrFactory::destroy, this, _1));
        }

        void destroy(T* p)
        {
            m_allocator.destroy(p);
        }
    };
   

} // namespace sc
#endif // _SHARED_MEMORY_POOL_H_