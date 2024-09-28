#pragma once

#include <list>
#include <map>

/*
 * GM��ɾ�� ���� ��Ŷ �߰� / ���� �� �� �ֵ��� �ؾ� �Ѵ�.
 * �����ʿ� ���� ������ �о �ϵ��� �� �� �� �ִ�.
 * ��Ŷ�� �����ϴ� ���̱� ������ ������ ��Ŷ�� � �������� ó���Ǵ��� �� �˾ƾ� �ϸ�
 * �߰��� �� �����ؾ� �Ѵ�.
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
	eNomalPacket,		// ���� ��� ��Ŷ�� �ƴϴ�.
	eVerifyPacketOK,	// ��� ��Ŷ�� �°�, ī��Ʈ ����
	eVerifyPacketBlock,	// ���� ĳ�����̴�.
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
	// ������ ��Ŷ ��ȣ
	int m_nPacketNumber;

	// �� �����̻� ��Ŷ�� ������ ��� ����Ų��.
	DWORD m_dwBlockPacketCount;

	// �ɷ��� ��� ����ų �ð� (���� �и�������: 1000 == 1��)
	DWORD m_dwBlockDurationMillisecond;

	// �󸶵����� �ð� ���� ���ø� �ϴ°�? (���� �и�������: 1000 == 1��)
	DWORD m_dwObserveDurationMillisecond;

	// ���ϰ� �ִ� ĳ���͹�ȣ ���: ĳ���͹�ȣ / �� ������ �ð�
	std::map< DWORD, DWORD > m_BlockCharacter;

	// m_nPacketNumber ���� ��Ŷ�� �� ������ ������ �ϰ�, ī��Ʈ�� �ø���.
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
