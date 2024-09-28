#ifndef Param_H
#define Param_H

#include <vector>
#include <cassert>
#include <tchar.h>

/**
	다양한 변수들을 담을 수 있는 그릇
	내부에서는 벡터에 의해 처리 된다.

	resize시에 복사 이동 되므로 복사에 의해 문제가 될 요소는 사용하며 안된다.
	또한 소멸 시 별다른 처리를 하지 않으므로 삭제시 문제가 될 요소 역시 사용하면 안된다.
	또한 별다른 안전성 검열을 하지 않으므로 입력순으로 정확히 출력이 되어야 한다.

 */
class CParam
{
public:
		CParam() : m_write(0), m_read(0) { }

		/**
			입력 연산자
			문자열은 특수 처리 한다.
		 */
		template<class T>
		CParam& 							operator << (T t);
		CParam& 							operator << (const TCHAR* str);
		CParam& 							operator << (TCHAR* str);

		/**
			출력 연산자
			문자열은 특수 처리 한다.
		 */
		template<class T>
		CParam& 							operator >> (T& t);
		CParam& 							operator >> (const TCHAR*& str);
		CParam& 							operator >> (TCHAR*& str);
		CParam&								operator >> (void* fun);

		/**
			출력 상태를 초기화 한다.
		 */
		void								Reset();

		/**
			입출력 상태 및 버퍼를 초기화 한다.
		 */
		void								Clear();

        /**
            현재까지 읽은 것들을 삭제 한다.
         */
        void                                Remove();

protected:
private:

		/**
			문자열의 바이트 사이즈를 출력 한다.

		 */
		size_t								GetByteFromString(const TCHAR* str);

		/* 내부 멤버 */
		typedef std::vector<char>			BufferContainer;
		typedef BufferContainer::iterator	BufferContainerIterator;
		BufferContainer						m_vBuffer;
		size_t								m_write;
		size_t								m_read;
};

#include "Param.inl"

#endif