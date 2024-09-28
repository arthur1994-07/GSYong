#ifndef _EMUL_BASE_H_
#define _EMUL_BASE_H_

#pragma once 

#include <deque>
//#include "../CoreCommon/CoreCommon.h"
//#include "../NetCommon/NetCommon.h"

struct NET_MSG_GENERIC;

class EmulBase
{
protected:
	// 에뮬레이트용 메시지 구조체
	struct CEMULMSG
	{
		float	fDelay;
		DWORD	dwSize;
		PBYTE	pBuffer;

		CEMULMSG();
		CEMULMSG( float _fdelay, DWORD _dwSize, PBYTE _pbuffer );
		~CEMULMSG();

        CEMULMSG& operator= ( const CEMULMSG &cMsg );
	};

public:
	EmulBase();
	virtual ~EmulBase();

protected:
	std::deque<CEMULMSG*> g_vecMsg;

public:
	virtual void _update_emul_msg(
		DWORD dwClientID,
		DWORD dwGaeaID,
		float fElapsTime ) = 0;

	//! 에뮬레이트에 메시지를 삽입한다.
	void _insert_emul_msg( NET_MSG_GENERIC *nmg );
	void _clear_emul_msg();	

}; // class EmulBase

#endif // _EMUL_BASE_H_