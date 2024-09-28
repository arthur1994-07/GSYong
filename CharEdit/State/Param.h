#ifndef Param_H
#define Param_H

#include <vector>
#include <cassert>
#include <tchar.h>

/**
	�پ��� �������� ���� �� �ִ� �׸�
	���ο����� ���Ϳ� ���� ó�� �ȴ�.

	resize�ÿ� ���� �̵� �ǹǷ� ���翡 ���� ������ �� ��Ҵ� ����ϸ� �ȵȴ�.
	���� �Ҹ� �� ���ٸ� ó���� ���� �����Ƿ� ������ ������ �� ��� ���� ����ϸ� �ȵȴ�.
	���� ���ٸ� ������ �˿��� ���� �����Ƿ� �Է¼����� ��Ȯ�� ����� �Ǿ�� �Ѵ�.

 */
class CParam
{
public:
		CParam() : m_write(0), m_read(0) { }

		/**
			�Է� ������
			���ڿ��� Ư�� ó�� �Ѵ�.
		 */
		template<class T>
		CParam& 							operator << (T t);
		CParam& 							operator << (const TCHAR* str);
		CParam& 							operator << (TCHAR* str);

		/**
			��� ������
			���ڿ��� Ư�� ó�� �Ѵ�.
		 */
		template<class T>
		CParam& 							operator >> (T& t);
		CParam& 							operator >> (const TCHAR*& str);
		CParam& 							operator >> (TCHAR*& str);
		CParam&								operator >> (void* fun);

		/**
			��� ���¸� �ʱ�ȭ �Ѵ�.
		 */
		void								Reset();

		/**
			����� ���� �� ���۸� �ʱ�ȭ �Ѵ�.
		 */
		void								Clear();

        /**
            ������� ���� �͵��� ���� �Ѵ�.
         */
        void                                Remove();

protected:
private:

		/**
			���ڿ��� ����Ʈ ����� ��� �Ѵ�.

		 */
		size_t								GetByteFromString(const TCHAR* str);

		/* ���� ��� */
		typedef std::vector<char>			BufferContainer;
		typedef BufferContainer::iterator	BufferContainerIterator;
		BufferContainer						m_vBuffer;
		size_t								m_write;
		size_t								m_read;
};

#include "Param.inl"

#endif