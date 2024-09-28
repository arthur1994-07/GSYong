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
		// �ִ�.
		// ������ �ð��� ������?
		DWORD& dwLastCalled = ( *it ).second;
		if ( dwCurrentTime > dwLastCalled + dwDelayMillisecond )
		{
			// �����̽ð��� ������. �޽��� ó�� �����ϴ�.
			// ������ �ð� �����ϰ�
			// �����Ѵ�.
			dwLastCalled = dwCurrentTime;
			return false;
		}
		else
		{
			// �����̽ð��� ������ �ʾҴ�.
			// �޽��� ó�� �Ұ��ϴ�.
			return true;
		}
	}
	else
	{
		// ������ �ð��� ������� �ʾҴ�.
		// �޽��� ó�� �����ϴ�.
		std::pair< MSG_DELAY_ITER, bool > retPair = m_theDelay.insert( MSG_DELAY_VALUE( nType, dwCurrentTime ) );
		return false;
	}
}
// ------- GLNetMsgDelayCharacter --------------



// ------- GLNetMsgDelayMgr -----------------
GLNetMsgDelayMgr::GLNetMsgDelayMgr()
{
	// �׽�Ʈ �뵵 1.5��
	InsertNetMsg( NET_MSG_GCTRL_STORAGE_SAVE_MONEY, 1500 );
	InsertNetMsg( NET_MSG_GCTRL_STORAGE_DRAW_MONEY, 1500 );

	// ���������� �������� �޽��� Ŭ���̾�Ʈ������ üũ�ϰ� �־ �߰���
	InsertNetMsg( NET_MSG_GET_CHARGED_ITEM_CF, 60000 );

	// ������ ���� �޽��� ������ �߰�
	InsertNetMsg( NET_MSG_GCTRL_REQ_INVEN_TO_SLOT, 500 );
	InsertNetMsg( NET_MSG_GCTRL_REQ_INVEN_EX_INVEN, 500 );
	InsertNetMsg( NET_MSG_GCTRL_REQ_STORAGE_EX_INVEN, 500 );
	InsertNetMsg( NET_MSG_GCTRL_REQ_SLOT_EX_INVEN, 500 );
	InsertNetMsg( NET_MSG_GCTRL_REQ_HOLD_TO_INVEN, 500 );
	InsertNetMsg( NET_MSG_GCTRL_REQ_INVEN_TO_INVEN, 500 );
	InsertNetMsg( NET_MSG_GCTRL_REQ_STORAGE_TO_INVEN, 500 );
	InsertNetMsg( NET_MSG_GCTRL_REQ_HOLD_TO_SLOT, 500 );

	// ��ųâ���� ��ų�ʱ�ȭ ��û, 1.5�ʷ� ����
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
	// ����
	// true : ����ų �޽����̴�.
	// false : ����Ű�� �ʴ´�.
	DWORD dwDelayMillisecond = 0;
	dwDelayMillisecond = m_theDelay.GetDelayMillisecond( nType );
	if ( 0 == dwDelayMillisecond )
	{
		return false;
	}

	// ������ insert �ϰ� false
	// ������ �ð� üũ
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