#ifndef TCreateLink_H
#define TCreateLink_H

/**	
	고유 구분자로 구분되는 객체를 생성하기 위한 템플릿

	보통 객체를 생성하기 위해서는 고유 구분자를 가진다.(enum 이나 상수 등)
	이때 보통 스위치 문이나 if 문 등으로 객체를 생성 한다.
	이를 자동화 하자는 의도에서 구현된 코드

	아래 예를 보고 이해, 사용 그리고 개선 하는데 쉬울 것이다.

	// 고유 구분자
	enum 
	{
		ID_1,
		ID_2,
		ID_3,
		...
		ID_END,
	};

	// 고유 구분자 타입
	typedef int IDType;

	// 생성 헬퍼 객체 선언
	DecleareCreateSingleLink(class ConcreteClassEnd, IDType, ID_End, CreatorLinkEnd);
	...
	DecleareCreateDoubleLink(class ConcreteClass2, IDType, ID_2, CreatorLink3, CreatorLink2);
	DecleareCreateDoubleLink(class ConcreteClass1, IDType, ID_1, CreatorLink2, RootCreator);

	// 사용
	RootCreator creator;
	ConcreteClass1* p = creator.Create<ConcreteClass1>(ID_1);

 */

#include <assert.h>

template<class T, class I, I id>
class TCreateObject
{
public:
	T* Create() { return new T; }
	I GetID() { return id; }
};

template<class T, class I, I i, class S>
class TCreateDoubleLink
{
public:
	template<class A>
	A*  Create(I id)
	{
		if (m_front.GetID() == id)
		{
			return dynamic_cast<A*>(m_front.Create());
		}
		else
		{
			return m_back.Create<A>(id);
		}
	}

protected:
	TCreateObject<T, I, i> m_front;
	S m_back;
};

template<class T, class I, I i>
class TCreateSingleLink
{
public:
	template<class A>
	A*  Create(I id)
	{
		if (m_front.GetID() == id)
		{
			return dynamic_cast<A*>(m_front.Create());
		}
		else
		{
			assert(0 && _T("잘못된 ID 를 통해 오브젝트 생성 요청!!!"));
			return 0;
		}
	}

protected:
	TCreateObject<T, I, i> m_front;
};

#define DecleareCreateSingleLink(class, type, idx, name)		typedef TCreateSingleLink<class, type, idx> name;
#define DecleareCreateDoubleLink(class, type, idx, link, name)	typedef TCreateDoubleLink<class, type, idx, link> name;

#endif