#pragma once

#include <string>
#include <vector>

enum REUSE_WAIT_TYPE
{
	REUSE_WAIT_TYPE_ITEMBANK, 
	REUSE_WAIT_TYPE_PRIVATE_MARKET_ITEM_SEARCH, 
	REUSE_WAIT_TYPE_PRIVATE_MARKET_LOG_SEARCH, 
};

class GLReuseWaitTime
{
public:
	GLReuseWaitTime( int nType, DWORD dwReuseWaitTime );
	virtual ~GLReuseWaitTime();

public:
	DWORD GetUsableTime() { return m_dwUsableTime; }
	
	// �����ִ� ���� ���ð��� �����ش�.
	// ����: millisecond
	DWORD GetRemainReuseWaitTime();

public:
	// ���� ���ð����� ����
	// bSetReuseWaitTime true : ���� ���ð��� �ƴ϶�� �ڵ����� ���� ��� �ð��� �����Ѵ�.
	// bSetReuseWaitTime false : �ܼ� Ȯ�� �뵵, ȣ�� �� ���� ���� ���ð��� �ƴϰ� ���� ����� �ȴٸ� �� ���� ���ð���(SetReuseWaitTime) ȣ������� �Ѵ�.
	// true: ���� ���ð��̴�.
	// false: ���� ���ð��� �ƴϴ�.
	bool IsReuseWaitTime( bool bSetReuseWaitTime );

	// ���� ���ð��� �ƴ� ��� ����� �� �� ���� ���ð��� �����Ѵ�.
	// dwReuseWaitTime ����(����: millisecond) ������ ��� �� �ð��� ���ؼ� ��밡���� �ð��� �����Ѵ�.
	// dwReuseWaitTime �� GAEAID_NULL �� ��� �⺻ ������ m_dwReuseWaitTime ���� �����ش�.
	void SetReuseWaitTime( DWORD dwReuseWaitTime = GAEAID_NULL );

public:
	bool operator == ( const GLReuseWaitTime& rvalue ) const
	{
		if ( m_nType == rvalue.m_nType )
			return true;

		return false;
	}

protected:
	// REUSE_WAIT_TYPE
	int m_nType;

	// ��밡���� �ð�
	DWORD m_dwUsableTime;
	
	// ���� ���ð�(������, ����: millisecond)
	// m_dwUsableTime �� �������� ���̴�.
	DWORD m_dwReuseWaitTime;

};

class GLReuseWaitTimeClient
{
public:
	GLReuseWaitTimeClient();
	~GLReuseWaitTimeClient();

public:
	// �����ϰ� ���� ���� ���ð� �������� �����Ѵ�. 
	// lua ���� ���� ���� ����ϴ� ��쵵 �ֱ� ������ ȣ�� �ñ⸦ �����Ѵ�.
	void InsertReuseWaitTime();
	DWORD GetRemainReuseWaitTime( int nType );
	bool IsReuseWaitTime( int nType );

protected:
	std::vector< GLReuseWaitTime > m_vecReuseWaitTime;

};