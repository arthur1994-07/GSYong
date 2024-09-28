

#ifndef _mrcSingleton_h_
#define _mrcSingleton_h_

#include <loki/Singleton.h>

namespace Loki
{
	namespace LongevityLifetime
	{
		// ���� ����
	
		// DieFirst


		template <class T> struct DieDependencyMgr : SingletonFixedLongevity<0x900 ,T> {};
		// �ؿ��� �߰�
		template <class T> struct DieIndependencyMgr : SingletonFixedLongevity<0x1000 ,T> {};

		// ������ �ٷ����� ����Ǿ�� �Ұ͵�
		template <class T> struct DieDirectlyBeforeRenderer : SingletonFixedLongevity<0x1990 ,T> {};	

		// ��� �ý��� �޴����� ���ؽ� ������ ������ �ִ� �޴����� ������� ����Ǿ����
		template <class T> struct DieRenderer : SingletonFixedLongevity<0x2000 ,T> {};	

		// DieDirectlyBeforeLast
		// DieLast

	}
}



#endif