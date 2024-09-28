#ifndef _LOG_PACKET_H_
#define _LOG_PACKET_H_

#pragma once

/*
namespace CORE_COMMON
{

class IPacketAnalysis;

//! 2008-06-25 Jgkim
//! ��Ŷ �м��� ���� �α�
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
 	��Ŷ �α�
    ������ ��Ŷ�� �ְ� ���� �� �ش� ��Ŷ�� �ܼ�, ���ϵ ����� �� �ִ�.
    ��Ŷ �α׸� ���� ������ ���۵� �������� �帧(Flow)�� ������ �ľ��� �� �ִ�.

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
            ��Ŷ �α׸� �ʱ�ȭ, ������ ó�� �Ѵ�.
            
            ����ܿ��� ��ӹ޾� �ܼ�, ���� ���� ���� ó���� �� �� �ִ�.
         */
virtual bool                                    Init() = 0;
virtual void                                    Destroy() = 0;

       /**
            ��Ŷ�� �аų� �� �� �α׸� ����� ������ �Ѵ�.
            
            ����ܿ��� �� �Լ��� ��Ŷ�� ���ų� ���� �� ȣ��ǵ��� �ؾ� �Ѵ�.
         */
virtual void                                    BeginRead();
virtual void                                    EndRead(int nType, int nPacketTotalCount = 0, CTimeSpan spanTime = 0);
virtual void                                    BeginWrite();
virtual void                                    EndWrite(int nType, int nPacketTotalCount = 0, CTimeSpan spanTime = 0);

        /**
            ����� ���� �Ѵ�.

            ����ܿ��� �� �Լ��� ��� �޾� ���ڿ��� ������ ��(�ܼ�, ���ϵ�)�� ��� �Ѵ�.
            
            \param pData ����� ���ڿ�
         */
virtual void                                    Write(const TCHAR* pData) = 0;

        /**
            ���� ��Ŷ Ÿ���� ���� ��Ʈ���� ��� �Ѵ�.
            ������ ���� ���޵Ǵ� ��Ŷ�� Read �� �� ȣ�� �ȴ�.

            ����ܿ����� ��Ŷ Ÿ�Կ� ���� ���� ��Ʈ���� ����� �ش�.

            \param nType ��Ŷ Ÿ��
         */
virtual const TCHAR*                            GetServerPacketType(int nType)  { return 0; }

        /**
            Ŭ���̾�Ʈ ��Ŷ Ÿ���� ���� ��Ʈ���� ��� �Ѵ�.
            Ŭ��� ���� ��Ŷ�� Write �� �� ȣ�� �ȴ�.

            ����ܿ����� ��Ŷ Ÿ�Կ� ���� ���� ��Ʈ���� ����� �ش�.

            \param nType ��Ŷ Ÿ��
         */
virtual const TCHAR*                            GetClientPacketType(int nType)  { return 0; }

protected:
        /**
            ���Ŀ� �°� �ð��� ��´�.
            ������ YY, MM, DD, HH, mm, SS, ss �� ��ġ �ȴ�.
            YY - �� MM - �� DD - ��¥ HH - �ð� mm - �� SS - �� ss - �и���
            "YY�� MM��" �� �������� getData("YY�� MM��");

            \param out ��� �� ��Ʈ��
            \param pType ����� ���� ���ڿ�
         */
        void                                    GetDate(tstring& out, TCHAR* pType = _T("YY-MM-DD HH:mm:SS:ss"));

        /**
    	    STL �� ��Ʈ���� Printf ó�� ó�� �ϴ� ����� �Ѵ�.

            \param pStr ó���� ��Ʈ���� ������
                   �ݵ�� �����ͷ� �Ѱ��־�� �Ѵ�. ������ �Ѱ��� ��� ����ġ ���� ������ �����.
            \param ... ���� ����
        */
        void                                    PrintfWithString(tstring* pStr, ...);

        /* �Է� �α� �����<����� ����Ұ� ���� �����ϴ� ������ ���ÿ� ����� ���� �ֱ� �����̴�.>*/
        tstring                                 m_strReadHeader;
        tstringstream                           m_strReadContents;

        /* ��� �α� ����� */
        tstring                                 m_strSendHeader;        
        tstringstream                           m_strSendContents;

        /* ��Ʈ�� ��ȯ�� ���� */
        TCHAR                                   m_buff[1024];
};

}; //namespace


#endif // _LOG_PACKET_H_