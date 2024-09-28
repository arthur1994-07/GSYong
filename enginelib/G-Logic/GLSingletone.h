// #pragma once
// 
// // 다른 곳에서도 사용하게 하기 위하여 Singletone 클래스를 분리한다;
// 
// class BasicCriticalSection
// {
// private:
// 	CRITICAL_SECTION m_Win32CS;
// 
// public:
// 	BasicCriticalSection() { ::InitializeCriticalSection( &m_Win32CS ); }
// 	virtual ~BasicCriticalSection() { ::DeleteCriticalSection( &m_Win32CS ); }
// 
// 	void enter() { ::EnterCriticalSection( &m_Win32CS ); }
// 	void leave() { ::LeaveCriticalSection( &m_Win32CS ); }
// 
// private:
// 	BasicCriticalSection( const BasicCriticalSection& ) {}
// 	const BasicCriticalSection& operator = ( const BasicCriticalSection& ) { return *this; }
// };
// 
// //
// //  CSingletone
// //
// template < typename TYPE > 
// class CSingletone
// {
// protected:
// 	static TYPE* volatile s_pInstance;
// 	static BasicCriticalSection s_InstanceSection; 
// 
// protected:
// 	CSingletone() { atexit( clear ); }
// 	virtual ~CSingletone() {};
// 
// public:
// 	static TYPE* Instance( void )
// 	{
// 		if ( s_pInstance == 0 )
// 		{
// 			s_InstanceSection.enter();
// 			if ( s_pInstance == 0 )
// 			{
// 				s_pInstance = new TYPE;
// 			}
// 			s_InstanceSection.leave();
// 		}
// 		return s_pInstance;
// 	}
// 
// 	static void clear( void )
// 	{
// 		if ( s_pInstance != 0 )
// 		{
// 			s_InstanceSection.enter();
// 			if ( s_pInstance != 0 )
// 			{
// 				delete s_pInstance;
// 				s_pInstance = 0;
// 			}
// 			s_InstanceSection.leave();
// 		}
// 	}
// 
// private:
// 	CSingletone( const CSingletone<TYPE>& ) {}
// 	const CSingletone< TYPE >& operator = ( const CSingletone< TYPE >& ) { return *this; }
// };
// 
// template < typename TYPE >
// TYPE* volatile CSingletone< TYPE >::s_pInstance = 0;
// 
// template < typename TYPE >
// BasicCriticalSection CSingletone< TYPE >::s_InstanceSection;