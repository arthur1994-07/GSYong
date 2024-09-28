

#ifndef _mrcSingleton_h_
#define _mrcSingleton_h_

#include <loki/Singleton.h>

namespace Loki
{
	namespace LongevityLifetime
	{
		// 종료 순서
	
		// DieFirst


		template <class T> struct DieDependencyMgr : SingletonFixedLongevity<0x900 ,T> {};
		// 밑에건 중간
		template <class T> struct DieIndependencyMgr : SingletonFixedLongevity<0x1000 ,T> {};

		// 렌더러 바로전에 종료되어야 할것들
		template <class T> struct DieDirectlyBeforeRenderer : SingletonFixedLongevity<0x1990 ,T> {};	

		// 적어도 택스쳐 메니저와 버텍스 정보를 가지고 있는 메니저가 종료된후 종료되어야함
		template <class T> struct DieRenderer : SingletonFixedLongevity<0x2000 ,T> {};	

		// DieDirectlyBeforeLast
		// DieLast

	}
}



#endif