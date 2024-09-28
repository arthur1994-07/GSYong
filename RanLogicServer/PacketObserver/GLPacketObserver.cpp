#include "pch.h"
#include "../../RanLogic/s_NetGlobal.h"

#include "./GLPacketObserver.h"
#include "../../SigmaCore/DebugInclude.h"


GLObservePacket::GLObservePacket( int nPacketNumber, DWORD dwBlockPacketCount, DWORD dwBlockDurationMillisecond, DWORD dwObserveDurationMillisecond )
: m_nPacketNumber( nPacketNumber )
, m_dwBlockPacketCount( dwBlockPacketCount )
, m_dwBlockDurationMillisecond( dwBlockDurationMillisecond )
, m_dwObserveDurationMillisecond( dwObserveDurationMillisecond )
{

}

GLObservePacket::~GLObservePacket()
{
}

int GLObservePacket::VerifyPacket( DWORD dwChaDbNum )
{
	std::map< DWORD, DWORD >::iterator itBlockCharacter = m_BlockCharacter.find( dwChaDbNum );
	if ( m_BlockCharacter.end() != itBlockCharacter )
	{
		// ���� ĳ�����̴�.
		return eVerifyPacketBlock;
	}

	std::map< DWORD, sPacketCount >::iterator itPacketCount = m_PacketCount.find( dwChaDbNum );
	if ( m_PacketCount.end() == itPacketCount )
	{
		m_PacketCount.insert( std::map< DWORD, sPacketCount >::value_type( dwChaDbNum, sPacketCount() ) );
	}
	else
	{
		sPacketCount& sCount = itPacketCount->second;
		sCount.dwPacketCount++;
	}

	return eVerifyPacketOK;
}

void GLObservePacket::Update( DWORD dwCurrentTime )
{
	// ���ð� ����� ĳ���� ó��
	std::map< DWORD, DWORD >::iterator itBlockCharacter = m_BlockCharacter.begin();
	std::map< DWORD, DWORD >::iterator itBlockCharacterEnd = m_BlockCharacter.end();
	for ( ; itBlockCharacter != itBlockCharacterEnd; )
	{
		DWORD& dwEndTime = itBlockCharacter->second;
		if ( dwCurrentTime >= dwEndTime )
		{
			itBlockCharacter = m_BlockCharacter.erase( itBlockCharacter );
		}
		else
		{
			itBlockCharacter++;
		}
	}

	if ( m_PacketCount.empty() )
	{
		return;
	}

	std::map< DWORD, sPacketCount >::iterator itPacketCount = m_PacketCount.begin();
	std::map< DWORD, sPacketCount >::iterator itPacketCountEnd = m_PacketCount.end();
	for ( ; itPacketCount != itPacketCountEnd; )
	{
		sPacketCount& sCount = itPacketCount->second;
		if ( dwCurrentTime >= sCount.dwRecvTime + m_dwObserveDurationMillisecond )
		{
			if ( sCount.dwPacketCount >= m_dwBlockPacketCount )
			{
				// ������ �� �̻����� ��Ŷ�� ���� ĳ���ʹ� �� ��Ų��.
				m_BlockCharacter.insert( std::map< DWORD, DWORD >::value_type( itPacketCount->first, dwCurrentTime + m_dwBlockDurationMillisecond ) );
			}
			itPacketCount = m_PacketCount.erase( itPacketCount );
		}
		else
		{
			itPacketCount++;
		}
	}
}

GLObservePacketMgr::GLObservePacketMgr()
: m_dwUpdateTimer( timeGetTime() )
{
	createObservePacket();
}

GLObservePacketMgr::~GLObservePacketMgr()
{

}

int GLObservePacketMgr::VerifyPacket( int nPacketNumber, DWORD dwChaDbNum )
{
	std::map< int, GLObservePacket >::iterator it = m_ObservePacket.find( nPacketNumber );
	if ( m_ObservePacket.end() == it )
	{
		return eNomalPacket;
	}
	else
	{
		GLObservePacket& cObservePacket = it->second;
		return cObservePacket.VerifyPacket( dwChaDbNum );
	}
}

void GLObservePacketMgr::Update()
{
	if ( m_ObservePacket.empty() )
	{
		return;
	}

	DWORD dwCurrentTime	= timeGetTime();

	if ( ( dwCurrentTime - m_dwUpdateTimer ) > 1000/* 1�� */ )
	{
		m_dwUpdateTimer = dwCurrentTime;
		std::map< int, GLObservePacket >::iterator itObservePacket = m_ObservePacket.begin();
		std::map< int, GLObservePacket >::iterator itObservePacketEnd = m_ObservePacket.end();
		for ( ; itObservePacket != itObservePacketEnd; itObservePacket++ )
		{
			GLObservePacket& cObservePacket = itObservePacket->second;
			cObservePacket.Update( dwCurrentTime );
		}
	}
}

void GLObservePacketMgr::AddObservePacket( int nPacketNumber, DWORD dwBlockPacketCount, DWORD dwBlockDurationMillisecond, DWORD dwObserveDurationMillisecond )
{
	GLObservePacket cObservePacket( nPacketNumber, dwBlockPacketCount, dwBlockDurationMillisecond, dwObserveDurationMillisecond );
	m_ObservePacket.insert( std::map< int, GLObservePacket >::value_type( NET_MSG_GCTRL_TRADE, cObservePacket ) );
}

void GLObservePacketMgr::DeleteObservePacket( int nPacketNumber )
{
	m_ObservePacket.erase( nPacketNumber );
}

void GLObservePacketMgr::GetObservePacket( std::vector< GLObservePacket >& vecObservePacket )
{
	if ( m_ObservePacket.empty() )
	{
		return;
	}

	std::map< int, GLObservePacket >::iterator itObservePacket = m_ObservePacket.begin();
	std::map< int, GLObservePacket >::iterator itObservePacketEnd = m_ObservePacket.end();
	for ( ; itObservePacket != itObservePacketEnd; itObservePacket++ )
	{
		vecObservePacket.push_back( itObservePacket->second );
	}
}

void GLObservePacketMgr::createObservePacket()
{
	// Ʈ���̵� ��û ��Ŷ�� 5�ʵ��� 5���̻� �������� Ȯ���ϰ�, ���̻� ���� ĳ���͸� 60�ʵ��� ����Ű�� ��ü.
	AddObservePacket( NET_MSG_GCTRL_TRADE, 5, 60000, 5000 );
}
