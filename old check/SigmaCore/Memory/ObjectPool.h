#ifndef _SC_OBJECT_POOL_H_
#define _SC_OBJECT_POOL_H_

#pragma once

#include <boost/pool/object_pool.hpp>
#include <boost/pool/pool.hpp>

#include "../Log/LogMan.h"
#include "../Util/SystemInfo.h"
#include "../Container/MinTr1.h"
#include "../Util/Lock.h"
#include "../gassert.h"

#ifndef SAFE_DELETE
#define SAFE_DELETE(p) { delete(p); (p)=NULL; }
#endif

namespace sc
{
	//! 2015-01-26
	//! shhan
	//! pool �� ��������� destroy �� ����� ���־����� Ȯ���ϱ����� �ѹ� ���մ�.
	//! Ÿ������ �޸𸮰� ���������� �����ϴ� ������ ã�� ����.
	template < class T >
	class BOOST_OBJECT_POOL_CHECKER
	{
	private:
		boost::object_pool< T > m_sObjcetPool;
		std::string				m_strPoolName;
		int						m_nUsedObject;
		int						m_nMaxObject;

	public:
		T* construct()
		{
			++m_nUsedObject;
			m_nMaxObject = (m_nUsedObject > m_nMaxObject) ? m_nUsedObject : m_nMaxObject;

			T* temp = m_sObjcetPool.construct();
			if ( NULL == temp )
			{
				sc::writeLogError(
					sc::string::format(
					"object_pool construct failed next size is: %1% current workset: %2%", 
					m_sObjcetPool.get_next_size(), sc::getMemoryWorkingSet() ) );

				m_sObjcetPool.set_next_size( 32 );
				temp = m_sObjcetPool.construct();
				if ( NULL != temp )
				{
					sc::writeLogInfo(
						sc::string::format(
						"object_pool construct failed but after call set_next_size construct success next size is: %1%",
						m_sObjcetPool.get_next_size() ) );
				}
			}

			return temp;
		}

		template <class Arg1>
		T* construct(Arg1& sArg1)
		{
			++m_nUsedObject;
			m_nMaxObject = (m_nUsedObject > m_nMaxObject) ? m_nUsedObject : m_nMaxObject;

			T* temp = m_sObjcetPool.construct( sArg1 );
			if ( NULL == temp )
			{
				sc::writeLogError(
					sc::string::format(
					"object_pool construct failed next size is: %1% current workset: %2%", 
					m_sObjcetPool.get_next_size(), sc::getMemoryWorkingSet() ) );

				m_sObjcetPool.set_next_size( 32 );
				temp = m_sObjcetPool.construct( sArg1 );
				if ( NULL != temp )
				{
					sc::writeLogInfo(
						sc::string::format(
						"object_pool construct failed but after call set_next_size construct success next size is: %1%",
						m_sObjcetPool.get_next_size() ) );
				}
			}

			return temp;
		}

		template <class Arg1, class Arg2>
		T* construct(Arg1& sArg1, Arg2& sArg2)
		{
			++m_nUsedObject;
			m_nMaxObject = (m_nUsedObject > m_nMaxObject) ? m_nUsedObject : m_nMaxObject;

			T* temp = m_sObjcetPool.construct( sArg1, sArg2 );
			if ( NULL == temp )
			{
				sc::writeLogError(
					sc::string::format(
					"object_pool construct failed next size is: %1% current workset: %2%", 
					m_sObjcetPool.get_next_size(), sc::getMemoryWorkingSet() ) );

				m_sObjcetPool.set_next_size( 32 );
				temp = m_sObjcetPool.construct( sArg1, sArg2 );
				if ( NULL != temp )
				{
					sc::writeLogInfo(
						sc::string::format(
						"object_pool construct failed but after call set_next_size construct success next size is: %1%",
						m_sObjcetPool.get_next_size() ) );
				}
			}

			return temp;
		}

		template <class Arg1, class Arg2, class Arg3>
		T* construct(Arg1& sArg1, Arg2& sArg2, Arg3& sArg3)
		{
			++m_nUsedObject;
			m_nMaxObject = (m_nUsedObject > m_nMaxObject) ? m_nUsedObject : m_nMaxObject;

			T* temp = m_sObjcetPool.construct( sArg1, sArg2, sArg3 );
			if ( NULL == temp )
			{
				sc::writeLogError(
					sc::string::format(
					"object_pool construct failed next size is: %1% current workset: %2%", 
					m_sObjcetPool.get_next_size(), sc::getMemoryWorkingSet() ) );

				m_sObjcetPool.set_next_size( 32 );
				temp = m_sObjcetPool.construct( sArg1, sArg2, sArg3 );
				if ( NULL != temp )
				{
					sc::writeLogInfo(
						sc::string::format(
						"object_pool construct failed but after call set_next_size construct success next size is: %1%",
						m_sObjcetPool.get_next_size() ) );
				}
			}

			return temp;
		}

