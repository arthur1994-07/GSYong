#ifndef CMemBitmap_H
#define CMemBitmap_H

/**
	���� ���۸��� ó�� Ŭ����
    MFC �� CDC �� CBitmap �� ����Ͽ� ���� ���۸��� ���� �Ѵ�.
	
 */
class CMemBitmap
{
public:
        /**
            Ŭ������ �����Ѵ�.

            \param pDC ���� ������ DC
            \param rect ������ ����
         */
		CMemBitmap(CDC* pDC, const CRect& rect);
virtual ~CMemBitmap();

		/**
			������� �غ� �Ѵ�.
			�޸� DC �� ��Ʈ�� �� ���� �Ѵ�.

		 */
		bool				BeginDraw(BOOL bAutoCreate = TRUE);

		/**
			�����ڿ��� �Էµ� DC �� ���� �޸� DC �� ������
			���� ó���� ���� �Ѵ�.

		 */
		void				EndDraw();

		/**
			���Ǵ� �޸� DC �� ��� �Ѵ�.
			�ܺο��� �� �޸� DC �� ����� ���־��
			EndDraw �ÿ� ���������� ��� �ȴ�.

		 */
		CDC&				GetMemDC()											{ return m_dcMemory; }
		
		/**
			���� DC ���� �ÿ� ������� ���� ó���� �� �ִ�.

		 */
		void				SetBitBltInfo(COLORREF clrTransparent);

		/**
			���� DC ���� �ÿ� ���ĺ���� ó���� �� �ִ�.

		 */
		void				SetBitBltInfo(BLENDFUNCTION sBlendFunction);

		/**
			���� DC �� ���縦 ���� �Ѵ�.

		 */
		void				BitBlt();

protected:

		// �⺻ ���� ����
		CDC					m_dcMemory;
		CBitmap				m_bmp;
		CBitmap*			m_pBmpOld;
		CDC*				m_pDC;

		// BltBlt �⺻ ����
		DWORD				m_dwType;
		DWORD				m_dwRop;
		CRect				m_rect;
		POINT				m_ptOldWndOrg;

		// BltBlt Ÿ�Ժ� ����
		union BitBltInfo
		{
			COLORREF		clrTransparent;
			BLENDFUNCTION	sBlendFunction;
		};

		BitBltInfo			m_sBitBltInfo;
};

#endif