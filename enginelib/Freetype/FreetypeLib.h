#ifndef FreetypeLib_H
#define FreetypeLib_H

namespace FreetypeLib
{
	/**
		프리타입 라이브러리를 초기화 한다.
		프로그램 실행 중 단 1번만 호출 되어야 한다.
		
		\param lang 언어 코드
		\return 초기화 성공 유무
	 */
	bool InitFreetype(int lang);

	/**
		프리타입 라이브러리를 파괴 한다.
		프로그램 종료시 1번만 호출 되어야 한다.

	 */
	void DestroyFreetype();

	/**
		입력되는 문자열의 넓이와 높이를 프리타입 라이브러리를 통해 계산한다.
		
		\param pStr 면적을 구할 문자열
		\param nFontSize 문자열 크기
		\param nWidth 계산된 문자열 넓이
		\param nHeight 계산된 문자열 높이
		\return 계산 성공 유무
	 */
	bool GetExtentText(const wchar_t* pStr, int nFontSize, int& nWidth, int& nHeight);

	/**
		입력되는 문자열의 비트맵을 버퍼에 복사 한다.
		
		\param pStr 비트맵을 만들 문자열
		\param nFontSize 문자열 크기
		\param pBuff 비트맵을 복사할 버퍼
        \param byBuffSize 비트맵을 복사할 버퍼의 단위 색상 버퍼의 바이트 사이즈
		\param rt 복사할 버퍼의 영역
		\param nTSize 버퍼(텍스처)의 가로 사이즈
		\param dwFlags 출력 옵션 플래그
					   D3DFONT_BOLD : 볼드체
					   D3DFONT_ITALIC : 이탤릭체
					   D3DFONT_SHADOW : 아웃라인(왜 아웃라인이 쉐도우인지 모름;; 초창기때 이렇게 지은듯)
					   D3DFONT_SHADOW_EX : 쉐도우
		\return 성공 유무
	 */
	bool CopyBitmap(const wchar_t* pStr, int nFontSize, void* pBuff, BYTE byBuffSize, RECT rt, int nTSize, DWORD dwFlags);

}; // end namespace

#endif