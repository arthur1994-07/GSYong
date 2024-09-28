#include "pch.h"
#include <algorithm>
#include "../SigmaCore/gassert.h"
#include "../EngineLib/Common/StlFunctions.h"
#include "./EmulBase.h"
#include "./s_NetGlobal.h"
#include "../SigmaCore/DebugInclude.h"

EmulBase::CEMULMSG::CEMULMSG()
	: pBuffer( NULL )
	, fDelay( 0.0f )
    , dwSize( 0 )
{
}

EmulBase::CEMULMSG::CEMULMSG( float _fdelay, DWORD _dwSize, PBYTE _pbuffer )
    : fDelay( _fdelay )
    , dwSize( _dwSize )
{
    if ( dwSize == 0 )
        return;

    pBuffer = new BYTE[dwSize];
	memcpy( pBuffer, _pbuffer, _dwSize );
}

EmulBase::CEMULMSG::~CEMULMSG()
{
	SAFE_DELETE_ARRAY( pBuffer );
}

EmulBase::CEMULMSG& EmulBase::CEMULMSG::operator= ( const CEMULMSG &cMsg )
{
	SAFE_DELETE_ARRAY( pBuffer );

	fDelay = cMsg.fDelay;
	dwSize = cMsg.dwSize;

    if ( cMsg.dwSize == 0 )
		return *this;

    pBuffer = new BYTE[cMsg.dwSize];
	memcpy( pBuffer, cMsg.pBuffer, cMsg.dwSize );

    return *this;
}

EmulBase::EmulBase()
{
}

EmulBase::~EmulBase()
{
	_clear_emul_msg();
}

// 에뮬레이트에 메시지를 삽입한다.
void EmulBase::_insert_emul_msg(
	NET_MSG_GENERIC *nmg )
{
    if ( nmg->Size() < sizeof( NET_MSG_GENERIC ) ||
		nmg->Size() > NET_DATA_BUFSIZE )
	{
		MIN_ASSERT( 0 && "EmulBase::_insert_emul_msg msg size error" );
		return;
	}

	CEMULMSG* pMsg = new CEMULMSG;
	pMsg->fDelay = 0.1f;
	pMsg->dwSize = nmg->Size();
	pMsg->pBuffer = new BYTE[pMsg->dwSize];
	memcpy( pMsg->pBuffer, nmg, pMsg->dwSize );
	g_vecMsg.push_back( pMsg );
}

void EmulBase::_clear_emul_msg()
{
	std::for_each( g_vecMsg.begin(), g_vecMsg.end(), std_afunc::DeleteObject() );
	g_vecMsg.clear();
}