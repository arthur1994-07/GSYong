#ifndef CMemBitmap_H
#define CMemBitmap_H

/**
	더블 버퍼링을 처리 클래스
    MFC 의 CDC 와 CBitmap 을 사용하여 더블 버퍼링을 수행 한다.
	
 */
class CMemBitmap
{
public:
        /**
            클래스를 생성한다.

            \param pDC 최종 복사할 DC
            \param rect 복사할 영역
         */
		CMemBitmap(CDC* pDC, const CRect& rect);
virtual ~CMemBitmap();

		/**
			드로윙을 준비 한다.
			메모리 DC 와 비트맵 을 생성 한다.

		 */
		bool				BeginDraw(BOOL bAutoCreate = TRUE);

		/**
			생성자에서 입력된 DC 에 만든 메모리 DC 의 내용을
			복사 처리를 수행 한다.

		 */
		void				EndDraw();

		/**
			사용되는 메모리 DC 를 출력 한다.
			외부에서 이 메모리 DC 로 출력을 해주어야
			EndDraw 시에 정상적으로 출력 된다.

		 */
		CDC&				GetMemDC()											{ return m_dcMemory; }
		
		/**
			최종 DC 복사 시에 투명색을 빼고 처리할 수 있다.

		 */
		void				SetBitBltInfo(COLORREF clrTransparent);

		/**
			최종 DC 복사 시에 알파블렌드로 처리할 수 있다.

		 */
		void				SetBitBltInfo(BLENDFUNCTION sBlendFunction);

		/**
			최종 DC 에 복사를 수행 한다.

		 */
		void				BitBlt();

protected:

		// 기본 정보 변수
		CDC					m_dcMemory;
		CBitmap				m_bmp;
		CBitmap*			m_pBmpOld;
		CDC*				m_pDC;

		// BltBlt 기본 정보
		DWORD				m_dwType;
		DWORD				m_dwRop;
		CRect				m_rect;
		POINT				m_ptOldWndOrg;

		// BltBlt 타입별 정보
		union BitBltInfo
		{
			COLORREF		clrTransparent;
			BLENDFUNCTION	sBlendFunction;
		};

		BitBltInfo			m_sBitBltInfo;
};

#endif