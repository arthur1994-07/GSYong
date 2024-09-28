#include "pch.h"

#include "../../RanLogic/s_NetGlobal.h"

#include "./GLMsgMgr.h"

#include "../../SigmaCore/DebugInclude.h"


// ------- GLNetMsgDelay --------------------
GLNetMsgDelay::GLNetMsgDelay()
{

}

GLNetMsgDelay::~GLNetMsgDelay()
{
	Reset();
}

bool GLNetMsgDelay::InsertNetMsg( int nType, DWORD dwDelayMillisecond )
{
	std::pair< MSG_DELAY_ITER, bool > retPair = m_theDelay.insert( MSG_DELAY_VALUE( nType, dwDelayMillisecond ) );
	return retPair.second;
}

bool GLNetMsgDelay::DeleteNetMsg( int nType )
{
	MSG_DELAY_ITER it = m_theDelay.find( nType );
	if ( it != m_theDelay.end() )
	{	
		m_theDelay.erase( it );
		return true;
	}
	return false;
}

bool GLNetMsgDelay::UpdateNetMsg( int nType, DWORD dwDelayMillisecond )
{
	MSG_DELAY_ITER it = m_theDelay.find( nType );
	if ( it != m_theDelay.end() )
	{
		( *it ).second = dwDelayMillisecond;
		return true;
	}
	return false;
}

DWORD GLNetMsgDelay::GetDelayMillisecond( int nType )
{
	MSG_DELAY_ITER it = m_theDelay.find( nType );
	if ( it != m_theDelay.end() )
	{
		return ( *it ).second;
	}
	return 0;
}

bool GLNetMsgDelay::IsNetMsg( int nType )
{
	MSG_DELAY_ITER it = m_theDelay.find( nType );
	return ( it != m_theDelay.end() ) ? true : false;
}

void GLNetMsgDelay::Reset()
{
	m_theDelay.clear();
}
// ------- GLNetMsgDelay --------------------



// ------- GLNetMsgDelayCharacter --------------
GLNetMsgDelayCharacter::GLNetMsgDelayCharacter( int nType, DWORD dwDelayMillisecond )
{
	BlockNetMsg( nType, dwDelayMillisecond );
}

GLNetMsgDelayCharacter::~GLNetMsgDelayCharacter()
{
	m_theDelay.clear();
}

bool GLNetMsgDelayCharacter::BlockNetMsg( int nType, DWORD dwDelayMillisecond )
{
	DWORD dwCurrentTime = ::GetTickCount();
	MSG_DELAY_ITER it = m_theDelay.find( nType );
	if ( it != m_theDelay.end() )
	{
		// 있다.
		// 딜레이 시간이 지났나?
		DWORD& dwLastCalled = ( *it ).second;
		if ( dwCurrentTime > dwLastCalled + dwDelayMillisecond )
		{
			// 딜레이시간이 지났다. 메시지 처리 가능하다.
			// 마지막 시간 저장하고
			// 리턴한다.
			dwLastCalled = dwCurrentTime;
			return false;
		}
		else
		{
			// 딜레이시간이 지나지 않았다.
			// 메시지 처리 불가하다.
			return true;
		}
	}
	else
	{
		// 딜레이 시간이 저장되지 않았다.
		// 메시지 처리 가능하다.
		std::pair< MSG_DELAY_ITER, bool > retPair = m_theDelay.insert( MSG_DELAY_VALUE( nType, dwCurrentTime ) );
		return false;
	}
}
// ------- GLNetMsgDelayCharacter --------------



// ------- GLNetMsgDelayMgr -----------------
GLNetMsgDelayMgr::GLNetMsgDelayMgr()
{
	// 테스트 용도 1.5초
	InsertNetMsg( NET_MSG_GCTRL_STORAGE_SAVE_MONEY, 1500 );
	InsertNetMsg( NET_MSG_GCTRL_STORAGE_DRAW_MONEY, 1500 );

	// 빌링아이템 가져오는 메시지 클라이언트에서만 체크하고 있어서 추가함
	InsertNetMsg( NET_MSG_GET_CHARGED_ITEM_CF, 60000 );

	// 아이템 관련 메시지 딜레이 추가
	InsertNetMsg( NET_MSG_GCTRL_REQ_INVEN_TO_SLOT, 500 );
	InsertNetMsg( NET_MSG_GCTRL_REQ_INVEN_EX_INVEN, 500 );
	InsertNetMsg( NET_MSG_GCTRL_REQ_STORAGE_EX_INVEN, 500 );
	InsertNetMsg( NET_MSG_GCTRL_REQ_SLOT_EX_INVEN, 500 );
	InsertNetMsg( NET_MSG_GCTRL_REQ_HOLD_TO_INVEN, 500 );
	InsertNetMsg( NET_MSG_GCTRL_REQ_INVEN_TO_INVEN, 500 );
	InsertNetMsg( NET_MSG_GCTRL_REQ_STORAGE_TO_INVEN, 500 );
	InsertNetMsg( NET_MSG_GCTRL_REQ_HOLD_TO_SLOT, 500 );

	// 스킬창에서 스킬초기화 요청, 1.5초로 설정
	InsertNetMsg( NET_MSG_SKILL_RESET_CF, 1500 );
}

GLNetMsgDelayMgr::~GLNetMsgDelayMgr()
{

}

bool GLNetMsgDelayMgr::InsertNetMsg( int nType, DWORD dwDelayMillisecond )
{
	return m_theDelay.InsertNetMsg( nType, dwDelayMillisecond );
}

bool GLNetMsgDelayMgr::DeleteNetMsg( int nType )
{
	return m_theDelay.DeleteNetMsg( nType );
}

bool GLNetMsgDelayMgr::UpdateNetMsg( int nType, DWORD dwDelayMillisecond )
{
	return m_theDelay.UpdateNetMsg( nType, dwDelayMillisecond );
}

void GLNetMsgDelayMgr::ResetNetMsg()
{
	m_theDelay.Reset();
}

bool GLNetMsgDelayMgr::BlockNetMsg( int nType, DWORD dwChaDbNum )
{
	// 리턴
	// true : 블럭시킬 메시지이다.
	// false : 블럭시키지 않는다.
	DWORD dwDelayMillisecond = 0;
	dwDelayMillisecond = m_theDelay.GetDelayMillisecond( nType );
	if ( 0 == dwDelayMillisecond )
	{
		return false;
	}

	// 없으면 insert 하고 false
	// 있으면 시간 체크
	MSG_DELAY_CHARACTER_ITER it = m_theDelayCharacter.find( dwChaDbNum );
	if ( it != m_theDelayCharacter.end() )
	{
		GLNetMsgDelayCharacter& sDelay = ( *it ).second;
		return sDelay.BlockNetMsg( nType, dwDelayMillisecond );
	}
	else
	{
		GLNetMsgDelayCharacter sDelay( nType, dwDelayMillisecond );
		std::pair< MSG_DELAY_CHARACTER_ITER, bool > retPair = m_theDelayCharacter.insert( MSG_DELAY_CHARACTER_VALUE( dwChaDbNum, sDelay ) );
		return false;
	}
}

void GLNetMsgDelayMgr::DeleteCharacter( DWORD dwChaDbNum )
{
	MSG_DELAY_CHARACTER_ITER it = m_theDelayCharacter.find( dwChaDbNum );
	if ( it != m_theDelayCharacter.end() )
	{	
		m_theDelayCharacter.erase( it );
	}
}

// ------- GLNetMsgDelayMgr -----------------