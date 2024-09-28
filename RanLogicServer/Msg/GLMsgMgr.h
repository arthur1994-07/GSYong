#pragma once

#include <map>

typedef std::map< int, DWORD > MSG_DELAY;
typedef MSG_DELAY::iterator MSG_DELAY_ITER;
typedef MSG_DELAY::const_iterator MSG_DELAY_CITER;
typedef MSG_DELAY::value_type MSG_DELAY_VALUE;

class GLNetMsgDelay
{
public:
	GLNetMsgDelay();
	~GLNetMsgDelay();

public:
	bool InsertNetMsg( int nType, DWORD dwDelayMillisecond  );
	bool DeleteNetMsg( int nType );
	bool UpdateNetMsg( int nType, DWORD dwDelayMillisecond  );
	DWORD GetDelayMillisecond( int nType );
	bool IsNetMsg( int nType );
	void Reset();

protected:
	MSG_DELAY m_theDelay;

};

class GLNetMsgDelayCharacter
{
public:
	GLNetMsgDelayCharacter( int nType, DWORD dwDelayMillisecond );
	~GLNetMsgDelayCharacter();

public:
	bool BlockNetMsg( int nType, DWORD dwDelayMillisecond );

protected:
	MSG_DELAY m_theDelay;

};

class GLNetMsgDelayMgr
{
public:
	GLNetMsgDelayMgr();
	~GLNetMsgDelayMgr();

public:
	typedef std::map< DWORD, GLNetMsgDelayCharacter > MSG_DELAY_CHARACTER;
	typedef MSG_DELAY_CHARACTER::iterator MSG_DELAY_CHARACTER_ITER;
	typedef MSG_DELAY_CHARACTER::const_iterator MSG_DELAY_CHARACTER_CITER;
	typedef MSG_DELAY_CHARACTER::value_type MSG_DELAY_CHARACTER_VALUE;

public:

	// for GLNetMsgDelay
	bool InsertNetMsg( int nType, DWORD dwDelayMillisecond  );
	bool DeleteNetMsg( int nType );
	bool UpdateNetMsg( int nType, DWORD dwDelayMillisecond  );
	void ResetNetMsg();

public:

	bool BlockNetMsg( int nType, DWORD dwChaDbNum );
	void DeleteCharacter( DWORD dwChaDbNum );

protected:
	GLNetMsgDelay m_theDelay;
	MSG_DELAY_CHARACTER m_theDelayCharacter;

};