		void destroy( T* pMem )
		{
			if ( NULL != pMem )
			{
				--m_nUsedObject;
				m_sObjcetPool.destroy( pMem );
				pMem = NULL;
			}
		}

		void set_next_size(size_t unSize)
		{
			m_sObjcetPool.set_next_size( unSize );
		}

		inline const int GetMaxObjectSize () { return m_nMaxObject; }

	public:
		BOOST_OBJECT_POOL_CHECKER( const TCHAR* pName )
			: m_nUsedObject(0)
			, m_nMaxObject(0)
			, m_strPoolName(pName)
		{

		}

		~BOOST_OBJECT_POOL_CHECKER()
		{
			sc::writeLogInfo( sc::string::format( "object_pool Name [%1%] Create Max Object [%2%]", m_strPoolName, m_nMaxObject ) );

			if ( m_nUsedObject != 0 )
			{
				GASSERT( 0 && "[����] �ʿ���� memory �� ������ �� ����. ���α׷����� �˷��ּ���. �α׵� Ȯ���� �ּ���." );
				sc::writeLogError( sc::string::format( "object_pool Name [%1%] Undestroy Object [%2%]", m_strPoolName, m_nUsedObject ) );
			}
		}
	}; // class BOOST_OBJECT_POOL_CHECKER


	//! 2015-01-26
	//! shhan
	//! pool �� ��������� destroy �� ����� ���־����� Ȯ���ϱ����� �ѹ� ���մ�.
	//! Ÿ������ �޸𸮰� ���������� �����ϴ� ������ ã�� ����.
	template < class T >
	class BOOST_OBJECT_POOL_LOCK_CHECKER : public DefaultLock
	{
	private:
		boost::object_pool< T > m_sObjcetPool;
		std::string				m_strPoolName;
		int						m_nUsedObject;
		int						m_nMaxObject;
		BOOL					m_bGassert;

	public:
		T* construct()
		{
			lockOn();

			++m_nUsedObject;
			m_nMaxObject = (m_nUsedObject > m_nMaxObject) ? m_nUsedObject : m_nMaxObject;

			T* temp = m_sObjcetPool.construct();
			if ( NULL == temp )
			{
				sc::writeLogError(
					sc::string::format(
					"object_pool construct failed next size is: %1% current workset: %2%", 
					m_sObjcetPool.get_next_size(), sc::getMemoryWorkingSet() ) );

				m_sObjcetPool.set_next_size( 32 );
				temp = m_sObjcetPool.construct();
				if ( NULL != temp )
				{
					sc::writeLogInfo(
						sc::string::format(
						"object_pool construct failed but after call set_next_size construct success next size is: %1%",
						m_sObjcetPool.get_next_size() ) );
				}
			}

			lockOff();

			return temp;
		}

		template <class Arg1>
		T* construct(Arg1& sArg1)
		{
			lockOn();

			++m_nUsedObject;
			m_nMaxObject = (m_nUsedObject > m_nMaxObject) ? m_nUsedObject : m_nMaxObject;

			T* temp = m_sObjcetPool.construct( sArg1 );
			if ( NULL == temp )
			{
				sc::writeLogError(
					sc::string::format(
					"object_pool construct failed next size is: %1% current workset: %2%", 
					m_sObjcetPool.get_next_size(), sc::getMemoryWorkingSet() ) );

				m_sObjcetPool.set_next_size( 32 );
				temp = m_sObjcetPool.construct( sArg1 );
				if ( NULL != temp )
				{
					sc::writeLogInfo(
						sc::string::format(
						"object_pool construct failed but after call set_next_size construct success next size is: %1%",
						m_sObjcetPool.get_next_size() ) );
				}
			}

			lockOff();

			return temp;
		}

		void destroy( T* pMem )
		{
			lockOn();

			if ( NULL != pMem )
			{
				--m_nUsedObject;
				m_sObjcetPool.destroy( pMem );
				pMem = NULL;
			}

			lockOff();
		}

	public:
		BOOST_OBJECT_POOL_LOCK_CHECKER( const TCHAR* pName, BOOL bGassert=TRUE )
			: m_nUsedObject(0)
			, m_nMaxObject(0)
			, m_strPoolName(pName)
			, m_bGassert(bGassert)		// PER_IO_OPERATION_DATA ���� ���������ٰ� ����� �ϴµ� �������� ��ġ�ϲ� ���Ƽ� GASSERT �� ������ �ʰ� ��.
		{

		}

