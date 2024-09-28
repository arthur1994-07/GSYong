#ifndef TCreateLink_H
#define TCreateLink_H

/**	
	���� �����ڷ� ���еǴ� ��ü�� �����ϱ� ���� ���ø�

	���� ��ü�� �����ϱ� ���ؼ��� ���� �����ڸ� ������.(enum �̳� ��� ��)
	�̶� ���� ����ġ ���̳� if �� ������ ��ü�� ���� �Ѵ�.
	�̸� �ڵ�ȭ ���ڴ� �ǵ����� ������ �ڵ�

	�Ʒ� ���� ���� ����, ��� �׸��� ���� �ϴµ� ���� ���̴�.

	// ���� ������
	enum 
	{
		ID_1,
		ID_2,
		ID_3,
		...
		ID_END,
	};

	// ���� ������ Ÿ��
	typedef int IDType;

	// ���� ���� ��ü ����
	DecleareCreateSingleLink(class ConcreteClassEnd, IDType, ID_End, CreatorLinkEnd);
	...
	DecleareCreateDoubleLink(class ConcreteClass2, IDType, ID_2, CreatorLink3, CreatorLink2);
	DecleareCreateDoubleLink(class ConcreteClass1, IDType, ID_1, CreatorLink2, RootCreator);

	// ���
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
			assert(0 && _T("�߸��� ID �� ���� ������Ʈ ���� ��û!!!"));
			return 0;
		}
	}

protected:
	TCreateObject<T, I, i> m_front;
};

#define DecleareCreateSingleLink(class, type, idx, name)		typedef TCreateSingleLink<class, type, idx> name;
#define DecleareCreateDoubleLink(class, type, idx, link, name)	typedef TCreateDoubleLink<class, type, idx, link> name;

#endif