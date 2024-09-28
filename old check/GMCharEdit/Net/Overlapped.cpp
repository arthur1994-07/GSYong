#include "stdafx.h"
#include "Overlapped.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace sc
{
    namespace net
    {
        Overlapped::Overlapped()
            //: m_dwUseCount(0)
			: m_Mem(_T("PER_IO_OPERATION_DATA"),FALSE)
        {
//             int i = 0;	
//             for (i=0; i<nStartSize; ++i)
//             {
//                 m_UnUse.AddTail();
//             }
        }

        Overlapped::~Overlapped()
        {
        }

        PER_IO_OPERATION_DATA* Overlapped::GetFreeOverIO(EMMSG_IO_TYPE emType)
        {	
            /*
            PER_IO_OPERATION_DATA* pTemp = NULL;
            int nSize = 0;
            int i     = 0;

            lockOn();	

            pTemp = (PER_IO_OPERATION_DATA*) m_UnUse.Get();

            if (pTemp == NULL) 
            {
                lockOff();
                return NULL;
            }
            else
            {
                // pTemp->Reset();
                // ZeroMemory(&pTemp->Overlapped, sizeof(OVERLAPPED));
                pTemp->OperationType = nType;
                pTemp->dwRcvBytes    = 0;
                pTemp->dwSndBytes    = 0;
                pTemp->dwTotalBytes  = 0;
                m_Use.AddTail((sc::NODE<PER_IO_OPERATION_DATA>*) pTemp);
                lockOff();
                return pTemp;
            }
            */
            //lockOn();
            //PER_IO_OPERATION_DATA* pTemp = m_Mem.getMem();
			PER_IO_OPERATION_DATA* pTemp = m_Mem.construct();
            if (pTemp)
            {
                pTemp->OperationType = emType;
                //m_dwUseCount++;	//mjeon.단순 출력용도인 이 카운터로 인해 lock이 필요해진다. 사용하게 된다면 atomic 연산으로 바꿀 것.
            }
            //lockOff();
            return pTemp;
        }

        void Overlapped::Release(PER_IO_OPERATION_DATA* pMem)
        {   
            //lockOn();
            if (pMem)
            {   
                //m_Mem.releaseMem(pMem);
				m_Mem.destroy(pMem);
                //m_dwUseCount--;	//mjeon.단순 출력용도인 이 카운터로 인해 lock이 필요해진다. 사용하게 된다면 atomic 연산으로 바꿀 것.
            }
            //lockOff();
        }

//         DWORD Overlapped::GetUseCount() const
//         {
//             return m_dwUseCount;
//             //return m_Use.GetSize();
//         }

        /*
        int Overlapped::GetUnUseCount() const
        {
            return m_UnUse.GetSize();
        }
        */

        IOManager::IOManager()
        {
        }

        IOManager::~IOManager()
        {
        }

        PER_IO_OPERATION_DATA* IOManager::GetIO(EMMSG_IO_TYPE nType)
        {
            if (nType == NET_SEND_POSTED)
                return GetSendIO();
            else
                return GetRecvIO();
        }

        PER_IO_OPERATION_DATA* IOManager::GetSendIO()
        {
            return m_Send.GetFreeOverIO(NET_SEND_POSTED);
        }

        PER_IO_OPERATION_DATA* IOManager::GetRecvIO()
        {
            return m_Recv.GetFreeOverIO(NET_RECV_POSTED);
        }

        void IOManager::ReleaseIO(PER_IO_OPERATION_DATA* pData)
        {
            if (NET_SEND_POSTED == pData->OperationType)
                ReleaseSendIO(pData);
            else
                ReleaseRecvIO(pData);
        }

        void IOManager::ReleaseSendIO(PER_IO_OPERATION_DATA* pData)
        {
            m_Send.Release(pData);
        }

        void IOManager::ReleaseRecvIO(PER_IO_OPERATION_DATA* pData)
        {
            m_Recv.Release(pData);
        }
    } // namespace net
} // namespace sc