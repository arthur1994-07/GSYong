#ifndef _GL_AUTO_LEVEL_DEFINE_H_
#define _GL_AUTO_LEVEL_DEFINE_H_

#pragma once

#include <string>
#include <vector>
#include <queue>

#define MAX_EVENT_POS 5

enum GLLEV_COMMAND
{
    CMD_MSG					= 0,	//	Msg���
    CMD_WARNING				= 1,	//	Wanning ���
    CMD_COUNTDOWN			= 2,	//	ī��Ʈ �ٿ�
    CMD_CALL_MAP			= 3,	//	map�� lev���� �ε�
    CMD_DEL_MAP				= 4,	//	map�� lev���� ����
    CMD_CALL_HALL_MAP		= 5,	//	hall map�� lev���� �ε�
    CMD_DEL_HALL_MAP		= 6,	//	hall map�� lev���� ����
    CMD_MOVE_CHA_HALL_MAP	= 7,	//	hall map���� ĳ���� ��� �̵�
    CMD_OLD_SIZE			= 8,	//	���� ��ɾ� 

    CMD_EVENTITEM_BEGIN		= 8,	//	������ ��� Ȯ�� ����
    CMD_EVENTITEM_END		= 9,	//	������ ��� Ȯ�� ���� 		
    CMD_EVENTMONEY_BEGIN	= 10,	//	�Ӵ� ��� Ȯ�� ����
    CMD_EVENTMONEY_END		= 11,	//	�Ӵ� ��� Ȯ�� ���� 		
    CMD_EVENTEXP_BEGIN		= 12,	//	����ġ ��� Ȯ�� ����
    CMD_EVENTEXP_END		= 13,	//	����ġ ��� Ȯ�� ���� 		
    CMD_LIMIT_EVENT_BEGIN	= 14,	//	����ġ �ν��� ����
    CMD_LIMIT_EVENT_END		= 15,	//	����ġ �ν��� ����
    CMD_EVENT_EX_BEGIN		= 16,	//	Ư�� �̺�Ʈ ����
    CMD_EVENT_EX_END		= 17,	//	Ư�� �̺�Ʈ ����

    CMD_EVENT_GRADE_BEGIN	= 18,	//	���� Ȯ�� ���
    CMD_EVENT_GRADE_END		= 19,	//	���� Ȯ�� ����

    CMD_SIZE				= 20,	//	��ü ������
};


extern std::string strCommandList[CMD_SIZE];
extern std::string strCommandHelp[CMD_SIZE];

struct GLLEV_TIME
{
	union
	{
		int nTime;
		struct  
		{
			short nHour;		// �ð� (0-23����)
			short nMinute;		// �� (0-59����)
		};
	};

	const GLLEV_TIME operator -(const GLLEV_TIME& _rhs)
	{
		const int _nHour(_rhs.nMinute / 60);
		const int _nMinute(_rhs.nMinute % 60);

		GLLEV_TIME _time;
		if ( _nMinute > nMinute )
		{
			_time.nHour = nHour - (_nHour + _rhs.nHour + 1);
			_time.nMinute = nMinute - (_nMinute - 60);
		}
		else
		{
			_time.nHour = nHour - (_nHour + _rhs.nHour);
			_time.nMinute = nMinute - _nMinute;
		}

		return _time;
	}
	const GLLEV_TIME operator +(const GLLEV_TIME& _rhs)
	{
		const int _nHour(_rhs.nMinute / 60);
		const int _nMinute(_rhs.nMinute % 60);

		GLLEV_TIME _time;
		if ( _nMinute + nMinute > 59 )
		{
			_time.nHour = nHour + (_nHour + _rhs.nHour + 1);
			_time.nMinute = nMinute + _nMinute - 60;
		}
		else
		{
			_time.nHour = nHour + (_nHour + _rhs.nHour);
			_time.nMinute = nMinute + _nMinute;
		}

		return _time;
	}
	const GLLEV_TIME& operator -=(const GLLEV_TIME& _rhs)
	{	
		const int _nHour(_rhs.nMinute / 60);
		const int _nMinute(_rhs.nMinute % 60);

		if ( _nMinute > nMinute )
		{
			nHour -= (_nHour + _rhs.nHour + 1);
			nMinute -= (_nMinute - 60);
		}
		else
		{
			nHour -= (_nHour + _rhs.nHour);
			nMinute -= _nMinute;
		}

		return *this;
	}
	const GLLEV_TIME& operator -=(const int _nTotalMinute)
	{	
		const int _nHour(_nTotalMinute / 60);
		const int _nMinute(_nTotalMinute % 60);
		
		if ( _nMinute > nMinute )
		{
			nHour -= (_nHour + 1);
			nMinute -= (_nMinute - 60);
		}
		else
		{
			nHour -= _nHour;
			nMinute -= _nMinute;
		}

		return *this;
	}
	const bool operator <=(const GLLEV_TIME& _rhs)
	{
		if ( nHour != _rhs.nHour )
			return false;

		return nMinute <= _rhs.nMinute;
	}
	const bool operator ==(const GLLEV_TIME& _rhs)
	{
		return (nMinute == _rhs.nMinute) && (nHour == _rhs.nHour);
	}
    GLLEV_TIME(const int _nHour = 0, const int _nMinute = 0)
        : nHour(short(_nHour))
        , nMinute(short(_nMinute))
    {
    }
};

struct GLLEV_SCRIPT_OLD
{
    GLLEV_TIME levTime;				
    std::string strCommand;		// �ڵ� ���� ��ɾ�
    std::string strArgument;	// �ڵ� ���� ��ɾ� ����
};

struct GLLEV_SCRIPT
{
    GLLEV_TIME levTime;				
    GLLEV_COMMAND	emCommand;

    std::string		strArgument;

    GLLEV_SCRIPT() 
        : emCommand ( CMD_SIZE )
    {
    }

};

struct GLLEV_POS
{
    DWORD	m_dwPOS_X;
    DWORD	m_dwPOS_Y;

    GLLEV_POS()
        : m_dwPOS_X(0)
        , m_dwPOS_Y(0)
    {
    }
};

typedef std::vector<GLLEV_SCRIPT>	VEC_LEV_SCRIPT;
typedef VEC_LEV_SCRIPT::iterator	VEC_LEV_SCRIPT_ITER;

typedef std::queue<GLLEV_SCRIPT>	QUEUE_LEV_SCRIPT;

typedef std::list<std::string>		STRING_LIST;
typedef STRING_LIST::iterator		STRING_LIST_ITER;

#endif // _GL_AUTO_LEVEL_DEFINE_H_