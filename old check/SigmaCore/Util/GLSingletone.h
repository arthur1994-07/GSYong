#pragma once

// SigmaCore 에서 사용하기 위해 어쩔 수 없이 여기에 하나 만든다. 엔진에도 이거 하나 있다. 나중에 정리 필요

class BasicCriticalSection
{
private:
	CRITICAL_SECTION m_Win32CS;

public:
	BasicCriticalSection() { ::InitializeCriticalSection( &m_Win32CS ); }
	virtual ~BasicCriticalSection() { ::DeleteCriticalSection( &m_Win32CS ); }

	void enter() { ::EnterCriticalSection( &m_Win32CS ); }
	void leave() { ::LeaveCriticalSection( &m_Win32CS ); }

private:
	BasicCriticalSection( const BasicCriticalSection& ) {}
	const BasicCriticalSection& operator = ( const BasicCriticalSection& ) { return *this; }
};

//
//  CSingletone
//
template < typename TYPE > 
class CSingletone
{
protected:
	static TYPE* volatile s_pInstance;
	static BasicCriticalSection s_InstanceSection; 

protected:
	CSingletone() { atexit( clear ); }
	virtual ~CSingletone() {};

public:
	static TYPE* Instance( void )
	{
		if ( s_pInstance == 0 )
		{
			s_InstanceSection.enter();
			if ( s_pInstance == 0 )
			{
				s_pInstance = new TYPE;
			}
			s_InstanceSection.leave();
		}
		return s_pInstance;
	}

	static void clear( void )
	{
		if ( s_pInstance != 0 )
		{
			s_InstanceSection.enter();
			if ( s_pInstance != 0 )
			{
				delete s_pInstance;
				s_pInstance = 0;
			}
			s_InstanceSection.leave();
		}
	}

private:
	CSingletone( const CSingletone<TYPE>& ) {}
	const CSingletone< TYPE >& operator = ( const CSingletone< TYPE >& ) { return *this; }
};

template < typename TYPE >
TYPE* volatile CSingletone< TYPE >::s_pInstance = 0;

template < typename TYPE >
BasicCriticalSection CSingletone< TYPE >::s_InstanceSection;