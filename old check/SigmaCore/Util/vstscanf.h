#include <stdarg.h>

/**
	vc++ 에서는 가변 인자 리스트 Scanf 입력 함수를 지원하지 않는다.
    이 함수는 이를 가능케 한다.

    주의할 점은 내부적으로 _stscanf_s 를 사용하고 있으므로
    스트링을 읽을 때는 스트링의 사이즈를 반드시 알려주어야 한다.

    예를들어 (%s %d ...) 형식으로 읽어 들인다면 
    strBuff, strBuffSize, int... 형식으로 스트링 버퍼의 크기(갯수)를 입력해 주어야 한다.

    참조 사이트
    http://www.gamedev.net/topic/310888-no-vfscanf-in-visual-studio/

	\param buf 스캔할 문자열 버퍼
	\param format 스캔할 포멧
	\param argPtr 가변 인자 리스트
	\return 스캔된 포멧 갯 수
 */
int _vstscanf_s(const TCHAR* buf, const TCHAR* format, va_list argPtr);