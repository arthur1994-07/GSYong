#ifndef _NETLOG_PACKET_H_
#define _NETLOG_PACKET_H_

#include <set>
#include <afxmt.h>
#include "../../SigmaCore/log/LogPacket.h"
#include "../../RanLogic/s_NetGlobal.h"

/*
	�ܼ��� ���� ��Ŷ �α׸�
	���� �ʹٸ� �Ʒ� �ּ��� Ǯ ��
	NDEBUG �� Release ��� �̹Ƿ�
	Release ��忡���� ������� �� ��
*/
//#ifndef NDEBUG
//#define _PACKET_LOG
//#define _USE_SIMPLE_PACKET_LOG // ���ʿ��� ��Ŷ�� ���� ���� // �̿ɼ��� ������� ������ Heartbeat ���� ���ʿ��� ��Ŷ�� �α׷� �����
//#endif

#ifdef _PACKET_LOG
#   define Packet_Log_Declare()         NetLogPacket g_kLog
#   define Packet_Log_Init()            g_kLog.Init()
#   define Packet_Log_Destroy()         g_kLog.Destroy()
#   define Packet_Log_Read_begin()      g_kLog.BeginRead()
#   define Packet_Log_Read_End(x)       g_kLog.EndRead(x)
#   define Packet_Log_Read(x)           g_kLog << x
#   define Packet_Log_Write_Begin()     g_kLog.BeginWrite()
#   define Packet_Log_Write_End(y)      g_kLog.EndWrite(y)
#   define Packet_Log_Write(x)          g_kLog >> x
#else
#   define Packet_Log_Declare() 
#   define Packet_Log_Init()            __noop
#   define Packet_Log_Destroy()         __noop
#   define Packet_Log_Read_begin()      __noop
#   define Packet_Log_Read_End(x)       __noop
#   define Packet_Log_Read(x)           __noop
#   define Packet_Log_Write_Begin()     __noop
#   define Packet_Log_Write_End(y)      __noop
#   define Packet_Log_Write(x)          __noop
#endif

namespace sc { class LogConsole; }


/**
 	�� Ŭ���̾�Ʈ ��Ŷ �α�
    Ŭ���̾�Ʈ�� ������ ��Ŷ�� �ְ� �޴� ���� ����͸� �� �� �ִ�.

    \date 2012-06-20
    \developer oen 
 */

struct SLogPacketCount
{
	int nPacketTotalCount;
	CTime lastPacketTime;
};

class NetLogPacket
    : public sc::LogPacket
{
public:
        NetLogPacket();

        typedef std::map<int, tstring>  symbol_map;
        typedef symbol_map::iterator	symbol_map_iterator;
        typedef std::set<int>			filter_symbol;
        typedef filter_symbol::iterator filter_symbol_iterator;
        typedef std::map<tstring, int>	string_map;
        typedef string_map::iterator    string_map_iterator;

        bool                            Init();
        void                            Destroy();

        /**
             ������� ���ƾ� �� �������� ���͸��� ó�� �Ѵ�.			 
         */			
		void                            BeginRead();		
		void							EndRead(int nType);
		void                            BeginWrite();
		void							EndWrite(int nType);

		/**
			���������� �����Ǵ� ��Ŷ ���ڿ��� ������ ��� ��ҿ� ��� �Ѵ�.
			����� �ֿܼ� ����Ѵ�.

			\param pData ����� ��Ŷ �α� ���ڿ�
		 */
        void                            Write(const TCHAR* pData);

		/**
			��Ŷ Ÿ�԰� ���� �Ǵ� �ɹ��� ���� �Ѵ�.
			���� �������� ������ Ŭ�� ��Ŷ�� �����ϰ� ����ϹǷ�
			������ Ÿ���̶�� �Ѵ� �����ϴ� ���� ����.

			\param nType ��Ŷ ���� Ÿ��
		 */
        const TCHAR*                    GetServerPacketType(int nType);
        const TCHAR*                    GetClientPacketType(int nType);

protected:
        /** 
            ���������� �ε� �ؼ� �ɹ��� ���� �Ѵ�.
            s_NetGlobal.h �� �ִ� �ɹ��� �ε� �Ѵ�.

         */
		void							LoadProtocol();
        void                            RecursiveSymbol(string_map& kMap, tfstream& fs, int& nSum, int& nOperator);

		/**
            ������� ���ƾ��� ���������� ��� �Ѵ�.
            �ֱ������� �ְ� �޴� �������ݵ��� ���� ����� �ʿ䰡 ����.
            �̷� �͵��� ��� �Ѵ�.

         */
        void							CreateFilter();

		/* ��� �ܼ� */
        sc::LogConsole*                 m_pConsole;

		/* ��� ���� */
		tfstream						m_file;

		/* ��Ŷ Ÿ�԰� �����Ǵ� �ɹ��� */
        symbol_map                      m_mapSymbol;

		/* ���͸��� �ɹ��� */
		filter_symbol					m_setFilter;

		/* �� ���� ������� �б�, ���� ���� �������� ���� �� */
		CCriticalSection				m_lock[3];

#ifdef _PACKET_LOG
		// ���Ŀ� ���Ϲ�ȣ�� ��Ŷ�� ���ü� �ְ� 
		// CS�� ������ �����⶧���� Send �� Recv �� ���� ó��
		SLogPacketCount					m_pLogC2SPacket[EMNET_MSG::NET_MSG_GCTRL_MAX];
		SLogPacketCount					m_pLogS2CPacket[EMNET_MSG::NET_MSG_GCTRL_MAX];
#endif
};

#endif // _LOG_PACKET_H_