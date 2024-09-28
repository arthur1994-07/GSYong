#pragma once

#include <list>
#include <map>

/*
 * GM명령어로 감시 패킷 추가 / 삭제 할 수 있도록 해야 한다.
 * 서버쪽에 설정 파일을 읽어서 하도록 할 수 도 있다.
 * 패킷을 감시하는 것이기 때문에 감시할 패킷이 어떤 서버에서 처리되는지 잘 알아야 하며
 * 추가할 때 주의해야 한다.
 * Desc: 
 */

struct sBlockCharacter 
{
	DWORD dwChaDbNum;
	DWORD dwBlockEndTime;

	sBlockCharacter( DWORD _dwChaDbNum, DWORD _dwBlockEndTime )
		: dwChaDbNum( _dwChaDbNum )
		, dwBlockEndTime( _dwBlockEndTime )
	{
	}
};

struct sPacketCount 
{
	DWORD dwPacketCount;
	DWORD dwRecvTime;

	sPacketCount()
		: dwPacketCount( 1 )
		, dwRecvTime( timeGetTime() )
	{
	}
};

enum eObservePacket
{
	eNomalPacket,		// 감시 대상 패킷이 아니다.
	eVerifyPacketOK,	// 대상 패킷이 맞고, 카운트 성공
	eVerifyPacketBlock,	// 블럭된 캐릭터이다.
};

// 
class GLObservePacket
{
public:
	GLObservePacket( int nPacketNumber, DWORD dwBlockPacketCount, DWORD dwBlockDurationMillisecond, DWORD dwObserveDurationMillisecond );
	~GLObservePacket();

public:
	int VerifyPacket( DWORD dwChaDbNum );
	void Update( DWORD dwCurrentTime );

protected:
	// 감시할 패킷 번호
	int m_nPacketNumber;

	// 이 개수이상 패킷을 보냈을 경우 블럭시킨다.
	DWORD m_dwBlockPacketCount;

	// 걸렸을 경우 블럭시킬 시간 (단위 밀리세컨드: 1000 == 1초)
	DWORD m_dwBlockDurationMillisecond;

	// 얼마동안의 시간 동안 감시를 하는가? (단위 밀리세컨드: 1000 == 1초)
	DWORD m_dwObserveDurationMillisecond;

	// 블럭하고 있는 캐릭터번호 목록: 캐릭터번호 / 블럭 끝나는 시간
	std::map< DWORD, DWORD > m_BlockCharacter;

	// m_nPacketNumber 같은 패킷이 올 때마다 저장을 하고, 카운트를 올린다.
	std::map< DWORD, sPacketCount > m_PacketCount;

};

class GLObservePacketMgr
{
public:
	GLObservePacketMgr();
	~GLObservePacketMgr();

public:
	int VerifyPacket( int nPacketNumber, DWORD dwChaDbNum );
	void Update();

	void AddObservePacket( int nPacketNumber, DWORD dwBlockPacketCount, DWORD dwBlockDurationMillisecond, DWORD dwObserveDurationMillisecond );
	void DeleteObservePacket( int nPacketNumber );
	void GetObservePacket( std::vector< GLObservePacket >& vecObservePacket );

protected:
	void createObservePacket();

protected:
	std::map< int, GLObservePacket > m_ObservePacket;
	DWORD m_dwUpdateTimer;

};
