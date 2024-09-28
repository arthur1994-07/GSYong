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
	
	// 남아있는 재사용 대기시간을 돌려준다.
	// 단위: millisecond
	DWORD GetRemainReuseWaitTime();

public:
	// 재사용 대기시간인지 질의
	// bSetReuseWaitTime true : 재사용 대기시간이 아니라면 자동으로 다음 사용 시간을 세팅한다.
	// bSetReuseWaitTime false : 단순 확인 용도, 호출 후 실제 재사용 대기시간이 아니고 실제 사용이 된다면 꼭 재사용 대기시간을(SetReuseWaitTime) 호출해줘야 한다.
	// true: 재사용 대기시간이다.
	// false: 재사용 대기시간이 아니다.
	bool IsReuseWaitTime( bool bSetReuseWaitTime );

	// 재사용 대기시간이 아닐 경우 사용을 한 후 재사용 대기시간을 설정한다.
	// dwReuseWaitTime 값을(단위: millisecond) 설정할 경우 그 시간을 더해서 사용가능한 시간을 설정한다.
	// dwReuseWaitTime 이 GAEAID_NULL 일 경우 기본 설정된 m_dwReuseWaitTime 값을 더해준다.
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

	// 사용가능한 시간
	DWORD m_dwUsableTime;
	
	// 재사용 대기시간(고정값, 단위: millisecond)
	// m_dwUsableTime 에 더해지는 값이다.
	DWORD m_dwReuseWaitTime;

};

class GLReuseWaitTimeClient
{
public:
	GLReuseWaitTimeClient();
	~GLReuseWaitTimeClient();

public:
	// 관리하고 싶은 재사용 대기시간 컨텐츠를 삽입한다. 
	// lua 에서 읽은 값을 사용하는 경우도 있기 때문에 호출 시기를 주의한다.
	void InsertReuseWaitTime();
	DWORD GetRemainReuseWaitTime( int nType );
	bool IsReuseWaitTime( int nType );

protected:
	std::vector< GLReuseWaitTime > m_vecReuseWaitTime;

};