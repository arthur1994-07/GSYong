#ifndef S_COVERLAPPED_H_
#define S_COVERLAPPED_H_

#pragma once

//#include "s_NetGlobal.h"
#include <boost/noncopyable.hpp>
#include "../Util/Lock.h"
//#include "../Memory/CMemPooler.h"
#include "../Memory/ObjectPool.h"
#include "./NetDefine.h"

namespace sc
{
    namespace net
    {        
        //! Overlapped.h
        //! Copyright (c) Min Communications. All rights reserved.                 
        //!
        //! class Overlapped       
        //!
        //! * History
        //! 2002.05.30 Jgkim Create
        //! 2011.03.14 Jgkim namespace net        
        //!
        //! Overlapped I/O 를 위한 memory 를 관리한다.
        //!
        //! 리스트는 전송대기 / 전송요청을 함께 처리한다.
        class Overlapped sealed : public sc::DefaultLock, private boost::noncopyable
        {
        public:
            Overlapped();
            ~Overlapped();

        private:
			sc::BOOST_OBJECT_POOL_LOCK_CHECKER<PER_IO_OPERATION_DATA> m_Mem; //sc::ObjectPoolLock<PER_IO_OPERATION_DATA> m_Mem;
            //DWORD m_dwUseCount;

            //sc::TCList<PER_IO_OPERATION_DATA> m_Use;
            //sc::TCList<PER_IO_OPERATION_DATA> m_UnUse;

        public:
            PER_IO_OPERATION_DATA* GetFreeOverIO(EMMSG_IO_TYPE emType=NET_SEND_POSTED);
            void Release(PER_IO_OPERATION_DATA* pMem);
            
			//inline DWORD GetUseCount() const { return m_dwUseCount; }	//mjeon.현재 사용되는 곳이 없다.(모두 주석처리되어 있음)

            //int GetUnUseCount() const;
        };

        //! Send, Recv memory 관리
        class IOManager sealed : private boost::noncopyable
        {
        public:
            IOManager();
            ~IOManager();

        protected:
            Overlapped m_Recv;
            Overlapped m_Send;

        public:
            PER_IO_OPERATION_DATA* GetIO(EMMSG_IO_TYPE nType);
            PER_IO_OPERATION_DATA* GetSendIO();
            PER_IO_OPERATION_DATA* GetRecvIO();

            void ReleaseIO(PER_IO_OPERATION_DATA* pData );
            void ReleaseSendIO(PER_IO_OPERATION_DATA* pData );
            void ReleaseRecvIO(PER_IO_OPERATION_DATA* pData );		
			
			/*
            inline DWORD GetSendUseCount() const
            {
                return m_Send.GetUseCount();
            }

            inline DWORD GetRecvUseCount() const
            {
                return m_Recv.GetUseCount();
            }
			*/
			
        };
    } // namespace net
} // namespace sc

#endif // S_COVERLAPPED_H_