#ifndef _NETLOG_PACKET_H_
#define _NETLOG_PACKET_H_

#include <set>
#include <afxmt.h>
#include "../../SigmaCore/log/LogPacket.h"
#include "../../RanLogic/s_NetGlobal.h"

/*
	콘솔을 통한 패킷 로그를
	보고 싶다면 아래 주석을 풀 것
	NDEBUG 는 Release 모드 이므로
	Release 모드에서는 사용하지 말 것
*/
//#ifndef NDEBUG
//#define _PACKET_LOG
//#define _USE_SIMPLE_PACKET_LOG // 불필요한 패킷은 찍지 않음 // 이옵션을 사용하지 않으면 Heartbeat 등의 불필요한 패킷도 로그로 출력함
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
 	란 클라이언트 패킷 로그
    클라이언트가 서버와 패킷을 주고 받는 것을 모니터링 할 수 있다.

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
             출력하지 말아야 할 프로토콜 필터링을 처리 한다.			 
         */			
		void                            BeginRead();		
		void							EndRead(int nType);
		void                            BeginWrite();
		void							EndWrite(int nType);

		/**
			내부적으로 생성되는 패킷 문자열을 적당한 출력 장소에 출력 한다.
			현재는 콘솔에 출력한다.

			\param pData 출력할 패킷 로그 문자열
		 */
        void                            Write(const TCHAR* pData);

		/**
			패킷 타입과 대응 되는 심벌을 리턴 한다.
			현재 란에서는 서버와 클라 패킷을 동일하게 사용하므로
			동일한 타입이라면 둘다 리턴하는 값은 같다.

			\param nType 패킷 고유 타입
		 */
        const TCHAR*                    GetServerPacketType(int nType);
        const TCHAR*                    GetClientPacketType(int nType);

protected:
        /** 
            프로토콜을 로드 해서 심벌을 생성 한다.
            s_NetGlobal.h 에 있는 심벌을 로딩 한다.

         */
		void							LoadProtocol();
        void                            RecursiveSymbol(string_map& kMap, tfstream& fs, int& nSum, int& nOperator);

		/**
            출력하지 말아야할 프로토콜을 등록 한다.
            주기적으로 주고 받는 프로토콜등은 굳이 출력할 필요가 없다.
            이런 것들을 등록 한다.

         */
        void							CreateFilter();

		/* 출력 콘솔 */
        sc::LogConsole*                 m_pConsole;

		/* 출력 파일 */
		tfstream						m_file;

		/* 패킷 타입과 대응되는 심벌들 */
        symbol_map                      m_mapSymbol;

		/* 필터링할 심벌들 */
		filter_symbol					m_setFilter;

		/* 각 서버 스레드와 읽기, 쓰기 동시 접근으로 인한 락 */
		CCriticalSection				m_lock[3];

#ifdef _PACKET_LOG
		// 차후에 동일번호의 패킷이 나올수 있고 
		// CS를 별도로 돌리기때문에 Send 와 Recv 를 따로 처리
		SLogPacketCount					m_pLogC2SPacket[EMNET_MSG::NET_MSG_GCTRL_MAX];
		SLogPacketCount					m_pLogS2CPacket[EMNET_MSG::NET_MSG_GCTRL_MAX];
#endif
};

#endif // _LOG_PACKET_H_