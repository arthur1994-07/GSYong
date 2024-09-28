#ifndef _LOG_PACKET_H_
#define _LOG_PACKET_H_

#pragma once

/*
namespace CORE_COMMON
{

class IPacketAnalysis;

//! 2008-06-25 Jgkim
//! 패킷 분석을 위한 로그
class LogPacket
{
private:
	LogPacket();

public:
	~LogPacket();
	static LogPacket& getInstance();

protected:
	IPacketAnalysis* m_pImpl;

public:
	void add( WORD Type, WORD Size );
	void start();
	void end();
};

};
*/

#include <sstream>

namespace sc
{

/**
 	패킷 로그
    서버와 패킷을 주고 받을 때 해당 패킷을 콘솔, 파일등에 출력할 수 있다.
    패킷 로그를 통해 기존에 제작된 컨텐츠의 흐름(Flow)을 빠르게 파악할 수 있다.

    \date 2012-06-20
    \developer oen 
 */
class LogPacket
{
public:
        LogPacket();
virtual ~LogPacket();

		typedef std::basic_string<TCHAR>        tstring;
		typedef std::basic_fstream<TCHAR>       tfstream;
		typedef std::basic_stringstream<TCHAR>  tstringstream;

        /**
            패킷 로그를 초기화, 삭제를 처리 한다.
            
            응용단에서 상속받아 콘솔, 파일 관련 사전 처리를 할 수 있다.
         */
virtual bool                                    Init() = 0;
virtual void                                    Destroy() = 0;

       /**
            패킷을 읽거나 쓸 때 로그를 남기는 역할을 한다.
            
            응용단에서 이 함수를 패킷을 쓰거나 읽을 때 호출되도록 해야 한다.
         */
virtual void                                    BeginRead();
virtual void                                    EndRead(int nType, int nPacketTotalCount = 0, CTimeSpan spanTime = 0);
virtual void                                    BeginWrite();
virtual void                                    EndWrite(int nType, int nPacketTotalCount = 0, CTimeSpan spanTime = 0);

        /**
            출력을 제어 한다.

            응용단에서 이 함수를 상속 받아 문자열을 적당한 곳(콘솔, 파일등)에 출력 한다.
            
            \param pData 출력할 문자열
         */
virtual void                                    Write(const TCHAR* pData) = 0;

        /**
            서버 패킷 타입의 따른 스트링을 출력 한다.
            서버로 부터 전달되는 패킷을 Read 할 때 호출 된다.

            응용단에서는 패킷 타입에 따른 구분 스트링을 출력해 준다.

            \param nType 패킷 타입
         */
virtual const TCHAR*                            GetServerPacketType(int nType)  { return 0; }

        /**
            클라이언트 패킷 타입의 따른 스트링을 출력 한다.
            클라로 부터 패킷을 Write 할 때 호출 된다.

            응용단에서는 패킷 타입에 따른 구분 스트링을 출력해 준다.

            \param nType 패킷 타입
         */
virtual const TCHAR*                            GetClientPacketType(int nType)  { return 0; }

protected:
        /**
            형식에 맞게 시간을 얻는다.
            형식은 YY, MM, DD, HH, mm, SS, ss 로 매치 된다.
            YY - 년 MM - 월 DD - 날짜 HH - 시간 mm - 분 SS - 초 ss - 밀리초
            "YY년 MM월" 로 얻어오려면 getData("YY년 MM월");

            \param out 출력 될 스트링
            \param pType 출력할 형식 문자열
         */
        void                                    GetDate(tstring& out, TCHAR* pType = _T("YY-MM-DD HH:mm:SS:ss"));

        /**
    	    STL 의 스트링을 Printf 처럼 처리 하는 기능을 한다.

            \param pStr 처리할 스트링의 포인터
                   반드시 포인터로 넘겨주어야 한다. 참조로 넘겨줄 경우 예기치 못한 문제가 생긴다.
            \param ... 가변 인자
        */
        void                                    PrintfWithString(tstring* pStr, ...);

        /* 입력 로그 저장소<입출력 저장소가 따로 존재하는 이유는 동시에 진행될 수도 있기 때문이다.>*/
        tstring                                 m_strReadHeader;
        tstringstream                           m_strReadContents;

        /* 출력 로그 저장소 */
        tstring                                 m_strSendHeader;        
        tstringstream                           m_strSendContents;

        /* 스트링 변환용 버퍼 */
        TCHAR                                   m_buff[1024];
};

}; //namespace


#endif // _LOG_PACKET_H_