		~BOOST_OBJECT_POOL_LOCK_CHECKER()
		{
			sc::writeLogInfo( sc::string::format( "object_pool Name [%1%] Create Max Object [%2%]", m_strPoolName, m_nMaxObject ) );

			if ( m_nUsedObject != 0 )
			{
				if ( m_bGassert )
				{
					GASSERT( 0 && "[����] �ʿ���� memory �� ������ �� ����. ���α׷����� �˷��ּ���. �α׵� Ȯ���� �ּ���." );
				}
				sc::writeLogError( sc::string::format( "object_pool Name [%1%] Undestroy Object [%2%]", m_strPoolName, m_nUsedObject ) );
			}
		}
	}; // class BOOST_OBJECT_POOL_LOCK_CHECKER



    //! 2007-04-16
    //! Jgkim
    template < class T >
    class ObjectPool
    {
    protected:
        boost::object_pool< T > m_Mem;

    public:
		T* getMem()
		{
			T* temp = m_Mem.construct();
			if ( NULL == temp )
			{
				sc::writeLogError(
					sc::string::format(
					"object_pool construct failed next size is: %1% current workset: %2%", 
					m_Mem.get_next_size(), sc::getMemoryWorkingSet() ) );

				m_Mem.set_next_size( 32 );
				temp = m_Mem.construct();
				if ( NULL != temp )
				{
					sc::writeLogInfo(
						sc::string::format(
						"object_pool construct failed but after call set_next_size construct success next size is: %1%",
						m_Mem.get_next_size() ) );
				}
			}

			return temp;
		}

		void releaseMem( T* pMem )
		{
			if ( NULL != pMem )
			{
				m_Mem.destroy( pMem );
				pMem = NULL;
			}
		}
    }; // class ObjectPool

    //! 2007-04-16
    //! Jgkim
    template < class T >
    class ObjectPoolLock : public DefaultLock
    {
    protected:
        boost::object_pool< T > m_Mem;

    public:
        T* getMem()
        {
            lockOn();

			T* temp = m_Mem.construct();
			if ( NULL == temp )
			{
				sc::writeLogError(
					sc::string::format(
					"object_pool construct failed next size is: %1% current workset: %2%", 
					m_Mem.get_next_size(), sc::getMemoryWorkingSet() ) );

				m_Mem.set_next_size( 32 );
				temp = m_Mem.construct();
				if ( NULL != temp )
				{
					sc::writeLogInfo(
						sc::string::format(
						"object_pool construct failed but after call set_next_size construct success next size is: %1%",
						m_Mem.get_next_size() ) );
				}
			}

            lockOff();

            return temp;
        }

        void releaseMem( T* pMem )
        {
			lockOn();

            if ( NULL != pMem )
            {
                m_Mem.destroy( pMem );
                pMem = NULL;
            }

			lockOff();
        }
    }; // class ObjectPool



    //////////////////////////////////////////////////////////////////////////
    /// @class �޸� Ǯ Ŭ����
    /// @brief ��Ŭ������ ��� ������  new delete �����ڰ� ȣ��ɶ� PoolAllocMgr�� 
    /// operator new , delete �� ȣ��Ǿ� Ǯ �޸𸮸� ����ϰ� ��
    class PoolAllocMgr
    {
        typedef std::tr1::shared_ptr< boost::pool<> > SP_POOL; 
        typedef std::tr1::unordered_map< UINT, SP_POOL > POOL_TABLE;
    public:

#ifndef _DEBUG
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        /// ����׿��� �������ؼ� ������ ������
        /// ����� �����϶� ������ �߻� :  ����� ��忡�� DEBUG_NEW�� �̿��ϴµ� DEBUG_DELETE�� �߰��� ������ ����ؾ���
        /// ���� ��� ���� ó�� ó���Ϸ���  new , delete �Ѵ� ��ũ�� ����������
        static void* operator new ( UINT size ) 
        {
            CLASS_AUTO_DEFAULT_LOCK;
            MIN_ASSERT(size > 0);
            POOL_TABLE::iterator it = s_poolTable.find(size);
            if( it == s_poolTable.end() )
            {
                SP_POOL spPool(new boost::pool<>(size));

                std::pair<POOL_TABLE::iterator, bool> ret = s_poolTable.insert( std::make_pair(size,spPool) );
                MIN_ASSERT(ret.second);
                it = ret.first;
            }

            return it->second->malloc();
        }

        static void operator delete ( void * p, UINT size ) throw () 
        {
            CLASS_AUTO_DEFAULT_LOCK;
            POOL_TABLE::iterator it = s_poolTable.find(size);
            MIN_ASSERT( s_poolTable.end() != it );
            it->second->free(p);
        }
#endif
        //////////////////////////////////////////////////////////////////////////

    private:
        static DECLARE_CLASS_DEFAULT_LOCK;  
        static POOL_TABLE s_poolTable;
    };

    //////////////////////////////////////////////////////////////////////////

} // namespace sc

#endif // _SC_OBJECT_POOL_H_